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
 [pf_stub_extret_p.h] - PrFILE2 EXTRET API Stub functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_STUB_EXTRET_P_H__
#define __PF_STUB_EXTRET_P_H__

#include "prfile2/pf_config.h"

#if PF_USE_API_STUB

#include "pf_stub.h"

/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
  PrFILE2 API function number
 *---------------------------------------------------------------------*/
#define ECRTFIL     (PFS_BASE_NUMBER_EXTRET + 0)      /* pf_e_create()      */
#define EOPNFIL     (PFS_BASE_NUMBER_EXTRET + 1)      /* pf_e_fopen()       */
#define ECLSFIL     (PFS_BASE_NUMBER_EXTRET + 2)      /* pf_e_fclose()      */
#define ERADFIL     (PFS_BASE_NUMBER_EXTRET + 3)      /* pf_e_fread()       */
#define EWRTFIL     (PFS_BASE_NUMBER_EXTRET + 4)      /* pf_e_fwrite()      */
#define ESEKFIL     (PFS_BASE_NUMBER_EXTRET + 5)      /* pf_e_fseek()       */
#define ESRCFIL     (PFS_BASE_NUMBER_EXTRET + 6)      /* (no equivalent)    */
#define EERSFIL     (PFS_BASE_NUMBER_EXTRET + 7)      /* pf_e_remove()      */
#define ERENFIL     (PFS_BASE_NUMBER_EXTRET + 8)      /* pf_e_rename()      */
#define EMAKDIR     (PFS_BASE_NUMBER_EXTRET + 9)      /* pf_e_mkdir()       */
#define ERMVDIR     (PFS_BASE_NUMBER_EXTRET + 10)     /* pf_e_rmdir()       */
#define ECHGDIR     (PFS_BASE_NUMBER_EXTRET + 11)     /* pf_e_chdir()       */
#define EGETSTS     (PFS_BASE_NUMBER_EXTRET + 12)     /* pf_e_fstat()       */
#define ECHGMOD     (PFS_BASE_NUMBER_EXTRET + 13)     /* pf_e_chmod()       */
#define EMNTHDD     (PFS_BASE_NUMBER_EXTRET + 14)     /* pf_e_attach()/pf_e_mount() */
#define EFMTHDD     (PFS_BASE_NUMBER_EXTRET + 15)     /* pf_e_format()      */
#define EBUFRNG     (PFS_BASE_NUMBER_EXTRET + 16)     /* pf_e_buffering()   */
#define EERRFIL     (PFS_BASE_NUMBER_EXTRET + 17)     /* pf_e_error()       */
#define EEOFFIL     (PFS_BASE_NUMBER_EXTRET + 18)     /* pf_e_eof()         */
#define EGETERR     (PFS_BASE_NUMBER_EXTRET + 19)     /* pf_e_errnum()      */
#define EDEVINF     (PFS_BASE_NUMBER_EXTRET + 20)     /* pf_e_devinf()      */
#define ESETVOL     (PFS_BASE_NUMBER_EXTRET + 21)     /* pf_e_setvol()      */
#define EGETVOL     (PFS_BASE_NUMBER_EXTRET + 22)     /* pf_e_getvol()      */
#define ERMVVOL     (PFS_BASE_NUMBER_EXTRET + 23)     /* pf_e_rmvvol()      */
#define ECHDMOD     (PFS_BASE_NUMBER_EXTRET + 24)     /* pf_e_chdmod()      */
#define ECONCAT     (PFS_BASE_NUMBER_EXTRET + 25)     /* pf_e_fconcat()     */
#define EPFINIT     (PFS_BASE_NUMBER_EXTRET + 26)     /* pf_e_init_prfile2()*/
#define EATCHDD     (PFS_BASE_NUMBER_EXTRET + 27)     /* pf_e_attach()      */
#define EFSFFIL     (PFS_BASE_NUMBER_EXTRET + 28)     /* pf_e_fsfirst()     */
#define EFSNFIL     (PFS_BASE_NUMBER_EXTRET + 29)     /* pf_e_fsnext()      */
#define ESUPFSI     (PFS_BASE_NUMBER_EXTRET + 30)     /* pf_e_setupfsi()    */
#define ESETCLK     (PFS_BASE_NUMBER_EXTRET + 31)     /* pf_e_setclstlink() */
#define EFSTCLK     (PFS_BASE_NUMBER_EXTRET + 32)     /* pf_e_fsetclstlink()*/
#define ESYNCCH     (PFS_BASE_NUMBER_EXTRET + 33)     /* pf_e_sync()        */
#define EFSYNCC     (PFS_BASE_NUMBER_EXTRET + 34)     /* pf_e_fsync()       */
#define EFAPPND     (PFS_BASE_NUMBER_EXTRET + 35)     /* pf_e_fappend()     */
#define ESETTAL     (PFS_BASE_NUMBER_EXTRET + 36)     /* pf_e_settailbuf()  */
#define EDRVERR     (PFS_BASE_NUMBER_EXTRET + 37)     /* pf_e_derrnum()     */
#define EFSEXEC     (PFS_BASE_NUMBER_EXTRET + 38)     /* pf_e_fsexec()      */
#define EUMTHDD     (PFS_BASE_NUMBER_EXTRET + 39)     /* pf_e_unmount()     */
#define ECOMBIN     (PFS_BASE_NUMBER_EXTRET + 40)     /* pf_e_combine()     */
#define EDTCHDD     (PFS_BASE_NUMBER_EXTRET + 41)     /* pf_e_detach()      */
#define EDIVIDE     (PFS_BASE_NUMBER_EXTRET + 42)     /* pf_e_divide()      */
#define EXDIVID     (PFS_BASE_NUMBER_EXTRET + 43)     /* pf_e_divide()      */
#define EFADJST     (PFS_BASE_NUMBER_EXTRET + 44)     /* pf_e_fadjust()     */
#define EGETINF     (PFS_BASE_NUMBER_EXTRET + 45)     /* pf_e_finfo()       */
#define EMOVFIL     (PFS_BASE_NUMBER_EXTRET + 46)     /* pf_e_move()        */
#define ECINSRT     (PFS_BASE_NUMBER_EXTRET + 47)     /* pf_e_cinsert()     */
#define EINSERT     (PFS_BASE_NUMBER_EXTRET + 48)     /* pf_e_insert()      */
#define EDELETE     (PFS_BASE_NUMBER_EXTRET + 49)     /* pf_e_cdelete()     */
#define ECUTCLS     (PFS_BASE_NUMBER_EXTRET + 50)     /* pf_e_cut()         */
#define ESTVCFG     (PFS_BASE_NUMBER_EXTRET + 51)     /* pf_e_setvolcfg()   */
#define EGTVCFG     (PFS_BASE_NUMBER_EXTRET + 52)     /* pf_e_getvolcfg()   */
#define ESETCOD     (PFS_BASE_NUMBER_EXTRET + 53)     /* pf_e_setcode()     */
#define EOPNDIR     (PFS_BASE_NUMBER_EXTRET + 54)     /* pf_e_opendir()     */
#define ECLSDIR     (PFS_BASE_NUMBER_EXTRET + 55)     /* pf_e_closedir()    */
#define EREADIR     (PFS_BASE_NUMBER_EXTRET + 56)     /* pf_e_readdir()     */
#define ETELDIR     (PFS_BASE_NUMBER_EXTRET + 57)     /* pf_e_telldir()     */
#define ESEKDIR     (PFS_BASE_NUMBER_EXTRET + 58)     /* pf_e_seekdir()     */
#define ERWDDIR     (PFS_BASE_NUMBER_EXTRET + 59)     /* pf_e_rewinddir()   */
#define EFCHDIR     (PFS_BASE_NUMBER_EXTRET + 60)     /* pf_e_fchdir()      */
#define EREGCTX     (PFS_BASE_NUMBER_EXTRET + 61)     /* pf_e_regctx()      */
#define EUNRCTX     (PFS_BASE_NUMBER_EXTRET + 62)     /* pf_e_unregctx()    */
#define ELCKFIL     (PFS_BASE_NUMBER_EXTRET + 63)     /* pf_e_flock()       */
#define ESETENC     (PFS_BASE_NUMBER_EXTRET + 64)     /* pf_e_setencode()   */
#define ECREDIR     (PFS_BASE_NUMBER_EXTRET + 65)     /* pf_e_createdir()   */
#define ECLNDIR     (PFS_BASE_NUMBER_EXTRET + 66)     /* pf_e_cleandir()    */
#define EDLTDIR     (PFS_BASE_NUMBER_EXTRET + 67)     /* pf_e_deletedir()   */
#define ETRNCAT     (PFS_BASE_NUMBER_EXTRET + 68)     /* pf_e_truncate()    */
#define EFTRNCT     (PFS_BASE_NUMBER_EXTRET + 69)     /* pf_e_ftruncate()   */
#define EGTSTAP     (PFS_BASE_NUMBER_EXTRET + 70)     /* pf_e_getstamp()    */
#define ESTSTAP     (PFS_BASE_NUMBER_EXTRET + 71)     /* pf_e_setstamp()    */
#define EGTSTMP     (PFS_BASE_NUMBER_EXTRET + 72)     /* pf_e_getstmp()     */
#define ESTSTMP     (PFS_BASE_NUMBER_EXTRET + 73)     /* pf_e_setstmp()     */
#define ECHMDIR     (PFS_BASE_NUMBER_EXTRET + 74)     /* pf_e_chmoddir()    */
#define ELCKDIR     (PFS_BASE_NUMBER_EXTRET + 75)     /* pf_e_dlock()       */
#define EISDLCK     (PFS_BASE_NUMBER_EXTRET + 76)     /* pf_e_dlock_islocked()  */
#define EFISDLK     (PFS_BASE_NUMBER_EXTRET + 77)     /* pf_e_dlock_fislocked() */
#define EDISDLK     (PFS_BASE_NUMBER_EXTRET + 78)     /* pf_e_dlock_dislocked() */

#endif /* PF_USE_API_STUB */

#endif  /* __PF_STUB_EXTRET_P_H__ */
