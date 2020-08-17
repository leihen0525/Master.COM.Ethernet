/*
 * Protocol.PORT.c
 *
 *  Created on: 2020年3月16日
 *      Author: Master.HE
 */
#include "Error.h"
#include "API.h"
#include "Protocol.PORT.Define.h"
#include "Protocol.PORT.h"

int Protocol_PORT_Init(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA)
{
	if(P_Protocol_PORT_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}


	int Err;
	for(int i=0;i<Net_Protocol_PORT_Hash_Group_Number;i++)
	{
		if((Err=Protocol_PORT_List_Init(&P_Protocol_PORT_DATA->Hash_List[i]))!=Error_OK)
		{
			return Err;
		}
	}

	return Error_OK;
}

//增加一个节点进链表 进行自动排序
int Protocol_PORT_Add(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type Flag)
{
	if(P_Protocol_PORT_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	return Protocol_PORT_List_Add(&P_Protocol_PORT_DATA->Hash_List[Net_Protocol_PORT_Hash_Get_Group_Index(PORT)],Net_Protocol_PORT_Hash_Get_Sub_Potr(PORT),Flag);

}

//
int Protocol_PORT_New(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,Net_Protocol_PORT_Node_Flag_Type Flag,uint16_t Begin_PORT,uint16_t End_PORT)
{
	if(P_Protocol_PORT_DATA==Null || Begin_PORT>End_PORT)
	{
		return Error_Invalid_Parameter;
	}
	int Begin_Index=Net_Protocol_PORT_Hash_Get_Group_Index(Begin_PORT);
	int End_Index=Net_Protocol_PORT_Hash_Get_Group_Index(End_PORT);
	int Err;

	for(int Index=Begin_Index;Index<=End_Index;Index++)
	{
		int B_Port;
		int E_Port;

		if(Index==Begin_Index)
		{
			B_Port=Net_Protocol_PORT_Hash_Get_Sub_Potr(Begin_PORT);
		}
		else
		{
			B_Port=0;
		}
		if(Index==End_Index)
		{
			E_Port=Net_Protocol_PORT_Hash_Get_Sub_Potr(End_PORT);
		}
		else
		{
			E_Port=Net_Protocol_PORT_Hash_Sub_Potr_Mask;
		}


		Err=Protocol_PORT_List_New(&P_Protocol_PORT_DATA->Hash_List[Index],Flag,B_Port,E_Port);

		if(Error_OK<=Err)
		{
			return Net_Protocol_PORT_Hash_Set_Port(Index,Err);
		}
		else if(Err==Error_Dissatisfy)
		{
			;
		}
		else
		{
			return Err;
		}
	}
	return Err;
}
int Protocol_PORT_Del(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,uint16_t PORT)
{
	if(P_Protocol_PORT_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	return Protocol_PORT_List_Del(&P_Protocol_PORT_DATA->Hash_List[Net_Protocol_PORT_Hash_Get_Group_Index(PORT)],Net_Protocol_PORT_Hash_Get_Sub_Potr(PORT));

}

int Protocol_PORT_Find(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type *Flag)
{
	if(P_Protocol_PORT_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	return Protocol_PORT_List_Find(&P_Protocol_PORT_DATA->Hash_List[Net_Protocol_PORT_Hash_Get_Group_Index(PORT)],Net_Protocol_PORT_Hash_Get_Sub_Potr(PORT),Flag);

}
int Protocol_PORT_Clear(Net_Protocol_PORT_DATA_Type *P_Protocol_PORT_DATA)
{
	if(P_Protocol_PORT_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	int Err;
	for(int i=0;i<Net_Protocol_PORT_Hash_Group_Number;i++)
	{
		if((Err=Protocol_PORT_List_Clear(&P_Protocol_PORT_DATA->Hash_List[i]))!=Error_OK)
		{
			return Err;
		}
	}
	return Error_OK;
}
//----------------------------------------------------------------------------------------------------
static int Protocol_PORT_List_Init(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA)
{
	if(P_Protocol_PORT_List_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}

	P_Protocol_PORT_List_DATA->Count=0;

	P_Protocol_PORT_List_DATA->Head=Null;
	P_Protocol_PORT_List_DATA->End=Null;

	return Error_OK;

}
//增加一个节点进链表 进行自动排序
static int Protocol_PORT_List_Add(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type Flag)
{
	if(P_Protocol_PORT_List_DATA==Null || PORT>Net_Protocol_PORT_Hash_Sub_Potr_Mask)
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_PORT_Node_DATA_Type *Add_Node=Memory_Malloc(sizeof(Net_Protocol_PORT_Node_DATA_Type));

	if(Add_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}
	Add_Node->PORT=PORT;
	Add_Node->Flag=Flag;
	Add_Node->NEXT=Null;

	if(P_Protocol_PORT_List_DATA->Head==Null || P_Protocol_PORT_List_DATA->End==Null)//第一次加
	{
		P_Protocol_PORT_List_DATA->Head=Add_Node;
		P_Protocol_PORT_List_DATA->End=Add_Node;

	}
	else
	{
		if(Add_Node->PORT<P_Protocol_PORT_List_DATA->Head->PORT)//比头小 加入头之前
		{
			Add_Node->NEXT=P_Protocol_PORT_List_DATA->Head;
			P_Protocol_PORT_List_DATA->Head=Add_Node;
		}
		else if(P_Protocol_PORT_List_DATA->End->PORT<Add_Node->PORT)//比尾大 加入尾之后
		{
			P_Protocol_PORT_List_DATA->End->NEXT=Add_Node;
			P_Protocol_PORT_List_DATA->End=Add_Node;
		}
		else//在中间
		{
			Net_Protocol_PORT_Node_DATA_Type *Temp_Node=Null;

			Temp_Node=P_Protocol_PORT_List_DATA->Head;

			while(Temp_Node!=Null)
			{
				if(Temp_Node->PORT==Add_Node->PORT)
				{
					Memory_Free(Add_Node);

					return Error_Exist;
				}
				else if(Temp_Node->PORT<Add_Node->PORT)
				{
					if(Temp_Node->NEXT!=Null)
					{
						if(Add_Node->PORT<Temp_Node->NEXT->PORT)
						{

							Add_Node->NEXT=Temp_Node->NEXT;

							Temp_Node->NEXT=Add_Node;

							break;
						}
						else
						{
							Temp_Node=Temp_Node->NEXT;
						}
					}
					else//前面代码已经处理过 理论上不可能出现
					{
						P_Protocol_PORT_List_DATA->End->NEXT=Add_Node;
						P_Protocol_PORT_List_DATA->End=Add_Node;

						break;
					}
				}
				else//前面代码已经处理过 理论上不可能出现
				{
					Add_Node->NEXT=P_Protocol_PORT_List_DATA->Head;
					P_Protocol_PORT_List_DATA->Head=Add_Node;

					break;
				}
			}
		}
	}


	P_Protocol_PORT_List_DATA->Count++;

	return Error_OK;

}

//
static int Protocol_PORT_List_New(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,Net_Protocol_PORT_Node_Flag_Type Flag,uint16_t Begin_PORT,uint16_t End_PORT)
{
	if(P_Protocol_PORT_List_DATA==Null || Begin_PORT>End_PORT || Begin_PORT>Net_Protocol_PORT_Hash_Sub_Potr_Mask || End_PORT>Net_Protocol_PORT_Hash_Sub_Potr_Mask)
	{
		return Error_Invalid_Parameter;
	}

	Net_Protocol_PORT_Node_DATA_Type *Add_Node=Memory_Malloc(sizeof(Net_Protocol_PORT_Node_DATA_Type));

	if(Add_Node==Null)
	{
		return Error_Allocation_Memory_Failed;
	}

	Add_Node->PORT=Begin_PORT;
	Add_Node->Flag=Flag;
	Add_Node->NEXT=Null;

	if(P_Protocol_PORT_List_DATA->Head==Null || P_Protocol_PORT_List_DATA->End==Null)//第一次加
	{
		P_Protocol_PORT_List_DATA->Head=Add_Node;
		P_Protocol_PORT_List_DATA->End=Add_Node;
	}
	else
	{
		Net_Protocol_PORT_Node_DATA_Type *Temp_Node=Null;

		Temp_Node=P_Protocol_PORT_List_DATA->Head;

		while(Temp_Node!=Null)
		{

			if(Add_Node->PORT<=Temp_Node->PORT)
			{
				Add_Node->PORT++;
			}
			else //Temp_Node->PORT<Add_Node->PORT
			{
				if(Temp_Node->NEXT!=Null)
				{
					if(Add_Node->PORT<Temp_Node->NEXT->PORT)
					{
						Add_Node->NEXT=Temp_Node->NEXT;
						Temp_Node->NEXT=Add_Node;
						break;
					}
					else//(Temp_Node->NEXT->PORT<=Add_Node->PORT)
					{
						Add_Node->PORT++;
					}

				}
				else//Temp_Node->NEXT==Null
				{

				}
			}

			if(End_PORT<Add_Node->PORT)
			{
				Memory_Free(Add_Node);

				return Error_Dissatisfy;
			}
			if(Temp_Node->NEXT==Null)
			{
				P_Protocol_PORT_List_DATA->End->NEXT=Add_Node;
				P_Protocol_PORT_List_DATA->End=Add_Node;
			}
			Temp_Node=Temp_Node->NEXT;

		}


	}


	P_Protocol_PORT_List_DATA->Count++;
	return Add_Node->PORT;
}
static int Protocol_PORT_List_Del(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,uint16_t PORT)
{
	if(P_Protocol_PORT_List_DATA==Null || PORT>Net_Protocol_PORT_Hash_Sub_Potr_Mask)
	{
		return Error_Invalid_Parameter;
	}
	Net_Protocol_PORT_Node_DATA_Type *Temp_Node_LAST=Null,*Temp_Node;

	Temp_Node=P_Protocol_PORT_List_DATA->Head;

	while(Temp_Node!=Null)
	{
		if(Temp_Node->PORT==PORT)
		{
			if(Temp_Node_LAST==Null)
			{
				P_Protocol_PORT_List_DATA->Head=Temp_Node->NEXT;
			}
			else
			{
				Temp_Node_LAST->NEXT=Temp_Node->NEXT;
			}
			if(Temp_Node==P_Protocol_PORT_List_DATA->End || Temp_Node->NEXT==Null)
			{
				P_Protocol_PORT_List_DATA->End=Temp_Node_LAST;
			}
			Memory_Free(Temp_Node);
			P_Protocol_PORT_List_DATA->Count--;
			return Error_OK;

		}
		Temp_Node_LAST=Temp_Node;
		Temp_Node=Temp_Node->NEXT;
	}
	return Error_No_Find;

}

static int Protocol_PORT_List_Find(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA,uint16_t PORT,Net_Protocol_PORT_Node_Flag_Type *Flag)
{
	if(P_Protocol_PORT_List_DATA==Null || PORT>Net_Protocol_PORT_Hash_Sub_Potr_Mask)
	{
		return Error_Invalid_Parameter;
	}
	Net_Protocol_PORT_Node_DATA_Type *Temp_Node=P_Protocol_PORT_List_DATA->Head;

	while(Temp_Node!=Null)
	{
		if(Temp_Node->PORT==PORT)
		{
			if(Flag!=Null)
			{
				*Flag=Temp_Node->Flag;
			}
			return Error_OK;
		}

		Temp_Node=Temp_Node->NEXT;
	}

	return Error_No_Find;
}
static int Protocol_PORT_List_Clear(Net_Protocol_PORT_List_Node_Type *P_Protocol_PORT_List_DATA)
{
	if(P_Protocol_PORT_List_DATA==Null)
	{
		return Error_Invalid_Parameter;
	}
	Net_Protocol_PORT_Node_DATA_Type *Temp_Node=P_Protocol_PORT_List_DATA->Head,*Temp_Node_NEXT=Null;

	while(Temp_Node!=Null)
	{
		Temp_Node_NEXT=Temp_Node->NEXT;

		Memory_Free(Temp_Node);

		Temp_Node=Temp_Node_NEXT;
	}

	P_Protocol_PORT_List_DATA->Count=0;

	P_Protocol_PORT_List_DATA->Head=Null;
	P_Protocol_PORT_List_DATA->End=Null;

	return Error_OK;

}
