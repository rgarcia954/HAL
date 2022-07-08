/**
 * @file cortex.h
 * @brief Hardware abstraction layer for the Arm Cortex-M3 and Cortex-M33 macros
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
#ifndef CORTEX_H_
#define CORTEX_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 *   Arm Cortex-M3/Arm Cortex-M33 processor support macros
 * ------------------------------------------------------------------------- */
/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALCORTEX Arm Cortex-M Processor
 *  Arm Cortex-M processor support hardware abstraction layer
 *  @{
 */

/**
 * @brief       Wait for an event
 * @examplecode HAL_examples.c SYS_WAIT_FOR_EVENT_Example
 */
#define SYS_WAIT_FOR_EVENT              __ASM("wfe")

/**
 * @brief       Wait for an interrupt
 * @examplecode HAL_examples.c SYS_WAIT_FOR_INTERRUPT_Example
 */
#define SYS_WAIT_FOR_INTERRUPT          __ASM("wfi")

/** @} */ /* End of the HALCORTEX group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* CORTEX_H_ */
