/**
 * @file		mba_buf.h
 * @brief		MBA Log buffer Definition
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MBA_BUF_H_
#define _MBA_BUF_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
MBA Log Record Max Count
@remarks Don't change it without previous notice to SocioNext.
*/
#define D_MBALOG_RECORD_MAX				144000
#define D_MBALOG_AREA_MAX				1

#if defined(CO_DEBUG_MBALOG_ON) || defined(CO_BF_MBALOG_ON)
//MBA Log Output buffer length(bytes)
#define MBALOG_OUTBUF_FLUSH_THRESH		8192
#define MBALOG_OUTBUF_MAX_LENGTH		MBALOG_OUTBUF_FLUSH_THRESH + 2048
#endif	/* CO_DEBUG_MBALOG_ON or CO_BF_MBALOG_ON */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

#endif	// _MBA_BUF_H_
