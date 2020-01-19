/*
 * Socket.c
 *
 *  Created on: 2019Äê12ÔÂ19ÈÕ
 *      Author: Master.HE
 */
#include "Error.h"

#include "Core/Core.Struct.h"

#include "Session/Session.Enum.h"
#include "Session/Session.h"

#include "Socket.Struct.h"

#include "Socket.h"

#include "Module\Module.h"

extern Net_Core_DATA_Type Net_Core_DATA;

Net_Socket_DATA_Type Net_Socket_DATA;

int Net_Socket_Init(void)
{
	Net_Socket_DATA.Handle=0;

	int Err;

	if((Err=Session_Init(&Net_Socket_DATA.Session_DATA))!=Error_OK)
	{
		return Err;
	}

	return Error_OK;
}


int Net_Socket(int Domain, int Type, int Protocol)
{
	if(Domain<0 || Domain>=Net_Socket_Domain_End || Type<0 || Type>=Net_Socket_Type_End || Protocol<0 || Protocol>=Net_Socket_Protocol_End)
	{
		return Error_Invalid_Parameter;
	}


	if(Net_Core_DATA.Device_Node_List.Head==Null)
	{
		return Error_No_Open;
	}

	int Err;

	//Net_Core_Device_Node_Type *P_Net_Node=Net_Core_DATA.Device_Node_List.Head;

	Net_Session_Handle_Type Handle={.DATA=0};

	Handle.Protocol=Net_Session_Protocol_UDP;
	Handle.Handle=Net_Socket_DATA.Handle;


	if((Err=Session_Add_Node(&Net_Socket_DATA.Session_DATA,Handle,Net_Session_Protocol_UDP,Null))!=Error_OK)
	{
		return Err;
	}



	Net_Socket_DATA.Handle++;

	return Handle.DATA;
}


int Net_Bind(int Socket, const Net_Socket_Address_DATA_Type * Addr)
{


	return Error_OK;
}
/*
int Net_Connect(int Socket, const Net_Socket_Address_DATA_Type * Addr)
{


	return Error_OK;
}

int Net_Send(int Socket, const void * Data, int SendLength, int Flags)
{


	return Error_OK;
}

int Net_Recv(int Socket, void * Data, int RecvLength, int Flags)
{


	return Error_OK;
}
*/
int Net_SendTo(int Socket, const void * Data, int Sendlength, int Flags, const Net_Socket_Address_DATA_Type * Addr)
{


	return Error_OK;
}

int Net_RecvFrom(int Socket, void * Data, int RecvLength, int Flags, Net_Socket_Address_DATA_Type * Addr)
{


	return Error_OK;
}
/*
int Net_Listen(int Socket, int Max_Connections)
{


	return Error_OK;
}

int Net_Accept(int Socket, Net_Socket_Address_DATA_Type * Addr)
{


	return Error_OK;
}

int Net_ShutDown(int Socket, int ShutDown_Type)
{


	return Error_OK;
}
*/
int Net_CloseSocket(int Socket)
{


	return Error_OK;
}
/*
int Net_Ioctl(int Socket, long Cmd, void * Arg)
{


	return Error_OK;
}
*/
int Net_SetSockopt(int Socket, int Level, int Option_Name, const void * Data, int Data_Len)
{


	return Error_OK;
}

int Net_GetSockopt(int Socket, int Level, int Option_Name, void * Data, int * Data_Len)
{


	return Error_OK;
}
/*
int Net_GetPeername(int Socket, Net_Socket_Address_DATA_Type *Addr)
{


	return Error_OK;
}

int Net_GetSockname(int Socket, Net_Socket_Address_DATA_Type *Addr)
{


	return Error_OK;
}

// sys/time.h (actually intersects partly with libnds, so I'm letting libnds handle fd_set for the time being)
//int Net_Select(int Nfds, fd_set *ReadFds, fd_set *WriteFds, fd_set *ErrorFds, struct timeval *TimeOut);

// arpa/inet.h
unsigned long Net_Inet_Addr(const char *Cp)
{


	return Error_OK;
}
*/

Com_Module_Init_Export(Net_Socket_Init);
