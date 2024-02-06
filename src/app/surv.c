/*
 *==============================================================================
 * Name: surv.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "surv.h"
#include "probe.h"
#include "com.h"
#include "str.h"
#include "msg.h"
#include "print.h"
#include "expt.h"

#define PERIPH_BASE			0x40000000ul

#define SLOT_CNT			256ul
#define SLOT_SHFT			12u
#define SLOT_SIZE			(1ul << SLOT_SHFT)

static void surv_probe_print(uint32_t addr, uint32_t val, uint8_t val_dig_cnt);
static void surv_check_slot(uint32_t (*probe)(uint32_t *, const void *), uint32_t slot_ind, uint32_t step);

static void surv_probe_print(uint32_t addr, uint32_t val, uint8_t val_dig_cnt)
{
	uint16_t msg_len;

	while(!com_try_lock());

	msg_len = 0u;
	msg_len += strcpy(&msg[msg_len], "0x");
	msg_len += print_uhex(&msg[msg_len], 8u, addr);
	msg_len += strcpy(&msg[msg_len], " => 0x");
	msg_len += print_uhex(&msg[msg_len], val_dig_cnt, val);
	msg_len += strcpy(&msg[msg_len], "\r\n");

	com_send(msg, msg_len);
}

static void surv_check_slot(uint32_t (*probe)(uint32_t *, const void *), uint32_t slot_ind, uint32_t step)
{
	uint32_t probe_ind, reg_val;
	uint16_t msg_len, hit_cnt;
	const void *reg;

	hit_cnt = 0u;

	while(!com_try_lock());

	msg_len = 0u;
	msg_len += strcpy(&msg[msg_len], "PROBE");
	msg_len += print_udec(&msg[msg_len], step * 8u);
	msg_len += strcpy(&msg[msg_len], "\r\n");

	com_send(msg, msg_len);

	for (probe_ind = 0ul; probe_ind < SLOT_SIZE; probe_ind += step) {
		reg = (void *) (PERIPH_BASE | (slot_ind << SLOT_SHFT) | probe_ind);

		if (probe(&reg_val, reg)) {
			hit_cnt++;
			surv_probe_print((uint32_t) reg, reg_val, step * 2u);
		}
	}

	while(!com_try_lock());

	msg_len = 0u;
	msg_len += strcpy(&msg[msg_len], "Hit count: ");
	msg_len += print_udec(&msg[msg_len], hit_cnt);
	msg_len += strcpy(&msg[msg_len], "\r\n\r\n");

	com_send(msg, msg_len);
}

void surv(void)
{
	uint32_t slot_ind;
	uint16_t msg_len;

	SIM->SCGC4 = 0xFFFFFFFFul;
	SIM->SCGC5 = 0xFFFFFFFFul;
	SIM->SCGC6 = 0xFFFFFFFFul;
	SIM->SCGC7 = 0xFFFFFFFFul;

	com_init();

	expt_vect_set(HardFault_IRQn, probe_hardfault_handler);

	for (slot_ind = 0ul; slot_ind < SLOT_CNT; slot_ind++) {

		while(!com_try_lock());

		msg_len = 0u;
		msg_len += strcpy(&msg[msg_len], "SLOT ");
		msg_len += print_udec(&msg[msg_len], slot_ind);
		msg_len += strcpy(&msg[msg_len], "\r\n\r\n");

		com_send(msg, msg_len);

		surv_check_slot(probe8, slot_ind, 1ul);
		surv_check_slot(probe16, slot_ind, 2ul);
		surv_check_slot(probe32, slot_ind, 4ul);

		while(!com_try_lock());

		msg_len = 0u;
		msg_len += strcpy(&msg[msg_len], "================");
		msg_len += strcpy(&msg[msg_len], "\r\n\r\n");

		com_send(msg, msg_len);
	}

	while(!com_try_lock());

	msg_len = 0u;
	msg_len += strcpy(&msg[msg_len], "DONE");
	msg_len += strcpy(&msg[msg_len], "\r\n");

	com_send(msg, msg_len);

	while (1) {}
}
