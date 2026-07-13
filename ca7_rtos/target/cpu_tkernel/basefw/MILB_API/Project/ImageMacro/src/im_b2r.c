/**
 * @file		im_b2r.c
 * @brief		Bayer to RGB comversion
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "im_b2r.h"

#include <string.h>
//#if defined(CO_ACT_CLOCK) || defined(CO_ACT_ICLOCK) || defined(CO_ACT_HCLOCK) || defined(CO_ACT_PCLOCK)
#include "dd_top.h"
//#endif
#include "dd_arm.h"

#include "jdsb2r.h"
//#include "im_rdma.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

#if defined(CO_ACT_CLOCK) || defined(CO_ACT_IMAGE_CLOCK)
#define CO_ACT_B2R_CLOCK
#endif	// CO_ACT_PCLOCK
#if defined(CO_ACT_HCLOCK) || defined(CO_ACT_IMAGE_HCLOCK)
#define CO_ACT_B2R_HCLOCK
#endif	// CO_ACT_HCLOCK
#if defined(CO_ACT_PCLOCK) || defined(CO_ACT_IMAGE_PCLOCK)
#define CO_ACT_B2R_PCLOCK
#endif	// CO_ACT_PCLOCK
#if defined(CO_ACT_ICLOCK) || defined(CO_ACT_IMAGE_ICLOCK)
#define CO_ACT_B2R_ICLOCK
#endif	// CO_ACT_ICLOCK

//#define CO_B2R_RDMA_ON

#define Im_B2R_Dsb()	Dd_ARM_Dsb_Pou()

#define D_IM_B2R1_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_B2R1_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_B2R1_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())
#define D_IM_B2R1_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP10())

#define D_IM_B2R2_CLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_B2R2_HCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_B2R2_PCLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())
#define D_IM_B2R2_ICLK_REG_ADDR	(&Dd_Top_Get_CLKSTOP12())

#define D_IM_B2R1_CLK_REG_BIT	D_DD_TOP_B2R1CK_BIT
#define D_IM_B2R1_HCLK_REG_BIT	D_DD_TOP_B2R1AH_BIT
#define D_IM_B2R1_PCLK_REG_BIT	D_DD_TOP_B2R1AP_BIT
#define D_IM_B2R1_ICLK_REG_BIT	D_DD_TOP_B2R1AX_BIT

#define D_IM_B2R2_CLK_REG_BIT	D_DD_TOP_B2R2CK_BIT
#define D_IM_B2R2_HCLK_REG_BIT	D_DD_TOP_B2R2AH_BIT
#define D_IM_B2R2_PCLK_REG_BIT	D_DD_TOP_B2R2AP_BIT
#define D_IM_B2R2_ICLK_REG_BIT	D_DD_TOP_B2R2AX_BIT


/* debug print switch  for im_b2r.c developping/debugging person.
 * (not im_b2r user)
 */
//#define IM_B2R_DEBUG_PRINT
//#define IM_B2R_STATUS_PRINT
//#define IM_B2R_REG_TYPE_CHECK

// True/False (UCHAR)
#define D_IM_B2R_TRUE					((UCHAR)1)
#define D_IM_B2R_FALSE					((UCHAR)0)

// PIPE count
#define D_IM_B2R_PIPE_COUNT				(2)

// Driver state of Starting flag
#define D_IM_B2R_STATE_IDLE				(0x00)
#define D_IM_B2R_STATE_BUSY_F_B2R		(0x01)
#define D_IM_B2R_STATE_BUSY_YBR			(0x02)
#define D_IM_B2R_STATE_BUSY_YBW			(0x04)
#define D_IM_B2R_STATE_BUSY_ALL			(D_IM_B2R_STATE_BUSY_F_B2R | D_IM_B2R_STATE_BUSY_YBR | D_IM_B2R_STATE_BUSY_YBW)

// YBINTE value
#define D_IM_B2R_YBINTE_ALL_ENABLE		(0x00003333)	// YBR, YBW, AXI
#define D_IM_B2R_YBINTE_EXT_ONLY_ENABLE	(0x00001111)	// YBR, AXI
#define D_IM_B2R_YBINTE_DISABLE			(0x00000000)
// YBINTF value
#define D_IM_B2R_YBINTF_ALL_CLR			(0x00003333)	// YBR, YBW, AXI

// B2RINTE value
#define D_IM_B2R_B2RINTE_ENABLE			(0x01)
#define D_IM_B2R_B2RINTE_DISABLE		(0x00)

// B2RINTF value
#define D_IM_B2R_B2RINTF_CLR			(0x01)

// register bit pattern
#define D_IM_B2R_FB2RTRG_OFF			(0)
#define D_IM_B2R_FB2RTRG_ON				(1)
#define D_IM_B2R_FB2RTRG_IDLE			(2)
#define D_IM_B2R_FB2RTRG_BUSY			(3)

#define D_IM_B2R_YBRTRG_OFF				(0)
#define D_IM_B2R_YBRTRG_ON				(1)
#define D_IM_B2R_YBRTRG_IDLE			(2)
#define D_IM_B2R_YBRTRG_BUSY			(3)

#define D_IM_B2R_YBWTRG_OFF				(0)
#define D_IM_B2R_YBWTRG_ON				(1)
#define D_IM_B2R_YBWTRG_IDLE			(2)
#define D_IM_B2R_YBWTRG_BUSY			(3)

#define D_IM_B2R_PACK10_MSB_CYCLE_PIXS	(4)
#define D_IM_B2R_PACK12_MSB_CYCLE_PIXS	(2)

// YBRHSIZ register value
#define D_IM_B2R_YBRHSIZ_MIN			(96)
#define D_IM_B2R_YBRHSIZ_MAX			(2360)

// YBRVSIZ register value
#define D_IM_B2R_YBRVSIZ_MIN			(72)
#define D_IM_B2R_YBRVSIZ_MAX			(8960)

// DINHSTA register value
#define D_IM_B2R_DINHSTA_MIN			(0)
#define D_IM_B2R_DINHSTA_MAX			(2262)

// DINVSTA register value
#define D_IM_B2R_DINVSTA_MIN			(0)
#define D_IM_B2R_DINVSTA_MAX			(8886)

// DINHSIZ register value
#define D_IM_B2R_DINHSIZ_MIN			(96)
#define D_IM_B2R_DINHSIZ_MAX			(2360)

// DINVSIZ register value
#define D_IM_B2R_DINVSIZ_MIN			(72)
#define D_IM_B2R_DINVSIZ_MAX			(8960)

// TRMHSTA register value
#define D_IM_B2R_TRMHSTA_MIN			(0)
#define D_IM_B2R_TRMHSTA_MAX			(2262)

// TRMVSTA register value
#define D_IM_B2R_TRMVSTA_MIN			(0)
#define D_IM_B2R_TRMVSTA_MAX			(8886)

// TRMHSIZ register value
#define D_IM_B2R_TRMHSIZ_MIN			(96)
#define D_IM_B2R_TRMHSIZ_MAX			(2360)

// TRMVSIZ register value
#define D_IM_B2R_TRMVSIZ_MIN			(72)
#define D_IM_B2R_TRMVSIZ_MAX			(8960)

// B2R Photo/Video mode(DINSEL.B2BDIN)
#define D_IM_B2R_MODE_SDRAM_INPUT		(0)
#define D_IM_B2R_MODE_B2B_DIRECT		(1)

#define D_IM_B2R_SRAM_WAIT_USEC			(1)

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define im_b2r_inc_bank_index(bank_index, max)	(((bank_index) + 1) % (max))

#define im_b2r_rounddown_2(a)					(((a) >> 1) << 1)					// for 2 byte alignment

#define im_b2r_max( a, b )						(((a) > (b)) ? (a):(b))
#define im_b2r_min( a, b )						(((a) < (b)) ? (a):(b))

#define im_b2r_check_target_pipe_no_1(a)		(((a) + 1) & (D_IM_B2R_PIPE1 + 1))
#define im_b2r_check_target_pipe_no_2(a)		(((a) + 1) & (D_IM_B2R_PIPE2 + 1))


// The structure type matching check.
#ifdef IM_B2R_REG_TYPE_CHECK
#define im_b2r_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_b2r_check_reg_type( dst, src )
#endif



// Signed register access macros.
#define im_b2r_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_b2r_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}

