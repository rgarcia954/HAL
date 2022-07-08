/**
 * @file power_modes.c
 * @brief Hardware abstraction layer for the Power Modes
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */
#ifndef NON_SECURE
#include <hw.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* Linker Script sct file variables support */
#if defined(__ARMCC_VERSION)
#define __stack  Image$$ARM_LIB_STACK$$ZI$$Limit
#define __Wakeup_addr Image$$DRAM_WAKEUP_RSVD$$ZI$$Limit
#define app_ptr Image$$DRAM_WAKEUP_RSVD$$ZI$$Base
#endif

/* Exported variables from linker script
 * for RAM wakeup configuration with BOOT_CUSTOM,
 * used in _Sys_PowerModes_Sleep_MemoryRetention */
extern uint32_t app_ptr[];

/* Linker script exported stack pointer address,
 * use for wakeup from RAM address with BOOT_CUSTOM */
extern uint32_t __stack;

/* wakeup address location where wakeup configuration is written
 * for wakeup from RAM with BOOT_CUSTOM */
extern uint32_t __Wakeup_addr;

static volatile uint32_t g_sleep_dur;
static volatile uint32_t g_slp_period;

static uint32_t ACS_VCC_CTRL_backup;
static uint8_t vddc_trim_backup;
static uint8_t restore_trim = 0;

/* RAM region to save BLE hardware registers and RF front-end registers */
static uint32_t bb_registers_image[sizeof(BB_Type) / 4];
static uint32_t rf_registers_image_1[RF_REGISTERS_IMAGE_SIZE_BYTES / 4];
static uint32_t rf_registers_image_2[RF_REGISTERS_IMAGE_SIZE_BYTES / 4];

/* exported function to execute WFI statement for core in retention */
extern void __wfi_for_power_mode(void);

static void _Sys_PowerModes_Sleep_CoreRetention(sleep_mode_cfg *p_sleep_mode_cfg);

static void _Sys_PowerModes_Sleep_MemoryRetention(sleep_mode_cfg *p_sleep_mode_cfg);

static void _Sys_PowerModes_Sleep_NoRetention(sleep_mode_cfg *p_sleep_mode_cfg);

static void _Sys_PowerModes_Sleep_BLERegistersConfig_Enter(uint8_t dma_num);

static void _Sys_PowerModes_Sleep_BLERegistersConfig_Wakeup(uint8_t dma_num);

static void _Sys_PowerModes_Sleep_BLERegister_DMACopy(uint32_t src, uint32_t dest, uint32_t size, uint8_t dma_num);

static void _Sys_PowerModes_Sleep_WakeupInit(sleep_mode_cfg *p_sleep_mode_cfg);

static void _Sys_PowerModes_WakeupFromRAM(void) __attribute__((used)) __attribute__ ((section (".wakeup_section")));

static void _Sys_PowerModes_Sleep_ClockSetup(void);

static void _Sys_PowerModes_Standby_WakeupInit(standby_mode_cfg *p_standby_mode_cfg);

static void _Sys_PowerModes_Wakeup_ClockInit(clock_cfg *p_clock_cfg, uint32_t boot_cfg);

static uint32_t _Sys_PowerModes_CalculateCRC(void);


void Sys_PowerModes_Sleep_Init(sleep_mode_cfg *p_sleep_mode_cfg)
{
    /* Set wake-up configuration registers */
    ACS->WAKEUP_CFG = p_sleep_mode_cfg->wakeup_cfg;

    /* Configure boot settings */
    ACS->BOOT_CFG = p_sleep_mode_cfg->boot_cfg;
}

void Sys_PowerModes_Sleep_Enter(sleep_mode_cfg *p_sleep_mode_cfg, Sleep_Retention_t retention_type)
{
    if(p_sleep_mode_cfg->ble_present)
    {
        _Sys_PowerModes_Sleep_BLERegistersConfig_Enter(p_sleep_mode_cfg->DMA_channel_RF);
    }

    /* Enable global interrupts so that pending interrupts could be served before going to sleep
     * and disabling it before issuing sleep request */
    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();

    /* Disable all interrupts before switching the system clock to RC Clock so
     * that enabled NVIC interrupts are not served with 3MHz RC system clock */
    __disable_irq();

    /* Switch the system clock to 3MHz RC clock in order for Processor to enter Sleep */
    _Sys_PowerModes_Sleep_ClockSetup();

    if(SYSCTRL->RF_ACCESS_CFG & RF_ACCESS_ENABLE)
    {
        /* Disable XTAL oscillator */
        RF->XTAL_CTRL = (RF->XTAL_CTRL | XTAL_CTRL_DISABLE_OSCILLATOR);
    }

    /* RF Access Configuration */
    SYSCTRL->RF_ACCESS_CFG &= (~RF_ACCESS_ENABLE & ~RF_IRQ_ACCESS_ENABLE & ~BB_ACCESS_ENABLE);

    /* RF Power Configuration */
    SYSCTRL->RF_POWER_CFG &= (~RF_POWER_ENABLE & ~BB_POWER_ENABLE);

    /* RF Block Regulator Configuration */
    ACS->VDDPA_CTRL = VDDPA_INITIAL_TRIM_1P05V | VDDPA_SW_HIZ | VDDPA_ISENSE_DISABLE | VDDPA_DISABLE | VDDPA_TRIM_1P60V;

    /* Disable the VDDRF regulator */
    ACS->VDDRF_CTRL &= ~VDDRF_ENABLE;

    /* Enable pad retention */
    ACS_BOOT_CFG->PADS_RETENTION_EN_BYTE = PADS_RETENTION_ENABLE_BYTE;

    switch(retention_type)
    {
    case SLEEP_NO_RETENTION:
        _Sys_PowerModes_Sleep_NoRetention(p_sleep_mode_cfg);
        break;

    case SLEEP_MEM_RETENTION:
        _Sys_PowerModes_Sleep_MemoryRetention(p_sleep_mode_cfg);
        break;

    case SLEEP_CORE_RETENTION:
        _Sys_PowerModes_Sleep_CoreRetention(p_sleep_mode_cfg);
        break;

    default:
        break;
    }

    /* If the processor does not enter sleep due to pending wakeup event
     * or pending enabled NVIC interrupts,
     * restore the settings and enable interrupts */
    _Sys_PowerModes_Sleep_WakeupInit(p_sleep_mode_cfg);

    /* Enable NVIC interrupts */
    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();
}

