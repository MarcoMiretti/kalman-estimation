#include "msp.h"
#include "stdint.h"

/**
 * main.c
 */

void main(void)
{
    volatile unsigned int i;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop watchdog


	// TI example to test launchpad board
    // GPIO Setup
    P1->OUT &= ~BIT0;                       // Clear LED1 to start
    P1->DIR |= BIT0;                        // Set P1.0/LED1 to output
    P6->SEL1 |= BIT0;                       // Configure P6.0 for ADC
    P6->SEL0 |= BIT0;

    // Enable global interrupt
    __enable_irq();

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_2;         // Use sampling timer, 12-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;   // A1 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conversion complete interrupt

    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;   // Wake up on exit from ISR

    while (1)
    {
        for (i = 20000; i > 0; i--);        // Delay

        // Start sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        __sleep();

    }
}

// ADC15 interrupt service routine
void ADC15_IRQHandler(void) {
    if (ADC14->MEM[0] >= 0x7FF)             // ADC14MEM0 = A1 > 0.5AVcc?
      P1->OUT |= BIT0;                      // P1.0 = 1
    else
      P1->OUT &= ~BIT0;                     // P1.0 = 0
}
