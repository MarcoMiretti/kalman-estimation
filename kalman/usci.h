/*
 * usci.h
 *
 *  Created on: 5 mar. 2018
 *      Author: emanuel
 */

#ifndef USCI_H_
#define USCI_H_

void vInitUSCI(void);

void vSendByte(unsigned char cByte);

unsigned char cReceiveByte(void);

void vSendStringUSART(const char *cString);

void EUSCIA0_IRQHandler(void);




#endif /* USCI_H_ */
