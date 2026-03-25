/**
  ******************************************************************************
  * @file    dragoneye_wifi.c
  * @author  Anders Engineering Team
  * @version 1.0
  * @brief   This file provides a set of functions needed to manage the Wi-Fi
  *          expansion feature on DragonEYE board.
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

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Driver description:
----------------------
   - This driver is used to provide Wi-Fi access to the DragonEYE board.
     The WiFi support is achieved through IC responding to AT Command prototcol.
	 As such this layer does also provide full TCP/IP and UDP protocol stack
	 interfaces to setup full server/client communications. 

2. How To use this driver:
--------------------------
  + Initialization steps:
     o Initialize the Wi-Fi module using the BSP_WIFI_SetUp(wifi_mode) function.
	   This function performs hardware resources initialization and configure the 
       WiFi in either Access Point or Station (or both) mode.

		Following are list of example parameters that can be passed to the functions.

	   Examples for calling Functions:
	   	BSP_WIFI_Get_WIFI_State(rspBuffer);
		BSP_WIFI_Restart();
		BSP_WIFI_SetMode(STA_MODE);
		BSP_WIFI_SetMode(AP_MODE);
		BSP_WIFI_SetMode(BOTH_MODE);
		BSP_WIFI_Configure_SoftAP("DragonEYE78", "12345678",5,WPS_PSK_ENCRYPTION);
		BSP_WIFI_Obtain_IP(rspBuffer);
		BSP_WIFI_Disconnect_Stations_from_ESP();
		BSP_WIFI_Check_Info(rspBuffer);
		BSP_WIFI_Connect_Wifi_Station("Anders Guests","XXXXXXXXXX");
		BSP_WIFI_Connect_Wifi_Station("ZZOOMM-8A25D5","XXXXXXXXXX");
		BSP_WIFI_Disconnect_from_AP();
		BSP_WIFI_Reconnecting_configuration(5,5);
		BSP_WIFI_List_Ap("Anders Guests", rspBuffer);
		BSP_WIFI_List_Ap("", rspBuffer);
		BSP_WIFI_Enable_IPv6();
		BSP_WIFI_Establish_Connection("TCP", "mail.server.net", 25);
		BSP_WIFI_Establish_Connection("TCP", "example.com", 80);
		BSP_WIFI_Establish_Connection("UDP", "pool.ntp.org", 123);
		BSP_WIFI_Close_Connection();
		BSP_WIFI_HTTP_client("http://httpbin.org/get","httpbin.org",HEAD_REQUEST,rspBuffer);
		BSP_WIFI_HTTP_client("http://httpbin.org/get","httpbin.org",GET_REQUEST,rspBuffer);
		BSP_WIFI_HTTP_GetSize("http://www.baidu.com/img/bdlogo.gif",rspBuffer);
		BSP_WIFI_HTTP_GetSize("http://example.com",rspBuffer);
		BSP_WIFI_DHCP_Configuration(ENABLE_DHCP,DHCP_SOFTAP_MODE);
		BSP_WIFI_DHCP_IP_Configuration(1,120,"192.168.4.100","192.168.4.150");
		BSP_WIFI_Set_Static_IP("192.168.1.100","192.168.1.1","255.255.255.0");
		BSP_WIFI_Send_Data("Test_String");
		BSP_WIFI_Get_IP_Address(rspBuffer);
		BSP_WIFI_configure_connections(1);
		BSP_WIFI_TCP_server(1, 80);
		BSP_WIFI_HTTP_GET_Request("http://example.com",rspBuffer);
        BSP_WIFI_Ping_Module();                               // Check if module responds to AT
        BSP_WIFI_Get_MAC(rspBuffer);                           // Get MAC address
        BSP_WIFI_Get_RSSI(rspBuffer);                          // Get signal strength
        BSP_WIFI_Ping("google.com", rspBuffer);                // Ping remote server
        BSP_WIFI_Ping("192.168.1.1", rspBuffer);               // Ping local router
        BSP_WIFI_AutoConnect(1);                                // Enable auto reconnect
        BSP_WIFI_AutoConnect(0);                                // Disable auto reconnect
        BSP_WIFI_Set_Hostname("DragonEye_Device");             // Set device hostname


 	 	BSP_BT_BLEINIT(1);                       				// Initialize BLE in Client mode
        BSP_BT_BLEINIT(2);                       				// Initialize BLE in Server mode
        BSP_BT_BLE_ADDR(0);                      				// Use Public Address
        BSP_BT_BLE_ADDR(1);                      				// Use Private Address
        BSP_BT_BLE_Advertising_Data("06 09 48 65 6C 6C 6F");	// Example advertising name "Hello"
        BSP_BT_BLE_Advertising_Data("02 01 06 11 09 41 4E 44 45 52 53 2D 44 72 61 67 6F 6E 45 79 65");	// Example advertising name "ANDERS-DragonEye"
        BSP_BT_BLE_Start_Advertising();          // Start BLE advertising
        BSP_BT_BLE_Stop_Advertising();           // Stop BLE advertising
        BSP_BT_BLE_DEINIT();                     // Deinitialize BLE

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "dragoneye_wifi.h"
#include "string.h"
#include "stdio.h"
#include "../../../Core/Inc/main.h"


#if __DRAGONEYE_WIFI_H != 0101	/* BSP WIFI REVISION v.1.1 */
#error Wrong API version file (dragoneye_wifi.h).
#endif


