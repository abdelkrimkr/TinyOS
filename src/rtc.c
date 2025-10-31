
#include "rtc.h"
#include "ports.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

// RTC registers
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS   0x04
#define RTC_DAY     0x07
#define RTC_MONTH   0x08
#define RTC_YEAR    0x09
#define RTC_CENTURY 0x32 // (May not be available on all systems)

static uint8_t read_register(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static int is_update_in_progress() {
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

void rtc_read_time(rtc_time_t* rtc_time) {
    // Wait for the update in progress flag to be clear
    while (is_update_in_progress());

    rtc_time->second = read_register(RTC_SECONDS);
    rtc_time->minute = read_register(RTC_MINUTES);
    rtc_time->hour = read_register(RTC_HOURS);
    rtc_time->day = read_register(RTC_DAY);
    rtc_time->month = read_register(RTC_MONTH);
    rtc_time->year = read_register(RTC_YEAR);

    uint8_t register_b = read_register(0x0B);

    // BCD to binary conversion if necessary
    if (!(register_b & 0x04)) {
        rtc_time->second = (rtc_time->second & 0x0F) + ((rtc_time->second / 16) * 10);
        rtc_time->minute = (rtc_time->minute & 0x0F) + ((rtc_time->minute / 16) * 10);
        rtc_time->hour = ((rtc_time->hour & 0x0F) + (((rtc_time->hour & 0x70) / 16) * 10)) | (rtc_time->hour & 0x80);
        rtc_time->day = (rtc_time->day & 0x0F) + ((rtc_time->day / 16) * 10);
        rtc_time->month = (rtc_time->month & 0x0F) + ((rtc_time->month / 16) * 10);
        rtc_time->year = (rtc_time->year & 0x0F) + ((rtc_time->year / 16) * 10);
    }

    //
    // TODO: Read century and calculate full year. For now, assume 21st century.
    rtc_time->year += 2000;
}
