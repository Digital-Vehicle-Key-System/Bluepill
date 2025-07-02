/*
 * NFC_Prog.c
 *
 *  Created on: Jan 13, 2025
 *      Author: Fathy
 */

#include "NFC_Int.h"
#include <string.h>

// UART handle
UART_HandleTypeDef *huart_nfc;

// Initialize NFC module
void NFC_HAL_Init(UART_HandleTypeDef *huart) {
    huart_nfc = huart;

    // NFC wake up
    NFC_WakeUpFrame();
}

// Get firmware version of the PN532
uint8_t NFC_GetFirmwareVersion(uint8_t *version, uint8_t* version_len) {
    uint8_t cmd = PN532_CMD_GETFIRMWAREVERSION;
    uint8_t response[6]={0};
    uint8_t response_len=0;

    // Send command
    NFC_SendCommand(cmd, NULL, 0);

    // Read response
    NFC_ReadResponse(response, &response_len, PN532_HSU_READ_TIMEOUT);       // TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
    // Check if response is valid
    if (response[0] == PN532_CMD_GETFIRMWAREVERSION_RESPONSE) {
    	*version_len = response_len-1;
        memcpy(version, &response[1], response_len-1);
        return NFC_OK;
    }

    return NFC_ERROR;
}

// Configure SAM (Secure Access Module)
uint8_t NFC_SAMConfiguration() {
    uint8_t cmd = PN532_CMD_SAMCONFIGURATION;
    uint8_t data[] = {0x01, 0x00, 0x01};  // Normal mode, timeout 1s (0x14), IRQ // TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
    uint8_t response[3]={0};
    uint8_t response_len=0;

    // Send command
    NFC_SendCommand(cmd, data, 3);

    // Read response
    NFC_ReadResponse(response, &response_len, PN532_HSU_READ_TIMEOUT);		// TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

    // Check if response is valid
    if (response[0] == PN532_CMD_SAMCONFIGURATION_RESPONSE) {
        return NFC_OK;
    }

    return NFC_ERROR;
}

// List passive targets (NFC tags)
uint8_t NFC_InListPassiveTarget(uint8_t *uid, uint8_t *uid_len) {
    uint8_t cmd = PN532_CMD_INLISTPASSIVETARGET;
    uint8_t data[] = {0x01, 0x00};  // Max 1 target, 106 kbps
    uint8_t response[20]={0};
    uint8_t response_len=0;

    // Send command
    NFC_SendCommand(cmd, data, 2);

    // Read response
    NFC_ReadResponse(response, &response_len, PN532_HSU_READ_TIMEOUT);							// TEASTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

    // Check if response is valid
    if (response[0] == PN532_CMD_INLISTPASSIVETARGET_RESPONSE) {
        *uid_len = response[6];  // UID length
        memcpy(uid, &response[7], *uid_len);  // Copy UID
        return NFC_OK;
    }

    return NFC_ERROR;
}

uint8_t NFC_SetInListPassiveTarget(void)
{
	 uint8_t cmd = PN532_CMD_INLISTPASSIVETARGET;
	 uint8_t data[] = {0x01, 0x00};  // Max 1 target, 106 kbps

	 // Send command
	 return NFC_SendCommand(cmd, data, 2);
}

uint8_t NFC_RecievePassiveTarget(uint8_t *uid, uint8_t *uid_len) {
    uint8_t response[20]={0};
    uint8_t response_len=0;

    // Read response
    NFC_ReadResponse(response, &response_len, PN532_HSU_READ_TIMEOUT);							// TEASTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

    // Check if response is valid
    if (response[0] == PN532_CMD_INLISTPASSIVETARGET_RESPONSE) {
        *uid_len = response[6];  // UID length
        memcpy(uid, &response[7], *uid_len);  // Copy UID
        return NFC_OK;
    }

    return NFC_ERROR;
}


/*
// Exchange data with the NFC tag
uint8_t NFC_InDataExchange(uint8_t *data, uint8_t len, uint8_t *response) {
    uint8_t cmd = PN532_CMD_INDATAEXCHANGE;
    uint8_t tx_buffer[64];
    uint8_t rx_buffer[64];

    // Prepare the command frame
    tx_buffer[0] = cmd;
    memcpy(&tx_buffer[1], data, len);

    // Send command
    NFC_SendCommand(cmd, data, len);

    // Read response
    NFC_ReadResponse(response, 64);

    return NFC_OK;
}
*/

/**********************************  Helper Functions  ***************************************/

