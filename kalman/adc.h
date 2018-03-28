/*************************************
 * File: adc.h                       *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#ifndef ADC_H_
#define ADC_H_

#include "main.h"

void vInitADC(void);

void vStartADC(void);

unsigned int iReadADC(void);

void ADC14_IRQHandler(void);

#endif /* ADC_H_ */
