/*
 * pin.c
 *
 *  Created on: 2020Äê7ÔÂ23ÈÕ
 *      Author: Su-xi
 */

#include "pin.h"

void pins_init(void)
{
	//USR_LED1 PJ3
	SIUL2.MSCR[PJ3].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PJ3].B.OBE = 1;          /* Output Buffer Enable on */
	SIUL2.MSCR[PJ3].B.IBE = 0;			/* Input Buffer Enable off */
	SIUL2.GPDO[PJ3].B.PDO_4n = 0;		/* Turn LED one, 0 for ON, 1 for OFF */

	//USR_LED2 PJ2
	SIUL2.MSCR[PJ2].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PJ2].B.OBE = 1;          /* Output Buffer Enable on */
	SIUL2.MSCR[PJ2].B.IBE = 0;			/* Input Buffer Enable off */
	SIUL2.GPDO[PJ2].B.PDO_4n = 1;		/* Turn LED off, 0 for ON, 1 for OFF */

	//PWR CTRL1 PA14	RS485_RD7
	SIUL2.MSCR[PA14].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PA14].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PA14].B.IBE = 0;   		/* Input Buffer Enable on */
	RS485_RD7 = 1;

	//PWR CTRL1 PA15	RS485_RD6
	SIUL2.MSCR[PA15].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PA15].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PA15].B.IBE = 0;   		/* Input Buffer Enable on */
	RS485_RD6 = 1;

	//PWR CTRL1 PA10
	SIUL2.MSCR[PA10].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PA10].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PA10].B.IBE = 0;   		/* Input Buffer Enable on */
	PWR_24V_PS2 = 1;

	//PWR CTRL1 PD12
	SIUL2.MSCR[PD12].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PD12].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PD12].B.IBE = 0;   		/* Input Buffer Enable on */
	PWR_CTR1 = 1;

	//PWR CTRL1 PD13
	SIUL2.MSCR[PD13].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PD13].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PD13].B.IBE = 0;   		/* Input Buffer Enable on */
	PWR_CTR2 = 1;

	//PWR CTRL1 PD14
	SIUL2.MSCR[PD14].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PD14].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PD14].B.IBE = 0;   		/* Input Buffer Enable on */
	PWR_CTR3 = 1;

	//PWR CTRL1 PD15
	SIUL2.MSCR[PD15].B.SSS = 0;   		/* Pin functionality as GPIO */
	SIUL2.MSCR[PD15].B.OBE = 1;         /* Output Buffer Enable off */
	SIUL2.MSCR[PD15].B.IBE = 0;  		/* Input Buffer Enable on */
	PWR_CTR4 = 1;

	//MAN_DH PF3
	SIUL2.MSCR[PF3].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PF3].B.OBE = 0;          /* Output Buffer Enable off */
	SIUL2.MSCR[PF3].B.IBE = 1;			/* Input Buffer Enable on */

	//MAN_DP PF4
	SIUL2.MSCR[PF4].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PF4].B.OBE = 0;          /* Output Buffer Enable off */
	SIUL2.MSCR[PF4].B.IBE = 1;			/* Input Buffer Enable on */

	//AUTO_DI5 PF5
	SIUL2.MSCR[PF5].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PF5].B.OBE = 0;          /* Output Buffer Enable off */
	SIUL2.MSCR[PF5].B.IBE = 1;			/* Input Buffer Enable on */

	//AUTO_DI6 PF6
	SIUL2.MSCR[PF6].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PF6].B.OBE = 0;          /* Output Buffer Enable off */
	SIUL2.MSCR[PF6].B.IBE = 1;			/* Input Buffer Enable on */

	//SW_PWR_EN PH14
	SIUL2.MSCR[PH14].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PH14].B.OBE = 0;         /* Output Buffer Enable off */
	SIUL2.MSCR[PH14].B.IBE = 1;			/* Input Buffer Enable on */

	//SW_LED_EN PH15
	SIUL2.MSCR[PH15].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[PH15].B.OBE = 1;         /* Output Buffer Enable on */
	SIUL2.MSCR[PH15].B.IBE = 0;			/* Input Buffer Enable off */
	SIUL2.GPDO[PH15].B.PDO_4n = 0;		/* Turn LED on, 0 for ON, 1 for OFF */

