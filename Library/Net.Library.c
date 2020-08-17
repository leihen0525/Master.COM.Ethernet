/*
 * Net.Library.c
 *
 *  Created on: 2020年1月21日
 *      Author: Master.HE
 */
#include "Core/Core.Define.h"

#include "Net.Library.h"


unsigned long Net_Inet_Addr(const char *cp)
{
	unsigned long IP;
	if(sgIP_DNS_isipaddress(cp,&IP)) {
		return IP;
	}
	return 0xFFFFFFFF;
}

//下面这个函数是我抄来的和printf函数处理手法一致
//出处 rtems-4.11.3\c\src\lib\libbsp\arm\nds\dswifi\arm9\source\sgIP_DNS.c (4 hits)
int sgIP_DNS_isipaddress(const char * name, unsigned long * ipdest)
{
	int i,j,t,ndot;
	unsigned long out_addr, g[4];
	const char * c;

	ndot=0;
	c=name;
	while(*c) { // scan for invalid characters
		if(!((*c>='0' && *c<='9') || (*c>='A' && *c<='F') || (*c>='a' && *c<='f') || *c=='.' || *c=='x' || *c=='X')) return 0;
		if(*c=='.') ndot++;
		c++;
	}
	if(ndot>3) return 0;
	c=name;
	for(i=0;i<=ndot;i++) {
		g[i]=0;
		t=0;
		j=0;
		while(*c && *c!='.') {
			if(j==0 && *c=='0') { t++; }
			else if(j==1 && t==1 && (*c=='x' || *c=='X')) { t++; }
			else {
				switch(t) {
				case 0: // decimal
					if(*c=='x' || *c=='X' || (*c>='A' && *c<='F') || (*c>='a' && *c<='f')) return 0;
					g[i]=(g[i]*10)+(*c-'0');
					break;
				case 1: // octal
					if(*c=='x' || *c=='X' || (*c>='A' && *c<='F') || (*c>='a' && *c<='f') || *c=='8' || *c=='9') return 0;
					g[i]=(g[i]<<3)+(*c-'0');
					break;
				case 2: // hex
					if(*c=='x' || *c=='X') return 0;
					if(*c>='0' && *c<='9') {
						g[i]=(g[i]<<4)+(*c-'0');
					} else {
						g[i]=(g[i]<<4)+(*c&0xDF)+9;
					}
					break;
				}
			}
			j++; c++;
		}
		if(*c) c++; else break;
	}
	out_addr=0;
	switch(ndot) {
	case 0:
		out_addr=g[0];
		break;
	case 1:
		if(g[0]>=0x100 || g[1]>=0x1000000) return 0;
		out_addr= (g[0]<<24) | g[1];
		break;
	case 2:
		if(g[0]>=0x100 || g[1]>=0x100 || g[2]>=0x10000) return 0;
		out_addr= (g[0]<<24) | (g[1]<<16) | g[2];
		break;
	case 3:
		if(g[0]>=0x100 || g[1]>=0x100 || g[2]>=0x100 || g[3]>=0x100) return 0;
		out_addr= (g[0]<<24) | (g[1]<<16) | (g[2]<<8) | g[3];
		break;
	}
	*ipdest=UINT32_REVERSE(out_addr);
	return 1;
}
