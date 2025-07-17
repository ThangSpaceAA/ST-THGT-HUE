#ifndef PARAMETER_CPU_H
#define PARAMETER_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define auto_Mode           4
#define manual_Mode         5
#define relay_1             0
#define relay_2             2
#define MAX_SIDE            8


#define GREEN_DEPENDENT  1
#define YELLOW_DEPENDENT 2
#define RED_DEPENDENT    3

#define TEMPLATE_MODE "CPU-{\"CMD\": %d,\
                            \"onoff\": %d}"

#define TEMPLATE_PHASE "CPU-{\"CMD\": %d,\
                            \"onoff2\": %d,\
                            \"phase2\": %d}"

#define TEMPLATE_STATUS_PWR "CPU-{\"CMD\": %d,\
                                  \"power\":%d}"
                                  
#define TEMPLATE_TIME_ACTIVE "CPU-{\"CMD\": %d,\
                             \"hh_start\": %d,\
                             \"mm_start\": %d,\
                             \"hh_end\": %d,\
                             \"mm_end\": %d}"
                             
#define TEMPLATE_SCHEDULE "CPU-{\"CMD\": %d,\
                             \"xanh1\": %d,\
                             \"xanh2\": %d,\
                             \"xanh3\": %d,\
                             \"xanh4\": %d,\
                             \"vang\": %d,\
                             \"giaitoa\": %d}"   

#define TEMPLATE_SOPHA "CPU-{\"CMD\": %d,\
                            \"sopha\": %d}"
                             
#define TEMPLATE_AUTO_CONTROL "CPU-{\"CMD\": %d,\
                            \"staus\": %s}"
                            
#define TEMPLATE_CARD_INFOR "CPU-{\"CMD\": %d,\
                             \"card\": %d,\
                             \"sn\": \"%s\",\
                             \"emei\": %d,\
                             \"phase\": %d,\
                             \"is_railway_enabled\": %d,\
                             \"is_walking_enabled\": %d,\
                             \"is_dependent_phase\": %d,\
                             \"time_delay_dependent_phase\": %d}"   

#define TEMPLATE_CURRENT_STATUS "CPU-{\"CMD\": %d,\
                             \"is_dependent_phase_1\": %d,\
                             \"is_dependent_phase_2\": %d,\
                             \"is_dependent_phase_3\": %d,\
                             \"is_dependent_phase_4\": %d,\
                             \"is_railway_enabled_1\": %d,\
                             \"is_railway_enabled_2\": %d,\
                             \"is_railway_enabled_3\": %d,\
                             \"is_railway_enabled_4\": %d,\
                             \"mode\": %d,\
                             \"type_card\": \"%s\",\
                             \"type_cpu\": %d,\
                             \"server_conect\": %d,\
                             \"time_form\": %d,\
                             \"time_line\": %d,\
                             \"time_index\": %d,\
                             \"time_begin_hour_apply\": %d,\
                             \"time_begin_minute_apply\": %d,\
                             \"t\": %d,\
                             \"num_side\": %d,\
                             \"yellow\": %d,\
                             \"giaitoa\": %d,\
                             \"green_1\": %d,\
                             \"red_1\": %d,\
                             \"green_2\": %d,\
                             \"red_2\": %d,\
                             \"green_3\": %d,\
                             \"red_3\": %d,\
                             \"green_4\": %d,\
                             \"red_4\": %d}" 
                             
                             
#define TEMPLATE_STATUS "CPU-{\"CMD\": %d,%s, %s}"

#define TEMPLATE_CPU_TO_QT_FB "CPU-{\"CMD\": %d,\
                            \"flagFB\": %d}"
                            
