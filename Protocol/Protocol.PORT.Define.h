/*
 * Protocol.PORT.Define.h
 *
 *  Created on: 2020年3月16日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_PORT_DEFINE_H_
#define PROTOCOL_PORT_DEFINE_H_

//公共端口
#define Net_Protocol_PORT_Well_Known_Ports_Begin					(0)
#define Net_Protocol_PORT_Well_Known_Ports_End						(1023)

//注册端口
#define Net_Protocol_PORT_Registered_Ports_Begin					(1024)
#define Net_Protocol_PORT_Registered_Ports_End						(49151)

//动态或者私有端口
#define Net_Protocol_PORT_Dynamic_Or_Private_Ports_Begin			(49152)
#define Net_Protocol_PORT_Dynamic_Or_Private_Ports_End				(65535)


#define Net_Protocol_PORT_Hash_Group_Number							(16)
#define Net_Protocol_PORT_Hash_Group_SHIFT							(12)
#define Net_Protocol_PORT_Hash_Group_Number_Mask					(0xF000)
#define Net_Protocol_PORT_Hash_Sub_Potr_Mask						(0x0FFF)

#define Net_Protocol_PORT_Hash_Get_Group_Index(x)					((x&Net_Protocol_PORT_Hash_Group_Number_Mask)>>Net_Protocol_PORT_Hash_Group_SHIFT)
#define Net_Protocol_PORT_Hash_Get_Sub_Potr(n)						(n&Net_Protocol_PORT_Hash_Sub_Potr_Mask)

#define Net_Protocol_PORT_Hash_Set_Port(x,n)						(((x<<Net_Protocol_PORT_Hash_Group_SHIFT)&Net_Protocol_PORT_Hash_Group_Number_Mask)|(n&Net_Protocol_PORT_Hash_Sub_Potr_Mask))

#endif /* PROTOCOL_PORT_DEFINE_H_ */
