/**
 * @file		im_disp.c
 * @brief		LCD, HDMI, VIDEO display process api<br>
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#include "jdsdisp3a.h"
#include "im_disp.h"
#include "dd_arm.h"
#include "ddim_user_custom.h"
#include <stddef.h>
#include <string.h>

#ifdef SOURCE_CHECKER
#define __builtin_offsetof( type, member ) (1)	// Workaround of stddef.h by gcc-4.9.3
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_DEBUG_DISP
//---------------------- driver  section -------------------------------

#define D_IM_DISP_VSYNC_TIME	(33) // Vertical synchronization period
// Interrupt flag
#define D_IM_DISP_FLG_LEF				(0x00000001)	/**< Main inpuut error flag			*/
#define D_IM_DISP_FLG_GR0EF				(0x00000010)	/**< GR0 error flag					*/
#define D_IM_DISP_FLG_GR1EF				(0x00000020)	/**< GR1 error flag					*/
#define D_IM_DISP_FLG_GA0EF				(0x00000040)	/**< GR0 A component error flag		*/
#define D_IM_DISP_FLG_GA1EF				(0x00000080)	/**< GR1 A component error flag		*/
#define D_IM_DISP_FLG_LREE				(0x00000100)	/**< Main inpuut AXI error flag		*/
#define D_IM_DISP_FLG_GR0REF			(0x00001000)	/**< GR0 AXI error flag				*/
#define D_IM_DISP_FLG_GR1REF			(0x00002000)	/**< GR1 AXI error flag				*/
#define D_IM_DISP_FLG_GA0REF			(0x00004000)	/**< GR0 A component AXI error flag	*/
#define D_IM_DISP_FLG_GA1REF			(0x00008000)	/**< GR1 A component AXI error flag	*/
#define D_IM_DISP_FLG_VE 				(0x00010000)	/**< Main VSYNC flag				*/
#define D_IM_DISP_FLG_VE2 				(0x00020000)	/**< Main VSYNC flag for 2nd field	*/
#define D_IM_DISP_GRID_NUM				(8)				/**< Max number of grid line		*/

// Data Synchronization Barrier.
#define Im_DISP_Dsb()	Dd_ARM_Dsb_Pou()

// Check point debug print.
#ifdef CO_DEBUG_ON_PC
#define Im_DISP_Check_Point		Ddim_Print
#else
#define Im_DISP_Check_Point(dmy)
#endif

//#define IM_DISP_REG_TYPE_CHECK
// The structure type matching check.
#ifdef IM_DISP_REG_TYPE_CHECK
#define im_disp_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_disp_check_reg_type( dst, src )
#endif

// Signed register access macros.
#define im_disp_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_disp_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}

#define im_disp_set_reg_signed_a( reg, type, member, val )		\
		{ \
			type work = { .word[0] = 0xFFFFFFFFUL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_disp_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}

#define im_disp_get_reg_signed( val, reg, type, member )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_disp_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

#define im_disp_get_reg_signed_a( val, reg, type, member )		\
		{ \
			type work = { .word[0] = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member +1); \
			} \
			im_disp_check_reg_type( (reg), work );	/* The structure type matching check. */ \
		}

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

// Startup status of layer
typedef enum {
	E_IM_DISP_TRG_WRITE_NO_ACT = 0x00000000,	// Set TRG to off at once
	E_IM_DISP_TRG_WRITE_ACT,					// Set TRG to on
	E_IM_DISP_TRG_READ_NO_ACT,					// TRG is not on or set to off by frame stop
	E_IM_DISP_TRG_READ_ACT						// TRG is on
} E_IM_DISP_TRG;

// VFTMG value
typedef enum {
	E_IM_DISP_VFTMG_NONE_FLAG = 0x00000000,		// None flag generate
	E_IM_DISP_VFTMG_FIRST_FLAG,					// First field generate a flag
	E_IM_DISP_VFTMG_SECOND_FLAG,				// Second field generate a flag
	E_IM_DISP_VFTMG_BOTH_FLAG,					// Both field generate a flag
} E_IM_DISP_VFTMG;

// for Gamma table set/get
typedef enum {
	E_IM_DISP_ACCESS_GAMMA_SET = 0,	// copy the local gamma table to the gamma region registers
	E_IM_DISP_ACCESS_GAMMA_GET,		// copy the gamma region registers to the local gamma table
} E_IM_DISP_ACCESS_GAMMA;

// Startup status of P2M/PWCH
typedef enum {
	E_IM_DISP_P2M_PWCH_TRG_WRITE_STOP = 0x00000000,		// Set TRG to off by frame stop
	E_IM_DISP_P2M_PWCH_TRG_WRITE_START,					// Set TRG to on
	E_IM_DISP_P2M_PWCH_TRG_READ_NO_ACT,					// TRG is not on, or set to off at once
	E_IM_DISP_P2M_PWCH_TRG_READ_ACT						// TRG is on
} E_IM_DISP_P2M_PWCH_TRG;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**< Interrupt callback functions.	*/
static volatile VP_CALLBACK	gIM_DISP_Interrupt_Callback[D_IM_DISP_BLOCK_NUM][E_IM_DISP_INT_CB_MAX] = {
		{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
		{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};

static volatile VP_CALLBACK	gIM_DISP_DWCH_Interrupt_Callback[D_IM_DISP_BLOCK_NUM][E_IM_DISP_DWCH_INT_CB_MAX] = {
		{NULL, NULL, NULL},
		{NULL, NULL, NULL}};

// OSD layer ID
static volatile const INT32 osd_layer_id[] = {
		E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_SEL_LAYER_OSD_1};

// Write channel block(P2M/PWCH) force stop flag.
static volatile UCHAR gIM_DISP_Wc_Force_Stop = D_IM_DISP_ENABLE_OFF;

// Spin Lock
static ULONG gIM_DISP_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64))) = 0;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define im_disp_set_word_bit(org_data, bit_patten, set_data) (org_data = ((org_data) & (~(bit_patten))) | ((set_data) & (bit_patten)))
#define COMBINE_FLG_INFO( flg, ch )	( (flg) | ( (UINT32)(1 << ch) << 15 ) )

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/* DISP PCLK change to ON
*/
static VOID im_disp_pclk_on(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_On();
#endif	// CO_ACT_PCLOCK
}

/* DISP PCLK change to OFF
*/
static VOID im_disp_pclk_off(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_Off();
#endif	// CO_ACT_PCLOCK
}

/* DISP HCLK change to ON
*/
static VOID im_disp_hclk_on(VOID)
{
#ifdef CO_ACT_HCLOCK
	Im_DISP_AHB_Clock_On();
#endif	// CO_ACT_HCLOCK
}

/* DISP HCLK change to OFF
*/
static VOID im_disp_hclk_off(VOID)
{
#ifdef CO_ACT_HCLOCK
	Im_DISP_AHB_Clock_Off();
#endif	// CO_ACT_HCLOCK
}

/* DISP ICLK change to ON
*/
static VOID im_disp_iclk_on(VOID)
{
#ifdef CO_ACT_ICLOCK
	Dd_Top_Set_CLKSTOP3_DISPAX(2);	// DISP AXI clock on.
#endif	// CO_ACT_ICLOCK
}

/**
wait usec.
@param[in]	usec				micro second.<br>
*/
static VOID im_disp_wait_usec(UINT32 usec)
{
	Im_DISP_Dsb();
	Dd_ARM_Wait_ns(usec * 1000);
}

/**
Copy Gamma table to IO_DISP or IO_DISP to Gamma table.
@param[in]	table_top				gamma table top address.<br>
@param[in]	register_table_top		gamma table register top address.<br>
@param[in]	correct					Correct selection.<br>
									<ul><li>@ref E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA
										<li>@ref E_IM_DISP_CORRECT_SELECT_GAMMA_IN
										<li>@ref E_IM_DISP_CORRECT_SELECT_GAMMA_OUT
										<li>@ref E_IM_DISP_CORRECT_SELECT_LUMINANCE
										<li>@ref E_IM_DISP_CORRECT_SELECT_CHROMA</ul>
@param[in]	access					Set or get.<br>
									<ul><li>@ref E_IM_DISP_ACCESS_GAMMA_SET
										<li>@ref E_IM_DISP_ACCESS_GAMMA_GET</ul>
*/
static VOID im_disp_access_gamma_table( BYTE* table_top,
									 const BYTE* register_table_top,
									 const E_IM_DISP_CORRECT_SELECT correct,
									 const E_IM_DISP_ACCESS_GAMMA access)
{
	INT32 loop;
	INT32 array_num;
	BYTE** tbl_address;
	BYTE* tbl_register_address;

	// table index
	UINT32 anti_gamma_tbl_index[] = {
		offsetof(T_IM_DISP_ANTI_GAMMA_TBL, r_anti),
		offsetof(T_IM_DISP_ANTI_GAMMA_TBL, g_anti),
		offsetof(T_IM_DISP_ANTI_GAMMA_TBL, b_anti)
	};
	UINT32 gamma_tbl_in_index[] = {
		offsetof(T_IM_DISP_GAMMA_TBL_IN, r_data),
		offsetof(T_IM_DISP_GAMMA_TBL_IN, g_data),
		offsetof(T_IM_DISP_GAMMA_TBL_IN, b_data),
	};
	UINT32 gamma_tbl_out_index[] = {
		offsetof(T_IM_DISP_GAMMA_TBL_OUT, r_data),
		offsetof(T_IM_DISP_GAMMA_TBL_OUT, g_data),
		offsetof(T_IM_DISP_GAMMA_TBL_OUT, b_data),
	};
	
	// register table index
	UINT32 anti_gamma_tbl_register_index[] = {
		offsetof(struct io_jdslch_igtbl_rgb, IGTBLR),
		offsetof(struct io_jdslch_igtbl_rgb, IGTBLG),
		offsetof(struct io_jdslch_igtbl_rgb, IGTBLB)
	};
	UINT32 gamma_tbl_in_register_index[] = {
		offsetof(struct io_jdslch_gtbl_rgb, GTBLR),
		offsetof(struct io_jdslch_gtbl_rgb, GTBLG),
		offsetof(struct io_jdslch_gtbl_rgb, GTBLB),
	};
	UINT32 gamma_tbl_out_register_index[] = {
		offsetof(struct io_jdsdcore_gtbl_rgb, GTBLR),
		offsetof(struct io_jdsdcore_gtbl_rgb, GTBLG),
		offsetof(struct io_jdsdcore_gtbl_rgb, GTBLB),
	};

	// table size
	UINT32 anti_gamma_tbl_size[] = {
		sizeof(IO_DISP_TBL.MAIN[0].LCH.IGTBLA.IGTBLR),
		sizeof(IO_DISP_TBL.MAIN[0].LCH.IGTBLA.IGTBLG),
		sizeof(IO_DISP_TBL.MAIN[0].LCH.IGTBLA.IGTBLB)
	};
	UINT32 gamma_tbl_in_size[] = {
		sizeof(IO_DISP_TBL.MAIN[0].LCH.GTBLA.GTBLR),
		sizeof(IO_DISP_TBL.MAIN[0].LCH.GTBLA.GTBLG),
		sizeof(IO_DISP_TBL.MAIN[0].LCH.GTBLA.GTBLB)
	};
	UINT32 gamma_tbl_out_size[] = {
		sizeof(IO_DISP_TBL.MAIN[0].DCORE.GTBLA.GTBLR),
		sizeof(IO_DISP_TBL.MAIN[0].DCORE.GTBLA.GTBLG),
		sizeof(IO_DISP_TBL.MAIN[0].DCORE.GTBLA.GTBLB)
	};

	switch (correct) {
		case E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA:			// Anti Gamma.
			array_num = sizeof(anti_gamma_tbl_index) / sizeof(anti_gamma_tbl_index[0]);
			for(loop = 0; loop < array_num; loop++) {
				tbl_address = (BYTE**)(table_top + anti_gamma_tbl_index[loop]);
				tbl_register_address = (BYTE*)(register_table_top + anti_gamma_tbl_register_index[loop]);
				// check address
				if (((*tbl_address) != NULL) && ((tbl_register_address) != NULL)) {
					if (access == E_IM_DISP_ACCESS_GAMMA_SET){
						// Set table data
						memcpy((tbl_register_address), (*tbl_address), anti_gamma_tbl_size[loop]);
					}
					else {
						// Get table data
						memcpy((*tbl_address), (tbl_register_address), anti_gamma_tbl_size[loop]);
					}
				}
			}
			break;
		case E_IM_DISP_CORRECT_SELECT_GAMMA_IN:				// Gamma (Main data input block).
			array_num = sizeof(gamma_tbl_in_index) / sizeof(gamma_tbl_in_index[0]);
			for(loop = 0; loop < array_num; loop++) {
				tbl_address = (BYTE**)(table_top + gamma_tbl_in_index[loop]);
				tbl_register_address = (BYTE*)(register_table_top + gamma_tbl_in_register_index[loop]);
				// check address
				if (((*tbl_address) != NULL) && ((tbl_register_address) != NULL)) {
					if (access == E_IM_DISP_ACCESS_GAMMA_SET){
						// Set table data
						memcpy((tbl_register_address), (*tbl_address), gamma_tbl_in_size[loop]);
					}
					else {
						// Get table data
						memcpy((*tbl_address), (tbl_register_address), gamma_tbl_in_size[loop]);
					}
				}
			}
			break;
		case E_IM_DISP_CORRECT_SELECT_GAMMA_OUT:				// Gamma (Data output block).
			array_num = sizeof(gamma_tbl_out_index) / sizeof(gamma_tbl_out_index[0]);
			for(loop = 0; loop < array_num; loop++) {
				tbl_address = (BYTE**)(table_top + gamma_tbl_out_index[loop]);
				tbl_register_address = (BYTE*)(register_table_top + gamma_tbl_out_register_index[loop]);
				// check address
				if (((*tbl_address) != NULL) && ((tbl_register_address) != NULL)) {
					if (access == E_IM_DISP_ACCESS_GAMMA_SET){
						// Set table data
						memcpy((tbl_register_address), (*tbl_address), gamma_tbl_out_size[loop]);
					}
					else {
						// Get table data
						memcpy((*tbl_address), (tbl_register_address), gamma_tbl_out_size[loop]);
					}
				}
			}
			break;
		default:												// Luminance or Chroma.
			// check address
			tbl_address = (BYTE**)(table_top);
			tbl_register_address = (BYTE*)(register_table_top);
			if (((*tbl_address) != NULL) && ((tbl_register_address) != NULL)) {
				if (access == E_IM_DISP_ACCESS_GAMMA_SET) {
					// Set table data
					memcpy((tbl_register_address), (*tbl_address), sizeof(IO_DISP_TBL.MAIN[0].DCORE.YSTBLAYT));
				}
				else {
					// Get table data
					memcpy((*tbl_address), (tbl_register_address), sizeof(IO_DISP_TBL.MAIN[0].DCORE.YSTBLAYT));
				}
			}
			break;
	}
}

/**
Wait the channel stop.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer		layer selection.<br>
						(Two or more layer can be specified by the OR value.)<br>
						<ul><li>@ref E_IM_DISP_SEL_LAYER_MAIN
							<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
							<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
							<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	act			E_IM_DISP_TRG_READ_NO_ACT: Wait stop.E_IM_DISP_TRG_READ_ACT: Wait Start.
@retval	D_DDIM_OK				Normal end.
@retval	D_IM_DISP_TIMEOUT		The channel cannot stop within 33ms.
*/
static INT32 im_disp_wait_trg_stop_start(E_IM_DISP_SEL block, UINT32 layer, UINT32 act)
{
	INT32 ret = D_DDIM_OK;
	INT32 wait_counter = 0;
	INT32 loop;
	INT32 array_num;
	UINT32 stopped_channel_bit_map = layer;
	UINT32 trgger;

	UINT32 layer_id[] ={
		E_IM_DISP_SEL_LAYER_MAIN,
		E_IM_DISP_SEL_LAYER_DCORE,
		E_IM_DISP_SEL_LAYER_OSD_0,
		E_IM_DISP_SEL_LAYER_OSD_1
	};

	while(wait_counter < D_IM_DISP_VSYNC_TIME) {
		// Wait tgr stop
		im_disp_pclk_on();
		array_num = sizeof(layer_id) / sizeof(layer_id[0]);
		for(loop = 0; loop < array_num; loop++) {
			switch( stopped_channel_bit_map & layer_id[loop]){
				// this channel had not stopped, yet.
				case E_IM_DISP_SEL_LAYER_MAIN: 
					trgger = IO_DISP.MAIN[block].LCH.LTRG.word;
					break;
				case E_IM_DISP_SEL_LAYER_DCORE:
					trgger = IO_DISP.MAIN[block].DCORE.TRG.bit.TRG;
					break;
				case E_IM_DISP_SEL_LAYER_OSD_0:
					trgger = IO_DISP.MAIN[block].GRCH[0].GRTRG.word;
					break;
				case E_IM_DISP_SEL_LAYER_OSD_1:
					trgger = IO_DISP.MAIN[block].GRCH[1].GRTRG.word;
					break;
				default:
					// Check next trigger
					continue;
			}

			if(trgger == act) {
				stopped_channel_bit_map &= (~(layer_id[loop]));
			}
		}
		im_disp_pclk_off();

		if(stopped_channel_bit_map == 0){
			// The channels want to stop were stopped
			break;
		}
		wait_counter++;
		DDIM_User_Dly_Tsk(1);
#ifdef CO_DEBUG_ON_PC
		stopped_channel_bit_map = 0;	// to avoid time out error.
#endif // CO_DEBUG_ON_PC
	}

	if(stopped_channel_bit_map){
		// Can not stop within 33 ms
		Im_DISP_Check_Point(("CP:im_disp_wait_trg_stop_start() 1\n"));
		ret = D_IM_DISP_TIMEOUT;
	}
	return ret;
}

/**
It is confirmed whether TRG can be set up. 
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	start_stop		Startup status.<br>
							<ul><li>@ref E_IM_DISP_TRG_WRITE_NO_ACT
								<li>@ref E_IM_DISP_TRG_WRITE_ACT
								<li>@ref E_IM_DISP_TRG_READ_NO_ACT
								<li>@ref E_IM_DISP_TRG_READ_ACT</ul>
@retval	D_DDIM_OK				Normal end.
@retval	D_IM_DISP_ACCESS_ERR	TRG cannot be set up.
*/
static INT32 im_disp_is_trg_set(E_IM_DISP_SEL block, E_IM_DISP_TRG start_stop)
{
	INT32 ret = D_DDIM_OK;
	ULONG ifs = 0;

	if(start_stop == E_IM_DISP_TRG_WRITE_ACT) {
		ifs = IO_DISP.MAIN[block].DCORE.IFS.bit.IFS;
		switch (ifs) {
			case D_IM_DISP_IFS_LCD:		// LCD.
				if( Dd_Top_Get_CLKSTOP3_LCDCK() != 0 ) {
					Im_DISP_Check_Point(("CP:im_disp_is_trg_set() : LCD clock stoped.\n"));
					ret = D_IM_DISP_ACCESS_ERR;
				}
				break;
			case D_IM_DISP_IFS_HDMI:	// HDMI.
				if( Dd_Top_Get_CLKSTOP3_HIFCK() != 0 ) {
					Im_DISP_Check_Point(("CP:im_disp_is_trg_set() : HDMI clock stoped.\n"));
					ret = D_IM_DISP_ACCESS_ERR;
				}
				break;
			case D_IM_DISP_IFS_MIPI:	// MIPI.
				if( Dd_Top_Get_CLKSTOP3_MIFCK() != 0 ) {
					Im_DISP_Check_Point(("CP:im_disp_is_trg_set() : MIPI clock stoped.\n"));
					ret = D_IM_DISP_ACCESS_ERR;
				}
				break;
			default:					// Not select.
				Im_DISP_Check_Point(("CP:im_disp_is_trg_set() : Display I/F not select.\n"));
				ret = D_IM_DISP_ACCESS_ERR;
				break;
		}
	}
	else{
		// do nothing.
	}

	return ret;
}

/**
Start/stop Main layer.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			layer selection.<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	write_channel	Write channel block selection.<br>
							Value:<br>
							0 : not select<br>
							1 : Write channel 0 (Luminance data)<br>
							2 : Write channel 1 (Chrominance data)<br>
							3 : Write channel 0 & 1<br>
@param[in]	start_stop		Startup status.<br>
							<ul><li>@ref E_IM_DISP_TRG_WRITE_NO_ACT
								<li>@ref E_IM_DISP_TRG_WRITE_ACT
								<li>@ref E_IM_DISP_TRG_READ_NO_ACT
								<li>@ref E_IM_DISP_TRG_READ_ACT</ul>
@retval	D_DDIM_OK						Normal end.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_TIMEOUT				The channel cannot stop within 33ms.
@retval	D_IM_DISP_ACCESS_ERR			TRG cannot be set up.
*/
static INT32 im_disp_set_trg(E_IM_DISP_SEL block, UINT32 layer, BYTE write_channel, E_IM_DISP_TRG start_stop)
{
	// Wait
	INT32 ret = D_DDIM_OK;
	UINT32 act = E_IM_DISP_TRG_WRITE_NO_ACT;
	E_IM_DISP_P2M_PWCH_TRG wc_trg = E_IM_DISP_P2M_PWCH_TRG_READ_NO_ACT;
	INT32 loop;
	INT32 wc_num[2] = { 1, 2};	// Write channel 0, Write channel 1.
	
	switch(start_stop) {
		case  E_IM_DISP_TRG_WRITE_ACT:		// Start
			// To set start must be at stopping state
			act = E_IM_DISP_TRG_READ_NO_ACT;
			break;
		case E_IM_DISP_TRG_READ_NO_ACT:		// Stop
		case E_IM_DISP_TRG_WRITE_NO_ACT:
			act = E_IM_DISP_TRG_READ_ACT;
			break;
		default:
			// do nothing
			Im_DISP_Check_Point(("CP:im_disp_set_trg() 1\n"));
			ret = D_IM_DISP_INPUT_PARAM_ERROR;
			break;
	}

	if(ret == D_DDIM_OK) {
		// Main data input block
		if ((layer & E_IM_DISP_SEL_LAYER_MAIN) == E_IM_DISP_SEL_LAYER_MAIN) {
			// wait
			if (im_disp_wait_trg_stop_start(block, E_IM_DISP_SEL_LAYER_MAIN, act) == D_DDIM_OK) {
				// check trigger status
				if ((start_stop == E_IM_DISP_TRG_WRITE_NO_ACT) ||
					(start_stop == E_IM_DISP_TRG_READ_NO_ACT) ||
					(IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT)){
					
					// SpinLock.
					Dd_ARM_Critical_Section_Start(gIM_DISP_Spin_Lock);
					
					ret = im_disp_is_trg_set(block, start_stop);
					if(ret == D_DDIM_OK) {
						// Set trigger
						IO_DISP.MAIN[block].LCH.LTRG.word = start_stop;
#ifdef CO_DEBUG_ON_PC
						IO_DISP.MAIN[block].LCH.LTRG.word = (start_stop == E_IM_DISP_TRG_WRITE_ACT) ? E_IM_DISP_TRG_READ_ACT : E_IM_DISP_TRG_READ_NO_ACT;
#endif // CO_DEBUG_ON_PC
					}
					
					// SpinUnLock.
					Dd_ARM_Critical_Section_End(gIM_DISP_Spin_Lock);
				}
			}
		}
	}
	if(ret == D_DDIM_OK) {
		// OSD_0 data input block
		if ((layer & E_IM_DISP_SEL_LAYER_OSD_0) == E_IM_DISP_SEL_LAYER_OSD_0) {
			// wait
			if (im_disp_wait_trg_stop_start(block, E_IM_DISP_SEL_LAYER_OSD_0, act) == D_DDIM_OK) {
				// check trigger status
				if ((start_stop == E_IM_DISP_TRG_WRITE_NO_ACT) ||
					(start_stop == E_IM_DISP_TRG_READ_NO_ACT) ||
					(IO_DISP.MAIN[block].GRCH[0].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT)){
					
					// SpinLock.
					Dd_ARM_Critical_Section_Start(gIM_DISP_Spin_Lock);
					
					ret = im_disp_is_trg_set(block, start_stop);
					if(ret == D_DDIM_OK) {
						// Set trigger
						IO_DISP.MAIN[block].GRCH[0].GRTRG.word = start_stop;
#ifdef CO_DEBUG_ON_PC
						IO_DISP.MAIN[block].GRCH[0].GRTRG.word = (start_stop == E_IM_DISP_TRG_WRITE_ACT) ? E_IM_DISP_TRG_READ_ACT : E_IM_DISP_TRG_READ_NO_ACT;
#endif // CO_DEBUG_ON_PC
					}
					
					// SpinUnLock.
					Dd_ARM_Critical_Section_End(gIM_DISP_Spin_Lock);
				}
			}
		}
	}
	if(ret == D_DDIM_OK) {
		// OSD_1 data input block
		if ((layer & E_IM_DISP_SEL_LAYER_OSD_1) == E_IM_DISP_SEL_LAYER_OSD_1) {
			// wait
			if (im_disp_wait_trg_stop_start(block, E_IM_DISP_SEL_LAYER_OSD_1, act) == D_DDIM_OK) {
				// check trigger status
				if ((start_stop == E_IM_DISP_TRG_WRITE_NO_ACT) ||
					(start_stop == E_IM_DISP_TRG_READ_NO_ACT) ||
					(IO_DISP.MAIN[block].GRCH[1].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT)){
					
					// SpinLock.
					Dd_ARM_Critical_Section_Start(gIM_DISP_Spin_Lock);
					
					ret = im_disp_is_trg_set(block, start_stop);
					if(ret == D_DDIM_OK) {
						// Set trigger
						IO_DISP.MAIN[block].GRCH[1].GRTRG.word = start_stop;
#ifdef CO_DEBUG_ON_PC
						IO_DISP.MAIN[block].GRCH[1].GRTRG.word = (start_stop == E_IM_DISP_TRG_WRITE_ACT) ? E_IM_DISP_TRG_READ_ACT : E_IM_DISP_TRG_READ_NO_ACT;
#endif // CO_DEBUG_ON_PC
					}
					
					// SpinUnLock.
					Dd_ARM_Critical_Section_End(gIM_DISP_Spin_Lock);
				}
			}
		}
	}
	
	if(ret == D_DDIM_OK) {
		// Write channel block(PWCH macro, P2M macro).
		for (loop = 0; loop < 2; loop++) {
			// check write channel number.
			if ((write_channel & wc_num[loop]) == wc_num[loop]) {
				// set write channel trigger.
				if (start_stop == E_IM_DISP_TRG_WRITE_ACT) {
					// After forcibly stopped, if it is not already stopped.
					if ((gIM_DISP_Wc_Force_Stop == D_IM_DISP_ENABLE_ON) &&
						((IO_DISP.PWCH[loop].PWCHTRG.bit.PWCHTRG == E_IM_DISP_P2M_PWCH_TRG_READ_ACT) ||
						 (IO_DISP.P2M[loop].P2MTRG.bit.P2MTRG == E_IM_DISP_P2M_PWCH_TRG_READ_ACT)))
					{
						// not set trigger.
						break;
					}
					
					// start.
					wc_trg = E_IM_DISP_P2M_PWCH_TRG_WRITE_START;
					
					// force stop flag OFF.
					gIM_DISP_Wc_Force_Stop = D_IM_DISP_ENABLE_OFF;
				}
				else if (start_stop == E_IM_DISP_TRG_READ_NO_ACT) {
					// stop.
					wc_trg = E_IM_DISP_P2M_PWCH_TRG_WRITE_STOP;
					
					// force stop flag OFF.
					gIM_DISP_Wc_Force_Stop = D_IM_DISP_ENABLE_OFF;
				}
				else {
					// force stop.
					wc_trg = E_IM_DISP_P2M_PWCH_TRG_READ_NO_ACT;
					
					// force stop flag ON.
					gIM_DISP_Wc_Force_Stop = D_IM_DISP_ENABLE_ON;
				}
				
				// Set PWCH TRG.
				IO_DISP.PWCH[loop].PWCHTRG.bit.PWCHTRG	= wc_trg;
				// Set P2M TRG.
				IO_DISP.P2M[loop].P2MTRG.bit.P2MTRG		= wc_trg;
			}
		}
	}
	
	if(ret == D_DDIM_OK) {
		// Output data input block
		if ((layer & E_IM_DISP_SEL_LAYER_DCORE) == E_IM_DISP_SEL_LAYER_DCORE) {
			// wait
			if (im_disp_wait_trg_stop_start(block, E_IM_DISP_SEL_LAYER_DCORE, act) == D_DDIM_OK) {
				// check trigger status
				if ((start_stop == E_IM_DISP_TRG_WRITE_NO_ACT) ||
					(start_stop == E_IM_DISP_TRG_READ_NO_ACT) ||
					(IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)){
					// DCORE and other layer access
					if (layer != E_IM_DISP_SEL_LAYER_DCORE) {
						// wait
						DDIM_User_Dly_Tsk(20);
					}
					
					// SpinLock.
					Dd_ARM_Critical_Section_Start(gIM_DISP_Spin_Lock);
					
					ret = im_disp_is_trg_set(block, start_stop);
					if(ret == D_DDIM_OK) {
						// Set trigger
						IO_DISP.MAIN[block].DCORE.TRG.bit.TRG = start_stop;
#ifdef CO_DEBUG_ON_PC
						IO_DISP.MAIN[block].DCORE.TRG.bit.TRG = (start_stop == E_IM_DISP_TRG_WRITE_ACT) ? E_IM_DISP_TRG_READ_ACT : E_IM_DISP_TRG_READ_NO_ACT;
#endif // CO_DEBUG_ON_PC
					}
					
					// SpinUnLock.
					Dd_ARM_Critical_Section_End(gIM_DISP_Spin_Lock);
				}
			}
		}
	}
	return ret;
}

