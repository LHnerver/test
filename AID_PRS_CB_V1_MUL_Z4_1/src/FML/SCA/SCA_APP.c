/* Includes ----------------------------------------------------------------------*/
#include "SCA_APP.h"
#include "flexcan.h"


/**
  * @��	��	��ʼ��SCA
  * @��	��	��
  * @��	��	��
  */
void Add_SCA(CAN_HANDLER* CAN_Port, uint8_t id, Send_t send_func)
{
	/* ��ʼ��CAN�˿ڲ��� */
	CAN_Port->CanPort = id;				//��Ƕ˿ں�
	CAN_Port->Retry = 3;				//�ط�����
	CAN_Port->Send = send_func;			//�˿ڷ��ͺ���
	setupActuators(id, CAN_Port);  		//ID1 ��CAN
}

SCA_HANDLER* SCA_Handler_Update(uint8_t id)
{
	/* ��ȡID�ľ�� */
	return  getInstance(id);
}

/**
  * @��	��	ִ������,����ʽ
  * @��	��	id��Ҫʹ�ܵ�ִ����ID
  * @��	��	SCA_NoError�������ɹ�
  *			����ͨ�Ŵ���μ� SCA_Error �����б�
  */
uint8_t SCA_Open_Actuator(uint8_t id)
{
	return enableActuator(id);
}


/**
  * @��	��	ִ�����ر�,����ʽ
  * @��	��	id��Ҫʹ�ܵ�ִ����ID
  * @��	��	SCA_NoError�������ɹ�
  *			����ͨ�Ŵ���μ� SCA_Error �����б�
  */
uint8_t SCA_Close_Actuator(uint8_t id)
{
	return disableActuator(id);
}

uint8_t SCA_Mode_Actuator(uint8_t id,uint8_t mode)
{
	uint8_t Error;
	Error = getActuatorMode(id,Block);
	if(Error)	return Error;
	/* �л�ִ��������ģʽ */
	Error = activateActuatorMode(id,mode,Block);
	return Error;
}

float SCA_Real_Velocity(uint8_t id)
{
	uint8_t Error;
	float velocity;
	SCA_HANDLER* pSCA = getInstance(id);
	if(pSCA == NULL)	return SCA_UnknownID;
	switch(id)
	{
		case 1:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = getVelocity(0x01,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
		case 2:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = getVelocity(0x02,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
		case 3:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = getVelocity(0x03,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
		case 4:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = getVelocity(0x04,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
	}
	return velocity;
}

float SCA_Real_Pos(uint8_t id)
{
	uint8_t Error;
	float position;
	SCA_HANDLER* pSCA = getInstance(id);
	if(pSCA == NULL)	return SCA_UnknownID;
	switch(id)
	{
		case 1:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = getPosition(0x01,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
		case 2:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = getPosition(0x02,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
		case 3:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = getPosition(0x03,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
		case 4:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = getPosition(0x04,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
	}
	return position;
}

uint8_t SCA_Move_Pos(uint8_t id,float pos,float spd,float acc,float dec)
{
	uint8_t Error;
	switch(id)
	{
		case 1:/* 1��ִ�����˶� */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x01, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x01,SCA_Profile_Position_Mode,Block); /* �л�ִ��������ģʽ������λ��ģʽ */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x01,spd,Unblock);/* �ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x01,acc,Unblock);/* ���ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x01,dec,Unblock);/* ���ٶȲ���*/
			if(Error)	return Error;
			Error = setPosition(0x01,pos); /* λ��ִ�� */
			break;
		case 2:/* 2��ִ�����˶� */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x02, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x02,SCA_Profile_Position_Mode,Block); /* �л�ִ��������ģʽ������λ��ģʽ */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x02,spd,Unblock);/* �ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x02,acc,Unblock);/* ���ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x02,dec,Unblock);/* ���ٶȲ���*/
			if(Error)	return Error;
			Error = setPosition(0x02,pos); /* λ��ִ�� */
			break;
		case 3:/* 3��ִ�����˶� */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x03, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x03,SCA_Profile_Position_Mode,Block); /* �л�ִ��������ģʽ������λ��ģʽ */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x03,spd,Unblock);/* �ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x03,acc,Unblock);/* ���ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x03,dec,Unblock);/* ���ٶȲ���*/
			if(Error)	return Error;
			Error = setPosition(0x03,pos); /* λ��ִ�� */
			break;
		case 4:/* 4��ִ�����˶� */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x04, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x04,SCA_Profile_Position_Mode,Block); /* �л�ִ��������ģʽ������λ��ģʽ */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x04,spd,Unblock);/* �ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x04,acc,Unblock);/* ���ٶȲ��� */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x04,dec,Unblock);/* ���ٶȲ���*/
			if(Error)	return Error;
			Error = setPosition(0x04,pos); /* λ��ִ�� */
			break;
	}
	return Error;
}

uint8_t SCA_Real_Attribute(uint8_t id)
{
	uint8_t Error;
	switch(id)
	{
		case 1:/* 1��ִ�����˶� */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x01,Unblock); /* λ�û�ȡ */
			break;
		case 2:/* 2��ִ�����˶� */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x02,Unblock); /* λ�û�ȡ */
			break;
		case 3:/* 3��ִ�����˶� */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x03,Unblock); /* λ�û�ȡ */
			break;
		case 4:/* 4��ִ�����˶� */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x04,Unblock); /* λ�û�ȡ */
			break;
	}
	return Error;
}

