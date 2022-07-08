/**
 * @file spi.h
 * @brief Hardware abstraction layer for SPI
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

#ifndef SPI_H
#define SPI_H

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
/** @defgroup HALSPI SPI
 *  Serial Peripheral Interface (SPI) hardware abstraction layer
 *  @{
 */

#include <gpio.h>

/** Mask for the SPI_CFG register. */
#if RSL15_CID == 202
#define SPI_CONFIG_MASK  \
    ((1 << SPI_CFG_TX_DMA_ENABLE_Pos)           | \
     (1 << SPI_CFG_RX_DMA_ENABLE_Pos)           | \
     (1 << SPI_CFG_TX_END_INT_ENABLE_Pos)       | \
     (1 << SPI_CFG_TX_START_INT_ENABLE_Pos)     | \
     (1 << SPI_CFG_RX_INT_ENABLE_Pos)           | \
     (1 << SPI_CFG_CS_RISE_INT_ENABLE_Pos)      | \
     (1 << SPI_CFG_OVERRUN_INT_ENABLE_Pos)      | \
     (1 << SPI_CFG_UNDERRUN_INT_ENABLE_Pos)     | \
     (1 << SPI_CFG_MODE_Pos)                    | \
     SPI_CFG_MODE_Mask                          | \
     (1 << SPI_CFG_WORD_SIZE_Pos)               | \
     SPI_CFG_WORD_SIZE_Mask                     | \
     (1 << SPI_CFG_PRESCALE_Pos)                | \
     SPI_CFG_PRESCALE_Mask                      | \
     (1 << SPI_CFG_CLK_PHASE_Pos)               | \
     (1 << SPI_CFG_CLK_POLARITY_Pos)            | \
     (1 << SPI_CFG_SLAVE_Pos))
#else
#define SPI_CONFIG_MASK  \
    ((1 << SPI_CFG_TX_DMA_ENABLE_Pos)           | \
     (1 << SPI_CFG_RX_DMA_ENABLE_Pos)           | \
     (1 << SPI_CFG_TX_END_INT_ENABLE_Pos)       | \
     (1 << SPI_CFG_TX_START_INT_ENABLE_Pos)     | \
     (1 << SPI_CFG_RX_INT_ENABLE_Pos)           | \
     (1 << SPI_CFG_CS_RISE_INT_ENABLE_Pos)      | \
     (1 << SPI_CFG_OVERRUN_INT_ENABLE_Pos)      | \
     (1 << SPI_CFG_UNDERRUN_INT_ENABLE_Pos)     | \
     (1 << SPI_CFG_MODE_Pos)                    | \
     SPI_CFG_MODE_Mask                          | \
     (1 << SPI_CFG_WORD_SIZE_Pos)               | \
     SPI_CFG_WORD_SIZE_Mask                     | \
     (1 << SPI_CFG_PRESCALE_Pos)                | \
     SPI_CFG_PRESCALE_Mask                      | \
     (1 << SPI_CFG_CLK_POLARITY_Pos)            | \
     (1 << SPI_CFG_SLAVE_Pos))
#endif

/** Number of pads used for the SPI interface, for a single instance */
#define SPI_PADS_NUM 6

/**
 * @brief       Configure the specified SPI interface's operation and
 *              controller information
 * @param[in]   spi     Pointer to the SPI instance
 * @param[in]   config  Interface operation configuration; use
 *                      SPI_SELECT_[MASTER | SLAVE],
 *                      SPI_CLK_POLARITY_[NORMAL | INVERSE],
 *                      SPI_PRESCALE_*,
 *                      SPI_WORD_SIZE_*,
 *                      SPI_MODE_[SPI | DSPI | QSPI]
 *                      SPI_UNDERRUN_INT_[ENABLE | DISABLE],
 *                      SPI_OVERRRUN_INT_[ENABLE | DISABLE],
 *                      SPI_CS_RISE_INT_[ENABLE | DISABLE],
 *                      SPI_RX_INT_[ENABLE | DISABLE]
 *                      SPI_TX_INT_[ENABLE | DISABLE]
 *                      SPI_RX_DMA_[ENABLE | DISABLE]
 *                      SPI_TX_DMA_[ENABLE | DISABLE]
 * @examplecode HAL_examples.c Sys_SPI_Config_Example
 */
