/*
 *@File name   :espnow_comm.h
 *
 *@Created on : 21_Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : ESPnow init, send, receive
 *
 */
#ifndef ESPNOW_COMM_H
#define ESPNOW_COMM_H

#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------
// Includes
//-------------------------------------------
#include "esp_now.h"


//------------------------------------------
// typdef / define
//-------------------------------------------
/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
//#if CONFIG_ESPNOW_WIFI_MODE_STATION

// #else
// #define ESPNOW_WIFI_MODE WIFI_MODE_AP
// #define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
// #endif

#define CONFIG_ESPNOW_PMK "espnow123"

#define MAX_WELCOME_MSG_TIMEOUT (TickType_t)(10000/portTICK_PERIOD_MS) ///master timeout
#define CONFIG_ESPNOW_CHANNEL 0
 
#define ESPNOW_QUEUE_SIZE           6

#define IS_BROADCAST_ADDR(addr) (memcmp(addr, broadcast_mac, ESP_NOW_ETH_ALEN) == 0)

#define SEND_DATA_MAX 255
#define RCV_DATA_MAX   SEND_DATA_MAX

typedef enum
{
    ESPNOW_IDLE,
    ESPNOW_SEND_BOOT,
    ESPNOW_SEND_BOOT_WAIT,
    ESPNOW_BOOT_RECV,
	ESPNOW_SEND_SENSOR,
	ESPNOW_SEND_SENSOR_WAIT,
	ESPNOW_SEND_SENSOR_RECV ,
    ESPNOW_FAIL
}espnow_state_t;

typedef struct{
    uint8_t smac_addr[ESP_NOW_ETH_ALEN];
    uint8_t rmac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
    uint8_t data[RCV_DATA_MAX];
    int dlen;
    espnow_state_t stat;
}espnow_event_t;

 

//------------------------------------------
// Functions Prototypes
//-------------------------------------------


esp_err_t espnow_init(void);
void espnow_StateMachine(void);
 
#ifdef __cplusplus
}
#endif

#endif /* ESPNOW_COMM_H */
 
