/**
 * @file		ct_dd_tmr32.c
 * @brief		This is ct code for dd_tmr32
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_tmr32.h"
#include "peripheral.h"
#include "dd_top.h"
#include "chiptop.h"
#include <stdlib.h>
#include <string.h>
#include "dd_gic.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_CT_DD_TMR32_USTIME		// Enable;get time(us). Disenable:get time(ms)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static DDIM_USER_SYSTIM	gDd_timer32_tim1;
static DDIM_USER_SYSTIM	gDd_timer32_tim2;

#if 0
static ULONG gDD_Timer32_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
static VOID  ct_timer32_cb( VOID );

#ifdef	CO_CT_DD_TMR32_USTIME
//Get_Time_us
ULONG CT_DD_TMR32_Get_Time_us(VOID)
{
	ULONG h_count;
	ULONG s_count;
	ULLONG h_count_long;
	static UINT32 get_time_ch =2;
	Dd_TMR32_Get_Counter(get_time_ch, &h_count, &s_count);
	h_count = 0xFFFFFFFF - h_count;
	h_count_long = (ULLONG)h_count;
	h_count_long = h_count_long * 32;
	
	return (ULONG)(h_count_long / 54);
}
#endif	// CO_CT_DD_TMR32_USTIME

/**
 * @brief  Command main function for timer32 test.
 * @param  int argc		:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 */
