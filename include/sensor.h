/**
 * @file sensor.h
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

#ifndef SENSOR_H
#define SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <hw.h>

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALSENSOR Sensor API
 *  Sensor Hardware Abstraction Layer
 *  @{ */

/**
 * @brief Configure the sensor interface, ensuring the sensor is powered
 *        if possible.
 *
 * @param [in] if_cfg interface configuration
 *
 * @param [in] wedac_high WEDAC output voltage during pulse (16mV steps)
 *                          (except 0x1F -> 0x20 = 8mV step.)
 *
 * @param [in] wedac_low WEDAC output voltage not during pulse (16mV steps)
 *                          (except 0x1F -> 0x20 = 8mV step.)
 * @param [in] clk_cfg select interface clock source
 * @examplecode  HAL_examples.c Sys_Sensor_ADCConfig_Example
 */
static inline void Sys_Sensor_ADCConfig(uint32_t if_cfg,
                                        uint32_t wedac_high,
                                        uint32_t wedac_low,
                                        uint32_t clk_cfg)
{
    SYS_ASSERT(SENSOR);
#ifndef NON_SECURE
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_SENSOR_PWR_EN_Pos))
            | ACS_PWR_KEY | ACS_SENSOR_POWERED;
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_SENSOR_ISOLATE_Pos))
            | ACS_PWR_KEY | ACS_SENSOR_NOT_ISOLATED;
#endif

    /* Set interface config */
    SENSOR->IF_CFG = if_cfg  | ((wedac_high & SENSOR_IF_CFG_WEDAC_HIGH_Mask)
                                | (wedac_low & SENSOR_IF_CFG_WEDAC_LOW_Mask));

    /* Chose clock source. */
    SENSOR->CLK_CFG = clk_cfg;
}

/**
 * @brief Configure sensor integration states.
 *
 * @param [in] pulse_count Duration of "Pulse Count Sample" state
 *
 * @param [in] pre_count Absolute Value of main counter to trigger
 *             the change of "Pre Count Sample" state
 *
 * @examplecode  HAL_examples.c Sys_Sensor_IntConfig_Example
 */
static inline void Sys_Sensor_IntConfig(uint32_t pulse_count,
                                        uint32_t pre_count)
{
    SYS_ASSERT(SENSOR);

    /* Set interrupt config */
    SENSOR->INT_CFG = ((pulse_count & SENSOR_INT_CFG_PULSE_COUNT_INT_Mask) |
                       (pre_count & SENSOR_INT_CFG_PRE_COUNT_INT_Mask));
}

/**
 * @brief Configure sensor idle time state.
 *
 * @param [in] idle_count Absolute Value of main counter to trigger
 *             the change of "Idle Time" state
 * @examplecode  HAL_examples.c Sys_Sensor_IdleConfig_Example
 */
static inline void Sys_Sensor_IdleConfig(uint32_t idle_count)
{
    SYS_ASSERT(SENSOR);

    /* Set interrupt config */
    SENSOR->IDLE_CFG = (idle_count & SENSOR_IDLE_CFG_IDLE_TIME_Mask);
}

/**
 * @brief Configure sensor timer settings.
 *
 * @param [in] cfg timer configuration
 * @param [in] re_idle_connect RE pad connected during idle state switch
 * @examplecode  HAL_examples.c Sys_Sensor_IdleConfig_Example
 */
static inline void Sys_Sensor_TimerConfig(uint8_t cfg,
                                          uint8_t re_idle_connect)
{
    SYS_ASSERT(SENSOR);
    SENSOR->TIMER_CTRL = cfg;
    SENSOR_TIMER_CTRL->RE_IDLE_CONNECT_BYTE = re_idle_connect;
}

/**
 * @brief Configure data storage settings.
 *
 * @param [in] diff_mode  enable differential storage mode
 *
 * @param [in] sum_en  enable summation mode
 *
 * @param [in] nbr_samples number of samples to store before wakeup in
 *                         sensor detect mode or for impedance measurement
 *
 * @param [in] threshold Sensor data level threshold for wakeup
 *
 * @param [in] store_en Enable storing samples in FIFO
 *
 * @param [in] fifo_size Number of samples to store in FIFO before wakeup of core.
 * @examplecode  HAL_examples.c Sys_Sensor_StorageConfig_Example
 */
static inline void Sys_Sensor_StorageConfig(uint32_t diff_mode, uint32_t sum_en,
                                            uint32_t nbr_samples, uint32_t threshold,
                                            uint32_t store_en, uint32_t fifo_size)
{
    SYS_ASSERT(SENSOR);
    SENSOR->PROCESSING =  diff_mode | sum_en | threshold;
    if (nbr_samples > 0xFFFF)
    {
        /** Assume user input define from header */
        SENSOR_PROCESSING->NBR_SAMPLES_BYTE =  (uint8_t)(nbr_samples >> SENSOR_PROCESSING_NBR_SAMPLES_Pos);
    }
    else
    {
        /** Assume user input direct number of samples*/
        SENSOR_PROCESSING->NBR_SAMPLES_BYTE =  (uint8_t)nbr_samples;
    }
    SENSOR->FIFO_CFG =  store_en | fifo_size;
}

