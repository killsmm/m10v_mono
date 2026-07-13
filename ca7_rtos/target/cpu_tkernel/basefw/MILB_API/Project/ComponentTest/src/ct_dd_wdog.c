/**
 * @file		ct_dd_wdog.c
 * @brief		dd_wdog Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "dd_gic.h"
#include "dd_wdog.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID test_callback(VOID)
{
	UCHAR	raw_wdog_int;
	UCHAR	wdog_int;
	
	Ddim_Print(("Watchdog Timer Interrupt\n"));
	Dd_WDOG_Get_Status(&raw_wdog_int, &wdog_int);
	Ddim_Print(("WDOGRIS.STATUS = 0x%08x\n", raw_wdog_int));
	Ddim_Print(("WDOGMIS.STATUS = 0x%08x\n", wdog_int));
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for watchdog timer test.
 * @param  INT32 argc	:The number of parameters
 *		   UCHAR** argv	:The value of parameters
 *
 *			The meaning of parameters
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| P1   | P2         | P3          | P4            |Meaning                                             |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 1    |            |             |               |Initialize Watchdog timer Register.                 |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 2    |            |             |               |Exclusively controll of Watchdog timer Open/Close.  |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 3    |wdog_load   |reset_enable |int_enable     |Watchdog timer Controller.                          |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 4    |wdog_wdmode |msec         |               |Watchdog timer Timer Set.                           |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 5    |            |             |               |Watchdog timer Get API.                             |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 6    |            |             |               |Watchdog timer Error Test.                          |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 7    |            |             |               |Watchdog timer Interrupt Handler                    |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 8    |            |             |               |Watchdog timer API Sequence1.                       |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *			| 9    |            |             |               |Watchdog timer API Sequence2.                       |
 *			+------+------------+-------------+---------------+----------------------------------------------------+
 *
 * @return VOID
 */

