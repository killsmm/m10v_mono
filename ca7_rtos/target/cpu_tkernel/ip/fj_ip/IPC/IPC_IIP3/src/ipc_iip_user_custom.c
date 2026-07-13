/**
 * @file		ipc_ii_user_custom.c
 * @brief		IPC_IIP Header
 * @author		hata
 * @since		2014/04/10
 * @date		
 * @note		IPC_IIP User Program
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2015</I></B>
 */

#include <string.h>
#include "ipc_if.h"
#include "../include/ipc_iip_user_custom.h"
#include "fj_iip_resource.h"
#include "im_iip.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define D_IPC_IIP_USER_FLAGS_32		(32)
#define D_IPC_IIP_USER_FLAGS_64		(64)

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/
#define D_IPC_IIP_USER_WAIT_TIME	(100)

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/
typedef struct {
	IPC_UINT		ipc;
	IPC_UINT		im;
} T_IPC_IIP_USER_IPC2IM_TBL;

typedef struct {
	IPC_LLONG		ipc;
	IPC_LLONG		im;
} T_IPC_IIP_USER_IPC2IM_TBL8;

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Local Method Definition																*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Table																				*/
/*--------------------------------------------------------------------------------------*/
T_IPC_IIP_USER_IPC2IM_TBL8 ipc_iip_user_unitid_flags_tbl[E_IPC_IIP_UNIT_ID_MAX] = {
	{ D_IPC_IIP_UNIT_FLG_1D0,      D_IM_IIP_PARAM_PLDUNIT_LD0    },
	{ D_IPC_IIP_UNIT_FLG_1D1,      D_IM_IIP_PARAM_PLDUNIT_LD1    },
	{ D_IPC_IIP_UNIT_FLG_1D2,      D_IM_IIP_PARAM_PLDUNIT_LD2    },
	{ D_IPC_IIP_UNIT_FLG_1D3,      D_IM_IIP_PARAM_PLDUNIT_LD3    },
	{ D_IPC_IIP_UNIT_FLG_1D4,      D_IM_IIP_PARAM_PLDUNIT_LD4    },
	{ D_IPC_IIP_UNIT_FLG_1D5,      D_IM_IIP_PARAM_PLDUNIT_LD5    },
	{ D_IPC_IIP_UNIT_FLG_1D6,      D_IM_IIP_PARAM_PLDUNIT_LD6    },
	{ D_IPC_IIP_UNIT_FLG_1D7,      D_IM_IIP_PARAM_PLDUNIT_LD7    },
	{ D_IPC_IIP_UNIT_FLG_FLT,      D_IM_IIP_PARAM_PLDUNIT_FLT    },
	{ D_IPC_IIP_UNIT_FLG_AFN0,     D_IM_IIP_PARAM_PLDUNIT_AFN0   },
	{ D_IPC_IIP_UNIT_FLG_AFN1,     D_IM_IIP_PARAM_PLDUNIT_AFN1   },
	{ D_IPC_IIP_UNIT_FLG_AFN2,     D_IM_IIP_PARAM_PLDUNIT_AFN2   },
	{ D_IPC_IIP_UNIT_FLG_AFN3,     D_IM_IIP_PARAM_PLDUNIT_AFN3   },
	{ D_IPC_IIP_UNIT_FLG_FRECT0,   D_IM_IIP_PARAM_PLDUNIT_FRECT0 },
	{ D_IPC_IIP_UNIT_FLG_FRECT1,   D_IM_IIP_PARAM_PLDUNIT_FRECT1 },
	{ D_IPC_IIP_UNIT_FLG_SL0,      D_IM_IIP_PARAM_PLDUNIT_SL0    },
	{ D_IPC_IIP_UNIT_FLG_SL1,      D_IM_IIP_PARAM_PLDUNIT_SL1    },
	{ D_IPC_IIP_UNIT_FLG_SL2,      D_IM_IIP_PARAM_PLDUNIT_SL2    },
	{ D_IPC_IIP_UNIT_FLG_SL3,      D_IM_IIP_PARAM_PLDUNIT_SL3    },
	{ D_IPC_IIP_UNIT_FLG_SL4,      D_IM_IIP_PARAM_PLDUNIT_SL4    },
	{ D_IPC_IIP_UNIT_FLG_SL5,      D_IM_IIP_PARAM_PLDUNIT_SL5    },
	{ D_IPC_IIP_UNIT_FLG_SL6,      D_IM_IIP_PARAM_PLDUNIT_SL6    },
	{ D_IPC_IIP_UNIT_FLG_SL7,      D_IM_IIP_PARAM_PLDUNIT_SL7    },
	{ D_IPC_IIP_UNIT_FLG_SL8,      D_IM_IIP_PARAM_PLDUNIT_SL8    },
	{ D_IPC_IIP_UNIT_FLG_CSC0,     D_IM_IIP_PARAM_PLDUNIT_CSC0   },
	{ D_IPC_IIP_UNIT_FLG_CSC1,     D_IM_IIP_PARAM_PLDUNIT_CSC1   },
	{ D_IPC_IIP_UNIT_FLG_CSC2,     D_IM_IIP_PARAM_PLDUNIT_CSC2   },
	{ D_IPC_IIP_UNIT_FLG_CSC3,     D_IM_IIP_PARAM_PLDUNIT_CSC3   },
	{ D_IPC_IIP_UNIT_FLG_LUT,      D_IM_IIP_PARAM_PLDUNIT_LUT    },
	{ D_IPC_IIP_UNIT_FLG_GPC,      D_IM_IIP_PARAM_PLDUNIT_GPC    },
	{ D_IPC_IIP_UNIT_FLG_BLEND0,   D_IM_IIP_PARAM_PLDUNIT_BLEND0 },
	{ D_IPC_IIP_UNIT_FLG_BLEND1,   D_IM_IIP_PARAM_PLDUNIT_BLEND1 },
	{ D_IPC_IIP_UNIT_FLG_MON,      D_IM_IIP_PARAM_PLDUNIT_MON    },
	{ D_IPC_IIP_UNIT_FLG_CFC,      D_IM_IIP_PARAM_PLDUNIT_CFL0   },
	{ D_IPC_IIP_UNIT_FLG_MFT,      D_IM_IIP_PARAM_PLDUNIT_MFT    }
};

