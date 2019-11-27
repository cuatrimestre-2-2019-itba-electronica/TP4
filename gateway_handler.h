/*
 * gateway_handler.h
 *
 *  Created on: Nov 22, 2019
 *      Author: arthur
 */

#ifndef GATEWAY_HANDLER_H_
#define GATEWAY_HANDLER_H_

#include "uart.h"

typedef enum{noData, wrongData, sendDataOk, sendDataFail} gateway_data_status;

void gateway_sendData(uint8_t uart_id, uint16_t * num, uint8_t length);
gateway_data_status gateway_checkReceivedData(uint8_t uart_id);

#endif /* GATEWAY_HANDLER_H_ */
