/*
 * uart_sensor.c
 *
 *  Created on: 2022年9月23日
 *      Author: SuJunwei
 * Used for:
 * 	用于与RS232、RS485接口通信，采集传感器信息
 */
//Z0 核
#include "uart_sensor.h"
#include "string.h"

SCI_MSG sMsgT7;					//串口数据 LIN7(RS485)
SCI_MSG sMsgT8;					//串口数据 LIN8(RS232)
SCI_MSG sMsgT10;				//串口数据 LIN10(RS232)

uint8_t  uart10_recv_flag = 0 ;  //接受完成的标志   0 完成接收  1 完成接收

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
	//if((msg->rx_data[2]==0x2D)||(msg->rx_data[1]==0x2D)||(msg->rx_data[0]==0x2D))
	///	return 0;

	/* 传感器D56上传ASCII字符数据
	 * 格式： 口口口.口口<		< 表示回车
	 * 如：123.45		-12.34	- 负号占一个字符
	 * 此处换算时，扩大100倍。
	 *
	 * 实际串口接收范围  *99.99	*表示0xD（回车）
	 */
	//data = 1000*(msg->rx_data[1]&0x0F) +
	//		100*(msg->rx_data[2]&0x0F) + 10*(msg->rx_data[4]&0x0F) + (msg->rx_data[5]&0x0F);
	//return data;
//}


