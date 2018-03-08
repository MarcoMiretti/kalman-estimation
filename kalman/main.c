/*************************************
 * File: main.c                      *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

// Ubuntu app => sudo gtkterm
// Port => ttyACMx

#include "main.h"

void main(void)
{
    volatile unsigned int i;

	WDT_A->CTL = WDT_A_CTL_PW
	        | WDT_A_CTL_HOLD;               // Stop watchdog timer

	// Clock setup
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

    // Configure GPIO
    P1->OUT &= ~BIT0;                       // Clear LED1 to start
    P1->DIR |= BIT0;                        // Set P1.0/LED1 to output
    P2->OUT &= ~(BIT0|BIT1|BIT2);           // Clear LED2 to start
    P2->DIR |= BIT0|BIT1|BIT2;              // Set P2.0-2/LED2 to output

    vInitUSCI();                            // Calls USCI Init routine
    vInitADC();                             // Calls ADC Init routine

    // Enable global interrupt
    __enable_irq();

    // Say "HI"
    vSendByte('H');
    vSendByte('I');

    while (1)
    {
        for (i = 0xFFFF; i > 0; i--);       // Delay

        P2->OUT ^= BIT0;                    // Blink P2.0 LED

        vStartADC();

        //vSendByte(iReadADC());
    }
}