VOID Ct_Dd_TMR32_Main( int argc, char** argv )
{
	INT32			ret;
	UINT32			ch;
	ULONG			usec;
	ULONG			num;
	T_DD_TMR32_CTRL	tmr32_ctrl = {{0}};
#ifndef	CO_CT_DD_TMR32_USTIME
	DDIM_USER_ER	er;
#endif	// CO_CT_DD_TMR32_USTIME

	/*-------*/
	/* Open  */
	/*-------*/
	if( strcmp(argv[1], "open") == 0 ){
		/* ch number */
		ch = atoi(argv[2]);
#if 0	// for Debug code
		// DSR_C is not used.
		//IO_CHIPTOP.PDR_C.bit.P41  = 1;	//for Oscilloscope	//This is not needed for PT.
		//IO_CHIPTOP.PDR.bit.P41  = 0; //?????
		*(volatile unsigned long*)0x1DFFB004 = 0x00400000;
#endif	// for Debug code
		// Set for gic(For the channel that it's not set).
		if (ch == 6){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH6_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 7){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH7_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 8){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH8_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 9){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH9_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 10){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH10_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 11){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH11_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 12){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH12_INT,1,D_DD_GIC_PRI30,1);
		}
		else if (ch == 13){
			Dd_GIC_Ctrl(E_DD_GIC_INTID_TIMER_CH13_INT,1,D_DD_GIC_PRI30,1);
		}
		else{
		}
		/* Open */
		ret = Dd_TMR32_Open( ch, (INT32)atoi(argv[3]) );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Open[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Open[%d] ERR=%d\n", ch, ret) );
		}
#if 0	// for Debug code
		gDd_timer32_tim1 = CT_DD_TMR32_Get_Time_us();
		DDIM_User_Dly_Tsk(10);
		gDd_timer32_tim2 = CT_DD_TMR32_Get_Time_us();
		Ddim_Print(("TMR32_Dly test. %d\n", (ULONG)(gDd_timer32_tim2 - gDd_timer32_tim1) ));
		gDd_timer32_tim1=gDd_timer32_tim2;
		DDIM_User_Dly_Tsk(10);
		gDd_timer32_tim2 = CT_DD_TMR32_Get_Time_us();
		Ddim_Print(("TMR32_Dly test. %d\n", (ULONG)(gDd_timer32_tim2 - gDd_timer32_tim1) ));
#endif	// for Debug code
	}
	/*-------*/
	/* close */
	/*-------*/
	else if( strcmp(argv[1], "close") == 0 ){
		/* ch number */
		ch = atoi(argv[2]);

		/* Close */
		ret = Dd_TMR32_Close( ch );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Close[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Close[%d] ERR=%d\n", ch, ret) );
		}
	}
	/*------*/
	/* Ctrl */
	/*------*/
	else if( strcmp(argv[1], "ctrl") == 0 ){
		// CH
		ch = atoi(argv[2]);
		
		tmr32_ctrl.csr.word = 0x00000008;
		
		// DIV
		if( strcmp(argv[3], "32") == 0 ){
			tmr32_ctrl.csr.bit.csl = 2;
		}
		else if( strcmp(argv[3], "8") == 0 ){
			tmr32_ctrl.csr.bit.csl = 1;
		}
		else if( strcmp(argv[3], "2") == 0 ){
			tmr32_ctrl.csr.bit.csl = 0;
		}
		else if( strcmp(argv[3], "ext") == 0 ){
			tmr32_ctrl.csr.bit.csl = 3;
		}
		else {
			tmr32_ctrl.csr.bit.csl = 0;
		}
		// MODE
		if( strcmp(argv[4], "one") == 0 ){
			tmr32_ctrl.csr.bit.reld = 0;
		}
		else if( strcmp(argv[4], "reload") == 0 ){
			tmr32_ctrl.csr.bit.reld = 1;
		}
		else {
			tmr32_ctrl.csr.bit.reld = 0;
		}
		
		tmr32_ctrl.rlr1 = (ULONG)atoi(argv[5]);
		tmr32_ctrl.rlr2 = (ULONG)atoi(argv[6]);
		tmr32_ctrl.soft_counter = (UCHAR)atoi(argv[7]);
		tmr32_ctrl.pcallback = ct_timer32_cb;
		
		// Edge hunt MODE
		if( strcmp(argv[8], "u") == 0 ){
			// Upper
			tmr32_ctrl.csr.bit.mod = 1;
		}
		else if( strcmp(argv[8], "d") == 0 ){
			// Down
			tmr32_ctrl.csr.bit.mod = 2;
		}
		else if( strcmp(argv[8], "b") == 0 ){
			// Both
			tmr32_ctrl.csr.bit.mod = 3;
		}
		else {
			tmr32_ctrl.csr.bit.mod = 0;
		}


		Ddim_Print( ("Dd_TMR32_Ctrl Arg( T_DD_TMR32_CTRL )///\n") );
		Ddim_Print( ("csr.trg_fixed0 = %d\n", tmr32_ctrl.csr.bit.trg_fixed0) );		/* pgr0872 */
		Ddim_Print( ("csr.cnte       = %d\n", tmr32_ctrl.csr.bit.cnte) );			/* pgr0872 */
		Ddim_Print( ("csr.uf_fixed1  = %d\n", tmr32_ctrl.csr.bit.uf_fixed1) );		/* pgr0872 */
		Ddim_Print( ("csr.inte       = %d\n", tmr32_ctrl.csr.bit.inte) );			/* pgr0872 */
		Ddim_Print( ("csr.reld       = %d\n", tmr32_ctrl.csr.bit.reld) );
		Ddim_Print( ("csr.outl       = %d\n", tmr32_ctrl.csr.bit.outl) );			/* pgr0872 */
		Ddim_Print( ("csr.mod        = %d\n", tmr32_ctrl.csr.bit.mod) );			/* pgr0872 */
		Ddim_Print( ("csr.csl        = %d\n", tmr32_ctrl.csr.bit.csl) );
		Ddim_Print( ("csr.rels       = %d\n", tmr32_ctrl.csr.bit.rels) );			/* pgr0872 */
		Ddim_Print( ("rlr1           = %lu\n", tmr32_ctrl.rlr1) );
		Ddim_Print( ("rlr2           = %lu\n", tmr32_ctrl.rlr2) );
		Ddim_Print( ("soft_counter   = %lu\n", tmr32_ctrl.soft_counter) );
		Ddim_Print( ("pcallback      = %p\n", tmr32_ctrl.pcallback) );
		Ddim_Print( ("///////////////////////////////////////\n") );

		/* Ctrl */
		ret = Dd_TMR32_Ctrl( ch, &tmr32_ctrl );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Ctrl[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("timer Dd_TMR32_Ctrl[%d] error=%d\n", ch, ret) );
		}
		Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch, IO_PERI.TMR[ch].TMCSR.word));
	}
	/*-----------*/
	/* Calculate */
	/*-----------*/
	else if( strcmp(argv[1], "cal") == 0 ){
		usec = (ULONG)atoi(argv[2]);

		ret = Dd_TMR32_Calculate( usec, &tmr32_ctrl );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Calculate OK\n") );
			Ddim_Print( ("rels         = %d\n", tmr32_ctrl.csr.bit.rels) );
			Ddim_Print( ("csl          = %d\n", tmr32_ctrl.csr.bit.csl) );
			Ddim_Print( ("mod          = %d\n", tmr32_ctrl.csr.bit.mod) );
			Ddim_Print( ("outl         = %d\n", tmr32_ctrl.csr.bit.outl) );
			Ddim_Print( ("reld         = %d\n", tmr32_ctrl.csr.bit.reld) );
			Ddim_Print( ("inte         = %d\n", tmr32_ctrl.csr.bit.inte) );
			Ddim_Print( ("uf_fixed1    = %d\n", tmr32_ctrl.csr.bit.uf_fixed1) );
			Ddim_Print( ("cnte         = %d\n", tmr32_ctrl.csr.bit.cnte) );
			Ddim_Print( ("trg_fixed0   = %d\n", tmr32_ctrl.csr.bit.trg_fixed0) );
			Ddim_Print( ("rlr1         = %lu\n", tmr32_ctrl.rlr1) );
			Ddim_Print( ("rlr2         = %lu\n", tmr32_ctrl.rlr2) );
			Ddim_Print( ("soft_counter = %lu\n", tmr32_ctrl.soft_counter) );
			Ddim_Print( ("pcallback    = %p\n", tmr32_ctrl.pcallback) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Calculate ERR=%d\n", ret) );
		}
	}
	/*----------------*/
	/* Start / Resume */
	/*----------------*/
	else if( strcmp(argv[1], "start") == 0 ){
		/* ch number */
		ch = atoi(argv[2]);
		
		/* Start */
		ret = Dd_TMR32_Start( ch );
			/* Debug: Time(msec) */
#if 0	// for Debug code
			// DSR_S is not used.
			//IO_CHIPTOP.PDR_S.bit.P41 = 1;	//for Oscilloscope	//This is not needed for PT.
			//IO_CHIPTOP.PDR.bit.P41 = 1;//?????
			*(volatile unsigned long*)0x1DFFB004 = 0x00400040;
#endif	// for Debug code

#ifdef	CO_CT_DD_TMR32_USTIME
			gDd_timer32_tim1 = CT_DD_TMR32_Get_Time_us();
#else	// CO_CT_DD_TMR32_USTIME
			er = DDIM_User_Get_Tim( &gDd_timer32_tim1 );
			if( er != D_DDIM_USER_E_OK ){
				Ddim_Print(("get_tim() er=%d\n",er));
			}
#endif	// CO_CT_DD_TMR32_USTIME

#if 0	// for Debug code
	gDd_timer32_tim2 = 0;
	gDd_timer32_tim3 = 0;
	gDd_timer32_tim4 = 0;
	Dd_ARM_Critical_Section_Start( gDD_Timer32_Spin_Lock );
	IO_PERI.TMR[ch].TMCSR.word |= 0x0000000B;
	gDd_timer32_tim1 = CT_DD_TMR32_Get_Time_us();
	// UF clear 
	IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFFA;
	
	// UF 
	while(1){
		if(IO_PERI.TMR[ch].TMCSR.bit.__UF == 1){
			if ( gDd_timer32_tim2 == 0){
				gDd_timer32_tim2 = CT_DD_TMR32_Get_Time_us();
				IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFFA;
			}
			else if ( gDd_timer32_tim3 == 0){
				gDd_timer32_tim3 = CT_DD_TMR32_Get_Time_us();
				IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFFA;
			}
			else if ( gDd_timer32_tim4 == 0){
				gDd_timer32_tim4 = CT_DD_TMR32_Get_Time_us();
				IO_PERI.TMR[ch].TMCSR.word &= 0xFFFFFFFA;
				break;
			}
		}
	}
	Ddim_Print(("\n___%d(us)\n", (ULONG)(gDd_timer32_tim2 - gDd_timer32_tim1) ));
	Ddim_Print(("\n___%d(us)\n", (ULONG)(gDd_timer32_tim3 - gDd_timer32_tim2) ));
	Ddim_Print(("\n___%d(us)\n", (ULONG)(gDd_timer32_tim4 - gDd_timer32_tim3) ));
	gDd_timer32_tim1 = gDd_timer32_tim4;
	Dd_ARM_Critical_Section_End( gDD_Timer32_Spin_Lock );
#endif	// for Debug code
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Start[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Start[%d] ERR=%d\n", ch, ret) );
		}
		Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch, IO_PERI.TMR[ch].TMCSR.word));
	}
	/*------------------------*/
	/* External trigger Start */
	/*------------------------*/
	else if( strcmp(argv[1], "ext_start") == 0 ){
		/* ch number */
		ch = atoi(argv[2]);
		
		/* Start */
		ret = Dd_TMR32_External_Start( ch );
			/* Debug: Time(msec) */
#ifdef	CO_CT_DD_TMR32_USTIME
			gDd_timer32_tim1 = CT_DD_TMR32_Get_Time_us();
#else	// CO_CT_DD_TMR32_USTIME
			er = DDIM_User_Get_Tim( &gDd_timer32_tim1 );
			if( er != D_DDIM_USER_E_OK ){
				Ddim_Print(("get_tim() er=%d\n",er));
			}
#endif	// CO_CT_DD_TMR32_USTIME
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_External_Start[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_External_Start[%d] ERR=%d\n", ch, ret) );
		}
//		Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch, IO_PERI.TMR[ch].TMCSR.word));
	}
	/*------*/
	/* stop */
	/*------*/
	else if( strcmp(argv[1], "stop") == 0 ){  		// Timer32 stop
		/* ch number */
		ch = atoi(argv[2]);

		/* Stop */
		ret = Dd_TMR32_Stop( ch );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Stop[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Stop[%d] ERR=%d\n", ch, ret) );
		}
		Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch, IO_PERI.TMR[ch].TMCSR.word));
	}
	/*-------*/
	/* Pause */
	/*-------*/
	else if( strcmp(argv[1], "pause") == 0 ){
		/* ch number */
		ch = atoi(argv[2]);

		/* Pause */
		ret = Dd_TMR32_Pause( ch );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Pause[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Pause[%d] ERR=%d\n", ch, ret) );
		}
		Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch, IO_PERI.TMR[ch].TMCSR.word));
	}
	/*-----------------------------------*/
	/* Get TMR Register and Soft Counter */
	/*-----------------------------------*/
	else if( strcmp(argv[1], "get_counter") == 0 ){
		ULONG	hw_count ;
		ULONG	sw_count ;

		/* ch number */
		ch = atoi(argv[2]);

		/* Get H/W & S/W Counter */
		ret = Dd_TMR32_Get_Counter( ch, &hw_count, &sw_count );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Get_Counter[%d] OK\n", ch) );
			Ddim_Print( ("H/W counter = 0x%lx, S/W Counter=%lu\n", hw_count, sw_count) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Get_Counter[%d] ERR=%d\n", ch, ret) );
		}
	}
	/*------------------------*/
	/* Get Timer's Ctrl table */
	/*------------------------*/
	else if( strcmp(argv[1], "get_table") == 0 ){
		/* ch number */
		ch = atoi(argv[2]);

		/* Get H/W & S/W Counter */
		ret = Dd_TMR32_Get_Ctrl( ch, &tmr32_ctrl ) ;
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Get_Ctrl[%d] OK\n", ch) );
			Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch, IO_PERI.TMR[ch].TMCSR.word));
			Ddim_Print( ("csr         =0x%lx\n", tmr32_ctrl.csr.word) );
			Ddim_Print( ("rlr1        =%lu\n", tmr32_ctrl.rlr1) );
			Ddim_Print( ("rlr2        =%lu\n", tmr32_ctrl.rlr2) );
			Ddim_Print( ("soft_counter=%lu\n", tmr32_ctrl.soft_counter) );
			Ddim_Print( ("pcallback   =%p\n", tmr32_ctrl.pcallback) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Get_Ctrl ERR=%d\n", ret) );
		}
	}
	/*------------------*/
	/* Set Soft Counter */
	/*------------------*/
	else if( strcmp(argv[1], "set_softcounter") == 0 ){
		/* ch number */
		ch  = atoi(argv[2]);
		num = atoi(argv[3]);

		/* Set H/W & S/W Counter */
		ret = Dd_TMR32_Set_Softcounter( ch, num ) ;
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Set_Softcounter[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Set_Softcounter[%d] ERR=%d\n", ch, ret) );
		}
	}
	/*------------------*/
	/* Get Soft Counter */
	/*------------------*/
	else if( strcmp(argv[1], "get_softcounter") == 0 ){
		/* ch number */
		ch  = atoi(argv[2]);

		/* Get H/W & S/W Counter */
		num = Dd_TMR32_Get_Softcounter( ch ) ;
		Ddim_Print( ("Soft counter[%d]=%lu\n", ch, num) );
	}
	/*-----------------*/
	/* Set Reload Flag */
	/*-----------------*/
	else if( strcmp(argv[1], "set_reload") == 0 ){
		/* ch number */
		ch  = atoi(argv[2]);
		num = atoi(argv[3]);

		/* Get H/W & S/W Counter */
		ret = Dd_TMR32_Set_Reload_Flg( ch, num ) ;
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR32_Set_Reload_Flg[%d] OK\n", ch) );
		}
		else {
			Ddim_Print( ("Dd_TMR32_Set_Reload_Flg[%d] ERR=%d\n", ch, ret) );
		}
	}
	/*-----------------*/
	/* Get Reload Flag */
	/*-----------------*/
	else if( strcmp(argv[1], "get_reload") == 0 ){
		/* ch number */
		ch  = atoi(argv[2]);

		/* Get H/W & S/W Counter */
		num = Dd_TMR32_Get_Reload_Flg( ch ) ;
		Ddim_Print( ("Reload Flag[%d]=%lu\n", ch, num) );
	}
	/*---------------------*/
	/* Utility : Set_Timer */
	/*---------------------*/
	else if( strcmp(argv[1], "u_set") == 0 ){
		ch   = atoi(argv[2]);
		usec = (ULONG)atoi(argv[3]);
		
		Ddim_Print(("Peri CLK=%lu\n", Dd_Top_Get_RCLK()));
		
		ret = Dd_TMR32_SetTimer(ch, usec, ct_timer32_cb);
		if (ret == 0) {
			Ddim_Print(("Dd_TMR32_SetTimer[%d] OK\n", ch));
		}
		else {
			Ddim_Print(("Dd_TMR32_SetTimer[%d] ERR. ret=0x%x\n", ch, ret));
		}
	}
	/*------------------------*/
	/* Utility : Start Reload */
	/*------------------------*/
	else if( strcmp(argv[1], "u_start_reload") == 0 ){
		ch   = atoi(argv[2]);
		
		/* Debug: Time(msec) */
#ifdef	CO_CT_DD_TMR32_USTIME
		gDd_timer32_tim1 = CT_DD_TMR32_Get_Time_us();
#else	// CO_CT_DD_TMR32_USTIME
		er = DDIM_User_Get_Tim( &gDd_timer32_tim1 );
		if( er != D_DDIM_USER_E_OK ){
			Ddim_Print(("get_tim() er=%d\n",er));
		}
#endif	// CO_CT_DD_TMR32_USTIME
		ret = Dd_TMR32_StartReload(ch);
		if (ret == 0) {
			Ddim_Print(("Dd_TMR32_StartReload[%d] OK\n", ch));
		}
		else {
			Ddim_Print(("Dd_TMR32_StartReload[%d] ERR. ret=0x%x\n", ch, ret));
		}
	}
	/*--------------------------*/
	/* Utility : Start OneShot  */
	/*--------------------------*/
	else if( strcmp(argv[1], "u_start_one") == 0 ){
		ch   = atoi(argv[2]);
		
		/* Debug: Time(msec) */
#ifdef	CO_CT_DD_TMR32_USTIME
		gDd_timer32_tim1 = CT_DD_TMR32_Get_Time_us();
#else	// CO_CT_DD_TMR32_USTIME
		er = DDIM_User_Get_Tim( &gDd_timer32_tim1 );
		if( er != D_DDIM_USER_E_OK ){
			Ddim_Print(("get_tim() er=%d\n",er));
		}
#endif	// CO_CT_DD_TMR32_USTIME
		ret = Dd_TMR32_StartOneshot(ch);
		if (ret == 0) {
			Ddim_Print(("Dd_TMR32_StartOneshot[%d] OK\n", ch));
		}
		else {
			Ddim_Print(("Dd_TMR32_StartOneshot[%d] ERR. ret=0x%x\n", ch, ret));
		}
	}
	/*---------------*/
	/* Set Out Level */
	/*---------------*/
	else if( strcmp(argv[1], "set_outlevel") == 0 ){

		ch   = atoi(argv[2]);

		ret = Dd_TMR32_Set_Outlevel(atoi(argv[3]));
		if (ret == 0) {
			Ddim_Print(("Dd_TMR32_Set_Outlevel[%d] OK\n", ch));
			Ddim_Print(("OUTL = [0x%lx] \n", IO_PERI.TMR[3].TMCSR.word));
		}
		else {
			Ddim_Print(("Dd_TMR32_Set_Outlevel[%d] ERR. ret=0x%x\n", ch, ret));
		}
	}
	/*------------*/
	/* Test 1-all */
	/*------------*/
	else if( strcmp(argv[1], "test_1_all") == 0 ){
		INT32	ch_roop;
		ULONG	hw_count ;
		ULONG	sw_count ;

		// Open/Close check
		for( ch_roop=0; ch_roop<D_DD_TMR32_CH_NUM_MAX; ch_roop++ ){
			Ddim_Print(("test_1_all : %d /////////////////////////\n", ch_roop));
			/* Open */
			ret = Dd_TMR32_Open( ch_roop, 0 );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Open[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Open[%d] ERR=%d\n", ch_roop, ret) );
			}

			/* Calculate */
			Dd_TMR32_Calculate( 3000, &tmr32_ctrl );
			/* Ctrl */
			ret = Dd_TMR32_Ctrl( ch_roop, &tmr32_ctrl );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Ctrl[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("timer Dd_TMR32_Ctrl[%d] error=%d\n", ch_roop, ret) );
			}

			/* Start */
			ret = Dd_TMR32_Start( ch_roop );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Start[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Start[%d] ERR=%d\n", ch_roop, ret) );
			}
			Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch_roop, IO_PERI.TMR[ch_roop].TMCSR.word));

			/* Pause */
			ret = Dd_TMR32_Pause( ch_roop );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Pause[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Pause[%d] ERR=%d\n", ch_roop, ret) );
			}
			Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch_roop, IO_PERI.TMR[ch_roop].TMCSR.word));

			/* ReStart */
			ret = Dd_TMR32_Start( ch_roop );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Start[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Start[%d] ERR=%d\n", ch_roop, ret) );
			}
			Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch_roop, IO_PERI.TMR[ch_roop].TMCSR.word));

