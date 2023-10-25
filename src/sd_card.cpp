#include "sd_card.h"

esp_err_t init_sd_card() 
{
    // Start SD Card
    Serial.println("Starting SD Card");
    delay(500);
    if(!SD_MMC.begin())
    {
        Serial.println("SD Card Mount Failed");
        return ESP_ERR_NO_MEM;
    }
  
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE)
    {
        Serial.println("No SD Card attached");
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

void close_sd_card() 
{
    SD_MMC.end();  
}

String generate_file_name(String timestamp, int i)
{
  // Path where new picture will be saved in SD Card
  return "/" + timestamp + "_" + String(i).c_str() +".jpg";
}

String generate_file_name(String timestamp, int k, int i)
{
  // Path where new picture will be saved in SD Card
  return "/" + timestamp + "_" + String(k).c_str()+ "_" + String(i).c_str() +".jpg";
}

String generate_file_name(int i)
{
  // Path where new picture will be saved in SD Card
  return String("/") + String(i).c_str() +".jpg";
}

bool rename(String old_filename, String new_filename) {
    return SD_MMC.rename(old_filename, new_filename);
}

esp_err_t save_pic (camera_fb_t * fb, String path)
{
    //Serial.printf("Picture file name: %s\n", path.c_str());
    
    File file = SD_MMC.open(path.c_str(), FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file in writing mode");
        esp_camera_fb_return(fb);  
        return ESP_ERR_NO_MEM;
    } 
    else 
    {
        
        file.write(fb->buf, fb->len); // payload (image), payload length
        
        file.close();
        
        esp_camera_fb_return(fb); 
    }
        
    return ESP_OK;
}

PICTURE *new_pic (size_t sz) 
{
    // Try to allocate PICTURE structure.
    PICTURE *retVal = (PICTURE *)malloc (sizeof (PICTURE));
    if (retVal == NULL) {
      Serial.println("retVal is NULL");
      return NULL;
    }

    // Try to allocate PICTURE buffer, free structure if fail.
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
    Serial.printf("Trying to allocate %d size\n", sz);
    retVal->buf = (unsigned char *)heap_caps_malloc(sz, MALLOC_CAP_SPIRAM);
    
    if (retVal->buf == NULL) {
        Serial.println("retVal -> buf is NULL");
        free (retVal);
        return NULL;
    }

    // Set size and return.
    retVal->len = sz;
    return retVal;
}

void delete_pic (PICTURE *picture) {
    // Can safely assume picture is NULL or fully built.
    if (picture != NULL) {
        heap_caps_free (picture->buf);
        free (picture);
    }
}

PICTURE * read_pic (const char * path)
{
    //Serial.printf("Picture file name: %s\n", path);
  
    char *x;
    asprintf(&x, "/%s", path);
    File file2 = SD_MMC.open(x, FILE_READ);
 
    if (!file2) {
        Serial.println("Opening file to read failed");
        return NULL;
    }
    
    Serial.printf("File Size: %d\n", file2.size());
 
    Serial.println("Reading file content...");
    size_t buf_size = file2.size();
    PICTURE * result = new_pic(buf_size);
    if (result != NULL)
    {
      Serial.println("Allocated picture");
      size_t nbytes = file2.read(result->buf, buf_size);
      Serial.printf("Pic size: %d\n", nbytes);
    }
    else 
    {
      Serial.println("Failed to allocate memory for PICTURE");
    }
    free(x);
    file2.close();

    return result;
}

vector<string> get_file_names_for_current_dir(int noPicsToFetch, int batchNo, int batchCap)
{
    vector<string> result = {};

    if (batchNo < batchCap)
    {
        String rootDir = "/";
        int pics_read = 0;

        Serial.println("Starting to get " + String(noPicsToFetch) + " file names for current dir, batch: " + String(batchNo));

        if(!SD_MMC.exists(rootDir))
        {
            Serial.println( "Can't open directory " + rootDir);
        }
        else
        {
            File dir= SD_MMC.open(rootDir);
            File file = dir.openNextFile();                                                                                                                                                                                                                                                                                                                                                                                                                                                    
        
            while( file && (pics_read < noPicsToFetch))
            {
                String fn, fs;
                fn = file.name();

                int i = fn.lastIndexOf("/")+1;
                fn.remove(0, i);
                fs = String(file.size());
            
                if(file.isDirectory())
                {
                    //Serial.println( "Dir Name = " + fn);
                } else {
                    //Serial.println( "File Name:" + fn + ", Size: " + fs + " bytes");
                    result.push_back(fn.c_str());
                    ++pics_read;
                    file.close();
                }
        
                file = dir.openNextFile();
            }

            file.close();
            dir.close();
        }

        if (pics_read > 0) 
        {
            Serial.println();
            Serial.println("TOTAL: " + String(batchNo * noPicsToFetch + pics_read) + " files read.");
            Serial.println();
        }
    }

    return result;
}

void delete_file(const char* file_name) 
{
    char *x;
    asprintf(&x, "/%s", file_name);
    if (SD_MMC.remove(x)) {
        Serial.println("Deleted " + String(file_name));
    }
    else {
        Serial.println("Can't delete " + String(file_name));
    }
    free(x);
}