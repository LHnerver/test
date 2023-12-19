/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

#include "math.h"

#include "timer.h"
#include "sh_mem.h"
#include "ctrl_can.h"
#include "platform.h"
#include "motor_can.h"
#include "power_monitor.h"
#include "uart_hmi.h"
#include "uart_sensor.h"
#include "flash.h"
#include "bootloader.h"
#include "encoder.h"
#include "linflexd_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "derivative.h"
#include "typedefs.h"
#include "pin.h"
#include "hardware_iic.h"

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define VERSION 	1001
#define FORCE_PRINTF    1
/*******************************************************************************
* Global variables
*******************************************************************************/
SHARED_DATA s_data __attribute__((section (".sharedRAM")));

SYSTEM_TIME system_time;

DELAY_TIME delay_time;

POWER_INFO power_msg;

PEDAL_FORCE sensor_f;

/*******************************************************************************
* Function prototypes
*******************************************************************************/
void param_init(void);
void multicore_share_data(void);

extern void xcptn_xmpl(void);

void param_init(void)
{
	delay_time.base_100us_cnt = 0;
	delay_time.base_100us_flag = false;
	delay_time.base_1ms_cnt = 0;
	delay_time.base_1ms_flag = false;

	system_time.base_1ms_cnt = 0;
	system_time.base_1ms_flag = false;
	system_time.base_5ms_cnt = 0;
	system_time.base_5ms_flag = false;
	system_time.base_10ms_cnt = 0;
	system_time.base_10ms_flag = false;
	system_time.base_20ms_cnt = 0;
	system_time.base_20ms_flag = false;
	system_time.base_40ms_cnt = 0;
	system_time.base_40ms_flag = false;
	system_time.base_50ms_flag = 0;
	system_time.base_50ms_flag = false;
	system_time.base_100ms_cnt = 0;
	system_time.base_100ms_flag = false;
	system_time.base_125ms_cnt = 0;
	system_time.base_125ms_flag = false;
	system_time.base_200ms_cnt = 0;
	system_time.base_200ms_flag = false;
	system_time.base_250ms_cnt = 0;
	system_time.base_250ms_flag = false;
	system_time.base_500ms_cnt = 0;
	system_time.base_500ms_flag = false;
	system_time.base_1000ms_cnt = 0;
	system_time.base_1000ms_flag = false;

	(void)memset(&s_data, 0x00, sizeof(SHARED_DATA));

//	(void)sprintf(s_data.device_info.name, "%s", NAME);
//	(void)sprintf(s_data.device_info.model, "%s", MODEL);
//	(void)sprintf(s_data.device_info.version, "%s", VERSION);
//	(void)sprintf(s_data.device_info.serial_num, "%s", SN);
}

void multicore_share_data(void)
{
	uint8_t status = UNLOCK;

	/* Critical section */
	/* when the Gate is not lock */
	while(status != CORE0_LOCK)
	{
		/* Lock the gate */
		status = lock_gate(GATE_0);
	}

	/* Execute critical section */
	s_data.pwr_msg = power_msg;

	s_data.sys_info.power = power_msg.power_in;
	s_data.sys_info.voltage = power_msg.voltage_in;
	s_data.sys_info.current = power_msg.current_in;

	s_data.force = sensor_f;

	while(status == CORE0_LOCK)
	{
		/* Unlock the gate */
		status = unlock_gate(GATE_0);
	}
	/*End of the critical section*/
}



__attribute__ ((section(".text")))
int main(void)
{
	/* Configure and Enable Interrupts */
	xcptn_xmpl ();

	platform_init();
	timer_init();
	ctrl_can_init();
	motor_can_init();
	//uart_hmi_init();
	//uart_sensor_init();   //串口10
	boot_init();
	Init_ADC();		// 初始化AD通道CH0,CH1,CH2, 用于采样角编、电压、电流
	/* Initialize LINFlex10: UART Mode 80MHz, 115200 Baud */

	i2c_3_init(I2C_SPEED_400K);  //新增IIC3,获取压力传感器的电流值。
	Init_LINFlexD8(80, 115200);  // 新增串口8 做log打印输出
	/* initialize parameters */
	param_init();

	/* boot CPU1 and CPU2 */
	boot_cores();

	//Init_SWT0();

	/* Loop forever */
	for(;;)
	{
		//100hz处理

		if(system_time.base_10ms_flag)
		{
			 get_pressure(&sensor_f.BR);
			//get_iic_pressure(&sensor_f.BR);
			//uart8_printf("sensor_f.BR :%d\n",sensor_f.BR);
			 system_time.base_10ms_flag = false;
		}

		//50hz处理
		if(system_time.base_20ms_flag)
		{
		     multicore_share_data();
			system_time.base_20ms_flag = false;

		}
		//25hz处理
		if(system_time.base_40ms_flag)
		{
			system_time.base_40ms_flag = false;
		}
		//20hz处理
		if(system_time.base_50ms_flag)
		{
			//读取电压
			read_power_data(&power_msg);
			system_time.base_50ms_flag = false;
		}

		//10hz处理
		if(system_time.base_100ms_flag)
		{
			boot_execute();
			system_time.base_100ms_flag = false;
		}

		//5hz处理
		if(system_time.base_200ms_flag)
		{
			system_time.base_200ms_flag = false;
		}

		//4hz处理
		if(system_time.base_250ms_flag)
		{
			//SWT0_Service();
			system_time.base_250ms_flag = false;
		}

		//2hz处理
		if(system_time.base_500ms_flag)
		{

			system_time.base_500ms_flag = false;
		}

		//1hz处理
		if(system_time.base_1000ms_flag)
		{
			system_time.base_1000ms_flag = false;

			USR_LED1 = !USR_LED1;
		}
	}
}

void pit0_isr(void)
{
	/* Clear interrupt flag */
	PIT.TIMER[0].TFLG.R = 1;
	if(delay_time.base_100us_flag)
	{
		++delay_time.base_100us_cnt;
	}

	if(delay_time.base_1ms_flag)
	{
		++delay_time.base_1ms_cnt;
	}

	if(++system_time.base_10ms_cnt > 100)
	{
		system_time.base_10ms_cnt = 0;
		system_time.base_10ms_flag = true;
	}

	if(++system_time.base_20ms_cnt > 200)
	{
		system_time.base_20ms_cnt = 0;
		system_time.base_20ms_flag = true;
	}

	if(++system_time.base_40ms_cnt > 400)
	{
		system_time.base_40ms_cnt = 0;
		system_time.base_40ms_flag = true;
	}

	if(++system_time.base_50ms_flag > 500)
	{
		system_time.base_50ms_flag = 0;
		system_time.base_50ms_flag = true;
	}

	if(++system_time.base_100ms_cnt > 1000)
	{
		system_time.base_100ms_cnt = 0;
		system_time.base_100ms_flag = true;
	}

	if(++system_time.base_200ms_cnt > 2000)
	{
		system_time.base_200ms_cnt = 0;
		system_time.base_200ms_flag = true;
	}

	if(++system_time.base_250ms_cnt > 2500)
	{
		system_time.base_250ms_cnt = 0;
		system_time.base_250ms_flag = true;
	}

	if(++system_time.base_500ms_cnt > 5000)
	{
		system_time.base_500ms_cnt = 0;
		system_time.base_500ms_flag = true;
	}

	if(++system_time.base_1000ms_cnt > 10000)
	{
		system_time.base_1000ms_cnt = 0;
		system_time.base_1000ms_flag = true;
	}
}

