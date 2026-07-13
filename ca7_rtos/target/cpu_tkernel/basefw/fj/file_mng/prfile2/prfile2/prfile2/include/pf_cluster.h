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
 [pf_cluster.h] -
 ****************************************************************************/
#ifndef __PF_CLUSTER_H__
#define __PF_CLUSTER_H__

#include "pf_common.h"

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCLUSTER_InitLastAccessCluster - Initialize last access cluster information.
 ----------------------------------------------------------------------------*/
void PFCLUSTER_InitLastAccessCluster(PF_FILE *p_file);

/*---------------------------------------------------------------------------
  PFCLUSTER_UpdateLastAccessCluster - Update last access cluster information.
 ----------------------------------------------------------------------------*/
void PFCLUSTER_UpdateLastAccessCluster(PF_FILE *p_file, PF_SECT sector);

/*---------------------------------------------------------------------------
  PFCLUSTER_SetLastAccessCluster - Set last access cluster information.
 ----------------------------------------------------------------------------*/
void PFCLUSTER_SetLastAccessCluster(PF_FILE *p_file);

/*---------------------------------------------------------------------------
  PFCLUSTER_AppendCluster - Append 'size' bytes clusters from the current
                            cluster boarder of the file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_AppendCluster(PF_FILE    *p_file,
                                  PF_FSIZE_T req_byte,
                                  PF_U_LONG  search_type,
                                  PF_FSIZE_T *p_append_byte,
                                  PF_SECT    *p_append_start_sect);

/*---------------------------------------------------------------------------
  PFCLUSTER_AdjustCluster - Adjust the unused clusters in the specified file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_AdjustCluster(PF_FILE *p_file);

/*---------------------------------------------------------------------------
  PFCLUSTER_GetAppendSize - Get the total size including specified size form
                            the specified cluster.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_GetAppendSize(PF_FILE *p_file, PF_FSIZE_T *p_size);

/*---------------------------------------------------------------------------
 PFCLUSTER_CombineFiles - Combine files.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_CombineFiles(PF_FFD  *p_base_ffd,
                                 PF_FFD  *p_add_ffd,
                                 PF_DENT *p_base_ent,
                                 PF_DENT *p_add_ent);

/*---------------------------------------------------------------------------
 PFCLUSTER_DivideFile - Divide files.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_DivideFile(PF_FFD    *p_org_ffd,
                               PF_DENT   *p_org_ent,
                               PF_DENT   *p_new_ent,
                               PF_FPOS_T offset);

/*---------------------------------------------------------------------------
 PFCLUSTER_InsertCluster - Insert clusters in file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_InsertCluster(PF_FFD  *p_ffd,
                                  PF_DENT *p_ent,
                                  PF_CLST cluster_offset,
                                  PF_CLST num_cluster,
                                  PF_CLST *p_inserted_cluster);

/*---------------------------------------------------------------------------
 PFCLUSTER_DeleteCluster - Delete clusters in file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFCLUSTER_DeleteCluster(PF_FFD  *p_ffd,
                                  PF_DENT *p_ent,
                                  PF_CLST cluster_offset,
                                  PF_CLST num_cluster,
                                  PF_CLST *p_deleted_cluster);

#endif  /* __PF_CLUSTER_H__ */
