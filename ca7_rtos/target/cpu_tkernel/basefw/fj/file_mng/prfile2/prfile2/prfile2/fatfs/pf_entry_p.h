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
 [pf_entry_p.h] - 

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_ENTRY_P_H__
#define __PF_ENTRY_P_H__

#include "pf_common.h"
#if PF_EXFAT_SUPPORT
#include "pf_exfat_entry.h"
#endif /* PF_EXFAT_SUPPORT */

/****************************************************************************
  Macro defintions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  SFN default tail number
 ----------------------------------------------------------------------------*/
#define DEFAULT_TAIL_NUMBER         (1)

/*---------------------------------------------------------------------------
  Timestamp
 ----------------------------------------------------------------------------*/
#define TS_MIN_YEAR                 (1980)
#define TS_MAX_YEAR                 (2107)
#define TS_MIN_MONTH                (1)
#define TS_MAX_MONTH                (12)
#define TS_MIN_DAY                  (1)
#define TS_MAX_DAY                  (31)
#define TS_MIN_HOUR                 (0)
#define TS_MAX_HOUR                 (23)
#define TS_MIN_MIN                  (0)
#define TS_MAX_MIN                  (59)
#define TS_MIN_SEC                  (0)
#define TS_MAX_SEC                  (59)
#define TS_MIN_MS                   (0)
#define TS_MAX_MS                   (199)
#if PF_EXFAT_SUPPORT
#define TS_MIN_UTC                  (-960)
#define TS_MAX_UTC                  (945)
#endif

#define YEAR_MASK                   (0xfe00)
#define MONTH_MASK                  (0x01e0)
#define DAY_MASK                    (0x001f)
#define HOUR_MASK                   (0xf800)
#define MIN_MASK                    (0x07e0)
#define SEC_MASK                    (0x001f)
#define MS_MASK                     (0xff)

#define YEAR_SHIFT                  (9)
#define MONTH_SHIFT                 (5)
#define DAY_SHIFT                   (0)
#define HOUR_SHIFT                  (11)
#define MIN_SHIFT                   (5)
#define SEC_SHIFT                   (0)

#define YEAR_ADJUST                 (1980)
#define SEC_ADJUST                  (1)

#define GET_SYS_YEAR(val)           ((((val) & YEAR_MASK ) >> YEAR_SHIFT) + YEAR_ADJUST)
#define GET_SYS_MONTH(val)          (((val)  & MONTH_MASK) >> MONTH_SHIFT)
#define GET_SYS_DAY(val)            ((val)   & DAY_MASK)
#define GET_SYS_HOUR(val)           (((val)  & HOUR_MASK ) >> HOUR_SHIFT)
#define GET_SYS_MIN(val)            (((val)  & MIN_MASK  ) >> MIN_SHIFT)
#define GET_SYS_SEC(val)            (((val)  & SEC_MASK  ) << SEC_ADJUST)

#define SET_SYS_DATE(y,m,d)         (((((y) - YEAR_ADJUST) << YEAR_SHIFT) & YEAR_MASK) | \
                                     (((m) << MONTH_SHIFT) & MONTH_MASK)               | \
                                     ((d) & DAY_MASK))
#define SET_SYS_TIME(h,m,s)         ((((h) << HOUR_SHIFT) & HOUR_MASK) | \
                                     (((m) << MIN_SHIFT) & MIN_MASK)   | \
                                     (((s) >> SEC_ADJUST) & SEC_MASK))
                                     
#if PF_EXFAT_SUPPORT
#define GET_SYS_UTC_VALID(val)      ((val) & (PF_U_CHAR)PF_SETSTMP_UTC_VALID)

#define GET_SYS_UTC_OFFSET(val)     (!GET_SYS_UTC_VALID(val) ?              \
                                     0 : (((val) & 0x40) ?                  \
                                      (PF_S_SHORT)(0xff80 | (val)) * 15 :   \
                                      (PF_S_SHORT)(0x003f & (val)) * 15 ))


#define SET_SYS_UTC(valid, val)     PFENT_EXFAT_CalcUtcOffset(valid, val)
#endif

#define IS_SEC_ODD(sec)             (((sec) & 0x01uL) == 0x01uL)

#endif  /* __PF_ENTRY_P_H__ */
