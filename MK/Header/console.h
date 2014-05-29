

#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "main.h"

typedef  uint8_t fpga_size_byte;
//=================================================
/*setting W5200 and Server's IP*/

typedef struct _CONFIG_MSG
{
	uint8 version[3];
	uint8 day;
	uint8 month;
	uint16 year;
	
	int ID;
	
	uint8 Mac[6];
	uint8 Lip[4];
	uint8 Sub[4];
	uint8 Gw[4];
	uint8 DNS_Server_IP[4];
	uint8  DHCP;

	uint16 port;
	uint8 destip[4];
			
}
CONFIG_MSG;



void CONSOLE_USART_INIT(void);
void CommandProcessing( char *bufer_in, char *bufer_out);
void console_send( char* str);
void 	LED_INIT(void);
bool ReadConfig(void);

#endif