#define im_b2r_set_reg_signed_a( reg, type, member, val )		\
		{ \
			type work = { .word[0] = 0xFFFFFFFFUL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_b2r_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}


// wait usec
#define im_b2r_wait_usec( usec )				Dd_ARM_Wait_ns( (usec) * 1000 )


#ifdef CO_ASSERT_ON
#define M_IM_B2R_ASSETION_MSG( msg )		(msg)
#else
#define M_IM_B2R_ASSETION_MSG( msg )		(NULL)
#endif


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

// Ring pixel information
typedef struct {
	UCHAR	pad_enable;
	UCHAR	ring_pixs;
	UCHAR	pad_pixs;
	UCHAR	ref_pixs;
} T_IM_B2R_RING_PIXS;

// control information
typedef struct {
	UCHAR					was_started;					// 1st call Im_B2R_Start() after Im_B2R_Stop() or Im_B2R_Init()
	UCHAR					video_photo_mode;				// Now operation mode(Photo/Video)
	UCHAR					busy_state;						// Starting state for YBR/YBW/F_B2R
	UCHAR					cia_bypass;						// CIA bypass mode
	UCHAR					cia_padding;					// CIA padding mode
	UINT32					int_status;						// Interrupt status
	VOID					(*b2r_user_handler)(UINT32* int_status, UINT32 user_param);	// Interrupt Handler
	UINT32					user_param;						// return to callback argument value when interrupt occurs
	UCHAR					input_dtype;					// input dtype
	UCHAR					bayer_top_pixel;				// Color of Bayer top pixel
	ULONG					in_addr;						// input image address
	USHORT					input_global;					// bayer global width(16bytes align)
	UCHAR					top_offset;						// Bayer Top area bit offset
	T_IM_B2R_RING_PIXS		ring_pixs_info;					// Ring pixel information
	T_IM_B2R_IN_RECT		input_size;						// YBR input size
	T_IM_B2R_CTRL_TRIMMING	trim;							// YBW trimming
} T_IM_B2R_STATE_MNG;

// Output address management information
typedef struct {
	T_IM_B2R_OUTBANK_INFO	bank_info;		// Output bank information
	UCHAR					latest_idx;		// Latest Output bank index.
	T_IM_B2R_OUTADDR_INFO	latest_addr;	// Latest Output address.
} T_IM_B2R_OUTPUT_MNG;

typedef struct {
	USHORT	ofs_x_pixs;
	USHORT	ofs_y_pixs;
	ULONG	ofs_bytes;
} T_IM_B2R_YBRA_OFS_INFO;

// Interrupt parameter table
typedef struct {
	ULONG				flg_bitmask;		// Bitmask of interrupt flag.
	ULONG				ena_bitmask;		// Bitmask of inaerrupt enable.
	ULONG				waitflg;			// Value of set_flg.
	ULONG				int_status;			// Value of gIM_B2R_State.int_status.
	ULONG				busy_state;			// Value of gIM_B2R_State.busy_state.
} T_IM_B2R_INTFLG_TBL;

// Access enable register management
typedef struct {
	UCHAR	ctrl_cnt[D_IM_B2R_PIPE_COUNT];
	VOID	(*reg_set_func)( UCHAR pipe_no, UCHAR enable );
} T_IM_B2R_ACCESS_ENABLE_MANAGE;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// B2R status
static volatile T_IM_B2R_STATE_MNG gIM_B2R_State[D_IM_B2R_PIPE_COUNT];

// rectangle parameter
static volatile T_IM_B2R_RECT gIM_B2R_Rect_Param[D_IM_B2R_PIPE_COUNT];

// Output parameter
static volatile T_IM_B2R_OUTPUT_MNG gIM_B2R_Out_Mng[D_IM_B2R_PIPE_COUNT];

// YBRA offset information
static volatile T_IM_B2R_YBRA_OFS_INFO gIM_B2R_ybra_ofs_info[D_IM_B2R_PIPE_COUNT];

static volatile struct io_b2r* gIM_Io_B2r_Reg_Ptr[D_IM_B2R_PIPE_MAX] = { &IO_B2R_P1, &IO_B2R_P2, &IO_B2R_P3 };
#ifndef CO_B2R_RDMA_ON
static volatile struct io_b2r_sram* gIM_Io_B2r_Tbl_Ptr[D_IM_B2R_PIPE_MAX] = { &IO_B2R_TBL_P1, &IO_B2R_TBL_P2, &IO_B2R_TBL_P3 };
#endif	// CO_B2R_RDMA_ON


// Clock control counter
#ifdef CO_ACT_B2R_CLOCK
static volatile UCHAR gIM_B2R_Clk_Ctrl_Cnt[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_CLOCK
#ifdef CO_ACT_B2R_HCLOCK
static volatile UCHAR gIM_B2R_Hclk_Ctrl_Cnt[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_HCLOCK
#ifdef CO_ACT_B2R_PCLOCK
static volatile UCHAR gIM_B2R_Pclk_Ctrl_Cnt[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_PCLOCK
#ifdef CO_ACT_B2R_ICLOCK
static volatile UCHAR gIM_B2R_Iclk_Ctrl_Cnt[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_ICLOCK


// clock off control is disabled(for register dump on debugger)
#ifdef CO_ACT_B2R_CLOCK
static volatile UINT32 gIM_B2R_Clk_Ctrl_Disable[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_CLOCK
#ifdef CO_ACT_B2R_HCLOCK
static volatile UINT32 gIM_B2R_Hclk_Ctrl_Disable[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_HCLOCK
#ifdef CO_ACT_B2R_PCLOCK
static volatile UINT32 gIM_B2R_Pclk_Ctrl_Disable[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_PCLOCK
#ifdef CO_ACT_B2R_ICLOCK
static volatile UINT32 gIM_B2R_Iclk_Ctrl_Disable[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif	// CO_ACT_B2R_ICLOCK


#ifdef D_IM_B2R_DEBUG_ON_PC
volatile UCHAR  gIM_B2R_macro_fake_finish[D_IM_B2R_PIPE_COUNT] = {0,0};
#endif

static ULONG gOsUsr_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));


//
// Interrupt parameter table
//
static const T_IM_B2R_INTFLG_TBL gim_b2r_inttbl_jdsb2r[D_IM_B2R_PIPE_COUNT][8] = {
	{
		{ 0x00000001, 0x00000001, D_IM_B2R1_INT_FLG_YBR_END,  D_IM_B2R1_INT_STATE_YBR_END,  D_IM_B2R_STATE_BUSY_YBR },	// YBREE
		{ 0x00000002, 0x00000002, D_IM_B2R1_INT_FLG_YBW_END,  D_IM_B2R1_INT_STATE_YBW_END,  D_IM_B2R_STATE_BUSY_YBW },	// YBWEE
		{ 0x00000010, 0x00000010, D_IM_B2R1_INT_FLG_FB2R_END, D_IM_B2R1_INT_STATE_FB2R_END, 0 },						// FB2REE
		{ 0x00000020, 0x00000020, D_IM_B2R1_INT_FLG_LINE_END, D_IM_B2R1_INT_STATE_LINE_END, 0 },						// ELNEE
		{ 0x00000100, 0x00000100, D_IM_B2R1_INT_FLG_YBR_ERR,  D_IM_B2R1_INT_STATE_YBR_ERR,  D_IM_B2R_STATE_BUSY_YBR },	// YBRERE
		{ 0x00000200, 0x00000200, D_IM_B2R1_INT_FLG_YBW_ERR,  D_IM_B2R1_INT_STATE_YBW_ERR,  D_IM_B2R_STATE_BUSY_YBW },	// YBWERE
		{ 0x00001000, 0x00001000, D_IM_B2R1_INT_FLG_AXR_ERR,  D_IM_B2R1_INT_STATE_AXR_ERR,  0 },						// AXRERE
		{ 0x00002000, 0x00002000, D_IM_B2R1_INT_FLG_AXW_ERR,  D_IM_B2R1_INT_STATE_AXW_ERR,  0 },						// AXWERE
	},
	{
		{ 0x00000001, 0x00000001, D_IM_B2R2_INT_FLG_YBR_END,  D_IM_B2R2_INT_STATE_YBR_END,  D_IM_B2R_STATE_BUSY_YBR },	// YBREE
		{ 0x00000002, 0x00000002, D_IM_B2R2_INT_FLG_YBW_END,  D_IM_B2R2_INT_STATE_YBW_END,  D_IM_B2R_STATE_BUSY_YBW },	// YBWEE
		{ 0x00000010, 0x00000010, D_IM_B2R2_INT_FLG_FB2R_END, D_IM_B2R2_INT_STATE_FB2R_END, 0 },						// FB2REE
		{ 0x00000020, 0x00000020, D_IM_B2R2_INT_FLG_LINE_END, D_IM_B2R2_INT_STATE_LINE_END, 0 },						// ELNEE
		{ 0x00000100, 0x00000100, D_IM_B2R2_INT_FLG_YBR_ERR,  D_IM_B2R2_INT_STATE_YBR_ERR,  D_IM_B2R_STATE_BUSY_YBR },	// YBRERE
		{ 0x00000200, 0x00000200, D_IM_B2R2_INT_FLG_YBW_ERR,  D_IM_B2R2_INT_STATE_YBW_ERR,  D_IM_B2R_STATE_BUSY_YBW },	// YBWERE
		{ 0x00001000, 0x00001000, D_IM_B2R2_INT_FLG_AXR_ERR,  D_IM_B2R2_INT_STATE_AXR_ERR,  0 },						// AXRERE
		{ 0x00002000, 0x00002000, D_IM_B2R2_INT_FLG_AXW_ERR,  D_IM_B2R2_INT_STATE_AXW_ERR,  0 },						// AXWERE
	},
};

static const T_IM_B2R_INTFLG_TBL gim_b2r_inttbl_fb2r[D_IM_B2R_PIPE_COUNT] = {
	{ 0x00000001, 0x00000001, D_IM_B2R1_INT_FLG_CPERR_ERR, D_IM_B2R1_INT_STATE_CPERR_ERR, 0 },	// CPERE
	{ 0x00000001, 0x00000001, D_IM_B2R2_INT_FLG_CPERR_ERR, D_IM_B2R2_INT_STATE_CPERR_ERR, 0 },	// CPERE
};


//
// Access enable register manager
//
static VOID im_b2r_set_deknee_access_enable( UCHAR pipe_no, const UCHAR enable );
static volatile T_IM_B2R_ACCESS_ENABLE_MANAGE gim_b2r_deknee_accen_ctrl = {
	.ctrl_cnt = { 0, 0 },
	.reg_set_func = im_b2r_set_deknee_access_enable,
};

static VOID im_b2r_set_rgb_knee_access_enable( UCHAR pipe_no, const UCHAR enable );
static volatile T_IM_B2R_ACCESS_ENABLE_MANAGE gim_b2r_rgb_knee_accen_ctrl = {
	.ctrl_cnt = { 0, 0 },
	.reg_set_func = im_b2r_set_rgb_knee_access_enable,
};


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
static const T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_ADDR	gIM_B2R_Deknee_Tbl_Addr[3] = {
	{
		{
			0x29482000, 0x29482004, 0x29482008, 0x2948200C, 0x29482010, 0x29482014, 0x29482018, 0x2948201C,
			0x29482020, 0x29482024, 0x29482028, 0x2948202C, 0x29482030, 0x29482034, 0x29482038, 0x2948203C,
			0x29482040, 0x29482044, 0x29482048, 0x2948204C, 0x29482050, 0x29482054, 0x29482058, 0x2948205C,
			0x29482060, 0x29482064, 0x29482068, 0x2948206C, 0x29482070, 0x29482074, 0x29482078, 0x2948207C,
			0x29482080, 0x29482084, 0x29482088, 0x2948208C, 0x29482090, 0x29482094, 0x29482098, 0x2948209C,
			0x294820A0, 0x294820A4, 0x294820A8, 0x294820AC, 0x294820B0, 0x294820B4, 0x294820B8, 0x294820BC,
			0x294820C0, 0x294820C4, 0x294820C8, 0x294820CC, 0x294820D0, 0x294820D4, 0x294820D8, 0x294820DC,
			0x294820E0, 0x294820E4, 0x294820E8, 0x294820EC, 0x294820F0, 0x294820F4, 0x294820F8, 0x294820FC,
			0x29482100, 0x29482104, 0x29482108, 0x2948210C, 0x29482110, 0x29482114, 0x29482118, 0x2948211C,
			0x29482120, 0x29482124, 0x29482128, 0x2948212C, 0x29482130, 0x29482134, 0x29482138, 0x2948213C,
			0x29482140, 0x29482144, 0x29482148, 0x2948214C, 0x29482150, 0x29482154, 0x29482158, 0x2948215C,
			0x29482160, 0x29482164, 0x29482168, 0x2948216C, 0x29482170, 0x29482174, 0x29482178, 0x2948217C,
			0x29482180, 0x29482184, 0x29482188, 0x2948218C, 0x29482190, 0x29482194, 0x29482198, 0x2948219C,
			0x294821A0, 0x294821A4, 0x294821A8, 0x294821AC, 0x294821B0, 0x294821B4, 0x294821B8, 0x294821BC,
			0x294821C0, 0x294821C4, 0x294821C8, 0x294821CC, 0x294821D0, 0x294821D4, 0x294821D8, 0x294821DC,
			0x294821E0, 0x294821E4, 0x294821E8, 0x294821EC, 0x294821F0, 0x294821F4, 0x294821F8, 0x294821FC,
		},
	},
	{
		{
			0x29582000, 0x29582004, 0x29582008, 0x2958200C, 0x29582010, 0x29582014, 0x29582018, 0x2958201C,
			0x29582020, 0x29582024, 0x29582028, 0x2958202C, 0x29582030, 0x29582034, 0x29582038, 0x2958203C,
			0x29582040, 0x29582044, 0x29582048, 0x2958204C, 0x29582050, 0x29582054, 0x29582058, 0x2958205C,
			0x29582060, 0x29582064, 0x29582068, 0x2958206C, 0x29582070, 0x29582074, 0x29582078, 0x2958207C,
			0x29582080, 0x29582084, 0x29582088, 0x2958208C, 0x29582090, 0x29582094, 0x29582098, 0x2958209C,
			0x295820A0, 0x295820A4, 0x295820A8, 0x295820AC, 0x295820B0, 0x295820B4, 0x295820B8, 0x295820BC,
			0x295820C0, 0x295820C4, 0x295820C8, 0x295820CC, 0x295820D0, 0x295820D4, 0x295820D8, 0x295820DC,
			0x295820E0, 0x295820E4, 0x295820E8, 0x295820EC, 0x295820F0, 0x295820F4, 0x295820F8, 0x295820FC,
			0x29582100, 0x29582104, 0x29582108, 0x2958210C, 0x29582110, 0x29582114, 0x29582118, 0x2958211C,
			0x29582120, 0x29582124, 0x29582128, 0x2958212C, 0x29582130, 0x29582134, 0x29582138, 0x2958213C,
			0x29582140, 0x29582144, 0x29582148, 0x2958214C, 0x29582150, 0x29582154, 0x29582158, 0x2958215C,
			0x29582160, 0x29582164, 0x29582168, 0x2958216C, 0x29582170, 0x29582174, 0x29582178, 0x2958217C,
			0x29582180, 0x29582184, 0x29582188, 0x2958218C, 0x29582190, 0x29582194, 0x29582198, 0x2958219C,
			0x295821A0, 0x295821A4, 0x295821A8, 0x295821AC, 0x295821B0, 0x295821B4, 0x295821B8, 0x295821BC,
			0x295821C0, 0x295821C4, 0x295821C8, 0x295821CC, 0x295821D0, 0x295821D4, 0x295821D8, 0x295821DC,
			0x295821E0, 0x295821E4, 0x295821E8, 0x295821EC, 0x295821F0, 0x295821F4, 0x295821F8, 0x295821FC,
		},
	},
	{
		{
			0x29682000, 0x29682004, 0x29682008, 0x2968200C, 0x29682010, 0x29682014, 0x29682018, 0x2968201C,
			0x29682020, 0x29682024, 0x29682028, 0x2968202C, 0x29682030, 0x29682034, 0x29682038, 0x2968203C,
			0x29682040, 0x29682044, 0x29682048, 0x2968204C, 0x29682050, 0x29682054, 0x29682058, 0x2968205C,
			0x29682060, 0x29682064, 0x29682068, 0x2968206C, 0x29682070, 0x29682074, 0x29682078, 0x2968207C,
			0x29682080, 0x29682084, 0x29682088, 0x2968208C, 0x29682090, 0x29682094, 0x29682098, 0x2968209C,
			0x296820A0, 0x296820A4, 0x296820A8, 0x296820AC, 0x296820B0, 0x296820B4, 0x296820B8, 0x296820BC,
			0x296820C0, 0x296820C4, 0x296820C8, 0x296820CC, 0x296820D0, 0x296820D4, 0x296820D8, 0x296820DC,
			0x296820E0, 0x296820E4, 0x296820E8, 0x296820EC, 0x296820F0, 0x296820F4, 0x296820F8, 0x296820FC,
			0x29682100, 0x29682104, 0x29682108, 0x2968210C, 0x29682110, 0x29682114, 0x29682118, 0x2968211C,
			0x29682120, 0x29682124, 0x29682128, 0x2968212C, 0x29682130, 0x29682134, 0x29682138, 0x2968213C,
			0x29682140, 0x29682144, 0x29682148, 0x2968214C, 0x29682150, 0x29682154, 0x29682158, 0x2968215C,
			0x29682160, 0x29682164, 0x29682168, 0x2968216C, 0x29682170, 0x29682174, 0x29682178, 0x2968217C,
			0x29682180, 0x29682184, 0x29682188, 0x2968218C, 0x29682190, 0x29682194, 0x29682198, 0x2968219C,
			0x296821A0, 0x296821A4, 0x296821A8, 0x296821AC, 0x296821B0, 0x296821B4, 0x296821B8, 0x296821BC,
			0x296821C0, 0x296821C4, 0x296821C8, 0x296821CC, 0x296821D0, 0x296821D4, 0x296821D8, 0x296821DC,
			0x296821E0, 0x296821E4, 0x296821E8, 0x296821EC, 0x296821F0, 0x296821F4, 0x296821F8, 0x296821FC,
		},
	},
};

static const T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR	gIM_B2R_Knee_Tbl_R_Addr[3] = {
	{
		{
			0x29482800, 0x29482804, 0x29482808, 0x2948280C, 0x29482810, 0x29482814, 0x29482818, 0x2948281C,
			0x29482820, 0x29482824, 0x29482828, 0x2948282C, 0x29482830, 0x29482834, 0x29482838, 0x2948283C,
			0x29482840, 0x29482844, 0x29482848, 0x2948284C, 0x29482850, 0x29482854, 0x29482858, 0x2948285C,
			0x29482860, 0x29482864, 0x29482868, 0x2948286C, 0x29482870, 0x29482874, 0x29482878, 0x2948287C,
			0x29482880, 0x29482884, 0x29482888, 0x2948288C, 0x29482890, 0x29482894, 0x29482898, 0x2948289C,
			0x294828A0, 0x294828A4, 0x294828A8, 0x294828AC, 0x294828B0, 0x294828B4, 0x294828B8, 0x294828BC,
			0x294828C0, 0x294828C4, 0x294828C8, 0x294828CC, 0x294828D0, 0x294828D4, 0x294828D8, 0x294828DC,
			0x294828E0, 0x294828E4, 0x294828E8, 0x294828EC, 0x294828F0, 0x294828F4, 0x294828F8, 0x294828FC,
			0x29482900,
		},
	},
	{
		{
			0x29582800, 0x29582804, 0x29582808, 0x2958280C, 0x29582810, 0x29582814, 0x29582818, 0x2958281C,
			0x29582820, 0x29582824, 0x29582828, 0x2958282C, 0x29582830, 0x29582834, 0x29582838, 0x2958283C,
			0x29582840, 0x29582844, 0x29582848, 0x2958284C, 0x29582850, 0x29582854, 0x29582858, 0x2958285C,
			0x29582860, 0x29582864, 0x29582868, 0x2958286C, 0x29582870, 0x29582874, 0x29582878, 0x2958287C,
			0x29582880, 0x29582884, 0x29582888, 0x2958288C, 0x29582890, 0x29582894, 0x29582898, 0x2958289C,
			0x295828A0, 0x295828A4, 0x295828A8, 0x295828AC, 0x295828B0, 0x295828B4, 0x295828B8, 0x295828BC,
			0x295828C0, 0x295828C4, 0x295828C8, 0x295828CC, 0x295828D0, 0x295828D4, 0x295828D8, 0x295828DC,
			0x295828E0, 0x295828E4, 0x295828E8, 0x295828EC, 0x295828F0, 0x295828F4, 0x295828F8, 0x295828FC,
			0x29582900,
		},
	},
	{
		{
			0x29682800, 0x29682804, 0x29682808, 0x2968280C, 0x29682810, 0x29682814, 0x29682818, 0x2968281C,
			0x29682820, 0x29682824, 0x29682828, 0x2968282C, 0x29682830, 0x29682834, 0x29682838, 0x2968283C,
			0x29682840, 0x29682844, 0x29682848, 0x2968284C, 0x29682850, 0x29682854, 0x29682858, 0x2968285C,
			0x29682860, 0x29682864, 0x29682868, 0x2968286C, 0x29682870, 0x29682874, 0x29682878, 0x2968287C,
			0x29682880, 0x29682884, 0x29682888, 0x2968288C, 0x29682890, 0x29682894, 0x29682898, 0x2968289C,
			0x296828A0, 0x296828A4, 0x296828A8, 0x296828AC, 0x296828B0, 0x296828B4, 0x296828B8, 0x296828BC,
			0x296828C0, 0x296828C4, 0x296828C8, 0x296828CC, 0x296828D0, 0x296828D4, 0x296828D8, 0x296828DC,
			0x296828E0, 0x296828E4, 0x296828E8, 0x296828EC, 0x296828F0, 0x296828F4, 0x296828F8, 0x296828FC,
			0x29682900,
		},
	},
};

static const T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR	gIM_B2R_Knee_Tbl_G_Addr[3] = {
	{
		{
			0x29482A00, 0x29482A04, 0x29482A08, 0x29482A0C, 0x29482A10, 0x29482A14, 0x29482A18, 0x29482A1C,
			0x29482A20, 0x29482A24, 0x29482A28, 0x29482A2C, 0x29482A30, 0x29482A34, 0x29482A38, 0x29482A3C,
			0x29482A40, 0x29482A44, 0x29482A48, 0x29482A4C, 0x29482A50, 0x29482A54, 0x29482A58, 0x29482A5C,
			0x29482A60, 0x29482A64, 0x29482A68, 0x29482A6C, 0x29482A70, 0x29482A74, 0x29482A78, 0x29482A7C,
			0x29482A80, 0x29482A84, 0x29482A88, 0x29482A8C, 0x29482A90, 0x29482A94, 0x29482A98, 0x29482A9C,
			0x29482AA0, 0x29482AA4, 0x29482AA8, 0x29482AAC, 0x29482AB0, 0x29482AB4, 0x29482AB8, 0x29482ABC,
			0x29482AC0, 0x29482AC4, 0x29482AC8, 0x29482ACC, 0x29482AD0, 0x29482AD4, 0x29482AD8, 0x29482ADC,
			0x29482AE0, 0x29482AE4, 0x29482AE8, 0x29482AEC, 0x29482AF0, 0x29482AF4, 0x29482AF8, 0x29482AFC,
			0x29482B00,
		},
	},
	{
		{
			0x29582A00, 0x29582A04, 0x29582A08, 0x29582A0C, 0x29582A10, 0x29582A14, 0x29582A18, 0x29582A1C,
			0x29582A20, 0x29582A24, 0x29582A28, 0x29582A2C, 0x29582A30, 0x29582A34, 0x29582A38, 0x29582A3C,
			0x29582A40, 0x29582A44, 0x29582A48, 0x29582A4C, 0x29582A50, 0x29582A54, 0x29582A58, 0x29582A5C,
			0x29582A60, 0x29582A64, 0x29582A68, 0x29582A6C, 0x29582A70, 0x29582A74, 0x29582A78, 0x29582A7C,
			0x29582A80, 0x29582A84, 0x29582A88, 0x29582A8C, 0x29582A90, 0x29582A94, 0x29582A98, 0x29582A9C,
			0x29582AA0, 0x29582AA4, 0x29582AA8, 0x29582AAC, 0x29582AB0, 0x29582AB4, 0x29582AB8, 0x29582ABC,
			0x29582AC0, 0x29582AC4, 0x29582AC8, 0x29582ACC, 0x29582AD0, 0x29582AD4, 0x29582AD8, 0x29582ADC,
			0x29582AE0, 0x29582AE4, 0x29582AE8, 0x29582AEC, 0x29582AF0, 0x29582AF4, 0x29582AF8, 0x29582AFC,
			0x29582B00,
		},
	},
	{
		{
			0x29682A00, 0x29682A04, 0x29682A08, 0x29682A0C, 0x29682A10, 0x29682A14, 0x29682A18, 0x29682A1C,
			0x29682A20, 0x29682A24, 0x29682A28, 0x29682A2C, 0x29682A30, 0x29682A34, 0x29682A38, 0x29682A3C,
			0x29682A40, 0x29682A44, 0x29682A48, 0x29682A4C, 0x29682A50, 0x29682A54, 0x29682A58, 0x29682A5C,
			0x29682A60, 0x29682A64, 0x29682A68, 0x29682A6C, 0x29682A70, 0x29682A74, 0x29682A78, 0x29682A7C,
			0x29682A80, 0x29682A84, 0x29682A88, 0x29682A8C, 0x29682A90, 0x29682A94, 0x29682A98, 0x29682A9C,
			0x29682AA0, 0x29682AA4, 0x29682AA8, 0x29682AAC, 0x29682AB0, 0x29682AB4, 0x29682AB8, 0x29682ABC,
			0x29682AC0, 0x29682AC4, 0x29682AC8, 0x29682ACC, 0x29682AD0, 0x29682AD4, 0x29682AD8, 0x29682ADC,
			0x29682AE0, 0x29682AE4, 0x29682AE8, 0x29682AEC, 0x29682AF0, 0x29682AF4, 0x29682AF8, 0x29682AFC,
			0x29682B00,
		},
	},
};

static const T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR	gIM_B2R_Knee_Tbl_B_Addr[3] = {
	{
		{
			0x29482C00, 0x29482C04, 0x29482C08, 0x29482C0C, 0x29482C10, 0x29482C14, 0x29482C18, 0x29482C1C,
			0x29482C20, 0x29482C24, 0x29482C28, 0x29482C2C, 0x29482C30, 0x29482C34, 0x29482C38, 0x29482C3C,
			0x29482C40, 0x29482C44, 0x29482C48, 0x29482C4C, 0x29482C50, 0x29482C54, 0x29482C58, 0x29482C5C,
			0x29482C60, 0x29482C64, 0x29482C68, 0x29482C6C, 0x29482C70, 0x29482C74, 0x29482C78, 0x29482C7C,
			0x29482C80, 0x29482C84, 0x29482C88, 0x29482C8C, 0x29482C90, 0x29482C94, 0x29482C98, 0x29482C9C,
			0x29482CA0, 0x29482CA4, 0x29482CA8, 0x29482CAC, 0x29482CB0, 0x29482CB4, 0x29482CB8, 0x29482CBC,
			0x29482CC0, 0x29482CC4, 0x29482CC8, 0x29482CCC, 0x29482CD0, 0x29482CD4, 0x29482CD8, 0x29482CDC,
			0x29482CE0, 0x29482CE4, 0x29482CE8, 0x29482CEC, 0x29482CF0, 0x29482CF4, 0x29482CF8, 0x29482CFC,
			0x29482D00,
		},
	},
	{
		{
			0x29582C00, 0x29582C04, 0x29582C08, 0x29582C0C, 0x29582C10, 0x29582C14, 0x29582C18, 0x29582C1C,
			0x29582C20, 0x29582C24, 0x29582C28, 0x29582C2C, 0x29582C30, 0x29582C34, 0x29582C38, 0x29582C3C,
			0x29582C40, 0x29582C44, 0x29582C48, 0x29582C4C, 0x29582C50, 0x29582C54, 0x29582C58, 0x29582C5C,
			0x29582C60, 0x29582C64, 0x29582C68, 0x29582C6C, 0x29582C70, 0x29582C74, 0x29582C78, 0x29582C7C,
			0x29582C80, 0x29582C84, 0x29582C88, 0x29582C8C, 0x29582C90, 0x29582C94, 0x29582C98, 0x29582C9C,
			0x29582CA0, 0x29582CA4, 0x29582CA8, 0x29582CAC, 0x29582CB0, 0x29582CB4, 0x29582CB8, 0x29582CBC,
			0x29582CC0, 0x29582CC4, 0x29582CC8, 0x29582CCC, 0x29582CD0, 0x29582CD4, 0x29582CD8, 0x29582CDC,
			0x29582CE0, 0x29582CE4, 0x29582CE8, 0x29582CEC, 0x29582CF0, 0x29582CF4, 0x29582CF8, 0x29582CFC,
			0x29582D00,
		},
	},
	{
		{
			0x29682C00, 0x29682C04, 0x29682C08, 0x29682C0C, 0x29682C10, 0x29682C14, 0x29682C18, 0x29682C1C,
			0x29682C20, 0x29682C24, 0x29682C28, 0x29682C2C, 0x29682C30, 0x29682C34, 0x29682C38, 0x29682C3C,
			0x29682C40, 0x29682C44, 0x29682C48, 0x29682C4C, 0x29682C50, 0x29682C54, 0x29682C58, 0x29682C5C,
			0x29682C60, 0x29682C64, 0x29682C68, 0x29682C6C, 0x29682C70, 0x29682C74, 0x29682C78, 0x29682C7C,
			0x29682C80, 0x29682C84, 0x29682C88, 0x29682C8C, 0x29682C90, 0x29682C94, 0x29682C98, 0x29682C9C,
			0x29682CA0, 0x29682CA4, 0x29682CA8, 0x29682CAC, 0x29682CB0, 0x29682CB4, 0x29682CB8, 0x29682CBC,
			0x29682CC0, 0x29682CC4, 0x29682CC8, 0x29682CCC, 0x29682CD0, 0x29682CD4, 0x29682CD8, 0x29682CDC,
			0x29682CE0, 0x29682CE4, 0x29682CE8, 0x29682CEC, 0x29682CF0, 0x29682CF4, 0x29682CF8, 0x29682CFC,
			0x29682D00,
		},
	},
};

static const T_IM_B2R_CTRL_RDMA_OFST_ADDR	gIM_B2R_Ofst_Addr[3] = {
	{
		0x28412040, 0x28412044,
	},
	{
		0x28512040, 0x28512044,
	},
	{
		0x28612040, 0x28612044,
	},
};

static const T_IM_B2R_CTRL_RDMA_WB_SLOPE_ADDR	gIM_B2R_Wb_Slope_Addr[3] = {
	{
		0x28412054, 0x28412058, 0x2841205C,
	},
	{
		0x28512054, 0x28512058, 0x2851205C,
	},
	{
		0x28612054, 0x28612058, 0x2861205C,
	},
};

static const T_IM_B2R_CTRL_RDMA_WB_CLIP_ADDR	gIM_B2R_Wb_Clip_Addr[3] = {
	{
		0x28412060, 0x28412064,
	},
	{
		0x28512060, 0x28512064,
	},
	{
		0x28612060, 0x28612064,
	},
};

static const T_IM_B2R_CTRL_RDMA_SENSITIVITY_ADDR	gIM_B2R_Sensitivity_Addr[3] = {
	{
		0x28412080, 0x28412084, 0x28412088, 0x2841208C, 0x28412090,
	},
	{
		0x28512080, 0x28512084, 0x28512088, 0x2851208C, 0x28512090,
	},
	{
		0x28612080, 0x28612084, 0x28612088, 0x2861208C, 0x28612090,
	},
};

static const T_IM_B2R_CTRL_RDMA_HPF_ADDR	gIM_B2R_Hpf_Addr[3] = {
	{
		0x28412100, 0x28412110, 0x28412114, 0x28412118, 0x2841211C, 0x28412120, 0x28412124, 0x28412128,
		0x2841212C, 0x28412130, 0x28412134, 0x28412138, 0x2841213C, 0x28412140, 0x28412144, 0x28412148,
		0x28412150, 0x28412154, 0x28412158, 0x2841215C, 0x28412160, 0x28412168, 0x2841216C, 0x28412170,
		0x28412174, 0x28412178, 0x2841217C, 0x28412180, 0x28412184, 0x28412188, 0x2841218C, 0x28412190,
		0x28412194, 0x28412198, 0x2841219C, 0x284121A0, 0x284121A4, 0x284121A8, 0x284121AC, 0x284121B0,
	},
	{
		0x28512100, 0x28512110, 0x28512114, 0x28512118, 0x2851211C, 0x28512120, 0x28512124, 0x28512128,
		0x2851212C, 0x28512130, 0x28512134, 0x28512138, 0x2851213C, 0x28512140, 0x28512144, 0x28512148,
		0x28512150, 0x28512154, 0x28512158, 0x2851215C, 0x28512160, 0x28512168, 0x2851216C, 0x28512170,
		0x28512174, 0x28512178, 0x2851217C, 0x28512180, 0x28512184, 0x28512188, 0x2851218C, 0x28512190,
		0x28512194, 0x28512198, 0x2851219C, 0x285121A0, 0x285121A4, 0x285121A8, 0x285121AC, 0x285121B0,
	},
	{
		0x28612100, 0x28612110, 0x28612114, 0x28612118, 0x2861211C, 0x28612120, 0x28612124, 0x28612128,
		0x2861212C, 0x28612130, 0x28612134, 0x28612138, 0x2861213C, 0x28612140, 0x28612144, 0x28612148,
		0x28612150, 0x28612154, 0x28612158, 0x2861215C, 0x28612160, 0x28612168, 0x2861216C, 0x28612170,
		0x28612174, 0x28612178, 0x2861217C, 0x28612180, 0x28612184, 0x28612188, 0x2861218C, 0x28612190,
		0x28612194, 0x28612198, 0x2861219C, 0x286121A0, 0x286121A4, 0x286121A8, 0x286121AC, 0x286121B0,
	},
};
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
static VOID im_b2r_reset( UCHAR pipe_no );
static VOID im_b2r_manager_init( UCHAR pipe_no );
static VOID im_b2r_calc_top_color_sdram_input( UCHAR pipe_no );
static VOID im_b2r_set_top_color_b2b_direct( UCHAR pipe_no );
static VOID im_b2r_calc_ring_pixs( UCHAR pipe_no );
static VOID im_b2r_calc_top_msb_offset_sdram_input( UCHAR pipe_no );
static VOID im_b2r_calc_ybr_address_sdram_input( UCHAR pipe_no );
static VOID im_b2r_set_ybr_address_sdram_input( UCHAR pipe_no );
static VOID im_b2r_calc_in_rect_sdram_input( UCHAR pipe_no );
static VOID im_b2r_calc_in_rect_b2b_direct( UCHAR pipe_no );
static VOID im_b2r_latest_addr_update( UCHAR pipe_no );
static UCHAR im_b2r_is_act_fb2r( UCHAR pipe_no );
static INT32 im_b2r_set_access_enable( UCHAR pipe_no, volatile T_IM_B2R_ACCESS_ENABLE_MANAGE* const acc_en_mng, const UCHAR acc_enable, const UCHAR wait_enable, const CHAR errmsg[] );
static VOID im_b2r_set_deknee_access_enable( UCHAR pipe_no, const UCHAR enable );
static VOID im_b2r_set_rgb_knee_access_enable( UCHAR pipe_no, const UCHAR enable );
static VOID im_b2r_get_loop_val( UCHAR pipe_no, UCHAR* loop_sta, UCHAR* loop_end );
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
#ifdef CO_B2R_RDMA_ON
static VOID im_b2r_start_rdma( T_IM_RDMA_CTRL* ctrl );
static VOID im_b2r_set_rdma_val_deknee_table( UCHAR pipe_no, const USHORT* const src_tbl );
static VOID im_b2r_set_rdma_val_knee_table( UCHAR pipe_no, E_B2R_RGBTBL tbl_type, const UCHAR* const src_tbl );
static VOID im_b2r_set_rdma_val_offset_ctrl( UCHAR pipe_no, const T_IM_B2R_OFS* const ofs );
static VOID im_b2r_set_rdma_val_wb_slope_ctrl( UCHAR pipe_no, UCHAR slope_gain_enable, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color );
static VOID im_b2r_set_rdma_val_wb_clip_ctrl( UCHAR pipe_no, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color );
static VOID im_b2r_set_rdma_val_sensitivity_ctrl( UCHAR pipe_no, const T_IM_B2R_CTRL_ADIP* const b2r_ctrl_adip );
static VOID im_b2r_set_rdma_val_hpf_ctrl( UCHAR pipe_no, const T_IM_B2R_CTRL_HPF* const b2r_ctrl_hpf );
#endif	// CO_B2R_RDMA_ON
#endif	// CO_DDIM_UTILITY_USE


/* B2R reset sequence
 */
static VOID im_b2r_reset( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_reset error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	Im_B2R_On_Hclk( pipe_no );
	Im_B2R_On_Iclk( pipe_no );
	Im_B2R_On_Clk( pipe_no );	// TRG off control accompanies the B2R reset
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.CNTL.word = 0x0000;
	Im_B2R_Off_Clk( pipe_no );
	Im_B2R_Off_Iclk( pipe_no );
	Im_B2R_Off_Hclk( pipe_no );
	Im_B2R_Off_Pclk( pipe_no );
}

/* Reset internal value
 */
static VOID im_b2r_manager_init( UCHAR pipe_no )
{
	UCHAR size_coef = 1;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_manager_init error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	if( pipe_no == D_IM_B2R_PIPE12 ){
		size_coef = 2;
		pipe_no = 0;
	}

	// Initialize of State & Handler management information
	memset( (USHORT*)&gIM_B2R_State[pipe_no], 0, sizeof(T_IM_B2R_STATE_MNG) * size_coef );

	// Initialize of Address management information
	memset( (USHORT*)&gIM_B2R_Out_Mng[pipe_no], 0, sizeof(T_IM_B2R_OUTPUT_MNG) * size_coef );

	// initialize of Rectangle information
	memset( (USHORT*)&gIM_B2R_Rect_Param[pipe_no], 0, sizeof(T_IM_B2R_RECT) * size_coef );

	// Initialize of control information
	memset( (USHORT*)&gIM_B2R_ybra_ofs_info[pipe_no], 0, sizeof(T_IM_B2R_YBRA_OFS_INFO) * size_coef );

	// Initialize of PCLK/HCLK/CLK control information
#ifdef CO_ACT_B2R_CLOCK
	memset( (USHORT*)&gIM_B2R_Clk_Ctrl_Cnt[pipe_no], 0, sizeof(UCHAR) * size_coef );
	memset( (USHORT*)&gIM_B2R_Clk_Ctrl_Disable[pipe_no], 0, sizeof(UINT32) * size_coef );
#endif	// CO_ACT_B2R_CLOCK
#ifdef CO_ACT_B2R_PCLOCK
	memset( (USHORT*)&gIM_B2R_Pclk_Ctrl_Cnt[pipe_no], 0, sizeof(UCHAR) * size_coef );
	memset( (USHORT*)&gIM_B2R_Pclk_Ctrl_Disable[pipe_no], 0, sizeof(UINT32) * size_coef );
#endif	// CO_ACT_B2R_PCLOCK
#ifdef CO_ACT_B2R_HCLOCK
	memset( (USHORT*)&gIM_B2R_Hclk_Ctrl_Cnt[pipe_no], 0, sizeof(UCHAR) * size_coef );
	memset( (USHORT*)&gIM_B2R_Hclk_Ctrl_Disable[pipe_no], 0, sizeof(UINT32) * size_coef );
#endif	// CO_ACT_B2R_HCLOCK
#ifdef CO_ACT_B2R_ICLOCK
	memset( (USHORT*)&gIM_B2R_Iclk_Ctrl_Cnt[pipe_no], 0, sizeof(UCHAR) * size_coef );
	memset( (USHORT*)&gIM_B2R_Iclk_Ctrl_Disable[pipe_no], 0, sizeof(UINT32) * size_coef );
#endif	// CO_ACT_B2R_ICLOCK

	return;
}

/* Calculate bayer left-top pixel color.
 */
static VOID im_b2r_calc_top_color_sdram_input( UCHAR pipe_no )
{
	static const UCHAR t_top_pix_of_trim[4][2][2] = {	// top_color, trim_v_odd, trim_h_odd
			// H even				 H odd
		{	// R
			{ D_IM_B2R_TOP_PIXEL_R,  D_IM_B2R_TOP_PIXEL_GR },	// V even
			{ D_IM_B2R_TOP_PIXEL_GB, D_IM_B2R_TOP_PIXEL_B }		// V odd
		},
		{	// B
			{ D_IM_B2R_TOP_PIXEL_B,  D_IM_B2R_TOP_PIXEL_GB }, 	// V even
			{ D_IM_B2R_TOP_PIXEL_GR, D_IM_B2R_TOP_PIXEL_R }		// V odd
		},
		{	// GR
			{ D_IM_B2R_TOP_PIXEL_GR, D_IM_B2R_TOP_PIXEL_R },	// V even
			{ D_IM_B2R_TOP_PIXEL_B,  D_IM_B2R_TOP_PIXEL_GB }	// V odd
		},
		{	// GB
			{ D_IM_B2R_TOP_PIXEL_GB, D_IM_B2R_TOP_PIXEL_B },	// V even
			{ D_IM_B2R_TOP_PIXEL_R,  D_IM_B2R_TOP_PIXEL_GR }	// V odd
		}
	};
	USHORT trim_v_odd;
	USHORT trim_h_odd;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_calc_top_color_sdram_input error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );

	trim_v_odd = gIM_B2R_State[pipe_no].input_size.img_top % 2;
	trim_h_odd = gIM_B2R_State[pipe_no].input_size.img_left % 2;

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRCTL.bit.PIXEL
			 = t_top_pix_of_trim[gIM_B2R_State[pipe_no].bayer_top_pixel][trim_v_odd][trim_h_odd];

	Im_B2R_Off_Pclk( pipe_no );
}

/* Top color set to register
 */
static VOID im_b2r_set_top_color_b2b_direct( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_set_top_color_b2b_direct error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRCTL.bit.PIXEL = gIM_B2R_State[pipe_no].bayer_top_pixel;

	Im_B2R_Off_Pclk( pipe_no );
}

/* Calculate padding pixs and referenced pixs with filter mode
 */
static VOID im_b2r_calc_ring_pixs( UCHAR pipe_no )
{
	static const T_IM_B2R_RING_PIXS ring_pixs_info_tbl[2][2] = {	// CIABYP, CIAPDD
		{	// CIABYP=0
			{ D_IM_B2R_ENABLE_OFF, 3, 0, 6 },	// CIABYP=0, CIAPDD=0
			{ D_IM_B2R_ENABLE_ON,  3, 6, 0 },	// CIABYP=0, CIAPDD=1
		},
		{	// CIABYP=1
			{ D_IM_B2R_ENABLE_OFF, 0, 0, 0 },	// CIABYP=1, CIAPDD=0
			{ D_IM_B2R_ENABLE_ON,  0, 0, 0 },	// CIABYP=1, CIAPDD=1	NG Setting
		}
	};
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_calc_ring_pixs error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		// Get current ring pixel information
		gIM_B2R_State[loop_cnt].ring_pixs_info = ring_pixs_info_tbl[gIM_B2R_State[loop_cnt].cia_bypass][gIM_B2R_State[loop_cnt].cia_padding];

#ifdef IM_B2R_DEBUG_PRINT
		Ddim_Print(( "im_b2r_calc_ring_pixs(): en=%u ring=%u pad=%u ref=%u\n",
					gIM_B2R_State[loop_cnt].ring_pixs_info.pad_enable,
					gIM_B2R_State[loop_cnt].ring_pixs_info.ring_pixs,
					gIM_B2R_State[loop_cnt].ring_pixs_info.pad_pixs,
					gIM_B2R_State[loop_cnt].ring_pixs_info.ref_pixs ));
#endif
	}
}

/* Calculate top-left pixel MSB offset bit.
 */
static VOID im_b2r_calc_top_msb_offset_sdram_input( UCHAR pipe_no )
{
	static const UCHAR msb_offset_pack12[D_IM_B2R_PACK12_MSB_CYCLE_PIXS * 2] = {
		0, 4, 0, 4
	};
	static const UCHAR msb_offset_pack10[D_IM_B2R_PACK10_MSB_CYCLE_PIXS * 2] = {
		0, 2, 4, 6, 0, 2, 4, 6
	};
	static const UCHAR msb_offset_to_pixs_pack10[8] = {	// from msb offset to msb_offset_pack10' s index number
		0, 0, 1, 0, 2, 0, 3, 0
	};
	static const UCHAR msb_offset_pack8_16[1] = {
		0
	};
	USHORT trim_left;
	USHORT msb_array_idx;
	USHORT cycle_pixs;
	UCHAR const* p_msb_offset;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_calc_top_msb_offset_sdram_input error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );

	switch( gIM_B2R_State[pipe_no].input_dtype ) {
		case D_IM_B2R_STL_DTYP_PACK8:	// FALL THROUGH
		case D_IM_B2R_STL_DTYP_PACK16:
			p_msb_offset = &msb_offset_pack8_16[0];
			cycle_pixs = 1;
			msb_array_idx = 0;
			break;
		case D_IM_B2R_STL_DTYP_PACK10:
			p_msb_offset = &msb_offset_pack10[0];
			cycle_pixs = D_IM_B2R_PACK10_MSB_CYCLE_PIXS;
			msb_array_idx = msb_offset_to_pixs_pack10[gIM_B2R_State[pipe_no].top_offset];
			break;
//		case D_IM_B2R_STL_DTYP_PACK12:
		default:
			p_msb_offset = &msb_offset_pack12[0];
			cycle_pixs = D_IM_B2R_PACK12_MSB_CYCLE_PIXS;
			msb_array_idx = (gIM_B2R_State[pipe_no].top_offset == 0)?(0):(1);
			break;
	}

	trim_left = gIM_B2R_State[pipe_no].input_size.img_left;

	msb_array_idx += (USHORT)( trim_left % cycle_pixs );

#ifdef IM_B2R_DEBUG_PRINT
#if 0
	Ddim_Print(( "im_b2r_calc_top_msb_offset_sdram_input(): msb=%u\n", p_msb_offset ));
#endif
#endif

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBROFS.word = p_msb_offset[msb_array_idx];

	Im_B2R_Off_Pclk( pipe_no );
}

/* Calculate bayer trimming bytes from bayer address set by Im_B2R_Set_InAddr_Info()
 */
static VOID im_b2r_calc_ybr_address_sdram_input( UCHAR pipe_no )
{
	ULONG tmp_top;
	ULONG tmp_left;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_calc_ybr_address_sdram_input error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	gIM_B2R_ybra_ofs_info[pipe_no].ofs_x_pixs = gIM_B2R_State[pipe_no].input_size.img_left;
	gIM_B2R_ybra_ofs_info[pipe_no].ofs_y_pixs = gIM_B2R_State[pipe_no].input_size.img_top;

	switch( gIM_B2R_State[pipe_no].input_dtype ) {
		case D_IM_B2R_STL_DTYP_PACK12:
			tmp_top  = (gIM_B2R_State[pipe_no].input_global * gIM_B2R_ybra_ofs_info[pipe_no].ofs_y_pixs);
			tmp_left = (gIM_B2R_ybra_ofs_info[pipe_no].ofs_x_pixs * 3 / 2);	// x1.5
			break;
		case D_IM_B2R_STL_DTYP_PACK10:
			tmp_top  = (gIM_B2R_State[pipe_no].input_global * gIM_B2R_ybra_ofs_info[pipe_no].ofs_y_pixs);
			tmp_left = (gIM_B2R_ybra_ofs_info[pipe_no].ofs_x_pixs * 5 / 4);	// x1.25
			break;
		case D_IM_B2R_STL_DTYP_PACK16:
			tmp_top  = (gIM_B2R_State[pipe_no].input_global * gIM_B2R_ybra_ofs_info[pipe_no].ofs_y_pixs);
			tmp_left = (gIM_B2R_ybra_ofs_info[pipe_no].ofs_x_pixs * 2);		// x2.0
			break;
		default:// 8 bit PACK
			tmp_top  = (gIM_B2R_State[pipe_no].input_global * gIM_B2R_ybra_ofs_info[pipe_no].ofs_y_pixs);
			tmp_left = (gIM_B2R_ybra_ofs_info[pipe_no].ofs_x_pixs);			// x1.0
			break;
	}

	gIM_B2R_ybra_ofs_info[pipe_no].ofs_bytes = (tmp_top + tmp_left);
}

/* Calculate YBRA address after trimming.
 */
static VOID im_b2r_set_ybr_address_sdram_input( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_set_ybr_address_sdram_input error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk(pipe_no);

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRA.word = (gIM_B2R_State[pipe_no].in_addr + gIM_B2R_ybra_ofs_info[pipe_no].ofs_bytes);

	Im_B2R_Off_Pclk(pipe_no);
}

/* Calculate input(YBR) width and lines when SDRAM input mode.
 */
static VOID im_b2r_calc_in_rect_sdram_input( UCHAR pipe_no )
{
	USHORT img_width;
	USHORT img_lines;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_set_ybr_address_sdram_input error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk(pipe_no);

	// YBRHSIZ (2pixels alignment & Advancing)
	img_width = gIM_B2R_State[pipe_no].input_size.img_width;
	img_width = im_b2r_min( img_width, (USHORT)D_IM_B2R_YBRHSIZ_MAX );
	img_width = im_b2r_max( img_width, (USHORT)(D_IM_B2R_YBRHSIZ_MIN + gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs) );

	// YBRVSIZ
	img_lines = gIM_B2R_State[pipe_no].input_size.img_lines;
	img_lines = im_b2r_min( img_lines, (USHORT)D_IM_B2R_YBRVSIZ_MAX );
	img_lines = im_b2r_max( img_lines, (USHORT)(D_IM_B2R_YBRVSIZ_MIN + gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs) );

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRHSIZ.word = img_width;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRVSIZ.word = img_lines;

	Im_B2R_Off_Pclk(pipe_no);
}

/* Calculate input(YBR) width and lines when B2B direct input mode.
 */
static VOID im_b2r_calc_in_rect_b2b_direct( UCHAR pipe_no )
{
	USHORT img_top;
	USHORT img_left;
	USHORT img_width;
	USHORT img_lines;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_calc_in_rect_b2b_direct error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk(pipe_no);

	// DINVSTA
	img_top = gIM_B2R_State[pipe_no].input_size.img_top;
	img_top = im_b2r_min( img_top, (USHORT)D_IM_B2R_DINVSTA_MAX );
	img_top = im_b2r_max( img_top, (USHORT)D_IM_B2R_DINVSTA_MIN );

	// DINHSTA
	img_left = gIM_B2R_State[pipe_no].input_size.img_left;
	img_left = im_b2r_min( img_left, (USHORT)D_IM_B2R_DINHSTA_MAX );
	img_left = im_b2r_max( img_left, (USHORT)D_IM_B2R_DINHSTA_MIN );

	// DINVSIZ
	img_lines = gIM_B2R_State[pipe_no].input_size.img_lines;
	img_lines = im_b2r_min( img_lines, (USHORT)D_IM_B2R_DINVSIZ_MAX );
	img_lines = im_b2r_max( img_lines, (USHORT)(D_IM_B2R_DINVSIZ_MIN + gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs) );

	// DINHSIZ (2pixels alignment & Advancing)
	img_width = gIM_B2R_State[pipe_no].input_size.img_width;
	img_width = im_b2r_min( img_width, (USHORT)D_IM_B2R_DINHSIZ_MAX );
	img_width = im_b2r_max( img_width, (USHORT)(D_IM_B2R_DINHSIZ_MIN + gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs) );

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINVSTA.word = img_top;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINHSTA.word = img_left;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINVSIZ.word = img_lines;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINHSIZ.word = img_width;

	Im_B2R_Off_Pclk(pipe_no);
}

/* Update of Address information
 */
static VOID im_b2r_latest_addr_update( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "im_b2r_latest_addr_update error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk(pipe_no);

	gIM_B2R_Out_Mng[pipe_no].latest_idx = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWBANK.bit.BANKSTS;
	gIM_B2R_Out_Mng[pipe_no].latest_addr = gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[ gIM_B2R_Out_Mng[pipe_no].latest_idx ];

	Im_B2R_Off_Pclk(pipe_no);
}

#ifdef IM_B2R_STATUS_PRINT
// Calculate YBW output image width
static UINT32 im_b2r_calc_ybw_out_width( UCHAR pipe_no )
{
	UINT32 in_pixs;
	UINT32 out_pixs;
	UINT32 ref_pixs;

	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Print(( "im_b2r_calc_ybw_out_width:pipe_no err\n" ));
		return 0;
	}

	Im_B2R_On_Pclk(pipe_no);
	if( gIM_B2R_State[pipe_no].video_photo_mode == D_IM_B2R_MODE_SDRAM_INPUT ) {
		// Trimming is disable and photo mode.
		in_pixs = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRHSIZ.word;
		ref_pixs = gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs;
	}
	else {
		// Trimming is disable and movie mode.
		in_pixs = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINHSIZ.word;
		ref_pixs = gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs;
	}

	out_pixs = ((in_pixs - ref_pixs) & 0x1FFE);
	Im_B2R_Off_Pclk(pipe_no);

	return out_pixs;
}

// Calculate YBW output image lines
static UINT32 im_b2r_calc_ybw_out_lines( UCHAR pipe_no )
{
	UINT32 in_pixs;
	UINT32 out_pixs;
	UINT32 ref_pixs;

	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Print(( "im_b2r_calc_ybw_out_lines:pipe_no err\n" ));
		return 0;
	}

	Im_B2R_On_Pclk(pipe_no);
	if( gIM_B2R_State[pipe_no].video_photo_mode == D_IM_B2R_MODE_SDRAM_INPUT ) {
		// Trimming is disable and photo mode.
		in_pixs = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRVSIZ.word;
		ref_pixs = gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs;
	}
	else {
		// Trimming is disable and movie mode.
		in_pixs = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINVSIZ.word;
		ref_pixs = gIM_B2R_State[pipe_no].ring_pixs_info.ref_pixs;
	}

	out_pixs = ((in_pixs - ref_pixs) & 0x1FFE);
	Im_B2R_Off_Pclk(pipe_no);

	return out_pixs;
}

#endif

/* Is F_B2R active
 */
static UCHAR im_b2r_is_act_fb2r( UCHAR pipe_no )
{
	UCHAR t_b2r_enable;

	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Print(( "im_b2r_is_act_fb2r:pipe_no err\n" ));
		return 0;
	}

	Im_B2R_On_Pclk(pipe_no);
	t_b2r_enable = (gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.CNTL.bit.TRG != D_IM_B2R_FB2RTRG_IDLE)?(D_IM_B2R_ENABLE_ON):(D_IM_B2R_ENABLE_OFF);
	Im_B2R_Off_Pclk(pipe_no);

	return t_b2r_enable;
}

// Control access enable register.
static INT32 im_b2r_set_access_enable( UCHAR pipe_no, volatile T_IM_B2R_ACCESS_ENABLE_MANAGE* const acc_en_mng, const UCHAR acc_enable, const UCHAR wait_enable, const CHAR errmsg[] )
{
	INT32 ret = D_DDIM_OK;
	UCHAR cnt;
	UCHAR loop_cnt, loop_sta, loop_end;

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_B2R_On_Pclk( pipe_no );
	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		cnt = acc_en_mng->ctrl_cnt[loop_cnt];
		if( acc_enable == 0 ){
			if( cnt != 0 ){
				cnt--;
				if( cnt == 0 ){
					if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG != D_IM_B2R_YBWTRG_IDLE ){
						// Macro busy error
						Ddim_Assertion(( errmsg ));
						ret = D_IM_B2R_MACRO_BUSY;
					}
					else{
						acc_en_mng->reg_set_func( loop_cnt, 0 );
					}
				}
			}
		}
		else{
			if( cnt == 0 ){
				if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG != D_IM_B2R_YBWTRG_IDLE ){
					// Macro busy error
					Ddim_Assertion(( errmsg ));
					ret = D_IM_B2R_MACRO_BUSY;
				}
				else{
					cnt++;
					acc_en_mng->reg_set_func( loop_cnt, 1 );

					if( wait_enable != D_IM_B2R_WAIT_OFF ){
						im_b2r_wait_usec( D_IM_B2R_SRAM_WAIT_USEC );
					}
				}
			}
			else if((0 < cnt) && (cnt < 255)){
				cnt++;
			}
		}
		acc_en_mng->ctrl_cnt[loop_cnt] = cnt;
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
	Im_B2R_Off_Pclk( pipe_no );

	return ret;
}

