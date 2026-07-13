/*
     Copyright (C) 2009 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_lfs_common.h] - Definitions and Declarations commonly used in LFS.
 ****************************************************************************/
#ifndef __PF_LFS_APICMN_H__
#define __PF_LFS_APICMN_H__

#include "prfile2/pf_apicmn_def.h"

/****************************************************************************
  Internal Definitions and Declarations
 ****************************************************************************/
/*---------------------------------------------------------------------------
  API number for PFS_IOCTL
 ----------------------------------------------------------------------------*/
/*
    Notes:
        These definitions are used for the 'dcmd' parameter of the devctl().
        The numbering rule is as follows.

        bits 30-31: direction of the 'data' parameter of the devctl()
                      none  : 0x00000000
                      in    : 0x40000000
                      out   : 0x80000000
                      in/out: 0xc0000000
        bits 16-29: reserved
        bits  0-15: command
*/

#define  PF_IOCTL_RMFS            (0x00000000)

#if PFS_PLUGIN_FOR_LFS_SUPPORT
#define  PF_IOCTL_MOUNT           (0x00000010)
#define  PF_IOCTL_UNMOUNT         (0x40000025)
#define  PF_IOCTL_BLKINSERT       (0x4000002d)
#define  PF_IOCTL_BLKDELETE       (0x4000002f)
#define  PF_IOCTL_COMBINE         (0x40000026)
#define  PF_IOCTL_DIVIDE          (0x40000029)
#define  PF_IOCTL_CLEANDIR        (0xc0000041)
#define  PF_IOCTL_DELETEDIR       (0x40000042)
#define  PF_IOCTL_CHMODDIR        (0xc000004b)
#define  PF_IOCTL_SETVOL          (0x40000017)
#define  PF_IOCTL_GETVOL          (0x80000018)
#define  PF_IOCTL_RMVOL           (0x00000019)
#define  PF_IOCTL_BUFFERING       (0x40000012)
#define  PF_IOCTL_SETUPFSI        (0x4000001c)
#define  PF_IOCTL_SETTAILBUF      (0x40000022)
#define  PF_IOCTL_SETVOLCFG       (0x40000033)
#define  PF_IOCTL_GETVOLCFG       (0x80000032)
#define  PF_IOCTL_SETENCODE       (0x4000003f)
#define  PF_IOCTL_FORMAT          (0x40000011)
#define  PF_IOCTL_STATVFS         (0x80000016)
#define  PF_IOCTL_INERRNUM        (0x00000052)
#define  PF_IOCTL_MODERRNUM       (0x0000004e)
#define  PF_IOCTL_DLOCK           (0x40000053)
#endif /* PFS_PLUGIN_FOR_LFS_SUPPORT */

/* for debug */
#define  PF_IOCTL_DEBUG           (0xc000ffff)

/*---------------------------------------------------------------------------
  Special Charactor for Open Drive
 ----------------------------------------------------------------------------*/
#define  DRV_OPEN_NAME          "???"
#define  DRV_OPEN_NAME_LENGTH   (3ul)

/*---------------------------------------------------------------------------
  data sturcture for xxx_np() API & PFS
 ----------------------------------------------------------------------------*/
#if PFS_PLUGIN_FOR_LFS_SUPPORT
/* unmount_np */
struct PF_LFS_UNMOUNT_PARAM
{
    PF_U_LONG   umnt_mode;
};
typedef struct PF_LFS_UNMOUNT_PARAM unmount_np_param_t;

/* blkinsert_np */
struct PF_LFS_BLKINSERT_PARAM
{
    PF_CONST PF_CHAR    *bist_fname;
    PF_U_LONG           bist_pos;
    PF_U_LONG           bist_num;
};
typedef struct PF_LFS_BLKINSERT_PARAM blkinsert_np_param_t;

/* blkdelete_np */
struct PF_LFS_BLKDELETE_PARAM
{
    PF_CONST PF_CHAR    *bdel_fname;
    PF_U_LONG           bdel_pos;
    PF_U_LONG           bdel_num;
};
typedef struct PF_LFS_BLKDELETE_PARAM blkdelete_np_param_t;

/* combine_np */
struct PF_LFS_COMBINE_PARAM
{
    PF_CONST PF_CHAR    *cmb_base_path;
    PF_CONST PF_CHAR    *cmb_add_path;
};
typedef struct PF_LFS_COMBINE_PARAM combine_np_param_t;

