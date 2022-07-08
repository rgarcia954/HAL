/**
 * @file gpio.h
 * @brief Hardware abstraction layer for GPIO
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee..
 *
 * This is Reusable Code.
 * @endparblock
 */
#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

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

/** @defgroup HALGPIO General-Purpose I/O Interface
 *  General-Purpose I/O (GPIO) Interface hardware abstraction layer
 *  @{
 */

/* ----------------------------------------------------------------------------
 * GPIO definitions
 * ------------------------------------------------------------------------- */

/** 1st level GPIO drive strength */
#define GPIO_LEVEL1_DRIVE               GPIO_2X_DRIVE

/** 2nd level GPIO drive strength */
#define GPIO_LEVEL2_DRIVE               GPIO_3X_DRIVE

/** 3rd level GPIO drive strength */
#define GPIO_LEVEL3_DRIVE               GPIO_5X_DRIVE

/** 4th level GPIO drive strength */
#define GPIO_LEVEL4_DRIVE               GPIO_6X_DRIVE

#ifndef NON_SECURE
/* ----------------------------------------------------------------------------
 * GPIO pad support functions
 * ------------------------------------------------------------------------- */

/**
 * @brief       Configure the specified digital I/O
 * @param[in]   pad Digital I/O pad to configure; use a constant
 * @param[in]    config  I/O configuration; use GPIO_*X_DRIVE,
 *                            GPIO_LPF_[ENABLE | DISABLE],
 *                            GPIO_*_PULL, and GPIO_MODE_*
 * @examplecode HAL_examples.c SYS_GPIO_CONFIG_Example
 */
#define SYS_GPIO_CONFIG(pad, config) \
    SYS_ASSERT(pad < GPIO_PAD_COUNT); \
    GPIO->CFG[(pad)] = (config)

/**
 * @brief       Configure a source for NMI input selection
 * @param[in]   config      GPIO pin configuration if NMI is pad
 * @param[in]   source      NMI source; use NMI_SRC_*
 * @param[in]   polarity    NMI polarity; use NMI_ACTIVE_[LOW | HIGH]
 * @examplecode HAL_examples.c Sys_GPIO_NMIConfig_Example
 */
static inline void Sys_GPIO_NMIConfig(uint32_t config,
                                      uint32_t source,
                                      uint32_t polarity)
{
    /* If source is a GPIO pad configure GPIO as input */
    if (source < GPIO_PAD_COUNT)
    {
        SYS_GPIO_CONFIG(source, ((config) | GPIO_MODE_INPUT));
    }
    GPIO->SRC_NMI = source | polarity;
}

/**
 * @brief       Configure a GPIO interrupt source
 * @param[in]   index GPIO interrupt source to configure; use a constant
 * @param[in]    config     GPIO interrupt configuration; use
 *                          GPIO__INT_DEBOUNCE_[DISABLE | ENABLE],
 *                          GPIO_INT_SRC_*, and
 *                          GPIO_INT_EVENT_[NONE | HIGH_LEVEL | LOW_LEVEL |
 *                          RISING_EDGE | FALLING_EDGE | TRANSITION]
 * @param[in]    dbnc_clk   Interrupt button debounce filter clock; use
 *                          GPIO_DEBOUNCE_SLOWCLK_DIV[32 | 1024]
 * @param[in]    dbnc_cnt  Interrupt button debounce filter count
 * @examplecode HAL_examples.c Sys_GPIO_IntConfig_Example
 */
static inline void Sys_GPIO_IntConfig(uint32_t index, uint32_t config,
                                      uint32_t dbnc_clk, uint32_t dbnc_cnt)
{
    SYS_ASSERT(index < GPIO_EVENT_CHANNEL_COUNT);
    GPIO->INT_CFG[index] = config;
    GPIO->INT_DEBOUNCE = (dbnc_clk |
                          ((dbnc_cnt << GPIO_INT_DEBOUNCE_DEBOUNCE_COUNT_Pos) &
                           GPIO_INT_DEBOUNCE_DEBOUNCE_COUNT_Mask));
}

