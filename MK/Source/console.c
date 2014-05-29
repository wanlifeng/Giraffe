/******************** (C) COPYRIGHT 2014 Kazakov Andrey ********************
* File Name          : console.c
* Author             : Kazakov Andrey
* Version            : V1.0
* Date               : 28/05/2014
* Description        : This file provides a set of functions needed to manage the
*                      communication between user and device via console 
*******************************************************************************/

#include "console.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


extern SemaphoreHandle_t xMutexUSART_CONSOLE;
CONFIG_MSG Config_Msg;
/********privat function *****************/
void send_byte(char data);
void SettingsDefault(void);
bool CheckAndWriteVersion(void);


enum tel_cmd {
  HELP_CMD,
	PRINT,
	SETTINGS,
	DEFAULT,
	SAVE,
	REBOOT,
    
};

// Command table
char *commands[] = {
  "help",
	"print",
	"set",
	"default",
	"save",
	"reboot",
  NULL
};

enum set_cmd {
  IP,
	MASK,
	DNS,
	GATEWAY,
	PORT,
    
};

// Command table
char *set_comm[] = {
  "ip",
	"mask",
	"dns",
	"gateway",
	"port",
	NULL
};

/*********************************************
* Function Name  : LED_INIT
* Description    : init led for indication work of different process and freertos
*********************************************/
void LED_INIT(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(LED_GPIO_RCC, ENABLE);
 
    GPIO_StructInit(&GPIO_InitStructure);
    
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = LED1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	  GPIO_ResetBits(LED_GPIO,LED1);
}

/*********************************************
* Function Name  : DEBUG_USART_INIT
* Description    : init console for control and debug devices
*********************************************/

void CONSOLE_USART_INIT(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
/* Enable GPIOx and AFIO clocks */

	RCC_APB2PeriphClockCmd(USART_CLK , ENABLE);

	RCC_AHB1PeriphClockCmd(USART_PORT , ENABLE);

  /* Configure    USART2 Tx as alternate function push-pull */

  GPIO_InitStructure.GPIO_Pin = USART_GPIO_TX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_Init(USART_GPIO, &GPIO_InitStructure);

  /* Configure    USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin =  USART_GPIO_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  
  GPIO_Init(USART_GPIO, &GPIO_InitStructure);
	
	GPIO_PinAFConfig( USART_GPIO, USART_TX_SOURCE, USART_AF );
	GPIO_PinAFConfig( USART_GPIO, USART_RX_SOURCE, USART_AF );

  /* Enable USART2 clocks */

	USART_InitStructure.USART_BaudRate = 921600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART, &USART_InitStructure);
	//USART_Cmd(USART,ENABLE);
	
	__enable_irq(); 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	

	NVIC_EnableIRQ(USART1_IRQn); 
	NVIC_SetPriority(USART1_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1); 
	USART->CR1 |= USART_CR1_RXNEIE; 
	USART_Cmd(USART,ENABLE);
}

/******************************************************************************
* Function Name  : ResetStart
* Description    : Reboot system
*******************************************************************************/
void ResetStart(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(TIME_RESET);
	IWDG_ReloadCounter();
	IWDG_Enable();
}
/******************************************************************************
* Function Name  : ReadConfig
* Description    : settings all parameter if eeprom don't read or empty
*******************************************************************************/
bool ReadConfig(void)
{
	if (I2C_EE_BufferRead((u8*)&Config_Msg,EE_START_STRUCT,sizeof(Config_Msg))==TRUE) /*if read struct from eeprom without error*/
	{
		/*if  eeprom is empty*/
		if((Config_Msg.version[0]==0)&&(Config_Msg.version[1]==0)&&(Config_Msg.version[2]==0))
		{
			/*Eeprom is empty. Fill the structure with default values ??and write*/
			SettingsDefault();
			if (I2C_EE_BufferWrite((u8*)&Config_Msg,EE_START_STRUCT,sizeof(Config_Msg))==FALSE) return FALSE;				
		}
		else
		{
			/*if eeprom is not empty, check version*/
			if (CheckAndWriteVersion()==FALSE)
			{
				if (I2C_EE_BufferWrite((u8*)&Config_Msg,EE_START_STRUCT,sizeof(Config_Msg))==FALSE) return FALSE;
			}
		}	
	}		
	else
	{
			/*if error read from eeprom*/
			SettingsDefault();
			return FALSE;
		
	}
	return TRUE;
}
/******************************************************************************
* Function Name  : SettingsDefault
* Description    : settings all parameter if eeprom don't read or empty
*******************************************************************************/

