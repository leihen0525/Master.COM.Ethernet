/*
 * Net.API.h
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */

#ifndef NET_API_H_
#define NET_API_H_

#include "API.h"

#if (Master_OS_Main_Versions!=1)
#error "Master OS Versions Error!!!"
#endif

#include "Net.Error.h"
#include "Net.IPv4.Define.h"
#include "Net.API.Enum.h"
#include "Net.API.Struct.h"


//UDP
//创建一个连接
int Net_UDP_Create(
		const char *Device_Name,
		uint32_t Rx_Buffer_Max_Length);
//释放一个连接
int Net_UDP_Close(int Handle);
//将连接绑定到指定IP和端口上
int Net_UDP_Bind(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information);
//连接对方IP和端口
int Net_UDP_Connect(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information);
//关闭这个连接
int Net_UDP_Disconnect(int Handle);
//接收数据
int Net_UDP_Recv(
		int Handle,
		Net_API_IP_Information_Type *P_IP_Information,
		uint8_t *DATA,
		uint32_t Size,
		int32_t TimeOut);
//发送数据
int Net_UDP_Send(
		int Handle,
		Net_API_IP_Information_Type *P_IP_Information,
		uint8_t *DATA,
		uint32_t Size);

//TCP
//创建一个连接
int Net_TCP_Create(
		const char *Device_Name,
		uint32_t Rx_Buffer_Max_Length);


//释放一个连接
int Net_TCP_Close(int Handle);

//将连接绑定到指定IP和端口上
int Net_TCP_Bind(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information);
//连接对方IP和端口
int Net_TCP_Connect(
		int Handle,
		const Net_API_IP_Information_Type *P_IP_Information);

//在当前端口上进行监听
int Net_TCP_Listen(
		int Handle,
		uint32_t Backlog);

//接受一个连接
int Net_TCP_Accept(
		int Handle,
		Net_API_IP_Information_Type *P_IP_Information,
		int32_t TimeOut);

//关闭一个连接的方向
int Net_TCP_Shutdown(
		int Handle,
		bool Shut_Rx,
		bool Shut_Tx);

//接收数据
int Net_TCP_Recv(
		int Handle,
		uint8_t *DATA,
		uint32_t Size,
		int Flags,
		int32_t TimeOut);

//发送数据
int Net_TCP_Send(
		int Handle,
		uint8_t *DATA,
		uint32_t Size,
		int Flags);

#endif /* NET_API_H_ */
