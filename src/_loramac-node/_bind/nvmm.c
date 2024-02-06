/*
 *==============================================================================
 * Name: nvmm.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "nvmm.h"

#include "LoRaMac.h"
#include "utilities.h"

static uint8_t nvmm_data[sizeof(LoRaMacNvmData_t)];

uint16_t NvmmWrite(uint8_t* src, uint16_t size, uint16_t offset)
{
	CRITICAL_SECTION_BEGIN();
	memcpy(nvmm_data + offset, src, size);
	CRITICAL_SECTION_END();

	return size;
}

uint16_t NvmmRead(uint8_t* dest, uint16_t size, uint16_t offset)
{
	CRITICAL_SECTION_BEGIN();
	memcpy(dest, nvmm_data + offset, size);
	CRITICAL_SECTION_END();

	return size;
}

bool NvmmCrc32Check(uint16_t size, uint16_t offset)
{
	uint32_t cksum_data, cksum_comp;

	CRITICAL_SECTION_BEGIN();
	memcpy(&cksum_data, nvmm_data + offset + size - sizeof(cksum_data), sizeof(cksum_data));
	cksum_comp = Crc32(nvmm_data + offset, size - sizeof(cksum_comp));
	CRITICAL_SECTION_END();

	if (cksum_data != cksum_comp) {
		return false;
	}

	return true;
}

bool NvmmReset(uint16_t size, uint16_t offset)
{
	uint32_t cksum_dummy = 0ul;

	CRITICAL_SECTION_BEGIN();
	memcpy(nvmm_data + offset + size - sizeof(cksum_dummy), &cksum_dummy, sizeof(cksum_dummy));
	CRITICAL_SECTION_END();

	return true;
}
