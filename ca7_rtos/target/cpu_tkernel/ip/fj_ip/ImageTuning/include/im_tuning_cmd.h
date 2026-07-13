#pragma once

#ifndef IM_TUNING_CMD_H_INC
#define IM_TUNING_CMD_H_INC

// ****************************************************************************
//  Include
// ****************************************************************************
#include "ddim_typedef.h"

// ****************************************************************************
//  Definition
// ****************************************************************************
/*
PC to ISP:  Bin Command
b6:4
0(0x80)	Start Packet
1(0x90)	Core CommandTYPE
2(0xA0)	Data Size(in Hex
3(0xB0)	DataBody
4(0xC0)	(reserved)
5(0xD0)	(reserved)
6(0xE0) BinayData Control (reserve)
7(0xF0)	EndPacket
*/
#define  kBinCmd_Start	(0x80)
#define  kBinCmd_CmdID	(0x90)
#define	 kBinCmd_Size	(0xA0)
#define  kBinCmd_Data	(0xB0)
#define  kBinCmd_DataDual	(0xB0B0)
#define  kBinCmd_End	(0xF0)


/*
ISP to PC:  Reply Bin Command
b6:4
0(0x80)	OK Packet
1(0x90)	NG Packet
2(0xA0)	DataSize
3(0xB0)	DataBody
4(0xC0)	Interrupt
5(0xD0)	Periodic Response
6(0xE0) (reserved)BinayData Control
7(0xF0)	EndPacket
*/

#define  kBinRpy_OK	(0x80)
#define  kBinRpy_NG	(0x90)
#define	 kBinRpy_Size	(0xA0)
#define  kBinRpy_Data	(0xB0)
#define  kBinRpy_Intr	(0xC0)
#define  kBinRpy_Period	(0xD0)
#define  kBinRpy_End	(0xF0)


#define	kBinCmd_Err_Head	        1//(-1000)
#define	kBinCmd_Err_Cmd		        2//(-1001)
#define	kBinCmd_Err_NO_Tail	        3//(-1002)
#define	kBinCmd_Err_DataBufOver	    4//(-1003)
#define	kBinCmd_Err_SizeNotMatch	5//(-1004)
#define kBinCmd_Err_CmdTooShort	    6//(-1005)
#define kBinCmd_Err_CheckSum	    7//(-1006)

// ****************************************************************************
//  Global Function Definition
// ****************************************************************************

void VISP_debugprintf(char *lpBuf, long dwBufLen);
int asciicmd_parse(unsigned char *in_args, long in_len, unsigned char **out_argv, unsigned long *argc);
int bincmd_parse(unsigned char *in_str, unsigned long in_len, unsigned char *out_cmd, unsigned char *out_data, unsigned long *out_datalen, unsigned char *out_sid, unsigned long max_outdata);

#endif // IM_TUNING_CMD_H_INC