#define TEMPLATE_CYCLE  "CPU-{\"CMD\": %d,\
                             \"cycle_cycle_1_0\": %d,\
                             \"cycle_cycle_1_1\": %d,\
                             \"cycle_cycle_1_2\": %d,\
                             \"cycle_cycle_1_3\": %d,\
                             \"cycle_cycle_1_4\": %d,\
                             \"cycle_cycle_1_5\": %d,\
                             \"cycle_cycle_1_6\": %d,\
                             \"cycle_cycle_1_7\": %d,\
                             \"cycle_cycle_1_8\": %d,\
                             \"cycle_cycle_1_9\": %d,\
                             \"cycle_cycle_2_0\": %d,\
                             \"cycle_cycle_2_1\": %d,\
                             \"cycle_cycle_2_2\": %d,\
                             \"cycle_cycle_2_3\": %d,\
                             \"cycle_cycle_2_4\": %d,\
                             \"cycle_cycle_2_5\": %d,\
                             \"cycle_cycle_2_6\": %d,\
                             \"cycle_cycle_2_7\": %d,\
                             \"cycle_cycle_2_8\": %d,\
                             \"cycle_cycle_2_9\": %d,\
                             \"cycle_cycle_3_0\": %d,\
                             \"cycle_cycle_3_1\": %d,\
                             \"cycle_cycle_3_2\": %d,\
                             \"cycle_cycle_3_3\": %d,\
                             \"cycle_cycle_3_4\": %d,\
                             \"cycle_cycle_3_5\": %d,\
                             \"cycle_cycle_3_6\": %d,\
                             \"cycle_cycle_3_7\": %d,\
                             \"cycle_cycle_3_8\": %d,\
                             \"cycle_cycle_3_9\": %d,\
                             \"cycle_cycle_4_0\": %d,\
                             \"cycle_cycle_4_1\": %d,\
                             \"cycle_cycle_4_2\": %d,\
                             \"cycle_cycle_4_3\": %d,\
                             \"cycle_cycle_4_4\": %d,\
                             \"cycle_cycle_4_5\": %d,\
                             \"cycle_cycle_4_6\": %d,\
                             \"cycle_cycle_4_7\": %d,\
                             \"cycle_cycle_4_8\": %d,\
                             \"cycle_cycle_4_9\": %d,\
                             \"cycle_cycle_5_0\": %d,\
                             \"cycle_cycle_5_1\": %d,\
                             \"cycle_cycle_5_2\": %d,\
                             \"cycle_cycle_5_3\": %d,\
                             \"cycle_cycle_5_4\": %d,\
                             \"cycle_cycle_5_5\": %d,\
                             \"cycle_cycle_5_6\": %d,\
                             \"cycle_cycle_5_7\": %d,\
                             \"cycle_cycle_5_8\": %d,\
                             \"cycle_cycle_5_9\": %d,\
                             \"cycle_day_0\": %d,\
                             \"cycle_day_1\": %d,\
                             \"cycle_day_2\": %d,\
                             \"cycle_day_3\": %d,\
                             \"cycle_day_4\": %d,\
                             \"cycle_day_5\": %d,\
                             \"cycle_day_6\": %d,\
                             \"cycle_timeline_00_begin_h\": %d,\
                             \"cycle_timeline_00_begin_m\": %d,\
                             \"cycle_timeline_00_begin_c\": %d,\
                             \"cycle_timeline_01_begin_h\": %d,\
                             \"cycle_timeline_01_begin_m\": %d,\
                             \"cycle_timeline_01_begin_c\": %d,\
                             \"cycle_timeline_02_begin_h\": %d,\
                             \"cycle_timeline_02_begin_m\": %d,\
                             \"cycle_timeline_02_begin_c\": %d,\
                             \"cycle_timeline_03_begin_h\": %d,\
                             \"cycle_timeline_03_begin_m\": %d,\
                             \"cycle_timeline_03_begin_c\": %d,\
                             \"cycle_timeline_04_begin_h\": %d,\
                             \"cycle_timeline_04_begin_m\": %d,\
                             \"cycle_timeline_04_begin_c\": %d,\
                             \"cycle_timeline_10_begin_h\": %d,\
                             \"cycle_timeline_10_begin_m\": %d,\
                             \"cycle_timeline_10_begin_c\": %d,\
                             \"cycle_timeline_11_begin_h\": %d,\
                             \"cycle_timeline_11_begin_m\": %d,\
                             \"cycle_timeline_11_begin_c\": %d,\
                             \"cycle_timeline_12_begin_h\": %d,\
                             \"cycle_timeline_12_begin_m\": %d,\
                             \"cycle_timeline_12_begin_c\": %d,\
                             \"cycle_timeline_13_begin_h\": %d,\
                             \"cycle_timeline_13_begin_m\": %d,\
                             \"cycle_timeline_13_begin_c\": %d,\
                             \"cycle_timeline_14_begin_h\": %d,\
                             \"cycle_timeline_14_begin_m\": %d,\
                             \"cycle_timeline_14_begin_c\": %d,\
                             \"cycle_timeline_20_begin_h\": %d,\
                             \"cycle_timeline_20_begin_m\": %d,\
                             \"cycle_timeline_20_begin_c\": %d,\
                             \"cycle_timeline_21_begin_h\": %d,\
                             \"cycle_timeline_21_begin_m\": %d,\
                             \"cycle_timeline_21_begin_c\": %d,\
                             \"cycle_timeline_22_begin_h\": %d,\
                             \"cycle_timeline_22_begin_m\": %d,\
                             \"cycle_timeline_22_begin_c\": %d,\
                             \"cycle_timeline_23_begin_h\": %d,\
                             \"cycle_timeline_23_begin_m\": %d,\
                             \"cycle_timeline_23_begin_c\": %d,\
                             \"cycle_timeline_24_begin_h\": %d,\
                             \"cycle_timeline_24_begin_m\": %d,\
                             \"cycle_timeline_24_begin_c\": %d}"

