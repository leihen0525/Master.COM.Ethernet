/*
 * IPv6.Struct.h
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */

#ifndef IPV6_STRUCT_H_
#define IPV6_STRUCT_H_

#include "Master.Stdint.h"

typedef struct
{
	struct
	{
		bool Confirm;
		uint8_t Address[16];
		uint8_t SubNet_Mask[16];
	}IP;

	struct
	{
		bool Confirm;
		uint8_t Address[16];
	}Default_Gateway;

}Net_IPv6_Address_Type;

#endif /* IPV6_STRUCT_H_ */
