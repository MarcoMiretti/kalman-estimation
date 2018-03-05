#ifndef INIT
#define INIT
#include "msp.h"
#include "stdint.h"
#include "usci.h"
#endif

#define TRUE    1
#define FALSE   0
#define ERROR   -1

// Ubuntu app => sudo gtkterm
// Port => ttyACM0

/**
 * main.c
 */
void vInitUSCI(void);
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

    // Configure ADC pins
    P6->SEL1 |= BIT0;                       // Set P6.0 for ADC
    P6->SEL0 |= BIT0;

    vInitUSCI();

    // Configure ADC
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 |
            ADC14_CTL0_SHP | ADC14_CTL0_ON; // Sampling time, S&H=16, ADC14 on
    ADC14->CTL1 = ADC14_CTL1_RES_2;         // Use sampling timer, 12-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;  // A15 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conversion complete interrupt

    // Enable global interrupt
    __enable_irq();

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

    // Say "HI"
    vSendByte('H');
    vSendByte('I');

    while (1)
    {
        for (i = 0xFFFF; i > 0; i--);       // Delay

        P2->OUT ^= BIT0;                    // Blink P2.0 LED

        // Start sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    }
}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    if (ADC14->MEM[0] >= 0x7FF)             // ADC14MEM0 = A15 > 0.5AVcc?
      P1->OUT |= BIT0;                      // P1.0 = 1
    else
      P1->OUT &= ~BIT0;                     // P1.0 = 0
}
