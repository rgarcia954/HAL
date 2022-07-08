/**
 * @file power_modes.h
 * @brief header file for power mode API
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

#ifndef POWER_MODES_H
#define POWER_MODES_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALPOWERMODES HAL Power Modes
 *
 *  @{
 */

/** Macro for enabling BLE stack RF registers copy during
 * sleep/wakeup, used in  sleep_mode_cfg, or standby_mode_cfg, or
 * deepsleep_mode_cfg */
#define BLE_PRESENT                         1

/** Macro for disabling BLE stack RF registers copy during
 * sleep/wakeup, used in  sleep_mode_cfg, or standby_mode_cfg, or
 * deepsleep_mode_cfg */
#define BLE_NOT_PRESENT                     0

/** Size of RF registers in bytes */
#define RF_REGISTERS_IMAGE_SIZE_BYTES       0xf8

/** VDDM retention minimum trim value */
#define VDDMRETENTION_TRIM_MINIMUM          0x0U

/** VDDM retention maximum trim value */
#define VDDMRETENTION_TRIM_MAXIMUM          0x3U

/** VDDT baseband retention regulator disable macro */
#define VDDTRETENTION_DISABLE               0x0U

/** VDDT baseband retention regulator enable macro */
#define VDDTRETENTION_ENABLE                0x1U

/** VDDC retention minimum trim value */
#define VDDCRETENTION_TRIM_MINIMUM          0x0U

/** VDDC retention maximum trim value */
#define VDDCRETENTION_TRIM_MAXIMUM          0x3U

/** VDDACS retention maximum trim value */
#define VDDACSRETENTION_TRIM_MINIMUM        0x0U

/** VDDACS retention maximum trim value */
#define VDDACSRETENTION_TRIM_MAXIMUM        0x3U

/**
 * @brief enum for sleep mode with retention type
 *
 */
typedef enum
{
    SLEEP_NO_RETENTION,     /**< Sleep with no retention */
    SLEEP_MEM_RETENTION,    /**< Sleep with memory retention only */
    SLEEP_CORE_RETENTION    /**< Sleep with Core retention */
}Sleep_Retention_t;

/** Function pointer for application return address in BOOT_CUSTOM boot config */
typedef void (*p_application_return)(void);

/** Function pointer to configure GPIO after wakeup from sleep */
typedef void (*p_appplication_gpio_config)(void);

/**
 * @brief sleep mode retention regulator configuration
 *
 */
typedef struct sleep_mode_ret_regulator_cfg_t
{
    uint8_t vddm_ret_trim:2;    /**< vddm retention trimming value; use
                                     0x00 to 0x03 */
    uint8_t vddt_ret:1;         /**< vddt baseband timer regulator retention; use
                                     VDDTRETENTION_ENABLE,
                                     VDDTRETENTION_DISABLE */
    uint8_t vddacs_ret_trim:2;  /**< vddacs retention trimming value; use
                                     0x00 to 0x03 */
    uint8_t vddc_ret_trim:2;    /**< vddc retention trimming value; use
                                     0x00 to 0x03 */
} sleep_mode_ret_regulator_cfg;

/**
 * @brief standby regulator trim value
 */
typedef struct standby_ret_trim_cfg_t
{
    uint8_t vddc_standby_trim;  /**< vddc standby retention trimming value; use
                                     0x00 to 0x3F */
    uint8_t vddm_standby_trim;  /**< vddm standby retention trimming value; use
                                     0x00 to 0x3F */
}standby_ret_trim_cfg;

/**
 * @brief clock configuration for Power RUN mode
 */
typedef struct clock_cfg_t
{
    uint32_t systemclk_freq;    /**< System clock frequency value */
    uint32_t uartclk_freq;      /**< UART clock frequency derived from system clock */
    uint32_t sensorclk_freq;    /**< Sensor clock frequency value */
    uint32_t userclk_freq;      /**< user clock frequency */
} clock_cfg;


/**
 * @brief sleep mode configuration
 */
typedef struct sleep_mode_cfg_t
{

    uint32_t wakeup_cfg;                        /**< ACS Wake up configuration; use
                                                     WAKEUP_DELAY_[1 | 2 | 4 | ... | 128],
                                                     WAKEUP_DCDC_OVERLOAD_[ENABLE | DISABLE],
                                                     WAKEUP_GPIO*_[RISING | FALLING],
                                                     WAKEUP_GPIO*_[ENABLE | DISABLE],
                                                     WAKEUP_FIFO_[ENABLE | DISABLE],
                                                     WAKEUP_RTC_OVERFLOW_[ENABLE | DISABLE]*/

    uint32_t boot_cfg;                          /**< Boot Configuration; use
                                                     BOOT_FLASH_XTAL_DISABLE,
                                                     BOOT_CUSTOM,
                                                     BOOT_FLASH_XTAL_DEFAULT_TRIM,
                                                     BOOT_FLASH_XTAL_CUSTOM_TRIM */

    clock_cfg clock_cfg;                        /**< clock configuration */


    p_appplication_gpio_config app_gpio_config; /**< pointer to function for application GPIO
                                                     Configuration after wakeup */


    uint8_t DMA_channel_RF;                     /**< DMA channel used to save/restore RF registers
                                                     in each sleep/wake-up cycle */


    uint8_t ble_present;                        /**< ble stack present flag */


    sleep_mode_ret_regulator_cfg vddret_ctrl;   /**< VDD Retention regulator configuration */


    p_application_return app_addr;              /**< pointer to function for application resume address
                                                     for BOOT_CUSTOM wakeup configuration*/

} sleep_mode_cfg;