static VOID im_b2r_set_deknee_access_enable( UCHAR pipe_no, const UCHAR enable )
{
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE = enable;
}

static VOID im_b2r_set_rgb_knee_access_enable( UCHAR pipe_no, const UCHAR enable )
{
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE = enable;
}

static VOID im_b2r_get_loop_val( UCHAR pipe_no, UCHAR* loop_sta, UCHAR* loop_end )
{
	switch( pipe_no ){
		case D_IM_B2R_PIPE1:
			*loop_sta = 0;
			*loop_end = 0;
			break;
		case D_IM_B2R_PIPE2:
			*loop_sta = 1;
			*loop_end = 1;
			break;
//		case D_IM_B2R_PIPE12:
		default:
			*loop_sta = 0;
			*loop_end = 1;
			break;
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
#ifdef CO_B2R_RDMA_ON
static VOID im_b2r_start_rdma( T_IM_RDMA_CTRL* ctrl )
{
	INT32 ercd;

	ercd = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_USER_SEM_WAIT_FEVR, ctrl, E_IM_RDMA_INT_MODE_OR );

	if( ercd != D_IM_RDMA_OK ){
		Ddim_Print(( "im_b2r_start_rdma error. %x\n", ercd ));
	}
}

static VOID im_b2r_set_rdma_val_deknee_table( UCHAR pipe_no, const USHORT* const src_tbl )
{
	T_IM_RDMA_CTRL rdma_ctrl = {
		.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Deknee_Tbl_Addr[pipe_no]),
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	rdma_ctrl.transfer_byte = sizeof( T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_VAL );
	rdma_ctrl.reg_data_top_addr = (ULONG)src_tbl;
	im_b2r_start_rdma( &rdma_ctrl );
}

static VOID im_b2r_set_rdma_val_knee_table( UCHAR pipe_no, E_B2R_RGBTBL tbl_type, const UCHAR* const src_tbl )
{
	T_IM_RDMA_CTRL rdma_ctrl = {
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	switch( tbl_type ){
		case E_B2R_RGBTBL_G:
			rdma_ctrl.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Knee_Tbl_G_Addr[pipe_no]);
			break;
		case E_B2R_RGBTBL_B:
			rdma_ctrl.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Knee_Tbl_B_Addr[pipe_no]);
			break;
		default:	// E_B2R_RGBTBL_R
			rdma_ctrl.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Knee_Tbl_R_Addr[pipe_no]);
			break;
	}
	rdma_ctrl.transfer_byte = ( sizeof( U_IM_B2R_CTRL_RDMA_KNEE_TBL_VAL ) + 3 ) & 0xFFF8;
	rdma_ctrl.reg_data_top_addr = (ULONG)src_tbl;
	im_b2r_start_rdma( &rdma_ctrl );
}

