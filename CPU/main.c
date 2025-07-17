#include "main.h"
#include "Parameter_CPU.h"
#include "Serial.h"

// #include "connect_app_ws.h"
// Bu#ild: gcc -Wall -lev -luwsc -o Cpu main.c Parameter_CPU.c function_system.c Modbus_Master_MFM384.c Sitech_Json.c Serial.c ../main_proto.pb-c.c ../device_proto.pb-c.c ../user_proto.pb-c.c ../common_proto.pb-c.c -lwiringPi -lpthread -lprotobuf-c -lmodbus -ljson-c -lev -luwsc -I.

#pragma region BIEN TOAN CUC
const char *url = "ws://localhost:8080";
const char *url_server = "wss://dev.hue-traffic.sitech.com.vn/websocket";
// static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ev_signal signal_watcher;
int ping_interval = 10; /* second */
struct uwsc_client *cl;

struct uwsc_client *mtfc_client_sv;
struct ev_signal signal_watcher_app;

int sockfd;
pthread_t client_daemon;
struct mqtt_client client;

long long previousMillis = 0;
long long previousMillis_CPU_5s = 0;
long long previousMillis_CPU_1s = 0;
long long previousMillis_updateServer = 0;
long long interval = 10000;
long long interval_CPU_5s = 5000;
long long interval_CPU_1s = 1000;

void *client_refresher(void *client);
void publish_callback(void **unused, struct mqtt_response_publish *published);
void exit_example(int status, int sockfd, pthread_t *client_daemon);
void mtfc_debug_print_time_form(type_time_lamp_form_t *time_form);
void mtfc_debug_print_days_form(type_days_lamp_form_t *days_form);
void matching_data_to_old_app();
void converttobinary(int n, int m);

int cycle_form1 = 0;
int cycle_form2 = 0;
int cycle_form3 = 0;
int cycle_form4 = 0;
int cycle_form5 = 0;
int cycle_form = 0;
int day_form1 = 0;
int day_form2 = 0;
int day_form3 = 0;
int day = 0;
int red1 = 0;
int yellow1 = 0;
int green1 = 0;
int red2 = 0;
int yellow2 = 0;
int green2 = 0;
int red3 = 0;
int yellow3 = 0;
int green3 = 0;
int red4 = 0;
int yellow4 = 0;
int green4 = 0;
int cardred1 = 0;
int cardyellow1 = 0;
int cardgreen1 = 0;
int cardred2 = 0;
int cardyellow2 = 0;
int cardgreen2 = 0;
int cardred3 = 0;
int cardyellow3 = 0;
int cardgreen3 = 0;
int cardred4 = 0;
int cardyellow4 = 0;
int cardgreen4 = 0;
int cardred5 = 0;
int cardyellow5 = 0;
int cardgreen5 = 0;
int cardred6 = 0;
int cardyellow6 = 0;
int cardgreen6 = 0;
int count = 60;
uint8_t tm_count_card1 = 0;
uint8_t tm_count_card2 = 0;
uint8_t tm_count_card3 = 0;
uint8_t tm_count_card4 = 0;
uint8_t tm_count_card5 = 0;
uint8_t tm_count_card6 = 0;
uint8_t tm_count_card7 = 0;
uint8_t tm_count_card8 = 0;

int modeactive = 0;
int RS232;
char dat;
int switchinfor = 0;
int phase = 0;
int phase_infor = 0;
int yellow_infor = 0;
int mode_cross = 2;
int check_stt_card = 0;
int flat_fb_to_qt = 0;
int routeId1 = 0;
int routeId2 = 0;
int routeId3 = 0;
int routeId4 = 0;
int routeId5 = 0;
int routeId6 = 0;
char topicsub[50] = "/route-setting-infor";
char topicsub2[50] = "/route-setting";
char topicsub3[50] = "/time-setting";
char topicsub4[50] = "/time-setting-infor";
char topicsub5[50] = "/sync-time";
char topicsub6[50] = "/pinout-setting";
char topicsub7[50] = "/strategy-setting";
char topicsub8[50] = "/card-setting";
char topicsub9[50] = "/request-data";
char topicsub10[50] = "/station-route-first";
char topicsub11[50] = "/station-power";

char topic[255] = "";
char topic2[255] = "";
char topic3[255] = "";
char topic4[255] = "";
char topic5[255] = "";
char topic6[255] = "";
char topic7[255] = "";
char topic8[255] = "";
char topic9[255] = "";
char topic10[255] = "";
char topic11[255] = "";

char topicrawpub[50] = "/time-setting";
char topicrawpub2[50] = "/pinout-setting";
char topicrawpub3[50] = "/route-setting";
char topicrawpub4[50] = "/strategy-setting";
char topicrawpub5[50] = "/card-setting";
char topicrawpub6[50] = "/controller";
char topicrawpub7[50] = "/fb-strategy-setting";
char topicrawpub8[50] = "/fb-card-setting";
char topicrawpub9[50] = "/fb-card-setting1";
char topicrawpub10[50] = "/power";
char topicrawpub11[50] = "/station-route-first";

char topicpub[255] = "";
char topicpub2[255] = "";
char topicpub3[255] = "";
char topicpub4[255] = "";
char topicpub5[255] = "";
char topicpub6[255] = "";
char topicpub7[255] = "";
char topicpub8[255] = "";
char topicpub9[255] = "";
char topicpub10[255] = "";
char topicpub11[255] = "";

FILE *fp; // File luu macid Gateway
FILE *fptr;

char mac[18];
char re_mac[100] = "";
char Coordinate_str[100] = "";

char *mac_id_gateway = NULL;
char *mac_final = NULL;
char *temp = NULL;
char *device_id = NULL;

char topicraw[50] = "device/";
char topicraw_pub[50] = "ping/";

#pragma endregion
#pragma region LUONG XU LY KET NOI APP THEO PHUONG THUC MQTT
void exit_example(int status, int sockfd, pthread_t *client_daemon)
{
    if (sockfd != -1)
        close(sockfd);
    if (client_daemon != NULL)
        pthread_cancel(*client_daemon);
    exit(status);
}

void *send_data_to_all(void *threadArgs)
{
    while (1)
    {
        // Active_time
        if (flag_send_data_to_qt.is_active_time)
        {
            char data_send_active_time[1000] = {0};
            int data_send_active_time_len = sprintf(data_send_active_time, TEMPLATE_TIME_ACTIVE, CMD_CPU_TO_QT_TIME_ACTIVE,
                                                    time_active.hh_start,
                                                    time_active.mm_start,
                                                    time_active.hh_end,
                                                    time_active.mm_end);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_active_time_len, data_send_active_time);
            printf("Da send time active to QT OK\r\n");
            send_struct(CMD_CPU_TO_MASTER_TIME_ACTIVE, (uint8_t *)&time_active, sizeof(time_active_t));
            printf("Da send time active to master OK\r\n");
            // flag_control.time_setting_infor = 1;
            printf("Da send time active to server OK\r\n");
            flag_send_data_to_qt.is_active_time = 0;
        }
        if (flag_send_data_to_qt.is_cycle_form)
        {
            printf("Da update cycle form to pt\n");
            printf("\r\n%s", "------------------Cycle Form--------------------------");
            for (int idx = 0; idx < 8; idx++)
            {
                for (int j = 0; j < 8; j++)
                {
                    printf("\t%03d\t%03d\t%03d\t%03d",
                           idx,
                           mtfc_schedule.cycle.index[idx].side[j].t_green,
                           // mtfc_schedule.cycle.index[1].side[j].t_red,
                           mtfc_schedule.cycle.index[idx].side[j].t_yellow,
                           mtfc_schedule.cycle.index[idx].clearance_time_crossroads);

                    char data_send[1000] = {0};
                    int data_send_len = sprintf(data_send, "CPU-{\"CMD\": %d,\
                        \"index\": %d,\
                        \"line\": %d,\
                        \"t_green\": %d,\
                        \"t_yellow\": %d,\
                        \"clearance_time_crossroads\": %d}",
                                                CMD_CPU_TO_QT_SCHEDULE_CYCLE,
                                                (idx),
                                                (j),
                                                mtfc_schedule.cycle.index[idx].side[j].t_green,
                                                mtfc_schedule.cycle.index[idx].side[j].t_yellow,
                                                mtfc_schedule.cycle.index[idx].clearance_time_crossroads);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                    printf("%s\r\n", data_send);
                }
            }
            flag_send_data_to_qt.is_cycle_form = 0;
        }
        if (flag_send_data_to_qt.is_day_form)
        {
            printf("Da update day form to pt\n");
            mtfc_debug_print_days_form(&mtfc_schedule.days);
            flag_send_data_to_qt.is_day_form = 0;
        }
        if (flag_send_data_to_qt.is_time_form)
        {
            printf("Da update day form to pt\n");
            mtfc_debug_print_time_form(&mtfc_schedule.time);
            flag_send_data_to_qt.is_time_form = 0;
        }
    }
}

void reconnect_client(struct mqtt_client *client, void **reconnect_state_vptr)
{
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<reconnect client>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");

    struct reconnect_state_t *reconnect_state = *((struct reconnect_state_t **)reconnect_state_vptr);

    char data_tmp[100] = {0};
    int status_connect_mqtt = -1;
    /* Close the clients socket if this isn't the initial reconnect call */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT)
    {
        close(client->socketfd);
    }

    // /* Perform error handling here. */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT)
    {
        printf("reconnect_client: called while client was in error state \"%s\"\n",
               mqtt_error_str(client->error));
    }
    /* Open a new socket. */
    int sockfd = open_nb_socket(reconnect_state->hostname, reconnect_state->port);
    if (sockfd == -1)
    {
        perror("Failed to open socket: ");
        // exit_example(EXIT_FAILURE, sockfd, NULL);
        status_connect_mqtt = 0;
    }
    else
    {
        status_connect_mqtt = 1;
        printf("Success to open socket\n");
    }

    /* Reinitialize the client. */
    mqtt_reinit(client, sockfd,
                reconnect_state->sendbuf, reconnect_state->sendbufsz,
                reconnect_state->recvbuf, reconnect_state->recvbufsz);
    /* Create an anonymous session */
    const char *client_id = mac_final;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    mqtt_connect(client, client_id, NULL, NULL, 0, mac_final, mac_final, connect_flags, 400);

    int data_send_len3 = sprintf(data_tmp, TEMPLATE_STATUS_CONNECT_MQTT, CMD_CPU_TO_QT_STATUS_CONNECT_MQTT,
                                 status_connect_mqtt);
    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_tmp);
    printf("%s\r\n", data_tmp);
    printf("Send status connect mqtt to QT OK\r\n");

    /* Subscribe to the topic. */
    printf("MQTT check 2\n");
    /* subscribe */
    mqtt_subscribe(client, topic, 0);
    mqtt_subscribe(client, topic2, 0);
    mqtt_subscribe(client, topic3, 0);
    mqtt_subscribe(client, topic4, 0);
    mqtt_subscribe(client, topic5, 0);
    mqtt_subscribe(client, topic6, 0);
    mqtt_subscribe(client, topic7, 0);
    mqtt_subscribe(client, topic8, 0);
    mqtt_subscribe(client, topic9, 0);
    mqtt_subscribe(client, topic10, 0);
    mqtt_subscribe(client, topic11, 0);
    // mqtt_subscribe(&client, topic12, 0);
    /* start publishing the time */
    sleep(1);
}

void publish_callback_recv(void **unused, struct mqtt_response_publish *published)
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("Received publish('%s'): %s\n", topic_name, (const char *)published->application_message);

    free(topic_name);
}

void *Mqtt(void *threadArgs)
{
    printf("MQTT check 1\n");
    // delay(1000);
    //  const char* username =mac_final;
    //  const char* password=mac_final;
    //  const char* client_id =mac_final;
    const char *addr = "traffic.sitech.com.vn";
    // char *add = NULL;
    // add = setting.urlmqtt;
    // const char* addr = add;
    // const char* addr = "mqtt.svnet.xyz";
    // addr = "192.168.1.34";
    const char *port = "1884";

    sprintf(topic, "%s%s%s", topicraw, mac_final, topicsub);
    sprintf(topic2, "%s%s%s", topicraw, mac_final, topicsub2);
    sprintf(topic3, "%s%s%s", topicraw, mac_final, topicsub3);
    sprintf(topic4, "%s%s%s", topicraw, mac_final, topicsub4);
    sprintf(topic5, "%s%s%s", topicraw, mac_final, topicsub5);
    sprintf(topic6, "%s%s%s", topicraw, mac_final, topicsub6);
    sprintf(topic7, "%s%s%s", topicraw, mac_final, topicsub7);
    sprintf(topic8, "%s%s%s", topicraw, mac_final, topicsub8);
    sprintf(topic9, "%s%s%s", topicraw, mac_final, topicsub9);
    sprintf(topic10, "%s%s%s", topicraw, mac_final, topicsub10);
    sprintf(topic11, "%s%s%s", topicraw, mac_final, topicsub11);
    // sprintf(topic12, "%s%s%s", topicraw, mac_final, topicsub12);

    sprintf(topicpub, "%s%s%s", topicraw_pub, mac_final, topicrawpub);
    sprintf(topicpub2, "%s%s%s", topicraw_pub, mac_final, topicrawpub2);
    sprintf(topicpub3, "%s%s%s", topicraw_pub, mac_final, topicrawpub3);
    sprintf(topicpub4, "%s%s%s", topicraw_pub, mac_final, topicrawpub4);
    sprintf(topicpub5, "%s%s%s", topicraw_pub, mac_final, topicrawpub5);
    sprintf(topicpub6, "%s%s%s", topicraw_pub, mac_final, topicrawpub6);
    sprintf(topicpub7, "%s%s%s", topicraw_pub, mac_final, topicrawpub7);
    sprintf(topicpub8, "%s%s%s", topicraw_pub, mac_final, topicrawpub8);
    sprintf(topicpub9, "%s%s%s", topicraw_pub, mac_final, topicrawpub9);
    sprintf(topicpub10, "%s%s%s", topicraw_pub, mac_final, topicrawpub10);
    sprintf(topicpub11, "%s%s%s", topicraw_pub, mac_final, topicrawpub11);
    // sprintf(topicpub12, "%s%s%s", topicraw_pub, mac_final, topicrawpub12);

    /* build the reconnect_state structure which will be passed to reconnect */
    struct reconnect_state_t reconnect_state;
    reconnect_state.hostname = addr;
    reconnect_state.port = port;
    reconnect_state.topic = topic;
    uint8_t sendbuf[2048];
    uint8_t recvbuf[12800];
    reconnect_state.sendbuf = sendbuf;
    reconnect_state.sendbufsz = sizeof(sendbuf);
    reconnect_state.recvbuf = recvbuf;
    reconnect_state.recvbufsz = sizeof(recvbuf);

    mqtt_init_reconnect(&client,
                        reconnect_client, &reconnect_state,
                        publish_callback); //
    if (pthread_create(&client_daemon, NULL, client_refresher, &client))
    {
        fprintf(stderr, "Failed to start client daemon.\n");
        printf("Failed to start client daemon.\n");
        // exit_example(EXIT_FAILURE, sockfd, NULL);
    }
    // printf("MQTT check 2\n");
    /* subscribe */
    // mqtt_subscribe(&client, topic, 0);
    // mqtt_subscribe(&client, topic2, 0);
    // mqtt_subscribe(&client, topic3, 0);
    // mqtt_subscribe(&client, topic4, 0);
    // mqtt_subscribe(&client, topic5, 0);
    // mqtt_subscribe(&client, topic6, 0);
    // mqtt_subscribe(&client, topic7, 0);
    // mqtt_subscribe(&client, topic8, 0);
    // mqtt_subscribe(&client, topic9, 0);
    // mqtt_subscribe(&client, topic10, 0);
    // mqtt_subscribe(&client, topic11, 0);
    // mqtt_subscribe(&client, topic12, 0);
    /* start publishing the time */
    // #ifdef DEBUG_MQTT
    printf("listening for '%s' messages.\n", topic);
    printf("listening for '%s' messages.\n", topic2);
    printf("listening for '%s' messages.\n", topic3);
    printf("listening for '%s' messages.\n", topic4);
    printf("listening for '%s' messages.\n", topic5);
    printf("listening for '%s' messages.\n", topic6);
    printf("listening for '%s' messages.\n", topic7);
    printf("listening for '%s' messages.\n", topic8);
    printf("listening for '%s' messages.\n", topic9);
    printf("listening for '%s' messages.\n", topic10);
    printf("listening for '%s' messages.\n", topic11);
    // printf("listening for '%s' messages.\n", topic12);
    // #endif
    printf("MQTT check 3\n");

    while (1)
    {
        // printf("mqtt ping: %d\r\n", mqtt_ping(&client));
        if (flag_control.time_setting_infor)
        {
            char message3[1280] = {0};
            snprintf(message3, sizeof(message3), "{\"hh_start\":%d,\"mm_start\":%d,\"hh_end\":%d,\"mm_end\":%d}",
                     time_active.hh_start,
                     time_active.mm_start,
                     time_active.hh_end,
                     time_active.mm_end);
            mqtt_publish(&client, topicpub, message3, strlen(message3), MQTT_PUBLISH_QOS_1);
            if (client.error != MQTT_OK)
            {
                fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
                exit_example(EXIT_FAILURE, sockfd, &client_daemon);
            }
            // #ifdef DEBUG_MQTT
            printf("toppic pud: \"%s\"\r\n", topicpub);
            printf("published feedback: \"%s\"\r\n", message3);
            // #endif
            flag_control.time_setting_infor = 0;
        }
        if (flag_control.pinout_setting_infor)
        {
            char message3[1280] = {0};
            snprintf(message3, sizeof(message3), "{\"pinout\":%d}", mode_cross);
            mqtt_publish(&client, topicpub2, message3, strlen(message3), MQTT_PUBLISH_QOS_1);
            if (client.error != MQTT_OK)
            {
                fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
                // exit_example(EXIT_FAILURE, sockfd, &client_daemon);
            }
#ifdef DEBUG_MQTT
            printf("published feedback: \"%s\"\r\n", message3);
#endif
            flag_control.pinout_setting_infor = 0;
        }

        if (flag_control.route_setting_infor)
        {
            char message3[1280] = {0};
            snprintf(message3, sizeof(message3), "{\"route\":%d}", sopha.sopha);
            mqtt_publish(&client, topicpub3, message3, strlen(message3), MQTT_PUBLISH_QOS_1);
            if (client.error != MQTT_OK)
            {
                fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
                // exit_example(EXIT_FAILURE, sockfd, &client_daemon);
            }
#ifdef DEBUG_MQTT
            printf("published feedback: \"%s\"\r\n", message3);
#endif
            flag_control.route_setting_infor = 0;
        }
        if (flag_control.sttrategy_setting_infor)
        {
            send_byte(CMD_CPU_TO_MASTER_SCHEDULE, 1);
            delay(500);
            char message1[1280] = {0};
            char message2[24800] = {0};
            char message3[12800] = {0};
            char message4[1280] = {0};
            char message5[1280] = {0};
            char message6[1280] = {0};
            char message7[1280] = {0};
            char message8[1280] = {0};
            char message9[24800] = {0};

            snprintf(message1, sizeof(message1), "{\"actionSetting\":{\"monday\":%d,\"tuesday\":%d,\"wednesday\":%d,\"thursday\":%d,\"friday\":%d,\"saturday\":%d,\"sunday\":%d}}",
                     cycle.day[1],
                     cycle.day[2],
                     cycle.day[3],
                     cycle.day[4],
                     cycle.day[5],
                     cycle.day[6],
                     cycle.day[0]);

            snprintf(message2, sizeof(message2), "{\"timeForm\":[{\"name\":\"%s\",\"details\":[{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d}],\"local_id\":%d},{\"name\":\"%s\",\"details\":[{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d}],\"local_id\":%d},{\"name\":\"%s\",\"details\":[{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d},{\"timeline\":%d,\"detail_id\":%d,\"start_time\":\"%d:%d\",\"strategy_id\":%d}],\"local_id\":%d}]}",
                     "Biểu mẫu 1",
                     1,
                     1,
                     cycle.time.index[0].select_point[0].t_begin_apply.hour,
                     cycle.time.index[0].select_point[0].t_begin_apply.minute,
                     cycle.time.index[0].select_point[0].index_cycle_form + 1,
                     2,
                     2,
                     cycle.time.index[0].select_point[1].t_begin_apply.hour,
                     cycle.time.index[0].select_point[1].t_begin_apply.minute,
                     cycle.time.index[0].select_point[1].index_cycle_form + 1,
                     3,
                     3,
                     cycle.time.index[0].select_point[2].t_begin_apply.hour,
                     cycle.time.index[0].select_point[2].t_begin_apply.minute,
                     cycle.time.index[0].select_point[2].index_cycle_form + 1,
                     4,
                     4,
                     cycle.time.index[0].select_point[3].t_begin_apply.hour,
                     cycle.time.index[0].select_point[3].t_begin_apply.minute,
                     cycle.time.index[0].select_point[3].index_cycle_form + 1,
                     5,
                     5,
                     cycle.time.index[0].select_point[4].t_begin_apply.hour,
                     cycle.time.index[0].select_point[4].t_begin_apply.minute,
                     cycle.time.index[0].select_point[4].index_cycle_form + 1,
                     1,
                     "Biểu mẫu 2",
                     1,
                     1,
                     cycle.time.index[1].select_point[0].t_begin_apply.hour,
                     cycle.time.index[1].select_point[0].t_begin_apply.minute,
                     cycle.time.index[1].select_point[0].index_cycle_form + 1,
                     2,
                     2,
                     cycle.time.index[1].select_point[1].t_begin_apply.hour,
                     cycle.time.index[1].select_point[1].t_begin_apply.minute,
                     cycle.time.index[1].select_point[1].index_cycle_form + 1,
                     3,
                     3,
                     cycle.time.index[1].select_point[2].t_begin_apply.hour,
                     cycle.time.index[1].select_point[2].t_begin_apply.minute,
                     cycle.time.index[1].select_point[2].index_cycle_form + 1,
                     4,
                     4,
                     cycle.time.index[1].select_point[3].t_begin_apply.hour,
                     cycle.time.index[1].select_point[3].t_begin_apply.minute,
                     cycle.time.index[1].select_point[3].index_cycle_form + 1,
                     5,
                     5,
                     cycle.time.index[1].select_point[4].t_begin_apply.hour,
                     cycle.time.index[1].select_point[4].t_begin_apply.minute,
                     cycle.time.index[1].select_point[4].index_cycle_form + 1,
                     2,
                     "Biểu mẫu 3",
                     1,
                     1,
                     cycle.time.index[2].select_point[0].t_begin_apply.hour,
                     cycle.time.index[2].select_point[0].t_begin_apply.minute,
                     cycle.time.index[2].select_point[0].index_cycle_form + 1,
                     2,
                     2,
                     cycle.time.index[2].select_point[1].t_begin_apply.hour,
                     cycle.time.index[2].select_point[1].t_begin_apply.minute,
                     cycle.time.index[2].select_point[1].index_cycle_form + 1,
                     3,
                     3,
                     cycle.time.index[2].select_point[2].t_begin_apply.hour,
                     cycle.time.index[2].select_point[2].t_begin_apply.minute,
                     cycle.time.index[2].select_point[2].index_cycle_form + 1,
                     4,
                     4,
                     cycle.time.index[2].select_point[3].t_begin_apply.hour,
                     cycle.time.index[2].select_point[3].t_begin_apply.minute,
                     cycle.time.index[2].select_point[3].index_cycle_form + 1,
                     5,
                     5,
                     cycle.time.index[2].select_point[4].t_begin_apply.hour,
                     cycle.time.index[2].select_point[4].t_begin_apply.minute,
                     cycle.time.index[2].select_point[4].index_cycle_form + 1,
                     3);

            snprintf(message4, sizeof(message4), "%d,%d,%d,%d,%d,%d,%d,%d", cycle.cycle_1[0], cycle.cycle_1[1], cycle.cycle_1[2], cycle.cycle_1[3], cycle.cycle_1[4], cycle.cycle_1[5], cycle.cycle_1[6], cycle.cycle_1[7]);
            snprintf(message5, sizeof(message5), "%d,%d,%d,%d,%d,%d,%d,%d", cycle.cycle_2[0], cycle.cycle_2[1], cycle.cycle_2[2], cycle.cycle_2[3], cycle.cycle_2[4], cycle.cycle_2[5], cycle.cycle_2[6], cycle.cycle_2[7]);
            snprintf(message6, sizeof(message6), "%d,%d,%d,%d,%d,%d,%d,%d", cycle.cycle_3[0], cycle.cycle_3[1], cycle.cycle_3[2], cycle.cycle_3[3], cycle.cycle_3[4], cycle.cycle_3[5], cycle.cycle_3[6], cycle.cycle_3[7]);
            snprintf(message7, sizeof(message7), "%d,%d,%d,%d,%d,%d,%d,%d", cycle.cycle_4[0], cycle.cycle_4[1], cycle.cycle_4[2], cycle.cycle_4[3], cycle.cycle_4[4], cycle.cycle_4[5], cycle.cycle_4[6], cycle.cycle_4[7]);
            snprintf(message8, sizeof(message8), "%d,%d,%d,%d,%d,%d,%d,%d", cycle.cycle_5[0], cycle.cycle_5[1], cycle.cycle_5[2], cycle.cycle_5[3], cycle.cycle_5[4], cycle.cycle_5[5], cycle.cycle_5[6], cycle.cycle_5[7]);

            snprintf(message3, sizeof(message3), "{\"name\":\"%d\",\"yellow\":%d,\"free\":%d,\"routies\":[%s],\"local_id\":%d},{\"name\":\"%d\",\"yellow\":%d,\"free\":%d,\"routies\":[%s],\"local_id\":%d},{\"name\":\"%d\",\"yellow\":%d,\"free\":%d,\"routies\":[%s],\"local_id\":%d},{\"name\":\"%d\",\"yellow\":%d,\"free\":%d,\"routies\":[%s],\"local_id\":%d},{\"name\":\"%d\",\"yellow\":%d,\"free\":%d,\"routies\":[%s],\"local_id\":%d}",
                     1, cycle.cycle_1[8], cycle.cycle_1[9], message4, 1,
                     2, cycle.cycle_2[8], cycle.cycle_2[9], message5, 2,
                     3, cycle.cycle_3[8], cycle.cycle_3[9], message6, 3,
                     4, cycle.cycle_4[8], cycle.cycle_4[9], message7, 4,
                     5, cycle.cycle_5[8], cycle.cycle_5[9], message8, 5);

            snprintf(message9, sizeof(message9), "{\"strategy\":[%s]}", message3);
            mqtt_publish(&client, topicpub4, message1, strlen(message1), MQTT_PUBLISH_QOS_1);
            delay(100);
            mqtt_publish(&client, topicpub4, message2, strlen(message2), MQTT_PUBLISH_QOS_1);
            delay(100);
            mqtt_publish(&client, topicpub4, message9, strlen(message9), MQTT_PUBLISH_QOS_1);
            if (client.error != MQTT_OK)
            {
                fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
            }
            // #ifdef DEBUG_MQTT
            printf("published feedback: \"%s\"\r\n", message1);
            printf("published feedback: \"%s\"\r\n", message2);
            printf("published feedback: \"%s\"\r\n", message9);
            // #endif
            flag_control.sttrategy_setting_infor = 0;
        }
        if (flag_control.card_setting_infor)
        {
            char message1[16192] = {0};
            char message2[1280] = {0};
            char message3[17800] = {0};
            snprintf(message2, sizeof(message2), "\"routeData\":{\"dependents\":[{\"value\":%d,\"route_id\":%d},{\"value\":%d,\"route_id\":%d},{\"value\":%d,\"route_id\":%d}],\"rail_on\":%d,\"rail_off\":%d,\"prioritize\":%d}",
                     type_cardConfig.index[0].time_delay_dependent_phase,
                     type_cardConfig.index[0].phase,
                     type_cardConfig.index[1].time_delay_dependent_phase,
                     type_cardConfig.index[1].phase,
                     type_cardConfig.index[2].time_delay_dependent_phase,
                     type_cardConfig.index[2].phase,
                     time_setting.time_delay_on_railway,
                     time_setting.time_delay_off_railway,
                     time_setting.time_walking);
            snprintf(message1, sizeof(message1), "\"cardSettingData\":[{\"card_num\":%d,\"route_id\":%d,\"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d},{\"card_num\":%d,\"route_id\":%d,\
            \"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d},{\"card_num\":%d,\"route_id\":%d,\"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d},{\"card_num\":%d,\"route_id\":%d,\
            \"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d},{\"card_num\":%d,\"route_id\":%d,\"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d},{\"card_num\":%d,\"route_id\":%d,\
            \"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d},{\"card_num\":%d,\"route_id\":%d,\"is_route\":%d,\"is_rail\":%d,\"is_prioritize\":%d}]",
                     1, type_cardConfig.index[0].phase, type_cardConfig.index[0].is_dependent_phase, type_cardConfig.index[0].is_railway_enabled, type_cardConfig.index[0].is_walking_enabled,
                     2, type_cardConfig.index[1].phase, type_cardConfig.index[1].is_dependent_phase, type_cardConfig.index[1].is_railway_enabled, type_cardConfig.index[1].is_walking_enabled,
                     3, type_cardConfig.index[2].phase, type_cardConfig.index[2].is_dependent_phase, type_cardConfig.index[2].is_railway_enabled, type_cardConfig.index[2].is_walking_enabled,
                     4, type_cardConfig.index[3].phase, type_cardConfig.index[3].is_dependent_phase, type_cardConfig.index[3].is_railway_enabled, type_cardConfig.index[3].is_walking_enabled,
                     5, type_cardConfig.index[4].phase, type_cardConfig.index[4].is_dependent_phase, type_cardConfig.index[4].is_railway_enabled, type_cardConfig.index[4].is_walking_enabled,
                     6, type_cardConfig.index[5].phase, type_cardConfig.index[5].is_dependent_phase, type_cardConfig.index[5].is_railway_enabled, type_cardConfig.index[5].is_walking_enabled,
                     7, type_cardConfig.index[6].phase, type_cardConfig.index[6].is_dependent_phase, type_cardConfig.index[6].is_railway_enabled, type_cardConfig.index[6].is_walking_enabled);

            // mqtt_publish(&client, topicpub5, message3, strlen(message3), MQTT_PUBLISH_QOS_1);
            snprintf(message3, sizeof(message3), "{%s,%s}", message2, message1);

            mqtt_publish(&client, topicpub5, message3, strlen(message3), MQTT_PUBLISH_QOS_1);
            if (client.error != MQTT_OK)
            {
                fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
                // exit_example(EXIT_FAILURE, sockfd, &client_daemon);
            }
            // #ifdef DEBUG_MQTT
            printf("published feedback: \"%s\"\r\n", message3);

            // #endif
            flag_control.card_setting_infor = 0;
        }
        if ((flag_control.controller_infor) && (count > 0) && (flag_control.controller_check_infor))
        {
            count--;
            // char message3[1280]={0};

            // snprintf(message3, sizeof(message3), "[{\"routeId\":%d,\"red\":%d,\"yellow\":%d,\"green\":%d},{\"routeId\":%d,\"red\":%d,\"yellow\":%d,\"green\":%d},{\"routeId\":%d,\"red\":%d,\"yellow\":%d,\"green\":%d}]",
            // 1, red1, yellow1, green1,
            // 2, red2, yellow2, green2,
            // 3, red3, yellow3, green3);
            /// phase 1
            char message1[1280] = {0};
            if ((yellow1 == 0) && (green1 == 0))
                snprintf(message1, sizeof(message1), "{\"routeId\":%d,\"red\":%d}",
                         1, red1);
            else if ((red1 == 0) && (green1 == 0))
                snprintf(message1, sizeof(message1), "{\"routeId\":%d,\"yellow\":%d}",
                         1, yellow1);
            else if ((red1 == 0) && (yellow1 == 0))
                snprintf(message1, sizeof(message1), "{\"routeId\":%d,\"green\":%d}",
                         1, green1);
            else
            {
                if (yellow1 == 1)
                    snprintf(message1, sizeof(message1), "{\"routeId\":%d,\"yellow\":%d}",
                             1, yellow1);
                else
                    snprintf(message1, sizeof(message1), "{\"routeId\":%d,\"yellow\":%d}",
                             1, 0);
            }
            /// phase 2:
            char message2[1280] = {0};
            if ((yellow2 == 0) && (green2 == 0))
                snprintf(message2, sizeof(message2), "{\"routeId\":%d,\"red\":%d}",
                         2, red2);
            else if ((red2 == 0) && (green2 == 0))
                snprintf(message2, sizeof(message2), "{\"routeId\":%d,\"yellow\":%d}",
                         2, yellow2);
            else if ((red2 == 0) && (yellow2 == 0))
                snprintf(message2, sizeof(message2), "{\"routeId\":%d,\"green\":%d}",
                         2, green2);
            else
            {
                if (yellow2 == 1)
                    snprintf(message2, sizeof(message2), "{\"routeId\":%d,\"yellow\":%d}",
                             2, yellow2);
                else
                    snprintf(message2, sizeof(message2), "{\"routeId\":%d,\"yellow\":%d}",
                             2, 0);
            }
            /// phase 3:
            char message3[1280] = {0};
            if ((yellow3 == 0) && (green3 == 0))
                snprintf(message3, sizeof(message3), "{\"routeId\":%d,\"red\":%d}",
                         3, red3);
            else if ((red3 == 0) && (green3 == 0))
                snprintf(message3, sizeof(message3), "{\"routeId\":%d,\"yellow\":%d}",
                         3, yellow3);
            else if ((red3 == 0) && (yellow3 == 0))
                snprintf(message3, sizeof(message3), "{\"routeId\":%d,\"green\":%d}",
                         3, green3);
            else
            {
                if (yellow3 == 1)
                    snprintf(message3, sizeof(message3), "{\"routeId\":%d,\"yellow\":%d}",
                             3, yellow3);
                else
                    snprintf(message3, sizeof(message3), "{\"routeId\":%d,\"yellow\":%d}",
                             3, 0);
            }
            char message4[6000] = {0};
            snprintf(message4, sizeof(message4), "[%s,%s,%s]",
                     message1, message2, message3);

            mqtt_publish(&client, topicpub6, message4, strlen(message4), MQTT_PUBLISH_QOS_1);

            if (client.error != MQTT_OK)
            {
                fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
                // exit_example(EXIT_FAILURE, sockfd, &client_daemon);
            }

            // #ifdef DEBUG_MQTT

            // printf("published feedback: \"%s\"\r\n", message1);
            // printf("published feedback: \"%s\"\r\n", message2);
            // printf("published feedback: \"%s\"\r\n", message3);
            //  printf("published feedback route Id: \"%s\"\r\n", message4);

            // #endif
            if (count <= 0)
                flag_control.controller_infor = 0;
            flag_control.controller_check_infor = 0;
        }
        if (flag_control.phase_infor)
        {
            char message5[6000] = {0};
            snprintf(message5, sizeof(message5), "{\"route\":%d}", phase);
            mqtt_publish(&client, topicpub10, message5, strlen(message5), MQTT_PUBLISH_QOS_1);
            printf("published feedback: \"%s\"\r\n", message5);
            flag_control.phase_infor = 0;
        }
        if (flag_control.phase_infor_app)
        {
            char message5[6000] = {0};
            snprintf(message5, sizeof(message5), "{\"route\":%d}", phase_infor);
            mqtt_publish(&client, topicpub10, message5, strlen(message5), MQTT_PUBLISH_QOS_1);
            printf("published feedback: \"%s\"\r\n", message5);
            flag_control.phase_infor_app = 0;
        }
        if (flag_control.flash_yellow)
        {
            if (phase != 0)
            {
                flag_control.flash_yellow = 0;
                flag_control.phase_infor = 1;
            }
            else if (phase_infor != 0)
            {
                flag_control.flash_yellow = 0;
                flag_control.phase_infor_app = 1;
            }
            else
            {
                char message5[6000] = {0};
                if (modeactive == 0)
                    snprintf(message5, sizeof(message5), "{\"isOn\":%d}", 0);
                else
                    snprintf(message5, sizeof(message5), "{\"isOn\":%d}", 1);
                mqtt_publish(&client, topicpub10, message5, strlen(message5), MQTT_PUBLISH_QOS_1);
                printf("flash_yellow feedback: \"%s\"\r\n", message5);
                flag_control.flash_yellow = 0;
            }
        }
        if (flag_control.flash_yellow_app)
        {
            char message5[6000] = {0};
            snprintf(message5, sizeof(message5), "{\"isOn\":%d}", yellow_infor);
            mqtt_publish(&client, topicpub10, message5, strlen(message5), MQTT_PUBLISH_QOS_1);
            printf("flash_yellow_app feedback: \"%s\"\r\n", message5);
            flag_control.flash_yellow_app = 0;
        }

        if (switchinfor)
        {
            char message5[6000] = {0};
            if (modeactive == 0 || modeactive == 1)
                snprintf(message5, sizeof(message5), "{\"isOn\":%d}", modeactive);
            else
                snprintf(message5, sizeof(message5), "{\"route\":%d}", phase);
            mqtt_publish(&client, topicpub10, message5, strlen(message5), MQTT_PUBLISH_QOS_1);
            printf("switchinfor feedback: \"%s\"\r\n", message5);
            switchinfor = 0;
        }

        // printf("status mqtt ping: %d\r\n", __mqtt_ping(&client));
        // sleep(1);
    }
}
#pragma endregion

