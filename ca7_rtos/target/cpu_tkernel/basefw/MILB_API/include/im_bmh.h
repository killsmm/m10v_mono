/**
 * @file		im_bmh.h
 * @brief		Header file of JMLBMH2A driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/**
@addtogroup image
@{
	@addtogroup im_bmh
	@{
	- @ref im_bmh_sample
*/

#ifndef _IM_BMH_H_
#define _IM_BMH_H_

#include "driver_common.h"

#include "ddim_typedef.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#define	D_IM_BMH_SEM_TIMEOUT_ERR		(D_IM_BMH | D_DDIM_SEM_TIMEOUT)			/**< Time Out                 (0xXX000011) */
#define	D_IM_BMH_NG						(D_IM_BMH | D_DDIM_SYSTEM_ERROR)		/**< Processing NG            (0xXX000099) */
#define D_IM_BMH_SEM_NG					(D_IM_BMH | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG (0xXX000010) */
#define D_IM_BMH_INPUT_PARAMETER_ERROR	(D_IM_BMH | D_DDIM_INPUT_PARAM_ERROR)	/**< parameter error.         (0xXX000001) */
#define D_IM_BMH_BUSY_NG				(D_IM_BMH | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy               (0xXX000005) */
#define	D_IM_BMH_AXI_ERR				(D_IM_BMH | 0x100)						/**< AXI bus error.           (0xXX000100) */

#define D_IM_BMH_FLG_BMH_END		(0x00000001)	/**< bmh end flag		*/
#define D_IM_BMH_FLG_BMH_AXI_ERR	(0x00000010)	/**< bmh AXI error flag	*/



#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** BMH enable / disable
*/
typedef enum {
	E_IM_BMH_ONOFF_DISABLE = 0,		/**< BMH disable	*/
	E_IM_BMH_ONOFF_ENABLE			/**< BMH enable	*/
} E_IM_BMH_ONOFF;


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

typedef VOID (*T_IM_BMH_CALLBACK)( ULONG interrupt_flag_mask );	/**< Callback function pointer	*/

/** BMH Control info for common
*/
typedef struct {
	UCHAR				lselcnt;	/**< Processing block vertical size setting [0000b ~ 1000b (4bit)]		@@LSELCNT	*/
	UCHAR				rbs;		/**< Reference block size setting [000b ~ 111b (3bit)]		@@RBS	*/
	UCHAR				pshift;		/**< Current block Pixel shift number setting [000b ~ 100b (3bit)]		@@PSHIFT	*/
	UCHAR				outmd;		/**< Setting the operation result output mode [000b ~ 110b (3bit)]		@@OUTMD	*/
	UCHAR				lutmd;		/**< LUT mode setting (0:LUT unused / 1:LUT used)		@@LUTMD	*/
	UCHAR				offmd;		/**< Position offset mode setting ((0:Minimum SAD position / 1:Position offset value))		@@OFFMD	*/
	UCHAR				mode;		/**< Setting process mode [0000b ~ 1000b (4bit)]		@@MODE	*/
	ULONG				poffset;	/**< Position offset value (aligned on 1byte)			@@POFFSET	*/
	ULONG				sadlv;		/**< SAD level comparison value (aligned on 2byte)		@@SADLV	*/
	T_IM_BMH_CALLBACK	pCallBack;	/**< callback function	*/
} T_IM_BMH_CTRL;

/** Im_BMH_Ctrl_Axi() parameter structure */
typedef struct {
	UINT32	read_cache_type;			/**< Read Cache type. */
	UINT32	read_protection_type;		/**< Read Protection type. */
	UINT32	write_cache_type;			/**< Write Cache type. */
	UINT32	write_protection_type;		/**< Write Protection type. */
} T_IM_BMH_AXI;

/** Im_BMH_Get_AXI_Status() parameter structure */
typedef struct {
	UCHAR	read_channel_response;	/**< RRESP register */
	UCHAR	write_channel_response;	/**< WRESP register */
} T_IM_BMH_AXI_STATUS;

