#include "camera.h"

// ESP32Cam (AiThinker) PIN Map
static camera_config_t camera_config = {
    .pin_pwdn = 32,
    .pin_reset = -1,
    .pin_xclk = 0,
    .pin_sscb_sda = 26,
    .pin_sscb_scl = 27,

    .pin_d7 = 35,
    .pin_d6 = 34,
    .pin_d5 = 39,
    .pin_d4 = 36,
    .pin_d3 = 21,
    .pin_d2 = 19,
    .pin_d1 = 18,
    .pin_d0 = 5,
    .pin_vsync = 25,
    .pin_href = 23,
    .pin_pclk = 22,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG

    .frame_size = FRAMESIZE_UXGA, //FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA (Do not use sizes above QVGA when not JPEG)
    .jpeg_quality = 6, //0-63 lower number means higher quality
    .fb_count = 1      //if more than one, i2s runs in continuous mode. Use only with JPEG
};

camera_fb_t * fb = NULL;

esp_err_t init_camera()
{
    // Initialize the camera
    Serial.println("Initializing camera");
    delay(500);
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return err;
    }

    return ESP_OK;
}

camera_fb_t* take_picture()
{
    // adding delay for pictures taken ourside as they are overexposed
    delay(400);
 
    fb = esp_camera_fb_get();  

    if(!fb)     
    {
        Serial.println("Camera capture failed");
        return NULL;
    } else {
        //Serial.println("Camera capture SUCCESS");
        return fb;
    }  
}