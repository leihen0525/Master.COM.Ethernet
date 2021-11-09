/*
 * Protocol.TCP.Link.h
 *
 *  Created on: 2021年5月11日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_H_
#define PROTOCOL_TCP_LINK_H_

#include "Net.IP.Enum.h"
#include "Protocol.TCP.Link.Define.h"
#include "Protocol.TCP.Link.Struct.h"

int Protocol_TCP_Link_Init(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA);

int Protocol_TCP_Link_Add_Listen_Node_To_Listen_List(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		int Handle,
		Net_IP_Address_Type IP_Type,
		uint16_t PORT,
		uint32_t Backlog,
		uint32_t Window_Size,
		Net_Protocol_TCP_Listen_Node_Type **P_Add_Node_GET);

int Protocol_TCP_Link_Del_Link_Node_From_Listen_Syn_List(
		Net_Protocol_TCP_Listen_Node_Type *Listen_Node,
		Net_Protocol_TCP_Link_Node_Type *Del_Node,
		bool Free_Del_Node);

int Protocol_TCP_Link_Del_Link_Node_From_Listen_Accept_List(
		Net_Protocol_TCP_Listen_Node_Type *Listen_Node,
		Net_Protocol_TCP_Link_Node_Type *Del_Node,
		bool Free_Del_Node);

int Protocol_TCP_Link_Del_Link_Node_From_Link_List(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_TCP_Link_Node_Type *Del_Node,
		bool Free_Del_Node);


#endif /* PROTOCOL_TCP_LINK_H_ */
