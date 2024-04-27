/*
 *==============================================================================
 * Name: main.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include <stdint.h>

#include "wdog.h"
#include "hex_sw.h"
#include "port_int.h"
#include "assert.h"

#include "test_com.h"
#include "test_acc.h"
#include "test_eeprom.h"
#include "test_btn_led.h"
#include "test_clkout.h"

#include "test_systime.h"
#include "test_timer.h"

#include "test_radio.h"
#include "test_lora.h"
#include "test_lora_abp.h"
#include "test_lora_otaa.h"

#include "surv.h"

#include "main_alt.h"

#define HEX_SW_TEST_COM_VAL				0x0u
#define HEX_SW_TEST_ACC_VAL				0x1u
#define HEX_SW_TEST_EEPROM_VAL			0x2u
#define HEX_SW_TEST_BTN_LED_VAL			0x3u
#define HEX_SW_TEST_CLKOUT_VAL			0x4u

#define HEX_SW_TEST_SYSTIME_VAL			0x5u
#define HEX_SW_TEST_TIMER_VAL			0x6u

#define HEX_SW_TEST_RADIO_VAL			0x7u
#define HEX_SW_TEST_LORA_VAL			0x8u
#define HEX_SW_TEST_LORA_APB_VAL		0x9u
#define HEX_SW_TEST_LORA_OTAA_VAL		0xAu
#define HEX_SW_TEST_MAIN_CODE			0xBu

#define HEX_SW_SURV_VAL					0xFu

int main(void)
{
	uint8_t sw_val;

	wdog_init(WDOG_CONF_DIS);
	port_int_init();

	alt_main();

	return 0;
}
