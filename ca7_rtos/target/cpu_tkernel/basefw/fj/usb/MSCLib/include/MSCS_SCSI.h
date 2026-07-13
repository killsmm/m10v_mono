/*!! Saved Date : 2008/08/08 9:41 !!*/
/************************************************************************/
/*	<Contents>		USB Mass Storage Sub-class header for SCSI			*/
/*	<Filename>		MSCS_SCSI.h											*/
/*	<Enviroment>	Softune V6 for FR-80								*/
/*	<Creation Date/author>												*/
/*					2001/07/06		N.Uenishi							*/
/*	<Latest modify Date/author>											*/
/*					2005/12/28		S.Hibino							*/
/*					2016/01/26		H.Hata								*/
/*	<Version>    0.1													*/
/*							Socionext Middleware Library				*/
/*								Copyright Socionext Inc. 2001-20016		*/
/************************************************************************/

#ifndef __MSCS_SCSI_H
#define __MSCS_SCSI_H

/************************/
/* Inquiry data format  */
/* Command:		12h		*/
/************************/
typedef struct
{
#ifdef MSC_BIG_ENDIAN
	Uchar	PDT;
	struct
	{
		Uchar RMB		:1;
		Uchar reserved	:7;
	} __attribute__((__packed__)) byte1;
	union ver
	{
		Uchar VERSION;
		struct
		{
			Uchar ISO	:2;
			Uchar ECMA	:3;
			Uchar ANSI	:3;
		} __attribute__((__packed__)) bit;
	} __attribute__((__packed__)) byte2 ;
	struct
	{
		Uchar AERC		:1;
		Uchar Obsolete	:1;
		Uchar NormACA	:1;
		Uchar HiSup		:1;
		Uchar RDF		:4;	/* Bugfix 2002.07.16 */
	} __attribute__((__packed__)) byte3;
	Uchar AddLength;
	struct
	{
		Uchar SCCS		:1;
		Uchar reserved	:7;
	} __attribute__((__packed__)) byte5;
	struct
	{
		Uchar BQue		:1;
		Uchar EncServ	:1;
		Uchar reserved	:1;
		Uchar MultiP	:1;
		Uchar MChngr	:1;
		Uchar Obsolete	:2;
		Uchar ADDR16	:1;
	} __attribute__((__packed__)) byte6;
	struct
	{
		Uchar RelAdr	:1;
		Uchar Obsolete1	:1;
		Uchar WBUS16	:1;
		Uchar SYNC		:1;
		Uchar LINKED	:1;
		Uchar Obsolete2	:1;
		Uchar CmdQue	:1;
		Uchar reserved	:1;
	} __attribute__((__packed__)) byte7;
	Uchar Vendor[8];
	Uchar Product[16];
	Uchar RevLvl[4];
	
#else /* MSC_BIG_ENDIAN */
/* Little Endian M-7 */
	Uchar	PDT;
	struct
	{
		Uchar reserved	:7;
		Uchar RMB		:1;
	} __attribute__((__packed__)) byte1;
	union ver
	{
		Uchar VERSION;
		struct
		{
			Uchar ANSI	:3;
			Uchar ECMA	:3;
			Uchar ISO	:2;
		} __attribute__((__packed__)) bit;
	} __attribute__((__packed__)) byte2 ;
	struct
	{
		Uchar RDF		:4;	/* Bugfix 2002.07.16 */
		Uchar HiSup		:1;
		Uchar NormACA	:1;
		Uchar Obsolete	:1;
		Uchar AERC		:1;
	} __attribute__((__packed__)) byte3;
	Uchar AddLength;
	struct
	{
		Uchar reserved	:7;
		Uchar SCCS		:1;
	} __attribute__((__packed__)) byte5;
	struct
	{
		Uchar ADDR16	:1;
		Uchar Obsolete	:2;
		Uchar MChngr	:1;
		Uchar MultiP	:1;
		Uchar reserved	:1;
		Uchar EncServ	:1;
		Uchar BQue		:1;
	} __attribute__((__packed__)) byte6;
	struct
	{
		Uchar reserved	:1;
		Uchar CmdQue	:1;
		Uchar Obsolete2	:1;
		Uchar LINKED	:1;
		Uchar SYNC		:1;
		Uchar WBUS16	:1;
		Uchar Obsolete1	:1;
		Uchar RelAdr	:1;
	} __attribute__((__packed__)) byte7;
	Uchar Vendor[10];
	Uchar Product[16];
	Uchar RevLvl[4];
#endif /* MSC_BIG_ENDIAN */
} __attribute__((__packed__)) MSCS_INQUIRY_DATA;	/* 36 bytes */


