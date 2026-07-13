/**
 * @file		mipi.h
 * @brief		MIPI-DSI Command and Image control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MIPI_H_
#define _MIPI_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_disp.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define E_IM_MIPI_CMD_ONLY				(0x05)	/**< Command Only.		*/
#define E_IM_MIPI_CMD_DATA				(0x15)	/**< Command And Data.	*/
#define E_IM_MIPI_LONGPKT				(0x39)	/**< Long Packet.		*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Bit pattern of MIFCLK. */
typedef enum {
	E_BF_MIFCLK_SEL_169_714		= 0x08,					/**< 169.714MHz	*/
	E_BF_MIFCLK_SEL_148_5		= 0x09,					/**< 148.5MHz	*/
	E_BF_MIFCLK_SEL_108			= 0x0A,					/**< 108MHZ		*/
	E_BF_MIFCLK_SEL_74_25		= 0x0B,					/**< 74.25MHZ	*/
	E_BF_MIFCLK_SEL_66			= 0x0C,					/**< 66MHZ		*/
	E_BF_MIFCLK_SEL_54			= 0x0D,					/**< 54MHZ		*/
	E_BF_MIFCLK_SEL_49_5		= 0x0E,					/**< 49.5MHZ	*/
} E_BF_MIFCLK_SEL;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern FJ_ERR_CODE fj_mipi_video_ctrl( FJ_MIPI_CTRL_VIDEO* mipi_video_ctrl );
extern FJ_ERR_CODE fj_mipi_init( UCHAR freq_ctrl );
extern FJ_ERR_CODE fj_mipi_set_mode( UCHAR mode );
extern FJ_ERR_CODE fj_mipi_command_ctrl( FJ_MIPI_CMD_INFO* mipi_command_info );

#endif
