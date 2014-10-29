/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#include "packet/packet.h"

#define REGULATOR _LATA9
#define LED_RED _LATA4
#define LED_BLUE _LATA8
#define SENDER_FLAG IFS0bits.OC1IF
#define PARSER_FLAG IFS0bits.OC2IF
//#define DEAD_RECKONING_FLAG IFS3bits.RTCIF

typedef struct Buffer {
    int16_t infrared[NUMBER_INFRARED];
    int16_t hymidity;
    int16_t temperature;
    int16_t voltage;
    int16_t current;
} Buffer_t;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */
void update_parameter(void);
void update_autosend(void);
int send_data(void);
void update_parameter(void);
int ProcessADCSamples(Buffer_t* AdcBuffer);