/*
 *==============================================================================
 * Name: init.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _INIT_H_
#define _INIT_H_

#include "soc_def.h"

#define SIM_SOPT1_OSC32KSEL_VAL				SIM_SOPT1_OSC32KSEL_VAL_LPOCLK

#define SIM_SOPT2_UART0SRC_VAL				SIM_SOPT2_UART0SRC_VAL_MCGIRCLK
#define SIM_SOPT2_TPMSRC_VAL				SIM_SOPT2_TPMSRC_VAL_MCGIRCLK
#define SIM_SOPT2_CLKOUTSEL_VAL				SIM_SOPT2_CLKOUTSEL_VAL_LPOCLK

#if VLP

#define SIM_CLKDIV1_OUTDIV1_VAL				SIM_CLKDIV1_OUTDIV1_VAL_D1
#define SIM_CLKDIV1_OUTDIV4_VAL				SIM_CLKDIV1_OUTDIV4_VAL_D5

#else

#define SIM_CLKDIV1_OUTDIV1_VAL				SIM_CLKDIV1_OUTDIV1_VAL_D1
#define SIM_CLKDIV1_OUTDIV4_VAL				SIM_CLKDIV1_OUTDIV4_VAL_D1

#endif

void init(void);

#endif /* _INIT_H_ */
