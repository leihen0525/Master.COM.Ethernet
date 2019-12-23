/*
 * IPv4.c
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */
#include <string.h>
#include "Error.h"
#include "Module/Module.h"
#include "API.h"
#include "Core/Core.Define.h"
#include "Core/Core.h"
#include "IPv4.Define.h"


#include "IPv4.ICMP.h"

#include "IPv4.h"
#include "IPv4.ARP.h"
#include "Protocol/Protocol.UDP.h"
#include "Protocol/Protocol.TCP.h"

//TODO 有一个功能没有实现 就是当分片的数据包被丢弃的时候 理论上会发送一帧ICMP数据包给发送着 告知数据包被丢弃了

int IPv4_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_IPv4_DATA_Type *P_IPv4_DATA)
{
	if(P_Net_Node==Null || P_IPv4_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	P_IPv4_DATA->Tx_List.Ready.Head=Null;
	P_IPv4_DATA->Tx_List.Ready.End=Null;

	P_IPv4_DATA->Tx_List.Pend.Head=Null;
	P_IPv4_DATA->Tx_List.Pend.End=Null;

	P_IPv4_DATA->Rx_List.Count=0;
	P_IPv4_DATA->Rx_List.List.Head=Null;
	P_IPv4_DATA->Rx_List.List.End=Null;



	P_IPv4_DATA->ID=0;
	P_IPv4_DATA->Semaphore_Tx_Ready=Semaphore_Create(Null,0,Net_IPv4_Semaphore_Tx_Ready_Max_Count,Event_Queue_Option_Priority);
	P_IPv4_DATA->Mutex_Tx=Mutex_Create(Null,Event_Queue_Option_Priority);

	P_IPv4_DATA->Time_10MS=Timer_Register(IPv4_Time_10MS,P_Net_Node);
	Timer_Start(P_IPv4_DATA->Time_10MS,0,10,Timer_Operation_Loop_Cycle);

	P_IPv4_DATA->Mutex_Rx=Mutex_Create(Null,Event_Queue_Option_Priority);

	P_IPv4_DATA->Time_1S=Timer_Register(IPv4_Time_1S,P_Net_Node);
	Timer_Start(P_IPv4_DATA->Time_1S,0,1000,Timer_Operation_Loop_Cycle);

	return Error_OK;
}

void IPv4_Time_10MS(void *Args)
{
	Net_Core_Device_Node_Type *P_Net_Node=Args;

	if(P_Net_Node==Null)
	{
		return ;
	}

	Protocol_TCP_ISN_10MS(&P_Net_Node->Protocol_TCP_DATA);


	if(Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Tx,-1)!=Error_OK)
	{
		return ;
	}

	Net_IPv4_Tx_Node_Type *P_Tx_Node=Null,*P_Tx_Node_LAST=Null,*P_Tx_Node_NEXT=Null;

	P_Tx_Node=P_Net_Node->IPv4_DATA.Tx_List.Pend.Head;

	while(P_Tx_Node!=Null)
	{
		P_Tx_Node_NEXT=P_Tx_Node->NEXT;

		P_Tx_Node->TimeOut=P_Tx_Node->TimeOut-10;

		if(P_Tx_Node->TimeOut<=0)
		{
			P_Tx_Node->Count--;
			if(P_Tx_Node->Count>0)
			{
				P_Tx_Node->TimeOut=Net_IPv4_Pend_Max_Time;

				//
				if(IPv4_ARP_Reqst(P_Net_Node,P_Tx_Node->Target_IPv4_Address)!=Error_OK)
				{
					//while(1);
					break;//
				}
			}
		}

		if(P_Tx_Node->TimeOut<=0 && P_Tx_Node->Count<=0)
		{
			if(P_Tx_Node_LAST==Null)
			{
				P_Net_Node->IPv4_DATA.Tx_List.Pend.Head=P_Tx_Node_NEXT;
			}
			else
			{
				P_Tx_Node_LAST->NEXT=P_Tx_Node_NEXT;
			}
			if(P_Tx_Node==P_Net_Node->IPv4_DATA.Tx_List.Pend.End || P_Tx_Node_NEXT==Null)
			{
				P_Net_Node->IPv4_DATA.Tx_List.Pend.End=P_Tx_Node_LAST;
			}
			P_Tx_Node->NEXT=Null;

			if(P_Tx_Node->DATA_Free!=false)
			{
				Memory_Free(P_Tx_Node->DATA);
				P_Tx_Node->DATA=Null;
			}

			Memory_Free(P_Tx_Node);


			P_Tx_Node=P_Tx_Node_NEXT;


		}
		else
		{
			P_Tx_Node_LAST=P_Tx_Node;
			P_Tx_Node=P_Tx_Node_NEXT;
		}

	}



	Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Tx);
}

void IPv4_Time_1S(void *Args)
{
	Net_Core_Device_Node_Type *P_Net_Node=Args;

	if(P_Net_Node==Null)
	{
		return ;
	}

	if(Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Rx,-1)!=Error_OK)
	{
		return ;
	}

	Net_IPv4_Rx_Node_Type *P_Rx_Node=Null;

IPv4_Time_1S_S:

	P_Rx_Node=P_Net_Node->IPv4_DATA.Rx_List.List.Head;

	if(P_Rx_Node!=Null)
	{
		P_Rx_Node->TimeOut_1S--;

		if(P_Rx_Node->TimeOut_1S<=0)
		{
			P_Net_Node->IPv4_DATA.Rx_List.List.Head=P_Rx_Node->NEXT;


			Net_IPv4_Rx_Node_Data_Type *P_Rx_Node_Data=P_Rx_Node->DATA,*P_Rx_Node_Data_NEXT=Null;

			while(P_Rx_Node_Data!=Null)
			{
				P_Rx_Node_Data_NEXT=P_Rx_Node_Data->NEXT;

				Memory_Free(P_Rx_Node_Data->DATA);
				Memory_Free(P_Rx_Node_Data);

				P_Rx_Node_Data=P_Rx_Node_Data_NEXT;
			}

			Memory_Free(P_Rx_Node);
			P_Rx_Node=Null;
			P_Net_Node->IPv4_DATA.Rx_List.Count--;

			if(P_Net_Node->IPv4_DATA.Rx_List.List.Head==Null)//下一个还有
			{
				P_Net_Node->IPv4_DATA.Rx_List.List.End=Null;
			}
			else
			{
				if(P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S<=0)
				{
					goto IPv4_Time_1S_S;
				}
			}

		}


	}



	Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Rx);
}

