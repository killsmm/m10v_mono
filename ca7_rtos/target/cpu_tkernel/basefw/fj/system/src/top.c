/**
 * @file		top.c
 * @brief		LSI top control api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "dd_top.h"
#include "fj_top.h"
#include "debug.h"
#include "h264_user.h"

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID top_get_clock_regi( FJ_TOP_CLOCK_NAME clock_name, volatile ULONG** reg, ULONG* bit )
{
	switch( clock_name ){
		case FJ_TOP_CLOCK_NAME_DSPCK:
			*reg  = &Dd_Top_Get_CLKSTOP1();
			*bit = D_DD_TOP_DSPCK_BIT;
			break;
		case FJ_TOP_CLOCK_NAME_DSPAX:
			*reg  = &Dd_Top_Get_CLKSTOP1();
			*bit = D_DD_TOP_DSPAX_BIT;
			break;
		default:
			break;
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Start clock.
*/
FJ_ERR_CODE fj_top_start_clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter )
{
	volatile ULONG* reg;
	ULONG bit;

	if( clock_name >= FJ_TOP_CLOCK_NAME_MAX ){
		BF_Debug_Print_Warning(( "fj_top_start_clock() - clock_name is error! : clock_name=%d\n", clock_name ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( counter == NULL ){
		BF_Debug_Print_Warning(( "fj_top_start_clock() - counter is NULL!!\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	top_get_clock_regi( clock_name, &reg, &bit );
	Dd_Top_Start_Clock( counter, reg, ~bit );

	return FJ_ERR_OK;
}

/**
Stop clock.
*/
FJ_ERR_CODE fj_top_stop_clock( FJ_TOP_CLOCK_NAME clock_name, UCHAR* counter )
{
	volatile ULONG* reg;
	ULONG bit;

	if( clock_name >= FJ_TOP_CLOCK_NAME_MAX ){
		BF_Debug_Print_Warning(( "fj_top_start_clock() - clock_name is error! : clock_name=%d\n", clock_name ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( counter == NULL ){
		BF_Debug_Print_Warning(( "fj_top_start_clock() - counter is NULL!!\n" ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	top_get_clock_regi( clock_name, &reg, &bit );
	Dd_Top_Stop_Clock( counter, reg, bit );

	return FJ_ERR_OK;
}

/**
Set EMMC clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_emmc( FJ_TOP_EMMC_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_EMMC_CLKSEL_187MHZ) &&
		(clksel != FJ_TOP_EMMC_CLKSEL_166MHZ) &&
		(clksel != FJ_TOP_EMMC_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_EMMC_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_emmc() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_EMMC_CLKSEL)Dd_Top_Get_CLKSEL1_EMMCCLK()){
		Dd_Top_Set_CLKSEL1_EMMCCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set Nand Flash I/F clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_nf( FJ_TOP_NF_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_NF_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_NF_CLKSEL_80MHZ) &&
		(clksel != FJ_TOP_NF_CLKSEL_61MHZ) &&
		(clksel != FJ_TOP_NF_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_NF_CLKSEL_20MHZ) &&
		(clksel != FJ_TOP_NF_CLKSEL_32MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_nf() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_NF_CLKSEL)Dd_Top_Get_CLKSEL1_NFCLK()){
		Dd_Top_Set_CLKSEL1_NFCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set SD Card I/F ch2 UHS-II clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sd2u2( FJ_TOP_SD2U2_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SD2U2_CLKSEL_51MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_46MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_42MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_38MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_35MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_33MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_29MHZ) &&
		(clksel != FJ_TOP_SD2U2_CLKSEL_25MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sd2u2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SD2U2_CLKSEL)Dd_Top_Get_CLKSEL1_UHS2CLK()){
		UINT8 last;
		last = Dd_Top_Get_CLKSTOP2_UHS2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL1_UHS2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SD Card I/F ch2 UHS-I clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sd2u1( FJ_TOP_SD2U1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SD2U1_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_SD2U1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SD2U1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SD2U1_CLKSEL_700MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sd2u1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SD2U1_CLKSEL)Dd_Top_Get_CLKSEL1_UHS1CLK1()){
		UINT8 last;

		last = Dd_Top_Get_CLKSTOP2_UHS1CK1();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS1CK1( 1 );
		}
		
		Dd_Top_Set_CLKSEL1_UHS1CLK1( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS1CK1( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SD Card I/F ch3 UHS-I clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sd3u1( FJ_TOP_SD3_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SD3_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SD3_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SD3_CLKSEL_700MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sd3u1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SD3_CLKSEL)Dd_Top_Get_CLKSEL1_UHS1CLK2()){
		UINT8 last;
		last = Dd_Top_Get_CLKSTOP2_UHS1CK2();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS1CK2( 1 );
		}
		
		Dd_Top_Set_CLKSEL1_UHS1CLK2( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS1CK2( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SD Card I/F ch1 SDIO clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sd1io( FJ_TOP_SD1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SD1_CLKSEL_800MHZ) &&
		(clksel != FJ_TOP_SD1_CLKSEL_533MHZ) &&
		(clksel != FJ_TOP_SD1_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_SD1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SD1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SD1_CLKSEL_700MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sd1io() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SD1_CLKSEL)Dd_Top_Get_CLKSEL1_UHS1CLK0()){
		UINT8 last;
		last = Dd_Top_Get_CLKSTOP2_UHS1CK0();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS1CK0( 1 );
		}
		
		Dd_Top_Set_CLKSEL1_UHS1CLK0( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP2_UHS1CK0( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set RAW clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_raw( FJ_TOP_RAW_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_RAW_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_RAW_CLKSEL_160MHZ) &&
		(clksel != FJ_TOP_RAW_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_RAW_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_RAW_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_raw() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_RAW_CLKSEL)Dd_Top_Get_CLKSEL2_RAWCLK()){
		Dd_Top_Set_CLKSEL2_RAWCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set HIF clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_hif( FJ_TOP_HIF_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_HIF_CLKSEL_594MHZ) &&
		(clksel != FJ_TOP_HIF_CLKSEL_297MHZ) &&
		(clksel != FJ_TOP_HIF_CLKSEL_148MHZ) &&
		(clksel != FJ_TOP_HIF_CLKSEL_74MHZ) &&
		(clksel != FJ_TOP_HIF_CLKSEL_27MHZ) &&
		(clksel != FJ_TOP_HIF_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_hif() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_HIF_CLKSEL)Dd_Top_Get_CLKSEL2_HIFCLK()){
		UCHAR hifcks = 0;

		hifcks = Dd_Top_Get_CLKSTOP3_HIFCK();
		if(hifcks == 0){
			Dd_Top_Set_CLKSTOP3_HIFCK(1);
		}

		Dd_Top_Set_CLKSEL2_HIFCLK( clksel );

		if(hifcks == 0){
			Dd_Top_Set_CLKSTOP3_HIFCK(0);
		}
	}

	return FJ_ERR_OK;
}

/**
Set MIF clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_mif( FJ_TOP_MIF_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_MIF_CLKSEL_169MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_148MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_108MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_74MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_66MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_54MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_49MHZ) &&
		(clksel != FJ_TOP_MIF_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_mif() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_MIF_CLKSEL)Dd_Top_Get_CLKSEL2_MIFCLK()){
		UCHAR mifcks = 0;

		mifcks = Dd_Top_Get_CLKSTOP3_MIFCK();
		if(mifcks == 0){
			Dd_Top_Set_CLKSTOP3_MIFCK(1);
		}

		Dd_Top_Set_CLKSEL2_MIFCLK( clksel );

		if(mifcks == 0){
			Dd_Top_Set_CLKSTOP3_MIFCK(0);
		}
	}

	return FJ_ERR_OK;
}

/**
Set IPU clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ipu( FJ_TOP_IPU_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_IPU_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_250MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_350MHZ) &&
		(clksel != FJ_TOP_IPU_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ipu() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_IPU_CLKSEL)Dd_Top_Get_CLKSEL2_IPUCLK()){
		Dd_Top_Set_CLKSEL2_IPUCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set IPU TME clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ipu_tme( FJ_TOP_IPU_TME_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_IPU_TME_CLKSEL_500MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_62MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_125MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_187MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_250MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_312MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_375MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_437MHZ) &&
		(clksel != FJ_TOP_IPU_TME_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ipu_tme() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_IPU_TME_CLKSEL)Dd_Top_Get_CLKSEL2_IPUTMECLK()){
		Dd_Top_Set_CLKSEL2_IPUTMECLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set GPU clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_gpu( FJ_TOP_GPU_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_GPU_CLKSEL_466MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_58MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_116MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_175MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_233MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_291MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_350MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_408MHZ) &&
		(clksel != FJ_TOP_GPU_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_gpu() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_GPU_CLKSEL)Dd_Top_Get_CLKSEL2_GPUCLK()){
		Dd_Top_Set_CLKSEL2_GPUCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set JPEG clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_jpeg( FJ_TOP_JPG_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_JPG_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_25MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_125MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_175MHZ) &&
		(clksel != FJ_TOP_JPG_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_jpeg() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_JPG_CLKSEL)Dd_Top_Get_CLKSEL2_JPGCLK()){
		Dd_Top_Set_CLKSEL2_JPGCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set ELA clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ela( FJ_TOP_ELA_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_ELA_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_ELA_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ela() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_ELA_CLKSEL)Dd_Top_Get_CLKSEL2_ELACLK()){
		Dd_Top_Set_CLKSEL2_ELACLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set PIPESEL clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_pipe( FJ_TOP_PIPE_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_PIPE_CLKSEL_PLL02) &&
		(clksel != FJ_TOP_PIPE_CLKSEL_PLL01) &&
		(clksel != FJ_TOP_PIPE_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_pipe() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_PIPE_CLKSEL)Dd_Top_Get_CLKSEL3_PIPESEL()){
		Dd_Top_Set_CLKSEL3_PIPESEL( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set SENSOR clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sen( FJ_TOP_SEN_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SEN_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_SEN_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sen() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SEN_CLKSEL)Dd_Top_Get_CLKSEL3_SENCLK()){
		Dd_Top_Set_CLKSEL3_SENCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set SENSOR clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_senmsk( FJ_TOP_SENMSK_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SENMSK_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SENMSK_CLKSEL_80MHZ) &&
		(clksel != FJ_TOP_SENMSK_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_senmsk() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SENMSK_CLKSEL)Dd_Top_Get_CLKSEL3_SENMSKCLK()){
		Dd_Top_Set_CLKSEL3_SENMSKCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set IIP clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_iip( FJ_TOP_IIP_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_IIP_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_IIP_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_iip() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_IIP_CLKSEL)Dd_Top_Get_CLKSEL3_IIPCLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP3_IIPCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP3_IIPCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL3_IIPCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP3_IIPCK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set STAT clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_pas( FJ_TOP_PAS_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_PAS_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_PAS_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_pas() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_PAS_CLKSEL)Dd_Top_Get_CLKSEL3_PASCLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP14_PASCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP14_PASCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL3_PASCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP14_PASCK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set IPP clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ipp( FJ_TOP_IPP_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_IPP_CLKSEL_800MHZ) &&
		(clksel != FJ_TOP_IPP_CLKSEL_533MHZ) &&
		(clksel != FJ_TOP_IPP_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_IPP_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_IPP_CLKSEL_700MHZ) &&
		(clksel != FJ_TOP_IPP_CLKSEL_600MHZ) &&
		(clksel != FJ_TOP_IPP_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ipp() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_IPP_CLKSEL)Dd_Top_Get_CLKSEL3_IPPCLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP8_HEIPPCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEIPPCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL3_IPPCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEIPPCK( last );
		}
	}

	return FJ_ERR_OK;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
Set PXF clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_pxf( FJ_TOP_PXF_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_PXF_CLKSEL_700MHZ) &&
		(clksel != FJ_TOP_PXF_CLKSEL_280MHZ) &&
		(clksel != FJ_TOP_PXF_CLKSEL_155MHZ) &&
		(clksel != FJ_TOP_PXF_CLKSEL_533MHZ) &&
		(clksel != FJ_TOP_PXF_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_PXF_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_PXF_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_pxf() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_PXF_CLKSEL)Dd_Top_Get_CLKSEL3_PXFCLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP8_HEPXFCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEPXFCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL3_PXFCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEPXFCK( last );
		}
	}
	return FJ_ERR_OK;
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
Set HEVENCSEL clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_hevenc( FJ_TOP_HEVENC_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_HEVENC_CLKSEL_466MHZ) &&
		(clksel != FJ_TOP_HEVENC_CLKSEL_350MHZ) &&
		(clksel != FJ_TOP_HEVENC_CLKSEL_280MHZ) &&
		(clksel != FJ_TOP_HEVENC_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_HEVENC_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_HEVENC_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_hevenc() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( clksel != (FJ_TOP_HEVENC_CLKSEL)Dd_Top_Get_CLKSEL3_HEVENCCLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP8_HEVENCCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEVENCCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL3_HEVENCCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEVENCCK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set HEVENCSEL clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_hevsel( FJ_TOP_HEVSEL_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_HEVSEL_CLKSEL_500MHZ) &&
		(clksel != FJ_TOP_HEVSEL_CLKSEL_HEVENCCLK) &&
		(clksel != FJ_TOP_HEVSEL_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_hevsel() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_HEVSEL_CLKSEL)Dd_Top_Get_CLKSEL3_HEVENCSEL()){
		Dd_Top_Set_CLKSEL3_HEVENCSEL( clksel );
	}

	return FJ_ERR_OK;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
Set VDF clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_vdf( FJ_TOP_VDF_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_VDF_CLKSEL_700MHZ) &&
		(clksel != FJ_TOP_VDF_CLKSEL_280MHZ) &&
		(clksel != FJ_TOP_VDF_CLKSEL_155MHZ) &&
		(clksel != FJ_TOP_VDF_CLKSEL_533MHZ) &&
		(clksel != FJ_TOP_VDF_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_VDF_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_VDF_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_vdf() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_VDF_CLKSEL)Dd_Top_Get_CLKSEL3_VDFCLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP8_HEVDFCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEVDFCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL3_VDFCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_HEVDFCK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SRO1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sro1_2( FJ_TOP_SRO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SRO_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sro1_2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL4_SRO1CLK_2()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP10_SRO1CK_2();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_SRO1CK_2( 1 );
		}
		
		Dd_Top_Set_CLKSEL4_SRO1CLK_2( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_SRO1CK_2( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set R2Y1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_r2y1( FJ_TOP_R2Y_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_R2Y_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_r2y1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_R2Y_CLKSEL)Dd_Top_Get_CLKSEL4_R2Y1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP11_R2Y1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP11_R2Y1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL4_R2Y1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP11_R2Y1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set LTM1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ltm1( FJ_TOP_LTM1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_LTM1_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_SRO1CLK) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_SRO1CLK_2) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_B2B1CLK) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_B2R1CLK) &&
		(clksel != FJ_TOP_LTM1_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ltm1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_LTM1_CLKSEL)Dd_Top_Get_CLKSEL4_LTM1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP11_LTM1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP11_LTM1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL4_LTM1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP11_LTM1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set B2R1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_b2r1( FJ_TOP_B2R1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_B2R1_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_SRO1CLK) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_SRO1CLK_2) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_B2B1CLK) &&
		(clksel != FJ_TOP_B2R1_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_b2r1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_B2R1_CLKSEL)Dd_Top_Get_CLKSEL4_B2R1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP10_B2R1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL4_B2R1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_B2R1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set CNR1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_cnr1( FJ_TOP_CNR1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_CNR1_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_R2Y1CLK) &&
		(clksel != FJ_TOP_CNR1_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_cnr1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_CNR1_CLKSEL)Dd_Top_Get_CLKSEL4_CNR1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP11_CNR1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP11_CNR1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL4_CNR1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP11_CNR1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SRO1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sro1( FJ_TOP_SRO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SRO_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sro1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL5_SRO1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP10_SRO1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_SRO1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL5_SRO1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_SRO1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set B2B1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_b2b1( FJ_TOP_B2B1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_B2B1_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_SRO1CLK) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_SRO1CLK_2) &&
		(clksel != FJ_TOP_B2B1_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_b2b1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_B2B1_CLKSEL)Dd_Top_Get_CLKSEL5_B2B1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP10_B2B1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_B2B1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL5_B2B1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_B2B1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set LTMRBK1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ltmrbk1( FJ_TOP_LTMRBK1_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_LTMRBK1_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_SRO1CLK) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_SRO1CLK_2) &&
		(clksel != FJ_TOP_LTMRBK1_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ltmrbk1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_LTMRBK1_CLKSEL)Dd_Top_Get_CLKSEL5_LTMRBK1CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP10_LTMRBK1CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL5_LTMRBK1CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP10_LTMRBK1CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SRO2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sro2_2( FJ_TOP_SRO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SRO_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sro2_2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL6_SRO2CLK_2()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP12_SRO2CK_2();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_SRO2CK_2( 1 );
		}
		
		Dd_Top_Set_CLKSEL6_SRO2CLK_2( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_SRO2CK_2( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set R2Y2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_r2y2( FJ_TOP_R2Y_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_R2Y_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_R2Y_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_r2y2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_R2Y_CLKSEL)Dd_Top_Get_CLKSEL6_R2Y2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP13_R2Y2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP13_R2Y2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL6_R2Y2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP13_R2Y2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set LTM2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ltm2( FJ_TOP_LTM2_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_LTM2_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_SRO2CLK) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_SRO2CLK_2) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_B2B2CLK) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_B2R2CLK) &&
		(clksel != FJ_TOP_LTM2_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ltm2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_LTM2_CLKSEL)Dd_Top_Get_CLKSEL6_LTM2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP13_LTM2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP13_LTM2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL6_LTM2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP13_LTM2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set B2R2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_b2r2( FJ_TOP_B2R2_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_B2R2_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_SRO2CLK) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_SRO2CLK_2) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_B2B2CLK) &&
		(clksel != FJ_TOP_B2R2_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_b2r2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_B2R2_CLKSEL)Dd_Top_Get_CLKSEL6_B2R2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP12_B2R2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL6_B2R2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_B2R2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set CNR2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_cnr2( FJ_TOP_CNR2_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_CNR2_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_R2Y1CLK) &&
		(clksel != FJ_TOP_CNR2_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_cnr2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_CNR2_CLKSEL)Dd_Top_Get_CLKSEL6_CNR2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP13_CNR2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP13_CNR2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL6_CNR2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP13_CNR2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SRO2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_sro2( FJ_TOP_SRO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SRO_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_SRO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_sro2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL7_SRO2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP12_SRO2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_SRO2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL7_SRO2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_SRO2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set B2B2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_b2b2( FJ_TOP_B2B2_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_B2B2_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_SRO2CLK) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_SRO2CLK_2) &&
		(clksel != FJ_TOP_B2B2_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_b2b2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_B2B2_CLKSEL)Dd_Top_Get_CLKSEL7_B2B2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP12_B2B2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_B2B2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL7_B2B2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_B2B2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set LTMRBK2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ltmrbk2( FJ_TOP_LTMRBK2_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_LTMRBK2_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_SRO2CLK) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_SRO2CLK_2) &&
		(clksel != FJ_TOP_LTMRBK2_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ltmrbk2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_LTMRBK2_CLKSEL)Dd_Top_Get_CLKSEL7_LTMRBK2CLK()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP12_LTMRBK2CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL7_LTMRBK2CLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP12_LTMRBK2CK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SNAPCLKO3 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ap3( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SNAPCLKO_CLKSEL_PLL0) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_APCLK) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ap3() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP3SEL()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP4_APCK3();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK3( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AP3SEL( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK3( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SNAPCLKO2 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ap2( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SNAPCLKO_CLKSEL_PLL0) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_APCLK) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ap2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP2SEL()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP4_APCK2();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK2( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AP2SEL( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK2( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SNAPCLKO1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ap1( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SNAPCLKO_CLKSEL_PLL0) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_APCLK) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ap1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP1SEL()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP4_APCK1();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK1( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AP1SEL( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK1( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SNAPCLKO0 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_ap0( FJ_TOP_SNAPCLKO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SNAPCLKO_CLKSEL_PLL0) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_APCLK) &&
		(clksel != FJ_TOP_SNAPCLKO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ap0() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP0SEL()){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP4_APCK0();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK0( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AP0SEL( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP4_APCK0( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set APCLK clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_apclk( FJ_TOP_APCLK_CLKSEL clksel )
{
	if ( clksel >= FJ_TOP_APCLK_CLKSEL_MAX ){
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_apclk() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_APCLK_CLKSEL)Dd_Top_Get_CLKSEL8_APCLK()){
		UINT8 last[4] = {2,2,2,2};
		
		if( (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP0SEL() == FJ_TOP_SNAPCLKO_CLKSEL_APCLK ){
			last[0] = Dd_Top_Get_CLKSTOP4_APCK0();
			if( last[0] != 1 ){
				Dd_Top_Set_CLKSTOP4_APCK0( 1 );
			}
		}
		if( (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP1SEL() == FJ_TOP_SNAPCLKO_CLKSEL_APCLK ){
			last[1] = Dd_Top_Get_CLKSTOP4_APCK1();
			if( last[1] != 1 ){
				Dd_Top_Set_CLKSTOP4_APCK1( 1 );
			}
		}
		if( (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP2SEL() == FJ_TOP_SNAPCLKO_CLKSEL_APCLK ){
			last[2] = Dd_Top_Get_CLKSTOP4_APCK2();
			if( last[2] != 1 ){
				Dd_Top_Set_CLKSTOP4_APCK2( 1 );
			}
		}
		if( (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP3SEL() == FJ_TOP_SNAPCLKO_CLKSEL_APCLK ){
			last[3] = Dd_Top_Get_CLKSTOP4_APCK3();
			if( last[3] != 1 ){
				Dd_Top_Set_CLKSTOP4_APCK3( 1 );
			}
		}
		
		Dd_Top_Set_CLKSEL8_APCLK( clksel );
		
		if( last[0] == 0 ){
			Dd_Top_Set_CLKSTOP4_APCK0( 0 );
		}
		if( last[1] == 0 ){
			Dd_Top_Set_CLKSTOP4_APCK1( 0 );
		}
		if( last[2] == 0 ){
			Dd_Top_Set_CLKSTOP4_APCK2( 0 );
		}
		if( last[3] == 0 ){
			Dd_Top_Set_CLKSTOP4_APCK3( 0 );
		}
	}

	return FJ_ERR_OK;
}

/**
Set Audio ch0 Master clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_au0( FJ_TOP_AU_CLKSEL clksel )
{
	if( clksel >= FJ_TOP_AU_CLKSEL_MAX ){
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_au0() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AU_CLKSEL)Dd_Top_Get_CLKSEL8_AU0SEL() ){
		UINT8 last;
		
		last = Dd_Top_Get_CLKSTOP1_AU0CK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP1_AU0CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AU0SEL( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP1_AU0CK( last );
		}
	}
	return FJ_ERR_OK;
}

/**
Set Audio ch2 Master clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_au2( FJ_TOP_AU_CLKSEL clksel )
{
	if( clksel >= FJ_TOP_AU_CLKSEL_MAX ){
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_au2() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AU_CLKSEL)Dd_Top_Get_CLKSEL8_AU2SEL() ){
		UINT8 au2cks;
		
		au2cks = Dd_Top_Get_CLKSTOP1_AU2CK();
		if( 1 != au2cks ){
			Dd_Top_Set_CLKSTOP1_AU2CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AU2SEL( clksel );
		
		if( 1 != au2cks ){
			Dd_Top_Set_CLKSTOP1_AU2CK( au2cks );
		}
	}
	return FJ_ERR_OK;
}

/**
Set Audio ch3 Master clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_au3( FJ_TOP_AU_CLKSEL clksel )
{
	if( clksel >= FJ_TOP_AU_CLKSEL_MAX ){
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_au3() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AU_CLKSEL)Dd_Top_Get_CLKSEL8_AU3SEL() ){
		UINT8 au3cks;
		
		au3cks = Dd_Top_Get_CLKSTOP1_AU3CK();
		if( 1 != au3cks ){
			Dd_Top_Set_CLKSTOP1_AU3CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AU3SEL( clksel );
		
		if( 1 != au3cks ){
			Dd_Top_Set_CLKSTOP1_AU3CK( au3cks );
		}
	}
	return FJ_ERR_OK;
}

#if 0	// MILB_Porting
/**
Set Audio ch4 Master clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_au4( FJ_TOP_AU_CLKSEL clksel )
{
	if( clksel >= FJ_TOP_AU_CLKSEL_MAX ){
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_au4() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AU_CLKSEL)Dd_Top_Get_CLKSEL8_AU4SEL() ){
		UINT8 au4cks;
		
		au4cks = Dd_Top_Get_CLKSTOP1_AU4CK();
		if( 1 != au4cks ){
			Dd_Top_Set_CLKSTOP1_AU4CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AU4SEL( clksel );
		
		if( 1 != au4cks ){
			Dd_Top_Set_CLKSTOP1_AU4CK( au4cks );
		}
	}
	return FJ_ERR_OK;
}

/**
Set Audio ch5 Master clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_au5( FJ_TOP_AU_CLKSEL clksel )
{
	if( clksel >= FJ_TOP_AU_CLKSEL_MAX ){
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_au5() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AU_CLKSEL)Dd_Top_Get_CLKSEL8_AU5SEL() ){
		UINT8 au5cks;
		
		au5cks = Dd_Top_Get_CLKSTOP1_AU5CK();
		if( 1 != au5cks ){
			Dd_Top_Set_CLKSTOP1_AU5CK( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_AU5SEL( clksel );
		
		if( 1 != au5cks ){
			Dd_Top_Set_CLKSTOP1_AU5CK( au5cks );
		}
	}
	return FJ_ERR_OK;
}
#endif

/**
Set NETSEC clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_netau( FJ_TOP_AU_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_AU_CLKSEL_APLCLK) &&
		(clksel != FJ_TOP_AU_CLKSEL_EXTERNAL) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_netau() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AU_CLKSEL)Dd_Top_Get_CLKSEL8_NETAUSEL()){
		UINT8 netaucks;
		
		netaucks = Dd_Top_Get_CLKSTOP1_NETAUCK();
		if( 1 != netaucks ){
			Dd_Top_Set_CLKSTOP1_NETAUCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL8_NETAUSEL( clksel );
		
		if( 1 != netaucks ){
			Dd_Top_Set_CLKSTOP1_NETAUCK( netaucks );
		}
	}

	return FJ_ERR_OK;
}

/**
Set SPI clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_spi( FJ_TOP_SPI_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_SPI_CLKSEL_216MHZ) &&
		(clksel != FJ_TOP_SPI_CLKSEL_108MHZ) &&
		(clksel != FJ_TOP_SPI_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_spi() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SPI_CLKSEL)Dd_Top_Get_CLKSEL8_SPICLK()){
		Dd_Top_Set_CLKSEL8_SPICLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set DSP clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_dsp( FJ_TOP_DSP_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_DSP_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_DSP_CLKSEL_266MHZ) &&
		(clksel != FJ_TOP_DSP_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_DSP_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_dsp() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_DSP_CLKSEL)Dd_Top_Get_CLKSEL8_DSPCLK()){
		Dd_Top_Set_CLKSEL8_DSPCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set AXI clock select (except for EXS BLOCK).
*/
FJ_ERR_CODE fj_top_set_clock_select_axi( FJ_TOP_AXI_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_AXI_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_AXI_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_axi() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AXI_CLKSEL)Dd_Top_Get_CLKSEL9_ACLK()){
		Dd_Top_Set_CLKSEL9_ACLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set EXS BLOCK AXI clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_eaxi( FJ_TOP_EAXI_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_EAXI_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_EAXI_CLKSEL_160MHZ) &&
		(clksel != FJ_TOP_EAXI_CLKSEL_133MHZ) &&
		(clksel != FJ_TOP_EAXI_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_eaxi() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_EAXI_CLKSEL)Dd_Top_Get_CLKSEL9_ACLKEXS()){
		Dd_Top_Set_CLKSEL9_ACLKEXS( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set BMH BLOCK AHB clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_bahb( FJ_TOP_BAHB_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_BAHB_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_BAHB_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_bahb() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_BAHB_CLKSEL)Dd_Top_Get_CLKSEL9_HCLKBMH()){
		Dd_Top_Set_CLKSEL9_HCLKBMH( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set AHB clock select (except for EXS BLOCK).
*/
FJ_ERR_CODE fj_top_set_clock_select_ahb( FJ_TOP_AHB_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_AHB_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_AHB_CLKSEL_50MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_ahb() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AHB_CLKSEL)Dd_Top_Get_CLKSEL9_HCLK()){
		Dd_Top_Set_CLKSEL9_HCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set APB clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_apb( FJ_TOP_APB_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_APB_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_APB_CLKSEL_25MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_apb() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_APB_CLKSEL)Dd_Top_Get_CLKSEL9_PCLK()){
		Dd_Top_Set_CLKSEL9_PCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set MXI400 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_mxi400( FJ_TOP_MXI400_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_MXI400_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_MXI400_CLKSEL_200MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_mxi400() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_MXI400_CLKSEL)Dd_Top_Get_CLKSEL10_MCLK400()){
		Dd_Top_Set_CLKSEL10_MCLK400( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set MXI200 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_mxi200( FJ_TOP_MXI200_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_MXI200_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_MXI200_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_mxi200() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_MXI200_CLKSEL)Dd_Top_Get_CLKSEL10_MCLK200()){
		Dd_Top_Set_CLKSEL10_MCLK200( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set AXI400 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_axi400( FJ_TOP_AXI400_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_AXI400_CLKSEL_400MHZ) &&
		(clksel != FJ_TOP_AXI400_CLKSEL_200MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_axi400() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AXI400_CLKSEL)Dd_Top_Get_CLKSEL10_ACLK400()){
		Dd_Top_Set_CLKSEL10_ACLK400( clksel );
	}

	return FJ_ERR_OK;
}

/**
Request setting of select for CPU, L2C, CPU Peripheral, EAXI, AXI, AHB, and APB clocks.
*/
FJ_ERR_CODE fj_top_request_clock_select_dchreq( UINT8 req )
{
	if( req > 1 ) {
		BF_Debug_Print_Warning(( "fj_top_request_clock_select_dchreq() - clksel is error! : req=%d\n", req ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
// Bus Clock Errata
//	if( 1 == req ) {
//		Dd_Top_Set_CLKSEL11_DCHREQ( req );
//	}
	
	return FJ_ERR_OK;
}

/**
Set RIBERY clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_rib( FJ_TOP_RIB_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_RIB_CLKSEL_466MHZ) &&
		(clksel != FJ_TOP_RIB_CLKSEL_350MHZ) &&
		(clksel != FJ_TOP_RIB_CLKSEL_233MHZ) &&
		(clksel != FJ_TOP_RIB_CLKSEL_116MHZ) &&
		(clksel != FJ_TOP_RIB_CLKSEL_58MHZ) &&
		(clksel != FJ_TOP_RIB_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_rib() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_RIB_CLKSEL)Dd_Top_Get_CLKSEL12_RIBCLK()){
		UINT8 last;
		last = Dd_Top_Get_CLKSTOP8_RIBCK();
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_RIBCK( 1 );
		}
		
		Dd_Top_Set_CLKSEL12_RIBCLK( clksel );
		
		if( 1 != last ){
			Dd_Top_Set_CLKSTOP8_RIBCK( last );
		}
	}

	return FJ_ERR_OK;
}

/**
Set NFBCH clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_nfbch( FJ_TOP_NFBCH_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_NFBCH_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_NFBCH_CLKSEL_100MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_nfbch() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_NFBCH_CLKSEL)Dd_Top_Get_CLKSEL12_NFBCHCLK()){
		Dd_Top_Set_CLKSEL12_NFBCHCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set ME clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_me( FJ_TOP_ME_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_ME_CLKSEL_466MHZ) &&
		(clksel != FJ_TOP_ME_CLKSEL_233MHZ) &&
		(clksel != FJ_TOP_ME_CLKSEL_116MHZ) &&
		(clksel != FJ_TOP_ME_CLKSEL_58MHZ) &&
		(clksel != FJ_TOP_ME_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_me() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_ME_CLKSEL)Dd_Top_Get_CLKSEL12_MECLK()){
		Dd_Top_Set_CLKSEL12_MECLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set FPT1 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_fpt1( FJ_TOP_FPT_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_FPT_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_fpt1() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_FPT_CLKSEL)Dd_Top_Get_CLKSEL12_FPT1CLK()){
		Dd_Top_Set_CLKSEL12_FPT1CLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set FPT0 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_fpt0( FJ_TOP_FPT_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_FPT_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_150MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_75MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_60MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_50MHZ) &&
		(clksel != FJ_TOP_FPT_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_fpt0() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_FPT_CLKSEL)Dd_Top_Get_CLKSEL12_FPT0CLK()){
		Dd_Top_Set_CLKSEL12_FPT0CLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set SHDR clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_shdr( FJ_TOP_SHDR_CLKSEL clksel )
{
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if ( (clksel != FJ_TOP_SHDR_CLKSEL_400MHZ) &&
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if ( (clksel != FJ_TOP_SHDR_CLKSEL_432MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_400MHZ) &&
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		(clksel != FJ_TOP_SHDR_CLKSEL_266MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_200MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_160MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_133MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_80MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_66MHZ) &&
		(clksel != FJ_TOP_SHDR_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_shdr() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_SHDR_CLKSEL)Dd_Top_Get_CLKSEL12_SHDRCLK()){
		Dd_Top_Set_CLKSEL12_SHDRCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set GYRO clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_gyro( FJ_TOP_GYRO_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_GYRO_CLKSEL_100MHZ) &&
		(clksel != FJ_TOP_GYRO_CLKSEL_32MHZ) &&
		(clksel != FJ_TOP_GYRO_CLKSEL_2MHZ) &&
		(clksel != FJ_TOP_GYRO_CLKSEL_OFF) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_gyro() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_GYRO_CLKSEL)Dd_Top_Get_CLKSEL12_GYROCLK()){
		Dd_Top_Set_CLKSEL12_GYROCLK( clksel );
	}

	return FJ_ERR_OK;
}

/**
Set AXI300 clock select.
*/
FJ_ERR_CODE fj_top_set_clock_select_axi300( FJ_TOP_AXI300_CLKSEL clksel )
{
	if ( (clksel != FJ_TOP_AXI300_CLKSEL_300MHZ) &&
		(clksel != FJ_TOP_AXI300_CLKSEL_200MHZ) ) {
		BF_Debug_Print_Warning(( "fj_top_set_clock_select_axi300() - clksel is error! : clksel=%d\n", clksel ));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( clksel != (FJ_TOP_AXI300_CLKSEL)Dd_Top_Get_CLKSEL12_ACLK300()){
		Dd_Top_Set_CLKSEL12_ACLK300( clksel );
	}

	return FJ_ERR_OK;
}

/**
Assert DSP reset.
*/
VOID fj_top_assert_reset_dsp( VOID )
{
	Dd_Top_Reset_CRRRS_DSPSR();
}

/**
Negate DSP reset.
*/
VOID fj_top_negate_reset_dsp( VOID )
{
	Dd_Top_Clear_CRRRS_DSPSR();
}

/**
Get DSP reset state.
*/
ULONG fj_top_get_reset_state_dsp( VOID )
{
	return Dd_Top_Get_CRRRS_DSPSR();
}

/**
Get clock frequency of ARM Cortex-A7 MPCore Peripheral.
*/
ULONG fj_top_get_clock_core_peripheral( VOID )
{
	return Dd_Top_Get_RCLK();
}

FJ_ERR_CODE fj_top_set_image_clock( FJ_TOP_IMAGE_CLOCK image_clock )
{
	FJ_ERR_CODE ercd;

	if( FJ_TOP_RAW_CLKSEL_OFF == image_clock.raw ) {
		Dd_Top_Set_CLKSTOP7_RAWCK( 1 );
	}
	else {
		if( image_clock.raw != (FJ_TOP_RAW_CLKSEL)Dd_Top_Get_CLKSEL2_RAWCLK() ) {
			Dd_Top_Set_CLKSTOP7_RAWCK( 1 );
			ercd = fj_top_set_clock_select_raw( image_clock.raw );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP7_RAWCK() ) {
			Dd_Top_Set_CLKSTOP7_RAWCK( 0 );
		}
	}

	if( FJ_TOP_JPG_CLKSEL_OFF == image_clock.jpg ) {
		Dd_Top_Set_CLKSTOP3_JPGCK( 1 );
	}
	else {
		if( image_clock.jpg != (FJ_TOP_JPG_CLKSEL)Dd_Top_Get_CLKSEL2_JPGCLK() ) {
			Dd_Top_Set_CLKSTOP3_JPGCK( 1 );
			ercd = fj_top_set_clock_select_jpeg( image_clock.jpg );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP3_JPGCK() ) {
			Dd_Top_Set_CLKSTOP3_JPGCK( 0 );
		}
	}

	if( FJ_TOP_ELA_CLKSEL_OFF == image_clock.ela ) {
		Dd_Top_Set_CLKSTOP6_ELACK( 1 );
	}
	else {
		if( image_clock.ela != (FJ_TOP_ELA_CLKSEL)Dd_Top_Get_CLKSEL2_ELACLK() ){
			Dd_Top_Set_CLKSTOP6_ELACK( 1 );
			ercd = fj_top_set_clock_select_ela( image_clock.ela );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP6_ELACK() ) {
			Dd_Top_Set_CLKSTOP6_ELACK( 0 );
		}
	}

	if( FJ_TOP_SEN_CLKSEL_OFF == image_clock.sen ) {
		Dd_Top_Set_CLKSTOP1_SENCK( 1 );
	}
	else {
		if( image_clock.sen != (FJ_TOP_SEN_CLKSEL)Dd_Top_Get_CLKSEL3_SENCLK() ){
			Dd_Top_Set_CLKSTOP1_SENCK( 1 );
			ercd = fj_top_set_clock_select_sen( image_clock.sen );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP1_SENCK() ) {
			Dd_Top_Set_CLKSTOP1_SENCK( 0 );
		}
	}

	if( FJ_TOP_SENMSK_CLKSEL_OFF == image_clock.senmsk ) {
		Dd_Top_Set_CLKSTOP14_SENMSKCK( 1 );
	}
	else {
		if( image_clock.senmsk != (FJ_TOP_SENMSK_CLKSEL)Dd_Top_Get_CLKSEL3_SENMSKCLK() ){
			Dd_Top_Set_CLKSTOP14_SENMSKCK( 1 );
			ercd = fj_top_set_clock_select_senmsk( image_clock.senmsk );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP14_SENMSKCK() ) {
			Dd_Top_Set_CLKSTOP14_SENMSKCK( 0 );
		}
	}

	if( FJ_TOP_IIP_CLKSEL_OFF == image_clock.iip ) {
		Dd_Top_Set_CLKSTOP3_IIPCK( 1 );
	}
	else {
		if( image_clock.iip != (FJ_TOP_IIP_CLKSEL)Dd_Top_Get_CLKSEL3_IIPCLK()){
			Dd_Top_Set_CLKSTOP3_IIPCK( 1 );
			ercd = fj_top_set_clock_select_iip( image_clock.iip );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP3_IIPCK() ) {
			Dd_Top_Set_CLKSTOP3_IIPCK( 0 );
		}
	}

	if( FJ_TOP_PAS_CLKSEL_OFF == image_clock.pas ) {
		Dd_Top_Set_CLKSTOP14_PASCK( 1 );
	}
	else {
		if( image_clock.pas != (FJ_TOP_PAS_CLKSEL)Dd_Top_Get_CLKSEL3_PASCLK()){
			Dd_Top_Set_CLKSTOP14_PASCK( 1 );
			ercd = fj_top_set_clock_select_pas( image_clock.pas );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP14_PASCK() ) {
			Dd_Top_Set_CLKSTOP14_PASCK( 0 );
		}
	}

	if( FJ_TOP_SRO_CLKSEL_OFF == image_clock.sro1 ) {
		Dd_Top_Set_CLKSTOP10_SRO1CK( 1 );
	}
	else {
		if( image_clock.sro1 != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL5_SRO1CLK()){
			Dd_Top_Set_CLKSTOP10_SRO1CK( 1 );
			ercd = fj_top_set_clock_select_sro1( image_clock.sro1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP10_SRO1CK() ) {
			Dd_Top_Set_CLKSTOP10_SRO1CK( 0 );
		}
	}

	if( FJ_TOP_SRO_CLKSEL_OFF == image_clock.sro1_2 ) {
		Dd_Top_Set_CLKSTOP10_SRO1CK_2( 1 );
	}
	else {
		if( image_clock.sro1_2 != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL4_SRO1CLK_2()){
			Dd_Top_Set_CLKSTOP10_SRO1CK_2( 1 );
			ercd = fj_top_set_clock_select_sro1_2( image_clock.sro1_2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP10_SRO1CK_2() ) {
			Dd_Top_Set_CLKSTOP10_SRO1CK_2( 0 );
		}
	}

	if( FJ_TOP_B2B1_CLKSEL_OFF == image_clock.b2b1 ) {
		Dd_Top_Set_CLKSTOP10_B2B1CK( 1 );
	}
	else {
		if( image_clock.b2b1 != (FJ_TOP_B2B1_CLKSEL)Dd_Top_Get_CLKSEL5_B2B1CLK()){
			Dd_Top_Set_CLKSTOP10_B2B1CK( 1 );
			ercd = fj_top_set_clock_select_b2b1( image_clock.b2b1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP10_B2B1CK() ) {
			Dd_Top_Set_CLKSTOP10_B2B1CK( 0 );
		}
	}

	if( FJ_TOP_B2R1_CLKSEL_OFF == image_clock.b2r1 ) {
		Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );
	}
	else {
		if( image_clock.b2r1 != (FJ_TOP_B2R1_CLKSEL)Dd_Top_Get_CLKSEL4_B2R1CLK()){
			Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );
			ercd = fj_top_set_clock_select_b2r1( image_clock.b2r1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP10_B2R1CK() ) {
			Dd_Top_Set_CLKSTOP10_B2R1CK( 0 );
		}
	}

	if( FJ_TOP_LTM1_CLKSEL_OFF == image_clock.ltm1 ) {
		Dd_Top_Set_CLKSTOP11_LTM1CK( 1 );
	}
	else {
		if( image_clock.ltm1 != (FJ_TOP_LTM1_CLKSEL)Dd_Top_Get_CLKSEL4_LTM1CLK()){
			Dd_Top_Set_CLKSTOP11_LTM1CK( 1 );
			ercd = fj_top_set_clock_select_ltm1( image_clock.ltm1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP11_LTM1CK() ) {
			Dd_Top_Set_CLKSTOP11_LTM1CK( 0 );
		}
	}

	if( FJ_TOP_LTMRBK1_CLKSEL_OFF == image_clock.ltmrbk1 ) {
		Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
	}
	else {
		if( image_clock.ltmrbk1 != (FJ_TOP_LTMRBK1_CLKSEL)Dd_Top_Get_CLKSEL5_LTMRBK1CLK()){
			Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
			ercd = fj_top_set_clock_select_ltmrbk1( image_clock.ltmrbk1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP10_LTMRBK1CK() ) {
			Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 0 );
		}
	}

	if( FJ_TOP_R2Y_CLKSEL_OFF == image_clock.r2y1 ) {
		Dd_Top_Set_CLKSTOP11_R2Y1CK( 1 );
	}
	else {
		if( image_clock.r2y1 != (FJ_TOP_R2Y_CLKSEL)Dd_Top_Get_CLKSEL4_R2Y1CLK()){
			Dd_Top_Set_CLKSTOP11_R2Y1CK( 1 );
			ercd = fj_top_set_clock_select_r2y1( image_clock.r2y1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP11_R2Y1CK() ) {
			Dd_Top_Set_CLKSTOP11_R2Y1CK( 0 );
		}
	}

	if( FJ_TOP_CNR1_CLKSEL_OFF == image_clock.cnr1 ) {
		Dd_Top_Set_CLKSTOP11_CNR1CK( 1 );
	}
	else {
		if( image_clock.cnr1 != (FJ_TOP_CNR1_CLKSEL)Dd_Top_Get_CLKSEL4_CNR1CLK()){
			Dd_Top_Set_CLKSTOP11_CNR1CK( 1 );
			ercd = fj_top_set_clock_select_cnr1( image_clock.cnr1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP11_CNR1CK() ) {
			Dd_Top_Set_CLKSTOP11_CNR1CK( 0 );
		}
	}

	if( FJ_TOP_SRO_CLKSEL_OFF == image_clock.sro2 ) {
		Dd_Top_Set_CLKSTOP12_SRO2CK( 1 );
	}
	else {
		if( image_clock.sro2 != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL7_SRO2CLK()){
			Dd_Top_Set_CLKSTOP12_SRO2CK( 1 );
			ercd = fj_top_set_clock_select_sro2( image_clock.sro2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP12_SRO2CK() ) {
			Dd_Top_Set_CLKSTOP12_SRO2CK( 0 );
		}
	}

	if( FJ_TOP_SRO_CLKSEL_OFF == image_clock.sro2_2 ) {
		Dd_Top_Set_CLKSTOP12_SRO2CK_2( 1 );
	}
	else {
		if( image_clock.sro2_2 != (FJ_TOP_SRO_CLKSEL)Dd_Top_Get_CLKSEL6_SRO2CLK_2()){
			Dd_Top_Set_CLKSTOP12_SRO2CK_2( 1 );
			ercd = fj_top_set_clock_select_sro2_2( image_clock.sro2_2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP12_SRO2CK_2() ) {
			Dd_Top_Set_CLKSTOP12_SRO2CK_2( 0 );
		}
	}

	if( FJ_TOP_B2B2_CLKSEL_OFF == image_clock.b2b2 ) {
		Dd_Top_Set_CLKSTOP12_B2B2CK( 1 );
	}
	else {
		if( image_clock.b2b2 != (FJ_TOP_B2B2_CLKSEL)Dd_Top_Get_CLKSEL7_B2B2CLK()){
			Dd_Top_Set_CLKSTOP12_B2B2CK( 1 );
			ercd = fj_top_set_clock_select_b2b2( image_clock.b2b2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP12_B2B2CK() ) {
			Dd_Top_Set_CLKSTOP12_B2B2CK( 0 );
		}
	}

	if( FJ_TOP_B2R2_CLKSEL_OFF == image_clock.b2r2 ) {
		Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );
	}
	else {
		if( image_clock.b2r2 != (FJ_TOP_B2R2_CLKSEL)Dd_Top_Get_CLKSEL6_B2R2CLK()){
			Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );
			ercd = fj_top_set_clock_select_b2r2( image_clock.b2r2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP12_B2R2CK() ) {
			Dd_Top_Set_CLKSTOP12_B2R2CK( 0 );
		}
	}

	if( FJ_TOP_LTM2_CLKSEL_OFF == image_clock.ltm2 ) {
		Dd_Top_Set_CLKSTOP13_LTM2CK( 1 );
	}
	else {
		if( image_clock.ltm2 != (FJ_TOP_LTM2_CLKSEL)Dd_Top_Get_CLKSEL6_LTM2CLK()){
			Dd_Top_Set_CLKSTOP13_LTM2CK( 1 );
			ercd = fj_top_set_clock_select_ltm2( image_clock.ltm2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP13_LTM2CK() ) {
			Dd_Top_Set_CLKSTOP13_LTM2CK( 0 );
		}
	}

	if( FJ_TOP_LTMRBK2_CLKSEL_OFF == image_clock.ltmrbk2 ) {
		Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
	}
	else {
		if( image_clock.ltmrbk2 != (FJ_TOP_LTMRBK2_CLKSEL)Dd_Top_Get_CLKSEL7_LTMRBK2CLK()){
			Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
			ercd = fj_top_set_clock_select_ltmrbk2( image_clock.ltmrbk2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP12_LTMRBK2CK() ) {
			Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 0 );
		}
	}

	if( FJ_TOP_R2Y_CLKSEL_OFF == image_clock.r2y2 ) {
		Dd_Top_Set_CLKSTOP13_R2Y2CK( 1 );
	}
	else {
		if( image_clock.r2y2 != (FJ_TOP_R2Y_CLKSEL)Dd_Top_Get_CLKSEL6_R2Y2CLK()){
			Dd_Top_Set_CLKSTOP13_R2Y2CK( 1 );
			ercd = fj_top_set_clock_select_r2y2( image_clock.r2y2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP13_R2Y2CK() ) {
			Dd_Top_Set_CLKSTOP13_R2Y2CK( 0 );
		}
	}

	if( FJ_TOP_CNR2_CLKSEL_OFF == image_clock.cnr2 ) {
		Dd_Top_Set_CLKSTOP13_CNR2CK( 1 );
	}
	else {
		if( image_clock.cnr2 != (FJ_TOP_CNR2_CLKSEL)Dd_Top_Get_CLKSEL6_CNR2CLK()){
			Dd_Top_Set_CLKSTOP13_CNR2CK( 1 );
			ercd = fj_top_set_clock_select_cnr2( image_clock.cnr2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP13_CNR2CK() ) {
			Dd_Top_Set_CLKSTOP13_CNR2CK( 0 );
		}
	}

	if( image_clock.apclk != (FJ_TOP_APCLK_CLKSEL)Dd_Top_Get_CLKSEL8_APCLK()){
		ercd = fj_top_set_clock_select_apclk( image_clock.apclk );
		if( FJ_ERR_OK != ercd ) {
			return ercd;
		}
	}

	if( FJ_TOP_SNAPCLKO_CLKSEL_OFF == image_clock.apclk0 ) {
		Dd_Top_Set_CLKSTOP4_APCK0( 1 );
	}
	else {
		if( image_clock.apclk0 != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP0SEL()){
			Dd_Top_Set_CLKSTOP4_APCK0( 1 );
			ercd = fj_top_set_clock_select_ap0( image_clock.apclk0 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_APCK0() ) {
			Dd_Top_Set_CLKSTOP4_APCK0( 0 );
		}
	}

	if( FJ_TOP_SNAPCLKO_CLKSEL_OFF == image_clock.apclk1 ) {
		Dd_Top_Set_CLKSTOP4_APCK1( 1 );
	}
	else {
		if( image_clock.apclk1 != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP1SEL()){
			Dd_Top_Set_CLKSTOP4_APCK1( 1 );
			ercd = fj_top_set_clock_select_ap1( image_clock.apclk1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_APCK1() ) {
			Dd_Top_Set_CLKSTOP4_APCK1( 0 );
		}
	}

	if( FJ_TOP_SNAPCLKO_CLKSEL_OFF == image_clock.apclk2 ) {
		Dd_Top_Set_CLKSTOP4_APCK2( 1 );
	}
	else {
		if( image_clock.apclk2 != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP2SEL()){
			Dd_Top_Set_CLKSTOP4_APCK2( 1 );
			ercd = fj_top_set_clock_select_ap2( image_clock.apclk2 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_APCK2() ) {
			Dd_Top_Set_CLKSTOP4_APCK2( 0 );
		}
	}

	if( FJ_TOP_SNAPCLKO_CLKSEL_OFF == image_clock.apclk3 ) {
		Dd_Top_Set_CLKSTOP4_APCK3( 1 );
	}
	else {
		if( image_clock.apclk3 != (FJ_TOP_SNAPCLKO_CLKSEL)Dd_Top_Get_CLKSEL8_AP3SEL()){
			Dd_Top_Set_CLKSTOP4_APCK3( 1 );
			ercd = fj_top_set_clock_select_ap3( image_clock.apclk3 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_APCK3() ) {
			Dd_Top_Set_CLKSTOP4_APCK3( 0 );
		}
	}

	if( FJ_TOP_IPP_CLKSEL_OFF == image_clock.ipp ) {
		Dd_Top_Set_CLKSTOP8_HEIPPCK( 1 );
	}
	else {
		if( image_clock.ipp != (FJ_TOP_IPP_CLKSEL)Dd_Top_Get_CLKSEL3_IPPCLK()){
			Dd_Top_Set_CLKSTOP8_HEIPPCK( 1 );
			ercd = fj_top_set_clock_select_ipp( image_clock.ipp );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP8_HEIPPCK() ) {
			Dd_Top_Set_CLKSTOP8_HEIPPCK( 0 );
		}
	}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	if( FJ_TOP_PXF_CLKSEL_OFF == image_clock.pxf ) {
		Dd_Top_Set_CLKSTOP8_HEPXFCK( 1 );
	}
	else {
		if( image_clock.pxf != (FJ_TOP_PXF_CLKSEL)Dd_Top_Get_CLKSEL3_PXFCLK()){
			Dd_Top_Set_CLKSTOP8_HEPXFCK( 1 );
			ercd = fj_top_set_clock_select_pxf( image_clock.pxf );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP8_HEPXFCK() ) {
			Dd_Top_Set_CLKSTOP8_HEPXFCK( 0 );
		}
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if( FJ_TOP_HEVENC_CLKSEL_OFF == image_clock.hevenc ) {
		Dd_Top_Set_CLKSTOP8_HEVENCCK( 1 );
	}
	else {
		if( image_clock.hevsel != (FJ_TOP_HEVSEL_CLKSEL)Dd_Top_Get_CLKSEL3_HEVENCSEL()){
			Dd_Top_Set_CLKSTOP8_HEVENCCK( 1 );
			ercd = fj_top_set_clock_select_hevsel( image_clock.hevsel );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( image_clock.hevenc != (FJ_TOP_HEVENC_CLKSEL)Dd_Top_Get_CLKSEL3_HEVENCCLK()){
			Dd_Top_Set_CLKSTOP8_HEVENCCK( 1 );
			ercd = fj_top_set_clock_select_hevenc( image_clock.hevenc );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP8_HEVENCCK() ) {
			Dd_Top_Set_CLKSTOP8_HEVENCCK( 0 );
		}
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	if( FJ_TOP_VDF_CLKSEL_OFF == image_clock.vdf ) {
		Dd_Top_Set_CLKSTOP8_HEVDFCK( 1 );
	}
	else {
		if( image_clock.vdf != (FJ_TOP_VDF_CLKSEL)Dd_Top_Get_CLKSEL3_VDFCLK()){
			Dd_Top_Set_CLKSTOP8_HEVDFCK( 1 );
			ercd = fj_top_set_clock_select_vdf( image_clock.vdf );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP8_HEVDFCK() ) {
			Dd_Top_Set_CLKSTOP8_HEVDFCK( 0 );
		}
	}
	
	BF_H264_User_Set_Codec_CoreMode( image_clock.rib_core );
	
	if( FJ_TOP_RIB_CLKSEL_OFF == image_clock.rib ) {
		Dd_Top_Set_CLKSTOP8_RIBCK( 1 );
	}
	else {
		if( image_clock.rib != (FJ_TOP_RIB_CLKSEL)Dd_Top_Get_CLKSEL12_RIBCLK() ){
			Dd_Top_Set_CLKSTOP8_RIBCK( 1 );
			ercd = fj_top_set_clock_select_rib( image_clock.rib );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP8_RIBCK() ) {
			Dd_Top_Set_CLKSTOP8_RIBCK( 0 );
		}
	}

	if( FJ_TOP_FPT_CLKSEL_OFF == image_clock.fpt0 ) {
		Dd_Top_Set_CLKSTOP4_FPT0CK( 1 );
		Dd_Top_Set_CLKSTOP4_FPT0AX( 1 );
		Dd_Top_Set_CLKSTOP4_FPT0AH( 1 );
		Dd_Top_Set_CLKSTOP4_FPT0AP( 1 );
	}
	else {
		if( image_clock.fpt0 != (FJ_TOP_FPT_CLKSEL)Dd_Top_Get_CLKSEL12_FPT0CLK() ){
			Dd_Top_Set_CLKSTOP4_FPT0CK( 1 );
			ercd = fj_top_set_clock_select_fpt0( image_clock.fpt0 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT0CK() ) {
			Dd_Top_Set_CLKSTOP4_FPT0CK( 0 );
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT0AX() ) {
			Dd_Top_Set_CLKSTOP4_FPT0AX( 0 );
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT0AH() ) {
			Dd_Top_Set_CLKSTOP4_FPT0AH( 0 );
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT0AP() ) {
			Dd_Top_Set_CLKSTOP4_FPT0AP( 0 );
		}
	}

	if( FJ_TOP_FPT_CLKSEL_OFF == image_clock.fpt1 ) {
		Dd_Top_Set_CLKSTOP4_FPT1CK( 1 );
		Dd_Top_Set_CLKSTOP4_FPT1AX( 1 );
		Dd_Top_Set_CLKSTOP4_FPT1AH( 1 );
		Dd_Top_Set_CLKSTOP4_FPT1AP( 1 );
	}
	else {
		if( image_clock.fpt1 != (FJ_TOP_FPT_CLKSEL)Dd_Top_Get_CLKSEL12_FPT1CLK() ){
			Dd_Top_Set_CLKSTOP4_FPT1CK( 1 );
			ercd = fj_top_set_clock_select_fpt1( image_clock.fpt1 );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT1CK() ) {
			Dd_Top_Set_CLKSTOP4_FPT1CK( 0 );
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT1AX() ) {
			Dd_Top_Set_CLKSTOP4_FPT1AX( 0 );
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT1AH() ) {
			Dd_Top_Set_CLKSTOP4_FPT1AH( 0 );
		}
		if( 0 != Dd_Top_Get_CLKSTOP4_FPT1AP() ) {
			Dd_Top_Set_CLKSTOP4_FPT1AP( 0 );
		}
	}

	if( FJ_TOP_ME_CLKSEL_OFF == image_clock.me ) {
		Dd_Top_Set_CLKSTOP7_MECK( 1 );
	}
	else {
		if( image_clock.me != (FJ_TOP_ME_CLKSEL)Dd_Top_Get_CLKSEL12_MECLK() ){
			Dd_Top_Set_CLKSTOP7_MECK( 1 );
			ercd = fj_top_set_clock_select_me( image_clock.me );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP7_MECK() ) {
			Dd_Top_Set_CLKSTOP7_MECK( 0 );
		}
	}

	if( FJ_TOP_SHDR_CLKSEL_OFF == image_clock.shdr ) {
		Dd_Top_Set_CLKSTOP7_SHDRCK( 1 );
	}
	else {
		if( image_clock.shdr != (FJ_TOP_SHDR_CLKSEL)Dd_Top_Get_CLKSEL12_SHDRCLK() ){
			Dd_Top_Set_CLKSTOP7_SHDRCK( 1 );
			ercd = fj_top_set_clock_select_shdr( image_clock.shdr );
			if( FJ_ERR_OK != ercd ) {
				return ercd;
			}
		}
		if( 0 != Dd_Top_Get_CLKSTOP7_SHDRCK() ) {
			Dd_Top_Set_CLKSTOP7_SHDRCK( 0 );
		}
	}

	// IPU
	ercd = fj_top_set_clock_select_ipu( image_clock.ipu );
	if( FJ_ERR_OK != ercd ) {
		return ercd;
	}
	ercd = fj_top_set_clock_select_ipu_tme( image_clock.ipu_tme );
	if( FJ_ERR_OK != ercd ) {
		return ercd;
	}

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_top_set_clock_stop_micax0( UCHAR val )
{
	if( 1 < val ) {
		return FJ_ERR_NG_INPUT_PARAM;
	}
	Dd_Top_Set_CLKSTOP5_MICAX0( val );
	return FJ_ERR_OK;
}

