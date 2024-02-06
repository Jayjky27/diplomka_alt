/*
 *==============================================================================
 * Name: led.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "led.h"
#include "soc_def.h"
#include "brd_def.h"
#include "gpio.h"

void led_init(void)
{
	LED_OFF(LED_D1);
	GPIO_MODE_OUT(LED_D1);
	PORT_LED_D1->PCR[IOIND_LED_D1] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	LED_OFF(LED_D2);
	GPIO_MODE_OUT(LED_D2);
	PORT_LED_D2->PCR[IOIND_LED_D2] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
}