static void _Sys_PowerModes_Sleep_CoreRetention(sleep_mode_cfg *p_sleep_mode_cfg)
{
    /* Set VDDC to 1.1 V, so that VDDCRET does not overdrive it,
     * since core is in retention, we can save the current VDDC trim
     * and load it on wakeup */
    /* Check if VDDC Voltage Trim is less than 1.1V */
    if(ACS_VDDC_CTRL->VTRIM_BYTE < VDDC_TRIM_1P10V_BYTE)
    {
        restore_trim = 1;
        vddc_trim_backup = ACS_VDDC_CTRL->VTRIM_BYTE;
        /* Check if VCC is less than 1.1V */
        if((ACS->VCC_CTRL & ACS_VCC_CTRL_VTRIM_Mask) < VCC_TRIM_1P10V)
        {
            ACS_VCC_CTRL_backup = ACS->VCC_CTRL;

            Sys_Trim_LoadDCDC(TRIM, TARGET_DCDC_1120);
        }

        ACS_VDDC_CTRL->VTRIM_BYTE = VDDC_TRIM_1P10V_BYTE;
    }


    /* Enable retention regulators with trimming values */
    ACS->VDDRET_CTRL = (p_sleep_mode_cfg->vddret_ctrl.vddm_ret_trim << ACS_VDDRET_CTRL_VDDMRET_VTRIM_Pos)    |
                        VDDMRET_ENABLE                                                                       |
                       (p_sleep_mode_cfg->vddret_ctrl.vddacs_ret_trim << ACS_VDDRET_CTRL_VDDACS_VTRIM_Pos)   |
                       (p_sleep_mode_cfg->vddret_ctrl.vddt_ret << ACS_VDDRET_CTRL_VDDTRET_ENABLE_Pos)        |
                       (p_sleep_mode_cfg->vddret_ctrl.vddc_ret_trim << ACS_VDDRET_CTRL_VDDCRET_VTRIM_Pos)    |
                        VDDCRET_ENABLE;

    /* Set the boot general purpose data to memory access configuration */
    ACS->BOOT_GP_DATA = SYSCTRL->MEM_ACCESS_CFG;

    /* Go to sleep */
    ACS->PWR_MODES_CTRL = PWR_SLEEP_MODE;

    /* Wait for interrupt */
    __wfi_for_power_mode();

    if(restore_trim)
    {
        /* Restore ACS_VCC_CTRL register after wakeup */
        ACS->VCC_CTRL = ACS_VCC_CTRL_backup;

        /* Restore the VDDC Trim values after wakeup */
        ACS_VDDC_CTRL->VTRIM_BYTE = vddc_trim_backup;

        restore_trim = 0;
    }
}

