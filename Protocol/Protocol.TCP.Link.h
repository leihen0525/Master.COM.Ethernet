/*
 * Protocol.TCP.Link.h
 *
 *  Created on: 2021年5月11日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_H_
#define PROTOCOL_TCP_LINK_H_

#include "Net.IP.Enum.h"
#include "Protocol.TCP.Link.Struct.h"

int Protocol_TCP_Link_Init(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA);

int Protocol_TCP_Link_Listen_Add(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		int Handle,
		Net_IP_Address_Type IP_Type,
		uint16_t PORT,
		uint32_t Backlog,
		uint32_t Window_Size,
		Net_Protocol_TCP_Listen_Node_Type **P_Add_Node_GET);

#endif /* PROTOCOL_TCP_LINK_H_ */
