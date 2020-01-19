/*
 * Session.h
 *
 *  Created on: 2019Äê10ÔÂ15ÈÕ
 *      Author: Master.HE
 */

#ifndef IPV4_SESSION_H_
#define IPV4_SESSION_H_

#include "Master.Stdint.h"
//#include "Core/Core.Struct.h"
#include "Net.Core.Struct.h"
#include "Protocol/Protocol.Struct.h"
#include "Session.Struct.h"



int Session_Init(Net_Session_DATA_Type *P_Session_DATA);

int Session_Switch_List(
		Net_Session_DATA_Type *P_Session_DATA,
		Net_Session_Protocol_Type Protocol,
		Net_Session_Node_List_Type **P_List);

int Session_Add_Node(
		Net_Session_DATA_Type *P_Session_DATA,
		Net_Session_Handle_Type Handle,

		Net_Session_Protocol_Type Protocol,

		Net_Session_Node_DATA_Type **P_Node_DATA);
/*
int Session_Del_Node(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Core_Address_DATA_Type *P_Address_DATA,
		Net_Session_Protocol_Type Protocol,
		Net_Session_Direction_Type Direction,
		Net_Protocol_Node_DATA_Type **Protocol_Node_DATA);
*/
int Session_Find_Node(
		Net_Session_DATA_Type *P_Session_DATA,

		Net_Session_Protocol_Type Protocol,

		Net_Session_Handle_Type *P_Handle,

		Net_Core_Address_DATA_Type *P_Address_DATA,

		Net_Session_Node_DATA_Type **P_Node_DATA);


#endif /* IPV4_SESSION_H_ */
