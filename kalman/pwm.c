/*************************************
 * File: pwm.c                       *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#include "pwm.h"

// PWM Motor 1 (PAN)
// P2.4 - IN3 (TA0.1)
// P2.6 - IN2 (TA0.3)
// P2.7 - IN1 (TA0.4)

// P1.6 - nFault
// P1.7 - nSleep
// P2.3 - nReset

// P2.5 - EN1
// P3.0 - EN2
// P5.7 - EN3

// PWM Motor 2 (TILT)
// P5.6 - IN1 (TA2.1)
// P6.6 - IN2 (TA2.3)
// P6.7 - IN3 (TA2.4)

// P3.5 - nFault
// P3.7 - nSleep
// P3.6 - nReset

// P5.0 - EN1
// P5.1 - EN2
// P5.2 - EN3

// PWM Initialization
void vInitPWM(void)
{
    // PAN
    P2->DIR |= BIT4 | BIT6 | BIT7;              // P2.4, P2.6~7 set TA0.1, TA0.3~4
    P2->SEL0 |= BIT4 | BIT6 | BIT7;
    P2->SEL1 &= ~(BIT4 | BIT6 | BIT7);

    P1->OUT &= ~BIT7;                           // Clear nSleep
    P1->DIR |= BIT7;                            // Set P1.7/nSleep to output
    P2->OUT &= ~BIT3;                           // Clear nReset
    P2->DIR |= BIT3;                            // Set P2.3/nReset to output

    P2->OUT &= ~BIT5;                           // Clear EN1
    P2->DIR |= BIT5;                            // Set P2.5/EN1 to output
    P3->OUT &= ~BIT0;                           // Clear EN2
    P3->DIR |= BIT0;                            // Set P3.0/EN2 to output
    P5->OUT &= ~BIT7;                           // Clear EN3
    P5->DIR |= BIT7;                            // Set P5.7/EN3 to output

    P3->DIR &= ~BIT5;                           // Set P3.5/nFault to input

    // TILT
    P5->DIR |= BIT6;                            // P5.6 set TA2.1
    P5->SEL0 |= BIT6;
    P5->SEL1 &= ~BIT6;
    P6->DIR |= BIT6 | BIT7;                     // P6.6~7 set TA2.3~4
    P6->SEL0 |= BIT6 | BIT7;
    P6->SEL1 &= ~(BIT6 | BIT7);

    P3->OUT &= ~BIT7;                           // Clear nSleep
    P3->DIR |= BIT7;                            // Set P3.7/nSleep to output
    P3->OUT &= ~BIT6;                           // Clear nReset
    P3->DIR |= BIT6;                            // Set P3.6/nReset to output

    P5->OUT &= ~BIT0;                           // Clear EN1
    P5->DIR |= BIT0;                            // Set P5.0/EN1 to output
    P5->OUT &= ~BIT1;                           // Clear EN2
    P5->DIR |= BIT1;                            // Set P5.1/EN2 to output
    P5->OUT &= ~BIT2;                           // Clear EN3
    P5->DIR |= BIT2;                            // Set P5.2/EN3 to output

    P3->DIR &= ~BIT5;                           // Set P3.5/nFault to input


    // Timer 0
    TIMER_A0->CCR[0] = 1000 - 1;                // PWM Period; PWM frequency = 12 kHz; SMCLK = 12 MHz;
                                                // PWM Period = (SMCLK/PWM Frequency) -1

    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set
    TIMER_A0->CCR[1] = 067;                     // CCR1 PWM duty cycle  6.7%

    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set
    TIMER_A0->CCR[3] = 933;                     // CCR2 PWM duty cycle  93.3%

    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set
    TIMER_A0->CCR[4] = 500;                     // CCR2 PWM duty cycle  50%

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |   // SMCLK
            TIMER_A_CTL_MC__UP |                // Up mode
            TIMER_A_CTL_CLR;                    // Clear TAR

    // Enable PAN (DRV8313)
    P1->OUT |= BIT7;                            // Set nSleep - P1.7
    P2->OUT |= BIT3;                            // Set nReset - P2.3
    P2->OUT |= BIT5;                            // Set EN1 - P2.5
    P3->OUT |= BIT0;                            // Set EN2 - P3.0
    P5->OUT |= BIT7;                            // Set EN3 - P5.7

    // Timer 2
    TIMER_A2->CCR[0] = 1000 - 1;                // PWM Period; PWM frequency = 12 kHz; SMCLK = 12 MHz;
                                                // PWM Period = (SMCLK/PWM Frequency) -1

    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 reset/set
    TIMER_A2->CCR[1] = 500;                     // CCR1 PWM duty cycle  50%

    TIMER_A2->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set
    TIMER_A2->CCR[3] = 933;                     // CCR2 PWM duty cycle  93.3%

    TIMER_A2->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7; // CCR2 reset/set
    TIMER_A2->CCR[4] = 067;                     // CCR2 PWM duty cycle  6.7%

    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK |   // SMCLK
            TIMER_A_CTL_MC__UP |                // Up mode
            TIMER_A_CTL_CLR;                    // Clear TAR

    // Enable TILT (DRV8313)
    P3->OUT |= BIT7;                            // Set nSleep - P3.7
    P3->OUT |= BIT6;                            // Set nReset - P3.6
    P5->OUT |= BIT0;                            // Set EN1 - P5.0
    P5->OUT |= BIT1;                            // Set EN2 - P5.1
    P5->OUT |= BIT2;                            // Set EN3 - P5.2
}

// Set duty PWM
void vSetDutyPWM(int iAngle)
{

    // Duty (%)
    //PWM A = (1+sen(theta))*50
    //PWM B = (1+sen(theta+120°))*50
    //PWM C = (1+sen(theta+240°))*50
    if (iAngle == 0)
    {
        // Pan (0 degrees)
        // P2.7
        TIMER_A0->CCR[4] = 500;                     // PWM A duty cycle  50%
        // P2.6
        TIMER_A0->CCR[3] = 933;                     // PWM B duty cycle  93.3%
        // P2.4
        TIMER_A0->CCR[1] = 67;                      // PWM C duty cycle  6.7%

        // Tilt (0 degrees)
        // P5.6
        TIMER_A2->CCR[1] = 500;                     // PWM A duty cycle  50%
        // P6.6
        TIMER_A2->CCR[3] = 933;                     // PWM B duty cycle  93.3%
        // P6.7
        TIMER_A2->CCR[4] = 67;                      // PWM C duty cycle  6.7%
    }
    else
    {
        // Pan (90 degrees)
        // P2.7
        TIMER_A0->CCR[4] = 1000;                     // PWM A duty cycle  100%
        // P2.6
        TIMER_A0->CCR[3] = 250;                     // PWM B duty cycle  25%
        // P2.4
        TIMER_A0->CCR[1] = 250;                     // PWM C duty cycle  25%

        // Tilt (90 degrees)
        // P5.6
        TIMER_A2->CCR[1] = 1000;                     // PWM A duty cycle  100%
        // P6.6
        TIMER_A2->CCR[3] = 250;                     // PWM B duty cycle  25%
        // P6.7
        TIMER_A2->CCR[4] = 250;                     // PWM C duty cycle  25%
    }
}
