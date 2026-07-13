/**
 *
 * @file		ceva_cv_smartframe_user.h
 * @brief		User implementation
 * @author		H.Maeda (SNI)
 * @version		V00L01R00
 * @since		2015/12/24
 * @note
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT INC. All Rights Reserved.
 *
 */


#ifndef _CEVA_CV_SMARTFRAME_USER_H_
#define _CEVA_CV_SMARTFRAME_USER_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CEVA_CV_SF_USER_OK	(0)		/**< Success */
#define D_CEVA_CV_SF_USER_NG	(-1)	/**< Failure */

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Message to be sent
typedef struct {
	unsigned long	cmd;			/**< command. It should be set the value of "e_sf_cmd". */
	unsigned long	param_buf;		/**< The pointer of body of parameter buffer */
	unsigned long	param_size;		/**< The size of parameters [bytes] */
} send_msg_t;

// Message to be received
typedef struct {
	unsigned long 	retval;			/**< Returned value of smartframe api. */
	unsigned long	seq_handle;		/**< seq_handle. Is is valid only when issuing "E_SF_CMD_INIT_SEQUENCE". */
} receive_msg_t;

/*----------------------------------------------------------------------*/
/* External Data														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* External function													*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

extern void* ceva_cv_sf_user_convert_host2dsp(void *addr);
extern void ceva_cv_sf_user_clean_data_cache(void* addr, unsigned long size);
extern int ceva_cv_sf_user_send_msg(unsigned short *send_id, send_msg_t	*msg);
extern int ceva_cv_sf_user_wait_msg(unsigned short send_id, receive_msg_t *msg);
extern int ceva_cv_sf_user_wai_sem(void);
extern int ceva_cv_sf_user_sig_sem(void);
extern int ceva_cv_sf_user_loc_mtx(void);
extern int ceva_cv_sf_user_unl_mtx(void);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _CEVA_CV_SMARTFRAME_USER_H_
