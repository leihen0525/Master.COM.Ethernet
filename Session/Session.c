/*
 * Session.c
 *
 *  Created on: 2019年10月15日
 *      Author: Master.HE
 */
#include <string.h>
#include "Error.h"

#include "API.h"

#include "Net.Core.Enum.h"

#include "Session.h"

int Session_Init(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Session_DATA_Type *P_Session_DATA)
{
	if(P_Net_Node==Null || P_Session_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}

	P_Session_DATA->TCP_List.Head=Null;
	P_Session_DATA->TCP_List.End=Null;

	P_Session_DATA->UDP_List.Head=Null;
	P_Session_DATA->UDP_List.End=Null;

//	P_Session_DATA->TCP.IN.Head=Null;
//	P_Session_DATA->TCP.IN.End=Null;
//
//	P_Session_DATA->TCP.OUT.Head=Null;
//	P_Session_DATA->TCP.OUT.End=Null;
//
//	P_Session_DATA->UDP.IN.Head=Null;
//	P_Session_DATA->UDP.IN.End=Null;
//
//	P_Session_DATA->UDP.OUT.Head=Null;
//	P_Session_DATA->UDP.OUT.End=Null;

	return Error_OK;

}



int Session_Switch_List(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Session_Protocol_Type Protocol,
		//Net_Session_Direction_Type Direction,
		Net_Session_Node_List_Type **P_List)
{
	if(P_Net_Node==Null
	|| Protocol>=Net_Session_Protocol_End
	//|| Direction>=Net_Session_Direction_End
	|| P_List==Null)
	{
		return Error_Invalid_Parameter;
	}

	if(Protocol==Net_Session_Protocol_UDP)
	{

		*P_List=&P_Net_Node->Session_DATA.UDP_List;

		return Error_OK;

	}
	else if(Protocol==Net_Session_Protocol_TCP)
	{

		*P_List=&P_Net_Node->Session_DATA.TCP_List;

		return Error_OK;

	}
	return Error_Undefined;
}

