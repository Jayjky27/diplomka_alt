/*
 *==============================================================================
 * Name: test_btn_led.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "test_btn_led.h"

#include "soc_def.h"
#include "brd_def.h"
#include "led.h"
#include "btn.h"
#include "port_int.h"

static void test_btn_led_sw1_cb(void);

static void test_btn_led_sw1_cb(void)
{
	PORT_BTN_SW1->ISFR = IOMASK_BTN_SW1;
	LED_TGL(LED_D1);
}

void test_btn_led(void)
{
	led_init();

	port_int_en_pin(IRQN_BTN_SW1, test_btn_led_sw1_cb, IOIND_BTN_SW1);

	PORT_BTN_SW1->PCR[IOIND_BTN_SW1] = PORT_PCR_IRQC(PORT_PCR_IRQC_VAL_INT_EFALL) |
			PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

	while (1) {
		if (btn_get(BTN_ID_SW2)) {
			LED_ON(LED_D2);
		} else {
			LED_OFF(LED_D2);
		}
	}
}
