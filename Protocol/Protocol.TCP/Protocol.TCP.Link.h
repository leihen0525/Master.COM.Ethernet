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

int Protocol_TCP_Link_New_Syn_Node_To_ListenSyn_List(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Node_Type **PP_New_Link_Node);

int Protocol_TCP_Link_New_Listen_Node_To_Listen_List(
		Net_Protocol_TCP_Listen_List_Type *P_Listen_List,
		int Handle,
		Net_IP_Address_Type IP_Type,
		uint16_t PORT,
		uint32_t Backlog,
		uint32_t Window_Size,
		Net_Protocol_TCP_Listen_Node_Type **PP_New_Node);

int Protocol_TCP_Link_New_Link_Node_To_Link_List(
		Net_Protocol_TCP_Link_List_Type *P_Link_List,
		Net_Protocol_TCP_Link_Node_Type **PP_New_Link_Node);

int Protocol_TCP_Link_Syn_Node_Move_Accept_List_From_Listen(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Node_Type *P_Move_Node);

int Protocol_TCP_Link_Del_Link_Node_From_Listen_SynOrAccept_List(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type Link_Queue,
		Net_Protocol_TCP_Link_Node_Type *P_Del_Node,
		bool Free_Del_Node);

int Protocol_TCP_Link_Del_Link_Node_From_Link_List(
		Net_Protocol_TCP_Link_List_Type *P_Link_List,
		Net_Protocol_TCP_Link_Node_Type *P_Del_Node,
		bool Free_Del_Node);

int Protocol_TCP_Link_Find_Link_Node_From_Listen_SynOrAccept_List(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type Link_Queue,
		Net_IP_Address_Type IP_Type,
		uint8_t *P_Dest_IP,
		uint16_t Dest_PORT,
		Net_Protocol_TCP_Link_Node_Type **PP_Find_Node);

int Protocol_TCP_Link_Find_Link_Node_From_Link_List(
		Net_Protocol_TCP_Link_List_Type *P_Link_List,
		Net_IP_Address_Type IP_Type,
		uint8_t *P_Dest_IP,
		uint16_t Local_PORT,
		uint16_t Dest_PORT,
		Net_Protocol_TCP_Link_Node_Type **PP_Find_Node);

#endif /* PROTOCOL_TCP_LINK_H_ */
