/* Includes ----------------------------------------------------------------------*/
#include "SCA_APP.h"
#include "flexcan.h"


/**
  * @功	能	初始化SCA
  * @参	数	无
  * @返	回	无
  */
void Add_SCA(CAN_HANDLER* CAN_Port, uint8_t id, Send_t send_func)
{
	/* 初始化CAN端口参数 */
	CAN_Port->CanPort = id;				//标记端口号
	CAN_Port->Retry = 3;				//重发次数
	CAN_Port->Send = send_func;			//端口发送函数
	setupActuators(id, CAN_Port);  		//ID1 绑定CAN
}

SCA_HANDLER* SCA_Handler_Update(uint8_t id)
{
	/* 获取ID的句柄 */
	return  getInstance(id);
}

/**
  * @功	能	执行器打开,阻塞式
  * @参	数	id：要使能的执行器ID
  * @返	回	SCA_NoError：操作成功
  *			其他通信错误参见 SCA_Error 错误列表
  */
uint8_t SCA_Open_Actuator(uint8_t id)
{
	return enableActuator(id);
}


/**
  * @功	能	执行器关闭,阻塞式
  * @参	数	id：要使能的执行器ID
  * @返	回	SCA_NoError：操作成功
  *			其他通信错误参见 SCA_Error 错误列表
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
	/* 切换执行器操作模式 */
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
			/* 未开机直接退出  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = getVelocity(0x01,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
		case 2:
			/* 未开机直接退出  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = getVelocity(0x02,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
		case 3:
			/* 未开机直接退出  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = getVelocity(0x03,Block);
			if(Error)	return Error;
			velocity = pSCA->Velocity_Real;
			break;
		case 4:
			/* 未开机直接退出  */
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
			/* 未开机直接退出  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = getPosition(0x01,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
		case 2:
			/* 未开机直接退出  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = getPosition(0x02,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
		case 3:
			/* 未开机直接退出  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = getPosition(0x03,Block);
			if(Error)	return Error;
			position = pSCA->Position_Real;
			break;
		case 4:
			/* 未开机直接退出  */
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
		case 1:/* 1号执行器运动 */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x01, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x01,SCA_Profile_Position_Mode,Block); /* 切换执行器操作模式到梯形位置模式 */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x01,spd,Unblock);/* 速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x01,acc,Unblock);/* 加速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x01,dec,Unblock);/* 减速度参数*/
			if(Error)	return Error;
			Error = setPosition(0x01,pos); /* 位置执行 */
			break;
		case 2:/* 2号执行器运动 */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x02, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x02,SCA_Profile_Position_Mode,Block); /* 切换执行器操作模式到梯形位置模式 */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x02,spd,Unblock);/* 速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x02,acc,Unblock);/* 加速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x02,dec,Unblock);/* 减速度参数*/
			if(Error)	return Error;
			Error = setPosition(0x02,pos); /* 位置执行 */
			break;
		case 3:/* 3号执行器运动 */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x03, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x03,SCA_Profile_Position_Mode,Block); /* 切换执行器操作模式到梯形位置模式 */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x03,spd,Unblock);/* 速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x03,acc,Unblock);/* 加速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x03,dec,Unblock);/* 减速度参数*/
			if(Error)	return Error;
			Error = setPosition(0x03,pos); /* 位置执行 */
			break;
		case 4:/* 4号执行器运动 */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = enablePositionLimit(0x04, Actr_Enable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x04,SCA_Profile_Position_Mode,Block); /* 切换执行器操作模式到梯形位置模式 */
			if(Error)	return Error;
			Error = setProfilePositionMaxVelocity(0x04,spd,Unblock);/* 速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionAcceleration(0x04,acc,Unblock);/* 加速度参数 */
			if(Error)	return Error;
			Error = setProfilePositionDeceleration(0x04,dec,Unblock);/* 减速度参数*/
			if(Error)	return Error;
			Error = setPosition(0x04,pos); /* 位置执行 */
			break;
	}
	return Error;
}

uint8_t SCA_Real_Attribute(uint8_t id)
{
	uint8_t Error;
	switch(id)
	{
		case 1:/* 1号执行器运动 */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x01,Unblock); /* 位置获取 */
			break;
		case 2:/* 2号执行器运动 */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x02,Unblock); /* 位置获取 */
			break;
		case 3:/* 3号执行器运动 */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x03,Unblock); /* 位置获取 */
			break;
		case 4:/* 4号执行器运动 */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = regainAttribute2(0x04,Unblock); /* 位置获取 */
			break;
	}
	return Error;
}

/**
  * @功	能	设置零点位置
  * @参	数	uint8_t id
  * @返	回	SCA_NoError：操作成功
  *			其他通信错误参见 SCA_Error 错误列表
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
			/* 未开机直接退出  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x01,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
			//Error = saveAllParams(0x01,Block);
			break;
		case 2:
			/* 未开机直接退出  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x02,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
			//Error = saveAllParams(0x02,Block);
			break;
		case 3:
			/* 未开机直接退出  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x03,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
			//Error = saveAllParams(0x03,Block);
			break;
		case 4:
			/* 未开机直接退出  */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = setMaximumPosition(0x04,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
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
			/* 未开机直接退出  */
			Error = isEnable(0x01,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x01,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
			//Error = saveAllParams(0x01,Block);
			break;
		case 2:
			/* 未开机直接退出  */
			Error = isEnable(0x02,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x02,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
			//Error = saveAllParams(0x02,Block);
			break;
		case 3:
			/* 未开机直接退出  */
			Error = isEnable(0x03,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x03,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
			//Error = saveAllParams(0x03,Block);
			break;
		case 4:
			/* 未开机直接退出  */
			Error = isEnable(0x04,Block);
			if(Error)	return Error;
			Error = setMinimumPosition(0x04,position,Block);
			//if(Error)	return Error;
			/* 执行保存指令  */
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
		case 1:/* 1号执行器运动 */
			Error = isEnable(0x01,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x01, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x01,SCA_Profile_Velocity_Mode,Block); /* 切换执行器操作模式到速度模式 */
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
			Error = setVelocity(0x01,spd);  /* 速度执行 */
			break;
		case 2:/* 2号执行器运动 */
			Error = isEnable(0x02,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x02, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x02,SCA_Profile_Velocity_Mode,Block); /* 切换执行器操作模式到速度模式 */
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
			Error = setVelocity(0x02,spd);  /* 速度执行 */
			break;
		case 3:/* 3号执行器运动 */
			Error = isEnable(0x03,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x03, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x03,SCA_Profile_Velocity_Mode,Block); /* 切换执行器操作模式到速度模式 */
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
			Error = setVelocity(0x03,spd);  /* 速度执行 */
			break;
		case 4:/* 4号执行器运动 */
			Error = isEnable(0x04,Block);
			if(Error) return Error;
			Error = enablePositionLimit(0x04, Actr_Disable,Block);
			if(Error)	return Error;
			Error = activateActuatorMode(0x04,SCA_Profile_Velocity_Mode,Block); /* 切换执行器操作模式到速度模式 */
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
			Error = setVelocity(0x04,spd);  /* 速度执行 */
			break;
	}
	return Error;
}

//清除报警
uint8_t SCA_Clear_Alarm(uint8_t id)
{
	uint8_t Error = 0;

	Error = isEnable(id,Block);	/* 判定是否使能 */
	if(Error)	return Error;
	Error = clearError(id,Block); /* 清除报警指令 */
	return Error;
}

/***************解析相关******************/
void SCA_CAN_Msg_Analysis(CAN_MSG* RxMsg)
{
	canMsgAnalysis(RxMsg);
}
