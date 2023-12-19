/*****************************************************************************/
/* FILE NAME: linflexd_uart.c         COPYRIGHT (c) NXP Semiconductors 2016  */
/*                                                      All Rights Reserved  */
/* PLATFFORM: DEVKIT-MPC5748G												 */
/* DESCRIPTION: Routines to transmit & receive UART messages with LINflexD.  */
/*              Data is sent one byte at a time without using the FIFO.      */
/*                                                                           */
/*****************************************************************************/
/* REV      AUTHOR        DATE        DESCRIPTION OF CHANGE                  */
/* ---   -----------    ----------    ---------------------                  */
/* 1.0	 Scott Obrien   Mar 31 2014   Initial Version                        */
/*****************************************************************************/
/*
** LINFLEX UART Driver
** Notes:
** 1. UARTCR bits can NOT be written until UARTCR.UART is SET
** 2. There is no way  to determine if the transmit buffer is
** empty before the first data transmission.  The DTF bit (Transmit Complete)
** flag is the only indication that it's OK to write to the data buffer.
** There is no way to set this bit in software.  To set this bit, a transmit
** must occur.
*/
/*****************************************************************************/

#include "linflexd_uart.h"

/*****************************************************************************/
/*
** Baud Rate = LINCLK / (16 x LFDIV)
** LINCLK = BR x (16 x LFDIV)
** LINCLK / (BR x 16) = LFDIV
**
** LFDIV = Mantissa.Fraction.
** Mantissa = LINIBRR
** Fraction = LINFBRR / 16
**
** Baud Rate = LINCLK / (16 x LINIBRR.(LINFBRR / 16))
** LINIBRR.(LINFBRR / 16) = LINCLK / (BR x 16)
** LINIBRR = Mantissa[LINCLK / (BR x 16)]
** Remainder =  LINFBRR / 16
** LINFBRR = Remainder * 16
** The Remainder is < 1, So how does the Remainder work during a divide?
** May be best to use a table?
**
** For Reference & Quick Tests
** LINFLEX_x.LINIBRR.R = 416;                   // 9600 at 64MHz
** LINFLEX_x.LINFBRR.R = 11;
**
** LINFLEX_x.LINIBRR.R = 781;                   // 9600 at 120MHz
** LINFLEX_x.LINFBRR.R = 4;
*/

/*****************************************************************************/

/* LINFlex 1 for UART functioning (RS232-1)*/
void Init_LINFlexD1 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_1.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_1.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_1.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_1.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_1.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_1.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_1.LINIBRR.R = Integer;
	LINFlexD_1.LINFBRR.R = Fraction;

	LINFlexD_1.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_1.LINIER.B.DTIE = 1;		  		/* Enable Tx interrupt */

	LINFlexD_1.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void uart1_send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_1.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/*****************************************************************************/

/* LINFlex 3 for UART functioning (RS485-0)*/
void Init_LINFlexD3 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_3.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_3.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_3.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_3.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_3.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_3.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_3.LINIBRR.R = Integer;
	LINFlexD_3.LINFBRR.R = Fraction;

	LINFlexD_3.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_3.LINIER.B.DTIE = 0;		  		/* Disable Tx interrupt */

	LINFlexD_3.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	LINFlexD_3.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART3_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_3.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/* LINFlex 4 for UART functioning (RS485-1)*/
void Init_LINFlexD4 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_4.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_4.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_4.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_4.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_4.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_4.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_4.LINIBRR.R = Integer;
	LINFlexD_4.LINFBRR.R = Fraction;

	LINFlexD_4.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_4.LINIER.B.DTIE = 0;		  		/* Disable Tx interrupt */

	LINFlexD_4.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	LINFlexD_4.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART4_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_4.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/*****************************************************************************/
/* LINFlex 5 for UART functioning */
void Init_LINFlexD5 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_5.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_5.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_5.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_5.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_5.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_5.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_5.LINIBRR.R = Integer;
	LINFlexD_5.LINFBRR.R = Fraction;

	LINFlexD_5.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_5.LINIER.B.DTIE = 1;		  		/* Disable Tx interrupt */

	LINFlexD_5.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	LINFlexD_5.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART5_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_5.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/*****************************************************************************/
/* LINFlex 6 for UART functioning */
void Init_LINFlexD6 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_6.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_6.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_6.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_6.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_6.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_6.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_6.LINIBRR.R = Integer;
	LINFlexD_6.LINFBRR.R = Fraction;

	LINFlexD_6.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_6.LINIER.B.DTIE = 0;		  		/* Disable Tx interrupt */

	LINFlexD_6.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	LINFlexD_6.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART6_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_6.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/*****************************************************************************/
