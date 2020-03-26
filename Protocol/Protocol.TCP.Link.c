/*
 * Protocol.TCP.Link.c
 *
 *  Created on: 2020年3月24日
 *      Author: Master.HE
 */
#include "Error.h"

#include "Protocol.TCP.Link.h"

int Protocol_TCP_Link_Init(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA)
{
	if(P_Protocol_TCP_Link_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}

	P_Protocol_TCP_Link_DATA->In.Head=Null;
	P_Protocol_TCP_Link_DATA->In.End=Null;

	P_Protocol_TCP_Link_DATA->Out.Head=Null;
	P_Protocol_TCP_Link_DATA->Out.End=Null;

	return Error_OK;
}
