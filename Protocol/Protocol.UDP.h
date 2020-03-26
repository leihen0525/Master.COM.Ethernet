/*
 * Protocol.UDP.h
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_UDP_H_
#define PROTOCOL_UDP_H_

#include "Master.Stdint.h"
#include "Core/Core.Struct.h"
#include "IPv4/IPv4.Struct.h"
#include "Protocol.Enum.h"

int Protocol_UDP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_UDP_DATA_Type *P_Protocol_UDP_DATA);

void Protocol_UDP_Handle_Rx(
		Net_Protocol_IP_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *Protocol_UDP_Packet,
		uint16_t Protocol_UDP_Packet_Size);


int Protocol_UDP_Tx(
		Net_Protocol_IP_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint8_t *DATA,
		uint16_t Size);

#endif /* PROTOCOL_UDP_H_ */
