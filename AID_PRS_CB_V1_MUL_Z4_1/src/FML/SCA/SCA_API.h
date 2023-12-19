/**
  ******************************************************************************
  * @文	件 ： SCA_API.h
  * @作	者 ： INNFOS Software Team
  * @版	本 ： V1.5.3
  * @日	期 ： 2019.09.10
  * @摘	要 ： SCA 控制接口层
  ******************************************************************************/ 

#ifndef __SCA_API_H
#define __SCA_API_H

#include "SCA_Protocol.h"

extern DELAY_TIME delay_time;

/* 参数配置 */
#define SCA_NUM_USE		4			//驱动器数量为3，地址为分别为1：转向,2：制动,3：油门,4：驻车
#define CAN_OVER_TIME	25	 	    //数据阻塞超时（180MHZ），最多等待25ms
#define CAN_POWER_TIME	2000        //开关机阻塞超时（180MHZ），最多等待50ms
#define SEND_INTERVAL	10			//非阻塞时的指令发送间隔，最多等待500us

/* ！！！以下宏定义信息参数请勿修改！！！ */

//SCA状态定义
#define Actr_Enable		0x01
#define Actr_Disable	0x00

//通信方式定义
#define Block			0x01
#define Unblock			0x00

//SCA操作模式定义
#define SCA_Current_Mode			0x01
#define SCA_Velocity_Mode			0x02
#define SCA_Position_Mode			0x03
#define SCA_Profile_Position_Mode	0X06
#define SCA_Profile_Velocity_Mode	0X07
#define SCA_Homing_Mode				0X08

/***************延时相关******************/
void SCA_Delay(unsigned char x);

/***************解析相关******************/
void canMsgAnalysis(CAN_MSG* RxMsg);

/***************控制相关******************/
void setupActuators(uint8_t id, CAN_HANDLER* can);
void resetController(uint8_t id);
SCA_HANDLER* getInstance(uint8_t id);
uint8_t isOnline(uint8_t id, uint8_t isBlock);
uint8_t isEnable(uint8_t id, uint8_t isBlock);
uint8_t isUpdate(uint8_t id);
uint8_t enableActuator(uint8_t id);
uint8_t disableActuator(uint8_t id);
uint8_t activateActuatorMode(uint8_t id, uint8_t ActuatorMode, uint8_t isBlock);
uint8_t getActuatorMode(uint8_t id, uint8_t isBlock);
uint8_t getErrorCode(uint8_t id, uint8_t isBlock);
uint8_t clearError(uint8_t id, uint8_t isBlock);
uint8_t regainAttribute(uint8_t id,uint8_t isBlock);
uint8_t regainAttribute2(uint8_t id,uint8_t isBlock);
uint8_t regainAttribute3(uint8_t id,uint8_t isBlock);
uint8_t saveAllParams(uint8_t id, uint8_t isBlock);
uint8_t setHoming(uint8_t id);

/***************位置相关******************/
uint8_t setPosition(uint8_t id, float pos);
uint8_t setPositionFast(SCA_HANDLER* pSCA, float pos);
uint8_t getPosition(uint8_t id, uint8_t isBlock);
uint8_t getPositionFast(SCA_HANDLER* pSCA, uint8_t isBlock);
uint8_t setMaximumPosition(uint8_t id,float maxPos,uint8_t isBlock);
uint8_t getMaximumPosition(uint8_t id,uint8_t isBlock);
uint8_t setMinimumPosition(uint8_t id,float minPos,uint8_t isBlock);
uint8_t getMinimumPosition(uint8_t id,uint8_t isBlock);
uint8_t enablePositionLimit(uint8_t id, uint8_t enable,uint8_t isBlock);
uint8_t isPositionLimitEnable(uint8_t id,uint8_t isBlock);
uint8_t setHomingPosition(uint8_t id,float homingPos,uint8_t isBlock);
uint8_t clearHomingInfo(uint8_t id,uint8_t isBlock);
uint8_t setProfilePositionAcceleration(uint8_t id, float acceleration,uint8_t isBlock);
uint8_t getProfilePositionAcceleration(uint8_t id,uint8_t isBlock);
uint8_t setProfilePositionDeceleration(uint8_t id, float deceleration,uint8_t isBlock);
uint8_t getProfilePositionDeceleration(uint8_t id,uint8_t isBlock);
uint8_t setProfilePositionMaxVelocity(uint8_t id, float maxVelocity,uint8_t isBlock);
uint8_t getProfilePositionMaxVelocity(uint8_t id,uint8_t isBlock);

