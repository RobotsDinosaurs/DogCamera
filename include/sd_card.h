#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <string>
#include "esp_camera.h" // camera
#include "SD_MMC.h" // SD card      
#include "ntp.h"
#include <vector>
#include <FS.h>

using namespace std;

typedef struct picture {
    unsigned char* buf;
    int len;
} PICTURE;

esp_err_t init_sd_card();
void close_sd_card();
String generate_file_name(String timestamp, int i);
String generate_file_name(String timestamp, int k, int i);
String generate_file_name(int i);
bool rename(String old_filename, String new_filename);
esp_err_t save_pic (camera_fb_t * fb, String path);
PICTURE * read_pic (const char * path);
void delete_pic (PICTURE *picture);
vector<string> get_file_names_for_current_dir(int noPicsToFetch, int batchNo, int batchCap);
void delete_file(const char* file_name);

#endif