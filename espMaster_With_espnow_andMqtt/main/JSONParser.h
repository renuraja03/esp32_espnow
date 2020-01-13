/*
 *@File name   :JSONParser.h
 *
 *@Created on : 22-Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : JSON framer, Parser
 *
 */
#ifndef __JSON_PARSER__H
#define __JSON_PARSER__H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------
// Includes
//-------------------------------------------
#include "esp_err.h"
 
 
//------------------------------------------
// CJOSN constant
//-------------------------------------------
 typedef int (*jsonFunc)(uint8_t *dst_ptr, int *dlen, int MaxLen);


//------------------------------------------
// Prototypes
//-------------------------------------------
int parse_bootJSON(uint8_t *data, int data_len, int MaxLen);
int prepare_bootJSON(uint8_t *dst_ptr, int *dlen, int MaxLen);
int prepare_sensorACKJSON(uint8_t *dst_ptr, int *dlen, int MaxLen);
int parse_sensordataJSON(uint8_t *data, int data_len, int MaxLen);
int prepare_errorJSON(uint8_t *dst_ptr, int *dlen, int MaxLen);


#ifdef __cplusplus
}
#endif

#endif /* __JSON_PARSER__H */