/**
 * @file		ct_im_sdramc.c
 * @brief		This is ct code for im_sdramc.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_sdramc.h"

#include "sdramc.h"
#include "im_sdramc.h"
#include <string.h>
#include <stdlib.h>

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
#endif //CO_ACT_CLOCK | CO_ACT_HCLOCK | CO_ACT_PCLOCK

#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
static VOID ct_sdramc_callback( ULONG int_cause )
{
	Ddim_Print(("ct_sdramc_callback. int_cause=%08lx\n", int_cause));
}

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
VOID CT_Im_SDRAMC_1_01( UCHAR ch )
{
	E_IM_SDRAMC_PORT			port;
	T_IM_SDRAMC_CMD_ARB_RATE	arb_rate;

	Ddim_Print(("CT_Im_SDRAMC_1_01(test ch=%d)\n", ch));

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		arb_rate.throughput[ port ]	= 0xFFF;
		arb_rate.latency[ port ]	= 0xFFF;
		arb_rate.enable[ port ]		= E_IM_SDRAMC_EN_ENABLE;
	}

	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( (E_IM_SDRAMC_CH)ch, &arb_rate );

	memset( &arb_rate, 0, sizeof(arb_rate) );

	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( (E_IM_SDRAMC_CH)ch, &arb_rate );

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		Ddim_Print( ("arb_rate.throughput[%d] = %d\n", port, arb_rate.throughput[ port ] ) );
		Ddim_Print( ("arb_rate.latency[%d]    = %d\n", port, arb_rate.latency[ port ]    ) );
		Ddim_Print( ("arb_rate.enable[%d]     = %d\n", port, arb_rate.enable[ port ]     ) );
	}

	Ddim_Print(("\n"));
	Ddim_Print(("CT_Im_SDRAMC_1_01(reset ch=%d)\n", ch));

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		arb_rate.throughput[ port ]	= 0;
		arb_rate.latency[ port ]	= 0;
		arb_rate.enable[ port ]		= E_IM_SDRAMC_EN_DISABLE;
	}

	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( (E_IM_SDRAMC_CH)ch, &arb_rate );

	memset( &arb_rate, 0, sizeof(arb_rate) );

	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( (E_IM_SDRAMC_CH)ch, &arb_rate );

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		Ddim_Print( ("arb_rate.throughput[%d] = %d\n", port, arb_rate.throughput[ port ] ) );
		Ddim_Print( ("arb_rate.latency[%d]    = %d\n", port, arb_rate.latency[ port ]    ) );
		Ddim_Print( ("arb_rate.enable[%d]     = %d\n", port, arb_rate.enable[ port ]     ) );
	}

	Ddim_Print(("CT_Im_SDRAMC_1_01 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_02( UCHAR ch )
{
	E_IM_SDRAMC_PORT			port;
	T_IM_SDRAMC_CMD_ARB_PRI		arb_pri;

	Ddim_Print(("CT_Im_SDRAMC_1_02(test ch=%d)\n", ch));

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		arb_pri.priority[ port ]	= 0x1F;
		arb_pri.enable[ port ]		= E_IM_SDRAMC_EN_ENABLE;
	}

	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( (E_IM_SDRAMC_CH)ch, &arb_pri );

	memset( &arb_pri, 0, sizeof(arb_pri) );

	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( (E_IM_SDRAMC_CH)ch, &arb_pri );

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		Ddim_Print( ("arb_pri.priority[%d] = %d\n", port, arb_pri.priority[ port ] ) );
		Ddim_Print( ("arb_pri.enable[%d]   = %d\n", port, arb_pri.enable[ port ] ) );
	}

	Ddim_Print(("\n"));
	Ddim_Print(("CT_Im_SDRAMC_1_02(reset ch=%d)\n", ch));

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		arb_pri.priority[ port ]	= 0;
		arb_pri.enable[ port ]		= E_IM_SDRAMC_EN_DISABLE;
	}

	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( (E_IM_SDRAMC_CH)ch, &arb_pri );

	memset( &arb_pri, 0, sizeof(arb_pri) );

	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( (E_IM_SDRAMC_CH)ch, &arb_pri );

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		Ddim_Print( ("arb_pri.priority[%d] = %d\n", port, arb_pri.priority[ port ] ) );
		Ddim_Print( ("arb_pri.enable[%d]   = %d\n", port, arb_pri.enable[ port ] ) );
	}

	Ddim_Print(("CT_Im_SDRAMC_1_02 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_03( UCHAR ch )
{
	UCHAR				data;
	E_IM_SDRAMC_RANK	rank	= E_IM_SDRAMC_RANK_0;

	Ddim_Print(("CT_Im_SDRAMC_1_03(ch=%d)\n", ch));

	IO_SDRAMC[ ch ].UMCMRACSST.bit.MRST = 1;

	(VOID)Im_SDRAMC_Set_Mode_Register( (E_IM_SDRAMC_CH)ch, rank, 0, 0xFF );

	(VOID)Im_SDRAMC_Get_Mode_Register( (E_IM_SDRAMC_CH)ch, rank, 0, &data );

	Ddim_Print( ("data = %d\n", data  ) );

	Ddim_Print(("CT_Im_SDRAMC_1_03 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_04( UCHAR ch )
{
	E_IM_SDRAMC_PORT port;

	Ddim_Print(("CT_Im_SDRAMC_1_04(ch=%d)\n", ch));

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {

		Ddim_Print( ("Bus IF(read %d) on\n", port) );
		(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( (E_IM_SDRAMC_CH)ch, port, E_IM_SDRAMC_ACCESS_READ, E_IM_SDRAMC_EN_ENABLE );

		Ddim_Print( ("Bus IF(read %d) off\n", port) );
		(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( (E_IM_SDRAMC_CH)ch, port, E_IM_SDRAMC_ACCESS_READ, E_IM_SDRAMC_EN_DISABLE );

		Ddim_Print( ("Bus IF(write %d) on\n", port) );
		(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( (E_IM_SDRAMC_CH)ch, port, E_IM_SDRAMC_ACCESS_WRITE, E_IM_SDRAMC_EN_ENABLE );

		Ddim_Print( ("Bus IF(write %d) off\n", port) );
		(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( (E_IM_SDRAMC_CH)ch, port, E_IM_SDRAMC_ACCESS_WRITE, E_IM_SDRAMC_EN_DISABLE );

	}

	Ddim_Print(("CT_Im_SDRAMC_1_04 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_05( UCHAR ch )
{
	E_IM_SDRAMC_DATABUF	dbno;

	Ddim_Print(("CT_Im_SDRAMC_1_05(ch=%d)\n", ch));

	for ( dbno = E_IM_SDRAMC_DATABUF_0; dbno < E_IM_SDRAMC_DATABUF_MAX; dbno++ ) {

		Ddim_Print( ("Data buf(%d) on\n", dbno) );
		(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( (E_IM_SDRAMC_CH)ch, dbno, E_IM_SDRAMC_EN_ENABLE );

		Ddim_Print( ("Data buf(%d) off\n", dbno) );
		(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( (E_IM_SDRAMC_CH)ch, dbno, E_IM_SDRAMC_EN_DISABLE );
	}

	Ddim_Print(("CT_Im_SDRAMC_1_05 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_06( UCHAR ch )
{
	Ddim_Print(("CT_Im_SDRAMC_1_06(ch=%d)\n", ch));

//	IO_SDRAMC[ ch ].UMCSPCSTAT.bit.SLFRST = 1;

	Ddim_Print( ("refresh start ch = %d\n", ch) );
	Ddim_Print(("\n"));
	(VOID)Im_SDRAMC_Strat_SelfRefresh( (E_IM_SDRAMC_CH)ch );

//	IO_SDRAMC[ ch ].UMCSPCSTAT.bit.SLFRST = 0;

	Ddim_Print( ("refresh stop ch = %d\n", ch) );
	Ddim_Print(("\n"));
	(VOID)Im_SDRAMC_Stop_SelfRefresh( (E_IM_SDRAMC_CH)ch );

	Ddim_Print(("CT_Im_SDRAMC_1_06 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_07( UCHAR ch )
{
	T_IM_SDRAMC_ACCESS_DETECTION	acs_det;
	E_IM_SDRAMC_AREA				area = E_IM_SDRAMC_AREA_A;
	E_IM_SDRAMC_PORT				port;
	ULONG							addr;

	Ddim_Print(("CT_Im_SDRAMC_1_07(ch=%d)\n", ch));

	Ddim_Print( ("ch   = %d\n", ch  ) );
	Ddim_Print( ("area = %d\n", area) );
	Ddim_Print( ("Access detection start\n") );
	Ddim_Print(("\n"));

	acs_det.s_addr		= 0x48000000;
	acs_det.e_addr		= 0x49000000;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	acs_det.w_port		= 1;
	acs_det.r_port		= 2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	acs_det.w_port_l	= 1;
	acs_det.r_port_l	= 2;
	acs_det.w_port_u	= 1;
	acs_det.r_port_u	= 2;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	acs_det.pCallBack	= (T_IM_SDRAMC_CALLBACK)ct_sdramc_callback;

	(VOID)Im_SDRAMC_Start_Access_Detection( (E_IM_SDRAMC_CH)ch, area, &acs_det );

	Ddim_Print( ("Access detection stop\n") );
	Ddim_Print(("\n"));

	(VOID)Im_SDRAMC_Stop_Access_Detection( (E_IM_SDRAMC_CH)ch, area );

	(VOID)Im_SDRAMC_Get_Access_Detection( (E_IM_SDRAMC_CH)ch, &area, &port, &addr );

	Ddim_Print( ("Access detection get\n") );
	Ddim_Print( ("area = %d\n",      area) );
	Ddim_Print( ("port = %d\n",      port) );
	Ddim_Print( ("addr = 0x%08lx\n", addr) );
	Ddim_Print(("\n"));

	Ddim_Print(("CT_Im_SDRAMC_1_07 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_08( UCHAR ch )
{
	E_IM_SDRAMC_MONMODE	mode	= E_IM_SDRAMC_MONMODE_SIZE;
	E_IM_SDRAMC_PORT	port;
	T_IM_SDRAMC_MONITOR	mon;

	Ddim_Print(("CT_Im_SDRAMC_1_08(ch=%d)\n", ch));

	Ddim_Print( ("ch   = %d\n", ch  ) );
	Ddim_Print( ("mode = %d\n", mode) );
	Ddim_Print( ("Monitor start\n") );
	Ddim_Print(("\n"));

	(VOID)Im_SDRAMC_Start_Monitor( (E_IM_SDRAMC_CH)ch, mode, TRUE, TRUE, TRUE );
	
	Ddim_Print( ("ch   = %d\n", ch  ) );
	Ddim_Print( ("Monitor stop\n") );
	Ddim_Print(("\n"));

	(VOID)Im_SDRAMC_Stop_Monitor( (E_IM_SDRAMC_CH)ch );

	Ddim_Print( ("Monitor get\n") );
	Ddim_Print( ("ch   = %d\n", ch  ) );

	memset( &mon, 0, sizeof(mon) );

	(VOID)Im_SDRAMC_Get_Monitor( (E_IM_SDRAMC_CH)ch, &mon );

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		Ddim_Print( ("mon.data[%d] = %lu\n", port, mon.data[ port ] ) );
	}

	Ddim_Print(("CT_Im_SDRAMC_1_08 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_09( UCHAR ch )
{
	E_IM_SDRAMC_MONMODE			mode		= E_IM_SDRAMC_MONMODE_SIZE;
	UCHAR						sub_id;
	T_IM_SDRAMC_MONITOR_MXIC	monmxic;

	Ddim_Print(("CT_Im_SDRAMC_1_09(ch=%d)\n", ch));

	Ddim_Print( ("ch   = %d\n", ch  ) );
	Ddim_Print( ("mode = %d\n", mode) );
	Ddim_Print( ("Monitor for MXIC start\n") );
	Ddim_Print(("\n"));

	(VOID)Im_SDRAMC_Start_Monitor_Mxic( (E_IM_SDRAMC_CH)ch, mode, TRUE, TRUE, TRUE );

	Ddim_Print( ("ch   = %d\n", ch  ) );
	Ddim_Print( ("Monitor for MXIC stop\n") );
	Ddim_Print(("\n"));

	(VOID)Im_SDRAMC_Stop_Monitor_Mxic( (E_IM_SDRAMC_CH)ch );

	Ddim_Print( ("Monitor for MXIC get\n") );
	Ddim_Print( ("ch   = %d\n", ch  ) );

	memset( &monmxic, 0, sizeof(monmxic) );

	(VOID)Im_SDRAMC_Get_Monitor_Mxic( (E_IM_SDRAMC_CH)ch, &monmxic );

	for ( sub_id = 0; sub_id < E_IM_SDRAMC_SUBID_MAX; sub_id++ ) {
		Ddim_Print( ("monmxic.data[%d] = %lu\n", sub_id, monmxic.data[ sub_id ] ) );
	}

	Ddim_Print(("CT_Im_SDRAMC_1_09 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_10( UCHAR ch )
{
	Ddim_Print(("CT_Im_SDRAMC_1_10(ch=%d)\n", ch));

	Ddim_Print( (" manual mode start\n") );
	Ddim_Print(("\n"));
	(VOID)Im_SDRAMC_Start_ManualMode( (E_IM_SDRAMC_CH)ch );

	Ddim_Print( ("manual mode stop\n") );
	Ddim_Print(("\n"));
	(VOID)Im_SDRAMC_Stop_ManualMode( (E_IM_SDRAMC_CH)ch );

	Ddim_Print(("CT_Im_SDRAMC_1_10 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_11( UCHAR ch )
{
	Ddim_Print(("CT_Im_SDRAMC_1_11(ch=%d)\n", ch));

	Ddim_Print( (" ReLock_DLL start\n") );
	Ddim_Print(("\n"));
	(VOID)Im_SDRAMC_ReLock_DLL( (E_IM_SDRAMC_CH)ch );

	Ddim_Print(("CT_Im_SDRAMC_1_11 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_1_12( UCHAR ch )
{
	INT32 ret;

	Ddim_Print(("CT_Im_SDRAMC_1_12(ch=%d)\n", ch));

	Ddim_Print( ("Training1 start\n") );
	Ddim_Print(("\n"));

	ret = Im_SDRAMC_Start_Training( (E_IM_SDRAMC_CH)ch, E_IM_SDRAMC_TRAINING_MODE_WRITE );

	Ddim_Print( ("Training1 end(ret=%d)\n", ret) );
	Ddim_Print(("\n"));

	Ddim_Print( ("Training2 start\n") );
	Ddim_Print(("\n"));

	ret = Im_SDRAMC_Start_Training( (E_IM_SDRAMC_CH)ch, E_IM_SDRAMC_TRAINING_MODE_CA );

	Ddim_Print( ("Training2 end(ret=%d)\n", ret) );
	Ddim_Print(("\n"));

	Ddim_Print(("CT_Im_SDRAMC_1_12 end\n"));
	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_01( VOID )
{
	E_IM_SDRAMC_PORT			port;
	T_IM_SDRAMC_CMD_ARB_RATE	arb_rate;

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		arb_rate.enable[ port ] = E_IM_SDRAMC_EN_ENABLE;
	}

	arb_rate.enable[ 10 ] = (E_IM_SDRAMC_EN)2;

	Ddim_Print(("CT_Im_SDRAMC_2_01\n"));

	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_0,   NULL );
	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_MAX, NULL );
	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_0,   &arb_rate );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_02( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_02\n"));

	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_0,   NULL );
	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_MAX, NULL );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_03( VOID )
{
	E_IM_SDRAMC_PORT			port;
	T_IM_SDRAMC_CMD_ARB_PRI		arb_pri;

	for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
		arb_pri.enable[ port ] = E_IM_SDRAMC_EN_ENABLE;
	}

	arb_pri.enable[ 11 ] = (E_IM_SDRAMC_EN)2;

	Ddim_Print(("CT_Im_SDRAMC_2_03\n"));

	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_0,   NULL );
	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_MAX, NULL );
	(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_0,   &arb_pri );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_04( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_04\n"));

	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_0,   NULL );
	(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_MAX, NULL );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_05( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_05\n"));

	(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_PORT_MX10,  E_IM_SDRAMC_ACCESS_WRITE,  E_IM_SDRAMC_EN_ENABLE );
	(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_PORT_MAX,   E_IM_SDRAMC_ACCESS_WRITE,  E_IM_SDRAMC_EN_ENABLE );
	(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_PORT_MX10, (E_IM_SDRAMC_ACCESS)2,      E_IM_SDRAMC_EN_ENABLE );
	(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_PORT_MX10,  E_IM_SDRAMC_ACCESS_WRITE, (E_IM_SDRAMC_EN)2 );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_06( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_06\n"));

	(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_DATABUF_0,   E_IM_SDRAMC_EN_ENABLE );
	(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_DATABUF_MAX, E_IM_SDRAMC_EN_ENABLE );
	(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_DATABUF_0,   (E_IM_SDRAMC_EN)2 );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_07( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_07\n"));

	(VOID)Im_SDRAMC_Set_Mode_Register( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_RANK_0,  0xFF, 0xFF );
	(VOID)Im_SDRAMC_Set_Mode_Register( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_RANK)3, 0xFF, 0xFF );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_08( VOID )
{
	UCHAR data;

	Ddim_Print(("CT_Im_SDRAMC_2_08\n"));

	(VOID)Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_RANK_0,  0xFF, &data );
	(VOID)Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_RANK)3, 0xFF, &data );
	(VOID)Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_RANK_0,  0xFF, NULL );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_09( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_09\n"));

	(VOID)Im_SDRAMC_Strat_SelfRefresh( E_IM_SDRAMC_CH_MAX );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_10( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_10\n"));

	(VOID)Im_SDRAMC_Stop_SelfRefresh( E_IM_SDRAMC_CH_MAX );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_11( VOID )
{
	T_IM_SDRAMC_ACCESS_DETECTION	acs_det;

	Ddim_Print(("CT_Im_SDRAMC_2_11\n"));

	acs_det.s_addr		= 0x00001000;
	acs_det.e_addr		= 0x00002000;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	acs_det.w_port		= 1;
	acs_det.r_port		= 2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	acs_det.w_port_l	= 1;
	acs_det.r_port_l	= 2;
	acs_det.w_port_u	= 1;
	acs_det.r_port_u	= 2;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	acs_det.pCallBack	= ct_sdramc_callback;
	(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_AREA_A,  &acs_det );
	(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_AREA)2, &acs_det );
	(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_AREA_A,  NULL );
	acs_det.e_addr		= acs_det.s_addr;
	(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_AREA_A,  &acs_det );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_12( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_12\n"));

	(VOID)Im_SDRAMC_Stop_Access_Detection( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_AREA_A );
	(VOID)Im_SDRAMC_Stop_Access_Detection( E_IM_SDRAMC_CH_0, (E_IM_SDRAMC_AREA)2 );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_13( VOID )
{
	E_IM_SDRAMC_AREA				area;
	E_IM_SDRAMC_PORT				port;
	ULONG							addr;

	Ddim_Print(("CT_Im_SDRAMC_2_13\n"));

	(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_MAX, &area, &port, &addr );
	(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_0,   NULL,  &port, &addr );
	(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_0,   &area, NULL,  &addr );
	(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_0,   &area, &port, NULL );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_14( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_14\n"));

	(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_MONMODE)2,   TRUE, TRUE, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, 2,    TRUE, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE,    2, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE,    2 );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_15( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_15\n"));

	(VOID)Im_SDRAMC_Stop_Monitor( E_IM_SDRAMC_CH_MAX );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_16( VOID )
{
	T_IM_SDRAMC_MONITOR	mon;

	Ddim_Print(("CT_Im_SDRAMC_2_16\n"));

	(VOID)Im_SDRAMC_Get_Monitor( E_IM_SDRAMC_CH_MAX, &mon );
	(VOID)Im_SDRAMC_Get_Monitor( E_IM_SDRAMC_CH_0,   NULL );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_17( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_17\n"));

	(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_MONMODE)2,   TRUE, TRUE, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, 2,    TRUE, TRUE );
	(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, 2,    TRUE );
	(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE,    2 );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_18( VOID )
{
	Ddim_Print(("CT_Im_SDRAMC_2_18\n"));

	(VOID)Im_SDRAMC_Stop_Monitor_Mxic( E_IM_SDRAMC_CH_MAX );

	Ddim_Print(("\n"));
}

VOID CT_Im_SDRAMC_2_19( VOID )
{
	T_IM_SDRAMC_MONITOR_MXIC	monmxic;

	Ddim_Print(("CT_Im_SDRAMC_2_19\n"));

	(VOID)Im_SDRAMC_Get_Monitor_Mxic( E_IM_SDRAMC_CH_MAX, &monmxic );
	(VOID)Im_SDRAMC_Get_Monitor_Mxic( E_IM_SDRAMC_CH_0,   NULL );

	Ddim_Print(("\n"));
}

/**
 * @brief       Command processing of SDRAM control.
 * @param[in]   INT32 argc
 * @param[in]   CHAR** argv
 *
 *	The meaning of parameters
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| P1       | P2      | P4     | P5    | P6    | P7    | Meaning                                             |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| set      | arbrate | ch     | test  |       |       | Set command arbitration (rate control) for test     |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Cmd_Arbitration_Rate()                |
 *	|          +         +        +-------+-------+-------+-----------------------------------------------------+
 *	|          |         |        | reset |       |       | Reset command arbitration (rate control)            |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Cmd_Arbitration_Rate()                |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | arbpri  | ch     | test  |       |       | Set command arbitration (priority control) for test |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Cmd_Arbitration_Priority()            |
 *	|          +         +        +-------+-------+-------+-----------------------------------------------------+
 *	|          |         |        | reset |       |       | Reset command arbitration (priority control)        |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Cmd_Arbitration_Priority()            |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | mode    | ch     | rank  | ma    |       | Set mode register                                   |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Mode_Register()                       |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | clkbus  | ch     | port  | access| on    | BusIF clock enable                                  |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Clock_Enable_BusIf()                  |
 *	|          |         +        +       +       +-------+-----------------------------------------------------+
 *	|          |         |        |       |       | off   | BusIF clock disable                                 |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Clock_Enable_BusIf()                  |
 *	|          |---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | clkdat  | ch     | dbno  | on    |       | Data buffer clock enable                            |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Clock_Enable_DataBuf()                |
 *	|          |         +        +       +-------+-------+-----------------------------------------------------+
 *	|          |         |        |       | off   |       | Data buffer clock disable                           |
 *	|          |         |        |       |       |       | Im_SDRAMC_Set_Clock_Enable_DataBuf()                |
 *	|----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| get      | arbrate | ch     |       |       |       | Get command arbitration (rate control)              |
 *	|          |         |        |       |       |       | Im_SDRAMC_Get_Cmd_Arbitration_Rate()                |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | arbpri  | ch     |       |       |       | Get command arbitration (priority control)          |
 *	|          |         |        |       |       |       | Im_SDRAMC_Get_Cmd_Arbitration_Priority()            |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | mode    | ch     | rank  | ma    |       | Get mode register                                   |
 *	|          |         |        |       |       |       | Im_SDRAMC_Get_Mode_Register()                       |
 *	|----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| refresh  | start   | ch     |       |       |       | Self refresh mode start                             |
 *	|          |         |        |       |       |       | Im_SDRAMC_Strat_SelfRefresh()                       |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | stop    | ch     |       |       |       | Self refresh mode stop                              |
 *	|          |         |        |       |       |       | Im_SDRAMC_Stop_SelfRefresh()                        |
 *	|----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| detect   | start   | ch     | area  |       |       | Access detection start                              |
 *	|          |         |        |       |       |       | Im_SDRAMC_Start_Access_Detection()                  |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | stop    | ch     | area  |       |       | Access detection stop                               |
 *	|          |         |        |       |       |       | Im_SDRAMC_Stop_Access_Detection()                   |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | get     | ch     |       |       |       | Get access detection                                |
 *	|          |         |        |       |       |       | Im_SDRAMC_Get_Access_Detection()                    |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| mon      | start   | ch     | mode  | write | read  | Monitor start                                       |
 *	|          |         |        |       |       |       | Im_SDRAMC_Start_Monitor()                           |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | stop    | ch     |       |       |       | Monitor stop                                        |
 *	|          |         |        |       |       |       | Im_SDRAMC_Stop_Monitor()                            |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | get     | ch     |       |       |       | Get access monitor                                  |
 *	|          |         |        |       |       |       | Im_SDRAMC_Get_Monitor()                             |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| monmxic  | start   | ch     | mode  | write | read  | Monitor for MXIC start                              |
 *	|          |         |        |       |       |       | Im_SDRAMC_Start_Monitor_Mxic()                      |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | stop    | ch     |       |       |       | Monitor for MXIC stop                               |
 *	|          |         |        |       |       |       | Im_SDRAMC_Stop_Monitor_Mxic()                       |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | get     | ch     |       |       |       | Get access monitor for MXIC                         |
 *	|          |         |        |       |       |       | Im_SDRAMC_Get_Monitor_Mxic()                        |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| manual   | start   | ch     |       |       |       | manual mode start                                   |
 *	|          |         |        |       |       |       | Im_SDRAMC_Start_ManualMode()                        |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | stop    | ch     |       |       |       | manual mode stop                                    |
 *	|          |         |        |       |       |       | Im_SDRAMC_Stop_ManualMode()                         |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| relock   | ch      |        |       |       |       | Relock dll                                          |
 *	|          |         |        |       |       |       | Im_SDRAMC_ReLock_DLL()                              |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| training | ch      | mode   |       |       |       | Training startt                                     |
 *	|          |         |        |       |       |       | Im_SDRAMC_Start_Training()                          |
 *	+----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	| err      | arbrate | set    |       |       |       | Im_SDRAMC_Set_Cmd_Arbitration_Rate()                |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | get    |       |       |       | Im_SDRAMC_Get_Cmd_Arbitration_Rate()                |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | arbpri  | set    |       |       |       | Im_SDRAMC_Set_Cmd_Arbitration_Priority()            |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | get    |       |       |       | Im_SDRAMC_Get_Cmd_Arbitration_Priority()            |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | clkbus  | set    |       |       |       | Im_SDRAMC_Set_Clock_Enable_BusIf()                  |
 *	|          |---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | clkdat  | set    |       |       |       | Im_SDRAMC_Set_Clock_Enable_DataBuf()                |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | mode    | set    |       |       |       | Im_SDRAMC_Set_Mode_Register()                       |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | get    |       |       |       | Im_SDRAMC_Get_Mode_Register()                       |
 *	|          +---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | refresh | start  |       |       |       | Im_SDRAMC_Strat_SelfRefresh()                       |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | stop   |       |       |       | Im_SDRAMC_Stop_SelfRefresh()                        |
 *	|          |---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | detect  | start  |       |       |       | Im_SDRAMC_Start_Access_Detection()                  |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | stop   |       |       |       | Im_SDRAMC_Stop_Access_Detection()                   |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | get    |       |       |       | Im_SDRAMC_Get_Access_Detection()                    |
 *	|          |---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | mon     | start  |       |       |       | Im_SDRAMC_Start_Monitor()                           |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | stop   |       |       |       | Im_SDRAMC_Stop_Monitor()                            |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | get    |       |       |       | Im_SDRAMC_Get_Monitor()                             |
 *	|          |---------+--------+-------+-------+-------+-----------------------------------------------------+
 *	|          | monmxic | start  |       |       |       | Im_SDRAMC_Start_Monitor_Mxic()                      |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | stop   |       |       |       | Im_SDRAMC_Stop_Monitor_Mxic()                       |
 *	|          |         +--------+-------+-------+-------+-----------------------------------------------------+
 *	|          |         | get    |       |       |       | Im_SDRAMC_Get_Monitor_Mxic()                        |
 *	|----------+---------+--------+-------+-------+-------+-----------------------------------------------------+
 * 
 * @return      VOID
 */
