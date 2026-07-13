/**
 * @file		palladium_test_disp.c
 * @brief		palladium test disp code
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "ct_im_disp.h"
#include "im_disp.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "palladium_test_common.h"
#include "jdsdisp3a.h"

#include "driver_common.h"
#include "palladium_test.h"

#include "ct_im_mxic.h"
#include "im_mxic.h"

#include <string.h>
extern UCHAR gIM_DISP_PCLK_Counter;
// HCLK_Disp setting task counter (0~255)
extern UCHAR gIM_DISP_HCLK_Counter;

const char gPT_IM_DISP_TEST_str[] = {"=============== 01-01-%03d ===============\n"};

#ifdef CO_DEBUG_ON_PC
static char filename[] = "IO_DISP_00.bin";
static VOID pt_im_disp_pc_file_write(ULONG reg_address, ULONG size)
{
	if(filename[9] < 0x39) {
		filename[9]++;
	}
	else{
		filename[8]++;
		filename[9] = 0x30;
	}
	FILE *fp = fopen(filename, "w");
	if (fp != NULL) {
		fwrite((void*)reg_address, size, 1 ,fp);
		fclose(fp);
	}
	printf("Dump memory. file = %s\n\n", filename);
}
#endif // CO_DEBUG_ON_PC

/**
 * @brief		Dump register data to file 
 */
static VOID pt_im_disp_Dump_Reg(ULONG reg_address, ULONG size)
{
#ifdef CO_ACT_PCLOCK
	Dd_Top_Start_Clock(&gIM_DISP_PCLK_Counter, &Dd_Top_Get_CLKSTOP_E(), ~D_DD_TOP_PCKDISPSTP_BIT);
#endif	// CO_ACT_PCLOCK
#ifdef CO_ACT_HCLOCK
	Dd_Top_Start_Clock(&gIM_DISP_HCLK_Counter, &Dd_Top_Get_CLKSTOP_D(), ~D_DD_TOP_HCKDISPSTP_BIT);
#endif	// CO_ACT_HCLOCK

#ifndef CO_DEBUG_ON_PC
	Palladium_Test_Cmd_Dump_Reg(reg_address, size);
#else // CO_DEBUG_ON_PC
	pt_im_disp_pc_file_write(reg_address, size);
#endif // CO_DEBUG_ON_PC

#ifdef CO_ACT_PCLOCK
	Dd_Top_Stop_Clock(&gIM_DISP_PCLK_Counter, &Dd_Top_Get_CLKSTOP_E(), D_DD_TOP_PCKDISPSTP_BIT);
#endif	// CO_ACT_PCLOCK
#ifdef CO_ACT_HCLOCK
	Dd_Top_Stop_Clock(&gIM_DISP_HCLK_Counter, &Dd_Top_Get_CLKSTOP_D(), D_DD_TOP_HCKDISPSTP_BIT);
#endif	// CO_ACT_HCLOCK
}

// GIC enable
static VOID pt_im_disp_enable_gic( VOID )
{
	Dd_GIC_Ctrl( E_DD_GIC_INTID_DISP_LCD_INT, 1, D_DD_GIC_PRI30, 1 );		//intid=INT_LCD, enable, pri=31, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_DISP_HDMI_INT, 1, D_DD_GIC_PRI30, 1 );		//intid=INT_HIF, enable, pri=31, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_DISP_MIPI_DSI_INT, 1, D_DD_GIC_PRI30, 1 );	//intid=INT_MIF, enable, pri=31, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_MIPI_DSI_Tx_INT, 1, D_DD_GIC_PRI30, 1 );	//intid=INT_DSI, enable, pri=31, cpu=1
	Dd_GIC_Ctrl( E_DD_GIC_INTID_DISP_DWCH_INT, 1, D_DD_GIC_PRI30, 1 );		//intid=INT_DW, enable, pri=31, cpu=1

}

static VOID pt_im_disp_cmd_wrap(ct_Main ct_command_main, CHAR* cmd)
{
	INT32	argc = 0;
	CHAR*	argv[32];
	CHAR	arg[32][32];
	INT32	pos = 0;
	INT32	len = 0;
	INT32	max = strlen(cmd);
	INT32	i;

	printf("CMD> %s\n", cmd);

	// Clear arg buffer
	for(i = 0; i < 32; i++){
		argv[i] = arg[i];
		memset(argv[i], 0, 32);
	}

	// Set arg buffer
	for(i = 0; i < max; i++){
		if(cmd[i] == ' '){
			if(len){
				memcpy(argv[argc], &cmd[pos], len);
				argc++;
				len = 0;
			}
			pos = i + 1;
		}
		else if(i == max - 1){
			memcpy(argv[argc], &cmd[pos], len + 1);
			argc++;
			len = 0;
			pos = i + 1;
		}
		else{
			len++;
		}
	}

	// Execute command
	ct_command_main(argc, argv);
}

// Initialization
static VOID palladium_test_disp_lcd_init(VOID)
{
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0x000F 0 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x0002 0x0002 1");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 1 1");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Init");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMs 3 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTs 3 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 3 1 0x0004 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 3 1 0x0008 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_Main_TBLs 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_Output_TBLs 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 1 0x00010000");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 1 0x00100000");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Wait 1 0x00110000 1");
}

static VOID palladium_test_disp_hdmi_init(VOID)
{
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0x000F 0 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x0002 0x0002 1");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 0 2");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Init");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMs 3 0 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTs 3 0 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 3 0 0x0004 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 3 0 0x0008 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_Main_TBLs 1 0 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_Output_TBLs 1 0 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0 0x00010000");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0 0x00100000");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Wait 0 0x00110000 1");
}

// stop LCD
static VOID palladium_test_disp_lcd_stop(VOID)
{
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x000F 0x000F 1");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 1 0");
}

// stop HDMI
static VOID palladium_test_disp_hdmi_stop(VOID)
{
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x000F 0x000F 1");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 0 0");
}

// Interrupt
static VOID palladium_test_disp_int_VF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00010000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 0 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00010000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00010000 7");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00010000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		if(gDDIM_Info.com._6a == 2){	// Interace only
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 0 1");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 0 2");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
		}
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00010000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00010000 0x00000000");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00010000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 1 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00010000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00010000 7");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 1 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00010000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		if(gDDIM_Info.com._6a == 2){	// Interace only
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1 1");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1 2");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
		}
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00010000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 1 0x00010000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00010000 0x00000000");
	}
}

static VOID palladium_test_disp_int_VF2(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00100000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 0 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00100000 1");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00100000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00100000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00100000 8");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00100000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00100000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		if(gDDIM_Info.com._6a == 2){	// Interace only
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp 0 SetVSN 1");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp 0 SetVSN 2");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
		}
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00100000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00100000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00100000 0x00000000");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00100000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00100000 1");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00100000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00100000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00100000 8");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00100000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00100000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		if(gDDIM_Info.com._6a == 2){	// Interace only
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1 1");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1 2");
			pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
		}
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00100000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00100000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00100000 0x00000000");
	}
}

static VOID palladium_test_disp_int_LEF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00000001 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000001 2");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000001 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "immxic mask master enable set 0x8E 1 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000001;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000001 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000001 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00000001 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000001 2");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000001 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "immxic mask master enable set 0x8E 1 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000001;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000001 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000001 0");
	}
}

