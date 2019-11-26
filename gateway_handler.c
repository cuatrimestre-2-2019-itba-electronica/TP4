/*
 * gateway_handler.c
 *
 *  Created on: Nov 22, 2019
 *      Author: arthur
 */


#include "gateway_handler.h"

#define COM_HEADER_SIZE     4

static uint16_t com_header [] = {0xAA, 0x55, 0xC3, 0x3C};


void gateway_sendData(uint8_t uart_id, uint16_t * num, uint8_t length){

	char uartwrite[12];
	uint8_t curr = COM_HEADER_SIZE;

	// Send header
	for (uint8_t i = 0; i < COM_HEADER_SIZE; i++) {
		uartwrite[i] = com_header[i];
	}

	// Send length
	uartwrite[curr++] = 0x07;

	// Send command
	uartwrite[curr++] = 0x01;

	// Send data
	for (uint8_t i = 0; i < length; i++) {
		uartwrite[curr++] = num[i] & 0xFF;
		uartwrite[curr++] = num[i] >> 8;
	}

	uartWriteMsg(uart_id, uartwrite, 12);
}
