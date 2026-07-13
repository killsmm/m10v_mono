/**
 * @file		boot.c
 * @brief		boot process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
 
#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "dd_arm.h"
#include "dd_top.h"
#include "dd_gpv.h"
#include "dd_gic.h"
#include "im_mxic.h"
#include "dd_timestamp.h"
#include "boot.h"
#include "bf_startup.h"
#include "mba_if.h"
#include "assertion.h"
#include "debug.h"
#include "bus_arb.h"
#include "sdram_map_manager.h"
#include "ipcu.h"
#include "dd_exs.h"
extern VOID init_io_set(VOID);

#ifdef CO_FAST_BOOT_ON
extern unsigned int __first_sec_start;
extern unsigned int __first_sec_end;
extern void BF_Boot_Loader_Load_First(char* dstStartAdr, ULONG boot_size);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
extern void BF_Boot_Loader_Load_First_emmc(char* dstStartAdr, ULONG boot_size);
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#endif

#ifdef CO_TKERNEL_ONLY
#define CO_BOOT_LOADER_NF

#ifdef CO_BOOT_LOADER_NF
extern void BF_Boot_Loader_Load_PartitionData(void);
#else
extern void BF_Boot_Loader_Load_PartitionData_emmc(void);
#endif
#endif

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define ADDRESS_FOR_CONDITION_OF_RESET		(0x0100FFFC)	/* Temporary: last of internal SRAM */
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#ifdef CO_MEMORY_1GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_1GB BEGIN ---
#define D_BOOT_MAGIC_NUM_NF_CONTROL			(0x46890040)
// --- REMOVE_MEMORY_1GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_1GB
#ifdef CO_MEMORY_2GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_2GB BEGIN ---
#define D_BOOT_MAGIC_NUM_NF_CONTROL			(0x4FE90040)	// temp
// --- REMOVE_MEMORY_2GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_2GB
#ifdef CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_3GB BEGIN ---
#define D_BOOT_MAGIC_NUM_NF_CONTROL			(0x4FE90040)	// temp
// --- REMOVE_MEMORY_3GB END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
// --- REMOVE_MEMORY_SIZE_OPT BEGIN ---
//#ifdef CO_MEMORY_2GB
//#define D_BOOT_MAGIC_NUM_NF_CONTROL			(0x4FE90040)
//#endif	// CO_MEMORY_2GB
//#ifdef CO_MEMORY_3GB
//#define D_BOOT_MAGIC_NUM_NF_CONTROL			(0x4FE90040)
//#endif	// CO_MEMORY_3GB
// --- REMOVE_MEMORY_SIZE_OPT END ---
#define D_BOOT_NF_USE						(0xBEEFCAFE)
#define D_BOOT_NF_NOT_USE					(0xFEEDBACF)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
static VOID bf_boot_mxic_dec_err_handler0(UCHAR ch);
static VOID bf_boot_mxic_dec_err_handler1(UCHAR ch);
static VOID bf_boot_mxic_dec_err_handler2(UCHAR ch);
static VOID bf_boot_mxic_dec_err_handler3(UCHAR ch);
static VOID bf_boot_mxic_dec_err_handler4(UCHAR ch);
static VOID bf_boot_mxic_dec_err_handler5(UCHAR ch);
static VOID bf_boot_mxic_dec_err_handler6(UCHAR ch);
// MXIC Dec Error handler
static const IM_MXIC_DEC_ERR_CALLBACK	bf_boot_mxic_dec_err_handler_tbl[E_IM_MXIC_UNIT_MAX] = {
	bf_boot_mxic_dec_err_handler0,
	bf_boot_mxic_dec_err_handler1,
	bf_boot_mxic_dec_err_handler2,
	bf_boot_mxic_dec_err_handler3,
	bf_boot_mxic_dec_err_handler4,
	bf_boot_mxic_dec_err_handler5,
	bf_boot_mxic_dec_err_handler6
};
// MXIC master table
static const CHAR* const bf_boot_mxic_master_table[E_IM_MXIC_UNIT_MAX][44] = {
	// unit0
	{
	"Reserve ", "SEN0    ", "SEN1    ", "SEN2    ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve "
	},
	// unit1
	{
	"Reserve ", "SRO     ", "B2B     ", "B2RA    ",
	"B2RB    ", "B2RC    ", "LTMDT   ", "LTMR    ",
	"LTMG    ", "LTMB    ", "R2YA    ", "R2YB    ",
	"R2YC    ", "R2YD    ", "CNRA    ", "CNRB    ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve "
	},
	// unit2
	{
	"Reserve ", "SRO     ", "B2B     ", "B2RA    ",
	"B2RB    ", "B2RC    ", "LTMDT   ", "LTMR    ",
	"LTMG    ", "LTMB    ", "R2YA    ", "R2YB    ",
	"R2YC    ", "R2YD    ", "CNRA    ", "CNRB    ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve "
	},
	// unit3
	{
	"Reserve ", "STAT    ", "IIPA    ", "IIPB    ",
	"IIPC    ", "SHDRA   ", "SHDRB   ", "SHDRC   ",
	"SHDRD   ", "ME      ", "IPU_A   ", "IPU_B   ",
	"IPU_C   ", "IPU_D   ", "IPU_E   ", "DSP_A   ",
	"DSP_B   ", "GPU     ", "DW      ", "JPEG    ",
	"RAW     ", "none    ", "Group0_0", "Group0_1",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "XCH     ", "FPT0    ",
	"FPT1    ", "Reserve ", "Reserve ", "Reserve "
	},
	// unit4
	{
	"Reserve ", "DISPA   ", "DISPB   ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve "
	},
	// unit5
	{
	"Reserve ", "EXS0    ", "EXS1    ", "NIC     ",
	"BMH0    ", "BMH1    ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve "
	},
	// unit6
	{
	"Reserve ", "CA7     ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve ",
	"Reserve ", "Reserve ", "Reserve ", "Reserve "
	}
};
static ULONG  gbf_boot_mxic_spin_lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile UINT32 gbf_boot_mxic_dec_err_count[E_IM_MXIC_UNIT_MAX];


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
#ifdef CO_MEASURE_TIMER
static VOID start_measure_timer(VOID)
{
#ifdef CO_TKERNEL_ONLY
	T_DD_TIMESTAMP_CTRL		timestamp_ctrl;
	
	timestamp_ctrl.hdbg = 0;						// has no effect
	timestamp_ctrl.counter = 0x0000000000000000;	// Counter
	timestamp_ctrl.frequency = 0x2625A00ULL;		// 40MHz ( tentative )
	
	Dd_TIMESTAMP_Init();
	if (Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL) == D_DDIM_OK) {
		Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
		Dd_TIMESTAMP_Start();
	}
