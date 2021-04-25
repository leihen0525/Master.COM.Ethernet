/*
 * API.UDP.h
 *
 *  Created on: 2021年3月15日
 *      Author: Master.HE
 */

#ifndef API_UDP_H_
#define API_UDP_H_

#include "Net.API.Struct.h"
#include "API.UDP.Struct.h"

int Net_UDP_Init(void);

//创建一个连接
int Net_UDP_Create(
		const char *Device_Name,
		uint32_t Ring_Queue_Length);
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


extern Net_API_UDP_DATA_Type Net_API_UDP_DATA;

#endif /* API_UDP_H_ */
