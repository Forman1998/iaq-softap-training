/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#include "da16200_AT.h"
#include <string.h>
#include <stdio.h>

char country[]          = "CH";
char ap_ssid[]          = "'"WIFI_SSID"',";
char ap_pw[]            = WIFI_PASSWORD;
char mqtt_broker_ip[]   = "io.adafruit.com,";
char mqtt_port[]        = "1883";
char mqtt_broker_ssid[] = ADAFRUIT_USERNAME",";
char mqtt_broker_pw[]   = ADAFRUIT_AIO_KEY;
char mqtt_client_id[]   = "DA16200-client";
char at_cmd_end[]       = "\r\n";
char at_topic_t[]         = ","ADAFRUIT_USERNAME"/feeds/temperature";
char at_topic_h[]         = ","ADAFRUIT_USERNAME"/feeds/humidity";

uint8_t     at_cmd_data[AT_CMD_LENGTH];
uint32_t    part_array[3];
char        ip_addr[20];

/** AT Command sets */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
da16200_at_cmd_set_t g_da16200_cmd_set[] =
{
		/** Intial AT command */
		[DA16200_AT_CMD_INDEX_ATZ] =
		{
				.p_cmd = (uint8_t *) "ATZ\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_64,
				.retry = DA16200_RETRY_VALUE_10,
				.retry_delay = DA16200_DELAY_500MS
		},
		/** Echo on/off */
		[DA16200_AT_CMD_INDEX_ATE] =
		{
				.p_cmd = (uint8_t *) "ATE\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_32,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set AP mode, here is station */
		[DA16200_AT_CMD_INDEX_AT_WFMODE] =
		{
				.p_cmd = (uint8_t *) "AT+WFMODE=0\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_32,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set Country Code */
		[DA16200_AT_CMD_INDEX_AT_WFCC] =
		{
				.p_cmd = (uint8_t *) "AT+WFCC=",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_32,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* All profiles in NVRAM are removed and set up in Soft-AP mode with the default configuration. */
		[ DA16200_AT_CMD_INDEX_AT_DEFAP] =
		{
				.p_cmd = (uint8_t *) "AT+DEFAP\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.p_success_resp[1] = (uint8_t *) "+INIT:DONE,1",
				.max_resp_length = DA16200_STR_LEN_128,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* System restart */
		[ DA16200_AT_CMD_INDEX_AT_RESTART] =
		{
				.p_cmd = (uint8_t *) "AT+RESTART\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.p_success_resp[1] = (uint8_t *) "+INIT:DONE,0",
				.max_resp_length = DA16200_STR_LEN_128,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_1000MS
		},
		/* Connect to an AP */
		[ DA16200_AT_CMD_INDEX_AT_WFJAPA] =
		{
				.p_cmd = (uint8_t *) ("AT+WFJAPA="),
				.p_success_resp[0] = (uint8_t *) "OK",
				.p_success_resp[1] = (uint8_t *) "+WFJAP:1",
				.max_resp_length = DA16200_STR_LEN_128,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_1000MS
		},
		/* Start the DHCP Client */
		[ DA16200_AT_CMD_INDEX_AT_NWDHC] =
		{
				.p_cmd = (uint8_t *) "AT+NWDHC=1\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_64,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Start the DHCP Client */
		[ DA16200_AT_CMD_INDEX_AT_NWDHC_READ] =
		{
				.p_cmd = (uint8_t *) "AT+NWDHC=?\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.p_success_resp[1] = (uint8_t *) "+NWDHC:1",
				.max_resp_length = DA16200_STR_LEN_64,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set the IP address and the port number of the MQTT Broker */
		[ DA16200_AT_CMD_INDEX_AT_NWMQBR] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQBR=",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_64,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set the MQTT QoS level */
		[ DA16200_AT_CMD_INDEX_AT_NWMQQOS] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQQOS=0\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_64,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* MQTT login information */
		[ DA16200_AT_CMD_INDEX_AT_NWMQLI] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQLI=",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_32,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set the MQTT Client ID */
		[ DA16200_AT_CMD_INDEX_AT_NWMQCID] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQCID=",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_32,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Enable the MQTT client, 1 enable, 0 disable */
		[ DA16200_AT_CMD_INDEX_AT_NWMQCL] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQCL=1\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_32,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Publish an MQTT message with <msg>,<topic> */
		[ DA16200_AT_CMD_INDEX_AT_NWMQMSG] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQMSG=",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_128,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_1000MS
		},
		[ DA16200_AT_CMD_INDEX_AT_NWMQTS] =
		{
				.p_cmd = (uint8_t *) "AT+NWMQTS=2,"ADAFRUIT_USERNAME"/feeds/led_toggle,"ADAFRUIT_USERNAME"/feeds/buzzer\r\n",
				.p_success_resp[0] = (uint8_t *) "OK",
				.max_resp_length = DA16200_STR_LEN_64,
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		}
};

static fsp_err_t wifi_serial_read(uint8_t * p_dest, const uint8_t * expected_resp0, const uint8_t * expected_resp1, uint32_t timeout_ms);
static void wifi_serial_write(uint8_t * p_src, uint16_t bytes);
static void DA16200_err(void);

/************************************************************************************
 * Name:       AT_cmd_send_ok
 * Function:   AT command with OK feedback only
 * Parameters: da16200_at_cmd_index_t CMD index
 * Return:     AT command operation status
 ************************************************************************************/
fsp_err_t AT_cmd_send_ok(da16200_at_cmd_index_t cmd_index)
{
	uint16_t    bytes_write;
	static uint8_t     resp_buff[DA16200_STR_LEN_128] = {0};
	fsp_err_t   retval = FSP_ERR_ASSERTION;
	fsp_err_t   result;
	uint8_t     retry_count = 0U;
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;

	do
	{
		bytes_write = (uint16_t)strlen((char *) p_cmd_set[cmd_index].p_cmd);
		wifi_serial_write((uint8_t*)p_cmd_set[cmd_index].p_cmd, bytes_write);

		/** Clear respond memory **/
		memset (resp_buff, 0 , sizeof(resp_buff));
		result = wifi_serial_read(resp_buff, p_cmd_set[cmd_index].p_success_resp[0], NULL, 1000);

		if(FSP_SUCCESS == result)
		{
			retval = FSP_SUCCESS;
			break;
		}

		R_BSP_SoftwareDelay(p_cmd_set[cmd_index].retry_delay, BSP_DELAY_MILLISECS);
		++retry_count;
	}
	while(retry_count < p_cmd_set[cmd_index].retry);

	return retval;
}

/************************************************************************************
 * Name:       AT_cmd_send_data
 * Function:   AT command with OK and DATA feedback
 * Parameters: da16200_at_cmd_index_t    CMD index
 *             uint16_t                  wait time (ms)
 * Return:     AT command operation status
 ************************************************************************************/
fsp_err_t AT_cmd_send_data(da16200_at_cmd_index_t cmd_index, uint16_t wait_time_ms)
{
	uint16_t    bytes_write;
	static uint8_t     resp_buff[DA16200_STR_LEN_128] = {0};
	fsp_err_t   retval = FSP_ERR_ASSERTION;
	fsp_err_t   result;
	uint8_t     retry_count = 0U;
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;

	do
	{
		bytes_write = (uint16_t)strlen((char *) p_cmd_set[cmd_index].p_cmd);
		wifi_serial_write((uint8_t*)p_cmd_set[cmd_index].p_cmd, bytes_write);

		/** Clear respond memory **/
		memset (resp_buff, 0 , sizeof(resp_buff));
		result = wifi_serial_read(resp_buff, p_cmd_set[cmd_index].p_success_resp[0], p_cmd_set[cmd_index].p_success_resp[1], 1000);

		if(FSP_SUCCESS == result)
		{
			retval = FSP_SUCCESS;
			break;
		}

		R_BSP_SoftwareDelay(p_cmd_set[cmd_index].retry_delay, BSP_DELAY_MILLISECS);
		++retry_count;
	}
	while(retry_count < p_cmd_set[cmd_index].retry);

	return retval;
}

/************************************************************************************
 * Name:       AT_cmd_set_confirm
 * Function:   AT command confirm if the value has been set already
 * Parameters: da16200_at_cmd_index_t CMD index
 * Return:     AT command operation status
 ************************************************************************************/
fsp_err_t AT_cmd_set_confirm(da16200_at_cmd_index_t cmd_index)
{
	uint16_t    bytes_write;
	static uint8_t     resp_buff[DA16200_STR_LEN_64] = {0};
	fsp_err_t   retval = FSP_ERR_ASSERTION;
	fsp_err_t   result;
	uint8_t     retry_count = 0U;
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;

	do
	{
		bytes_write = (uint16_t)strlen((char *) p_cmd_set[cmd_index].p_cmd);
		wifi_serial_write((uint8_t*)p_cmd_set[cmd_index].p_cmd, bytes_write);

		/** Clear respond memory **/
		memset (resp_buff, 0 , sizeof(resp_buff));
		result = wifi_serial_read(resp_buff, p_cmd_set[cmd_index].p_success_resp[0], p_cmd_set[cmd_index].p_success_resp[1], 1000);

		if(FSP_SUCCESS == result)
		{
			retval = FSP_SUCCESS;
			break;
		}

		R_BSP_SoftwareDelay(p_cmd_set[cmd_index].retry_delay, BSP_DELAY_MILLISECS);
		++retry_count;
	}
	while(retry_count < p_cmd_set[cmd_index].retry);

	return retval;
}

/************************************************************************************
 * Name:       AT_cmd_rcv_data
 * Function:   AT command get information from DA16200
 * Parameters: da16200_at_cmd_index_t CMD index
 * Return:     AT command operation status
 ************************************************************************************/
fsp_err_t AT_cmd_rcv_data(da16200_at_cmd_index_t cmd_index, uint16_t wait_time_ms)
{
	uint16_t    bytes_write;
	static uint8_t     resp_buff[DA16200_STR_LEN_128] = {0};
	fsp_err_t   retval = FSP_ERR_ASSERTION;
	fsp_err_t   result;
	uint8_t     retry_count = 0U;
	const char *    p;
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;

	do
	{
		bytes_write = (uint16_t)strlen((char *) p_cmd_set[cmd_index].p_cmd);
		wifi_serial_write((uint8_t*)p_cmd_set[cmd_index].p_cmd, bytes_write);

		/** Clear respond memory **/
		memset (resp_buff, 0 , sizeof(resp_buff));
		result = wifi_serial_read(resp_buff, p_cmd_set[cmd_index].p_success_resp[0], p_cmd_set[cmd_index].p_success_resp[1], 1000);

		if(FSP_SUCCESS == result)
		{
			p = strrchr((const char *)resp_buff,',');
			memcpy(ip_addr, (p+1), 16);
			retval = FSP_SUCCESS;
			break;
		}

		R_BSP_SoftwareDelay(p_cmd_set[cmd_index].retry_delay, BSP_DELAY_MILLISECS);
		++retry_count;
	}
	while(retry_count < p_cmd_set[cmd_index].retry);

	return retval;
}

void At_cmd_combine(da16200_at_cmd_index_t cmd_index, uint32_t * part_ptr, uint8_t number)
{
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;
	uint8_t * temp_ptr;
	uint16_t length, i;

	length = (uint16_t)strlen((char *) p_cmd_set[cmd_index].p_cmd);
	temp_ptr = p_cmd_set[cmd_index].p_cmd;
	for(i = 0; i < AT_CMD_LENGTH; i++)
	{
		at_cmd_data[i] = 0;
	}
	for(i = 0; i < length; i++)
	{
		at_cmd_data[i] = *temp_ptr;
		temp_ptr++;
	}
	for(i = 0; i < number; i++)
	{
		strcat((char *)at_cmd_data, (char *)(*(part_ptr + i)));
	}
	p_cmd_set[cmd_index].p_cmd = (uint8_t *)at_cmd_data;
}


/************************************************************************************
 * Name:       wifi_con_init
 * Function:   wifi connection test by ATZ CMD
 * Parameters: none
 * Return:     none
 ************************************************************************************/
fsp_err_t wifi_con_init(void)
{
	fsp_err_t status;

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_ATZ);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_ATE);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	return status;
}

/************************************************************************************
 * Name:       wifi_con_routine
 * Function:   Start wifi connection procedure
 * Parameters: none
 * Return:     wifi connection status
 ************************************************************************************/
fsp_err_t wifi_con_routine(void)
{
	fsp_err_t status = FSP_SUCCESS;

	status = AT_cmd_send_data(DA16200_AT_CMD_INDEX_AT_DEFAP, 4000);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_ATE);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	part_array[0] = (uint32_t)country;
	part_array[1] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_WFCC, part_array, 2);
	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_WFCC);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_WFMODE);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQTS);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_data(DA16200_AT_CMD_INDEX_AT_RESTART, 3000);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_ATE);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_set_confirm(DA16200_AT_CMD_INDEX_AT_NWDHC_READ);
	if(status != FSP_SUCCESS)
	{
		status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWDHC);
		{
			DA16200_err();
		}
	}

	part_array[0] = (uint32_t)ap_ssid;
	part_array[1] = (uint32_t)ap_pw;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_WFJAPA, part_array, 3);
	status = AT_cmd_rcv_data(DA16200_AT_CMD_INDEX_AT_WFJAPA, 4000);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	return status;
}

