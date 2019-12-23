/*
 * IPv4.Struct.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef IPV4_STRUCT_H_
#define IPV4_STRUCT_H_

#include "Master.Stdint.h"
#include "IPv4.Define.h"

#pragma pack(1)

typedef struct
{
	union
	{
		uint8_t SRC_IPv4_Address[4];
		uint32_t _SRC_IPv4_Address;
	};
	union
	{
		uint8_t DEST_IPv4_Address[4];
		uint32_t _DEST_IPv4_Address;
	};

	uint8_t mbz;

	uint8_t Protocol;

	uint16_t Length;
}Net_IPv4_Packet_Pseudo_Heade_Type;


typedef union
{
	uint16_t DATA;
	struct
	{
		uint16_t Offset					:13;

		uint16_t MF						:1;
		uint16_t DF						:1;
		uint16_t						:1;
	};
}Net_IPv4_Packet_Heade_Flags_Type;

typedef struct
{
	uint8_t Header_Length			:4;
	uint8_t Version					:4;

	uint8_t TOS;

	uint16_t Total_Length;

	uint16_t Identification;

	uint16_t Flags;

	uint8_t TTL;

	uint8_t Protocol;

	uint16_t Header_CheckSum;

	uint8_t SRC_IPv4_Address[4];
	uint8_t DEST_IPv4_Address[4];

}Net_IPv4_Packet_Heade_Type;


#pragma pack()

typedef struct Net_IPv4_Rx_NODE_DATA
{
	uint16_t Address;
	uint16_t Size;

	uint8_t *DATA;

	struct Net_IPv4_Rx_NODE_DATA *NEXT;
}Net_IPv4_Rx_Node_Data_Type;


typedef struct Net_IPv4_Rx_NODE
{
	uint8_t SRC_IPv4_Address[4];
	uint8_t DEST_IPv4_Address[4];

	uint8_t Protocol;

	uint8_t Head_Part			:1;
	uint8_t End_Part			:1;
	int8_t TimeOut_1S			:6;

	uint16_t Identification;

	uint16_t Size;
	uint16_t Len;

	Net_IPv4_Rx_Node_Data_Type *DATA;

	struct Net_IPv4_Rx_NODE *NEXT;
}Net_IPv4_Rx_Node_Type;

typedef struct
{
	Net_IPv4_Rx_Node_Type *Head;
	Net_IPv4_Rx_Node_Type *End;
}Net_IPv4_Rx_List_Type;


typedef struct Net_IPv4_Tx_NODE
{

	uint8_t Target_IPv4_Address[4];

	uint8_t Target_MAC_Address[6];

	uint8_t Protocol;

	uint8_t DEST_IPv4_Address[4];

	uint8_t *DATA;

	uint16_t Size;

	bool DATA_Free;

	int16_t TimeOut;

	int16_t Count;

	struct Net_IPv4_Tx_NODE *NEXT;
}Net_IPv4_Tx_Node_Type;

typedef struct
{
	Net_IPv4_Tx_Node_Type *Head;
	Net_IPv4_Tx_Node_Type *End;
}Net_IPv4_Tx_List_Type;

typedef struct
{
	uint16_t ID;

	int Semaphore_Tx_Ready;
	int Mutex_Tx;
	int Time_10MS;

	int Mutex_Rx;
	int Time_1S;

	struct
	{
		Net_IPv4_Tx_List_Type Ready;
		Net_IPv4_Tx_List_Type Pend;
	}Tx_List;

	struct
	{
		int Count;

		Net_IPv4_Rx_List_Type List;
	}Rx_List;

}Net_IPv4_DATA_Type;






#endif /* IPV4_STRUCT_H_ */
