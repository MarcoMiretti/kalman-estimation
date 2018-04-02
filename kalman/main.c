/*************************************
 * File: main.c                      *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

// PWM Motor 1
// P2.4 - TA0.1
// P2.6 - TA0.3
// P2.7 - TA0.4

// PWM Motor 2
// P5.6 - TA2.1
// P6.6 - TA2.3
// P6.7 - TA2.4

#include "main.h"

char TIMER_STATE = FALSE, I2C_STATE = I2C_OFF, cIndexRX, cData[10];
int8_t iData[MAX_RX_BYTES];

// Main loop
void main(void)
{
    int iTmp, count = 0;
    float fTmp;

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

    // Timer32 set up in periodic mode, 32-bit, no pre-scale
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE |
            TIMER32_CONTROL_MODE;

    // Load Timer32 counter with period = 0x2DC6C0 (four times per second)
    //                           period = 0xB71B00 (one time per second)
    TIMER32_1->LOAD = 0xB71B00;

    // Enable the Timer32 interrupt in NVIC
    NVIC->ISER[0] = 1 << ((T32_INT1_IRQn) & 31);

    // Start Timer32 with interrupt enabled
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE |
            TIMER32_CONTROL_IE;

    vInitUSCI();                            // Calls USCI Initialization routine
    vInitDebugUSCI();                       // Calls USCI Initialization routine
    vSendStringDebugUSART("USART initialized...\r\n");

    vInitADC();                             // Calls ADC Initialization routine
    vSendStringDebugUSART("ADC initialized...\r\n");

    vInitEUSCI();                           // Calls eUSCI Initialization routine
    vSendStringDebugUSART("I2C initialized...\r\n");

    // Enable global interrupt
    __enable_irq();

    if(iInitMPU6050() == ERROR)
        vSendStringDebugUSART("Error initializing MPU6050\r\n");
    else
        vSendStringDebugUSART("MPU6050 initialized...\r\n");

    // Labels
    vSendStringUSART("Time, Temp, AX, AY, AZ, GX, GY, GZ\r\n");

    while (TRUE)
    {
        if (TIMER_STATE)
        {
            TIMER_STATE = FALSE;
            P2->OUT ^= BIT0;                    // Blink P2.0 LED

            count++;
            intToStr(count, cData, 4);
            vSendStringUSART(cData);

    //        vStartADC();
            vSendStringDebugUSART("Reading MPU6050\r\n");
            if (iReadBytesI2C(MPU6050_ACCEL_XOUT_H, 14) != ERROR)
            {
                fTmp = (iData[6]<<8) + iData[7];    // Temperature
                fTmp = fTmp/340.0f + 36.53f;
                ftoa(fTmp, cData, 2);
                vSendStringUSART(", ");
                vSendStringUSART(cData);

                fTmp = (iData[0]<<8) + iData[1];    // Acceleration X
                fTmp = fTmp/16384.f;
                ftoa(fTmp, cData, 3);
                vSendStringUSART(", ");
                vSendStringUSART(cData);

                fTmp = (iData[2]<<8) + iData[3];    // Acceleration Y
                fTmp = fTmp/16384.f;
                ftoa(fTmp, cData, 3);
                vSendStringUSART(", ");
                vSendStringUSART(cData);

                fTmp = (iData[4]<<8) + iData[5];    // Acceleration Z
                fTmp = fTmp/16384.f;
                ftoa(fTmp, cData, 3);
                vSendStringUSART(", ");
                vSendStringUSART(cData);

                iTmp = (iData[8]<<8) + iData[9];    // Gyroscope X
                iTmp = ~(iTmp - 1);
                fTmp = iTmp/131.f;
                fTmp -= 1.5464f;
                ftoa(fTmp, cData, 3);
                vSendStringUSART(", ");
                vSendStringUSART(cData);

                iTmp = (iData[10]<<8) + iData[11];  // Gyroscope Y
                iTmp = ~(iTmp - 1);
                fTmp = iTmp/131.f;
                fTmp -= 2.2377f;
                ftoa(fTmp, cData, 3);
                vSendStringUSART(", ");
                vSendStringUSART(cData);

                iTmp = (iData[12]<<8) + iData[13];  // Gyroscope Z
                iTmp = ~(iTmp - 1);
                fTmp = iTmp/131.f;
                fTmp += 0.7158f;
                ftoa(fTmp, cData, 3);
                vSendStringUSART(", ");
                vSendStringUSART(cData);
                vSendStringUSART("\r\n");
            }
            else
                vSendStringDebugUSART("Error reading MPU6050\r\n");
        }
    }
}

void T32_INT1_IRQHandler(void)
{
    TIMER32_1->INTCLR |= BIT0;          // Clear Timer32 interrupt flag
    P2->OUT ^= BIT1;                    // Blink P2.1 LED

    TIMER_STATE = TRUE;
}
