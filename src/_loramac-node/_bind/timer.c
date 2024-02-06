/*!
 * \file      timer.c
 *
 * \brief     Timer objects and scheduling management implementation
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
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Martin Stankus ( VSB-TUO )
 */

#include "mkl25z4.h"

#include "soc_def.h"
#include "bme.h"
#include "expt.h"
#include "assert.h"

#include "timer.h"
#include "systime.h"
#include "utilities.h"

#define TIMER_TPM_SC_PS_VAL			TPM_SC_PS_VAL_D32
#define TIMER_ITER_PER_MSEC			125u

#define TIMER_TPM0_OBJ_OFFSET		0u
#define TIMER_TPM1_OBJ_OFFSET		TPM0_CHAN_CNT
#define TIMER_TPM2_OBJ_OFFSET		(TPM0_CHAN_CNT + TPM1_CHAN_CNT)

#define TIMER_CNT					(TPM0_CHAN_CNT + TPM1_CHAN_CNT + TPM2_CHAN_CNT)

static volatile TimerEvent_t *volatile timer_obj[TIMER_CNT];
static volatile uint8_t timer_init_ind;

static void __attribute__ ((interrupt)) tpm_handler(void);
static void timer_dispatch(TPM_Type *tpm, uint8_t tmr_ind, uint8_t tmr_cnt);

static void __attribute__ ((interrupt)) tpm_handler(void)
{
	switch (__get_IPSR() - 16u) {
	case TPM0_IRQn:
		timer_dispatch(TPM0, TIMER_TPM0_OBJ_OFFSET, TPM0_CHAN_CNT);
		break;
	case TPM1_IRQn:
		timer_dispatch(TPM1, TIMER_TPM1_OBJ_OFFSET, TPM1_CHAN_CNT);
		break;
	case TPM2_IRQn:
		timer_dispatch(TPM2, TIMER_TPM2_OBJ_OFFSET, TPM2_CHAN_CNT);
		break;
	default:
		assert(0);
		break;
	}
}

static void timer_dispatch(TPM_Type *tpm, uint8_t tmr_ind, uint8_t tmr_cnt)
{
	volatile TimerEvent_t *obj;
	uint8_t chan_ind;

	for (chan_ind = 0u; chan_ind < tmr_cnt; chan_ind++) {
		if (BME_UBFX_W(&tpm->CONTROLS[chan_ind].CnSC, TPM_CnSC_CHF_SHIFT, 1u)) {
			tpm->CONTROLS[chan_ind].CnSC = TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK | TPM_CnSC_MSA_MASK;

			obj = timer_obj[tmr_ind];

			if (obj->ovf_count != obj->ovf_target) {
				obj->ovf_count++;
			} else {
				tpm->CONTROLS[chan_ind].CnV = TPM1->CNT + obj->period;
				obj->ovf_count = 0ul;
				obj->cb(NULL);
			}

		}
		tmr_ind++;
	}
}

void TimerGlobalInit(void)
{
	expt_vect_set(TPM0_IRQn, tpm_handler);
	expt_vect_set(TPM1_IRQn, tpm_handler);
	expt_vect_set(TPM2_IRQn, tpm_handler);

	//TPM1->CONF = TPM_CONF_DBGMODE(TPM_CONF_DBGM_VAL_RUN);
	TPM0->CONF = TPM_CONF_GTBEEN_MASK;
	TPM2->CONF = TPM_CONF_GTBEEN_MASK;

	TPM1->SC = TPM_SC_CMOD(TPM_SC_CMOD_VAL_INTCLK) | TPM_SC_PS(TIMER_TPM_SC_PS_VAL);

	NVIC_EnableIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM2_IRQn);
}

void TimerInit(volatile TimerEvent_t *obj, void (*callback)(void *context))
{
	CRITICAL_SECTION_BEGIN();

	assert(timer_init_ind < TIMER_CNT);

	timer_obj[timer_init_ind] = obj;

	if (timer_init_ind < TIMER_TPM1_OBJ_OFFSET) {
		obj->irqn = TPM0_IRQn;
		obj->tpm = TPM0;
		obj->tpm_chan = timer_init_ind - TIMER_TPM0_OBJ_OFFSET;
	} else if (timer_init_ind < TIMER_TPM2_OBJ_OFFSET) {
		obj->irqn = TPM1_IRQn;
		obj->tpm = TPM1;
		obj->tpm_chan = timer_init_ind - TIMER_TPM1_OBJ_OFFSET;
	} else {
		obj->irqn = TPM2_IRQn;
		obj->tpm = TPM2;
		obj->tpm_chan = timer_init_ind - TIMER_TPM2_OBJ_OFFSET;
	}

	obj->period = 0ul;
	obj->cb = callback;

	timer_init_ind++;

	CRITICAL_SECTION_END();
}

void TimerDeInitAll(void)
{
	uint8_t chan_ind;

	CRITICAL_SECTION_BEGIN();

	for (chan_ind = 0; chan_ind < TPM0_CHAN_CNT; chan_ind++) {
		TPM0->CONTROLS[chan_ind].CnSC = TPM_CnSC_CHF_MASK;
	}

	for (chan_ind = 0; chan_ind < TPM1_CHAN_CNT; chan_ind++) {
		TPM1->CONTROLS[chan_ind].CnSC = TPM_CnSC_CHF_MASK;
	}

	for (chan_ind = 0; chan_ind < TPM2_CHAN_CNT; chan_ind++) {
		TPM2->CONTROLS[chan_ind].CnSC = TPM_CnSC_CHF_MASK;
	}

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_ClearPendingIRQ(TPM2_IRQn);

	timer_init_ind = 0ul;

	CRITICAL_SECTION_END();
}

void TimerStart(volatile TimerEvent_t *obj)
{
	CRITICAL_SECTION_BEGIN();

	assert(obj->period != 0ul);

	TimerStop(obj);

	obj->tpm->CONTROLS[obj->tpm_chan].CnV = TPM1->CNT + obj->period;
	obj->ovf_count = 0ul;

	obj->tpm->CONTROLS[obj->tpm_chan].CnSC = TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK | TPM_CnSC_MSA_MASK;

	CRITICAL_SECTION_END();
}

void TimerStop(const volatile TimerEvent_t *obj)
{
	CRITICAL_SECTION_BEGIN();

	obj->tpm->CONTROLS[obj->tpm_chan].CnSC = TPM_CnSC_CHF_MASK;
	NVIC_ClearPendingIRQ(obj->irqn);

	CRITICAL_SECTION_END();
}

void TimerSetValue(volatile TimerEvent_t *obj, uint32_t value)
{
	uint8_t run_flag = 0u;

	assert(value > 0ul);

	CRITICAL_SECTION_BEGIN();

	if (BME_UBFX_W(&obj->tpm->CONTROLS[obj->tpm_chan].CnSC, TPM_CnSC_CHIE_SHIFT, 1u)) {
		run_flag = 1u;
		TimerStop(obj);
	}

	obj->period = value * TIMER_ITER_PER_MSEC;
	obj->ovf_target = obj->period >> 16u;

	if (run_flag) {
		TimerStart(obj);
	}

	CRITICAL_SECTION_END();
}

TimerTime_t TimerGetCurrentTime(void)
{
	return SysTimeGetMcuTimeMs();
}

TimerTime_t TimerGetElapsedTime(TimerTime_t past)
{
	if (past == 0ul) {
		return 0ul;
	}

	return TimerGetCurrentTime() - past;
}

TimerTime_t TimerTempCompensation(TimerTime_t period, float temperature)
{
	return period;
}
