/*
 * uart_sensor.c
 *
 *  Created on: 2022��9��23��
 *      Author: SuJunwei
 * Used for:
 * 	������RS232��RS485�ӿ�ͨ�ţ��ɼ���������Ϣ
 */

#ifndef FML_UART_SENSOR_H_
#define FML_UART_SENSOR_H_


#include "typedefs.h"
#include "linflexd_uart.h"


void uart_sensor_init(void);
int16_t get_pedal_force(SENSOR_ID id);









#endif