/*
 *@File name   :MQTTComm.h
 *
 *@Created on : 21-Dec-2019
 *
 *@Author   : Rajarajan
 *
 *@Description  : MQTT init, set wifi credential.
 *
 */
#ifndef __MQTT_COMM__H
#define __MQTT_COMM__H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------
// Includes
//-------------------------------------------
#include "esp_err.h"

 //------------------------------------------
// MQTT config
//------------------------------------------- 
#define MQTT_SERVER_IP "x.x.x.x" ///mqtt broker ip address 
#define MQTT_SERVER_PORT portnumber ///mqttt broker port number

//------------------------------------------
// MQTT topics
//-------------------------------------------
///sub topics
#define TOPIC_SUB_SENSOR_CONFIG "/sensor/config"

///Publish topic
#define TOPIC_PUB_SENSOR_ACQTICK "/sensor/acqtick"
#define TOPIC_PUB_SENSOR_DATA "/sensor/data"

//------------------------------------------
// Prototypes
//-------------------------------------------
 
void InitMQTT(void);
void mqttPublish(char *dataptr);
void MQTT_StateMachine();

#ifdef __cplusplus
}
#endif

#endif /* __MQTT_COMM__H */