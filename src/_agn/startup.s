/*
 *==============================================================================
 * Name: startup.s
 * Author: Martin Stankus
 *==============================================================================
 */

	.syntax unified

	.global startup

	.set SCB_BASE,			0xE000ED00
	.set SCB_VTOR_OFFSET,	0x08

/*
 *==============================================================================
 * startup
 *==============================================================================
 */

	.section .text.startup
	.type startup, %function
startup:

	bl init

	/* ram vectab init */

	ldr r0, =VECTAB_RAM_BEGIN
	ldr r1, =VECTAB_ROM_BEGIN
	ldr r2, =VECTAB_RAM_LEN
	bl memcpy

	ldr r0, =VECTAB_RAM_BEGIN
	ldr r1, =SCB_BASE
	str r0, [r1, #SCB_VTOR_OFFSET]

	/* data section init */

	ldr r0, =DATA_BEGIN
	ldr r1, =DATA_INIT
	ldr r2, =DATA_LEN
	bl memcpy

	/* bss section zeroization */

	ldr r0, =BSS_BEGIN
	ldr r1, =0
	ldr r2, =BSS_LEN
	bl memset

	/* stack pattern fill */
	/* this is for debugging purposes */

	ldr r0, =STACK_BOTTOM
	ldr r1, =0xA5
	ldr r2, =STACK_SZ
	bl memset

	bl main

stop:
	b stop

	.size startup, . - startup

/*
 *==============================================================================
 * EOF
 *==============================================================================
 */
	.end
