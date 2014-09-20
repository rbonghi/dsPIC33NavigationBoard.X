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

#include "parsing_packet.h"
#include "decode_packet.h"
#include "serial.h"

#include "user.h"
#include "system.h"

static unsigned int hashmap_default[10];
static unsigned int hashmap_navigation[10];

//From System
extern parameter_system_t parameter_system;

// From Interrupt
extern unsigned int counter_stop;
extern volatile process_t time, priority, frequency;
extern process_buffer_t name_process_adc_sensor, name_process_sender;

// From serial
extern error_pkg_t serial_error;
extern packet_t receive_pkg;
extern char receive_header;

/******************************************************************************/
/* Parsing functions                                                          */
/******************************************************************************/

void init_hashmap() {
    INITIALIZE_HASHMAP_DEFAULT
    INITIALIZE_HASHMAP_NAVIGATION
}

void saveData(information_packet_t* list_send, size_t len, information_packet_t info) {
    abstract_packet_t send;
    if (info.type == HASHMAP_DEFAULT) {
        switch (info.command) {
            case SERVICES:
                send.services = services(info.packet.services);
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case PRIORITY_PROCESS:
                priority = info.packet.process;
                list_send[len] = createPacket(info.command, ACK, info.type, NULL);
            case FRQ_PROCESS:
                frequency = info.packet.process;
                list_send[len] = createPacket(info.command, ACK, info.type, NULL);
                break;
            case NAME_PROCESS:
                send.process_name = decodeNameProcess(info.packet.process_name.name);
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case TIME_PROCESS:
            case PARAMETER_SYSTEM:
            case ERROR_SERIAL:
                list_send[len] = createPacket(info.command, NACK, info.type, NULL);
                return;
            default:
                list_send[len] = createPacket(info.command, NACK, info.type, NULL);
                break;
        }
    } else saveOtherData(list_send, len, info);
}

void sendData(information_packet_t* list_send, size_t len, information_packet_t info) {
    abstract_packet_t send;
    if (info.type == HASHMAP_DEFAULT) {
        switch (info.command) {
            case SERVICES:
                send.services = services(info.packet.services);
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case PRIORITY_PROCESS:
                send.process = priority;
                list_send[len] = createDataPacket(info.command, info.type, &send);
            case FRQ_PROCESS:
                send.process = frequency;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case TIME_PROCESS:
                send.process = time;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case PARAMETER_SYSTEM:
                send.parameter_system = parameter_system;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case ERROR_SERIAL:
                send.error_pkg = serial_error;
                list_send[len] = createDataPacket(info.command, info.type, &send);
                break;
            case NAME_PROCESS:
            default:
                list_send[len] = createPacket(info.command, NACK, info.type, NULL);
                break;
        }
    } else sendOtherData(list_send, len, info);
}

int parse_packet() {
    int i;
    unsigned int t = TMR1; // Timing function
    information_packet_t list_data[10];
    unsigned int counter = 0;
    //Save single packet
    for (i = 0; i < receive_pkg.length;) {
        buffer_packet_u buffer_packet;
        memcpy(&buffer_packet.buffer, &receive_pkg.buffer[i], receive_pkg.buffer[i]);
        list_data[counter++] = buffer_packet.information_packet;
        i += receive_pkg.buffer[i];
    }
    //Compute packet
    for (i = 0; i < counter; ++i) {
        information_packet_t info = list_data[i];
        switch (info.option) {
            case DATA:
                saveData(&list_data[0], i, info);
                break;
            case REQUEST:
                sendData(&list_data[0], i, info);
                break;
        }
    }
    //Restart counter stop operation
    counter_stop = 0;
    //Send new packet
    packet_t send = encoder(&list_data[0], counter);
    if ((send.length != 0) && receive_header != HEADER_ASYNC)
        pkg_send(receive_header, send);
    return TMR1 - t; // Time of esecution
}

packet_t encoder(information_packet_t *list_send, size_t len) {
    int i;
    packet_t packet_send;
    packet_send.length = 0;
    for (i = 0; i < len; ++i) {
        buffer_packet_u buffer_packet;
        buffer_packet.information_packet = list_send[i];

        memcpy(&packet_send.buffer[packet_send.length], &buffer_packet.buffer, buffer_packet.information_packet.length);

        packet_send.length += buffer_packet.information_packet.length;
    }
    return packet_send;
}

