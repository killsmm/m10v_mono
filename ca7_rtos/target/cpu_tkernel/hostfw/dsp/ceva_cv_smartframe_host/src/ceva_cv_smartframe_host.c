/**
 *
 * @file		ceva_cv_smartframe_host.c
 * @brief		Smart Frame API for host
 * @author		H.Maeda (SNI)
 * @version		V00L01R00
 * @since		2015/12/24
 * @note		CEVA-Link/AMF is not used.
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT INC. All Rights Reserved.
 *
 */


#include "CEVA_CV_SmartFrame_host.h"

#include "ceva_cv_smartframe_user.h"		// user implementation function
#include "ceva_cv_smartframe_cmd_param.h"	// command and parameters

#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CACHE_LINE_SIZE			(64)	/**< The line length of cpu data cache [bytes] */

// note: About "D_CEVA_CV_NUM_PARAM_BUF"
//   This value is same as initial value of the counting semaphore "SID_DSPIF_REQ".
#define D_CEVA_CV_NUM_PARAM_BUF		(6)		/**< The number of parameter buffer */

// note: About "D_CEVA_CV_SIZE_PARAM_BUF"
//   This value should be bigger than the size of struct "sf_xxx_cmd_param_s".
//   In this case, (64*5) = 320 >= sizeof(sf_execute_cmd_param_s).
//   "sf_execute_cmd_param_s" is biggest struct.
#define D_CEVA_CV_SIZE_PARAM_BUF	(D_CACHE_LINE_SIZE * 5)	/**< The size of one parameter buffer [bytes] */

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// bool
typedef enum {
	FALSE = 0,	/**< false */
	TRUE = 1	/**< true */
} BOOL;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Parameter buffer. It is used when host requests dsp to execute SmartFrame API.
typedef struct {
	BOOL			is_locked;									/**< TRUE: locked   FALSE: Unlocked */
	unsigned long	param_buf[D_CEVA_CV_SIZE_PARAM_BUF >> 2];	/**< Body of Parameter buffer */
} param_buf_t;

/*----------------------------------------------------------------------*/
/* Data																	*/
/*----------------------------------------------------------------------*/
static param_buf_t g_param_buf[D_CEVA_CV_NUM_PARAM_BUF] __attribute__((aligned(D_CACHE_LINE_SIZE)));	/**< All Paramter buffer. */
//static param_buf_t g_param_buf[D_CEVA_CV_NUM_PARAM_BUF];	/**< All Paramter buffer. */

/*----------------------------------------------------------------------*/
/* Static function														*/
/*----------------------------------------------------------------------*/

/**
 * Get Parameter buffer.
 *
 * @retval  not NULL : The pointer of body of parameter buffer.
 * @retval	NULL 	 : Error
 */
static void* get_param_buf(void)
{
	int retval;
	void *ret_param_buf = NULL;


	retval = ceva_cv_sf_user_wai_sem();
	if (D_CEVA_CV_SF_USER_OK != retval) {
		return NULL;
	}

	// note: --- Start of Critical Section ---
	retval = ceva_cv_sf_user_loc_mtx();
	if (D_CEVA_CV_SF_USER_OK == retval) {
		int i;
		param_buf_t	*ptr = g_param_buf;

		for (i = 0; i < D_CEVA_CV_NUM_PARAM_BUF; i++) {
			if (FALSE == ptr->is_locked) {
				ptr->is_locked = TRUE;
				ret_param_buf = ptr->param_buf;
				break;
			}
			ptr++;
		}

		ceva_cv_sf_user_unl_mtx();
	}
	// note: --- End of Critical Section ---

	return ret_param_buf;
}


/**
 * Free Parameter buffer.
 *
 * @param[in] param_buf : parameter buffer to be released.
 */
static void free_param_buf(void *param_buf)
{
	int i;
	int retval;
	param_buf_t	*ptr = g_param_buf;


	// note: --- Start of Critical Section ---
	retval = ceva_cv_sf_user_loc_mtx();
	if (D_CEVA_CV_SF_USER_OK == retval) {

		for (i = 0; i < D_CEVA_CV_NUM_PARAM_BUF; i++) {
			if ((param_buf == ptr->param_buf) &&
				(ptr->is_locked == TRUE)) {
				ptr->is_locked = FALSE;
				break;
			}
		}

		ceva_cv_sf_user_unl_mtx();
	}
	// note: --- End of Critical Section ---

	ceva_cv_sf_user_sig_sem();

	return;
}


/**
 * Execute smartframe using inter-processor-communication.
 *
 * @param[in]   cmd
 * @param[in]   param_buf
 * @param[in]   param_size
 * @param[out]  p_seq_handle
 * @retval  sf_error_e
 */
