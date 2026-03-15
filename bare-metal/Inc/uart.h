/*
 * uart.h
 *
 *  Created on: 06-Mar-2026
 *      Author: HARSHA
 */

#ifndef UART_H_
#define UART_H_


#include<stdint.h>

void uart_init(uint32_t baudrate);
void uart_tx_byte(uint8_t data_byte);

#endif /* UART_IF_H_ */