/** Im_BMH_Set_Bmhre()/Im_BMH_Get_Bmhre() parameter structure */
typedef struct {
	UCHAR	bmhre;	/**< BMHRE register */
} T_IM_BMH_BMHRE;

/** Im_BMH_Get_Rsst() parameter structure */
typedef struct {
	UCHAR	rsst;	/**< BMHINTFS-RSST register */
} T_IM_BMH_RSST;

/** Im_BMH_Set_Bmhcont()/Im_BMH_Get_Bmhcont() parameter structure */
typedef struct {
	UCHAR	cont;	/**< BMHCONT-CONT register */
} T_IM_BMH_BMHCONT;

/** Im_BMH_Set_Bmhhcnt()/Im_BMH_Get_Bmhhcnt() parameter structure */
typedef struct {
	UCHAR	hcnt;	/**< BMHHCNT-HCNT register */
	UCHAR	rcnt;	/**< BMHHCNT-RCNT register */
} T_IM_BMH_BMHHCNT;

/** Im_BMH_Set_Bmhsad_wr()/Im_BMH_Get_Bmhsad_wr() parameter structure */
typedef struct {
	UCHAR	sad_wr;	/**< BMHSAD_WR-SAD_WR register */
} T_IM_BMH_BMHSAD_WR;

/** BMH Control info for DMA mode
*/
typedef struct {
	ULONG				dmamd;			/**< DMA mode setting (0:DMA mode Off / 1:DMA mode ON)	@@DMAMD	*/
	ULONG				ghsize_cur;		/**< Current image GHSIZE setting register (16bit) 8byte boundary		@@GHSIZE_CUR	*/
	ULONG				ghsize_ref;		/**< Reference image GHSIZE setting register (16bit) 8byte boundary	@@GHSIZE_REF	*/
	ULONG				hsize_ref;		/**< Reference image HSIZE setting register (16bit)		@@HSIZE_REF	*/
	ULONG				vsize_ref;		/**< Reference image VSIZE setting register (16bit)		@@VSIZE_REF	*/
	ULONG				curstad;		/**< Current image storage location start address setting register (32bit) <br>
											The start address, 8byte boundary		@@CURSTAD	*/
	ULONG				refstad;		/**< Reference image storage destination top address setting register (32bit) <br>
											The start address, 8byte boundary		@@REFSTAD	*/
	ULONG				sadstad;		/**< Calculation result storage destination top address setting register (32bit) <br>
											The start address, 8byte boundary		@@SADSTAD	*/
	ULONG				sadghsize;		/**< Calculation result storage destination GHSIZE setting register (16bit) 8byte boundary	@@BMHSADGH	*/
} T_IM_BMH_CTRL_DMA;

/** Im_BMH_Set_Cur() parameter structure */
typedef struct {
	UCHAR			cur_write_flg[9][256];	/**< CUR write flag (1:write other:do nothing)*/
	UCHAR			cur_data[9][256];		/**< CUR Write data */
} T_IM_BMH_CUR_BLOCK;

/** Im_BMH_Set_Ref() parameter structure */
typedef struct {
	UCHAR			ref_write_flg[9][256];	/**< REF write flag (1:write other:do nothing)*/
	UCHAR			ref_data[9][256];		/**< REF Write data */
} T_IM_BMH_REF_BLOCK;

/** Im_BMH_Set_Lut() parameter structure */
typedef struct {
	USHORT	lut[2048];				/**< LUT data */
} T_IM_BMH_LUT;

/** Im_BMH_Get_Sadr_000() parameter structure */
/** Array[225]-[227] is dummy.(Do not use) **/
typedef struct {
	UCHAR	sadcmp[2][228];			/**< SAD comparative result */
	UCHAR	pos[2][228];			/**< Position of minimum SAD  */
} T_IM_BMH_SADR_000;

