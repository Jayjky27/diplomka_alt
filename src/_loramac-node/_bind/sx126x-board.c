/*!
 * \file      sx126x-board.c
 *
 * \brief     Target board SX126x driver implementation
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
#include "brd_def.h"
#include "port_int.h"
#include "expt.h"
#include "gpio.h"
#include "assert.h"

#include "sx126x-board.h"
#include "utilities.h"
#include "cb_def.h"
#include "radio.h"
#include "sx126x.h"

#define LORA_PWR_ON_DELAY_MS		250ul
#define LORA_PWR_OFF_DELAY_MS		50ul
#define LORA_BOOT_DELAY_MS			100ul

#define SPI_LORA_CS_ASSERT()		GPIO_OUT_LO(SPI_LORA_CS)
#define SPI_LORA_CS_DEASSERT()		GPIO_OUT_HI(SPI_LORA_CS)

#define LORA_DIO1_GET()				GPIO_IN(LORA_DIO1)

#define LORA_BUSY_GET()				GPIO_IN(LORA_BUSY)

#define LORA_PWR_CTRL_ASSERT()		GPIO_OUT_HI(LORA_PWR_CTRL)
#define LORA_PWR_CTRL_DEASSERT()	GPIO_OUT_LO(LORA_PWR_CTRL)

#define LORA_NRST_ASSERT()			GPIO_OUT_LO(LORA_NRST)
#define LORA_NRST_DEASSERT()		GPIO_OUT_HI(LORA_NRST)

static RadioOperatingModes_t OperatingMode;
static DioIrqHandler *dio1_cb = cb_def;
static volatile uint32_t dio1_int_cnt;
static volatile uint32_t busy_iters;

static void dio1_port_int_cb(void);
static uint8_t spi_rdwr(uint8_t data_out);

static void dio1_port_int_cb(void)
{
	PORT_LORA_DIO1->ISFR = IOMASK_LORA_DIO1;

	dio1_int_cnt++;
	dio1_cb(NULL);
}

static uint8_t spi_rdwr(uint8_t data_out)
{
	SPI_LORA->S;
	SPI_LORA->D = data_out;

	while (!BME_UBFX_B(&SPI_LORA->S, SPI_S_SPRF_SHIFT, 1u));
	return SPI_LORA->D;
}

void SX126xGlobalInit(void)
{
	LORA_PWR_CTRL_DEASSERT();
	GPIO_MODE_OUT(LORA_PWR_CTRL);
	PORT_LORA_PWR_CTRL->PCR[IOIND_LORA_PWR_CTRL] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	GPIO_MODE_OUT(LORA_NRST);
	GPIO_MODE_OUT(SPI_LORA_CS);

	SPI_LORA->BR = SPI_BR_SPPR(SPI_LORA_BR_SPPR_VAL) | SPI_BR_SPR(SPI_LORA_BR_SPR_VAL);
	SPI_LORA->C1 = SPI_C1_SPE_MASK | SPI_C1_MSTR_MASK;

	port_int_en_pin(IRQN_LORA_DIO1, dio1_port_int_cb, IOIND_LORA_DIO1);
}

void SX126xPwrOn(uint8_t pwr_off_delay)
{
	if (pwr_off_delay) {
		DelayMs(LORA_PWR_OFF_DELAY_MS);
	}

	LORA_PWR_CTRL_ASSERT();

	LORA_NRST_ASSERT();
	PORT_LORA_NRST->PCR[IOIND_LORA_NRST] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	SPI_LORA_CS_DEASSERT();
	PORT_SPI_LORA_CS->PCR[IOIND_SPI_LORA_CS] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	PORT_SPI_LORA_MOSI->PCR[IOIND_SPI_LORA_MOSI] = PORT_PCR_MUX(MUX_SPI_LORA_MOSI);
	PORT_SPI_LORA_MISO->PCR[IOIND_SPI_LORA_MISO] = PORT_PCR_MUX(MUX_SPI_LORA_MISO);
	PORT_SPI_LORA_SCK->PCR[IOIND_SPI_LORA_SCK] = PORT_PCR_MUX(MUX_SPI_LORA_SCK);

	PORT_LORA_BUSY->PCR[IOIND_LORA_BUSY] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	DelayMs(LORA_PWR_ON_DELAY_MS);

	LORA_NRST_DEASSERT();

	DelayMs(LORA_BOOT_DELAY_MS);

	PORT_LORA_DIO1->PCR[IOIND_LORA_DIO1] = PORT_PCR_IRQC(PORT_PCR_IRQC_VAL_INT_ERISE) |
											PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
}

void SX126xPwrOff(void)
{
	PORT_LORA_DIO1->PCR[IOIND_LORA_DIO1] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS); // PORTA->PCR[4] = MUX(0)
	PORT_LORA_BUSY->PCR[IOIND_LORA_BUSY] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

	PORT_SPI_LORA_CS->PCR[IOIND_SPI_LORA_CS] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS); // PORTB
	PORT_SPI_LORA_MOSI->PCR[IOIND_SPI_LORA_MOSI] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
	PORT_SPI_LORA_MISO->PCR[IOIND_SPI_LORA_MISO] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
	PORT_SPI_LORA_SCK->PCR[IOIND_SPI_LORA_SCK] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

	PORT_LORA_NRST->PCR[IOIND_LORA_NRST] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

	LORA_PWR_CTRL_DEASSERT();
}

void SX126xIoIrqInit(DioIrqHandler dioIrq)
{
	dio1_cb = dioIrq;
}

void SX126xIoTcxoInit(void)
{
}

uint32_t SX126xGetBoardTcxoWakeupTime(void)
{
	return 0ul;
}

void SX126xIoRfSwitchInit( void )
{
	SX126xSetDio2AsRfSwitchCtrl(true);
}

RadioOperatingModes_t SX126xGetOperatingMode(void)
{
	return OperatingMode;
}

void SX126xSetOperatingMode(RadioOperatingModes_t mode)
{
	OperatingMode = mode;
}

void SX126xReset(void)
{
}

void SX126xWaitOnBusy(void)
{
	while (LORA_BUSY_GET()) {
		busy_iters++;
	}
}

void SX126xWakeup(void)
{
	CRITICAL_SECTION_BEGIN();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(RADIO_GET_STATUS);
	spi_rdwr(0u);

	SPI_LORA_CS_DEASSERT();

	SX126xWaitOnBusy();

	SX126xSetOperatingMode(MODE_STDBY_RC);

	CRITICAL_SECTION_END();
}

void SX126xWriteCommand(RadioCommands_t command, uint8_t *buffer,uint16_t size )
{
	SX126xCheckDeviceReady();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(command);

	for(uint16_t i = 0; i < size; i++) {
		spi_rdwr(buffer[i]);
	}

	SPI_LORA_CS_DEASSERT();

	if(command != RADIO_SET_SLEEP) {
		SX126xWaitOnBusy();
	}
}

uint8_t SX126xReadCommand(RadioCommands_t command, uint8_t *buffer, uint16_t size)
{
	uint8_t status;

	SX126xCheckDeviceReady();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(command);
	status = spi_rdwr(0u);

	for(uint16_t i = 0; i < size; i++) {
		buffer[i] = spi_rdwr(0u);
	}

	SPI_LORA_CS_DEASSERT();

	SX126xWaitOnBusy();

	return status;
}

void SX126xWriteRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
	SX126xCheckDeviceReady();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(RADIO_WRITE_REGISTER);
	spi_rdwr(address >> 8u);
	spi_rdwr(address);

	for(uint16_t i = 0; i < size; i++) {
		spi_rdwr(buffer[i]);
    }

	SPI_LORA_CS_DEASSERT();

	SX126xWaitOnBusy();
}

void SX126xWriteRegister(uint16_t address, uint8_t value)
{
	SX126xWriteRegisters(address, &value, 1u);
}

void SX126xReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
	SX126xCheckDeviceReady();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(RADIO_READ_REGISTER);
	spi_rdwr(address >> 8u);
	spi_rdwr(address);
	spi_rdwr(0u);

	for(uint16_t i = 0; i < size; i++) {
		buffer[i] = spi_rdwr(0u);
    }

	SPI_LORA_CS_DEASSERT();

	SX126xWaitOnBusy();
}

uint8_t SX126xReadRegister(uint16_t address)
{
	uint8_t data;

	SX126xReadRegisters(address, &data, 1u);

	return data;
}

void SX126xWriteBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
	SX126xCheckDeviceReady();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(RADIO_WRITE_BUFFER);
	spi_rdwr(offset);

	for (uint16_t i = 0; i < size; i++) {
		spi_rdwr(buffer[i]);
	}

	SPI_LORA_CS_DEASSERT();

	SX126xWaitOnBusy();
}

void SX126xReadBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
	SX126xCheckDeviceReady();

	SPI_LORA_CS_ASSERT();

	spi_rdwr(RADIO_READ_BUFFER);
    spi_rdwr(offset);
    spi_rdwr(0u);

	for (uint16_t i = 0; i < size; i++) {
    	buffer[i] = spi_rdwr(0u);
	}

	SPI_LORA_CS_DEASSERT();

	SX126xWaitOnBusy();
}

void SX126xSetRfTxPower(int8_t power)
{
	SX126xSetTxParams(power, RADIO_RAMP_40_US);
}

uint8_t SX126xGetDeviceId(void)
{
	return SX1261;
}

void SX126xAntSwOn(void)
{
	//ANT_SW = 1
}

void SX126xAntSwOff(void)
{
	//ANT_SW = HIZ
}

uint32_t SX126xGetDio1PinState(void)
{
	return LORA_DIO1_GET();
}