/**
  * @��	��	�������λ��
  * @��	��	uint8_t id
  * @��	��	SCA_NoError�������ɹ�
  *			����ͨ�Ŵ���μ� SCA_Error �����б�
  */
uint8_t SCA_Set_Up_Home(uint8_t id)
{
	return setHoming(id);
}

uint8_t SCA_Pos_Max_Limit(uint8_t id,float position)
{
	uint8_t Error;
	SCA_HANDLER* pSCA = getInstance(id);
	if(pSCA == NULL)	return SCA_UnknownID;
	switch(id)
	{
		case 1:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x01,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x01,Block);
			break;
		case 2:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x02,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x02,Block);
			break;
		case 3:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x03,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x03,Block);
			break;
		case 4:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x04,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x03,Block);
			break;
	}
	return Error;
}

uint8_t SCA_Pos_Min_Limit(uint8_t id,float position)
{
	uint8_t Error;
	SCA_HANDLER* pSCA = getInstance(id);
	if(pSCA == NULL)	return SCA_UnknownID;
	switch(id)
	{
		case 1:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x01,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x01,Block);
			break;
		case 2:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x02,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x02,Block);
			break;
		case 3:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x03,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x03,Block);
			break;
		case 4:
			/* δ����ֱ���˳�  */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x04,position,Block);
			//if(Error)	return Error;
			/* ִ�б���ָ��  */
			//Error = saveAllParams(0x03,Block);
			break;
	}
	return Error;
}

uint8_t SCA_Move_Profile_Vel(uint8_t id, float current_limit, float spd,
	float acc_dec)
{
	uint8_t Error;
	switch(id)
	{
		case 1:/* 1��ִ�����˶� */
			Error = isEnable(0x01,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x01, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x01,SCA_Profile_Velocity_Mode,Block); /* �л�ִ��������ģʽ���ٶ�ģʽ */
			if(Error) return Error;
			Error = setCurrentFilterLimitL(0x01,-current_limit,Block);
			if(Error) return Error;
			Error = setCurrentFilterLimitH(0x01,current_limit,Block);
			if(Error) return Error;
			Error = setPVMaxAcc(0x01,acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxDec(0x01,-acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxVelocity(0x01,ABS(spd),Block);
			if(Error)	return Error;
			Error = setVelocity(0x01,spd);  /* �ٶ�ִ�� */
			break;
		case 2:/* 2��ִ�����˶� */
			Error = isEnable(0x02,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x02, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x02,SCA_Profile_Velocity_Mode,Block); /* �л�ִ��������ģʽ���ٶ�ģʽ */
			if(Error) return Error;
			Error = setCurrentFilterLimitL(0x02,-current_limit,Block);
			if(Error) return Error;
			Error = setCurrentFilterLimitH(0x02,current_limit,Block);
			if(Error) return Error;
			Error = setPVMaxAcc(0x02,acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxDec(0x02,-acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxVelocity(0x02,ABS(spd),Block);
			if(Error)	return Error;
			Error = setVelocity(0x02,spd);  /* �ٶ�ִ�� */
			break;
		case 3:/* 3��ִ�����˶� */
			Error = isEnable(0x03,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x03, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x03,SCA_Profile_Velocity_Mode,Block); /* �л�ִ��������ģʽ���ٶ�ģʽ */
			if(Error) return Error;
			Error = setCurrentFilterLimitL(0x03,-current_limit,Block);
			if(Error) return Error;
			Error = setCurrentFilterLimitH(0x03,current_limit,Block);
			if(Error) return Error;
			Error = setPVMaxAcc(0x03,acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxDec(0x03,-acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxVelocity(0x03,ABS(spd),Block);
			if(Error)	return Error;
			Error = setVelocity(0x03,spd);  /* �ٶ�ִ�� */
			break;
		case 4:/* 4��ִ�����˶� */
			Error = isEnable(0x04,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x04, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x04,SCA_Profile_Velocity_Mode,Block); /* �л�ִ��������ģʽ���ٶ�ģʽ */
			if(Error) return Error;
			Error = setCurrentFilterLimitL(0x04,-current_limit,Block);
			if(Error) return Error;
			Error = setCurrentFilterLimitH(0x04,current_limit,Block);
			if(Error) return Error;
			Error = setPVMaxAcc(0x04,acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxDec(0x04,-acc_dec,Block);
			if(Error)	return Error;
			Error = setPVMaxVelocity(0x04,ABS(spd),Block);
			if(Error)	return Error;
			Error = setVelocity(0x04,spd);  /* �ٶ�ִ�� */
			break;
	}
	return Error;
}

//�������
uint8_t SCA_Clear_Alarm(uint8_t id)
{
	uint8_t Error = 0;

	Error = isEnable(id,Block);	/* �ж��Ƿ�ʹ�� */
	if(Error)	return Error;
	Error = clearError(id,Block); /* �������ָ�� */
	return Error;
}

/***************�������******************/
void SCA_CAN_Msg_Analysis(CAN_MSG* RxMsg)
{
	canMsgAnalysis(RxMsg);
}