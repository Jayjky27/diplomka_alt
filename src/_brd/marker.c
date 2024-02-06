/*
 *==============================================================================
 * Name: marker.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "marker.h"
#include "soc_def.h"
#include "brd_def.h"
#include "gpio.h"

void marker_init(void)
{
	GPIO_OUT_LO(MARKER_A);
	GPIO_MODE_OUT(MARKER_A);
	PORT_MARKER_A->PCR[IOIND_MARKER_A] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	GPIO_OUT_LO(MARKER_B);
	GPIO_MODE_OUT(MARKER_B);
	PORT_MARKER_B->PCR[IOIND_MARKER_B] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	GPIO_OUT_LO(MARKER_C);
	GPIO_MODE_OUT(MARKER_C);
	PORT_MARKER_C->PCR[IOIND_MARKER_C] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	GPIO_OUT_LO(MARKER_D);
	GPIO_MODE_OUT(MARKER_D);
	PORT_MARKER_D->PCR[IOIND_MARKER_D] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	GPIO_OUT_LO(MARKER_E);
	GPIO_MODE_OUT(MARKER_E);
	PORT_MARKER_E->PCR[IOIND_MARKER_E] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	GPIO_OUT_LO(MARKER_F);
	GPIO_MODE_OUT(MARKER_F);
	PORT_MARKER_F->PCR[IOIND_MARKER_F] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
}
