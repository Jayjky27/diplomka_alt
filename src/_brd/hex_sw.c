/*
 *==============================================================================
 * Name: hex_sw.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "hex_sw.h"
#include "soc_def.h"
#include "brd_def.h"
#include "bme.h"
#include "crsec.h"

uint8_t hex_sw_get(void)
{
	uint8_t sw_val;

	crsec_enter();

	GPCR_HEX_SW = (IOMASK_HEX_SW << 16u) | PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO) |
			PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

	sw_val = BME_UBFX_W(&GPIO_HEX_SW->PDIR, IOIND_HEX_SW, 4u) ^ 0xFul;

	GPCR_HEX_SW = (IOMASK_HEX_SW << 16u) | PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

	crsec_leave();

	return sw_val;
}
