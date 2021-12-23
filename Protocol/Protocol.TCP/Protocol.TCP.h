/*
 * Protocol.TCP.h
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_H_
#define PROTOCOL_TCP_H_

#include "Master.Stdint.h"
#include "Net.Core.Enum.h"
#include "Core/Core.Struct.h"
#include "IPv4/IPv4.Struct.h"
#include "Protocol/Protocol.Enum.h"
#include "Net.IP.Enum.h"

int Protocol_TCP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA);

int Protocol_TCP_10MS(Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA);

int Protocol_TCP_Handle_Rx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *P_Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size);

int Protocol_TCP_Server_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IP_Address_Type IP_Type,
		uint8_t *P_Dest_IP,
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type Queue,
		Net_Protocol_TCP_Link_Node_Type *P_Link_Node,
		uint8_t *P_Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size);

/*
int Protocol_TCP_Find_LISTEN_Node(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IP_Address_Type IP_Type,
		uint8_t *IP_Src,
		Net_Protocol_TCP_Packet_Heade_Type *P_TCP_Packet_Heade,
		Net_Protocol_TCP_Listen_Node_Type **P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type *P_Queue,
		Net_Protocol_TCP_Link_Node_Type **P_Link_Node);
*/

int Protocol_TCP_Tx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *P_DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint32_t SEQ,
		uint32_t ACKN,
		uint16_t Flags,
		uint16_t Windows,
		uint16_t Urgent_Pointer,
		uint8_t *P_Option,
		uint16_t Option_Size,
		uint8_t *P_DATA,
		uint16_t DATA_Size);

#endif /* PROTOCOL_TCP_H_ */
