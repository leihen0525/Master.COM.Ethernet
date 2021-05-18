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

#include "Core/Core.Define.h"
#include "Core/Core.h"

#include "IPv4/IPv4.Enum.h"
#include "IPv4/IPv4.h"

#include "Protocol.TCP.Struct.h"

#include "Protocol.TCP.h"

#include "Protocol.PORT.h"

#include "Protocol.TCP.Link.Define.h"
#include "Protocol.TCP.Link.h"

#include "API/API.TCP.h"

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


void Protocol_TCP_Handle_Rx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade,
		uint8_t *Protocol_TCP_Packet,
		uint16_t Protocol_TCP_Packet_Size)
{
	int Err;
	if((IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Net_Node==Null
	|| P_Pseudo_Heade==Null
	|| Protocol_TCP_Packet==Null
	|| Protocol_TCP_Packet_Size==0
	|| Protocol_TCP_Packet_Size<sizeof(Net_Protocol_TCP_Packet_Heade_Type))
	{
		return ;
	}

	if(Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)Protocol_TCP_Packet,
			Protocol_TCP_Packet_Size)!=UINT16_REVERSE(0x0000))
	{
		return ;
	}

	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)Protocol_TCP_Packet;

	//检查端口
	uint16_t PORT_Src=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->SRC_PORT);
	uint16_t PORT_Dest=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->DEST_PORT);

	uint32_t SEQ_Dest=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Sequence_Number);
	uint32_t ACK_Dest=UINT32_REVERSE(P_Protocol_TCP_Packet_Heade->Acknowledgment_Number);

	uint8_t *IP_Src=P_Pseudo_Heade->SRC_IPv4_Address;

	Net_Protocol_TCP_Packet_Heade_Flags_Type Flags={
														.DATA=UINT16_REVERSE(P_Protocol_TCP_Packet_Heade->Flags)
													};


	Net_Protocol_TCP_Link_Queue_Type Queue;
	Net_Protocol_TCP_Listen_Node_Type *Temp_Listen_Node=Null;
	Net_Protocol_TCP_Link_Node_Type *Temp_Link_Node=Null;

	//查找监听节点
	List_Find_Node_From_Symbol_2And(P_Net_Node->Protocol_TCP_DATA.Link_DATA.Listen.Head,NEXT,PORT,PORT_Dest,Condition,Net_Protocol_TCP_Link_Condition_LISTEN,Temp_Listen_Node);
	if(Temp_Listen_Node!=Null)
	{
		//
		Temp_Link_Node=Temp_Listen_Node->Syn.Head;
		while(Temp_Link_Node!=Null)
		{
			if(Temp_Link_Node->IP_Type==IP_Type)
			{
				if(Temp_Link_Node->Dest_Info.PORT==PORT_Src)
				{
					if(memcmp(Temp_Link_Node->Dest_Info.IP_Address,IP_Src,IP_Type)==0)
					{
						Queue=Net_Protocol_TCP_Link_Queue_Listen_Syn;
						break;
					}
				}
			}
			Temp_Link_Node=Temp_Link_Node->NEXT;
		}

		//
		if(Temp_Link_Node==Null)
		{
			Temp_Link_Node=Temp_Listen_Node->Accept.Head;
			while(Temp_Link_Node!=Null)
			{
				if(Temp_Link_Node->IP_Type==IP_Type)
				{
					if(Temp_Link_Node->Dest_Info.PORT==PORT_Src)
					{
						if(memcmp(Temp_Link_Node->Dest_Info.IP_Address,IP_Src,IP_Type)==0)
						{
							Queue=Net_Protocol_TCP_Link_Queue_Listen_Accept;
							break;
						}
					}
				}
				Temp_Link_Node=Temp_Link_Node->NEXT;
			}
		}
	}
	else
	{

	}
	if(Temp_Link_Node==Null)
	{
		Temp_Link_Node=P_Net_Node->Protocol_TCP_DATA.Link_DATA.Link.Head;

		while(Temp_Link_Node!=Null)
		{
			if(Temp_Link_Node->IP_Type==IP_Type)
			{
				if(Temp_Link_Node->Local_Info.PORT==PORT_Dest)
				{
					if(Temp_Link_Node->Dest_Info.PORT==PORT_Src)
					{
						if(memcmp(Temp_Link_Node->Dest_Info.IP_Address,IP_Src,IP_Type)==0)
						{
							Queue=Net_Protocol_TCP_Link_Queue_Link;
							break;
						}
					}
				}

			}
			Temp_Link_Node=Temp_Link_Node->NEXT;
		}
	}

