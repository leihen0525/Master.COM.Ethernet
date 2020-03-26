/*
 * Net.Socket.h
 *
 *  Created on: 2019年7月15日
 *      Author: Master.HE
 */

#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include "Net.Socket.Enum.h"
#include "Net.Socket.Define.h"
#include "Net.Socket.Struct.h"


// sys/socket.h

int Net_Socket(int Domain, int Type, int Protocol);

int Net_Bind(int Socket, const Net_Socket_Address_DATA_Type * Addr);
/*
int Net_Connect(int Socket, const Net_Socket_Address_DATA_Type * Addr);

int Net_Send(int Socket, const void * Data, int SendLength, int Flags);

int Net_Recv(int Socket, void * Data, int RecvLength, int Flags);
*/
int Net_SendTo(int Socket, const void * Data, int Sendlength, int Flags, const Net_Socket_Address_DATA_Type * Addr);

int Net_RecvFrom(int Socket, void * Data, int RecvLength, int Flags, Net_Socket_Address_DATA_Type * Addr);
/*
int Net_Listen(int Socket, int Max_Connections);

int Net_Accept(int Socket, Net_Socket_Address_DATA_Type * Addr);

int Net_ShutDown(int Socket, int ShutDown_Type);
*/
int Net_CloseSocket(int Socket);
/*
int Net_Ioctl(int Socket, long Cmd, void * Arg);
*/
int Net_SetSockopt(int Socket, int Level, int Option_Name, const void * Data, int Data_Len);

int Net_GetSockopt(int Socket, int Level, int Option_Name, void * Data, int * Data_Len);
/*
int Net_GetPeername(int Socket, Net_Socket_Address_DATA_Type *Addr);

int Net_GetSockname(int Socket, Net_Socket_Address_DATA_Type *Addr);

// sys/time.h (actually intersects partly with libnds, so I'm letting libnds handle fd_set for the time being)
//int Net_Select(int Nfds, fd_set *ReadFds, fd_set *WriteFds, fd_set *ErrorFds, struct timeval *TimeOut);

// arpa/inet.h
unsigned long Net_Inet_Addr(const char *Cp);
*/




//
unsigned long Net_Inet_Addr(const char *cp);








#endif /* NET_SOCKET_H_ */
