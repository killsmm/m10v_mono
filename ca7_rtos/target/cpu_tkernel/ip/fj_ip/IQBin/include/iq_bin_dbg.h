
#ifndef _IQ_BIN_DBG_H_
#define _IQ_BIN_DBG_H_

#include "ddim_typedef.h"
#include "share.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_IQ_DBG_DUMP_NONE = 0,
	E_IQ_DBG_DUMP_DPC,
	E_IQ_DBG_DUMP_ELF,
	E_IQ_DBG_DUMP_CIA,
	E_IQ_DBG_DUMP_HFA,
	E_IQ_DBG_DUMP_TC,
	E_IQ_DBG_DUMP_CCM1,
	E_IQ_DBG_DUMP_GAM,
	E_IQ_DBG_DUMP_EE,
	E_IQ_DBG_DUMP_CLPF,
	E_IQ_DBG_DUMP_CSCO,
	E_IQ_DBG_DUMP_CNR_OTF,
	E_IQ_DBG_DUMP_CNR_OFL,
	E_IQ_DBG_DUMP_CAG,
	E_IQ_DBG_DUMP_FSHD,
	E_IQ_DBG_DUMP_TBL,
	E_IQ_DBG_DUMP_MCC,
	E_IQ_DBG_DUMP_ALL,
} E_IQ_DBG_DUMP;	

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern INT32 IQDbg_Dump_IQ_DPC(void);
extern INT32 IQDbg_Dump_IQ_ELF(void);
extern INT32 IQDbg_Dump_IQ_CIA(void);
extern INT32 IQDbg_Dump_IQ_HFA(void);
extern INT32 IQDbg_Dump_IQ_TC(void);
extern INT32 IQDbg_Dump_IQ_CCM1(void);
extern INT32 IQDbg_Dump_IQ_GAM(void);
extern INT32 IQDbg_Dump_IQ_EE(void);
extern INT32 IQDbg_Dump_IQ_CLPF(void);
extern INT32 IQDbg_Dump_IQ_CSCO(void);
extern INT32 IQDbg_Dump_IQ_CNR_OTF(void);
extern INT32 IQDbg_Dump_IQ_CNR_OFL(void);
extern INT32 IQDbg_Dump_IQ_CAG(void);
extern INT32 IQDbg_Dump_IQ_FSHD(void);
extern INT32 IQDbg_Dump_IQ_TBL(void);
extern INT32 IQDbg_Dump_IQ_SetFlag(E_IQ_DBG_DUMP flag, UCHAR dol);
extern INT32 IQDbg_Dump_IQ_PrintOut(UCHAR stream, UCHAR dol);


#endif
