/*
 *==============================================================================
 * Name: wait.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "wait.h"
#include "assert.h"
#include "wdog.h"

void wait(uint32_t sys_ticks_x16, uint8_t wdog_ref_en)
{
	assert((sys_ticks_x16 > 1ul) && (sys_ticks_x16 <= 0x1000000ul));

	SysTick->LOAD = sys_ticks_x16 - 1ul;
	SysTick->VAL = 0ul;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
		if (wdog_ref_en) {
			wdog_refresh();
		}
	}

	SysTick->CTRL = 0ul;
}
