/*
 * IPv4.ARP.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef IPV4_ARP_H_
#define IPV4_ARP_H_

#include "Core/Core.Struct.h"
#include "IPv4.ARP.Enum.h"

int IPv4_ARP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_IPv4_ARP_DATA_Type *P_IPv4_ARP_DATA);

int IPv4_ARP_Cache_Add(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address,uint8_t *MAC_Address);
int IPv4_ARP_Cache_Find_Mac_Address(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address,uint8_t *MAC_Address);

void IPv4_ARP_Time_1S(void *Args);

void IPv4_ARP_Handle_Rx(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *ARP_Packet);

int IPv4_ARP_Tx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_MAC_Address,
		Net_IPv4_ARP_Opcode_Type Opcode,
		uint8_t *Target_MAC_Address,
		uint8_t *Target_IPv4_Address);

int IPv4_ARP_Reqst(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address);


#endif /* IPV4_ARP_H_ */
