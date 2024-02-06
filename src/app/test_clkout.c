/*
 *==============================================================================
 * Name: test_clkout.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "brd_def.h"

void test_clkout(void)
{
	//TP9
	PORT_CLKOUT->PCR[IOIND_CLKOUT] = PORT_PCR_MUX(MUX_CLKOUT);

	while (1) {}
}
