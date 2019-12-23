/*
 * IPv4.ICMP.Struct.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef IPV4_ICMP_STRUCT_H_
#define IPV4_ICMP_STRUCT_H_

#include "Master.Stdint.h"

#pragma pack(1)

typedef struct
{
	uint8_t Type;

	uint8_t Code;

	uint16_t Checksum;

	//uint16_t Identifier;

	//uint16_t Sequence_Number;

}Net_IPv4_ICMP_Packet_Heade_Type;

#pragma pack()

#endif /* IPV4_ICMP_STRUCT_H_ */
