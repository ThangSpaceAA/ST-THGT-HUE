// #include "Parameter_CPU.h"
#include "connect_app_ws.h"

#pragma region BIEN TOAN CUC

int ping_interval_status_APP = 10;
int opt;

long long previousMillis = 0;
long long previousMillis_CPU_5s = 0;
long long previousMillis_CPU_1s = 0;
long long previousMillis_updateServer = 0;
long long interval = 10000;
long long interval_CPU_5s = 5000;
long long interval_CPU_1s = 1000;

char *device_id = NULL;


uint8_t tx_buffer[4096] = { 0 };
int counter_login_fail = 0;

uint32_t g_signal[MAX_SIDE1] = { 0 };
uint32_t g_mapping[MAX_SIDE1] = { 0 };
uint32_t g_countdown[MAX_SIDE1] = { 0 };
uint32_t g_output_sensor[MAX_SIDE1] = { 0 };

volatile bool isconnected_websocket_local = false; 
volatile int keepAlliveCloud = 0;
volatile int keepAlliveCPU = 0;

volatile bool is_begin_start_up = true;
volatile bool is_finish_start_up = true;
volatile bool is_get_details_startup = false;
volatile bool is_send_config = true;
volatile bool is_send_schedule = true;

volatile bool is_register_success = false;
volatile bool is_login_success = false;
volatile bool is_connected_socket =false;
volatile bool isSoftConnected = false;
volatile bool is_flag_register_qtApp = false;
volatile bool is_flag_opend_weboscket_online = false;
#pragma endregion

#pragma region LUONG WEBSOCKET SERVER
void onopen_ws(struct uwsc_client *cl)
{

}
void onclose_ws(struct uwsc_client *cl, int code, const char *reason)
{
    uwsc_log_err("onclose:%d: %s\r\n", code, reason);
    ev_break(cl->loop, EVBREAK_ALL);
}
#pragma endregion