/**
 * @brief standby mode configuration
 */
typedef struct standby_mode_cfg_t
{
    uint32_t wakeup_cfg;                        /**< ACS Wake up configuration; use
                                                     WAKEUP_DELAY_[1 | 2 | 4 | ... | 128],
                                                     WAKEUP_DCDC_OVERLOAD_[ENABLE | DISABLE],
                                                     WAKEUP_GPIO*_[RISING | FALLING],
                                                     WAKEUP_GPIO*_[ENABLE | DISABLE],
                                                     WAKEUP_FIFO_[ENABLE | DISABLE],
                                                     WAKEUP_RTC_OVERFLOW_[ENABLE | DISABLE]*/

    uint32_t boot_cfg;                          /**< Boot Configuration; use
                                                     BOOT_FLASH_XTAL_DISABLE,
                                                     BOOT_CUSTOM,
                                                     BOOT_FLASH_XTAL_DEFAULT_TRIM,
                                                     BOOT_FLASH_XTAL_CUSTOM_TRIM */

    clock_cfg clock_cfg;                        /**< clock configuration */

    p_appplication_gpio_config app_gpio_config; /**< pointer to function for application GPIO
                                                     Configuration after wakeup */


    uint8_t DMA_channel_RF;                     /**< DMA channel used to save/restore RF registers
                                                     in each sleep/wake-up cycle */


    uint8_t ble_present;                        /**< ble stack present flag */


    standby_ret_trim_cfg vdd_standby_trim;      /**< standby Retention regulator trim */

} standby_mode_cfg;

/**
 * @brief deep sleep mode configuration
 */
typedef struct deepsleep_mode_cfg_t
{
    uint32_t wakeup_cfg;                        /**< ACS Wake up configuration; use
                                                     WAKEUP_DELAY_[1 | 2 | 4 | ... | 128],
                                                     WAKEUP_GPIO*_[RISING | FALLING],
                                                     WAKEUP_GPIO*_[ENABLE | DISABLE] */

    uint32_t boot_cfg;                          /**< Boot Configuration; use
                                                     BOOT_FLASH_XTAL_DISABLE,
                                                     BOOT_FLASH_XTAL_DEFAULT_TRIM,
                                                     BOOT_FLASH_XTAL_CUSTOM_TRIM */

    clock_cfg clock_cfg;                        /**< clock configuration */

    p_appplication_gpio_config app_gpio_config; /**< pointer to function for application GPIO
                                                     Configuration after wakeup */

} deepsleep_mode_cfg;

/**
 * @brief Initialize the Sleep Configuration
 *
 * @param[in] p_sleep_mode_cfg  pointer to the sleep_mode_cfg instance
 * @note      This function will modify ACS->WAKEUP_CFG and ACS->BOOT_CFG
 * @examplecode HAL_examples.c  Sys_PowerModes_Sleep_Init_Example
 */
void Sys_PowerModes_Sleep_Init(sleep_mode_cfg *p_sleep_mode_cfg);

/**
 * @brief Enter Sleep Mode with the retention_type as specified by the input
 *        parameter
 *
 * @param[in] p_sleep_mode_cfg  pointer to the sleep_mode_cfg instance
 * @param[in] retention_type    retention type for sleep mode; use SLEEP_NO_RETENTION,
 *                              SLEEP_MEM_RETENTION, SLEEP_CORE_RETENTION
 * @note  This function will enter sleep mode if there are no wakeup events
 *        or NVIC interrupts pending, otherwise it will continue to RUN mode and service
 *        pending events and interrupts
 * @examplecode HAL_examples.c  Sys_PowerModes_Sleep_Enter_Example
 */
void Sys_PowerModes_Sleep_Enter(sleep_mode_cfg *p_sleep_mode_cfg, Sleep_Retention_t retention_type);

/**
 * @brief Configure XTAL, Initialize System Clock to RF and clock dividers
 *
 * @param[in] p_clock_cfg  pointer to the clock_cfg instance
 * @note  This function will configure XTAL oscillator and set system clock
 *        to RF clock. It will also configure clock dividers.
 * @examplecode HAL_examples.c  Sys_PowerModes_Sleep_Enter_Example
 */
