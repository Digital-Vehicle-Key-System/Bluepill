
#include "CAN_interface.h"
#include "main.h"
#include <stdio.h>
#include <string.h>



extern CAN_HandleTypeDef hcan1;

void CAN1_Init(void)
{
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING );
	if ( HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING ) != HAL_OK )
	{
		Error_Handler();
	}
}

void CAN1_TX(uint8_t copy_id, char size,uint8_t* copy_data)
{
	CAN_TxHeaderTypeDef CAN_TX_Data ;

	uint32_t Local_u32MailBox ;
	CAN_TX_Data.StdId = copy_id;
	CAN_TX_Data.IDE = CAN_ID_STD ;
	CAN_TX_Data.RTR = CAN_RTR_DATA ;
	CAN_TX_Data.DLC = size ;

	if ( HAL_CAN_AddTxMessage(&hcan1 , &CAN_TX_Data ,copy_data ,&Local_u32MailBox ) != HAL_OK )
	{
		Error_Handler();
	}

	/* Wait as long as the message is pending */
	//	while(HAL_CAN_IsTxMessagePending(&hcan1, Local_u32MailBox) == 1);
	//

}


void CAN1_RX(void)
{
	CAN_RxHeaderTypeDef LOCAL_ReceivedMsg ;
	//char Local_RXMSG [50];

	uint8_t LOCAL_au8ReceivedData[9] = " ";

	while (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) == 0 );

	if(	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &LOCAL_ReceivedMsg, LOCAL_au8ReceivedData) != HAL_OK)
	{
		Error_Handler();
	}

//	sprintf(Local_RXMSG , "Message received : %s \r\n" ,LOCAL_au8ReceivedData );
//	HAL_UART_Transmit(&huart2,(uint8_t*) Local_RXMSG, strlen(Local_RXMSG), HAL_MAX_DELAY);

}


void CAN_FilterConfig(void)
{
	CAN_FilterTypeDef LOCAL_FilterConfig =
	{
			.FilterActivation = CAN_FILTER_ENABLE ,
			.FilterBank = 0 ,
			.FilterFIFOAssignment = CAN_RX_FIFO0 ,
			.FilterMaskIdHigh = 0 ,
			.FilterMaskIdLow = 0 ,
			.FilterMode = CAN_FILTERMODE_IDMASK ,
			.FilterScale = CAN_FILTERSCALE_32BIT
	};



	if(	HAL_CAN_ConfigFilter(&hcan1, &LOCAL_FilterConfig) != HAL_OK)
	{
		Error_Handler();
	}

}










void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//		sprintf(Local_TranmitMSG , "Message transmitted\r\n");
	//		HAL_UART_Transmit(&huart2, Local_TranmitMSG, strlen(Local_TranmitMSG), HAL_MAX_DELAY);

	//HAL_UART_Transmit(&huart2, "Message transmitted 0 \r\n", strlen("Message transmitted 0 \r\n"), HAL_MAX_DELAY);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//HAL_UART_Transmit(&huart2, "Message transmitted 1 \r\n ", strlen("Message transmitted 0 \r\n"), HAL_MAX_DELAY);

}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//HAL_UART_Transmit(&huart2, "Message transmitted 2 \r\n ", strlen("Message transmitted 0 \r\n"), HAL_MAX_DELAY);

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef LOCAL_ReceivedMsg ;
	char Local_RXMSG [50];

	uint8_t LOCAL_au8ReceivedData[9] = " ";



	if(	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &LOCAL_ReceivedMsg, LOCAL_au8ReceivedData) != HAL_OK)
	{
		Error_Handler();
	}




	//CAN_FILTERING_MESSAGE(LOCAL_ReceivedMsg.StdId,LOCAL_au8ReceivedData);

	char LOCAL_au8ReceivedDataChar[9] = " ";

	for(int i = 0; i < 8; i++)
	{
		LOCAL_au8ReceivedDataChar[i] = LOCAL_au8ReceivedData[i] + '0';
	}

//	sprintf(Local_RXMSG , "Message received : %s \r\n" ,LOCAL_au8ReceivedDataChar );
//	HAL_UART_Transmit(&huart2,(uint8_t*) Local_RXMSG, strlen(Local_RXMSG), HAL_MAX_DELAY);

//	if(LOCAL_ReceivedMsg.StdId == 0x12)
//	{
//		id =12;
//	}
//	else if(LOCAL_ReceivedMsg.StdId == 0x8)
//	{
//		id =8;
//	}
}

//
//HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//
//}











