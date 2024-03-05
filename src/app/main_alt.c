/*
 * main_alt.c
 *
 *  Created on: 6. 2. 2024
 *      Author: Jan Fiedor
 */

#include "main_alt.h"
#include "helper_functions.h"
#include "assert.h"
#include "utilities.h"
#include "delay.h"

/* LORAWAN INCLUDES */
#include "led.h" // TO BE DELETED
#include "LmHandler.h"
#include "RegionCommon.h"
#include "sx126x-board.h"
#include "systime.h"
#include "timer.h"


/* LORAWAN DEFINES */
#define TX_PAYLOAD_SIZE			32u
#define RX_TRACE_LEN			8u
#define RX_BUF_LEN				32u
#define TX_PORT					3u

/* LORAWAN FUNCTIONS PROTOTYPES */
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
void lora_init_complet(LmHandlerCallbacks_t *lmh_cb, LmHandlerParams_t *lmh_prm, MibRequestConfirm_t mib_req, ChannelParams_t chan_prm);

/* LORAWAN GLOBAL VARIABLES + STRUCTURES */
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

/* DEFINITIONS OF THE LORAWAN FUNCTIONS */
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

int lptmrIntFlag;
uint8_t okFlag;

uint8_t itterationCnt = 0;

void alt_main(void)
{
	okFlag = 0;
	/* INIT SECTION */
	led_init();
	initButton();

	assert(!initADC());
	initPins();
	initLPTMR();

	float Qtable[NUM_STATES][NUM_ACTIONS], reward = 0.0, tempValue = 0.0, stateOfCharge = 0.0;
	uint16_t valueLPTMR = 0;
	int action = 0, currentState = 0, previousState = 0, epochsCnt = 1, epsilon = 90;
	init_Qtable(Qtable);

	/* LORAWAN LOCAL VARIABLES + STRUCTURES */
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

	/* LORAWAN INIT FUNCTIONS */
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
		//static LoRaMacPrimitives_t LoRaMacPrimitives;
		LmHandlerJoin();

	while(1){
		/* MAIN LOOP */
		// DEINITIALIZATION OF LORAMAC
		if(epochsCnt > 1){
			lora_init_complet(&lmh_cb, &lmh_prm, mib_req, chan_prm);
		}
		LmHandlerProcess();

		/* DETERMINATION OF ENERGY STATUS */
		assert(!initADC());
		stateOfCharge = getEnergy();

		if(stateOfCharge < 10){
			currentState = 0;
		}else if(stateOfCharge >= 80){
			currentState = 3;
		}else if((stateOfCharge >= 10) && (stateOfCharge < 40)){
			currentState = 1;
		}else{
			currentState = 2;
		}

		/* CALCULATION OF THE REWARD + UPDATING THE VALUES IN THE Q-TABLE */
		if (epochsCnt > 1) {
			reward = STATE_WEIGHT*(currentState - previousState) - ACTION_WEIGHT*(action + 1);
			updateQ(Qtable, previousState, action, reward, currentState);
		}

		/* MEAS FUNCTION */
		tempValue = getTemp();
		/* Data preparation for sending */
		dataPrep(tx_buf, tempValue);
		/* SEND FUNCTION */
		tx_buf[4] = itterationCnt;
		tx_buf[5] = okFlag;
		if(!tx_busy && !LmHandlerIsBusy()) {
			assert(LmHandlerSend(&tx_desc, LORAMAC_HANDLER_UNCONFIRMED_MSG) == LORAMAC_HANDLER_SUCCESS);
			//LED_ON(LED_D1);
		}
		while(tx_busy);
		while(LmHandlerIsBusy()){
			LmHandlerProcess();
		}

		/* SELECTION OF ACTION */
		action = selectAction(Qtable, currentState, epochsCnt, epsilon);

		switch(action){
			case 0:
				valueLPTMR = 6000;
				break;

			case 1:
				valueLPTMR = 4000;
				break;

			case 2:
				valueLPTMR = 2000;
				break;

			case 3:
				valueLPTMR = 1000;
				break;
			default:
				assert(0);
				break;
		}

		/* OPERATIONS NEEDED FOR TRAINING */
		if(epochsCnt <= EPOCHS){
			epsilon -= 8;
			epochsCnt++;
		}

		/* DEINITIALIZATION OF MODULES */
		TimerDeInitAll();
		assert(!(LoRaMacDeInitialization()));
		SX126xPwrOff();
		// Deinit of ADC
		deInitADC();
		//LED_OFF(LED_D1);

		/* STARTING LPTMR + ENTERING SLEEP MODE */
		lptmrIntFlag = 0;
		initLPTMR();
		startLPTMR(valueLPTMR);
		assert(setVLPS());
		itterationCnt++;
		previousState = currentState;
	}
}


void lora_init_complet(LmHandlerCallbacks_t *lmh_cb, LmHandlerParams_t *lmh_prm, MibRequestConfirm_t mib_req, ChannelParams_t chan_prm)
{
	SX126xPwrOn(1u);

	//lmh_prm->Region = LORAMAC_REGION_EU868;

	assert(LmHandlerInit(lmh_cb, lmh_prm) == LORAMAC_HANDLER_SUCCESS);
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
		//static LoRaMacPrimitives_t LoRaMacPrimitives;
		LmHandlerJoin();
}