static VOID palladium_test_disp_int_GR0EF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00000010 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000010 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000010 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0004 0x00000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x00040 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000010;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000010 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 x00000010 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00000010 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000010 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000010 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0004 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000010;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000010 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000010 0");
	}
}

static VOID palladium_test_disp_int_GR1EF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00000020 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000020 4");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000020 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0008 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000020;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000020 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000020 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00000020 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000020 4");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000020 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0008 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000020;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000020 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000020 0");
	}
}

static VOID palladium_test_disp_int_LREF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00000100 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000100 5");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000100 0");
		pt_im_disp_cmd_wrap(Ct_Im_MXIC_Main, "immxic mask master enable set 0x8E 1 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000100;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000100 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000100 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00000100 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000100 5");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000100 0");
		pt_im_disp_cmd_wrap(Ct_Im_MXIC_Main, "immxic mask master enable set 0x8E 1 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000100;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000100 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000100 0");
	}
}

static VOID palladium_test_disp_int_GR0REF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00001000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00001000 6");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00001000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0004 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00001000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00001000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00001000 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00001000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00001000 6");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00001000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0004 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00001000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00001000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00001000 0");
	}
}

static VOID palladium_test_disp_int_GR1REF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00002000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00002000 7");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00002000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0008 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00002000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00002000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00002000 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00002000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00002000 7");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00002000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0008 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00002000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00002000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00002000 0");
	}
}

static VOID palladium_test_disp_int_GA0REF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00004000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00004000 6");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00004000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0004 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00004000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00004000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00004000 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00004000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00004000 6");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00004000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0004 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00004000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00004000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00004000 0");
	}
}

static VOID palladium_test_disp_int_GA1REF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00008000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00008000 7");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00008000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0008 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00008000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00008000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00008000 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00008000 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00008000 7");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00008000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0008 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00008000;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00008000 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00008000 0");
	}
}

static VOID palladium_test_disp_int_GA0EF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00000040 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000040 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000040 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0004 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0004 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000040;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000040 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000040 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00000040 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000040 3");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000040 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0004 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0004 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0004");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000040;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000040 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000040 0");
	}
}

static VOID palladium_test_disp_int_GA1EF(E_IM_DISP_SEL block)
{
	if (block == E_IM_DISP_HDMI) {
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0 0x00000080 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000080 4");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000080 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0008 0x0000 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0008 0x0000");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000080;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0 0x00000080 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0 0x00000080 0");
	}
	else {	// LCD
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 1 0x00000080 0x00113131");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00113131 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000080 4");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000080 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x0000 0x0008 0");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 1 0x0008 0x1ff");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0000 0x0008");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 200");
#ifdef CO_DEBUG_ON_PC
		IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000080;
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 1 0x00000080 9");
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 1 0x00000080 0");
	}
}