void IPv4_Handle_Tx(Net_Core_Device_Node_Type *P_Net_Node)
{
	if(P_Net_Node==Null)
	{
		return ;
	}

	int Err=Semaphore_Wait(P_Net_Node->IPv4_DATA.Semaphore_Tx_Ready,-1);

	if(Err==Error_OK)
	{
		Net_IPv4_Tx_Node_Type *P_Tx_Node=Null;


		//
		Err=Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Tx,-1);
		if(Err!=Error_OK)
		{
			return ;
		}

		P_Tx_Node=P_Net_Node->IPv4_DATA.Tx_List.Ready.Head;

		if(P_Tx_Node!=Null)
		{
			P_Net_Node->IPv4_DATA.Tx_List.Ready.Head=P_Tx_Node->NEXT;
			if(P_Net_Node->IPv4_DATA.Tx_List.Ready.Head==Null)
			{
				P_Net_Node->IPv4_DATA.Tx_List.Ready.End=Null;
			}
			P_Tx_Node->NEXT=Null;

		}
		//
		Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Tx);


		if(P_Tx_Node!=Null)
		{
			IPv4_Tx_Send_DATA(
					P_Net_Node,
					P_Tx_Node->Target_MAC_Address,
					P_Tx_Node->Protocol,
					P_Tx_Node->DEST_IPv4_Address,
					P_Tx_Node->DATA,
					P_Tx_Node->Size);
			if(P_Tx_Node->DATA_Free!=false)
			{
				Memory_Free(P_Tx_Node->DATA);
				P_Tx_Node->DATA=Null;
			}
			Memory_Free(P_Tx_Node);
			P_Tx_Node=Null;
		}

	}
	else
	{
		Scheduling_Sleep_Task(10);
	}


}
void IPv4_Handle_Rx(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Packet)
{
	Net_IPv4_Packet_Heade_Type *P_IPv4_Packet_Heade=(Net_IPv4_Packet_Heade_Type *)IPv4_Packet;

	if(P_Net_Node==Null || P_IPv4_Packet_Heade==Null)
	{
		return ;
	}

	if(P_IPv4_Packet_Heade->Version!=4 || P_IPv4_Packet_Heade->Header_Length!=5)
	{
		return ;
	}

	if(Net_Core_CheckSum((uint16_t *)P_IPv4_Packet_Heade,sizeof(Net_IPv4_Packet_Heade_Type),Null,0)!=UINT16_REVERSE(0x0000))
	{
		return ;
	}

	if(memcmp(P_IPv4_Packet_Heade->DEST_IPv4_Address,Net_IPv4_Adress_0x00,4)==0)
	{
		return ;
	}
	else if(memcmp(P_IPv4_Packet_Heade->DEST_IPv4_Address,Net_IPv4_Adress_0x7F,4)==0)
	{
		return ;
	}
	else if(memcmp(P_IPv4_Packet_Heade->DEST_IPv4_Address,Net_IPv4_Adress_0xFF,4)==0)
	{
		//return ;
	}
	else if(memcmp(P_IPv4_Packet_Heade->DEST_IPv4_Address,P_Net_Node->IP_Config.IPv4.IP_Address,4)==0)
	{
		;
	}
	else
	{
		return ;
	}

	uint16_t IPv4_DATA_Size=UINT16_REVERSE(P_IPv4_Packet_Heade->Total_Length)-sizeof(Net_IPv4_Packet_Heade_Type);


	uint8_t *Out_DATA;
	uint16_t Out_Size=0;

	int Err=IPv4_Rx_Packing_DATA(
				P_Net_Node,
				P_IPv4_Packet_Heade,
				&IPv4_Packet[sizeof(Net_IPv4_Packet_Heade_Type)],
				IPv4_DATA_Size,
				&Out_DATA,
				&Out_Size);


	if(Err<Error_OK)
	{
		return ;
	}
	else if(Err==Net_IPv4_Packet_Null)
	{
		return ;
	}
	else if(Err==Net_IPv4_Packet_Single)
	{

	}
	else if(Err==Net_IPv4_Packet_Multi)
	{

	}
	else
	{
		return ;
	}



	switch(P_IPv4_Packet_Heade->Protocol)
	{
		case Net_IPv4_Protocol_ICMP:
		{
			IPv4_ICMP_Handle_Rx(
					P_Net_Node,
					P_IPv4_Packet_Heade->SRC_IPv4_Address,
					Out_DATA,
					Out_Size);
		}break;
		case Net_IPv4_Protocol_TCP:
		{
			Net_IPv4_Packet_Pseudo_Heade_Type Net_IPv4_TCP_Pseudo_Heade;

			if(IPv4_Pseudo_Heade_Init(
					&Net_IPv4_TCP_Pseudo_Heade,
					P_IPv4_Packet_Heade->SRC_IPv4_Address,
					P_IPv4_Packet_Heade->DEST_IPv4_Address,
					P_IPv4_Packet_Heade->Protocol,
					Out_Size)==Error_OK)
			{
				Protocol_TCP_Handle_Rx(
						P_Net_Node,
						&Net_IPv4_TCP_Pseudo_Heade,
						Out_DATA,
						Out_Size);
			}
		}break;
		case Net_IPv4_Protocol_UDP:
		{
			Net_IPv4_Packet_Pseudo_Heade_Type Net_IPv4_UDP_Pseudo_Heade;

			if(IPv4_Pseudo_Heade_Init(
					&Net_IPv4_UDP_Pseudo_Heade,
					P_IPv4_Packet_Heade->SRC_IPv4_Address,
					P_IPv4_Packet_Heade->DEST_IPv4_Address,
					P_IPv4_Packet_Heade->Protocol,
					Out_Size)==Error_OK)
			{
				Protocol_UDP_Handle_Rx(
						P_Net_Node,
						&Net_IPv4_UDP_Pseudo_Heade,
						Out_DATA,
						Out_Size);
			}

		}break;
		default:
		{

		}break;
	}

	if(Err==Net_IPv4_Packet_Multi)
	{
		if(Out_DATA!=Null)
		{
			Memory_Free(Out_DATA);
		}
	}


}

