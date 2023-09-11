/*
 * sensor.h
 *
 *  Created on: 26 Oct 2022
 *      Author: a5126135
 */

#ifndef APP_SENSOR_SENSOR_H_
#define APP_SENSOR_SENSOR_H_

#include <stdbool.h>
#include <stdint.h>

/** @brief Initiaises the sensors in the application*/
void Sensor_init(void);

/** @brief performs a blocking read on the sensors in the application.*/
void Sensor_read(void);

/** @brief Gets live humidity readings*/
float Sensor_get_humidity(void);

/** @brief Gets live temperature readings*/
float Sensor_get_temp(void);

#endif /* APP_SENSOR_SENSOR_H_ */
