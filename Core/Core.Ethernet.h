/*
 * Core.Ethernet.h
 *
 *  Created on: 2019年7月17日
 *      Author: Master.HE
 */

#ifndef CORE_ETHERNET_H_
#define CORE_ETHERNET_H_

#include "Master.Stdint.h"
#include "Core.Ethernet.Enum.h"
#include "Core.Struct.h"

void Net_Core_Ethernet_Handle_Rx(Net_Core_Device_Node_Type *P_Net_Node,uint8_t *DATA);

int Net_Core_Ethernet_Tx_DATA(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Core_Ethernet_Protocol_Type Protocol,
		uint8_t *Target_MAC_Address,
		uint8_t *DATA,
		uint16_t Size);

int Net_Core_Ethernet_Tx_DATAs(
		Net_Core_Device_Node_Type *P_Net_Node,
		Net_Core_Ethernet_Protocol_Type Protocol,
		uint8_t *Target_MAC_Address,
		uint8_t *DATA0,
		uint16_t Size0,
		uint8_t *DATA1,
		uint16_t Size1);

#endif /* CORE_ETHERNET_H_ */