int IPv4_Tx_Add_Node(Net_IPv4_Tx_List_Type *P_List,Net_IPv4_Tx_Node_Type *P_Tx_Node)
{
	if(P_List==Null || P_Tx_Node==Null)
	{
		return Error_Invalid_Parameter;
	}

	if(P_List->End==Null)
	{
		P_List->Head=P_Tx_Node;
	}
	else
	{
		P_List->End->NEXT=P_Tx_Node;
	}
	P_List->End=P_Tx_Node;
	P_Tx_Node->NEXT=Null;

	return Error_OK;
}

int IPv4_Tx_Check_Pend_List(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address,uint8_t *Mac_Address)
{
	if(P_Net_Node==Null || IPv4_Address==Null || Mac_Address==Null)
	{
		return Error_Invalid_Parameter;
	}

	int Err=Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Tx,-1);

	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_IPv4_Tx_Node_Type *P_Tx_Node=Null,*P_Tx_Node_LAST=Null,*P_Tx_Node_NEXT=Null;

	P_Tx_Node=P_Net_Node->IPv4_DATA.Tx_List.Pend.Head;

	while(P_Tx_Node!=Null)
	{
		P_Tx_Node_NEXT=P_Tx_Node->NEXT;

		if(memcmp(P_Tx_Node->Target_IPv4_Address,IPv4_Address,4)==0)
		{
			if(P_Tx_Node_LAST==Null)
			{
				P_Net_Node->IPv4_DATA.Tx_List.Pend.Head=P_Tx_Node_NEXT;
			}
			else
			{
				P_Tx_Node_LAST->NEXT=P_Tx_Node_NEXT;
			}
			if(P_Tx_Node==P_Net_Node->IPv4_DATA.Tx_List.Pend.End || P_Tx_Node_NEXT==Null)
			{
				P_Net_Node->IPv4_DATA.Tx_List.Pend.End=P_Tx_Node_LAST;
			}
			P_Tx_Node->NEXT=Null;

			memcpy(P_Tx_Node->Target_MAC_Address,Mac_Address,6);

			Err=IPv4_Tx_Add_Node(&P_Net_Node->IPv4_DATA.Tx_List.Ready,P_Tx_Node);

			if(Err==Error_OK)
			{
				Semaphore_Release(P_Net_Node->IPv4_DATA.Semaphore_Tx_Ready,1,Null);
			}
			else
			{
				if(P_Tx_Node->DATA_Free!=false)
				{
					Memory_Free(P_Tx_Node->DATA);
					P_Tx_Node->DATA=Null;
				}

				Memory_Free(P_Tx_Node);

				break;
			}

			P_Tx_Node=P_Tx_Node_NEXT;


		}
		else
		{
			P_Tx_Node_LAST=P_Tx_Node;
			P_Tx_Node=P_Tx_Node_NEXT;
		}

	}



	Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Tx);

	return Err;

}

int IPv4_Tx_Check_IPv4_Segment(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *IPv4_Address)
{
	if(P_Net_Node==Null || IPv4_Address==Null)
	{
		return Error_Invalid_Parameter;
	}
	if(memcmp(Net_IPv4_Adress_0x00,IPv4_Address,4)==0)
	{
		return Net_IPv4_Address_Type_Invalid;
	}
	else if(memcmp(Net_IPv4_Adress_0x7F,IPv4_Address,4)==0)
	{
		return Net_IPv4_Address_Type_SelfAddress;
	}
	else if(memcmp(Net_IPv4_Adress_0xFF,IPv4_Address,4)==0)
	{
		return Net_IPv4_Address_Type_BroadcastAddress;
	}

	for(int i=0;i<4;i++)
	{

		if((P_Net_Node->IP_Config.IPv4.IP_Address[i]&P_Net_Node->IP_Config.IPv4.SubNet_Mask[i])!=(IPv4_Address[i]&P_Net_Node->IP_Config.IPv4.SubNet_Mask[i]))
		{
			return Net_IPv4_Address_Type_ExtraNet;
		}

	}
	return Net_IPv4_Address_Type_SubNet;

}