#if 0
	//MAN-SW1 PF5
	SIUL2.MSCR[85].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[85].B.OBE = 0;         	/* Output Buffer Enable off */
	SIUL2.MSCR[85].B.IBE = 1;			/* Input Buffer Enable on */

	//MAN-SW2 PF6
	SIUL2.MSCR[86].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[86].B.OBE = 0;         	/* Output Buffer Enable off */
	SIUL2.MSCR[86].B.IBE = 1;			/* Input Buffer Enable on */

	//MOD-SW1 PF11
	SIUL2.MSCR[91].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[91].B.OBE = 0;         	/* Output Buffer Enable off */
	SIUL2.MSCR[91].B.IBE = 1;			/* Input Buffer Enable on */

	//MOD-SW2 PI15
	//	SIUL2.MSCR[143].B.SSS = 0;			/* Pin functionality as GPIO */
	//	SIUL2.MSCR[143].B.OBE = 0;         	/* Output Buffer Enable off */
	//	SIUL2.MSCR[143].B.IBE = 1;			/* Input Buffer Enable on */

	SIUL2.MSCR[143].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[143].B.OBE = 1;          /* Output Buffer Enable on */
	SIUL2.MSCR[143].B.IBE = 0;			/* Input Buffer Enable off */
	SIUL2.GPDO[143].B.PDO_4n = 0;		/* Turn LED on, 0 for ON, 1 for OFF */

	//ZERO-SW PF10
	SIUL2.MSCR[90].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[90].B.OBE = 0;         	/* Output Buffer Enable off */
	SIUL2.MSCR[90].B.IBE = 1;			/* Input Buffer Enable on */

	//EBR PH4
	SIUL2.MSCR[116].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[116].B.OBE = 1;          /* Output Buffer Enable on */
	SIUL2.MSCR[116].B.IBE = 0;			/* Input Buffer Enable off */
	EBR = 1;

	//ESTOP1 PH5
	SIUL2.MSCR[117].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[117].B.OBE = 0;          /* Output Buffer Enable on */
	SIUL2.MSCR[117].B.IBE = 1;			/* Input Buffer Enable off */

	//ESTOP2 PH6
	SIUL2.MSCR[118].B.SSS = 0;			/* Pin functionality as GPIO */
	SIUL2.MSCR[118].B.OBE = 0;          /* Output Buffer Enable on */
	SIUL2.MSCR[118].B.IBE = 1;			/* Input Buffer Enable off */

	//  /* PW1-PA5 */
	//	SIUL2.MSCR[5].B.SSS = 0;		 /* Pin functionality as GPIO */
	//	SIUL2.MSCR[5].B.OBE = 1;         /* Output Buffer Enable on */
	//	SIUL2.MSCR[5].B.IBE = 0;		 /* Input Buffer Enable off */
	//	SIUL2.GPDO[5].B.PDO_4n = 1;		 /* Set High */
	//
	//	/* PW2-PA6 */
	//	SIUL2.MSCR[6].B.SSS = 0;		 /* Pin functionality as GPIO */
	//	SIUL2.MSCR[6].B.OBE = 1;         /* Output Buffer Enable on */
	//	SIUL2.MSCR[6].B.IBE = 0;		 /* Input Buffer Enable off */
	//	SIUL2.GPDO[6].B.PDO_4n = 1;		 /* Set High */

	//	/* PWCTL1-PD2 */
	//	SIUL2.MSCR[50].B.SSS = 0;		 /* Pin functionality as GPIO */
	//	SIUL2.MSCR[50].B.OBE = 1;        /* Output Buffer Enable on */
	//	SIUL2.MSCR[50].B.IBE = 0;		 /* Input Buffer Enable off */
	//	SIUL2.GPDO[50].B.PDO_4n = 1;	 /* Set High */
	//
	//	/* PWCTL2-PD3 */
	//	SIUL2.MSCR[51].B.SSS = 0;		 /* Pin functionality as GPIO */
	//	SIUL2.MSCR[51].B.OBE = 1;        /* Output Buffer Enable on */
	//	SIUL2.MSCR[51].B.IBE = 0;		 /* Input Buffer Enable off */
	//	SIUL2.GPDO[51].B.PDO_4n = 1;	 /* Set High */
	//
	//	/* PWCTL3-PD4 */
	//	SIUL2.MSCR[52].B.SSS = 0;		 /* Pin functionality as GPIO */
	//	SIUL2.MSCR[52].B.OBE = 1;        /* Output Buffer Enable on */
	//	SIUL2.MSCR[52].B.IBE = 0;		 /* Input Buffer Enable off */
	//	SIUL2.GPDO[52].B.PDO_4n = 1;	 /* Set High */
