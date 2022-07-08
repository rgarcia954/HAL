/**
 * @file i2c.h
 * @brief Hardware abstraction layer for I2C
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

#ifndef I2C_H
#define I2C_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <gpio.h>

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALI2C I2C
 *  Inter-Integrated Circuit (I2C) hardware abstraction layer
 *  @{
 */

/** Mask for the I2C_CFG register. */
#define I2C_CONFIG_MASK     \
    (((uint32_t)1U << I2C_CFG_CONNECT_IN_STANDBY_Pos) | \
     ((uint32_t)1U << I2C_CFG_TX_DMA_ENABLE_Pos) | \
     ((uint32_t)1U << I2C_CFG_RX_DMA_ENABLE_Pos) | \
     ((uint32_t)1U << I2C_CFG_TX_INT_ENABLE_Pos) | \
     ((uint32_t)1U << I2C_CFG_RX_INT_ENABLE_Pos) | \
     ((uint32_t)1U << I2C_CFG_BUS_ERROR_INT_ENABLE_Pos) | \
     (1U << I2C_CFG_OVERRUN_INT_ENABLE_Pos) | \
     (1U << I2C_CFG_STOP_INT_ENABLE_Pos) | \
     (1U << I2C_CFG_AUTO_ACK_ENABLE_Pos) | \
     I2C_CFG_SLAVE_PRESCALE_Mask | \
     I2C_CFG_MASTER_PRESCALE_Mask | \
     I2C_CFG_SLAVE_ADDRESS_Mask | \
     (1U << I2C_CFG_SLAVE_Pos))

/** Number of pads used for the I2C interface, for a single instance */
#define I2C_PADS_NUM        2U

#ifndef NON_SECURE
/**
 * @brief       Configure two GPIOs for the specified I2C interface
 * @param[in]   i2c     Pointer to the I2C instance
 * @param[in]   config  GPIO pin configuration for the I2C pads
 * @param[in]   scl     GPIO to use as the I2C transmit pad; use an integer
 * @param[in]   sda     GPIO to use as the I2C receive pad; use an integer
 * @examplecode HAL_examples.c Sys_I2C_GPIOConfig_Example
 */
static inline void Sys_I2C_GPIOConfig(const I2C_Type *i2c, \
                                      uint32_t config, \
                                      uint32_t scl, uint32_t sda)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));

    unsigned int diff = (i2c - I2C);

    /* Apply GPIO pad configuration */
    SYS_GPIO_CONFIG(scl, (config | (GPIO_MODE_I2C0_SCL +
                                    diff * I2C_PADS_NUM)));
    SYS_GPIO_CONFIG(sda, (config | (GPIO_MODE_I2C0_SDA +
                                    diff * I2C_PADS_NUM)));

    /* Apply GPIO source configuration */
    GPIO->SRC_I2C[diff] = (((scl << GPIO_SRC_I2C_SCL_Pos) &
                            GPIO_SRC_I2C_SCL_Mask) |
                           ((sda << GPIO_SRC_I2C_SDA_Pos) &
                            GPIO_SRC_I2C_SDA_Mask));
}
#endif /* ifndef NON_SECURE */

/**
 * @brief       Apply I2C Master mode related configuration
 * @param[in]   i2c     Pointer to the I2C instance
 * @param[in]   config  I2C configurations for master mode; use
 *                      I2C_[CONNECT | DISCONNECT]_IN_STANDBY,
 *                      I2C_TX_DMA_[ENABLE | DISABLE]
 *                      I2C_RX_DMA_[ENABLE | DISABLE]
 *                      I2C_TX_INT_[ENABLE | DISABLE]
 *                      I2C_RX_INT_[ENABLE | DISABLE]
 *                      I2C_BUS_ERROR_INT_[ENABLE | DISABLE]
 *                      I2C_OVERRUN_INT_[ENABLE | DISABLE]
 *                      I2C_STOP_INT_[ENABLE | DISABLE]
 *                      I2C_AUTO_ACK_[ENABLE | DISABLE]
 *                      I2C_MASTER_PRESCALE_*,
 *                      I2C_SLAVE_PRESCALE_*,
 *                      a slave address constant shifted to
 *                      I2C_CTRL0_SLAVE_ADDRESS_Pos,
 *                      I2C_SLAVE_[ENABLE | DISABLE]
 * @examplecode HAL_examples.c Sys_I2C_Config_Example
 */
static inline void Sys_I2C_Config(I2C_Type *i2c, uint32_t config)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->CFG = config & I2C_CONFIG_MASK;
}

/**
 * @brief       Send slave address on the bus with a read request
 * @param[in]   i2c   Pointer to the I2C instance
 * @param[in]   addr  I2C address to use for write transaction
 * @examplecode HAL_examples.c Sys_I2C_StartRead_Example
 */
static inline void Sys_I2C_StartRead(I2C_Type *i2c, uint32_t addr)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->ADDR_START = I2C_START_READ |
                      ((addr << I2C_ADDR_START_ADDRESS_Pos) &
                       I2C_CFG_SLAVE_ADDRESS_Mask);
}

/**
 * @brief       Send slave address on the bus with a write request
 * @param[in]   i2c   I2C instance number
 * @param[in]   addr  I2C address to use for write transaction
 * @examplecode HAL_examples.c Sys_I2C_StartWrite_Example
 */
static inline void Sys_I2C_StartWrite(I2C_Type *i2c, uint32_t addr)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->ADDR_START = ((addr << I2C_ADDR_START_ADDRESS_Pos) &
                       I2C_CFG_SLAVE_ADDRESS_Mask);
}

/**
 * @brief       Issue a ACK on the I2C interface
 * @param[in]   i2c Pointer to the I2C instance
 * @examplecode HAL_examples.c Sys_I2C_ACK_Example
 */
