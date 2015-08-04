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
#include "system/user.h"     /* variables/params used by user.c               */
#include "system/system.h"
#include "math.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
sensor_t sensors;
//infrared_t infrared;
//humidity_t humidity;
//parameter_sensor_t parameter_sensors;
//bool enable_autosend = false;
//autosend_t autosend;

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void update_parameter() {
//    parameter_sensors.exp_sharp = -1.11;
//    parameter_sensors.gain_current = 1;
//    parameter_sensors.gain_humidity = 1;
//    parameter_sensors.gain_sharp = 12.33;
//    parameter_sensors.gain_temperature = 1;
//    parameter_sensors.gain_voltage = 1;
}

void update_autosend() {
//    if (autosend.pkgs[0] != -1)
//        enable_autosend = true;
//    else
//        enable_autosend = false;
}

int send_data() {
    unsigned int t = TMR3; // Timing function
//    int i;
//    information_packet_t list_data[BUFFER_AUTOSEND];
//    int counter = 0;
//    abstract_packet_t packet;
//    for (i = 0; i < BUFFER_AUTOSEND; ++i) {
//        if (autosend.pkgs[i] == -1)
//            break;
//        switch (autosend.pkgs[i]) {
//            case INFRARED:
//                packet.infrared = infrared;
//                list_data[counter++] = createDataPacket(autosend.pkgs[i], HASHMAP_NAVIGATION, &packet);
//                break;
//            case SENSOR:
//                packet.sensor = sensors;
//                list_data[counter++] = createDataPacket(autosend.pkgs[i], HASHMAP_NAVIGATION, &packet);
//                break;
//            default:
//                break;
//        }
//    }
//    packet_t send = encoder(&list_data[0], counter);
    //pkg_send(HEADER_ASYNC, send);
    return TMR3 - t; // Time of esecution
}
