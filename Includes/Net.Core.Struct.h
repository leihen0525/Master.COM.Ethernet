/*
 * Net.Core.Struct.h
 *
 *  Created on: 2019年12月20日
 *      Author: Master.HE
 */

#ifndef NET_CORE_STRUCT_H_
#define NET_CORE_STRUCT_H_
/*
typedef struct
{
	union
	{
		uint8_t IP_DATA[8];

		uint32_t _IPv4_Address;
		uint8_t IPv4_Address[4];

		uint64_t _IPv6_Address;
		uint8_t IPv6_Address[8];
	};


	uint16_t SRC_PORT;
	uint16_t DEST_PORT;

	uint8_t Address_Type;


	union
	{
		uint8_t Enabled_DATA;

		struct
		{
			uint8_t IPv4			:1;
			uint8_t IPv6			:1;
			uint8_t SRC_PORT		:1;
			uint8_t DEST_PORT		:1;
			uint8_t					:4;
		}Enabled;
	};

}Net_Core_Address_DATA_Type;
*/
#endif /* NET_CORE_STRUCT_H_ */