#pragma region LUONG XU LY GPS
void *process_GPS(void *threadArgs)
{
    while (1)
    {
        readGPS();
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        ;

        type_date_time.day = (uint8_t)atoi(gps.day);
        type_date_time.hh = (uint8_t)atoi(gps.hour);
        type_date_time.mm = (uint8_t)atoi(gps.minute);
        type_date_time.ss = (uint8_t)atoi(gps.second);
        type_date_time.date = (uint8_t)atoi(gps.day);
        type_date_time.month = (uint8_t)atoi(gps.month);
        type_date_time.years = 2000 + ((uint8_t)atoi(gps.year));

        printf("Time setting: %d/%d/%d %d:%d:%d\r\n", type_date_time.years,
               type_date_time.month,
               type_date_time.date,
               type_date_time.hh,
               type_date_time.mm, type_date_time.ss);
        if (type_date_time.years != 2000 && type_date_time.month != 0 && type_date_time.date != 0)
        {
            send_struct(CMD_CPU_TO_MASTER_SET_RTC_MANUAL, (uint8_t *)&type_date_time, sizeof(type_date_time_t));
            printf("Da send RTC to master OK\r\n");
        }
        pthread_exit(NULL);
    }
}
#pragma endregion

void *Serial(void *threadArgs)
{
    while (1)
    {
        delay(1);
        serial_get_buffer();
    }
}

#pragma region LUONG GIAO TIEP MAN HINH
//**********************************************************************CAC HAM WEBSOCKET*********************************************//
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
#ifdef DEBUG_WEBSOCKET
            printf("exiting...\n");
#endif
            cl->send_close(cl, UWSC_CLOSE_STATUS_NORMAL, "ByeBye");
        }
        else
        {
            if (strcmp(buf, "HELLO") == 0)
            {
                cl->send(cl, buf, strlen(buf) + 1, UWSC_OP_TEXT);
#ifdef DEBUG_WEBSOCKET
                printf("sent Hello to server \r\n");
                printf("Please input:\n");
#endif
            }
        }
    }
}

static void uwsc_onopen(struct uwsc_client *cl)
{
    static struct ev_io stdin_watcher;

    uwsc_log_info("onopen\n");
    stdin_watcher.data = cl;

    // ev_io_init(&stdin_watcher, stdin_read_cb, STDIN_FILENO, EV_READ);
    // ev_io_start(cl->loop, &stdin_watcher);
    is_flag_opened_websocket = true;
    cl->send_ex(cl, UWSC_OP_TEXT, 1, 3, "CPU");
}

static void uwsc_onerror(struct uwsc_client *cl, int err, const char *msg)
{
    uwsc_log_err("onerror:%d: %s\n", err, msg);
    ev_break(cl->loop, EVBREAK_CANCEL );
    is_flag_opened_websocket = false;
    // pthread_exit(NULL);
    exit(1);
}

static void uwsc_onclose(struct uwsc_client *cl, int code, const char *reason)
{
    uwsc_log_err("onclose:%d: %s\n", code, reason);
    ev_break(cl->loop, EVBREAK_ALL);
    is_flag_opened_websocket = false;
    exit(1);
}

static void signal_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
    if (w->signum == SIGINT)
    {
        ev_break(loop, EVBREAK_ALL);
        uwsc_log_info("Normal quit\n");
        is_flag_opened_websocket = false;
        exit(1);
    }
}

