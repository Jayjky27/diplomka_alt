/*
 *==============================================================================
 * Name: test_eeprom.c
 * Author: Martin Stankus
 *==============================================================================
 */

#include "mkl25z4.h"

#include "test_eeprom.h"

#include "mem.h"
#include "eeprom.h"
#include "assert.h"

#define TEST0_EEPROM_ADDR	1u
#define TEST1_EEPROM_ADDR	32u
#define TEST2_EEPROM_ADDR	496u

#define TEST0_LEN			1u
#define TEST1_LEN			64u
#define TEST2_LEN			1056u

#define TEST_LEN_MAX		TEST2_LEN

#define TEST_PATTERN_A		0xA5u
#define TEST_PATTERN_B		0x5Au

static uint8_t src[TEST_LEN_MAX];
static uint8_t dst[TEST_LEN_MAX];

static void test_dispatch(uint16_t eeprom_addr, uint16_t len, uint8_t pattern)
{
	assert(!eeprom_wr(eeprom_addr, src, len, 1u));
	memset(dst, ~pattern, len);
	assert(!eeprom_rd(eeprom_addr, dst, len, 1u));
	assert(!memcmp(src, dst, len));
}

static void test_patt(uint8_t pattern)
{
	memset(src, pattern, TEST_LEN_MAX);
	test_dispatch(TEST0_EEPROM_ADDR, TEST0_LEN, pattern);
	test_dispatch(TEST1_EEPROM_ADDR, TEST1_LEN, pattern);
	test_dispatch(TEST2_EEPROM_ADDR, TEST2_LEN, pattern);
}

static void test_incr(void)
{
	uint16_t ind;

	for (ind = 0u; ind < TEST_LEN_MAX; ind++) {
		src[ind] = ind;
	}

	test_dispatch(TEST0_EEPROM_ADDR, TEST0_LEN, 0xFFu);
	test_dispatch(TEST1_EEPROM_ADDR, TEST1_LEN, 0xFFu);
	test_dispatch(TEST2_EEPROM_ADDR, TEST2_LEN, 0xFFu);
}

void test_eeprom(void)
{
	eeprom_init();

	test_patt(TEST_PATTERN_A);
	test_patt(TEST_PATTERN_B);
	test_incr();

	while (1) {
		__BKPT(0);
	}
}
