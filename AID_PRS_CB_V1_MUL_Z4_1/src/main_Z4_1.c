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



#define   EP_BRAKE_SYMBOL     0  // ����EP�ƶ��ĺ�




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

//CAN�˿���Ϣ���壬���ڰ�SCA�����ʵ�ֶ�˿ڿ���
CAN_HANDLER can_port1, can_port2, can_port3, can_port4;

// IPC ͨ��
CAN_MSG can0_tx_msg;            	//CAN0д����
CAN_MSG can0_rx_msg;             	//CAN0������

// ADCU ͨ��
CAN_MSG can1_tx_msg;            	//CAN1д����
CAN_MSG can1_rx_msg;             	//CAN1������

// EP �ؽ�ͨ��
CAN_MSG can2_tx_msg;            	//CAN2д����
CAN_MSG can2_rx_msg;             	//CAN2������

// SR �ؽ�ͨ��
CAN_MSG can3_tx_msg;            	//CAN3д����
CAN_MSG can3_rx_msg;             	//CAN3������

// BR �ؽ�ͨ��
CAN_MSG can5_tx_msg;            	//CAN5д����
CAN_MSG can5_rx_msg;             	//CAN5������

// AR �ؽ�ͨ��
CAN_MSG can7_tx_msg;            	//CAN7д����
	//CAN7������

//���»�������
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

//0x0����Ч
//0x1���Լ�ģʽ
//0x2���ֶ�ģʽ
//0x3���Զ�ģʽ
//0x4����ȫģʽ
//0x5������ģʽ

void process_judge_event(void)
{
	if(sys_info.sys_st.b.self_check_st == 1)
	{
		//�����ֶ���ʻ״̬���л�
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
				SW_LED = 1;//����
			}
		}

		//�����Զ���ʻ״̬���л�
		if(sys_info.sys_st.b.sys_mode == 3 ||
		   sys_info.sys_st.b.sys_mode == 4)
		{
			// ������Զ���ʻ������������ݰ��������Զ���ʻ״̬
			if (AUTO_DI5 == 0 || AUTO_DI6 == 0)
			{
				sys_info.sys_st.b.auto_driving_st = 1;
			}

			// �Զ���ʻ״̬ʱ���ӹ�̤�塢�ֱ�
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
				SW_LED = 0;//���
			}
		}
	}
}

