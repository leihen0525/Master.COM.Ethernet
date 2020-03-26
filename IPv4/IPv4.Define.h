/*
 * IPv4.Define.h
 *
 *  Created on: 2019年7月19日
 *      Author: Master.HE
 */

#ifndef IPV4_DEFINE_H_
#define IPV4_DEFINE_H_

#define Net_IPv4_Packet_Heade_TTL_Default				(64)

#define Net_IPv4_Semaphore_Tx_Ready_Max_Count			(0xFFFF)

//#define Net_IPv4_Rx_Node_Tab_Nb							(16)

#define Net_IPv4_Rx_Multi_Wait_Tims_1S					(15)

#define Net_IPv4_Pend_Max_Count							(5)
#define Net_IPv4_Pend_Max_Time							(50)

#define Net_IPv4_DATA_Rx_List_Max_Count					(20)


#define Net_IPv4_Adress_0x00							"\x00\x00\x00\x00"
#define Net_IPv4_Adress_0x7F							"\x7F\x00\x00\x01"
#define Net_IPv4_Adress_0xFF							"\xFF\xFF\xFF\xFF"

#endif /* IPV4_DEFINE_H_ */
