/*
 * Protocol.UDP.Struct.h
 *
 *  Created on: 2019��7��17��
 *      Author: Master.HE
 */

#ifndef PROTOCOL_UDP_STRUCT_H_
#define PROTOCOL_UDP_STRUCT_H_

#include "Master.Stdint.h"


#pragma pack(1)


typedef struct
{
	uint16_t SRC_PORT;

	uint16_t DEST_PORT;

	uint16_t Total_Length;

	uint16_t CheckSum;
}Net_Protocol_UDP_Packet_Heade_Type;


#pragma pack()

typedef struct
{
	int a;

}Net_Protocol_UDP_Node_DATA_Type;

typedef struct
{
	int a;
}Net_Protocol_UDP_DATA_Type;


#endif /* PROTOCOL_UDP_STRUCT_H_ */