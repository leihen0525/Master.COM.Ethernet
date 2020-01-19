/*
 * Net.Socket.Struct.h
 *
 *  Created on: 2019Äê12ÔÂ19ÈÕ
 *      Author: Master.HE
 */

#ifndef NET_SOCKET_STRUCT_H_
#define NET_SOCKET_STRUCT_H_

#include "Net.Socket.Type.h"


typedef struct
{
	union
	{
		uint8_t DATA[8];

		uint32_t _IPv4_Address;
		uint8_t IPv4_Address[4];

		uint64_t _IPv6_Address;
		uint8_t IPv6_Address[8];
	};

	uint16_t PORT;

	uint8_t Address_Type;

}Net_Socket_Address_DATA_Type;


#endif /* NET_SOCKET_STRUCT_H_ */
