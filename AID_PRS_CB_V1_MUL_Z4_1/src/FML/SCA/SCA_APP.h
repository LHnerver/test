#ifndef __SCA_APP_H
#define __SCA_APP_H

/*
 * ����ͺš���������;˵����
	ID 		��;		�ͺ�				����ת��RPM	���������A		ת��ϵ��Nm/A		����W
	1		SR		DD-NU80			2200		33.0		0.1				500
	2		BR		QDD-PR60-36		116.67		33.0		1.91			199.82
	3		AR		QDD-NE30-36		166.67		16.5		1.35			75.55
	4		EP		QDD-PR60		4200.0		33.0		0.06			207.2
 * ����ͺŴ��� -36 ��ʾ������ٱ�Ϊ 36��1
 * SRת���е�ṹ���м��ٱ�Ϊ 10��1
 */
#include "SCA_API.h"

#define SR_ID		1
#define BR_ID 		2
#define AR_ID 		3
#define EP_ID 		4

// ���Ƶ��ת���Ƕ�  --> ���ִ��Ȧ��ת����������Ҫ���������ٱ�
#define THR_INF_RE	100.0
#define	STE_INF_RE	360.0
#define	BRK_INF_RE	100.0
#define GEAR_INF_RE	100.0
#define EP_INF_RE	72.0


void Add_SCA(CAN_HANDLER* CAN_Port, uint8_t id, Send_t send_func);//��ʼ������
SCA_HANDLER* SCA_Handler_Update(uint8_t id);
uint8_t SCA_Open_Actuator(uint8_t id);
uint8_t SCA_Close_Actuator(uint8_t id);
uint8_t SCA_Mode_Actuator(uint8_t id,uint8_t mode);
float SCA_Real_Velocity(uint8_t id);
float SCA_Real_Pos(uint8_t id);
uint8_t SCA_Move_Pos(uint8_t id,float pos,float spd,float acc,float dec);
uint8_t SCA_Real_Attribute(uint8_t id);
uint8_t SCA_Set_Up_Home(uint8_t id);
uint8_t SCA_Pos_Max_Limit(uint8_t id,float position);
uint8_t SCA_Pos_Min_Limit(uint8_t id,float position);

uint8_t SCA_Move_Profile_Vel(uint8_t id, float current_limit, float spd,
	float acc_dec);
uint8_t SCA_Clear_Alarm(uint8_t id);

/***************�������******************/
void SCA_CAN_Msg_Analysis(CAN_MSG* RxMsg);
  
#endif

