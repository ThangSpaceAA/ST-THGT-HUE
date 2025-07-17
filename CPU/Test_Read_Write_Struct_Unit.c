#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct __attribute__((packed))
{
    uint8_t hh_start;
    uint8_t mm_start;
    uint8_t hh_end;
    uint8_t mm_end;
} time_active_t;

time_active_t time_active;

typedef struct __attribute__((packed))
{
    char device_hardware[100];
    char fimware_version[100];
    char project_id[100];
} packet_connect_server_onine_t;
packet_connect_server_onine_t packet_connect_server_onine;

void Write_struct_time_active_toFile(FILE *file, char *link, time_active_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(data_Write), 1, file);
    fclose(file);
}

void Read_struct_time_active_toFile(FILE *file, char *link, time_active_t data_Write){
    file = fopen(link, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }
    while (fread(&data_Write, sizeof(time_active_t), 1, file));
    printf("Start: hh:mm -> %d:%d\r\n End: hh:mm -> %d:%d\r\n", data_Write.hh_start,
           data_Write.mm_start,
           data_Write.hh_end,
           data_Write.mm_end);
    fclose(file);
}

int main()
{
    time_active.hh_start = 18;
    time_active.mm_start = 30;
    time_active.hh_end = 5;
    time_active.mm_end = 30;

    // sprintf(packet_connect_server_onine.device_hardware, "GW-684C9D", 9);
    // sprintf(packet_connect_server_onine.fimware_version, "1.0.8", 5);
    // sprintf(packet_connect_server_onine.project_id, "Hello", 5);

    FILE *file_data_save;
    // file_data_save = fopen("/home/host/Sitech_CPU_Lighting/CPU/data_save.dat", "w");
    // if (file_data_save == NULL)
    // {
    //     fprintf(stderr, "\nError opened file\n");
    //     exit(1);
    // }
    // fwrite(&time_active, sizeof(time_active_t), 1, file_data_save);
    // // fwrite(&packet_connect_server_onine, sizeof(packet_connect_server_onine_t), 1, file_data_save);
    // fclose(file_data_save);

    Write_struct_time_active_toFile(file_data_save,"/home/pi/time_active.dat", time_active);

#pragma region DOC DU LIEU TU FILE
    Read_struct_time_active_toFile(file_data_save, "/home/host/Sitech_CPU_Lighting/CPU/data_save.dat", time_active);
#pragma endregion
}
