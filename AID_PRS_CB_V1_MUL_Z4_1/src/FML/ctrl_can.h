/*
 * ctrl_can.h
 *
 *  Created on: 2020年7月23日
 *      Author: Su-xi
 */

#ifndef FML_CTRL_CAN_H_
#define FML_CTRL_CAN_H_

#include "flexcan.h"

#define CTRL_0_1  0x101
#define CTRL_0_2  0x102
#define CTRL_0_3  0x103
#define CTRL_0_4  0x104
#define CTRL_0_5  0x105
#define CTRL_0_6  0x106
#define CTRL_0_7  0x107
#define CTRL_0_8  0x108
#define CTRL_0_9  0x109
#define CTRL_0_A  0x10A

#define CTRL_1_1  0x111
#define CTRL_1_2  0x112
#define CTRL_1_3  0x113
#define CTRL_1_4  0x114
#define CTRL_1_5  0x115
#define CTRL_1_6  0x116
#define CTRL_1_7  0x117

#define CTRL_2_1  0x121
#define CTRL_2_2  0x122
#define CTRL_2_3  0x123
#define CTRL_2_4  0x124
#define CTRL_2_5  0x125
#define CTRL_2_6  0x126
#define CTRL_2_7  0x127

#define PRS_0_1   0x181
#define PRS_0_2   0x182
#define PRS_0_3   0x183
#define PRS_0_4   0x184
#define PRS_0_5   0x185
#define PRS_0_6   0x186
#define PRS_0_7   0x187

#define PRS_1_1   0x191
#define PRS_1_2   0x192
#define PRS_1_3   0x193
#define PRS_1_4   0x194
#define PRS_1_5   0x195
#define PRS_1_6   0x196
#define PRS_1_7   0x197
#define PRS_1_8   0x198
#define PRS_1_9   0x199
#define PRS_1_10  0x19A

#define PRS_1_11  0x19B
#define PRS_1_12  0x19C
#define PRS_1_13  0x19D
#define PRS_1_14  0x19E
#define PRS_1_15  0x19F

#define RESULT_OK				0x00
#define RESULT_FAIL				0x01
#define RESULT_UNSUPPORTED		0x02
#define RESULT_ERROR			0x03


void ctrl_can_init(void);

uint8_t check_sum(CAN_MSG* cmp);

void general_replay_msg(CAN_MSG* cmp, uint16_t std_id, uint16_t reply_std_id,
		uint8_t actuator_id,uint8_t state,uint8_t reason);

void calibration_data_query_replay_msg(CAN_MSG* cmp, uint16_t std_id,
		CALI_DATA cali_data,DEVICE_INFO device_info);

void actuator_parameter_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		uint8_t actuator_id,uint16_t type, int32_t value);

void device_info_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		DEVICE_INFO device_info);

void system_status_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data);

void actuator_position_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data);

void actuator_alarm_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data);

void actuator_status_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data);

void power_reply_msg(CAN_MSG* cmp, uint16_t std_id,
		volatile SHARED_DATA* s_data);

void actuator_CVP_msg(CAN_MSG* cmp, uint16_t std_id, ROBOT_INFO* motor);
void actuator_CVP_msg_ext(CAN_MSG* cmp, uint16_t std_id, ROBOT_INFO* motor, volatile SHARED_DATA* s_data);

void actuator_Ratio_msg(CAN_MSG* cmp, uint16_t std_id, volatile SHARED_DATA* s_data);

// 2023-9-13  新增 0x19F  上传 EP 角度信息等
void actuator_CVP_EP_msg(CAN_MSG* cmp, uint16_t std_id, ROBOT_INFO* motor,volatile SHARED_DATA* s_data);

uint8_t check_sum(CAN_MSG* cmp);

#endif /* FML_CTRL_CAN_H_ */