/**
 * @brief Configure sensor delay clocks and length. Use sub-register defines.
 *
 * @note Clocks can be 32kHz(0) or 1kHz(1-default)
 *
 * @param [in] clk_1l clock for delay state "Delay 1 WE_L"
 *
 * @param [in] clk_2l clock for delay state "Delay 2 WE_L"
 *
 * @param [in] clk_1h clock for delay state "Delay 1 WE_H"
 *
 * @param [in] clk_2h clock for delay state "Delay 2 WE_H"
 *
 * @param [in] len_1l number of periods for delay state "Delay 1 WE_L"
 *
 * @param [in] len_2l number of periods for delay state "Delay 2 WE_L"
 *
 * @param [in] len_1h number of periods for delay state "Delay 1 WE_H"
 *
 * @param [in] len_2h number of periods for delay state "Delay 2 WE_H"
 * @examplecode  HAL_examples.c Sys_Sensor_DelayConfig_Example
 */
static inline void Sys_Sensor_DelayConfig(uint32_t clk_1l, uint32_t clk_2l,
                                          uint32_t clk_1h, uint32_t clk_2h,
                                          uint32_t len_1l, uint32_t len_2l,
                                          uint32_t len_1h, uint32_t len_2h)
{
    SYS_ASSERT(SENSOR);

    /** Set delay configuration */
    SENSOR_DELAY_L_CFG->DLY1_WE_L_SHORT = (clk_1l) | (len_1l);

    SENSOR_DELAY_L_CFG->DLY2_WE_L_SHORT = (clk_2l) | (len_2l);

    SENSOR_DELAY_H_CFG->DLY1_WE_H_SHORT = (clk_1h) |  (len_1h);

    SENSOR_DELAY_H_CFG->DLY2_WE_H_SHORT = (clk_2h) |  (len_2h);
}

/**
 * @brief Enable the sensor.  In secure mode, also enable power to the sensor.
 * @examplecode HAL_examples.c  Sys_Sensor_Enable_Example
 */
static inline void Sys_Sensor_Enable(void)
{
    SYS_ASSERT(SENSOR);

    /* Enable the sensor and power for the sensor. */
    SENSOR->IF_CFG |= SENSOR_ENABLED;

#ifndef NON_SECURE
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_SENSOR_PWR_EN_Pos))
            | ACS_PWR_KEY | ACS_SENSOR_POWERED;
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_SENSOR_ISOLATE_Pos))
            | ACS_PWR_KEY | ACS_SENSOR_NOT_ISOLATED;
#endif
}

/**
 * @brief Disable the sensor.  In secure mode, also disable power to the sensor.
 * @examplecode HAL_examples.c  Sys_Sensor_Disable_Example
 */
static inline void Sys_Sensor_Disable(void)
{
    SYS_ASSERT(SENSOR);

    /* Disable the sensor and power for the sensor. */
    SENSOR->IF_CFG &= ~SENSOR_DISABLED;

#ifndef NON_SECURE
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_SENSOR_ISOLATE_Pos))
            | ACS_PWR_KEY | ACS_SENSOR_ISOLATED;
    ACS->PWR_CTRL = ((ACS->PWR_CTRL) & ~(1 << ACS_PWR_CTRL_SENSOR_PWR_EN_Pos))
            | ACS_PWR_KEY | ACS_SENSOR_SHUTDOWN;
#endif
}

/**
 * @brief The sensor timer counter, the sensor timer enable
 *        and the ADC counter are reset
 * @examplecode  HAL_examples.c Sys_Sensor_TimerReset_Example
 */
static inline void Sys_Sensor_TimerReset(void)
{
    SYS_ASSERT(SENSOR);
    SENSOR_TIMER_CTRL->RESET_BYTE = SENSOR_CNT_RESET_BYTE;
}

/**
 * @brief Read the current delay state of the sensor interface.
 *
 * @return  current state of the sensor interface.
 * @examplecode  HAL_examples.c Sys_Sensor_CurrentState_Example
 */
static inline uint8_t Sys_Sensor_CurrentState(void)
{
    SYS_ASSERT(SENSOR);
    return SENSOR_MAIN_COUNT->STATE_BYTE;
}

/**
 * @brief Read the current value of the sensor's main counter
 *
 * @return  current value of the main counter.
 * @examplecode  HAL_examples.c Sys_Sensor_CurrentCountValue_Example
 */
static inline uint32_t Sys_Sensor_CurrentCountValue(void)
{
    SYS_ASSERT(SENSOR);
    return (SENSOR->MAIN_COUNT & SENSOR_MAIN_COUNT_VALUE_Mask);
}

/** @} */ /* End of the HALSENSOR group */
/** @} */ /* End of the HAL group */
#ifdef __cplusplus
}

#endif    /* ifdef __cplusplus */

#endif    /* SENSOR_H */
