
// Build: clear && gcc -Wall -lev -luwsc -o Cpu Cpu.c function_system.c Modbus_Master_MFM384.c Sitech_Json.c -lwiringPi -lpthread -lmodbus -ljson-c -I.

#include "Cpu.h"


float Random(float n)
{
   return n*rand()/RAND_MAX;
}

const char *url = "ws://localhost:8080";
struct ev_signal signal_watcher;
int ping_interval = 10; /* second */
struct uwsc_client *cl;
int opt;

uint8_t tx_buffer[1024] = {0};

static void stdin_read_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
    struct uwsc_client *cl = w->data;
    char buf[128] = "";
    int n;

    n = read(w->fd, buf, sizeof(buf));
    if (n > 1)
    {
        buf[n - 1] = 0;

        if (buf[0] == 'q')
        {
            printf("exiting...\n");
            cl->send_close(cl, UWSC_CLOSE_STATUS_NORMAL, "ByeBye");
        }
        else
        {
            if (strcmp(buf, "HELLO") == 0)
            {
                cl->send(cl, buf, strlen(buf) + 1, UWSC_OP_TEXT);
                printf("sent Hello to server \r\n");
                printf("Please input:\n");
            }
        }
    }
}

static void uwsc_onopen(struct uwsc_client *cl)
{
    static struct ev_io stdin_watcher;

    uwsc_log_info("onopen\n");

    stdin_watcher.data = cl;

    ev_io_init(&stdin_watcher, stdin_read_cb, STDIN_FILENO, EV_READ);
    ev_io_start(cl->loop, &stdin_watcher);
    is_flag_opened_webSocket = true;
    cl->send_ex(cl, UWSC_OP_TEXT, 1, 3, "CPU");
}

