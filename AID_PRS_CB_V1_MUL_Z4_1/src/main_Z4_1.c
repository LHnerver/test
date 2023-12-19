/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include <MPC5748G.h>
#include "math.h"
#include "stdlib.h"
#include "sh_mem.h"
#include "platform.h"
#include "canmsgapi.h"
#include "SCA_APP.h"
#include "encoder.h"
#include "beep.h"
#include "ctrl_can.h"
#include "flash.h"
#include "linflexd_uart.h"
#include "flexcan.h"
#include "security_policy.h"
#include "beep.h"

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define VOLTAGE_MIN    21



#define   EP_BRAKE_SYMBOL     0  // 增加EP制动的宏




/*******************************************************************************
* Global variables
*******************************************************************************/
extern SHARED_DATA __shared_data[];
volatile SHARED_DATA *s_data = __shared_data;

SYSTEM_TIME system_time;

DELAY_TIME delay_time;

SYS_INFO sys_info;

CHECK_INFO check_info;

CONFIG_DATA config_data;

DEVICE_INFO device_info;

CAN_CTRL_MSG can_ctrl_msg;

ROBOT_INFO sr_info;

ROBOT_INFO ar_info;

ROBOT_INFO br_info;

ROBOT_INFO ep_info;

//CAN端口信息定义，用于绑定SCA句柄，实现多端口控制
CAN_HANDLER can_port1, can_port2, can_port3, can_port4;

// IPC 通信
CAN_MSG can0_tx_msg;            	//CAN0写数据
CAN_MSG can0_rx_msg;             	//CAN0读数据

// ADCU 通信
CAN_MSG can1_tx_msg;            	//CAN1写数据
CAN_MSG can1_rx_msg;             	//CAN1读数据

// EP 关节通信
CAN_MSG can2_tx_msg;            	//CAN2写数据
CAN_MSG can2_rx_msg;             	//CAN2读数据

// SR 关节通信
CAN_MSG can3_tx_msg;            	//CAN3写数据
CAN_MSG can3_rx_msg;             	//CAN3读数据

// BR 关节通信
CAN_MSG can5_tx_msg;            	//CAN5写数据
CAN_MSG can5_rx_msg;             	//CAN5读数据

// AR 关节通信
CAN_MSG can7_tx_msg;            	//CAN7写数据
	//CAN7读数据

//更新缓存数据
SCA_HANDLER* PSCA;

/*******************************************************************************
* Function prototypes
*******************************************************************************/
CAN_MSG can7_rx_msg;
extern void xcptn_xmpl(void);

void param_init(void);
void multicore_share_data(void);

/*******************************************************************************
* Global Functions
*******************************************************************************/
void multicore_share_data(void)
{
	uint8_t status = UNLOCK;

	/* Critical section */
	/* when the Gate is not lock */
	while(status != CORE1_LOCK)
	{
		/* Lock the gate */
		status = lock_gate(GATE_0);
	}

	/* Execute critical section */
	/* Read */

	if (s_data->sys_info.enter_cali_flag || s_data->sys_info.enter_debug_flag)
	{
		s_data->sys_info.enter_cali_flag = 0;
		s_data->sys_info.enter_debug_flag = 0;
//		canCacheMsgT.Driving_Ste_Sign = 1;
//		canCacheMsgT.Driving_Ste_Bo = true;
//		canCacheMsgT.Driving_Brk_Sign = 1;
//		canCacheMsgT.Driving_Brk_Bo = true;
//		canCacheMsgT.Driving_Thr_Sign = 1;
//		canCacheMsgT.Driving_Thr_Bo = true;
	}

	if (s_data->sys_info.exit_cali_flag || s_data->sys_info.exit_debug_flag)
	{
		s_data->sys_info.exit_cali_flag = 0;
		s_data->sys_info.exit_debug_flag = 0;

//		canCacheMsgT.Driving_Ste_Sign = 0;
//		canCacheMsgT.Driving_Ste_Bo = true;
//		canCacheMsgT.Driving_Brk_Sign = 0;
//		canCacheMsgT.Driving_Brk_Bo = true;
//		canCacheMsgT.Driving_Thr_Sign = 0;
//		canCacheMsgT.Driving_Thr_Bo = true;
	}

	/* Write */
	s_data->sys_info.sys_fault = sys_info.sys_fault;
	s_data->sys_info.sys_st = sys_info.sys_st;
	s_data->check_info = check_info;
	s_data->config_data = config_data;
	s_data->sr_info = sr_info;
	s_data->ar_info = ar_info;
	s_data->br_info = br_info;
	s_data->ep_info = ep_info;

	while(status == CORE1_LOCK)
	{
		/* Unlock the gate */
		status = unlock_gate(GATE_0);
	}
	/*End of the critical section*/
}

void param_init(void)
{
	delay_time.base_100us_cnt = 0;
	delay_time.base_100us_flag = false;
	delay_time.base_1ms_cnt = 0;
	delay_time.base_1ms_flag = false;

	system_time.base_1ms_cnt = 0;
	system_time.base_1ms_flag = false;
	system_time.base_5ms_cnt = 0;
	system_time.base_5ms_flag = false;
	system_time.base_10ms_cnt = 0;
	system_time.base_10ms_flag = false;
	system_time.base_20ms_cnt = 0;
	system_time.base_20ms_flag = false;
	system_time.base_40ms_cnt = 0;
	system_time.base_40ms_flag = false;
	system_time.base_50ms_flag = 0;
	system_time.base_50ms_flag = false;
	system_time.base_100ms_cnt = 0;
	system_time.base_100ms_flag = false;
	system_time.base_125ms_cnt = 0;
	system_time.base_125ms_flag = false;
	system_time.base_200ms_cnt = 0;
	system_time.base_200ms_flag = false;
	system_time.base_250ms_cnt = 0;
	system_time.base_250ms_flag = false;
	system_time.base_500ms_cnt = 0;
	system_time.base_500ms_flag = false;
	system_time.base_1000ms_cnt = 0;
	system_time.base_1000ms_flag = false;

	//config_data.register_info.r = 0x1e;

	check_info.check_state = 0;
	check_info.check_step = 0;
	check_info.in_power_state = 0;
	check_info.ep_state = 0;
	check_info.ar_state = 0;
	check_info.br_state = 0;
	check_info.sr_state = 0;
	check_info.hmi_state = 1;
	check_info.encoder_state = 0;
	check_info.retry_count = 0;
	check_info.time_count = 0;
	check_info.failure_info = 0;

	sys_info.sys_st.b.sys_ctrl_st = 1;

	//get from flash
	flash_get_cali_data(&config_data.register_info,
			&config_data.cali_data,&device_info);
}

//0x0：无效
//0x1：自检模式
//0x2：手动模式
//0x3：自动模式
//0x4：安全模式
//0x5：故障模式

void process_judge_event(void)
{
	if(sys_info.sys_st.b.self_check_st == 1)
	{
		//处于手动驾驶状态才切换
		if(sys_info.sys_st.b.sys_mode == 2)
		{
			if(SW_PWR == 1)
			{
				sr_info.robot_st.b.change_mode_st = 0;
				br_info.robot_st.b.change_mode_st = 0;
				ar_info.robot_st.b.change_mode_st = 0;
				ep_info.robot_st.b.change_mode_st = 0;
				sys_info.sys_st.b.auto_driving_st = 0;
				sys_info.sys_st.b.take_over_st = 0;
				CAN_communication_abnormal_flag = 0 ;
				SW_LED = 1;//亮灯
			}
		}

		//处于自动驾驶状态才切换
		if(sys_info.sys_st.b.sys_mode == 3 ||
		   sys_info.sys_st.b.sys_mode == 4)
		{
			// 如果有自动驾驶控制请求，则根据按键进入自动驾驶状态
			if (AUTO_DI5 == 0 || AUTO_DI6 == 0)
			{
				sys_info.sys_st.b.auto_driving_st = 1;
			}

			// 自动驾驶状态时，接管踏板、手柄
			if(MAN_DH == 0 || MAN_DP == 0)
			{
				sr_info.robot_st.b.change_mode_st = 1;
				br_info.robot_st.b.change_mode_st = 1;
				br_info.limit_time_count = 0;
				ar_info.robot_st.b.change_mode_st = 1;
				ar_info.limit_time_count = 0;
				ep_info.robot_st.b.change_mode_st = 1;
				ep_info.limit_time_count = 0;
				sys_info.sys_st.b.auto_driving_st = 0;
				sys_info.sys_st.b.sys_mode = 2;
				sys_info.sys_st.b.take_over_st = 1;
				SW_LED = 0;//灭灯
			}
		}
	}
}