static inline void Sys_SPI_Config(SPI_Type *spi, uint32_t config)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    spi->CFG = config & SPI_CONFIG_MASK;
}

/**
 * @brief       Configure the SPI transfer information for the specified
 *              SPI instance
 * @param[in]   spi Pointer to the SPI instance
 * @param[in]   config  Interface transfer configuration; use
 *              SPI_ENABLE,
 *              SPI_DISABLE,
 *              SPI_RESET,
 *              SPI_START,
 *              SPI_MODE_READ_WRITE,
 *              SPI_MODE_READ,
 *              SPI_MODE_WRITE,
 *              SPI_MODE_NOP,
 *              SPI_CS_0,
 *              SPI_CS_1,
 * @examplecode HAL_examples.c Sys_SPI_TransferConfig_Example
 */
static inline void Sys_SPI_TransferConfig(SPI_Type *spi, uint32_t config)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    spi->CTRL = config;
}

/**
 * @brief       Generate clock and CS to read data from SPI interface
 * @param[in]   spi  Pointer to the SPI instance
 * @return      data Data read from the SPI interface
 * @assumptions SPI is configured as master mode and transfer operation mode
 *              is SPI_MODE_READ_WRITE(full duplex) or
 *              (SPI_MODE_READ) half duplex
 * @examplecode HAL_examples.c Sys_SPI_Read_Example
 */
static inline uint32_t Sys_SPI_Read(const SPI_Type *spi)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    return spi->RX_DATA;
}

/**
 * @brief       Generate clock and CS to write data to SPI interface
 * @param[in]   spi Pointer to the SPI instance
 * @param[in]   data Data to be sent over SPI
 * @assumptions SPI is configured as master mode and transfer operation mode
 *              is SPI_MODE_READ_WRITE(full duplex) or
 *              (SPI_MODE_WRITE) half duplex
 * @examplecode HAL_examples.c Sys_SPI_Write_Example
 */
static inline void Sys_SPI_Write(SPI_Type *spi, uint32_t data)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    spi->TX_DATA = data;
}

#ifndef NON_SECURE
/**
 * @brief       Configure four GPIOs for the specified SPI interface
 * @param[in]   spi Pointer to the SPI instance
 * @param[in]   slave  SPI master/slave configuration; use
 *              SPI*_SELECT_[MASTER | SLAVE]
 * @param[in]   cfg  GPIO pin configuration for the SPI pads
 * @param[in]   clk  GPIO to use as the SPI clock pad
 * @param[in]   cs   GPIO to use as the SPI chip select pad
 * @param[in]   seri GPIO to use as the SPI serial input pad
 * @param[in]   sero GPIO to use as the SPI serial output pad
 * @examplecode HAL_examples.c Sys_SPI_GPIOConfig_Example
 */
