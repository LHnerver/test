/*
 * pit.c
 *
 *  Created on: 2019Äê1ÔÂ2ÈÕ
 *      Author: Su-xi
 */
#include "pit.h"

static void pit0_init(uint32_t LDVAL);
static void pit1_init(uint32_t LDVAL);
static void pit2_init(uint32_t LDVAL);
static void pit3_init(uint32_t LDVAL);

static void pit0_init(uint32_t LDVAL)
{
    PIT.TIMER[0].LDVAL.R = LDVAL;  		/* Load # Pit clocks to count */
    PIT.TIMER[0].TCTRL.B.TIE = 1;      	/* Enable interrupt */
    PIT.TIMER[0].TCTRL.B.TEN = 1;      	/* enable channel */
}

static void pit1_init(uint32_t LDVAL)
{
    PIT.TIMER[1].LDVAL.R = LDVAL;  		/* load PIT counter */
    PIT.TIMER[1].TCTRL.B.TIE = 1;      	/* enable interrupt */
    PIT.TIMER[1].TCTRL.B.TEN = 1;      	/* enable channel */
}

static void pit2_init(uint32_t LDVAL)
{
    PIT.TIMER[2].LDVAL.R = LDVAL;  		/* load PIT counter */
    PIT.TIMER[2].TCTRL.B.TIE = 1;      	/* enable interrupt */
    PIT.TIMER[2].TCTRL.B.TEN = 1;      	/* enable channel */
}

static void pit3_init(uint32_t LDVAL)
{
    PIT.TIMER[3].LDVAL.R = LDVAL;  		/* load PIT counter */
    PIT.TIMER[3].TCTRL.B.TIE = 1;      	/* enable interrupt */
    PIT.TIMER[3].TCTRL.B.TEN = 1;      	/* enable channel */
}

void init_pit()
{
    PIT.MCR.B.MDIS = 0;	/* Enable PIT module. NOTE: PIT module must be       */
                      	/* enabled BEFORE writing to it's registers.         */
                     	/* Other cores will write to PIT registers so the    */
                      	/* PIT is enabled here before starting other cores.  */
    PIT.MCR.B.FRZ = 1; 	/* Freeze PIT timers in debug mode */

    pit0_init(4000);	/* 100us */
    pit1_init(4000);	/* 100us */
    pit2_init(4000);	/* 100us */
    pit3_init(40000);   /* 1ms */
}

