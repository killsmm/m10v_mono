/**
 * @file		user_IQTune.c
 * @brief		user IQ tool cmd API
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifdef CO_IM_TUNING_ON
// ****************************************************************************
//  Include
// ****************************************************************************
#include "user.h"
#include "../include/im_tuning_cmd.h"
#include "../include/im_tuning_custom.h"
#include "../include/im_tuning_task.h"
#include "../include/im_tuning_define.h"
#include "../include/im_tuning_buffer.h"
#include "iq_bin_dbg2.h"
#include "category_parameter.h"
#include "dd_uart.h"
#include "uart.h"
#include "iqpara_api.h"
#include "bf_startup.h"
#include "bf_compile_info_update.h"
#include "bf_compile_info_build.h"
#include "l1l2cache.h"
#include "camera_config.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>




// ****************************************************************************
//  Define
// ****************************************************************************

//OR  ORDER_LITTLE_ENDIAN ORDER_BIG_ENDIAN
#define ORDER_BIG_ENDIAN        1
#define ORDER_LITTLE_ENDIAN     0

#define BYTE_ORDER ORDER_LITTLE_ENDIAN

#define TO_BIG_ENDIAN_UINT16(p)   (uint16_t) (      ( *( (UCHAR*)(p) + 1)  << 8 ) |        \
                                                    ( *( (UCHAR*)(p)    )       )   )
#define TO_BIG_ENDIAN_UINT32(p)   (uint32_t) (      ( *( (UCHAR*)(p) + 3) << 24)    | \
                                                    ( *( (UCHAR*)(p) + 2) << 16)    | \
                                                    ( *( (UCHAR*)(p) + 1) << 8 )    | \
                                                    ( *( (UCHAR*)(p)    )      ))

#define TO_LITTLE_ENDIAN_UINT16(p)   (uint16_t) (       ( *( (UCHAR*)(p)    )  << 8) |  \
                                                        ( *( (UCHAR*)(p) + 1)      ) )
#define TO_LITTLE_ENDIAN_UINT32(p)   (uint32_t) (       ( *( (UCHAR*)(p)    ) << 24 ) | \
                                                        ( *( (UCHAR*)(p) + 1) << 16 ) | \
                                                        ( *( (UCHAR*)(p) + 2) << 8 ) |  \
                                                        ( *( (UCHAR*)(p) + 3)  ) )

#define BYTE_TOUINT16(p)   (uint16_t) (         ( *( (UCHAR*)(p)    )  << 8) |  \
                                                ( *( (UCHAR*)(p) + 1)      ) )
#define BYTE_TOUINT32(p)   (uint32_t) (         ( *( (UCHAR*)(p)    ) << 24 ) | \
                                                ( *( (UCHAR*)(p) + 1) << 16 ) | \
                                                ( *( (UCHAR*)(p) + 2) << 8 ) |  \
                                                ( *( (UCHAR*)(p) + 3)  ) )
#define  _16bit_TO_BIG_ENDIAN(v,p)	{	\
											*((char*)(p)+0) = (uint8_t)(((v) >>8) & 0xFF); \
											*((char*)(p)+1) = (uint8_t)(((v) >>0) & 0xFF); \
										}
#define  _32bit_TO_BIG_ENDIAN(v,p)   {	*((char*)(p)+0) = (uint8_t)(((v) >>24) & 0xFF); \
											*((char*)(p)+1) = (uint8_t)(((v) >>16) & 0xFF); \
											*((char*)(p)+2) = (uint8_t)(((v) >>8) & 0xFF); \
											*((char*)(p)+3) = (uint8_t)(((v) >>0) & 0xFF); \
										}

#define MaxBinCmdSize 128

#define kCmdMaxSize	(256)
#define kRawTextSize (1000)
#define kShowStringSize	 (kCmdMaxSize+64)
#define kMaxARGC	(10)
#define kBinDataSize	(256)
#define kMaxBinReplySize    (256)
#define rPCtoISP    (1)
#define rISPtoPC    (2)



enum{
        E_Cmd_GetSystemVersion = 0x0,         //0x00
        E_Cmd_GetAllModeInfo,                 //0x01
        E_Cmd_GetModeInfo,                    //0x02
        E_Cmd_GetCurrentMode,                 //0x03
        E_Cmd_SetMode,                        //0x04
        E_Cmd_Exit,                           //0x05
        E_Cmd_CateCmd,                        //0x06
        E_Cmd_Help,                           //0x07
        E_Cmd_SetCateByte=0x8,                //0x08
        E_Cmd_GetCateByte,                    //0x09
        E_Cmd_SetDbgMask,                     //0x0A
        E_Cmd_GetDbgMask,                     //0x0B
        E_Cmd_Set_32b_Memory=0x10,            //0x10
        E_Cmd_Set_16b_Memory,                 //0x11
        E_Cmd_Set_8b_Memory,                  //0x12
        E_Cmd_Get_32b_Memory,                 //0x13
        E_Cmd_Get_16b_Memory,                 //0x14
        E_Cmd_Get_8b_Memory,                  //0x15
        E_Cmd_Set_32b_MemoryMask,             //0x16
        E_Cmd_Set_16b_MemoryMask,             //0x17
        E_Cmd_Set_8b_MemoryMask,              //0x18
        E_Cmd_Set_Burst_Memory,               //0x19
        E_Cmd_Get_Burst_Memory,               //0x1A
        E_Cmd_AE_control=0x20,                //0x20
        E_Cmd_AWB_control,                    //0x21
        E_Cmd_AF_control,                     //0x22
        E_Cmd_SHD_control,                    //0x23
        E_Cmd_IQ_control,                     //0x24
        E_Cmd_SmartAE_Control,                //0x25
        E_Cmd_AFN_onoff,                      //0x26
        E_Cmd_SPR_onoff,                      //0x27
        E_Cmd_AE_adv_control=0x30,            //0x30
        E_Cmd_EVP_adv_control,                //0x31
        E_Cmd_AWB_adv_control,                //0x32
        E_Cmd_IQ_adv_control,                 //0x33
        E_Cmd_SHD_adv_control,                //0x34
        E_Cmd_Get_General_Info=0x40,          //0x40
        E_Cmd_Get_IQ_info,                    //0x41
        E_Cmd_Get_AE_info,                    //0x42
        E_Cmd_Get_AWB_info,                   //0x43
        E_Cmd_Get_SHD_info,                   //0x44
        E_Cmd_Get_ae_static_data,             //0x45
        E_Cmd_Get_AWB_static_data,            //0x46
        E_Cmd_Get_AF_static_data,             //0x47
        E_Cmd_Auto_periodic_Info=0x4F,        //0x4F
        E_Cmd_Lock_Secssion,                  //0x50
        E_Cmd_Unlock_Secion,                  //0x51
        E_Cmd_GetResourceList_info,           //0x52
        E_Cmd_GetResource_from_ISP,           //0x53
        E_Cmd_SendResource_to_ISP,            //0x54
        E_Cmd_GetResource_from_ISP_burst,     //0x55
        E_Cmd_SendResource_to_ISP_burst,      //0x56
        E_Cmd_Get_memory_from_ISP,            //0x57
        E_Cmd_Send_memory_to_ISP,             //0x58
        E_Cmd_Get_file_from_ISP,              //0x59
        E_Cmd_Send_file_to_ISP,               //0x5A
        E_Cmd_Save_Reg_Data = 0x60,           //0x60
        E_Cmd_Set_OSD_Control=0xD0,           //0xD0
        E_Cmd_Set_Customer_string,            //0xD1
        E_Cmd_Set_OSD_status,                 //0xD2
        E_Cmd_Set_OSD_status_extend,          //0xD3
        E_Cmd_Set_OSD_position,               //0xD4
        E_Cmd_Customer_API_1=0xE0,            //0xE0

};

_ASCIICmdNode ascii_node[] = {
                                                    {0x00,"getver",       "no arg"},                        //0x00
                                                    {0x01,"getmodes",     "no arg"},                        //0x01
                                                    {0x02,"getmodeinfo",  "<mode #>"},                      //0x02
                                                    {0x03,"getmode",      ""},                              //0x03
                                                    {0x04,"setmode",      ""},                              //0x04
                                                    {0x05,"exit",         ""},                              //0x05
                                                    {0x06,"rtos",         ""},                              //0x06
                                                    {0x07,"help",         "no arg"},                        //0x07
                                                    {0x08,"wcat",         "<cate> <byte> <size of value> <value> <value> ...."},         //0x08
                                                    {0x09,"rcat",         "<cate> <byte> <size> "},         //0x09
                                                    {0x0A,"setdbm",       "<MaskVal>"},                     //0x0A
                                                    {0x0B,"getdbm",       "no arg"},                        //0x0B
                                                    {0x10,"w32",          "<addr> <value>"},                //0x10
                                                    {0x11,"w16",          "<addr> <value>"},                //0x11
                                                    {0x12,"w8",           "<addr> <value>"},                //0x12
                                                    {0x13,"r32",          "<addr> <value>"},                //0x13
                                                    {0x14,"r16",          "<addr> <value>"},                //0x14
                                                    {0x15,"r8",           "<addr> <value>"},                //0x15
                                                    {0x16,"w32m",         "<addr> <value> <mask>"},         //0x16
                                                    {0x17,"w16m",         "<addr> <value> <mask>"},         //0x17
                                                    {0x18,"w8m",          "<addr> <value> <mask>"},         //0x18
                                                    {0x20,"ae",           "<ctrl>"},                        //0x20
                                                    {0x21,"awb",          "<ctrl>"},                        //0x21
                                                    {0x22,"af",           "<ctrl>"},                        //0x22
                                                    {0x23,"shd",          "<ctrl>"},                        //0x23
                                                    {0x24,"iq",           "<ctrl>"},                        //0x24
                                                    {0x25,"smartae",      "<ctrl>"},                        //0x25
                                                    {0x26,"afn",          "<ctrl>"},                        //0x26
                                                    {0x27,"spr",          "<ctrl>"},                        //0x27
                                                    {0x30,"aeset",        "<subcmd> <arg>"},                //0x30
                                                    {0x31,"evpset",       "<subcmd> <arg>"},                //0x31
                                                    {0x32,"awbset",       "<subcmd> <arg>"},                //0x32
                                                    {0x33,"iqset",        "<subcmd> <arg...>"},             //0x33
                                                    {0x34,"shdset",       "<subcmd> <arg>"},                //0x34
                                                    {0x40,"info",         "no arg"},                        //0x40
                                                    {0x41,"iqinfo",       "no arg"},                        //0x41
                                                    {0x42,"aeinfo",       "no arg"},                        //0x42
                                                    {0x43,"awbinfo",      "no arg"},                        //0x43
                                                    {0x44,"shdinfo",      "no arg"},                        //0x44
                                                    {0x47,"afstat",       "no arg"},                        //0x47
                                                    {0x4F,"autoinfo",     "timer # period(in sec)"},        //0x4F
                                                    {0x50,"lock",         "<id>"},                          //0x50
                                                    {0x51,"unlock",       "<id>"},                          //0x51
                                                    {0x52,"reslist",      "no arg"},                        //0x52
                                                    {0x57,"getmem",       "<addr> <size>"},                 //0x57
                                                    {0x60,"savereg",      "no arg"},                        //0x60
                                                    {0xD0,"osd",          "on/off/lock"},                   //0xD0
                                                    {0xD1,"osdstring",    "#id string"},                    //0xD1
                                                    {0xD2,"osdinfo",      "frame content"},                 //0xD2
                                                    {0xD3,"osdinfoex",    "frame content"},                 //0xD3
                                                    {0xD4,"osdstyle",     "osdstyle #font #pos"},           //0xD4
                                                    {0xE0,"cuapi0",       "no arg"},                        //0xE0
													{0xFF,"NoCmd",       "no arg"},                         //0xFF
                                                    };

// ****************************************************************************
//  Global Data
// ****************************************************************************
UCHAR	g_free_state;

static UCHAR gBufCommand[kCmdMaxSize];
static UCHAR gBufCommand_Copy[kCmdMaxSize];
static UCHAR gGetRawText[kRawTextSize];
static UCHAR *gArgv[kMaxARGC];
static ULONG gArgC = 0;
static UCHAR gBinCmd;
static UCHAR gBinData[kBinDataSize];
static ULONG gBinDataSize;
static UCHAR gBinSID;

_GetVersion GetVersion;

//extern T_IQ_BIN	IQ_BIN;
static UCHAR gPeriodicID = 0;
static UCHAR gSessionID = 0;
static UCHAR gSessionID_burst =0;
static UCHAR iBurstDirection = 0;
static UCHAR* Burst_Buf_pt = NULL;
static UCHAR* Burst_Cur_pt = NULL;
_GetResource Cmd55_GRes;
_GetResource Cmd56_GRes;

static UINT32 Burst_Buffer_Size =0;
static ULONG Burst_Rcv_cnt = 0;
static UCHAR gResource_Num = 0;
_GetResource* gGetRes;

// ****************************************************************************
//  Local Function
// ****************************************************************************

/*
static VOID Set_RTOS_UART_Enable()
{
    OS_User_Sta_Tsk(TID_UART_CONTROLLER, 0);
}

static VOID Set_RTOS_UART_Disable()
{
    OS_User_Ter_Tsk(TID_UART_CONTROLLER, 0);
}
*/
static VOID Set_Total_Resource_Num(UCHAR val)
{
    gResource_Num =val;
}

