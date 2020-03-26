/*
 * Net.Library.h
 *
 *  Created on: 2020��1��21��
 *      Author: Master.HE
 */

#ifndef NET_LIBRARY_H_
#define NET_LIBRARY_H_

#include "Master.Stdint.h"

unsigned long Net_Inet_Addr(const char *cp);

int sgIP_DNS_isipaddress(const char * name, unsigned long * ipdest);

#endif /* NET_LIBRARY_H_ */
