/*
 * encoder.h
 *
 *  Created on: 2020Äê11ÔÂ11ÈÕ
 *      Author: Su-xi
 */

#ifndef FML_ENCODER_H_
#define FML_ENCODER_H_

#include "adc.h"

void encoder_init(void);
uint16_t get_encoder_value(void);
uint16_t get_pressure_voltage_value(void);

#endif /* FML_ENCODER_H_ */
