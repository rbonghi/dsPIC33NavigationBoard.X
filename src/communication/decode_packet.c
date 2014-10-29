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

#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include <string.h>

#include "communication/parsing_packet.h"

#include "communication/serial.h"
#include "system/user.h"
#include "system/system.h"

// From Interrupt
extern enable_sensor_t enable_sensor;

//From User
extern sensor_t sensors;
extern humidity_t humidity;
extern infrared_t infrared;
extern parameter_sensor_t parameter_sensors;
extern autosend_t autosend;

/******************************************************************************/
/* Computation functions                                                      */

/******************************************************************************/

void saveOtherData(information_packet_t* list_send, size_t len, information_packet_t info) {
    if (info.type == HASHMAP_NAVIGATION)
        switch (info.command) {
            case PARAMETER_SENSOR:
                parameter_sensors = info.packet.parameter_sensor;
                update_parameter();
                list_send[len] = createPacket(info.command, ACK, info.type, NULL);
                break;
            case ENABLE_AUTOSEND:
                autosend = info.packet.enable_autosend;
                update_autosend();
                list_send[len] = createPacket(info.command, ACK, info.type, NULL);
                break;
            case ENABLE_SENSOR:
                enable_sensor = info.packet.enable_sensor;
                list_send[len] = createPacket(info.command, ACK, info.type, NULL);
                break;
            case SENSOR:
            case INFRARED:
            case HUMIDITY:
                list_send[len] = createPacket(info.command, NACK, info.type, NULL);
                return;
            default:
                list_send[len] = createPacket(info.command, NACK, info.type, NULL);
                break;
        }
}

void sendOtherData(information_packet_t* list_send, size_t len, information_packet_t info) {
    abstract_packet_t send;
    if (info.type == HASHMAP_NAVIGATION)
        switch (info.command) {
            case SENSOR:
                send.sensor = sensors;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case INFRARED:
                send.infrared = infrared;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case HUMIDITY:
                send.humidity = humidity;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case PARAMETER_SENSOR:
                send.parameter_sensor = parameter_sensors;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case ENABLE_AUTOSEND:
                send.enable_autosend = autosend;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case ENABLE_SENSOR:
                send.enable_sensor = REGULATOR;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            default:
                list_send[len] = createPacket(info.command, NACK, info.type, NULL);
                break;
        }
}