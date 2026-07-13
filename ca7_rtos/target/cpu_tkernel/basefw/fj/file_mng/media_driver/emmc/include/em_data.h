// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_data.h
* @brief generic area data group
* @note None
* @attention None
*/
/*****************************************************************************/

#ifndef EM_DATA_H_INCLUDE
#define EM_DATAIG_H_INCLUDE

// include -------------------------------------
#include "em_config.h"
#include "em_host.h"

// definition -------------------------------------
#define FACT_EM_INIT		0x20
#define FACT_EM_RW_ACCESS	0x21
#define FACT_EM_HW_ERROR	0x22
#define FACT_EM_SWFAIL		0x2F
#define FACT_TRACE			0xFF

// enumeration -------------------------------------
typedef enum {
	FILE_EM_COMMAND 	= 0x000,	/**< [0x0100]em_command.c		*/
	FILE_EM_DATA,					/**< [0x0101]em_data.c			*/
	FILE_EM_DMA,					/**< [0x0102]em_dma.c			*/
	FILE_EM_GENERAL,				/**< [0x0103]em_general.c		*/
	FILE_EM_HOST,					/**< [0x0104]em_host.c			*/
	FILE_EM_MAIN,					/**< [0x0105]em_main.c			*/
	FILE_EM_MEMORYCARD,				/**< [0x0106]em_memorycard.c	*/
	FILE_EM_FORMAT,					/**< [0x0107]em_format.c		*/
	FILE_ID_END
}E_FILEID;

// structure -------------------------------------
typedef struct {
	unsigned char	mode;
	unsigned char	factor;
	unsigned short	fileid;
	unsigned short	lines;
	unsigned char	rsv1[2];
	unsigned long	time;
	unsigned char	rsv2[4];
	unsigned long	info[8];
} T_TRACE;

// function -------------------------------------
extern void traceinfo( E_FILEID, int );


// data -------------------------------------
extern volatile T_TRACE g_sTracedata[30] ;
extern volatile unsigned long g_sTracedata_cnt ;
extern volatile char sc_descriptor_area[ EM_CONFIG_MAX_DESCR_BUFF_SIZE * EM_CONFIG_SLOT_COUNT ];
extern volatile EM_HOST sHosts ;
extern volatile unsigned short us_gRCA ;
extern volatile EM_ACCESS_MODE e_gAccessMode ;
extern volatile EM_ACCESS_MODE e_gOperationMode ;
extern volatile EM_PARTITION_ACCESS e_gAccessPrttition;
extern volatile unsigned long e_gInterruptEnable;
extern volatile unsigned long e_gInterruptStatus;


/// @def DEBUG_TRACE
#define DEBUG_TRACE( f, info ) { \
	g_sTracedata[g_sTracedata_cnt].factor = (unsigned char)(f);\
	traceinfo( FILEID, __LINE__);}

/// @def DEBUG_TRACE_INFO(1byte)
#define DEBUG_TRACE_INFO( num, val )  \
	g_sTracedata[g_sTracedata_cnt].info[num] = (val)

#endif
