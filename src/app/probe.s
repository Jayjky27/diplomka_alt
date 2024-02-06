/*
 *==============================================================================
 * Name: probe.s
 * Author: Martin Stankus
 *==============================================================================
 */

	.syntax unified

	.global probe_hardfault_handler

	.global probe8
	.global probe16
	.global probe32

/*
 *==============================================================================
 * probe_hardfault_handler
 *==============================================================================
 */

	.section .text.probe_hardfault_handler
	.type probe_hardfault_handler, %function
probe_hardfault_handler:

	ldr r0, [sp, 0x18]
	adds r0, #2
	str r0, [sp, 0x18]
	eors r0, r0
	str r0, [sp]
	bx lr

	.size probe_hardfault_handler, . - probe_hardfault_handler

/*
 *==============================================================================
 * probe8
 *==============================================================================
 */

	.section .text.probe8
	.type probe8, %function
probe8:

	mov r2, r0
	ldr r0, =1
	ldrb r1, [r1]
	str r1, [r2]
	bx lr

	.size probe8, . - probe8

/*
 *==============================================================================
 * probe16
 *==============================================================================
 */

	.section .text.probe16
	.type probe16, %function
probe16:

	mov r2, r0
	ldr r0, =1
	ldrh r1, [r1]
	str r1, [r2]
	bx lr

	.size probe16, . - probe16
/*
 *==============================================================================
 * probe32
 *==============================================================================
 */

	.section .text.probe32
	.type probe32, %function
probe32:

	mov r2, r0
	ldr r0, =1
	ldr r1, [r1]
	str r1, [r2]
	bx lr

	.size probe32, . - probe32
/*
 *==============================================================================
 * EOF
 *==============================================================================
 */
	.end
