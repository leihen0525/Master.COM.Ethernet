/*
 * IPv4.ICMP.Enum.h
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */

#ifndef IPV4_ICMP_ENUM_H_
#define IPV4_ICMP_ENUM_H_

typedef enum
{
	Net_IPv4_ICMP_Type_Echo_Reply_Message					=0,		//回显应答
	Net_IPv4_ICMP_Type_Destination_Unreachable_Message		=3,		//目的不可达
	Net_IPv4_ICMP_Type_Source_Quench_Message				=4,		//源冷却
	Net_IPv4_ICMP_Type_Redirect_Message						=5,		//重定向
	Net_IPv4_ICMP_Type_Echo_Message							=8,		//请求回显
	Net_IPv4_ICMP_Type_Time_Exceeded_Message				=11,	//超时
	Net_IPv4_ICMP_Type_Parameter_Problem_Message			=12,	//参数错误报文
	Net_IPv4_ICMP_Type_Timestamp_Message					=13,	//时间戳请求
	Net_IPv4_ICMP_Type_Timestamp_Timestamp_Reply_Message	=14,	//时间戳应答

}Net_IPv4_ICMP_Type;


typedef enum
{
	Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Net_Unreachable	=0,

	Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Host_Unreachable,

	Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Protocol_Unreachable,

	Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Port_Unreachable,

	Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Fragmentation_Needed_And_DF_Set,

	Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Source_Route_Failed,

}Net_IPv4_ICMP_Code_Destination_Unreachable_Message_Type;

#endif /* IPV4_ICMP_ENUM_H_ */
