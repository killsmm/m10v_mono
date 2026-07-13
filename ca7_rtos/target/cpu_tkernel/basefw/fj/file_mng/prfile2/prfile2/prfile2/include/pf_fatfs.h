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
 [pf_fatfs.h] - Declaration of FATFS module entry routine.
 ****************************************************************************/
#ifndef __PF_FATFS_H__
#define __PF_FATFS_H__

#include "pf_common.h"

/****************************************************************************
  FATFS Primitive Functions Prototypes

  These functions are a part of FATFS Primitives which is the programming
  interface between FATFS Layer and API Layer.
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFFATFS_initializeFATFS - initialize FATFS modules.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFATFS_initializeFATFS(PF_U_LONG config, void* param);


#endif  /* __PF_FATFS_H__ */
