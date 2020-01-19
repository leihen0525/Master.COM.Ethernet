/*
 * Core.Device.c
 *
 *  Created on: 2019Äê7ÔÂ15ÈÕ
 *      Author: Master.HE
 */
#include <string.h>
#include "Master.Stdint.h"
#include "API.h"
#include "Core.Struct.h"

#include "Includes/Net.Core.Device.h"
//#include "Includes/Net.Device.Struct.h"

#include "IPv4/IPv4.ARP.h"
#include "IPv4/IPv4.h"
#include "Protocol/Protocol.TCP.h"
#include "Protocol/Protocol.UDP.h"
//#include "Session/Session.h"
#include "Socket/Socket.h"


extern Net_Core_DATA_Type Net_Core_DATA;

int Net_Core_Device_Register(
		Net_Device_Type Type,
		bool DHCP,
		Net_Device_IP_Config *P_IP_Config,
		const Net_Device_OPS_Type *P_OPS)
{

	if(P_OPS==Null || Type>=Net_Device_Type_End || (DHCP!=false && P_IP_Config==Null))
	{
		return Error_Invalid_Parameter;
	}

	if(P_OPS->Init==Null
	|| P_OPS->Open==Null
	|| P_OPS->Close==Null
	|| P_OPS->SET_MAC_Address==Null
	|| P_OPS->GET_Linked_Status==Null
	|| P_OPS->Read==Null
	|| P_OPS->Write==Null)
	{
		return Error_Invalid_Parameter;
	}


	Net_Core_Device_Node_Type *Temp_Node=Memory_Malloc(sizeof(Net_Core_Device_Node_Type));
	if(Temp_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	Temp_Node->Device_Type=Type;
	Temp_Node->DHCP=DHCP;

	if(DHCP==false)
	{
		memcpy(&Temp_Node->IP_Config,P_IP_Config,sizeof(Net_Device_IP_Config));
	}
	Temp_Node->P_OPS=P_OPS;


	//Init
	int Err;
	Temp_Node->Open=false;

	Temp_Node->Rx_DATA=Memory_Malloc(Temp_Node->P_OPS->HEADER+Temp_Node->P_OPS->MTU);
	if(Temp_Node->Rx_DATA==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	if((Err=IPv4_ARP_Init(Temp_Node,&Temp_Node->ARP_DATA))!=Error_OK)
	{
		return Err;
	}
	if((Err=IPv4_Init(Temp_Node,&Temp_Node->IPv4_DATA))!=Error_OK)
	{
		return Err;
	}
	if((Err=Protocol_TCP_Init(Temp_Node,&Temp_Node->Protocol_TCP_DATA))!=Error_OK)
	{
		return Err;
	}
	if((Err=Protocol_UDP_Init(Temp_Node,&Temp_Node->Protocol_UDP_DATA))!=Error_OK)
	{
		return Err;
	}

//	if((Err=Session_Init(Temp_Node,&Temp_Node->Session_DATA))!=Error_OK)
//	{
//		return Err;
//	}

	//
	if(Net_Core_DATA.Device_Node_List.End==Null)
	{
		Net_Core_DATA.Device_Node_List.Head=Temp_Node;
	}
	else
	{
		Net_Core_DATA.Device_Node_List.End->NEXT=Temp_Node;
	}
	Net_Core_DATA.Device_Node_List.End=Temp_Node;
	Temp_Node->NEXT=Null;

	return Error_OK;
}
