/*
 *==============================================================================
 * Name: wait.h
 * Author: Martin Stankus
 *
 * This module uses SysTick timer.
 * DO NOT use SysTick timer if wait() is used.
 *==============================================================================
 */

#ifndef _WAIT_H_
#define _WAIT_H_

#include <stdint.h>

#define WAIT_10MSEC_FSYS48M			30000ul
#define WAIT_100MSEC_FSYS48M		300000ul
#define WAIT_500MSEC_FSYS48M		1500000ul
#define WAIT_1000MSEC_FSYS48M		3000000ul
#define WAIT_2000MSEC_FSYS48M		6000000ul
#define WAIT_3000MSEC_FSYS48M		9000000ul

void wait(uint32_t sys_ticks_x16, uint8_t wdog_ref_en);

#endif /* _WAIT_H_ */
