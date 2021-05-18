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
#include "Protocol.Enum.h"
#include "Net.IP.Enum.h"

int Protocol_TCP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA);

int Protocol_TCP_10MS(Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA);

void Protocol_TCP_Handle_Rx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size);

int Protocol_TCP_Tx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint32_t SEQ,
		uint32_t ACKN,
		Net_Protocol_TCP_Packet_Heade_Flags_Type Flags,
		uint16_t Windows,
		uint16_t Urgent_Pointer,
		uint8_t *Option,
		uint16_t Option_Size,
		uint8_t *DATA,
		uint16_t DATA_Size);

#endif /* PROTOCOL_TCP_H_ */
