/*************************************
 * File: usci.h                      *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#ifndef USCI_H_
#define USCI_H_

#include "main.h"

// USART Debug Interface
void vInitDebugUSCI(void);

void vSendDebugByte(unsigned char cByte);

unsigned char cReceiveDebugByte(void);

void vSendStringDebugUSART(const char *cString);

void EUSCIA0_IRQHandler(void);

// USART Interface
void vInitUSCI(void);

void vSendByte(unsigned char cByte);

unsigned char cReceiveByte(void);

void vSendStringUSART(const char *cString);

void EUSCIA2_IRQHandler(void);

#endif /* USCI_H_ */
