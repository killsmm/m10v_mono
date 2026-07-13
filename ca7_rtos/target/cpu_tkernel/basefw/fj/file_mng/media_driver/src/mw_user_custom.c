/**
 * @file		mw_user_custom.c
 * @brief		common process for media drivers
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "debug.h" 
#include "os_user_custom.h"
#include "sdram_map_common.h"
#include "sdram_map_cache_define.h"
#include "../../include/fs_user_custom.h"

#include "nf_custom.h" 
#include "emmc_custom.h" 
#include "mw_user_custom.h"
#include "mwnf.h"
#include "mwnf_format.h"
#include "mwsdio.h"
#include "sd_cmd.h"

#include "ddim_typedef.h"
//#include "api_wrapper.h"

#include <string.h>

#include "dd_arm.h"
#include "dd_top.h"
#include "dd_gic.h"
#include "dd_exs.h"
#include "l1l2cache.h"
#include "os_user_custom.h"

#include "fj_host.h"

/*---------------------------------------------------------------*/
/* Definition 													 */
/*---------------------------------------------------------------*/
#define  CO_MEDIA_CLK_CTL

#define  D_SDRAM_MAP_COMMON_SDRAM (D_SDRAM_MAP_COMMON_SDRAM_BASE | D_SDRAM_MAP_COMMON_DCACHE_OR)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Structure 													 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Global Data 													 */
/*---------------------------------------------------------------*/
// NOTE: Should be set up following IDs before each media driver initialization.
// semaphore ID
volatile UINT32	gSD0_SID_SD;		// SD0
volatile UINT32	gSD1_SID_SD;		// SD1
volatile UINT32	gMWNF_SID_NF;		// NF
volatile UINT32	gEMMC_SID;			// eMMC

// eventflag ID
volatile UINT32	gSD0_FID_SD;		// SD0
volatile UINT32	gSD1_FID_SD;		// SD1
volatile UINT32	gMWNF_FID_NF;		// NF
volatile UINT32	gEMMC_FID;			// eMMC

volatile T_NF_PARTITION gNF_partition_table[D_NF_PARTITION_NUM] __attribute__((section(".PARTITION_TABLE_SECTION")));
volatile UINT32 gPtableBlockTable[D_NF_PARTITION_TABLE_PARAM_NUM] __attribute__((section(".PARTITION_BLOCK_SECTION")));
volatile T_NF_NAND_PARAMETER gNANDFlashParameter __attribute__((section(".NAND_PARAMETER_SECTION")));

static volatile T_NF_ATTRIBUTE	gMWNF_Attribute = { D_NF_CE_1,
													D_NF_MAX_BAD_BLOCK_NUM_96,
													D_NF_DEVICE_BLOCK_NUM_4096,
													D_NF_SEARCH_OTHER_PAGE_1,
#ifdef CO_META_ON
													D_FLAG_META_MODE_ON,
#else
													D_FLAG_META_MODE_OFF,
#endif
													D_NF_DC_1,
													D_NF_DC_1,
													D_NF_DC_1,
													0 };

volatile UCHAR	gMW_emp_data[D_MW_SECTOR_SIZE * 32];
volatile ULONG	formatSectors = 0;		// format sectors
volatile ULONG	formatted = 0;			// formatted sectors
volatile ULONG	formatSectors_phy = 0;	// format sectors(physical format)

/*---------------------------------------------------------------*/
/* Macro 														 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/
/* Function 													 */
/*---------------------------------------------------------------*/
/*------------------------------------------------------------------
    function : Media_Sleep_Task
  ------------------------------------------------------------------*/
VOID Media_Sleep_Task( VOID )
{
	OS_User_Slp_Tsk();
}

/*------------------------------------------------------------------
    function : Media_Get_Time
  ------------------------------------------------------------------*/
VOID Media_Get_Time(T_SYSTEM_TIME* time)
{
	OS_USER_SYSTIM	systime;
	OS_User_Get_Tim(&systime);
	
	time->utime = (short)(systime >> 32);
	time->ltime = (unsigned long)systime;
}

/*------------------------------------------------------------------
    function : Media_Custom_Sleep_Task
  ------------------------------------------------------------------*/
VOID Media_Custom_Sleep_Task(INT32 time)
{
	OS_User_Tslp_Tsk( time );
}

/*------------------------------------------------------------------
    function : Media_Custom_Get_Time
  ------------------------------------------------------------------*/
VOID Media_Custom_Get_Time(T_SYSTEM_TIME* time)
{
	BF_Fs_User_Custom_Get_Time(time);
}

/*------------------------------------------------------------------
    function : Media_Custom_Get_Time
  ------------------------------------------------------------------*/
VOID Media_Custom_Get_Time_us(ULONG* time)
{
	BF_Fs_User_Custom_Get_Time_us(time);
}

/*------------------------------------------------------------------
    function : Media_Custom_Wait_ns
  ------------------------------------------------------------------*/
VOID Media_Custom_Wait_ns(UINT32 time)
{
	Dd_ARM_Wait_ns(time);
}

/*------------------------------------------------------------------
    function : Media_Custom_Info_Printer
  ------------------------------------------------------------------*/
VOID (*Media_Custom_Info_Printer)(const CHAR*, ...) = (VOID (*)(const CHAR *, ...))FJ_Debug_Print_Info;


/*------------------------------------------------------------------
    function : Media_Custom_Wrn_Printer
  ------------------------------------------------------------------*/
//VOID (*Media_Custom_Wrn_Printer)(const CHAR*, ...) = (VOID (*)(const CHAR *, ...))Wrn_Printer;


/*------------------------------------------------------------------
	function : Media_Custom_Err_Printer
  ------------------------------------------------------------------*/
VOID (*Media_Custom_Err_Printer)(const CHAR*, ...) = (VOID (*)(const CHAR *, ...))FJ_Debug_Print_Err;

/*------------------------------------------------------------------
    function : Media_Custom_Set_DSBU
  ------------------------------------------------------------------*/
VOID Media_Custom_Set_DSBU(VOID)
{
	Dd_ARM_Dsb_Pou();
}

/*------------------------------------------------------------------
    function : Media_Custom_Set_DSBC
  ------------------------------------------------------------------*/
VOID Media_Custom_Set_DSBC(VOID)
{
	Dd_ARM_Dsb_Poc();
}

/*---------------------------------------------------------------*/
/* Global Function 												 */
/*---------------------------------------------------------------*/
/*------------------------------------------------------------------
    function : Media_Custom_Sig_Sem
  ------------------------------------------------------------------*/
INT32 Media_Custom_Sig_Sem(INT32 semid)
{
	return OS_User_Sig_Sem((OS_USER_ID)semid);
}

INT32 Media_Custom_Dly_Tsk(UINT32 time)
{
	return OS_User_Dly_Tsk( (OS_USER_RELTIM)time );
}

