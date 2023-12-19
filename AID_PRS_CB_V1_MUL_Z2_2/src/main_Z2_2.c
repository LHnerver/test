/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

#include "platform.h"
#include "uart_hmi.h"

/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Global variables
*******************************************************************************/
extern SHARED_DATA __shared_data[];
volatile SHARED_DATA *s_data = __shared_data;

SYSTEM_TIME system_time;
DELAY_TIME delay_time;

SCI_MSG sci1_msg;                	/* 串口数据 LIN1(TTL) */
HMI_CACHE_MSG hmi_cache_msg;
bool hmi_msg_head;
bool hmi_timeout_flag;
uint8_t hmi_timeout_cnt;

uint8_t hmi_offline_cnt;
bool hmi_offline_flag;
/*******************************************************************************
* Function prototypes
*******************************************************************************/
extern void xcptn_xmpl(void);

void param_init(void);

/*******************************************************************************
* Global functions
*******************************************************************************/

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

	(void)memset(&hmi_cache_msg, 0x00, sizeof(HMI_CACHE_MSG));
	hmi_msg_head = false;
	hmi_timeout_flag = false;
	hmi_timeout_cnt = 0;

	hmi_offline_cnt = 0;
	hmi_offline_flag = true;
}

__attribute__ ((section(".text")))
int main(void)
{
	xcptn_xmpl ();       /* Configure and Enable Interrupts */

	param_init();

	//Init_SWT2();

	/* Loop forever */
	for(;;)
	{
		/* 50hz处理 */
		if(system_time.base_20ms_flag)
		{
			system_time.base_20ms_flag = false;
		}

		/* 25hz处理  */
		if(system_time.base_40ms_flag)
		{
			system_time.base_40ms_flag = false;
		}

		/* 20hz处理  */
		if(system_time.base_50ms_flag)
		{
			system_time.base_50ms_flag = false;
		}

		/* 10hz处理  */
		if(system_time.base_100ms_flag)
		{
			system_time.base_100ms_flag = false;

			analyze_hmi_msg(&hmi_cache_msg, s_data);
		}

		/* 5hz处理  */
		if(system_time.base_200ms_flag)
		{
			system_time.base_200ms_flag = false;

			reply_hmi_msg(s_data, &sci1_msg);
		}

		/* 4hz处理   */
		if(system_time.base_250ms_flag)
		{
			//SWT2_Service();
			system_time.base_250ms_flag = false;
		}

		/* 2hz处理   */
		if(system_time.base_500ms_flag)
		{
			system_time.base_500ms_flag = false;

			USR_LED2 = !USR_LED2;
		}

		/* 1hz处理  */
		if(system_time.base_1000ms_flag)
		{
			system_time.base_1000ms_flag = false;
		}
	}
}

void pit2_isr(void)
{
	/* Clear interrupt flag */
	PIT.TIMER[2].TFLG.R = 1;

	if(delay_time.base_100us_flag)
	{
		++delay_time.base_100us_cnt;
	}

	if(delay_time.base_1ms_flag)
	{
		++delay_time.base_1ms_cnt;
		if(hmi_timeout_flag)
		{
			hmi_timeout_cnt++;
		}
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

	if(++system_time.base_50ms_cnt > 500)
	{
		system_time.base_50ms_cnt = 0;
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

		hmi_offline_cnt++;
		if (hmi_offline_cnt > 3)
		{
			hmi_offline_flag = true;
		}
	}
}

void uart1_recv_isr(void)
{
	bool new_data_flag = false;
	LINFlexD_1.UARTSR.B.DRFRFE = 1;				/*Clear DRF interrupt flag */

	sci1_msg.rx_data[sci1_msg.rx_index++] = LINFlexD_1.BDRM.B.DATA4;

	if (!hmi_msg_head && ((sci1_msg.rx_data[sci1_msg.rx_index - 1] == 0x66)
		|| (sci1_msg.rx_data[sci1_msg.rx_index - 1] == 0x70)
		|| (sci1_msg.rx_data[sci1_msg.rx_index - 1] == 0x71)))
	{
		hmi_msg_head = true;
		hmi_timeout_flag = true;
		sci1_msg.rx_data[0] = sci1_msg.rx_data[sci1_msg.rx_index - 1];
		sci1_msg.rx_index = 0x01;
	}

	if (hmi_msg_head)
	{
		if (sci1_msg.rx_data[0] == 0x66 || sci1_msg.rx_data[0] == 0x70)
		{
			if ((sci1_msg.rx_index >= 5)
				&& (sci1_msg.rx_data[sci1_msg.rx_index - 1] == 0xFF)
				&& (sci1_msg.rx_data[sci1_msg.rx_index - 2] == 0xFF)
				&& (sci1_msg.rx_data[sci1_msg.rx_index - 3] == 0xFF))
			{
				new_data_flag = true;
			}
		}
		else
		{
			if ((sci1_msg.rx_index >= 8)
				&& (sci1_msg.rx_data[sci1_msg.rx_index - 1] == 0xFF)
				&& (sci1_msg.rx_data[sci1_msg.rx_index - 2] == 0xFF)
				&& (sci1_msg.rx_data[sci1_msg.rx_index - 3] == 0xFF))
			{
				new_data_flag = true;
			}
		}

		if (hmi_timeout_cnt >= 20)
		{
			new_data_flag = true;
		}

		if (new_data_flag)
		{
			(void)memset(hmi_cache_msg.msg[hmi_cache_msg.w_index].data,
				0x00, sci1_msg.rx_index);
			(void)memcpy(hmi_cache_msg.msg[hmi_cache_msg.w_index].data,
				sci1_msg.rx_data, sci1_msg.rx_index);
			hmi_cache_msg.msg[hmi_cache_msg.w_index].len = sci1_msg.rx_index;
			hmi_cache_msg.w_index++;
			if (hmi_cache_msg.w_index == 5)
			{
				hmi_cache_msg.w_index = 0;
			}
			sci1_msg.rx_index = 0;

			hmi_offline_cnt = 0;
			hmi_offline_flag = false;
		}
	}

	if(sci1_msg.rx_index > 48)
		sci1_msg.rx_index = 0;
}

void uart1_send_isr(void)
{
	LINFlexD_1.UARTSR.B.DTFTFF = 1;				/* Clear DTF interrupt flag */
    uart1_send(&sci1_msg);
}
