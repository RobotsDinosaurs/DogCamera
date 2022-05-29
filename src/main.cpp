#include <stdlib.h>
#include "camera.h"
#include "sd_card.h"
#include "wi_fi.h"
#include "ftp.h"
#include "ntp.h"

#define PICS_PER_MOTION 4
#define PICS_PER_READ 10
#define BATCH_CAP 7
#define MOTION_RETRIGGER_CAP 5
#define MESSAGE_FINISH_TAKING_PICS 111

gpio_num_t pirPin = GPIO_NUM_13; // signal pin of the PIR sensor
gpio_num_t flashlightPin = GPIO_NUM_4;
camera_fb_t* pic =  NULL;

// Extra task to run simultaneously on core 0
TaskHandle_t camera_capture_task;
QueueHandle_t queue;
int queueSize = 1;
SemaphoreHandle_t lock = NULL;

void camera_capture_code(void * parameter) {
  Serial.print("camera_capture_code() running on core ");
  Serial.println(xPortGetCoreID());
  int element = 0;

  xSemaphoreTake(lock, portMAX_DELAY);
  int k = PICS_PER_MOTION * MOTION_RETRIGGER_CAP;

  for(;;) {    
      // Keep on taking pictures while there is motion
      // PIR sensor is set to re-triggering (H) and sensitivity is set to minimum (3s)
      int j = 0;
      int pirPinValue = LOW;
      pirPinValue = digitalRead(pirPin);

      while ((pirPinValue == HIGH) && (j < MOTION_RETRIGGER_CAP)) 
      {
          Serial.printf("PIR sensor re-triggered for the %i. time\n", j);

          // Take all pics per motion and save to SD card
          for (int i = j * PICS_PER_MOTION; i < (j+1) * PICS_PER_MOTION; i++)
          {
              pic = take_picture();

              // Generate file name
              String path = generate_file_name(*((String*)parameter), k, i);
    
              // Save picture 
              if (save_pic(pic, path) == ESP_OK)
              {
                  Serial.printf("Saved file to path: %s\n", path.c_str());
              }
              else 
              {
                  Serial.printf("Failed to save file to path: %s\n", path.c_str());
              }
          }
          j++;
          k++;
          pirPinValue = digitalRead(pirPin);
      }

      if (queue == NULL){
        Serial.println("Error: queue IS NULL");
        break;
      }
      else 
      {
        // portMAX_DELAY - wait indefinitely; here wait only for 500 ticks
        // If no message received, keep checking for motion and taking pics
        xQueueReceive(queue, &element, 500);
      
        if (element == MESSAGE_FINISH_TAKING_PICS)
        {
          Serial.print(element);
          Serial.println(" - Message to finish taking pics RECEIVED");
          break;
        }
      }

      vTaskDelay(500);
  }

  Serial.println("Deleted task camera_capture_code");
  xSemaphoreGive(lock);
  vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200); 
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.print("setup() running on core ");
    Serial.println(xPortGetCoreID());

    // Using queue to signal to core 0 that core 1 is done processing and will be going to sleep
    queue = xQueueCreate( queueSize, sizeof( int ) );
    if (queue == NULL){
        Serial.println("Error creating the queue");
    }

    // Mutex for process synchronization
    lock = xSemaphoreCreateMutex();

    // Capture the pictures first
    if (init_camera() == ESP_OK && init_sd_card() == ESP_OK)
    {
        // Keep on taking pictures while there is motion
        // PIR sensor is set to re-triggering (H) and sensitivity is set to minimum (3s)
        int j = 0;
        int pirPinValue = LOW;

        do 
        {
            Serial.printf("PIR sensor triggered for the %i. time\n", j);

            // Take all pics per motion and save to SD card
            for (int i = j * PICS_PER_MOTION; i < (j+1) * PICS_PER_MOTION; i++)
            {
                pic = take_picture();
                //Serial.printf("Free heap after take picture: %d\n", ESP.getFreeHeap());

                // Generate file name
                String path = generate_file_name(i);
    
                // Save picture 
                if (save_pic(pic, path) == ESP_OK)
                {
                    Serial.printf("Saved file to path: %s\n", path.c_str());
                }
                else 
                {
                    Serial.printf("Failed to save file to path: %s\n", path.c_str());
                }
            }
            j++;
            pirPinValue = digitalRead(pirPin);
            //Serial.printf("pirPin: %i, j: %i\n", pirPinValue, j);
        }
        while ((pirPinValue == HIGH) && (j < MOTION_RETRIGGER_CAP));

        bool wifiWorks = connect_to_wifi();

        // Connect to NTP server to get a timestamp
        String timestmp;
        timestmp = get_time_stamp(wifiWorks);

         // Rename the captured pics to include the timestamp
        for (int i = 0; i < j * PICS_PER_MOTION; i++)
        {
            // Generate new file names
            String old_path = generate_file_name(i);
            String new_path = generate_file_name(timestmp, i);
    
            if (rename(old_path, new_path)) 
            {
                Serial.printf("File: %s renamed successfully to %s\n", old_path.c_str(), new_path.c_str());
            }
            else 
            {
                Serial.printf("FAILED to rename file: %s to %s\n", old_path.c_str(), new_path.c_str());
            }
        }

        // Keep checking for motion and capturing photos while uploading to FTP
        xTaskCreatePinnedToCore(
                    camera_capture_code, /* Function to implement the task */
                    "Task1", /* Name of the task */
                    10000,  /* Stack size in words */
                    (void*)&timestmp,  /* Task input parameter */
                    0,  /* Priority of the task */
                    &camera_capture_task,  /* Task handle. */
                    0); /* Core where the task should run */

        if (wifiWorks) 
        {
            // Connect to the FTP server to upload the captured pictures
            bool ftpWorks = init_ftp();

            if (ftpWorks) 
            {
                int batchNo = 0;

                vector<string> allFileNames = get_file_names_for_current_dir(PICS_PER_READ, batchNo, BATCH_CAP);

                while (allFileNames.size() > 0 && batchNo < BATCH_CAP) 
                {
                    ++batchNo;
                    // Upload all saved pics from SD card to FTP server
                    for (int i=0; i<allFileNames.size(); i++)
                    {
                        PICTURE * currentPic = read_pic(allFileNames[i].c_str());

                        if (currentPic != NULL && currentPic->buf != NULL)
                        {
                            if (ESP_OK == upload_image_to_ftp_server(allFileNames[i].c_str(), currentPic->buf, currentPic->len))
                            {
                                // Delete from SD card if FTP transfer successful
                                delete_file(allFileNames[i].c_str());
                            }
                                               
                            // Free PSRAM
                            delete_pic(currentPic);
                        }
                    }

                    allFileNames = get_file_names_for_current_dir(PICS_PER_READ, batchNo, BATCH_CAP);
                }
            }
            else 
            {
                Serial.println("FTP not available.");
            }
            close_ftp();

            disconnect_from_wifi();
        }
        
        // Kill camera_capture_task if it's running
        if(camera_capture_task != NULL) 
        {
            int i = MESSAGE_FINISH_TAKING_PICS;
            xQueueSend(queue, &i, portMAX_DELAY);
            Serial.println("Sent message to finish taking pics to camera_capture_task");
        }

        // wait for camera_capture_task to finish
        xSemaphoreTake(lock, portMAX_DELAY);

        SD_MMC.end();
    }
    else 
    {
        Serial.println("Init of camera/SD card failed.");
    }

    // turn off flashlight when sleeping
    pinMode(flashlightPin, INPUT_PULLDOWN);

    // set up PIR sensor pin to receive signal when motion is detected
    pinMode(pirPin, INPUT);

    esp_sleep_enable_ext0_wakeup(pirPin, RISING);
        
    Serial.println("Going to sleep now");
        
    //sleeping until external wakeup triggered (motion detector)
    esp_deep_sleep_start();
}

void loop() 
{
    // this will never be called
}