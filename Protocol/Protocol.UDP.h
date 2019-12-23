/*
 * Protocol.UDP.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef PROTOCOL_UDP_H_
#define PROTOCOL_UDP_H_

#include "Master.Stdint.h"
#include "Core/Core.Struct.h"
#include "IPv4/IPv4.Struct.h"

int Protocol_UDP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_UDP_DATA_Type *P_IPv4_UDP_DATA);

void Protocol_UDP_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *IPv4_UDP_Packet,
		uint16_t IPv4_UDP_Packet_Size);


int Protocol_UDP_Tx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IPv4_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint8_t *DATA,
		uint16_t Size);

#endif /* PROTOCOL_UDP_H_ */
