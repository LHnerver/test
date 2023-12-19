/*
 * encoder.c
 *
 *  Created on: 2020Äê11ÔÂ11ÈÕ
 *      Author: Su-xi
 */

#include "encoder.h"

void encoder_init(void)
{
	Init_ADC();
}

uint16_t get_encoder_value(void)
{
	uint16_t value;
	/* If normal scan channels finished converting */
	if (ADC_1.ISR.B.ECH)
	{
		/* Read conversion Results */
		value = ADC1_Read_CH0();
		/* Clear End of CHain (ECH) status bit */
		ADC_1.ISR.R = 0x00000001;
	}

	return value;
}