static sf_error_e ceva_cv_smartframe_main(e_sf_cmd		cmd,
										  void 			*param_buf,
										  unsigned int	param_size,
										  sf_seq_handle	*p_seq_handle)
{
	unsigned short 	send_id;
	int				retval;
	send_msg_t		send_msg;
	receive_msg_t	receive_msg;


	if (param_buf != NULL) {
		ceva_cv_sf_user_clean_data_cache(param_buf, param_size);
	}

	send_msg.cmd = (unsigned long)cmd;
	send_msg.param_buf = (unsigned long)ceva_cv_sf_user_convert_host2dsp(param_buf);
	send_msg.param_size = (unsigned long)param_size;

	retval = ceva_cv_sf_user_send_msg(&send_id, &send_msg);
	if (D_CEVA_CV_SF_USER_OK != retval) {
		return SMARTFRAME_FAILED;
	}

	retval = ceva_cv_sf_user_wait_msg(send_id, &receive_msg);
	if (D_CEVA_CV_SF_USER_OK != retval) {
		return SMARTFRAME_FAILED;
	}

	if (p_seq_handle != NULL) {
		*p_seq_handle = (sf_seq_handle*)receive_msg.seq_handle;
	}

	return (sf_error_e)receive_msg.retval;
}

/*----------------------------------------------------------------------*/
/* External function													*/
/*----------------------------------------------------------------------*/

