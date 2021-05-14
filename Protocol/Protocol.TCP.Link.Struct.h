/*
 * Protocol.TCP.Link.Struct.h
 *
 *  Created on: 2021年5月11日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_STRUCT_H_
#define PROTOCOL_TCP_LINK_STRUCT_H_

#include "Master.Stdint.h"
#include "Protocol.Enum.h"


#include "Master.Stdint.h"
#include "Protocol.Enum.h"

#include "Protocol.TCP.Link.Enum.h"

#include "Net.API.Struct.h"
#include "Net.IP.Enum.h"
/*
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
		//Net_Protocol_IP_Type Type;

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
*/
typedef struct Net_Protocol_TCP_Link_Node
{
	int Handle;

	Net_IP_Address_Type IP_Type;

	struct
	{
		Net_Protocol_TCP_Link_Condition_Type Condition;

		uint8_t IP_Address[16];
		uint16_t PORT;
		uint32_t ISN;

		uint32_t Window_Size;
	}Dest_Info;

	struct
	{
		Net_Protocol_TCP_Link_Condition_Type Condition;


		uint16_t PORT;
		uint32_t ISN;

		uint32_t Window_Size;
	}Local_Info;

	struct Net_Protocol_TCP_Link_Node *NEXT;
}Net_Protocol_TCP_Link_Node_Type;

typedef struct Net_Protocol_TCP_Listen_Node
{
	int Handle;

	Net_Protocol_TCP_Link_Condition_Type Condition;

	Net_IP_Address_Type IP_Type;
	uint16_t PORT;

	uint32_t Window_Size;

	uint32_t Count;
	uint32_t Backlog;

	int Semaphore;
	int Mutex;
	struct
	{
		uint32_t Count;
		Net_Protocol_TCP_Link_Node_Type *Head;
		Net_Protocol_TCP_Link_Node_Type *End;
	}Syn;

	struct
	{
		uint32_t Count;
		Net_Protocol_TCP_Link_Node_Type *Head;
		Net_Protocol_TCP_Link_Node_Type *End;
	}Accept;


	struct Net_Protocol_TCP_Listen_Node *NEXT;
}Net_Protocol_TCP_Listen_Node_Type;



typedef struct
{
	struct
	{
		uint32_t Count;
		Net_Protocol_TCP_Listen_Node_Type *Head;
		Net_Protocol_TCP_Listen_Node_Type *End;
	}Listen;

	struct
	{
		uint32_t Count;
		Net_Protocol_TCP_Link_Node_Type *Head;
		Net_Protocol_TCP_Link_Node_Type *End;
	}Link;

}Net_Protocol_TCP_Link_DATA_Type;

#endif /* PROTOCOL_TCP_LINK_STRUCT_H_ */
