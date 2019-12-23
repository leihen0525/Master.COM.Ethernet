/*
 * Core.Ethernet.c
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */
#include <string.h>
#include "Error.h"
#include "API.h"
#include "Core.Define.h"


#include "Core.Ethernet.Struct.h"
#include "Core.Ethernet.h"

#include "IPv4/IPv4.ARP.h"
#include "IPv4/IPv4.h"


void Net_Core_Ethernet_Handle_Rx(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *DATA)
{
	Net_Core_Ethernet_Packet_Heade_Type *Ethernet_Packet_Heade=(Net_Core_Ethernet_Packet_Heade_Type *)DATA;

	if(P_Net_Node==Null || Ethernet_Packet_Heade==Null)
	{
		return;
	}

	switch (UINT16_REVERSE(Ethernet_Packet_Heade->Length_Type))
	{
		case Net_Core_Ethernet_Protocol_IPv4:
		{
			IPv4_Handle_Rx(P_Net_Node,&DATA[sizeof(Net_Core_Ethernet_Packet_Heade_Type)]);
		}break;
		case Net_Core_Ethernet_Protocol_ARP:
		{
			IPv4_ARP_Handle_Rx(P_Net_Node,&DATA[sizeof(Net_Core_Ethernet_Packet_Heade_Type)]);
		}break;
		case Net_Core_Ethernet_Protocol_IPv6:
		{

		}break;
		default:
		{

		}break;
	}

}

int Net_Core_Ethernet_Tx_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Core_Ethernet_Protocol_Type Protocol,
		uint8_t *Target_MAC_Address,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Net_Node==Null || Target_MAC_Address==Null || DATA==Null || Size==0)
	{
		return Error_Invalid_Parameter;
	}
	if(Size>P_Net_Node->P_OPS->MTU)
	{
		return Error_Invalid_Parameter;
	}
	Net_Core_Ether_TX_DATA_Type Ether_TX_DATA[2];

	Ether_TX_DATA[0].DATA=Memory_Malloc(sizeof(Net_Core_Ethernet_Packet_Heade_Type));
	Ether_TX_DATA[0].Size=sizeof(Net_Core_Ethernet_Packet_Heade_Type);

	if(Ether_TX_DATA[0].DATA==Null)
	{
		return Error_Allocation_Memory_Failed;

	}
	Net_Core_Ethernet_Packet_Heade_Type *P_Ethernet_Heade=(Net_Core_Ethernet_Packet_Heade_Type *)Ether_TX_DATA[0].DATA;

	memcpy(P_Ethernet_Heade->DEST_MAC_Address,Target_MAC_Address,6);

	memcpy(P_Ethernet_Heade->SRC_MAC_Address,P_Net_Node->P_OPS->MAC_Address,6);

	P_Ethernet_Heade->Length_Type=UINT16_REVERSE(Protocol);


	Ether_TX_DATA[1].DATA=DATA;
	Ether_TX_DATA[1].Size=Size;

	int Err=P_Net_Node->P_OPS->Write(1,&Ether_TX_DATA,2,10);

	Memory_Free(Ether_TX_DATA[0].DATA);

	return Err;

}

int Net_Core_Ethernet_Tx_DATAs(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Core_Ethernet_Protocol_Type Protocol,
		uint8_t *Target_MAC_Address,
		uint8_t *DATA0,
		uint16_t Size0,
		uint8_t *DATA1,
		uint16_t Size1)
{
	if(P_Net_Node==Null || Target_MAC_Address==Null || DATA0==Null || Size0==0 || DATA1==Null || Size1==0)
	{
		return Error_Invalid_Parameter;
	}
	if((Size0+Size1)>P_Net_Node->P_OPS->MTU)
	{
		return Error_Invalid_Parameter;
	}
	Net_Core_Ether_TX_DATA_Type Ether_TX_DATA[3];

	Ether_TX_DATA[0].DATA=Memory_Malloc(sizeof(Net_Core_Ethernet_Packet_Heade_Type));
	Ether_TX_DATA[0].Size=sizeof(Net_Core_Ethernet_Packet_Heade_Type);

	if(Ether_TX_DATA[0].DATA==Null)
	{
		return Error_Allocation_Memory_Failed;

	}
	Net_Core_Ethernet_Packet_Heade_Type *P_Ethernet_Heade=(Net_Core_Ethernet_Packet_Heade_Type *)Ether_TX_DATA[0].DATA;

	memcpy(P_Ethernet_Heade->DEST_MAC_Address,Target_MAC_Address,6);

	memcpy(P_Ethernet_Heade->SRC_MAC_Address,P_Net_Node->P_OPS->MAC_Address,6);

	P_Ethernet_Heade->Length_Type=UINT16_REVERSE(Protocol);


	Ether_TX_DATA[1].DATA=DATA0;
	Ether_TX_DATA[1].Size=Size0;

	Ether_TX_DATA[2].DATA=DATA1;
	Ether_TX_DATA[2].Size=Size1;

	int Err=P_Net_Node->P_OPS->Write(1,&Ether_TX_DATA,3,10);

	Memory_Free(Ether_TX_DATA[0].DATA);

	return Err;

}
