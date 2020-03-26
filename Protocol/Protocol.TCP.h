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

int Protocol_TCP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA);

int Protocol_TCP_ISN_10MS(Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA);

void Protocol_TCP_Handle_Rx(
		Net_Protocol_IP_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size);

#endif /* PROTOCOL_TCP_H_ */