/** Im_BMH_Get_Sadr_001() parameter structure */
typedef struct {
	UCHAR	sadcmp[225];			/**< SAD comparative result */
	UCHAR	pos[225];				/**< Position of minimum SAD */
	USHORT	sadmin[225];			/**< Value minimum SAD */
} T_IM_BMH_SADR_001;

/** Im_BMH_Get_Sadr_010() parameter structure */
/** Array[225] is dummy.(Do not use) **/
typedef struct {
	UCHAR	sadcmp[2][226];			/**< SAD comparative result */
	UCHAR	pos_integer[2][226];	/**< Position of minimum SAD(integer part) */
	UCHAR	pos_fraction[2][226];	/**< Position of minimum SAD(fractional part) */
} T_IM_BMH_SADR_010;

/** Im_BMH_Get_Sadr_011() parameter structure */
typedef struct {
	UCHAR	sadcmp[225];			/**< SAD comparative result */
	UCHAR	pos_integer[225];		/**< Position of minimum SAD(integer part) */
	UCHAR	pos_fraction[225];		/**< Position of minimum SAD(fractional part) */
	USHORT	sadmin[225];			/**< Value minimum SAD */
} T_IM_BMH_SADR_011;

/** Im_BMH_Get_Sadr_100() parameter structure */
typedef struct {
	UCHAR	sadcmp[2][112];			/**< SAD comparative result */
	UCHAR	pos_integer[2][112];	/**< Position of minimum SAD(integer part) */
	UCHAR	pos_fraction[2][112];	/**< Position of minimum SAD(fractional part) */
	USHORT	sadmin[2][112];			/**< Value minimum SAD */
} T_IM_BMH_SADR_100;

/** Im_BMH_Get_Sadr_101() parameter structure */
typedef struct {
	UCHAR	sadcmp[2][56];			/**< SAD comparative result */
	UCHAR	pos_integer[2][56];		/**< Position of minimum SAD(integer part) */
	UCHAR	pos_fraction[2][56];	/**< Position of minimum SAD(fractional part) */
	USHORT	sadmin[2][56];			/**< Value minimum SAD */
	UCHAR	weight[2][56];			/**< Weight of minimum SAD */
	UCHAR	deviation[2][56];		/**< standard deviation */
	UCHAR	cur_max[2][56];			/**< Max value of Current */
	UCHAR	cur_min[2][56];			/**< Minimum value of Current */
} T_IM_BMH_SADR_101;

/** Im_BMH_Get_Sadr_110() parameter structure */
typedef struct {
	UCHAR	sadcmp[2][28];			/**< SAD comparative result */
	UCHAR	pos[2][28];				/**< Position of minimum SAD */
	USHORT	sadmin[2][28];			/**< Value minimum SAD */
	USHORT	sadmin1before[2][28];	/**< Value minimum SAD(sadmin[x][y]-1) */
	USHORT	sadmin1after[2][28];	/**< Value minimum SAD(sadmin[x][y]+1) */
	USHORT	sadmin2before[2][28];	/**< Value minimum SAD(sadmin[x][y]-2) */
	USHORT	sadmin2after[2][28];	/**< Value minimum SAD(sadmin[x][y]+2) */
	UCHAR	weight[2][28];			/**< Weight of minimum SAD */
	UCHAR	deviation[2][28];		/**< standard deviation */
	UCHAR	cur_max[2][28];			/**< Max value of Current */
	UCHAR	cur_min[2][28];			/**< Minimum value of Current */
} T_IM_BMH_SADR_110;




#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus
//---------------------------- driver section ----------------------------

