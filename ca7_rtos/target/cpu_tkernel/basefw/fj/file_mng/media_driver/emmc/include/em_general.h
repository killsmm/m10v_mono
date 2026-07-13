// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_general.h
* @brief Host handling functions definition
* @note None
* @attention None
*
*/
/*****************************************************************************/

#ifndef EM_GENERAL_H_INCLUDE
#define EM_GENERAL_H_INCLUDE


// include -------------------------------------
#include "em_types.h"



// struct --------------------------------------
/*****************************************************************************/
/**
 * @struct EM_CONFIG
 * @brief Configuration information
 */
/*****************************************************************************/
typedef struct EM_CONFIG
{
	unsigned long ul_BaseAddress;		 //!< base address of registers
	unsigned long* descLogAddress;		 //!< logical address of descriptors
}  EM_CONFIG;


// function --------------------------------------
extern unsigned long em_init(
	EM_HOST* pHost,
	EM_CONFIG *pConfig
);

extern unsigned long em_start(
	EM_HOST* pHost
);

extern void em_end(
	EM_HOST* pHost
);

extern unsigned long em_InterruptHandler(
	EM_HOST *pHost,
	unsigned long ul_start_ms,
	unsigned long ul_timeout_value
);


#endif
