/*
 * Protocol.TCP.Link.Timer.Struct.h
 *
 *  Created on: 2021年12月22日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_TIMER_STRUCT_H_
#define PROTOCOL_TCP_LINK_TIMER_STRUCT_H_

#include "Master.Stdint.h"

//超时重传计时器
typedef struct //Net_Protocol_TCP_Timer_Retransmission_Node
{
	int Handle;

	int32_t TimeOut;
	uint32_t Count;

	uint16_t Flags;


	//Net_Protocol_TCP_Link_Node_Type *Link_Node;

	//struct Net_Protocol_TCP_Timer_Retransmission_Node *NEXT;
}Net_Protocol_TCP_Timer_Retransmission_Node_Type;

//坚持计时器
typedef struct //Net_Protocol_TCP_Timer_Persistent_Node
{
	int Handle;

	int32_t TimeOut;
	uint32_t Count;


	//Net_Protocol_TCP_Link_Node_Type *Link_Node;

	//struct Net_Protocol_TCP_Timer_Persistent_Node *NEXT;
}Net_Protocol_TCP_Timer_Persistent_Node_Type;

//保活计时器
typedef struct //Net_Protocol_TCP_Timer_Keeplive_Node
{
	int Handle;

	int32_t TimeOut;
	uint32_t Count;


	//Net_Protocol_TCP_Link_Node_Type *Link_Node;

	//struct Net_Protocol_TCP_Timer_Keeplive_Node *NEXT;
}Net_Protocol_TCP_Timer_Keeplive_Node_Type;

//时间等待计时器
typedef struct //Net_Protocol_TCP_Timer_TimeWait_Node
{
	int Handle;

	int32_t TimeOut;
	uint32_t Count;


	//Net_Protocol_TCP_Link_Node_Type *Link_Node;

	//struct Net_Protocol_TCP_Timer_TimeWait_Node *NEXT;
}Net_Protocol_TCP_Timer_TimeWait_Node_Type;

#endif /* PROTOCOL_TCP_LINK_TIMER_STRUCT_H_ */