static VOID im_b2r_set_rdma_val_offset_ctrl( UCHAR pipe_no, const T_IM_B2R_OFS* const ofs )
{
	T_IM_B2R_CTRL_RDMA_OFST_VAL ofst_val;
	T_IM_RDMA_CTRL rdma_ctrl = {
		.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Ofst_Addr[pipe_no]),
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	im_b2r_set_reg_signed( ofst_val.OFSTRB, union io_b2r_ofstrb, OFSTRR, ofs->R );
	im_b2r_set_reg_signed( ofst_val.OFSTRB, union io_b2r_ofstrb, OFSTBB, ofs->B );
	im_b2r_set_reg_signed( ofst_val.OFSTG,  union io_b2r_ofstg,  OFSTGR, ofs->Gr );
	im_b2r_set_reg_signed( ofst_val.OFSTG,  union io_b2r_ofstg,  OFSTGB, ofs->Gb );

	rdma_ctrl.transfer_byte = sizeof( T_IM_B2R_CTRL_RDMA_OFST_VAL );
	rdma_ctrl.reg_data_top_addr = (ULONG)&ofst_val;
	im_b2r_start_rdma( &rdma_ctrl );
}

static VOID im_b2r_set_rdma_val_wb_slope_ctrl( UCHAR pipe_no, UCHAR slope_gain_enable, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color )
{
	T_IM_B2R_CTRL_RDMA_WB_SLOPE_VAL slope_val;
	T_IM_RDMA_CTRL rdma_ctrl = {
		.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Wb_Slope_Addr[pipe_no]),
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	slope_val.WBHCTL.bit.WBHCEN = slope_gain_enable;
	slope_val.WBSLP.bit.WBSLPRR = b2r_bay_color->R;
	slope_val.WBSLP.bit.WBSLPBB = b2r_bay_color->B;
	slope_val.WBSLP.bit.WBSLPGR = b2r_bay_color->Gr;
	slope_val.WBSLP.bit.WBSLPGB = b2r_bay_color->Gb;

	rdma_ctrl.transfer_byte = sizeof( T_IM_B2R_CTRL_RDMA_WB_SLOPE_VAL );
	rdma_ctrl.reg_data_top_addr = (ULONG)&slope_val;
	im_b2r_start_rdma( &rdma_ctrl );
}

static VOID im_b2r_set_rdma_val_wb_clip_ctrl( UCHAR pipe_no, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color )
{
	T_IM_B2R_CTRL_RDMA_WB_CLIP_VAL clip_val;
	T_IM_RDMA_CTRL rdma_ctrl = {
		.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Wb_Clip_Addr[pipe_no]),
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	clip_val.WBLVRB.bit.WBLVRR = b2r_bay_color->R;
	clip_val.WBLVRB.bit.WBLVBB = b2r_bay_color->B;
	clip_val.WBLVGR.bit.WBLVGR = b2r_bay_color->Gr;
	clip_val.WBLVGR.bit.WBLVGB = b2r_bay_color->Gb;

	rdma_ctrl.transfer_byte = sizeof( T_IM_B2R_CTRL_RDMA_WB_CLIP_VAL );
	rdma_ctrl.reg_data_top_addr = (ULONG)&clip_val;
	im_b2r_start_rdma( &rdma_ctrl );
}

static VOID im_b2r_set_rdma_val_sensitivity_ctrl( UCHAR pipe_no, const T_IM_B2R_CTRL_ADIP* const b2r_ctrl_adip )
{
	T_IM_B2R_CTRL_RDMA_SENSITIVITY_VAL adip_val;
	T_IM_RDMA_CTRL rdma_ctrl = {
		.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Sensitivity_Addr[pipe_no]),
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	adip_val.SHARP.bit.SHARPA      = b2r_ctrl_adip->sharp_level_a;
	adip_val.SHARP.bit.SHARPB      = b2r_ctrl_adip->sharp_level_b;
	adip_val.SHARP.bit.SHARPC      = b2r_ctrl_adip->sharp_level_c;
	adip_val.AIRESY.bit.AIRESYA    = b2r_ctrl_adip->adj_feel_a;
	adip_val.AIRESY.bit.AIRESYB    = b2r_ctrl_adip->adj_feel_b;
	adip_val.LF.bit.LFSTR          = b2r_ctrl_adip->lf_strength;
	adip_val.LF.bit.LFTH           = b2r_ctrl_adip->lf_threshold;
	adip_val.CMC.bit.CMCSCL        = b2r_ctrl_adip->cmc_level;
	adip_val.CMC.bit.CMCLMT        = b2r_ctrl_adip->cmc_limit;
	adip_val.DIRRLITH.bit.DIRRLITH = b2r_ctrl_adip->dir_reliab_threshold;

	rdma_ctrl.transfer_byte = sizeof( T_IM_B2R_CTRL_RDMA_SENSITIVITY_VAL );
	rdma_ctrl.reg_data_top_addr = (ULONG)&adip_val;
	im_b2r_start_rdma( &rdma_ctrl );
}

static VOID im_b2r_set_rdma_val_hpf_ctrl( UCHAR pipe_no, const T_IM_B2R_CTRL_HPF* const b2r_ctrl_hpf )
{
	T_IM_B2R_CTRL_RDMA_HPF_VAL hpf_val;
	T_IM_RDMA_CTRL rdma_ctrl = {
		.reg_addr_tbl_addr = (ULONG)&(gIM_B2R_Hpf_Addr[pipe_no]),
		.req_threshold = E_IM_RDMA_PRCH_CNT_NOLIMIT,
		.pCallBack = NULL,
	};

	hpf_val.HFACTL.bit.HFEEN  = b2r_ctrl_hpf->hpf_enable;
	hpf_val.HFACTL.bit.HFAFEN = b2r_ctrl_hpf->flat_ctrl_enable;
	hpf_val.HFACTL.bit.HFAYEN = b2r_ctrl_hpf->y_ctrl_enable;
	hpf_val.HFK.bit.HFK0      = b2r_ctrl_hpf->hpf_k[0];
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK1, b2r_ctrl_hpf->hpf_k[1] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK2, b2r_ctrl_hpf->hpf_k[2] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK3, b2r_ctrl_hpf->hpf_k[3] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK4, b2r_ctrl_hpf->hpf_k[4] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK5, b2r_ctrl_hpf->hpf_k[5] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK6, b2r_ctrl_hpf->hpf_k[6] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK7, b2r_ctrl_hpf->hpf_k[7] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK8, b2r_ctrl_hpf->hpf_k[8] );
	im_b2r_set_reg_signed_a( hpf_val.HFK, union io_b2r_hfk, HFK9, b2r_ctrl_hpf->hpf_k[9] );
	hpf_val.HFEFCLP.bit.HFEFCLPP = b2r_ctrl_hpf->hpf_level_clip_p;
	hpf_val.HFEFCLP.bit.HFEFCLPM = b2r_ctrl_hpf->hpf_level_clip_m;
	hpf_val.HFAFST.bit.HFAFSL    = b2r_ctrl_hpf->flat_scale;
	hpf_val.HFAFST.bit.HFAFTH    = b2r_ctrl_hpf->flat_threshold;
	hpf_val.HFAFGU.bit.HFAFGU    = b2r_ctrl_hpf->flat_gain_limit;
	hpf_val.HFAFCLP.bit.HFAFCLPP = b2r_ctrl_hpf->flat_level_clip_p;
	hpf_val.HFAFCLP.bit.HFAFCLPM = b2r_ctrl_hpf->flat_level_clip_m;
	hpf_val.HFAYC.bit.HFAYC00    = b2r_ctrl_hpf->y_convert_coef[0];
	hpf_val.HFAYC.bit.HFAYC01    = b2r_ctrl_hpf->y_convert_coef[1];
	hpf_val.HFAYC.bit.HFAYC02    = b2r_ctrl_hpf->y_convert_coef[2];
	im_b2r_set_reg_signed_a( hpf_val.HFAYC, union io_b2r_hfayc, HFAYC10, b2r_ctrl_hpf->y_convert_coef[3] );
	im_b2r_set_reg_signed_a( hpf_val.HFAYC, union io_b2r_hfayc, HFAYC11, b2r_ctrl_hpf->y_convert_coef[4] );
	im_b2r_set_reg_signed_a( hpf_val.HFAYC, union io_b2r_hfayc, HFAYC12, b2r_ctrl_hpf->y_convert_coef[5] );
	im_b2r_set_reg_signed_a( hpf_val.HFAYC, union io_b2r_hfayc, HFAYC20, b2r_ctrl_hpf->y_convert_coef[6] );
	im_b2r_set_reg_signed_a( hpf_val.HFAYC, union io_b2r_hfayc, HFAYC21, b2r_ctrl_hpf->y_convert_coef[7] );
	im_b2r_set_reg_signed_a( hpf_val.HFAYC, union io_b2r_hfayc, HFAYC22, b2r_ctrl_hpf->y_convert_coef[8] );
	hpf_val.HFAYOF.bit.HFAYOF_0 = b2r_ctrl_hpf->y_ctrl_offset[0];
	hpf_val.HFAYOF.bit.HFAYOF_1 = b2r_ctrl_hpf->y_ctrl_offset[1];
	hpf_val.HFAYOF.bit.HFAYOF_2 = b2r_ctrl_hpf->y_ctrl_offset[2];
	hpf_val.HFAYOF.bit.HFAYOF_3 = b2r_ctrl_hpf->y_ctrl_offset[3];
	im_b2r_set_reg_signed( hpf_val.HFAYGA, union io_b2r_hfayga, HFAYGA_0, b2r_ctrl_hpf->y_ctrl_gain[0] );
	im_b2r_set_reg_signed( hpf_val.HFAYGA, union io_b2r_hfayga, HFAYGA_1, b2r_ctrl_hpf->y_ctrl_gain[1] );
	im_b2r_set_reg_signed( hpf_val.HFAYGA, union io_b2r_hfayga, HFAYGA_2, b2r_ctrl_hpf->y_ctrl_gain[2] );
	im_b2r_set_reg_signed( hpf_val.HFAYGA, union io_b2r_hfayga, HFAYGA_3, b2r_ctrl_hpf->y_ctrl_gain[3] );
	hpf_val.HFAYBD.bit.HFAYBD_0     = b2r_ctrl_hpf->y_ctrl_border[0];
	hpf_val.HFAYBD.bit.HFAYBD_1     = b2r_ctrl_hpf->y_ctrl_border[1];
	hpf_val.HFAYBD.bit.HFAYBD_2     = b2r_ctrl_hpf->y_ctrl_border[2];
	hpf_val.HFAYBD.bit.HFAYBD_3     = b2r_ctrl_hpf->y_ctrl_border[3];
	hpf_val.HFAYDIV.bit.HFAYDIV1    = b2r_ctrl_hpf->y_ctrl_gain_posi_dec1;
	hpf_val.HFAYDIV.bit.HFAYDIV2    = b2r_ctrl_hpf->y_ctrl_gain_posi_dec2;
	hpf_val.HFAYCLP.bit.HFAYCLPP    = b2r_ctrl_hpf->y_ctrl_level_clip_p;
	hpf_val.HFAYCLP.bit.HFAYCLPM    = b2r_ctrl_hpf->y_ctrl_level_clip_m;
	hpf_val.HFACOR.bit.HFACORP      = b2r_ctrl_hpf->y_ctrl_cor_p;
	hpf_val.HFACOR.bit.HFACORM      = b2r_ctrl_hpf->y_ctrl_cor_m;
	hpf_val.HFACLP.bit.HFACLPR      = b2r_ctrl_hpf->hpf_add_level_clip_r;
	hpf_val.HFACLP.bit.HFACLPG      = b2r_ctrl_hpf->hpf_add_level_clip_g;
	hpf_val.HFACLP.bit.HFACLPB      = b2r_ctrl_hpf->hpf_add_level_clip_b;
	hpf_val.HFACRVCTL.bit.HFACAE    = b2r_ctrl_hpf->area_a_enable;
	hpf_val.HFACRVCTL.bit.HFACBE    = b2r_ctrl_hpf->area_b_enable;
	hpf_val.HFACRVCTL.bit.HFACRVMRG = b2r_ctrl_hpf->area_marge_method;
	im_b2r_set_reg_signed( hpf_val.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFX1, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus1_x );
	im_b2r_set_reg_signed( hpf_val.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFY1, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus1_y );
	im_b2r_set_reg_signed( hpf_val.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFX2, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus2_x );
	im_b2r_set_reg_signed( hpf_val.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFY2, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus2_y );
	hpf_val.HFACRVASCLOF.bit.HFACRVASCLOF1 = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_offset;
	im_b2r_set_reg_signed( hpf_val.HFACRVASCLGA, union io_b2r_hfacrvasclga, HFACRVASCLGA_0, b2r_ctrl_hpf->area_a_ctrl.c_area_scale_gain[0] );
	im_b2r_set_reg_signed( hpf_val.HFACRVASCLGA, union io_b2r_hfacrvasclga, HFACRVASCLGA_1, b2r_ctrl_hpf->area_a_ctrl.c_area_scale_gain[1] );
	hpf_val.HFACRVASCLBD.bit.HFACRVASCLBD1 = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_border;
	hpf_val.HFACRVASCLCP.bit.HFACRVASCLCPL = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_clip_l;
	hpf_val.HFACRVASCLCP.bit.HFACRVASCLCPH = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_clip_h;
	hpf_val.HFAYASCLGA.bit.HFAYASCLGA_0    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_gain[0];
	hpf_val.HFAYASCLGA.bit.HFAYASCLGA_1    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_gain[1];
	hpf_val.HFAYASCLBD.bit.HFAYASCLBD_0    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_border[0];
	hpf_val.HFAYASCLBD.bit.HFAYASCLBD_1    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_border[1];
	hpf_val.HFAYASCLCLP.bit.HFAYASCLCLPL   = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_clip_l;
	hpf_val.HFAYASCLCLP.bit.HFAYASCLCLPH   = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_clip_h;
	im_b2r_set_reg_signed( hpf_val.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFX1, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus1_x );
	im_b2r_set_reg_signed( hpf_val.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFY1, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus1_y );
	im_b2r_set_reg_signed( hpf_val.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFX2, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus2_x );
	im_b2r_set_reg_signed( hpf_val.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFY2, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus2_y );
	hpf_val.HFACRVBSCLOF.bit.HFACRVBSCLOF1 = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_offset;
	im_b2r_set_reg_signed( hpf_val.HFACRVBSCLGA, union io_b2r_hfacrvbsclga, HFACRVBSCLGA_0, b2r_ctrl_hpf->area_b_ctrl.c_area_scale_gain[0] );
	im_b2r_set_reg_signed( hpf_val.HFACRVBSCLGA, union io_b2r_hfacrvbsclga, HFACRVBSCLGA_1, b2r_ctrl_hpf->area_b_ctrl.c_area_scale_gain[1] );
	hpf_val.HFACRVBSCLBD.bit.HFACRVBSCLBD1 = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_border;
	hpf_val.HFACRVBSCLCP.bit.HFACRVBSCLCPL = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_clip_l;
	hpf_val.HFACRVBSCLCP.bit.HFACRVBSCLCPH = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_clip_h;
	hpf_val.HFAYBSCLGA.bit.HFAYBSCLGA_0    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_gain[0];
	hpf_val.HFAYBSCLGA.bit.HFAYBSCLGA_1    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_gain[1];
	hpf_val.HFAYBSCLBD.bit.HFAYBSCLBD_0    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_border[0];
	hpf_val.HFAYBSCLBD.bit.HFAYBSCLBD_1    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_border[1];
	hpf_val.HFAYBSCLCLP.bit.HFAYBSCLCLPL   = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_clip_l;
	hpf_val.HFAYBSCLCLP.bit.HFAYBSCLCLPH   = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_clip_h;

	rdma_ctrl.transfer_byte = sizeof( T_IM_B2R_CTRL_RDMA_HPF_VAL );
	rdma_ctrl.reg_data_top_addr = (ULONG)&hpf_val;
	im_b2r_start_rdma( &rdma_ctrl );
}