#endif	// CO_TKERNEL_ONLY
}
#endif	// CO_MEASURE_TIMER


#ifdef CO_TKERNEL_ONLY
static VOID bf_boot_init_persel(VOID)
{
	// PERSEL Setting
	IO_CHIPTOP.PERSEL1.word	= 0x00000000;
	IO_CHIPTOP.PERSEL2.word	= 0x00000000;
	IO_CHIPTOP.PERSEL3.word	= 0x00000000;
	IO_CHIPTOP.PERSEL4.word	= 0x00000000;
#ifdef __GNUC__
	__asm__ volatile ("DMB 0xF");
#else
	__dmb(0xF);
#endif
}
#endif	// CO_TKERNEL_ONLY


#ifdef CO_TKERNEL_ONLY
static VOID bf_boot_init_clock(VOID)
{
#if 0 //Look at mcr file
	// Clock Disable Setting
	// Clock OFF
	IO_CHIPTOP.CLKSTOP1.word = 0x33FFFFFF;
	IO_CHIPTOP.CLKSTOP2.word = 0xFFFFFFFF;
	IO_CHIPTOP.CLKSTOP3.word = 0xFFFFFFFF;
	IO_CHIPTOP.CLKSTOP4.word = 0xFFFFFFFF;
	IO_CHIPTOP.CLKSTOP5.word = 0x0FFFFFFF;
	IO_CHIPTOP.CLKSTOP6.word = 0x0FFFFFFF;
	IO_CHIPTOP.CLKSTOP7.word = 0x3FFFFFFF;
	IO_CHIPTOP.CLKSTOP8.word = 0xFFFFFFFF;
	IO_CHIPTOP.CLKSTOP9.word = 0x3FFFFFFF;
	IO_CHIPTOP.CLKSTOP10.word = 0x0FFFFFFF;
	IO_CHIPTOP.CLKSTOP11.word = 0x0FFFFFFF;
	IO_CHIPTOP.CLKSTOP12.word = 0x0FFFFFFF;
	IO_CHIPTOP.CLKSTOP13.word = 0xFFFFFFFF;
	IO_CHIPTOP.CLKSTOP14.word = 0xFCFFFFFF;
#ifdef __GNUC__
	__asm__ volatile ("DMB 0xF");
#else
	__dmb(0xF);
#endif
	
	// CLKSEL Setting
	IO_CHIPTOP.CLKSEL1.word = 0x48221084;
	IO_CHIPTOP.CLKSEL2.word = 0x488888C8;
	IO_CHIPTOP.CLKSEL3.word = 0x14511488;
	IO_CHIPTOP.CLKSEL4.word = 0x88102040;
	IO_CHIPTOP.CLKSEL5.word = 0x00010820;
	IO_CHIPTOP.CLKSEL6.word = 0x88102040;
	IO_CHIPTOP.CLKSEL7.word = 0x00010820;
	IO_CHIPTOP.CLKSEL8.word = 0x55415524;
	IO_CHIPTOP.CLKSEL9.word = 0x00BBBBCF;
	IO_CHIPTOP.CLKSEL10.word = 0x000002DD;
	IO_CHIPTOP.CLKSEL11.word = 0x00000000;
	IO_CHIPTOP.CLKSEL12.word = 0x22148812;
#ifdef __GNUC__
	__asm__ volatile ("DMB 0xF");
#else
	__dmb(0xF);
#endif
	
	// Clock Enable Setting
	// Clock ON
	IO_CHIPTOP.CLKSTOP1.word = 0x22AAAAAA;
	IO_CHIPTOP.CLKSTOP2.word = 0xAAAAAAAA;
	IO_CHIPTOP.CLKSTOP3.word = 0xAAAAAAAA;
	IO_CHIPTOP.CLKSTOP4.word = 0xAAAAAAAA;
	IO_CHIPTOP.CLKSTOP5.word = 0x0AAAAAAA;
	IO_CHIPTOP.CLKSTOP6.word = 0x0AAAAAAA;
	IO_CHIPTOP.CLKSTOP7.word = 0x2AAAAAAA;
	IO_CHIPTOP.CLKSTOP8.word = 0xAAAAAAAA;
	IO_CHIPTOP.CLKSTOP9.word = 0x2AAAAAAA;
	IO_CHIPTOP.CLKSTOP10.word = 0x0AAAAAAA;
	IO_CHIPTOP.CLKSTOP11.word = 0x0AAAAAAA;
	IO_CHIPTOP.CLKSTOP12.word = 0x0AAAAAAA;
	IO_CHIPTOP.CLKSTOP13.word = 0xAAAAAAAA;
	IO_CHIPTOP.CLKSTOP14.word = 0xA4AAAAAA;
#ifdef __GNUC__
	__asm__ volatile ("DMB 0xF");
#else
	__dmb(0xF);
#endif
#endif //Look at mcr file
}
#endif	// CO_TKERNEL_ONLY

