#include "hal.h"
#include "da16200_AT.h"
#include "sensor.h"
#include <string.h>
#include <stdio.h>
#include "st7735s.h"
#include "text.h"

static const uint8_t COLOUR_BLACK[] = {0x00, 0x00};
static const uint8_t COLOUR_WHITE[] = {0xFF, 0xFF};
static ST7735S_display_area_info_t disp_info;

static void mqtt_msg_send(void);
static fsp_err_t try_read_mqtt_msg(void);
static void demo_err(void);
static void write_string_to_display(const char * const str);

void main(void)
{
	static fsp_err_t err;

	EI();

	/* Enable the UART*/
	Hal_uart_start();

	/* Initialise the sensor*/
	Sensor_init();

	/* Enable display SPI*/
	R_Config_CSI00_Start_app();

	/* Initialse display*/
	St7735s_init(COLOUR_WHITE);
	St7735s_get_display_area_info(&disp_info);
	Text_init(disp_info.xmax, disp_info.ymax);
	Text_set_font(&default_font);
	St7735s_wake_display();
	St7735s_display_on();
	St7735s_refresh();
	CCS6 = 0x02U; /* 5mA backlight*/

	/* Connect MCU with Wi-Fi module and initialize Wi-Fi module */
	write_string_to_display("Connecting\nDA16200...");
	err = wifi_con_init();
	if(FSP_SUCCESS != err)
	{
		demo_err();
	}

	write_string_to_display("Success!");
	R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);

	/* Configure Wi-Fi module to station mode and let it connect to AP */
	write_string_to_display("Connecting\nWiFi...");
	err = wifi_con_routine();
	if(FSP_SUCCESS != err)
	{
		demo_err();
	}

	write_string_to_display("Success!");
	R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);

	/* Configure Wi-Fi module to MQTT client function */
	write_string_to_display("Connecting\nMQTT...");
	err = mqtt_con_routine();
	if(FSP_SUCCESS != err)
	{
		demo_err();
	}

	write_string_to_display("Success!");
	R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);

	Sensor_read(); /* Dummy read to populate structs with data*/

	while(1)
	{
		/* If we haven't received an LED toggle - send temperature and humidity update*/
		if(FSP_SUCCESS != try_read_mqtt_msg())
		{
			Sensor_read();

			mqtt_msg_send();
		}
	}
}

static void mqtt_msg_send(void)
{
	static char temperature_value_str[10];
	static char humidity_value_str[10];
	static char mqtt_head[] = "AT+NWMQMSG=";
	static float l_temperature = 0.00f;
	static float l_humidity = 0.00f;
	fsp_err_t err;

	l_temperature = (float)Sensor_get_temp();
	l_humidity = (float)Sensor_get_humidity();

	sprintf(temperature_value_str, "%f", l_temperature);
	sprintf(humidity_value_str, "%f", l_humidity);

	g_da16200_cmd_set[DA16200_AT_CMD_INDEX_AT_NWMQMSG].p_cmd = (uint8_t *)mqtt_head;

	part_array[0] = (uint32_t)temperature_value_str;
	part_array[1] = (uint32_t)at_topic_t;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQMSG, part_array, 3);
	write_string_to_display("Sending...\nTemperature");
	err = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQMSG);
	if(FSP_SUCCESS != err)
	{
		write_string_to_display("Error!");
		R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
	}

	R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);

	g_da16200_cmd_set[DA16200_AT_CMD_INDEX_AT_NWMQMSG].p_cmd = (uint8_t *)mqtt_head;

	part_array[0] = (uint32_t)humidity_value_str;
	part_array[1] = (uint32_t)at_topic_h;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQMSG, part_array, 3);
	write_string_to_display("Sending...\nHumidity");
	err = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQMSG);
	if(FSP_SUCCESS != err)
	{
		write_string_to_display("Error!");
		R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
	}
}
/* END OF FUNCTION*/

static fsp_err_t try_read_mqtt_msg(void)
{
	fsp_err_t status = FSP_ERR_ASSERTION;
	uint16_t total_count = 0U;
	static uint8_t mqtt_read_buf[512U] = {0U,};

	memset(mqtt_read_buf, 0, 512);

	write_string_to_display("MQTT Read...");

	/* Start timeout timer*/
	Hal_oneshot_start(4000U);

	do
	{
		/* If bytes are available*/
		if(Hal_uart_rx_ready())
		{
			/* Check if success response is present*/
			total_count += Hal_uart_read((char *)&mqtt_read_buf[total_count]);
			if(STRING_EXIST == is_str_present((const char *)mqtt_read_buf, "led_toggle,1"))
			{
				CCS0 ^= 0x01U; /* Toggle LED*/
				write_string_to_display("Success!");
				R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
				status = FSP_SUCCESS;
			}
			else if(STRING_EXIST == is_str_present((const char *)mqtt_read_buf, "buzzer,1"))
			{
				/* Enable Buzzer*/
				R_Config_PCLBUZ1_Start();
				write_string_to_display("Success!");
				R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
				R_Config_PCLBUZ1_Stop();
				status = FSP_SUCCESS;
			}
			else
			{
				/* Do Nothing*/
			}
		}
		else if(Hal_oneshot_elapsed())
		{
			/* If oneshot has elpased - timeout occurred*/
			write_string_to_display("No Data!");
			R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
			status = FSP_ERR_TIMEOUT;
		}
		else
		{
			/* Do Nothing*/
		}
	}
	while( (FSP_SUCCESS != status) && (FSP_ERR_TIMEOUT != status) );

	return status;
}
/* END OF FUNCTION*/

static void demo_err(void)
{
	write_string_to_display("Fatal Error!");

	while(1)
	{
		/* Infinite Hang...*/
	}
}
/* END OF FUNCTION*/

static void write_string_to_display(const char * const str)
{
	static char prev_string[128] = {0,};
	static uint16_t str_len = 0U;

	/* remove old string if necessary*/
	if(str_len > 0U)
	{
		(void)Text_put_str(20U, 20U, prev_string, COLOUR_WHITE, COLOUR_WHITE);
	}

	str_len = Text_put_str(20U, 20U, str, COLOUR_BLACK, COLOUR_WHITE);

	St7735s_refresh();

	(void)strcpy(prev_string, str);
}
/* END OF FUNCTION*/
