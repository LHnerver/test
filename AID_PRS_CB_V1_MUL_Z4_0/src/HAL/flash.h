/*
 * flash.h
 *
 *  Created on: 2019Äê2ÔÂ19ÈÕ
 *      Author: Su-xi
 */

#ifndef FML_FLASH_H_
#define FML_FLASH_H_

#include "derivative.h"

#define  CALI_ADDR       0xFAC000
#define  BOOT_ADDR       0xFB8000

void flash_set_boot_data(void);
void flash_save_cali_data(REGISTER_INFO reg_info,CALI_DATA cali_data,DEVICE_INFO dev_data);
void flash_get_cali_data(REGISTER_INFO* p_reg_info,CALI_DATA* p_cali_data,DEVICE_INFO* p_dev_data);

#endif /* DRIVERS_FLASH_H_ */