/**
The semaphore of BMH is acquired
@param[in]	channel_no		Channel No
@param[in]	tmout			Time of timeout<br>
							<ul><li>Positive Value(Time of timeout)
								<li>@ref D_DDIM_USER_SEM_WAIT_POL
								<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_BMH_SEM_NG					Lock Error (System Using designated channel number)
@retval		D_IM_BMH_TIMEOUT				Input Parameter Error
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32 Im_BMH_Open( BYTE channel_no, const INT32 tmout );

/**
Set software reset and operating mode
@param[in]	channel_no		Channel No
@retval		D_DDIM_OK						Success
*/
extern	INT32 Im_BMH_Init( BYTE channel_no );

/**
Bmh Control
@param[in]	channel_no		Channel No
@param[in]	bmh_ctrl		BMH control parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Ctrl( BYTE channel_no, const T_IM_BMH_CTRL* const bmh_ctrl );

/**
Get Bmh Control Status
@param[in]	channel_no		Channel No
@param[out]	bmh_ctrl	Bmh Control Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Ctrl( BYTE channel_no, T_IM_BMH_CTRL* const bmh_ctrl );

/**
BMHRE set
@param[in]	channel_no		Channel No
@param[in]	sram
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Bmhre( BYTE channel_no, const T_IM_BMH_BMHRE* const sram );

/**
Get BMHRE Status
@param[in]	channel_no		Channel No
@param[out]	sram	BMHRE Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Bmhre( BYTE channel_no, T_IM_BMH_BMHRE* const sram );

/**
Get BMHINTFS-RSST Status
@param[in]	channel_no		Channel No
@param[out]	rsst	BMHINTFS-RSST Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Rsst( BYTE channel_no, T_IM_BMH_RSST* const rsst );

/**
BMHCONT set
@param[in]	channel_no		Channel No
@param[in]	cont
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Bmhcont( BYTE channel_no, const T_IM_BMH_BMHCONT* const cont );

/**
Get BMHCONT Status
@param[in]	channel_no		Channel No
@param[out]	cont	BMHCONT Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Bmhcont( BYTE channel_no, T_IM_BMH_BMHCONT* const cont );

/**
BMH Control set AXI bus I/F Control
@param[in]	channel_no		Channel No
@param[in]	axi_ctrl
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Bmhaxi( BYTE channel_no, const T_IM_BMH_AXI* const axi_ctrl );

/**
Get AXI bus I/F Status
@param[in]	channel_no		Channel No
@param[out]	sts	AXI bus I/F Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Bmhaxierr( BYTE channel_no, T_IM_BMH_AXI_STATUS* const sts );

/**
BMHHCNT set
@param[in]	channel_no		Channel No
@param[in]	cnt
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Bmhhcnt( BYTE channel_no, const T_IM_BMH_BMHHCNT* const cnt );

/**
Get BMHHCNT Status
@param[in]	channel_no		Channel No
@param[out]	cnt	BMHHCNT Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Bmhhcnt( BYTE channel_no, T_IM_BMH_BMHHCNT* const cnt );

/**
BMHSAD_WR set
@param[in]	channel_no		Channel No
@param[in]	sad_wr
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Bmhsad_wr( BYTE channel_no, const T_IM_BMH_BMHSAD_WR* const sad_wr );

/**
Get BMHSAD_WR Status
@param[in]	channel_no		Channel No
@param[out]	sad_wr	BMHSAD_WR Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Bmhsad_wr( BYTE channel_no, T_IM_BMH_BMHSAD_WR* const sad_wr );

/**
Dma mode control set
@param[in]	channel_no		Channel No
@param[in]	bmh_ctrl_dma
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_BMH_BUSY_NG				Macro busy.
*/
extern	INT32 Im_BMH_Ctrl_Dma( BYTE channel_no, const T_IM_BMH_CTRL_DMA* const bmh_ctrl_dma );

