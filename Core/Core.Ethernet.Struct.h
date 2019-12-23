/*
 * Core.Ethernet.Struct.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef CORE_ETHERNET_STRUCT_H_
#define CORE_ETHERNET_STRUCT_H_

#include "Master.Stdint.h"


#pragma pack(1)

typedef struct
{
	uint8_t DEST_MAC_Address[6];
	uint8_t SRC_MAC_Address[6];

	uint16_t Length_Type;

}Net_Core_Ethernet_Packet_Heade_Type;


#pragma pack()


typedef struct
{
	uint8_t *DATA;
	uint32_t Size;

}Net_Core_Ether_TX_DATA_Type;


#endif /* CORE_ETHERNET_STRUCT_H_ */