int16_t get_pedal_force(SENSOR_ID id)
{
	switch(id)
	{
	case SR_F:
		break;
	case BR_F:
	case EP_F:
		//return     //pedal_force_sensor(&sMsgT10);
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


// 单通道压力传感器 返回命令 01 03 04 FF FF FF FF 3A 67  长度为9
// 四通道压力传感器   02 2B 30 30 30 34 33 36 2D 30 30 30 30 30 33 2D 30 30 31 38 39 31 2B 30 30 30 30 30 31 31 33 33 FF   //长度为33
void uart10_recv_isr(void)
{
	LINFlexD_10.UARTSR.B.DRFRFE = 1;			/*Clear DRF interrupt flag */


//  四通道传感器
# if   1

	if(sMsgT10.rx_index <= 32)
	{
		sMsgT10.rx_data[sMsgT10.rx_index] = LINFlexD_10.BDRM.B.DATA4;
		if(sMsgT10.rx_data[0] == 0x02)
		{
			sMsgT10.rx_index ++ ;
			if(sMsgT10.rx_index == 33 && sMsgT10.rx_data[32] == 0xFF)
			{
			    uart_printf("sMsgT10.rx_data[32] :%02x\n",sMsgT10.rx_data[32]);
				uart10_recv_flag = 1 ;
				sMsgT10.rx_index = 0 ;
			}
		}else{

			sMsgT10.rx_index = 0 ;
		}
	}
	else
	{
		sMsgT10.rx_index = 0 ;
	}
    	sMsgT10.rx_frame_timeout = 0;
#endif

}


void uart10_send_isr(void)
{
	LINFlexD_10.UARTSR.B.DTFTFF = 1;			/*Clear DTF interrupt flag */
    UART10_Send(&sMsgT10);
}


/*  四通道压力传感器通道接口
 *
 * 通道 1  左转向压力传感器
 * 通道 2  右转向压力传感器
 * 通道 3  刹车踏板压力传感器
 *
 * */

void  pressure_sensor(SCI_MSG_p msg ,  int16_t * braking_pressure , int16_t *left_steering_pressure ,int16_t *right_steering_pressure)
{
	int16_t  braking_pressure_tmp = 0 ;                //踏板压力传感器数值
	int16_t  left_steering_pressure_tmp = 0 ;          //左边转向压力传感器数值
	int16_t  right_steering_pressure_tmp = 0 ;         //右边边转向压力传感器数值

	int8_t   pedal_pressure_sensor_symbol = 0 ;  //踏板压力传感器符号位                                第十五个字节
	int8_t   left_steering_pressure_sensor_symbol = 0 ;   //左边转向压力传感器符号位         第二个字节
	int8_t   right_steering_pressure_sensor_symbol = 0 ;  //右边边转向压力传感器符号位      第九个字节


    if(uart10_recv_flag == 1)   //代表数据收满
    {
    	/*
    	 * uint8_t i = 0 ;
		for(i = 0 ; i<33 ; i++)
		{
			uart_printf("sMsgT10.rx_data[%d]:%02x\n",i,sMsgT10.rx_data[i]);
		}
		*/
		if(sMsgT10.rx_data[15] == 0x2D)   // 踏板压力传感器  '负值'
		{
			pedal_pressure_sensor_symbol = -1 ;
		}
		else if(sMsgT10.rx_data[15] == 0x2B)
		{
			pedal_pressure_sensor_symbol =  1 ;
		}
		else{  // 报错


		}
    	 if(sMsgT10.rx_data[1] == 0x2D)   // 左转向 压力传感器  '负值'
		 {
    		 left_steering_pressure_sensor_symbol = -1 ;
		 }
		 else if(sMsgT10.rx_data[1] == 0x2B)
		 {
			 left_steering_pressure_sensor_symbol =  1 ;
		 }
		 else{

			 // 报错
		 }

      	 if(sMsgT10.rx_data[8] == 0x2D)   // 右转向 压力传感器  '负值'
		 {
      		right_steering_pressure_sensor_symbol = -1 ;
		 }
		 else if(sMsgT10.rx_data[8] == 0x2B)
		 {
			 right_steering_pressure_sensor_symbol =  1 ;
		 }else{

			 // 报错
		 }

        //踏板压力值
     	braking_pressure_tmp = 100000*(msg->rx_data[16] - '0') +  10000*(msg->rx_data[17] - '0') +  1000*(msg->rx_data[18] - '0')
         					+ 100*(msg->rx_data[19] - '0') + 10*(msg->rx_data[20] - '0') +  1*(msg->rx_data[21] - '0') ;

     	*braking_pressure = braking_pressure_tmp*pedal_pressure_sensor_symbol*10 ;  //扩大十倍


     	//左转向压力值
     	left_steering_pressure_tmp = 100000*(msg->rx_data[2] - '0') +  10000*(msg->rx_data[3] - '0') +  1000*(msg->rx_data[4] - '0')
         					+ 100*(msg->rx_data[5] - '0') + 10*(msg->rx_data[6] - '0') +  1*(msg->rx_data[7] - '0') ;

     	//uart_printf("left_steering_pressure_tmp :%d\n",left_steering_pressure_tmp);

     	*left_steering_pressure = left_steering_pressure_tmp * left_steering_pressure_sensor_symbol ;


        //右转向压力值
     	right_steering_pressure_tmp = 100000*(msg->rx_data[9] - '0') +  10000*(msg->rx_data[10] - '0') +  1000*(msg->rx_data[11] - '0')
         					+ 100*(msg->rx_data[12] - '0') + 10*(msg->rx_data[13] - '0') +  1*(msg->rx_data[14] - '0') ;

        //uart_printf("right_steering_pressure_tmp :%d\n",right_steering_pressure_tmp);

     	*right_steering_pressure = right_steering_pressure_tmp * right_steering_pressure_sensor_symbol;


		//uart_printf("braking_pressure :%d\n",*braking_pressure);
		//uart_printf("left_steering_pressure :%d\n",*left_steering_pressure);
     	//uart_printf("right_steering_pressure :%d\n",*right_steering_pressure);

     	uart10_recv_flag = 0 ;

     	memset(msg->rx_data, 0, sizeof(msg->rx_data));  //数值清空


    }
}


#if    0

//单通道压力传感器暂时先不用
/*  HY-XSA 单通道压力传感器压力传感器 crc 校验函数  */
/*  Z0 核 增加
 *addr  需要传入的数据
 *data_len  数据的长度
 *返回值   crc 数值   高字字节在后 ， 低字节在前
*/
uint8_t  pressure_command[8] = {0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};  //modbus 发送的命令

unsigned short int crc16(uint8_t *addr,uint8_t data_len)
{
	int i,j,temp=0;
	int crc=0xFFFF;
	for(i=0;i<data_len;i++)
	{
		crc=crc^(*addr);
		for(j=0;j<8;j++)
		{
			temp=crc&0x0001;
			crc=crc>>1;
			if(temp)
			{
				crc=crc^0xA001;
			}
		}
		addr++;
	}
	return crc;
}

/* 压力传感器的发生函数
 *  command 发送的命令
 *
 *  length  发送的长度
 *  */
void  Send_pressure_command(uint8_t *command , uint8_t length)
{
	uint8_t i = 0 ;
	for(i =0 ; i< length ; i++)
	{
		UART10_SendByte(command[i]);
	}

}
unsigned short int  pressure_receive_data(void)
{
	uint16_t crc_vale = 0 ;
	uint16_t  weight = 0 ;

	if(uart10_recv_flag == 1)
	{
		crc_vale = crc16(sMsgT10.rx_data,7);
		UART1_SendByte(sMsgT10.rx_data[8]);  //67
		UART1_SendByte(sMsgT10.rx_data[7]);  //3A

		UART1_SendByte(crc_vale);
		UART1_SendByte(crc_vale>>8);
		if(crc_vale ==  (sMsgT10.rx_data[8]*256 + sMsgT10.rx_data[7]))
		{
			weight = sMsgT10.rx_data[5] *256 +  sMsgT10.rx_data[6] ;      // 单位 0.1 kg

		}

        memset(sMsgT10.rx_data ,0,sizeof(sMsgT10.rx_data));
        uart10_recv_flag = 0 ;

        return  weight  ;
	}

}

#endif
/****************************************************************************************/


/***************************UART1 串口重定向 uart_printf ************************************/

void uart_sendstring(char *pt,int len)
{
	while(*pt)
	 {
		UART1_SendByte(*pt++);
	 }
}

void uart_printf(char *fmt,...)
{
	va_list ap;
	int len=0;
	char string[100]={0};
	memset(string,0,100);
	va_start(ap,fmt);

	len = vsprintf(string,fmt,ap);
	if(len >0)
	{
		uart_sendstring(string, len);
	}

	va_end(ap);
}

/**************************************************************************************************/