void Sys_PowerModes_XTAL_ClockInit(clock_cfg *p_clock_cfg);

/**
 * @brief Initialize the Standby Configuration
 *
 * @param[in] p_standby_mode_cfg  pointer to the standby_mode_cfg instance
 * @note      This function will modify ACS->WAKEUP_CFG and ACS->BOOT_CFG
 * @examplecode HAL_examples.c  Sys_PowerModes_Standby_Init_Example
 */
void Sys_PowerModes_Standby_Init(standby_mode_cfg *p_standby_mode_cfg);

/**
 * @brief Enter Standby mode
 *
 * @param[in] p_standby_mode_cfg  pointer to the sleep_mode_cfg instance
 * @note  This function will enter standby mode if there are no wakeup events
 *        or NVIC interrupts pending, otherwise it will continue to RUN mode and service
 *        pending events and interrupts
 * @examplecode HAL_examples.c  Sys_PowerModes_Standby_Enter_Example
 */
void Sys_PowerModes_Standby_Enter(standby_mode_cfg *p_standby_mode_cfg);

/**
 * @brief Initialize the DeepSleep Configuration
 *
 * @param[in] p_deepsleep_mode_cfg  pointer to the deepsleep_mode_cfg instance
 * @note      This function will modify ACS->WAKEUP_CFG and ACS->BOOT_CFG
 * @examplecode HAL_examples.c  Sys_PowerModes_DeepSleep_Init_Example
 */
void Sys_PowerModes_DeepSleep_Init(deepsleep_mode_cfg *p_deepsleep_mode_cfg);

/**
 * @brief Enter DeepSleep mode
 *
 * @param[in] p_deepsleep_mode_cfg  pointer to the deepsleep_mode_cfg instance
 * @note  This function will enter standby mode if there are no wakeup events
 *        or NVIC interrupts pending, otherwise it will continue to RUN mode and service
 *        pending events and interrupts
 * @examplecode HAL_examples.c  Sys_PowerModes_DeepSleep_Enter_Example
 */
void Sys_PowerModes_DeepSleep_Enter(deepsleep_mode_cfg *p_deepsleep_mode_cfg);

/**
 * @brief Wakeup from Flash with reset for sleep mode with SLEEP_NO_RETENTION
 *        and deep sleep power mode
 *
 * @param[in] p_sleep_mode_cfg  pointer to the sleep_mode_cfg instance
 * @note  This function initialize the system after wakeup with reset
 */
void Sys_PowerModes_Wakeup_WithReset(sleep_mode_cfg *p_sleep_mode_cfg);

/**
 * @brief ACS Wakeup configuration enable
 *
 * @param[in] acs_wakeup_cfg  Wakeup Config configuration; use
 *                            WAKEUP_DCDC_OVERLOAD_ENABLE,
 *                            WAKEUP_GPIO*_FALLING,
 *                            WAKEUP_GPIO*_ENABLE,
 *                            WAKEUP_RTC_OVERFLOW_ENABLE,
 *                            WAKEUP_FIFO_ENABLE,
 *
 * @param[in] p_wakeup_cfg pointer to the structure member instance of sleep_mode_cfg, or
 *                         deepsleep_mode_cfg, or standby_mode_cfg
 * @note  This function will modify the corresponding enables bits only
 * @examplecode HAL_examples.c  Sys_PowerModes_Wakeup_ConfigEnable_Example
 */
void Sys_PowerModes_Wakeup_ConfigEnable(uint32_t acs_wakeup_cfg, uint32_t *p_wakeup_cfg);

/**
 * @brief ACS Wakeup configuration disable
 *
 * @param[in] acs_wakeup_cfg  Wakeup Config configuration; use
 *                            WAKEUP_DCDC_OVERLOAD_ENABLE,
 *                            WAKEUP_GPIO*_FALLING,
 *                            WAKEUP_GPIO*_ENABLE,
 *                            WAKEUP_RTC_OVERFLOW_ENABLE,
 *                            WAKEUP_FIFO_ENABLE,
 *
 * @param[in] p_wakeup_cfg pointer to the structure member instance of sleep_mode_cfg, or
 *                         deepsleep_mode_cfg, or standby_mode_cfg
 * @note  This function will modify the corresponding enables bits only
 * @examplecode HAL_examples.c  Sys_PowerModes_Wakeup_ConfigDisable_Example
 */
void Sys_PowerModes_Wakeup_ConfigDisable(uint32_t acs_wakeup_cfg, uint32_t *p_wakeup_cfg);

/** @} */ /* End of the HALPOWERMODES group */
/** @} */ /* End of the HAL group */

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* POWER_MODES_H */
