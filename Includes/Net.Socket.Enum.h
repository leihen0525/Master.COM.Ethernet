/*
 * Net.Socket.Enum.h
 *
 *  Created on: 2019Äê12ÔÂ19ÈÕ
 *      Author: Master.HE
 */

#ifndef NET_SOCKET_ENUM_H_
#define NET_SOCKET_ENUM_H_


typedef enum
{
	Net_Socket_Domain_AF_UNIX			=0,	/* standardized name for AF_LOCAL */
	Net_Socket_Domain_AF_INET,				/* internetwork: UDP, TCP, etc. */
	Net_Socket_Domain_AF_INET6,				/* IPv6 */

	Net_Socket_Domain_End,
}Net_Socket_Domain_Type;

typedef enum
{
	Net_Socket_Type_SOCK_STREAM			=0,	/* stream socket */
	Net_Socket_Type_SOCK_DGRAM,				/* datagram socket */
	Net_Socket_Type_SOCK_RAW,				/* raw-protocol interface */
	Net_Socket_Type_SOCK_SEQPACKET,			/* sequenced packet stream */

	Net_Socket_Type_End,
}Net_Socket_Type_Type;

typedef enum
{
	Net_Socket_Protocol_IPPROTO_IP		=0,	/* dummy for IP */
	Net_Socket_Protocol_IPPROTO_ICMP,		/* control message protocol */

	Net_Socket_Protocol_IPPROTO_TCP,		/* tcp */
	Net_Socket_Protocol_IPPROTO_UDP,		/* user datagram protocol */

	Net_Socket_Protocol_End,
}Net_Socket_Protocol_Type;

typedef enum
{
	Net_Socket_Address_Type_IPv4		=0,
	Net_Socket_Address_Type_IPv6,

	Net_Socket_Address_Type_End,

}Net_Socket_Address_Type;

#endif /* NET_SOCKET_ENUM_H_ */
