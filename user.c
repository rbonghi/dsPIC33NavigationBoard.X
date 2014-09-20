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
#include "user.h"            /* variables/params used by user.c               */
#include "system.h"
#include "serial.h"
#include "parsing_packet.h"
#include "math.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
sensor_t sensors;
infrared_t infrared;
humidity_t humidity;
parameter_sensor_t parameter_sensors;
bool enable_autosend = false;
autosend_t autosend;

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
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

    InitADC(); //Open ADC for measure sensors
    InitDMA0(); //Open DMA0 for buffering measures ADC

    InitUART1(); //Open UART1 for serial comunication
    InitDMA1(); //Open DMA1 for Tx UART1

    InitTimer3(); //Open Timer1 for clock system
    InitInterrupts(); //Start others interrupts
}

void update_parameter() {
    parameter_sensors.exp_sharp = -1.11;
    parameter_sensors.gain_current = 1;
    parameter_sensors.gain_humidity = 1;
    parameter_sensors.gain_sharp = 12.33;
    parameter_sensors.gain_temperature = 1;
    parameter_sensors.gain_voltage = 1;
}

void update_autosend() {
    if (autosend.pkgs[0] != -1)
        enable_autosend = true;
    else
        enable_autosend = false;
}

int send_data() {
    unsigned int t = TMR3; // Timing function
    int i;
    information_packet_t list_data[BUFFER_AUTOSEND];
    int counter = 0;
    abstract_packet_t packet;
    for (i = 0; i < BUFFER_AUTOSEND; ++i) {
        if (autosend.pkgs[i] == -1)
            break;
        switch (autosend.pkgs[i]) {
            case INFRARED:
                packet.infrared = infrared;
                list_data[counter++] = createDataPacket(autosend.pkgs[i], HASHMAP_NAVIGATION, &packet);
                break;
            case SENSOR:
                packet.sensor = sensors;
                list_data[counter++] = createDataPacket(autosend.pkgs[i], HASHMAP_NAVIGATION, &packet);
                break;
            default:
                break;
        }
    }
    packet_t send = encoder(&list_data[0], counter);
    pkg_send(HEADER_ASYNC, send);
    return TMR3 - t; // Time of esecution
}

int ProcessADCSamples(Buffer_t* AdcBuffer) {
    unsigned int t = TMR3; // Timing function
    int i;
    //Convert adc value to distance
    for (i = 0; i < NUMBER_INFRARED; i++) {
        infrared.infrared[i] = parameter_sensors.gain_sharp * powf((3.3 / 1024) * AdcBuffer->infrared[i], parameter_sensors.exp_sharp);
    }
    //Convert other sensors
    humidity = (3.3 / 1024) * parameter_sensors.gain_humidity * AdcBuffer->hymidity;
    sensors.current = (3.3 / 1024) * parameter_sensors.gain_current * AdcBuffer->current;
    sensors.voltage = (3.3 / 1024) * parameter_sensors.gain_voltage * AdcBuffer->voltage;
    sensors.temperature = (3.3 / 1024) * parameter_sensors.gain_temperature * AdcBuffer->temperature;
    return TMR3 - t; // Time of esecution
}
