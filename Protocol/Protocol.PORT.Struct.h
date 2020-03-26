/*
 * Protocol.PORT.Struct.h
 *
 *  Created on: 2020年3月16日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_PORT_STRUCT_H_
#define PROTOCOL_PORT_STRUCT_H_

#include "Master.Stdint.h"
#include "Protocol.PORT.Define.h"

typedef struct Net_Protocol_PORT_Node_DATA
{
	uint16_t PORT;
	uint16_t Flag;
	struct Net_Protocol_PORT_Node_DATA *NEXT;

}Net_Protocol_PORT_Node_DATA_Type;

typedef struct
{
	uint32_t Count;

	Net_Protocol_PORT_Node_DATA_Type *Head;
	Net_Protocol_PORT_Node_DATA_Type *End;

}Net_Protocol_PORT_List_Node_Type;

typedef struct
{
	Net_Protocol_PORT_List_Node_Type Hash_List[Net_Protocol_PORT_Hash_Group_Number];

}Net_Protocol_PORT_DATA_Type;



#endif /* PROTOCOL_PORT_STRUCT_H_ */