/****************************************/
/* Mode Sense data format	  			*/
/* Command:		1Ah	& 5Ah 				*/
/****************************************/
/* ModeSense(10)_0x5A Parameter Header */	/* see UFI command Spec */
typedef struct
{
#ifdef MSC_BIG_ENDIAN
	Ushort	Length;
	Uchar	MediumTypeCode;
	struct
	{
		Uchar WP		:1;
		Uchar reserved1	:2;
		Uchar DPOFUA	:1;
		Uchar reserved2	:4;
	} __attribute__((__packed__)) byte3;
	struct
	{
		Uchar reserved3	:7;
		Uchar LONGLBA	:1;
	} __attribute__((__packed__)) byte4;
	Uchar	Reserved4;
	Ushort	BLKDESCLENGTH;
	
#else /* MSC_BIG_ENDIAN */
/* Little Endian M-7 */
	Ushort	Length;
	Uchar	MediumTypeCode;
	struct
	{
		Uchar reserved2	:4;
		Uchar DPOFUA	:1;
		Uchar reserved1	:2;
		Uchar WP		:1;
	} __attribute__((__packed__)) byte3;
	struct
	{
		Uchar LONGLBA	:1;
		Uchar reserved3	:7;
	} __attribute__((__packed__)) byte4;
	Uchar	Reserved4;
	Ushort	BLKDESCLENGTH;
#endif /* MSC_BIG_ENDIAN */
} __attribute__((__packed__)) MSCS_MODE_DATA_HEADER_0x5A;	/* 8 bytes */	/* changed 2002.07.16 */

/* ModeSense(6)_1A Parameter Header */	/* see SPC-2 command Spec */
typedef struct
{
#ifdef MSC_BIG_ENDIAN
	Uchar	Length;
	Uchar	MediumTypeCode;
	struct
	{
		Uchar WP		:1;
		Uchar reserved1	:2;
		Uchar DPOFUA	:1;
		Uchar reserved2	:4;
	} __attribute__((__packed__)) byte2;
	Uchar	BLKDESCLENGTH;
	
#else /* MSC_BIG_ENDIAN */
/* Little Endian M-7 */
	Uchar	Length;
	Uchar	MediumTypeCode;
	struct
	{
		Uchar reserved2	:4;
		Uchar DPOFUA	:1;
		Uchar reserved1	:2;
		Uchar WP		:1;
	} __attribute__((__packed__)) byte2;
	Uchar	BLKDESCLENGTH;
#endif /* MSC_BIG_ENDIAN */
} __attribute__((__packed__)) MSCS_MODE_DATA_HEADER_0x1A;	/* 4 bytes */	/* add 2002.07.16 */

/* Direct-access device block descriptor format for LONGLBA=0 */
typedef struct
{
	Ulong	NumOfBlks;
	Ulong	BlockLength;	/* include DENSITY CODE */
} __attribute__((__packed__)) MSCS_MODE_BLKDESC;		/* 8 bytes */	/* add 2002.07.16 */

/* Read-Write Error Recovery Page */
typedef struct
{
#ifdef MSC_BIG_ENDIAN
	struct
	{
		Uchar PS		:1;
		Uchar reserved	:1;
		Uchar PageCode	:6;	/* 01h fixed */
	} __attribute__((__packed__)) byte0;
	Uchar PageLength;		/* 0Ah fixed */
	struct
	{				/* Error Recovery parameters */
		Uchar AWRE		:1;
		Uchar reserved1	:1;
		Uchar TB		:1;
		Uchar RC		:1;
		Uchar reserved2	:1;
		Uchar PER		:1;
		Uchar reserved3	:1;
		Uchar DCR		:1;
	} __attribute__((__packed__)) byte2;
	Uchar ReadRetryCount;
	Uchar Reserved1[4];
	Uchar WriteRetryCount;
	Uchar Reserved2[3];
	
#else /* MSC_BIG_ENDIAN */
/* Little Endian for M-7 */
	struct
	{
		Uchar PageCode	:6;	/* 01h fixed */
		Uchar reserved	:1;
		Uchar PS		:1;
	} __attribute__((__packed__)) byte0;
	Uchar PageLength;		/* 0Ah fixed */
	struct
	{				/* Error Recovery parameters */
		Uchar DCR		:1;
		Uchar reserved3	:1;
		Uchar PER		:1;
		Uchar reserved2	:1;
		Uchar RC		:1;
		Uchar TB		:1;
		Uchar reserved1	:1;
		Uchar AWRE		:1;
	} __attribute__((__packed__)) byte2;
	Uchar ReadRetryCount;
	Uchar Reserved1[4];
	Uchar WriteRetryCount;
	Uchar Reserved2[3];
#endif /* MSC_BIG_ENDIAN */
} __attribute__((__packed__)) MSCS_MODE_DATA_01;	/* 12 bytes */

