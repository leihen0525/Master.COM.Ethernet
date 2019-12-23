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
