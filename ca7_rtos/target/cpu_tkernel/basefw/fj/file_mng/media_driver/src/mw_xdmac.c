/**
 * @file		mw_xdmac.c
 * @brief		XDMAC Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "arm.h"
#include "mw_xdmac.h"
#include "dd_arm.h"
#include "dd_xdmac.h"
#include "os_user_custom.h"
#include "ddim_typedef.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition 															*/
/*----------------------------------------------------------------------*/
#define	D_MW_XDMAC_MAX_CH	(4)


/*----------------------------------------------------------------------*/
/* Global variables														*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* Function        														*/
/*----------------------------------------------------------------------*/

VOID MW_XDMAC_Global_Ctrl( INT32 PriorityCtrl, INT32 LowPowerCtrl )
{
	T_DD_XDMAC_COMMON common;
	UCHAR enable;
	
	MW_XDMAC_Get_XE_bit(&enable);
	common.common_config.bit.CP = PriorityCtrl;
	common.common_config.bit.XE = enable;
	Dd_XDMAC_Ctrl_Common(&common);
	Dd_XDMAC_Set_LowPower(LowPowerCtrl);
}

VOID MW_XDMAC_Get_XE_bit( UCHAR *XE )
{
	USHORT dummy1;
	USHORT dummy2;
	
	Dd_XDMAC_Get_Status(0, (USHORT*)XE, &dummy1, &dummy2);
}

VOID MW_XDMAC_Global_OnOff( INT32 OnOff )
{
	T_DD_XDMAC_COMMON common;
	
	common.common_config.bit.CP = D_DD_XDMAC_XDACS_CP_FIXED;
	common.common_config.bit.XE = OnOff;
	
	// Enable XDMAC
	Dd_XDMAC_Ctrl_Common(&common);
}

INT32 MW_XDMAC_Open( INT32 ch, INT32 TimeOut )
{
	return	Dd_XDMAC_Open((UCHAR)ch, TimeOut);
}

VOID MW_XDMAC_Close( INT32 ch )
{
	Dd_XDMAC_Close((UCHAR)ch);
}

VOID MW_XDMAC_Ctrl( INT32 ch, T_MW_XDMAC_CTRL *cfg )
{
	T_DD_XDMAC_CTRL ctrl;
	
	ctrl.trns_size = cfg->TransferByteCount;
	ctrl.src_addr = cfg->SrcStartAddr;
	ctrl.dst_addr = cfg->DstStartAddr;
	ctrl.config_1.bit.SBS = cfg->SrcBurstSize;
	ctrl.config_1.bit.DBS = cfg->DstBurstSize;
	ctrl.config_1.bit.SRL = cfg->SrcAddrReload;
	ctrl.config_1.bit.SAF = cfg->SrcAddrFix;
	ctrl.config_1.bit.SBL = cfg->SrcBurstLength;
	ctrl.config_1.bit.DRL = cfg->DstAddrReload;
	ctrl.config_1.bit.DAF = cfg->DstAddrFix;
	ctrl.config_1.bit.DBL = cfg->DstBurstLength;
	ctrl.config_1.bit.TI = cfg->TerminationInt;
	ctrl.config_1.bit.EI = cfg->ErrorInt;
	ctrl.config_1.bit.AT = cfg->DACK_AssertTiming;
	ctrl.config_1.bit.BR = cfg->TransferByteCountReload;
	ctrl.config_1.bit.BT = cfg->BlockTransfer;
	ctrl.config_1.bit.SA = cfg->SerialAccess;
	ctrl.config_1.bit.TF = cfg->TransferFactor;
	ctrl.int_handler = (VP_CALLBACK)cfg->Callback;

	if ( D_MW_XDMAC_TF_SW == cfg->TransferFactor ) {
		ctrl.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
	}
	else {
		ctrl.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_DISABLE;
	}
	
	if ( 0x0 != cfg->D_ChainAddr ) {
		ctrl.config_2.bit.DCN = 1;
		ctrl.config_2.bit.DCA = cfg->D_ChainAddr;
	}
	else {
		ctrl.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
		ctrl.config_2.bit.DCA = 0;
	}
	
	if ( 0x0 != cfg->D_ChainAddr ) {
		IO_XDMAC.CH[ch].XDDCC = (cfg->D_ChainAddr | 0x1);	// // Enable D_Chain
	}
	else {
		IO_XDMAC.CH[ch].XDDCC = 0x0;	// Disable D_Chain
	}
	
	Dd_XDMAC_Ctrl_Trns(ch, &ctrl);
}

VOID MW_XDMAC_Start( INT32 ch )
{
	Dd_XDMAC_Start_Async(ch);
}

VOID MW_XDMAC_Stop( INT32 ch )
{
	Dd_XDMAC_Stop(ch);
}

