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
#include "Protocol.TCP.Link.Timer.h"


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




int Protocol_TCP_Link_New_Listen_Node_To_Listen_List(
		Net_Protocol_TCP_Listen_List_Type *P_Listen_List,
		int Handle,
		Net_IP_Address_Type IP_Type,
		uint16_t PORT,
		uint32_t Backlog,
		uint32_t Window_Size,
		Net_Protocol_TCP_Listen_Node_Type **PP_New_Node)//内部分配
{
	int Err;

	if(P_Listen_List==Null
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

	P_Listen_List->Count++;

	List_Add_Node_To_End(P_Listen_List->Head,P_Listen_List->End,NEXT,Temp_Add_Node);

	if(PP_New_Node!=Null)
	{
		*PP_New_Node=Temp_Add_Node;
	}

	return Error_OK;

//Protocol_TCP_Link_Listen_Add_Exit2:
//	Semaphore_Delete(Temp_Add_Node->Semaphore);
Protocol_TCP_Link_Listen_Add_Exit1:
	Mutex_Delete(Temp_Add_Node->Mutex);
Protocol_TCP_Link_Listen_Add_Exit:
	Memory_Free(Temp_Add_Node);
	return Err;
}


int Protocol_TCP_Link_New_Syn_Node_To_ListenSyn_List(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Node_Type **PP_New_Link_Node)
{
	int Err;
	if(P_Listen_Node==Null
	|| PP_New_Link_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Memory_Malloc(sizeof(Net_Protocol_TCP_Link_Node_Type));
	if(P_Link_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}


	Error_Args(Err,Protocol_TCP_Link_Timer_Init(P_Link_Node))
	{
		Memory_Free(P_Link_Node);
		return Err;
	}

	P_Listen_Node->Syn.Count++;
	P_Listen_Node->Count++;
	List_Add_Node_To_End(P_Listen_Node->Syn.Head,P_Listen_Node->Syn.End,NEXT,P_Link_Node);

	*PP_New_Link_Node=P_Link_Node;

	return Error_OK;
}
int Protocol_TCP_Link_New_Link_Node_To_Link_List(
		Net_Protocol_TCP_Link_List_Type *P_Link_List,
		Net_Protocol_TCP_Link_Node_Type **PP_New_Link_Node)
{
	if(P_Link_List==Null
	|| PP_New_Link_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Memory_Malloc(sizeof(Net_Protocol_TCP_Link_Node_Type));
	if(P_Link_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	//TODO 定时器初始化代码未添加

	P_Link_List->Count++;
	List_Add_Node_To_End(P_Link_List->Head,P_Link_List->End,NEXT,P_Link_Node);

	*PP_New_Link_Node=P_Link_Node;

	return Error_OK;
}
int Protocol_TCP_Link_Syn_Node_Move_Accept_List_From_Listen(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Node_Type *P_Move_Node)
{
	if(P_Listen_Node==Null
	|| P_Move_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	bool Del_OK=false;

	List_Del_Node_From_Pointer(Del,Net_Protocol_TCP_Link_Node_Type,P_Listen_Node->Syn.Head,P_Listen_Node->Syn.End,NEXT,P_Move_Node,Del_OK);
	if(Del_OK==true)
	{
		P_Listen_Node->Syn.Count--;

		P_Listen_Node->Accept.Count++;
		List_Add_Node_To_End(P_Listen_Node->Accept.Head,P_Listen_Node->Accept.End,NEXT,P_Move_Node);

		return Error_OK;
	}
	return Error_No_Find;
}

int Protocol_TCP_Link_Del_Link_Node_From_Listen_SynOrAccept_List(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type Link_Queue,
		Net_Protocol_TCP_Link_Node_Type *P_Del_Node,
		bool Free_Del_Node)
{
	if(P_Listen_Node==Null
	|| (Link_Queue!=Net_Protocol_TCP_Link_Queue_Listen_Syn && Link_Queue!=Net_Protocol_TCP_Link_Queue_Listen_Accept)
	|| P_Del_Node==Null
	|| Free_Del_Node>=bool_End)
	{
		return Error_Invalid_Parameter;
	}
	bool Del_OK=false;

	if(Link_Queue==Net_Protocol_TCP_Link_Queue_Listen_Syn)
	{
		List_Del_Node_From_Pointer(Del1,Net_Protocol_TCP_Link_Node_Type,P_Listen_Node->Syn.Head,P_Listen_Node->Syn.End,NEXT,P_Del_Node,Del_OK);

		if(Del_OK==true)
		{
			P_Listen_Node->Syn.Count--;
			P_Listen_Node->Count--;
		}
		else
		{
			return Error_No_Find;
		}
	}
	else//Net_Protocol_TCP_Link_Queue_Listen_Accept
	{
		List_Del_Node_From_Pointer(Del1,Net_Protocol_TCP_Link_Node_Type,P_Listen_Node->Accept.Head,P_Listen_Node->Accept.End,NEXT,P_Del_Node,Del_OK);

		if(Del_OK==true)
		{
			P_Listen_Node->Accept.Count--;
			P_Listen_Node->Count--;
		}
		else
		{
			return Error_No_Find;
		}
	}
	//TODO 释放代码未添加
	if(Free_Del_Node==true)
	{
		Memory_Free(P_Del_Node);
	}
	return Error_OK;
}

int Protocol_TCP_Link_Del_Link_Node_From_Link_List(
		Net_Protocol_TCP_Link_List_Type *P_Link_List,
		Net_Protocol_TCP_Link_Node_Type *P_Del_Node,
		bool Free_Del_Node)
{
	if(P_Link_List==Null
	|| P_Del_Node==Null
	|| Free_Del_Node>=bool_End)
	{
		return Error_Invalid_Parameter;
	}

	bool Del_OK=false;

	List_Del_Node_From_Pointer(Del1,Net_Protocol_TCP_Link_Node_Type,P_Link_List->Head,P_Link_List->End,NEXT,P_Del_Node,Del_OK);

	if(Del_OK==true)
	{
		P_Link_List->Count--;
	}
	else
	{
		return Error_No_Find;
	}

	//TODO 释放代码未添加
	if(Free_Del_Node==true)
	{
		Memory_Free(P_Del_Node);
	}
	return Error_OK;
}
int Protocol_TCP_Link_Find_Link_Node_From_Listen_SynOrAccept_List(
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type Link_Queue,
		Net_IP_Address_Type IP_Type,
		uint8_t *P_Dest_IP,
		uint16_t Dest_PORT,
		Net_Protocol_TCP_Link_Node_Type **PP_Find_Node)
{
	if(P_Listen_Node==Null
	|| (Link_Queue!=Net_Protocol_TCP_Link_Queue_Listen_Syn && Link_Queue!=Net_Protocol_TCP_Link_Queue_Listen_Accept)
	|| (IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Dest_IP==Null
	|| PP_Find_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_TCP_Link_Node_Type *Temp_Link_Node=Null;
	if(Link_Queue==Net_Protocol_TCP_Link_Queue_Listen_Syn)
	{
		Temp_Link_Node=P_Listen_Node->Syn.Head;
	}
	else//Net_Protocol_TCP_Link_Queue_Listen_Accept
	{
		Temp_Link_Node=P_Listen_Node->Accept.Head;
	}
	while(Temp_Link_Node!=Null)
	{
		if(Temp_Link_Node->IP_Type==IP_Type)
		{
			if(Temp_Link_Node->Dest_Info.PORT==Dest_PORT)
			{
				if(memcmp(Temp_Link_Node->Dest_Info.IP_Address,P_Dest_IP,IP_Type)==0)
				{
					*PP_Find_Node=Temp_Link_Node;
					return Error_OK;
				}
			}
		}
		Temp_Link_Node=Temp_Link_Node->NEXT;
	}
	return Error_No_Find;
}
int Protocol_TCP_Link_Find_Link_Node_From_Link_List(
		Net_Protocol_TCP_Link_List_Type *P_Link_List,
		Net_IP_Address_Type IP_Type,
		uint8_t *P_Dest_IP,
		uint16_t Local_PORT,
		uint16_t Dest_PORT,
		Net_Protocol_TCP_Link_Node_Type **PP_Find_Node)
{
	if(P_Link_List==Null
	|| (IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Dest_IP==Null
	|| PP_Find_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_TCP_Link_Node_Type *Temp_Link_Node=P_Link_List->Head;
	while(Temp_Link_Node!=Null)
	{
		if(Temp_Link_Node->IP_Type==IP_Type)
		{
			if(Temp_Link_Node->Local_Info.PORT==Local_PORT)
			{
				if(Temp_Link_Node->Dest_Info.PORT==Dest_PORT)
				{
					if(memcmp(Temp_Link_Node->Dest_Info.IP_Address,P_Dest_IP,IP_Type)==0)
					{
						*PP_Find_Node=Temp_Link_Node;
						return Error_OK;
					}
				}
			}

		}
		Temp_Link_Node=Temp_Link_Node->NEXT;
	}

	return Error_No_Find;
}
