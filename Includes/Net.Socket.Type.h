/*
 * Net.Socket.Type.h
 *
 *  Created on: 2019年12月19日
 *      Author: Master.HE
 */

#ifndef NET_SOCKET_TYPE_H_
#define NET_SOCKET_TYPE_H_

#include <stdint.h>

typedef uint8_t		__sa_family_t;
typedef uint32_t	__socklen_t;

/*
 * Data types.
 */

#ifndef _SA_FAMILY_T_DECLARED
typedef	__sa_family_t	sa_family_t;
#define	_SA_FAMILY_T_DECLARED
#endif

#ifndef _SOCKLEN_T_DECLARED
typedef	__socklen_t	socklen_t;
#define	_SOCKLEN_T_DECLARED
#endif


#endif /* NET_SOCKET_TYPE_H_ */