static void uwsc_onmessage(struct uwsc_client *cl, void *data, size_t len, bool binary)
{
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
        if (!strncmp(data, "qtApp-", 6))
        {
            printf("reach here\r\n");
            char *temp = strtok(data, "-");
            temp = strtok(NULL, "\n");
            printf("[%.*s]\n", strlen(temp), (char *)temp);
            struct json_object *new_obj;
            new_obj = json_tokener_parse(temp);
            printf("Parsed is: %s\n", temp);
            printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
            if (!new_obj)
            {
                return; // oops, we failed.
            }
            struct json_object *o = NULL;
            if (!json_object_object_get_ex(new_obj, "CMD", &o))
            {
                printf("Field %s does not exist\n", "CMD");
                return;
            }
            switch (json_object_get_int(o))
            {
            case CMD_QT_TO_CPU_TIME_ACTIVE:
            {
                printf("save time active to file\r\n");
                if (!json_object_object_get_ex(new_obj, "hh_start", &o))
                {
                    printf("Field %s does not exist\n", "hh_start");
                    return;
                }
                time_active.hh_start = (uint8_t)json_object_get_int(o);
                printf("\nhh_start is: %d", time_active.hh_start);

                if (!json_object_object_get_ex(new_obj, "mm_start", &o))
                {
                    printf("Field %s does not exist\n", "mm_start");
                    return;
                }
                time_active.mm_start = (uint8_t)json_object_get_int(o);
                printf("\nmm_start is: %d", time_active.mm_start);

                if (!json_object_object_get_ex(new_obj, "hh_end", &o))
                {
                    printf("Field %s does not exist\n", "hh_end");
                    return;
                }
                time_active.hh_end = (uint8_t)json_object_get_int(o);
                printf("\nhh_end is: %d", time_active.hh_end);

                if (!json_object_object_get_ex(new_obj, "mm_end", &o))
                {
                    printf("Field %s does not exist\n", "mm_end");
                    return;
                }
                time_active.mm_end = (uint8_t)json_object_get_int(o);
                printf("\nmm_end is: %d", time_active.mm_end);
                Write_struct_time_active_toFile(time_active_file, "/home/pi/time_active.txt", time_active);

                char data_send_active_time[1000] = {0};
                int data_send_active_time_len = sprintf(data_send_active_time, TEMPLATE_TIME_ACTIVE, CMD_CPU_TO_QT_TIME_ACTIVE,
                                                        time_active.hh_start,
                                                        time_active.mm_start,
                                                        time_active.hh_end,
                                                        time_active.mm_end);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_active_time_len, data_send_active_time);
                printf("Da send time active to QT OK\r\n");
                send_struct(CMD_CPU_TO_MASTER_TIME_ACTIVE, (uint8_t *)&time_active, sizeof(time_active_t));
                printf("Da send time active to master OK\r\n");
                flag_control.time_setting_infor = 1;
                printf("Da send time active to server OK\r\n");
                break;
            }

            case CMD_CPU_TO_QT_TIME_ACTIVE:
            {
                flag_send_data_to_qt.is_active_time = 1;
                break;
            }

            case CMD_QT_TO_CPU_SCHEDULE:
            {
                int num_side = 0;
                printf("save schedule to file\r\n");
                if (!json_object_object_get_ex(new_obj, "xanh1", &o))
                {
                    printf("Field %s does not exist\n", "xanh1");
                    //    return;
                    schedule.xanh1 = 0;
                    num_side = 0;
                }
                else
                    schedule.xanh1 = (uint8_t)json_object_get_int(o);
                type_mtfc_quick_cycle_config.green_t[0] = schedule.xanh1;
                printf("\nxanh1 is: %d", schedule.xanh1);

                if (!json_object_object_get_ex(new_obj, "xanh2", &o))
                {
                    printf("Field %s does not exist\n", "xanh2");
                    //    return;
                    schedule.xanh2 = 0;
                    num_side = 1;
                }
                else
                    schedule.xanh2 = (uint8_t)json_object_get_int(o);
                type_mtfc_quick_cycle_config.green_t[1] = schedule.xanh2;
                printf("\nxanh2 is: %d", schedule.xanh2);

                if (!json_object_object_get_ex(new_obj, "xanh3", &o))
                {
                    printf("Field %s does not exist\n", "xanh3");
                    //    return;
                    schedule.xanh3 = 0;
                    if ((schedule.xanh2 != 0))
                        num_side = 2;
                }
                else
                    schedule.xanh3 = (uint8_t)json_object_get_int(o);
                type_mtfc_quick_cycle_config.green_t[2] = schedule.xanh3;
                printf("\nxanh3 is: %d", schedule.xanh3);

                if (!json_object_object_get_ex(new_obj, "xanh4", &o))
                {
                    printf("Field %s does not exist\n", "xanh4");
                    //    return;
                    schedule.xanh4 = 0;
                    if ((schedule.xanh2 != 0) && (schedule.xanh3 != 0))
                        num_side = 3;
                }
                else
                {
                    schedule.xanh4 = (uint8_t)json_object_get_int(o);
                    num_side = 4;
                }
                type_mtfc_quick_cycle_config.green_t[3] = schedule.xanh4;
                printf("\nxanh4 is: %d", schedule.xanh4);
                if (!json_object_object_get_ex(new_obj, "vang", &o))
                {
                    printf("Field %s does not exist\n", "vang");
                    return;
                }
                schedule.vang = (uint8_t)json_object_get_int(o);
                type_mtfc_quick_cycle_config.yellow_t = schedule.vang;
                printf("\nvang is: %d", schedule.vang);

                if (!json_object_object_get_ex(new_obj, "giaitoa", &o))
                {
                    printf("Field %s does not exist\n", "giaitoa");
                    return;
                }
                schedule.giaitoa = (uint8_t)json_object_get_int(o);
                type_mtfc_quick_cycle_config.clearance_t = schedule.giaitoa;
                printf("\ngiaitoa is: %d\n", schedule.giaitoa);
                Write_struct_schedule_toFile(schedule_file, "/home/pi/schedule.txt", schedule);

                char data_send_schedule[1000] = {0};
                int data_send_schedule_len = sprintf(data_send_schedule, TEMPLATE_SCHEDULE, CMD_CPU_TO_QT_SCHEDULE,
                                                     schedule.xanh1,
                                                     schedule.xanh2,
                                                     schedule.xanh3,
                                                     schedule.xanh4,
                                                     schedule.vang,
                                                     schedule.giaitoa);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_schedule_len, data_send_schedule);
                printf("Da send schedule to QT OK\r\n");
                type_mtfc_quick_cycle_config.num_side = num_side;
                send_struct(CMD_CPU_TO_MASTER_CYCLE_FORM, (uint8_t *)&type_mtfc_quick_cycle_config, sizeof(type_mtfc_quick_cycle_config_t));
                printf("Da send schedule to master OK\r\n");
                flag_control.sttrategy_setting_infor = 1;
                printf("Da send schedule to server OK\r\n");
                break;
            }

            case CMD_CPU_TO_QT_SCHEDULE:
            {
                char data_send_schedule[1000] = {0};
                int data_send_schedule_len = sprintf(data_send_schedule, TEMPLATE_SCHEDULE, CMD_CPU_TO_QT_SCHEDULE,
                                                     schedule.xanh1,
                                                     schedule.xanh2,
                                                     schedule.xanh3,
                                                     schedule.xanh4,
                                                     schedule.vang,
                                                     schedule.giaitoa);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_schedule_len, data_send_schedule);
                printf("%s\r\n", data_send_schedule);
                printf("Da send schedule to QT OK\r\n");
                break;
            }

            case CMD_QT_TO_CPU_SOPHA:
            {
                printf("save sopha to file\r\n");
                if (!json_object_object_get_ex(new_obj, "sopha", &o))
                {
                    printf("Field %s does not exist\n", "sopha");
                    return;
                }
                sopha.sopha = (uint8_t)json_object_get_int(o);
                printf("\nxanh1 is: %d", sopha.sopha);

                Write_struct_sopha_toFile(schedule_file, "/home/pi/sopha.txt", sopha);

                char data_send[1000] = {0};
                int data_send_len = sprintf(data_send, TEMPLATE_SOPHA, CMD_CPU_TO_QT_SOPHA, sopha.sopha);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                printf("%s\r\n", data_send);
                printf("Da send sopha to QT OK\r\n");
                send_byte(CMD_CPU_TO_MASTER_SOPHA, sopha.sopha);
                printf("Da send sopha to master OK\r\n");
                flag_control.route_setting_infor = 1;
                printf("Da send sopha to server OK\r\n");
                break;
            }

            case CMD_CPU_TO_QT_SOPHA:
            {
                char data_send[1000] = {0};
                int data_send_len = sprintf(data_send, TEMPLATE_SOPHA, CMD_CPU_TO_QT_SOPHA, sopha.sopha);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                printf("%s\r\n", data_send);
                printf("Da send sopha to QT OK\r\n");

                break;
            }

            case CMD_QT_TO_CPU_DASHBOARD:
            {
                char data_send1[1000] = {0};
                int data_send_len1 = sprintf(data_send1, TEMPLATE_SOPHA, CMD_CPU_TO_QT_SOPHA, sopha.sopha);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len1, data_send1);
                printf("%s\r\n", data_send1);
                printf("Da send sopha to QT OK\r\n");

                char data_send2[1000] = {0};
                int data_send_len2 = sprintf(data_send2, TEMPLATE_SCHEDULE, CMD_CPU_TO_QT_SCHEDULE,
                                             schedule.xanh1,
                                             schedule.xanh2,
                                             schedule.xanh3,
                                             schedule.xanh4,
                                             schedule.vang,
                                             schedule.giaitoa);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len2, data_send2);
                printf("%s\r\n", data_send2);
                printf("Da send schedule to QT OK\r\n");

                char data_send3[1000] = {0};
                int data_send_len3 = sprintf(data_send3, TEMPLATE_TIME_ACTIVE, CMD_CPU_TO_QT_TIME_ACTIVE,
                                             time_active.hh_start,
                                             time_active.mm_start,
                                             time_active.hh_end,
                                             time_active.mm_end);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_send3);
                printf("%s\r\n", data_send3);
                printf("Da send time active to QT OK\r\n");

                for (int i = 0; i < 8; i++)
                {
                    char data_send4[1000] = {0};
                    int data_send_len = sprintf(data_send4, TEMPLATE_CARD_INFOR, CMD_CPU_TO_QT_CARD_INFOR,
                                                i,
                                                type_cardConfig.index[i].sn,
                                                type_cardConfig.index[i].imei,
                                                type_cardConfig.index[i].phase,
                                                type_cardConfig.index[i].is_railway_enabled,
                                                type_cardConfig.index[i].is_walking_enabled,
                                                type_cardConfig.index[i].is_dependent_phase,
                                                type_cardConfig.index[i].time_delay_dependent_phase);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send4);
                    printf("%s\r\n", data_send4);
                    printf("%s", "Đã send card infor to QT OK\n");
                }
                if (modeactive != 2)
                {
                    char data_send5[1000] = {0};
                    int data_send_len = sprintf(data_send5, TEMPLATE_MODE, CMD_CPU_TO_QT_MODE_ACTIVE, modeactive);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send5);
                    printf("%s\r\n", data_send5);
                    printf("%s", "Đã send mode on/off to QT OK\n");
                }
                else
                {
                    char data_send5[1000] = {0};
                    int data_send_len = sprintf(data_send5, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send5);
                    printf("%s\r\n", data_send5);
                    printf("%s", "Đã send mode on/off to QT OK\n");
                }

                break;
            }

            case CMD_QT_TO_CPU_SET_RTC_MANUAL:
            {

                if (!json_object_object_get_ex(new_obj, "years", &o))
                {
                    return;
                }

                type_date_time.years = json_object_get_int(o); // Lay du lieu cai nam
                if (!json_object_object_get_ex(new_obj, "month", &o))
                {
                    return;
                }
                type_date_time.month = json_object_get_int(o); // Lay du lieu cai thang
                if (!json_object_object_get_ex(new_obj, "date", &o))
                {
                    return;
                }
                type_date_time.date = json_object_get_int(o); // Lay du lieu cai ngay
                if (!json_object_object_get_ex(new_obj, "day", &o))
                {
                    return;
                }
                type_date_time.day = json_object_get_int(o); // Lay du lieu thu
                if (!json_object_object_get_ex(new_obj, "hh", &o))
                {
                    return;
                }
                type_date_time.hh = json_object_get_int(o); // Lay du lieu cai gio
                if (!json_object_object_get_ex(new_obj, "mm", &o))
                {
                    return;
                }
                type_date_time.mm = json_object_get_int(o); // Lay du lieu cai gio
                type_date_time.ss = 0;

                printf("Time set ting: %d/%d/%d %d:%d:%d\r\n", type_date_time.years,
                       type_date_time.month,
                       type_date_time.date,
                       type_date_time.hh,
                       type_date_time.mm, 0);

                char timeSeting_temp[200];
                // sudo date --set="2012/12/12 12:12:12"
                // sudo hwclock --set --date "2012/12/12 12:12:12"
                int len = sprintf(timeSeting_temp, "sudo hwclock --set --date \"%d/%d/%d %d:%d:%d +07\"", type_date_time.years,
                                  type_date_time.month,
                                  type_date_time.date,
                                  type_date_time.hh,
                                  type_date_time.mm, 0);
                printf("%s\r\n", timeSeting_temp);
                system(timeSeting_temp);
                delay(100);
                memset(timeSeting_temp, 0, sizeof(timeSeting_temp));
                len = sprintf(timeSeting_temp, "sudo date --set=\"%d/%d/%d %d:%d:%d\"", type_date_time.years,
                              type_date_time.month,
                              type_date_time.date,
                              type_date_time.hh,
                              type_date_time.mm, 0);
                printf("%s\r\n", timeSeting_temp);
                system(timeSeting_temp);
                delay(100);
                char data[1000];
                len = sprintf(data, "CPU-{\"CMD\": %d}", CMD_QT_TO_CPU_SET_RTC_MANUAL);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                memset(data, 0, sizeof(data));
                printf("send status set new time success\r\n");
                send_struct(CMD_CPU_TO_MASTER_SET_RTC_MANUAL, (uint8_t *)&type_date_time, sizeof(type_date_time_t));
                printf("Da send RTC to master OK\r\n");
                break;
            }

            case CMD_QT_TO_CPU_MODE_AUTO_RTC: // mode
                // printf("Hoi auto mode RTC\r\n");
                if (get_State_Cmd_Terminal("timedatectl", "NTP service: active"))
                {
                    char data[1000];
                    // printf("Founded keyword\r\n");
                    int len = sprintf(data, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_MODE_AUTO_RTC, 1);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                    memset(data, 0, sizeof(data));
                }
                else
                {
                    char data[1000];
                    // printf("Not found keyword\r\n");
                    int len = sprintf(data, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_MODE_AUTO_RTC, 0);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                    memset(data, 0, sizeof(data));
                }
                break;

            case CMD_QT_TO_CPU_CONTROL_UPDATE_NTP_RTC:
                if (!json_object_object_get_ex(new_obj, "NTP", &o))
                {
                    printf("Field %s does not exist\n", "NTP");
                    return;
                }
                if (json_object_get_int(o))
                {
                    printf("NTP_update: active\r\n");
                    system("sudo timedatectl set-ntp true");
                    char data[1000] = {0};
                    int len = sprintf(data, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_CONTROL_UPDATE_NTP_RTC, 1);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                    memset(data, 0, sizeof(data));

                    // printf("Founded keyword\r\n");
                    len = sprintf(data, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_MODE_AUTO_RTC, 1);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                    memset(data, 0, sizeof(data));
                }
                else
                {
                    printf("NTP update: inactive\r\n");
                    char data[1000] = {0};
                    system("sudo timedatectl set-ntp false");
                    int len = sprintf(data, "CPU-{\"CMD\": %d,\"Value\": %d}", CMD_QT_TO_CPU_CONTROL_UPDATE_NTP_RTC, 0);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                    memset(data, 0, sizeof(data));

                    // printf("Not found keyword\r\n");
                    len = sprintf(data, "CPU-{\"CMD\": %d, \"Value\": %d}", CMD_QT_TO_CPU_MODE_AUTO_RTC, 0);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
                    memset(data, 0, sizeof(data));
                }
                break;

            case CMD_QT_TO_CPU_MODE_CROSS: // mode
                if (!json_object_object_get_ex(new_obj, "mode", &o))
                {
                    return;
                }
                mode_cross = json_object_get_int(o); // Lay du lieu cai nam
                send_byte(CMD_CPU_TO_MASTER_MODE_CROSS, mode_cross);
                printf("Da send mode cross to master OK\r\n");
                flag_control.pinout_setting_infor = 1;
                printf("Da send mode cross to server OK\r\n");
                break;

            case CMD_CPU_TO_QT_CARD_INFOR2:
                for (int i = 0; i < 8; i++)
                {
                    char data_send4[1000] = {0};
                    int data_send_len = sprintf(data_send4, TEMPLATE_CARD_INFOR, CMD_CPU_TO_QT_CARD_INFOR,
                                                i,
                                                type_cardConfig.index[i].sn,
                                                type_cardConfig.index[i].imei,
                                                type_cardConfig.index[i].phase,
                                                type_cardConfig.index[i].is_railway_enabled,
                                                type_cardConfig.index[i].is_walking_enabled,
                                                type_cardConfig.index[i].is_dependent_phase,
                                                type_cardConfig.index[i].time_delay_dependent_phase);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send4);
                    printf("%s\r\n", data_send4);
                }
                flag_control.card_setting_infor = 1; ///<==== Send card infor to server
                printf("%s", "Đã send card infor to QT OK\n");
                break;

            case CMD_QT_TO_CPU_CARD_INFOR:
                // printf("Da send mode cross to master OK\r\n");
                if (!json_object_object_get_ex(new_obj, "card1", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 1)
                {
                    if (!json_object_object_get_ex(new_obj, "phase1", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[0].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled1", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[0].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled1", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[0].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase1", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[0].is_dependent_phase = json_object_get_int(o);

                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard1.txt", type_cardConfig.index[0]);
                    printf("%s\r\n", "Da save 1");
                }
                if (!json_object_object_get_ex(new_obj, "card2", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 2)
                {
                    if (!json_object_object_get_ex(new_obj, "phase2", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[1].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled2", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[1].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled2", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[1].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase2", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[1].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard2.txt", type_cardConfig.index[1]);
                    printf("%s\r\n", "Da save 2");
                }
                if (!json_object_object_get_ex(new_obj, "card3", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 3)
                {
                    if (!json_object_object_get_ex(new_obj, "phase3", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[2].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled3", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[2].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled3", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[2].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase3", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[2].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard3.txt", type_cardConfig.index[2]);
                    printf("%s\r\n", "Da save 3");
                }
                if (!json_object_object_get_ex(new_obj, "card4", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 4)
                {
                    if (!json_object_object_get_ex(new_obj, "phase4", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[3].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled4", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[3].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled4", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[3].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase4", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[3].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard4.txt", type_cardConfig.index[3]);
                    printf("%s\r\n", "Da save 4");
                }
                if (!json_object_object_get_ex(new_obj, "card5", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 5)
                {
                    if (!json_object_object_get_ex(new_obj, "phase5", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[4].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled5", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[4].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled5", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[4].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase5", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[4].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard5.txt", type_cardConfig.index[4]);
                    printf("%s\r\n", "Da save 5");
                }
                if (!json_object_object_get_ex(new_obj, "card6", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 6)
                {
                    if (!json_object_object_get_ex(new_obj, "phase6", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[5].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled6", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[5].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled6", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[5].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase6", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[5].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard6.txt", type_cardConfig.index[5]);
                    printf("%s\r\n", "Da save 6");
                }
                if (!json_object_object_get_ex(new_obj, "card7", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 7)
                {
                    if (!json_object_object_get_ex(new_obj, "phase7", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[6].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled7", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[6].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled7", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[6].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase7", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[6].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard7.txt", type_cardConfig.index[6]);
                    printf("%s\r\n", "Da save 7");
                }
                if (!json_object_object_get_ex(new_obj, "card8", &o))
                {
                    return;
                }
                if (json_object_get_int(o) == 8)
                {
                    if (!json_object_object_get_ex(new_obj, "phase8", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[7].phase = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_railway_enabled8", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[7].is_railway_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_walking_enabled8", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[7].is_walking_enabled = json_object_get_int(o);
                    if (!json_object_object_get_ex(new_obj, "is_dependent_phase8", &o))
                    {
                        return;
                    }
                    type_cardConfig.index[7].is_dependent_phase = json_object_get_int(o);
                    Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard8.txt", type_cardConfig.index[7]);
                    printf("%s\r\n", "Da save 8");
                }
                for (int i = 0; i < 8; i++)
                {
                    type_mtfc_one_card_config.slot = i + 1;
                    type_mtfc_one_card_config.phase = type_cardConfig.index[i].phase;
                    type_mtfc_one_card_config.is_railway = type_cardConfig.index[i].is_railway_enabled;
                    type_mtfc_one_card_config.is_walking = type_cardConfig.index[i].is_walking_enabled;
                    type_mtfc_one_card_config.is_dependent_phase = type_cardConfig.index[i].is_dependent_phase;
                    send_struct(CMD_CPU_TO_MASTER_CHANGE_PHASE_1, (uint8_t *)&type_mtfc_one_card_config, sizeof(type_mtfc_one_card_config_t));

                    printf("%s:%d\r\n", "Da send den masterxxxxxxxxxxxxxxxx", i + 1);
                    delay(3000);
                }
                flag_control.card_setting_infor = 1; ///<==== Send card infor to server
                printf("%s", "Đã send card infor to QT OK\n");
                break;
            case CMD_QT_TO_CPU_SCHEDULE_ALL:
                send_byte(CMD_CPU_TO_MASTER_SCHEDULE, 1);
                printf("%s", "Yeu Cau Master gui lich chay tu duoi len CPU\n");
                break;
            case CMD_QT_TO_CPU_SCHEDULE_TIME:
                // Temple 1
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_00_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[0].t_begin_apply.hour = json_object_get_int(o);
                // printf("got data cycle time 0: %02d\r\n", cycle.time.index[0].select_point[0].t_begin_apply.hour);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_00_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[0].t_begin_apply.minute = json_object_get_int(o);
                // printf("got data cycle time 0: %02d\r\n", cycle.time.index[0].select_point[0].t_begin_apply.hour);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_00_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[0].index_cycle_form = json_object_get_int(o);
                // printf("got data cycle time 0: %02d\r\n", cycle.time.index[0].select_point[0].t_begin_apply.hour);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_01_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[1].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_01_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[1].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_01_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[1].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_02_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[2].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_02_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[2].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_02_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[2].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_03_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[3].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_03_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[3].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_03_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[3].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_04_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[4].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_04_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[4].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_04_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[0].select_point[4].index_cycle_form = json_object_get_int(o);

                // Temple 2
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_10_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[0].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_10_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[0].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_10_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[0].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_11_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[1].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_11_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[1].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_11_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[1].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_12_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[2].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_12_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[2].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_12_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[2].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_13_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[3].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_13_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[3].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_13_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[3].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_14_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[4].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_14_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[4].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_14_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[1].select_point[4].index_cycle_form = json_object_get_int(o);

                // Temple 3
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_20_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[0].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_20_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[0].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_20_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[0].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_21_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[1].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_21_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[1].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_21_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[1].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_22_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[2].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_22_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[2].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_22_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[2].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_23_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[3].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_23_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[3].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_23_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[3].index_cycle_form = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_24_begin_h", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[4].t_begin_apply.hour = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_24_begin_m", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[4].t_begin_apply.minute = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_timeline_24_begin_c", &o))
                {
                    return;
                }
                cycle.time.index[2].select_point[4].index_cycle_form = json_object_get_int(o);

                send_struct(CMD_CPU_TO_MASTER_SCHEDULE_TIME_UPDATE, (uint8_t *)&cycle.time, sizeof(type_time_lamp_form_t));

                /**************************ghi chien luoc vao file txt**********************************/
                /**************************ghi chien luoc vao file txt**********************************/
                /**************************ghi chien luoc vao file txt**********************************/
                Write_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", cycle);
                printf("Đã send update time form to master Ok\n");
                /*******************************update len app cu***************************************/
                matching_data_to_old_app();

                updateServerSchedule();

                // flag_control.sttrategy_setting_infor = 1;
                break;

            case CMD_QT_TO_CPU_SCHEDULE_CYCLE_UPDATE:
            {
                ///////Cycle 1
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_0", &o))
                {
                    return;
                }
                cycle.cycle_1[0] = json_object_get_int(o);

                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_1", &o))
                {
                    return;
                }
                cycle.cycle_1[1] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_2", &o))
                {
                    return;
                }
                cycle.cycle_1[2] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_3", &o))
                {
                    return;
                }
                cycle.cycle_1[3] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_4", &o))
                {
                    return;
                }
                cycle.cycle_1[4] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_5", &o))
                {
                    return;
                }
                cycle.cycle_1[5] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_6", &o))
                {
                    return;
                }
                cycle.cycle_1[6] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_7", &o))
                {
                    return;
                }
                cycle.cycle_1[7] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_8", &o))
                {
                    return;
                }
                cycle.cycle_1[8] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_1_9", &o))
                {
                    return;
                }
                cycle.cycle_1[9] = json_object_get_int(o);
                ///////Cycle 2
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_0", &o))
                {
                    return;
                }
                cycle.cycle_2[0] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_1", &o))
                {
                    return;
                }
                cycle.cycle_2[1] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_2", &o))
                {
                    return;
                }
                cycle.cycle_2[2] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_3", &o))
                {
                    return;
                }
                cycle.cycle_2[3] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_4", &o))
                {
                    return;
                }
                cycle.cycle_2[4] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_5", &o))
                {
                    return;
                }
                cycle.cycle_2[5] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_6", &o))
                {
                    return;
                }
                cycle.cycle_2[6] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_7", &o))
                {
                    return;
                }
                cycle.cycle_2[7] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_8", &o))
                {
                    return;
                }
                cycle.cycle_2[8] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_2_9", &o))
                {
                    return;
                }
                cycle.cycle_2[9] = json_object_get_int(o);
                ///////Cycle 3
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_0", &o))
                {
                    return;
                }
                cycle.cycle_3[0] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_1", &o))
                {
                    return;
                }
                cycle.cycle_3[1] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_2", &o))
                {
                    return;
                }
                cycle.cycle_3[2] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_3", &o))
                {
                    return;
                }
                cycle.cycle_3[3] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_4", &o))
                {
                    return;
                }
                cycle.cycle_3[4] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_5", &o))
                {
                    return;
                }
                cycle.cycle_3[5] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_6", &o))
                {
                    return;
                }
                cycle.cycle_3[6] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_7", &o))
                {
                    return;
                }
                cycle.cycle_3[7] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_8", &o))
                {
                    return;
                }
                cycle.cycle_3[8] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_3_9", &o))
                {
                    return;
                }
                cycle.cycle_3[9] = json_object_get_int(o);
                ///////Cycle 4
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_0", &o))
                {
                    return;
                }
                cycle.cycle_4[0] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_1", &o))
                {
                    return;
                }
                cycle.cycle_4[1] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_2", &o))
                {
                    return;
                }
                cycle.cycle_4[2] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_3", &o))
                {
                    return;
                }
                cycle.cycle_4[3] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_4", &o))
                {
                    return;
                }
                cycle.cycle_4[4] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_5", &o))
                {
                    return;
                }
                cycle.cycle_4[5] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_6", &o))
                {
                    return;
                }
                cycle.cycle_4[6] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_7", &o))
                {
                    return;
                }
                cycle.cycle_4[7] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_8", &o))
                {
                    return;
                }
                cycle.cycle_4[8] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_4_9", &o))
                {
                    return;
                }
                cycle.cycle_4[9] = json_object_get_int(o);
                ///////Cycle 5
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_0", &o))
                {
                    return;
                }
                cycle.cycle_5[0] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_1", &o))
                {
                    return;
                }
                cycle.cycle_5[1] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_2", &o))
                {
                    return;
                }
                cycle.cycle_5[2] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_3", &o))
                {
                    return;
                }
                cycle.cycle_5[3] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_4", &o))
                {
                    return;
                }
                cycle.cycle_5[4] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_5", &o))
                {
                    return;
                }
                cycle.cycle_5[5] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_6", &o))
                {
                    return;
                }
                cycle.cycle_5[6] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_7", &o))
                {
                    return;
                }
                cycle.cycle_5[7] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_8", &o))
                {
                    return;
                }
                cycle.cycle_5[8] = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "cycle_cycle_5_9", &o))
                {
                    return;
                }
                cycle.cycle_5[9] = json_object_get_int(o);

                send_struct(44, (uint8_t *)&cycle, sizeof(cycle_t));
                delay(50);
                /**************************ghi chien luoc vao file txt**********************************/
                /**************************ghi chien luoc vao file txt**********************************/
                /**************************ghi chien luoc vao file txt**********************************/
                Write_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", cycle);
                /*******************************update len app cu***************************************/
                matching_data_to_old_app();
                updateServerSchedule();
                printf("%s", "Đã send check schedule infor to Master OK\n");
                // flag_control.sttrategy_setting_infor = 1;
            }
            break;

            case CMD_QT_TO_CPU_SCHEDULE_DAYS_UPDATE:
                if (!json_object_object_get_ex(new_obj, "day", &o))
                {
                    return;
                }
                day = json_object_get_int(o);
                if (!json_object_object_get_ex(new_obj, "dayform", &o))
                {
                    return;
                }
                mtfc_schedule.days.index[day] = json_object_get_int(o);
                send_struct(CMD_CPU_TO_MASTER_SCHEDULE_DAYS_UPDATE, (uint8_t *)&mtfc_schedule.days, sizeof(type_days_lamp_form_t));
                printf("Da send update day form to master OK\r\n");
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[0]);
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[1]);
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[2]);
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[3]);
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[4]);
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[5]);
                // printf("mtfc_schedule.days.index[0] OK: %d\r\n",mtfc_schedule.days.index[6]);

                flag_control.sttrategy_setting_infor = 1;
                printf("Da send update day form to server OK\r\n");
                break;
            case CMD_QT_TO_CPU_TIME_SETTING:
                // send_byte(CMD_CPU_TO_MASTER_SCHEDULE, 1);
                if (!json_object_object_get_ex(new_obj, "phase", &o))
                {
                    return;
                }
                time_setting.phase = json_object_get_int(o); // Lay du lieu cai nam
                if (!json_object_object_get_ex(new_obj, "time_delay_dependent_phase", &o))
                {
                    return;
                }
                time_setting.time_delay_dependent_phase = json_object_get_int(o); // Lay du lieu cai nam
                if (!json_object_object_get_ex(new_obj, "time_delay_on_railway", &o))
                {
                    return;
                }
                time_setting.time_delay_on_railway = json_object_get_int(o); // Lay du lieu cai nam
                if (!json_object_object_get_ex(new_obj, "time_delay_off_railway", &o))
                {
                    return;
                }
                time_setting.time_delay_off_railway = json_object_get_int(o); // Lay du lieu cai nam
                if (!json_object_object_get_ex(new_obj, "time_walking", &o))
                {
                    return;
                }
                time_setting.time_walking = json_object_get_int(o); // Lay du lieu cai nam
                send_struct(CMD_CPU_TO_MASTER_TIME_SETTING, (uint8_t *)&time_setting, sizeof(time_setting_t));
                printf("%s", "Đã send time setting to Master OK\n");
                break;
            case CMD_QT_TO_CPU_OPEN_SERVER:
                printf("Open connect to server!\r\n");
                if (!json_object_object_get_ex(new_obj, "server", &o))
                {
                    printf("Field %s does not exist\n", "server");
                    return;
                }
                setting.server = (uint8_t)json_object_get_int(o);
                Write_struct_setting_toFile(setting_file, "/home/pi/setting.txt", setting);
                break;

            case CMD_CPU_TO_QT_OPEN_SERVER:
                printf("Open connect to QT!\r\n");
                char data[1000] = {0};
                int data_len = sprintf(data, TEMPLATE_SETTING, CMD_CPU_TO_QT_OPEN_SERVER, setting.server);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_len, data);
                printf("%s\r\n", data);
                break;

            case CMD_QT_TO_CPU_OFF_SCREEN:
                printf("Tat man hinh\r\n");
                system("sudo vcgencmd display_power 0");
                break;
            case CMD_QT_TO_CPU_ON_SCREEN:
                printf("Bat man hinh\r\n");
                system("sudo vcgencmd display_power 1");
                break;
            case CMD_QT_TO_CPU_DKTB:
                printf("Dang ky thiet bi\r\n");
                printf("Mac ID: %s\r\n", mac_final);
                if (!json_object_object_get_ex(new_obj, "url", &o))
                {
                    printf("Field %s does not exist\n", "url");
                    return;
                }
                // char url[100] = "http://api.sitechlighting.one/device";
                char *tmp = (char *)malloc(100);
                strcpy(tmp, json_object_get_string(o));
                memcpy(&setting.urlmqtt, tmp, 100);
                free(tmp);
                Write_struct_setting_toFile(setting_file, "/home/pi/setting.txt", setting);
                printf("setting.urlmqtt: %s\r\n", setting.urlmqtt);

                char message[10000];
                snprintf(message, sizeof(message), "%s traffic.%s/api/devices/register %s \"%s\", %s \"%s\"%s", "curl -X POST", setting.urlmqtt, "-H \"Content-Type: application/json\" -d '{\"key\":", mac_final, "\"password\":", mac_final, "}'");
                printf("%s\r\n", message);
                system(message);
                break;
            default:
                break;
            }
        }
    }
}
//**********************************************************************CAC HAM WEBSOCKET*********************************************//
#pragma endregion
#pragma region LUONG XU LY KET NOI APP THEO PHUONG THUC WEBSOCKET
void uwsc_onopen_server(struct uwsc_client *mtfc_client_sv)
{
    uwsc_log_info("onopen socket connect app\n");
    is_flag_opend_weboscket_online = true;
}

void uwsc_onmessage_server(struct uwsc_client *mtfc_client_sv,
                           void *type_mtfc_working_message, size_t len, bool binary)
{
    if (binary)
    {
        size_t i;

        for (i = 0; i < len; i++)
        {
            // printf("%02hhX ", p[i]);
            if (i % 16 == 0 && i > 0)
                puts("");
        }
        puts("");
    }
    else
    {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
        printf("[%.*s]\n", (int)len, (char *)type_mtfc_working_message);
#endif
    }

    MainMessage *mainMessage = main_message__unpack(NULL, len, type_mtfc_working_message);
    if (mainMessage == NULL)
    {
        printf("Failed to unpack main message\n");
        return;
    }
    // -------------------------------------------------------------------------------- Kiem tra dang ki thanh cong khong--------------------------------------------------------------------------//
    // -------------------------------------------------------------------------------- Kiem tra dang ki thanh cong khong--------------------------------------------------------------------------//
    // -------------------------------------------------------------------------------- Kiem tra dang ki thanh cong khong--------------------------------------------------------------------------//
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->deviceauthmessage &&
        mainMessage->devicemessage->deviceauthmessage->deviceregisterresponse)
    {
        DeviceRegisterResponse *deviceRegisterResponse = mainMessage->devicemessage->deviceauthmessage->deviceregisterresponse;
        printf("Has Register with code %d and message %s\r\n", deviceRegisterResponse->statuscode->code, deviceRegisterResponse->statuscode->message);
        if (deviceRegisterResponse->statuscode->code == 0)
        {
            printf("Device register success, device id = %s\r\n", deviceRegisterResponse->deviceid);
            if (deviceRegisterResponse != NULL)
            {
                if (device_id != NULL)
                {
                    free(device_id);
                }
                device_id = strdup(deviceRegisterResponse->deviceid);
                printf("Assign device id success\r\n");
                printf("Device ID: %s\r\n", device_id);
            }
            is_register_success = true;
        }
    }
    // -------------------------------------------------------------------------------- Kiem tra login thanh cong khong--------------------------------------------------------------------------//
    // -------------------------------------------------------------------------------- Kiem tra login thanh cong khong--------------------------------------------------------------------------//
    // -------------------------------------------------------------------------------- Kiem tra login thanh cong khong--------------------------------------------------------------------------//
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->deviceauthmessage &&
        mainMessage->devicemessage->deviceauthmessage->deviceloginresponse)
    {
        DeviceLoginResponse *deviceLoginResponse = mainMessage->devicemessage->deviceauthmessage->deviceloginresponse;
        printf("Has login response with code = %d and message = %s\n", deviceLoginResponse->statuscode->code, deviceLoginResponse->statuscode->message);

        if (deviceLoginResponse->statuscode->code == 0 || deviceLoginResponse->statuscode->code == 217)
        {
            is_login_success = true;
            printf("Annouce CPU online login\r\n");
            memset((char *)online.online, '\0', 32);
            sprintf(online.online, "%s", "clound_connect");
            printf("buffer request: %s\r\n", online.online);
            delay(500);
            memset((char *)online.online, '\0', 32);
        }
        else
        {
            is_login_success = false;
            printf("Annouce CPU offline login\r\n");
            memset((char *)online.online, '\0', 32);
            sprintf(online.online, "%s", "clound_disconnect");
            printf("buffer request: %s\r\n", online.online);
            delay(500);
            memset((char *)online.online, '\0', 32);
        }
    }

    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->requirestreamdevicerequest)
    {
        RequireStreamDeviceRequest *requireStreamDeviceRequest = mainMessage->devicemessage->userdevicemessage->requirestreamdevicerequest;
        printf("Has response connect cloud with code and message %d, device id: %s\r\n", requireStreamDeviceRequest->enable, requireStreamDeviceRequest->deviceid);
        if (requireStreamDeviceRequest->enable)
        {
            printf("App connected\r\n");
            isSoftConnected = true;
        }
        else
        {
            printf("App disconnected\r\n");
            isSoftConnected = false;
        }

        if (isSoftConnected)
        {
            printf("Annouce CPU app connected\r\n");
            memset((char *)app_connected.app_connected, '\0', 32);
            sprintf(app_connected.app_connected, "%s", "app_connect");
            printf("buffer request: %s\r\n", app_connected.app_connected);
            status_connect_server_app = 1;
            char data_tmp[100] = {0};
            int data_send_len3 = sprintf(data_tmp, TEMPLATE_STATUS_CONNECT_MQTT, CMD_CPU_TO_QT_STATUS_CONNECT_MQTT,
                                        status_connect_server_app);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_tmp);
            printf("%s\r\n", data_tmp);
        }
        else
        {
            printf("Annouce CPU app disconnected\r\n");
            memset((char *)app_disconnected.app_disconnected, '\0', 32);
            sprintf(app_disconnected.app_disconnected, "%s", "app_disconnect");
            printf("buffer request: %s\r\n", app_disconnected.app_disconnected);
            status_connect_server_app = 0;
            char data_tmp[100] = {0};
            int data_send_len3 = sprintf(data_tmp, TEMPLATE_STATUS_CONNECT_MQTT, CMD_CPU_TO_QT_STATUS_CONNECT_MQTT,
                                        status_connect_server_app);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_tmp);
            printf("%s\r\n", data_tmp);
        }
    }

    // do bo thoi gian tu app
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->synctimedevicerequest)
    {
        SyncTimeDeviceRequest *SyncTimeDeviceRequest = mainMessage->devicemessage->userdevicemessage->synctimedevicerequest;
        printf("Sync Time Request\r\n");
        time_t now;
        struct tm *timeinfo;
        time(&now);
        timeinfo = localtime(&now);
        printf("Time : %s", asctime(timeinfo));

        type_date_time.day = timeinfo->tm_wday;
        type_date_time.hh = timeinfo->tm_hour;
        type_date_time.mm = timeinfo->tm_min;
        type_date_time.ss = timeinfo->tm_sec;
        type_date_time.date = timeinfo->tm_mday;
        type_date_time.month = timeinfo->tm_mon + 1;
        type_date_time.years = timeinfo->tm_year + 1900;

        send_struct(CMD_CPU_TO_MASTER_SET_RTC_MANUAL, (uint8_t *)&type_date_time, sizeof(type_date_time_t));
    }

    // Lay cycle CPU
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->deviceupdatemessage &&
        mainMessage->devicemessage->deviceupdatemessage->pingwebsocketdeviceresponse)
    {
        PingWebsocketDeviceResponse *pingWebsocketDeviceResponse = mainMessage->devicemessage->deviceupdatemessage->pingwebsocketdeviceresponse;
        keepAlliveCloud--;
        printf("Value kepp allive response: %d\r\n", keepAlliveCloud);
    }

    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->setcycleconfigdevicerequest)
    {
        SetCycleConfigDeviceRequest *setCycleConfigDeviceRequest = mainMessage->devicemessage->userdevicemessage->setcycleconfigdevicerequest;
        printf("Write cycle for CPU =======================> Success\r\n");
        type_mtfc_quick_cycle_config.num_side = setCycleConfigDeviceRequest->numphase;
        type_mtfc_quick_cycle_config.yellow_t = setCycleConfigDeviceRequest->yellowtime;
        type_mtfc_quick_cycle_config.clearance_t = setCycleConfigDeviceRequest->clearancetime;
        copy_u32_array_to_u8_array(setCycleConfigDeviceRequest->greentime, type_mtfc_quick_cycle_config.green_t, type_mtfc_quick_cycle_config.num_side);
        send_struct(CMD_CPU_TO_MASTER_CYCLE_FORM, (uint8_t *)&type_mtfc_quick_cycle_config, sizeof(type_mtfc_quick_cycle_config_t));
    }

    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->setactivetimedevicerequest)
    {
        SetActiveTimeDeviceRequest *setActiveTimeDeviceRequest = mainMessage->devicemessage->userdevicemessage->setactivetimedevicerequest;
        printf("Time begin: %02d: %02d \r\n", setActiveTimeDeviceRequest->activelamp->begin->hour, setActiveTimeDeviceRequest->activelamp->begin->minute);
        printf("Time end: %02d: %02d \r\n", setActiveTimeDeviceRequest->activelamp->end->hour, setActiveTimeDeviceRequest->activelamp->end->minute);

        type_active_lamp_t type_active_lamp;

        time_active.hh_start = setActiveTimeDeviceRequest->activelamp->begin->hour;
        time_active.mm_start = setActiveTimeDeviceRequest->activelamp->begin->minute;

        time_active.hh_end = setActiveTimeDeviceRequest->activelamp->end->hour;
        time_active.mm_end = setActiveTimeDeviceRequest->activelamp->end->minute;

        Write_struct_time_active_toFile(time_active_file, "/home/pi/time_active.txt", time_active);

        send_struct(CMD_CPU_TO_MASTER_TIME_ACTIVE, (uint8_t *)&time_active, sizeof(time_active_t));
        /*********************************************Gui lai data cap nhat tu app len man hinh QT***************************************************************** */
        char data_send_active_time[200] = {0};
        int data_send_active_time_len = sprintf(data_send_active_time, TEMPLATE_TIME_ACTIVE, CMD_CPU_TO_QT_TIME_ACTIVE,
                                                time_active.hh_start,
                                                time_active.mm_start,
                                                time_active.hh_end,
                                                time_active.mm_end);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_active_time_len, data_send_active_time);
    }

    // kiem tra dieu tuyen
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->controllanedevicerequest)
    {
        ControlLaneDeviceRequest *controlLaneDeviceRequest = mainMessage->devicemessage->userdevicemessage->controllanedevicerequest;
        printf("Has control with code %d and message %s =================> control lane\r\n", controlLaneDeviceRequest->laneindex, controlLaneDeviceRequest->deviceid);
        char app_control_lane[200];
        if (controlLaneDeviceRequest->laneindex == 1)
        {
            mtfc_input_software.input_software = ADDR_INPUT_SOFTWARE_APP_CONTROL_T1;
            printf("Annouce CPU control Tuyen 1\r\n");
            memset((char *)feedback_log.feedback_log, '\0', 64);
            sprintf(feedback_log.feedback_log, "%s", "mtfc app control T1");
            // mtfc_log_update(&feedback_log);
            send_byte(CMD_CPU_TO_MASTER_MANUAL, 1);
            modeactive = 2;
            phase = 1;
            switchinfor = 1;
            // char tmp_data_Lan1[500]={0};
            memset((char *)app_control_lane, '\0', sizeof(app_control_lane));
            int length_inputStr_T1 = sprintf(app_control_lane, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, length_inputStr_T1, app_control_lane);
        }
        if (controlLaneDeviceRequest->laneindex == 2)
        {
            mtfc_input_software.input_software = ADDR_INPUT_SOFTWARE_APP_CONTROL_T2;
            printf("Annouce CPU control Tuyen 2\r\n");
            send_byte(CMD_CPU_TO_MASTER_MANUAL, 2);
            modeactive = 2;
            phase = 2;
            switchinfor = 1;
            // char app_con[500]={0};
            memset((char *)app_control_lane, '\0', sizeof(app_control_lane));
            int length_inputStr_T2 = sprintf(app_control_lane, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, length_inputStr_T2, app_control_lane);
        }
        if (controlLaneDeviceRequest->laneindex == 3)
        {
            mtfc_input_software.input_software = ADDR_INPUT_SOFTWARE_APP_CONTROL_T3;
            printf("Annouce CPU control Tuyen 3\r\n");
            send_byte(CMD_CPU_TO_MASTER_MANUAL, 2);
            modeactive = 2;
            phase = 3;
            switchinfor = 1;
            // char app_control_T3[500]={0};
            memset((char *)app_control_lane, '\0', sizeof(app_control_lane));
            int length_inputStr_T3 = sprintf(app_control_lane, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, length_inputStr_T3, app_control_lane);
        }
        if (controlLaneDeviceRequest->laneindex == 9)
        {
            mtfc_input_software.input_software = ADDR_INPUT_SOFTWARE_APP_CONTROL_DEFAULT;
            printf("Annouce CPU control Auto\r\n");
            send_byte(CMD_CPU_TO_MASTER_MANUAL, 0);
            // flag_control.phase_infor_app = 1;
            modeactive = 1;
            phase = 0;
            switchinfor = 1;
            // char tmp_data_auto[500]={0};
            memset((char *)app_control_lane, '\0', sizeof(app_control_lane));
            int length_inputStr_Auto = sprintf(app_control_lane, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
            cl->send_ex(cl, UWSC_OP_TEXT, 1, length_inputStr_Auto, app_control_lane);
        }
    }
    // Kiem tra online offline
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->deviceupdatemessage &&
        mainMessage->devicemessage->deviceupdatemessage->pingwebsocketdeviceresponse)
    {
        PingWebsocketDeviceResponse *pingWebsocketDeviceResponse = mainMessage->devicemessage->deviceupdatemessage->pingwebsocketdeviceresponse;
        keepAlliveCloud--;
    }

    // Get GPS
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->getcoordinatedevicerequest)
    {
        GetCoordinateDeviceRequest *getCoordinateDeviceRequest = mainMessage->devicemessage->userdevicemessage->getcoordinatedevicerequest;
        printf("Has sync GPS %s\r\n", getCoordinateDeviceRequest->deviceid);
    }

    // Kiem tra cap nhat lich tu app toi thiet bi
    if (mainMessage->devicemessage &&
        mainMessage->devicemessage->userdevicemessage &&
        mainMessage->devicemessage->userdevicemessage->updatescheduleuserdevicerequest)
    {
        UpdateScheduleUserDeviceRequest *updateScheduleUserDeviceRequest = mainMessage->devicemessage->userdevicemessage->updatescheduleuserdevicerequest;
        printf("\r\n%s", "--------------------------------------Days Form--------------------------------------");
#pragma region SEND DAYS FORM
        for (uint8_t i = 0; i < MAX_DAYS_WORKING; i++)
        {
            // printf("\r\n Days: [%d] at time form index [%03d]", i + 1, updateScheduleUserDeviceRequest->days->index[i]);
            mtfc_cycle_app_to_device.day[i] = updateScheduleUserDeviceRequest->days->index[i] + 1;
        }
        send_struct(CMD_CPU_TO_MASTER_SCHEDULE_DAYS_UPDATE, (uint8_t *)&mtfc_cycle_app_to_device.day, sizeof(type_days_lamp_form_t));

        Write_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", mtfc_cycle_app_to_device);
        delay(200);
        printf("\r\n%s", "-------------------------------------------------------------------------------------");
#pragma endregion
#pragma region SEND TIME FORM
        printf("\r\n%s", "--------------------------------------Time Form--------------------------------------");
        for (uint8_t i = 0; i < MAX_TIME_FORM; i++)
        {
            // printf("\r\nTime form index [%02d]", (i + 1));
            for (uint8_t j = 0; j < MAX_TIME_LINE; j++)
            {
                // printf("\r\n\tTime Line [%02d]", (j + 1));
                // printf("\r\n\tBegin at: [%03d %03d]", updateScheduleUserDeviceRequest->timelampform->index[i]->selectpoint[j]->t_begin_apply->hour,
                // updateScheduleUserDeviceRequest->timelampform->index[i]->selectpoint[j]->t_begin_apply->minute);
                // printf("\r\n\tIndex Cycle Form: [%03d]", updateScheduleUserDeviceRequest->timelampform->index[i]->selectpoint[j]->index_cycle_form);

                mtfc_cycle_app_to_device.time.index[i].select_point[j].t_begin_apply.hour = updateScheduleUserDeviceRequest->timelampform->index[i]->selectpoint[j]->t_begin_apply->hour;
                mtfc_cycle_app_to_device.time.index[i].select_point[j].t_begin_apply.minute = updateScheduleUserDeviceRequest->timelampform->index[i]->selectpoint[j]->t_begin_apply->minute;
                mtfc_cycle_app_to_device.time.index[i].select_point[j].index_cycle_form = updateScheduleUserDeviceRequest->timelampform->index[i]->selectpoint[j]->index_cycle_form;
            }
        }
        send_struct(CMD_CPU_TO_MASTER_SCHEDULE_TIME_UPDATE, (uint8_t *)&mtfc_cycle_app_to_device.time, sizeof(type_time_lamp_form_t));
        Write_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", mtfc_cycle_app_to_device);
        delay(200);
        printf("\r\n%s", "-------------------------------------------------------------------------------------");
#pragma endregion
#pragma region SEND CYCLE FORM
        printf("\r\n%s", "--------------------------------------Cycle Form--------------------------------------");
        for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
        {
            // printf("\r\ncycle form-----------------------------------------[%02d]", i + 1);
            // printf ("\r\nnum side used: %02d\r\nperiod crossroads: %03d\r\nclearance time crossroads: %02d",
            //         updateScheduleUserDeviceRequest->cycle->index[i]->num_side_used,
            //         updateScheduleUserDeviceRequest->cycle->index[i]->period_crossroads,
            //         updateScheduleUserDeviceRequest->cycle->index[i]->clearance_time_crossroads);
            for (uint8_t j = 0; j < MAX_SIDE; j++)
            {
                // printf ("\r\n\tSide------------------------------------------[%02d]", j + 1);
                // printf ("\r\nt_green: %02d\r\n\tt_red: %02d\r\n\tt_yellow: %02d",
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green,
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_red,
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_yellow);
                // printf ("\r\nt_start_green: %02d\tt_end_green: %02d",
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_start_green,
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_end_green);
                // printf ("\r\nt_start_yellow: %02d\tt_end_yellow: %02d",
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_start_yellow,
                //         updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_end_yellow);
                if (i == 0 && updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green)
                {
                    mtfc_cycle_app_to_device.cycle_1[j] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green;
                    mtfc_cycle_app_to_device.cycle_1[8] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_yellow;
                    mtfc_cycle_app_to_device.cycle_1[9] = updateScheduleUserDeviceRequest->cycle->index[i]->clearance_time_crossroads;
                    // printf("\r\nTime yellow cycle 1: %02d", mtfc_cycle_app_to_device.cycle_1[8]);
                }
                else if (i == 1 && updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green)
                {
                    mtfc_cycle_app_to_device.cycle_2[j] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green;
                    mtfc_cycle_app_to_device.cycle_2[8] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_yellow;
                    mtfc_cycle_app_to_device.cycle_2[9] = updateScheduleUserDeviceRequest->cycle->index[i]->clearance_time_crossroads;
                    // printf("\r\nTime yellow cycle 2: %02d", mtfc_cycle_app_to_device.cycle_2[8]);
                }
                else if (i == 2 && updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green)
                {
                    mtfc_cycle_app_to_device.cycle_3[j] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green;
                    mtfc_cycle_app_to_device.cycle_3[8] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_yellow;
                    mtfc_cycle_app_to_device.cycle_3[9] = updateScheduleUserDeviceRequest->cycle->index[i]->clearance_time_crossroads;
                    // printf("\r\nTime yellow cycle 3: %02d", mtfc_cycle_app_to_device.cycle_3[8]);
                }
                else if (i == 3 && updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green)
                {
                    mtfc_cycle_app_to_device.cycle_4[j] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green;
                    mtfc_cycle_app_to_device.cycle_4[8] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_yellow;
                    mtfc_cycle_app_to_device.cycle_4[9] = updateScheduleUserDeviceRequest->cycle->index[i]->clearance_time_crossroads;
                    // printf("\r\nTime yellow cycle 4: %02d", mtfc_cycle_app_to_device.cycle_4[8]);
                }
                else if (i == 4 && updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green)
                {
                    mtfc_cycle_app_to_device.cycle_5[j] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_green;
                    mtfc_cycle_app_to_device.cycle_5[8] = updateScheduleUserDeviceRequest->cycle->index[i]->side[j]->t_yellow;
                    mtfc_cycle_app_to_device.cycle_5[9] = updateScheduleUserDeviceRequest->cycle->index[i]->clearance_time_crossroads;
                    // printf("\r\nTime yellow cycle 5: %02d", mtfc_cycle_app_to_device.cycle_5[8]);
                }
            }
        }
        send_struct(CMD_CPU_TO_MASTER_SCHEDULE_CYCLE, (uint8_t *)&mtfc_cycle_app_to_device, sizeof(cycle_t));
        delay(100);
        Write_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", mtfc_cycle_app_to_device);
        is_resend_cycle_to_app = true; // Bật biến cập nhật lại lịch từ app xuống master
