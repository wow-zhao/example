# ESP32-CAM Project
## 1. Introduction
### environment
   esp-idf 4.4 + Vscode(espressif IDF) 
### hardware
   board:   Ai thinker esp32-cam  
   camera:  ov2640
### The workflow of this project is as follows  
   a ) ESP32 reboot, open a wifi with softAP/station coexistence mode.  
   b ) User uses PC or phone connect to AP open by ESP32. In this project, the AP ssid is "mywifissid", password is "123456789", which can be changed in WIFI_StaAp.c.  
   c ) User uses browser to access 192.168.4.1/{command}, in which {command} can be "jpeg", "stream", "predict", "wifi".  
       *if you want to use command "predict", you should use command "wifi" to help ESP32 connect to a wifi first.*
    
 command  | query  | function | example
 -------- | ------ | -------- | ------- 
 wifi  | query1, query2 | help ESP32 connect to a wifi, whose ssid is query1, password is query2 | 192.168.4.1/wifi?query1=A&query2=123456
 jpeg  | none | get a capture.jpg | 192.168.4.1/jpeg
 stream  | none | get a video stream | 192.168.4.1/stream
 predict  | none | Upload current image(jpeg) to server url | 192.168.4.1/predict

## 2. How to use (Vscode with espressif IDF example)
   * download the Project first.
   * open a new project with sample_project template.
   * copy the main