/**
Get the event flag of vertical synchronization interrupt.
@param[in]	interrupt_type	interrupt type
@return The event flag.
*/
static UINT32 im_disp_get_event_flag(E_IM_DISP_INTERRUPTION_SELECT interrupt_type)
{
	UINT32 disp_flg;

	// Set GR/Main flag
	switch(interrupt_type) {
		case E_IM_DISP_INTERRUPTION_SELECT_VE:
			disp_flg = D_IM_DISP_FLG_VE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_VE2:
			disp_flg = D_IM_DISP_FLG_VE2;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA0EE:
			disp_flg = D_IM_DISP_FLG_GA0EF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA1EE:
			disp_flg = D_IM_DISP_FLG_GA1EF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR0EE:
			disp_flg = D_IM_DISP_FLG_GR0EF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR1EE:
			disp_flg = D_IM_DISP_FLG_GR1EF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_LEE:
			disp_flg = D_IM_DISP_FLG_LEF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA0REE:
			disp_flg = D_IM_DISP_FLG_GA0REF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA1REE:
			disp_flg = D_IM_DISP_FLG_GA1REF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR0REE:
			disp_flg = D_IM_DISP_FLG_GR0REF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR1REE:
			disp_flg = D_IM_DISP_FLG_GR1REF;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_LREE:
			disp_flg = D_IM_DISP_FLG_LREE;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_VE:
			disp_flg =  D_IM_DISP_FLG_VE |
						D_IM_DISP_FLG_VE2;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_GRREE:
			disp_flg =  D_IM_DISP_FLG_GR0REF |
						D_IM_DISP_FLG_GR1REF |
						D_IM_DISP_FLG_GA0REF |
						D_IM_DISP_FLG_GA1REF;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_GREE:
			disp_flg =  D_IM_DISP_FLG_GR0EF |
						D_IM_DISP_FLG_GR1EF |
						D_IM_DISP_FLG_GA0EF |
						D_IM_DISP_FLG_GA1EF;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_REE:
			disp_flg =  D_IM_DISP_FLG_GR0EF |
						D_IM_DISP_FLG_GR1EF |
						D_IM_DISP_FLG_GA0EF |
						D_IM_DISP_FLG_GA1EF |
						D_IM_DISP_FLG_LREE;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_EE:
			disp_flg =  D_IM_DISP_FLG_GR0EF |
						D_IM_DISP_FLG_GR1EF |
						D_IM_DISP_FLG_GA0EF |
						D_IM_DISP_FLG_GA1EF |
						D_IM_DISP_FLG_LEF;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_GR_ERROR:
			disp_flg =  D_IM_DISP_FLG_GR0EF |
						D_IM_DISP_FLG_GR1EF |
						D_IM_DISP_FLG_GA0EF |
						D_IM_DISP_FLG_GA1EF |
						D_IM_DISP_FLG_GR0REF |
						D_IM_DISP_FLG_GR1REF |
						D_IM_DISP_FLG_GA0REF |
						D_IM_DISP_FLG_GA1REF;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_MIAN_ERROR:
			disp_flg =  D_IM_DISP_FLG_LREE |
						D_IM_DISP_FLG_LEF;
			break;
		case E_IM_DISP_CORRECT_SELECT_ALL_ERROR:
			disp_flg =  D_IM_DISP_FLG_LEF |
						D_IM_DISP_FLG_GR0EF |
						D_IM_DISP_FLG_GR1EF |
						D_IM_DISP_FLG_GA0EF |
						D_IM_DISP_FLG_GA1EF |
						D_IM_DISP_FLG_LREE |
						D_IM_DISP_FLG_GR0REF |
						D_IM_DISP_FLG_GR1REF |
						D_IM_DISP_FLG_GA0REF |
						D_IM_DISP_FLG_GA1REF;
			break;
		default: // E_IM_DISP_CORRECT_SELECT_ALL
			disp_flg =  D_IM_DISP_FLG_LEF |
						D_IM_DISP_FLG_GR0EF |
						D_IM_DISP_FLG_GR1EF |
						D_IM_DISP_FLG_GA0EF |
						D_IM_DISP_FLG_GA1EF |
						D_IM_DISP_FLG_LREE |
						D_IM_DISP_FLG_GR0REF |
						D_IM_DISP_FLG_GR1REF |
						D_IM_DISP_FLG_GA0REF |
						D_IM_DISP_FLG_GA1REF |
						D_IM_DISP_FLG_VE |
						D_IM_DISP_FLG_VE2;
			break;
	}

	return disp_flg;
}

/**
Get the event flag of dwch interrupt.
@param[in]	interrupt_type	interrupt type
@return The event flag.
*/
static UINT32 im_disp_get_dwch_event_flag(E_IM_DISP_DWCH_INTERRUPTION_SELECT interrupt_type)
{
	UINT32 disp_flg;

	// Set GR/Main flag
	switch(interrupt_type) {
		case E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE:
			disp_flg =  D_IM_DISP_DWCH_FLG_PWF;
			break;
		case E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE:
			disp_flg =  D_IM_DISP_DWCH_FLG_PWEF;
			break;
		case E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE:
			disp_flg =  D_IM_DISP_DWCH_FLG_PWXF;
			break;
		default: //E_IM_DISP_DWCH_CORRECT_SELECT_ALL
			disp_flg =  D_IM_DISP_DWCH_FLG_PWF  |
						D_IM_DISP_DWCH_FLG_PWEF |
						D_IM_DISP_DWCH_FLG_PWXF;
			break;
		break;
	}

	return disp_flg;

}
/**
Get call back function pointer array's index
@param[in]	interrupt_type	interrupt's type
@return call back function pointer array's index.
*/
static INT32 im_disp_get_cb_index(E_IM_DISP_INTERRUPTION_SELECT interrupt_type)
{
	INT32 cb_index;
	switch(interrupt_type) {
		case E_IM_DISP_INTERRUPTION_SELECT_VE:
			cb_index = E_IM_DISP_INT_CB_VE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_VE2:
			cb_index = E_IM_DISP_INT_CB_VE2;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR0EE:
			cb_index = E_IM_DISP_INT_CB_GR0EE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR1EE:
			cb_index = E_IM_DISP_INT_CB_GR1EE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA0EE:
			cb_index = E_IM_DISP_INT_CB_GA0EE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA1EE:
			cb_index = E_IM_DISP_INT_CB_GA1EE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_LEE:
			cb_index = E_IM_DISP_INT_CB_LEE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR0REE:
			cb_index = E_IM_DISP_INT_CB_GR0REE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GR1REE:
			cb_index = E_IM_DISP_INT_CB_GR1REE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA0REE:
			cb_index = E_IM_DISP_INT_CB_GA0REE;
			break;
		case E_IM_DISP_INTERRUPTION_SELECT_GA1REE:
			cb_index = E_IM_DISP_INT_CB_GA1REE;
			break;
		default: // E_IM_DISP_INTERRUPTION_SELECT_LREE,
			cb_index = E_IM_DISP_INT_CB_LREE;
			break;
	}
	return cb_index;
}

/**
Get call back function pointer array's index for DWCH
@param[in]	interrupt_type	interrupt's type
@return call back function pointer array's index.
*/
static INT32 im_disp_get_dwch_cb_index(E_IM_DISP_DWCH_INTERRUPTION_SELECT interrupt_type)
{
	INT32 cb_index;
	switch(interrupt_type) {
		case E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE:
			cb_index = E_IM_DISP_DWCH_INT_CB_PWE;
			break;
		case E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE:
			cb_index = E_IM_DISP_DWCH_INT_CB_PWEE;
			break;
		default:	// E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE
			cb_index = E_IM_DISP_DWCH_INT_CB_PWXE;
			break;
	}
	return cb_index;
}

#ifdef CO_PARAM_CHECK
/**
Check input data transfer parameter.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	lidt			Input data forwarding setting register.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_lidt(E_IM_DISP_SEL block, U_IM_DISP_LIDT lidt)
{
	INT32 ret = D_DDIM_OK;

	if (block == E_IM_DISP_HDMI) {
		// HDMI.
		if ((lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT) &&
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC422_RP_12BIT_PACK) &&
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC422_RP_10BIT_PACK) && 
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT) && 
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC420_RP_10BIT_PACK) && 
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_GENERAL_10BIT) &&
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_GENERAL_10BITx2)) {
			Im_DISP_Check_Point(("CP:im_disp_param_check_lidt() 1\n"));
			ret = D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}
	else {
		// LCD/MIPI.
		if ((lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT) &&
			(lidt.bit.IFMT != D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT)) {
			Im_DISP_Check_Point(("CP:im_disp_param_check_lidt() 2\n"));
			ret = D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}

	return ret;
}

/**
Check LISIZE parameter.
@param[in]	lisize			width and height of input data buffer.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_lisize(U_IM_DISP_SIZE lisize)
{
	INT32 ret = D_DDIM_OK;

	if (lisize.size.width < 128) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_lisize() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((lisize.size.width & 0x00000001) != 0) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_lisize() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if (lisize.size.lines < 16) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_lisize() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((lisize.size.lines & 0x00000001) != 0) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_lisize() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check LDSTA parameter.
@param[in]	ldsta			Display position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_ldsta(U_IM_DISP_DSTA ldsta)
{
	INT32 ret = D_DDIM_OK;

	if ((ldsta.bit.DSH & 0x00000001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_ldsta() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((ldsta.bit.DSV & 0x00000001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_ldsta() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check global size parameter.
@param[in]	y_hga			Global width of Y.<br>
							Value range:2~65535<br>
							Bit depth=8bit -> 2-byte unit<br>
							Bit depth=12bit -> 3-byte unit<br>
							Bit depth=16bit -> 4-byte unit<br>
@param[in]	c_hga			Global width of CbCr.<br>
							Value range:2~65535<br>
							Bit depth=8bit -> 2-byte unit<br>
							Bit depth=12bit -> 3-byte unit<br>
							Bit depth=16bit -> 4-byte unit<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_lfd(ULONG y_hga, ULONG c_hga)
{
	INT32 ret = D_DDIM_OK;

	if (y_hga == 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_lfd() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((y_hga & 0x00000001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_lfd() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (c_hga == 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_lfd() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((c_hga & 0x00000001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_lfd() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check resize value parameter.
@param[in]	rsz0			Resize method<br>
							 <ul><li>@ref E_IM_DISP_RSZSL_PADDING_THINNING
								 <li>@ref E_IM_DISP_RSZSL_BILINEAR</ul>
@param[in]	rsz1			Resize data for horizontal.
@param[in]	rsz2			Resize data for vertical.
@param[in]	rsz3			Chrominance start position adjustment.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_resize(E_IM_DISP_RSZSL rsz0, U_IM_DISP_RSZ1 rsz1, U_IM_DISP_RSZ2 rsz2, U_IM_DISP_RSZ3 rsz3)
{
	INT32 ret = D_DDIM_OK;

	if ((double)rsz1.bit.HRSZM / rsz1.bit.HRSZN < 0.5){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((double)rsz1.bit.HRSZM / rsz1.bit.HRSZN > 8.0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((rsz0 == E_IM_DISP_RSZSL_PADDING_THINNING) && (rsz1.bit.HRSZOF != 0)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((double)rsz1.bit.HRSZM / rsz1.bit.HRSZN == 1.0) && (rsz1.bit.HRSZOF != 0)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (rsz1.bit.HRSZOF >= rsz1.bit.HRSZM){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 5\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((double)rsz2.bit.VRSZM / rsz2.bit.VRSZN < 0.5){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 6\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((double)rsz2.bit.VRSZM / rsz2.bit.VRSZN > 8){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 7\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((rsz0 == E_IM_DISP_RSZSL_PADDING_THINNING) && (rsz2.bit.VRSZOF != 0)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 8\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((double)rsz2.bit.VRSZM / rsz2.bit.VRSZN == 1.0) && (rsz2.bit.VRSZOF != 0)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 9\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (rsz2.bit.VRSZOF >= rsz2.bit.VRSZM){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 10\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((rsz0 == E_IM_DISP_RSZSL_PADDING_THINNING) && (rsz3.bit.HCSTA != 0)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 11\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((rsz0 == E_IM_DISP_RSZSL_PADDING_THINNING) && (rsz3.bit.VCSTA != 0)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_resize() 12\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check warning parameter.
@param[in]	warning			Warning processing data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_warning(T_IM_DISP_WARNING const *const warning)
{
	INT32 ret = D_DDIM_OK;

	if (warning->lyw0th.bit.YWTHH < warning->lyw0th.bit.YWTHL) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_warning() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if (warning->lyw1th.bit.YWTHH < warning->lyw1th.bit.YWTHL) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_warning() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((warning->lywctl.bit.YWSL == E_IM_DISP_YWSL_BRIGHTNESS_SPECIFIED_COLOR) ||
		(warning->lywctl.bit.YWSL == E_IM_DISP_YWSL_BRIGHTNESS_ZEBRA)) {
		
		if ((warning->lyw0st == E_IM_DISP_YWMD_G) || (warning->lyw0st == E_IM_DISP_YWMD_B)) {
			Im_DISP_Check_Point(("CP:im_disp_param_check_warning() 3\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}

		if ((warning->lyw1st == E_IM_DISP_YWMD_G) || (warning->lyw1st == E_IM_DISP_YWMD_B)) {
			Im_DISP_Check_Point(("CP:im_disp_param_check_warning() 4\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}
	
	if ((warning->lbost != D_IM_DISP_BOMD_MASTER) && (warning->lbost != D_IM_DISP_BOMD_SLAVE)) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_warning() 5\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check input data zebra warning parameter.
@param[in]	zebra			Setting of zebra warning.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_zebra(T_IM_DISP_ZEBRA const *const zebra)
{
	INT32 ret = D_DDIM_OK;
	
	if (zebra->lzbwid.bit.ZBHH < 2) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_zebra() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (zebra->lzbwid.bit.ZBHL < 2) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_zebra() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check blend order parameter.
@param[in]	bldctl			Blend order data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_blend(U_IM_DISP_BLDCTL bldctl)
{
	INT32 ret = D_DDIM_OK;

	if (bldctl.bit.BLD1 == bldctl.bit.BLD2){
		Im_DISP_Check_Point(("CP:im_disp_param_check_blend() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bldctl.bit.BLD1 == bldctl.bit.BLD3){
		Im_DISP_Check_Point(("CP:im_disp_param_check_blend() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bldctl.bit.BLD1 == bldctl.bit.BLD4){
		Im_DISP_Check_Point(("CP:im_disp_param_check_blend() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bldctl.bit.BLD2 == bldctl.bit.BLD3){
		Im_DISP_Check_Point(("CP:im_disp_param_check_blend() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bldctl.bit.BLD2 == bldctl.bit.BLD4){
		Im_DISP_Check_Point(("CP:im_disp_param_check_blend() 5\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bldctl.bit.BLD3 == bldctl.bit.BLD4){
		Im_DISP_Check_Point(("CP:im_disp_param_check_blend() 6\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check grid parameter.
@param[in]	grid			Grid data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grid(T_IM_DISP_GRID const *const grid)
{
	INT32 ret = D_DDIM_OK;

	if ((grid->ghdsta.bit.DSH & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((grid->ghdsta.bit.DSV & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((grid->gvdsta.bit.DSH & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((grid->gvdsta.bit.DSV & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grid->glength.bit.GHLEN & 0x0001) != 0) || (grid->glength.bit.GHLEN < 8)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 5\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grid->glength.bit.GVLEN & 0x0001) != 0) || (grid->glength.bit.GVLEN < 8)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 6\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grid->gwidth.bit.GHWID & 0x01) != 0) || (grid->gwidth.bit.GHWID < 2)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 7\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grid->gwidth.bit.GVWID & 0x01) != 0) || (grid->gwidth.bit.GVWID < 2)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 8\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grid->gitvl.bit.GHITV & 0x0001) != 0) || (grid->gitvl.bit.GHITV < grid->gwidth.bit.GHWID + 2)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 9\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grid->gitvl.bit.GVITV & 0x0001) != 0) || (grid->gitvl.bit.GVITV < grid->gwidth.bit.GVWID + 2)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grid() 10\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check face frame parameter.
@param[in]	face			Face frame data parameter.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_face_frame(T_IM_DISP_FACE_FRAME_PARAM const *const face)
{
	INT32 ret = D_DDIM_OK;

	if ((face->ffdsta.bit.DSH & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_face_frame() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((face->ffdsta.bit.DSV & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_face_frame() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((face->ffsize.bit.FFHSZ & 0x0001) != 0) || (face->ffsize.bit.FFHSZ < 8)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_face_frame() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((face->ffsize.bit.FFVSZ & 0x0001) != 0) || (face->ffsize.bit.FFVSZ < 8)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_face_frame() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((face->ffwidth.bit.FFHWID & 0x0001) != 0) || (face->ffwidth.bit.FFHWID < 2)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_face_frame() 5\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((face->ffwidth.bit.FFVWID & 0x0001) != 0) || (face->ffwidth.bit.FFVWID < 2)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_face_frame() 6\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check top face frame parameter.
@param[in]	msff			FFDISPEN.MSFF: Top face frame setting register.<br>
							Value range:0~15 (face frame 0 ~ face frame 15)
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_top_face(BYTE msff)
{
	INT32 ret = D_DDIM_OK;

	if (msff >= D_IM_DISP_FACE_FRAME_COUNT) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_top_face() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check clip parameter.
@param[in]	clip_cal		clip/gain/offset data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_clip(T_IM_DISP_CLIP_CAL const *const clip_cal)
{
	INT32 ret = D_DDIM_OK;

	if (clip_cal->y_cal.yofs > 8191){
		Im_DISP_Check_Point(("CP:im_disp_param_check_clip() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((clip_cal->cb_cal.cofs < -8192) || (clip_cal->cb_cal.cofs > 8191)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_clip() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((clip_cal->cr_cal.cofs < -8192) || (clip_cal->cr_cal.cofs > 8191)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_clip() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check input data transfer of a OSD layer parameter.
@param[in]	gridt			Input data forwarding setting register.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_gridt(U_IM_DISP_GRIDT gridt)
{
	INT32 ret = D_DDIM_OK;

	if (gridt.bit.IFMT > D_IM_DISP_GRIDT_IFMT_YCC422_A8){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gridt() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check OSD data territory size parameter.
@param[in]	grtisize	Width and height of OSD data territory.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grtisize(U_IM_DISP_SIZE grtisize)
{
	INT32 ret = D_DDIM_OK;

	if (((grtisize.size.width & 0x0001) != 0) || (grtisize.size.width < 8)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grtisize() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (((grtisize.size.lines & 0x0001) != 0) || (grtisize.size.lines < 8)){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grtisize() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check display position for OSD parameter.
@param[in]	grtdsta		Display position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grtdsta(U_IM_DISP_DSTA grtdsta)
{
	INT32 ret = D_DDIM_OK;

	if ((grtdsta.bit.DSH & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grtdsta() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((grtdsta.bit.DSV & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grtdsta() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check input data's parameter.
@param[in]	ipo				Input data order.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_gripo(U_IM_DISP_GRIPO ipo)
{
	INT32 ret = D_DDIM_OK;

	if (ipo.bit.IPO1 == ipo.bit.IPO2){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gripo() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (ipo.bit.IPO1 == ipo.bit.IPO3){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gripo() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (ipo.bit.IPO1 == ipo.bit.IPO4){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gripo() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (ipo.bit.IPO2 == ipo.bit.IPO3){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gripo() 4\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (ipo.bit.IPO2 == ipo.bit.IPO4){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gripo() 5\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (ipo.bit.IPO3 == ipo.bit.IPO4){
		Im_DISP_Check_Point(("CP:im_disp_param_check_gripo() 6\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check input data size of OSD parameter.
@param[in]	grisize				Width and height of input data buffer.<br>
@param[in]	input_size_count	input_size's number.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grisize(const U_IM_DISP_SIZE grisize[], UINT32 input_size_count)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

	for(loop = input_size_count - 1; loop >= 0; loop--) {
		if (((grisize[loop].size.width & 0x0003) != 0) || (grisize[loop].size.width < 8)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_grisize() 1 (area=%d)\n", loop));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (((grisize[loop].size.lines & 0x0001) != 0) || (grisize[loop].size.lines < 8)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_grisize() 2 (area=%d)\n", loop));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}

	return ret;
}

/**
Check OSD input data address parameter.
@param[in]	address			OSD start address.<br>
							Value range:0x00000000~0xFFFFFFFF (8-bytel unit)<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grsa(ULONG address)
{
	INT32 ret = D_DDIM_OK;

	if ((address & 0x00000007) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grsa() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check horizontal global area width parameter.
@param[in]	grhga			Horizontal global area width.<br>
							Value range:8~65528 (8-bytel unit)<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grhga(ULONG grhga)
{
	INT32 ret = D_DDIM_OK;

	if (grhga == 0) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_grhga() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if (grhga > 0xFFF8) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_grhga() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((grhga & 0x00000007) != 0) {
		Im_DISP_Check_Point(("CP:im_disp_param_check_grhga() 3\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check input start position parameter.
@param[in]	position		Input start position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_grdsta(U_IM_DISP_DSTA grdsta)
{
	INT32 ret = D_DDIM_OK;

	if ((grdsta.bit.DSH & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grdsta() 1\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((grdsta.bit.DSV & 0x0001) != 0){
		Im_DISP_Check_Point(("CP:im_disp_param_check_grdsta() 2\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	return ret;
}

/**
Check control output parameter.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ctrl			Setting of main data input block.<br>
							It is possible to rewrite it while main data input block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_ctrl_main_layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_LAYER const *const ctrl)
{
	INT32 ret = D_DDIM_OK;

	if(ctrl != NULL){
		if (im_disp_param_check_lidt(block, ctrl->lidt) != D_DDIM_OK) {
			Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: LIDT parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_lisize(ctrl->lisize) != D_DDIM_OK) {
			Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: LISIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_ldsta(ctrl->ldsta) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: LDSTA parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_lfd(ctrl->y_hga, ctrl->c_hga) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: LYHGA/LCHGA parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_resize(ctrl->lrsz0, ctrl->lrsz1, ctrl->lrsz2, ctrl->lrsz3) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: LHRSZ0/LHRSZ1/LVRSZ parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_warning(&(ctrl->warning)) != D_DDIM_OK) {
			Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: LYWTH parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}

	return ret;
}

/**
Check control output parameter.
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ctrl_trg_limit		Setting of data output block.<br>
								It is not possible to rewrite it while data output block is operating.<br>
@param[in]	ctrl				Setting of data output block.<br>
								It is possible to rewrite it while data output block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_ctrl_output(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT const *const ctrl_trg_limit, T_IM_DISP_CTRL_OUTPUT const *const ctrl)
{
	INT32 ret = D_DDIM_OK;
#if 0	// skip for HDMI
	ULONG	cycle;
#endif
	INT32	loop;
	ULONG	ohsize, ovsize, mode;
	ULONG	con, ifs, veps, vens;

	im_disp_pclk_on();
	ohsize = IO_DISP.MAIN[block].DCORE.OHSIZE.word;
	ovsize = IO_DISP.MAIN[block].DCORE.OVSIZE.word;
	mode = IO_DISP.MAIN[block].DCORE.DOMD.bit.MODE;
	con  = IO_DISP.MAIN[block].DCORE.TOCTL.bit.CON;
	ifs  = IO_DISP.MAIN[block].DCORE.IFS.bit.IFS;
	veps = IO_DISP.MAIN[block].DCORE.POLSEL.bit.VEPS;
	vens = IO_DISP.MAIN[block].DCORE.VRFCTL.bit.VENS;
	im_disp_pclk_off();

	if(ctrl_trg_limit != NULL){
		if (ctrl_trg_limit->tsl == D_IM_DISP_TSL_PROGRESSIVE){
#if 0	// skip for AUO LCD
			if (ctrl_trg_limit->vcyc.bit.VCYC1 < ctrl_trg_limit->vblk.bit.VBLK1 + ctrl_trg_limit->ovsize + 1){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 1\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: VCYC1 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (ctrl_trg_limit->vcyc.bit.VCYC2 < ctrl_trg_limit->vblk.bit.VBLK2 + ctrl_trg_limit->ovsize + 1){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 2\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: VCYC2 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
#endif
		}
		else{
#if 0	// skip for HDMI
			if (ctrl_trg_limit->vcyc.bit.VCYC1 < ctrl_trg_limit->vblk.bit.VBLK1 + (ctrl_trg_limit->ovsize / 2) + 1){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 3\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: VCYC1 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (ctrl_trg_limit->vcyc.bit.VCYC2 < ctrl_trg_limit->vblk.bit.VBLK2 + (ctrl_trg_limit->ovsize / 2) + 1){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 4\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: VCYC2 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
#endif
		}

		switch(ctrl_trg_limit->domd.bit.MODE){
			case D_IM_DISP_MODE_RGB888_8BIT_2:
#if 0	// skip for HDMI
				cycle = ctrl_trg_limit->ohsize * 3;
#endif
				break;
			case D_IM_DISP_MODE_RGB888_8BIT_3:
			case D_IM_DISP_MODE_YCC422_8BIT:
#if 0	// skip for HDMI
				cycle = ctrl_trg_limit->ohsize * 2;
#endif
				break;
			case D_IM_DISP_MODE_YCC420_24BIT:
			case D_IM_DISP_MODE_YCC420_30BIT:
#if 0	// skip for HDMI
				cycle = ctrl_trg_limit->ohsize / 2;
#endif
				break;
			case D_IM_DISP_MODE_RGB888_24BIT:
			case D_IM_DISP_MODE_RGB888_30BIT:
			case D_IM_DISP_MODE_RGB888_36BIT:
			case D_IM_DISP_MODE_RGB888_8BIT_1:
			case D_IM_DISP_MODE_RGB565_16BIT:
			case D_IM_DISP_MODE_RGB666_18BIT:
			case D_IM_DISP_MODE_YCC422_16BIT:
			case D_IM_DISP_MODE_YCC444_24BIT:
			case D_IM_DISP_MODE_YCC422_20BIT:
			case D_IM_DISP_MODE_YCC422_24BIT:
			case D_IM_DISP_MODE_DEEP_YCC444_30BIT:
			case D_IM_DISP_MODE_DEEP_YCC444_36BIT:
			case D_IM_DISP_MODE_GENERAL_10BIT_1:
			case D_IM_DISP_MODE_GENERAL_10BIT_2:
#if 0	// skip for HDMI
				cycle = ctrl_trg_limit->ohsize;
#endif
				break;
			default:
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 5\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOMD parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
		}
#if 0	// skip for HDMI
		if (ctrl_trg_limit->hcyc < ctrl_trg_limit->hblk + cycle + 8){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 6\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: HCYC parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
#endif

#if 0	// skip for HDMI
		if (ctrl_trg_limit->ovpw.bit.OVPW == 0){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 8\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OVPW parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ctrl_trg_limit->hpw == 0){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 9\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: HPW parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if ((ctrl_trg_limit->vblk.bit.VBLK1 == 0) || (ctrl_trg_limit->vblk.bit.VBLK2 == 0)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 10\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: VBLK parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
#endif
		if (ctrl_trg_limit->hdly == 0){
#if 0	// skip for CASIO LCD
			if ((ctrl_trg_limit->hblk < 16)){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 11\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: HBLK parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
#endif
		}
		else{
			if ((ctrl_trg_limit->hblk < 8)){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 12\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: HBLK parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		if (ctrl_trg_limit->vdly > (ctrl_trg_limit->hcyc / 2 - 1)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 13\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: VDLY parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ctrl_trg_limit->hdly > (ctrl_trg_limit->hblk - 8)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 14\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: HDLY parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if ((ctrl_trg_limit->tsl == D_IM_DISP_TSL_INTERLACE) && ((ctrl_trg_limit->ovsize & 0x00000001) != 0)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 15\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OVSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ctrl_trg_limit->ohsize < 0x00000100){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 16\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OHSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if ((ctrl_trg_limit->domd.bit.MODE == D_IM_DISP_MODE_YCC420_24BIT) ||
			(ctrl_trg_limit->domd.bit.MODE == D_IM_DISP_MODE_YCC420_30BIT)) {
			if ((ctrl_trg_limit->ovsize & 0x00000001) != 0){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 17\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: OVSIZE parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if ((ctrl_trg_limit->ohsize & 0x00000001) != 0){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 18\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: OHSIZE parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}

		ohsize = ctrl_trg_limit->ohsize;
		ovsize = ctrl_trg_limit->ovsize;
		mode = ctrl_trg_limit->domd.bit.MODE;
		con  = ctrl_trg_limit->toctl.bit.CON;
		veps = ctrl_trg_limit->polsel.bit.VEPS;
		vens = ctrl_trg_limit->vrfctl;
	}

	if(ctrl != NULL){
		if (ctrl->clbhsize * 16 < ohsize) {
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 19\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: CLBHSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_blend(ctrl->bldctl) != D_DDIM_OK) {
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 20\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: BLDCTL parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (mode != D_IM_DISP_MODE_RGB888_24BIT){
			if (ctrl->doctl1.bit.ODO1 == D_IM_DISP_ENABLE_ON){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 21\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL1 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (ctrl->doctl1.bit.ODO2 == D_IM_DISP_ENABLE_ON){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 22\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL1 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (ctrl->doctl1.bit.ODE1 == D_IM_DISP_ENABLE_ON){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 23\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL1 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (ctrl->doctl1.bit.ODE2 == D_IM_DISP_ENABLE_ON){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 24\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL1 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		if ((mode != D_IM_DISP_MODE_YCC422_8BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_16BIT) &&
			(mode != D_IM_DISP_MODE_YCC444_24BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_20BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_24BIT) &&
			(mode != D_IM_DISP_MODE_DEEP_YCC444_30BIT) &&
			(mode != D_IM_DISP_MODE_DEEP_YCC444_36BIT) &&
			(mode != D_IM_DISP_MODE_DUAL_YCC444_12BIT) &&
			(mode != D_IM_DISP_MODE_YCC420_24BIT) &&
			(mode != D_IM_DISP_MODE_YCC420_30BIT)){
			if (ctrl->doctl2.bit.CLVS != D_IM_DISP_CLVS_NORMAL){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 25\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL2 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		if ((mode != D_IM_DISP_MODE_YCC422_8BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_16BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_20BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_24BIT) &&
			(mode != D_IM_DISP_MODE_YCC420_24BIT) &&
			(mode != D_IM_DISP_MODE_YCC420_30BIT)){
			if (ctrl->doctl2.bit.CTOH != D_IM_DISP_CTOH_LEFT){
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 26\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL2 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}

		for(loop = D_IM_DISP_FACE_FRAME_COUNT - 1; loop >= 0 ; loop--) {
			if (im_disp_param_check_face_frame(&(ctrl->face.param[loop])) != D_DDIM_OK) {
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 27\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_Output: FFDSTA/FFSIZE/FFWIDTH parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		if (im_disp_param_check_top_face(ctrl->face.msff) != D_DDIM_OK) {
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() 28\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: FFDISPEN.MSFF parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}

	if(ifs == D_IM_DISP_IFS_HDMI){
		if ((mode != D_IM_DISP_MODE_RGB888_24BIT) &&
			(mode != D_IM_DISP_MODE_RGB888_30BIT) &&
			(mode != D_IM_DISP_MODE_RGB888_36BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_16BIT) &&
			(mode != D_IM_DISP_MODE_YCC444_24BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_20BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_24BIT) &&
			(mode != D_IM_DISP_MODE_DEEP_YCC444_30BIT) &&
			(mode != D_IM_DISP_MODE_DEEP_YCC444_36BIT) &&
			(mode != D_IM_DISP_MODE_YCC420_24BIT) &&
			(mode != D_IM_DISP_MODE_YCC420_30BIT) &&
			(mode != D_IM_DISP_MODE_GENERAL_10BIT_1) &&
			(mode != D_IM_DISP_MODE_GENERAL_10BIT_2)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : HDMI 1\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: DOMD parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (veps != D_IM_DISP_POLARITY_POSITIVE){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : HDMI 2\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: POLSEL parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (vens != D_IM_DISP_VENS_H_ENABLE){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : HDMI 3\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: VRFCTL parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ovsize > 0x2000){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : HDMI 4\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OVSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ohsize > 0x1000){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : HDMI 5\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OHSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (con != D_IM_DISP_CON_INTERNAL){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : HDMI 6\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: TOCTL parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}
	else if(ifs == D_IM_DISP_IFS_LCD){
		if ((mode != D_IM_DISP_MODE_RGB888_24BIT) &&
			(mode != D_IM_DISP_MODE_RGB888_8BIT_1) &&
			(mode != D_IM_DISP_MODE_RGB888_8BIT_2) &&
			(mode != D_IM_DISP_MODE_RGB888_8BIT_3) &&
			(mode != D_IM_DISP_MODE_RGB565_16BIT) &&
			(mode != D_IM_DISP_MODE_RGB666_18BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_8BIT) &&
			(mode != D_IM_DISP_MODE_YCC422_16BIT)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : LCD 1\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: DOMD parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (con != D_IM_DISP_CON_EXTERNAL){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : LCD 2\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: TOCTL parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ovsize > 0x2000){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : LCD 3\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OVSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ohsize > 0x780){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : LCD 4\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OHSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ctrl->doctl2.bit.CTOV != D_IM_DISP_CTOV_TOP){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : LCD 5\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL2.CTOV parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}
	else if(ifs == D_IM_DISP_IFS_MIPI){
		if ((mode != D_IM_DISP_MODE_RGB888_24BIT) &&
			(mode != D_IM_DISP_MODE_RGB565_16BIT) &&
			(mode != D_IM_DISP_MODE_RGB666_18BIT)){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : MIPI 1\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: DOMD parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (con != D_IM_DISP_CON_INTERNAL){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : MIPI 2\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: TOCTL parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ovsize > 0x2000){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : MIPI 3\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OVSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ohsize > 0x780){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : MIPI 4\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: OHSIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (ctrl->doctl2.bit.CTOV != D_IM_DISP_CTOV_TOP){
			Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_output() : MIPI 5\n"));
			Ddim_Print(("E:Im_DISP_Ctrl_Output: DOCTL2.CTOV parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}
	
	return ret;
}

/**
Check control OSD data input parameter.
@param[in]	ctrl			Setting of OSD data input block.<br>
							It is possible to rewrite it while OSD data input block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
static INT32 im_disp_param_check_ctrl_osd_layer(T_IM_DISP_CTRL_OSD_LAYER const *const ctrl)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

	if(ctrl != NULL){
		if (im_disp_param_check_gridt(ctrl->gridt) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRIDT parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_grtisize(ctrl->grtisize) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRTISIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_grtdsta(ctrl->grtdsta) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRTDSTA parameter error,DSH=%x,DSV=%x\n",ctrl->grtdsta.bit.DSH,ctrl->grtdsta.bit.DSV));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_gripo(ctrl->gripo) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRIPO parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		if (im_disp_param_check_grisize(ctrl->grisize, D_IM_DISP_OSD_DISPLAY_AREA_COUNT) != D_DDIM_OK) {
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRISIZE parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
		for(loop = D_IM_DISP_OSD_DISPLAY_AREA_COUNT - 1; loop >= 0; loop--) {
			if (ctrl->grdsta[loop].bit.DSH + ctrl->grisize[loop].size.width > ctrl->grtisize.size.width) {
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_osd_layer() 1\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRISIZE parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (ctrl->grdsta[loop].bit.DSV + ctrl->grisize[loop].size.lines > ctrl->grtisize.size.lines) {
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_osd_layer() 2\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRISIZE parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (im_disp_param_check_grhga(ctrl->grhga[loop]) != D_DDIM_OK) {
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRHGA parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if (im_disp_param_check_grdsta(ctrl->grdsta[loop]) != D_DDIM_OK) {
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRDSTA parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
			if ((loop > 0) && (ctrl->grdsta[loop-1].bit.DSH > ctrl->grdsta[loop].bit.DSH)) {
				Im_DISP_Check_Point(("CP:im_disp_param_check_ctrl_osd_layer() 3\n"));
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRDSTA parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		for(loop = D_IM_DISP_OSD_ADDR_0_BANK_SIZE - 1; loop >= 0; loop--) {
			if (im_disp_param_check_grsa(ctrl->grsa0[loop]) != D_DDIM_OK){
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRSA0 parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		for(loop = D_IM_DISP_OSD_ADDR_BANK_SIZE - 1; loop >= 0; loop--) {
			if (im_disp_param_check_grsa(ctrl->grsa[loop]) != D_DDIM_OK){
				Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRSA parameter error\n"));
				return D_IM_DISP_INPUT_PARAM_ERROR;
			}
		}
		if (im_disp_param_check_resize(ctrl->grrsz0, ctrl->grrsz1, ctrl->grrsz2, ctrl->grrsz3) != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: GRRSZ0/GRRSZ1/GRRSZ2 parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}

	return ret;
}

#endif // CO_PARAM_CHECK

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/* Initialize DISP macro to clear soft-reset bit.
*/
VOID Im_DISP_Init(VOID)
{
	im_disp_iclk_on();
	im_disp_pclk_on();
	Im_DISP_Dsb();

	// Clear SR.
	// Common block 0.
	IO_DISP.MAIN[0].LCH.LRST.word			= D_IM_DISP_SR_CANCEL;	// Main data input block.
	IO_DISP.MAIN[0].DCORE.RESET.word		= D_IM_DISP_SR_CANCEL;	// Data output block.
	IO_DISP.MAIN[0].GRCH[0].GRRST.word		= D_IM_DISP_SR_CANCEL;	// OSD_0 data input block.
	IO_DISP.MAIN[0].GRCH[1].GRRST.word		= D_IM_DISP_SR_CANCEL;	// OSD_1 data input block.
	// Common block 1.
	IO_DISP.MAIN[1].LCH.LRST.word			= D_IM_DISP_SR_CANCEL;	// Main data input block.
	IO_DISP.MAIN[1].DCORE.RESET.word		= D_IM_DISP_SR_CANCEL;	// Data output block.
	IO_DISP.MAIN[1].GRCH[0].GRRST.word		= D_IM_DISP_SR_CANCEL;	// OSD_0 data input block.
	IO_DISP.MAIN[1].GRCH[1].GRRST.word		= D_IM_DISP_SR_CANCEL;	// OSD_1 data input block.

	// MIPI-DSI.
	IO_DISP.MDSCTL.MDSSR.word				= D_IM_DISP_SR_CANCEL;
	
	// HDMI.
	IO_DISP.JDDISP_HDMI_HDMIC.HDMISR.word	= D_IM_DISP_SR_CANCEL;
	
	Im_DISP_Dsb();

	im_disp_pclk_off();
	Im_DISP_Dsb();
}

