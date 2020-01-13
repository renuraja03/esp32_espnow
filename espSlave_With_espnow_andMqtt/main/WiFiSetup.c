/**
 * @file   :WiFiSetup.c
 *
 * @created : 21-Dec-2019
 *
 * @author   : Rajarajan
 *
 * @description  : WiFi configuration and Initialization done here 
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
#include "esp_now.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "main.h"
#include "WiFiSetup.h"
#include "MQTTComm.h"


//---------------------------------------------------------------
// global variables:
//-------------------------
#define ESP_MAXIMUM_RETRY 5
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event 
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "WiFi";

static int s_retry_num = 0;

#define WIFI_MODE 0
#define ESPNOW_MODE 1
int wifimode = WIFI_MODE;

wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };
//---------------------------------------------------------------
// Static functoins prototypes:
//----------------------------
void printMAC();


//---------------------------------------------------------------
// class and function definitions:
//------------------------------
/**
* @brief  : Callback event handler of wiif init.
* @param  : esp_gap_ble_cb_event_t event - GAP BLE callback event type.
          :  event_id : current event
          : event_data : details of event
* @return : none
*/
static void event_handler(void* arg, esp_event_base_t event_base, 
                                int32_t event_id, void* event_data)
{
	if(wifimode != WIFI_MODE)
	{
		return;
	}
	        ESP_LOGI(TAG,"WiFi event.'.");

	
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } 
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(TAG, "Error Retry to connect to the AP");
        }
        ESP_LOGI(TAG,"Connect to the AP fail");
    } 
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {


        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IP Assigned :" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Init MQTT now" );

        ///Init MQTT
        InitMQTT();
    }
	
	
}

/**
* @brief : Initialize wifi module, with default SSID and PAss, WiFi station created here
           refer event handler
* @param  : None
* @return : none
*/
void wifi_init( )
{
    s_wifi_event_group = xEventGroupCreate();

   
    esp_netif_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "wifi_init module initiated");

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    wifimode = WIFI_MODE ;

    //printMAC();
}


void printMAC()
{
    wifi_ap_record_t pMAC;
    if( esp_wifi_sta_get_ap_info( &pMAC ) == ESP_OK )
    {
        ESP_LOGI(TAG, "MAC details \n\r bssid: %X %X %X %X %X %X", 
        pMAC.bssid[0],
        pMAC.bssid[1],
        pMAC.bssid[2],
        pMAC.bssid[3],
        pMAC.bssid[4],
        pMAC.bssid[5]
        );
    }
    else
    {
        ESP_LOGI(TAG, "Failed to get MAC details");
    }
}

/**
* @brief : Initialize wifi module with espnow Mode
* @param  : None
* @return : none
*/
void wifi_init_ESPNOW_mode()
{
    s_wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
	
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	
	wifimode = ESPNOW_MODE;
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
	
   // ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(ESPNOW_WIFI_MODE) );
     ESP_ERROR_CHECK( esp_wifi_start());
    ESP_LOGI(TAG, "WiFi ESPNOW Init done\n");
    //printMAC();

}

/**
* @brief : Get WiFi ssid, pass from external source and Initialize wifi settings
* @param  : wifissi : Wifi ssid in string 
*        : wifipass : Wifi password in string 
* @return : none
*/
/*
void updateWiFiCredential(uint8_t *wifissid, uint8_t *wifipass)
{
    memcpy(wifi_config.sta.ssid , wifissid, WIFI_SSID_MAX_SIZE); ///get user ssid
    memcpy(wifi_config.sta.password, wifipass, WIFI_PASS_MAX_SIZE); //get user pass

        ESP_ERROR_CHECK(esp_wifi_stop() );

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             wifissid, wifipass);  
}
*/

void updateWiFiCredential_default()
{
   // memcpy(wifi_config.sta.ssid , WIFI_SSID, WIFI_SSID_MAX_SIZE); ///get user ssid
   // memcpy(wifi_config.sta.password, WIFI_PASS, WIFI_PASS_MAX_SIZE); //get user pass

    wifimode = WIFI_MODE ;
   
	esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_stop() );

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

	
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
 
}

void wifi_deinitialize()
{
	ESP_LOGI(TAG, "wifi_init_sta stop sta");
    ESP_ERROR_CHECK(esp_wifi_stop() );
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
	ESP_LOGI(TAG, "wifi_init_sta deinit sta");
    ESP_ERROR_CHECK(esp_wifi_deinit() );

	vTaskDelay(500 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "WIFI deinit completed");
}

//-----------------------------------
//          File end
//-----------------------------------
