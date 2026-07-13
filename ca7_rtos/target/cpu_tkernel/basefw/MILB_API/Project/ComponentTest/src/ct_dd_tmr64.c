/**
 * @file		ct_dd_tmr64.c
 * @brief		This is ct code for dd_tmr64
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_dd_tmr64.h"
#include "dd_tmr64.h"
#include "dd_hdmac1.h"
#include "peripheral.h"
#include "chiptop.h"
#include "ddim_user_custom.h"
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for timer64 test.
 * @param  int argc		:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 */
VOID Ct_Dd_TMR64_Main( int argc, char** argv )
{
	INT32				ret;
	T_DD_TMR64_CTRL		tmr64_ctrl = {{0}};
	DDIM_USER_SYSTIM	systim;

	/*-------*/
	/* Open  */
	/*-------*/
	// ddtmr64 open [tmout]
	if( strcmp(argv[1], "open") == 0 ){
		/* Open */
		ret = Dd_TMR64_Open( (INT32)atoi(argv[2]) );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Open OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Open ERR=%d\n", ret) );
		}
	}
	/*-------*/
	/* close */
	/*-------*/
	// ddtmr64 close
	else if( strcmp(argv[1], "close") == 0 ){
		/* Stop */
		ret = Dd_TMR64_Close();
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Close OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Close ERR=%d\n", ret) );
		}
	}
	/*------*/
	/* Ctrl */
	/*------*/
	// ddtmr64 ctrl [trg] [cnte] [csl] [cvt] [tsen]
	else if( strcmp(argv[1], "ctrl") == 0 ){
		// trg
		if( strcmp(argv[2], "1") == 0 ){
			tmr64_ctrl.csr1.bit.trg = 1;
		}
		else {
			tmr64_ctrl.csr1.bit.trg = 0;
		}

		// cnte
		if( strcmp(argv[3], "1") == 0 ){
			tmr64_ctrl.csr1.bit.cnte = 1;
		}
		else {
			tmr64_ctrl.csr1.bit.cnte = 0;
		}

		// csl
		if( strcmp(argv[4], "32") == 0 ){
			tmr64_ctrl.csr1.bit.csl = 2;
		}
		else if( strcmp(argv[4], "8") == 0 ){
			tmr64_ctrl.csr1.bit.csl = 1;
		}
		else if( strcmp(argv[4], "2") == 0 ){
			tmr64_ctrl.csr1.bit.csl = 0;
		}
		else {
			tmr64_ctrl.csr1.bit.csl = 0;
		}

		// cvt
		if( strcmp(argv[5], "1") == 0 ){
			tmr64_ctrl.csr2.bit.cvt = 1;
		}
		else {
			tmr64_ctrl.csr2.bit.cvt = 0;
		}

		// tsen
		if( strcmp(argv[6], "1") == 0 ){
			tmr64_ctrl.csr2.bit.tsen = 1;
		}
		else {
			tmr64_ctrl.csr2.bit.tsen = 0;
		}

		Ddim_Print( ("--Dd_TMR64_Ctrl()--\n") );
		Ddim_Print( ("tmr64_ctrl.csr1.bit.trg  = %d\n", tmr64_ctrl.csr1.bit.trg) );
		Ddim_Print( ("tmr64_ctrl.csr1.bit.cnte = %d\n", tmr64_ctrl.csr1.bit.cnte) );
		Ddim_Print( ("tmr64_ctrl.csr1.bit.csl  = %d\n", tmr64_ctrl.csr1.bit.csl) );
		Ddim_Print( ("tmr64_ctrl.csr2.bit.cvt  = %d\n", tmr64_ctrl.csr2.bit.cvt) );
		Ddim_Print( ("tmr64_ctrl.csr2.bit.tsen = %d\n", tmr64_ctrl.csr2.bit.tsen) );
		Ddim_Print( ("-------------------\n") );

		/* Ctrl */
		ret = Dd_TMR64_Ctrl( &tmr64_ctrl );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Ctrl OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Ctrl ERR=%d\n", ret) );
		}
		Ddim_Print(("IO_PERI.TMR64.TMCSR64_1.word = [0x%08lx]\n", IO_PERI.TMR64.TMCSR64_1.word));
		Ddim_Print(("IO_PERI.TMR64.TMCSR64_2.word = [0x%08lx]\n", IO_PERI.TMR64.TMCSR64_2.word));
	}
	else if( strcmp(argv[1], "get_ctrl") == 0 ){
#ifdef CO_DEBUG_ON_PC
		IO_PERI.TMR64.TMCSR64_1.word	= 0x00000800;
		IO_PERI.TMR64.TMCSR64_2.word	= 0x00010001;
#endif	// CO_DEBUG_ON_PC

		/* Get H/W & S/W Counter */
		ret = Dd_TMR64_Get_Ctrl( &tmr64_ctrl ) ;
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_Ctrl OK\n") );
			Ddim_Print(("IO_PERI.TMR64.TMCSR64_1.word = [0x%08lx]\n", IO_PERI.TMR64.TMCSR64_1.word));
			Ddim_Print(("IO_PERI.TMR64.TMCSR64_2.word = [0x%08lx]\n", IO_PERI.TMR64.TMCSR64_2.word));
			Ddim_Print( ("tmr64_ctrl.csr1.word        = [0x%08lx]\n", tmr64_ctrl.csr1.word) );
			Ddim_Print( ("tmr64_ctrl.csr2.word        = [0x%08lx]\n", tmr64_ctrl.csr2.word) );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_Ctrl ERR=%d\n", ret) );
		}
	}
	/*----------------*/
	/* Start / Resume */
	/*----------------*/
	else if( strcmp(argv[1], "start") == 0 ){
		/* Start */
		ret = Dd_TMR64_Start(atoi(argv[2]));

		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Start OK\n") );

			ret = DDIM_User_Get_Tim(&systim);
			if (ret != D_DDIM_USER_E_OK) {
				Ddim_Print( ("DDIM_User_Get_Tim ERR=%d\n", ret) );
			}
			else {
				Ddim_Print(("System time = %llu ms.\n", systim));
			}
		}
		else {
			Ddim_Print( ("Dd_TMR64_Start ERR=%d\n", ret) );
		}
		Ddim_Print(("IO_PERI.TMR64.TMCSR64_1.word =[0x%08lx].\n", IO_PERI.TMR64.TMCSR64_1.word));
		Ddim_Print(("IO_PERI.TMR64.TMCSR64_2.word =[0x%08lx].\n", IO_PERI.TMR64.TMCSR64_1.word));
	}
	/*------*/
	/* Stop */
	/*------*/
	else if( strcmp(argv[1], "stop") == 0 ){
#ifdef CO_DEBUG_ON_PC
		Dd_TMR64_Start(TRUE);
#endif	// CO_DEBUG_ON_PC

		/* Stop */
		ret = Dd_TMR64_Stop();
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Stop OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Stop ERR=%d\n", ret) );
		}
		Ddim_Print(("IO_PERI.TMR64.TMCSR64_1.word =[0x%08lx].\n", IO_PERI.TMR64.TMCSR64_1.word));
		Ddim_Print(("IO_PERI.TMR64.TMCSR64_2.word =[0x%08lx].\n", IO_PERI.TMR64.TMCSR64_1.word));
	}
	/*-------------------------*/
	/* Get TMR64 Counter value */
	/*-------------------------*/
	else if( strcmp(argv[1], "get_counter") == 0 ){
		UINT64	counter;

#ifdef CO_DEBUG_ON_PC
		IO_PERI.TMR64.TMR64_L	= 0x13572468;
		IO_PERI.TMR64.TMR64_H	= 0x24681357;
#endif	// CO_DEBUG_ON_PC

		/* Get H/W & S/W Counter */
		ret = Dd_TMR64_Get_Counter( &counter );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_Counter OK\n") );
			Ddim_Print( ("Counter = 0x%llX\n", counter) );
			ret = DDIM_User_Get_Tim(&systim);
			if (ret != D_DDIM_USER_E_OK) {
				Ddim_Print( ("DDIM_User_Get_Tim ERR=%d\n", ret) );
			}
			else {
				Ddim_Print(("System time = %llu ms.\n", systim));
			}
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_Counter ERR=%d\n", ret) );
		}
	}
	/*--------------------------------------*/
	/* Get TMR Register and Gryo Time Stamp */
	/*--------------------------------------*/
	else if( strcmp(argv[1], "get_gyro") == 0 ){
		ULONG	time_stamp ;

#ifdef CO_DEBUG_ON_PC
		IO_PERI.TMR64.GSTMP = 0xAAAAAAAA;
#endif	// CO_DEBUG_ON_PC

		/* Get Time Stamp */
		ret = Dd_TMR64_Get_Gyro_TimeStamp( &time_stamp );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_Gyro_TimeStamp OK\n") );
			Ddim_Print( ("time_stamp = 0x%lX\n", time_stamp) );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_Gyro_TimeStamp ERR=%d\n", ret) );
		}
	}
	/*---------------------------------------------*/
	/* Get TMR Register and Frame Start Time Stamp */
	/*---------------------------------------------*/
	else if( strcmp(argv[1], "get_start") == 0 ){
		UINT32	sensor_no;
		UINT32	lotation_count;
		UINT64	time_stamp;

// ToDo: Set data for PC Debug

		Ddim_Print( ("--Dd_TMR64_Get_FrameStart_TimeStamp()--\n") );
		for (sensor_no = 0; sensor_no < 4; sensor_no ++) {
			for (lotation_count = 0; lotation_count < 3; lotation_count ++) {   // Strange???? Change?????
				/* Get Counter */
				ret = Dd_TMR64_Get_FrameStart_TimeStamp( sensor_no, lotation_count, &time_stamp );
				if( ret == D_DDIM_OK ){
					Ddim_Print( ("TimeStamp[%d][%d] = 0x%llX\n", sensor_no, lotation_count, time_stamp) );
				}
				else {
					Ddim_Print( ("TimeStamp[%d][%d] = ERR\n", sensor_no, lotation_count) );
				}
			}
		}
		Ddim_Print( ("---------------------------------------\n") );
	}
	/*-------------------------------------------*/
	/* Get TMR Register and Frame End Time Stamp */
	/*-------------------------------------------*/
	else if( strcmp(argv[1], "get_end") == 0 ){
		UINT32	sensor_no;
		UINT32	lotation_count;
		UINT64	time_stamp;

// ToDo: Set data for PC Debug

		Ddim_Print( ("--Dd_TMR64_Get_FrameEnd_TimeStamp()--\n") );
		for (sensor_no = 0; sensor_no < 4; sensor_no ++) {
			for (lotation_count = 0; lotation_count < 3; lotation_count ++) {
				/* Get Counter */
				ret = Dd_TMR64_Get_FrameEnd_TimeStamp( sensor_no, lotation_count, &time_stamp );
				if( ret == D_DDIM_OK ){
					Ddim_Print( ("TimeStamp[%d][%d] = 0x%llX\n", sensor_no, lotation_count, time_stamp) );
				}
				else {
					Ddim_Print( ("TimeStamp[%d][%d] = ERR\n", sensor_no, lotation_count) );
				}
			}
		}
		Ddim_Print( ("-------------------------------------\n") );
	}
	/*-------------------*/
	/* Init Gyro trigger */
	/*-------------------*/
	else if( strcmp(argv[1], "gyro_init") == 0 ){
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		IO_CHIPTOP.EPCR.bit.PB4 = 0;	// SPIRLTRG
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		IO_CHIPTOP.EPCR.bit.TRGEN = 0;	// SPIRLTRG
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		IO_CHIPTOP.DDR.bit.PB4 = 0;		// SPIRLTRG

//		IO_CHIPTOP.EPCR.bit.PF3 = 0;
		IO_CHIPTOP.DDR.bit.PF3 = 1;
		IO_CHIPTOP.PDR.bit.PF3 = 0;
	}
	/*----------------------*/
	/* Set Gyro trigger ON  */
	/*----------------------*/
	else if( strcmp(argv[1], "gyro_trg_on") == 0 ){
		IO_CHIPTOP.PDR.bit.PF3 = 1;		// ToDo

		ret = DDIM_User_Get_Tim(&systim);
		if (ret != D_DDIM_USER_E_OK) {
			Ddim_Print( ("DDIM_User_Get_Tim ERR=%d\n", ret) );
		}
		else {
			Ddim_Print(("System time = %llu ms.\n", systim));
		}
	}
	/*----------------------*/
	/* Set Gyro trigger OFF  */
	/*----------------------*/
	else if( strcmp(argv[1], "gyro_trg_off") == 0 ){
		IO_CHIPTOP.PDR.bit.PF3 = 0;
	}
	/*----------*/
	/* test 2-1 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_1") == 0 ){
		ret = Dd_TMR64_Open( 0 );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Open OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Open ERR=0x%08x\n", ret) );
		}

		ret = Dd_TMR64_Open( 0 );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Open OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Open ERR=0x%08x\n", ret) );
		}

		ret = Dd_TMR64_Close();
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Close OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Close ERR=0x%08x\n", ret) );
		}
	}
	/*----------*/
	/* test 2-2 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_2") == 0 ){
		ret = Dd_TMR64_Open( -2 );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Open OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Open ERR=0x%08x\n", ret) );
		}
	}
	/*----------*/
	/* test 2-3 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_3") == 0 ){
		DDIM_USER_ID sid_back;

		sid_back = SID_DD_TMR64;
		SID_DD_TMR64 = 0xfff;

		ret = Dd_TMR64_Open( 0 );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Open OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Open ERR=0x%08x\n", ret) );
		}

		SID_DD_TMR64 = sid_back;
	}
	/*----------*/
	/* test 2-4 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_4") == 0 ){
		DDIM_USER_ID sid_back;

		sid_back = SID_DD_TMR64;
		SID_DD_TMR64 = 0xfff;

		ret = Dd_TMR64_Close();
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Close OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Close ERR=0x%08x\n", ret) );
		}

		SID_DD_TMR64 = sid_back;
	}
	/*----------*/
	/* test 2-5 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_5") == 0 ){
		ret = Dd_TMR64_Ctrl( NULL );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Ctrl OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Ctrl ERR=0x%08x\n", ret) );
		}
	}
	/*----------*/
	/* test 2-6 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_6") == 0 ){
		ret = Dd_TMR64_Get_Gyro_TimeStamp( NULL );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_Gyro_TimeStamp OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_Gyro_TimeStamp ERR=0x%08x\n", ret) );
		}
	}
	/*----------*/
	/* test 2-7 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_7") == 0 ){
		UINT32	sensor_no		= 0;
		UINT32	lotation_count	= 0;
		UINT64	time_stamp;

		// time_stamp is null
		ret = Dd_TMR64_Get_FrameStart_TimeStamp( sensor_no, lotation_count, NULL );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_FrameStart_TimeStamp OK") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_FrameStart_TimeStamp ERR=0x%08x\n", ret) );
		}

		// sensor_no is over
		sensor_no		= 4;
		lotation_count	= 0;
		ret = Dd_TMR64_Get_FrameStart_TimeStamp( sensor_no, lotation_count, &time_stamp );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_FrameStart_TimeStamp OK") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_FrameStart_TimeStamp ERR=0x%08x\n", ret) );
		}

		// lotation_count is over
		sensor_no		= 0;
		lotation_count	= 4;
		ret = Dd_TMR64_Get_FrameStart_TimeStamp( sensor_no, lotation_count, &time_stamp );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_FrameStart_TimeStamp OK") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_FrameStart_TimeStamp ERR=0x%08x\n", ret) );
		}
	}
	/*----------*/
	/* test 2-8 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_8") == 0 ){
		UINT32	sensor_no		= 0;
		UINT32	lotation_count	= 0;
		UINT64	time_stamp;

		// time_stamp is null
		ret = Dd_TMR64_Get_FrameEnd_TimeStamp( sensor_no, lotation_count, NULL );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_FrameEnd_TimeStamp OK") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_FrameEnd_TimeStamp ERR=0x%08x\n", ret) );
		}

		// sensor_no is over
		sensor_no		= 4;
		lotation_count	= 0;
		ret = Dd_TMR64_Get_FrameEnd_TimeStamp( sensor_no, lotation_count, &time_stamp );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_FrameEnd_TimeStamp OK") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_FrameEnd_TimeStamp ERR=0x%08x\n", ret) );
		}

		// lotation_count is over
		sensor_no		= 0;
		lotation_count	= 4;
		ret = Dd_TMR64_Get_FrameEnd_TimeStamp( sensor_no, lotation_count, &time_stamp );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_FrameEnd_TimeStamp OK") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_FrameEnd_TimeStamp ERR=0x%08x\n", ret) );
		}
	}
	/*----------*/
	/* test 2-9 */
	/*----------*/
	else if( strcmp(argv[1], "test_2_9") == 0 ){
		// counter is null
		ret = Dd_TMR64_Get_Counter( NULL );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_CounterOK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_Counter ERR=0x%08x\n", ret) );
		}
	}
	/*-----------*/
	/* test 2-10 */
	/*-----------*/
	else if( strcmp(argv[1], "test_2_10") == 0 ){
		ret = Dd_TMR64_Get_Ctrl( NULL );
		if( ret == D_DDIM_OK ){
			Ddim_Print( ("Dd_TMR64_Get_Ctrl OK\n") );
		}
		else {
			Ddim_Print( ("Dd_TMR64_Get_Ctrl ERR=0x%08x\n", ret) );
		}
	}
	else{
		Ddim_Print( ("please check 2nd parameter!!\n" ) );
	}
}
