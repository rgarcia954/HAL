/**
 * @file clock.h
 * @brief Hardware abstraction layer for the system clock
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

#ifndef CLOCK_H_
#define CLOCK_H_

#include <power.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALCLOCK Clock Configuration
 *  Clock configuration  hardware abstraction layer
 *  @{
 */

/** @brief CMSIS required system core clock variable */
extern uint32_t SystemCoreClock;

/**
 * @brief       Configure the RC oscillator and system clock
 * @param[in]   cfg     Configuration of the system clock source and
 *                      prescale value; use
 *                      SYSCLK_CLKSRC_[RCCLK | STANDBYCLK | RFCLK | JTCK],
 *                      and JTCK_PRESCALE_*
 * @param[in]   rc_cfg  Configuration for the RC oscillator
 * @examplecode HAL_examples.c Sys_Clocks_RCSystemClkConfig_Example
 */
static inline void Sys_Clocks_RCSystemClkConfig(uint32_t cfg, uint32_t rc_cfg)
{
    /* Configure the flash delay control initially to the max delay configuration
     * so flash reads following the clock updates will always return meaningful data.
     * The correct delays will be set after the switch in SystemCoreClockUpdate(). */
    FLASH->DELAY_CTRL = (FLASH->DELAY_CTRL & ~FLASH_DELAY_CTRL_SYSCLK_FREQ_Mask) |
                        FLASH_DELAY_FOR_SYSCLK_48MHZ;

    ACS->RCOSC_CTRL = rc_cfg;
    CLK->SYS_CFG = cfg;
    SystemCoreClockUpdate();
}

/**
 * @brief       Configure the system clock
 * @param[in]   cfg     Configuration of the system clock source and
 *                      prescale value; use
 *                      SYSCLK_CLKSRC_[RCCLK | STANDBYCLK | RFCLK | JTCK],
 *                      and JTCK_PRESCALE_*
 * @assumptions The flash delay configuration is correct for the previously
 *              selected system clock source and frequency; if also changing
 *              the RC oscillator frequency, use Sys_Clocks_RCSystemClkConfig()
 * @examplecode HAL_examples.c Sys_Clocks_SystemClkConfig_Example
 */
static inline void Sys_Clocks_SystemClkConfig(uint32_t cfg)
{
    Sys_Clocks_RCSystemClkConfig(cfg, ACS->RCOSC_CTRL);
}

/**
 * @brief       Configure the 48 MHz XTAL Oscillator
 * @param[in]   xtal_prescaler     Configuration of the 48MHz XTAL Oscillator
 *                                 as clock and its prescale value; use
 *                                 CK_DIV_1_6_PRESCALE_[NO_CLOCK_BYTE | 1_BYTE
 *                                 | 2_BYTE | 3_BYTE | 4_BYTE | 5_BYTE | 6_BYTE],
 * @examplecode HAL_examples.c Sys_Clocks_XTALClkConfig_Example
 */
static inline void Sys_Clocks_XTALClkConfig(uint32_t xtal_prescaler)
{
    /* Enable VDDRF supply without changing trimming settings */
    ACS->VDDRF_CTRL = ((ACS->VDDRF_CTRL & ACS_VDDRF_CTRL_VTRIM_Mask) | VDDRF_ENABLE | VDDRF_DISABLE_HIZ);

    /* Wait until VDDRF supply has powered up */
    while (!(ACS->VDDRF_CTRL & VDDRF_READY));

    /* Connect VDDPA to VDDRF */
    ACS->VDDPA_CTRL = VDDPA_INITIAL_TRIM_1P05V | VDDPA_SW_VDDRF | VDDPA_ISENSE_DISABLE | VDDPA_DISABLE |
                      VDDPA_TRIM_1P60V;

    Sys_Power_RFEnable();

    /* Start the 48 MHz oscillator without changing the other register bits */
    RF->XTAL_CTRL = (RF->XTAL_CTRL & (~XTAL_CTRL_DISABLE_OSCILLATOR)) | XTAL_CTRL_REG_VALUE_SEL_INTERNAL;

    /* Enable 48 MHz oscillator divider at desired prescale value */
    RF0_REG33->CK_DIV_1_6_CK_DIV_1_6_BYTE = xtal_prescaler;

    /* Wait until 48 MHz oscillator is started */
    while ((RF0_ANALOG_INFO->ANALOG_INFO_BYTE & ANALOG_INFO_CLK_DIG_READY_BYTE) != ANALOG_INFO_CLK_DIG_READY_BYTE);
}

/**
 * @brief       Configure the clock divisors for a standard configuration:
 *              - SLOWCLK (1 MHz)
 *              - BBCLK (8 MHz)
 *              - DCCLK (4 MHz)
 *              - CPCLK (166 kHz)
 *              - UARTCLK as per uartclk_freq
 *              - SENSOR_CLK as per sensorclk_freq
 *              - USERCLK as per userclk_freq
 *
 *              If an exact configuration cannot be found for the desired
 *              frequency, the clock divisor will be set to ensure the divided
 *              clock does not exceed the specified target frequency.
 * @param[in]   uartclk_freq    Target frequency for the UART clock
 * @param[in]   sensorclk_freq  Target frequency for the sensor clock
 * @param[in]   userclk_freq    Target frequency for user clock; if the target
 *                              frequency exceeds the system clock frequency
 *                              and the RF clock is available, USERCLK will
 *                              be sourced from RF clock.
 * @assumptions The system clock has previously been configured.
 * @examplecode HAL_examples.c Sys_Clocks_DivisorConfig_Example
 */
void Sys_Clocks_DividerConfig(uint32_t uartclk_freq, uint32_t sensorclk_freq,
                              uint32_t userclk_freq);

/** @} */ /* End of the HALCLOCK group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* CLOCK_H_ */