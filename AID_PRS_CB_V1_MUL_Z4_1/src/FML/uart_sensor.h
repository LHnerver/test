/*
 * uart_sensor.c
 *
 *  Created on: 2022年9月23日
 *      Author: SuJunwei
 * Used for:
 * 	用于与RS232、RS485接口通信，采集传感器信息
 */

#ifndef FML_UART_SENSOR_H_
#define FML_UART_SENSOR_H_


#include "typedefs.h"
#include "linflexd_uart.h"


void uart_sensor_init(void);
int16_t get_pedal_force(SENSOR_ID id);










#endif
