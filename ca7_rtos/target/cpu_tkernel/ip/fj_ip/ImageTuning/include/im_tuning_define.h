#ifndef IM_TUNING_DEFINE_H_INC
#define IM_TUNING_DEFINE_H_INC

// ****************************************************************************
//  Include
// ****************************************************************************
//#include "common.h"
#include "ddim_typedef.h"

// ****************************************************************************
//  Definition
// ****************************************************************************

#define MAX_RESOURCE_NUM (25)

typedef struct GetResource{
    UCHAR Name[8];
    UCHAR ID;
    UINT32 Mem_ptr;
    UINT32 Maxsize;
    UINT32 Cursize;
}_GetResource;

typedef struct GetVersion{
    UCHAR    endian;
    UCHAR    chipid;
    UCHAR    revserse;
    UCHAR    protocol_ver[4];
    UCHAR    fw_ver[16];
    USHORT   uart_bufsize;
    USHORT   max_bufsize_bincmd;
    USHORT   max_bufsize_asciicmd;
    USHORT   support_mode;
}_GetVersion;

typedef struct ASCIICmdNode{
    UCHAR id;
    CHAR* name;
    CHAR* help;
}_ASCIICmdNode;

typedef struct ListAllMode{
    UCHAR mode_num;
    UCHAR stream_num;
    USHORT  _function;
    UCHAR mode_name[16];
}_ListAllMode;

typedef struct GetModeInfo{
    UCHAR cameraID;
    UCHAR inputfps;
    UCHAR outputfps;
    USHORT bayerw;
    USHORT bayerh;
    USHORT yuvw;
    USHORT yuvh;
    USHORT ycc;
}_GetModeInfo;

typedef enum {
    IQ_MODE,   // 0: cmd mode, 1:RAW mode
	E_IQCATE_Gb_Ctrl_MAX
} E_IQCATE_GCTRL;

typedef union {
	UCHAR byte[E_IQCATE_Gb_Ctrl_MAX];

	struct {
		UCHAR	IQ_MODE;
	} var;
} T_IQCATE_GCTRL;



T_IQCATE_GCTRL      IQCATE_GCTRL;

// ****************************************************************************
//  Global Function Definition
// ****************************************************************************

#endif // IM_TUNING_DEFINE_H_INC
