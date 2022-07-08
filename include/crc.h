/**
 * @file crc.h
 * @brief Hardware abstraction layer for the crc module of the Cortex M3
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

#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALCRC Cyclic Redundancy Check
 *  Cyclic Redundancy Check (CRC) hardware abstraction layer
 * @{
 */

/**
 * @brief       Configure the CRC generator type, endianness of the
 *              input data, and standard vs non-standard CRC behavior
 * @param[in]   crc     Pointer to the CRC instance
 * @param[in]   config  CRC generator configuration; use
 *                      CRC_[CCITT | 32],
 *                      CRC_[BIG | LITTLE]_ENDIAN,
 *                      CRC_BIT_ORDER_[STANDARD | NON_STANDARD],
 *                      CRC_FINAL_REVERSE_[STANDARD | NON_STANDARD], and
 *                      CRC_FINAL_XOR_[STANDARD | NON_STANDARD]
 * @assumptions Note that D_CRC supports only CRC_CCITT mode, hence no
 *              configuration is applied for this instance.
 * @examplecode HAL_examples.c Sys_Set_CRC_Config_Example
 */
static inline void Sys_Set_CRC_Config(CRC_Type *crc, uint32_t config)
{
    SYS_ASSERT(CRC_REF_VALID(CRC));
    if (crc == CRC)
    {
        crc->CFG = config;
    }
}

/**
 * @brief       Initialize CRC for CRC-32
 * @param[in]   crc Pointer to the CRC instance
 * @assumptions Note that D_CRC supports only CRC_CCITT mode, hence
 *              no configuration is applied for this instance.
 * @examplecode HAL_examples.c Sys_CRC_32InitValue_Example
 */
static inline void Sys_CRC_32InitValue(CRC_Type *crc)
{
    SYS_ASSERT(CRC_REF_VALID(CRC));
    if (crc == CRC)
    {
        crc->VALUE = CRC_32_INIT_VALUE;
    }
}

/**
 * @brief       Initialize CRC for CRC-CCITT
 * @param[in]   crc Pointer to the CRC instance
 * @assumptions CRC is configured to work in CRC-CCITT mode.
 * @examplecode HAL_examples.c Sys_CRC_CCITTInitValue_Example
 */
static inline void Sys_CRC_CCITTInitValue(CRC_Type *crc)
{
    SYS_ASSERT(CRC_REF_VALID(CRC));
    crc->VALUE = CRC_CCITT_INIT_VALUE;
}

/**
 * @brief       Retrieve current value from CRC.
 * @param[in]   crc Pointer to the CRC instance
 * @return      Current CRC value.
 * @examplecode HAL_examples.c Sys_CRC_GetCurrentValue_Example
 */
static inline uint32_t Sys_CRC_GetCurrentValue(const CRC_Type *crc)
{
    SYS_ASSERT(CRC_REF_VALID(CRC));
    return crc->VALUE;
}

/**
 * @brief       Initialize final CRC value.
 * @param[in]   crc Pointer to the CRC instance
 * @return      CRC final value.
 * @assumptions D_CRC only supports CRC-CCITT mode.
 *              Use Sys_CRC_GetCurrentValue instead.
 *              Returns initial value of CRC if D_CRC is passed or any other
 *              unknown instance.
 * @examplecode HAL_examples.c Sys_CRC_GetFinalValue_Example
 */
static inline uint32_t Sys_CRC_GetFinalValue(const CRC_Type *crc)
{
    SYS_ASSERT(CRC_REF_VALID(CRC));
    if (crc == CRC)
    {
        return crc->FINAL;
    }
#if D_CRC_PRESENT
    else if (crc == D_CRC)
    {
        return crc->VALUE;
    }
#endif    /* if D_CRC_PRESENT */
    else
    {
        /* Unknown CRC instance passed, return the initial value of this CRC*/
        return crc->VALUE;
    }
}

