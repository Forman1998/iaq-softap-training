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

#ifndef DA16200_AT_H_
#define DA16200_AT_H_

#include "hal.h"

/** Configuration Macros for connection*/
#define ADAFRUIT_USERNAME	"Foram1998"
#define ADAFRUIT_AIO_KEY	"aio_wHiK024niolBtlTjzfxLxX7wolAc"
#define WIFI_SSID			"Android"
#define WIFI_PASSWORD		"g9zq5q7i"

/** Macros to define string length */
#define DA16200_STR_LEN_8        (8U)       ///< Length 8
#define DA16200_STR_LEN_16       (16U)      ///< Length 16
#define DA16200_STR_LEN_32       (32U)      ///< Length 32
#define DA16200_STR_LEN_64       (64U)      ///< Length 64
#define DA16200_STR_LEN_128      (128U)     ///< Length 128
#define DA16200_STR_LEN_512      (512U)     ///< Length 512


/** DA16200 delay in milliseconds between AT command retry */
#define DA16200_DELAY_0      (0U)       ///< No delay
#define DA16200_DELAY_20MS   (20U)      ///< Delay of 20 milliseconds
#define DA16200_DELAY_50MS   (50U)      ///< Delay of 50 milliseconds
#define DA16200_DELAY_100MS  (100U)     ///< Delay of 100 milliseconds
#define DA16200_DELAY_200MS  (200U)     ///< Delay of 200 milliseconds
#define DA16200_DELAY_300MS  (300U)     ///< Delay of 300 milliseconds
#define DA16200_DELAY_500MS  (500U)     ///< Delay of 500 milliseconds
#define DA16200_DELAY_1000MS (1000U)    ///< Delay of 1000 milliseconds
#define DA16200_DELAY_2000MS (2000U)    ///< Delay of 2000 milliseconds

/** DA16200 retry count for AT command set */
#define DA16200_RETRY_VALUE_0    (0U)       ///< No Retry
#define DA16200_RETRY_VALUE_1    (1U)       ///< Retry Once
#define DA16200_RETRY_VALUE_5    (5U)       ///< Retry 5 times
#define DA16200_RETRY_VALUE_10   (10U)      ///< Retry 10 times

/** Logical Value TRUE for WiFi Feedback */
#define STRING_EXIST        (1U)
/** Logical Value FALSE for WiFi Feedback */
#define STRING_ABSENCE      (0U)

#define AT_CMD_LENGTH       128

/** Structure defining AT commands parameters */
typedef struct da16200_at_cmd_set
{
    uint8_t * p_cmd;                ///< AT Command
    uint8_t * p_success_resp[2];       ///< Success response string
    uint16_t max_resp_length;      ///< Maximum length of expected response
    uint8_t retry;                ///< Retry count
    uint16_t retry_delay;          ///< Delay between AT command retry
} da16200_at_cmd_set_t;

///< Index of AT Command
typedef enum da16200_at_cmd_index
{
    DA16200_AT_CMD_INDEX_ATZ = 0,
    DA16200_AT_CMD_INDEX_ATE,
    DA16200_AT_CMD_INDEX_AT_WFMODE,
    DA16200_AT_CMD_INDEX_AT_WFCC,
    DA16200_AT_CMD_INDEX_AT_DEFAP,
    DA16200_AT_CMD_INDEX_AT_RESTART,
    DA16200_AT_CMD_INDEX_AT_WFJAPA,
    DA16200_AT_CMD_INDEX_AT_NWDHC,
    DA16200_AT_CMD_INDEX_AT_NWDHC_READ,
    DA16200_AT_CMD_INDEX_AT_NWMQBR,
    DA16200_AT_CMD_INDEX_AT_NWMQQOS,
    DA16200_AT_CMD_INDEX_AT_NWMQLI,
    DA16200_AT_CMD_INDEX_AT_NWMQCID,
    DA16200_AT_CMD_INDEX_AT_NWMQCL,
    DA16200_AT_CMD_INDEX_AT_NWMQMSG,
    DA16200_AT_CMD_INDEX_AT_NWMQTS,
} da16200_at_cmd_index_t;

/* External variable */
extern uint32_t part_array[];
extern char at_cmd_end[];
extern char at_topic_t[];
extern char at_topic_h[];
extern da16200_at_cmd_set_t g_da16200_cmd_set[];

/* External function */
fsp_err_t AT_cmd_send_ok(da16200_at_cmd_index_t cmd_index);
fsp_err_t AT_cmd_send_data(da16200_at_cmd_index_t cmd_index, uint16_t wait_time_ms);
fsp_err_t AT_cmd_set_confirm(da16200_at_cmd_index_t cmd_index);
fsp_err_t AT_cmd_rcv_data(da16200_at_cmd_index_t cmd_index, uint16_t wait_time_ms);

fsp_err_t wifi_con_init(void);
fsp_err_t wifi_con_routine(void);
fsp_err_t mqtt_con_routine(void);

uint8_t is_str_present(const char * p_resp_str, const char * p_search_str);
void At_cmd_combine(da16200_at_cmd_index_t cmd_index, uint32_t * part_ptr, uint8_t number);

#endif /* DA16200_AT_H_ */