T_IPC_IIP_USER_IPC2IM_TBL ipc_iip_user_pixid_flags_tbl[E_IPC_IIP_PIXID_MAX] = {
	{ D_IPC_IIP_PIXID_FLG_0,       E_IM_IIP_PIXID_0  },
	{ D_IPC_IIP_PIXID_FLG_1,       E_IM_IIP_PIXID_1  },
	{ D_IPC_IIP_PIXID_FLG_2,       E_IM_IIP_PIXID_2  },
	{ D_IPC_IIP_PIXID_FLG_3,       E_IM_IIP_PIXID_3  },
	{ D_IPC_IIP_PIXID_FLG_4,       E_IM_IIP_PIXID_4  },
	{ D_IPC_IIP_PIXID_FLG_5,       E_IM_IIP_PIXID_5  },
	{ D_IPC_IIP_PIXID_FLG_6,       E_IM_IIP_PIXID_6  },
	{ D_IPC_IIP_PIXID_FLG_7,       E_IM_IIP_PIXID_7  },
	{ D_IPC_IIP_PIXID_FLG_8,       E_IM_IIP_PIXID_8  },
	{ D_IPC_IIP_PIXID_FLG_9,       E_IM_IIP_PIXID_9  },
	{ D_IPC_IIP_PIXID_FLG_10,      E_IM_IIP_PIXID_10 },
	{ D_IPC_IIP_PIXID_FLG_11,      E_IM_IIP_PIXID_11 }
};

T_IPC_IIP_USER_IPC2IM_TBL ipc_iip_user_other_flags_tbl[E_IPC_IIP_RES_MAX] = {
	{ D_IPC_IIP_OPEN_RES_LUT_A,    D_FJ_IIP_RES_OTHER_LUT_A          },
	{ D_IPC_IIP_OPEN_RES_LUT_B,    D_FJ_IIP_RES_OTHER_LUT_B          },
	{ D_IPC_IIP_OPEN_RES_LUT_C,    D_FJ_IIP_RES_OTHER_LUT_C          },
	{ D_IPC_IIP_OPEN_RES_LUT_D,    D_FJ_IIP_RES_OTHER_LUT_D          },
	{ D_IPC_IIP_OPEN_RES_LUT_E,    D_FJ_IIP_RES_OTHER_LUT_E          },
	{ D_IPC_IIP_OPEN_RES_LUT_F,    D_FJ_IIP_RES_OTHER_LUT_F          },
	{ D_IPC_IIP_OPEN_RES_HIST0,    D_FJ_IIP_RES_OTHER_HIST_0         },
	{ D_IPC_IIP_OPEN_RES_HIST1,    D_FJ_IIP_RES_OTHER_HIST_1         },
	{ D_IPC_IIP_OPEN_RES_HIST2,    D_FJ_IIP_RES_OTHER_HIST_2         },
	{ D_IPC_IIP_OPEN_RES_HIST3,    D_FJ_IIP_RES_OTHER_HIST_3         },
	{ D_IPC_IIP_OPEN_RES_BICUBIC,  D_FJ_IIP_RES_OTHER_BICUBIC_0_OR_1 },
	{ D_IPC_IIP_OPEN_RES_BICUBIC0, D_FJ_IIP_RES_OTHER_BICUBIC_0      },
	{ D_IPC_IIP_OPEN_RES_BICUBIC1, D_FJ_IIP_RES_OTHER_BICUBIC_1      }
};

