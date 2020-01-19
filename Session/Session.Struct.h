/*
 * Session.Struct.h
 *
 *  Created on: 2019Äê10ÔÂ15ÈÕ
 *      Author: Master.HE
 */

#ifndef SESSION_STRUCT_H_
#define SESSION_STRUCT_H_

#include "Master.Stdint.h"

#include "Session.Enum.h"

#include "Net.Core.Struct.h"

#include "Protocol/Protocol.Struct.h"

typedef union
{
	int DATA;
	struct
	{
		//int Direction				:1;
		int Handle					:29;
		int Protocol				:2;
		int 						:1;
	};
}Net_Session_Handle_Type;



typedef struct Net_Session_NODE_DATA
{
	Net_Session_Handle_Type Handle;

	struct Net_Session_NODE_DATA *NEXT;

	Net_Protocol_Node_DATA_Type *Protocol_Node_DATA;

	Net_Core_Address_DATA_Type *Core_Address_DATA;


}Net_Session_Node_DATA_Type;

typedef struct
{
	Net_Session_Node_DATA_Type *Head;
	Net_Session_Node_DATA_Type *End;
}Net_Session_Node_List_Type;

typedef struct
{
	Net_Session_Node_List_Type IP_List;
	Net_Session_Node_List_Type ICMP_List;

	Net_Session_Node_List_Type TCP_List;
	Net_Session_Node_List_Type UDP_List;

//	struct
//	{
//		Net_Session_Node_List_Type IN;
//		Net_Session_Node_List_Type OUT;
//	}TCP;
//
//	struct
//	{
//		Net_Session_Node_List_Type IN;
//		Net_Session_Node_List_Type OUT;
//	}UDP;


}Net_Session_DATA_Type;


#endif /* SESSION_STRUCT_H_ */
