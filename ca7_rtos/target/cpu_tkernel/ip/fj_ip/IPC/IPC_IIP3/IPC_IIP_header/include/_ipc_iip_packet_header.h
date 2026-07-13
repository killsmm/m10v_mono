/**************************************************************************/
/*  <Contents>   Definition IIP Unit packet header                        */
/*  <Filename>   _ipc_iip_packet_header.h                                 */
/*  <Environment> RVDS V4.0 Pro for ARM                                   */
/*  <Creation Date/author>                                                */
/*               2014/01/20   H.Hata                                 1.10 */
/*  <Latest modify Date/author>                                           */
/*                                                                        */
/*                                      Socionext Middleware Library      */
/*                                      COPYRIGHT(C) Socionext Inc. 2015  */
/**************************************************************************/

#ifndef IPC_IIP_PACEKT_HEADER_H_INC
#define IPC_IIP_PACEKT_HEADER_H_INC

#include "ipc_iip_packet_option.h"

/* structure of packet header (4Byte) */
union ipc_iip_packet_header{
	unsigned long			word;
	struct{
		unsigned long		BeginningAddress	:16;
		unsigned long		WordLength			:8;
		unsigned long		ControlCode			:8;
	} bit;
};

#define D_IPC_IIP_PARAM_CTL_CODE_LOAD_ONLY			(0x11)
#define D_IPC_IIP_PARAM_CTL_CODE_EXE_LAST_PACKET	(0x01)
#define D_IPC_IIP_PARAM_CTL_CODE_EXE				(0x02)
#define D_IPC_IIP_PARAM_CTL_CODE_EXE_NEXT_CHAIN		(0x03)

#endif // IPC_IIP_PACEKT_HEADER_H_INC 
