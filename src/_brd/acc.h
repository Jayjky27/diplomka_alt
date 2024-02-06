/*
 *==============================================================================
 * Name: acc.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _ACC_H_
#define _ACC_H_

#include <stdint.h>

typedef struct __attribute__ ((packed)) {
	uint8_t ctrl;
	uint8_t rsvd;
	int8_t temp;
	uint16_t mag;
	int16_t x;
	int16_t y;
	int16_t z;
} ACC_SAMP_BLK;

typedef void (*ACC_CB)(ACC_SAMP_BLK *samp_blk);

uint8_t acc_init(void);
void acc_enable(ACC_CB cb, uint8_t cfg1_fsr, uint8_t cfg2_wakepm, uint8_t cfg3_wakeodr);
void acc_disable(void);

#endif /* _ACC_H_ */
