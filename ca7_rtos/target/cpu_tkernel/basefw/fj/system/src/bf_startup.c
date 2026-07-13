/**
 * @file		bf_startup.c
 * @brief		bf_startup process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "os_user_custom.h"
#include "bf_startup.h"
#include "fj_boot.h"
#include "fj_mode.h"
#include "debug.h"
#include "color_bar.h"
#if 1	// Disp temp
#include "LCDCtrl.h"
#endif	// Disp temp
#include "bf_startup.h"
#include "fj_filesystem.h"
#include "fj_host.h"
#include "fs_if.h"
#include "sdram_map_play.h"
// delete_basefw\fj\iq
//#include "iq_boot.h"
#include "fj_iq.h"
#include "mba_if.h"

#include "NF_driver_wrapper.h"
#include "uart.h"

#include "bf_compile_info_build.h"
#include "bf_compile_info_update.h"

#include "dd_top.h"
#include "im_img.h"
#include "im_pro.h"
#include "im_mxic.h"
#include "im_jpeg.h"
#include "im_xch.h"
#include "im_b2r.h"
#include "im_r2y.h"
#include "im_ela.h"
#include "im_iip.h"
#include "im_cnr.h"
#include "im_disp.h"
#include "im_rdma.h"
#include "im_shdr.h"

#include "assertion.h"
#include "exception.h"
#include "fj_emergency_log.h"
#ifdef CO_USB_MASS_STORAGE_ON	//For USB Mass Storage(No filesystem. Media driver only)
#include "fs_controller.h"
#include "media_drv_wrapper.h"
#endif	// CO_USB_MASS_STORAGE_ON

#include "osd_attr.h"
#include "osd_config_comm.h"
#include "camera_config.h"

// delete basefw\fj\imgproc\still
//#include "still_pro_ctrl.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_STARTUP_INIT_DISPLAY_NTSC (0)
#define D_BF_STARTUP_INIT_DISPLAY_PAL (1)
#if 1	// Disp temp
#define FJ_LCDBL_CTRL_OFF (0)
#define FJ_LCDBL_CTRL_ON (1)
#endif	// Disp temp

#define D_BF_STARTUP_TEST_IMAGE_AREA		(SDRAM_ADR_LCD_MAIN_2)
//#define D_BF_STARTUP_TEST_IMAGE_AREA		(SDRAM_ADR_PLAY_IMAGE_LOAD_END)
//#define D_BF_STARTUP_TEST_IMAGE_AREA		(0x81F00000)
#define D_BF_STARTUP_TEST_IMAGE_AREA_CB		(D_BF_STARTUP_TEST_IMAGE_AREA + 640 * 480)
#define D_BF_STARTUP_TEST_IMAGE_AREA_CR		(D_BF_STARTUP_TEST_IMAGE_AREA + 640 * 480 + 320 * 240)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID BF_Startup_Init_BaseFW(VOID)
{
	BF_Host_Init_Message( SID_HOST_MSG_COUNT);
}

VOID BF_Initialize_Image_Master_Module( VOID )
{
	Im_IMG_Init();
	Im_PRO_SENTOP_Init();
	Im_PRO_SROTOP_Init( E_IM_PRO_BOTH_UNIT );
	Im_PRO_B2BTOP_Init( E_IM_PRO_BOTH_UNIT );
	Im_PRO_STATTOP_Init();
	Im_PRO_PASTOP_Init();
//	Im_PRO_DMIPI_Init( E_IM_PRO_DMIPI_CH0 );
//	Im_PRO_DMIPI_Init( E_IM_PRO_DMIPI_CH1 );
//	Im_PRO_DMIPI_Init( E_IM_PRO_DMIPI_CH2 );
//	Im_PRO_DMIPI_Init( E_IM_PRO_DMIPI_CH3 );
	Im_PRO_CMIPI_Init( E_IM_PRO_CMIPI_CH0 );
	Im_PRO_CMIPI_Init( E_IM_PRO_CMIPI_CH1 );
	Im_PRO_CMIPI_Init( E_IM_PRO_CMIPI_CH2 );
	Im_PRO_CMIPI_Init( E_IM_PRO_CMIPI_CH3 );
	Im_JPEG_Init();
	Im_B2R_Init( D_IM_B2R_PIPE12 );
	Im_R2Y_Init( D_IM_R2Y_PIPE12 );
	Im_IIP_Init();
	Im_CNR_OTF_Init( 2 );
	Im_CNR_OFL_Init( 2 );
	Im_Xch_Init();
	Im_ELA_Init();
	Im_SHDR_Init( FALSE );
	Im_RDMA_Init();

	// delete\basefw\fj\imgproc\still
	//BF_Still_Init_Interrupt_Module();
}

VOID fj_bootmessageprint(VOID)
{
#ifdef UART_CTRL_CH
//	BF_Debug_Change_Term_Clear();
	printf("\n\
*************************************************************\n\
*                                                           *\n\
*   Welcome to SOCIONEXT Software Terminal !!               *\n\
*                                                           *\n\
*   COPYRIGHT(c)   \x1b[31mSOCIONEXT \x1b[0m                               *\n\
*                                                           *\n\
*************************************************************\n");
	printf("OK F/W version: %s: %x.%x.%x\n", BF_Get_OK_Version_str(), BF_Get_OK_MP_Version(), BF_Get_OK_Sub_Version(), BF_Get_OK_Debug_Version());
	printf("... %s build\n", BF_CompileInfoBuild_Get_DataTimeStr() );
	printf("... %s update\n", BF_CompileInfoUpdate_Get_DataTimeStr() );
	printf("F/W type: %s\n" , BF_Get_OK_FW_type());
	printf("... Board type: %s\n" , BF_Get_OK_Board_type());
	printf("... Memory size: %s\n" , BF_Get_OK_Memory_size_type());
	printf("... Sensor type: %s\n" , Camera_Get_SensorType());
//	printf("Just Now %04d/%02d/%02d %02d:%02d:%02d\n", rtc_date.Year, rtc_date.Month, rtc_date.Day, rtc_date.Hour, rtc_date.Minute, rtc_date.Second);
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
	printf("... File System: MDF\n");
#elif defined(CO_FS_USED_PRFILE2)		// --- REMOVE_PRFILE2 ELSEIF ---
	printf("... File System: PrFILE2\n");
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
	printf("\n** description of indications **\n");
	printf("I:taskid:mode: This is information message.\n");
	printf("\x1b[43;30mW\x1b[0m:taskid:mode: This is warning message.\n");
	printf("\x1b[41;30mE\x1b[0m:taskid:mode: This is error message.\n");
	printf("I:taskid:mode: \x1b[4mMT=777(ms)\x1b[0m: This is measured time message.\n");
	printf("** ** **\n\n");

	printf("booting ...\n");
	//	Dd_clk_print();
	//	Dd_Interrupt_Display_Each_Level();
#else
	printf("OK version:%x.%x.%x\n", BF_Get_OK_MP_Version(), BF_Get_OK_Sub_Version(), BF_Get_OK_Debug_Version());
	printf("... %s build\n", BF_CompileInfoBuild_Get_DataTimeStr() );
	printf("... %s update\n", BF_CompileInfoUpdate_Get_DataTimeStr() );
	printf("booting...\n");
#endif
}

VOID BF_Startup_Start_File_System(VOID)
{
#ifdef CO_USB_MASS_STORAGE_ON
	//For USB Mass Storage(No filesystem. Media driver only)
	T_FS_CONTROLLER_MEDIA_STATUS	gBF_Fs_Controller_Media_Status;
	E_BF_MEDIA_DRV_WRAPPER_WP		media_wp;
	static FW_INT32 ercd_nf = 0;
	static FW_INT32 ercd_sd0 = 0;
	static FW_INT32 ercd_sd1 = 0;

	ercd_nf = BF_Media_Drv_Wrapper_Init_NF(&gBF_Fs_Controller_Media_Status.driver_tbl, &gBF_Fs_Controller_Media_Status.media_info, &media_wp);
	if (ercd_nf!=0){
		printf("BF_Startup_Start_File_System(): Init_NF NG(%d)\n", ercd_nf);
	}
	ercd_sd0 = BF_Media_Drv_Wrapper_Init_SDIO0(&gBF_Fs_Controller_Media_Status.driver_tbl, &gBF_Fs_Controller_Media_Status.media_info, &media_wp);
	if (ercd_sd0 != 0){
		printf("BF_Startup_Start_File_System(): Init_SDIO0 NG(%d)\n", ercd_sd0);
	}
	ercd_sd1 = BF_Media_Drv_Wrapper_Init_SDIO1(&gBF_Fs_Controller_Media_Status.driver_tbl, &gBF_Fs_Controller_Media_Status.media_info, &media_wp);
	if (ercd_sd1 != 0){
		printf("BF_Startup_Start_File_System(): Init_SDIO1 NG(%d)\n", ercd_sd1);
	}
#else	//CO_USB_MASS_STORAGE_ON
	//For normal boot
	BF_Fs_If_Start_File_System();
#endif	//CO_USB_MASS_STORAGE_ON
}

VOID BF_Startup_Init_Display(VOID)
{
	U_IM_DISP_LALP			lalp = {0};

	//Set DISP clock
	Dd_Top_Set_CLKSTOP3_DISPAP(0);
	Dd_Top_Set_CLKSTOP3_DISPAH(0);
	Dd_Top_Set_CLKSTOP3_DISPAX(0);
	Dd_Top_Set_CLKSTOP3_LCDCK(0);

	// Initialize DISP macro to clear soft-reset bit.
	Im_DISP_Init();

	// Enable Black Back
	lalp.bit.ALP = 0xFF;
	Im_DISP_Set_Alpha_Value( E_IM_DISP_LCD_MIPI, lalp );
	OS_User_Sta_Tsk(TID_IPCU_SET_OSD_CONFIG, 0);
}

VOID BF_Startup_Boot_Sample(void)
{
	BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID_BOOT);

	BF_Startup_Init_BaseFW();
	BF_Initialize_Image_Master_Module();

	FJ_HostPreBootProc(0);

	BF_Startup_Start_File_System();

	BF_Startup_Init_Display();
	
	// delete_basefw\fj\iq
	//BF_IQ_Boot_Init();

	#if 0 // The lowest driving current
	Dd_Top_Set_DBCNT1_T07DC(0);
	Dd_Top_Set_DBCNT1_T08DC(0);
	Dd_Top_Set_DBCNT1_T09DC(0);
	Dd_Top_Set_DBCNT1_T10DC(0);
	Dd_Top_Set_DBCNT1_T11DC(0);
	Dd_Top_Set_DBCNT1_T12DC(0);
	Dd_Top_Set_DBCNT1_T13DC(0);
	Dd_Top_Set_DBCNT1_T14DC(0);
	Dd_Top_Set_DBCNT1_T15DC(0);

	Dd_Top_Set_DBCNT2_T16DC(0);
	Dd_Top_Set_DBCNT2_T17DC(0);
	Dd_Top_Set_DBCNT2_T18DC(0);
	Dd_Top_Set_DBCNT2_T19DC(0);
	Dd_Top_Set_DBCNT2_T20DC(0);
	Dd_Top_Set_DBCNT2_T21DC(0);
	Dd_Top_Set_DBCNT2_T22DC(0);
	Dd_Top_Set_DBCNT2_T23DC(0);
	Dd_Top_Set_DBCNT2_T24DC(0);
	Dd_Top_Set_DBCNT2_T25DC(0);
	Dd_Top_Set_DBCNT2_T26DC(0);
	Dd_Top_Set_DBCNT2_T27DC(0);
	Dd_Top_Set_DBCNT2_T28DC(0);
	Dd_Top_Set_DBCNT2_T29DC(0);
	Dd_Top_Set_DBCNT2_T30DC(0);
	Dd_Top_Set_DBCNT2_T31DC(0);

	Dd_Top_Set_DBCNT3_T32DC(0);
	Dd_Top_Set_DBCNT3_T33DC(0);
	Dd_Top_Set_DBCNT3_T34DC(0);
	Dd_Top_Set_DBCNT3_T35DC(0);
	Dd_Top_Set_DBCNT3_T36DC(0);
	Dd_Top_Set_DBCNT3_T37DC(0);
	Dd_Top_Set_DBCNT3_T38DC(0);
	Dd_Top_Set_DBCNT3_T39DC(0);
	Dd_Top_Set_DBCNT3_T40DC(0);
	Dd_Top_Set_DBCNT3_T41DC(0);
	Dd_Top_Set_DBCNT3_T42DC(0);
	Dd_Top_Set_DBCNT3_T43DC(0);
	Dd_Top_Set_DBCNT3_T44DC(0);
	Dd_Top_Set_DBCNT3_T45DC(0);
	Dd_Top_Set_DBCNT3_T46DC(0);
	Dd_Top_Set_DBCNT3_T47DC(0);
	
	Dd_Top_Set_DBCNT4_T48DC(0);
	Dd_Top_Set_DBCNT4_T49DC(0);
	Dd_Top_Set_DBCNT4_T50DC(0);
	Dd_Top_Set_DBCNT4_T51DC(0);
	Dd_Top_Set_DBCNT4_T52DC(0);
	Dd_Top_Set_DBCNT4_T53DC(0);
	Dd_Top_Set_DBCNT4_T54DC(0);
	Dd_Top_Set_DBCNT4_T55DC(0);
	Dd_Top_Set_DBCNT4_T56DC(0);
	Dd_Top_Set_DBCNT4_T57DC(0);
	Dd_Top_Set_DBCNT4_T58DC(0);
	Dd_Top_Set_DBCNT4_T59DC(0);
	Dd_Top_Set_DBCNT4_T60DC(0);
	Dd_Top_Set_DBCNT4_T61DC(0);
	Dd_Top_Set_DBCNT4_T62DC(0);
	Dd_Top_Set_DBCNT4_T63DC(0);

	Dd_Top_Set_DBCNT5_T64DC(0);
	Dd_Top_Set_DBCNT5_T65DC(0);
	Dd_Top_Set_DBCNT5_T66DC(0);
	#endif

	FJ_HostPreBootProc(1);

	OS_User_Sta_Tsk(TID_SYSTEM, 0);
	OS_User_Sta_Tsk(TID_USER, 0);
	OS_User_Sta_Tsk(TID_USER_SYNC, 0);
	OS_User_Sta_Tsk(TID_USER_RECORD, 0);

	fj_bootmessageprint();
	
	BF_Measure_Time_Print(E_BF_MEASURE_TIME_ID_BOOT, ":::TEST TIMER:::\n");

	FJ_HostProc(0);
}

// --- for Fast Boot Debug ---

void dummy(void)
{
	__asm__ volatile ("nop");
}

void dummy_view(void)
{
	__asm__ volatile ("nop");
}

void dummy_pb(void)
{
	__asm__ volatile ("nop");
}