static void _Sys_PowerModes_Sleep_MemoryRetention(sleep_mode_cfg *p_sleep_mode_cfg)
{
    /* Enable retention regulators with trimming values */
    ACS->VDDRET_CTRL = (p_sleep_mode_cfg->vddret_ctrl.vddm_ret_trim << ACS_VDDRET_CTRL_VDDMRET_VTRIM_Pos)    |
                        VDDMRET_ENABLE                                                                       |
                       (p_sleep_mode_cfg->vddret_ctrl.vddacs_ret_trim << ACS_VDDRET_CTRL_VDDACS_VTRIM_Pos)   |
                       (p_sleep_mode_cfg->vddret_ctrl.vddt_ret << ACS_VDDRET_CTRL_VDDTRET_ENABLE_Pos)        |
                       (p_sleep_mode_cfg->vddret_ctrl.vddc_ret_trim << ACS_VDDRET_CTRL_VDDCRET_VTRIM_Pos)    |
                       VDDCRET_DISABLE;


    /* Configure wake up configuration and write configuration to specified location in RAM */
    app_ptr[0] = ((uint32_t)&__stack);                          /* Stack Pointer use the same as main application */
    app_ptr[1] = SCB->VTOR;                                     /* Vector table, use the same as main application */
    app_ptr[2] = ((uint32_t) &_Sys_PowerModes_WakeupFromRAM);   /* Wakeup address */
    app_ptr[3] = 0x0;                                           /* Reserved 0 */
    app_ptr[4] = 0x0;                                           /* Reserved 1 */
    app_ptr[5] = 0x0;                                           /* Reserved 2 */
    app_ptr[6] = 0x0;                                           /* Reserved 3 */
    app_ptr[7] = _Sys_PowerModes_CalculateCRC();                /* CRC */

    /* Set wakeup address to the location where wakeup configuration is written */
    SYSCTRL->WAKEUP_ADDR = ((uint32_t)&__Wakeup_addr);

    /* Set the boot general purpose data to memory access configuration */
    ACS->BOOT_GP_DATA = SYSCTRL->MEM_ACCESS_CFG;

    ACS->GP_DATA = (volatile uint32_t)p_sleep_mode_cfg;

    /* Go to sleep */
    ACS->PWR_MODES_CTRL = PWR_SLEEP_MODE;

    /* Wait for interrupt */
    __WFI();
}

static void _Sys_PowerModes_Sleep_NoRetention(sleep_mode_cfg *p_sleep_mode_cfg)
{
    /* Enable retention regulators with trimming values */
    ACS->VDDRET_CTRL = (p_sleep_mode_cfg->vddret_ctrl.vddm_ret_trim << ACS_VDDRET_CTRL_VDDMRET_VTRIM_Pos)    |
                        VDDMRET_DISABLE                                                                      |
                       (p_sleep_mode_cfg->vddret_ctrl.vddacs_ret_trim << ACS_VDDRET_CTRL_VDDACS_VTRIM_Pos)   |
                       (p_sleep_mode_cfg->vddret_ctrl.vddt_ret << ACS_VDDRET_CTRL_VDDTRET_ENABLE_Pos)        |
                       (p_sleep_mode_cfg->vddret_ctrl.vddc_ret_trim << ACS_VDDRET_CTRL_VDDCRET_VTRIM_Pos)    |
                        VDDCRET_DISABLE;
    /* Go to sleep */
    ACS->PWR_MODES_CTRL = PWR_SLEEP_MODE;

    /* Wait for interrupt */
    __WFI();
}

static void _Sys_PowerModes_WakeupFromRAM(void)
{
    /* Disable all interrupts */
    __disable_irq();

    sleep_mode_cfg *p_sleep_mode_cfg = NULL;

    p_sleep_mode_cfg = (sleep_mode_cfg*)ACS->GP_DATA;

    SYS_WATCHDOG_REFRESH();

    /* Enable the Wakeup interrupt */
    NVIC_EnableIRQ(WAKEUP_IRQn);

#if defined (__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 10U * 2U) | (3U << 11U * 2U)); /* enable CP10 Full Access */
#endif

    _Sys_PowerModes_Sleep_WakeupInit(p_sleep_mode_cfg);

    if(p_sleep_mode_cfg->ble_present)
    {
        /* Enable Interrupts */
        NVIC_EnableIRQ(BLE_HSLOT_IRQn);
        NVIC_EnableIRQ(BLE_SLP_IRQn);
        NVIC_EnableIRQ(BLE_FIFO_IRQn);
        NVIC_EnableIRQ(BLE_CRYPT_IRQn);
        NVIC_EnableIRQ(BLE_ERROR_IRQn);
        NVIC_EnableIRQ(BLE_TIMESTAMP_TGT1_IRQn);
        NVIC_EnableIRQ(BLE_FINETGT_IRQn);
        NVIC_EnableIRQ(BLE_TIMESTAMP_TGT2_IRQn);
        NVIC_EnableIRQ(BLE_SW_IRQn);
    }

    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();

    /* If the application return address is set */
    if(p_sleep_mode_cfg->app_addr)
    {
        p_sleep_mode_cfg->app_addr();
    }
    else
    {
        /* We should not reach here as application return address is not set
         * by the application */
    	SYS_WATCHDOG_REFRESH();
        while(1)
        {

        }
    }
}

