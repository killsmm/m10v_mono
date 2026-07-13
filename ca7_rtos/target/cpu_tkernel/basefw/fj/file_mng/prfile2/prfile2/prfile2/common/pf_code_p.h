/*
     Copyright (C) 2005 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_code_p.h] - Character Code Module
 ****************************************************************************/
#ifndef __PF_CODE_P_H__
#define __PF_CODE_P_H__

#include "prfile2/pf_config.h"
#include "prfile2/pf_types.h"
#include "prfile2/pf_apicmn_def.h"
#include "pf_code.h"
#include "pf_volume.h"


/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define PF_CODE_OEM_WIDTH_MASK      (0xFFFF0000)
#define PF_CODE_UNI_WIDTH_MASK      (0x0000FFFF)

#define PF_CODE_U16                 (16)

#endif  /* __PF_CODE_P_H__ */
