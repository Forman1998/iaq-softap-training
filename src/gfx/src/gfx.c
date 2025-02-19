/*
 * gfx.c
 *
 *  Created on: 26 Oct 2022
 *      Author: a5126135
 */

#include "gfx.h"
#include "st7735s_port.h"
#include "st7735s.h"
#include "r_smc_entry.h"
#include "draw.h"
#include "text.h"
#include "string.h"
#include "images.h"

/********** TEXT ********************/
#define BACKGROUND_TEXT_COLOUR	COLOUR_WHITE
#define FOREGROUND_TEXT_COLOUR	COLOUR_GREY
#define PROBLEM_TEXT_COLOUR	COLOUR_RED

/*********** TITLE ********************/
#define TITLE_IMAGE_OFFSET_X  (22U)
#define TITLE_IMAGE_OFFSET_Y  (3U)
#define TITLE_IMAGE_WIDTH  (116U)
#define TITLE_IMAGE_HEIGHT  (20U)

/*********** BACKGROUND GENERIC *************/
#define BACKGROUND_X_START	(10U)
#define BACKGROUND_Y_START	(23U)
#define BACKGROUND_WIDTH	(149U)
#define BACKGROUND_HEIGHT	(57U)

/*********** TEMPERATURE ********************/
#define TEMPERATURE_IMAGE_OFFSET_X  (12U)
#define TEMPERATURE_IMAGE_OFFSET_Y  (30U)
#define TEMPERATURE_IMAGE_WIDTH  (40U)
#define TEMPERATURE_IMAGE_HEIGHT  (40U)

#define TEMPERATURE_TEXT_OFFSET_X (42U)
#define TEMPERATURE_TEXT_OFFSET_Y (28U)

/*********** HUMIDITY ********************/
#define HUMIDITY_IMAGE_OFFSET_X  (77U)
#define HUMIDITY_IMAGE_OFFSET_Y  (30U)
#define HUMIDITY_IMAGE_WIDTH  (40U)
#define HUMIDITY_IMAGE_HEIGHT  (40U)

#define HUMIDITY_TEXT_OFFSET_X (117U)
#define HUMIDITY_TEXT_OFFSET_Y (28U)

/** @brief abs function for gfx stuff*/
#define GFX_ABS(a, b) ((a > b) ? a-b : b-a)

/** Display area data*/
static ST7735S_display_area_info_t disp_info;

/** @brief Utility function to erase text.
 * @param[in] x - x position of text to erase.
 * @param[in] y - y position of text to erase.
 * @param[in] length - length of text (in pixels) to erase - can be found using utility function Text_str_len_px */
static void Erase_text(uint16_t x, uint16_t y, uint16_t length);
/** @brief Utility functino used to clear the display background area (everything but the title/renesas logo)*/
static void Erase_background(void);
/** @brief Utility functino used to clear the entire display*/
static void Erase_display(void);

void Gfx_init(void)
{
	St7735s_init(COLOUR_WHITE);

	St7735s_get_display_area_info(&disp_info);

	Text_init(disp_info.xmax, disp_info.ymax);
	Draw_init(disp_info.xmax, disp_info.ymax);

	Text_set_font(&default_font);
}
/* END OF FUNCTION*/

void Gfx_set_background_title(void)
{
	St7735s_send_image(TITLE_IMAGE_OFFSET_X, TITLE_IMAGE_OFFSET_Y, TITLE_IMAGE_WIDTH, TITLE_IMAGE_HEIGHT, (__far const uint8_t *)renesas_title);
}
/* END OF FUNCTION*/

void Gfx_set_background_temp_humid(void)
{
	Erase_background();
	St7735s_send_image(TEMPERATURE_IMAGE_OFFSET_X, TEMPERATURE_IMAGE_OFFSET_Y, TEMPERATURE_IMAGE_WIDTH, TEMPERATURE_IMAGE_HEIGHT, (__far const uint8_t *)thermometer);
	St7735s_send_image(HUMIDITY_IMAGE_OFFSET_X, HUMIDITY_IMAGE_OFFSET_Y, HUMIDITY_IMAGE_WIDTH, HUMIDITY_IMAGE_HEIGHT, (__far const uint8_t *)droplet);
}
/* END OF FUNCTION*/

