/**
 * @file		NF_driver_wrapper.c
 * @brief		Nand Flash driver wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "mwnf.h"
#include "NF_driver_wrapper.h"
#include "ddim_typedef.h"
#include "debug.h"
#include "mdf_wrapper.h"
#include "fs_user_custom.h"
#include "nf_custom.h"
#include "media_drv_wrapper.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
volatile CHAR gNF_Drive_Name[NF_MAX_DRV_NUM][2] = {
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
};

volatile ULONG gNF_Drive_Protect[NF_MAX_DRV_NUM] = {
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive A not protect
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive B not protect
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive C not protect
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive D not protect
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive E not protect
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive F not protect
	E_BF_MEDIA_DRV_WRAPPER_WP_UNPROTECT,			// Drive G not protect
};

volatile NF_Device_Info NFdevInfo[NF_MAX_DRV_NUM];
volatile FS_Nformat_Info NFformatInfo[NF_MAX_DRV_NUM];

volatile ULONG gNF_Code_Size;
volatile UINT32  gNF_Drive_Num = D_NF_STORAGE_NUM;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/*------------------------------------------------------------------
    function : NF_InitDataDrive
    outline  : initialize A, B, C drive
    return   : error code
    argument : none
  ------------------------------------------------------------------*/
FW_INT32 NF_InitDataDrive(FW_VOID)
{
	INT32 ercd;
	UINT32 w_addr;
	T_NF_ATTRIBUTE nf_attr;

	BF_Fs_User_Custom_Set_NF_Port();

	ercd = MWNF_Init();
	if (ercd != 0) {
		BF_Debug_Print_Error(("MWNF_Init() failed: ercd=%d\n", ercd));
		return ercd;
	}

	nf_attr.ce_num             = D_NF_CE_1;
	nf_attr.device_block_num   = D_NF_DEVICE_BLOCK_NUM_4096;
	nf_attr.max_bad_block_num  = D_NF_MAX_BAD_BLOCK_NUM_96;
	nf_attr.search_other_page  = D_NF_SEARCH_OTHER_PAGE_1;
	nf_attr.dbcnt0_t0dc        = D_NF_DC_1;
	nf_attr.dbcnt0_t1dc        = D_NF_DC_1;
	nf_attr.dbcnt0_t2dc        = D_NF_DC_1;
	Media_Custom_NF_Set_Attribute(&nf_attr);

	w_addr = (UINT32)gMW_Exs_IRAM_NF;
	// initialize NF driver
	ercd = MWNF_BasicInit(w_addr, 1);
	if (ercd != 0) {
		BF_Debug_Print_Error(("MWNF_BasicInit() failed: ercd=%d\n", ercd));
		return ercd;
	}

	return 0;
}
