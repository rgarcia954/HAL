/**
 * @file rffe.h
 * @brief Radio Frequency Front End support header
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

#ifndef RFFE_H
#define RFFE_H

/** @addtogroup HAL
 *  @{ */
/** @defgroup HALRFFE RFFE Radio Frequency Front End
 *  Radio Frequency Front End (RFFE) hardware abstraction layer
 *  @{
 **/

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @brief External include files */
#include <hw.h>
#include <stdlib.h>
/* ----------------------------------------------------------------------------
 * Assumptions: LSAD = low frequency mode (SLOWCLK = 1 MHz)
 * ------------------------------------------------------------------------- */

/** Three times the length of time corresponding to the minimum sample rate,
 * which is deemed sufficient to allow the LSAD to stabilize */
#define STABILIZATION_DELAY              (SystemCoreClock * 3 / 625)

/** Corresponds to sample rate of the LSAD as configured (625 Hz) */
#define MEASUREMENT_DELAY                (SystemCoreClock / 625)

/** Factor for converting back and forth from mV to V. */
#define V_TO_MV                          1000

/** Float iteration of factor for converting back and forth from mV to V. */
#define V_TO_MV_F                        1000.0f

/** Default LSAD channel used to measure voltage rails */
#define DEF_CHANNEL                      6

/** Maximum number of LSAD channels in the design */
#define MAX_LSAD_CHANNEL                 7

/** VDDPA enable selection */
#define VDDPA_EN                         1

/** VDDPA disable selection */
#define VDDPA_DIS                        0

/** We strongly recommended having VCC at least 50mV higher than VDDRF. */
#define VCC_VDDRF_MARGIN                 50

/** Trim margin. The granularity of trims for VDDRF allow for a certain error
 * above or below the set value */
#define TRIM_MARGIN                      10    /*mv*/

/** Estimated voltage increase per 1dBm increased TX power */
#define MV_PER_DBM_VDDPA                 100

/** Estimated voltage increase per 1dBm increased TX power */
#define MV_PER_DBM_VDDRF                 75

/** Estimated trim steps per 1dBm increased TX power */
#define STEPS_PER_DBM_VDDRF              6

/** Estimated trim steps per 1dBm increased TX power */
#define STEPS_PER_DBM_VDDPA              10

/** Maximum RF output power possible */
#define RF_MAX_POWER                     6

/** Maximum RF output power possible without using VDDPA */
#define RF_MAX_POWER_NO_VDDPA            2

/** Typical RF output power when VDDPA is not used, with default trims */
#define RF_NO_VDDPA_TYPICAL_POWER        0

/** RF output power used by default */
#define RF_DEFAULT_POWER                 0

/** Minimum possible RF output power */
#define RF_MIN_POWER                     -17

/** RF Output Power code for 0dBm */
#define PA_PWR_BYTE_0DBM                  0x0C

/** Power amplifier bias enable */
#define PA_ENABLE_BIAS_SETTING            0xF3

/** Power amplifier bias disable */
#define PA_DISABLE_BIAS_SETTING           0x73

/* Delay for various VDDPA automatic switching states */
/** Switch control delay */
#define SW_CTRL_DELAY_3_BYTE              ((uint8_t)0x2U)

/** Ramp-up delay */
#define RAMPUP_DELAY_3_BYTE               ((uint8_t)0x2U)

/** Disable delay */
#define DISABLE_DELAY_3_BYTE              ((uint8_t)0x2U)

/* Definitions for possible RFFE error conditions */
/** Error marker for RFFE errors */
#define ERRNO_TX_POWER_MARKER             0x30

/** No trims found when attempting to adjust voltage rails */
#define ERRNO_NO_TRIMS                    (0x01 | ERRNO_TX_POWER_MARKER)

/** Setting does not exist */
#define ERRNO_RFFE_MISSINGSETTING_ERROR   (0x02 | ERRNO_TX_POWER_MARKER)

/** Setting is not possible */
#define ERRNO_RFFE_INVALIDSETTING_ERROR   (0x03 | ERRNO_TX_POWER_MARKER)

