/**
 * @file		cmd_ipt_entry.c
 * @brief		IP Command set
 * @author		
 * @since		2016/6/9
 * @date		
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT Inc. 2016</I></B>
 */

#include "command_entry.h"

/**
 * command entry
 */
extern void cmd_ipt_sample( int argc, char** argv );
extern void cmd_ipt_sample_help( void );

/**
 * command entry table
 */
const T_CMD_ENTRY gCmd_IP_Entry_Table[] = {
	{ "sample", { cmd_ipt_sample, cmd_ipt_sample_help } },
	{ NULL,{ NULL, NULL} }
};
