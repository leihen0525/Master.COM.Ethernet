/*
 * Protocol.UDP.c
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */
#include <string.h>
#include "Define.h"
#include "List.h"
#include "Ring.Queue.h"
#include "Error.h"
#include "API.h"

#include "Net.Error.h"
#include "Core/Core.Define.h"
#include "Core/Core.h"

#include "IPv4/IPv4.Enum.h"
#include "IPv4/IPv4.h"

#include "Protocol.UDP.Struct.h"

#include "Protocol.UDP.h"

#include "Protocol.PORT.h"

#include "API/API.UDP.h"

int Protocol_UDP_Init(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Protocol_UDP_DATA_Type *P_Protocol_UDP_DATA)
{
	if(P_Net_Node==Null || P_Protocol_UDP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	int Err;

	if((Err=Protocol_PORT_Init(&P_Protocol_UDP_DATA->PORT_DATA))!=Error_OK)
	{
		return Err;
	}

	return Error_OK;
}

int Protocol_UDP_Handle_Rx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *Protocol_UDP_Packet,
		uint16_t Protocol_UDP_Packet_Size)
{
	int Err=Error_OK;

	if((IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Net_Node==Null
	|| P_Pseudo_Heade==Null
	|| Protocol_UDP_Packet==Null
	|| Protocol_UDP_Packet_Size==0
	|| Protocol_UDP_Packet_Size<sizeof(Net_Protocol_UDP_Packet_Heade_Type))
	{
		return Error_Invalid_Parameter;
	}

	if(Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)Protocol_UDP_Packet,
			Protocol_UDP_Packet_Size)!=UINT16_REVERSE(0x0000))
	{
		return Net_Error_CheckSum_Fail;
	}
	Net_Protocol_UDP_Packet_Heade_Type *P_Protocol_UDP_Packet_Heade=(Net_Protocol_UDP_Packet_Heade_Type *)Protocol_UDP_Packet;

	if(Protocol_UDP_Packet_Size!=UINT16_REVERSE(P_Protocol_UDP_Packet_Heade->Total_Length))
	{
		return Net_Error_Packet_Size_No_Match;
	}

	uint16_t PORT=UINT16_REVERSE(P_Protocol_UDP_Packet_Heade->DEST_PORT);

	Net_API_UDP_Node_Type *P_UDP_Node=Null;

	List_Find_Node_From_Symbol_2And(Net_API_UDP_DATA.Head,NEXT,Bind.SET,true,Bind.IP_Info.PORT,PORT,P_UDP_Node);

	if(P_UDP_Node==Null)
	{
		return Net_Error_No_Bind;
	}

	Net_API_UDP_Buff_Type *P_UDP_Buff;

	P_UDP_Buff=Memory_Malloc(sizeof(Net_API_UDP_Buff_Type));

	if(P_UDP_Buff==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	int Len=Protocol_UDP_Packet_Size-sizeof(Net_Protocol_UDP_Packet_Heade_Type);

	P_UDP_Buff->DATA=Memory_Malloc(Len);
	//无法分配更多空间 只能丢弃数据包
	if(P_UDP_Buff->DATA==Null)
	{
		Err=Error_Allocation_Memory_Failed;

		goto Protocol_UDP_Handle_Rx_Exit_1;
	}

	memcpy(P_UDP_Buff->DATA,&Protocol_UDP_Packet[sizeof(Net_Protocol_UDP_Packet_Heade_Type)],Len);
	P_UDP_Buff->Index=0;
	P_UDP_Buff->Size=Len;
	P_UDP_Buff->IP_Info.PORT=UINT16_REVERSE(P_Protocol_UDP_Packet_Heade->SRC_PORT);

	if(IP_Type==Net_IP_Address_IPv4)
	{
		memcpy(P_UDP_Buff->IP_Info.IP_Address,P_Pseudo_Heade->SRC_IPv4_Address,Net_IPv4_Adress_Size_Byte);

	}
	else if(IP_Type==Net_IP_Address_IPv6)
	{
		//TODO IPv6 未实现
	}
	P_UDP_Buff->IP_Info.IP_Type=IP_Type;

	Error_NoArgs(Err,Mutex_Wait(P_UDP_Node->Rx_Buffer.Mutex,-1))
	{
		goto Protocol_UDP_Handle_Rx_Exit_2;
	}

	if(P_UDP_Node->Rx_Buffer.Buff.Size<P_UDP_Node->Rx_Buffer.Buff.Max_Length)
	{
		List_Add_Node_To_End(P_UDP_Node->Rx_Buffer.Buff.Head,P_UDP_Node->Rx_Buffer.Buff.End,NEXT,P_UDP_Buff);
		P_UDP_Node->Rx_Buffer.Buff.Count++;
		P_UDP_Node->Rx_Buffer.Buff.Size+=Len;
	}
	else
	{
		P_UDP_Node->Rx_Buffer.Buff.OverFlow=true;
		Err=Net_Error_Buff_OverFlow;
		goto Protocol_UDP_Handle_Rx_Exit_3;
	}

	Error_NoArgs(Err,Semaphore_Release(P_UDP_Node->Rx_Buffer.Semaphore,1,Null))
	{
		goto Protocol_UDP_Handle_Rx_Exit_3;
	}

	Error_NoArgs(Err,Mutex_Release(P_UDP_Node->Rx_Buffer.Mutex))
	{
		goto Protocol_UDP_Handle_Rx_Exit_3;
	}

	return Err;


Protocol_UDP_Handle_Rx_Exit_3:
	Mutex_Release(P_UDP_Node->Rx_Buffer.Mutex);
Protocol_UDP_Handle_Rx_Exit_2:
	Memory_Free(P_UDP_Buff->DATA);
Protocol_UDP_Handle_Rx_Exit_1:
	Memory_Free(P_UDP_Buff);

	return Err;
}

