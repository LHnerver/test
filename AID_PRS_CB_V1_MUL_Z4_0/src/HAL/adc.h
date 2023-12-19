/*
 * adc.h
 *
 *  Created on: 2019Äê1ÔÂ15ÈÕ
 *      Author: Su-xi
 */

#ifndef ADC_H_
#define ADC_H_

#include "project.h"

uint16_t ADC1_Read_CH0(void);
uint16_t ADC1_Read_CH1(void);
uint16_t ADC1_Read_CH2(void);
uint16_t ADC1_Read_CH3(void);

void Init_ADC(void);

void  get_pressure(uint16_t  *pressure);
uint16_t get_pressure_voltage_value(void);

#endif /* ADC_H_ */
