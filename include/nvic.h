/**
 * @file nvic.h
 * @brief Hardware abstraction layer for NVIC
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
/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */

#ifndef NVIC_H
#define NVIC_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALNVIC Nested Vectored Interrupt Controller
 *  Nested Vectored Interrupt Controller (NVIC) hardware abstraction layer
 *  @{
 */

/**
 * @brief       Disable all external interrupts
 * @examplecode HAL_examples.c Sys_NVIC_DisableAllInt_Example
 */
__STATIC_INLINE void Sys_NVIC_DisableAllInt(void)
{
    NVIC->ICER[0] = 0xFFFFFFFFU;
    NVIC->ICER[1] = 0xFFFFFFFFU;
    NVIC->ICER[2] = 0xFFFFFFFFU;
}

/**
 * @brief       Clear the pending status for all external interrupts
 * @examplecode HAL_examples.c Sys_NVIC_ClearAllPendingInt_Example
 */
__STATIC_INLINE void Sys_NVIC_ClearAllPendingInt(void)
{
    NVIC->ICPR[0] = 0xFFFFFFFFU;
    NVIC->ICPR[1] = 0xFFFFFFFFU;
    NVIC->ICPR[2] = 0xFFFFFFFFU;
}

/** @} */ /* End of the HALNVIC group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* NVIC_H */
