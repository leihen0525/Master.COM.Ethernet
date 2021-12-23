/*
 * API.TCP.Struct.h
 *
 *  Created on: 2021年4月25日
 *      Author: Master.HE
 */

#ifndef API_TCP_STRUCT_H_
#define API_TCP_STRUCT_H_

#include "Master.Stdint.h"
#include "Core/Core.Struct.h"
#include "API.TCP.Enum.h"
#include "Net.API.Struct.h"

#include "Protocol/Protocol.TCP/Protocol.TCP.Link.Struct.h"

typedef struct Net_API_TCP_Node
{
	int Handle;
	Net_API_TCP_Handle_Type Type;


	//设备
	Net_Core_Device_Node_Type *Device_Node;

	uint32_t Rx_Buffer_Max_Length;
	//uint32_t Backlog;


	//Bind
	struct
	{
		bool SET;
		Net_API_IP_Information_Type IP_Info;
	}Bind;


	//
	union
	{
		void *Node;
		Net_Protocol_TCP_Listen_Node_Type *Listen_Node;
		Net_Protocol_TCP_Link_Node_Type *Link_Node;
	};


	struct Net_API_TCP_Node *NEXT;
}Net_API_TCP_Node_Type;

typedef struct
{
	Net_API_TCP_Node_Type *Head;
	Net_API_TCP_Node_Type *End;
}Net_API_TCP_DATA_Type;

#endif /* API_TCP_STRUCT_H_ */
