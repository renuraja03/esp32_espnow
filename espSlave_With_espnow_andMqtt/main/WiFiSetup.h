/*
 *@File name   :WiFiSetup.h
 *
 *@Created on : 21-Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : WiFi init, set wifi credential.
 *
 */
#ifndef __WIFISETUP_H_
#define __WIFISETUP_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------
// Includes
//-------------------------------------------
#include "esp_err.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA

///Default ssid and passs
#define WIFI_SSID   "give-your-wifissid"
#define WIFI_PASS   "give-your-wifipass"
///the following size are derived from esp_wifi.h
#define WIFI_SSID_MAX_SIZE 32
#define WIFI_PASS_MAX_SIZE 64

//------------------------------------------
// Prototypes
//-------------------------------------------
void wifi_init(); ///init wifi in station mode and set ssid, pass
void wifi_deinitialize();
void wifi_init_ESPNOW_mode(); ///init wifi in espnow mode
void updateWiFiCredential(uint8_t *wifissid, uint8_t *wifipass);
void updateWiFiCredential_default();
#ifdef __cplusplus
}
#endif

#endif /* __WIFISETUP_H_ */