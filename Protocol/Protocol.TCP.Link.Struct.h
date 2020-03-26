/*
 * Protocol.TCP.Link.Struct.h
 *
 *  Created on: 2020年3月24日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_STRUCT_H_
#define PROTOCOL_TCP_LINK_STRUCT_H_

#include "Master.Stdint.h"

#include "Protocol.TCP.Link.Enum.h"


typedef struct Net_Protocol_TCP_Link_Node_DATA
{
	uint32_t Dest_IPv4;

	uint16_t Dest_PORT;

	uint16_t Local_PORT;

	uint32_t ISN;

	Net_Protocol_TCP_Link_Condition_Type Condition;



	struct Net_Protocol_TCP_Link_Node_DATA *NEXT;

}Net_Protocol_TCP_Link_Node_DATA_Type;


typedef struct
{
	Net_Protocol_TCP_Link_Node_DATA_Type *Head;
	Net_Protocol_TCP_Link_Node_DATA_Type *End;
}Net_Protocol_TCP_Link_Node_List_Type;

typedef struct
{
	Net_Protocol_TCP_Link_Node_List_Type In;
	Net_Protocol_TCP_Link_Node_List_Type Out;
}Net_Protocol_TCP_Link_DATA_Type;



#endif /* PROTOCOL_TCP_LINK_STRUCT_H_ */
