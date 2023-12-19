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


void  pressure_sensor(SCI_MSG_p msg ,  int16_t * braking_pressure , int16_t *left_steering_pressure ,int16_t *right_steering_pressure);


void uart_printf(char *fmt,...) ;
# if 0

// 单通道传感器暂时不用

extern uint8_t  pressure_command[8];

void  Send_pressure_command(uint8_t *command , uint8_t length);
unsigned short int crc16(uint8_t *addr,uint8_t data_len);
unsigned short int  pressure_receive_data(void);

#endif

#endif
