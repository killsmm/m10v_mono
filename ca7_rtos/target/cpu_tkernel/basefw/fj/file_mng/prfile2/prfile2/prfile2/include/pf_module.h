/*
     Copyright (C) 2008 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_module.h] - Extension Module
 ****************************************************************************/
#ifndef __PF_MODULE_H__
#define __PF_MODULE_H__

#include "prfile2/pfm_module.h"
#include "pf_common.h"

#if PF_EXAPI_SUPPORT
/****************************************************************************
  PrF2 Extension API
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Handle
 ----------------------------------------------------------------------------*/
/*
   Handle format

   32              16              0 bit
   +---+---+---+---+---+---+---+---+
   |   Signature   |  ID   | Data  |
   +---+---+---+---+---+---+---+---+
 */
#define PFMOD_HANDLE_VOID        (0x00000000ul)

#define PFMOD_HANDLE_MASK_DATA   (0x000000FFul)
#define PFMOD_HANDLE_MASK_ID     (0x000000FFul)
#define PFMOD_HANDLE_MASK_SIGN   (0x0000FFFFul)

#define PFMOD_HANDLE_SHIFT_DATA  (0)
#define PFMOD_HANDLE_SHIFT_ID    (8)
#define PFMOD_HANDLE_SHIFT_SIGN  (16)


/* Getter */
#define PFMOD_HANDLE_GET_DATA(hdl)                                                  \
            (                                                                       \
                (PF_U_CHAR)(((hdl) >> PFMOD_HANDLE_SHIFT_DATA)                      \
                             & PFMOD_HANDLE_MASK_DATA)                              \
            )

#define PFMOD_HANDLE_GET_ID(hdl)                                                    \
            (                                                                       \
                (PF_U_CHAR)(((hdl) >> PFMOD_HANDLE_SHIFT_ID)                        \
                             & PFMOD_HANDLE_MASK_ID)                                \
            )

#define PFMOD_HANDLE_GET_SIGN(hdl)                                                  \
            (                                                                       \
                (PF_U_SHORT)(((hdl) >> PFMOD_HANDLE_SHIFT_SIGN)                     \
                              & PFMOD_HANDLE_MASK_SIGN)                             \
            )


/* Setter */
#define PFMOD_HANDLE_SET_DATA(hdl,data)                                             \
            do {                                                                    \
                (hdl) &= ~(PFMOD_HANDLE_MASK_DATA << PFMOD_HANDLE_SHIFT_DATA);      \
                (hdl) |= (((data) & PFMOD_HANDLE_MASK_DATA)                         \
                          << PFMOD_HANDLE_SHIFT_DATA);                              \
            } while(0)

#define PFMOD_HANDLE_SET_ID(hdl,ID)                                                 \
            do {                                                                    \
                (hdl) &= ~(PFMOD_HANDLE_MASK_ID << PFMOD_HANDLE_SHIFT_ID);          \
                (hdl) |= (((ID) & PFMOD_HANDLE_MASK_ID)                             \
                          << PFMOD_HANDLE_SHIFT_ID);                                \
            } while(0)

#define PFMOD_HANDLE_SET_SIGN(hdl,sign)                                             \
            do {                                                                    \
                (hdl) &= ~(PFMOD_HANDLE_MASK_SIGN << PFMOD_HANDLE_SHIFT_SIGN);      \
                (hdl) |= (((sign) & PFMOD_HANDLE_MASK_SIGN)                         \
                          << PFMOD_HANDLE_SHIFT_SIGN);                              \
            } while(0)


/*---------------------------------------------------------------------------
  PFMOD_CreateHandle - Create handle
 ----------------------------------------------------------------------------*/
PFM_HANDLE  PFMOD_CreateHandle(PFM_HANDLE hdl, PF_U_CHAR ID, PF_U_CHAR data);

/*---------------------------------------------------------------------------
  PFMOD_UpdateHandle - Create handle
 ----------------------------------------------------------------------------*/
PFM_HANDLE  PFMOD_UpdateHandle(PFM_HANDLE hdl, PF_U_CHAR data);

#endif /* PF_EXAPI_SUPPORT */


#if PF_FATINFO_API_SUPPORT
/****************************************************************************
  FAT Information API
 ****************************************************************************/
/*---------------------------------------------------------------------------
  FATD
 ----------------------------------------------------------------------------*/
#define PF_FATD_HANDLE_ID           (0x00000006ul)

#define IS_FATD(hdl)                (PFMOD_HANDLE_GET_ID(hdl) == PF_FATD_HANDLE_ID)

#define FATD_STAT_UNKNOWN           (-1)
#define FATD_STAT_CALL_BY_INSIDE    (0)
#define FATD_STAT_CALL_BY_OUTSIDE   (1)

#endif /* PF_FATINFO_API_SUPPORT */


#endif /* __PF_MODULE_H__ */
