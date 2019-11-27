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

gateway_data_status gateway_checkReceivedData(uint8_t uart_id){
	char uartread[6];
	uint8_t curr = COM_HEADER_SIZE;

	uint8_t n = uartReadMsg(0, uartread, 6);
	if(n){
		for(uint8_t i; i < COM_HEADER_SIZE; i++){
			if((uint8_t) uartread[i] != com_header[i])
				return wrongData;
		}

		if((uint8_t) uartread[curr++] != 0x01)
			return wrongData;

		if((uint8_t) uartread[curr] == 0x81)
			return sendDataOk;
		else if((uint8_t) uartread[curr] == 0xC1)
			return sendDataFail;
		else
			return wrongData;
	}
	else return noData;
}
