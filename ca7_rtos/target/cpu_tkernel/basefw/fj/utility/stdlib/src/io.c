/**
 * @file		io.c
 * @brief		I/O function
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include <unistd.h>
#include "os_user_custom.h"
#include "uart.h"

#ifdef D_COM_BUF_FOR_TERMINAL
#include "debug.h"
#endif	// D_COM_BUF_FOR_TERMINAL

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
#if 1
INT32 io_read(INT32 fileno, CHAR* buf, UINT32 size)
{
	int i;
	char c;
	char result;
	for ( i = 0; i < size; i++ ) {
#ifdef D_COM_BUF_FOR_TERMINAL
		if( BF_Com_Buff_Is() ) {
			result = BF_Com_Buff_Get_Char(&c);
		}
		else {
			result = BF_UART_Debug_Get_Char((CHAR*)&c);
		}
#else	// D_COM_BUF_FOR_TERMINAL
		result = BF_UART_Debug_Get_Char((CHAR*)&c);
#endif	// D_COM_BUF_FOR_TERMINAL
		
		if ( result != 0 ){
			return -1;
		}
		if ( c == (char)-1 ){
			return -1;
		}
		if ( c == '\r'){
			c = '\n';
		}
		*buf++ = c;
		if ( c == '\n'){
			break;
		}
	}
	return i;
}
#endif

INT32 io_getchar(VOID)
{
	char c;
	
	if (io_read(STDIN_FILENO, &c, 1) == -1){
		return -1;
	}
	return (int)c;
}

INT32 io_gets(CHAR* str)
{
	int c;
	int size = 0;
	int loopcnt = 0;
	
	while ((c = io_getchar()) != '\n') {
		loopcnt++;
		if( (loopcnt % 20) == 0 ) {
			OS_User_Dly_Tsk(100);
		}
		if (c == -1){
			continue;
		}
		
		if( c == 0x8){
			if(size > 0){
				--str;
				*str = 0x0;  // null
				--size;
			}
		}
		else{
			*str++ = c;
			++size;
		}
	}
	return size;
}

#if 1
INT32 io_write(INT32 fileno, CHAR* buf, UINT32 size)
{
	int i;
	for ( i = 0; i < size; i++ ) {
#ifdef D_COM_BUF_FOR_TERMINAL
		if( BF_Com_Buff_Is() ) {
			BF_Com_Buff_Put_Char(*buf++);
			if ( *buf == '\n'){
				BF_Com_Buff_Put_Char('\r');
			}
		}
		else {
			BF_UART_Debug_Put_Char(*buf++);
			if ( *buf == '\n'){
				BF_UART_Debug_Put_Char('\r');
			}
		}
#else	// D_COM_BUF_FOR_TERMINAL
		BF_UART_Debug_Put_Char(*buf++);
		if ( *buf == '\n'){
			BF_UART_Debug_Put_Char('\r');
		}
#endif	// D_COM_BUF_FOR_TERMINAL
	}
	return i;
}
#endif
