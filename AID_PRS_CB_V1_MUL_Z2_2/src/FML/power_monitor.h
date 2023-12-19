#ifndef _POWER_MONITOR_H_
#define _POWER_MONITOR_H_

/*******************************************************************************
* Includes
*******************************************************************************/
#include "typedefs.h"
#include "adc.h"
//#include "iic.h"

//#define LTC2945_POWER_MSB2_REG              	0x05
//#define LTC2945_DELTA_SENSE_MSB_REG        		0x14
//#define LTC2945_VIN_MSB_REG                 	0x1E
//#define LTC2945_ADIN_MSB_REG                  	0x28
//
//#define LTC2945_ADDR_IN                      	0xD2
//#define LTC2945_ADDR_OUT1                   	0xDE
//#define LTC2945_ADDR_OUT2                     	0xCE
//#define LTC2945_ADDR_OUT3                     	0xD8
//#define LTC2945_ADDR_OUT4                     	0xD0

void read_power_data(POWER_INFO* pmp);
//void power_monitor_init(void);

#endif