VOID Ct_Dd_WDOG_Main(int argc, char** argv)
{
	INT32  	ercd;
	UCHAR	raw_wdog_int;
	UCHAR	wdog_int;
	UCHAR	wdog_lock;
	UCHAR	test_mode;
	ULONG	wdog_load;
	ULONG	wdog_counter;
	ULONG	wdog_wdmode;
	ULONG	msec;
	ULONG	pcell;
	T_DD_WDOG_PERI_IDENTIFICATION	identification;
	T_DD_WDOG_CTRL					wdog_ctrl;
	
	switch ( strtol(argv[1], NULL, 16) ) {
		case 1:		// Init
			Ddim_Print(("Dd_WDOG_Init Test Start\n"));
			Dd_WDOG_Init();
			
			// WDOGLOCK
			Dd_WDOG_Get_Lock(&wdog_lock);
			Ddim_Print(("WDOGLOCK = 0x%08x (set 0x00000001)\n", wdog_lock));
			// WDOGITCR
			Dd_WDOG_Get_Test_Mode(&test_mode);
			Ddim_Print(("WDOGITCR = 0x%08x (set 0x00000000)\n", test_mode));
			// WDOGCONTROL
			Dd_WDOG_Get_Control(&wdog_ctrl);
			Ddim_Print(("WDOGCONTROL.RESEN = 0x%08x (set 0x00000000)\n", wdog_ctrl.reset_enable));
			Ddim_Print(("WDOGCONTROL.INTEN = 0x%08x (set 0x00000000)\n", wdog_ctrl.int_enable));
			
			Ddim_Print(("Dd_WDOG_Init Test End\n"));
			break;
			
		case 2:		// Open/Close
			// D_DDIM_USER_SEM_WAIT_POL
			Ddim_Print(("Dd_WDOG_Open / Dd_WDOG_Close POL Test Start\n"));
			ercd = Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_POL);
			Ddim_Print(("Dd_WDOG_Open return = 0x%08x\n", ercd));
			if( ercd == D_DDIM_OK ){
				ercd = Dd_WDOG_Close();
				Ddim_Print(("Dd_WDOG_Close return = 0x%08x\n", ercd));
			}
			Ddim_Print(("Dd_WDOG_Open / Dd_WDOG_Close POL Test End\n"));
			
			// Twai
			Ddim_Print(("Dd_WDOG_Open / Dd_WDOG_Close Twai Test Start\n"));
			ercd = Dd_WDOG_Open(200);
			Ddim_Print(("Dd_WDOG_Open return = 0x%08x\n", ercd));
			if( ercd == D_DDIM_OK ){
				ercd = Dd_WDOG_Close();
				Ddim_Print(("Dd_WDOG_Close return = 0x%08x\n", ercd));
			}
			Ddim_Print(("Dd_WDOG_Open / Dd_WDOG_Close Twai Test End\n"));
			
			break;
			
		case 3:		// Ctrl
			Ddim_Print(("Dd_WDOG_Ctrl Test Start\n"));
			wdog_ctrl.wdog_load = strtoul(argv[2], NULL, 16);
			wdog_ctrl.reset_enable = strtoul(argv[3], NULL, 16);
			wdog_ctrl.int_enable = strtoul(argv[4], NULL, 16);
			wdog_ctrl.pCallback = test_callback;
			
			if( wdog_ctrl.wdog_load <= 0 ){
				wdog_ctrl.wdog_load = 0;
			}
			if( wdog_ctrl.reset_enable <= 0 ){
				wdog_ctrl.reset_enable = 0;
			}
			if( wdog_ctrl.int_enable <= 0 ){
				wdog_ctrl.int_enable = 0;
			}
			if( Dd_WDOG_Ctrl(&wdog_ctrl) == D_DDIM_OK ){
				// WDOGLOAD
				Dd_WDOG_Get_Load(&wdog_load);
				Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
				// WDOGVALUE
				Dd_WDOG_Get_Counter(&wdog_counter);
				Ddim_Print(("WDOGVALUE = 0x%08lx\n", wdog_counter));
				// WDOGCONTROL
				Dd_WDOG_Get_Control(&wdog_ctrl);
				Ddim_Print(("WDOGCONTROL.RESEN = 0x%08x\n", wdog_ctrl.reset_enable));
				Ddim_Print(("WDOGCONTROL.INTEN = 0x%08x\n", wdog_ctrl.int_enable));
			}
			else{
				Ddim_Print(("please check parameter!!\n"));
			}
			
			Ddim_Print(("Dd_WDOG_Ctrl Test End\n"));
			
			break;
			
		case 4:		// Timer Set
			Ddim_Print(("Dd_WDOG_Set_Timer Test Start\n"));
			wdog_wdmode = strtoul(argv[2], NULL, 16);
			msec = strtoul(argv[3], NULL, 16);
			
			if( wdog_wdmode <= 0 ){
				wdog_wdmode = 0;
			}
			if( msec <= 0 ){
				msec = 0;
			}
			if( Dd_WDOG_Set_Timer(wdog_wdmode, msec) == D_DDIM_OK ){
				// WDOGLOAD
				Dd_WDOG_Get_Load(&wdog_load);
				Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
				// WDOGVALUE
				Dd_WDOG_Get_Counter(&wdog_counter);
				Ddim_Print(("WDOGVALUE = 0x%08lx\n", wdog_counter));
			}
			
			Ddim_Print(("Dd_WDOG_Set_Timer Test End\n"));
			
			break;
			
		case 5:		// Get API
			// WDOGPERIPHID0~3
			Dd_WDOG_Get_Peri_Identification(&identification);
			Ddim_Print(("WDOGPERIPHID Get part_number = 0x%08x\n", identification.part_number));
			Ddim_Print(("WDOGPERIPHID Get designer = 0x%08x\n", identification.designer));
			Ddim_Print(("WDOGPERIPHID Get revision_number = 0x%08x\n", identification.revision_number));
			Ddim_Print(("WDOGPERIPHID Get configuration = 0x%08x\n", identification.configuration));
			// WDOGPCELLID0~3
			Dd_WDOG_Get_PCell_Identification(&pcell);
			Ddim_Print(("WDOGPCELLID Get WDOGPCELLID[3:0] = 0x%08lx\n", pcell));
			
			break;
			
		case 6:		// Error Test
			Ddim_Print(("Dd_WDOG_Open Error Test\n"));
			ercd = Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_FEVR - 1);
			Ddim_Print(("Dd_WDOG_Open Error = 0x%08x\n", ercd));
			ercd = Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_POL);
			ercd = Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_POL);
			Ddim_Print(("Dd_WDOG_Open Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Close Error Test\n"));
			ercd = Dd_WDOG_Close();
			Ddim_Print(("Dd_WDOG_Close Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Ctrl Error Test\n"));
			ercd = Dd_WDOG_Ctrl(NULL);
			Ddim_Print(("Dd_WDOG_Ctrl Error = 0x%08x\n", ercd));
			wdog_ctrl.wdog_load = 0;
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_ON;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_OFF;
			ercd = Dd_WDOG_Ctrl(&wdog_ctrl);
			Ddim_Print(("Dd_WDOG_Ctrl Error = 0x%08x\n", ercd));
			wdog_ctrl.wdog_load = 0xFFFFFFFF;
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_ON + 1;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON;
			ercd = Dd_WDOG_Ctrl(&wdog_ctrl);
			Ddim_Print(("Dd_WDOG_Ctrl Error = 0x%08x\n", ercd));
			wdog_ctrl.wdog_load = 0xFFFFFFFF;
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_OFF;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON + 1;
			ercd = Dd_WDOG_Ctrl(&wdog_ctrl);
			Ddim_Print(("Dd_WDOG_Ctrl Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Load Error Test\n"));
			ercd = Dd_WDOG_Get_Load(NULL);
			Ddim_Print(("Dd_WDOG_Get_Load Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Counter Error Test\n"));
			ercd = Dd_WDOG_Get_Counter(NULL);
			Ddim_Print(("Dd_WDOG_Get_Counter Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Control Error Test\n"));
			ercd = Dd_WDOG_Get_Control(NULL);
			Ddim_Print(("Dd_WDOG_Get_Control Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Status Error Test\n"));
			ercd = Dd_WDOG_Get_Status(NULL, &wdog_int);
			Ddim_Print(("Dd_WDOG_Get_Status Error = 0x%08x\n", ercd));
			ercd = Dd_WDOG_Get_Status(&raw_wdog_int, NULL);
			Ddim_Print(("Dd_WDOG_Get_Status Error = 0x%08x\n", ercd));
			ercd = Dd_WDOG_Get_Status(NULL, NULL);
			Ddim_Print(("Dd_WDOG_Get_Status Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Lock Error Test\n"));
			ercd = Dd_WDOG_Get_Lock(NULL);
			Ddim_Print(("Dd_WDOG_Get_Lock Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Test_Mode Error Test\n"));
			ercd = Dd_WDOG_Get_Test_Mode(NULL);
			Ddim_Print(("Dd_WDOG_Get_Test_Mode Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_Peri_Identification Error Test\n"));
			ercd = Dd_WDOG_Get_Peri_Identification(NULL);
			Ddim_Print(("Dd_WDOG_Get_Peri_Identification Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Get_PCell_Identification Error Test\n"));
			ercd = Dd_WDOG_Get_PCell_Identification(NULL);
			Ddim_Print(("Dd_WDOG_Get_PCell_Identification Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Set_Load Error Test\n"));
			ercd = Dd_WDOG_Set_Load(0);
			Ddim_Print(("Dd_WDOG_Set_Load Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Set_Control Error Test\n"));
			ercd = Dd_WDOG_Set_Control(NULL);
			Ddim_Print(("Dd_WDOG_Set_Control Error = 0x%08x\n", ercd));
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_ON + 1;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON;
			ercd = Dd_WDOG_Set_Control(&wdog_ctrl);
			Ddim_Print(("Dd_WDOG_Set_Control Error = 0x%08x\n", ercd));
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_OFF;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON + 1;
			ercd = Dd_WDOG_Set_Control(&wdog_ctrl);
			Ddim_Print(("Dd_WDOG_Set_Control Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Set_Lock Error Test\n"));
			ercd = Dd_WDOG_Set_Lock(D_DD_WDOG_ENABLE_ON + 1);
			Ddim_Print(("Dd_WDOG_Set_Lock Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Set_Test_Mode Error Test\n"));
			ercd = Dd_WDOG_Set_Test_Mode(D_DD_WDOG_ENABLE_ON + 1, D_DD_WDOG_ENABLE_ON, D_DD_WDOG_ENABLE_OFF);
			Ddim_Print(("Dd_WDOG_Set_Test_Mode Error = 0x%08x\n", ercd));
			ercd = Dd_WDOG_Set_Test_Mode(D_DD_WDOG_ENABLE_OFF, D_DD_WDOG_ENABLE_ON + 1, D_DD_WDOG_ENABLE_ON);
			Ddim_Print(("Dd_WDOG_Set_Test_Mode Error = 0x%08x\n", ercd));
			ercd = Dd_WDOG_Set_Test_Mode(D_DD_WDOG_ENABLE_ON, D_DD_WDOG_ENABLE_OFF, D_DD_WDOG_ENABLE_ON + 1);
			Ddim_Print(("Dd_WDOG_Set_Test_Mode Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Set_Timer Error Test\n"));
			ercd = Dd_WDOG_Set_Timer(2, 1000);
			Ddim_Print(("Dd_WDOG_Set_Timer Error = 0x%08x\n", ercd));
			ercd = Dd_WDOG_Set_Timer(1, 0);
			Ddim_Print(("Dd_WDOG_Set_Timer Error = 0x%08x\n", ercd));
			
			Ddim_Print(("Dd_WDOG_Calculate Error Test\n"));
			ercd = Dd_WDOG_Calculate(1, 1000, NULL);
			Ddim_Print(("Dd_WDOG_Calculate Error = 0x%08x\n", ercd));
			
			break;
			
		case 7:		// Int_Handler
			Ddim_Print(("Dd_WDOG_Int_Handler Test Start\n"));
			Dd_WDOG_Set_Timer(0, 1000);  // (PCLK = 50MHz) -> (gDD_Wdog_Soft_Counter = 0)
			Dd_WDOG_Int_Handler();
			// WDOGLOAD
			Dd_WDOG_Get_Load(&wdog_load);
			Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
			
			Dd_WDOG_Set_Timer(0, 300000);  // (PCLK = 50MHz) -> (gDD_Wdog_Soft_Counter = 3)
			Dd_WDOG_Int_Handler();
			// WDOGLOAD
			Dd_WDOG_Get_Load(&wdog_load);
			Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
			
			Dd_WDOG_Int_Handler();
			// WDOGLOAD
			Dd_WDOG_Get_Load(&wdog_load);
			Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
			
			Dd_WDOG_Int_Handler();
			// WDOGLOAD
			Dd_WDOG_Get_Load(&wdog_load);
			Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
			
			Dd_WDOG_Int_Handler();
			// WDOGLOAD
			Dd_WDOG_Get_Load(&wdog_load);
			Ddim_Print(("WDOGLOAD = 0x%08lx\n", wdog_load));
			Ddim_Print(("Dd_WDOG_Int_Handler Test End\n"));
			
			break;
			
		case 8:		// Sequence1
			Ddim_Print(("Sequence1 Test Start\n"));
			
			wdog_ctrl.wdog_load = 1000000000;
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_OFF;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_ON;
			wdog_ctrl.pCallback = test_callback;
			
			Dd_WDOG_Init();
			Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_POL);
			Dd_WDOG_Ctrl(&wdog_ctrl);
			
			Dd_WDOG_Close();
			
			Ddim_Print(("Sequence1 Test End\n"));
			
			break;
			
		case 9:		// Sequence2
			Ddim_Print(("Sequence2 Test Start\n"));
			
			wdog_ctrl.wdog_load = 1000000;
			wdog_ctrl.reset_enable = D_DD_WDOG_ENABLE_ON;
			wdog_ctrl.int_enable = D_DD_WDOG_ENABLE_OFF;
			wdog_ctrl.pCallback = NULL;
			
			Dd_WDOG_Init();
			Dd_WDOG_Open(D_DDIM_USER_SEM_WAIT_POL);
			Dd_WDOG_Ctrl(&wdog_ctrl);
			
			Dd_WDOG_Get_Status(&raw_wdog_int, &wdog_int);
			Ddim_Print(("WDOGRIS.STATUS = 0x%08x\n", raw_wdog_int));
			Ddim_Print(("WDOGMIS.STATUS = 0x%08x\n", wdog_int));
			
			Dd_WDOG_Close();
			
			Ddim_Print(("Sequence2 Test End\n"));
			
			break;
			
		default :
			Ddim_Print(("please check 1st parameter!!\n"));
	}
}
