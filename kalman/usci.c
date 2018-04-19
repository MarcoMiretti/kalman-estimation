/*************************************
 * File: usci.c                      *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#include "usci.h"


/* USCI DEBUG Interface */
// USCI Initialization
void vInitDebugUSCI(void)
{
    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                // Set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation (57600)
    // 12000000/(16*57600) = 13.0208333
    // Fractional portion = 0.0208333
    // User's Guide Table 21-4: UCBRSx = 0x00
    //EUSCI_A0->BRW = 13;
    //EUSCI_A0->MCTLW = (0 << EUSCI_A_MCTLW_BRF_OFS) |
    //        EUSCI_A_MCTLW_OS16;

    // Baud Rate calculation (115200)
    // 12000000/(16*115200) = 6.51041
    // Fractional portion = 0.51041
    // User's Guide Table 21-4: UCBRSx = 0x08
    EUSCI_A0->BRW = 6;
    EUSCI_A0->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
}

// USART Transmission
void vSendDebugByte(unsigned char cByte)
{
    // Check if the TX buffer is empty first
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

    // Put data into buffer
    EUSCI_A0->TXBUF = cByte;
}

// USART Reception
unsigned char cReceiveDebugByte(void)
{
    // Wait for byte to be received
    while(!EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG);

    // Get and return received data
    return EUSCI_A0->RXBUF;
}

// USART String Transmission
void vSendStringDebugUSART(const char *cString)
{
    while (*cString)
        vSendDebugByte(*cString++);
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
    }
}

/* USCI Interface */
// USCI Initialization
void vInitUSCI(void)
{
    // Configure UART pins
    P3->SEL0 |= BIT2 | BIT3;                // Set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A2->BRW = 78;                     // 12000000/16/9600
    EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
}

// USART Transmission
void vSendByte(unsigned char cByte)
{
    // Check if the TX buffer is empty first
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));

    // Put data into buffer
    EUSCI_A2->TXBUF = cByte;
}

// USART Reception
unsigned char cReceiveByte(void)
{
    // Wait for byte to be received
    while(!EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG);

    // Get and return received data
    return EUSCI_A2->RXBUF;
}

// USART String Transmission
void vSendStringUSART(const char *cString)
{
    while (*cString)
        vSendByte(*cString++);
}

// UART interrupt service routine
void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        // Check if the TX buffer is empty first
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));

        // Echo the received character back
        EUSCI_A2->TXBUF = EUSCI_A2->RXBUF;

    }
}
