#ifndef INIT
#define INIT
#include "msp.h"
#include "stdint.h"
#endif

// USCI Initialization
void vInitUSCI(void)
{
    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                // Set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78;                     // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
}

// USART Transmission
void vSendByte(unsigned char cByte)
{
    // Check if the TX buffer is empty first
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

    // Put data into buffer
    EUSCI_A0->TXBUF = cByte;
}

// USART Reception
unsigned char cReceiveByte(void)
{
    // Wait for byte to be received
    while(!EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG);

    // Get and return received data
    return EUSCI_A0->RXBUF;
}

// USART String Transmission
void vSendStringUSART(const char *cString)
{
    while (*cString)
        vSendByte(*cString++);
}

// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        // Check if the TX buffer is empty first
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

        // Echo the received character back
        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

        P2->OUT ^= BIT1;                    // Blink P2.1 LED
    }
}
