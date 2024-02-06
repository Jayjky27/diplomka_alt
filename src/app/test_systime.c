/*
 *==============================================================================
 * Name: test_systime.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "test_systime.h"
#include "brd_def.h"
#include "delay.h"
#include "systime.h"

void test_systime(void)
{
	volatile SysTime_t start_systime __attribute__ ((unused));
	volatile SysTime_t end_systime __attribute__ ((unused));
	volatile SysTime_t epoch_time __attribute__ ((unused));

	volatile uint32_t start_msec __attribute__ ((unused));
	volatile uint32_t end_msec __attribute__ ((unused));

	SysTimeGlobalInit();

	start_systime = SysTimeGetMcuTime();
	start_msec = SysTimeGetMcuTimeMs();

	delay(DELAY_MS_TO_CNT(1555ul));

	end_systime = SysTimeGetMcuTime();
	end_msec = SysTimeGetMcuTimeMs();

	epoch_time.Seconds = 10;
	epoch_time.SubSeconds = 100u;

	SysTimeSet(epoch_time);

	delay(DELAY_MS_TO_CNT(5600ul));

	epoch_time = SysTimeGet();

	while (1) {
		__BKPT(0);
	}
}
