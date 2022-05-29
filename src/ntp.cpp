#include "ntp.h"

void print_time(struct tm *tm, suseconds_t usec) 
{
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d.%06ld\n",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec,
                usec);
}

String get_time_stamp(bool wifiWorks) 
{
  struct tm *tm = NULL;
  int retries = 0;

   // Do not retry if there is no wifi available
  if (!wifiWorks) {
    retries = SNTP_RETRIES - 1;
  }

  //Getting wrong date when trying to connect to SNTP server for the first time, so adding retries
  while ((tm == NULL || tm->tm_year < 100) && retries < SNTP_RETRIES) 
  {
    // TODO: pick your timezone
    // For ESP32 users, you can refer to following link to get POSIX-style tz string:
    // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
    pftime::configTzTime("EST5EDT,M3.2.0,M11.1.0", "ca.ntp.pool.org", "time.nist.gov", "time.cloudflare.com");

    // You can get microseconds at the same time
    // by passing suseconds_t* as 2nd argument
    suseconds_t usec;
    tm = pftime::localtime(nullptr, &usec);

    // Print them to serial
    print_time(tm, usec);

    delay(1000);
    retries++;
  }

  char curr_time[20];
  strftime(curr_time, 20, "%Y%m%d%H%M%S", tm);

  Serial.print("Current time: ");
  Serial.println(curr_time);

  return curr_time;
}