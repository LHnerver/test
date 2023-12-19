/*
 * linflexd_uart.h
 *
 *  Created on: Mar 2, 2016
 *      Author: B55457
 */

#ifndef LINFLEXD_UART_H_
#define LINFLEXD_UART_H_

#include "derivative.h"
#include "typedefs.h"
#include "pin.h"

void Init_LINFlexD1 ( unsigned int MegaHertz, unsigned int BaudRate );
void uart1_send(SCI_MSG* msg);

void Init_LINFlexD3 ( unsigned int MegaHertz, unsigned int BaudRate );
void UART3_Send(SCI_MSG* msg);

void Init_LINFlexD4 ( unsigned int MegaHertz, unsigned int BaudRate );
void UART4_Send(SCI_MSG* msg);

void Init_LINFlexD5 ( unsigned int MegaHertz, unsigned int BaudRate );
void UART5_Send(SCI_MSG* msg);

void Init_LINFlexD6 ( unsigned int MegaHertz, unsigned int BaudRate );
void UART6_Send(SCI_MSG* msg);

void Init_LINFlexD7 ( unsigned int MegaHertz, unsigned int BaudRate );
void UART7_Send(SCI_MSG* msg);

void Init_LINFlexD10 ( unsigned int MegaHertz, unsigned int BaudRate );

void UART10_Send(SCI_MSG* msg);
void UART10_SendByte(unsigned char data);


void UART1_SendByte(unsigned char data);
#endif /* LINFLEXD_UART_H_ */
