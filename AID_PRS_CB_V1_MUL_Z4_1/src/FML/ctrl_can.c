/*
 * ctrl_can.c
 *
 *  Created on: 2020年7月23日
 *      Author: Su-xi
 */

#include "ctrl_can.h"

void ctrl_can_init()
{
	can0_init();
	can1_init();
}

uint8_t check_sum(CAN_MSG* cmp)
{
	uint8_t check_sum, i;
	check_sum = 0;
	for (i = 1; i < cmp->length; i++)
		check_sum ^= cmp->data[i];
	return check_sum;
}

void general_replay_msg(CAN_MSG* cmp, uint16_t std_id, uint16_t reply_std_id,
		uint8_t actuator_id,uint8_t state,uint8_t reason)
{
	static uint8_t ct_1;
	//帧ID
	cmp->std_id = std_id;
	cmp->data[1] = reply_std_id&0xff;
	cmp->data[2] = reply_std_id>>8;
	cmp->data[3] = ((state&0x03)<<4)|(actuator_id&0x0f);
	cmp->data[4] = reason&0xff;
	cmp->data[5] = ((ct_1++)&0x0f)<<4;
	if(ct_1 > 15) ct_1 = 0x00;

	//数据长度
	cmp->length = 6;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

void calibration_data_query_replay_msg(CAN_MSG* cmp, uint16_t std_id,
		CALI_DATA cali_data,DEVICE_INFO device_info)
{
	static uint8_t ct_2;
	switch(std_id)
	{
		case PRS_0_1:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = ((uint16_t)ABS(cali_data.sr_left_angle))&0xff;
			cmp->data[2] = ((((uint16_t)ABS(cali_data.br_max_angle))&0x03)<<6)|
					((((uint16_t)ABS(cali_data.sr_left_angle))>>8)&0x3f);
			cmp->data[3] = (((uint16_t)ABS(cali_data.br_max_angle))>>2)&0xff;
			cmp->data[4] = ((uint16_t)ABS(cali_data.sr_right_angle))&0xff;
			cmp->data[5] = ((((uint16_t)ABS(cali_data.ar_max_angle))&0x03)<<6)|
					((((uint16_t)ABS(cali_data.sr_right_angle))>>8)&0x3f);
			cmp->data[6] = (((uint16_t)ABS(cali_data.ar_max_angle))>>2)&0xff;
			cmp->data[7] = ((ct_2++)&0x0f)<<4;
			if(ct_2 > 15) ct_2 = 0x00;
			//数据长度
			cmp->length = 8;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
		case PRS_0_2:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = ((uint16_t)ABS(cali_data.ep_max_angle))&0xff;
			cmp->data[2] = ((((uint16_t)ABS(cali_data.ep_encoder_up))&0x3f)<<2)|
					((((uint16_t)ABS(cali_data.ep_max_angle))>>8)&0x03);
			cmp->data[3] =(((uint16_t)ABS(cali_data.ep_encoder_up))>>6)&0x3f;
			cmp->data[4] = (((uint16_t)ABS(cali_data.ep_encoder_down)))&0xff;
			cmp->data[5] = (((ct_2++)&0x0f)<<4)|((((uint16_t)ABS(cali_data.ep_encoder_down))>>8)&0x0f);
			if(ct_2 > 15) ct_2 = 0x00;
			//数据长度
			cmp->length = 6;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
		case PRS_0_3:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = ((uint16_t)ABS(cali_data.sr_acc_dec))&0xff;
			cmp->data[2] = ((((uint16_t)ABS(cali_data.br_speed))&0x0f)<<4)|
							((((uint16_t)ABS(cali_data.sr_acc_dec))>>8)&0x0f);
			cmp->data[3] = ((((uint16_t)ABS(cali_data.br_acc_dec))&0x07)<<5)|
							((((uint16_t)ABS(cali_data.br_speed))>>4)&0x1f);
			cmp->data[4] = ((((uint16_t)ABS(cali_data.ar_speed))&0x03)<<6)|
							((((uint16_t)ABS(cali_data.br_acc_dec))>>3)&0x3f);
			cmp->data[5] = ((((uint16_t)ABS(cali_data.ar_acc_dec))&0x01)<<7)|
							((((uint16_t)ABS(cali_data.ar_speed))>>2)&0x7f);
			cmp->data[6] = (((uint16_t)ABS(cali_data.ar_acc_dec))>>1)&0xff;
			cmp->data[7] = ((ct_2++)&0x0f)<<4;
			if(ct_2 > 15) ct_2 = 0x00;
			//数据长度
			cmp->length = 8;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
	}
}

void actuator_parameter_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		uint8_t actuator_id,uint16_t type, int32_t value)
{
	static uint8_t ct_3;
	//帧ID
	cmp->std_id = std_id;
	cmp->data[1] = (actuator_id & 0x07);
	cmp->data[2] = type & 0xff;
	cmp->data[3] = value & 0xff;
	cmp->data[4] = value >> 8;
	cmp->data[5] = value >> 16;
	cmp->data[6] = value >> 24;
	cmp->data[7] = ((ct_3++) & 0x0f)<<4;
	if(ct_3 > 15) ct_3 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

void device_info_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		DEVICE_INFO dev_info)
{
	static uint8_t ct_4;
	if(std_id == PRS_0_6)
	{
		//帧ID
		cmp->std_id = std_id;
		cmp->data[1] = dev_info.serial_num&0xff;
		cmp->data[2] = dev_info.serial_num>>8;
		cmp->data[3] = dev_info.serial_num>>16;
		cmp->data[4] = dev_info.serial_num>>24;
		cmp->data[5] = dev_info.hardware_ver;
		cmp->data[6] = dev_info.device_id&0xff;
		cmp->data[7] = (((ct_4++) & 0x0f)<<4)|((dev_info.device_id>>8)&0x0f);
		if(ct_4 > 15) ct_4 = 0x00;
		//数据长度
		cmp->length = 8;
		cmp->rtr = false;
		//优先等级
		cmp->prty = 8;
		cmp->data[0] = check_sum(cmp);
	}
	else if(std_id == PRS_0_7)
	{
		//帧ID
		cmp->std_id = std_id;
		cmp->data[1] = dev_info.firmware_ver&0xff;
		cmp->data[2] = dev_info.firmware_ver>>8;
		cmp->data[3] = ((ct_4++) & 0x0f)<<4;
		if(ct_4 > 15) ct_4 = 0x00;
		//数据长度
		cmp->length = 4;
		cmp->rtr = false;
		//优先等级
		cmp->prty = 8;
		cmp->data[0] = check_sum(cmp);
	}
}

