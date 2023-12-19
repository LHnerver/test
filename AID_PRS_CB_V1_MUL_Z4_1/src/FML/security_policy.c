#include "security_policy.h"
#include "SCA_API.h"
#include "SCA_Protocol.h"
#include "linflexd_uart.h"
#include "derivative.h" /* include peripheral declarations */
#include "SCA_APP.h"
#include <MPC5748G.h>
#include "typedefs.h"
#include "ctrl_can.h"
// Z1 --security_policy.c  安全策略   Z1 核 CAN_communication_abnormal()

#define   EP_BRAKE_SYMBOL     0  // 增加EP制动的宏


extern ROBOT_INFO sr_info;

// CAN 通信故障计数器 和 CAN通信故障标志
uint8_t  CAN_communication_count = 0 ;
uint8_t  CAN_communication_abnormal_flag = 0  ;
uint8_t  first_run_flag = 0 ;


void   CAN_communication_abnormal(void)
 {

       if(CAN_communication_count > 20  &&  (sys_info.sys_st.b.sys_mode == 3 || sys_info.sys_st.b.sys_mode == 4) )  // 超过1s 没有CAN数据
       {

    	   if(config_data.cali_data.br_max_angle_valid)
    	   {


		       SCA_Move_Pos(BR_ID,
								((float)(config_data.cali_data.br_max_angle)) / BRK_INF_RE,
								(float)(500),
								(float)(3000.00),(float)(-3000.00));


				#if   EP_BRAKE_SYMBOL    //EP制动的标志

						SCA_Move_Pos(EP_ID,
						((float)(config_data.cali_data.br_max_angle)
						) / EP_INF_RE,
						(float)(695),                       //695
						(float)(3000.00),(float)(-3000.00));//500-695/600-835

				#endif

						SCA_Move_Pos(AR_ID,
						(float)0/ THR_INF_RE,
						(float)(500),
						(float)(3000.00),(float)(-3000.00));

    	   }
    	   else{



    		    SCA_Move_Pos(BR_ID,
    							(-(float)(250)) / BRK_INF_RE,
    									(float)(1500),
    									(float)(3000.00),(float)(-3000.00));



			#if   EP_BRAKE_SYMBOL    //EP制动的标志

					SCA_Move_Pos(EP_ID,
					(-(float)(250)
					) / EP_INF_RE,
					(float)(695),                       //695
					(float)(3000.00),(float)(-3000.00));//500-695/600-835

			#endif

					SCA_Move_Pos(AR_ID,
					(float)0/ THR_INF_RE,
					(float)(500),
					(float)(3000.00),(float)(-3000.00));
    	   }
			CAN_communication_abnormal_flag = 1 ;    // 故障标志， 在蜂鸣器函数中处理报警
     }
 }




 void  manual_control_BR_EP(CAN_MSG* can0_rx_msg)
 {

     if(sys_info.sys_st.b.sys_mode != 3 && sys_info.sys_st.b.sys_mode != 4)   //非自动驾驶模式下
     {
		 if(can0_rx_msg->std_id == 0x117)
		 {
			 can0_rx_msg->data[0] = check_sum(can0_rx_msg);
			 if(can_ctrl_msg.CTRL1_HandDrivingCtrl == 1)
			 {
//				uart8_printf("can0_rx_msg->std_id :%02x\n",can0_rx_msg->std_id);
//		        uart8_printf("can_ctrl_msg.CTRL1_EP_DIRECTION :%d\n",can_ctrl_msg.CTRL1_EP_DIRECTION);
//			    uart8_printf("can_ctrl_msg.CTRL1_HandDrivingCtrl:%d\n",can_ctrl_msg.CTRL1_HandDrivingCtrl);
//			    uart8_printf("can_ctrl_msg.CTRL1_HAND_EP_Angle:%d\n",can_ctrl_msg.CTRL1_HAND_EP_Angle);

                if(can_ctrl_msg.CTRL1_EP_DIRECTION == 0)
                {


                 SCA_Move_Pos(BR_ID,
              						((float)(can_ctrl_msg.CTRL1_HAND_EP_Angle)) / BRK_INF_RE,
              						(float)(1500),
              						(float)(3000.00),(float)(-3000.00));

#if     		 EP_BRAKE_SYMBOL

						SCA_Move_Pos(EP_ID,
						((float)(can_ctrl_msg.CTRL1_HAND_EP_Angle)
						 ) / EP_INF_RE,
						(float)(695),
						(float)(3000.00),(float)(-3000.00));//500-695/600-835
#endif

                  }
                  else if(can_ctrl_msg.CTRL1_EP_DIRECTION == 1)
                  {

						SCA_Move_Pos(BR_ID,
						(-(float)(can_ctrl_msg.CTRL1_HAND_EP_Angle)) / BRK_INF_RE,
						(float)(1500),
						(float)(3000.00),(float)(-3000.00));

#if    EP_BRAKE_SYMBOL

						SCA_Move_Pos(EP_ID,
						(-(float)(can_ctrl_msg.CTRL1_HAND_EP_Angle)
						 ) / EP_INF_RE,
						(float)(695),
						(float)(3000.00),(float)(-3000.00));//500-695/600-835
#endif

                  }else{

                  }

                  can_ctrl_msg.CTRL1_HandDrivingCtrl  = 0 ;
			 }

		 }
	}
 }






































