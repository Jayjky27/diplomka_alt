/*
 *==============================================================================
 * Name: com.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "com.h"
#include "soc_def.h"
#include "brd_def.h"
#include "expt.h"
#include "gpio.h"
#include "crsec.h"

static volatile uint8_t com_flag_busy = 0u;

static void __attribute__ ((interrupt)) com_dma_handler(void);

static void __attribute__ ((interrupt)) com_dma_handler(void)
{
	DMA0->DMA[DMA_CHAN_UART_COM_TX].DSR_BCR = DMA_DSR_BCR_DONE_MASK;
	com_flag_busy = 0u;

#ifdef MARKER_COM
	GPIO_OUT_LO(MARKER_COM);
#endif
}

void com_init(void)
{
	PORT_UART_COM_RX->PCR[IOIND_UART_COM_RX] = PORT_PCR_MUX(MUX_UART_COM_RX);
	PORT_UART_COM_TX->PCR[IOIND_UART_COM_TX] = PORT_PCR_MUX(MUX_UART_COM_TX);

	UART_COM->BDH = UART_BDH_SBR(UART_COM_BD_SBR_VAL >> 8u);
	UART_COM->BDL = UART_BDL_SBR(UART_COM_BD_SBR_VAL);

	UART_COM->C4 = UART_C4_TDMAS_MASK;
	UART_COM->C2 = UART_C2_TIE_MASK | UART_C2_TE_MASK;

	DMAMUX0->CHCFG[DMA_CHAN_UART_COM_TX] = DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(DMAMUX_SRC_UART_COM_TX);
	DMA0->DMA[DMA_CHAN_UART_COM_TX].DAR = (uint32_t) &UART_COM->D;

	expt_vect_set(IRQN_DMA_UART_COM_TX, com_dma_handler);
	NVIC_EnableIRQ(IRQN_DMA_UART_COM_TX);
}

uint8_t com_try_lock(void)
{
	crsec_enter();

	if (com_flag_busy) {
		crsec_leave();
		return 0u;
	}

	com_flag_busy = 1u;
	crsec_leave();

	return 1u;
}

void com_send(const void *data, uint16_t len)
{
#ifdef MARKER_COM
	GPIO_OUT_HI(MARKER_COM);
#endif

	DMA0->DMA[DMA_CHAN_UART_COM_TX].SAR = (uint32_t) data;
	DMA0->DMA[DMA_CHAN_UART_COM_TX].DSR_BCR = DMA_DSR_BCR_BCR(len);
	DMA0->DMA[DMA_CHAN_UART_COM_TX].DCR = DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK |
			DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(1u) | DMA_DCR_DSIZE(1u) | DMA_DCR_D_REQ_MASK;
}