void SettingsDefault(void)
{
		Config_Msg.ID = 				ID_DEFAULT;
			// MAC ADDRESS
		Config_Msg.Mac[0] = MAC_1;
		Config_Msg.Mac[1] = MAC_2;
		Config_Msg.Mac[2] = MAC_3;
		Config_Msg.Mac[3] = MAC_4;
		Config_Msg.Mac[4] = MAC_5;
		Config_Msg.Mac[5] = MAC_6;
		// Local IP ADDRESS
		Config_Msg.Lip[0] = IP_1;
		Config_Msg.Lip[1] = IP_2; 
		Config_Msg.Lip[2] = IP_3; 
		Config_Msg.Lip[3] = IP_4;
		// GateWay ADDRESS
		Config_Msg.Gw[0] = GateWay_1;
		Config_Msg.Gw[1] = GateWay_2;
		Config_Msg.Gw[2] = GateWay_3;
		Config_Msg.Gw[3] = GateWay_4;
		// Subnet Mask ADDRESS
		Config_Msg.Sub[0] = SubNet_1;
		Config_Msg.Sub[1] = SubNet_2; 
		Config_Msg.Sub[2] = SubNet_3; 
		Config_Msg.Sub[3] = SubNet_4;
		
		//Destination IP address for TCP Client
		Config_Msg.destip[0] = Dest_IP_1; 
		Config_Msg.destip[1] = Dest_IP_2;
		Config_Msg.destip[2] = Dest_IP_3; 
		Config_Msg.destip[3] = Dest_IP_4;
		//Destination PORT for TCP Client
		Config_Msg.port = Dest_PORT;
	}
/******************************************************************************
* Function Name  : CheckAndWriteVersion
* Description    : Checking the version of the project in eeprom,  
*									if not te same, then replace
*******************************************************************************/

bool CheckAndWriteVersion(void)
{	
				
		if ((Config_Msg.version[0]!=	TOP_VERSION)&&(Config_Msg.version[1]!=VERSION)&&(Config_Msg.version[2]!= SUB_VERSION))
		{
			/*Version and Date, update*/
			Config_Msg.version[2]	=	TOP_VERSION;
			Config_Msg.version[1]	= VERSION;
			Config_Msg.version[0]	= SUB_VERSION;
			Config_Msg.day =				DAY_VERSION;
			Config_Msg.month =			MONTH_VERSION;	
			Config_Msg.year = 			YEAR_VERSION;	
			return FALSE;
		}
		
		return TRUE;
}

/******************************************************************************
* Function Name  : CommandProcessing
* Description    : Processing input command 
*******************************************************************************/
void CommandProcessing( char *bufer_in, char *bufer_out)
{
	
	char **cmdp;
	char **set_cmdp;
  char *cp;
	char *help = {"\nhelp: Show all available commands\
    \r\n print: show all settings parameter\
    \r\n set: setting relevant parameters, need to save after\
		\r\n default: setting default parameters, need to save after\
		\r\n save: change settings to save\
		\r\n reboot: reboot system, start with new settings\r"}; /* command HELP : Message */ 

		for(cp = bufer_in; *cp != '\0';  cp++){
    *cp = tolower(*cp);     /* Translate big letter to small letter */       
		} 
		
		if(*bufer_in != '\0') {
    /* Find the input command in table; if it isn't there, return Syntax Error */
    for(cmdp = commands; *cmdp != NULL; cmdp++) {      
      if(strncmp(*cmdp, bufer_in, strlen(*cmdp)) == 0) break;      
    }
		
		 if(*cmdp == NULL) {
      sprintf(bufer_out,"\nBAD command\r");
      return;
    }
		 switch(cmdp - commands) {
					 
				case HELP_CMD:     
							sprintf(bufer_out, help);
					
					break;
				
				case PRINT:    
						sprintf(bufer_out,"\nSET IP = %u.%u.%u.%u\r",Config_Msg.Lip[0],Config_Msg.Lip[1],Config_Msg.Lip[2],Config_Msg.Lip[3]);	
	
				break;
				
				case SETTINGS :         
					
				
					break;
				case DEFAULT :
						SettingsDefault();
				break;				
				case SAVE :      
						if (I2C_EE_BufferWrite((u8*)&Config_Msg,EE_START_STRUCT,sizeof(Config_Msg))==FALSE) sprintf(bufer_out,"\ncould not save\r");	
						else sprintf(bufer_out,"\nsave successfully\r");
				break;
				case REBOOT :     
					sprintf(bufer_out, "\ndevice rebooot\r");
					ResetStart();
				default :
					break;
			}
	}
}

/******************************************************************************
* Function Name  : send_byte
* Description    : Send byte to usart
*******************************************************************************/	
void send_byte(char data)
{
	while(!(USART->SR & USART_SR_TC));
	USART_SendData(USART,data);
}
/******************************************************************************
* Function Name  : usart_send
* Description    : send string to out 
*******************************************************************************/	
void console_send( char* string)
{
	int i=0;
	
	xSemaphoreTake(xMutexUSART_CONSOLE , portMAX_DELAY);

	
	while(string[i])
	{
		send_byte(string[i]);
			i++;
	}
	xSemaphoreGive(xMutexUSART_CONSOLE );

}
/******************************************************************************
* Function Name  : 
* Description    : 
*******************************************************************************/
