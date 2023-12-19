/*
 * motor_can.c
 *
 *  Created on: 2020Äê7ÔÂ23ÈÕ
 *      Author: Su-xi
 */

#include "motor_can.h"

void motor_can_init()
{
	/* Configure and Enable  CAN2 */
	Init_CAN2();
	/* Configure and Enable  CAN3 */
	Init_CAN3();
	/* Configure and Enable  CAN5 */
	Init_CAN5();
	/* Configure and Enable  CAN7 */
	Init_CAN7();
}

