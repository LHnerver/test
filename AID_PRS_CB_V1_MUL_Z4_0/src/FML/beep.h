/*
 * beep.h
 *
 *  Created on: 2020Äê12ÔÂ8ÈÕ
 *      Author: Su-xi
 */

#ifndef FML_BEEP_H_
#define FML_BEEP_H_

#include "pin.h"

typedef enum
{
	BEEP_OFF,
	BEEP_ON,
	BEEP_UNKNOWN
}BEEP_STATE;

void beep_init(void);
void set_beep(uint8_t cnt, uint16_t cycle1_ms, uint32_t cycle2_ms);
BEEP_STATE beep_switch(BEEP_STATE state);
BEEP_STATE get_beep_ctrl_state(void);
void beep_ctrl(BEEP_STATE state);

#endif /* FML_BEEP_H_ */
