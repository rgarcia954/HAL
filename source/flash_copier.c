/**
 * @file flash_copier.c
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

#include <hw.h>

void Sys_Flash_Copy(FLASH_Type *flash, uint32_t src_addr, uint32_t dest_addr, uint32_t length,
                    uint32_t cpy_dest)
{
    /* Wait for the flash copier to be idle */
    while ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_BUSY_Pos)) == COPY_BUSY);

    /* Setup the source, destination, and length of the copy */
    flash->COPY_SRC_ADDR_PTR = src_addr;
    flash->COPY_DST_ADDR_PTR = dest_addr;
    flash->COPY_WORD_CNT = length;

    /* Configure the copy configuration register */
#if defined (RSL15_CID) && (RSL15_CID == 202)
    flash->COPY_CFG =   (cpy_dest | COPY_MODE);
#else
    flash->COPY_CFG =   (cpy_dest | COPY_MODE | COPY_TO_32BIT);
#endif

    /* Start the copy */
    flash->COPY_CTRL |= COPY_START;
}

uint32_t Sys_Flash_Compare(FLASH_Type *flash, uint32_t cfg, uint32_t addr, uint32_t length,
                           uint32_t value, uint32_t value_ecc)
{
    /* Wait for the flash copier to be idle */
    while ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_BUSY_Pos)) == COPY_BUSY);

    /* Configure the flash comparator */
    flash->COPY_CFG = (cfg | COMPARATOR_MODE);

    /* Setup the source and length of the comparison, and the data value to
     * compare against */
    flash->COPY_SRC_ADDR_PTR = addr;
    flash->COPY_WORD_CNT = length;
    flash->DATA[0] =  value;
    flash->DATA[1] =  value_ecc;

    /* Start the comparison */
    flash->COPY_CTRL |= COPY_START;

    /* Wait for the flash copier to be idle, and return the error code */
    while ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_BUSY_Pos)) == COPY_BUSY);

    return ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_ERROR_Pos)) == COPY_ERROR);
}

uint32_t Sys_Flash_CalculateCRC(FLASH_Type *flash, uint32_t addr, uint32_t length,
                                uint32_t *crc)
{
    /* Wait for the flash copier to be idle */
    while ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_BUSY_Pos)) == COPY_BUSY);

    /* Setup the source, destination, and length of the copy */
    flash->COPY_SRC_ADDR_PTR = addr;
    flash->COPY_WORD_CNT = length;

    /* Configure the copy configuration register */
    flash->COPY_CFG =   (COPY_MODE | COPY_TO_CRC);

    /*Initialize the CRC config block*/
    SYS_CRC_CONFIG(CRC_32 | CRC_LITTLE_ENDIAN);

    /*Initialize the CRC init value*/
    SYS_CRC_32INITVALUE();

    /* Start the copy */
    flash->COPY_CTRL |= COPY_START;

    /* Wait for the flash copier to be idle, and return the error code */
    while ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_BUSY_Pos)) == COPY_BUSY);

    /* Get the final CRC computed value */
    *crc = SYS_CRC_GETFINALVALUE();

    return ((flash->COPY_CTRL & (0x1U << FLASH_COPY_CTRL_ERROR_Pos)) == COPY_ERROR);
}