#pragma endregion
    }
}

void uwsc_onerror_server(struct uwsc_client *mtfc_client_sv, int err, const char *msg)
{
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
    uwsc_log_err("onerror:%d: %s\n", err, msg);
#endif
    ev_break(mtfc_client_sv->loop, EVBREAK_ALL);
}

void signal_cb_server(struct ev_loop *loop_sv, ev_signal *w, int revents)
{
    if (w->signum == SIGINT)
    {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
        uwsc_log_info("Normal quit\n");
#endif
    }
    ev_break(loop_sv, EVBREAK_ALL);
}

void uwsc_onclose_server(struct uwsc_client *mtfc_client_sv, int code, const char *reason)
{
    // printf("Recreate socket to login\r\n");
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
    uwsc_log_err("onclose:%d: %s\n", code, reason);
#endif
    // ev_break(mtfc_client_sv->loop, EVBREAK_ALL);
}

void updateServer()
{
    if (device_id == NULL)
    {
        printf("Device did not register update server, please type: register\r\n");
        return;
    }

    if (!is_login_success)
    {
        printf("device did not login, please type: login\r\n");
        return;
    }

    MainMessage mainMessage = MAIN_MESSAGE__INIT;
    DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
    DeviceUpdateMessage deviceUpdateMessage = DEVICE_UPDATE_MESSAGE__INIT;
    StateOnChangeDeviceMessage stateOnchange = STATE_ON_CHANGE_DEVICE_MESSAGE__INIT;
    DeviceState update = DEVICE_STATE__INIT;

    stateOnchange.deviceid = device_id;
    update.num_phase = sopha.sopha;

    copy_u8_array_to_u32_array(mtfc_card_data_out.ouput, g_signal, MAX_SIDE);
    update.signal = g_signal;
    update.n_signal = MAX_SIDE;

    copy_u8_array_to_u32_array(mtfc_card_data_out.ouput, g_mapping, MAX_SIDE);
    update.mapping = g_mapping;
    update.n_mapping = MAX_SIDE;

    copy_u8_array_to_u32_array(mtfc_card_data_out.tm_cycle.countdown, g_countdown, MAX_SIDE);
    update.countdown = g_countdown;
    update.n_countdown = MAX_SIDE;

    setenv("TZ", ":Asia/Ho_Chi_Minh", 1);
    tzset();
    time_t now;
    now = time(NULL);
    now += (time_t)(3600 * 7);
    char timestamp[20];
    sprintf(timestamp, "%ld", now);

    update.rtc_epoch = (uint32_t)atoi(timestamp);
    update.walking_signal = 0;
    update.railway_signal = 0;
    update.cpu_active_mode = 0;
    update.input_hardware = 1;
    update.input_software = mtfc_input_software.input_software;
    update.idx_time_form = time_current.time_form - 1; // do code cu time form bat dau tu 1
    update.idx_timeline = time_current.time_line;      // code cu time line bat dau tu 0
    update.idx_cycle_form = time_current.time_index;   // code cu time index bat dau tu 0

    stateOnchange.update1second = &update;
    deviceUpdateMessage.stateonchange = &stateOnchange;
    deviceMessage.deviceupdatemessage = &deviceUpdateMessage;
    mainMessage.devicemessage = &deviceMessage;

    int size = main_message__pack(&mainMessage, tx_buffer);
    mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
}

void updateServerSchedule()
{
    if (device_id == NULL)
    {
        printf("Device did not register update server Schedule, please type: register\r\n");
        return;
    }

    if (!is_login_success)
    {
        printf("device did not login, please type: login\r\n");
        return;
    }

    MainMessage mainMessage = MAIN_MESSAGE__INIT;
    DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
    DeviceUpdateMessage deviceUpdateMessage = DEVICE_UPDATE_MESSAGE__INIT;
    DeviceUpdateScheduleRequest deviceUpdateScheduleRequest = DEVICE_UPDATE_SCHEDULE_REQUEST__INIT;

    deviceUpdateScheduleRequest.deviceid = device_id;
    deviceUpdateScheduleRequest.num_side = sopha.sopha;

    ActiveLamp active_lamp = ACTIVE_LAMP__INIT;
    HMTime begin = HMTIME__INIT;
    HMTime end = HMTIME__INIT;

    begin.hour = time_active.hh_start;
    begin.minute = time_active.mm_start;
    active_lamp.begin = &begin;

    end.hour = time_active.hh_end;
    end.minute = time_active.mm_end;
    active_lamp.end = &end;

    /*****************************************************Cycle Form*************************************** */
    CycleLampForm cycleLampForm = CYCLE_LAMP_FORM__INIT;
    OneCycleLampTime *oneCycleLampTimeList[32];

    for (int i = 0; i < 32; i++)
    {
        oneCycleLampTimeList[i] = (OneCycleLampTime *)malloc(sizeof(OneCycleLampTime));
        one_cycle_lamp_time__init(oneCycleLampTimeList[i]);
    }
    OneSideLampTime *oneSideLampTimeList[32][8];

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            oneSideLampTimeList[i][j] = (OneSideLampTime *)malloc(sizeof(OneSideLampTime));
            one_side_lamp_time__init(oneSideLampTimeList[i][j]);
        }

        for (int j = 0; j < 8; j++)
        {
            oneSideLampTimeList[i][j]->t_green = mtfc_schedule.cycle.index[i].side[j].t_green;
            oneSideLampTimeList[i][j]->t_yellow = mtfc_schedule.cycle.index[i].side[j].t_yellow;
            oneSideLampTimeList[i][j]->t_red = mtfc_schedule.cycle.index[i].side[j].t_red;
            oneSideLampTimeList[i][j]->t_start_green = mtfc_schedule.cycle.index[i].side[j].t_start_green;
            oneSideLampTimeList[i][j]->t_start_yellow = mtfc_schedule.cycle.index[i].side[j].t_start_yellow;
            oneSideLampTimeList[i][j]->t_end_green = mtfc_schedule.cycle.index[i].side[j].t_end_green;
            oneSideLampTimeList[i][j]->t_end_yellow = mtfc_schedule.cycle.index[i].side[j].t_end_yellow;
        }
        oneCycleLampTimeList[i]->side = oneSideLampTimeList[i];
        oneCycleLampTimeList[i]->n_side = MAX_SIDE;
        oneCycleLampTimeList[i]->num_side_used = mtfc_schedule.cycle.index[i].num_side_used;
        oneCycleLampTimeList[i]->clearance_time_crossroads = mtfc_schedule.cycle.index[i].clearance_time_crossroads;
        oneCycleLampTimeList[i]->period_crossroads = mtfc_schedule.cycle.index[i].period_crossroads;
    }

    cycleLampForm.n_index = 32;
    cycleLampForm.index = oneCycleLampTimeList;
    /*****************************************************Cycle Form*************************************** */
    /*****************************************************Time Form*************************************** */
    TimeLampForm timeLampForm = TIME_LAMP_FORM__INIT;
    DayTimeLine *dayTimeLine[MAX_TIME_FORM];

    for (int i = 0; i < MAX_TIME_LINE; i++)
    {
        dayTimeLine[i] = (DayTimeLine *)malloc(sizeof(DayTimeLine));
        day_time_line__init(dayTimeLine[i]);
    }

    HMTime t_begin_apply = HMTIME__INIT;
    OneTimeline *oneTimeline[MAX_TIME_FORM][MAX_TIME_LINE];
    for (int i = 0; i < MAX_TIME_FORM; i++)
    {
        for (int j = 0; j < MAX_TIME_LINE; j++)
        {
            oneTimeline[i][j] = (OneTimeline *)malloc(sizeof(OneTimeline));
            one_timeline__init(oneTimeline[i][j]);
        }
        for (int j = 0; j < MAX_TIME_LINE; j++)
        {
            oneTimeline[i][j]->t_begin_apply = malloc(sizeof(HMTime));
            hmtime__init(oneTimeline[i][j]->t_begin_apply);

            oneTimeline[i][j]->t_begin_apply->hour = mtfc_schedule.time.index[i].select_point[j].t_begin_apply.hour;
            oneTimeline[i][j]->t_begin_apply->minute = mtfc_schedule.time.index[i].select_point[j].t_begin_apply.minute;
            oneTimeline[i][j]->index_cycle_form = mtfc_schedule.time.index[i].select_point[j].index_cycle_form;
        }
        dayTimeLine[i]->n_selectpoint = MAX_TIME_FORM;
        dayTimeLine[i]->selectpoint = oneTimeline[i];
    }
    timeLampForm.n_index = MAX_TIME_FORM;
    timeLampForm.index = dayTimeLine;
    /*****************************************************Time Form*************************************** */

    /*****************************************************Day Form*************************************** */
    DayLampForm dayLampForm = DAY_LAMP_FORM__INIT;
    uint32_t temp[7];
    for (int i = 0; i < MAX_DAYS_WORKING; i++)
    {
        temp[i] = mtfc_schedule.days.index[i] - 1;
    }
    dayLampForm.index = temp;
    dayLampForm.n_index = MAX_DAYS_WORKING;
    printf("DEBUG DAYSFORM------------------------------------------------------------------------------");
    for (int i = 0; i < MAX_DAYS_WORKING; i++)
    {
        printf("\r\nDays :[%d] at time form index [%03d]", i + 1, dayLampForm.index[i]);
    }
    /*****************************************************Day Form*************************************** */
    deviceUpdateScheduleRequest.days = &dayLampForm;
    deviceUpdateScheduleRequest.time = &timeLampForm;
    deviceUpdateScheduleRequest.cycle = &cycleLampForm;
    deviceUpdateScheduleRequest.active_time = &active_lamp;
    deviceUpdateMessage.deviceupdateschedulerequest = &deviceUpdateScheduleRequest;
    deviceMessage.deviceupdatemessage = &deviceUpdateMessage;
    mainMessage.devicemessage = &deviceMessage;

    int size = main_message__pack(&mainMessage, tx_buffer);
    // MainMessage *mainMessage_recieved = main_message__unpack(NULL, size, tx_buffer);
    mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
}

void mtfc_log_update(feedback_log_t *dat)
{
    printf("\r\n--------------------------------------------------------------------\r\n");
    printf("Log update: %s\r\n", dat->feedback_log);

    MainMessage mainMessage = MAIN_MESSAGE__INIT;
    DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
    DeviceUpdateMessage deviceUpdateMessage = DEVICE_UPDATE_MESSAGE__INIT;
    DeviceLogUpdate deviceLogUpdate = DEVICE_LOG_UPDATE__INIT;

    deviceLogUpdate.deviceid = device_id;
    deviceLogUpdate.message = dat->feedback_log;

    deviceUpdateMessage.devicelogupdate = &deviceLogUpdate;
    deviceMessage.deviceupdatemessage = &deviceUpdateMessage;
    mainMessage.devicemessage = &deviceMessage;

    int size = main_message__pack(&mainMessage, tx_buffer);
    mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
    memset((char *)dat->feedback_log, '\0', 64);
    printf("\r\n--------------------------------------------------------------------\r\n");
}

void *Open_Websocket_Online(void *threadArgs)
{   
    delay(10000);
    struct ev_loop *loop_sv = EV_DEFAULT;
   while (1)
    {
        // sem_post(&semaphore);
        // pthread_mutex_lock(&mutex);
        // if (is_start_open_wss == true)
        // {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
            printf("thread for open websocket server websocket online\r\n");
            uwsc_log_info("Libuwsc: %s\n", UWSC_VERSION_STRING);
#endif      delay(5000);
            mtfc_client_sv = uwsc_new(loop_sv, url_server, ping_interval, NULL); //

            if (!mtfc_client_sv)
            {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
                printf("Create new socket server fail\r\n");
#endif
                // pthread_exit(NULL);
                continue;
            }
            else
            {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
                printf("Create new socket server success\r\n");
#endif
            }
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
            uwsc_log_info("Start connect server...\n");
#endif
            mtfc_client_sv->onopen = uwsc_onopen_server;
            mtfc_client_sv->onmessage = uwsc_onmessage_server;
            mtfc_client_sv->onerror = uwsc_onerror_server;
            mtfc_client_sv->onclose = uwsc_onclose_server;

            device_id = NULL;
            ev_signal_init(&signal_watcher_app, signal_cb_server, SIGINT);
            ev_signal_start(loop_sv, &signal_watcher_app);
            ev_run(loop_sv, 0);
            free(mtfc_client_sv);
            return 0;
        // }
    }
}