static VOID bf_boot_disable_mxic_dec_err_int(E_IM_MXIC_UNIT unit)
{
	T_IM_MXIC_DEC_ERR_INT	cfg_dec_err;

	cfg_dec_err.w_err_int_en	= 0;
	cfg_dec_err.r_err_int_en	= 0;
	cfg_dec_err.pCallBack		= NULL;

	Im_MXIC_Set_Decode_Error_Int( unit, &cfg_dec_err );
}

static VOID bf_boot_enable_mxic_dec_err_int(E_IM_MXIC_UNIT unit)
{
	T_IM_MXIC_DEC_ERR_INT	cfg_dec_err;

	cfg_dec_err.w_err_int_en	= 1;
	cfg_dec_err.r_err_int_en	= 1;
	cfg_dec_err.pCallBack		= bf_boot_mxic_dec_err_handler_tbl[unit];

	Im_MXIC_Set_Decode_Error_Int( unit, &cfg_dec_err );
}

static VOID bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT unit, CHAR ch)
{
	T_IM_MXIC_DEC_ERR	dec_err_info;

	Dd_ARM_Critical_Section_Start( gbf_boot_mxic_spin_lock );

	bf_boot_disable_mxic_dec_err_int( unit );

	Im_MXIC_Get_Decode_Error( unit, &dec_err_info );

	if ( ch == D_IM_MXIC_DEC_ERR_CH_W ) {
		BF_Debug_Print_Error(( " DECERR(Wch) : unit = %u , master(%u) = %s, req addr = 0x%x\n" ,
							  unit,
							  dec_err_info.w_req_master,
							  bf_boot_mxic_master_table[(UINT32)unit][(UINT32)dec_err_info.w_req_master],
							  dec_err_info.w_req_addr
							 ));
	} else {
		BF_Debug_Print_Error(( " DECERR(Rch) : unit = %u , master(%u) = %s, req addr = 0x%x\n" ,
							  unit,
							  dec_err_info.r_req_master,
							  bf_boot_mxic_master_table[(UINT32)unit][(UINT32)dec_err_info.r_req_master],
							  dec_err_info.r_req_addr
							 ));
	}

	if( gbf_boot_mxic_dec_err_count[unit] <= 1 ) {
		gbf_boot_mxic_dec_err_count[unit]++;
//		bf_boot_enable_mxic_dec_err_int( unit );	// Assert data abort if enable this line. Why?
	}
	else {
		// for Interrupt storm.
		bf_boot_disable_mxic_dec_err_int( unit );
	}

	Dd_ARM_Critical_Section_End( gbf_boot_mxic_spin_lock );
}
static VOID bf_boot_mxic_dec_err_handler0(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_0, ch);
}
static VOID bf_boot_mxic_dec_err_handler1(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_1, ch);
}
static VOID bf_boot_mxic_dec_err_handler2(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_2, ch);
}
static VOID bf_boot_mxic_dec_err_handler3(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_3, ch);
}
static VOID bf_boot_mxic_dec_err_handler4(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_4, ch);
}
static VOID bf_boot_mxic_dec_err_handler5(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_5, ch);
}
static VOID bf_boot_mxic_dec_err_handler6(UCHAR ch)
{
	bf_boot_mxic_dec_err_handler(E_IM_MXIC_UNIT_6, ch);
}

