/*
 * uart_sensor.c
 *
 *  Created on: 2022年9月23日
 *      Author: SuJunwei
 * Used for:
 * 	用于与RS232、RS485接口通信，采集传感器信息
 */

#include "uart_sensor.h"

SCI_MSG sMsgT7;					//串口数据 LIN7(RS485)
SCI_MSG sMsgT8;					//串口数据 LIN8(RS232)
SCI_MSG sMsgT10;				//串口数据 LIN10(RS232)



void uart_sensor_init(void)
{
//	/* Initialize LINFlex8: UART Mode 80MHz, 115200 Baud for 拉线传感，测量合力叉车后轮转角*/
//	Init_LINFlexD7(80, 115200);	//RS485

//	/* Initialize LINFlex8: UART Mode 80MHz, 115200 Baud for AR 力矩采样*/
//	Init_LINFlexD8(80, 115200);

	/* Initialize LINFlex10: UART Mode 80MHz, 115200 Baud for BR 力矩采样*/
	Init_LINFlexD10(80, 115200);
}


//int16_t pedal_force_sensor(SCI_MSG_p msg)
//{

	//int16_t data;
	// 如果数据为负，则返回0。
	/*
	if((msg->rx_data[2]==0x2D)||(msg->rx_data[1]==0x2D)||(msg->rx_data[0]==0x2D))
		return 0;
    */
	/* 传感器D56上传ASCII字符数据
	 * 格式： 口口口.口口<		< 表示回车
	 * 如：123.45		-12.34	- 负号占一个字符
	 * 此处换算时，扩大100倍。
	 *
	 * 实际串口接收范围  *99.99	*表示0xD（回车）
	 */
   /*
	data = 1000*(msg->rx_data[1]&0x0F) +
			100*(msg->rx_data[2]&0x0F) + 10*(msg->rx_data[4]&0x0F) + (msg->rx_data[5]&0x0F);
	return data;
   */
//}



int16_t get_pedal_force(SENSOR_ID id)
{
	switch(id)
	{
	case SR_F:
		break;
	case BR_F:
	case EP_F:
		break;
	case AR_F:
		break;
	default:
		break;
	}
}

void uart7_recv_isr(void)
{
	LINFlexD_7.UARTSR.B.DRFRFE = 1;			/*Clear DRF interrupt flag */

	//do something
}


void uart8_recv_isr(void)
{
	LINFlexD_8.UARTSR.B.DRFRFE = 1;			/*Clear DRF interrupt flag */

	//do something
	sMsgT8.rx_data[sMsgT8.rx_index++] = LINFlexD_8.BDRM.B.DATA4;

	if (sMsgT8.rx_data[sMsgT8.rx_index - 1] == 'b')
	{
		sMsgT8.rx_data[0] = 'b';
		sMsgT8.rx_index = 0x01;
	}
	else if (sMsgT8.rx_data[0] == 'b'
		&& sMsgT8.rx_data[sMsgT8.rx_index - 1] == '\n')
	{
		if (sMsgT8.rx_data[1] == 'o' && sMsgT8.rx_data[2] == 'o' &&
				sMsgT8.rx_data[3] == 't')
		{
			//boot_flag = true;
		}
	}

	if(sMsgT8.rx_index > 48)
		sMsgT8.rx_index = 0x00;
}

void uart8_send_isr(void)
{
	LINFlexD_8.UARTSR.B.DTFTFF = 1;			/*Clear DTF interrupt flag */
    UART8_Send(&sMsgT8);
}

void uart10_recv_isr(void)
{

	LINFlexD_10.UARTSR.B.DRFRFE = 1;			/*Clear DRF interrupt flag */

}

void uart10_send_isr(void)
{
	LINFlexD_10.UARTSR.B.DTFTFF = 1;			/*Clear DTF interrupt flag */
    UART10_Send(&sMsgT10);
}

