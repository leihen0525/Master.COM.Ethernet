/*
 * Core.h
 *
 *  Created on: 2019年7月15日
 *      Author: Master.HE
 */

#ifndef CORE_H_
#define CORE_H_

#include "Master.Stdint.h"

#include "Core.Struct.h"

int __Sys_Net_Core_Init(void);
int Net_Core_Init(void);

__task void Net_Core_Task_TX(void *Args);
__task void Net_Core_Task_RX(void *Args);

int Net_Core_Tx_DATA_ARP(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *Target_MAC_Address,
		uint8_t *ARP_Packet,
		uint16_t Packet_Size);

int Net_Core_Tx_DATA_IPv4(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *Target_MAC_Address,
		uint8_t *IPv4_Packet_Heade,
		uint16_t IPv4_Packet_Heade_Size,
		uint8_t *DATA,
		uint16_t Size);

uint16_t Net_Core_CheckSum(uint16_t * buffer1, uint32_t size1,uint16_t * buffer2, uint32_t size2);

extern Net_Core_DATA_Type Net_Core_DATA;

#endif /* CORE_H_ */
