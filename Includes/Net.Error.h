/*
 * Net.Error.h
 *
 *  Created on: 2021年5月6日
 *      Author: Master.HE
 */

#ifndef NET_ERROR_H_
#define NET_ERROR_H_

typedef enum
{
	Net_Error_No_Listen						=-102,					//没有监听
	Net_Error_No_Bind						=-101,					//没有绑定
	Net_Error_No_Connect					=-100,					//没有连接
}Net_Error_Type;

#endif /* NET_ERROR_H_ */
