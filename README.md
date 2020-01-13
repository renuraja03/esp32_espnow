# esp32IoT ESPNOW and MQTT

Description:
 This project has two branch of code. 
Espnow protocol is used to exchang message between master and slave.

Tools you need:
- Esp IDF Latest version
 Please refer https://docs.espressif.com/projects/esp-idf/en/latest/get-started/
- Two ESP32 board (i have used nodemcu ESP32)

 
#Espnow master: /espMaster_With_espnow_andMqtt
This directory contains esp32 master code. 
 - Master initialize the espnow protocol and waits for message from slave. 
 - Added mqtt code into master branch but time being it is disabled, if you need please go to master code->main.h and enable the MQTT macro .
 - WiFi settings:
   Please go to WiFiSetup.h file and change the following parameters 
   ///Default ssid and passs
   #define WIFI_SSID   "give-your-wifissid"
   #define WIFI_PASS   "give-your-wifipass"

  - MQTT settings:
    Please go to MQTTComm.h and change the following parameters,
      //------------------------------------------
      // MQTT config
      //------------------------------------------- 
      #define MQTT_SERVER_IP "x.x.x.x" ///mqtt broker ip address 
      #define MQTT_SERVER_PORT portnumber ///mqttt broker port number
  - Once you completed the changes, please keep master code under ESPIDF directory and start build using make command
    use idf.py build to build the code
  - Connect your master esp32 to pc and find out the com port number  then execute the below command to program it
    idf.py -p COMx flash
    Where x is com number in windows
=====================================================================================================================

#Espnow Slave: /espSlave_With_espnow_andMqtt
This directory contains esp32 slave code. 
 - Slave initialize the espnow protocol and starts sending the message to master. 
 - Added mqtt code with slave branch, whenever slave fails to send to espnow master then slave starts sending to MQTT broker.
   Code is structured in file by file so you can take MQTT code to communicate only with MQTT broker

 - WiFi settings:
   Please go to WiFiSetup.h file and change the following parameters 
   ///Default ssid and passs
   #define WIFI_SSID   "give-your-wifissid"
   #define WIFI_PASS   "give-your-wifipass"

  - MQTT settings:
    Please go to MQTTComm.h and change the following parameters,
      //------------------------------------------
      // MQTT config
      //------------------------------------------- 
      #define MQTT_SERVER_IP "x.x.x.x" ///mqtt broker ip address 
      #define MQTT_SERVER_PORT portnumber ///mqttt broker port number
  - Once you completed the changes, please keep slave code under ESPIDF directory and start build using make command
    use idf.py build to build the code
  - Connect your slave esp32 to pc and find out the com port number  then execute the below command to program it
    idf.py -p COMx flash
    Where x is com number in windows

Test result:
Slave to master message:
![SlaveToESPnowMaster](https://github.com/renuraja03/esp32_espnow/blob/master/Test_Result/slave%20to%20master%20espnow.PNG)

Slave sends to MQTT broker: i have used Rpi as MQTT broker
![SlaveToMQTT](https://github.com/renuraja03/esp32_espnow/blob/master/Test_Result/MasterTimeout_MQTTStarts.PNG)