void Sys_PowerModes_Wakeup_WithReset(sleep_mode_cfg *p_sleep_mode_cfg)
{
    SYS_WATCHDOG_REFRESH();

   /*  To enable the clock detector, it is recommended to:
    *    1. Enable the clock detector with ACS_CLK_DET_CTRL.ENABLE bit.
    *    2. Check the flag ACS_CLK_DET_CTRL.CLOCK_PRESENT and wait until high. It is the time needed for the clock detector to be set-up.
    *    3. Clear the ACS_CLK_DET_CTRL.RESET_IGNORE bit, to accept the reset if the clock is missing. */
    ACS->CLK_DET_CTRL |= ((uint32_t)(0x1U << ACS_CLK_DET_CTRL_ENABLE_Pos));
    while(!(ACS->CLK_DET_CTRL & (0x1U << ACS_CLK_DET_CTRL_CLOCK_PRESENT_Pos)));
    ACS->CLK_DET_CTRL &= ~((uint32_t)(0x1U << ACS_CLK_DET_CTRL_RESET_IGNORE_Pos));

    if(p_sleep_mode_cfg->app_gpio_config)
    {
        p_sleep_mode_cfg->app_gpio_config();
    }

    /* Disable pad retention */
    ACS_BOOT_CFG->PADS_RETENTION_EN_BYTE = PADS_RETENTION_DISABLE_BYTE;

    /* Clear reset flags */
    RESET->DIG_STATUS = (ACS_RESET_FLAG_CLEAR | CM33_SW_RESET_FLAG_CLEAR |
                         WATCHDOG_RESET_FLAG_CLEAR | LOCKUP_FLAG_CLEAR |
                         DEU_RESET_FLAG_CLEAR);

    ACS->RESET_STATUS = (POR_RESET_FLAG_CLEAR | PAD_RESET_FLAG_CLEAR | BG_VREF_RESET_FLAG_CLEAR |
                         VDDC_RESET_FLAG_CLEAR | VDDM_RESET_FLAG_CLEAR | VDDFLASH_RESET_FLAG_CLEAR |
                         CLK_DET_RESET_FLAG_CLEAR | TIMEOUT_RESET_FLAG_CLEAR | WRONG_STATE_RESET_FLAG_CLEAR |
#ifdef MONTANA_CID
						 SOC_WDG_RESET_FLAG_CLEAR);
#else
                         CCAO_REBOOT_RESET_FLAG_CLEAR);
#endif

    /* Re-configure clock after wakeup */
    _Sys_PowerModes_Wakeup_ClockInit(&p_sleep_mode_cfg->clock_cfg, p_sleep_mode_cfg->boot_cfg);

    /* Set the NVIC WAKEUP_IRQn in case of sleep with reset */
    if(ACS->WAKEUP_CTRL)
    {
        if(!NVIC_GetPendingIRQ(WAKEUP_IRQn))
        {
            NVIC_SetPendingIRQ(WAKEUP_IRQn);
        }
    }

    /* Enable the Wakeup interrupt */
    NVIC_EnableIRQ(WAKEUP_IRQn);

    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();
}

void Sys_PowerModes_Standby_Init(standby_mode_cfg *p_standby_mode_cfg)
{
    /* Set wake-up configuration registers */
    ACS->WAKEUP_CFG = p_standby_mode_cfg->wakeup_cfg;

    /* Configure boot settings */
    ACS->BOOT_CFG = p_standby_mode_cfg->boot_cfg;
}

void Sys_PowerModes_Standby_Enter(standby_mode_cfg *p_standby_mode_cfg)
{
    if(p_standby_mode_cfg->ble_present)
    {
        _Sys_PowerModes_Sleep_BLERegistersConfig_Enter(p_standby_mode_cfg->DMA_channel_RF);
    }

    /* Enable global interrupts so that pending interrupts could be served before going to sleep
     * and disabling it before issuing sleep request */
    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();

    /* Disable all interrupts before switching the system clock to RC Clock so
     * that enabled NVIC interrupts are not served with 3MHz RC system clock */
    __disable_irq();

    /* Switch the system clock to 3MHz RC clock in order for Processor to enter Sleep */
    _Sys_PowerModes_Sleep_ClockSetup();

    if(SYSCTRL->RF_ACCESS_CFG & RF_ACCESS_ENABLE)
    {
        /* Disable XTAL oscillator */
        RF->XTAL_CTRL = (RF->XTAL_CTRL | XTAL_CTRL_DISABLE_OSCILLATOR);
    }

    /* RF Access Configuration */
    SYSCTRL->RF_ACCESS_CFG &= (~RF_ACCESS_ENABLE & ~RF_IRQ_ACCESS_ENABLE & ~BB_ACCESS_ENABLE);

    /* RF Power Configuration */
    SYSCTRL->RF_POWER_CFG &= (~RF_POWER_ENABLE & ~BB_POWER_ENABLE);

    /* RF Block Regulator Configuration */
    ACS->VDDPA_CTRL = VDDPA_INITIAL_TRIM_1P05V | VDDPA_SW_HIZ | VDDPA_ISENSE_DISABLE | VDDPA_DISABLE | VDDPA_TRIM_1P60V;

    /* Disable the VDDRF regulator */
    ACS->VDDRF_CTRL &= ~VDDRF_ENABLE;

    /* Enable retention regulators with trimming values */
    ACS_VDDC_CTRL->STANDBY_VTRIM_BYTE = p_standby_mode_cfg->vdd_standby_trim.vddc_standby_trim;

    ACS_VDDM_CTRL->STANDBY_VTRIM_BYTE = p_standby_mode_cfg->vdd_standby_trim.vddm_standby_trim;

    /* Enable pad retention */
    ACS_BOOT_CFG->PADS_RETENTION_EN_BYTE = PADS_RETENTION_ENABLE_BYTE;

    /* Set the boot general purpose data to memory access configuration */
    ACS->BOOT_GP_DATA = SYSCTRL->MEM_ACCESS_CFG;

    /* Go to sleep */
    ACS->PWR_MODES_CTRL = PWR_STANDBY_MODE;

    /* Wait for interrupt */
    __wfi_for_power_mode();

    /* If the processor does not enter sleep due to pending wakeup event
     * or pending enabled NVIC interrupts,
     * restore the settings and enable interrupts */
    _Sys_PowerModes_Standby_WakeupInit(p_standby_mode_cfg);

    /* Enable NVIC interrupts */
    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();
}

