/*
 *==============================================================================
 * Name: led.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _LED_H_
#define _LED_H_

#include "gpio.h"

#define LED_ON(ID)			GPIO_OUT_LO(ID)
#define LED_OFF(ID)			GPIO_OUT_HI(ID)
#define LED_TGL(ID)			GPIO_OUT_TGL(ID)

void led_init(void);

#endif /* _LED_H_ */
