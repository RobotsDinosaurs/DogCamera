#ifndef FTP_H
#define FTP_H

#include <Arduino.h>
#include <WiFiClient.h>

#include <ESP32_FTPClient.h>
#include "camera.h"

bool init_ftp();
void close_ftp();
esp_err_t upload_image_to_ftp_server(const char * file_name, unsigned char* buf, int len);

#endif