/**
 * @file   :JSONParser.c
 *
 * @created : 22-Decc-2019
 *
 * @author   : Rajarajan
 *
 * @description  : JSON framer and parser
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
#include "JSONParser.h"
#include <string.h>

//---------------------------------------------------------------
// global variables:
//-------------------------
 
//---------------------------------------------------------------
// Static functions prototypes:
//----------------------------
//cmdCallback_t cmdCallback[] = {
//	{prepare_bootJSON, parse_bootJSON, "BOOTMSG"},
//	{NULL            , NULL          , NULL}
//};


//---------------------------------------------------------------
// class and function definitions:
//------------------------------
/**
* @brief  :  
* @param  :  
* @return : none
*/
int parse_bootJSON(uint8_t *data, int data_len, int MaxLen)
{
    if( strstr((const char*)data, (const char*)"welcome") != NULL )
	{
		return 1;
	}
    return 0;
}

 
int prepare_bootJSON(uint8_t *dst_ptr, int *dlen, int MaxLen)
{
   memset( dst_ptr, 0, MaxLen);
   *dlen = sprintf((char*)dst_ptr, "{message: \"hello\"}" );
  return 1;   
}

int prepare_sensorJSON(uint8_t *dst_ptr, int *dlen, int MaxLen)
{
	static volatile int i=0;
	//{type: “sensor”, data: 123(random integer between 1-4000), deviceId: “2d:4f:23:e4:34:ee”(receiver’s mac address)}
   memset( dst_ptr, 0, MaxLen);
   *dlen = sprintf((char*)dst_ptr, "{type: \"sensor\", data:%d}", i++ );
  return 1;   
}

//-----------------------------------
//          File end
//-----------------------------------
