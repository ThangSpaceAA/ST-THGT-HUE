#ifndef TEST_H
#define TEST_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    uint8_t hh_start;
    uint8_t mm_start;
    uint8_t hh_end;
    uint8_t mm_end;
} time_active_test_t;
time_active_test_t time_active_test;

uint8_t data_tmp_qqq;
#endif