/** VCC is too low to increase VDDRF sufficiently to suppor the requested RF output power */
#define ERRNO_RFFE_VCC_INSUFFICIENT       (0x04 | ERRNO_TX_POWER_MARKER)

/* Definitions for possible RFFE warning conditions */
/** Warning that the device is in a very low RF output power state */
#define WARNING_RFFE_VLOW_POWER_STATE     (0x05 | ERRNO_TX_POWER_MARKER)

/** Warning that the device has the power amplifier enabled */
#define WARNING_RFFE_PA_ENABLED_STATE     (0x06 | ERRNO_TX_POWER_MARKER)

/**
 * @brief       Converts an ADC code to a voltage, calculated as follows
 *              voltage = adc_code * (2 V * 1000 [mV]/1 V / 2^14 steps).
 * @param[in]     x     the ADC code input
 * @return      The voltage output in mV
 * @assumptions Low frequency mode for the ADC is used, meaning that the
 *              resolution of the ADC is 14-bits. CONVERT provides voltage
 *              level as a milliVolt value based on the input ADC code.
 */
#define CONVERT(x)                      ((uint32_t)((x * 1000) >> 13))

/**
 * @brief       Swap the values in variables a and b
 * @param[in]   a  holds the value that must go to b
 * @param[in]   b  holds the value that must go to a
 * @param[in]   t  a temporary buffer for the swap
 * @return      a holds the value previously in b
 * @return      b holds the value previously in a
 */
#define SWAP(a, b, t)                   ((t) = (a), (a) = (b), (b) = (t))

/**
 * @brief       Retrieve the current setting for RF output power by using the
 *              values retrieved from the appropriate registers.
 * @param[in]   lsad_channel  The LSAD channel used for measuring VDDRF
 * @return      The currently set TX output power. Returns -100 in error state.
 * @examplecode HAL_examples.c Sys_RFFE_GetTXPower_Example
 */
int8_t Sys_RFFE_GetTXPower(uint32_t lsad_channel);

/**
 * @brief       Set the TX Power according to the desired target value with
 *              an accuracy of +/-1 dBm for +6 dBm to -17 dBm. This function
 *              sets VDDRF, VDDPA, and PA_PWR (RF_REG1A) when applicable.
 *              Note:  This function provides RF TX power configurations
 *                     that match the requested levels, without considering
 *                     the potential for increased power consumption due to
 *                     the use of VDDPA.
 * @param[in]   target        Target transmission power in the range
 *                            from -17 to +6 dBm in 1 dBm increments.
 * @param[in]   lsad_channel  LSAD channel to use to measure rails,
 *                            if necessary.
 * @param[in]   pa_en         If 1, the power amplifier will be enabled, otherwise,
 *                            it will be disabled. The power amplifier will be
 *                            enabled regardless if 'target' is greater than 2.
 * @return      Return value  error value, if any
 * @examplecode HAL_examples.c Sys_RFFE_SetTXPower_Example
 */
uint32_t Sys_RFFE_SetTXPower(int8_t target, uint8_t lsad_channel, bool pa_en);

/**
 * @brief       Set the TX Power according to the desired target value with
 *              an accuracy of +/-1 dBm for +6 dBm to -17 dBm. This function
 *              sets VDDRF, VDDPA, and PA_PWR (RF_REG1A) when applicable.
 *              Note:  This function provides RF TX power configurations
 *                     that match the requested levels, without considering
 *                     the potential for increased power consumption due to
 *                     the use of VDDPA.
 * @param[in]   target        Target transmission power in the range
 *                            from -17 to +6 dBm in 1 dBm increments.
 * @return      Return value  error value, if any
 * @examplecode HAL_examples.c SYS_RFFE_SETTXPOWER_Example
 */
#define SYS_RFFE_SETTXPOWER(target) Sys_RFFE_SetTXPower(target, DEF_CHANNEL, VDDPA_DIS)

/** @} */ /* End of the HALRFFE group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* RFFE_H */