/**
Get Dma mode control Status
@param[in]	channel_no		Channel No
@param[out]	bmh_ctrl_dma	Dma mode control Status parameter
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Ctrl_Dma( BYTE channel_no, T_IM_BMH_CTRL_DMA* const bmh_ctrl_dma );

/**
LUT set
@param[in]	channel_no		Channel No
@param[in]	lut_data
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Lut( BYTE channel_no, const T_IM_BMH_LUT* const lut_data );

/**
BMH Start Sync
@param[in]	channel_no		Channel No
@param[in]	wait_time		Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_DDIM_OK						success.
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_BMH_AXI_ERR				Fail - AXI bus error.
@retval		D_IM_BMH_TIMEOUT				Fail - Processing NG. (Time outed)
@retval		D_IM_BMH_NG						Fail - Process NG. (system error)
@remarks	BMH function is started(enable) Sync.
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_BMH_Start_Sync( BYTE channel_no, const INT32 wait_time );

/**
BMH Start Async
@param[in]	channel_no		Channel No
@retval		D_DDIM_OK						Success
@remarks	BMH function is started(enable) Async.
@remarks	This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_BMH_Start_Async( BYTE channel_no );

/**
BMH Wait End
@param[in]	channel_no		Channel No
@param[out]	p_wait_factor	: Pointer of factor bitfield parameter which release wait process.<br>
								  <ul>
										 <li>@ref D_IM_BMH_FLG_BMH_END
										 <li>@ref D_IM_BMH_FLG_BMH_AXI_ERR
								  </ul>
@param[in]	wait_time		Wait time [msec]. The valid range is -1, 0, ... .
							If this parameter is set to -1, driver waits permanently unless system-call sets event-flag.
@retval		D_DDIM_OK						Success.
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
@retval		D_IM_BMH_AXI_ERR				Fail - AXI bus error.
@retval		D_IM_BMH_TIMEOUT				Fail - Processing NG.(Time outed)
@retval		D_IM_BMH_SEM_NG					Fail - Process NG. (Event flag error)
@remarks	Wait for BMH function is ended.
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Im_BMH_Wait_End( BYTE channel_no, UINT32* const p_wait_factor, const INT32 wait_time );

/**
BMH Stop
@param[in]	channel_no		Channel No
@retval		D_DDIM_OK						Success.
@remarks	BMH function is stopped(disable).
*/
extern	INT32 Im_BMH_Stop( BYTE channel_no );

/**
The semaphore of BMH is returned
@param[in]	channel_no		Channel No
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_SEM_NG					Fail - Processing NG (system error)
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern	INT32 Im_BMH_Close( BYTE channel_no );

/**
BMH Macro interrupt permission set
@param[in]	channel_no		Channel No
@param[in]	interrupt_bit : Specified interrupt bit
		value range :[D_IM_BMH_FLG_BMH_END | D_IM_BMH_FLG_BMH_AXI_ERR ]<br>
@param[in]	permission_flg : Interrupt enable or disable flg<br>
							value range :[0:disable 1:enable]
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@code
	//sample code
	USHORT interrupt_bit;
	Byte channel_no = 0;

	interrupt_bit = (D_IM_PRO_LVDSINTENB_EOLE|D_IM_BMH_FLG_BMH_AXI_ERR);

	Im_BMH_Set_Interrupt( channel_no, interrupt_bit, 1 );
@endcode
*/
extern	INT32 Im_BMH_Set_Interrupt( BYTE channel_no, USHORT interrupt_bit, UCHAR permission_flg );

