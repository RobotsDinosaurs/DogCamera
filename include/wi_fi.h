#ifndef WI_FI_H
#define WI_FI_H

#include <WiFi.h>
#include <WiFiClient.h> 

#define WIFI_SSID "TODO"
#define WIFI_PASS "TODO"
#define WIFI_RETRIES 5

bool connect_to_wifi();
void disconnect_from_wifi();

#endif