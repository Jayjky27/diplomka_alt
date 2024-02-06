/*
 *==============================================================================
 * Name: nvmm.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _NVMM_H_
#define _NVMM_H_

#include <stdint.h>
#include <stdbool.h>

uint16_t NvmmWrite(uint8_t* src, uint16_t size, uint16_t offset);
uint16_t NvmmRead(uint8_t* dest, uint16_t size, uint16_t offset);
bool NvmmCrc32Check(uint16_t size, uint16_t offset);
bool NvmmReset(uint16_t size, uint16_t offset);

#endif // _NVMM_H_
