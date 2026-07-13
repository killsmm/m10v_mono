/**
 *
 * @file		user_kernel_index.h
 * @brief		User implementation
 * @author		H.Maeda (SNI)
 * @version		V00L01R00
 * @since		2016/2/8
 * @note
 * @attention	
 * @Copyright	Copyright (C) 2016 SOCIONEXT INC. All Rights Reserved.
 *
 */


#ifndef _USER_KERNEL_INDEX_H_
#define _USER_KERNEL_INDEX_H_

#include "CEVA_CV_SmartFrame_common.h"

/*----------------------------------------------------------------------*/
/* User Kernel Index [0~65535]											*/
/*----------------------------------------------------------------------*/

#define USER_SAMPLE_BIN_INDEX	( (sf_internal_kernel_e)((unsigned int)USER_BASE_INDEX | 0) )

#endif // _USER_KERNEL_INDEX_H_
