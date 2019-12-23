/*
 * IPv4.ARP.Struct.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef IPV4_ARP_STRUCT_H_
#define IPV4_ARP_STRUCT_H_

#include "Master.Stdint.h"


#pragma pack(1)
typedef struct
{

	uint16_t Hardware_Type;
	uint16_t Protocol_Type;
	uint8_t Hardware_Size;
	uint8_t Protocol_Size;
	uint16_t Opcode;
	uint8_t Sender_MAC_Address[6];
	uint8_t Sender_IP_Address[4];
	uint8_t Target_MAC_Address[6];
	uint8_t Target_IP_Address[4];

	uint8_t Padding[18];
}Net_IPv4_ARP_Packet_Type;
#pragma pack()

typedef struct Net_IPv4_ARP_Cache_NODE
{

	uint8_t IPv4_Address[4];
	uint8_t MAC_Address[6];
	int16_t TimeOut;

	struct Net_IPv4_ARP_Cache_NODE *NEXT;
}Net_IPv4_ARP_Cache_Node_Type;

typedef struct
{
	Net_IPv4_ARP_Cache_Node_Type *Head;
	Net_IPv4_ARP_Cache_Node_Type *End;
}Net_IPv4_ARP_Cache_List_Type;

typedef struct
{
	int Mutex;
	int Time_1S;

	Net_IPv4_ARP_Cache_List_Type ARP_Cache_List;


}Net_IPv4_ARP_DATA_Type;

#endif /* IPV4_ARP_STRUCT_H_ */
