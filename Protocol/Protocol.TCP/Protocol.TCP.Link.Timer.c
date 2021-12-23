/*
 * Protocol.TCP.Link.Timer.c
 *
 *  Created on: 2021年12月22日
 *      Author: Master.HE
 */
#include "Error.h"
#include "Define.h"
#include "API.h"

#include "Protocol.TCP.Link.Timer.h"

int Protocol_TCP_Link_Timer_Init(Net_Protocol_TCP_Link_Node_Type *P_Link_Node)
{
	int Handle;

	if(P_Link_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	Error_Args(Handle,Timer_Register(Protocol_TCP_Link_Timer_Retransmission_Handle, P_Link_Node))
	{
		goto Protocol_TCP_Link_Timer_Init_Exit1;
	}
	P_Link_Node->Timer.Retransmission.Handle=Handle;

	Error_Args(Handle,Timer_Register(Protocol_TCP_Link_Timer_Persistent_Handle, P_Link_Node))
	{
		goto Protocol_TCP_Link_Timer_Init_Exit2;
	}
	P_Link_Node->Timer.Persistent.Handle=Handle;

	Error_Args(Handle,Timer_Register(Protocol_TCP_Link_Timer_Keeplive_Handle, P_Link_Node))
	{
		goto Protocol_TCP_Link_Timer_Init_Exit3;
	}
	P_Link_Node->Timer.Keeplive.Handle=Handle;

	Error_Args(Handle,Timer_Register(Protocol_TCP_Link_Timer_TimeWait_Handle, P_Link_Node))
	{
		goto Protocol_TCP_Link_Timer_Init_Exit4;
	}
	P_Link_Node->Timer.TimeWait.Handle=Handle;

	return Error_OK;

Protocol_TCP_Link_Timer_Init_Exit4:
	Timer_Delete(P_Link_Node->Timer.Keeplive.Handle);
Protocol_TCP_Link_Timer_Init_Exit3:
	Timer_Delete(P_Link_Node->Timer.Persistent.Handle);
Protocol_TCP_Link_Timer_Init_Exit2:
	Timer_Delete(P_Link_Node->Timer.Retransmission.Handle);

Protocol_TCP_Link_Timer_Init_Exit1:

	P_Link_Node->Timer.Retransmission.Handle=-1;
	P_Link_Node->Timer.Persistent.Handle=-1;
	P_Link_Node->Timer.Keeplive.Handle=-1;
	P_Link_Node->Timer.TimeWait.Handle=-1;

	return Handle;
}
int Protocol_TCP_Link_Timer_DeInit(Net_Protocol_TCP_Link_Node_Type *P_Link_Node)
{
	if(P_Link_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	Timer_Delete(P_Link_Node->Timer.Retransmission.Handle);
	Timer_Delete(P_Link_Node->Timer.Persistent.Handle);
	Timer_Delete(P_Link_Node->Timer.Keeplive.Handle);
	Timer_Delete(P_Link_Node->Timer.TimeWait.Handle);

	return Error_OK;
}



void Protocol_TCP_Link_Timer_Retransmission_Handle(void *Args)
{
	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Args;
	if(P_Link_Node==Null)
	{
		return ;
	}
}
void Protocol_TCP_Link_Timer_Persistent_Handle(void *Args)
{
	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Args;
	if(P_Link_Node==Null)
	{
		return ;
	}
}
void Protocol_TCP_Link_Timer_Keeplive_Handle(void *Args)
{
	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Args;
	if(P_Link_Node==Null)
	{
		return ;
	}
}
void Protocol_TCP_Link_Timer_TimeWait_Handle(void *Args)
{
	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Args;
	if(P_Link_Node==Null)
	{
		return ;
	}
}