static VOID bf_boot_init_mxic_decerr(VOID)
{
	E_IM_MXIC_UNIT	unit;

	for ( unit = E_IM_MXIC_UNIT_0; unit < E_IM_MXIC_UNIT_MAX; unit++ ) {
		gbf_boot_mxic_dec_err_count[unit] = 0;
		// MXIC Error Interrupt Setting
		bf_boot_enable_mxic_dec_err_int( unit );
	}
}

static VOID bf_boot_init_mxic(VOID)
{
	E_IM_MXIC_UNIT	unit;

	for ( unit = E_IM_MXIC_UNIT_0; unit < E_IM_MXIC_UNIT_MAX; unit++ ) {
		// MXIC soft reset is set by M0 process.
		Im_MXIC_Init(unit);
	}

	Sdram_Map_Mgr_Init_MxicSlaveRegion();

	bf_boot_init_mxic_decerr();
}


/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
VOID boot(INT32 stacd)
{
#ifdef CO_MEASURE_BOOTUP_TIME
	// LED10 ON P24
	*((volatile unsigned int*)0x1DFFB104) |= 0x10;			// DDR32
	*((volatile unsigned int*)0x1DFFB204) &= 0x10;			// EPCR32
	*((volatile unsigned int*)0x1DFFB004) = 0x00100010;		// PDR32
#endif

	// Disable Overcurrent Detection of USB3.0
	{
		uint32_t *reg_addr;
		reg_addr  = (uint32_t *)0x1b110298;
		*reg_addr = 0x00000020;
	}
	
#if defined(ETHER_EN) 
	//Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP( 0 );
	//Dd_Exs_Set_NETSEC_Soft_Reset( 1 );
#endif	// ETHER_EN

	memset( ( FW_VOID* )&bf_ipcu_buffer, 0, sizeof( bf_ipcu_buffer ));
	memset( ( FW_VOID* )&bf_ipcu_sync, 0, sizeof( bf_ipcu_sync ));


#ifdef CO_MEASURE_TIMER
	start_measure_timer();
#endif	// CO_MEASURE_TIMER
	BF_Measure_Time_Init();

	M_MBA_START();

	fj_busarb_set_sdramcbootdefault();

	bf_boot_init_mxic();

	fj_busarb_set_mxicbootdefault();

	Assertion_Log_Init();

	BF_Startup_Boot_Sample();
}


