/*!
 * \file      systime.h
 *
 * \brief     System time functions implementation.
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
 *              (C)2013-2018 Semtech - STMicroelectronics
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    MCD Application Team ( STMicroelectronics International )
 *
 * \author    Martin Stankus ( VSB-TUO )
 */
#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <stdint.h>

/*!
 * \brief Number of seconds elapsed between Unix and GPS epoch
 */
#define UNIX_GPS_EPOCH_OFFSET		315964800

/*!
 * \brief Structure holding the system time in seconds and milliseconds
 */
typedef struct SysTime_s {
	uint32_t Seconds;
	int16_t SubSeconds;
} SysTime_t;

/*!
 * \brief Adds 2 SysTime_t values
 *
 * \param a Value
 * \param b Value to added
 *
 * \retval result Addition result (SysTime_t value)
 */
SysTime_t SysTimeAdd(SysTime_t a, SysTime_t b);

/*!
 * \brief Subtracts 2 SysTime_t values
 *
 * \param a Value
 * \param b Value to be subtracted
 *
 * \retval result Subtraction result (SysTime_t value)
 */
SysTime_t SysTimeSub(SysTime_t a, SysTime_t b);

/*!
 * \brief Initializes system time subsystem
 */
void SysTimeGlobalInit(void);

/*!
 * \brief Sets new system time
 *
 * \param  sysTime    New seconds/sub-seconds since UNIX epoch origin
 */
void SysTimeSet(SysTime_t sysTime);

/*!
 * \brief Gets current system time
 *
 * \retval sysTime    Current seconds/sub-seconds since UNIX epoch origin
 */
SysTime_t SysTimeGet(void);

/*!
 * \brief Gets current MCU time as a system time (SysTime_t)
 *
 * \retval sysTime    Current seconds/sub-seconds since MCU started
 */
SysTime_t SysTimeGetMcuTime(void);

/*!
 * \brief Gets current MCU time in msecs
 *
 * \retval timeMs     Current msecs since MCU started
 */
uint32_t SysTimeGetMcuTimeMs(void);

/*!
 * Converts the given SysTime to the equivalent RTC value in msecs
 *
 * \param [IN] sysTime System time to be converted
 *
 * \retval timeMs The RTC converted time value in msecs
 */
uint32_t SysTimeToMs(SysTime_t sysTime);

/*!
 * Converts the given RTC value in milliseconds to the equivalent SysTime
 *
 * \param [IN] timeMs The RTC time value in msecs to be converted
 *
 * \retval sysTime Converted system time
 */
SysTime_t SysTimeFromMs(uint32_t timeMs);

#endif // _SYS_TIME_H_
