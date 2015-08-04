/*
 * Copyright (C) 2015 Officine Robotiche
 * Author: Raffaello Bonghi
 * email:  raffaello.bonghi@officinerobotiche.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU Lesser General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif
#endif

#include <peripherals/led.h>

// Define Message Buffer Length for ECAN1/ECAN2
// Highest Analog input number in Channel Scan
#define MAX_CHNUM 11
//#define LNG_BUFFER sizeof(Buffer_t)
#define LNG_BUFFER 256

#define REGULATOR _LATA9
#define LED_RED _LATA4
#define LED_BLUE _LATA8

typedef struct Buffer {
    int16_t infrared[20];//NUMBER_INFRARED];
    int16_t hymidity;
    int16_t temperature;
    int16_t voltage;
    int16_t current;
} Buffer_t;

/*****************************************************************************/
/* Global Variable Declaration                                               */
/*****************************************************************************/

// Number of locations for ADC buffer = 14 (AN0 to AN13) x 8 = 112 words
// Align the buffer to 128 words or 256 bytes. This is needed for peripheral indirect mode

Buffer_t BufferA __attribute__((space(dma), aligned(LNG_BUFFER)));
Buffer_t BufferB __attribute__((space(dma), aligned(LNG_BUFFER)));

/*****************************************************************************/
/* User Functions                                                            */
/*****************************************************************************/

void InitADC(void) {
    AD1CON1bits.FORM = 0; // Data Output Format: Fraction (Q15 format)
    //AD1CON1bits.FORM   = 3;		// Data Output Format: Signed Fraction (Q15 format)
    //AD1CON1bits.FORM = 2; // Data Output Format: Fraction (Q15 format)
    AD1CON1bits.SSRC = 2; // Sample Clock Source: GP Timer starts conversion
    AD1CON1bits.ASAM = 1; // ADC Sample Control: Sampling begins immediately after conversion
    AD1CON1bits.AD12B = 0; // 10-bit ADC operation

    AD1CON2bits.CSCNA = 1; // Scan Input Selections for CH0+ during Sample A bit
    AD1CON2bits.CHPS = 0; // Converts CH0

    AD1CON3bits.ADRC = 0; // ADC Clock is derived from Systems Clock
    AD1CON3bits.ADCS = 63; // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
    // ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us

    AD1CON1bits.ADDMABM = 0; // DMA buffers are built in scatter/gather mode
    AD1CON2bits.SMPI = (MAX_CHNUM - 1); // 11 ADC Channel is scanned
    AD1CON4bits.DMABL = 0; // Each buffer contains 8 words

    //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    AD1CSSLbits.CSS0 = 1; // Enable AN0 for channel scan
    AD1CSSLbits.CSS1 = 1; // Enable AN1 for channel scan
    AD1CSSLbits.CSS2 = 1; // Enable AN2 for channel scan
    AD1CSSLbits.CSS3 = 1; // Enable AN3 for channel scan
    AD1CSSLbits.CSS4 = 1; // Enable AN4 for channel scan
    AD1CSSLbits.CSS5 = 1; // Enable AN5 for channel scan
    AD1CSSLbits.CSS6 = 1; // Enable AN6 for channel scan
    AD1CSSLbits.CSS7 = 1; // Enable AN7 for channel scan
    AD1CSSLbits.CSS8 = 1; // Enable AN8 for channel scan
    AD1CSSLbits.CSS9 = 1; // Enable AN9 for channel scan
    AD1CSSLbits.CSS10 = 1; // Enable AN10 for channel scan
    //AD1PCFGH/AD1PCFGL: Port Configuration Register
    AD1PCFGL = 0xFFFF;
    AD1PCFGLbits.PCFG0 = 0; // AN0 as Analog Input
    AD1PCFGLbits.PCFG1 = 0; // AN1 as Analog Input
    AD1PCFGLbits.PCFG2 = 0; // AN2 as Analog Input
    AD1PCFGLbits.PCFG3 = 0; // AN3 as Analog Input
    AD1PCFGLbits.PCFG4 = 0; // AN4 as Analog Input
    AD1PCFGLbits.PCFG5 = 0; // AN5 as Analog Input
    AD1PCFGLbits.PCFG6 = 0; // AN6 as Analog Input
    AD1PCFGLbits.PCFG7 = 0; // AN7 as Analog Input
    AD1PCFGLbits.PCFG8 = 0; // AN8 as Analog Input
    AD1PCFGLbits.PCFG9 = 0; // AN9 as Analog Input
    AD1PCFGLbits.PCFG10 = 0; // AN10 as Analog Input

    IFS0bits.AD1IF = 0; // Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 0; // Do Not Enable A/D interrupt
    AD1CON1bits.ADON = 1; // Turn on the A/D converter
}

/**
 * Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM
 * AMODE: Peripheral Indirect Addressing Mode
 * MODE: Continuous, Ping-Pong Mode
 * IRQ: ADC Interrupt
 */
void InitDMA0(void) {
    DMA0CONbits.AMODE = 2; // Configure DMA for Peripheral indirect mode
    DMA0CONbits.MODE = 2; // Configure DMA for Continuous Ping-Pong mode

    DMA0CNT = MAX_CHNUM - 1;
    DMA0REQ = 13; // Select ADC1 as DMA Request source

    DMA0STA = __builtin_dmaoffset(&BufferA);
    DMA0STB = __builtin_dmaoffset(&BufferB);
    DMA0PAD = (int) &ADC1BUF0;

    IFS0bits.DMA0IF = 0; //Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1; //Set the DMA interrupt enable bit

    DMA0CONbits.CHEN = 1; // Enable DMA

}

void Peripherals_Init(void) {
    
    // Unlock Registers  *****************************************
    asm volatile ( "mov #OSCCONL, w1 \n"
                "mov #0x45, w2 \n"
                "mov #0x57, w3 \n"
                "mov.b w2, [w1] \n"
                "mov.b w3, [w1] \n"
                "bclr OSCCON, #6 ");
    //**********************************************************//
    // Configure Input Functions
    //UART RX
    RPINR18bits.U1RXR = 25; // Assign U1RX To Pin RP25

    // Configure Output Functions
    //UART TX
    RPOR12bits.RP24R = 3; // Assign U1Tx To Pin RP24

    //************************************************************
    // Lock Registers ********************************************
    asm volatile ( "mov #OSCCONL, w1 \n"
                "mov #0x45, w2 \n"
                "mov #0x57, w3 \n"
                "mov.b w2, [w1] \n"
                "mov.b w3, [w1] \n"
                "bset OSCCON, #6");
    //**********************************************************//?

    /* Setup analog functionality and port direction */
    _TRISA9 = 0; //Regulator
    _TRISA4 = 0; //Led RED
    _TRISA8 = 0; //Led BLUE
    _TRISC8 = 0;
    _TRISC9 = 1;
    /* Initialize peripherals */
    LED_RED = 0;
    LED_BLUE = 0;
    REGULATOR = 0;
    
    InitADC();
    InitDMA0();
}