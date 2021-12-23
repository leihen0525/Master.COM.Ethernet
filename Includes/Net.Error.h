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
	Net_Error_Exceed_Backlog_Link			=-108,					//超过Backlog定义的link数量
	Net_Error_Buff_OverFlow					=-107,					//缓冲区溢出
	Net_Error_Packet_Size_No_Match			=-106,					//数据包长度不匹配
	Net_Error_CheckSum_Fail					=-105,					//CheckSum失败

	Net_Error_Already_Connect				=-104,					//已经连接
	Net_Error_IP_Address_Type_Inconformity	=-103,					//IP地址类型和之前定义不一致
	Net_Error_No_Listen						=-102,					//没有监听
	Net_Error_No_Bind						=-101,					//没有绑定
	Net_Error_No_Connect					=-100,					//没有连接


}Net_Error_Type;

#endif /* NET_ERROR_H_ */
