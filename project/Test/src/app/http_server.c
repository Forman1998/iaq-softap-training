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
#include "http_server.h"
#include "da16200_AT.h"
#include "string.h"
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	int16_t temperature_int; /**< integer portion of the temperature*/
	int16_t humidity_int; /**< integer portion of the humidity*/
}sensor_data_t;

static sensor_data_t sensor_data = {0, 0};

static char my_get_contents_updated[] =
		"<html style= \"font-family: Arial, sans-serif\">"
		"<head>"
		"<meta http-equiv=\"refresh\" content=\"10\">"
		"<style>"
		".hide {display: none;}"
		".margin{margin: 25px;}"
		".semi-donut{"
		"--percentage: 0; --fill: #ff0;width: 450px; height: 225px; position: relative; text-align: center; color: #fff; font-size: 22px; font-weight: 600; border-radius: 225px 225px 0 0; overflow: hidden; color: black; display: flex; align-items: flex-end; justify-content: center; box-sizing : border-box; "
		"&:before, &:after{content: '';width: 450px; height: 225px; border:50px solid var(--fill); border-top: none; position: absolute; transform-origin :50%%  0%% 0; border-radius: 0 0 300px 300px ; box-sizing : border-box;left: 0;top: 100%%;}"
		"&:before {border-color:rgba(0,0,0,.15);transform: rotate(180deg);}"
		"&:after{z-index: 3;animation : 1s fillGraphAnimation ease-in;transform:rotate(calc(1deg*(var(--percentage)*1.8)));}"
		"}"
		".semi-donut:hover +.hide{ display: block; color: var(--fill);}"
		"</style>"
		"</head>"
		"<body style= \"margin: 0; background-color:=#fcfcfd\">"
		"<header style=\"height: 80px; color: #fcfcfd; background-color: #29289D; text-align: center;font-size: x-large;\">"
		"<h2 style=\"float: left;  max-width:1000px;  margin-left: 100px;  margin-top: 30px;  font-size: x-large;\"> RL78/G23 - DA16200 WiFi Demo </h2>"
#ifdef LOGO_SUPPORTED
		"<img style = \"height: 80px; width: 800px;  float: right;  margin-top: 20px;  margin-right: 20px;\" src=\"data:image/jpeg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD//gAfQ29tcHJlc3NlZCBieSBqcGVnLXJlY29tcHJlc3P/2wCEAA0NDQ0ODQ4QEA4UFhMWFB4bGRkbHi0gIiAiIC1EKjIqKjIqRDxJOzc7STxsVUtLVWx9aWNpfZeHh5e+tb75+f8BDQ0NDQ4NDhAQDhQWExYUHhsZGRseLSAiICIgLUQqMioqMipEPEk7NztJPGxVS0tVbH1pY2l9l4eHl761vvn5///CABEIAIMC+wMBIgACEQEDEQH/xAAVAAEBAAAAAAAAAAAAAAAAAAAAB//aAAgBAQAAAACnAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/8QAFAEBAAAAAAAAAAAAAAAAAAAAAP/aAAgBAhAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/xAAUAQEAAAAAAAAAAAAAAAAAAAAA/9oACAEDEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP/EABQQAQAAAAAAAAAAAAAAAAAAAKD/2gAIAQEAAT8ATB//xAAUEQEAAAAAAAAAAAAAAAAAAACA/9oACAECAQE/AFh//8QAFBEBAAAAAAAAAAAAAAAAAAAAgP/aAAgBAwEBPwBYf//Z\"  />"
#endif
		"</header><br>"
		"<br>\r\n"
		"<br>\r\n"
		"<br>\r\n"

		"<table ALIGN=\"center\" width=\"1000\">\r\n"

		"<tr>"
		"<H1 ALIGN=\"center\"> "
		"<td>"
		"<div class=\"semi-donut margin\" id = \"temp\"  style=\"--percentage : %2d; --fill: %s;\"><font size=\"+3\">%2d&#176C<br>\r\n   <font size=\"5\">Temperature</font></font></div>"
		"</td>\r\n"
		"<td>"
		"<div class=\"semi-donut margin\" id = \"humidity\" style=\"--percentage : %2d; --fill: %s ;\"><font size=\"+3\">%2d&#37<br>\r\n  <font size=\"5\">Humidity</font></font></div>"
		"</td>\r\n"		"</H1>\r\n"
		"</tr>"

		"</table>\r\n"

		"<br>\r\n"

		"<form ALIGN=\"center\" method=\"POST\" action=\"index.html\">\r\n"
		"<br><input type=submit name=\"blink\" value=\"BLINK\"> \r\n \r\n"
		"</form>\r\n\r\n"

		"<H1 ALIGN=\"center\">"
		"  Powered By Renesas RL78 "
		"</H1>\r\n"

		"<br><br>\r\n"
		"</body>"
		"</html>";



static uint8_t http_buffer[2048];
static uint8_t temp_buffer[2048];
static uint8_t user_buffer[2048];
static uint8_t resp_buff[2048] = {0};

