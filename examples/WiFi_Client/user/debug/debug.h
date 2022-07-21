/*
 * debug.h
 *
 *  Created on: Jul 20, 2022
 *      Author: Hashib
 */

#ifndef USER_DEBUG_DEBUG_H_
#define USER_DEBUG_DEBUG_H_

#include "string.h"
#include "driver/uart.h"

//set BIT_RATE_115200 for UART1 in uart_init
//#include "debug/debug.h"

#define DEBUG_UART_PRINT(msg)	do{										\
		ets_uart_printf(msg);	\
}while(0)

#define DEBUG_USART_PRINT_SEPARATOR     "=============================================\r\n"
#define DEBUG_USART_NEWLINE "\r\n"


#endif /* USER_DEBUG_DEBUG_H_ */
