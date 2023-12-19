#include "power_monitor.h"

#if 0	// 使用LTC2945采样程序
// LSB Weights
const float LTC2945_ADIN_lsb = 5.001221E-04;
const float LTC2945_DELTA_SENSE_lsb = 2.5006105E-05;
const float LTC2945_VIN_lsb = 2.5006105E-02;
const float LTC2945_Power_lsb = 6.25305E-07;
const float LTC2945_ADIN_DELTA_SENSE_lsb = 1.25061E-08;

float Convert_To_Power(uint32_t  code,float resistor)
{
	float voltage, power;
	voltage = (float)code * LTC2945_Power_lsb;  //! 1) Calculate V^2 from ADC code and power lsb
	power = voltage/resistor;                     //! 2) Calculate Power, P = V^2/R
	return(power);
}

float Convert_To_VIN(uint16_t  code)
{
	float voltage;
	voltage = (float)code * LTC2945_VIN_lsb;
	return(voltage);
}

float Convert_To_Sense(uint16_t  code,float resistor)
{
	float voltage,current;
	voltage = (float)code * LTC2945_DELTA_SENSE_lsb;
	current = voltage / resistor;
	return(current);
}

void read_power_data(POWER_INFO* pmp)
{
	unsigned int  code2;
	unsigned char POWER[3];
	unsigned char Vol_VIN[2];
	unsigned char Cur_SENSE[2];

	// DCIN
	IIC_Receive(LTC2945_ADDR_IN, LTC2945_POWER_MSB2_REG,
		(unsigned char *)&POWER,3);

	code2 = (POWER[0] << 16) | (POWER[1] << 8) | (POWER[2] & 0xFF);
	code2 &= 0xFFFFFF;

	pmp->power_in = Convert_To_Power(code2,0.002);

	IIC_Receive(LTC2945_ADDR_IN, LTC2945_VIN_MSB_REG,
		(unsigned char *)&Vol_VIN,2);

	code2 = (Vol_VIN[0] << 8) | Vol_VIN[1];
	code2 >>= 4;

	pmp->voltage_in = Convert_To_VIN(code2);

	IIC_Receive(LTC2945_ADDR_IN, LTC2945_DELTA_SENSE_MSB_REG,
		(unsigned char *)&Cur_SENSE,2);

	code2 = (Cur_SENSE[0] << 8) | Cur_SENSE[1];
	code2 >>= 4;

	pmp->current_in = Convert_To_Sense(code2,0.002);

	// PWROUT1
	IIC_Receive(LTC2945_ADDR_OUT1, LTC2945_POWER_MSB2_REG,
		(unsigned char *)&POWER,3);
	code2 = (POWER[0] << 16) | (POWER[1] << 8) | (POWER[2] & 0xFF);
	code2 &= 0xFFFFFF;

	pmp->power_out1 = Convert_To_Power(code2,0.004);

	IIC_Receive(LTC2945_ADDR_OUT1, LTC2945_VIN_MSB_REG,
		(unsigned char *)&Vol_VIN,2);

	code2 = (Vol_VIN[0] << 8) | Vol_VIN[1];
	code2 >>= 4;

	pmp->voltage_out1 = Convert_To_VIN(code2);

	IIC_Receive(LTC2945_ADDR_OUT1, LTC2945_DELTA_SENSE_MSB_REG,
		(unsigned char *)&Cur_SENSE,2);

	code2 = (Cur_SENSE[0] << 8) | Cur_SENSE[1];
	code2 >>= 4;

	pmp->current_out1 = Convert_To_Sense(code2,0.004);

	// PWROUT2
	IIC_Receive(LTC2945_ADDR_OUT2, LTC2945_POWER_MSB2_REG,
		(unsigned char *)&POWER,3);

	code2 = (POWER[0] << 16) | (POWER[1] << 8) | (POWER[2] & 0xFF);
	code2 &= 0xFFFFFF;

	pmp->power_out2 = Convert_To_Power(code2,0.004);

	IIC_Receive(LTC2945_ADDR_OUT2, LTC2945_VIN_MSB_REG,
		(unsigned char *)&Vol_VIN,2);

	code2 = (Vol_VIN[0] << 8) | Vol_VIN[1];
	code2 >>= 4;

	pmp->voltage_out2 = Convert_To_VIN(code2);

	IIC_Receive(LTC2945_ADDR_OUT2, LTC2945_DELTA_SENSE_MSB_REG,
		(unsigned char *)&Cur_SENSE,2);

	code2 = (Cur_SENSE[0] << 8) | Cur_SENSE[1];
	code2 >>= 4;

	pmp->current_out2 = Convert_To_Sense(code2,0.004);

	// PWROUT3
	IIC_Receive(LTC2945_ADDR_OUT3, LTC2945_POWER_MSB2_REG,
		(unsigned char *)&POWER,3);

	code2 = (POWER[0] << 16) | (POWER[1] << 8) | (POWER[2] & 0xFF);
	code2 &= 0xFFFFFF;

	pmp->power_out3 = Convert_To_Power(code2,0.004);

	IIC_Receive(LTC2945_ADDR_OUT3, LTC2945_VIN_MSB_REG,
		(unsigned char *)&Vol_VIN,2);

	code2 = (Vol_VIN[0] << 8) | Vol_VIN[1];
	code2 >>= 4;

	pmp->voltage_out3 = Convert_To_VIN(code2);

	IIC_Receive(LTC2945_ADDR_OUT3, LTC2945_DELTA_SENSE_MSB_REG,
		(unsigned char *)&Cur_SENSE,2);

	code2 = (Cur_SENSE[0] << 8) | Cur_SENSE[1];
	code2 >>= 4;

	pmp->current_out3 = Convert_To_Sense(code2,0.004);

	// PWROUT4
	IIC_Receive(LTC2945_ADDR_OUT4, LTC2945_POWER_MSB2_REG,
		(unsigned char *)&POWER,3);

	code2 = (POWER[0] << 16) | (POWER[1] << 8) | (POWER[2] & 0xFF);
	code2 &= 0xFFFFFF;

	pmp->power_out4 = Convert_To_Power(code2,0.004);

	IIC_Receive(LTC2945_ADDR_OUT4, LTC2945_VIN_MSB_REG,
		(unsigned char *)&Vol_VIN,2);

	code2 = (Vol_VIN[0] << 8) | Vol_VIN[1];
	code2 >>= 4;

	pmp->voltage_out4 = Convert_To_VIN(code2);

	IIC_Receive(LTC2945_ADDR_OUT4, LTC2945_DELTA_SENSE_MSB_REG,
		(unsigned char *)&Cur_SENSE,2);

	code2 = (Cur_SENSE[0] << 8) | Cur_SENSE[1];
	code2 >>= 4;

	pmp->current_out4 = Convert_To_Sense(code2,0.004);
}