void Sys_PowerModes_DeepSleep_Init(deepsleep_mode_cfg *p_deepsleep_mode_cfg)
{
    /* Set wake-up configuration registers */
    ACS->WAKEUP_CFG = p_deepsleep_mode_cfg->wakeup_cfg;

    /* Configure boot settings */
    ACS->BOOT_CFG = p_deepsleep_mode_cfg->boot_cfg;
}

void Sys_PowerModes_DeepSleep_Enter(deepsleep_mode_cfg *p_deepsleep_mode_cfg)
{
    /* Enable global interrupts so that pending interrupts could be served before going to sleep
     * and disabling it before issuing sleep request */
    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();

    /* Disable all interrupts before switching the system clock to RC Clock so
     * that enabled NVIC interrupts are not served with 3MHz RC system clock */
    __disable_irq();

    /* Switch the system clock to 3MHz RC clock in order for Processor to enter Sleep */
    _Sys_PowerModes_Sleep_ClockSetup();

    /* Enable pad retention */
    ACS_BOOT_CFG->PADS_RETENTION_EN_BYTE = PADS_RETENTION_ENABLE_BYTE;

    /* Go to sleep */
    ACS->PWR_MODES_CTRL = PWR_DEEP_SLEEP_MODE;

    /* Wait for interrupt */
    __WFI();

    /* If the processor does not enter sleep due to pending wakeup event
     * or pending enabled NVIC interrupts,
     * restore the settings and enable interrupts */
    Sys_PowerModes_Wakeup_WithReset((sleep_mode_cfg*)p_deepsleep_mode_cfg);

    /* Enable NVIC interrupts */
    __enable_irq();

    /* Allow process pending interrupts if there are any */
    __ISB();
}

void Sys_PowerModes_Wakeup_ConfigEnable(uint32_t acs_wakeup_cfg, uint32_t *p_wakeup_cfg)
{
    *p_wakeup_cfg = *p_wakeup_cfg | acs_wakeup_cfg;

    ACS->WAKEUP_CFG = *p_wakeup_cfg;
}

void Sys_PowerModes_Wakeup_ConfigDisable(uint32_t acs_wakeup_cfg, uint32_t *p_wakeup_cfg)
{
    *p_wakeup_cfg = *p_wakeup_cfg & ~acs_wakeup_cfg;

    ACS->WAKEUP_CFG = *p_wakeup_cfg;
}

static void _Sys_PowerModes_Sleep_BLERegistersConfig_Enter(uint8_t dma_num)
{
    /* 2Mbps config setting */
    RF0_REG08->BANK_BYTE = 0x01;
    _Sys_PowerModes_Sleep_BLERegister_DMACopy((uint32_t)RF_BASE,
                                              (uint32_t)rf_registers_image_2,
                                              RF_REGISTERS_IMAGE_SIZE_BYTES/4,
                                              dma_num);

    /* 1Mbps config setting */
    RF0_REG08->BANK_BYTE = 0x00;
    _Sys_PowerModes_Sleep_BLERegister_DMACopy((uint32_t)RF_BASE,
                                              (uint32_t)rf_registers_image_1,
                                              RF_REGISTERS_IMAGE_SIZE_BYTES/4,
                                              dma_num);

    /* Wait until the baseband clock is switch to low power clock */
    SYS_WATCHDOG_REFRESH();
    while((BBIF->STATUS & LOW_POWER_CLK) == MASTER_CLK)
    {

    }

    _Sys_PowerModes_Sleep_BLERegister_DMACopy((uint32_t)BB_BASE, (uint32_t)bb_registers_image, (sizeof(BB_Type)/4), dma_num);

    bb_registers_image[(BB_DEEPSLCNTL_BASE - BB_BASE)/4] = DEEP_SLEEP_ON_0 |
                                                           OSC_SLEEP_EN_0  |
                                                           RADIO_SLEEP_EN_0;

    /* Wait until the status bits of the OSC_EN register are reset
     * indicating that the baseband low power timer is in deep sleep
     * mode and properly isolated.*/
    SYS_WATCHDOG_REFRESH();
    while((BBIF->STATUS & OSC_ENABLED) == OSC_ENABLED)
    {

    }
}

static void _Sys_PowerModes_Sleep_BLERegistersConfig_Wakeup(uint8_t dma_num)
{
    /* 2Mbps config setting */
    RF0_REG08->BANK_BYTE = 0x01;
    _Sys_PowerModes_Sleep_BLERegister_DMACopy((uint32_t)rf_registers_image_2,
                                              (uint32_t)RF_BASE,
                                              RF_REGISTERS_IMAGE_SIZE_BYTES/4,
                                              dma_num);

    /* 1Mbps config setting */
    RF0_REG08->BANK_BYTE = 0x00;

    _Sys_PowerModes_Sleep_BLERegister_DMACopy((uint32_t)rf_registers_image_1,
                                              (uint32_t)RF_BASE,
                                              RF_REGISTERS_IMAGE_SIZE_BYTES/4,
                                              dma_num);

    _Sys_PowerModes_Sleep_BLERegister_DMACopy((uint32_t)bb_registers_image,
                                              (uint32_t)BB_BASE,
                                              (sizeof(BB_Type)/4),
                                              dma_num);
}

