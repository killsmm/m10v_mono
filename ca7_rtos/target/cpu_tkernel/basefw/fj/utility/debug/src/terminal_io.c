/**
 * @file		terminal_io.c
 * @brief		terminal buffer I/O function
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2017 Socionext Inc.</I></B>
 */
#ifdef D_COM_BUF_FOR_TERMINAL

#include "dd_arm.h"
#include "sdram_map_define.h"
#include "os_user_custom.h"
#include "debug.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define COM_BUF_SEND_CH		(0)
#define COM_BUF_RECEIVE_CH	(1)
#define COM_BUF_SIZE		(100 * 1024)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct {
	UINT32	write_pointer;
	UINT32	read_pointer;
	CHAR	string_buf[COM_BUF_SIZE];
} T_IERMINAL_IO_BUF;

typedef struct {
	UINT32	com_enable;
	T_IERMINAL_IO_BUF	buf[2];		// 0: RTOS->Linux,  1: Linux->RTOS
} T_TERMINAL_IO;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
#define IO_TERMINAL_IO	((volatile T_TERMINAL_IO *) SDRAM_ADR_LINUX_RTOS_COMMON_TERMINAL_IO)

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
void BF_Com_Buff_Ini(void)
{
	IO_TERMINAL_IO->com_enable = 0;
	IO_TERMINAL_IO->buf[COM_BUF_SEND_CH].write_pointer = 0;
	IO_TERMINAL_IO->buf[COM_BUF_SEND_CH].read_pointer = 0;
	IO_TERMINAL_IO->buf[COM_BUF_RECEIVE_CH].write_pointer = 0;
	IO_TERMINAL_IO->buf[COM_BUF_RECEIVE_CH].read_pointer = 0;
}

unsigned BF_Com_Buff_Is(void)
{
	return IO_TERMINAL_IO->com_enable;
}

void BF_Com_Buff_Put_Char(char send_data)
{
	int	cnt;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	UINT32	write_pointer_tmp;
	UINT32	write_pointer_tmp_1;
	
	Dd_ARM_Critical_Section_Start( critseq );
	
	write_pointer_tmp = IO_TERMINAL_IO->buf[COM_BUF_SEND_CH].write_pointer;
	write_pointer_tmp_1 = write_pointer_tmp + 1;
	if( COM_BUF_SIZE <= write_pointer_tmp_1 ) {
		write_pointer_tmp_1 = 0;
	}
	for( cnt=0; cnt<=10; cnt++ ) {
		if( IO_TERMINAL_IO->buf[COM_BUF_SEND_CH].read_pointer != write_pointer_tmp_1 ) {
			IO_TERMINAL_IO->buf[COM_BUF_SEND_CH].string_buf[write_pointer_tmp] = send_data;
			IO_TERMINAL_IO->buf[COM_BUF_SEND_CH].write_pointer = write_pointer_tmp_1;
			break;
		}
		if( ! BF_Com_Buff_Is() ) {
			break;
		}
		OS_User_Dly_Tsk(100);
	}
	
	Dd_ARM_Critical_Section_End( critseq );
}

int BF_Com_Buff_Get_Char(char* receive_data)
{
	INT32	ans = 0;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	UINT32	read_pointer_tmp;
	
	Dd_ARM_Critical_Section_Start( critseq );
	
	read_pointer_tmp = IO_TERMINAL_IO->buf[COM_BUF_RECEIVE_CH].read_pointer;
	if( IO_TERMINAL_IO->buf[COM_BUF_RECEIVE_CH].write_pointer == read_pointer_tmp ) {
		ans = -1;
	}
	else {
		*receive_data = IO_TERMINAL_IO->buf[COM_BUF_RECEIVE_CH].string_buf[read_pointer_tmp];
		read_pointer_tmp++;
		if( COM_BUF_SIZE <= read_pointer_tmp ) {
			read_pointer_tmp = 0;
		}
		IO_TERMINAL_IO->buf[COM_BUF_RECEIVE_CH].read_pointer = read_pointer_tmp;
	}
	
	Dd_ARM_Critical_Section_End( critseq );
	
	return ans;
}

#endif	// D_COM_BUF_FOR_TERMINAL
