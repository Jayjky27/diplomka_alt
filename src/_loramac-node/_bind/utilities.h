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
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stdint.h>
#include <string.h>

#include "crsec.h"
#include "mem.h"

/*!
 * \brief Returns the minimum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval minValue Minimum value
 */
#ifndef MIN
#define MIN(a, b)	(((a ) < (b)) ? (a) : (b))
#endif

/*!
 * \brief Returns the maximum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
#ifndef MAX
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))
#endif

typedef union Version_u {
	struct Version_s {
		uint8_t Revision;
		uint8_t Patch;
		uint8_t Minor;
		uint8_t Major;
	} Fields;
	uint32_t Value;
} Version_t;

/*!
 * \brief Initializes the pseudo random generator initial value
 *
 * \param [IN] seed Pseudo random generator initial value
 */
void srand1(uint32_t seed);

/*!
 * \brief Computes a random number between min and max
 *
 * \param [IN] min range minimum value
 * \param [IN] max range maximum value
 * \retval random random value in range min..max
 */
int32_t randr(int32_t min, int32_t max);

/*!
 * \brief Ask Semtech (and see LoRaMacCrypto.c line 969)
 */
void SecureElementRandomNumber(uint32_t *rand_num);

#define memset1 memset
#define memcpy1 memcpy

/*!
 * \brief Copies size elements of src array to dst array reversing the byte order
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size);

/*!
 * \brief Computes a CCITT 32 bits CRC
 *
 * \param [IN] buffer   Data buffer used to compute the CRC
 * \param [IN] length   Data buffer length
 *
 * \retval crc          The computed buffer of length CRC
 */
uint32_t Crc32(uint8_t *buffer, uint16_t length);

/*!
 * Blocking delay (msecs)
 */
void DelayMs(uint32_t ms);

/*!
 * Begins critical section
 */
#define CRITICAL_SECTION_BEGIN()	crsec_enter()

/*!
 * Ends critical section
 */
#define CRITICAL_SECTION_END()		crsec_leave()

#endif // _UTILITIES_H_