static void http_ippt_get(uint8_t * buffer, uint8_t *ippt);

/************************************************************************************
 * Name:       http_server_receive
 * Function:   receive data and process receive message
 * Parameters: http_ippt - http IP port info
 * Return:     http receive status
 ************************************************************************************/
http_receive_status http_server_receive( uint8_t * http_ippt)
{
	http_receive_status retval = HTTP_ERROR;
	uint16_t bytes_read = 2048U;
	memset(resp_buff, 0, 2048);
	wifi_serial_read(resp_buff, &bytes_read, 300);

	if(is_str_present((const char*)resp_buff, "/index.html") || is_str_present((const char*)resp_buff, "/"))
	{
		if(is_str_present((const char*)resp_buff, "GET")
				&& !is_str_present((const char*)resp_buff,"blink=BLINK"))
		{
			http_ippt_get(resp_buff,http_ippt);
			return HTTP_GET;
		}
		else if(is_str_present((const char*)resp_buff,"blink=BLINK"))
		{
			http_ippt_get(resp_buff,http_ippt);
			if(http_ippt[0] !='1')
			{
				return HTTP_ERROR;
			}
			return HTTP_LED_BLINK;
		}
		else
		{
			/*Do Nothing*/
		}
	}

	return retval;
}

/************************************************************************************
 * Name:       http_server_send
 * Function:   send http data
 * Parameters: http_ippt, data, length
 * Return:     none
 ************************************************************************************/
void http_server_send(uint8_t * http_ippt,uint8_t * data, uint16_t length)
{
	uint16_t length_num;
	uint16_t length_len;
	uint16_t head_len;
	uint16_t data_len;
	char lenth_str[10];
	memset(lenth_str,0, 10);
	length_num = length;

	memset(http_buffer, 0, 2048);
	length_len = length_num;

	/* <ESC>S - data transmission command in 5.6.9.1 of um-wi-003-da16200*/
	sprintf((char *)http_buffer, "\x1BS0%d,",length_len);
	strcat((char *)http_buffer, (char *)http_ippt);
	head_len = (uint16_t)strlen((char *)http_buffer);
	strcpy((char *)(http_buffer+head_len), (char *)data);
	data_len = (uint16_t)strlen((char *)http_buffer);
	wifi_serial_write(http_buffer, data_len);
}

/************************************************************************************
 * Name:       http_ippt_get
 * Function:   get ip and port data
 * Parameters: buffer, ippt
 * Return:     none
 ************************************************************************************/
static void http_ippt_get(uint8_t * buffer, uint8_t *ippt)
{
	char * p;
	uint8_t ippt_count = 0;

	p = strstr((char *)buffer, "+TRDTS:0,");
	if(0U == p)
	{
		return ;
	}
	p = p + strlen("+TRDTS:0,");
	while(*p != ',')
	{
		*ippt =(uint8_t) *p;
		ippt++;
		p++;
		ippt_count++;
		if(ippt_count >= 30)
		{
			return;
		}
	}
	*ippt =(uint8_t) *p;
	ippt++;
	p++;
	while(*p != ',')
	{
		*ippt =(uint8_t) *p;
		ippt++;
		p++;
	}
	*ippt =(uint8_t) *p;
	ippt++;
	p++;
}
/************************************************************************************
 * Name:       http_update
 * Function:   update html page
 * Parameters: http_ippt
 * Return:     none
 ************************************************************************************/
void  http_update(uint8_t * http_ippt)
{
	uint16_t len;
	sensor_data.humidity_int = Sensor_get_humidity();
	sensor_data.temperature_int = Sensor_get_temp();
	memset(user_buffer, 0 , 2048);
	char temp_colour[8] = "";
	char humidity_colour[8] = "";
	if(sensor_data.temperature_int < 25) {
		strcpy(temp_colour,"#FEDA3E");
	}
	else if(sensor_data.temperature_int < 35)
	{
		strcpy(temp_colour,"#FFA500");
	}
	else{
		strcpy(temp_colour,"#E62A39");
	}


	if(sensor_data.humidity_int < 25) {
		strcpy(humidity_colour,"#86CEFA");
	}
	else if(sensor_data.humidity_int < 50)
	{
		strcpy(humidity_colour,"#5494DA");
	}
	else if(sensor_data.humidity_int < 75)
	{
		strcpy(humidity_colour,"#1750AC");
	}
	else{
		strcpy(humidity_colour,"#152238");
	}
	sprintf((char *)user_buffer,(char *)my_get_contents_updated,(uint16_t)(sensor_data.temperature_int*100/65),(char *) temp_colour, (uint16_t)sensor_data.temperature_int,(uint16_t)sensor_data.humidity_int,(char *)humidity_colour,(uint16_t)sensor_data.humidity_int,0U);
	len = (uint16_t)strlen((char *)user_buffer);
	sprintf((char *)temp_buffer, "HTTP/1.1 200 \r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n", len);

	http_server_send(http_ippt,temp_buffer, (uint16_t)strlen((char *)temp_buffer));
	http_server_send(http_ippt,user_buffer, len);
}
