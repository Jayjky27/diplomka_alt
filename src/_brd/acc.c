/*
 *==============================================================================
 * Name: acc.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "acc.h"
#include "acc_regs.h"
#include "soc_def.h"
#include "brd_def.h"
#include "port_int.h"
#include "expt.h"
#include "crsec.h"
#include "delay.h"
#include "gpio.h"
#include "assert.h"

#define ACC_RESET_DELAY_MS			20ul

#define ACC_CTRL_ADDR_MASK			0x7Fu
#define ACC_CTRL_RD_MASK			0x80u

#define ACC_CTRL_RD(addr) 			((addr) | ACC_CTRL_RD_MASK)
#define ACC_CTRL_WR(addr)			((addr) & ACC_CTRL_ADDR_MASK)

#define SPI_SENS_CS_ASSERT()		GPIO_OUT_LO(SPI_SENS_CS)
#define SPI_SENS_CS_DEASSERT()		GPIO_OUT_HI(SPI_SENS_CS)

static volatile ACC_SAMP_BLK acc_samp_blk_rx;
static volatile ACC_SAMP_BLK acc_samp_blk_tx;

static volatile ACC_CB acc_cb;

static void acc_int_cb(void);
static void __attribute__ ((interrupt)) acc_dma_handler(void);

static uint8_t acc_reg_rd(uint8_t addr);
static void acc_reg_wr(uint8_t addr, uint8_t val);
static uint8_t acc_spi_poll_rdwr(uint8_t data_out);

static void acc_int_cb(void)
{
#ifdef MARKER_ACC_SAMP
	GPIO_OUT_HI(MARKER_ACC_SAMP);
#endif
#ifdef MARKER_ACC_INT
	GPIO_OUT_HI(MARKER_ACC_INT);
#endif

	PORT_SENS_INT->PCR[IOIND_SENS_INT] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
	PORT_SENS_INT->ISFR = IOMASK_SENS_INT;

	SPI_SENS_CS_ASSERT();

	DMA0->DMA[DMA_CHAN_SPI_SENS_RX].DAR = (uint32_t) &acc_samp_blk_rx;
	DMA0->DMA[DMA_CHAN_SPI_SENS_RX].DSR_BCR = DMA_DSR_BCR_BCR(sizeof(ACC_SAMP_BLK));

	DMA0->DMA[DMA_CHAN_SPI_SENS_TX].SAR = (uint32_t) &acc_samp_blk_tx;
	DMA0->DMA[DMA_CHAN_SPI_SENS_TX].DSR_BCR = DMA_DSR_BCR_BCR(sizeof(ACC_SAMP_BLK));
	DMA0->DMA[DMA_CHAN_SPI_SENS_TX].DCR = DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK |
			DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(1u) | DMA_DCR_DSIZE(1u) | DMA_DCR_D_REQ_MASK;

#ifdef MARKER_ACC_INT
	GPIO_OUT_LO(MARKER_ACC_INT);
#endif
}

static void __attribute__ ((interrupt)) acc_dma_handler(void)
{
#ifdef MARKER_ACC_DMA
	GPIO_OUT_HI(MARKER_ACC_DMA);
#endif

	DMA0->DMA[DMA_CHAN_SPI_SENS_RX].DSR_BCR = DMA_DSR_BCR_DONE_MASK;
	DMA0->DMA[DMA_CHAN_SPI_SENS_TX].DSR_BCR = DMA_DSR_BCR_DONE_MASK;

	SPI_SENS_CS_DEASSERT();

	PORT_SENS_INT->PCR[IOIND_SENS_INT] = PORT_PCR_IRQC(PORT_PCR_IRQC_VAL_INT_LHIGH) |
			PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

#ifdef MARKER_ACC_CB
	GPIO_OUT_HI(MARKER_ACC_CB);
#endif

	acc_cb((ACC_SAMP_BLK *) &acc_samp_blk_rx);

#ifdef MARKER_ACC_CB
	GPIO_OUT_LO(MARKER_ACC_CB);
#endif
#ifdef MARKER_ACC_DMA
	GPIO_OUT_LO(MARKER_ACC_DMA);
#endif
#ifdef MARKER_ACC_SAMP
	GPIO_OUT_LO(MARKER_ACC_SAMP);
#endif
}

static uint8_t acc_reg_rd(uint8_t addr)
{
	uint8_t val;

	SPI_SENS_CS_ASSERT();

	acc_spi_poll_rdwr(ACC_CTRL_RD(addr));
	acc_spi_poll_rdwr(0u);
	val = acc_spi_poll_rdwr(0u);

	SPI_SENS_CS_DEASSERT();

	return val;
}

static void acc_reg_wr(uint8_t addr, uint8_t val)
{
	SPI_SENS_CS_ASSERT();

	acc_spi_poll_rdwr(ACC_CTRL_WR(addr));
	acc_spi_poll_rdwr(0u);
	acc_spi_poll_rdwr(val);

	SPI_SENS_CS_DEASSERT();
}

static uint8_t acc_spi_poll_rdwr(uint8_t data_out)
{
	SPI_SENS->S;
	SPI_SENS->D = data_out;

	while (!(SPI_SENS->S & SPI_S_SPRF_MASK));
	return SPI_SENS->D;
}

uint8_t acc_init(void)
{
	uint8_t prod_rev;

	PORT_SENS_INT->PCR[IOIND_SENS_INT] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	SPI_SENS_CS_DEASSERT();
	GPIO_MODE_OUT(SPI_SENS_CS);
	PORT_SPI_SENS_CS->PCR[IOIND_SPI_SENS_CS] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);

	PORT_SPI_SENS_SCK->PCR[IOIND_SPI_SENS_SCK] = PORT_PCR_MUX(MUX_SPI_SENS_SCK);
	PORT_SPI_SENS_MOSI->PCR[IOIND_SPI_SENS_MOSI] = PORT_PCR_MUX(MUX_SPI_SENS_MOSI);
	PORT_SPI_SENS_MISO->PCR[IOIND_SPI_SENS_MISO] = PORT_PCR_MUX(MUX_SPI_SENS_MISO) |
			PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

	SPI_SENS->BR = SPI_BR_SPPR(SPI_SENS_BR_SPPR_VAL) | SPI_BR_SPR(SPI_SENS_BR_SPR_VAL);
	SPI_SENS->C1 = SPI_C1_SPE_MASK | SPI_C1_MSTR_MASK;

	delay(DELAY_MS_TO_CNT(ACC_RESET_DELAY_MS));
	acc_reg_wr(ACC_ADDR_CFG1, ACC_CFG1_RST_ENC(1u));
	delay(DELAY_MS_TO_CNT(ACC_RESET_DELAY_MS));

	assert(acc_reg_rd(ACC_ADDR_WHOAMI) == ACC_WHOAMI_FXLS8974CF);
	assert(ACC_SMODE_MODE_DEC(acc_reg_rd(ACC_ADDR_SMODE)) == ACC_SMODE_MODE_STDB);

	prod_rev = acc_reg_rd(ACC_ADDR_PROD_REV);

	DMAMUX0->CHCFG[DMA_CHAN_SPI_SENS_RX] = DMAMUX_CHCFG_ENBL_MASK |
			DMAMUX_CHCFG_SOURCE(DMAMUX_SRC_SPI_SENS_RX);
	DMAMUX0->CHCFG[DMA_CHAN_SPI_SENS_TX] = DMAMUX_CHCFG_ENBL_MASK |
			DMAMUX_CHCFG_SOURCE(DMAMUX_SRC_SPI_SENS_TX);

	DMA0->DMA[DMA_CHAN_SPI_SENS_RX].SAR = (uint32_t) &SPI_SENS->D;
	DMA0->DMA[DMA_CHAN_SPI_SENS_RX].DCR = DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK |
			DMA_DCR_SSIZE(1u) | DMA_DCR_DINC_MASK | DMA_DCR_DSIZE(1u);

	DMA0->DMA[DMA_CHAN_SPI_SENS_TX].DAR = (uint32_t) &SPI_SENS->D;

	expt_vect_set(IRQN_DMA_SPI_SENS_RX, acc_dma_handler);
	NVIC_EnableIRQ(IRQN_DMA_SPI_SENS_RX);

	port_int_en_pin(IRQN_SENS_INT, acc_int_cb, IOIND_SENS_INT);

	acc_samp_blk_tx.ctrl = ACC_CTRL_RD(ACC_ADDR_SAMP_BLK);

	return prod_rev;
}

void acc_enable(ACC_CB cb, uint8_t cfg1_fsr, uint8_t cfg2_wakepm, uint8_t cfg3_wakeodr)
{
	uint8_t reg_val;

	acc_reg_wr(ACC_ADDR_CFG1, ACC_CFG1_RST_ENC(1u));
	delay(DELAY_MS_TO_CNT(ACC_RESET_DELAY_MS));

	assert(acc_reg_rd(ACC_ADDR_WHOAMI) == ACC_WHOAMI_FXLS8974CF);
	assert(ACC_SMODE_MODE_DEC(acc_reg_rd(ACC_ADDR_SMODE)) == ACC_SMODE_MODE_STDB);

	//additional register check here ?

	acc_reg_wr(ACC_ADDR_CFG2, ACC_CFG2_WAKEPM_ENC(cfg2_wakepm) | ACC_CFG2_AINCTEMP_ENC(1u));
	reg_val = acc_reg_rd(ACC_ADDR_CFG2);
	assert(ACC_CFG2_WAKEPM_DEC(reg_val) == cfg2_wakepm);
	assert(ACC_CFG2_AINCTEMP_DEC(reg_val) == 1u);

	acc_reg_wr(ACC_ADDR_CFG3, ACC_CFG3_WAKEODRDEC_ENC(cfg3_wakeodr));
	assert(ACC_CFG3_WAKEODRDEC_DEC(acc_reg_rd(ACC_ADDR_CFG3)) == cfg3_wakeodr);

	acc_reg_wr(ACC_ADDR_CFG5, ACC_CFG5_VECMEN_ENC(1u));
	assert(ACC_CFG5_VECMEN_DEC(acc_reg_rd(ACC_ADDR_CFG5)) == 1u);

	acc_reg_wr(ACC_ADDR_INTEN, ACC_INTEN_DRDYEN_ENC(1u));
	assert(ACC_INTEN_DRDYEN_DEC(acc_reg_rd(ACC_ADDR_INTEN)) == 1u);

	acc_reg_wr(ACC_ADDR_INTPIN, ACC_INTPIN_DRDYINT2_ENC(1u));
	assert(ACC_INTPIN_DRDYINT2_DEC(acc_reg_rd(ACC_ADDR_INTPIN)) == 1u);

	acc_reg_wr(ACC_ADDR_CFG1, ACC_CFG1_FSR_ENC(cfg1_fsr) | ACC_CFG1_ACT_ENC(1u));
	reg_val = acc_reg_rd(ACC_ADDR_CFG1);
	assert(ACC_CFG1_FSR_DEC(reg_val) == cfg1_fsr);
	assert(ACC_CFG1_ACT_DEC(reg_val) == 1u);

	assert(ACC_SMODE_MODE_DEC(acc_reg_rd(ACC_ADDR_SMODE)) == ACC_SMODE_MODE_WAKE);

	SPI_SENS->C1 = 0;
	SPI_SENS->C2 = SPI_C2_TXDMAE_MASK | SPI_C2_RXDMAE_MASK;
	SPI_SENS->C1 = SPI_C1_SPE_MASK | SPI_C1_MSTR_MASK;

	acc_cb = cb;

	PORT_SENS_INT->PCR[IOIND_SENS_INT] = PORT_PCR_IRQC(PORT_PCR_IRQC_VAL_INT_LHIGH) |
			PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
}

void acc_disable(void)
{
	crsec_enter();

	PORT_SENS_INT->PCR[IOIND_SENS_INT] = PORT_PCR_MUX(PORT_PCR_MUX_VAL_GPIO);
	PORT_SENS_INT->ISFR = IOMASK_SENS_INT;

	SPI_SENS->C1 = 0u;
	SPI_SENS->C2 = 0u;

	DMA0->DMA[DMA_CHAN_SPI_SENS_RX].DSR_BCR = DMA_DSR_BCR_DONE_MASK;
	DMA0->DMA[DMA_CHAN_SPI_SENS_TX].DSR_BCR = DMA_DSR_BCR_DONE_MASK;

	NVIC_ClearPendingIRQ(IRQN_DMA_SPI_SENS_RX);

	crsec_leave();

	SPI_SENS_CS_DEASSERT();

	SPI_SENS->C1 = SPI_C1_SPE_MASK | SPI_C1_MSTR_MASK;

	acc_reg_wr(ACC_ADDR_CFG1, ACC_CFG1_ACT_ENC(0u));
	assert(ACC_CFG1_ACT_DEC(acc_reg_rd(ACC_ADDR_CFG1)) == 0u);

	assert(ACC_SMODE_MODE_DEC(acc_reg_rd(ACC_ADDR_SMODE)) == ACC_SMODE_MODE_STDB);
}
