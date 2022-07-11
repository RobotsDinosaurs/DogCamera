# DOG CAMERA

<img src="/assets/images/dog-camera.jpg" alt="ESP32 Camera with PIR sensor" height="200" title="Dog Camera">

 When the PIR sensor detects a motion, the ESP32CAM wakes up, takes a couple of photos and saves them to the microSD card.
 After that, if wifi is accessible, it will try and transfer the pics to a locally set up FTP server.
 If the transfer was successful, the picture is erased from the SD card. 
 There is batch upload of pictures, so even pictures that were not transferred b/c of network/FTP issues, will be transferred 
 to the FTP server over time.

 Using board "AI Thinker ESP32-CAM" which has PSRAM. 
 Pin definitions are only for this board. 
 
 Make sure to set your own Wifi/FTP credentials. Pick a static IP for your ESP32CAM as well as local FTP server.
 Select a specific FTP folder for each camera. Pick your own NTP timezone. 

 **Video Tutorial:**
[YouTube Video - Dog Camera with ESP32CAM and PIR sensor](https://youtu.be/Xqssep32oWI)

**IDE:**
Visual Studio Code + PlatformIO plugin

 
## Connections for uploading a sketch

<img src="/assets/images/computer-connections.jpg" alt="Computer connections" height="200" title="Computer connections">

1. GPIO 0 must be connected to GND to upload a sketch. (I00 - GND)
2. After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode.

Connections Tutorial:
[YouTube Video - How to connect ESP32CAM+PIR to the computer](https://youtu.be/_SIfywxHqL0)
 
 ESP32CAM - FTDI Programmer
 * U0T - RX
 * U0R - TX
 
 ESP32CAM - 220 uF capacitor
 * 5V - positive leg of the capacitor
 * GND - negative leg of the capacitor
 
 ESP32CAM - PIR
 * GPIO13 - OUT
 
 PIR - 220 uF capacitor
 * VCC - positive leg of the capacitor
 * GND - negative leg of the capacitor
 
 FTDI Programmer (or 5V Battery Source) - 220 uF capacitor
 * VCC - positive leg of the capacitor
 * GND - negative leg of the capacitor
 
 
## Connections for the final version for the 3D printed box
  
 I am using a 3cm x 7cm board. 
 * MicroUSB port is soldered to positions: B08 (VCC)-B04 (GND)
 * 4-pin connectors (for ESP32CAM) are at: C07-C10 and L07-L10
 * 3-pin connector (for PIR sensor) is at: S09-U09
 * 220 uF capacitor is at S08 (+) and U08 (-)

 <img src="/assets/images/soldered-connections.jpg" alt="Soldered connections" height="200" title="Soldered connections">
 <img src="/assets/images/soldered-connections-2.jpg" alt="Soldered connections - rear side" height="200" title="Soldered connections - rear side">

 ## 3D Printed box

 [Thingiverse link - ESP32CAM+PIR box](https://www.thingiverse.com/thing:5389750)

 <img src="/assets/images/3d-printed-box.jpg" alt="3D Printed Box" height="200" title="3D Printed Box">

 ## Ubuntu management scripts

 [organize.py](assets/scripts/organize.py) - creates folders from timestamp in the file name and organizes pictures based on day taken
 [suspend_until.sh](assets/scripts/suspend_until.sh) - shuts down and starts PC at a certain time (defined in crontab as well)