#endif	// CO_B2R_RDMA_ON
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/* B2R PCLK change to ON
 */
INT32 Im_B2R_On_Pclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_PCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_On_Pclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[0], D_IM_B2R1_PCLK_REG_ADDR, ~D_IM_B2R1_PCLK_REG_BIT );
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[1], D_IM_B2R2_PCLK_REG_ADDR, ~D_IM_B2R2_PCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_PCLOCK

	return D_DDIM_OK;
}

/* B2R PCLK change to OFF
 */
INT32 Im_B2R_Off_Pclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_PCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Off_Pclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_B2R_Pclk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_B2R_Pclk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[0], D_IM_B2R1_PCLK_REG_ADDR, D_IM_B2R1_PCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[0], D_IM_B2R1_PCLK_REG_ADDR, D_IM_B2R1_PCLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_B2R_Pclk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_B2R_Pclk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[1], D_IM_B2R2_PCLK_REG_ADDR, D_IM_B2R2_PCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[1], D_IM_B2R2_PCLK_REG_ADDR, D_IM_B2R2_PCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_PCLOCK

	return D_DDIM_OK;
}

/* B2R PCLK change to OFF force
 */
INT32 Im_B2R_Force_Off_Pclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_PCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Force_Off_Pclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_Pclk_Ctrl_Cnt[0] = 1;
		if( gIM_B2R_Pclk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[0], D_IM_B2R1_PCLK_REG_ADDR, D_IM_B2R1_PCLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_Pclk_Ctrl_Cnt[1] = 1;
		if( gIM_B2R_Pclk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Pclk_Ctrl_Cnt[1], D_IM_B2R2_PCLK_REG_ADDR, D_IM_B2R2_PCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_PCLOCK

	return D_DDIM_OK;
}

/* B2R HCLK change to ON
 */
INT32 Im_B2R_On_Hclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_HCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_On_Hclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[0], D_IM_B2R1_HCLK_REG_ADDR, ~D_IM_B2R1_HCLK_REG_BIT );
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[1], D_IM_B2R2_HCLK_REG_ADDR, ~D_IM_B2R2_HCLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_HCLOCK

	return D_DDIM_OK;
}

/* B2R HCLK change to OFF
 */
INT32 Im_B2R_Off_Hclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_HCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Off_Hclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_B2R_Hclk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_B2R_Hclk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[0], D_IM_B2R1_HCLK_REG_ADDR, D_IM_B2R1_HCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[0], D_IM_B2R1_HCLK_REG_ADDR, D_IM_B2R1_HCLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_B2R_Hclk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_B2R_Hclk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[1], D_IM_B2R2_HCLK_REG_ADDR, D_IM_B2R2_HCLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[1], D_IM_B2R2_HCLK_REG_ADDR, D_IM_B2R2_HCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_HCLOCK

	return D_DDIM_OK;
}

/* B2R HCLK change to OFF force
 */
INT32 Im_B2R_Force_Off_Hclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_HCLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Force_Off_Hclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_Hclk_Ctrl_Cnt[0] = 1;
		if( gIM_B2R_Hclk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[0], D_IM_B2R1_HCLK_REG_ADDR, D_IM_B2R1_HCLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_Hclk_Ctrl_Cnt[1] = 1;
		if( gIM_B2R_Hclk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Hclk_Ctrl_Cnt[1], D_IM_B2R2_HCLK_REG_ADDR, D_IM_B2R2_HCLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_HCLOCK

	return D_DDIM_OK;
}

/* B2R ICLK change to ON
 */
INT32 Im_B2R_On_Iclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_ICLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_On_Iclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[0], D_IM_B2R1_ICLK_REG_ADDR, ~D_IM_B2R1_ICLK_REG_BIT );
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[1], D_IM_B2R2_ICLK_REG_ADDR, ~D_IM_B2R2_ICLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_ICLOCK

	return D_DDIM_OK;
}

/* B2R ICLK change to OFF
 */
INT32 Im_B2R_Off_Iclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_ICLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Off_Iclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_B2R_Iclk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_B2R_Iclk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[0], D_IM_B2R1_ICLK_REG_ADDR, D_IM_B2R1_ICLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[0], D_IM_B2R1_ICLK_REG_ADDR, D_IM_B2R1_ICLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_B2R_Iclk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_B2R_Iclk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[1], D_IM_B2R2_ICLK_REG_ADDR, D_IM_B2R2_ICLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[1], D_IM_B2R2_ICLK_REG_ADDR, D_IM_B2R2_ICLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_ICLOCK

	return D_DDIM_OK;
}

/* B2R ICLK change to OFF force
 */
INT32 Im_B2R_Force_Off_Iclk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_ICLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Force_Off_Iclk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_Iclk_Ctrl_Cnt[0] = 1;
		if( gIM_B2R_Iclk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[0], D_IM_B2R1_ICLK_REG_ADDR, D_IM_B2R1_ICLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_Iclk_Ctrl_Cnt[1] = 1;
		if( gIM_B2R_Iclk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Iclk_Ctrl_Cnt[1], D_IM_B2R2_ICLK_REG_ADDR, D_IM_B2R2_ICLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_ICLOCK

	return D_DDIM_OK;
}

/* B2RCLK change to ON
 */
INT32 Im_B2R_On_Clk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_On_Clk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[0], D_IM_B2R1_CLK_REG_ADDR, ~D_IM_B2R1_CLK_REG_BIT );
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		Dd_Top_Start_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[1], D_IM_B2R2_CLK_REG_ADDR, ~D_IM_B2R2_CLK_REG_BIT );
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_CLOCK

	return D_DDIM_OK;
}

/* B2RCLK change to OFF
 */
INT32 Im_B2R_Off_Clk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Off_Clk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		if( gIM_B2R_Clk_Ctrl_Cnt[0] == 1 ) {
			if( gIM_B2R_Clk_Ctrl_Disable[0] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[0], D_IM_B2R1_CLK_REG_ADDR, D_IM_B2R1_CLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[0], D_IM_B2R1_CLK_REG_ADDR, D_IM_B2R1_CLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		if( gIM_B2R_Clk_Ctrl_Cnt[1] == 1 ) {
			if( gIM_B2R_Clk_Ctrl_Disable[1] == 0 ) {
				Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[1], D_IM_B2R2_CLK_REG_ADDR, D_IM_B2R2_CLK_REG_BIT );
			}
		}
		else {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[1], D_IM_B2R2_CLK_REG_ADDR, D_IM_B2R2_CLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_CLOCK

	return D_DDIM_OK;
}

/* B2RCLK change to OFF force
 */
INT32 Im_B2R_Force_Off_Clk( UCHAR pipe_no )
{
#ifdef CO_ACT_B2R_CLOCK
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Force_Off_Clk error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Dd_ARM_Critical_Section_Start(gOsUsr_Spin_Lock);

	if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
		gIM_B2R_Clk_Ctrl_Cnt[0] = 1;
		if( gIM_B2R_Clk_Ctrl_Disable[0] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[0], D_IM_B2R1_CLK_REG_ADDR, D_IM_B2R1_CLK_REG_BIT );
		}
	}
	if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
		gIM_B2R_Clk_Ctrl_Cnt[1] = 1;
		if( gIM_B2R_Clk_Ctrl_Disable[1] == 0 ) {
			Dd_Top_Stop_Clock( (UCHAR*)&gIM_B2R_Clk_Ctrl_Cnt[1], D_IM_B2R2_CLK_REG_ADDR, D_IM_B2R2_CLK_REG_BIT );
		}
	}

	Dd_ARM_Critical_Section_End(gOsUsr_Spin_Lock);
#endif	// CO_ACT_B2R_CLOCK

	return D_DDIM_OK;
}

/* B2R Initialize
 */
INT32 Im_B2R_Init( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Init error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_reset( pipe_no );
	Im_B2R_Dsb();

	// Initalize of Managemnet information & Reset of Internal buffer
	im_b2r_manager_init( pipe_no );

	im_b2r_calc_ring_pixs( pipe_no );

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].top_offset = 0;
		gIM_B2R_State[loop_cnt].was_started = D_IM_B2R_FALSE;
	}

#ifdef CO_DEBUG_ON_PC
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->F_B2R.CNTL.CNTL.bit.TRG = D_IM_B2R_FB2RTRG_IDLE;
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG = D_IM_B2R_YBRTRG_IDLE;
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG = D_IM_B2R_YBWTRG_IDLE;
	}
#endif

	return D_DDIM_OK;
}

/* set AXI bus I/F Control
 */
INT32 Im_B2R_Ctrl_Axi( UCHAR pipe_no, const T_IM_B2R_CTRL_AXI* const b2r_ctrl_axi )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( b2r_ctrl_axi == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_Axi error. b2r_ctrl_axi = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_Axi error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBRAXCTL.bit.YBRCACHE = b2r_ctrl_axi->ybr.cache_type;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBRAXCTL.bit.YBRPROT  = b2r_ctrl_axi->ybr.protect_type;

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXCTL.bit.YBWCACHE_0 = b2r_ctrl_axi->ybw[D_IM_B2R_PORT_0].cache_type;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXCTL.bit.YBWPROT_0  = b2r_ctrl_axi->ybw[D_IM_B2R_PORT_0].protect_type;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXCTL.bit.YBWCACHE_1 = b2r_ctrl_axi->ybw[D_IM_B2R_PORT_1].cache_type;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXCTL.bit.YBWPROT_1  = b2r_ctrl_axi->ybw[D_IM_B2R_PORT_1].protect_type;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXCTL.bit.YBWCACHE_2 = b2r_ctrl_axi->ybw[D_IM_B2R_PORT_2].cache_type;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXCTL.bit.YBWPROT_2  = b2r_ctrl_axi->ybw[D_IM_B2R_PORT_2].protect_type;
	Im_B2R_Off_Pclk( pipe_no );

	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* get AXI bus read channel status
 */
INT32 Im_B2R_Get_AxiReadStat( UCHAR pipe_no, T_IM_B2R_AXI_YBR_STAT* const b2r_axi_read_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( b2r_axi_read_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_AxiReadStat error. b2r_axi_read_stat = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_AxiReadStat error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	b2r_axi_read_stat->ybr_axi_stat = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBRAXSTS.bit.YBRRESP;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus write channel status
 */
INT32 Im_B2R_Get_AxiWriteStat( UCHAR pipe_no, T_IM_B2R_AXI_YBW_STAT* const b2r_axi_write_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( b2r_axi_write_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_AxiWriteStat error. b2r_axi_write_stat = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_AxiWriteStat error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	b2r_axi_write_stat->ybw_axi_stat[D_IM_B2R_PORT_0] = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXSTS.bit.YBBRESP_0;
	b2r_axi_write_stat->ybw_axi_stat[D_IM_B2R_PORT_1] = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXSTS.bit.YBBRESP_1;
	b2r_axi_write_stat->ybw_axi_stat[D_IM_B2R_PORT_2] = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXSTS.bit.YBBRESP_2;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus write channel Transfer mode
 */
INT32 Im_B2R_Get_AxiWriteMode( UCHAR pipe_no, T_IM_B2R_AXI_YBW_MODE* const b2r_axi_write_mode )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( b2r_axi_write_mode == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_AxiWriteMode error. b2r_axi_write_mode = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_AxiWriteMode error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	b2r_axi_write_mode->out_sel      = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXMD.bit.YBWDOS;
	b2r_axi_write_mode->burst_length = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXMD.bit.YBWBL;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* set AXI bus write channel Transfer mode
 */
INT32 Im_B2R_Set_AxiWriteMode( UCHAR pipe_no, const T_IM_B2R_AXI_YBW_MODE* const b2r_axi_write_mode )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( b2r_axi_write_mode == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_AxiWriteMode error. b2r_axi_write_mode = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_AxiWriteMode error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXMD.bit.YBWDOS = b2r_axi_write_mode->out_sel;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXMD.bit.YBWBL  = b2r_axi_write_mode->burst_length;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* B2R Control (each mode common control)
 */
INT32 Im_B2R_Ctrl( UCHAR pipe_no, const T_IM_B2R_CTRL* const b2r_ctrl )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( b2r_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl error. b2r_ctrl = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].bayer_top_pixel  = b2r_ctrl->bayer_top_pixel;
		gIM_B2R_State[loop_cnt].cia_bypass       = b2r_ctrl->cia_bypass;
		gIM_B2R_State[loop_cnt].cia_padding      = b2r_ctrl->cia_padding;
		gIM_B2R_State[loop_cnt].b2r_user_handler = b2r_ctrl->b2r_user_handler;
		gIM_B2R_State[loop_cnt].user_param       = b2r_ctrl->user_param;
	}

	Im_B2R_On_Pclk( pipe_no );

	// YBR
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRCTL.bit.DKNEN = b2r_ctrl->deknee_enable;

	// YBCH
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXMD.bit.YBWDOS        = b2r_ctrl->ybw.axi_write_mode.out_sel;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWAXMD.bit.YBWBL         = b2r_ctrl->ybw.axi_write_mode.burst_length;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWREQMSK.bit.YBWREQMSK_0 = b2r_ctrl->ybw.write_request_mask[D_IM_B2R_PORT_0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWREQMSK.bit.YBWREQMSK_1 = b2r_ctrl->ybw.write_request_mask[D_IM_B2R_PORT_1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBWREQMSK.bit.YBWREQMSK_2 = b2r_ctrl->ybw.write_request_mask[D_IM_B2R_PORT_2];

	// B2R_CMN
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.ELNLV.word  = b2r_ctrl->line_intr.count;
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.B2RMODE.bit.CIABYP = gIM_B2R_State[loop_cnt].cia_bypass;
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.B2RMODE.bit.CIAPDD = gIM_B2R_State[loop_cnt].cia_padding;
	}
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.DINSEL.bit.EXTOEN   = b2r_ctrl->ext_out_en;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.EXALMSL.bit.EXALMSL = b2r_ctrl->ext_out_alarm;

	// YBW
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWCTL.bit.YBWDTYP = b2r_ctrl->ybw.output_dtype;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWCTL.bit.YBWCSE  = b2r_ctrl->ybw_continue_start_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWMD.bit.TRMEN    = b2r_ctrl->ybw.trim_out_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWMD.bit.KNEEN    = b2r_ctrl->ybw.knee_enable;

	Im_B2R_Off_Pclk( pipe_no );

	im_b2r_calc_ring_pixs( pipe_no );

	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* B2R Control (SDRAM input mode control)
 */
INT32 Im_B2R_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_B2R_CTRL_SDRAM_INPUT* const b2r_ctrl_sdram_input )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( b2r_ctrl_sdram_input == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_ModeSDRAMInput error. b2r_ctrl_sdram_input = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_ModeSDRAMInput error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_B2R_On_Pclk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG != D_IM_B2R_YBRTRG_IDLE ){
#ifdef CO_DEBUG_PRINT_IM_B2R
			Ddim_Print(( "Im_B2R_Ctrl_ModeSDRAMInput error. PIPE%d YBR busy\n", loop_cnt ));
#endif
			Im_B2R_Off_Pclk( pipe_no );
			return D_IM_B2R_MACRO_BUSY;
		}
	}

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].input_dtype      = b2r_ctrl_sdram_input->input_dtype;
		gIM_B2R_State[loop_cnt].top_offset       = b2r_ctrl_sdram_input->top_offset;
		gIM_B2R_State[loop_cnt].input_global     = b2r_ctrl_sdram_input->input_global;
		gIM_B2R_State[loop_cnt].video_photo_mode = D_IM_B2R_MODE_SDRAM_INPUT;
	}

	// YBCH
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBRAXMD.bit.YBRBL       = b2r_ctrl_sdram_input->burst_length;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBCH.YBRREQMSK.bit.YBRREQMSK = b2r_ctrl_sdram_input->read_request_mask;

	// B2R_CMN
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.DINSEL.bit.B2BDIN = D_IM_B2R_MODE_SDRAM_INPUT;

	// YBR
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRCTL.bit.YBRDTYP    = b2r_ctrl_sdram_input->input_dtype;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRDEF.bit.YBRDEF     = b2r_ctrl_sdram_input->input_global;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRHBLNK.bit.YBRHBLNK = b2r_ctrl_sdram_input->line_space;

	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* Set Managed Input Address information
 */
INT32 Im_B2R_Set_InAddr_Info( UCHAR pipe_no, const T_IM_B2R_INADDR_INFO* const in_addr )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( in_addr == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_Still_InAddr_Info error. in_addr = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_InAddr_Info error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].in_addr = (ULONG)in_addr->raw.addr;
		im_b2r_set_ybr_address_sdram_input( loop_cnt );
	}

	return D_DDIM_OK;
}

/* Set Managed bank information
 */
INT32 Im_B2R_Set_OutBankInfo( UCHAR pipe_no, const T_IM_B2R_OUTBANK_INFO* const b2r_bank )
{
	INT32 ercd;

#ifdef CO_PARAM_CHECK
	if( b2r_bank == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_OutBank_Info error. b2r_bank = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_OutBankInfo error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	gIM_B2R_Out_Mng[pipe_no].bank_info = *b2r_bank;

	// YBWA
	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWCTL.bit.USEBANK = gIM_B2R_Out_Mng[pipe_no].bank_info.use_bank_num;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_0_0 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_1_0 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_G;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_2_0 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_0].rgb.addr_B;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_0_1 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_1_1 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_G;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_2_1 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_1].rgb.addr_B;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_0_2 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_1_2 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_G;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_2_2 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_2].rgb.addr_B;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_0_3 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_1_3 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_G;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWA.bit.YBWA_2_3 = (ULONG)gIM_B2R_Out_Mng[pipe_no].bank_info.output_addr[D_IM_B2R_YBW_BANK_3].rgb.addr_B;
	Im_B2R_Off_Pclk( pipe_no );

	// Set bank index & top address information
	ercd = Im_B2R_Set_OutBankIndex( pipe_no, gIM_B2R_Out_Mng[pipe_no].bank_info.bank_initial_position );
	if( ercd != D_DDIM_OK ) {
		return ercd;
	}

	return D_DDIM_OK;
}

