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
#include "string.h"
#include "sensor.h"

/** AT Command sets */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
da16200_at_cmd_set_t g_da16200_cmd_set[] =
{
		/** Intial AT command */
		[DA16200_AT_CMD_INDEX_ATZ] =
		{
				.p_cmd = (uint8_t *) "ATZ\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_10,
				.retry_delay = DA16200_DELAY_500MS
		},
		/** Echo on/ off */
		[DA16200_AT_CMD_INDEX_ATE] =
		{
				.p_cmd = (uint8_t *) "ATE\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set AP mode */
		[DA16200_AT_CMD_INDEX_AT_WFMODE] =
		{
				.p_cmd = (uint8_t *) "AT+WFMODE=1\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set Country Code */
		[DA16200_AT_CMD_INDEX_AT_WFCC] =
		{
				.p_cmd = (uint8_t *) "AT+WFCC=GB\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Set Country Code */
		[ DA16200_AT_CMD_INDEX_AT_WFSAP] =
		{
				.p_cmd = (uint8_t *) "AT+WFSAP=Renesas_Wifi,3,1,12345678,1,CH\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* Restart */
		[ DA16200_AT_CMD_INDEX_AT_RESTART] =
		{
				.p_cmd = (uint8_t *) "AT+RESTART\r\n",
				.p_success_resp = (uint8_t *) "DONE",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_1000MS
		},
		/* Set IP */
		[ DA16200_AT_CMD_INDEX_AT_NWIP] =
		{
				.p_cmd = (uint8_t *) "AT+NWIP=1,192.168.10.2,255.255.255.0,192.168.10.1\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_1000MS
		},
		/* Start DHCP */
		[ DA16200_AT_CMD_INDEX_AT_NWDHS] =
		{
				.p_cmd = (uint8_t *) "AT+NWDHS=1\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* DHCP IP area set*/
		[ DA16200_AT_CMD_INDEX_AT_NWDHR] =
		{
				.p_cmd = (uint8_t *) "AT+NWDHR=192.168.10.3,192.168.10.10\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* TCP server port set*/
		[ DA16200_AT_CMD_INDEX_AT_TRTS] =
		{
				.p_cmd = (uint8_t *) "AT+TRTS=80\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		/* TCP server port message save */
		[ DA16200_AT_CMD_INDEX_AT_TRSAVE] =
		{
				.p_cmd = (uint8_t *) "AT+TRSAVE\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		},
		[ DA16200_AT_CMD_INDEX_SDKVER ] =
		{
				.p_cmd = (uint8_t *) "AT+SDKVER\r\n",
				.p_success_resp = (uint8_t *) "OK",
				.retry = DA16200_RETRY_VALUE_5,
				.retry_delay = DA16200_DELAY_200MS
		}


};

static uint8_t resp_buf[DA16200_STR_LEN_512] = {0U,};

#define DA16200_CMD_SET_LENGTH	(sizeof(g_da16200_cmd_set)/sizeof(g_da16200_cmd_set[0]))

/** @brief Issue WiFi AT Command
 * @param command_index - index of command to handle
 * @param timeout_ms - milliseconds timeout for command handling
 * @return FSP_SUCCESS - successfully detected response
 * FSP_ERR_TIMEOUT - timed out waiting for successful response.
 */
static fsp_err_t wifi_command_issue(da16200_at_cmd_index_t command_index, uint16_t timeout_ms);

/** @brief Wait for AT command response and test if it is the correct response.
 * @param p_resp - response to test UART read data against.
 * @param timeout_ms - maximum delay to wait for successful response.
 * @return FSP_SUCCESS - successfully detected response
 * FSP_ERR_TIMEOUT - timed out waiting for successful response.
 */
static fsp_err_t wifi_serial_read_response(uint8_t * p_resp, uint32_t timeout_ms);

static fsp_err_t wifi_command_issue(da16200_at_cmd_index_t command_index, uint16_t timeout_ms)
{
	fsp_err_t result = FSP_SUCCESS;
	uint8_t retry_count = 0U;
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;
	memset(resp_buf,0,512);
	do
	{
		/* Flush the buffer*/
		(void)Hal_uart_read((char *) resp_buf);

		/* Transmit data*/
		wifi_serial_write(p_cmd_set[command_index].p_cmd, p_cmd_set[command_index].cmd_length);

		/* wait for and process response*/
		result = wifi_serial_read_response(p_cmd_set[command_index].p_success_resp, timeout_ms);

		/* If success response has not been received - delay*/
		if(FSP_SUCCESS != result)
		{
			R_BSP_SoftwareDelay(p_cmd_set[command_index].retry_delay, BSP_DELAY_MILLISECS);
			++retry_count;
		}
	}
	while( (retry_count < p_cmd_set[command_index].retry) && (FSP_SUCCESS != result) );

	return result;
}
/* END OF FUNCTION*/

fsp_err_t wifi_init(void)
{
	fsp_err_t status = FSP_SUCCESS;
	da16200_at_cmd_set_t  * p_cmd_set = g_da16200_cmd_set;

	/* Enable the UART*/
	Hal_uart_start();
	
	Sensor_init();
	
	/* Initialise cmd lengths*/
	for(uint16_t i = 0U; i < DA16200_CMD_SET_LENGTH; ++i)
	{
		p_cmd_set[i].cmd_length = strlen((char *) p_cmd_set[i].p_cmd);
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_ATZ, 100);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_ATE, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}
	status = wifi_command_issue(DA16200_AT_CMD_INDEX_SDKVER, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

#ifdef LOGO_SUPPORTED
	/* TODO: Check version is compatible with transmission of image and include image if it is*/
#endif

	return status;
}
/* END OF FUNCTION*/

fsp_err_t wifi_set(void)
{
	fsp_err_t status = FSP_SUCCESS;

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_WFMODE, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_WFCC, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_WFSAP, 3000);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_RESTART, 8000);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_ATE, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_NWIP, 3000);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_NWDHS, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_NWDHR, 2000);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_TRTS, 500);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	status = wifi_command_issue(DA16200_AT_CMD_INDEX_AT_TRSAVE, 300);
	if(status != FSP_SUCCESS)
	{
		return status;
	}

	return status;
}
/* END OF FUNCTION*/

void wifi_serial_write(uint8_t * p_src, uint16_t len)
{
	/* Wait until TX is ready*/
	while(!Hal_uart_tx_ready())
	{
		/* Do Nothing*/
	}

	/* Transmit data*/
	Hal_uart_send((char *)p_src, len);

	/* Wait until TX is ready*/
	while(!Hal_uart_tx_ready())
	{
		/* Do Nothing*/
	}
}
/* END OF FUNCTION*/

fsp_err_t wifi_serial_read(uint8_t * p_dst, uint16_t * p_num_bytes, uint16_t timeout_ms)
{
	fsp_err_t status = FSP_ERR_ASSERTION;
	uint16_t total_count = 0U;
	const uint16_t max_bytes = *p_num_bytes;

	/* Start timeout timer*/
	Hal_oneshot_start(timeout_ms);

	do
	{
		/* If bytes are available*/
		if(Hal_uart_rx_ready())
		{
			/* Check if success response is present*/
			total_count += Hal_uart_read((char *)&p_dst[total_count]);
			if(total_count >= max_bytes)
			{
				status = FSP_SUCCESS;
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

	*p_num_bytes = total_count;

	return status;
}
/* END OF FUNCTION*/

uint8_t is_str_present(const char * p_resp_str, const char * p_search_str)
{
	if (strstr (p_resp_str, p_search_str))
	{
		return SF_WIFI_TRUE;
	}

	return SF_WIFI_FALSE;
}
/* END OF FUNCTION*/

static fsp_err_t wifi_serial_read_response(uint8_t * p_resp, uint32_t timeout_ms)
{
	fsp_err_t status = FSP_ERR_ASSERTION;
	uint16_t total_count = 0U;
	const uint16_t max_bytes_read = strlen((char *)p_resp);
	uint16_t resp_length = max_bytes_read;
	uint8_t expected_resp_present;

	do
	{
		status = wifi_serial_read(&resp_buf[total_count], &resp_length, timeout_ms);

		if(resp_length > 0U)
		{
			total_count += resp_length;
			resp_length = max_bytes_read;

			expected_resp_present = is_str_present((const char *)resp_buf, (const char*)p_resp);
			if(SF_WIFI_TRUE == expected_resp_present)
			{
				status = FSP_SUCCESS;
			}
			else
			{
				status = FSP_ERR_ASSERTION;
			}
		}
	}
	while( (FSP_SUCCESS != status) && (FSP_ERR_TIMEOUT != status) );

	return status;
}
/* END OF FUNCTION*/

