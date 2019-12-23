/*
 * Protocol.Struct.h
 *
 *  Created on: 2019Äê12ÔÂ20ÈÕ
 *      Author: Master.HE
 */

#ifndef PROTOCOL_STRUCT_H_
#define PROTOCOL_STRUCT_H_

#include "Protocol.TCP.Struct.h"
#include "Protocol.UDP.Struct.h"

typedef union
{
	Net_Protocol_TCP_Node_DATA_Type TCP;
	Net_Protocol_UDP_Node_DATA_Type UDP;

}Net_Protocol_Node_DATA_Type;

#endif /* PROTOCOL_STRUCT_H_ */
