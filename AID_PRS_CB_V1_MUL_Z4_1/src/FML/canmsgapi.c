#include "canmsgapi.h"

uint8_t CheckSum(CAN_MSG* cmp)
{
	uint8_t checkSum, i;
	checkSum = 0;
	for (i = 1; i < cmp->length; i++)
		checkSum ^= cmp->data[i];
	return checkSum;
}

//void ActuatorSystemStatusReplyMsg(CAN_MSG* cmp, uint16_t StdId,
//	System_Msg_p sysmp)
//{
//	static uint8_t ct_0 = 0;
//	//帧ID
//	cmp->std_id = StdId;
//	cmp->data[1] = ((sysmp->AutoDrivingState & 0x03) << 6)
//		| ((sysmp->ZeroRealState&0x01) << 5)
//		| ((sysmp->RobotSelfCheckFailureState & 0x07) << 1)
//		| (sysmp->RobotSelfCheckState);
//	cmp->data[2] = ((sysmp->AutoDrivingControlPassageState & 0x03) << 6)
//		| ((sysmp->EBR_Sign & 0x01) << 4)
//		| ((sysmp->ESTOP_Sign & 0x01) << 3)
//		| (sysmp->ActuatorOpenState & 0x07);
//	cmp->data[3] = ((sysmp->SystemRunModelState & 0x0F) << 4)
//		| (((sysmp->Power_OUT3_Sign & 0x01) << 3)
//		| ((sysmp->Power_OUT2_Sign & 0x01) << 2)
//		| ((sysmp->Power_OUT1_Sign & 0x01) << 1)
//		| (sysmp->Power_IN_Sign & 0x01));
//	cmp->data[4] = (((sysmp->AutoDrivingFailureState >> 4) & 0x1f) << 3)
//		| (sysmp->AutoDrivingControlFailureState & 0x03);
//	cmp->data[5] = (((ct_0++) & 0x0f) << 4)
//		| (sysmp->AutoDrivingFailureState & 0x0f);
//	if(ct_0 > 15) ct_0 = 0x00;
//	//数据长度
//	cmp->length = 6;
//	cmp->rtr = false;
//	//优先等级
//	cmp->prty = 8;
//	cmp->data[0] = CheckSum(cmp);
//}
//
//void ActuatorStatusReplyMsg(CAN_MSG* cmp, uint16_t StdId,
//	Controller_Msg_p ctrmp)
//{
//	static uint8_t ct_1 = 0;
//	//帧ID
//	cmp->std_id = StdId;
//	cmp->data[1] = ((ctrmp->AccelerationAutoControlState & 0x01) << 7)
//		| ((ctrmp->AccelerationPosLimitState & 0x01) << 6)
//		| ((ctrmp->AccelerationPowerState & 0x01) << 5)
//		| ((ctrmp->AccelerationMode & 0x0f) << 1)
//		| (ctrmp->AccelerationOnlineState & 0x01);
//	cmp->data[2] = ((ctrmp->DecelerationAutoControlState & 0x01) << 7)
//		| ((ctrmp->DecelerationPosLimitState & 0x01) << 6)
//		| ((ctrmp->DecelerationPowerState & 0x01) << 5)
//		| ((ctrmp->DecelerationMode & 0x0f) << 1)
//		| (ctrmp->DecelerationOnlineState & 0x01);
//	cmp->data[3] = ((ctrmp->SteeringAutoControlState & 0x01) << 7)
//		| ((ctrmp->SteeringPosLimitState & 0x01) << 6)
//		| ((ctrmp->SteeringPowerState & 0x01) << 5)
//		| ((ctrmp->SteeringMode & 0x0f) << 1)
//		| (ctrmp->SteeringOnlineState & 0x01);
//	cmp->data[4] = ((ct_1++) & 0x0f);
//	if(ct_1 > 15) ct_1 = 0x00;
//	//数据长度
//	cmp->length = 5;
//	cmp->rtr = false;
//	//优先等级
//	cmp->prty = 8;
//	cmp->data[0] = CheckSum(cmp);
//}
//
//void ActuatorPositionReplyMsg(CAN_MSG* cmp, uint16_t StdId,
//	Controller_Msg_p ctrmp)
//{
//	static uint8_t ct_2 = 0;
//	//帧ID
//	cmp->std_id = StdId;
//	cmp->data[1] = (((ctrmp->SteeringAngle >> 8) & 0x3f) << 2)
//		| (ctrmp->SteeringAngleSign & 0x03);
//	cmp->data[2] = ctrmp->SteeringAngle & 0xff;
//	cmp->data[3] = ((ctrmp->DecelerationAngle >> 4) & 0x3f)
//		| (ctrmp->DecelerationAngleSign & 0x03);
//	cmp->data[4] = (((ctrmp->AccelerationAngle >> 8) & 0x03) << 6)
//		| (ctrmp->AccelerationAngleSign << 2)
//		| (ctrmp->DecelerationAngle & 0x0f);
//	cmp->data[5] = (ctrmp->AccelerationAngle & 0xff);
//	cmp->data[6] = ((ct_2++) & 0x0f);
//	if(ct_2 > 15) ct_2 = 0x00;
//	//数据长度
//	cmp->length = 7;
//	cmp->rtr = false;
//	//优先等级
//	cmp->prty = 8;
//	cmp->data[0] = CheckSum(cmp);
//}