void system_status_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data)
{
	static uint8_t ct_5;
	//帧ID
	cmp->std_id = std_id;
	cmp->data[1] = ((s_data->ep_info.robot_st.b.self_check_st&0x01)<<4)|
				   ((s_data->ar_info.robot_st.b.self_check_st&0x01)<<3)|
				   ((s_data->br_info.robot_st.b.self_check_st&0x01)<<2)|
				   ((s_data->sr_info.robot_st.b.self_check_st&0x01)<<1)|
				   (s_data->check_info.check_state);
	cmp->data[2] = ((s_data->sys_info.sys_st.b.auto_driving_st&0x03)<<6)|
				   ((s_data->sys_info.sys_st.b.sys_ctrl_st&0x03)<<4)|
				   (s_data->sys_info.sys_st.b.sys_mode&0x07);
	cmp->data[3] = ((s_data->sys_info.sys_st.b.estop_st&0x03)<<5)|
				   ((s_data->sys_info.sys_st.b.take_over_st&0x01)<<4)|(0x01<<3)|
				   ((s_data->ar_info.robot_st.b.auto_ctrl_st&0x01)<<2)|
				   ((s_data->br_info.robot_st.b.auto_ctrl_st&0x01)<<1)|
				   (s_data->sr_info.robot_st.b.auto_ctrl_st&0x01);
	cmp->data[4] = ((s_data->sys_info.sys_fault.b.ebr&0x01)<<5)|
				   ((s_data->sys_info.sys_fault.b.encoder&0x01)<<4)|
				   ((s_data->sys_info.sys_fault.b.hmi&0x01)<<3)|
				   ((s_data->sys_info.sys_fault.b.power&0x01)<<2)|
				   (s_data->sys_info.sys_fault.b.sys_ctrl&0x03);
	cmp->data[5] = ((s_data->sys_info.sys_fault.b.ep_motor&0x03)<<6)|
				   ((s_data->sys_info.sys_fault.b.ar_motor&0x03)<<4)|
				   ((s_data->sys_info.sys_fault.b.br_motor&0x03)<<2)|
				   (s_data->sys_info.sys_fault.b.sr_motor&0x03);
	cmp->data[6] = ((s_data->ep_info.robot_st.b.power_st&0x03)<<6)|
			   	   ((s_data->ar_info.robot_st.b.power_st&0x03)<<4)|
				   ((s_data->br_info.robot_st.b.power_st&0x03)<<2)|
				   (s_data->sr_info.robot_st.b.power_st&0x03);
	cmp->data[7] = (((ct_5++) & 0x0f)<<4)|
				   (((~s_data->config_data.register_info.b.ep_use)&0x01)<<3)|
				   (((~s_data->config_data.register_info.b.ar_use)&0x01)<<2)|
				   (((~s_data->config_data.register_info.b.br_use)&0x01)<<1)|
				   ((~s_data->config_data.register_info.b.sr_use)&0x01);
	if(ct_5 > 15) ct_5 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

void actuator_position_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data)
{
	static uint8_t ct_6;
	uint8_t ep_pos_status = 0;
	//帧ID
	cmp->std_id = std_id;
	if(s_data->sr_info.angle >= 0)
	{
		cmp->data[1] =	0x01;
	}
	else
	{
		cmp->data[1] =	0x02;
	}

	if(ABS(s_data->ep_info.encoder_value - s_data->config_data.cali_data.ep_encoder_up) < 20)
	{
		ep_pos_status = 0;
	}
	else if(ABS(s_data->ep_info.encoder_value - s_data->config_data.cali_data.ep_encoder_down) < 20)
	{
		ep_pos_status = 1;
	}
	else
	{
		ep_pos_status = 2;
	}

	cmp->data[1] |= ((((uint16_t)ABS(s_data->sr_info.angle))&0x3f)<<2);
	cmp->data[2] = (((uint16_t)ABS(s_data->sr_info.angle))>>6)&0xff;
	cmp->data[3] = ((uint16_t)ABS(s_data->br_info.angle))&0xff;
	cmp->data[4] = ((((uint16_t)ABS(s_data->ar_info.angle))&0x3f)<<2)|
				   ((((uint16_t)ABS(s_data->br_info.angle))>>8)&0x03);
	cmp->data[5] = ((s_data->ep_info.encoder_value&0x0f)<<4)|
				   ((((uint16_t)ABS(s_data->ar_info.angle))>>6)&0x0f);
	cmp->data[6] = (s_data->ep_info.encoder_value>>4)&0xff;
	cmp->data[7] = (((ct_6++) & 0x0f)<<4)|(ep_pos_status);
	if(ct_6 > 15) ct_6 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

void actuator_alarm_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data)
{
	static uint8_t ct_7;
	//帧ID
	cmp->std_id = std_id;
	cmp->data[1] = s_data->sr_info.alarm_code&0xff;
	cmp->data[2] = ((s_data->br_info.alarm_code&0x0f)<<4)|
				   ((s_data->sr_info.alarm_code>>8)&0x0f);
	cmp->data[3] = (s_data->br_info.alarm_code>>4)&0xff;
	cmp->data[4] = s_data->ar_info.alarm_code&0xff;
	cmp->data[5] = ((s_data->ep_info.alarm_code&0x0f)<<4)|
				   ((s_data->ar_info.alarm_code>>8)&0x0f);
	cmp->data[6] = (s_data->ep_info.alarm_code>>4)&0xff;
	cmp->data[7] = ((ct_7++) & 0x0f)<<4;
	if(ct_7 > 15) ct_7 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

void actuator_status_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data)
{
	static uint8_t ct_8;
	//帧ID
	cmp->std_id = std_id;
	cmp->data[1] = ((s_data->sr_info.robot_st.b.motor_ctrl_mode&0x0f)<<4)|
			   	   ((s_data->sr_info.robot_st.b.motor_limit_st&0x01)<<3)|
				   ((s_data->sr_info.robot_st.b.motor_power_st&0x01)<<2)|
				   ((s_data->sr_info.robot_st.b.motor_online_st&0x01)<<1)|
				   (s_data->sr_info.robot_st.b.motor_power_st&0x01);
	cmp->data[2] = ((s_data->br_info.robot_st.b.motor_ctrl_mode&0x0f)<<4)|
				   ((s_data->br_info.robot_st.b.motor_limit_st&0x01)<<3)|
				   ((s_data->br_info.robot_st.b.motor_power_st&0x01)<<2)|
				   ((s_data->br_info.robot_st.b.motor_online_st&0x01)<<1)|
				   (s_data->br_info.robot_st.b.motor_power_st&0x01);
	cmp->data[3] = ((s_data->ar_info.robot_st.b.motor_ctrl_mode&0x0f)<<4)|
				   ((s_data->ar_info.robot_st.b.motor_limit_st&0x01)<<3)|
				   ((s_data->ar_info.robot_st.b.motor_power_st&0x01)<<2)|
				   ((s_data->ar_info.robot_st.b.motor_online_st&0x01)<<1)|
				   (s_data->ar_info.robot_st.b.motor_power_st&0x01);
	cmp->data[4] = ((s_data->ep_info.robot_st.b.motor_ctrl_mode&0x0f)<<4)|
				   ((s_data->ep_info.robot_st.b.motor_limit_st&0x01)<<3)|
				   ((s_data->ep_info.robot_st.b.motor_power_st&0x01)<<2)|
				   ((s_data->ep_info.robot_st.b.motor_online_st&0x01)<<1)|
				   (s_data->ep_info.robot_st.b.motor_power_st&0x01);
	cmp->data[5] = ((ct_8++) & 0x0f)<<4;
	if(ct_8 > 15) ct_8 = 0x00;
	//数据长度
	cmp->length = 6;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

void power_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data)
{
	static uint8_t ct_9;
	switch(std_id)
	{
		case PRS_1_6:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = ((uint16_t)(s_data->sys_info.power*10))&0xff;
			cmp->data[2] = ((((uint16_t)(s_data->sys_info.voltage*10))&0x01)<<7)|
						   (((uint16_t)(s_data->sys_info.power*10))>>8&0x7f);
			cmp->data[3] = (((uint16_t)(s_data->sys_info.voltage*10))>>1)&0xff;
			cmp->data[4] = (((uint16_t)(s_data->sys_info.current*10))&0xff);
			cmp->data[5] = (((ct_9++) & 0x0f)<<4)|
						   ((((uint16_t)(s_data->sys_info.current*10))>>8)&0x01);
			if(ct_9 > 15) ct_9 = 0x00;
			//数据长度
			cmp->length = 6;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
		case PRS_1_7:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = (((uint16_t)(s_data->sr_info.power*10)))&0xff;
			cmp->data[2] = ((((uint16_t)(s_data->sr_info.voltage*10))&0x01)<<7)|
						   (((uint16_t)(s_data->sr_info.power*10))>>8&0x7f);
			cmp->data[3] = (((uint16_t)(s_data->sr_info.voltage*10))>>1)&0xff;
			cmp->data[4] = (((uint16_t)(s_data->sr_info.current*10))&0xff);
			cmp->data[5] = (((ct_9++) & 0x0f)<<4)|
						   ((((uint16_t)(s_data->sr_info.current*10))>>8)&0x01);
			if(ct_9 > 15) ct_9 = 0x00;
			//数据长度
			cmp->length = 6;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
		case PRS_1_8:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = (((uint16_t)(s_data->br_info.power*10)))&0xff;
			cmp->data[2] = ((((uint16_t)(s_data->br_info.voltage*10))&0x01)<<7)|
						   ((((uint16_t)s_data->br_info.power*10))>>8&0x7f);
			cmp->data[3] = (((uint16_t)(s_data->br_info.voltage*10))>>1)&0xff;
			cmp->data[4] = (((uint16_t)(s_data->br_info.current*10))&0xff);
			cmp->data[5] = (((ct_9++) & 0x0f)<<4)|
						   ((((uint16_t)(s_data->br_info.current*10))>>8)&0x01);
			if(ct_9 > 15) ct_9 = 0x00;
			//数据长度
			cmp->length = 6;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
		case PRS_1_9:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = (((uint16_t)(s_data->ar_info.power*10)))&0xff;
			cmp->data[2] = ((((uint16_t)(s_data->ar_info.voltage*10))&0x01)<<7)|
						   ((((uint16_t)s_data->ar_info.power*10))>>8&0x7f);
			cmp->data[3] = (((uint16_t)(s_data->ar_info.voltage*10))>>1)&0xff;
			cmp->data[4] = (((uint16_t)(s_data->ar_info.current*10))&0xff);
			cmp->data[5] = (((ct_9++) & 0x0f)<<4)|
						   ((((uint16_t)(s_data->ar_info.current*10))>>8)&0x01);
			if(ct_9 > 15) ct_9 = 0x00;
			//数据长度
			cmp->length = 6;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
		case PRS_1_10:
		{
			//帧ID
			cmp->std_id = std_id;
			cmp->data[1] = (((uint16_t)(s_data->ep_info.power*10)))&0xff;
			cmp->data[2] = ((((uint16_t)(s_data->ep_info.voltage*10))&0x01)<<7)|
						   ((((uint16_t)s_data->ep_info.power*10))>>8&0x7f);
			cmp->data[3] = (((uint16_t)(s_data->ep_info.voltage*10))>>1)&0xff;
			cmp->data[4] = (((uint16_t)(s_data->ep_info.current*10))&0xff);
			cmp->data[5] = (((ct_9++) & 0x0f)<<4)|
						   ((((uint16_t)(s_data->ep_info.current*10))>>8)&0x01);
			if(ct_9 > 15) ct_9 = 0x00;
			//数据长度
			cmp->length = 6;
			cmp->rtr = false;
			//优先等级
			cmp->prty = 8;
			cmp->data[0] = check_sum(cmp);
			break;
		}
	}
}