int IPv4_Tx_Pend_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t Protocol,
		uint8_t *DEST_IPv4_Address,
		uint8_t *DATA,
		uint16_t Size,
		bool DATA_Free)
{
	if(P_Net_Node==Null
	|| (Protocol!=Net_IPv4_Protocol_ICMP && Protocol!=Net_IPv4_Protocol_TCP && Protocol!=Net_IPv4_Protocol_UDP)
	|| DEST_IPv4_Address==Null
	|| DATA==Null
	|| Size==0
	|| DATA_Free>=bool_End)
	{
		return Error_Invalid_Parameter;
	}



	Net_IPv4_Tx_Node_Type *P_Tx_Node=Memory_Malloc(sizeof(Net_IPv4_Tx_Node_Type));

	if(P_Tx_Node==Null)
	{
		return Error_Allocation_Memory_Failed;

	}

	int Err=IPv4_Tx_Check_IPv4_Segment(P_Net_Node,DEST_IPv4_Address);

	if(Err<Error_OK)
	{
		goto IPv4_Tx_Pend_Exit2;
	}

	if(Err==Net_IPv4_Address_Type_Invalid)
	{
		Err=Error_Invalid_Parameter;
		goto IPv4_Tx_Pend_Exit2;
	}
	else if(Err==Net_IPv4_Address_Type_SubNet || Err==Net_IPv4_Address_Type_BroadcastAddress)
	{
		memcpy(P_Tx_Node->Target_IPv4_Address,DEST_IPv4_Address,4);
	}
	else if(Err==Net_IPv4_Address_Type_ExtraNet)
	{
		memcpy(P_Tx_Node->Target_IPv4_Address,P_Net_Node->IP_Config.IPv4.Default_Gateway,4);
	}
	else if(Err==Net_IPv4_Address_Type_SelfAddress)
	{
		//TODO 这个地方有点特殊 回环地址 需要后期实现
		Err=Error_Invalid_Parameter;
		goto IPv4_Tx_Pend_Exit2;
	}
	else
	{
		Err=Error_Unknown;
		goto IPv4_Tx_Pend_Exit2;
	}

//	if(Err==false)
//	{
//		memcpy(P_Tx_Node->Target_IPv4_Address,P_Net_Node->IP_Config.IPv4.Default_Gateway,4);
//	}
//	else
//	{
//		memcpy(P_Tx_Node->Target_IPv4_Address,DEST_IPv4_Address,4);
//	}


	P_Tx_Node->Protocol=Protocol;
	memcpy(P_Tx_Node->DEST_IPv4_Address,DEST_IPv4_Address,4);
	P_Tx_Node->DATA=DATA;
	P_Tx_Node->Size=Size;
	P_Tx_Node->DATA_Free=DATA_Free;

	P_Tx_Node->Count=Net_IPv4_Pend_Max_Count;
	P_Tx_Node->TimeOut=Net_IPv4_Pend_Max_Time;


	Err=IPv4_ARP_Cache_Find_Mac_Address(P_Net_Node,P_Tx_Node->Target_IPv4_Address,P_Tx_Node->Target_MAC_Address);

	if(Err<Error_OK)
	{
		goto IPv4_Tx_Pend_Exit2;
	}

	if(Err==false)
	{
		Err=IPv4_ARP_Reqst(P_Net_Node,P_Tx_Node->Target_IPv4_Address);

		if(Err==Error_OK)
		{
			Err=Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Tx,-1);
			if(Err==Error_OK)
			{
				Err=IPv4_Tx_Add_Node(&P_Net_Node->IPv4_DATA.Tx_List.Pend,P_Tx_Node);

				Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Tx);

				if(Err==Error_OK)
				{
					goto IPv4_Tx_Pend_Exit1;
				}
				else
				{
					goto IPv4_Tx_Pend_Exit2;
				}
			}
			else
			{
				goto IPv4_Tx_Pend_Exit2;
			}



		}
		else
		{
			goto IPv4_Tx_Pend_Exit2;
		}

	}
	else
	{
		Err=Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Tx,-1);
		if(Err==Error_OK)
		{
			Err=IPv4_Tx_Add_Node(&P_Net_Node->IPv4_DATA.Tx_List.Ready,P_Tx_Node);

			Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Tx);

			if(Err==Error_OK)
			{
				Semaphore_Release(P_Net_Node->IPv4_DATA.Semaphore_Tx_Ready,1,Null);

				goto IPv4_Tx_Pend_Exit1;
			}
			else
			{
				goto IPv4_Tx_Pend_Exit2;
			}
		}
		else
		{
			goto IPv4_Tx_Pend_Exit2;
		}

	}

IPv4_Tx_Pend_Exit2:
	Memory_Free(P_Tx_Node);

IPv4_Tx_Pend_Exit1:

	return Err;

}




