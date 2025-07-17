#ifndef CPU_H
#define CPU_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include "Sitech_Json.h"

#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <wsserver/ws.h>

#include <errno.h>
#include <pthread.h>

#include <semaphore.h>
#include <stdbool.h>
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

#include <sys/time.h>
#include <time.h>
#include "modbus/modbus.h"
#include "Modbus_Master_MFM384.h"
#include <stdint.h>
#include "mqtt-c/mqtt.h"
#include "mqtt-c/mqtt.c"
#include "mqtt-c/mqtt_pal.h"
#include "mqtt-c/mqtt_pal.c"
#include "templates/posix_sockets.h"

#include "Parameter_CPU.h"
#include "function_system.h"
#include <time.h>
#include "/home/pi/GPS/gps.c"

#define DEBUG_OPEN_WEBSOCKET_ONLINE             1
#define DEBUG_SENDER_WEBSOCKET_ONLINE           1    

int LED_GPS   = 28;
int LED_CLOUD = 25;
int Buzzer    = 3;

int status_connect_server_app = 0;

typedef struct __attribute__((packed))
{
    char *nameClient;
    int ID;
}listDevice_t;
listDevice_t listDevice[2];

#define qt_packet                listDevice[0].ID
#define Cpu_packet               listDevice[1].ID

#define DEBUG_WEBSOCKET

uint32_t g_signal[MAX_SIDE] = { 0 };
uint32_t g_mapping[MAX_SIDE] = { 0 };
uint32_t g_countdown[MAX_SIDE] = { 0 };
uint32_t g_output_sensor[MAX_SIDE] = { 0 };

static bool is_flag_opened_websocket = false;

volatile bool is_flag_opend_weboscket_online = false;
volatile bool isconnected_websocket_local = false; 

char firmware_GW_version[1000] = "";

uint8_t tx_buffer[4096] = { 0 };
int counter_login_fail = 0;

volatile int keepAlliveCloud = 0;
volatile int keepAlliveCPU = 0;

volatile bool is_start_open_wss = false;
volatile bool is_begin_start_up = true;
volatile bool is_finish_start_up = true;
volatile bool is_get_details_startup = false;
volatile bool is_send_config = true;
volatile bool is_send_schedule = true;
volatile bool is_resend_cycle_to_app = false; // = true cho phép cpu gửi tất cả các thông tin time form, cycle, index lên app cũ
volatile bool is_first_send_cycle = true;
volatile bool is_get_finish_data_gps = false;

volatile bool is_register_success = false;
volatile bool is_login_success = false;
volatile bool is_connected_socket =false;
volatile bool isSoftConnected = false;

pthread_mutex_t mutex;
sem_t semaphore;

// Cấu hình test mutex và semaphore
pthread_mutex_t lock;
ev_timer timeout_watcher;

double timeout = 0.00001;
int timeout_count = 0;

ev_async async_watcher;
int async_count = 0;

struct ev_loop* loop2;

/*---------------------------------------------------------OPTION MAIN---------------------------------------------------------------------------------------*/
void copy_u8_array_to_u32_array(uint8_t *u8, uint32_t *u32, int size);
void copy_u32_array_to_u8_array(uint32_t *u32, uint8_t *u8, int size);
char *substr(char *s, int start, int end);
void mtfc_debug_print_all_cycle_form(type_cycle_lamp_form_t *dat);
void serial_get_buffer(void);
void get_data_finish(void);
void mtfc_log_update(feedback_log_t *dat);
void mtfc_log_update();
// #define DEBUG_MAIN
pthread_t thread_Mqtt;
void *Mqtt(void *threadArgs);

pthread_t thread_mqtt_reconnect_client_test;
void *mqtt_reconnect_client(void *threadArgs);

pthread_t thread_ProcessGPS; 
void *process_GPS(void *threadArgs);

pthread_t thread_send_parameter_to_Monitor;
void *send_parameter_to_Monitor(void *threadArgs);

pthread_t thread_Serial;
void *Serial(void *threadArgs);

pthread_t thread_send_data_to_all;
void *send_data_to_all(void *threadArgs);

pthread_t thread_Open_Websocket_Online;
void *Open_Websocket_Online(void *threadArgs);

pthread_t thread_send_Websocket_Online;
void *Register_Device_GateWay(void *threadArgs);
/*---------------------------------------------------------OPTION MAIN---------------------------------------------------------------------------------------*//*---------------------------------------------------------DEFINE CMD------------------------------------------------------------------------------------*/

