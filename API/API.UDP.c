/*
 * API.UDP.c
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */
#include "string.h"
#include "List.h"
#include "Define.h"
#include "Error.h"
#include "Net.Error.h"
#include "API.h"

#include "Core/Core.h"

#include "Net.IPv4.Define.h"
#include "API.UDP.Struct.h"
#include "API.UDP.h"

#include "Protocol/Protocol.PORT.h"
#include "Protocol/Protocol.UDP.h"


Net_API_UDP_DATA_Type Net_API_UDP_DATA;

int Net_UDP_Init(void)
{
	Net_API_UDP_DATA.Head=Null;
	Net_API_UDP_DATA.End=Null;

	return Error_OK;
}

//创建一个连接
int Net_UDP_Create(
		const char *Device_Name,
		uint32_t Rx_Buffer_Max_Length)
{
	int Err;
	if(Device_Name==Null || Rx_Buffer_Max_Length==0)
	{
		//TODO 这个参数为NULL还未实现
		return Error_Invalid_Parameter;
	}

	Net_Core_Device_Node_Type *P_Device_Node=Null;


	if(Device_Name!=Null)
	{
		List_Find_Node_From_CharName(
				Net_Core_DATA.Device_Node_List.Head,
				NEXT,
				P_OPS->Device_Name,
				Device_Name,
				P_Device_Node);

		if(P_Device_Node==Null)
		{
			return Error_Invalid_Parameter;
		}
	}
	int Handle;

	Error_Args_Return(Handle,Handle_New());


	Net_API_UDP_Node_Type *P_UDP_Node=Memory_Malloc(sizeof(Net_API_UDP_Node_Type));
	if(P_UDP_Node==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Net_UDP_Create_Exit;
	}

	//申请一个互斥信号
	Error_Args(Err,Mutex_Create(Null,Event_Queue_Option_Priority))
	{
		goto Net_UDP_Create_Exit1;
	}
	P_UDP_Node->Rx_Buffer.Mutex=Err;

	//申请一个数据接收缓冲区有数据的事件
	Error_Args(Err,Semaphore_Create(Null,0,1,Event_Queue_Option_Priority))
	{
		goto Net_UDP_Create_Exit2;
	}
	P_UDP_Node->Rx_Buffer.Semaphore=Err;

	//初始化缓冲区
	P_UDP_Node->Rx_Buffer.Buff.Max_Length=Rx_Buffer_Max_Length;
	P_UDP_Node->Rx_Buffer.Buff.Count=0;
	P_UDP_Node->Rx_Buffer.Buff.Size=0;
	P_UDP_Node->Rx_Buffer.Buff.Head=Null;
	P_UDP_Node->Rx_Buffer.Buff.End=Null;
	P_UDP_Node->Rx_Buffer.Buff.OverFlow=false;

	//Init
	P_UDP_Node->Handle=Handle;
	P_UDP_Node->Device_Node=P_Device_Node;

	//
	P_UDP_Node->Bind.SET=false;

	P_UDP_Node->Connect.SET=false;

	//将这个创建的link添加到链表中
	List_Add_Node_To_End(Net_API_UDP_DATA.Head,Net_API_UDP_DATA.End,NEXT,P_UDP_Node);


	return Handle;

Net_UDP_Create_Exit3:
	Semaphore_Delete(P_UDP_Node->Rx_Buffer.Semaphore);
Net_UDP_Create_Exit2:
	Mutex_Delete(P_UDP_Node->Rx_Buffer.Mutex);
Net_UDP_Create_Exit1:
	Memory_Free(P_UDP_Node);
Net_UDP_Create_Exit:
	Handle_Free(Handle);
	return Err;
}
//释放一个连接
int Net_UDP_Close(int Handle)
{
	Net_API_UDP_Node_Type *P_UDP_Node=Null;

	List_Del_Node_From_Symbol(del,Net_API_UDP_Node_Type,Net_API_UDP_DATA.Head,Net_API_UDP_DATA.End,NEXT,Handle,Handle,P_UDP_Node);

	if(P_UDP_Node==Null)
	{
		return Error_Invalid_Handle;
	}
	//TODO 未实现


	Handle_Free(Handle);
	Memory_Free(P_UDP_Node);

	return Error_OK;
}
//将连接绑定到指定IP和端口上
int Net_UDP_Bind(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information)
{
	int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}

	if(P_IP_Information==Null)
	{
		return Error_Invalid_Parameter;
	}

	//
	Net_API_UDP_Node_Type *P_UDP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_UDP_DATA.Head,NEXT,Handle,Handle,P_UDP_Node);
	if(P_UDP_Node==Null)
	{
		return Error_Invalid_Handle;
	}


	if(P_IP_Information->IP_Type==Net_IP_Address_IPv4)
	{

		//绑定本地地址0.0.0.0
		if(memcmp(P_IP_Information->IP_Address,Net_IPv4_Adress_LocalHost,Net_IPv4_Adress_Size_Byte)==0)
		{
			//如果没有指定设备
			//那么绑定将会是所有设备
			if(P_UDP_Node->Device_Node==Null)
			{
				//TODO 还未实现
				return Error_Unknown;
			}
			else
			{
				if(P_IP_Information->PORT==0)
				{
					Error_Args_Return(Err,
							Protocol_PORT_New(
									&P_UDP_Node->Device_Node->Protocol_UDP_DATA.PORT_DATA,
									(Net_Protocol_PORT_Node_Flag_Type){.DATA=0},
									Net_Protocol_PORT_Dynamic_Or_Private_Ports_Begin,
									Net_Protocol_PORT_Dynamic_Or_Private_Ports_End));
					P_UDP_Node->Bind.IP_Info.PORT=Err;
				}
				else
				{

					Error_NoArgs_Return(Err,
							Protocol_PORT_Add(
									&P_UDP_Node->Device_Node->Protocol_UDP_DATA.PORT_DATA,
									P_IP_Information->PORT,
									(Net_Protocol_PORT_Node_Flag_Type){.DATA=0})
									);

					P_UDP_Node->Bind.IP_Info.PORT=P_IP_Information->PORT;
				}
				memcpy(P_UDP_Node->Bind.IP_Info.IP_Address,P_IP_Information->IP_Address,Net_IPv4_Adress_Size_Byte);
				P_UDP_Node->Bind.IP_Info.IP_Type=P_IP_Information->IP_Type;
				P_UDP_Node->Bind.SET=true;

			}
		}
		//
		else
		{
			//TODO 还未实现
			return Error_Unknown;
			/*
			if(P_UDP_Node->Device_Node==Null)
			{

			}
			else
			{

			}
			*/
		}



	}
	else if(P_IP_Information->IP_Type==Net_IP_Address_IPv6)
	{
		//TODO IPv6还未实现
		return Error_Undefined;
	}
	else
	{
		return Error_Invalid_Parameter;
	}


	return Error_OK;
}
//连接对方IP和端口
int Net_UDP_Connect(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information)
{
	//int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}

	if(P_IP_Information==Null)
	{
		return Error_Invalid_Parameter;
	}
	if((P_IP_Information->IP_Type!=Net_IP_Address_IPv4 && P_IP_Information->IP_Type!=Net_IP_Address_IPv6)
	|| P_IP_Information->PORT==0)
	{
		return Error_Invalid_Parameter;
	}
	if(P_IP_Information->IP_Type==Net_IP_Address_IPv4)
	{
		if(memcmp(P_IP_Information->IP_Address,Net_IPv4_Adress_LocalHost,Net_IPv4_Adress_Size_Byte)==0)
		{
			return Error_Invalid_Parameter;
		}
	}
	else if(P_IP_Information->IP_Type==Net_IP_Address_IPv6)
	{
		//TODO IPv6还未实现
		return Error_Undefined;
	}

	//
	Net_API_UDP_Node_Type *P_UDP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_UDP_DATA.Head,NEXT,Handle,Handle,P_UDP_Node);
	if(P_UDP_Node==Null)
	{
		return Error_Invalid_Handle;
	}


	memcpy(P_UDP_Node->Connect.IP_Info.IP_Address,P_IP_Information->IP_Address,Net_IPv4_Adress_Size_Byte);
	P_UDP_Node->Connect.IP_Info.IP_Type=P_IP_Information->IP_Type;
	P_UDP_Node->Connect.IP_Info.PORT=P_IP_Information->PORT;
	P_UDP_Node->Connect.SET=true;


	return Error_OK;
}
//关闭这个连接
int Net_UDP_Disconnect(int Handle)
{
	//TODO 未实现
	return Error_OK;
}
//接收数据
int Net_UDP_Recv(
		int Handle,
		Net_API_IP_Information_Type *P_IP_Information,
		uint8_t *DATA,
		uint32_t Size,
		int32_t TimeOut)
{
	int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}

	if(P_IP_Information==Null || DATA==Null || Size==0)
	{
		return Error_Invalid_Parameter;
	}

	//
	Net_API_UDP_Node_Type *P_UDP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_UDP_DATA.Head,NEXT,Handle,Handle,P_UDP_Node);
	if(P_UDP_Node==Null)
	{
		return Error_Invalid_Handle;
	}
	int Len=0;

	Error_NoArgs_Return(Err,Semaphore_Wait(P_UDP_Node->Rx_Buffer.Semaphore,TimeOut));

	//
	Error_NoArgs_Return(Err,Mutex_Wait(P_UDP_Node->Rx_Buffer.Mutex,-1));


	Net_API_UDP_Buff_Type *P_Temp_Buff=P_UDP_Node->Rx_Buffer.Buff.Head;

	if(P_Temp_Buff==Null)
	{
		Err=Error_Unknown;

		goto Net_UDP_Recv_Exit;
	}
	else
	{
		Len=P_Temp_Buff->Size-P_Temp_Buff->Index;

		if(Len<=Size)
		{
			bool OK;
			List_Del_Node_From_Pointer(IN,Net_API_UDP_Buff_Type,P_UDP_Node->Rx_Buffer.Buff.Head,P_UDP_Node->Rx_Buffer.Buff.End,NEXT,P_Temp_Buff,OK);
			if(OK==true)
			{
				P_UDP_Node->Rx_Buffer.Buff.Count--;
				P_UDP_Node->Rx_Buffer.Buff.Size-=P_Temp_Buff->Size;

				if(P_UDP_Node->Rx_Buffer.Buff.Head!=Null)
				{
					Error_NoArgs(Err,Semaphore_Release(P_UDP_Node->Rx_Buffer.Semaphore,1,Null))
					{
						goto Net_UDP_Recv_Exit_1;
					}
				}


				Error_NoArgs(Err,Mutex_Release(P_UDP_Node->Rx_Buffer.Mutex))
				{
					goto Net_UDP_Recv_Exit_1;
				}
			}
			else
			{
				Err=Error_Unknown;

				goto Net_UDP_Recv_Exit;
			}

			memcpy(DATA,&P_Temp_Buff->DATA[P_Temp_Buff->Index],Len);

			Memory_Free(P_Temp_Buff->DATA);
			Memory_Free(P_Temp_Buff);

		}
		else
		{
			Len=Size;
			memcpy(DATA,&P_Temp_Buff->DATA[P_Temp_Buff->Index],Size);
			P_Temp_Buff->Index+=Len;

			Error_NoArgs(Err,Semaphore_Release(P_UDP_Node->Rx_Buffer.Semaphore,1,Null))
			{
				goto Net_UDP_Recv_Exit;
			}

			Error_NoArgs(Err,Mutex_Release(P_UDP_Node->Rx_Buffer.Mutex))
			{
				goto Net_UDP_Recv_Exit;
			}
		}
	}
	return Len;

