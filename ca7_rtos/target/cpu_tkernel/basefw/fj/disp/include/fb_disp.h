/**
 * @file		fb_disp.h
 * @brief		Set Disp's GR by request from IPCU
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2017 Socionext Inc.</I></B>
 */

#ifndef FB_DISP_H
#define FB_DISP_H

#include "fs_user_custom.h"
#include "sdram_map_define.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_MAX_COMMAND_NUM	(20)
#define D_MAX_COMMAND_WORD	(15)
#define D_MAX_COMMAND_BUFFER_SIZE (120)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{
	eGRTISIZE=1,
	eGRTDSTA,
	eGRSCCTL,
	eGRRSZ,
	eGRISIZE,
	eGRDSTA,
	eGRHGA,
	eGRSA0,
	eGRSA,
	eGRAREN,
	eGRTRG,
	eGRIDT,
	eGRRST,
	eGRIPO,
}e_command_code;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	unsigned int sequence_no;
	unsigned int data;
} t_ipcu_mail_box;

typedef union {
	unsigned int word;
	struct {
        unsigned int   command_num	:8;
        unsigned int   block_no		:8;
        unsigned int   gr_no		:8;
        unsigned int   reserve		:8;
	}bit;
}u_ipcu_command_head;

typedef union {
	unsigned int word;
	struct {
        unsigned int   size			:8;/* word's num */
        unsigned int   command_code	:8;
        unsigned int   reserve		:16;
	}bit;
}u_command_head;

typedef struct {
	u_command_head command_head;
	unsigned int command[D_MAX_COMMAND_WORD];  
} t_request;

typedef struct {
	u_ipcu_command_head head;
	unsigned int command_offset[D_MAX_COMMAND_NUM];
	unsigned int buffer[D_MAX_COMMAND_BUFFER_SIZE];
}t_ipcu_command;
	
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
VOID IPCU_request_OSD_cb( UCHAR id, UCHAR *data, UINT32 length, UINT32 cont, UINT32 total_length );
VOID IPCU_Set_OSD_config(INT32 stacd);

#endif//FB_DISP_H