/*---------------------------------------------------------DEFINE CMD------------------------------------------------------------------------------------*/
#define CMD_QT_TO_CPU_TIME_ACTIVE	                                            1
#define CMD_CPU_TO_QT_TIME_ACTIVE                                               2
#define CMD_QT_TO_CPU_SCHEDULE													3
#define CMD_CPU_TO_QT_SCHEDULE	                                                4
#define CMD_QT_TO_CPU_SOPHA														5
#define CMD_CPU_TO_QT_SOPHA						                                6
#define CMD_QT_TO_CPU_DASHBOARD					                                7
#define CMD_CPU_TO_QT_DASHBOADR					                                8
#define CMD_CPU_TO_MASTER_SOPHA					                                9
#define CMD_CPU_TO_MASTER_SOPHA_FB					                            10
#define CMD_CPU_TO_MASTER_TIME_ACTIVE					                        11
#define CMD_CPU_TO_MASTER_CYCLE_FORM					                        12
#define CMD_QT_TO_CPU_SET_RTC_MANUAL                                            13
#define CMD_QT_TO_CPU_MODE_AUTO_RTC                                             14
#define CMD_QT_TO_CPU_CONTROL_UPDATE_NTP_RTC                                    15
#define CMD_CPU_TO_MASTER_SET_RTC_MANUAL          		                        16
#define CMD_MASTER_TO_CPU_CARD_INFOR          		                        	17
#define CMD_CPU_TO_QT_CARD_INFOR		          		                       	18
#define CMD_MASTER_TO_CPU_MODE_ACTIVE	          		                       	19
#define CMD_MASTER_TO_CPU_SELECT_PHASE	          		                        20
#define CMD_CPU_TO_QT_MODE_ACTIVE	          		                      	 	21
#define CMD_CPU_TO_QT_SELECT_PHASE	          		                    	    22
#define CMD_QT_TO_CPU_MODE_CROSS	          		                    	    23
#define CMD_CPU_TO_MASTER_MODE_CROSS          		                    	    24
#define CMD_CPU_TO_QT_CARD_INFOR2	          		                    	    25
#define CMD_QT_TO_CPU_CARD_INFOR	          		                    	    26
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_1       		                    	    27
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_2        	                    	    28
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_3        	                    	    29
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_4        	                    	    30
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_5        	                    	    31
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_6        	                    	    32
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_7        	                    	    33
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_8        	                    	    34
#define CMD_QT_TO_CPU_SCHEDULE_ALL			        	                    	35
#define CMD_CPU_TO_MASTER_SCHEDULE			        	                    	36
#define CMD_MASTER_TO_CPU_SCHEDULE_TIME			        	                    37
#define CMD_MASTER_TO_CPU_SCHEDULE_DAYS			        	                    38
#define CMD_CPU_TO_QT_SCHEDULE_TIME				        	                    39
#define CMD_CPU_TO_QT_SCHEDULE_DAYS				        	                    40
#define CMD_QT_TO_CPU_SCHEDULE_TIME				        	                    41
#define CMD_CPU_TO_MASTER_SCHEDULE_TIME_UPDATE	        	                    42
#define CMD_MASTER_TO_CPU_SCHEDULE_CYCLE		        	                    43
#define CMD_CPU_TO_QT_SCHEDULE_CYCLE			        	                    44
#define CMD_QT_TO_CPU_SCHEDULE_CYCLE_UPDATE		        	                    45
#define CMD_CPU_TO_MASTER_SCHEDULE_CYCLE_UPDATE	        	                    46
#define CMD_QT_TO_CPU_SCHEDULE_DAYS_UPDATE		        	                    47
#define CMD_CPU_TO_MASTER_SCHEDULE_DAYS_UPDATE	        	                    48
#define CMD_QT_TO_CPU_TIME_SETTING				        	                    49
#define CMD_CPU_TO_MASTER_TIME_SETTING			        	                    50
#define CMD_MASTER_TO_CPU_CYCLE_NUMBER			        	                    51
#define CMD_MASTER_TO_CPU_CHECK_CARD			        	                    52
#define CMD_CPU_TO_QT_STATUS					        	                    53
#define CMD_MASTER_TO_CPU_CURRENT                   							54
#define CMD_CPU_TO_QT_CURRENT                  		 							55
#define CMD_MASTER_TO_CPU_FB                  		 							56
#define CMD_CPU_TO_QT_FB	                  		 							57
#define CMD_MASTER_TO_CPU_FB_DAY_FORM              		 						58
#define CMD_MASTER_TO_CPU_FB_TIME_FORM            		 						59
#define CMD_MASTER_TO_CPU_FB_CYCLE_FORM              		 					60
#define CMD_QT_TO_CPU_OPEN_SERVER              		 							61
#define CMD_CPU_TO_QT_OPEN_SERVER              		 							62
#define CMD_QT_TO_CPU_OFF_SCREEN    											63
#define CMD_QT_TO_CPU_ON_SCREEN    								     			64
#define CMD_QT_TO_CPU_DKTB	    								     			65
#define CMD_MASTER_TO_CPU_PHASE    								     			66
#define CMD_MASTER_TO_CPU_FB_CARD_CONFIG						     			67
#define CMD_CPU_TO_MASTER_START									     			68
#define CMD_CPU_TO_MASTER_MANUAL								     			69
#define CMD_CPU_TO_MASTER_POWER									     			70

/* custom by thangnm*/
#define CMD_CPU_TO_QT_STATUS_CONNECT_MQTT				        	            101
#define CMD_QT_CPU_RE_GET_GPS                                                   129

// Goi tin lam viec gui dinh ki 1s
#define CMD_MASTER_TO_CPU_ONE_SECOND_MESSAGE                                     0

// Goi tin lam viec gui lich lam viec tu master
#define CMD_MASTER_TO_CPU_ALL_SCHEDULE                                          200

// Goi tin lam viec gui lic lam viec tu app
#define CMD_CPU_TO_MASTER_SCHEDULE_CYCLE			        	                44

#define ADDR_INPUT_SOFTWARE_APP_CONTROL_DEFAULT                                 1
#define ADDR_INPUT_SOFTWARE_APP_CONTROL_T1                                      9
#define ADDR_INPUT_SOFTWARE_APP_CONTROL_T2                                      17
#define ADDR_INPUT_SOFTWARE_APP_CONTROL_T3                                      33
#define ADDR_INPUT_SOFTWARE_APP_CONTROL_T4                                      65
// #define CMD_CPU_STATU_AUTO_CONTROL                                              10
// #define CND_QT_CALL_SCHEDULE_LOAD_EDIT                                          11
// #define CMD_QT_TO_CPU_MODE_AUTO_RTC                                             12
// #define CMD_QT_CONTROL_UPDATE_NTP_RTC                                           13
// #define CMD_QT_SET_RTC_MANUAL                                                   14

// #define CMD_APP_CONTROL_ON_OFF                                                  15
// #define CMD_APP_RELEASE_CONTROL                                                 16

#endif