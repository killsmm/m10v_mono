#ifndef __LIBC_SUPPORT_H__
#define __LIBC_SUPPORT_H__

#if !defined(_in_asm_source_)

#include <stdio.h>
#include <errno.h>
#include <reent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


int     _system   _PARAMS ((const char *));
int     _rename   _PARAMS ((const char *, const char *));
int     _isatty		_PARAMS ((int));
clock_t _times		_PARAMS ((struct tms *));
int     _gettimeofday	_PARAMS ((struct timeval *, void *));
void    _raise 		_PARAMS ((void));
int     _unlink		_PARAMS ((const char *));
int     _link 		_PARAMS ((void));
int     _stat 		_PARAMS ((const char *, struct stat *));
int     _fstat 		_PARAMS ((int, struct stat *));
void*   _sbrk     _PARAMS ((ptrdiff_t));
int     _getpid		_PARAMS ((int));
int     _kill		  _PARAMS ((int, int));
void    _exit		  _PARAMS ((int));
int     _close		_PARAMS ((int));
int     _open		  _PARAMS ((const char *, int, ...));
int     _write 		_PARAMS ((int, char *, int));
int     _lseek		_PARAMS ((int, int, int));
int     _read		  _PARAMS ((int, char *, int));

#endif /* _in_asm_source_ */
#endif /* __LIBC_SUPPORT_H__ */