/*--------------------------------------------------------------------------------------*/
/* Function  																			*/
/*--------------------------------------------------------------------------------------*/
static IPC_UINT ipc_iip_user_ipc2im_tbl( IPC_UINT ipc, T_IPC_IIP_USER_IPC2IM_TBL* tbl, IPC_UINT size )
{
	IPC_INT i;
	
	for( i=0; i<size; i++ ){
		if( ipc == tbl[i].ipc ){
			return tbl[i].im;
		}
	}
	return 0;
}


static IPC_LLONG ipc_iip_user_ipc2im_tbl8( IPC_LLONG ipc, T_IPC_IIP_USER_IPC2IM_TBL8* tbl, IPC_UINT size )
{
	IPC_INT i;
	
	for( i=0; i<size; i++ ){
		if( ipc == tbl[i].ipc ){
			return tbl[i].im;
		}
	}
	return 0;
}


static IPC_UINT ipc_iip_user_im2ipc_tbl( IPC_UINT im, T_IPC_IIP_USER_IPC2IM_TBL* tbl, IPC_UINT size )
{
	IPC_INT i;
	
	for( i=0; i<size; i++ ){
		if( im == tbl[i].im ){
			return tbl[i].ipc;
		}
	}
	return 0;
}


static IPC_LLONG ipc_iip_user_im2ipc_tbl8( IPC_LLONG im, T_IPC_IIP_USER_IPC2IM_TBL8* tbl, IPC_UINT size )
{
	IPC_INT i;
	
	for( i=0; i<size; i++ ){
		if( im == tbl[i].im ){
			return tbl[i].ipc;
		}
	}
	return 0;
}


static IPC_VOID ipc_iip_user_unitid_flags_im2ipc( IPC_LLONG* out_flags, IPC_LLONG in_flags )
{
	IPC_INT  i;
	IPC_LLONG match_flags;
	IPC_LLONG search_flag;
	
	match_flags = 0;
	
	for( i=0; i<D_IPC_IIP_USER_FLAGS_64; i++ ){			/* E_IPC_IIP_UNIT_ID_MAX */
		search_flag = ( in_flags & (1ULL<<i) );
		match_flags |= ipc_iip_user_im2ipc_tbl8( search_flag, ipc_iip_user_unitid_flags_tbl, E_IPC_IIP_UNIT_ID_MAX );
	}
	
	*out_flags = match_flags;
}


static IPC_VOID ipc_iip_user_pixid_flags_im2ipc( IPC_UINT* out_flags, IPC_UINT in_flags )
{
	IPC_INT  i;
	IPC_UINT match_flags;
	IPC_UINT search_flag;
	
	match_flags = 0;
	
	for( i=0; i<D_IPC_IIP_USER_FLAGS_32; i++ ){				/* E_IPC_IIP_PIXID_MAX */
		search_flag = ( in_flags & (1<<i) );
		match_flags |= ipc_iip_user_im2ipc_tbl( search_flag, ipc_iip_user_pixid_flags_tbl, E_IPC_IIP_PIXID_MAX );
	}
	
	*out_flags = match_flags;
}


static IPC_VOID ipc_iip_user_other_flags_im2ipc( IPC_UINT* out_flags, IPC_UINT in_flags )
{
	IPC_INT  i;
	IPC_UINT match_flags;
	IPC_UINT search_flag;
	
	match_flags = 0;
	
	for( i=0; i<D_IPC_IIP_USER_FLAGS_32; i++ ){				/* E_IPC_IIP_RES_MAX */
		search_flag = ( in_flags & (1<<i) );
		match_flags |= ipc_iip_user_im2ipc_tbl( search_flag, ipc_iip_user_other_flags_tbl, E_IPC_IIP_RES_MAX );
	}
	
	*out_flags = match_flags;
}


