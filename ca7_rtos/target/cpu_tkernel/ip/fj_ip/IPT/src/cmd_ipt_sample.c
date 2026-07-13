/**
 * @file		cmd_ipt_sample.c
 * @brief		Sample command for IP Command
 * @author		
 * @since		2016/6/9
 * @date		
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

#include <stdio.h>
#include <stdlib.h>

/*----------------------------------------------------------------------*/
/* Compile option														*/
/*----------------------------------------------------------------------*/
//#define CO_IPT_SAMPLE_ON				/**< ENABLE SAMPLE COMMOND */

#ifdef CO_IPT_SAMPLE_ON

#include "../../IPC/IPC_IF3/include/ipc_if.h"

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Data																	*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local API															*/
/*----------------------------------------------------------------------*/

static IPC_VOID cmd_ipt_sample_main( IPC_INT arg0, IPC_INT arg1, IPC_INT arg2 )
{
	switch ( arg0 )
	{
		default:
			break;
	}
}


/*----------------------------------------------------------------------*/
/* External API															*/
/*----------------------------------------------------------------------*/

void cmd_ipt_sample_help( void )
{
	printf("USAGE>sample : help\n"); 
	printf("USAGE>sample 0 0: test0-0\n"); 
	printf("USAGE>sample 0 1: test0-1\n"); 
}

void cmd_ipt_sample( int argc, char** argv )
{
	IPC_INT arg[3];
	
	arg[0] = atoi(argv[1]);
	arg[1] = atoi(argv[2]);
	arg[2] = atoi(argv[3]);
	
	cmd_ipt_sample_main( arg[0], arg[1], arg[2] );
}

#else	// CO_IPT_SAMPLE_ON

// Dummy APIs.
void cmd_ipt_sample_help( void ) {}
void cmd_ipt_sample( int argc, char** argv ) {}

#endif
