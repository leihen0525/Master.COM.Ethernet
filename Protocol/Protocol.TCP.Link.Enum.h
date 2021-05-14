/*
 * Protocol.TCP.Link.Enum.h
 *
 *  Created on: 2021年5月11日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_ENUM_H_
#define PROTOCOL_TCP_LINK_ENUM_H_

typedef enum
{
	Net_Protocol_TCP_Link_Condition_CLOSED				=0,			//起始点(关闭)

	Net_Protocol_TCP_Link_Condition_LISTEN,							//被动打开(监听)

	Net_Protocol_TCP_Link_Condition_SYN_SENT,						//主动打开
	Net_Protocol_TCP_Link_Condition_SYN_RCVD,						//

	Net_Protocol_TCP_Link_Condition_ESTABLISHED,					//数据传输状态

																	//被动关闭
	Net_Protocol_TCP_Link_Condition_CLOSE_WAIT,
	Net_Protocol_TCP_Link_Condition_LAST_ACK,

																	//主动关闭
	Net_Protocol_TCP_Link_Condition_FIN_WAIT_1,
	Net_Protocol_TCP_Link_Condition_FIN_WAIT_2,
	Net_Protocol_TCP_Link_Condition_CLOSING,
	Net_Protocol_TCP_Link_Condition_TIME_WAIT,

}Net_Protocol_TCP_Link_Condition_Type;

typedef enum
{
	Net_Protocol_TCP_Link_Queue_Listen_Syn,
	Net_Protocol_TCP_Link_Queue_Listen_Accept,
	Net_Protocol_TCP_Link_Queue_Link,
}Net_Protocol_TCP_Link_Queue_Type;


#endif /* PROTOCOL_TCP_LINK_ENUM_H_ */