/* LINFlex 7 for RS485-7 functioning */
void Init_LINFlexD7 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_7.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_7.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_7.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_7.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_7.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_7.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_7.LINIBRR.R = Integer;
	LINFlexD_7.LINFBRR.R = Fraction;

	LINFlexD_7.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_7.LINIER.B.DTIE = 0;		  		/* Disable Tx interrupt */

	LINFlexD_7.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	LINFlexD_7.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART7_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_7.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/*****************************************************************************/
/* LINFlex 8 for RS232 functioning */
void Init_LINFlexD8 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_8.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_8.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_8.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_8.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_8.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_8.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_8.LINIBRR.R = Integer;
	LINFlexD_8.LINFBRR.R = Fraction;

	LINFlexD_8.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_8.LINIER.B.DTIE = 1;		  		/* Enable Tx interrupt */

	LINFlexD_8.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	LINFlexD_8.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART8_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_8.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

/*****************************************************************************/
/* LINFlex 10 for RS232_EBR functioning 用于BR测力采样 */
void Init_LINFlexD10 (unsigned int MegaHertz, unsigned int BaudRate)
{
	unsigned int Fraction, Integer, i;

	LINFlexD_10.LINCR1.B.INIT = 1;     			/* Enter Initialization Mode */
	LINFlexD_10.LINCR1.B.SLEEP = 0;    			/* Exit Sleep Mode */
	LINFlexD_10.UARTCR.B.UART = 1;     			/* UART Enable, Req'd before UART configuration */
	LINFlexD_10.UARTCR.R = 0x0033;     			/* UART Enable, 1 byte tx, no parity, 8 data*/
	LINFlexD_10.UARTSR.B.SZF = 1;      			/* CHANGE THIS LINE   Clear the Zero status bit */
	LINFlexD_10.UARTSR.B.DRFRFE = 1;   			/* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

	BaudRate  = (MegaHertz * 1000000) / BaudRate;
	Integer   = BaudRate / 16;
	Fraction  = BaudRate - (Integer * 16);
	LINFlexD_10.LINIBRR.R = Integer;
	LINFlexD_10.LINFBRR.R = Fraction;

	LINFlexD_10.LINIER.B.DRIE = 1;				/* Enable Rx interrupt */
	LINFlexD_10.LINIER.B.DTIE = 1;		  		/* Disable Tx interrupt */

	LINFlexD_10.LINCR1.B.INIT = 0;     			/* Exit Initialization Mode */

	for(i = 0; i < 1000; i++) {}				/*delay for correct LinFlex setup*/
}

void UART10_Send(SCI_MSG* msg)
{
	if (msg->rx_index < msg->rx_length)
	{
		LINFlexD_10.BDRL.B.DATA0 = msg->rx_data[msg->rx_index++];
	}
	else
	{
		msg->rx_index = 0;
		msg->rx_length = 0;
		msg->rx_index = 0;
	}
}

void UART10_SendByte(unsigned char data)
{
	uint16_t i = 10000;
	LINFlexD_10.UARTSR.B.DTFTFF = 1;
	LINFlexD_10.BDRL.B.DATA0 = data;
	while (!LINFlexD_10.UARTSR.B.DTFTFF)
	{
		i--;
		if (i == 0)
			break;
	}
}



void UART1_SendByte(unsigned char data)
{
	uint16_t i = 10000;
	LINFlexD_1.UARTSR.B.DTFTFF = 1;
	LINFlexD_1.BDRL.B.DATA0 = data;
	while (!LINFlexD_1.UARTSR.B.DTFTFF)
	{
		i--;
		if (i == 0)
			break;
	}
}


void UART8_SendByte(unsigned char data)
{
	uint16_t i = 10000;
	LINFlexD_8.UARTSR.B.DTFTFF = 1;
	LINFlexD_8.BDRL.B.DATA0 = data;
	while (!LINFlexD_8.UARTSR.B.DTFTFF)
	{
		i--;
		if (i == 0)
			break;
	}
}

void uart8_sendstring(char *pt,int len)
{
	while(*pt)
	 {
		UART8_SendByte(*pt++);
	 }
}

void uart8_printf(char *fmt,...)
{
	va_list ap;
	int len=0;
	char string[100]={0};
	memset(string,0,100);
	va_start(ap,fmt);

	len = vsprintf(string,fmt,ap);
	if(len >0)
	{
		uart8_sendstring(string, len);
	}

	va_end(ap);
}