packet_t encoderSingle(information_packet_t send) {
    packet_t packet_send;
    packet_send.length = send.length + 1;
    buffer_packet_u buffer_packet;
    buffer_packet.information_packet = send;
    memcpy(&packet_send.buffer, &buffer_packet.buffer, buffer_packet.information_packet.length + 1);
    return packet_send;
}

information_packet_t createPacket(unsigned char command, unsigned char option, unsigned char type, abstract_packet_t * packet) {
    information_packet_t information;
    information.command = command;
    information.option = option;
    information.type = type;
    if (option == DATA) {
        switch (type) {
            case HASHMAP_DEFAULT:
                information.length = LNG_HEAD_INFORMATION_PACKET + hashmap_default[command];
                break;
            case HASHMAP_NAVIGATION:
                information.length = LNG_HEAD_INFORMATION_PACKET + hashmap_navigation[command];
                break;
            default:
                //TODO trohw
                break;
        }
    } else {
        information.length = LNG_HEAD_INFORMATION_PACKET;
    }
    if (packet != NULL) {
        memcpy(&information.packet, packet, sizeof (abstract_packet_t));
    }
    return information;
}

information_packet_t createDataPacket(unsigned char command, unsigned char type, abstract_packet_t * packet) {
    return createPacket(command, DATA, type, packet);
}

