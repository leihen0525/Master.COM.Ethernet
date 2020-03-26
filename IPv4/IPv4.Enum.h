/*
 * IPv4.Enum.h
 *
 *  Created on: 2019年7月19日
 *      Author: Master.HE
 */

#ifndef IPV4_ENUM_H_
#define IPV4_ENUM_H_

typedef enum
{
	Net_IPv4_Protocol_ICMP					=0x01,
	Net_IPv4_Protocol_TCP					=0x06,
	Net_IPv4_Protocol_UDP					=0x11,

}Net_IPv4_Protocol_Type;

typedef enum
{
	Net_IPv4_Address_Type_Invalid			=0,	//无效地址
	Net_IPv4_Address_Type_SubNet,				//同一个子网地址
	Net_IPv4_Address_Type_ExtraNet,				//外网地址
	Net_IPv4_Address_Type_SelfAddress,			//自身地址
	Net_IPv4_Address_Type_BroadcastAddress,		//广播地址

}Net_IPv4_Address_Type;

typedef enum
{
	Net_IPv4_Packet_Null					=0, //空包或者不完整
	Net_IPv4_Packet_Single,						//单包
	Net_IPv4_Packet_Multi,						//多包
}Net_IPv4_Packet_Type;

#endif /* IPV4_ENUM_H_ */
