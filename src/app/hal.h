/*
 * hal.h
 *
 *  Created on: 16 Jan 2023
 *      Author: a5126135
 */

#ifndef APP_HAL_H_
#define APP_HAL_H_

/** TODO: PORT: Make sure appropriate code generation tool headers are placed here*/
#include "r_smc_entry.h"
#include <stdbool.h>

/** @brief configuration macro for RX buffer size*/
#define HAL_UART_RX_BUFFER_SIZE	(512U)

/************************************************
 * 		API										*
 ***********************************************/

/** @brief starts/enables UART peripheral*/
void Hal_uart_start(void);

/** @brief stops/disables UART peripheral*/
void Hal_uart_stop(void);

/** @brief Start a UART transmission.
 * Ensure tx is complete by checking Hal_uart_tx_ready function.
 * @param ptr_bytes - pointer to bytes to send.
 * @param num_bytes - number of bytes to send.
 */
void Hal_uart_send(char * const ptr_bytes, const uint16_t num_bytes);

/** @brief Attempts to read the UART HAL buffers
 * @param ptr_bytes - pointer to buffer to copy receive data
 * @return number of bytes returned.
 */
uint16_t Hal_uart_read(char * const ptr_bytes);

/** @brief function to check if we have bytes available to read.
 @return true if available, false otherwise.*/
bool Hal_uart_rx_ready(void);

/** @brief function to check if we are ready to transmit.
 @return true if ready, false otherwise.*/
bool Hal_uart_tx_ready(void);

/** @brief start the oneshot timer
 * @param[in] ms - milliseconds to wait*/
void Hal_oneshot_start(uint16_t ms);

/** @brief checks if the oneshot has finished.
 * @return true if it has finished, false otherwise.
 */
bool Hal_oneshot_elapsed(void);

/************************************************
 * 		CALLBACKS								*
 ***********************************************/

/** @brief callback for tx done interrupt - place in ISR*/
void Hal_uart_tx_done_callback(void);

/** @brief callback for rx done interrupt - place in ISR and only call when set number of bytes have been received*/
void Hal_uart_rx_done_callback(void);

#endif /* APP_HAL_HAL_H_ */