static void uwsc_onmessage(struct uwsc_client *cl, void *data, size_t len, bool binary)
{
    // printf("Recv:");

    if (binary)
    {
        size_t i;
        uint8_t *p = data;

        for (i = 0; i < len; i++)
        {
            printf("%02hhX ", p[i]);
            if (i % 16 == 0 && i > 0)
                puts("");
        }
        puts("");
    }
    else
    {
        if (!strncmp(data, "qtApp-", 4))
        {
            // qtApp-{"CMD": 5, "PWR": "OFF"}
            char *temp = strtok(data, "-");
            temp = strtok(NULL, "\n");
            // printf("[%.*s]\n", strlen(temp), (char *)temp);

            struct json_object *new_obj;
            new_obj = json_tokener_parse(temp);
            // printf("Parsed is: %s\n", temp);
            if (new_obj == NULL)
            {
                return 1;
            }
            if (!new_obj)
            {
                return 1; // oops, we failed.
            }

            getit(new_obj, "CMD");
            if (!json_object_object_get_ex(new_obj, "CMD", &o))
            {
                printf("Field %s does not exist\n", "CMD");
                return;
            }
            switch (json_object_get_int(o))
            {
                // Case Bat tat contactor
            case CMD_CPU_TO_QT_STATUS_PWR: // Trang thai bat CPU
                if (!json_object_object_get_ex(new_obj, "PWR", &o))
                {
                    printf("Field %s does not exist\n", "PWR");
                    return;
                }
                if (json_object_get_int(o))
                {
                    digitalWrite(Relay_1, 1);
                    digitalWrite(Relay_2, 1);
                    digitalWrite(Relay_3, 1);
                    digitalWrite(Relay_4, 1);
                    flag_PowerOn = true;
                    status_Pwr = PWR_IS_ON;
                }
                else
                {
                    digitalWrite(Relay_1, 0);
                    digitalWrite(Relay_2, 0);
                    digitalWrite(Relay_3, 0);
                    digitalWrite(Relay_4, 0);
                    status_Pwr = PWR_OFF;
                }
                break;
            case CMD_QT_TO_CPU_MODE_AUTO_RTC: // mode
                // printf("Hoi auto mode RTC\r\n");
                if (get_State_Cmd_Terminal("timedatectl", "NTP service: active"))
                {
                    // printf("Founded keyword\r\n");
                    int len = sprintf(txBuff, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_MODE_AUTO_RTC, 1);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                    memset(txBuff, 0, sizeof(txBuff));
                }
                else
                {
                    // printf("Not found keyword\r\n");
                    int len = sprintf(txBuff, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_MODE_AUTO_RTC, 0);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                    memset(txBuff, 0, sizeof(txBuff));
                }
                break;
            case CMD_QT_CONTROL_UPDATE_NTP_RTC: // Chon che do update RTC qua internet hay khong
            #pragma region UPDATE TRANG THAI RTC ?
                if (!json_object_object_get_ex(new_obj, "NTP", &o))
                {
                    printf("Field %s does not exist\n", "PWR");
                    return;
                }
                if (json_object_get_int(o))
                {
                    printf("NTP_update: active\r\n");
                    system("sudo timedatectl set-ntp true");
                    int len = sprintf(txBuff, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_CONTROL_UPDATE_NTP_RTC,1);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                    memset(txBuff, 0, sizeof(txBuff));
                }
                else
                {
                    printf("NTP update: inactive\r\n");
                    system("sudo timedatectl set-ntp false");
                    int len = sprintf(txBuff, "CPU-{\"CMD\": %d,\"Value\": %d}", CMD_QT_CONTROL_UPDATE_NTP_RTC, 0);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                    memset(txBuff, 0, sizeof(txBuff));
                }
                break;
#pragma endregion
            case CMD_QT_CONTROL_SET_TIME_RTC: // Cai  dat thoi gian thuc moi
            #pragma region  CAI DAT RTC              
                if (!json_object_object_get_ex(new_obj, "years", &o))
                {
                    return;
                }
                type_RTC_set_CPU_t type_RTC_set_CPU;
                type_RTC_set_CPU.years = json_object_get_int(o); // Lay du lieu cai nam
                if (!json_object_object_get_ex(new_obj, "month", &o))
                {
                    return;
                }
                type_RTC_set_CPU.month = json_object_get_int(o); // Lay du lieu cai thang
                if (!json_object_object_get_ex(new_obj, "day", &o))
                {
                    return;
                }
                type_RTC_set_CPU.day = json_object_get_int(o); // Lay du lieu cai ngay
                if (!json_object_object_get_ex(new_obj, "hh", &o))
                {
                    return;
                }
                type_RTC_set_CPU.hh = json_object_get_int(o); // Lay du lieu cai gio
                if (!json_object_object_get_ex(new_obj, "mm", &o))
                {
                    return;
                }
                type_RTC_set_CPU.mm = json_object_get_int(o); // Lay du lieu cai gio
                type_RTC_set_CPU.ss = 0;
                // printf("Time set ting: %d/%d/%d %d:%d:%d\r\n",type_RTC_set_CPU.years,\
                //                                               type_RTC_set_CPU.month,\
                //                                               type_RTC_set_CPU.day,\
                //                                               type_RTC_set_CPU.hh,\
                //                                               type_RTC_set_CPU.mm, 0);

                char timeSeting_temp[200];
                // sudo date --set="2012/12/12 12:12:12"
                // sudo hwclock --set --date "2012/12/12 12:12:12"
                int len = sprintf(timeSeting_temp, "sudo hwclock --set --date \"%d/%d/%d %d:%d:%d +07\"", type_RTC_set_CPU.years,\
                                                                                                  type_RTC_set_CPU.month,\
                                                                                                  type_RTC_set_CPU.day,\
                                                                                                  type_RTC_set_CPU.hh,\
                                                                                                  type_RTC_set_CPU.mm, 0);
                printf("%s\r\n", timeSeting_temp);
                system(timeSeting_temp);
                delay(100);
                memset(timeSeting_temp,0, sizeof(timeSeting_temp));
                len = sprintf(timeSeting_temp, "sudo date --set=\"%d/%d/%d %d:%d:%d\"", type_RTC_set_CPU.years,\
                                                                                                  type_RTC_set_CPU.month,\
                                                                                                  type_RTC_set_CPU.day,\
                                                                                                  type_RTC_set_CPU.hh,\
                                                                                                  type_RTC_set_CPU.mm, 0);
                printf("%s\r\n", timeSeting_temp);
                system(timeSeting_temp);
                delay(100);
                // gui status thanh cong.
                len = sprintf(txBuff,"CPU-{\"CMD\": %d}", CMD_QT_CONTROL_SET_TIME_RTC);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                memset(txBuff, 0, sizeof(txBuff));
                printf("send status set new time success\r\n");
                break;
#pragma endregion
            default:
                break;
            }

        }
    }
}