int Protocol_UDP_Tx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint8_t *DATA,
		uint16_t Size)
{
	if((IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Net_Node==Null
	|| DEST_IP_Address==Null
	|| (Size!=0 && DATA==Null))
	{
		return Error_Invalid_Parameter;
	}
	int Err=Error_OK;

	Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade=Memory_Malloc(sizeof(Net_IPv4_Packet_Pseudo_Heade_Type));

	if(P_Pseudo_Heade==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_UDP_Tx_Exit1;
	}
	if((Err=IPv4_Pseudo_Heade_Init(
			P_Pseudo_Heade,
			P_Net_Node->IPv4_DATA.IP_Address.IP.Address,
			DEST_IP_Address,
			Net_IPv4_Protocol_UDP,
			sizeof(Net_Protocol_UDP_Packet_Heade_Type)+Size))!=Error_OK)
	{
		goto Protocol_UDP_Tx_Exit2;
	}

	uint8_t *Protocol_UDP_Packet=Memory_Malloc(sizeof(Net_Protocol_UDP_Packet_Heade_Type)+Size);

	if(Protocol_UDP_Packet==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_UDP_Tx_Exit2;
	}

	Net_Protocol_UDP_Packet_Heade_Type *P_Protocol_UDP_Packet_Heade=(Net_Protocol_UDP_Packet_Heade_Type *)Protocol_UDP_Packet;

	P_Protocol_UDP_Packet_Heade->SRC_PORT=UINT16_REVERSE(SRC_PORT);
	P_Protocol_UDP_Packet_Heade->DEST_PORT=UINT16_REVERSE(DEST_PORT);
	P_Protocol_UDP_Packet_Heade->Total_Length=UINT16_REVERSE(sizeof(Net_Protocol_UDP_Packet_Heade_Type)+Size);
	P_Protocol_UDP_Packet_Heade->CheckSum=UINT16_REVERSE(0x0000);

	memcpy(&Protocol_UDP_Packet[sizeof(Net_Protocol_UDP_Packet_Heade_Type)],DATA,Size);

	uint16_t CheckSum=Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)Protocol_UDP_Packet,
			sizeof(Net_Protocol_UDP_Packet_Heade_Type)+Size);
	P_Protocol_UDP_Packet_Heade->CheckSum=UINT16_REVERSE(CheckSum);

	Err=IPv4_Tx_Pend_DATA(
				P_Net_Node,
				Net_IPv4_Protocol_UDP,
				DEST_IP_Address,
				Protocol_UDP_Packet,
				sizeof(Net_Protocol_UDP_Packet_Heade_Type)+Size,
				true);

	if(Err==Error_OK)
	{
		goto Protocol_UDP_Tx_Exit2;
	}
	else
	{
		goto Protocol_UDP_Tx_Exit3;
	}

Protocol_UDP_Tx_Exit3:
	Memory_Free(Protocol_UDP_Packet);

Protocol_UDP_Tx_Exit2:
	Memory_Free(P_Pseudo_Heade);

Protocol_UDP_Tx_Exit1:
	return Err;
}
