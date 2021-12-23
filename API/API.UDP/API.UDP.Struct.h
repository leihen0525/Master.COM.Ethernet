/*
 * API.UDP.Struct.h
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */

#ifndef API_UDP_STRUCT_H_
#define API_UDP_STRUCT_H_

#include "Master.Stdint.h"


#include "Core/Core.Struct.h"
#include "Net.API.Struct.h"

typedef struct Net_API_UDP_Buff_Node
{
	Net_API_IP_Information_Type IP_Info;
	uint8_t *DATA;
	uint16_t Index;
	uint16_t Size;

	struct Net_API_UDP_Buff_Node *NEXT;
}Net_API_UDP_Buff_Type;



typedef struct Net_API_UDP_Node
{
	int Handle;

	//设备
	Net_Core_Device_Node_Type *Device_Node;

	struct
	{
		int Mutex;
		int Semaphore;

		struct
		{
			uint32_t Max_Length;
			int32_t Count;
			uint32_t Size;
			bool OverFlow;

			Net_API_UDP_Buff_Type *Head;
			Net_API_UDP_Buff_Type *End;
		}Buff;

	}Rx_Buffer;

	//Bind
	struct
	{
		bool SET;
		Net_API_IP_Information_Type IP_Info;
	}Bind;

	//Connect
	struct
	{
		bool SET;
		Net_API_IP_Information_Type IP_Info;
	}Connect;

	struct Net_API_UDP_Node *NEXT;
}Net_API_UDP_Node_Type;

typedef struct
{
	Net_API_UDP_Node_Type *Head;
	Net_API_UDP_Node_Type *End;
}Net_API_UDP_DATA_Type;


#endif /* API_UDP_STRUCT_H_ */
