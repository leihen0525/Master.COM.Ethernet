/*
 * Core.c
 *
 *  Created on: 2019年7月15日
 *      Author: Master.HE
 */
#include "Define.h"
#include "Error.h"
#include "Module/Module.h"
#include "API.h"

#include "Core.Define.h"

#include "Core.Ethernet/Core.Ethernet.h"

#include "Includes/Net.Device.Enum.h"

#include "IPv4/IPv4.ARP/IPv4.ARP.h"
#include "IPv4/IPv4.h"
#include "Protocol/Protocol.TCP/Protocol.TCP.h"
#include "Protocol/Protocol.UDP/Protocol.UDP.h"
//#include "Session/Session.h"
//#include "Socket/Socket.h"


#include "Core.h"

#include "API/API.UDP/API.UDP.h"
#include "API/API.TCP/API.TCP.h"


Net_Core_DATA_Type Net_Core_DATA;


int __Sys_Net_Core_Init(void)
{

	Net_Core_DATA.Device_Node_List.Head=Null;
	Net_Core_DATA.Device_Node_List.End=Null;



	return Error_OK;
}

int Net_Core_Init(void)
{

	Net_Core_Device_Node_Type *Net_Node=Net_Core_DATA.Device_Node_List.Head;

	int Err;
//	Net_Socket_Init();
//	if(Err!=Error_OK)
//	{
//		return Err;
//	}
	Error_NoArgs_Return(Err,Net_TCP_Init());
	Error_NoArgs_Return(Err,Net_UDP_Init());

	while(Net_Node!=Null)
	{

		Error_Args_Return(Net_Node->Task_PID_Rx,Scheduling_Create_Task(
				(char*)Net_Node->P_OPS->Device_Name,
				Net_Core_Task_RX,
				Net_Node,
				100,
				Null,
				200,
				Scheduling_Task_Option_User));


		Net_Node=Net_Node->NEXT;
	}


	return Error_OK;
}

__task void Net_Core_Task_TX(void *Args)
{
	Net_Core_Device_Node_Type *Net_Node=Args;

	if(Net_Node==Null)return;


	while(1)
	{
		switch (Net_Node->Device_Type)
		{
			case Net_Device_Ethernet:
			{
				Net_Core_Ethernet_Handle_Tx(Net_Node);
			}break;

			default:
			{

			}break;
		}


	}
}

__task void Net_Core_Task_RX(void *Args)
{
	Net_Core_Device_Node_Type *Net_Node=Args;

	if(Net_Node==Null)return;


	if(Net_Node->P_OPS->Init(0)!=Error_OK)
	{
		return ;
	}


	if(Net_Node->P_OPS->SET_MAC_Address((uint8_t*)Net_Node->P_OPS->MAC_Address)!=Error_OK)
	{
		return ;
	}


	Net_Node->Task_PID_Tx=Scheduling_Create_Task(
			(char*)Net_Node->P_OPS->Device_Name,
			Net_Core_Task_TX,
			Net_Node,
			99,
			Null,
			200,
			Scheduling_Task_Option_User);



	int Err;

	while(1)
	{
		Err=Net_Node->P_OPS->Read(Net_Node->P_Rx_DATA,Net_Node->P_OPS->HEADER+Net_Node->P_OPS->MTU,1000);

		if(Err==Error_OK)
		{
			switch (Net_Node->Device_Type)
			{
				case Net_Device_Ethernet:
				{
					Net_Core_Ethernet_Handle_Rx(Net_Node,Net_Node->P_Rx_DATA);
				}break;

				default:
				{

				}break;
			}


		}
		else if(Err==Error_Time_Out)
		{
			if(Net_Node->P_OPS->GET_Linked_Status()==true)
			{
				if(Net_Node->Open==false)
				{
					Net_Node->Open=true;
					Net_Node->P_OPS->Open();
				}
			}
			else
			{
				if(Net_Node->Open==true)
				{
					Net_Node->Open=false;
					Net_Node->P_OPS->Close();
				}
			}
		}
		else
		{
			Scheduling_Sleep_Task(100);
		}




	}
}


int Net_Core_Tx_DATA_ARP(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *Target_MAC_Address,
		uint8_t *ARP_Packet,
		uint16_t Packet_Size)
{
	if(P_Net_Node==Null || Target_MAC_Address==Null || ARP_Packet==Null || Packet_Size==0)
	{
		return Error_Invalid_Parameter;
	}
	switch (P_Net_Node->Device_Type)
	{
		case Net_Device_Ethernet:
		{
			return Net_Core_Ethernet_Tx_DATA(
						P_Net_Node,
						Net_Core_Ethernet_Protocol_ARP,
						Target_MAC_Address,
						ARP_Packet,
						Packet_Size);
		}break;

		default:
		{
			return Error_Undefined;
		}break;
	}

	//return Error_Undefined;
}

int Net_Core_Tx_DATA_IPv4(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *Target_MAC_Address,
		uint8_t *IPv4_Packet_Heade,
		uint16_t IPv4_Packet_Heade_Size,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Net_Node==Null || Target_MAC_Address==Null || IPv4_Packet_Heade==Null || IPv4_Packet_Heade_Size==0 || DATA==Null || Size==0)
	{
		return Error_Invalid_Parameter;
	}
	switch (P_Net_Node->Device_Type)
	{
		case Net_Device_Ethernet:
		{
			return Net_Core_Ethernet_Tx_DATAs(
						P_Net_Node,
						Net_Core_Ethernet_Protocol_IPv4,
						Target_MAC_Address,
						IPv4_Packet_Heade,
						IPv4_Packet_Heade_Size,
						DATA,
						Size);
		}break;

		default:
		{
			return Error_Undefined;
		}break;
	}

	//return Error_Undefined;
}






uint16_t Net_Core_CheckSum(uint16_t * buffer1, uint32_t size1,uint16_t * buffer2, uint32_t size2)
{
	uint32_t sum = 0;
	uint16_t tmp16;


	//1.计算校验和
	while(size1 > 1)
	{
		tmp16 = *buffer1 ++;
		sum  += UINT16_REVERSE(tmp16);
		size1 -= sizeof(uint16_t);
	}
	if(size1)
	{
		sum  += UINT16_REVERSE(*(uint8_t*)buffer1);
	}

	while(size2 > 1)
	{
		tmp16 = *buffer2 ++;
		sum  += UINT16_REVERSE(tmp16);
		size2 -= sizeof(uint16_t);
	}
	if(size2)
	{
		sum  += UINT16_REVERSE(*(uint8_t*)buffer2);
	}

	//2.将32位数转换成16
	while(sum>>16)
	{
		sum = (sum>>16) + (sum & 0x0000FFFF);
	}
	return (uint16_t)(~sum);
}






__Sys_Com_Module_Init_Export(__Sys_Net_Core_Init);

Application_Module_Init_Export(Net_Core_Init);