/** @addtogroup BSP
  * @{
  */

/** @addtogroup DragonEYE
  * @{
  */

/** @defgroup DragonEYE_WIFI DragonEYE WiFi
  * @{
  */

/** @defgroup DragonEYE_WIFI_Private_Types_Definitions DragonEYE WiFi Private Types Definitions
  * @{
  */


/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Private_Defines DragonEYE WiFi Private Types Defines
  * @{
  */

#define ESP_RESET GPIO_PIN_3
#define ESP_CP_PD GPIO_PIN_9
#define ESP_GPIO_2 GPIO_PIN_6
#define ESP_GPIO_0 GPIO_PIN_7

#define ESP_RESET_PORT GPIOE
#define ESP_CP_PD_PORT GPIOG
#define ESP_GPIO_2_PORT GPIOB
#define ESP_GPIO_0_PORT GPIOD


/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Private_Macros DragonEYE WiFi Private Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Imported_Variables DragonEYE WiFi Imported Variables
  * @{
  */
  /**
    * @}
    */

/** @defgroup DragonEYE_WIFI_Private_Variables DragonEYE WiFi Private Variables
  * @{
  */

uint8_t WiFi_Status = 0;
uint8_t BT_Status = 0;

/**
  * @}
  */


/** @defgroup DragonEYE_WIFI_Private_Function_Prototypes DragonEYE WiFi Private Function Prototypes
  * @{
  */

uint8_t sendATCommand(const char *cmd, char *response, uint32_t timeout);
static UART_HandleTypeDef *p_huart = NULL;  // Static global variable for internal use only
/**
  * @}
  */

/** @defgroup DragonEYE_WIFI_Public_Functions DragonEYE WiFi Public Functions
  * @{
  */

/**
  * @brief  Initialize WiFi with a reset.
  */
WIFI_RET_TypeDef BSP_WIFI_BT_Init(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef  gpio_init_structure;

	  /* Configure the generic GPIOs */
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();

	  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
	  gpio_init_structure.Pull  = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_HIGH;

	  gpio_init_structure.Pin   = ESP_RESET;
	  HAL_GPIO_WritePin(ESP_RESET_PORT, gpio_init_structure.Pin, GPIO_PIN_SET);
	  HAL_GPIO_Init(ESP_RESET_PORT, &gpio_init_structure);

	  HAL_Delay(100);
	  HAL_GPIO_WritePin(ESP_RESET_PORT, ESP_RESET, GPIO_PIN_SET);
	  HAL_Delay(100);

	  char rspBuffer[MAX_RSP_BYTES] = {0};
	  p_huart = huart;
	  WiFi_Status = OFF;
	  HAL_Delay(500);

	  //Clear buffer
	  HAL_UART_Receive (p_huart, (uint8_t *)rspBuffer, strlen(rspBuffer), 10);

	  if(sendATCommand("AT", (char *)rspBuffer, 500)){
		  HAL_Delay(500);
		  if(sendATCommand("AT+RST", (char *)rspBuffer, 1000)){
			  // Wait for ESP32C3 to reset (adjust delay as needed)
			  HAL_Delay(500);
			  return WIFI_ERROR;
		  }
		  //Clear buffer
		  HAL_UART_Receive (p_huart, (uint8_t *)rspBuffer, strlen(rspBuffer), 10);
	  }

	  WiFi_Status = ON;
	  return WIFI_OK;
}

/**
  * @brief  Wifi Restart.
  */
WIFI_RET_TypeDef BSP_WIFI_Restart(void)
{
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	snprintf(cmd, sizeof(cmd), "AT+RST\r\n");
	if(!sendATCommand(cmd, rspBuffer, 2000)){
		HAL_Delay(500);
		WiFi_Status = ON;
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  DeInit WiFi.
  * 			Puts the module to sleep cutting all radio functions, but CPU is still working.
  */
void BSP_WIFI_DeInit(void)
{
	/* Turn off WIFI_PORT */
	HAL_GPIO_WritePin(ESP_RESET_PORT, ESP_RESET, GPIO_PIN_RESET);
	HAL_GPIO_DeInit(ESP_RESET_PORT, ESP_RESET);

	WiFi_Status = OFF;
}


/**
  * @brief  Get Version Info.
  */
WIFI_RET_TypeDef BSP_WIFI_Check_Info(char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}
    snprintf(cmd, sizeof(cmd), "AT+GMR\r\n");
	if(!sendATCommand(cmd, rspBuffer, 1000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Set Mode for Wifi Module from the 3 defined modes.
  * @param	WIFIMODE: STA_MODE = 1,AP_MODE = 2,BOTH_MODE = 3
  */
WIFI_RET_TypeDef BSP_WIFI_SetMode(int WIFIMODE){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}
	snprintf(cmd, sizeof(cmd), "AT+CWMODE=%u\r\n", WIFIMODE);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Connect to WiFi in Station Mode with provided SSID and Password.
  * @param  ssid: Name of the Station to be connected.
  *         password: Password needed to establish Wi-Fi connection to the station.
  */
WIFI_RET_TypeDef BSP_WIFI_Connect_Wifi_Station(char *ssid, char *password){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}
	BSP_WIFI_SetMode(STA_MODE);

	snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		WiFi_Status = Connected;
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Disconnect from AP.
  */
WIFI_RET_TypeDef BSP_WIFI_Disconnect_from_AP(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}
	snprintf(cmd, sizeof(cmd), "AT+CWQAP\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		WiFi_Status = DisConnected;
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  When Disconnected from WiFi in Station Mode how many retries before giving up.
  * @param  interval: Number of seconds before retrying.
  *         iteration: Number of tries before Giving up.
  */
WIFI_RET_TypeDef BSP_WIFI_Reconnecting_configuration(uint16_t interval_seconds, uint16_t iterations){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}
	snprintf(cmd, sizeof(cmd), "AT+CWRECONNCFG=%u,%u\r\n", interval_seconds, iterations);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Listing All the available APs.
  * @param  ssid: Finding AP when the SSID is known, else leave empty i.e. "".
  *         rspBuffer: char buffer to store the respose.
  */
WIFI_RET_TypeDef BSP_WIFI_List_Ap(char *ssid, char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}
	if(!sendATCommand("AT+CWMODE=1\r\n", rspBuffer, 5000)){
			HAL_Delay(500);
			snprintf(cmd, sizeof(cmd), "AT+CWLAP=\"%s\"\r\n", ssid);
			if(!sendATCommand(cmd, rspBuffer, 5000)){
				HAL_Delay(500);
				return WIFI_OK;
	}
	}
	return WIFI_ERROR;
}

/**
  * @brief  Query the Wi-Fi state and Wi-Fi information.
  * @param rspBuffer :	Response Buffer
  */
WIFI_RET_TypeDef BSP_WIFI_Get_WIFI_State(char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CWSTATE?\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Enable the network of Internet Protocol Version 6 (IPv6).
  * 		which is essential before using IPv6 related upper layer AT commands
  */
WIFI_RET_TypeDef BSP_WIFI_Enable_IPv6(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPV6=1\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Disable the network of Internet Protocol Version 6 (IPv6).
  */
WIFI_RET_TypeDef BSP_WIFI_Disable_IPv6(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPV6=0\r\n");
	if(!sendATCommand(cmd, rspBuffer, 1000)){
		HAL_Delay(100);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Establish a TCP/UDP/SSL connection with the host.
  * @param  type: 3 type of connections can be made using this which are TCP,UDP,SSL
  * @param	host: host address that need to establish connection
  * @param	port: port is required to establish connection with anyhost
  * @example	:	BSP_WIFI_Establish_Connection("TCP", "mail.server.net", 25)
  */
WIFI_RET_TypeDef BSP_WIFI_Establish_Connection(char *type, char *host, uint8_t port){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"%s\",\"%s\",%u\r\n", type, host, port);
	if(!sendATCommand(cmd, rspBuffer, 10000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Close any connection made using TCP/UDP/SSL.
  */
WIFI_RET_TypeDef BSP_WIFI_Close_Connection(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPCLOSE\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Function to request HTTP GET and HEAD.
  * @param	url : HTTP URL
  * 		host: domain name or IP address
  * 		rspBuffer: Response Buffer
  */
WIFI_RET_TypeDef BSP_WIFI_HTTP_client(char *url ,char *host,int request_type, char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd),  "AT+HTTPCLIENT=%u,0,\"%s\",\"%s\",\"/get\",1\r\n",request_type,url,host);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Function to request HTTP Size.
  * @param	url : HTTP URL
  * 		rspBuffer: Response Buffer
  */
WIFI_RET_TypeDef BSP_WIFI_HTTP_GetSize(char *url, char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd),  "AT+HTTPGETSIZE=\"%s\"\r\n",url);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  SetUp external WiFi module in Soft Access Point Mode with default SSID and Password.
  * @param  ssid: Name of the SoftAP to be discovered.
  *         password: Password needed for stations to establish Wi-Fi connection.
  *         channel: The Wi-Fi channel on which the access point will operate
  *         encryption: The type of encryption used for the Wi-Fi network. The available options are:
				NO_ENCRYPTION = 0
				WEP_ENCRYPTION = 1
				WPS_PSK_ENCRYPTION = 2
				WPA2_PSK_ENCRYPTION = 3
				WPA_WPA2_PSK_ENCRYPTION = 4
  */
WIFI_RET_TypeDef BSP_WIFI_Configure_SoftAP(char *ssid, char *password,int channel,int encryption){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CWSAP=\"%s\",\"%s\",%u,%u\r\n", ssid, password, channel,encryption);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		WiFi_Status = Connected;
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Obtain IP from devices connected to ESP.
  * @param rspBuffer: Response Buffer
  */
WIFI_RET_TypeDef BSP_WIFI_Obtain_IP(char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CWLIF\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Disconnect connected devices from ESP.
  */
WIFI_RET_TypeDef BSP_WIFI_Disconnect_Stations_from_ESP(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CWQIF\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		BSP_BT_BLE_Start_Advertising();
		sendATCommand(cmd, rspBuffer, 5000);
		BSP_BT_BLE_Stop_Advertising();
		WiFi_Status = DisConnected;
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  configure the Dynamic Host Configuration Protocol (DHCP) settings.
  * @param	DHCP_operation
  * 				Disable_DHCP = 0
					ENABLE_DHCP = 1
  * 		DHCP_Mode
  * 				DHCP_STA_MODE = 0
					DHCP_SOFTAP_MODE = 1
					DHCP_BOTH_MODE = 2
  */
WIFI_RET_TypeDef BSP_WIFI_DHCP_Configuration(int DHCP_operation, int DHCP_mode){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CWDHCP=%u,%u\r\n",DHCP_operation,DHCP_mode);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  configure the DHCP setting for IP range allocation and lease time.
  * @param	DHCP_status: Disable 0, Enable 1
  * 		lease_time: time for each Ip can be allocated, range from 1-120 minutes.
  * 		IP_Start_Range: Ip start range i.e.192.168.4.100
  * 		IP_End_Range:	IP End Range i.e. 192.168.4.150
  */
WIFI_RET_TypeDef BSP_WIFI_DHCP_IP_Configuration(int DHCP_status, int lease_time,char *IP_Start_Range ,char*IP_End_Range){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CWDHCPS=%u,%u,\"%s\",\"%s\"\r\n",DHCP_status,lease_time,IP_Start_Range,IP_End_Range);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  configure the Static IP, GAteway IP and Netmask.
  * @param	Station IP: The static IP address that the ESP8266/ESP32 will use when connected to the Wi-Fi network.
  * 		Gateway IP: The IP address of the gateway (typically the router) on the network.
* 			NetMask:	The subnet mask used to determine the network segment
  */
WIFI_RET_TypeDef BSP_WIFI_Set_Static_IP(char *Station_IP ,char *Gateway_IP, char *Netmask){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPAP=\"%s\",\"%s\",\"%s\"\r\n",Station_IP, Gateway_IP, Netmask);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Send Data using TCp,UDP or SSL connection.
  * @param	data: Data that needs to be send to the already established connection
  */
WIFI_RET_TypeDef BSP_WIFI_Send_Data(char *data){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];
	int datalength = strlen(data);

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%u\r\n",datalength);
	sendATCommand(cmd, rspBuffer, 5000);

	snprintf(cmd, sizeof(cmd), "\"%s\"\r\n\r\n",data);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Query the IP address of the device when it has established a network connection.
  * @param	rspBuffer: Response Buffer
  */
WIFI_RET_TypeDef BSP_WIFI_Get_IP_Address(char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIFSR\r\n");
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Set the number of TCP/UDP/SSL connection, If more than 1 required..
  * @param	number_of_connections: Range is 1-4, Maximum 4 connections can be established at any single time.
  */
WIFI_RET_TypeDef BSP_WIFI_configure_connections(int number_of_connections){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPMUX=%u\r\n",number_of_connections);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Set the number of TCP/UDP/SSL connection, If more than 1 required.
  * @param	number_of_connections: Range is 1-4, Maximum 4 connections can be established at any single time.
  */
WIFI_RET_TypeDef BSP_WIFI_TCP_server(int enable_server, int port){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=%u,%u\r\n", enable_server, port);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Get HTTP Resource
  * @param	url	:	URL for the HTTP.
  * 		rspBuffer:	Response Buffer
  * @note	:	The response size is currently limited to 1024, If the HTTP Resource is larger than 1024,
  * 			which can be checked using BSP_WIFI_HTTP_GetSize("http://example.com",rspBuffer),
  * 			increase the response buffer size from MAX_RSP_BYTES.
  */
WIFI_RET_TypeDef BSP_WIFI_HTTP_GET_Request(char *url, char *rspBuffer){
	memset(rspBuffer, 0,MAX_RSP_BYTES);
	char cmd[256];

	if(WiFi_Status == OFF){
		return WIFI_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+HTTPCGET=\"%s\"\r\n", url);
	if(!sendATCommand(cmd, rspBuffer, 10000)){
		HAL_Delay(500);
		return WIFI_OK;
	}
	return WIFI_ERROR;
}

/**
  * @brief  Test if WiFi module is responding.
  */
WIFI_RET_TypeDef BSP_WIFI_Ping_Module(void){
    char rspBuffer[MAX_RSP_BYTES] = {0};

    if(sendATCommand("AT\r\n", rspBuffer, 1000) == 0){
        return WIFI_OK;
    }
    return WIFI_ERROR;
}

/**
  * @brief  Get WiFi Signal Strength (RSSI).
  * @param  rspBuffer: Response Buffer
  */
WIFI_RET_TypeDef BSP_WIFI_Get_RSSI(char *rspBuffer){
    memset(rspBuffer,0,MAX_RSP_BYTES);
    char cmd[64];

    if(WiFi_Status == OFF){
        return WIFI_ERROR;
    }

    snprintf(cmd,sizeof(cmd),"AT+CWJAP?\r\n");

    if(!sendATCommand(cmd,rspBuffer,2000)){
        HAL_Delay(100);
        return WIFI_OK;
    }

    return WIFI_ERROR;
}

/**
  * @brief  Ping a remote server
  * @param  host: hostname or IP
  * @param  rspBuffer: response buffer
  */
WIFI_RET_TypeDef BSP_WIFI_Ping(char *host, char *rspBuffer){

    memset(rspBuffer,0,MAX_RSP_BYTES);
    char cmd[128];

    if(WiFi_Status == OFF){
        return WIFI_ERROR;
    }

    snprintf(cmd,sizeof(cmd),"AT+PING=\"%s\"\r\n",host);

    if(!sendATCommand(cmd,rspBuffer,5000)){
        HAL_Delay(100);
        return WIFI_OK;
    }

    return WIFI_ERROR;
}

/**
  * @brief  Get WiFi MAC Address
  */
WIFI_RET_TypeDef BSP_WIFI_Get_MAC(char *rspBuffer){

    memset(rspBuffer,0,MAX_RSP_BYTES);

    if(WiFi_Status == OFF){
        return WIFI_ERROR;
    }

    if(!sendATCommand("AT+CIFSR\r\n", rspBuffer, 2000)){
        return WIFI_OK;
    }

    return WIFI_ERROR;
}

/**
  * @brief  Enable or Disable Auto Connect
  * @param  enable: 1 enable, 0 disable
  */
WIFI_RET_TypeDef BSP_WIFI_AutoConnect(uint8_t enable){

    char rspBuffer[MAX_RSP_BYTES] = {0};
    char cmd[64];

    if(WiFi_Status == OFF){
        return WIFI_ERROR;
    }

    snprintf(cmd,sizeof(cmd),"AT+CWAUTOCONN=%u\r\n",enable);

    if(!sendATCommand(cmd,rspBuffer,2000)){
        return WIFI_OK;
    }

    return WIFI_ERROR;
}

/**
  * @brief  Set device hostname
  */
WIFI_RET_TypeDef BSP_WIFI_Set_Hostname(char *hostname){

    char rspBuffer[MAX_RSP_BYTES] = {0};
    char cmd[128];

    if(WiFi_Status == OFF){
        return WIFI_ERROR;
    }

    snprintf(cmd,sizeof(cmd),"AT+CWHOSTNAME=\"%s\"\r\n",hostname);

    if(!sendATCommand(cmd,rspBuffer,2000)){
        return WIFI_OK;
    }

    return WIFI_ERROR;
}

/**
  * @brief  Initialize Bluetooth LE
  * @param	mode:
  *					1: client role
  *					2: server role
  *
  */
BT_RET_TypeDef BSP_BT_BLEINIT(int mode){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	snprintf(cmd, sizeof(cmd), "AT+BLEINIT=%u\r\n", mode);
	if(!sendATCommand(cmd, rspBuffer, 1000)){
		HAL_Delay(500);
		BT_Status = ON;
		return BT_OK;
	}
	return BT_ERROR;
}

/**
  * @brief  De-Initialize Bluetooth LE
  *
  */
BT_RET_TypeDef BSP_BT_BLE_DEINIT(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(BT_Status == OFF){
		return BT_ERROR;
	}
	snprintf(cmd, sizeof(cmd), "AT+BLEINIT=0\r\n");
	if(!sendATCommand(cmd, rspBuffer, 4000)){
		HAL_Delay(500);
		BT_Status = OFF;
		return BT_OK;
	}
	return BT_ERROR;
}

/**
  * @brief  Set BLE Address
  * @param mode :
  * 			0: Public Address
  * 			1: Private Address
  *
  */
BT_RET_TypeDef BSP_BT_BLE_ADDR(int mode){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(BT_Status == OFF){
		return BT_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+BLEADDR=%u\r\n",mode);
	if(!sendATCommand(cmd, rspBuffer, 2000)){
		HAL_Delay(500);
		return BT_OK;
	}
	return BT_ERROR;
}

/**
  * @brief  Set Bluetooth LE Advertising Data
  * @param data : 	BT name Hello : "06   09  48 65 6C 6C 6F"
  * 							    Size Flag H  e  l  l  o
  * 				BT name ANDERS-DragonEye : "02 	   01 	   06    11     09    41 4E 44 45 52 53 2D 44 72 61 67 6F 6E 45 79 65"
  * 										   Size DataType  Flag 	Size DataType  A  N  D  E  R  S  -  D  r  a  g  o  n  E  y  e
  *
  */
BT_RET_TypeDef BSP_BT_BLE_Advertising_Data(char *data){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(BT_Status == OFF){
		return BT_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+BLEADVDATA=\"%s\"\r\n",data);
	if(!sendATCommand(cmd, rspBuffer, 5000)){
		HAL_Delay(500);
		BSP_BT_BLE_Start_Advertising();
		sendATCommand(cmd, rspBuffer, 5000);
		BSP_BT_BLE_Stop_Advertising();
		return BT_OK;
	}
	return BT_ERROR;
}

/**
  * @brief  Start Bluetooth LE Advertising
  * @param none
  */
BT_RET_TypeDef BSP_BT_BLE_Start_Advertising(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(BT_Status == OFF){
		return BT_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+BLEADVSTART\r\n");
	if(!sendATCommand(cmd, rspBuffer, 4000)){
		HAL_Delay(500);
		BT_Status = Searching;
		return BT_OK;
	}
	return BT_ERROR;
}

/**
  * @brief  Stop Bluetooth LE Advertising
  * @param none
  */
BT_RET_TypeDef BSP_BT_BLE_Stop_Advertising(){
	char rspBuffer[MAX_RSP_BYTES] = {0};
	char cmd[256];

	if(BT_Status == OFF){
		return BT_ERROR;
	}

	snprintf(cmd, sizeof(cmd), "AT+BLEADVSTOP\r\n");
	if(!sendATCommand(cmd, rspBuffer, 1000)){
		HAL_Delay(500);
		BT_Status = DisConnected;
		return BT_OK;
	}
	return BT_ERROR;
}



/**
  * @}
  */


/** @defgroup DragonEYE_WIFI_Private_Functions DragonEYE WiFi Private Functions
  * @{
  */
// Function to send AT command and wait for response
uint8_t sendATCommand(const char *cmd, char *response, uint32_t timeout) {
	char rsp_str[MAX_RSP_BYTES] = {0};
	char *np=NULL;

    HAL_UART_Transmit(p_huart, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
    HAL_UART_Transmit(p_huart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

    HAL_UART_Receive(p_huart, (uint8_t *)rsp_str, MAX_RSP_BYTES, timeout);

	// Parse the response to find 'OK'.
    np = strstr(rsp_str, "OK\r\n");
	if (np != NULL){
		strncpy(response, rsp_str, np-rsp_str);
		return 0; //Expected Response found
	} else {
		strncpy(response, rsp_str, MAX_RSP_BYTES);
		return 1;//Error
	}
}



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

/**
  * @}
  */
