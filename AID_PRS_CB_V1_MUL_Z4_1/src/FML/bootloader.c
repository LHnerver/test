/*
 * bootloader.c
 *
 *  Created on: 2020??11??26??
 *      Author: Su-xi
 */

#include "bootloader.h"

bool boot_flag;
SCI_MSG sMsgT5;					//???????? LIN5(RS232)

void boot_init(void)
{
	/* Initialize LINFlex5: UART Mode 80MHz, 115200 Baud for BootLoader*/
	Init_LINFlexD5(80, 115200);

	// ???????????????app
	memset(sMsgT5.rx_data, 0x00, 50);
	sMsgT5.rx_data[0] = 0x01;
	sMsgT5.rx_data[1] = 0x07;
	sMsgT5.rx_data[2] = 0x0D;
	sMsgT5.rx_data[3] = 0x0A;
	sMsgT5.rx_length = 4;
	UART5_Send(&sMsgT5);
}

void boot_execute(void)
{
	if (boot_flag)
	{
		uint32_t i;

		/* Disable SWT_0 */
		SWT_0.CR.R = 0xFF000100;

		flash_set_boot_data();

		//some delay
		for(i=0; i<1000000; i++){};
		//	asm("nop");

		MC_ME.MCTL.R = KEY_VALUE1;			//Functional RESET & Key
		MC_ME.MCTL.R = KEY_VALUE2;			//Functional RESET & Key
	}
}

void uart5_recv_isr(void)
{
	LINFlexD_5.UARTSR.B.DRFRFE = 1;			/*Clear DRF interrupt flag */

	sMsgT5.rx_data[sMsgT5.rx_index++] = LINFlexD_5.BDRM.B.DATA4;

	if (sMsgT5.rx_data[sMsgT5.rx_index - 1] == 'b')
	{
		sMsgT5.rx_data[0] = 'b';
		sMsgT5.rx_index = 0x01;
	}
	else if (sMsgT5.rx_data[0] == 'b'
		&& sMsgT5.rx_data[sMsgT5.rx_index - 1] == '\n')
	{
		if (sMsgT5.rx_data[1] == 'o' && sMsgT5.rx_data[2] == 'o' &&
				sMsgT5.rx_data[3] == 't')
		{
			boot_flag = true;
		}
	}

	if(sMsgT5.rx_index > 48)
		sMsgT5.rx_index = 0x00;
}

void uart5_send_isr(void)
{
	LINFlexD_5.UARTSR.B.DTFTFF = 1;			/*Clear DTF interrupt flag */
    UART5_Send(&sMsgT5);
}
