// Build: gcc -Wall -o GateWay GateWay.c ../main_proto.pb-c.c ../device_proto.pb-c.c ../user_proto.pb-c.c ../common_proto.pb-c.c -lpthread -lws -lwiringPi -luwsc -lev -lprotobuf-c -ljson-c
#include "GateWay.h"

#pragma region BIEN TOAN CUC
// Bien khai bao toan cuc gw online
// const char *url = "wss://api.lighting.sdkd.com.vn/websocket";
const char *url = "wss://dev.hue-traffic.sitech.com.vn/websocket";
// const char *url = "ws://192.168.1.22:8443/websocket";
struct ev_signal signal_watcher;
int ping_interval = 10; /* second */
struct uwsc_client *cl;
// struct uwsc_client *mtfc_client_sv;
int opt;

// Bien xu ly login
char *device_id = NULL;
/**
 * @brief Called when a client connects to the server.
 *
 * @param fd File Descriptor belonging to the client. The @p fd parameter
 * is used in order to send messages and retrieve informations
 * about the client.
 */
#pragma endregion

#pragma region LUONG WEBSOCKET SERVER LOCAL
void onopen(int fd)
{
	char *cli;
	cli = ws_getaddress(fd);
	// #ifndef DISABLE_VERBOSE
	// 	printf("Connection opened, client: %d | addr: %s\n", fd, cli);
	// #endif
	// 	free(cli);

	// Gui ma thiet bi, firmware version, va project ID cho GateWay
}

/**
 * @brief Called when a client disconnects to the server.
 *
 * @param fd File Descriptor belonging to the client. The @p fd parameter
 * is used in order to send messages and retrieve informations
 * about the client.
 */
void onclose(int fd)
{
	char *cli;
	cli = ws_getaddress(fd);
	if (listDevice[0].ID == fd)
	{
		is_flag_register_qtApp = false;
	}
#ifndef DISABLE_VERBOSE
	printf("Connection closed, client: %d | addr: %s\n", fd, cli);
#endif
	free(cli);
}

/**
 * @brief Called when a client connects to the server.
 *
 * @param fd File Descriptor belonging to the client. The
 * @p fd parameter is used in order to send messages and
 * retrieve informations about the client.
 *
 * @param msg Received message, this message can be a text
 * or binary message.
 *
 * @param size Message size (in bytes).
 *
 * @param type Message type.
 */
void onmessage(int fd, const unsigned char *msg, uint64_t size, int type)
{
	char *cli;
	cli = ws_getaddress(fd);
#ifndef DISABLE_VERBOSE
	// printf("I receive a message: %s (size: %" PRId64 ", type: %d), from: %s/%d\n",
	// 	   msg, size, type, cli, fd);
	//	I receive a message: CPU-{"MODE": 2 } (size: 16, type: 1), from: 127.0.0.1/4

#endif

	if (strcmp((char *)msg, "qtApp") == 0)
	{
		is_flag_register_qtApp = true;
		listDevice[0].nameClient = "qtApp";
		listDevice[0].ID = fd;
		// printf("Ten cua o thu nhat la: %s\r\n", listDevice[0].nameClient );
		// printf("Ten cua o thu nhat la: %d\r\n", listDevice[0].ID );
		// ws_sendframe(listDevice[1].ID, "qtApp", size, false, type);

		char data[1000];
		printf("mac id: %s\r\n", mac_final);
		printf("fw: %s\r\n", firmware_GW_version);
		printf("Project id %s\r\n", "Sitech Co.Ltd");
		int data_len = sprintf(data, TEMPLATE_HARDWARE_ID, CMD_SERVER_TO_QT_HARDWARE_ID, 11, mac_final, 5, firmware_GW_version, 13, "Sitech Co.Ltd");
		// printf("reach here\r\n");
		ws_sendframe(qt_packet, (char *)data, data_len, false, 1);
	}
	if (strcmp((char *)msg, "CPU") == 0)
	{
		listDevice[1].nameClient = "CPU";
		listDevice[1].ID = fd;
		// printf("Ten cua o thu hai la: %s\r\n", listDevice[1].nameClient );
		// printf("Ten cua o thu hai la: %d\r\n", listDevice[1].ID);
		// char data[100];
		// int data_len = sprintf(data, TEMPLATE_HARDWARE_ID, CMD_SERVER_TO_QT_HARDWARE_ID, mac_id_gateway, firmware_GW_version, "Sitech Co.Ltd");
		// ws_sendframe(qt_packet, (char *)data, data_len, false, 1);
	}
	if (!strncmp(msg, "CPU-", 4))
	{
		// Chuyen tiep goi tin len cho qt
		ws_sendframe(listDevice[0].ID, (char *)msg, size, false, type);
		// MSG: CPU-{"CMD": 4,                            "mode": 2}
		// printf("MSG: %s", msg);
		

	}
	if (!strncmp(msg, "qtApp-", 6))
	{
		// // Chuyen tiep goi tin xuong cho cpu
		// printf("Message of CPU\r\n");
		ws_sendframe(listDevice[1].ID, (char *)msg, size, false, type);
	}
	free(cli);
}
#pragma endregion

