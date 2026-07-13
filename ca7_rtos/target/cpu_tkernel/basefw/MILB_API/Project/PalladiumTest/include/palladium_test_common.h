/**
 * @file		palladium_test_common.h
 * @brief		Test DD/IM by palladium define
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _PALLADIUM_TEST_CMD_H_
#define _PALLADIUM_TEST_CMD_H_
#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PALLADIUM_TEST_CMD_PARA_NUM				(20)
#define D_PALLADIUM_TEST_CMD_CHAR_NUM				(20)
#define D_PALLADIUM_TEST_CMD_NUM					(20)
typedef VOID (*ct_Main)(INT32 argc, CHAR** argv);	//CT commande main

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

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
extern int Palladium_Test_Cmd_Call_Main(INT32 argc, CHAR argv[][D_PALLADIUM_TEST_CMD_CHAR_NUM], ct_Main ct_command_main);
extern int Palladium_Test_Cmd_Dump_Reg(ULONG reg_address, ULONG size );

#endif
