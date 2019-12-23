/*
 * Protocol.TCP.Struct.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_STRUCT_H_
#define PROTOCOL_TCP_STRUCT_H_

#include "Master.Stdint.h"


#pragma pack(1)

typedef union
{
	uint16_t DATA;
	struct
	{
		uint16_t Fin					:1;
		uint16_t Syn					:1;
		uint16_t Reset					:1;
		uint16_t Push					:1;
		uint16_t Ack					:1;
		uint16_t Urgent					:1;
		uint16_t ECN_Echo				:1;
		uint16_t CWR					:1;

		uint16_t Nonce					:1;
		uint16_t 						:3;
		uint16_t Header_Length			:4;
	};
}Net_Protocol_TCP_Packet_Heade_Flags_Type;

typedef struct
{
	uint16_t SRC_PORT;

	uint16_t DEST_PORT;

	uint32_t Sequence_Number;

	uint32_t Acknowledgment_Number;

	uint16_t Flags;

	uint16_t Window_Size;

	uint16_t CheckSum;

	uint16_t Urgent_Pointer;

}Net_Protocol_TCP_Packet_Heade_Type;


#pragma pack()

//-------------------------------------------------------------------------------------------------


typedef struct
{
	int a;

}Net_Protocol_TCP_Node_DATA_Type;


typedef struct
{
	uint32_t ISN;


}Net_Protocol_TCP_DATA_Type;



#endif /* PROTOCOL_TCP_STRUCT_H_ */