int Session_Add_Node(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Session_Handle_Type Handle,

		Net_Session_Protocol_Type Protocol,
		//Net_Session_Direction_Type Direction,

		Net_Session_Node_DATA_Type **P_Node_DATA
		)
{
	if(P_Net_Node==Null

	|| Protocol>=Net_Session_Protocol_End

	)
	{
		return Error_Invalid_Parameter;
	}

	int Err=Session_Find_Node(P_Net_Node,Protocol,&Handle,Null,Null);

	if(Err!=Error_Undefined)
	{
		if(Err==Error_OK)
		{
			Err=Error_Exist;
		}
		return Err;
	}

	Net_Session_Node_List_Type *P_Temp_List=Null;

	Err=Session_Switch_List(P_Net_Node,Protocol,&P_Temp_List);

	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_Session_Node_DATA_Type *Temp_Node=Null;

	Temp_Node=Memory_Malloc(sizeof(Net_Session_Node_DATA_Type));

	if(Temp_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}
	Temp_Node->Handle.DATA=Handle.DATA;

	if(P_Temp_List->End==Null)
	{
		P_Temp_List->Head=Temp_Node;
	}
	else
	{
		P_Temp_List->End->NEXT=Temp_Node;
	}
	P_Temp_List->End=Temp_Node;
	Temp_Node->NEXT=Null;

	if(P_Node_DATA!=Null)
	{
		*P_Node_DATA=Temp_Node;
	}

	return Error_OK;
}
/*
int Session_Del_Node(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Core_Address_DATA_Type *P_Address_DATA,
		Net_Session_Protocol_Type Protocol,
		Net_Session_Direction_Type Direction,
		Net_Protocol_Node_DATA_Type **Protocol_Node_DATA)
{
	if(P_Net_Node==Null
	|| P_Address_DATA==Null
	|| Protocol>=Net_Session_Protocol_End
	|| Direction>=Net_Session_Direction_End)
	{
		return Error_Invalid_Parameter;
	}
	if(P_Address_DATA->Address_Type>=Net_Core_Address_Type_End)
	{
		return Error_Invalid_Parameter;
	}

	Net_Session_Node_List_Type *P_Temp_List=Null;
	int Err=Session_Switch_List(P_Net_Node,Protocol,Direction,&P_Temp_List);

	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_Session_Node_DATA_Type *Temp_Node=Null,*Temp_Node_LAST=Null;

	Temp_Node=P_Temp_List->Head;
	while(Temp_Node!=Null)
	{
		if(Protocol_Node_DATA==Null && Temp_Node->Core_Address_DATA!=Null)
		{
			if((Temp_Node->Core_Address_DATA->Address_Type==P_Address_DATA->Address_Type)
			&& (Temp_Node->Core_Address_DATA->SRC_PORT==P_Address_DATA->SRC_PORT)
			&& (Temp_Node->Core_Address_DATA->DEST_PORT==P_Address_DATA->DEST_PORT)
			&& (
					(Temp_Node->Core_Address_DATA->Address_Type==Net_Core_Address_Type_IPv4 && Temp_Node->Core_Address_DATA->_IPv4_Address==P_Address_DATA->_IPv4_Address)
				|| (Temp_Node->Core_Address_DATA->Address_Type==Net_Core_Address_Type_IPv6 && Temp_Node->Core_Address_DATA->_IPv6_Address==P_Address_DATA->_IPv6_Address)
				)
			)
//			if(Temp_Node->_IPv4_Address==_IPv4_Address
//			&& Temp_Node->SRC_PORT==SRC_PORT
//			&& Temp_Node->DEST_PORT==DEST_PORT
//			//&& Temp_Node->Protocol==Protocol
//			//&& Temp_Node->Direction==Direction
//			)
			{

			}
			else
			{
				goto Loop_End;
			}
		}
		else
		{
			if(*Protocol_Node_DATA==Null && Temp_Node->Core_Address_DATA!=Null)
			{
				if((Temp_Node->Core_Address_DATA->Address_Type==P_Address_DATA->Address_Type)
				&& (Temp_Node->Core_Address_DATA->SRC_PORT==P_Address_DATA->SRC_PORT)
				&& (Temp_Node->Core_Address_DATA->DEST_PORT==P_Address_DATA->DEST_PORT)
				&& (
						(Temp_Node->Core_Address_DATA->Address_Type==Net_Core_Address_Type_IPv4 && Temp_Node->Core_Address_DATA->_IPv4_Address==P_Address_DATA->_IPv4_Address)
					|| (Temp_Node->Core_Address_DATA->Address_Type==Net_Core_Address_Type_IPv6 && Temp_Node->Core_Address_DATA->_IPv6_Address==P_Address_DATA->_IPv6_Address)
					)
				)
//				if(Temp_Node->_IPv4_Address==_IPv4_Address
//				&& Temp_Node->SRC_PORT==SRC_PORT
//				&& Temp_Node->DEST_PORT==DEST_PORT
//				//&& Temp_Node->Protocol==Protocol
//				//&& Temp_Node->Direction==Direction
//				)
				{

				}
				else
				{
					goto Loop_End;
				}
			}
			else
			{
				//TODO 这个地方就会受牵制于 Protocol和Direction两个参数
				if(*Protocol_Node_DATA!=Temp_Node->Protocol_Node_DATA)
				{
					goto Loop_End;
				}
			}
		}

		if(Temp_Node_LAST==Null)
		{
			P_Temp_List->Head=Temp_Node->NEXT;
		}
		else
		{
			Temp_Node_LAST->NEXT=Temp_Node->NEXT;
		}

		if(Temp_Node==P_Temp_List->End || Temp_Node->NEXT==Null)
		{
			P_Temp_List->End=Temp_Node_LAST;
		}

		Temp_Node->NEXT=Null;

		if(Protocol_Node_DATA!=Null)
		{
			*Protocol_Node_DATA=Temp_Node->Protocol_Node_DATA;
		}
		if(Temp_Node->Core_Address_DATA!=Null)
		{
			Memory_Free(Temp_Node->Core_Address_DATA);
		}
		Memory_Free(Temp_Node);

		return Error_OK;

Loop_End:
		Temp_Node_LAST=Temp_Node;
		Temp_Node=Temp_Node->NEXT;
	}

	return Error_Invalid_Parameter;
}
*/
int Session_Find_Node(
		Net_Core_Device_Node_Type *P_Net_Node,

		Net_Session_Protocol_Type Protocol,
		//Net_Session_Direction_Type Direction,

		Net_Session_Handle_Type *P_Handle,

		Net_Core_Address_DATA_Type *P_Address_DATA,

		Net_Session_Node_DATA_Type **P_Node_DATA
		)
{
	if(P_Net_Node==Null
	//|| P_Address_DATA==Null
	|| Protocol>=Net_Session_Protocol_End
	//|| Direction>=Net_Session_Direction_End
	|| (P_Handle==Null && P_Address_DATA==Null && P_Node_DATA==Null)
	)
	{
		return Error_Invalid_Parameter;
	}
	if(P_Handle!=Null)
	{
		if(P_Handle->Protocol!=Protocol)
		{
			return Error_Invalid_Parameter;
		}
	}
	if(P_Address_DATA!=Null)
	{
		if(P_Address_DATA->Address_Type>=Net_Core_Address_Type_End)
		{
			return Error_Invalid_Parameter;
		}
	}
	if(P_Handle==Null && P_Address_DATA==Null)
	{
		if(*P_Node_DATA==Null)
		{
			return Error_Invalid_Parameter;
		}
	}

	Net_Session_Node_List_Type *P_Temp_List=Null;

	int Err=Session_Switch_List(P_Net_Node,Protocol,&P_Temp_List);

	if(Err!=Error_OK)
	{
		return Err;
	}

	Net_Session_Node_DATA_Type *Temp_Node=Null;

	Temp_Node=P_Temp_List->Head;

	while(Temp_Node!=Null)
	{
		if(P_Handle!=Null)
		{
			if(P_Handle->Handle!=Temp_Node->Handle.Handle)
			{
				goto NEXT_Loop;
			}
		}

		if(P_Address_DATA!=Null)
		{
			if((Temp_Node->Core_Address_DATA->Address_Type==P_Address_DATA->Address_Type)
			&& (Temp_Node->Core_Address_DATA->SRC_PORT==P_Address_DATA->SRC_PORT)
			&& (Temp_Node->Core_Address_DATA->DEST_PORT==P_Address_DATA->DEST_PORT)
			&& (
				   (Temp_Node->Core_Address_DATA->Address_Type==Net_Core_Address_Type_IPv4 && Temp_Node->Core_Address_DATA->_IPv4_Address==P_Address_DATA->_IPv4_Address)
				|| (Temp_Node->Core_Address_DATA->Address_Type==Net_Core_Address_Type_IPv6 && Temp_Node->Core_Address_DATA->_IPv6_Address==P_Address_DATA->_IPv6_Address)
				)
			)
			{
				;
			}
			else
			{
				goto NEXT_Loop;
			}
		}

		if(P_Node_DATA==Null)
		{
			return Error_OK;
		}
		else//P_Node_DATA!=Null
		{
			if(*P_Node_DATA==Null)
			{
				*P_Node_DATA=Temp_Node;

				return Error_OK;
			}
			else//*P_Node_DATA!=Null
			{
				if(*P_Node_DATA==Temp_Node)
				{
					return Error_OK;
				}
			}
		}

NEXT_Loop:
		Temp_Node=Temp_Node->NEXT;
	}

	return Error_Undefined;

}

