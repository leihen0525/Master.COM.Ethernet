/*
 * Protocol.TCP.Link.c
 *
 *  Created on: 2021年5月11日
 *      Author: Master.HE
 */
#include "Define.h"
#include "List.h"
#include "Error.h"
#include "API.h"
#include "Protocol.TCP.Link.h"

int Protocol_TCP_Link_Init(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA)
{
	if(P_Protocol_TCP_Link_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}

	P_Protocol_TCP_Link_DATA->Listen.Count=0;
	P_Protocol_TCP_Link_DATA->Listen.Head=Null;
	P_Protocol_TCP_Link_DATA->Listen.End=Null;

	P_Protocol_TCP_Link_DATA->Link.Count=0;
	P_Protocol_TCP_Link_DATA->Link.Head=Null;
	P_Protocol_TCP_Link_DATA->Link.End=Null;

	return Error_OK;
}

int Protocol_TCP_Link_Listen_Add(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		int Handle,
		Net_IP_Address_Type IP_Type,
		uint16_t PORT,
		uint32_t Backlog,
		uint32_t Window_Size,
		Net_Protocol_TCP_Listen_Node_Type **P_Add_Node_GET)
{
	int Err;

	if(P_Protocol_TCP_Link_DATA==Null
	|| Handle<Valid_Handle
	|| (IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| Backlog==0
	|| Window_Size==0)
	{
		return Error_Invalid_Parameter;
	}
	Net_Protocol_TCP_Listen_Node_Type *Temp_Add_Node=Memory_Malloc(sizeof(Net_Protocol_TCP_Listen_Node_Type));

	if(Temp_Add_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}
	//申请一个互斥信号

	Error_Args(Temp_Add_Node->Mutex,Mutex_Create(Null,Event_Queue_Option_Priority))
	{
		Err=Temp_Add_Node->Mutex;
		goto Protocol_TCP_Link_Listen_Add_Exit;
	}

	Error_Args(Temp_Add_Node->Semaphore,Semaphore_Create(Null,0,Backlog,Event_Queue_Option_Priority))
	{
		Err=Temp_Add_Node->Semaphore;
		goto Protocol_TCP_Link_Listen_Add_Exit1;
	}


	Temp_Add_Node->Handle=Handle;
	Temp_Add_Node->Condition=Net_Protocol_TCP_Link_Condition_LISTEN;
	Temp_Add_Node->IP_Type=IP_Type;
	Temp_Add_Node->PORT=PORT;
	Temp_Add_Node->Count=0;
	Temp_Add_Node->Backlog=Backlog;

	Temp_Add_Node->Window_Size=Window_Size;

	Temp_Add_Node->Syn.Count=0;
	Temp_Add_Node->Syn.Head=Null;
	Temp_Add_Node->Syn.End=Null;

	Temp_Add_Node->Accept.Count=0;
	Temp_Add_Node->Accept.Head=Null;
	Temp_Add_Node->Accept.End=Null;

	P_Protocol_TCP_Link_DATA->Listen.Count++;

	List_Add_Node_To_End(P_Protocol_TCP_Link_DATA->Listen.Head,P_Protocol_TCP_Link_DATA->Listen.End,NEXT,Temp_Add_Node);

	if(P_Add_Node_GET!=Null)
	{
		*P_Add_Node_GET=Temp_Add_Node;
	}

	return Error_OK;

Protocol_TCP_Link_Listen_Add_Exit2:
	Semaphore_Delete(Temp_Add_Node->Semaphore);
Protocol_TCP_Link_Listen_Add_Exit1:
	Mutex_Delete(Temp_Add_Node->Mutex);
Protocol_TCP_Link_Listen_Add_Exit:
	Memory_Free(Temp_Add_Node);
	return Err;
}
