/**
 * @file		mw_xdmac.h
 * @brief		Header of XDMAC Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MW_XDMAC_H_
#define _MW_XDMAC_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_MW_XDMAC_RETVAL_OK				(0)
#define	D_MW_XDMAC_RETVAL_ERR_USE_XDMAC		(-1)
#define	D_MW_XDMAC_RETVAL_ERR_TIMEOUT		(-2)

#define D_MW_XDMAC_OFF	(0)
#define D_MW_XDMAC_ON	(1)

#define D_MW_XDMAC_CH_PRIORITY_FIXED	(0)
#define D_MW_XDMAC_CH_PRIORITY_ROTATED	(1)

#define D_MW_XDMAC_BS_1BYTE		(0)
#define D_MW_XDMAC_BS_2BYTE		(1)
#define D_MW_XDMAC_BS_4BYTE		(2)
#define D_MW_XDMAC_BS_8BYTE		(3)

#define D_MW_XDMAC_BL_1			(0)
#define D_MW_XDMAC_BL_2			(1)
#define D_MW_XDMAC_BL_3			(2)
#define D_MW_XDMAC_BL_4			(3)
#define D_MW_XDMAC_BL_5			(4)
#define D_MW_XDMAC_BL_6			(5)
#define D_MW_XDMAC_BL_7			(6)
#define D_MW_XDMAC_BL_8			(7)
#define D_MW_XDMAC_BL_9			(8)
#define D_MW_XDMAC_BL_10		(9)
#define D_MW_XDMAC_BL_11		(10)
#define D_MW_XDMAC_BL_12		(11)
#define D_MW_XDMAC_BL_13		(12)
#define D_MW_XDMAC_BL_14		(13)
#define D_MW_XDMAC_BL_15		(14)
#define D_MW_XDMAC_BL_16		(15)

#define D_MW_XDMAC_IS_NONE			(0)
#define D_MW_XDMAC_IS_ERR_DSTP		(1)
#define D_MW_XDMAC_IS_ERR_SW_STOP	(2)
#define D_MW_XDMAC_IS_ERR_SRC		(4)
#define D_MW_XDMAC_IS_ERR_DSR		(5)
#define D_MW_XDMAC_IS_ERR_D_CHAIN	(6)
#define D_MW_XDMAC_IS_NORMAL_END	(8)

#define D_MW_XDMAC_TF_SW			(0x1)
#define D_MW_XDMAC_TF_DREQ_CF		(0xF)

#define D_MW_XDMAC_AT_SRC		(0x0)
#define D_MW_XDMAC_AT_DST		(0x1)

/*----------------------------------------------------------------------*/
/* Structure   															*/
/*----------------------------------------------------------------------*/
typedef struct {
	unsigned int 	SrcStartAddr;
	unsigned char 	SrcBurstSize;
	unsigned char 	SrcBurstLength;
	unsigned char 	SrcAddrFix;
	unsigned char	SrcAddrReload;
	
	unsigned int 	DstStartAddr;
	unsigned char 	DstBurstSize;
	unsigned char 	DstBurstLength;
	unsigned char 	DstAddrFix;
	unsigned char	DstAddrReload;
	
	unsigned int 	TransferByteCount;
	unsigned char	TransferByteCountReload;
	
	unsigned char 	TransferFactor;
	unsigned char 	SerialAccess;
	unsigned char	BlockTransfer;
	unsigned char	DACK_AssertTiming;
	
	unsigned char	ErrorInt;
	unsigned char	TerminationInt;
	void			(*Callback)( unsigned char, unsigned short*);
	
	unsigned int	D_ChainAddr;
} T_MW_XDMAC_CTRL;


/*----------------------------------------------------------------------*/
/* API      															*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

extern void MW_XDMAC_Get_XE_bit( unsigned char *XE );
extern void MW_XDMAC_Global_OnOff( int OnOff );
extern void MW_XDMAC_Global_Ctrl( int ChannelPri, int LowPower );
extern int MW_XDMAC_Open( int ch, int TimeOut );
extern void MW_XDMAC_Close( int ch );
extern void MW_XDMAC_Ctrl( int ch, T_MW_XDMAC_CTRL *cfg );
extern void MW_XDMAC_Start( int ch );
extern void MW_XDMAC_Stop( int ch );

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _MW_XDMAC_H_
