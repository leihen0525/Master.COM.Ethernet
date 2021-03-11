/*
 * Protocol.TCP.Link.h
 *
 *  Created on: 2020年3月24日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_H_
#define PROTOCOL_TCP_LINK_H_

#include "Protocol.TCP.Link.Struct.h"

int Protocol_TCP_Link_Init(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA);

int Protocol_TCP_Link_Add(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_IP_Type IP_Type,
		uint8_t *Dest_IP_Address,
		uint16_t Dest_PORT,
		uint16_t Local_PORT,
		Net_Protocol_TCP_Link_Node_DATA_Type **P_DATA);

int Protocol_TCP_Link_Find(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_IP_Type IP_Type,
		uint8_t *Dest_IP_Address,
		uint16_t Dest_PORT,
		uint16_t Local_PORT,
		Net_Protocol_TCP_Link_Node_DATA_Type **P_DATA);

int Protocol_TCP_Link_Del(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_IP_Type IP_Type,
		uint8_t *Dest_IP_Address,
		uint16_t Dest_PORT,
		uint16_t Local_PORT,
		bool Internal_Del_DATA,
		Net_Protocol_TCP_Link_Node_DATA_Type **P_DATA);

#endif /* PROTOCOL_TCP_LINK_H_ */
