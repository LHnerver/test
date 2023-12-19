/*
 * platform.c
 *
 *  Created on: 2020Äê7ÔÂ23ÈÕ
 *      Author: Su-xi
 */

#include "platform.h"

static void peri_clock_gating(void);
static void pll_160mhz(void);
static void system_160mhz(void);
static void irq_config(void);

/* Configures gating/enabling peripheral clocks for modes*/
static void peri_clock_gating (void)
{
	/* Gate off clock for all RUN modes */
	MC_ME.RUN_PC[0].R = 0x00000000;
	/* Configures peripheral clock for all RUN modes */
	MC_ME.RUN_PC[1].R = 0x000000FE;

	/* ADC1: select peripheral configuration to RUN_PC[1] */
	MC_ME.PCTL[25].B.RUN_CFG = 0x1;

//	/* I2C_0: select peripheral configuration RUN_PC[1] */
//	MC_ME.PCTL[30].B.RUN_CFG = 0x1;
	/* I2C_3: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[33].B.RUN_CFG = 0x1;

	/* PIT: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[91].B.RUN_CFG = 0x1;

	/* FlexCAN 0: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[70].B.RUN_CFG = 0x1;
	/* FlexCAN 1: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[71].B.RUN_CFG = 0x1;
	/* FlexCAN 2: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[72].B.RUN_CFG = 0x1;
	/* FlexCAN 3: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[73].B.RUN_CFG = 0x1;
//	/* FlexCAN 4: select peripheral configuration RUN_PC[1] */
//	MC_ME.PCTL[74].B.RUN_CFG = 0x1;
	/* FlexCAN 5: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[75].B.RUN_CFG = 0x1;
	/* FlexCAN 7: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[77].B.RUN_CFG = 0x1;

	/* LINFlex 1: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[51].B.RUN_CFG = 0x1;
//	/* LINFlex 3: select peripheral configuration RUN_PC[1] */
//	MC_ME.PCTL[53].B.RUN_CFG = 0x1;
	/* LINFlex 5: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[55].B.RUN_CFG = 0x1;
//	/* LINFlex 6: select peripheral configuration RUN_PC[1] */
//	MC_ME.PCTL[56].B.RUN_CFG = 0x1;
	/* LINFlex 7: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[57].B.RUN_CFG = 0x1;
	/* LINFlex 8: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[58].B.RUN_CFG = 0x1;
	/* LINFlex 10: select peripheral configuration RUN_PC[1] */
	MC_ME.PCTL[60].B.RUN_CFG = 0x1;
}

static void pll_160mhz(void)
{
	/* Connect XOSC to PLL */
	MC_CGM.AC5_SC.B.SELCTL=1;

	/* Configure PLL0 Dividers - 160MHz from 40Mhx XOSC */
	/* PLL input = FXOSC = 40MHz
	 	 VCO range = 600-1200MHz
	 */

	/* Divide input clock by 1, resulting clock will be reference input for the
	 * PLL analog loop */
	PLLDIG.PLLDV.B.PREDIV  = 1;
	/* Loop multiplication factor 16*40 MHz */
	PLLDIG.PLLDV.B.MFD     = 16;
	/* VCO post divider = 4 so 16*40/40 = 160MHz */
	PLLDIG.PLLDV.B.RFDPHI  = 1;

	/* Contains the denominator of the fractional loop division factor */
	PLLDIG.PLLCAL3.R = 0x09C3C000;	/* MFDEN = 9C3C000h = 163823616 */

	/* Sigma delta modulation enabled */
	PLLDIG.PLLFD.B.SMDEN = 1;

	/* switch to PLL */
	/* MVRON = 1, FLAON = RUN mode, PLLON=1, FXOSCON = 1, FIRCON = 1,
	 * SYSCLK = PLL PHI_0 */
	MC_ME.DRUN_MC.R = 0x00130072;

	/* MVRON = 1, FLAON = RUN mode, SIRCON = 1, PLLON=1, FXOSCON = 1,
	 * FIRCON = 1, SYSCLK = PLL PHI_0 */
	//MC_ME.DRUN_MC.R = 0x001300F2;

	/* Target mode = DRUN, KEY = 5AF0 */
	MC_ME.MCTL.R = 0x30005AF0;

	/* Target mode = DRUN, KEY = A50F */
	MC_ME.MCTL.R = 0x3000A50F;

	/* Wait for mode transition complete */
	while(MC_ME.GS.B.S_MTRANS == 1);
}