//Syn和Fin一次会消耗一次SEQ号 只有当ACK置位 ACK位才会+1
	if(Temp_Listen_Node!=Null)
	{
//Server-Listen
		//请求连接
		if(Flags.Syn==1)
		{
			//新建一个Link
			if(Temp_Link_Node==Null)
			{
				//检查队列长度
				if(Temp_Listen_Node->Count>=Temp_Listen_Node->Backlog)
				{
					return ;
				}
				Temp_Link_Node=Memory_Malloc(sizeof(Net_Protocol_TCP_Link_Node_Type));
				if(Temp_Link_Node==Null)
				{
					return ;
				}
	//			Error_Args(Temp_Link_Node->Handle,Handle_New())
	//			{
	//				Memory_Free(Temp_Link_Node);
	//				return ;
	//			}
				Temp_Link_Node->Server=true;
				Temp_Link_Node->IP_Type=IP_Type;

				Temp_Link_Node->Local_Info.PORT=PORT_Dest;
				Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_SYN_RCVD;
				Temp_Link_Node->Local_Info.SEQ=P_Net_Node->Protocol_TCP_DATA.ISN;
				Temp_Link_Node->Local_Info.Window_Size=Temp_Listen_Node->Window_Size;

				Temp_Link_Node->Dest_Info.PORT=PORT_Src;
				Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_SYN_SENT;
				Temp_Link_Node->Dest_Info.SEQ=SEQ_Dest;
				memcpy(Temp_Link_Node->Dest_Info.IP_Address,IP_Src,IP_Type);

				Temp_Link_Node->Local_Info.ACK=Net_Protocol_TCP_Link_LEN_SYN;
				//
				Temp_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_SYN;


				Temp_Listen_Node->Syn.Count++;
				Temp_Listen_Node->Count++;
				List_Add_Node_To_End(Temp_Listen_Node->Syn.Head,Temp_Listen_Node->Syn.End,NEXT,Temp_Link_Node);

				Net_Protocol_TCP_Packet_Heade_Flags_Type Temp_Flags={.DATA=0};
				Temp_Flags.Syn=1;
				Temp_Flags.Ack=1;
				//进行第一次握手
				Protocol_TCP_Tx(
						IP_Type,
						P_Net_Node,
						Temp_Link_Node->Dest_Info.IP_Address,
						Temp_Link_Node->Local_Info.PORT,
						Temp_Link_Node->Dest_Info.PORT,
						Temp_Link_Node->Local_Info.SEQ,
						Temp_Link_Node->Dest_Info.SEQ,
						Temp_Flags,
						Temp_Link_Node->Local_Info.Window_Size,
						0,
						Null,
						0,
						Null,
						0);


				//TODO 开启 超时重传计时器

				return ;
			}
			//已经存在
			else
			{

			}
		}
	}
	else
	{

	}


	if(Temp_Link_Node!=Null)
	{
		//Server
		if(Temp_Link_Node->Server==true)
		{
			switch (Temp_Link_Node->Local_Info.Condition)
			{

				case Net_Protocol_TCP_Link_Condition_SYN_RCVD:
				{
					if(Flags.Ack==1)
					{
						if(Queue==Net_Protocol_TCP_Link_Queue_Listen_Syn)
						{
							//
							if((Temp_Link_Node->Local_Info.SEQ+Temp_Link_Node->Local_Info.ACK)==ACK_Dest)
							{
								if(Temp_Link_Node->Dest_Info.SEQ==SEQ_Dest)
								{
									//TODO  关闭 超时重传计时器

									Temp_Link_Node->Local_Info.SEQ+=Temp_Link_Node->Local_Info.ACK;

									Temp_Link_Node->Local_Info.ACK=0;

									Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;
									Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_ESTABLISHED;

									//将准备好的队列放进Listen_Accept
									bool Del_OK=false;
									List_Del_Node_From_Pointer(Del,Net_Protocol_TCP_Link_Node_Type,Temp_Listen_Node->Syn.Head,Temp_Listen_Node->Syn.End,NEXT,Temp_Link_Node,Del_OK);
									if(Del_OK==true)
									{
										Temp_Listen_Node->Syn.Count--;

										Temp_Listen_Node->Accept.Count++;
										List_Add_Node_To_End(Temp_Listen_Node->Accept.Head,Temp_Listen_Node->Accept.End,NEXT,Temp_Link_Node);

										Semaphore_Release(Temp_Listen_Node->Semaphore,1,Null);

									}

									//TODO 开启 保活计时器
								}
								else
								{

								}
							}
							else
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
								Temp_Link_Node->Dest_Info.SEQ=SEQ_Dest;

								Temp_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_SYN;

								Net_Protocol_TCP_Packet_Heade_Flags_Type Temp_Flags={.DATA=0};
								Temp_Flags.Syn=1;
								Temp_Flags.Ack=1;
								//进行第一次握手
								Protocol_TCP_Tx(
										IP_Type,
										P_Net_Node,
										Temp_Link_Node->Dest_Info.IP_Address,
										Temp_Link_Node->Local_Info.PORT,
										Temp_Link_Node->Dest_Info.PORT,
										Temp_Link_Node->Local_Info.SEQ,
										Temp_Link_Node->Dest_Info.SEQ,
										Temp_Flags,
										Temp_Link_Node->Local_Info.Window_Size,
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



					if(Flags.Fin==1)
					{
						if(Temp_Link_Node->Dest_Info.SEQ==SEQ_Dest)
						{
							Temp_Link_Node->Dest_Info.SEQ+=Net_Protocol_TCP_Link_LEN_FIN;

							Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_CLOSE_WAIT;
							Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_FIN_WAIT_1;


							Net_Protocol_TCP_Packet_Heade_Flags_Type Temp_Flags={.DATA=0};
							Temp_Flags.Ack=1;
							//进行第一次握手
							Protocol_TCP_Tx(
									IP_Type,
									P_Net_Node,
									Temp_Link_Node->Dest_Info.IP_Address,
									Temp_Link_Node->Local_Info.PORT,
									Temp_Link_Node->Dest_Info.PORT,
									Temp_Link_Node->Local_Info.SEQ,
									Temp_Link_Node->Dest_Info.SEQ,
									Temp_Flags,
									Temp_Link_Node->Local_Info.Window_Size,
									0,
									Null,
									0,
									Null,
									0);
							Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_FIN_WAIT_2;
							//TODO


							Temp_Link_Node->Local_Info.ACK=Net_Protocol_TCP_Link_LEN_FIN;

							Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_LAST_ACK;
							Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_TIME_WAIT;

							Temp_Flags=(Net_Protocol_TCP_Packet_Heade_Flags_Type){.DATA=0};
							Temp_Flags.Fin=1;
							Temp_Flags.Ack=1;
							//进行第一次握手
							Protocol_TCP_Tx(
									IP_Type,
									P_Net_Node,
									Temp_Link_Node->Dest_Info.IP_Address,
									Temp_Link_Node->Local_Info.PORT,
									Temp_Link_Node->Dest_Info.PORT,
									Temp_Link_Node->Local_Info.SEQ,
									Temp_Link_Node->Dest_Info.SEQ,
									Temp_Flags,
									Temp_Link_Node->Local_Info.Window_Size,
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
						if((uint32_t)(Temp_Link_Node->Local_Info.SEQ+Temp_Link_Node->Local_Info.ACK)==ACK_Dest)
						{
							if(Temp_Link_Node->Dest_Info.SEQ==SEQ_Dest)
							{
								//TODO  关闭 超时重传计时器

								Temp_Link_Node->Dest_Info.Condition=Net_Protocol_TCP_Link_Condition_CLOSED;
								Temp_Link_Node->Local_Info.Condition=Net_Protocol_TCP_Link_Condition_CLOSED;


								//TODO 删除这个节点

								Error_NoArgs(Err,Protocol_TCP_Link_Del(&P_Net_Node->Protocol_TCP_DATA.Link_DATA, Queue, Temp_Listen_Node, Temp_Link_Node))
								{
									return ;
								}

								if(Queue==Net_Protocol_TCP_Link_Queue_Listen_Accept)
								{
									//???
									Semaphore_Wait(Temp_Listen_Node->Semaphore,1);
								}
								else if(Queue==Net_Protocol_TCP_Link_Queue_Link)
								{
									//
									Net_TCP_Del_Link_Node(Temp_Link_Node->Handle);

								}
								Memory_Free(Temp_Link_Node);
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


				}break;
				default:
				{

				}break;
			}


		}
		//Client
		else
		{
			switch (Temp_Link_Node->Local_Info.Condition)
			{
				case Net_Protocol_TCP_Link_Condition_CLOSED:
				{

				}break;
				case Net_Protocol_TCP_Link_Condition_SYN_SENT:
				{

				}break;
				case Net_Protocol_TCP_Link_Condition_ESTABLISHED:
				{

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

	}
	else
	{

	}

























	return ;
}
//发送一帧TCP报文
int Protocol_TCP_Tx(
		Net_IP_Address_Type IP_Type,
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IP_Address,
		uint16_t SRC_PORT,
		uint16_t DEST_PORT,
		uint32_t SEQ,
		uint32_t ACKN,
		Net_Protocol_TCP_Packet_Heade_Flags_Type Flags,
		uint16_t Windows,
		uint16_t Urgent_Pointer,
		uint8_t *Option,
		uint16_t Option_Size,
		uint8_t *DATA,
		uint16_t DATA_Size)
{
	if((IP_Type!=Net_IP_Address_IPv4 && IP_Type!=Net_IP_Address_IPv6)
	|| P_Net_Node==Null
	|| DEST_IP_Address==Null
	|| (Option_Size!=0 && Option==Null)
	|| (Option_Size%4)!=0
	|| Option_Size>40
	|| (DATA_Size!=0 && DATA==Null))
	{
		return Error_Invalid_Parameter;
	}

	int Err=Error_OK;

	Net_IPv4_Packet_Pseudo_Heade_Type *P_Pseudo_Heade=Memory_Malloc(sizeof(Net_IPv4_Packet_Pseudo_Heade_Type));

	if(P_Pseudo_Heade==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_TCP_Tx_Exit1;
	}
	if((Err=IPv4_Pseudo_Heade_Init(
			P_Pseudo_Heade,
			P_Net_Node->IPv4_DATA.IP_Address.IP.Address,
			DEST_IP_Address,
			Net_IPv4_Protocol_TCP,
			sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size))!=Error_OK)
	{
		goto Protocol_TCP_Tx_Exit2;
	}

	uint8_t *Protocol_TCP_Packet=Memory_Malloc(sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size);

	if(Protocol_TCP_Packet==Null)
	{
		Err=Error_Allocation_Memory_Failed;
		goto Protocol_TCP_Tx_Exit2;
	}

	Net_Protocol_TCP_Packet_Heade_Type *P_Protocol_TCP_Packet_Heade=(Net_Protocol_TCP_Packet_Heade_Type *)Protocol_TCP_Packet;

	P_Protocol_TCP_Packet_Heade->SRC_PORT=UINT16_REVERSE(SRC_PORT);
	P_Protocol_TCP_Packet_Heade->DEST_PORT=UINT16_REVERSE(DEST_PORT);
	P_Protocol_TCP_Packet_Heade->Sequence_Number=UINT32_REVERSE(SEQ);
	P_Protocol_TCP_Packet_Heade->Acknowledgment_Number=UINT32_REVERSE(ACKN);
	Flags.Header_Length=(sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size)/4;
	P_Protocol_TCP_Packet_Heade->Flags=UINT16_REVERSE(Flags.DATA);
	P_Protocol_TCP_Packet_Heade->Window_Size=UINT16_REVERSE(Windows);
	P_Protocol_TCP_Packet_Heade->CheckSum=UINT16_REVERSE(0x0000);
	P_Protocol_TCP_Packet_Heade->Urgent_Pointer=UINT16_REVERSE(Urgent_Pointer);

	//Option
	if(Option_Size!=0)
	{
		memcpy(&Protocol_TCP_Packet[sizeof(Net_Protocol_TCP_Packet_Heade_Type)],Option,Option_Size);
	}
	//DATA
	if(DATA_Size!=0)
	{
		memcpy(&Protocol_TCP_Packet[sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size],DATA,DATA_Size);
	}

	uint16_t CheckSum=Net_Core_CheckSum(
			(uint16_t *)P_Pseudo_Heade,
			sizeof(Net_IPv4_Packet_Pseudo_Heade_Type),
			(uint16_t *)Protocol_TCP_Packet,
			sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size);
	P_Protocol_TCP_Packet_Heade->CheckSum=UINT16_REVERSE(CheckSum);

	Err=IPv4_Tx_Pend_DATA(
				P_Net_Node,
				Net_IPv4_Protocol_TCP,
				DEST_IP_Address,
				Protocol_TCP_Packet,
				sizeof(Net_Protocol_TCP_Packet_Heade_Type)+Option_Size+DATA_Size,
				true);

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
	Memory_Free(P_Pseudo_Heade);

Protocol_TCP_Tx_Exit1:
	return Err;
}