/*------------------------------------------------------------------
    function : Media_Custom_Twai_Sem
  ------------------------------------------------------------------*/
INT32 Media_Custom_Twai_Sem(INT32 semid,INT32 tmout)
{
	return OS_User_Twai_Sem((OS_USER_ID)semid,(OS_USER_TMO)tmout);
}

/*------------------------------------------------------------------
    function : Media_Custom_Set_Flag
  ------------------------------------------------------------------*/
INT32  Media_Custom_Set_Flag(INT32 flg_id,INT32 flg_ptn)
{
	return OS_User_Set_Flg((OS_USER_ID)flg_id, (OS_USER_FLGPTN)flg_ptn);
}

/*------------------------------------------------------------------
    function : Media_Custom_Wait_Flag
  ------------------------------------------------------------------*/
INT32  Media_Custom_Wait_Flag(INT32 flg_id, UINT32* flg_ptn, UINT32 wai_ptn, UINT32 wai_mode, INT32 time)
{
	UINT32 wait_mode;

	wait_mode = 0;
	if (wai_mode == D_FLAG_WAIT_AND) {
		wait_mode = D_OS_USER_TWF_ANDW;
	}
	else if ((wai_mode & D_FLAG_WAIT_OR) == D_FLAG_WAIT_OR) {
		wait_mode = D_OS_USER_TWF_ORW;
	}

	return OS_User_Twai_Flg((OS_USER_ID)flg_id, (OS_USER_FLGPTN)wai_ptn, (OS_USER_MODE)wait_mode, (OS_USER_FLGPTN*)flg_ptn, (OS_USER_TMO)time);
}

/*------------------------------------------------------------------
    function : Media_Custom_Clear_Flag
  ------------------------------------------------------------------*/
INT32  Media_Custom_Clear_Flag(UINT32 flg_id, UINT32 flg_ptn)
{
	return OS_User_Clr_Flg((OS_USER_ID)flg_id, (OS_USER_FLGPTN)flg_ptn);
}

/*------------------------------------------------------------------
    function : Media_Custom_memcpy
  ------------------------------------------------------------------*/
VOID Media_Custom_memcpy(VOID* d_buf, VOID* s_buf, ULONG size)
{
	memcpy(d_buf, s_buf, size);
}

/*------------------------------------------------------------------
    function : Media_Custom_memset
  ------------------------------------------------------------------*/
VOID Media_Custom_memset(VOID* s_buf, BYTE data, ULONG size)
{
	memset(s_buf, data, size);
}
/*------------------------------------------------------------------
    function : Media_Custom_memcmp
  ------------------------------------------------------------------*/
INT32 Media_Custom_memcmp(PBYTE s_data, PBYTE t_data, ULONG size)
{
	return memcmp(s_data, t_data, size);
}

/*------------------------------------------------------------------
    function : Media_Custom_strncmp
  ------------------------------------------------------------------*/
INT32 Media_Custom_strncmp(const CHAR* s_buf, CHAR* t_buf, ULONG size)
{
	return strncmp(s_buf, t_buf, size);
}

static INT32 media_custom_cache_aera_check( ULONG addr )
{
	UINT32 ttbaddr;				// Translation table base Address
	UINT32 ttbdata;				// Translation table base Data
	UINT32 l1cache_flg = 0;		// L1 Cache run flag

	// Get Translation table base Address
	ttbaddr = Dd_ARM_Get_TTB();
	// Translation table base Convert to PageTable
	ttbaddr = (ttbaddr & 0xFFFFC000) | ((addr & 0xFFF00000) >> 20) * 4;
	ttbdata = *((volatile unsigned int*)(ttbaddr));
	// Cache run flag
	switch((ttbdata >> 2) & 0x3){
		case 1:
		case 2:
		case 3:
			l1cache_flg = 1;
			break;
	}
	return l1cache_flg;
}

/*------------------------------------------------------------------
    function : Media_Custom_Dcache_Clean_Flush
------------------------------------------------------------------*/
VOID Media_Custom_Dcache_Clean_Flush( ULONG* addr, ULONG num_sec )
{
	if( media_custom_cache_aera_check((ULONG)addr) == 1 ) {
		BF_L1l2cache_Clean_Flush_Addr((UINT32)addr, (num_sec * 512));
		Media_Custom_Set_DSBC();
	}
}

VOID Media_Custom_Clean_Dcache_All( ULONG* addr, ULONG num_sec )
{
	if( media_custom_cache_aera_check((ULONG)addr) == 1 ) {
		BF_L1l2cache_Clean_Addr((UINT32)addr, (num_sec * 512));
		Media_Custom_Set_DSBC();
	}
}

INT32 Media_Custom_Get_Log2Phy_Addr( ULONG addr )
{
	return M_SDRAM_MAP_COMMON_ADR_LOG2PHY(addr);
}

INT32 Media_Custom_NF_GIC_Ctrl(VOID)
{
#ifdef CO_NF_DRIVER_ENABLE // for Linux Co-op
	OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_NANDFLASH_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
	return 0;
#else	// for Linux Co-op
	return 0;
#endif	// for Linux Co-op
}

INT32 Media_Custom_NF_GIC_Ctrl_Dis(VOID)
{
#ifdef CO_NF_DRIVER_ENABLE // for Linux Co-op
	OS_User_DisableInt((OS_USER_INTID)E_DD_GIC_INTID_NANDFLASH_INT);
	return 0;
#else	// for Linux Co-op
	return 0;
#endif	// for Linux Co-op
}

INT32 Media_Custom_XDMAC_EXS_CH0_GIC_Ctrl(VOID)
{
	OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_EXS_XDMAC_CH0_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
	return 0;
}

