/*
 * Protocol.TCP.Link.Struct.h
 *
 *  Created on: 2020年3月24日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_STRUCT_H_
#define PROTOCOL_TCP_LINK_STRUCT_H_

#include "Master.Stdint.h"
#include "Protocol.Enum.h"

#include "Protocol.TCP.Link.Enum.h"

typedef union
{
	uint16_t DATA;
	struct
	{
		uint16_t Server							:1;
	};
}Net_Protocol_TCP_Link_Node_Flag_Type;


typedef struct
{
	Net_Protocol_TCP_Link_Node_Flag_Type Flag;
//	uint32_t ISN;
//	Net_Protocol_TCP_Link_Condition_Type Condition;



}Net_Protocol_TCP_Link_Node_DATA_Type;


typedef struct Net_Protocol_TCP_Link_Node
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

	Net_Protocol_TCP_Link_Node_DATA_Type DATA;


	struct Net_Protocol_TCP_Link_Node *NEXT;

}Net_Protocol_TCP_Link_Node_Type;



typedef struct
{
	Net_Protocol_TCP_Link_Node_Type *Head;
	Net_Protocol_TCP_Link_Node_Type *End;

}Net_Protocol_TCP_Link_DATA_Type;



#endif /* PROTOCOL_TCP_LINK_STRUCT_H_ */