#ifdef CO_DEBUG_ON_PC
			IO_PERI.TMR[ch_roop].TMR	= 0x11111111 * (ch_roop+1);
#endif	// CO_DEBUG_ON_PC

			/* Get H/W & S/W Counter */
			ret = Dd_TMR32_Get_Counter( ch_roop, &hw_count, &sw_count );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Get_Counter[%d] OK\n", ch_roop) );
				Ddim_Print( ("H/W counter = 0x%lx, S/W Counter=%lu\n", hw_count, sw_count) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Get_Counter[%d] ERR=%d\n", ch_roop, ret) );
			}

			/* Get H/W & S/W Counter */
			ret = Dd_TMR32_Get_Ctrl( ch_roop, &tmr32_ctrl ) ;
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Get_Ctrl[%d] OK\n", ch_roop) );
				Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch_roop, IO_PERI.TMR[ch_roop].TMCSR.word));
				Ddim_Print( ("csr         =0x%lx\n", tmr32_ctrl.csr.word) );
				Ddim_Print( ("rlr1        =%lu\n", tmr32_ctrl.rlr1) );
				Ddim_Print( ("rlr2        =%lu\n", tmr32_ctrl.rlr2) );
				Ddim_Print( ("soft_counter=%lu\n", tmr32_ctrl.soft_counter) );
				Ddim_Print( ("pcallback   =%p\n", tmr32_ctrl.pcallback) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Get_Ctrl ERR=%d\n", ret) );
			}

			/* Stop */
			ret = Dd_TMR32_Stop( ch_roop );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Stop[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Stop[%d] ERR=%d\n", ch_roop, ret) );
			}
			Ddim_Print(("Timer32 IO_PERI.TMR[%d].TMCSR.word =[%lx].\n", ch_roop, IO_PERI.TMR[ch_roop].TMCSR.word));

			/* Close */
			ret = Dd_TMR32_Close( ch_roop );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Close[%d] OK\n", ch_roop) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Close[%d] ERR=%d\n", ch_roop, ret) );
			}

			/* Check SoftCounter */
			ret = Dd_TMR32_Set_Softcounter( ch_roop, (0x11111111*(ch_roop+1)) );
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Set_Softcounter[%d] OK\n", ch_roop) );
				Ddim_Print( ("Dd_TMR32_Get_Softcounter[%d] %lx\n", ch_roop, Dd_TMR32_Get_Softcounter(ch_roop)) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Set_Softcounter[%d] ERR=%d\n", ch_roop, ret) );
			}

			/* Check Reload_Flg */
			ret = Dd_TMR32_Set_Reload_Flg( ch_roop, (0x11*(ch_roop+1)) ) ;
			if( ret == D_DDIM_OK ){
				Ddim_Print( ("Dd_TMR32_Set_Reload_Flg[%d] OK\n", ch_roop) );
				Ddim_Print( ("Dd_TMR32_Get_Reload_Flg[%d] %x\n", ch_roop, Dd_TMR32_Get_Reload_Flg(ch_roop)) );
			}
			else {
				Ddim_Print( ("Dd_TMR32_Set_Reload_Flg[%d] ERR=%d\n", ch_roop, ret) );
			}
		}
	}
	else{
		Ddim_Print( ("please check 2nd parameter!!\n" ) );
	}
}