static void uwsc_onerror(struct uwsc_client *cl, int err, const char *msg)
{
    uwsc_log_err("onerror:%d: %s\n", err, msg);
    ev_break(cl->loop, EVBREAK_ALL);
}

static void uwsc_onclose(struct uwsc_client *cl, int code, const char *reason)
{
    uwsc_log_err("onclose:%d: %s\n", code, reason);
    ev_break(cl->loop, EVBREAK_ALL);
}

static void signal_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
    if (w->signum == SIGINT)
    {
        ev_break(loop, EVBREAK_ALL);
        uwsc_log_info("Normal quit\n");
    }
}

int main()
{

    struct ev_loop *loop = EV_DEFAULT;

    if (wiringPiSetup() < 0)
        return 1;
    pinMode(Relay_1, OUTPUT);
    pinMode(Relay_2, OUTPUT);
    pinMode(Relay_3, OUTPUT);
    pinMode(Relay_4, OUTPUT);

    if ((RS485_RF = serialOpen("/dev/ttyUSB_modeBusSELECT", 115200)) < 0)
        return 1; // Luong giao tiep vs Root RF

    if (pthread_create(&thread_ModBus, NULL, ModeBus, NULL) != 0)
    {
        printf("thread_create() failed\n");
        return 1;
    }

    if (pthread_create(&thread_Serial, NULL, SerialRF, NULL) != 0)
    {
        return 1;
        printf("thread_create() failed\n");
    }

    if (pthread_create(&thread_WebsocketQt, NULL, WebsocketQt, NULL) != 0)
    {
        printf("thread_create() failed\n");
        return 1;
    }

    if (pthread_create(&thread_Cpu, NULL, Cpu_process, NULL) != 0)
    {
        printf("thread_create() failed\n");
        return 1;
    }
    if (pthread_create(&thread_Common_QT, NULL, Common_QT, NULL) != 0)
    {
        printf("thread_create() failed\n");
        return 1;
    }

    uwsc_log_info("Libuwsc: %s\n", UWSC_VERSION_STRING);

    cl = uwsc_new(loop, url, ping_interval, NULL);
    if (!cl)
        return -1;

    uwsc_log_info("Start connect...\n");

    cl->onopen = uwsc_onopen;
    cl->onmessage = uwsc_onmessage;
    cl->onerror = uwsc_onerror;
    cl->onclose = uwsc_onclose;

    ev_signal_init(&signal_watcher, signal_cb, SIGINT);
    ev_signal_start(loop, &signal_watcher);

    ev_run(loop, 0);

    free(cl);
    return 0;
}

void *ModeBus(void *threadArgs)
{
    modbus_t *ctx = modbus_new_rtu("/dev/ttyUSB_FTDI", 9600, 'N', 8, 1);
    if (!ctx)
    {
        fprintf(stderr, "Failed to create the context: %s\n", modbus_strerror(errno));
        exit(1);
    }
    printf("RS485 khoi tao thanh cong\r\n");
    delay(100);
    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Unable to connect: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        exit(1);
    }
    //Set the Modbus address of the remote slave (to 3)
    modbus_set_slave(ctx, 2);
#ifdef DEBUG
    printf("Ket noi modbus RTU thanh cong\r\n");