#endif

	/* BEEP-PB12 */
	SIUL2.MSCR[28].B.SSS = 0;		 /* Pin functionality as GPIO */
	SIUL2.MSCR[28].B.OBE = 1;        /* Output Buffer Enable on */
	SIUL2.MSCR[28].B.IBE = 0;		 /* Input Buffer Enable off */
	SIUL2.GPDO[28].B.PDO_4n = 0;	 /* Set High */

//	//I2C_0 SDA SIGNAL PB2
//	SIUL2.MSCR[18].B.OBE = 1;
//	SIUL2.MSCR[18].B.ODE = 1;
//	SIUL2.MSCR[18].B.IBE = 1;
//	SIUL2.MSCR[18].B.PUS = 0;
//	SIUL2.MSCR[18].B.PUE = 1;
//	SIUL2.MSCR[18].B.SSS = 2;
//	SIUL2.IMCR[266].R = 2;
//
//	//I2C_0 SCL SIGNAL PB3
//	SIUL2.MSCR[19].B.OBE = 1;
//	SIUL2.MSCR[19].B.ODE = 1;
//	SIUL2.MSCR[19].B.IBE = 1;
//	SIUL2.MSCR[19].B.PUS = 0;
//	SIUL2.MSCR[19].B.PUE = 1;
//	SIUL2.MSCR[19].B.SSS = 2;
//	SIUL2.IMCR[265].R = 2;

	//I2C_3 SDA SIGNAL PE10
	SIUL2.MSCR[PE10].B.OBE = 1;
	SIUL2.MSCR[PE10].B.ODE = 1;
	SIUL2.MSCR[PE10].B.IBE = 1;
	SIUL2.MSCR[PE10].B.PUS = 0;
	SIUL2.MSCR[PE10].B.PUE = 1;
	SIUL2.MSCR[PE10].B.SSS = 4;
	SIUL2.IMCR[272].R = 1;

	//I2C_3 SCL SIGNAL PE11
	SIUL2.MSCR[PE11].B.OBE = 1;
	SIUL2.MSCR[PE11].B.ODE = 1;
	SIUL2.MSCR[PE11].B.IBE = 1;
	SIUL2.MSCR[PE11].B.PUS = 0;
	SIUL2.MSCR[PE11].B.PUE = 1;
	SIUL2.MSCR[PE11].B.SSS = 4;
	SIUL2.IMCR[271].R = 1;


	/* FlexCAN_0 */
	SIUL2.MSCR[16].B.SSS = 1;  	/* Pad PB0: Source signal is CAN0_TX  */
	SIUL2.MSCR[16].B.OBE = 1;  	/* Pad PB0: Output Buffer Enable */
	SIUL2.MSCR[16].B.SRC = 3;  	/* Pad PB0: Maximum slew rate */
	SIUL2.MSCR[17].B.IBE = 1;  	/* Pad PB1: Enable pad for input - CAN0_RX */
	SIUL2.IMCR[188].B.SSS = 2; 	/* CAN0_RX: connected to pad PB1 */

	/* FlexCAN_1 */
	SIUL2.MSCR[42].B.SSS = 1;  	/* Pad PC10: Source signal is CAN1_TX  */
	SIUL2.MSCR[42].B.OBE = 1;  	/* Pad PC10: Output Buffer Enable */
	SIUL2.MSCR[42].B.SRC = 3;	/* Pad PC10: Maximum slew rate */
	SIUL2.MSCR[43].B.IBE = 1;	/* Pad PC11: Enable pad for input - CAN1_RX */
	SIUL2.IMCR[189].B.SSS = 3;	/* CAN1_RX: connected to pad PC11 */

	/* FlexCAN_2 */
	SIUL2.MSCR[88].B.SSS = 3;  	/* Pad PF8: Source signal is CAN2_TX  */
	SIUL2.MSCR[88].B.OBE = 1;  	/* Pad PF8: Output Buffer Enable */
	SIUL2.MSCR[88].B.SRC = 3;	/* Pad PF8: Maximum slew rate */
	SIUL2.MSCR[89].B.IBE = 1;	/* Pad PF9: Enable pad for input - CAN2_RX */
	SIUL2.IMCR[190].B.SSS = 2;	/* CAN2_RX: connected to pad PF9 */

	/* FlexCAN_3 */
	SIUL2.MSCR[72].B.SSS = 3;  	/* Pad PE8: Source signal is CAN3_TX  */
	SIUL2.MSCR[72].B.OBE = 1;  	/* Pad PE8: Output Buffer Enable */
	SIUL2.MSCR[72].B.SRC = 3;	/* Pad PE8: Maximum slew rate */
	SIUL2.MSCR[73].B.IBE = 1;	/* Pad PE9: Enable pad for input - CAN3_RX */
	SIUL2.IMCR[191].B.SSS = 3;	/* CAN3_RX: connected to pad PE9 */

