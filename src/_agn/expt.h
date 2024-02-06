/*
 *==============================================================================
 * Name: expt.h
 * Author: Martin Stankus
 *==============================================================================
 */

#ifndef _EXPT_H_
#define _EXPT_H_

typedef void (*HDLR)(void);

void expt_vect_set(int expt_num, HDLR hdlr);
uint32_t expt_disable(void);
void expt_restore(uint32_t primask);

#endif /* _EXPT_H_ */
