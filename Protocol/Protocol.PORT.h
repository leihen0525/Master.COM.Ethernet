/*
 * Protocol.PORT.h
 *
 *  Created on: 2020年3月16日
 *      Author: Master.HE
 */

#ifndef PROTOCOL_PORT_H_
#define PROTOCOL_PORT_H_

#include "Protocol.PORT.Struct.h"

int Protocol_PORT_Init(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA);


int Protocol_PORT_Add(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type Flag);
int Protocol_PORT_New(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,Net_Protocol_PORT_Node_Flag_Type Flag,uint16_t Begin_PORT,uint16_t End_PORT);
int Protocol_PORT_Del(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,uint16_t PORT);
int Protocol_PORT_Find(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type *Flag);
int Protocol_PORT_Clear(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA);

static int Protocol_PORT_List_Init(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA);
static int Protocol_PORT_List_Add(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type Flag);
static int Protocol_PORT_List_New(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,Net_Protocol_PORT_Node_Flag_Type Flag,uint16_t Begin_PORT,uint16_t End_PORT);
static int Protocol_PORT_List_Del(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,uint16_t PORT);
static int Protocol_PORT_List_Find(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type *Flag);
static int Protocol_PORT_List_Clear(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA);



#endif /* PROTOCOL_PORT_H_ */
