/**
 * @file lsad.h
 * @brief Hardware abstraction layer for the LSAD
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
#ifndef LSAD_H
#define LSAD_H

#include <trim.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALLSAD LSAD
 *  LSAD hardware abstraction layer
 *  @{
 */

/* Quotients required to convert saved lsad offset/gain in nvr to Voltages */
/** LSAD offset conversion quotient */
#define LSAD_OFFSET_ERROR_CONV_QUOTIENT  32768.0f;

/** LSAD gain conversion quotient */
#define LSAD_GAIN_ERROR_CONV_QUOTIENT    65536.0f;

/** Error code for failed configuration */
#define ERROR_LSAD_INPUT_CFG             0x80

/** Value size of pre-select inputs in bits */
#define PRE_SEL_SIZE                     4

/** LSAD structure for offset/gain conversion function */
struct F_LSAD_TRIM
{
    /** Low frequency LSAD offset */
    float lf_offset;

    /** High frequency LSAD offset */
    float hf_offset;

    /** Low frequency LSAD gain */
    float lf_gain;

    /** High frequency LSAD gain */
    float hf_gain;
};

/* ----------------------------------------------------------------------------
 * LSAD support functions
 * ------------------------------------------------------------------------- */

/**
 * @brief       Convert a gain and offset value from NVR in integer format
 *              to float format.
 * @param[in]   i_gain_offset Gain and offset error from NVR, raw integer form
 * @param[out]  f_gain_offset Gain and offset error converted to floating
 *              point.
 */
static inline void Sys_LSAD_Gain_Offset(const volatile struct LSAD_TRIM *i_gain_offset,
                                        struct F_LSAD_TRIM *f_gain_offset)
{
    SYS_ASSERT(LSAD);
    f_gain_offset->hf_gain = (float)(i_gain_offset->hf_gain) / \
                             LSAD_GAIN_ERROR_CONV_QUOTIENT;
    f_gain_offset->lf_gain = (float)(i_gain_offset->lf_gain) / \
                             LSAD_GAIN_ERROR_CONV_QUOTIENT;
    f_gain_offset->hf_offset = (float)((int16_t)(i_gain_offset->hf_offset)) / \
                               LSAD_OFFSET_ERROR_CONV_QUOTIENT;
    f_gain_offset->lf_offset = (float)((int16_t)(i_gain_offset->lf_offset)) / \
                               LSAD_OFFSET_ERROR_CONV_QUOTIENT;
}

/**
 * @brief       Convert a gain and offset value from NVR in integer format
 *              to float format for the temperature sensor.
 * @param[in]   i_gain_offset Gain and offset error from NVR, raw integer form
 * @param[out]  f_gain_offset Gain and offset error converted to floating
 *              point.
 */
static inline void Sys_LSAD_TempSensor_Gain_Offset
    (const volatile struct TEMP_SENSOR_TRIM *i_gain_offset,
    struct F_LSAD_TRIM *f_gain_offset)
{
    SYS_ASSERT(LSAD);
    f_gain_offset->lf_gain = (float)(i_gain_offset->gain) / \
                             LSAD_GAIN_ERROR_CONV_QUOTIENT;
    f_gain_offset->lf_offset = (float)(i_gain_offset->offset) / \
                               LSAD_OFFSET_ERROR_CONV_QUOTIENT;
}

/**
 * @brief       Configure LSAD input channel
 * @param[in]   num      Channel number; use an integer
 * @param[in]   source   Select ADC channel input source use
 *                       LSAD_[POS | NEG]_INPUT_[SEL* | AOUT | VBAT | TEMP | GND].
 * @param[in]   pos_gpio Select GPIO to use for positive input(if applicable);
 *                       use an integer. If no GPIO used, use a negative integer.
 * @param[in]   neg_gpio Select GPIO to use for negative input(if applicable);
 *                       use an integer. If no GPIO used, use a negative integer.
 * @return      Returns success/fail result of configuration. 0 = success.
 * @examplecode HAL_Examples.c Sys_LSAD_InputConfig_Example
 */
#ifndef NON_SECURE
uint32_t Sys_LSAD_InputConfig(uint32_t num, uint32_t source,
                              uint32_t pos_gpio, uint32_t neg_gpio);
#endif

/** @} */ /* End of the HALLSAD group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* LSAD_H */
