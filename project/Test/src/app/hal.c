/*
 * HAL.c
 *
 *  Created on: 16 Jan 2023
 *      Author: a5126135
 */

#include "hal.h"
#include "sensor.h"
/** @brief macro used to prepare for critical section*/
#define PREPARE_CRITICAL_SECTION() uint8_t l_int_status = __get_psw() >> 7U
/** @brief macro used to disable interrupts*/
#define ENTER_CRITICAL_SECTION()	{ __DI(); }
/** @brief macro used to enable interrupts*/
#define EXIT_CRITICAL_SECTION()	if(l_int_status == 1U) { __EI(); }

/** @brief Sets an event*/
#define HAL_SET_EVENT(flags,event)	{flags |= event;}
/** @brief Clears an event*/
#define HAL_CLEAR_EVENT(flags,event)	{flags &= ~event;}
/** @brief Utility MACRO used to check if all events are set.
 * @param[in] flags - variable containing flags.
 * @param[in] events - the events for checking.
 * @return true if the exact events are detected, false otherwise.*/
#define HAL_EXACT_EVENT_OCCURRED(flags,events)	( ((flags) & (events)) == events )
/** @brief Utility MACRO used to check if any event is set.
 * @param[in] flags - variable containing flags.
 * @param[in] events - the events for checking.
 * @return true if any events are detected, false otherwise.*/
#define HAL_ANY_EVENT_OCCURED(flags,events)	(((flags) & (events)) != 0U

/** @brief enumerated type for hal uart events*/
typedef enum
{
	NO_EVENT				=	0x0000U, /**< No events occurred*/
	RX_READY				=	0x0001U, /**< receive data ready*/
	TX_READY				=	0x0002U, /**< ready to transmit more data*/
	ALL_EVENTS				=	0x0003U, /**< All events*/
}hal_uart_event_t;

static hal_uart_event_t uart_events	=	NO_EVENT; /**< tracks uart events*/
static uint8_t rx_buf[HAL_UART_RX_BUFFER_SIZE] = {0U,};
static uint16_t rx_buffer_index = 0U;
bool oneshot_timer_done = false;

void Hal_uart_start(void)
{
	/* Start the UART peripheral*/
	R_Config_UARTA0_Start();

	/* Prepare peripheral for transmission*/
	HAL_SET_EVENT(uart_events, TX_READY);

	/* Start receive operation*/
	R_Config_UARTA0_Receive(rx_buf, 1U);
}
/* END OF FUNCTION*/

void Hal_uart_stop(void)
{
	/* Stop the UART peripheral*/
	R_Config_UARTA0_Stop();
}
/* END OF FUNCTION*/

void Hal_uart_send(char * const ptr_bytes, const uint16_t num_bytes)
{
	/* Clear the event and start sending data*/
	HAL_CLEAR_EVENT(uart_events, TX_READY);
	(void)R_Config_UARTA0_Send((uint8_t *) ptr_bytes, num_bytes);
}
/* END OF FUNCTION*/

uint16_t Hal_uart_read(char * const ptr_bytes)
{
	uint16_t ret_val = 0U;

	/* Disable interrupts while copying the buffer */
	PREPARE_CRITICAL_SECTION();

	ENTER_CRITICAL_SECTION();

	/* Copy buffer*/
	for(uint16_t copy_index = 0U; copy_index < rx_buffer_index; ++copy_index)
	{
		ptr_bytes[copy_index] = rx_buf[copy_index];
		++ret_val;
	}

	/* clear receive events & reset buffer index*/
	HAL_CLEAR_EVENT(uart_events, RX_READY);
	rx_buffer_index = 0U;

	/* Restart receive operation of the peripheral*/
	(void)R_Config_UARTA0_Receive(rx_buf, 1U);

	EXIT_CRITICAL_SECTION();

	return ret_val;
}
/* END OF FUNCTION*/

bool Hal_uart_rx_ready(void)
{
	return HAL_EXACT_EVENT_OCCURRED(uart_events, RX_READY);
}
/* END OF FUNCTION*/

bool Hal_uart_tx_ready(void)
{
	return HAL_EXACT_EVENT_OCCURRED(uart_events, TX_READY);
}
/* END OF FUNCTION*/

void Hal_oneshot_start(uint16_t ms)
{
	if(ms > 0U)
	{
		oneshot_timer_done = false;
		TDR00 = (ms > 1U) ? (ms << 2U)-1U : 0U;
		R_Config_TAU0_0_Start();
	}
}
/* END OF FUNCTION*/

bool Hal_oneshot_elapsed(void)
{
	return oneshot_timer_done;
}
/* END OF FUNCTION*/

/************************************************
 * 		CALLBACKS								*
 ***********************************************/

void Hal_uart_tx_done_callback(void)
{
	/* Upon finishing transmission - set the TX ready event*/
	HAL_SET_EVENT(uart_events, TX_READY);
}
/* END OF FUNCTION*/

void Hal_uart_rx_done_callback(void)
{
	/* Upon finishing reception - set the RX ready event*/
	HAL_SET_EVENT(uart_events, RX_READY);

	++rx_buffer_index;

	if(rx_buffer_index < (HAL_UART_RX_BUFFER_SIZE-1U))
	{
		(void)R_Config_UARTA0_Receive(&rx_buf[rx_buffer_index], 1U);
	}
}
/* END OF FUNCTION*/
