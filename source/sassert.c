/**
 * @file sassert.c
 * @brief Simple abstraction assert implementation
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

#if defined (DEBUG_SYS)
void Sys_Assert(const char *file, int line)
{
    (void)file;    /* Unused parameter */
    (void)line;    /* Unused parameter */

    while (true)
    {
        /* Intentionally left blank for debugging */
    }
}

#else    /* if defined (DEBUG_SYS) */
/* Intentionally added to remove empty translation unit warnings */
typedef int dummy_type;
#endif    /* if defined (DEBUG_SYS) */
