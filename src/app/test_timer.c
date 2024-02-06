/*
 *==============================================================================
 * Name: test_timer.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include <string.h>

#include "mkl25z4.h"

#include "test_timer.h"
#include "soc_def.h"
#include "brd_def.h"
#include "led.h"
#include "gpio.h"
#include "timer.h"

TimerEvent_t tmr[10];

void cb0(void *arg)
{
	static uint8_t state;

	LED_TGL(LED_D1);

	if (state) {
		state = 0u;
		TimerSetValue(&tmr[2], 50ul);
	} else {
		state = 1u;
		TimerSetValue(&tmr[2], 950ul);
	}
}

void cb1(void *arg)
{
	LED_TGL(LED_D2);
	TimerStop(&tmr[5]);
}

void cb2(void *arg)
{
	LED_TGL(LED_D2);
	TimerStart(&tmr[5]);
}

void test_timer(void)
{
	led_init();

	TimerGlobalInit();

	TimerInit(&tmr[0], NULL);
	TimerInit(&tmr[1], NULL);
	TimerInit(&tmr[2], cb0);
	TimerInit(&tmr[3], NULL);
	TimerInit(&tmr[4], NULL);
	TimerInit(&tmr[5], cb1);
	TimerInit(&tmr[6], NULL);
	TimerInit(&tmr[7], NULL);
	TimerInit(&tmr[8], NULL);
	TimerInit(&tmr[9], cb2);

	TimerSetValue(&tmr[2], 50ul);
	TimerSetValue(&tmr[5], 50ul);
	TimerSetValue(&tmr[9], 1000ul);

	LED_ON(LED_D1);
	LED_ON(LED_D2);

	TimerStart(&tmr[2]);
	TimerStart(&tmr[5]);
	TimerStart(&tmr[9]);

	while (1) {
	}
}
