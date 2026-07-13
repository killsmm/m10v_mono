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
 [pf_stub_unicode.h] - PrFILE2 Unicode API Stub functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_STUB_UNICODE_P_H__
#define __PF_STUB_UNICODE_P_H__

#include "prfile2/pf_config.h"

#if PF_USE_API_STUB

#include "pf_stub.h"

/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
  PrFILE2 API function number
 *---------------------------------------------------------------------*/
#define WCRTFL      (PFS_BASE_NUMBER_UNICODE + 0)       /* pf_w_create()    */
#define WOPENF      (PFS_BASE_NUMBER_UNICODE + 1)       /* pf_w_fopen()     */
#define WERSFL      (PFS_BASE_NUMBER_UNICODE + 2)       /* pf_w_remove()    */
#define WRENFL      (PFS_BASE_NUMBER_UNICODE + 3)       /* pf_w_rename()    */
#define WMKDIR      (PFS_BASE_NUMBER_UNICODE + 4)       /* pf_w_mkdir()     */
#define WRMVDR      (PFS_BASE_NUMBER_UNICODE + 5)       /* pf_w_rmdir()     */
#define WCHGDR      (PFS_BASE_NUMBER_UNICODE + 6)       /* pf_w_chdir()     */
#define WGTSTS      (PFS_BASE_NUMBER_UNICODE + 7)       /* pf_w_fstat()     */
#define WCHGMD      (PFS_BASE_NUMBER_UNICODE + 8)       /* pf_w_chmod()     */
#define WCHDMD      (PFS_BASE_NUMBER_UNICODE + 9)       /* pf_w_chdmod()    */
#define WCONCT      (PFS_BASE_NUMBER_UNICODE + 10)      /* pf_w_fconcat()   */
#define WFSFFL      (PFS_BASE_NUMBER_UNICODE + 11)      /* pf_w_fsfirst()   */
#define WFSNFL      (PFS_BASE_NUMBER_UNICODE + 12)      /* pf_w_fsnext()    */
#define WFSEXC      (PFS_BASE_NUMBER_UNICODE + 13)      /* pf_w_fsexec()    */
#define WCOMBN      (PFS_BASE_NUMBER_UNICODE + 14)      /* pf_w_combine()   */
#define WDIVID      (PFS_BASE_NUMBER_UNICODE + 15)      /* pf_w_divide()    */
#define WXDIVD      (PFS_BASE_NUMBER_UNICODE + 16)      /* pf_w_xdivide()   */
#define WMOVFL      (PFS_BASE_NUMBER_UNICODE + 17)      /* pf_w_move()      */
#define WCINST      (PFS_BASE_NUMBER_UNICODE + 18)      /* pf_w_cinsert()   */
#define WINSRT      (PFS_BASE_NUMBER_UNICODE + 19)      /* pf_w_insert()    */
#define WDELET      (PFS_BASE_NUMBER_UNICODE + 20)      /* pf_w_cdelete()   */
#define WCUTCT      (PFS_BASE_NUMBER_UNICODE + 21)      /* pf_w_cut()       */
#define WOPNDR      (PFS_BASE_NUMBER_UNICODE + 22)      /* pf_w_opendir()   */
#define WCRDIR      (PFS_BASE_NUMBER_UNICODE + 23)      /* pf_w_createdir() */
#define WCLDIR      (PFS_BASE_NUMBER_UNICODE + 24)      /* pf_w_cleandir()  */
#define WDLDIR      (PFS_BASE_NUMBER_UNICODE + 25)      /* pf_w_deletedir() */
#define WTRNCT      (PFS_BASE_NUMBER_UNICODE + 26)      /* pf_w_truncate()  */
#define WGTSMP      (PFS_BASE_NUMBER_UNICODE + 27)      /* pf_w_getstamp()  */
#define WSTSMP      (PFS_BASE_NUMBER_UNICODE + 28)      /* pf_w_setstamp()  */
#define WCMDIR      (PFS_BASE_NUMBER_UNICODE + 29)      /* pf_w_chmoddir()  */
#define WGTDTA      (PFS_BASE_NUMBER_UNICODE + 30)      /* pf_w_fgetdta()   */
#define WFDIVD      (PFS_BASE_NUMBER_UNICODE + 31)      /* pf_w_fdivide()   */
#define WSTVOL      (PFS_BASE_NUMBER_UNICODE + 32)      /* pf_w_setvol()    */
#define WGTVOL      (PFS_BASE_NUMBER_UNICODE + 33)      /* pf_w_getvol()    */
#define WRDDIR      (PFS_BASE_NUMBER_UNICODE + 34)      /* pf_w_readdir()   */
#define WDLISL      (PFS_BASE_NUMBER_UNICODE + 35)      /* pf_w_dlock_islocked()    */
#define WDFSTT      (PFS_BASE_NUMBER_UNICODE + 36)      /* pf_w_dp_fstat()  */

#endif /* PF_USE_API_STUB */


#endif  /* __PF_STUB_UNICODE_P_H__ */
