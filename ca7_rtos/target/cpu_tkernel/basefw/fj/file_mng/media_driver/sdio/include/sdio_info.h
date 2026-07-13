/**
 * @file		sdio_info.h
 * @brief		SD Memory card SDIO function header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef	_SDIO_INFO_H_
#define	_SDIO_INFO_H_

/*----------------------------------------------------------
	Command for SDIO controler								
------------------------------------------------------------
*/
// ===============================================
//   Compile Option
// ===============================================

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

//--- Register:CMD Set Data ----
#define D_SDIO_CMD05		0x0502
#define D_SDIO_CMD52		0x341A
#define D_SDIO_CMD53		0x353A

//--- Register:TRansfer Mode Set Data ----
#define D_SDIO_CMD05_TM		0x0000
#define D_SDIO_CMD52_TM		0x0000
#define D_SDIO_CMD53_TM		0x0000

/*----------------------------------------------------------
	SD Card Command Status									
	# except bit23(COM_CRC_ERROR) because it is the CRC 	
	  check of the previous command failed					
	# except bit22(ILLEGAL_COMMAND) because it is the 		
	  previous command not legal for the card state			
------------------------------------------------------------
*/
#define	D_SDIO_CARD_ST_CMD03	(0x00002000)

/*-----  Bit order of register for SDIO  ---------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SDIO_SD_NO_RESPONSE		(0xffffffff)
#define	D_SDIO_SD_CARD_READY		(0x80000000)
#define D_SDIO_SD_NUMBER_OF_IO		(0x70000000)
#define D_SDIO_SD_MEMORY_PRESENT	(0x08000000)
#define D_SDIO_SD_RESPONSE_ERR		(0x0000CF00)


/*-----  SDIO R5 Response Flags  -----------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define D_SDIO_CRTS_DIS				(0x0000)
#define D_SDIO_CRTS_CMD				(0x1000)
#define D_SDIO_CRTS_TRN				(0x2000)
#define D_SDIO_CRTS_RFU				(0x3000)

#define D_SDIO_R5_ERROR				(0xCB00)
#define D_SDIO_R5_CRCERR			(0x8000)
#define D_SDIO_R5_ILGERR			(0x4000)
#define D_SDIO_R5_GENERR			(0x0800)
#define D_SDIO_R5_FNCERR			(0x0200)
#define D_SDIO_R5_OUTERR			(0x0100)

#define D_SDIO_R5_DATA				(0x00FF)
#define D_SDIO_CCCR_VER				(0x000F)
#define D_SDIO_CCCR_VER2			(0x0002)
#define D_SDIO_CCCR_SHS				(0x0001)
#define D_SDIO_CCCR_EHS				(0x0002)


/*-----  define Bit order for SDIO_CMD52  --------------------------------------------------------
# b31:0R/1W, b30-28:FuncNo, b27:RAW, b26:stuff, b25-9:Add, b8:stuff, b7-0:Data
# RFFF WsAA AAAA AAAA AAAA AAAs DDDD DDDD
------------------------------------------------------------------------------------------------*/
#define	D_SDIO_ADD0_RD_CMD52		(0x00000000)
#define	D_SDIO_ADD7_RD_CMD52		(0x00000e00)
#define	D_SDIO_ADD8_RD_CMD52		(0x00001000)
#define	D_SDIO_ADD13_RD_CMD52		(0x00002600)
#define	D_SDIO_ADD4_RAW_CMD52		(0x88000800)
#define	D_SDIO_RST_RAW_CMD52		(0x88000c08)
#define	D_SDIO_FNC0_ABT_CMD52		(0x80000c00)
#define	D_SDIO_FNC1_ABT_CMD52		(0x80000c01)
#define	D_SDIO_BIT4_RAW_CMD52		(0x88000e82)
#define	D_SDIO_BIT1_RAW_CMD52		(0x88000e80)
#define	D_SDIO_E4MI_RAW_CMD52		(0x88001020)
#define	D_SDIO_EHS_RAW_CMD52		(0x88002602)
#define	D_SDIO_DHS_RAW_CMD52		(0x88002600)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

#endif /* _SDIO_INFO_H_ */


