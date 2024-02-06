/*
 *==============================================================================
 * Name: eeprom.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

void eeprom_init(void);
uint8_t eeprom_rd(uint16_t eeprom_addr, void *dst, uint16_t len, uint8_t pwr_off_delay);
uint8_t eeprom_wr(uint16_t eeprom_addr, const void *src, uint16_t len, uint8_t pwr_off_delay);

#endif /* _EEPROM_H_ */
