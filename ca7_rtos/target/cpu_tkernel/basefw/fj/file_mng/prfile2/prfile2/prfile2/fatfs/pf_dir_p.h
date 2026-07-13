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
 [pf_dir_p.h] - 
 ****************************************************************************/
#ifndef __PF_DIR_P_H__
#define __PF_DIR_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
 Definitions for directory
 ----------------------------------------------------------------------------*/
#define PF_DIR_CURSOR_OFFSET_INIT   (0)
#define PF_DIR_CURSOR_OFFSET_LAST   (0x7fffffff)

/*---------------------------------------------------------------------------
 IS_VALID_SDD - 
 ----------------------------------------------------------------------------*/
#define IS_VALID_SDD(p_dir)         ((((p_dir)->p_sdd->stat & PF_SDD_USED) == PF_SDD_USED)      \
                                     && (((p_dir)->p_sdd->stat & PF_SDD_VALID) == PF_SDD_VALID))


/*---------------------------------------------------------------------------
 IS_VALID_P_DIR - 
 ----------------------------------------------------------------------------*/
#define IS_VALID_P_DIR(p_dir)       ((p_dir != NULL)                                    \
                                     && ((p_dir)->p_sdd != NULL)                        \
                                     && (((p_dir)->stat & PF_DIR_USED) == PF_DIR_USED)  \
                                     && IS_VALID_SDD(p_dir))


/*---------------------------------------------------------------------------
 IS_UDD_FREE - 
 ----------------------------------------------------------------------------*/
#define IS_UDD_FREE(udd)            (((udd).stat & PF_DIR_USED) == 0)


/*---------------------------------------------------------------------------
 IS_SDD_FREE - 
 ----------------------------------------------------------------------------*/
#define IS_SDD_FREE(sdd)            ((((sdd).stat & PF_SDD_USED) == 0)                  \
                                     || ((((sdd).stat & PF_SDD_USED) == PF_SDD_USED)    \
                                        && (((sdd).stat & PF_SDD_VALID) == 0)))


/*---------------------------------------------------------------------------
 IS_SDD_USED - 
 ----------------------------------------------------------------------------*/
#define IS_SDD_USED(sdd)            ((((sdd).stat & PF_SDD_USED) == PF_SDD_USED)        \
                                     && (((sdd).stat & PF_SDD_VALID) == PF_SDD_VALID))


#endif  /* __PF_DIR_P_H__ */
