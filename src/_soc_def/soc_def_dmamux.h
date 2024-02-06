/*
 *==============================================================================
 * Name: soc_def_dmamux.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _SOC_DEF_DMAMUX_H_
#define _SOC_DEF_DMAMUX_H_

#define DMAMUX_SRC_DIS			0u

#define DMAMUX_SRC_UART0_RX		2u
#define DMAMUX_SRC_UART0_TX		3u

#define DMAMUX_SRC_UART1_RX		4u
#define DMAMUX_SRC_UART1_TX		5u

#define DMAMUX_SRC_UART2_RX		6u
#define DMAMUX_SRC_UART2_TX		7u

#define DMAMUX_SRC_SPI0_RX		16u
#define DMAMUX_SRC_SPI0_TX		17u

#define DMAMUX_SRC_SPI1_RX		18u
#define DMAMUX_SRC_SPI1_TX		19u

#define DMAMUX_SRC_I2C0			22u
#define DMAMUX_SRC_I2C1			23u

#define DMAMUX_SRC_TPM0CH0		24u
#define DMAMUX_SRC_TPM0CH1		25u
#define DMAMUX_SRC_TPM0CH2		26u
#define DMAMUX_SRC_TPM0CH3		27u
#define DMAMUX_SRC_TPM0CH4		28u
#define DMAMUX_SRC_TPM0CH5		29u

#define DMAMUX_SRC_TPM1CH0		32u
#define DMAMUX_SRC_TPM1CH1		33u

#define DMAMUX_SRC_TPM2CH0		34u
#define DMAMUX_SRC_TPM2CH1		35u

#define DMAMUX_SRC_ADC0			40u

#define DMAMUX_SRC_CMP0			42u

#define DMAMUX_SRC_DAC0			45u

#define DMAMUX_SRC_PORTA		49u
#define DMAMUX_SRC_PORTD		52u

#define DMAMUX_SRC_TPM0OVF		54u
#define DMAMUX_SRC_TPM1OVF		55u
#define DMAMUX_SRC_TPM2OVF		56u

#define DMAMUX_SRC_TSI			57u

#define DMAMUX_SRC_ALWEN0		60u
#define DMAMUX_SRC_ALWEN1		61u
#define DMAMUX_SRC_ALWEN2		62u
#define DMAMUX_SRC_ALWEN3		63u

#endif /* _SOC_DEF_DMAMUX_H_ */