int IPv4_Tx_Send_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *Target_MAC_Address,
		uint8_t Protocol,
		uint8_t *DEST_IPv4_Address,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Net_Node==Null
	|| (Protocol!=Net_IPv4_Protocol_ICMP && Protocol!=Net_IPv4_Protocol_TCP && Protocol!=Net_IPv4_Protocol_UDP)
	|| DEST_IPv4_Address==Null
	|| DATA==Null
	|| Size==0)
	{
		return Error_Invalid_Parameter;
	}

	Net_IPv4_Packet_Heade_Type *P_IPv4_Packet_Heade=Memory_Malloc(sizeof(Net_IPv4_Packet_Heade_Type));

	if(P_IPv4_Packet_Heade==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	P_IPv4_Packet_Heade->Version=4;
	P_IPv4_Packet_Heade->Header_Length=5;
	P_IPv4_Packet_Heade->TOS=0;
	P_IPv4_Packet_Heade->Identification=UINT16_REVERSE(P_Net_Node->IPv4_DATA.ID);
	P_IPv4_Packet_Heade->Protocol=Protocol;
	P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(0x0000);
	P_IPv4_Packet_Heade->TTL=Net_IPv4_Packet_Heade_TTL_Default;
	memcpy(P_IPv4_Packet_Heade->SRC_IPv4_Address,P_Net_Node->IP_Config.IPv4.IP_Address,4);
	memcpy(P_IPv4_Packet_Heade->DEST_IPv4_Address,DEST_IPv4_Address,4);

	P_Net_Node->IPv4_DATA.ID++;

	//int Block;
	int Max_Packet_Size=P_Net_Node->P_OPS->MTU-sizeof(Net_IPv4_Packet_Heade_Type);

	Max_Packet_Size=(Max_Packet_Size/8)*8;

	int Index=0;
	Net_IPv4_Packet_Heade_Flags_Type Flags;
	Flags.DATA=0x0000;

	//TODO 以下部分需要后期UDP测试

	int Err=Error_OK;
	while(Err==Error_OK)
	{
		Flags.Offset=Index/8;

		if((Index+Max_Packet_Size)<Size)
		{
			P_IPv4_Packet_Heade->Total_Length=UINT16_REVERSE(sizeof(Net_IPv4_Packet_Heade_Type)+Max_Packet_Size);
			Flags.MF=1;
			P_IPv4_Packet_Heade->Flags=UINT16_REVERSE(Flags.DATA);

			P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(0x0000);

			uint16_t CheckSum=Net_Core_CheckSum((uint16_t*)P_IPv4_Packet_Heade,sizeof(Net_IPv4_Packet_Heade_Type),Null,0);
			P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(CheckSum);

			Err=Net_Core_Tx_DATA_IPv4(
					P_Net_Node,
					Target_MAC_Address,
					(uint8_t *)P_IPv4_Packet_Heade,
					sizeof(Net_IPv4_Packet_Heade_Type),
					&DATA[Index],
					Max_Packet_Size);
			Index=Index+Max_Packet_Size;

		}
		else if((Index+Max_Packet_Size)==Size)
		{
			P_IPv4_Packet_Heade->Total_Length=UINT16_REVERSE(sizeof(Net_IPv4_Packet_Heade_Type)+Max_Packet_Size);
			Flags.MF=0;
			P_IPv4_Packet_Heade->Flags=UINT16_REVERSE(Flags.DATA);

			P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(0x0000);

			uint16_t CheckSum=Net_Core_CheckSum((uint16_t*)P_IPv4_Packet_Heade,sizeof(Net_IPv4_Packet_Heade_Type),Null,0);
			P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(CheckSum);

			Err=Net_Core_Tx_DATA_IPv4(
					P_Net_Node,
					Target_MAC_Address,
					(uint8_t *)P_IPv4_Packet_Heade,
					sizeof(Net_IPv4_Packet_Heade_Type),
					&DATA[Index],
					Max_Packet_Size);
			break;
		}
		else
		{
			P_IPv4_Packet_Heade->Total_Length=UINT16_REVERSE(sizeof(Net_IPv4_Packet_Heade_Type)+(Size-Index));
			Flags.MF=0;
			P_IPv4_Packet_Heade->Flags=UINT16_REVERSE(Flags.DATA);

			P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(0x0000);

			uint16_t CheckSum=Net_Core_CheckSum((uint16_t*)P_IPv4_Packet_Heade,sizeof(Net_IPv4_Packet_Heade_Type),Null,0);
			P_IPv4_Packet_Heade->Header_CheckSum=UINT16_REVERSE(CheckSum);

			Err=Net_Core_Tx_DATA_IPv4(
					P_Net_Node,
					Target_MAC_Address,
					(uint8_t *)P_IPv4_Packet_Heade,
					sizeof(Net_IPv4_Packet_Heade_Type),
					&DATA[Index],
					(Size-Index));
			break;
		}

	}

	Memory_Free(P_IPv4_Packet_Heade);

	return Err;
}
//这个函数炸了 内容过于复杂
//有必要注释一下 以免回头填坑的时候 找不到头
//Linux中有几个防止攻击的策略 分片不能大于128个 分片占用的空间不能超过256KB
//同一个IP地址的数据包 被分片数量不能超过一定值 有两个计数器 我只是过了一下 并没看什么意思 怎么玩的
//哪天设备被人攻击 或者出现问题 在适当加点策略限制吧
//目前策略就是 限制需要分片的数据包 加上个数 超过则删除 最先到来的数据包 再加上一个等到所有分片到齐的超时等待时间
int IPv4_Rx_Packing_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Heade_Type *P_IPv4_Packet_Heade,
		uint8_t *In_DATA,
		uint16_t In_Size,
		uint8_t **Out_DATA,
		uint16_t *Out_Size)
{
	if(P_Net_Node==Null || P_IPv4_Packet_Heade==Null || In_DATA==Null || In_Size==0)
	{
		return Error_Invalid_Parameter;
	}

	int Err=Error_OK;

	//检查当前数据包是完整的还是一个分片
	Net_IPv4_Packet_Heade_Flags_Type Flags;

	Flags.DATA=UINT16_REVERSE(P_IPv4_Packet_Heade->Flags);
	if(Flags.MF==0 && Flags.Offset==0)//当前数据包是单包
	{
		if(Out_DATA==Null || Out_Size==Null)
		{
			return Error_Invalid_Parameter;
		}
		*Out_DATA=In_DATA;
		*Out_Size=In_Size;

		return Net_IPv4_Packet_Single;
	}

	//开始操作队列 互斥一下 希望时间不要太久
	Err=Mutex_Wait(P_Net_Node->IPv4_DATA.Mutex_Rx,-1);
	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_IPv4_Rx_Node_Type *P_Rx_Node=Null,*P_Rx_Node_LAST=Null,*P_Rx_Node_NEXT=Null;

	P_Rx_Node=P_Net_Node->IPv4_DATA.Rx_List.List.Head;

	//TODO 这个地方查找 确实很费时间 linux里面采用哈希法 我是写了玩玩的 哪天走狗屎运了 在来优化这代码
	//目前就一个个找吧 时间是长了点
	while(P_Rx_Node!=Null)
	{
		P_Rx_Node_NEXT=P_Rx_Node->NEXT;

		//查找数据节点
		if(memcmp(P_Rx_Node->SRC_IPv4_Address,P_IPv4_Packet_Heade->SRC_IPv4_Address,4)==0
		&& memcmp(P_Rx_Node->DEST_IPv4_Address,P_IPv4_Packet_Heade->DEST_IPv4_Address,4)==0
		&& P_Rx_Node->Identification==P_IPv4_Packet_Heade->Identification
		&& P_Rx_Node->Protocol==P_IPv4_Packet_Heade->Protocol)
		{

//			if(P_Rx_Node->DATA==Null)
//			{
//				Err=Error_Unknown;
//				goto IPv4_Rx_Packing_DATA_Exit;
//			}

			Err=IPv4_Rx_Node_Insert_DATA(P_Rx_Node,Flags,In_DATA,In_Size);

//			if(Err==Net_IPv4_Packet_Multi)
//			{
//				Err=Net_IPv4_Packet_Null;
//			}
			if(Err<Error_OK || Err==Net_IPv4_Packet_Multi)//已经收集到所有分片 开始组装所有分片 或者出现了错误
			{
				//将这个节点从队列中删除
				if(P_Rx_Node_LAST==Null || P_Rx_Node==P_Net_Node->IPv4_DATA.Rx_List.List.Head)
				{
					P_Net_Node->IPv4_DATA.Rx_List.List.Head=P_Rx_Node_NEXT;

					if(P_Net_Node->IPv4_DATA.Rx_List.List.Head!=Null)//下一个还有
					{
						//修补时间队列
						if(P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S>=0)
						{
							if(P_Rx_Node->TimeOut_1S>=0)
							{
								P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S=P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S+P_Rx_Node->TimeOut_1S;
							}
						}
					}

				}
				if(P_Rx_Node_NEXT==Null || P_Rx_Node==P_Net_Node->IPv4_DATA.Rx_List.List.End)
				{
					P_Net_Node->IPv4_DATA.Rx_List.List.End=P_Rx_Node_LAST;
					if(P_Net_Node->IPv4_DATA.Rx_List.List.End!=Null)
					{
						P_Net_Node->IPv4_DATA.Rx_List.List.End->NEXT=Null;
					}
				}
				if(P_Rx_Node_LAST!=Null && P_Rx_Node_NEXT!=Null)
				{
					//修补时间队列
					if(P_Rx_Node->TimeOut_1S>=0 && P_Rx_Node_NEXT->TimeOut_1S>=0)
					{
						P_Rx_Node_NEXT->TimeOut_1S=P_Rx_Node_NEXT->TimeOut_1S+P_Rx_Node->TimeOut_1S;
					}
					P_Rx_Node_LAST->NEXT=P_Rx_Node_NEXT;
				}

				uint8_t *P_Temp_DATA=Null;

				if(Err==Net_IPv4_Packet_Multi)
				{
					P_Temp_DATA=Memory_Malloc(P_Rx_Node->Size);
					if(P_Temp_DATA==Null)
					{
						Err=Error_Allocation_Memory_Failed;
					}
					else
					{
						*Out_DATA=P_Temp_DATA;
						*Out_Size=P_Rx_Node->Size;
					}
				}
				Net_IPv4_Rx_Node_Data_Type *P_Rx_Node_Data=P_Rx_Node->DATA,*P_Rx_Node_Data_NEXT=Null;

				while(P_Rx_Node_Data!=Null)
				{
					P_Rx_Node_Data_NEXT=P_Rx_Node_Data->NEXT;

					if(P_Temp_DATA!=Null)
					{
						memcpy(&P_Temp_DATA[P_Rx_Node_Data->Address],P_Rx_Node_Data->DATA,P_Rx_Node_Data->Size);
					}


					Memory_Free(P_Rx_Node_Data->DATA);
					Memory_Free(P_Rx_Node_Data);

					P_Rx_Node_Data=P_Rx_Node_Data_NEXT;
				}

				Memory_Free(P_Rx_Node);
				P_Rx_Node=Null;
			}


			goto IPv4_Rx_Packing_DATA_Exit;


		}
		P_Rx_Node_LAST=P_Rx_Node;

		P_Rx_Node=P_Rx_Node_NEXT;
	}

//没有找到

	//超出队列最大数量
	if(P_Net_Node->IPv4_DATA.Rx_List.Count>Net_IPv4I_DATA_Rx_List_Max_Count)
	{
		//删除队列中超时时间快到的节点

		P_Rx_Node=P_Net_Node->IPv4_DATA.Rx_List.List.Head;

		if(P_Rx_Node!=Null)
		{
			P_Net_Node->IPv4_DATA.Rx_List.List.Head=P_Rx_Node->NEXT;

			if(P_Net_Node->IPv4_DATA.Rx_List.List.Head!=Null)//下一个还有
			{
				//修补时间队列
				if(P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S>=0)
				{
					if(P_Rx_Node->TimeOut_1S>=0)
					{
						P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S=P_Net_Node->IPv4_DATA.Rx_List.List.Head->TimeOut_1S+P_Rx_Node->TimeOut_1S;
					}
				}
			}
			else
			{
				P_Net_Node->IPv4_DATA.Rx_List.List.End=Null;
			}

			Net_IPv4_Rx_Node_Data_Type *P_Rx_Node_Data=P_Rx_Node->DATA,*P_Rx_Node_Data_NEXT=Null;

			while(P_Rx_Node_Data!=Null)
			{
				P_Rx_Node_Data_NEXT=P_Rx_Node_Data->NEXT;

				Memory_Free(P_Rx_Node_Data->DATA);
				Memory_Free(P_Rx_Node_Data);

				P_Rx_Node_Data=P_Rx_Node_Data_NEXT;
			}

			Memory_Free(P_Rx_Node);
			P_Rx_Node=Null;
			P_Net_Node->IPv4_DATA.Rx_List.Count--;
		}

	}


	//添加一个新节点
	P_Rx_Node=Memory_Malloc(sizeof(Net_IPv4_Rx_Node_Type));

	if(P_Rx_Node==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto IPv4_Rx_Packing_DATA_Exit;
	}

	if((Err=IPv4_Rx_Node_DATA_Add(&P_Rx_Node->DATA,Flags.Offset*8,In_DATA,In_Size))!=Error_OK)
	{
		Memory_Free(P_Rx_Node);
		Err=Error_Allocation_Memory_Failed;
		goto IPv4_Rx_Packing_DATA_Exit;
	}
	memcpy(P_Rx_Node->SRC_IPv4_Address,P_IPv4_Packet_Heade->SRC_IPv4_Address,4);
	memcpy(P_Rx_Node->DEST_IPv4_Address,P_IPv4_Packet_Heade->DEST_IPv4_Address,4);
	P_Rx_Node->Protocol=P_IPv4_Packet_Heade->Protocol;
	P_Rx_Node->Head_Part=0;
	P_Rx_Node->End_Part=0;
	P_Rx_Node->TimeOut_1S=Net_IPv4_Rx_Multi_Wait_Tims_1S;

	P_Rx_Node->Identification=P_IPv4_Packet_Heade->Identification;
	P_Rx_Node->Size=0;
	P_Rx_Node->Len=In_Size;

	if(Flags.Offset==0)//当前分片是否为第一个
	{
		P_Rx_Node->Head_Part=1;
	}

	if(Flags.MF==0)//当前分片是否为最后一个
	{
		P_Rx_Node->Size=Flags.Offset*8+In_Size;
		P_Rx_Node->End_Part=1;
	}

	P_Net_Node->IPv4_DATA.Rx_List.Count++;

	//添加新节点到队列中

	Net_IPv4_Rx_Node_Type *P_Temp_Rx_Node=Null;

	P_Temp_Rx_Node=P_Net_Node->IPv4_DATA.Rx_List.List.Head;

	//时间差分队列
	while(P_Temp_Rx_Node!=Null)
	{
		P_Rx_Node->TimeOut_1S=P_Rx_Node->TimeOut_1S-P_Temp_Rx_Node->TimeOut_1S;

		P_Temp_Rx_Node=P_Temp_Rx_Node->NEXT;
	}
	if(P_Rx_Node->TimeOut_1S<0)
	{
		P_Rx_Node->TimeOut_1S=0;
	}

	if(P_Net_Node->IPv4_DATA.Rx_List.List.End==Null)
	{
		P_Net_Node->IPv4_DATA.Rx_List.List.Head=P_Rx_Node;
	}
	else
	{
		P_Net_Node->IPv4_DATA.Rx_List.List.End->NEXT=P_Rx_Node;
	}
	P_Net_Node->IPv4_DATA.Rx_List.List.End=P_Rx_Node;
	P_Rx_Node->NEXT=Null;


IPv4_Rx_Packing_DATA_Exit:

	Mutex_Release(P_Net_Node->IPv4_DATA.Mutex_Rx);

	return Err;
}