static void system_160mhz(void)
{
	/* F160 - max 160 MHz */
	MC_CGM.SC_DC0.B.DIV = 0;  /* Freq = sysclk / (0+1) = sysclk */
	MC_CGM.SC_DC0.B.DE  = 1;  /* Enable divided clock */

	/* S80 - max 80 MHz */
	/* MC_CGM_SC_DC1[DIV] and MC_CGM_SC_DC5[DIV] must be equal at all times */
	MC_CGM.SC_DC1.B.DIV = 1;  /* Freq = sysclk / (2+1) = sysclk/2 */
	MC_CGM.SC_DC1.B.DE  = 1;  /* Enable divided clock */

	/* FS80 - max 80 MHz */
	/* MC_CGM_SC_DC1[DIV] and MC_CGM_SC_DC5[DIV] must be equal at all times */
	MC_CGM.SC_DC5.R = MC_CGM.SC_DC1.R;  /* 80 MHz max */

	/* S40 - max 40 MHz */
	MC_CGM.SC_DC2.B.DIV = 3;  /* Freq = sysclk / (3+1) = sysclk/4 */
	MC_CGM.SC_DC2.B.DE  = 1;  /* Enable divided clock */

	/* F40 - max 40 MHz (for PIT, etc.) - use default values */

	/* F80 - max 80 MHz - use default values*/

	/* F20 - clock out configured at clock_out* function */

	pll_160mhz();
}

static void irq_config(void)
{
	/********************************* Core 0 *********************************/
	//PIT0
    INTC.PSR[226].B.PRC_SELN0 = 1; 	/* PIT0 IRQ sent to Core 0 */
    INTC.PSR[226].B.PRIN = 12;    	/* PIT0 IRQ priority = 12 (15 = highest) */

//	//FlexCAN_4
//	INTC.PSR[617].B.PRC_SELN0 = 1; 	/* FlexCAN_1 BUFF[4~7] IRQ sent to Core 0 */
//	INTC.PSR[617].B.PRIN = 4;     	/* FlexCAN_1 BUFF[4~7] IRQ priority = 4 */

	//LinFlexD_5
	INTC.PSR[391].B.PRC_SELN0 = 1; 	/* UART5 RX IRQ sent to Core 0 */
	INTC.PSR[391].B.PRIN = 5;    	/* UART5 RX IRQ priority = 5 */
	INTC.PSR[392].B.PRC_SELN0 = 1; 	/* UART5 TX IRQ sent to Core 0 */
	INTC.PSR[392].B.PRIN = 5;    	/* UART5 TX IRQ priority = 5 */

	//LinFlexD_7
	INTC.PSR[397].B.PRC_SELN0 = 1; 	/* UART7 RX IRQ sent to Core 0 */
	INTC.PSR[397].B.PRIN = 5;    	/* UART7 RX IRQ priority = 5 */
	INTC.PSR[398].B.PRC_SELN0 = 1; 	/* UART7 TX IRQ sent to Core 0 */
	INTC.PSR[398].B.PRIN = 5;    	/* UART7 TX IRQ priority = 5 */

	//LinFlexD_8
	INTC.PSR[400].B.PRC_SELN0 = 1; 	/* UART8 RX IRQ sent to Core 0 */
	INTC.PSR[400].B.PRIN = 5;    	/* UART8 RX IRQ priority = 5 */
	INTC.PSR[401].B.PRC_SELN0 = 1; 	/* UART8 TX IRQ sent to Core 0 */
	INTC.PSR[401].B.PRIN = 5;    	/* UART8 TX IRQ priority = 5 */

	//LinFlexD_10
	INTC.PSR[406].B.PRC_SELN0 = 1; 	/* UART10 RX IRQ sent to Core 0 */
	INTC.PSR[406].B.PRIN = 5;    	/* UART10 RX IRQ priority = 5 */
	INTC.PSR[407].B.PRC_SELN0 = 1; 	/* UART10 TX IRQ sent to Core 0 */
	INTC.PSR[407].B.PRIN = 5;    	/* UART10 TX IRQ priority = 5 */
	/********************************* Core 0 *********************************/

	/********************************* Core 1 *********************************/
	//PIT1
	INTC.PSR[227].B.PRC_SELN0 = 0; 	/* PIT1 IRQ not sent to Core 0 */
	INTC.PSR[227].B.PRC_SELN1 = 1; 	/* PIT1 IRQ sent to Core 1 */
	INTC.PSR[227].B.PRIN = 12;    	/* PIT1 IRQ priority = 12 (15 = highest) */

	//PIT3
	INTC.PSR[229].B.PRC_SELN0 = 0; 	/* PIT3 IRQ not sent to Core 0 */
	INTC.PSR[229].B.PRC_SELN1 = 1; 	/* PIT3 IRQ sent to Core 1 */
	INTC.PSR[229].B.PRIN = 12;    	/* PIT3 IRQ priority = 12 (15 = highest) */

	//FlexCAN_0
	INTC.PSR[569].B.PRC_SELN0 = 0; 	/* FlexCAN_0 BUFF[4~7] IRQ not sent to Core 0 */
	INTC.PSR[569].B.PRC_SELN1 = 1; 	/* FlexCAN_0 BUFF[4~7] IRQ sent to Core 0 */
	INTC.PSR[569].B.PRIN = 10;   	/* FlexCAN_0 BUFF[4~7] IRQ priority = 10 */

	//FlexCAN_1
	INTC.PSR[581].B.PRC_SELN0 = 0;
	INTC.PSR[581].B.PRC_SELN1 = 1;	/* FlexCAN_1 BUFF[4~7] IRQ sent to Core 1 */
	INTC.PSR[581].B.PRIN = 8;     	/* FlexCAN_1 BUFF[4~7] IRQ priority = 8 */

	//FlexCAN_2
	INTC.PSR[593].B.PRC_SELN0 = 0;
	INTC.PSR[593].B.PRC_SELN1 = 1;	/* FlexCAN_2 BUFF[4~7] IRQ sent to Core 1 */
	INTC.PSR[593].B.PRIN = 10;    	/* FlexCAN_2 BUFF[4~7] IRQ priority = 10 */

	//FlexCAN_3
	INTC.PSR[605].B.PRC_SELN0 = 0;
	INTC.PSR[605].B.PRC_SELN1 = 1;	/* FlexCAN_3 BUFF[4~7] IRQ sent to Core 1 */
	INTC.PSR[605].B.PRIN = 10;    	/* FlexCAN_3 BUFF[4~7] IRQ priority = 10 */

	//FlexCAN_5
	INTC.PSR[629].B.PRC_SELN0 = 0;
	INTC.PSR[629].B.PRC_SELN1 = 1;	/* FlexCAN_5 BUFF[4~7] IRQ sent to Core 1 */
	INTC.PSR[629].B.PRIN = 11;     	/* FlexCAN_5 BUFF[4~7] IRQ priority = 11 */

	//FlexCAN_7
	INTC.PSR[653].B.PRC_SELN0 = 0;
	INTC.PSR[653].B.PRC_SELN1 = 1;	/* FlexCAN_7 BUFF[4~7] IRQ sent to Core 1 */
	INTC.PSR[653].B.PRIN = 9;     	/* FlexCAN_7 BUFF[4~7] IRQ priority = 9 */
	/********************************* Core 1 *********************************/

	/********************************* Core 2 *********************************/
	//PIT2
	INTC.PSR[228].B.PRC_SELN0 = 0; 	/* PIT0 IRQ not sent to Core 0 */
	INTC.PSR[228].B.PRC_SELN2 = 1; 	/* PIT0 IRQ sent to Core 1 */
	INTC.PSR[228].B.PRIN = 12;    	/* PIT0 IRQ priority = 12 (15 = highest) */

	//LinFlexD_1
	INTC.PSR[379].B.PRC_SELN0 = 0; 	/* UART1 RX IRQ sent to Core 2 */
	INTC.PSR[379].B.PRC_SELN2 = 1; 	/* UART1 RX IRQ sent to Core 2 */
	INTC.PSR[379].B.PRIN = 5;    	/* UART1 RX IRQ priority = 5 */
	INTC.PSR[380].B.PRC_SELN0 = 0; 	/* UART1 TX IRQ sent to Core 2 */
	INTC.PSR[380].B.PRC_SELN2 = 1; 	/* UART1 TX IRQ sent to Core 2 */
	INTC.PSR[380].B.PRIN = 5;    	/* UART1 TX IRQ priority = 5 */
	/********************************* Core 2 *********************************/
}

