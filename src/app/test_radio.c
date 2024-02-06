/*
 *==============================================================================
 * Name: test_radio.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "test_radio.h"
#include "sx126x-board.h"
#include "systime.h"
#include "board.h"

void test_radio(void)
{
	uint8_t buf[16], id[8];

	BoardGetUniqueId(id);

	SX126xGlobalInit();
	SysTimeGlobalInit();

	SX126xPwrOn(1u);
	SX126xReadCommand(RADIO_GET_STATUS, buf, 16u);
	SX126xPwrOff();

	while (1) {
		__BKPT(0);
	}
}
