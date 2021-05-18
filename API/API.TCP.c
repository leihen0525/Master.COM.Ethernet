/*
 * API.TCP.c
 *
 *  Created on: 2021年4月25日
 *      Author: Master.HE
 */
#include "Error.h"
#include "Net.Error.h"
#include "List.h"
#include "Define.h"
#include "API.h"
#include "API.TCP.h"

#include "Core/Core.h"

#include "Protocol/Protocol.PORT.h"
#include "Protocol/Protocol.TCP.Link.h"


Net_API_TCP_DATA_Type Net_API_TCP_DATA;

int Net_TCP_Init(void)
{
	Net_API_TCP_DATA.Head=Null;
	Net_API_TCP_DATA.End=Null;
	return Error_OK;
}

//创建一个连接
int Net_TCP_Create(
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


	Net_API_TCP_Node_Type *P_TCP_Node=Memory_Malloc(sizeof(Net_API_TCP_Node_Type));
	if(P_TCP_Node==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Net_TCP_Create_Exit;
	}


	//
	P_TCP_Node->Handle=Handle;
	P_TCP_Node->Type=Net_API_TCP_Handle_Null;
	P_TCP_Node->Device_Node=P_Device_Node;
	P_TCP_Node->Rx_Buffer_Max_Length=Rx_Buffer_Max_Length;
	P_TCP_Node->Node=Null;


	//将这个创建的link添加到链表中
	List_Add_Node_To_End(Net_API_TCP_DATA.Head,Net_API_TCP_DATA.End,NEXT,P_TCP_Node);

	return Handle;

	/*
Net_TCP_Create_Exit3:
	Semaphore_Delete(P_TCP_Node->Listen_Type.Semaphore);
Net_TCP_Create_Exit2:
	Mutex_Delete(P_TCP_Node->Listen_Type.Mutex);
	*/
Net_TCP_Create_Exit1:
	Memory_Free(P_TCP_Node);
Net_TCP_Create_Exit:
	Handle_Free(Handle);
	return Err;
}


//释放一个连接
int Net_TCP_Close(int Handle)
{
	return Error_OK;
}

