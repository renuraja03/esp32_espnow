/*
 *@File name   :espnow_comm.c
 *
 *@Created on : 21_Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : ESPnow init, send, receive
 *
 */

/*
   This example shows how to use ESPNOW.
   Prepare two device, one for sending ESPNOW data and another for receiving
   ESPNOW data.
*/

//---------------------------------------------------------------
// included header files:
//----------------------
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_crc.h"

#include "espnow_comm.h"
#include "JSONParser.h"
#include "MQTTComm.h"
#include "WiFiSetup.h"
#include "main.h"

//---------------------------------------------------------------
// global variables:
//-------------------------
static const char *TAG = "espnowSlave";

static xQueueHandle espnow_queue_msg;

static uint8_t broadcast_mac[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
esp_now_peer_info_t peer_config;
 
espnow_state_t espnowState = ESPNOW_IDLE;


volatile uint8_t sendData[SEND_DATA_MAX] ;
volatile int sendDataLen;

volatile uint8_t rcvData[RCV_DATA_MAX] ;
volatile int rcvDataLen;


#define DEST_MAC_COUNT 1
#define DEST_MAC_SLAVE1 0
uint8_t destMAC[DEST_MAC_COUNT][6] = 
{
	  { 0x3C, 0x71, 0xBF, 0x4C, 0x92, 0x60 } ///esp master MAC address
 // { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};

typedef enum{
	CMD_SEND_BOOT_MSG,
	CMD_SEND_SENSOR_MSG 
} cmdSendFuncIndex_t;

cmdSendFuncIndex_t cmdSendFuncIndex = CMD_SEND_BOOT_MSG;

 jsonFunc cmdSendFunc[] = {
	 prepare_bootJSON,
	 prepare_sensorJSON,
	 NULL
 };
 //---------------------------------------------------------------
// Static functoins prototypes:
//----------------------------
void espnow_deinit(void);



//---------------------------------------------------------------
// class and function definitions:
//------------------------------



/**
 * @brief : espnow tx callback
 * on succesfull sent of msg this functoinsi called , it helps to mark that transmit is done
 * If transmit completed it sends que msg to espnow task 
 * @param : none
 * 
 */
static void espnow_Tx_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    espnow_event_t evt;

    if (mac_addr == NULL) {
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }
        ESP_LOGE(TAG, "Send cb called");

    memcpy(evt.smac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    evt.status = status;
    if (xQueueSend(espnow_queue_msg, &evt, portMAX_DELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Tx Send queue fail");
    }
}
/**
 * @brief : espnow recive callback
 * Whenver this module got any msg this fuctiona is called
 * This function copies the received msg and sends msgque to espnow task
 * @param : none
 * 
 */
static void espnow_Rx_callback(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    espnow_event_t event_r;
 
    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }
        ESP_LOGE(TAG, "Recv cb called");

    memcpy(event_r.rmac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    event_r.dlen = len;
    if(len >= RCV_DATA_MAX )
    {
        event_r.dlen = RCV_DATA_MAX;
    }
	memset( event_r.data,0, RCV_DATA_MAX);
    memcpy(event_r.data, data, event_r.dlen); 
    
    if (xQueueSend(espnow_queue_msg, &event_r, portMAX_DELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Send receive queue fail");
     }
}
 


/**
 * @brief : espnow handler called by espnow task
 * This fuction runs a statmachine to process received message and gives response 
 * on valid msg foind it gives valid response
 * If invalid msg found it gives error response
 * 
 * Supported msg:
 *   bootmsg
 *   sensor msg   
 * @param : none
 * 
 */
void espnow_StateMachine(void)
{
    espnow_event_t evt;
    bool is_broadcast = false;
    int ret;

    switch(espnowState)
    {
        case ESPNOW_IDLE:
            ESP_LOGI(TAG, "state IDLE");
        break;

        case ESPNOW_SEND_BOOT:
        prepare_bootJSON(sendData, &sendDataLen, (int)SEND_DATA_MAX); 
        ESP_LOGI(TAG, "Send  frame to [%X %X %X %X %X %X]: \n\r%s \n\r", 
                    destMAC[DEST_MAC_SLAVE1][0],
                    destMAC[DEST_MAC_SLAVE1][1],
                    destMAC[DEST_MAC_SLAVE1][2],
                    destMAC[DEST_MAC_SLAVE1][3],
                    destMAC[DEST_MAC_SLAVE1][4],
                    destMAC[DEST_MAC_SLAVE1][5],
                    sendData);

        if (esp_now_send( destMAC[DEST_MAC_SLAVE1], (const uint8_t*) sendData, sendDataLen) != ESP_OK) {
                ESP_LOGE(TAG, "Send error");
				espnowState = ESPNOW_SEND_BOOT;
            }
			else{
			   espnowState = ESPNOW_SEND_BOOT_WAIT;
			}
        break;  

        case ESPNOW_SEND_BOOT_WAIT:
        ESP_LOGI(TAG, "state wait for send BOOT completed");
        if(xQueueReceive(espnow_queue_msg, &evt, portMAX_DELAY) == pdTRUE)
        {
          //  is_broadcast = IS_BROADCAST_ADDR(evt->smac_addr);
          //  ESP_LOGD(TAG, "Send data to "MACSTR", status1: %d", MAC2STR(evt->smac_addr), evt->status); 
          // memcpy(destMAC[DEST_MAC_SLAVE1], evt->smac_addr, ESP_NOW_ETH_ALEN);
            ESP_LOGD(TAG, "Send BOOT msg completed");
            espnowState = ESPNOW_BOOT_RECV;
        }
        break;

        case ESPNOW_BOOT_RECV:
        ESP_LOGI(TAG, "state wait for RECV ");
        //if(xQueueReceive(espnow_queue_msg, &evt, portMAX_DELAY) == pdTRUE) //
        if(xQueueReceive(espnow_queue_msg, &evt, MAX_WELCOME_MSG_TIMEOUT) == pdTRUE)
        {
           ESP_LOGI(TAG, "BOOT RecvedData Len: %d,\n\r%s \n\r",evt.dlen, evt.data);
		   if(parse_bootJSON(evt.data, evt.dlen, 0) )
		   {
			   ESP_LOGI(TAG, "BOOT valid msg found");
				espnowState = ESPNOW_SEND_SENSOR;		   
		   }
		   else{
			   espnowState = ESPNOW_SEND_BOOT;
		   }
        }
		else{
           ESP_LOGI(TAG, "ACK failed from Master");
		   espnowState = ESPNOW_FAIL;
		}
        break;  
		
		case ESPNOW_SEND_SENSOR:
		
        prepare_sensorJSON(sendData, &sendDataLen, (int)SEND_DATA_MAX); 
        ESP_LOGI(TAG, "Send sensor frame to , [%X %X %X %X %X %X]: \n\r%d, %s \n\r", 
                    destMAC[DEST_MAC_SLAVE1][0],
                    destMAC[DEST_MAC_SLAVE1][1],
                    destMAC[DEST_MAC_SLAVE1][2],
                    destMAC[DEST_MAC_SLAVE1][3],
                    destMAC[DEST_MAC_SLAVE1][4],
                    destMAC[DEST_MAC_SLAVE1][5],
					sendDataLen,
                    sendData);

        if (esp_now_send( destMAC[DEST_MAC_SLAVE1], (const uint8_t*) sendData, sendDataLen) != ESP_OK) {
                ESP_LOGE(TAG, "Send sensor error");
			   espnowState = ESPNOW_SEND_SENSOR;
            }
			else{
			   espnowState = ESPNOW_SEND_SENSOR_WAIT;
			}
        break;  
	   
		case ESPNOW_SEND_SENSOR_WAIT:
        ESP_LOGI(TAG, "state wait for send sensor completed");
        if(xQueueReceive(espnow_queue_msg, &evt, portMAX_DELAY) == pdTRUE)
        {
          //  is_broadcast = IS_BROADCAST_ADDR(evt->smac_addr);
          //  ESP_LOGD(TAG, "Send data to "MACSTR", status1: %d", MAC2STR(evt->smac_addr), evt->status); 
          // memcpy(destMAC[DEST_MAC_SLAVE1], evt->smac_addr, ESP_NOW_ETH_ALEN);
            ESP_LOGD(TAG, "Send sensor completed");
            espnowState = ESPNOW_SEND_SENSOR_RECV;
        }
        break;

        case ESPNOW_SEND_SENSOR_RECV:
        ESP_LOGI(TAG, "state wait for RECV sensor");
        if(xQueueReceive(espnow_queue_msg, &evt, portMAX_DELAY) == pdTRUE)
        {
           ESP_LOGI(TAG, "sensor RecvedData Len: %d,\n\r%s \n\r",evt.dlen, evt.data);
            espnowState = ESPNOW_SEND_SENSOR;		   
        }
        break;  
		
        case ESPNOW_FAIL:
           ESP_LOGI(TAG, "Suspending the Slave espnow task");
		   espnow_deinit();
		   xTaskNotifyGive( xTask_Mqtt ) ;
		   vTaskSuspend( NULL );
			
        break;  

        default:
            espnowState = ESPNOW_IDLE;
        break;

    }
}

/**
 * @brief : Add peer into espnow setup 
 * @param : Destination mac address
 * 
 */
int addPeer(uint8_t* tMac)
{
    memset(&peer_config, 0, sizeof(esp_now_peer_info_t));
    peer_config.channel = CONFIG_ESPNOW_CHANNEL;
    peer_config.ifidx = ESPNOW_WIFI_IF;
    peer_config.encrypt = false;
    memcpy(peer_config.peer_addr, tMac, ESP_NOW_ETH_ALEN);
    return esp_now_add_peer(&peer_config) ;
}

 /**
 * @brief : Init WiFi in espnow mode, station mode and then init espnow driver
 * @param : none
 * @return : error code if init failed, success on init pass
 * 
 */

esp_err_t espnow_init(void)
{
    wifi_init_ESPNOW_mode(); 

    espnow_queue_msg = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));

	  
    if (espnow_queue_msg == NULL) {
        ESP_LOGE(TAG, "Create mutex fail");
        espnowState = ESPNOW_FAIL ; 
        return ESP_FAIL;
    }

    /* Initialize ESPNOW and register sending and receiving callback function. */
    if( esp_now_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init ESPNOW "); 
        espnowState = ESPNOW_FAIL ; 
        return ESP_FAIL;
    }
    if( esp_now_register_send_cb(espnow_Tx_callback) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init Tx callback"); 
        espnowState = ESPNOW_FAIL ; 
        return ESP_FAIL;
    }

    if( esp_now_register_recv_cb(espnow_Rx_callback) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init Rx callback "); 
        espnowState = ESPNOW_FAIL ; 
        return ESP_FAIL;
    }

    /* Set primary master key. */
    if( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set PMK "); 
        espnowState = ESPNOW_FAIL ; 
        return ESP_FAIL;
    }

    ///add peer with dest MAC address
    if( addPeer(  destMAC[DEST_MAC_SLAVE1] ) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add peer "); 
        espnowState = ESPNOW_FAIL ; 
        return ESP_FAIL;
    }
    ///Addd parameter with dest MAC address
    //addParam(send_param);
    ///Prepare parameter 
    //espnow_data_prepare(send_param,  broadcast_mac);
    ///Create task to handle
    espnowState = ESPNOW_SEND_BOOT ;

    return ESP_OK;
}
/**
 * @brief : deinitialize espnow driver
 * @param : none
 * 
 */
void espnow_deinit()
{
    vSemaphoreDelete(espnow_queue_msg);
    esp_now_deinit();
	//wifi_deinitialize();
}

  ///----------------------------File end ------------------------------
