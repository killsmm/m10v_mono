/**
 * @file		command.h
 * @brief		Observe Terminal Command
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
 * max string of command
 * @attention NUM_OF_ARG x MAX_OF_ARG_STRING
 */
#define MAX_OF_CMD_LINE		(1024)

/**
 * number of arg on a command line
 */
#define NUM_OF_ARG			(30)

/**
 * max string of a arg
 */
#define MAX_OF_ARG_STRING	(MAX_OF_CMD_LINE / NUM_OF_ARG)

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

extern VOID Command_Observer(INT32 stacd);

#endif	// _COMMAND_H_
