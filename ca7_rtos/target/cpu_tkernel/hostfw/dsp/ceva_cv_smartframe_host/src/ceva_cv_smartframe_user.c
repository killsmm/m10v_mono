/**
 *
 * @file		ceva_cv_smartframe_user.c
 * @brief		User implementation
 * @author		H.Maeda (SNI)
 * @version		V00L01R00
 * @since		2015/12/24
 * @note
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT INC. All Rights Reserved.
 *
 */

#include "ceva_cv_smartframe_user.h"

#include "rs_dsp.h"				// dsp if
#include "fj_memory.h"			// cache operation
#include "os_user_custom.h"		// OS Wrapper

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Compile option
//#define CO_ENABLE_PRINTF

#define D_CEVA_CV_USER_PROC_ID		(D_PROCID_DSP_CEVA_CV_SF)	/**< Processing ID for SmartFrame */
#define D_CEVA_CV_USER_WAIT_TIME_ms	(5000)						/**< Wait-time [ms] */

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/
#ifdef CO_ENABLE_PRINTF
#	include <stdio.h>
#	define M_DBG_PRINTF(arg)		printf arg
#else
#	define M_DBG_PRINTF(arg)
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Data																	*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Static function														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* External function													*/
/*----------------------------------------------------------------------*/

/**
 * Convert memory address.
 *
 * @param[in]   addr : address for host
 *
 * @retval	converted address for dsp.
 */
void* ceva_cv_sf_user_convert_host2dsp(void *addr)
{
	return addr;	// note: same address between host and dsp.
}


/**
 * Cleaning cpu data cache.
 *
 * @param[in]   addr :	Beginning address.
 * @param[in]   size :	The cleaning size.
 */
void ceva_cv_sf_user_clean_data_cache(void* addr,
									  unsigned long size)
{
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN, (ULONG)addr, size);
}


/**
 * Send message to dsp.
 *
 * @param[out]  send_id :	ID. It is used at "ceva_cv_sf_user_wait_msg".
 * @param[in]   msg 	:	Message to be sent.
 *
 * @retval	D_RS_DSP_OK : Success
 * @retval	D_RS_DSP_NG : Failure
 */
int ceva_cv_sf_user_send_msg(unsigned short *send_id,
							 send_msg_t *msg)
{
	T_RS_DSP_MSG	snd_msg;
	int	retval;


	snd_msg.data[1] = msg->cmd;
	snd_msg.data[2] = msg->param_buf;
	snd_msg.data[3] = msg->param_size;
	retval = RS_DSP_SendMsg(send_id,
							D_CEVA_CV_USER_PROC_ID,
							D_CEVA_CV_USER_WAIT_TIME_ms,
							&snd_msg);

	return (retval == D_RS_DSP_OK) ? D_CEVA_CV_SF_USER_OK : D_CEVA_CV_SF_USER_NG;
}


/**
 * Wait message from dsp.
 *
 * @param[in]  send_id :	ID. It is get from "ceva_cv_sf_user_send_msg".
 * @param[out]   msg   :	Received message.
 *
 * @retval	D_RS_DSP_OK : Success
 * @retval	D_RS_DSP_NG : Failure
 */
int ceva_cv_sf_user_wait_msg(unsigned short send_id,
							 receive_msg_t *msg)
{
	T_RS_DSP_MSG	rcv_msg;
	int	retval;


	retval = RS_DSP_WaitMsg(send_id,
							D_CEVA_CV_USER_WAIT_TIME_ms,
							&rcv_msg);
	if (D_RS_DSP_OK == retval) {
		msg->retval     = rcv_msg.data[2];
		msg->seq_handle = rcv_msg.data[3];
		
		M_DBG_PRINTF(("dsp:run  cyc: %d\n", (unsigned int)rcv_msg.data[7]));
		M_DBG_PRINTF(("dsp:wait cyc: %d\n", (unsigned int)rcv_msg.data[8]));
		
		return D_CEVA_CV_SF_USER_OK;
	}

	return D_CEVA_CV_SF_USER_NG;
}


/**
 * Wait semaphore
 *
 * @retval	D_RS_DSP_OK : Success
 * @retval	D_RS_DSP_NG : Failure
 *
 * @remarks	Please add the semaphore "SID_CEVA_CV_SF_USER".
 */
int ceva_cv_sf_user_wai_sem(void)
{
	OS_USER_ER retval;
	
	retval = OS_User_Twai_Sem( SID_CEVA_CV_SF_USER, D_CEVA_CV_USER_WAIT_TIME_ms );
	
	return (D_OS_USER_E_OK == retval) ? D_CEVA_CV_SF_USER_OK : D_CEVA_CV_SF_USER_NG;
}


/**
 * Release semaphore
 *
 * @retval	D_RS_DSP_OK : Success
 * @retval	D_RS_DSP_NG : Failure
 */
int ceva_cv_sf_user_sig_sem(void)
{
	OS_USER_ER retval;
	
	retval = OS_User_Sig_Sem( SID_CEVA_CV_SF_USER );
	
	return (D_OS_USER_E_OK == retval) ? D_CEVA_CV_SF_USER_OK : D_CEVA_CV_SF_USER_NG;
}


/**
 * Lock mutex
 *
 * @retval	D_RS_DSP_OK : Success
 * @retval	D_RS_DSP_NG : Failure
 *
 * @remarks	Please add the mutex "MTXID_CEVA_CV_SF_USER".
 */
int ceva_cv_sf_user_loc_mtx(void)
{
	OS_USER_ER retval;
	
	retval = OS_User_Tloc_Mtx( MTXID_CEVA_CV_SF_USER, D_CEVA_CV_USER_WAIT_TIME_ms );
	
	return (D_OS_USER_E_OK == retval) ? D_CEVA_CV_SF_USER_OK : D_CEVA_CV_SF_USER_NG;
}


/**
 * Unlock mutex
 *
 * @retval	D_RS_DSP_OK : Success
 * @retval	D_RS_DSP_NG : Failure
 */
int ceva_cv_sf_user_unl_mtx(void)
{
	OS_USER_ER retval;
	
	retval = OS_User_Unl_Mtx( MTXID_CEVA_CV_SF_USER );
	
	return (D_OS_USER_E_OK == retval) ? D_CEVA_CV_SF_USER_OK : D_CEVA_CV_SF_USER_NG;
}
