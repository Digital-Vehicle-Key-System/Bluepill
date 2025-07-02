/*
 * NFC_APP_Prog.c
 *
 *  Created on: Apr 15, 2025
 *      Author: Fathy
 */

// includes
#include "stm32f1xx_hal.h"
#include "NFC_APP_Int.h"
#include "NFC_Int.h"
#include <string.h>
#include "../../Services/CAN_Communication/CAN_interface.h"

// Global var
//UART_HandleTypeDef huart1;
uint8_t PN532_version[10]  ={0x32, 0x01, 0x06, 0x07};
//uint8_t Authorized_UID[10] ={0x2C, 0x94, 0x99, 0x2F};
uint8_t Authorized_UID[10] ={0x12, 0x34, 0x56, 0x78};

extern UART_HandleTypeDef *huart_nfc;


void byte_to_hex(unsigned char byte, char *output) {
    const char hex_chars[] = "0123456789ABCDEF";
    output[0] = hex_chars[(byte >> 4) & 0x0F];  // High nibble
    output[1] = hex_chars[byte & 0x0F];         // Low nibble
}


uint8_t NFC_Init(void)
{



	 uint8_t version[10];
	 uint8_t version_len;

	 uint8_t state=NFC_INIT_NOK;

	 // Initialize NFC with UART1
	 //MX_USART1_UART_Init();
	 //NFC_HAL_Init(&huart1);

	 // Check Firmware virsion
	 if (NFC_GetFirmwareVersion(version, &version_len) == NFC_OK)
	 {
		 if(memcmp(version, PN532_version, version_len) == 0)
		 {
			// Firmware version OK
			 state = NFC_INIT_OK;

//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 , GPIO_PIN_SET);
//			 HAL_Delay(1000);
//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 , GPIO_PIN_RESET);
//			 HAL_Delay(1000);
		 }
		 else
		 {
//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 , GPIO_PIN_SET);
//			 HAL_Delay(1000);
//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 , GPIO_PIN_RESET);
//			 HAL_Delay(1000);

			 state = NFC_INIT_UNKNOWN_VERSION;
		 }
	 }
	 else
	 {
//		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 , GPIO_PIN_SET);
//		 HAL_Delay(1000);
//		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 , GPIO_PIN_RESET);

		 state = NFC_INIT_UNKNOWN_VERSION;
	 }

	 //HAL_Delay(10);							// TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	 // SAM Configuration
	 if (NFC_SAMConfiguration() == NFC_OK)
	 {
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 , GPIO_PIN_SET);
		 HAL_Delay(1000);
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 , GPIO_PIN_RESET);

		 // SAM Configuration successful
		 state = NFC_INIT_OK;
	 }
	 else
	 {
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 , GPIO_PIN_SET);
		 HAL_Delay(1000);
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 , GPIO_PIN_RESET);

		 //SAM Configuration failed
		 state = NFC_INIT_SAM_FAILED;
	 }
	 return state;
}

void NFC_Runnable(void)
{
	uint8_t uid[10]={0};
	uint8_t uid_len=0;

	//if (NFC_RecievePassiveTarget(uid, &uid_len) == NFC_OK)
	if (NFC_InListPassiveTarget(uid, &uid_len) == NFC_OK)
	{
		if(memcmp(uid, Authorized_UID, uid_len) == 0)
		{
			// Authorized phone is tapped
			//HAL_UART_Transmit(huart_nfc, (uint8_t *)"Authorized phone is tapped ", 27, HAL_MAX_DELAY);
			  CAN1_TX(0xD ,3, "NFC");
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
			  HAL_Delay(500);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

		}
		else
		{
			// device is not authorized
			//HAL_UART_Transmit(huart_nfc, (uint8_t *)"device is not authorized ", 25, HAL_MAX_DELAY);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
			  HAL_Delay(500);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		HAL_Delay(1000);

	}
	else
	{
		//NO Tag detected
		//HAL_UART_Transmit(huart_nfc, (uint8_t *)"NO Tag detected ", 16, HAL_MAX_DELAY);
	}

}
