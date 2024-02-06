/*
 *==============================================================================
 * Name: port_int.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "port_int.h"
#include "expt.h"
#include "assert.h"

#define PORT_INT_PINS_PER_PORT		32u

static void port_int_def_cb(void);
static void __attribute__ ((interrupt)) port_int_porta_handler(void);
static void __attribute__ ((interrupt)) port_int_portd_handler(void);

static volatile PORT_INT_CB port_int_porta_cb[PORT_INT_PINS_PER_PORT] = {port_int_def_cb};
static volatile PORT_INT_CB port_int_portd_cb[PORT_INT_PINS_PER_PORT] = {port_int_def_cb};

static void port_int_def_cb(void)
{
	assert(0);
}

static void __attribute__ ((interrupt)) port_int_porta_handler(void)
{
	uint32_t isfr_val;
	uint8_t pin_ind;

	isfr_val = PORTA->ISFR;

	for (pin_ind = 0u; pin_ind < PORT_INT_PINS_PER_PORT; pin_ind++) {
		if (isfr_val & (1ul << pin_ind)) {
			port_int_porta_cb[pin_ind]();
		}
	}
}

static void __attribute__ ((interrupt)) port_int_portd_handler(void)
{
	uint32_t isfr_val;
	uint8_t pin_ind;

	isfr_val = PORTD->ISFR;

	for (pin_ind = 0u; pin_ind < PORT_INT_PINS_PER_PORT; pin_ind++) {
		if (isfr_val & (1ul << pin_ind)) {
			port_int_portd_cb[pin_ind]();
		}
	}
}

void port_int_init(void)
{
	expt_vect_set(PORTA_IRQn, port_int_porta_handler);
	expt_vect_set(PORTD_IRQn, port_int_portd_handler);
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
}

void port_int_en_pin(IRQn_Type irqn, PORT_INT_CB cb, uint8_t pin_ind)
{
	switch (irqn) {
	case PORTA_IRQn:
		port_int_porta_cb[pin_ind] = cb;
		break;
	case PORTD_IRQn:
		port_int_portd_cb[pin_ind] = cb;
		break;
	default:
		assert(0);
		break;
	}
}
