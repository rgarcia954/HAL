/**
 * @file go_to_sleep_asm.S
 * @brief Assembly code routine to go to sleep with VDDC and VDDM in retention

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
 * Place this code in the usual .text segment for code
 * ------------------------------------------------------------------------- */

 	.thumb
	.section ".text"; .syntax unified;

	.globl __wfi_for_power_mode
	.type __wfi_for_power_mode, %function


/**
 * @brief Wait for interrupt routine before going to sleep
 */
__wfi_for_power_mode:
    /* Save register corrupted by the funtion */
    push {r0-r3}

    /* After WFI LDM will load memory content from the memory
     * that is used as program memory (Note that LDM require address word aligned */
    ldr r3, =__wfi_for_power_mode_data

    /* nop's are added because the precedent instruction will load the address from data memory.
     * We need to be sure that all instruction are fetched when executing WFI */
    nop
    nop
    nop

    /* At this point the instruction after WFI is fetched, after WFI it will start this instruction that will load content from memory.
     * The content can be corrupted but it will not be used. */
    wfi
    ldmia.w  r3, {r0, r1, r2}

    /* restore registers corrupted */
    pop {r0-r3}
    bx lr
__wfi_for_power_mode_data:


/* ----------------------------------------------------------------------------
 * File end
 * ------------------------------------------------------------------------- */
    .end
