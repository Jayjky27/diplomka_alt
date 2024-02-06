/*
 *==============================================================================
 * Name: expt.s
 * Author: Martin Stankus
 *==============================================================================
 */

	.syntax unified

	.global expt_vect_set
	.global expt_disable
	.global expt_restore

	.set SCB_BASE,			0xE000ED00
	.set SCB_VTOR_OFFSET,	0x08

/*
 *==============================================================================
 * expt_vect_set
 *==============================================================================
 */

	.section .text.expt_vect_set
	.type expt_vect_set, %function
expt_vect_set:

	adds r0, #16
	lsls r0, #2
	ldr r2, =SCB_BASE
	ldr r2, [r2, #SCB_VTOR_OFFSET]
	str r1, [r2, r0]
	bx lr

	.size expt_vect_set, . - expt_vect_set

/*
 *==============================================================================
 * expt_disable
 *==============================================================================
 */

	.section .text.expt_disable
	.type expt_disable, %function
expt_disable:

	mrs r0, primask
	cpsid i
	bx lr

	.size expt_disable, . - expt_disable

/*
 *==============================================================================
 * expt_restore
 *==============================================================================
 */

	.section .text.expt_restore
	.type expt_restore, %function
expt_restore:

	msr primask, r0
	bx lr

	.size expt_restore, . - expt_restore

/*
 *==============================================================================
 * EOF
 *==============================================================================
 */
	.end
