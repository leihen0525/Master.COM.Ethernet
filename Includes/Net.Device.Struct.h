/*
 * Net.Device.Struct.h
 *
 *  Created on: 2019Äê7ÔÂ15ÈÕ
 *      Author: Master.HE
 */

#ifndef NET_DEVICE_STRUCT_H_
#define NET_DEVICE_STRUCT_H_

#include "Master.Stdint.h"

typedef struct
{

	struct
	{
		uint8_t IP_Address[4];
		uint8_t SubNet_Mask[4];
		uint8_t Default_Gateway[4];
	}IPv4;

}Net_Device_IP_Config;


typedef struct
{
	const char *Device_Name;
	const char *Info;

	uint16_t HEADER;
	uint16_t MTU;
	uint8_t MAC_Address[6];

	int (*Init)   (int Flag);
	int (*Open)   (void);
	int (*Close)  (void);

	int (*SET_MAC_Address)(uint8_t *Address);

	int (*GET_Linked_Status)(void);

	int (*Read)   (void *Buffer, unsigned long Size,int TimeOut);
	int (*Write)  (long OffSet_Pos,const void *Buffer, unsigned long Size,int TimeOut);

}Net_Device_OPS_Type;

#endif /* NET_DEVICE_STRUCT_H_ */
