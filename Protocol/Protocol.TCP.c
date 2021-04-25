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
	Protocol_PORT_Add(&P_Protocol_TCP_DATA->PORT_DATA,80,(Net_Protocol_PORT_Node_Flag_Type){.DATA=0});

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
	uint16_t PORT_Src=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->SRC_PORT);
	uint16_t PORT_Dest=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->DEST_PORT);
	uint32_t ISN=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Sequence_Number);


	Net_Protocol_TCP_Packet_Heade_Flags_Type Flags={
														.DATA=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->Flags)
													};

	//请求连接
	if(Flags.Syn==1)
	{

		if(Protocol_PORT_Find(&P_Net_Node->Protocol_TCP_DATA.PORT_DATA,PORT_Dest,Null)!=Error_OK)//拒绝掉未监听的端口连接请求
		{
			Net_Protocol_TCP_Packet_Heade_Flags_Type Temp_Flags={.DATA=0};
			Temp_Flags.Ack=1;
			Temp_Flags.Reset=1;
			Protocol_TCP_Tx(IP_Type,P_Net_Node,P_Pseudo_Heade->SRC_IPv4_Address,PORT_Dest,PORT_Src,0,ISN+1,Temp_Flags,0,0,Null,0,Null,0);

			return ;
		}


		//return ;
	}



	return ;
}
//发送一帧TCP报文
int Protocol_TCP_Tx(
		Net_Protocol_IP_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint32_t ISN,
		uint32_t ACKN,
		Net_Protocol_TCP_Packet_Heade_Flags_Type Flags,
		uint16_t Windows,
		uint16_t Urgent_Pointer,
		uint8_t *Option,
		uint16_t Option_Size,
		uint8_t *DATA,
		uint16_t DATA_Size)
{
	if(IP_Type>=Net_Protocol_IP_End
	|| P_Net_Node==Null
	|| DEST_IP_Address==Null
	|| (Option_Size!=0 && Option==Null)
	|| (Option_Size%4)!=0
	|| Option_Size>40
	|| (DATA_Size!=0 && DATA==Null))
	{
		return Error_Invalid_Parameter;
	}

	int Err=Error_OK;

	Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade=Memory_Malloc(sizeof(Net_IPv4_Packet_Pseudo_Heade_Type));

	if(P_Pseudo_Heade==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_TCP_Tx_Exit1;
	}
	if((Err=IPv4_Pseudo_Heade_Init(
			P_Pseudo_Heade,
			P_Net_Node->IPv4_DATA.IP_Address.IP.Address,
			DEST_IP_Address,
			Net_IPv4_Protocol_TCP,
			sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size))!=Error_OK)
	{
		goto Protocol_TCP_Tx_Exit2;
	}

	uint8_t *Protocol_TCP_Packet=Memory_Malloc(sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size);

	if(Protocol_TCP_Packet==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_TCP_Tx_Exit2;
	}

	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)Protocol_TCP_Packet;

	P_Protocol_TCP_Packet_Heade->SRC_PORT=UINT16_REVERSE(SRC_PORT);
	P_Protocol_TCP_Packet_Heade->DEST_PORT=UINT16_REVERSE(DEST_PORT);
	P_Protocol_TCP_Packet_Heade->Sequence_Number=UINT32_REVERSE(ISN);
	P_Protocol_TCP_Packet_Heade->Acknowledgment_Number=UINT32_REVERSE(ACKN);
	Flags.Header_Length=(sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size)/4;
	P_Protocol_TCP_Packet_Heade->Flags=UINT16_REVERSE(Flags.DATA);
	P_Protocol_TCP_Packet_Heade->Window_Size=UINT16_REVERSE(Windows);
	P_Protocol_TCP_Packet_Heade->CheckSum=UINT16_REVERSE(0x0000);
	P_Protocol_TCP_Packet_Heade->Urgent_Pointer=UINT16_REVERSE(Urgent_Pointer);

	//Option
	if(Option_Size!=0)
	{
		memcpy(&Protocol_TCP_Packet[sizeof(Net_Protocol_TCP_Packet_Heade_Type)],Option,Option_Size);
	}
	//DATA
	if(DATA_Size!=0)
	{
		memcpy(&Protocol_TCP_Packet[sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size],DATA,DATA_Size);
	}

	uint16_t CheckSum=Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)Protocol_TCP_Packet,
			sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size);
	P_Protocol_TCP_Packet_Heade->CheckSum=UINT16_REVERSE(CheckSum);

	Err=IPv4_Tx_Pend_DATA(
				P_Net_Node,
				Net_IPv4_Protocol_TCP,
				DEST_IP_Address,
				Protocol_TCP_Packet,
				sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size,
				true);

	if(Err==Error_OK)
	{
		goto Protocol_TCP_Tx_Exit2;
	}
	else
	{
		goto Protocol_TCP_Tx_Exit3;
	}

Protocol_TCP_Tx_Exit3:
	Memory_Free(Protocol_TCP_Packet);

Protocol_TCP_Tx_Exit2:
	Memory_Free(P_Pseudo_Heade);

Protocol_TCP_Tx_Exit1:
	return Err;
}