static void _Sys_PowerModes_Sleep_BLERegister_DMACopy(uint32_t src, uint32_t dest, uint32_t size, uint8_t dma_num)
{
    DMA[dma_num].STATUS = DMA_COMPLETE_INT_CLEAR;
    DMA[dma_num].CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;

    Sys_DMA_ChannelConfig(&DMA[dma_num],
                          (DMA_SRC_ADDR_INCR_1 | DMA_DEST_ADDR_INCR_1 |
                           WORD_SIZE_32BITS_TO_32BITS | DMA_COMPLETE_INT_ENABLE),
                           size,
                           0,
                          (uint32_t)src,
                          (uint32_t)dest);

    Sys_DMA_Mode_Enable(&DMA[dma_num], DMA_ENABLE);

    while(((DMA[dma_num].STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_FALSE));

    Sys_DMA_Mode_Enable(&DMA[dma_num], DMA_DISABLE);

    NVIC_ClearPendingIRQ(DMA0_IRQn + dma_num);
}

static void _Sys_PowerModes_Sleep_WakeupInit(sleep_mode_cfg *p_sleep_mode_cfg)
{
    SYS_WATCHDOG_REFRESH();

   /*  To enable the clock detector, it is recommended to:
    *    1. Enable the clock detector with ACS_CLK_DET_CTRL.ENABLE bit.
    *    2. Check the flag ACS_CLK_DET_CTRL.CLOCK_PRESENT and wait until high. It is the time needed for the clock detector to be set-up.
    *    3. Clear the ACS_CLK_DET_CTRL.RESET_IGNORE bit, to accept the reset if the clock is missing. */
    ACS->CLK_DET_CTRL |= ((uint32_t)(0x1U << ACS_CLK_DET_CTRL_ENABLE_Pos));
    while(!(ACS->CLK_DET_CTRL & (0x1U << ACS_CLK_DET_CTRL_CLOCK_PRESENT_Pos)));
    ACS->CLK_DET_CTRL &= ~((uint32_t)(0x1U << ACS_CLK_DET_CTRL_RESET_IGNORE_Pos));

    if(p_sleep_mode_cfg->app_gpio_config)
    {
        p_sleep_mode_cfg->app_gpio_config();
    }

    /* Disable pad retention */
    ACS_BOOT_CFG->PADS_RETENTION_EN_BYTE = PADS_RETENTION_DISABLE_BYTE;

    /* Clear reset flags */
    RESET->DIG_STATUS = (ACS_RESET_FLAG_CLEAR | CM33_SW_RESET_FLAG_CLEAR |
                         WATCHDOG_RESET_FLAG_CLEAR | LOCKUP_FLAG_CLEAR |
                         DEU_RESET_FLAG_CLEAR);

    ACS->RESET_STATUS = (POR_RESET_FLAG_CLEAR | PAD_RESET_FLAG_CLEAR | BG_VREF_RESET_FLAG_CLEAR |
                         VDDC_RESET_FLAG_CLEAR | VDDM_RESET_FLAG_CLEAR | VDDFLASH_RESET_FLAG_CLEAR |
                         CLK_DET_RESET_FLAG_CLEAR | TIMEOUT_RESET_FLAG_CLEAR | WRONG_STATE_RESET_FLAG_CLEAR |
#ifdef MONTANA_CID
						 SOC_WDG_RESET_FLAG_CLEAR);
#else
                         CCAO_REBOOT_RESET_FLAG_CLEAR);
#endif

    /* Re-configure clock after wakeup */
    _Sys_PowerModes_Wakeup_ClockInit(&p_sleep_mode_cfg->clock_cfg, p_sleep_mode_cfg->boot_cfg);

    if(p_sleep_mode_cfg->ble_present)
    {
        _Sys_PowerModes_Sleep_BLERegistersConfig_Wakeup(p_sleep_mode_cfg->DMA_channel_RF);
    }

    if((ACS->VDDRET_CTRL & VDDTRET_ENABLE) && p_sleep_mode_cfg->ble_present)
    {
        BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8 | BB_DEEP_SLEEP);

        /* Wakeup the BB timer intentionally */
        BBIF->CTRL |= (BB_WAKEUP);

        Sys_Delay((SystemCoreClock / (32768)) * 2);

        SYS_WATCHDOG_REFRESH();
        while((BBIF->STATUS & LOW_POWER_CLK) != MASTER_CLK)
        {

        }

        BBIF->CTRL &= (~BB_WAKEUP);

        /* Clear the BB Timer sticky flag */
        ACS->WAKEUP_CTRL |= WAKEUP_BB_TIMER_CLEAR;
    }
}