VOID Media_Custom_Change_NF_CLK(INT32 clk)
{
	Dd_Top_Set_CLKSTOP2_NFCK(1);
	Dd_Top_Set_CLKSEL1_NFCLK(clk);
	Dd_Top_Set_CLKSTOP2_NFCK(0);
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Drive_Ctrl
  ------------------------------------------------------------------*/
VOID Media_Custom_SD0_Drive_Ctrl(UINT32 dbcnt)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	Dd_Top_Set_DBCNT4_T53DC(dbcnt);
	Dd_Top_Set_DBCNT4_T54DC(dbcnt);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Set_Clock
  ------------------------------------------------------------------*/
VOID Media_Custom_SD0_Set_Clock(ULONG clock)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	BYTE clksel;

	if(clock >= D_SD_SDR104_CLK) {
		clksel = 8;			// 700MHz
	}
	else if(clock >= D_SD_SDR50_CLK) {
		clksel = 2;			// 400MHz
	}
	else if(clock >= D_SD_BASE_CLK) {
		clksel = 3;			// 200MHz
	}
	else {
		clksel = 4;			// 100MHz
	}

	Dd_Top_Set_CLKSTOP2_UHS1CK2(1);
	Dd_Top_Set_CLKSEL1_UHS1CLK2(clksel);
	Dd_Top_Set_CLKSTOP2_UHS1CK2(0);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Get_Clock
  ------------------------------------------------------------------*/
ULONG Media_Custom_SD0_Get_Clock(VOID)
{
	ULONG clock;

	switch( Dd_Top_Get_CLKSEL1_UHS1CLK2() ) {
		case 2:
			clock = D_SD_SDR50_CLK;
			break;
		case 3:
			clock = D_SD_BASE_CLK;
			break;
		case 4:
			clock = D_SD_TRN_CLK;
			break;
		case 8:
			clock = D_SD_SDR104_CLK;
			break;
		default :
			clock = D_SD_TRN_CLK;
			break;
	}
	return clock;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Start_Clock
------------------------------------------------------------------*/
INT32 Media_Custom_SD0_Start_Clock(VOID)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	int ercd;

	ercd = Media_Custom_Twai_Sem(gSD0_SID_SD, D_SEM_TIMEOUT);
	if (ercd != 0) {
//		M_SD0_DEBUG_PRINT(("Start SD0 clk Sem wait err(%d)\n", ercd));
		return ercd;
	}
//#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Start_SD0_Clock();
//#endif
#endif	// for Linux Co-op
	return 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Stop_Clock
------------------------------------------------------------------*/
extern volatile BYTE gSD0_Card_Type;
VOID Media_Custom_SD0_Stop_Clock(VOID)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
#ifdef CO_MEDIA_CLK_CTL
	if (gSD0_Card_Type != (BYTE)D_SD_CARD_TYPE_SDIO) {	// SDIO case, don't must stop clock for interrupt period.
		BF_Fs_User_Custom_Stop_SD0_Clock();
	}
#endif
	Media_Custom_Sig_Sem(gSD0_SID_SD);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Get_SDCDX
------------------------------------------------------------------*/
INT32 Media_Custom_SD0_Get_SDCDX(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	BYTE status = 0;
#else
	BYTE status = 1;
#endif

#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	status = Dd_Exs_Get_SIGMON_SD4_CD();
#endif	// for Linux Co-op

	return ( status ) ? 
		D_SD_NO_CARD : D_SD_ANY_CARD;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Get_State
------------------------------------------------------------------*/
INT32 Media_Custom_SD0_Get_State(VOID)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	return ( MWSDIO0_Get_State() );
#else
	return 1;
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Crear_State
------------------------------------------------------------------*/
VOID Media_Custom_SD0_Crear_State(VOID)
{
	MWSDIO0_Clear_State();
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Set_RAMPD
  ------------------------------------------------------------------*/
VOID Media_Custom_SD0_Set_RAMPD(UINT32 slp_flg)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	Dd_Exs_Set_RAM_PD_Ctrl_SD4(slp_flg);
	Media_Custom_Sleep_Task(D_SD_RAM_PD_WAIT_TIME);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Set_Power
------------------------------------------------------------------*/
VOID Media_Custom_SD0_Set_Power(UINT32 onoff)
{
#ifndef CO_BOARD_TYPE_CHOPES_EDB1
	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P65, onoff);
#else
	FJ_HostSDPowerSupplyCB( 0, (UCHAR)onoff );
#endif
}

/*------------------------------------------------------------------
    function : Media_Custom_SD0_Change_Voltage_CB
------------------------------------------------------------------*/
BOOL Media_Custom_SD0_Change_Voltage_CB(UCHAR voltage)
{
	return FJ_HostSDChangeVoltageCB( 0, voltage );
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Drive_Ctrl
  ------------------------------------------------------------------*/
VOID Media_Custom_SD1_Drive_Ctrl(UINT32 dbcnt)
{
#ifdef CO_SD0_DRIVER_ENABLE // for Linux Co-op
	Dd_Top_Set_DBCNT4_T52DC(dbcnt);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Set_Clock
  ------------------------------------------------------------------*/
VOID Media_Custom_SD1_Set_Clock(ULONG clock)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	BYTE clksel;

	if(clock > D_SD_SDR104_CLK_1) {
		Media_Custom_Err_Printer("Set SDIO1 clk err(%d).\n", clock);
	}
	clksel = 3;		 	// 200MHz

	Dd_Top_Set_CLKSTOP2_UHS1CK1(1);
	Dd_Top_Set_CLKSEL1_UHS1CLK1(clksel);
	Dd_Top_Set_CLKSTOP2_UHS1CK1(0);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Set_Clock_Uhs2
  ------------------------------------------------------------------*/
VOID Media_Custom_SD1_Set_Clock_Uhs2(ULONG clock)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	BYTE clksel;

	switch(clock) {
		case D_SD_UHS2_CLK:
			clksel = 0;
			break;
		case D_SD_UHS2_CLK_46:
			clksel = 1;
			break;
		case D_SD_UHS2_CLK_42:
			clksel = 2;
			break;
		case D_SD_UHS2_CLK_38:
			clksel = 3;
			break;
		case D_SD_UHS2_CLK_35:
			clksel = 4;
			break;
		case D_SD_UHS2_CLK_33:
			clksel = 5;
			break;
		case D_SD_UHS2_CLK_29:
			clksel = 6;
			break;
		case D_SD_UHS2_CLK_25:
			clksel = 7;
			break;
		default :
			clksel = 0;
			break;
	}

	Dd_Top_Set_CLKSTOP2_UHS2CK(1);
	Dd_Top_Set_CLKSEL1_UHS2CLK(clksel);
	Dd_Top_Set_CLKSTOP2_UHS2CK(0);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Get_Clock
  ------------------------------------------------------------------*/
ULONG Media_Custom_SD1_Get_Clock(VOID)
{
	ULONG clock;

	switch( Dd_Top_Get_CLKSEL1_UHS1CLK1() ) {
		case 3:
			clock = D_SD_BASE_CLK;
			break;
		case 4:
			clock = D_SD_TRN_CLK;
			break;
		case 8:
			clock = D_SD_SDR104_CLK;
			break;
		default :
			clock = D_SD_TRN_CLK;
			break;
	}
	return clock;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Get_Clock_Uhs2
  ------------------------------------------------------------------*/
ULONG Media_Custom_SD1_Get_Clock_Uhs2(VOID)
{
	ULONG clock;

	switch(Dd_Top_Get_CLKSEL1_UHS2CLK()) {
		case 0:
			clock = D_SD_UHS2_CLK;
			break;
		case 1:
			clock = D_SD_UHS2_CLK_46;
			break;
		case 2:
			clock = D_SD_UHS2_CLK_42;
			break;
		case 3:
			clock = D_SD_UHS2_CLK_38;
			break;
		case 4:
			clock = D_SD_UHS2_CLK_35;
			break;
		case 5:
			clock = D_SD_UHS2_CLK_33;
			break;
		case 6:
			clock = D_SD_UHS2_CLK_29;
			break;
		case 7:
			clock = D_SD_UHS2_CLK_25;
			break;
		default:
			clock = 0;
			break;
	}

	return clock;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Start_Clock
------------------------------------------------------------------*/
INT32 Media_Custom_SD1_Start_Clock(VOID)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	INT32 ercd;

	ercd = Media_Custom_Twai_Sem(gSD1_SID_SD, D_SEM_TIMEOUT);
	if (ercd != 0) {
//		BF_Debug_Print_Error(("Start SDIO clk Sem wait err(%d)\n", ercd));
		return ercd;
	}
#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Start_SD1_Clock();
#endif
#endif	// for Linux Co-op
	return 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Stop_Clock
------------------------------------------------------------------*/
extern volatile BYTE gSD1_Card_Type;
VOID Media_Custom_SD1_Stop_Clock(VOID)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
#ifdef CO_MEDIA_CLK_CTL
	if (gSD1_Card_Type != (BYTE)D_SD_CARD_TYPE_SDIO) {	// SDIO case, don't must stop clock for interrupt period.
		BF_Fs_User_Custom_Stop_SD1_Clock();
	}
#endif
	Media_Custom_Sig_Sem(gSD1_SID_SD);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Get_SDCDX
------------------------------------------------------------------*/
INT32 Media_Custom_SD1_Get_SDCDX(VOID)
{
#ifdef CO_LINUX_VERSION_ON
	BYTE status = 0;
#else
	BYTE status = 1;
#endif

#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	status = Dd_Exs_Get_SIGMON_SD3_CD();
#endif	// for Linux Co-op

	return ( status ) ? 
		D_SD_NO_CARD : D_SD_ANY_CARD;
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Get_State
------------------------------------------------------------------*/
INT32 Media_Custom_SD1_Get_State(VOID)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	return ( MWSDIO1_Get_State() );
#else
	return 1;
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Crear_State
------------------------------------------------------------------*/
VOID Media_Custom_SD1_Crear_State(VOID)
{
	MWSDIO1_Clear_State();
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Set_RAMPD
  ------------------------------------------------------------------*/
VOID Media_Custom_SD1_Set_RAMPD(UINT32 slp_flg)
{
#ifdef CO_SD1_DRIVER_ENABLE // for Linux Co-op
	Dd_Exs_Set_RAM_PD_Ctrl_SD3(slp_flg);
	Media_Custom_Sleep_Task(D_SD_RAM_PD_WAIT_TIME);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Set_Power
------------------------------------------------------------------*/
VOID Media_Custom_SD1_Set_Power(UINT32 onoff)
{
#ifndef CO_BOARD_TYPE_CHOPES_EDB1
	Dd_Top_Set_Gpio_Status(E_DD_TOP_GPIO_P64, onoff);
#else
	FJ_HostSDPowerSupplyCB( 1, (UCHAR)onoff );
#endif
}

/*------------------------------------------------------------------
    function : Media_Custom_SD1_Change_Voltage_CB
------------------------------------------------------------------*/
BOOL Media_Custom_SD1_Change_Voltage_CB(UCHAR voltage)
{
	return FJ_HostSDChangeVoltageCB( 1, voltage );
}

static ULONG media_custom_get_partition_size(VOID)
{
	ULONG cnt;
	ULONG total_sector;

	total_sector = gNF_partition_table[0].area[0].sector_num;

	for(cnt = 0; cnt < D_NF_PARTITION_NUM; cnt++) {
		total_sector += (gNF_partition_table[cnt].area_size / 512);
	}
	return total_sector;
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_SoftReset
------------------------------------------------------------------*/
VOID Media_Custom_NF_SoftReset(VOID)
{
	Dd_Exs_Set_nf_rst_n_Soft_Reset(0);
	Dd_Exs_Set_bch_rst_n_Soft_Reset(0);
	Dd_Exs_Set_reg_rst_n_Soft_Reset(0);

	Media_Custom_Dly_Tsk(2);

	Dd_Exs_Set_nf_rst_n_Soft_Reset(1);
	Dd_Exs_Set_bch_rst_n_Soft_Reset(1);
	Dd_Exs_Set_reg_rst_n_Soft_Reset(1);
	Dd_Exs_Set_NF_NFWPX_CNT(1);
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Start_Clock
------------------------------------------------------------------*/
INT32 Media_Custom_NF_Start_Clock(USHORT timeout_NF, USHORT timeout_XDMAC)
{
	INT32 ercd;

	ercd = Media_Custom_Twai_Sem(gMWNF_SID_NF, D_SEM_TIMEOUT);
	if (ercd != 0) {
		Media_Custom_Err_Printer("MWNF : ERROR : Media_Custom_NF_Start_Clock(). Start NF clk Sem wait err(%d).\n", ercd);
		return ercd;
	}

#ifdef CO_MEDIA_CLK_CTL
	ercd = BF_Fs_User_Custom_Start_NF_Clock(timeout_XDMAC, 0);
	if (ercd != 0) {
		Media_Custom_NF_Stop_Clock(timeout_NF, timeout_XDMAC);

		Media_Custom_Err_Printer("MWNF : ERROR : Media_Custom_NF_Start_Clock(). Start NF clk BF_Fs_User_Custom_Start_NF_Clock err(%d).\n", ercd);
		return ercd;
	}
#endif

	return 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Direct_Start_Clock
------------------------------------------------------------------*/
INT32 Media_Custom_NF_Direct_Start_Clock(USHORT timeout)
{
#ifdef CO_MEDIA_CLK_CTL
	INT32 ercd;

	ercd = BF_Fs_User_Custom_Start_NF_Clock(timeout, 1);
	if (ercd != 0) {
		return ercd;
	}
#endif
	return 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Stop_Clock
------------------------------------------------------------------*/
VOID Media_Custom_NF_Stop_Clock(USHORT timeout_NF, USHORT timeout_XDMAC)
{
#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Stop_NF_Clock(timeout_NF, timeout_XDMAC, 0);
#endif

	Media_Custom_Sig_Sem(gMWNF_SID_NF);
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Direct_Stop_Clock
------------------------------------------------------------------*/
VOID Media_Custom_NF_Direct_Stop_Clock(USHORT timeout_NF, USHORT timeout_XDMAC)
{
#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Stop_NF_Clock(timeout_NF, timeout_XDMAC, 1);
#endif
}

#if 0
/*------------------------------------------------------------------
    function : Media_Custom_NF_Set_Attribute
------------------------------------------------------------------*/
VOID Media_Custom_NF_Set_Attribute(UINT32 MID, UINT32 DID, T_NF_ATTRIBUTE* attr)
{
	UINT32 default_flg = 0;

	if (MID == D_MICRON_MID) {
		switch(DID) {
	        case D_MT29F2G08AAD :
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_2048;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_48;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_DIS;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
	        case D_MT29F16G08CBACA :	// 4K
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_2048;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_48;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_DIS;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
	        case D_MT29F64G08CBAAA :	// 8K
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_4096;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_96;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_DIS;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
	        case D_MT29F256G08CJAAA :	// 8K
				attr->ce_num            = D_NF_CE_2;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_8192;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_192;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_DIS;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
			default :
				default_flg = 1;
				break;
		}
	} else if (MID == D_SUMSUNG_MID) {
		switch(DID) {
	        case D_K9F1G08U0A :
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_1024;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_24;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_1;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
	        case D_K9F5608U0D :	// small block
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_2048;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_48;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_1;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
			default :
				default_flg = 1;
				break;			
		}
	} else if (MID == D_HYNIX_MID) {
		switch(DID) {		
			case D_HY27US08561A :	// small block
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_2048;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_48;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_1;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
			default :
				default_flg = 1;
				break;
		}
	} else if (MID == D_NUMONYX_MID) {
		switch(DID) {
	        case D_NAND256W3A28N6 :	// small block
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_2048;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_48;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_DIS;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
			default :
				default_flg = 1;
				break;
		}
    } else if (MID == D_SPANSION_MID) {
		switch(DID) {
			case D_S34ML04G1 :
				attr->ce_num            = D_NF_CE_2;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_4096;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_96;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_1;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
			default :
				default_flg = 1;
				break;
		}

    } else if (MID == D_MARCRNIX_MID) {
		switch(DID) {
			case D_MX30UF4G16 :
				attr->ce_num            = D_NF_CE_1;
				attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_4096;
				attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_96;
				attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_1;
				attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
				attr->dbcnt0_t0dc       = D_NF_DC_0;
				attr->dbcnt0_t1dc       = D_NF_DC_0;
				attr->dbcnt0_t2dc       = D_NF_DC_0;
				attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
				attr->ecc_move_bits     = 0;
				break;
			default :
				default_flg = 1;
				break;
		}

	} else {
		default_flg = 1;
    }

	if (default_flg == 1) {
		attr->ce_num            = D_NF_CE_1;
		attr->device_block_num  = D_NF_DEVICE_BLOCK_NUM_1024;
		attr->max_bad_block_num = D_NF_MAX_BAD_BLOCK_NUM_24;
		attr->search_other_page = D_NF_SEARCH_OTHER_PAGE_63;
		attr->meta_mode_flg     = D_FLAG_META_MODE_ON;
		attr->dbcnt0_t0dc       = D_NF_DC_0;
		attr->dbcnt0_t1dc       = D_NF_DC_0;
		attr->dbcnt0_t2dc       = D_NF_DC_0;
		attr->access_cycle      = E_MWNF_ACCESS_CYCLE_25NSEC_CLK1;
		attr->ecc_move_bits     = 0;
	}
}
#endif

/*------------------------------------------------------------------
    function : Media_Custom_NF_Set_Attribute
------------------------------------------------------------------*/
VOID Media_Custom_NF_Set_Attribute(T_NF_ATTRIBUTE* attr)
{
	gMWNF_Attribute.ce_num            = attr->ce_num;
	gMWNF_Attribute.device_block_num  = attr->device_block_num;
	gMWNF_Attribute.max_bad_block_num = attr->max_bad_block_num;
	gMWNF_Attribute.search_other_page = attr->search_other_page;
	
#ifdef CO_META_ON
	gMWNF_Attribute.meta_mode_flg     = D_FLAG_META_MODE_ON;
#else
	gMWNF_Attribute.meta_mode_flg     = D_FLAG_META_MODE_OFF;
#endif
	gMWNF_Attribute.dbcnt0_t0dc       = attr->dbcnt0_t0dc;
	gMWNF_Attribute.dbcnt0_t1dc       = attr->dbcnt0_t1dc;
	gMWNF_Attribute.dbcnt0_t2dc       = attr->dbcnt0_t2dc;
	gMWNF_Attribute.ecc_move_bits     = 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Set_Attribute
------------------------------------------------------------------*/
VOID Media_Custom_NF_Get_Attribute(UINT32 MID, UINT32 DID, T_NF_ATTRIBUTE* attr)
{
	UINT32 default_flg = 0;

	attr->ce_num            = gMWNF_Attribute.ce_num;
	attr->device_block_num  = gMWNF_Attribute.device_block_num;
	attr->max_bad_block_num = gMWNF_Attribute.max_bad_block_num;
	attr->search_other_page = gMWNF_Attribute.search_other_page;
	attr->meta_mode_flg     = gMWNF_Attribute.meta_mode_flg;
	attr->dbcnt0_t0dc       = gMWNF_Attribute.dbcnt0_t0dc;
	attr->dbcnt0_t1dc       = gMWNF_Attribute.dbcnt0_t1dc;
	attr->dbcnt0_t2dc       = gMWNF_Attribute.dbcnt0_t2dc;
	attr->ecc_move_bits     = gMWNF_Attribute.ecc_move_bits;

	if(gNANDFlashParameter.toggle_timings_0 != 0) {
		attr->nf_clk_sel			  = ((gNANDFlashParameter.nf_clk_sel >> 22) & 0x1F);
		attr->common_settings		  = gNANDFlashParameter.common_settings;
		attr->toggle_timings_0		  = gNANDFlashParameter.toggle_timings_0;
		attr->toggle_timings_1		  = gNANDFlashParameter.toggle_timings_1;
		attr->async_toggle_timings	  = gNANDFlashParameter.async_toggle_timings;
		attr->sync_timings			  = gNANDFlashParameter.sync_timings;
		attr->timings0				  = gNANDFlashParameter.timings0;
		attr->timings1				  = gNANDFlashParameter.timings1;
		attr->timings2				  = gNANDFlashParameter.timings2;
		attr->dll_phy_ctrl			  = gNANDFlashParameter.dll_phy_ctrl;
		attr->phy_ctrl_reg			  = gNANDFlashParameter.phy_ctrl_reg;
		attr->phy_tsel_reg			  = gNANDFlashParameter.phy_tsel_reg;
		attr->phy_dq_timing_reg		  = gNANDFlashParameter.phy_dq_timing_reg;
		attr->phy_dqs_timing_reg	  = gNANDFlashParameter.phy_dqs_timing_reg;
		attr->phy_gate_lpbk_ctrl_reg  = gNANDFlashParameter.phy_gate_lpbk_ctrl_reg;
		attr->phy_dll_master_ctrl_reg = gNANDFlashParameter.phy_dll_master_ctrl_reg;
		attr->phy_dll_slave_ctrl_reg  = gNANDFlashParameter.phy_dll_slave_ctrl_reg;
		attr->timing_mode			  = gNANDFlashParameter.timing_mode;
		attr->dbcnt			 		  = gNANDFlashParameter.dbcnt;
		attr->ecc_config			  = gNANDFlashParameter.ecc_config;
		attr->skip_bytes_conf		  = gNANDFlashParameter.skip_bytes_conf;
		attr->skip_bytes_offset		  = gNANDFlashParameter.skip_bytes_offset;
		attr->cache_read_enable		  = gNANDFlashParameter.cache_read_enable;
		return;
	}

	switch(MID) {
		case D_SPANSION_MID :
			switch(DID) {
				case D_S34MS04G2 :
					attr->nf_clk_sel			  = 0x00000000;
					attr->common_settings		  = 0x00000000;
					attr->toggle_timings_0		  = 0x01020a03;
					attr->toggle_timings_1		  = 0x0b020203;
					attr->async_toggle_timings	  = 0x02040203;
					attr->sync_timings			  = 0x00040205;
					attr->timings0				  = 0x0a32060a;
					attr->timings1				  = 0x0a0c0505;
					attr->timings2				  = 0x00640104;
					attr->dll_phy_ctrl			  = 0x01030300;
					attr->phy_ctrl_reg			  = 0x00000040;
					attr->phy_tsel_reg			  = 0x00000000;
					attr->phy_dq_timing_reg		  = 0x00000002;
					attr->phy_dqs_timing_reg	  = 0x00100004;
					attr->phy_gate_lpbk_ctrl_reg  = 0x00380001;
					attr->phy_dll_master_ctrl_reg = 0x00800000;
					attr->phy_dll_slave_ctrl_reg  = 0x00000000;
					attr->timing_mode			  = 0x00000005;
					attr->dbcnt			 		  = 0xA8000000;
					attr->ecc_config			  = 0x00000401;
					attr->skip_bytes_conf		  = 0xFFFF0002;
					attr->skip_bytes_offset		  = 0x00000800;
					attr->cache_read_enable		  = 0x00000000;
					break;
				default :
					default_flg = 1;
					break;
			}
			break;

		case D_MARCRNIX_MID :
			switch(DID) {
				case D_MX30UF4G16 :
					attr->nf_clk_sel			  = 0x00000000;
					attr->common_settings		  = 0x00000000;
					attr->toggle_timings_0		  = 0x01020a03;
					attr->toggle_timings_1		  = 0x0b020203;
					attr->async_toggle_timings	  = 0x02030202;
					attr->sync_timings			  = 0x00050205;
					attr->timings0				  = 0x07080806;
					attr->timings1				  = 0x060c0505;
					attr->timings2				  = 0x00640104;
					attr->dll_phy_ctrl			  = 0x01030300;
					attr->phy_ctrl_reg			  = 0x00000030;
					attr->phy_tsel_reg			  = 0x00000000;
					attr->phy_dq_timing_reg		  = 0x00000002;
					attr->phy_dqs_timing_reg	  = 0x00100004;
					attr->phy_gate_lpbk_ctrl_reg  = 0x00300001;
					attr->phy_dll_master_ctrl_reg = 0x00800000;
					attr->phy_dll_slave_ctrl_reg  = 0x00000000;
					attr->timing_mode			  = 0x00000004;
					attr->dbcnt			 		  = 0xA8000000;
					attr->ecc_config			  = 0x00000401;
					attr->skip_bytes_conf		  = 0xFFFF0002;
					attr->skip_bytes_offset		  = 0x00000800;
					attr->cache_read_enable		  = 0x00000000;
					break;
				default :
					default_flg = 1;
					break;
			}
			break;

		default :
			default_flg = 1;
			break;
	}

	if (default_flg == 1) {
		attr->nf_clk_sel			  = 0x00000000;
		attr->common_settings		  = 0x00000000;
		attr->toggle_timings_0		  = 0x01020a02;
		attr->toggle_timings_1		  = 0x0b020409;
		attr->async_toggle_timings	  = 0x05060505;
		attr->sync_timings			  = 0x00070203;
		attr->timings0				  = 0x14320c14;
		attr->timings1				  = 0x14161905;
		attr->timings2				  = 0x00640207;
		attr->dll_phy_ctrl			  = 0x01030300;
		attr->phy_ctrl_reg			  = 0x00000060;
		attr->phy_tsel_reg			  = 0x00000000;
		attr->phy_dq_timing_reg		  = 0x00000002;
		attr->phy_dqs_timing_reg	  = 0x00100004;
		attr->phy_gate_lpbk_ctrl_reg  = 0x00480001;
		attr->phy_dll_master_ctrl_reg = 0x00800000;
		attr->phy_dll_slave_ctrl_reg  = 0x00000000;
		attr->timing_mode			  = 0x00000000;
		attr->dbcnt			 		  = 0xA8000000;
		attr->ecc_config			  = 0x00000401;
		attr->skip_bytes_conf		  = 0xFFFF0002;
		attr->skip_bytes_offset		  = 0x00000800;
		attr->cache_read_enable		  = 0x00000000;
	}
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Drive_Ctrl
------------------------------------------------------------------*/
VOID Media_Custom_NF_Drive_Ctrl(T_NF_ATTRIBUTE* attr)
{
	Dd_Top_Set_DBCNT3_T45DC(attr->dbcnt0_t0dc);
	Dd_Top_Set_DBCNT3_T46DC(attr->dbcnt0_t1dc);
	Dd_Top_Set_DBCNT3_T47DC(attr->dbcnt0_t2dc);
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Set_Partition_Info
------------------------------------------------------------------*/
VOID Media_Custom_NF_Set_Partition_Info(UCHAR device_kind, UINT32 MID, UINT32 DID, T_NF_Partition_Info* info)
{
	unsigned long	partition_size[NF_MAX_DRV_NUM];
	unsigned long	partition_top[NF_MAX_DRV_NUM];
	int				partition_fat[NF_MAX_DRV_NUM];
	int				partition_cluster_size[NF_MAX_DRV_NUM];
	int				partition_entry_num[NF_MAX_DRV_NUM];
	unsigned long	code_size;
	unsigned long	param_blk_top;
	unsigned long	param_blk_size;
	unsigned long	exception_log_top;

	code_size					= media_custom_get_partition_size();

	partition_top[0]			= D_NF_STORAGE_MBR_SIZE;
	partition_top[1]			= (partition_top[0] + D_NF_STORAGE_0_SIZE);
	partition_top[2]			= (partition_top[1] + D_NF_STORAGE_1_SIZE);
	partition_top[3]			= (partition_top[2] + D_NF_STORAGE_2_SIZE);
	partition_top[4]			= (partition_top[3] + D_NF_STORAGE_3_SIZE);
	partition_top[5]			= (partition_top[4] + D_NF_STORAGE_4_SIZE);
	partition_top[6]			= (partition_top[5] + D_NF_STORAGE_5_SIZE);
	param_blk_top				= (partition_top[6] + D_NF_STORAGE_6_SIZE);
	exception_log_top			= (param_blk_top    + D_NF_PARAMETER_SIZE);

	partition_size[0]			= D_NF_STORAGE_0_SIZE;
	partition_size[1]			= D_NF_STORAGE_1_SIZE;
	partition_size[2]			= D_NF_STORAGE_2_SIZE;
	partition_size[3]			= D_NF_STORAGE_3_SIZE;
	partition_size[4]			= D_NF_STORAGE_4_SIZE;
	partition_size[5]			= D_NF_STORAGE_5_SIZE;
	partition_size[6]			= D_NF_STORAGE_6_SIZE;
	param_blk_size				= D_NF_PARAMETER_SIZE;

	partition_fat[0]			= D_NF_STORAGE_0_FAT;
	partition_fat[1]			= D_NF_STORAGE_1_FAT;
	partition_fat[2]			= D_NF_STORAGE_2_FAT;
	partition_fat[3]			= D_NF_STORAGE_3_FAT;
	partition_fat[4]			= D_NF_STORAGE_4_FAT;
	partition_fat[5]			= D_NF_STORAGE_5_FAT;
	partition_fat[6]			= D_NF_STORAGE_6_FAT;

	partition_cluster_size[0]	= D_NF_STORAGE_0_CLUSTER;
	partition_cluster_size[1]	= D_NF_STORAGE_1_CLUSTER;
	partition_cluster_size[2]	= D_NF_STORAGE_2_CLUSTER;
	partition_cluster_size[3]	= D_NF_STORAGE_3_CLUSTER;
	partition_cluster_size[4]	= D_NF_STORAGE_4_CLUSTER;
	partition_cluster_size[5]	= D_NF_STORAGE_5_CLUSTER;
	partition_cluster_size[6]	= D_NF_STORAGE_6_CLUSTER;

	partition_entry_num[0]		= D_NF_STORAGE_0_ENTRY;
	partition_entry_num[1]		= D_NF_STORAGE_1_ENTRY;
	partition_entry_num[2]		= D_NF_STORAGE_2_ENTRY;
	partition_entry_num[3]		= D_NF_STORAGE_3_ENTRY;
	partition_entry_num[4]		= D_NF_STORAGE_4_ENTRY;
	partition_entry_num[5]		= D_NF_STORAGE_5_ENTRY;
	partition_entry_num[6]		= D_NF_STORAGE_6_ENTRY;






	info->Code_Size					= code_size;

	info->devInfo[0].top			= partition_top[0];
	info->devInfo[1].top			= partition_top[1];
	info->devInfo[2].top			= partition_top[2];
	info->devInfo[3].top			= partition_top[3];
	info->devInfo[4].top			= partition_top[4];
	info->devInfo[5].top			= partition_top[5];
	info->devInfo[6].top			= partition_top[6];

	info->devInfo[0].size			= partition_size[0];
	info->devInfo[1].size			= partition_size[1];
	info->devInfo[2].size			= partition_size[2];
	info->devInfo[3].size			= partition_size[3];
	info->devInfo[4].size			= partition_size[4];
	info->devInfo[5].size			= partition_size[5];
	info->devInfo[6].size			= partition_size[6];

	info->formatInfo[0].formatType	= partition_fat[0];
	info->formatInfo[1].formatType	= partition_fat[1];
	info->formatInfo[2].formatType	= partition_fat[2];
	info->formatInfo[3].formatType	= partition_fat[3];
	info->formatInfo[4].formatType	= partition_fat[4];
	info->formatInfo[5].formatType	= partition_fat[5];
	info->formatInfo[6].formatType	= partition_fat[6];

	info->formatInfo[0].clusterSize	= partition_cluster_size[0];
	info->formatInfo[1].clusterSize	= partition_cluster_size[1];
	info->formatInfo[2].clusterSize	= partition_cluster_size[2];
	info->formatInfo[3].clusterSize	= partition_cluster_size[3];
	info->formatInfo[4].clusterSize	= partition_cluster_size[4];
	info->formatInfo[5].clusterSize	= partition_cluster_size[5];
	info->formatInfo[6].clusterSize	= partition_cluster_size[6];

	info->formatInfo[0].entryNum	= partition_entry_num[0];
	info->formatInfo[1].entryNum	= partition_entry_num[1];
	info->formatInfo[2].entryNum	= partition_entry_num[2];
	info->formatInfo[3].entryNum	= partition_entry_num[3];
	info->formatInfo[4].entryNum	= partition_entry_num[4];
	info->formatInfo[5].entryNum	= partition_entry_num[5];
	info->formatInfo[6].entryNum	= partition_entry_num[6];

	info->Param_Blk_Top				= param_blk_top;
	info->Param_Blk_Size			= param_blk_size;
	info->Exception_Log_Top			= exception_log_top;
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Start_Abort_Timer
------------------------------------------------------------------*/
UINT32 Media_Custom_NF_Start_Abort_Timer(UCHAR exception_flg)
{
	return (UINT32)BF_Fs_User_Custom_Start_Abort_Timer((FW_UCHAR)exception_flg);
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Check_Abort_Timer
------------------------------------------------------------------*/
INT32 Media_Custom_NF_Check_Abort_Timer(UINT32 timout, UINT32 start_time, UCHAR exception_flg)
{
	return (INT32)BF_Fs_User_Custom_Check_Abort_Timer((FW_ULONG)timout, (FW_ULONG)start_time, (FW_UCHAR)exception_flg);
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Set_RAMPD
  ------------------------------------------------------------------*/
VOID Media_Custom_NF_Set_RAMPD(UINT32 pd_flg)
{
	Dd_Exs_Set_RAM_PD_Ctrl_NF(pd_flg);
	Media_Custom_Sleep_Task(D_NF_RAM_PD_WAIT_TIME);
}

/*------------------------------------------------------------------
    function : Media_Custom_NF_Get_INIT_FAIL
  ------------------------------------------------------------------*/
ULONG Media_Custom_NF_Get_INIT_FAIL(VOID)
{
	return Dd_Exs_Get_NF_BTSQ_STATE_INIT_FAIL();
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_GIC_Ctrl
  ------------------------------------------------------------------*/
INT32 Media_Custom_EMMC_GIC_Ctrl(VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_EMMC_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
	return 0;
#else	// for Linux Co-op
	return 0;
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_GIC_Ctrl_Dis
  ------------------------------------------------------------------*/
INT32 Media_Custom_EMMC_GIC_Ctrl_Dis(VOID)
{
// MILB_Porting
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	OS_User_DisableInt((OS_USER_INTID)E_DD_GIC_INTID_EMMC_INT);
	return 0;
#else	// for Linux Co-op
	return 0;
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Drive_Ctrl
  ------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Drive_Ctrl(UINT32 dbcnt)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	Dd_Top_Set_DBCNT4_T48DC(dbcnt);
	Dd_Top_Set_DBCNT4_T49DC(dbcnt);
	Dd_Top_Set_DBCNT4_T50DC(dbcnt);
	Dd_Top_Set_DBCNT4_T51DC(dbcnt);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Signal_Ctrl
  ------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Signal_Ctrl()
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	Dd_Exs_Set_EM_PV_DTIMEC(D_EM_PV_DTIMEC);
	Dd_Exs_Set_EM_PV_AMPBL(D_EM_PV_AMPBL);
	Dd_Exs_Set_EM_CR_SLOTTYPE(D_EM_CR_SLOTTYPE);
	Dd_Exs_Set_EM_CR_BCLKFREQ(D_EM_CR_BCLKFREQ);
	Dd_Exs_Set_EM_CDDET_EMCD(D_EM_CDDET);
	Media_Custom_Wait_ns(100);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Set_Clock
  ------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Set_Clock(ULONG clock)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	BYTE clksel;

	if(clock >= D_EM_CLK_187_5M) {
		clksel = 0;
	}
	else if(clock >= D_EM_CLK_166_6M) {
		clksel = 1;
	}
	else if(clock >= D_EM_CLK_150_0M) {
		clksel = 2;
	}
	else {
		clksel = 3;
	}

	Dd_Top_Set_CLKSTOP2_EMMCCK(1);
	Dd_Top_Set_CLKSEL1_EMMCCLK(clksel);
	Dd_Top_Set_CLKSTOP2_EMMCCK(0);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Get_Clock
  ------------------------------------------------------------------*/
ULONG Media_Custom_EMMC_Get_Clock(VOID)
{
	return Dd_Top_Get_CLKSEL1_EMMCCLK();
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Start_Clock
  ------------------------------------------------------------------*/
INT32 Media_Custom_EMMC_Start_Clock(VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	int ercd;

	ercd = Media_Custom_Twai_Sem(gEMMC_SID, D_SEM_TIMEOUT);
	if (ercd != 0) {
		Media_Custom_Err_Printer("EMMC : Media_Custom_EM_Start_Clock(). Start eMMC clk Sem wait err(%d).\n", ercd);
		return ercd;
	}
#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Start_EMMC_Clock();
#endif
#endif	// for Linux Co-op
	return 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Start_Clock_Direct
  ------------------------------------------------------------------*/
INT32 Media_Custom_EMMC_Start_Clock_Direct(VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Start_EMMC_Clock();
#endif
#endif	// for Linux Co-op
	return 0;
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Stop_Clock
  ------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Stop_Clock(VOID)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
#ifdef CO_MEDIA_CLK_CTL
	BF_Fs_User_Custom_Stop_EMMC_Clock();
#endif
	Media_Custom_Sig_Sem(gEMMC_SID);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Set_SoftReset
  ------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Set_SoftReset(UINT32 val)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	Dd_Exs_Set_eMMC_Soft_Reset(val);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Set_RAMPD
  ------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Set_RAMPD(UINT32 slp_flg)
{
#ifdef CO_EMMC_DRIVER_ENABLE // for Linux Co-op
	Dd_Exs_Set_RAM_PD_Ctrl_EMMC(slp_flg);
	Media_Custom_Sleep_Task(D_EM_RAM_PD_WAIT_TIME);
#endif	// for Linux Co-op
}

/*------------------------------------------------------------------
    function : Media_Custom_EMMC_Set_Partition_Info
------------------------------------------------------------------*/
VOID Media_Custom_EMMC_Set_Partition_Info(T_EM_Partition_Info* info)
{
	info->Code_Size					= media_custom_get_partition_size();

	info->storage_num				= D_EM_STORAGE_NUM;
	info->storage_mbr				= info->Code_Size;
	info->devInfo[0].top			= (info->storage_mbr + D_EM_STORAGE_MBR_SIZE);
	info->devInfo[1].top			= (info->devInfo[0].top + D_EM_STORAGE_0_SIZE);
	info->devInfo[2].top			= (info->devInfo[1].top + D_EM_STORAGE_1_SIZE);
	info->devInfo[3].top			= (info->devInfo[2].top + D_EM_STORAGE_2_SIZE);
	info->devInfo[4].top			= (info->devInfo[3].top + D_EM_STORAGE_3_SIZE);
	info->devInfo[5].top			= (info->devInfo[4].top + D_EM_STORAGE_4_SIZE);
	info->devInfo[6].top			= (info->devInfo[5].top + D_EM_STORAGE_5_SIZE);
	info->Param_Blk_Top				= (info->devInfo[6].top + D_EM_STORAGE_6_SIZE);
	info->Exception_Log_Top			= (info->Param_Blk_Top  + D_EM_PARAMETER_SIZE);

	info->devInfo[0].size			= D_EM_STORAGE_0_SIZE;
	info->devInfo[1].size			= D_EM_STORAGE_1_SIZE;
	info->devInfo[2].size			= D_EM_STORAGE_2_SIZE;
	info->devInfo[3].size			= D_EM_STORAGE_3_SIZE;
	info->devInfo[4].size			= D_EM_STORAGE_4_SIZE;
	info->devInfo[5].size			= D_EM_STORAGE_5_SIZE;
	info->devInfo[6].size			= D_EM_STORAGE_6_SIZE;
	info->Param_Blk_Size			= D_EM_PARAMETER_SIZE;

	info->formatInfo[0].formatType	= D_EM_STORAGE_0_FAT;
	info->formatInfo[1].formatType	= D_EM_STORAGE_1_FAT;
	info->formatInfo[2].formatType	= D_EM_STORAGE_2_FAT;
	info->formatInfo[3].formatType	= D_EM_STORAGE_3_FAT;
	info->formatInfo[4].formatType	= D_EM_STORAGE_4_FAT;
	info->formatInfo[5].formatType	= D_EM_STORAGE_5_FAT;
	info->formatInfo[6].formatType	= D_EM_STORAGE_6_FAT;

	info->formatInfo[0].clusterSize	= D_EM_STORAGE_0_CLUSTER;
	info->formatInfo[1].clusterSize	= D_EM_STORAGE_1_CLUSTER;
	info->formatInfo[2].clusterSize	= D_EM_STORAGE_2_CLUSTER;
	info->formatInfo[3].clusterSize	= D_EM_STORAGE_3_CLUSTER;
	info->formatInfo[4].clusterSize	= D_EM_STORAGE_4_CLUSTER;
	info->formatInfo[5].clusterSize	= D_EM_STORAGE_5_CLUSTER;
	info->formatInfo[6].clusterSize	= D_EM_STORAGE_6_CLUSTER;

	info->formatInfo[0].entryNum	= D_EM_STORAGE_0_ENTRY;
	info->formatInfo[1].entryNum	= D_EM_STORAGE_1_ENTRY;
	info->formatInfo[2].entryNum	= D_EM_STORAGE_2_ENTRY;
	info->formatInfo[3].entryNum	= D_EM_STORAGE_3_ENTRY;
	info->formatInfo[4].entryNum	= D_EM_STORAGE_4_ENTRY;
	info->formatInfo[5].entryNum	= D_EM_STORAGE_5_ENTRY;
	info->formatInfo[6].entryNum	= D_EM_STORAGE_6_ENTRY;
}

