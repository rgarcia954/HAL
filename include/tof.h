/**
 * @file tof.h
 * @brief Hardware abstraction layer for the time of flight module.
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

#ifndef TOF_H
#define TOF_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALTOF Time of Flight
 *  Time of Flight (TOF) hardware abstraction layer
 *  @{
 */

/**
 * @brief       Configure TOF module
 * @param[in]   cfg Time of flight configuration; use
 *                       [TOF_ERROR_INT_ENABLE | TOF_ERROR_INT_DISABLE],
 *                       [TOF_OVERRUN_INT_ENABLE | TOF_OVERRUN_INT_DISABLE],
 *                       [TOF_AVG_DATA_INT_ENABLE | TOF_AVG_DATA_INT_DISABLE],
 *                       [TOF_DATA_INT_ENABLE | TOF_DATA_INT_DISABLE],
 *                       [TOF_AVG_DATA_DMA_ENABLE | TOF_AVG_DATA_DMA_DISABLE],
 *                       [TOF_DATA_DMA_ENABLE | TOF_DATA_DMA_DISABLE],
 *                        TOF_AVG_DATA_*,
 *                        TOF_STOP_SRC_*,
 *                        TOF_START_SRC_*,
 *                        TOF_CLK_PRESCALE_*
 * @examplecode HAL_examples.c Sys_TOF_Config_Example
 */
static inline void Sys_TOF_Config(uint32_t cfg)
{
    SYS_ASSERT(TIMER_REF_VALID(TOF));
    TOF->CFG = cfg;
}

/**
 * @brief       Start the time of flight module
 * @examplecode HAL_examples.c Sys_TOF_Start_Example
 */
static inline void Sys_TOF_Start(void)
{
    SYS_ASSERT(TIMER_REF_VALID(TOF));
    TOF->CTRL = TOF_START;
}

/**
 * @brief       Stop the time of flight module
 * @examplecode HAL_examples.c Sys_TOF_Stop_Example
 */
static inline void Sys_TOF_Stop(void)
{
    SYS_ASSERT(TIMER_REF_VALID(TOF));
    TOF->CTRL = TOF_STOP;
}

/** @} */ /* End of the HALTOF group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* TIMER_H */
