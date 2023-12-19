#ifndef _CAN_MSG_API_H_
#define _CAN_MSG_API_H_

#include "flexcan.h"

uint8_t CheckSum(CAN_MSG* cmp);

//void ActuatorSystemStatusReplyMsg(CAN_MSG* cmp, uint16_t StdId,System_Msg_p sysmp);
//void ActuatorStatusReplyMsg(CAN_MSG* cmp, uint16_t StdId,Controller_Msg_p ctrmp);
//void ActuatorPositionReplyMsg(CAN_MSG* cmp, uint16_t StdId,Controller_Msg_p ctrmp);
void ActuatorParameterReplyMsg(CAN_MSG* cmp, uint16_t StdId, uint8_t id,uint16_t type,int32_t value);
//void VechileStatusReplayMsg(CAN_MSG* cmp, uint16_t StdId, Vehicle_Msg_t vmt);
void GeneralReplayMsg(CAN_MSG* cmp, uint16_t StdId, uint16_t CANStdId,uint8_t ActuatorID, uint8_t ExecuCmd, uint8_t ExecuState);
//void ActuatorWarnReplyMsg(CAN_MSG* cmp, uint16_t StdId,Controller_Msg_p ctrmp);
//void ActuatorPowerReplyMsg(CAN_MSG* cmp, uint16_t StdId, POWER_MSG* pmp);
//void ActuatorVoltageReplyMsg(CAN_MSG* cmp, uint16_t StdId, POWER_MSG* pmp);
//void ActuatorCurrentReplyMsg(CAN_MSG* cmp, uint16_t StdId, POWER_MSG* pmp);
void LoRaStatusReplyMsg(CAN_MSG* cmp, uint16_t StdId, uint8_t cmd);

#endif