int IPv4_Rx_Node_DATA_Add(
		Net_IPv4_Rx_Node_Data_Type **P_Rx_Node_DATA,
		uint16_t Address,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Rx_Node_DATA==Null || DATA==Null || Size==0)
	{
		return Error_Invalid_Parameter;
	}
	Net_IPv4_Rx_Node_Data_Type *P_Temp_Node_DATA=Memory_Malloc(sizeof(Net_IPv4_Rx_Node_Data_Type));

	if(P_Temp_Node_DATA==Null)
	{
		return Error_Allocation_Memory_Failed;
	}
	P_Temp_Node_DATA->DATA=Memory_Malloc(Size);
	if(P_Temp_Node_DATA->DATA==Null)
	{
		Memory_Free(P_Temp_Node_DATA);
		return Error_Allocation_Memory_Failed;
	}
	memcpy(P_Temp_Node_DATA->DATA,DATA,Size);

	P_Temp_Node_DATA->Address=Address;
	P_Temp_Node_DATA->Size=Size;
	P_Temp_Node_DATA->NEXT=Null;

	*P_Rx_Node_DATA=P_Temp_Node_DATA;

	return Error_OK;
}

//这个IP组包函数 有点复杂
//这个函数在写的过程中 不知不觉被优化了一次 注释已经不是原来可能表达的意思了 所以不要被注释误导
//Linux中防止攻击 采用一个手段是限制分片个数不能大于128个 我这没有实现
int IPv4_Rx_Node_Insert_DATA(
		Net_IPv4_Rx_Node_Type *P_Rx_Node,
		Net_IPv4_Packet_Heade_Flags_Type Flags,
		uint8_t *DATA,
		uint16_t Size)
{
	if(P_Rx_Node==Null || DATA==Null || Size==0)
	{
		return Error_Invalid_Parameter;
	}
	uint16_t Index=Flags.Offset*8;//根据偏移量计算分片地址

	int Err=Error_OK;

	Net_IPv4_Rx_Node_Data_Type *P_Rx_Node_DATA=Null,*P_Rx_Node_DATA_LAST=Null,*P_Rx_Node_DATA_NEXT=Null,*P_Temp_Node_DATA=Null;

	P_Rx_Node_DATA=P_Rx_Node->DATA;

	if(P_Rx_Node->DATA==Null)
	{
		return Error_Unknown;
		//goto IPv4_Rx_Node_Insert_DATA_Exit;
	}

	if(Index==0)//当前分片是否为第一个
	{
		P_Rx_Node->Head_Part=1;
	}

	if(Flags.MF==0)//当前分片是否为最后一个
	{
		P_Rx_Node->Size=Index+Size;
		P_Rx_Node->End_Part=1;
	}
	uint16_t Node_DATA_Size=0;

	//查找数据块
	while(P_Rx_Node_DATA!=Null)
	{
		P_Rx_Node_DATA_NEXT=P_Rx_Node_DATA->NEXT;

		if(Index==P_Rx_Node_DATA->Address)//收到的数据块和队列中数据有重叠
		{
			if(Size<=P_Rx_Node_DATA->Size)//收到的数据完全和队列中重叠则丢弃
			{
				goto IPv4_Rx_Node_Insert_DATA_Exit_1;
			}
			else//还多出一部分 不是重叠的
			{
				DATA=&DATA[P_Rx_Node_DATA->Size];
				Size=Size-P_Rx_Node_DATA->Size;
				Index=P_Rx_Node_DATA->Address+P_Rx_Node_DATA->Size;

				continue;
			}
		}
		else //队列第一个数据块
		{
			if(Index<P_Rx_Node_DATA->Address)//收到的数据块在队列第一个数据块之前
			{

				if((Index+Size)<=P_Rx_Node_DATA->Address)//收到的数据块完整保存
				{
					Node_DATA_Size=Size;
					Size=0;
				}
				else//收到的数据块有重叠部分 先保存不重叠部分
				{
					Node_DATA_Size=P_Rx_Node_DATA->Address-Index;
					Size=Size-Node_DATA_Size;
				}

				if((Err=IPv4_Rx_Node_DATA_Add(&P_Temp_Node_DATA,Index,DATA,Node_DATA_Size))!=Error_OK)
				{
					goto IPv4_Rx_Node_Insert_DATA_Exit;
				}

				P_Temp_Node_DATA->NEXT=P_Rx_Node_DATA;

				if(P_Rx_Node_DATA_LAST==Null)
				{
					P_Rx_Node->DATA=P_Temp_Node_DATA;
				}
				else
				{
					P_Rx_Node_DATA_LAST->NEXT=P_Temp_Node_DATA;
				}
				P_Rx_Node->Len=P_Rx_Node->Len+Node_DATA_Size;//累加收到的数据个数


				if(Size==0)
				{
					goto IPv4_Rx_Node_Insert_DATA_Exit_1;
				}
				else
				{
					DATA=&DATA[Node_DATA_Size];
					Index=Index+Node_DATA_Size;

					P_Rx_Node_DATA=P_Temp_Node_DATA;

					continue;
				}
			}
			else if(P_Rx_Node_DATA->Address<Index)//收到的数据块在队列中第一个数据块之后
			{
				if(Index<(P_Rx_Node_DATA->Address+P_Rx_Node_DATA->Size))//且在队列中第一个数据块中
				{
					if((Index+Size)<=(P_Rx_Node_DATA->Address+P_Rx_Node_DATA->Size))//收到的数据块完全重叠队列中数据块
					{
						goto IPv4_Rx_Node_Insert_DATA_Exit_1;
					}
					else//去除重叠部分
					{
						Node_DATA_Size=(P_Rx_Node_DATA->Address+P_Rx_Node_DATA->Size)-Index;

						DATA=&DATA[Node_DATA_Size];

						Size=Size-Node_DATA_Size;

						Index=P_Rx_Node_DATA->Address+P_Rx_Node_DATA->Size;

						continue;
					}
				}
				else//不在队列中的第一个数据块中
				{

					if(P_Rx_Node_DATA_NEXT==Null)//队列后面没有数据块 则保存整个收到数据
					{

						if((Err=IPv4_Rx_Node_DATA_Add(&P_Temp_Node_DATA,Index,DATA,Size))!=Error_OK)
						{
							goto IPv4_Rx_Node_Insert_DATA_Exit;
						}

						P_Rx_Node_DATA->NEXT=P_Temp_Node_DATA;
						P_Temp_Node_DATA->NEXT=Null;

						P_Rx_Node->Len=P_Rx_Node->Len+Size;//累加收到的数据个数

						goto IPv4_Rx_Node_Insert_DATA_Exit_1;

					}
					else//队列后面还有数据块
					{
						if(Index<P_Rx_Node_DATA_NEXT->Address)//在后面数据之前
						{
							if((Index+Size)<=P_Rx_Node_DATA_NEXT->Address)//收到的数据块完整保存
							{
								Node_DATA_Size=Size;
								Size=0;
							}
							else//收到的数据块有重叠部分 先保存不重叠部分
							{
								Node_DATA_Size=P_Rx_Node_DATA_NEXT->Address-Index;
								Size=Size-Node_DATA_Size;
							}

							if((Err=IPv4_Rx_Node_DATA_Add(&P_Temp_Node_DATA,Index,DATA,Node_DATA_Size))!=Error_OK)
							{
								goto IPv4_Rx_Node_Insert_DATA_Exit;
							}

							P_Temp_Node_DATA->NEXT=P_Rx_Node_DATA_NEXT;
							P_Rx_Node_DATA->NEXT=P_Temp_Node_DATA;

							P_Rx_Node->Len=P_Rx_Node->Len+Node_DATA_Size;//累加收到的数据个数


							if(Size==0)
							{
								goto IPv4_Rx_Node_Insert_DATA_Exit_1;
							}
							else
							{
								DATA=&DATA[Node_DATA_Size];
								Index=Index+Node_DATA_Size;

								continue;
							}
						}
						else
						{
							;
						}
					}

				}
			}
		}


		P_Rx_Node_DATA_LAST=P_Rx_Node_DATA;

		P_Rx_Node_DATA=P_Rx_Node_DATA_NEXT;
	}
IPv4_Rx_Node_Insert_DATA_Exit_1:

	//抄自Linux代码的一段话条件 满足三个条件 则认为是完整的一个数据包
	if(P_Rx_Node->Head_Part==1 && P_Rx_Node->End_Part==1 && P_Rx_Node->Size==P_Rx_Node->Len)
	{
		Err=Net_IPv4_Packet_Multi;
	}
	else
	{
		Err=Net_IPv4_Packet_Null;
	}

IPv4_Rx_Node_Insert_DATA_Exit:

	return Err;

}


int IPv4_Pseudo_Heade_Init(
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *SRC_IPv4_Address,
		uint8_t *DEST_IPv4_Address,

		uint8_t Protocol,

		uint16_t Length)
{
	if(P_Pseudo_Heade==Null || SRC_IPv4_Address==Null || DEST_IPv4_Address==Null)
	{
		return Error_Invalid_Parameter;
	}
	memcpy(P_Pseudo_Heade->SRC_IPv4_Address,SRC_IPv4_Address,4);
	memcpy(P_Pseudo_Heade->DEST_IPv4_Address,DEST_IPv4_Address,4);
	P_Pseudo_Heade->mbz=0;
	P_Pseudo_Heade->Protocol=Protocol;

	P_Pseudo_Heade->Length=UINT16_REVERSE(Length);

	return Error_OK;
}


