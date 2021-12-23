/*
 * IPv4.ARP.c
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */
#include <string.h>
#include "List.h"
#include "Error.h"
#include "API.h"
#include "Core/Core.Define.h"
#include "Core/Core.Ethernet/Core.Ethernet.Enum.h"
#include "Core/Core.h"
#include "IPv4/IPv4.Define.h"
#include "IPv4.ARP.Define.h"
#include "IPv4.ARP.Struct.h"
#include "IPv4.ARP.h"
#include "IPv4/IPv4.h"


Net_IPv4_ARP_Cache_Node_Type Static_Net_IPv4_ARP_Cache_Node[]=
{
	{
		.IPv4_Address={255,255,255,255},
		.MAC_Address={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		.TimeOut=-1,
		.NEXT=Null
	},

};






int IPv4_ARP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_IPv4_ARP_DATA_Type *P_IPv4_ARP_DATA)
{
	if(P_Net_Node==Null || P_IPv4_ARP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}

	P_IPv4_ARP_DATA->ARP_Cache_List.Head=Null;
	P_IPv4_ARP_DATA->ARP_Cache_List.End=Null;


	//添加静态ARP
	int Size=sizeof(Static_Net_IPv4_ARP_Cache_Node)/sizeof(Net_IPv4_ARP_Cache_Node_Type);
	for(int i=0;i<Size;i++)
	{
		Net_IPv4_ARP_Cache_Node_Type *P_Node=&Static_Net_IPv4_ARP_Cache_Node[i];
		List_Add_Node_To_End(
				P_Net_Node->ARP_DATA.ARP_Cache_List.Head,
				P_Net_Node->ARP_DATA.ARP_Cache_List.End,
				NEXT,
				P_Node);

	}

	P_IPv4_ARP_DATA->Mutex=Mutex_Create(Null,Event_Queue_Option_Priority);
	P_IPv4_ARP_DATA->Time_1S=Timer_Register(IPv4_ARP_Time_1S,P_Net_Node);
	Timer_Start(P_IPv4_ARP_DATA->Time_1S,0,1000,Timer_Operation_Loop_Cycle);

	return Error_OK;

}
int IPv4_ARP_Cache_Add(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address,uint8_t *MAC_Address)
{
	if(P_Net_Node==Null || IPv4_Address==Null || MAC_Address==Null)
	{
		return Error_Invalid_Parameter;
	}
	int Err=Mutex_Wait(P_Net_Node->ARP_DATA.Mutex,-1);
	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_IPv4_ARP_Cache_Node_Type *P_Cache_Node=P_Net_Node->ARP_DATA.ARP_Cache_List.Head;

	while(P_Cache_Node!=Null)
	{
		if(memcmp(P_Cache_Node->IPv4_Address,IPv4_Address,4)==0)
		{
			memcpy(P_Cache_Node->MAC_Address,MAC_Address,6);

			P_Cache_Node->TimeOut=Net_IPv4_ARP_Cache_Max_Validity_Time_1S;

			Err=Error_OK;

			goto IPv4_ARP_Cache_Add_Exit;
		}

		P_Cache_Node=P_Cache_Node->NEXT;
	}
	P_Cache_Node=Null;
	P_Cache_Node=Memory_Malloc(sizeof(Net_IPv4_ARP_Cache_Node_Type));

	if(P_Cache_Node==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto IPv4_ARP_Cache_Add_Exit;
	}

	memcpy(P_Cache_Node->IPv4_Address,IPv4_Address,4);
	memcpy(P_Cache_Node->MAC_Address,MAC_Address,6);
	P_Cache_Node->TimeOut=Net_IPv4_ARP_Cache_Max_Validity_Time_1S;


	List_Add_Node_To_End(
			P_Net_Node->ARP_DATA.ARP_Cache_List.Head,
			P_Net_Node->ARP_DATA.ARP_Cache_List.End,
			NEXT,
			P_Cache_Node);


	Err=Error_OK;

IPv4_ARP_Cache_Add_Exit:

	Mutex_Release(P_Net_Node->ARP_DATA.Mutex);

	return Err;

}
int IPv4_ARP_Cache_Find_Mac_Address(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address,uint8_t *MAC_Address)
{
	if(P_Net_Node==Null || IPv4_Address==Null || MAC_Address==Null)
	{
		return Error_Invalid_Parameter;
	}
	int Err=Mutex_Wait(P_Net_Node->ARP_DATA.Mutex,-1);
	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_IPv4_ARP_Cache_Node_Type *P_Cache_Node=P_Net_Node->ARP_DATA.ARP_Cache_List.Head;

	Err=false;

	while(P_Cache_Node!=Null)
	{
		if(memcmp(P_Cache_Node->IPv4_Address,IPv4_Address,4)==0)
		{
			memcpy(MAC_Address,P_Cache_Node->MAC_Address,6);

			Err=true;

			goto IPv4_ARP_Cache_Find_Mac_Address_Exit;
		}

		P_Cache_Node=P_Cache_Node->NEXT;
	}

IPv4_ARP_Cache_Find_Mac_Address_Exit:
	Mutex_Release(P_Net_Node->ARP_DATA.Mutex);

	return Err;
}

