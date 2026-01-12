#ifndef RTC_H
#define RTC_H

#include <stdint.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year; // Changed to uint16_t to hold full year
} rtc_time_t;

void rtc_init();
void get_time(rtc_time_t *time);

#endif // RTC_H
