/*
 * sh_mem.h
 *
 *  Created on: 2020Äê7ÔÂ21ÈÕ
 *      Author: Su-xi
 */

#ifndef SH_MEM_H_
#define SH_MEM_H_

#include "derivative.h"
#include "typedefs.h"

#define stringify(s) tostring(s)
#define tostring(s) #s
#define mfspr(rn) \
	({unsigned int rval; __asm__ volatile("mfspr %0," stringify(rn) \
	: "=r" (rval)); rval;})
#define mtspr(rn, v)   \
	__asm__ volatile("mtspr " stringify(rn) ",%0" : : "r" (v))

#define GET_CORE_ID() ((uint16_t) mfspr(286))

#define UNLOCK 0
#define CORE0_LOCK 1
#define CORE1_LOCK 2
#define CORE2_LOCK 3
#define GATE_0  0

uint8_t get_gate_status(uint8_t gate_no);
uint8_t lock_gate(uint8_t gate_no);
uint8_t unlock_gate(uint8_t gate_no);
uint8_t reset_gate(uint8_t gate_no);

#endif /* SH_MEM_H_ */