#endif
    delay(10000);
    srand(time(NULL));
    while (1)
    {
        delay(1);
        if (is_flag_opened_webSocket)
        {
            /*----------------------------------------------Doc U, I, P, Cos Pi-------------------------------------------------------------*/
            uint16_t reg[60] = {0}; // will store read registers values

            //Read 60 holding registers starting from address 10
            int num = modbus_read_input_registers(ctx, 0, 60, reg);
            if (num != 60)
            { // number of read registers is not the one expected
                fprintf(stderr, "Failed to read: %s\n", modbus_strerror(errno));
            }
            parameter_MFM384_t parameter_MFM384;
            parameter_MFM384.V1_MFM384 = convert_array_uint16_to_float(reg, 0, 1);
            parameter_MFM384.V2_MFM384 = convert_array_uint16_to_float(reg, 2, 3);
            parameter_MFM384.V3_MFM384 = convert_array_uint16_to_float(reg, 4, 5);

            parameter_MFM384.I1_MFM384 = convert_array_uint16_to_float(reg, 16, 17);
            parameter_MFM384.I2_MFM384 = convert_array_uint16_to_float(reg, 18, 19);
            parameter_MFM384.I3_MFM384 = convert_array_uint16_to_float(reg, 20, 21);

            parameter_MFM384.P1_MFM384 = convert_array_uint16_to_float(reg, 24, 25);
            parameter_MFM384.P2_MFM384 = convert_array_uint16_to_float(reg, 26, 27);
            parameter_MFM384.P3_MFM384 = convert_array_uint16_to_float(reg, 28, 29);
            parameter_MFM384.P_Total_MFM384 = convert_array_uint16_to_float(reg, 42, 43);

            parameter_MFM384.PF1_MFM384 = convert_array_uint16_to_float(reg, 48, 49);
            parameter_MFM384.PF2_MFM384 = convert_array_uint16_to_float(reg, 50, 51);
            parameter_MFM384.PF3_MFM384 = convert_array_uint16_to_float(reg, 52, 53);

            parameter_MFM384.F1_MFM384 = convert_array_uint16_to_float(reg, 56, 57);
            parameter_MFM384.F2_MFM384 = convert_array_uint16_to_float(reg, 56, 57);
            parameter_MFM384.F3_MFM384 = convert_array_uint16_to_float(reg, 56, 57);

#ifdef DEBUG
            /*----------------------------------------------Doc U, I, P, Cos Pi------------------------------------------------------------*/
            printf("--------------------------------------------------Parameter Power Update----------------------------------------------------\r\n");
            printf("\r\n");

            printf("V1: %.02f --- ", parameter_MFM384.V1_MFM384);
            printf("V2: %.02f --- ", parameter_MFM384.V2_MFM384);
            printf("V3: %.02f --- ", parameter_MFM384.V3_MFM384);

            printf("I1: %.02f --- ", parameter_MFM384.I1_MFM384);
            printf("I2: %.02f --- ", parameter_MFM384.I2_MFM384);
            printf("I3: %.02f --- ", parameter_MFM384.I3_MFM384);

            printf("P1: %.02f --- ", parameter_MFM384.P1_MFM384);
            printf("P2: %.02f --- ", parameter_MFM384.P2_MFM384);
            printf("P3: %.02f --- ", parameter_MFM384.P3_MFM384);
            printf("ToTal Power: %.02f --- ", parameter_MFM384.P_Total_MFM384);

            printf("PF1: %.02f --- ", parameter_MFM384.PF1_MFM384);
            printf("PF2: %.02f --- ", parameter_MFM384.PF2_MFM384);
            printf("PF3: %.02f --- ", parameter_MFM384.PF3_MFM384);

            printf("F1: %.02fHz --- ", parameter_MFM384.F1_MFM384);
            printf("F2: %.02fHz ---", parameter_MFM384.F2_MFM384);
            printf("F3: %.02fHz", parameter_MFM384.F3_MFM384);
            printf("\r\n---------------------------------------------------------------------------------------------------------------------------\r\n");
            printf("\r\n");
#endif
            char data[1000] = {0};
            
            int data_len = sprintf(data, TEMPLATE_POWER,
                                   CMD_CPU_TO_QT_PARAMETER_PWR,
                                   parameter_MFM384.V1_MFM384,
                                   parameter_MFM384.V2_MFM384,
                                   parameter_MFM384.V3_MFM384,
                                   parameter_MFM384.I1_MFM384,
                                   parameter_MFM384.I2_MFM384,
                                   parameter_MFM384.I3_MFM384,
                                   parameter_MFM384.P1_MFM384,
                                   parameter_MFM384.P2_MFM384,
                                   parameter_MFM384.P3_MFM384,
                                //    parameter_MFM384.P_Total_MFM384,
                                   Random(30.0),
                                   parameter_MFM384.PF1_MFM384,
                                   parameter_MFM384.PF2_MFM384,
                                   parameter_MFM384.PF3_MFM384,
                                   parameter_MFM384.F1_MFM384,
                                   parameter_MFM384.F2_MFM384,
                                   parameter_MFM384.F3_MFM384);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, data_len, data);
            memset(data, 0, 1000);
            // char *data = get_json_update_power_cabinet(&parameter_MFM384);
            // int data_len = strlen(data);
            // cl->send_ex(cl, UWSC_OP_TEXT, 1, data_len, data);
            // free(data);
            delay(1000);
        }
    }
    modbus_close(ctx);
    modbus_free(ctx);
}

