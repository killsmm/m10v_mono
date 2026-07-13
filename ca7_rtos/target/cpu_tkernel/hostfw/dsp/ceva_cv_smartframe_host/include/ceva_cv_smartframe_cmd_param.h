/**
 *
 * @file		ceva_cv_smartframe_cmd_param.h
 * @brief		Smartframe Command and Parameters.
 * @author		H.Maeda (SNI)
 * @version		V00L01R00
 * @since		2015/12/24
 * @note		It is implemented with reference to
 * 			    "CEVA-MM3101_CEVA-Link_V1.0.1\host\AMFSmartFrameComponent\include\SmartFrame_component_common.h"
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT INC. All Rights Reserved.
 *
 */


#ifndef _CEVA_CV_SMARTFRAME_CMD_PARAM_H_
#define _CEVA_CV_SMARTFRAME_CMD_PARAM_H_

#include "frame_buf.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define SF_EXECUTE_CMD_MAX_FRAME_BUFS_SRC       8	/**< The maximum number of input  frames */
#define SF_EXECUTE_CMD_MAX_FRAME_BUFS_DST       8	/**< The maximum number of output frames */

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Command for Smartframe API.
typedef enum {
	E_SF_CMD_INIT_SEQUENCE = 0,		/**< Command: CEVA_CV_SmartFrame_init_sequence */
	E_SF_CMD_ADD_KERNEL,			/**< Command: CEVA_CV_SmartFrame_add_kernel_by_enum */
	E_SF_CMD_SET_KERNEL_PARAM,		/**< Command: CEVA_CV_SmartFrame_set_kernel_param */
	E_SF_CMD_SET_SEQUENCE_PARAM,	/**< Command: CEVA_CV_SmartFrame_set_sequence_param */
	E_SF_CMD_VERIFY_SEQUENCE,		/**< Command: CEVA_CV_SmartFrame_verify */
	E_SF_CMD_EXECUTE,				/**< Command: CEVA_CV_SmartFrame_execute */
	E_SF_CMD_DEINIT_SEQUENCE,		/**< Command: CEVA_CV_SmartFrame_deinit_sequence */
	E_SF_CMD_NUM					/**< The number of Command */
} e_sf_cmd;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Parameters for E_SF_CMD_ADD_KERNEL
typedef struct {
	unsigned long	seq_handle;
	unsigned long 	kernel_idx;
	unsigned long 	kernel;
	unsigned long	src_data_type;
	unsigned long	dst_data_type;
} sf_add_kernel_cmd_param_s;

// Parameters for E_SF_CMD_VERIFY_SEQUENCE
typedef struct {
	unsigned long	seq_handle;
} sf_verify_cmd_param_s;

// Parameters for E_SF_CMD_SET_SEQUENCE_PARAM
typedef struct {
	unsigned long	seq_handle;
	unsigned long	param_idx;
	unsigned long	param_val;
} sf_set_sequence_param_cmd_param_s;

// Parameters for E_SF_CMD_SET_KERNEL_PARAM
typedef struct {
	unsigned long	seq_handle;
	unsigned long	kernel_idx;
	unsigned long	param_idx;
	unsigned long	param_val;
} sf_set_kernel_param_cmd_param_s;

// Parameters for E_SF_CMD_EXECUTE
typedef struct {
	unsigned long	seq_handle;
	frame_buf_t 	src_frame[SF_EXECUTE_CMD_MAX_FRAME_BUFS_SRC];
	unsigned long 	num_inputs;
	frame_buf_t 	dst_frame[SF_EXECUTE_CMD_MAX_FRAME_BUFS_DST];
	unsigned long 	num_outputs;
} sf_execute_cmd_param_s;

// Parameters for E_SF_CMD_DEINIT_SEQUENCE
typedef struct {
	unsigned long	seq_handle;
} sf_deinit_param_cmd_param_s;

/*----------------------------------------------------------------------*/
/* External Data														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* External function													*/
/*----------------------------------------------------------------------*/

#endif // _CEVA_CV_SMARTFRAME_CMD_PARAM_H_
