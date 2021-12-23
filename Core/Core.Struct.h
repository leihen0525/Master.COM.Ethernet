/*
 * Core.Struct.h
 *
 *  Created on: 2019年7月15日
 *      Author: Master.HE
 */

#ifndef CORE_STRUCT_H_
#define CORE_STRUCT_H_

#include "Master.Stdint.h"
#include "Includes/Net.Device.Struct.h"
#include "IPv4/IPv4.Struct.h"
#include "IPv4/IPv4.ARP/IPv4.ARP.Struct.h"
#include "Protocol/Protocol.TCP/Protocol.TCP.Struct.h"
#include "Protocol/Protocol.UDP/Protocol.UDP.Struct.h"
//#include "Session/Session.Struct.h"



typedef struct Net_Core_Device_Node
{
	int Device_Type;

	int Task_PID_Tx;
	int Task_PID_Rx;


	bool Open;
	uint8_t *P_Rx_DATA;


	Net_IPv4_ARP_DATA_Type ARP_DATA;
	Net_IPv4_DATA_Type IPv4_DATA;

	Net_Protocol_TCP_DATA_Type Protocol_TCP_DATA;
	Net_Protocol_UDP_DATA_Type Protocol_UDP_DATA;

	//Net_Session_DATA_Type Session_DATA;

	const Net_Device_Ethernet_Config_Type *P_Ethernet_Config;

	const Net_Device_OPS_Type *P_OPS;

	struct Net_Core_Device_Node *NEXT;

}Net_Core_Device_Node_Type;



typedef struct
{

	struct
	{
		Net_Core_Device_Node_Type *Head;
		Net_Core_Device_Node_Type *End;
	}Device_Node_List;

}Net_Core_DATA_Type;

#endif /* CORE_STRUCT_H_ */