void *SerialRF(void *threadArgs)
{
    while (1)
    {
        delay(1);
        serialPrintf(RS485_RF, "Thread RS485 RF!!!\n");
        delay(1000);
    }
}

void *Common_QT(void *threadArgs)
{
    while (1)
    {
        delay(500);

        // Doc time bat tat he thong hien tai
        type_active_lamp_t *type_active_lamp_Read = malloc(sizeof(type_active_lamp_Read));
        file_active_time = fopen("/home/pi/File_CPU_Lighting/TimeActive.txt", "rb");

        if (file_active_time != NULL)
        {
            fread(type_active_lamp_Read, sizeof(type_active_lamp_Read), 1, file_active_time);
            fclose(file_active_time);
        }
#ifdef DEBUG
        printf("Time active systems: %.02d:%.02d - %.02d:%.02d\r\n", type_active_lamp_Read->tbegin.hour,
               type_active_lamp_Read->tbegin.minute,
               type_active_lamp_Read->tend.hour,
               type_active_lamp_Read->tend.minute);
#endif
        // Gan bien toan cuc bat tat he thong
        type_active_lamp.tbegin.hour = type_active_lamp_Read->tbegin.hour;
        type_active_lamp.tbegin.minute = type_active_lamp_Read->tbegin.minute;
        type_active_lamp.tend.hour = type_active_lamp_Read->tend.hour;
        type_active_lamp.tend.minute = type_active_lamp_Read->tend.minute;
        free(type_active_lamp_Read);

        // Gui len man hinh hien thi thoi bgian bat tat
        int length_activeTime = sprintf(txBuff, "CPU-{\"CMD\": %d, \"Hour_On\": %d, \"Minute_On\": %d,\"Hour_Off\": %d, \"Minute_Off\": %d}",
                                        CMD_CPU_TO_QT_ACTIVE_TIME,
                                        type_active_lamp.tbegin.hour,
                                        type_active_lamp.tbegin.minute,
                                        type_active_lamp.tend.hour,
                                        type_active_lamp.tend.minute);
#ifdef DEBUG
        printf("Encoder time active JSON: %s\n", txBuff);

#endif
        // cl->send_ex(cl, UWSC_OP_TEXT, 1, length_activeTime, txBuff);
        // memset(txBuff, 0, sizeof(txBuff));

        // Lay thong tin nguon dang bat hay tat hien thi
        // int length_status_pwr = sprintf(txBuff, "CPU-{\"CMD\": %d, \"STATUS_PWR\": %d}", CMD_CPU_TO_QT_STATUS_PWR, status_Pwr);
        // cl->send_ex(cl, UWSC_OP_TEXT, 1, length_status_pwr, txBuff);
        // memset(txBuff, 0, sizeof(txBuff));
    }
}

void *WebsocketQt(void *threadArgs)
{
    printf("Qt websocket here\r\n");
    while (1)
    {
        delay(1);
        if (flag_PowerOn)
        {
            delay(5000);
            flag_PowerOn = false;
            status_Pwr = PWR_ON;
        }
    }
}

