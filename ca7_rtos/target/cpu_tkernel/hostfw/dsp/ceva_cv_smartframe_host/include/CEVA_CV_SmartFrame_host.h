/*************************************************************************************\
*																					 *
*Copyright (C) CEVA Inc. All rights reserved                                         *
*																					 *
*																					 *
*THIS PRODUCT OR SOFTWARE IS MADE AVAILABLE EXCLUSIVELY TO LICENSEES THAT HAVE       *
*RECEIVED EXPRESS WRITTEN AUTHORIZATION FROM CEVA TO DOWNLOAD OR RECEIVE THE         *
*PRODUCT OR SOFTWARE AND HAVE AGREED TO THE END USER LICENSE AGREEMENT (EULA).       *
*IF YOU HAVE NOT RECEIVED SUCH EXPRESS AUTHORIZATION AND AGREED TO THE               *
*CEVA EULA, YOU MAY NOT DOWNLOAD, INSTALL OR USE THIS PRODUCT OR SOFTWARE.           *
*																					 *
*The information contained in this document is subject to change without notice and  *
*does not represent a commitment on any part of CEVA®, Inc. CEVA®, Inc. and its      *
*subsidiaries make no warranty of any kind with regard to this material, including,  *
*but not limited to implied warranties of merchantability and fitness for a          *
*particular purpose whether arising out of law, custom, conduct or otherwise.        *
*																					 *
*While the information contained herein is assumed to be accurate, CEVA®, Inc.       *
*assumes no responsibility for any errors or omissions contained herein, and         *
*assumes no liability for special, direct, indirect or consequential damage,         *
*losses, costs, charges, claims, demands, fees or expenses, of any nature or kind,   *
*which are incurred in connection with the furnishing, performance or use of this    *
*material.                                                                           *
*																					 *
*This document contains proprietary information, which is protected by U.S. and      *
*international copyright laws. All rights reserved. No part of this document may be  *
*reproduced, photocopied, or translated into another language without the prior      *
*written consent of CEVA®, Inc.                                                      *
*************************************************************************************/


/** 
\brief CEVA SmartFrame Host API
*/


#ifndef _CEVA_CV_SMARTFRAME_HOST_H_
#define _CEVA_CV_SMARTFRAME_HOST_H_

#include "CEVA_CV_SmartFrame_common.h"
#include "frame_buf.h"

/** 
* \addtogroup SmartFrame
*  @{
*/

/**
* \brief Performs initialization of SmartFrame sequence.
*
* Initialize a SmartFrame sequence, allocating all internal resources.
*
* @param[out] p_seq_handle pointer to the returned sequence handle
* @return error status enumeration
*/
sf_error_e ceva_cv_init_sequence(sf_seq_handle* p_seq_handle);



/**
* \brief Add a kernel to a sequence by enumeration
*
* @param[in] seq_handle sequence handle
* @param[in] kernel_idx position of the kernel in the sequence
* @param[in] kernel kernel name enumeration
* @param[in] src_data_type source data type enumeration
* @param[in] dst_data_type destination data type enumeration
* @return error status enumeration
*/
sf_error_e ceva_cv_add_kernel(sf_seq_handle seq_handle,
												 sf_kernel_index_e kernel_idx,
												 sf_internal_kernel_e kernel,
												 sf_data_type_e src_data_type,
												 sf_data_type_e dst_data_type);

/**
* \brief Check validity of the sequence.
*
* @param[in] seq_handle smart frame sequence handle.
* @return error status enumeration
*/

sf_error_e ceva_cv_verify_sequence(sf_seq_handle seq_handle);

/**
* \brief Set a sequence parameter
*
* Set a sequence parameter, e.g. padding type and padding value.
*
* @param[in] seq_handle sequence handle
* @param[in] param_idx index of the parameter
* @param[in] param_val parameter value
* @return error status enumeration
*/

sf_error_e ceva_cv_set_sequence_param(sf_seq_handle seq_handle,
												 sf_param_seq_e param_idx,
												 unsigned int param_val);

/**
* \brief Set a kernel parameter.
*
* @param[in] seq_handle sequence handle
* @param[in] kernel_idx kernel index
* @param[in] param_idx parameter index
* @param[in] param_val parameter value
* @return error status enumeration
*/
sf_error_e ceva_cv_set_kernel_param(sf_seq_handle seq_handle,
											   sf_kernel_index_e kernel_idx,
											   sf_param_num_e param_idx,
											   unsigned int param_val);

/**
* \brief Set a kernel parameter.
*
* @param[in] seq_handle sequence handle
* @param[in] kernel_idx kernel index
* @param[in] param_idx parameter index
* @param[in] p_table pointer to table param
* @param[in] table size im bytes
* @return error status enumeration
*/
sf_error_e ceva_cv_set_kernel_table_param(sf_seq_handle seq_handle,
												sf_kernel_index_e kernel_idx,
												sf_param_num_e param_idx,
												void* p_table,
												unsigned int size_in_bytes);

/**
* \brief Executes the selected sequence on a frame.
*
* Source and destination buffers should contain required frame parameters: input size, input and output data and strides.
* Function calculates required output size according to the sequence and updates output buffer if successful.
* Function handles DMA transactions and calls to the kernels.
* User can also configure input padding type, if sequence contains kernels with padding.
*
* @param[in] seq_handle sequence handle.
* @param[in] p_src_frame pointer to array of source buffers.
* @param[in] p_dst_frame pointer to array of destination buffers.
* @return error status enumeration.
*/
sf_error_e ceva_cv_execute_sequence(sf_seq_handle seq_handle,
											  frame_buf_t* p_src_frame[],
											  unsigned int num_inputs,
											  frame_buf_t* p_dst_frame[],
											  unsigned int num_outputs);

/**
* \brief Destroy the SmartFrame sequence
*
* Releases all allocated resources for the SmartFrame sequence
*
* @param[in] seq_handle smart frame sequence handle
* @return error status enumeration
*/
sf_error_e ceva_cv_deinit_sequence(sf_seq_handle seq_handle);

#endif//_CEVA_CV_HOST_H_
/** @}*/
