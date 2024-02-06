/*
 *==============================================================================
 * Name: gpio.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include "mkl25z4.h"

#include "brd_def.h"
#include "bme.h"

#define GPIO_MODE_IN_EVAL(ID)		BME_LAC1_W(&GPIO_##ID->PDDR, IOIND_##ID)
#define GPIO_MODE_IN(ID)			GPIO_MODE_IN_EVAL(ID)

#define GPIO_MODE_OUT_EVAL(ID)		BME_LAS1_W(&GPIO_##ID->PDDR, IOIND_##ID)
#define GPIO_MODE_OUT(ID)			GPIO_MODE_OUT_EVAL(ID)

#define GPIO_OUT_HI_EVAL(ID)		BME_LAS1_W(&GPIO_##ID->PSOR, IOIND_##ID)
#define GPIO_OUT_HI(ID)				GPIO_OUT_HI_EVAL(ID)

#define GPIO_OUT_LO_EVAL(ID)		BME_LAS1_W(&GPIO_##ID->PCOR, IOIND_##ID)
#define GPIO_OUT_LO(ID)				GPIO_OUT_LO_EVAL(ID)

#define GPIO_OUT_TGL_EVAL(ID)		BME_LAS1_W(&GPIO_##ID->PTOR, IOIND_##ID)
#define GPIO_OUT_TGL(ID)			GPIO_OUT_TGL_EVAL(ID)

#define GPIO_IN_EVAL(ID)			BME_UBFX_W(&GPIO_##ID->PDIR, IOIND_##ID, 1u)
#define GPIO_IN(ID)					GPIO_IN_EVAL(ID)

#endif /* _GPIO_H_ */
