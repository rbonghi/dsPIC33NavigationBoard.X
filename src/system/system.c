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


#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include <string.h>

#include "system/user.h"
#include "system/system.h"          /* variables/params used by system.c             */

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

unsigned int reset_count = 1;
unsigned char version_date_[] = __DATE__;
unsigned char version_time_[] = __TIME__;
unsigned char author_code[] = "Raffaello Bonghi";
unsigned char name_board[] = "Navigation Board";
unsigned char version_code[] = "v2.0";
//parameter_system_t parameter_system;

// From Interrupt
//extern volatile process_t time, priority, frequency;
//extern process_buffer_t name_process_adc_sensor, name_process_sender;

//extern unsigned char BufferTx[MAX_TX_BUFF] __attribute__((space(dma)));

// Number of locations for ADC buffer = 14 (AN0 to AN13) x 8 = 112 words
// Align the buffer to 128 words or 256 bytes. This is needed for peripheral indirect mode

Buffer_t BufferA __attribute__((space(dma), aligned(LNG_BUFFER)));
Buffer_t BufferB __attribute__((space(dma), aligned(LNG_BUFFER)));

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/


void InitEvents(void) {
    
}

/*=============================================================================
Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). As a result, the module
will stop sampling and trigger a conversion on every Timer3 time-out, i.e., Ts=125us.
=============================================================================*/
void InitTimer3() {
    T3CONbits.TON = 0; // Disable Timer
    T3CONbits.TSIDL = 1; // Stop in Idle Mode bit
    T3CONbits.TGATE = 0; // Disable Gated Timer mode
    T3CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    T3CONbits.TCS = 0; // Select internal clock source
    TMR3 = 0x0000;
    PR3 = TMR3_VALUE; // Trigger ADC1 every 125usec

    IPC2bits.T3IP = SYS_TIMER_LEVEL; // Set Timer 3 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer 3 interrupt
    IEC0bits.T3IE = 1; // Enable Timer 3 interrupt

    T3CONbits.TON = 1; //Start Timer 3
}


void init_process(void) {
//    name_process_adc_sensor.name = PROCESS_ADC_SENSOR;
//    strcpy(name_process_adc_sensor.buffer, ADC_SENSOR_STRING);
//    name_process_sender.name = PROCESS_SENDER;
//    strcpy(name_process_sender.buffer, SENDER_STRING);
//    //Parameter system
//    parameter_system.step_timer = (int) (TMR3_VALUE);
//    parameter_system.int_tm_mill = (int) (TCTMR3 * 1000);
//    //Basic information priority and frequency
//    priority.length = PROCESS_NAVIGATION_LENGTH;
//    priority.idle = 0;
//    priority.parse_packet = RX_PARSER_LEVEL;
//    priority.process[PROCESS_SENDER] = SENDER_LEVEL;
//    frequency.length = PROCESS_NAVIGATION_LENGTH;
//    frequency.idle = 0;
//    frequency.parse_packet = 0;
//    frequency.process[PROCESS_SENDER] = SENDERSW;
//    time.length = PROCESS_NAVIGATION_LENGTH;
}

//process_buffer_t decodeNameProcess(int number) {
//    process_buffer_t process;
//    switch (number) {
//        case -1:
//            process.name = PROCESS_NAVIGATION_LENGTH;
//            break;
//        case PROCESS_ADC_SENSOR:
//            process = name_process_adc_sensor;
//            break;
//        case PROCESS_SENDER:
//            process = name_process_sender;
//            break;
//    }
//    return process;
//}

unsigned char update_priority(void) {
//    priority.idle = 0;
//    InitInterrupts();
//    return ACK;
}

unsigned char update_frequency(void) {
//    frequency.idle = 0;
//    frequency.parse_packet = 0;
//    if (frequency.process[PROCESS_SENDER] == 0) {
//        SENDER_ENABLE = 0; // Disable Output Compare Channel 1 interrupt
//    } else
//        SENDER_ENABLE = 1; // Enable Output Compare Channel 1 interrupt
//    return ACK;
}

//services_t services(services_t service) {
//    services_t service_send;
//    service_send.command = service.command;
//    switch (service.command) {
//        case RESET:
//            if (reset_count < 3) {
//                reset_count++;
//            } else {
//                SET_CPU_IPL(7); // disable all user interrupts
//                //DelayN1ms(200);
//                asm("RESET");
//            }
//            break;
//        case DATE_CODE:
//            memcpy(service_send.buffer, version_date_, sizeof (version_date_));
//            service_send.buffer[sizeof (version_date_) - 1] = ' ';
//            memcpy(service_send.buffer + sizeof (version_date_), version_time_, sizeof (version_time_));
//            break;
//        case NAME_BOARD:
//            memcpy(service_send.buffer, name_board, sizeof (name_board));
//            break;
//        case VERSION_CODE:
//            memcpy(service_send.buffer, version_code, sizeof (version_code));
//            break;
//        case AUTHOR_CODE:
//            memcpy(service_send.buffer, author_code, sizeof (author_code));
//            break;
//        default:
//            break;
//    }
//    return service_send;
//}

void InitInterrupts(void) {
    //For ADC sensor
    SENDER_ENABLE = 0; // Disable Output Compare Channel 1 interrupt
//    SENDER_PRIORITY = priority.process[PROCESS_SENDER]; // Set Output Compare Channel 1 Priority Level
    IFS0bits.OC1IF = 0; // Clear Output Compare Channel 1 Interrupt Flag
    SENDER_ENABLE = 1; // Enable Output Compare Channel 1 interrupt

    //For Parsing UART message
    RX_PARSER_ENABLE = 0; // Disable Output Compare Channel 2 interrupt
    RX_PARSER_PRIORITY = RX_PARSER_LEVEL; //priority.parse_packet; // Set Output Compare Channel 2 Priority Level
    IFS0bits.OC2IF = 0; // Clear Output Compare Channel 2 Interrupt Flag
    RX_PARSER_ENABLE = 1; // Enable Output Compare Channel 2 interrupt

    //    // For dead reckoning
    //    DEAD_RECK_ENABLE = 0; // Disable RTC interrupt
    //    DEAD_RECK_PRIORITY = priority.dead_reckoning; // Set RTC Priority Level
    //    IFS3bits.RTCIF = 0; // Clear RTC Interrupt Flag
    //    DEAD_RECK_ENABLE = 1; // Enable RTC interrupt
}

void ConfigureOscillator(void) {
    PLLFBD = 30; // M=32  //Old configuration: PLLFBD=29 - M=31
    CLKDIVbits.PLLPOST = 0; // N1=2
    CLKDIVbits.PLLPRE = 0; // N2=2
    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;
    // Clock switching to incorporate PLL
    // Initiate Clock Switch to Primary
    __builtin_write_OSCCONH(0x03); // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(0x01); // Start clock switching
    while (OSCCONbits.COSC != 0b011); // Wait for Clock switch to occur
    while (OSCCONbits.LOCK != 1) {
    }; // Wait for PLL to lock
}

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

/*************************************************************
Funzione:	void initDma0(void)
Tipologia:	Init
Scopo:		DMA0 configuration
Note: Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM
          AMODE: Peripheral Indirect Addressing Mode
          MODE: Continuous, Ping-Pong Mode
          IRQ: ADC Interrupt
 *************************************************************/
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

