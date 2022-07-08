/**
 * @file clock.c
 * @brief Hardware abstraction layer for the system clocks
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

#ifndef NON_SECURE
#include <hw.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

void Sys_Clocks_DividerConfig(uint32_t uartclk_freq, uint32_t sensorclk_freq,
                              uint32_t userclk_freq)
{
    uint32_t slowclk_div;
    uint32_t bbclk_div;
    uint32_t uartclk_div;

    uint32_t dcclk_div;
    uint32_t sensorclk_div;
#ifndef RSL15_CID
    int sensorclk_div_flag;
#endif    /* ifndef RSL15_CID */

    uint32_t userclk_div;

    /* Assert that the system core clock variables contains a non-zero value */
    SYS_ASSERT(SystemCoreClock != 0);

    /* Calculate the divisors; if the desired frequency is not an integer
     * division of SYSCLK, keep the larger prescaler value to ensure that the
     * set frequency doesn't exceed the target.
     */
    slowclk_div = (SystemCoreClock / 1000000);
    if (SystemCoreClock % 1000000 == 0)
    {
        slowclk_div = slowclk_div - 1;
    }

    bbclk_div = (SystemCoreClock / 8000000);
    if (SystemCoreClock % 8000000 == 0)
    {
        bbclk_div = bbclk_div - 1;
    }

    uartclk_div = (SystemCoreClock / uartclk_freq);
    if (SystemCoreClock % uartclk_freq == 0)
    {
        uartclk_div = uartclk_div - 1;
    }

    CLK->DIV_CFG0 = ((slowclk_div << CLK_DIV_CFG0_SLOWCLK_PRESCALE_Pos) |
                     (bbclk_div << CLK_DIV_CFG0_BBCLK_PRESCALE_Pos) |
                     (uartclk_div << CLK_DIV_CFG0_UARTCLK_PRESCALE_Pos));

    dcclk_div = (SystemCoreClock / 4000000);
    if (SystemCoreClock % 4000000 == 0)
    {
        dcclk_div = dcclk_div - 1;
    }

#ifdef RSL15_CID
    sensorclk_div = (SystemCoreClock / sensorclk_freq);
    if (SystemCoreClock % sensorclk_freq == 0)
    {
        sensorclk_div = sensorclk_div - 1;
    }
#else    /* ifdef RSL15_CID */
    /* On other devices sensor clock is derived from SLOWCLK with a power of 2
     * divisor (use the integer log2 of the desired divisor) */
    sensorclk_freq = (1000000 / sensorclk_freq);
    sensorclk_div_flag = 0;
    for (sensorclk_div = 0; sensorclk_freq != 0; sensorclk_freq = sensorclk_freq >> 1)
    {
        sensorclk_div++;
        if ((sensorclk_freq & 1) != 0)
        {
            sensorclk_div_flag = sensorclk_div_flag + 1;
        }
    }

    /* If the divider is an even power of 2, the division is exact and
     * sensorclk_div is now one too high.
     */
    if (sensorclk_div_flag == 1)
    {
        sensorclk_div--;
    }
#endif    /* ifdef RSL15_CID */

    /* Only enable the DC clock if using the buck converter. */
    if ((ACS->VCC_CTRL & (1 << ACS_VCC_CTRL_BUCK_ENABLE_Pos)) == VCC_BUCK)
    {
        CLK->DIV_CFG1 = ((dcclk_div << CLK_DIV_CFG1_DCCLK_PRESCALE_Pos) |
                         CPCLK_PRESCALE_6 |
                         (sensorclk_div << CLK_DIV_CFG1_SENSOR_CLK_PRESCALE_Pos) |
                         DCCLK_ENABLE | CPCLK_ENABLE | SENSOR_CLK_ENABLE);
    }
    else
    {
        CLK->DIV_CFG1 = ((dcclk_div << CLK_DIV_CFG1_DCCLK_PRESCALE_Pos) |
                         CPCLK_PRESCALE_6 |
                         (sensorclk_div << CLK_DIV_CFG1_SENSOR_CLK_PRESCALE_Pos) |
                         DCCLK_DISABLE | CPCLK_ENABLE | SENSOR_CLK_ENABLE);
    }

    /* If the requested user clock exceeds the system clock frequency and the
     * RF clock is enabled, use RF clock as the source for USERCLK. Otherwise,
     * use SYSCLK as the source.
     */
    if ((userclk_freq > SystemCoreClock) &&
        ((RF0_ANALOG_INFO->ANALOG_INFO_BYTE & ANALOG_INFO_CLK_DIG_READY_BYTE) == ANALOG_INFO_CLK_DIG_READY_BYTE))
    {
        userclk_div = ((48000000 / RF0_REG33->CK_DIV_1_6_CK_DIV_1_6_BYTE) / userclk_freq);
        if (SystemCoreClock % userclk_freq == 0)
        {
            userclk_div = userclk_div - 1;
        }
        CLK->DIV_CFG2 = userclk_div | USRCLK_SRC_RFCLK;
    }
    else
    {
        userclk_div = (SystemCoreClock / userclk_freq);
        if (SystemCoreClock % userclk_freq == 0)
        {
            userclk_div = userclk_div - 1;
        }
        CLK->DIV_CFG2 = userclk_div | USRCLK_SRC_SYSCLK;
    }
}

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */
#endif
