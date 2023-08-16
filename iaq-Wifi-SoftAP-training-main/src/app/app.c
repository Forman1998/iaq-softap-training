/*
 * app.c
 *
 *  Created on: 19 Apr 2023
 *      Author: a5126135
 */

#include "app.h"

/* Hardware event flags & buffer variable*/
volatile hardware_event_t hw_event_flags = NO_EVENT;
/* rotary counter*/
volatile int16_t rotary_count = 0;
/* Display area data*/
ST7735S_display_area_info_t disp_info = {ORIENT_0, 0U, 0U, 0U, 0U};

/* Array of titles for each state*/
const char * titles[MAXIMUM_SYSTEM_STATES] = {
		"1",
		"2",
		"3"
};

int16_t system_output_state = 1;
int16_t prev_system_output_state = 1;

void App_init(void)
{
	/* Start necessary drivers*/
	R_Config_TAU0_0_Start();
	R_Config_TAU0_1_Start();
	R_Config_CSI00_Start_app();
	R_Config_INTC_INTP4_Start();

	/* Display initialise*/
	St7735s_init(COLOUR_WHITE);
	St7735s_get_display_area_info(&disp_info);
	Text_init(disp_info.xmax, disp_info.ymax);
	Text_set_font(&default_font);
	St7735s_wake_display();
	St7735s_display_on();
	Text_put_line(TEXT_X_POSITION, TEXT_Y_POSITION, "1", COLOUR_BLACK, COLOUR_WHITE);
	St7735s_refresh();
	Hw_enable_backlight();
}
/* END OF FUNCTION*/

void App_update_display(void)
{
	/* Erase previous text*/
	Text_put_line(TEXT_X_POSITION, TEXT_Y_POSITION, titles[prev_system_output_state-1], COLOUR_WHITE, COLOUR_WHITE);
	/* Write new text*/
	Text_put_line(TEXT_X_POSITION, TEXT_Y_POSITION, titles[system_output_state-1], COLOUR_BLACK, COLOUR_WHITE);
	St7735s_refresh();
}
/* END OF FUNCTION*/

bool App_button_click_event(void)
{
	bool event_occured = false;
	PREPARE_CRITICAL_SECTION();

	ENTER_CRITICAL_SECTION();
	event_occured = HW_EVENT_OCCURRED(hw_event_flags, BUTTON_CLICK);
	HW_CLEAR_EVENT(hw_event_flags, BUTTON_CLICK);
	EXIT_CRITICAL_SECTION();

	/* Check for a button click*/
	if(event_occured)
	{
		bool debounce_event_occured = true;

		/* Debounce*/
		while(debounce_event_occured)
		{
			Hw_delay_ms(1U);

			ENTER_CRITICAL_SECTION();

			debounce_event_occured = HW_EVENT_OCCURRED(hw_event_flags, BUTTON_CLICK);
			HW_CLEAR_EVENT(hw_event_flags, BUTTON_CLICK);

			EXIT_CRITICAL_SECTION();
		}
	}

	return event_occured;
}
/* END OF FUNCTION*/

bool App_rotary_event(void)
{
	bool event_occured = false;
	PREPARE_CRITICAL_SECTION();

	ENTER_CRITICAL_SECTION();

	event_occured = HW_EVENT_OCCURRED(hw_event_flags, ROTARY_COUNT_UPDATED);
	HW_CLEAR_EVENT(hw_event_flags, ROTARY_COUNT_UPDATED);

	EXIT_CRITICAL_SECTION();

	return event_occured;
}
/* END OF FUNCTION*/