#pragma region LUONG MO SOCKET ONLINE

void uwsc_onopen(struct uwsc_client *cl)
{
	is_flag_opend_weboscket_online = true;
}

void uwsc_onmessage(struct uwsc_client *cl,
					void *type_mtfc_working_message, size_t len, bool binary)
{
	if (binary)
	{
		size_t i;
		uint8_t *p = type_mtfc_working_message;

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
		printf("Has login response with code = %d and message = %s\n", deviceLoginResponse->statuscode, deviceLoginResponse->statuscode->message);
		if (deviceLoginResponse->statuscode->code == 0 || deviceLoginResponse->statuscode->code == 217)
		{
			is_login_success = true;
			printf("Annouce CPU online login\r\n");
			memset((char*)online.online, '\0', 32);
			sprintf(online.online, "%s","clound_connect");
			printf("buffer request: %s\r\n", online.online);
			delay(500);
			memset((char*)online.online, '\0', 32);
		}
		else
		{
			is_login_success = false;
			printf("Annouce CPU offline login\r\n");
			memset((char*)online.online, '\0', 32);
			sprintf(online.online, "%s","clound_disconnect");
			printf("buffer request: %s\r\n", online.online);
			delay(500);
			memset((char*)online.online, '\0', 32);
		}
	}

	if	(mainMessage->devicemessage &&
		mainMessage->devicemessage->userdevicemessage &&
		mainMessage->devicemessage->userdevicemessage->requirestreamdevicerequest)
		{
			RequireStreamDeviceRequest *requireStreamDeviceRequest = mainMessage->devicemessage->userdevicemessage->requirestreamdevicerequest;
			printf("Has response connect cloud with code and message %d, device id: %s\r\n", requireStreamDeviceRequest->enable, requireStreamDeviceRequest->deviceid);
			if(requireStreamDeviceRequest->enable)
			{
				printf("App connected\r\n");
				isSoftConnected = true;
			}
			else{
				printf("App disconnected\r\n");
				isSoftConnected = false;
			}

			if(isSoftConnected)
			{
				printf("Annouce CPU app connected\r\n");
				memset((char*)app_connected.app_connected, '\0', 32);
				sprintf(app_connected.app_connected, "%s", "app_connect");
				printf("buffer request: %s\r\n", app_connected.app_connected);
			}
			else{
				printf("Annouce CPU app disconnected\r\n");
				memset((char*)app_disconnected.app_disconnected, '\0', 32);
				sprintf(app_disconnected.app_disconnected, "%s", "app_disconnect");
				printf("buffer request: %s\r\n", app_disconnected.app_disconnected);
			}
		}

	if (mainMessage->devicemessage &&
		mainMessage->devicemessage->deviceupdatemessage &&
		mainMessage->devicemessage->deviceupdatemessage->pingwebsocketdeviceresponse)
		{
			PingWebsocketDeviceResponse *pingWebsocketDeviceResponse = mainMessage->devicemessage->deviceupdatemessage->pingwebsocketdeviceresponse;
			keepAlliveCloud--;
			printf("Value kepp aliive response: %d\r\n", keepAlliveCloud);
		}
	// Goi tin dieu khien on off
	if (mainMessage->devicemessage &&
			 mainMessage->devicemessage->userdevicemessage &&
			 mainMessage->devicemessage->userdevicemessage->powercontroluserrequest)
	{
		PowerControlUserRequest *request = mainMessage->devicemessage->userdevicemessage->powercontroluserrequest;
		printf("Has a power control request %d\r\n", (int)request->ispoweron);
		char data[1000];
		int len = sprintf(data, "qtApp-{\"CMD\": 15, \"PWR\": %d}",  (int)request->ispoweron);
		// "qtApp-{\"CMD\": 6, \"PWR\": 1}"
		ws_sendframe(Cpu_packet, data, len, false, 1);
		printf("Gui CPU done\r\n");
	}
	// Release control
	if (mainMessage->devicemessage &&
		mainMessage->devicemessage->userdevicemessage &&
		mainMessage->devicemessage->userdevicemessage->releasecontroluserrequest)
	{
		printf("Has release control\r\n");
		char data[1000];
		int len = sprintf(data, "qtApp-{\"CMD\": 16}");
		// "qtApp-{\"CMD\": 6, \"PWR\": 1}"
		ws_sendframe(Cpu_packet, data, len, false, 1);
		printf("Gui CPU done\r\n");
	}

}
void uwsc_onerror(struct uwsc_client *cl, int err, const char *msg)
{
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
	uwsc_log_err("onerror:%d: %s\n", err, msg);
#endif
	is_flag_opend_weboscket_online = false;
	is_register_success = false;
	is_login_success = false;
	ev_break(cl->loop, EVBREAK_ALL);
}