void *Register_Device_GateWay(void *threadArgs)
{
    while (1)
    {
        delay(1);
        if (is_begin_start_up)
        {
            if (is_finish_start_up)
            {
                keepAlliveCPU = 0;
                is_begin_start_up = false;
                is_finish_start_up = false;
                is_get_details_startup = true;

                while (!is_get_details_startup)
                {
                    printf("Get detail\r\n");
                    memset((char *)request_detail.buff_request, '\0', 32);
                    sprintf(request_detail.buff_request, "%s", "details");
                    printf("buffer request: %s\r\n", request_detail.buff_request);
                    //==============================================> ham send_struct gui cho master
                    delay(5000);
                }

                while (is_register_success == 0) // Dang ky thiet bi
                {
                    if (!is_flag_opend_weboscket_online)
                    {
                        printf("Cho mo ket noi vs server online\r\n");
                        delay(2000);
                        continue;
                    }
                    printf("ket noi thanh cong vs server online thanh cong\r\n");//
                    MainMessage mainMessage = MAIN_MESSAGE__INIT;
                    DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
                    DeviceAuthMessage deviceAuthMessage = DEVICE_AUTH_MESSAGE__INIT;
                    DeviceRegisterRequest deviceRegisterRequest = DEVICE_REGISTER_REQUEST__INIT;

                    printf("Mac ID: %s\r\n", mac_final);
                    printf("firmware version is: %s", firmware_GW_version);
                    printf("project ID is: %s", "Sitech Co.Ltd\r\n");

                    deviceRegisterRequest.hardwareid = mac_final;
                    deviceRegisterRequest.firmwareversion = firmware_GW_version;
                    deviceRegisterRequest.mantoken = "112233445566";
                    deviceRegisterRequest.cpuimei = "12345943860";
                    deviceRegisterRequest.numphase = sopha.sopha;

                    // Them phan GPS khi dang ky thiet bi
                    deviceRegisterRequest.lat = (double)Coordinate_float.Lat;
                    deviceRegisterRequest.log = (double)Coordinate_float.Long;

                    deviceAuthMessage.deviceregisterrequest = &deviceRegisterRequest;
                    deviceMessage.deviceauthmessage = &deviceAuthMessage;
                    mainMessage.devicemessage = &deviceMessage;

                    int size = main_message__pack(&mainMessage, tx_buffer);
                    mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
                    printf("Send Register device GateWay to server \r\n");
                    delay(2000);
                }

                while ((is_register_success == true) && (!is_login_success)) // Login
                {
                    printf("Send Register device GateWay to server success ========> login server\r\n");
                    if (device_id == NULL)
                    {
                        printf("Device did not register\n");
                        continue;
                    }
                    if (!is_register_success)
                    {
                        delay(2000);
                        continue;
                    }
                    if (strlen(device_id) <= 0)
                    {
                        delay(1000);
                        exit(1);
                    }
                    printf("Device registered, to login\n");
                    printf("Device ID: %s", device_id);

                    MainMessage mainMessage = MAIN_MESSAGE__INIT;
                    DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
                    DeviceAuthMessage deviceAuthMessage = DEVICE_AUTH_MESSAGE__INIT;
                    DeviceLoginRequest deviceLoginRequest = DEVICE_LOGIN_REQUEST__INIT;

                    deviceLoginRequest.deviceid = device_id;
                    deviceLoginRequest.cpuimage = "12345943860";

                    deviceAuthMessage.deviceloginrequest = &deviceLoginRequest;
                    deviceMessage.deviceauthmessage = &deviceAuthMessage;
                    mainMessage.devicemessage = &deviceMessage;

                    int size = main_message__pack(&mainMessage, tx_buffer);
                    mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
                    printf("Send Login to server\r\n");
                    delay(2000);
                }

                // bien bat buzzer
                if (isSoftConnected) // Send state app connect
                {
                    printf("Annouce CPU app connected\r\n");
                    memset((char *)app_connected.app_connected, '\0', 32);
                    sprintf(app_connected.app_connected, "%s", "app_connect");
                    printf("buffer request: %s\r\n", app_connected.app_connected);
                    //============================> ham send_struct cho master
                    delay(500);
                }
                else
                {
                    printf("Annouce CPU app disconnected\r\n");
                    memset((char *)app_disconnected.app_disconnected, '\0', 32);
                    sprintf(app_disconnected.app_disconnected, "%s", "app_disconnect");
                    printf("buffer request: %s\r\n", app_disconnected.app_disconnected);
                    //============================> ham send_struct cho master
                    delay(500);
                }

                printf("Get config CPU\r\n");
                memset((char *)request_config.buff_config, '\0', 32);
                sprintf(request_config.buff_config, "%s", "config");
                printf("buffer request: %s\r\n", request_config.buff_config);
                //=================================> ham send_struct cho master
                delay(1000);
                printf("Get schedule for CPU\r\n");
                memset((char *)request_schedule.buff_schedule, '\0', 32);
                sprintf(request_schedule.buff_schedule, "%s", "schedule");
                printf("buffer request: %s\r\n", request_schedule.buff_schedule);
                //=================================> ham send_struct cho master
            }
            continue;
        }

        while (!is_get_details_startup)
        {
            printf("Get detail\n");
            memset((char *)request_detail.buff_request, '\0', 32);
            sprintf(request_detail.buff_request, "%s", "details");
            printf("buffer request: %s\r\n", request_detail.buff_request);
            //=================================> ham send_struct cho master
            delay(5000);
        }

        if (!is_register_success)
        {
            delay(2000);
            continue;
        }

        if (is_register_success == 0)
        {
            MainMessage mainMessage = MAIN_MESSAGE__INIT;
            DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
            DeviceAuthMessage deviceAuthMessage = DEVICE_AUTH_MESSAGE__INIT;
            DeviceRegisterRequest deviceRegisterRequest = DEVICE_REGISTER_REQUEST__INIT;

            deviceRegisterRequest.hardwareid = mac_final;
            deviceRegisterRequest.firmwareversion = firmware_GW_version;
            deviceRegisterRequest.mantoken = "112233445566";
            deviceRegisterRequest.cpuimei = "12345943860";
            deviceRegisterRequest.numphase = sopha.sopha;

            // Them phan GPS khi dang ky thiet bi
            deviceRegisterRequest.lat = (double)Coordinate_float.Lat;
            deviceRegisterRequest.log = (double)Coordinate_float.Long;

            deviceAuthMessage.deviceregisterrequest = &deviceRegisterRequest;
            deviceMessage.deviceauthmessage = &deviceAuthMessage;
            mainMessage.devicemessage = &deviceMessage;

            int size = main_message__pack(&mainMessage, tx_buffer);
            mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
            printf("Send Register device GateWay to server \r\n");
            delay(2000);
        }

        if ((is_register_success == true) && (!is_login_success))
        {
            if (device_id == NULL)
            {
                printf("Device did not register\n");
                return;
            }
            printf("Device ID: %s\n", device_id);
            if (strlen(device_id) <= 0)
            {
                delay(1000);
                exit(1);
            }
            if (counter_login_fail > 5)
            {
                counter_login_fail = 0;
                exit(1);
            }

            MainMessage mainMessage = MAIN_MESSAGE__INIT;
            DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
            DeviceAuthMessage deviceAuthMessage = DEVICE_AUTH_MESSAGE__INIT;
            DeviceLoginRequest deviceLoginRequest = DEVICE_LOGIN_REQUEST__INIT;

            deviceLoginRequest.deviceid = device_id;
            deviceLoginRequest.cpuimage = "12345943860";

            deviceAuthMessage.deviceloginrequest = &deviceLoginRequest;
            deviceMessage.deviceauthmessage = &deviceAuthMessage;
            mainMessage.devicemessage = &deviceMessage;

            int size = main_message__pack(&mainMessage, tx_buffer);
            mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);
            printf("Send Login to server\r\n");
            delay(2000);
            counter_login_fail++;
        }

        if (is_login_success)
        {
            if (device_id == NULL)
            {
                printf("Device did not register is login success, please type: register\n");
                return;
            }
            if (is_send_config)
            {
                printf("Get config CPU\r\n");
                memset((char *)request_config.buff_config, '\0', 32);
                sprintf(request_config.buff_config, "%s", "config");
                printf("buffer request: %s\r\n", request_config.buff_config);
                //===================================> send_struct cho master
                is_send_config = false;
            }
            if (is_send_schedule)
            {
                memset((char *)request_schedule.buff_schedule, '\0', 32);
                sprintf(request_schedule.buff_schedule, "%s", "schedule");
                printf("buffer request: %s\r\n", request_schedule.buff_schedule);
                //===================================> send_struct cho master
                is_send_schedule = false;
            }

            if (is_first_send_cycle)
            {
                is_first_send_cycle = false;
                updateServerSchedule();
                printf("\r\n%s\r\n", "First send cycle to app");
            }

            unsigned long long currentMillis = millis();
            unsigned long long currentMillis_update = millis();
            unsigned long long currentMillis_CPU_5s = millis();
            unsigned long long currentMillis_CPU_1s = millis();

            if ((currentMillis_CPU_5s - previousMillis_CPU_5s > interval_CPU_5s) && (!isSoftConnected))
            {
                previousMillis_CPU_5s = currentMillis_CPU_5s;
                keepAlliveCPU = keepAlliveCPU + 5;
            }
            if ((currentMillis_CPU_1s - previousMillis_CPU_1s > interval_CPU_1s) && (!isSoftConnected))
            {
                previousMillis_CPU_1s = currentMillis_CPU_1s;
                keepAlliveCPU++;
            }

            if (keepAlliveCPU > 30)
            {
                printf("CPU disconnected\r\n");
                delay(1000);
                // exit(1);
            }
            if (currentMillis - previousMillis > interval)
            {
                previousMillis = currentMillis;

                MainMessage mainMessage = MAIN_MESSAGE__INIT;
                DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
                DeviceUpdateMessage deviceUpdateMessage = DEVICE_UPDATE_MESSAGE__INIT;
                PingWebsocketDeviceRequest pingWebsocketDeviceRequest = PING_WEBSOCKET_DEVICE_REQUEST__INIT;

                pingWebsocketDeviceRequest.deviceid = device_id;

                deviceUpdateMessage.pingwebsocketdevicerequest = &pingWebsocketDeviceRequest;
                deviceMessage.deviceupdatemessage = &deviceUpdateMessage;
                mainMessage.devicemessage = &deviceMessage;

                int size = main_message__pack(&mainMessage, tx_buffer);

                mtfc_client_sv->send_ex(mtfc_client_sv, UWSC_OP_BINARY, 1, size, tx_buffer);

                keepAlliveCloud++;
                
                if (keepAlliveCloud >= 4)
                {
                    printf("Annouce CPU offline\r\n");
                    memset(offline.offline, 0, 32);
                    memcpy(offline.offline, "cloud_disconnect", 16);
                    printf("buffer request: %s\r\n", offline.offline);
                    //====================================> send_struct cho master
                    is_login_success = false;
                    delay(2000);
                    exit(1);
                }
            }
        }

        // Gửi lại cập nhật lịch lên app cũ
        if (is_resend_cycle_to_app)
        {
            send_byte(CMD_CPU_TO_MASTER_SCHEDULE, 1);
            delay(1000);
        }
    }
}

void* loop2thread(void* args)
{
    printf("Inside loop 2"); // Here one could initiate another timeout watcher
    
    mtfc_client_sv = uwsc_new(loop2, url_server, ping_interval, NULL);
    device_id = NULL;    
    if (!mtfc_client_sv)
    {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
        printf("Create new socket server fail\r\n");
#endif      
        // return -1;
        // continue;
    }
    else
    {
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
        printf("Create new socket server success\r\n");
#endif
        // ev_signal_init(&signal_watcher_app, signal_cb_server, SIGINT);
        // ev_signal_start(loop_sv, &signal_watcher_app);
    }   
    if(mtfc_client_sv)  {
        mtfc_client_sv->onopen = uwsc_onopen_server;
        mtfc_client_sv->onmessage = uwsc_onmessage_server;
        mtfc_client_sv->onerror = uwsc_onerror_server;
        mtfc_client_sv->onclose = uwsc_onclose_server;

        ev_run(loop2, 0);       // similar to the main loop - call it say timeout_cb1
    }  
    free(mtfc_client_sv);
    return NULL;
}

static void async_cb (EV_P_ ev_async *w, int revents)
{
    // puts ("async ready");
    pthread_mutex_lock(&lock);     //Don't forget locking
    ++async_count;
    // printf("async = %d, timeout = %d \n", async_count, timeout_count);
    pthread_mutex_unlock(&lock);   //Don't forget unlocking
}

static void timeout_cb (EV_P_ ev_timer *w, int revents) // Timer callback function
{
    //puts ("timeout");
    if (ev_async_pending(&async_watcher)==false) { //the event has not yet been processed (or even noted) by the event loop? (i.e. Is it serviced? If yes then proceed to)
        ev_async_send(loop2, &async_watcher); //Sends/signals/activates the given ev_async watcher, that is, feeds an EV_ASYNC event on the watcher into the event loop.
    }
    pthread_mutex_lock(&lock);     //Don't forget locking
    ++timeout_count;
    pthread_mutex_unlock(&lock);   //Don't forget unlocking
    // w->repeat = timeout;
    // ev_timer_again(loop, &timeout_watcher); //Start the timer again.
}

#pragma endregion
void startup_memset(void)
{
    memset((char *)&online, '\0', sizeof(online_t));
    memset((char *)&offline, '\0', sizeof(offline_t));
    memset((char *)&app_connected, '\0', sizeof(app_connected_t));
    memset((char *)&app_disconnected, '\0', sizeof(app_disconnected_t));
    memset((char *)&Coordinate_float, '\0', sizeof(Coordinate_float_t));
    memset((char *)&request_config, '\0', sizeof(request_config_t));
    memset((char *)&request_schedule, '\0', sizeof(request_schedule_t));
    memset((char *)&request_detail, '\0', sizeof(request_detail_t));
    memset((char *)&feedback_log, '\0', sizeof(feedback_log_t));
    memset((char *)&mtfc_input_software, '\0', sizeof(input_software_t));
}

#pragma region LUONG MAIN INIT
int main()
{
    startup_memset();
    //-------------------------------------------------Khoi tao UART--------------------------------------------------------------//
    if (wiringPiSetup() < 0)
    {
        return 1;
    }
    delay(100);

    if ((fd = serialOpen("/dev/ttyAMA1", 115200)) < 0)
    {
        return 1;
    }
    //-------------------------------------------------Khoi tao IO--------------------------------------------------------------//
    pinMode(auto_Mode, PUD_UP);
    pinMode(manual_Mode, PUD_UP);
    pinMode(LED_CLOUD, OUTPUT);
    pinMode(LED_GPS, OUTPUT);
    pinMode(Buzzer, OUTPUT);

    digitalWrite(Buzzer, 1);
    digitalWrite(LED_GPS, 1);
    digitalWrite(LED_CLOUD, 1);

    time_active = Read_struct_time_active_toFile(time_active_file, "/home/pi/time_active.txt", time_active);
    schedule = Read_struct_schedule_toFile(schedule_file, "/home/pi/schedule.txt", schedule);
    sopha = Read_struct_sopha_toFile(sopha_file, "/home/pi/sopha.txt", sopha);
    cycle = Read_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", cycle);
    setting = Read_struct_setting_toFile(setting_file, "/home/pi/setting.txt", setting);

    /*chuyen doi data phu hop voi app cu*/
    matching_data_to_old_app();

    type_cardConfig.index[0] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard1.txt", type_cardConfig.index[0]);
    type_cardConfig.index[1] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard2.txt", type_cardConfig.index[1]);
    type_cardConfig.index[2] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard3.txt", type_cardConfig.index[2]);
    type_cardConfig.index[3] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard4.txt", type_cardConfig.index[3]);
    type_cardConfig.index[4] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard5.txt", type_cardConfig.index[4]);
    type_cardConfig.index[5] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard6.txt", type_cardConfig.index[5]);
    type_cardConfig.index[6] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard7.txt", type_cardConfig.index[6]);
    type_cardConfig.index[7] = Read_struct_card_toFile(card_file, "/home/pi/data/card/datacard8.txt", type_cardConfig.index[7]);

    mtfc_input_software.input_software = 1;
    time_current.type_cardConfig = type_cardConfig;

    printf("setting.server:%d\r\n", setting.server);

    printf("\r\nhh_start is: %d", time_active.hh_start);
    printf("\r\nmm_start is: %d\r\n", time_active.mm_start);

    system("cat /sys/class/net/eth0/address > mac.txt");

    fp = fopen("mac.txt", "r");
    if (fp == NULL)
    {
        printf("Error openfile\r\n");
        perror("Error opening file");
    }
    
    if (fgets(mac, 18, fp) != NULL)
    {
        /* writing content to stdout */
    }
    
    /* Closing the file mac.txt */
    fclose(fp);
    printf("Mac CM4: %s\r\n", mac);

    // printf("MAC: %s\r\n",mac);
    substr(mac, 6, 16);

    asprintf(&mac_id_gateway, "GW-%s", re_mac);
    // printf("MAC ID: %s\r\n", mac_id_gateway);
    char *token = NULL;
    const char s[2] = ":";
    token = strtok(mac_id_gateway, s);
    mac_final = token;
    // strcat(mac_final, token);
    token = strtok(NULL, s);
    strcat(mac_final, token);
    token = strtok(NULL, s);
    strcat(mac_final, token);
    token = strtok(NULL, s);
    strcat(mac_final, token);
    printf("Mac ID: %s\r\n", mac_final);
    //-----------------------------------------------------------------------Lay du lieu GPS mac dinh luu trong file--------------------------------------------------------------------//
    fptr = fopen("/home/pi/GPS.txt", "r");
    if (fptr == NULL)
    {
        printf("Error!");
    }
    if (fgets(Coordinate_str, 50, fptr) != NULL){}
    fclose(fptr);

    temp = strtok(Coordinate_str, ":");
    temp = strtok(NULL, ",");
    Coordinate_float.Lat = atof(temp);
    temp = strtok(NULL, ":");
    temp = strtok(NULL, "/");
    Coordinate_float.Long = atof(temp);

    printf("Lat: %f\r\n", Coordinate_float.Lat);
    printf("Long: %f\r\n", Coordinate_float.Long);

    // mac_final = "GW-016d55cb";
    //  if(setting.server)
    //  {
    //      if (pthread_create(&thread_Mqtt, NULL, Mqtt, NULL) != 0)
    //          printf("thread_create() failed\n");
    //  }

#pragma region TAO LUONG WEBSOCKET ONLINE
    // if (pthread_create(&thread_Open_Websocket_Online, NULL, Open_Websocket_Online, NULL) != 0)
    //     printf("thread_create() failed\n");
#pragma endregion
#pragma region TAO LUONG GUI THONG SO LEN MAN HINH QT
    if (pthread_create(&thread_send_parameter_to_Monitor, NULL, send_parameter_to_Monitor, NULL) != 0)
    {
        printf("thread_create main process() failed\n");
    }
#pragma endregion
#pragma region TAO LUONG DOC DATA SERIAL TU MASTER
    if (pthread_create(&thread_Serial, NULL, Serial, NULL) != 0)
    {
        printf("thread_create() failed\n");
    }
#pragma endregion
#pragma region TAO LUONG LAY DU LIEU GPS
    if (pthread_create(&thread_ProcessGPS, NULL, process_GPS, NULL) != 0)
        printf("thread process get GPS create() failed\n");
#pragma endregion
#pragma region TAO LUONG GUI TAT CA DATA LEN MAN HINH QT
    if (pthread_create(&thread_send_data_to_all, NULL, send_data_to_all, NULL) != 0)
        printf("send_data_to_all  failed\n");
#pragma endregion
#pragma region TAO LUONG KET NOI SERVER
    if (pthread_create(&thread_send_Websocket_Online, NULL, Register_Device_GateWay, NULL) != 0)
        printf("open websocket online  failed\n");
#pragma endregion

    send_byte(CMD_CPU_TO_MASTER_START, 1);
    printf("Start.........\r\n");
    delay(50);
    send_byte(CMD_CPU_TO_MASTER_MANUAL, 0);
    
    struct ev_loop *loop = EV_DEFAULT;
    while (1)
    {
        delay(1);
        RS232 = serialOpen("/dev/ttyAMA1", 115200); /* open serial port */

        serialPrintf(RS232, "Xin Chao\r\n");

        uwsc_log_info("Start connect...\n");
        uwsc_log_info("Libuwsc: %s\n", UWSC_VERSION_STRING);

        cl = uwsc_new(loop, url, ping_interval, NULL);

#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
        printf("thread for open websocket server\r\n");
        uwsc_log_info("Libuwsc: %s\n", UWSC_VERSION_STRING);
#endif
        if (!cl){
            printf("Create new socket fail\r\n");
            return -1;
            // continue;
        }
        else{
            printf("Create new socket app success\r\n");
        }
        
        cl->onopen = uwsc_onopen;
        cl->onmessage = uwsc_onmessage;
        cl->onerror = uwsc_onerror;
        cl->onclose = uwsc_onclose;

        pthread_mutex_init(&lock, NULL);
        pthread_t thread;
        loop2 = ev_loop_new(0);

        ev_async_init(&async_watcher, async_cb);
        ev_async_start(loop2, &async_watcher);

        ev_timer_init (&timeout_watcher, timeout_cb, timeout, 0.); // Non repeating timer. The timer starts repeating in the timeout callback function
        ev_timer_start (loop, &timeout_watcher);
        
        pthread_create(&thread, NULL, loop2thread, NULL);

        // now wait for events to arrive
        ev_run(loop, 0);
        free(cl);
        
        //Wait on threads for execution
        pthread_join(thread, NULL);

        pthread_mutex_destroy(&lock);
        return 0;
    }
    exit(1);
}
#pragma endregion

#pragma region LUONG GUI DU LIEU LEN MAN HINH
void *send_parameter_to_Monitor(void *threadArgs)
{
    volatile bool is_first_send_sopha = false;
    volatile bool is_first_send_schedule = false;
    volatile bool is_first_send_time_active = false;
    volatile bool is_first_send_card_infor = false;
    volatile bool is_first_send_mode = false;
    
    while (1)
    {
        delay(1);
        if (is_flag_opened_websocket)
        {
            if (!is_first_send_sopha)
            {
                is_first_send_sopha = true;
                char data_send1[1000] = {0};
                int data_send_len1 = sprintf(data_send1, TEMPLATE_SOPHA, CMD_CPU_TO_QT_SOPHA, sopha.sopha);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len1, data_send1);
                printf("%s\r\n", data_send1);

                printf("Da send sopha to QT OK\r\n");
            }
            if (!is_first_send_schedule)
            {
                is_first_send_schedule = true;
                char data_send2[1000] = {0};
                int data_send_len2 = sprintf(data_send2, TEMPLATE_SCHEDULE, CMD_CPU_TO_QT_SCHEDULE,
                                             schedule.xanh1,
                                             schedule.xanh2,
                                             schedule.xanh3,
                                             schedule.xanh4,
                                             schedule.vang,
                                             schedule.giaitoa);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len2, data_send2);
                printf("%s\r\n", data_send2);
                printf("Da send schedule to QT OK\r\n");
            }
            if (!is_first_send_time_active)
            {
                is_first_send_time_active = true;

                char data_send3[1000] = {0};
                int data_send_len3 = sprintf(data_send3, TEMPLATE_TIME_ACTIVE, CMD_CPU_TO_QT_TIME_ACTIVE,
                                             time_active.hh_start,
                                             time_active.mm_start,
                                             time_active.hh_end,
                                             time_active.mm_end);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_send3);
                printf("%s\r\n", data_send3);
                printf("Da send time active to QT OK\r\n");
            }

            if (!is_first_send_card_infor)
            {
                is_first_send_card_infor = true;
                for (int i = 0; i < 8; i++)
                {
                    char data_send[1000] = {0};
                    int data_send_len = sprintf(data_send, TEMPLATE_CARD_INFOR, CMD_CPU_TO_QT_CARD_INFOR,
                                                i,
                                                type_cardConfig.index[i].sn,
                                                type_cardConfig.index[i].imei,
                                                type_cardConfig.index[i].phase,
                                                type_cardConfig.index[i].is_railway_enabled,
                                                type_cardConfig.index[i].is_walking_enabled,
                                                type_cardConfig.index[i].is_dependent_phase,
                                                type_cardConfig.index[i].time_delay_dependent_phase);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                    printf("%s\r\n", data_send);
                    printf("%s", "Đã send card infor to QT OK\n");
                }
            }
            if (!is_first_send_mode)
            {
                is_first_send_mode = true;
                if (modeactive != 2)
                {
                    printf("chay vao day 1234\r\n");
                    char data_send[1000] = {0};
                    int data_send_len = sprintf(data_send, TEMPLATE_MODE, CMD_CPU_TO_QT_MODE_ACTIVE, modeactive);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                    printf("%s\r\n", data_send);
                    printf("%s:%s", "Đã send mode on/off to QT OK\n", data_send);
                    // is_start_open_wss = true;
                }
                else
                {
                    char data_send[1000] = {0};
                    int data_send_len = sprintf(data_send, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
                    cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                    printf("%s\r\n", data_send);
                    printf("%s:%s", "Đã send mode on/off to QT OK\n", data_send);
                }
                // system("./../../GPS/gps");
                // readGPS();
                // time_t rawtime;
                // struct tm * timeinfo;

                // time(&rawtime);
                // timeinfo = localtime(&rawtime);;

                // type_date_time.day=(uint8_t)atoi(gps.day);
                // type_date_time.hh=(uint8_t)atoi(gps.hour);
                // type_date_time.mm=(uint8_t)atoi(gps.minute);
                // type_date_time.ss=(uint8_t)atoi(gps.second);
                // type_date_time.date=(uint8_t)atoi(gps.day);
                // type_date_time.month=(uint8_t)atoi(gps.month);
                // type_date_time.years=2000 + ((uint8_t)atoi(gps.year));

                // printf("Time setting: %d/%d/%d %d:%d:%d\r\n", type_date_time.years,
                //     type_date_time.month,
                //     type_date_time.date,
                //     type_date_time.hh,
                //     type_date_time.mm, type_date_time.ss);

                // send_struct(CMD_CPU_TO_MASTER_SET_RTC_MANUAL, (uint8_t *)&type_date_time, sizeof(type_date_time_t));
                // printf("Da send RTC to master OK\r\n");
            }
        }
    }
}
#pragma endregion

#pragma region LUONG NHAN type_mtfc_working_message UART
void serial_get_buffer(void)
{
    // printf("Has a data\r\n");
    uint8_t c = 0;
    if (serialDataAvail(fd))
    {
        c = (uint8_t)(serialGetchar(fd));
        // cout << "This is check: " << c <<endl;
        if (c_state == IDLE)
        {
            c_state = (c == '$') ? HEADER_START : IDLE;

            if (c_state == IDLE)
            {
            }
        }
        else if (c_state == HEADER_START)
        {
            c_state = (c == 'M') ? HEADER_M : IDLE;
        }
        else if (c_state == HEADER_M)
        {
            c_state = (c == '>') ? HEADER_ARROW : IDLE;
        }
        else if (c_state == HEADER_ARROW)
        {
            if (c > 255)
            {
                c_state = IDLE;
            }
            else
            {
                dataSize = c;
                offset = 0;
                checksum = 0;
                indRX = 0;
                checksum ^= c;
                c_state = HEADER_SIZE;
            }
        }
        else if (c_state == HEADER_SIZE)
        {
            cmdMSP = c;
            checksum ^= c;
            c_state = HEADER_CMD;
        }
        else if (c_state == HEADER_CMD && offset < dataSize)
        {
            checksum ^= c;
            inBuf[offset++] = c;
        }
        else if (c_state == HEADER_CMD && offset >= dataSize)
        {
            if (checksum == c)
            {
                // printf("Has a data\r\n");
                get_data_finish();
            }
            c_state = IDLE;
        }
    }
}
#pragma endregion

