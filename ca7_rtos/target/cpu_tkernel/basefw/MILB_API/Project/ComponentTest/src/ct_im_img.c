/**
 * @file		ct_im_img.c
 * @brief		This is ct code for im_img.c
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdsimg.h"
#include "im_img.h"
#include "ct_im_img.h"
#include "stdlib.h"
#include "string.h"
#include "driver_common.h"

VOID CT_Im_Img_Reg_Print( VOID )
{
	Ddim_Print(("---------------------------------\n"));
	Ddim_Print(("  Display MXICMON Register Value\n"));
	Ddim_Print(("---------------------------------\n"));
	Ddim_Print(("MXICMON.VDEN      = %d\n", IO_IMG.MXICMON.bit.VDEN));
	Ddim_Print(("MXICMON.HDEN      = %d\n", IO_IMG.MXICMON.bit.HDEN));
	Ddim_Print(("MXICMON.SENEN     = %d\n", IO_IMG.MXICMON.bit.SENEN));
	Ddim_Print(("MXICMON.SRO1EN    = %d\n", IO_IMG.MXICMON.bit.SRO1EN));
	Ddim_Print(("MXICMON.SRO2EN    = %d\n", IO_IMG.MXICMON.bit.SRO2EN));
	Ddim_Print(("MXICMON.B2B1EN    = %d\n", IO_IMG.MXICMON.bit.B2B1EN));
	Ddim_Print(("MXICMON.B2B2EN    = %d\n", IO_IMG.MXICMON.bit.B2B2EN));
	Ddim_Print(("MXICMON.YO1EN     = %d\n", IO_IMG.MXICMON.bit.YO1EN));
	Ddim_Print(("MXICMON.YO2EN     = %d\n", IO_IMG.MXICMON.bit.YO2EN));
	Ddim_Print(("MXICMON.STATEN    = %d\n", IO_IMG.MXICMON.bit.STATEN));
	Ddim_Print(("MXICMON.PASEN     = %d\n", IO_IMG.MXICMON.bit.PASEN));
	Ddim_Print(("MXICMON.DEN       = %d\n", IO_IMG.MXICMON.bit.DEN));
	Ddim_Print(("------------------------------------\n"));
	Ddim_Print(("  Display IMGPIPEPSW Register Value\n"));
	Ddim_Print(("------------------------------------\n"));
	Ddim_Print(("IMGPIPEPSW.R2YSEL = %d\n", IO_IMG.IMGPIPEPSW.bit.R2YSEL));
	Ddim_Print(("IMGPIPEPSW.B2RCNC = %d\n", IO_IMG.IMGPIPEPSW.bit.B2RCNC));
	Ddim_Print(("------------------------------------\n"));
	Ddim_Print(("  Display SRAMPD Register Value\n"));
	Ddim_Print(("------------------------------------\n"));
	Ddim_Print(("SRAMPD.JPEGPD = %d\n", IO_IMG.SRAMPD.bit.JPEGPD));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_1_01( VOID )
{
	Im_IMG_Init();

	Ddim_Print(("CT_Im_Img_1_01\n"));
	CT_Im_Img_Reg_Print();
}

VOID CT_Im_Img_2_01( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_VDEN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_VDEN );

	Ddim_Print(("CT_Im_Img_2_01\n"));
	Ddim_Print(("MXICMON.VDEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_02( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_VDEN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_VDEN );

	Ddim_Print(("CT_Im_Img_2_02\n"));
	Ddim_Print(("MXICMON.VDEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_03( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_HDEN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_HDEN );

	Ddim_Print(("CT_Im_Img_2_03\n"));
	Ddim_Print(("MXICMON.HDEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_04( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_HDEN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_HDEN );

	Ddim_Print(("CT_Im_Img_2_04\n"));
	Ddim_Print(("MXICMON.HDEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_05( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_SENEN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_SENEN );

	Ddim_Print(("CT_Im_Img_2_05\n"));
	Ddim_Print(("MXICMON.SEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_06( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_SENEN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_SENEN );

	Ddim_Print(("CT_Im_Img_2_06\n"));
	Ddim_Print(("MXICMON.SEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_07( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_SRO1EN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_SRO1EN );

	Ddim_Print(("CT_Im_Img_2_07\n"));
	Ddim_Print(("MXICMON.SRO1EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_08( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_SRO1EN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_SRO1EN );

	Ddim_Print(("CT_Im_Img_2_08\n"));
	Ddim_Print(("MXICMON.SRO1EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_09( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_SRO2EN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_SRO2EN );

	Ddim_Print(("CT_Im_Img_2_09\n"));
	Ddim_Print(("MXICMON.SRO2EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_10( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_SRO2EN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_SRO2EN );

	Ddim_Print(("CT_Im_Img_2_10\n"));
	Ddim_Print(("MXICMON.SRO2EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_11( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_B2B1EN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_B2B1EN );

	Ddim_Print(("CT_Im_Img_2_11\n"));
	Ddim_Print(("MXICMON.B2B1EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_12( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_B2B1EN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_B2B1EN );

	Ddim_Print(("CT_Im_Img_2_12\n"));
	Ddim_Print(("MXICMON.B2B1EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_13( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_B2B2EN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_B2B2EN );

	Ddim_Print(("CT_Im_Img_2_13\n"));
	Ddim_Print(("MXICMON.B2B2EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_14( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_B2B2EN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_B2B2EN );

	Ddim_Print(("CT_Im_Img_2_14\n"));
	Ddim_Print(("MXICMON.B2B2EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_15( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_YO1EN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_YO1EN );

	Ddim_Print(("CT_Im_Img_2_15\n"));
	Ddim_Print(("MXICMON.Y01EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_16( VOID )
{
	USHORT reg;
	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_YO1EN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_YO1EN );

	Ddim_Print(("CT_Im_Img_2_16\n"));
	Ddim_Print(("MXICMON.Y01EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_17( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_YO2EN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_YO2EN );

	Ddim_Print(("CT_Im_Img_2_17\n"));
	Ddim_Print(("MXICMON.Y02EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_18( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_YO2EN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_YO2EN );

	Ddim_Print(("CT_Im_Img_2_18\n"));
	Ddim_Print(("MXICMON.Y02EN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_19( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_STATEN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_STATEN );

	Ddim_Print(("CT_Im_Img_2_19\n"));
	Ddim_Print(("MXICMON.STATEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_20( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_STATEN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_STATEN );

	Ddim_Print(("CT_Im_Img_2_20\n"));
	Ddim_Print(("MXICMON.STATEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_21( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_PASEN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_PASEN );

	Ddim_Print(("CT_Im_Img_2_21\n"));
	Ddim_Print(("MXICMON.PASEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_22( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_PASEN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_PASEN );

	Ddim_Print(("CT_Im_Img_2_22\n"));
	Ddim_Print(("MXICMON.PASEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_23( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_DEN, E_IM_IMG_MXICMON_DISABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_DEN );

	Ddim_Print(("CT_Im_Img_2_23\n"));
	Ddim_Print(("MXICMON.DEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_24( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_DEN, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_DEN );

	Ddim_Print(("CT_Im_Img_2_24\n"));
	Ddim_Print(("MXICMON.DEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_2_25_26( VOID )
{
	USHORT reg;

	Im_IMG_Set_MXICMON( (E_IM_IMG_MXICMON_TYP)12, E_IM_IMG_MXICMON_ENABLE );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_DEN );

	Ddim_Print(("CT_Im_Img_2_25\n"));
	Ddim_Print(("MXICMON.DEN = %d\n", reg));
	Ddim_Print(("\n"));

	Im_IMG_Set_MXICMON( E_IM_IMG_MXICMON_DEN, (E_IM_IMG_MXICMON_STS)2 );
	reg = Im_IMG_Get_MXICMON( E_IM_IMG_MXICMON_DEN );

	Ddim_Print(("CT_Im_Img_2_26\n"));
	Ddim_Print(("MXICMON.DEN = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_01( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL, E_IM_IMG_IMGPIPEPSW_B2R );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL );

	Ddim_Print(("CT_Im_Img_3_01\n"));
	Ddim_Print(("IMGPIPEPSW.R2YSEL = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_02( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL, E_IM_IMG_IMGPIPEPSW_LTM );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL );

	Ddim_Print(("CT_Im_Img_3_02\n"));
	Ddim_Print(("IMGPIPEPSW.R2YSEL = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_03( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL, E_IM_IMG_IMGPIPEPSW_NONE );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL );

	Ddim_Print(("CT_Im_Img_3_03\n"));
	Ddim_Print(("IMGPIPEPSW.R2YSEL = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_04( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL, (E_IM_IMG_IMGPIPEPSW_SEL)3 );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_R2YSEL );

	Ddim_Print(("CT_Im_Img_3_04\n"));
	Ddim_Print(("IMGPIPEPSW.R2YSEL = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_05( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC, E_IM_IMG_IMGPIPEPSW_B2R );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC );

	Ddim_Print(("CT_Im_Img_3_05\n"));
	Ddim_Print(("IMGPIPEPSW.B2RCNC = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_06( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC, E_IM_IMG_IMGPIPEPSW_LTM );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC );

	Ddim_Print(("CT_Im_Img_3_06\n"));
	Ddim_Print(("IMGPIPEPSW.B2RCNC = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_07( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC, E_IM_IMG_IMGPIPEPSW_NONE );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC );

	Ddim_Print(("CT_Im_Img_3_07\n"));
	Ddim_Print(("IMGPIPEPSW.B2RCNC = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_08( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC, (E_IM_IMG_IMGPIPEPSW_SEL)3 );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC );

	Ddim_Print(("CT_Im_Img_3_08\n"));
	Ddim_Print(("IMGPIPEPSW.B2RCNC = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_3_09( VOID )
{
	USHORT	reg;

	Im_IMG_Set_IMGPIPEPSW( (E_IM_IMG_IMGPIPEPSW_TYP)2, E_IM_IMG_IMGPIPEPSW_NONE );
	reg = Im_IMG_Get_IMGPIPEPSW( E_IM_IMG_IMGPIPEPSW_B2RCNC );

	Ddim_Print(("CT_Im_Img_3_09\n"));
	Ddim_Print(("IMGPIPEPSW.B2RCNC = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_01( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_Y01, TRUE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_Y01 );

	Ddim_Print(("CT_Im_Img_4_01\n"));
	Ddim_Print(("SRAMPD.JPEGPD(Y01) = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_02( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_Y01, FALSE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_Y01 );

	Ddim_Print(("CT_Im_Img_4_02\n"));
	Ddim_Print(("SRAMPD.JPEGPD(Y01) = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_03( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_Y23, TRUE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_Y23 );

	Ddim_Print(("CT_Im_Img_4_03\n"));
	Ddim_Print(("SRAMPD.JPEGPD(Y23) = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_04( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_Y23, FALSE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_Y23 );

	Ddim_Print(("CT_Im_Img_4_04\n"));
	Ddim_Print(("SRAMPD.JPEGPD(Y23) = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_05( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_C, TRUE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_C );

	Ddim_Print(("CT_Im_Img_4_05\n"));
	Ddim_Print(("SRAMPD.JPEGPD(C) = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_06( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( E_IM_IMG_SRAMPD_C, FALSE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_C );

	Ddim_Print(("CT_Im_Img_4_06\n"));
	Ddim_Print(("SRAMPD.JPEGPD(C) = %d\n", reg));
	Ddim_Print(("\n"));
}

VOID CT_Im_Img_4_07( VOID )
{
	BOOL reg;

	Im_IMG_Set_SRAMPD( (E_IM_IMG_SRAMPD_TYP)3, FALSE );
	reg = Im_IMG_Get_SRAMPD( E_IM_IMG_SRAMPD_C );

	Ddim_Print(("CT_Im_Img_4_07\n"));
	Ddim_Print(("SRAMPD.JPEGPD(C) = %d\n", reg));
	Ddim_Print(("\n"));
}

void Ct_Im_IMG_Main( INT32 argc, CHAR** argv )
{
	E_IM_IMG_MXICMON_TYP		mxicmon_typ;
	E_IM_IMG_MXICMON_STS		mxicmon_sts;
	E_IM_IMG_IMGPIPEPSW_TYP		imgpipesw_typ;
	E_IM_IMG_IMGPIPEPSW_SEL		imgpipesw_sel;
	E_IM_IMG_SRAMPD_TYP			srampd_typ;
	BOOL						srampd_sts;
	UINT32						argv1;
	USHORT						argv2;
	USHORT						argv3;

	if( argc < 4 ) {
		Ddim_Print(( "Ct_Im_IMG_Main: parameter error.\n" ));
		return;
	}

	Ddim_Print(("img command execute\n"));

	argv1 = (UINT32)atoi((const char*)argv[1]);
	argv2 = (USHORT)atoi((const char*)argv[2]);
	argv3 = (USHORT)atoi((const char*)argv[3]);

	switch ( argv1 ) {
		case 1:	/* initialize */
			if ( argv2 == 1 ) {
				CT_Im_Img_1_01();
			}
			else if ( argv2 == 2 ) {
				CT_Im_Img_Reg_Print();
			}
			else {
				Ddim_Print(("input parameter error\n"));
			}
			break;

		case 2:	/* MXICMON Register Set */
			/* ----------------------------------------------
				<MXICMON Type>
					MXICMON.VDEN	: 0
					MXICMON.HDEN	: 1
					MXICMON.SENEN	: 2
					MXICMON.SRO1EN	: 3
					MXICMON.SRO2EN	: 4
					MXICMON.B2B1EN	: 5
					MXICMON.B2B2EN	: 6
					MXICMON.YO1EN	: 7
					MXICMON.YO2EN	: 8
					MXICMON.STATEN	: 9
					MXICMON.PASEN	: 10
					MXICMON.DEN		: 11
			------------------------------------------------
				<enable/disable status>
					Disable			: 0
					Enable			: 1
			------------------------------------------------
			*/

			mxicmon_typ = (E_IM_IMG_MXICMON_TYP)argv2;
			mxicmon_sts = (E_IM_IMG_MXICMON_STS)argv3;

			switch ( mxicmon_typ ) {
				case E_IM_IMG_MXICMON_VDEN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_01();
					}
					else {
						CT_Im_Img_2_02();
					}
					break;
				case E_IM_IMG_MXICMON_HDEN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_03();
					}
					else {
						CT_Im_Img_2_04();
					}
					break;
				case E_IM_IMG_MXICMON_SENEN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_05();
					}
					else {
						CT_Im_Img_2_06();
					}
					break;
				case E_IM_IMG_MXICMON_SRO1EN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_07();
					}
					else {
						CT_Im_Img_2_08();
					}
					break;
				case E_IM_IMG_MXICMON_SRO2EN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_09();
					}
					else {
						CT_Im_Img_2_10();
					}
					break;
				case E_IM_IMG_MXICMON_B2B1EN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_11();
					}
					else {
						CT_Im_Img_2_12();
					}
					break;
				case E_IM_IMG_MXICMON_B2B2EN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_13();
					}
					else {
						CT_Im_Img_2_14();
					}
					break;
				case E_IM_IMG_MXICMON_YO1EN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_15();
					}
					else {
						CT_Im_Img_2_16();
					}
					break;
				case E_IM_IMG_MXICMON_YO2EN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_17();
					}
					else {
						CT_Im_Img_2_18();
					}
					break;
				case E_IM_IMG_MXICMON_STATEN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_19();
					}
					else {
						CT_Im_Img_2_20();
					}
					break;
				case E_IM_IMG_MXICMON_PASEN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_21();
					}
					else {
						CT_Im_Img_2_22();
					}
					break;
				case E_IM_IMG_MXICMON_DEN:
					if ( mxicmon_sts == E_IM_IMG_MXICMON_DISABLE) {
						CT_Im_Img_2_23();
					}
					else {
						CT_Im_Img_2_24();
					}
					break;
				default :
					CT_Im_Img_2_25_26();
					break;
			}
			break;

		case 3:	/* IMGPIPEPSW Register Set. */
			/* ----------------------------------------------
				<IMGPIPEPSW Type>
					IMGPIPEPSW.R2YSEL	: 0
				<Status>
					R2Y input B2R		: 0
					R2Y input LTM		: 1
					R2Y input None		: 2

				<IMGPIPEPSW Type>
					IMGPIPEPSW.B2RCNC	: 1
				<Status>
					B2R connection None	: 0
					B2R connection LTM	: 1
					B2R connection None	: 2
			------------------------------------------------
			*/

			imgpipesw_typ = (E_IM_IMG_IMGPIPEPSW_TYP)argv2;
			imgpipesw_sel = (E_IM_IMG_IMGPIPEPSW_SEL)argv3;

			switch ( imgpipesw_typ ) {
				case E_IM_IMG_IMGPIPEPSW_R2YSEL:
					if ( imgpipesw_sel == E_IM_IMG_IMGPIPEPSW_B2R ) {
						CT_Im_Img_3_01();
					}
					else if ( imgpipesw_sel == E_IM_IMG_IMGPIPEPSW_LTM ) {
						CT_Im_Img_3_02();
					}
					else if ( imgpipesw_sel == E_IM_IMG_IMGPIPEPSW_NONE ) {
						CT_Im_Img_3_03();
					}
					else {
						CT_Im_Img_3_04();
					}
					break;
				case E_IM_IMG_IMGPIPEPSW_B2RCNC:
					if ( imgpipesw_sel == E_IM_IMG_IMGPIPEPSW_B2R ) {
						CT_Im_Img_3_05();
					}
					else if ( imgpipesw_sel == E_IM_IMG_IMGPIPEPSW_LTM ) {
						CT_Im_Img_3_06();
					}
					else if ( imgpipesw_sel == E_IM_IMG_IMGPIPEPSW_NONE ) {
						CT_Im_Img_3_07();
					}
					else {
						CT_Im_Img_3_08();
					}
					break;
				default:
					CT_Im_Img_3_09();
					break;
			}
			break;

		case 4:	/* SRAMPD Register Set */
			/* ----------------------------------------------
				<SRAMPD Type>
					E_IM_IMG_SRAMPD_Y01	: 0
					E_IM_IMG_SRAMPD_Y23	: 1
					E_IM_IMG_SRAMPD_C	: 2
			------------------------------------------------
				<SRAMPD status>
					Power down off	: 0
					Power down on	: 1
			------------------------------------------------
			*/

			srampd_typ = (E_IM_IMG_SRAMPD_TYP)argv2;
			srampd_sts = (BOOL)argv3;

			switch ( srampd_typ ) {
				case E_IM_IMG_SRAMPD_Y01:
					if ( srampd_sts ) {
						CT_Im_Img_4_01();
					}
					else {
						CT_Im_Img_4_02();
					}
					break;
				case E_IM_IMG_SRAMPD_Y23:
					if ( srampd_sts ) {
						CT_Im_Img_4_03();
					}
					else {
						CT_Im_Img_4_04();
					}
					break;
				case E_IM_IMG_SRAMPD_C:
					if ( srampd_sts ) {
						CT_Im_Img_4_05();
					}
					else {
						CT_Im_Img_4_06();
					}
					break;
				default:
					CT_Im_Img_4_07();
					break;
			}
			break;

		default:
			/* Nothing special */
			break;
	}

	Ddim_Print(("img command complete\n"));
	return;
}