static UCHAR Get_Total_Resource_Num()
{
    return gResource_Num;
}
static VOID Init_GetVersion()
{
    GetVersion.chipid = 10;// M10V
    GetVersion.endian = 0; // BIG Endian
    GetVersion.protocol_ver[3] = 0;
    GetVersion.protocol_ver[2] = 0;
    GetVersion.protocol_ver[1] = 9;
    GetVersion.protocol_ver[0] = 2;

    GetVersion.fw_ver[15] = (UCHAR)((BF_Get_OK_MP_Version() & 0xFF00) >> 8);
    GetVersion.fw_ver[14] = (UCHAR)(BF_Get_OK_MP_Version() & 0xFF) ;
    GetVersion.fw_ver[13] = BF_Get_OK_Sub_Version();
    GetVersion.fw_ver[12] = BF_Get_OK_Debug_Version();

    GetVersion.uart_bufsize = kRxBuf_Size;
    GetVersion.max_bufsize_asciicmd = kCmdMaxSize;
    GetVersion.max_bufsize_bincmd = kCmdMaxSize;
    GetVersion.support_mode = 0;
}

static VOID Init_GetResource()
{
    gGetRes = MIL_Get_Resource_List();
}

static VOID update_GetResource()
{
    UCHAR i=0;
    UCHAR total_cnt = 0;

    for (i=0;i<9;i++)   // update IQ_BIN
    {
        (gGetRes+i)->Mem_ptr = (UINT32)User_Get_Resource()->IQ_BIN_addr[i];
        (gGetRes+i)->Cursize = User_Get_Resource()->IQ_BIN_size[i];
        (gGetRes+i)->Maxsize = User_Get_Resource()->IQ_BIN_size[i];
    }

    //EVP_MAP
    (gGetRes+i)->Mem_ptr = (UINT32)User_Get_Resource()->EVP_MAP_addr;
    (gGetRes+i)->Cursize = User_Get_Resource()->EVP_MAP_size;
    (gGetRes+i)->Maxsize = User_Get_Resource()->EVP_MAP_size;

    //FSHD
    for (i=10;i<19;i++)   // update FSHD_BIN
    {
        (gGetRes+i)->Mem_ptr = (UINT32)User_Get_Resource()->FSHD_BIN_addr[i-10];
        (gGetRes+i)->Cursize = User_Get_Resource()->FSHD_BIN_size[i-10];
        (gGetRes+i)->Maxsize = User_Get_Resource()->FSHD_BIN_size[i-10];
    }

    //RGB
    (gGetRes+i)->Mem_ptr = (UINT32)User_Get_Resource()->RGB_BIN_addr;
    (gGetRes+i)->Cursize = User_Get_Resource()->RGB_BIN_size;
    (gGetRes+i)->Maxsize = User_Get_Resource()->RGB_BIN_size;

    //LDC
    i++;
    (gGetRes+i)->Mem_ptr = (UINT32)User_Get_Resource()->LDC_BIN_addr[Cate_Get_IQ_LDC_TBL_IDX()];
    (gGetRes+i)->Cursize = (UINT32)User_Get_Resource()->LDC_BIN_size[Cate_Get_IQ_LDC_TBL_IDX()];
    (gGetRes+i)->Maxsize = (UINT32)User_Get_Resource()->LDC_BIN_size[Cate_Get_IQ_LDC_TBL_IDX()];

    while(i<MAX_RESOURCE_NUM)
    {
        if ( (gGetRes+i)->ID != 0xFF)
            total_cnt++;
        i++;
    }
    Set_Total_Resource_Num(total_cnt);
}

static VOID UART_CMD_INIT()
{
    Init_GetVersion();
    Init_GetResource();
    update_GetResource();

}


static UCHAR Get_PeriodicID()
{
    return gPeriodicID;
}

static VOID Set_PeriodicID(UCHAR val)
{
    gPeriodicID = val;
}

static VOID Next_PeriodicID()
{
    Set_PeriodicID(gPeriodicID++);
    if (Get_PeriodicID() > 15)
        Set_PeriodicID(0);
}

static VOID BinCmd_Pack_Periodic_ACK(UCHAR* in_buf, UCHAR* out_buf, ULONG* totalcmdlen , ULONG datalen)
{
    UCHAR* ptr = NULL;
    ULONG cmdsize = 0;
    UCHAR checksum = 0;
    UCHAR sum =0;
    ULONG i = 0;

    ptr = &out_buf[0];
    //pid
    *ptr = 0xD0 +  Get_PeriodicID();
    Next_PeriodicID();

    ptr++;
    cmdsize++;

    //datasize
    if (datalen > 255)  //should not possible... however
    {
        *ptr++ = 0xA0 + ((datalen & 0xF00) >> 8);
        *ptr++ = 0xA0 + ((datalen & 0x0F0) >> 4);
        *ptr++ = 0xA0 + (datalen & 0x0F);
        cmdsize += 3;
    }
    else
    {
        *ptr++ = 0xA0 + ((datalen & 0x0F0) >> 4);
        *ptr++ = 0xA0 + (datalen & 0x0F);
        cmdsize += 2;
    }

    //data
    for (i = 0; i < (datalen); i++)
    {
        *ptr++ = 0xB0 + ((in_buf[i] & 0xF0) >> 4);
        *ptr++ = 0xB0 + (in_buf[i] & 0x0F);
        cmdsize += 2;
    }

    //checksum
    for (i = 0; i <  cmdsize ; i++ )
    {
        sum += out_buf[i];
    }
    sum = ((sum &0xF0) >> 4) + (sum & 0xF);
    checksum = sum & 0xF;
    *ptr = 0xF0 + checksum;
    *totalcmdlen = cmdsize + 1 ; // 1 for checksum
}