void signal_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
	if (w->signum == SIGINT)
	{
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
		uwsc_log_info("Normal quit\n");
#endif
	}
	ev_break(loop, EVBREAK_ALL);
}

void uwsc_onclose(struct uwsc_client *cl, int code, const char *reason)
{

	// printf("Recreate socket to login\r\n");
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
	uwsc_log_err("onclose:%d: %s\n", code, reason);
#endif
	is_flag_opend_weboscket_online = false;
	is_register_success = false;
	is_login_success = false;
	ev_break(cl->loop, EVBREAK_ALL);
}

void *open_Websocket_Online(void *threadArgs)
{
	struct ev_loop *loop = EV_DEFAULT;
	while (1)
	{
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
		printf("thread for open websocket\r\n");
		uwsc_log_info("Libuwsc: %s\n", UWSC_VERSION_STRING);
#endif
		cl = uwsc_new(loop, url, ping_interval, NULL);
		if (!cl)
		{
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
			printf("Create new socket fail\r\n");
#endif
			continue;
		}
		else
		{
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
			printf("Create new socket success\r\n");
#endif
		}
#ifdef DEBUG_OPEN_WEBSOCKET_ONLINE
		uwsc_log_info("Start connect...\n");
#endif
		cl->onopen = uwsc_onopen;
		cl->onmessage = uwsc_onmessage;
		cl->onerror = uwsc_onerror;
		cl->onclose = uwsc_onclose;

		device_id = NULL;
		ev_signal_init(&signal_watcher, signal_cb, SIGINT);
		ev_signal_start(loop, &signal_watcher);
		ev_run(loop, 0);
		free(cl);
	}
	// free(device_id);
	free(mac_id_gateway);
	return 0;
}
#pragma endregion

#pragma region LUONG MAIN
/**
 * @brief Main routine.
 *
 * @note After invoking @ref ws_socket, this routine never returns,
 * unless if invoked from a different thread.
 */
