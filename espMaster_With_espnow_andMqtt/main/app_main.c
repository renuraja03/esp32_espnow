/**
 * @file   :main.c
 *
 *@Created on : 21-Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : main routine initialize the app and RTOS task, start scheduler 
 *
 */

//---------------------------------------------------------------
// included header files:
//----------------------
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "string.h"
#include "stdlib.h"

#include "WiFiSetup.h"
#include "main.h"
#include "MQTTComm.h"
#include "espnow_comm.h"


#include "nvs_flash.h"
#include <esp_log.h>
#include <esp_err.h>
//---------------------------------------------------------------
// global defines:
//----------------
#define MAIN_TAG "Main: "

//---------------------------------------------------------------
// global type definitions:
//-------------------------


//---------------------------------------------------------------
// global variables:
//------------------
 
//---------------------------------------------------------------
// class and function definitions:
//------------------------------
 

/**
 * @brief  :  espnow task runs the espnow message processing statemachine
 * @param	  : None
 * @return 	  : None
 * @control	  :  
 */
void espnow_task(void *pvParameter)
{
    while(1) 
	{
		espnow_StateMachine();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

 #ifdef MQTTT_ENABLE
/**
 * @brief   : this task cchecks for espnow master connnectoin if failed then 
 * this task communicate to mqtt broker
 * @param	  : None
 * @return 	  : None
 */
void mqtt_Task(void *pvParameter)
{
    while(1) 
	{
		MQTT_StateMachine();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
#endif

/**
 * @brief   : Hardware init - initialize WiFi 
 * @param	  : None
 * @return 	  : None
 * @Control	  : Main routine starts RTOS scheduler
 */
void HWInit()
{
    ///Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(MAIN_TAG, "\n\r===================================================");
    ESP_LOGI(MAIN_TAG, "\n\rESPNOW : %s ,\n\rVersion: %s",ESPNOW_DEVICE, ESPNOW_FW_VER);
    ESP_LOGI(MAIN_TAG, "\n\r===================================================\n\r");


    ///Initi wifi module
	//wifi_init();
    ///ESP_LOGI(MAIN_TAG, "WiFi station with SSID Init done\n");

    espnow_init();
    ///note mqtt will be initialized once ip got assigned by wifi

}

/**
 * @brief   : Initialize RTOS scheduler , create BLE task and WiFi task
 * @param	  : None
 * @return 	  : None
 * @Control	  : Main routine starts RTOS scheduler
 */
void app_main(void)
{
	///Initialize ESPnow or WiFi / MQTT
    HWInit(); 
    xTaskCreate(&espnow_task, "espnow_task",2048, NULL, 4, NULL);
   // xTaskCreate(&mqtt_Task, "mqtt_Task", 5120, NULL, 12, NULL);	
}

//..
//--------------------------------------------------------------------------------------
//                         File End
//--------------------------------------------------------------------------------------

