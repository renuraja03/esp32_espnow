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
     if( strstr((const char*)data, (const char*)"hello")  != NULL)
     {
       return 1;
     }
    return 0;
}

 
int parse_sensordataJSON(uint8_t *data, int data_len, int MaxLen)
{
     if( strstr((const char*)data, (const char*)"sensor")  != NULL)
     {
       return 1;
     }
    return 0;
}


int prepare_bootJSON(uint8_t *dst_ptr, int *dlen, int MaxLen)
{
   memset( dst_ptr, 0, MaxLen);
   *dlen = sprintf((char*)dst_ptr, "{message: \"welcome\"}" );
  return 1;   
}

int prepare_sensorACKJSON(uint8_t *dst_ptr, int *dlen, int MaxLen)
{
   memset( dst_ptr, 0, MaxLen);
   *dlen = sprintf((char*)dst_ptr, "{message: \"ok\"}" );
  return 1;   
}

int prepare_errorJSON(uint8_t *dst_ptr, int *dlen, int MaxLen)
{
   memset( dst_ptr, 0, MaxLen);
   *dlen = sprintf((char*)dst_ptr, "{message: \"error\"}" );
  return 1;   
}


//-----------------------------------
//          File end
//-----------------------------------
