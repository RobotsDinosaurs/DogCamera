#ifndef NTP_H
#define NTP_H

#include <Arduino.h>
#include <ESPPerfectTime.h>

#define SNTP_RETRIES 5

void print_time(struct tm *tm, suseconds_t usec);
String get_time_stamp(bool wifiWorks);

#endif