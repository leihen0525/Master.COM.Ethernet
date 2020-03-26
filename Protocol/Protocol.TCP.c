/*
 * Protocol.TCP.c
 *
 *  Created on: 2019年7月17日
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

#include "Protocol.PORT.h"

#include "Protocol.TCP.Link.h"

int Protocol_TCP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA)
{
	if(P_Net_Node==Null || P_Protocol_TCP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	int Err;

	if((Err=Protocol_PORT_Init(&P_Protocol_TCP_DATA->PORT_DATA))!=Error_OK)
	{
		return Err;
	}

	if((Err=Protocol_TCP_Link_Init(&P_Protocol_TCP_DATA->Link_DATA))!=Error_OK)
	{
		return Err;
	}

	P_Protocol_TCP_DATA->ISN=0;


	//TODO 临时添加了80端口 测试
	Protocol_PORT_Add(&P_Protocol_TCP_DATA->PORT_DATA,80,0);

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
		Net_Protocol_IP_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size)
{
	if(IP_Type>=Net_Protocol_IP_End
	|| P_Net_Node==Null
	|| P_Pseudo_Heade==Null
	|| Protocol_TCP_Packet==Null
	|| Protocol_TCP_Packet_Size==0
	|| Protocol_TCP_Packet_Size<sizeof(Net_Protocol_TCP_Packet_Heade_Type))
	{
		return ;
	}

	if(Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)Protocol_TCP_Packet,
			Protocol_TCP_Packet_Size)!=UINT16_REVERSE(0x0000))
	{
		return ;
	}

	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)Protocol_TCP_Packet;

	//检查端口
	uint16_t PORT_Dest=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->DEST_PORT);

	if(Protocol_PORT_Find(&P_Net_Node->Protocol_TCP_DATA.PORT_DATA,PORT_Dest,Null)!=Error_OK)
	{
		return ;
	}

	Net_Protocol_TCP_Packet_Heade_Flags_Type Flags={
														.DATA=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->Flags)
													};

	//请求连接
	if(Flags.Syn==1)
	{
		//return ;
	}

	return ;
}
