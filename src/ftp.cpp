#include "ftp.h"

// Important: Set this static IP on the FTP server
char ftp_server[] = "192.168.TODO.TODO";
char ftp_user[]   = "TODO";
char ftp_pass[]   = "TODO";

// You can change this folder name for different cameras
// The folder name needs to exist before using camera
char ftp_folder[] = "TODO/frontyard";

// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 2);

bool init_ftp() 
{
  // Start FTP
  ftp.OpenConnection();

  // Get directory content
  ftp.InitFile("Type A");
  ftp.ChangeWorkDir(ftp_folder);

   return ftp.isConnected();
}

void close_ftp() 
{
  ftp.CloseConnection();
}

esp_err_t upload_image_to_ftp_server(const char * file_name, unsigned char* buf, int len) 
{
  ftp.InitFile("Type I");

  Serial.println(file_name);

  ftp.NewFile(file_name);
  ftp.WriteData(buf, len);
  ftp.CloseFile();

   if (!ftp.isConnected()) {
    // FTP Error
    Serial.println("FTP error detected");
    return ESP_ERR_TIMEOUT;
  }
  
  return ESP_OK;
}