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

/******************************************************************************/
/* System Level Functions                                                     */
/******************************************************************************/

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

//******** OLD

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/

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
    return 'a';
}

unsigned char update_frequency(void) {
//    frequency.idle = 0;
//    frequency.parse_packet = 0;
//    if (frequency.process[PROCESS_SENDER] == 0) {
//        SENDER_ENABLE = 0; // Disable Output Compare Channel 1 interrupt
//    } else
//        SENDER_ENABLE = 1; // Enable Output Compare Channel 1 interrupt
//    return ACK;
    return 'a';
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
