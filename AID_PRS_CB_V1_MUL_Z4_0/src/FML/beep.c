/*
 * beep.c
 *
 *  Created on: 2020年12月8日
 *      Author: Su-xi
 */

#include "beep.h"

uint8_t beep_repeat_cnt;	// 单次蜂鸣重复次数
uint8_t beep_cnt0;
uint16_t beep_cycle1_cnt;	// 单次蜂鸣内部周期控制计数
uint16_t beep_cnt1;
uint32_t beep_cycle2_cnt;	// 循环蜂鸣周期控制计数
uint32_t beep_cnt2;

BEEP_STATE beep_state;

void beep_ctrl(BEEP_STATE state)
{
	if (state == BEEP_ON)
	{
		BEEP = 1;
	}
	else
	{
		BEEP = 0;
	}
}

// 获取蜂鸣器状态
BEEP_STATE get_beep_state(void)
{
	return (BEEP_STATE)BEEP;
}

// 初始化
void beep_init(void)
{
	beep_repeat_cnt = 0;
	beep_cycle1_cnt = 0;
	beep_cycle2_cnt = 0;
	beep_state = BEEP_OFF;
}

// 设置蜂鸣控制参数
void set_beep(uint8_t cnt, uint16_t cycle1_ms, uint32_t cycle2_ms)
{
	beep_repeat_cnt = cnt * 2;
	beep_cycle1_cnt = cycle1_ms / 2;
	beep_cycle2_cnt = cycle2_ms;
}

// 蜂鸣开闭控制
BEEP_STATE beep_switch(BEEP_STATE state)
{
	if (beep_repeat_cnt == 0 || beep_cycle1_cnt == 0)
	{
		beep_state = BEEP_OFF;
		beep_ctrl(BEEP_OFF);
	}
	else
	{
		beep_state = state;
		beep_ctrl(state);
	}
	beep_cnt0 = 0;
	beep_cnt1 = 0;
	beep_cnt2 = 0;

	return beep_state;
}

BEEP_STATE get_beep_ctrl_state(void)
{
	return beep_state;
}

// beep_repeat_cnt  6
// beep_cycle1_cnt 500
// beep_cycle2_cnt 60000

void beep_service(void)
{
	if (beep_state == BEEP_ON)
	{
		beep_cnt1++;
		beep_cnt2++;

		// 单次蜂鸣内部周期控制
		if (beep_cnt1 >= beep_cycle1_cnt)
		{
			beep_cnt1 = 0;
			beep_cnt0++;
			if (get_beep_state() == BEEP_ON)
			{
				beep_ctrl(BEEP_OFF);
			}
			else
			{
				beep_ctrl(BEEP_ON);
			}
		}

		// 单次蜂鸣结束
		if (beep_cnt0 >= beep_repeat_cnt)
		{
			beep_cnt1 = 0;
			if (get_beep_state() == BEEP_ON)
			{
				beep_ctrl(BEEP_OFF);
			}

			if (beep_cycle2_cnt == 0)
			{
				beep_state = BEEP_OFF;
			}
		}

		// 循环蜂鸣控制
		if (beep_cycle2_cnt != 0)
		{
			if (beep_cnt2 >= beep_cycle2_cnt)
			{
				beep_ctrl(BEEP_ON);
				beep_cnt0 = 0;
				beep_cnt1 = 0;
				beep_cnt2 = 0;
			}
		}
		else
		{
			beep_cnt2 = 0;
		}
	}
	else
	{
		beep_ctrl(BEEP_OFF);
		beep_cnt0 = 0;
		beep_cnt1 = 0;
		beep_cnt2 = 0;
	}
}

void pit3_isr(void)
{
	/* Clear interrupt flag */
	PIT.TIMER[3].TFLG.R = 1;
	beep_service();
}
