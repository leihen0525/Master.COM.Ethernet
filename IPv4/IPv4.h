/*
 * IPv4.h
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */

#ifndef IPV4_H_
#define IPV4_H_

#include "IPv4.Enum.h"
#include "Core/Core.Struct.h"
#include "IPv4.Struct.h"

int IPv4_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_IPv4_DATA_Type *P_IPv4_DATA);

void IPv4_Time_10MS(void *Args);
void IPv4_Time_1S(void *Args);

void IPv4_Handle_Tx(Net_Core_Device_Node_Type *P_Net_Node);
void IPv4_Handle_Rx(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Packet);

int IPv4_Tx_Add_Node(Net_IPv4_Tx_List_Type *P_List,Net_IPv4_Tx_Node_Type *P_Tx_Node);

int IPv4_Tx_Check_Pend_List(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address,uint8_t *Mac_Address);

int IPv4_Tx_Check_IPv4_Segment(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address);

int IPv4_Tx_Pend_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t Protocol,
		uint8_t *DEST_IPv4_Address,
		uint8_t *DATA,
		uint16_t Size,
		bool DATA_Free);

int IPv4_Tx_Send_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *Target_MAC_Address,
		uint8_t Protocol,
		uint8_t *DEST_IPv4_Address,
		uint8_t *DATA,
		uint16_t Size);

int IPv4_Rx_Packing_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Heade_Type *P_IPv4_Packet_Heade,
		uint8_t *In_DATA,
		uint16_t In_Size,
		uint8_t **Out_DATA,
		uint16_t *Out_Size);

int IPv4_Rx_Node_DATA_Add(
		Net_IPv4_Rx_Node_Data_Type **P_Rx_Node_DATA,
		uint16_t Address,
		uint8_t *DATA,
		uint16_t Size);

int IPv4_Rx_Node_Insert_DATA(
		Net_IPv4_Rx_Node_Type *P_Rx_Node,
		Net_IPv4_Packet_Heade_Flags_Type Flags,
		uint8_t *DATA,
		uint16_t Size);



int IPv4_Pseudo_Heade_Init(
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *P_SRC_IPv4_Address,
		uint8_t *P_DEST_IPv4_Address,

		uint8_t Protocol,

		uint16_t Length);

#endif /* IPV4_H_ */
