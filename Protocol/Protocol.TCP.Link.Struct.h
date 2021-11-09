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


typedef struct Net_Protocol_TCP_Link_Node
{
	int Handle;

	bool Server;

	Net_IP_Address_Type IP_Type;


	struct
	{
		//重传时间
		uint32_t RTT;
	}Timer;

	struct
	{
		Net_Protocol_TCP_Link_Condition_Type Condition;

		uint8_t IP_Address[16];
		uint16_t PORT;
		uint32_t SEQ;

		uint32_t Window_Size;
	}Dest_Info;

	struct
	{
		Net_Protocol_TCP_Link_Condition_Type Condition;


		uint16_t PORT;
		uint32_t SEQ;
		uint32_t ACK;

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

//超时重传计时器
typedef struct Net_Protocol_TCP_Timer_Retransmission_Node
{
	int32_t TimeOut;
	uint32_t Count;

	uint16_t Flags;


	Net_Protocol_TCP_Link_Node_Type *Link_Node;

	struct Net_Protocol_TCP_Timer_Retransmission_Node *NEXT;
}Net_Protocol_TCP_Timer_Retransmission_Node_Type;

//坚持计时器
typedef struct Net_Protocol_TCP_Timer_Persistent_Node
{
	int32_t TimeOut;
	uint32_t Count;


	Net_Protocol_TCP_Link_Node_Type *Link_Node;

	struct Net_Protocol_TCP_Timer_Persistent_Node *NEXT;
}Net_Protocol_TCP_Timer_Persistent_Node_Type;

//保活计时器
typedef struct Net_Protocol_TCP_Timer_Keeplive_Node
{
	int32_t TimeOut;
	uint32_t Count;


	Net_Protocol_TCP_Link_Node_Type *Link_Node;

	struct Net_Protocol_TCP_Timer_Keeplive_Node *NEXT;
}Net_Protocol_TCP_Timer_Keeplive_Node_Type;

//时间等待计时器
typedef struct Net_Protocol_TCP_Timer_TimeWait_Node
{
	int32_t TimeOut;
	uint32_t Count;


	Net_Protocol_TCP_Link_Node_Type *Link_Node;

	struct Net_Protocol_TCP_Timer_TimeWait_Node *NEXT;
}Net_Protocol_TCP_Timer_TimeWait_Node_Type;

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

	struct
	{
		struct
		{
			uint32_t Count;
			Net_Protocol_TCP_Timer_Retransmission_Node_Type *Head;
			Net_Protocol_TCP_Timer_Retransmission_Node_Type *End;
		}Retransmission;

		struct
		{
			uint32_t Count;
			Net_Protocol_TCP_Timer_Persistent_Node_Type *Head;
			Net_Protocol_TCP_Timer_Persistent_Node_Type *End;
		}Persistent;

		struct
		{
			uint32_t Count;
			Net_Protocol_TCP_Timer_Keeplive_Node_Type *Head;
			Net_Protocol_TCP_Timer_Keeplive_Node_Type *End;
		}Keeplive;

		struct
		{
			uint32_t Count;
			Net_Protocol_TCP_Timer_TimeWait_Node_Type *Head;
			Net_Protocol_TCP_Timer_TimeWait_Node_Type *End;
		}TimeWait;
	}Timer;
}Net_Protocol_TCP_Link_DATA_Type;

#endif /* PROTOCOL_TCP_LINK_STRUCT_H_ */
