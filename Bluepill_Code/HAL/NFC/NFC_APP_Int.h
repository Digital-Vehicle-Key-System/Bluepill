/*
 * NFC_APP_Int.h
 *
 *  Created on: Apr 15, 2025
 *      Author: Fathe
 */

#ifndef INC_NFC_APP_INT_H_
#define INC_NFC_APP_INT_H_




// NFC Initialization return status
#define NFC_INIT_OK         		0
#define NFC_INIT_UNKNOWN_VERSION    1
#define NFC_INIT_SAM_FAILED         2
#define NFC_INIT_NOK         		3


uint8_t NFC_Init(void);
void NFC_Runnable(void);



#endif /* INC_NFC_APP_INT_H_ */