// Send a command to the PN532 over UART
uint8_t NFC_SendCommand(uint8_t cmd, uint8_t *data, uint8_t len) {
    uint8_t tx_buffer[64];
    uint8_t index = 0;

    // Build the UART frame
    tx_buffer[index++] = PN532_PREAMBLE;
    tx_buffer[index++] = PN532_STARTCODE1;
    tx_buffer[index++] = PN532_STARTCODE2;
    tx_buffer[index++] = len + 2;  // Length of data + command + TFI Frame Identifier
    tx_buffer[index++] = ~(len + 2) + 1;  // Length checksum
    tx_buffer[index++] = TFI_HOST_TO_PN532; // TFI Frame Identifier
    tx_buffer[index++] = cmd;  // Command
    for (uint8_t i = 0; i < len; i++) {
        tx_buffer[index++] = data[i];  // Data
    }
    tx_buffer[index++] = NFC_CalculateChecksum(&tx_buffer[5], len + 2);  // Data checksum
    tx_buffer[index++] = PN532_POSTAMBLE;

    // Send the frame over UART
    HAL_UART_Transmit(huart_nfc, tx_buffer, index, HAL_MAX_DELAY);

    // check for ACK
    return NFC_CheckACK();
}

// Read response from the PN532 over UART
uint8_t NFC_ReadResponse(uint8_t *response, uint8_t *response_len, uint32_t timeout) {
    uint8_t rx_buffer[64]={0};
    uint8_t frame_len=0;

    // check for NULL Pointer
    if(response==(void*)0 ||response_len==(void*)0 ){

    	return NFC_ERROR;
    }

    // --- Step 1: Read Preamble (0x00) + Start Codes (0x00, 0xFF) ---
    HAL_UART_Receive(huart_nfc, &rx_buffer[0], 3, timeout);  // Preamble (0x00) & Start codes (0x00, 0xFF)

    if (rx_buffer[0] != PN532_PREAMBLE ||
    		rx_buffer[1] != PN532_STARTCODE1 ||
            rx_buffer[2] != PN532_STARTCODE2) {
            return NFC_ERROR;  // Invalid frame start
    }

    // --- Step 2: Read LEN (length) + LCS (checksum) ---
    HAL_UART_Receive(huart_nfc, &rx_buffer[3], 2, timeout);
    frame_len = rx_buffer[3];  // LEN = TFI + PD0..PDn (excludes DCS/postamble)
    uint8_t lcs = rx_buffer[4];
    HAL_UART_Receive(huart_nfc, &rx_buffer[5], frame_len + 2, timeout); // +1 for DCS, +1 for postamble

    // Verify LEN + LCS = 0x00 (8-bit arithmetic)
    if ((uint8_t)(frame_len + lcs) != 0x00) {
        return NFC_ERROR;  // Corrupted length
    }

    // --- Step 3: Read TFI + Data (PD0..PDn) + DCS + Postamble ---
    //HAL_UART_Receive(huart_nfc, &rx_buffer[5], frame_len + 2, timeout); // +1 for DCS, +1 for postamble

    // --- Step 4: Verify Data Checksum (DCS) ---
    uint8_t checksum = NFC_CalculateChecksum(&rx_buffer[5], frame_len);  // Length + data + command
    if ((uint8_t)checksum != rx_buffer[frame_len + 5]) {
        return NFC_ERROR;
    }

    // --- Step 5: Verify Postamble (0x00) ---
    if (rx_buffer[5 + frame_len + 1] != PN532_POSTAMBLE) {
        return NFC_ERROR;  // Invalid postamble
    }

    // --- Step 6: Extract Data (skip TFI byte) ---
    *response_len = frame_len - 1;  // Exclude TFI (D5h)
    memcpy(response, &rx_buffer[6], *response_len);  // Copy PD0..PDn

    return NFC_OK;

}


// Sending wake up Frame for HSU
void NFC_WakeUpFrame (void)
{
	uint8_t WakeUpFrame[5] = {0x55,0x55,0x00,0x00,0x00};

	HAL_UART_Transmit(huart_nfc, WakeUpFrame, 5, HAL_MAX_DELAY);

}

// Calculate checksum for PN532 UART frame
uint8_t NFC_CalculateChecksum(uint8_t *data, uint8_t len) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < len; i++) {
        checksum += data[i];
    }
    return ~checksum + 1;
}


void NFC_SendACK(void)
{
	uint8_t ACK_Frame[6] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};

	HAL_UART_Transmit(huart_nfc, ACK_Frame, sizeof(ACK_Frame), HAL_MAX_DELAY);

}

uint8_t NFC_CheckACK(void)
{
	uint8_t ACK_Frame[6]  = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
	uint8_t NACK_Frame[6] = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};
	uint8_t responce_Frame[6];
	uint8_t status=NFC_ERROR;

	HAL_UART_Receive(huart_nfc, responce_Frame, sizeof(responce_Frame), PN532_ACK_WAIT_TIME);

	if(memcmp(responce_Frame, ACK_Frame, sizeof(responce_Frame)) == 0)
	{
		status=NFC_ACK;
	}
	else if (memcmp(responce_Frame, NACK_Frame, sizeof(responce_Frame)) == 0)
	{
		status=NFC_NACK;
	}

	return status;
}
