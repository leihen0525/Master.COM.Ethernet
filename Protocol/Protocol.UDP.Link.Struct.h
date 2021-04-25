/*
 * Protocol.UDP.Link.Struct.h
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_UDP_LINK_STRUCT_H_
#define PROTOCOL_UDP_LINK_STRUCT_H_

#include "Master.Stdint.h"
#include "Protocol.Enum.h"


typedef struct Net_Protocol_UDP_Link_Node
{

	struct
	{
		Net_Protocol_IP_Type Type;

		uint32_t Dest[16];
	}IP;

	struct
	{
		uint16_t Dest;

		uint16_t Local;
	}PORT;

	//Net_Protocol_UDP_Link_Node_DATA_Type DATA;


	struct Net_Protocol_UDP_Link_Node *NEXT;

}Net_Protocol_UDP_Link_Node_Type;

typedef struct
{
	Net_Protocol_UDP_Link_Node_Type *Head;
	Net_Protocol_UDP_Link_Node_Type *End;

}Net_Protocol_UDP_Link_DATA_Type;


#endif /* PROTOCOL_UDP_LINK_STRUCT_H_ */
