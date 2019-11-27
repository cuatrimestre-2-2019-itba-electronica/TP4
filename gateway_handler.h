/*
 * gateway_handler.h
 *
 *  Created on: Nov 22, 2019
 *      Author: arthur
 */

#ifndef GATEWAY_HANDLER_H_
#define GATEWAY_HANDLER_H_

#include "uart.h"

void gateway_sendData(uint8_t uart_id, uint16_t * num, uint8_t length);

#endif /* GATEWAY_HANDLER_H_ */