static inline void Sys_SPI_GPIOConfig(const SPI_Type *spi, uint32_t slave,
                                      uint32_t cfg, uint32_t clk, uint32_t cs,
                                      uint32_t seri, uint32_t sero)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    unsigned int diff = (spi - SPI);

    /* Configure the SPI master */
    if (slave == 0)
    {
        /* Configure SPI master output pads */
        SYS_GPIO_CONFIG(sero, (cfg | (GPIO_MODE_SPI0_IO0 +
                                      diff * SPI_PADS_NUM)));
        SYS_GPIO_CONFIG(clk,  (cfg | (GPIO_MODE_SPI0_CLK +
                                      diff * SPI_PADS_NUM)));
        SYS_GPIO_CONFIG(cs,   (cfg | (GPIO_MODE_SPI0_CS +
                                      diff * SPI_PADS_NUM)));

        /* Configure SERI for SPI master input */
        SYS_GPIO_CONFIG(seri, (cfg | GPIO_MODE_INPUT));
        GPIO->SRC_SPI_IO[diff] = ((GPIO->SRC_SPI_IO[diff] &
                                   !GPIO_SRC_SPI_IO_IO1_Mask) |
                                  ((seri << GPIO_SRC_SPI_IO_IO1_Pos) &
                                   GPIO_SRC_SPI_IO_IO1_Mask));
    }
    else
    {
        /* Configure SPI slave SERO output pad */
        SYS_GPIO_CONFIG(sero, (cfg | (GPIO_MODE_SPI0_IO1 +
                                      diff * SPI_PADS_NUM)));

        /* Configure SERI, CLK and CS pad inputs to SPI */
        SYS_GPIO_CONFIG(seri, (cfg | GPIO_MODE_INPUT));
        SYS_GPIO_CONFIG(clk,  (cfg | GPIO_MODE_INPUT));
        SYS_GPIO_CONFIG(cs,   (cfg | GPIO_MODE_INPUT));
        GPIO->SRC_SPI[diff] = ((clk << GPIO_SRC_SPI_CLK_Pos) & GPIO_SRC_SPI_CLK_Mask) |
                              ((cs << GPIO_SRC_SPI_CS_Pos) & GPIO_SRC_SPI_CS_Mask);
        GPIO->SRC_SPI_IO[diff] = ((GPIO->SRC_SPI_IO[diff] &
                                   !GPIO_SRC_SPI_IO_IO0_Mask) |
                                  ((seri << GPIO_SRC_SPI_IO_IO0_Pos) &
                                   GPIO_SRC_SPI_IO_IO0_Mask));
    }
}

/**
 * @brief       Configure four GPIOs for the specified SPI interface
 * @param[in]   spi  Pointer to the SPI instance
 * @param[in]   cfg  GPIO pin configuration for the SPI pads
 * @param[in]   clk  GPIO to use as the DSPI clock pad
 * @param[in]   cs   GPIO to use as the DSPI chip select pad
 * @param[in]   io0  GPIO to use as the DSPI io0
 * @param[in]   io1  GPIO to use as the DSPI io1
 * @examplecode HAL_examples.c Sys_DSPI_GPIOConfig_Example
 */
static inline void Sys_DSPI_GPIOConfig(const SPI_Type *spi, uint32_t cfg,
                                       uint32_t clk, uint32_t cs,
                                       uint32_t io0, uint32_t io1)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    unsigned int diff = (spi - SPI);

    /* Configure DSPI master output pads */
    SYS_GPIO_CONFIG(io0,  (cfg | (GPIO_MODE_SPI0_IO0 + diff * SPI_PADS_NUM)));
    SYS_GPIO_CONFIG(io1,  (cfg | (GPIO_MODE_SPI0_IO1 + diff * SPI_PADS_NUM)));

    /* Configure DSPI master IO0 and IO1 */
    GPIO->SRC_SPI_IO[diff] = ((io0 << GPIO_SRC_SPI_IO_IO0_Pos) & GPIO_SRC_SPI_IO_IO0_Mask) |
                             ((io1 << GPIO_SRC_SPI_IO_IO1_Pos) & GPIO_SRC_SPI_IO_IO1_Mask);

    /* Configure DSPI master clock and chip select */
    SYS_GPIO_CONFIG(clk,  (cfg | (GPIO_MODE_SPI0_CLK + diff * SPI_PADS_NUM)));
    SYS_GPIO_CONFIG(cs,   (cfg | (GPIO_MODE_SPI0_CS + diff * SPI_PADS_NUM)));
}

