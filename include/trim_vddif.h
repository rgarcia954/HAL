/**
 * @file trim.h
 * @brief Hardware abstraction layer for loading trim values from NVR.
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

#include <hw.h>

#ifndef TRIM_VDDIF_H
#define TRIM_VDDIF_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALTRIM Trimming Support
 *  Trim Hardware Abstraction Layer
 *  @{
 */

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDIF_Example
 */
uint32_t Sys_Trim_LoadVDDIF(TRIM_Type *trim_values, uint32_t target);

/** @} */ /* End of the HALTRIM group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* TRIM_VDDIF_H */
