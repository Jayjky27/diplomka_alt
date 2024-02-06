/*
 *==============================================================================
 * Name: brd_def_periph.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _BRD_DEF_PERIPH_H_
#define _BRD_DEF_PERIPH_H_

#include "mkl25z4.h"

#include "soc_def.h"

#define SPI_SENS					SPI0
#define	SPI_LORA					SPI1
#define	IIC_EEPROM					I2C1
#define	UART_COM					UART1

#define IRQN_SPI_SENS				SPI0_IRQn
#define IRQN_SPI_LORA				SPI1_IRQn
#define IRQN_IIC_EEPROM				I2C1_IRQn
#define IRQN_UART_COM				UART1_IRQn

#define IRQN_DMA_UART_COM_TX		DMA0_IRQn
#define IRQN_DMA_SPI_SENS_RX		DMA1_IRQn
#define IRQN_DMA_SPI_SENS_TX		DMA2_IRQn

#define DMA_CHAN_UART_COM_TX		0u
#define DMA_CHAN_SPI_SENS_RX		1u
#define DMA_CHAN_SPI_SENS_TX		2u

#define DMAMUX_SRC_UART_COM_TX		DMAMUX_SRC_UART1_TX
#define DMAMUX_SRC_SPI_SENS_RX		DMAMUX_SRC_SPI0_RX
#define DMAMUX_SRC_SPI_SENS_TX		DMAMUX_SRC_SPI0_TX

#endif /* _BRD_DEF_PERIPH_H_ */
