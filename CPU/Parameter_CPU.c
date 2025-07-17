#include "Parameter_CPU.h"

void Write_struct_time_active_toFile(FILE *file, char *link, time_active_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(time_active_t), 1, file);
    fclose(file);
}

time_active_t Read_struct_time_active_toFile(FILE *file, char *link, time_active_t data_Write){
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
    return data_Write;
}

void Write_struct_schedule_toFile(FILE *file, char *link, schedule_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(schedule_t), 1, file);
        printf("Xanh 1: %d\r\n", data_Write.xanh1);
    printf("Xanh 2: %d\r\n", data_Write.xanh2);
    printf("Xanh 3: %d\r\n", data_Write.xanh3);
    printf("Xanh 4: %d\r\n", data_Write.xanh4);
    printf("Vang  : %d\r\n", data_Write.vang);
    printf("Giai toa: %d\r\n", data_Write.giaitoa);
    fclose(file);
}

schedule_t Read_struct_schedule_toFile(FILE *file, char *link, schedule_t data_Write){
    file = fopen(link, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }
    while (fread(&data_Write, sizeof(schedule_t), 1, file));
    printf("Xanh 1: %d\r\n", data_Write.xanh1);
    printf("Xanh 2: %d\r\n", data_Write.xanh2);
    printf("Xanh 3: %d\r\n", data_Write.xanh3);
    printf("Xanh 4: %d\r\n", data_Write.xanh4);
    printf("Vang  : %d\r\n", data_Write.vang);
    printf("Giai toa: %d\r\n", data_Write.giaitoa);
    fclose(file);
    return data_Write;
}

void Write_struct_sopha_toFile(FILE *file, char *link, sopha_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(sopha_t), 1, file);
    fclose(file);
}

sopha_t Read_struct_sopha_toFile(FILE *file, char *link, sopha_t data_Write){
    file = fopen(link, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }
    while (fread(&data_Write, sizeof(sopha_t), 1, file));
    printf("sopha : %d\r\n", sopha.sopha);

    fclose(file);
    return data_Write;
}

void Write_struct_card_toFile(FILE *file, char *link, type_one_cardConfig_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(type_one_cardConfig_t), 1, file);
    fclose(file);
}

type_one_cardConfig_t Read_struct_card_toFile(FILE *file, char *link, type_one_cardConfig_t data_Write){
    file = fopen(link, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }
    while (fread(&data_Write, sizeof(type_one_cardConfig_t), 1, file));
    fclose(file);
    return data_Write;
}

void Write_struct_setting_toFile(FILE *file, char *link, setting_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(setting_t), 1, file);
    fclose(file);
}

setting_t Read_struct_setting_toFile(FILE *file, char *link, setting_t data_Write){
    file = fopen(link, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }
    while (fread(&data_Write, sizeof(setting_t), 1, file));
    printf("Server: %d\r\n", setting.server);
    fclose(file);
    return data_Write;
}

void Write_struct_cycle_toFile(FILE *file, char *link, cycle_t data_Write){
    file = fopen(link, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opened file\n");
        exit(1);
    }
    fwrite(&data_Write, sizeof(cycle_t), 1, file);
    fclose(file);
}

cycle_t Read_struct_cycle_toFile(FILE *file, char *link, cycle_t data_Write){
    file = fopen(link, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }
    while (fread(&data_Write, sizeof(cycle_t), 1, file));
    fclose(file);
    return data_Write;
}