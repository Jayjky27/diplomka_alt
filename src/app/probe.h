/*
 *==============================================================================
 * Name: probe.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _PROBE_H_
#define _PROBE_H_

#include <stdint.h>

void probe_hardfault_handler(void);

uint32_t probe8(uint32_t *val, const void *src);
uint32_t probe16(uint32_t *val, const void *src);
uint32_t probe32(uint32_t *val, const void *src);

#endif /* _PROBE_H_ */
