/**
 * @file rtc.h
 * @brief Header file for API functions for the sensor interface.
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

#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <hw.h>

/** @addtogroup HAL
 *  @{
 */
/** \defgroup HALRTC RTC
 *  Real Time Clock Hardware Abstraction Layer
 *  @{
 */

/**
 * @brief Configure RTC block
 *
 * @param [in] start_value Start value for the RTC timer counter; use a 32 bit
 *                         value
 * @param [in] rtc_ctrl RTC control register; use
 *                          RTC_FORCE_CLOCK,
 *                          RTC_RESET,
 *                          RTC_ALARM_*,
 *                          RTC_CLK_SRC_*, and
 *                          RTC_[DISABLE | ENABLE]
 * @examplecode HAL_examples.c Sys_RTC_Config_Example
 */
static inline void Sys_RTC_Config(uint32_t start_value,
                                  uint32_t rtc_ctrl)
{
    ACS->RTC_CFG = start_value;
    ACS->RTC_CTRL = rtc_ctrl;
}

/**
 * @brief Read the current value of the RTC timer
 * @return  RTC timer counter current value
 * @examplecode HAL_examples.c Sys_RTC_Value_Example
 */
static inline uint32_t Sys_RTC_Value(void)
{
    /* The read of the RTC_COUNT register is byte wise in the ACS bridge and
     * non-atomic. This can lead to read failures of the counter value due to
     * synchronization between SYSCLK and RTC_CLK clock domains.
     * To avoid the issue, we perform three reads to make sure the counter is
     * not clocked in the middle of the read.
     * Interrupts should be disabled to avoid potential delays between the
     * first and second read. */

    uint32_t read1 = ACS->RTC_COUNT;
    uint32_t read2 = ACS->RTC_COUNT;
    return (read1 != read2) ? ACS->RTC_COUNT : read1;
}

/** @} */ /* End of the HALRTC group */
/** @} */ /* End of the HAL group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* RTC_H */
