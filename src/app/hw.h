/*
 * hw.h
 *
 *  Created on: 19 Feb 2025
 *      Author: a5126135
 */

#ifndef APP_HW_H_
#define APP_HW_H_


#include <stdint.h>
#include <stdbool.h>

/** @brief Enumerated type for backlight level*/
typedef enum
{
	BACKLIGHT_OFF			=	0x0000U,
	BACKLIGHT_DIM			=	0x0001U,
	BACKLIGHT_ON			=	0x0002U,
}backlight_level_t;

/** @brief Function to delay a set number of milliseconds.*/
void Hw_delay_ms(uint16_t ms);

/** @brief Set backlight level
 * @param[in] backlight_level - level of backlight.*/
void Hw_backlight_set(backlight_level_t const backlight_level);

#endif /* APP_HW_H_ */
