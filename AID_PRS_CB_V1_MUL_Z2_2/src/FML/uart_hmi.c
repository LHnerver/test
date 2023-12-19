/*
 * uart_hmi.c
 *
 *  Created on: 2020年7月28日
 *      Author: Su-xi
 */

#include "uart_hmi.h"

uint8_t page_id = 0, last_page_id = 0;

void uart_hmi_init(void)
{
	/* Initialize LINFlex1: UART Mode 80MHz, 115200 Baud */
	Init_LINFlexD1(80, 115200);
}

bool analyze_hmi_msg(P_HMI_CACHE_MSG c_msg, volatile SHARED_DATA *s_data)
{
	uint8_t status = UNLOCK;
	HMI_MSG msg;

	if (c_msg->r_index == c_msg->w_index)
	{
		/* No valid data */
		return false;
	}
	else
	{
		msg.len = c_msg->msg[c_msg->r_index].len;
		c_msg->msg[c_msg->r_index].len = 0;
		(void)memcpy(msg.data, c_msg->msg[c_msg->r_index].data, msg.len);
		c_msg->r_index++;
		if (c_msg->r_index == 5)
		{
			c_msg->r_index = 0;
		}
	}

	if (msg.len < 5)
	{
		return false;
	}

	/* Lock the gate */
	while(status != CORE2_LOCK)
	{
		status = lock_gate(GATE_0);
	}
/*
	if (msg.data[0] == 0x66)					// send me(page id)
	{
		last_page_id = page_id;
		page_id = msg.data[1];

		if (last_page_id == P_HOME1 && page_id == P_CALI)
		{
			s_data->sys_info.enter_cali_flag = 1;
		}
		else if (last_page_id >= P_CALI && last_page_id <= P_CALI_DR
			&& (page_id == P_HOME1 || page_id == P_HOME2))
		{
			s_data->sys_info.exit_cali_flag = 1;
		}
		else if (last_page_id == P_HOME1 && page_id == P_DEBUG)
		{
			s_data->sys_info.enter_debug_flag = 1;
		}
		else if (last_page_id >= P_DEBUG && last_page_id <= P_DEBUG_DR
				&& (page_id == P_HOME1 || page_id == P_HOME2))
		{
			s_data->sys_info.exit_debug_flag = 1;
		}
	}
	else if (msg.data[0] == 0x70)			// get "X" (calibration)
	{
		switch (page_id)
		{
			case P_CALI_SR:
			{
				if ((msg.data[1] - '0') == 1)
				{
					s_data->sr_info.cali_flag = 1;
				}
				else
				{
					s_data->sr_info.cali_type = msg.data[1] - '0' - 2;
				}

				break;
			}

			case P_CALI_AR:
			{
				if ((msg.data[1] - '0') == 1)
				{
					s_data->ar_info.cali_flag = 1;
				}
				else
				{
					s_data->ar_info.cali_type = msg.data[1] - '0' - 2;
				}

				break;
			}

			case P_CALI_DR:
			{
				if ((msg.data[1] - '0') == 1)
				{
					s_data->dr_info.cali_flag = 1;
				}
				else
				{
					s_data->dr_info.cali_type = msg.data[1] - '0' - 2;
				}

				break;
			}
		}
	}
	else if (msg.data[0] == 0x71)			// get "X.val" (debug)
	{
		int32_t temp;
		int16_t angle;
		temp = msg.data[1] | (msg.data[2] << 8) | (msg.data[3] << 16)
			| (msg.data[4] << 24);
		angle = (int16_t)temp;

		switch (page_id)
		{
			case P_DEBUG_SR:
			{
				s_data->sr_info.debug_flag = 1;
				s_data->sr_info.debug_angle = angle;

				break;
			}

			case P_DEBUG_AR:
			{
				s_data->ar_info.debug_flag = 1;
				s_data->ar_info.debug_angle = angle;

				break;
			}

			case P_DEBUG_DR:
			{
				s_data->dr_info.debug_flag = 1;
				s_data->dr_info.debug_angle = angle;

				break;
			}
		}
	}
*/
	/* Unlock the gate */
	while(status == CORE2_LOCK)
	{
		status = unlock_gate(GATE_0);
	}

	return true;
}