void IPv4_ARP_Time_1S(void *Args)
{
	Net_Core_Device_Node_Type *P_Net_Node=Args;

	if(P_Net_Node==Null)
	{
		return ;
	}

	if(Mutex_Wait(P_Net_Node->ARP_DATA.Mutex,-1)!=Error_OK)
	{
		return ;
	}


	Net_IPv4_ARP_Cache_Node_Type *P_Cache_Node=Null,*P_Cache_Node_LAST=Null,*P_Cache_Node_NEXT=Null;

	P_Cache_Node=P_Net_Node->ARP_DATA.ARP_Cache_List.Head;


	while(P_Cache_Node!=Null)
	{
		P_Cache_Node_NEXT=P_Cache_Node->NEXT;


		if(P_Cache_Node->TimeOut>0)
		{
			P_Cache_Node->TimeOut--;
		}
		if(P_Cache_Node->TimeOut==0)
		{

			if(P_Cache_Node_LAST==Null)
			{
				P_Net_Node->ARP_DATA.ARP_Cache_List.Head=P_Cache_Node_NEXT;
			}
			else
			{
				P_Cache_Node_LAST->NEXT=P_Cache_Node_NEXT;
			}

			if(P_Cache_Node==P_Net_Node->ARP_DATA.ARP_Cache_List.End || P_Cache_Node_NEXT==Null)
			{
				P_Net_Node->ARP_DATA.ARP_Cache_List.End=P_Cache_Node_LAST;
			}
			P_Cache_Node->NEXT=Null;

			Memory_Free(P_Cache_Node);

			P_Cache_Node=P_Cache_Node_NEXT;

		}
		else
		{

			P_Cache_Node_LAST=P_Cache_Node;
			P_Cache_Node=P_Cache_Node_NEXT;
		}

	}


	Mutex_Release(P_Net_Node->ARP_DATA.Mutex);
}

void IPv4_ARP_Handle_Rx(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *ARP_Packet)
{
	Net_IPv4_ARP_Packet_Type *P_ARP_Packet=(Net_IPv4_ARP_Packet_Type *)ARP_Packet;

	if(P_Net_Node==Null || P_ARP_Packet==Null)
	{
		return ;
	}

	if(memcmp(P_Net_Node->IPv4_DATA.IP_Address.IP.Address,P_ARP_Packet->Target_IP_Address,4)!=0)
	{
		return ;
	}

	switch(UINT16_REVERSE(P_ARP_Packet->Opcode))
	{
		case Net_IPv4_ARP_Opcode_Reqst:
		{
			IPv4_ARP_Tx(
					P_Net_Node,
					P_ARP_Packet->Sender_MAC_Address,
					Net_IPv4_ARP_Opcode_Reply,
					P_ARP_Packet->Sender_MAC_Address,
					P_ARP_Packet->Sender_IP_Address);
		}break;

		case Net_IPv4_ARP_Opcode_Reply:
		{
			//检查目标MAC Address是否为本机
			if(memcmp(P_Net_Node->P_OPS->MAC_Address,P_ARP_Packet->Target_MAC_Address,6)!=0)
			{
				return ;
			}

			if(IPv4_ARP_Cache_Add(P_Net_Node,P_ARP_Packet->Sender_IP_Address,P_ARP_Packet->Sender_MAC_Address)==Error_OK)
			{

				IPv4_Tx_Check_Pend_List(P_Net_Node,P_ARP_Packet->Sender_IP_Address,P_ARP_Packet->Sender_MAC_Address);

			}
		}break;

		default:
		{

		}break;
	}
}
int IPv4_ARP_Tx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_MAC_Address,
		Net_IPv4_ARP_Opcode_Type Opcode,
		uint8_t *Target_MAC_Address,
		uint8_t *Target_IPv4_Address)
{
	if(DEST_MAC_Address==Null
	|| (Opcode!=Net_IPv4_ARP_Opcode_Reqst && Opcode!=Net_IPv4_ARP_Opcode_Reply)
	|| Target_MAC_Address==Null
	|| Target_IPv4_Address==Null)
	{
		return Error_Invalid_Parameter;
	}
	Net_IPv4_ARP_Packet_Type *P_ARP_Packet=Memory_Malloc(sizeof(Net_IPv4_ARP_Packet_Type));

	P_ARP_Packet->Hardware_Type=UINT16_REVERSE(0x01);

	P_ARP_Packet->Protocol_Type=UINT16_REVERSE(Net_Core_Ethernet_Protocol_IPv4);

	P_ARP_Packet->Hardware_Size=0x06;

	P_ARP_Packet->Protocol_Size=0x04;

	P_ARP_Packet->Opcode=UINT16_REVERSE(Opcode);

	memcpy(P_ARP_Packet->Sender_MAC_Address,P_Net_Node->P_OPS->MAC_Address,6);
	memcpy(P_ARP_Packet->Sender_IP_Address,P_Net_Node->IPv4_DATA.IP_Address.IP.Address,4);

	memcpy(P_ARP_Packet->Target_MAC_Address,Target_MAC_Address,6);
	memcpy(P_ARP_Packet->Target_IP_Address,Target_IPv4_Address,4);

	int Err=Net_Core_Tx_DATA_ARP(P_Net_Node,DEST_MAC_Address,(uint8_t *)P_ARP_Packet,sizeof(Net_IPv4_ARP_Packet_Type));

	Memory_Free(P_ARP_Packet);

	return Err;

}
int IPv4_ARP_Reqst(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address)
{
	if(IPv4_Address==Null)
	{
		return Error_Invalid_Parameter;
	}
	return IPv4_ARP_Tx(
				P_Net_Node,
				Net_Core_MAC_Adress_0xFF,
				Net_IPv4_ARP_Opcode_Reqst,
				Net_Core_MAC_Adress_0x00,
				IPv4_Address);

}