#pragma region LUONG XU LY type_mtfc_working_message UART
int card = 0;
int idx = 0;
void get_data_finish(void)
{
    printf("Has a data %d\r\n", cmdMSP);
    switch (cmdMSP)
    {
    case CMD_MASTER_TO_CPU_CARD_INFOR: // CMD_MASTER_TO_CPU_CARD_INFOR

        readstruct((uint8_t *)&type_cardConfig.index[check_stt_card], sizeof(type_one_cardConfig_t));
        printf("\r\nCard config slot -------------------[%d]", check_stt_card);
        printf("\r\nsn:[%s]", type_cardConfig.index[check_stt_card].sn);
        printf("\r\nimei:[%d]", type_cardConfig.index[check_stt_card].imei);
        printf("\r\nPhase:[%d]", type_cardConfig.index[check_stt_card].phase);
        printf("\r\nis_railway_enabled:[%d]", type_cardConfig.index[check_stt_card].is_railway_enabled);
        printf("\r\nis_walking_enabled:[%d]", type_cardConfig.index[check_stt_card].is_walking_enabled);
        printf("\r\nis_dependent_phase:[%d]", type_cardConfig.index[check_stt_card].is_dependent_phase);
        printf("\r\ntime_delay_dependent_phase:[%d]", type_cardConfig.index[check_stt_card].time_delay_dependent_phase);
        printf("%s", "\r\n----------------------------------------------\n");

        if (check_stt_card == 0)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard1.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 1)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard2.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 2)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard3.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 3)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard4.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 4)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard5.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 5)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard6.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 6)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard7.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        if (check_stt_card == 7)
        {
            Write_struct_card_toFile(card_file, "/home/pi/data/card/datacard8.txt", type_cardConfig.index[check_stt_card]);
            printf("%s:%d\r\n", "Da save card", check_stt_card);
        }
        printf("%s", "Đã luu card infor to QT OK\n");
        break;

    case CMD_MASTER_TO_CPU_MODE_ACTIVE:
        modeactive = read8();
        cpu_active.mode_active = read8();
        printf("Mode on/off: %d\r\n", modeactive);
        if (modeactive == 0)
            phase = 0;
        char data_send2[200] = {0};
        int data_send_len2 = sprintf(data_send2, TEMPLATE_MODE, CMD_CPU_TO_QT_MODE_ACTIVE, modeactive);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len2, data_send2);
        // flag_control.phase_infor = 1;
        // flag_control.flash_yellow = 1;
        switchinfor = 1;
        printf("%s\r\n", data_send2);
        printf("%s", "Đã send mode on/off to QT OK\n");
        break;

    case CMD_MASTER_TO_CPU_SELECT_PHASE:
        phase = read8();
        // if ((abc != 0) &&(abc != 1))
        printf(" Phase: %d\r\n", phase);
        char data_send3[1000] = {0};
        int data_send_len3 = sprintf(data_send3, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_send3);
        // flag_control.phase_infor = 1;
        switchinfor = 1;
        printf("%s\r\n", data_send3);
        printf("%s", "Đã send phase to QT OK\n");
        break;

    case CMD_MASTER_TO_CPU_SCHEDULE_TIME:
        printf("%s", "Đã nhan schedule tu master OK\n");
        readstruct((uint8_t *)&mtfc_schedule.time, sizeof(type_time_lamp_form_t));
        mtfc_debug_print_time_form(&mtfc_schedule.time);
        break;

    case CMD_MASTER_TO_CPU_SCHEDULE_DAYS:
        printf("%s", "Đã nhan schedule tu master OK\n");
        readstruct((uint8_t *)&mtfc_schedule.days, sizeof(type_days_lamp_form_t));
        mtfc_debug_print_days_form(&mtfc_schedule.days);
        break;

    case CMD_MASTER_TO_CPU_PHASE:
        idx = read8();
        printf("%s: %d\n", "Đã nhan phase number tu master OK", idx);
        break;

    case CMD_MASTER_TO_CPU_SCHEDULE_CYCLE:
        printf("%s", "Đã nhan schedule tu master OK\n");
        {
            readstruct((uint8_t *)&mtfc_schedule.cycle.index[idx], sizeof(type_one_cycle_lamp_time_t));
            printf("\r\n%s\n", "------------------Cycle Form--------------------------");
            for (int j = 0; j < 5; j++)
            {
                printf("\n\t%03d\t%03d\t%03d\t%03d\n",
                       idx,
                       mtfc_schedule.cycle.index[idx].side[j].t_green,
                       mtfc_schedule.cycle.index[idx].side[j].t_yellow,
                       mtfc_schedule.cycle.index[idx].clearance_time_crossroads);

                char data_send[1000] = {0};
                int data_send_len = sprintf(data_send, "CPU-{\"CMD\": %d,\
                        \"index\": %d,\
                        \"line\": %d,\
                        \"t_green\": %d,\
                        \"t_yellow\": %d,\
                        \"clearance_time_crossroads\": %d}",
                                            CMD_CPU_TO_QT_SCHEDULE_CYCLE,
                                            (idx),
                                            (j),
                                            mtfc_schedule.cycle.index[idx].side[j].t_green,
                                            mtfc_schedule.cycle.index[idx].side[j].t_yellow,
                                            mtfc_schedule.cycle.index[idx].clearance_time_crossroads);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
                printf("%s\r\n", data_send);
            }
            printf("\r\n%s\n", "-----------------------------------------------------");
        }
        break;

    case CMD_MASTER_TO_CPU_ALL_SCHEDULE:
        printf("%s", "Đã nhan schedule tu master days working time ==============================>\n");
        is_resend_cycle_to_app = 0; // Tắt biến cho phép gửi lại biếu mẫu lên app cũ
        // flag_control.sttrategy_setting_infor = 1;  ///<== Send schedule den server
        {
            readstruct((uint8_t *)&cycle, sizeof(cycle_t));
            mtfc_schedule.days.index[0] = cycle.day[0];
            mtfc_schedule.days.index[1] = cycle.day[1];
            mtfc_schedule.days.index[2] = cycle.day[2];
            mtfc_schedule.days.index[3] = cycle.day[3];
            mtfc_schedule.days.index[4] = cycle.day[4];
            mtfc_schedule.days.index[5] = cycle.day[5];
            mtfc_schedule.days.index[6] = cycle.day[6];

            char data_send3[6000] = {0};
            int data_send_len3 = sprintf(data_send3, TEMPLATE_CYCLE, CMD_CPU_TO_QT_SCHEDULE_CYCLE,
                                         cycle.cycle_1[0], cycle.cycle_1[1], cycle.cycle_1[2], cycle.cycle_1[3], cycle.cycle_1[4], cycle.cycle_1[5],
                                         cycle.cycle_1[6], cycle.cycle_1[7], cycle.cycle_1[8], cycle.cycle_1[9], cycle.cycle_2[0], cycle.cycle_2[1],
                                         cycle.cycle_2[2],
                                         cycle.cycle_2[3],
                                         cycle.cycle_2[4],
                                         cycle.cycle_2[5],
                                         cycle.cycle_2[6],
                                         cycle.cycle_2[7],
                                         cycle.cycle_2[8],
                                         cycle.cycle_2[9],
                                         cycle.cycle_3[0],
                                         cycle.cycle_3[1],
                                         cycle.cycle_3[2],
                                         cycle.cycle_3[3],
                                         cycle.cycle_3[4],
                                         cycle.cycle_3[5],
                                         cycle.cycle_3[6],
                                         cycle.cycle_3[7],
                                         cycle.cycle_3[8],
                                         cycle.cycle_3[9],
                                         cycle.cycle_4[0],
                                         cycle.cycle_4[1],
                                         cycle.cycle_4[2],
                                         cycle.cycle_4[3],
                                         cycle.cycle_4[4],
                                         cycle.cycle_4[5],
                                         cycle.cycle_4[6],
                                         cycle.cycle_4[7],
                                         cycle.cycle_4[8],
                                         cycle.cycle_4[9],
                                         cycle.cycle_5[0],
                                         cycle.cycle_5[1],
                                         cycle.cycle_5[2],
                                         cycle.cycle_5[3],
                                         cycle.cycle_5[4],
                                         cycle.cycle_5[5],
                                         cycle.cycle_5[6],
                                         cycle.cycle_5[7],
                                         cycle.cycle_5[8],
                                         cycle.cycle_5[9],
                                         cycle.day[0],
                                         cycle.day[1],
                                         cycle.day[2],
                                         cycle.day[3],
                                         cycle.day[4],
                                         cycle.day[5],
                                         cycle.day[6],
                                         cycle.time.index[0].select_point[0].t_begin_apply.hour,
                                         cycle.time.index[0].select_point[0].t_begin_apply.minute,
                                         cycle.time.index[0].select_point[0].index_cycle_form,
                                         cycle.time.index[0].select_point[1].t_begin_apply.hour,
                                         cycle.time.index[0].select_point[1].t_begin_apply.minute,
                                         cycle.time.index[0].select_point[1].index_cycle_form,
                                         cycle.time.index[0].select_point[2].t_begin_apply.hour,
                                         cycle.time.index[0].select_point[2].t_begin_apply.minute,
                                         cycle.time.index[0].select_point[2].index_cycle_form,
                                         cycle.time.index[0].select_point[3].t_begin_apply.hour,
                                         cycle.time.index[0].select_point[3].t_begin_apply.minute,
                                         cycle.time.index[0].select_point[3].index_cycle_form,
                                         cycle.time.index[0].select_point[4].t_begin_apply.hour,
                                         cycle.time.index[0].select_point[4].t_begin_apply.minute,
                                         cycle.time.index[0].select_point[4].index_cycle_form,
                                         cycle.time.index[1].select_point[0].t_begin_apply.hour,
                                         cycle.time.index[1].select_point[0].t_begin_apply.minute,
                                         cycle.time.index[1].select_point[0].index_cycle_form,
                                         cycle.time.index[1].select_point[1].t_begin_apply.hour,
                                         cycle.time.index[1].select_point[1].t_begin_apply.minute,
                                         cycle.time.index[1].select_point[1].index_cycle_form,
                                         cycle.time.index[1].select_point[2].t_begin_apply.hour,
                                         cycle.time.index[1].select_point[2].t_begin_apply.minute,
                                         cycle.time.index[1].select_point[2].index_cycle_form,
                                         cycle.time.index[1].select_point[3].t_begin_apply.hour,
                                         cycle.time.index[1].select_point[3].t_begin_apply.minute,
                                         cycle.time.index[1].select_point[3].index_cycle_form,
                                         cycle.time.index[1].select_point[4].t_begin_apply.hour,
                                         cycle.time.index[1].select_point[4].t_begin_apply.minute,
                                         cycle.time.index[1].select_point[4].index_cycle_form,
                                         cycle.time.index[2].select_point[0].t_begin_apply.hour,
                                         cycle.time.index[2].select_point[0].t_begin_apply.minute,
                                         cycle.time.index[2].select_point[0].index_cycle_form,
                                         cycle.time.index[2].select_point[1].t_begin_apply.hour,
                                         cycle.time.index[2].select_point[1].t_begin_apply.minute,
                                         cycle.time.index[2].select_point[1].index_cycle_form,
                                         cycle.time.index[2].select_point[2].t_begin_apply.hour,
                                         cycle.time.index[2].select_point[2].t_begin_apply.minute,
                                         cycle.time.index[2].select_point[2].index_cycle_form,
                                         cycle.time.index[2].select_point[3].t_begin_apply.hour,
                                         cycle.time.index[2].select_point[3].t_begin_apply.minute,
                                         cycle.time.index[2].select_point[3].index_cycle_form,
                                         cycle.time.index[2].select_point[4].t_begin_apply.hour,
                                         cycle.time.index[2].select_point[4].t_begin_apply.minute,
                                         cycle.time.index[2].select_point[4].index_cycle_form);
            Write_struct_cycle_toFile(cycle_file, "/home/pi/cycle.txt", cycle);
            matching_data_to_old_app();
            cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_send3);
            /*-------------------------Nhan thong tin cap nhat tu master sau do gui len app cu----------------------------*/
            updateServerSchedule();
        }
        break;

    case CMD_MASTER_TO_CPU_ONE_SECOND_MESSAGE:
        readstruct((uint8_t *)&mtfc_card_data_out, sizeof(type_mtfc_card_data_t));
        uint8_t out[8];
        out[0] = mtfc_card_data_out.ouput[0];
        out[1] = mtfc_card_data_out.ouput[1];
        out[2] = mtfc_card_data_out.ouput[2];
        out[3] = mtfc_card_data_out.ouput[3];

        // printf("%s:[%02X]\n","out[0]",out[0]);
        // printf("%s:[%02X]\n","out[1]",out[1]);
        // printf("%s:%d\n","out[2]",out[2]);
        // printf("%s:%d\n","out[3]",out[3]);
        printf("pha 1: %d ------ pha 2: %d ------ pha 3: %d ------ phu thuoc: %d ------ status lamp dependent: %d\r\n", mtfc_card_data_out.tm_cycle.countdown[0], mtfc_card_data_out.tm_cycle.countdown[1],
               mtfc_card_data_out.tm_cycle.countdown[2], mtfc_card_data_out.tm_cycle.tm_inpendent[3], mtfc_card_data_out.tm_cycle.status_lamp_dependent[0]);

        converttobinary(out[0], 1);
        converttobinary(out[1], 2);
        converttobinary(out[2], 3);
        converttobinary(out[3], 4);

        char message1[256] = {0};
        char message2[256] = {0};
        char message3[256] = {0};
        char message4[256] = {0};
        char message5[256] = {0};
        char message6[256] = {0};
        char message7[6000] = {0};
        /// Card 1:
        if (type_cardConfig.index[0].phase != 0)
        {
            // char message1[1280]={0};
            if (type_cardConfig.index[0].is_dependent_phase == 1)
            {
                // printf("%s","Card 1 la card phu thuoc\n");
                routeId1 = -type_cardConfig.index[0].phase;
            }
            else
                routeId1 = type_cardConfig.index[0].phase;
            if (type_cardConfig.index[0].phase == 1)
            {
                cardred1 = red1;
                cardyellow1 = yellow1;
                cardgreen1 = green1;
                tm_count_card1 = mtfc_card_data_out.tm_cycle.countdown[0];
            }
            if (type_cardConfig.index[0].phase == 2)
            {
                cardred1 = red2;
                cardyellow1 = yellow2;
                cardgreen1 = green2;
                tm_count_card1 = mtfc_card_data_out.tm_cycle.countdown[1];
            }
            if (type_cardConfig.index[0].phase == 3)
            {
                cardred1 = red3;
                cardyellow1 = yellow3;
                cardgreen1 = green3;
                tm_count_card1 = mtfc_card_data_out.tm_cycle.countdown[2];
            }
            if (type_cardConfig.index[0].phase == 4)
            {
                cardred1 = red4;
                cardyellow1 = yellow4;
                cardgreen1 = green4;
            }
            if ((cardyellow1 == 0) && (cardgreen1 == 0))
            {
                if (routeId1 < 0)
                {
                    snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"red1\":%d,\"time1\":%d",
                             routeId1, cardred1, mtfc_card_data_out.tm_cycle.tm_inpendent[0]);
                }
                else
                {
                    snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"red1\":%d,\"time1\":%d",
                             routeId1, cardred1, tm_count_card1);
                }
            }
            else if ((cardred1 == 0) && (cardgreen1 == 0))
            {
                if (routeId1 < 0)
                {
                    snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"yellow1\":%d,\"time1\":%d",
                             routeId1, cardyellow1, mtfc_card_data_out.tm_cycle.tm_inpendent[0]);
                }
                else
                {
                    snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"yellow1\":%d,\"time1\":%d",
                             routeId1, cardyellow1, tm_count_card1);
                }
            }
            else
            {
                if (routeId1 < 0)
                {
                    if (mtfc_card_data_out.tm_cycle.status_lamp_dependent[0] == RED_DEPENDENT)
                    {
                        cardred1 = 1;
                        snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"red1\":%d,\"time1\":%d",
                                 routeId1, cardred1, mtfc_card_data_out.tm_cycle.tm_inpendent[0]);
                    }
                    else
                    {
                        snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"green1\":%d,\"time1\":%d",
                                 routeId1, cardgreen1, mtfc_card_data_out.tm_cycle.tm_inpendent[0]);
                    }
                }
                else
                {
                    snprintf(message1, sizeof(message1), "\"routeId1\":%d,\"green1\":%d,\"time1\":%d",
                             routeId1, cardgreen1, tm_count_card1);
                }
            }
            // printf("%s\n","Co card 1");
            // printf("%s\n",message1);
        }
        /// Card 2:
        if (type_cardConfig.index[1].phase != 0)
        {
            // char message2[1280]={0};
            if (type_cardConfig.index[1].is_dependent_phase == 1)
            {
                // printf("%s","Card 2 la card phu thuoc\n");
                routeId2 = -type_cardConfig.index[1].phase;
            }
            else
                routeId2 = type_cardConfig.index[1].phase;
            if (type_cardConfig.index[1].phase == 1)
            {
                cardred2 = red1;
                cardyellow2 = yellow1;
                cardgreen2 = green1;
                tm_count_card2 = mtfc_card_data_out.tm_cycle.countdown[0];
            }
            if (type_cardConfig.index[1].phase == 2)
            {
                cardred2 = red2;
                cardyellow2 = yellow2;
                cardgreen2 = green2;
                tm_count_card2 = mtfc_card_data_out.tm_cycle.countdown[1];
            }
            if (type_cardConfig.index[1].phase == 3)
            {
                cardred2 = red3;
                cardyellow2 = yellow3;
                cardgreen2 = green3;
                tm_count_card2 = mtfc_card_data_out.tm_cycle.countdown[2];
            }
            if (type_cardConfig.index[1].phase == 4)
            {
                cardred2 = red4;
                cardyellow2 = yellow4;
                cardgreen2 = green4;
            }
            if ((cardyellow2 == 0) && (cardgreen2 == 0))
            {
                if (routeId2 < 0)
                {
                    snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"red2\":%d,\"time2\":%d",
                             routeId2, cardred2, mtfc_card_data_out.tm_cycle.tm_inpendent[1]);
                }
                else
                {
                    snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"red2\":%d,\"time2\":%d",
                             routeId2, cardred2, tm_count_card2);
                }
            }
            else if ((cardred2 == 0) && (cardgreen2 == 0))
            {
                if (routeId2 < 0)
                {
                    snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"yellow2\":%d,\"time2\":%d",
                             routeId2, cardyellow2, mtfc_card_data_out.tm_cycle.tm_inpendent[1]);
                }
                else
                {
                    snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"yellow2\":%d,\"time2\":%d",
                             routeId2, cardyellow2, tm_count_card2);
                }
            }
            else
            {
                if (routeId2 < 0)
                {
                    if (mtfc_card_data_out.tm_cycle.status_lamp_dependent[1] == RED_DEPENDENT)
                    {
                        cardred2 = 1;
                        snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"red2\":%d,\"time2\":%d",
                                 routeId2, cardred2, mtfc_card_data_out.tm_cycle.tm_inpendent[1]);
                    }
                    else
                    {
                        snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"green2\":%d,\"time2\":%d",
                                 routeId2, cardgreen2, mtfc_card_data_out.tm_cycle.tm_inpendent[1]);
                    }
                }
                else
                {
                    snprintf(message2, sizeof(message2), "\"routeId2\":%d,\"green2\":%d,\"time2\":%d",
                             routeId2, cardgreen2, tm_count_card2);
                }
            }
            // printf("%s\n","Co card 2");

            // printf("%s\r\n", message2);
        }
        /// Card 3:
        if (type_cardConfig.index[2].phase != 0)
        {
            // char message3[1280]={0};
            if (type_cardConfig.index[2].is_dependent_phase == 1)
            {
                // printf("%s","Card 3 la card phu thuoc\n");
                routeId3 = -type_cardConfig.index[2].phase;
            }
            else
                routeId3 = type_cardConfig.index[2].phase;
            if (type_cardConfig.index[2].phase == 1)
            {
                cardred3 = red1;
                cardyellow3 = yellow1;
                cardgreen3 = green1;
                tm_count_card3 = mtfc_card_data_out.tm_cycle.countdown[0];
            }
            if (type_cardConfig.index[2].phase == 2)
            {
                cardred3 = red2;
                cardyellow3 = yellow2;
                cardgreen3 = green2;
                tm_count_card3 = mtfc_card_data_out.tm_cycle.countdown[1];
            }
            if (type_cardConfig.index[2].phase == 3)
            {
                cardred3 = red3;
                cardyellow3 = yellow3;
                cardgreen3 = green3;
                tm_count_card3 = mtfc_card_data_out.tm_cycle.countdown[2];
            }
            if (type_cardConfig.index[2].phase == 4)
            {
                cardred3 = red4;
                cardyellow3 = yellow4;
                cardgreen3 = green4;
            }
            if ((cardyellow3 == 0) && (cardgreen3 == 0))
            {
                if (routeId3 < 0)
                {
                    snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"red3\":%d,\"time3\":%d",
                             routeId3, cardred3, mtfc_card_data_out.tm_cycle.tm_inpendent[2]);
                }
                else
                {
                    snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"red3\":%d,\"time3\":%d",
                             routeId3, cardred3, tm_count_card3);
                }
            }
            else if ((cardred3 == 0) && (cardgreen3 == 0))
            {
                if (routeId3 < 0)
                {
                    snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"yellow3\":%d,\"time3\":%d",
                             routeId3, cardyellow3, mtfc_card_data_out.tm_cycle.tm_inpendent[2]);
                }
                else
                {
                    snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"yellow3\":%d,\"time3\":%d",
                             routeId3, cardyellow3, tm_count_card3);
                }
            }
            else
            {
                if (routeId3 < 0)
                {
                    if (mtfc_card_data_out.tm_cycle.status_lamp_dependent[2] == RED_DEPENDENT)
                    {
                        cardred3 = 1;
                        snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"red3\":%d,\"time3\":%d",
                                 routeId3, cardred3, mtfc_card_data_out.tm_cycle.tm_inpendent[2]);
                    }
                    else
                    {
                        snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"green3\":%d,\"time3\":%d",
                                 routeId3, cardgreen3, mtfc_card_data_out.tm_cycle.tm_inpendent[2]);
                    }
                }
                else
                {
                    snprintf(message3, sizeof(message3), "\"routeId3\":%d,\"green3\":%d,\"time3\":%d",
                             routeId3, cardgreen3, tm_count_card3);
                }
            }
            // printf("%s\n","Co card 3");
        }
        /// Card 4:
        if (type_cardConfig.index[3].phase != 0)
        {
            // char message4[1280]={0};
            if (type_cardConfig.index[3].is_dependent_phase == 1)
            {
                // printf("%s","Card 4 la card phu thuoc\n");
                routeId4 = -type_cardConfig.index[3].phase;
            }
            else
                routeId4 = type_cardConfig.index[3].phase;
            if (type_cardConfig.index[3].phase == 1)
            {
                cardred4 = red1;
                cardyellow4 = yellow1;
                cardgreen4 = green1;
                tm_count_card4 = mtfc_card_data_out.tm_cycle.countdown[0];
            }
            if (type_cardConfig.index[3].phase == 2)
            {
                cardred4 = red2;
                cardyellow4 = yellow2;
                cardgreen4 = green2;
                tm_count_card4 = mtfc_card_data_out.tm_cycle.countdown[1];
            }
            if (type_cardConfig.index[3].phase == 3)
            {
                cardred4 = red3;
                cardyellow4 = yellow3;
                cardgreen4 = green3;
                tm_count_card4 = mtfc_card_data_out.tm_cycle.countdown[2];
            }
            if (type_cardConfig.index[3].phase == 4)
            {
                cardred4 = red4;
                cardyellow4 = yellow4;
                cardgreen4 = green4;
            }
            if ((cardyellow4 == 0) && (cardgreen4 == 0))
            {
                if (routeId4 < 0)
                {
                    snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"red4\":%d,\"time4\":%d",
                             routeId4, cardred4, mtfc_card_data_out.tm_cycle.tm_inpendent[3]);
                }
                else
                {
                    printf("%s:%d\r\n", "card 4 vao red 4:", tm_count_card4);
                    snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"red4\":%d,\"time4\":%d",
                             routeId4, cardred4, tm_count_card4);
                }
            }
            else if ((cardred4 == 0) && (cardgreen4 == 0))
            {
                if (routeId4 < 0)
                {
                    snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"yellow4\":%d,\"time4\":%d",
                             routeId4, cardyellow4, mtfc_card_data_out.tm_cycle.tm_inpendent[3]);
                }
                else
                {
                    snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"yellow4\":%d,\"time4\":%d",
                             routeId4, cardyellow4, tm_count_card4);
                }
            }
            else
            {
                if (routeId4 < 0)
                {
                    if (mtfc_card_data_out.tm_cycle.status_lamp_dependent[3] == RED_DEPENDENT)
                    {
                        cardred4 = 1;
                        snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"red4\":%d,\"time4\":%d",
                                 routeId4, cardred4, mtfc_card_data_out.tm_cycle.tm_inpendent[3]);
                    }
                    else
                    {
                        snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"green4\":%d,\"time4\":%d",
                                 routeId4, cardgreen4, mtfc_card_data_out.tm_cycle.tm_inpendent[3]);
                    }
                }
                else
                {
                    snprintf(message4, sizeof(message4), "\"routeId4\":%d,\"green4\":%d,\"time4\":%d",
                             routeId4, cardgreen4, tm_count_card4);
                }
            }
            // printf("%s\r\n", message4);
        }
        /// Card 5:
        if (type_cardConfig.index[4].phase != 0)
        {
            // char message5[1280]={0};
            if (type_cardConfig.index[4].is_dependent_phase == 1)
            {
                // printf("%s","Card 5 la card phu thuoc\n");
                routeId5 = -type_cardConfig.index[4].phase;
            }
            else
                routeId5 = type_cardConfig.index[4].phase;
            if (type_cardConfig.index[4].phase == 1)
            {
                cardred5 = red1;
                cardyellow5 = yellow1;
                cardgreen5 = green1;
                tm_count_card5 = mtfc_card_data_out.tm_cycle.countdown[0];
            }
            if (type_cardConfig.index[4].phase == 2)
            {
                cardred5 = red2;
                cardyellow5 = yellow2;
                cardgreen5 = green2;
                tm_count_card5 = mtfc_card_data_out.tm_cycle.countdown[1];
            }
            if (type_cardConfig.index[4].phase == 3)
            {
                cardred5 = red3;
                cardyellow5 = yellow3;
                cardgreen5 = green3;
                tm_count_card5 = mtfc_card_data_out.tm_cycle.countdown[2];
            }
            if (type_cardConfig.index[4].phase == 4)
            {
                cardred5 = red4;
                cardyellow5 = yellow4;
                cardgreen5 = green4;
            }
            if ((cardyellow5 == 0) && (cardgreen5 == 0))
            {
                if (routeId5 < 0)
                {
                    snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"red5\":%d,\"time5\":%d",
                             routeId5, cardred5, mtfc_card_data_out.tm_cycle.tm_inpendent[4]);
                }
                else
                {
                    snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"red5\":%d,\"time5\":%d",
                             routeId5, cardred5, tm_count_card5);
                }
            }
            else if ((cardred5 == 0) && (cardgreen5 == 0))
            {
                if (routeId5 < 0)
                {
                    snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"yellow5\":%d,\"time5\":%d",
                             routeId5, cardyellow5, mtfc_card_data_out.tm_cycle.tm_inpendent[4]);
                }
                else
                {
                    snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"yellow5\":%d,\"time5\":%d",
                             routeId5, cardyellow5, tm_count_card5);
                }
            }
            else
            {
                if (routeId5 < 0)
                {
                    if (mtfc_card_data_out.tm_cycle.status_lamp_dependent[4] == RED_DEPENDENT)
                    {
                        cardred5 = 1;
                        snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"red5\":%d,\"time5\":%d",
                                 routeId5, cardred5, mtfc_card_data_out.tm_cycle.tm_inpendent[4]);
                    }
                    else
                    {
                        snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"green5\":%d,\"time5\":%d",
                                 routeId5, cardgreen5, mtfc_card_data_out.tm_cycle.tm_inpendent[4]);
                    }
                }
                else
                {
                    snprintf(message5, sizeof(message5), "\"routeId5\":%d,\"green5\":%d,\"time5\":%d",
                             routeId5, cardgreen5, tm_count_card5);
                }
            }
            // printf("%s\n","Co card 5");
        }
        /// Card 6:
        if (type_cardConfig.index[5].phase != 0)
        {
            // char message6[1280]={0};
            if (type_cardConfig.index[5].is_dependent_phase == 1)
            {
                printf("%s", "Card 6 la card phu thuoc\n");
                routeId6 = -type_cardConfig.index[5].phase;
            }
            else
                routeId6 = type_cardConfig.index[5].phase;
            if (type_cardConfig.index[5].phase == 1)
            {
                cardred6 = red1;
                cardyellow6 = yellow1;
                cardgreen6 = green1;
                tm_count_card6 = mtfc_card_data_out.tm_cycle.countdown[0];
            }
            if (type_cardConfig.index[5].phase == 2)
            {
                cardred6 = red2;
                cardyellow6 = yellow2;
                cardgreen6 = green2;
                tm_count_card6 = mtfc_card_data_out.tm_cycle.countdown[1];
            }
            if (type_cardConfig.index[5].phase == 3)
            {
                cardred6 = red3;
                cardyellow6 = yellow3;
                cardgreen6 = green3;
                tm_count_card6 = mtfc_card_data_out.tm_cycle.countdown[2];
            }
            if (type_cardConfig.index[5].phase == 4)
            {
                cardred6 = red4;
                cardyellow6 = yellow4;
                cardgreen6 = green4;
            }
            if ((cardyellow6 == 0) && (cardgreen6 == 0))
            {
                if (routeId6 < 0)
                {
                    snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"red6\":%d,\"time6\":%d",
                             routeId6, cardred6, mtfc_card_data_out.tm_cycle.tm_inpendent[5]);
                }
                else
                {
                    snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"red6\":%d,\"time6\":%d",
                             routeId6, cardred6, tm_count_card6);
                }
            }
            else if ((cardred6 == 0) && (cardgreen6 == 0))
            {
                if (routeId6 < 0)
                {
                    snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"yellow6\":%d,\"time6\":%d",
                             routeId6, cardyellow6, mtfc_card_data_out.tm_cycle.tm_inpendent[5]);
                }
                else
                {
                    snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"yellow6\":%d,\"time6\":%d",
                             routeId6, cardyellow6, tm_count_card6);
                }
            }
            else
            {
                if (routeId6 < 0)
                {
                    if (mtfc_card_data_out.tm_cycle.status_lamp_dependent[5] == RED_DEPENDENT)
                    {
                        cardred6 = 1;
                        snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"red6\":%d,\"time6\":%d",
                                 routeId6, cardgreen6, mtfc_card_data_out.tm_cycle.tm_inpendent[5]);
                    }
                    else
                    {
                        snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"green6\":%d,\"time6\":%d",
                                 routeId6, cardgreen6, mtfc_card_data_out.tm_cycle.tm_inpendent[5]);
                    }
                }
                else
                {
                    snprintf(message6, sizeof(message6), "\"routeId6\":%d,\"green6\":%d,\"time6\":%d",
                             routeId6, cardgreen6, tm_count_card6);
                }
            }
            // printf("%s\n","Co card 6");
        }
        if ((type_cardConfig.index[6].phase == 0) && (type_cardConfig.index[5].phase != 0))
            snprintf(message7, sizeof(message7), "%s,%s,%s,%s,%s,%s",
                     message1, message2, message3, message4, message5, message6);
        if ((type_cardConfig.index[5].phase == 0) && (type_cardConfig.index[4].phase != 0))
            snprintf(message7, sizeof(message7), "%s,%s,%s,%s,%s",
                     message1, message2, message3, message4, message5);
        if ((type_cardConfig.index[4].phase == 0) && (type_cardConfig.index[3].phase != 0))
            snprintf(message7, sizeof(message7), "%s,%s,%s,%s",
                     message1, message2, message3, message4);
        if ((type_cardConfig.index[3].phase == 0) && (type_cardConfig.index[2].phase != 0))
            snprintf(message7, sizeof(message7), "%s,%s,%s",
                     message1, message2, message3);
        if ((type_cardConfig.index[2].phase == 0) && (type_cardConfig.index[1].phase != 0))
            snprintf(message7, sizeof(message7), "%s,%s",
                     message1, message2);
        // printf("%s:%s\n","Đã nhan data out tu master",message7);

        char mess_rtc_current_tmp[256] = {0};

        snprintf(mess_rtc_current_tmp, sizeof(mess_rtc_current_tmp), "\"hh\":%d,\"mm\":%d",
                 mtfc_card_data_out.type_date_time.hh, mtfc_card_data_out.type_date_time.mm);

        char data_send4[8000] = {0};
        int data_send_len4 = sprintf(data_send4, TEMPLATE_STATUS, CMD_CPU_TO_QT_STATUS, message7, mess_rtc_current_tmp);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len4, data_send4);
        printf("%s\r\n",data_send4);
        printf("keep allive CPU: %d\r\n", keepAlliveCPU);
        keepAlliveCPU -= 10;
        if (keepAlliveCPU < 0)
        {
            keepAlliveCPU = 0;
        }
        updateServer();
        // flag_control.controller_check_infor = 1; // bat co send mqtt
        break;

    case CMD_MASTER_TO_CPU_CHECK_CARD: // CMD_MASTER_TO_CPU_CHECK_CARD
        check_stt_card = read8() - 1;
        // char data_send3[1000]={0};
        // int data_send_len3 = sprintf(data_send3, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
        // cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len3, data_send3);
        // printf("%s\r\n",data_send3);
        printf("%s:%d\n", "Check card", check_stt_card);

        break;

    case CMD_MASTER_TO_CPU_CURRENT:
        readstruct((uint8_t *)&time_current, sizeof(time_current_t));
        int data_send_len5 = sprintf(mtfc_data_tmp_current_one_sec, TEMPLATE_CURRENT_STATUS, CMD_CPU_TO_QT_CURRENT,
                                     type_cardConfig.index[0].is_dependent_phase,
                                     type_cardConfig.index[1].is_dependent_phase,
                                     type_cardConfig.index[2].is_dependent_phase,
                                     type_cardConfig.index[3].is_dependent_phase,
                                     type_cardConfig.index[0].is_railway_enabled,
                                     type_cardConfig.index[1].is_railway_enabled,
                                     type_cardConfig.index[2].is_railway_enabled,
                                     type_cardConfig.index[3].is_railway_enabled,
                                     1,
                                     type_cardConfig.index[0].sn,
                                     0,
                                     setting.server,
                                     time_current.time_form,
                                     time_current.time_line,
                                     time_current.time_index,
                                     time_current.time_begin_hour_apply,
                                     time_current.time_begin_minute_apply,
                                     time_current.t,
                                     time_current.num_side,
                                     time_current.yellow,
                                     time_current.giaitoa,
                                     time_current.green_1,
                                     time_current.red_1,
                                     time_current.green_2,
                                     time_current.red_2,
                                     time_current.green_3,
                                     time_current.red_3,
                                     time_current.green_4,
                                     time_current.red_4);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len5, mtfc_data_tmp_current_one_sec);
        // printf("\r\n\tTime form current: %02d\r\n\tTime line current: %02d\r\n\tTime index current: %02d\r\n\t", time_current.time_form, time_current.time_line, time_current.time_index);
        // printf("\rCMD_MASTER_TO_CPU_CURRENT: %s\r\n",mtfc_data_tmp_current_one_sec);
        // printf("%d , %d : %d , %d green 1 , red 1 : green 2 , red 2\r\n", time_current.green_1, time_current.red_1, time_current.green_2, time_current.red_2);
        sopha.sopha = time_current.num_side;
        break;
    case CMD_MASTER_TO_CPU_FB:
        flat_fb_to_qt = read8();
        char data_send6[1000] = {0};
        int data_send_len6 = sprintf(data_send6, TEMPLATE_CPU_TO_QT_FB, CMD_CPU_TO_QT_FB, flat_fb_to_qt);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len6, data_send6);
        // printf("%s\r\n",data_send6);
        break;
    default:
        break;
    }
}