static VOID BinCmd_Pack_ACK_ONLY(UCHAR* out_buf, ULONG* totalcmdlen , UCHAR sid)
{
    //UCHAR  out_data[kCmdMaxSize] = {0};
    UCHAR* ptr = NULL;
    ULONG cmdsize = 0;
    UCHAR checksum = 0;
    UCHAR sum =0;
    ULONG i = 0;

    ptr = &out_buf[0];
    //sid
    *ptr = 0x80 + sid;
    ptr++;
    cmdsize++;

    //checksum
    for (i = 0; i <  cmdsize ; i++ )
    {
        sum += out_buf[i];
    }
    sum = ((sum &0xF0) >> 4) + (sum & 0xF);
    checksum = sum & 0xF;
    *ptr = 0xF0 + checksum;
    *totalcmdlen = cmdsize + 1 ; // 1 for checksum
}

static VOID BinCmd_Pack(UCHAR* in_buf, UCHAR* out_buf, ULONG* totalcmdlen , UCHAR sid, ULONG datalen)
{
    UCHAR* ptr = NULL;
    ULONG cmdsize = 0;
    UCHAR checksum = 0;
    UCHAR sum =0;
    ULONG i = 0;

    ptr = &out_buf[0];
    //sid
    *ptr = 0x80 + sid;
    ptr++;
    cmdsize++;

    //datasize
    if (datalen > 255)
    {
        *ptr++ = 0xA0 + ((datalen & 0xF00) >> 8);
        *ptr++ = 0xA0 + ((datalen & 0x0F0) >> 4);
        *ptr++ = 0xA0 + (datalen & 0x0F);
        cmdsize += 3;
    }
    else
    {
        *ptr++ = 0xA0 + ((datalen & 0x0F0) >> 4);
        *ptr++ = 0xA0 + (datalen & 0x0F);
        cmdsize += 2;
    }
    //IQ_DBG_PRINTLN("Datalen %d cmdlen %d",datalen,*totalcmdlen);

    //data
    for (i = 0; i < (datalen); i++)
    {
        *ptr++ = 0xB0 + ((in_buf[i] & 0xF0) >> 4);
        *ptr++ = 0xB0 + (in_buf[i] & 0x0F);
        cmdsize += 2;
    }

    //checksum
    for (i = 0; i <  cmdsize ; i++ )
    {
        sum += out_buf[i];
    }
    sum = ((sum &0xF0) >> 4) + (sum & 0xF);
    checksum = sum & 0xF;
    *ptr = 0xF0 + checksum;
    *totalcmdlen = cmdsize + 1 ; // 1 for checksum

}


static VOID Cmd_SetMode(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Help(UCHAR sid)
{
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;
    UINT32 i = 0;
    UINT32 Total_AsciiCmd_Num = (sizeof(ascii_node) / sizeof(_ASCIICmdNode));

    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"--- Help Cmd ---\n");
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"--- Cmd_Name        Arg ---\n");
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"---------------------------\n");
    for(i = 0; i<Total_AsciiCmd_Num ; i++)
    {
        IQ_PRINT(kAlwaysBinAscii_Cmdtype,"%s      %s\n",ascii_node[i].name,ascii_node[i].help);
    }

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Exit(UCHAR sid)
{
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(D_BIN_CMD_REPLY,"Exit IQTool mode\n");

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
#ifndef CO_DDIM_IM_TUNING_ON
    #error "Image tuning can work only with both CO_DDIM_IQTOOL_ON and CO_IQTOOL_ON defined"
#else
    Dd_IQTOOL_DISABLE();
    BF_UART_IQ_Command_Disable();
#endif
    BF_UART_Debug_Command_Enable();

}


static VOID Cmd_GetSystemVersion(UCHAR sid)
{
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"--- GetSystemVersion ---\n");
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"Endian: %d\n",GetVersion.endian);
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"ChipID: %d\n",GetVersion.chipid);
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"Protocol Ver: %d %d %d %d\n",GetVersion.protocol_ver[3],GetVersion.protocol_ver[2],GetVersion.protocol_ver[1],GetVersion.protocol_ver[0]);
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"max_bufsize_bincmd %d\n",GetVersion.max_bufsize_bincmd);
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"max_bufsize_asciicmd %d\n",GetVersion.max_bufsize_asciicmd);
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"Support mode: %d\n",GetVersion.support_mode);

    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"FW Version: %02x%02x:%02x:%02x \n",GetVersion.fw_ver[15],GetVersion.fw_ver[14],GetVersion.fw_ver[13],GetVersion.fw_ver[12]);
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"Sensor: %s\n",Camera_Get_SensorType());
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"FW Type: %s\n",BF_Get_OK_FW_type());
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"Build Date: %s\n",BF_CompileInfoBuild_Get_DataTimeStr());
    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"Update Date: %s\n",BF_CompileInfoUpdate_Get_DataTimeStr());


    IQ_PRINT(kAlwaysBinAscii_Cmdtype,"--- GetSystemVersion End ---\n");

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack(&GetVersion.endian,&out_buf[0],&out_cmdlen,sid,sizeof(GetVersion));
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }

}

static VOID Cmd_GetAllModeInfo(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_GetModeInfo(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_GetCurrentMode(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}


static VOID Cmd_SetCateByte(UCHAR sid, UCHAR* data_in, LONG datasize)    // data[0]: Cate, data[1]: byte, data[2]...[..]:val,
{

    UCHAR Cate = 0;
    UCHAR Addr = 0;
    UCHAR total_size = 0;
    UCHAR i = 0 ;
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;
    ULONG Cate_Data[10] = {0}; // M10V Cate_set need ULONG, why?

    if ( datasize < 4 )
    {
        IQ_PRINT(D_CMD_ERROR,"Cmd Data is %d and error\n",datasize);
        return;
    }
    else
    {
        Cate = data_in[0];
        Addr = data_in[1];
        total_size = data_in[2];
        if (total_size == 0)
        {
            IQ_PRINT(D_CMD_ERROR,"Cmd DataSize error\n");
            return;
        }


        for (i=0;i<(total_size);i++)
        {
            Cate_Data[i]=data_in[3+i];
            ///// Add temporary, because UPRINT is not work now///
            IQ_PRINT(kAutoBinAscii_Cmdtype,"W 0x%x / 0x%x = 0x%x\n",Cate,(Addr+i),Cate_Data[i]);
            //////////////////////////////////////////////////////
        }
        Cate_Set( Cate, Addr, total_size, &Cate_Data[0]);


    }

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }

}

static VOID Cmd_GetCateByte(UCHAR sid, UCHAR* data_in, LONG datasize)
{

    UCHAR Cate = 0;
    UCHAR Addr = 0;
    UCHAR total_size = 0;
    UCHAR i = 0 ;
    ULONG Cate_Data[10] = {0}; // M10V Cate_set need ULONG, why?
    UCHAR ret_val[10] = {0}; // Assume can support 10 return value only

    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    if (datasize < 3)
    {
        IQ_PRINT(D_CMD_ERROR,"Cmd size is %d and error\n",datasize);
        return;
    }
    else
    {
        Cate = data_in[0];
        Addr = data_in[1];
        total_size = data_in[2];
        if (data_in[2] == 0)
            total_size = 1;// in case user haven't insert data size
        else if (total_size > 10)
        {
            IQ_PRINT(D_CMD_ERROR,"Cannot support more than 10 datasize\n");
            return;
        }

        Cate_Get(Cate,Addr,total_size,&Cate_Data[0]);

        ///// Add temporary, because UPRINT is not work now///
        for(i=0;i<total_size;i++)
        {
            IQ_PRINT(kAutoBinAscii_Cmdtype,"R 0x%x / 0x%x = 0x%x\n",Cate,Addr+i,Cate_Data[i]);
            ret_val[i] = (UCHAR) (Cate_Data[i] & 0xFF);
        }
        //////////////////////////////////////////////////////

        if (sid < 16) // if larger than 15 should be ASCII cmd
        {
            BinCmd_Pack(&ret_val[0],&out_buf[0],&out_cmdlen,sid,total_size);
            uart_TxBuf_write(&out_buf[0],out_cmdlen);
        }
    }

}


static VOID Cmd_Set_32b_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    ULONG *addr_ptr = NULL;
    ULONG val = 0;
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(D_ASCII_DBG_MSG,"ISP Cmd_Set_32b_Memory\n");
	addr_ptr = (ULONG*)(BYTE_TOUINT32(&data_in[0]));
    if ( ((uint32_t)addr_ptr & 0x3 ) != 0)
    {
        IQ_PRINT(D_CMD_WARNING,"Warning:w32@%x (4-Byte alignment), =>@ %x\n", addr_ptr, ((uint32_t)addr_ptr & 0xFFFFFFFC));
		addr_ptr = (ULONG*)((uint32_t)addr_ptr & 0xFFFFFFFC);
	}

    val = BYTE_TOUINT32(&data_in[4]);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"w32@%x =0x%x \n",addr_ptr,val);

    *(addr_ptr) = val;//memcpy(addr_ptr,&data_in[4],4);

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Set_16b_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    uint16_t *addr_ptr = NULL;
    uint16_t val = 0;

    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

	addr_ptr = (uint16_t*)(BYTE_TOUINT32(&data_in[0]));
	if (((uint32_t)addr_ptr & 0x1) != 0)
	{
		IQ_PRINT(D_CMD_WARNING, "Warning:w16@%x (2-Byte alignment), =>@ %x\n", addr_ptr, ((uint32_t)addr_ptr & 0xFFFFFFFE));
		addr_ptr = (uint16_t*)((uint32_t)addr_ptr & 0xFFFFFFFE);
	}

    val =  BYTE_TOUINT16(&data_in[4]);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"w16@%x =0x%x \n",addr_ptr,val);

    *(addr_ptr) = val;

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Set_8b_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR *addr_ptr = NULL;
    UCHAR val = 0;
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    addr_ptr = (UCHAR*) BYTE_TOUINT32(&data_in[0]);
    val = (data_in[4]);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"w8@%x =0x%x \n",addr_ptr,val);

    *(addr_ptr) = val;

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Get_32b_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    ULONG *addr_ptr = NULL;
    ULONG val = 0;
    UCHAR val_pack[4] = {0};
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(D_ASCII_DBG_MSG,"ISP Cmd_Get_32b_Memory\n");

	addr_ptr = (ULONG*)(BYTE_TOUINT32(&data_in[0]));
	if (((uint32_t)addr_ptr & 0x3) != 0)
	{
		IQ_PRINT(D_CMD_WARNING, "Warning:r32@%x (4-Byte alignment), =>@ %x\n", addr_ptr, ((uint32_t)addr_ptr & 0xFFFFFFFC));
		addr_ptr = (ULONG*)((uint32_t)addr_ptr & 0xFFFFFFFC);
	}

    val = *(addr_ptr);

    val_pack[0] = (UCHAR)((val & 0xFF000000) >> 24);
    val_pack[1] = (UCHAR)((val & 0x00FF0000) >> 16);
    val_pack[2] = (UCHAR)((val & 0x0000FF00) >> 8);
    val_pack[3] = (UCHAR)(val & 0x000000FF);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"r32@%x =0x%x\n",addr_ptr,val);

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack(&val_pack[0],&out_buf[0],&out_cmdlen,sid,4);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Get_16b_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    uint16_t *addr_ptr = NULL;
    uint16_t val = 0;
    UCHAR val_pack[2] = {0};
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(D_ASCII_DBG_MSG,"ISP Cmd_Get_16b_Memory\n");

	addr_ptr = (uint16_t*)(BYTE_TOUINT32(&data_in[0]));
	if (((uint32_t)addr_ptr & 0x1) != 0)
	{
		IQ_PRINT(D_CMD_WARNING, "Warning:r16@%x (2-Byte alignment), =>@ %x\n", addr_ptr, ((uint32_t)addr_ptr & 0xFFFFFFFE));
		addr_ptr = (uint16_t*)((uint32_t)addr_ptr & 0xFFFFFFFE);
	}

    val = *(addr_ptr);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"r16@%x =0x%x\n",addr_ptr,val);

    val_pack[0] = (UCHAR)((val & 0xFF00) >> 8);
    val_pack[1] = (UCHAR)((val & 0x00FF));

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack(&val_pack[0],&out_buf[0],&out_cmdlen,sid,2);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }

}