void process_can0_1_rx_msg(void)
{
	float real_temp;
    //  非自动和安全模式   机器人参数标定 位置 左右零点
	if(can_ctrl_msg.robot_limit_cali_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//转向电机已开启
			{
				if(can_ctrl_msg.sr_limit_cali_req != 0)//标定转向电机
				{
					switch(can_ctrl_msg.sr_limit_cali_req)//转向电机标定判定
					{
						case 1://零点标定
						{
							if(SCA_Set_Up_Home(SR_ID) == SCA_NoError)
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_OK,0x00);//封装通用应答
								can0_send_msg(can0_tx_msg);	//CAN0发送数据
							}
							else
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_FAIL,0x01);//封装通用应答
								can0_send_msg(can0_tx_msg);	//CAN0发送数据
							}
							break;
						}
						case 2://极左点标定
						{
							if(SCA_Pos_Min_Limit(SR_ID,(float)(sr_info.angle/STE_INF_RE)) == SCA_NoError)
							{
								config_data.cali_data.sr_left_angle =
										sr_info.angle;//数据存储到结构体变量
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_OK,0x00);//封装通用应答
								can0_send_msg(can0_tx_msg);	//CAN0发送数据
							}
							else
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_FAIL,0x02);//封装通用应答
								can0_send_msg(can0_tx_msg);	//CAN0发送数据
							}
							break;
						}
						case 3://极右点标定
						{
							if(SCA_Pos_Max_Limit(SR_ID,(float)(sr_info.angle/STE_INF_RE)) == SCA_NoError)
							{
								config_data.cali_data.sr_right_angle =
										sr_info.angle;//数据存储到结构体变量
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_OK,0x00);//封装通用应答
								can0_send_msg(can0_tx_msg);	//CAN0发送数据
							}
							else
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_FAIL,0x03);//封装通用应答
								can0_send_msg(can0_tx_msg);	//CAN0发送数据
							}
							break;
						}
					}
					can_ctrl_msg.sr_limit_cali_req = 0;
				}
			}
			else
			{
				can_ctrl_msg.sr_limit_cali_req = 0;
			}
		}
		else
		{
			can_ctrl_msg.sr_limit_cali_req = 0;
		}

		//制动
		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				if(can_ctrl_msg.br_limit_cali_req == 1)//制动电机
				{
					//向下运动，直到力矩达到停止
					if(SCA_Move_Profile_Vel(BR_ID, (float)6, (float)-100,
												(float)100) == SCA_NoError)
					{
						can_ctrl_msg.br_limit_cali_req = 2;
						can_ctrl_msg.limit_retry_count = 0;
						can_ctrl_msg.limit_time_count = 0;
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.br_limit_cali_req = 0x7f;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.br_limit_cali_req == 2)
				{
					PSCA = SCA_Handler_Update(BR_ID);
					real_temp = PSCA->Current_Real;
					if (real_temp < -4)
					{
						if(SCA_Mode_Actuator(BR_ID,0x01) == SCA_NoError)
						{
							can_ctrl_msg.br_limit_cali_req = 3;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
						else
						{
							can_ctrl_msg.limit_retry_count++;
							if(can_ctrl_msg.limit_retry_count > 9)
							{
								can_ctrl_msg.br_limit_cali_req = 0x7c;
								can_ctrl_msg.limit_retry_count = 0;
								can_ctrl_msg.limit_time_count = 0;
							}
						}
					}
					else
					{
						can_ctrl_msg.limit_time_count++;
						if(can_ctrl_msg.limit_time_count > 400)
						{
							can_ctrl_msg.br_limit_cali_req = 0x7e;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.br_limit_cali_req == 3)
				{
					if(SCA_Real_Attribute(BR_ID) == SCA_NoError)
					{
						PSCA = SCA_Handler_Update(BR_ID);
						real_temp = (PSCA->Position_Real);
						if(SCA_Pos_Min_Limit(BR_ID,(float)(real_temp)) == SCA_NoError)
						{
							config_data.cali_data.br_max_angle =
									(int16_t)(real_temp*BRK_INF_RE);//数据存储到结构体变量
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_OK,0x00);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据


							can_ctrl_msg.br_limit_cali_req = 4;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.br_limit_cali_req = 0x7d;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.br_limit_cali_req == 4)
				{
					if(SCA_Move_Pos(BR_ID, (float)(0/BRK_INF_RE),
						(float)500, (float)500, (float)-500) == SCA_NoError)
					{
						can_ctrl_msg.br_limit_cali_req = 0;
						can_ctrl_msg.limit_retry_count = 0;
						can_ctrl_msg.limit_time_count = 0;
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.br_limit_cali_req = 0x7b;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.br_limit_cali_req == 5)  //修改原来上电自动设置零点的操作，修改为上位机设置零点的操作
				{
					SCA_Set_Up_Home(BR_ID);
					SCA_Set_Up_Home(BR_ID);
					if(SCA_Set_Up_Home(BR_ID) == SCA_NoError)
					{

						general_replay_msg(&can0_tx_msg,
										   PRS_1_1,CTRL_0_1,BR_ID,RESULT_OK,0x00);//封装通用应答
											can0_send_msg(can0_tx_msg);	//CAN0发送数据

						can_ctrl_msg.br_limit_cali_req = 0 ;
						config_data.cali_data.br_zero_valid = 0 ;
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.br_limit_cali_req = 0x7a;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else
				{
					switch(can_ctrl_msg.br_limit_cali_req)
					{
						case 0x7f:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x04);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7e:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x05);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7d:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x06);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7c:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x07);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7b:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x08);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7a:   //增加BR电机设置零点错误的返回msg
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x09);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
					}
				}
			}
		}

		//加速
		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				if(can_ctrl_msg.ar_limit_cali_req != 0)//加速电机
				{
					if(can_ctrl_msg.ar_limit_cali_req)//加速电机标定判定
					{
						if(SCA_Pos_Max_Limit(AR_ID,(float)(ar_info.angle/THR_INF_RE)) == SCA_NoError)
						{
							config_data.cali_data.ar_max_angle =
									ar_info.angle;//数据存储到结构体变量
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,AR_ID,RESULT_OK,0x00);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
						}
						else
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,AR_ID,RESULT_FAIL,0x09);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
						}
					}
					can_ctrl_msg.ar_limit_cali_req = 0;
				}
			}
		}

		//驻车
		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				if(can_ctrl_msg.ep_limit_cali_req == 1)//驻车电机
				{
					//向下运动，直到力矩达到停止
					if(SCA_Move_Profile_Vel(EP_ID, (float)10, (float)-100,
												(float)100) == SCA_NoError)
					{
						can_ctrl_msg.ep_limit_cali_req = 2;
						can_ctrl_msg.limit_retry_count = 0;
						can_ctrl_msg.limit_time_count = 0;
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.ep_limit_cali_req = 0x7f;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.ep_limit_cali_req == 2)
				{
					PSCA = SCA_Handler_Update(EP_ID);
					real_temp = PSCA->Current_Real;    //当前电流值
					if (real_temp < -7)
					{
						if(SCA_Mode_Actuator(EP_ID,0x01) == SCA_NoError)
						{
							can_ctrl_msg.ep_limit_cali_req = 3;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
						else
						{
							can_ctrl_msg.limit_retry_count++;
							if(can_ctrl_msg.limit_retry_count > 9)
							{
								can_ctrl_msg.ep_limit_cali_req = 0x7c;
								can_ctrl_msg.limit_retry_count = 0;
								can_ctrl_msg.limit_time_count = 0;
							}
						}
					}
					else
					{
						can_ctrl_msg.limit_time_count++;
						if(can_ctrl_msg.limit_time_count > 400)
						{
							can_ctrl_msg.ep_limit_cali_req = 0x7e;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.ep_limit_cali_req == 3)
				{
					if(SCA_Real_Attribute(EP_ID) == SCA_NoError)
					{
						PSCA = SCA_Handler_Update(EP_ID);
						real_temp = (PSCA->Position_Real);
						if(SCA_Pos_Min_Limit(EP_ID,(float)(real_temp)) == SCA_NoError)
						{
							config_data.cali_data.ep_encoder_down =
									ep_info.encoder_value;
							config_data.cali_data.ep_max_angle =
									real_temp*EP_INF_RE;//数据存储到结构体变量
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_OK,0x00);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据

							can_ctrl_msg.ep_limit_cali_req = 4;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.ep_limit_cali_req = 0x7d;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.ep_limit_cali_req == 4)
				{
					if(SCA_Move_Pos(EP_ID, (float)(-10/EP_INF_RE),
						(float)500, (float)500, (float)-500) == SCA_NoError)
					{
						can_ctrl_msg.ep_limit_cali_req = 0;
						can_ctrl_msg.limit_retry_count = 0;
						can_ctrl_msg.limit_time_count = 0;
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.ep_limit_cali_req = 0x7b;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else if(can_ctrl_msg.ep_limit_cali_req == 5) //修改原来EP电机自动上电标零的操作，改为上位机标定零点的版本
				{

					SCA_Set_Up_Home(EP_ID);
					SCA_Set_Up_Home(EP_ID);
					if(SCA_Set_Up_Home(EP_ID) == SCA_NoError)
					{
						can_ctrl_msg.ep_limit_cali_req  =  0 ;
						general_replay_msg(&can0_tx_msg,PRS_1_1,CTRL_0_1,EP_ID,RESULT_OK,0x00);//封装通用应答can0_send_msg(can0_tx_msg);	//CAN0发送数据
						config_data.cali_data.ep_zero_valid = 0 ;
					}
					else
					{
						can_ctrl_msg.limit_retry_count++;
						if(can_ctrl_msg.limit_retry_count > 9)
						{
							can_ctrl_msg.ep_limit_cali_req = 0x7a;
							can_ctrl_msg.limit_retry_count = 0;
							can_ctrl_msg.limit_time_count = 0;
						}
					}
				}
				else
				{
					switch(can_ctrl_msg.ep_limit_cali_req)
					{
						case 0x7f:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0A);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7e:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0B);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7d:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0C);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7c:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0D);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7b:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0E);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
						case 0x7a:    //增加EP电机设置零点失败的返回操作
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0F);//封装通用应答
							can0_send_msg(can0_tx_msg);	//CAN0发送数据
							break;
						}
					}
				}
			}
		}

		//标定操作结束后进行存储工作
		if(can_ctrl_msg.sr_limit_cali_req == 0 &&
				can_ctrl_msg.br_limit_cali_req == 0 &&
				can_ctrl_msg.ar_limit_cali_req == 0 &&
				can_ctrl_msg.ep_limit_cali_req == 0)
		{
			//save to flash
			flash_save_cali_data(config_data.register_info,
					config_data.cali_data,device_info);
			//get from flash
			flash_get_cali_data(&config_data.register_info,
					&config_data.cali_data,&device_info);

			//关闭标定操作
			can_ctrl_msg.robot_limit_cali_bo = false;
		}
		else
		{
			if(can_ctrl_msg.br_limit_cali_req > 0x7a ||
			      can_ctrl_msg.ar_limit_cali_req > 0x7a ||
			      can_ctrl_msg.ep_limit_cali_req > 0x7a )
			   {
					can_ctrl_msg.br_limit_cali_req = 0;
					can_ctrl_msg.ar_limit_cali_req = 0;
					can_ctrl_msg.ep_limit_cali_req = 0;
					//关闭标定操作
					can_ctrl_msg.robot_limit_cali_bo = false;
			   }
		}
	}
   // 非自动和安全模式   机器人运动参数标准加速度减速度等
	if(can_ctrl_msg.robot_param_cali_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//转向电机已开启
			{
				if(can_ctrl_msg.sr_param_cali_req != 0)//标定转向电机
				{
					config_data.cali_data.sr_acc_dec =
							can_ctrl_msg.sr_acc_dec_cali;
					general_replay_msg(&can0_tx_msg,
							PRS_1_1,CTRL_0_2,SR_ID,RESULT_OK,0x00);//封装通用应答
					can0_send_msg(can0_tx_msg);	//CAN0发送数据
					can_ctrl_msg.sr_param_cali_req = 0;
				}
			}
		}

		//制动
		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				if(can_ctrl_msg.br_param_cali_req == 1)//制动电机
				{
					config_data.cali_data.br_speed =
							can_ctrl_msg.br_speed_cali;
					config_data.cali_data.br_acc_dec =
							can_ctrl_msg.br_acc_dec_cali;
					general_replay_msg(&can0_tx_msg,
							PRS_1_1,CTRL_0_2,BR_ID,RESULT_OK,0x00);//封装通用应答
					can0_send_msg(can0_tx_msg);	//CAN0发送数据
					can_ctrl_msg.br_param_cali_req = 0;
				}
			}
		}

		//加速
		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				if(can_ctrl_msg.ar_param_cali_req != 0)//加速电机
				{
					config_data.cali_data.ar_speed =
							can_ctrl_msg.ar_speed_cali;
					config_data.cali_data.ar_acc_dec =
							can_ctrl_msg.ar_acc_dec_cali;
					general_replay_msg(&can0_tx_msg,
							PRS_1_1,CTRL_0_2,AR_ID,RESULT_OK,0x00);//封装通用应答
					can0_send_msg(can0_tx_msg);	//CAN0发送数据
					can_ctrl_msg.ar_param_cali_req = 0;
				}
			}
		}

		//标定操作结束后进行存储工作
		if(can_ctrl_msg.sr_param_cali_req == 0 &&
				can_ctrl_msg.br_param_cali_req == 0 &&
				can_ctrl_msg.ar_param_cali_req == 0)
		{
			//save to flash
			flash_save_cali_data(config_data.register_info,
					config_data.cali_data,device_info);
			//get from flash
			flash_get_cali_data(&config_data.register_info,
					&config_data.cali_data,&device_info);

			//关闭标定操作
			can_ctrl_msg.robot_param_cali_bo = false;
		}
	}

	if(can_ctrl_msg.cali_data_query_bo)
	{
		if(can_ctrl_msg.cali_data_query_req)
		{
			calibration_data_query_replay_msg(&can0_tx_msg,PRS_0_1,
					config_data.cali_data,device_info);//封装通用应答
			can0_send_msg(can0_tx_msg);	//CAN0发送数据

			calibration_data_query_replay_msg(&can0_tx_msg,PRS_0_2,
					config_data.cali_data,device_info);//封装通用应答
			can0_send_msg(can0_tx_msg);	//CAN0发送数据

			calibration_data_query_replay_msg(&can0_tx_msg,PRS_0_3,
					config_data.cali_data,device_info);//封装通用应答
			can0_send_msg(can0_tx_msg);	//CAN0发送数据

			//关闭查询操作
			can_ctrl_msg.cali_data_query_req = 0;
		}
		can_ctrl_msg.cali_data_query_bo = false;
	}

	if(can_ctrl_msg.motor_info_query_bo)
	{
		if(can_ctrl_msg.motor_voltage_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x01,(int32_t)(sr_info.voltage * IQ10));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x01,(int32_t)(br_info.voltage * IQ10));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x01,(int32_t)(ar_info.voltage * IQ10));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x01,(int32_t)(ep_info.voltage * IQ10));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}

		if(can_ctrl_msg.motor_current_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x02,(int32_t)(sr_info.current * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x02,(int32_t)(br_info.current * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x02,(int32_t)(ar_info.current * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x02,(int32_t)(ep_info.current * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}

		if(can_ctrl_msg.motor_speed_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x03,(int32_t)(sr_info.speed * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x03,(int32_t)(br_info.speed * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x03,(int32_t)(ar_info.speed * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x03,(int32_t)(ep_info.speed * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}

		if(can_ctrl_msg.motor_pos_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,0x04,
						(int32_t)(sr_info.angle/STE_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,0x04,
						(int32_t)(br_info.angle/BRK_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,0x04,
						(int32_t)(ar_info.angle/THR_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,0x04,
						(int32_t)(ep_info.angle/EP_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}

		if(can_ctrl_msg.motor_temp_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x05,(int32_t)(sr_info.temperature * IQ8));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x05,(int32_t)(br_info.temperature * IQ8));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x05,(int32_t)(ar_info.temperature * IQ8));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x05,(int32_t)(ep_info.temperature * IQ8));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}

		if(can_ctrl_msg.motor_pos_max_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x06,(int32_t)(config_data.cali_data.sr_left_angle/
								STE_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x06,(int32_t)(config_data.cali_data.br_max_angle/
								BRK_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x06,(int32_t)(config_data.cali_data.ar_max_angle/
								THR_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x06,(int32_t)(config_data.cali_data.ep_max_angle/
								EP_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}

		if(can_ctrl_msg.motor_pos_min_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x07,(int32_t)(config_data.cali_data.sr_right_angle/
								STE_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x07,(int32_t)(config_data.cali_data.br_min_angle/
								BRK_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x07,(int32_t)(config_data.cali_data.ar_min_angle/
								THR_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x07,(int32_t)(config_data.cali_data.ep_min_angle/
								EP_INF_RE * IQ24));//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
		}
		can_ctrl_msg.motor_info_query_bo = false;
	}

	if(can_ctrl_msg.device_info_set_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		//save value to register
		device_info.serial_num = can_ctrl_msg.serial_number;
		device_info.device_id = can_ctrl_msg.device_id;
		device_info.hardware_ver = can_ctrl_msg.hardware_version;

		//save value to flash
		flash_save_cali_data(config_data.register_info,
				config_data.cali_data,device_info);
		//get value from flash
		flash_get_cali_data(&config_data.register_info,
				&config_data.cali_data,&device_info);

		//reply msg
		general_replay_msg(&can0_tx_msg,
				PRS_1_1,CTRL_0_5,0x00,RESULT_OK,0x00);//封装通用应答
		can0_send_msg(can0_tx_msg);	//CAN0发送数据

		can_ctrl_msg.device_info_set_bo = false;
	}

	if(can_ctrl_msg.device_info_query_bo)
	{
		if(can_ctrl_msg.device_info_query_req)
		{
			device_info_reply_msg(&can0_tx_msg, PRS_0_6,device_info);//封装通用应答
			can0_send_msg(can0_tx_msg);	//CAN0发送数据

			device_info_reply_msg(&can0_tx_msg, PRS_0_7,device_info);//封装通用应答
			can0_send_msg(can0_tx_msg);	//CAN0发送数据

			can_ctrl_msg.device_info_query_req = 0;
		}
		can_ctrl_msg.device_info_query_bo = false;
	}

	if(can_ctrl_msg.cali_data_clear_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(can_ctrl_msg.cali_data_clear_req)
		{
			config_data.cali_data.sr_left_angle = -1;
			config_data.cali_data.sr_right_angle = -1;
			config_data.cali_data.br_max_angle = -1;
			config_data.cali_data.ar_max_angle = -1;
			config_data.cali_data.ep_max_angle = -1;

			config_data.cali_data.sr_acc_dec = -1;
			config_data.cali_data.br_speed = -1;
			config_data.cali_data.br_acc_dec = -1;
			config_data.cali_data.ar_speed = -1;
			config_data.cali_data.ar_acc_dec = -1;

			//save to flash
			flash_save_cali_data(config_data.register_info,
					config_data.cali_data,device_info);
			//get from flash
			flash_get_cali_data(&config_data.register_info,
					&config_data.cali_data,&device_info);

			//reply msg
			general_replay_msg(&can0_tx_msg,
					PRS_1_1,CTRL_0_7,0x00,RESULT_OK,0x00);//封装通用应答
			can0_send_msg(can0_tx_msg);	//CAN0发送数据

			can_ctrl_msg.cali_data_clear_req = 0;
		}
		can_ctrl_msg.cali_data_clear_bo = false;
	}

	if(can_ctrl_msg.robot_disable_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(can_ctrl_msg.sr_disable_req)    /* 转向机器人禁用请求：0-不禁用 1-禁用 */
		{
			config_data.register_info.b.sr_use = 0;
		}
		else
		{
			config_data.register_info.b.sr_use = 1;
		}

		if(can_ctrl_msg.br_disable_req)
		{
			config_data.register_info.b.br_use = 0;
		}
		else
		{
			config_data.register_info.b.br_use = 1;
		}

		if(can_ctrl_msg.ar_disable_req)
		{
			config_data.register_info.b.ar_use = 0;
		}
		else
		{
			config_data.register_info.b.ar_use = 1;
		}

		if(can_ctrl_msg.ep_disable_req)
		{
			config_data.register_info.b.ep_use = 0;    // 0  不禁止   1 禁止
		}
		else
		{
			config_data.register_info.b.ep_use = 1;
		}

		//save to flash
		flash_save_cali_data(config_data.register_info,
				config_data.cali_data,device_info);
		//get from flash
		flash_get_cali_data(&config_data.register_info,
				&config_data.cali_data,&device_info);

		//reply msg
		general_replay_msg(&can0_tx_msg,
				PRS_1_1,CTRL_0_8,0x00,RESULT_OK,0x00);//封装通用应答
		can0_send_msg(can0_tx_msg);	//CAN0发送数据

		can_ctrl_msg.robot_disable_bo = false;
	}

	if (can_ctrl_msg.motor_clearERR_bo)   // 0x109  清除故障报警
	{
		if(config_data.register_info.b.sr_use && can_ctrl_msg.sr_clearERR_req)//转向电机已启用&&请求
		{
			//使能驱动器
			if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(SR_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,SR_ID,RESULT_OK,0x00);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,SR_ID,RESULT_FAIL,0x01);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			can_ctrl_msg.sr_clearERR_req = 0;
		}

		if(config_data.register_info.b.br_use && can_ctrl_msg.br_clearERR_req)//制动电机已启用&&请求
		{
			//使能驱动器
			if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(BR_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,BR_ID,RESULT_OK,0x00);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,BR_ID,RESULT_FAIL,0x01);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			can_ctrl_msg.br_clearERR_req = 0;
		}

		if(config_data.register_info.b.ar_use && can_ctrl_msg.ar_clearERR_req)//油门电机已启用&&请求
		{
			//使能驱动器
			if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(AR_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,AR_ID,RESULT_OK,0x00);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,AR_ID,RESULT_FAIL,0x01);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			can_ctrl_msg.ar_clearERR_req = 0;
		}

		if(config_data.register_info.b.ep_use && can_ctrl_msg.ep_clearERR_req)//驻车电机已启用&&请求
		{
			//使能驱动器
			if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(EP_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,EP_ID,RESULT_OK,0x00);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,EP_ID,RESULT_FAIL,0x01);//封装通用应答
				can0_send_msg(can0_tx_msg);	//CAN0发送数据
			}
			can_ctrl_msg.ep_clearERR_req = 0;
		}
		can_ctrl_msg.motor_clearERR_bo = false;
	}

	if(can_ctrl_msg.robot_infoSwitch_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(can_ctrl_msg.sr_infoSwitch_req)
		{
			config_data.register_info.b.sr_infoSwitch = 1;      //开启转向开关
		}
		else
		{
			config_data.register_info.b.sr_infoSwitch = 0;
		}

		if(can_ctrl_msg.br_infoSwitch_req)
		{
			config_data.register_info.b.br_infoSwitch = 1;
		}
		else
		{
			config_data.register_info.b.br_infoSwitch = 0;
		}

		if(can_ctrl_msg.ar_infoSwitch_req)
		{
			config_data.register_info.b.ar_infoSwitch = 1;
		}
		else
		{
			config_data.register_info.b.ar_infoSwitch = 0;
		}

		if(can_ctrl_msg.ep_infoSwitch_req)
		{
			config_data.register_info.b.ep_infoSwitch = 1;
		}
		else
		{
			config_data.register_info.b.ep_infoSwitch = 0;
		}
		if(can_ctrl_msg.brar_ratioSwitch_req)
		{
			config_data.register_info.b.brar_ratioSwitch = 1;
		}
		else
		{
			config_data.register_info.b.brar_ratioSwitch = 0;
		}

		//save to flash
		flash_save_cali_data(config_data.register_info,
				config_data.cali_data,device_info);
		//get from flash
		flash_get_cali_data(&config_data.register_info,
				&config_data.cali_data,&device_info);

		//reply msg
		general_replay_msg(&can0_tx_msg,
				PRS_1_1,CTRL_0_A,0x00,RESULT_OK,0x00);//封装通用应答
		can0_send_msg(can0_tx_msg);	//CAN0发送数据

		can_ctrl_msg.robot_infoSwitch_bo = false;
	}

	if(can_ctrl_msg.robot_open_ctrl_bo_1 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.self_check_st)//转向电机已自检成功
			{
				if(can_ctrl_msg.sr_open_ctrl_req_1 == 1)//请求关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(SR_ID) == SCA_NoError)
					{
						sr_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_FAIL,0x0F);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.sr_open_ctrl_req_1 == 2)//请求开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
					{
						sr_info.robot_st.b.motor_power_st = 1;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_FAIL,0x10);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.sr_open_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				if(can_ctrl_msg.br_open_ctrl_req_1 == 1)//请求关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(BR_ID) == SCA_NoError)
					{
						br_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_FAIL,0x11);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.br_open_ctrl_req_1 == 2)//开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
					{
						br_info.robot_st.b.motor_power_st = 1;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_FAIL,0x12);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.br_open_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				if(can_ctrl_msg.ar_open_ctrl_req_1 == 1)//请求关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(AR_ID) == SCA_NoError)
					{
						ar_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_FAIL,0x13);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_open_ctrl_req_1 == 2)//请求开启
				{
					ar_info.robot_st.b.motor_power_st = 1;
					//开启驱动器
					if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_FAIL,0x14);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ar_open_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				if(can_ctrl_msg.ep_open_ctrl_req_1 == 1)//请求关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(EP_ID) == SCA_NoError)
					{
						ep_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_FAIL,0x15);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ep_open_ctrl_req_1 == 2)//请求开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
					{
						ep_info.robot_st.b.motor_power_st = 1;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_FAIL,0x16);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ep_open_ctrl_req_1 = 0;
			}
		}
		can_ctrl_msg.robot_open_ctrl_bo_1 = false;
	}

	 // 0x112  机器人模式控制
	if(can_ctrl_msg.robot_mode_ctrl_bo_1 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//转向电机已开启
			{
				if(can_ctrl_msg.sr_mode_ctrl_req_1 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_FAIL,0x17);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_1 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_FAIL,0x18);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_1 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_FAIL,0x19);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.sr_mode_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				if(can_ctrl_msg.br_mode_ctrl_req_1 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(BR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_FAIL,0x20);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}

				}
				else if(can_ctrl_msg.br_mode_ctrl_req_1 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(BR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_FAIL,0x21);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.br_mode_ctrl_req_1 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(BR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_FAIL,0x22);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.br_mode_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				if(can_ctrl_msg.ar_mode_ctrl_req_1 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(AR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_FAIL,0x23);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_1 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(AR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_FAIL,0x24);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_1 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(AR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_FAIL,0x25);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ar_mode_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				if(can_ctrl_msg.ep_mode_ctrl_req_1 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(EP_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_FAIL,0x26);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_1 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_FAIL,0x27);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_1 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_FAIL,0x28);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ep_mode_ctrl_req_1 = 0;
			}
		}

		can_ctrl_msg.robot_mode_ctrl_bo_1 = false;
	}

	if(can_ctrl_msg.robot_move_ctrl_bo_1)   //自动驾驶模式
	{
		//自动驾驶状态&&通道1控制
		if((sys_info.sys_st.b.sys_mode == 3||sys_info.sys_st.b.sys_mode == 4)
				&& sys_info.sys_st.b.sys_ctrl_st == 1)
		{
			sys_info.sys_st.b.auto_driving_st = can_ctrl_msg.auto_driving_ctrl_1;
			switch(sys_info.sys_st.b.auto_driving_st)
			{
				case 0: // 自动驾驶预准备阶段， 仅踩下刹车       /* 自动驾驶状态：0-预备 1-启动 2-停止 3-急停 */
				{
					if(can_ctrl_msg.br_request_st_1)//制动请求控制
					{
						if(config_data.cali_data.br_max_angle_valid)
						{
							if(-(float)(can_ctrl_msg.br_angle_1) < config_data.cali_data.br_max_angle)
							{
								SCA_Move_Pos(BR_ID,
								(float)config_data.cali_data.br_max_angle / BRK_INF_RE,
								(float)(4200),
								(float)(10000.00),(float)(-10000.00));  // 3000

							}
							else
							{
								SCA_Move_Pos(BR_ID,
								-(float)(can_ctrl_msg.br_angle_1) / BRK_INF_RE,
								(float)(4200),
								(float)(10000.00),(float)(-10000.00));

							}
						}
						else
						{
							SCA_Move_Pos(BR_ID,
							-(float)(can_ctrl_msg.br_angle_1) / BRK_INF_RE,
							(float)(3000),
							(float)(10000.00),(float)(-10000.00));

						}
					}
					break;
				}
				case 1:	// 自动驾驶阶段
				{
					if(can_ctrl_msg.ep_request_st_1 == 0)
					{
						if(config_data.register_info.b.ep_use)
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(10000.00),(float)(-10000.00));//500-695/600-835

						}

						if(can_ctrl_msg.sr_request_st_1)//转向请求控制
						{
							if(can_ctrl_msg.sr_angle_sign_1 == 0x01)
							{
								if(config_data.cali_data.sr_left_angle_valid)
								{
									if(-can_ctrl_msg.sr_angle_1 < config_data.cali_data.sr_left_angle)
									{
										SCA_Move_Pos(SR_ID,
										(float)config_data.cali_data.sr_left_angle / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_1),
										(float)(3000.00),(float)(-3000.00));
									}
									else
									{
										SCA_Move_Pos(SR_ID,
										-(float)can_ctrl_msg.sr_angle_1 / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_1),
										(float)(3000.00),(float)(-3000.00));
									}
								}
								else
								{
									SCA_Move_Pos(SR_ID,
									-(float)can_ctrl_msg.sr_angle_1 / STE_INF_RE,
									(float)(can_ctrl_msg.sr_speed_1),
									(float)(3000.00),(float)(-3000.00));
								}
							}
							else if(can_ctrl_msg.sr_angle_sign_1 == 0x02)
							{
								if(config_data.cali_data.sr_right_angle_valid)
								{
									if(can_ctrl_msg.sr_angle_1 > config_data.cali_data.sr_right_angle )
									{
										SCA_Move_Pos(SR_ID,
										(float)config_data.cali_data.sr_right_angle / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_1),
										(float)(3000.00),(float)(-3000.00));
									}
									else
									{
										SCA_Move_Pos(SR_ID,
										(float)can_ctrl_msg.sr_angle_1 / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_1),
										(float)(3000.00),(float)(-3000.00));
									}
								}
								else
								{
									SCA_Move_Pos(SR_ID,
									(float)can_ctrl_msg.sr_angle_1 / STE_INF_RE,
									(float)(can_ctrl_msg.sr_speed_1),
									(float)(3000.00),(float)(-3000.00));
								}
							}
						}

						if(can_ctrl_msg.br_request_st_1)//制动请求控制      br_max_angle == 目前最大的角度为-782    br_angle_1为正值
						{
							if(config_data.cali_data.br_max_angle_valid)
							{
								if(-(float)(can_ctrl_msg.br_angle_1) < config_data.cali_data.br_max_angle)  // 不会执行
								{



									SCA_Move_Pos(BR_ID,
									(float)config_data.cali_data.br_max_angle / BRK_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));



									#if   EP_BRAKE_SYMBOL   //EP制动的标志

									config_data.cali_data.ep_max_angle  = config_data.cali_data.br_max_angle;

									SCA_Move_Pos(EP_ID,
									((float)(config_data.cali_data.ep_max_angle)
						             ) / EP_INF_RE,
									(float)(695),
									(float)(3000.00),(float)(-3000.00));//500-695/600-835

									#endif

								}
								else
								{



									SCA_Move_Pos(BR_ID,
									-(float)(can_ctrl_msg.br_angle_1) / BRK_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));



                                    #if   EP_BRAKE_SYMBOL    //双效制动的标准

									ep_info.angle   = can_ctrl_msg.br_angle_1 ;


									SCA_Move_Pos(EP_ID,
									(-(float)(ep_info.angle)
									) / EP_INF_RE,
									(float)(695),
									(float)(3000.00),(float)(-3000.00));//500-695/600-835

                                    #endif

								}
							}
							else
							{




								SCA_Move_Pos(BR_ID,
								-(float)(can_ctrl_msg.br_angle_1) / BRK_INF_RE,
								(float)(500),
								(float)(3000.00),(float)(-3000.00));



                               #if   EP_BRAKE_SYMBOL    //EP制动的标志

								ep_info.angle   = can_ctrl_msg.br_angle_1 ;


								SCA_Move_Pos(EP_ID,
								(-(float)(ep_info.angle)
						        ) / EP_INF_RE,
								(float)(695),                       //695
								(float)(3000.00),(float)(-3000.00));//500-695/600-835

								#endif
							}
						}

						if(can_ctrl_msg.ar_request_st_1)//加速请求控制
						{
							if(config_data.cali_data.ar_max_angle_valid)
							{
								if(can_ctrl_msg.ar_angle_1 > config_data.cali_data.ar_max_angle)
								{
									SCA_Move_Pos(AR_ID,
									(float)config_data.cali_data.ar_max_angle / THR_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));
								}
								else
								{
									SCA_Move_Pos(AR_ID,
									(float)can_ctrl_msg.ar_angle_1 / THR_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));
								}
							}
							else
							{
								SCA_Move_Pos(AR_ID,
								(float)can_ctrl_msg.ar_angle_1 / THR_INF_RE,
								(float)(500),
								(float)(3000.00),(float)(-3000.00));
							}
						}
					}
					else
					{	// 停车
						SCA_Move_Pos(AR_ID,
						(float)0,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));


						if(config_data.cali_data.br_max_angle_valid)
						{
							SCA_Move_Pos(BR_ID,
							((float)(config_data.cali_data.br_max_angle) *
									0.6) / BRK_INF_RE,
							(float)(500),
							(float)(3000.00),(float)(-3000.00));

						}
						else
						{
							SCA_Move_Pos(BR_ID,
							(float)0,
							(float)(500),
							(float)(3000.00),(float)(-3000.00));
						}


                       #if   EP_BRAKE_SYMBOL    //EP制动的标志
						if(config_data.cali_data.ep_max_angle_valid)
						{
							SCA_Move_Pos(EP_ID,
							((float)(config_data.cali_data.ep_max_angle) *
							0.5) / EP_INF_RE,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
						else
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}

                       #endif
					}
					break;
				}
				case 2:	// 自动驾驶完成，停止阶段
				{
					SCA_Move_Pos(AR_ID,
					(float)0,
					(float)(500),
					(float)(3000.00),(float)(-3000.00));

					if(config_data.cali_data.br_max_angle_valid)
					{
						SCA_Move_Pos(BR_ID,
						((float)(config_data.cali_data.br_max_angle) *
								0.6) / BRK_INF_RE,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}
					else
					{
						SCA_Move_Pos(BR_ID,
						(float)0,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}

					if(can_ctrl_msg.ep_request_st_1 == 1)
					{
						if(config_data.cali_data.ep_max_angle_valid)
						{
							SCA_Move_Pos(EP_ID,
							((float)(config_data.cali_data.ep_max_angle) *
							0.5) / EP_INF_RE,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
						else
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
					}
					break;
				}
				case 3:	// 自动驾驶中途急停阶段
				{
					SCA_Move_Pos(AR_ID,
					(float)0,
					(float)(500),
					(float)(3000.00),(float)(-3000.00));

					if(config_data.cali_data.br_max_angle_valid)
					{
						SCA_Move_Pos(BR_ID,
						((float)(config_data.cali_data.br_max_angle) *
								0.6) / BRK_INF_RE,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}
					else
					{
						SCA_Move_Pos(BR_ID,
						(float)0,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}

					if(can_ctrl_msg.ep_request_st_1 == 1)
					{
						if(config_data.cali_data.ep_max_angle_valid)
						{
							SCA_Move_Pos(EP_ID,
							((float)(config_data.cali_data.ep_max_angle) *
							0.5) / EP_INF_RE,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
						else
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
					}
					break;
				}
			}
		}

		if(can_ctrl_msg.sr_request_st_1)//转向请求控制
		{
			sr_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			sr_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.br_request_st_1)//制动请求控制
		{
			br_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			br_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.ar_request_st_1)//加速请求控制
		{
			ar_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			ar_info.robot_st.b.auto_ctrl_st = 0;
		}
		can_ctrl_msg.robot_move_ctrl_bo_1 = false;
	}

	if(can_ctrl_msg.state_detail_query_bo_1)
	{
		if(can_ctrl_msg.state_detail_query_req_1)
		{
			system_status_reply_msg(&can0_tx_msg,PRS_1_2,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
			actuator_status_reply_msg(&can0_tx_msg,PRS_1_5,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
		can_ctrl_msg.state_detail_query_bo_1 = false;
	}

	if(can_ctrl_msg.power_query_bo_1)
	{
		if(can_ctrl_msg.power_sys_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_6,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_sr_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_7,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_br_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_8,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_ar_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_9,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_ep_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_10,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
		can_ctrl_msg.power_query_bo_1 = false;
	}

	// 0x116指令进入/退出自动驾驶
	if(can_ctrl_msg.ctrl_auto_driver_control_bo_1)
	{
		if(can_ctrl_msg.auto_driver_ctrl_1)    //	/* CTRL控制标志：0-人工模式 1-自动驾驶 */
		{
			//手动驾驶状态&&自检结束&&通道1控制
			if(sys_info.sys_st.b.sys_mode == 2 &&
					sys_info.sys_st.b.self_check_st == 1 &&
					sys_info.sys_st.b.sys_ctrl_st == 1)
			{
				sr_info.robot_st.b.change_mode_st = 0;
				br_info.robot_st.b.change_mode_st = 0;
				ar_info.robot_st.b.change_mode_st = 0;
				ep_info.robot_st.b.change_mode_st = 0;
				sys_info.sys_st.b.auto_driving_st = 0;	// 此状态标志由按键、或0x113指令改变
				sys_info.sys_st.b.sys_mode = 3;
				sys_info.sys_st.b.take_over_st = 0;
				SW_LED = 1;//亮灯
			}
		}
		else
		{
			sr_info.robot_st.b.change_mode_st = 1;
			br_info.robot_st.b.change_mode_st = 1;
			br_info.limit_time_count = 0;
			ar_info.robot_st.b.change_mode_st = 1;
			ar_info.limit_time_count = 0;
			ep_info.robot_st.b.change_mode_st = 1;
			ep_info.limit_time_count = 0;
			sys_info.sys_st.b.auto_driving_st = 0;
			sys_info.sys_st.b.sys_mode = 2;
			sys_info.sys_st.b.take_over_st = 1;
			SW_LED = 0;//灭灯
		}
		can_ctrl_msg.ctrl_auto_driver_control_bo_1 = false;
	}

#if 0
	if(can_ctrl_msg.robot_open_ctrl_bo_2 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//转向电机已开启
			{
				if(can_ctrl_msg.sr_open_ctrl_req_2 == 1)//关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(SR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_FAIL,0x0F);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.sr_open_ctrl_req_2 == 2)//开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_FAIL,0x10);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.sr_open_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				if(can_ctrl_msg.br_open_ctrl_req_2 == 1)//关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(BR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_FAIL,0x11);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.br_open_ctrl_req_2 == 2)//开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_FAIL,0x12);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.br_open_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				if(can_ctrl_msg.ar_open_ctrl_req_2 == 1)//关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(AR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_FAIL,0x13);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_open_ctrl_req_2 == 2)//开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_FAIL,0x14);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ar_open_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				if(can_ctrl_msg.ep_open_ctrl_req_2 == 1)//关闭
				{
					//关闭驱动器
					if(SCA_Close_Actuator(EP_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_FAIL,0x15);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_open_ctrl_req_1 == 2)//开启
				{
					//开启驱动器
					if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_FAIL,0x16);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ep_open_ctrl_req_2 = 0;
			}
		}

		can_ctrl_msg.robot_open_ctrl_bo_2 = false;
	}

	if(can_ctrl_msg.robot_mode_ctrl_bo_2 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//转向电机已开启
			{
				if(can_ctrl_msg.sr_mode_ctrl_req_2 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_FAIL,0x17);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_2 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_FAIL,0x18);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_2 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_FAIL,0x19);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.sr_mode_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				if(can_ctrl_msg.br_mode_ctrl_req_2 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(BR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_FAIL,0x20);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}

				}
				else if(can_ctrl_msg.br_mode_ctrl_req_2 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(BR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_FAIL,0x21);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.br_mode_ctrl_req_2 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(BR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_FAIL,0x22);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				//清零操作
				can_ctrl_msg.br_mode_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				if(can_ctrl_msg.ar_mode_ctrl_req_2 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(AR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_FAIL,0x23);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_2 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_FAIL,0x24);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_2 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_FAIL,0x25);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ar_mode_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				if(can_ctrl_msg.ep_mode_ctrl_req_2 == 1)//电流模式
				{
					if(SCA_Mode_Actuator(EP_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_FAIL,0x26);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_1 == 2)//位置模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_FAIL,0x27);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_2 == 3)//速度模式
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_OK,0x00);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_FAIL,0x28);//封装通用应答
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
					}
				}

				//清零操作
				can_ctrl_msg.ep_mode_ctrl_req_2 = 0;
			}
		}

		can_ctrl_msg.robot_mode_ctrl_bo_2 = false;
	}

	if(can_ctrl_msg.robot_move_ctrl_bo_2)
	{
		//自动驾驶状态&&自检结束&&通道2控制
		if((sys_info.sys_st.b.sys_mode == 3||sys_info.sys_st.b.sys_mode == 4) &&
				sys_info.sys_st.b.self_check_st == 1 &&
				sys_info.sys_st.b.sys_ctrl_st == 2)
		{
			sys_info.sys_st.b.auto_driving_st =
					can_ctrl_msg.auto_driving_ctrl_2;
			switch(sys_info.sys_st.b.auto_driving_st)
			{
				case 1:
				{
					if(can_ctrl_msg.ep_request_st_2 == 0)
					{
						if(config_data.register_info.b.ep_use)
						{
							SCA_Move_Pos(EP_ID,
								(float)0,
								(float)(695),
								(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}

						if(can_ctrl_msg.sr_request_st_2)//转向请求控制
						{
							if(can_ctrl_msg.sr_angle_sign_2 == 0x01)
							{
								if(config_data.cali_data.sr_left_angle_valid)
								{
									if(-can_ctrl_msg.sr_angle_2 < config_data.cali_data.sr_left_angle)
									{
										SCA_Move_Pos(SR_ID,
										(float)config_data.cali_data.sr_left_angle / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_2),
										(float)(3000.00),(float)(-3000.00));
									}
									else
									{
										SCA_Move_Pos(SR_ID,
										-(float)can_ctrl_msg.sr_angle_2 / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_2),
										(float)(3000.00),(float)(-3000.00));
									}
								}
								else
								{
									SCA_Move_Pos(SR_ID,
									-(float)can_ctrl_msg.sr_angle_2 / STE_INF_RE,
									(float)(can_ctrl_msg.sr_speed_2),
									(float)(3000.00),(float)(-3000.00));
								}
							}
							else if(can_ctrl_msg.sr_angle_sign_2 == 0x02)
							{
								if(config_data.cali_data.sr_right_angle_valid)
								{
									if(can_ctrl_msg.sr_angle_2 > config_data.cali_data.sr_right_angle )
									{
										SCA_Move_Pos(SR_ID,
										(float)config_data.cali_data.sr_right_angle / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_2),
										(float)(3000.00),(float)(-3000.00));
									}
									else
									{
										SCA_Move_Pos(SR_ID,
										(float)can_ctrl_msg.sr_angle_2 / STE_INF_RE,
										(float)(can_ctrl_msg.sr_speed_2),
										(float)(3000.00),(float)(-3000.00));
									}
								}
								else
								{
									SCA_Move_Pos(SR_ID,
									(float)can_ctrl_msg.sr_angle_2 / STE_INF_RE,
									(float)(can_ctrl_msg.sr_speed_2),
									(float)(3000.00),(float)(-3000.00));
								}
							}
						}

						if(can_ctrl_msg.br_request_st_2)//制动请求控制
						{
							if(config_data.cali_data.br_max_angle_valid)
							{
								if(-(float)(can_ctrl_msg.br_angle_2) < config_data.cali_data.br_max_angle)
								{
									SCA_Move_Pos(BR_ID,
									(float)(config_data.cali_data.br_max_angle) / BRK_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));
								}
								else
								{
									SCA_Move_Pos(BR_ID,
									-(float)(can_ctrl_msg.br_angle_2) / BRK_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));
								}
							}
							else
							{
								SCA_Move_Pos(BR_ID,
								-(float)(can_ctrl_msg.br_angle_2) / BRK_INF_RE,
								(float)(500),
								(float)(3000.00),(float)(-3000.00));
							}
						}

						if(can_ctrl_msg.ar_request_st_2)//加速请求控制
						{
							if(config_data.cali_data.ar_max_angle_valid)
							{
								if(can_ctrl_msg.ar_angle_2 > config_data.cali_data.ar_max_angle)
								{
									SCA_Move_Pos(AR_ID,
									(float)config_data.cali_data.ar_max_angle / THR_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));
								}
								else
								{
									SCA_Move_Pos(AR_ID,
									(float)can_ctrl_msg.ar_angle_2 / THR_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));
								}
							}
							else
							{
								SCA_Move_Pos(AR_ID,
								(float)can_ctrl_msg.ar_angle_2 / THR_INF_RE,
								(float)(500),
								(float)(3000.00),(float)(-3000.00));
							}
						}
					}
					else
					{
						if(config_data.cali_data.br_max_angle_valid)
						{
							SCA_Move_Pos(BR_ID,
							((float)(config_data.cali_data.br_max_angle) *
									0.6) / BRK_INF_RE,
							(float)(500),
							(float)(3000.00),(float)(-3000.00));
						}
						else
						{
							SCA_Move_Pos(BR_ID,
							(float)0,
							(float)(500),
							(float)(3000.00),(float)(-3000.00));
						}

						if(config_data.cali_data.ep_max_angle_valid)
						{
							SCA_Move_Pos(EP_ID,
							((float)(config_data.cali_data.ep_max_angle) *
							0.5) / EP_INF_RE,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
						else
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}

						SCA_Move_Pos(AR_ID,
						(float)0,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}
					break;
				}
				case 2:
				{
					if(config_data.cali_data.br_max_angle_valid)
					{
						SCA_Move_Pos(BR_ID,
						((float)(config_data.cali_data.br_max_angle) *
								0.6) / BRK_INF_RE,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}
					else
					{
						SCA_Move_Pos(BR_ID,
						(float)0,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));
					}

					if(can_ctrl_msg.ep_request_st_2 == 1)
					{
						if(config_data.cali_data.ep_max_angle_valid)
						{
							SCA_Move_Pos(EP_ID,
							((float)(config_data.cali_data.ep_max_angle) *
							0.5) / EP_INF_RE,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
						else
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
					}

					SCA_Move_Pos(AR_ID,
					(float)0,
					(float)(500),
					(float)(3000.00),(float)(-3000.00));
					break;
				}
				case 3:
				{
					if(config_data.cali_data.br_max_angle_valid)
					{
						SCA_Move_Pos(BR_ID,
						((float)(config_data.cali_data.br_max_angle) *
								0.6) / BRK_INF_RE,
						(float)(600),
						(float)(3000.00),(float)(-3000.00));
					}
					else
					{
						SCA_Move_Pos(BR_ID,
						(float)0,
						(float)(600),
						(float)(3000.00),(float)(-3000.00));
					}

					if(can_ctrl_msg.ep_request_st_2 == 1)
					{
						if(config_data.cali_data.ep_max_angle_valid)
						{
							SCA_Move_Pos(EP_ID,
							((float)(config_data.cali_data.ep_max_angle) *
							0.5) / EP_INF_RE,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
						else
						{
							SCA_Move_Pos(EP_ID,
							(float)0,
							(float)(695),
							(float)(3000.00),(float)(-3000.00));//500-695/600-835
						}
					}

					SCA_Move_Pos(AR_ID,
					(float)0,
					(float)(500),
					(float)(3000.00),(float)(-3000.00));
					break;
				}
			}
		}

		if(can_ctrl_msg.sr_request_st_2)//转向请求控制
		{
			sr_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			sr_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.br_request_st_2)//制动请求控制
		{
			br_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			br_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.ar_request_st_2)//加速请求控制
		{
			ar_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			ar_info.robot_st.b.auto_ctrl_st = 0;
		}
		can_ctrl_msg.robot_move_ctrl_bo_2 = false;
	}

	if(can_ctrl_msg.state_detail_query_bo_2)
	{
		if(can_ctrl_msg.state_detail_query_req_2)
		{
			system_status_reply_msg(&can0_tx_msg,PRS_1_2,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
			actuator_status_reply_msg(&can0_tx_msg,PRS_1_5,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
		can_ctrl_msg.state_detail_query_bo_2 = false;
	}

	if(can_ctrl_msg.power_query_bo_2)
	{
		if(can_ctrl_msg.power_sys_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_6,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_sr_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_7,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_br_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_8,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_ar_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_9,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if(can_ctrl_msg.power_ep_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_10,s_data);//封装应答消息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
		can_ctrl_msg.power_query_bo_2 = false;
	}

	if(can_ctrl_msg.ctrl_auto_driver_control_bo_2)
	{
		if(can_ctrl_msg.auto_driver_ctrl_2)
		{
			//手动驾驶状态&&自检结束&&通道2控制
			if(sys_info.sys_st.b.sys_mode == 2 &&
					sys_info.sys_st.b.self_check_st == 1 &&
					sys_info.sys_st.b.sys_ctrl_st == 2)
			{

				sr_info.robot_st.b.change_mode_st = 0;
				br_info.robot_st.b.change_mode_st = 0;
				ar_info.robot_st.b.change_mode_st = 0;
				ep_info.robot_st.b.change_mode_st = 0;
				sys_info.sys_st.b.auto_driving_st = 0;
				sys_info.sys_st.b.sys_mode = 3;
				sys_info.sys_st.b.take_over_st = 0;
				SW_LED = 1;//亮灯
			}
		}
		else
		{
			sr_info.robot_st.b.change_mode_st = 1;
			br_info.robot_st.b.change_mode_st = 1;
			br_info.limit_time_count = 0;
			ar_info.robot_st.b.change_mode_st = 1;
			ar_info.limit_time_count = 0;
			ep_info.robot_st.b.change_mode_st = 1;
			ep_info.limit_time_count = 0;
			sys_info.sys_st.b.auto_driving_st = 0;
			sys_info.sys_st.b.sys_mode = 2;
			sys_info.sys_st.b.take_over_st = 1;
			SW_LED = 0;//灭灯
		}
		can_ctrl_msg.ctrl_auto_driver_control_bo_2 = false;
	}

	if(can_ctrl_msg.ctrl_route_set_bo)
	{
		switch(can_ctrl_msg.ctrl_flag)
		{
			case 0:
			{
				sys_info.sys_st.b.sys_ctrl_st = 1;
				break;
			}
			case 1:
			{
				sys_info.sys_st.b.sys_ctrl_st = 2;
				break;
			}
			case 2:
			{
				sys_info.sys_st.b.sys_ctrl_st = 0;
				break;
			}
		}
		can_ctrl_msg.ctrl_route_set_bo = false;
	}
#endif
}

void check_in_power(void)
{
	//判定外部供电状态
	if(s_data->pwr_msg.voltage_in > VOLTAGE_MIN)
	{
		check_info.in_power_state = 1;      //   0-异常  1-正常
		sys_info.sys_fault.b.power = 0;     //  0-正常 1-异常
	}
	else
	{
		check_info.in_power_state = 0;
		sys_info.sys_fault.b.power = 1;
	}
}

void check_sensor(void)
{

	check_info.encoder_state = 1;    //0-异常  1-正常
	sys_info.sys_fault.b.encoder = 0;
//	if(s_data->ep_info.encoder_value > ENCODER_MIN ||
//			s_data->ep_info.encoder_value < ENCODER_MAX)
//	{
//		check_info.encoder_state = 1;    //0-异常  1-正常
//		sys_info.sys_fault.b.encoder = 0;
//	}
//	else
//	{
//		check_info.encoder_state = 0;
//		sys_info.sys_fault.b.encoder = 1;
//	}

}

void check_hmi(void)
{
	check_info.hmi_state = 1;
		sys_info.sys_fault.b.hmi = 0;
	// 不再使用hmi显示屏，保持状态正常
//	if(config_data.register_info.b.hmi_use)
//	{
//		check_info.hmi_state = 1;
//		sys_info.sys_fault.b.hmi = 0;
// 	}
}

void check_motor_power(void)
{
	if(s_data->pwr_msg.voltage_in > 50)
	{
		check_info.last_motor_power_state =
								check_info.motor_power_state;
		check_info.motor_power_state = 0;          /* 电机供电状态：0-掉电  1-正常 */
		sys_info.sys_st.b.estop_st = 1;            /* 急停状态：0-无急停  1-ESTOP激活 2-EBR激活 */
		sys_info.sys_st.b.auto_driving_st = 0;     /* 自动驾驶状态：0-预备 1-启动 2-停止 3-急停 */
		sys_info.sys_st.b.sys_mode = 2;            /* 系统模式：1-自检 2-手动 3-自动 4-安全 5-故障 */
		sys_info.sys_st.b.take_over_st = 1;       	/* 接管状态：0-未接管 1-已接管 */
		SW_LED = 0;//灭灯
	}
	else
	{
		if(sr_info.voltage > VOLTAGE_MIN && sr_info.voltage < 45)
		{

			sr_info.robot_st.b.power_st = 0;  	/* 供电状态：0-正常 1-未供电 2-异常 3-保留 */
		}
		else
		{
			sr_info.robot_st.b.power_st = 1;
		}

		if(br_info.voltage > VOLTAGE_MIN && br_info.voltage < 45)
		{
			br_info.robot_st.b.power_st = 0;
		}
		else
		{
			br_info.robot_st.b.power_st = 1;
		}


		if(ar_info.voltage > VOLTAGE_MIN && ar_info.voltage < 45)
		{
			ar_info.robot_st.b.power_st = 0;
		}
		else
		{
			ar_info.robot_st.b.power_st = 1;
		}


		if(ep_info.voltage > VOLTAGE_MIN && ep_info.voltage < 45)
		{
			ep_info.robot_st.b.power_st = 0;
		}
		else
		{
			ep_info.robot_st.b.power_st = 1;
		}

		check_info.last_motor_power_state =
								check_info.motor_power_state;

		check_info.motor_power_state = 1;
		sys_info.sys_st.b.estop_st = 0;
	}
}

void check_self(void)
{
	float real_temp=0;

	//检查外部供电状态
	check_in_power();

	//检查磁编码器状态，不在使用编码器，默认参数直接正常
	check_sensor();

	//检查hmi状态 , 不在使用hmi，默认参数正常
	check_hmi();

	//外部供电正常时，判定estop按钮是否按下
	if(check_info.in_power_state)
	{
		//检查电机供电状态
		check_motor_power();

		//供电状态由断电改为上电时，重新进行自检操作
		if(check_info.last_motor_power_state == 0 &&
				check_info.motor_power_state == 1)
		{
			//重新进行自检操作
			check_info.check_state = 0;
			check_info.check_step = 0;
			check_info.ep_state = 0;
			check_info.ar_state = 0;
			check_info.br_state = 0;
			check_info.sr_state = 0;
			check_info.hmi_state = 1;
			check_info.encoder_state = 0;
			check_info.retry_count = 0;
			check_info.time_count = 0;
			check_info.failure_info = 0;
		}
	}
	//自检操作
	if(check_info.check_state == 0)
	{
		//进入自检模式
		sys_info.sys_st.b.sys_mode = 1;

		if(check_info.in_power_state)
		{
			if(check_info.motor_power_state)
			{
				//EP执行器自检过程


				if(check_info.ep_state == 0 &&
						config_data.register_info.b.ep_use)
				{
					if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
					{
						ep_info.robot_st.b.motor_power_st = 1;   ///* 开机状态：0-关机  1-开机 */
						check_info.ep_state = 2;    //0-掉电  1-正常  >1-自检失败
						check_info.time_count = 0;
						check_info.retry_count = 0;
						check_info.failure_info = 0x00;

					}
					else
					{
						check_info.retry_count++;
					}
					if(check_info.retry_count > 9)
					{
						ep_info.robot_st.b.self_check_st = 0;
						ep_info.robot_st.b.motor_power_st = 0;
						check_info.retry_count = 0;
						check_info.time_count = 0;
						check_info.ep_state = 0x7f;
						check_info.failure_info = 0x01;
					}
				}
				else if(check_info.ep_state == 2 &&
						config_data.register_info.b.ep_use)
				{

					{
						if(SCA_Move_Profile_Vel(EP_ID, (float)10, (float)100,
								(float)100) == SCA_NoError)
						{
							check_info.ep_state = 3;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.failure_info = 0x00;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ep_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ep_state = 0x7e;
							check_info.failure_info = 0x02;
						}
					}
				}
				else if(check_info.ep_state == 3 &&
						config_data.register_info.b.ep_use)
				{
					PSCA = SCA_Handler_Update(EP_ID);
					real_temp = PSCA->Current_Real;
					if (real_temp > 6)
					{
						if(SCA_Mode_Actuator(EP_ID,0x01) == SCA_NoError)
						{
							check_info.ep_state = 4;
							check_info.time_count = 0;
							check_info.retry_count = 0;
							check_info.failure_info = 0x00;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ep_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ep_state = 0x7d;
							check_info.failure_info = 0x04;
						}
					}
					else
					{
						check_info.time_count++;
						if(check_info.time_count > 80)
						{
							ep_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ep_state = 0x7c;
							check_info.failure_info = 0x08;
						}
					}
				}
				else if(check_info.ep_state == 4 &&
						config_data.register_info.b.ep_use)
				{
					if(config_data.cali_data.ep_zero_valid == 0)   //去掉EP电机上电自动标零，修改为上电检测零点有效性
					 {
						check_info.ep_state =  5 ;
						check_info.time_count = 0;
						check_info.retry_count = 0;
					 }
					 else
					 {
						check_info.retry_count++;
					 }
					 if(check_info.retry_count > 9)
					 {
						ep_info.robot_st.b.self_check_st = 0;
						check_info.retry_count = 0;
						check_info.time_count = 0;
						check_info.ep_state = 0x80;
						check_info.failure_info = 0x40;
					 }
				}
				else if(check_info.ep_state == 5 &&
						config_data.register_info.b.ep_use)
				{
					if(SCA_Move_Pos(EP_ID, (float)(-10/EP_INF_RE), (float)100,
							(float)100,(float)-100) == SCA_NoError)
					{
						check_info.ep_state = 6;
						check_info.time_count = 0;
						check_info.retry_count = 0;
						check_info.failure_info = 0x00;
					}
					else
					{
						check_info.retry_count++;
					}
					if(check_info.retry_count > 9)
					{
						ep_info.robot_st.b.self_check_st = 0;
						check_info.retry_count = 0;
						check_info.time_count = 0;
						check_info.ep_state = 0x7a;
						check_info.failure_info = 0x20;
					}
				}
				else if(check_info.ep_state == 6 &&
						config_data.register_info.b.ep_use)
				{
					if(SCA_Mode_Actuator(EP_ID,0x01) == SCA_NoError) /*切换为电流模式*/
					{
						config_data.cali_data.ep_encoder_up =
								ep_info.encoder_value;
						check_info.ep_state = 7;
						check_info.time_count = 0;
						check_info.retry_count = 0;
						check_info.failure_info = 0x00;
					}
					else
					{
						check_info.retry_count++;
					}
					if(check_info.retry_count > 9)
					{
						ep_info.robot_st.b.self_check_st = 0;
						check_info.retry_count = 0;
						check_info.time_count = 0;
						check_info.ep_state = 0x78;
						check_info.failure_info = 0x80;
					}
				}
				else if(check_info.ep_state == 7 &&
						config_data.register_info.b.ep_use)
				{
					PSCA = SCA_Handler_Update(EP_ID);
					if(SCA_Pos_Max_Limit(EP_ID,PSCA->Position_Real) == SCA_NoError)
					{
						config_data.cali_data.ep_min_angle =
								(int16_t)(PSCA->Position_Real*EP_INF_RE);
						check_info.ep_state = 8;
						check_info.time_count = 0;
						check_info.retry_count = 0;
						check_info.failure_info = 0x00;
					}
					else
					{
						check_info.retry_count++;
					}
					if(check_info.retry_count > 9)
					{
						ep_info.robot_st.b.self_check_st = 0;
						check_info.retry_count = 0;
						check_info.time_count = 0;
						check_info.ep_state = 0x79;
						check_info.failure_info = 0x40;
					}
				}
				else if(check_info.ep_state == 8 &&
						config_data.register_info.b.ep_use)
				{
					if(config_data.cali_data.ep_max_angle_valid)
					{
						if(SCA_Pos_Min_Limit(EP_ID,
						   (float)(config_data.cali_data.ep_max_angle
						    /EP_INF_RE))== SCA_NoError)
						{
							ep_info.robot_st.b.self_check_st = 1;   //EP 执行器 自检成功
							check_info.ep_state = 1;
							check_info.time_count = 0;
							check_info.retry_count = 0;
							check_info.failure_info = 0x00;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ep_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ep_state = 0x78;
							check_info.failure_info = 0x80;
						}
					}
					else
					{
						ep_info.robot_st.b.self_check_st = 1;    // EP
						check_info.ep_state = 1;   //自检完成
					}
				}
				else
				{
					if(check_info.ar_state == 0 &&
							config_data.register_info.b.ar_use)
					{
						if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
						{
							ar_info.robot_st.b.motor_power_st = 1;
							check_info.ar_state = 2;
							check_info.time_count = 0;
							check_info.retry_count = 0;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ar_info.robot_st.b.self_check_st = 0;
							ar_info.robot_st.b.motor_power_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ar_state = 0x7f;
							check_info.failure_info |= 0x0100;
						}
					}
					else if(check_info.ar_state == 2 &&
							config_data.register_info.b.ar_use)
					{
						if(SCA_Move_Profile_Vel(AR_ID, (float)3, (float)-50,
								(float)50) == SCA_NoError)
						{
							check_info.ar_state = 3;
							check_info.time_count = 0;
							check_info.retry_count = 0;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ar_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ar_state = 0x7e;
							check_info.failure_info |= 0x0200;
						}
					}
					else if(check_info.ar_state == 3 &&
							config_data.register_info.b.ar_use)
					{
						PSCA = SCA_Handler_Update(AR_ID);
						real_temp = PSCA->Current_Real;
						if (real_temp < -2)
						{
							if(SCA_Mode_Actuator(AR_ID,0x01) == SCA_NoError)
							{
								check_info.ar_state = 4;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								ar_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.ar_state = 0x7d;
								check_info.failure_info |= 0x0400;
							}
						}
						else
						{
							check_info.time_count++;
							if(check_info.time_count > 80)
							{
								ar_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.ar_state = 0x7c;
								check_info.failure_info |= 0x0800;
							}
						}
					}
					else if(check_info.ar_state == 4 &&
							config_data.register_info.b.ar_use)
					{
						SCA_Set_Up_Home(AR_ID);
						SCA_Set_Up_Home(AR_ID);
						if(SCA_Set_Up_Home(AR_ID) == SCA_NoError)
						{
							check_info.ar_state = 5;
							check_info.time_count = 0;
							check_info.retry_count = 0;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ar_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ar_state = 0x7b;
							check_info.failure_info |= 0x1000;
						}
					}
					else if(check_info.ar_state == 5 &&
							config_data.register_info.b.ar_use)
					{
						if(SCA_Move_Pos(AR_ID, (float)(10/THR_INF_RE), (float)50,
								(float)50, (float)-50) == SCA_NoError)
						{
							check_info.ar_state = 6;
							check_info.time_count = 0;
							check_info.retry_count = 0;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ar_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ar_state = 0x7a;
							check_info.failure_info |= 0x2000;
						}
					}
					else if(check_info.ar_state == 6 &&
							config_data.register_info.b.ar_use)
					{
						if(SCA_Mode_Actuator(AR_ID,0x01) == SCA_NoError)
						{
							check_info.ar_state = 7;
							check_info.time_count = 0;
							check_info.retry_count = 0;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ar_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ar_state = 0x78;
							check_info.failure_info |= 0x8000;
						}
					}
					else if(check_info.ar_state == 7 &&
							config_data.register_info.b.ar_use)
					{
						PSCA = SCA_Handler_Update(AR_ID);
						if(SCA_Pos_Min_Limit(AR_ID,PSCA->Position_Real) == SCA_NoError)
						{
							config_data.cali_data.ar_min_angle =
									(int16_t)(PSCA->Position_Real*THR_INF_RE);
							check_info.ar_state = 8;
							check_info.time_count = 0;
							check_info.retry_count = 0;
						}
						else
						{
							check_info.retry_count++;
						}
						if(check_info.retry_count > 9)
						{
							ar_info.robot_st.b.self_check_st = 0;
							check_info.retry_count = 0;
							check_info.time_count = 0;
							check_info.ar_state = 0x79;
							check_info.failure_info |= 0x4000;
						}
					}
					else if(check_info.ar_state == 8 &&
							config_data.register_info.b.ar_use)
					{
						if(config_data.cali_data.ar_max_angle_valid)
						{
							if(SCA_Pos_Max_Limit(AR_ID,
							  (float)(config_data.cali_data.ar_max_angle
							   /THR_INF_RE))== SCA_NoError)
							{
								ar_info.robot_st.b.self_check_st = 1;
								check_info.ar_state = 1;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								ar_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.ar_state = 0x79;
								check_info.failure_info |= 0x4000;
							}
						}
						else
						{
							ar_info.robot_st.b.self_check_st = 1;
							check_info.ar_state = 1;
						}
					}
					else
					{

						// br 开启自检
						if(check_info.br_state == 0 &&
								config_data.register_info.b.br_use)
						{
							if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
							{
								br_info.robot_st.b.motor_power_st = 1;
								check_info.br_state = 2;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								br_info.robot_st.b.self_check_st = 0;
								br_info.robot_st.b.motor_power_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.br_state = 0x7f;
								check_info.failure_info |= 0x10000;
							}
						}
						else if(check_info.br_state == 2 &&
								config_data.register_info.b.br_use)
						{
							if(SCA_Move_Profile_Vel(BR_ID, (float)3, (float)50,
									(float)50) == SCA_NoError)
							{
								check_info.br_state = 3;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								br_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.br_state = 0x7e;
								check_info.failure_info |= 0x20000;
							}
						}
						else if(check_info.br_state == 3 &&
								config_data.register_info.b.br_use)
						{
							PSCA = SCA_Handler_Update(BR_ID);
							real_temp = PSCA->Current_Real;
							if (real_temp > 2)
							{
								if(SCA_Mode_Actuator(BR_ID,0x01) == SCA_NoError)
								{
									check_info.br_state = 4;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
								else
								{
									check_info.retry_count++;
								}
								if(check_info.retry_count > 9)
								{
									br_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.br_state = 0x7d;
									check_info.failure_info |= 0x40000;
								}
							}
							else
							{
								check_info.time_count++;
								if(check_info.time_count > 450)
								{
									br_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.br_state = 0x7c;
									check_info.failure_info |= 0x80000;
								}
							}
						}
						else if(check_info.br_state == 4 &&
								config_data.register_info.b.br_use)    //去掉BR电机上电自动标零，修改为上电检测零点有效性
						{
							if(config_data.cali_data.br_zero_valid == 0)
							{
								check_info.br_state = 5;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								ar_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.br_state = 0x7a;
								check_info.failure_info |= 0x2000;
							}
						}
						else if(check_info.br_state == 5 &&
								config_data.register_info.b.br_use)
						{
							if(SCA_Move_Pos(BR_ID, (float)(-10/BRK_INF_RE),
								(float)200, (float)200, (float)-200) == SCA_NoError)
							{
								check_info.br_state = 6;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								br_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.br_state = 0x7a;
								check_info.failure_info |= 0x200000;
							}
						}
						else if(check_info.br_state == 6 &&
								config_data.register_info.b.br_use)
						{
							PSCA = SCA_Handler_Update(BR_ID);
							real_temp = PSCA->Position_Real*BRK_INF_RE;
							if (ABS(real_temp) <= 15)
							{
								if(SCA_Mode_Actuator(BR_ID,0x01) == SCA_NoError)
								{
									check_info.br_state = 7;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
								else
								{
									check_info.retry_count++;
								}
								if(check_info.retry_count > 9)
								{
									br_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.br_state = 0x79;
									check_info.failure_info |= 0x400000;
								}
							}
							else
							{
								check_info.time_count++;
								if(check_info.time_count > 600)
								{
									br_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.br_state = 0x78;
									check_info.failure_info |= 0x800000;
								}
							}
						}
						else if(check_info.br_state == 7 &&
								config_data.register_info.b.br_use)
						{
							PSCA = SCA_Handler_Update(BR_ID);
							if(SCA_Pos_Max_Limit(BR_ID,PSCA->Position_Real)
									== SCA_NoError)
							{
								config_data.cali_data.br_min_angle =
										(int32_t)(real_temp);
								check_info.br_state = 8;
								check_info.time_count = 0;
								check_info.retry_count = 0;
							}
							else
							{
								check_info.retry_count++;
							}
							if(check_info.retry_count > 9)
							{
								br_info.robot_st.b.self_check_st = 0;
								check_info.retry_count = 0;
								check_info.time_count = 0;
								check_info.br_state = 0x77;
								check_info.failure_info |= 0x1000000;
							}
						}
						else if(check_info.br_state == 8 &&
							config_data.register_info.b.br_use)
						{
							if(config_data.cali_data.br_max_angle_valid)
							{
								if(SCA_Pos_Min_Limit(BR_ID,(float)
								  (config_data.cali_data.br_max_angle
								  /BRK_INF_RE))== SCA_NoError)
								{
									br_info.robot_st.b.self_check_st = 1;
									check_info.br_state = 1;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
								else
								{
									check_info.retry_count++;
								}
								if(check_info.retry_count > 9)
								{
									br_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.br_state = 0x76;
									check_info.failure_info |= 0x2000000;
								}
							}
							else
							{

								br_info.robot_st.b.self_check_st = 1;
								check_info.br_state = 1;
							}
						}
						else
						{
							//SR 开始自检
							if(check_info.sr_state == 0 &&
									config_data.register_info.b.sr_use)
							{
								if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
								{
									check_info.sr_state = 2;
									sr_info.robot_st.b.motor_power_st = 1;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
								else
								{
									check_info.retry_count++;
								}
								if(check_info.retry_count > 9)
								{
									sr_info.robot_st.b.self_check_st = 0;
									sr_info.robot_st.b.motor_power_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.sr_state = 0x7f;
									check_info.failure_info |= 0x400000;
								}
							}
							else if(check_info.sr_state == 2 &&
									config_data.register_info.b.sr_use)
							{
								SCA_Set_Up_Home(SR_ID);
								SCA_Set_Up_Home(SR_ID);
								if(SCA_Set_Up_Home(SR_ID) == SCA_NoError)
								{
									check_info.sr_state = 3;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
								else
								{
									check_info.retry_count++;
								}
								if(check_info.retry_count > 9)
								{
									sr_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.sr_state = 0x7e;
									check_info.failure_info |= 0x800000;
								}
							}
							else if(check_info.sr_state == 3 &&
									config_data.register_info.b.sr_use)
							{
								if(config_data.cali_data.sr_left_angle_valid)
								{
									if(SCA_Pos_Min_Limit(SR_ID,
									(float)(config_data.cali_data.sr_left_angle
												/STE_INF_RE)) == SCA_NoError)
									{
										check_info.sr_state = 4;
										check_info.time_count = 0;
										check_info.retry_count = 0;
									}
									else
									{
										check_info.retry_count++;
									}
									if(check_info.retry_count > 9)
									{
										sr_info.robot_st.b.self_check_st = 0;
										check_info.retry_count = 0;
										check_info.time_count = 0;
										check_info.sr_state = 0x7d;
										check_info.failure_info |= 0x01000000;
									}
								}
								else
								{
									check_info.sr_state = 4;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
							}
							else if(check_info.sr_state == 4 &&
									config_data.register_info.b.sr_use)
							{
								if(config_data.cali_data.sr_right_angle_valid)
								{
									if(SCA_Pos_Max_Limit(SR_ID,
									(float)(config_data.cali_data.sr_right_angle
												/STE_INF_RE)) == SCA_NoError)
									{
										check_info.sr_state = 5;
										check_info.time_count = 0;
										check_info.retry_count = 0;
									}
									else
									{
										check_info.retry_count++;
									}
									if(check_info.retry_count > 9)
									{
										sr_info.robot_st.b.self_check_st = 0;
										check_info.retry_count = 0;
										check_info.time_count = 0;
										check_info.sr_state = 0x7c;
										check_info.failure_info |= 0x02000000;
									}
								}
								else
								{
									check_info.sr_state = 5;
									check_info.time_count = 0;
									check_info.retry_count = 0;
								}
							}
							else if(check_info.sr_state == 5 &&
									config_data.register_info.b.sr_use)
							{
								if(SCA_Mode_Actuator(SR_ID,0x01) == SCA_NoError)
								{
									sr_info.robot_st.b.self_check_st = 1;
									check_info.sr_state = 1;
									check_info.time_count = 0;
									check_info.retry_count = 0;

								}
								else
								{
									check_info.retry_count++;
								}
								if(check_info.retry_count > 9)
								{
									sr_info.robot_st.b.self_check_st = 0;
									check_info.retry_count = 0;
									check_info.time_count = 0;
									check_info.sr_state = 0x7b;
									check_info.failure_info |= 0x04000000;
								}
							}
							else
							{

								//自检结束
								check_info.check_state = 1;
								sys_info.sys_st.b.self_check_st = 1;

								//进入手动模式
								sys_info.sys_st.b.sys_mode = 2;
							}
						}  //SR自检结束
					} // BR
				} //AR
			}
			else   // 供电正常else 不会执行
			{

				//estop拍下异常
				check_info.failure_info |= 0x410101;

				//自检结束
				check_info.check_state = 1;
				sys_info.sys_st.b.self_check_st = 1;

				//进入手动模式
				sys_info.sys_st.b.sys_mode = 2;
			}
		} // 供电正常else 不会执行
		else
		{
			//未供电状态
			check_info.failure_info = 0x01000000;

			//自检结束
			check_info.check_state = 1;
			sys_info.sys_st.b.self_check_st = 1;    // 0  开始自检    1 自检结束
			//进入手动模式
			sys_info.sys_st.b.sys_mode = 2;
		}
	}
	else
	{
		if(sys_info.sys_st.b.sys_mode == 2)    // 自检结束后 100ms 进来一次
		{
			//自检存在错误

			//CAN_communication_abnormal_flag = 0 ;   // 手动模式 下，can 通信故障清除，避免蜂鸣器一直响
			if(check_info.failure_info != 0)
			{
				//进入故障模式
				sys_info.sys_st.b.sys_mode = 5;
			}


			/*初次上电 且标定零点有效，回到零点、切换电流模式*/
            if(br_info.zero_point_flag == 0  &&  config_data.cali_data.br_zero_valid  == 0)
            {
            	SCA_Move_Pos(BR_ID, (float)(0/BRK_INF_RE), (float)500, (float)500, (float)-500);

            	SCA_Mode_Actuator(BR_ID,0x01);
            	br_info.zero_point_flag = 1 ;

            }

            if(ep_info.zero_point_flag == 0 && config_data.cali_data.ep_zero_valid  == 0)
            {

            	SCA_Move_Pos(EP_ID, (float)(0/EP_INF_RE), (float)500, (float)500, (float)-500);


            	SCA_Mode_Actuator(EP_ID,0x01);
            	ep_info.zero_point_flag = 1 ;
            }

			//转向模式检测
			if(sr_info.robot_st.b.motor_ctrl_mode == 0x06
					&& sr_info.robot_st.b.change_mode_st)
			{
				//切换电流模式
				SCA_Mode_Actuator(SR_ID,0x01);
				sr_info.robot_st.b.change_mode_st = 0;
			}

			if(br_info.robot_st.b.change_mode_st)
			{
				//踏板回位检测
				if(br_info.angle >= -10)  // -10
				{
					//切换电流模式
					SCA_Mode_Actuator(BR_ID,0x01);
					br_info.robot_st.b.change_mode_st = 0;
				}
				else
				{
					//回零点
					SCA_Move_Pos(BR_ID, (float)(0/BRK_INF_RE), (float)500, (float)500, (float)-500);
					//等待3秒，若没有到零点位置，直接切出

					if(br_info.limit_time_count > 30000)
					{
						br_info.robot_st.b.change_mode_st = 0;
						//切换电流模式
						SCA_Mode_Actuator(BR_ID,0x01);
					}
				}
			}

			if(ar_info.robot_st.b.change_mode_st)
			{
				//踏板回位检测
				if(ar_info.angle <= 10)
				{
					//切换电流模式
					SCA_Mode_Actuator(AR_ID,0x01);
					ar_info.robot_st.b.change_mode_st = 0;
				}
				else
				{
					//回零点
					SCA_Move_Pos(AR_ID, (float)(10/THR_INF_RE), (float)500, (float)500, (float)-500);
					//等待3秒，若没有到零点位置，直接切出
					if(ar_info.limit_time_count > 30000)
					{
						ar_info.robot_st.b.change_mode_st = 0;
						//切换电流模式
						SCA_Mode_Actuator(AR_ID,0x01);
					}
				}
			}

			if(ep_info.robot_st.b.change_mode_st)
			{
				//踏板回位检测
				if(ep_info.angle >= -10)
				{
					//切换电流模式
					SCA_Mode_Actuator(EP_ID,0x01);
					ep_info.robot_st.b.change_mode_st = 0;
				}
				else
				{
					//回零点
					SCA_Move_Pos(EP_ID, (float)(0/EP_INF_RE), (float)500, (float)500, (float)-500);  // -10
					//等待3秒，若没有到零点位置，直接切出
					if(ep_info.limit_time_count > 30000)
					{
						ep_info.robot_st.b.change_mode_st = 0;
						//切换电流模式
						SCA_Mode_Actuator(EP_ID,0x01);
					}
				}
			}
		}
		else if(sys_info.sys_st.b.sys_mode == 3 ||
				sys_info.sys_st.b.sys_mode == 4)
		{
			if((config_data.register_info.b.sr_use &&
						sr_info.alarm_code!= 0)||
			(config_data.register_info.b.br_use &&
						br_info.alarm_code!= 0)||
			   (config_data.register_info.b.ar_use &&
						ar_info.alarm_code!= 0)||
			   (config_data.register_info.b.ep_use &&
						ep_info.alarm_code!= 0))
			{
				sys_info.sys_st.b.sys_mode = 4;
				//first_run_flag  = 1 ;
			}
			else
			{
				sys_info.sys_st.b.sys_mode = 3;
				//first_run_flag  = 1 ;
			}
		}
	}
}

// 100ms 执行一次
void process_beep_event(void)
{
	if(check_info.check_state == 0)
	{
		if(check_info.check_step == 0)
		{
			check_info.check_step += 1;
			// BEEP蜂鸣一次
			set_beep(1, 1000, 0);
			beep_switch(BEEP_ON);
		}
	}
	else if(check_info.check_state == 1)
	{
		if(check_info.check_step == 1 && check_info.failure_info == 0 )
		{
			// BEEP蜂鸣二次
			set_beep(2, 500, 0);
			beep_switch(BEEP_ON);
			check_info.check_step += 1;
		}
		else
		{
			if(check_info.failure_info != 0)
			{
				//BEEP蜂鸣3声/分钟
				set_beep(3, 1000, 60000);
				if (get_beep_ctrl_state() == BEEP_OFF)
					beep_switch(BEEP_ON);
			}
			else
			{
				if(sys_info.sys_st.b.estop_st == 1)  	/* 急停状态：0-无急停  1-ESTOP激活 2-EBR激活 */
				{
					//BEEP蜂鸣5声/分钟
					set_beep(5, 2000, 60000);
					if (get_beep_ctrl_state() == BEEP_OFF)
						beep_switch(BEEP_ON);
				}
				else if(sys_info.sys_st.b.estop_st == 2)
				{

					set_beep(6, 3000, 120000);
					if (get_beep_ctrl_state() == BEEP_OFF)
						beep_switch(BEEP_ON);
				}
				else if(sys_info.sys_st.b.sys_mode == 4)    //安全模式响彻四声
				{
					//BEEP蜂鸣4声/2分钟
					set_beep(4, 600, 120000);
					if (get_beep_ctrl_state() == BEEP_OFF)
						beep_switch(BEEP_ON);
				}
				else
				{

				}
			}
		}
	}
}

void search_sca_real_attribute(void)
{
	if(check_info.check_state == 0)
	{
		if(check_info.ep_state == 3 || check_info.ep_state == 7)
		{
			SCA_Real_Attribute(EP_ID);
		}

		if(check_info.br_state == 3 || check_info.br_state == 6 ||
				check_info.br_state == 7)
		{
			SCA_Real_Attribute(BR_ID);
		}

		if(check_info.ar_state == 3 || check_info.ar_state == 7)
		{
			SCA_Real_Attribute(AR_ID);
		}
	}
	else
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//转向电机已开启
			{
				SCA_Real_Attribute(SR_ID);
			}
		}

		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				SCA_Real_Attribute(BR_ID);
			}
		}

		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				SCA_Real_Attribute(AR_ID);
			}
		}

		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				SCA_Real_Attribute(EP_ID);
			}
		}
	}
}

void process_sca_real_attribute(void)
{
	if(check_info.check_state == 1)
	{
		if(config_data.register_info.b.sr_use)//转向电机已使用
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)    //转向电机已开启且自检通过
			{

				PSCA = SCA_Handler_Update(1);
				sr_info.robot_st.b.motor_ctrl_mode = PSCA->Mode;
				sr_info.robot_st.b.motor_limit_st = PSCA->Position_Limit_State;
				sr_info.robot_st.b.motor_online_st = PSCA->Online_State;

				sr_info.angle = (int16_t)(PSCA->Position_Real*STE_INF_RE);
				sr_info.speed = PSCA->Velocity_Real / 10;	// 电机速度--> 转向盘速度，减速比10：1
				sr_info.alarm_code = PSCA->SCA_Warn.Error_Code;
				sr_info.voltage = PSCA->Voltage;
				sr_info.current = PSCA->Current_Real;
				sr_info.power = sr_info.voltage * sr_info.current;
				sr_info.temperature = PSCA->Motor_Temp;
				sr_info.torque = PSCA->Current_Real * PSCA->Torque_Factor *10; // // 转向10:1 需要0.1*10

				if(sr_info.alarm_code > 0)
					sys_info.sys_fault.b.sr_motor = 2;
				else
					sys_info.sys_fault.b.sr_motor = 0;
			}
			else
			{
				sr_info.robot_st.b.motor_ctrl_mode = 0;
				sr_info.robot_st.b.motor_limit_st = 0;
				sr_info.robot_st.b.motor_online_st = 0;

				sr_info.angle = 0;
				sr_info.speed = 0;
				sr_info.alarm_code = 0;
				sr_info.power = 0;
				sr_info.voltage = 0;
				sr_info.current = 0;
				sr_info.temperature = 0;
				sr_info.torque = 0;

				sys_info.sys_fault.b.sr_motor = 1;
			}
		}

		if(config_data.register_info.b.br_use)//制动电机已使用
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//制动电机已开启
			{
				PSCA = SCA_Handler_Update(2);
				br_info.robot_st.b.motor_ctrl_mode = PSCA->Mode;
				br_info.robot_st.b.motor_limit_st = PSCA->Position_Limit_State;
				br_info.robot_st.b.motor_online_st = PSCA->Online_State;

				br_info.angle =  (int16_t)(PSCA->Position_Real*BRK_INF_RE);
				br_info.speed = PSCA->Velocity_Real;
				br_info.alarm_code = PSCA->SCA_Warn.Error_Code;
				br_info.voltage = PSCA->Voltage;
				br_info.current = PSCA->Current_Real;
				br_info.power = br_info.current * br_info.current;
				br_info.temperature = PSCA->Motor_Temp;
				br_info.torque = PSCA->Current_Real * PSCA->Torque_Factor;

				if(br_info.alarm_code > 0)
					sys_info.sys_fault.b.br_motor = 2;
				else
					sys_info.sys_fault.b.br_motor = 0;
			}
			else
			{
				br_info.robot_st.b.motor_ctrl_mode = 0;
				br_info.robot_st.b.motor_limit_st = 0;
				br_info.robot_st.b.motor_online_st = 0;

				br_info.angle = 0;
				br_info.speed = 0;
				br_info.alarm_code = 0;
				br_info.power = 0;
				br_info.voltage = 0;
				br_info.current = 0;
				br_info.temperature = 0;
				br_info.torque = 0;

				sys_info.sys_fault.b.br_motor = 1;
			}
		}

		if(config_data.register_info.b.ar_use)//加速电机已使用
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//加速电机已开启
			{
				PSCA = SCA_Handler_Update(3);
				ar_info.robot_st.b.motor_ctrl_mode = PSCA->Mode;
				ar_info.robot_st.b.motor_limit_st = PSCA->Position_Limit_State;
				ar_info.robot_st.b.motor_online_st = PSCA->Online_State;

				ar_info.angle =  (int16_t)(PSCA->Position_Real*THR_INF_RE);
				ar_info.speed = PSCA->Velocity_Real;
				ar_info.alarm_code = PSCA->SCA_Warn.Error_Code;
				ar_info.voltage = PSCA->Voltage;
				ar_info.current = PSCA->Current_Real;
				ar_info.power = ar_info.current * ar_info.current;
				ar_info.temperature = PSCA->Motor_Temp;
				ar_info.torque = PSCA->Current_Real * PSCA->Torque_Factor;

				if(ar_info.alarm_code > 0)
					sys_info.sys_fault.b.ar_motor = 2;
				else
					sys_info.sys_fault.b.ar_motor = 0;
			}
			else
			{
				ar_info.robot_st.b.motor_ctrl_mode = 0;
				ar_info.robot_st.b.motor_limit_st = 0;
				ar_info.robot_st.b.motor_online_st = 0;

				ar_info.angle = 0;
				ar_info.speed = 0;
				ar_info.alarm_code = 0;
				ar_info.power = 0;
				ar_info.voltage = 0;
				ar_info.current = 0;
				ar_info.temperature = 0;
				ar_info.torque = 0;

				sys_info.sys_fault.b.ar_motor = 1;
			}
		}

		if(config_data.register_info.b.ep_use)//驻车电机已使用
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//驻车电机已开启
			{
				PSCA = SCA_Handler_Update(4);
				ep_info.robot_st.b.motor_ctrl_mode = PSCA->Mode;
				ep_info.robot_st.b.motor_limit_st = PSCA->Position_Limit_State;
				ep_info.robot_st.b.motor_online_st = PSCA->Online_State;

				ep_info.angle =  (int16_t)(PSCA->Position_Real*EP_INF_RE);
				ep_info.speed = PSCA->Velocity_Real;
				ep_info.alarm_code = PSCA->SCA_Warn.Error_Code;
				ep_info.voltage = PSCA->Voltage;
				ep_info.current = PSCA->Current_Real;
				ep_info.power = ep_info.current * ep_info.current;
				ep_info.temperature = PSCA->Motor_Temp;
				ep_info.torque = PSCA->Current_Real * PSCA->Torque_Factor;


				if(ep_info.alarm_code > 0)
					sys_info.sys_fault.b.ep_motor = 2;
				else
					sys_info.sys_fault.b.ep_motor = 0;
			}
			else
			{
				ep_info.robot_st.b.motor_ctrl_mode = 0;
				ep_info.robot_st.b.motor_limit_st = 0;
				ep_info.robot_st.b.motor_online_st = 0;

				ep_info.angle = 0;
				ep_info.speed = 0;
				ep_info.alarm_code = 0;
				ep_info.power = 0;
				ep_info.voltage = 0;
				ep_info.current = 0;
				ep_info.temperature = 0;
				ep_info.torque = 0;

				sys_info.sys_fault.b.ep_motor = 1;
			}
		}
	}
}