#if 0
/* Flexible Disk Page */		/* see UFI command Spec */
typedef struct
{
	struct
	{
		Uchar PS	:1;
		Uchar reserved:1;
		Uchar PageCode:6;	/* 05h fixed */
	} byte0;
	Uchar PageLength;	/* 1Eh fixed */
	Ushort TransferRate;
	Uchar NumOfHeads;
	Uchar SectorPerTrack;
	Ushort BytePerSector;
	Ushort NumOfCylinders;
	Uchar Reserved1[9];
	Uchar MotorOnDelay;
	Uchar MotorOffDelay;
	Uchar Reserved2[7];
	Ushort RotationRate;
	Uchar Reserved3[2];
} MSCS_MODE_DATA_05;	/* 32 bytes */

/* Caching Page */		/* see SFF8070i spec */
typedef struct
{
	struct
	{
		Uchar PS		:1;
		Uchar reserved	:1;
		Uchar PageCode	:6;	/* 08h fixed */
	} byte0;
	Uchar PageLength;		/* 0Ah fixed */
	struct
	{
		Uchar reserved1	:5;
		Uchar WCE		:1;
		Uchar reserved2	:1;
		Uchar RCD		:1;
	} byte2;
	Uchar Reserved1[9];
} MSCS_MODE_DATA_08;	/* 12 bytes */

/* Removable Block Access Capabilities Page */		/* see UFI command Spec */
typedef struct
{
	struct
	{
		Uchar PS	:1;
		Uchar reserved:1;
		Uchar PageCode:6;	/* 1Bh fixed */
	} byte0;
	Uchar PageLength;	/* 0Ah fixed */
	struct
	{
		Uchar reserved1:1;
		Uchar SRFP	:1;
		Uchar reserved2:6;
	} byte2;
	struct
	{
		Uchar NCD	:1;
		Uchar SML	:1;
		Uchar reserved3:3;
		Uchar TLUN	:3;
	} byte3;
	Uchar Reserved1[8];
} MSCS_MODE_DATA_1B;	/* 12 bytes */

/* Timer & Protect Page */		/* see UFI command Spec */
typedef struct
{
	struct
	{
		Uchar PS	:1;
		Uchar reserved:1;
		Uchar PageCode:6;	/* 1Ch fixed */
	} byte0;
	Uchar PageLength;	/* 06h fixed */
	Uchar Reserved1;
	struct
	{
		Uchar reserved:4;
		Uchar ITM	:4;
	} byte3;
	struct
	{
		Uchar reserved:6;
		Uchar DISP	:1;
		Uchar SWPP	:1;
	} byte4;
	Uchar Reserved2[3];
} MSCS_MODE_DATA_1C;	/* 8 bytes */

/* RBC Device Parameters page */		/* see RBC command Spec */
typedef struct
{
	struct
	{
		Uchar PS	:1;
		Uchar reserved:1;
		Uchar PageCode:6;	/* 06h fixed */
	} byte0;
	Uchar PageLength;	/* 0Bh fixed */
	struct
	{
		Uchar reserved:7;
		Uchar WCD	:1;			/* Write cache disable 		*/
	} byte2;
	unsigned char LBS[2];		/* Logical block size = 512	*/
	unsigned char NumOfLB[5];	/* Number of Logical block 	*/
	unsigned char POWER;		/* Power/Perfomance 		*/
	struct
	{
		Uchar reserved:4;
		Uchar READD	:1;			/* Read disable				*/
		Uchar WRITED:1;			/* Write disable			*/
		Uchar FORMATD:1;		/* Format disable			*/
		Uchar LOCKD	:1;			/* lock disable				*/
	} byte11;
	Uchar Reserved;
} MSCS_MODE_DATA_06;
#endif

/****************************************/
/* Read Format data format  			*/
/* Command:		25h						*/
/****************************************/
typedef struct
{
	Ulong NumOfBlks;
	Ulong BlockLength;
} __attribute__((__packed__)) MSCS_READ_CAPACITIES_DATA;

/****************************************/
/* Read Format Capacities data format   */
/* Command:		23h						*/
/****************************************/
#if 0 /* M-7 */
typedef struct
{
	Uchar Reserved1[3];
	Uchar ListLength;
	struct
	{
		Ulong NumOfBlks;
		Ulong DescriptorCode:8;
		Ulong BlockLength	:24;
	} Max;
	struct
	{
		Ulong NumOfBlks;
		Ulong reserved		:8;
		Ulong BlockLength	:24;
	} Formattable;
} MSCS_READ_FORMAT_CAPACITIES_DATA;
#endif

