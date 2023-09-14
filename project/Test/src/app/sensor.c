/*
 * sensor.c
 *
 *  Created on: 26 Oct 2022
 *      Author: a5126135
 */

#include "sensor.h"
#include "r_cg_macrodriver.h"
#include "r_hs400x_if.h"
#include "http_server.h"

/** callback status enumerated type*/
typedef enum
{
	SENSOR_CALLBACK_STATUS_WAIT = (0),
	SENSOR_CALLBACK_STATUS_SUCCESS,
	SENSOR_CALLBACK_STATUS_REPEAT,
} sensor_callback_status_t;

static volatile sensor_callback_status_t hs400x_i2c_callback_status = SENSOR_CALLBACK_STATUS_WAIT;
static rm_hs400x_data_t hs400x_data;
bool wait = true;
static volatile sensor_read_state_t sensor_read_state = SENSOR_WAITING;

/** @brief function to handle/log errors in sensor code*/
static void Demo_err(void);

/**********************************************************************
 * Sensor API
 *********************************************************************/
void Sensor_init(void)
{
	fsp_err_t err = g_hs400x_sensor0.p_api->open(g_hs400x_sensor0.p_ctrl, g_hs400x_sensor0.p_cfg);

	if (FSP_SUCCESS != err)
	{
		Demo_err();
	}
}
/* END OF FUNCTION*/

void Sensor_read(void)
{
	static rm_hs400x_raw_data_t hs400x_raw_data;

	fsp_err_t err = FSP_SUCCESS;
	if(wait)
	{

	/**********************
		 * HS4001
		 *********************/
		/* Start measurement */
		err = g_hs400x_sensor0.p_api->measurementStart(g_hs400x_sensor0.p_ctrl);
		if (FSP_SUCCESS != err)
		{
			Demo_err();
		}

		/* Wait for IIC to finish*/
		while (SENSOR_CALLBACK_STATUS_WAIT == hs400x_i2c_callback_status)
		{
			HALT();
		}
		R_Config_TAU0_5_Start();
		wait = false;
	}
	else
	{
		hs400x_i2c_callback_status = SENSOR_CALLBACK_STATUS_WAIT;

		/* Read data */
		err = g_hs400x_sensor0.p_api->read(g_hs400x_sensor0.p_ctrl, &hs400x_raw_data);
		if (FSP_SUCCESS != err)
		{
			Demo_err();
		}

		/* Wait for IIC to finish*/
		while (SENSOR_CALLBACK_STATUS_WAIT == hs400x_i2c_callback_status)
		{
			HALT();
		}

		/* Calculate data */
		(void)g_hs400x_sensor0.p_api->dataCalculate(g_hs400x_sensor0.p_ctrl, &hs400x_raw_data, &hs400x_data);

		hs400x_i2c_callback_status = SENSOR_CALLBACK_STATUS_WAIT;
		wait = true;
	}
}
/* END OF FUNCTION*/

uint16_t Sensor_get_temp(void)
{
	return hs400x_data.temperature.integer_part;
}

uint16_t Sensor_get_humidity(void)
{
	return hs400x_data.humidity.integer_part;
}

void hs400x_user_i2c_callback0(rm_hs400x_callback_args_t *p_args)
{
	if (RM_HS400X_EVENT_SUCCESS == p_args->event)
	{
		hs400x_i2c_callback_status = SENSOR_CALLBACK_STATUS_SUCCESS;
	}
	else
	{
		hs400x_i2c_callback_status = SENSOR_CALLBACK_STATUS_REPEAT;
	}
}

/**********************************************************************
 * General
 *********************************************************************/

static void Demo_err(void)
{
	while (1)
	{
		// nothing
	}
}