//将连接绑定到指定IP和端口上
int Net_TCP_Bind(
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
	Net_API_TCP_Node_Type *P_TCP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_TCP_DATA.Head,NEXT,Handle,Handle,P_TCP_Node);
	if(P_TCP_Node==Null)
	{
		return Error_Invalid_Handle;
	}

	if(P_TCP_Node->Type!=Net_API_TCP_Handle_Null)
	{
		return Error_Unknown;
	}

	if(P_IP_Information->IP_Type==Net_IP_Address_IPv4)
	{

		//绑定本地地址0.0.0.0
		if(memcmp(P_IP_Information->IP_Address,Net_IPv4_Adress_LocalHost,Net_IPv4_Adress_Size_Byte)==0)
		{
			//如果没有指定设备
			//那么绑定将会是所有设备
			if(P_TCP_Node->Device_Node==Null)
			{
				//TODO 还未实现
				return Error_Unknown;
			}
			else
			{
				if(P_IP_Information->PORT==0)
				{
					/*
					Error_Args_Return(Err,
							Protocol_PORT_New(
									&P_TCP_Node->Device_Node->Protocol_TCP_DATA.PORT_DATA,
									(Net_Protocol_PORT_Node_Flag_Type){.DATA=0},
									Net_Protocol_PORT_Dynamic_Or_Private_Ports_Begin,
									Net_Protocol_PORT_Dynamic_Or_Private_Ports_End));
					P_TCP_Node->Bind.IP_Info.PORT=Err;
					*/
					return Error_Unknown;
				}
				else
				{

					Error_NoArgs_Return(Err,
							Protocol_PORT_Add(
									&P_TCP_Node->Device_Node->Protocol_TCP_DATA.PORT_DATA,
									P_IP_Information->PORT,
									(Net_Protocol_PORT_Node_Flag_Type){.DATA=0})
									);

					P_TCP_Node->Bind.IP_Info.PORT=P_IP_Information->PORT;
				}
				memcpy(P_TCP_Node->Bind.IP_Info.IP_Address,P_IP_Information->IP_Address,Net_IPv4_Adress_Size_Byte);
				P_TCP_Node->Bind.IP_Info.IP_Type=P_IP_Information->IP_Type;
				P_TCP_Node->Bind.SET=true;

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
int Net_TCP_Connect(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information)
{





	return Error_OK;
}

//在当前端口上进行监听
int Net_TCP_Listen(
		int Handle,
		uint32_t Backlog)
{
	int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}
	//TODO Backlog=0可以理解关闭监听，此处暂时未实现
	if(Backlog==0)
	{
		return Error_Invalid_Parameter;
	}
	Net_API_TCP_Node_Type *P_TCP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_TCP_DATA.Head,NEXT,Handle,Handle,P_TCP_Node);
	if(P_TCP_Node==Null)
	{
		return Error_Invalid_Handle;
	}

	if(P_TCP_Node->Type!=Net_API_TCP_Handle_Null)
	{
		return Error_Unknown;
	}
	if(P_TCP_Node->Bind.SET!=true)
	{
		return Net_Error_No_Bind;
	}

	P_TCP_Node->Type=Net_API_TCP_Handle_Listen;

	if(P_TCP_Node->Device_Node==Null)
	{
		return Error_Unknown;
	}

	Error_NoArgs_Return(Err,
			Protocol_TCP_Link_Listen_Add(
					&P_TCP_Node->Device_Node->Protocol_TCP_DATA.Link_DATA,
					Handle,
					P_TCP_Node->Bind.IP_Info.IP_Type,
					P_TCP_Node->Bind.IP_Info.PORT,
					Backlog,
					P_TCP_Node->Rx_Buffer_Max_Length,
					&P_TCP_Node->Listen_Node));



	return Error_OK;

}

//接受一个连接
int Net_TCP_Accept(
		int Handle,
		Net_API_IP_Information_Type *P_IP_Information,
		int32_t TimeOut)
{
	int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}
	Net_API_TCP_Node_Type *P_TCP_Node=Null;
	List_Find_Node_From_Symbol(Net_API_TCP_DATA.Head,NEXT,Handle,Handle,P_TCP_Node);
	if(P_TCP_Node==Null)
	{
		return Error_Invalid_Handle;
	}
	if(P_TCP_Node->Type!=Net_API_TCP_Handle_Listen)
	{
		return Net_Error_No_Listen;
	}
	if(P_TCP_Node->Listen_Node==Null || P_TCP_Node->Device_Node==Null)
	{
		return Error_Unknown;
	}
	Error_NoArgs_Return(Err,Semaphore_Wait(P_TCP_Node->Listen_Node->Semaphore, TimeOut));

	Net_Protocol_TCP_Link_Node_Type *Temp_Link_Node=Null;
	List_Del_Node_From_First(P_TCP_Node->Listen_Node->Accept.Head,P_TCP_Node->Listen_Node->Accept.End,NEXT,Temp_Link_Node);
	if(Temp_Link_Node==Null)
	{
		return Error_Unknown;
	}
	P_TCP_Node->Listen_Node->Accept.Count--;
	P_TCP_Node->Listen_Node->Count--;


	Net_API_TCP_Node_Type *P_TCP_Add=Memory_Malloc(sizeof(Net_API_TCP_Node_Type));
	if(P_TCP_Add==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Net_TCP_Accept_Exit;
	}

	Error_Args(P_TCP_Add->Handle,Handle_New())
	{
		Err=P_TCP_Add->Handle;
		goto Net_TCP_Accept_Exit1;
	}
	//

	P_TCP_Add->Type=Net_API_TCP_Handle_Link_Listen;
	P_TCP_Add->Device_Node=P_TCP_Node->Device_Node;
	P_TCP_Add->Rx_Buffer_Max_Length=P_TCP_Node->Rx_Buffer_Max_Length;
	P_TCP_Add->Link_Node=Temp_Link_Node;
	Temp_Link_Node->Handle=P_TCP_Add->Handle;

	P_TCP_Node->Device_Node->Protocol_TCP_DATA.Link_DATA.Link.Count++;
	List_Add_Node_To_End(P_TCP_Node->Device_Node->Protocol_TCP_DATA.Link_DATA.Link.Head,P_TCP_Node->Device_Node->Protocol_TCP_DATA.Link_DATA.Link.End,NEXT,Temp_Link_Node);

	//将这个创建的link添加到链表中
	List_Add_Node_To_End(Net_API_TCP_DATA.Head,Net_API_TCP_DATA.End,NEXT,P_TCP_Add);

	return P_TCP_Add->Handle;

Net_TCP_Accept_Exit1:
	Memory_Free(P_TCP_Add);
Net_TCP_Accept_Exit:
	Memory_Free(Temp_Link_Node);

	return Err;
}

int Net_TCP_Del_Link_Node(int Handle)
{
	int Err;
	if(Handle<Valid_Handle)
	{
		return Error_Invalid_Handle;
	}
	Net_API_TCP_Node_Type *Del_Node=Null;
	List_Del_Node_From_Symbol(Del1,Net_API_TCP_Node_Type,Net_API_TCP_DATA.Head,Net_API_TCP_DATA.End,NEXT,Handle,Handle,Del_Node);

	if(Del_Node==Null)
	{
		return Error_No_Find;
	}
	Handle_Free(Del_Node->Handle);
	Memory_Free(Del_Node);

	return Error_OK;
}

//关闭一个连接的方向
int Net_TCP_Shutdown(
		int Handle,
		bool Shut_Rx,
		bool Shut_Tx)
{
	return Error_OK;
}

//接收数据
int Net_TCP_Recv(
		int Handle,
		uint8_t *DATA,
		uint32_t Size,
		int Flags,
		int32_t TimeOut)
{
	return Error_OK;
}
//发送数据
int Net_TCP_Send(
		int Handle,
		uint8_t *DATA,
		uint32_t Size,
		int Flags)
{
	return Error_OK;
}