static void _Sys_PowerModes_Standby_WakeupInit(standby_mode_cfg *p_standby_mode_cfg)
{
    SYS_WATCHDOG_REFRESH();

   /*  To enable the clock detector, it is recommended to:
    *    1. Enable the clock detector with ACS_CLK_DET_CTRL.ENABLE bit.
    *    2. Check the flag ACS_CLK_DET_CTRL.CLOCK_PRESENT and wait until high. It is the time needed for the clock detector to be set-up.
    *    3. Clear the ACS_CLK_DET_CTRL.RESET_IGNORE bit, to accept the reset if the clock is missing. */
    ACS->CLK_DET_CTRL |= ((uint32_t)(0x1U << ACS_CLK_DET_CTRL_ENABLE_Pos));
    while(!(ACS->CLK_DET_CTRL & (0x1U << ACS_CLK_DET_CTRL_CLOCK_PRESENT_Pos)));
    ACS->CLK_DET_CTRL &= ~((uint32_t)(0x1U << ACS_CLK_DET_CTRL_RESET_IGNORE_Pos));

    if(p_standby_mode_cfg->app_gpio_config)
    {
        p_standby_mode_cfg->app_gpio_config();
    }

    /* Disable pad retention */
    ACS_BOOT_CFG->PADS_RETENTION_EN_BYTE = PADS_RETENTION_DISABLE_BYTE;

    /* Clear reset flags */
    RESET->DIG_STATUS = (ACS_RESET_FLAG_CLEAR | CM33_SW_RESET_FLAG_CLEAR |
                         WATCHDOG_RESET_FLAG_CLEAR | LOCKUP_FLAG_CLEAR |
                         DEU_RESET_FLAG_CLEAR);

    ACS->RESET_STATUS = (POR_RESET_FLAG_CLEAR | PAD_RESET_FLAG_CLEAR | BG_VREF_RESET_FLAG_CLEAR |
                         VDDC_RESET_FLAG_CLEAR | VDDM_RESET_FLAG_CLEAR | VDDFLASH_RESET_FLAG_CLEAR |
                         CLK_DET_RESET_FLAG_CLEAR | TIMEOUT_RESET_FLAG_CLEAR | WRONG_STATE_RESET_FLAG_CLEAR |
#ifdef MONTANA_CID
						 SOC_WDG_RESET_FLAG_CLEAR);
#else
                         CCAO_REBOOT_RESET_FLAG_CLEAR);
#endif

    /* Re-configure clock after wakeup */
    _Sys_PowerModes_Wakeup_ClockInit(&p_standby_mode_cfg->clock_cfg, p_standby_mode_cfg->boot_cfg);

    if(p_standby_mode_cfg->ble_present)
    {
        _Sys_PowerModes_Sleep_BLERegistersConfig_Wakeup(p_standby_mode_cfg->DMA_channel_RF);
    }

    if(p_standby_mode_cfg->ble_present)
    {
        BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8 | BB_DEEP_SLEEP);

        /* Wakeup the BB timer intentionally */
        BBIF->CTRL |= (BB_WAKEUP);

        Sys_Delay((SystemCoreClock / (32768)) * 2);

        SYS_WATCHDOG_REFRESH();
        while((BBIF->STATUS & LOW_POWER_CLK) != MASTER_CLK)
        {

        }

        BBIF->CTRL &= (~BB_WAKEUP);

        /* Clear the BB Timer sticky flag */
        ACS->WAKEUP_CTRL |= WAKEUP_BB_TIMER_CLEAR;
    }
}

static void _Sys_PowerModes_Sleep_ClockSetup(void)
{
    /* Set RC OSC to 3MHz */
    ACS->RCOSC_CTRL = (ACS->RCOSC_CTRL & ~(0x3U << ACS_RCOSC_CTRL_RC_FSEL_Pos)) | RC_OSC_ENABLE;

    /* Switch to internal RC oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);
}

void Sys_PowerModes_XTAL_ClockInit(clock_cfg *p_clock_cfg)
{
	uint8_t systemclk_div;

	systemclk_div = (RFCLK_BASE_FREQ / p_clock_cfg->systemclk_freq);

	/* Boundary condition check for the system clock division */
	/* If the prescale is greater than CK_DIV_1_6_PRESCALE_7_BYTE set it to
	 * CK_DIV_1_6_PRESCALE_7_BYTE to avoid system clock being an unknown value */
	if (systemclk_div > CK_DIV_1_6_PRESCALE_7_BYTE)
	{
		systemclk_div = CK_DIV_1_6_PRESCALE_7_BYTE;
	}

	/* If the system clock prescale is less than CK_DIV_1_6_PRESCALE_1_BYTE set it to
	 * CK_DIV_1_6_PRESCALE_1_BYTE to avoid system clock being an unknown value */
	else if(systemclk_div < CK_DIV_1_6_PRESCALE_1_BYTE)
	{
		systemclk_div = CK_DIV_1_6_PRESCALE_1_BYTE;
	}

    /* Start 48 MHz XTAL oscillator */
    Sys_Clocks_XTALClkConfig(systemclk_div);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Call the Divider Config after the SystemCoreClock global variable is updated */
    Sys_Clocks_DividerConfig(p_clock_cfg->uartclk_freq,
            p_clock_cfg->sensorclk_freq,
            p_clock_cfg->userclk_freq);
}

