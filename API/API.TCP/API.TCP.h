/*
 * API.TCP.h
 *
 *  Created on: 2021年4月25日
 *      Author: Master.HE
 */

#ifndef API_TCP_H_
#define API_TCP_H_

#include "Master.Stdint.h"
#include "Net.API.Enum.h"
#include "API.TCP.Struct.h"

int Net_TCP_Init(void);

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

int Net_TCP_Del_Link_Node(int Handle);

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

extern Net_API_TCP_DATA_Type Net_API_TCP_DATA;

#endif /* API_TCP_H_ */