#define TEMPLATE_SETTING "CPU-{\"CMD\": %d,\
                            \"server\": %d}"
                            
#define TEMPLATE_STATUS_CONNECT_MQTT "CPU-{\"CMD\": %d,\
\"server_connect\": %d}"

#define MAX_CYCLE_FORM              32
#define MAX_TIME_FORM               8
#define MAX_TIME_LINE               8
#define MAX_DAYS_WORKING            7

#define NONE                        0
#define AUTO_MODE                   1
#define MANUAL_MODE                 2

typedef struct
{
    uint8_t hh_start;
    uint8_t mm_start;
    uint8_t hh_end;
    uint8_t mm_end;
} time_active_t;
time_active_t time_active;

FILE *time_active_file;
void Write_struct_time_active_toFile(FILE *file, char *link, time_active_t data_Write);
time_active_t Read_struct_time_active_toFile(FILE *file, char *link, time_active_t data_Write);

typedef struct __attribute__((packed))
{
    uint8_t xanh1;
    uint8_t xanh2;
    uint8_t xanh3;
    uint8_t xanh4;
    uint8_t vang;
    uint8_t giaitoa;
} schedule_t;
schedule_t schedule;

FILE *schedule_file;
void Write_struct_schedule_toFile(FILE *file, char *link, schedule_t data_Write);
schedule_t Read_struct_schedule_toFile(FILE *file, char *link, schedule_t data_Write);

typedef struct __attribute__((packed))
{
    uint8_t sopha;
} sopha_t;
sopha_t sopha;

FILE *sopha_file;
void Write_struct_sopha_toFile(FILE *file, char *link, sopha_t data_Write);
sopha_t Read_struct_sopha_toFile(FILE *file, char *link, sopha_t data_Write);

typedef struct __attribute__((packed)){
    uint8_t day;
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
    uint8_t date;//1-31
    uint8_t month;
    uint16_t years;
}type_date_time_t;
type_date_time_t type_date_time;

typedef struct __attribute__((packed))
{
    uint8_t num_side; //So pha cua CPU
    uint8_t yellow_t;//Thoi gian den vang
    uint8_t clearance_t;//Thoi gian giai toa
    uint8_t green_t[MAX_SIDE];//Thoi gian xanh cua tung pha
} type_mtfc_quick_cycle_config_t;
type_mtfc_quick_cycle_config_t type_mtfc_quick_cycle_config;

typedef struct __attribute__((packed))
{
    char sn[32];
    uint32_t imei;
    uint8_t phase;
    uint8_t is_railway_enabled;
    uint8_t is_walking_enabled;
    uint8_t is_dependent_phase;
    uint8_t time_delay_dependent_phase;
} type_one_cardConfig_t;

typedef struct __attribute__((packed))
{
    type_one_cardConfig_t index[MAX_SIDE];
} type_cardConfig_t;
type_cardConfig_t type_cardConfig;
type_cardConfig_t change_type_cardConfig;

