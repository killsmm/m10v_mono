/**
 * @file		exception.c
 * @brief		Exception Handler
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "fj_emergency_log.h"
#include "fj_media.h"
#include "os_user_custom.h"
#include "assertion.h"
#include "exception.h"
#include "uart.h"
#include "sdram_map_common.h"
#include "dd_uart.h"
#include "im_mxic.h"
#include "mwnf.h"

extern volatile UCHAR gBF_UART_Debug_Send_Buf[D_UART_DEBUG_SEND_BUFF_SIZE];
#ifdef CO_ASSERT_LOG_ON
extern volatile int gAssertion_tbl_idx;
extern T_ASSERTION_DATA gAssertion_tbl[ASSERTION_LOG_TBL_SIZE];
#endif
#ifdef CO_FAST_BOOT_ON
extern UINT32 gBF_Boot_Loader_Area;
#endif

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define ABORT_STACK_SIZE			(0x2500)
#define EXCEPTION_LOG_MAX_TSK		(100)
#define EXCEPTION_LOG_MAX_SEM		(300)
#define EXCEPTION_LOG_MAX_FLG		(500)
#define EXCEPTION_LOG_MAX_MBX		(50)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct {
	UINT32	TMIF0;
	UINT32	TDEADW0;
	UINT32	TDEMNW0;
	UINT32	TDEADR0;
	UINT32	TDEMNR0;
	UINT32	TMIF1;
	UINT32	TDEADW1;
	UINT32	TDEMNW1;
	UINT32	TDEADR1;
	UINT32	TDEMNR1;
	UINT32	TMIF2;
	UINT32	TDEADW2;
	UINT32	TDEMNW2;
	UINT32	TDEADR2;
	UINT32	TDEMNR2;
	UINT32	TMIF3;
	UINT32	TDEADW3;
	UINT32	TDEMNW3;
	UINT32	TDEADR3;
	UINT32	TDEMNR3;
	UINT32	TMIF4;
	UINT32	TDEADW4;
	UINT32	TDEMNW4;
	UINT32	TDEADR4;
	UINT32	TDEMNR4;
	UINT32	TMIF5;
	UINT32	TDEADW5;
	UINT32	TDEMNW5;
	UINT32	TDEADR5;
	UINT32	TDEMNR5;
	UINT32	TMIF6;
	UINT32	TDEADW6;
	UINT32	TDEMNW6;
	UINT32	TDEADR6;
	UINT32	TDEMNR6;
} T_EXCEPTION_REG;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
#ifdef __GNUC__
unsigned char	gAbort_stack[ABORT_STACK_SIZE] __attribute__((aligned(8)));
#else
__align(8) unsigned char	gAbort_stack[ABORT_STACK_SIZE];
#endif

int gException_Cnt = 1;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static VOID exception_log_out( VOID )
{
	OS_USER_ER				sys_ercd;
	T_OS_USER_RCFG			rcfg;
	T_OS_USER_RTSK			rtsk1[EXCEPTION_LOG_MAX_TSK];	// task status
	T_OS_USER_RTSK			rtsk2[EXCEPTION_LOG_MAX_TSK];	// task status
	T_OS_USER_RTSK			rtsk3[EXCEPTION_LOG_MAX_TSK];	// task status
	T_OS_USER_RSEM			rsem[EXCEPTION_LOG_MAX_SEM];	// semaphore status
	T_OS_USER_RFLG			rflg[EXCEPTION_LOG_MAX_FLG];	// event flag status
	T_OS_USER_RMBX			rmbx[EXCEPTION_LOG_MAX_MBX];	// mailbox status
	INT32					headerInfo;
#ifdef UART_CTRL_CH
	ULONG					terminal_write_pointer;
#endif

#ifdef CO_FAST_BOOT_ON
	// Fast boot separated loading area
	// write NF   UINT32 gBF_Boot_Loader_Area (label=0x0B)
	headerInfo = (0x0B << 24) | sizeof(gBF_Boot_Loader_Area);
	FJ_EmergencyLog_Store_Memory( (ULONG)(&gBF_Boot_Loader_Area), headerInfo );
#endif

#ifdef CO_ASSERT_LOG_ON
	// Assertion Log
	// write NF   int gAssertion_tbl_idx (label=0x03)
	if( gAssertion_tbl_idx < ASSERTION_LOG_TBL_SIZE ){
		headerInfo = (0x03 << 24) | sizeof(gAssertion_tbl_idx);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&gAssertion_tbl_idx), headerInfo );
		// write NF   T_ASSERTION_DATA gAssertion_tbl[ASSERTION_LOG_TBL_SIZE] (label=0x04)
		headerInfo = (0x04 << 24) | sizeof(gAssertion_tbl);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&gAssertion_tbl), headerInfo );
	}
#endif

#ifdef UART_CTRL_CH
	// Debug Terminal Log
	// write NF   ULONG terminal_write_pointer (label=0x01)
	terminal_write_pointer = Dd_UART_Get_Send_Write_Pointer_Addr( UART_CTRL_CH );
	headerInfo = (0x01 << 24) | sizeof(terminal_write_pointer);
	FJ_EmergencyLog_Store_Memory( (ULONG)(&terminal_write_pointer), headerInfo );
	// write NF   UCHAR gBF_UART_Debug_Send_Buf[32768] (label=0x02)
	headerInfo = (0x02 << 24) | sizeof(gBF_UART_Debug_Send_Buf);
	FJ_EmergencyLog_Store_Memory( (ULONG)gBF_UART_Debug_Send_Buf, headerInfo );
#endif

	// RTOS status
	sys_ercd = OS_User_Ref_Cfg( &rcfg );
	if( sys_ercd == D_OS_USER_E_OK ) {
		OS_USER_ID id_max;
		OS_USER_ID id;

		// configuration (label=0x21)
		headerInfo = (0x21 << 24) | sizeof(rcfg);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rcfg), headerInfo );

		// task (label=0x22)
		memset(rtsk1, 0, sizeof(rtsk1));
		for( id = (OS_USER_ID)1; id <= 100; id++ ) {
			OS_User_Ref_Tsk( (0x01000000 | id), &rtsk1[id-1] );
		}
		headerInfo = (0x22 << 24) | sizeof(rtsk1);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rtsk1), headerInfo );

		memset(rtsk2, 0, sizeof(rtsk2));
		for( id = (OS_USER_ID)101; id <= 200; id++ ) {
			OS_User_Ref_Tsk( (0x01000000 | id), &rtsk2[id-101] );
		}
		headerInfo = (0x22 << 24) | sizeof(rtsk2);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rtsk2), headerInfo );

		memset(rtsk3, 0, sizeof(rtsk3));
		id_max = (rcfg.max_tskid >= 300) ? EXCEPTION_LOG_MAX_TSK*3 : rcfg.max_tskid;
		for( id = (OS_USER_ID)201; id <= id_max; id++ ) {
			OS_User_Ref_Tsk( (0x01000000 | id), &rtsk3[id-201] );
		}
		headerInfo = (0x22 << 24) | sizeof(rtsk3);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rtsk3), headerInfo );

		// semaphore (label=0x23)
		id_max = (rcfg.max_semid >= EXCEPTION_LOG_MAX_SEM) ? (EXCEPTION_LOG_MAX_SEM-1) : rcfg.max_semid;
		memset(rsem, 0, sizeof(rsem));
		for( id = (OS_USER_ID)1; id <= id_max; id++ ) {
			OS_User_Ref_Sem( (0x01000000 | id), &rsem[id-1] );
		}
		headerInfo = (0x23 << 24) | sizeof(rsem);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rsem), headerInfo );

		// event flag (label=0x24)
		id_max = (rcfg.max_flgid >= EXCEPTION_LOG_MAX_FLG) ? (EXCEPTION_LOG_MAX_FLG-1) : rcfg.max_flgid;
		memset(rflg, 0, sizeof(rflg));
		for( id = (OS_USER_ID)1; id <= id_max; id++ ) {
			OS_User_Ref_Flg( (0x01000000 | id), &rflg[id-1] );
		}
		headerInfo = (0x24 << 24) | sizeof(rflg);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rflg), headerInfo );

		// mailbox (label=0x25)
		id_max = (rcfg.max_mbxid >= EXCEPTION_LOG_MAX_MBX) ? (EXCEPTION_LOG_MAX_MBX-1) : rcfg.max_mbxid;
		memset(rmbx, 0, sizeof(rmbx));
		for( id = (OS_USER_ID)1; id <= id_max; id++ ) {
			OS_User_Ref_Mbx( (0x01000000 | id), &rmbx[id-1] );
		}
		headerInfo = (0x25 << 24) | sizeof(rmbx);
		FJ_EmergencyLog_Store_Memory( (ULONG)(&rmbx), headerInfo );
	}

	// forever
//	for(;;) {
//		;
//	}
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID Exception_Handler( unsigned stack_pointer )
{
	INT32 headerInfo;
	T_EXCEPTION_REG exception_log;
	ULONG exception_log_top;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
		// eMMC
		FJ_EMMC_Get_ExceptionLog_Info((UINT32*)&exception_log_top);
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NAND Flash
		FJ_NF_Get_ExceptionLog_Info((UINT32*)&exception_log_top);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	FJ_EmergencyLog_Set_Log_Start_Sector( exception_log_top );

	// general registers and program status registers (label=0x11)
	// 0    +0 : sp (user stack pointer)
	// 1    +4 : lr (user return address)
	// 2    +8 : DFSR (Data Fault Status Register)
	// 3   +12 : IFSR (Instruction Fault Status Register)
	// 4   +16 : DFAR (Data Fault Address Register)
	// 5   +20 : IFAR (Instruction Fault Address Register)
	// 6   +24 : spsr (saved program status register)
	// 7   +28 : cpsr (current program status register)
	// 8   +32 : CAUSE CODE (0x4:Undefined Instruction / 0xC:Prefetch Abort / 0x10:Data Abort)
	// 9   +36 : cpsr (current program status register)
	// 10  +40 : r2
	// 11  +44 : r3
	// 12  +48 : r4
	// 13  +52 : r5
	// 14  +56 : r6
	// 15  +60 : r7
	// 16  +64 : r8
	// 17  +68 : r9
	// 18  +72 : r10
	// 19  +76 : r11
	// 20  +80 : r12
	// 21  +84 : lr (Abort state)
	// 22  +88 : r0
	// 23  +92 : r1
	// write NF
	headerInfo = (0x11 << 24) | 128;
	FJ_EmergencyLog_Store_Memory( (ULONG)stack_pointer, headerInfo );

	// call stack (label=0x12)
	//   start address : r0 register (= callstack)
	// write NF
	headerInfo = (0x12 << 24) | 512;
	FJ_EmergencyLog_Store_Memory( *((ULONG*)stack_pointer), headerInfo );

	// MXIC registers (label=0x13)
	// write NF
	exception_log.TMIF0  = Im_MXIC_Get_TMIF0;
	exception_log.TDEADW0 = Im_MXIC_Get_TDEADW0;
	exception_log.TDEMNW0 = Im_MXIC_Get_TDEMNW0;
	exception_log.TDEADR0 = Im_MXIC_Get_TDEADR0;
	exception_log.TDEMNR0 = Im_MXIC_Get_TDEMNR0;
	exception_log.TMIF1  = Im_MXIC_Get_TMIF1;
	exception_log.TDEADW1 = Im_MXIC_Get_TDEADW1;
	exception_log.TDEMNW1 = Im_MXIC_Get_TDEMNW1;
	exception_log.TDEADR1 = Im_MXIC_Get_TDEADR1;
	exception_log.TDEMNR1 = Im_MXIC_Get_TDEMNR1;
	exception_log.TMIF2  = Im_MXIC_Get_TMIF2;
	exception_log.TDEADW2 = Im_MXIC_Get_TDEADW2;
	exception_log.TDEMNW2 = Im_MXIC_Get_TDEMNW2;
	exception_log.TDEADR2 = Im_MXIC_Get_TDEADR2;
	exception_log.TDEMNR2 = Im_MXIC_Get_TDEMNR2;
	exception_log.TMIF3  = Im_MXIC_Get_TMIF3;
	exception_log.TDEADW3 = Im_MXIC_Get_TDEADW3;
	exception_log.TDEMNW3 = Im_MXIC_Get_TDEMNW3;
	exception_log.TDEADR3 = Im_MXIC_Get_TDEADR3;
	exception_log.TDEMNR3 = Im_MXIC_Get_TDEMNR3;
	exception_log.TMIF4  = Im_MXIC_Get_TMIF4;
	exception_log.TDEADW4 = Im_MXIC_Get_TDEADW4;
	exception_log.TDEMNW4 = Im_MXIC_Get_TDEMNW4;
	exception_log.TDEADR4 = Im_MXIC_Get_TDEADR4;
	exception_log.TDEMNR4 = Im_MXIC_Get_TDEMNR4;
	exception_log.TMIF5  = Im_MXIC_Get_TMIF5;
	exception_log.TDEADW5 = Im_MXIC_Get_TDEADW5;
	exception_log.TDEMNW5 = Im_MXIC_Get_TDEMNW5;
	exception_log.TDEADR5 = Im_MXIC_Get_TDEADR5;
	exception_log.TDEMNR5 = Im_MXIC_Get_TDEMNR5;
	exception_log.TMIF6  = Im_MXIC_Get_TMIF6;
	exception_log.TDEADW6 = Im_MXIC_Get_TDEADW6;
	exception_log.TDEMNW6 = Im_MXIC_Get_TDEMNW6;
	exception_log.TDEADR6 = Im_MXIC_Get_TDEADR6;
	exception_log.TDEMNR6 = Im_MXIC_Get_TDEMNR6;
	headerInfo = (0x13 << 24) | sizeof(exception_log);
	FJ_EmergencyLog_Store_Memory( (ULONG)(&exception_log), headerInfo );

	// Others
	exception_log_out();
}

VOID Watchdog_Handler( VOID )
{
	ULONG exception_log_top;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x7);

	if (mdr == 1) {
		// eMMC
		FJ_EMMC_Get_ExceptionLog_Info((UINT32*)&exception_log_top);
	}
	else {
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		// NAND Flash
		FJ_NF_Get_ExceptionLog_Info((UINT32*)&exception_log_top);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	}
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	FJ_EmergencyLog_Set_Log_Start_Sector( exception_log_top );

	// Others
	exception_log_out();
}
