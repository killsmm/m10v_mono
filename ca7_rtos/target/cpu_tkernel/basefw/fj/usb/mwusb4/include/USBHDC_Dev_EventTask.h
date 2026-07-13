/**
 * @file		USBHDC_Dev_EventTask.h
 * @brief		This is a header file for USB Device event task function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_EVENTTASK
#define H_USBHDC_DEV_EVENTTASK

#include "USBHDC_StdDef.h"


/* function prototype declaration */
/* event task function when phy error occurred. */
extern void Dev_PhyErrorTask(void);

/* event task function */
extern void Dev_DmaIntTask(u8 dma_ch);

/* event task function when bus reset beginning interrupt is generated */
extern void Dev_BusResetBeginTask(void);

/* event task function when bus reset end interrupt is generated */
extern void Dev_BusResetEndTask(void);

/* event task function when setup transaction start interrupt is generated */
extern void Dev_SetupTransactionStartTask(void);

/* event task function when status transaction normal end interrupt is generated */
extern void Dev_StatusTransactionNormalEndTask(void);

/* event task function when status transaction failure end interrupt is generated */
extern void Dev_StatusTransactionFailEndTask(void);

/* event task function when SOF interrupt is generated */
extern void Dev_SofTask(void);

/* event task function when suspend beginning interrupt is generated */
extern void Dev_SuspendBeginTask(void);

/* event task function when suspend end interrupt is generated */
extern void Dev_SuspendEndTask(void);

/* event task function when endpoint stall clear interrupt is generated */
extern void Dev_EpxClearStallTask(u8 ep);

/* event task function when NACK packet sending interrupt is generated */
extern void Dev_EpxNackTask(u8 ep);

/* event task function when endpoint stall interrupt is generated */
extern void Dev_EpxStalledTask(u8 ep);

/* event task function when PING interrupt is generated */
extern void Dev_EpxPingTask(u8 ep);

/* event task function when endpoint empty interrupt is generated */
extern void Dev_EpxEmptyTask(u8 ep);

/* event task function when control IN transaction end interrupt is generated */
extern void Dev_CntlInTransactionEndTask(void);

/* event task function when control OUT transaction end interrupt is generated */
extern void Dev_CntlOutTransactionEndTask(void);

/* event task function when endpoint transaction end interrupt is generated */
extern void Dev_EpxTransactionEndTask(u8 ep);

/* set dma end state */
extern void Dev_SetDmaEndState(u8 dma_ch, u8 dma_state);


#endif
