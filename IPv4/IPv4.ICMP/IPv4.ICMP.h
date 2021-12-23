/*
 * IPv4.ICMP.h
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */

#ifndef IPV4_ICMP_H_
#define IPV4_ICMP_H_

#include "Master.Stdint.h"
#include "Core/Core.Struct.h"

void IPv4_ICMP_Handle_Rx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *SRC_IPv4_Address,
		uint8_t *IPv4_ICMP_Packet,
		uint16_t IPv4_ICMP_Packet_Size);

int IPv4_ICMP_Tx(
		Net_Core_Device_Node_Type *P_Net_Node,
		uint8_t *DEST_IPv4_Address,
		uint8_t Type,
		uint8_t Code,
		uint8_t *DATA,
		uint16_t Size);

#endif /* IPV4_ICMP_H_ */
