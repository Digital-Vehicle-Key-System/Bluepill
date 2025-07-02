/*
 * NFC_Int.h
 *
 *  Created on: Jan 13, 2025
 *      Author: Fathy
 */

#ifndef HAL_NFC_NFC_INT_H_
#define HAL_NFC_NFC_INT_H_



#include "stm32f1xx_hal.h"  // Include HAL library for STM32

// PN532 Commands
#define PN532_CMD_GETFIRMWAREVERSION 				0x02
#define PN532_CMD_GETFIRMWAREVERSION_RESPONSE   	0x03
#define PN532_CMD_SAMCONFIGURATION   				0x14
#define PN532_CMD_SAMCONFIGURATION_RESPONSE     	0x15
#define PN532_CMD_INLISTPASSIVETARGET 				0x4A
#define PN532_CMD_INLISTPASSIVETARGET_RESPONSE  	0x4B
#define PN532_CMD_INDATAEXCHANGE     				0x40

// PN532 UART Frame Constants
#define PN532_PREAMBLE 		0x00
#define PN532_STARTCODE1 	0x00
#define PN532_STARTCODE2 	0xFF
#define PN532_POSTAMBLE 	0x00

// TFI Frame Identifier
#define TFI_HOST_TO_PN532	0xD4
#define TFI_PN532_TO_HOST	0xD5

// NFC Tag Types
#define NFC_TAG_TYPE_MIFARE 0x00
#define NFC_TAG_TYPE_FELICA 0x01

// NFC Error Codes
#define NFC_OK    0
#define NFC_ERROR 1

// NFC ACK Codes
#define NFC_ACK   0
#define NFC_NACK  1

// Function Prototypes
void NFC_HAL_Init(UART_HandleTypeDef *huart);
void NFC_WakeUpFrame (void);
void NFC_SendACK(void);
uint8_t NFC_CheckACK(void);
uint8_t NFC_CalculateChecksum(uint8_t *data, uint8_t len);
uint8_t NFC_SendCommand(uint8_t cmd, uint8_t *data, uint8_t len);
uint8_t NFC_ReadResponse(uint8_t *response, uint8_t *response_len, uint32_t timeout);
uint8_t NFC_GetFirmwareVersion(uint8_t *version, uint8_t* version_len);
uint8_t NFC_SAMConfiguration();
uint8_t NFC_InListPassiveTarget(uint8_t *uid, uint8_t *uid_len);
uint8_t NFC_InDataExchange(uint8_t *data, uint8_t len, uint8_t *response);

uint8_t NFC_SetInListPassiveTarget(void);
uint8_t NFC_RecievePassiveTarget(uint8_t *uid, uint8_t *uid_len);


#define PN532_ACK_WAIT_TIME           (15)
#define PN532_HSU_READ_TIMEOUT		  (1000)


#endif /* HAL_NFC_NFC_INT_H_ */