/* Set control data of main data input block.
*/
INT32 Im_DISP_Ctrl_Main_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT const *const ctrl_trg_limit, T_IM_DISP_CTRL_MAIN_LAYER const *const ctrl)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;
	ULONG rpgen;

#ifdef CO_PARAM_CHECK
	if ((ctrl_trg_limit == NULL) && (ctrl == NULL)) {
		// Error when all arguments are NULL.
		Ddim_Print(("E:Im_DISP_Ctrl_Main_Layer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_ctrl_main_layer(block, ctrl) != D_DDIM_OK) {
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN off
	rpgen = IO_DISP.MAIN[block].DCORE.RPGEN.word;
	IO_DISP.MAIN[block].DCORE.RPGEN.word = D_IM_DISP_ENABLE_OFF;
	Im_DISP_Dsb();

	// Set data to this layer
	if (ctrl_trg_limit != NULL) {
		// Set core part
		if(IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT ){
			// Must stop LCH's Trigger
			// Set LRPGCTL
			IO_DISP.MAIN[block].LCH.LRPGCTL.word = ctrl_trg_limit->lrpgctl;
			
			// Set LTBLASET.IGTAEN/LTBLASET.GTAEN
			IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN = ctrl_trg_limit->ltblaset_igtaen;
			IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN = ctrl_trg_limit->ltblaset_gtaen;
		}
		else{
			// TRG is on, enable setting possible.
			if (ctrl_trg_limit->ltblaset_igtaen == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// Set LTBLASET.IGTAEN
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN = ctrl_trg_limit->ltblaset_igtaen;
			}
			if (ctrl_trg_limit->ltblaset_gtaen == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// Set LTBLASET.GTAEN
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN = ctrl_trg_limit->ltblaset_gtaen;
			}
		}
	}

	if(ctrl != NULL) {
		// Set LIDT0/LIDT1/LISIZE
		IO_DISP.MAIN[block].LCH.LIDT0.word = ctrl->lidt.word[0];
		IO_DISP.MAIN[block].LCH.LIDT1.word = ctrl->lidt.word[1];
		IO_DISP.MAIN[block].LCH.LISIZE.word = ctrl->lisize.word;
		// Set LYSA/LCSA
		for(loop = D_IM_DISP_MAIN_ADDR_BANK_SIZE - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].LCH.LYSA[loop].word = ctrl->laddr[loop].y_addr;
			IO_DISP.MAIN[block].LCH.LCSA[loop].word = ctrl->laddr[loop].c_addr;
		}
		// Set LYHGA/LCHGA
		IO_DISP.MAIN[block].LCH.LYHGA.word = ctrl->y_hga;
		IO_DISP.MAIN[block].LCH.LCHGA.word = ctrl->c_hga;
		// Set LIBCTL.IBEN/LIBCTL.IBSET
		IO_DISP.MAIN[block].LCH.LIBCTL.bit.IBEN = 0;
		IO_DISP.MAIN[block].LCH.LIBCTL.bit.IBSET = ctrl->libctl_ibset;
		
		// Set LRSZ0/LRSZ1/LRSZ2/LRSZ3
		IO_DISP.MAIN[block].LCH.LRSZ0.word = ctrl->lrsz0;
		IO_DISP.MAIN[block].LCH.LRSZ1.word = ctrl->lrsz1.word;
		IO_DISP.MAIN[block].LCH.LRSZ2.word = ctrl->lrsz2.word;
		IO_DISP.MAIN[block].LCH.LRSZ3.word = ctrl->lrsz3.word;
		
		// Set LCEPMD
		// 3DA register unmatch, chekc V1.0
		// IO_DISP.MAIN[block].LCH.LCEPMD.word = ctrl->lcepmd;
		
		// Set LYHLPCTL/LYHLPK0/LYHLPK1/LYHLPOL/LYHLPCLR
		IO_DISP.MAIN[block].LCH.LYHLPCTL.word = ctrl->yhlp.lyhlpcl.word;
		IO_DISP.MAIN[block].LCH.LYHLPK0.word = ctrl->yhlp.lyhlpk.word[0];
		IO_DISP.MAIN[block].LCH.LYHLPK1.word = ctrl->yhlp.lyhlpk.word[1];
		IO_DISP.MAIN[block].LCH.LYHLPOL.word = ctrl->yhlp.lyhlpol.word;
		IO_DISP.MAIN[block].LCH.LYHLPCLR.dword = ctrl->yhlp.lyhlpclr.dword;
		
		// Set LYWCTL/LYW0TH/LYW1TH/LYW0ST/LYW0CA/LYW0CB/LYW1ST/LYW1CA/LYW1CB/LBOST
		IO_DISP.MAIN[block].LCH.LYWCTL.word = ctrl->warning.lywctl.word;
		IO_DISP.MAIN[block].LCH.LYW0TH.word = ctrl->warning.lyw0th.word;
		IO_DISP.MAIN[block].LCH.LYW1TH.word = ctrl->warning.lyw1th.word;
		IO_DISP.MAIN[block].LCH.LYW0ST.word = ctrl->warning.lyw0st;
		IO_DISP.MAIN[block].LCH.LYW0CA.dword = ctrl->warning.lyw0ca.dword;
		IO_DISP.MAIN[block].LCH.LYW0CB.dword = ctrl->warning.lyw0cb.dword;
		IO_DISP.MAIN[block].LCH.LYW1ST.word = ctrl->warning.lyw1st;
		IO_DISP.MAIN[block].LCH.LYW1CA.dword = ctrl->warning.lyw1ca.dword;
		IO_DISP.MAIN[block].LCH.LYW1CB.dword = ctrl->warning.lyw1cb.dword;
		IO_DISP.MAIN[block].LCH.LBOST.word = ctrl->warning.lbost;
		// Set LBLTMR
		IO_DISP.MAIN[block].LCH.LBLTMR.word = ctrl->lbltmr.word;
		
		// Set LZBWID/LZBV/LZBPT
		IO_DISP.MAIN[block].LCH.LZBWID.word = ctrl->zebra.lzbwid.word;
		IO_DISP.MAIN[block].LCH.LZBV.word = ctrl->zebra.lzbv.word;
		IO_DISP.MAIN[block].LCH.LZBPT.word = ctrl->zebra.lzbpt.word;
		
		// Set LY2R0/LY2R1/LY2R2/LCC0/LCC1/LCC2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].LCH.LY2R[loop].dword = ctrl->ly2r[loop].dword;
			IO_DISP.MAIN[block].LCH.LCC[loop].dword = ctrl->lcc[loop].dword;
		}
		
		// Set LTBLASET.IGEN
		if (ctrl->ltblaset_igen) {
			// check anti-gamma table access enable.
			if (IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// setting enable.
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGEN = E_IM_DISP_CORRECT_PROC_ENABLE;
			}
			else {
				// not setting enable.
				Ddim_Print(("W:Im_DISP_Ctrl_Main_Layer: not setting anti-gamma correction enable\n"));
			}
		}
		else {
			// settign disable.
			IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGEN = E_IM_DISP_CORRECT_PROC_DISABLE;
		}
		// Set LTBLASET.GMEN
		if (ctrl->ltblaset_gmen) {
			// check gamma table access enable.
			if (IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// setting enable.
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_ENABLE;
			}
			else {
				// not setting enable.
				Ddim_Print(("W:Im_DISP_Ctrl_Main_Layer: not setting gamma correction enable\n"));
			}
		}
		else {
			// setting disable.
			IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_DISABLE;
		}
		// Set LTBLASET.IGTSL/LTBLASET.GMTSL
		IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL = ctrl->ltblaset_igtsl;
		IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL = ctrl->ltblaset_gmtsl;

		// Set LGMMD/LALP
		IO_DISP.MAIN[block].LCH.LGMMD.word = ctrl->lgmmd.word;
		IO_DISP.MAIN[block].LCH.LALP.word = ctrl->lalp.word;

		// Set LDSTA/LREVDISP
		IO_DISP.MAIN[block].LCH.LDSTA.word = ctrl->ldsta.word;
		IO_DISP.MAIN[block].LCH.LREVDISP.word = ctrl->lrevdisp.word;
	}

	// Recover RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.word = rpgen;
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* Get control data of main data input block.
*/
INT32 Im_DISP_Get_Ctrl_Main_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT *const ctrl_trg_limit, T_IM_DISP_CTRL_MAIN_LAYER *const ctrl)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ctrl_trg_limit == NULL) && (ctrl == NULL)) {
		// Error when all arguments are NULL.
		Ddim_Print(("E:Im_DISP_Get_Ctrl_Main_Layer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if(ctrl_trg_limit != NULL) {
		// Get core part
		// Get LRPGCTL
		ctrl_trg_limit->lrpgctl = (E_IM_DISP_RPGTMG)IO_DISP.MAIN[block].LCH.LRPGCTL.word;
		// Get LTBLASET.IGTAEN/LTBLASET.GTAEN
		ctrl_trg_limit->ltblaset_igtaen = (E_IM_DISP_TABLE_ACCESS)IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN;
		ctrl_trg_limit->ltblaset_gtaen = (E_IM_DISP_TABLE_ACCESS)IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN;
	}

	if(ctrl != NULL) {
		// Get main part
		ctrl->lidt.word[0] = IO_DISP.MAIN[block].LCH.LIDT0.word;
		ctrl->lidt.word[1] = IO_DISP.MAIN[block].LCH.LIDT1.word;
		ctrl->lisize.word = IO_DISP.MAIN[block].LCH.LISIZE.word;
		// Get LYSA/LCSA
		for(loop = D_IM_DISP_MAIN_ADDR_BANK_SIZE - 1; loop >= 0; loop--) {
			ctrl->laddr[loop].y_addr = IO_DISP.MAIN[block].LCH.LYSA[loop].word;
			ctrl->laddr[loop].c_addr = IO_DISP.MAIN[block].LCH.LCSA[loop].word;
		}
		// Get LYHGA/LCHGA
		ctrl->y_hga = IO_DISP.MAIN[block].LCH.LYHGA.word;
		ctrl->c_hga = IO_DISP.MAIN[block].LCH.LCHGA.word;
		// Get LIBCTL.IBSET
		ctrl->libctl_ibset = (E_IM_DISP_BANK)IO_DISP.MAIN[block].LCH.LIBCTL.bit.IBSET;
		
		// Get LRSZ0/LRSZ1/LRSZ2/LRSZ3
		ctrl->lrsz0 = (E_IM_DISP_RSZSL)IO_DISP.MAIN[block].LCH.LRSZ0.bit.RSZSL;
		ctrl->lrsz1.word = IO_DISP.MAIN[block].LCH.LRSZ1.word;
		ctrl->lrsz2.word = IO_DISP.MAIN[block].LCH.LRSZ2.word;
		ctrl->lrsz3.word = IO_DISP.MAIN[block].LCH.LRSZ3.word;
		// Get LYHLPCTL/LYHLPK0/LYHLPK1/LYHLPOL/LYHLPCLR
		ctrl->yhlp.lyhlpcl.word = IO_DISP.MAIN[block].LCH.LYHLPCTL.word;
		ctrl->yhlp.lyhlpk.word[0] = IO_DISP.MAIN[block].LCH.LYHLPK0.word;
		ctrl->yhlp.lyhlpk.word[1] = IO_DISP.MAIN[block].LCH.LYHLPK1.word;
		ctrl->yhlp.lyhlpol.word = IO_DISP.MAIN[block].LCH.LYHLPOL.word;
		ctrl->yhlp.lyhlpclr.dword = IO_DISP.MAIN[block].LCH.LYHLPCLR.dword;

		// Get LYWCTL/LYW0TH/LYW1TH/LYW0ST/LYW0CA/LYW0CB/LYW1ST/LYW1CA/LYW1CB/LBLTMR/LBOST
		ctrl->warning.lywctl.word = IO_DISP.MAIN[block].LCH.LYWCTL.word;
		ctrl->warning.lyw0th.word = IO_DISP.MAIN[block].LCH.LYW0TH.word;
		ctrl->warning.lyw1th.word = IO_DISP.MAIN[block].LCH.LYW1TH.word;
		ctrl->warning.lyw0st = IO_DISP.MAIN[block].LCH.LYW0ST.word;
		ctrl->warning.lyw0ca.dword = IO_DISP.MAIN[block].LCH.LYW0CA.dword;
		ctrl->warning.lyw0cb.dword = IO_DISP.MAIN[block].LCH.LYW0CB.dword;
		ctrl->warning.lyw1st = IO_DISP.MAIN[block].LCH.LYW1ST.word;
		ctrl->warning.lyw1ca.dword = IO_DISP.MAIN[block].LCH.LYW1CA.dword;
		ctrl->warning.lyw1cb.dword = IO_DISP.MAIN[block].LCH.LYW1CB.dword;
		ctrl->warning.lbost = IO_DISP.MAIN[block].LCH.LBOST.word;
		ctrl->lbltmr.word = IO_DISP.MAIN[block].LCH.LBLTMR.word;

		// Get LZBWID/LZBV/LZBPT
		ctrl->zebra.lzbwid.word = IO_DISP.MAIN[block].LCH.LZBWID.word;
		ctrl->zebra.lzbv.word = IO_DISP.MAIN[block].LCH.LZBV.word;
		ctrl->zebra.lzbpt.word = IO_DISP.MAIN[block].LCH.LZBPT.word;
		
		// Get LY2R0/LY2R1/LY2R2/LCC0/LCC1/LCC2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			ctrl->ly2r[loop].dword = IO_DISP.MAIN[block].LCH.LY2R[loop].dword;
			ctrl->lcc[loop].dword = IO_DISP.MAIN[block].LCH.LCC[loop].dword;
		}
		// Get LTBLASET.IGEN/LTBLASET.IGTSL/LTBLASET.GMEN/LTBLASET.GMTSL
		ctrl->ltblaset_igen = (E_IM_DISP_CORRECT_PROC)IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGEN;
		ctrl->ltblaset_igtsl = IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL;
		ctrl->ltblaset_gmen = (E_IM_DISP_CORRECT_PROC)IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMEN;
		ctrl->ltblaset_gmtsl = IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL;
		// Get LGMMD/LALP
		ctrl->lgmmd.word = IO_DISP.MAIN[block].LCH.LGMMD.word;
		ctrl->lalp.word = IO_DISP.MAIN[block].LCH.LALP.word;
		
		// Get LDSTA/LREVDISP
		ctrl->ldsta.word = IO_DISP.MAIN[block].LCH.LDSTA.word;
		ctrl->lrevdisp.word = IO_DISP.MAIN[block].LCH.LREVDISP.word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Set control data of data output block.
*/
INT32 Im_DISP_Ctrl_Output(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT const *const ctrl_trg_limit, T_IM_DISP_CTRL_OUTPUT const *const ctrl)
{
	INT32 loop;
	ULONG rpgen;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ctrl_trg_limit == NULL) && (ctrl == NULL)) {
		// Error when all arguments are NULL.
		Ddim_Print(("E:Im_DISP_Ctrl_Output: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_ctrl_output(block, ctrl_trg_limit, ctrl) != D_DDIM_OK){
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN off
	rpgen = IO_DISP.MAIN[block].DCORE.RPGEN.word;
	IO_DISP.MAIN[block].DCORE.RPGEN.word = D_IM_DISP_ENABLE_OFF;
	Im_DISP_Dsb();

	if(ctrl_trg_limit != NULL) {
		// Set core part
		if(IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT ){
			// Must stop DCORE's TRiger
			// Set TRG.TGKMD/TGKST/TOCTL
			IO_DISP.MAIN[block].DCORE.TRG.bit.TGKMD = ctrl_trg_limit->tgkmd;
			IO_DISP.MAIN[block].DCORE.TGKST.word = ctrl_trg_limit->tgkst;
			IO_DISP.MAIN[block].DCORE.TOCTL.word = ctrl_trg_limit->toctl.word;
			// Set RPGCTL/POLSEL/TSL
			IO_DISP.MAIN[block].DCORE.RPGCTL.bit.RPGTMG = ctrl_trg_limit->rpgctl;
			IO_DISP.MAIN[block].DCORE.POLSEL.word = ctrl_trg_limit->polsel.word;
			IO_DISP.MAIN[block].DCORE.TSL.bit.TSL = ctrl_trg_limit->tsl;
			// Set VCYC/HCYC/VPW/HPW/VBLK/HBLK/VDLY/HDLY/OVSIZE/OHSIZE
			IO_DISP.MAIN[block].DCORE.VCYC.word = ctrl_trg_limit->vcyc.word;
			IO_DISP.MAIN[block].DCORE.HCYC.word = ctrl_trg_limit->hcyc;
			IO_DISP.MAIN[block].DCORE.OVPW.word = ctrl_trg_limit->ovpw.word;
			IO_DISP.MAIN[block].DCORE.HPW.word = ctrl_trg_limit->hpw;
			IO_DISP.MAIN[block].DCORE.VBLK.word = ctrl_trg_limit->vblk.word;
			IO_DISP.MAIN[block].DCORE.HBLK.word = ctrl_trg_limit->hblk;
			IO_DISP.MAIN[block].DCORE.VDLY.word = ctrl_trg_limit->vdly;
			IO_DISP.MAIN[block].DCORE.HDLY.word = ctrl_trg_limit->hdly;
			IO_DISP.MAIN[block].DCORE.OVSIZE.word = ctrl_trg_limit->ovsize;
			IO_DISP.MAIN[block].DCORE.OHSIZE.word = ctrl_trg_limit->ohsize;
			// Set VRFCTL/HRFCTL
			IO_DISP.MAIN[block].DCORE.VRFCTL.bit.VENS = ctrl_trg_limit->vrfctl;
			IO_DISP.MAIN[block].DCORE.HRFCTL.bit.HENS = ctrl_trg_limit->hrfctl;

			// Set DOMD
			IO_DISP.MAIN[block].DCORE.DOMD.word = ctrl_trg_limit->domd.word;
			// Set TBLASET.GMTAEN/TBLASET.YSATTA
			IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN = ctrl_trg_limit->tblaset_gmtaen;
			IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA = ctrl_trg_limit->tblaset_ysatta;
		}
		else{
			// TRG is on, enable setting possible.
			if (ctrl_trg_limit->tblaset_gmtaen == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// Set TBLASET.GMTAEN
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN = ctrl_trg_limit->tblaset_gmtaen;
			}
			if (ctrl_trg_limit->tblaset_ysatta == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// Set TBLASET.YSATTA
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA = ctrl_trg_limit->tblaset_ysatta;
			}
		}
	}
	
	if(ctrl != NULL) {
		// Set main part
		// Set INTC/INTE
		IO_DISP.MAIN[block].DCORE.INTC.word = ctrl->intc;
		IO_DISP.MAIN[block].DCORE.INTE.word = ctrl->inte.word;
		
		// Set HABLK
		IO_DISP.MAIN[block].DCORE.HABLK.bit.HABLK = ctrl->hablk;
		
		// Set FDOEN/FODATA/BLANKDT/CLBHSIZE
		IO_DISP.MAIN[block].DCORE.FDOEN.bit.FDEN = ctrl->fdoen;
		IO_DISP.MAIN[block].DCORE.FODATA.word = ctrl->fodata.word;
		IO_DISP.MAIN[block].DCORE.BLANKDT.BLANKDT1.word = ctrl->blankdt.word[0];
		IO_DISP.MAIN[block].DCORE.BLANKDT.BLANKDT2.word = ctrl->blankdt.word[1];
		IO_DISP.MAIN[block].DCORE.CLBHSIZE.word = ctrl->clbhsize;
		// Set CLBDT
		for(loop = D_IM_DISP_COLOR_BAR_COUNT - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].DCORE.CLBDT[loop].word = ctrl->clbdt[loop].word;
		}
		// Set BLDCTL
		IO_DISP.MAIN[block].DCORE.BLDCTL.word = ctrl->bldctl.word;
		// Set R2Y0/1/2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0 ; loop--) {
			IO_DISP.MAIN[block].DCORE.R2Y[loop].dword = ctrl->r2y[loop].dword;
		}
		// Set YCAL/YCLIP/CBCAL/CBCLIP/CRCAL/CRCLIP
		IO_DISP.MAIN[block].DCORE.YCAL.bit.YGAIN = ctrl->clip_cal.y_cal.ygain;
		IO_DISP.MAIN[block].DCORE.YCAL.bit.YOFS = ctrl->clip_cal.y_cal.yofs;
		IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPH = ctrl->clip_cal.y_clip.cph;
		IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPL = ctrl->clip_cal.y_clip.cpl;
		IO_DISP.MAIN[block].DCORE.CBCAL.bit.CBGAIN = ctrl->clip_cal.cb_cal.cgain;
		im_disp_set_reg_signed(IO_DISP.MAIN[block].DCORE.CBCAL, union io_jdsdcore_reg_cbcal, CBOFS, ctrl->clip_cal.cb_cal.cofs);
		IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPH = ctrl->clip_cal.cb_clip.cph;
		IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPL = ctrl->clip_cal.cb_clip.cpl;
		IO_DISP.MAIN[block].DCORE.CRCAL.bit.CRGAIN = ctrl->clip_cal.cr_cal.cgain;
		im_disp_set_reg_signed(IO_DISP.MAIN[block].DCORE.CRCAL, union io_jdsdcore_reg_crcal, CROFS, ctrl->clip_cal.cr_cal.cofs);
		IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPH = ctrl->clip_cal.cr_clip.cph;
		IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPL = ctrl->clip_cal.cr_clip.cpl;
		// Set DOCTL0~DOCTL2
		IO_DISP.MAIN[block].DCORE.DOCTL0.word = ctrl->doctl0.word;
		IO_DISP.MAIN[block].DCORE.DOCTL1.word = ctrl->doctl1.word;
		IO_DISP.MAIN[block].DCORE.DOCTL2.word = ctrl->doctl2.word;
		// Set TRSCODE0/TRSCODE1
		IO_DISP.MAIN[block].DCORE.TRSCODE0.word = ctrl->trscode[0].word;
		IO_DISP.MAIN[block].DCORE.TRSCODE1.word = ctrl->trscode[1].word;
		// Set Y2R0/1/2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0 ; loop--) {
			IO_DISP.MAIN[block].DCORE.Y2R[loop].dword = ctrl->y2r[loop].dword;
		}
		// Set TBLASET.GMEN
		if (ctrl->tblaset_gmen) {
			// check gamma table access enable.
			if (IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// setting enable.
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_ENABLE;
			}
			else {
				// not setting enable.
				Ddim_Print(("W:Im_DISP_Ctrl_Output: not setting gamma correction enable\n"));
			}
		}
		else {
			// settign disable.
			IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_DISABLE;
		}
		// Set TBLASET.YSATEN
		if (ctrl->tblaset_ysaten) {
			// check luminance and chroma table access enable.
			if (IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA == E_IM_DISP_TABLE_ACCESS_ENABLE) {
				// setting enable.
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATEN = E_IM_DISP_CORRECT_PROC_ENABLE;
			}
			else {
				// not setting enable.
				Ddim_Print(("W:Im_DISP_Ctrl_Output: not setting luminance and chroma correction enable\n"));
			}
		}
		else {
			// setting disable.
			IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATEN = E_IM_DISP_CORRECT_PROC_DISABLE;
		}
		// Set TBLASET.GMTSL/TBLASET.YSATSL
		IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL = ctrl->tblaset_gmtsl;
		IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL = ctrl->tblaset_ysatsl;

		// Set FFDSTA/FFSIZE/FFWIDTH/FFCLR
		for(loop = D_IM_DISP_FACE_FRAME_COUNT - 1; loop >= 0 ; loop--) {
			IO_DISP.MAIN[block].DCORE.FFDSTA[loop].word = ctrl->face.param[loop].ffdsta.word;
			IO_DISP.MAIN[block].DCORE.FFSIZE[loop].word = ctrl->face.param[loop].ffsize.word;
			IO_DISP.MAIN[block].DCORE.FFWIDTH[loop].word = ctrl->face.param[loop].ffwidth.word;
			IO_DISP.MAIN[block].DCORE.FFCLR[loop].word = ctrl->face.param[loop].ffclr.word;
		}
		// Set FFDISPEN
		IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN = ctrl->face.ffden;
		// Set FFDO
		IO_DISP.MAIN[block].DCORE.FFDO.bit.MSFF = ctrl->face.msff;

		// Set Call back function at interrupt
		memcpy((void*)gIM_DISP_Interrupt_Callback[block], ctrl->int_callback, sizeof(ctrl->int_callback));
	}

	// Recover RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.word = rpgen;
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}


