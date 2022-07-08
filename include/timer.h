/**
 * @file timer.h
 * @brief Hardware abstraction layer for the general purpose timers
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

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALTIMER General-Purpose Timer
 *  General-purpose timer hardware abstraction layer
 *  @{
 */

/**
 * @brief       Configure timer instance
 * @param[in]   timer     Pointer to the timer instance
 * @param[in]   cfg0      Timer configuration 0; use
 *                        TIMER_PRESCALE_*,
 * @param[in]   cfg1      Timer configuration 1; use
 *                        TIMER_MULTI_COUNT_*
 *                        [TIMER_SHOT_MODE | TIMER_FREE_RUN]
 * @param[in]   timeout   number of timer clock cycles before a timeout would
 *                        occur
 * @examplecode HAL_examples.c Sys_Timer_Config_Example
 */
static inline void Sys_Timer_Config(TIMER_Type *timer, uint32_t cfg0,
                                    uint32_t cfg1, uint32_t timeout)
{
    SYS_ASSERT(TIMER_REF_VALID(timer));
    timer->CFG0 = (cfg0 | ((timeout << TIMER_CFG0_TIMEOUT_VALUE_Pos) &
                           TIMER_CFG0_TIMEOUT_VALUE_Mask));
    timer->CFG1 = cfg1;
}

/**
 * @brief       Start or restart timer instance
 * @param[in]   timer Pointer to the timer instance
 * @examplecode HAL_examples.c Sys_Timer_Start_Example
 */
static inline void Sys_Timer_Start(TIMER_Type *timer)
{
    SYS_ASSERT(TIMER_REF_VALID(timer));
    timer->CTRL = TIMER_START;
}

/**
 * @brief       Stop the timer instance
 * @param[in]   timer Pointer to the timer instance
 * @examplecode HAL_examples.c Sys_Timer_Stop_Example
 */
static inline void Sys_Timer_Stop(TIMER_Type *timer)
{
    SYS_ASSERT(TIMER_REF_VALID(timer));
    timer->CTRL = TIMER_STOP;
}

/** Configures the default timer instance */
#define SYS_TIMER_CONFIG(cfg0, cfg1, timeout) Sys_Timer_Config(TIMER, (cfg0), \
                                                               (cfg1), (timeout))

/** Starts the default timer instance */
#define SYS_TIMER_START() Sys_Timer_Start(TIMER)

/** Stops the default timer instance */
#define SYS_TIMER_STOP() Sys_Timer_Stop(TIMER)

/** @} */ /* End of the HALTIMER group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* TIMER_H */
