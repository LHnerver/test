/*
 * bootloader.h
 *
 *  Created on: 2020Äê11ÔÂ26ÈÕ
 *      Author: Su-xi
 */

#ifndef FML_BOOTLOADER_H_
#define FML_BOOTLOADER_H_

#include "typedefs.h"
#include "linflexd_uart.h"
#include "flash.h"

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

void boot_init(void);
void boot_execute(void);

#endif /* FML_BOOTLOADER_H_ */