void ActuatorParameterReplyMsg(CAN_MSG* cmp, uint16_t StdId, uint8_t id,
	uint16_t type, int32_t value)
{
	static uint8_t ct_3 = 0;
	//帧ID
	cmp->std_id = StdId;
	cmp->data[1] = (((type>>8) & 0x1f) << 3) | (id & 0x07);
	cmp->data[2] = type & 0xff;
	cmp->data[3] = value >> 24;
	cmp->data[4] = value >> 16;
	cmp->data[5] = value >> 8;
	cmp->data[6] = value & 0xff;
	cmp->data[7] = (ct_3++) & 0x0f;
	if(ct_3 > 15) ct_3 = 0x00;
	//数据长度
	cmp->length = 8;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = CheckSum(cmp);
}

//void VechileStatusReplayMsg(CAN_MSG* cmp, uint16_t StdId, Vehicle_Msg_t vmt)
//{
//	static uint8_t ct_4 = 0;
//	//帧ID
//	cmp->std_id = StdId;
//	cmp->data[1] = ((vmt.TurningLightState & 0x03) << 6)
//		| ((vmt.GearState & 0x0f) <<2 ) | (vmt.EPBState & 0x03);
//	cmp->data[2] = (((ct_4++) & 0x0f) << 4)
//		| ((vmt.HeadLightBeamState & 0x03) << 2) | (vmt.FogLampState & 0x03);
//	if(ct_4 > 15) ct_4 = 0x00;
//	//数据长度
//	cmp->length = 2;
//	cmp->rtr = false;
//	//优先等级
//	cmp->prty = 8;
//	cmp->data[0] = CheckSum(cmp);
//}

void GeneralReplayMsg(CAN_MSG* cmp, uint16_t StdId, uint16_t CANStdId,
	uint8_t ActuatorID, uint8_t ExecuCmd, uint8_t ExecuState)
{
	static uint8_t ct_5 = 0;
	//帧ID
	cmp->std_id = StdId;
	cmp->data[1] = ((CANStdId>>3) & 0xff);
	cmp->data[2] = ((ExecuCmd&0x03) << 6) | ((ActuatorID & 0x07) << 3)
		| (CANStdId & 0x07);
	cmp->data[3] = (((ct_5++) & 0x0f) << 2) | (ExecuState & 0x03);
	if(ct_5 > 15) ct_5 = 0x00;

	//数据长度
	cmp->length = 4;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = CheckSum(cmp);
}

