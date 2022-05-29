#include "wi_fi.h"

// Set static IP address for the ESP32 camera
// TODO: update the local_IP/gateway/secondary DNS...
IPAddress local_IP(192,168,0,0);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(8,8,8,8); // google DNS
IPAddress secondaryDNS(209,197,2,2);

bool connect_to_wifi() 
{
  // Set static IP
  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
  }

  // Connect to WiFi
  WiFi.begin( WIFI_SSID, WIFI_PASS );

  int retries = 0;
  Serial.print("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED && retries < WIFI_RETRIES) {
      Serial.print(".");
      retries ++;
      delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
      Serial.printf("\nFailed to connect to Wi-Fi\n");
      return false;
  }
  else {
      Serial.printf("\nIP address: ");
      Serial.println(WiFi.localIP());
  }

  return true;
}

void disconnect_from_wifi()
{
    // Turn off Wi-Fi
    WiFi.disconnect();
    Serial.println("");
    Serial.println("WiFi disconnected.");
}