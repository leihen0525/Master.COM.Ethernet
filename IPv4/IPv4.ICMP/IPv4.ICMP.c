/*
 * IPv4.ICMP.c
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */
#include <string.h>
#include "Error.h"
#include "API.h"
#include "Core/Core.Define.h"
#include "Core/Core.h"

#include "IPv4/IPv4.h"
#include "IPv4.ICMP.Enum.h"
#include "IPv4.ICMP.Struct.h"
#include "IPv4.ICMP.h"


void IPv4_ICMP_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *SRC_IPv4_Address,
		uint8_t *IPv4_ICMP_Packet,
		uint16_t IPv4_ICMP_Packet_Size)
{
	if(P_Net_Node==Null
	|| SRC_IPv4_Address==Null
	|| IPv4_ICMP_Packet==Null
	|| IPv4_ICMP_Packet_Size==0
	|| IPv4_ICMP_Packet_Size<sizeof(Net_IPv4_ICMP_Packet_Heade_Type))
	{
		return ;
	}
	if(Net_Core_CheckSum((uint16_t *)IPv4_ICMP_Packet,IPv4_ICMP_Packet_Size,Null,0)!=UINT16_REVERSE(0x0000))
	{
		return ;
	}

	Net_IPv4_ICMP_Packet_Heade_Type *P_IPv4_ICMP_Packet_Heade=(Net_IPv4_ICMP_Packet_Heade_Type *)IPv4_ICMP_Packet;

	switch (P_IPv4_ICMP_Packet_Heade->Type)
	{
		case Net_IPv4_ICMP_Type_Echo_Reply_Message:
		{
			if(P_IPv4_ICMP_Packet_Heade->Code!=0x00)
			{
				return ;
			}
		}break;
		case Net_IPv4_ICMP_Type_Echo_Message:
		{
			if(P_IPv4_ICMP_Packet_Heade->Code!=0x00)
			{
				return ;
			}

			IPv4_ICMP_Tx(
					P_Net_Node,
					SRC_IPv4_Address,
					Net_IPv4_ICMP_Type_Echo_Reply_Message,
					0x00,
					&IPv4_ICMP_Packet[sizeof(Net_IPv4_ICMP_Packet_Heade_Type)],
					IPv4_ICMP_Packet_Size-sizeof(Net_IPv4_ICMP_Packet_Heade_Type));




		}break;
		default:
		{

		}break;
	}


}



int IPv4_ICMP_Tx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IPv4_Address,
		uint8_t Type,
		uint8_t Code,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Net_Node==Null || DEST_IPv4_Address==Null || (Size!=0 && DATA==Null))
	{
		return Error_Invalid_Parameter;
	}
	uint8_t *P_DATA=Memory_Malloc(sizeof(Net_IPv4_ICMP_Packet_Heade_Type)+Size);

	if(P_DATA==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	Net_IPv4_ICMP_Packet_Heade_Type *P_IPv4_ICMP_Packet=(Net_IPv4_ICMP_Packet_Heade_Type *)P_DATA;

	P_IPv4_ICMP_Packet->Type=Type;
	P_IPv4_ICMP_Packet->Code=Code;
	P_IPv4_ICMP_Packet->Checksum=UINT16_REVERSE(0x0000);

	memcpy(&P_DATA[sizeof(Net_IPv4_ICMP_Packet_Heade_Type)],DATA,Size);

	uint16_t CheckSum=Net_Core_CheckSum((uint16_t *)P_DATA,sizeof(Net_IPv4_ICMP_Packet_Heade_Type)+Size,Null,0);
	P_IPv4_ICMP_Packet->Checksum=UINT16_REVERSE(CheckSum);

	int Err=IPv4_Tx_Pend_DATA(
				P_Net_Node,
				Net_IPv4_Protocol_ICMP,
				DEST_IPv4_Address,
				P_DATA,
				sizeof(Net_IPv4_ICMP_Packet_Heade_Type)+Size,
				true);

	if(Err!=Error_OK)
	{
		Memory_Free(P_DATA);
	}

	return Err;
}