/**
 * @brief       Configure Arm Cortex-M3 SWJ-DP
 * @param[in]   config  JTAG pad configuration; use
 *                      JTMS_[NO_PULL | WEAK_PULL_UP | WEAK_PULL_DOWN |
 *                      STRONG_PULL_UP],
 *                      JTMS_[2X | 3X | 5X | 6X]_DRIVE,
 *                      JTCK_[NO_PULL | WEAK_PULL_UP | WEAK_PULL_DOWN |
 *                      STRONG_PULL_UP],
 *                      JTMS_LPF_[DISABLED | ENABLED], and
 *                      JTCK_LPF_[DISABLED | ENABLED]
 * @param[in]     mode  Enable one of the two JTAG modes or SW mode; use
 *                      0 for SW mode, 1 for JTAG with reset enabled, and
 *                      2 for JTAG with reset disabled
 * @examplecode HAL_examples.c Sys_GPIO_CM33JTAGConfig_Example
 */
static inline void Sys_GPIO_CM33JTAGConfig(uint32_t config, uint8_t mode)
{
    if (mode)
    {
        if (mode == 1)
        {
            GPIO->JTAG_SW_PAD_CFG = config | CM33_JTAG_TRST_ENABLED | CM33_JTAG_DATA_ENABLED;
        }
        else
        {
            GPIO->JTAG_SW_PAD_CFG = config | CM33_JTAG_TRST_DISABLED | CM33_JTAG_DATA_ENABLED;
        }
    }
    else
    {
        GPIO->JTAG_SW_PAD_CFG = config | CM33_JTAG_TRST_ENABLED | CM33_JTAG_DATA_DISABLED;
    }
}
#endif /* ifndef NON_SECURE */

/* ----------------------------------------------------------------------------
 * GPIO support functions
 * ------------------------------------------------------------------------- */

/**
 * @brief       Set the specified GPIO output value to high
 * @param[in]   pad GPIO pad to set high
 * @examplecode HAL_examples.c Sys_GPIO_Set_High_Example
 */
static inline void Sys_GPIO_Set_High(uint32_t pad)
{
    SYS_ASSERT(pad < GPIO_PAD_COUNT);
    GPIO->OUTPUT_DATA_SET = (1 << pad);
}

/**
 * @brief       Set the specified GPIO output value to low
 * @param[in]   pad GPIO pad to set low
 * @examplecode HAL_examples.c Sys_GPIO_Set_Low_Example
 */
static inline void Sys_GPIO_Set_Low(uint32_t pad)
{
    SYS_ASSERT(pad < GPIO_PAD_COUNT);
    GPIO->OUTPUT_DATA_CLR = (1 << pad);
}

/**
 * @brief       Toggle the current value of the specified GPIO output
 * @param[in]   pad GPIO pad to toggle
 * @examplecode HAL_examples.c Sys_GPIO_Toggle_Example
 */
static inline void Sys_GPIO_Toggle(uint32_t pad)
{
    SYS_ASSERT(pad < GPIO_PAD_COUNT);
    GPIO->OUTPUT_DATA ^= (1 << pad);
}

/**
 * @brief       Read the specified GPIO value
 * @param[in]   pad GPIO pad to set low
 * @return     uint32_t pin value; 0 or 1
 * @examplecode HAL_examples.c Sys_GPIO_Read_Example
 */
static inline uint32_t Sys_GPIO_Read(uint32_t pad)
{
    SYS_ASSERT(pad < GPIO_PAD_COUNT);
    return ((GPIO->INPUT_DATA >> pad) & 0x1);
}

/**
 * @brief       Write the specified GPIO value
 * @param[in]   pad GPIO pad to write value
 * @param[in]   value 1 or 0 written to GPIO
 * @examplecode HAL_examples.c Sys_GPIO_Write_Example
 */
static inline void Sys_GPIO_Write(uint32_t pad, bool value)
{
    SYS_ASSERT(pad < GPIO_PAD_COUNT);
    GPIO->OUTPUT_DATA_CLR = (0x1 << pad);
    GPIO->OUTPUT_DATA_SET = ((value & 0x1) << pad);
}

/**
 * @brief       Set the input/output direction for any GPIOs configured
 *                 as GPIOs
 * @param[in]   dir Input/output configuration settings for any GPIOs
 *                  from 0 to 15 that are configured as GPIO pads; use
 *                  GPIO*_INPUT, and GPIO*_OUTPUT
 * @examplecode HAL_examples.c Sys_GPIO_Set_Direction_Example
 */
static inline void Sys_GPIO_Set_Direction(uint32_t dir)
{
    /* Check if input is not a null pointer */
    SYS_ASSERT(dir);
    GPIO->DIR = (dir & GPIO_DIR_GPIO_Mask);
}

/** @} */ /* End of the HALGPIO group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* GPIO_H */
