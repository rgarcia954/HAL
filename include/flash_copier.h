/**
 * @file flash_copier.h
 * @brief Hardware abstraction layer for Flash support functions
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

#ifndef FLASH_COPIER_H
#define FLASH_COPIER_H

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALFLASH Flash Copier
 *  Flash copier hardware abstraction layer
 * @{
 */

/**
 * @brief       Copy data from the flash memory to a RAM memory instance
 * @param[in]   flash      Pointer to the flash instance
 * @param[in]   src_addr   Source address in flash to copy data from
 * @param[in]   dest_addr  Destination address in RAM to copy data to
 * @param[in]   length     Number of words to copy
 * @param[in]   cpy_dest   Destination copier is CRC or memories; use
 *                         COPY_TO_[CRC | MEM], and
 *                         COPY_TO_[32 | 40]_BIT
 * @assumptions src_addr points to an address in flash memory
 *              dest_addr points to an address in RAM memory
 *              The flash copy does not need to be complete before
 *              returning
 *              If CRC is selected as the destination, dest_addr is
 *              ignored and 32-bit copy mode is selected automatically.
 */
void Sys_Flash_Copy(FLASH_Type *flash, uint32_t src_addr, uint32_t dest_addr,
                    uint32_t length, uint32_t cpy_dest);

/**
 * @brief       Compare data in the flash to a pre-specified value
 * @param[in]   flash      Pointer to the flash instance
 * @param[in]   cfg        Flash comparator configuration; use
 *                         COMP_MODE_[CONSTANT | CHBK]_BYTE,
 *                         COMP_ADDR_[DOWN | UP]_BYTE, and
 *                         COMP_ADDR_STEP_*_BYTE
 * @param[in]   addr       Base address of the area to verify
 * @param[in]   length     Number of words to verify
 * @param[in]   value      Value that the words read from flash will be
 *                         compared against
 * @param[in]   value_ecc  Value that the error-correction coding
 *                         bits from the extended words read from flash
 *                         will be compared against
 * @return      0 if comparison succeeded, 1 if the comparison failed.
 * @assumptions addr points to an address in flash memory
 */
uint32_t Sys_Flash_Compare(FLASH_Type *flash, uint32_t cfg, uint32_t addr,
                           uint32_t length, uint32_t value,
                           uint32_t value_ecc);

/**
 * @brief Calculate CRC of words in flash using flash copier
 * @param[in]   flash      Flash instance being used
 * @param[in]   addr       Address belonging to the flash instance
 * @param[in]   length     Total number of words used in CRC calculation
 * @param[out]  crc        CRC value calculated using CRC peripheral
 * @return      0 if calculation has succeeded, 1 if the calculation has
 *              failed.
 * @note Flash copier and CRC register are modified
 */
uint32_t Sys_Flash_CalculateCRC(FLASH_Type *flash, uint32_t addr, uint32_t length,
                                uint32_t *crc);

/** @} */ /* End of the HALFLASH group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* FLASH_COPIER_H */