int main(void)
{
	struct ws_events evs;
#pragma region LAY THONG TIN MACID THIET BI LUU FILE
	system("cat /sys/class/net/eth0/address > mac.txt");
	fp = fopen("mac.txt", "r");
	if (fp == NULL)
	{
		printf("Error openfile\r\n");
		perror("Error opening file");
		// return(-1);
	}
	if (fgets(mac, 18, fp) != NULL)
	{
		/* writing content to stdout */
	}
	/* Closing the file mac.txt */
	fclose(fp);
	printf("Mac CM4: %s\r\n", mac);
	fw_version_GW = fopen("/home/pi/Version.txt", "r");
	if (fw_version_GW == NULL)
	{
		printf("Error openfile\r\n");
		perror("Error opening file");
		// return(-1);
	}
	if (fgets(firmware_GW_version, 18, fp) != NULL)
	{
		/* writing content to stdout */
		// exit(1);
	}
	fclose(fp);
	printf("Firmware version is: %s", firmware_GW_version);

	// printf("MAC: %s\r\n",mac);
	substr(mac, 6, 16);

	int len = asprintf(&mac_id_gateway, "GW-%s", re_mac, sizeof(re_mac));
	printf("MAC ID: %s\r\n", mac_id_gateway);
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
#pragma endregion
//-----------------------------------------------------------------------Lay du lieu GPS luu trong file--------------------------------------------------------------------//
	fptr = fopen("/home/pi/GPS.txt", "r");
	if(fptr == NULL)
	{
		printf("Error!");
	}
	if(fgets(Coordinate_str, 50, fptr) != NULL)
	{
	}
	fclose(fptr);

	temp = strtok(Coordinate_str, ":");
	temp = strtok(NULL, ",");
	Coordinate_float.Lat = atof(temp);
	temp = strtok(NULL, ":");
	temp = strtok(NULL, "/");
	Coordinate_float.Long = atof(temp);

	printf("Lat: %f\r\n", Coordinate_float.Lat);
	printf("Long: %f\r\n", Coordinate_float.Long);
	
    // time_active = Read_struct_time_active_toFile(time_active_file, "/home/pi/time_active.txt", time_active);
    schedule = Read_struct_schedule_toFile(schedule_file, "/home/pi/schedule.txt", schedule);
    // sopha = Read_struct_sopha_toFile(sopha_file, "/home/pi/sopha.txt", sopha);

#pragma region TAO LUONG WEBSOCKET ONLINE
	// if (pthread_create(&thread_Open_Websocket_Online, NULL, open_Websocket_Online, NULL) != 0)
	// 	printf("thread_create() failed\n");
#pragma endregion

#pragma region TAO LUONG DANG KY THIET BI ONLINE
	// if (pthread_create(&thread_send_Websocket_Online, NULL, Register_Device_GateWay, NULL) != 0)
	// 	printf("thread_create() failed\n");
#pragma endregion

	evs.onopen = &onopen;
	evs.onclose = &onclose;      
	evs.onmessage = &onmessage;
	ws_socket(&evs, 8080, 0); /* Never returns. */
	/*
	 * If you want to execute code past ws_socket, invoke it like:
	 *   ws_socket(&evs, 8080, 1)
	 */
	return 0;
}
#pragma endregion

