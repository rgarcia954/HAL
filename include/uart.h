/**
 * @file uart.h
 * @brief Hardware abstraction layer for UART
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
#ifndef UART_H_
#define UART_H_

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
/** @defgroup HALUART UART
 *  Universal Asynchronous Receiver/Transmitter (UART) hardware abstraction layer
 *  @{
 */

/** The number of input GPIO pad configurations for a UART interface (1 per
 * instance) */
#define UART_PADS_NUM                   1

#ifndef NON_SECURE
/**
 * @brief       Configure two GPIOs for the specified UART interface
 * @param[in]   uart    Pointer to the UART instance
 * @param[in]   cfg     GPIO pin configuration for the UART pads
 * @param[in]   pad_tx  GPIO to use as the UART transmit pad
 * @param[in]   pad_rx  GPIO to use as the UART receive pad
 * @examplecode HAL_examples.c Sys_UART_GPIOConfig_Example
 */
static inline void Sys_UART_GPIOConfig(const UART_Type *uart, uint32_t cfg,
                                       uint32_t pad_tx, uint32_t pad_rx)
{
    SYS_ASSERT(UART_REF_VALID(uart));

    unsigned int diff = (uart - UART);

    SYS_GPIO_CONFIG(pad_tx, (cfg | (GPIO_MODE_UART0_TX +
                                    diff * UART_PADS_NUM)));
    SYS_GPIO_CONFIG(pad_rx, (cfg | GPIO_MODE_INPUT));

    GPIO->SRC_UART[diff] = (pad_rx << GPIO_SRC_UART_RX_Pos) & GPIO_SRC_UART_RX_Mask;
}
#endif /* ifndef NON_SECURE */

/**
 * @brief        Derive the UARTCLK frequency from the clock configuration registers
 * @examplecode  HAL_examples.c Sys_UART_GetClk_Example
 */
static inline uint32_t Sys_UART_GetClk(void)
{
    return (SystemCoreClock/
                (((CLK->DIV_CFG0 & 
                    CLK_DIV_CFG0_UARTCLK_PRESCALE_Mask) >> 
                        CLK_DIV_CFG0_UARTCLK_PRESCALE_Pos) + 
                            0x01U));
}

/**
 * @brief       Configure and enable a UART interface.
 * @param[in]   uart         Pointer to the UART instance
 * @param[in]   uart_clk_hz  UART clock speed in hertz
 * @param[in]   baud         Baud rate to which UART* is configured
 * @param[in]   config       DMA and interrupt mode enable; use
 *                           UART_TX_DMA_[ENABLE | DISABLE]
 *                           UART_RX_DMA_[ENABLE | DISABLE]
 *                           UART_TX_INT_[ENABLE | DISABLE]
 *                           UART_RX_INT_[ENABLE | DISABLE]
 *                           UART_OVERRUN_INT_[ENABLE | DISABLE]
 * @examplecode HAL_examples.c Sys_UART_Config_Example
 */
void Sys_UART_Config(UART_Type *uart, uint32_t uart_clk_hz, uint32_t baud,
                     uint32_t config);

/**
 * @brief       Macro wrapper for Sys_UART_GPIOConfig().
 *              Configure two GPIOs for the specified UART interface.
 * @param[in]   cfg     GPIO pin configuration for the UART pads
 * @param[in]   pad_tx  GPIO to use as the UART transmit pad
 * @param[in]   pad_rx  GPIO to use as the UART receive pad
 * @examplecode HAL_examples.c SYS_UART_GPIOCONFIG_Example
 */
#define SYS_UART_GPIOCONFIG(cfg, pad_tx, pad_rx) \
    Sys_UART_GPIOConfig(UART, (cfg), (pad_tx), (pad_rx))

/**
 * @brief       Macro wrapper for Sys_UART_Config().
 *              Configure and enable a UART interface.
 * @param[in]   uart_clk_hz  UART clock speed in hertz
 * @param[in]   baud         Baud rate to which UART* is configured
 * @param[in]   config       DMA and interrupt mode enable; use
 *                           UART_TX_DMA_[ENABLE | DISABLE]
 *                           UART_RX_DMA_[ENABLE | DISABLE]
 *                           UART_TX_INT_[ENABLE | DISABLE]
 *                           UART_RX_INT_[ENABLE | DISABLE]
 *                           UART_OVERRUN_INT_[ENABLE | DISABLE]
 * @examplecode HAL_examples.c SYS_UART_CONFIG_Example
 */
#define SYS_UART_CONFIG(uart_clk_hz, baud, config) \
    Sys_UART_Config(UART, (uart_clk_hz), (baud), (config))

/** @} */ /* End of the HALUART group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* UART_H_ */