void converttobinary(int n, int m)
{
    int a[10], i;
    for (i = 0; n > 0; i++)
    {
        a[i] = n % 2;
        n = n / 2;
    }

    // printf("\nDang nhi phan cua so vua nhap la = ");
    // for(i=i-1;i>=0;i--)
    {
        // printf("Do[%d]:   %d\n",m,a[2]);
        if (m == 1)
            red1 = a[2];
        if (m == 2)
            red2 = a[2];
        if (m == 3)
            red3 = a[2];
        if (m == 4)
            red4 = a[2];
        // printf("Vang[%d]: %d\n",m,a[1]);
        if (m == 1)
            yellow1 = a[1];
        if (m == 2)
            yellow2 = a[1];
        if (m == 3)
            yellow3 = a[1];
        if (m == 4)
            yellow4 = a[1];
        // printf("Xanh[%d]: %d\n",m,a[0]);
        if (m == 1)
            green1 = a[0];
        if (m == 2)
            green2 = a[0];
        if (m == 3)
            green3 = a[0];
        if (m == 4)
            green4 = a[0];
    }
    //
}

void mtfc_debug_print_time_form(type_time_lamp_form_t *time_form)
{
    // printf("\r\n%s", "------------------Time Form------------------------");
    // printf("\r\n%s", "Index\tTimeline\tBegin\t\tCycleForm");
    for (int i = 0; i < MAX_TIME_FORM; i++)
    {
        printf("\r\n[%02d]", (i + 1));
        for (int j = 0; j < MAX_TIME_FORM; j++)
        {
            printf("\t[%02d]", (j + 1));
            printf("\t[%03d %03d]\t\t[%02d]", time_form->index[i].select_point[j].t_begin_apply.hour, time_form->index[i].select_point[j].t_begin_apply.minute, time_form->index[i].select_point[j].index_cycle_form);
            printf("%s", "\r\n");

            char data_send[1000] = {0};
            int data_send_len = sprintf(data_send, "CPU-{\"CMD\": %d,\
                \"index\": %d,\
                \"timeline\": %d,\
                \"time_form_t_begin_apply_hour\": %d,\
                \"time_form_t_begin_apply_minute\": %d,\
                \"time_form_index_cycle_form\": %d}",
                                        CMD_CPU_TO_QT_SCHEDULE_TIME,
                                        (i),
                                        (j),
                                        time_form->index[i].select_point[j].t_begin_apply.hour,
                                        time_form->index[i].select_point[j].t_begin_apply.minute,
                                        time_form->index[i].select_point[j].index_cycle_form);
            // cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
            // printf("%s\r\n",data_send);
        }
    }
}

void mtfc_caclator_one_cycle_form(type_one_cycle_lamp_time_t *dat)
{
    for (uint8_t i = 0; i < dat->num_side_used; i++)
    {
        dat->period_crossroads += dat->side[i].t_green;
    }
    dat->period_crossroads = dat->period_crossroads + dat->num_side_used * (dat->clearance_time_crossroads + dat->side[0].t_yellow);
    for (uint8_t i = 0; i < dat->num_side_used; i++)
    {
        dat->side[i].t_red = dat->period_crossroads - (dat->side[i].t_green + dat->side[i].t_yellow);
    }
    if (dat->num_side_used > 0)
    {
        dat->side[0].t_start_green = 0;
        dat->side[0].t_end_green = dat->side[0].t_green;
        dat->side[0].t_start_yellow = dat->side[0].t_end_green;
        dat->side[0].t_end_yellow = dat->side[0].t_start_yellow + dat->side[0].t_yellow;
    }
    //.Tinh cac pha khac
    for (uint8_t i = 1; i < dat->num_side_used; i++)
    {
        dat->side[i].t_start_green = dat->side[i - 1].t_end_yellow + dat->clearance_time_crossroads;
        dat->side[i].t_end_green = dat->side[i].t_start_green + dat->side[i].t_green;
        dat->side[i].t_start_yellow = dat->side[i].t_end_green;
        dat->side[i].t_end_yellow = dat->side[i].t_start_yellow + dat->side[i].t_yellow;
    }
    // Xoa cac pha khong su dung
    for (int i = dat->num_side_used; i < MAX_SIDE; i++)
    {
        dat->side[i].t_start_green = 0;
        dat->side[i].t_end_green = 0;
        dat->side[i].t_start_yellow = 0;
        dat->side[i].t_end_yellow = 0;
    }
}

void mtfc_contruct_all_cycle_user_config(type_cycle_lamp_form_t *dat, uint8_t num_phase)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        dat->index[i].num_side_used = num_phase;
        dat->index[i].period_crossroads = 0;
        for (uint8_t j = num_phase; j < MAX_SIDE; j++)
        {
            dat->index[i].side[j].t_green = mtfc_unset;
            dat->index[i].side[j].t_yellow = mtfc_unset;
            dat->index[i].side[j].t_red = mtfc_unset;
        }
        mtfc_caclator_one_cycle_form(&dat->index[i]);
    }
}

