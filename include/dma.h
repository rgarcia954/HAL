/**
 * @file dma.h
 * @brief DMA interface support
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

#ifndef DMA_H
#define DMA_H

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
/** @defgroup HALDMA Direct Memory Access
 *  Direct Memory Access (DMA) hardware abstraction layer
 *  @{
 */

/**
 * @brief       Configure the DMA channels for a data transfer
 * @param[in]   dma Pointer to the DMA instance
 * @param[in]   cfg             Configuration of the DMA transfer
 *                              behavior; use
 *                              DMA_[LITTLE | BIG]_ENDIAN,
 *                              [DEST | SRC]_TRANS_LENGTH_SEL,
 *                              DMA_PRIORITY_[0 | 1 | 2 | 3],
 *                              DMA_SRC_*
 *                              DMA_DEST_*
 *                              WORD_SIZE_*,
 *                              DMA_SRC_ADDR_*,
 *                              DMA_DEST_ADDR_*,
 *                              DMA_SRC_ADDR_LSB_TOGGLE_[DISABLE | ENABLE],
 *                              DMA_CNT_INT_[DISABLE | ENABLE],
 *                              DMA_COMPLETE_INT_[DISABLE | ENABLE]
 * @param[in]   transferLength  Configuration of the DMA transfer length
 * @param[in]   counterInt      Configuration of when the counter
 *                              interrupt will occur during the transfer
 * @param[in]   srcAddr         Base source address for the DMA transfer
 * @param[in]   destAddr        Base destination address for the DMA
 *                              transfer
 * @examplecode HAL_examples.c Sys_DMA_ChannelConfig_Example
 */
static inline void Sys_DMA_ChannelConfig(DMA_Type *dma, uint32_t cfg,
                                         uint32_t transferLength,
                                         uint32_t counterInt,
                                         uint32_t srcAddr,
                                         uint32_t destAddr)
{
    SYS_ASSERT(DMA_REF_VALID(dma));

    /* Setup the base addresses for the source and destination */
    dma->SRC_ADDR = srcAddr;
    dma->DEST_ADDR = destAddr;

    /* Configure the DMA channel */
    dma->CFG0 =  cfg &
                (((uint32_t)0x1U << DMA_CFG0_BYTE_ORDER_Pos) |
                 ((uint32_t)0x1U << DMA_CFG0_SRC_DEST_TRANS_LENGTH_SEL_Pos) |
                 DMA_CFG0_CHANNEL_PRIORITY_Mask |
                 DMA_CFG0_SRC_SELECT_Mask |
                 DMA_CFG0_DEST_SELECT_Mask |
                 DMA_CFG0_SRC_DEST_WORD_SIZE_Mask |
                 DMA_CFG0_SRC_ADDR_STEP_Mask |
                 DMA_CFG0_DEST_ADDR_STEP_Mask |
                 ((uint32_t)0x1U << DMA_CFG0_SRC_ADDR_LSB_TOGGLE_Pos) |
                 ((uint32_t)0x1U << DMA_CFG0_DEST_ADDR_LSB_TOGGLE_Pos) |
                 ((uint32_t)0x1U << DMA_CFG0_CNT_INT_ENABLE_Pos) |
                 ((uint32_t)0x1U << DMA_CFG0_COMPLETE_INT_ENABLE_Pos));

    /* Setup the transfer length and transfer counter interrupt setting */
    dma->CFG1 = (((transferLength << DMA_CFG1_TRANSFER_LENGTH_Pos) &
                  DMA_CFG1_TRANSFER_LENGTH_Mask) |
                 ((counterInt << DMA_CFG1_INT_TRANSFER_LENGTH_Pos) &
                  DMA_CFG1_INT_TRANSFER_LENGTH_Mask));
}

/**
 * @brief       Configure the DMA channels for a data transfer
 * @param[in]   dma   Pointer to the DMA instance
 * @param[in]   mode  Enable mode of operation of the DMA Channel;
 *                    use
 *                    DMA_[DISABLE | ENABLE | DMA_ENABLE_WRAP |
 *                    DMA_ENABLE_WRAP_RESTART | DMA_TRIGGER
 *                    DMA_TRIGGER_WRAP |
 *                    DMA_TRIGGER_WRAP_RESTART]
 * @examplecode HAL_examples.c Sys_DMA_Mode_Enable_Example
 */