static IPC_VOID ipc_iip_user_unitid_flags_ipc2im( IPC_LLONG* out_flags, IPC_LLONG in_flags )
{
	IPC_INT  i;
	IPC_LLONG match_flags;
	IPC_LLONG search_flag;
	
	match_flags = 0;
	
	for( i=0; i<D_IPC_IIP_USER_FLAGS_64; i++ ){			/* E_IPC_IIP_UNIT_ID_MAX */
		search_flag = ( in_flags & (1ULL<<i) );
		match_flags |= ipc_iip_user_ipc2im_tbl8( search_flag, ipc_iip_user_unitid_flags_tbl, E_IPC_IIP_UNIT_ID_MAX );
	}
	
	*out_flags = match_flags;
}


static IPC_VOID ipc_iip_user_pixid_flags_ipc2im( IPC_UINT* out_flags, IPC_UINT in_flags )
{
	IPC_INT  i;
	IPC_UINT match_flags;
	IPC_UINT search_flag;
	
	match_flags = 0;
	
	for( i=0; i<D_IPC_IIP_USER_FLAGS_32; i++ ){				/* E_IPC_IIP_PIXID_MAX */
		search_flag = ( in_flags & (1<<i) );
		match_flags |= ipc_iip_user_ipc2im_tbl( search_flag, ipc_iip_user_pixid_flags_tbl, E_IPC_IIP_PIXID_MAX );
	}
	
	*out_flags = match_flags;
}


static IPC_VOID ipc_iip_user_other_flags_ipc2im( IPC_UINT* out_flags, IPC_UINT in_flags )
{
	IPC_INT  i;
	IPC_UINT match_flags;
	IPC_UINT search_flag;
	
	match_flags = 0;
	
	for( i=0; i<D_IPC_IIP_USER_FLAGS_32; i++ ){				/* E_IPC_IIP_RES_MAX */
		search_flag = ( in_flags & (1<<i) );
		match_flags |= ipc_iip_user_ipc2im_tbl( search_flag, ipc_iip_user_other_flags_tbl, E_IPC_IIP_RES_MAX );
	}
	
	*out_flags = match_flags;
}


static IPC_INT ipc_iip_user_lock_resouce( const T_IPC_IIP_RES_REQ* req, 
										  IPC_LLONG* unit_flags, 
										  IPC_UINT*  pixid_flags, 
										  IPC_UINT*  other_flags )
{
	INT32 ercd;
	T_FJ_IIP_RES_CONFIG_OUT iip_res;
	T_FJ_IIP_RES_CONFIG_ARG config_arg;
	IPC_LLONG unit;
	IPC_UINT  pixid;
	IPC_UINT  other;
	
	memset( &config_arg, 0, sizeof(T_FJ_IIP_RES_CONFIG_ARG) );
	
	/* Unit */
	config_arg.unit.oned   = req->unit.oned;
	config_arg.unit.flt    = req->unit.flt;
	config_arg.unit.afn_0  = req->unit.afn0;			/* 14bit機能有 */
	config_arg.unit.afn    = req->unit.afnx;
	config_arg.unit.frect  = req->unit.frect;
	config_arg.unit.mon    = req->unit.mon;
	
	/* Store Unit
		bit7-5 MasterIF, bit4-0 unit数
		req->unit.slm & 0xE0 -> D_IPC_IIP_MASTER_IF0/1/2
		req->unit.slh & 0xE0 -> D_IPC_IIP_MASTER_IF0/1/2
		req->unit.slx & 0xE0 -> D_IPC_IIP_MASTER_IF0/1/2
	*/
	if( (req->unit.slm & 0xE0) == D_IPC_IIP_MASTER_IF0 ){
		config_arg.unit.sl_0_2_master_if_0 = (req->unit.slm & 0x1F);
	}else if( (req->unit.slm & 0xE0) == D_IPC_IIP_MASTER_IF1 ){
		config_arg.unit.sl_0_2_master_if_1 = (req->unit.slm & 0x1F);
	}else if( (req->unit.slm & 0xE0) == D_IPC_IIP_MASTER_IF2 ){
		config_arg.unit.sl_0_2_master_if_2 = (req->unit.slm & 0x1F);
	}else{
		config_arg.unit.sl_0_2 = (req->unit.slm & 0x1F);	/* MASK/MON機能有 */
	}
	
	if( (req->unit.slh & 0xE0) == D_IPC_IIP_MASTER_IF0 ){
		config_arg.unit.sl_mon_master_if_0 = (req->unit.slh & 0x1F);
	}else if( (req->unit.slh & 0xE0) == D_IPC_IIP_MASTER_IF1 ){
		config_arg.unit.sl_mon_master_if_1 = (req->unit.slh & 0x1F);
	}else if( (req->unit.slh & 0xE0) == D_IPC_IIP_MASTER_IF2 ){
		config_arg.unit.sl_mon_master_if_2 = (req->unit.slh & 0x1F);
	}else{
		config_arg.unit.sl_mon = (req->unit.slh & 0x1F);	/* MON機能有 */
	}
	
	if( (req->unit.slx & 0xE0) == D_IPC_IIP_MASTER_IF0 ){
		config_arg.unit.sl_master_if_0 = (req->unit.slx & 0x1F);
	}else if( (req->unit.slx & 0xE0) == D_IPC_IIP_MASTER_IF1 ){
		config_arg.unit.sl_master_if_1 = (req->unit.slx & 0x1F);
	}else if( (req->unit.slx & 0xE0) == D_IPC_IIP_MASTER_IF2 ){
		config_arg.unit.sl_master_if_2 = (req->unit.slx & 0x1F);
	}else{
		config_arg.unit.sl = (req->unit.slx & 0x1F);
	}
	
	config_arg.unit.csc    = req->unit.csc;
	config_arg.unit.lut    = req->unit.lut;
	config_arg.unit.gpc    = req->unit.gpc;
	config_arg.unit.blend  = req->unit.blend;
	config_arg.unit.cfl    = req->unit.cfc;
	config_arg.unit.mft    = req->unit.mft;
	
	/* Pixel ID */
	config_arg.pixid = req->pixid;
	
	/* IPC_IIP FlagsをIM形式に変更 */
	ipc_iip_user_other_flags_ipc2im( &other, req->other );
	
	/* Resource */
	config_arg.other = other;
	
	ercd = FJ_IIP_Res_Config_Lock_Host( &iip_res, &config_arg, 1, D_IPC_IIP_USER_WAIT_TIME );
	if( ercd != FJ_ERR_OK ) {
		return D_IPC_IIP_NG_RESOURCE;
	}
	
	/* IM Flags形式をIPC_IIP形式に変換 */
	ipc_iip_user_unitid_flags_im2ipc( &unit,  iip_res.unitid_bitmask64 );
	ipc_iip_user_pixid_flags_im2ipc(  &pixid, iip_res.pixid_bitmask );
	ipc_iip_user_other_flags_im2ipc(  &other, iip_res.other_bitmask );
	
	*unit_flags  = unit;
	*pixid_flags = pixid;
	*other_flags = other;
	
	return D_IPC_IIP_OK;
}


