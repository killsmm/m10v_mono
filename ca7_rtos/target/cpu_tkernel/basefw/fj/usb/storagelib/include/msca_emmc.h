/**
 * @file        msca_emmc.h
 * @brief       USB Mass Storage class Media Access Function for eMMC
 * @note        None
 * @attention   None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef _MSCA_EMMC_h
#define _MSCA_EMMC_h

/* MODESENSE Response data */
#define MSCS_EM_ReadRetryCount        (3)
#define MSCS_EM_WriteRetryCount       (3)
#define MSCA_EM_TransferRate          (0x0FA0)    /* 4Mbps */


/* Prototype */
int MSCA_EM_Init(unsigned char driveno);
int MSCA_EM_Reset(void);
int MSCA_EM_CheckMedia(void);

int MSCA_EM_CheckWriteProtect0(void);
int MSCA_EM_CheckWriteProtect1(void);
int MSCA_EM_CheckWriteProtect2(void);
int MSCA_EM_CheckWriteProtect3(void);
int MSCA_EM_CheckWriteProtect4(void);
int MSCA_EM_CheckWriteProtect5(void);
int MSCA_EM_CheckWriteProtect6(void);

int MSCA_EM_LockMedia(void);
int MSCA_EM_UnlockMedia(void);
int MSCA_EM_MediaIntHandler(void);
int MSCA_EM_TaskTerm(void);

int MSCA_EM_GetDriveInfo0(MSCA_IDI*);
int MSCA_EM_GetDriveInfo1(MSCA_IDI*);
int MSCA_EM_GetDriveInfo2(MSCA_IDI*);
int MSCA_EM_GetDriveInfo3(MSCA_IDI*);
int MSCA_EM_GetDriveInfo4(MSCA_IDI*);
int MSCA_EM_GetDriveInfo5(MSCA_IDI*);
int MSCA_EM_GetDriveInfo6(MSCA_IDI*);
int MSCA_EM_ReadLBA0(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_ReadLBA1(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_ReadLBA2(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_ReadLBA3(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_ReadLBA4(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_ReadLBA5(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_ReadLBA6(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA0(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA1(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA2(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA3(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA4(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA5(unsigned long, unsigned char, unsigned char*);
int MSCA_EM_WriteLBA6(unsigned long, unsigned char, unsigned char*);
unsigned long MSCA_EM_Capacity0(void);
unsigned long MSCA_EM_Capacity1(void);
unsigned long MSCA_EM_Capacity2(void);
unsigned long MSCA_EM_Capacity3(void);
unsigned long MSCA_EM_Capacity4(void);
unsigned long MSCA_EM_Capacity5(void);
unsigned long MSCA_EM_Capacity6(void);


#endif	/* _MSCA_EMMC_h */