/* Get bank index value
 */
INT32 Im_B2R_Get_OutBankIndex( UCHAR pipe_no, UCHAR* const bank_index )
{
#ifdef CO_PARAM_CHECK
	if( bank_index == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_BankIndex error. bank_index = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_OutBankIndex error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*bank_index = gIM_B2R_Out_Mng[pipe_no].latest_idx;

	return D_DDIM_OK;
}

/* Set bank index value
 */
INT32 Im_B2R_Set_OutBankIndex( UCHAR pipe_no, UCHAR bank_index )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_OutBankIndex error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWBANK.bit.BANKPS = bank_index;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWBANK.bit.BANKST = 1;
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* Increment bank index value
 */
INT32 Im_B2R_Inc_OutBankIndex( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Inc_OutBankIndex error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_B2R_On_Pclk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKPS = im_b2r_inc_bank_index( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKSTS, gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWCTL.bit.USEBANK +1 );
	}
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWBANK.bit.BANKST = 1;
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* B2R Control (B2B direct connection mode control)
 */
INT32 Im_B2R_Ctrl_ModeB2BDirect( UCHAR pipe_no, const T_IM_B2R_CTRL_B2B_DIRECT* const b2r_ctrl_b2b_direct )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( b2r_ctrl_b2b_direct == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_ModePRODirect error. b2r_ctrl_b2b_direct = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_ModeB2BDirect error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_B2R_On_Pclk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG != D_IM_B2R_YBRTRG_IDLE ){
#ifdef CO_DEBUG_PRINT_IM_B2R
			Ddim_Print(( "Im_B2R_Ctrl_ModeB2BDirect error. PIPE%d YBR busy\n", loop_cnt ));
#endif
			Im_B2R_Off_Pclk( pipe_no );
			return D_IM_B2R_MACRO_BUSY;
		}
	}

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINELS.bit.DINELS     = b2r_ctrl_b2b_direct->even_line_start_timming;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINCTL.bit.FRMSTP     = b2r_ctrl_b2b_direct->frame_stop;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.DINSEL.bit.B2BDIN = D_IM_B2R_MODE_B2B_DIRECT;
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].video_photo_mode = D_IM_B2R_MODE_B2B_DIRECT;
	}

	return D_DDIM_OK;
}

/* B2R rectangle Control
 */
INT32 Im_B2R_Set_Rect( UCHAR pipe_no, const T_IM_B2R_RECT* const b2r_rect_param )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if(b2r_rect_param == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_Rect error. b2r_rect_param = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_Rect error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].input_size = b2r_rect_param->input_size;
		gIM_B2R_State[loop_cnt].input_size.img_width = im_b2r_rounddown_2(gIM_B2R_State[loop_cnt].input_size.img_width);

		if( gIM_B2R_State[loop_cnt].video_photo_mode == D_IM_B2R_MODE_B2B_DIRECT ) {
			im_b2r_calc_in_rect_b2b_direct( loop_cnt );
			im_b2r_set_top_color_b2b_direct( loop_cnt );
		}
		else {
			im_b2r_calc_ybr_address_sdram_input( loop_cnt );
			im_b2r_calc_in_rect_sdram_input( loop_cnt );
			im_b2r_calc_top_color_sdram_input( loop_cnt );
			im_b2r_calc_top_msb_offset_sdram_input( loop_cnt );
		}
		gIM_B2R_Rect_Param[loop_cnt] = *b2r_rect_param;
	}

	// It sets it to the register at once.
	Im_B2R_On_Pclk( pipe_no );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWDEF, union io_b2r_ybwdef, YBWDEF, b2r_rect_param->output_size.output_global_w );
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* B2R get rectangle Control information.
 */
INT32 Im_B2R_Get_Rect_Param( UCHAR pipe_no, T_IM_B2R_RECT* const b2r_rect_param )
{
#ifdef CO_PARAM_CHECK
	if( b2r_rect_param == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_Rect_Param error. b2r_rect_param = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_Rect_Param error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*b2r_rect_param = gIM_B2R_Rect_Param[pipe_no];

	return D_DDIM_OK;
}

/* Get Latest Address information
 */
INT32 Im_B2R_Get_Latest_OutAddr( UCHAR pipe_no, UCHAR* const latest_bank_idx, T_IM_B2R_OUTADDR_INFO* const latest_addr )
{
#ifdef CO_PARAM_CHECK
	if( latest_addr == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_Latest_OutAddr error. latest_addr = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_Latest_OutAddr error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*latest_bank_idx = gIM_B2R_Out_Mng[pipe_no].latest_idx;
	*latest_addr     = gIM_B2R_Out_Mng[pipe_no].latest_addr;

	return D_DDIM_OK;
}

/* Set YBW External I/F output.
*/
INT32 Im_B2R_Set_ExternalIfOutput( UCHAR pipe_no, const UCHAR ext_out_en, const UCHAR ext_out_alarm )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_ExternalIfOutput error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.DINSEL.bit.EXTOEN   = ext_out_en;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.EXALMSL.bit.EXALMSL = ext_out_alarm;
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* Get YBW External I/F output status.
*/
INT32 Im_B2R_Get_ExternalIfOutput( UCHAR pipe_no, UCHAR* const ext_out_en, UCHAR* const ext_out_alarm )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_ExternalIfOutput error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	*ext_out_en    = gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.DINSEL.bit.EXTOEN;
	*ext_out_alarm = gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.EXALMSL.bit.EXALMSL;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* Set Frame stop
 */
INT32 Im_B2R_Set_FrameStop( UCHAR pipe_no, UCHAR onoff )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_FrameStop error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINCTL.bit.FRMSTP = onoff;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRTRG.bit.YBRTRG = D_IM_B2R_YBRTRG_ON;
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* Get YBRch busy status.
 */
INT32 Im_B2R_Get_YbrBusy( UCHAR pipe_no, BOOL* const busy_status )
{
	ULONG ybrtrg;

#ifdef CO_PARAM_CHECK
	if( busy_status == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_YbrBusy error. busy_status = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_YbrBusy error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	ybrtrg = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRTRG.bit.YBRTRG;
	Im_B2R_Off_Pclk( pipe_no );

	if( ybrtrg == D_IM_B2R_YBRTRG_BUSY ) {
		*busy_status = TRUE;
	}
	else {
		*busy_status = FALSE;
	}
	return D_DDIM_OK;
}

/* Get Horizontal ring pixel padding number
 */
INT32 Im_B2R_Get_HRingPixs( UCHAR pipe_no, USHORT* const ring_pixs )
{
#ifdef CO_PARAM_CHECK
	if( ring_pixs == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_HRingPixs error. ring_pixs = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_HRingPixs error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	*ring_pixs = gIM_B2R_State[pipe_no].ring_pixs_info.ring_pixs;

	return D_DDIM_OK;
}

/* Restart B2R process when ybw_continue_start_enable equal D_IM_B2R_ENABLE_OFF.
 */
INT32 Im_B2R_ContStart( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_ContStart error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_B2R_State[loop_cnt].was_started == D_IM_B2R_FALSE ) {
			return D_DDIM_OK;
		}
	}

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_ContStart on pclk\n" ));
#endif
	Im_B2R_On_Pclk( pipe_no );

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRG.bit.YBWTRG = D_IM_B2R_YBWTRG_ON;

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_ContStart off pclk\n" ));
#endif
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* Start B2R process.
 */
INT32 Im_B2R_Start( UCHAR pipe_no, UCHAR start_type )
{
	DDIM_USER_ER		ercd;
	INT32				retcd;
	UCHAR				loop_cnt, loop_sta, loop_end;
	DDIM_USER_FLGPTN	flgptn = 0;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Start error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Start begin\n" ));
#endif


	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Start on pclk\n" ));
#endif

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_B2R_On_Pclk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG != D_IM_B2R_YBRTRG_IDLE ){
			Ddim_Print(( "Im_B2R_Start error. YBR busy\n" ));
			return D_IM_B2R_MACRO_BUSY;
		}
		if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG != D_IM_B2R_YBWTRG_IDLE ){
			Ddim_Print(( "Im_B2R_Start error. YBW busy\n" ));
			return D_IM_B2R_MACRO_BUSY;
		}
		if( im_b2r_is_act_fb2r(loop_cnt) != D_IM_B2R_ENABLE_OFF ){
			Ddim_Print(( "Im_B2R_Start error. F_B2R busy\n" ));
			return D_IM_B2R_MACRO_BUSY;
		}
	}

	while( 1 ) {	// Pclk enable section
		// Clear of All Interrupt enable & Interrupt status
#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start begin initialize\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start initialize interrupt\n" ));
#endif
		// Initialized Interrupt
		gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTE.word = D_IM_B2R_YBINTE_DISABLE;
		gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTF.word = D_IM_B2R_YBINTF_ALL_CLR;

		gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTE.word = D_IM_B2R_B2RINTE_DISABLE;
		gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTF.word = D_IM_B2R_B2RINTF_CLR;

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_B2R_State[loop_cnt].int_status = 0;
		}

#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start enable interrupt\n" ));
#endif
		if( start_type != D_IM_B2R_START_EXTERNAL_IF_ONLY ){
			gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTE.word = D_IM_B2R_YBINTE_ALL_ENABLE;
		}
		else{
			gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTE.word = D_IM_B2R_YBINTE_EXT_ONLY_ENABLE;
		}

		gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTE.word = D_IM_B2R_B2RINTE_ENABLE;


#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start on clk\n" ));
#endif
		Im_B2R_On_Iclk( pipe_no );
		Im_B2R_On_Clk( pipe_no );


		// Set access enable

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			// Bayer Deknee
			if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRCTL.bit.DKNEN != 0 ) {
				Im_B2R_Set_DekneeAccessEnable( loop_cnt, D_IM_B2R_ENABLE_ON, D_IM_B2R_WAIT_OFF );
			}

			// RGB Knee
			if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWMD.bit.KNEEN != 0 ) {
				Im_B2R_Set_RGB_KneeAccessEnable( loop_cnt, D_IM_B2R_ENABLE_ON, D_IM_B2R_WAIT_OFF );
			}
		}


		gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.RAMAEN.bit.RAMAEN_YBR = 1;
		if( start_type != D_IM_B2R_START_EXTERNAL_IF_ONLY ){
			gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.RAMAEN.bit.RAMAEN_YBW = 1;
		}

		gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.FLTAEN.bit.CIAEN = 1;

		im_b2r_wait_usec( D_IM_B2R_SRAM_WAIT_USEC );


#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start clear event flag\n" ));
#endif

		// Clear of Event Flag
		if( im_b2r_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_B2R1_INT_FLG_ALL;
		}
		if( im_b2r_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_B2R2_INT_FLG_ALL;
		}

		ercd = DDIM_User_Clr_Flg( FID_IM_B2R, ~flgptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(( "Im_B2R_Start: invalid FID\n" ));
			retcd = D_IM_B2R_SYSTEM_ERROR;
			break;
		}

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_B2R_State[loop_cnt].was_started = D_IM_B2R_TRUE;

#ifdef CO_DEBUG_PRINT_IM_B2R
			Ddim_Print(( "Im_B2R_Start check macro busy state\n" ));
#endif
#ifndef D_IM_B2R_DEBUG_ON_PC
			Im_B2R_Dsb();
			while( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG == D_IM_B2R_YBRTRG_BUSY ) {
				;	// DO NOTHING
			}

			while( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG == D_IM_B2R_YBWTRG_BUSY ) {
				;	// DO NOTHING
			}
#endif

			if( start_type != D_IM_B2R_START_EXTERNAL_IF_ONLY ){
				gIM_B2R_State[loop_cnt].busy_state |= D_IM_B2R_STATE_BUSY_YBW;
			}
			gIM_B2R_State[loop_cnt].busy_state |= D_IM_B2R_STATE_BUSY_F_B2R;
			gIM_B2R_State[loop_cnt].busy_state |= D_IM_B2R_STATE_BUSY_YBR;
		}

		if( start_type != D_IM_B2R_START_EXTERNAL_IF_ONLY ){
#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start enable YBW\n" ));
#endif
			gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRG.bit.YBWTRG = D_IM_B2R_YBWTRG_ON;
		}

#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start enable F_B2R\n" ));
#endif
		gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.CNTL.bit.TRG = D_IM_B2R_FB2RTRG_ON;
#ifdef CO_DEBUG_ON_PC
		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->F_B2R.CNTL.CNTL.bit.TRG = D_IM_B2R_FB2RTRG_BUSY;
		}
#endif

#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Start enable YBR\n" ));
#endif
		gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRTRG.bit.YBRTRG = D_IM_B2R_YBRTRG_ON;

		retcd = D_DDIM_OK;
		break;
	}

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Start off pclk\n" ));
#endif
	Im_B2R_Off_Pclk( pipe_no );

#ifdef D_IM_B2R_DEBUG_ON_PC
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_B2R_macro_fake_finish[loop_cnt] != 0 ) {
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.YBINTF.word = 0;
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->F_B2R.CNTL.B2RINTF.word = 0;
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.YBINTF.bit.__YBREF = 1;
			if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.ELNLV.bit.ELNLV != 0 ){
				gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.YBINTF.bit.__ELNEF = 1;
			}
			if( start_type != D_IM_B2R_START_EXTERNAL_IF_ONLY ){
				gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.YBINTF.bit.__YBWEF = 1;
				if( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKST != 0 ) {
					gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKST = 0;
					gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKSTS = gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKPS;
				}
			}
			Im_B2R_Int_Handler( loop_cnt );
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->B2R_CMN.YBINTF.word = 0;
			if( start_type != D_IM_B2R_START_EXTERNAL_IF_ONLY ){
				gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKSTS = im_b2r_inc_bank_index( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWBANK.bit.BANKSTS, gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWCTL.bit.USEBANK +1 );
			}
		}
	}
#endif

	Im_B2R_Dsb();

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Start end\n" ));
#endif

	return retcd;
}

/* Stop B2R process
 */
INT32 Im_B2R_Stop( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Stop error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_b2r_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].was_started = D_IM_B2R_FALSE;
	}

	Im_B2R_On_Pclk( pipe_no );
	Im_B2R_On_Iclk( pipe_no );
	Im_B2R_On_Clk( pipe_no );	// D_IM_B2R_FB2RTRG_OFF control accompanies the B2R reset.

	//
	// Stop YBW
	//
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].busy_state &= ~(D_IM_B2R_STATE_BUSY_YBW);
	}
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRG.bit.YBWTRG = D_IM_B2R_YBWTRG_OFF;

	Im_B2R_Dsb();
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		while( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG != D_IM_B2R_YBWTRG_IDLE ) {
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(( "Im_B2R_Stop YBW wait loop. %u\n", gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG ));
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBW.YBWTRG.bit.YBWTRG = D_IM_B2R_YBWTRG_IDLE;
#endif
			;	// DO NOTHING
		}
	}

	//
	// Stop F_B2R
	//
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].busy_state &= ~(D_IM_B2R_STATE_BUSY_F_B2R);
	}
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.CNTL.bit.TRG = D_IM_B2R_FB2RTRG_OFF;
#ifdef CO_DEBUG_ON_PC
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_B2r_Reg_Ptr[loop_cnt]->F_B2R.CNTL.CNTL.bit.TRG = D_IM_B2R_FB2RTRG_IDLE;
	}
#endif

	//
	// Stop YBR
	//
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_B2R_State[loop_cnt].busy_state &= ~(D_IM_B2R_STATE_BUSY_YBR);
	}
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRTRG.bit.YBRTRG = D_IM_B2R_YBRTRG_OFF;

	Im_B2R_Dsb();
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		while( gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG != D_IM_B2R_YBRTRG_IDLE ) {
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(( "Im_B2R_Stop YBR wait loop. %u\n", gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG ));
			gIM_Io_B2r_Reg_Ptr[loop_cnt]->YBR.YBRTRG.bit.YBRTRG = D_IM_B2R_YBRTRG_IDLE;
#endif
			;	// DO NOTHING
		}
	}

	// Clear of All Interrupt disable
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTE.word = D_IM_B2R_YBINTE_DISABLE;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTF.word = D_IM_B2R_YBINTF_ALL_CLR;

	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTE.word = D_IM_B2R_B2RINTE_DISABLE;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTF.word = D_IM_B2R_B2RINTF_CLR;

	// Bayer Deknee
	Im_B2R_Set_DekneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_OFF, D_IM_B2R_WAIT_OFF );

	// RGB Knee
	Im_B2R_Set_RGB_KneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_OFF, D_IM_B2R_WAIT_OFF );

	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.RAMAEN.bit.RAMAEN_YBR = 0;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.RAMAEN.bit.RAMAEN_YBW = 0;

	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.FLTAEN.bit.CIAEN = 0;

	Im_B2R_Force_Off_Clk( pipe_no );
	Im_B2R_Force_Off_Iclk( pipe_no );
	Im_B2R_Force_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

	return D_DDIM_OK;
}

/* Wait end of B2R macro.
 */
INT32 Im_B2R_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout )
{
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_WaitEnd begin\n" ));
#endif

#ifdef CO_PARAM_CHECK
	if( (waiptn & D_IM_B2R_INT_FLG_ALL) != waiptn ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_WaitEnd error. waiptn\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif
	Im_B2R_Dsb();

	switch( DDIM_User_Twai_Flg( FID_IM_B2R, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, tmout ) ) {
		case D_DDIM_USER_E_OK:
#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_WaitEnd twai_flg OK return\n" ));
#endif
			flgptn = waiptn & flgptn;
			ercd = DDIM_User_Clr_Flg( FID_IM_B2R, ~flgptn );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_B2R_Start: invalid FID.\n" ));
				return D_IM_B2R_SYSTEM_ERROR;
			}
			if( p_flgptn != NULL ) {
				*p_flgptn = flgptn;
			}
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_WaitEnd twai_flg TimeOut return\n" ));
#endif
			return D_IM_B2R_TIMEOUT;
		default:
#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_WaitEnd twai_flg NG return\n" ));
#endif
			return D_IM_B2R_PARAM_ERROR;
	}
}

