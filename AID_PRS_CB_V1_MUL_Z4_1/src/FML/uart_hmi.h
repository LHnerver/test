/*
 * uart_hmi.h
 *
 *  Created on: 2020年7月28日
 *      Author: Su-xi
 */

#ifndef FML_UART_HMI_H_
#define FML_UART_HMI_H_

#include "linflexd_uart.h"
#include "sh_mem.h"

#define P_WELCOME 	0x00
#define P_HOME1		0x01
#define P_HOME2		0x02
#define P_STATE		0x03
#define P_STATE_SYS	0x04
#define P_STATE_SR	0x05
#define P_STATE_AR	0x06
#define P_STATE_DR	0x07
#define P_CALI		0x08
#define P_CALI_SR	0x09
#define P_CALI_AR	0x0A
#define P_CALI_DR	0x0B
#define P_DEBUG		0x0C
#define P_DEBUG_SR	0x0D
#define P_DEBUG_AR	0x0E
#define P_DEBUG_DR	0x0F
#define P_ALARM		0x10
#define P_SET		0x11
#define P_ABOUT		0x12

typedef union
{
	uint16_t r;
	struct
	{
		uint16_t dr_alarm_st	:1;		/* 减速电机报警状态：0-报警  1-正常 */
		uint16_t dr_ctrl_mode	:2;		/* 控制模式：0-电流  1-速度  2-位置  3-回零 */
		uint16_t dr_open_st		:1;		/* 减速电机开启状态：0-关闭  1-开启 */
		uint16_t ar_alarm_st	:1;		/* 加速电机报警状态：0-报警  1-正常 */
		uint16_t ar_ctrl_mode	:2;		/* 控制模式：0-电流  1-速度  2-位置  3-回零 */
		uint16_t ar_open_st		:1;		/* 加速电机开启状态：0-关闭  1-开启 */

		uint16_t sr_alarm_st	:1;		/* 转向电机报警状态：0-报警  1-正常 */
		uint16_t sr_ctrl_mode	:2;		/* 控制模式：0-电流  1-速度  2-位置  3-回零 */
		uint16_t sr_open_st		:1;		/* 转向电机开启状态：0-关闭  1-开启 */
		uint16_t sys_run_st		:2;		/* 运行状态：0-无效  1-启动 2-停止 3-急停  */
		uint16_t sys_auto_st	:1;		/* 自动驾驶状态：0-手动接管 1-自动驾驶 */
		uint16_t self_check		:1;		/* 自检结果：0-失败  1-成功*/
	}b;
}STATE;

typedef struct _HMI_MSG
{
	uint8_t len;
	uint8_t data[10];
}HMI_MSG;

typedef struct _HMI_CACHE_MSG
{
	uint8_t r_index;
	uint8_t w_index;
	HMI_MSG msg[5];
}HMI_CACHE_MSG, *P_HMI_CACHE_MSG;

void uart_hmi_init(void);

bool analyze_hmi_msg(P_HMI_CACHE_MSG c_msg, volatile SHARED_DATA *s_data);
void reply_hmi_msg(volatile SHARED_DATA *s_data, SCI_MSG* smp);

#endif /* FML_UART_HMI_H_ */
