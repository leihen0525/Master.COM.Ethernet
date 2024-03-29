/*
 * Protocol.TCP.Enum.h
 *
 *  Created on: 2020年3月30日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_ENUM_H_
#define PROTOCOL_TCP_ENUM_H_


typedef enum
{
	Net_Protocol_TCP_Options_Kind_EOL				=0,		//选项列表结束
	Net_Protocol_TCP_Options_Kind_NOP				=1,		//无操作（用于补位填充）
	Net_Protocol_TCP_Options_Kind_MSS				=2,		//最大Segment长度
	Net_Protocol_TCP_Options_Kind_WSOPT				=3,		//窗口扩大系数（Window Scaling Factor）
	Net_Protocol_TCP_Options_Kind_SACK_Premitted	=4,		//表明支持SACK
	Net_Protocol_TCP_Options_Kind_SACK				=5,		//SACK Block（收到乱序数据）
	Net_Protocol_TCP_Options_Kind_TSPOT				=8,		//Timestamps
	Net_Protocol_TCP_Options_Kind_TCP_MD5			=19,	//MD5认证
	Net_Protocol_TCP_Options_Kind_UTO				=28,	//User Timeout（超过一定闲置时间后拆除连接）
	Net_Protocol_TCP_Options_Kind_TCP_AO			=29,	//认证（可选用各种算法）
	Net_Protocol_TCP_Options_Kind_Experimental1		=253,	//保留，用于科研实验
	Net_Protocol_TCP_Options_Kind_Experimental2		=254,	//保留，用于科研实验
}Net_Protocol_TCP_Options_Kind_Type;



typedef enum
{
	Net_Protocol_TCP_Packet_Flags_Fin				=(1<<0),//释放连接
	Net_Protocol_TCP_Packet_Flags_Syn				=(1<<1),//请求连接
	Net_Protocol_TCP_Packet_Flags_Reset				=(1<<2),//拒绝连接
	Net_Protocol_TCP_Packet_Flags_Push				=(1<<3),
	Net_Protocol_TCP_Packet_Flags_Ack				=(1<<4),//确认号有效
	Net_Protocol_TCP_Packet_Flags_Urgent			=(1<<5),
	Net_Protocol_TCP_Packet_Flags_ECN_Echo			=(1<<6),
	Net_Protocol_TCP_Packet_Flags_CWR				=(1<<7),

	Net_Protocol_TCP_Packet_Flags_Nonce				=(1<<8),
}Net_Protocol_TCP_Packet_Flags_Type;



#endif /* PROTOCOL_TCP_ENUM_H_ */
