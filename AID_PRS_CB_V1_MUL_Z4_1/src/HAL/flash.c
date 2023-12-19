/*
 * flash.c
 *
 *  Created on: 2019年2月19日
 *      Author: Su-xi
 */
#include "flash.h"

void flash_erase(unsigned int addr)
{
	/* Erase the mid block */
	if (addr >= 0xFAC000 && addr < 0xFAFFFF)
	{
		C55FMC.MCR.B.ERS = 1;
		C55FMC.SEL0.R = 0x00000080;	/* Select the mid block(bit24) */
	}
	else if (addr >= 0xFB8000 && addr < 0xFBFFFF)
	{
		C55FMC.MCR.B.ERS = 1;
		C55FMC.SEL0.R = 0x00000200;	/* Select the mid block(bit22) */
	}

	*(unsigned int*)(addr +  0) = 0xFFFFFFFF;	/* Interlock write */
//	*(unsigned int*)(addr +  4) = 0xFFFFFFFF;  	/* Up to 8 words (one 256bit page) can be programmed at once */
//	*(unsigned int*)(addr +  8) = 0xFFFFFFFF;
//	*(unsigned int*)(addr + 12) = 0xFFFFFFFF;
//	*(unsigned int*)(addr + 16) = 0xFFFFFFFF;
//	*(unsigned int*)(addr + 20) = 0xFFFFFFFF;
//	*(unsigned int*)(addr + 24) = 0xFFFFFFFF;
//	*(unsigned int*)(addr + 28) = 0xFFFFFFFF;

	C55FMC.MCR.B.EHV = 1;
	while(C55FMC.MCR.B.DONE == 0);
	C55FMC.MCR.B.EHV = 0;
	C55FMC.MCR.B.ERS = 0;
}

void flash_set_boot_data(void)
{
	uint32_t i = 10000;

	/* Unlock mid block 0x00FB_8000 - 0x00FB_FFFF(bit22) */
	C55FMC.LOCK0.R &= 0xFFFFFDFF;

	//先擦除整页
	flash_erase(BOOT_ADDR);

	/* Program one page in the mid block */
	C55FMC.MCR.B.PGM = 1;

	*(unsigned int*)(BOOT_ADDR +  0) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR +  4) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR +  8) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR + 12) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR + 16) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR + 20) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR + 24) = 0x00000000;
//	*(unsigned int*)(BOOT_ADDR + 28) = 0x00000000;

	C55FMC.MCR.B.EHV = 1;
	while(C55FMC.MCR.B.DONE == 0);
	C55FMC.MCR.B.EHV = 0;
	C55FMC.MCR.B.PGM = 0;

	while(i--);
}

void flash_save_cali_data(REGISTER_INFO reg_info,CALI_DATA cali_data,DEVICE_INFO dev_data)
{
	uint32_t i = 10000;

	/* Unlock mid block 0x00FA_C000 - 0x00FA_FFFF(bit24) */
	C55FMC.LOCK0.R &= 0xFFFFFF7F;

	//先擦除整页
	flash_erase(CALI_ADDR);

	/* Program one page in the mid block */
	C55FMC.MCR.B.PGM = 1;

	*(unsigned int*)(CALI_ADDR + 0) = reg_info.r;

	//[1-48] address save calibration data
	*(unsigned int*)(CALI_ADDR + 4) = cali_data.sr_left_angle;
	*(unsigned int*)(CALI_ADDR + 8) = cali_data.sr_right_angle;
	*(unsigned int*)(CALI_ADDR + 12) = cali_data.br_max_angle;
	*(unsigned int*)(CALI_ADDR + 16) = cali_data.ar_max_angle;
	*(unsigned int*)(CALI_ADDR + 20) = cali_data.ep_max_angle;

	*(unsigned int*)(CALI_ADDR + 24) = cali_data.sr_acc_dec;
	*(unsigned int*)(CALI_ADDR + 28) = cali_data.br_speed;
	*(unsigned int*)(CALI_ADDR + 32) = cali_data.br_acc_dec;
	*(unsigned int*)(CALI_ADDR + 36) = cali_data.ar_speed;
	*(unsigned int*)(CALI_ADDR + 40) = cali_data.ar_acc_dec;

	*(unsigned int*)(CALI_ADDR + 44) = cali_data.ep_encoder_up;
	*(unsigned int*)(CALI_ADDR + 48) = cali_data.ep_encoder_down;

	//[52-60] address save device info
	*(unsigned int*)(CALI_ADDR + 52) = dev_data.hardware_ver;
	*(unsigned int*)(CALI_ADDR + 56) = dev_data.serial_num;
	*(unsigned int*)(CALI_ADDR + 60) = dev_data.device_id;

	C55FMC.MCR.B.EHV = 1;
	while(C55FMC.MCR.B.DONE == 0);
	C55FMC.MCR.B.EHV = 0;
	C55FMC.MCR.B.PGM = 0;

	while(i--);
}