void upload_sca_real_position(void)
{
	if(check_info.check_state == 1)
	{
		actuator_position_msg(&can0_tx_msg,PRS_1_3,s_data);//封装应答消息    0x193  机器人位置信息上传
		can0_send_msg(can0_tx_msg);	//CAN0发送数据

		if (config_data.register_info.b.sr_infoSwitch)
		{
			actuator_CVP_msg(&can0_tx_msg,PRS_1_11, &sr_info);//封装应答消息      0x19B    SR信息上传
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

		if (config_data.register_info.b.br_infoSwitch)
		{
			//封装应答消息     0x19C    BR信息上传
			actuator_CVP_msg_ext(&can0_tx_msg,PRS_1_12, &br_info, s_data);
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
		if (config_data.register_info.b.ar_infoSwitch)
		{
			actuator_CVP_msg(&can0_tx_msg,PRS_1_13, &ar_info);//封装应答消息        0x19D   AR信息上传
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
		if (config_data.register_info.b.brar_ratioSwitch)
		{
			actuator_Ratio_msg(&can0_tx_msg,PRS_1_14, s_data);//封装应答消息       0x19E   AR,BR 加速百分比信息上传
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}

# if  0
		 // 新增加 0x19F EP 上报信息   暂时先不用
		if(config_data.register_info.b.ep_infoSwitch)
		{
		     actuator_CVP_EP_msg(&can0_tx_msg,PRS_1_15,&ep_info, s_data);//封装应答消息       0x19F   EP信息上传
						can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
#endif

	}
}

void upload_sca_real_alarm(void)
{
	if(check_info.check_state == 1)
	{
		if(sr_info.alarm_code != 0 || br_info.alarm_code != 0 ||
		   ar_info.alarm_code != 0 || ep_info.alarm_code != 0)
		{
			actuator_alarm_msg(&can0_tx_msg,PRS_1_4,s_data);//封装应答消    0x194  上报机器人故障信息
			can0_send_msg(can0_tx_msg);	//CAN0发送数据
		}
	}
}

void upload_sys_status(void)
{
	if (check_info.check_state)
	{
		system_status_reply_msg(&can0_tx_msg,PRS_1_2,s_data);//封装应答消息
		can0_send_msg(can0_tx_msg);	//CAN0发送数据
	}
}

__attribute__ ((section(".text")))
int main(void)
{
	AXBS_0.PORT[3].CRS.B.ARB = 1;  /* Round-robin (rotating) priority */
	
	xcptn_xmpl();                  /* Configure and Enable Interrupts */

	beep_init();

	Add_SCA(&can_port1, SR_ID, can3_send_msg);
	Add_SCA(&can_port2, BR_ID, can5_send_msg);
	Add_SCA(&can_port3, AR_ID, can7_send_msg);
	Add_SCA(&can_port4, EP_ID, can2_send_msg);

	param_init();


	//Init_SWT1();

	/* Loop forever */
	for(;;)
	{
		//100hz处理
		if(system_time.base_10ms_flag)
		{

			system_time.base_10ms_flag = false;
		}
		//50hz处理
		if(system_time.base_20ms_flag)
		{

			multicore_share_data();

			//处理CAN0/1数据
			process_can0_1_rx_msg();


			if(!can_ctrl_msg.robot_move_ctrl_bo_1 && !can_ctrl_msg.robot_move_ctrl_bo_2)
			{
				//查询sca实时数据
				search_sca_real_attribute();
			}

			//处理sca实时数据
			process_sca_real_attribute();

			//上传执行器实时位置数据, CVP数据
			upload_sca_real_position();

			// 上传系统状态信息
			upload_sys_status();   //机器人本体状态信息上传    帧ID 0x192

			system_time.base_20ms_flag = false;


		}

		//25hz处理
		if(system_time.base_40ms_flag)
		{
			system_time.base_40ms_flag = false;

		}

		//20hz处理
		if(system_time.base_50ms_flag)
		{
			system_time.base_50ms_flag = false;

		}

		//10hz处理
		if(system_time.base_100ms_flag)
		{
			system_time.base_100ms_flag = false;

			/* 机器人自检操作 -- 结束上电自检，进入循环自检后，在手动模式会进行电机模式切换会进行状态模式切换
			 * 在自动驾驶模式，会根据有无电机故障信息，在自动驾驶模式3、4之间切换
			 */
			check_self();

			//处理蜂鸣事件
			process_beep_event();

			//处理判定事件
			process_judge_event();

			//上传执行器实时报警数据
			upload_sca_real_alarm();
		}

		//5hz处理
		if(system_time.base_200ms_flag)
		{
			system_time.base_200ms_flag = false;

			//SWT0_Service();
		}

		//4hz处理
		if(system_time.base_250ms_flag)
		{
			//SWT1_Service();
			system_time.base_250ms_flag = false;
		}

		//2hz处理
		if(system_time.base_500ms_flag)
		{
			system_time.base_500ms_flag = false;
		}

		//1hz处理
		if(system_time.base_1000ms_flag)
		{
			system_time.base_1000ms_flag = false;
		}
	}
}

void pit1_isr(void)
{
	/* Clear interrupt flag */
	PIT.TIMER[1].TFLG.R = 1;

	if(delay_time.base_100us_flag)
	{
		++delay_time.base_100us_cnt;
	}

	if(delay_time.base_1ms_flag)
	{
		if (++delay_time.base_1ms_flag_cnt > 9)
		{
			delay_time.base_1ms_flag_cnt = 0;
			delay_time.base_1ms_cnt++;
		}
	}
	else
	{
		delay_time.base_1ms_flag_cnt = 0;
	}

	if(++system_time.base_20ms_cnt > 200)
	{
		system_time.base_20ms_cnt = 0;
		system_time.base_20ms_flag = true;
	}

	if(++system_time.base_40ms_cnt > 400)
	{
		system_time.base_40ms_cnt = 0;
		system_time.base_40ms_flag = true;
	}

	if(++system_time.base_50ms_cnt > 500)
	{
		system_time.base_50ms_cnt = 0;
		system_time.base_50ms_flag = true;
	}

	if(++system_time.base_100ms_cnt > 1000)
	{
		system_time.base_100ms_cnt = 0;
		system_time.base_100ms_flag = true;
	}

	if(++system_time.base_200ms_cnt > 2000)
	{
		system_time.base_200ms_cnt = 0;
		system_time.base_200ms_flag = true;
	}

	if(++system_time.base_250ms_cnt > 2500)
	{
		system_time.base_250ms_cnt = 0;
		system_time.base_250ms_flag = true;
	}

	if(++system_time.base_500ms_cnt > 5000)
	{
		system_time.base_500ms_cnt = 0;
		system_time.base_500ms_flag = true;
	}

	if(++system_time.base_1000ms_cnt > 10000)
	{
		system_time.base_1000ms_cnt = 0;
		system_time.base_1000ms_flag = true;
	}

	if(br_info.robot_st.b.change_mode_st)
	{
		br_info.limit_time_count++;
	}

	if(ar_info.robot_st.b.change_mode_st)
	{
		ar_info.limit_time_count++;
	}

	if(ep_info.robot_st.b.change_mode_st)
	{
		ep_info.limit_time_count++;
	}
}

void can0_isr(void)      // CAN0  根据帧ID来处理can_0 接收的数据
{
	if (CAN_0.IFLAG1.B.BUF6I)	//overflow warning
	{
		CAN_0.IFLAG1.B.BUF6I = 1;
	}

	if (CAN_0.IFLAG1.B.BUF7I)	//overflow
	{
		CAN_0.IFLAG1.B.BUF7I = 1;
	}

	if (CAN_0.IFLAG1.B.BUF5I)
	{
		if(can0_get_msg(&can0_rx_msg))
		{
			//if(check_sum(&can0_rx_msg) == can0_rx_msg.data[0])
			//{
				switch(can0_rx_msg.std_id)
				{
				case CTRL_0_1:         //0x101   机器人限制标定  标定位置、 最大行程
					{
						can_ctrl_msg.sr_limit_cali_req =
										can0_rx_msg.data[1]&0x03;
						can_ctrl_msg.br_limit_cali_req =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.ar_limit_cali_req =
										(can0_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.ep_limit_cali_req =
										(can0_rx_msg.data[1]>>4)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_1 =
										can0_rx_msg.data[2]&0x0f;
						can_ctrl_msg.limit_retry_count = 0;
						can_ctrl_msg.limit_time_count = 0;
						can_ctrl_msg.robot_limit_cali_bo = true;
					break;
					}
					case CTRL_0_2:       //0x102 机器人运动参数标准，标定加减速
					{
						can_ctrl_msg.sr_param_cali_req =
										can0_rx_msg.data[1]&0x01;
						can_ctrl_msg.br_param_cali_req =
										(can0_rx_msg.data[1]>>1)&0x01;
						can_ctrl_msg.ar_param_cali_req =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.sr_acc_dec_cali =
										((can0_rx_msg.data[2]&0xff)<<4)|
						((can0_rx_msg.data[1]>>4)&0x0f);
						can_ctrl_msg.br_speed_cali =
										((can0_rx_msg.data[4]&0x01)<<8)|
						((can0_rx_msg.data[3])&0xff);
						can_ctrl_msg.br_acc_dec_cali =
										((can0_rx_msg.data[5]&0x03)<<7)|
						((can0_rx_msg.data[4]>>1)&0x7f);
						can_ctrl_msg.ar_speed_cali =
										((can0_rx_msg.data[6]&0x07)<<6)|
						((can0_rx_msg.data[5]>>2)&0x3f);
						can_ctrl_msg.ar_acc_dec_cali =
										((can0_rx_msg.data[7]&0x0f)<<5)|
						((can0_rx_msg.data[6]>>3)&0x1f);
						can_ctrl_msg.live_counter_ctrl_0_2 =
										(can0_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.robot_param_cali_bo = true;
					break;
					}
					case CTRL_0_3:    //0x103   查询标定数据
					{
						can_ctrl_msg.cali_data_query_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_3 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.cali_data_query_bo = true;
					break;
					}
					case CTRL_0_4:    //0x104   机器人电机参数查询
					{
						can_ctrl_msg.motor_info_query_req =
										(can0_rx_msg.data[1]&0x07);
						can_ctrl_msg.motor_voltage_query_req =
										(can0_rx_msg.data[2]&0x01);
						can_ctrl_msg.motor_current_query_req =
										(can0_rx_msg.data[2]>>1)&0x01;
						can_ctrl_msg.motor_speed_query_req =
										(can0_rx_msg.data[2]>>2)&0x01;
						can_ctrl_msg.motor_pos_query_req =
										(can0_rx_msg.data[2]>>3)&0x01;
						can_ctrl_msg.motor_temp_query_req =
										(can0_rx_msg.data[2]>>4)&0x01;
						can_ctrl_msg.motor_pos_max_query_req =
										(can0_rx_msg.data[2]>>5)&0x01;
						can_ctrl_msg.motor_pos_min_query_req =
										(can0_rx_msg.data[2]>>6)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_4 =
										(can0_rx_msg.data[3]>>4)&0x0f;
						can_ctrl_msg.motor_info_query_bo = true;
					break;
					}
					case CTRL_0_5:
					{
						can_ctrl_msg.serial_number =
										(can0_rx_msg.data[4]<<24)|
										(can0_rx_msg.data[3]<<16)|
										(can0_rx_msg.data[2]<<8)|
										(can0_rx_msg.data[1]&0xff);
						can_ctrl_msg.hardware_version =
										(can0_rx_msg.data[5]&0xff);
						can_ctrl_msg.device_id =
										((can0_rx_msg.data[7]&0x0f)<<8)|
										(can0_rx_msg.data[6]&0xff);
						can_ctrl_msg.live_counter_ctrl_0_5 =
										(can0_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.device_info_set_bo = true;
					break;
					}
					case CTRL_0_6:
					{
						can_ctrl_msg.device_info_query_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_6 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.device_info_query_bo = true;
					break;
					}
					case CTRL_0_7:  // 0x107  机器人标定数据清除
					{
						can_ctrl_msg.cali_data_clear_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_7 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.cali_data_clear_bo = true;
					break;
					}
					case CTRL_0_8:     // 0x108
					{
						can_ctrl_msg.sr_disable_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.br_disable_req =
										(can0_rx_msg.data[1]>>1)&0x01;
						can_ctrl_msg.ar_disable_req =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.ep_disable_req =
										(can0_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_8 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.robot_disable_bo = true;
					break;
					}
					case CTRL_0_9:
					{
						can_ctrl_msg.sr_clearERR_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.br_clearERR_req =
										(can0_rx_msg.data[1]>>1)&0x01;
						can_ctrl_msg.ar_clearERR_req =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.ep_clearERR_req =
										(can0_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_8 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.motor_clearERR_bo = true;
					break;
					}
					case CTRL_0_A:
					{
						can_ctrl_msg.sr_infoSwitch_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.br_infoSwitch_req =
										(can0_rx_msg.data[1]>>1)&0x01;
						can_ctrl_msg.ar_infoSwitch_req =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.ep_infoSwitch_req =
										(can0_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.brar_ratioSwitch_req =
										(can0_rx_msg.data[1]>>4)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_8 =
										(can0_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_infoSwitch_bo = true;
					break;
					}
					case CTRL_1_1:
					{
						can_ctrl_msg.sr_open_ctrl_req_1 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_open_ctrl_req_1 =
										(can0_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_open_ctrl_req_1 =
										(can0_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_open_ctrl_req_1 =
										(can0_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl1_1_1 =
										(can0_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_open_ctrl_bo_1 = true;
					break;
					}
					case CTRL_1_2:
					{
						can_ctrl_msg.sr_mode_ctrl_req_1 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_mode_ctrl_req_1 =
										(can0_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_mode_ctrl_req_1 =
										(can0_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_mode_ctrl_req_1 =
										(can0_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl1_1_2 =
										(can0_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_mode_ctrl_bo_1 = true;
					break;
					}
					case CTRL_1_3:
					{
						can_ctrl_msg.auto_driving_ctrl_1 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.ep_request_st_1 =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.sr_request_st_1 =
										(can0_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.sr_angle_sign_1 =
										(can0_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.sr_speed_1 =
										((can0_rx_msg.data[2]&0xff)<<2)|
										((can0_rx_msg.data[1]>>6)&0x03);
						can_ctrl_msg.sr_angle_1 =
										((can0_rx_msg.data[4]&0x3f)<<8)|
										(can0_rx_msg.data[3]&0xff);
						can_ctrl_msg.br_angle_1 =
										((can0_rx_msg.data[5]&0xff)<<2)|
										((can0_rx_msg.data[4]>>6)&0x03);
						can_ctrl_msg.br_request_st_1 =
										(can0_rx_msg.data[6]&0x01);
						can_ctrl_msg.ar_request_st_1 =
										(can0_rx_msg.data[6]>>1)&0x01;
						can_ctrl_msg.ar_angle_1 =
										((can0_rx_msg.data[7]&0x0f)<<6)|
										((can0_rx_msg.data[6]>>2)&0x3f);
						can_ctrl_msg.live_counter_ctrl1_1_3 =
										(can0_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.robot_move_ctrl_bo_1 = true;
					break;
					}
					case CTRL_1_4:
					{
						can_ctrl_msg.state_detail_query_req_1 =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl1_1_4 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.state_detail_query_bo_1 = true;
					break;
					}
					case CTRL_1_5:
					{
						can_ctrl_msg.power_sys_query_req_1 =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.power_sr_query_req_1 =
										((can0_rx_msg.data[1]>>1)&0x01);
						can_ctrl_msg.power_br_query_req_1 =
										((can0_rx_msg.data[1]>>2)&0x01);
						can_ctrl_msg.power_ar_query_req_1 =
										((can0_rx_msg.data[1]>>3)&0x01);
						can_ctrl_msg.power_ep_query_req_1 =
										((can0_rx_msg.data[1]>>4)&0x01);
						can_ctrl_msg.live_counter_ctrl1_1_5 =
										((can0_rx_msg.data[2]>>4)&0x0f);
						can_ctrl_msg.power_query_bo_1 = true;
					break;
					}
					case CTRL_1_6:
					{
						can_ctrl_msg.auto_driver_ctrl_1 =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl1_1_6 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.ctrl_auto_driver_control_bo_1 = true;
					break;
					}
					case  CTRL_1_7:       //新增117协议  增加EP和BR电机反转操作
					{
						can_ctrl_msg.CTRL1_HandDrivingCtrl = (can0_rx_msg.data[1]& 0x01) ;
						can_ctrl_msg.CTRL1_EP_DIRECTION = (can0_rx_msg.data[1]>>1)&0x01 ;
						can_ctrl_msg.CTRL1_BR_DIRECTION = (can0_rx_msg.data[1]>>2)&0x01 ;
						can_ctrl_msg.CTRL1_HAND_EP_Angle = (can0_rx_msg.data[2]<<8)| can0_rx_msg.data[3] ;
						can_ctrl_msg.CTRL1_HAND_BR_Angle = (can0_rx_msg.data[4]<<8)| can0_rx_msg.data[5] ;

						break ;
					}

					case CTRL_2_1:
					{
						can_ctrl_msg.sr_open_ctrl_req_2 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_open_ctrl_req_2 =
										(can0_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_open_ctrl_req_2 =
										(can0_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_open_ctrl_req_2 =
										(can0_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl2_1_1 =
										(can0_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_open_ctrl_bo_2 = true;
					break;
					}
					case CTRL_2_2:
					{
						can_ctrl_msg.sr_mode_ctrl_req_2 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_mode_ctrl_req_2 =
										(can0_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_mode_ctrl_req_2 =
										(can0_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_mode_ctrl_req_2 =
										(can0_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl2_1_2 =
										(can0_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_mode_ctrl_bo_2 = true;
					break;
					}
					case CTRL_2_3:
					{
						can_ctrl_msg.auto_driving_ctrl_2 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.ep_request_st_2 =
										(can0_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.sr_request_st_2 =
										(can0_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.sr_angle_sign_2 =
										(can0_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.sr_speed_2 =
										((can0_rx_msg.data[2]&0xff)<<2)|
										((can0_rx_msg.data[1]>>6)&0x03);
						can_ctrl_msg.sr_angle_2 =
										((can0_rx_msg.data[4]&0x3f)<<8)|
										(can0_rx_msg.data[3]&0xff);
						can_ctrl_msg.br_angle_2 =
										((can0_rx_msg.data[5]&0xff)<<2)|
										((can0_rx_msg.data[4]>>6)&0x03);
						can_ctrl_msg.br_request_st_2 =
										(can0_rx_msg.data[6]&0x01);
						can_ctrl_msg.ar_request_st_2 =
										(can0_rx_msg.data[6]>>1)&0x01;
						can_ctrl_msg.ar_angle_2 =
										((can0_rx_msg.data[7]&0x0f)<<6)|
										((can0_rx_msg.data[6]>>2)&0x3f);
						can_ctrl_msg.live_counter_ctrl2_1_3 =
										(can0_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.robot_move_ctrl_bo_2 = true;
					break;
					}
					case CTRL_2_4:
					{
						can_ctrl_msg.state_detail_query_req_2 =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl2_1_4 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.state_detail_query_bo_2 = true;
					break;
					}
					case CTRL_2_5:
					{
						can_ctrl_msg.power_sys_query_req_2 =
										((can0_rx_msg.data[1])&0x01);
						can_ctrl_msg.power_sr_query_req_2 =
										((can0_rx_msg.data[1]>>1)&0x01);
						can_ctrl_msg.power_br_query_req_2 =
										((can0_rx_msg.data[1]>>2)&0x01);
						can_ctrl_msg.power_ar_query_req_2 =
										((can0_rx_msg.data[1]>>3)&0x01);
						can_ctrl_msg.power_ep_query_req_2 =
										((can0_rx_msg.data[1]>>4)&0x01);
						can_ctrl_msg.live_counter_ctrl2_1_5 =
										(can0_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.power_query_bo_2 = true;
					break;
					}
					case CTRL_2_6:
					{
						can_ctrl_msg.auto_driver_ctrl_2 =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.live_counter_ctrl2_1_6 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.ctrl_auto_driver_control_bo_2 = true;
					break;
					}
					case CTRL_2_7:
					{
						can_ctrl_msg.ctrl_flag =
										(can0_rx_msg.data[1]&0x03);
						can_ctrl_msg.live_counter_ctrl2_1_7 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.ctrl_route_set_bo = true;
					break;
					}
				}
			}
		//}
	}
}

void can1_isr(void)
{
	if (CAN_1.IFLAG1.B.BUF6I)	//overflow warning
	{
		CAN_1.IFLAG1.B.BUF6I = 1;
	}

	if (CAN_1.IFLAG1.B.BUF7I)	//overflow
	{
		CAN_1.IFLAG1.B.BUF7I = 1;
	}

	if (CAN_1.IFLAG1.B.BUF5I)
	{
		if(can1_get_msg(&can1_rx_msg))
		{
			if(check_sum(&can1_rx_msg) == can1_rx_msg.data[0])
			{
				switch(can1_rx_msg.std_id)
				{
					case CTRL_0_1:
					{
						can_ctrl_msg.sr_limit_cali_req =
										can1_rx_msg.data[1]&0x03;
						can_ctrl_msg.br_limit_cali_req =
										(can1_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.ar_limit_cali_req =
										(can1_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.ep_limit_cali_req =
										(can1_rx_msg.data[1]>>4)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_1 =
										can1_rx_msg.data[2]&0x0f;
						can_ctrl_msg.limit_retry_count = 0;
						can_ctrl_msg.limit_time_count = 0;
						can_ctrl_msg.robot_limit_cali_bo = true;
					break;
					}
					case CTRL_0_2:
					{
						can_ctrl_msg.sr_param_cali_req =
										can1_rx_msg.data[1]&0x01;
						can_ctrl_msg.br_param_cali_req =
										(can1_rx_msg.data[1]>>1)&0x01;
						can_ctrl_msg.ar_param_cali_req =
										(can1_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.sr_acc_dec_cali =
										((can1_rx_msg.data[2]&0xff)<<4)|
						((can1_rx_msg.data[1]>>4)&0x0f);
						can_ctrl_msg.br_speed_cali =
										((can1_rx_msg.data[4]&0x01)<<8)|
						((can1_rx_msg.data[3])&0xff);
						can_ctrl_msg.br_acc_dec_cali =
										((can1_rx_msg.data[5]&0x03)<<7)|
						((can1_rx_msg.data[4]>>1)&0x7f);
						can_ctrl_msg.ar_speed_cali =
										((can1_rx_msg.data[6]&0x07)<<6)|
						((can1_rx_msg.data[5]>>2)&0x3f);
						can_ctrl_msg.ar_acc_dec_cali =
										((can1_rx_msg.data[7]&0x0f)<<5)|
						((can1_rx_msg.data[6]>>3)&0x1f);
						can_ctrl_msg.live_counter_ctrl_0_2 =
										(can1_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.robot_param_cali_bo = true;
					break;
					}
					case CTRL_0_3:
					{
						can_ctrl_msg.cali_data_query_req =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_3 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.cali_data_query_bo = true;
					break;
					}
					case CTRL_0_4:
					{
						can_ctrl_msg.motor_info_query_req =
										(can1_rx_msg.data[1]&0x07);
						can_ctrl_msg.motor_voltage_query_req =
										(can1_rx_msg.data[2]&0x01);
						can_ctrl_msg.motor_current_query_req =
										(can1_rx_msg.data[2]>>1)&0x01;
						can_ctrl_msg.motor_speed_query_req =
										(can1_rx_msg.data[2]>>2)&0x01;
						can_ctrl_msg.motor_pos_query_req =
										(can1_rx_msg.data[2]>>3)&0x01;
						can_ctrl_msg.motor_temp_query_req =
										(can1_rx_msg.data[2]>>4)&0x01;
						can_ctrl_msg.motor_pos_max_query_req =
										(can1_rx_msg.data[2]>>5)&0x01;
						can_ctrl_msg.motor_pos_min_query_req =
										(can1_rx_msg.data[2]>>6)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_4 =
										(can1_rx_msg.data[3]>>4)&0x0f;
						can_ctrl_msg.motor_info_query_bo = true;
					break;
					}
					case CTRL_0_5:
					{
						can_ctrl_msg.serial_number =
										(can1_rx_msg.data[4]<<24)|
										(can1_rx_msg.data[3]<<16)|
										(can1_rx_msg.data[2]<<8)|
										(can1_rx_msg.data[1]&0xff);
						can_ctrl_msg.hardware_version =
										(can1_rx_msg.data[5]&0xff);
						can_ctrl_msg.device_id =
										((can1_rx_msg.data[7]&0x0f)<<8)|
										(can1_rx_msg.data[6]&0xff);
						can_ctrl_msg.live_counter_ctrl_0_5 =
										(can1_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.device_info_set_bo = true;
					break;
					}
					case CTRL_0_6:
					{
						can_ctrl_msg.device_info_query_req =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_6 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.device_info_query_bo = true;
					break;
					}
					case CTRL_0_7:
					{
						can_ctrl_msg.cali_data_clear_req =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_7 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.cali_data_clear_bo = true;
					break;
					}
					case CTRL_0_8:
					{
						can_ctrl_msg.sr_disable_req =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.br_disable_req =
										(can1_rx_msg.data[1]>>1)&0x01;
						can_ctrl_msg.ar_disable_req =
										(can1_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.ep_disable_req =
										(can1_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.live_counter_ctrl_0_8 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.robot_disable_bo = true;
					break;
					}
					case CTRL_1_1:
					{
						can_ctrl_msg.sr_open_ctrl_req_1 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_open_ctrl_req_1 =
										(can1_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_open_ctrl_req_1 =
										(can1_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_open_ctrl_req_1 =
										(can1_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl1_1_1 =
										(can1_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_open_ctrl_bo_1 = true;
					break;
					}
					case CTRL_1_2:
					{
						can_ctrl_msg.sr_mode_ctrl_req_1 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_mode_ctrl_req_1 =
										(can1_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_mode_ctrl_req_1 =
										(can1_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_mode_ctrl_req_1 =
										(can1_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl1_1_2 =
										(can1_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_mode_ctrl_bo_1 = true;
					break;
					}
					case CTRL_1_3:
					{
						can_ctrl_msg.auto_driving_ctrl_1 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.ep_request_st_1 =
										(can1_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.sr_request_st_1 =
										(can1_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.sr_angle_sign_1 =
										(can1_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.sr_speed_1 =
										((can1_rx_msg.data[2]&0xff)<<2)|
										((can1_rx_msg.data[1]>>6)&0x03);
						can_ctrl_msg.sr_angle_1 =
										((can1_rx_msg.data[4]&0x3f)<<8)|
										(can1_rx_msg.data[3]&0xff);
						can_ctrl_msg.br_angle_1 =
										((can1_rx_msg.data[5]&0xff)<<2)|
										((can1_rx_msg.data[4]>>6)&0x03);
						can_ctrl_msg.br_request_st_1 =
										(can1_rx_msg.data[6]&0x01);
						can_ctrl_msg.ar_request_st_1 =
										(can1_rx_msg.data[6]>>1)&0x01;
						can_ctrl_msg.ar_angle_1 =
										((can1_rx_msg.data[7]&0x0f)<<6)|
										((can1_rx_msg.data[6]>>2)&0x3f);
						can_ctrl_msg.live_counter_ctrl1_1_3 =
										(can1_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.robot_move_ctrl_bo_1 = true;
					break;
					}
					case CTRL_1_4:
					{
						can_ctrl_msg.state_detail_query_req_1 =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl1_1_4 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.state_detail_query_bo_1 = true;
					break;
					}
					case CTRL_1_5:
					{
						can_ctrl_msg.power_sys_query_req_1 =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.power_sr_query_req_1 =
										((can1_rx_msg.data[1]>>1)&0x01);
						can_ctrl_msg.power_br_query_req_1 =
										((can1_rx_msg.data[1]>>2)&0x01);
						can_ctrl_msg.power_ar_query_req_1 =
										((can1_rx_msg.data[1]>>3)&0x01);
						can_ctrl_msg.power_ep_query_req_1 =
										((can1_rx_msg.data[1]>>4)&0x01);
						can_ctrl_msg.live_counter_ctrl1_1_5 =
										((can1_rx_msg.data[2]>>4)&0x0f);
						can_ctrl_msg.power_query_bo_1 = true;
					break;
					}
					case CTRL_1_6:
					{
						can_ctrl_msg.auto_driver_ctrl_1 =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl1_1_6 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.ctrl_auto_driver_control_bo_1 = true;
					break;

					}
					case  CTRL_1_7:       //新增117协议  增加EP和BR电机反转操作
					{
						can_ctrl_msg.CTRL1_HandDrivingCtrl = (can0_rx_msg.data[1]& 0x01) ;
						can_ctrl_msg.CTRL1_EP_DIRECTION = (can0_rx_msg.data[1]>>1)&0x01 ;
						can_ctrl_msg.CTRL1_BR_DIRECTION = (can0_rx_msg.data[1]>>2)&0x01 ;
						can_ctrl_msg.CTRL1_HAND_EP_Angle = (can0_rx_msg.data[2]<<8)| can0_rx_msg.data[3] ;
						can_ctrl_msg.CTRL1_HAND_BR_Angle = (can0_rx_msg.data[4]<<8)| can0_rx_msg.data[5] ;

						break ;
					}
					case CTRL_2_1:
					{
						can_ctrl_msg.sr_open_ctrl_req_2 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_open_ctrl_req_2 =
										(can1_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_open_ctrl_req_2 =
										(can1_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_open_ctrl_req_2 =
										(can1_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl2_1_1 =
										(can1_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_open_ctrl_bo_2 = true;
					break;
					}
					case CTRL_2_2:
					{
						can_ctrl_msg.sr_mode_ctrl_req_2 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.br_mode_ctrl_req_2 =
										(can1_rx_msg.data[1]>>2)&0x03;
						can_ctrl_msg.ar_mode_ctrl_req_2 =
										(can1_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.ep_mode_ctrl_req_2 =
										(can1_rx_msg.data[1]>>6)&0x03;
						can_ctrl_msg.live_counter_ctrl2_1_2 =
										(can1_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.robot_mode_ctrl_bo_2 = true;
					break;
					}
					case CTRL_2_3:
					{
						can_ctrl_msg.auto_driving_ctrl_2 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.ep_request_st_2 =
										(can1_rx_msg.data[1]>>2)&0x01;
						can_ctrl_msg.sr_request_st_2 =
										(can1_rx_msg.data[1]>>3)&0x01;
						can_ctrl_msg.sr_angle_sign_2 =
										(can1_rx_msg.data[1]>>4)&0x03;
						can_ctrl_msg.sr_speed_2 =
										((can1_rx_msg.data[2]&0xff)<<2)|
										((can1_rx_msg.data[1]>>6)&0x03);
						can_ctrl_msg.sr_angle_2 =
										((can1_rx_msg.data[4]&0x3f)<<8)|
										(can1_rx_msg.data[3]&0xff);
						can_ctrl_msg.br_angle_2 =
										((can1_rx_msg.data[5]&0xff)<<2)|
										((can1_rx_msg.data[4]>>6)&0x03);
						can_ctrl_msg.br_request_st_2 =
										(can1_rx_msg.data[6]&0x01);
						can_ctrl_msg.ar_request_st_2 =
										(can1_rx_msg.data[6]>>1)&0x01;
						can_ctrl_msg.ar_angle_2 =
										((can1_rx_msg.data[7]&0x0f)<<6)|
										((can1_rx_msg.data[6]>>2)&0x3f);
						can_ctrl_msg.live_counter_ctrl2_1_3 =
										(can1_rx_msg.data[7]>>4)&0x0f;
						can_ctrl_msg.robot_move_ctrl_bo_2 = true;
					break;
					}
					case CTRL_2_4:
					{
						can_ctrl_msg.state_detail_query_req_2 =
										(can1_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl2_1_4 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.state_detail_query_bo_2 = true;
					break;
					}
					case CTRL_2_5:
					{
						can_ctrl_msg.power_sys_query_req_2 =
										((can1_rx_msg.data[1])&0x01);
						can_ctrl_msg.power_sr_query_req_2 =
										((can1_rx_msg.data[1]>>1)&0x01);
						can_ctrl_msg.power_br_query_req_2 =
										((can1_rx_msg.data[1]>>2)&0x01);
						can_ctrl_msg.power_ar_query_req_2 =
										((can1_rx_msg.data[1]>>3)&0x01);
						can_ctrl_msg.power_ep_query_req_2 =
										((can1_rx_msg.data[1]>>4)&0x01);
						can_ctrl_msg.live_counter_ctrl2_1_5 =
										(can1_rx_msg.data[2]>>4)&0x0f;
						can_ctrl_msg.power_query_bo_2 = true;
					break;
					}
					case CTRL_2_6:
					{
						can_ctrl_msg.auto_driver_ctrl_2 =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.live_counter_ctrl2_1_6 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.ctrl_auto_driver_control_bo_2 = true;
					break;
					}
					case CTRL_2_7:
					{
						can_ctrl_msg.ctrl_flag =
										(can1_rx_msg.data[1]&0x03);
						can_ctrl_msg.live_counter_ctrl2_1_7 =
										(can1_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.ctrl_route_set_bo = true;
					break;
					}
				}
			}
		}
	}
}

void can2_isr(void)
{
	if (CAN_2.IFLAG1.B.BUF6I)	//overflow warning
	{
		CAN_2.IFLAG1.B.BUF6I = 1;
	}

	if (CAN_2.IFLAG1.B.BUF7I)	//overflow
	{
		CAN_2.IFLAG1.B.BUF7I = 1;
	}

	if (CAN_2.IFLAG1.B.BUF5I)
	{
		if(CAN2_GetMsg(&can2_rx_msg))
		{
			SCA_CAN_Msg_Analysis(&can2_rx_msg);
		}
	}
}

void can3_isr(void)
{
	if (CAN_3.IFLAG1.B.BUF6I)	//overflow warning
	{
		CAN_3.IFLAG1.B.BUF6I = 1;
	}

	if (CAN_3.IFLAG1.B.BUF7I)	//overflow
	{
		CAN_3.IFLAG1.B.BUF7I = 1;
	}

	if (CAN_3.IFLAG1.B.BUF5I)
	{
		if(CAN3_GetMsg(&can3_rx_msg))
		{
			SCA_CAN_Msg_Analysis(&can3_rx_msg);
		}
	}
}

void can5_isr(void)
{
	if (CAN_5.IFLAG1.B.BUF6I)	//overflow warning
	{
		CAN_5.IFLAG1.B.BUF6I = 1;
	}

	if (CAN_5.IFLAG1.B.BUF7I)	//overflow
	{
		CAN_5.IFLAG1.B.BUF7I = 1;
	}

	if (CAN_5.IFLAG1.B.BUF5I)
	{
		if(CAN5_GetMsg(&can5_rx_msg))
		{
			SCA_CAN_Msg_Analysis(&can5_rx_msg);
		}
	}
}

void can7_isr(void)
{
	if (CAN_7.IFLAG1.B.BUF6I)	//overflow warning
	{
		CAN_7.IFLAG1.B.BUF6I = 1;
	}

	if (CAN_7.IFLAG1.B.BUF7I)	//overflow
	{
		CAN_7.IFLAG1.B.BUF7I = 1;
	}

	if (CAN_7.IFLAG1.B.BUF5I)
	{
		if(CAN7_GetMsg(&can7_rx_msg))
		{
			SCA_CAN_Msg_Analysis(&can7_rx_msg);
		}
	}
}


