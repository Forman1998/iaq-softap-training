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

/** Logical Value TRUE for Cellular */
#define SF_WIFI_TRUE        (1U)
/** Logical Value FALSE for Cellular */
#define SF_WIFI_FALSE       (0U)


/** Structure defining AT commands parameters */
typedef struct da16200_at_cmd_set
{
	uint8_t * p_cmd;                ///< AT Command
	uint16_t cmd_length;			///< AT Command length
	uint8_t * p_success_resp;       ///< Success response string
	uint8_t retry;                ///< Retry count
	uint16_t retry_delay;          ///< Delay between AT command retry
} da16200_at_cmd_set_t;

typedef enum da16200_at_cmd_index
{
	DA16200_AT_CMD_INDEX_ATZ = 0,                            ///< Index for Command ATZ
	DA16200_AT_CMD_INDEX_ATE,
	DA16200_AT_CMD_INDEX_AT_WFMODE,
	DA16200_AT_CMD_INDEX_AT_WFCC,
	DA16200_AT_CMD_INDEX_AT_WFSAP,
	DA16200_AT_CMD_INDEX_AT_RESTART,
	DA16200_AT_CMD_INDEX_AT_NWIP,
	DA16200_AT_CMD_INDEX_AT_NWDHS,
	DA16200_AT_CMD_INDEX_AT_NWDHR,
	DA16200_AT_CMD_INDEX_AT_TRTS,
	DA16200_AT_CMD_INDEX_AT_TRSAVE,
	DA16200_AT_CMD_INDEX_SDKVER,

} da16200_at_cmd_index_t;

/** @brief Initialises wifi module & drivers.
 * @return FSP_SUCCESS - if successful
 */
fsp_err_t wifi_init(void);

/** @brief Sets the wifi data for connection.
 * @return FSP_SUCCESS - if successful
 */
fsp_err_t wifi_set(void);

/** @brief Writes serial data to DA16200
 * @param p_src - pointer to source bytes.
 * @param len - length of data to send (number of bytes)
 */
void wifi_serial_write(uint8_t * p_src, uint16_t len);

/** @brief Reads serial data from DA16200 - upto maximum number of bytes passed.
 * @param p_dst - pointer to buffer to store rx bytes.
 * @param p_num_bytes - pointer to variable containing maximum number of bytes to read (returns number read).
 * @param timeout_ms - number of milliseconds to wait before considering response received.
 * @return FSP_SUCCESS - if maximum at least *p_num_bytes number of bytes have been read, FSP_ERR_TIMEOUT otherwise
 * check *p_num_bytes for actual bytes read count.
 */
fsp_err_t wifi_serial_read(uint8_t * p_dst, uint16_t * p_num_bytes, uint16_t timeout_ms);

/** @brief Checks if string is present in another string.
 * @param p_resp_str - expected response string.
 * @param p_search_str - string to search.
 * @return 1 if successful
 */
uint8_t is_str_present(const char * p_resp_str, const char * p_search_str);

#endif /* DA16200_AT_H_ */
