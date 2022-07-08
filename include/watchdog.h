/**
 * @file watchdog.h
 * @brief Hardware abstraction layer for watchdog timers
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

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALWATCHDOG Watchdog
 * Watchdog timer hardware abstraction layer
 *  @{
 */

/**
 * @brief       Refresh the chip and software watchdog timers
 */
#define SYS_WATCHDOG_REFRESH() WATCHDOG->CTRL = WATCHDOG_REFRESH

/** @} */ /* End of the HALWATCHDOG group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* WATCHDOG_H_ */