#pragma region LUONG XU LY GUI GOI TIN SOCKET ONLINE
void *Register_Device_GateWay(void *threadArgs)
{
	while (1)
	{
		delay(1);
		if(is_begin_start_up)
		{
			if(is_finish_start_up)
			{
				is_begin_start_up = false;
				is_finish_start_up = false;
				is_get_details_startup = true;
				while(!is_get_details_startup)
				{
					printf("Get detail\r\n");
					memset((char*)request_detail.buff_request, '\0', 32);
					sprintf(request_detail.buff_request, "%s", "details");
					printf("buffer request: %s\r\n", request_detail.buff_request);
					//==============================================> ham send_struct gui cho master
					delay(5000);
				}
				while (is_register_success == 0) // Dang ky thiet bi
				{
					if (!is_flag_opend_weboscket_online)
					{
						printf("Cho mo ket noi thanh cong vs server online\r\n");
						delay(2000);
						continue;
					}
					printf("ket noi thanh cong vs server online thanh cong\r\n");
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
					deviceRegisterRequest.numphase = 2;
					
					// Them phan GPS khi dang ky thiet bi
					deviceRegisterRequest.lat = (double)Coordinate_float.Lat;
					deviceRegisterRequest.log = (double)Coordinate_float.Long;

					deviceAuthMessage.deviceregisterrequest = &deviceRegisterRequest;
					deviceMessage.deviceauthmessage = &deviceAuthMessage;
					mainMessage.devicemessage = &deviceMessage;

					int size = main_message__pack(&mainMessage, tx_buffer);
					cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
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
						// exit(1);
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
					cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
					printf("Send Login to server\r\n");
					delay(2000);
				}

				// bien bat buzzer
				if(isSoftConnected) // Send state app connect
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
				memset((char *) request_config.buff_config, '\0', 32);
				sprintf(request_config.buff_config, "%s", "config");
				printf("buffer request: %s\r\n", request_config.buff_config);
				//=================================> ham send_struct cho master
				delay(1000);
				printf("Get schedule for CPU\r\n");
				memset((char *) request_schedule.buff_schedule, '\0', 32);
				sprintf(request_schedule.buff_schedule, "%s", "schedule");
				printf("buffer request: %s\r\n", request_schedule.buff_schedule);
				//=================================> ham send_struct cho master
			}
			continue;
		}

		while(!is_get_details_startup)
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

		if(is_register_success == 0)
		{
			MainMessage mainMessage = MAIN_MESSAGE__INIT;
			DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
			DeviceAuthMessage deviceAuthMessage = DEVICE_AUTH_MESSAGE__INIT;
			DeviceRegisterRequest deviceRegisterRequest = DEVICE_REGISTER_REQUEST__INIT;

			deviceRegisterRequest.hardwareid = mac_final;
			deviceRegisterRequest.firmwareversion = firmware_GW_version;
			deviceRegisterRequest.mantoken = "112233445566";
			deviceRegisterRequest.cpuimei = "12345943860";
			deviceRegisterRequest.numphase = 2;
			
			// Them phan GPS khi dang ky thiet bi
			deviceRegisterRequest.lat = (double)Coordinate_float.Lat;
			deviceRegisterRequest.log = (double)Coordinate_float.Long;

			deviceAuthMessage.deviceregisterrequest = &deviceRegisterRequest;
			deviceMessage.deviceauthmessage = &deviceAuthMessage;
			mainMessage.devicemessage = &deviceMessage;

			int size = main_message__pack(&mainMessage, tx_buffer);
			cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
			printf("Send Register device GateWay to server \r\n");
			delay(2000);
		}

		if((is_register_success == true) && (!is_login_success))
		{
			if(device_id == NULL)
			{
				printf("Device did not register\n");
				return;
			}
			printf("Device ID: %s\n", device_id);
			if(strlen(device_id) <= 0)
			{
				delay(1000);
				// exit(1);
			}
			if(counter_login_fail > 5)
			{
				counter_login_fail = 0;
				// exit(1);
			}

			MainMessage mainMessage = MAIN_MESSAGE__INIT;
			DeviceMessage deviceMessage = DEVICE_MESSAGE__INIT;
			DeviceAuthMessage deviceAuthMessage = DEVICE_AUTH_MESSAGE__INIT;
			DeviceLoginRequest deviceLoginRequest = DEVICE_LOGIN_REQUEST__INIT;
			;
			deviceLoginRequest.deviceid = device_id;
			deviceLoginRequest.cpuimage = "12345943860";

			deviceAuthMessage.deviceloginrequest = &deviceLoginRequest;
			deviceMessage.deviceauthmessage = &deviceAuthMessage;
			mainMessage.devicemessage = &deviceMessage;

			int size = main_message__pack(&mainMessage, tx_buffer);
			cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
			printf("Send Login to server\r\n");
			delay(2000);
			counter_login_fail++;
		}
		
		if(is_login_success)
		{
			if(device_id == NULL)
			{
				printf("Device did not register, please type: register\n");
				return;
			}
			if(is_send_config)
			{
				printf("Get config CPU\r\n");
				memset((char*)request_config.buff_config, '\0', 32);
				sprintf(request_config.buff_config, "%s", "config");
				printf("buffer request: %s\r\n", request_config.buff_config);
				//===================================> send_struct cho master
				is_send_config = false;
			}
			if(is_send_schedule)
			{
				memset((char*)request_schedule.buff_schedule, '\0', 32);
				sprintf(request_schedule.buff_schedule, "%s", "schedule");
				printf("buffer request: %s\r\n", request_schedule.buff_schedule);
				//===================================> send_struct cho master
				is_send_schedule = false;
			}
			unsigned long long currentMillis = millis();
			unsigned long long currentMillis_update = millis();
			unsigned long long currentMillis_CPU_5s = millis();
			unsigned long long currentMillis_CPU_1s = millis();

			if((currentMillis_CPU_5s - previousMillis_CPU_5s > interval_CPU_5s) && (!isSoftConnected))
			{
				previousMillis_CPU_5s = currentMillis_CPU_5s;
				keepAlliveCPU = keepAlliveCPU + 5;
			}
			if((currentMillis_CPU_1s - previousMillis_CPU_1s > interval_CPU_1s) && (!isSoftConnected))
			{
				previousMillis_CPU_1s = currentMillis_CPU_1s;
				keepAlliveCPU++;
			}

			if(keepAlliveCPU > 30)
			{
				printf("CPU disconnected\r\n");
				delay(1000);
			}
			if(currentMillis - previousMillis > interval)
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

				cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
				keepAlliveCloud++;
				if(keepAlliveCloud >= 4)
				{
					printf("Annouce CPU offline\r\n");
					memset(offline.offline, 0, 32);
					memcpy(offline.offline, "cloud_disconnect", 16);
					printf("buffer request: %s\r\n", offline.offline);
					//====================================> send_struct cho master
					is_login_success = false;
					delay(2000);
					// exit(1);
				}
			}
			if(currentMillis_update - previousMillis_updateServer > 2000)
			{
				printf("====checkkkkkk\r\n");
				// updateServer();
				// updateServerSchedule(&time_active);
				previousMillis_updateServer = currentMillis_update;
			}
		}
	
	}
}
#pragma endregion

