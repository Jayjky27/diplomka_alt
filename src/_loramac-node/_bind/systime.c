/*!
 * \file      systime.c
 *
 * \brief     System time functions implementation.
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2018 Semtech - STMicroelectronics
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    MCD Application Team ( STMicroelectronics International )
 *
 * \author    Martin Stankus ( VSB-TUO )
 */
#include "mkl25z4.h"

#include "brd_def.h"

#include "systime.h"
#include "utilities.h"

#define SYSTIME_ITER_PER_SEC		FREQ_BUS
#define SYSTIME_ITER_PER_MSEC		(SYSTIME_ITER_PER_SEC / 1000ul)

static volatile uint64_t systime_cnt_diff;

static uint64_t tstamp_cnt_rd(void);
static uint64_t time_to_cnt(const SysTime_t *time);
static void cnt_to_time(SysTime_t *time, uint64_t cnt);

static uint64_t tstamp_cnt_rd(void)
{
	uint32_t cnt_hi, cnt_lo;
	uint64_t cnt;

	CRITICAL_SECTION_BEGIN();
	cnt_hi = PIT->LTMR64H;
	cnt_lo = PIT->LTMR64L;
	CRITICAL_SECTION_END();

	cnt = cnt_hi;
	cnt <<= 32u;
	cnt |= cnt_lo;
	cnt = ~0ull - cnt;

	return cnt;
}

static uint64_t time_to_cnt(const SysTime_t *time)
{
	uint64_t cnt;

	cnt = time->Seconds * 1000ul + time->SubSeconds;
	cnt *= SYSTIME_ITER_PER_MSEC;

	return cnt;
}

static void cnt_to_time(SysTime_t *time, uint64_t cnt)
{
	time->Seconds = cnt / SYSTIME_ITER_PER_SEC;
	time->SubSeconds = ((uint32_t) (cnt % SYSTIME_ITER_PER_SEC)) / SYSTIME_ITER_PER_MSEC;
}

SysTime_t SysTimeAdd(SysTime_t a, SysTime_t b)
{
	SysTime_t c;

	c.Seconds = a.Seconds + b.Seconds;
	c.SubSeconds = a.SubSeconds + b.SubSeconds;

	if (c.SubSeconds >= 1000) {
		c.Seconds++;
		c.SubSeconds -= 1000;
	}

	return c;
}

SysTime_t SysTimeSub(SysTime_t a, SysTime_t b)
{
	SysTime_t c;

	c.Seconds = a.Seconds - b.Seconds;
	c.SubSeconds = a.SubSeconds - b.SubSeconds;

	if (c.SubSeconds < 0) {
		c.Seconds--;
		c.SubSeconds += 1000;
	}

	return c;
}

void SysTimeGlobalInit(void)
{
	PIT->MCR = 0ul;
	PIT->CHANNEL[1].LDVAL = ~0ul;
	PIT->CHANNEL[1].TCTRL = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
	PIT->CHANNEL[0].LDVAL = ~0ul;
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK;
}

void SysTimeSet(SysTime_t sysTime)
{
	CRITICAL_SECTION_BEGIN();
	systime_cnt_diff = time_to_cnt(&sysTime) - tstamp_cnt_rd();
	CRITICAL_SECTION_END();
}

SysTime_t SysTimeGet(void)
{
	SysTime_t time;
	uint64_t cnt;

	cnt = tstamp_cnt_rd();

	CRITICAL_SECTION_BEGIN();
	cnt += systime_cnt_diff;
	CRITICAL_SECTION_END();

	cnt_to_time(&time, cnt);

	return time;
}

SysTime_t SysTimeGetMcuTime(void)
{
	SysTime_t time;

	cnt_to_time(&time, tstamp_cnt_rd());

	return time;
}

uint32_t SysTimeGetMcuTimeMs(void)
{
	return tstamp_cnt_rd() / SYSTIME_ITER_PER_MSEC;
}

uint32_t SysTimeToMs(SysTime_t sysTime)
{
	uint64_t cnt;

	cnt = time_to_cnt(&sysTime);

	CRITICAL_SECTION_BEGIN();
	cnt -= systime_cnt_diff;
	CRITICAL_SECTION_END();

	return cnt / SYSTIME_ITER_PER_MSEC;
}

SysTime_t SysTimeFromMs(uint32_t timeMs)
{
	SysTime_t time;
	uint64_t cnt;

	cnt = ((uint64_t) timeMs) * SYSTIME_ITER_PER_MSEC;

	CRITICAL_SECTION_BEGIN();
	cnt += systime_cnt_diff;
	CRITICAL_SECTION_END();

	cnt_to_time(&time, cnt);

	return time;
}
