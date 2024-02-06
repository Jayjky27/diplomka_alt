/*
 *==============================================================================
 * Name: port_int.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _PORT_INT_H_
#define _PORT_INT_H_

#include "mkl25z4.h"

typedef void (*PORT_INT_CB)(void);

void port_int_init(void);
void port_int_en_pin(IRQn_Type irqn, PORT_INT_CB cb, uint8_t pin_ind);

#endif /* _PORT_INT_H_ */