#pragma region HAM UPDATE SCHEDULE LEN SERVER
void updateServerSchedule(time_active_t *time_active)
{
    // time_active = Read_struct_time_active_toFile(time_active_file, "/home/pi/time_active.txt", time_active);
    schedule = Read_struct_schedule_toFile(schedule_file, "/home/pi/schedule.txt", schedule);
    // sopha = Read_struct_sopha_toFile(sopha_file, "/home/pi/sopha.txt", sopha);

	if(device_id == NULL)
	{
		printf("Device did not register, please type: register\r\n");
		return;
	}

	if(!is_login_success)
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
		
	begin.hour = time_active->hh_start;
	begin.minute = time_active->mm_start;
	active_lamp.begin = &begin;

	end.hour = time_active->hh_end;
	end.minute = time_active->mm_end;
	active_lamp.end = &end;
	
	printf("\r\nhh_start is: %d", time_active->hh_start);
	// printf("\r\nmm_start is: %d\r\n", bb);

	// printf("\r\nhh_end is: %d", time_active1.hh_end);
	// printf("\r\nmm_end is: %d", time_active1.mm_end);

	deviceUpdateScheduleRequest.active_time = &active_lamp;
	deviceUpdateMessage.deviceupdateschedulerequest = &deviceUpdateScheduleRequest;
	deviceMessage.deviceupdatemessage = &deviceUpdateMessage;
	mainMessage.devicemessage = &deviceMessage;

	int size = main_message__pack(&mainMessage, tx_buffer);
	MainMessage *mainMessage_recieved = main_message__unpack(NULL, size, tx_buffer);
	cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
}
#pragma endregion

#pragma region HAM GUI GOI TIN 1S LEN SERVER
void updateServer()
{
	if(device_id == NULL)
	{
		printf("Device did not register, please type: register\r\n");
		return;
	}

	if(!is_login_success)
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

	copy_u8_array_to_u32_array(mtfc_card_data_out.mapping, g_mapping, MAX_SIDE);
	update.signal = g_signal;
	update.n_signal = MAX_SIDE;

	copy_u8_array_to_u32_array(mtfc_card_data_out.mapping, g_mapping, MAX_SIDE);
	update.mapping = g_mapping;
	update.n_mapping = MAX_SIDE;

	copy_u8_array_to_u32_array(mtfc_card_data_out.mapping, g_countdown, MAX_SIDE);
	update.countdown = g_countdown;
	update.n_countdown = MAX_SIDE;

	update.walking_signal = 1;
	update.railway_signal = 1;
	update.cpu_active_mode = cpu_active.mode_active;
	update.input_hardware = 1;
	update.input_software = 1;
	update.idx_time_form = 1;
	update.idx_timeline = 1;
	update.idx_cycle_form = 1;		

	stateOnchange.update1second = &update;
	deviceUpdateMessage.stateonchange = &stateOnchange;
	deviceMessage.deviceupdatemessage = &deviceUpdateMessage;
	mainMessage.devicemessage = &deviceMessage;

	int size = main_message__pack(&mainMessage, tx_buffer);
	cl->send_ex(cl, UWSC_OP_BINARY, 1, size, tx_buffer);
}

#pragma endregion

#pragma region HAM XU LY CHUOI
void copy_u8_array_to_u32_array(uint8_t *u8, uint32_t *u32, int size)
{
	for(int i = 0 ; i < size; i++)
	{
		u32[i] = u8[i];
	}
}

void copy_u32_array_to_u8_array(uint32_t *u32, uint8_t *u8, int size)
{
	for(int i = 0 ; i < size ; i++)
	{
		u8[i] = u32[i];
	}
}

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
#pragma endregion
