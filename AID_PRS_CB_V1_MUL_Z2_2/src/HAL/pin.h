/*
 * pin.h
 *
 *  Created on: 2020年7月23日
 *      Author: Su-xi
 */

#ifndef HAL_PIN_H_
#define HAL_PIN_H_

#include "derivative.h"
#include "project.h"

#define USR_LED1 	SIUL2.GPDO[PJ3].B.PDO_4n
#define USR_LED2 	SIUL2.GPDO[PJ2].B.PDO_4n
//#define LED3 	  SIUL2.GPDO[143].B.PDO_4n

// 接管踏板、手柄按键IO
#define MAN_DH   	SIUL2.GPDI[PF3].B.PDI_4n
#define MAN_DP   	SIUL2.GPDI[PF4].B.PDI_4n

// 启动自动驾驶IO
#define AUTO_DI5   	SIUL2.GPDI[PF5].B.PDI_4n
#define AUTO_DI6   	SIUL2.GPDI[PF6].B.PDI_4n

// LOGO按钮IO
#define SW_PWR   	SIUL2.GPDI[PH14].B.PDI_4n
#define SW_LED      SIUL2.GPDO[PH15].B.PDO_4n

//#define ZERO_SW   SIUL2.GPDI[90].B.PDI_4n
//
//#define ESTOP1    SIUL2.GPDI[117].B.PDI_4n
//#define ESTOP2    SIUL2.GPDI[118].B.PDI_4n
//
//#define EBR   	  SIUL2.GPDI[116].B.PDI_4n

#define BEEP      SIUL2.GPDO[PB12].B.PDO_4n

#define RS485_RD7  	SIUL2.GPDO[PA14].B.PDO_4n
#define RS485_RD6  	SIUL2.GPDO[PA15].B.PDO_4n

#define PWR_24V_PS2  	SIUL2.GPDO[PA10].B.PDO_4n

#define PWR_CTR1  	SIUL2.GPDO[PD12].B.PDO_4n
#define PWR_CTR2  	SIUL2.GPDO[PD13].B.PDO_4n
#define PWR_CTR3  	SIUL2.GPDO[PD14].B.PDO_4n
#define PWR_CTR4  	SIUL2.GPDO[PD15].B.PDO_4n

void pins_init(void);

#endif /* HAL_PIN_H_ */