void reply_hmi_msg(volatile SHARED_DATA *s_data, SCI_MSG* smp)
{
	uint8_t len = 0;
	char buffer[200];
	uint8_t status = UNLOCK;

	(void)memset(buffer, 0x00, 200);
	(void)memset(smp->tx_data, 0x00, 200);
	smp->tx_index = 0;
	smp->tx_length = 0;

	/* Lock the gate */
	while(status != CORE2_LOCK)
	{
		status = lock_gate(GATE_0);
	}
/*
	// sys0
	len += sprintf(buffer, "sys0=%d", s_data->sys_info.sys_st.b.auto_st);
	buffer[len++] = 0xFF;
	buffer[len++] = 0xFF;
	buffer[len++] = 0xFF;

	//
	switch (page_id)
	{
		case P_WELCOME:
		{
			len += sprintf(buffer + len, "va0_0.val=%d",
				s_data->sys_info.sys_st.b.self_check_st);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_HOME1:
			break;

		case P_HOME2:
			break;

		case P_STATE:
		{
			STATE st;

			if (s_data->dr_info.alarm_code > 0)
			{
				st.b.dr_alarm_st = 0;
			}
			else
			{
				st.b.dr_alarm_st = 1;
			}
			st.b.dr_ctrl_mode = s_data->dr_info.robot_st.b.ctrl_mode;
			st.b.dr_open_st = s_data->dr_info.robot_st.b.open_st;

			if (s_data->ar_info.alarm_code > 0)
			{
				st.b.ar_alarm_st = 0;
			}
			else
			{
				st.b.ar_alarm_st = 1;
			}
			st.b.ar_ctrl_mode = s_data->ar_info.robot_st.b.ctrl_mode;
			st.b.ar_open_st = s_data->ar_info.robot_st.b.open_st;

			if (s_data->sr_info.alarm_code > 0)
			{
				st.b.sr_alarm_st = 0;
			}
			else
			{
				st.b.sr_alarm_st = 1;
			}
			st.b.sr_ctrl_mode = s_data->sr_info.robot_st.b.ctrl_mode;
			st.b.sr_open_st = s_data->sr_info.robot_st.b.open_st;

			st.b.sys_run_st = s_data->sys_info.sys_st.b.run_st;
			st.b.sys_auto_st =  s_data->sys_info.sys_st.b.auto_st;
			st.b.self_check = s_data->sys_info.sys_st.b.self_check;

			len += sprintf(buffer + len, "va0_3.val=%d", st.r);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_STATE_SYS:
		{
			// va0
			len += sprintf(buffer + len, "va0_4.val=%d",
				(uint16_t)((s_data->sys_info.sys_st.r >> 16) & 0xFFFF));

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// va1
			len += sprintf(buffer + len, "va1_4.val=%d",
				(uint16_t)(s_data->sys_info.sys_st.r & 0xFFFF));

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 功率
			len += sprintf(buffer + len, "t30_4.txt=\"%.1f W\"",
				s_data->sys_info.power);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电压
			len += sprintf(buffer + len, "t31_4.txt=\"%.1f V\"",
				s_data->sys_info.voltage);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电流
			len += sprintf(buffer + len, "t32_4.txt=\"%.1f A\"",
				s_data->sys_info.current);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_STATE_SR:
		{
			// va0
			len += sprintf(buffer + len, "va0_5.val=%d",
				s_data->sr_info.robot_st.r);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 功率
			len += sprintf(buffer + len, "t15_5.txt=\"%.1f W\"",
				s_data->sr_info.power);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电压
			len += sprintf(buffer + len, "t16_5.txt=\"%.1f V\"",
				s_data->sr_info.voltage);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电流
			len += sprintf(buffer + len, "t17_5.txt=\"%.1f A\"",
				s_data->sr_info.current);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 温度
			len += sprintf(buffer + len, "t18_5.txt=\"%.1f ",
				s_data->sr_info.temperature);

			buffer[len++] = 0xA1;
			buffer[len++] = 0xE6;
			buffer[len++] = '\"';
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 转角
			len += sprintf(buffer + len, "t21_5.txt=\"%d\"",
				s_data->sr_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 极左值
			len += sprintf(buffer + len, "t22_5.txt=\"%d\"",
				s_data->sr_info.top_pos);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 极右值
			len += sprintf(buffer + len, "t23_5.txt=\"%d\"",
				s_data->sr_info.bottom_pos);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 报警
			len += sprintf(buffer + len, "t24_5.txt=\"%d\"",
				s_data->sr_info.alarm_code);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_STATE_AR:
		{
			// va0
			len += sprintf(buffer + len, "va0_6.val=%d",
				s_data->ar_info.robot_st.r);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 功率
			len += sprintf(buffer + len, "t15_6.txt=\"%.1f W\"",
				s_data->ar_info.power);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电压
			len += sprintf(buffer + len, "t16_6.txt=\"%.1f V\"",
				s_data->ar_info.voltage);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电流
			len += sprintf(buffer + len, "t17_6.txt=\"%.1f A\"",
				s_data->ar_info.current);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 温度
			len += sprintf(buffer + len, "t18_6.txt=\"%.1f ",
				s_data->ar_info.temperature);

			buffer[len++] = 0xA1;
			buffer[len++] = 0xE6;
			buffer[len++] = '\"';
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 转角
			len += sprintf(buffer + len, "t21_6.txt=\"%d\"",
				s_data->ar_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 极左值
			len += sprintf(buffer + len, "t22_6.txt=\"%d\"",
				s_data->ar_info.top_pos);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 极右值
			len += sprintf(buffer + len, "t23_6.txt=\"%d\"",
				s_data->ar_info.bottom_pos);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 报警
			len += sprintf(buffer + len, "t24_6.txt=\"%d\"",
				s_data->ar_info.alarm_code);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_STATE_DR:
		{
			// va0
			len += sprintf(buffer + len, "va0_7.val=%d",
				s_data->dr_info.robot_st.r);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 功率
			len += sprintf(buffer + len, "t15_7.txt=\"%.1f W\"",
				s_data->dr_info.power);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电压
			len += sprintf(buffer + len, "t16_7.txt=\"%.1f V\"",
				s_data->dr_info.voltage);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 电流
			len += sprintf(buffer + len, "t17_7.txt=\"%.1f A\"",
				s_data->dr_info.current);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 温度
			len += sprintf(buffer + len, "t18_7.txt=\"%.1f ",
				s_data->dr_info.temperature);

			buffer[len++] = 0xA1;
			buffer[len++] = 0xE6;
			buffer[len++] = '\"';
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 转角
			len += sprintf(buffer + len, "t21_7.txt=\"%d\"",
				s_data->dr_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 极左值
			len += sprintf(buffer + len, "t22_7.txt=\"%d\"",
				s_data->dr_info.top_pos);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 极右值
			len += sprintf(buffer + len, "t23_7.txt=\"%d\"",
				s_data->dr_info.bottom_pos);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// 报警
			len += sprintf(buffer + len, "t24_7.txt=\"%d\"",
				s_data->dr_info.alarm_code);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_CALI:
			break;

		case P_CALI_SR:
		{
			// n0
			len += sprintf(buffer + len, "n0_9.val=%d",
				s_data->sr_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// n1
			switch (s_data->sr_info.cali_type)
			{
				case 0:
				{
					len += sprintf(buffer + len, "n1_9.val=%d",
						s_data->sr_info.top_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}

				case 1:
				{
					len += sprintf(buffer + len, "n1_9.val=%d",
						s_data->sr_info.zero_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}

				case 2:
				{
					len += sprintf(buffer + len, "n1_9.val=%d",
						s_data->sr_info.bottom_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}
			}

			break;
		}

		case P_CALI_AR:
		{
			// n0
			len += sprintf(buffer + len, "n0_10.val=%d",
				s_data->ar_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// n1
			switch (s_data->ar_info.cali_type)
			{
				case 0:
				{
					len += sprintf(buffer + len, "n1_10.val=%d",
						s_data->ar_info.top_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}

				case 1:
				{
					len += sprintf(buffer + len, "n1_10.val=%d",
						s_data->ar_info.zero_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}

				case 2:
				{
					len += sprintf(buffer + len, "n1_10.val=%d",
						s_data->ar_info.bottom_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}
			}

			break;
		}

		case P_CALI_DR:
		{
			// n0
			len += sprintf(buffer + len, "n0_11.val=%d",
				s_data->dr_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			// n1
			switch (s_data->dr_info.cali_type)
			{
				case 0:
				{
					len += sprintf(buffer + len, "n1_11.val=%d",
						s_data->dr_info.top_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}

				case 1:
				{
					len += sprintf(buffer + len, "n1_11.val=%d",
						s_data->dr_info.zero_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}

				case 2:
				{
					len += sprintf(buffer + len, "n1_11.val=%d",
						s_data->dr_info.bottom_pos);

					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;
					buffer[len++] = 0xFF;

					break;
				}
			}

			break;
		}

		case P_DEBUG:
			break;

		case P_DEBUG_SR:
		{
			// n1
			len += sprintf(buffer + len, "n1_13.val=%d",
				s_data->sr_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_DEBUG_AR:
		{
			// n1
			len += sprintf(buffer + len, "n1_14.val=%d",
				s_data->ar_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_DEBUG_DR:
		{
			// n1
			len += sprintf(buffer + len, "n1_15.val=%d",
				s_data->dr_info.angle);

			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;
			buffer[len++] = 0xFF;

			break;
		}

		case P_ALARM:
			break;

		case P_SET:
			break;

		case P_ABOUT:
		{
//			// t11
//			len += sprintf(buffer + len, "t11_18.txt=\"%s\"",
//				s_data->device_info.name);
//
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//
//			// t12
//			len += sprintf(buffer + len, "t12_18.txt=\"%s\"",
//				s_data->device_info.model);
//
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//
//			// t13
//			len += sprintf(buffer + len, "t13_18.txt=\"%s\"",
//				s_data->device_info.version);
//
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//
//			// t14
//			len += sprintf(buffer + len, "t14_18.txt=\"%s\"",
//				s_data->device_info.serial_num);
//
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;
//			buffer[len++] = 0xFF;

			break;
		}
	}
*/
	/* Unlock the gate */
	while(status == CORE2_LOCK)
	{
		status = unlock_gate(GATE_0);
	}

	(void)memcpy(smp->rx_data, buffer, len);
	smp->rx_length = len;
	uart1_send(smp);
}
