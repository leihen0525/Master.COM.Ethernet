/*
 * Protocol.TCP.Link.Timer.h
 *
 *  Created on: 2021年12月22日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_TCP_LINK_TIMER_H_
#define PROTOCOL_TCP_LINK_TIMER_H_

#include "Protocol.TCP.Link.Struct.h"

int Protocol_TCP_Link_Timer_Init(Net_Protocol_TCP_Link_Node_Type *P_Link_Node);

int Protocol_TCP_Link_Timer_DeInit(Net_Protocol_TCP_Link_Node_Type *P_Link_Node);


void Protocol_TCP_Link_Timer_Retransmission_Handle(void *Args);
void Protocol_TCP_Link_Timer_Persistent_Handle(void *Args);
void Protocol_TCP_Link_Timer_Keeplive_Handle(void *Args);
void Protocol_TCP_Link_Timer_TimeWait_Handle(void *Args);

#endif /* PROTOCOL_TCP_LINK_TIMER_H_ */
