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

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system/system.h"        /* System funct/params, like osc/peripheral config */
#include "system/user.h"          /* User funct/params, such as InitApp              */
#include "communication/parsing_packet.h"
#include "communication/serial.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

int16_t main(void) {

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize hashmap packet */
    init_hashmap();
    /* Initialize variables for process */
    init_process();
    /* Initialize variables for robots */
    init_buff_serial_error();

    /* Initialize IO ports and peripherals */
    InitApp();

    /* Initialize parameter sensor */
    update_parameter();

    while (1) {

    }
}
