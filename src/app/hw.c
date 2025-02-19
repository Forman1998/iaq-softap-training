/*
 * hw.c
 *
 *  Created on: 19 Feb 2025
 *      Author: a5126135
 */

#include "hw.h"
#include "r_smc_entry.h"

void Hw_delay_ms(uint16_t ms)
{
	if(ms > 0U)
	{
		TDR04 = (ms > 1U) ? (ms-1U) : 0U;

		TMMK04 = 1U;    /* disable INTTM02 interrupt */
		TMIF04 = 0U;    /* clear INTTM02 interrupt flag */

		TS0 |= _0010_TAU_CH4_START_TRG_ON;

		while(0U == TMIF04)
		{
			NOP();
		}

		TT0 |= _0010_TAU_CH4_STOP_TRG_ON;

		TMIF04 = 0U;    /* clear INTTM02 interrupt flag */
	}
}
/* END OF FUNCTION*/

void Hw_backlight_set(backlight_level_t const backlight_level)
{
	switch(backlight_level)
	{
	case BACKLIGHT_OFF:
	{
		P6_bit.no2 = 1U;
		CCS6 = 0x00U; /* Hi-Z backlight*/
		CCDE &= ~0x40U;
	}
	break;

	case BACKLIGHT_DIM:
	{
		P6_bit.no2 = 0U;
		CCDE |= 0x40U;
		CCS6 = 0x02U; /* 5mA backlight*/
	}
	break;

	case BACKLIGHT_ON:
	{
		P6_bit.no2 = 0U;
		CCDE |= 0x40U;
		CCS6 = 0x03U; /* 10mA backlight*/
	}
	break;

	default:
		/* Do Nothing - shouldn;t get here*/
		break;

	}
}
/* END OF FUNCTION*/
