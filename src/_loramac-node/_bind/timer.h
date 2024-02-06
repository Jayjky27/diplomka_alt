/*!
 * \file      timer.h
 *
 * \brief     Timer objects and scheduling management implementation
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
#ifndef _TIMER_H_
#define _TIMER_H_

#include "mkl25z4.h"

/*!
 * \brief Timer time variable definition
 */
#define TIMERTIME_T_MAX			~0ul
typedef uint32_t TimerTime_t;

/*!
 * \brief Timer object description
 */
typedef struct TimerEvent_s {
	IRQn_Type irqn;
	TPM_Type *tpm;
	uint32_t tpm_chan;
	uint32_t period;
	uint32_t ovf_count;
	uint32_t ovf_target;
	void (*cb)(void *context);
} TimerEvent_t;

/*!
 * \brief Timer global init
 */
void TimerGlobalInit(void);

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void TimerInit(volatile TimerEvent_t *obj, void (*callback)(void *context));

/*!
 * \brief Deinitializes all timer objects
 */
void TimerDeInitAll(void);

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStart(volatile TimerEvent_t *obj);

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStop(const volatile TimerEvent_t *obj);

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
void TimerSetValue(volatile TimerEvent_t *obj, uint32_t value);

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
TimerTime_t TimerGetCurrentTime(void);

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \remark TimerGetElapsedTime will return 0 for argument 0.
 *
 * \param [IN] past         fix moment in Time
 * \retval time             returns elapsed time
 */
TimerTime_t TimerGetElapsedTime(TimerTime_t past);

/*!
 * \brief Computes the temperature compensation for a period of time on a
 *        specific temperature.
 *
 * \param [IN] period Time period to compensate
 * \param [IN] temperature Current temperature
 *
 * \retval Compensated time period
 */
TimerTime_t TimerTempCompensation(TimerTime_t period, float temperature);

#endif // _TIMER_H_
