/*
 * flexcan.c
 *
 *  Created on: 2018年12月24日
 *      Author: Su-xi
 */

#include "flexcan.h"

//CAN0 initialization
void can0_init(void)
{
	uint16_t i, j;

	CAN_0.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_0.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_0.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_0.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	//CAN_0.CTRL1.R = 0x09DB0086;				/* 250 kbps */
	CAN_0.CTRL1.R = 0x04DB0086; 			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_0.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_0.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_0.RXIMR[i].R = 0x00000000;		/* set individual mask registers - no bits need to match */

	//CAN_0.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_0.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_0.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_0.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN0_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN0_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN0_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN0_BASE_ADDRESS + i) = 0x601 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_0.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_0.MCR.B.FRZACK & CAN_0.MCR.B.NOTRDY) {} /* Wait to clear */
											/* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool can0_send_msg(CAN_MSG msg)
{
	uint8_t	i, mb = 8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_0.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	CAN_0.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_0.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_0.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_0.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_0.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_0.MB[mb].ID.B.PRIO = msg.prty;

	CAN_0.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_0.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

bool can0_get_msg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_0.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_0.MB[0].ID.R;

	msg->std_id = CAN_0.MB[0].ID.B.ID_STD;
	msg->length = CAN_0.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_0.MB[0].DATA.B[j];
	}

	dummy = CAN_0.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_0.IFLAG1.B.BUF5I = 1;
	return(true);
}

/* CAN1*/
void can1_init(void)
{
	uint16_t i, j;

	CAN_1.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_1.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_1.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_1.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	//CAN_0.CTRL1.R = 0x09DB0086;			/* 250 kbps */
	//CAN_1.CTRL1.R = 0x017A0087; 			/* 1000kbps */
	CAN_1.CTRL1.R = 0x04DB0086; 			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_1.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_1.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_1.RXIMR[i].R = 0x00000000;		/* individual mask */

	//CAN_1.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_1.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_1.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_1.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN1_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN1_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN1_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN1_BASE_ADDRESS + i) = 0x555 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_1.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_1.MCR.B.FRZACK & CAN_1.MCR.B.NOTRDY) {} /* Wait to clear */ /* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool can1_send_msg(CAN_MSG msg)
{
	uint8_t	i, mb = 8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_1.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	if (i > 17)
	{
		mb = 17;
	}

	CAN_1.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_1.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_1.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_1.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_1.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_1.MB[mb].ID.B.PRIO = msg.prty;

	CAN_1.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_1.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

bool can1_get_msg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_1.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_1.MB[0].ID.R;

	msg->std_id = CAN_1.MB[0].ID.B.ID_STD;
	msg->length = CAN_1.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_1.MB[0].DATA.B[j];
	}

	dummy = CAN_1.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_1.IFLAG1.B.BUF5I = 1;
	return(true);
}

/* CAN2 */
void Init_CAN2(void)
{
	uint16_t i, j;

	CAN_2.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_2.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_2.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_2.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	//CAN_0.CTRL1.R = 0x09DB0086;			/* 250 kbps */
	CAN_2.CTRL1.R = 0x017A0087; 			/* 1000kbps */
	//CAN_2.CTRL1.R = 0x04DB0086; 			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_2.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_2.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_2.RXIMR[i].R = 0x00000000;		/* individual mask */

	//CAN_1.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_2.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_2.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_2.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN2_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN2_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN2_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN2_BASE_ADDRESS + i) = 0x555 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_2.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_2.MCR.B.FRZACK & CAN_2.MCR.B.NOTRDY) {} /* Wait to clear */ /* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool CAN2_SendMsg(CAN_MSG msg)
{
	uint8_t	i, mb =8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_2.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	if (i > 17)
	{
		mb = 17;
	}

	CAN_2.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_2.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_2.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_2.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_2.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_2.MB[mb].ID.B.PRIO = msg.prty;

	CAN_2.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_2.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

uint8_t can2_send_msg(uint8_t ID, uint8_t* msg, uint8_t len)
{
	uint8_t i;
	CAN_MSG cMt;

	//帧ID
	cMt.std_id = ID;

    //查询位
	for(i=0;i<len;i++)
	{
		cMt.data[i] = msg[i];
	}
    //数据长度
	cMt.length = len;
	cMt.rtr = false;
    //优先等级
	cMt.prty = 5;

	//数据发送, 成功0,失败1;
    if(CAN2_SendMsg(cMt))
    	return 0;
    else
    	return 1;
}

bool CAN2_GetMsg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_2.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_2.MB[0].ID.R;

	msg->std_id = CAN_2.MB[0].ID.B.ID_STD;
	msg->length = CAN_2.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_2.MB[0].DATA.B[j];
	}

	dummy = CAN_2.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_2.IFLAG1.B.BUF5I = 1;
	return(true);
}

void Init_CAN3(void)
{
	uint16_t i, j;

	CAN_3.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_3.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_3.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_3.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	//CAN_0.CTRL1.R = 0x09DB0086;			/* 250 kbps */
	CAN_3.CTRL1.R = 0x017A0087; 			/* 1000kbps */
	//CAN_3.CTRL1.R = 0x04DB0086; 			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_3.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_3.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_3.RXIMR[i].R = 0x00000000;		/* individual mask */

	//CAN_1.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_3.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_3.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_3.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN3_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN3_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN3_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN3_BASE_ADDRESS + i) = 0x555 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_3.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_3.MCR.B.FRZACK & CAN_3.MCR.B.NOTRDY) {} /* Wait to clear */
											/* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool CAN3_SendMsg(CAN_MSG msg)
{
	uint8_t	i, mb = 8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_3.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	if (i > 17)
	{
		mb = 17;
	}

	CAN_3.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_3.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_3.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_3.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_3.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_3.MB[mb].ID.B.PRIO = msg.prty;

	CAN_3.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_3.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

bool CAN3_GetMsg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_3.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_3.MB[0].ID.R;

	msg->std_id = CAN_3.MB[0].ID.B.ID_STD;
	msg->length = CAN_3.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_3.MB[0].DATA.B[j];
	}

	dummy = CAN_3.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_3.IFLAG1.B.BUF5I = 1;
	return(true);
}

uint8_t can3_send_msg(uint8_t ID, uint8_t* msg, uint8_t len)
{
	uint8_t i;
	CAN_MSG cMt;

	//帧ID
	cMt.std_id = ID;

    //查询位
	for(i=0;i<len;i++)
	{
		cMt.data[i] = msg[i];
	}
    //数据长度
	cMt.length = len;
	cMt.rtr = false;
    //优先等级
	cMt.prty = 5;

	//数据发送, 成功0,失败1;
    if(CAN3_SendMsg(cMt))
    	return 0;
    else
    	return 1;
}

void Init_CAN4(void)
{
	uint16_t i, j;

	CAN_4.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_4.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_4.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_4.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	//CAN_0.CTRL1.R = 0x09DB0086;				/* 250 kbps */
	CAN_4.CTRL1.R = 0x04DB0086; 			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_4.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_4.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_4.RXIMR[i].R = 0x00000000;		/* individual mask */

	//CAN_1.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_4.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_4.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_4.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN4_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN4_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN4_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN4_BASE_ADDRESS + i) = 0x555 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_4.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_4.MCR.B.FRZACK & CAN_4.MCR.B.NOTRDY) {} /* Wait to clear */
											/* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool CAN4_SendMsg(CAN_MSG msg)
{
	uint8_t	i, mb = 8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_4.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	if (i > 17)
	{
		mb = 17;
	}

	CAN_4.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_4.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_4.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_4.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_4.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_4.MB[mb].ID.B.PRIO = msg.prty;

	CAN_4.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_4.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

bool CAN4_GetMsg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_4.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_4.MB[0].ID.R;

	msg->std_id = CAN_4.MB[0].ID.B.ID_STD;
	msg->length = CAN_4.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_4.MB[0].DATA.B[j];
	}

	dummy = CAN_4.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_4.IFLAG1.B.BUF5I = 1;
	return(true);
}

void Init_CAN5(void)
{
	uint16_t i, j;

	CAN_5.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_5.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_5.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_5.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	//CAN_0.CTRL1.R = 0x09DB0086;			/* 250 kbps */
	CAN_5.CTRL1.R = 0x017A0087; 			/* 1000kbps */
	//CAN_5.CTRL1.R = 0x04DB0086; 			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_5.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_5.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_5.RXIMR[i].R = 0x00000000;		/* individual mask */

	//CAN_1.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_5.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_5.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_5.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN5_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN5_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN5_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN5_BASE_ADDRESS + i) = 0x555 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_5.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_5.MCR.B.FRZACK & CAN_5.MCR.B.NOTRDY) {} /* Wait to clear */
											/* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool CAN5_SendMsg(CAN_MSG msg)
{
	uint8_t	i, mb = 8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_5.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	if (i > 17)
	{
		mb = 17;
	}

	CAN_5.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_5.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_5.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_5.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_5.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_5.MB[mb].ID.B.PRIO = msg.prty;

	CAN_5.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_5.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

bool CAN5_GetMsg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_5.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_5.MB[0].ID.R;

	msg->std_id = CAN_5.MB[0].ID.B.ID_STD;
	msg->length = CAN_5.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_5.MB[0].DATA.B[j];
	}

	dummy = CAN_5.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_5.IFLAG1.B.BUF5I = 1;
	return(true);
}

uint8_t can5_send_msg(uint8_t ID, uint8_t* msg, uint8_t len)
{
	uint8_t i;
	CAN_MSG cMt;

	//帧ID
	cMt.std_id = ID;

    //查询位
	for(i=0;i<len;i++)
	{
		cMt.data[i] = msg[i];
	}
    //数据长度
	cMt.length = len;
	cMt.rtr = false;
    //优先等级
	cMt.prty = 5;

	//数据发送, 成功0,失败1;
    if(CAN5_SendMsg(cMt))
    	return 0;
    else
    	return 1;
}

//CAN7 initialization
void Init_CAN7(void)
{
	uint16_t i, j;

	CAN_7.MCR.B.MDIS = 1;       			/* Disable module before selecting clock source */
	CAN_7.CTRL1.B.CLKSRC = 0;     			/* Clock Source = oscillator clock (40 MHz) */
	CAN_7.MCR.B.MDIS = 0;       			/* Enable module for config. (Sets FRZ, HALT) */
	while (!CAN_7.MCR.B.FRZACK) {}			/* Wait for freeze acknowledge to set */

	CAN_7.CTRL1.R = 0x017A0087; 			/* 1000kbps */
	//CAN_7.CTRL1.R = 0x04DB0086;			/* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
	                              			/* PRESDIV+1 = Fclksrc/Ftq = 40 MHz/8MHz = 5 */
	                              			/*    so PRESDIV = 4 */
	                              			/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
	                              			/* PSEG1 = PSEG2 = 3 */
	                              			/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
	                              			/* RJW = Resync Jump Width - 1 = 4 = 1 */
	  	  	  	  	  	  	  	  			/* SMP = 1: use 3 bits per CAN sample */
	                              			/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz */

	for (i=0; i<96; i++) 					/* MPC574xG has 96 buffers after MPC5748G rev 0 */
		CAN_7.MB[i].CS.B.CODE = 0;  		/* Inactivate all message buffers */

	for (i=8; i<18; i++)
		CAN_7.MB[i].CS.B.CODE = 8;  		/* Message Buffer 8~17 set to TX INACTIVE */

	for (i=0; i<8; i++)
		CAN_7.RXIMR[i].R = 0x00000000;		/* set individual mask registers - no bits need to match */

	//CAN_0.RXMGMASK.R = 0x00000000;			/* Global acceptance mask */
	CAN_7.IMASK1.R = 0x000000E0;			/*enable interrupt from MB5*/

	/* RX FIFO structure is placed at address CAN_BASE_ADDRESS + 0x80 */
	/* First "ID filter table element 0" is placed at CAN_BASE_ADDRESS + 0xE0 */

	CAN_7.CTRL2.B.RFFN = 0x0;				/* Message buffers 0-7 are occupied by RX FIFO and ID filter table */
											/* There are 8 ID filter table elements */

	CAN_7.MCR.B.IDAM = 0x0;					/* One full ID (standard and extended) per ID Filter Table element */

	/* Initialize ID filter table elements 0-7 to receive messages with standard ID 0x555 (RTR and IDE = 0) */
	for(i = 0xE0, j = 0; i < 0xFF; i = i + 4, j++)
	{
		if(j < 4)
			*(unsigned int*)(CAN7_BASE_ADDRESS + i) = (0x60A + j) << 19;
		else if (j == 5)
			*(unsigned int*)(CAN7_BASE_ADDRESS + i) = 0x201 << 19;
		else if (j == 6)
			*(unsigned int*)(CAN7_BASE_ADDRESS + i) = 0x700 << 19;
		else
			*(unsigned int*)(CAN7_BASE_ADDRESS + i) = 0x601 << 19;
	}

	/* Enable RX FIFO, negate FlexCAN 0 halt state for 64 MBs, disable Self Reception, individual Rx Masking And Queue Enable */
	CAN_7.MCR.R = 0x2003003F;				/* RFEN = 1, SRXDIS = 1, IRMQ = 1 */

	while (CAN_7.MCR.B.FRZACK & CAN_7.MCR.B.NOTRDY) {} /* Wait to clear */
											/* Good practice: wait for FRZACK on freeze mode entry/exit */
}

bool CAN7_SendMsg(CAN_MSG msg)
{
	uint8_t	i, mb = 8;
	//Find MB which is INACTIVE
	for (i=8; i<18; i++)
	{
		if (CAN_7.MB[i].CS.B.CODE == 0x08)
		{
			mb = i;
			break;
		}
	}

	if (i > 17)
	{
		mb = 17;
	}

	CAN_7.MB[mb].CS.B.IDE = 0;       				/* Use standard ID length */
	CAN_7.MB[mb].ID.B.ID_STD = msg.std_id;				/* Transmit ID */
	CAN_7.MB[mb].CS.B.RTR = 0;       				/* Data frame, not remote Tx request frame */
	CAN_7.MB[mb].CS.B.DLC = msg.length ;			/* #bytes to transmit w/o null */

	for (i=0; i<msg.length; i++)
	{
		CAN_7.MB[mb].DATA.B[i] = msg.data[i];      	/* Data to be transmitted */
	}

	CAN_7.MB[mb].ID.B.PRIO = msg.prty;

	CAN_7.MB[mb].CS.B.SRR = 1;     					/* Tx frame (not req'd for standard frame) */
	CAN_7.MB[mb].CS.B.CODE = 0xC;   				/* Activate msg. buf. to transmit data frame */

	return(true);
}

bool CAN7_GetMsg(CAN_MSG* msg)
{
	uint8_t j;
	uint32_t __attribute__ ((unused)) dummy;

	/* mandatory - read control/status word - lock the MB */
	dummy = CAN_7.MB[0].CS.R;
	/* check the ID field */
	/* not really needed in this example */
	/* useful in case when multiple IDs are received
	 * by one message buffer (Rx mask feature) */
	dummy = CAN_7.MB[0].ID.R;

	msg->std_id = CAN_7.MB[0].ID.B.ID_STD;
	msg->length = CAN_7.MB[0].CS.B.DLC;
	for (j=0; j<msg->length; j++)
	{
		msg->data[j] = CAN_7.MB[0].DATA.B[j];
	}

	dummy = CAN_7.TIMER.R;             				/* Read TIMER to unlock message buffers */

	CAN_7.IFLAG1.B.BUF5I = 1;
	return(true);
}

uint8_t can7_send_msg(uint8_t ID, uint8_t* msg, uint8_t len)
{
	uint8_t i;
	CAN_MSG cMt;

	//帧ID
	cMt.std_id = ID;

    //查询位
	for(i=0;i<len;i++)
	{
		cMt.data[i] = msg[i];
	}
    //数据长度
	cMt.length = len;
	cMt.rtr = false;
    //优先等级
	cMt.prty = 5;

	//数据发送, 成功0,失败1;
    if(CAN7_SendMsg(cMt))
    	return 0;
    else
    	return 1;
}
