/*
 *==============================================================================
 * Name: test_com.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include <stdint.h>

#include "test_com.h"
#include "brd_def.h"
#include "com.h"
#include "str.h"
#include "print.h"
#include "delay.h"
#include "marker.h"
#include "msg.h"
#include "assert.h"

void test_com(void)
{
	uint16_t pass, msg_len;

	marker_init();
	com_init();

	pass = 0u;

	while (1) {
		assert(com_try_lock());
		assert(!com_try_lock());

		pass++;

		msg_len = 0u;
		msg_len += strcpy(&msg[msg_len], "TEST COM PASS ");
		msg_len += print_udec(&msg[msg_len], pass);
		msg_len += strcpy(&msg[msg_len], "\r\n");

		com_send(msg, msg_len);
		delay(DELAY_MS_TO_CNT(500ul));
	}
}
