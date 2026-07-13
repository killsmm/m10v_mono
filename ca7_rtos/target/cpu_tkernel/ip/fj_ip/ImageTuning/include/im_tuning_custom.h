#ifndef IM_TUNING_CUSTOM_H_INC
#define IM_TUNING_CUSTOM_H_INC

// ****************************************************************************
//  Include
// ****************************************************************************
#include "im_tuning_define.h"
// ****************************************************************************
//  Definition
// ****************************************************************************

#define D_DBGMASK               (D_SYSTEM_MSG|D_ASCII_CMD_REPLY|D_CMD_ERROR|D_BIN_CMD_REPLY|D_CMD_WARNING)
#define D_ALWAYS				(0xFFFFFFFF)

#define D_ASCII_CMD_REPLY           (0x00000001)
#define D_CMD_ERROR                 (0x00000002)
#define D_ASCII_ECHO                (0x00000004)
#define D_BIN_CMD_REPLY             (0x00000008)
#define D_ASCII_DBG_MSG             (0x00000010)
#define D_CMD_WARNING			    (0x00000020)
#define D_SYSTEM_MSG                (0x00000040)    //All message from RTOS
//#define D_IQPRINT_MSG               (0x40000000)    //All message from ImageTune

//#define D_ENB_ALL_MSG               (0xC0000000)


#define kAutoBinAscii_Cmdtype		((sid>16)?D_ASCII_CMD_REPLY:D_BIN_CMD_REPLY)
#define kAlwaysBinAscii_Cmdtype		(D_ASCII_CMD_REPLY|D_BIN_CMD_REPLY)

// ****************************************************************************
//  Global Function Definition
// ****************************************************************************


#endif // IM_TUNING_CUSTOM_H_INC
