/*
 * Socket.Struct.h
 *
 *  Created on: 2019Äê12ÔÂ20ÈÕ
 *      Author: Master.HE
 */

#ifndef SOCKET_STRUCT_H_
#define SOCKET_STRUCT_H_

#include "Master.Stdint.h"
#include "Session/Session.h"

typedef struct
{
	int Handle							:29;

	Net_Session_DATA_Type Session_DATA;

}Net_Socket_DATA_Type;


#endif /* SOCKET_STRUCT_H_ */
