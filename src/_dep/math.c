/*
 *==============================================================================
 * Name: math.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include <math.h>
#include "qfplib-m0-full.h"

double floor(double arg)
{
	return qfp_int2double(qfp_double2int(arg));
}
