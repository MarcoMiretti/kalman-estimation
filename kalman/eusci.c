/*************************************
 * File: eusci.c                     *
 * Authors: GIDE - UTN San Francisco *
 * Project: Gimbal                   *
 *************************************/

#include "eusci.h"

extern char I2C_STATE, cIndexRX;
extern int8_t iData[MAX_RX_BYTES];
char cNumDataRX;

// eUSCI Initialization
void vInitEUSCI(void)
{
    // Configure I2C pins
    P6->SEL0 |= BIT4 | BIT5;                // Set 2-I2C pin as secondary function

    // Configure USCI_B1 for I2C mode
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_SWRST; // put eUSCI_B in reset state
    EUSCI_B1->CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain eUSCI_B in reset state
            EUSCI_B_CTLW0_MODE_3 |          // I2C mode
            EUSCI_B_CTLW0_MST |             // I2C master mode
            EUSCI_B_CTLW0_SYNC |            // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;      // SMCLK
    EUSCI_B1->BRW = 0x001E;                 // baud rate = SMCLK /30 = 100 KHz
    EUSCI_B1->I2CSA = MPU6050_ADDRESS;      // Slave address
    EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;// clear reset register
    EUSCI_B1->IE |= EUSCI_A_IE_RXIE |       // Enable receive interrupt
            EUSCI_B_IE_TXIE0 |              // Enable transmit interrupt
            EUSCI_B_IE_NACKIE;              // Enable NACK interrupt

    // Enable eUSCIB1 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB1_IRQn) & 31);

}

int iWriteByteI2C(unsigned char cAddress, unsigned char cData)
{
    unsigned char cWaitI2C;

    // Ensure stop condition got sent
    cWaitI2C = MAX_I2C_WAIT;
    while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP) && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;

    // I2C start condition
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR |   // I2C TX
            EUSCI_B_CTLW0_TXSTT;            // Start condition
                                            // Automatically send SLAVE+W

    cWaitI2C = MAX_I2C_WAIT;
    while (I2C_STATE != I2C_TX_INT && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;
    else
        I2C_STATE = I2C_OFF;

    // Send Address
    EUSCI_B1->TXBUF = cAddress;

    cWaitI2C = MAX_I2C_WAIT;
    while (I2C_STATE != I2C_TX_INT && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;
    else
        I2C_STATE = I2C_OFF;

    // Send data
    EUSCI_B1->TXBUF = cData;

    cWaitI2C = MAX_I2C_WAIT;
    while (I2C_STATE != I2C_TX_INT && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;
    else
        I2C_STATE = I2C_OFF;

    // I2C stop condition
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;

    return ACK;
}

int iReadBytesI2C(unsigned char cAddress, unsigned char cSize)
{
    unsigned char cWaitI2C;

    // Ensure stop condition got sent
    cWaitI2C = MAX_I2C_WAIT;
    while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP) && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;

    // Restart RX Index
    cIndexRX = FALSE;
    cNumDataRX = cSize;

    // I2C start condition
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR |   // I2C TX
            EUSCI_B_CTLW0_TXSTT;            // Start condition
                                            // Automatically send SLAVE+W

    cWaitI2C = MAX_I2C_WAIT;
    while (I2C_STATE != I2C_TX_INT && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;
    else
        I2C_STATE = I2C_OFF;

    // Send Address
    EUSCI_B1->TXBUF = cAddress;

    cWaitI2C = MAX_I2C_WAIT;
    while (I2C_STATE != I2C_TX_INT && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;
    else
        I2C_STATE = I2C_OFF;


    EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // I2C Receive mode

    // I2C start condition
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // Start condition
                                            // Automatically send SLAVE+W

    cWaitI2C = MAX_I2C_WAIT;
    while (I2C_STATE != I2C_RX_INT && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;

    cWaitI2C = MAX_I2C_WAIT;
    while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP) && --cWaitI2C);
    if (!cWaitI2C)
        return ERROR;
    iData[cIndexRX++] = EUSCI_B1->RXBUF;

    return ACK;
}

int iInitMPU6050(void)
{
    // MPU6050_PWR_MGMT_1
    // 0x01 - PLL with X axis gyroscope reference | Temperature enabled | Sleep disabled
    if (iWriteByteI2C(MPU6050_PWR_MGMT_1, 0x01) == ERROR)
        return ERROR;

    // MPU6050_PWR_MGMT_2
    // 0x00 - Disabled standby mode
    if (iWriteByteI2C(MPU6050_PWR_MGMT_2, 0x00) == ERROR)
        return ERROR;

    // MPU6050_CONFIG
    // 0x03 - Set DLPF bandwidth to 42Hz
    if (iWriteByteI2C(MPU6050_CONFIG, 0x03) == ERROR)
        return ERROR;

    // MPU6050_SMPLRT_DIV
    // 0x00 Set sample rate -- 1khz / (1 + 4) = 200Hz
    if (iWriteByteI2C(MPU6050_SMPLRT_DIV, 0x00) == ERROR)
        return ERROR;

    // MPU6050_GYRO_CONFIG
    // 0x00 - Set gyroscope range ±250°/s
    if (iWriteByteI2C(MPU6050_GYRO_CONFIG, 0x00) == ERROR)
        return ERROR;

    // MPU6050_ACCEL_CONFIG:
    // 0x00 - Set accelerometer range ±2g
    if (iWriteByteI2C(MPU6050_ACCEL_CONFIG, 0x00) == ERROR)
        return ERROR;

    // MPU6050_PWR_MGMT_1
    // 0x01 - PLL with X axis gyroscope reference | Temperature enabled | Sleep disabled
    if (iWriteByteI2C(MPU6050_PWR_MGMT_1, 0x01) == ERROR)
        return ERROR;

    return ACK;
}

// I2C interrupt service routine
void EUSCIB1_IRQHandler(void)
{
    if (EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0)
    {
        I2C_STATE = I2C_TX_INT;
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG0;
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;
    }
    else if (EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0)
    {
        if (cIndexRX < cNumDataRX - 2)
            iData[cIndexRX++] = EUSCI_B1->RXBUF;
        else
        {
            if (I2C_STATE != I2C_RX_INT)
            {
                I2C_STATE = I2C_RX_INT;
                // I2C stop condition
                EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                iData[cIndexRX++] = EUSCI_B1->RXBUF;
            }
            EUSCI_B1->IFG &= ~EUSCI_B_IFG_RXIFG0;
            EUSCI_B1->IFG &= ~EUSCI_B_IFG_RXIFG;
            EUSCI_B1->IFG &= ~EUSCI_B_IFG_STPIFG;
        }
    }
    else if (EUSCI_B1->IFG & EUSCI_B_IFG_NACKIFG)
    {
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_NACKIFG;

        // I2C stop condition
        EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

        // Clear USCI_B1 TX int flag
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG0;
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_RXIFG0;
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_RXIFG;
        EUSCI_B1->IFG &= ~EUSCI_B_IFG_STPIFG;
    }
}