/************************************************************************************
 * Name:       mqtt_con_routine
 * Function:   Start MQTT Server connection procedure
 * Parameters: none
 * Return:     MQTT connection status
 ************************************************************************************/
fsp_err_t mqtt_con_routine(void)
{
	fsp_err_t status = FSP_SUCCESS;

	part_array[0] = (uint32_t)mqtt_broker_ip;
	part_array[1] = (uint32_t)mqtt_port;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQBR, part_array, 3);
	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQBR);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQQOS);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	part_array[0] = (uint32_t)mqtt_broker_ssid;
	part_array[1] = (uint32_t)mqtt_broker_pw;
	part_array[2] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQLI, part_array, 3);
	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQLI);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	part_array[0] = (uint32_t)mqtt_client_id;
	part_array[1] = (uint32_t)at_cmd_end;
	At_cmd_combine(DA16200_AT_CMD_INDEX_AT_NWMQCID, part_array, 2);
	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQCID);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	status = AT_cmd_send_ok(DA16200_AT_CMD_INDEX_AT_NWMQCL);
	if(status != FSP_SUCCESS)
	{
		DA16200_err();
	}

	return status;
}

/************************************************************************************
 * Name:       is_str_present
 * Function:   compare string
 * Parameters: p_resp_str, p_search_str
 * Return:     comparing result
 ************************************************************************************/
