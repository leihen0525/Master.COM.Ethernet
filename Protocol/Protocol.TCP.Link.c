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

	P_Protocol_TCP_Link_DATA->Head=Null;
	P_Protocol_TCP_Link_DATA->End=Null;

	return Error_OK;
}

int Protocol_TCP_Link_Add(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_IP_Type IP_Type,
		uint8_t *Dest_IP_Address,
		uint16_t Dest_PORT,
		uint16_t Local_PORT,
		Net_Protocol_TCP_Link_Node_DATA_Type **P_DATA)
{

}

int Protocol_TCP_Link_Find(
		Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_IP_Type IP_Type,
		uint8_t *Dest_IP_Address,
		uint16_t Dest_PORT,
		uint16_t Local_PORT,
		Net_Protocol_TCP_Link_Node_DATA_Type **P_DATA)
{

}
int Protocol_TCP_Link_Del(Net_Protocol_TCP_Link_DATA_Type *P_Protocol_TCP_Link_DATA,
		Net_Protocol_IP_Type IP_Type,
		uint8_t *Dest_IP_Address,
		uint16_t Dest_PORT,
		uint16_t Local_PORT,
		bool Internal_Del_DATA,
		Net_Protocol_TCP_Link_Node_DATA_Type **P_DATA)
{

}
