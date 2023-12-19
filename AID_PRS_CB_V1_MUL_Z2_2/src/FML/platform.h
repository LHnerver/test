/*
 * platform.h
 *
 *  Created on: 2020Äê7ÔÂ23ÈÕ
 *      Author: Su-xi
 */

#ifndef FML_PLATFORM_H_
#define FML_PLATFORM_H_

#include "pin.h"

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

extern void xcptn_xmpl(void);

void boot_cores(void);
void platform_init(void);

#endif /* FML_PLATFORM_H_ */
