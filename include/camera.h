#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include "esp_camera.h" // camera

esp_err_t init_camera();
camera_fb_t* take_picture();

#endif