/**
 * @file sassert.h
 * @brief Simple abstraction assert implementation
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

#ifndef SASSERT_H
#define SASSERT_H

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
/** @defgroup HALSASSERT Simple Assertions
 *  Simple Assertion (SASSERT) hardware abstraction layer
 *  @{
 */

#if defined (DEBUG_SYS)

/* For code space optimization, only filename and line number are passed*/
void Sys_Assert(const char *file, int line);

/** @brief Assertion handler; debug behavior */
#define SYS_ASSERT(expr)   ((expr) ? ((void)0) : Sys_Assert(__FILE__, __LINE__))
#else    /* if defined (DEBUG_SYS) */

/** @brief Assertion handler; default behavior is no operation */
#define SYS_ASSERT(expr)
#endif    /* if defined (DEBUG_SYS) */

/** @} */ /* End of the HALSASSERT group */
/** @} */ /* End of the HAL group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* SASSERT_H */
