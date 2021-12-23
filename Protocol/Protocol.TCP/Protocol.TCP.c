/*
 * Protocol.TCP.c
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */
#include "string.h"

#include "List.h"
#include "Define.h"
#include "Error.h"
#include "API.h"

#include "Net.Error.h"

#include "Core/Core.Define.h"
#include "Core/Core.h"

#include "IPv4/IPv4.Enum.h"
#include "IPv4/IPv4.h"
#include "Protocol.TCP.Enum.h"
#include "Protocol.TCP.Struct.h"

#include "Protocol.TCP.h"

#include "Protocol/Protocol.PORT.h"

#include "Protocol.TCP.Link.Define.h"
#include "Protocol.TCP.Link.h"

#include "API/API.TCP/API.TCP.h"

int Protocol_TCP_Init(Net_Core_Device_Node_Type *P_Net_Node,Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA)
{
	if(P_Net_Node==Null || P_Protocol_TCP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	int Err;

	Error_NoArgs_Return(Err,Protocol_PORT_Init(&P_Protocol_TCP_DATA->PORT_DATA));


	Error_NoArgs_Return(Err,Protocol_TCP_Link_Init(&P_Protocol_TCP_DATA->Link_DATA));

	P_Protocol_TCP_DATA->ISN=0;


	return Error_OK;
}

int Protocol_TCP_10MS(Net_Protocol_TCP_DATA_Type *P_Protocol_TCP_DATA)
{
	if(P_Protocol_TCP_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	P_Protocol_TCP_DATA->ISN++;

	return Error_OK;
}

//
int Protocol_TCP_Handle_Rx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_IPv4_Pseudo_Heade,
		uint8_t *P_Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size)
{
	int Err=Error_OK;

	if((IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Net_Node==Null
	|| (P_IPv4_Pseudo_Heade==Null && IP_Type==Net_IP_Address_IPv4)
	|| P_Protocol_TCP_Packet==Null
	|| Protocol_TCP_Packet_Size==0
	|| Protocol_TCP_Packet_Size<sizeof(Net_Protocol_TCP_Packet_Heade_Type))
	{
		return Error_Invalid_Parameter;
	}

	uint8_t *IP_Src=Null;

	if(IP_Type==Net_IP_Address_IPv4)
	{
		if(Net_Core_CheckSum(
				(uint16_t *)P_IPv4_Pseudo_Heade,
				sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
				(uint16_t *)P_Protocol_TCP_Packet,
				Protocol_TCP_Packet_Size)!=UINT16_REVERSE(0x0000))
		{
			return Net_Error_CheckSum_Fail;
		}
		IP_Src=P_IPv4_Pseudo_Heade->SRC_IPv4_Address;
	}
	else if(IP_Type==Net_IP_Address_IPv6)
	{
		//TODO IPv6 未实现
	}
	else
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)P_Protocol_TCP_Packet;

	//检查端口
	uint16_t PORT_Src=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->SRC_PORT);
	uint16_t PORT_Dest=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->DEST_PORT);

	uint32_t SEQ_Dest=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Sequence_Number);
	uint32_t ACK_Dest=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Acknowledgment_Number);

	Net_Protocol_TCP_Packet_Heade_Flags_Type Flags={.DATA=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->Flags)};

	Net_Protocol_TCP_Link_Queue_Type Queue;
	Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node=Null;
	Net_Protocol_TCP_Link_Node_Type *P_Link_Node=Null;

	//先在Link队列下寻找
	Error_NoArgs(Err,Protocol_TCP_Link_Find_Link_Node_From_Link_List(&P_Net_Node->Protocol_TCP_DATA.Link_DATA.Link, IP_Type, IP_Src, PORT_Src, PORT_Dest, &P_Link_Node))
	{
		if(Err!=Error_No_Find)
		{
			return Err;
		}
	}
	else
	{
		Queue=Net_Protocol_TCP_Link_Queue_Link;
	}

	//
	if(P_Link_Node==Null)
	{
		//查找LISTEN队列
		List_Find_Node_From_Symbol_2And(P_Net_Node->Protocol_TCP_DATA.Link_DATA.Listen.Head,NEXT,PORT,PORT_Dest,Condition,Net_Protocol_TCP_Link_Condition_LISTEN,P_Listen_Node);
		if(P_Listen_Node!=Null)
		{
			//确认当前节点是否在Syn队列
			Error_NoArgs(Err,Protocol_TCP_Link_Find_Link_Node_From_Listen_SynOrAccept_List(P_Listen_Node, Net_Protocol_TCP_Link_Queue_Listen_Syn, IP_Type, IP_Src, PORT_Src, &P_Link_Node))
			{
				if(Err==Error_No_Find)
				{
					//在Accept队列下寻找
					Error_NoArgs(Err,Protocol_TCP_Link_Find_Link_Node_From_Listen_SynOrAccept_List(P_Listen_Node, Net_Protocol_TCP_Link_Queue_Listen_Accept, IP_Type, IP_Src, PORT_Src, &P_Link_Node))
					{
						if(Err!=Error_No_Find)
						{
							return Err;
						}
					}
					else
					{
						Queue=Net_Protocol_TCP_Link_Queue_Listen_Accept;
					}
				}
				else
				{
					return Err;
				}
			}
			else
			{
				Queue=Net_Protocol_TCP_Link_Queue_Listen_Syn;
			}

//Server-Listen
			//请求连接
			if(Flags.Syn==1)
			{
				//新建一个Link
				if(P_Link_Node==Null)
				{
					//检查队列长度
					if(P_Listen_Node->Count>=P_Listen_Node->Backlog)
					{
						return Net_Error_Exceed_Backlog_Link;
					}

					Error_NoArgs_Return(Err,Protocol_TCP_Link_New_Syn_Node_To_ListenSyn_List(P_Listen_Node, &P_Link_Node));


		//			Error_Args(Temp_Link_Node->Handle,Handle_New())
		//			{
		//				Memory_Free(Temp_Link_Node);
		//				return ;
		//			}
					P_Link_Node->Server=true;
					P_Link_Node->IP_Type=IP_Type;

					P_Link_Node->Local_Info.PORT=PORT_Dest;
					P_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_SYN_RCVD;
					P_Link_Node->Local_Info.SEQ=P_Net_Node->Protocol_TCP_DATA.ISN;
					P_Link_Node->Local_Info.Window_Size=P_Listen_Node->Window_Size;

					P_Link_Node->Dest_Info.PORT=PORT_Src;
					P_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_SYN_SENT;
					P_Link_Node->Dest_Info.SEQ=SEQ_Dest;
					memcpy(P_Link_Node->Dest_Info.IP_Address,IP_Src,IP_Type);

					P_Link_Node->Local_Info.ACK=Net_Protocol_TCP_Link_LEN_SYN;
					//
					P_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_SYN;



					//进行第一次握手
					Error_NoArgs(Err,Protocol_TCP_Tx(
							IP_Type,
							P_Net_Node,
							P_Link_Node->Dest_Info.IP_Address,
							P_Link_Node->Local_Info.PORT,
							P_Link_Node->Dest_Info.PORT,
							P_Link_Node->Local_Info.SEQ,
							P_Link_Node->Dest_Info.SEQ,
							Net_Protocol_TCP_Packet_Flags_Syn|Net_Protocol_TCP_Packet_Flags_Ack,
							P_Link_Node->Local_Info.Window_Size,
							0,
							Null,
							0,
							Null,
							0))
					{
						Protocol_TCP_Link_Del_Link_Node_From_Listen_SynOrAccept_List(P_Listen_Node, Net_Protocol_TCP_Link_Queue_Listen_Syn, P_Link_Node, true);
						return Err;
					}


					//TODO 开启 超时重传计时器

					return Error_OK;
				}
				else
				{
					//已经存在
				}
			}
		}
		else
		{
			//假设自己是服务端 那么因为自己端口未监听 所以未找到
			//假设自己是客户端 那么继续下去
		}
	}