static void _Sys_PowerModes_Wakeup_ClockInit(clock_cfg *p_clock_cfg, uint32_t boot_cfg)
{
    /* If XTAL is not enabled by ROM */
    if (((boot_cfg & ACS_BOOT_CFG_BOOT_SELECT_Mask) == BOOT_CUSTOM) ||
        ((boot_cfg & ACS_BOOT_CFG_BOOT_SELECT_Mask) == BOOT_FLASH_XTAL_DEFAULT_TRIM) ||
        ((boot_cfg & ACS_BOOT_CFG_BOOT_SELECT_Mask) == BOOT_FLASH_XTAL_CUSTOM_TRIM))
    {
    	Sys_PowerModes_XTAL_ClockInit(p_clock_cfg);
    }
    else /* ((boot_cfg & ACS_BOOT_CFG_BOOT_SELECT_Mask) == BOOT_FLASH_XTAL_DISABLE) */
    {
        /* Load the RC OSC Trim value */
        uint32_t ret_val = Sys_Trim_LoadRCOSC(TRIM, p_clock_cfg->systemclk_freq/1000);

        if(ret_val == ERROR_NO_ERROR)
        {
        	switch(p_clock_cfg->systemclk_freq/1000)
        	{
        	case TARGET_RC3:
        		/* Set the Flash delay for 3MHz system clock */
        		FLASH->DELAY_CTRL = (FLASH->DELAY_CTRL & ~FLASH_DELAY_CTRL_SYSCLK_FREQ_Mask ) | FLASH_DELAY_FOR_SYSCLK_3MHZ;

        		/* Set RC OSC to 3MHz */
        		ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
        		ACS->RCOSC_CTRL |= (RC_OSC_3MHZ | RC_OSC_ENABLE);
        		break;

        	case TARGET_RC12:
        		/* Set the Flash delay for 12MHz system clock */
        		FLASH->DELAY_CTRL = (FLASH->DELAY_CTRL & ~FLASH_DELAY_CTRL_SYSCLK_FREQ_Mask ) | FLASH_DELAY_FOR_SYSCLK_12MHZ;

        		/* Set RC OSC to 12MHz */
        		ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
        		ACS->RCOSC_CTRL |= (RC_OSC_12MHZ | RC_OSC_ENABLE);
        		break;

        	case TARGET_RC24:
        		/* Set the Flash delay for 24MHz system clock */
        		FLASH->DELAY_CTRL = (FLASH->DELAY_CTRL & ~FLASH_DELAY_CTRL_SYSCLK_FREQ_Mask ) | FLASH_DELAY_FOR_SYSCLK_24MHZ;

        		/* Set RC OSC to 24MHz */
        		ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
        		ACS->RCOSC_CTRL |= (RC_OSC_24MHZ | RC_OSC_ENABLE);
        		break;

        	case TARGET_RC48:
        		/* Set the Flash delay for 48MHz system clock */
        		FLASH->DELAY_CTRL = (FLASH->DELAY_CTRL & ~FLASH_DELAY_CTRL_SYSCLK_FREQ_Mask ) | FLASH_DELAY_FOR_SYSCLK_48MHZ;

        		/* Set RC OSC to 48MHz */
        		ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
        		ACS->RCOSC_CTRL |= (RC_OSC_48MHZ | RC_OSC_ENABLE);
        		break;
        	}
        }
        else
        {
        	/* Load the RC OSC Trim value */
        	Sys_Trim_LoadRCOSC(TRIM, TARGET_RC12);

        	/* Set the Flash delay for 12MHz system clock */
        	FLASH->DELAY_CTRL = (FLASH->DELAY_CTRL & ~FLASH_DELAY_CTRL_SYSCLK_FREQ_Mask ) | FLASH_DELAY_FOR_SYSCLK_12MHZ;

        	/* Set RC OSC to 12MHz */
        	ACS->RCOSC_CTRL &= ~ACS_RCOSC_CTRL_RC_FSEL_Mask;
        	ACS->RCOSC_CTRL |= (RC_OSC_12MHZ | RC_OSC_ENABLE);
        }

        /* Switch the system clock to internal RC Clock */
        Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RCCLK);

        /* Call the Divider Config after the SystemCoreClock global variable is updated */
        Sys_Clocks_DividerConfig(p_clock_cfg->uartclk_freq,
        		p_clock_cfg->sensorclk_freq,
				p_clock_cfg->userclk_freq);
    }
}

static uint32_t _Sys_PowerModes_CalculateCRC(void)
{
    CRC_Type *crc = CRC;
    Sys_Set_CRC_Config(crc, CRC_32 | CRC_BIG_ENDIAN | CRC_BIT_ORDER_STANDARD);
    Sys_CRC_32InitValue(crc);

    uint32_t *cptr = (uint32_t *)((uint32_t)&__Wakeup_addr);
    for (uint32_t i = 0; i < 7; i++)
    {
        Sys_CRC_Add(crc, *cptr, 32);
        cptr += 1;
    }
    return Sys_CRC_GetFinalValue(crc);
}
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */
#endif