typedef struct __attribute__((packed))
{
    uint8_t slot;
    uint8_t phase;
    uint8_t is_walking;
    uint8_t is_railway;
    uint8_t is_dependent_phase;
} type_mtfc_one_card_config_t;
type_mtfc_one_card_config_t type_mtfc_one_card_config;

typedef struct __attribute__((packed)){
    uint8_t hour;
    uint8_t minute;
}type_hm_time_t;

typedef struct __attribute__((packed))
{
    uint8_t index[7];
} type_days_lamp_form_t;

typedef struct __attribute__((packed))
{
    type_hm_time_t t_begin_apply;
    uint8_t index_cycle_form;
} type_one_timeline_t;

typedef struct __attribute__((packed))
{
    type_one_timeline_t select_point[5];
} type_day_time_line_t;

typedef struct __attribute__((packed))
{
    type_day_time_line_t index[5];
} type_time_lamp_form_t;

typedef struct __attribute__((packed))
{
    uint8_t t_green;//thoi gian xanh
    uint8_t t_yellow;//thoi gian vang
    uint8_t t_red;//thoi gin do
    uint8_t t_start_green;//thoi gian bat dau xanh
    uint8_t t_end_green;//thoi gian end xanh
    uint8_t t_start_yellow;//thoi gian bat dau vang
    uint8_t t_end_yellow;//thoi gian ket thuc vang
} type_one_side_lamp_time_t;

typedef struct __attribute__((packed))
{
    uint8_t num_side_used;
    uint8_t period_crossroads;
    type_one_side_lamp_time_t side[8];
    uint8_t clearance_time_crossroads;
} type_one_cycle_lamp_time_t;

typedef struct __attribute__((packed))
{
    type_one_cycle_lamp_time_t index[32];
} type_cycle_lamp_form_t;

typedef struct __attribute__((packed))
{
    type_hm_time_t tbegin;
    type_hm_time_t tend;
} type_active_lamp_t;

typedef struct __attribute__((packed))
{
    uint8_t num_side; //So pha cua CPU
    type_active_lamp_t active_time;//Thoi gian hoat dong cua CPU
    type_cycle_lamp_form_t cycle;//Luu cac chien luoc
    type_time_lamp_form_t time;//Bieu mau thoi gian
    type_days_lamp_form_t days;//Bieu mau tuan
} type_mtfc_schedule_t;
type_mtfc_schedule_t mtfc_schedule;
type_mtfc_schedule_t mtfc_schedule_app_to_device;

typedef struct __attribute__((packed))
{
    uint8_t phase;//thoi gian xanh
    uint8_t time_delay_dependent_phase;//thoi gian vang
    uint8_t time_delay_on_railway;//thoi gin do
    uint8_t time_delay_off_railway;//thoi gian bat dau xanh
    uint8_t time_walking;//thoi gian end xanh
} time_setting_t;
time_setting_t time_setting;

typedef struct __attribute__((packed))
{
    uint8_t route_setting;
    uint8_t route_setting_infor;
    uint8_t time_setting;
    uint8_t time_setting_infor;
    uint8_t sync_time;
    uint8_t pinout_setting;
    uint8_t strategy_setting;
    uint8_t pinout_setting_infor;
    uint8_t sttrategy_setting_infor;
    uint8_t card_setting_infor;
    uint8_t controller_infor;
    uint8_t controller_check_infor;
    uint8_t phase_infor;
    uint8_t flash_yellow;
    uint8_t phase_infor_app;
    uint8_t flash_yellow_app;
} flag_control_t;
 flag_control_t flag_control;

typedef struct __attribute__((packed))
{
    uint8_t is_active_time;
    uint8_t is_cycle_form;
    uint8_t is_time_form;
    uint8_t is_day_form;

} flag_send_data_to_qt_t;
 flag_send_data_to_qt_t flag_send_data_to_qt;

typedef struct __attribute__((packed))
{
    uint8_t countdown[MAX_SIDE];
    uint8_t tm_inpendent[MAX_SIDE];
    uint8_t status_lamp_dependent[MAX_SIDE]; //x 1 v 2 d 3
}type_time_mtfc_xvdi_t;