void process_can0_1_rx_msg(void)
{
	float real_temp;
    //  ���Զ��Ͱ�ȫģʽ   �����˲����궨 λ�� �������
	if(can_ctrl_msg.robot_limit_cali_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//ת�����ѿ���
			{
				if(can_ctrl_msg.sr_limit_cali_req != 0)//�궨ת����
				{
					switch(can_ctrl_msg.sr_limit_cali_req)//ת�����궨�ж�
					{
						case 1://���궨
						{
							if(SCA_Set_Up_Home(SR_ID) == SCA_NoError)
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
								can0_send_msg(can0_tx_msg);	//CAN0��������
							}
							else
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_FAIL,0x01);//��װͨ��Ӧ��
								can0_send_msg(can0_tx_msg);	//CAN0��������
							}
							break;
						}
						case 2://�����궨
						{
							if(SCA_Pos_Min_Limit(SR_ID,(float)(sr_info.angle/STE_INF_RE)) == SCA_NoError)
							{
								config_data.cali_data.sr_left_angle =
										sr_info.angle;//���ݴ洢���ṹ�����
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
								can0_send_msg(can0_tx_msg);	//CAN0��������
							}
							else
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_FAIL,0x02);//��װͨ��Ӧ��
								can0_send_msg(can0_tx_msg);	//CAN0��������
							}
							break;
						}
						case 3://���ҵ�궨
						{
							if(SCA_Pos_Max_Limit(SR_ID,(float)(sr_info.angle/STE_INF_RE)) == SCA_NoError)
							{
								config_data.cali_data.sr_right_angle =
										sr_info.angle;//���ݴ洢���ṹ�����
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
								can0_send_msg(can0_tx_msg);	//CAN0��������
							}
							else
							{
								general_replay_msg(&can0_tx_msg,
										PRS_1_1,CTRL_0_1,SR_ID,RESULT_FAIL,0x03);//��װͨ��Ӧ��
								can0_send_msg(can0_tx_msg);	//CAN0��������
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

		//�ƶ�
		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				if(can_ctrl_msg.br_limit_cali_req == 1)//�ƶ����
				{
					//�����˶���ֱ�����شﵽֹͣ
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
									(int16_t)(real_temp*BRK_INF_RE);//���ݴ洢���ṹ�����
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������


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
				else if(can_ctrl_msg.br_limit_cali_req == 5)  //�޸�ԭ���ϵ��Զ��������Ĳ������޸�Ϊ��λ���������Ĳ���
				{
					SCA_Set_Up_Home(BR_ID);
					SCA_Set_Up_Home(BR_ID);
					if(SCA_Set_Up_Home(BR_ID) == SCA_NoError)
					{

						general_replay_msg(&can0_tx_msg,
										   PRS_1_1,CTRL_0_1,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
											can0_send_msg(can0_tx_msg);	//CAN0��������

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
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x04);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7e:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x05);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7d:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x06);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7c:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x07);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7b:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x08);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7a:   //����BR�������������ķ���msg
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,BR_ID,RESULT_FAIL,0x09);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
					}
				}
			}
		}

		//����
		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				if(can_ctrl_msg.ar_limit_cali_req != 0)//���ٵ��
				{
					if(can_ctrl_msg.ar_limit_cali_req)//���ٵ���궨�ж�
					{
						if(SCA_Pos_Max_Limit(AR_ID,(float)(ar_info.angle/THR_INF_RE)) == SCA_NoError)
						{
							config_data.cali_data.ar_max_angle =
									ar_info.angle;//���ݴ洢���ṹ�����
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
						}
						else
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,AR_ID,RESULT_FAIL,0x09);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
						}
					}
					can_ctrl_msg.ar_limit_cali_req = 0;
				}
			}
		}

		//פ��
		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//פ������ѿ���
			{
				if(can_ctrl_msg.ep_limit_cali_req == 1)//פ�����
				{
					//�����˶���ֱ�����شﵽֹͣ
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
					real_temp = PSCA->Current_Real;    //��ǰ����ֵ
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
									real_temp*EP_INF_RE;//���ݴ洢���ṹ�����
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������

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
				else if(can_ctrl_msg.ep_limit_cali_req == 5) //�޸�ԭ��EP����Զ��ϵ����Ĳ�������Ϊ��λ���궨���İ汾
				{

					SCA_Set_Up_Home(EP_ID);
					SCA_Set_Up_Home(EP_ID);
					if(SCA_Set_Up_Home(EP_ID) == SCA_NoError)
					{
						can_ctrl_msg.ep_limit_cali_req  =  0 ;
						general_replay_msg(&can0_tx_msg,PRS_1_1,CTRL_0_1,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��can0_send_msg(can0_tx_msg);	//CAN0��������
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
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0A);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7e:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0B);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7d:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0C);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7c:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0D);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7b:
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0E);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
						case 0x7a:    //����EP����������ʧ�ܵķ��ز���
						{
							general_replay_msg(&can0_tx_msg,
									PRS_1_1,CTRL_0_1,EP_ID,RESULT_FAIL,0x0F);//��װͨ��Ӧ��
							can0_send_msg(can0_tx_msg);	//CAN0��������
							break;
						}
					}
				}
			}
		}

		//�궨������������д洢����
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

			//�رձ궨����
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
					//�رձ궨����
					can_ctrl_msg.robot_limit_cali_bo = false;
			   }
		}
	}
   // ���Զ��Ͱ�ȫģʽ   �������˶�������׼���ٶȼ��ٶȵ�
	if(can_ctrl_msg.robot_param_cali_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//ת�����ѿ���
			{
				if(can_ctrl_msg.sr_param_cali_req != 0)//�궨ת����
				{
					config_data.cali_data.sr_acc_dec =
							can_ctrl_msg.sr_acc_dec_cali;
					general_replay_msg(&can0_tx_msg,
							PRS_1_1,CTRL_0_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
					can0_send_msg(can0_tx_msg);	//CAN0��������
					can_ctrl_msg.sr_param_cali_req = 0;
				}
			}
		}

		//�ƶ�
		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				if(can_ctrl_msg.br_param_cali_req == 1)//�ƶ����
				{
					config_data.cali_data.br_speed =
							can_ctrl_msg.br_speed_cali;
					config_data.cali_data.br_acc_dec =
							can_ctrl_msg.br_acc_dec_cali;
					general_replay_msg(&can0_tx_msg,
							PRS_1_1,CTRL_0_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
					can0_send_msg(can0_tx_msg);	//CAN0��������
					can_ctrl_msg.br_param_cali_req = 0;
				}
			}
		}

		//����
		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				if(can_ctrl_msg.ar_param_cali_req != 0)//���ٵ��
				{
					config_data.cali_data.ar_speed =
							can_ctrl_msg.ar_speed_cali;
					config_data.cali_data.ar_acc_dec =
							can_ctrl_msg.ar_acc_dec_cali;
					general_replay_msg(&can0_tx_msg,
							PRS_1_1,CTRL_0_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
					can0_send_msg(can0_tx_msg);	//CAN0��������
					can_ctrl_msg.ar_param_cali_req = 0;
				}
			}
		}

		//�궨������������д洢����
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

			//�رձ궨����
			can_ctrl_msg.robot_param_cali_bo = false;
		}
	}

	if(can_ctrl_msg.cali_data_query_bo)
	{
		if(can_ctrl_msg.cali_data_query_req)
		{
			calibration_data_query_replay_msg(&can0_tx_msg,PRS_0_1,
					config_data.cali_data,device_info);//��װͨ��Ӧ��
			can0_send_msg(can0_tx_msg);	//CAN0��������

			calibration_data_query_replay_msg(&can0_tx_msg,PRS_0_2,
					config_data.cali_data,device_info);//��װͨ��Ӧ��
			can0_send_msg(can0_tx_msg);	//CAN0��������

			calibration_data_query_replay_msg(&can0_tx_msg,PRS_0_3,
					config_data.cali_data,device_info);//��װͨ��Ӧ��
			can0_send_msg(can0_tx_msg);	//CAN0��������

			//�رղ�ѯ����
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
						0x01,(int32_t)(sr_info.voltage * IQ10));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x01,(int32_t)(br_info.voltage * IQ10));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x01,(int32_t)(ar_info.voltage * IQ10));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x01,(int32_t)(ep_info.voltage * IQ10));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
		}

		if(can_ctrl_msg.motor_current_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x02,(int32_t)(sr_info.current * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x02,(int32_t)(br_info.current * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x02,(int32_t)(ar_info.current * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x02,(int32_t)(ep_info.current * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
		}

		if(can_ctrl_msg.motor_speed_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x03,(int32_t)(sr_info.speed * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x03,(int32_t)(br_info.speed * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x03,(int32_t)(ar_info.speed * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x03,(int32_t)(ep_info.speed * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
		}

		if(can_ctrl_msg.motor_pos_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,0x04,
						(int32_t)(sr_info.angle/STE_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,0x04,
						(int32_t)(br_info.angle/BRK_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,0x04,
						(int32_t)(ar_info.angle/THR_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,0x04,
						(int32_t)(ep_info.angle/EP_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
		}

		if(can_ctrl_msg.motor_temp_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x05,(int32_t)(sr_info.temperature * IQ8));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x05,(int32_t)(br_info.temperature * IQ8));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x05,(int32_t)(ar_info.temperature * IQ8));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x05,(int32_t)(ep_info.temperature * IQ8));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
		}

		if(can_ctrl_msg.motor_pos_max_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x06,(int32_t)(config_data.cali_data.sr_left_angle/
								STE_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x06,(int32_t)(config_data.cali_data.br_max_angle/
								BRK_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x06,(int32_t)(config_data.cali_data.ar_max_angle/
								THR_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x06,(int32_t)(config_data.cali_data.ep_max_angle/
								EP_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
		}

		if(can_ctrl_msg.motor_pos_min_query_req)
		{
			if(can_ctrl_msg.motor_info_query_req == SR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,SR_ID,
						0x07,(int32_t)(config_data.cali_data.sr_right_angle/
								STE_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == BR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,BR_ID,
						0x07,(int32_t)(config_data.cali_data.br_min_angle/
								BRK_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == AR_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,AR_ID,
						0x07,(int32_t)(config_data.cali_data.ar_min_angle/
								THR_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}

			if(can_ctrl_msg.motor_info_query_req == EP_ID)
			{
				actuator_parameter_reply_msg(&can0_tx_msg,PRS_0_5,EP_ID,
						0x07,(int32_t)(config_data.cali_data.ep_min_angle/
								EP_INF_RE * IQ24));//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
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
				PRS_1_1,CTRL_0_5,0x00,RESULT_OK,0x00);//��װͨ��Ӧ��
		can0_send_msg(can0_tx_msg);	//CAN0��������

		can_ctrl_msg.device_info_set_bo = false;
	}

	if(can_ctrl_msg.device_info_query_bo)
	{
		if(can_ctrl_msg.device_info_query_req)
		{
			device_info_reply_msg(&can0_tx_msg, PRS_0_6,device_info);//��װͨ��Ӧ��
			can0_send_msg(can0_tx_msg);	//CAN0��������

			device_info_reply_msg(&can0_tx_msg, PRS_0_7,device_info);//��װͨ��Ӧ��
			can0_send_msg(can0_tx_msg);	//CAN0��������

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
					PRS_1_1,CTRL_0_7,0x00,RESULT_OK,0x00);//��װͨ��Ӧ��
			can0_send_msg(can0_tx_msg);	//CAN0��������

			can_ctrl_msg.cali_data_clear_req = 0;
		}
		can_ctrl_msg.cali_data_clear_bo = false;
	}

	if(can_ctrl_msg.robot_disable_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(can_ctrl_msg.sr_disable_req)    /* ת������˽�������0-������ 1-���� */
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
			config_data.register_info.b.ep_use = 0;    // 0  ����ֹ   1 ��ֹ
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
				PRS_1_1,CTRL_0_8,0x00,RESULT_OK,0x00);//��װͨ��Ӧ��
		can0_send_msg(can0_tx_msg);	//CAN0��������

		can_ctrl_msg.robot_disable_bo = false;
	}

	if (can_ctrl_msg.motor_clearERR_bo)   // 0x109  ������ϱ���
	{
		if(config_data.register_info.b.sr_use && can_ctrl_msg.sr_clearERR_req)//ת����������&&����
		{
			//ʹ��������
			if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(SR_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,SR_ID,RESULT_FAIL,0x01);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			can_ctrl_msg.sr_clearERR_req = 0;
		}

		if(config_data.register_info.b.br_use && can_ctrl_msg.br_clearERR_req)//�ƶ����������&&����
		{
			//ʹ��������
			if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(BR_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,BR_ID,RESULT_FAIL,0x01);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			can_ctrl_msg.br_clearERR_req = 0;
		}

		if(config_data.register_info.b.ar_use && can_ctrl_msg.ar_clearERR_req)//���ŵ��������&&����
		{
			//ʹ��������
			if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(AR_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,AR_ID,RESULT_FAIL,0x01);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			can_ctrl_msg.ar_clearERR_req = 0;
		}

		if(config_data.register_info.b.ep_use && can_ctrl_msg.ep_clearERR_req)//פ�����������&&����
		{
			//ʹ��������
			if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
			{
				SCA_Clear_Alarm(EP_ID);
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			else
			{
				//reply msg
				general_replay_msg(&can0_tx_msg,
						PRS_1_1,CTRL_0_9,EP_ID,RESULT_FAIL,0x01);//��װͨ��Ӧ��
				can0_send_msg(can0_tx_msg);	//CAN0��������
			}
			can_ctrl_msg.ep_clearERR_req = 0;
		}
		can_ctrl_msg.motor_clearERR_bo = false;
	}

	if(can_ctrl_msg.robot_infoSwitch_bo && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(can_ctrl_msg.sr_infoSwitch_req)
		{
			config_data.register_info.b.sr_infoSwitch = 1;      //����ת�򿪹�
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
				PRS_1_1,CTRL_0_A,0x00,RESULT_OK,0x00);//��װͨ��Ӧ��
		can0_send_msg(can0_tx_msg);	//CAN0��������

		can_ctrl_msg.robot_infoSwitch_bo = false;
	}

	if(can_ctrl_msg.robot_open_ctrl_bo_1 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.self_check_st)//ת�������Լ�ɹ�
			{
				if(can_ctrl_msg.sr_open_ctrl_req_1 == 1)//����ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(SR_ID) == SCA_NoError)
					{
						sr_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_FAIL,0x0F);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.sr_open_ctrl_req_1 == 2)//������
				{
					//����������
					if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
					{
						sr_info.robot_st.b.motor_power_st = 1;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,SR_ID,RESULT_FAIL,0x10);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.sr_open_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				if(can_ctrl_msg.br_open_ctrl_req_1 == 1)//����ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(BR_ID) == SCA_NoError)
					{
						br_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_FAIL,0x11);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.br_open_ctrl_req_1 == 2)//����
				{
					//����������
					if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
					{
						br_info.robot_st.b.motor_power_st = 1;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,BR_ID,RESULT_FAIL,0x12);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.br_open_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				if(can_ctrl_msg.ar_open_ctrl_req_1 == 1)//����ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(AR_ID) == SCA_NoError)
					{
						ar_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_FAIL,0x13);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_open_ctrl_req_1 == 2)//������
				{
					ar_info.robot_st.b.motor_power_st = 1;
					//����������
					if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,AR_ID,RESULT_FAIL,0x14);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ar_open_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.self_check_st)//פ������ѿ���
			{
				if(can_ctrl_msg.ep_open_ctrl_req_1 == 1)//����ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(EP_ID) == SCA_NoError)
					{
						ep_info.robot_st.b.motor_power_st = 0;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_FAIL,0x15);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ep_open_ctrl_req_1 == 2)//������
				{
					//����������
					if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
					{
						ep_info.robot_st.b.motor_power_st = 1;
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_1,EP_ID,RESULT_FAIL,0x16);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ep_open_ctrl_req_1 = 0;
			}
		}
		can_ctrl_msg.robot_open_ctrl_bo_1 = false;
	}

	 // 0x112  ������ģʽ����
	if(can_ctrl_msg.robot_mode_ctrl_bo_1 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//ת�����ѿ���
			{
				if(can_ctrl_msg.sr_mode_ctrl_req_1 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_FAIL,0x17);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_1 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_FAIL,0x18);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_1 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,SR_ID,RESULT_FAIL,0x19);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.sr_mode_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				if(can_ctrl_msg.br_mode_ctrl_req_1 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(BR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_FAIL,0x20);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}

				}
				else if(can_ctrl_msg.br_mode_ctrl_req_1 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(BR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_FAIL,0x21);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.br_mode_ctrl_req_1 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(BR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,BR_ID,RESULT_FAIL,0x22);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.br_mode_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				if(can_ctrl_msg.ar_mode_ctrl_req_1 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(AR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_FAIL,0x23);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_1 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(AR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_FAIL,0x24);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_1 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(AR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,AR_ID,RESULT_FAIL,0x25);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ar_mode_ctrl_req_1 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//פ������ѿ���
			{
				if(can_ctrl_msg.ep_mode_ctrl_req_1 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(EP_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_FAIL,0x26);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_1 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_FAIL,0x27);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_1 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_1_2,EP_ID,RESULT_FAIL,0x28);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ep_mode_ctrl_req_1 = 0;
			}
		}

		can_ctrl_msg.robot_mode_ctrl_bo_1 = false;
	}

	if(can_ctrl_msg.robot_move_ctrl_bo_1)   //�Զ���ʻģʽ
	{
		//�Զ���ʻ״̬&&ͨ��1����
		if((sys_info.sys_st.b.sys_mode == 3||sys_info.sys_st.b.sys_mode == 4)
				&& sys_info.sys_st.b.sys_ctrl_st == 1)
		{
			sys_info.sys_st.b.auto_driving_st = can_ctrl_msg.auto_driving_ctrl_1;
			switch(sys_info.sys_st.b.auto_driving_st)
			{
				case 0: // �Զ���ʻԤ׼���׶Σ� ������ɲ��       /* �Զ���ʻ״̬��0-Ԥ�� 1-���� 2-ֹͣ 3-��ͣ */
				{
					if(can_ctrl_msg.br_request_st_1)//�ƶ��������
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
				case 1:	// �Զ���ʻ�׶�
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

						if(can_ctrl_msg.sr_request_st_1)//ת���������
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

						if(can_ctrl_msg.br_request_st_1)//�ƶ��������      br_max_angle == Ŀǰ���ĽǶ�Ϊ-782    br_angle_1Ϊ��ֵ
						{
							if(config_data.cali_data.br_max_angle_valid)
							{
								if(-(float)(can_ctrl_msg.br_angle_1) < config_data.cali_data.br_max_angle)  // ����ִ��
								{



									SCA_Move_Pos(BR_ID,
									(float)config_data.cali_data.br_max_angle / BRK_INF_RE,
									(float)(500),
									(float)(3000.00),(float)(-3000.00));



									#if   EP_BRAKE_SYMBOL   //EP�ƶ��ı�־

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



                                    #if   EP_BRAKE_SYMBOL    //˫Ч�ƶ��ı�׼

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



                               #if   EP_BRAKE_SYMBOL    //EP�ƶ��ı�־

								ep_info.angle   = can_ctrl_msg.br_angle_1 ;


								SCA_Move_Pos(EP_ID,
								(-(float)(ep_info.angle)
						        ) / EP_INF_RE,
								(float)(695),                       //695
								(float)(3000.00),(float)(-3000.00));//500-695/600-835

								#endif
							}
						}

						if(can_ctrl_msg.ar_request_st_1)//�����������
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
					{	// ͣ��
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


                       #if   EP_BRAKE_SYMBOL    //EP�ƶ��ı�־
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
				case 2:	// �Զ���ʻ��ɣ�ֹͣ�׶�
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
				case 3:	// �Զ���ʻ��;��ͣ�׶�
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

		if(can_ctrl_msg.sr_request_st_1)//ת���������
		{
			sr_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			sr_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.br_request_st_1)//�ƶ��������
		{
			br_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			br_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.ar_request_st_1)//�����������
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
			system_status_reply_msg(&can0_tx_msg,PRS_1_2,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
			actuator_status_reply_msg(&can0_tx_msg,PRS_1_5,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
		can_ctrl_msg.state_detail_query_bo_1 = false;
	}

	if(can_ctrl_msg.power_query_bo_1)
	{
		if(can_ctrl_msg.power_sys_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_6,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_sr_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_7,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_br_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_8,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_ar_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_9,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_ep_query_req_1)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_10,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
		can_ctrl_msg.power_query_bo_1 = false;
	}

	// 0x116ָ�����/�˳��Զ���ʻ
	if(can_ctrl_msg.ctrl_auto_driver_control_bo_1)
	{
		if(can_ctrl_msg.auto_driver_ctrl_1)    //	/* CTRL���Ʊ�־��0-�˹�ģʽ 1-�Զ���ʻ */
		{
			//�ֶ���ʻ״̬&&�Լ����&&ͨ��1����
			if(sys_info.sys_st.b.sys_mode == 2 &&
					sys_info.sys_st.b.self_check_st == 1 &&
					sys_info.sys_st.b.sys_ctrl_st == 1)
			{
				sr_info.robot_st.b.change_mode_st = 0;
				br_info.robot_st.b.change_mode_st = 0;
				ar_info.robot_st.b.change_mode_st = 0;
				ep_info.robot_st.b.change_mode_st = 0;
				sys_info.sys_st.b.auto_driving_st = 0;	// ��״̬��־�ɰ�������0x113ָ��ı�
				sys_info.sys_st.b.sys_mode = 3;
				sys_info.sys_st.b.take_over_st = 0;
				SW_LED = 1;//����
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
			SW_LED = 0;//���
		}
		can_ctrl_msg.ctrl_auto_driver_control_bo_1 = false;
	}

#if 0
	if(can_ctrl_msg.robot_open_ctrl_bo_2 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//ת�����ѿ���
			{
				if(can_ctrl_msg.sr_open_ctrl_req_2 == 1)//�ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(SR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_FAIL,0x0F);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.sr_open_ctrl_req_2 == 2)//����
				{
					//����������
					if(SCA_Open_Actuator(SR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,SR_ID,RESULT_FAIL,0x10);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.sr_open_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				if(can_ctrl_msg.br_open_ctrl_req_2 == 1)//�ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(BR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_FAIL,0x11);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.br_open_ctrl_req_2 == 2)//����
				{
					//����������
					if(SCA_Open_Actuator(BR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,BR_ID,RESULT_FAIL,0x12);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.br_open_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				if(can_ctrl_msg.ar_open_ctrl_req_2 == 1)//�ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(AR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_FAIL,0x13);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_open_ctrl_req_2 == 2)//����
				{
					//����������
					if(SCA_Open_Actuator(AR_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,AR_ID,RESULT_FAIL,0x14);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ar_open_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//פ������ѿ���
			{
				if(can_ctrl_msg.ep_open_ctrl_req_2 == 1)//�ر�
				{
					//�ر�������
					if(SCA_Close_Actuator(EP_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_FAIL,0x15);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_open_ctrl_req_1 == 2)//����
				{
					//����������
					if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_1,EP_ID,RESULT_FAIL,0x16);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ep_open_ctrl_req_2 = 0;
			}
		}

		can_ctrl_msg.robot_open_ctrl_bo_2 = false;
	}

	if(can_ctrl_msg.robot_mode_ctrl_bo_2 && (sys_info.sys_st.b.sys_mode != 3||sys_info.sys_st.b.sys_mode != 4))
	{
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//ת�����ѿ���
			{
				if(can_ctrl_msg.sr_mode_ctrl_req_2 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_FAIL,0x17);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_2 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_FAIL,0x18);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.sr_mode_ctrl_req_2 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,SR_ID,RESULT_FAIL,0x19);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.sr_mode_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				if(can_ctrl_msg.br_mode_ctrl_req_2 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(BR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_FAIL,0x20);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}

				}
				else if(can_ctrl_msg.br_mode_ctrl_req_2 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(BR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_FAIL,0x21);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.br_mode_ctrl_req_2 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(BR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,BR_ID,RESULT_FAIL,0x22);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				//�������
				can_ctrl_msg.br_mode_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				if(can_ctrl_msg.ar_mode_ctrl_req_2 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(AR_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_FAIL,0x23);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_2 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_FAIL,0x24);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ar_mode_ctrl_req_2 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,AR_ID,RESULT_FAIL,0x25);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ar_mode_ctrl_req_2 = 0;
			}
		}

		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//פ������ѿ���
			{
				if(can_ctrl_msg.ep_mode_ctrl_req_2 == 1)//����ģʽ
				{
					if(SCA_Mode_Actuator(EP_ID,0x01)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_FAIL,0x26);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_1 == 2)//λ��ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x06)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_FAIL,0x27);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}
				else if(can_ctrl_msg.ep_mode_ctrl_req_2 == 3)//�ٶ�ģʽ
				{
					if(SCA_Mode_Actuator(SR_ID,0x07)== SCA_NoError)
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_OK,0x00);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
					else
					{
						//reply msg
						general_replay_msg(&can0_tx_msg,
								PRS_1_1,CTRL_2_2,EP_ID,RESULT_FAIL,0x28);//��װͨ��Ӧ��
						can0_send_msg(can0_tx_msg);	//CAN0��������
					}
				}

				//�������
				can_ctrl_msg.ep_mode_ctrl_req_2 = 0;
			}
		}

		can_ctrl_msg.robot_mode_ctrl_bo_2 = false;
	}

	if(can_ctrl_msg.robot_move_ctrl_bo_2)
	{
		//�Զ���ʻ״̬&&�Լ����&&ͨ��2����
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

						if(can_ctrl_msg.sr_request_st_2)//ת���������
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

						if(can_ctrl_msg.br_request_st_2)//�ƶ��������
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

						if(can_ctrl_msg.ar_request_st_2)//�����������
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

		if(can_ctrl_msg.sr_request_st_2)//ת���������
		{
			sr_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			sr_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.br_request_st_2)//�ƶ��������
		{
			br_info.robot_st.b.auto_ctrl_st = 1;
		}
		else
		{
			br_info.robot_st.b.auto_ctrl_st = 0;
		}

		if(can_ctrl_msg.ar_request_st_2)//�����������
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
			system_status_reply_msg(&can0_tx_msg,PRS_1_2,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
			actuator_status_reply_msg(&can0_tx_msg,PRS_1_5,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
		can_ctrl_msg.state_detail_query_bo_2 = false;
	}

	if(can_ctrl_msg.power_query_bo_2)
	{
		if(can_ctrl_msg.power_sys_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_6,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_sr_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_7,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_br_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_8,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_ar_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_9,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if(can_ctrl_msg.power_ep_query_req_2)
		{
			power_reply_msg(&can0_tx_msg,
					PRS_1_10,s_data);//��װӦ����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
		can_ctrl_msg.power_query_bo_2 = false;
	}

	if(can_ctrl_msg.ctrl_auto_driver_control_bo_2)
	{
		if(can_ctrl_msg.auto_driver_ctrl_2)
		{
			//�ֶ���ʻ״̬&&�Լ����&&ͨ��2����
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
				SW_LED = 1;//����
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
			SW_LED = 0;//���
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
	//�ж��ⲿ����״̬
	if(s_data->pwr_msg.voltage_in > VOLTAGE_MIN)
	{
		check_info.in_power_state = 1;      //   0-�쳣  1-����
		sys_info.sys_fault.b.power = 0;     //  0-���� 1-�쳣
	}
	else
	{
		check_info.in_power_state = 0;
		sys_info.sys_fault.b.power = 1;
	}
}

void check_sensor(void)
{

	check_info.encoder_state = 1;    //0-�쳣  1-����
	sys_info.sys_fault.b.encoder = 0;
//	if(s_data->ep_info.encoder_value > ENCODER_MIN ||
//			s_data->ep_info.encoder_value < ENCODER_MAX)
//	{
//		check_info.encoder_state = 1;    //0-�쳣  1-����
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
	// ����ʹ��hmi��ʾ��������״̬����
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
		check_info.motor_power_state = 0;          /* �������״̬��0-����  1-���� */
		sys_info.sys_st.b.estop_st = 1;            /* ��ͣ״̬��0-�޼�ͣ  1-ESTOP���� 2-EBR���� */
		sys_info.sys_st.b.auto_driving_st = 0;     /* �Զ���ʻ״̬��0-Ԥ�� 1-���� 2-ֹͣ 3-��ͣ */
		sys_info.sys_st.b.sys_mode = 2;            /* ϵͳģʽ��1-�Լ� 2-�ֶ� 3-�Զ� 4-��ȫ 5-���� */
		sys_info.sys_st.b.take_over_st = 1;       	/* �ӹ�״̬��0-δ�ӹ� 1-�ѽӹ� */
		SW_LED = 0;//���
	}
	else
	{
		if(sr_info.voltage > VOLTAGE_MIN && sr_info.voltage < 45)
		{

			sr_info.robot_st.b.power_st = 0;  	/* ����״̬��0-���� 1-δ���� 2-�쳣 3-���� */
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

	//����ⲿ����״̬
	check_in_power();

	//���ű�����״̬������ʹ�ñ�������Ĭ�ϲ���ֱ������
	check_sensor();

	//���hmi״̬ , ����ʹ��hmi��Ĭ�ϲ�������
	check_hmi();

	//�ⲿ��������ʱ���ж�estop��ť�Ƿ���
	if(check_info.in_power_state)
	{
		//���������״̬
		check_motor_power();

		//����״̬�ɶϵ��Ϊ�ϵ�ʱ�����½����Լ����
		if(check_info.last_motor_power_state == 0 &&
				check_info.motor_power_state == 1)
		{
			//���½����Լ����
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
	//�Լ����
	if(check_info.check_state == 0)
	{
		//�����Լ�ģʽ
		sys_info.sys_st.b.sys_mode = 1;

		if(check_info.in_power_state)
		{
			if(check_info.motor_power_state)
			{
				//EPִ�����Լ����


				if(check_info.ep_state == 0 &&
						config_data.register_info.b.ep_use)
				{
					if(SCA_Open_Actuator(EP_ID) == SCA_NoError)
					{
						ep_info.robot_st.b.motor_power_st = 1;   ///* ����״̬��0-�ػ�  1-���� */
						check_info.ep_state = 2;    //0-����  1-����  >1-�Լ�ʧ��
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
					if(config_data.cali_data.ep_zero_valid == 0)   //ȥ��EP����ϵ��Զ����㣬�޸�Ϊ�ϵ��������Ч��
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
					if(SCA_Mode_Actuator(EP_ID,0x01) == SCA_NoError) /*�л�Ϊ����ģʽ*/
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
							ep_info.robot_st.b.self_check_st = 1;   //EP ִ���� �Լ�ɹ�
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
						check_info.ep_state = 1;   //�Լ����
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

						// br �����Լ�
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
								config_data.register_info.b.br_use)    //ȥ��BR����ϵ��Զ����㣬�޸�Ϊ�ϵ��������Ч��
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
							//SR ��ʼ�Լ�
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

								//�Լ����
								check_info.check_state = 1;
								sys_info.sys_st.b.self_check_st = 1;

								//�����ֶ�ģʽ
								sys_info.sys_st.b.sys_mode = 2;
							}
						}  //SR�Լ����
					} // BR
				} //AR
			}
			else   // ��������else ����ִ��
			{

				//estop�����쳣
				check_info.failure_info |= 0x410101;

				//�Լ����
				check_info.check_state = 1;
				sys_info.sys_st.b.self_check_st = 1;

				//�����ֶ�ģʽ
				sys_info.sys_st.b.sys_mode = 2;
			}
		} // ��������else ����ִ��
		else
		{
			//δ����״̬
			check_info.failure_info = 0x01000000;

			//�Լ����
			check_info.check_state = 1;
			sys_info.sys_st.b.self_check_st = 1;    // 0  ��ʼ�Լ�    1 �Լ����
			//�����ֶ�ģʽ
			sys_info.sys_st.b.sys_mode = 2;
		}
	}
	else
	{
		if(sys_info.sys_st.b.sys_mode == 2)    // �Լ������ 100ms ����һ��
		{
			//�Լ���ڴ���

			//CAN_communication_abnormal_flag = 0 ;   // �ֶ�ģʽ �£�can ͨ�Ź�����������������һֱ��
			if(check_info.failure_info != 0)
			{
				//�������ģʽ
				sys_info.sys_st.b.sys_mode = 5;
			}


			/*�����ϵ� �ұ궨�����Ч���ص���㡢�л�����ģʽ*/
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

			//ת��ģʽ���
			if(sr_info.robot_st.b.motor_ctrl_mode == 0x06
					&& sr_info.robot_st.b.change_mode_st)
			{
				//�л�����ģʽ
				SCA_Mode_Actuator(SR_ID,0x01);
				sr_info.robot_st.b.change_mode_st = 0;
			}

			if(br_info.robot_st.b.change_mode_st)
			{
				//̤���λ���
				if(br_info.angle >= -10)  // -10
				{
					//�л�����ģʽ
					SCA_Mode_Actuator(BR_ID,0x01);
					br_info.robot_st.b.change_mode_st = 0;
				}
				else
				{
					//�����
					SCA_Move_Pos(BR_ID, (float)(0/BRK_INF_RE), (float)500, (float)500, (float)-500);
					//�ȴ�3�룬��û�е����λ�ã�ֱ���г�

					if(br_info.limit_time_count > 30000)
					{
						br_info.robot_st.b.change_mode_st = 0;
						//�л�����ģʽ
						SCA_Mode_Actuator(BR_ID,0x01);
					}
				}
			}

			if(ar_info.robot_st.b.change_mode_st)
			{
				//̤���λ���
				if(ar_info.angle <= 10)
				{
					//�л�����ģʽ
					SCA_Mode_Actuator(AR_ID,0x01);
					ar_info.robot_st.b.change_mode_st = 0;
				}
				else
				{
					//�����
					SCA_Move_Pos(AR_ID, (float)(10/THR_INF_RE), (float)500, (float)500, (float)-500);
					//�ȴ�3�룬��û�е����λ�ã�ֱ���г�
					if(ar_info.limit_time_count > 30000)
					{
						ar_info.robot_st.b.change_mode_st = 0;
						//�л�����ģʽ
						SCA_Mode_Actuator(AR_ID,0x01);
					}
				}
			}

			if(ep_info.robot_st.b.change_mode_st)
			{
				//̤���λ���
				if(ep_info.angle >= -10)
				{
					//�л�����ģʽ
					SCA_Mode_Actuator(EP_ID,0x01);
					ep_info.robot_st.b.change_mode_st = 0;
				}
				else
				{
					//�����
					SCA_Move_Pos(EP_ID, (float)(0/EP_INF_RE), (float)500, (float)500, (float)-500);  // -10
					//�ȴ�3�룬��û�е����λ�ã�ֱ���г�
					if(ep_info.limit_time_count > 30000)
					{
						ep_info.robot_st.b.change_mode_st = 0;
						//�л�����ģʽ
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

// 100ms ִ��һ��
void process_beep_event(void)
{
	if(check_info.check_state == 0)
	{
		if(check_info.check_step == 0)
		{
			check_info.check_step += 1;
			// BEEP����һ��
			set_beep(1, 1000, 0);
			beep_switch(BEEP_ON);
		}
	}
	else if(check_info.check_state == 1)
	{
		if(check_info.check_step == 1 && check_info.failure_info == 0 )
		{
			// BEEP��������
			set_beep(2, 500, 0);
			beep_switch(BEEP_ON);
			check_info.check_step += 1;
		}
		else
		{
			if(check_info.failure_info != 0)
			{
				//BEEP����3��/����
				set_beep(3, 1000, 60000);
				if (get_beep_ctrl_state() == BEEP_OFF)
					beep_switch(BEEP_ON);
			}
			else
			{
				if(sys_info.sys_st.b.estop_st == 1)  	/* ��ͣ״̬��0-�޼�ͣ  1-ESTOP���� 2-EBR���� */
				{
					//BEEP����5��/����
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
				else if(sys_info.sys_st.b.sys_mode == 4)    //��ȫģʽ�쳹����
				{
					//BEEP����4��/2����
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
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)//ת�����ѿ���
			{
				SCA_Real_Attribute(SR_ID);
			}
		}

		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
			{
				SCA_Real_Attribute(BR_ID);
			}
		}

		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
			{
				SCA_Real_Attribute(AR_ID);
			}
		}

		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//פ������ѿ���
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
		if(config_data.register_info.b.sr_use)//ת������ʹ��
		{
			if(sr_info.robot_st.b.motor_power_st &&
					sr_info.robot_st.b.self_check_st)    //ת�����ѿ������Լ�ͨ��
			{

				PSCA = SCA_Handler_Update(1);
				sr_info.robot_st.b.motor_ctrl_mode = PSCA->Mode;
				sr_info.robot_st.b.motor_limit_st = PSCA->Position_Limit_State;
				sr_info.robot_st.b.motor_online_st = PSCA->Online_State;

				sr_info.angle = (int16_t)(PSCA->Position_Real*STE_INF_RE);
				sr_info.speed = PSCA->Velocity_Real / 10;	// ����ٶ�--> ת�����ٶȣ����ٱ�10��1
				sr_info.alarm_code = PSCA->SCA_Warn.Error_Code;
				sr_info.voltage = PSCA->Voltage;
				sr_info.current = PSCA->Current_Real;
				sr_info.power = sr_info.voltage * sr_info.current;
				sr_info.temperature = PSCA->Motor_Temp;
				sr_info.torque = PSCA->Current_Real * PSCA->Torque_Factor *10; // // ת��10:1 ��Ҫ0.1*10

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

		if(config_data.register_info.b.br_use)//�ƶ������ʹ��
		{
			if(br_info.robot_st.b.motor_power_st &&
					br_info.robot_st.b.self_check_st)//�ƶ�����ѿ���
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

		if(config_data.register_info.b.ar_use)//���ٵ����ʹ��
		{
			if(ar_info.robot_st.b.motor_power_st &&
					ar_info.robot_st.b.self_check_st)//���ٵ���ѿ���
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

		if(config_data.register_info.b.ep_use)//פ�������ʹ��
		{
			if(ep_info.robot_st.b.motor_power_st &&
					ep_info.robot_st.b.self_check_st)//פ������ѿ���
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
		actuator_position_msg(&can0_tx_msg,PRS_1_3,s_data);//��װӦ����Ϣ    0x193  ������λ����Ϣ�ϴ�
		can0_send_msg(can0_tx_msg);	//CAN0��������

		if (config_data.register_info.b.sr_infoSwitch)
		{
			actuator_CVP_msg(&can0_tx_msg,PRS_1_11, &sr_info);//��װӦ����Ϣ      0x19B    SR��Ϣ�ϴ�
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

		if (config_data.register_info.b.br_infoSwitch)
		{
			//��װӦ����Ϣ     0x19C    BR��Ϣ�ϴ�
			actuator_CVP_msg_ext(&can0_tx_msg,PRS_1_12, &br_info, s_data);
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
		if (config_data.register_info.b.ar_infoSwitch)
		{
			actuator_CVP_msg(&can0_tx_msg,PRS_1_13, &ar_info);//��װӦ����Ϣ        0x19D   AR��Ϣ�ϴ�
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
		if (config_data.register_info.b.brar_ratioSwitch)
		{
			actuator_Ratio_msg(&can0_tx_msg,PRS_1_14, s_data);//��װӦ����Ϣ       0x19E   AR,BR ���ٰٷֱ���Ϣ�ϴ�
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}

# if  0
		 // ������ 0x19F EP �ϱ���Ϣ   ��ʱ�Ȳ���
		if(config_data.register_info.b.ep_infoSwitch)
		{
		     actuator_CVP_EP_msg(&can0_tx_msg,PRS_1_15,&ep_info, s_data);//��װӦ����Ϣ       0x19F   EP��Ϣ�ϴ�
						can0_send_msg(can0_tx_msg);	//CAN0��������
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
			actuator_alarm_msg(&can0_tx_msg,PRS_1_4,s_data);//��װӦ����    0x194  �ϱ������˹�����Ϣ
			can0_send_msg(can0_tx_msg);	//CAN0��������
		}
	}
}

void upload_sys_status(void)
{
	if (check_info.check_state)
	{
		system_status_reply_msg(&can0_tx_msg,PRS_1_2,s_data);//��װӦ����Ϣ
		can0_send_msg(can0_tx_msg);	//CAN0��������
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
		//100hz����
		if(system_time.base_10ms_flag)
		{

			system_time.base_10ms_flag = false;
		}
		//50hz����
		if(system_time.base_20ms_flag)
		{

			multicore_share_data();

			//����CAN0/1����
			process_can0_1_rx_msg();


			if(!can_ctrl_msg.robot_move_ctrl_bo_1 && !can_ctrl_msg.robot_move_ctrl_bo_2)
			{
				//��ѯscaʵʱ����
				search_sca_real_attribute();
			}

			//����scaʵʱ����
			process_sca_real_attribute();

			//�ϴ�ִ����ʵʱλ������, CVP����
			upload_sca_real_position();

			// �ϴ�ϵͳ״̬��Ϣ
			upload_sys_status();   //�����˱���״̬��Ϣ�ϴ�    ֡ID 0x192

			system_time.base_20ms_flag = false;


		}

		//25hz����
		if(system_time.base_40ms_flag)
		{
			system_time.base_40ms_flag = false;

		}

		//20hz����
		if(system_time.base_50ms_flag)
		{
			system_time.base_50ms_flag = false;

		}

		//10hz����
		if(system_time.base_100ms_flag)
		{
			system_time.base_100ms_flag = false;

			/* �������Լ���� -- �����ϵ��Լ죬����ѭ���Լ�����ֶ�ģʽ����е��ģʽ�л������״̬ģʽ�л�
			 * ���Զ���ʻģʽ����������޵��������Ϣ�����Զ���ʻģʽ3��4֮���л�
			 */
			check_self();

			//���������¼�
			process_beep_event();

			//�����ж��¼�
			process_judge_event();

			//�ϴ�ִ����ʵʱ��������
			upload_sca_real_alarm();
		}

		//5hz����
		if(system_time.base_200ms_flag)
		{
			system_time.base_200ms_flag = false;

			//SWT0_Service();
		}

		//4hz����
		if(system_time.base_250ms_flag)
		{
			//SWT1_Service();
			system_time.base_250ms_flag = false;
		}

		//2hz����
		if(system_time.base_500ms_flag)
		{
			system_time.base_500ms_flag = false;
		}

		//1hz����
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

void can0_isr(void)      // CAN0  ����֡ID������can_0 ���յ�����
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
				case CTRL_0_1:         //0x101   ���������Ʊ궨  �궨λ�á� ����г�
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
					case CTRL_0_2:       //0x102 �������˶�������׼���궨�Ӽ���
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
					case CTRL_0_3:    //0x103   ��ѯ�궨����
					{
						can_ctrl_msg.cali_data_query_req =
										(can0_rx_msg.data[1]&0x01);
						can_ctrl_msg.live_counter_ctrl_0_3 =
										(can0_rx_msg.data[1]>>4)&0x0f;
						can_ctrl_msg.cali_data_query_bo = true;
					break;
					}
					case CTRL_0_4:    //0x104   �����˵��������ѯ
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
					case CTRL_0_7:  // 0x107  �����˱궨�������
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
					case  CTRL_1_7:       //����117Э��  ����EP��BR�����ת����
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
					case  CTRL_1_7:       //����117Э��  ����EP��BR�����ת����
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