/* divide_np */
struct PF_LFS_DIVIDE_PARAM
{
    PF_CONST PF_CHAR    *divd_org_path;
    PF_CONST PF_CHAR    *divd_new_path;
    PF_SIZE_T           divd_offset;
};
typedef struct PF_LFS_DIVIDE_PARAM divide_np_param_t;

/* cleandir_np */
struct PF_LFS_CLEANDIR_PARAM
{
    PF_CONST PF_CHAR    *cledir_dname;
    PF_CONST PF_CHAR    *cledir_fname;
    PF_U_LONG           cledir_mode;
    PF_U_LONG           *cledir_p_count;
};
typedef struct PF_LFS_CLEANDIR_PARAM cleandir_np_param_t;

/* deletedir_np */
struct PF_LFS_DELETEDIR_PARAM
{
    PF_CONST PF_CHAR    *deldir_dname;
};
typedef struct PF_LFS_DELETEDIR_PARAM deletedir_np_param_t;

/* chmoddir_np */
struct PF_LFS_CHMODDIR_PARAM
{
    PF_CONST PF_CHAR    *chmdir_dname;
    PF_CONST PF_CHAR    *chmdir_fname;
    PF_U_LONG           chmdir_mode;
    PF_U_LONG           chmdir_attr;
    PF_U_LONG           *chmdir_p_count;
};
typedef struct PF_LFS_CHMODDIR_PARAM chmoddir_np_param_t;

/* setvol_np */
struct PF_LFS_SETVOL_PARAM
{
    PF_CONST PF_CHAR    *svol_vname;
};
typedef struct PF_LFS_SETVOL_PARAM setvol_np_param_t;

/* getvol_np */
struct PF_LFS_GETVOL_PARAM
{
    PF_VOLTAB   *gvol_p_voltab;
};
typedef struct PF_LFS_GETVOL_PARAM getvol_np_param_t;

/* buffering_np */
struct PF_LFS_BUFFERING_PARAM
{
    PF_S_LONG   bufrng_mode;
};
typedef struct PF_LFS_BUFFERING_PARAM buffering_np_param_t;

/* setupfsi_np */
struct PF_LFS_SETUPFSI_PARAM
{
    PF_S_SHORT  sfsi_flags;
};
typedef struct PF_LFS_SETUPFSI_PARAM setupfsi_np_param_t;

/* settailbuf_np */
struct PF_LFS_SETTAILBUF_PARAM
{
    PF_U_LONG   stailb_size;
};
typedef struct PF_LFS_SETTAILBUF_PARAM settailbuf_np_param_t;

/* setvolcfg_np */
struct PF_LFS_SETVOLCFG_PARAM
{
    PF_VOL_CFG  *svcfg_p_config;
};
typedef struct PF_LFS_SETVOLCFG_PARAM setvolcfg_np_param_t;

/* getvolcfg_np */
struct PF_LFS_GETVOLCFG_PARAM
{
    PF_VOL_CFG  *gvcfg_p_config;
};
typedef struct PF_LFS_GETVOLCFG_PARAM getvolcfg_np_param_t;

/* setencode_np */
struct PF_LFS_SETENCODE_PARAM
{
    PF_U_LONG   senc_mode;
};
typedef struct PF_LFS_SETENCODE_PARAM setencode_np_param_t;

/* format_np */
struct PF_LFS_FORMAT_PARAM
{
    PF_CONST PF_CHAR   *fmt_param;
    PF_SIZE_T           fmt_size;
};
typedef struct PF_LFS_FORMAT_PARAM format_np_param_t;

/* statvfs_np */
struct PF_LFS_STATVFS_PARAM
{
    PF_STATVFS  *stv_p_statvfs;
};
typedef struct PF_LFS_STATVFS_PARAM statvfs_np_param_t;

/* dlock_np */
struct PF_LFS_DLOCK_PARAM
{
    PF_U_LONG   dlck_lock_mode;
};
typedef struct PF_LFS_DLOCK_PARAM dlock_np_param_t;

#endif /* PFS_PLUGIN_FOR_LFS_SUPPORT */

#endif  /* __PF_LFS_APICMN_H__ */