void *Cpu_process(void *threadArgs)
{
    while (1)
    {
        delay(1);
        if (digitalRead(auto_Mode) == 0)
        {
            // Time out 1s gui che do len hien thi tren man hinh`
            if (is_flag_opened_webSocket)
            {
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis_sendMode_to_App > interval_sendMode)
                {
                    previousMillis_sendMode_to_App = currentMillis;

                    // Lay thong tin loai he thong o duoi
                    // Gui len man hinh
                    // int length_home = sprintf(txBuff, "CPU-{\"CMD\": %d,\"MODE\": %d , \"TYPE\": %d, \"NODE\": %d, \"CONNECTSERVER\": %d}", CMD_CPU_TO_QT_HOME, AUTO_MODE, RF, 120, 0);
                    // cl->send_ex(cl, UWSC_OP_TEXT, 1, length_home, txBuff);
                    // memset(txBuff, 0, sizeof(txBuff));
                }
            }
        }
        else if (digitalRead(manual_Mode) == 0)
        {
            if (is_flag_opened_webSocket)
            {
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis_sendMode_to_App > interval_sendMode)
                {
                    previousMillis_sendMode_to_App = currentMillis;

                    // int len = sprintf(txBuff, "CPU-{\"CMD\": %d,\"MODE\": %d , \"TYPE\": %d, \"NODE\": %d, \"CONNECTSERVER\": %d}", CMD_CPU_TO_QT_HOME, MANUAL_MODE, RF, 120, 0);
                    // cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                }
            }
        }
        else
        {
            if (is_flag_opened_webSocket)
            {
                unsigned long currentMillis = millis();
                if (currentMillis - previousMillis_sendMode_to_App > interval_sendMode)
                {
                    previousMillis_sendMode_to_App = currentMillis;
                    // int len = sprintf(txBuff, "CPU-{\"CMD\": %d, \"MODE\": %d , \"TYPE\": %d, \"NODE\": %d, \"CONNECTSERVER\": %d}", CMD_CPU_TO_QT_HOME, NONE, RF, 120, 0);
                    // cl->send_ex(cl, UWSC_OP_TEXT, 1, len, txBuff);
                }
            }
        }
    }
}

#pragma region HAM MO RONG CAC KIEU

static void getit(struct json_object *new_obj, const char *field)
{
    struct json_object *o = NULL;
    if (!json_object_object_get_ex(new_obj, field, &o))
        printf("Field %s does not exist\n", field);

    enum json_type o_type = json_object_get_type(o);
    // printf("new_obj.%s json_object_get_type()=%s\n", field, json_type_to_name(o_type));
    // printf("new_obj.%s json_object_get_int()=%d\n", field, json_object_get_int(o));
    // printf("new_obj.%s json_object_get_int64()=%" PRId64 "\n", field, json_object_get_int64(o));
    // printf("new_obj.%s json_object_get_uint64()=%" PRIu64 "\n", field,
    //        json_object_get_uint64(o));
    // printf("new_obj.%s json_object_get_boolean()=%d\n", field, json_object_get_boolean(o));
    // printf("new_obj.%s json_object_get_double()=%f\n", field, json_object_get_double(o));
}

static void checktype_header()
{
    printf("json_object_is_type: %s,%s,%s,%s,%s,%s,%s\n", json_type_to_name(json_type_null),
           json_type_to_name(json_type_boolean), json_type_to_name(json_type_double),
           json_type_to_name(json_type_int), json_type_to_name(json_type_object),
           json_type_to_name(json_type_array), json_type_to_name(json_type_string));
}
static void checktype(struct json_object *new_obj, const char *field)
{
    struct json_object *o = new_obj;
    if (field && !json_object_object_get_ex(new_obj, field, &o))
        printf("Field %s does not exist\n", field);

    printf("new_obj%s%-18s: %d,%d,%d,%d,%d,%d,%d\n", field ? "." : " ", field ? field : "",
           json_object_is_type(o, json_type_null), json_object_is_type(o, json_type_boolean),
           json_object_is_type(o, json_type_double), json_object_is_type(o, json_type_int),
           json_object_is_type(o, json_type_object), json_object_is_type(o, json_type_array),
           json_object_is_type(o, json_type_string));
}
#pragma endregion