/*
 *==============================================================================
 * Name: vectab_rom.s
 * Author: Martin Stankus
 *==============================================================================
 */

	.syntax unified

/*
 *==============================================================================
 * rom vector table
 *==============================================================================
 */

	.section .vectab_rom, "a"

	/* system exceptions */
	.long STACK_TOP
	.long startup
	.long hdlr_nmi
	.long hdlr_hardfault
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long hdlr_def			//svc
	.long 0
	.long 0
	.long hdlr_def			//pendsv
	.long hdlr_def			//systick

	/* interrupts */
	.rept 32
	.long hdlr_def
	.endr

/*
 *==============================================================================
 * EOF
 *==============================================================================
 */
	.end