//void ActuatorWarnReplyMsg(CAN_MSG* cmp, uint16_t StdId, Controller_Msg_p ctrmp)
//{
//	static uint8_t ct_6 = 0;
//	//帧ID
//	cmp->std_id = StdId;
//	cmp->data[1] = ctrmp->AccelerationWarn >> 8;
//	cmp->data[2] = ctrmp->AccelerationWarn & 0xff;
//	cmp->data[3] = ctrmp->DecelerationWarn >> 8;
//	cmp->data[4] = ctrmp->DecelerationWarn & 0xff;
//	cmp->data[5] = ctrmp->SteeringWarn >> 8;
//	cmp->data[6] = ctrmp->SteeringWarn & 0xff;
//	cmp->data[7] = ((ct_6++) & 0x0f);
//	if(ct_6 > 15) ct_6 = 0x00;
//	//数据长度
//	cmp->length = 8;
//	cmp->rtr = false;
//	//优先等级
//	cmp->prty = 8;
//	cmp->data[0] = CheckSum(cmp);
//}
//
//void ActuatorPowerReplyMsg(CAN_MSG* cmp, uint16_t StdId, POWER_MSG* pmp)
//{
//	static uint8_t ct_7 = 0;
//	//帧ID
//	cmp->std_id = StdId;
//	cmp->data[1] = (((unsigned short)(pmp->Watt_IN * 10)) >> 6) & 0xff;
//	cmp->data[2] = ((((unsigned short)(pmp->Watt_OUT3*10)) >> 9) & 0x03)
//		| (((unsigned short)(pmp->Watt_IN * 10)) & 0x3f);
//	cmp->data[3] = ((((unsigned short)(pmp->Watt_OUT3 * 10)) >> 1) & 0xff);
//	cmp->data[4] = (((unsigned short)(pmp->Watt_OUT2 * 10) >> 6) & 0x7f)
//		| ((unsigned short)(pmp->Watt_OUT3 * 10) & 0x01);
//	cmp->data[5] = (((unsigned short)(pmp->Watt_OUT1 * 10) >> 9) & 0x03)
//		| ((unsigned short)(pmp->Watt_OUT2 * 10) & 0x3f);
//	cmp->data[6] = ((((unsigned short)(pmp->Watt_OUT1 * 10)) >> 1) & 0xff);
//	cmp->data[7] = (((ct_7++) & 0x0f) << 1)
//		| (((unsigned short)(pmp->Watt_OUT1 * 10)) & 0x01);
//	if(ct_7 > 15) ct_7 = 0x00;
//	//数据长度
//	cmp->length = 8;
//	cmp->rtr = false;
//	//优先等级
//	cmp->prty = 8;
//	cmp->data[0] = CheckSum(cmp);
//}
////
////void ActuatorVoltageReplyMsg(CAN_MSG* cmp, uint16_t StdId, POWER_MSG* pmp)
////{
////	static uint8_t ct_8 = 0;
////	cmp->std_id = StdId;
////	cmp->data[1] = (((unsigned short)(pmp->VIN_IN * 10)) >> 1) & 0xff;
////	cmp->data[2] = (((((unsigned short)(pmp->VIN_OUT3 * 10)) >> 2) & 0x7f) << 1)
////		| (((unsigned short)(pmp->VIN_IN * 10)) & 0x01);
////	cmp->data[3] = (((((unsigned short)(pmp->VIN_OUT2 * 10)) >> 3) & 0x3f) << 2)
////		| (((unsigned short)(pmp->VIN_OUT3 * 10)) & 0x03);
////	cmp->data[4] = ((((unsigned short)(pmp->VIN_OUT1 * 10) >> 4) & 0x1f) << 3)
////		| (((unsigned short)(pmp->VIN_OUT2 * 10)) & 0x07);
////	cmp->data[5] = (((ct_8++) & 0x0f) << 4)
////		| ((unsigned short)(pmp->VIN_OUT1 * 10) & 0x0f);
////	if(ct_8 > 15) ct_8 = 0x00;
////	//数据长度
////	cmp->length = 6;
////	cmp->rtr = false;
////	//优先等级
////	cmp->prty = 8;
////	cmp->data[0] = CheckSum(cmp);
////}
//////
//////void ActuatorCurrentReplyMsg(CAN_MSG* cmp, uint16_t StdId, POWER_MSG* pmp)
//////{
//////	static uint8_t ct_9 = 0;
//////	//帧ID
//////	cmp->std_id = StdId;
//////	cmp->data[1] = (((unsigned short)(pmp->SENSE_IN * 10)) >> 1) & 0xff;
//////	cmp->data[2] = (((((unsigned short)(pmp->SENSE_OUT3*10)) >> 2) & 0x7f) << 1)
//////		| (((unsigned short)(pmp->SENSE_IN * 10)) & 0x01);
//////	cmp->data[3] =
//////		(((((unsigned short)(pmp->SENSE_OUT2 * 10)) >> 3) & 0x3f) << 2)
//////		| (((unsigned short)(pmp->SENSE_OUT3 * 10)) & 0x03);
//////	cmp->data[4] = ((((unsigned short)(pmp->SENSE_OUT1 * 10) >> 4) & 0x1f) << 3)
//////		| (((unsigned short)(pmp->SENSE_OUT2 * 10)) & 0x07);
//////	cmp->data[5] = (((ct_9++) & 0x0f) << 4)
//////		| ((unsigned short)(pmp->SENSE_OUT1 * 10) & 0x0f);
//////	if(ct_9 > 15) ct_9 = 0x00;
//////	//数据长度
//////	cmp->length = 6;
//////	cmp->rtr = false;
//////	//优先等级
//////	cmp->prty = 8;
//////	cmp->data[0] = CheckSum(cmp);
//////}

void LoRaStatusReplyMsg(CAN_MSG* cmp, uint16_t StdId, uint8_t cmd)
{
	static uint8_t ct_10 = 0;
	//帧ID
	cmp->std_id = StdId;
	cmp->data[1] = (((ct_10++) & 0x0f) << 4) | (cmd & 0x0f);
	if(ct_10 > 15) ct_10 = 0x00;
	//数据长度
	cmp->length = 2;
	cmp->rtr = false;
	//优先等级
	cmp->prty = 8;
	cmp->data[0] = CheckSum(cmp);
}
