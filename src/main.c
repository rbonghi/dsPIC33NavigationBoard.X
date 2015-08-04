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
#include "system/peripherals.h"
#include "communication/serial.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

int16_t main(void) {
    /** INITIALIZATION Operative System **/
    ConfigureOscillator();  ///< Configure the oscillator for the device
    Peripherals_Init();     ///< Initialize IO ports and peripherals
    
    InitEvents();   ///< Initialize processes controller
    InitTimer3();   ///< Open Timer1 for clock system

    /** SERIAL CONFIGURATION **/
    SerialComm_Init();  ///< Open UART1 for serial communication and Open DMA1 for TX UART1

//    /* Initialize parameter sensor */
//    update_parameter();

    while (true) {
        
    }

    return true;
}