/***************速度相关******************/
uint8_t setVelocity(uint8_t id,float vel);
uint8_t setVelocityFast(SCA_HANDLER* pSCA,float vel);
uint8_t getVelocity(uint8_t id,uint8_t isBlock);
uint8_t getVelocityFast(SCA_HANDLER* pSCA,uint8_t isBlock);
float getVelocityRange(uint8_t id);
uint8_t setVelocityLimit(uint8_t id,float limit,uint8_t isBlock);
uint8_t getVelocityLimit(uint8_t id,uint8_t isBlock);

uint8_t setPVMaxVelocity(uint8_t id,float limit,uint8_t isBlock);
uint8_t setPVMaxAcc(uint8_t id,float limit,uint8_t isBlock);
uint8_t setPVMaxDec(uint8_t id,float limit,uint8_t isBlock);

/***************电流相关******************/
uint8_t setCurrent(uint8_t id,float current);
uint8_t setCurrentFast(SCA_HANDLER* pSCA,float current);
uint8_t getCurrent(uint8_t id,uint8_t isBlock);
uint8_t getCurrentFast(SCA_HANDLER* pSCA,uint8_t isBlock);
uint8_t getCurrentRange(uint8_t id,uint8_t isBlock);
uint8_t setCurrentLimit(uint8_t id,float limit,uint8_t isBlock);
uint8_t getCurrentLimit(uint8_t id,uint8_t isBlock);
uint8_t setCurrentFilterLimitL(uint8_t id,float limit,uint8_t isBlock);
uint8_t setCurrentFilterLimitH(uint8_t id,float limit,uint8_t isBlock);

/***************其他参数******************/
uint8_t getVoltage(uint8_t id,uint8_t isBlock);
uint8_t getLockEnergy(uint8_t id,uint8_t isBlock);
uint8_t setLockEnergy(uint8_t id,float energy,uint8_t isBlock);
uint8_t getActuatorSerialNumber(uint8_t id,uint8_t isBlock);
uint8_t getMotorTemperature(uint8_t id,uint8_t isBlock);
uint8_t getInverterTemperature(uint8_t id,uint8_t isBlock);
uint8_t getMotorProtectedTemperature(uint8_t id,uint8_t isBlock);
uint8_t setMotorProtectedTemperature(uint8_t id,float temp,uint8_t isBlock);
uint8_t getMotorRecoveryTemperature(uint8_t id,uint8_t isBlock);
uint8_t setMotorRecoveryTemperature(uint8_t id,float temp,uint8_t isBlock);
uint8_t getInverterProtectedTemperature(uint8_t id,uint8_t isBlock);
uint8_t setInverterProtectedTemperature(uint8_t id,float temp,uint8_t isBlock);
uint8_t getInverterRecoveryTemperature(uint8_t id,uint8_t isBlock);
uint8_t setInverterRecoveryTemperature(uint8_t id,float temp,uint8_t isBlock);
uint8_t setActuatorID(uint8_t currentID, uint8_t newID,uint8_t isBlock);
uint8_t getActuatorLastState(uint8_t id,uint8_t isBlock);
uint8_t requestCVPValue(uint8_t id,uint8_t isBlock);
uint8_t requestCVPValueFast(SCA_HANDLER* pSCA,uint8_t isBlock);

#endif
