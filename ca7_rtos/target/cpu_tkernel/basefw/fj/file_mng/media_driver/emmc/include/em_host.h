// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_host.h
* @brief Host controller definition
* @note None
* @attention None
*
*/
/*****************************************************************************/

#ifndef EM_HOST_H_INCLUDE
#define EM_HOST_H_INCLUDE


// include -------------------------------------
#include "em_types.h"
#include "em_general.h"


// function -------------------------------------
extern unsigned long em_configAccessMode(
	EM_SLOT *pSlot,
	EM_ACCESS_MODE *e_AccessMode
);

extern unsigned char em_checkErrorState(
	EM_SLOT* pSlot,
	unsigned long ul_srs12
);

extern unsigned long em_checkInterrupt(
	EM_SLOT* pSlot,
	unsigned long ul_srs12
);

extern unsigned long em_deviceAttach(
	EM_SLOT* pSlot
);

extern void em_recoverRegister(
	EM_SLOT *pSlot
) ;

extern unsigned long em_readCSD_Proc(
	EM_SLOT* pSlot,
	unsigned long *pBuffer
);

extern unsigned long  em_readExCSD_Proc(
	EM_SLOT* pSlot,
	unsigned char *pBuffer
);

extern unsigned long em_resetSlot(
	EM_SLOT* pSlot
);

extern unsigned long em_setPartAccess(
	EM_SLOT* pSlot,
	EM_PARTITION_ACCESS e_Partition
);

extern unsigned char em_getResponse(
	EM_REQUEST_PARAM* pRequest,
	EM_SLOT* pSlot
);

extern unsigned long em_initializeSlot(
	EM_SLOT* pSlot,
	EM_HOST* pHost
);

extern unsigned long em_waitForValue(
	volatile unsigned long *pAddress,
	unsigned long ul_Mask,
	unsigned char uc_set,
	unsigned long ul_timeout_ms
);

extern unsigned long em_memExtCsdInfo(
	EM_SLOT* pSlot
);

extern unsigned long em_setExtendsCSDInfo(
	EM_SLOT *pSlot,
	unsigned long ul_byte,
	unsigned char uc_value
);

#endif