FJ_ERR_CODE fj_getbootcond( LONG* bootCond )
{
	if( NULL == bootCond ) {
		return FJ_ERR_NG;
	}
	*bootCond = *((volatile LONG *)ADDRESS_FOR_CONDITION_OF_RESET);
	return FJ_ERR_OK;
}


VOID fj_reboot_pre( VOID )
{
	M_MBA_END();

#ifdef CO_MEASURE_TIMER
#ifdef CO_TKERNEL_ONLY
	// Stop measure timer
	Dd_TIMESTAMP_Stop();
	Dd_TIMESTAMP_Close();
#endif
#endif

//	OS_User_Ter_Tsk( TID_BASEFW );

	// Disable IRQ & FIQ
	Dd_ARM_DIF();
}


/**
 * @brief Reboot(software reset)
 * @param None
 * @return None
 * @note software reset for reboot
 *		   WATCH DOG Setting -> Software Reset --> Watch Dog Reset
 *
 * @attention None
*/
VOID fj_reboot(VOID)
{
	IO_CHIPTOP.CLKSTOP1.word  = 0x22aaaaaa;
	IO_CHIPTOP.CLKSTOP2.word  = 0xaaaaaaaa;
	IO_CHIPTOP.CLKSTOP3.word  = 0xaaaaaaaa;
	IO_CHIPTOP.CLKSTOP4.word  = 0xffffffaa;
	IO_CHIPTOP.CLKSTOP5.word  = 0x0aaaaaaa;
	IO_CHIPTOP.CLKSTOP6.word  = 0x0aaaaaaa;
	IO_CHIPTOP.CLKSTOP7.word  = 0x2aaaaaaa;
	IO_CHIPTOP.CLKSTOP8.word  = 0xaaaaaaaa;
	IO_CHIPTOP.CLKSTOP9.word  = 0x2aaaaaaa;
	IO_CHIPTOP.CLKSTOP10.word = 0x0aaaaaaa;
	IO_CHIPTOP.CLKSTOP11.word = 0x0aaaaaaa;
	IO_CHIPTOP.CLKSTOP12.word = 0x0aaaaaaa;
	IO_CHIPTOP.CLKSTOP13.word = 0xaaaaaaaa;
	IO_CHIPTOP.CLKSTOP14.word = 0xaaaaaaaa;
	IO_CHIPTOP.PLLCNT2.word = 0x000015A6;
	IO_CHIPTOP.PLLCNT3.word = 0x00010001;
	IO_CHIPTOP.PLLCNT4.word = 0x00000000;
	IO_CHIPTOP.PLLCNT5.word = 0x00000000;
	IO_CHIPTOP.PLLCNT6.word = 0x10A410A4;
	IO_CHIPTOP.PLLCNT7.word = 0x00000011;
	IO_CHIPTOP.PLLCNT8.word = 0x00000000;
	IO_CHIPTOP.PLLCNT9.word = 0x00000000;
	IO_CHIPTOP.PLLCNT1.word = 0x000015C6;
	Dd_Top_Set_CRSWR_SRREQ();
}

VOID BF_Boot_Enable_MxicDecErr( VOID )
{
	bf_boot_init_mxic_decerr();
}