//information_packet_t decode_single_pkg(Ptr_packet send, char command, unsigned char* Buffer, unsigned int position) {
//    information_packet_t packet;
//    Ptr_abstract_packet ptr_packet_send;
//    abstract_packet_t packet_send;
//    services_t service;
//    switch (command) {
//        case TIME_PROCESS:
//            return addPacket(send, command, NACK, NULL);
//            break;
//        case PRIORITY_PROCESS:
//            ptr_packet_send = (Ptr_abstract_packet) & priority;
//            packet = addChangePacket(send, command, Buffer, position, LNG_PROCESS, ptr_packet_send);
//            update_priority();
//            return packet;
//            break;
//        case FRQ_PROCESS:
//            ptr_packet_send = (Ptr_abstract_packet) & frequency;
//            packet = addChangePacket(send, command, Buffer, position, LNG_PROCESS, ptr_packet_send);
//            update_frequency();
//            return packet;
//            break;
//        case ERROR_SERIAL:
//            return addPacket(send, command, NACK, NULL);
//            break;
//        case PARAMETER_SYSTEM:
//            return addPacket(send, command, NACK, NULL);
//            break;
//        case PARAMETER_SENSOR:
//            ptr_packet_send = (Ptr_abstract_packet) & parameter_sensors;
//            packet = addChangePacket(send, command, Buffer, position, LNG_PARAMETER_SENSOR, ptr_packet_send);
////            update_parameter();
//            return packet;
//            break;
//        case SENSOR:
//        case HUMIDITY:
//        case INFRARED:
//            return addPacket(send, command, NACK, NULL);
//            break;
//        case ENABLE_AUTOSEND:
//            ptr_packet_send = (Ptr_abstract_packet) & autosend;
//            packet = addChangePacket(send, command, Buffer, position, LNG_AUTOSEND, ptr_packet_send);
//            update_autosend();
//            return packet;
//            break;
//        case ENABLE_SENSOR:
//            ptr_packet_send = (Ptr_abstract_packet) & enable_sensor;
//            return addChangePacket(send, command, Buffer, position, LNG_ENABLE, ptr_packet_send);
//            break;
//        case SERVICES:
//            //Save packet
//            ptr_packet_send = (Ptr_abstract_packet) & service;
//            addChangePacket(send, command, Buffer, position, LNG_SERVICES, ptr_packet_send);
//            //Esecution service
//            packet_send.services = services(service);
//            //Send return service packet
//            return buildRequestPacket(send, command, LNG_SERVICES, &packet_send);
//            break;
//        default:
//            pkg_error(ERROR_PKG);
//            return addPacket(send, command, NACK, NULL);
//            break;
//    }
//}
//
//information_packet_t addChangePacket(Ptr_packet send, char command, unsigned char* Buffer, unsigned int position, unsigned int length, Ptr_abstract_packet packet) {
//    if (packet != NULL) {
//        memcpy(packet->buffer, Buffer + (position * sizeof (unsigned char)), length);
//        return addPacket(send, command, ACK, NULL);
//    } else return addPacket(send, command, NACK, NULL);
//}
//
//information_packet_t addPacket(Ptr_packet send, unsigned char command, unsigned char option, Ptr_abstract_packet packet) {
//    switch (option) {
//        case REQUEST:
//        case CHANGE:
//            return addRequestPacket(send, command, packet);
//            break;
//        case ACK:
//        case NACK:
//            return addInformationPacket(send, command, option);
//            break;
//        default:
//            pkg_error(ERROR_OPTION);
//            return addPacket(send, command, NACK, NULL);
//            break;
//    }
//}
//
//information_packet_t addRequestPacket(Ptr_packet send, unsigned char command, Ptr_abstract_packet pkg) {
//    abstract_packet_t packet;
//    switch (command) {
//        case TIME_PROCESS:
//            packet.process = time;
//            return buildRequestPacket(send, command, LNG_PROCESS, &packet);
//            break;
//        case PRIORITY_PROCESS:
//            packet.process = priority;
//            return buildRequestPacket(send, command, LNG_PROCESS, &packet);
//            break;
//        case FRQ_PROCESS:
//            packet.process = frequency;
//            return buildRequestPacket(send, command, LNG_PROCESS, &packet);
//            break;
//        case ERROR_SERIAL:
//            packet.error_pkg = serial_error;
//            return buildRequestPacket(send, command, LNG_ERROR_PKG, &packet);
//            break;
//        case PARAMETER_SYSTEM:
//            packet.parameter_system = parameter_system;
//            return buildRequestPacket(send, command, LNG_PARAMETER_SYSTEM, &packet);
//            break;
//        case PARAMETER_SENSOR:
//            packet.parameter_sensor = parameter_sensors;
//            return buildRequestPacket(send, command, LNG_PARAMETER_SENSOR, &packet);
//            break;
//        case SENSOR:
//            packet.sensor = sensors;
//            return buildRequestPacket(send, command, LNG_SENSOR, &packet);
//            break;
//        case HUMIDITY:
//            packet.humidity = humidity;
//            return buildRequestPacket(send, command, LNG_HUMIDITY, &packet);
//            break;
//        case INFRARED:
//            packet.infrared = infrared;
//            return buildRequestPacket(send, command, LNG_INFRARED, &packet);
//            break;
//        case ENABLE_AUTOSEND:
//            packet.enable_autosend = autosend;
//            return buildRequestPacket(send, command, LNG_AUTOSEND, &packet);
//            break;
//        case ENABLE_SENSOR:
//            packet.enable_sensor = REGULATOR;
//            return buildRequestPacket(send, command, LNG_ENABLE, &packet);
//            break;
//        case SERVICES:
//            //TODO
//            return addPacket(send, command, NACK, NULL);
//            break;
//        default:
//            pkg_error(ERROR_CREATE_PKG);
//            return addPacket(send, command, NACK, NULL);
//            break;
//    }
//}
//
//information_packet_t addInformationPacket(Ptr_packet send, unsigned char command, unsigned char option) {
//    send->buffer[send->length] = 1;
//    send->buffer[send->length + 1] = command;
//    send->buffer[send->length + 2] = option;
//    send->length += 3;
//    information_packet_t option_pkg;
//    option_pkg.command = command;
//    option_pkg.option = option;
//    return option_pkg;
//}
//
//information_packet_t buildRequestPacket(Ptr_packet send, unsigned char command, const unsigned int length, Ptr_abstract_packet packet) {
//    if (packet != NULL) {
//        information_packet_t request_packet;
//        send->buffer[send->length] = length;
//        send->buffer[send->length + 1] = command;
//        memcpy(send->buffer + (send->length + 2) * sizeof (unsigned char), packet->buffer, length);
//        send->length += length + 2;
//        request_packet.command = command;
//        request_packet.option = REQUEST;
//        memcpy(&request_packet.packet.buffer, packet->buffer, length);
//        return request_packet;
//    } else {
//        return addInformationPacket(send, command, REQUEST);
//    }
//}