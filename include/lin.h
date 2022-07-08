/**
 * @file lin.h
 * @brief Hardware abstraction layer for LIN interfaces
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
#ifndef LIN_H
#define LIN_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <gpio.h>

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALLIN LIN
 *  LIN hardware abstraction layer
 *  @{
 */

#ifndef NON_SECURE
/**
 * @brief       Configure two GPIOs for the specified LIN interface
 * @param[in]   lin     Pointer to the LIN instance
 * @param[in]   config  GPIO pin configuration for the LIN pads
 * @param[in]   tx      GPIO to use as the LIN transmit pad; use an integer
 * @param[in]   rx      GPIO to use as the LIN receive pad; use an integer
 * @assumptions
 *
 * @examplecode HAL_examples.c Sys_LIN_GPIOConfig_Example
 */
static inline void Sys_LIN_GPIOConfig(const LIN_Type *lin,
                                      uint32_t config,
                                      uint32_t tx,
                                      uint32_t rx)
{
    SYS_ASSERT(LIN_REF_VALID(lin));

    /* Divide by size of block in register map */
    unsigned int diff = (lin - LIN) / 0x100;

    /* Apply GPIO pad configuration */
    SYS_GPIO_CONFIG(tx, (config | (GPIO_MODE_LIN0_TX + (diff << GPIO_CFG_IO_MODE_Pos))));
    SYS_GPIO_CONFIG(rx, (config | GPIO_MODE_INPUT));

    /* Apply GPIO source configuration */
    GPIO->SRC_LIN[diff] = ((rx << GPIO_SRC_LIN_LIN_Pos) &
                           GPIO_SRC_LIN_LIN_Mask) |
                          LIN_ACTIVE_HIGH;
}
#endif /* ifndef NON_SECURE */

/**
 * @brief       Enable/Wake the connected transceiver, enable LIN.
 * @param[in]   lin     Pointer to the LIN instance
 * @assumptions None
 * @examplecode HAL_examples.c Sys_LIN_Enable_Example
 */
static inline void Sys_LIN_Enable(LIN_Type *lin)
{
    SYS_ASSERT(LIN_REF_VALID(lin));
    lin->CTRL = LIN_ENABLE;
    lin->CFG |= INIT_C617;
    lin->CFG &= ~INIT_C617;
}

/**
 * @brief       Disable the connected transceiver, disable LIN.
 * @param[in]   lin     Pointer to the LIN instance
 *
 * @examplecode HAL_examples.c Sys_LIN_Disable_Example
 */
static inline void Sys_LIN_Disable(LIN_Type *lin)
{
    SYS_ASSERT(LIN_REF_VALID(lin));
    lin->CTRL = LIN_DISABLE;
    lin->CFG &= ~INIT_C617;
}

/**
 * @brief       Clear all error flags.
 * @param[in]   lin     Pointer to the LIN instance
 *
 * @examplecode HAL_examples.c Sys_LIN_ClearErrors_Example
 */
static inline void Sys_LIN_ClearErrors(LIN_Type *lin)
{
    SYS_ASSERT(LIN_REF_VALID(lin));
    lin->ERROR = CLR_CE | CLR_PE | CLR_BE | CLR_FE;
}

/** @} */ /* End of the HALLIN group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* LIN_H */
