/*
 * Core.Device.c
 *
 *  Created on: 2019年7月15日
 *      Author: Master.HE
 */
#include <string.h>
#include "List.h"
#include "Master.Stdint.h"
#include "API.h"
#include "Core.Struct.h"

#include "Includes/Net.Core.Device.h"
//#include "Includes/Net.Device.Struct.h"

#include "IPv4/IPv4.ARP.h"
#include "IPv4/IPv4.h"
#include "IPv4/IPv4.Define.h"
#include "Protocol/Protocol.TCP.h"
#include "Protocol/Protocol.UDP.h"
//#include "Session/Session.h"
//#include "Socket/Socket.h"


extern Net_Core_DATA_Type Net_Core_DATA;

int Net_Core_Device_Register(
		Net_Device_Type Device_Type,
		const Net_Device_Ethernet_Config_Type *P_Ethernet_Config,
		const Net_Device_IP_Config_Type *P_IP_Config,
		const Net_Device_OPS_Type *P_OPS)
{

	if(P_OPS==Null || Device_Type>=Net_Device_Type_End || P_Ethernet_Config==Null || P_IP_Config==Null)
	{
		return Error_Invalid_Parameter;
	}

	if(P_OPS->Init==Null
	|| P_OPS->Open==Null
	|| P_OPS->Close==Null
	|| P_OPS->SET_MAC_Address==Null
	|| P_OPS->GET_Linked_Status==Null
	|| P_OPS->Read==Null
	|| P_OPS->Write==Null

	|| P_IP_Config->IPv4.DHCP>=Enabled_End
	|| P_IP_Config->IPv6.DHCP>=Enabled_End)
	{
		return Error_Config;
	}


	Net_Core_Device_Node_Type *Temp_Node=Memory_Malloc(sizeof(Net_Core_Device_Node_Type));
	if(Temp_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	Temp_Node->Device_Type=Device_Type;

	//IPv4
	Temp_Node->IPv4_DATA.IP_Address.DHCP.Enabled=P_IP_Config->IPv4.DHCP;
	Temp_Node->IPv4_DATA.IP_Address.DHCP.Achieve=false;
	if(P_IP_Config->IPv4.DHCP==Enable)
	{
		Temp_Node->IPv4_DATA.IP_Address.IP.Confirm=false;
		Temp_Node->IPv4_DATA.IP_Address.Default_Gateway.Confirm=false;
	}
	else
	{
		Temp_Node->IPv4_DATA.IP_Address.IP.Confirm=true;
		Temp_Node->IPv4_DATA.IP_Address.Default_Gateway.Confirm=true;

		memcpy(&Temp_Node->IPv4_DATA.IP_Address.IP.Address,P_IP_Config->IPv4.IP_Address,Net_IPv4_Adress_Size_Byte);
		memcpy(&Temp_Node->IPv4_DATA.IP_Address.IP.SubNet_Mask,P_IP_Config->IPv4.SubNet_Mask,Net_IPv4_Adress_Size_Byte);
		memcpy(&Temp_Node->IPv4_DATA.IP_Address.Default_Gateway.Address,P_IP_Config->IPv4.Default_Gateway,Net_IPv4_Adress_Size_Byte);
	}

	//TODO IPv6


	Temp_Node->P_Ethernet_Config=P_Ethernet_Config;
	Temp_Node->P_OPS=P_OPS;


	//Init
	int Err;
	Temp_Node->Open=false;

	Temp_Node->P_Rx_DATA=Memory_Malloc(Temp_Node->P_OPS->HEADER+Temp_Node->P_OPS->MTU);
	if(Temp_Node->P_Rx_DATA==Null)
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
	List_Add_Node_To_End(
			Net_Core_DATA.Device_Node_List.Head,
			Net_Core_DATA.Device_Node_List.End,
			NEXT,
			Temp_Node);

	return Error_OK;
}