void flash_get_cali_data(REGISTER_INFO* p_reg_info,CALI_DATA* p_cali_data,DEVICE_INFO* p_dev_data)
{
	// 读取数据
	p_reg_info->r 				 = *(unsigned int*)(CALI_ADDR + 0);

	p_cali_data->sr_left_angle 	 = *(unsigned int*)(CALI_ADDR + 4);
	p_cali_data->sr_right_angle  = *(unsigned int*)(CALI_ADDR + 8);
	p_cali_data->br_max_angle 	 = *(unsigned int*)(CALI_ADDR + 12);
	p_cali_data->ar_max_angle 	 = *(unsigned int*)(CALI_ADDR + 16);
	p_cali_data->ep_max_angle 	 = *(unsigned int*)(CALI_ADDR + 20);

	p_cali_data->sr_acc_dec 	 = *(unsigned int*)(CALI_ADDR + 24);
	p_cali_data->br_speed 		 = *(unsigned int*)(CALI_ADDR + 28);
	p_cali_data->br_acc_dec 	 = *(unsigned int*)(CALI_ADDR + 32);
	p_cali_data->ar_speed 		 = *(unsigned int*)(CALI_ADDR + 36);
	p_cali_data->ar_acc_dec 	 = *(unsigned int*)(CALI_ADDR + 40);

	p_cali_data->ep_encoder_up 	 = *(unsigned int*)(CALI_ADDR + 44);
	p_cali_data->ep_encoder_down = *(unsigned int*)(CALI_ADDR + 48);

	p_dev_data->hardware_ver 	 = *(unsigned int*)(CALI_ADDR + 52);
	p_dev_data->serial_num 		 = *(unsigned int*)(CALI_ADDR + 56);
	p_dev_data->device_id 		 = *(unsigned int*)(CALI_ADDR + 60);

	// 检查数据有效性
	if (p_cali_data->sr_left_angle != -1)
	{
		p_cali_data->sr_left_angle_valid = true;
	}
	else
	{
		p_cali_data->sr_left_angle_valid = false;
	}

	if (p_cali_data->sr_right_angle != -1)
	{
		p_cali_data->sr_right_angle_valid = true;
	}
	else
	{
		p_cali_data->sr_right_angle_valid = false;
	}

	if (p_cali_data->br_max_angle != -1)
	{
		p_cali_data->br_max_angle_valid = true;
	}
	else
	{
		p_cali_data->br_max_angle_valid = false;
	}

	if (p_cali_data->ar_max_angle != -1)
	{
		p_cali_data->ar_max_angle_valid = true;
	}
	else
	{
		p_cali_data->ar_max_angle_valid = false;
	}

	if (p_cali_data->ep_max_angle != -1)
	{
		p_cali_data->ep_max_angle_valid = true;
	}
	else
	{
		p_cali_data->ep_max_angle_valid = false;
	}

	if (p_cali_data->sr_acc_dec != -1)
	{
		p_cali_data->sr_acc_dec_valid = true;
	}
	else
	{
		p_cali_data->sr_acc_dec_valid = false;
	}

	if (p_cali_data->br_speed != -1)
	{
		p_cali_data->br_speed_valid = true;
	}
	else
	{
		p_cali_data->br_speed_valid = false;
	}

	if (p_cali_data->br_acc_dec != -1)
	{
		p_cali_data->br_acc_dec_valid = true;
	}
	else
	{
		p_cali_data->br_acc_dec_valid = false;
	}

	if (p_cali_data->ar_speed != -1)
	{
		p_cali_data->ar_speed_valid = true;
	}
	else
	{
		p_cali_data->ar_speed_valid = false;
	}

	if (p_cali_data->ar_acc_dec != -1)
	{
		p_cali_data->ar_acc_dec_valid = true;
	}
	else
	{
		p_cali_data->ar_acc_dec_valid = false;
	}

	if (p_cali_data->ep_encoder_up != -1)
	{
		p_cali_data->ep_encoder_up_valid = true;
	}
	else
	{
		p_cali_data->ep_encoder_up_valid = false;
	}

	if (p_cali_data->ep_encoder_down != -1)
	{
		p_cali_data->ep_encoder_down_valid = true;
	}
	else
	{
		p_cali_data->ep_encoder_down_valid = false;
	}
}

