/*!
 * \file      board.c
 *
 * \brief     Target board general functions implementation
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

#include "mkl25z4.h"
#include "board.h"

void BoardGetUniqueId(uint8_t *id)
{
	id[0] = SIM->UIDL;
	id[1] = SIM->UIDL >> 8u;
	id[2] = SIM->UIDL >> 16u;
	id[3] = SIM->UIDL >> 24u;

	id[4] = SIM->UIDML ^ SIM->UIDMH;
	id[5] = (SIM->UIDML ^ SIM->UIDMH) >> 8u;
	id[6] = SIM->UIDML >> 16u;
	id[7] = SIM->UIDML >> 24u;
}
