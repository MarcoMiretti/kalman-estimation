/*************************************
 * File: ftoa.h                      *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#ifndef FTOA_H_
#define FTOA_H_

#include "main.h"

void reverse(char *str, int len);

int intToStr(int x, char str[], int d);

void ftoa(float n, char *res, int afterpoint);

#endif /* FTOA_H_ */
