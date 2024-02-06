/*
 *==============================================================================
 * Name: test_lora_abp.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "led.h"
#include "btn.h"
#include "assert.h"

#include "LoRaMac.h"
#include "RegionCommon.h"

#include "sx126x-board.h"
#include "systime.h"
#include "timer.h"

static void lora_prim_mcps_cnf(McpsConfirm_t *spec)
{
}

static void lora_prim_mcps_ind(McpsIndication_t *spec)
{
}

static void lora_prim_mlme_cnf(MlmeConfirm_t *spec)
{
}

static void lora_prim_mlme_ind(MlmeIndication_t *spec)
{
}

static uint8_t lora_cb_batt(void)
{
	return 0u;
}

static float lora_cb_temp(void)
{
	return 0.0f;
}

static void lora_cb_nvm(uint16_t flags)
{

}

static void lora_cb_proc(void)
{

}

static LoRaMacPrimitives_t lora_prim = {lora_prim_mcps_cnf, lora_prim_mcps_ind, lora_prim_mlme_cnf, lora_prim_mlme_ind};
static LoRaMacCallback_t lora_cb = {lora_cb_batt, lora_cb_temp, lora_cb_nvm, lora_cb_proc};

void test_lora_abp(void)
{

	SX126xGlobalInit();
	TimerGlobalInit();
	SysTimeGlobalInit();

	SX126xPwrOn(1u);

	LoRaMacInitialization(&lora_prim, &lora_cb, LORAMAC_REGION_EU868);
	LoRaMacStart();

	while (1) {

	}
}