static inline void Sys_DMA_Mode_Enable(DMA_Type *dma, uint32_t mode)
{
    SYS_ASSERT(DMA_REF_VALID(dma));
    dma->CTRL = ((dma->CTRL & ~DMA_CTRL_MODE_ENABLE_Mask) |
                 (mode & DMA_CTRL_MODE_ENABLE_Mask));
}

/**
 * @brief       Get the status register of the DMA instance
 * @param[in]   dma   Pointer to the DMA instance
 * @return The DMA_STATUS of the DMA instance.
 */
static inline uint32_t Sys_DMA_Get_Status(DMA_Type *dma)
{
    SYS_ASSERT(DMA_REF_VALID(dma));
    return dma->STATUS;
}

/**
 * @brief Writes to the CNT_INT_CLEAR, COMPLETE_INT_CLEAR,
 *        or SRC_BUFFER_FILL_LVL_WR
 * @param[in]   dma   Pointer to the DMA instance
 * @param[in]   ctrl  Data to be written to the DMA_SATUS register
 * @return The DMA_STATUS of the DMA instance
 */
static inline void Sys_DMA_Clear_Status(DMA_Type *dma, uint32_t ctrl)
{
    SYS_ASSERT(DMA_REF_VALID(dma));
    dma->STATUS = ctrl;
}

/**
 * @brief       Sets the DMA_CTRL of the DMA instance
 * @param[in]   dma   Pointer to the DMA instance
 * @param[in]   ctrl  Data to be written to the DMA_CTRL register
 */
static inline void Sys_DMA_Set_Ctrl(DMA_Type *dma, uint32_t ctrl)
{
    SYS_ASSERT(DMA_REF_VALID(dma));
    dma->CTRL = ctrl;
}

/**
 * @brief       Macro wrapper for Sys_DMA_ChannelConfig()
 *              Configure the DMA channels for a data transfer
 * @param[in]   cfg              Configuration of the DMA transfer
 *                               behavior; use
 *                               DMA_[LITTLE | BIG]_ENDIAN,
 *                               [DEST | SRC]_TRANS_LENGTH_SEL,
 *                               DMA_PRIORITY_[0 | 1 | 2 | 3],
 *                               DMA_SRC_*
 *                               DMA_DEST_*
 *                               WORD_SIZE_*,
 *                               DMA_SRC_ADDR_*,
 *                               DMA_DEST_ADDR_*,
 *                               DMA_SRC_ADDR_LSB_TOGGLE_[DISABLE | ENABLE],
 *                               DMA_CNT_INT_[DISABLE | ENABLE],
 *                               DMA_COMPLETE_INT_[DISABLE | ENABLE]
 * @param[in]    transferlength  Configuration of the DMA transfer length
 * @param[in]    counterInt      Configuration of when the counter
 *                               interrupt will occur during the transfer
 * @param[in]    srcAddr         Base source address for the DMA transfer
 * @param[in]    destAddr        Base destination address for the DMA
 *                               transfer
 * @examplecode HAL_examples.c SYS_DMA_CHANNELCONFIG_Example
 */
#define SYS_DMA_CHANNELCONFIG(cfg, transferlength, counterInt, srcAddr, \
                              destAddr) Sys_DMA_ChannelConfig(DMA, cfg, \
                                                              transferlength, counterInt, srcAddr, \
                                                              destAddr)

/**
 * @brief       Macro wrapper for Sys_DMA_Mode_Enable()
 *              Configure the DMA channels for a data transfer
 * @param[in]   mode Enable mode of operation of the DMA Channel;
 *                   use
 *                   DMA_[DISABLE | ENABLE | DMA_ENABLE_WRAP |
 *                   DMA_ENABLE_WRAP_RESTART | DMA_TRIGGER
 *                   DMA_TRIGGER_WRAP |
 *                   DMA_TRIGGER_WRAP_RESTART]
 * @examplecode HAL_examples.c SYS_DMA_MODE_ENABLE_Example
 */
#define SYS_DMA_MODE_ENABLE(mode) Sys_DMA_Mode_Enable(DMA, (mode))

/** @} */ /* End of the HALDMA group */
/** @} */ /* End of the HAL group */

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* DMA_H */
