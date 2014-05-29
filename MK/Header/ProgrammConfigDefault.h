
#ifndef PROGRAMM_CONFIG_DEFAULT_H
#define PROGRAMM_CONFIG_DEFAULT_H

#include "stm32f4xx.h"

#include "Types.h"

/*ID DEVICE*/
#define ID_DEFAULT 	0
/*******************************************
VERSION Programm and DATA Relelease 
NESESSARILY CHANGE WITH THE NEW RELEASE
********************************************/
#define TOP_VERSION 	0
#define VERSION 			1
#define SUB_VERSION		0

#define DAY_VERSION		29
#define MONTH_VERSION	05
#define YEAR_VERSION	2014
/*******************************************/



#define SIZE_CONS_OUT		100 // size out bufer for console USART
#define SIZE_CONS_IN		20  // size input bufer for condole USART

#define BUFFER_SIZE     200  /*Maximum size bufer receive from FPGA 2048*/
#define TIME_RESET			20   /* 20*256 clk before reset*/



/*Priority interrupt */



/*Priority tasks*/
/*Period task*/
#define START_CAL_PERIOD			10000
#define TCP_IPConnect_PERIOD 	10000
#define PERIOD_LED_TASK 			10000


/**************************************
Network setting default
***************************************/
#define MAC_1			0x00
#define MAC_2			0x08
#define MAC_3			0xDC
#define MAC_4			0x01
#define MAC_5			0x02
#define MAC_6			0x03//MAC Address

#define IP_1 	 		192
#define IP_2			168
#define IP_3			0
#define IP_4			51//IP Address

#define GateWay_1	192
#define GateWay_2 168
#define GateWay_3 0
#define GateWay_4 1//Gateway Address

#define SubNet_1 	255
#define SubNet_2 	255
#define SubNet_3	255
#define SubNet_4	0//SubnetMask Address

#define PortScience 5000

/*Network settings server default*/
#define Dest_IP_1 	192
#define Dest_IP_2 	168
#define Dest_IP_3 	0
#define Dest_IP_4 	36 //DST_IP Address server

#define Dest_PORT 	5000 //DST_IP port server




/***************************************
EEPROM SETTINGS
****************************************/

#define WAYT_REQUEST_EEPROM 0xFFFF
#define I2C_PageSize        16
#define EE_START_STRUCT			0	
/****************************************
SEtting w5200
*****************************************/	
#define SOCK_CONFIG		2	// UDP
#define SOCK_DNS		2	// UDP
#define SOCK_DHCP		3	// UDP

#define MAX_BUF_SIZE		1460
#define KEEP_ALIVE_TIME	30	// 30sec

#define ON	1
#define OFF	0

#define HIGH		1
#define LOW		0

// SRAM address range is 0x2000 0000 ~ 0x2000 4FFF (20KB)
#define TX_RX_MAX_BUF_SIZE	2048
#define TX_BUF	0x20004000
#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)
#define ApplicationAddress 	0x08004000


#endif

