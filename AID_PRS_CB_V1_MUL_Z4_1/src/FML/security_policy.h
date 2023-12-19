#ifndef FML_SECURITY_POLICY_H_
#define FML_SECURITY_POLICY_H_

// Z1 --security_policy.h

#include "typedefs.h"


#define   MOTOR_NO_ALARMS                0x0000     //�޹���
#define   MOTOR_OVERVOLTAGE_ERROR        0x0001     //��ѹ�쳣
#define   MOTOR_UNDERVOLTAGE_ERROR       0x0002     // Ƿѹ�쳣
#define   MOTOR_LOCKED_ROTOR_ERROR       0x0004     //��ת
#define   MOTOR_SUPERHEAT_ERROR          0x0008      //����
#define   MOTOR_WRITE_READ_ERROR         0x0010     //��д�����쳣 (��Ӱ������)
#define   MOTOR_MULTI_LOOP_COUNT_ERROR   0x0020     //��Ȧ�����쳣(!��Ӱ������)
#define   MOTOR_INVERTER_TEMP            0x0040      //������¶ȴ������쳣
#define   MOTOR_COMMUNICATION_ERROR      0x0080      // CANͨ���쳣
#define   MOTOR_SENSOR_ERROR             0x0100      //����¶ȴ������쳣
#define   MOTOR_STEP_ERROR               0x0200      //��Ծ����
#define   MOTOR_DRV_ERROR                0x0400      // DRV����
#define   MOTOR_ENCODER_ERROR            0x0800      // �������쳣


extern SYSTEM_TIME system_time;

extern DELAY_TIME delay_time;

extern SYS_INFO sys_info;

extern CHECK_INFO check_info;

extern CONFIG_DATA config_data;

extern DEVICE_INFO device_info;

extern CAN_CTRL_MSG can_ctrl_msg;

extern ROBOT_INFO sr_info;

extern ROBOT_INFO ar_info;

extern ROBOT_INFO br_info;

extern ROBOT_INFO ep_info;

extern uint8_t  CAN_communication_count ;
extern uint8_t   CAN_communication_abnormal_flag  ;


void  CAN_communication_abnormal(void);
void  manual_control_BR_EP(CAN_MSG* can0_rx_msg);


#endif