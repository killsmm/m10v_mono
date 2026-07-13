/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_path_p.h] - 

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_PATH_P_H__
#define __PF_PATH_P_H__

#include "pf_common.h"

/****************************************************************************
  Type definitions
 ****************************************************************************/
typedef struct PF_FILE_NAME_ITER
{
    const PF_CHAR*  buf;
    PF_U_SHORT      kind;
    PF_U_SHORT      index;

} PF_FILE_NAME_ITER;

#define SHORT_NAME      (0x0)
#define LONG_NAME       (0x1)


#endif  /* __PF_PATH_P_H__ */
