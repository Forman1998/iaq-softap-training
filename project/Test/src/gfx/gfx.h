/*
 * gfx.h
 *
 *  Created on: 26 Oct 2022
 *      Author: a5126135
 */

#ifndef APP_GFX_GFX_H_
#define APP_GFX_GFX_H_

#include <stdint.h>
#include "sensor.h"

/** struct type for defining the sensor data*/
typedef struct
{
	int16_t temperature_int; /**< integer portion of the temperature*/
	int16_t humidity_int; /**< integer portion of the humidity*/
}sensor_data_t;

/** @brief Initialises all graphical portions of the application*/
void Gfx_init(void);

/** @brief writes the title (renesas logo) to the display.*/
void Gfx_set_background_title(void);

/** @brief Sets up the display for temperature and humidity readings*/
void Gfx_set_background_temp_humid(void);

/** @brief Writes the temperature and humidity readings to the display.*/
void Gfx_write_temp_humid(const volatile sensor_data_t * sense_data);

/** @brief turns display on*/
void Gfx_display_on(void);

/** @brief set normal backlight*/
void Gfx_normal_backlight(void);

/** @brief refreshes display with RAM buffer contents*/
void Gfx_display_refresh(void);

#endif /* APP_GFX_GFX_H_ */