void boot_cores(void)
{
#if defined(TURN_ON_CPU1) || defined(TURN_ON_CPU2)
	uint32_t mctl = MC_ME.MCTL.R;
#endif

#if defined(TURN_ON_CPU1)
	/* enable core 1 in all modes */
	MC_ME.CCTL[2].R = 0x00FE;
	/* Set Start address for core 1: Will reset and start */
    MC_ME.CADDR[2].R = 0x1280000 | 0x1;
#endif

#if defined(TURN_ON_CPU2)
	/* enable core 2 in all modes */
	MC_ME.CCTL[3].R = 0x00FE;
	/* Set Start address for core 2: Will reset and start */
    MC_ME.CADDR[3].R = 0x1500000 | 0x1;
#endif

#if defined(TURN_ON_CPU1) || defined(TURN_ON_CPU2)
	MC_ME.MCTL.R = (mctl & 0xffff0000ul) | KEY_VALUE1;
	MC_ME.MCTL.R =  mctl; /* key value 2 always from MCTL */
#endif
}


//void FCCU_Init(void)
//{
//	/* Unlock configuration */
//	FCCU.TRANS_LOCK.B.TRANSKEY = 0xBC;
//
//	/* provide Config state key */
//	FCCU.CTRLK.R = 0x913756AF;
//
//	/* enter config state - OP1 */
//	FCCU.CTRL.R = 0x1;
//
//	/* Verify if state change was sucessful */
//	while (FCCU.CTRL.B.OPS != 0x3);	//operation status successful
//
//	/* Configure FCCU to react on NCF14 with long/short functional reset */
//	//FCCU.NCFS_CFG[0].R = 0x20000000;	//long functional reset reaction
//	FCCU.NCFS_CFG[0].B.NCFSC14 = 0x1;		//short functional reset reaction
//
//	//enable fault 14
//	FCCU.NCF_E[0].B.NCFE14 = 0x1;
//
//	//set up the NOMAL mode of FCCU
//	FCCU.CTRLK.R = 0x825A132B;		//key for OP2
//	FCCU.CTRL.R = 0x2;				//set the OP2 - set up FCCU into the NORMAL mode
//	while (FCCU.CTRL.B.OPS != 0x3);	//operational status succesfull
//
//}//FCCU_Init
//
//
//void Init_SWT0(void)
//{
//	/* Provide a Key to unlock SWT configuraiton registers */
//	SWT_0.SR.R = 0xC520;
//	SWT_0.SR.R = 0xD928;
//	/* Wait for solf-lock bit */
//	while (SWT_0.CR.B.SLK == 0x1);
//
//	//MAP = 0xFF(Enable Access for the Master); SMD = 0(Fixed Service Sequence); RIA = 1(Reset on Invalid Access);
//	//WND = 0(Regular mode); ITR = 0(Generate a reset request on a time-out); HLK = 0; SLK = 0; STP = 0;
//	//FRZ = 1(SWT counter is stopped in debug mode); WEN = 1(SWT is enabled)
//	SWT_0.CR.R = 0xFF000100;
//	/* Set SWT timeout */
//	SWT_0.TO.R = 128*3000;		//SIRC(128kHz)   1s
//
//	/* Enable SWT_0 */
//	SWT_0.CR.R = 0xFF000101;
//}
//
//void SWT0_Service(void)
//{
//	/* Fixed Service Sequence Mode */
//	SWT_0.SR.R = 0xA602;
//	SWT_0.SR.R = 0xB480;
//}
//
//void Init_SWT1(void)
//{
//	/* Provide a Key to unlock SWT configuraiton registers */
//	SWT_1.SR.R = 0xC520;
//	SWT_1.SR.R = 0xD928;
//	/* Wait for solf-lock bit */
//	while (SWT_1.CR.B.SLK == 0x1);
//
//	//MAP = 0xFF(Enable Access for the Master); SMD = 0(Fixed Service Sequence); RIA = 1(Reset on Invalid Access);
//	//WND = 0(Regular mode); ITR = 0(Generate a reset request on a time-out); HLK = 0; SLK = 0; STP = 0;
//	//FRZ = 1(SWT counter is stopped in debug mode); WEN = 1(SWT is enabled)
//	SWT_1.CR.R = 0xFF000100;
//	/* Set SWT timeout */
//	SWT_1.TO.R = 128000;		//SIRC(128kHz)   1s
//
//	/* Enable SWT_1 */
//	SWT_1.CR.R = 0xFF000101;
//}
//
//void SWT1_Service(void)
//{
//	/* Fixed Service Sequence Mode */
//	SWT_1.SR.R = 0xA602;
//	SWT_1.SR.R = 0xB480;
//}
//
//void Init_SWT2(void)
//{
//	/* Provide a Key to unlock SWT configuraiton registers */
//	SWT_2.SR.R = 0xC520;
//	SWT_2.SR.R = 0xD928;
//	/* Wait for solf-lock bit */
//	while (SWT_2.CR.B.SLK == 0x1);
//
//	//MAP = 0xFF(Enable Access for the Master); SMD = 0(Fixed Service Sequence); RIA = 1(Reset on Invalid Access);
//	//WND = 0(Regular mode); ITR = 0(Generate a reset request on a time-out); HLK = 0; SLK = 0; STP = 0;
//	//FRZ = 1(SWT counter is stopped in debug mode); WEN = 1(SWT is enabled)
//	SWT_2.CR.R = 0xFF000100;
//	/* Set SWT timeout */
//	SWT_2.TO.R = 128000;		//SIRC(128kHz)   1s
//
//	/* Enable SWT_2 */
//	SWT_2.CR.R = 0xFF000101;
//}
//
//void SWT2_Service(void)
//{
//	/* Fixed Service Sequence Mode */
//	SWT_2.SR.R = 0xA602;
//	SWT_2.SR.R = 0xB480;
//}

void platform_init(void)
{
	peri_clock_gating();

	system_160mhz();

	//FCCU_Init();

	irq_config();

	pins_init();
}