//Syn和Fin一次会消耗一次SEQ号 只有当ACK置位 ACK位才会+1

	if(P_Link_Node!=Null)
	{
		//ACK
		if(Flags.Ack==1)
		{
			//TODO  关闭 超时重传计时器

			if((P_Link_Node->Local_Info.SEQ+P_Link_Node->Local_Info.ACK)==ACK_Dest)
			{

			}
			else
			{
				//TODO ACK错误 出现丢帧
				goto Protocol_TCP_Handle_Rx_Exit1;
			}

		}



		//Server
		if(P_Link_Node->Server==true)
		{

			Error_NoArgs_Return(Err,Protocol_TCP_Server_Handle_Rx(P_Net_Node, IP_Type, IP_Src, P_Listen_Node, Queue, P_Link_Node, P_Protocol_TCP_Packet, Protocol_TCP_Packet_Size));

		}
		//Client
		else
		{

		}

	}
	else
	{
		//当前link不存在
	}



Protocol_TCP_Handle_Rx_Exit1:

	return Err;
}
int Protocol_TCP_Server_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IP_Address_Type IP_Type,
		uint8_t *P_Dest_IP,
		Net_Protocol_TCP_Listen_Node_Type *P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type Queue,
		Net_Protocol_TCP_Link_Node_Type *P_Link_Node,
		uint8_t *P_Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size)
{
	int Err;


	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)P_Protocol_TCP_Packet;


	//uint16_t PORT_Src=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->SRC_PORT);
	//uint16_t PORT_Dest=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->DEST_PORT);

	uint32_t SEQ_Dest=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Sequence_Number);
	uint32_t ACK_Dest=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Acknowledgment_Number);


	Net_Protocol_TCP_Packet_Heade_Flags_Type Flags={.DATA=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->Flags)};



	switch (P_Link_Node->Local_Info.Condition)
	{

		case Net_Protocol_TCP_Link_Condition_SYN_RCVD:
		{
			if(Flags.Ack==1)
			{
				if(Queue==Net_Protocol_TCP_Link_Queue_Listen_Syn)
				{
					//
					//if((Temp_Link_Node->Local_Info.SEQ+Temp_Link_Node->Local_Info.ACK)==ACK_Dest)
					{
						if(P_Link_Node->Dest_Info.SEQ==SEQ_Dest)
						{
							//TODO  关闭 超时重传计时器

							P_Link_Node->Local_Info.SEQ+=P_Link_Node->Local_Info.ACK;

							P_Link_Node->Local_Info.ACK=0;

							P_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;
							P_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;

							//将准备好的队列放进Listen_Accept
							Error_NoArgs(Err,Protocol_TCP_Link_Syn_Node_Move_Accept_List_From_Listen(P_Listen_Node, P_Link_Node))
							{
								return Err;//TODO 出错
							}
							else
							{
								Semaphore_Release(P_Listen_Node->Semaphore,1,Null);
							}


							//TODO 开启 保活计时器
						}
						else
						{

						}
					}
					//else
					{

					}

				}
				else
				{

				}
			}
			else
			{
				//对方没有收到SYN,ACK
				if(Flags.Syn==1)
				{
					if(Queue==Net_Protocol_TCP_Link_Queue_Listen_Syn)
					{
						P_Link_Node->Dest_Info.SEQ=SEQ_Dest;

						P_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_SYN;


						//进行第一次握手
						Protocol_TCP_Tx(
								IP_Type,
								P_Net_Node,
								P_Link_Node->Dest_Info.IP_Address,
								P_Link_Node->Local_Info.PORT,
								P_Link_Node->Dest_Info.PORT,
								P_Link_Node->Local_Info.SEQ,
								P_Link_Node->Dest_Info.SEQ,
								Net_Protocol_TCP_Packet_Flags_Syn|Net_Protocol_TCP_Packet_Flags_Ack,
								P_Link_Node->Local_Info.Window_Size,
								0,
								Null,
								0,
								Null,
								0);

						//TODO 重新开启 超时重传计时器


					}
				}
			}

//				if(Flags.Reset==1)
//				{
//
//				}
		}break;

		case Net_Protocol_TCP_Link_Condition_ESTABLISHED:
		{
			/*
			if(Flags.Ack==1)
			{
				if((uint32_t)(Temp_Link_Node->Local_Info.SEQ+Temp_Link_Node->Local_Info.ACK)==ACK_Dest)
				{
					if(Temp_Link_Node->Dest_Info.SEQ==SEQ_Dest)
					{
						//TODO  关闭 超时重传计时器



					}
				}
				else
				{
					//TODO ACK错误 出现丢帧
					return ;
				}
			}
			else
			{
				//
			}
			*/


			if(Flags.Fin==1)
			{
				if(P_Link_Node->Dest_Info.SEQ==SEQ_Dest)
				{
					P_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_FIN;

					P_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_CLOSE_WAIT;
					P_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_FIN_WAIT_1;


					//进行第一次握手
					Protocol_TCP_Tx(
							IP_Type,
							P_Net_Node,
							P_Link_Node->Dest_Info.IP_Address,
							P_Link_Node->Local_Info.PORT,
							P_Link_Node->Dest_Info.PORT,
							P_Link_Node->Local_Info.SEQ,
							P_Link_Node->Dest_Info.SEQ,
							Net_Protocol_TCP_Packet_Flags_Ack,
							P_Link_Node->Local_Info.Window_Size,
							0,
							Null,
							0,
							Null,
							0);
					P_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_FIN_WAIT_2;
					//TODO


					P_Link_Node->Local_Info.ACK=Net_Protocol_TCP_Link_LEN_FIN;

					P_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_LAST_ACK;
					P_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_TIME_WAIT;


					//进行第一次握手
					Protocol_TCP_Tx(
							IP_Type,
							P_Net_Node,
							P_Link_Node->Dest_Info.IP_Address,
							P_Link_Node->Local_Info.PORT,
							P_Link_Node->Dest_Info.PORT,
							P_Link_Node->Local_Info.SEQ,
							P_Link_Node->Dest_Info.SEQ,
							Net_Protocol_TCP_Packet_Flags_Fin|Net_Protocol_TCP_Packet_Flags_Ack,
							P_Link_Node->Local_Info.Window_Size,
							0,
							Null,
							0,
							Null,
							0);

				}
				else
				{

				}
			}
		}break;
		case Net_Protocol_TCP_Link_Condition_CLOSE_WAIT:
		{

		}break;
		case Net_Protocol_TCP_Link_Condition_LAST_ACK:
		{
			if(Flags.Ack==1)
			{
				//if((uint32_t)(Temp_Link_Node->Local_Info.SEQ+Temp_Link_Node->Local_Info.ACK)==ACK_Dest)
				{
					if(P_Link_Node->Dest_Info.SEQ==SEQ_Dest)
					{
						//TODO  关闭 超时重传计时器

						P_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_CLOSED;
						P_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_CLOSED;


						//TODO 删除这个节点


						if(Queue==Net_Protocol_TCP_Link_Queue_Listen_Syn)
						{
							Error_NoArgs(Err,Protocol_TCP_Link_Del_Link_Node_From_Listen_SynOrAccept_List(P_Listen_Node, Net_Protocol_TCP_Link_Queue_Listen_Syn,P_Link_Node,false))
							{
								return Err;
							}
						}
						else if(Queue==Net_Protocol_TCP_Link_Queue_Listen_Accept)
						{
							Error_NoArgs(Err,Protocol_TCP_Link_Del_Link_Node_From_Listen_SynOrAccept_List(P_Listen_Node, Net_Protocol_TCP_Link_Queue_Listen_Accept,P_Link_Node,false))
							{
								return Err;
							}
							//???
							Semaphore_Wait(P_Listen_Node->Semaphore,1);
						}
						else if(Queue==Net_Protocol_TCP_Link_Queue_Link)
						{
							Error_NoArgs(Err,Protocol_TCP_Link_Del_Link_Node_From_Link_List(&P_Net_Node->Protocol_TCP_DATA.Link_DATA.Link, P_Link_Node,false))
							{
								return Err;
							}
							//
							Net_TCP_Del_Link_Node(P_Link_Node->Handle);

						}
						Memory_Free(P_Link_Node);
					}
				}
				//else
				{
					//TODO ACK错误 出现丢帧
					//return ;
				}
			}
			else
			{
				//
			}


		}break;
		default:
		{

		}break;
	}
}
/*
int Protocol_TCP_Client_Handle_Rx()
{
	switch (Temp_Link_Node->Local_Info.Condition)
	{
		case Net_Protocol_TCP_Link_Condition_CLOSED:
		{

		}break;
		case Net_Protocol_TCP_Link_Condition_SYN_SENT:
		{
			if(Flags.Syn==1)
			{
				if(Flags.Ack==1)
				{
					Temp_Link_Node->Dest_Info.SEQ=SEQ_Dest;
					Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;

					Temp_Link_Node->Local_Info.SEQ+=Temp_Link_Node->Local_Info.ACK;
					Temp_Link_Node->Local_Info.ACK=0;
					Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;

					Temp_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_SYN;


					//进行第一次握手
					Protocol_TCP_Tx(
							IP_Type,
							P_Net_Node,
							Temp_Link_Node->Dest_Info.IP_Address,
							Temp_Link_Node->Local_Info.PORT,
							Temp_Link_Node->Dest_Info.PORT,
							Temp_Link_Node->Local_Info.SEQ,
							Temp_Link_Node->Dest_Info.SEQ,
							Net_Protocol_TCP_Packet_Flags_Ack,
							Temp_Link_Node->Local_Info.Window_Size,
							0,
							Null,
							0,
							Null,
							0);
					//TODO 开启

				}
			}
		}break;
		case Net_Protocol_TCP_Link_Condition_ESTABLISHED:
		{
			if(Flags.Syn==1)
			{
				if(Flags.Ack==1)
				{
					Temp_Link_Node->Dest_Info.SEQ=SEQ_Dest;
					//Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;

					//Temp_Link_Node->Local_Info.SEQ+=Temp_Link_Node->Local_Info.ACK;
					Temp_Link_Node->Local_Info.ACK=0;
					//Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;

					Temp_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_SYN;

					//进行第一次握手
					Protocol_TCP_Tx(
							IP_Type,
							P_Net_Node,
							Temp_Link_Node->Dest_Info.IP_Address,
							Temp_Link_Node->Local_Info.PORT,
							Temp_Link_Node->Dest_Info.PORT,
							Temp_Link_Node->Local_Info.SEQ,
							Temp_Link_Node->Dest_Info.SEQ,
							Net_Protocol_TCP_Packet_Flags_Ack,
							Temp_Link_Node->Local_Info.Window_Size,
							0,
							Null,
							0,
							Null,
							0);
					//TODO 开启
				}
			}
		}break;
		case Net_Protocol_TCP_Link_Condition_FIN_WAIT_1:
		{

		}break;
		case Net_Protocol_TCP_Link_Condition_FIN_WAIT_2:
		{

		}break;
		case Net_Protocol_TCP_Link_Condition_CLOSING:
		{

		}break;
		case Net_Protocol_TCP_Link_Condition_TIME_WAIT:
		{

		}break;
		default:
		{

		}break;
	}
}
*/
/*
//查找监听节点
int Protocol_TCP_Find_LISTEN_Node(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IP_Address_Type IP_Type,
		uint8_t *IP_Src,
		Net_Protocol_TCP_Packet_Heade_Type *P_TCP_Packet_Heade,
		Net_Protocol_TCP_Listen_Node_Type **P_Listen_Node,
		Net_Protocol_TCP_Link_Queue_Type *P_Queue,
		Net_Protocol_TCP_Link_Node_Type **P_Link_Node)
{
	if(P_Net_Node==Null
	|| (IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| IP_Src==Null
	|| P_TCP_Packet_Heade==Null
	|| P_Listen_Node==Null
	|| P_Queue==Null
	|| P_Link_Node==Null)
	{
		return Error_Invalid_Parameter;
	}
	uint16_t PORT_Src=UINT16_REVERSE(P_TCP_Packet_Heade->SRC_PORT);
	uint16_t PORT_Dest=UINT16_REVERSE(P_TCP_Packet_Heade->DEST_PORT);

	*P_Listen_Node=Null;
	*P_Link_Node=Null;


	List_Find_Node_From_Symbol_2And(P_Net_Node->Protocol_TCP_DATA.Link_DATA.Listen.Head,NEXT,PORT,PORT_Dest,Condition,Net_Protocol_TCP_Link_Condition_LISTEN,(*P_Listen_Node));
	if((*P_Listen_Node)!=Null)
	{
		//
		(*P_Link_Node)=(*P_Listen_Node)->Syn.Head;
		while((*P_Link_Node)!=Null)
		{
			if((*P_Link_Node)->IP_Type==IP_Type)
			{
				if((*P_Link_Node)->Dest_Info.PORT==PORT_Src)
				{
					if(memcmp((*P_Link_Node)->Dest_Info.IP_Address,IP_Src,IP_Type)==0)
					{
						*P_Queue=Net_Protocol_TCP_Link_Queue_Listen_Syn;
						break;
					}
				}
			}
			(*P_Link_Node)=(*P_Link_Node)->NEXT;
		}

		//
		if((*P_Link_Node)==Null)
		{
			(*P_Link_Node)=(*P_Listen_Node)->Accept.Head;
			while((*P_Link_Node)!=Null)
			{
				if((*P_Link_Node)->IP_Type==IP_Type)
				{
					if((*P_Link_Node)->Dest_Info.PORT==PORT_Src)
					{
						if(memcmp((*P_Link_Node)->Dest_Info.IP_Address,IP_Src,IP_Type)==0)
						{
							*P_Queue=Net_Protocol_TCP_Link_Queue_Listen_Accept;
							break;
						}
					}
				}
				(*P_Link_Node)=(*P_Link_Node)->NEXT;
			}
		}
	}
	else
	{
		//当前不属于本地监听端口的link
	}

	if((*P_Link_Node)==Null)
	{
		(*P_Link_Node)=P_Net_Node->Protocol_TCP_DATA.Link_DATA.Link.Head;

		while((*P_Link_Node)!=Null)
		{
			if((*P_Link_Node)->IP_Type==IP_Type)
			{
				if((*P_Link_Node)->Local_Info.PORT==PORT_Dest)
				{
					if((*P_Link_Node)->Dest_Info.PORT==PORT_Src)
					{
						if(memcmp((*P_Link_Node)->Dest_Info.IP_Address,IP_Src,IP_Type)==0)
						{
							*P_Queue=Net_Protocol_TCP_Link_Queue_Link;
							break;
						}
					}
				}

			}
			(*P_Link_Node)=(*P_Link_Node)->NEXT;
		}
	}

	return Error_OK;
}
*/
//发送一帧TCP报文
int Protocol_TCP_Tx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *P_DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint32_t SEQ,
		uint32_t ACKN,
		uint16_t Flags,
		uint16_t Windows,
		uint16_t Urgent_Pointer,
		uint8_t *P_Option,
		uint16_t Option_Size,
		uint8_t *P_DATA,
		uint16_t DATA_Size)
{
	if((IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Net_Node==Null
	|| P_DEST_IP_Address==Null
	|| (Option_Size!=0 && P_Option==Null)
	|| (Option_Size%4)!=0
	|| Option_Size>40
	|| (DATA_Size!=0 && P_DATA==Null))
	{
		return Error_Invalid_Parameter;
	}

	int Err=Error_OK;

	Net_IPv4_Packet_Pseudo_Heade_Type *P_IPv4_Pseudo_Heade=Null;
	uint8_t *Protocol_TCP_Packet=Null;
	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=Null;

	if(IP_Type==Net_IP_Address_IPv4)
	{
		P_IPv4_Pseudo_Heade=Memory_Malloc(sizeof(Net_IPv4_Packet_Pseudo_Heade_Type));

		if(P_IPv4_Pseudo_Heade==Null)
		{
			Err=Error_Allocation_Memory_Failed;
			goto Protocol_TCP_Tx_Exit1;
		}
		if((Err=IPv4_Pseudo_Heade_Init(
				P_IPv4_Pseudo_Heade,
				P_Net_Node->IPv4_DATA.IP_Address.IP.Address,
				P_DEST_IP_Address,
				Net_IPv4_Protocol_TCP,
				sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size))!=Error_OK)
		{
			goto Protocol_TCP_Tx_Exit2;
		}
	}
	else if(IP_Type==Net_IP_Address_IPv6)
	{
		//TODO IPv6 未实现
	}
	else
	{
		return Error_Invalid_Parameter;
	}

	Protocol_TCP_Packet=Memory_Malloc(sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size);

	if(Protocol_TCP_Packet==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_TCP_Tx_Exit2;
	}

	Net_Protocol_TCP_Packet_Heade_Flags_Type Temp_Flags=(Net_Protocol_TCP_Packet_Heade_Flags_Type){.DATA=Flags};

	P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)Protocol_TCP_Packet;

	P_Protocol_TCP_Packet_Heade->SRC_PORT=UINT16_REVERSE(SRC_PORT);
	P_Protocol_TCP_Packet_Heade->DEST_PORT=UINT16_REVERSE(DEST_PORT);
	P_Protocol_TCP_Packet_Heade->Sequence_Number=UINT32_REVERSE(SEQ);
	P_Protocol_TCP_Packet_Heade->Acknowledgment_Number=UINT32_REVERSE(ACKN);
	Temp_Flags.Header_Length=(sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size)/4;
	P_Protocol_TCP_Packet_Heade->Flags=UINT16_REVERSE(Temp_Flags.DATA);
	P_Protocol_TCP_Packet_Heade->Window_Size=UINT16_REVERSE(Windows);
	P_Protocol_TCP_Packet_Heade->CheckSum=UINT16_REVERSE(0x0000);
	P_Protocol_TCP_Packet_Heade->Urgent_Pointer=UINT16_REVERSE(Urgent_Pointer);

	//Option
	if(Option_Size!=0)
	{
		memcpy(&Protocol_TCP_Packet[sizeof(Net_Protocol_TCP_Packet_Heade_Type)],P_Option,Option_Size);
	}
	//DATA
	if(DATA_Size!=0)
	{
		memcpy(&Protocol_TCP_Packet[sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size],P_DATA,DATA_Size);
	}

	if(IP_Type==Net_IP_Address_IPv4)
	{
		uint16_t CheckSum=Net_Core_CheckSum(
				(uint16_t *)P_IPv4_Pseudo_Heade,
				sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
				(uint16_t *)Protocol_TCP_Packet,
				sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size);
		P_Protocol_TCP_Packet_Heade->CheckSum=UINT16_REVERSE(CheckSum);

		Err=IPv4_Tx_Pend_DATA(
					P_Net_Node,
					Net_IPv4_Protocol_TCP,
					P_DEST_IP_Address,
					Protocol_TCP_Packet,
					sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size,
					true);
	}
	else if(IP_Type==Net_IP_Address_IPv6)
	{
		//TODO IPv6 未实现
	}
	else
	{
		return Error_Invalid_Parameter;
	}
	if(Err==Error_OK)
	{
		goto Protocol_TCP_Tx_Exit2;
	}
	else
	{
		goto Protocol_TCP_Tx_Exit3;
	}

Protocol_TCP_Tx_Exit3:
	Memory_Free(Protocol_TCP_Packet);

Protocol_TCP_Tx_Exit2:
	Memory_Free(P_IPv4_Pseudo_Heade);

Protocol_TCP_Tx_Exit1:
	return Err;
}