/**
 * Performs initialization of SmartFrame sequence.
 *
 * @param[out]  p_seq_handle
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_init_sequence(sf_seq_handle* p_seq_handle)
{
	sf_error_e	retval;
	void *param_buf = NULL;
	unsigned int param_size = 0;

	retval = ceva_cv_smartframe_main(E_SF_CMD_INIT_SEQUENCE,
									 param_buf,
									 param_size,
									 p_seq_handle);

	return retval;
}


/**
 * Add a kernel to a sequence by enumeration
 *
 * @param[in]  seq_handle
 * @param[in]  kernel_idx
 * @param[in]  kernel
 * @param[in]  src_data_type
 * @param[in]  dst_data_type
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_add_kernel(sf_seq_handle 		seq_handle,
							  sf_kernel_index_e 	kernel_idx,
							  sf_internal_kernel_e 	kernel,
							  sf_data_type_e 		src_data_type,
							  sf_data_type_e 		dst_data_type)
{
	sf_error_e					retval;
	sf_add_kernel_cmd_param_s 	*param_buf;
	unsigned int param_size = sizeof(sf_add_kernel_cmd_param_s);


	param_buf = get_param_buf();
	if (NULL == param_buf) {
		return SMARTFRAME_FAILED;
	}

	param_buf->seq_handle = (unsigned long)seq_handle;
	param_buf->kernel_idx = (unsigned long)kernel_idx;
	param_buf->kernel = (unsigned long)kernel;
	param_buf->src_data_type = (unsigned long)src_data_type;
	param_buf->dst_data_type = (unsigned long)dst_data_type;

	retval = ceva_cv_smartframe_main(E_SF_CMD_ADD_KERNEL,
									 param_buf,
									 param_size,
									 NULL);

	free_param_buf(param_buf);

	return retval;
}


/**
 * Check validity of the sequence.
 *
 * @param[in]  seq_handle
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_verify_sequence(sf_seq_handle 	seq_handle)
{
	sf_error_e				retval;
	sf_verify_cmd_param_s 	*param_buf;
	unsigned int param_size = sizeof(sf_verify_cmd_param_s);


	param_buf = get_param_buf();
	if (NULL == param_buf) {
		return SMARTFRAME_FAILED;
	}

	param_buf->seq_handle = (unsigned long)seq_handle;

	retval = ceva_cv_smartframe_main(E_SF_CMD_VERIFY_SEQUENCE,
									 param_buf,
									 param_size,
									 NULL);

	free_param_buf(param_buf);

	return retval;
}


/**
 * Set a sequence parameter
 *
 * @param[in]  seq_handle
 * @param[in]  param_idx
 * @param[in]  param_val
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_set_sequence_param(sf_seq_handle 	seq_handle,
									  sf_param_seq_e 	param_idx,
									  unsigned int 		param_val)
{
	sf_error_e							retval;
	sf_set_sequence_param_cmd_param_s 	*param_buf;
	unsigned int param_size = sizeof(sf_set_sequence_param_cmd_param_s);


	param_buf = get_param_buf();
	if (NULL == param_buf) {
		return SMARTFRAME_FAILED;
	}

	param_buf->seq_handle = (unsigned long)seq_handle;
	param_buf->param_idx = (unsigned long)param_idx;
	param_buf->param_val = (unsigned long)param_val;

	retval = ceva_cv_smartframe_main(E_SF_CMD_SET_SEQUENCE_PARAM,
									 param_buf,
									 param_size,
									 NULL);

	free_param_buf(param_buf);

	return retval;
}


/**
 * Set a kernel parameter.
 *
 * @param[in]  seq_handle
 * @param[in]  kernel_idx
 * @param[in]  param_idx
 * @param[in]  param_val
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_set_kernel_param(sf_seq_handle 		seq_handle,
									sf_kernel_index_e 	kernel_idx,
									sf_param_num_e 		param_idx,
									unsigned int 		param_val)
{
	sf_error_e							retval;
	sf_set_kernel_param_cmd_param_s 	*param_buf;
	unsigned int param_size = sizeof(sf_set_kernel_param_cmd_param_s);


	param_buf = get_param_buf();
	if (NULL == param_buf) {
		return SMARTFRAME_FAILED;
	}

	param_buf->seq_handle = (unsigned long)seq_handle;
	param_buf->kernel_idx = (unsigned long)kernel_idx;
	param_buf->param_idx = (unsigned long)param_idx;
	param_buf->param_val = (unsigned long)param_val;

	retval = ceva_cv_smartframe_main(E_SF_CMD_SET_KERNEL_PARAM,
									 param_buf,
									 param_size,
									 NULL);

	free_param_buf(param_buf);

	return retval;
}


/**
 * Set a kernel parameter.
 *
 * @param[in]  seq_handle
 * @param[in]  kernel_idx
 * @param[in]  param_idx
 * @param[in]  p_table
 * @param[in]  size_in_bytes
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_set_kernel_table_param(sf_seq_handle 		seq_handle,
										  sf_kernel_index_e 	kernel_idx,
										  sf_param_num_e 		param_idx,
										  void* 				p_table,
										  unsigned int 			size_in_bytes)
{
	unsigned int param_val;

	(void)size_in_bytes;

	param_val = (unsigned int)ceva_cv_sf_user_convert_host2dsp(p_table);

	return ceva_cv_set_kernel_param(seq_handle, kernel_idx, param_idx, param_val);
}


/**
 * Executes the selected sequence on a frame.
 *
 * @param[in]  seq_handle
 * @param[in]  p_src_frame
 * @param[in]  num_inputs
 * @param[in]  p_dst_frame
 * @param[in]  num_outputs
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_execute_sequence(sf_seq_handle 	seq_handle,
									frame_buf_t* 	p_src_frame[],
									unsigned int 	num_inputs,
									frame_buf_t* 	p_dst_frame[],
									unsigned int 	num_outputs)
{
	int i;
	sf_error_e				retval;
	sf_execute_cmd_param_s 	*param_buf;
	unsigned int			param_size = sizeof(sf_execute_cmd_param_s);


	if ((num_inputs  > SF_EXECUTE_CMD_MAX_FRAME_BUFS_SRC) ||
		(num_outputs > SF_EXECUTE_CMD_MAX_FRAME_BUFS_DST)) {
		return SMARTFRAME_FAILED;
	}

	param_buf = get_param_buf();
	if (NULL == param_buf) {
		return SMARTFRAME_FAILED;
	}

	param_buf->seq_handle = (unsigned long)seq_handle;
	for (i = 0; i < num_inputs; i++) {
		memcpy(&param_buf->src_frame[i], p_src_frame[i], sizeof(frame_buf_t));
		param_buf->src_frame[i].data = ceva_cv_sf_user_convert_host2dsp(p_src_frame[i]->data);
	}
	param_buf->num_inputs = (unsigned long)num_inputs;
	for (i = 0; i < num_outputs; i++) {
		memcpy(&param_buf->dst_frame[i], p_dst_frame[i], sizeof(frame_buf_t));
		param_buf->dst_frame[i].data = ceva_cv_sf_user_convert_host2dsp(p_dst_frame[i]->data);
	}
	param_buf->num_outputs = (unsigned long)num_outputs;

	retval = ceva_cv_smartframe_main(E_SF_CMD_EXECUTE,
									 param_buf,
									 param_size,
									 NULL);

	free_param_buf(param_buf);

	return retval;
}


/**
 * Destroy the SmartFrame sequence.
 *
 * @param[in]  seq_handle
 *
 * @retval  sf_error_e
 */
sf_error_e ceva_cv_deinit_sequence(sf_seq_handle 	seq_handle)
{
	sf_error_e						retval;
	sf_deinit_param_cmd_param_s 	*param_buf;
	unsigned int					param_size = sizeof(sf_deinit_param_cmd_param_s);


	param_buf = get_param_buf();
	if (NULL == param_buf) {
		return SMARTFRAME_FAILED;
	}

	param_buf->seq_handle = (unsigned long)seq_handle;

	retval = ceva_cv_smartframe_main(E_SF_CMD_DEINIT_SEQUENCE,
									 param_buf,
									 param_size,
									 NULL);

	free_param_buf(param_buf);

	return retval;
}
