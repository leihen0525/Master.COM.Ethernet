/*
 * Protocol.TCP.c
 *
 *  Created on: 2019��7��17��
 *      Author: Master.HE
 */
#include <string.h>

#include "Error.h"
#include "API.h"

#include "Core/Core.Define.h"
#include "Core/Core.h"

#include "IPv4/IPv4.Enum.h"
#include "IPv4/IPv4.h"

#include "Protocol.TCP.Struct.h"

#include "Protocol.TCP.h"

int Protocol_TCP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA)
{
	if(P_Net_Node==Null || P_Protocol_TCP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}

	P_Protocol_TCP_DATA->ISN=0;


	return Error_OK;
}

int Protocol_TCP_ISN_10MS(Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA)
{
	if(P_Protocol_TCP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	P_Protocol_TCP_DATA->ISN++;

	return Error_OK;
}


void Protocol_TCP_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *IPv4_TCP_Packet,
		uint16_t IPv4_TCP_Packet_Size)
{
	if(P_Net_Node==Null
	|| P_Pseudo_Heade==Null
	|| IPv4_TCP_Packet==Null
	|| IPv4_TCP_Packet_Size==0
	|| IPv4_TCP_Packet_Size<sizeof(Net_Protocol_TCP_Packet_Heade_Type))
	{
		return ;
	}

	if(Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)IPv4_TCP_Packet,
			IPv4_TCP_Packet_Size)!=UINT16_REVERSE(0x0000))
	{
		return ;
	}
	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)IPv4_TCP_Packet;


	return ;
}
