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

#include <system/events.h>
#include <system/task_manager.h>

#include "system/system.h"          /* variables/params used by system.c             */

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

unsigned char _VERSION_DATE[] = __DATE__;
unsigned char _VERSION_TIME[] = __TIME__;
unsigned char _VERSION_CODE[] = "v0.6";
unsigned char _AUTHOR_CODE[] = "Raffaello Bonghi";
unsigned char _BOARD_TYPE[] = "Navigation Board";
unsigned char _BOARD_NAME[] = "Navigation Board";

#define EVENT_PRIORITY_LOW_ENABLE IEC3bits.RTCIE
#define EVENT_PRIORITY_LOW_FLAG IFS3bits.RTCIF
#define EVENT_PRIORITY_LOW_P IPC15bits.RTCIP
hardware_bit_t RTCIF = REGISTER_INIT(IFS3, 14);

#define EVENT_PRIORITY_MEDIUM_ENABLE IEC0bits.OC1IE
#define EVENT_PRIORITY_MEDIUM_FLAG IFS0bits.OC1IF
#define EVENT_PRIORITY_MEDIUM_P IPC0bits.OC1IP
hardware_bit_t OC1IF = REGISTER_INIT(IFS0, 2);

#define EVENT_PRIORITY_HIGH_ENABLE IEC0bits.OC2IE
#define EVENT_PRIORITY_HIGH_FLAG IFS0bits.OC2IF
#define EVENT_PRIORITY_HIGH_P IPC1bits.OC2IP
hardware_bit_t OC2IF = REGISTER_INIT(IFS0, 6);

#define EVENT_PRIORITY_VERY_LOW_ENABLE IEC1bits.OC3IE
#define EVENT_PRIORITY_VERY_LOW_FLAG IFS1bits.OC3IF
#define EVENT_PRIORITY_VERY_LOW_P IPC6bits.OC3IP
hardware_bit_t OC3IF = REGISTER_INIT(IFS1, 9);


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

void InitEvents(void) {
    /// Register event controller
    init_events(&TMR1, &PR1);
    
    EVENT_PRIORITY_VERY_LOW_ENABLE = 0;
    EVENT_PRIORITY_VERY_LOW_P = EVENT_PRIORITY_VERY_LOW_LEVEL;
    register_interrupt(EVENT_PRIORITY_VERY_LOW, &RTCIF);
    EVENT_PRIORITY_VERY_LOW_ENABLE = 1;
    
    EVENT_PRIORITY_LOW_ENABLE = 0;
    EVENT_PRIORITY_LOW_P = EVENT_PRIORITY_LOW_LEVEL;
    register_interrupt(EVENT_PRIORITY_LOW, &RTCIF);
    EVENT_PRIORITY_LOW_ENABLE = 1;
    
    EVENT_PRIORITY_MEDIUM_ENABLE = 0;
    EVENT_PRIORITY_MEDIUM_P = EVENT_PRIORITY_MEDIUM_LEVEL;
    register_interrupt(EVENT_PRIORITY_MEDIUM, &OC1IF);
    EVENT_PRIORITY_MEDIUM_ENABLE = 1;
    
    EVENT_PRIORITY_HIGH_ENABLE = 0;
    EVENT_PRIORITY_HIGH_P = EVENT_PRIORITY_HIGH_LEVEL;
    register_interrupt(EVENT_PRIORITY_HIGH, &OC2IF);
    EVENT_PRIORITY_HIGH_ENABLE = 1;
    
    /// Initialization task controller
    task_init(FRTMR3);
}

void __attribute__((interrupt, auto_psv)) _T3Interrupt(void) {
    /// Execution task manager
    task_manager();
    IFS0bits.T3IF = 0; // Clear Timer 3 Interrupt Flag
//    REGULATOR = enable_sensor;
//    if (!(counter_send % frequency.process[PROCESS_SENDER]) && (enable_autosend == true)) {
//        SENDER_FLAG ^= 1;
//        counter_send = 0;
//    }
//    if (!(counter_led % BLINKSW)) {
//        LED_BLUE ^= 1;
//        counter_led = 0;
//    }
//    if (!((counter_stop + 1) % SENDER_STOP_SW)) {
//        //stop autosend
//        enable_autosend = false;
//        //disable regulator
//        enable_sensor = 0;
//        counter_stop = 0;
//    }
}

void __attribute__((interrupt, auto_psv)) _RTCCInterrupt(void) {
    event_manager(EVENT_PRIORITY_LOW);
    EVENT_PRIORITY_LOW_FLAG = 0; //interrupt flag reset
}

void __attribute__((interrupt, auto_psv)) _OC1Interrupt(void) {
    event_manager(EVENT_PRIORITY_MEDIUM);
    EVENT_PRIORITY_MEDIUM_FLAG = 0; // interrupt flag reset
}

void __attribute__((interrupt, auto_psv)) _OC2Interrupt(void) {
    event_manager(EVENT_PRIORITY_HIGH);
    EVENT_PRIORITY_HIGH_FLAG = 0; //interrupt flag reset
}

void __attribute__((interrupt, auto_psv)) _OC3Interrupt(void) {
    event_manager(EVENT_PRIORITY_VERY_LOW);
    EVENT_PRIORITY_VERY_LOW_FLAG = 0;
}

//******** OLD






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
