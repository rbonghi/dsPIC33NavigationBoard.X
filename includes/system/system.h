/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* Interrupt priority */
/* Max priority 7 - Min priority 1 */
#define UART_RX_LEVEL 6
#define SYS_TIMER_LEVEL 5
#define UART_TX_LEVEL 1

#define EVENT_PRIORITY_HIGH_LEVEL 4
#define EVENT_PRIORITY_MEDIUM_LEVEL 3
#define EVENT_PRIORITY_LOW_LEVEL 2
#define EVENT_PRIORITY_VERY_LOW_LEVEL 1

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        80000000
#define FCY             SYS_FREQ/2

//Timer 3 is setup to time-out every 125 microseconds (8Khz Rate)
#define FRTMR3 8000
#define TCTMR3 0.000125            // Timer1 - Value in second [s]
#define TMR3_VALUE TCTMR3*FCY   // Timer1 - Value in CLK

//Blink led
#define BLINK_LED 0.5 //in secondi
#define BLINKSW (int)(BLINK_LED/TCTMR3)
//Repeat sender
#define RP_SENDER 0.1 //in secondi
#define SENDERSW (int)(RP_SENDER/TCTMR3)
//Time to stop
#define STOP_SENDER 2 //in secondi
#define SENDER_STOP_SW (int)(STOP_SENDER/TCTMR3)

//UART
#define BAUDRATE 115200
//#define BAUDRATE 57600
#define BRGVAL   ((FCY/BAUDRATE)/16)-1

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/**
 * Handles clock switching/osc initialization
 */
void ConfigureOscillator(void);
/**
 * Initialization all system events
 */
void InitEvents(void);
/**
 * Initialization Timer 3
 * Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). 
 * As a result, the module will stop sampling and trigger a conversion on 
 * every Timer3 time-out, i.e., Ts=125us.
 */
void InitTimer3(void); 

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

////void init_process(void);
////process_buffer_t decodeNameProcess(int number);
//unsigned char update_priority(void);
//unsigned char update_frequency(void);
////services_t services(services_t service);
