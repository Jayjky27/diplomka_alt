/*
 *==============================================================================
 * Name: test_lora.c
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

#include "LmHandler.h"
#include "RegionCommon.h"

#include "sx126x-board.h"
#include "systime.h"
#include "timer.h"

#define TX_PAYLOAD_SIZE			32u

#define RX_TRACE_LEN			8u
#define RX_BUF_LEN				32u

#define TX_PORT					3u

static uint8_t batt_lev_cb(void);
static float temp_cb(void);
static uint32_t rand_seed_cb(void);
static void mac_cb(void);
static void nvm_chng_cb(LmHandlerNvmContextStates_t state, uint16_t size);
static void net_prm_chng_cb(CommissioningParams_t *prm);
static void mcps_req(LoRaMacStatus_t status, McpsReq_t *req, TimerTime_t delay);
static void mlme_req(LoRaMacStatus_t status, MlmeReq_t *req, TimerTime_t delay);
static void join_req_cb(LmHandlerJoinParams_t *prm);
static void tx_cb(LmHandlerTxParams_t *prm);
static void rx_cb(LmHandlerAppData_t *rx_data, LmHandlerRxParams_t *prm);
static void class_change_cb(DeviceClass_t class);
static void bcn_stat_chng_cb(LoRaMacHandlerBeaconParams_t *prm);
static void systime_upd_cb(bool sync, int32_t corr);

static uint8_t tx_busy, rx_desc_ind;
static LmHandlerAppData_t rx_desc[RX_TRACE_LEN];
static uint8_t rx_buf[RX_TRACE_LEN][RX_BUF_LEN];

static LmHandlerCallbacks_t lmh_cb = {
	.GetBatteryLevel = batt_lev_cb,
	.GetTemperature = temp_cb,
	.GetRandomSeed = rand_seed_cb,
	.OnMacProcess = mac_cb,
	.OnNvmDataChange = nvm_chng_cb,
	.OnNetworkParametersChange = net_prm_chng_cb,
	.OnMacMcpsRequest = mcps_req,
	.OnMacMlmeRequest = mlme_req,
	.OnJoinRequest = join_req_cb,
	.OnTxData = tx_cb,
	.OnRxData = rx_cb,
	.OnClassChange = class_change_cb,
	.OnBeaconStatusChange = bcn_stat_chng_cb,
	.OnSysTimeUpdate = systime_upd_cb
};

static LmHandlerParams_t lmh_prm = {
	.Region = LORAMAC_REGION_EU868,
	.AdrEnable = false,
	.IsTxConfirmed = LORAMAC_HANDLER_UNCONFIRMED_MSG,
	.TxDatarate = DR_0,
	.PublicNetworkEnable = true,
	.DutyCycleEnabled = true,
	.DataBufferMaxSize = 0u,
	.DataBuffer = NULL,
	.PingSlotPeriodicity = REGION_COMMON_DEFAULT_PING_SLOT_PERIODICITY
};

static uint8_t batt_lev_cb(void)
{
	return 0u;
}

static float temp_cb(void)
{
	return 0.0f;
}

static uint32_t rand_seed_cb(void)
{
	return 0ul;
}

static void mac_cb(void)
{
}

static void nvm_chng_cb(LmHandlerNvmContextStates_t state, uint16_t size)
{
}

static void net_prm_chng_cb(CommissioningParams_t *prm)
{
}

static void mcps_req(LoRaMacStatus_t status, McpsReq_t *req, TimerTime_t delay)
{
	assert(status == LORAMAC_STATUS_OK);
}

static void mlme_req(LoRaMacStatus_t status, MlmeReq_t *req, TimerTime_t delay)
{
	assert(status == LORAMAC_STATUS_OK);
}

static void join_req_cb(LmHandlerJoinParams_t *prm)
{
	if (prm->Status == LORAMAC_HANDLER_ERROR) {
		__BKPT();
		LmHandlerJoin();
	} else {
		assert(LmHandlerRequestClass(CLASS_A) == LORAMAC_HANDLER_SUCCESS);
	}
}

static void tx_cb(LmHandlerTxParams_t *prm)
{
	//assert(prm->Status == LORAMAC_EVENT_INFO_STATUS_OK);
	LED_OFF(LED_D1);
	tx_busy = 0u;
}

static void rx_cb(LmHandlerAppData_t *rx_data, LmHandlerRxParams_t *prm)
{
	assert(prm->Status == LORAMAC_EVENT_INFO_STATUS_OK);

	if (rx_desc_ind < RX_TRACE_LEN) {
		rx_desc[rx_desc_ind] = *rx_data;
		memcpy(rx_buf[rx_desc_ind], rx_data->Buffer, (rx_data->BufferSize < RX_BUF_LEN) ? rx_data->BufferSize : RX_BUF_LEN);
		rx_desc_ind++;
	}

	LED_ON(LED_D2);
}

static void class_change_cb(DeviceClass_t class)
{
	__BKPT();
	/*
	app_data.Buffer = NULL;
	app_data.BufferSize = 0u;
	app_data.Port = 0u;

	assert(LmHandlerSend(&app_data, LORAMAC_HANDLER_UNCONFIRMED_MSG) == LORAMAC_HANDLER_SUCCESS);
	*/
}

