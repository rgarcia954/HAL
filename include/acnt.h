/**
 * @file acnt.h
 * @brief Hardware abstraction layer for the activity counter module.
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

#ifndef ACNT_H
#define ACNT_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALACNT Activity Counter
 *  Activity counter hardware abstraction layer
 *  @{
 */

/**
 * @brief       Start activity counter
 * @examplecode HAL_examples.c Sys_ACNT_Start_Example
 */
static inline void Sys_ACNT_Start(void)
{
    SYS_ASSERT(SYSCTRL);
    SYSCTRL->CNT_CTRL = CNT_START;
}

/**
 * @brief       Stop activity counter
 * @examplecode HAL_examples.c Sys_ACNT_Stop_Example
 */
static inline void Sys_ACNT_Stop(void)
{
    SYS_ASSERT(SYSCTRL);
    SYSCTRL->CNT_CTRL = CNT_STOP;
}

/**
 * @brief       Clear activity counter values
 * @examplecode HAL_examples.c Sys_ACNT_Clear_Example
 */
static inline void Sys_ACNT_Clear(void)
{
    SYS_ASSERT(SYSCTRL);
    SYSCTRL->CNT_CTRL = CNT_CLEAR;
}

/** @} */ /* End of the HALACNT group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* TIMER_H */
