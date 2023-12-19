/*
 * sh_mem.c
 *
 *  Created on: 2020Äê7ÔÂ23ÈÕ
 *      Author: Su-xi
 */

#include "sh_mem.h"

/*******************************************************************************
Function Name : Get_Gate_status
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : return status of the selected Gate
Issues        :
*******************************************************************************/
uint8_t get_gate_status(uint8_t gate_no)
{
	return SEMA42.GATE[gate_no].R;
}


/*******************************************************************************
Function Name : Lock_Gate
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : Lock selected Gate
Issues        :
*******************************************************************************/
uint8_t lock_gate(uint8_t gate_no)
{
	SEMA42.GATE[gate_no].R = GET_CORE_ID() + 1;

	return get_gate_status(gate_no);
}


/*******************************************************************************
Function Name : Unlock_Gate
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : Unlock selected Gate
Issues        :
*******************************************************************************/
uint8_t unlock_gate(uint8_t gate_no)
{
	SEMA42.GATE[gate_no].R = UNLOCK;

	return get_gate_status(gate_no);
}


/*******************************************************************************
Function Name : Reset_Gate
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : Reset selected Gate
Issues        :
*******************************************************************************/
uint8_t reset_gate(uint8_t gate_no)
{
	uint8_t cnt = 0;

	SEMA42.RSTGT.W.R = (0xE2 << 8);

	while(SEMA42.RSTGT.R.B.RSTGSM != 0x01)
	{
		if(cnt++ > 10) return 1;
	}

	SEMA42.RSTGT.W.R = (0x1D << 8) | gate_no;

	return 0;
}
