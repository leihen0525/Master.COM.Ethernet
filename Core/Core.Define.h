/*
 * Core.Define.h
 *
 *  Created on: 2019Äê7ÔÂ17ÈÕ
 *      Author: Master.HE
 */

#ifndef CORE_DEFINE_H_
#define CORE_DEFINE_H_


//------------------------------------------------------------

#define BIG_ENDIAN 1

#if BIG_ENDIAN

#define UINT32_REVERSE(X)  (((((uint32_t)(X) & 0xff000000) >> 24) | (((uint32_t)(X) & 0x00ff0000) >> 8) | \
		  (((uint32_t)(X) & 0x0000ff00) << 8) | (((uint32_t)(X) & 0x000000ff) << 24)))

#define UINT16_REVERSE(X)  (((((uint16_t)(X) & 0xff00) >> 8) | (((uint16_t)(X) & 0x00ff) << 8)))


#else

#define UINT32_REVERSE(X)  (X)
#define UINT16_REVERSE(X)  (X)

#endif

//------------------------------------------------------------

#define Net_Core_MAC_Adress_0x00							"\x00\x00\x00\x00\x00\x00"
#define Net_Core_MAC_Adress_0xFF							"\xFF\xFF\xFF\xFF\xFF\xFF"


#endif /* CORE_DEFINE_H_ */
