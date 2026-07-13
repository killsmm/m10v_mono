#include "libcsupport.h"

/* NOTE: This file is intended as example, please use your own definitions */

/* minimum reentrant system call definitions. These functions only provided as wrap to
 * existing system call definitions in newlib_syscalls.c, it is up to the user
 * to define the reentrant version of the system calls according to need 
 */

void * _sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
  char *ret;
  void *_sbrk(ptrdiff_t);

  errno = 0;
  if ((ret = (char *)(_sbrk (incr))) == (void *) -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

int
_isatty_r (ptr, fd)
     struct _reent *ptr;
     int fd;
{
  int ret;

  errno = 0;
  if ((ret = _isatty (fd)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

_ssize_t _read_r(struct _reent *ptr, int fd, _PTR buf, size_t cnt)
{
  _ssize_t ret;

  errno = 0;
  if ((ret = (_ssize_t)_read (fd, buf, cnt)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
  _off_t ret;

  errno = 0;
  if ((ret = _lseek (fd, pos, whence)) == (_off_t) -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

_ssize_t _write_r(struct _reent *ptr, int fd, _CONST _PTR buf, size_t cnt)
{
  _ssize_t ret;

  errno = 0;
  if ((ret = (_ssize_t)_write (fd, (char *)buf, cnt)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

int _close_r(struct _reent *ptr, int fd)
{
  int ret;

  errno = 0;
  if ((ret = _close (fd)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
  int ret;

  errno = 0;
  if ((ret = _fstat (fd, pstat)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}