static VOID palladium_disp_unit_test(E_IM_DISP_SEL block)
{
	Ddim_Print(("*** [CT] 01-01-002 : Im_DISP_Ctrl_Main_Layer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMs 1 0");	// D_CT_IM_DISP_FIRST_PARA_ONLY, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMs 2 0");	// D_CT_IM_DISP_SECOND_PARA_ONLY, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMs 3 0");	// D_CT_IM_DISP_BOTH_PARA, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-004 : Im_DISP_Get_Ctrl_Main_Layer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMg 1");	// D_CT_IM_DISP_FIRST_PARA_ONLY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMg 2");	// D_CT_IM_DISP_SECOND_PARA_ONLY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlMg 3");	// D_CT_IM_DISP_BOTH_PARA

	Ddim_Print(("*** [CT] 01-01-006 : Im_DISP_Ctrl_Output() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	if(gDDIM_Info.com._6a == 1){
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTs 1 0 0");	// D_CT_IM_DISP_FIRST_PARA_ONLY, LCD setting, Normal
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTs 3 0 0");	// D_CT_IM_DISP_BOTH_PARA, LCD setting, Normal
	}
	else{
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTs 1 1 0");	// D_CT_IM_DISP_FIRST_PARA_ONLY, HDMI setting, Normal
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTs 2 1 0");	// D_CT_IM_DISP_SECOND_PARA_ONLY, HDMI setting, Normal
	}
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-008 : Im_DISP_Get_Ctrl_Output() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTg 1");	// D_CT_IM_DISP_FIRST_PARA_ONLY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTg 2");	// D_CT_IM_DISP_SECOND_PARA_ONLY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlOUTg 3");	// D_CT_IM_DISP_BOTH_PARA

	Ddim_Print(("*** [CT] 01-01-010 : Im_DISP_Ctrl_Grid_Layer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlGRIDs 0 0");	// Normal, Normal

	Ddim_Print(("*** [CT] 01-01-012 : Im_DISP_Get_Ctrl_Grid_Layer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp CtrlGRIDg 0");		// Normal

	Ddim_Print(("*** [CT] 01-01-014 : Im_DISP_Ctrl_OSD_Layer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 0x0004 1 0");	// E_IM_DISP_SEL_LAYER_OSD_0, D_CT_IM_DISP_FIRST_PARA_ONLY, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 0x0004 2 0");	// E_IM_DISP_SEL_LAYER_OSD_0, D_CT_IM_DISP_SECOND_PARA_ONLY, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDs 0x0008 3 0");	// E_IM_DISP_SEL_LAYER_OSD_1, D_CT_IM_DISP_BOTH_PARA, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-016 : Im_DISP_Get_Ctrl_OSD_Layer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDg 0x0004 1");	// E_IM_DISP_SEL_LAYER_OSD_0, D_CT_IM_DISP_FIRST_PARA_ONLY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDg 0x0004 2");	// E_IM_DISP_SEL_LAYER_OSD_0, D_CT_IM_DISP_SECOND_PARA_ONLY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_OSDg 0x0008 3");	// E_IM_DISP_SEL_LAYER_OSD_1, D_CT_IM_DISP_BOTH_PARA

	Ddim_Print(("*** [CT] 01-01-018 : Im_DISP_Ctrl_Tbl() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_TBLs 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_TBLs 0x000D 0");	// E_IM_DISP_SEL_LAYER_MAIN | E_IM_DISP_SEL_LAYER_OSD_ALL, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-020 : Im_DISP_Get_Ctrl_Tbl() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_TBLg 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_TBLg 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Ctrl_TBLg 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-022 : Im_DISP_Start() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0001");	// E_IM_DISP_SEL_LAYER_MAIN
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0002");	// E_IM_DISP_SEL_LAYER_DCORE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0004");	// E_IM_DISP_SEL_LAYER_OSD_0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x0008");	// E_IM_DISP_SEL_LAYER_OSD_1
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000C");	// E_IM_DISP_SEL_LAYER_OSD_ALL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000C 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000C");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");	// E_IM_DISP_SEL_LAYER_ALL

	Ddim_Print(("*** [CT] 01-01-026 : Im_DISP_Get_Status() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp status 0");	// Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp status 0");	// Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-028 : Im_DISP_Wait_Vsync() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0xFFFFFFFF 0x00113131");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Wait 0x00010000 1");	// E_IM_DISP_INTERRUPTION_SELECT_VE, E_IM_DISP_WAIT_1_TIME
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Wait 0x00100000 1");	// E_IM_DISP_INTERRUPTION_SELECT_VE2, E_IM_DISP_WAIT_1_TIME
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Wait 0x00110000 2");	// E_IM_DISP_CORRECT_SELECT_ALL_VE, E_IM_DISP_WAIT_2_TIME

	Ddim_Print(("*** [CT] 01-01-030 : Im_DISP_Wait_Stop() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0001");	// E_IM_DISP_SEL_LAYER_MAIN
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0002");	// E_IM_DISP_SEL_LAYER_DCORE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0004");	// E_IM_DISP_SEL_LAYER_OSD_0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x0008");	// E_IM_DISP_SEL_LAYER_OSD_1
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000C");	// E_IM_DISP_SEL_LAYER_OSD_ALL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");	// E_IM_DISP_SEL_LAYER_ALL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("*** [CT] 01-01-032 : Im_DISP_Int_Handler()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00113131 9");	// E_IM_DISP_CORRECT_SELECT_ALL, NULL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000001 0");	// E_IM_DISP_INTERRUPTION_SELECT_LEE, int_callback00
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000010 1");	// E_IM_DISP_INTERRUPTION_SELECT_GR0EE, int_callback01
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000020 2");	// E_IM_DISP_INTERRUPTION_SELECT_GR1EE, int_callback02
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000100 3");	// E_IM_DISP_INTERRUPTION_SELECT_LREE, int_callback03
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00001000 4");	// E_IM_DISP_INTERRUPTION_SELECT_GR0REE, int_callback04
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00002000 5");	// E_IM_DISP_INTERRUPTION_SELECT_GR1REE, int_callback05
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00010000 6");	// E_IM_DISP_INTERRUPTION_SELECT_VE, int_callback06
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00100000 7");	// E_IM_DISP_INTERRUPTION_SELECT_VE2, int_callback07

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000001;	// E_IM_DISP_INTERRUPTION_SELECT_LEE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000010;	// E_IM_DISP_INTERRUPTION_SELECT_GR0EE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000020;	// E_IM_DISP_INTERRUPTION_SELECT_GR1EE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000100;	// E_IM_DISP_INTERRUPTION_SELECT_LREE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00001000;	// E_IM_DISP_INTERRUPTION_SELECT_GR0REE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00002000;	// E_IM_DISP_INTERRUPTION_SELECT_GR1REE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00010000;	// E_IM_DISP_INTERRUPTION_SELECT_VE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00100000;	// E_IM_DISP_INTERRUPTION_SELECT_VE2
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00008000;	// E_IM_DISP_INTERRUPTION_SELECT_GA1REE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00004000;	// E_IM_DISP_INTERRUPTION_SELECT_GA0REE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000080;	// E_IM_DISP_INTERRUPTION_SELECT_GA1EE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00000040;	// E_IM_DISP_INTERRUPTION_SELECT_GA0EE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");

	IO_DISP.MAIN[block].DCORE.INTF.word = 0x00110000;
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp IntHndl");
#endif	// CO_DEBUG_ON_PC

	Ddim_Print(("*** [CT] 01-01-037 : Im_DISP_Set_Parameter_Reflect_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRefTiming 1");	// E_IM_DISP_RPGTMG_VSYNC_VSYNC
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRefTiming 0");	// E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-039 : Im_DISP_Get_Parameter_Reflect_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetRefTiming 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-041 : Im_DISP_Set_Input_Data_Transfer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetITrans 0x00000000");	// IFMT = D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetITrans 0x00000004");	// IFMT = D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetITrans 0x117FF31C");	// IFMT = D_IM_DISP_LIDT_IFMT_RIBERY

	Ddim_Print(("*** [CT] 01-01-043 : Im_DISP_Get_Input_Data_Transfer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetITrans 0");		// Normal

	Ddim_Print(("*** [CT] 01-01-045 : Im_DISP_Set_Input_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetISize 0x00100080");	// U_IM_DISP_SIZE width=128 lines=16
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetISize 0x3FFEFFFE");	// U_IM_DISP_SIZE width=65534 lines=16382

	Ddim_Print(("*** [CT] 01-01-047 : Im_DISP_Get_Input_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetISize 0");			// Normal

	Ddim_Print(("*** [CT] 01-01-049 : Im_DISP_Set_Addr() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetAdd 0 0xAFFFFFFF 0xBFFFFFFF");	// E_IM_DISP_BANK_00, y_addr, c_addr
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetAdd 1 0xCFFFFFFF 0xDFFFFFFF");	// E_IM_DISP_BANK_01, y_addr, c_addr
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetAdd 2 0xEFFFFFFF 0xFFFFFFFF");	// E_IM_DISP_BANK_10, y_addr, c_addr
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetAdd 3 0x1FFFFFFF 0x2FFFFFFF");	// E_IM_DISP_BANK_11, y_addr, c_addr

	Ddim_Print(("*** [CT] 01-01-051 : Im_DISP_Get_Addr() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAdd 0");	// E_IM_DISP_BANK_00
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAdd 1");	// E_IM_DISP_BANK_01
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAdd 2");	// E_IM_DISP_BANK_10
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAdd 3");	// E_IM_DISP_BANK_11

	Ddim_Print(("*** [CT] 01-01-053 : Im_DISP_Set_Lfd() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetLfd 2 2");	// LYHGA, LCHGA
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetLfd 65534 65534");	// LYHGA, LCHGA

	Ddim_Print(("*** [CT] 01-01-055 : Im_DISP_Get_Lfd() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetLfd 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-057 : Im_DISP_Set_Main_Bank()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMBank 0");	// E_IM_DISP_BANK_00
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMBank 1");	// E_IM_DISP_BANK_01
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMBank 2");	// E_IM_DISP_BANK_10
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMBank 3");	// E_IM_DISP_BANK_11

	Ddim_Print(("*** [CT] 01-01-058 : Im_DISP_Get_Main_Bank() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetMBank 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-060 : Im_DISP_Get_Main_Bank_Monitor() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetMBkMon 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-062 : Im_DISP_Set_Error_Auto_Recovery()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetEAR 0");	// E_IM_DISP_ERCV_OFF
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetEAR 1");	// E_IM_DISP_ERCV_ON

	Ddim_Print(("*** [CT] 01-01-063 : Im_DISP_Get_Error_Auto_Recovery() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetEAR 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-065 : Im_DISP_Set_Resize() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRes 0 0x00001E0F 0x00000201 0");	// E_IM_DISP_HRSZSL_PADDING_THINNING, HRSZ1=15/30, VRSZ=1/2, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRes 1 0x17000318 0x00000108 0");	// E_IM_DISP_HRSZSL_LINEAR, HRSZ1=24/3, VRSZ=8/1, Normal

	Ddim_Print(("*** [CT] 01-01-067 : Im_DISP_Get_Resize() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetRes 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-069 : Im_DISP_Set_Brightness_Warning() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBriWrng 0x00010002 0xFFFF0001 0x00010001 0xFFFF0001 0x00010001");	// LYWTH, LYWHS0, LYWHS1, LYWLS0, LYWLS1
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBriWrng 0xFFFEFFFF 0x00000000 0xFFFFFFFF 0x00000000 0xFFFFFFFF");	// LYWTH, LYWHS0, LYWHS1, LYWLS0, LYWLS1

	Ddim_Print(("*** [CT] 01-01-071 : Im_DISP_Get_Brightness_Warning() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetBriWrng 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-073 : Im_DISP_Set_Blink_Timer()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBTim 0x00000000");	// LBLTMR
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBTim 0x00003F3F");	// LBLTMR

	Ddim_Print(("*** [CT] 01-01-074 : Im_DISP_Get_Blink_Timer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetBTim 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-076 : Im_DISP_Set_Data_Range() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRng 0 0x0000 -32 0x00 -16 0x00 -16 0");	// D_IM_DISP_ENABLE_OFF, DREYGAIN, DREYOFS, DRECBGAIN, DRECBOFS, DRECRGAIN, DRECROFS, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRng 1 0x0100 0 0x80 0 0x80 0 0");		// D_IM_DISP_ENABLE_OFF, DREYGAIN, DREYOFS, DRECBGAIN, DRECBOFS, DRECRGAIN, DRECROFS, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRng 1 0x01FF 31 0xFF 15 0xFF 15 0");	// D_IM_DISP_ENABLE_OFF, DREYGAIN, DREYOFS, DRECBGAIN, DRECBOFS, DRECRGAIN, DRECROFS, Normal

	Ddim_Print(("*** [CT] 01-01-078 : Im_DISP_Get_Data_Range() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetRng 0");	// Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRng 0 0x0000 -32 0x00 -16 0x00 -16 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetRng 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-080 : Im_DISP_Set_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMax 0 0x00000000 0x00000000 0x00000000 0");	// E_IM_DISP_MATRIX_KIND_Y2R, COEF, COEF, COEF, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMax 2 0x00FFFFFF 0x00FFFFFF 0x00FFFFFF 0");	// E_IM_DISP_MATRIX_KIND_CC, COEF, COEF, COEF, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMax 0 0x00650040 0x00E2F440 0x00007740 0");	// E_IM_DISP_MATRIX_KIND_Y2R, COEF, COEF, COEF, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetMax 0 0x005A0040 0x00D3EA40 0x00FF7140 0");	// E_IM_DISP_MATRIX_KIND_Y2R, COEF, COEF, COEF, Normal

	Ddim_Print(("*** [CT] 01-01-082 : Im_DISP_Get_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetMax 0 0");	// E_IM_DISP_MATRIX_KIND_Y2R, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetMax 2 0");	// E_IM_DISP_MATRIX_KIND_CC, Normal

	Ddim_Print(("*** [CT] 01-01-084 : Im_DISP_Set_Gamma_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 1 1 0");	// E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 1 1 0");	// E_IM_DISP_CORRECT_SELECT_GRADATION_ADJUST, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 1 1 0");	// E_IM_DISP_CORRECT_SELECT_GAMMA, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 1 0 0");	// E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 1 0 0");	// E_IM_DISP_CORRECT_SELECT_GRADATION_ADJUST, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 1 0 0");	// E_IM_DISP_CORRECT_SELECT_GAMMA, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 0 1 0");	// E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 0 1 0");	// E_IM_DISP_CORRECT_SELECT_GRADATION_ADJUST, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 0 1 0");	// E_IM_DISP_CORRECT_SELECT_GAMMA, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 0 0 0");	// E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 0 0 0");	// E_IM_DISP_CORRECT_SELECT_GRADATION_ADJUST, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 0 0 0");	// E_IM_DISP_CORRECT_SELECT_GAMMA, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-086 : Im_DISP_Get_Gamma_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetGamEn 0");		// Normal

	Ddim_Print(("*** [CT] 01-01-088 : Im_DISP_Set_Tc() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetTc 0x00000000 0x00000000");	// U_IM_DISP_TCYCAL, U_IM_DISP_TCCTL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetTc 0x00FFFFFF 0x1F010102");	// U_IM_DISP_TCYCAL, U_IM_DISP_TCCTL

	Ddim_Print(("*** [CT] 01-01-090 : Im_DISP_Get_Tc() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetTc 0");		// Normal

	Ddim_Print(("*** [CT] 01-01-092 : Im_DISP_Set_Display_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetDspEn 0");	// D_IM_DISP_ENABLE_OFF
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetDspEn 1");	// D_IM_DISP_ENABLE_ON

	Ddim_Print(("*** [CT] 01-01-094 : Im_DISP_Get_Display_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetDspEn 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-096 : Im_DISP_Set_Display_Position() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetPos 0x00000000");	// U_IM_DISP_DSTA
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetPos 0x3FFEFFFE");	// U_IM_DISP_DSTA

	Ddim_Print(("*** [CT] 01-01-098 : Im_DISP_Get_Display_Position() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetPos 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-100 : Im_DISP_Set_Reverse_Display()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRev 0x00000000");	// U_IM_DISP_LREVDISP
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetRev 0x00000101");	// U_IM_DISP_LREVDISP

	Ddim_Print(("*** [CT] 01-01-101 : Im_DISP_Get_Reverse_Display() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetRev 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-107 : Im_DISP_Set_Vsync_Int_Timing()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 0");	// E_IM_DISP_VSYNC_FLAG_NONE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 1");	// E_IM_DISP_VSYNC_FLAG_FIRST
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 2");	// E_IM_DISP_VSYNC_FLAG_SECOND
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetVSN 3");	// E_IM_DISP_VSYNC_FLAG_BOTH

	Ddim_Print(("*** [CT] 01-01-108 : Im_DISP_Get_Vsync_Int_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetVSN 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-110 : Im_DISP_Set_Int() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0x00000001 0x00000000");	// interruption_select, E_IM_DISP_INTERRUPTION_STATE_NONE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetInt 0xFFFFFFFF 0x00113131");	// interruption_select, E_IM_DISP_INTERRUPTION_STATE_SET

	Ddim_Print(("*** [CT] 01-01-112 : Im_DISP_Get_Int() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetInt 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-114 : Im_DISP_Set_Int_Callback() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00113131 9");	// E_IM_DISP_CORRECT_SELECT_ALL, NULL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000001 0");	// E_IM_DISP_INTERRUPTION_SELECT_LEE, int_callback00
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000010 1");	// E_IM_DISP_INTERRUPTION_SELECT_GR0EE, int_callback01
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000020 2");	// E_IM_DISP_INTERRUPTION_SELECT_GR1EE, int_callback02
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00000100 3");	// E_IM_DISP_INTERRUPTION_SELECT_LREE, int_callback03
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00001000 4");	// E_IM_DISP_INTERRUPTION_SELECT_GR0REE, int_callback04
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00002000 5");	// E_IM_DISP_INTERRUPTION_SELECT_GR1REE, int_callback05
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00010000 6");	// E_IM_DISP_INTERRUPTION_SELECT_VE, int_callback06
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetCBK 0x00100000 7");	// E_IM_DISP_INTERRUPTION_SELECT_VE2, int_callback07

	Ddim_Print(("*** [CT] 01-01-116 : Im_DISP_Get_Int_Callback() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00000001 0");	// E_IM_DISP_INTERRUPTION_SELECT_LEE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00000010 0");	// E_IM_DISP_INTERRUPTION_SELECT_GR0EE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00000020 0");	// E_IM_DISP_INTERRUPTION_SELECT_GR1EE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00000100 0");	// E_IM_DISP_INTERRUPTION_SELECT_LREE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00001000 0");	// E_IM_DISP_INTERRUPTION_SELECT_GR0REE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00002000 0");	// E_IM_DISP_INTERRUPTION_SELECT_GR1REE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00010000 0");	// E_IM_DISP_INTERRUPTION_SELECT_VE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetCBK 0x00100000 0");	// E_IM_DISP_INTERRUPTION_SELECT_VE2, Normal

	Ddim_Print(("*** [CT] 01-01-118 : Im_DISP_Get_AXI_Status() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAxi 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-120 : Im_DISP_Set_Output_Parameter_Reflect_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutRefTiming 0");	// E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutRefTiming 1");	// E_IM_DISP_RPGTMG_VSYNC_VSYNC
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-122 : Im_DISP_Get_Output_Parameter_Reflect_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOutRefTiming 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-124 : Im_DISP_Set_Output_Parameter_Reflect_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutRefEn 0");		// D_IM_DISP_ENABLE_OFF
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutRefEn 1");		// D_IM_DISP_ENABLE_ON

	Ddim_Print(("*** [CT] 01-01-126 : Im_DISP_Get_Output_Parameter_Reflect_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOutRefEn 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-128 : Im_DISP_Set_TSL() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetTSL 0");	// D_IM_DISP_TSL_PROGRESSIVE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetTSL 1");	// D_IM_DISP_TSL_INTERLACE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-130 : Im_DISP_Get_TSL() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetTSL 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-132 : Im_DISP_Get_Output_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOutSize 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-134 : Im_DISP_Set_Force_Out_Data_Enable()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFOE 0");	// E_IM_DISP_FDOEN_ORDINARY_OUT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFOE 1");	// E_IM_DISP_FDOEN_FORCE_DATA_OUT

	Ddim_Print(("*** [CT] 01-01-135 : Im_DISP_Get_Force_Out_Data_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetFOE 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-137 : Im_DISP_Set_Force_Out_Data()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFOD 0x00000000");	// U_IM_DISP_IMAGE_COLOR
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFOD 0x00FFFFFF");	// U_IM_DISP_IMAGE_COLOR

	Ddim_Print(("*** [CT] 01-01-138 : Im_DISP_Get_Force_Out_Data() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetFOD 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-140 : Im_DISP_Set_Color_Bar_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBarSz 120");	// CLBHSIZE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBarSz 65535");	// CLBHSIZE

	Ddim_Print(("*** [CT] 01-01-142 : Im_DISP_Get_Color_Bar_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetBarSz 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-144 : Im_DISP_Set_Color_Bar() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBarCr 0 0");	// Normal, Array size(Max=16)
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBarCr 0 16");	// Normal, Array size(Max=16)

	Ddim_Print(("*** [CT] 01-01-146 : Im_DISP_Get_Color_Bar() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetBarCr 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-148 : Im_DISP_Set_Blend() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBlnd 0x00000123");	// U_IM_DISP_BLDCTL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetBlnd 0x00003210");	// U_IM_DISP_BLDCTL

	Ddim_Print(("*** [CT] 01-01-150 : Im_DISP_Get_Blend() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetBlnd 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-152 : Im_DISP_Set_Output_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutMax 0 0x00FFFFFF 0x00FFFFFF 0x00FFFFFF 0");	// D_IM_DISP_R2REN_DISABLE, matrix, matrix, matrix, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutMax 1 0x00808080 0x00808080 0x00808080 0");	// D_IM_DISP_R2REN_ENABLE, matrix, matrix, matrix, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutMax 0 0x007F7F7F 0x007F7F7F 0x007F7F7F 0");	// D_IM_DISP_R2REN_DISABLE, matrix, matrix, matrix, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutMax 0 0x00042D0D 0x0020E8F9 0x00FEE320 0");	// D_IM_DISP_R2REN_DISABLE, matrix, matrix, matrix, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutMax 1 0x00072613 0x0020EBF5 0x00FBE520 0");	// D_IM_DISP_R2REN_ENABLE, matrix, matrix, matrix, Normal

	Ddim_Print(("*** [CT] 01-01-154 : Im_DISP_Get_Output_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOutMax 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-156 : Im_DISP_Set_Lpf() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetLPF 0");	// D_IM_DISP_YLPF_DISABLE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetLPF 1");	// D_IM_DISP_YLPF_ENABLE

	Ddim_Print(("*** [CT] 01-01-158 : Im_DISP_Get_Lpf() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetLPF 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-160 : Im_DISP_Set_Clip() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetClip 0");	// Normal parameter 0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetClip 1");	// Normal parameter 1

	Ddim_Print(("*** [CT] 01-01-162 : Im_DISP_Get_Clip() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetClip 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-164 : Im_DISP_Set_Output_Order()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutOd 0x00000000");	// DOCTL0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutOd 0x00111111");	// DOCTL0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutOd 0x00222222");	// DOCTL0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOutOd 0x00333333");	// DOCTL0

	Ddim_Print(("*** [CT] 01-01-165 : Im_DISP_Get_Output_Order() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOutOd 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-167 : Im_DISP_Set_Grid() normal\n"));
										// DSTA,      DSTA,      GLENGTH,   GWIDTH,    GITVL,     GNUM,      GDCTL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGrid 0x3FFEFFFE 0x3FFEFFFE 0x3FFEFFFE 0x003E003E 0x3FFEFFFE 0x00070007 0x03FFFFFF");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGrid 0x00000000 0x00000000 0x00080008 0x00020002 0x00040004 0x00000000 0x00000000");

	Ddim_Print(("*** [CT] 01-01-169 : Im_DISP_Get_Grid() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetGrid 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-171 : Im_DISP_Set_Grid_Enable()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGridEn 0");	// E_IM_DISP_GDISPEN_NOT_DISPLAY
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGridEn 1");	// E_IM_DISP_GDISPEN_DISPLAY

	Ddim_Print(("*** [CT] 01-01-172 : Im_DISP_Get_Grid_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetGridEn 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-174 : Im_DISP_Set_Face_Frame() normal\n"));
									// enable, top, cnt, DSTA,     FFSIZE,    FFWIDTH,   FFCLR
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFace 0x0000 0 1 0x00000000 0x00080008 0x00020002 0x00000000");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFace 0xFFFF 15 16 0x3FFEFFFE 0x3FFEFFFE 0x003E003E 0x00FFFFFF");

	Ddim_Print(("*** [CT] 01-01-176 : Im_DISP_Get_Face_Frame() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetFace 0");				// Normal

	Ddim_Print(("*** [CT] 01-01-178 : Im_DISP_Set_Face_Frame_Enable()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFaceEn 0xFFFF 0");	// Frame0-15 enable, Top face frame = 0
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetFaceEn 0x0000 15");	// Frame0-15 disable, Top face frame = 15

	Ddim_Print(("*** [CT] 01-01-179 : Im_DISP_Get_Face_Frame_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetFaceEn 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-181 : Im_DISP_Set_OSD_Parameter_Reflect_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRefTiming 0x0004 1");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_RPGTMG_VSYNC_VSYNC
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRefTiming 0x0008 1");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_RPGTMG_VSYNC_VSYNC
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRefTiming 0x000C 0");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-183 : Im_DISP_Get_OSD_Parameter_Reflect_Timing() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDRefTiming 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDRefTiming 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-185 : Im_DISP_Set_OSD_Input_Data_Transfer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDITrans 0x0004 0x107F0014");	// E_IM_DISP_SEL_LAYER_OSD_0, U_IM_DISP_GRIDT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDITrans 0x0008 0x107F0014");	// E_IM_DISP_SEL_LAYER_OSD_1, U_IM_DISP_GRIDT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDITrans 0x000C 0x00000000");	// E_IM_DISP_SEL_LAYER_OSD_ALL, U_IM_DISP_GRIDT

	Ddim_Print(("*** [CT] 01-01-187 : Im_DISP_Get_OSD_Input_Data_Transfer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDITrans 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDITrans 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-189 : Im_DISP_Set_OSD_Territory_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTrSize 0x0004 0x3FFEFFFE");	// E_IM_DISP_SEL_LAYER_OSD_0, U_IM_DISP_SIZE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTrSize 0x000C 0x00080008");	// E_IM_DISP_SEL_LAYER_OSD_ALL, U_IM_DISP_SIZE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTrSize 0x0008 0x3FFEFFFE");	// E_IM_DISP_SEL_LAYER_OSD_1, U_IM_DISP_SIZE

	Ddim_Print(("*** [CT] 01-01-191 : Im_DISP_Get_OSD_Territory_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDTrSize 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDTrSize 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-193 : Im_DISP_Set_OSD_Territory_Position() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTrPos 0x0004 0x3FFEFFFE");	// E_IM_DISP_SEL_LAYER_OSD_0, U_IM_DISP_DSTA
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTrPos 0x0008 0x3FFEFFFE");	// E_IM_DISP_SEL_LAYER_OSD_1, U_IM_DISP_DSTA
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTrPos 0x000C 0x00000000");	// E_IM_DISP_SEL_LAYER_OSD_ALL, U_IM_DISP_DSTA

	Ddim_Print(("*** [CT] 01-01-195 : Im_DISP_Get_OSD_Territory_Position() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDTrPos 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDTrPos 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-197 : Im_DISP_Set_OSD_Area0_Switch() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaSw 0x0004 1");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_INPUT_DATA_AREA_1
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaSw 0x000C 3");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_INPUT_DATA_AREA_3
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaSw 0x0008 2");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_INPUT_DATA_AREA_2

	Ddim_Print(("*** [CT] 01-01-199 : Im_DISP_Get_OSD_Area0_Switch() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAreaSw 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAreaSw 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-201 : Im_DISP_Get_OSD_Area0_Monitor() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAreaMon 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAreaMon 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-203 : Im_DISP_Set_OSD_Error_Auto_Recovery() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDEAR 0x0004 1");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_ERCV_ON
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDEAR 0x0008 1");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_ERCV_ON
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDEAR 0x000C 0");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_ERCV_OFF

	Ddim_Print(("*** [CT] 01-01-205 : Im_DISP_Get_OSD_Error_Auto_Recovery() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDEAR 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDEAR 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-207 : Im_DISP_Set_OSD_Area_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDASize 0x0004 1 0x00080008");	// E_IM_DISP_SEL_LAYER_OSD_0, count, U_IM_DISP_SIZE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDASize 0x0008 10 0x3FFEFFF8");	// E_IM_DISP_SEL_LAYER_OSD_1, count, U_IM_DISP_SIZE
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDASize 0x000C 10 0x00080008");	// E_IM_DISP_SEL_LAYER_OSD_ALL, count, U_IM_DISP_SIZE

	Ddim_Print(("*** [CT] 01-01-209 : Im_DISP_Get_OSD_Area_Size() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDASize 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDASize 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-211 : Im_DISP_Set_OSD_Area_Addr() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAAddr 0x0004 0 0xFFFFFFF8");		// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_OSD_SA_BANK_NO_0_0, address
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAAddr 0x0008 1 0xFFFFFFF8");		// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_OSD_SA_BANK_NO_0_1, address
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAAddr 0x000C 4 0xFFFFFFF8");		// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_OSD_SA_BANK_NO_1, address
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAAddr 0x000C 12 0xFFFFFFF8");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_OSD_SA_BANK_NO_9, address

	Ddim_Print(("*** [CT] 01-01-213 : Im_DISP_Get_OSD_Area_Addr() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAAddr 0x0004 0 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_OSD_SA_BANK_NO_0_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAAddr 0x0004 1 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_OSD_SA_BANK_NO_0_1, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAAddr 0x0008 4 0");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_OSD_SA_BANK_NO_1, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAAddr 0x0008 12 0");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_OSD_SA_BANK_NO_9, Normal

	Ddim_Print(("*** [CT] 01-01-215 : Im_DISP_Set_OSD_Lfd() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDALfd 0x0004 0 8");		// E_IM_DISP_SEL_LAYER_OSD_0, bank_no, width
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDALfd 0x000C 9 0xFFF8");	// E_IM_DISP_SEL_LAYER_OSD_ALL, bank_no, width
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDALfd 0x0008 1 8");		// E_IM_DISP_SEL_LAYER_OSD_1, bank_no, width

	Ddim_Print(("*** [CT] 01-01-217 : Im_DISP_Get_OSD_Lfd() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDALfd 0x0004 0 0");	// E_IM_DISP_SEL_LAYER_OSD_0, bank_no, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDALfd 0x0008 9 0");	// E_IM_DISP_SEL_LAYER_OSD_1, bank_no, Normal

	Ddim_Print(("*** [CT] 01-01-219 : Im_DISP_Set_OSD_Display_Position() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDPos 0x0004 0 0x01000200");	// E_IM_DISP_SEL_LAYER_OSD_0, bank_no, U_IM_DISP_DSTA
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDPos 0x000C 9 0x3FFEFFFE");	// E_IM_DISP_SEL_LAYER_OSD_ALL, bank_no, U_IM_DISP_DSTA
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDPos 0x0008 1 0x03000400");	// E_IM_DISP_SEL_LAYER_OSD_1, bank_no, U_IM_DISP_DSTA

	Ddim_Print(("*** [CT] 01-01-221 : Im_DISP_Get_OSD_Display_Position() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDPos 0x0004 0 0");	// E_IM_DISP_SEL_LAYER_OSD_0, bank_no, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDPos 0x0008 9 0");	// E_IM_DISP_SEL_LAYER_OSD_1, bank_no, Normal

	Ddim_Print(("*** [CT] 01-01-223 : Im_DISP_Set_OSD_IPO() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIPO 0x0004 0x00010203");	// E_IM_DISP_SEL_LAYER_OSD_0, U_IM_DISP_GRIPO
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIPO 0x0008 0x03000102");	// E_IM_DISP_SEL_LAYER_OSD_1, U_IM_DISP_GRIPO

	Ddim_Print(("*** [CT] 01-01-225 : Im_DISP_Get_OSD_IPO() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetIPO 0x0004 0");		// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetIPO 0x0008 0");		// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-227 : Im_DISP_Set_OSD_Area_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0x0004 0x0000");	// E_IM_DISP_SEL_LAYER_OSD_0, AreaBit
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0x000C 0x03FF");	// E_IM_DISP_SEL_LAYER_OSD_ALL, AreaBit
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAreaEn 0x0008 0x0001");	// E_IM_DISP_SEL_LAYER_OSD_1, AreaBit

	Ddim_Print(("*** [CT] 01-01-229 : Im_DISP_Get_OSD_Area_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAreaEn 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAreaEn 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-231 : Im_DISP_Set_OSD_Blink() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDBlnk 0x0004 0x0000");	// E_IM_DISP_SEL_LAYER_OSD_0, AreaBit
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDBlnk 0x000C 0x03FF");	// E_IM_DISP_SEL_LAYER_OSD_ALL, AreaBit
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDBlnk 0x0008 0x0001");	// E_IM_DISP_SEL_LAYER_OSD_1, AreaBit

	Ddim_Print(("*** [CT] 01-01-233 : Im_DISP_Get_OSD_Blink() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDBlnk 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDBlnk 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-235 : Im_DISP_Set_OSD_Blink_Timer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDBTim 0x0004 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000");	// E_IM_DISP_SEL_LAYER_OSD_0, GRBLINK[0-4]
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDBTim 0x000C 0x3F3F3F3F 0x3F3F3F3F 0x3F3F3F3F 0x3F3F3F3F 0x3F3F3F3F");	// E_IM_DISP_SEL_LAYER_OSD_ALL, GRBLINK[0-4]
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDBTim 0x0008 0x04030201 0x08070605 0x0C0B0A09 0x100F0E0D 0x14131211");	// E_IM_DISP_SEL_LAYER_OSD_1, GRBLINK[0-4]

	Ddim_Print(("*** [CT] 01-01-237 : Im_DISP_Get_OSD_Blink_Timer() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDBTim 0x0004 0");		// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDBTim 0x0008 0");		// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-239 : Im_DISP_Set_OSD_Resize() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRes 0x0004 0 0x00000101 0x00000101 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_HRSZSL_PADDING_THINNING, HRSZ1, VRSZ, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRes 0x000C 1 0x1E001E1F 0x00001F1F 0");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_HRSZSL_LINEAR, HRSZ1, VRSZ, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRes 0x0008 1 0x0A000B0C 0x00000D0E 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_HRSZSL_PADDING_THINNING, HRSZ1, VRSZ, Normal

	Ddim_Print(("*** [CT] 01-01-241 : Im_DISP_Get_OSD_Resize() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDRes 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDRes 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-243 : Im_DISP_Set_OSD_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDMax 0x0004 0 0x00000000 0x00000000 0x00000000");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_MATRIX_KIND_Y2R, matrix, matrix, matrix
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDMax 0x000C 0 0x00FFFFFF 0x00FFFFFF 0x00FFFFFF");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_MATRIX_KIND_Y2R, matrix, matrix, matrix
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDMax 0x0004 0 0x005A0040 0x00D3EA40 0x00FF7140");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_MATRIX_KIND_Y2R, matrix, matrix, matrix
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDMax 0x000C 2 0x00042D0D 0x0020E8F9 0x00FEE320");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_MATRIX_KIND_CC, matrix, matrix, matrix
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDMax 0x0008 2 0x00650040 0x00E2F440 0x00007740");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_MATRIX_KIND_CC, matrix, matrix, matrix

	Ddim_Print(("*** [CT] 01-01-245 : Im_DISP_Get_OSD_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDMax 0x0004 0 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_MATRIX_KIND_Y2R, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDMax 0x0008 2 0");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_MATRIX_KIND_CC, Normal

	Ddim_Print(("*** [CT] 01-01-247 : Im_DISP_Set_OSD_Data_Range() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRng 0x0004 0 0x0000 -32 0x00 -16 0x00 -16");	// E_IM_DISP_SEL_LAYER_OSD_0, D_IM_DISP_ENABLE_OFF, DREYGAIN, DREYOFS, DRECBGAIN, DRECBOFS, DRECRGAIN, DRECROFS
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRng 0x000C 1 0x01FF 31 0xFF 15 0xFF 15");	// E_IM_DISP_SEL_LAYER_OSD_ALL, D_IM_DISP_ENABLE_ON, DREYGAIN, DREYOFS, DRECBGAIN, DRECBOFS, DRECRGAIN, DRECROFS
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDRng 0x0008 1 0x0000 -32 0x00 -16 0x00 -16");	// E_IM_DISP_SEL_LAYER_OSD_1, D_IM_DISP_ENABLE_ON, DREYGAIN, DREYOFS, DRECBGAIN, DRECBOFS, DRECRGAIN, DRECROFS

	Ddim_Print(("*** [CT] 01-01-249 : Im_DISP_Get_OSD_Data_Range() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDRng 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDRng 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-251 : Im_DISP_Set_OSD_Gamma_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x0004 1 0 0 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 2 1 1 0");	// E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_CORRECT_SELECT_GRADATION_ADJUST, E_IM_DISP_TABLE_ACCESS_ENABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x0004 1 0 1 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_ENABLE, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x0008 2 0 0 0");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_CORRECT_SELECT_GRADATION_ADJUST, E_IM_DISP_TABLE_ACCESS_DISABLE, E_IM_DISP_CORRECT_PROC_DISABLE, Normal

	Ddim_Print(("*** [CT] 01-01-253 : Im_DISP_Get_OSD_Gamma_Enable() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDGamEn 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDGamEn 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-255 : Im_DISP_Set_OSD_Tc() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTc 0x0004 0x00000000 0x00000000");	// E_IM_DISP_SEL_LAYER_OSD_0, U_IM_DISP_TCYCAL, U_IM_DISP_TCCTL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTc 0x000C 0x00FFFFFF 0x1F010102");	// E_IM_DISP_SEL_LAYER_OSD_ALL, U_IM_DISP_TCYCAL, U_IM_DISP_TCCTL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDTc 0x0008 0x001D964D 0x00000001");	// E_IM_DISP_SEL_LAYER_OSD_1, U_IM_DISP_TCYCAL, U_IM_DISP_TCCTL

	Ddim_Print(("*** [CT] 01-01-257 : Im_DISP_Get_OSD_Tc() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDTc 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDTc 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-259 : Im_DISP_Set_OSD_Alpha() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAph 0x0004 0x0000");	// E_IM_DISP_SEL_LAYER_OSD_0, alpha
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAph 0x000C 0x00FF");	// E_IM_DISP_SEL_LAYER_OSD_ALL, alpha
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDAph 0x0008 0x0001");	// E_IM_DISP_SEL_LAYER_OSD_0, alpha

	Ddim_Print(("*** [CT] 01-01-261 : Im_DISP_Get_OSD_Alpha() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAph 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetOSDAph 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal

	Ddim_Print(("*** [CT] 01-01-263 : Im_DISP_Set_Anti_Gamma_Table() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetAnTbl 0x0001 0 0");	// E_IM_DISP_SEL_LAYER_MAIN, LCH, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetAnTbl 0x000D 1 0");	// E_IM_DISP_SEL_LAYER_MAIN | E_IM_DISP_SEL_LAYER_OSD_ALL, GRCH, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-265 : Im_DISP_Get_Anti_Gamma_Table() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 1 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAnTbl 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetAnTbl 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-267 : Im_DISP_Set_Tc_Table() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetTcTbl 0x0001 0 0");	// E_IM_DISP_SEL_LAYER_MAIN, LCH, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetTcTbl 0x000D 1 0");	// E_IM_DISP_SEL_LAYER_MAIN | E_IM_DISP_SEL_LAYER_OSD_ALL, GRCH, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-269 : Im_DISP_Get_Tc_Table() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 2 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetTcTbl 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetTcTbl 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-271 : Im_DISP_Set_Gamma_Table() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamTbl 0x0001 0 0");	// E_IM_DISP_SEL_LAYER_MAIN, LCH, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamTbl 0x000D 1 0");	// E_IM_DISP_SEL_LAYER_MAIN | E_IM_DISP_SEL_LAYER_OSD_ALL, GRCH, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-273 : Im_DISP_Get_Gamma_Table() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetOSDGamEn 0x000C 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetGamEn 4 1 1 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetGamTbl 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetGamTbl 0x0008 0");	// E_IM_DISP_SEL_LAYER_OSD_1, Normal
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");

	Ddim_Print(("*** [CT] 01-01-275 : Im_DISP_Change_CSC_Matrix() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgCsC 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, E_IM_DISP_CC_MATRIX_NOCONVERT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgCsC 0x0002 1");	// E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_CC_MATRIX_709
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgCsC 0x0004 0");	// E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_CC_MATRIX_NOCONVERT
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgCsC 0x0008 1");	// E_IM_DISP_SEL_LAYER_OSD_1, E_IM_DISP_CC_MATRIX_709
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgCsC 0x000F 0");	// E_IM_DISP_SEL_LAYER_ALL, E_IM_DISP_CC_MATRIX_NOCONVERT

	Ddim_Print(("*** [CT] 01-01-277 : Im_DISP_Change_BB_Color()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgBBCol 0x00000000");	// U_IM_DISP_IMAGE_COLOR
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgBBCol 0x00FFFFFF");	// U_IM_DISP_IMAGE_COLOR

	Ddim_Print(("*** [CT] 01-01-278 : Im_DISP_Check_BB_Color() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChkBBCol 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-280 : Im_DISP_Change_BB_Color_Enable()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgBBEn 1");	// 1:Black back on
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgBBEn 0");	// 0:Black back off

	Ddim_Print(("*** [CT] 01-01-281 : Im_DISP_Change_Grid() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgGrid 0 0");	// horizontal grid line, vertical grid line
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp ChgGrid 7 7");	// horizontal grid line, vertical grid line

	Ddim_Print(("*** [CT] 01-01-283 : Im_DISP_Draw_Face_Frame_Single() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp DrawFF 0 0 0x00000000 0x00080008 0x00020002 0x00000000");	// index, D_IM_DISP_ENABLE_OFF, DSTA, FFSIZE, FFWIDTH, FFCLR
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp DrawFF 15 1 0x3FFEFFFE 0x3FFEFFFE 0x003E003E 0x00FFFFFF");	// index, D_IM_DISP_ENABLE_ON, DSTA, FFSIZE, FFWIDTH, FFCLR

	Ddim_Print(("*** [CT] 01-01-033 : Im_DISP_Set_SR() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x0001 0");	// E_IM_DISP_SEL_LAYER_MAIN, D_IM_DISP_SR_CANCEL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x000F 1");	// E_IM_DISP_SEL_LAYER_ALL, D_IM_DISP_SR_RESET
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp start 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x000F 0");	// E_IM_DISP_SEL_LAYER_ALL, D_IM_DISP_SR_CANCEL
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp stop 0x000F 0");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp WaitStop 0x000F");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetSR 0x000F 1");	// E_IM_DISP_SEL_LAYER_ALL, D_IM_DISP_SR_RESET

	Ddim_Print(("*** [CT] 01-01-035 : Im_DISP_Get_SR() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetSR 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-103 : Im_DISP_Set_Display_Interface() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp delay 1");
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 0");	// D_IM_DISP_IFS_NONE
	if(gDDIM_Info.com._6a == 1){
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 1");	// D_IM_DISP_IFS_LCD
	}
	else{
		pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp SetIf 2");	// D_IM_DISP_IFS_HDMI
	}

	Ddim_Print(("*** [CT] 01-01-105 : Im_DISP_Get_Display_Interface() normal\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp GetIf 0");	// Normal

	Ddim_Print(("*** [CT] 01-01-001 : Im_DISP_Init()\n"));
	pt_im_disp_cmd_wrap(Ct_Im_DISP_Main, "imdisp Init");
}


// Main
VOID Pt_Im_DISP_Main( VOID )
{
	UCHAR	type, item;
	E_IM_DISP_SEL	block = E_IM_DISP_HDMI;

	// get test parameter
	type = gDDIM_Info.com._6a;
	item = gDDIM_Info.com._6b;

	// Set can be interrupt
	pt_im_disp_enable_gic();

	// Initialize Timer
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// Start Timer
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

//#ifdef CO_DEBUG_ON_PC
//	IO_DISP.MAIN.LCH.LTRG.word = 2;
//	IO_DISP.MAIN.DCORE.TRG.word = 2;
//	IO_DISP.MAIN.GRCH[0].GRTRG.word = 2;
//	IO_DISP.MAIN.GRCH[1].GRTRG.word = 2;
//#endif	// CO_DEBUG_ON_PC

	Dd_Top_Set_CLKSTOP3_LCDCK(2);
	Dd_Top_Set_CLKSTOP3_HIFCK(2);
	Dd_Top_Set_CLKSTOP3_MIFCK(2);
	Dd_Top_Set_CLKSTOP3_DISPAP(2);
	Dd_Top_Set_CLKSTOP3_DISPAH(2);
	Dd_Top_Set_CLKSTOP3_DISPAX(2);

	// Start
	switch (type) {
		case 1 :	// LCD
			Ddim_Print((gPT_IM_DISP_TEST_str, 2));
			block = E_IM_DISP_LCD_MIPI;
			palladium_test_disp_lcd_init();		// 01-01-001
			break;
		case 2 :	// HDMI
			Ddim_Print((gPT_IM_DISP_TEST_str, 1));
			block = E_IM_DISP_HDMI;
			palladium_test_disp_hdmi_init();	// 01-01-002
			break;
		default:
			break;
	}
	pt_im_disp_Dump_Reg((ULONG)(&IO_DISP), sizeof(IO_DISP));	// Dump IO_DISP register

	switch (item) {
		case 1 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 4 + type));
			palladium_test_disp_int_VF(block);		// 01-01-005,006
			break;
		case 2 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 6 + type));
			palladium_test_disp_int_VF2(block);		// 01-01-007,008
			break;
		case 3 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 8 + type));
			palladium_test_disp_int_LEF(block);		// 01-01-009,010
			break;
		case 4 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 10 + type));
			palladium_test_disp_int_GR0EF(block);	// 01-01-011,012
			break;
		case 5 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 12 + type));
			palladium_test_disp_int_GR1EF(block);	// 01-01-013,014
			break;
		case 6 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 14 + type));
			palladium_test_disp_int_LREF(block);	// 01-01-015,016
			break;
		case 7 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 16 + type));
			palladium_test_disp_int_GR0REF(block);	// 01-01-017,018
			break;
		case 8 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 18 + type));
			palladium_test_disp_int_GR1REF(block);	// 01-01-019,020
			break;
		case 9 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 20 + type));
			palladium_test_disp_int_GA0REF(block);	// 01-01-021,022
			break;
		case 10 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 22 + type));
			palladium_test_disp_int_GA1REF(block);	// 01-01-023,024
			break;
		case 11 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 24 + type));
			palladium_test_disp_int_GA0EF(block);	// 01-01-025,026
			break;
		case 12 :
			Ddim_Print((gPT_IM_DISP_TEST_str, 26 + type));
			palladium_test_disp_int_GA1EF(block);	// 01-01-027,028
			break;
		case 255 :
			palladium_disp_unit_test(block);
			break;
		default:
			break;
	}
	pt_im_disp_Dump_Reg((ULONG)(&IO_DISP), sizeof(IO_DISP));	// Dump IO_DISP register

	// Stop
	Ddim_Print((gPT_IM_DISP_TEST_str, 2 + type));
	switch (type) {
		case 1 :	// HDMI
			palladium_test_disp_lcd_stop();		// 01-01-003
			break;
		case 2 :	// LCD
			palladium_test_disp_hdmi_stop();	// 01-01-004
			break;
		default:
			break;
	}
	pt_im_disp_Dump_Reg((ULONG)(&IO_DISP), sizeof(IO_DISP));	// Dump IO_DISP register

	// Stop Timer
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// Log Output
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	// Force stop
	Palladium_Force_Stop();
}