uint8_t is_str_present(const char * p_resp_str, const char * p_search_str)
{
	if (strstr(p_resp_str, p_search_str))
	{
		return STRING_EXIST;
	}

	return STRING_ABSENCE;
}
/* END OF FUNCTION*/

/************************************************************************************
 * Name:       wifi_serial_read
 * Function:   read receive data
 * Parameters: p_dest, p_bytes, timeout_ms
 * Return:     read result
 ************************************************************************************/
static fsp_err_t wifi_serial_read(uint8_t * p_dest, const uint8_t * expected_resp0, const uint8_t * expected_resp1, uint32_t timeout_ms)
{
	fsp_err_t status = FSP_ERR_ASSERTION;
	uint16_t total_count = 0U;

	/* Start timeout timer*/
	Hal_oneshot_start(timeout_ms);

	do
	{
		/* If bytes are available*/
		if(Hal_uart_rx_ready())
		{
			/* Check if success response is present*/
			total_count += Hal_uart_read((char *)&p_dest[total_count]);
			if(STRING_EXIST == is_str_present((const char *)p_dest, (const char*)expected_resp0))
			{
				if( (STRING_EXIST == is_str_present((const char *)p_dest, (const char*)expected_resp1)) || (NULL == expected_resp1))
				{
					status = FSP_SUCCESS;
				}
			}
		}
		else if(Hal_oneshot_elapsed())
		{
			/* If oneshot has elpased - timeout occurred*/
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


/************************************************************************************
 * Name:       wifi_serial_write
 * Function:   write data
 * Parameters: p_src, bytes
 * Return:     write result
 ************************************************************************************/
static void wifi_serial_write(uint8_t * p_src, uint16_t bytes)
{
	/* Wait until TX is ready*/
	while(!Hal_uart_tx_ready())
	{
		/* Do Nothing*/
	}

	/* Transmit data*/
	Hal_uart_send((char *)p_src, bytes);

	/* Wait until TX is ready*/
	while(!Hal_uart_tx_ready())
	{
		/* Do Nothing*/
	}
}
/* END OF FUNCTION*/

static void DA16200_err(void)
{
	while(1)
	{
		/* TODO: Flash LED*/
	}
}
/* END OF FUNCTION*/