typedef struct __attribute__((packed))
{
    uint8_t ouput[8];
    uint8_t mapping[8];
    uint8_t pair_signal;
    type_date_time_t type_date_time;
    type_time_mtfc_xvdi_t tm_cycle;
} type_mtfc_card_data_t;
type_mtfc_card_data_t mtfc_card_data_out;

typedef struct __attribute__((packed))
{
    uint8_t time_form;
    uint8_t time_line;
    uint8_t time_index;
    uint8_t time_begin_hour_apply;
    uint8_t time_begin_minute_apply;
    uint8_t t;
    uint8_t num_side;
    uint8_t yellow;
    uint8_t giaitoa;
    uint8_t green_1;
    uint8_t red_1;
    uint8_t green_2;
    uint8_t red_2;
    uint8_t green_3;
    uint8_t red_3;
    uint8_t green_4;
    uint8_t red_4;
    type_cardConfig_t type_cardConfig;
} time_current_t;
time_current_t time_current;

typedef struct __attribute__((packed))
{
    uint8_t day[7];
    type_time_lamp_form_t time;
    uint8_t cycle_1[10];
    uint8_t cycle_2[10];
    uint8_t cycle_3[10];
    uint8_t cycle_4[10];
    uint8_t cycle_5[10];
    // uint8_t cycle_6[10];
    // uint8_t cycle_7[10];
    // uint8_t cycle_8[10];
} cycle_t;
cycle_t cycle;
cycle_t mtfc_cycle_app_to_device;

FILE *cycle_file;
void Write_struct_cycle_toFile(FILE *file, char *link, cycle_t data_Write);
cycle_t Read_struct_cycle_toFile(FILE *file, char *link, cycle_t data_Write);

typedef struct __attribute__((packed))
{
	uint8_t server;
	uint8_t type;
	char urlmqtt[100];
} setting_t; 
setting_t setting;

struct reconnect_state_t {
    const char* username;
    const char* password;
    const char* hostname;
    const char* port;
    const char* topic;
    uint8_t* sendbuf;
    size_t sendbufsz;
    uint8_t* recvbuf;
    size_t recvbufsz;
};

// bien su dung cho app cu CPU moi

typedef enum _set_state_
{
    mtfc_unset = 0,
    mtfc_unused_form = 255,
    mtfc_unused_time = 255
} _set_state_;

typedef struct cpu_active_t
{
    uint8_t mode_active;
}cpu_active_t;
cpu_active_t cpu_active;

typedef struct input_software_t
{
    uint8_t input_software;
}input_software_t;

typedef struct online_t
{
    char online[32];
}online_t;

typedef struct offline_t
{
    char offline[32];
}offline_t;

typedef struct app_connected_t
{
    char app_connected[32];
}app_connected_t;

typedef struct app_disconnected_t
{
    char app_disconnected[32];
}app_disconnected_t;

typedef struct Coordinate_float_t
{
    float Lat;
    float Long;
}Coordinate_float_t;

typedef struct request_config_t
{
    char buff_config[32];
}request_config_t;

typedef struct request_schedule_t
{
    char buff_schedule[32];
}request_schedule_t;

typedef struct request_detail_t
{
    char buff_request[32];
}request_detail_t;

typedef struct feedback_log_t
{
    char feedback_log[64];
}feedback_log_t;

offline_t offline;
online_t online;
app_connected_t app_connected;
app_disconnected_t app_disconnected;
Coordinate_float_t Coordinate_float;
request_config_t request_config;
request_schedule_t request_schedule;
request_detail_t request_detail;
feedback_log_t feedback_log;
input_software_t mtfc_input_software;

FILE *card_file;
void Write_struct_card_toFile(FILE *file, char *link, type_one_cardConfig_t data_Write);
type_one_cardConfig_t Read_struct_card_toFile(FILE *file, char *link, type_one_cardConfig_t data_Write);

FILE *setting_file;
void Write_struct_setting_toFile(FILE *file, char *link, setting_t data_Write);
setting_t Read_struct_setting_toFile(FILE *file, char *link, setting_t data_Write);

char mtfc_data_tmp_current_one_sec[2000];
#ifdef __cplusplus
}
#endif
#endif
