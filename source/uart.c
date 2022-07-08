/**
 * @file uart.c
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

#include <hw.h>

#define UINT32_BITS                     32
#define UART_FACTOR                     (1 << 18)
#define UART_FACTOR_SHIFT               18

void Sys_UART_Config(UART_Type *uart, uint32_t uart_clk_hz, uint32_t baud,
                     uint32_t config)
{
    unsigned int num_bits = 0;
    unsigned int overflow_shift = 0;
    uint32_t baud_calc;

    SYS_ASSERT(UART_REF_VALID(uart));

    /* Make sure we do not divide by zero */
    SYS_ASSERT(uart_clk_hz);

    /* Find the size of the baudrate variable (in number of bits) to calculate
     * the shift required to prevent overflow during calculation. */
    int32_t numerator = baud;
    while (numerator != 0)
    {
        numerator >>= 1;
        num_bits++;
    }

    /* Calculate the appropriate amount of right shift to prevent
     * overflow */
    if ((num_bits + UART_FACTOR_SHIFT) > UINT32_BITS)
    {
        overflow_shift = (UART_FACTOR_SHIFT + num_bits - UINT32_BITS);
    }

    /* Configure count step for baud rate generation */
    baud_calc = (((unsigned int)baud * (UART_FACTOR >> overflow_shift)) /
                 (unsigned int)(uart_clk_hz >> overflow_shift)) - 1;

    uart->CFG =  (uint32_t)(baud_calc | (~UART_CFG_CNT_STEP_Mask & config));
}
