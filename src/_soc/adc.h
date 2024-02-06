/*
 *==============================================================================
 * Name: adc.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

typedef enum {
	ADC_MODE_8BIT_SE,
	ADC_MODE_16BIT_SE,
	ADC_MODE_8BIT_DIFF,
	ADC_MODE_16BIT_DIFF
} ADC_MODE;

uint16_t adc_read(ADC_MODE adc_mode, uint8_t adc_chan);

#endif /* _ADC_H_ */
