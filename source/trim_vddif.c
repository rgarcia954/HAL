/**
 * @file trim_vddif.c
 * @brief Hardware abstraction layer for loading trim values for vddif from NVR.
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
 * @endparb
 * */

#include <hw.h>
#ifndef RSL15_CID

/* Private function definitions */
static uint32_t Sys_Trim_LoadVDDIF_private(TRIM_Type *trim_values, uint32_t target);

uint32_t Sys_Trim_LoadVDDIF(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadVDDIF_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadVDDIF_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)trim_values->vddif,
                               target,
                               TRIM_RECORDS,
                               &trim);
    if (ret_val == ERROR_NO_ERROR)
    {
        /* We have valid trim value, load the trim value. */
        ACS_VDDIF_CTRL->VTRIM_BYTE = trim & TRIM_8_BIT_TRIM_MASK;
    }

    return ret_val;
}

#endif    /* ifndef RSL15_CID */
