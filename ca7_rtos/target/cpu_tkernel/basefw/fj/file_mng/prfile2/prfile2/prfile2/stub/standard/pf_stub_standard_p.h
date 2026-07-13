/*
     Copyright (C) 2006 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_stub_standard_p.h] - PrFILE2 standard API Stub functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_STUB_STANDARD_P_H__
#define __PF_STUB_STANDARD_P_H__

#include "prfile2/pf_config.h"

#if PF_USE_API_STUB

#include "pf_stub.h"

/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
  PrFILE2 API function number
 *---------------------------------------------------------------------*/
#define CRTFIL      (PFS_BASE_NUMBER_STANDARD + 0)      /* pf_create()      */
#define OPNFIL      (PFS_BASE_NUMBER_STANDARD + 1)      /* pf_fopen()       */
#define CLSFIL      (PFS_BASE_NUMBER_STANDARD + 2)      /* pf_fclose()      */
#define RADFIL      (PFS_BASE_NUMBER_STANDARD + 3)      /* pf_fread()       */
#define WRTFIL      (PFS_BASE_NUMBER_STANDARD + 4)      /* pf_fwrite()      */
#define SEKFIL      (PFS_BASE_NUMBER_STANDARD + 5)      /* pf_fseek()       */
#define SRCFIL      (PFS_BASE_NUMBER_STANDARD + 6)      /* (no equivalent)  */
#define ERSFIL      (PFS_BASE_NUMBER_STANDARD + 7)      /* pf_remove()      */
#define RENFIL      (PFS_BASE_NUMBER_STANDARD + 8)      /* pf_rename()      */
#define MAKDIR      (PFS_BASE_NUMBER_STANDARD + 9)      /* pf_mkdir()       */
#define RMVDIR      (PFS_BASE_NUMBER_STANDARD + 10)     /* pf_rmdir()       */
#define CHGDIR      (PFS_BASE_NUMBER_STANDARD + 11)     /* pf_chdir()       */
#define GETSTS      (PFS_BASE_NUMBER_STANDARD + 12)     /* pf_fstat()       */
#define CHGMOD      (PFS_BASE_NUMBER_STANDARD + 13)     /* pf_chmod()       */
#define MNTHDD      (PFS_BASE_NUMBER_STANDARD + 14)     /* pf_attach()/pf_mount() */
#define FMTHDD      (PFS_BASE_NUMBER_STANDARD + 15)     /* pf_format()      */
#define BUFRNG      (PFS_BASE_NUMBER_STANDARD + 16)     /* pf_buffering()   */
#define ERRFIL      (PFS_BASE_NUMBER_STANDARD + 17)     /* pf_error()       */
#define EOFFIL      (PFS_BASE_NUMBER_STANDARD + 18)     /* pf_eof()         */
#define GETERR      (PFS_BASE_NUMBER_STANDARD + 19)     /* pf_errnum()      */
#define DEVINF      (PFS_BASE_NUMBER_STANDARD + 20)     /* pf_devinf()      */
#define SETVOL      (PFS_BASE_NUMBER_STANDARD + 21)     /* pf_setvol()      */
#define GETVOL      (PFS_BASE_NUMBER_STANDARD + 22)     /* pf_getvol()      */
#define RMVVOL      (PFS_BASE_NUMBER_STANDARD + 23)     /* pf_rmvvol()      */
#define CHDMOD      (PFS_BASE_NUMBER_STANDARD + 24)     /* pf_chdmod()      */
#define CONCAT      (PFS_BASE_NUMBER_STANDARD + 25)     /* pf_fconcat()     */
#define PFINIT      (PFS_BASE_NUMBER_STANDARD + 26)     /* pf_init_prfile2()*/
#define ATCHDD      (PFS_BASE_NUMBER_STANDARD + 27)     /* pf_attach()      */
#define FSFFIL      (PFS_BASE_NUMBER_STANDARD + 28)     /* pf_fsfirst()     */
#define FSNFIL      (PFS_BASE_NUMBER_STANDARD + 29)     /* pf_fsnext()      */
#define SUPFSI      (PFS_BASE_NUMBER_STANDARD + 30)     /* pf_setupfsi()    */
#define SETCLK      (PFS_BASE_NUMBER_STANDARD + 31)     /* pf_setclstlink() */
#define FSTCLK      (PFS_BASE_NUMBER_STANDARD + 32)     /* pf_fsetclstlink()*/
#define SYNCCH      (PFS_BASE_NUMBER_STANDARD + 33)     /* pf_sync()        */
#define FSYNCC      (PFS_BASE_NUMBER_STANDARD + 34)     /* pf_fsync()       */
#define FAPPND      (PFS_BASE_NUMBER_STANDARD + 35)     /* pf_fappend()     */
#define SETTAL      (PFS_BASE_NUMBER_STANDARD + 36)     /* pf_settailbuf()  */
#define DRVERR      (PFS_BASE_NUMBER_STANDARD + 37)     /* pf_derrnum()     */
#define FSEXEC      (PFS_BASE_NUMBER_STANDARD + 38)     /* pf_fsexec()      */
#define UMTHDD      (PFS_BASE_NUMBER_STANDARD + 39)     /* pf_unmount()     */
#define COMBIN      (PFS_BASE_NUMBER_STANDARD + 40)     /* pf_combine()     */
#define DTCHDD      (PFS_BASE_NUMBER_STANDARD + 41)     /* pf_detach()      */
#define DIVIDE      (PFS_BASE_NUMBER_STANDARD + 42)     /* pf_divide()      */
#define XDIVID      (PFS_BASE_NUMBER_STANDARD + 43)     /* pf_divide()      */
#define FADJST      (PFS_BASE_NUMBER_STANDARD + 44)     /* pf_fadjust()     */
#define GETINF      (PFS_BASE_NUMBER_STANDARD + 45)     /* pf_finfo()       */
#define MOVFIL      (PFS_BASE_NUMBER_STANDARD + 46)     /* pf_move()        */
#define CINSRT      (PFS_BASE_NUMBER_STANDARD + 47)     /* pf_cinsert()     */
#define INSERT      (PFS_BASE_NUMBER_STANDARD + 48)     /* pf_insert()      */
#define DELETE      (PFS_BASE_NUMBER_STANDARD + 49)     /* pf_cdelete()     */
#define CUTCLS      (PFS_BASE_NUMBER_STANDARD + 50)     /* pf_cut()         */
#define STVCFG      (PFS_BASE_NUMBER_STANDARD + 51)     /* pf_setvolcfg()   */
#define GTVCFG      (PFS_BASE_NUMBER_STANDARD + 52)     /* pf_getvolcfg()   */
#define SETCOD      (PFS_BASE_NUMBER_STANDARD + 53)     /* pf_setcode()     */
#define OPNDIR      (PFS_BASE_NUMBER_STANDARD + 54)     /* pf_opendir()     */
#define CLSDIR      (PFS_BASE_NUMBER_STANDARD + 55)     /* pf_closedir()    */
#define READIR      (PFS_BASE_NUMBER_STANDARD + 56)     /* pf_readdir()     */
#define TELDIR      (PFS_BASE_NUMBER_STANDARD + 57)     /* pf_telldir()     */
#define SEKDIR      (PFS_BASE_NUMBER_STANDARD + 58)     /* pf_seekdir()     */
#define RWDDIR      (PFS_BASE_NUMBER_STANDARD + 59)     /* pf_rewinddir()   */
#define FCHDIR      (PFS_BASE_NUMBER_STANDARD + 60)     /* pf_fchdir()      */
#define REGCTX      (PFS_BASE_NUMBER_STANDARD + 61)     /* pf_regctx()      */
#define UNRCTX      (PFS_BASE_NUMBER_STANDARD + 62)     /* pf_unregctx()    */
#define LCKFIL      (PFS_BASE_NUMBER_STANDARD + 63)     /* pf_flock()       */
#define SETENC      (PFS_BASE_NUMBER_STANDARD + 64)     /* pf_setencode()   */
#define CREDIR      (PFS_BASE_NUMBER_STANDARD + 65)     /* pf_createdir()   */
#define CLNDIR      (PFS_BASE_NUMBER_STANDARD + 66)     /* pf_cleandir()    */
#define DLTDIR      (PFS_BASE_NUMBER_STANDARD + 67)     /* pf_deletedir()   */
#define TRNCAT      (PFS_BASE_NUMBER_STANDARD + 68)     /* pf_truncate()    */
#define FTRNCT      (PFS_BASE_NUMBER_STANDARD + 69)     /* pf_ftruncate()   */
#define GTSTAP      (PFS_BASE_NUMBER_STANDARD + 70)     /* pf_getstamp()    */
#define STSTAP      (PFS_BASE_NUMBER_STANDARD + 71)     /* pf_setstamp()    */
#define GTSTMP      (PFS_BASE_NUMBER_STANDARD + 72)     /* pf_getstmp()     */
#define STSTMP      (PFS_BASE_NUMBER_STANDARD + 73)     /* pf_setstmp()     */
#define GETPOS      (PFS_BASE_NUMBER_STANDARD + 74)     /* pf_fgetpos()     */
#define SETPOS      (PFS_BASE_NUMBER_STANDARD + 75)     /* pf_fsetpos()     */
#define CHMDIR      (PFS_BASE_NUMBER_STANDARD + 76)     /* pf_chmoddir()    */
#define REGMOD      (PFS_BASE_NUMBER_STANDARD + 77)     /* pf_reg_module()  */
#define CTLMOD      (PFS_BASE_NUMBER_STANDARD + 78)     /* pf_ctrl_module() */
#define MODERR      (PFS_BASE_NUMBER_STANDARD + 79)     /* pf_module_errnum() */
#define GETEMP      (PFS_BASE_NUMBER_STANDARD + 80)     /* pf_getempent()   */
#define GETDTA      (PFS_BASE_NUMBER_STANDARD + 81)     /* pf_fgetdta()     */
#define LCKDIR      (PFS_BASE_NUMBER_STANDARD + 82)     /* pf_dlock()       */
#define GTFPST      (PFS_BASE_NUMBER_STANDARD + 83)     /* pf_fpstat()      */
#define STUPBF      (PFS_BASE_NUMBER_STANDARD + 84)     /* pf_setupcasebuf()*/
#define FDIVID      (PFS_BASE_NUMBER_STANDARD + 85)     /* pf_fdivide()     */
#define GTDEMP      (PFS_BASE_NUMBER_STANDARD + 86)     /* pf_dgetempent()  */
#define GINERR      (PFS_BASE_NUMBER_STANDARD + 87)     /* pf_inerrnum()    */
#define SINERR      (PFS_BASE_NUMBER_STANDARD + 88)     /* pf_setinerrnum() */
#define SFCSPO      (PFS_BASE_NUMBER_STANDARD + 89)     /* pf_setfcspos()   */
#define ICINST      (PFS_BASE_NUMBER_STANDARD + 90)     /* pf_iconv_install()  */
#define ICUNIN      (PFS_BASE_NUMBER_STANDARD + 91)     /* pf_iconv_uninstall()*/
#define ICOPEN      (PFS_BASE_NUMBER_STANDARD + 92)     /* pf_iconv_open()     */
#define ICCLOS      (PFS_BASE_NUMBER_STANDARD + 93)     /* pf_iconv_close()    */
#define DRVCTL      (PFS_BASE_NUMBER_STANDARD + 94)     /* pf_drvctl()      */
#define DPSTAT      (PFS_BASE_NUMBER_STANDARD + 95)     /* pf_dp_fstat()    */
#define DPTELD      (PFS_BASE_NUMBER_STANDARD + 96)     /* pf_dp_telldir()  */

#endif /* PF_USE_API_STUB */

#endif  /* __PF_STUB_STANDARD_P_H__ */