/* B2R Interrupt Handler
 */
VOID Im_B2R_Int_Handler( UCHAR pipe_no )
{
	UINT32			loopcnt;
	DDIM_USER_ER	ercd;
	ULONG			b2r_inte;
	ULONG			b2r_intf;
	ULONG			b2r_intf_clr;
	ULONG			f_b2r_inte;
	ULONG			f_b2r_intf;
	ULONG			f_b2r_intf_clr;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Int_Handler error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Int_Handler begin\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Int_Handler on pclk\n" ));
#endif
	Im_B2R_On_Pclk( pipe_no );

	if( gIM_B2R_State[pipe_no].video_photo_mode == D_IM_B2R_MODE_SDRAM_INPUT ) {
		if( !((gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.YBRTRG.bit.YBRTRG != D_IM_B2R_YBRTRG_IDLE )
		   || (gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRG.bit.YBWTRG != D_IM_B2R_YBWTRG_IDLE )
		   || (gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWCTL.bit.YBWCSE != 0 ))) {
#ifdef CO_DEBUG_PRINT_IM_B2R
			Ddim_Print(( "Im_B2R_Int_Handler off clk\n" ));
#endif
			Im_B2R_Off_Clk( pipe_no );
			Im_B2R_Off_Iclk( pipe_no );
		}
	}
	Im_B2R_Dsb();

	b2r_inte = gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTE.word;
	b2r_intf = gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTF.word;
	b2r_intf_clr = 0;
	f_b2r_inte = gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTE.word;
	f_b2r_intf = gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTF.word;
	f_b2r_intf_clr = 0;

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Int_Handler inte=0x%lx, intf=0x%lx f_inte=0x%lx f_intf=0x%lx\n", b2r_inte, b2r_intf, f_b2r_inte, f_b2r_intf ));
#endif

	// Call of Callback API for interrupt status

	for( loopcnt = 0; loopcnt < (sizeof(gim_b2r_inttbl_jdsb2r[0]) / sizeof(gim_b2r_inttbl_jdsb2r[0][0])); loopcnt++ ) {
		if( ((b2r_intf & gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].flg_bitmask) != 0)
		 && ((b2r_inte & gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].ena_bitmask) != 0) ) {

#ifdef CO_DEBUG_PRINT_IM_B2R
			Ddim_Print(( "Im_B2R_Int_Handler begin JDSB2R 0x%x\n", gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].flg_bitmask ));
#endif

			if(( gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].int_status == D_IM_B2R1_INT_STATE_YBW_END )
			|| ( gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].int_status == D_IM_B2R2_INT_STATE_YBW_END )) {
				im_b2r_latest_addr_update( pipe_no );
			}

			gIM_B2R_State[pipe_no].int_status = gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].int_status;
			gIM_B2R_State[pipe_no].busy_state &= ~(gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].busy_state);

			b2r_intf_clr |= gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].flg_bitmask;

			ercd = DDIM_User_Set_Flg( FID_IM_B2R, gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].waitflg );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_B2R_Int_Handler: invalid FID. 0x%lx\n", gim_b2r_inttbl_jdsb2r[pipe_no][loopcnt].waitflg ));
			}

			if (gIM_B2R_State[pipe_no].b2r_user_handler) {
				gIM_B2R_State[pipe_no].b2r_user_handler( (UINT32*)&gIM_B2R_State[pipe_no].int_status, gIM_B2R_State[pipe_no].user_param );
			}
		}
	}

	if( ((f_b2r_intf & gim_b2r_inttbl_fb2r[pipe_no].flg_bitmask) != 0)
	 && ((f_b2r_inte & gim_b2r_inttbl_fb2r[pipe_no].ena_bitmask) != 0) ) {

#ifdef CO_DEBUG_PRINT_IM_B2R
		Ddim_Print(( "Im_B2R_Int_Handler begin FB2R 0x%x\n", gim_b2r_inttbl_fb2r[pipe_no].flg_bitmask ));
#endif

		gIM_B2R_State[pipe_no].int_status = gim_b2r_inttbl_fb2r[pipe_no].int_status;
		gIM_B2R_State[pipe_no].busy_state &= ~(gim_b2r_inttbl_fb2r[pipe_no].busy_state);

		f_b2r_intf_clr |= gim_b2r_inttbl_fb2r[pipe_no].flg_bitmask;

		ercd = DDIM_User_Set_Flg( FID_IM_B2R, gim_b2r_inttbl_fb2r[pipe_no].waitflg );
		if( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(( "Im_B2R_Int_Handler: invalid FID. 0x%lx\n", gim_b2r_inttbl_fb2r[pipe_no].waitflg ));
		}

		if (gIM_B2R_State[pipe_no].b2r_user_handler) {
			gIM_B2R_State[pipe_no].b2r_user_handler( (UINT32*)&gIM_B2R_State[pipe_no].int_status, gIM_B2R_State[pipe_no].user_param );
		}
	}

	// Clear interrupt
	gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTF.word = b2r_intf_clr;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTF.word = f_b2r_intf_clr;


#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Int_Handler intf=0x%lx, %0xlx\n", gIM_Io_B2r_Reg_Ptr[pipe_no]->B2R_CMN.YBINTF.word, gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CNTL.B2RINTF.word ));
#endif

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Int_Handler off pclk\n" ));
#endif
	Im_B2R_Off_Pclk( pipe_no );
	Im_B2R_Dsb();

#ifdef CO_DEBUG_PRINT_IM_B2R
	Ddim_Print(( "Im_B2R_Int_Handler end\n" ));
#endif
}

/* Get YBR error factor
 */
INT32 Im_B2R_Get_YBR_Error_Factor( UCHAR pipe_no, UINT32* const err_factor )
{
#ifdef CO_PARAM_CHECK
	if( err_factor == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Get_YBR_Error_Factor error. err_factor = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_YBR_Error_Factor error. pipe_no>=D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	*err_factor = gIM_Io_B2r_Reg_Ptr[pipe_no]->YBR.DINERR.word;
	Im_B2R_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Trimming control
 */
INT32 Im_B2R_Ctrl_Trimming( UCHAR pipe_no, const T_IM_B2R_CTRL_TRIMMING* const b2r_ctrl_trimming )
{
	USHORT img_top;
	USHORT img_left;
	USHORT img_width;
	USHORT img_lines;

#ifdef CO_PARAM_CHECK
	if( b2r_ctrl_trimming == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_Trimming error. b2r_ctrl_trimming = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_Trimming error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWMD.bit.TRMEN = b2r_ctrl_trimming->trimming_enable;

	// TRMHSTA
	img_left = b2r_ctrl_trimming->start_x;
	img_left = im_b2r_min( img_left, (USHORT)D_IM_B2R_TRMHSTA_MAX );
	img_left = im_b2r_max( img_left, (USHORT)D_IM_B2R_TRMHSTA_MIN );

	// TRMVSTA
	img_top = b2r_ctrl_trimming->start_y;
	img_top = im_b2r_min( img_top, (USHORT)D_IM_B2R_TRMVSTA_MAX );
	img_top = im_b2r_max( img_top, (USHORT)D_IM_B2R_TRMVSTA_MIN );

	// TRMHSIZ (2pixels alignment & Advancing)
	img_width = b2r_ctrl_trimming->width;
	img_width = im_b2r_min( img_width, (USHORT)D_IM_B2R_TRMHSIZ_MAX );
	img_width = im_b2r_max( img_width, (USHORT)D_IM_B2R_TRMHSIZ_MIN );

	// TRMVSIZ
	img_lines = b2r_ctrl_trimming->lines;
	img_lines = im_b2r_min( img_lines, (USHORT)D_IM_B2R_TRMVSIZ_MAX );
	img_lines = im_b2r_max( img_lines, (USHORT)D_IM_B2R_TRMVSIZ_MIN );

	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRMH.bit.TRMHSTA = img_left;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRMH.bit.TRMHSIZ = img_width;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRMV.bit.TRMVSTA = img_top;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->YBW.YBWTRMV.bit.TRMVSIZ = img_lines;
	Im_B2R_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Set Bayer Deknee table.
 */
INT32 Im_B2R_Set_Deknee_Table( UCHAR pipe_no, const USHORT* const src_tbl )
{
#ifndef CO_B2R_RDMA_ON
	UINT32 loop_cnt;
#endif	// CO_B2R_RDMA_ON
	INT32  ercd;

#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(src_tbl == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_RGBDeknee_Table error. src_tbl = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_Deknee_Table error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	while( 1 ) {
		ercd = Im_B2R_Set_DekneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_ON, D_IM_B2R_WAIT_ON );
		if( ercd != D_DDIM_OK ) {
			break;
		}

#ifdef CO_B2R_RDMA_ON
		im_b2r_set_rdma_val_deknee_table( pipe_no, src_tbl );
#else	// CO_B2R_RDMA_ON
		Im_B2R_On_Hclk( pipe_no );

		for( loop_cnt = 0; loop_cnt < D_IM_B2R_TABLE_MAX_BAYER_DEKNEE; loop_cnt++ ) {
			gIM_Io_B2r_Tbl_Ptr[pipe_no]->DKNTBL.hword[loop_cnt] = src_tbl[loop_cnt];
		}

		Im_B2R_Off_Hclk( pipe_no );
#endif	// CO_B2R_RDMA_ON

		ercd = Im_B2R_Set_DekneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_OFF, D_IM_B2R_WAIT_OFF );
		if( ercd != D_DDIM_OK ) {
			break;
		}

		ercd = D_DDIM_OK;
		break;
	}

	return ercd;
}

/* Set Bayer Deknee access enable
 */
INT32 Im_B2R_Set_DekneeAccessEnable( UCHAR pipe_no, UCHAR access_enable, UCHAR wait_enable )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_DekneeAccessEnable error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	return im_b2r_set_access_enable( pipe_no,
									 &gim_b2r_deknee_accen_ctrl,
									 access_enable,
									 wait_enable,
									 M_IM_B2R_ASSETION_MSG( "Im_B2R_Set_DekneeAccessEnable error. Macro busy\n" )
									);
}

/* Set RGB Knee table.
 */
INT32 Im_B2R_Set_Knee_Table( UCHAR pipe_no, E_B2R_RGBTBL tbl_type, const UCHAR* const src_tbl )
{
#ifndef CO_B2R_RDMA_ON
	UINT32 loop_cnt;
	volatile UCHAR* dst_tbl;
#endif	// CO_B2R_RDMA_ON
	INT32  ercd;

#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(src_tbl == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_Knee_Table error. src_tbl = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_Knee_Table error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	while( 1 ) {
		ercd = Im_B2R_Set_RGB_KneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_ON, D_IM_B2R_WAIT_ON );
		if( ercd != D_DDIM_OK ) {
			break;
		}

#ifdef CO_B2R_RDMA_ON
		im_b2r_set_rdma_val_knee_table( pipe_no, tbl_type, src_tbl );
#else
		Im_B2R_On_Hclk( pipe_no );

		switch( tbl_type ) {
			case E_B2R_RGBTBL_G:
				dst_tbl = gIM_Io_B2r_Tbl_Ptr[pipe_no]->KNETBLG.byte;
				break;
			case E_B2R_RGBTBL_B:
				dst_tbl = gIM_Io_B2r_Tbl_Ptr[pipe_no]->KNETBLB.byte;
				break;
			default:	// E_B2R_RGBTBL_R
				dst_tbl = gIM_Io_B2r_Tbl_Ptr[pipe_no]->KNETBLR.byte;
				break;
		}
		for( loop_cnt = 0; loop_cnt < D_IM_B2R_TABLE_MAX_RGB_KNEE; loop_cnt++ ) {
			dst_tbl[loop_cnt] = src_tbl[loop_cnt];
		}

		Im_B2R_Off_Hclk( pipe_no );
#endif

		ercd = Im_B2R_Set_RGB_KneeAccessEnable( pipe_no, D_IM_B2R_ENABLE_OFF, D_IM_B2R_WAIT_OFF );
		if( ercd != D_DDIM_OK ) {
			break;
		}

		ercd = D_DDIM_OK;
		break;
	}

	return ercd;
}

/* Set RGB Knee access enable
 */
INT32 Im_B2R_Set_RGB_KneeAccessEnable( UCHAR pipe_no, UCHAR access_enable, UCHAR wait_enable )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_RGB_KneeAccessEnable error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	return im_b2r_set_access_enable( pipe_no,
									 &gim_b2r_rgb_knee_accen_ctrl,
									 access_enable,
									 wait_enable,
									 M_IM_B2R_ASSETION_MSG( "Im_B2R_Set_RGB_KneeAccessEnable error. Macro busy\n" )
									);
}

/* Offset Control
 */
INT32 Im_B2R_Set_Offset( UCHAR pipe_no, const T_IM_B2R_OFS* const ofs )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( ofs == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_Offset error. ofs = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_Offset error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

#ifdef CO_B2R_RDMA_ON
	im_b2r_set_rdma_val_offset_ctrl( pipe_no, ofs );
#else	// CO_B2R_RDMA_ON
	Im_B2R_On_Pclk( pipe_no );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.OFSTRB, union io_b2r_ofstrb, OFSTRR, ofs->R );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.OFSTRB, union io_b2r_ofstrb, OFSTBB, ofs->B );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.OFSTG,  union io_b2r_ofstg,  OFSTGR, ofs->Gr );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.OFSTG,  union io_b2r_ofstg,  OFSTGB, ofs->Gb );
	Im_B2R_Off_Pclk( pipe_no );
#endif	// CO_B2R_RDMA_ON

	return D_DDIM_OK;
}

/* Bayer left shift control
 */
