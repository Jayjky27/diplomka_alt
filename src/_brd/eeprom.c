/*
 *==============================================================================
 * Name: eeprom.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "eeprom.h"
#include "soc_def.h"
#include "brd_def.h"
#include "gpio.h"
#include "delay.h"
#include "assert.h"
//#include "crsec.h"

#define EEPROM_PAGE_SIZE_MASK		0x1Fu
#define EEPROM_PAGE_SIZE			(EEPROM_PAGE_SIZE_MASK + 1u)
#define EEPROM_PAGE_COUNT			512u
#define EEPROM_SIZE					(EEPROM_PAGE_SIZE * EEPROM_PAGE_COUNT)

#define EEPROM_IIC_ADDR_RD			0xA1u
#define EEPROM_IIC_ADDR_WR			0xA0u

#define EEPROM_POWER_ON_DELAY_MS	5ul
#define EEPROM_POWER_OFF_DELAY_MS	5ul

#define EEPROM_PWR_CTRL_ASSERT()	GPIO_OUT_HI(EEPROM_PWR_CTRL)
#define EEPROM_PWR_CTRL_DEASSERT()	GPIO_OUT_LO(EEPROM_PWR_CTRL)

static void eeprom_iic_start(void);
static void eeprom_iic_repstart(void);
static void eeprom_iic_stop(void);
static void eeprom_iic_rd(uint8_t *data, uint8_t last);
static uint8_t eeprom_iic_wr(uint8_t data);

static uint8_t eeprom_op_prologue(uint16_t eeprom_addr, uint16_t len, uint8_t pwr_off_delay);
static void eeprom_op_epilogue(void);
static void eeprom_ack_poll(void);

static void eeprom_iic_start(void)
{
	IIC_EEPROM->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK;
}

static void eeprom_iic_repstart(void)
{
	IIC_EEPROM->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK | I2C_C1_RSTA_MASK;
}

static void eeprom_iic_stop(void)
{
	IIC_EEPROM->C1 = I2C_C1_IICEN_MASK;
	while (BME_UBFX_B(&IIC_EEPROM->S, I2C_S_BUSY_SHIFT, 1u));
}

static void eeprom_iic_rd(uint8_t *data, uint8_t last)
{
	if (last) {
		IIC_EEPROM->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TXAK_MASK;
	} else {
		IIC_EEPROM->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK;
	}

	IIC_EEPROM->D;

	while (!BME_UBFX_B(&IIC_EEPROM->S, I2C_S_IICIF_SHIFT, 1u));
	IIC_EEPROM->S = I2C_S_IICIF_MASK;

	IIC_EEPROM->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK;

	*data = IIC_EEPROM->D;
}

static uint8_t eeprom_iic_wr(uint8_t data)
{
	IIC_EEPROM->D = data;

	while (!BME_UBFX_B(&IIC_EEPROM->S, I2C_S_IICIF_SHIFT, 1u));
	IIC_EEPROM->S = I2C_S_IICIF_MASK;

	if (BME_UBFX_B(&IIC_EEPROM->S, I2C_S_RXAK_SHIFT, 1u)) {
		return 1;
	}

	return 0;
}

static uint8_t eeprom_op_prologue(uint16_t eeprom_addr, uint16_t len, uint8_t pwr_off_delay)
{
	assert((eeprom_addr + len) <= EEPROM_SIZE);
	assert(len > 0u);

	if (pwr_off_delay) {
		delay(DELAY_MS_TO_CNT(EEPROM_POWER_OFF_DELAY_MS));
	}

	EEPROM_PWR_CTRL_ASSERT();

	PORT_IIC_EEPROM_SDA->PCR[IOIND_IIC_EEPROM_SDA] = PORT_PCR_MUX(MUX_IIC_EEPROM_SDA);
	PORT_IIC_EEPROM_SCL->PCR[IOIND_IIC_EEPROM_SCL] = PORT_PCR_MUX(MUX_IIC_EEPROM_SCL);

	delay(DELAY_MS_TO_CNT(EEPROM_POWER_ON_DELAY_MS));

	eeprom_iic_start();

	if (eeprom_iic_wr(EEPROM_IIC_ADDR_WR)) {
		return 1u;
	}

	if (eeprom_iic_wr(eeprom_addr >> 8u)) {
		eeprom_op_epilogue();
		return 1u;
	}

	if (eeprom_iic_wr(eeprom_addr)) {
		eeprom_op_epilogue();
		return 1u;
	}

	return 0;
}

static void eeprom_op_epilogue(void)
{
	eeprom_iic_stop();

	IIC_EEPROM->C1 = 0u;

	PORT_IIC_EEPROM_SDA->PCR[IOIND_IIC_EEPROM_SDA] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);
	PORT_IIC_EEPROM_SCL->PCR[IOIND_IIC_EEPROM_SCL] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_DIS);

	EEPROM_PWR_CTRL_DEASSERT();
}

static void eeprom_ack_poll(void)
{
	while (1) {
		eeprom_iic_start();
		if (!eeprom_iic_wr(EEPROM_IIC_ADDR_WR)) {
			break;
		}
		eeprom_iic_stop();
	}
}

void eeprom_init(void)
{
	EEPROM_PWR_CTRL_DEASSERT();
	GPIO_MODE_OUT(EEPROM_PWR_CTRL);
	PORT_EEPROM_PWR_CTRL->PCR[IOIND_EEPROM_PWR_CTRL] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
}

uint8_t eeprom_rd(uint16_t eeprom_addr, void *dst, uint16_t len, uint8_t pwr_off_delay)
{
	uint16_t data_ind;

	if (eeprom_op_prologue(eeprom_addr, len, pwr_off_delay)) {
		eeprom_op_epilogue();
		return 1u;
	}

	eeprom_iic_repstart();

	if (eeprom_iic_wr(EEPROM_IIC_ADDR_RD)) {
		eeprom_op_epilogue();
		return 1u;
	}

	for (data_ind = 0u; data_ind < (len - 1u); data_ind++) {
		eeprom_iic_rd(((uint8_t *) dst) + data_ind, 0u);
	}

	eeprom_iic_rd(((uint8_t *) dst) + data_ind, 1u);

	eeprom_op_epilogue();

	return 0;
}

uint8_t eeprom_wr(uint16_t eeprom_addr, const void *src, uint16_t len, uint8_t pwr_off_delay)
{
	uint16_t data_ind;

	if (eeprom_op_prologue(eeprom_addr, len, pwr_off_delay)) {
		eeprom_op_epilogue();
		return 1;
	}

	for (data_ind = 0u; data_ind < len; data_ind++) {
		if (eeprom_iic_wr(((uint8_t *) src)[data_ind])) {
			eeprom_op_epilogue();
			return 1;
		}

		eeprom_addr++;

		if ((eeprom_addr & EEPROM_PAGE_SIZE_MASK) == 0u) {
			eeprom_iic_stop();
			eeprom_ack_poll();

			if (eeprom_iic_wr(eeprom_addr >> 8u)) {
				eeprom_op_epilogue();
				return 1u;
			}

			if (eeprom_iic_wr(eeprom_addr)) {
				eeprom_op_epilogue();
				return 1u;
			}
		}
	}

	eeprom_iic_stop();
	eeprom_ack_poll();

	eeprom_op_epilogue();

	return 0;
}
