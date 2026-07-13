/**
 * @file		command_entry.h
 * @brief		Command Entry Control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _COMMAND_ENTRY_H_
#define _COMMAND_ENTRY_H_

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/**
 * command entry structure
 */
typedef struct {
	VOID (*cmd_entry)(INT32 argc, char** argv);
	VOID (*help_entry)(VOID);
} T_CMD_FUNC;

/**
 * command entry structure
 */
typedef struct {
	char* cmd_string;
	T_CMD_FUNC func;
} T_CMD_ENTRY;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* declare of CMD Entry Table */
// delete_basefw\fj\command
//extern const T_CMD_ENTRY gCmd_CT_Entry_Table[];
extern const T_CMD_ENTRY gCmd_IP_Entry_Table[];
extern const T_CMD_ENTRY gCmd_MP_Entry_Table[];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

#endif	// _COMMAND_ENTRY_H_
