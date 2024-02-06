/*
 *==============================================================================
 * Name: com.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _COM_H_
#define _COM_H_

void com_init(void);
uint8_t com_try_lock(void);
void com_send(const void *data, uint16_t len);

#endif /* _COM_H_ */
