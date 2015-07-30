/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* Name interrupt */
#define SENDER_PRIORITY IPC0bits.OC1IP
#define RX_PARSER_PRIORITY IPC1bits.OC2IP
//#define DEAD_RECK_PRIORITY IPC15bits.RTCIP

/* Interrupt enable */
#define SENDER_ENABLE IEC0bits.OC1IE
#define RX_PARSER_ENABLE IEC0bits.OC2IE
//#define DEAD_RECK_ENABLE IEC3bits.RTCIE

/* Interrupt priority */
/* Max priority 7 - Min priority 1 */
#define UART_RX_LEVEL 6
#define SYS_TIMER_LEVEL 5
#define SENDER_LEVEL 4
#define UART_TX_LEVEL 1
#define RX_PARSER_LEVEL 1

// Define Message Buffer Length for ECAN1/ECAN2
// Highest Analog input number in Channel Scan
#define MAX_CHNUM 11
//#define LNG_BUFFER sizeof(Buffer_t)
#define LNG_BUFFER 256

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        80000000
#define FCY             SYS_FREQ/2

//Timer 3 is setup to time-out every 125 microseconds (8Khz Rate)
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

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void init_process(void);
//process_buffer_t decodeNameProcess(int number);
unsigned char update_priority(void);
unsigned char update_frequency(void);
//services_t services(services_t service);
void ConfigureOscillator(void); /* Handles clock switching/osc initialization */
void InitTimer3(void); /* Initialization Timer 3 - Timer system and ADC timer */
void InitInterrupts(void); /* Initalization others interrupts */
void InitDMA0(void); /* Initialization DMA0 for ADC current */
void InitADC(void); /* Initialization ADC for measure sensors*/
