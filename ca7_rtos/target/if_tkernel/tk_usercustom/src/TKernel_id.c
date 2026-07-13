/**
 * @file		TKernel_id.c
 * @brief		T-Kernel resouce id
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
 
#include "TKernel_id.h"

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* MBA I/F Array */
unsigned char gTaskCreated[TID_NUM];
const char* gTaskName[TID_NUM];

/* Task Array */
int gTaskID[TID_NUM];

/* Semaphore Array */
int gSemID[SID_NUM];

/* Eventflag Array */
int gFlagID[FID_NUM];

/* Mutex Array */
int gMtxID[MTXID_NUM];

/* Variable size memory pool Array */
int gMemPoolID[MPKID_NUM];

/* Fixed memory pool Array */
int gMemPoolFixedID[MPFID_NUM];

/* Mailboxes Array */
int gMailBoxID[MID_NUM];

//int gMBufferID[MBFID_NUM];
