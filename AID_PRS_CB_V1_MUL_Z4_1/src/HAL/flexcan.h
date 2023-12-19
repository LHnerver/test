/*
 * flexcan.h
 *
 *  Created on: 2018Äê12ÔÂ24ÈÕ
 *      Author: Su-xi
 */

#ifndef FLEXCAN_H_
#define FLEXCAN_H_

#include "derivative.h"
#include "typedefs.h"
#include "platform_config.h"

#define CAN0_BASE_ADDRESS ((unsigned int)&CAN_0)
#define CAN1_BASE_ADDRESS ((unsigned int)&CAN_1)
#define CAN2_BASE_ADDRESS ((unsigned int)&CAN_2)
#define CAN3_BASE_ADDRESS ((unsigned int)&CAN_3)
#define CAN4_BASE_ADDRESS ((unsigned int)&CAN_4)
#define CAN5_BASE_ADDRESS ((unsigned int)&CAN_5)
#define CAN6_BASE_ADDRESS ((unsigned int)&CAN_6)
#define CAN7_BASE_ADDRESS ((unsigned int)&CAN_7)

void can0_init(void);
bool can0_send_msg(CAN_MSG msg);
bool can0_get_msg(CAN_MSG* msg);

void can1_init(void);
bool can1_send_msg(CAN_MSG msg);
bool can1_get_msg(CAN_MSG* msg);

void Init_CAN2(void);
bool CAN2_SendMsg(CAN_MSG msg);
bool CAN2_GetMsg(CAN_MSG* msg);
uint8_t can2_send_msg(uint8_t ID, uint8_t* msg, uint8_t len);

void Init_CAN3(void);
bool CAN3_SendMsg(CAN_MSG msg);
bool CAN3_GetMsg(CAN_MSG* msg);
uint8_t can3_send_msg(uint8_t ID, uint8_t* msg, uint8_t len);

void Init_CAN4(void);
bool CAN4_SendMsg(CAN_MSG msg);
bool CAN4_GetMsg(CAN_MSG* msg);

void Init_CAN5(void);
bool CAN5_SendMsg(CAN_MSG msg);
bool CAN5_GetMsg(CAN_MSG* msg);
uint8_t can5_send_msg(uint8_t ID, uint8_t* msg, uint8_t len);

void Init_CAN7(void);
bool CAN7_SendMsg(CAN_MSG msg);
bool CAN7_GetMsg(CAN_MSG* msg);
uint8_t can7_send_msg(uint8_t ID, uint8_t* msg, uint8_t len);

#endif /* FLEXCAN_H_ */
