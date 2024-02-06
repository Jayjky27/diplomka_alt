/*
 *==============================================================================
 * Name: brd_def_clk.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _BRD_DEF_CLK_H_
#define _BRD_DEF_CLK_H_

#if VLP

#define FREQ_CPU				4000000ul
#define FREQ_BUS				800000ul

#define DELAY_CNT_PER_MS		1310ul

//fsck = 400 kHz
#define SPI_SENS_BR_SPPR_VAL	0u
#define SPI_SENS_BR_SPR_VAL		0u

//fsck = 2 MHz
#define SPI_LORA_BR_SPPR_VAL	0u
#define SPI_LORA_BR_SPR_VAL		0u

//50000 bps
#define UART_COM_BD_SBR_VAL		1u

#else

#define FREQ_CPU				4000000ul
#define FREQ_BUS				4000000ul

#define DELAY_CNT_PER_MS		1310ul

//fsck = 400 kHz
#define SPI_SENS_BR_SPPR_VAL	4u
#define SPI_SENS_BR_SPR_VAL		0u

//fsck = 2 MHz
#define SPI_LORA_BR_SPPR_VAL	0u
#define SPI_LORA_BR_SPR_VAL		0u

//50000 bps
#define UART_COM_BD_SBR_VAL		5u

#endif

#define DELAY_MS_TO_CNT(time)	(DELAY_CNT_PER_MS * (time))

#endif /* _BRD_DEF_CLK_H_ */
