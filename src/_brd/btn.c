/*
 *==============================================================================
 * Name: btn.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "btn.h"
#include "soc_def.h"
#include "brd_def.h"
#include "bme.h"
#include "crsec.h"
#include "assert.h"

static uint8_t btn_get_dispatch(PORT_Type *port, GPIO_Type *gpio, uint8_t ioind);

static uint8_t btn_get_dispatch(PORT_Type *port, GPIO_Type *gpio, uint8_t ioind)
{
	uint8_t btn_val;

	crsec_enter();

	port->PCR[ioind] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO) |
						PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

	btn_val = BME_UBFX_W(&gpio->PDIR, ioind, 1u) ^ 0x1ul;

	port->PCR[ioind] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

	crsec_leave();

	return btn_val;
}

uint8_t btn_get(uint8_t btn_id)
{
	uint8_t btn_val;

	switch (btn_id) {
	case BTN_ID_SW1:
		btn_val = btn_get_dispatch(PORT_BTN_SW1, GPIO_BTN_SW1, IOIND_BTN_SW1);
		break;
	case BTN_ID_SW2:
		btn_val = btn_get_dispatch(PORT_BTN_SW2, GPIO_BTN_SW2, IOIND_BTN_SW2);
		break;
	default:
		assert(0);
		break;
	}

	return btn_val;
}