VOID Ct_Im_SDRAMC_Main( INT32 argc, char** argv )
{
	UCHAR							ma;
	UCHAR							data;
	ULONG							addr;
	UCHAR							sub_id;
	E_IM_SDRAMC_CH					ch;
	E_IM_SDRAMC_PORT				port;
	E_IM_SDRAMC_RANK				rank;
	E_IM_SDRAMC_AREA				area;
	E_IM_SDRAMC_ACCESS				access;
	E_IM_SDRAMC_DATABUF				dbno;
	E_IM_SDRAMC_MONMODE				mode;
	T_IM_SDRAMC_CMD_ARB_RATE		arb_rate;
	T_IM_SDRAMC_CMD_ARB_PRI			arb_pri;
	T_IM_SDRAMC_ACCESS_DETECTION	acs_det;
	T_IM_SDRAMC_MONITOR				mon;
	T_IM_SDRAMC_MONITOR_MXIC		monmxic;
	E_IM_SDRAMC_TRAINING_MODE		t_mode;

	if( strcmp(argv[1], "set") == 0 ){
		if( strcmp(argv[2], "arbrate") == 0 ){
			if( strcmp(argv[4], "test") == 0 ){

				ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
				Ddim_Print( ("ch = %d\n", ch) );

				for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
					arb_rate.throughput[ port ]	= 0xFFF;
					arb_rate.latency[ port ]	= 0xFFF;
					arb_rate.enable[ port ]		= E_IM_SDRAMC_EN_ENABLE;
					Ddim_Print( ("arb_rate.throughput[%d] = %d\n", port, arb_rate.throughput[ port ] ) );
					Ddim_Print( ("arb_rate.latency[%d]    = %d\n", port, arb_rate.latency[ port ]    ) );
					Ddim_Print( ("arb_rate.enable[%d]     = %d\n", port, arb_rate.enable[ port ]     ) );
				}

				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( ch, &arb_rate );
			}
			else if( strcmp(argv[4], "reset") == 0 ){

				ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
				Ddim_Print( ("ch = %d\n", ch) );

				for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
					arb_rate.throughput[ port ]	= 0;
					arb_rate.latency[ port ]	= 0;
					arb_rate.enable[ port ]		= E_IM_SDRAMC_EN_DISABLE;
					Ddim_Print( ("arb_rate.throughput[%d] = %d\n", port, arb_rate.throughput[ port ] ) );
					Ddim_Print( ("arb_rate.latency[%d]    = %d\n", port, arb_rate.latency[ port ]    ) );
					Ddim_Print( ("arb_rate.enable[%d]     = %d\n", port, arb_rate.enable[ port ]     ) );
				}

				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( ch, &arb_rate );
			}
			else {
				Ddim_Print( ("please check 4th parameter!\n") );
				return;
			}
		}
		else if( strcmp(argv[2], "arbpri") == 0 ){
			if( strcmp(argv[4], "test") == 0 ){

				ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
				Ddim_Print( ("ch = %d\n", ch) );

				for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
					arb_pri.priority[ port ]	= 0x1F;
					arb_pri.enable[ port ]		= E_IM_SDRAMC_EN_ENABLE;
					Ddim_Print( ("arb_pri.priority[%d] = %d\n", port, arb_pri.priority[ port ] ) );
					Ddim_Print( ("arb_pri.enable[%d]   = %d\n", port, arb_pri.enable[ port ]   ) );
				}

				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( ch, &arb_pri );
			}
			else if( strcmp(argv[4], "reset") == 0 ){

				ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
				Ddim_Print( ("ch = %d\n", ch) );

				for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
					arb_pri.priority[ port ]	= 0;
					arb_pri.enable[ port ]		= E_IM_SDRAMC_EN_DISABLE;
					Ddim_Print( ("arb_pri.priority[%d] = %d\n", port, arb_pri.priority[ port ] ) );
					Ddim_Print( ("arb_pri.enable[%d]   = %d\n", port, arb_pri.enable[ port ]  ) );
				}

				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( ch, &arb_pri );
			}
			else {
				Ddim_Print( ("please check 4th parameter!\n") );
				return;
			}
		}
		else if( strcmp(argv[2], "mode") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			rank	= (E_IM_SDRAMC_RANK)atoi(argv[4]);
			ma		= atoi(argv[5]);

			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("rank = %d\n", rank) );
			Ddim_Print( ("ma   = %d\n", ma  ) );

			(VOID)Im_SDRAMC_Set_Mode_Register( ch, rank, ma, 0xFF );
		}
		else if( strcmp(argv[2], "clkbus") == 0 ) {

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			port	= (E_IM_SDRAMC_PORT)atoi(argv[4]);
			access	= (E_IM_SDRAMC_ACCESS)atoi(argv[5]);

			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("port = %d\n", port) );
			
			if( strcmp(argv[6], "on") == 0 ) {
				Ddim_Print( ("Bus IF on\n") );
				(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( ch, port, access, E_IM_SDRAMC_EN_ENABLE );
			}
			else {
				Ddim_Print( ("Bus IF off\n") );
				(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( ch, port, access, E_IM_SDRAMC_EN_DISABLE );
			}
		}
		else if( strcmp(argv[2], "clkdat") == 0 ) {

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			dbno	= (E_IM_SDRAMC_DATABUF)atoi(argv[4]);

			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("dbno = %d\n", dbno) );
			
			if( strcmp(argv[5], "on") == 0 ) {
				Ddim_Print( ("Data buf on\n") );
				(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( ch, dbno, E_IM_SDRAMC_EN_ENABLE );
			}
			else {
				Ddim_Print( ("Data buf off\n") );
				(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( ch, dbno, E_IM_SDRAMC_EN_DISABLE );
			}
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "get") == 0 ){
		if( strcmp(argv[2], "arbrate") == 0 ){

			ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch = %d\n", ch) );

			memset( &arb_rate, 0, sizeof(arb_rate) );

			(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( ch, &arb_rate );

			for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
				Ddim_Print( ("arb_rate.throughput[%d] = %d\n", port, arb_rate.throughput[ port ] ) );
				Ddim_Print( ("arb_rate.latency[%d]    = %d\n", port, arb_rate.latency[ port ]    ) );
			}
		}
		else if( strcmp(argv[2], "arbpri") == 0 ){

			ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch = %d\n", ch) );

			memset( &arb_pri, 0, sizeof(arb_pri) );

			(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( ch, &arb_pri );

			for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
				Ddim_Print( ("arb_pri.priority[%d] = %d\n", port, arb_pri.priority[ port ] ) );
			}
		}
		else if( strcmp(argv[2], "mode") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			rank	= (E_IM_SDRAMC_RANK)atoi(argv[4]);
			ma		= atoi(argv[5]);

			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("rank = %d\n", rank) );
			Ddim_Print( ("ma   = %d\n", ma  ) );

			(VOID)Im_SDRAMC_Get_Mode_Register( ch, rank, ma, &data );

			Ddim_Print( ("data = %d\n", data  ) );
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "refresh") == 0 ){
		if( strcmp(argv[2], "start") == 0 ){

			ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("refresh start ch = %d\n", ch) );

			(VOID)Im_SDRAMC_Strat_SelfRefresh( ch );
		}
		else if( strcmp(argv[2], "stop") == 0 ){

			ch = (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("refresh stop ch = %d\n", ch) );

			(VOID)Im_SDRAMC_Stop_SelfRefresh( ch );
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "detect") == 0 ){
		if( strcmp(argv[2], "start") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			area	= (E_IM_SDRAMC_AREA)atoi(argv[4]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("area = %d\n", area) );
			Ddim_Print( ("Access detection start\n") );

			acs_det.s_addr		= 0x00001000;
			acs_det.e_addr		= 0x00002000;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			acs_det.w_port		= 1;
			acs_det.r_port		= 2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			acs_det.w_port_l	= 1;
			acs_det.r_port_l	= 2;
			acs_det.w_port_u	= 1;
			acs_det.r_port_u	= 2;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			acs_det.pCallBack	= (T_IM_SDRAMC_CALLBACK)ct_sdramc_callback;

			(VOID)Im_SDRAMC_Start_Access_Detection( ch, area, &acs_det );
		}
		else if( strcmp(argv[2], "stop") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			area	= (E_IM_SDRAMC_AREA)atoi(argv[4]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("area = %d\n", area) );
			Ddim_Print( ("Access detection stop\n") );

			(VOID)Im_SDRAMC_Stop_Access_Detection( ch, area );
		}
		else if( strcmp(argv[2], "get") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );

			(VOID)Im_SDRAMC_Get_Access_Detection( ch, &area, &port, &addr );

			Ddim_Print( ("area = %d\n",      area) );
			Ddim_Print( ("port = %d\n",      port) );
			Ddim_Print( ("addr = 0x%08lx\n", addr) );
			Ddim_Print( ("Access detection get\n") );
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "mon") == 0 ){
		if( strcmp(argv[2], "start") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			mode	= (E_IM_SDRAMC_MONMODE)atoi(argv[4]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("mode = %d\n", mode) );
			Ddim_Print( ("Monitor start\n") );

			(VOID)Im_SDRAMC_Start_Monitor( ch, mode, TRUE, TRUE, TRUE );
		}
		else if( strcmp(argv[2], "stop") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("Monitor stop\n") );

			(VOID)Im_SDRAMC_Stop_Monitor( ch );
		}
		else if( strcmp(argv[2], "get") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("Monitor get\n") );

			(VOID)Im_SDRAMC_Get_Monitor( ch, &mon );

			for ( port = E_IM_SDRAMC_PORT_MX10; port < E_IM_SDRAMC_PORT_MAX; port++ ) {
				Ddim_Print( ("mon.data[%d] = %lu\n", port, mon.data[ port ] ) );
			}
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "monmxic") == 0 ){
		if( strcmp(argv[2], "start") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			mode	= (E_IM_SDRAMC_MONMODE)atoi(argv[4]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("mode = %d\n", mode) );
			Ddim_Print( ("Monitor for MXIC start\n") );

			(VOID)Im_SDRAMC_Start_Monitor_Mxic( ch, mode, TRUE, TRUE, TRUE );
		}
		else if( strcmp(argv[2], "stop") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("Monitor for MXIC stop\n") );

			(VOID)Im_SDRAMC_Stop_Monitor_Mxic( ch );
		}
		else if( strcmp(argv[2], "get") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("Monitor for MXIC get\n") );

			(VOID)Im_SDRAMC_Get_Monitor_Mxic( ch, &monmxic );

			for ( sub_id = 0; sub_id < E_IM_SDRAMC_SUBID_MAX; sub_id++ ) {
				Ddim_Print( ("monmxic.data[%d] = %lu\n", sub_id, monmxic.data[ sub_id ] ) );
			}
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "manual") == 0 ){
		if( strcmp(argv[2], "start") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("Manual mode start\n") );

			(VOID)Im_SDRAMC_Start_ManualMode( ch );
		}
		else if( strcmp(argv[2], "stop") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("Manual mode stop\n") );

			(VOID)Im_SDRAMC_Stop_ManualMode( ch );
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
			return;
		}
	}
	else if( strcmp(argv[1], "relock") == 0 ){

		ch		= (E_IM_SDRAMC_CH)atoi(argv[2]);
		Ddim_Print( ("ch   = %d\n", ch  ) );
		Ddim_Print( ("Relock dll\n") );

		(VOID)Im_SDRAMC_ReLock_DLL( ch );
	}
	else if( strcmp(argv[1], "training") == 0 ){

			ch		= (E_IM_SDRAMC_CH)atoi(argv[2]);
			t_mode	= (E_IM_SDRAMC_TRAINING_MODE)atoi(argv[3]);
			Ddim_Print( ("ch   = %d\n", ch  ) );
			Ddim_Print( ("mode = %d\n", t_mode) );
			Ddim_Print( ("Training start\n") );

			(VOID)Im_SDRAMC_Start_Training( ch, t_mode );
	}
	else if ( strcmp(argv[1], "err") == 0 ) {
		if ( strcmp(argv[2], "arbrate") == 0 ) {
			if ( strcmp(argv[3], "set") == 0 ) {
				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_0,   NULL );
				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_MAX, NULL );
			}
			else if ( strcmp(argv[3], "get") == 0 ) {
				(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_0,   NULL );
				(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Rate( E_IM_SDRAMC_CH_MAX, NULL );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "arbpri") == 0 ) {
			if ( strcmp(argv[3], "set") == 0 ) {
				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_0,   NULL );
				(VOID)Im_SDRAMC_Set_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_MAX, NULL );
			}
			else if ( strcmp(argv[3], "get") == 0 ) {
				(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_0,   NULL );
				(VOID)Im_SDRAMC_Get_Cmd_Arbitration_Priority( E_IM_SDRAMC_CH_MAX, NULL );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "clkbus") == 0 ) {
			if ( strcmp(argv[3], "set") == 0 ) {
				(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_PORT_MX10,  E_IM_SDRAMC_ACCESS_WRITE,  E_IM_SDRAMC_EN_ENABLE );
				(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_PORT_MAX,   E_IM_SDRAMC_ACCESS_WRITE,  E_IM_SDRAMC_EN_ENABLE );
				(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_PORT_MX10, (E_IM_SDRAMC_ACCESS)2,      E_IM_SDRAMC_EN_ENABLE );
				(VOID)Im_SDRAMC_Set_Clock_Enable_BusIf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_PORT_MX10,  E_IM_SDRAMC_ACCESS_WRITE, (E_IM_SDRAMC_EN)2 );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "clkdat") == 0 ) {
			if ( strcmp(argv[3], "set") == 0 ) {
				(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_DATABUF_0,   E_IM_SDRAMC_EN_ENABLE );
				(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_DATABUF_MAX, E_IM_SDRAMC_EN_ENABLE );
				(VOID)Im_SDRAMC_Set_Clock_Enable_DataBuf( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_DATABUF_0,   (E_IM_SDRAMC_EN)2 );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "mode") == 0 ) {
			if ( strcmp(argv[3], "set") == 0 ) {
				(VOID)Im_SDRAMC_Set_Mode_Register( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_RANK_0,  0xFF, 0xFF );
				(VOID)Im_SDRAMC_Set_Mode_Register( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_RANK)3, 0xFF, 0xFF );
			}
			else if ( strcmp(argv[3], "get") == 0 ) {
				(VOID)Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_RANK_0,  0xFF, &data );
				(VOID)Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_RANK)3, 0xFF, &data );
				(VOID)Im_SDRAMC_Get_Mode_Register( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_RANK_0,  0xFF, NULL );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "refresh") == 0 ) {
			if ( strcmp(argv[3], "start") == 0 ) {
				(VOID)Im_SDRAMC_Strat_SelfRefresh( E_IM_SDRAMC_CH_MAX );
			}
			else if ( strcmp(argv[3], "stop") == 0 ) {
				(VOID)Im_SDRAMC_Stop_SelfRefresh( E_IM_SDRAMC_CH_MAX );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "detect") == 0 ) {
			if ( strcmp(argv[3], "start") == 0 ) {
				acs_det.s_addr		= 0x00001000;
				acs_det.e_addr		= 0x00002000;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
				acs_det.w_port		= 1;
				acs_det.r_port		= 2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
				acs_det.w_port_l	= 1;
				acs_det.r_port_l	= 2;
				acs_det.w_port_u	= 1;
				acs_det.r_port_u	= 2;
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
				acs_det.pCallBack	= ct_sdramc_callback;
				(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_AREA_A,  &acs_det );
				(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_AREA)2, &acs_det );
				(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_AREA_A,  NULL );
				acs_det.e_addr		= acs_det.s_addr;
				(VOID)Im_SDRAMC_Start_Access_Detection( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_AREA_A,  &acs_det );
			}
			else if ( strcmp(argv[3], "stop") == 0 ) {
				(VOID)Im_SDRAMC_Stop_Access_Detection( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_AREA_A );
				(VOID)Im_SDRAMC_Stop_Access_Detection( E_IM_SDRAMC_CH_0, (E_IM_SDRAMC_AREA)2 );
			}
			else if ( strcmp(argv[3], "get") == 0 ) {
				(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_MAX, &area, &port, &addr );
				(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_0,   NULL,  &port, &addr );
				(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_0,   &area, NULL,  &addr );
				(VOID)Im_SDRAMC_Get_Access_Detection( E_IM_SDRAMC_CH_0,   &area, &port, NULL );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "mon") == 0 ) {
			if ( strcmp(argv[3], "start") == 0 ) {
				(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE, TRUE );
				(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_MONMODE)2,   TRUE, TRUE, TRUE );
				(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, 2,    TRUE, TRUE );
				(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, 2,    TRUE );
				(VOID)Im_SDRAMC_Start_Monitor( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE,    2 );
			}
			else if ( strcmp(argv[3], "stop") == 0 ) {
				(VOID)Im_SDRAMC_Stop_Monitor( E_IM_SDRAMC_CH_MAX );
			}
			else if ( strcmp(argv[3], "get") == 0 ) {
				(VOID)Im_SDRAMC_Get_Monitor( E_IM_SDRAMC_CH_MAX, &mon );
				(VOID)Im_SDRAMC_Get_Monitor( E_IM_SDRAMC_CH_0,   NULL );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else if ( strcmp(argv[2], "monmxic") == 0 ) {
			if ( strcmp(argv[3], "start") == 0 ) {
				(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_MAX, E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE, TRUE );
				(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   (E_IM_SDRAMC_MONMODE)2,   TRUE, TRUE, TRUE );
				(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, 2,    TRUE, TRUE );
				(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, 2,    TRUE );
				(VOID)Im_SDRAMC_Start_Monitor_Mxic( E_IM_SDRAMC_CH_0,   E_IM_SDRAMC_MONMODE_SIZE, TRUE, TRUE,    2 );
			}
			else if ( strcmp(argv[3], "stop") == 0 ) {
				(VOID)Im_SDRAMC_Stop_Monitor_Mxic( E_IM_SDRAMC_CH_MAX );
			}
			else if ( strcmp(argv[3], "get") == 0 ) {
				(VOID)Im_SDRAMC_Get_Monitor_Mxic( E_IM_SDRAMC_CH_MAX, &monmxic );
				(VOID)Im_SDRAMC_Get_Monitor_Mxic( E_IM_SDRAMC_CH_0,   NULL );
			}
			else {
				Ddim_Print( ("please check 3rd parameter!\n") );
			}
		}
		else {
			Ddim_Print( ("please check 2nd parameter!\n") );
		}
	}
	else{
		Ddim_Print( ("please check 1st parameter!\n") );
	}

	return ;
}
