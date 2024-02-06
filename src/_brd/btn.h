/*
 *==============================================================================
 * Name: btn.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _BTN_H_
#define _BTN_H_

#include <stdint.h>

#define BTN_ID_SW1		0u
#define BTN_ID_SW2		1u

uint8_t btn_get(uint8_t btn_id);

#endif /* _BTN_H_ */
