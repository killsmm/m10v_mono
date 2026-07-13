#include "libcsupport.h"

/* minimum c standard library system call definitions */
/* User should redefine all system calls according to needs */

extern char __heap_start;
extern char __heap_end;
extern int BF_UART_Debug_Get_Char(char* receive_data);
extern int BF_UART_Debug_Get_Char(char* receive_data);
extern int BF_IPCU_Get_Char(char* receive_data);
extern void BF_UART_Debug_Put_Char(char str);

#ifdef D_COM_BUF_FOR_TERMINAL
extern unsigned BF_Com_Buff_Is(void);
extern void BF_Com_Buff_Put_Char(char send_data);
extern int BF_Com_Buff_Get_Char(char* receive_data);
#endif	// D_COM_BUF_FOR_TERMINAL

//void
//_exit (int status)
//{
//}

int
_kill (int pid, int sig)
{
  return -1;
}

void *
_sbrk (ptrdiff_t s)
{
	static char*	unused_top_ptr = NULL;
	char*			pt_ret;

	if( unused_top_ptr == NULL ){
		unused_top_ptr = &__heap_start;
	}

	pt_ret = unused_top_ptr;
	if( (unused_top_ptr + s) >= &__heap_end ) {
		errno = ENOMEM;
		return (void*)-1;
	}
	unused_top_ptr += s;
	return pt_ret;
}

int __attribute__((weak))
_write (int    fd,
	char * buf,
	int    nbytes)
{
	int i;
	for ( i = 0; i < nbytes; i++ ) {
#ifdef D_COM_BUF_FOR_TERMINAL
		if( BF_Com_Buff_Is() ) {
			BF_Com_Buff_Put_Char(*buf++);
		}
		else {
			BF_UART_Debug_Put_Char(*buf++);
		}
#else	// D_COM_BUF_FOR_TERMINAL
		BF_UART_Debug_Put_Char(*buf++);
#endif	// D_COM_BUF_FOR_TERMINAL
	}
	return i;
}

int
_lseek (int file,
	int ptr,
	int dir)
{
  return 0;
}

int
_close (int file)
{
  return 0;
}

int
_isatty (int fd)
{
  return (fd <= 2) ? 1 : 0;  /* one of stdin, stdout, stderr */
}

int
_fstat (int file, struct stat * st)
{
  return 0;
}

int __attribute__((weak))
_read (int file,
       char * ptr,
       int len)
{
	int i;
	char c;
	char result;
	for ( i = 0; i < len; i++ ) {
#ifdef D_COM_BUF_FOR_TERMINAL
		if( BF_Com_Buff_Is() ) {
			result = BF_Com_Buff_Get_Char(&c);
		}
		else {
			result = BF_UART_Debug_Get_Char(&c);
		}
#else	// D_COM_BUF_FOR_TERMINAL
		result = BF_UART_Debug_Get_Char((char*)&c);
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
		*ptr++ = c;
		if ( c == '\n'){
			break;
		}
	}
	return i;
}
