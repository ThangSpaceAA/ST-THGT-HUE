#ifndef CONNECTAPP_H
#define CONNECTAPP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wsserver/ws.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <string.h>
#include <unistd.h>     /* for close() */
#include <errno.h>

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "../main_proto.pb-c.h"
#include "../device_proto.pb-c.h"
#include "../common_proto.pb-c.h"
#include "../user_proto.pb-c.h"

#include "uwsc/log.h"
#include "uwsc/uwsc.h"
#include "uwsc/ssl.h"
#include "uwsc/utils.h"
#include "uwsc/buffer.h"
#include "uwsc/config.h"

// #include "Sitech_Json.h"
// #include "Parameter_CPU.h"

#define MAX_SIDE1            8

typedef struct __attribute__((packed))
{
    char *nameClient;
    int ID;
}listDevice_t;
listDevice_t listDevice[2];

struct ev_signal signal_watch_app;

#define qt_packet                listDevice[0].ID
#define Cpu_packet               listDevice[1].ID


void onopen_ws(struct uwsc_client *cl);
void onclose_ws(struct uwsc_client *cl, int code, const char *reason);
void copy_u8_array_to_u32_array(uint8_t *u8, uint32_t *u32, int size);
void copy_u32_array_to_u8_array(uint32_t *u32, uint8_t *u8, int size);
void updateServer();
void updateServerSchedule(void);
char *substr(char *s, int start, int end);

pthread_t thread_Open_Websocket_Online;
void *open_Websocket_Online(void *threadArgs);

pthread_t thread_send_Websocket_Online;
void *Register_Device_GateWay(void *threadArgs);
#endif