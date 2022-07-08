/**
 * @file bbif.h
 * @brief Hardware abstraction layer for the RF baseband interface
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

#ifndef BBIF_H_
#define BBIF_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif    /* ifdef __cplusplus */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALBBIF Baseband Interface
 *  Baseband interface hardware abstraction layer
 *  @{
 */

/** No Bluetooth Low Energy event */
#define BLE_NONE                        0x0U

/** Bluetooth Low Energy rising edge event */
#define BLE_RISING_EDGE                 0x1U

/** Bluetooth Low Energy falling edge event */
#define BLE_FALLING_EDGE                0x2U

/** Bluetooth Low Energy transition event */
#define BLE_TRANSITION                  0x3U

/**
 * @brief       Configure the coexistence interrupts to monitor for
 *              Bluetooth and other RF activity
 * @param[in]   edge    The edge used for all coexistence interrupts;
 *                      use BLE_[NONE | RISING_EDGE | FALLING_EDGE | TRANSITION]
 * @param[in]   types   The types of coexistence interrupts that are relevant;
 *                      use BLE_RX_BUSY, BLE_TX_BUSY, BLE_IN_PROCESS, and/or
 *                      EVENT_IN_PROCESS
 * @param[in]   cf_ant_delay    Correction factor to be applied to account for
 *                              antenna delays; used to advance the coexistence
 *                              interrupts by the specified number of uS.
 * @param[in]   cf_powerup      Correction factor to be applied to account for
 *                              power-up delays on TX, RX; used to advance the
 *                              coexistence interrupts by the specified number
 *                              of uS.
 * @examplecode HAL_examples.c Sys_BBIF_CoexIntConfig_Example
 */
static inline void Sys_BBIF_CoexIntConfig(uint32_t edge, uint32_t types,
                                          uint32_t cf_ant_delay, uint32_t cf_powerup)
{
    uint32_t cfg = 0;
    if ((types & BLE_RX_BUSY) != 0)
    {
        cfg = cfg + (edge << BBIF_COEX_INT_CFG_BLE_RX_Pos);
    }
    if ((types & BLE_TX_BUSY) != 0)
    {
        cfg = cfg + (edge << BBIF_COEX_INT_CFG_BLE_TX_Pos);
    }
    if ((types & BLE_IN_PROCESS) != 0)
    {
        cfg = cfg + (edge << BBIF_COEX_INT_CFG_BLE_IN_PROCESS_Pos);
    }
    if ((types & EVENT_IN_PROCESS) != 0)
    {
        cfg = cfg + (edge << BBIF_COEX_INT_CFG_EVENT_IN_PROCESS_Pos);
    }

    BBIF->COEX_INT_CFG = cfg;

    /* Keep the previous delay configuration for other items */
    BB->COEXIFCNTL1 = ((BB->COEXIFCNTL1 & (BB_COEXIFCNTL1_WLCPDELAY_Mask |
                                           BB_COEXIFCNTL1_WLCPDURATION_Mask)) |
                       (cf_powerup << BB_COEXIFCNTL1_WLCPTXTHR_Pos)              |
                       (cf_powerup << BB_COEXIFCNTL1_WLCPRXTHR_Pos));
    BB->COEXIFCNTL2 = ((cf_ant_delay << BB_COEXIFCNTL2_TX_ANT_DELAY_Pos) |
                       (cf_ant_delay << BB_COEXIFCNTL2_RX_ANT_DELAY_Pos));
}

/** @} */ /* End of the HALBBIF group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* BBIF_H_ */
