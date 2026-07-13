/**
 * @file		ct_dd_timestamp.c
 * @brief		dd_timestamp Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "dd_timestamp.h"

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
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for TIMESTAMP test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *		The meaning of parameters
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| P1  | P2       | P3         | P4         | P5         |Meaning                                              |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 1   | -        | -          | -          | -          |Initialize Timestamp Register.                       |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 2   | -        | -          | -          | -          |Exclusively controll of Timestamp Open/Close.        |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 3   | hdbg     | counter    | frequency  | -          |Timestamp Controller.                                |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 4   | -        | -          | -          | -          |Timestamp Start/Stop.                                |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 5   | counter  | -          | -          | -          |Timestamp Counter Set/Get.                           |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 6   | usec     | frequency  | -          | -          |Timestamp Timer Set.                                 |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 7   | -        | -          | -          | -          |Timestamp Get API.                                   |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 8   | -        | -          | -          | -          |Timestamp Error Test.                                |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 *		| 9   | -        | -          | -          | -          |Timestamp API Sequence.                              |
 *		+-----+----------+------------+------------+------------+-----------------------------------------------------+
 */
VOID Ct_Dd_TIMESTAMP_Main(int argc, char** argv)
{
	INT32 dat;
	T_DD_TIMESTAMP_CTRL timestamp_ctrl;
	T_DD_TIMESTAMP_CTRL timestamp_ctrl_get;
	UINT64 timestamp_counter;
	UINT64 timestamp_counter_get;
	ULONG usec;
	ULONG frequency;
	
	
	switch ( strtol(argv[1], NULL, 16) ) {
		case 1:		// Init
			Ddim_Print(("Dd_TIMESTAMP_Init Test Start\n"));
			Dd_TIMESTAMP_Init();
			
			// CNTCR
			dat = Dd_TIMESTAMP_Get_CNTCR_En;
			Ddim_Print(("CNTCR EN = 0x%08x (set 0x00000000)\n", dat));
			dat = Dd_TIMESTAMP_Get_CNTCR_HDbg;
			Ddim_Print(("CNTCR HDBG = 0x%08x (set 0x00000000)\n", dat));
			// CNTCVL
			dat = Dd_TIMESTAMP_Get_CNTCVL;
			Ddim_Print(("CNTCVL = 0x%08x (set 0x00000000)\n", dat));
			// CNTCVU
			dat = Dd_TIMESTAMP_Get_CNTCVU;
			Ddim_Print(("CNTCVU = 0x%08x (set 0x00000000)\n", dat));
			// CNTFID0
			dat = Dd_TIMESTAMP_Get_CNTFID0;
			Ddim_Print(("CNTFID0 = 0x%08x (set 0x00000000)\n", dat));

			Ddim_Print(("Dd_TIMESTAMP_Init Test End\n"));
			break;
			
		case 2:		// Open/Close
			// D_DDIM_USER_SEM_WAIT_POL
			Ddim_Print(("Dd_TIMESTAMP_Open / Dd_TIMESTAMP_Close POL Test Start\n"));
			dat = Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL);
			Ddim_Print(("Dd_TIMESTAMP_Open return = 0x%08x\n", dat));
			if( dat == D_DDIM_OK ){
				dat = Dd_TIMESTAMP_Close();
				Ddim_Print(("Dd_TIMESTAMP_Close return = 0x%08x\n", dat));
			}
			Ddim_Print(("Dd_TIMESTAMP_Open / Dd_TIMESTAMP_Close POL Test End\n"));
			
			// Twai
			Ddim_Print(("Dd_TIMESTAMP_Open / Dd_TIMESTAMP_Close Twai Test Start\n"));
			dat = Dd_TIMESTAMP_Open(200);
			Ddim_Print(("Dd_TIMESTAMP_Open return = 0x%08x\n", dat));
			if( dat == D_DDIM_OK ){
				dat = Dd_TIMESTAMP_Close();
				Ddim_Print(("Dd_TIMESTAMP_Close return = 0x%08x\n", dat));
			}
			Ddim_Print(("Dd_TIMESTAMP_Open / Dd_TIMESTAMP_Close Twai Test End\n"));
			
			break;
			
		case 3:		// Ctrl
			Ddim_Print(("Dd_TIMESTAMP_Ctrl Test Start\n"));
			timestamp_ctrl.hdbg = strtoul(argv[2], NULL, 16);
			timestamp_ctrl.counter = strtoull(argv[3], NULL, 16);
			timestamp_ctrl.frequency = strtoul(argv[4], NULL, 16);
			
			if( Dd_TIMESTAMP_Ctrl(&timestamp_ctrl) == D_DDIM_OK ){
				// CNTCR
				dat = Dd_TIMESTAMP_Get_CNTCR_HDbg;
				Ddim_Print(("CNTCR HDBG = 0x%08x\n", dat));
				// CNTCVL
				dat = Dd_TIMESTAMP_Get_CNTCVL;
				Ddim_Print(("CNTCVL = 0x%08x\n", dat));
				// CNTCVU
				dat = Dd_TIMESTAMP_Get_CNTCVU;
				Ddim_Print(("CNTCVU = 0x%08x\n", dat));
				// CNTFID0
				dat = Dd_TIMESTAMP_Get_CNTFID0;
				Ddim_Print(("CNTFID0 = 0x%08x\n", dat));
				// T_DD_TIMESTAMP_CTRL
				dat = Dd_TIMESTAMP_Get_Control(&timestamp_ctrl_get);
				if( dat == D_DDIM_OK ){
					Ddim_Print(("T_DD_TIMESTAMP_CTRL hdbg = 0x%08lx\n", timestamp_ctrl_get.hdbg));
					Ddim_Print(("T_DD_TIMESTAMP_CTRL counter = 0x%016llx\n", timestamp_ctrl_get.counter));
					Ddim_Print(("T_DD_TIMESTAMP_CTRL frequency = 0x%08lx\n", timestamp_ctrl_get.frequency));
				}
			}
			else{
				Ddim_Print(("please check parameter!!\n"));
			}
			
			Ddim_Print(("Dd_TIMESTAMP_Ctrl Test End\n"));
			
			break;
			
		case 4:		// Start/Stop
			Ddim_Print(("Dd_TIMESTAMP_Start / Dd_TIMESTAMP_Stop Test Start\n"));
			
			Dd_TIMESTAMP_Start();
			// CNTCR EN
			dat = Dd_TIMESTAMP_Get_CNTCR_En;
			Ddim_Print(("CNTCR EN = 0x%08x (set 0x00000001)\n", dat));
			
			Dd_TIMESTAMP_Stop();
			// CNTCR EN
			dat = Dd_TIMESTAMP_Get_CNTCR_En;
			Ddim_Print(("CNTCR EN = 0x%08x (set 0x00000000)\n", dat));
			
			Ddim_Print(("Dd_TIMESTAMP_Start / Dd_TIMESTAMP_Stop Test End\n"));
			
			break;
			
		case 5:		// Counter Set/Get
			Ddim_Print(("Dd_TIMESTAMP_Set_Counter / Dd_TIMESTAMP_Get_Counter Test Start\n"));
			timestamp_counter = strtoull(argv[2], NULL, 16);
			
			if( timestamp_counter <= 0 ){
				timestamp_counter = 0;
			}
			if( Dd_TIMESTAMP_Set_Counter(timestamp_counter) == D_DDIM_OK ){
				if( Dd_TIMESTAMP_Get_Counter(&timestamp_counter_get) == D_DDIM_OK ){
					Ddim_Print(("Counter = 0x%016llx\n", timestamp_counter_get));
				}
			}
			
			Ddim_Print(("Dd_TIMESTAMP_Set_Counter / Dd_TIMESTAMP_Get_Counter Test End\n"));
			
			break;
			
		case 6:		// Timer Set
			Ddim_Print(("Dd_TIMESTAMP_Set_Timer Test Start\n"));
			usec = strtoul(argv[2], NULL, 16);
			frequency = strtoul(argv[3], NULL, 16);
			
			if( usec <= 0 ){
				usec = 0;
			}
			if( frequency <= 0 ){
				frequency = 0;
			}
			if( Dd_TIMESTAMP_Set_Timer(usec, frequency) == D_DDIM_OK ){
				// CNTCR
				dat = Dd_TIMESTAMP_Get_CNTCR_En;
				Ddim_Print(("CNTCR EN = 0x%08x\n", dat));
				// CNTCVL
				dat = Dd_TIMESTAMP_Get_CNTCVL;
				Ddim_Print(("CNTCVL = 0x%08x\n", dat));
				// CNTCVU
				dat = Dd_TIMESTAMP_Get_CNTCVU;
				Ddim_Print(("CNTCVU = 0x%08x\n", dat));
				// CNTFID0
				dat = Dd_TIMESTAMP_Get_CNTFID0;
				Ddim_Print(("CNTFID0 = 0x%08x\n", dat));
			}
			
			Ddim_Print(("Dd_TIMESTAMP_Set_Timer Test End\n"));
			
			break;
			
		case 7:		// Get API
			// CNTSR
			dat = Dd_TIMESTAMP_Get_CNTSR_Dbgh;
			Ddim_Print(("CNTSR Get DBGH = %d\n", dat));
			// PERIPID4
			dat = Dd_TIMESTAMP_Get_PIDR4_Des_2;
			Ddim_Print(("PERIPID4 Get DES_2 = %d\n", dat));
			dat = Dd_TIMESTAMP_Get_PIDR4_Size;
			Ddim_Print(("PERIPID4 Get SIZE = %d\n", dat));
			// PERIPID0
			dat = Dd_TIMESTAMP_Get_PIDR0_Part_0;
			Ddim_Print(("PERIPID0 Get PART_0 = %d\n", dat));
			// PERIPID1
			dat = Dd_TIMESTAMP_Get_PIDR1_Des_0;
			Ddim_Print(("PERIPID1 Get DES_0 = %d\n", dat));
			dat = Dd_TIMESTAMP_Get_PIDR1_Part_1;
			Ddim_Print(("PERIPID1 Get PART_1 = %d\n", dat));
			// PERIPID2
			dat = Dd_TIMESTAMP_Get_PIDR2_Des_1;
			Ddim_Print(("PERIPID2 Get DES_1 = %d\n", dat));
			dat = Dd_TIMESTAMP_Get_PIDR2_Jedec;
			Ddim_Print(("PERIPID2 Get JEDEC = %d\n", dat));
			dat = Dd_TIMESTAMP_Get_PIDR2_Revision;
			Ddim_Print(("PERIPID2 Get REVISION = %d\n", dat));
			// PERIPID3
			dat = Dd_TIMESTAMP_Get_PIDR3_Cmod;
			Ddim_Print(("PERIPID3 Get CMOD = %d\n", dat));
			dat = Dd_TIMESTAMP_Get_PIDR3_Revand;
			Ddim_Print(("PERIPID3 Get REVAND = %d\n", dat));
			// CIDR0
			dat = Dd_TIMESTAMP_Get_CIDR0_Prmbl_0;
			Ddim_Print(("CIDR0 Get PRMBL_0 = %d\n", dat));
			// CIDR1
			dat = Dd_TIMESTAMP_Get_CIDR1_Prmbl_1;
			Ddim_Print(("CIDR1 Get PRMBL_1 = %d\n", dat));
			dat = Dd_TIMESTAMP_Get_CIDR1_Class;
			Ddim_Print(("CIDR1 Get CLASS = %d\n", dat));
			// CIDR2
			dat = Dd_TIMESTAMP_Get_CIDR2_Prmbl_2;
			Ddim_Print(("CIDR2 Get PRMBL_2 = %d\n", dat));
			// CIDR3
			dat = Dd_TIMESTAMP_Get_CIDR3_Prmbl_3;
			Ddim_Print(("CIDR3 Get PRMBL_3 = %d\n", dat));
			
			break;
			
		case 8:		// Error Test
			Ddim_Print(("Dd_TIMESTAMP_Open Error Test\n"));
			dat = Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_FEVR - 1);
			Ddim_Print(("Dd_TIMESTAMP_Open Error = 0x%08x\n", dat));
			dat = Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL);
			dat = Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL);
			Ddim_Print(("Dd_TIMESTAMP_Open Error = 0x%08x\n", dat));
			
			Ddim_Print(("Dd_TIMESTAMP_Ctrl Error Test\n"));
			dat = Dd_TIMESTAMP_Ctrl(NULL);
			Ddim_Print(("Dd_TIMESTAMP_Ctrl Error = 0x%08x\n", dat));
			timestamp_ctrl.hdbg = 0;
			timestamp_ctrl.counter = 0;
			timestamp_ctrl.frequency = 0x0;
			dat = Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
			Ddim_Print(("Dd_TIMESTAMP_Ctrl Error = 0x%08x\n", dat));
			timestamp_ctrl.hdbg = 0;
			timestamp_ctrl.counter = 0;
			timestamp_ctrl.frequency = 0xFFFFFFFF;
			dat = Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
			Ddim_Print(("Dd_TIMESTAMP_Ctrl Error = 0x%08x\n", dat));
			
			Ddim_Print(("Dd_TIMESTAMP_Get_Counter Error Test\n"));
			dat = Dd_TIMESTAMP_Get_Counter(NULL);
			Ddim_Print(("Dd_TIMESTAMP_Get_Counter Error = 0x%08x\n", dat));
			
			Ddim_Print(("Dd_TIMESTAMP_Get_Control Error Test\n"));
			dat = Dd_TIMESTAMP_Get_Control(NULL);
			Ddim_Print(("Dd_TIMESTAMP_Get_Control Error = 0x%08x\n", dat));
			
			Ddim_Print(("Dd_TIMESTAMP_Set_Timer Error Test\n"));
			dat = Dd_TIMESTAMP_Set_Timer(1000000, 0x0);
			Ddim_Print(("Dd_TIMESTAMP_Set_Timer Error = 0x%08x\n", dat));
			dat = Dd_TIMESTAMP_Set_Timer(1000000, 0xFFFFFFFF);
			Ddim_Print(("Dd_TIMESTAMP_Set_Timer Error = 0x%08x\n", dat));
			
			break;
			
		case 9:		// Sequence
			Ddim_Print(("Sequence Test Start\n"));
			timestamp_ctrl.hdbg = 0;						// has no effect
			timestamp_ctrl.counter = 0x0000000000000000;	// Counter
			timestamp_ctrl.frequency = 0xEE6B2800;			// 4GHz
			
			Dd_TIMESTAMP_Init();
			Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL);
			Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
			
			Dd_TIMESTAMP_Start();
			
			Dd_TIMESTAMP_Get_Control(&timestamp_ctrl_get);
			Ddim_Print(("T_DD_TIMESTAMP_CTRL hdbg = 0x%08lx\n", timestamp_ctrl_get.hdbg));
			Ddim_Print(("T_DD_TIMESTAMP_CTRL counter = 0x%016llx\n", timestamp_ctrl_get.counter));
			Ddim_Print(("T_DD_TIMESTAMP_CTRL frequency = 0x%08lx\n", timestamp_ctrl_get.frequency));
			
			Dd_TIMESTAMP_Stop();
			Dd_TIMESTAMP_Close();
			
			Ddim_Print(("Sequence Test End\n"));
			break;
			
		default :
			Ddim_Print(("please check 1st parameter!!\n"));
	}
}
