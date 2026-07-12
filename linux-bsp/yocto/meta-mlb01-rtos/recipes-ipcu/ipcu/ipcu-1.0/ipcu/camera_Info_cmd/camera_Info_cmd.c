/******************************************************************************/
/**
 *  @file   camera_Info_cmd.c
 *  @brief  test Camera information notify Command
 */
/*
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "cmfwk_ipcu.h"
#include "../include/cmfwk_std.h"
#define D_DEBUG_camera_Info_cmd
#define COMMAND_SET (0x00010100)
#define SUB_COMAND_SETGPS   (0x00000100)
#define SUB_COMAND_SETDATE   (0x00000200)
#define IPCU_CH_ID  (5)

typedef enum {
	E_COMMAND_TYPE_SetGPS = 1,		// Set GPS
	E_COMMAND_TYPE_SetDatetime,		// Set Date time
	E_COMMAND_TYPE_OTHER			// End flag
} E_COMMAND_TYPE;

const char* help_msg[]=
{
	"Usage:",
	"  camera_Info_cmd sub-command P1 P2 P3 P4 P5 P6 P7 P8 P9 P10 P11 P12 P13",
	" ",
	"sub-command:",
	"  =SetGPS      : Send set GPS comannd to RTOS",
	"  =SetDatetime : Send set date-time to RTOS",
	"When SetGPS was set:",
	" P1 : xy",
	    "x: GPSLatitudeRef  N or S (ASCII：'N'= 0x4E or 'S'= 0x53)",
	    "y: GPSLongitudeRe E or W (ASCII：'E'= 0x45 or 'W'= 0x57)",
	" P2 :Latitude (RATIONAL numerator)",
	" P3 :Latitude (RATIONAL denominator)",
	" P4 :Latitude (RATIONAL numerator)",
	" P5 :Latitude (RATIONAL denominator)",
	" P6 :Latitude (RATIONAL numerator)",
	" P7 :Latitude (RATIONAL denominator)",
	" P8 :Longitude (RATIONAL numerator)",
	" P9 :Longitude (RATIONAL denominator)",
	" P10:Longitude (RATIONAL numerator)",
	" P11:Longitude (RATIONAL denominator)",
	" P12:Longitude (RATIONAL numerator)",
	" P13:Longitude (RATIONAL denominator)",
	"When SetDatetime was set:",
	" P1 : Year   ( 2001 to 2099 )",
    " P2 : Month ( 01 to 12)",
	" P3 : Day     ( 01 to 28/29/30/31)",
	" P4 : Day of the week ( 00 to 06 ) . 00=Sunday",				
	" P5 : Hour    ( 00 to 23 )",
	" P6 : Minute  ( 00 to 59 )",
	" P7 : Second  ( 0 to 59 )",
	NULL,
};

/******************************************************************************/
/**
 *  @brief  Check parameter
 *  @fn int Check parameter( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
static int CheckParameter(int argc, char *argv[])
{
    int ret = 0;
    int loop;
	if (argc == 1) {
		ret = -1;
	} else {
		if(strcmp(argv[1],"SetGPS")==0) {
//printf("%d %s\n",__LINE__,__func__);
            if(argc != 15) {
        		ret = -2;
            }
            else{
                ret = E_COMMAND_TYPE_SetGPS;
            }
        }
        else if(strcmp(argv[1],"SetDatetime")==0) {
//printf("%d %s\n",__LINE__,__func__);
            if(argc != 9) {
        		ret = -2;
            }
            else{
                ret = E_COMMAND_TYPE_SetDatetime;
            }
        }
        else{
            ret = -3;
        }
    }
    if ( ret <=0 ) {
		for(loop = 0; help_msg[loop] != NULL; loop++) {
			printf("%s\n",help_msg[loop]);
		}

    }
    return ret;    
}
/******************************************************************************/
/**
 *  @fn int GetSendData( int CommandType, char* parameterStr[], char* SendBuffer )
 *  @param  [in]    CommandType      : Command type
 *  @param  [in]    parameterStr     : User parameters
 *  @param  [Out]    SendBuffer     : Send data from parse user parameters
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
static int GetSendData(int CommandType, char* parameterStr[], T_CPUCMD_NOTIFY_INFO* SendBuffer)
{
    int loop_num,i;
    ULONG* notify;
//printf("%d %s\n",__LINE__,__func__);
    memset((char*)(SendBuffer),0,sizeof(T_CPUCMD_NOTIFY_INFO));
    SendBuffer->t_head.magic_code = D_CPU_IF_MCODE_INFO_NOTIFY;
    SendBuffer->t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
    SendBuffer->t_head.cmd_set = COMMAND_SET;
	if(CommandType == E_COMMAND_TYPE_SetGPS ) {
        notify = &(SendBuffer->notify2);
        loop_num = 2 + 13;
        SendBuffer->t_head.cmd = SUB_COMAND_SETGPS;
        SendBuffer->notify1 = parameterStr[2][0];//Set N or S
        SendBuffer->notify1 <<= 8;
        SendBuffer->notify1 |= parameterStr[2][1];//Set E or W
        for(i = 3; i<loop_num; i++) {
            notify[i-3] = atoi(parameterStr[i]);
        }
    }
    else {
        notify = &(SendBuffer->notify1);
        loop_num = 2 + 7;
        SendBuffer->t_head.cmd = SUB_COMAND_SETDATE;
        *notify = atoi(parameterStr[2]);//Set Year
        (*notify) <<= 16;
        (*notify) |= (atoi(parameterStr[3]) <<8);//Set Month
        (*notify) |= (atoi(parameterStr[4]) );//Set Date
        notify++;
        (*notify) = (atoi(parameterStr[6]) );//Set hour
        (*notify) <<= 24;
        (*notify) |= (atoi(parameterStr[7]) <<16);//Set minute
        (*notify) |= (atoi(parameterStr[8]) <<8);//Set Second
        (*notify) |= (atoi(parameterStr[5]) );//Set week
    }
    return 0;
}
static void PrinftSendBuffer(T_CPUCMD_NOTIFY_INFO* SendBuffer)
{
#ifdef D_DEBUG_camera_Info_cmd
    ULONG* notify=(ULONG*)SendBuffer;
    int i;
    for( i=0;i<sizeof(T_CPUCMD_NOTIFY_INFO)/4;i++) {
        printf("0X%08x\n", notify[i]);   
    }
#endif
}

/******************************************************************************/
/**
 *  @brief  sample_app
 *  @fn int Check parameter( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	INT32  ret;
	UINT8  ch_id;
	T_CPUCMD_NOTIFY_INFO SendBuffer;
	
	ret = CheckParameter(argc, argv);
//printf("%d %s\n",__LINE__,__func__);
	switch(ret) {
    case E_COMMAND_TYPE_SetGPS:
    case E_COMMAND_TYPE_SetDatetime:
        GetSendData(ret,argv,&SendBuffer);
//         PrinftSendBuffer(&SendBuffer);
 	    ret = FJ_IPCU_Open(IPCU_CH_ID, &ch_id);
	    if (ret == FJ_ERR_OK) {
		    FJ_IPCU_Send(ch_id, (UINT32)(&SendBuffer), sizeof(SendBuffer), IPCU_SEND_SYNC);
	
        	FJ_IPCU_Close(ch_id);
            PrinftSendBuffer(&SendBuffer);
        }
        else {
            printf("IPCU error\n");
        }
        break;
    default:
        break;
    }
	return 0;
}
