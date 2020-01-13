/*
 *@File name   :main.h
 *
 *@Created on : 25-Nov-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : device configurations
 *
 */
#ifndef __MAIN_H_
#define __MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------
// Includes
//-------------------------------------------
 
 
 extern TaskHandle_t xTask_Mqtt;
 extern TaskHandle_t xTask_espnow;
//------------------------------------------
// device configurations
//-------------------------------------------
#define ESPNOW_DEVICE  "Slave" 
#define ESPNOW_FW_VER "1.0.0"

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H_ */