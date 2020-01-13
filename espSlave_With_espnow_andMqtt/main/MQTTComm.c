/*
 *@File name   :MQTTComm.c
 *
 *@Created on : 21-Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : MQTT configuration and Initialization done here 
 *
 */

//---------------------------------------------------------------
// included header files:
//----------------------
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"


#include "esp_log.h"


#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "JSONParser.h"
#include "main.h"
#include "MQTTComm.h"
#include "espnow_comm.h"
#include "WiFiSetup.h"

//---------------------------------------------------------------
// global variables:
//-------------------------
typedef enum{
    MQTT_IDLE,
    MQTT_INIT,
    MQTT_READY,
    MQTT_RUN
}Mqtt_State_t;

Mqtt_State_t mqttState = MQTT_IDLE;
static const char *TAG = "MQTTComm";

esp_mqtt_client_handle_t mqttClient;
char MQTTConnectStatus = 0;


#define SENSOR_DATA_SIZE 2048 
uint8_t sensorDataBuf[SENSOR_DATA_SIZE];
int sensorDataLen;
volatile static int sensortick =0;
//------------------------------------------------
// Static functoins prototypes:
//----------------------------



//---------------------------------------------------------------
// class and function definitions:
//------------------------------

  
/**
* @brief : : Callback event handler of MQTT process.
* @param  : esp_mqtt_event_handle_t event - MQTT callback event type.
* @pram   :  event : current event
* @return : none
*/
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    mqttClient = event->client;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_BEFORE_CONNECT: //SN_a
        ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
        break;

        case MQTT_EVENT_CONNECTED:
             MQTTConnectStatus = 1;
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            ///Keept subscribtion for sensor configuration
            msg_id = esp_mqtt_client_subscribe(client, TOPIC_SUB_SENSOR_CONFIG, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
			mqttState = MQTT_RUN;
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
			
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

/**
* @brief : : mqttPublish -publish the message predefined topic
* @param  : dataptr - data to be published on this topic  - string
* @return : none
*/
void mqttPublish(char *dataptr)
{
    int msg_id;
    
    if( mqttClient != NULL && MQTTConnectStatus)
    {
        msg_id = esp_mqtt_client_publish(mqttClient, TOPIC_PUB_SENSOR_ACQTICK, (char*)dataptr, 0, 0, 0);
       // msg_id = esp_mqtt_client_publish(mqttClient, TOPIC_PUB_SENSOR_DATA, dataptr, 0, 0, 0);
        ESP_LOGI(TAG, "Topic: %s, sent msg_id=%d, msg = %s, \n\rlen = %d", TOPIC_PUB_SENSOR_ACQTICK, msg_id, dataptr, strlen(dataptr) );

        
    }
    else
    {
        ESP_LOGI(TAG, "MQTT Client is not ready");
    }
}


/**
* @brief : : Initialize MQTT with server ip , port number
* @param  :  none
* @return : none
*/
void InitMQTT(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .host = MQTT_SERVER_IP,
        .port = MQTT_SERVER_PORT,
    
        .event_handle = mqtt_event_handler,
    };

    ESP_LOGI(TAG, "HEAP AvailableFree memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}


 
/**
* @brief : : MQTT statemachine, it initialize WIFI and then runs MQTT publis
* @param  :  none
* @return : none
*/
void MQTT_StateMachine()
{
    switch(mqttState)
    {
        case MQTT_IDLE:
         /*   if( isESPNOW_NotReady() )
            {
                //espnow_deinit();
               ESP_LOGI(TAG, "ESPNOW init failed, *** MQTT initiated *** ");
               //mqttState = MQTT_INIT;
            }*/
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		mqttState = MQTT_INIT;
        break;

        case MQTT_INIT:
            ///Initi wifi module
		   //wifi_deinitialize();
           ESP_LOGI(TAG, "Init WiFI and MQTT ");
		   //wifi_init();
		   vTaskDelay(500 / portTICK_PERIOD_MS);
	       updateWiFiCredential_default(); ///connect defualt ssid, pass , 
		   mqttState = MQTT_READY;
        break;

        case MQTT_READY:
            ESP_LOGI(TAG, "MQTT Ready");
        break;

        case MQTT_RUN:
            ESP_LOGI(TAG, "MQTT Publish");
            //memset(sensorDataBuf, 0 , SENSOR_DATA_SIZE);
			prepare_sensorJSON(sensorDataBuf, &sensorDataLen, (int)SENSOR_DATA_SIZE); 
            mqttPublish((char*)sensorDataBuf);
        break;

        default:
        mqttState = MQTT_RUN;
        break;
    }
}
//-----------------------------------
//          File end
//-----------------------------------
