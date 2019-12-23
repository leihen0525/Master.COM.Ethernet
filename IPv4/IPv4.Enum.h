/*
 * IPv4.Enum.h
 *
 *  Created on: 2019��7��19��
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
	Net_IPv4_Address_Type_Invalid			=0,	//��Ч��ַ
	Net_IPv4_Address_Type_SubNet,				//ͬһ��������ַ
	Net_IPv4_Address_Type_ExtraNet,				//������ַ
	Net_IPv4_Address_Type_SelfAddress,			//�����ַ
	Net_IPv4_Address_Type_BroadcastAddress,		//�㲥��ַ

}Net_IPv4_Address_Type;

typedef enum
{
	Net_IPv4_Packet_Null					=0, //�հ����߲�����
	Net_IPv4_Packet_Single,						//����
	Net_IPv4_Packet_Multi,						//���
}Net_IPv4_Packet_Type;

#endif /* IPV4_ENUM_H_ */