void power_monitor_init(void)
{
	Init_IIC();
}
#endif

#if 1	// 使用AD采样程序

/* LM2904
 * AD采集电压转换系数
 * V / (21*3.3) = AD_CHn / 4095
 * V = AD_CHn * (21*3.3)/4095
 * 21 - 电路分压系数；  3.3 - 满程电压；  4095 - 满程AD值
 * 令V_RATIO = (21*3.3)/4095 = 0.016923
 */
const float  V_RATIO = 0.016923;

/* 采样芯片：CC6902SO-30A
 * AD采集电流转换系数
 * Vout / 3.3 = AD_CHn / 4095
 * 1.5*Vout = (4.95/2 + 0.067 * I)	(30A)
 *  I(30A)  = AD_CHn * 4.95/(4095*0.067) - 4.95/(2*0.067)
 *          //式中AD_CHn * 4.95 的4.95取值5 （3.3*1.5）
 *  得到 I = AD_CHn * 0.018042 - 36.940299
 *  可能因为输入输出引脚反了？
 *  //实际 I = 36.940299 - AD_CHn * 0.018237
 */
const float  A30_RATIO = 0.018042;
const float  A30_CONST = 36.940299;

void read_power_data(POWER_INFO* pmp)
{
	float ampere = 0, voltage = 0;
	voltage = V_RATIO * ADC1_Read_CH1();
	ampere = A30_RATIO * ADC1_Read_CH2() - A30_CONST;

	// 实际测量电流有一定偏差
	// I = 1.051282 * ampere + 0.751282；
	pmp->voltage_in = voltage;
	pmp->current_in = 1.05 * ampere + 0.75;
	pmp->power_in = voltage * ampere;

//	pmp->voltage_in = V_RATIO * ADC1_Read_CH1();
//	pmp->current_in = A30_RATIO * ADC1_Read_CH2() - A30_CONST;
//	pmp->power_in = pmp->voltage_in * pmp->current_in;
}

#endif
