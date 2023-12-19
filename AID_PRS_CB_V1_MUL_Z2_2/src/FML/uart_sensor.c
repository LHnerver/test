/*
 * uart_sensor.c
 *
 *  Created on: 2022��9��23��
 *      Author: SuJunwei
 * Used for:
 * 	������RS232��RS485�ӿ�ͨ�ţ��ɼ���������Ϣ
 */

#include "uart_sensor.h"

SCI_MSG sMsgT7;					//�������� LIN7(RS485)
SCI_MSG sMsgT8;					//�������� LIN8(RS232)
SCI_MSG sMsgT10;				//�������� LIN10(RS232)



void uart_sensor_init(void)
{
//	/* Initialize LINFlex8: UART Mode 80MHz, 115200 Baud for ���ߴ��У����������泵����ת��*/
//	Init_LINFlexD7(80, 115200);	//RS485

//	/* Initialize LINFlex8: UART Mode 80MHz, 115200 Baud for AR ���ز���*/
//	Init_LINFlexD8(80, 115200);

	/* Initialize LINFlex10: UART Mode 80MHz, 115200 Baud for BR ���ز���*/
	Init_LINFlexD10(80, 115200);
}


//int16_t pedal_force_sensor(SCI_MSG_p msg)
//{

	//int16_t data;
	// �������Ϊ�����򷵻�0��
	/*
	if((msg->rx_data[2]==0x2D)||(msg->rx_data[1]==0x2D)||(msg->rx_data[0]==0x2D))
		return 0;
    */
	/* ������D56�ϴ�ASCII�ַ�����
	 * ��ʽ�� �ڿڿ�.�ڿ�<		< ��ʾ�س�
	 * �磺123.45		-12.34	- ����ռһ���ַ�
	 * �˴�����ʱ������100����
	 *
	 * ʵ�ʴ��ڽ��շ�Χ  *99.99	*��ʾ0xD���س���
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