void actuator_CVP_msg(CAN_MSG* cmp, uint16_t std_id, ROBOT_INFO* motor)
{
	static uint8_t ct_7;
	int16_t temp = 0;
	//帧ID
	cmp->std_id = std_id;

	temp = (int16_t) (motor->torque * 100);
	cmp->data[1] = temp & 0xff;
	cmp->data[2] = (temp >> 8) & 0xff;
	temp = (int16_t) (motor->speed * 100);
	cmp->data[3] = temp & 0xff;
	cmp->data[4] = (temp >> 8) & 0xff;
	cmp->data[5] = 0;
	cmp->data[6] = 0;
	cmp->data[7] = ((ct_7++) & 0x0f)<<4;
	if(ct_7 > 15) ct_7 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}
void actuator_CVP_msg_ext(CAN_MSG* cmp, uint16_t std_id, ROBOT_INFO* motor, volatile SHARED_DATA* s_data)
{
	static uint8_t ct_7;
	int16_t temp = 0;
	//帧ID
	cmp->std_id = std_id;

	temp = (int16_t) (motor->torque * 100);
	cmp->data[1] = temp & 0xff;
	cmp->data[2] = (temp >> 8) & 0xff;
	temp = (int16_t) (motor->speed * 100);
	cmp->data[3] = temp & 0xff;
	cmp->data[4] = (temp >> 8) & 0xff;
	cmp->data[5] = s_data->force.BR & 0xff;
	cmp->data[6] = (s_data->force.BR >> 8) & 0xff;
	cmp->data[7] = ((ct_7++) & 0x0f)<<4;
	if(ct_7 > 15) ct_7 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}
