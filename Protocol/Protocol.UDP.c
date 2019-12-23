/*
 * Protocol.UDP.c
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */
#include <string.h>
#include "Error.h"
#include "API.h"

#include "Core/Core.Define.h"
#include "Core/Core.h"

#include "IPv4/IPv4.Enum.h"
#include "IPv4/IPv4.h"

#include "Protocol.UDP.Struct.h"

#include "Protocol.UDP.h"



uint8_t DATA1[4500]={0};

int Protocol_UDP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_UDP_DATA_Type *P_Protocol_UDP_DATA)
{
	if(P_Net_Node==Null || P_Protocol_UDP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}


	return Error_OK;
}

void Protocol_UDP_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *IPv4_UDP_Packet,
		uint16_t IPv4_UDP_Packet_Size)
{
	if(P_Net_Node==Null
	|| P_Pseudo_Heade==Null
	|| IPv4_UDP_Packet==Null
	|| IPv4_UDP_Packet_Size==0
	|| IPv4_UDP_Packet_Size<sizeof(Net_Protocol_UDP_Packet_Heade_Type))
	{
		return ;
	}

	if(Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)IPv4_UDP_Packet,
			IPv4_UDP_Packet_Size)!=UINT16_REVERSE(0x0000))
	{
		return ;
	}
	Net_Protocol_UDP_Packet_Heade_Type *P_Protocol_UDP_Packet_Heade=(Net_Protocol_UDP_Packet_Heade_Type *)IPv4_UDP_Packet;

	if(UINT16_REVERSE(P_Protocol_UDP_Packet_Heade->DEST_PORT)==6000)
	{

		for(int i=0;i<4500;i++)
		{
			DATA1[i]='A';
		}
		Protocol_UDP_Tx(P_Net_Node,P_Pseudo_Heade->SRC_IPv4_Address,6000,6000,DATA1,4500);
	}



}

int Protocol_UDP_Tx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IPv4_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Net_Node==Null || DEST_IPv4_Address==Null || (Size!=0 && DATA==Null))
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
			P_Net_Node->IP_Config.IPv4.IP_Address,
			DEST_IPv4_Address,
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
				DEST_IPv4_Address,
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
