/*
 *==============================================================================
 * Name: test_acc.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include <stdint.h>

#include "test_acc.h"

#include "acc.h"
#include "acc_regs.h"
#include "com.h"
#include "led.h"
#include "marker.h"
#include "print.h"
#include "str.h"
#include "term.h"
#include "port_int.h"
#include "msg.h"
#include "assert.h"

static volatile ACC_SAMP_BLK test_acc_samp_blk;

static volatile uint8_t test_acc_flag_samp;
static volatile uint8_t test_acc_flag_sw1;
static volatile uint8_t test_acc_flag_sw2;

static void test_acc_cb(ACC_SAMP_BLK *samp_blk);
static void test_acc_led_sw1_cb(void);
static void test_acc_led_sw2_cb(void);

static void test_acc_cb(ACC_SAMP_BLK *samp_blk)
{
	if (!test_acc_flag_samp) {
		test_acc_samp_blk = *samp_blk;
		test_acc_flag_samp = 1u;
	} else {
		LED_ON(LED_D1);
	}
}

void test_acc_led_sw1_cb(void)
{
	PORT_BTN_SW1->ISFR = IOMASK_BTN_SW1;
	test_acc_flag_sw1 = 1u;
}

void test_acc_led_sw2_cb(void)
{
	PORT_BTN_SW2->ISFR = IOMASK_BTN_SW2;
	test_acc_flag_sw2 = 1u;
}

#include "delay.h"

void test_acc(void)
{
	uint8_t prod_rev, intro_msg_len, mode_msg_len, msg_len, run;

	marker_init();
	led_init();
	com_init();
	prod_rev = acc_init();

	intro_msg_len = 0u;

	intro_msg_len += strcpy(&msg[intro_msg_len], TERM_CU_OFF);
	intro_msg_len += strcpy(&msg[intro_msg_len], TERM_CUP(1,1));
	intro_msg_len += strcpy(&msg[intro_msg_len], "FXLS8974CF REV: ");
	intro_msg_len += print_udec(&msg[intro_msg_len], prod_rev >> 4u);
	intro_msg_len += strcpy(&msg[intro_msg_len], ".");
	intro_msg_len += print_udec(&msg[intro_msg_len], prod_rev & 0x0Fu);
	intro_msg_len += strcpy(&msg[intro_msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

	port_int_en_pin(IRQN_BTN_SW1, test_acc_led_sw1_cb, IOIND_BTN_SW1);
	port_int_en_pin(IRQN_BTN_SW2, test_acc_led_sw2_cb, IOIND_BTN_SW2);

	PORT_BTN_SW1->PCR[IOIND_BTN_SW1] = PORT_PCR_IRQC(PORT_PCR_IRQC_VAL_INT_EFALL) |
			PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

	PORT_BTN_SW2->PCR[IOIND_BTN_SW2] = PORT_PCR_IRQC(PORT_PCR_IRQC_VAL_INT_EFALL) |
			PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

	run = 0u;

	while (1) {
		if (test_acc_flag_sw1 || test_acc_flag_sw2) {
			mode_msg_len = intro_msg_len;
			mode_msg_len += strcpy(&msg[mode_msg_len], TERM_CUP(2,1));

			if (run) {
				mode_msg_len += strcpy(&msg[mode_msg_len], "DISABLED");

				acc_disable();
				test_acc_flag_samp = 0;

				LED_OFF(LED_D1);
				LED_OFF(LED_D2);

				run = 0u;
			} else {
				if (test_acc_flag_sw1) {
					mode_msg_len += strcpy(&msg[mode_msg_len], "8G, HP, 25Hz");
					acc_enable(test_acc_cb, ACC_CFG1_FSR_8G, ACC_CFG2_WAKEPM_HP, ACC_CFG3_WAKEODR_25HZ);
				} else {
					mode_msg_len += strcpy(&msg[mode_msg_len], "2G, LP, 50Hz");
					acc_enable(test_acc_cb, ACC_CFG1_FSR_2G, ACC_CFG2_WAKEPM_LP, ACC_CFG3_WAKEODR_50HZ);
				}

				run = 1u;
			}

			mode_msg_len += strcpy(&msg[mode_msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

			if (!run) {
				msg_len = mode_msg_len;

				msg_len += strcpy(&msg[msg_len], TERM_CUP(3,1));
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));
				msg_len += strcpy(&msg[msg_len], TERM_CUP(4,1));
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));
				msg_len += strcpy(&msg[msg_len], TERM_CUP(5,1));
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));
				msg_len += strcpy(&msg[msg_len], TERM_CUP(6,1));
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));
				msg_len += strcpy(&msg[msg_len], TERM_CUP(7,1));
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

				while (!com_try_lock());
				com_send(msg, msg_len);
			}

			test_acc_flag_sw1 = 0u;
			test_acc_flag_sw2 = 0u;
		}

		if (test_acc_flag_samp) {

			assert(run);

			if (com_try_lock()) {
				msg_len = mode_msg_len;

				msg_len += strcpy(&msg[msg_len], TERM_CUP(3,1));
				msg_len += strcpy(&msg[msg_len], "TEMP: ");
				msg_len += print_dec(&msg[msg_len], test_acc_samp_blk.temp);
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

				msg_len += strcpy(&msg[msg_len], TERM_CUP(4,1));
				msg_len += strcpy(&msg[msg_len], "MAG: ");
				msg_len += print_udec(&msg[msg_len], test_acc_samp_blk.mag);
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

				msg_len += strcpy(&msg[msg_len], TERM_CUP(5,1));
				msg_len += strcpy(&msg[msg_len], "X: ");
				msg_len += print_dec(&msg[msg_len], test_acc_samp_blk.x);
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

				msg_len += strcpy(&msg[msg_len], TERM_CUP(6,1));
				msg_len += strcpy(&msg[msg_len], "Y: ");
				msg_len += print_dec(&msg[msg_len], test_acc_samp_blk.y);
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

				msg_len += strcpy(&msg[msg_len], TERM_CUP(7,1));
				msg_len += strcpy(&msg[msg_len], "Z: ");
				msg_len += print_dec(&msg[msg_len], test_acc_samp_blk.z);
				msg_len += strcpy(&msg[msg_len], TERM_EL(TERM_EL_ARG_CUR_TO_END));

				com_send(msg, msg_len);
			} else {
				LED_ON(LED_D2);
			}

			test_acc_flag_samp = 0u;
		}
	}
}