/**
 * @brief       Add data to the current CRC calculation, based on size.
 * @param[in]   crc   Pointer to the CRC instance
 * @param[in]   data  Data to add
 * @param[in]   size  Size of data to add, 1, 8, 16, 24, 32 are valid.
 * @examplecode HAL_examples.c Sys_CRC_Add_Example
 */
static inline void Sys_CRC_Add(CRC_Type *crc, uint32_t data, \
                               uint32_t size)
{
    SYS_ASSERT(CRC_REF_VALID(CRC));
    SYS_ASSERT(size == 1  ||
               size == 8  ||
               size == 16 ||
               size == 24 ||
               size == 32);
    *((&crc->ADD_1) + (size >> 3)) = data;
}

/**
 * @brief       Macro wrapper for Sys_Set_CRC_Config()
 *              Configure the CRC generator type, endianness of the
 *              input data, and standard vs non-standard CRC behavior
 * @param[in]   config CRC generator configuration; use
 *                     CRC_[CCITT | 32],
 *                     CRC_[BIG | LITTLE]_ENDIAN,
 *                     CRC_BIT_ORDER_[STANDARD | NON_STANDARD],
 *                     CRC_FINAL_REVERSE_[STANDARD | NON_STANDARD], and
 *                     CRC_FINAL_XOR_[STANDARD | NON_STANDARD]
 * @assumptions Note that D_CRC supports only CRC_CCITT mode, hence
 *              no configuration is applied for this instance.
 * @examplecode HAL_examples.c SYS_CRC_CONFIG_Example
 */
#define SYS_CRC_CONFIG(config) Sys_Set_CRC_Config(CRC, (config))

/**
 * @brief       Macro wrapper for Sys_CRC_32InitValue()
 *              Initialize CRC for CRC-32
 * @assumptions Note that D_CRC supports only CRC_CCITT mode, hence
 *              no configuration is applied for this instance.
 * @examplecode HAL_examples.c SYS_CRC_32INITVALUE_Example
 */
#define SYS_CRC_32INITVALUE() Sys_CRC_32InitValue(CRC)

/**
 * @brief       Macro wrapperr for Sys_CRC_CCITTInitValue()
 *              Initialize CRC for CRC-CCITT
 * @assumptions CRC is configured to work in CRC-CCITT mode.
 * @examplecode HAL_examples.c SYS_CRC_CCITTINITVALUE_Example
 */
#define SYS_CRC_CCITTINITVALUE() Sys_CRC_CCITTInitValue(CRC)

/**
 * @brief       Macro wrapper for Sys_CRC_GetCurrentValue()
 *              Initialize CRC for CRC-CCITT
 * @examplecode HAL_examples.c SYS_CRC_GETCURRENTVALUE_Example
 */
#define SYS_CRC_GETCURRENTVALUE() Sys_CRC_GetCurrentValue(CRC)

/**
 * @brief       Macro wrapper for Sys_CRC_GetFinalValue()
 *              Initialize final CRC value.
 * @return      CRC final value.
 * @assumptions D_I2C only supports CRC-CCITT mode.
 *              Use Sys_CRC_GetCurrentValue instead.
 *              Returns initial value of CRC if D_CRC is passed or any other
 *              unknown instance.
 * @examplecode HAL_examples.c SYS_CRC_GETFINALVALUE_Example
 */
#define SYS_CRC_GETFINALVALUE() Sys_CRC_GetFinalValue(CRC)

/**
 * @brief       Macro wrapper for Sys_CRC_Add()
 *              Add data to the current CRC calculation, based on size.
 * @param[in]   data  Data to add
 * @param[in]   size  Size of data to add, 1, 8, 16, 24, 32 are valid.
 * @examplecode HAL_examples.c SYS_CRC_ADD_Example
 */
#define SYS_CRC_ADD(data, size) Sys_CRC_Add(CRC, (data), (size))

/** @} */ /* End of the HALCRC group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* CRC_H */
