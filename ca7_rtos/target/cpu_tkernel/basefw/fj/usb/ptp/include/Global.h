/**
 * @file		Global.h
 * @brief		Declare for Sofutune Config
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _MILENNIA_GROBAL
#define _MILENNIA_GROBAL
#include "media_state.h"
#define FPTN_CF_PULLED  0x00000001
#define FPTN_CF_INSERT 0x00000002
#define FPTN_CF_DMA_END 0x00000004
#define FPTN_CF_DMA_ERR 0x00000008

/*FID_PTP*/
#define FLG_DPSPRT2  0x00000001
#define FLG_DPSDSC   0x00000002
#define FLG_DPSEVENT 0x00000004
#define FLG_DPSDSC_STORE_ADDED   0x00000008
#define FLG_DPSDSC_STORE_REMOVED   0x00000010


// Media Type
//typedef enum{
//	eMEDIA_TYPE_RAM  = 0,	/* RAM Drive */
//	eMEDIA_TYPE_SDI = 1,	/* SD IO Card */
//	eMEDIA_TYPE_SM  = 2,	/* SmartMedia */
//} eMEDIA_TYPE;


// DPS
#define max_marked_file	128
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_TYPE	srcMedia;
	E_BF_MEDIA_STATE_MEDIA_TYPE	dstMedia;
	SHORT		file_cnt;	/* Modified variable type from CHAR to SHORT by Andy Jing 070306 */		// at most 32767 files 
	SHORT		dir_no[max_marked_file];
	SHORT		file_no[max_marked_file];
	CHAR		isJPG[max_marked_file];
	CHAR        mark[max_marked_file];
	LONG		FATData[max_marked_file];
	CHAR		copies[max_marked_file];
	SHORT		Media_Status[max_marked_file];
} Marked_File;

#endif
