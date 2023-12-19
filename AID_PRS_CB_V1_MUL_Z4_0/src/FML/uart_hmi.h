/*
 * uart_hmi.h
 *
 *  Created on: 2020��7��28��
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
		uint16_t dr_alarm_st	:1;		/* ���ٵ������״̬��0-����  1-���� */
		uint16_t dr_ctrl_mode	:2;		/* ����ģʽ��0-����  1-�ٶ�  2-λ��  3-���� */
		uint16_t dr_open_st		:1;		/* ���ٵ������״̬��0-�ر�  1-���� */
		uint16_t ar_alarm_st	:1;		/* ���ٵ������״̬��0-����  1-���� */
		uint16_t ar_ctrl_mode	:2;		/* ����ģʽ��0-����  1-�ٶ�  2-λ��  3-���� */
		uint16_t ar_open_st		:1;		/* ���ٵ������״̬��0-�ر�  1-���� */

		uint16_t sr_alarm_st	:1;		/* ת��������״̬��0-����  1-���� */
		uint16_t sr_ctrl_mode	:2;		/* ����ģʽ��0-����  1-�ٶ�  2-λ��  3-���� */
		uint16_t sr_open_st		:1;		/* ת��������״̬��0-�ر�  1-���� */
		uint16_t sys_run_st		:2;		/* ����״̬��0-��Ч  1-���� 2-ֹͣ 3-��ͣ  */
		uint16_t sys_auto_st	:1;		/* �Զ���ʻ״̬��0-�ֶ��ӹ� 1-�Զ���ʻ */
		uint16_t self_check		:1;		/* �Լ�����0-ʧ��  1-�ɹ�*/
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