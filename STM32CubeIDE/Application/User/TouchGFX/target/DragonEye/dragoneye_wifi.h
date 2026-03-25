/**
  ******************************************************************************
  * @file    dragoneye_wifi.h
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file contains definitions for DragonEYE WiFi hardware 
  *          resources.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 Anders Electronics plc.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRAGONEYE_WIFI_H
#define __DRAGONEYE_WIFI_H 0101 /* BSP WIFI API REVISION v.1.1 */

#ifdef __cplusplus
	extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx_hal.h"
#include "main.h"

#define MAX_RSP_BYTES 1024

/** @addtogroup BSP
  * @{
  */

/** @addtogroup DragonEYE
  * @{
  */

/** @addtogroup DragonEYE_WIFI 
  * @{
  */

/** @defgroup DragonEYE_WIFI_Exported_Types DragonEYE WiFi Exported Types
 * @{
 */
#ifndef MY_VARIABLE_H
#define MY_VARIABLE_H

	extern  uint8_t WiFi_Status;
	extern  uint8_t BT_Status;

#endif
/** @brief WIFI_RET_TypeDef
  *  WiFi access modes definitions.
  */
typedef enum
{
	WIFI_OK = 0,
	WIFI_ERROR,
} WIFI_RET_TypeDef;

typedef enum
{
	BT_OK = 0,
	BT_ERROR,
} BT_RET_TypeDef;


/** @brief WIFI_MODE_TypeDef
  *  WiFi access modes definitions.
  */
typedef enum
{
	STA_MODE = 1,
	AP_MODE = 2,
	BOTH_MODE = 3,
} WIFI_MODE_TypeDef;

/** @brief WIFI_HTTP REQUEST_TypeDef
  */

typedef enum
{
	HEAD_REQUEST = 1,
	GET_REQUEST = 2,
} WIFI_HTTPREQUEST_TypeDef;

typedef struct {
    char* command;
    char* description;
} ATCommand;

typedef enum
{
	OFF = 0,
	ON = 1,
	Searching = 2,
	Connected = 3,
	DisConnected = 4
} WiFi_BT_Status;

/** @brief WIFI_ENCRYPTION_TYPE_TypeDef
  */
typedef enum
{
	NO_ENCRYPTION = 0,
	WEP_ENCRYPTION = 1,
	WPS_PSK_ENCRYPTION = 2,
	WPA2_PSK_ENCRYPTION = 3,
	WPA_WPA2_PSK_ENCRYPTION = 4,
} WIFI_ENCRYPTION_TypeDef;

/** @brief WIFI_DHCP_MODE_TypeDef
  */
typedef enum
{
	Disable_DHCP = 0,
	ENABLE_DHCP = 1,
} WIFI_DHCP_MODE_TypeDef;

/** @brief WIFI_DHCP_OPERATION_TypeDef
  */
typedef enum
{
	DHCP_STA_MODE = 0,
	DHCP_SOFTAP_MODE = 1,
	DHCP_BOTH_MODE = 2,
} WIFI_DHCP_OPERATION_TypeDef;


/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Exported_Constants DragonEYE WiFi Exported Constants
  * @{
  */
  

/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Exported_Macros DragonEYE WiFi Exported Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Exported_Functions DragonEYE WiFi Exported Functions
  * @{
  */
WIFI_RET_TypeDef	BSP_WIFI_BT_Init(UART_HandleTypeDef *huart);
WIFI_RET_TypeDef	BSP_WIFI_Restart(void);
void 				BSP_WIFI_DeInit(void);
WIFI_RET_TypeDef 	BSP_WIFI_Check_Info(char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_SetMode(int WIFIMODE);
WIFI_RET_TypeDef	BSP_WIFI_Connect_Wifi_Station(char *ssid, char *password);
WIFI_RET_TypeDef	BSP_WIFI_Disconnect_from_AP();
WIFI_RET_TypeDef	BSP_WIFI_Reconnecting_configuration(uint16_t interval_seconds, uint16_t iterations);
WIFI_RET_TypeDef 	BSP_WIFI_List_Ap(char *ssid, char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_Get_WIFI_State(char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_Enable_IPv6();
WIFI_RET_TypeDef	BSP_WIFI_Disable_IPv6();
WIFI_RET_TypeDef 	BSP_WIFI_Establish_Connection(char *type, char *host, uint8_t port);
WIFI_RET_TypeDef	BSP_WIFI_Close_Connection();
WIFI_RET_TypeDef	BSP_WIFI_HTTP_client(char *url ,char *host,int request_type, char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_HTTP_GetSize(char *url, char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_Configure_SoftAP(char *ssid, char *password,int channel,int encryption);
WIFI_RET_TypeDef	BSP_WIFI_Obtain_IP(char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_Disconnect_Stations_from_ESP();
WIFI_RET_TypeDef	BSP_WIFI_DHCP_Configuration(int DHCP_operation, int DHCP_mode);
WIFI_RET_TypeDef	BSP_WIFI_DHCP_IP_Configuration(int DHCP_status, int time,char *IP_Start_Range ,char*IP_End_Range);
WIFI_RET_TypeDef 	BSP_WIFI_Set_Static_IP(char *Station_IP ,char *Gateway_IP, char *Netmask);
WIFI_RET_TypeDef	BSP_WIFI_Send_Data(char *data);
WIFI_RET_TypeDef	BSP_WIFI_Get_IP_Address(char *rspBuffer);
WIFI_RET_TypeDef	BSP_WIFI_configure_connections(int number_of_conenctions);
WIFI_RET_TypeDef	BSP_WIFI_TCP_server(int enable_server, int port);
WIFI_RET_TypeDef	BSP_WIFI_HTTP_GET_Request(char *url, char *rspBuffer);
WIFI_RET_TypeDef 	BSP_WIFI_Ping_Module(void);
WIFI_RET_TypeDef 	BSP_WIFI_Get_RSSI(char *rspBuffer);
WIFI_RET_TypeDef 	BSP_WIFI_Ping(char *host, char *rspBuffer);
WIFI_RET_TypeDef 	BSP_WIFI_Get_MAC(char *rspBuffer);
WIFI_RET_TypeDef 	BSP_WIFI_AutoConnect(uint8_t enable);
WIFI_RET_TypeDef 	BSP_WIFI_Set_Hostname(char *hostname);


BT_RET_TypeDef		BSP_BT_BLEINIT(int mode);
BT_RET_TypeDef 		BSP_BT_BLE_DEINIT();
BT_RET_TypeDef 		BSP_BT_BLE_ADDR(int mode);
BT_RET_TypeDef		BSP_BT_BLE_Advertising_Data(char *data);
BT_RET_TypeDef 		BSP_BT_BLE_Start_Advertising();
BT_RET_TypeDef 		BSP_BT_BLE_Stop_Advertising();



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* __DRAGONEYE_WIFI_H */