INT32 Im_B2R_Ctrl_Left_Shift( UCHAR pipe_no, UCHAR bayer_left_shift, UCHAR saturation_mode )
{
#ifdef CO_PARAM_CHECK
	if( bayer_left_shift > D_IM_B2R_BLSFT_4BIT ) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_Left_Shift error. bayer_left_shift > D_IM_B2R_BLSFT_4BIT\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_Left_Shift error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.BLSFT.bit.BLSFT  = bayer_left_shift;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.BLSFT.bit.BLSFMD = saturation_mode;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* WB Gain control
 */
INT32 Im_B2R_Set_WB_Gain( UCHAR pipe_no, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(b2r_bay_color == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_WB_Gain error. bay_color = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_WB_Gain error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBGARB.bit.WBGARR = b2r_bay_color->R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBGARB.bit.WBGABB = b2r_bay_color->B;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBGAGR.bit.WBGAGR = b2r_bay_color->Gr;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBGAGR.bit.WBGAGB = b2r_bay_color->Gb;
	Im_B2R_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* WB Gain Saturation compensation control
 */
INT32 Im_B2R_Set_WB_Slope_Gain( UCHAR pipe_no, UCHAR slope_gain_enable, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(b2r_bay_color == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_WB_Slope_Gain error. bay_color = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_WB_Slope_Gain error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

#ifdef CO_B2R_RDMA_ON
	im_b2r_set_rdma_val_wb_slope_ctrl( pipe_no, slope_gain_enable, b2r_bay_color );
#else	// CO_B2R_RDMA_ON
	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBHCTL.bit.WBHCEN = slope_gain_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBSLP.bit.WBSLPRR = b2r_bay_color->R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBSLP.bit.WBSLPBB = b2r_bay_color->B;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBSLP.bit.WBSLPGR = b2r_bay_color->Gr;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBSLP.bit.WBSLPGB = b2r_bay_color->Gb;
	Im_B2R_Off_Pclk( pipe_no );
#endif	// CO_B2R_RDMA_ON

	return D_DDIM_OK;
}

/* WB Clip Level control
 */
INT32 Im_B2R_Set_WB_Clip_Level( UCHAR pipe_no, const T_IM_B2R_BAYER_COLOR* const b2r_bay_color )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(b2r_bay_color == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Set_WB_Clip_Level error. b2r_bay_color = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Set_WB_Clip_Level error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

#ifdef CO_B2R_RDMA_ON
	im_b2r_set_rdma_val_wb_clip_ctrl( pipe_no, b2r_bay_color );
#else	// CO_B2R_RDMA_ON
	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBLVRB.bit.WBLVRR = b2r_bay_color->R;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBLVRB.bit.WBLVBB = b2r_bay_color->B;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBLVGR.bit.WBLVGR = b2r_bay_color->Gr;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.OFG.WBLVGR.bit.WBLVGB = b2r_bay_color->Gb;
	Im_B2R_Off_Pclk( pipe_no );
#endif	// CO_B2R_RDMA_ON

	return D_DDIM_OK;
}

/* Sensitivity(ADIP) Control
 */
INT32 Im_B2R_Ctrl_Sensitivity( UCHAR pipe_no, const T_IM_B2R_CTRL_ADIP* const b2r_ctrl_adip )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(b2r_ctrl_adip == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_Sensitivity error. b2r_ctrl_adip = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_Sensitivity error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

#ifdef CO_B2R_RDMA_ON
	im_b2r_set_rdma_val_sensitivity_ctrl( pipe_no, b2r_ctrl_adip );
#else	// CO_B2R_RDMA_ON
	// It sets it to the register at once.
	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.SHARP.bit.SHARPA      = b2r_ctrl_adip->sharp_level_a;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.SHARP.bit.SHARPB      = b2r_ctrl_adip->sharp_level_b;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.SHARP.bit.SHARPC      = b2r_ctrl_adip->sharp_level_c;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.AIRESY.bit.AIRESYA    = b2r_ctrl_adip->adj_feel_a;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.AIRESY.bit.AIRESYB    = b2r_ctrl_adip->adj_feel_b;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.LF.bit.LFSTR          = b2r_ctrl_adip->lf_strength;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.LF.bit.LFTH           = b2r_ctrl_adip->lf_threshold;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.CMC.bit.CMCSCL        = b2r_ctrl_adip->cmc_level;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.CMC.bit.CMCLMT        = b2r_ctrl_adip->cmc_limit;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.CIA.DIRRLITH.bit.DIRRLITH = b2r_ctrl_adip->dir_reliab_threshold;
	Im_B2R_Off_Pclk( pipe_no );
#endif	// CO_B2R_RDMA_ON

	return D_DDIM_OK;
}

/* High pass filter control
 */
INT32 Im_B2R_Ctrl_HighPassFilter( UCHAR pipe_no, const T_IM_B2R_CTRL_HPF* const b2r_ctrl_hpf )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(b2r_ctrl_hpf == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_B2R_Ctrl_HighPassFilter error. b2r_ctrl_cc = NULL\n"));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Ctrl_HighPassFilter error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

#ifdef CO_B2R_RDMA_ON
	im_b2r_set_rdma_val_hpf_ctrl( pipe_no, b2r_ctrl_hpf );
#else	// CO_B2R_RDMA_ON
	Im_B2R_On_Pclk( pipe_no );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACTL.bit.HFEEN  = b2r_ctrl_hpf->hpf_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACTL.bit.HFAFEN = b2r_ctrl_hpf->flat_ctrl_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACTL.bit.HFAYEN = b2r_ctrl_hpf->y_ctrl_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK.bit.HFK0      = b2r_ctrl_hpf->hpf_k[0];
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK1, b2r_ctrl_hpf->hpf_k[1] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK2, b2r_ctrl_hpf->hpf_k[2] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK3, b2r_ctrl_hpf->hpf_k[3] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK4, b2r_ctrl_hpf->hpf_k[4] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK5, b2r_ctrl_hpf->hpf_k[5] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK6, b2r_ctrl_hpf->hpf_k[6] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK7, b2r_ctrl_hpf->hpf_k[7] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK8, b2r_ctrl_hpf->hpf_k[8] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFK, union io_b2r_hfk, HFK9, b2r_ctrl_hpf->hpf_k[9] );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFEFCLP.bit.HFEFCLPP = b2r_ctrl_hpf->hpf_level_clip_p;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFEFCLP.bit.HFEFCLPM = b2r_ctrl_hpf->hpf_level_clip_m;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAFST.bit.HFAFSL    = b2r_ctrl_hpf->flat_scale;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAFST.bit.HFAFTH    = b2r_ctrl_hpf->flat_threshold;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAFGU.bit.HFAFGU    = b2r_ctrl_hpf->flat_gain_limit;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAFCLP.bit.HFAFCLPP = b2r_ctrl_hpf->flat_level_clip_p;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAFCLP.bit.HFAFCLPM = b2r_ctrl_hpf->flat_level_clip_m;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC.bit.HFAYC00    = b2r_ctrl_hpf->y_convert_coef[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC.bit.HFAYC01    = b2r_ctrl_hpf->y_convert_coef[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC.bit.HFAYC02    = b2r_ctrl_hpf->y_convert_coef[2];
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC, union io_b2r_hfayc, HFAYC10, b2r_ctrl_hpf->y_convert_coef[3] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC, union io_b2r_hfayc, HFAYC11, b2r_ctrl_hpf->y_convert_coef[4] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC, union io_b2r_hfayc, HFAYC12, b2r_ctrl_hpf->y_convert_coef[5] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC, union io_b2r_hfayc, HFAYC20, b2r_ctrl_hpf->y_convert_coef[6] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC, union io_b2r_hfayc, HFAYC21, b2r_ctrl_hpf->y_convert_coef[7] );
	im_b2r_set_reg_signed_a( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYC, union io_b2r_hfayc, HFAYC22, b2r_ctrl_hpf->y_convert_coef[8] );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYOF.bit.HFAYOF_0 = b2r_ctrl_hpf->y_ctrl_offset[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYOF.bit.HFAYOF_1 = b2r_ctrl_hpf->y_ctrl_offset[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYOF.bit.HFAYOF_2 = b2r_ctrl_hpf->y_ctrl_offset[2];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYOF.bit.HFAYOF_3 = b2r_ctrl_hpf->y_ctrl_offset[3];
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYGA, union io_b2r_hfayga, HFAYGA_0, b2r_ctrl_hpf->y_ctrl_gain[0] );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYGA, union io_b2r_hfayga, HFAYGA_1, b2r_ctrl_hpf->y_ctrl_gain[1] );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYGA, union io_b2r_hfayga, HFAYGA_2, b2r_ctrl_hpf->y_ctrl_gain[2] );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYGA, union io_b2r_hfayga, HFAYGA_3, b2r_ctrl_hpf->y_ctrl_gain[3] );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBD.bit.HFAYBD_0     = b2r_ctrl_hpf->y_ctrl_border[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBD.bit.HFAYBD_1     = b2r_ctrl_hpf->y_ctrl_border[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBD.bit.HFAYBD_2     = b2r_ctrl_hpf->y_ctrl_border[2];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBD.bit.HFAYBD_3     = b2r_ctrl_hpf->y_ctrl_border[3];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYDIV.bit.HFAYDIV1    = b2r_ctrl_hpf->y_ctrl_gain_posi_dec1;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYDIV.bit.HFAYDIV2    = b2r_ctrl_hpf->y_ctrl_gain_posi_dec2;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYCLP.bit.HFAYCLPP    = b2r_ctrl_hpf->y_ctrl_level_clip_p;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYCLP.bit.HFAYCLPM    = b2r_ctrl_hpf->y_ctrl_level_clip_m;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACOR.bit.HFACORP      = b2r_ctrl_hpf->y_ctrl_cor_p;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACOR.bit.HFACORM      = b2r_ctrl_hpf->y_ctrl_cor_m;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACLP.bit.HFACLPR      = b2r_ctrl_hpf->hpf_add_level_clip_r;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACLP.bit.HFACLPG      = b2r_ctrl_hpf->hpf_add_level_clip_g;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACLP.bit.HFACLPB      = b2r_ctrl_hpf->hpf_add_level_clip_b;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVCTL.bit.HFACAE    = b2r_ctrl_hpf->area_a_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVCTL.bit.HFACBE    = b2r_ctrl_hpf->area_b_enable;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVCTL.bit.HFACRVMRG = b2r_ctrl_hpf->area_marge_method;
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFX1, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus1_x );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFY1, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus1_y );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFX2, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus2_x );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVAFX, union io_b2r_hfacvafx, HFACRVAFY2, b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus2_y );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVASCLOF.bit.HFACRVASCLOF1 = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_offset;
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVASCLGA, union io_b2r_hfacrvasclga, HFACRVASCLGA_0, b2r_ctrl_hpf->area_a_ctrl.c_area_scale_gain[0] );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVASCLGA, union io_b2r_hfacrvasclga, HFACRVASCLGA_1, b2r_ctrl_hpf->area_a_ctrl.c_area_scale_gain[1] );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVASCLBD.bit.HFACRVASCLBD1 = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_border;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPL = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_clip_l;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPH = b2r_ctrl_hpf->area_a_ctrl.c_area_scale_clip_h;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_0    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_gain[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_1    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_gain[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_0    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_border[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_1    = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_border[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPL   = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_clip_l;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPH   = b2r_ctrl_hpf->area_a_ctrl.y_area_scale_clip_h;
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFX1, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus1_x );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFY1, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus1_y );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFX2, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus2_x );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACVBFX, union io_b2r_hfacvbfx, HFACRVBFY2, b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus2_y );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVBSCLOF.bit.HFACRVBSCLOF1 = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_offset;
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVBSCLGA, union io_b2r_hfacrvbsclga, HFACRVBSCLGA_0, b2r_ctrl_hpf->area_b_ctrl.c_area_scale_gain[0] );
	im_b2r_set_reg_signed( gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVBSCLGA, union io_b2r_hfacrvbsclga, HFACRVBSCLGA_1, b2r_ctrl_hpf->area_b_ctrl.c_area_scale_gain[1] );
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVBSCLBD.bit.HFACRVBSCLBD1 = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_border;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPL = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_clip_l;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPH = b2r_ctrl_hpf->area_b_ctrl.c_area_scale_clip_h;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_0    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_gain[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_1    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_gain[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_0    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_border[0];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_1    = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_border[1];
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPL   = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_clip_l;
	gIM_Io_B2r_Reg_Ptr[pipe_no]->F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPH   = b2r_ctrl_hpf->area_b_ctrl.y_area_scale_clip_h;
	Im_B2R_Off_Pclk( pipe_no );
#endif	// CO_B2R_RDMA_ON

	return D_DDIM_OK;
}

#ifdef IM_B2R_STATUS_PRINT
VOID Im_B2R_Print_Status( VOID )
{
	UINT32 loopcnt;
	UINT32 loopcnt2;

	for( loopcnt2 = 0; loopcnt2 < 2; loopcnt2++ ) {
		Ddim_Print(( "PIPE%u:\n", loopcnt2 ));
		Ddim_Print(( "\tOutput pixs:\n" ));
		Ddim_Print(( "\t\tYBW x=%u, y=%u\n", im_b2r_calc_ybw_out_width(loopcnt2), im_b2r_calc_ybw_out_lines(loopcnt2) ));
		Ddim_Print(( "\tgIM_B2R_State:\n" ));
		Ddim_Print(( "\t\twas_started = %u\n", gIM_B2R_State[loopcnt2].was_started ));
		Ddim_Print(( "\t\tvideo_photo_mode = %u\n", gIM_B2R_State[loopcnt2].video_photo_mode ));
		Ddim_Print(( "\t\tbusy_state = %u\n", gIM_B2R_State[loopcnt2].busy_state ));
		Ddim_Print(( "\t\tcia_bypass = %u\n", gIM_B2R_State[loopcnt2].cia_bypass ));
		Ddim_Print(( "\t\tcia_padding = %u\n", gIM_B2R_State[loopcnt2].cia_padding ));
		Ddim_Print(( "\t\tint_status = %u\n", gIM_B2R_State[loopcnt2].int_status ));
		Ddim_Print(( "\t\tb2r_user_handler = 0x%x\n", (UINT32)gIM_B2R_State[loopcnt2].b2r_user_handler ));
		Ddim_Print(( "\t\tuser_param = 0x%x\n", (UINT32)gIM_B2R_State[loopcnt2].user_param ));
		Ddim_Print(( "\t\tinput_dtype = %u\n", gIM_B2R_State[loopcnt2].input_dtype ));
		Ddim_Print(( "\t\tbayer_top_pixel = %u\n", gIM_B2R_State[loopcnt2].bayer_top_pixel ));
		Ddim_Print(( "\t\tin_addr = 0x%x\n", (UINT32)gIM_B2R_State[loopcnt2].in_addr ));
		Ddim_Print(( "\t\tinput_global = %u\n", gIM_B2R_State[loopcnt2].input_global ));
		Ddim_Print(( "\t\ttop_offset = %u\n", gIM_B2R_State[loopcnt2].top_offset ));
		Ddim_Print(( "\t\tring_pixs_info.pad_enable = %u\n", gIM_B2R_State[loopcnt2].ring_pixs_info.pad_enable ));
		Ddim_Print(( "\t\tring_pixs_info.ring_pixs = %u\n", gIM_B2R_State[loopcnt2].ring_pixs_info.ring_pixs ));
		Ddim_Print(( "\t\tring_pixs_info.pad_pixs = %u\n", gIM_B2R_State[loopcnt2].ring_pixs_info.pad_pixs ));
		Ddim_Print(( "\t\tring_pixs_info.ref_pixs = %u\n", gIM_B2R_State[loopcnt2].ring_pixs_info.ref_pixs ));
		Ddim_Print(( "\t\tinput_size.img_top = %u\n", gIM_B2R_State[loopcnt2].input_size.img_top ));
		Ddim_Print(( "\t\tinput_size.img_left = %u\n", gIM_B2R_State[loopcnt2].input_size.img_left ));
		Ddim_Print(( "\t\tinput_size.img_width = %u\n", gIM_B2R_State[loopcnt2].input_size.img_width ));
		Ddim_Print(( "\t\tinput_size.img_lines = %u\n", gIM_B2R_State[loopcnt2].input_size.img_lines ));
		Ddim_Print(( "\t\ttrim.trimming_enable = %u\n", gIM_B2R_State[loopcnt2].trim.trimming_enable ));
		Ddim_Print(( "\t\ttrim.start_x = %u\n", gIM_B2R_State[loopcnt2].trim.start_x ));
		Ddim_Print(( "\t\ttrim.start_y = %u\n", gIM_B2R_State[loopcnt2].trim.start_y ));
		Ddim_Print(( "\t\ttrim.width = %u\n", gIM_B2R_State[loopcnt2].trim.width ));
		Ddim_Print(( "\t\ttrim.lines = %u\n", gIM_B2R_State[loopcnt2].trim.lines ));
		Ddim_Print(( "\tgIM_B2R_Out_Mng[%u]:\n", loopcnt2 ));
		Ddim_Print(( "\t\tlatest_idx = %u\n", gIM_B2R_Out_Mng[loopcnt2].latest_idx ));
		Ddim_Print(( "\t\tbank_initial_position = %u\n", gIM_B2R_Out_Mng[loopcnt2].bank_info.bank_initial_position ));
		Ddim_Print(( "\t\tuse_bank_num = %u\n", gIM_B2R_Out_Mng[loopcnt2].bank_info.use_bank_num ));
		for( loopcnt = 0; loopcnt < D_IM_B2R_YBW_BANK_MAX; loopcnt++ ) {
			Ddim_Print(( "\t\tBankNum = %u\n", loopcnt ));
			Ddim_Print(( "\t\t\tout_addr[R] = 0x%x\n", (UINT32)gIM_B2R_Out_Mng[loopcnt2].bank_info.output_addr[loopcnt].rgb.addr_R ));
			Ddim_Print(( "\t\t\tout_addr[G] = 0x%x\n", (UINT32)gIM_B2R_Out_Mng[loopcnt2].bank_info.output_addr[loopcnt].rgb.addr_G ));
			Ddim_Print(( "\t\t\tout_addr[B] = 0x%x\n", (UINT32)gIM_B2R_Out_Mng[loopcnt2].bank_info.output_addr[loopcnt].rgb.addr_B ));
		}

		Ddim_Print(( "\tgIM_B2R_ybra_ofs_info:\n" ));
		Ddim_Print(( "\t\tofs_x_pixs[%u] = %u\n", loopcnt2, gIM_B2R_ybra_ofs_info[loopcnt2].ofs_x_pixs ));
		Ddim_Print(( "\t\tofs_y_pixs[%u] = %u\n", loopcnt2, gIM_B2R_ybra_ofs_info[loopcnt2].ofs_y_pixs ));
		Ddim_Print(( "\t\tofs_bytes[%u] = %lu\n", loopcnt2, gIM_B2R_ybra_ofs_info[loopcnt2].ofs_bytes ));
	}
}

VOID Im_B2R_Print_ClockStatus( VOID )
{
#ifdef CO_ACT_B2R_CLOCK
	Ddim_Print(( "B2R1CK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1CK ));
	Ddim_Print(( "\tgIM_B2R_Clk_Ctrl_Cnt[0]      = %u\n", gIM_B2R_Clk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_B2R_Clk_Ctrl_Disable[0]  = %u\n", gIM_B2R_Clk_Ctrl_Disable[0] ));
	Ddim_Print(( "B2R2CK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2CK ));
	Ddim_Print(( "\tgIM_B2R_Clk_Ctrl_Cnt[1]      = %u\n", gIM_B2R_Clk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_B2R_Clk_Ctrl_Disable[1]  = %u\n", gIM_B2R_Clk_Ctrl_Disable[1] ));
#endif	// CO_ACT_B2R_CLOCK
#ifdef CO_ACT_B2R_HCLOCK
	Ddim_Print(( "B2R1HCLK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1AH ));
	Ddim_Print(( "\tgIM_B2R_Hclk_Ctrl_Cnt[0]     = %u\n", gIM_B2R_Hclk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_B2R_Hclk_Ctrl_Disable[0] = %u\n", gIM_B2R_Hclk_Ctrl_Disable[0] ));
	Ddim_Print(( "B2R2HCLK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2AH ));
	Ddim_Print(( "\tgIM_B2R_Hclk_Ctrl_Cnt[1]     = %u\n", gIM_B2R_Hclk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_B2R_Hclk_Ctrl_Disable[1] = %u\n", gIM_B2R_Hclk_Ctrl_Disable[1] ));
#endif	// CO_ACT_B2R_HCLOCK
#ifdef CO_ACT_B2R_PCLOCK
	Ddim_Print(( "B2R1PCLK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1AP ));
	Ddim_Print(( "\tgIM_B2R_Pclk_Ctrl_Cnt[0]     = %u\n", gIM_B2R_Pclk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_B2R_Pclk_Ctrl_Disable[0] = %u\n", gIM_B2R_Pclk_Ctrl_Disable[0] ));
	Ddim_Print(( "B2R2PCLK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2AP ));
	Ddim_Print(( "\tgIM_B2R_Pclk_Ctrl_Cnt[1]     = %u\n", gIM_B2R_Pclk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_B2R_Pclk_Ctrl_Disable[1] = %u\n", gIM_B2R_Pclk_Ctrl_Disable[1] ));
#endif	// CO_ACT_B2R_PCLOCK
#ifdef CO_ACT_B2R_ICLOCK
	Ddim_Print(( "B2R1ICLK=%u\n", IO_CHIPTOP.CLKSTOP10.bit.B2R1AX ));
	Ddim_Print(( "\tgIM_B2R_Iclk_Ctrl_Cnt[0]     = %u\n", gIM_B2R_Iclk_Ctrl_Cnt[0] ));
	Ddim_Print(( "\tgIM_B2R_Iclk_Ctrl_Disable[0] = %u\n", gIM_B2R_Iclk_Ctrl_Disable[0] ));
	Ddim_Print(( "B2R2ICLK=%u\n", IO_CHIPTOP.CLKSTOP12.bit.B2R2AX ));
	Ddim_Print(( "\tgIM_B2R_Iclk_Ctrl_Cnt[1]     = %u\n", gIM_B2R_Iclk_Ctrl_Cnt[1] ));
	Ddim_Print(( "\tgIM_B2R_Iclk_Ctrl_Disable[1] = %u\n", gIM_B2R_Iclk_Ctrl_Disable[1] ));
#endif	// CO_ACT_B2R_ICLOCK
	Ddim_Print(( "\n" ));
}

VOID Im_B2R_Print_AccEnStatus( VOID )
{
	UINT32 loopcnt;

	Im_B2R_On_Pclk( D_IM_B2R_PIPE12 );
	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "RAMAEN_BAY_DEKNEE = 0x%x\n", gIM_Io_B2r_Reg_Ptr[loopcnt]->B2R_CMN.RAMAEN.bit.RAMAEN_BAY_DEKNEE ));
		Ddim_Print(( "RAMAEN_RGB_KNEE = 0x%x\n", gIM_Io_B2r_Reg_Ptr[loopcnt]->B2R_CMN.RAMAEN.bit.RAMAEN_RGB_KNEE ));
		Ddim_Print(( "RAMAEN_YBR = 0x%x\n", gIM_Io_B2r_Reg_Ptr[loopcnt]->B2R_CMN.RAMAEN.bit.RAMAEN_YBR ));
		Ddim_Print(( "RAMAEN_YBW = 0x%x\n", gIM_Io_B2r_Reg_Ptr[loopcnt]->B2R_CMN.RAMAEN.bit.RAMAEN_YBW ));
	}
	Im_B2R_Off_Pclk( D_IM_B2R_PIPE12 );
}
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
INT32 Im_B2R_Get_RdmaAddr_Deknee_Table( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_DEKNEE_TBL_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_Deknee_Table error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_B2R_Deknee_Tbl_Addr[pipe_no]);

	return D_DDIM_OK;
}

INT32 Im_B2R_Get_RdmaAddr_Knee_Table( UCHAR pipe_no, E_B2R_RGBTBL tbl_type, const T_IM_B2R_CTRL_RDMA_KNEE_TBL_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( tbl_type >=E_B2R_RGBTBL_MAX ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_Knee_Table error. tbl_type>=E_B2R_RGBTBL_MAX\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_Knee_Table error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	switch( tbl_type ){
		case E_B2R_RGBTBL_R:
			*addr = &(gIM_B2R_Knee_Tbl_R_Addr[pipe_no]);
			break;
		case E_B2R_RGBTBL_G:
			*addr = &(gIM_B2R_Knee_Tbl_G_Addr[pipe_no]);
			break;
		case E_B2R_RGBTBL_B:
			*addr = &(gIM_B2R_Knee_Tbl_B_Addr[pipe_no]);
			break;
		default:
			break;
	}

	return D_DDIM_OK;
}

INT32 Im_B2R_Get_RdmaAddr_Offset_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_OFST_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_Offset_Cntl error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_B2R_Ofst_Addr[pipe_no]);

	return D_DDIM_OK;
}

INT32 Im_B2R_Get_RdmaAddr_WB_Slope_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_WB_SLOPE_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_WB_Slope_Cntl error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_B2R_Wb_Slope_Addr[pipe_no]);

	return D_DDIM_OK;
}

INT32 Im_B2R_Get_RdmaAddr_WB_Clip_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_WB_CLIP_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_WB_Clip_Cntl error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_B2R_Wb_Clip_Addr[pipe_no]);

	return D_DDIM_OK;
}

INT32 Im_B2R_Get_RdmaAddr_Sensitivity_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_SENSITIVITY_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_Sensitivity_Cntl error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_B2R_Sensitivity_Addr[pipe_no]);

	return D_DDIM_OK;
}

INT32 Im_B2R_Get_RdmaAddr_HPF_Cntl( UCHAR pipe_no, const T_IM_B2R_CTRL_RDMA_HPF_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_B2R_PIPE12 ){
		Ddim_Assertion(( "Im_B2R_Get_RdmaAddr_HPF_Cntl error. pipe_no>D_IM_B2R_PIPE12\n" ));
		return D_IM_B2R_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_B2R_Hpf_Addr[pipe_no]);

	return D_DDIM_OK;
}

#endif	// CO_DDIM_UTILITY_USE