Net_UDP_Recv_Exit_1:
	Memory_Free(P_Temp_Buff->DATA);
	Memory_Free(P_Temp_Buff);
Net_UDP_Recv_Exit:
	Mutex_Release(P_UDP_Node->Rx_Buffer.Mutex);

	return Err;
}
//发送数据
int Net_UDP_Send(
		int Handle,
		Net_API_IP_Information_Type *P_IP_Information,
		uint8_t *DATA,
		uint32_t Size)
{
	int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}

	if(DATA==Null || Size==0)
	{
		return Error_Invalid_Parameter;
	}

	//
	Net_API_UDP_Node_Type *P_UDP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_UDP_DATA.Head,NEXT,Handle,Handle,P_UDP_Node);
	if(P_UDP_Node==Null)
	{
		return Error_Invalid_Handle;
	}

	Net_IP_Address_Type IP_Type;
	uint8_t *DEST_IP_Address;
	uint16_t DEST_PORT;
	//如果发送 没有目标地址或者没有 Connect连接 则无法发送
	if(P_IP_Information!=Null)
	{

		DEST_IP_Address=P_IP_Information->IP_Address;
		DEST_PORT=P_IP_Information->PORT;
		if(P_IP_Information->IP_Type==Net_IP_Address_IPv4)
		{

			if(memcmp(P_IP_Information->IP_Address,Net_IPv4_Adress_LocalHost,Net_IPv4_Adress_Size_Byte)==0)
			{
				return Error_Invalid_Parameter;
			}
		}
		else if(P_IP_Information->IP_Type==Net_IP_Address_IPv6)
		{

		}
		else
		{
			return Error_Invalid_Parameter;
		}
		IP_Type=P_IP_Information->IP_Type;
	}
	else
	{
		if(P_UDP_Node->Connect.SET==false)
		{
			return Error_Invalid_Parameter;
		}

		DEST_IP_Address=P_UDP_Node->Connect.IP_Info.IP_Address;
		DEST_PORT=P_UDP_Node->Connect.IP_Info.PORT;
		if(P_UDP_Node->Connect.IP_Info.IP_Type==Net_IP_Address_IPv4)
		{

			if(memcmp(P_UDP_Node->Connect.IP_Info.IP_Address,Net_IPv4_Adress_LocalHost,Net_IPv4_Adress_Size_Byte)==0)
			{
				return Error_Invalid_Parameter;
			}
		}
		else if(P_UDP_Node->Connect.IP_Info.IP_Type==Net_IP_Address_IPv6)
		{

		}
		else
		{
			return Error_Unknown;
		}
		IP_Type=P_UDP_Node->Connect.IP_Info.IP_Type;
	}

	if(P_UDP_Node->Device_Node==Null)
	{
		return Error_Unknown;
	}
	uint16_t SRC_PORT;
	if(P_UDP_Node->Bind.SET==false)
	{
		//TODO 自动分配一个端口没有实现这个功能
		return Net_Error_No_Bind;
	}
	SRC_PORT=P_UDP_Node->Bind.IP_Info.PORT;

	Error_NoArgs_Return(Err,Protocol_UDP_Tx(IP_Type, P_UDP_Node->Device_Node, DEST_IP_Address, SRC_PORT, DEST_PORT, DATA, Size));

	return Error_OK;
}
