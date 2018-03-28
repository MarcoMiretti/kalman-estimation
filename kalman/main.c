/*************************************
 * File: main.c                      *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#include "main.h"

char I2C_STATE = I2C_OFF, cIndexRX, cData[MAX_RX_BYTES], cTmpData[5];
float fTmp;

void main(void)
{
    int i, iTmp;

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
    vInitEUSCI();                           // Calls eUSCI Init routine

    // Enable global interrupt
    __enable_irq();


    // Say "HI"
    vSendByte('H');
    vSendByte('I');
    vSendByte(0x0D);

    i = iInitMPU6050();

    while (1)
    {
        for (i = 0xFFFF; i > 0; i--);       // Delay

        P2->OUT ^= BIT0;                    // Blink P2.0 LED

/*        vStartADC();

        if (iReadBytesI2C(MPU6050_WHO_AM_I, 1) == ERROR)
            vSendByte(0x05);
        else
            vSendByte(cData[0]);
*/
        if (iReadBytesI2C(MPU6050_TEMP_OUT_H, 2) == ERROR)
            vSendByte(0x05);
        else
        {
            vSendStringUSART("T: ");
            vSendByte(cData[0]);
            vSendByte(cData[1]);
        }

        if (iReadBytesI2C(MPU6050_ACCEL_XOUT_H, 6) == ERROR)
            vSendByte(0x05);
        else
        {
            iTmp = (cData[0]<<8) + cData[1];
            ltoa(iTmp, cTmpData);

            vSendStringUSART(" X: ");
            vSendStringUSART(cTmpData);

            iTmp = (cData[2]<<8) + cData[3];
            ltoa(iTmp, cTmpData);

            vSendStringUSART(" Y: ");
            vSendStringUSART(cTmpData);

            iTmp = (cData[4]<<8) + cData[5];
            ltoa(iTmp, cTmpData);

            vSendStringUSART(" Z: ");
            vSendStringUSART(cTmpData);
            vSendStringUSART("\r\n");
        }
    }
}