/**
 * @brief       Configure six GPIOs for the specified SPI interface
 * @param[in]   spi Pointer to the SPI instance
 * @param[in]   cfg  GPIO pin configuration for the SPI pads
 * @param[in]   clk  GPIO to use as the QSPI clock pad
 * @param[in]   cs   GPIO to use as the QSPI chip select pad
 * @param[in]   io0  GPIO to use as the QSPI io0
 * @param[in]   io1  GPIO to use as the QSPI io1
 * @param[in]   io2  GPIO to use as the QSPI io2
 * @param[in]   io3  GPIO to use as the QSPI io3
 * @examplecode HAL_examples.c Sys_QSPI_GPIOConfig_Example
 */
static inline void Sys_QSPI_GPIOConfig(const SPI_Type *spi, uint32_t cfg,
                                       uint32_t clk, uint32_t cs,
                                       uint32_t io0, uint32_t io1,
                                       uint32_t io2,  uint32_t io3)
{
    SYS_ASSERT(SPI_REF_VALID(spi));
    unsigned int diff = (spi - SPI);

    /* Configure QSPI master output pads */
    SYS_GPIO_CONFIG(io0,  (cfg | (GPIO_MODE_SPI0_IO0 + diff * SPI_PADS_NUM)));
    SYS_GPIO_CONFIG(io1,  (cfg | (GPIO_MODE_SPI0_IO1 + diff * SPI_PADS_NUM)));
    SYS_GPIO_CONFIG(io2,  (cfg | (GPIO_MODE_SPI0_IO2 + diff * SPI_PADS_NUM)));
    SYS_GPIO_CONFIG(io3,  (cfg | (GPIO_MODE_SPI0_IO3 + diff * SPI_PADS_NUM)));

    /* Configure SERI, for QSPI master input */
    GPIO->SRC_SPI_IO[diff] = ((io0 << GPIO_SRC_SPI_IO_IO0_Pos) & GPIO_SRC_SPI_IO_IO0_Mask) |
                             ((io1 << GPIO_SRC_SPI_IO_IO1_Pos) & GPIO_SRC_SPI_IO_IO1_Mask) |
                             ((io2 << GPIO_SRC_SPI_IO_IO2_Pos) & GPIO_SRC_SPI_IO_IO2_Mask) |
                             ((io3 << GPIO_SRC_SPI_IO_IO3_Pos) & GPIO_SRC_SPI_IO_IO3_Mask);

    /* Configure QSPI master */
    SYS_GPIO_CONFIG(clk,  (cfg | (GPIO_MODE_SPI0_CLK + diff * SPI_PADS_NUM)));
    SYS_GPIO_CONFIG(cs,   (cfg | (GPIO_MODE_SPI0_CS + diff * SPI_PADS_NUM)));
}
#endif /* ifndef NON_SECURE */

/**
 * @brief       Configure the specified SPI interface's operation and
 *              controller information
 * @param[in]   config Interface operation configuration; use
 *                     SPI_SELECT_[MASTER | SLAVE],
 *                     SPI_CLK_POLARITY_[NORMAL | INVERSE],
 *                     SPI_PRESCALE_*,
 *                     SPI_WORD_SIZE_*,
 *                     SPI_MODE_[SPI | DSPI | QSPI]
 *                     SPI_UNDERRUN_INT_[ENABLE | DISABLE],
 *                     SPI_OVERRRUN_INT_[ENABLE | DISABLE],
 *                     SPI_CS_RISE_INT_[ENABLE | DISABLE],
 *                     SPI_RX_INT_[ENABLE | DISABLE]
 *                     SPI_TX_INT_[ENABLE | DISABLE]
 *                     SPI_RX_DMA_[ENABLE | DISABLE]
 *                     SPI_TX_DMA_[ENABLE | DISABLE]
 * @examplecode HAL_examples.c SYS_SPI_CONFIG_Example
 */
#define SYS_SPI_CONFIG(config) Sys_SPI_Config(SPI, (config))

