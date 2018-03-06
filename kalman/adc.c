#ifndef INIT
#define INIT
#include "msp.h"
#include "stdint.h"
#endif

// ADC Initialization
void vInitADC(void)
{
    // Configure ADC pins
    P6->SEL1 |= BIT0;                       // Set P6.0 for ADC
    P6->SEL0 |= BIT0;

    // Configure ADC
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 |
            ADC14_CTL0_SHP | ADC14_CTL0_ON; // Sampling time, S&H=16, ADC14 on
    ADC14->CTL1 = ADC14_CTL1_RES_2;         // Use sampling timer, 12-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;  // A15 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conversion complete interrupt

}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    if (ADC14->MEM[0] >= 0x7FF)             // ADC14MEM0 = A15 > 0.5AVcc?
      P1->OUT |= BIT0;                      // P1.0 = 1
    else
      P1->OUT &= ~BIT0;                     // P1.0 = 0
}