void matching_data_to_old_app() // thang nm
{
    printf("\r\n%s\n", "------------------Cycle Form--------------------------");
    // for (int i = 0; i < 10 ; i++) // 7 bieu mau 5 chien luoc
    // {
    //     printf("\r\ncycle.cycle_1_x%d:%d",i, cycle.cycle_1[i]);
    //     printf("\r\ncycle.cycle_2_x%d:%d",i, cycle.cycle_2[i]);
    //     printf("\r\ncycle.cycle_3_x%d:%d",i, cycle.cycle_3[i]);
    //     printf("\r\ncycle.cycle_4_x%d:%d",i, cycle.cycle_4[i]);
    //     printf("\r\ncycle.cycle_5_x%d:%d",i, cycle.cycle_5[i]);
    // }

    mtfc_schedule.days.index[0] = cycle.day[0];
    mtfc_schedule.days.index[1] = cycle.day[1];
    mtfc_schedule.days.index[2] = cycle.day[2];
    mtfc_schedule.days.index[3] = cycle.day[3];
    mtfc_schedule.days.index[4] = cycle.day[4];
    mtfc_schedule.days.index[5] = cycle.day[5];
    mtfc_schedule.days.index[6] = cycle.day[6];
    mtfc_schedule.num_side = sopha.sopha;

    for (int i = 0; i < 8; i++)
    {
        // if(i >= 0 && i < 8){
        for (int j = 0; j < 8; j++)
        {
            if (i == 0)
            {
                mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_1[j];
                // printf("T Green When i = 0: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            }
            else if (i == 1)
            {
                mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_2[j];
                // printf("T Green When i = 1: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            }
            else if (i == 2)
            {
                mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_3[j];
                // printf("T Green When i = 2: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            }
            else if (i == 3)
            {
                mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_4[j];
                // printf("T Green When i = 3: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            }
            else if (i == 4)
            {
                mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_5[j];
                // printf("T Green When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            }
            // else if(i == 5){
            //     mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_6[j];
            //     // printf("T Green When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            // }
            // else if(i == 6){
            //     mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_7[j];
            //     // printf("T Green When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            // }
            // else if(i == 7){
            //     mtfc_schedule.cycle.index[i].side[j].t_green = cycle.cycle_8[j];
            //     // printf("T Green When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_green);
            // }
            // }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i == 0)
            {
                mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_1[8];
                // printf("T Yellow When i = 0: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            }
            else if (i == 1)
            {
                mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_2[8];
                // printf("T Yellow When i = 1: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            }
            else if (i == 2)
            {
                mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_3[8];
                // printf("T Yellow When i = 2: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            }
            else if (i == 3)
            {
                mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_4[8];
                // printf("T Yellow When i = 3: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            }
            else if (i == 4)
            {
                mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_5[8];
                // printf("T Yellow When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            }
            // else if (i == 5){
            //     mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_6[8];
            //     // printf("T Yellow When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            // }
            // else if (i == 6){
            //     mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_7[8];
            //     // printf("T Yellow When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            // }
            // else if (i == 7){
            //     mtfc_schedule.cycle.index[i].side[j].t_yellow = cycle.cycle_8[8];
            //     // printf("T Yellow When i = 4: %d\r\n", mtfc_schedule.cycle.index[i].side[j].t_yellow);
            // }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i == 0)
            {
                mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_1[9];
            }
            else if (i == 1)
            {
                mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_2[9];
            }
            else if (i == 2)
            {
                mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_3[9];
            }
            else if (i == 3)
            {
                mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_4[9];
            }
            else if (i == 4)
            {
                mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_5[9];
            }
            // else if(i == 5){
            //     mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_6[9];
            // }
            // else if(i == 6){
            //     mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_7[9];
            // }
            // else if(i == 7){
            //     mtfc_schedule.cycle.index[i].clearance_time_crossroads = cycle.cycle_8[9];
            // }
        }
    }

    mtfc_schedule.time = cycle.time;
    mtfc_contruct_all_cycle_user_config(&mtfc_schedule.cycle, mtfc_schedule.num_side);
    mtfc_debug_print_all_cycle_form(&mtfc_schedule.cycle);
    mtfc_debug_print_time_form(&mtfc_schedule.time);
    mtfc_debug_print_days_form(&mtfc_schedule.days);
}

const char string_name_days[7][5] =
    {
        "SUN",
        "MON",
        "TUE",
        "WED",
        "THU",
        "FRI",
        "SAT",
};

void mtfc_debug_print_days_form(type_days_lamp_form_t *days_form)
{
    printf("\r\n%s", "------------------Days Form--------------------------");
    printf("\r\n%s", "Days\tIndex Time Form");
    for (uint8_t i = 0; i < 7; i++)
    {
        printf("\r\n[%s]\t[%03d]", string_name_days[i], days_form->index[i]);
        char data_send[1000] = {0};
        int data_send_len = sprintf(data_send, "CPU-{\"CMD\": %d,\
            \"days\": %d,\
            \"days_form\": %d}",
                                    CMD_CPU_TO_QT_SCHEDULE_DAYS,
                                    i,
                                    days_form->index[i]);
        // cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
        printf("%s\r\n", data_send);
    }
    printf("\r\n%s", "-----------------------------------------------------");
}

void mtfc_debug_print_cycle_form(type_cycle_lamp_form_t *dat, uint8_t idx)
{
    // printf("\r\n%s", "Idex\tNumLine\tCycle\tClearanceT\tLine\tGreen\tRed\tYellow\tSgreen\tEgreen\tSyellow\tEyellow");
    // printf("\r\n[%02d]", idx + 1);
    // printf("\t%02d\t%03d\t%02d",
    //     dat->index[idx].num_side_used,
    //     dat->index[idx].period_crossroads,
    // dat->index[idx].clearance_time_crossroads);
    for (uint8_t j = 0; j < MAX_SIDE; j++)
    {
        if (j != 0)
            printf("\t\t\t\t\t%02d", j + 1);
        else
            printf("\t\t%02d", j + 1);
        printf("\t%03d\t%03d\t%03d",
               dat->index[idx].side[j].t_green,
               dat->index[idx].side[j].t_red,
               dat->index[idx].side[j].t_yellow);
        printf("\t%03d\t%03d",
               dat->index[idx].side[j].t_start_green,
               dat->index[idx].side[j].t_end_green);
        printf("\t%03d\t%03d\r\n",
               dat->index[idx].side[j].t_start_yellow,
               dat->index[idx].side[j].t_end_yellow);
        char data_send[1000] = {0};
        int data_send_len = sprintf(data_send, "CPU-{\"CMD\": %d,\
            \"index\": %d,\
            \"line\": %d,\
            \"t_green\": %d,\
            \"t_yellow\": %d,\
            \"clearance_time_crossroads\": %d}",
                                    CMD_CPU_TO_QT_SCHEDULE_CYCLE,
                                    (idx),
                                    (j),
                                    dat->index[idx].side[j].t_green,
                                    dat->index[idx].side[j].t_yellow,
                                    dat->index[idx].clearance_time_crossroads);
        // printf("%s\r\n",data_send);
    }
}

void mtfc_debug_print_all_cycle_form(type_cycle_lamp_form_t *dat)
{
    for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
    {
        mtfc_debug_print_cycle_form(dat, i);
    }
}

void *client_refresher(void *client)
{
    while (1)
    {
        mqtt_sync((struct mqtt_client *)client);
        usleep(100000U);
    }
    return NULL;
}

void publish_callback(void **unused, struct mqtt_response_publish *published)
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("Received publish('%s'): %s\n", topic_name, (const char *)published->application_message);
    char *temp = strtok(topic_name, "/");
    temp = strtok(NULL, "\n");
    printf("[%.*s]\n", strlen(temp), (char *)temp);
    char *toppic = strtok(temp, "/");
    toppic = strtok(NULL, "\n");
    printf("[%.*s]\n", strlen(toppic), (char *)toppic);
    printf("Parsed toppic is: %s\n", toppic);
    void *data = (char *)published->application_message;

    if (!strncmp(toppic, "route-setting-infor", 19))
    {
        printf("Server yeu cau kiem tra so pha\r\n");
        flag_control.route_setting_infor = 1;
    }

    if (!strncmp(toppic, "route-setting", 13))
    // if (!strncmp(data, "{\"reqinfo\"", 10))
    {
        printf("Server yeu cau thay doi so pha\r\n");
        // send_struct(CMD_CPU_TO_MASTER_COUNT_LAMP, (uint8_t *)&counters_package, sizeof(type_configPackage_t));
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        json_object_get_int(new_obj);
        if (!json_object_object_get_ex(new_obj, "route", &o))
        {
            printf("Field %s does not exist\n", "route");
            return;
        }
        sopha.sopha = (uint8_t)json_object_get_int(o);
        printf("so pha is: %d\n", sopha.sopha);
        Write_struct_sopha_toFile(schedule_file, "/home/pi/sopha.txt", sopha);
        printf("save sopha to file\r\n");
        char data_send[1000] = {0};
        int data_send_len = sprintf(data_send, TEMPLATE_SOPHA, CMD_CPU_TO_QT_SOPHA, sopha.sopha);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send);
        printf("%s\r\n", data_send);
        printf("Da send sopha to QT OK\r\n");
        send_byte(CMD_CPU_TO_MASTER_SOPHA, sopha.sopha);
        printf("Da send sopha to master OK\r\n");
        flag_control.route_setting = 1;
    }

    if (!strncmp(toppic, "time-setting", 12))
    {
        printf("Server yeu cau thay doi so pha\r\n");
        // send_struct(CMD_CPU_TO_MASTER_COUNT_LAMP, (uint8_t *)&counters_package, sizeof(type_configPackage_t));
        // send_struct(CMD_CPU_TO_MASTER_COUNT_LAMP, (uint8_t *)&counters_package, sizeof(type_configPackage_t));
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        printf("save time active to file\r\n");
        if (!json_object_object_get_ex(new_obj, "hh_start", &o))
        {
            printf("Field %s does not exist\n", "hh_start");
            return;
        }
        time_active.hh_start = (uint8_t)json_object_get_int(o);
        printf("\nhh_start is: %d", time_active.hh_start);

        if (!json_object_object_get_ex(new_obj, "mm_start", &o))
        {
            printf("Field %s does not exist\n", "mm_start");
            return;
        }
        time_active.mm_start = (uint8_t)json_object_get_int(o);
        printf("\nmm_start is: %d", time_active.mm_start);

        if (!json_object_object_get_ex(new_obj, "hh_end", &o))
        {
            printf("Field %s does not exist\n", "hh_end");
            return;
        }
        time_active.hh_end = (uint8_t)json_object_get_int(o);
        printf("\nhh_end is: %d", time_active.hh_end);

        if (!json_object_object_get_ex(new_obj, "mm_end", &o))
        {
            printf("Field %s does not exist\n", "mm_end");
            return;
        }
        time_active.mm_end = (uint8_t)json_object_get_int(o);
        printf("\nmm_end is: %d", time_active.mm_end);
        Write_struct_time_active_toFile(time_active_file, "/home/pi/time_active.txt", time_active);

        char data_send_active_time[1000] = {0};
        int data_send_active_time_len = sprintf(data_send_active_time, TEMPLATE_TIME_ACTIVE, CMD_CPU_TO_QT_TIME_ACTIVE,
                                                time_active.hh_start,
                                                time_active.mm_start,
                                                time_active.hh_end,
                                                time_active.mm_end);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_active_time_len, data_send_active_time);

        printf("Da send time active to QT OK\r\n");
        send_struct(CMD_CPU_TO_MASTER_TIME_ACTIVE, (uint8_t *)&time_active, sizeof(time_active_t));
        printf("Da send time active to master OK\r\n");
        flag_control.route_setting = 1;
    }

    if (!strncmp(toppic, "sync-time", 9))
    {
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        printf("Server yeu cau dong bo thoi gian\r\n");
        if (!json_object_object_get_ex(new_obj, "currentTime", &o))
        {
            return;
        }
        char t[64];
        // str = json_object_get_string(o);
        // printf("Bieu mau ngay thu 2: %s OK\r\n", str);
        // year
        strncpy(t, json_object_get_string(o), 4); /*Cắt 5 ký tự từ vị trí thứ 3 */
        t[4] = '\0';                              /*Thêm ký tự kết thúc chuỗi vào kết quả*/
        printf("%s\n", t);
        type_date_time.years = atoi(t);
        printf("%d\n", type_date_time.years);
        // month
        strncpy(t, json_object_get_string(o) + 5, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
        t[2] = '\0';                                  /*Thêm ký tự kết thúc chuỗi vào kết quả*/
        printf("%s\n", t);
        type_date_time.month = atoi(t);
        printf("%d\n", type_date_time.month);
        // date
        strncpy(t, json_object_get_string(o) + 8, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
        t[2] = '\0';                                  /*Thêm ký tự kết thúc chuỗi vào kết quả*/
        printf("%s\n", t);
        type_date_time.date = atoi(t);
        printf("%d\n", type_date_time.date);
        // hour
        strncpy(t, json_object_get_string(o) + 11, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
        t[2] = '\0';                                   /*Thêm ký tự kết thúc chuỗi vào kết quả*/
        printf("%s\n", t);
        type_date_time.hh = atoi(t) + 7;
        printf("%d\n", type_date_time.hh);
        // minute
        strncpy(t, json_object_get_string(o) + 14, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
        t[2] = '\0';                                   /*Thêm ký tự kết thúc chuỗi vào kết quả*/
        printf("%s\n", t);
        type_date_time.mm = atoi(t);
        printf("%d\n", type_date_time.mm);
        // second
        strncpy(t, json_object_get_string(o) + 17, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
        t[2] = '\0';                                   /*Thêm ký tự kết thúc chuỗi vào kết quả*/
        printf("%s\n", t);
        type_date_time.ss = atoi(t);
        printf("%d\n", type_date_time.ss);

        if (!json_object_object_get_ex(new_obj, "day", &o))
        {
            return;
        }
        type_date_time.day = json_object_get_int(o); // Lay du lieu cai thang
        // if (!json_object_object_get_ex(new_obj, "date", &o))
        //  {
        //  return;
        //  }
        //  type_date_time.date = json_object_get_int(o); // Lay du lieu cai ngay
        //  if (!json_object_object_get_ex(new_obj, "day", &o))
        //  {
        //  return;
        //  }
        //  type_date_time.day = json_object_get_int(o); // Lay du lieu thu
        //  if (!json_object_object_get_ex(new_obj, "hh", &o))
        //  {
        //  return;
        //  }
        //  type_date_time.hh = json_object_get_int(o); // Lay du lieu cai gio
        //  if (!json_object_object_get_ex(new_obj, "mm", &o))
        //  {
        //  return;
        //  }
        //  type_date_time.mm = json_object_get_int(o); // Lay du lieu cai gio
        //  type_date_time.ss = 0;

        // printf("Time set ting: %d/%d/%d %d:%d:%d\r\n", type_date_time.years,
        // type_date_time.month,
        // type_date_time.date,
        // type_date_time.hh,
        // type_date_time.mm, 0);

        char timeSeting_temp[200];
        // sudo date --set="2012/12/12 12:12:12"
        // sudo hwclock --set --date "2012/12/12 12:12:12"
        int len = sprintf(timeSeting_temp, "sudo hwclock --set --date \"%d/%d/%d %d:%d:%d +07\"", type_date_time.years,
                          type_date_time.month,
                          type_date_time.date,
                          type_date_time.hh,
                          type_date_time.mm, 0);
        printf("%s\r\n", timeSeting_temp);
        system(timeSeting_temp);
        delay(100);
        memset(timeSeting_temp, 0, sizeof(timeSeting_temp));
        len = sprintf(timeSeting_temp, "sudo date --set=\"%d/%d/%d %d:%d:%d\"", type_date_time.years,
                      type_date_time.month,
                      type_date_time.date,
                      type_date_time.hh,
                      type_date_time.mm, 0);
        printf("%s\r\n", timeSeting_temp);
        system(timeSeting_temp);
        delay(100);
        char data[1000];
        len = sprintf(data, "CPU-{\"CMD\": %d}", CMD_QT_TO_CPU_SET_RTC_MANUAL);
        cl->send_ex(cl, UWSC_OP_TEXT, 1, len, data);
        memset(data, 0, sizeof(data));
        printf("send status set new time success\r\n");
        send_struct(CMD_CPU_TO_MASTER_SET_RTC_MANUAL, (uint8_t *)&type_date_time, sizeof(type_date_time_t));
        printf("Da send RTC to master OK\r\n");
        // break;
        flag_control.sync_time = 1;
        flag_send_data_to_qt.is_active_time = 1;
    }

    if (!strncmp(toppic, "pinout-setting", 14))
    {
        printf("Server yeu cau chuyen doi trang thai den chu thap\r\n");
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        if (!json_object_object_get_ex(new_obj, "pinout", &o))
        {
            return;
        }
        mode_cross = json_object_get_int(o);
        send_byte(CMD_CPU_TO_MASTER_MODE_CROSS, mode_cross);
        printf("Da send mode cross to master OK\r\n");
        flag_control.pinout_setting = 1;
    }

    if (!strncmp(toppic, "strategy-setting", 16))
    {
        printf("Server yeu cau thay doi chien luoc\r\n");
        struct json_object *new_obj, *template_server, *timeForm, *timeFormDetail, *cycle1, *cycleDetail, *route;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;

        if (json_object_object_get_ex(new_obj, "timeForm", &o))
        {
            // if(json_object_object_get_ex(timeForm, "name", &timeFormDetail))
            {
                for (int i = 0; i < json_object_array_length(o); i++)
                {
                    timeForm = json_object_array_get_idx(o, i);
                    if (json_object_object_get_ex(timeForm, "name", &timeFormDetail))
                    {
                        char q[64];
                        strncpy(q, json_object_get_string(timeFormDetail) + 13, 1);
                        q[1] = '\0';
                        if (atoi(q) == 1)
                        {
                            // printf("Biểu mẫu 1\r\n");
                            if (json_object_object_get_ex(timeForm, "details", &timeFormDetail))
                            {
                                // printf("Detail 1\r\n");
                                for (int j = 0; j < json_object_array_length(timeFormDetail); j++)
                                {
                                    cycle1 = json_object_array_get_idx(timeFormDetail, j);
                                    if (json_object_object_get_ex(cycle1, "timeline", &cycleDetail))
                                    {
                                        // printf("timeline %d\r\n", json_object_get_int(cycleDetail));
                                    }
                                    if (json_object_object_get_ex(cycle1, "detail_id", &cycleDetail))
                                    {
                                        // printf("active %d\r\n", json_object_get_int(cycleDetail));
                                    }
                                    if (json_object_object_get_ex(cycle1, "start_time", &cycleDetail))
                                    {
                                        // printf("start_time %s\r\n", json_object_get_string(cycleDetail));
                                        char t[64];
                                        // hour
                                        strncpy(t, json_object_get_string(cycleDetail) + 11, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
                                        t[2] = '\0';                                             /*Thêm ký tự kết thúc chuỗi vào kết quả*/
                                        // printf( "%s\n", t );
                                        if ((atoi(t) + 7) > 24)
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = atoi(t) + 7 - 24;
                                        else
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = atoi(t) + 7;
                                        // type_date_time.hh = atoi(t)+7;
                                        // printf( "%d\n", cycle.time.index[i].select_point[j].t_begin_apply.hour);
                                        // minute
                                        strncpy(t, json_object_get_string(cycleDetail) + 14, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
                                        t[2] = '\0';                                             /*Thêm ký tự kết thúc chuỗi vào kết quả*/
                                        // printf( "%s\n", t );
                                        cycle.time.index[i].select_point[j].t_begin_apply.minute = atoi(t);
                                        // type_date_time.mm = atoi(t);
                                        // printf( "%d\n", cycle.time.index[i].select_point[j].t_begin_apply.minute);
                                        if ((cycle.time.index[i].select_point[j].t_begin_apply.hour == 19) && (cycle.time.index[i].select_point[j].t_begin_apply.minute == 15))
                                        {
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = 255;
                                            cycle.time.index[i].select_point[j].t_begin_apply.minute = 255;
                                        }
                                    }
                                    if (json_object_object_get_ex(cycle1, "strategy_id", &cycleDetail))
                                    {
                                        cycle.time.index[i].select_point[j].index_cycle_form = json_object_get_int(cycleDetail) - 1;
                                        if (cycle.time.index[i].select_point[j].index_cycle_form == 20)
                                            cycle.time.index[i].select_point[j].index_cycle_form = 255;
                                        // printf("strategy_id %d\r\n", cycle.time.index[i].select_point[j].index_cycle_form);
                                    }
                                }
                            }
                        }
                        if (atoi(q) == 2)
                        {
                            // printf("Biểu mẫu 2\r\n");
                            if (json_object_object_get_ex(timeForm, "details", &timeFormDetail))
                            {
                                // printf("Detail 2\r\n");
                                for (int j = 0; j < json_object_array_length(timeFormDetail); j++)
                                {
                                    cycle1 = json_object_array_get_idx(timeFormDetail, j);
                                    if (json_object_object_get_ex(cycle1, "timeline", &cycleDetail))
                                    {
                                        // printf("timeline %d\r\n", json_object_get_int(cycleDetail));
                                    }
                                    if (json_object_object_get_ex(cycle1, "detail_id", &cycleDetail))
                                    {
                                        // printf("active %d\r\n", json_object_get_int(cycleDetail));
                                    }
                                    if (json_object_object_get_ex(cycle1, "start_time", &cycleDetail))
                                    {
                                        // printf("start_time %s\r\n", json_object_get_string(cycleDetail));
                                        char t[64];
                                        // hour
                                        strncpy(t, json_object_get_string(cycleDetail) + 11, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
                                        t[2] = '\0';                                             /*Thêm ký tự kết thúc chuỗi vào kết quả*/
                                        // printf( "%s\n", t );
                                        if ((atoi(t) + 7) > 24)
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = atoi(t) + 7 - 24;
                                        else
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = atoi(t) + 7;
                                        // mtfc_schedule.time.index[i].select_point[j].t_begin_apply.hour = atoi(t)+7;
                                        // type_date_time.hh = atoi(t)+7;
                                        // printf( "%d\n", cycle.time.index[i].select_point[j].t_begin_apply.hour);
                                        // minute
                                        strncpy(t, json_object_get_string(cycleDetail) + 14, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
                                        t[2] = '\0';                                             /*Thêm ký tự kết thúc chuỗi vào kết quả*/
                                        // printf( "%s\n", t );
                                        cycle.time.index[i].select_point[j].t_begin_apply.minute = atoi(t);
                                        // type_date_time.mm = atoi(t);
                                        // printf( "%d\n", cycle.time.index[i].select_point[j].t_begin_apply.minute);
                                        if ((cycle.time.index[i].select_point[j].t_begin_apply.hour == 19) && (cycle.time.index[i].select_point[j].t_begin_apply.minute == 15))
                                        {
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = 255;
                                            cycle.time.index[i].select_point[j].t_begin_apply.minute = 255;
                                        }
                                    }
                                    if (json_object_object_get_ex(cycle1, "strategy_id", &cycleDetail))
                                    {
                                        cycle.time.index[i].select_point[j].index_cycle_form = json_object_get_int(cycleDetail) - 1;
                                        if (cycle.time.index[i].select_point[j].index_cycle_form == 20)
                                            cycle.time.index[i].select_point[j].index_cycle_form = 255;
                                    }
                                }
                            }
                        }
                        if (atoi(q) == 3)
                        {
                            // printf("Biểu mẫu 3\r\n");
                            if (json_object_object_get_ex(timeForm, "details", &timeFormDetail))
                            {
                                // printf("Detail 3\r\n");
                                for (int j = 0; j < json_object_array_length(timeFormDetail); j++)
                                {
                                    cycle1 = json_object_array_get_idx(timeFormDetail, j);
                                    if (json_object_object_get_ex(cycle1, "timeline", &cycleDetail))
                                    {
                                        // printf("timeline %d\r\n", json_object_get_int(cycleDetail));
                                    }
                                    if (json_object_object_get_ex(cycle1, "detail_id", &cycleDetail))
                                    {
                                        // printf("active %d\r\n", json_object_get_int(cycleDetail));
                                    }
                                    if (json_object_object_get_ex(cycle1, "start_time", &cycleDetail))
                                    {
                                        // printf("start_time %s\r\n", json_object_get_string(cycleDetail));
                                        char t[64];
                                        // hour
                                        strncpy(t, json_object_get_string(cycleDetail) + 11, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
                                        t[2] = '\0';                                             /*Thêm ký tự kết thúc chuỗi vào kết quả*/
                                        // printf( "%s\n", t );
                                        if ((atoi(t) + 7) > 24)
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = atoi(t) + 7 - 24;
                                        else
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = atoi(t) + 7;
                                        // mtfc_schedule.time.index[i].select_point[j].t_begin_apply.hour = atoi(t)+7;
                                        // type_date_time.hh = atoi(t)+7;
                                        // printf( "%d\n", cycle.time.index[i].select_point[j].t_begin_apply.hour);
                                        // minute
                                        strncpy(t, json_object_get_string(cycleDetail) + 14, 2); /*Cắt 5 ký tự từ vị trí thứ 3 */
                                        t[2] = '\0';                                             /*Thêm ký tự kết thúc chuỗi vào kết quả*/
                                        // printf( "%s\n", t );
                                        cycle.time.index[i].select_point[j].t_begin_apply.minute = atoi(t);
                                        // type_date_time.mm = atoi(t);
                                        // printf( "%d\n", cycle.time.index[i].select_point[j].t_begin_apply.minute);
                                        if ((cycle.time.index[i].select_point[j].t_begin_apply.hour == 19) && (cycle.time.index[i].select_point[j].t_begin_apply.minute == 15))
                                        {
                                            cycle.time.index[i].select_point[j].t_begin_apply.hour = 255;
                                            cycle.time.index[i].select_point[j].t_begin_apply.minute = 255;
                                        }
                                    }
                                    if (json_object_object_get_ex(cycle1, "strategy_id", &cycleDetail))
                                    {
                                        cycle.time.index[i].select_point[j].index_cycle_form = json_object_get_int(cycleDetail) - 1;
                                        if (cycle.time.index[i].select_point[j].index_cycle_form == 20)
                                            cycle.time.index[i].select_point[j].index_cycle_form = 255;
                                    }
                                }
                            }
                        }
                    }
                }
                send_struct(CMD_CPU_TO_MASTER_SCHEDULE_TIME_UPDATE, (uint8_t *)&cycle.time, sizeof(type_time_lamp_form_t));
                printf("Đã send update time form to master Ok\n");
            }
            // flag_send_data_to_qt.is_time_form = 1;
        }
        if (json_object_object_get_ex(new_obj, "strategy", &o))
        {
            for (int i = 0; i < json_object_array_length(o); i++)
            {
                timeForm = json_object_array_get_idx(o, i);
                if (!json_object_object_get_ex(timeForm, "name", &timeFormDetail))
                {
                    return;
                }
                // type_cardConfig.index[i].phase = json_object_get_int(timeFormDetail);
                // printf("name data: %d\r\n",json_object_get_int(timeFormDetail));
                if (json_object_object_get_ex(timeForm, "local_id", &timeFormDetail))
                {
                    cycle_form = json_object_get_int(timeFormDetail);
                    printf("Cycle_form : %d\r\n", cycle_form);
                    if (cycle_form == 1)
                    {
                        // type_cardConfig.index[i].phase = json_object_get_int(timeFormDetail);
                        if (json_object_object_get_ex(timeForm, "routies", &timeFormDetail))
                        {
                            for (int j = 0; j < 8; j++)
                            {
                                route = json_object_array_get_idx(timeFormDetail, j);
                                cycle.cycle_1[j] = atoi(json_object_to_json_string(route));
                                // printf("routies[%d] : %d\r\n",j,  mtfc_schedule.cycle.index[0].side[j].t_green);
                            }
                        }
                        if (!json_object_object_get_ex(timeForm, "yellow", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_1[8] = json_object_get_int(timeFormDetail);
                        // printf("yellow: %d\r\n",mtfc_schedule.cycle.index[0].side[7].t_yellow);
                        if (!json_object_object_get_ex(timeForm, "free", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_1[9] = json_object_get_int(timeFormDetail);
                        // printf("free: %d\r\n",mtfc_schedule.cycle.index[0].clearance_time_crossroads);
                    }
                    if (cycle_form == 2)
                    {
                        // type_cardConfig.index[i].phase = json_object_get_int(timeFormDetail);
                        if (json_object_object_get_ex(timeForm, "routies", &timeFormDetail))
                        {
                            for (int j = 0; j < 8; j++)
                            {
                                route = json_object_array_get_idx(timeFormDetail, j);
                                cycle.cycle_2[j] = atoi(json_object_to_json_string(route));
                                // printf("routies[%d] : %d\r\n",j,  mtfc_schedule.cycle.index[0].side[j].t_green);
                            }
                        }
                        if (!json_object_object_get_ex(timeForm, "yellow", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_2[8] = json_object_get_int(timeFormDetail);
                        // printf("yellow: %d\r\n",mtfc_schedule.cycle.index[0].side[7].t_yellow);
                        if (!json_object_object_get_ex(timeForm, "free", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_2[9] = json_object_get_int(timeFormDetail);
                        // printf("free: %d\r\n",mtfc_schedule.cycle.index[0].clearance_time_crossroads);
                    }
                    if (cycle_form == 3)
                    {
                        // type_cardConfig.index[i].phase = json_object_get_int(timeFormDetail);
                        if (json_object_object_get_ex(timeForm, "routies", &timeFormDetail))
                        {
                            for (int j = 0; j < 8; j++)
                            {
                                route = json_object_array_get_idx(timeFormDetail, j);
                                cycle.cycle_3[j] = atoi(json_object_to_json_string(route));
                                // printf("routies[%d] : %d\r\n",j,  mtfc_schedule.cycle.index[0].side[j].t_green);
                            }
                        }
                        if (!json_object_object_get_ex(timeForm, "yellow", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_3[8] = json_object_get_int(timeFormDetail);
                        // printf("yellow: %d\r\n",mtfc_schedule.cycle.index[0].side[7].t_yellow);
                        if (!json_object_object_get_ex(timeForm, "free", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_3[9] = json_object_get_int(timeFormDetail);
                        // printf("free: %d\r\n",mtfc_schedule.cycle.index[0].clearance_time_crossroads);
                    }
                    if (cycle_form == 4)
                    {
                        // type_cardConfig.index[i].phase = json_object_get_int(timeFormDetail);
                        if (json_object_object_get_ex(timeForm, "routies", &timeFormDetail))
                        {
                            for (int j = 0; j < 8; j++)
                            {
                                route = json_object_array_get_idx(timeFormDetail, j);
                                cycle.cycle_4[j] = atoi(json_object_to_json_string(route));
                                // printf("routies[%d] : %d\r\n",j,  mtfc_schedule.cycle.index[0].side[j].t_green);
                            }
                        }
                        if (!json_object_object_get_ex(timeForm, "yellow", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_4[8] = json_object_get_int(timeFormDetail);
                        // printf("yellow: %d\r\n",mtfc_schedule.cycle.index[0].side[7].t_yellow);
                        if (!json_object_object_get_ex(timeForm, "free", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_4[9] = json_object_get_int(timeFormDetail);
                        // printf("free: %d\r\n",mtfc_schedule.cycle.index[0].clearance_time_crossroads);
                    }
                    if (cycle_form == 5)
                    {
                        // type_cardConfig.index[i].phase = json_object_get_int(timeFormDetail);
                        if (json_object_object_get_ex(timeForm, "routies", &timeFormDetail))
                        {
                            for (int j = 0; j < 8; j++)
                            {
                                route = json_object_array_get_idx(timeFormDetail, j);
                                cycle.cycle_5[j] = atoi(json_object_to_json_string(route));
                                // printf("routies[%d] : %d\r\n",j,  mtfc_schedule.cycle.index[0].side[j].t_green);
                            }
                        }
                        if (!json_object_object_get_ex(timeForm, "yellow", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_5[8] = json_object_get_int(timeFormDetail);
                        if (!json_object_object_get_ex(timeForm, "free", &timeFormDetail))
                        {
                            return;
                        }
                        cycle.cycle_5[9] = json_object_get_int(timeFormDetail);
                    }
                }
            }
            send_struct(44, (uint8_t *)&cycle, sizeof(cycle_t));
        }

        if (json_object_object_get_ex(new_obj, "actionSetting", &o))
        {
            if (json_object_object_get_ex(o, "monday", &template_server))
            {
                mtfc_schedule.days.index[1] = json_object_get_int(template_server);
                cycle.day[1] = mtfc_schedule.days.index[1];
            }
            if (json_object_object_get_ex(o, "tuesday", &template_server))
            {
                mtfc_schedule.days.index[2] = json_object_get_int(template_server);
                cycle.day[2] = mtfc_schedule.days.index[2];
            }
            if (json_object_object_get_ex(o, "wednesday", &template_server))
            {
                mtfc_schedule.days.index[3] = json_object_get_int(template_server);
                cycle.day[3] = mtfc_schedule.days.index[3];
            }
            if (json_object_object_get_ex(o, "thursday", &template_server))
            {
                mtfc_schedule.days.index[4] = json_object_get_int(template_server);
                cycle.day[4] = mtfc_schedule.days.index[4];
            }
            if (json_object_object_get_ex(o, "friday", &template_server))
            {
                mtfc_schedule.days.index[5] = json_object_get_int(template_server);
                cycle.day[5] = mtfc_schedule.days.index[5];
            }
            if (json_object_object_get_ex(o, "saturday", &template_server))
            {
                mtfc_schedule.days.index[6] = json_object_get_int(template_server);
                cycle.day[6] = mtfc_schedule.days.index[6];
            }
            if (json_object_object_get_ex(o, "sunday", &template_server))
            {
                mtfc_schedule.days.index[0] = json_object_get_int(template_server);
                cycle.day[0] = mtfc_schedule.days.index[0];
            }
            send_struct(CMD_CPU_TO_MASTER_SCHEDULE_DAYS_UPDATE, (uint8_t *)&mtfc_schedule.days, sizeof(type_days_lamp_form_t));
            // printf("T2: %d\n", mtfc_schedule.days.index[1]);
            // printf("T3: %d\n", mtfc_schedule.days.index[2]);
            // printf("T4: %d\n", mtfc_schedule.days.index[3]);
            // printf("T5: %d\n", mtfc_schedule.days.index[4]);
            // printf("T6: %d\n", mtfc_schedule.days.index[5]);
            // printf("T7: %d\n", mtfc_schedule.days.index[6]);
            // printf("CN: %d\n", mtfc_schedule.days.index[0]);
            // flag_send_data_to_qt.is_day_form = 1;
        }
        printf("Đã send update cycle form to master Ok\n");
        flag_control.strategy_setting = 1;
        send_byte(CMD_CPU_TO_MASTER_SCHEDULE, 1);
        // matching_data_to_old_app();
    }

    if (!strncmp(toppic, "card-setting", 12))
    {
        printf("Server yeu cau thay doi cau hinh card\r\n");
        struct json_object *new_obj, *cardSettingData, *cardSettingDataDetail, *nextObject, *dependentsObject, *dependentsObjectDetail;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        printf("Da send card setting data to master OK\r\n");
        if (json_object_object_get_ex(new_obj, "routeData", &o))
        {
            printf("Co route data\r\n");

            if (!json_object_object_get_ex(o, "rail_on", &nextObject))
            {
                return;
            }
            time_setting.time_delay_on_railway = json_object_get_int(nextObject); // Lay du lieu cai nam
            if (!json_object_object_get_ex(o, "rail_off", &nextObject))
            {
                return;
            }
            time_setting.time_delay_off_railway = json_object_get_int(nextObject); // Lay du lieu cai nam
            if (!json_object_object_get_ex(o, "prioritize", &nextObject))
            {
                return;
            }
            time_setting.time_walking = json_object_get_int(nextObject); // Lay du lieu cai nam
            if (json_object_object_get_ex(o, "dependents", &nextObject))
            {
                for (int i = 0; i < json_object_array_length(nextObject); i++)
                {
                    printf("Co dependents data %d\r\n", i);
                    dependentsObject = json_object_array_get_idx(nextObject, i);
                    if (!json_object_object_get_ex(dependentsObject, "route_id", &dependentsObjectDetail))
                    {
                        return;
                    }
                    time_setting.phase = json_object_get_int(dependentsObjectDetail); // Lay du lieu cai nam
                    printf("Co route_id data %d\r\n", time_setting.phase);
                    if (!json_object_object_get_ex(dependentsObject, "value", &dependentsObjectDetail))
                    {
                        return;
                    }
                    time_setting.time_delay_dependent_phase = json_object_get_int(dependentsObjectDetail); // Lay du lieu cai nam
                    printf("Co value data %d\r\n", time_setting.time_delay_dependent_phase);
                    send_struct(CMD_CPU_TO_MASTER_TIME_SETTING, (uint8_t *)&time_setting, sizeof(time_setting_t));
                    delay(2000);
                }
            }
            printf("%s", "Đã send time setting to Master OK\n");
        }
        if (json_object_object_get_ex(new_obj, "cardSettingData", &o))
        {
            printf("Leng card setting data: %d\r\n", json_object_array_length(o));
            for (int i = 0; i < json_object_array_length(o); i++)
            {
                cardSettingData = json_object_array_get_idx(o, i);
                if (!json_object_object_get_ex(cardSettingData, "card_num", &cardSettingDataDetail))
                {
                    return;
                }
                type_mtfc_one_card_config.slot = json_object_get_int(cardSettingDataDetail);
                if (!json_object_object_get_ex(cardSettingData, "route_id", &cardSettingDataDetail))
                {
                    return;
                }
                type_cardConfig.index[i].phase = json_object_get_int(cardSettingDataDetail);
                printf("phase data: %d\r\n", type_cardConfig.index[i].phase);
                if (!json_object_object_get_ex(cardSettingData, "is_rail", &cardSettingDataDetail))
                {
                    return;
                }
                type_cardConfig.index[i].is_railway_enabled = json_object_get_int(cardSettingDataDetail);
                printf("is_railway_enabled data: %d\r\n", type_cardConfig.index[i].is_railway_enabled);
                if (!json_object_object_get_ex(cardSettingData, "is_prioritize", &cardSettingDataDetail))
                {
                    return;
                }
                type_cardConfig.index[i].is_walking_enabled = json_object_get_int(cardSettingDataDetail);
                printf("is_walking_enabled data: %d\r\n", type_cardConfig.index[i].is_walking_enabled);
                if (!json_object_object_get_ex(cardSettingData, "is_route", &cardSettingDataDetail))
                {
                    return;
                }
                type_cardConfig.index[i].is_dependent_phase = json_object_get_int(cardSettingDataDetail);
                printf("is_dependent_phase data: %d\r\n", type_cardConfig.index[i].is_dependent_phase);
                char card[1280] = {0};
                snprintf(card, sizeof(card), "%s%d%s", "/home/pi/data/card/datacard", i + 1, ".txt");
                printf("%s\r\n", card);
                Write_struct_card_toFile(card_file, card, type_cardConfig.index[i]);
                printf("%s%d\r\n", "Da save ", i + 1);
                type_mtfc_one_card_config.phase = type_cardConfig.index[i].phase;
                type_mtfc_one_card_config.is_railway = type_cardConfig.index[i].is_railway_enabled;
                type_mtfc_one_card_config.is_walking = type_cardConfig.index[i].is_walking_enabled;
                type_mtfc_one_card_config.is_dependent_phase = type_cardConfig.index[i].is_dependent_phase;
                send_struct(27, (uint8_t *)&type_mtfc_one_card_config, sizeof(type_mtfc_one_card_config_t));
                printf("%s%d\r\n", "Da send den master ", i + 1);
                delay(4000);
            }

            for (int i = 0; i < 8; i++)
            {
                char data_send4[1000] = {0};
                int data_send_len = sprintf(data_send4, TEMPLATE_CARD_INFOR, CMD_CPU_TO_QT_CARD_INFOR,
                                            i,
                                            type_cardConfig.index[i].sn,
                                            type_cardConfig.index[i].imei,
                                            type_cardConfig.index[i].phase,
                                            type_cardConfig.index[i].is_railway_enabled,
                                            type_cardConfig.index[i].is_walking_enabled,
                                            type_cardConfig.index[i].is_dependent_phase,
                                            type_cardConfig.index[i].time_delay_dependent_phase);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len, data_send4);
                printf("%s\r\n", data_send4);
            }
        }

        flag_control.pinout_setting = 1;
    }

    if (!strncmp(toppic, "request-data", 12))
    {
        printf("Server yeu cau lay data\r\n");
        // send_struct(CMD_CPU_TO_MASTER_COUNT_LAMP, (uint8_t *)&counters_package, sizeof(type_configPackage_t));
        // send_struct(CMD_CPU_TO_MASTER_COUNT_LAMP, (uint8_t *)&counters_package, sizeof(type_configPackage_t));
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        if (json_object_object_get_ex(new_obj, "type", &o))
        {
            if (strcmp(json_object_get_string(o), ("TIME-SETTING")) == 0)
            {
                printf("Server yeu cau lay data TIME-SETTING\n");
                flag_control.time_setting_infor = 1;
            }
            if (strcmp(json_object_get_string(o), ("ROUTE-SETTING")) == 0)
            {
                printf("Server yeu cau lay data ROUTE-SETTING\n");
                flag_control.route_setting_infor = 1;
            }
            if (strcmp(json_object_get_string(o), ("PINOUT-SETTING")) == 0)
            {
                printf("Server yeu cau lay data PINOUT-SETTING\n");
                flag_control.pinout_setting_infor = 1;
            }
            if (strcmp(json_object_get_string(o), ("STRATEGY-SETTING")) == 0)
            {
                printf("Server yeu cau lay data STRATEGY-SETTING\n");
                flag_control.sttrategy_setting_infor = 1;
            }
            if (strcmp(json_object_get_string(o), ("CARD-SETTING")) == 0)
            {
                printf("Server yeu cau lay data CARD-SETTING\n");
                flag_control.card_setting_infor = 1;
            }
            if (strcmp(json_object_get_string(o), ("CONTROLLER")) == 0)
            {
                printf("Server yeu cau lay data CONTROLLER\n");
                count = 60;
                flag_control.controller_infor = 1;
                // flag_control.phase_infor = 1;
                switchinfor = 1;
            }
        }
    }
    if (!strncmp(toppic, "station-route-first", 19))
    {
        printf("Server yeu cau dieu tuyen pha\r\n");
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        if (json_object_object_get_ex(new_obj, "route", &o))
        {
            phase_infor = json_object_get_int(o);
            switch (phase_infor)
            {
            case 0:
                printf("Back to Auto\n");
                send_byte(CMD_CPU_TO_MASTER_MANUAL, 0);
                // flag_control.phase_infor_app = 1;
                modeactive = 1;
                phase = 0;
                switchinfor = 1;

                break;
            case 1:
                printf("Pha 1 run\n");
                send_byte(CMD_CPU_TO_MASTER_MANUAL, 1);
                modeactive = 2;
                phase = 1;
                switchinfor = 1;
                char data_send_1[1000] = {0};
                int data_send_len_1 = sprintf(data_send_1, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len_1, data_send_1);
                // flag_control.phase_infor_app = 1;
                break;
            case 2:
                printf("Pha 2 run\n");
                send_byte(CMD_CPU_TO_MASTER_MANUAL, 2);
                modeactive = 2;
                phase = 2;
                switchinfor = 1;
                char data_send_2[1000] = {0};
                int data_send_len_2 = sprintf(data_send_2, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len_2, data_send_2);
                // flag_control.phase_infor_app = 1;
                break;
            case 3:
                printf("Pha 3 run\n");
                send_byte(CMD_CPU_TO_MASTER_MANUAL, 3);
                modeactive = 2;
                phase = 3;
                switchinfor = 1;
                char data_send_3[1000] = {0};
                int data_send_len_3 = sprintf(data_send_3, TEMPLATE_PHASE, CMD_CPU_TO_QT_SELECT_PHASE, modeactive, phase);
                cl->send_ex(cl, UWSC_OP_TEXT, 1, data_send_len_3, data_send_3);
                // flag_control.phase_infor_app = 1;
                break;
            default:
                break;
            }
        }
    }
    if (!strncmp(toppic, "station-power", 13))
    {
        printf("Server yeu cau power\r\n");
        struct json_object *new_obj;
        new_obj = json_tokener_parse(data);
        printf("Parsed is: %s\n", temp);
        printf("Result is %s\n", (new_obj == NULL) ? "NULL (error!)" : "not NULL");
        if (!new_obj)
        {
            printf("Error Null");
            return;
        }
        struct json_object *o = NULL;
        if (json_object_object_get_ex(new_obj, "isOn", &o))
        {
            yellow_infor = json_object_get_int(o);
            switch (yellow_infor)
            {
            case 1:
                printf("ON\n");
                send_byte(CMD_CPU_TO_MASTER_POWER, 0);
                modeactive = 1;
                phase = 0;
                switchinfor = 1;
                // flag_control.flash_yellow_app = 1;
                break;
            case 0:
                printf("OFF\n");
                send_byte(CMD_CPU_TO_MASTER_POWER, 1);
                modeactive = 0;
                phase = 0;
                switchinfor = 1;
                // flag_control.flash_yellow_app = 1;
                break;
            default:
                break;
            }
        }
    }

    free(topic_name);
}

#pragma region LUONG XU LY CHUOI
char *substr(char *s, int start, int end)
{
    int indext = 0;

    for (int i = start; i <= end; i++)
    {
        re_mac[indext] = s[i];
        indext++;
    }
    re_mac[indext] = '\0';

    return re_mac;
}

void copy_u8_array_to_u32_array(uint8_t *u8, uint32_t *u32, int size)
{
    for (int i = 0; i < size; i++)
    {
        u32[i] = u8[i];
    }
}

void copy_u32_array_to_u8_array(uint32_t *u32, uint8_t *u8, int size)
{
    for (int i = 0; i < size; i++)
    {
        u8[i] = u32[i];
    }
}
#pragma endregion
#pragma region LUONG XU LY STATE LED GPS
void *LEd_GPS_State(void *threadArgs)
{
    while (1)
    {
        delay(10);
        if (!is_get_finish_data_gps)
        {
            digitalWrite(LED_GPS, 0);
            delay(100);
            digitalWrite(LED_GPS, 1);
            delay(100);
        }
        else
        {
            digitalWrite(LED_GPS, 0);
            delay(1000);
            digitalWrite(LED_GPS, 1);
            delay(1000);
        }
    }
}
#pragma endregion