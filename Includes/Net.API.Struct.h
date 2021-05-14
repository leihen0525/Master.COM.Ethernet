/*
 * Net.API.Struct.h
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */

#ifndef NET_API_STRUCT_H_
#define NET_API_STRUCT_H_

#include "Master.Stdint.h"
#include "Net.IP.Enum.h"


typedef struct
{
	Net_IP_Address_Type IP_Type;

	uint8_t IP_Address[16];

	uint16_t PORT;

}Net_API_IP_Information_Type;

#endif /* NET_API_STRUCT_H_ */