static VOID Cmd_Get_8b_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR *addr_ptr = NULL;
    UCHAR val = {0};
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    addr_ptr = (UCHAR*)BYTE_TOUINT32(&data_in[0]);
    val = *(addr_ptr);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"r8@%x =0x%x\n",addr_ptr,val);

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack(&val,&out_buf[0],&out_cmdlen,sid,1);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Set_32b_MemoryMask(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    uint32_t *addr_ptr = NULL;
    uint32_t val = 0;
    uint32_t reg_val = 0;
    uint32_t mask = 0;
    uint32_t mask_result = 0;

    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(D_ASCII_DBG_MSG,"ISP Cmd_Set_32b_MemoryMask\n");

	addr_ptr = (ULONG*)(BYTE_TOUINT32(&data_in[0]));
	if (((uint32_t)addr_ptr & 0x3) != 0)
	{
		IQ_PRINT(D_CMD_WARNING, "Warning:w32mask@%x (4-Byte alignment), =>@ %x\n", addr_ptr, ((uint32_t)addr_ptr & 0xFFFFFFFC));
		addr_ptr = (ULONG*)((uint32_t)addr_ptr & 0xFFFFFFFC);
	}

    val = BYTE_TOUINT32(&data_in[4]);
    mask = BYTE_TOUINT32(&data_in[8]);

    reg_val = *(addr_ptr);
    mask_result = ((val & mask) | (reg_val & (~mask)) ) ;

    IQ_PRINT(kAutoBinAscii_Cmdtype,"w32mask@ %x =0x%x Mask %x result %x\n",addr_ptr,val,mask,mask_result);

    *(addr_ptr) = mask_result;

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }

}