static inline void Sys_I2C_ACK(I2C_Type *i2c)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->CTRL = I2C_ACK;
}

/**
 * @brief       Issue a NACK on the I2C interface
 * @param[in]   i2c Pointer to the I2C instance
 * @examplecode HAL_examples.c Sys_I2C_NACK_Example
 */
static inline void Sys_I2C_NACK(I2C_Type *i2c)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->CTRL = I2C_NACK;
}

/**
 * @brief       Indicate that the current data is the last byte
 * @param[in]   i2c Pointer to the I2C instance
 * @examplecode HAL_examples.c Sys_I2C_LastData_Example
 */
static inline void Sys_I2C_LastData(I2C_Type *i2c)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->CTRL = I2C_LAST_DATA;
}

/**
 * @brief       Reset the I2C interface
 * @param[in]   i2c Pointer to the I2C instance
 * @examplecode HAL_examples.c Sys_I2C_Reset_Example
 */
static inline void Sys_I2C_Reset(I2C_Type *i2c)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->CTRL = I2C_RESET;
}

/**
 * @brief       Issue a NACK followed by a Stop condition on I2C bus
 * @param[in]   i2c Pointer to the I2C instance
 * @examplecode HAL_examples.c Sys_I2C_NackAndStop_Example
 */
static inline void Sys_I2C_NackAndStop(I2C_Type *i2c)
{
    SYS_ASSERT(I2C_REF_VALID(i2c));
    i2c->CTRL =  (I2C_NACK | I2C_STOP);
}

/**
 * @brief       Macro wrapper for Sys_I2C_GPIOConfig()
 *              Configure two GPIOs for the specified I2C interface
 * @param[in]   config GPIO pin configuration for the I2C pads
 * @param[in]   scl    GPIO to use as the I2C transmit pad; use an integer
 * @param[in]   sda    GPIO to use as the I2C receive pad; use an integer
 * @examplecode HAL_examples.c SYS_I2C_GPIOCONFIG_Example
 */
#define SYS_I2C_GPIOCONFIG(config, scl, sda) \
    Sys_I2C_GPIOConfig(I2C, (config), (scl), (sda))

/**
 * @brief       Macro wrapper for Sys_I2C_Config()
 *              Apply I2C Master mode related configuration
 * @param[in]   config I2C configurations for master mode; use
 *                     I2C_[CONNECT | DISCONNECT]_IN_STANDBY,
 *                     I2C_TX_DMA_[ENABLE | DISABLE]
 *                     I2C_RX_DMA_[ENABLE | DISABLE]
 *                     I2C_TX_INT_[ENABLE | DISABLE]
 *                     I2C_RX_INT_[ENABLE | DISABLE]
 *                     I2C_BUS_ERROR_INT_[ENABLE | DISABLE]
 *                     I2C_OVERRUN_INT_[ENABLE | DISABLE]
 *                     I2C_STOP_INT_[ENABLE | DISABLE]
 *                     I2C_AUTO_ACK_[ENABLE | DISABLE]
 *                     I2C_MASTER_PRESCALE_*,
 *                     I2C_SLAVE_PRESCALE_*,
 *                     a slave address constant shifted to
 *                     I2C_CTRL0_SLAVE_ADDRESS_Pos,
 *                     I2C_SLAVE_[ENABLE | DISABLE]
 * @examplecode HAL_examples.c SYS_I2C_CONFIG_Example
 */
#define SYS_I2C_CONFIG(config) Sys_I2C_Config(I2C, (config))

/**
 * @brief       Macro wrapper for Sys_I2C_StartRead()
 *              Send slave address on the bus with a read request
 * @param[in]   addr I2C address to use for write transaction
 * @examplecode HAL_examples.c SYS_I2C_STARTREAD_Example
 */
#define SYS_I2C_STARTREAD(addr) Sys_I2C_StartRead(I2C, (addr))

/**
 * @brief       Macro wrapper for Sys_I2C_StartWrite()
 *              Send slave address on the bus with a write request
 * @param[in]   addr I2C address to use for write transaction
 * @examplecode HAL_examples.c SYS_I2C_STARTWRITE_Example
 */
#define SYS_I2C_STARTWRITE(addr) Sys_I2C_StartWrite(I2C, (addr))

/**
 * @brief       Macro wrapper for Sys_I2C_ACK()
 *              Issue an ACK on the I2C interface
 * @examplecode HAL_examples.c SYS_I2C_ACK_Example
 */
#define SYS_I2C_ACK() Sys_I2C_ACK(I2C)

/**
 * @brief       Macro wrapper for Sys_I2C_NACK()
 *              Issue a NACK on the I2C interface
 * @examplecode HAL_examples.c SYS_I2C_NACK_Example
 */
#define SYS_I2C_NACK() Sys_I2C_NACK(I2C)

/**
 * @brief       Macro wrapper for Sys_I2C_LastData()
 *              Indicate that the current data is the last byte
 * @examplecode HAL_examples.c SYS_I2C_LASTDATA_Example
 */
#define SYS_I2C_LASTDATA() Sys_I2C_LastData(I2C)

/**
 * @brief       Macro wrapper for Sys_I2C_Reset()
 *              Reset the I2C interface
 * @examplecode HAL_examples.c SYS_I2C_RESET_Example
 */
#define SYS_I2C_RESET() Sys_I2C_Reset(I2C)

/**
 * @brief       Macro wrapper for Sys_I2C_NackAndStop()
 *              Issue a NACK followed by a Stop condition on I2C bus
 * @examplecode HAL_examples.c SYS_I2C_NACKANDSTOP_Example
 */
#define SYS_I2C_NACKANDSTOP() Sys_I2C_NackAndStop(I2C)

/** @} */ /* End of the HALI2C group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* I2C_H */