static void bcn_stat_chng_cb(LoRaMacHandlerBeaconParams_t *prm)
{
	assert(prm->Status == LORAMAC_EVENT_INFO_STATUS_OK);
}

static void systime_upd_cb(bool sync, int32_t corr)
{
	__BKPT();
}

void test_lora(void)
{
	ChannelParams_t chan_prm;
	MibRequestConfirm_t mib_req;
	LmHandlerAppData_t tx_desc;
	uint8_t ind;
	uint8_t tx_buf[TX_PAYLOAD_SIZE];

	tx_desc.Buffer = tx_buf;
	tx_desc.BufferSize = TX_PAYLOAD_SIZE;
	tx_desc.Port = TX_PORT;

	memset(tx_buf, 0, TX_PAYLOAD_SIZE);

	for (ind = 0u; ind < TX_PAYLOAD_SIZE; ind += 8u) {
		tx_buf[ind] = ind + 1u;
	}

	tx_buf[TX_PAYLOAD_SIZE - 1u] = 0xFFu;

	__BKPT();

	led_init();

	SX126xGlobalInit();
	TimerGlobalInit();
	SysTimeGlobalInit();

	SX126xPwrOn(1u);

	assert(LmHandlerInit(&lmh_cb, &lmh_prm) == LORAMAC_HANDLER_SUCCESS);
	assert(LmHandlerSetSystemMaxRxError(200ul) == LORAMAC_HANDLER_SUCCESS);

#if 1
	mib_req.Type = MIB_CHANNELS_DEFAULT_TX_POWER;
	mib_req.Param.ChannelsDefaultTxPower = TX_POWER_0;
	assert(LoRaMacMibSetRequestConfirm(&mib_req) == LORAMAC_STATUS_OK);

	mib_req.Type = MIB_CHANNELS_TX_POWER;
	mib_req.Param.ChannelsTxPower = TX_POWER_0;
	assert(LoRaMacMibSetRequestConfirm(&mib_req) == LORAMAC_STATUS_OK);
#endif

#if 1
	//chan_prm = (ChannelParams_t) {868300000ul, 0ul, {(DR_6 << 4u) | DR_6}, 1u};
	//assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);

	chan_prm = (ChannelParams_t) {867100000ul, 0ul, {(DR_5 << 4u) | DR_0}, 1u};
	assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);

	chan_prm = (ChannelParams_t) {867300000ul, 0ul, {(DR_5 << 4u) | DR_0}, 1u};
	assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);

	chan_prm = (ChannelParams_t) {867500000ul, 0ul, {(DR_5 << 4u) | DR_0}, 1u};
	assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);

	chan_prm = (ChannelParams_t) {867700000ul, 0ul, {(DR_5 << 4u) | DR_0}, 1u};
	assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);

	chan_prm = (ChannelParams_t) {867900000ul, 0ul, {(DR_5 << 4u) | DR_0}, 1u};
	assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);

	chan_prm = (ChannelParams_t) {868800000ul, 0ul, {(DR_7 << 4u) | DR_7}, 1u};
	assert(LoRaMacChannelAdd(3u, chan_prm) == LORAMAC_STATUS_OK);
#endif

	LmHandlerJoin();

	//tx_buf = 0x5A;

	while (1) {
		LmHandlerProcess();

		if (!tx_busy && btn_get(BTN_ID_SW1) && !LmHandlerIsBusy()) {
			assert(LmHandlerSend(&tx_desc, LORAMAC_HANDLER_UNCONFIRMED_MSG) == LORAMAC_HANDLER_SUCCESS);
			LED_ON(LED_D1);
			tx_busy = 1u;
			//tx_buf++;
		}

		if (btn_get(BTN_ID_SW2)) {
			LED_OFF(LED_D2);
		}
	}

	//TimerDeInitAll();
	//SX126xPwrOff();
}