/**
 * @brief       Configure the SPI transfer information for the specified
 *              SPI instance
 * @param[in]   config Interface transfer configuration; use
 *                     SPI_ENABLE,
 *                     SPI_DISABLE,
 *                     SPI_RESET,
 *                     SPI_START,
 *                     SPI_MODE_READ_WRITE,
 *                     SPI_MODE_READ,
 *                     SPI_MODE_WRITE,
 *                     SPI_MODE_NOP,
 *                     SPI_CS_0,
 *                     SPI_CS_1,
 * @examplecode HAL_examples.c SYS_SPI_TRANSFERCONFIG_Example
 */
#define SYS_SPI_TRANSFERCONFIG(config) Sys_SPI_TransferConfig(SPI, (config))

/**
 * @brief       Generate clock and CS to read data from SPI interface
 * @return      Data read from the SPI interface
 * @assumptions SPI is configured as master mode and transfer operation mode
 *              is SPI_MODE_READ_WRITE(full duplex) or
 *              (SPI_MODE_READ) half duplex
 * @examplecode HAL_examples.c SYS_SPI_READ_Example
 */
#define SYS_SPI_READ() Sys_SPI_Read(SPI)

/**
 * @brief       Generate clock and CS to write data to SPI interface
 * @param[in]   data Data to be sent over SPI
 * @assumptions SPI is configured as master mode and transfer operation mode
 *              is SPI_MODE_READ_WRITE(full duplex) or
 *              (SPI_MODE_WRITE) half duplex
 * @examplecode HAL_examples.c SYS_SPI_WRITE_Example
 */
#define SYS_SPI_WRITE(data) Sys_SPI_Write(SPI, (data))

/**
 * @brief       Configure four GPIOs for the SPI0 interface
 * @param[in]   slave   SPI master/slave configuration; use
 *                      SPI*_SELECT_[MASTER | SLAVE]
 * @param[in]   cfg     GPIO pin configuration for the SPI pads
 * @param[in]   clk     GPIO to use as the SPI clock pad
 * @param[in]   cs      GPIO to use as the SPI chip select pad
 * @param[in]   seri    GPIO to use as the SPI serial input pad
 * @param[in]   sero    GPIO to use as the SPI serial output pad
 * @examplecode HAL_examples.c SYS_SPI_GPIOCONFIG_Example
 */
#define SYS_SPI_GPIOCONFIG(slave, cfg, clk, cs, seri, sero) \
    Sys_SPI_GPIOConfig(SPI, (slave), (cfg), (clk), (cs), (seri), (sero))

/**
 * @brief       Configure four GPIOs for the SPI0 interface for DSPI
 * @param[in]   cfg     GPIO pin configuration for the SPI pads
 * @param[in]   clk     GPIO to use as the DSPI clock pad
 * @param[in]   cs      GPIO to use as the DSPI chip select pad
 * @param[in]   io0     GPIO to use as the DSPI io0
 * @param[in]   io1     GPIO to use as the DSPI io1
 * @examplecode HAL_examples.c SYS_DSPI_GPIOCONFIG_Example
 */
#define SYS_DSPI_GPIOCONFIG(cfg, clk, cs, io0, io1) \
    Sys_DSPI_GPIOConfig(SPI, (cfg), (clk), (cs), (io0), (io1))

/**
 * @brief       Configure six GPIOs for the SPI0 interface for QSPI
 *              cfg     GPIO pin configuration for the SPI pads
 *              clk     GPIO to use as the QSPI clock pad
 *              cs      GPIO to use as the QSPI chip select pad
 *              io0     GPIO to use as the QSPI io0
 *              io1     GPIO to use as the QSPI io1
 *              io2     GPIO to use as the QSPI io2
 *              io3     GPIO to use as the QSPI io3
 * @examplecode HAL_examples.c SYS_QSPI_GPIOCONFIG_Example
 */
#define SYS_QSPI_GPIOCONFIG(cfg, clk, cs, io0, io1, io2, io3) \
    Sys_QSPI_GPIOConfig(SPI, (cfg), (clk), (cs), (io0), (io1), \
                        (io2), (io3))

/** @} */ /* End of the HALSPI group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* SPI_H */