// 上传制动、加速机器人百分比位置
//void actuator_Ratio_msg(CAN_MSG* cmp, uint16_t std_id, CONFIG_DATA* cfg, ROBOT_INFO* br, ROBOT_INFO* ar)
void actuator_Ratio_msg(CAN_MSG* cmp, uint16_t std_id, volatile SHARED_DATA* s_data)
{
	static uint8_t ct_7;
	uint16_t tempA, tempB;
	//帧ID
	cmp->std_id = std_id;
	tempB = (uint16_t)((s_data->br_info.angle / s_data->config_data.cali_data.br_max_angle)*10);
	tempA = (uint16_t)((s_data->ar_info.angle / s_data->config_data.cali_data.ar_max_angle)*10);

	cmp->data[1] = tempB & 0xff;
	cmp->data[2] = ((tempA & 0x3F) << 2)|((tempB >> 8) & 0x03);
	cmp->data[3] = (tempA >> 6) & 0x0f;
	cmp->data[4] = 0;
	cmp->data[5] = 0;
	cmp->data[6] = 0;
	cmp->data[7] = ((ct_7++) & 0x0f)<<4;
	if(ct_7 > 15) ct_7 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);
}

// 2023-9-13  新增 0x19F  上传 EP 角度信息等
void actuator_CVP_EP_msg(CAN_MSG* cmp, uint16_t std_id, ROBOT_INFO* motor,volatile SHARED_DATA* s_data)
{

	static uint8_t ct_7;
	int16_t temp = 0;
	//帧ID
	cmp->std_id = std_id;

	cmp->data[1] =     (ABS(s_data->ep_info.angle))>>8 ;
	cmp->data[2] =     (ABS(s_data->ep_info.angle))&0xFF ;

	temp = (int16_t) (motor->torque * 100);
	cmp->data[3] = temp & 0xff;
	cmp->data[4] = (temp >> 8) & 0xff;

	temp = (int16_t) (motor->speed * 100);
	cmp->data[5] = temp & 0xff;
	cmp->data[6] = (temp >> 8) & 0xff;

	cmp->data[7] = ((ct_7++) & 0x0f)<<4;
	if(ct_7 > 15) ct_7 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = check_sum(cmp);

}