/*-------------*/
/* 32bit Timer */
/*-------------*/
static VOID ct_timer32_cb( VOID )
{
#ifdef	CO_CT_DD_TMR32_USTIME
#else	// CO_CT_DD_TMR32_USTIME
	DDIM_USER_ER	er;
#endif	// CO_CT_DD_TMR32_USTIME

#if 0	// for Debug code
	//for Oscilloscope///////////////	//This is not needed for PT.
	// DSR_C and DSR_C are not used.
//	if (IO_CHIPTOP.PDR.bit.P41 == 0){
	if (*(volatile unsigned long*)0x1DFFB004 == 0x00400000){
		//IO_CHIPTOP.PDR_S.bit.P41=1;		// DSR_S is not defined. ok???
		//IO_CHIPTOP.PDR.bit.P41=1;
		*(volatile unsigned long*)0x1DFFB004 = 0x00400040;
	}else{
		//IO_CHIPTOP.PDR_C.bit.P41=1;		// DSR_C is not defined. ok???
		//IO_CHIPTOP.PDR.bit.P41=0;
		*(volatile unsigned long*)0x1DFFB004 = 0x00400000;
	}
//	IO_CHIPTOP.PDR_S.bit.P41=1;	//oscilloscope
#endif	// for Debug code

#ifdef	CO_CT_DD_TMR32_USTIME
	gDd_timer32_tim2 = CT_DD_TMR32_Get_Time_us();
#else	// CO_CT_DD_TMR32_USTIME
	er = DDIM_User_Get_Tim( &gDd_timer32_tim2 );
	if( er != D_DDIM_USER_E_OK ){
		Ddim_Print(("get_tim() er=%d\n",er));
	}
#endif	// CO_CT_DD_TMR32_USTIME
	Ddim_Print(("TMR32 INT. %lu\n", (ULONG)(gDd_timer32_tim2 - gDd_timer32_tim1) ));
	gDd_timer32_tim1 = gDd_timer32_tim2 ;
}