//	/* FlexCAN_4 */
//	SIUL2.MSCR[94].B.SSS = 1;  	/* Pad PF14: Source signal is CAN3_TX  */
//	SIUL2.MSCR[94].B.OBE = 1;  	/* Pad PF14: Output Buffer Enable */
//	SIUL2.MSCR[94].B.SRC = 3;	/* Pad PF14: Maximum slew rate */
//	SIUL2.MSCR[95].B.IBE = 1;	/* Pad PF15: Enable pad for input - CAN4_RX */
//	SIUL2.IMCR[192].B.SSS = 3;	/* CAN3_RX: connected to pad PF15 */

	/* FlexCAN_5 */
	SIUL2.MSCR[65].B.SSS = 2;  	/* Pad PE1: Source signal is CAN5_TX  */
	SIUL2.MSCR[65].B.OBE = 1;  	/* Pad PE1: Output Buffer Enable */
	SIUL2.MSCR[65].B.SRC = 3;	/* Pad PE1: Maximum slew rate */
	SIUL2.MSCR[64].B.IBE = 1;	/* Pad PE0: Enable pad for input - CAN5_RX */
	SIUL2.IMCR[193].B.SSS = 1;	/* CAN5_RX: connected to pad PE0 */

	/* FlexCAN_7 */
	SIUL2.MSCR[98].B.SSS = 3;  	/* Pad PG2: Source signal is CAN7_TX  */
	SIUL2.MSCR[98].B.OBE = 1;  	/* Pad PG2: Output Buffer Enable */
	SIUL2.MSCR[98].B.SRC = 3;  	/* Pad PG2: Maximum slew rate */
	SIUL2.MSCR[99].B.IBE = 1;  	/* Pad PG3: Enable pad for input - CAN7_RX */
	SIUL2.IMCR[195].B.SSS = 1; 	/* CAN7_RX: connected to pad PG3 */

	/* LINFlexD_1 for TTL */
	SIUL2.MSCR[38].B.SSS = 1;        	/* Pad PC6: Source signal is LIN1_TX */
	SIUL2.MSCR[38].B.OBE = 1;        	/* Pad PC6: OBE=1. */
	SIUL2.MSCR[38].B.SRC = 3;        	/* Pad PC6: Full strength slew rate */
	SIUL2.MSCR[39].B.IBE = 1;        	/* Pad PC7: Enable pad for input */
	SIUL2.IMCR[201].B.SSS = 1;        	/* LIN1_RX : connected to pad PC7 */

	/* LINFlexD_5 for RS232 */
	SIUL2.MSCR[92].B.SSS = 2;        	/* Pad PF12: Source signal is LIN5_TX */
	SIUL2.MSCR[92].B.OBE = 1;        	/* Pad PF12: OBE=1. */
	SIUL2.MSCR[92].B.SRC = 3;        	/* Pad PF12: Full strength slew rate */
	SIUL2.MSCR[93].B.IBE = 1;        	/* Pad PF13: Enable pad for input */
	SIUL2.IMCR[205].B.SSS = 2;        	/* LIN5_RX : connected to pad PF13 */

	/* LINFlexD_7 for RS485 */
	SIUL2.MSCR[PG8].B.SSS = 2;        	/* Pad PG8: Source signal is LIN7_TX */
	SIUL2.MSCR[PG8].B.OBE = 1;        	/* Pad PG8: OBE=1. */
	SIUL2.MSCR[PG8].B.SRC = 3;        	/* Pad PG8: Full strength slew rate */
	SIUL2.MSCR[PG9].B.IBE = 1;        	/* Pad PG9: Enable pad for input */
	SIUL2.IMCR[207].B.SSS = 1;        	/* LIN7_RX : connected to pad PG9 */

	/* LINFlexD_8 for RS232 */
	SIUL2.MSCR[PG14].B.SSS = 2;        	/* Pad PG14: Source signal is LIN8_TX */
	SIUL2.MSCR[PG14].B.OBE = 1;        	/* Pad PG14: OBE=1. */
	SIUL2.MSCR[PG14].B.SRC = 3;        	/* Pad PG14: Full strength slew rate */
	SIUL2.MSCR[PG15].B.IBE = 1;        	/* Pad PG15: Enable pad for input */
	SIUL2.IMCR[208].B.SSS = 1;        	/* LIN8_RX : connected to pad PG15 */

	/* LINFlexD_10 for RS232 */
	SIUL2.MSCR[100].B.SSS = 3;        	/* Pad PG4: Source signal is LIN10_TX */
	SIUL2.MSCR[100].B.OBE = 1;        	/* Pad PG4: OBE=1. */
	SIUL2.MSCR[100].B.SRC = 3;        	/* Pad PG4: Full strength slew rate */
	SIUL2.MSCR[101].B.IBE = 1;        	/* Pad PG5: Enable pad for input */
	SIUL2.IMCR[210].B.SSS = 1;        	/* LIN10_RX : connected to pad PG5 */

	/* ADC1 channel 0 for ADC */
	SIUL2.MSCR[PD0].B.APC = 1;    		/* PD0 = function ADC1_P[4] = ADC1 channel 0 */
	/* ADC1 channel 1 for ADC */
	SIUL2.MSCR[PD1].B.APC = 1;    		/* PD1 = function ADC1_P[5] = ADC1 channel 1 */
	/* ADC1 channel 2 for ADC */
	SIUL2.MSCR[PD2].B.APC = 1;    		/* PD2 = function ADC1_P[6] = ADC1 channel 2 */
	/* ADC1 channel 3 for ADC */
	SIUL2.MSCR[PD3].B.APC = 1;    		/* PD3 = function ADC1_P[7] = ADC1 channel 3 */
	/* ADC1 channel 4 for ADC */
	SIUL2.MSCR[PD4].B.APC = 1;    		/* PD4 = function ADC1_P[8] = ADC1 channel 4 */
}