static IPC_INT ipc_iip_user_free_resouce( const T_IPC_IIP_RES *res )
{
	T_FJ_IIP_RES_CONFIG_OUT iip_res;
	INT32 ercd;
	IPC_LLONG unit;
	IPC_UINT  pixid;
	IPC_UINT  other;
	
	ipc_iip_user_unitid_flags_ipc2im( &unit,  res->unit_flags );
	ipc_iip_user_pixid_flags_ipc2im(  &pixid, res->pixid_flags );
	ipc_iip_user_other_flags_ipc2im(  &other, res->other_flags );
	
	iip_res.unitid_bitmask64 = unit;
	iip_res.pixid_bitmask    = pixid;
	iip_res.other_bitmask    = other;
	
	ercd = FJ_IIP_Res_Config_Free_Host( &iip_res, 1, D_IPC_IIP_USER_WAIT_TIME );
	if( ercd != FJ_ERR_OK ) {
		return D_IPC_IIP_NG_RESOURCE;
	}
	
	return D_IPC_IIP_OK;
}


/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/
IPC_INT IPC_IIP_User_AllocIipResource( const T_IPC_IIP_RES_REQ *req, T_IPC_IIP_RES *res )
{
	IPC_INT   ret;
	IPC_LLONG unit_flags  = 0;
	IPC_UINT  pixid_flags = 0;
	IPC_UINT  other_flags = 0;
	
	ret = ipc_iip_user_lock_resouce( req, &unit_flags, &pixid_flags, &other_flags );
	if( ret != D_IPC_IIP_OK ){
		return D_IPC_IIP_NG_RESOURCE;
	}
	
	res->unit_flags  = unit_flags;
	res->pixid_flags = pixid_flags;
	res->other_flags = other_flags;
	
	return D_IPC_IIP_OK;
}


IPC_INT IPC_IIP_User_FreeIipResource( const T_IPC_IIP_RES *res )
{
	IPC_INT  ret;
	
	ret = ipc_iip_user_free_resouce( res );
	if( ret != D_IPC_IIP_OK ){
		return D_IPC_IIP_NG_RESOURCE;
	}
	
	return D_IPC_IIP_OK;
}