static VOID Cmd_Set_16b_MemoryMask(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    uint16_t *addr_ptr = NULL;
    uint16_t val = 0;
    uint16_t reg_val = 0;
    uint16_t mask = 0;
    uint16_t mask_result = 0;

    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

	addr_ptr = (uint16_t*)(BYTE_TOUINT32(&data_in[0]));
	if (((uint32_t)addr_ptr & 0x1) != 0)
	{
		IQ_PRINT(D_CMD_WARNING, "Warning:w16mask@%x (2-Byte alignment), =>@ %x\n", addr_ptr, ((uint32_t)addr_ptr & 0xFFFFFFFE));
		addr_ptr = (uint16_t*)((uint32_t)addr_ptr & 0xFFFFFFFE);
	}

    val = BYTE_TOUINT16(&data_in[4]);
    mask = BYTE_TOUINT16(&data_in[6]);

    reg_val = *(addr_ptr);
    mask_result = ((val & mask) | (reg_val & (~mask)) ) ;

    IQ_PRINT(kAutoBinAscii_Cmdtype,"w16mask@ %x =0x%x Mask %x result %x\n",addr_ptr,val,mask,mask_result);

    *(addr_ptr) = mask_result;

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_Set_8b_MemoryMask(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR *addr_ptr = NULL;
    UCHAR reg_val = 0;
    UCHAR val = 0;
    UCHAR mask = 0;
    UCHAR mask_result = 0;

    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    addr_ptr = (UCHAR*)BYTE_TOUINT32(&data_in[0]);
    val = data_in[4];
    mask = data_in[5];

    reg_val = *(addr_ptr);

    mask_result = ((val & mask) | (reg_val & (~mask)) ) ;

    IQ_PRINT(kAutoBinAscii_Cmdtype,"w8mask@ %x =0x%x Mask %x result %x\n",addr_ptr,val,mask,mask_result);

    *(addr_ptr) = mask_result;

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_SetDbgMask(UCHAR sid, UCHAR* data_in, LONG datasize)
{

    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;
    ULONG mask_val =0;

    mask_val = BYTE_TOUINT32(&data_in[0]);

    Set_Print_MsgMask(mask_val);

    IQ_PRINT(D_ALWAYS,"Set Debug Mask to %x\n",mask_val);

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack_ACK_ONLY(&out_buf[0],&out_cmdlen,sid);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}

static VOID Cmd_GetDbgMask(UCHAR sid)
{
    ULONG val = 0;
    UCHAR val_pack[4] = {0};
    UCHAR out_buf[MaxBinCmdSize] = {0};
    ULONG out_cmdlen = 0;

    IQ_PRINT(D_ASCII_DBG_MSG,"Cmd_GetDBGMask\n");

    val = Get_Print_MsgMask();

    val_pack[0] = (UCHAR)((val & 0xFF000000) >> 24);
    val_pack[1] = (UCHAR)((val & 0x00FF0000) >> 16);
    val_pack[2] = (UCHAR)((val & 0x0000FF00) >> 8);
    val_pack[3] = (UCHAR)(val & 0x000000FF);

    IQ_PRINT(kAutoBinAscii_Cmdtype,"Debug Mask is: %x\n",val);

    if (sid < 16) // if larger than 15 should be ASCII cmd
    {
        BinCmd_Pack(&val_pack[0],&out_buf[0],&out_cmdlen,sid,4);
        uart_TxBuf_write(&out_buf[0],out_cmdlen);
    }
}


static VOID Cmd_Set_Burst_Memory (UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_Burst_Memory(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_AE_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_AWB_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_AF_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_SHD_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_IQ_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_SmartAE_Control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_AFN_onoff(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_SPR_onoff(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_AE_adv_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_EVP_adv_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_AWB_adv_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_IQ_adv_control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_SHD_adv_control (UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_General_Info(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_IQ_info(UCHAR sid, UCHAR* data_in, LONG datasize)
{
	IQPara_Tune_PrintState();
       //iqpara_info_print();
}

static VOID Cmd_Get_AE_info(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_AWB_info(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_SHD_info(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_ae_static_data(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_AWB_static_data(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Get_AF_static_data(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Auto_periodic_Info(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}



static VOID Set_Burst_SessionID(UCHAR val)
{
    gSessionID_burst = val;
}

static UCHAR Get_Burst_SessionID()
{
    return gSessionID_burst;
}

static VOID Next_Burst_SessionID()
{
    Set_Burst_SessionID(gSessionID++);

    if (gSessionID > 254)
    {
        gSessionID = 0;
    }
}

static VOID SetBurstDirection(UCHAR val)
{
    iBurstDirection = val;
}

static UCHAR GetBurstDirection()
{
    return iBurstDirection;
}

static VOID Set_Buffer_Cur_PT(UCHAR* p)
{
    Burst_Cur_pt = p;
}

static VOID Set_Buffer_Start_PT(UCHAR* p)
{
    Burst_Buf_pt = p;
}

static UCHAR* Get_Buffer_Cur_PT()
{
    return Burst_Cur_pt;
}

static UCHAR* Get_Buffer_Start_PT()
{
    return Burst_Buf_pt;
}

static VOID Set_Buffer_Size(int val)
{
    Burst_Buffer_Size = val;
}

static UINT32 Get_Buffer_Size()
{
    return Burst_Buffer_Size;
}

static UCHAR WriteToBuffer(UCHAR *data_in, ULONG data_in_size)
{
    UCHAR data_topack[10] = {0};
    UCHAR data_out[30] = {0};
    ULONG cmdlen = 0;


    UCHAR* p = Get_Buffer_Cur_PT();
    {
        memcpy(p,&data_in[0],data_in_size);
        p += data_in_size;
        Burst_Rcv_cnt += data_in_size;

        if (Burst_Rcv_cnt  >=  Get_Buffer_Size())
        {
            Burst_Rcv_cnt=0;
            Set_Buffer_Cur_PT(Get_Buffer_Start_PT());

            data_topack[0] = Get_Burst_SessionID();
            data_topack[1] = 0xFF;
            data_topack[2] = 0xFF;
            data_topack[3] = 0xFF;
            data_topack[4] = 0xFF;
            data_topack[5] = (Get_Buffer_Size() & 0xFF000000) >> 24;
            data_topack[6] = (Get_Buffer_Size() & 0xFF0000) >> 16;
            data_topack[7] = (Get_Buffer_Size() & 0xFF00) >> 8;
            data_topack[8] =  Get_Buffer_Size() & 0xFF;
            BinCmd_Pack_Periodic_ACK(&data_topack[0],&data_out[0],&cmdlen,9);
            uart_TxBuf_write(&data_out[0],cmdlen);
            //just send finish packet
            return 0;
        }
        else
        {
            //cur = (UINT32)Burst_Rcv_cnt;
            Set_Buffer_Cur_PT(p);
        }


        return 1;
    }
}
static VOID send_data_to_pc()
{
    UCHAR* dptr = NULL;
    ULONG read_rp = 0;
    ULONG read_wp = 0;
    UINT32  divide_cnt = 0;
    UINT32  resduce_size = 0;

    divide_cnt = Get_Buffer_Size()/ MAX_UART_FIFO_SIZE;
    resduce_size = Get_Buffer_Size() - (divide_cnt * MAX_UART_FIFO_SIZE);
    dptr = Get_Buffer_Start_PT(); //(UCHAR*)Cmd55_GRes.Mem_ptr;
    Clr_Print_MsgMask_Bit(D_SYSTEM_MSG); // FOR burst mode, no debug msg can send out.
    setUartMode(DEBUG_BLOCK);

    while (divide_cnt != 0)
    {
        uart_TxBuf_write(dptr,MAX_UART_FIFO_SIZE);
        read_rp = Get_gtxbuf_rp();
        read_wp = Get_gtxbuf_wp();
        if (read_wp > read_rp)
        {
            if (read_wp - read_rp >= 2000)
            {
                OS_User_Dly_Tsk(10);
            }
        }
        else if (read_wp < read_rp)
        {
            if (kTxBuf_Size - read_rp + read_wp >= 2000)
            {
                OS_User_Dly_Tsk(10);
            }
        }
        else{}

        dptr += MAX_UART_FIFO_SIZE;
        divide_cnt--;

    }
    uart_TxBuf_write(dptr,resduce_size);
}

static VOID Cmd_Lock_Secssion(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR data_topack[2] = {0};
    UCHAR data_out[10] = {0};
    ULONG cmdlen = 0;
    UCHAR SessionID = data_in[0];
    UCHAR err_code = 0;


    if (GetBurstDirection() == rISPtoPC) // // 1:PC-> ISP ,2:ISP -> PC
    {
        if ( Get_Burst_SessionID() == SessionID) // ensure the Session is the same
        {
            err_code = 0;
            data_topack[0] = err_code;
            data_topack[1] = Get_Burst_SessionID();
            BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
            uart_TxBuf_write(&data_out[0],cmdlen);
            send_data_to_pc();
        }
        else
        {
            err_code = 1;
            IQ_PRINT(D_CMD_ERROR,"ISP SessionID not equal.\n");
            data_topack[0] = err_code;
            data_topack[1] = Get_Burst_SessionID();
            BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
            uart_TxBuf_write(&data_out[0],cmdlen);
        }
            //State_Cmd50 = State_Cmd50_Lock_Session_Ready;
            //return;
        //}
    }

    if (GetBurstDirection() == rPCtoISP) // 1:PC-> ISP ,2:ISP -> PC
    {
        if ( Get_Burst_SessionID() == SessionID) // ensure the Session is the same
        {
            err_code = 0;

            data_topack[0] = err_code;
            data_topack[1] = Get_Burst_SessionID();
            BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
            uart_TxBuf_write(&data_out[0],cmdlen);
            //IQ_INFO_PRINTLN("Set IQ_Mode 2");
            Clr_Print_MsgMask_Bit(D_SYSTEM_MSG);
            setUartMode(DEBUG_BLOCK);
            IQCATE_GCTRL.var.IQ_MODE = 2;
            //State_Cmd50 = State_Cmd50_Lock_Session_Ready;
        }
        else
        {
            err_code = 1;
            IQ_PRINT(D_CMD_ERROR,"ISP SessionID not equal.\n");
            data_topack[0] = err_code;
            data_topack[1] = Get_Burst_SessionID();
            BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
            uart_TxBuf_write(&data_out[0],cmdlen);
            IQCATE_GCTRL.var.IQ_MODE = 0;
            Set_Print_MsgMask_Bit(D_SYSTEM_MSG);
            setUartMode(DEBUG_GO);
        }
    }
}

static VOID Cmd_Unlock_Secion(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR data_topack[3] = {0};
    UCHAR data_out[20] = {0};
    ULONG cmdlen = 0;
    ULONG lFileSize = 0;
    UCHAR* dptr = NULL;
    UCHAR CheckSum = 0;
    UCHAR PC_SessionID = 0;
    UCHAR PC_CheckSum = 0;
    UCHAR err_code =0;


    if (GetBurstDirection() == rISPtoPC) // FROM ISP to PC
    {
        dptr = Get_Buffer_Start_PT();
        for (lFileSize = 0 ; lFileSize < Get_Buffer_Size() ; lFileSize++)
        {
            CheckSum += *(dptr + lFileSize);
        }
    }
    else if (GetBurstDirection() == rPCtoISP)
    {
        dptr = Get_Buffer_Start_PT();
        for (lFileSize = 0 ; lFileSize < Get_Buffer_Size() ; lFileSize++)
        {
            CheckSum += *(dptr + lFileSize);
        }
        IQCATE_GCTRL.var.IQ_MODE = 0;
        (gGetRes+Cmd56_GRes.ID)->Cursize = Get_Buffer_Size(); //update related Resource Cursize

    }
        Set_Print_MsgMask_Bit(D_SYSTEM_MSG);
        setUartMode(DEBUG_GO);

        PC_SessionID = data_in[0];
        PC_CheckSum = data_in[1];

        err_code = 0;
        if (!(PC_CheckSum == CheckSum))
        {
            err_code = 1;
        }

        if (!(PC_SessionID == Get_Burst_SessionID()))
        {
            err_code = 2;
        }

        // if no error, then Session complete
        //IQ_INFO_PRINTLN("ISP checksum %d",CheckSum);
        data_topack[0] = err_code;
        data_topack[1] = Get_Burst_SessionID();
        data_topack[2] = CheckSum;

        BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,3);
        uart_TxBuf_write(&data_out[0],cmdlen);
}

static VOID Cmd_GetResourceList_info(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    ULONG cmdlen = 0;
    UINT32 i =0;
    ULONG pack_cnt = 1; // 0 is resource total number
    UINT32 valid_cnt = 0; // for valid resource number
    UCHAR* pdata_pack = NULL;
    UCHAR* pdata_out = NULL;

    update_GetResource();

    if ((pdata_pack = (UCHAR*)malloc( (sizeof(_GetResource)* MAX_RESOURCE_NUM)+1)) == NULL) {
        IQ_PRINT(D_CMD_ERROR,"Cannot Allocate enough memory for data_pack, exit.\n");
		return ;
	}

    if ((pdata_out = (UCHAR*)malloc( (sizeof(_GetResource)*MAX_RESOURCE_NUM*2)+5)) == NULL) {
        IQ_PRINT(D_CMD_ERROR,"Cannot Allocate enough memory for data_out, exit.\n");
		return ;
	}

	IQ_PRINT(D_BIN_CMD_REPLY,"--- Cmd_GetResourceList_info ---\n");
    IQ_PRINT(D_BIN_CMD_REPLY,"Name    ID  Size    MaxSize     Addr\n");
    IQ_PRINT(D_BIN_CMD_REPLY,"--------------------------------------------------------\n");


    while(i<MAX_RESOURCE_NUM)
    {
        if ((gGetRes+i)->ID != 0xFF)
        {
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[0];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[1];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[2];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[3];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[4];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[5];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[6];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->Name[7];
            pdata_pack[pack_cnt++] =   (gGetRes+i)->ID;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Mem_ptr & 0xFF000000) >>24;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Mem_ptr & 0x00FF0000) >>16;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Mem_ptr & 0x0000FF00) >> 8;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Mem_ptr & 0x000000FF);
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Maxsize & 0xFF000000) >>24;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Maxsize & 0x00FF0000) >>16;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Maxsize & 0x0000FF00) >> 8;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Maxsize & 0x000000FF);
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Cursize & 0xFF000000) >>24;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Cursize & 0x00FF0000) >>16;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Cursize & 0x0000FF00) >> 8;
            pdata_pack[pack_cnt++] =   ((gGetRes+i)->Cursize & 0x000000FF);

            IQ_PRINT(D_BIN_CMD_REPLY,"%s  %d  %d  %d  %x\n", (gGetRes+i)->Name,(gGetRes+i)->ID,(gGetRes+i)->Cursize,(gGetRes+i)->Maxsize,(gGetRes+i)->Mem_ptr);
            valid_cnt ++;
        }
        else
            break;
        i++;
    }
    pdata_pack[0] = valid_cnt; //
    IQ_PRINT(D_BIN_CMD_REPLY,"Total Resource number %d \n", pdata_pack[0]);
    IQ_PRINT(D_BIN_CMD_REPLY,"--- Cmd_GetResourceList_info End ---\n");

    if (sid < 16)
    {
        BinCmd_Pack(&pdata_pack[0],&pdata_out[0],&cmdlen,sid,(21*valid_cnt+1) );
        uart_TxBuf_write(&pdata_out[0],cmdlen);
    }

}


static VOID Cmd_GetResource_from_ISP(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_SendResource_to_ISP(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_GetResource_from_ISP_burst(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR data_topack[6] = {0};
    UCHAR data_out[20] = {0};
    UCHAR name[8] = {0};
    ULONG cmdlen = 0;
    UCHAR err_code = 0;

    Cmd55_GRes.ID = data_in[0];

    if (Cmd55_GRes.ID < Get_Total_Resource_Num())
    {
        memcpy(name,&data_in[1],8);
        Cmd55_GRes.Cursize = (gGetRes + Cmd55_GRes.ID) -> Cursize;
        Cmd55_GRes.Mem_ptr = (gGetRes + Cmd55_GRes.ID) -> Mem_ptr;
    }
    else
    {
        IQ_PRINTLN(D_BIN_CMD_REPLY,"Cannot find bin");
        err_code = 1;
    }


    //OK? told PC
    Next_Burst_SessionID();

    data_topack[0] = err_code;
    data_topack[1] = Get_Burst_SessionID();
    data_topack[2] = ((Cmd55_GRes.Cursize & 0xFF000000) >> 24);
    data_topack[3] = ((Cmd55_GRes.Cursize & 0xFF0000) >> 16);
    data_topack[4] = ((Cmd55_GRes.Cursize & 0xFF00) >> 8);
    data_topack[5] = (Cmd55_GRes.Cursize & 0xFF);

    SetBurstDirection(rISPtoPC); // // 1:PC-> ISP ,2:ISP -> PC

    Set_Buffer_Start_PT((UCHAR*)Cmd55_GRes.Mem_ptr);
    Set_Buffer_Cur_PT((UCHAR*)Cmd55_GRes.Mem_ptr);

    Set_Buffer_Size(Cmd55_GRes.Cursize);

    BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,6);
    uart_TxBuf_write(&data_out[0],cmdlen);

}

static VOID Cmd_SendResource_to_ISP_burst(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR data_topack[2] = {0};
    UCHAR data_out[10] = {0};
    UCHAR name[8] = {0};
    ULONG buf_size = 0;
    UCHAR err_code = 0;
    ULONG cmdlen = 0;
    UCHAR* p = NULL;

    Cmd56_GRes.ID = data_in[0];
    memcpy(name,&data_in[1],8);
    buf_size = BYTE_TOUINT32(&data_in[9]);

    //IQ_PRINT(D_ASCII_DBG_MSG,"ISP rcv Bufsize %d\n",buf_size);
    if (Cmd56_GRes.ID < Get_Total_Resource_Num())
    {
        memcpy(name,&data_in[1],8);
        Cmd56_GRes.Maxsize = (gGetRes + Cmd56_GRes.ID) ->Maxsize;
        Cmd56_GRes.Cursize = (gGetRes + Cmd56_GRes.ID) ->Cursize;
        Cmd56_GRes.Mem_ptr = (gGetRes + Cmd56_GRes.ID) ->Mem_ptr;
        p = (UCHAR*)Cmd56_GRes.Mem_ptr;
    }
    else
    {
        IQ_PRINT(D_CMD_ERROR,"Cannot find bin\n");
        err_code = 1;
    }


    if (err_code == 0)
    {
        if (buf_size > Cmd56_GRes.Maxsize)
        {
            IQ_PRINT(D_CMD_ERROR,"Input size %d bigger than Maximum size %d\n", buf_size, Cmd56_GRes.Maxsize);
            err_code = 2;
        }
    }

    Set_Buffer_Start_PT(p);
    Set_Buffer_Cur_PT(p);
    Set_Buffer_Size(buf_size);

    Next_Burst_SessionID();
    SetBurstDirection(rPCtoISP); // 1:PC-> ISP ,2:ISP -> PC

    data_topack[0] = err_code;
    data_topack[1] = Get_Burst_SessionID();

    BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
    uart_TxBuf_write(&data_out[0],cmdlen);
}

static VOID Cmd_Get_memory_from_ISP(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR data_topack[2] = {0};
    UCHAR data_out[10] = {0};
    ULONG cmdlen = 0;
    UCHAR err_code = 0;
    ULONG buf_size = 0;
    ULONG buf_addr = 0;
    UCHAR* p = NULL;


    buf_addr = BYTE_TOUINT32(&data_in[0]);
    buf_size = BYTE_TOUINT32(&data_in[4]);
    p = (UCHAR *)buf_addr;

    if (buf_size == 0)
    {
        err_code = 1;
    }
    else
        Set_Buffer_Size(buf_size);

    Next_Burst_SessionID();

    data_topack[0] = err_code;
    data_topack[1] = Get_Burst_SessionID();

    SetBurstDirection(rISPtoPC); // // 1:PC-> ISP ,2:ISP -> PC

    Set_Buffer_Start_PT(p);
    Set_Buffer_Cur_PT(p);

    BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
    uart_TxBuf_write(&data_out[0],cmdlen);
}

static VOID Cmd_Send_memory_to_ISP(UCHAR sid, UCHAR* data_in, LONG datasize)
{
    UCHAR data_topack[2] = {0};
    UCHAR data_out[10] = {0};
    UINT32 buf_size = 0;
    UCHAR err_code = 0;
    ULONG cmdlen = 0;
    UCHAR* p = NULL;

    p = (UCHAR *) BYTE_TOUINT32(&data_in[0]);
    buf_size = BYTE_TOUINT32(&data_in[4]);

    if (buf_size == 0)
    {
        err_code = 1;
    }
    else
        Set_Buffer_Size(buf_size);
    //IQ_PRINTLN(D_CMD_ERROR,"Cmd58 buf size %d",buf_size);
    Set_Buffer_Start_PT(p);
    Set_Buffer_Cur_PT(p);


    Next_Burst_SessionID();
    SetBurstDirection(rPCtoISP); // 1:PC-> ISP ,2:ISP -> PC

    data_topack[0] = err_code;
    data_topack[1] = Get_Burst_SessionID();

    BinCmd_Pack(&data_topack[0],&data_out[0],&cmdlen,sid,2);
    uart_TxBuf_write(&data_out[0],cmdlen);
}

static VOID Cmd_Get_file_from_ISP(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Send_file_to_ISP(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Save_Reg_Data(UCHAR sid, UCHAR* data_in, LONG datasize)
{
	ULONG	addr = 0;
	ULONG	size = 0;
	ULONG	total = 0;
	INT32	ret  = 0;
	UCHAR	data_topack[10] = {0};
	UCHAR	data_out[30] = {0};
	ULONG	datalen = 0;
	ULONG	cmdlen = 0;
	
	// Request Save Reg
	Send_Share_System_Event( E_SHARE_SYSTEM_EVENT_IQ_DBG2_SAVE_REG, 0 );
	
	// Wait
	while( 1 ){
		// Wait
		ret = IQ_Dbg2_Wait_End( 3000 ); // 3sec temporary
		if( ret ){
			break; // NG
		}
		
		// Get Header
		ret = IQ_Dgb2_Get_Head( &addr, &size, &total );
		if( ret ){
			break; // NG
		}
		
		// Cache
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)addr, (UINT32)total );
		break; // OK
	}
	
	// Return ( ret, addr, size ) to Bin format
	datalen = 0;
	data_topack[ datalen++ ] = (UCHAR)ret;
	data_topack[ datalen++ ] = (addr & 0xFF000000) >> 24;
	data_topack[ datalen++ ] = (addr & 0xFF0000) >> 16;
	data_topack[ datalen++ ] = (addr & 0xFF00) >> 8;
	data_topack[ datalen++ ] =  addr & 0xFF;
	data_topack[ datalen++ ] = (size & 0xFF000000) >> 24;
	data_topack[ datalen++ ] = (size & 0xFF0000) >> 16;
	data_topack[ datalen++ ] = (size & 0xFF00) >> 8;
	data_topack[ datalen++ ] =  size & 0xFF;
	BinCmd_Pack( &data_topack[0], &data_out[0], &cmdlen, sid, datalen );
	
	// Send
	uart_TxBuf_write(&data_out[0],cmdlen);
}

static VOID Cmd_Set_OSD_Control(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Set_Customer_string(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Set_OSD_status(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Set_OSD_status_extend(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Set_OSD_position(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Customer_API_1(UCHAR sid, UCHAR* data_in, LONG datasize)
{

}

static VOID Cmd_Handler(UCHAR in_cmd, UCHAR *in_data, ULONG in_datalen, UCHAR in_sid)
{
    //UCHAR cmd =in_cmd;
    switch(in_cmd)
    {
           case E_Cmd_GetSystemVersion:
                    Cmd_GetSystemVersion(in_sid);
               break;
           case E_Cmd_GetAllModeInfo:
                    Cmd_GetAllModeInfo(in_sid,in_data,in_datalen);
               break;
           case E_Cmd_GetModeInfo:
                    Cmd_GetModeInfo(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_GetCurrentMode:
                    Cmd_GetCurrentMode(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_SetMode:
                    Cmd_SetMode(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Exit:
                    Cmd_Exit(in_sid);
                break;
           case E_Cmd_Help:
                    Cmd_Help(in_sid);
                break;
           case E_Cmd_SetCateByte:
                    Cmd_SetCateByte(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_GetCateByte:
                    Cmd_GetCateByte(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_SetDbgMask:
                    Cmd_SetDbgMask(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_GetDbgMask:
                    Cmd_GetDbgMask(in_sid);
                break;
           case E_Cmd_Set_32b_Memory:
                    Cmd_Set_32b_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Set_16b_Memory:
                    Cmd_Set_16b_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Set_8b_Memory:
                    Cmd_Set_8b_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_32b_Memory:
                    Cmd_Get_32b_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_16b_Memory:
                    Cmd_Get_16b_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_8b_Memory:
                    Cmd_Get_8b_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Set_32b_MemoryMask:
                    Cmd_Set_32b_MemoryMask(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Set_16b_MemoryMask:
                    Cmd_Set_16b_MemoryMask(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Set_8b_MemoryMask:
                    Cmd_Set_8b_MemoryMask(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Set_Burst_Memory:
                    Cmd_Set_Burst_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_Burst_Memory:
                    Cmd_Get_Burst_Memory(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_AE_control:
                    Cmd_AE_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_AWB_control:
                    Cmd_AWB_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_AF_control:
                    Cmd_AF_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_SHD_control:
                    Cmd_SHD_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_IQ_control:
                    Cmd_IQ_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_SmartAE_Control:
                    Cmd_SmartAE_Control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_AFN_onoff:
                    Cmd_AFN_onoff(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_SPR_onoff:
                    Cmd_SPR_onoff(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_AE_adv_control:
                    Cmd_AE_adv_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_EVP_adv_control:
                    Cmd_EVP_adv_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_AWB_adv_control:
                    Cmd_AWB_adv_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_IQ_adv_control:
                    Cmd_IQ_adv_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_SHD_adv_control:
                    Cmd_SHD_adv_control(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_General_Info:
                    Cmd_Get_General_Info(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_IQ_info:
                    Cmd_Get_IQ_info(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_AE_info:
                    Cmd_Get_AE_info(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_AWB_info:
                    Cmd_Get_AWB_info(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_SHD_info:
                    Cmd_Get_SHD_info(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_ae_static_data:
                    Cmd_Get_ae_static_data(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_AWB_static_data:
                    Cmd_Get_AWB_static_data(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Get_AF_static_data:
                    Cmd_Get_AF_static_data(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Auto_periodic_Info:
                    Cmd_Auto_periodic_Info(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Lock_Secssion:
                    Cmd_Lock_Secssion(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_Unlock_Secion:
                    Cmd_Unlock_Secion(in_sid,in_data,in_datalen);
                break;
           case E_Cmd_GetResourceList_info:
			   Cmd_GetResourceList_info(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_GetResource_from_ISP:
			   Cmd_GetResource_from_ISP(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_SendResource_to_ISP:
			   Cmd_SendResource_to_ISP(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_GetResource_from_ISP_burst:
			   Cmd_GetResource_from_ISP_burst(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_SendResource_to_ISP_burst:
			   Cmd_SendResource_to_ISP_burst(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Get_memory_from_ISP:
			   Cmd_Get_memory_from_ISP(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Send_memory_to_ISP:
			   Cmd_Send_memory_to_ISP(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Get_file_from_ISP:
			   Cmd_Get_file_from_ISP(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Send_file_to_ISP:
			   Cmd_Send_file_to_ISP(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Save_Reg_Data:
			   Cmd_Save_Reg_Data(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Set_OSD_Control:
			   Cmd_Set_OSD_Control(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Set_Customer_string:
			   Cmd_Set_Customer_string(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Set_OSD_status:
			   Cmd_Set_OSD_status(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Set_OSD_status_extend:
			   Cmd_Set_OSD_status_extend(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Set_OSD_position:
			   Cmd_Set_OSD_position(in_sid, in_data, in_datalen);
			   break;
		   case E_Cmd_Customer_API_1:
			   Cmd_Customer_API_1(in_sid, in_data, in_datalen);
			   break;
		   default:
			   IQ_PRINT(D_CMD_ERROR, "Not supported Bin Cmd\n");
			   break;
	}
}

static VOID ASCIICmd_Handler(UCHAR **argv, ULONG long_argc)
{
    UCHAR cmd=0;
    UINT32 i=0;   // can larger then 0xff
 //   UINT32 j=0;
 //   ULONG tmp=0;
 //   ULONG arg_long[10] = {0};
    UCHAR argc = long_argc ; // No need long for keep number of arg , change to char,
    UCHAR out_data[50] = {0}; // For convert to 8bit array.
    ULONG out_data_cnt = 0;
    UCHAR fake_sid = 0xFF;
    UINT32 Total_AsciiCmd_Num = (sizeof(ascii_node) / sizeof(_ASCIICmdNode));

    for (i=0;i<Total_AsciiCmd_Num;i++)  //TBL for ascii cmd...
    {
        if (strcmp((const char*)argv[0],(const char*)ascii_node[i].name) == 0)
        {
            cmd = ascii_node[i].id;
            //IQ_PRINTLN(D_CMD_ERROR,"Cmd found in %d",i);
            break;
        }
    }
    if (i == Total_AsciiCmd_Num)
    {
        IQ_PRINT(D_CMD_ERROR,"Cmd %s is not found, exit \n",argv[0]);
        return;
    }


    if (cmd == 0x06) //0x06 is "rtos" cmd
    {
        Cate_Decode_String(argc - 1 , (CHAR**)(argv + 1));
        return;
    }

	if (argc > 10)
	{
		IQ_PRINT(D_CMD_ERROR, "accepted cmd data cannot exceed 10\n");
		return;
	}

	if (cmd == E_Cmd_IQ_control)
	{
		const char **myargv = (const char**)(argv);
		int cnt = 1;
		int ret = 0;

		IQ_PRINT(D_ASCII_CMD_REPLY, "IQ Control:");


		while (myargv != NULL && cnt < argc)
		{
			if (myargv[cnt] == NULL)
				break;
			if (0==strcmp(myargv[cnt], "SetBinAddr"))
			{
				unsigned long addr;

				if ((cnt + 1) < argc){
					addr = strtoul(myargv[cnt+1], NULL, 16);
					cnt++;
					ret = IQPara_Tune_SetTuneBin((ULONG*)addr, 0x10000);
					IQ_PRINT(D_ASCII_CMD_REPLY, "SetTuneBin @%08X, ret:0x%x\n", addr, ret);
				} else {
					IQ_PRINT(D_ASCII_CMD_REPLY, "SetTuneBin, no data\n");
				}
			}
			else if (0==strcmp(myargv[cnt], "tuning")) {
				ret =IQPara_Tune_TuningBinMode();
				if (ret==0)
					IQ_PRINT(D_ASCII_CMD_REPLY, "Switch Tuning Bin\n");
				else
					IQ_PRINT(D_ASCII_CMD_REPLY, "Switch Tuning Bin Fail, please confirm address\n", ret);
			}
			else if (0==strcmp(myargv[cnt], "normal")) {
				ret =IQPara_Tune_NormalBinMode();
				if (ret == 0)
					IQ_PRINT(D_ASCII_CMD_REPLY, "Switch Noraml Bin OK\n");
				else
					IQ_PRINT(D_ASCII_CMD_REPLY, "Switch Noraml Bin fail, %i\n", ret);

			}
			else if (0==strcmp(myargv[cnt ], "pause")) {
				ret =IQPara_Tune_UpdateOnOff(0);
				IQ_PRINT(D_ASCII_CMD_REPLY, "Pause para update,ret=%i\n", ret);
			}
			else if (0==strcmp(myargv[cnt], "resume")) {
				ret =IQPara_Tune_UpdateOnOff(1);
				IQ_PRINT(D_ASCII_CMD_REPLY, "Resume para update,ret=%i\n", ret);
			}
			else if (0==strcmp(myargv[cnt ], "sceneswap")) {
				int scene_org;
				int scene_target;
				if ((cnt + 2) < argc && myargv[cnt + 1]!= NULL && myargv[cnt + 2] != NULL) {
					scene_org = atoi(myargv[cnt + 1]);
					scene_target = atoi(myargv[cnt + 2]);
					IQPara_Tune_SceneMapping((UCHAR)scene_org, (UCHAR)scene_target);
					IQ_PRINT(D_ASCII_CMD_REPLY, "Scene Swap: %i -> %i \n", scene_org, scene_target);
				}
				else {
					IQ_PRINT(D_ASCII_CMD_REPLY, "Scene Swap:, Error: No data\n");

				}
				cnt += 2;

			}
			else if (0==strcmp(myargv[cnt], "gainforce")) {
				float gain;
				cnt++;
				if (cnt < argc){
					gain = atof(myargv[cnt]);
					IQPara_Tune_ForceGainNumber(gain);
					IQ_PRINT(D_ASCII_CMD_REPLY, "Force Gain:%0.2f\n", gain);
				}
				else {
					IQ_PRINT(D_ASCII_CMD_REPLY, "Force Gain:, Error: No data\n");
				}
			}
			else if (0==strcmp(myargv[cnt], "reset")) {
				IQPara_Tune_Reset();
				IQ_PRINT(D_ASCII_CMD_REPLY, "Reset scene & gain paraemter\n");
			}
			else if (0 == strcmp(myargv[cnt], "dump")) {
				static unsigned long addr=0xA0000000;
				int i;
				const int dumpcnt = 128;
				char buf[128];
				char *p;

				if ((cnt + 1) < argc) {
					unsigned long tmp;
					tmp = strtoul(myargv[cnt + 1], NULL, 16);
					cnt++;
					if (tmp != 0)
						addr = tmp;
				} 

				memcpy(buf, (char*)addr, dumpcnt);
				IQ_PRINT(D_ASCII_CMD_REPLY, "Dump @%08X\n", addr, ret);
				for (i = 0; i < 8; i++)
				{
					int j;
					p = buf+ i * 16;
					IQ_PRINT(D_ASCII_CMD_REPLY, "%08X ", addr + i*16);
					for (j = 0; j < 16; j++)
					{
						int ic;
						unsigned char c = (unsigned char) p[j];
						ic = c;
						if (j == 8 )
							IQ_PRINT(D_ASCII_CMD_REPLY, " ");
						IQ_PRINT(D_ASCII_CMD_REPLY, "%02x ", ic);
					}
					IQ_PRINT(D_ASCII_CMD_REPLY, " : ");
					for (j = 0; j < 16; j++)
					{
						unsigned char c; 
						c = (unsigned char )p[j];
						if (c < 0x20 ||  c >0x7F)
							c = '.';
						if ( j == 8 )
							IQ_PRINT(D_ASCII_CMD_REPLY, " ");
						IQ_PRINT(D_ASCII_CMD_REPLY, "%c", c);
					}
					IQ_PRINT(D_ASCII_CMD_REPLY, "\n");
				}
				addr += dumpcnt;  // auto show next address at next dump
			}
			else {
				IQ_PRINT(D_ASCII_CMD_REPLY, "%s, Unknown sub-command\n", myargv[cnt]);
			}
			cnt++;
		}
		return;
	}
	switch (cmd)
	{
		case E_Cmd_Get_memory_from_ISP:
		{
			if (argc>2) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				fake_sid = 0;
			}

		}
			break;
		case E_Cmd_Lock_Secssion:
		case E_Cmd_Unlock_Secion:
			if (argc>1){
				out_data[0] = (CHAR)strtoul((const CHAR*)argv[1], NULL, 16);
				out_data_cnt ++;
				fake_sid = 0;
			}
			break;
		case E_Cmd_Save_Reg_Data:
			fake_sid = 0;
			break;
		case E_Cmd_SetDbgMask: // 32bit,
		case E_Cmd_Get_32b_Memory: // 32bit,
		case E_Cmd_Get_16b_Memory: // 32bit
		case E_Cmd_Get_8b_Memory:  //32bit
		{
			if (argc>1){
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
			}
		}
			break;
		case E_Cmd_Set_32b_Memory: // 32bit, 32bit, 
		{
			if (argc>2) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
			}

		}
			break;
		case E_Cmd_Set_16b_Memory: // 32bit, 16bit, 
		{
			if (argc>2) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				_16bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 2;
			}

		}
		break;
		case E_Cmd_Set_8b_Memory: // 32bit, 8bit, 
			if (argc>2) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				out_data[out_data_cnt++] = v & 0xFF;
			}
			break;
		case E_Cmd_Set_32b_MemoryMask: // 32bit, 32bit, 32bit, 
			if (argc>3) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[3], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
			}
			break;
		case E_Cmd_Set_16b_MemoryMask: // 32bit, 16bit, 16bit, 
			if (argc>2) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				_16bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 2;
				v = strtoul((const CHAR*)argv[3], NULL, 16);
				_16bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 2;
			}
			break;
		case E_Cmd_Set_8b_MemoryMask: // 32bit, 8bit,  8bit, 
			if (argc>2) {
				unsigned long v;
				v = strtoul((const CHAR*)argv[1], NULL, 16);
				_32bit_TO_BIG_ENDIAN(v, out_data + out_data_cnt);
				out_data_cnt += 4;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				out_data[out_data_cnt++] = v & 0xFF;
				v = strtoul((const CHAR*)argv[2], NULL, 16);
				out_data[out_data_cnt++] = v & 0xFF;
			}
			break;
		default:

			break;
	}
	/*
	if (argc > 1)
    {
        for (i=0;i<argc -1;i++) // we dont need argv[0] as this is cmd. not data
        {

            arg_long[i] = strtoul((const CHAR*)argv[i+1],NULL,16);

            tmp = arg_long[i];  //Assume data came in is no problem..

            if ((tmp >> 24 != 0))
            {
                for (j = 0;j< 4;j++)
                {
                    out_data[out_data_cnt++] = ((tmp & 0xFF000000) >> 24);
                    tmp = tmp << 8;
                }
            }
            else if ((tmp >> 16) != 0)
            {
                for (j = 0;j< 3;j++)
                {
                    out_data[out_data_cnt++] = ((tmp & 0xFF0000) >> 24);
                    tmp = tmp << 8;
                }
            }
            else if ((tmp >> 8) != 0)
            {
                for (j = 0;j< 2;j++)
                {
                    out_data[out_data_cnt++] = ((tmp & 0xFF00) >> 8);
                    tmp = tmp << 8;
                }
            }
            else
            {
                out_data[out_data_cnt++] = (UCHAR)tmp;
            }

        }
    }
	*/
    Cmd_Handler(cmd,&out_data[0],out_data_cnt,fake_sid);

}



static VOID BinCmd_Err_Handler(UCHAR* Buf,ULONG cmdlen, UINT32 ret, UCHAR sid)
{
    UCHAR out_data[7] = {0};
    UCHAR checksum =0;
    int i = 0;

    out_data[0] = 0x90 + sid;
    out_data[1] = 0xB0 + ret;
    out_data[2] = *(Buf+5);
    out_data[3] = *(Buf+6);
    out_data[4] = *(Buf+7);
    out_data[5] = *(Buf+8);
    for ( i =0; i< sizeof(out_data);i++)
    {
        checksum += out_data[i];
    }

    checksum = (((checksum&0xF0)>>4) + (checksum&0x0F))& 0x0f;
    out_data[6] = 0xF0 + checksum;


    uart_TxBuf_write(&out_data[0],sizeof(out_data));
}

// ****************************************************************************
//  Global Function
// ****************************************************************************

/**
 * command observe task
 * @param i:INT32 stacd
 */
VOID IQ_Command_Observer(INT32 stacd)
{
	UINT32    GetData_Ret =0;
    int    BinErr_Ret=0;
    LONG   cmdlen = 0;
	LONG readbuf_outsize= 0;
	UCHAR bBurst_ret = 0;

    //uart_FIFO_Init();
    uart_RxTxBuf_init();    //Byran UART buffer init
    UART_CMD_INIT();    //for cmd param init
    IQCATE_GCTRL.var.IQ_MODE = 0;  //0: cmd mode, 1: raw mode.

	IQ_PRINT(D_BIN_CMD_REPLY,"--- IQ CMD start ---\n",D_ASCII_DBG_MSG);

	for (;;){

        if (RX_HaveNewData())
        {

            if (IQCATE_GCTRL.var.IQ_MODE == 0) //cmd
            {
                GetData_Ret = uart_RxBuf_GetData(gBufCommand,kCmdMaxSize,&cmdlen,0);

                switch (GetData_Ret)
                {
                    case 1: // complete ASCII
                        if ( ((gBufCommand[cmdlen -2]) == '\n') || ((gBufCommand[cmdlen -2]) == '\r'))
                        {   //delete \r\n
                             gBufCommand[cmdlen -2] = '\0';
                             gBufCommand[cmdlen -1] = '\0';
                             cmdlen -= 2;
                        }
                        else if ( ((gBufCommand[cmdlen -1]) == '\n') || ((gBufCommand[cmdlen -1]) == '\r'))
                        {   //delete \r or \n
                            gBufCommand[cmdlen -1] = '\0';
                            cmdlen -= 1;
                        }

                        gArgC = 0;
                        memcpy(&gBufCommand_Copy[0],&gBufCommand[0],cmdlen);
                        asciicmd_parse(gBufCommand_Copy, cmdlen, gArgv, &gArgC);
                        ASCIICmd_Handler(gArgv,gArgC);  //Will run Cmd_Handler in this function.
                        memset(&gBufCommand,0,sizeof(gBufCommand)); // clr the Cmd buf
                        break;
                    case 2:

                        break;
                    case 3:
                        memcpy(&gBufCommand_Copy[0],&gBufCommand[0],cmdlen);
                        BinErr_Ret =bincmd_parse(gBufCommand_Copy, cmdlen, &gBinCmd, &gBinData[0], &gBinDataSize, &gBinSID, kBinDataSize);
                        //IQ_PRINTLN(D_ASCII_DBG_MSG,"ID: %d Datasize %d",gBinSID,gBinDataSize);
                        if (BinErr_Ret == 0)
                        {
                            Cmd_Handler(gBinCmd, &gBinData[0], gBinDataSize, gBinSID);
                        }
                        else
                        {
                            BinCmd_Err_Handler(gBufCommand_Copy,cmdlen,BinErr_Ret,gBinSID);
                            //IQ_PRINTLN(D_ASCII_DBG_MSG,"Ret %d",BinErr_Ret);
                        }
                        memset(&gBufCommand,0,sizeof(gBufCommand)); // clr the Cmd buf
                        break;
                    case 4:
                    default:
                        //IQ_PRINT(D_CMD_ERROR,"Receive IQ cmd Error... retry\n");
                        break;
                }


            }
            else if(IQCATE_GCTRL.var.IQ_MODE == 2)  // for burst mode.
            {
                uart_RxBuf_GetData(gBufCommand,kCmdMaxSize,&readbuf_outsize,1);
                bBurst_ret = WriteToBuffer(gBufCommand,readbuf_outsize);

                if (bBurst_ret == 0) // =0 should mean complete
                {
                    IQCATE_GCTRL.var.IQ_MODE = 0; // While complete, auto return to Bincmd Mode.
                    Set_Print_MsgMask_Bit(D_SYSTEM_MSG);
                    setUartMode(DEBUG_GO);
                }
            }
            else    //RAW
            {
                uart_RxBuf_GetData(gGetRawText,kRawTextSize,&readbuf_outsize,1);
                if (readbuf_outsize > 0)
                {
                    uart_TxBuf_write(gGetRawText,readbuf_outsize);
                    memset(&gGetRawText,0,sizeof(gGetRawText)); // clr the Cmd buf
                }
            }
        } else {
            OS_User_Dly_Tsk(1);
		}

	}
}
#endif