void Gfx_write_temp_humid(const volatile sensor_data_t * sense_data)
{
	static uint16_t str_nav = 0U; /* Variable used to navigate strings*/
	static uint16_t temp_str_len_prev = 0U; /* Variable used to store the previously written temperature string length*/
	static uint16_t hum_str_len_prev = 0U; /* Variable used to store the previously written humidity string length*/
	static uint16_t str_len_tmp = 0U; /* variable used to read in the current string length of a string of interest*/
	static char temp_str[16]; /* variable used to store the temperature string*/
	static char hum_str[16]; /* variable used to store the humidity string*/
	static const char temp_unit_str[] = "\x7f""C";
	static const char humidity_unit_str[] = "%";

	str_nav = Text_int_to_str(sense_data->temperature_int, temp_str);
	(void)strcpy(&temp_str[str_nav], temp_unit_str);
	str_nav = Text_int_to_str(sense_data->humidity_int, hum_str);
	(void)strcpy(&hum_str[str_nav], humidity_unit_str);

	/* Write the temperature*/
	Text_set_font(&default_font);
	str_len_tmp = Text_put_line(TEMPERATURE_TEXT_OFFSET_X, TEMPERATURE_TEXT_OFFSET_Y, temp_str, FOREGROUND_TEXT_COLOUR, BACKGROUND_TEXT_COLOUR);

	/* Erase any extra pixels resulting in  shorter string*/
	if(temp_str_len_prev > str_len_tmp)
	{
		Erase_text(TEMPERATURE_TEXT_OFFSET_X + str_len_tmp, TEMPERATURE_TEXT_OFFSET_Y, temp_str_len_prev - str_len_tmp);
	}

	temp_str_len_prev = str_len_tmp;

	/* Write the humidity*/
	str_len_tmp = Text_put_line(HUMIDITY_TEXT_OFFSET_X, HUMIDITY_TEXT_OFFSET_Y, hum_str, FOREGROUND_TEXT_COLOUR, BACKGROUND_TEXT_COLOUR);

	/* Erase any extra pixels resulting in  shorter string*/
	if(hum_str_len_prev > str_len_tmp)
	{
		Erase_text(HUMIDITY_TEXT_OFFSET_X + str_len_tmp, HUMIDITY_TEXT_OFFSET_Y, hum_str_len_prev - str_len_tmp);
	}

	hum_str_len_prev = str_len_tmp;
}
/* END OF FUNCTION*/

void Gfx_display_off(void)
{
	St7735s_sleep_display();
	St7735s_display_off();
}
/* END OF FUNCTION*/

void Gfx_display_on(void)
{
	St7735s_wake_display();
	St7735s_display_on();
}
/* END OF FUNCTION*/

void Gfx_normal_backlight(void)
{
	Hw_backlight_set(BACKLIGHT_ON);
}
/* END OF FUNCTION*/


void Gfx_display_refresh(void)
{
	St7735s_refresh();
}
/* END OF FUNCTION*/

static void Erase_text(uint16_t x, uint16_t y, uint16_t length)
{
	static solid_rectangle_t eraser = {
			.height = 16U,
			.colour = BACKGROUND_TEXT_COLOUR
	};

	eraser.width = length;
	eraser.y0 = y;
	eraser.x0 = x;
	Draw_solid_rectangle(&eraser);
}
/* END OF FUNCTION*/

static void Erase_background(void)
{
	static const solid_rectangle_t eraser = {
			.height = BACKGROUND_HEIGHT,
			.width = BACKGROUND_WIDTH,
			.x0 = BACKGROUND_X_START,
			.y0 = BACKGROUND_Y_START,
			.colour = BACKGROUND_TEXT_COLOUR,
	};

	Draw_solid_rectangle(&eraser);
}
/* END OF FUNCTION*/

static void Erase_display(void)
{
	St7735s_set_colour(BACKGROUND_TEXT_COLOUR);
	St7735s_fill_display();
	St7735s_set_colour(FOREGROUND_TEXT_COLOUR);
}
/* END OF FUNCTION*/
