
#ifndef CAN_INTERFACE_H_
#define CAN_INTERFACE_H_
#include <stdint.h>

void CAN1_TX(uint8_t copy_id, char size,uint8_t* copy_data);
void CAN1_RX(void);

void CAN_FilterConfig(void);

void CAN1_Init(void);















#endif