/**
BMH Macro Interrupt handler
@param[in]	channel_no		Channel No
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_BMH_Int_Handler( BYTE channel_no );

/**
CUR set
@param[in]	channel_no		Channel No
@param[in]	cur_write_data
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Cur( BYTE channel_no, const T_IM_BMH_CUR_BLOCK* const cur_write_data );

/**
REF set
@param[in]	channel_no		Channel No
@param[in]	ref_write_data
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Set_Ref( BYTE channel_no, const T_IM_BMH_REF_BLOCK* const ref_write_data );

/**
Get SADR data (BMHMD.OUTMD=000b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=000b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_000( BYTE channel_no, T_IM_BMH_SADR_000* const sadr_read_data );

/**
Get SADR data (BMHMD.OUTMD=001b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=001b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_001( BYTE channel_no, T_IM_BMH_SADR_001* const sadr_read_data );

/**
Get SADR data (BMHMD.OUTMD=010b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=010b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_010( BYTE channel_no, T_IM_BMH_SADR_010* const sadr_read_data );

/**
Get SADR data (BMHMD.OUTMD=011b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=011b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_011( BYTE channel_no, T_IM_BMH_SADR_011* const sadr_read_data );

/**
Get SADR data (BMHMD.OUTMD=100b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=100b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_100( BYTE channel_no, T_IM_BMH_SADR_100* const sadr_read_data );

/**
Get SADR data (BMHMD.OUTMD=101b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=101b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_101( BYTE channel_no, T_IM_BMH_SADR_101* const sadr_read_data );

/**
Get SADR data (BMHMD.OUTMD=110b)
@param[in]	channel_no		Channel No
@param[out]	sadr_read_data	SADR data (BMHMD.OUTMD=110b)
@retval		D_DDIM_OK						Success
@retval		D_IM_BMH_INPUT_PARAMETER_ERROR	parameter error.
*/
extern	INT32 Im_BMH_Get_Sadr_110( BYTE channel_no, T_IM_BMH_SADR_110* const sadr_read_data );


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/**	@}
@}*/

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IM_BMH_H_

/**
@weakgroup im_bmh
@{

<hr>

@section im_bmh_sample	Sample code
@code
// SAMPLE CODE //
#include "im_bmh.h"

INT32 sample_bmh( VOID )
{
	INT32 im_ercd;
	INT32 im_ercd2;
	UINT32 i, j;
	BYTE channel_no = 0;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,					// lselcnt	Processing block vertical size setting	@@LSELCNT
		.rbs = 0,						// rbs		Reference block size setting	@@RBS
		.pshift = 0,					// pshift	Current block Pixel shift number setting	@@PSHIFT
		.outmd = 0,						// outmd	Setting the operation result output mode	@@OUTMD
		.lutmd = 0,						// lutmd	LUT mode setting	@@LUTMD
		.offmd = 0,						// offmd	Position offset mode setting	@@OFFMD
		.mode = 1,						// mode		Setting process mode	@@MODE
		.poffset = 0,					// poffset	Position offset value	@@POFFSET
		.sadlv = 0,						// sadlv	SAD level comparison value	@@SADLV
		.pCallBack = NULL,				// pCallBack	callback function
	};
	T_IM_BMH_BMHHCNT cnt = {
		.hcnt = 0,						// hcnt		BMHHCNT-HCNT register		@@RCNT
		.rcnt = 0,						// rcnt		BMHHCNT-RCNT register		@@HCNT
	};
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for(i = 0; i < 9; i++){
		for(j = 0; j < 256; j++){
			cur_write_data.cur[i][j].cur_write_flg = 1;
			cur_write_data.cur[i][j].cur_data = j;
			ref_write_data.ref[i][j].ref_write_flg = 1;
			ref_write_data.ref[i][j].ref_data = j/2;
		}
	}

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ) {
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ) {
		return im_ercd;
	}

	// don't forget Im_BMH_Close() after this line

	while( 1 ) {
		im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
		if( im_ercd != D_DDIM_OK ) {
			break;
		}

		break;
	}

	im_ercd2 = Im_BMH_Stop( channel_no );
	if( im_ercd2 != D_DDIM_OK ) {
		im_ercd = im_ercd2;
	}

	if( im_ercd == D_DDIM_OK ) {
		im_ercd2 = Im_BMH_Close( channel_no );
		if( im_ercd2 != D_DDIM_OK ) {
			im_ercd = im_ercd2;
		}
	}
	else {
		(VOID)Im_BMH_Close( channel_no );
	}

	return im_ercd;
}
@endcode
@}*/