/****************************************/
/* Request Sense data format			*/
/* Command:		03h						*/
/****************************************/
typedef struct
{
	Uchar ErrorCode;
	Uchar Reserved1;
	Uchar SenseKey;
	Uchar Info[4];
	Uchar AddSenseLength;
	Uchar Reserved2[4];
	Uchar ASC;
	Uchar ASCQ;
	Uchar Reserved3[4];
} __attribute__((__packed__)) MSCS_REQUEST_SENSE;	/* 18 bytes */


/******************/
/* Command Number */
/******************/
typedef enum
{
	Cmd_Inquiry			= 0x12 ,
	Cmd_ModeSense_0x1A	= 0x1A ,
	Cmd_ModeSense_0x5A	= 0x5A ,
	Cmd_PreventAllowMediumRemoval = 0x1E ,
	Cmd_Read			= 0x28 ,
	Cmd_ReadCapacity	= 0x25 ,
	Cmd_RequestSense	= 0x03 ,
	Cmd_StartStop		= 0x1B ,
	Cmd_TestUnitReady	= 0x00 ,
	Cmd_Verify			= 0x2F ,
	Cmd_Write			= 0x2A 
} MSCS_CMD_SCSI;

/***************************/
/* Command function number */
/***************************/
typedef enum
{
	N_Cmd_Inquiry			= 0 ,
	N_Cmd_ModeSense_0x1A	= 1 ,
	N_Cmd_ModeSense_0x5A	= 2 ,
	N_Cmd_PreventAllowMediumRemoval = 3 ,
	N_Cmd_Read			= 4 ,
	N_Cmd_ReadCapacity	= 5 ,
	N_Cmd_RequestSense	= 6 ,
	N_Cmd_StartStop		= 7 ,
	N_Cmd_TestUnitReady	= 8 ,
	N_Cmd_Verify		= 9 ,
	N_Cmd_Write			= 10 ,
	N_Cmd_Last			= 11
} MSCS_CMD_NUM;

/***************************/
/* Media Drive information */
/***************************/
typedef struct
{
/* offset*/
/* 	0	*/	Uchar				MSCS_DriveNo;
/*	1	*/	Uchar				MSCS_ReadRetryCount;/* used by ModeSense command */
/*	2	*/	Uchar				MSCS_WriteRetryCount;/* used by ModeSense command */
/*	3	dummy */
/*	4-5	*/	Ushort				MSCA_TransferRate;	/* used by ModeSense command */
/*	6	*/	Uchar				MSCS_MEDIUM_REMOVAL;/* 0:Medium allow removal 	, 1:Medium prevent removal */
/*	7	*/	Uchar				MSCS_MEDIUM_EJECT;	/* 0:No Action 				, 1:Medium Eject(if possible) */
/*	8-25*/	MSCS_REQUEST_SENSE 	MSCS_SENSEdata;		/* Sense key */
/*  26,27*/	Uchar				Dummy[2];			/*  Dummy  */
/* 28-63*/	MSCS_INQUIRY_DATA	MSCS_INQUIRYdata;	/* INQUIRY response data */
	/* Mode Page data( usesd for MODE_SENSE,SELECT_SENSE command) */
/* 64-75*/	MSCS_MODE_DATA_01	MSCS_MODEdata01[1];	/* Read-Write Error Recovery Page */
/* 76	 */	Uchar				MSCS_MEDIUM_CHANGED;
/* 77-79	dummy */
/* 80-91 */	MSCA_IDI			MSCA_IDIdata;
/* Total 92 bytes */
} MSCS_DriveInfo_t;

/*********************************/
/* Function Prototype definition */
/*********************************/
/* Mass Storage Sub-Class SCSI Command Function */
extern int  MSCS_Cmd_Inquiry(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_ModeSelect(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_ModeSense_0x1A(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_ModeSense_0x5A(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_PreventAllowMediumRemoval(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_Read(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_ReadCapacity(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_RequestSense(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_StartStop(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_TestUnitReady(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_Verify(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_Write(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_Cmd_Invalid(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *MSCS_DriveInfo, Ulong *residue);
extern int  MSCS_ExeCmd(MSCP_CBW* CBWdata, Ulong* residue);
extern void MSCS_AllSetBecomeReady(void); // rename 2010.11.23 MSCS_SetAllBecomeReady -> MSCS_AllSetBecomeReady
#endif