/* Get control data of data output block.
*/
INT32 Im_DISP_Get_Ctrl_Output(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT *const ctrl_trg_limit, T_IM_DISP_CTRL_OUTPUT *const ctrl)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ctrl_trg_limit == NULL) && (ctrl == NULL)) {
		// Error when all arguments are NULL.
		Ddim_Print(("E:Im_DISP_Get_Ctrl_Output: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if(ctrl_trg_limit != NULL) {
		// Get core part
		// Get TRG.TGKMD/TGKST/TOCTL
		ctrl_trg_limit->tgkmd = (E_IM_DISP_TGKMD)IO_DISP.MAIN[block].DCORE.TRG.bit.TGKMD;
		ctrl_trg_limit->tgkst = IO_DISP.MAIN[block].DCORE.TGKST.word;
		ctrl_trg_limit->toctl.word = IO_DISP.MAIN[block].DCORE.TOCTL.word;
		// Get RPGCTL/POLSEL/TSL
		ctrl_trg_limit->rpgctl = (E_IM_DISP_RPGTMG)IO_DISP.MAIN[block].DCORE.RPGCTL.bit.RPGTMG;
		ctrl_trg_limit->polsel.word = IO_DISP.MAIN[block].DCORE.POLSEL.word;
		ctrl_trg_limit->tsl = IO_DISP.MAIN[block].DCORE.TSL.bit.TSL;
		// Get VCYC/HCYC/VPW/HPW/VBLK/HBLK/VDLY/HDLY/OVSIZE/OHSIZE
		ctrl_trg_limit->vcyc.word = IO_DISP.MAIN[block].DCORE.VCYC.word;
		ctrl_trg_limit->hcyc = IO_DISP.MAIN[block].DCORE.HCYC.word;
		ctrl_trg_limit->ovpw.word = IO_DISP.MAIN[block].DCORE.OVPW.word;
		ctrl_trg_limit->hpw = IO_DISP.MAIN[block].DCORE.HPW.word;
		ctrl_trg_limit->vblk.word = IO_DISP.MAIN[block].DCORE.VBLK.word;
		ctrl_trg_limit->hblk = IO_DISP.MAIN[block].DCORE.HBLK.word;
		ctrl_trg_limit->vdly = IO_DISP.MAIN[block].DCORE.VDLY.word;
		ctrl_trg_limit->hdly = IO_DISP.MAIN[block].DCORE.HDLY.word;
		ctrl_trg_limit->ovsize = IO_DISP.MAIN[block].DCORE.OVSIZE.word;
		ctrl_trg_limit->ohsize = IO_DISP.MAIN[block].DCORE.OHSIZE.word;
		// Get VRFCTL/HRFCTL
		ctrl_trg_limit->vrfctl = IO_DISP.MAIN[block].DCORE.VRFCTL.bit.VENS;
		ctrl_trg_limit->hrfctl = IO_DISP.MAIN[block].DCORE.HRFCTL.bit.HENS;
		// Get DOMD
		ctrl_trg_limit->domd.word = IO_DISP.MAIN[block].DCORE.DOMD.word;
		// Get TBLASET.GMTAEN/TBLASET.YSATTA
		ctrl_trg_limit->tblaset_gmtaen = IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN;
		ctrl_trg_limit->tblaset_ysatta = IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA;
	}
	
	if(ctrl != NULL) {
		// Get main part
		// Get INTC/INTE
		ctrl->intc = IO_DISP.MAIN[block].DCORE.INTC.word;
		ctrl->inte.word = IO_DISP.MAIN[block].DCORE.INTE.word;
		
		// Get HABLK
		ctrl->hablk = IO_DISP.MAIN[block].DCORE.HABLK.bit.HABLK;
		
		// Get FDOEN/FODATA/BLANKDT/CLBHSIZE
		ctrl->fdoen = IO_DISP.MAIN[block].DCORE.FDOEN.bit.FDEN;
		ctrl->fodata.word = IO_DISP.MAIN[block].DCORE.FODATA.word;
		ctrl->blankdt.word[0] = IO_DISP.MAIN[block].DCORE.BLANKDT.BLANKDT1.word;
		ctrl->blankdt.word[1] = IO_DISP.MAIN[block].DCORE.BLANKDT.BLANKDT2.word;
		ctrl->clbhsize = IO_DISP.MAIN[block].DCORE.CLBHSIZE.word;
		// Get CLBDT
		for(loop = D_IM_DISP_COLOR_BAR_COUNT - 1; loop >= 0; loop--) {
			ctrl->clbdt[loop].word = IO_DISP.MAIN[block].DCORE.CLBDT[loop].word;
		}
		// Get BLDCTL
		ctrl->bldctl.word = IO_DISP.MAIN[block].DCORE.BLDCTL.word;
		// Get R2Y0/1/2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0 ; loop--) {
			ctrl->r2y[loop].dword = IO_DISP.MAIN[block].DCORE.R2Y[loop].dword;
		}
		// Get YCAL/YCLIP/CBCAL/CBCLIP/CRCAL/CRCLIP
		ctrl->clip_cal.y_cal.ygain = IO_DISP.MAIN[block].DCORE.YCAL.bit.YGAIN;
		ctrl->clip_cal.y_cal.yofs = IO_DISP.MAIN[block].DCORE.YCAL.bit.YOFS;
		ctrl->clip_cal.y_clip.cph = IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPH;
		ctrl->clip_cal.y_clip.cpl = IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPL;
		ctrl->clip_cal.cb_cal.cgain = IO_DISP.MAIN[block].DCORE.CBCAL.bit.CBGAIN;
		im_disp_get_reg_signed(ctrl->clip_cal.cb_cal.cofs, IO_DISP.MAIN[block].DCORE.CBCAL, union io_jdsdcore_reg_cbcal, CBOFS);
		ctrl->clip_cal.cb_clip.cph = IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPH;
		ctrl->clip_cal.cb_clip.cpl = IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPL;
		ctrl->clip_cal.cr_cal.cgain = IO_DISP.MAIN[block].DCORE.CRCAL.bit.CRGAIN;
		im_disp_get_reg_signed(ctrl->clip_cal.cr_cal.cofs, IO_DISP.MAIN[block].DCORE.CRCAL, union io_jdsdcore_reg_crcal, CROFS);
		ctrl->clip_cal.cr_clip.cph = IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPH;
		ctrl->clip_cal.cr_clip.cpl = IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPL;
		// Get DOCTL0~DOCTL2
		ctrl->doctl0.word = IO_DISP.MAIN[block].DCORE.DOCTL0.word;
		ctrl->doctl1.word = IO_DISP.MAIN[block].DCORE.DOCTL1.word;
		ctrl->doctl2.word = IO_DISP.MAIN[block].DCORE.DOCTL2.word;
		// Get TRSCODE0/TRSCODE1
		ctrl->trscode[0].word = IO_DISP.MAIN[block].DCORE.TRSCODE0.word;
		ctrl->trscode[1].word = IO_DISP.MAIN[block].DCORE.TRSCODE1.word;
		// Get Y2R0/1/2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0 ; loop--) {
			ctrl->y2r[loop].dword = IO_DISP.MAIN[block].DCORE.Y2R[loop].dword;
		}
		// Get TBLASET.GMEN/TBLASET.GMTSL/TBLASET.YSATEN/TBLASET.YSATSL
		ctrl->tblaset_gmen = IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMEN;
		ctrl->tblaset_gmtsl = IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL;
		ctrl->tblaset_ysaten = IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATEN;
		ctrl->tblaset_ysatsl = IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL;
		
		// Get FFDSTA/FFSIZE/FFWIDTH/FFCLR
		for(loop = D_IM_DISP_FACE_FRAME_COUNT - 1; loop >= 0 ; loop--) {
			ctrl->face.param[loop].ffdsta.word = IO_DISP.MAIN[block].DCORE.FFDSTA[loop].word;
			ctrl->face.param[loop].ffsize.word = IO_DISP.MAIN[block].DCORE.FFSIZE[loop].word;
			ctrl->face.param[loop].ffwidth.word = IO_DISP.MAIN[block].DCORE.FFWIDTH[loop].word;
			ctrl->face.param[loop].ffclr.word = IO_DISP.MAIN[block].DCORE.FFCLR[loop].word;
		}
		// Get FFDISPEN
		ctrl->face.ffden = IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN;
		// Get FFDO
		ctrl->face.msff = IO_DISP.MAIN[block].DCORE.FFDO.bit.MSFF;
		
		// Get Call back function at interrupt
		memcpy(ctrl->int_callback, (void*)gIM_DISP_Interrupt_Callback[block], sizeof(ctrl->int_callback));
	}
	im_disp_pclk_off();

	return ret;
}

/* Set control data of Grid in common block.
*/
INT32 Im_DISP_Ctrl_Grid_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_GRID_LAYER const *const ctrl)
{
	INT32 ret = D_DDIM_OK;
	ULONG rpgen;

#ifdef CO_PARAM_CHECK
	if (ctrl == NULL) {
		Ddim_Print(("E:Im_DISP_Ctrl_Grid_Layer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if(im_disp_param_check_grid(&(ctrl->grid)) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Ctrl_Grid_Layer: GHDSTA/GVDSTA/GLENGTH/GWIDTH/GITVL parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN off
	rpgen = IO_DISP.MAIN[block].DCORE.RPGEN.word;
	IO_DISP.MAIN[block].DCORE.RPGEN.word = D_IM_DISP_ENABLE_OFF;
	Im_DISP_Dsb();

	// Set GHDSTA/GVDSTA/GLENGTH/GWIDTH/GITVL/GNUM/GDCTL
	IO_DISP.MAIN[block].DCORE.GHDSTA.word = ctrl->grid.ghdsta.word;
	IO_DISP.MAIN[block].DCORE.GVDSTA.word = ctrl->grid.gvdsta.word;
	IO_DISP.MAIN[block].DCORE.GLENGTH.word = ctrl->grid.glength.word;
	IO_DISP.MAIN[block].DCORE.GWIDTH.word = ctrl->grid.gwidth.word;
	IO_DISP.MAIN[block].DCORE.GITVL.word = ctrl->grid.gitvl.word;
	IO_DISP.MAIN[block].DCORE.GNUM.word = ctrl->grid.gnum.word;
	IO_DISP.MAIN[block].DCORE.GDCTL.word = ctrl->grid.gdctl.word;

	// Set GDISPEN
	IO_DISP.MAIN[block].DCORE.GDISPEN.word = ctrl->gdispen;

	// Recover RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.word = rpgen;
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* Get control data of Grid in common block.
*/
INT32 Im_DISP_Get_Ctrl_Grid_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_GRID_LAYER *const ctrl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (ctrl == NULL) {
		Ddim_Print(("E:Im_DISP_Ctrl_Grid_Layer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get GHDSTA/GVDSTA/GLENGTH/GWIDTH/GITVL/GNUM/GDCTL
	ctrl->grid.ghdsta.word = IO_DISP.MAIN[block].DCORE.GHDSTA.word;
	ctrl->grid.gvdsta.word = IO_DISP.MAIN[block].DCORE.GVDSTA.word;
	ctrl->grid.glength.word = IO_DISP.MAIN[block].DCORE.GLENGTH.word;
	ctrl->grid.gwidth.word = IO_DISP.MAIN[block].DCORE.GWIDTH.word;
	ctrl->grid.gitvl.word = IO_DISP.MAIN[block].DCORE.GITVL.word;
	ctrl->grid.gnum.word = IO_DISP.MAIN[block].DCORE.GNUM.word;
	ctrl->grid.gdctl.word = IO_DISP.MAIN[block].DCORE.GDCTL.word;

	// Get GDISPEN
	ctrl->gdispen = (E_IM_DISP_GDISPEN)IO_DISP.MAIN[block].DCORE.GDISPEN.bit.GDEN;
	im_disp_pclk_off();

	return ret;
}

/* Set control data of OSD data input block.
*/
INT32 Im_DISP_Ctrl_OSD_Layer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG* grrpgctl, T_IM_DISP_CTRL_OSD_LAYER const *const ctrl)
{
	INT32 layer_index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;
	ULONG rpgen;

#ifdef CO_PARAM_CHECK
	if ((grrpgctl == NULL) && (ctrl == NULL)) {
		// Error when all arguments are NULL.
		Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_ctrl_osd_layer(ctrl) != D_DDIM_OK){
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN off
	rpgen = IO_DISP.MAIN[block].DCORE.RPGEN.word;
	IO_DISP.MAIN[block].DCORE.RPGEN.word = D_IM_DISP_ENABLE_OFF;
	Im_DISP_Dsb();

	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	if(grrpgctl != NULL){
		if(IO_DISP.MAIN[block].GRCH[layer_index].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT){
			// Must stop GRCH's TRiger
			// Set GRRPGCTL
			IO_DISP.MAIN[block].GRCH[layer_index].GRRPGCTL.word = *grrpgctl;
		}
		else{
			Ddim_Print(("E:Im_DISP_Ctrl_OSD_Layer: OSD-%d layer busy\n", layer_index));
			ret = D_IM_DISP_MACRO_BUSY_NG;
		}
	}

	if(ctrl != NULL){
		// Set GRIDT/GRTISIZE/GRTDSTA/GRIPO/GRSCCTL
		IO_DISP.MAIN[block].GRCH[layer_index].GRIDT.word = ctrl->gridt.word;
		IO_DISP.MAIN[block].GRCH[layer_index].GRTISIZE.word = ctrl->grtisize.word;
		IO_DISP.MAIN[block].GRCH[layer_index].GRTDSTA.word = ctrl->grtdsta.word;
		IO_DISP.MAIN[block].GRCH[layer_index].GRIPO.word = ctrl->gripo.word;
		IO_DISP.MAIN[block].GRCH[layer_index].GRSCCTL.bit.SCEN = 0;
		IO_DISP.MAIN[block].GRCH[layer_index].GRSCCTL.bit.IDSET = ctrl->grscctl_idset;

		// Set GRISIZE/GRASA/GRHGA/GRAHGA/GRDSTA
		for(loop = D_IM_DISP_OSD_DISPLAY_AREA_COUNT - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].GRCH[layer_index].GRISIZE[loop].word = ctrl->grisize[loop].word;
			IO_DISP.MAIN[block].GRCH[layer_index].GRASA[loop].word = ctrl->grasa[loop];
			IO_DISP.MAIN[block].GRCH[layer_index].GRHGA[loop].word = ctrl->grhga[loop];
			IO_DISP.MAIN[block].GRCH[layer_index].GRAHGA[loop].word = ctrl->grahga[loop];
			IO_DISP.MAIN[block].GRCH[layer_index].GRDSTA[loop].word = ctrl->grdsta[loop].word;
		}
		// Set GRSA0
		for(loop = D_IM_DISP_OSD_ADDR_0_BANK_SIZE - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].GRCH[layer_index].GRSA0[loop].word = ctrl->grsa0[loop];
		}
		// Set GRSA
		for(loop = D_IM_DISP_OSD_ADDR_BANK_SIZE - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].GRCH[layer_index].GRSA[loop].word = ctrl->grsa[loop];
		}
		// Set GRAREN/GRBSL
		IO_DISP.MAIN[block].GRCH[layer_index].GRAREN.word = ctrl->graren;
		IO_DISP.MAIN[block].GRCH[layer_index].GRBSL.word = ctrl->grbsl;

		// Set GRBLINK
		for(loop = D_IM_DISP_GRBLINK_SIZE - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].GRCH[layer_index].GRBLINK.word[loop] = ctrl->grblink.word[loop];
		}
		// Set GRBLINK/GRBITRG
		IO_DISP.MAIN[block].GRCH[layer_index].GRBINIT.word = ctrl->grbinit;
		IO_DISP.MAIN[block].GRCH[layer_index].GRBITRG.word = ctrl->grbitrg;
		// Set GRRSZ0/GRRSZ1/GRRSZ2/GRRSZ3
		IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ0.word = ctrl->grrsz0;
		IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ1.word = ctrl->grrsz1.word;
		IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ2.word = ctrl->grrsz2.word;
		IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ3.word = ctrl->grrsz3.word;

		// Set GRR2Y0~2/GRY2R0~2/GRCC0~2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			IO_DISP.MAIN[block].GRCH[layer_index].GRY2R[loop].word = ctrl->gry2r[loop].word;
		}
		// Set GRALP
		IO_DISP.MAIN[block].GRCH[layer_index].GRALP.word = ctrl->gralp;
	}

	// Recover RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.word = rpgen;
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* Get control data of OSD layer in common block.
*/
INT32 Im_DISP_Get_Ctrl_OSD_Layer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG* grrpgctl, T_IM_DISP_CTRL_OSD_LAYER *const ctrl)
{
	INT32 layer_index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((grrpgctl == NULL) && (ctrl == NULL)) {
		// Error when all arguments are NULL.
		Ddim_Print(("E:Im_DISP_Get_Ctrl_OSD_Layer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_Ctrl_OSD_Layer: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	if(grrpgctl != NULL){
		// Get GRRPGCTL
		*grrpgctl = (E_IM_DISP_RPGTMG)IO_DISP.MAIN[block].GRCH[layer_index].GRRPGCTL.word;
	}

	if(ctrl != NULL){
		// Get GRIDT/GRTISIZE/GRTDSTA/GRIPO/GRSCCTL
		ctrl->gridt.word = IO_DISP.MAIN[block].GRCH[layer_index].GRIDT.word;
		ctrl->grtisize.word = IO_DISP.MAIN[block].GRCH[layer_index].GRTISIZE.word;
		ctrl->grtdsta.word = IO_DISP.MAIN[block].GRCH[layer_index].GRTDSTA.word;
		ctrl->gripo.word = IO_DISP.MAIN[block].GRCH[layer_index].GRIPO.word;
		ctrl->grscctl_idset = (E_IM_DISP_INPUT_DATA_AREA)IO_DISP.MAIN[block].GRCH[layer_index].GRSCCTL.bit.IDSET;

		// Get GRISIZE/GRASA/GRHGA/GRAHGA/GRDSTA
		for(loop = D_IM_DISP_OSD_DISPLAY_AREA_COUNT - 1; loop >= 0; loop--) {
			ctrl->grisize[loop].word = IO_DISP.MAIN[block].GRCH[layer_index].GRISIZE[loop].word;
			ctrl->grasa[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRASA[loop].word;
			ctrl->grhga[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRHGA[loop].word;
			ctrl->grahga[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRAHGA[loop].word;
			ctrl->grdsta[loop].word = IO_DISP.MAIN[block].GRCH[layer_index].GRDSTA[loop].word;
		}
		// Get GRSA0
		for(loop = D_IM_DISP_OSD_ADDR_0_BANK_SIZE - 1; loop >= 0; loop--) {
			ctrl->grsa0[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRSA0[loop].word;
		}
		// Get GRSA
		for(loop = D_IM_DISP_OSD_ADDR_BANK_SIZE - 1; loop >= 0; loop--) {
			ctrl->grsa[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRSA[loop].word;
		}
		// Get GRAREN/GRBSL
		ctrl->graren = IO_DISP.MAIN[block].GRCH[layer_index].GRAREN.word;
		ctrl->grbsl = IO_DISP.MAIN[block].GRCH[layer_index].GRBSL.word;

		// Get GRBLINK
		for(loop = D_IM_DISP_GRBLINK_SIZE - 1; loop >= 0; loop--) {
			ctrl->grblink.word[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRBLINK.word[loop];
		}
		// Get GRBLINK/GRBITRG
		ctrl->grbinit = IO_DISP.MAIN[block].GRCH[layer_index].GRBINIT.word;
		ctrl->grbitrg = IO_DISP.MAIN[block].GRCH[layer_index].GRBITRG.word;
		// Get GRRSZ0/GRRSZ1/GRRSZ2/GRRSZ3
		ctrl->grrsz0 = (E_IM_DISP_RSZSL)IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ0.bit.RSZSL;
		ctrl->grrsz1.word = IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ1.word;
		ctrl->grrsz2.word = IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ2.word;
		ctrl->grrsz3.word = IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ3.word;
		
		// Get GRR2Y0~2/GRY2R0~2/GRCC0~2
		for(loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			ctrl->gry2r[loop].word = IO_DISP.MAIN[block].GRCH[layer_index].GRY2R[loop].word;
		}

		// Get GRALP
		ctrl->gralp = IO_DISP.MAIN[block].GRCH[layer_index].GRALP.word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Set Gamma table (Main data input block).
*/
INT32 Im_DISP_Ctrl_Main_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_TBL const *const ctrl_tbl)
{
	INT32 ret = D_DDIM_OK;
	ULONG rpgen;

#ifdef CO_PARAM_CHECK
	if (ctrl_tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Ctrl_Main_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((ctrl_tbl->anti_a.r_anti == NULL) ||
		(ctrl_tbl->anti_a.g_anti == NULL) ||
		(ctrl_tbl->anti_a.b_anti == NULL) ||
		(ctrl_tbl->anti_b.r_anti == NULL) ||
		(ctrl_tbl->anti_b.g_anti == NULL) ||
		(ctrl_tbl->anti_b.b_anti == NULL) ||
		(ctrl_tbl->gamma_a.r_data == NULL) ||
		(ctrl_tbl->gamma_a.g_data == NULL) ||
		(ctrl_tbl->gamma_a.b_data == NULL) ||
		(ctrl_tbl->gamma_b.r_data == NULL) ||
		(ctrl_tbl->gamma_b.g_data == NULL) ||
		(ctrl_tbl->gamma_b.b_data == NULL)) {
		Ddim_Print(("E:Im_DISP_Ctrl_Main_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN off
	rpgen = IO_DISP.MAIN[block].DCORE.RPGEN.word;
	IO_DISP.MAIN[block].DCORE.RPGEN.word = D_IM_DISP_ENABLE_OFF;
	Im_DISP_Dsb();

	// Anti gamma table.
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and main data input block is stopped or Anti gamma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Anti gamma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->anti_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLA)),
									E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and main data input block is stopped or Anti gamma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Anti gamma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->anti_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLB)),
									E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	
	// Gamma table is enable.
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and main data input block is stopped or Gamma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Gamma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLA)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and main data input block is stopped or Gamma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Gamma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLB)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	
	// Recover RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.word = rpgen;
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return  ret;
}

/* Get Gamma table (Main data input block).
*/
INT32 Im_DISP_Get_Ctrl_Main_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_TBL *const ctrl_tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (ctrl_tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Ctrl_Main_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((ctrl_tbl->anti_a.r_anti == NULL) ||
		(ctrl_tbl->anti_a.g_anti == NULL) ||
		(ctrl_tbl->anti_a.b_anti == NULL) ||
		(ctrl_tbl->anti_b.r_anti == NULL) ||
		(ctrl_tbl->anti_b.g_anti == NULL) ||
		(ctrl_tbl->anti_b.b_anti == NULL) ||
		(ctrl_tbl->gamma_a.r_data == NULL) ||
		(ctrl_tbl->gamma_a.g_data == NULL) ||
		(ctrl_tbl->gamma_a.b_data == NULL) ||
		(ctrl_tbl->gamma_b.r_data == NULL) ||
		(ctrl_tbl->gamma_b.g_data == NULL) ||
		(ctrl_tbl->gamma_b.b_data == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Ctrl_Main_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// Anti gamma table.
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and main data input block is stopped or Anti gamma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Anti gamma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->anti_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLA)),
									E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and main data input block is stopped or Anti gamma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Anti gamma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->anti_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLB)),
									E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	
	// Gamma table.
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and main data input block is stopped or Gamma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Gamma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLA)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and main data input block is stopped or Gamma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Gamma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLB)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	im_disp_pclk_off();

	return ret;
}

/* Set Gamma table (Data output block).
*/
INT32 Im_DISP_Ctrl_Output_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TBL const *const ctrl_tbl)
{
	INT32 ret = D_DDIM_OK;
	ULONG rpgen;

#ifdef CO_PARAM_CHECK
	if (ctrl_tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Ctrl_Output_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((ctrl_tbl->gamma_a.r_data == NULL) ||
		(ctrl_tbl->gamma_a.g_data == NULL) ||
		(ctrl_tbl->gamma_a.b_data == NULL) ||
		(ctrl_tbl->gamma_b.r_data == NULL) ||
		(ctrl_tbl->gamma_b.g_data == NULL) ||
		(ctrl_tbl->gamma_b.b_data == NULL) ||
		(ctrl_tbl->luminance_a == NULL) ||
		(ctrl_tbl->chroma_a == NULL) ||
		(ctrl_tbl->luminance_b == NULL) ||
		(ctrl_tbl->chroma_b == NULL)) {
		Ddim_Print(("E:Im_DISP_Ctrl_Output_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN off
	rpgen = IO_DISP.MAIN[block].DCORE.RPGEN.word;
	IO_DISP.MAIN[block].DCORE.RPGEN.word = D_IM_DISP_ENABLE_OFF;
	Im_DISP_Dsb();

	// Gamma table.
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and data output block is stopped or Gamma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Gamma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLA)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and data output block is stopped or Gamma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Gamma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLB)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	
	// Luminance and Chroma table.
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and data output block is stopped or Luminance and Chroma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Luminance table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->luminance_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLAYT)),
									E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_SET);
		// Set Chroma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->chroma_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLACG)),
									E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and data output block is stopped or Luminance and Chroma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Set Luminance table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->luminance_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBYT)),
									E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_SET);
		// Set Chroma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->chroma_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBCG)),
									E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_SET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}

	// Recover RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.word = rpgen;
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return  ret;
}

/* Get Gamma table (Data output block).
*/
INT32 Im_DISP_Get_Ctrl_Output_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TBL *const ctrl_tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (ctrl_tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Ctrl_Output_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((ctrl_tbl->gamma_a.r_data == NULL) ||
		(ctrl_tbl->gamma_a.g_data == NULL) ||
		(ctrl_tbl->gamma_a.b_data == NULL) ||
		(ctrl_tbl->gamma_b.r_data == NULL) ||
		(ctrl_tbl->gamma_b.g_data == NULL) ||
		(ctrl_tbl->gamma_b.b_data == NULL) ||
		(ctrl_tbl->luminance_a == NULL) ||
		(ctrl_tbl->chroma_a == NULL) ||
		(ctrl_tbl->luminance_b == NULL) ||
		(ctrl_tbl->chroma_b == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Ctrl_Output_Tbl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// Gamma table.
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and data output block is stopped or Gamma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Gamma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLA)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and data output block is stopped or Gamma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Gamma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->gamma_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLB)),
									E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	
	// Luminance and Chroma table.
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL == D_IM_DISP_TABLE_SURFACE_B))) {
		// Table access enable, and data output block is stopped or Luminance and Chroma table is using surface B.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Luminance table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->luminance_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLAYT)),
									E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_GET);
		// Get Chroma table (surface A)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->chroma_a)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLACG)),
									E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	if ((IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA == E_IM_DISP_TABLE_ACCESS_ENABLE) &&
		((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		 (IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL == D_IM_DISP_TABLE_SURFACE_A))) {
		// Table access enable, and data output block is stopped or Luminance and Chroma table is using surface A.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		// Get Luminance table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->luminance_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBYT)),
									E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_GET);
		// Get Chroma table (surface B)
		im_disp_access_gamma_table((BYTE*)(&(ctrl_tbl->chroma_b)), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBCG)),
									E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_GET);
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	
	im_disp_pclk_off();

	return ret;
}

/* The block is started. 
*/
INT32 Im_DISP_Start(UINT32 hdmi_layer, UINT32 lcd_mipi_layer, BYTE write_channel)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) &&
		((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) &&
		(write_channel == 0 )){
		Ddim_Print(("E:Im_DISP_Start: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// HDMI(Common block 0)
	if((hdmi_layer & E_IM_DISP_SEL_LAYER_ALL) != E_IM_DISP_SEL_LAYER_NONE) {
		ret = im_disp_set_trg(E_IM_DISP_HDMI, hdmi_layer, write_channel, E_IM_DISP_TRG_WRITE_ACT);
	}
	
	// LCD or MIPI-DSI(Common block 1)
	if((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_ALL) != E_IM_DISP_SEL_LAYER_NONE) {
		ret = im_disp_set_trg(E_IM_DISP_LCD_MIPI, lcd_mipi_layer, write_channel, E_IM_DISP_TRG_WRITE_ACT);
	}
	
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* The block is stopped.
*/
INT32 Im_DISP_Stop(UINT32 hdmi_layer, UINT32 lcd_mipi_layer, BYTE write_channel, E_IM_DISP_STOP_TYPE stop_type)
{
	E_IM_DISP_TRG set_type;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) &&
		((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) &&
		(write_channel == 0)){
		Ddim_Print(("E:Im_DISP_Stop: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	
	im_disp_pclk_on();
	// HDMI(Common block 0)
	if( hdmi_layer & (~E_IM_DISP_SEL_LAYER_NONE)) {
		if ((stop_type == E_IM_DISP_STOP_TYPE_WITHOUT_WAIT) ||
			(IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)) {
			//force stop
			set_type = E_IM_DISP_TRG_WRITE_NO_ACT;
		}
		else {
			//frame stop
			set_type = E_IM_DISP_TRG_READ_NO_ACT;
		}
		ret = im_disp_set_trg(E_IM_DISP_HDMI, hdmi_layer, write_channel, set_type);
	}
	else if ((lcd_mipi_layer == E_IM_DISP_SEL_LAYER_NONE) && (write_channel != 0)) {
		if (IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) {
			//force stop
			set_type = E_IM_DISP_TRG_WRITE_NO_ACT;
		}
		else {
			//frame stop
			set_type = E_IM_DISP_TRG_READ_NO_ACT;
		}
		ret = im_disp_set_trg(E_IM_DISP_HDMI, hdmi_layer, write_channel, set_type);
	}
	
	// LCD or MIPI-DSI(Common block 1)
	if( lcd_mipi_layer & (~E_IM_DISP_SEL_LAYER_NONE)) {
		if ((stop_type == E_IM_DISP_STOP_TYPE_WITHOUT_WAIT) ||
			(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)) {
			//force stop
			set_type = E_IM_DISP_TRG_WRITE_NO_ACT;
		}
		else {
			//frame stop
			set_type = E_IM_DISP_TRG_READ_NO_ACT;
		}
		ret = im_disp_set_trg(E_IM_DISP_LCD_MIPI, lcd_mipi_layer, write_channel, set_type);
	}
	
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* Get startup status of all blocks. 
*/
INT32 Im_DISP_Get_Status(UINT32* hdmi_layer, UINT32* lcd_mipi_layer)
{
	UINT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((hdmi_layer == NULL) && (lcd_mipi_layer == NULL)){
		Ddim_Print(("E:Im_DISP_Get_Status: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if (hdmi_layer != NULL){
		*hdmi_layer = E_IM_DISP_SEL_LAYER_NONE;

		//Check common block 0
		if(IO_DISP.MAIN[E_IM_DISP_HDMI].LCH.LTRG.word == E_IM_DISP_TRG_READ_ACT) {
			*hdmi_layer |= E_IM_DISP_SEL_LAYER_MAIN;
		}
		if(IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_ACT) {
			*hdmi_layer |= E_IM_DISP_SEL_LAYER_DCORE;
		}
		if(IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[0].GRTRG.word == E_IM_DISP_TRG_READ_ACT) {
			*hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_0;
		}
		if(IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[1].GRTRG.word == E_IM_DISP_TRG_READ_ACT) {
			*hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_1;
		}
	}

	if (lcd_mipi_layer != NULL){
		*lcd_mipi_layer = E_IM_DISP_SEL_LAYER_NONE;

		//Check common block 0
		if(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].LCH.LTRG.word == E_IM_DISP_TRG_READ_ACT) {
			*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_MAIN;
		}
		if(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_ACT) {
			*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_DCORE;
		}
		if(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[0].GRTRG.word == E_IM_DISP_TRG_READ_ACT) {
			*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_OSD_0;
		}
		if(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[1].GRTRG.word == E_IM_DISP_TRG_READ_ACT) {
			*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_OSD_1;
		}
	}

	im_disp_pclk_off();

	return ret;
}

/* Wait VSYNC interruption.
*/
INT32 Im_DISP_Wait_Vsync(E_IM_DISP_SEL block, E_IM_DISP_INTERRUPTION_SELECT interruption_select, E_IM_DISP_WAIT wait_time)
{
	DDIM_USER_ER ercd;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((interruption_select != E_IM_DISP_INTERRUPTION_SELECT_VE2) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_VE) &&
		(interruption_select != E_IM_DISP_CORRECT_SELECT_ALL_VE)) {
		Ddim_Print(("E:Im_DISP_Wait_Vsync: interruption_select parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if(IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_ACT) {
		//Channel's triger On
		if(IO_DISP.MAIN[block].DCORE.INTC.word != E_IM_DISP_VFTMG_NONE_FLAG) {
			// Because it is a setting that generate the flag.
			if((IO_DISP.MAIN[block].DCORE.INTE.word & interruption_select) != 0) {
				// Because it is a setting that generate the interrupt.
				DDIM_USER_FLGPTN flg_ptn;
				UINT32 disp_flg = im_disp_get_event_flag(interruption_select);
				for (loop = wait_time; loop > 0; loop--) {
					ercd = DDIM_User_Clr_Flg(FID_IM_DISP, ~disp_flg);
					if (ercd != D_DDIM_USER_E_OK) {
						// DDIM_User_Twai_Flg error
						Ddim_Print(("E:Im_DISP_Wait_Vsync: DDIM_User_Clr_Flg error. ercd = %d\n", ercd));
						ret = D_IM_DISP_STATUS_ABNORMAL;
					}
#ifdef CO_DEBUG_ON_PC
					IO_DISP.MAIN[block].DCORE.INTF.word = interruption_select;
					Im_DISP_Int_Handler();
#endif // CO_DEBUG_ON_PC
#ifdef CO_DEBUG_DISP
					Ddim_Print(("I:Im_DISP_Wait_Vsync: DDIM_User_Twai_Flg(%08X)\n", disp_flg));
#endif // CO_DEBUG_DISP
					ercd = DDIM_User_Twai_Flg(FID_IM_DISP, disp_flg, D_DDIM_USER_TWF_ORW, &flg_ptn, D_IM_DISP_VSYNC_TIME * 2);
					if (ercd != D_DDIM_USER_E_OK) {
						// DDIM_User_Twai_Flg error
						Ddim_Print(("E:Im_DISP_Wait_Vsync: DDIM_User_Twai_Flg error. ercd = %d\n", ercd));
						ret = D_IM_DISP_STATUS_ABNORMAL;
					}
				}
			}
			else{
				Ddim_Print(("E:Im_DISP_Wait_Vsync: INTE was not set to 1\n"));
				ret = D_IM_DISP_STATUS_ABNORMAL;
			}
		}
		else{
			Ddim_Print(("E:Im_DISP_Wait_Vsync: INTC was 0\n"));
			ret = D_IM_DISP_STATUS_ABNORMAL;
		}
	}
	else{
		Ddim_Print(("E:Im_DISP_Wait_Vsync: Output layer free\n"));
		ret = D_IM_DISP_STATUS_ABNORMAL;
	}
	im_disp_pclk_off();

	return ret;
}

/* Wait the block stopping after the @ref Im_DISP_Stop was called if it was necessary.
*/
INT32 Im_DISP_Wait_Stop(UINT32 hdmi_layer, UINT32 lcd_mipi_layer)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) &&
		((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE)){
		Ddim_Print(("E:Im_DISP_Wait_Stop: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	// Wait stop
	// common block 0
	if ((hdmi_layer & E_IM_DISP_SEL_LAYER_ALL) != E_IM_DISP_SEL_LAYER_NONE) {
		ret = im_disp_wait_trg_stop_start(E_IM_DISP_HDMI, hdmi_layer, E_IM_DISP_TRG_READ_NO_ACT);
		if (ret != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Wait_Stop: Time-out occur when waiting the block stop\n"));
		}
	}

	// common block 1
	if ((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_ALL) != E_IM_DISP_SEL_LAYER_NONE) {
		ret = im_disp_wait_trg_stop_start(E_IM_DISP_LCD_MIPI, lcd_mipi_layer, E_IM_DISP_TRG_READ_NO_ACT);
		if (ret != D_DDIM_OK){
			Ddim_Print(("E:Im_DISP_Wait_Stop: Time-out occur when waiting the block stop\n"));
		}
	}

	return ret;
}

/* Wait PWCH interruption.
*/
INT32 Im_DISP_Wait_PWCH(E_IM_DISP_DWCH_INTERRUPTION_SELECT interruption_select, E_IM_DISP_WAIT wait_time)
{
	DDIM_USER_ER ercd;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

	im_disp_pclk_on();
	// Because it is a setting that generate the flag.
	if(((IO_DISP.PWCH[E_IM_DISP_WC_NUM_0].PWCHINTENB.word & interruption_select) != 0) && ((IO_DISP.PWCH[E_IM_DISP_WC_NUM_1].PWCHINTENB.word & interruption_select) != 0)) {
		// Because it is a setting that generate the interrupt.
		DDIM_USER_FLGPTN flg_ptn;						
		UINT32 dwch_flg = im_disp_get_dwch_event_flag(interruption_select);
		UINT32 wait_ptn = COMBINE_FLG_INFO( dwch_flg, E_IM_DISP_WC_NUM_0 ) | COMBINE_FLG_INFO( dwch_flg, E_IM_DISP_WC_NUM_1 );
		for (loop = wait_time; loop > 0; loop--) {
			ercd = DDIM_User_Twai_Flg(FID_IM_DWCH, wait_ptn, D_DDIM_USER_TWF_ORW, &flg_ptn, 3000 );
			if (ercd != D_DDIM_USER_E_OK) {
				// DDIM_User_Twai_Flg error
				Ddim_Print(("E:Im_DISP_Wait_PWCH: DDIM_User_Twai_Flg error. ercd=%dn", ercd));
				ret = D_IM_DISP_STATUS_ABNORMAL;
			}
		}
	}
	else{
		Ddim_Print(("E:Im_DISP_Wait_PWCH: PWCHINTENB unmatch interruption_select error\n"));
		ret = D_IM_DISP_STATUS_ABNORMAL;
	}
	im_disp_pclk_off();

	return ret;
}

/* PWCH interruption. detect
*/
INT32 Im_DISP_PWCH_Detect(E_IM_DISP_DWCH_INTERRUPTION_SELECT interruption_select )
{
	DDIM_USER_ER ercd;
	INT32 ret = D_DDIM_OK;
	UINT32 dwch_flg = im_disp_get_dwch_event_flag(interruption_select);
	UINT32 flg_ptn = COMBINE_FLG_INFO( dwch_flg, E_IM_DISP_WC_NUM_0 ) | COMBINE_FLG_INFO( dwch_flg, E_IM_DISP_WC_NUM_1 );

	ercd = DDIM_User_Set_Flg(FID_IM_DWCH, flg_ptn );
	if (ercd != D_DDIM_USER_E_OK) {
		// DDIM_User_Twai_Flg error
		ret = D_IM_DISP_INPUT_PARAM_ERROR;
		Ddim_Print(("E:Im_DISP_PWCH_Detect: DDIM_User_Set_Flg error. ercd = %d\n", ercd));
	}
	return ret;
}

/* PWCH interruption. detect clear
*/
INT32 Im_DISP_PWCH_Detect_Clear(E_IM_DISP_DWCH_INTERRUPTION_SELECT interruption_select )
{
	DDIM_USER_ER ercd;
	INT32 ret = D_DDIM_OK;
	UINT32 dwch_flg = im_disp_get_dwch_event_flag(interruption_select);
	UINT32 wait_ptn = COMBINE_FLG_INFO( dwch_flg, E_IM_DISP_WC_NUM_0 ) | COMBINE_FLG_INFO( dwch_flg, E_IM_DISP_WC_NUM_1 );

	ercd = DDIM_User_Clr_Flg(FID_IM_DWCH, ~wait_ptn );
	if (ercd != D_DDIM_USER_E_OK) {
		// DDIM_User_Twai_Flg error
		ret = D_IM_DISP_INPUT_PARAM_ERROR;
		Ddim_Print(("E:Im_DISP_PWCH_Detect_Clear: DDIM_User_Clr_Flg error\n"));
	}
	return ret;
}

/* DISP interrupt handler.
*/
VOID Im_DISP_Int_Handler(VOID)
{
	INT32 loop;
	INT32 block_index;
	INT32 ch_index;
	INT32 cbf_index;
	INT32 cbf_dwch_index;
	DDIM_USER_ER ercd = D_DDIM_USER_E_OK;
	UINT32 flg_ptn = 0;

	E_IM_DISP_INTERRUPTION_SELECT interrupt_bit_map[] ={
		E_IM_DISP_INTERRUPTION_SELECT_VE2,		// Check 2nd field at first.
		E_IM_DISP_INTERRUPTION_SELECT_VE,

		E_IM_DISP_INTERRUPTION_SELECT_GR0EE,
		E_IM_DISP_INTERRUPTION_SELECT_GR1EE,
		E_IM_DISP_INTERRUPTION_SELECT_GA0EE,
		E_IM_DISP_INTERRUPTION_SELECT_GA1EE,
		E_IM_DISP_INTERRUPTION_SELECT_LEE,

		E_IM_DISP_INTERRUPTION_SELECT_GR0REE,
		E_IM_DISP_INTERRUPTION_SELECT_GR1REE,
		E_IM_DISP_INTERRUPTION_SELECT_GA0REE,
		E_IM_DISP_INTERRUPTION_SELECT_GA1REE,
		E_IM_DISP_INTERRUPTION_SELECT_LREE,
	};

	E_IM_DISP_DWCH_INTERRUPTION_SELECT dwch_interrupt_bit_map[] ={
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE,
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE,
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE,
	};

	im_disp_pclk_on();
#ifdef CO_DEBUG_DISP
	Ddim_Print(("I:Im_DISP_Int_Handler: start IO_DISP.MAIN[0].DCORE.INTF = %08X\n", (UINT32)IO_DISP.MAIN[0].DCORE.INTF.word));
	Ddim_Print(("I:Im_DISP_Int_Handler: start IO_DISP.MAIN[1].DCORE.INTF = %08X\n", (UINT32)IO_DISP.MAIN[1].DCORE.INTF.word));
#endif // CO_DEBUG_DISP
	for(block_index = D_IM_DISP_BLOCK_NUM - 1; block_index >= 0; block_index--) {

		for(loop = 0; loop < sizeof(interrupt_bit_map)/sizeof(interrupt_bit_map[0]); loop++) {
			if ((IO_DISP.MAIN[block_index].DCORE.INTF.word & interrupt_bit_map[loop]) != 0) {
#if 0
				if( IO_DISP.MAIN[block_index].DCORE.INTF.bit.LEF == 1 ){
					Ddim_Print(("E:DISP%d:LEF=1\n", block_index));
				}
				if( IO_DISP.MAIN[block_index].DCORE.INTF.bit.GR0EF == 1 ){
					Ddim_Print(("E:DISP%d:GR0EF=1\n", block_index));
				}
				if( IO_DISP.MAIN[block_index].DCORE.INTF.bit.GR1EF == 1 ){
					Ddim_Print(("E:DISP%d:GR1EF=1\n", block_index));
				}
				if( IO_DISP.MAIN[block_index].DCORE.INTF.bit.LREF == 1 ){
					Ddim_Print(("E:DISP%d:LREF=1\n", block_index));
				}
				if( IO_DISP.MAIN[block_index].DCORE.INTF.bit.GR0REF == 1 ){
					Ddim_Print(("E:DISP%d:GR0REF=1\n", block_index));
				}
				if( IO_DISP.MAIN[block_index].DCORE.INTF.bit.GR1REF == 1 ){
					Ddim_Print(("E:DISP%d:GR1REF=1\n", block_index));
				}
#endif

#ifdef CO_DEBUG_ON_PC
				UINT32 next_intf = IO_DISP.MAIN[block_index].DCORE.INTF.word & ~interrupt_bit_map[loop];
#endif // CO_DEBUG_ON_PC
				//Vertical synchronization interrupt
				UINT32 disp_flg = im_disp_get_event_flag(interrupt_bit_map[loop]);
				// Set the flag for waiting vertical synchronization interrupt
				ercd = DDIM_User_Set_Flg(FID_IM_DISP, disp_flg);
#ifdef CO_DEBUG_DISP
				Ddim_Print(("I:Im_DISP_Int_Handler: DDIM_User_Set_Flg(%08X)\n", disp_flg));
#endif // CO_DEBUG_DISP
				if (ercd != D_DDIM_USER_E_OK) {
					// DDIM_User_Set_Flg error
					Ddim_Print(("E:Im_DISP_Int_Handler: DDIM_User_Set_Flg error. ercd = %d\n", ercd));
				}
				
				cbf_index = im_disp_get_cb_index(interrupt_bit_map[loop]);
				// check callback
				if (gIM_DISP_Interrupt_Callback[block_index][cbf_index] != NULL) {
					//Have call back function
					gIM_DISP_Interrupt_Callback[block_index][cbf_index]();
				}
				IO_DISP.MAIN[block_index].DCORE.INTF.word = interrupt_bit_map[loop];
#ifdef CO_DEBUG_ON_PC
				IO_DISP.MAIN[block_index].DCORE.INTF.word = next_intf;
#endif // CO_DEBUG_ON_PC
			}
			
			if ((IO_DISP.MAIN[block_index].DCORE.INTF.word & E_IM_DISP_CORRECT_SELECT_ALL) == 0) {
				break;
			}
		}
	}

	//PWCH
	for(ch_index = D_IM_DISP_DWCH_CHANNEL_NUM - 1; ch_index >= 0; ch_index--) {

		for(loop = 0; loop < sizeof(dwch_interrupt_bit_map)/sizeof(dwch_interrupt_bit_map[0]); loop++) {
			if ((IO_DISP.PWCH[ch_index].PWCHINTFLG.word & dwch_interrupt_bit_map[loop]) != 0) {

#if 0
				if( IO_DISP.PWCH[ch_index].PWCHINTFLG.bit.__PWEF == 1 ){
					Ddim_Print(("E:DISP:PWEF%d=1\n", ch_index));
				}
				if( IO_DISP.PWCH[ch_index].PWCHINTFLG.bit.__PWXF == 1 ){
					Ddim_Print(("E:DISP:PWXF%d=1\n", ch_index));
				}
#endif

				UINT32 dwch_flg = im_disp_get_dwch_event_flag(dwch_interrupt_bit_map[loop]);
				flg_ptn |= COMBINE_FLG_INFO( dwch_flg, ch_index );
				cbf_dwch_index = im_disp_get_dwch_cb_index(dwch_interrupt_bit_map[loop]);
				// check callback
				if (gIM_DISP_DWCH_Interrupt_Callback[ch_index][cbf_dwch_index] != NULL) {
					//Have call back function
					gIM_DISP_DWCH_Interrupt_Callback[ch_index][cbf_dwch_index]();
				}
				IO_DISP.PWCH[ch_index].PWCHINTFLG.word = dwch_interrupt_bit_map[loop];
			}
			
			if ((IO_DISP.PWCH[ch_index].PWCHINTFLG.word & E_IM_DISP_DWCH_CORRECT_SELECT_ALL) == 0) {
				break;
			}
		}
		if( flg_ptn != 0 ) {
			if (ercd != D_DDIM_USER_E_OK) {
				// DDIM_User_Set_Flg error
				Ddim_Print(("E:Im_DISP_Int_Handler: DDIM_User_Set_Flg error. ercd = %d\n", ercd));
			}
		}
	}

#ifdef CO_DEBUG_DISP
	Ddim_Print(("I:Im_DISP_Int_Handler: after IO_DISP.MAIN[0].DCORE.INTF = %08X\n", (UINT32)IO_DISP.MAIN[0].DCORE.INTF.word));
	Ddim_Print(("I:Im_DISP_Int_Handler: after IO_DISP.MAIN[1].DCORE.INTF = %08X\n", (UINT32)IO_DISP.MAIN[1].DCORE.INTF.word));
#endif // CO_DEBUG_DISP
	im_disp_pclk_off();
}

/* Set SR.
*/
INT32 Im_DISP_Set_SR(UINT32 hdmi_layer, UINT32 lcd_mipi_layer, BYTE sr)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) &&
		((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE)){
		Ddim_Print(("E:Im_DISP_Set_SR: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((sr != D_IM_DISP_SR_CANCEL) && (sr != D_IM_DISP_SR_RESET)) {
		Ddim_Print(("E:Im_DISP_Set_SR: sr parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// common block 0 (HDMI)
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_MAIN) == E_IM_DISP_SEL_LAYER_MAIN) &&
		(IO_DISP.MAIN[E_IM_DISP_HDMI].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_HDMI].LCH.LRST.bit.SR = sr;		// Main data input block
	}
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_DCORE) == E_IM_DISP_SEL_LAYER_DCORE) &&
		(IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.RESET.bit.SR = sr;	// Output data block
	}
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_OSD_0) == E_IM_DISP_SEL_LAYER_OSD_0) &&
		(IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[0].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[0].GRRST.bit.SR = sr;	// OSD_0 data input block
	}
	if (((hdmi_layer & E_IM_DISP_SEL_LAYER_OSD_1) == E_IM_DISP_SEL_LAYER_OSD_1) &&
		(IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[1].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[1].GRRST.bit.SR = sr;	// OSD_1 data input block
	}
	
	// common block 1 (LCD or MIPI)
	if (((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_MAIN) == E_IM_DISP_SEL_LAYER_MAIN) &&
		(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].LCH.LRST.bit.SR = sr;		// Main data input block
	}
	if (((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_DCORE) == E_IM_DISP_SEL_LAYER_DCORE) &&
		(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].DCORE.RESET.bit.SR = sr;	// Output data block
	}
	if (((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_OSD_0) == E_IM_DISP_SEL_LAYER_OSD_0) &&
		(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[0].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[0].GRRST.bit.SR = sr;	// OSD_0 data input block
	}
	if (((lcd_mipi_layer & E_IM_DISP_SEL_LAYER_OSD_1) == E_IM_DISP_SEL_LAYER_OSD_1) &&
		(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[1].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[1].GRRST.bit.SR = sr;	// OSD_1 data input block
	}
	
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* Get SR.
*/
INT32 Im_DISP_Get_SR(UINT32* hdmi_layer, UINT32* lcd_mipi_layer)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((hdmi_layer == NULL) || (lcd_mipi_layer == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_SR: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	*hdmi_layer = E_IM_DISP_SEL_LAYER_NONE;
	*lcd_mipi_layer = E_IM_DISP_SEL_LAYER_NONE;

	// common block 0 (HDMI)
	if (IO_DISP.MAIN[E_IM_DISP_HDMI].LCH.LRST.bit.SR == D_IM_DISP_SR_RESET) {
		*hdmi_layer |= E_IM_DISP_SEL_LAYER_MAIN;
	}
	if (IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.RESET.bit.SR == D_IM_DISP_SR_RESET) {
		*hdmi_layer |= E_IM_DISP_SEL_LAYER_DCORE;
	}
	if (IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[0].GRRST.bit.SR == D_IM_DISP_SR_RESET) {
		*hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_0;
	}
	if (IO_DISP.MAIN[E_IM_DISP_HDMI].GRCH[1].GRRST.bit.SR == D_IM_DISP_SR_RESET) {
		*hdmi_layer |= E_IM_DISP_SEL_LAYER_OSD_1;
	}
	
	// common block 1 (LCD or MIPI)
	if (IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].LCH.LRST.bit.SR == D_IM_DISP_SR_RESET) {
		*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_MAIN;
	}
	if (IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].DCORE.RESET.bit.SR == D_IM_DISP_SR_RESET) {
		*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_DCORE;
	}
	if (IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[0].GRRST.bit.SR == D_IM_DISP_SR_RESET) {
		*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_OSD_0;
	}
	if (IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].GRCH[1].GRRST.bit.SR == D_IM_DISP_SR_RESET) {
		*lcd_mipi_layer |= E_IM_DISP_SEL_LAYER_OSD_1;
	}
	
	im_disp_pclk_off();

	return ret;
}

/* Set parameter reflect timing.
*/
INT32 Im_DISP_Set_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG timing)
{
	INT32 ret = D_DDIM_OK;

	im_disp_pclk_on();
	if (IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT) {
		// Set LRPGCTL
		IO_DISP.MAIN[block].LCH.LRPGCTL.bit.RPGTMG = timing;
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Parameter_Reflect_Timing: Main layer busy\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get parameter reflect timing.
*/
INT32 Im_DISP_Get_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG *timing)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (timing == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Parameter_Reflect_Timing: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LRPGCTL
	*timing = (E_IM_DISP_RPGTMG)IO_DISP.MAIN[block].LCH.LRPGCTL.bit.RPGTMG;
	im_disp_pclk_off();

	return ret;
}

/* Set input data transfer.
*/
INT32 Im_DISP_Set_Input_Data_Transfer(E_IM_DISP_SEL block, U_IM_DISP_LIDT lidt)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (im_disp_param_check_lidt(block, lidt) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Input_Data_Transfer: LIDT parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LIDT0/LIDT1
	IO_DISP.MAIN[block].LCH.LIDT0.word = lidt.word[0];
	IO_DISP.MAIN[block].LCH.LIDT1.word = lidt.word[1];
	im_disp_pclk_off();

	return ret;
}

/* Get input data transfer.
*/
INT32 Im_DISP_Get_Input_Data_Transfer(E_IM_DISP_SEL block, U_IM_DISP_LIDT *lidt)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (lidt == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Input_Data_Transfer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LIDT0/LIDT1
	lidt->word[0] = IO_DISP.MAIN[block].LCH.LIDT0.word;
	lidt->word[1] = IO_DISP.MAIN[block].LCH.LIDT1.word;
	im_disp_pclk_off();

	return ret;
}

/* Set input data size of MAIN.
*/
INT32 Im_DISP_Set_Input_Size(E_IM_DISP_SEL block, U_IM_DISP_SIZE input_size)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (im_disp_param_check_lisize(input_size) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Input_Size: LISIZE parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LISIZE
	IO_DISP.MAIN[block].LCH.LISIZE.word = input_size.word;
	im_disp_pclk_off();

	return ret;
}

/* Get input data size of MAIN.
*/
INT32 Im_DISP_Get_Input_Size(E_IM_DISP_SEL block, U_IM_DISP_SIZE *input_size)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (input_size == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Input_Size: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LISIZE
	input_size->word = IO_DISP.MAIN[block].LCH.LISIZE.word;
	im_disp_pclk_off();

	return ret;
}

/* Connect MAIN input data address with the bank index.
*/
INT32 Im_DISP_Set_Addr(E_IM_DISP_SEL block, E_IM_DISP_BANK bank_no, T_IM_DISP_IMAGE_ADDR const *const address)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (address == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Addr: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LYSA0~3/LCSA0~3
	IO_DISP.MAIN[block].LCH.LYSA[bank_no].word = address->y_addr;
	IO_DISP.MAIN[block].LCH.LCSA[bank_no].word = address->c_addr;
	im_disp_pclk_off();

	return ret;
}

/* Get MAIN input data address of the bank index specified.
*/
INT32 Im_DISP_Get_Addr(E_IM_DISP_SEL block, E_IM_DISP_BANK bank_no, T_IM_DISP_IMAGE_ADDR *const address)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (address == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Addr: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LYSA0~3/LCSA0~3
	address->y_addr = IO_DISP.MAIN[block].LCH.LYSA[bank_no].word;
	address->c_addr = IO_DISP.MAIN[block].LCH.LCSA[bank_no].word;
	im_disp_pclk_off();

	return ret;
}

/* Set the global size.
*/
INT32 Im_DISP_Set_Lfd(E_IM_DISP_SEL block, ULONG y_hga, ULONG c_hga)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (im_disp_param_check_lfd(y_hga, c_hga) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_Lfd: LYHGA/LCHGA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LYHGA/LCHGA
	IO_DISP.MAIN[block].LCH.LYHGA.word = y_hga;
	IO_DISP.MAIN[block].LCH.LCHGA.word = c_hga;
	im_disp_pclk_off();

	return ret;
}

/* Get the global size.
*/
INT32 Im_DISP_Get_Lfd(E_IM_DISP_SEL block, ULONG* y_hga, ULONG* c_hga)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((y_hga == NULL) || (c_hga == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Lfd: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LYHGA/LCHGA
	*y_hga = IO_DISP.MAIN[block].LCH.LYHGA.word;
	*c_hga = IO_DISP.MAIN[block].LCH.LCHGA.word;
	im_disp_pclk_off();

	return ret;
}

/* Set main bank No. to show.
*/
VOID Im_DISP_Set_Main_Bank(E_IM_DISP_SEL block, E_IM_DISP_BANK bank_no)
{
	im_disp_pclk_on();
	// Set LIBCTL.IBSET
	IO_DISP.MAIN[block].LCH.LIBCTL.bit.IBSET = bank_no;
	im_disp_pclk_off();
}

/* Get main bank No. to show or showing.
*/
INT32 Im_DISP_Get_Main_Bank(E_IM_DISP_SEL block, E_IM_DISP_BANK *bank_no)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (bank_no == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Main_Bank: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LIBCTL.IBSET
	*bank_no = (E_IM_DISP_BANK)IO_DISP.MAIN[block].LCH.LIBCTL.bit.IBSET;
	im_disp_pclk_off();

	return ret;
}

/* Get main bank No. to showing.
*/
INT32 Im_DISP_Get_Main_Bank_Monitor(E_IM_DISP_SEL block, E_IM_DISP_BANK *bank_no)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (bank_no == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Main_Bank_Monitor: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LIBCTL.IBMNT
	*bank_no = (E_IM_DISP_BANK)IO_DISP.MAIN[block].LCH.LIBCTL.bit.IBMNT;
	im_disp_pclk_off();

	return ret;
}

/* Set resize value.
*/
INT32 Im_DISP_Set_Resize(E_IM_DISP_SEL block, T_IM_DISP_RESIZE const *const resize)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (resize == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Resize: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_resize(resize->rsz0, resize->rsz1, resize->rsz2, resize->rsz3) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_Resize: LHRSZ0/LHRSZ1/LVRSZ parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LRSZ0/LRSZ1/LRSZ2/LRSZ3
	IO_DISP.MAIN[block].LCH.LRSZ0.word = resize->rsz0;
	IO_DISP.MAIN[block].LCH.LRSZ1.word = resize->rsz1.word;
	IO_DISP.MAIN[block].LCH.LRSZ2.word = resize->rsz2.word;
	IO_DISP.MAIN[block].LCH.LRSZ3.word = resize->rsz3.word;
	im_disp_pclk_off();

	return ret;
}

/* Get resize value.
*/
INT32 Im_DISP_Get_Resize(E_IM_DISP_SEL block, T_IM_DISP_RESIZE *const resize)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (resize == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Resize: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LRSZ0/LRSZ1/LRSZ2/LRSZ3
	resize->rsz0 = IO_DISP.MAIN[block].LCH.LRSZ0.word;
	resize->rsz1.word = IO_DISP.MAIN[block].LCH.LRSZ1.word;
	resize->rsz2.word = IO_DISP.MAIN[block].LCH.LRSZ2.word;
	resize->rsz3.word = IO_DISP.MAIN[block].LCH.LRSZ3.word;

	im_disp_pclk_off();

	return ret;
}

/* Set YHLP data.
*/
INT32 Im_DISP_Set_Yhlp(E_IM_DISP_SEL block, T_IM_DISP_YHLP const *const yhlp)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (yhlp == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Yhlp: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LYHLPCTL/LYHLPK0/LYHLPK1/LYHLPOL/LYHLPCLR
	IO_DISP.MAIN[block].LCH.LYHLPCTL.word = yhlp->lyhlpcl.word;
	IO_DISP.MAIN[block].LCH.LYHLPK0.word = yhlp->lyhlpk.word[0];
	IO_DISP.MAIN[block].LCH.LYHLPK1.word = yhlp->lyhlpk.word[1];
	IO_DISP.MAIN[block].LCH.LYHLPOL.word = yhlp->lyhlpol.word;
	IO_DISP.MAIN[block].LCH.LYHLPCLR.dword = yhlp->lyhlpclr.dword;
	im_disp_pclk_off();

	return ret;
}

/* Get YHLP data.
*/
INT32 Im_DISP_Get_Yhlp(E_IM_DISP_SEL block, T_IM_DISP_YHLP *const yhlp)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (yhlp == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Yhlp: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LYHLPCTL/LYHLPK0/LYHLPK1/LYHLPOL/LYHLPCLR
	yhlp->lyhlpcl.word = IO_DISP.MAIN[block].LCH.LYHLPCTL.word;
	yhlp->lyhlpk.word[0] = IO_DISP.MAIN[block].LCH.LYHLPK0.word;
	yhlp->lyhlpk.word[1] = IO_DISP.MAIN[block].LCH.LYHLPK1.word;
	yhlp->lyhlpol.word = IO_DISP.MAIN[block].LCH.LYHLPOL.word;
	yhlp->lyhlpclr.dword = IO_DISP.MAIN[block].LCH.LYHLPCLR.dword;
	im_disp_pclk_off();

	return ret;
}

/* Set warning processing data.
*/
INT32 Im_DISP_Set_Warning(E_IM_DISP_SEL block, T_IM_DISP_WARNING const *const warning)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (warning == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Warning: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_warning(warning) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Warning: LYWTH parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LYWCTL/LYW0TH/LYW1TH/LYW0ST/LYW0CA/LYW0CB/LYW1ST/LYW1CA/LYW1CB/LBOST
	IO_DISP.MAIN[block].LCH.LYWCTL.word = warning->lywctl.word;
	IO_DISP.MAIN[block].LCH.LYW0TH.word = warning->lyw0th.word;
	IO_DISP.MAIN[block].LCH.LYW1TH.word = warning->lyw1th.word;
	IO_DISP.MAIN[block].LCH.LYW0ST.word = warning->lyw0st;
	IO_DISP.MAIN[block].LCH.LYW0CA.dword = warning->lyw0ca.dword;
	IO_DISP.MAIN[block].LCH.LYW0CB.dword = warning->lyw0cb.dword;
	IO_DISP.MAIN[block].LCH.LYW1ST.word = warning->lyw1st;
	IO_DISP.MAIN[block].LCH.LYW1CA.dword = warning->lyw1ca.dword;
	IO_DISP.MAIN[block].LCH.LYW1CB.dword = warning->lyw1cb.dword;
	IO_DISP.MAIN[block].LCH.LBOST.word = warning->lbost;
	im_disp_pclk_off();

	return ret;
}

/* Get warning.
*/
INT32 Im_DISP_Get_Warning(E_IM_DISP_SEL block, T_IM_DISP_WARNING *const warning)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (warning == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Warning: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LYWCTL/LYW0TH/LYW1TH/LYW0ST/LYW0CA/LYW0CB/LYW1ST/LYW1CA/LYW1CB/LBOST
	warning->lywctl.word = IO_DISP.MAIN[block].LCH.LYWCTL.word;
	warning->lyw0th.word = IO_DISP.MAIN[block].LCH.LYW0TH.word;
	warning->lyw1th.word = IO_DISP.MAIN[block].LCH.LYW1TH.word;
	warning->lyw0st = (E_IM_DISP_YWMD)IO_DISP.MAIN[block].LCH.LYW0ST.word;
	warning->lyw0ca.dword = IO_DISP.MAIN[block].LCH.LYW0CA.dword;
	warning->lyw0cb.dword = IO_DISP.MAIN[block].LCH.LYW0CB.dword;
	warning->lyw1st = (E_IM_DISP_YWMD)IO_DISP.MAIN[block].LCH.LYW1ST.word;
	warning->lyw1ca.dword = IO_DISP.MAIN[block].LCH.LYW1CA.dword;
	warning->lyw1cb.dword = IO_DISP.MAIN[block].LCH.LYW1CB.dword;
	warning->lbost = IO_DISP.MAIN[block].LCH.LBOST.word;
	im_disp_pclk_off();

	return ret;
}

/* Set blink timer control data.
*/
VOID Im_DISP_Set_Blink_Timer(E_IM_DISP_SEL block, U_IM_DISP_LBLTMR lbltmr)
{
	im_disp_pclk_on();
	// Set LBLTMR
	IO_DISP.MAIN[block].LCH.LBLTMR.word = lbltmr.word;
	im_disp_pclk_off();
}

/* Get blink timer control data.
*/
INT32 Im_DISP_Get_Blink_Timer(E_IM_DISP_SEL block, U_IM_DISP_LBLTMR *lbltmr)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(lbltmr == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Blink_Timer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LBLTMR
	lbltmr->word = IO_DISP.MAIN[block].LCH.LBLTMR.word;
	im_disp_pclk_off();

	return ret;
}

/* Set zebra warning data.
*/
INT32 Im_DISP_Set_Zebra(E_IM_DISP_SEL block, T_IM_DISP_ZEBRA const *const zebra)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (zebra == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Zebra: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_zebra(zebra) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Zebra: LYWTH parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LZBWID/LZBV/LZBPT
	IO_DISP.MAIN[block].LCH.LZBWID.word = zebra->lzbwid.word;
	IO_DISP.MAIN[block].LCH.LZBV.word = zebra->lzbv.word;
	IO_DISP.MAIN[block].LCH.LZBPT.word = zebra->lzbpt.word;
	im_disp_pclk_off();

	return ret;
}

/* Get zebra warning data.
*/
INT32 Im_DISP_Get_Zebra(E_IM_DISP_SEL block, T_IM_DISP_ZEBRA *const zebra)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (zebra == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Zebra: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LZBWID/LZBV/LZBPT
	zebra->lzbwid.word = IO_DISP.MAIN[block].LCH.LZBWID.word;
	zebra->lzbv.word = IO_DISP.MAIN[block].LCH.LZBV.word;
	zebra->lzbpt.word = IO_DISP.MAIN[block].LCH.LZBPT.word;
	im_disp_pclk_off();

	return ret;
}

/* Set YCbCr->RGB matrix.
*/
INT32 Im_DISP_Set_Matrix(E_IM_DISP_SEL block, const U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE])
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (matrix == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Matrix: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// YCbCr-> RGB's matrix
	for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
		// Set LY2R0~2
		IO_DISP.MAIN[block].LCH.LY2R[loop].dword = matrix[loop].dword;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get YCbCr->RGB matrix.
*/
INT32 Im_DISP_Get_Matrix(E_IM_DISP_SEL block, U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE])
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (matrix == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Matrix: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// YCbCr-> RGB's matrix
	for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
		// Set LY2R0~2
		matrix[loop].dword = IO_DISP.MAIN[block].LCH.LY2R[loop].dword;
	}
	im_disp_pclk_off();

	return ret;
}

/* Set gamma correct enable.
*/
INT32 Im_DISP_Set_Gamma_Enable(E_IM_DISP_SEL block, E_IM_DISP_CORRECT_SELECT set_item, T_IM_DISP_TBL_ACCESS_SET const *const correct)
{
	INT32 ret = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (correct == NULL) {
		// Error when argument is NULL.
		Ddim_Print(("E:Im_DISP_Set_Gamma_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	switch (set_item) {
		// Main data input block RAM table access (LTBLASET).
		case E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA:			// Anti-gamma.
			// check status.
			if ((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
				(correct->access == E_IM_DISP_TABLE_ACCESS_ENABLE)) {
				// Set LTBLASET.IGTAEN.
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN = correct->access;
				im_disp_wait_usec(1);		// Wait 1 usec
			}
			else {
				// not setting table access disable.
				Ddim_Print(("E:Im_DISP_Set_Gamma_Enable: not setting anti-gamma access disable\n"));
			}
			
			// Set LTBLASET.IGEN
			if (correct->proc) {
				// check anti-gamma table access enable.
				if (IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) {
					// setting enable.
					IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGEN = E_IM_DISP_CORRECT_PROC_ENABLE;
				}
				else {
					// not setting enable.
					Ddim_Print(("W:Im_DISP_Set_Gamma_Enable: not setting anti-gamma correction enable\n"));
				}
			}
			else {
				// settign disable.
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGEN = E_IM_DISP_CORRECT_PROC_DISABLE;
			}
		
			// Set LTBLASET.IGTSL
			IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL = correct->surface;
			break;
		case E_IM_DISP_CORRECT_SELECT_GAMMA_IN:				// Gamma(Main data input block).
			// check status.
			if ((IO_DISP.MAIN[block].LCH.LTRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
				(correct->access == E_IM_DISP_TABLE_ACCESS_ENABLE)) {
				// Set LTBLASET.GTAEN.
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN = correct->access;
				im_disp_wait_usec(1);		// Wait 1 usec
			}
			else {
				// not setting table access disable.
				Ddim_Print(("E:Im_DISP_Set_Gamma_Enable: not setting gamma(main data input block) access disable\n"));
			}
			
			// Set LTBLASET.GMEN
			if (correct->proc) {
				// check gamma table access enable.
				if (IO_DISP.MAIN[block].LCH.LTBLASET.bit.GTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) {
					// setting enable.
					IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_ENABLE;
				}
				else {
					// not setting enable.
					Ddim_Print(("W:Im_DISP_Set_Gamma_Enable: not setting Gamma(Main data input block) correction enable\n"));
				}
			}
			else {
				// settign disable.
				IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_DISABLE;
			}
		
			// Set LTBLASET.GMTSL
			IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL = correct->surface;
			break;
		
		// Data output block RAM table access (TBLASET).
		case E_IM_DISP_CORRECT_SELECT_GAMMA_OUT:			// Gamma(Data output block).
			// check status.
			if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
				(correct->access == E_IM_DISP_TABLE_ACCESS_ENABLE)) {
				// Set TBLASET.GMTAEN.
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN = correct->access;
				im_disp_wait_usec(1);		// Wait 1 usec
			}
			else {
				// not setting table access disable.
				Ddim_Print(("E:Im_DISP_Set_Gamma_Enable: not setting gamma(data output block) access disable\n"));
			}
			
			// Set TBLASET.GMEN
			if (correct->proc) {
				// check gamma table access enable.
				if (IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTAEN == E_IM_DISP_TABLE_ACCESS_ENABLE) {
					// setting enable.
					IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_ENABLE;
				}
				else {
					// not setting enable.
					Ddim_Print(("W:Im_DISP_Set_Gamma_Enable: not setting Gamma(data output block) correction enable\n"));
				}
			}
			else {
				// settign disable.
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMEN = E_IM_DISP_CORRECT_PROC_DISABLE;
			}
			
			// Set TBLASET.GMTSL
			IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL = correct->surface;
			break;
		default:											// Luminance and chroma.
			// check status.
			if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
				(correct->access == E_IM_DISP_TABLE_ACCESS_ENABLE)) {
				// Set TBLASET.YSATTA.
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA = correct->access;
				im_disp_wait_usec(1);		// Wait 1 usec
			}
			else {
				// not setting table access disable.
				Ddim_Print(("E:Im_DISP_Set_Gamma_Enable: not setting luminance and chroma access disable\n"));
			}
			
			// Set TBLASET.YSATEN
			if (correct->proc) {
				// check Luminance and chroma table access enable.
				if (IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATTA == E_IM_DISP_TABLE_ACCESS_ENABLE) {
					// setting enable.
					IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATEN = E_IM_DISP_CORRECT_PROC_ENABLE;
				}
				else {
					// not setting enable.
					Ddim_Print(("W:Im_DISP_Set_Gamma_Enable: not setting Luminance and chroma correction enable\n"));
				}
			}
			else {
				// settign disable.
				IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATEN = E_IM_DISP_CORRECT_PROC_DISABLE;
			}
			
			// Set TBLASET.YSATSL
			IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL = correct->surface;
			break;
	}
	
	im_disp_pclk_off();
	
	return ret;
}

/* Get gamma correct enable.
*/
INT32 Im_DISP_Get_Gamma_Enable(E_IM_DISP_SEL block, U_IM_DISP_LTBLASET *lch_ltblaset, U_IM_DISP_TBLASET *dcore_tblaset)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((lch_ltblaset == NULL) || (dcore_tblaset == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Gamma_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LTBLASET.
	lch_ltblaset->word = IO_DISP.MAIN[block].LCH.LTBLASET.word;
	// Get TBLASET.
	dcore_tblaset->word = IO_DISP.MAIN[block].DCORE.TBLASET.word;
	im_disp_pclk_off();

	return ret;
}

/* Set gamma correct mode.
*/
VOID Im_DISP_Set_Gamma_Mode(E_IM_DISP_SEL block, U_IM_DISP_LGMMD lgmmd)
{
	im_disp_pclk_on();
	
	// Set LGMMD.
	IO_DISP.MAIN[block].LCH.LGMMD.word = lgmmd.word;
	
	im_disp_pclk_off();
}

/* Get gamma correct mode.
*/
INT32 Im_DISP_Get_Gamma_Mode(E_IM_DISP_SEL block, U_IM_DISP_LGMMD* lgmmd)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (lgmmd == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Gamma_Mode: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LGMMD.
	lgmmd->word = IO_DISP.MAIN[block].LCH.LGMMD.word;
	im_disp_pclk_off();

	return ret;
}

/* Set Alpha value.
*/
VOID Im_DISP_Set_Alpha_Value(E_IM_DISP_SEL block, U_IM_DISP_LALP lalp)
{
	im_disp_pclk_on();
	
	// Set LALP.
	IO_DISP.MAIN[block].LCH.LALP.word = lalp.word;
	
	im_disp_pclk_off();
}

/* Get Alpha value.
*/
INT32 Im_DISP_Get_Alpha_Value(E_IM_DISP_SEL block, U_IM_DISP_LALP *lalp)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (lalp == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Alpha_Value: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LALP.
	lalp->word = IO_DISP.MAIN[block].LCH.LALP.word;
	im_disp_pclk_off();

	return ret;
}

/* Set display position.
*/
INT32 Im_DISP_Set_Display_Position(E_IM_DISP_SEL block, U_IM_DISP_DSTA position)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (im_disp_param_check_ldsta(position) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_Display_Position: LDSTA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set LDSTA.
	IO_DISP.MAIN[block].LCH.LDSTA.word = position.word;
	im_disp_pclk_off();

	return ret;
}

/* Get display position.
*/
INT32 Im_DISP_Get_Display_Position(E_IM_DISP_SEL block, U_IM_DISP_DSTA *position)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(position == NULL){
		Ddim_Print(("E:Im_DISP_Get_Display_Position: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LDSTA.
	position->word = IO_DISP.MAIN[block].LCH.LDSTA.word;
	im_disp_pclk_off();

	return ret;
}

/* Set reverse display setting.
*/
VOID Im_DISP_Set_Reverse_Display(E_IM_DISP_SEL block, U_IM_DISP_LREVDISP reverse)
{
	im_disp_pclk_on();
	// Set LREVDISP.
	IO_DISP.MAIN[block].LCH.LREVDISP.word = reverse.word;
	im_disp_pclk_off();
}

/* Get reverse display setting.
*/
INT32 Im_DISP_Get_Reverse_Display(E_IM_DISP_SEL block, U_IM_DISP_LREVDISP *reverse)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (reverse == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Reverse_Display: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get LREVDISP.
	reverse->word = IO_DISP.MAIN[block].LCH.LREVDISP.word;
	im_disp_pclk_off();

	return ret;
}

/* Set display interface selection.
*/
INT32 Im_DISP_Set_Display_Interface(E_IM_DISP_SEL block, BYTE ifs)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ifs != D_IM_DISP_IFS_NONE) &&
		(ifs != D_IM_DISP_IFS_LCD) &&
		(ifs != D_IM_DISP_IFS_HDMI) &&
		(ifs != D_IM_DISP_IFS_MIPI)) {
		Ddim_Print(("E:Im_DISP_Set_Display_Interface: ifs parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((block == E_IM_DISP_HDMI) &&
		((ifs == D_IM_DISP_IFS_LCD) || (ifs == D_IM_DISP_IFS_MIPI))) {
		Ddim_Print(("E:Im_DISP_Set_Display_Interface: ifs parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((block == E_IM_DISP_LCD_MIPI) && (ifs == D_IM_DISP_IFS_HDMI)) {
		Ddim_Print(("E:Im_DISP_Set_Display_Interface: ifs parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if(IO_DISP.MAIN[block].DCORE.RESET.bit.SR == D_IM_DISP_SR_RESET){
		// Set IFS.IFS
		IO_DISP.MAIN[block].DCORE.IFS.bit.IFS = ifs;
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Display_Interface: SR was not set to 1\n"));
		ret = D_IM_DISP_STATUS_ABNORMAL;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get display interface selection.
*/
INT32 Im_DISP_Get_Display_Interface(E_IM_DISP_SEL block, BYTE *ifs)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(ifs == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Display_Interface: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get IFS.IFS
	*ifs = IO_DISP.MAIN[block].DCORE.IFS.bit.IFS;
	im_disp_pclk_off();

	return ret;
}

/* Set external start-up mode.
*/
INT32 Im_DISP_Set_External_Startup_Mode(E_IM_DISP_SEL block, E_IM_DISP_TGKMD tgkmd)
{
	INT32 ret = D_DDIM_OK;

	im_disp_pclk_on();
	if(IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT){
		// Set TRG.TGKMD
		IO_DISP.MAIN[block].DCORE.TRG.bit.TGKMD = tgkmd;
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_External_Startup_Mode: output block busy\n\n"));
		ret = D_IM_DISP_STATUS_ABNORMAL;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get external start-up mode.
*/
INT32 Im_DISP_Get_External_Startup_Mode(E_IM_DISP_SEL block, E_IM_DISP_TGKMD* tgkmd)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(tgkmd == NULL) {
		Ddim_Print(("E:Im_DISP_Get_External_Startup_Mode: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get TRG.TGKMD
	*tgkmd = IO_DISP.MAIN[block].DCORE.TRG.bit.TGKMD;
	im_disp_pclk_off();

	return ret;
}

/* Set external start-up operation start timing.
*/
INT32 Im_DISP_Set_External_Startup_Timing(E_IM_DISP_SEL block, ULONG tgkst)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tgkst > 0x0FFF) {
		Ddim_Print(("E:Im_DISP_Set_External_Startup_Timing: parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if(IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT){
		// Set TGKST
		IO_DISP.MAIN[block].DCORE.TGKST.word = tgkst;
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_External_Startup_Timing: output block busy\n\n"));
		ret = D_IM_DISP_STATUS_ABNORMAL;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get display interface selection.
*/
INT32 Im_DISP_Get_External_Startup_Timing(E_IM_DISP_SEL block, ULONG* tgkst)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(tgkst == NULL) {
		Ddim_Print(("E:Im_DISP_Get_External_Startup_Mode: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get TGKST
	*tgkst = IO_DISP.MAIN[block].DCORE.TGKST.bit.TGKDL;
	im_disp_pclk_off();

	return ret;
}

/* Set a flag generation timing of vertical synchronization signal.
*/
VOID Im_DISP_Set_Vsync_Int_Timing(E_IM_DISP_SEL block, E_IM_DISP_VSYNC_FLAG timing)
{
	im_disp_pclk_on();
	// Set INTC
	IO_DISP.MAIN[block].DCORE.INTC.word = timing;
	im_disp_pclk_off();
}

/* Get a flag generation timing of vertical synchronization signal.
*/
INT32 Im_DISP_Get_Vsync_Int_Timing(E_IM_DISP_SEL block, E_IM_DISP_VSYNC_FLAG *timing)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(timing == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Vsync_Int_Timing: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get INTC
	*timing = (E_IM_DISP_VSYNC_FLAG)IO_DISP.MAIN[block].DCORE.INTC.bit.VFTMG;
	im_disp_pclk_off();

	return ret;
}

/* Set interruption demand permission setting.
*/
INT32 Im_DISP_Set_Int(E_IM_DISP_SEL block, UINT32 interruption_select, E_IM_DISP_INTERRUPTION_STATE interruption_state)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((interruption_select & E_IM_DISP_CORRECT_SELECT_ALL) == 0) {
		Ddim_Print(("E:Im_DISP_Set_Int: interruption_select parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set INTE
	im_disp_set_word_bit(IO_DISP.MAIN[block].DCORE.INTE.word, interruption_select, interruption_state);
	im_disp_pclk_off();

	return ret;
}

/* Get interruption demand permission setting.
*/
INT32 Im_DISP_Get_Int(E_IM_DISP_SEL block, UINT32* interruption_state)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(interruption_state == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Int: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get INTE
	*interruption_state = IO_DISP.MAIN[block].DCORE.INTE.word;
	im_disp_pclk_off();

	return ret;
}

/* Set interruption's call-back function pointer.
*/
INT32 Im_DISP_Set_Int_Callback(E_IM_DISP_SEL block, UINT32 interruption_select, VP_CALLBACK vp_callback)
{
	INT32 index;
	INT32 cbf_index;
	INT32 array_num;
	INT32 ret = D_DDIM_OK;

	E_IM_DISP_INTERRUPTION_SELECT interrupt_index[] = {
		E_IM_DISP_INTERRUPTION_SELECT_VE2,		/**< Vertical syschronization signal interruption for 2nd field permission */
		E_IM_DISP_INTERRUPTION_SELECT_VE,		/**< Vertical syschronization signal interruption permission*/

		E_IM_DISP_INTERRUPTION_SELECT_GA1REE,	/**< GR1 A component AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GA0REE,	/**< GR0 A component AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR1REE,	/**< GR1 AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR0REE,	/**< GR0 AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_LREE,		/**< Main AXI error interruption permission */

		E_IM_DISP_INTERRUPTION_SELECT_GA1EE,	/**< GR1 A component transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GA0EE,	/**< GR0 A component transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR1EE,	/**< GR1 transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR0EE,	/**< GR0 transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_LEE,		/**< Main transmitte error interruption permission */
	};

#ifdef CO_PARAM_CHECK
	if ((interruption_select & E_IM_DISP_CORRECT_SELECT_ALL) == 0) {
		Ddim_Print(("E:Im_DISP_Set_Int_Callback: interruption_select parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set Main
	array_num = sizeof(interrupt_index) / sizeof(interrupt_index[0]);
	for(index = array_num - 1; index >= 0; index--) {
		if((interruption_select & interrupt_index[index]) == interrupt_index[index]){
			// Set callback
			cbf_index = im_disp_get_cb_index(interrupt_index[index]);
			gIM_DISP_Interrupt_Callback[block][cbf_index] = vp_callback;
		}
	}

	im_disp_pclk_off();

	return ret;
}

INT32 Im_DISP_Set_DWCH_Int_Callback(E_IM_DISP_SEL block, UINT32 dwch_interruption_select, VP_CALLBACK vp_callback)
{
	INT32 index;
	INT32 cbf_index;
	INT32 array_num;
	INT32 ret = D_DDIM_OK;

	E_IM_DISP_DWCH_INTERRUPTION_SELECT dwch_interrupt_index[] = {
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE,		/**< PWCH successful completion interrupt request permission */
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE,	/**< PWCH transmission error interrupt request permission */
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE		/**< PWCH AXI error interrupt request permission */
	};

#ifdef CO_PARAM_CHECK
	if ((dwch_interruption_select & E_IM_DISP_DWCH_CORRECT_SELECT_ALL) == 0) {
		Ddim_Print(("E:Im_DISP_Set_DWCH_Int_Callback: interruption_select parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();

	// Set DWCH
	array_num = sizeof(dwch_interrupt_index) / sizeof(dwch_interrupt_index[0]);
	for(index = array_num - 1; index >= 0; index--) {
		if((dwch_interruption_select & dwch_interrupt_index[index]) == dwch_interrupt_index[index]){
			// Set callback
			cbf_index = im_disp_get_dwch_cb_index(dwch_interrupt_index[index]);
			gIM_DISP_DWCH_Interrupt_Callback[block][cbf_index] = vp_callback;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get interruption's call-back function pointer.
*/
INT32 Im_DISP_Get_Int_Callback(E_IM_DISP_SEL block, E_IM_DISP_INTERRUPTION_SELECT interruption_select, VP_CALLBACK *vp_callback)
{
	INT32 index;
	INT32 cbf_index;
	INT32 ret = D_DDIM_OK;

	E_IM_DISP_INTERRUPTION_SELECT interrupt_select[] = {
		E_IM_DISP_INTERRUPTION_SELECT_VE2,		/**< Vertical syschronization signal interruption for 2nd field permission */
		E_IM_DISP_INTERRUPTION_SELECT_VE,		/**< Vertical syschronization signal interruption permission*/

		E_IM_DISP_INTERRUPTION_SELECT_GA1REE,	/**< GR1 A component AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GA0REE,	/**< GR0 A component AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR1REE,	/**< GR1 AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR0REE,	/**< GR0 AXI error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_LREE,		/**< Main AXI error interruption permission */

		E_IM_DISP_INTERRUPTION_SELECT_GA1EE,	/**< GR1 A component transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GA0EE,	/**< GR0 A component transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR1EE,	/**< GR1 transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_GR0EE,	/**< GR0 transmitte error interruption permission */
		E_IM_DISP_INTERRUPTION_SELECT_LEE,		/**< Main transmitte error interruption permission */
	};

#ifdef CO_PARAM_CHECK
	if (vp_callback == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Int_Callback: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((interruption_select != E_IM_DISP_INTERRUPTION_SELECT_VE2) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_VE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GA1REE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GA0REE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GR1REE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GR0REE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_LREE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GA1EE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GA0EE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GR1EE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_GR0EE) &&
		(interruption_select != E_IM_DISP_INTERRUPTION_SELECT_LEE)) {
		Ddim_Print(("E:Im_DISP_Get_Int_Callback: interruption_select parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get Main
	for(index = sizeof(interrupt_select)/sizeof(interrupt_select[0]) - 1;
		index >= 0;
		index--) {
		if(interruption_select == interrupt_select[index]) {
			cbf_index = im_disp_get_cb_index(interrupt_select[index]);
			*vp_callback = gIM_DISP_Interrupt_Callback[block][cbf_index];
			break;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get interruption's call-back function pointer.
*/
INT32 Im_DISP_Get_DWCH_Int_Callback(E_IM_DISP_SEL block, E_IM_DISP_DWCH_INTERRUPTION_SELECT dwch_interruption_select, VP_CALLBACK *vp_callback)
{
	INT32 index;
	INT32 cbf_index;
	INT32 ret = D_DDIM_OK;

	E_IM_DISP_DWCH_INTERRUPTION_SELECT dwch_interrupt_select[] = {
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE,		/**< PWCH successful completion interrupt request permission */
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE,	/**< PWCH transmission error interrupt request permission */
		E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE		/**< PWCH AXI error interrupt request permission */
	};

#ifdef CO_PARAM_CHECK
	if (vp_callback == NULL) {
		Ddim_Print(("E:Im_DISP_Get_DWCH_Int_Callback: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((dwch_interruption_select != E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE) &&
		(dwch_interruption_select != E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE) &&
		(dwch_interruption_select != E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE)) {
		Ddim_Print(("E:Im_DISP_Get_DWCH_Int_Callback: interruption_select parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get DWCH
	for(index = sizeof(dwch_interrupt_select)/sizeof(dwch_interrupt_select[0]) - 1;
		index >= 0;
		index--) {
		if(dwch_interruption_select == dwch_interrupt_select[index]) {
			cbf_index = im_disp_get_dwch_cb_index(dwch_interrupt_select[index]);
			*vp_callback = gIM_DISP_DWCH_Interrupt_Callback[block][cbf_index];
			break;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get AXI state.
*/
INT32 Im_DISP_Get_AXI_Status(E_IM_DISP_SEL block, U_IM_DISP_AXISTS *axi_state)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (axi_state == NULL) {
		Ddim_Print(("E:Im_DISP_Get_AXI_Status: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get AXISTS
	axi_state->word = IO_DISP.MAIN[block].DCORE.AXISTS.word;
	im_disp_pclk_off();

	return ret;
}

/* Set parameter reflect timing.
*/
INT32 Im_DISP_Set_Output_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG timing)
{
	INT32 ret = D_DDIM_OK;

	im_disp_pclk_on();
	if (IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) {
		// Set RPGCTL
		IO_DISP.MAIN[block].DCORE.RPGCTL.bit.RPGTMG = timing;
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Output_Parameter_Reflect_Timing: Output layer busy\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get parameter reflect timing.
*/
INT32 Im_DISP_Get_Output_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG *timing)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (timing == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Output_Parameter_Reflect_Timing: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get RPGCTL
	*timing = (E_IM_DISP_RPGTMG)IO_DISP.MAIN[block].DCORE.RPGCTL.bit.RPGTMG;
	im_disp_pclk_off();

	return ret;
}

/* Set parameter reflect enable.
*/
INT32 Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_SEL block, UINT32 enable)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((enable != D_IM_DISP_ENABLE_OFF) && (enable != D_IM_DISP_ENABLE_ON)) {
		Ddim_Print(("E:Im_DISP_Set_Output_Parameter_Reflect_Enable: enable parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set RPGEN
	IO_DISP.MAIN[block].DCORE.RPGEN.bit.RPGEN = enable;
	Im_DISP_Dsb();
	im_disp_pclk_off();

	return ret;
}

/* Get parameter reflect enable.
*/
INT32 Im_DISP_Get_Output_Parameter_Reflect_Enable(E_IM_DISP_SEL block, UINT32 *enable)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (enable == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Output_Parameter_Reflect_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get RPGEN
	*enable = IO_DISP.MAIN[block].DCORE.RPGEN.bit.RPGEN;
	Im_DISP_Dsb();
	im_disp_pclk_off();

	return ret;
}

/* Set transmission mode.
*/
INT32 Im_DISP_Set_TSL(E_IM_DISP_SEL block, BYTE tsl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((tsl != D_IM_DISP_TSL_PROGRESSIVE) && (tsl != D_IM_DISP_TSL_INTERLACE)) {
		Ddim_Print(("E:Im_DISP_Set_TSL: tsl parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	if (IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) {
		// Set TSL
		IO_DISP.MAIN[block].DCORE.TSL.word = tsl;
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_TSL: Output layer busy\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get transmission mode.
*/
INT32 Im_DISP_Get_TSL(E_IM_DISP_SEL block, BYTE *tsl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tsl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_TSL: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get TSL
	*tsl = IO_DISP.MAIN[block].DCORE.TSL.bit.TSL;
	im_disp_pclk_off();

	return ret;
}

/* Get output size.
*/
INT32 Im_DISP_Get_Output_Size(E_IM_DISP_SEL block, ULONG *ovsize, ULONG *ohsize)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ovsize == NULL) || (ohsize == NULL)){
		Ddim_Print(("E:Im_DISP_Get_Output_Size: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get OVSIZE/OHSIZE
	*ovsize = IO_DISP.MAIN[block].DCORE.OVSIZE.word;
	*ohsize = IO_DISP.MAIN[block].DCORE.OHSIZE.word;
	im_disp_pclk_off();

	return ret;
}

/* Switch force data out.
*/
VOID Im_DISP_Set_Force_Out_Data_Enable(E_IM_DISP_SEL block, E_IM_DISP_FDOEN is_enable)
{
	im_disp_pclk_on();
	// Set FDOEN
	IO_DISP.MAIN[block].DCORE.FDOEN.word = is_enable;
	im_disp_pclk_off();
}

/* Get force data out state.
*/
INT32  Im_DISP_Get_Force_Out_Data_Enable(E_IM_DISP_SEL block, E_IM_DISP_FDOEN *is_enable)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(is_enable == NULL){
		Ddim_Print(("E:Im_DISP_Get_Force_Out_Data_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get FDOEN
	*is_enable = (E_IM_DISP_FDOEN)IO_DISP.MAIN[block].DCORE.FDOEN.bit.FDEN;
	im_disp_pclk_off();

	return ret;
}

/* Set force output data.
*/
VOID Im_DISP_Set_Force_Out_Data(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR color_data)
{
	im_disp_pclk_on();
	// Set FODATA
	IO_DISP.MAIN[block].DCORE.FODATA.word = color_data.word;
	im_disp_pclk_off();
}

/* Get force output data.
*/
INT32 Im_DISP_Get_Force_Out_Data(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR *color_data)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(color_data == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Force_Out_Data: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get FODATA
	color_data->word = IO_DISP.MAIN[block].DCORE.FODATA.word;
	im_disp_pclk_off();

	return ret;
}

/* Set color bar's width.
*/
INT32 Im_DISP_Set_Color_Bar_Size(E_IM_DISP_SEL block, ULONG clbhsize)
{
	INT32 ret = D_DDIM_OK;

	im_disp_pclk_on();
#ifdef CO_PARAM_CHECK
	if(clbhsize * 16 < IO_DISP.MAIN[block].DCORE.OHSIZE.word) {
		Ddim_Print(("E:Im_DISP_Set_Color_Bar_Size: CLBHSIZE parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	// Set CLBHSIZE
	IO_DISP.MAIN[block].DCORE.CLBHSIZE.word = clbhsize;
	im_disp_pclk_off();

	return ret;
}

/* Get color bar's width.
*/
INT32 Im_DISP_Get_Color_Bar_Size(E_IM_DISP_SEL block, ULONG *clbhsize)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(clbhsize == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Color_Bar_Size: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get CLBHSIZE
	*clbhsize = IO_DISP.MAIN[block].DCORE.CLBHSIZE.word;
	im_disp_pclk_off();

	return ret;
}

/* Set color bar's color.
*/
INT32 Im_DISP_Set_Color_Bar(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR const *const clbdt, UINT32 clbdt_size)
{
	INT32 ret = D_DDIM_OK;
	UINT32 loop;

#ifdef CO_PARAM_CHECK
	if (clbdt == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Color_Bar: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (clbdt_size > D_IM_DISP_COLOR_BAR_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_Color_Bar: clbdt_size parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for(loop = 0; loop < clbdt_size; loop++) {
		// Set CLBDT
		IO_DISP.MAIN[block].DCORE.CLBDT[loop].word = clbdt[loop].word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Get color bar's color.
*/
INT32 Im_DISP_Get_Color_Bar(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR clbdt[D_IM_DISP_COLOR_BAR_COUNT])
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (clbdt == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Color_Bar: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for(loop = 0; loop < D_IM_DISP_COLOR_BAR_COUNT; loop++) {
		// Get CLBDT
		clbdt[loop].word = IO_DISP.MAIN[block].DCORE.CLBDT[loop].word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Set blend order.
*/
INT32 Im_DISP_Set_Blend(E_IM_DISP_SEL block, U_IM_DISP_BLDCTL blend)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if(im_disp_param_check_blend(blend) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Blend: BLDCTL parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set BLDCTL
	IO_DISP.MAIN[block].DCORE.BLDCTL.word = blend.word;
	im_disp_pclk_off();

	return ret;
}

/* Get blend order.
*/
INT32 Im_DISP_Get_Blend(E_IM_DISP_SEL block, U_IM_DISP_BLDCTL *blend)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (blend == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Blend: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get BLDCTL
	blend->word = IO_DISP.MAIN[block].DCORE.BLDCTL.word;
	im_disp_pclk_off();

	return ret;
}

/* Set RGB->YCbCr or YCbCr->RGB matrix data.
*/
INT32 Im_DISP_Set_Output_Matrix(E_IM_DISP_SEL block, E_IM_DISP_MATRIX_KIND kind, const U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE])
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (matrix == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Output_Matrix: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (kind == E_IM_DISP_MATRIX_KIND_CC) {
		Ddim_Print(("E:Im_DISP_Set_Output_Matrix: kind parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	if (kind == E_IM_DISP_MATRIX_KIND_R2Y) {
		for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			// Set R2Y0~2
			IO_DISP.MAIN[block].DCORE.R2Y[loop].dword = matrix[loop].dword;
		}
	}
	else if (kind == E_IM_DISP_MATRIX_KIND_Y2R) {
		for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			// Set Y2R0~2
			IO_DISP.MAIN[block].DCORE.Y2R[loop].dword = matrix[loop].dword;
		}
	}
	else {
		// no processing.
	}
	im_disp_pclk_off();

	return ret;
}

/* Get RGB to RGB matrix data.
*/
INT32 Im_DISP_Get_Output_Matrix(E_IM_DISP_SEL block, E_IM_DISP_MATRIX_KIND kind, U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE])
{
	INT32 ret = D_DDIM_OK;
	INT32 loop;

#ifdef CO_PARAM_CHECK
	if (matrix == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Output_Matrix: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (kind == E_IM_DISP_MATRIX_KIND_CC) {
		Ddim_Print(("E:Im_DISP_Get_Output_Matrix: kind parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	if (kind == E_IM_DISP_MATRIX_KIND_R2Y) {
		for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			// Get R2Y0~2
			matrix[loop].dword = IO_DISP.MAIN[block].DCORE.R2Y[loop].dword;
		}
	}
	else if (kind == E_IM_DISP_MATRIX_KIND_Y2R) {
		for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
			// Get R2Y0~2
			matrix[loop].dword = IO_DISP.MAIN[block].DCORE.Y2R[loop].dword;
		}
	}
	else {
		// no processing.
	}
	im_disp_pclk_off();

	return ret;
}

/* Set clip/gain/offset.
*/
INT32 Im_DISP_Set_Clip(E_IM_DISP_SEL block, T_IM_DISP_CLIP_CAL const *const clip_cal)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (clip_cal == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Clip: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_clip(clip_cal) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Clip: YCAL/YCLIP/CBCAL/CBCLIP/CRCAL/CRCLIP parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set YCAL/YCLIP/CBCAL/CBCLIP/CRCAL/CRCLIP
	IO_DISP.MAIN[block].DCORE.YCAL.bit.YGAIN = clip_cal->y_cal.ygain;
	IO_DISP.MAIN[block].DCORE.YCAL.bit.YOFS = clip_cal->y_cal.yofs;
	IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPH = clip_cal->y_clip.cph;
	IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPL = clip_cal->y_clip.cpl;

	IO_DISP.MAIN[block].DCORE.CBCAL.bit.CBGAIN = clip_cal->cb_cal.cgain;
	im_disp_set_reg_signed(IO_DISP.MAIN[block].DCORE.CBCAL, union io_jdsdcore_reg_cbcal, CBOFS, clip_cal->cb_cal.cofs);
	IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPH = clip_cal->cb_clip.cph;
	IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPL = clip_cal->cb_clip.cpl;

	IO_DISP.MAIN[block].DCORE.CRCAL.bit.CRGAIN = clip_cal->cr_cal.cgain;
	im_disp_set_reg_signed(IO_DISP.MAIN[block].DCORE.CRCAL, union io_jdsdcore_reg_crcal, CROFS, clip_cal->cr_cal.cofs);
	IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPH = clip_cal->cr_clip.cph;
	IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPL = clip_cal->cr_clip.cpl;
	im_disp_pclk_off();

	return ret;
}

/* Get clip/gain/offset.
*/
INT32 Im_DISP_Get_Clip(E_IM_DISP_SEL block, T_IM_DISP_CLIP_CAL *const clip_cal)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (clip_cal == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Clip: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get YCAL/YCLIP/CBCAL/CBCLIP/CRCAL/CRCLIP
	clip_cal->y_cal.ygain = IO_DISP.MAIN[block].DCORE.YCAL.bit.YGAIN;
	clip_cal->y_cal.yofs = IO_DISP.MAIN[block].DCORE.YCAL.bit.YOFS;
	clip_cal->y_clip.cph = IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPH;
	clip_cal->y_clip.cpl = IO_DISP.MAIN[block].DCORE.YCLIP.bit.YCPL;

	clip_cal->cb_cal.cgain = IO_DISP.MAIN[block].DCORE.CBCAL.bit.CBGAIN;
	im_disp_get_reg_signed(clip_cal->cb_cal.cofs, IO_DISP.MAIN[block].DCORE.CBCAL, union io_jdsdcore_reg_cbcal, CBOFS);
	clip_cal->cb_clip.cph = IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPH;
	clip_cal->cb_clip.cpl = IO_DISP.MAIN[block].DCORE.CBCLIP.bit.CBCPL;

	clip_cal->cr_cal.cgain = IO_DISP.MAIN[block].DCORE.CRCAL.bit.CRGAIN;
	im_disp_get_reg_signed(clip_cal->cr_cal.cofs, IO_DISP.MAIN[block].DCORE.CRCAL, union io_jdsdcore_reg_crcal, CROFS);
	clip_cal->cr_clip.cph = IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPH;
	clip_cal->cr_clip.cpl = IO_DISP.MAIN[block].DCORE.CRCLIP.bit.CRCPL;
	im_disp_pclk_off();

	return ret;
}

/* Set RGB/YCbCr's output order.
*/
VOID Im_DISP_Set_Output_Order(E_IM_DISP_SEL block, U_IM_DISP_DOCTL0 doctl0)
{
	im_disp_pclk_on();
	// Set DOCTL0
	IO_DISP.MAIN[block].DCORE.DOCTL0.word = doctl0.word;
	im_disp_pclk_off();
}

/* Get RGB/YCbCr's output order.
*/
INT32 Im_DISP_Get_Output_Order(E_IM_DISP_SEL block, U_IM_DISP_DOCTL0 *doctl0)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (doctl0 == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Output_Order: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get DOCTL0
	doctl0->word = IO_DISP.MAIN[block].DCORE.DOCTL0.word;
	im_disp_pclk_off();

	return ret;
}

/* Set grid data.
*/
INT32 Im_DISP_Set_Grid(E_IM_DISP_SEL block, T_IM_DISP_GRID const *const grid)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (grid == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Grid: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if(im_disp_param_check_grid(grid) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_Grid: GHDSTA/GVDSTA/GLENGTH/GWIDTH/GITVL parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set GHDSTA/GVDSTA/GLENGTH/GWIDTH/GITVL/GNUM/GDCTL
	IO_DISP.MAIN[block].DCORE.GHDSTA.word = grid->ghdsta.word;
	IO_DISP.MAIN[block].DCORE.GVDSTA.word = grid->gvdsta.word;
	IO_DISP.MAIN[block].DCORE.GLENGTH.word = grid->glength.word;
	IO_DISP.MAIN[block].DCORE.GWIDTH.word = grid->gwidth.word;
	IO_DISP.MAIN[block].DCORE.GITVL.word = grid->gitvl.word;
	IO_DISP.MAIN[block].DCORE.GNUM.word = grid->gnum.word;
	IO_DISP.MAIN[block].DCORE.GDCTL.word = grid->gdctl.word;
	im_disp_pclk_off();

	return ret;
}

/* Get grid data.
*/
INT32 Im_DISP_Get_Grid(E_IM_DISP_SEL block, T_IM_DISP_GRID *const grid)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (grid == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Grid: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get GHDSTA/GVDSTA/GLENGTH/GWIDTH/GITVL/GNUM/GDCTL
	grid->ghdsta.word = IO_DISP.MAIN[block].DCORE.GHDSTA.word;
	grid->gvdsta.word = IO_DISP.MAIN[block].DCORE.GVDSTA.word;
	grid->glength.word = IO_DISP.MAIN[block].DCORE.GLENGTH.word;
	grid->gwidth.word = IO_DISP.MAIN[block].DCORE.GWIDTH.word;
	grid->gitvl.word = IO_DISP.MAIN[block].DCORE.GITVL.word;
	grid->gnum.word = IO_DISP.MAIN[block].DCORE.GNUM.word;
	grid->gdctl.word = IO_DISP.MAIN[block].DCORE.GDCTL.word;
	im_disp_pclk_off();

	return ret;
}

/* Set grid line display enable.
*/
VOID Im_DISP_Set_Grid_Enable(E_IM_DISP_SEL block, E_IM_DISP_GDISPEN gdispen)
{
	im_disp_pclk_on();
	// Set GDISPEN
	IO_DISP.MAIN[block].DCORE.GDISPEN.word = gdispen;
	im_disp_pclk_off();
}

/* Get grid line display enable.
*/
INT32 Im_DISP_Get_Grid_Enable(E_IM_DISP_SEL block, E_IM_DISP_GDISPEN *gdispen)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (gdispen == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Grid_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get GDISPEN
	*gdispen = (E_IM_DISP_GDISPEN)IO_DISP.MAIN[block].DCORE.GDISPEN.bit.GDEN;
	im_disp_pclk_off();

	return ret;
}

/* Set face frame data.
*/
INT32 Im_DISP_Set_Face_Frame(E_IM_DISP_SEL block, UINT32 frame_count, T_IM_DISP_FACE_FRAME const *const face)
{
	INT32 ret = D_DDIM_OK;
	INT32 loop;

#ifdef CO_PARAM_CHECK
	if (face == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Face_Frame: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (frame_count > D_IM_DISP_FACE_FRAME_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_Face_Frame: frame_count parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	for(loop = frame_count - 1; loop >= 0 ; loop--) {
		if (im_disp_param_check_face_frame(&(face->param[loop])) != D_DDIM_OK) {
			Ddim_Print(("E:Im_DISP_Set_Face_Frame: FFDSTA/FFSIZE/FFWIDTH parameter error\n"));
			return D_IM_DISP_INPUT_PARAM_ERROR;
		}
	}
	if (im_disp_param_check_top_face(face->msff) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Face_Frame: FFDISPEN.MSFF parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set FFDSTA/FFSIZE/FFWIDTH/FFCLR
	for(loop = frame_count - 1; loop >= 0 ; loop--) {
		IO_DISP.MAIN[block].DCORE.FFDSTA[loop].word = face->param[loop].ffdsta.word;
		IO_DISP.MAIN[block].DCORE.FFSIZE[loop].word = face->param[loop].ffsize.word;
		IO_DISP.MAIN[block].DCORE.FFWIDTH[loop].word = face->param[loop].ffwidth.word;
		IO_DISP.MAIN[block].DCORE.FFCLR[loop].word = face->param[loop].ffclr.word;
	}
	// Set FFDISPEN/FFDO
	IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN = face->ffden;
	IO_DISP.MAIN[block].DCORE.FFDO.bit.MSFF = face->msff;
	im_disp_pclk_off();

	return ret;
}

/* Get face frame data.
*/
INT32 Im_DISP_Get_Face_Frame(E_IM_DISP_SEL block, T_IM_DISP_FACE_FRAME *const face)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (face == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Face_Frame: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get FFDSTA/FFSIZE/FFWIDTH/FFCLR/FFDISPEN
	for (loop = D_IM_DISP_FACE_FRAME_COUNT - 1; loop >= 0; loop--) {
		face->param[loop].ffdsta.word = IO_DISP.MAIN[block].DCORE.FFDSTA[loop].word;
		face->param[loop].ffsize.word = IO_DISP.MAIN[block].DCORE.FFSIZE[loop].word;
		face->param[loop].ffwidth.word = IO_DISP.MAIN[block].DCORE.FFWIDTH[loop].word;
		face->param[loop].ffclr.word = IO_DISP.MAIN[block].DCORE.FFCLR[loop].word;
	}
	// Get FFDISPEN/FFDO
	face->ffden = IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN;
	face->msff = IO_DISP.MAIN[block].DCORE.FFDO.bit.MSFF;
	im_disp_pclk_off();

	return ret;
}

/* Set face frame display enable.
*/
INT32 Im_DISP_Set_Face_Frame_Enable(E_IM_DISP_SEL block, UINT64 ffden, BYTE msff)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (im_disp_param_check_top_face(msff) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_Face_Frame_Enable: FFDISPEN.MSFF parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set FFDISPEN.FFDEN
	IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN = ffden;
	// Set FFDO.MSFF
	IO_DISP.MAIN[block].DCORE.FFDO.bit.MSFF = msff;
	im_disp_pclk_off();

	return ret;
}

/* Get face frame display enable.
*/
INT32 Im_DISP_Get_Face_Frame_Enable(E_IM_DISP_SEL block, UINT64 *ffden, BYTE *msff)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ffden == NULL) || (msff == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Face_Frame_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get FFDISPEN.FFDEN
	*ffden = IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN;
	// Get FFDO.MSFF
	*msff = IO_DISP.MAIN[block].DCORE.FFDO.bit.MSFF;
	im_disp_pclk_off();

	return ret;
}

/* Set parameter reflect timing of a OSD layer.
*/
INT32 Im_DISP_Set_OSD_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG timing)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Parameter_Reflect_Timing: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// check trigger
			if (IO_DISP.MAIN[block].GRCH[loop].GRTRG.word == E_IM_DISP_TRG_READ_NO_ACT) {
				// Set GRRPGCTL
				IO_DISP.MAIN[block].GRCH[loop].GRRPGCTL.bit.RPGTMG = timing;
			}
			else {
				Ddim_Print(("E:Im_DISP_Set_OSD_Parameter_Reflect_Timing: OSD-%d layer busy\n", loop));
				ret = D_IM_DISP_MACRO_BUSY_NG;
			}
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get parameter reflect timing of a OSD layer.
*/
INT32 Im_DISP_Get_OSD_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG *timing)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (timing == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Parameter_Reflect_Timing: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Parameter_Reflect_Timing: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get RPGTMG
	*timing = (E_IM_DISP_RPGTMG)IO_DISP.MAIN[block].GRCH[layer_index].GRRPGCTL.bit.RPGTMG;
	im_disp_pclk_off();

	return ret;
}

/* Set input data transfer of a OSD layer.
*/
INT32 Im_DISP_Set_OSD_Input_Data_Transfer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRIDT gridt)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Input_Data_Transfer: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_gridt(gridt) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_OSD_Input_Data_Transfer: GRIDT parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRIDT
			IO_DISP.MAIN[block].GRCH[loop].GRIDT.word = gridt.word;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get input data transfer of a OSD layer.
*/
INT32 Im_DISP_Get_OSD_Input_Data_Transfer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRIDT *gridt)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (gridt == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Input_Data_Transfer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Input_Data_Transfer: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRIDT
	gridt->word = IO_DISP.MAIN[block].GRCH[layer_index].GRIDT.word;
	im_disp_pclk_off();

	return ret;
}

/* Set OSD data territory size.
*/
INT32 Im_DISP_Set_OSD_Territory_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_SIZE territory_size)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Territory_Size: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grtisize(territory_size) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_OSD_Territory_Size: GRTISIZE parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRTISIZE
			IO_DISP.MAIN[block].GRCH[loop].GRTISIZE.word = territory_size.word;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get OSD data territory size.
*/
INT32 Im_DISP_Get_OSD_Territory_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_SIZE *territory_size)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (territory_size == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Territory_Size: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Territory_Size: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}
	
	// Get GRTISIZE
	territory_size->word = IO_DISP.MAIN[block].GRCH[layer_index].GRTISIZE.word;
	im_disp_pclk_off();

	return ret;
}

/* Set display position for OSD.
*/
INT32 Im_DISP_Set_OSD_Territory_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_DSTA position)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Territory_Position: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grtdsta(position) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Get_OSD_Territory_Position: GRTDSTA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRTDSTA
			IO_DISP.MAIN[block].GRCH[loop].GRTDSTA.word = position.word;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get display position for OSD.
*/
INT32 Im_DISP_Get_OSD_Territory_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_DSTA* position)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (position == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Territory_Position: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Territory_Position: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRTDSTA
	position->word = IO_DISP.MAIN[block].GRCH[layer_index].GRTDSTA.word;
	im_disp_pclk_off();

	return ret;
}

/* Set input data's order.
*/
INT32 Im_DISP_Set_OSD_IPO(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer,  U_IM_DISP_GRIPO ipo)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Set_OSD_IPO: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_gripo(ipo) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_OSD_IPO: GRIPO parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Set GRIPO
	IO_DISP.MAIN[block].GRCH[layer_index].GRIPO.word = ipo.word;
	im_disp_pclk_off();

	return ret;
}

/* Get input data's order.
*/
INT32 Im_DISP_Get_OSD_IPO(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer,  U_IM_DISP_GRIPO* ipo)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (ipo == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_IPO: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_IPO: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRIPO
	ipo->word = IO_DISP.MAIN[block].GRCH[layer_index].GRIPO.word;
	im_disp_pclk_off();

	return ret;
}

/* Set input data area to access.
*/
INT32 Im_DISP_Set_OSD_Area0_Switch(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_INPUT_DATA_AREA area)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area0_Switch: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRSCCTL.IDSET
			IO_DISP.MAIN[block].GRCH[loop].GRSCCTL.bit.IDSET = area;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get input data area to access.
*/
INT32 Im_DISP_Get_OSD_Area0_Switch(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_INPUT_DATA_AREA *area)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (area == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area0_Switch: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area0_Switch: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRSCCTL.IDSET
	*area = (E_IM_DISP_INPUT_DATA_AREA)IO_DISP.MAIN[block].GRCH[layer_index].GRSCCTL.bit.IDSET;
	im_disp_pclk_off();

	return ret;
}

/* Get input data area inder processing.
*/
INT32 Im_DISP_Get_OSD_Area0_Monitor(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_INPUT_DATA_AREA *area)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (area == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area0_Monitor: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area0_Monitor: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRSCCTL.IDM
	*area = (E_IM_DISP_INPUT_DATA_AREA)IO_DISP.MAIN[block].GRCH[layer_index].GRSCCTL.bit.IDM;
	im_disp_pclk_off();

	return ret;
}

/* Set input data size of OSD.
*/
INT32 Im_DISP_Set_OSD_Area_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, 
								U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT],
								UINT32 input_size_count)
{
	INT32 loop1;
	INT32 loop2;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (input_size == NULL) {
		 Ddim_Print(("E:Im_DISP_Set_OSD_Area_Size: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Size: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (input_size_count > D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Size: input_size_count parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grisize(input_size, input_size_count) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Size: input_size parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop1 = 0; loop1 < 2; loop1++) {
		if ((layer & osd_layer_id[loop1]) == osd_layer_id[loop1]) {
			// Set GRISIZE
			for (loop2 = 0; loop2 <input_size_count; loop2++) {
				IO_DISP.MAIN[block].GRCH[loop1].GRISIZE[loop2].word = input_size[loop2].word;
			}
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get input data size of OSD.
*/
INT32 Im_DISP_Get_OSD_Area_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT])
{
	INT32 layer_index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (input_size == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Size: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Size: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRISIZE
	for (loop = D_IM_DISP_OSD_DISPLAY_AREA_COUNT - 1; loop >= 0; loop--) {
		input_size[loop].word = IO_DISP.MAIN[block].GRCH[layer_index].GRISIZE[loop].word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Connect OSD input data address with the bank index.
*/
INT32 Im_DISP_Set_OSD_Area_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_OSD_SA_BANK_NO bank_no, ULONG address)
{
	INT32 index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Addr: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grsa(address) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Addr: GRSA0/GRSA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			if (bank_no < E_IM_DISP_OSD_SA_BANK_NO_1) {
				// Set GRSA0
				IO_DISP.MAIN[block].GRCH[loop].GRSA0[bank_no].word = address;
			}
			else {
				index = bank_no - E_IM_DISP_OSD_SA_BANK_NO_1;
				// Set GRSA
				IO_DISP.MAIN[block].GRCH[loop].GRSA[index].word = address;
			}
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get OSD input data address of the bank index specified.
*/
INT32 Im_DISP_Get_OSD_Area_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_OSD_SA_BANK_NO bank_no, ULONG *address)
{
	INT32 layer_index;
	INT32 index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (address == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Addr: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Addr: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRIDT
	if (bank_no < E_IM_DISP_OSD_SA_BANK_NO_1) {
		// GRSA0
		*address = IO_DISP.MAIN[block].GRCH[layer_index].GRSA0[bank_no].word;
	}
	else {
		index = bank_no - E_IM_DISP_OSD_SA_BANK_NO_1;
		// GRSA
		*address = IO_DISP.MAIN[block].GRCH[layer_index].GRSA[index].word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Connect OSD input A data address with the bank index.
*/
INT32 Im_DISP_Set_OSD_Area_Adata_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG address)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Adata_Addr: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Adata_Addr: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grsa(address) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Adata_Addr: GRASA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRASA
			IO_DISP.MAIN[block].GRCH[loop].GRASA[bank_no].word = address;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get OSD input A data address of the bank index specified.
*/
INT32 Im_DISP_Get_OSD_Area_Adata_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG *address)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (address == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Adata_Addr: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Adata_Addr: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Adata_Addr: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRASA
	*address = IO_DISP.MAIN[block].GRCH[layer_index].GRASA[bank_no].word;
	im_disp_pclk_off();

	return ret;
}

/* Set the horizontal global area width.
*/
INT32 Im_DISP_Set_OSD_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG width)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Lfd: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Lfd: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grhga(width) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Lfd: GRHGA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRHGA
			IO_DISP.MAIN[block].GRCH[loop].GRHGA[bank_no].word = width;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get the horizontal global area width.
*/
INT32 Im_DISP_Get_OSD_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG *width)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (width == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Lfd: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Lfd: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Lfd: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRHGA
	*width = IO_DISP.MAIN[block].GRCH[layer_index].GRHGA[bank_no].word;
	im_disp_pclk_off();

	return ret;
}

/* Set the A data horizontal global area width.
*/
INT32 Im_DISP_Set_OSD_Adata_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG width)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Adata_Lfd: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Adata_Lfd: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grhga(width) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Adata_Lfd: GRAHGA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRAHGA
			IO_DISP.MAIN[block].GRCH[loop].GRAHGA[bank_no].word = width;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get the A data horizontal global area width.
*/
INT32 Im_DISP_Get_OSD_Adata_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG *width)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (width == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Adata_Lfd: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Adata_Lfd: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Adata_Lfd: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRAHGA
	*width = IO_DISP.MAIN[block].GRCH[layer_index].GRAHGA[bank_no].word;
	im_disp_pclk_off();

	return ret;
}

/* Set input start position.
*/
INT32 Im_DISP_Set_OSD_Display_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, U_IM_DISP_DSTA position)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Display_Position: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Display_Position: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_grdsta(position) != D_DDIM_OK) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Display_Position: GRDSTA parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRDSTA
			IO_DISP.MAIN[block].GRCH[loop].GRDSTA[bank_no].word = position.word;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get input start position.
*/
INT32 Im_DISP_Get_OSD_Display_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, U_IM_DISP_DSTA* position)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (position == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Display_Position: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (bank_no >= D_IM_DISP_OSD_DISPLAY_AREA_COUNT) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Display_Position: bank_no parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Display_Position: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRDSTA
	position->word = IO_DISP.MAIN[block].GRCH[layer_index].GRDSTA[bank_no].word;
	im_disp_pclk_off();

	return ret;
}

/* Set GR's horizontal rectangle to showing or hiden.
*/
INT32 Im_DISP_Set_OSD_Area_Enable(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG area_enable)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Area_Enable: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRAREN
			IO_DISP.MAIN[block].GRCH[loop].GRAREN.word = area_enable;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get GR's horizontal rectangle to showing or hiden.
*/
INT32 Im_DISP_Get_OSD_Area_Enable(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG *area_enable)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (area_enable == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Enable: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Area_Enable: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRAREN
	*area_enable = IO_DISP.MAIN[block].GRCH[layer_index].GRAREN.bit.AREN;
	im_disp_pclk_off();

	return ret;
}

/* Set blink method selection.
*/
INT32 Im_DISP_Set_OSD_Blink(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG blink)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Blink: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRBSL
			IO_DISP.MAIN[block].GRCH[loop].GRBSL.word = blink;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get blink method selection.
*/
INT32 Im_DISP_Get_OSD_Blink(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG *blink)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (blink == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Blink: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Blink: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRBSL
	*blink = IO_DISP.MAIN[block].GRCH[layer_index].GRBSL.bit.BSL;
	im_disp_pclk_off();

	return ret;
}

/* Set GR's blink period.
*/
INT32 Im_DISP_Set_OSD_Blink_Timer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRBLINK grblink)
{
	INT32 layer_index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Blink_Timer: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (layer_index = 0; layer_index < 2; layer_index++) {
		if ((layer & osd_layer_id[layer_index]) == osd_layer_id[layer_index]) {
			// Set GRBLINK
			for(loop = D_IM_DISP_GRBLINK_SIZE - 1; loop >= 0; loop--) {
				IO_DISP.MAIN[block].GRCH[layer_index].GRBLINK.word[loop] = grblink.word[loop];
			}
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get GR's blink period.
*/
INT32 Im_DISP_Get_OSD_Blink_Timer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRBLINK *grblink)
{
	INT32 layer_index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (grblink == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Blink_Timer: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Blink_Timer: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRBLINK
	for(loop = D_IM_DISP_GRBLINK_SIZE - 1; loop >= 0; loop--) {
		grblink->word[loop] = IO_DISP.MAIN[block].GRCH[layer_index].GRBLINK.word[loop];
	}
	im_disp_pclk_off();

	return ret;
}

/* Set GR's blink sync.
*/
INT32 Im_DISP_Set_OSD_Blink_Sync(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG grbinit, ULONG grbitrg)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Blink_Sync: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (grbinit > 0x03FF) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Blink_Sync: parameter error (GRBINIT)\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((grbitrg != D_IM_DISP_BITRG_NON) &&
		(grbitrg != D_IM_DISP_BITRG_WRITE_SYNC_WAIT) &&
		(grbitrg != D_IM_DISP_BITRG_READ_STOP) &&
		(grbitrg != D_IM_DISP_BITRG_READ_SYNC_WAIT)) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Blink_Sync: parameter error (GRBITRG)\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (layer_index = 0; layer_index < 2; layer_index++) {
		if ((layer & osd_layer_id[layer_index]) == osd_layer_id[layer_index]) {
			// Set GRBINIT
			IO_DISP.MAIN[block].GRCH[layer_index].GRBINIT.bit.BINIT = grbinit;
			
			// Set GRBITRG
			if ((grbitrg == D_IM_DISP_BITRG_WRITE_SYNC_WAIT) &&
				(IO_DISP.MAIN[block].GRCH[layer_index].GRBITRG.bit.BITRG != D_IM_DISP_BITRG_READ_SYNC_WAIT))
			{
				IO_DISP.MAIN[block].GRCH[layer_index].GRBITRG.bit.BITRG = grbitrg;
			}
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get GR's blink sync.
*/
INT32 Im_DISP_Get_OSD_Blink_Sync(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG* grbinit, ULONG* grbitrg)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((grbinit == NULL) || (grbitrg == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Blink_Sync: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Blink_Sync: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}
	// Get GRBINIT/GRBITRG
	*grbinit = IO_DISP.MAIN[block].GRCH[layer_index].GRBINIT.bit.BINIT;
	*grbitrg = IO_DISP.MAIN[block].GRCH[layer_index].GRBITRG.bit.BITRG;
	im_disp_pclk_off();

	return ret;
}

/* Set resize value.
*/
INT32 Im_DISP_Set_OSD_Resize(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, T_IM_DISP_RESIZE const *const resize)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (resize == NULL) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Resize: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Resize: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_resize(resize->rsz0, resize->rsz1, resize->rsz2, resize->rsz3) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Set_OSD_Resize: GRHRSZ0/GRHRSZ1/GRVRSZ parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRRSZ0/GRRSZ1/GRRSZ2/GRRSZ3
			IO_DISP.MAIN[block].GRCH[loop].GRRSZ0.word = resize->rsz0;
			IO_DISP.MAIN[block].GRCH[loop].GRRSZ1.word = resize->rsz1.word;
			IO_DISP.MAIN[block].GRCH[loop].GRRSZ2.word = resize->rsz2.word;
			IO_DISP.MAIN[block].GRCH[loop].GRRSZ3.word = resize->rsz3.word;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get resize value.
*/
INT32 Im_DISP_Get_OSD_Resize(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, T_IM_DISP_RESIZE *const resize)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (resize == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Resize: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Resize: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRRSZ0/GRRSZ1/GRRSZ2/GRRSZ3
	resize->rsz0 = (E_IM_DISP_RSZSL)IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ0.bit.RSZSL;
	resize->rsz1.word = IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ1.word;
	resize->rsz2.word = IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ2.word;
	resize->rsz3.word = IO_DISP.MAIN[block].GRCH[layer_index].GRRSZ3.word;
	im_disp_pclk_off();

	return ret;
}

/* Set YCbCr -> RGB matrix data.
*/
INT32 Im_DISP_Set_OSD_Matrix(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRY2R gry2r[D_IM_DISP_MATRIX_SIZE])
{
	INT32 loop1;
	INT32 loop2;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (gry2r == NULL) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Matrix: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Matrix: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop1 = 0; loop1 < 2; loop1++) {
		if ((layer & osd_layer_id[loop1]) == osd_layer_id[loop1]) {
			// Set GRY2R0~2
			for (loop2 = D_IM_DISP_MATRIX_SIZE - 1; loop2 >= 0; loop2--) {
				IO_DISP.MAIN[block].GRCH[loop1].GRY2R[loop2].word = gry2r[loop2].word;
			}
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get YCbCr -> RGB matrix data.
*/
INT32 Im_DISP_Get_OSD_Matrix(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRY2R gry2r[D_IM_DISP_MATRIX_SIZE])
{
	INT32 layer_index;
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (gry2r == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Matrix: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Matrix: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}
	
	// Set GRY2R0~2
	for (loop = D_IM_DISP_MATRIX_SIZE - 1; loop >= 0; loop--) {
		gry2r[loop].word = IO_DISP.MAIN[block].GRCH[layer_index].GRY2R[loop].word;
	}
	im_disp_pclk_off();

	return ret;
}

/* Set GR's Alpha.
*/
INT32 Im_DISP_Set_OSD_Alpha(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG alpha)
{
	INT32 loop;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_OSD_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Set_OSD_Alpha: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	for (loop = 0; loop < 2; loop++) {
		if ((layer & osd_layer_id[loop]) == osd_layer_id[loop]) {
			// Set GRALP
			IO_DISP.MAIN[block].GRCH[loop].GRALP.word = alpha;
		}
	}
	im_disp_pclk_off();

	return ret;
}

/* Get GR's Alpha.
*/
INT32 Im_DISP_Get_OSD_Alpha(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG* alpha)
{
	INT32 layer_index;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (alpha == NULL) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Alpha: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((layer != E_IM_DISP_SEL_LAYER_OSD_0) && (layer != E_IM_DISP_SEL_LAYER_OSD_1)) {
		Ddim_Print(("E:Im_DISP_Get_OSD_Alpha: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set layer index
	if (layer == E_IM_DISP_SEL_LAYER_OSD_0) {
		layer_index = 0;
	}
	else {
		layer_index = 1;
	}

	// Get GRALP
	*alpha = IO_DISP.MAIN[block].GRCH[layer_index].GRALP.word;
	im_disp_pclk_off();

	return ret;
}

/* Set Anti-gamma table.
*/
INT32 Im_DISP_Set_Anti_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_ANTI_GAMMA_TBL const *const tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
 		Ddim_Print(("E:Im_DISP_Set_Anti_Gamma_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Set_Anti_Gamma_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL != surface)) {
		// Main data input block is stopped, or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
			
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Set Anti gamma table (surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLA)),
										E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		else {
			// Set Anti gamma table (surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLB)),
										E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Anti_Gamma_Table: Main layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return  ret;
}

/* Get Anti-gamma table.
*/
INT32 Im_DISP_Get_Anti_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_ANTI_GAMMA_TBL *const tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Anti_Gamma_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Get_Anti_Gamma_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].LCH.LTBLASET.bit.IGTSL != surface)) {
		// Main data input block is stopped, or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
			
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Get Anti gamma table (surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLA)),
										E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		else {
			// Get Anti gamma table (surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.IGTBLB)),
										E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Get_Anti_Gamma_Table: Main layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return ret;
}

/* Set Gamma table (Main data input block).
*/
INT32 Im_DISP_Set_Main_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_IN const *const tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Main_Gamma_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Set_Main_Gamma_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL != surface)) {
		// Main data input block is stopped, or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Set Gamma table (Main data input block, surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLA)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		else {
			// Set Gamma table (Main data input block, surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLB)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Main_Gamma_Table: Main layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return  ret;
}

/* Get Gamma table (Main data input block).
*/
INT32 Im_DISP_Get_Main_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_IN *const tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Main_Gamma_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Get_Main_Gamma_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].LCH.LTRG.word == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].LCH.LTBLASET.bit.GMTSL != surface)) {
		// Main data input block is stopped, or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Get Gamma table (Main data input block, surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLA)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		else {
			// Get Gamma table (Main data input block, surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].LCH.GTBLB)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_IN, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Get_Main_Gamma_Table: Main layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return ret;
}

/* Set Gamma table (Data output block).
*/
INT32 Im_DISP_Set_Dcore_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_OUT const *const tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Dcore_Gamma_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Set_Dcore_Gamma_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL != surface)) {
		// Data output block is stopped, or or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Set Gamma table (Data output block, surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLA)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		else {
			// Set Gamma table (Data output block, surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLB)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Dcore_Gamma_Table: Output layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return  ret;
}

/* Get Gamma table (Data output block).
*/
INT32 Im_DISP_Get_Dcore_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_OUT *const tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Dcore_Gamma_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Get_Dcore_Gamma_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].DCORE.TBLASET.bit.GMTSL != surface)) {
		// Data output block is stopped, or or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Get Gamma table (Data output block, surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLA)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		else {
			// Get Gamma table (Data output block, surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.GTBLB)),
										E_IM_DISP_CORRECT_SELECT_GAMMA_OUT, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Get_Dcore_Gamma_Table: Output layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return ret;
}

/* Set Luminance correct table.
*/
INT32 Im_DISP_Set_Luminance_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Luminance_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Set_Luminance_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL != surface)) {
		// Data output block is stopped, or or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Set Luminance table (surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLAYT)),
										E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		else {
			// Set Luminance table (surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBYT)),
										E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Luminance_Table: Output layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return  ret;
}

/* Get Luminance correct table.
*/
INT32 Im_DISP_Get_Luminance_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Luminance_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Get_Luminance_Table: Parameter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL != surface)) {
		// Data output block is stopped, or or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Get Luminance table (surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLAYT)),
										E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		else {
			// Get Luminance table (surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBYT)),
										E_IM_DISP_CORRECT_SELECT_LUMINANCE, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Luminance_Table: Output layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();
	
	return ret;
}

/* Set Chroma correct gain table.
*/
INT32 Im_DISP_Set_Chroma_Gain_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Set_Chroma_Gain_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Set_Chroma_Gain_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL != surface)) {
		// Data output block is stopped, or or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Set Chroma gain table (surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLACG)),
										E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		else {
			// Set Chroma gain table (surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBCG)),
										E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_SET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Set_Chroma_Gain_Table: Output layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return  ret;
}

/* Get Chroma correct gain table.
*/
INT32 Im_DISP_Get_Chroma_Gain_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (tbl == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Chroma_Gain_Table: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	
	if ((surface != D_IM_DISP_TABLE_SURFACE_A) &&
		(surface != D_IM_DISP_TABLE_SURFACE_B)) {
		Ddim_Print(("E:Im_DISP_Get_Chroma_Gain_Table: Paramter check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// check trigger.
	if ((IO_DISP.MAIN[block].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT) ||
		(IO_DISP.MAIN[block].DCORE.TBLASET.bit.YSATSL != surface)) {
		// Data output block is stopped, or or specified surface is unused.
		
		DDIM_User_AhbReg_SpinLock();
		im_disp_hclk_on();
		
		// check table surface.
		if (surface == D_IM_DISP_TABLE_SURFACE_A) {
			// Get Chroma gain table (surface A)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLACG)),
										E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		else {
			// Get Chroma gain table (surface B)
			im_disp_access_gamma_table((BYTE*)(tbl), (BYTE*)(&(IO_DISP_TBL.MAIN[block].DCORE.YSTBLBCG)),
										E_IM_DISP_CORRECT_SELECT_CHROMA, E_IM_DISP_ACCESS_GAMMA_GET);
		}
		
		im_disp_hclk_off();
		DDIM_User_AhbReg_SpinUnLock();
	}
	else {
		Ddim_Print(("E:Im_DISP_Get_Chroma_Gain_Table: Output layer busy and specified surface is being used\n"));
		ret = D_IM_DISP_MACRO_BUSY_NG;
	}
	
	im_disp_pclk_off();

	return  ret;
}

/* Set MIPI-DSI IP input pin control 0.
*/
INT32 Im_DISP_Set_Mipi_Dsi_Ctrl0(BYTE shudown, BYTE color_mode)
{
	INT32 ret = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if ((shudown != D_IM_DISP_ENABLE_OFF) && (shudown != D_IM_DISP_ENABLE_ON)) {
		Ddim_Print(("E:Im_DISP_Set_Mipi_Dsi_Ctrl0: parameter error - shudown\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}

	if ((color_mode != D_IM_DISP_ENABLE_OFF) && (color_mode != D_IM_DISP_ENABLE_ON)) {
		Ddim_Print(("E:Im_DISP_Set_Mipi_Dsi_Ctrl0: parameter error - color_mode\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	
	im_disp_pclk_on();
	
	// Check trigger of common block that was selected MIPI.
	
	// Set MDSIC0
	IO_DISP.MDSCTL.MDSIC0.bit.MDSSD = shudown;
	IO_DISP.MDSCTL.MDSIC0.bit.MDSCM = color_mode;
	
	im_disp_pclk_off();
	
	return ret;
}

/* Get MIPI-DSI IP input pin control 0.
*/
INT32 Im_DISP_Get_Mipi_Dsi_Ctrl0(BYTE* shudown, BYTE* color_mode)
{
	INT32 ret = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if ((shudown == NULL) || (color_mode == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Mipi_Dsi_Ctrl0: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	
	im_disp_pclk_on();
	
	// Get MDSIC0
	*shudown = IO_DISP.MDSCTL.MDSIC0.bit.MDSSD;
	*color_mode= IO_DISP.MDSCTL.MDSIC0.bit.MDSCM;
	
	im_disp_pclk_off();
	
	return ret;
}

/* Set MIPI-DSI IP input pin control 1.
*/
INT32 Im_DISP_Set_Mipi_Dsi_Ctrl1(void)
{
	INT32 ret = D_DDIM_OK;
	
	im_disp_pclk_on();
	
	// not DPI configuration update wait.
	if (IO_DISP.MDSCTL.MDSIC1.bit.MDSUDC != D_IM_DISP_MDSUDC_READ_UPDATE_WAIT) {
		// Set MDSIC1
		IO_DISP.MDSCTL.MDSIC1.bit.MDSUDC = D_IM_DISP_MDSUDC_WRITE_1SHOT;
	}
	else {
		// status NG.
		Ddim_Print(("E:Im_DISP_Set_Mipi_Dsi_Ctrl1: Status NG - 1 shot signal input in update waiting.\n"));
		ret = D_IM_DISP_STATUS_ABNORMAL;
	}
	
	im_disp_pclk_off();
	
	return ret;
}

/* Get MIPI-DSI IP input pin control 1.
*/
INT32 Im_DISP_Get_Mipi_Dsi_Ctrl1(BYTE* update_config)
{
	INT32 ret = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if (update_config == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Mipi_Dsi_Ctrl1: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	
	im_disp_pclk_on();
	
	*update_config = IO_DISP.MDSCTL.MDSIC1.bit.MDSUDC;
	
	im_disp_pclk_off();
	
	return ret;
}

/* Set SR of IP (HDMI, MIPI).
*/
INT32 Im_DISP_Set_SR_IP(BYTE ip, BYTE sr)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (((ip & D_IM_DISP_IP_HDMI) != D_IM_DISP_IP_HDMI) &&
		((ip & D_IM_DISP_IP_MIPI) != D_IM_DISP_IP_MIPI)) {
		Ddim_Print(("E:Im_DISP_Set_SR_IP: ip parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((sr != D_IM_DISP_SR_CANCEL) && (sr != D_IM_DISP_SR_RESET)) {
		Ddim_Print(("E:Im_DISP_Set_SR_IP: sr parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// HDMI IP Reset.
	if (((ip & D_IM_DISP_IP_HDMI) == D_IM_DISP_IP_HDMI) &&
		(IO_DISP.MAIN[E_IM_DISP_HDMI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.JDDISP_HDMI_HDMIC.HDMISR.bit.SR = sr;	// software reset.
	}
	
	// MIPI-DSI IP Reset.
	if (((ip & D_IM_DISP_IP_MIPI) == D_IM_DISP_IP_MIPI) &&
		(IO_DISP.MAIN[E_IM_DISP_LCD_MIPI].DCORE.TRG.bit.TRG == E_IM_DISP_TRG_READ_NO_ACT)) {
		IO_DISP.MDSCTL.MDSSR.bit.SR = sr;				// software reset.
	}
	
	im_disp_pclk_off();
	Im_DISP_Dsb();

	return ret;
}

/* Get SR of IP (HDMI, MIPI).
*/
INT32 Im_DISP_Get_SR_IP(BYTE *sr)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (sr == NULL) {
		Ddim_Print(("E:Im_DISP_Get_SR_IP: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	*sr = 0;

	// check SR bit of HDMI IP and MIPI IP.
	if (IO_DISP.JDDISP_HDMI_HDMIC.HDMISR.bit.SR == D_IM_DISP_SR_RESET) {
		*sr |= D_IM_DISP_IP_HDMI;
	}
	if (IO_DISP.MDSCTL.MDSSR.bit.SR == D_IM_DISP_SR_RESET) {
		*sr |= D_IM_DISP_IP_MIPI;
	}
	
	im_disp_pclk_off();

	return ret;
}

/* Set Write Channel control.
*/
INT32 Im_DISP_Set_Write_Channel_Ctrl(E_IM_DISP_WC_NUM number, T_IM_DISP_CTRL_P2M const *const p2m, T_IM_DISP_CTRL_PWCH const *const pwch)
{
	INT32 ret = D_DDIM_OK;
	union io_pwchintenb pwchintenb;
	union io_pwchintflg pwchintflg;
	
#ifdef CO_PARAM_CHECK
	if ((p2m == NULL) || (pwch == NULL)) {
		Ddim_Print(("E:Im_DISP_Set_Write_Channel_Ctrl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	
	im_disp_pclk_on();
	
	// P2M Macro.
	
	// check status.
	if ((p2m->p2mpaen == D_IM_DISP_ENABLE_ON) ||
		(IO_DISP.P2M[number].P2MTRG.bit.P2MTRG == E_IM_DISP_P2M_PWCH_TRG_READ_NO_ACT)) {
		// Set P2MPAEN.
		IO_DISP.P2M[number].P2MPAEN.word	= p2m->p2mpaen;
	}
	
	// Set P2M2PMD (fix:1).
	IO_DISP.P2M[number].P2M2PMD.word		= 1;
	// Set P2MCTL.
	IO_DISP.P2M[number].P2MCTL.bit.PKNEMD	= 0;					// fix:0.
	IO_DISP.P2M[number].P2MCTL.bit.PLPF		= p2m->p2mctl_plpf;
	IO_DISP.P2M[number].P2MCTL.bit.P2MTYP	= p2m->p2mctl_p2mtyp;
	// Set P2MOFS/PBSFT/PCLPH.
	IO_DISP.P2M[number].P2MOFS.word			= p2m->p2mofs;
	IO_DISP.P2M[number].PBSFT.word			= p2m->pbsft;
	IO_DISP.P2M[number].PCLPH.word			= p2m->pclph;
	// Set PTRMV/PTRMH.
	IO_DISP.P2M[number].PTRMV.word			= p2m->ptrmv;
	IO_DISP.P2M[number].PTRMH.word			= p2m->ptrmh;
	// Set PTRMVW/PTRMHW.
	IO_DISP.P2M[number].PTRMVW.word			= p2m->ptrmvw;
	IO_DISP.P2M[number].PTRMHW.word			= p2m->ptrmhw;
	// Set PTRMXVCYC/PTRMXHCYC.
	IO_DISP.P2M[number].PTRMXVCYC.word		= p2m->ptrmxvcyc;
	IO_DISP.P2M[number].PTRMXHCYC.word		= p2m->ptrmxhcyc;
	// Set PTRMXVEN/PTRMXHEN.
	IO_DISP.P2M[number].PTRMXVEN.dword		= p2m->ptrmxven;
	IO_DISP.P2M[number].PTRMXHEN.dword		= p2m->ptrmxhen;
	// Set P2MMIR.
	IO_DISP.P2M[number].P2MMIR.word			= p2m->p2mmir;
	
	// PWCH Macro.
	// check trigger.
	if (IO_DISP.PWCH[number].PWCHTRG.bit.PWCHTRG == E_IM_DISP_P2M_PWCH_TRG_READ_NO_ACT) {
		// Set PWAXCTL.
		IO_DISP.PWCH[number].PWAXCTL.word	= pwch->pwaxctl.word;
	}
	
	// Set PWS (fix:0).
	IO_DISP.PWCH[number].PWS.word			= 0;
	
	// Set PWCHCTL.
	IO_DISP.PWCH[number].PWCHCTL.word		= 0x0002;		// PWLV[6:4]=000b, PWBMD[1:0]=01b.
	IO_DISP.PWCH[number].PWCHCTL.bit.PWMW	= pwch->pwmw;	// PWMW[8].

	// Set PWCHINTENB.
#if 0
	IO_DISP.PWCH[number].PWCHINTENB.word	= pwch->pwchintenb.word;
#else
	pwchintenb.word	= IO_DISP.PWCH[number].PWCHINTENB.word;
	pwchintflg.word	= 0;
	if (0 != (pwch->pwchintenb.word & D_IM_DISP_DWCH_FLG_PWF)){
		pwchintflg.bit.__PWF	= D_IM_DISP_INTFLG_CLEAR;
		pwchintenb.bit.PWE		= 1; //int_cfg->permission_flg;
	}
	if (0 != (pwch->pwchintenb.word & D_IM_DISP_DWCH_FLG_PWEF)){
		pwchintflg.bit.__PWEF	= D_IM_DISP_INTFLG_CLEAR;
		pwchintenb.bit.PWEE		= 1; //int_cfg->permission_flg;
	}
	if (0 != (pwch->pwchintenb.word & D_IM_DISP_DWCH_FLG_PWXF)){
		pwchintflg.bit.__PWXF	= D_IM_DISP_INTFLG_CLEAR;
		pwchintenb.bit.PWXE		= 1; //int_cfg->permission_flg;
	}

	IO_DISP.PWCH[number].PWCHINTFLG.word	= pwchintflg.word;
	IO_DISP.PWCH[number].PWCHINTENB.word	= pwchintenb.word;
#endif

	// Set PWSA/PWLSIZE.
	IO_DISP.PWCH[number].PWSA.word			= pwch->pwsa;
	IO_DISP.PWCH[number].PWLSIZE.word		= pwch->pwlsize;
	// Set PWVFM.
	IO_DISP.PWCH[number].PWVFM.word			= pwch->pwvfm;
	// Set PWDW.
	if (pwch->pwvfm == D_IM_DISP_FORMAT_VIDEO) {
		IO_DISP.PWCH[number].PWDW.word		= 0x0003;		// PWDW[1:0]=11b : 128byte aligned.
	}
	else {
		IO_DISP.PWCH[number].PWDW.word		= 0x0001;		// PWDW[1:0]=01b : 32byte aligned.
	}
	
	im_disp_pclk_off();
	
	return ret;
}

/* Get Write Channel control.
*/
INT32 Im_DISP_Get_Write_Channel_Ctrl(E_IM_DISP_WC_NUM number, T_IM_DISP_CTRL_P2M  *const p2m, T_IM_DISP_CTRL_PWCH *const pwch)
{
	INT32 ret = D_DDIM_OK;
	
#ifdef CO_PARAM_CHECK
	if ((p2m == NULL) || (pwch == NULL)) {
		Ddim_Print(("E:Im_DISP_Get_Write_Channel_Ctrl: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	
	im_disp_pclk_on();
	
	// P2M Macro.
	// Get P2MPAEN.
	p2m->p2mpaen		= IO_DISP.P2M[number].P2MPAEN.word;
	// Get P2MCTL.PLPF/P2MCTL.P2MTYP.
	p2m->p2mctl_plpf	= IO_DISP.P2M[number].P2MCTL.bit.PLPF;
	p2m->p2mctl_p2mtyp	= IO_DISP.P2M[number].P2MCTL.bit.P2MTYP;
	// Get P2MOFS/PBSFT/PCLPH.
	p2m->p2mofs			= IO_DISP.P2M[number].P2MOFS.word;
	p2m->pbsft			= IO_DISP.P2M[number].PBSFT.word;
	p2m->pclph			= IO_DISP.P2M[number].PCLPH.word;
	// Get PTRMV/PTRMH.
	p2m->ptrmv			= IO_DISP.P2M[number].PTRMV.word;
	p2m->ptrmh			= IO_DISP.P2M[number].PTRMH.word;
	// Get PTRMVW/PTRMHW.
	p2m->ptrmvw			= IO_DISP.P2M[number].PTRMVW.word;
	p2m->ptrmhw			= IO_DISP.P2M[number].PTRMHW.word;
	// Get PTRMXVCYC/PTRMXHCYC.
	p2m->ptrmxvcyc		= IO_DISP.P2M[number].PTRMXVCYC.word;
	p2m->ptrmxhcyc		= IO_DISP.P2M[number].PTRMXHCYC.word;
	// Get PTRMXVEN/PTRMXHEN.
	p2m->ptrmxven		= IO_DISP.P2M[number].PTRMXVEN.dword;
	p2m->ptrmxhen		= IO_DISP.P2M[number].PTRMXHEN.dword;
	// Get P2MMIR/P2M2PMD.
	p2m->p2mmir			= IO_DISP.P2M[number].P2MMIR.word;
	p2m->p2m2pmd		= IO_DISP.P2M[number].P2M2PMD.word;
	
	// PWCH Macro.
	// Get PWAXCTL.
	pwch->pwaxctl.word		= IO_DISP.PWCH[number].PWAXCTL.word;
	// Get PWCHCTL.PWMW.
	pwch->pwmw				= IO_DISP.PWCH[number].PWCHCTL.bit.PWMW;
	// Get PWCHINTENB.
	pwch->pwchintenb.word	= IO_DISP.PWCH[number].PWCHINTENB.word;
	// Get PWSA/PWLSIZE.
	pwch->pwsa				= IO_DISP.PWCH[number].PWSA.word;
	pwch->pwlsize			= IO_DISP.PWCH[number].PWLSIZE.word;
	// Get PWVFM.
	pwch->pwvfm				= IO_DISP.PWCH[number].PWVFM.word;
	
	im_disp_pclk_off();
	
	return ret;
}

/* Get write channel response.
*/
INT32 Im_DISP_Get_Write_Channel_Response(E_IM_DISP_WC_NUM number, ULONG *response)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (response == NULL) {
		Ddim_Print(("E:Im_DISP_Get_Write_Channel_Response: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	
	// Get PWCHBRESP.
	*response = IO_DISP.PWCH[number].PWCHBRESP.word;
	
	im_disp_pclk_off();

	return ret;
}


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/* Set CSC matrix to covert from BT.601 to BT.709's RGB or no any convert.
*/
INT32 Im_DISP_Change_CSC_Matrix(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_CC_MATRIX convert)
{
	INT32 i;
	INT32 ret = D_DDIM_OK;
	U_IM_DISP_YR_MATRIX_COEFFICIENT matrix_to_709[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x0000006500000040 },	// 0x40,0x00,0x65 (1.0,  0.0,      1.5748)
		{ 0x000000E200F40040 },	// 0x40,0xF4,0xE2 (1.0, -0.1873,  -0.4681)
		{ 0x0000000000770040 }	// 0x40,0x77,0x00 (1.0,  1.85565,  0.0)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT matrix_to_through[D_IM_DISP_MATRIX_SIZE] = {
		// Default
		{ 0x0000005A00000040 },	// 0x40,0x00,0x5A (1.0,  0.0,       1.40625)
		{ 0x000000D300EA0040 },	// 0x40,0xEA,0xD3 (1.0, -0.34375,  -0.703125)
		{ 0x000000FF00710040 }	// 0x40,0x71,0xFF (1.0,  1.765625,  0.015625)
	};	// No any convert
	
	U_IM_DISP_GRY2R matrix_to_709_osd[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x00650040 },	// 0x40,0x00,0x65 (1.0,  0.0,      1.5748)
		{ 0x00E2F440 },	// 0x40,0xF4,0xE2 (1.0, -0.1873,  -0.4681)
		{ 0x00007740 }	// 0x40,0x77,0x00 (1.0,  1.85565,  0.0)
	};
	U_IM_DISP_GRY2R matrix_to_through_osd[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x005A0040 },	// 0x40,0x00,0x5A (1.0,  0.0,       1.40625)
		{ 0x00D3EA40 },	// 0x40,0xEA,0xD3 (1.0, -0.34375,  -0.703125)
		{ 0x00FF7140 }	// 0x40,0x71,0xFF (1.0,  1.765625,  0.015625)
	};

	E_IM_DISP_SEL_LAYER osd_layer[2] = {
		E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_SEL_LAYER_OSD_1
	};
	
	U_IM_DISP_YR_MATRIX_COEFFICIENT* ycc2rgb;
	U_IM_DISP_GRY2R* ycc2rgb_osd;

#ifdef CO_PARAM_CHECK
	if ((layer & E_IM_DISP_SEL_LAYER_ALL) == E_IM_DISP_SEL_LAYER_NONE) {
		Ddim_Print(("E:Im_DISP_Change_CSC_Matrix: layer parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	if(convert == E_IM_DISP_CC_MATRIX_709) {
		ycc2rgb		= matrix_to_709;
		ycc2rgb_osd	= matrix_to_709_osd;
	}
	else {
		ycc2rgb		= matrix_to_through;
		ycc2rgb_osd	= matrix_to_through_osd;
	}
	
	// Main
	if((layer & E_IM_DISP_SEL_LAYER_MAIN) == E_IM_DISP_SEL_LAYER_MAIN) {
		ret = Im_DISP_Set_Matrix(block, ycc2rgb);
	}
	// OSD
	for(i = 0; i < 2; i++) {
		if((layer & osd_layer[i]) == osd_layer[i]) {
			ret = Im_DISP_Set_OSD_Matrix(block, layer, ycc2rgb_osd);
		}
	}
	// Output
	if((layer & E_IM_DISP_SEL_LAYER_DCORE) == E_IM_DISP_SEL_LAYER_DCORE) {
		ret = Im_DISP_Set_Output_Matrix(block, E_IM_DISP_MATRIX_KIND_Y2R, ycc2rgb);
	}

	return ret;
}

/* Change color bar's color which was used as black back.
*/
VOID Im_DISP_Change_BB_Color(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR black_back_color)
{
	im_disp_pclk_on();
	// Set CLBHSIZE/CLBDT0
	IO_DISP.MAIN[block].DCORE.CLBHSIZE.word = IO_DISP.MAIN[block].DCORE.OHSIZE.word;
	IO_DISP.MAIN[block].DCORE.CLBDT[0].word = black_back_color.word;
	im_disp_pclk_off();
}

/* Get the color bar's color which was used as black back.
*/
INT32 Im_DISP_Check_BB_Color(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR* black_back_color)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (black_back_color == NULL) {
		Ddim_Print(("E:Im_DISP_Check_BB_Color: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Get CLBDT0
	black_back_color->word = IO_DISP.MAIN[block].DCORE.CLBDT[0].word;
	im_disp_pclk_off();

	return ret;
}

/* Change grid.
*/
INT32 Im_DISP_Change_Grid(E_IM_DISP_SEL block, UINT32 ghnum, UINT32 gvnum)
{
	ULONG ohsize;
	ULONG ovsize;
	ULONG ghitv;
	ULONG gvitv;
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if ((ghnum >= D_IM_DISP_GRID_NUM) || (gvnum >= D_IM_DISP_GRID_NUM)) {
		Ddim_Print(("E:Im_DISP_Change_Grid: ghnum/gvnum parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	ohsize = IO_DISP.MAIN[block].DCORE.OHSIZE.word;
	ovsize = IO_DISP.MAIN[block].DCORE.OVSIZE.word;

	ghitv = ohsize / (ghnum + 2);
	gvitv = ovsize / (gvnum + 2);
	
	IO_DISP.MAIN[block].DCORE.GHDSTA.bit.GHDSV = ghitv - (IO_DISP.MAIN[block].DCORE.GWIDTH.bit.GHWID) / 2;
	IO_DISP.MAIN[block].DCORE.GVDSTA.bit.GVDSH = gvitv - (IO_DISP.MAIN[block].DCORE.GWIDTH.bit.GVWID) / 2;
	
	IO_DISP.MAIN[block].DCORE.GITVL.bit.GHITV = ghitv;
	IO_DISP.MAIN[block].DCORE.GITVL.bit.GVITV = gvitv;

	IO_DISP.MAIN[block].DCORE.GNUM.bit.GHNUM = ghnum;
	IO_DISP.MAIN[block].DCORE.GNUM.bit.GVNUM = gvnum;
	im_disp_pclk_off();

	return ret;
}

/* Draw single face frame.
*/
INT32 Im_DISP_Draw_Face_Frame_Single(E_IM_DISP_SEL block, INT32 index, INT32 enable, T_IM_DISP_FACE_FRAME_PARAM const *const face)
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if (face == NULL){
		Ddim_Print(("E:Im_DISP_Draw_Face_Frame_Single: NULL check error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (index >= D_IM_DISP_FACE_FRAME_COUNT){
		Ddim_Print(("E:Im_DISP_Draw_Face_Frame_Single: index parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if ((enable != D_IM_DISP_ENABLE_OFF) && (enable != D_IM_DISP_ENABLE_ON)){
		Ddim_Print(("E:Im_DISP_Draw_Face_Frame_Single: enable parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
	if (im_disp_param_check_face_frame(face) != D_DDIM_OK){
		Ddim_Print(("E:Im_DISP_Draw_Face_Frame_Single: FFDSTA/FFSIZE/FFWIDTH parameter error\n"));
		return D_IM_DISP_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_disp_pclk_on();
	// Set FFDSTA/FFSIZE/FFWIDTH/FFCLR
	IO_DISP.MAIN[block].DCORE.FFDSTA[index].word = face->ffdsta.word;
	IO_DISP.MAIN[block].DCORE.FFSIZE[index].word = face->ffsize.word;
	IO_DISP.MAIN[block].DCORE.FFWIDTH[index].word = face->ffwidth.word;
	IO_DISP.MAIN[block].DCORE.FFCLR[index].word = face->ffclr.word;

	// Set FFDISPEN
	if (enable == D_IM_DISP_ENABLE_ON) {
		// Display enable
		IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN |= ((ULLONG)D_IM_DISP_ENABLE_ON << index);
	}
	else {
		// Display disable
		IO_DISP.MAIN[block].DCORE.FFDISPEN.bit.FFDEN &= ~((ULLONG)D_IM_DISP_ENABLE_ON << index);
	}
	im_disp_pclk_off();

	return ret;
}
#endif
