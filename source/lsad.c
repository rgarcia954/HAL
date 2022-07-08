/**
 * @file lsad.c
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

#include <hw.h>

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
 *  LSAD Hardware Abstraction Layer
 *  @{
 */

#ifndef NON_SECURE
uint32_t Sys_LSAD_InputConfig(uint32_t num, uint32_t source,
                              uint32_t pos_gpio, uint32_t neg_gpio)
{
    uint32_t positive_input_select;
    uint32_t negative_input_select;
    uint32_t input_select_mask;
    uint32_t input_select_position;

    /* Isolate positive and negative input sources */
    positive_input_select = (source & LSAD_INPUT_SEL_POS_INPUT_SEL_Mask) >> PRE_SEL_SIZE;
    negative_input_select = source & LSAD_INPUT_SEL_NEG_INPUT_SEL_Mask;

    /* Check if the positive input selected is a GPIO */
    if ((positive_input_select << PRE_SEL_SIZE) < LSAD_POS_INPUT_AOUT)
    {
        if (pos_gpio <= GPIO15)
        {
            /* Connect the GPIO selected to the appropriate pre-select input*/
            /* Check that it is a valid GPIO/pre-select input combination */
            if (((pos_gpio % 2 == 0) && (positive_input_select % 2 == 0)) ||
                ((pos_gpio % 2 == 1) && (positive_input_select % 2 == 1)))
            {
                /* Valid combination, find position of input_select */
                input_select_position =
                    (LSAD_PRE_SEL_INPUT_LSAD_PRE_SEL_IN0_Pos + (PRE_SEL_SIZE * positive_input_select));
                input_select_mask =  0x7U << input_select_position;
                LSAD->PRE_SEL_INPUT = (LSAD->PRE_SEL_INPUT & ~input_select_mask) |
                                      (pos_gpio / 2) << input_select_position;

                /* Valid GPIO combination detected, disable digital functions */
                SYS_GPIO_CONFIG(pos_gpio, GPIO_MODE_DISABLE | GPIO_NO_PULL);
            }
            else
            {
                /* Invalid GPIO input selection combination */
                return ERROR_LSAD_INPUT_CFG;
            }
        }
    }

    /* Check if the negative input selected is a GPIO */
    if (negative_input_select < LSAD_NEG_INPUT_AOUT)
    {
        if (neg_gpio <= GPIO15)
        {
            /* Connect the GPIO selected to the appropriate pre-select input*/
            /* Check that it is a valid GPIO/pre-select input combination */
            if (((neg_gpio % 2 == 0) && (negative_input_select % 2 == 0)) ||
                ((neg_gpio % 2 == 1) && (negative_input_select % 2 == 1)))
            {
                /* Valid combination, find position of input_select */
                input_select_position =
                    (LSAD_PRE_SEL_INPUT_LSAD_PRE_SEL_IN0_Pos + (PRE_SEL_SIZE * negative_input_select));
                input_select_mask =  0x7U << input_select_position;
                LSAD->PRE_SEL_INPUT = (LSAD->PRE_SEL_INPUT & ~input_select_mask) |
                                      (neg_gpio / 2) << input_select_position;

                /* Valid GPIO combination detected, disable digital functions */
                SYS_GPIO_CONFIG(neg_gpio, GPIO_MODE_DISABLE | GPIO_NO_PULL);
            }
            else
            {
                /* Invalid GPIO input selection combination */
                return ERROR_LSAD_INPUT_CFG;
            }
        }
    }

    /* Apply input selection to ADC channel */
    LSAD->INPUT_SEL[num] = source;

    return ERROR_NO_ERROR;
}
#endif

/** @} */ /* End of the HALLSAD group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */
