/*!
 * \file      utilities.h
 *
 * \brief     Helper functions implementation
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

#include <stdint.h>
#include <string.h>

#include "utilities.h"

#include "brd_def.h"
#include "delay.h"
#include "adc.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX	2147483647l

static uint32_t next = 1ul;

static int32_t rand1(void)
{
	return ((next = next * 1103515245l + 12345l) % RAND_LOCAL_MAX);
}

void srand1(uint32_t seed)
{
	next = adc_read(ADC_MODE_16BIT_DIFF, ADC_CHAN_TEMP);
	next <<= 16u;
	next |= adc_read(ADC_MODE_16BIT_SE, ADC_CHAN_TEMP);

	next ^= seed;
}
// Standard random functions redefinition end

int32_t randr(int32_t min, int32_t max)
{
	return (int32_t) rand1() % (max - min + 1) + min;
}

void SecureElementRandomNumber(uint32_t *rand_num)
{
	*rand_num = rand1();
}

void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size)
{
	dst = dst + (size - 1);
	while (size--) {
		*dst-- = *src++;
	}
}

uint32_t Crc32(uint8_t *buffer, uint16_t length)
{
	// The CRC calculation follows CCITT - 0x04C11DB7
	const uint32_t reversedPolynom = 0xEDB88320ul;

	// CRC initial value
	uint32_t crc = 0xFFFFFFFFul;

	if (buffer == NULL) {
    	return 0;
	}

	for (uint16_t i = 0; i < length; ++i) {
    	crc ^= (uint32_t) buffer[i];
        for (uint16_t i = 0; i < 8; i++) {
        	crc = (crc >> 1) ^ (reversedPolynom & ~((crc & 0x01) - 1));
        }
	}

	return ~crc;
}


void DelayMs(uint32_t ms)
{
	delay(DELAY_MS_TO_CNT(ms));
}
