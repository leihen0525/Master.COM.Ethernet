/*
 * Net.Core.Device.h
 *
 *  Created on: 2019年7月15日
 *      Author: Master.HE
 */

#ifndef NET_CORE_DEVICE_H_
#define NET_CORE_DEVICE_H_

#include "Net.Device.Struct.h"
#include "Net.Device.Enum.h"


int Net_Core_Device_Register(
		Net_Device_Type Device_Type,
		const Net_Device_Ethernet_Config_Type *P_Ethernet_Config,
		const Net_Device_IP_Config_Type *P_IP_Config,
		const Net_Device_OPS_Type *P_OPS);


#endif /* NET_CORE_DEVICE_H_ */
