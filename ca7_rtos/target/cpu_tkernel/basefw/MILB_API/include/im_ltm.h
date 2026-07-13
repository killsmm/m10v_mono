/**
 * @file		im_ltm.h
 * @brief		local tone mapping process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/**
@weakgroup im_ltm
@{
	@defgroup im_ltm_rbk RBK
	@defgroup im_ltm_map MAP
	@defgroup im_ltm_ltm LTM
*/
/** @} */

/**
@weakgroup im_ltm
@{
	- @ref im_ltm_sequence
	  - @ref im_ltm_rbk_sequence
	    - @ref RBK_Init
	    - @ref RBK_Ctrl_SDRAMInput_Sample
	    - @ref RBK_Ctrl_SRODirect_Sample
	  - @ref im_ltm_map_sequence
	    - @ref MAP_Init
	    - @ref MAP_Ctrl_SDRAMInput_Sample
	    - @ref MAP_Ctrl_RBKDirect_Sample
	  - @ref im_ltm_ltm_sequence
	    - @ref LTM_Init
	    - @ref LTM_Ctrl_SDRAMInput_Sample
	    - @ref LTM_Ctrl_B2RDirect_Sample

	- @ref im_ltm_sample
	  - @ref im_ltm_sample_section1
	  - @ref im_ltm_sample_section2
	  - @ref im_ltm_sample_section3
	  - @ref im_ltm_sample_section4
	  - @ref im_ltm_sample_section5
	  - @ref im_ltm_sample_section6
	  - @ref im_ltm_sample_section7
	  - @ref im_ltm_sample_section8

	<hr>
	@section im_ltm_sequence Sequence
	@section im_ltm_rbk_sequence RBK Sequence

	@section RBK_Init						Sequence of RBK initialization
	@image html im_rbk_init.png

	@section RBK_Ctrl_SDRAMInput_Sample		Sequence of RBK control(SDRAM Input) sample sequence
	@image html im_rbk_sdraminput_sample.png

	@section RBK_Ctrl_SRODirect_Sample		Sequence of RBK control(SRO Direct) sample sequence
	@image html im_rbk_srodirect_sample.png

	@section im_ltm_map_sequence MAP Sequence

	@section MAP_Init						Sequence of MAP initialization
	@image html im_map_init.png

	@section MAP_Ctrl_SDRAMInput_Sample		Sequence of MAP control(SDRAM Input) sample sequence
	@image html im_map_sdraminput_sample.png

	@section MAP_Ctrl_RBKDirect_Sample		Sequence of MAP control(RBK Direct) sample sequence
	@image html im_map_rbkdirect_sample.png

	@section im_ltm_ltm_sequence LTM Sequence

	@section LTM_Init						Sequence of LTM initialization
	@image html im_ltm_init.png

	@section LTM_Ctrl_SDRAMInput_Sample		Sequence of LTM control(SDRAM Input) sample sequence
	@image html im_ltm_sdraminput_sample.png

	@section LTM_Ctrl_B2RDirect_Sample		Sequence of LTM control(B2R Direct) sample sequence
	@image html im_ltm_b2rdirect_sample.png
@}
*/

#ifndef _IM_LTM_H_
#define _IM_LTM_H_


#include "driver_common.h"
#include "ddim_user_custom.h"
#include "jdsltm.h"

#include "ddim_typedef.h"

#if 1
#ifdef CO_DEBUG_PRINT_XX_YYY
#ifndef IM_LTM_STATUS_PRINT
#define CO_DEBUG_PRINT_IM_LTM	/**< DEBUG */
#endif
#endif
#endif
#ifdef CO_DEBUG_PRINT_IM_LTM
#ifndef IM_LTM_STATUS_PRINT
#define IM_LTM_STATUS_PRINT		/**< DEBUG */
#endif
#endif


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/** @weakgroup im_ltm
@{
*/

/* Return value of processing result */
//	D_DDIM_OK is defined with driver_common.h
#define	D_IM_LTM_PARAM_ERROR				(D_IM_LTM | D_DDIM_INPUT_PARAM_ERROR)	/**< parameter error		*/
#define	D_IM_LTM_TIMEOUT					(D_IM_LTM | D_DDIM_TIMEOUT)				/**< timeout				*/
#define	D_IM_LTM_SYSTEM_ERROR				(D_IM_LTM | D_DDIM_SYSTEM_ERROR)		/**< System call error		*/
#define	D_IM_LTM_MACRO_BUSY					(D_IM_LTM | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy error		*/

/** Function Enable
*/
#define	D_IM_LTM_ENABLE_OFF					(0)						/**< Disable						*/
#define	D_IM_LTM_ENABLE_ON					(1)						/**< Enable							*/

/** Wait Enable
*/
#define	D_IM_LTM_WAIT_OFF					(0)						/**< Wait disable.					*/
#define	D_IM_LTM_WAIT_ON					(1)						/**< Wait enable.					*/

/** Pipe No
*/
#define	D_IM_LTM_PIPE1						(0)						/**< Pipe1.							*/
#define	D_IM_LTM_PIPE2						(1)						/**< Pipe2.							*/
#define	D_IM_LTM_PIPE12						(2)						/**< Pipe1&2.						*/
#define	D_IM_LTM_PIPE_MAX					(3)						/**< Max value						*/

/** RGB/Y component to be processed
*/
#define D_IM_LTM_PORT_1						(0)						/**< Port1							*/
#define D_IM_LTM_PORT_2						(1)						/**< Port2							*/
#define D_IM_LTM_PORT_3						(2)						/**< Port3							*/
#define D_IM_LTM_PORT_4						(3)						/**< Port4							*/
#define D_IM_LTM_PORT_MAX					(4)						/**< The maximum values of RGB/Y component to be processed */

#define	D_IM_LTM_PORT_R						(D_IM_LTM_PORT_1)		/**< R data(Planar)					*/
#define	D_IM_LTM_PORT_G						(D_IM_LTM_PORT_2)		/**< G data(Planar)					*/
#define	D_IM_LTM_PORT_B						(D_IM_LTM_PORT_3)		/**< B data(Planar)					*/
#define	D_IM_LTM_PORT_RGB					(D_IM_LTM_PORT_4)		/**< RGB data(Dot)					*/
#define D_IM_LTM_PORT_Y_YP_YN				(D_IM_LTM_PORT_4)		/**< Y data(Default)				*/
#define D_IM_LTM_PORT_Y_YP					(D_IM_LTM_PORT_1)		/**< Y data							*/
#define D_IM_LTM_PORT_Y_YN					(D_IM_LTM_PORT_2)		/**< Y data							*/

#define D_IM_LTM_RBK_READ_PORT_COUNT		(3)						/**< read port's count(RBK block)	*/
#define D_IM_LTM_RBK_WRITE_PORT_COUNT		(1)						/**< write port's count(RBK block)	*/
#define D_IM_LTM_MAP_READ_PORT_COUNT		(1)						/**< read port's count(MAP block)	*/
#define D_IM_LTM_MAP_WRITE_PORT_COUNT		(2)						/**< write port's count(MAP block)	*/
#define D_IM_LTM_LTM_READ_Y_PORT_COUNT		(2)						/**< read port's count(LTM block)	*/
#define D_IM_LTM_LTM_READ_RGB_PORT_COUNT	(3)						/**< read port's count(LTM block)	*/
#define D_IM_LTM_LTM_WRITE_PORT_COUNT		(3)						/**< write port's count(LTM block)	*/

/** burst length
*/
#define	D_IM_LTM_BRST_512					(0)						/**< 512bytes burst					*/
#define	D_IM_LTM_BRST_256					(1)						/**< 256bytes burst					*/
																		/* @RAXIBSIZ.BURSTRR, RAXIBSIZ.BURSTRG, RAXIBSIZ.BURSTRB, RAXIBSIZ.BURSTW	*/

/** PORT select
*/
#define	D_IM_LTM_PORT_DEFAULT				(0)						/**< Default port(Port4)		*/
#define	D_IM_LTM_PORT_SWITCH				(1)						/**< YP(Port1), YN(Port2)		*/
																		/* @AXICHSEL.AXIWSEL1, @AXICHSEL.AXIWSEL2, AXIRSEL.AXIRSEL1, AXIRSEL.AXIRSEL2	*/

/** Input/Output data type
*/
#define	D_IM_LTM_DTYP_PACK12				(2)						/**< 12bit PACK						*/
#define	D_IM_LTM_DTYP_UNPACK12				(3)						/**< 12bit UNPACK					*/
																		/* @RIOCTL.W0DTYP	*/

/** Maximum of Table size
*/
#define D_IM_LTM_TABLE_MAX_RCG				(32)					/**< RCG table array number	*/
#define D_IM_LTM_TABLE_MAX_YLOG				(512)					/**< YLOG table array number	*/

/** YLOG table
*/
#define D_LTM_YLOG_TBL_00					(0)						/**< YLOG Table0 side0	*/
#define D_LTM_YLOG_TBL_01					(1)						/**< YLOG Table0 side1	*/
#define D_LTM_YLOG_TBL_10					(2)						/**< YLOG Table1 side0	*/
#define D_LTM_YLOG_TBL_11					(3)						/**< YLOG Table1 side1	*/

/** YLOG table side select
*/
#define	D_IM_LTM_YLOG_TBL_SIDE_0			(0)						/**< Side 0 select(YLOG0/YLOG1 table)	*/
#define	D_IM_LTM_YLOG_TBL_SIDE_1			(1)						/**< Side 1 select(YLOG0/YLOG1 table)	*/
																		/* @MYLCTL.YLOGSL, @LYLCTL.YLOGSL	*/

/** mask write enable
*/
#define	D_IM_LTM_MASK_WRITE_ENABLE			(0)						/**< Writing with mask is enabled	*/
#define	D_IM_LTM_MASK_WRITE_DISABLE			(1)						/**< Writing with mask is disabled	*/


/* @} */	// im_ltm group



/** @weakgroup im_ltm_rbk
@{
*/

/** Interrupt status(bit field)
*/
#define	D_IM_LTM_RBK1_INT_STATE_RBK_END		(0x00000001)			/**< RBK: Normal end				*/
#define	D_IM_LTM_RBK1_INT_STATE_FRAME_END	(0x00000002)			/**< Receiving the top of the frame	*/
#define	D_IM_LTM_RBK1_INT_STATE_LINE_END	(0x00000004)			/**< Line transferred				*/
#define	D_IM_LTM_RBK1_INT_STATE_WAIT_ERR	(0x00000008)			/**< Overrun error					*/
#define	D_IM_LTM_RBK1_INT_STATE_AXR_ERR		(0x00000010)			/**< AXI Read channel error			*/
#define	D_IM_LTM_RBK1_INT_STATE_AXW_ERR		(0x00000020)			/**< AXI Write channel error		*/
#define	D_IM_LTM_RBK2_INT_STATE_RBK_END		(0x00000100)			/**< RBK: Normal end				*/
#define	D_IM_LTM_RBK2_INT_STATE_FRAME_END	(0x00000200)			/**< Receiving the top of the frame	*/
#define	D_IM_LTM_RBK2_INT_STATE_LINE_END	(0x00000400)			/**< Line transferred				*/
#define	D_IM_LTM_RBK2_INT_STATE_WAIT_ERR	(0x00000800)			/**< Overrun error					*/
#define	D_IM_LTM_RBK2_INT_STATE_AXR_ERR		(0x00001000)			/**< AXI Read channel error			*/
#define	D_IM_LTM_RBK2_INT_STATE_AXW_ERR		(0x00002000)			/**< AXI Write channel error		*/

/** Interrupt flag for Im_LTM_RBK_WaitEnd() (bit field)
*/
#define	D_IM_LTM_RBK1_INT_FLG_RBK_END		(0x00000001)			/**< RBK: Normal end				*/
#define	D_IM_LTM_RBK1_INT_FLG_FRAME_END		(0x00000002)			/**< Receiving the top of the frame	*/
#define	D_IM_LTM_RBK1_INT_FLG_LINE_END		(0x00000004)			/**< Line transferred				*/
#define	D_IM_LTM_RBK1_INT_FLG_WAIT_ERR		(0x00000008)			/**< Overrun error					*/
#define	D_IM_LTM_RBK1_INT_FLG_AXR_ERR		(0x00000010)			/**< AXI Read channel error			*/
#define	D_IM_LTM_RBK1_INT_FLG_AXW_ERR		(0x00000020)			/**< AXI Write channel error		*/
#define	D_IM_LTM_RBK1_INT_FLG_ALL			(0x0000003F)			/**< All flags(PIPE1)				*/
#define	D_IM_LTM_RBK2_INT_FLG_RBK_END		(0x00000100)			/**< RBK: Normal end				*/
#define	D_IM_LTM_RBK2_INT_FLG_FRAME_END		(0x00000200)			/**< Receiving the top of the frame	*/
#define	D_IM_LTM_RBK2_INT_FLG_LINE_END		(0x00000400)			/**< Line transferred				*/
#define	D_IM_LTM_RBK2_INT_FLG_WAIT_ERR		(0x00000800)			/**< Overrun error					*/
#define	D_IM_LTM_RBK2_INT_FLG_AXR_ERR		(0x00001000)			/**< AXI Read channel error			*/
#define	D_IM_LTM_RBK2_INT_FLG_AXW_ERR		(0x00002000)			/**< AXI Write channel error		*/
#define	D_IM_LTM_RBK2_INT_FLG_ALL			(0x00003F00)			/**< All flags(PIPE2)				*/
#define	D_IM_LTM_RBK_INT_FLG_ALL			(0x00003F3F)			/**< All flags						*/

/** Color of top pixel of Bayer data
*/
#define	D_IM_LTM_RBK_TOP_PIXEL_R			(0)						/**< Bayer top pixel is R			*/
#define	D_IM_LTM_RBK_TOP_PIXEL_GR			(1)						/**< Bayer top pixel is Gr			*/
#define	D_IM_LTM_RBK_TOP_PIXEL_GB			(2)						/**< Bayer top pixel is Gb			*/
#define	D_IM_LTM_RBK_TOP_PIXEL_B			(3)						/**< Bayer top pixel is B			*/
																		/* @RIOCTL.BAYER0, RIOCTL.BAYER1	*/

/** SRO direct input mode
*/
#define	D_IM_LTM_RBK_SRO_DIRECT_1			(0)						/**< Input SRO1						*/
#define	D_IM_LTM_RBK_SRO_DIRECT_2			(1)						/**< Input SRO1&SRO2				*/
																		/* @RIOCTL.SROMD	*/

/** Select output
*/
#define	D_IM_LTM_RBK_OUT_SDRAM				(0)						/**< Output to SDRAM				*/
#define	D_IM_LTM_RBK_OUT_MAP_DIRECT			(1)						/**< Output to MAP block(Direct)	*/
																		/* @RIOCTL.OUTSEL	*/

/** Pixel average reduction mode
*/
#define	D_IM_LTM_RBK_RDC_MODE_DIV_2			(0)						/**< Reduction 1/2	*/
#define	D_IM_LTM_RBK_RDC_MODE_DIV_4			(1)						/**< Reduction 1/4	*/
#define	D_IM_LTM_RBK_RDC_MODE_DIV_8			(2)						/**< Reduction 1/8	*/
																		/* @RDCMD.RDCMD	*/

/* @} */	// im_ltm_rbk group


/** @weakgroup im_ltm_map
@{
*/

/** Interrupt status(bit field)
*/
#define	D_IM_LTM_MAP1_INT_STATE_MAP_END		(0x00000001)			/**< MAP: Normal end				*/
#define	D_IM_LTM_MAP1_INT_STATE_LINE_END	(0x00000002)			/**< Line transferred				*/
#define	D_IM_LTM_MAP1_INT_STATE_AXR_ERR		(0x00000004)			/**< AXI Read channel error			*/
#define	D_IM_LTM_MAP1_INT_STATE_AXW_ERR		(0x00000008)			/**< AXI Write channel error		*/
#define	D_IM_LTM_MAP1_INT_STATE_YLOG_ERR	(0x00000010)			/**< YLOG Table CPU access error	*/
#define	D_IM_LTM_MAP1_INT_STATE_HISTINI_END	(0x00000020)			/**< RAM initialize End				*/
#define	D_IM_LTM_MAP2_INT_STATE_MAP_END		(0x00000100)			/**< MAP: Normal end				*/
#define	D_IM_LTM_MAP2_INT_STATE_LINE_END	(0x00000200)			/**< Line transferred				*/
#define	D_IM_LTM_MAP2_INT_STATE_AXR_ERR		(0x00000400)			/**< AXI Read channel error			*/
#define	D_IM_LTM_MAP2_INT_STATE_AXW_ERR		(0x00000800)			/**< AXI Write channel error		*/
#define	D_IM_LTM_MAP2_INT_STATE_YLOG_ERR	(0x00001000)			/**< YLOG Table CPU access error	*/
#define	D_IM_LTM_MAP2_INT_STATE_HISTINI_END	(0x00002000)			/**< RAM initialize End				*/

/** Interrupt flag for Im_LTM_MAP_WaitEnd() (bit field)
*/
#define	D_IM_LTM_MAP1_INT_FLG_MAP_END		(0x00000001)			/**< MAP: Normal end				*/
#define	D_IM_LTM_MAP1_INT_FLG_LINE_END		(0x00000002)			/**< Line transferred				*/
#define	D_IM_LTM_MAP1_INT_FLG_AXR_ERR		(0x00000004)			/**< AXI Read channel error			*/
#define	D_IM_LTM_MAP1_INT_FLG_AXW_ERR		(0x00000008)			/**< AXI Write channel error		*/
#define	D_IM_LTM_MAP1_INT_FLG_YLOG_ERR		(0x00000010)			/**< YLOG Table CPU access error	*/
#define	D_IM_LTM_MAP1_INT_FLG_HISTINI_END	(0x00000020)			/**< RAM initialize End				*/
#define	D_IM_LTM_MAP1_INT_FLG_ALL			(0x0000003F)			/**< All flags(PIPE1)				*/
#define	D_IM_LTM_MAP2_INT_FLG_MAP_END		(0x00000100)			/**< MAP: Normal end				*/
#define	D_IM_LTM_MAP2_INT_FLG_LINE_END		(0x00000200)			/**< Line transferred				*/
#define	D_IM_LTM_MAP2_INT_FLG_AXR_ERR		(0x00000400)			/**< AXI Read channel error			*/
#define	D_IM_LTM_MAP2_INT_FLG_AXW_ERR		(0x00000800)			/**< AXI Write channel error		*/
#define	D_IM_LTM_MAP2_INT_FLG_YLOG_ERR		(0x00001000)			/**< YLOG Table CPU access error	*/
#define	D_IM_LTM_MAP2_INT_FLG_HISTINI_END	(0x00002000)			/**< RAM initialize End				*/
#define	D_IM_LTM_MAP2_INT_FLG_ALL			(0x00003F00)			/**< All flags(PIPE2)				*/
#define	D_IM_LTM_MAP_INT_FLG_ALL			(0x00003F3F)			/**< All flags						*/

/** Map images generation mode
*/
#define	D_IM_LTM_MAP_GENERATION_MODE_0		(0)						/**< Mode0	*/
#define	D_IM_LTM_MAP_GENERATION_MODE_1		(1)						/**< Mode1	*/
																		/* @PEPS0M, @NPEPS0M, @PEPS1M, @NPEPS1M, @PEPS2M, @NPEPS2M	*/

/* @} */	// im_ltm_map group

/** @weakgroup im_ltm_ltm
@{
*/

/** Interrupt status(bit field)
*/
#define	D_IM_LTM_LTM1_INT_STATE_LTM_END		(0x00000001)			/**< LTM: Normal end						*/
#define	D_IM_LTM_LTM1_INT_STATE_EXIP_END	(0x00000002)			/**< Received transmission starting pulse	*/
#define	D_IM_LTM_LTM1_INT_STATE_LINE_END	(0x00000004)			/**< Line transferred						*/
#define	D_IM_LTM_LTM1_INT_STATE_AXR_ERR		(0x00000008)			/**< AXI Read channel error					*/
#define	D_IM_LTM_LTM1_INT_STATE_AXW_ERR		(0x00000010)			/**< AXI Write channel error				*/
#define	D_IM_LTM_LTM1_INT_STATE_YLOG_ERR	(0x00000020)			/**< YLOG Table CPU access error			*/
#define	D_IM_LTM_LTM2_INT_STATE_LTM_END		(0x00000100)			/**< LTM: Normal end						*/
#define	D_IM_LTM_LTM2_INT_STATE_EXIP_END	(0x00000200)			/**< Received transmission starting pulse	*/
#define	D_IM_LTM_LTM2_INT_STATE_LINE_END	(0x00000400)			/**< Line transferred						*/
#define	D_IM_LTM_LTM2_INT_STATE_AXR_ERR		(0x00000800)			/**< AXI Read channel error					*/
#define	D_IM_LTM_LTM2_INT_STATE_AXW_ERR		(0x00001000)			/**< AXI Write channel error				*/
#define	D_IM_LTM_LTM2_INT_STATE_YLOG_ERR	(0x00002000)			/**< YLOG Table CPU access error			*/

/** Interrupt flag for Im_LTM_LTM_WaitEnd() (bit field)
*/
#define	D_IM_LTM_LTM1_INT_FLG_LTM_END		(0x00000001)			/**< LTM: Normal end						*/
#define	D_IM_LTM_LTM1_INT_FLG_EXIP_END		(0x00000002)			/**< Received transmission starting pulse	*/
#define	D_IM_LTM_LTM1_INT_FLG_LINE_END		(0x00000004)			/**< Line transferred						*/
#define	D_IM_LTM_LTM1_INT_FLG_AXR_ERR		(0x00000008)			/**< AXI Read channel error					*/
#define	D_IM_LTM_LTM1_INT_FLG_AXW_ERR		(0x00000010)			/**< AXI Write channel error				*/
#define	D_IM_LTM_LTM1_INT_FLG_YLOG_ERR		(0x00000020)			/**< YLOG Table CPU access error			*/
#define	D_IM_LTM_LTM1_INT_FLG_ALL			(0x0000003F)			/**< All flags(PIPE1)						*/
#define	D_IM_LTM_LTM2_INT_FLG_LTM_END		(0x00000100)			/**< LTM: Normal end						*/
#define	D_IM_LTM_LTM2_INT_FLG_EXIP_END		(0x00000200)			/**< Received transmission starting pulse	*/
#define	D_IM_LTM_LTM2_INT_FLG_LINE_END		(0x00000400)			/**< Line transferred						*/
#define	D_IM_LTM_LTM2_INT_FLG_AXR_ERR		(0x00000800)			/**< AXI Read channel error					*/
#define	D_IM_LTM_LTM2_INT_FLG_AXW_ERR		(0x00001000)			/**< AXI Write channel error				*/
#define	D_IM_LTM_LTM2_INT_FLG_YLOG_ERR		(0x00002000)			/**< YLOG Table CPU access error			*/
#define	D_IM_LTM_LTM2_INT_FLG_ALL			(0x00003F00)			/**< All flags(PIPE2)						*/
#define	D_IM_LTM_LTM_INT_FLG_ALL			(0x00003F3F)			/**< All flags								*/

/** Select output
*/
#define	D_IM_LTM_LTM_OUT_SDRAM				(0)						/**< Output to SDRAM				*/
#define	D_IM_LTM_LTM_OUT_R2Y_DIRECT			(1)						/**< Output to R2Y macro(Direct)	*/
																		/* @LIOCTL.OUTSEL	*/

/* @} */	// im_ltm_ltm group


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


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
//
/** @weakgroup im_ltm
@{
*/

/** AXI bus I/F control
*/
typedef struct {
	UCHAR					cache_type;						/**< cache type (4bits)		*/
	UCHAR					protect_type;					/**< protect type (3bits)	*/
} T_IM_LTM_CTRL_AXI_TYPE;

/** Rectangle of image
*/
typedef struct {
	USHORT					img_top;						/**< Vertical start position	*/
	USHORT					img_left;						/**< Horizontal start position	*/
	USHORT					img_width;						/**< Horizontal size			*/
	USHORT					img_lines;						/**< Vertical size				*/
} T_IM_LTM_RECT;												/* Rectangle control data	*/

/** LTM RBK/LTM input address information
*/
typedef struct {
	VOID*					addr_r;							/**< RSDRADR/LSDRADR2R : R address(32bits, multiple of 2)	*/
	VOID*					addr_g;							/**< RSDRADG/LSDRADR2G : G address(32bits, multiple of 2)	*/
	VOID*					addr_b;							/**< RSDRADB/LSDRADR2B : B address(32bits, multiple of 2)	*/
} T_IM_LTM_INADDR_INFO;

/** LTM MAP/LTM internal parameter
*/
typedef struct {
	USHORT					yshadow;						/**< YSHADOW/LYSHADOW : Internal parameter 0(14bits)	*/
	USHORT					yhighlight;						/**< YHIGHLIGHT/LYHIGHLIGHT : Internal parameter 1(14bits)	*/
} T_IM_LTM_INTERNAL_PARAM;

/** LTM MAP/LTM Ylog table control
*/
typedef struct {
	UCHAR					use_tbl;						/**< MYLCTL.YLOGSL, LYLCTL.YLOGSL : YLOG table side select(1bit)
															<ul><li>@ref D_IM_LTM_YLOG_TBL_SIDE_0
																<li>@ref D_IM_LTM_YLOG_TBL_SIDE_1</ul>			*/
	USHORT					end_point0_tbl0;				/**< MYLOG0EP0, LYLOG0EP0 : YLOG0 table end point(side0)(14bits)	*/
	USHORT					end_point0_tbl1;				/**< MYLOG0EP1, LYLOG0EP1 : YLOG0 table end point(side1)(14bits)	*/
	USHORT					end_point1_tbl0;				/**< MYLOG1EP0, LYLOG1EP0 : YLOG1 table end point(side0)(14bits)	*/
	USHORT					end_point1_tbl1;				/**< MYLOG1EP1, LYLOG1EP1 : YLOG1 table end point(side1)(14bits)	*/
} T_IM_LTM_CTRL_YLOG;

/* @} */	// im_ltm group

/** @weakgroup im_ltm_rbk
@{
*/

/** AXI bus I/F control
*/
typedef struct {
	T_IM_LTM_CTRL_AXI_TYPE	rch[D_IM_LTM_RBK_READ_PORT_COUNT];	/**< RAXICTLR1.ARCACHE_x, RAXICTLR1.ARPROT_x Read channel types.	*/
	T_IM_LTM_CTRL_AXI_TYPE	wch;								/**< RAXICTLW1.AWCACHE, RAXICTLW1.AWPROT Write channel types.		*/
} T_IM_LTM_RBK_CTRL_AXI;

/** AXI bus read channel status
*/
typedef struct {
	UCHAR					rch_axi_stat[D_IM_LTM_RBK_READ_PORT_COUNT];	/**< RAXIRERR.RRESP_x : Read channel status	*/
} T_IM_LTM_RBK_AXI_RCH_STAT;

/** AXI bus write channel status
*/
typedef struct {
	UCHAR					wch_axi_stat;					/**< RAXIWERR.WRESP : Write channel status	*/
} T_IM_LTM_RBK_AXI_WCH_STAT;

/** AXI bus ctrl param
*/
typedef struct {
	UCHAR					rch_burst_length_r;				/**< RAXIBSIZ.BURSTRR : Transfer burst length of the read channel(R).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_g;				/**< RAXIBSIZ.BURSTRG : Transfer burst length of the read channel(G).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_b;				/**< RAXIBSIZ.BURSTRB : Transfer burst length of the read channel(B).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length;				/**< RAXIBSIZ.BURSTW  : Transfer burst length of the write channel.(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					mask_write;						/**< RAXIBSIZ.WSTBMSK : Writing with mask.(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
} T_IM_LTM_RBK_AXI_CTRL_PARAM;

/** LTM RBK control
*/
typedef struct {
	USHORT					line_intr;						/**< RLINTLV.LINTLV : Monitor line count setting. (14bits)
															<p>We generate the interrupt request flag when the set value of this register.
															<p>This interrupt request, can be disabled by setting the "0000" h.	*/
	UCHAR					burst_length;					/**< RAXIBSIZ.BURSTW  : Transfer burst length of the write channel.(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					mask_write;						/**< RAXIBSIZ.WSTBMSK : Writing with mask.(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					out_select;						/**< RIOCTL.OUTSEL : Output destination select. (1bit)
															<ul><li>@ref D_IM_LTM_RBK_OUT_SDRAM
																<li>@ref D_IM_LTM_RBK_OUT_MAP_DIRECT</ul>	*/
	UCHAR					data_type;						/**< RIOCTL.W0DTYP : Output data type select(for SDRAM output). (2bits)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
	UCHAR					reduction_enable;				/**< RDCMD.RDCEN : Pixel average reduction enable.(1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : Pixel average reduction function isn't used
																<li>@ref D_IM_LTM_ENABLE_ON : Pixel average reduction function is used</ul>	*/
	UCHAR					reduction_mode;					/**< RDCMD.RDCMD : Pixel average reduction mode.(2bits)
															<ul><li>@ref D_IM_LTM_RBK_RDC_MODE_DIV_2
																<li>@ref D_IM_LTM_RBK_RDC_MODE_DIV_4
																<li>@ref D_IM_LTM_RBK_RDC_MODE_DIV_8</ul>	*/
	VOID					(*rbk_user_handler)(UINT32* int_status, UINT32 user_param);	/**< Interrupt Handler														*/
	UINT32					user_param;						/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_LTM_RBK_CTRL_COMMON;								/* LTM RBK control	*/

/** LTM RBK SDRAM input mode control
*/
typedef struct {
	UCHAR					burst_length_r;					/**< RAXIBSIZ.BURSTRR : Transfer burst length of the read channel(R).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					burst_length_g;					/**< RAXIBSIZ.BURSTRG : Transfer burst length of the read channel(G).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					burst_length_b;					/**< RAXIBSIZ.BURSTRB : Transfer burst length of the read channel(B).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	USHORT					input_global;					/**< RSDRDEF : Input horizontal Gobal area bytes(16bits, multiple of 16)	*/
	T_IM_LTM_RECT			input_rect;						/**< input image size(and trimming)	*/
	UCHAR					shift_enable;					/**< RSFEN.RSFEN : Right 4bit shift enable.(1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : Store data on lower 12bits.
																<li>@ref D_IM_LTM_ENABLE_ON : Right 4bit shift</ul>	*/
} T_IM_LTM_RBK_CTRL_SDRAM_INPUT;					/* LTM RBK SDRAM input mode control	*/

/** SRO direct connection mode control
*/
typedef struct {
	UCHAR					top_pixel0;						/**< RIOCTL.BAYER0 : Bayer data first pixel type.(2bits)
															<ul><li>@ref D_IM_LTM_RBK_TOP_PIXEL_R
																<li>@ref D_IM_LTM_RBK_TOP_PIXEL_GR
																<li>@ref D_IM_LTM_RBK_TOP_PIXEL_GB
																<li>@ref D_IM_LTM_RBK_TOP_PIXEL_B</ul>	*/
	UCHAR					top_pixel1;						/**< RIOCTL.BAYER1 : Bayer data first pixel type.(2bits)
															<ul><li>@ref D_IM_LTM_RBK_TOP_PIXEL_R
																<li>@ref D_IM_LTM_RBK_TOP_PIXEL_GR
																<li>@ref D_IM_LTM_RBK_TOP_PIXEL_GB
																<li>@ref D_IM_LTM_RBK_TOP_PIXEL_B</ul>	*/
	UCHAR					sro_mode;					/**< RIOCTL.SROMD : SRO direct input mode.(1bit)
															<ul><li>@ref D_IM_LTM_RBK_SRO_DIRECT_1
																<li>@ref D_IM_LTM_RBK_SRO_DIRECT_2</ul>	*/
	UCHAR					shift_enable0;					/**< RSFEN.RSFEN0 : Right 2bit shift enable.(1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : Store data on lower 12bits.
																<li>@ref D_IM_LTM_ENABLE_ON : Right 2bit shift</ul>	*/
	UCHAR					shift_enable1;					/**< RSFEN.RSFEN1 : Right 2bit shift enable.(1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : Store data on lower 12bits.
																<li>@ref D_IM_LTM_ENABLE_ON : Right 2bit shift</ul>	*/
	UCHAR					half_reduction_enable;			/**< HVEN.HVEN : 1/2 reduction enable.(1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : 1/2 reduction function isn't used
																<li>@ref D_IM_LTM_ENABLE_ON : 1/2 reduction function is used</ul>	*/
	T_IM_LTM_RECT			bayer_trim0;					/**< Trimming settings for SRO direct input0	*/
	T_IM_LTM_RECT			bayer_trim1;					/**< Trimming settings for SRO direct input1	*/
} T_IM_LTM_RBK_CTRL_SRO_DIRECT;						/* SRO direct connection mode control	*/

/** Trimming control
*/
typedef struct {
	UCHAR					trimming_enable;				/**< RTEN.RTEN : Trimming enable (1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : Do not do the trimming
																<li>@ref D_IM_LTM_ENABLE_ON : Trimming</ul>			*/
	T_IM_LTM_RECT			rgb_trim;						/**< Trimming settings for RGB	*/
} T_IM_LTM_RBK_RGB_TRIMMING;

/** LTM RBK output data information
*/
typedef struct {
	VOID*					addr;							/**< RSDWAD : Output images top address																		*/
	USHORT					global_width;					/**< RSDWDEF : Output horizontal Gobal area bytes(16bits, multiple of 256 or 512(Depend on burst_length))	*/
} T_IM_LTM_RBK_OUTDATA_INFO;


/* @} */	// im_ltm_rbk group

/** @weakgroup im_ltm_map
@{
*/
/** AXI bus I/F control
*/
typedef struct {
	T_IM_LTM_CTRL_AXI_TYPE	rch;								/**< MAXICTLR.ARCACHE, MAXICTLR.ARPROT Read channel types.			*/
	T_IM_LTM_CTRL_AXI_TYPE	wch[D_IM_LTM_MAP_WRITE_PORT_COUNT];	/**< MAXICTLW.AWCACHE_x, MAXICTLW.AWPROT_x Write channel types.		*/
} T_IM_LTM_MAP_CTRL_AXI;

/** AXI bus read channel status
*/
typedef struct {
	UCHAR					rch_axi_stat;					/**< MAXIRERR.RRESP : Read channel status	*/
} T_IM_LTM_MAP_AXI_RCH_STAT;

/** AXI bus write channel status
*/
typedef struct {
	UCHAR					wch_axi_stat[D_IM_LTM_MAP_WRITE_PORT_COUNT];	/**< MAXIWERR.WRESP_x : Write channel status	*/
} T_IM_LTM_MAP_AXI_WCH_STAT;

/** AXI bus ctrl param
*/
typedef struct {
	UCHAR					rch_burst_length;				/**< MAXIBSIZ.BURSTR : Transfer burst length of the read channel.(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_yp;			/**< MAXIBSIZ.BURSTWYP : Transfer burst length of the write channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_yn;			/**< MAXIBSIZ.BURSTWYN : Transfer burst length of the write channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					mask_write_yp;					/**< MAXIBSIZ.WSTBMSKYP : Writing with mask(YP).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					mask_write_yn;					/**< MAXIBSIZ.WSTBMSKYN : Writing with mask(YN).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					write_select_yp;				/**< AXICHSEL.AXIWSEL1 : Select the master channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					write_select_yn;				/**< AXICHSEL.AXIWSEL2 : Select the master channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
} T_IM_LTM_MAP_AXI_CTRL_PARAM;

/** LTM MAP control
*/
typedef struct {
	USHORT					line_intr;						/**< MLINTLV.LINTLV : Monitor line count setting. (10bits)
															<p>We generate the interrupt request flag when the set value of this register.
															<p>This interrupt request, can be disabled by setting the "000" h.	*/
	UCHAR					wch_burst_length_yp;			/**< MAXIBSIZ.BURSTWYP : Transfer burst length of the write channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_yn;			/**< MAXIBSIZ.BURSTWYN : Transfer burst length of the write channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					mask_write_yp;					/**< MAXIBSIZ.WSTBMSKYP : Writing with mask(YP).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					mask_write_yn;					/**< MAXIBSIZ.WSTBMSKYN : Writing with mask(YN).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					write_select_yp;				/**< AXICHSEL.AXIWSEL1 : Select the master channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_PORT_DEFAULT
																<li>@ref D_IM_LTM_PORT_SWITCH</ul>	*/
	UCHAR					write_select_yn;				/**< AXICHSEL.AXIWSEL2 : Select the master channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_PORT_DEFAULT
																<li>@ref D_IM_LTM_PORT_SWITCH</ul>	*/
	UCHAR					data_type_yp;					/**< MIOCTL.W0DTYP : Output data type select(YP)(for SDRAM output). (2bits)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
	UCHAR					data_type_yn;					/**< MIOCTL.W1DTYP : Output data type select(YN)(for SDRAM output). (2bits)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
	VOID					(*map_user_handler)(UINT32* int_status, UINT32 user_param);	/**< Interrupt Handler														*/
	UINT32					user_param;						/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_LTM_MAP_CTRL_COMMON;								/* LTM MAP control	*/

/** LTM MAP SDRAM input mode control
*/
typedef struct {
	UCHAR					burst_length;					/**< MAXIBSIZ.BURSTR : Transfer burst length of the read channel.(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	USHORT					input_global;					/**< MSDRDEF : Input horizontal Gobal area bytes(16bits, multiple of 16)	*/
	T_IM_LTM_RECT			input_rect;						/**< input image size(and trimming)	*/
	UCHAR					data_type;						/**< MIOCTL.R1DTYP : Input data type select. (2bits)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
} T_IM_LTM_MAP_CTRL_SDRAM_INPUT;					/* LTM MAP SDRAM input mode control	*/

/** LTM MAP input data information
*/
typedef struct {
	VOID*					addr;							/**< MSDRAD : Intput images top address.*/
	UCHAR					top_bit_offset;					/**< MSDROFS : Setting top bit input data area (1bit)<br>
															<p>Set the first bit of the data area.
															   Set the position of the LSB bit in bit position.<br>
															   However, when you select 16-bit in input_dtype(MIOCTL.R1DTYP) is 0.
															   Only when you select a 12-bit pack, please select the setting of 0 or 1.
															<p>exp)<br>
															   I would like the following figure, data and each are stored in 12-bit pack.<br>
															   Assuming that the first pixel data3 at this time,
															   since the position of the LSB and bit 4 data3 address, I set 1.
															   @image html im_ltm_sdrofs_exp.png
															*/
} T_IM_LTM_MAP_INDATA_INFO;

/** LTM MAP output data information
*/
typedef struct {
	VOID*					addr_yp;						/**< MSDWADW0 : Output images top address(YP)									*/
	USHORT					global_width_yp;				/**< MSDWDEFW0 : Output horizontal Gobal area bytes(YP)(16bits, multiple of 8)	*/
	VOID*					addr_yn;						/**< MSDWADW1 : Output images top address(YN)									*/
	USHORT					global_width_yn;				/**< MSDWDEFW1 : Output horizontal Gobal area bytes(YN)(16bits, multiple of 8)	*/
} T_IM_LTM_MAP_OUTDATA_INFO;

/** LTM MAP WB gain control
*/
typedef struct {
	USHORT					gain_r;							/**< GAINR : WB operation R gain(gain_r / 256)(11bits)	*/
	USHORT					gain_g;							/**< GAING : WB operation G gain(gain_g / 256)(11bits)	*/
	USHORT					gain_b;							/**< GAINB : WB operation B gain(gain_b / 256)(11bits)	*/
	USHORT					clip_r;							/**< GAINCPR : WB operation R clip value(12bits)		*/
	USHORT					clip_g;							/**< GAINCPG : WB operation G clip value(12bits)		*/
	USHORT					clip_b;							/**< GAINCPB : WB operation B clip value(12bits)		*/
} T_IM_LTM_MAP_GAIN;

/** LTM MAP Map image Generation mode control
*/
typedef struct {
	UCHAR					mode0;							/**< PEPS0M.PEPS0M / NEPS0M.NEPS0M : Generation mode(1bit)
															<ul><li>@ref D_IM_LTM_MAP_GENERATION_MODE_0
																<li>@ref D_IM_LTM_MAP_GENERATION_MODE_1</ul>				*/
	USHORT					coef0;							/**< PEPS0OF.PEPS0OF_3 : Generation coefficient(12bits)	*/
	UCHAR					mode1;							/**< PEPS1M.PEPS1M / NEPS1M.NEPS1M : Generation mode(1bit)
															<ul><li>@ref D_IM_LTM_MAP_GENERATION_MODE_0
																<li>@ref D_IM_LTM_MAP_GENERATION_MODE_1</ul>				*/
	USHORT					coef1;							/**< PEPS1OF.PEPS1OF_3 : Generation coefficient(12bits)	*/
	UCHAR					mode2;							/**< PEPS2M.PEPS2M / NEPS2M.NEPS2M : Generation mode(1bit)
															<ul><li>@ref D_IM_LTM_MAP_GENERATION_MODE_0
																<li>@ref D_IM_LTM_MAP_GENERATION_MODE_1</ul>				*/
	USHORT					coef2;							/**< PEPS2OF.PEPS2OF_3 : Generation coefficient(12bits)	*/
} T_IM_LTM_MAP_CTRL_GENERATION_MODE;

/* @} */	// im_ltm_map group

/** @weakgroup im_ltm_ltm
@{
*/
/** AXI bus I/F control
*/
typedef struct {
	T_IM_LTM_CTRL_AXI_TYPE	rch_rgb[D_IM_LTM_LTM_READ_RGB_PORT_COUNT];	/**< LAXICTLR1.ARCACHE_x, LAXICTLR1.ARPROT_x Read channel types.	*/
	T_IM_LTM_CTRL_AXI_TYPE	rch_y[D_IM_LTM_LTM_READ_Y_PORT_COUNT];		/**< LAXICTLR2.ARCACHE_x, LAXICTLR2.ARPROT_x Read channel types.	*/
	T_IM_LTM_CTRL_AXI_TYPE	wch[D_IM_LTM_LTM_WRITE_PORT_COUNT];			/**< LAXICTLW.AWCACHE_x, LAXICTLW.AWPROT_x Write channel types.		*/
} T_IM_LTM_LTM_CTRL_AXI;

/** AXI bus read channel status
*/
typedef struct {
	UCHAR					rch_rgb_axi_stat[D_IM_LTM_LTM_READ_RGB_PORT_COUNT];		/**< LAXIRERR.RRESP_R/G/B : Read channel status	*/
	UCHAR					rch_y_axi_stat[D_IM_LTM_LTM_READ_Y_PORT_COUNT];			/**< LAXIRERR.RRESP_YP/YN : Read channel status	*/
} T_IM_LTM_LTM_AXI_RCH_STAT;

/** AXI bus write channel status
*/
typedef struct {
	UCHAR					wch_axi_stat[D_IM_LTM_LTM_WRITE_PORT_COUNT];	/**< LAXIWERR.WRESP_x : Write channel status	*/
} T_IM_LTM_LTM_AXI_WCH_STAT;

/** AXI bus ctrl param
*/
typedef struct {
	UCHAR					rch_burst_length_r;				/**< LAXIBSIZ.BURSTRR : Transfer burst length of the read channel(R).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_g;				/**< LAXIBSIZ.BURSTRG : Transfer burst length of the read channel(G).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_b;				/**< LAXIBSIZ.BURSTRB : Transfer burst length of the read channel(B).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_yp;			/**< LAXIBSIZ.BURSTRYP : Transfer burst length of the read channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_yn;			/**< LAXIBSIZ.BURSTRYN : Transfer burst length of the read channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_r;				/**< LAXIBSIZ.BURSTWR : Transfer burst length of the write channel(R).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_g;				/**< LAXIBSIZ.BURSTWG : Transfer burst length of the write channel(G).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_b;				/**< LAXIBSIZ.BURSTWB : Transfer burst length of the write channel(B).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					mask_write_r;					/**< LAXIBSIZ.WSTBMSKR : Writing with mask(R).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					mask_write_g;					/**< LAXIBSIZ.WSTBMSKG : Writing with mask(G).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					mask_write_b;					/**< LAXIBSIZ.WSTBMSKB : Writing with mask(B).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					read_select_yp;					/**< AXIRSEL.AXIRSEL1 : Select the master channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_PORT_DEFAULT
																<li>@ref D_IM_LTM_PORT_SWITCH</ul>	*/
	UCHAR					read_select_yn;					/**< AXIRSEL.AXIRSEL2 : Select the master channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_PORT_DEFAULT
																<li>@ref D_IM_LTM_PORT_SWITCH</ul>	*/
} T_IM_LTM_LTM_AXI_CTRL_PARAM;

/** LTM MAP control
*/
typedef struct {
	USHORT					line_intr;						/**< LLINTLV.LINTLV : Monitor line count setting. (14bits)
															<p>We generate the interrupt request flag when the set value of this register.
															<p>This interrupt request, can be disabled by setting the "0000" h.	*/
	UCHAR					rch_burst_length_yp;			/**< LAXIBSIZ.BURSTRYP : Transfer burst length of the read channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_yn;			/**< LAXIBSIZ.BURSTRYN : Transfer burst length of the read channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_r;				/**< LAXIBSIZ.BURSTWR : Transfer burst length of the write channel(R).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_g;				/**< LAXIBSIZ.BURSTWG : Transfer burst length of the write channel(G).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					wch_burst_length_b;				/**< LAXIBSIZ.BURSTWB : Transfer burst length of the write channel(B).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					mask_write_r;					/**< LAXIBSIZ.WSTBMSKR : Writing with mask(R).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					mask_write_g;					/**< LAXIBSIZ.WSTBMSKG : Writing with mask(G).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					mask_write_b;					/**< LAXIBSIZ.WSTBMSKB : Writing with mask(B).(1bit)
															<ul><li>@ref D_IM_LTM_MASK_WRITE_ENABLE
																<li>@ref D_IM_LTM_MASK_WRITE_DISABLE</ul>	*/
	UCHAR					read_select_yp;					/**< AXIRSEL.AXIRSEL1 : Select the master channel(YP).(1bit)
															<ul><li>@ref D_IM_LTM_PORT_DEFAULT
																<li>@ref D_IM_LTM_PORT_SWITCH</ul>	*/
	UCHAR					read_select_yn;					/**< AXIRSEL.AXIRSEL2 : Select the master channel(YN).(1bit)
															<ul><li>@ref D_IM_LTM_PORT_DEFAULT
																<li>@ref D_IM_LTM_PORT_SWITCH</ul>	*/
	UCHAR					in_data_type_yp;				/**< LIOCTL.R3DTYP : Select the input data type(YP).(1bit)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
	UCHAR					in_data_type_yn;				/**< LIOCTL.R4DTYP : Select the input data type(YN).(1bit)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
	UCHAR					out_select;						/**< LIOCTL.OUTSEL : Output destination select.(1bit)
															<ul><li>@ref D_IM_LTM_LTM_OUT_SDRAM	
																<li>@ref D_IM_LTM_LTM_OUT_R2Y_DIRECT</ul>	*/
	UCHAR					out_data_type;					/**< LIOCTL.W1DTYP : Select the output data type.(1bit)
															<ul><li>@ref D_IM_LTM_DTYP_PACK12
																<li>@ref D_IM_LTM_DTYP_UNPACK12</ul>	*/
	T_IM_LTM_RECT			input_rect;						/**< input image size(and trimming) for MAP images(YP/YN)	*/
	USHORT					global_width_yp;				/**< LSDWDEFR3 : Input horizontal Gobal area bytes(YP)(16bits, multiple of 16)	*/
	USHORT					global_width_yn;				/**< LSDWDEFR4 : Input horizontal Gobal area bytes(YN)(16bits, multiple of 16)	*/
	VOID					(*ltm_user_handler)(UINT32* int_status, UINT32 user_param);	/**< Interrupt Handler														*/
	UINT32					user_param;						/**< This set value will return to callback argument value when interrupt occurs.	*/
} T_IM_LTM_LTM_CTRL_COMMON;								/* LTM MAP control	*/

/** LTM LTM SDRAM input mode control
*/
typedef struct {
	UCHAR					rch_burst_length_r;				/**< LAXIBSIZ.BURSTRR : Transfer burst length of the read channel(R).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_g;				/**< LAXIBSIZ.BURSTRG : Transfer burst length of the read channel(G).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	UCHAR					rch_burst_length_b;				/**< LAXIBSIZ.BURSTRB : Transfer burst length of the read channel(B).(1bit)
															<ul><li>@ref D_IM_LTM_BRST_512
																<li>@ref D_IM_LTM_BRST_256</ul>				*/
	USHORT					input_global_rgb;				/**< LSDRDEFR2 : Input rgb image's horizontal Gobal area bytes(16bits, multiple of 16)	*/
	T_IM_LTM_RECT			input_rect;						/**< input image size(and trimming) for RGB images	*/
} T_IM_LTM_LTM_CTRL_SDRAM_INPUT;					/* LTM MAP SDRAM input mode control	*/

/** LTM MAP input data information
*/
typedef struct {
	VOID*					addr_yp;						/**< LSDRADR3 : Intput images top address(YP).*/
	VOID*					addr_yn;						/**< LSDRADR4 : Intput images top address(YN).*/
	UCHAR					top_bit_offset_yp;				/**< LSDROFS.SDROFSR3 : Setting top bit input data area(YP) (1bit)<br>
															<p>Set the first bit of the data area.
															   Set the position of the LSB bit in bit position.<br>
															   However, when you select 16-bit in input_dtype(LIOCTL.R3DTYP) is 0.
															   Only when you select a 12-bit pack, please select the setting of 0 or 1.
															<p>exp)<br>
															   I would like the following figure, data and each are stored in 12-bit pack.<br>
															   Assuming that the first pixel data3 at this time,
															   since the position of the LSB and bit 4 data3 address, I set 1.
															   @image html im_ltm_sdrofs_exp.png
															*/
	UCHAR					top_bit_offset_yn;				/**< LSDROFS.SDROFSR4 : Setting top bit input data area(YN) (1bit)<br>
															<p>Set the first bit of the data area.
															   Set the position of the LSB bit in bit position.<br>
															   However, when you select 16-bit in input_dtype(LIOCTL.R4DTYP) is 0.
															   Only when you select a 12-bit pack, please select the setting of 0 or 1.
															<p>exp)<br>
															   I would like the following figure, data and each are stored in 12-bit pack.<br>
															   Assuming that the first pixel data3 at this time,
															   since the position of the LSB and bit 4 data3 address, I set 1.
															   @image html im_ltm_sdrofs_exp.png
															*/
} T_IM_LTM_LTM_INDATA_INFO;

/** LTM LTM output data information
*/
typedef struct {
	VOID*					addr[D_IM_LTM_LTM_WRITE_PORT_COUNT];			/**< LSDWADWR/LSDWADWG/LSDWADWB : Output images top address						*/
	USHORT					global_width[D_IM_LTM_LTM_WRITE_PORT_COUNT];	/**< LSDWDEFR/LSDWDEFG/LSDWDEFB : Output horizontal Gobal area bytes(16bits, multiple of 8)	*/
} T_IM_LTM_LTM_OUTDATA_INFO;

/** B2R direct connection mode control
*/
typedef struct {
	T_IM_LTM_RECT			input_rect;						/**< input image size for RGB images	*/
	UCHAR					bypass;							/**< LIOCTL.BYPASS : bypass mode.(1bit)
															<ul><li>@ref D_IM_LTM_ENABLE_OFF : Normal operation
																<li>@ref D_IM_LTM_ENABLE_ON : Bypass</ul>	*/
} T_IM_LTM_LTM_CTRL_B2R_DIRECT;						/* B2R direct connection mode control	*/


/** resize pitch control
*/
typedef struct {
	UINT32					hsta;							/**< LHSTA : Horizontal start position(25bits)	*/
	USHORT					hpit;							/**< LHPIT : Horizontal pitch size(15bits)		*/
	UINT32					vsta;							/**< LVSTA : Vertical start position(25bits)	*/
	USHORT					vpit;							/**< LVPIT : Vertical pitch size(15bits)		*/
} T_IM_LTM_LTM_MAP_RESIZE_PITCH;

/** High frequency strength control
*/
typedef struct {
	USHORT					strength;						/**< NLVTHOF7, PLVTHOF7 : High frequency strength(13bits, 0~4096)	*/
} T_IM_LTM_LTM_CTRL_HF_STRENGTH;

/** Blend control
*/
typedef struct {
	UCHAR					ratio;							/**< BLDYRT : Blend ratio(4bits, 0~8)		*/
	USHORT					offset[8];						/**< BLDOF.BLDOF_x : Offset(13bits, 0~4096)	*/
	SHORT					gain[8];						/**< BLDGA.BLDGA_x : Gain(15bits)			*/
	USHORT					border[8];						/**< BLDBD.BLDBD_x : Border(12bits)			*/
	UCHAR					mode;							/**< LTMMD : Mode(1bit)						*/
} T_IM_LTM_LTM_CTRL_BLEND;

/** Chroma correction strength control
*/
typedef struct {
	USHORT					offset[8];						/**< CGOF.CGOF_x : Offset(13bits, 0~4096)	*/
	SHORT					gain[8];						/**< CGGA.CGGA_x : Gain(13bits)				*/
	USHORT					border[7];						/**< CGBD.CGBD_x : Border(12bits)			*/
} T_IM_LTM_LTM_CTRL_CHROMA;


/* @} */	// im_ltm_map group



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
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special
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
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/** @weakgroup im_ltm
@{
*/

/**
Enable PCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_On_Pclk( UCHAR pipe_no );

/**
Disable PCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Off_Pclk( UCHAR pipe_no );

/**
Force disable PCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Force_Off_Pclk( UCHAR pipe_no );

/**
Enable HCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_On_Hclk( UCHAR pipe_no );

/**
Disable HCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Off_Hclk( UCHAR pipe_no );

/**
Force disable HCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Force_Off_Hclk( UCHAR pipe_no );

/**
Enable ACLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_On_Aclk( UCHAR pipe_no );

/**
Disable ACLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Off_Aclk( UCHAR pipe_no );

/**
Force disable ACLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Force_Off_Aclk( UCHAR pipe_no );

/**
Enable LTMCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_On_Clk( UCHAR pipe_no );

/**
Disable LTMCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Off_Clk( UCHAR pipe_no );

/**
Force disable LTMCLK for LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Force_Off_Clk( UCHAR pipe_no );

/**
Enable LTMRBKCLK for LTMRBK/LTMMAP
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_On_RBKClk( UCHAR pipe_no );

/**
Disable LTMRBKCLK for LTMRBK/LTMMAP
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Off_RBKClk( UCHAR pipe_no );

/**
Force disable LTMRBKCLK for LTMRBK/LTMMAP
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		For debugging use only (for register dump on debugger)
*/
extern INT32 Im_LTM_Force_Off_RBKClk( UCHAR pipe_no );

/**
LTM All block(RBK/MAP/LTM) Initialize
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_Init_All_Block( UCHAR pipe_no );

/**
LTM All block(RBK/MAP/LTM) SW reset
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_LTM_SW_Reset_All_Block( UCHAR pipe_no );

/**
Stop LTM(All block)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_Stop_All_Block( UCHAR pipe_no );

/**
Interrupt handler.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
*/
extern VOID Im_LTM_Int_Handler( UCHAR pipe_no );
/* @} */	// im_ltm group


/** @weakgroup im_ltm_rbk
@{
*/

/**
LTM RBK Initialize
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Init( UCHAR pipe_no );

/**
LTM RBK SW reset
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_LTM_RBK_SW_Reset( UCHAR pipe_no );

/**
LTM RBK set AXI bus I/F Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ctrl_axi				: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Ctrl_Axi( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_AXI* const ctrl_axi );

/**
get AXI bus read channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_read_stat			: Status of AXI bus read channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Get_AxiReadStat( UCHAR pipe_no, T_IM_LTM_RBK_AXI_RCH_STAT* const axi_read_stat );

/**
get AXI bus write channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_write_stat			: Status of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Get_AxiWriteStat( UCHAR pipe_no, T_IM_LTM_RBK_AXI_WCH_STAT* const axi_write_stat );

/**
get AXI bus ctrl param
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_ctrl_prm			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Get_AxiCtrlParam( UCHAR pipe_no, T_IM_LTM_RBK_AXI_CTRL_PARAM* const axi_ctrl_prm );

/**
set AXI bus ctrl param
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		axi_ctrl_prm			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Set_AxiCtrlParam( UCHAR pipe_no, const T_IM_LTM_RBK_AXI_CTRL_PARAM* const axi_ctrl_prm );

/**
LTM RBK Control (each mode common control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		rbk_ctrl				: Control information for Common.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Ctrl_Common( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_COMMON* const rbk_ctrl );

/**
LTM RBK Control (SDRAM input mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		rbk_ctrl_sdram_input	: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_SDRAM_INPUT* const rbk_ctrl_sdram_input );

/**
Set Managed Input Address information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		in_addr					: Input address information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Set_InAddr_Info( UCHAR pipe_no, const T_IM_LTM_INADDR_INFO* const in_addr );

/**
Set Managed Output data information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		out_data_info			: Output data information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Set_OutData_Info( UCHAR pipe_no, const T_IM_LTM_RBK_OUTDATA_INFO* const out_data_info );

/**
LTM RBK Control (SRO direct mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		rbk_ctrl_sro_direct		: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Ctrl_ModeSRODirect( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_SRO_DIRECT* const rbk_ctrl_sro_direct );

/**
Resolution conversion control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		conversion_on_off		: Resolution conversion on off.(0:off 1:1/2 reduction)
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Ctrl_Resolution_Conversion( UCHAR pipe_no, const UCHAR conversion_on_off );

/**
Set Frame stop
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		onoff					: 0 : Frame stop disable / 1 : Frame stop enable
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Set_FrameStop( UCHAR pipe_no, UCHAR onoff );

/**
Set parameter hold
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		hold_enable				: Parameter hold enable/disable
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Set_ParamHold( UCHAR pipe_no, const UCHAR hold_enable );

/**
Get LTM RBK busy status.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		busy_status				: TRUE : RBK is running. (RTRG is on). / FALSE : RBK is not running. (RTRG is off).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Get_RbkBusy( UCHAR pipe_no, BOOL* const busy_status );

/**
Restart RBK process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_ContStart( UCHAR pipe_no );

/**
start RBK process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_LTM_RBK_Start( UCHAR pipe_no );

/**
Stop RBK
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Stop( UCHAR pipe_no );

/**
wait end of LTM RBK.
@param[in]		waiptn					: waiting end cause. (bit field)
										<ul><li>@ref D_IM_LTM_RBK1_INT_FLG_RBK_END
											<li>@ref D_IM_LTM_RBK1_INT_FLG_FRAME_END
											<li>@ref D_IM_LTM_RBK1_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_RBK1_INT_FLG_WAIT_ERR
											<li>@ref D_IM_LTM_RBK1_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_RBK1_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_RBK2_INT_FLG_RBK_END
											<li>@ref D_IM_LTM_RBK2_INT_FLG_FRAME_END
											<li>@ref D_IM_LTM_RBK2_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_RBK2_INT_FLG_WAIT_ERR
											<li>@ref D_IM_LTM_RBK2_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_RBK2_INT_FLG_AXW_ERR</ul>
@param[in]		tmout					: timeout.
@param[out]		p_flgptn				: end cause. (bit field)
										<ul><li>@ref D_IM_LTM_RBK1_INT_FLG_RBK_END
											<li>@ref D_IM_LTM_RBK1_INT_FLG_FRAME_END
											<li>@ref D_IM_LTM_RBK1_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_RBK1_INT_FLG_WAIT_ERR
											<li>@ref D_IM_LTM_RBK1_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_RBK1_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_RBK2_INT_FLG_RBK_END
											<li>@ref D_IM_LTM_RBK2_INT_FLG_FRAME_END
											<li>@ref D_IM_LTM_RBK2_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_RBK2_INT_FLG_WAIT_ERR
											<li>@ref D_IM_LTM_RBK2_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_RBK2_INT_FLG_AXW_ERR</ul>
@retval			D_DDIM_OK				: RBK normal end. and set end cause to p_flgptn.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_LTM_TIMEOUT		: wait timeout.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_LTM_RBK_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout );

/**
Interrupt handler.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_LTM_RBK_Int_Handler( UCHAR pipe_no );

/**
RGB Trimming control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		rbk_trimming			: pointer of bayer trimming control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_RBK_Ctrl_RGBTrimming( UCHAR pipe_no, const T_IM_LTM_RBK_RGB_TRIMMING* const rbk_trimming );

/* @} */	// im_ltm_rbk group

/** @weakgroup im_ltm_map
@{
*/

/**
LTM MAP Initialize
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Init( UCHAR pipe_no );

/**
LTM MAP SW reset
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_LTM_MAP_SW_Reset( UCHAR pipe_no );

/**
LTM MAP set AXI bus I/F Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ctrl_axi				: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Ctrl_Axi( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_AXI* const ctrl_axi );

/**
get AXI bus read channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_read_stat			: Status of AXI bus read channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Get_AxiReadStat( UCHAR pipe_no, T_IM_LTM_MAP_AXI_RCH_STAT* const axi_read_stat );

/**
get AXI bus write channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_write_stat			: Status of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Get_AxiWriteStat( UCHAR pipe_no, T_IM_LTM_MAP_AXI_WCH_STAT* const axi_write_stat );

/**
get AXI bus ctrl param
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_ctrl_prm			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Get_AxiCtrlParam( UCHAR pipe_no, T_IM_LTM_MAP_AXI_CTRL_PARAM* const axi_ctrl_prm );

/**
set AXI bus ctrl param
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		axi_ctrl_prm			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_AxiCtrlParam( UCHAR pipe_no, const T_IM_LTM_MAP_AXI_CTRL_PARAM* const axi_ctrl_prm );

/**
LTM MAP Control (each mode common control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		map_ctrl				: Control information for Common.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Ctrl_Common( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_COMMON* const map_ctrl );

/**
LTM MAP Control (SDRAM input mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		map_ctrl_sdram_input	: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_SDRAM_INPUT* const map_ctrl_sdram_input );

/**
Set Managed Input data information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		in_data_info			: Input data information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_InData_Info( UCHAR pipe_no, const T_IM_LTM_MAP_INDATA_INFO* const in_data_info );

/**
Set Managed Output data information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		out_data_info			: Output data information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_OutData_Info( UCHAR pipe_no, const T_IM_LTM_MAP_OUTDATA_INFO* const out_data_info );

/**
LTM MAP Control (RBK direct mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Ctrl_ModeRBKDirect( UCHAR pipe_no );

/**
Set Frame stop
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		onoff					: 0 : Frame stop disable / 1 : Frame stop enable
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_FrameStop( UCHAR pipe_no, UCHAR onoff );

/**
Set parameter hold
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		hold_enable				: Parameter hold enable/disable
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_ParamHold( UCHAR pipe_no, const UCHAR hold_enable );

/**
Get LTM MAP busy status.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		busy_status				: TRUE : MAP is running. (MTRG is on). / FALSE : MAP is not running. (MTRG is off).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Get_MapBusy( UCHAR pipe_no, BOOL* const busy_status );

/**
Restart MAP process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_ContStart( UCHAR pipe_no );

/**
start MAP process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_LTM_MAP_Start( UCHAR pipe_no );

/**
Stop MAP
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Stop( UCHAR pipe_no );

/**
wait end of LTM MAP.
@param[in]		waiptn					: waiting end cause. (bit field)
										<ul><li>@ref D_IM_LTM_MAP1_INT_FLG_MAP_END
											<li>@ref D_IM_LTM_MAP1_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_MAP1_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_MAP1_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_MAP1_INT_FLG_YLOG_ERR
											<li>@ref D_IM_LTM_MAP2_INT_FLG_MAP_END
											<li>@ref D_IM_LTM_MAP2_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_MAP2_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_MAP2_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_MAP2_INT_FLG_YLOG_ERR</ul>
@param[in]		tmout					: timeout.
@param[out]		p_flgptn				: end cause. (bit field)
										<ul><li>@ref D_IM_LTM_MAP1_INT_FLG_MAP_END
											<li>@ref D_IM_LTM_MAP1_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_MAP1_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_MAP1_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_MAP1_INT_FLG_YLOG_ERR
											<li>@ref D_IM_LTM_MAP2_INT_FLG_MAP_END
											<li>@ref D_IM_LTM_MAP2_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_MAP2_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_MAP2_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_MAP2_INT_FLG_YLOG_ERR</ul>
@retval			D_DDIM_OK				: MAP normal end. and set end cause to p_flgptn.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_LTM_TIMEOUT		: wait timeout.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_LTM_MAP_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout );

/**
Interrupt handler.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_LTM_MAP_Int_Handler( UCHAR pipe_no );

/**
WB Gain control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		wb_gain					: pointer of WB gain parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_WB_Gain( UCHAR pipe_no, const T_IM_LTM_MAP_GAIN* const wb_gain );

/**
Map image Generation mode control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		mode					: pointer of generation mode control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Ctrl_Generation_Mode( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_GENERATION_MODE* const mode );

/**
Get LTM MAP internal parameters.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		param					: Internal parameter information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Get_Internal_Param( UCHAR pipe_no, T_IM_LTM_INTERNAL_PARAM* const param );

/**
YLOG table control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ylog_ctrl				: pointer of YLOG table control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Ctrl_Ylog_Tbl( UCHAR pipe_no, const T_IM_LTM_CTRL_YLOG* const ylog_ctrl );

/**
Get active YLOG table No.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		tbl_no					: Active YLOG table No.
											<ul><li>@ref D_IM_LTM_YLOG_TBL_SIDE_0
												<li>@ref D_IM_LTM_YLOG_TBL_SIDE_1</ul>
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Get_Active_Ylog_Tbl_No( UCHAR pipe_no, UCHAR* const tbl_no );

/**
Set YLOG table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_sel					: Select the YLOG table.
											<ul><li>@ref D_LTM_YLOG_TBL_00
												<li>@ref D_LTM_YLOG_TBL_01
												<li>@ref D_LTM_YLOG_TBL_10
												<li>@ref D_LTM_YLOG_TBL_11</ul>
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_LTM_TABLE_MAX_YLOG)
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_MAP_Set_YLOG_Table( UCHAR pipe_no, UCHAR tbl_sel, const USHORT* const src_tbl );

/* @} */	// im_ltm_map group


/** @weakgroup im_ltm_ltm
@{
*/

/**
LTM LTM Initialize
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Init( UCHAR pipe_no );

/**
LTM LTM SW reset
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_MACRO_BUSY		: Macro busy error.
*/
extern INT32 Im_LTM_LTM_SW_Reset( UCHAR pipe_no );

/**
LTM LTM set AXI bus I/F Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ctrl_axi				: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_Axi( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_AXI* const ctrl_axi );

/**
get AXI bus read channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_read_stat			: Status of AXI bus read channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Get_AxiReadStat( UCHAR pipe_no, T_IM_LTM_LTM_AXI_RCH_STAT* const axi_read_stat );

/**
get AXI bus write channel status
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_write_stat			: Status of AXI bus write channel.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Get_AxiWriteStat( UCHAR pipe_no, T_IM_LTM_LTM_AXI_WCH_STAT* const axi_write_stat );

/**
get AXI bus ctrl param
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		axi_ctrl_prm			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Get_AxiCtrlParam( UCHAR pipe_no, T_IM_LTM_LTM_AXI_CTRL_PARAM* const axi_ctrl_prm );

/**
set AXI bus ctrl param
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		axi_ctrl_prm			: Control information for AXI bus.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_AxiCtrlParam( UCHAR pipe_no, const T_IM_LTM_LTM_AXI_CTRL_PARAM* const axi_ctrl_prm );

/**
LTM LTM Control (each mode common control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ltm_ctrl				: Control information for Common.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_Common( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_COMMON* const ltm_ctrl );

/**
LTM LTM Control (SDRAM input mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ltm_ctrl_sdram_input	: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_SDRAM_INPUT* const ltm_ctrl_sdram_input );

/**
Set Managed Input Address information(Main RGB images)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		in_addr					: Input data information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_InAddr_Info_Rgb( UCHAR pipe_no, const T_IM_LTM_INADDR_INFO* const in_addr );

/**
Set Managed Input data information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		in_data_info			: Input data information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_InData_Info( UCHAR pipe_no, const T_IM_LTM_LTM_INDATA_INFO* const in_data_info );

/**
Set Managed Output data information
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		out_data_info			: Output data information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_OutData_Info( UCHAR pipe_no, const T_IM_LTM_LTM_OUTDATA_INFO* const out_data_info );

/**
LTM LTM Control (B2R direct mode control)
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ltm_ctrl_b2r_direct		: Control information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_ModeB2RDirect( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_B2R_DIRECT* const ltm_ctrl_b2r_direct );

/**
LTM LTM resize pitch Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( UCHAR pipe_no );

/**
LTM LTM resize pitch Control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		resize_pitch			: Resize information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_MapImage_Resize_Pitch( UCHAR pipe_no, const T_IM_LTM_LTM_MAP_RESIZE_PITCH* const resize_pitch );

/**
Set Frame stop
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		onoff					: 0 : Frame stop disable / 1 : Frame stop enable
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_FrameStop( UCHAR pipe_no, UCHAR onoff );

/**
Set parameter hold
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		hold_enable				: Parameter hold enable/disable
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_ParamHold( UCHAR pipe_no, const UCHAR hold_enable );

/**
Get LTM LTM busy status.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@param[out]		busy_status				: TRUE : LTM is running. (LTRG is on). / FALSE : LTM is not running. (LTRG is off).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Get_LtmBusy( UCHAR pipe_no, BOOL* const busy_status );

/**
Restart LTM process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_ContStart( UCHAR pipe_no );

/**
start LTM process.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@remarks		This API uses DDIM_User_Clr_Flg().
*/
extern INT32 Im_LTM_LTM_Start( UCHAR pipe_no );

/**
Stop LTM
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Stop( UCHAR pipe_no );

/**
wait end of LTM LTM.
@param[in]		waiptn					: waiting end cause. (bit field)
										<ul><li>@ref D_IM_LTM_LTM1_INT_FLG_LTM_END
											<li>@ref D_IM_LTM_LTM1_INT_FLG_EXIP_END
											<li>@ref D_IM_LTM_LTM1_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_LTM1_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_LTM1_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_LTM1_INT_FLG_YLOG_ERR
											<li>@ref D_IM_LTM_LTM2_INT_FLG_LTM_END
											<li>@ref D_IM_LTM_LTM2_INT_FLG_EXIP_END
											<li>@ref D_IM_LTM_LTM2_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_LTM2_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_LTM2_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_LTM2_INT_FLG_YLOG_ERR</ul>
@param[in]		tmout					: timeout.
@param[out]		p_flgptn				: end cause. (bit field)
										<ul><li>@ref D_IM_LTM_LTM1_INT_FLG_LTM_END
											<li>@ref D_IM_LTM_LTM1_INT_FLG_EXIP_END
											<li>@ref D_IM_LTM_LTM1_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_LTM1_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_LTM1_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_LTM1_INT_FLG_YLOG_ERR
											<li>@ref D_IM_LTM_LTM2_INT_FLG_LTM_END
											<li>@ref D_IM_LTM_LTM2_INT_FLG_EXIP_END
											<li>@ref D_IM_LTM_LTM2_INT_FLG_LINE_END
											<li>@ref D_IM_LTM_LTM2_INT_FLG_AXR_ERR
											<li>@ref D_IM_LTM_LTM2_INT_FLG_AXW_ERR
											<li>@ref D_IM_LTM_LTM2_INT_FLG_YLOG_ERR</ul>
@retval			D_DDIM_OK				: LTM normal end. and set end cause to p_flgptn.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
@retval			D_IM_LTM_SYSTEM_ERROR	: systemcall error.
@retval			D_IM_LTM_TIMEOUT		: wait timeout.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Im_LTM_LTM_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout );

/**
Interrupt handler.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2).
@remarks		This API uses DDIM_User_Set_Flg().
*/
extern VOID Im_LTM_LTM_Int_Handler( UCHAR pipe_no );

/**
Set High frequency strength
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		hf_ctrl					: pointer of HF control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_High_Frequency_Strength( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_HF_STRENGTH* hf_ctrl );

/**
Set LTM LTM internal parameters.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		param					: Internal parameter information.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_Internal_Param( UCHAR pipe_no, T_IM_LTM_INTERNAL_PARAM* const param );

/**
LTM Blend control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		blend_ctrl				: pointer of blend control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_Blend( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_BLEND* blend_ctrl );

/**
Set RCG table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_LTM_TABLE_MAX_RCG)
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
INT32 Im_LTM_LTM_Set_RCG_Table( UCHAR pipe_no, const USHORT* const src_tbl );

/**
LTM chroma correction strength control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		chroma_ctrl				: pointer of chroma correction strength control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_Chroma_Correction_Strength( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_CHROMA* chroma_ctrl );

/**
YLOG table control
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		ylog_ctrl				: pointer of YLOG table control parameters.
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Ctrl_Ylog_Tbl( UCHAR pipe_no, const T_IM_LTM_CTRL_YLOG* const ylog_ctrl );

/**
Get active YLOG table No.
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[out]		tbl_no					: Active YLOG table No.
											<ul><li>@ref D_IM_LTM_YLOG_TBL_SIDE_0
												<li>@ref D_IM_LTM_YLOG_TBL_SIDE_1</ul>
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Get_Active_Ylog_Tbl_No( UCHAR pipe_no, UCHAR* const tbl_no );

/**
Set YLOG table
@param[in]		pipe_no					: Image pipe no(pipe1/pipe2/pipe12).
@param[in]		tbl_sel					: Select the YLOG table.
											<ul><li>@ref D_LTM_YLOG_TBL_00
												<li>@ref D_LTM_YLOG_TBL_01
												<li>@ref D_LTM_YLOG_TBL_10
												<li>@ref D_LTM_YLOG_TBL_11</ul>
@param[in]		src_tbl					: Pointer of Table data (Type of USHORT, Maximum array number is D_IM_LTM_TABLE_MAX_YLOG)
@retval			D_DDIM_OK				: success.
@retval			D_IM_LTM_PARAM_ERROR	: parameter error.
*/
extern INT32 Im_LTM_LTM_Set_YLOG_Table( UCHAR pipe_no, UCHAR tbl_sel, const USHORT* const src_tbl );

/* @} */	// im_ltm_map group






#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IM_LTM_H_

/**
@weakgroup im_ltm
@{

<hr>

@section im_ltm_sample Sample code

@section im_ltm_sample_section1	Sample code of LTM(All block) Initialization.
	@code
	Im_LTM_Stop_All_Block( D_IM_LTM_PIPE12 );
	Im_LTM_Init_All_Block( D_IM_LTM_PIPE12 );
	@endcode
	<br><br>

@section im_ltm_sample_section2	Sample code of LTM(Individual block) Initialization.
	@code
	Im_LTM_RBK_Stop( D_IM_LTM_PIPE12 );
	Im_LTM_RBK_Init( D_IM_LTM_PIPE12 );

	Im_LTM_MAP_Stop( D_IM_LTM_PIPE12 );
	Im_LTM_MAP_Init( D_IM_LTM_PIPE12 );

	Im_LTM_LTM_Stop( D_IM_LTM_PIPE12 );
	Im_LTM_LTM_Init( D_IM_LTM_PIPE12 );
	@endcode
	<br><br>

@section im_ltm_sample_section3	Sample code of RBK block's data setting for SDRAM input mode.
	@code
	#define D_USER_RGB_TOP_X_PIXS 0
	#define D_USER_RGB_TOP_Y_PIXS 0
	#define D_USER_RGB_WIDTH_PIXS 640
	#define D_USER_RGB_LINES_PIXS 480

	#define D_USER_RGB_IN_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS)
	#define D_USER_RGB_OUT_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 3 * 2)
	#define D_USER_RGB_IN_ADDR_R 0x00000000
	#define D_USER_RGB_IN_ADDR_G 0x10000000
	#define D_USER_RGB_IN_ADDR_B 0x20000000
	#define D_USER_RGB_OUT_ADDR 0x30000000

	VOID User_RBK_Handler( UINT32* status, UINT32 user_param );

	VOID User_SDRAM_Input_RBK_Process( VOID ) {
		T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl;
		T_IM_LTM_RBK_CTRL_SDRAM_INPUT rbk_ctrl_sdram_in;
		T_IM_LTM_INADDR_INFO input_addr;
		T_IM_LTM_RBK_OUTDATA_INFO out_data_info;

		rbk_ctrl.line_intr        = 0;
		rbk_ctrl.burst_length     = D_IM_LTM_BRST_512;
		rbk_ctrl.mask_write       = D_IM_LTM_MASK_WRITE_ENABLE;
		rbk_ctrl.out_select       = D_IM_LTM_RBK_OUT_SDRAM;
		rbk_ctrl.data_type        = D_IM_LTM_DTYP_UNPACK12;
		rbk_ctrl.reduction_enable = D_IM_LTM_ENABLE_OFF;
		rbk_ctrl.reduction_mode   = D_IM_LTM_RBK_RDC_MODE_DIV_2;	// dummy
		rbk_ctrl.rbk_user_handler = User_RBK_Handler;
		rbk_ctrl.user_param       = 0;

		rbk_ctrl_sdram_in.burst_length_r       = D_IM_LTM_BRST_512;
		rbk_ctrl_sdram_in.burst_length_g       = D_IM_LTM_BRST_512;
		rbk_ctrl_sdram_in.burst_length_b       = D_IM_LTM_BRST_512;
		rbk_ctrl_sdram_in.input_global         = D_USER_RGB_IN_LINEFEED_BYTES;
		rbk_ctrl_sdram_in.input_rect.img_left  = D_USER_RGB_TOP_X_PIXS;
		rbk_ctrl_sdram_in.input_rect.img_top   = D_USER_RGB_TOP_Y_PIXS;
		rbk_ctrl_sdram_in.input_rect.img_width = D_USER_RGB_WIDTH_PIXS;
		rbk_ctrl_sdram_in.input_rect.img_lines = D_USER_RGB_LINES_PIXS;
		rbk_ctrl_sdram_in.shift_enable         = D_IM_LTM_ENABLE_ON;

		input_addr.addr_r = (VOID*)D_USER_RGB_IN_ADDR_R;
		input_addr.addr_g = (VOID*)D_USER_RGB_IN_ADDR_G;
		input_addr.addr_b = (VOID*)D_USER_RGB_IN_ADDR_B;

		out_data_info.addr = (VOID*)D_USER_RGB_OUT_ADDR;
		out_data_info.global_width = D_USER_RGB_OUT_LINEFEED_BYTES;

		Im_LTM_RBK_Ctrl_Common( D_IM_LTM_PIPE1, &rbk_ctrl );
		Im_LTM_RBK_Ctrl_ModeSDRAMInput( D_IM_LTM_PIPE1, &rbk_ctrl_sdram_in );
		Im_LTM_RBK_Set_InAddr_Info( D_IM_LTM_PIPE1, &input_addr );
		Im_LTM_RBK_Set_OutData_Info( D_IM_LTM_PIPE1, &out_data_info );

		// Start RBK
		Im_LTM_RBK_Start( D_IM_LTM_PIPE1 );

		// Wait RBK is normal end.
		Im_LTM_RBK_WaitEnd( NULL, D_IM_LTM_RBK1_INT_FLG_RBK_END, 60 );

		Im_LTM_RBK_Stop( D_IM_LTM_PIPE1 );
	}

	VOID User_RBK_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_LTM_RBK1_INT_FLG_WAIT_ERR:
			case D_IM_LTM_RBK1_INT_FLG_AXR_ERR:
			case D_IM_LTM_RBK1_INT_FLG_AXW_ERR:
				printf( "LTM RBK Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>

@section im_ltm_sample_section4	Sample code of RBK block's data setting for SRO direct input mode.
	@code
	#define D_USER_BAYER_TOP_X_PIXS 0
	#define D_USER_BAYER_TOP_Y_PIXS 0
	#define D_USER_BAYER_WIDTH_PIXS 640
	#define D_USER_BAYER_LINES_PIXS 480

	#define D_USER_RGB_OUT_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 3 * 2)
	#define D_USER_RGB_OUT_ADDR 0x30000000

	VOID User_RBK_Handler( UINT32* status, UINT32 user_param );

	VOID User_SRO_Direct_Input_RBK_Process( VOID ) {
		T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl;
		T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_direct;
		T_IM_LTM_RBK_OUTDATA_INFO out_data_info;

		rbk_ctrl.line_intr        = 0;
		rbk_ctrl.burst_length     = D_IM_LTM_BRST_512;
		rbk_ctrl.mask_write       = D_IM_LTM_MASK_WRITE_ENABLE;
		rbk_ctrl.out_select       = D_IM_LTM_RBK_OUT_SDRAM;
		rbk_ctrl.data_type        = D_IM_LTM_DTYP_UNPACK12;
		rbk_ctrl.reduction_enable = D_IM_LTM_ENABLE_OFF;
		rbk_ctrl.reduction_mode   = D_IM_LTM_RBK_RDC_MODE_DIV_2;	// dummy
		rbk_ctrl.rbk_user_handler = User_RBK_Handler;
		rbk_ctrl.user_param       = 0;

		rbk_ctrl_direct.top_pixel0            = D_IM_LTM_RBK_TOP_PIXEL_R;
		rbk_ctrl_direct.top_pixel1            = D_IM_LTM_RBK_TOP_PIXEL_R;	// dummy
		rbk_ctrl_direct.sro_mode              = D_IM_LTM_RBK_SRO_DIRECT_1;
		rbk_ctrl_direct.shift_enable0         = D_IM_LTM_ENABLE_ON;
		rbk_ctrl_direct.shift_enable1         = D_IM_LTM_ENABLE_ON;			// dummy
		rbk_ctrl_direct.half_reduction_enable = D_IM_LTM_ENABLE_OFF;
		rbk_ctrl_direct.bayer_trim0.img_left  = D_USER_BAYER_TOP_X_PIXS;
		rbk_ctrl_direct.bayer_trim0.img_top   = D_USER_BAYER_TOP_Y_PIXS;
		rbk_ctrl_direct.bayer_trim0.img_width = D_USER_BAYER_WIDTH_PIXS;
		rbk_ctrl_direct.bayer_trim0.img_lines = D_USER_BAYER_LINES_PIXS;
		rbk_ctrl_direct.bayer_trim1.img_left  = 0;							// dummy
		rbk_ctrl_direct.bayer_trim1.img_top   = 0;							// dummy
		rbk_ctrl_direct.bayer_trim1.img_width = 0;							// dummy
		rbk_ctrl_direct.bayer_trim1.img_lines = 0;							// dummy

		out_data_info.addr = (VOID*)D_USER_RGB_OUT_ADDR;
		out_data_info.global_width = D_USER_RGB_OUT_LINEFEED_BYTES;

		Im_LTM_RBK_Ctrl_Common( D_IM_LTM_PIPE1, &rbk_ctrl );
		Im_LTM_RBK_Ctrl_ModeSRODirect( D_IM_LTM_PIPE1, &rbk_ctrl_direct );
		Im_LTM_RBK_Set_OutData_Info( D_IM_LTM_PIPE1, &out_data_info );

		// Start RBK
		Im_LTM_RBK_Start( D_IM_LTM_PIPE1 );

		// Wait RBK is normal end.
		Im_LTM_RBK_WaitEnd( NULL, D_IM_LTM_RBK1_INT_FLG_RBK_END, 60 );

		Im_LTM_RBK_Stop( D_IM_LTM_PIPE1 );
	}

	VOID User_RBK_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_LTM_RBK1_INT_FLG_WAIT_ERR:
			case D_IM_LTM_RBK1_INT_FLG_AXR_ERR:
			case D_IM_LTM_RBK1_INT_FLG_AXW_ERR:
				printf( "LTM RBK Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>


@section im_ltm_sample_section5	Sample code of MAP block's data setting for SDRAM input mode.
	@code
	#define D_USER_RGB_TOP_X_PIXS 0
	#define D_USER_RGB_TOP_Y_PIXS 0
	#define D_USER_RGB_WIDTH_PIXS 640
	#define D_USER_RGB_LINES_PIXS 480

	#define D_USER_RGB_IN_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 3 * 2)
	#define D_USER_Y_OUT_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 2)
	#define D_USER_RGB_IN_ADDR 0x00000000
	#define D_USER_YP_OUT_ADDR 0x10000000
	#define D_USER_YN_OUT_ADDR 0x20000000

	VOID User_MAP_Handler( UINT32* status, UINT32 user_param );

	VOID User_SDRAM_Input_MAP_Process( VOID ) {
		T_IM_LTM_MAP_CTRL_COMMON map_ctrl;
		T_IM_LTM_MAP_CTRL_SDRAM_INPUT map_ctrl_sdram_in;
		T_IM_LTM_MAP_INDATA_INFO input_data_info;
		T_IM_LTM_MAP_OUTDATA_INFO out_data_info;
		T_IM_LTM_CTRL_YLOG ylog_ctrl;
		USHORT ylog_tbl[512] = {
			    0,   31,   47,   63,   79,   95,  111,  127,  143,  159,  175,  191,  207,  223,  239,  255,
			  271,  287,  303,  319,  335,  351,  367,  383,  399,  415,  431,  447,  463,  479,  495,  511,
			  527,  543,  559,  575,  591,  607,  623,  639,  655,  671,  687,  703,  719,  735,  751,  767,
			  783,  799,  815,  831,  847,  863,  879,  895,  911,  927,  943,  959,  975,  991, 1007, 1023,
			 1039, 1055, 1071, 1087, 1103, 1119, 1135, 1151, 1167, 1183, 1199, 1215, 1231, 1247, 1263, 1279,
			 1295, 1311, 1327, 1343, 1359, 1375, 1391, 1407, 1423, 1439, 1455, 1471, 1487, 1503, 1519, 1535,
			 1551, 1567, 1583, 1599, 1615, 1631, 1647, 1663, 1679, 1695, 1711, 1727, 1743, 1759, 1775, 1791,
			 1807, 1823, 1839, 1855, 1871, 1887, 1903, 1919, 1935, 1951, 1967, 1983, 1999, 2015, 2031, 2047,
			 2063, 2079, 2095, 2111, 2127, 2143, 2159, 2175, 2191, 2207, 2223, 2239, 2255, 2271, 2287, 2303,
			 2319, 2335, 2351, 2367, 2383, 2399, 2415, 2431, 2447, 2463, 2479, 2495, 2511, 2527, 2543, 2559,
			 2575, 2591, 2607, 2623, 2639, 2655, 2671, 2687, 2703, 2719, 2735, 2751, 2767, 2783, 2799, 2815,
			 2831, 2847, 2863, 2879, 2895, 2911, 2927, 2943, 2959, 2975, 2991, 3007, 3023, 3039, 3055, 3071,
			 3087, 3103, 3119, 3135, 3151, 3167, 3183, 3199, 3215, 3231, 3247, 3263, 3279, 3295, 3311, 3327,
			 3343, 3359, 3375, 3391, 3407, 3423, 3439, 3455, 3471, 3487, 3503, 3519, 3535, 3551, 3567, 3583,
			 3599, 3615, 3631, 3647, 3663, 3679, 3695, 3711, 3727, 3743, 3759, 3775, 3791, 3807, 3823, 3839,
			 3855, 3871, 3887, 3903, 3919, 3935, 3951, 3967, 3983, 3999, 4015, 4031, 4047, 4063, 4079, 4095,
			 4111, 4127, 4143, 4159, 4175, 4191, 4207, 4223, 4239, 4255, 4271, 4287, 4303, 4319, 4335, 4351,
			 4367, 4383, 4399, 4415, 4431, 4447, 4463, 4479, 4495, 4511, 4527, 4543, 4559, 4575, 4591, 4607,
			 4623, 4639, 4655, 4671, 4687, 4703, 4719, 4735, 4751, 4767, 4783, 4799, 4815, 4831, 4847, 4863,
			 4879, 4895, 4911, 4927, 4943, 4959, 4975, 4991, 5007, 5023, 5039, 5055, 5071, 5087, 5103, 5119,
			 5135, 5151, 5167, 5183, 5199, 5215, 5231, 5247, 5263, 5279, 5295, 5311, 5327, 5343, 5359, 5375,
			 5391, 5407, 5423, 5439, 5455, 5471, 5487, 5503, 5519, 5535, 5551, 5567, 5583, 5599, 5615, 5631,
			 5647, 5663, 5679, 5695, 5711, 5727, 5743, 5759, 5775, 5791, 5807, 5823, 5839, 5855, 5871, 5887,
			 5903, 5919, 5935, 5951, 5967, 5983, 5999, 6015, 6031, 6047, 6063, 6079, 6095, 6111, 6127, 6143,
			 6159, 6175, 6191, 6207, 6223, 6239, 6255, 6271, 6287, 6303, 6319, 6335, 6351, 6367, 6383, 6399,
			 6415, 6431, 6447, 6463, 6479, 6495, 6511, 6527, 6543, 6559, 6575, 6591, 6607, 6623, 6639, 6655,
			 6671, 6687, 6703, 6719, 6735, 6751, 6767, 6783, 6799, 6815, 6831, 6847, 6863, 6879, 6895, 6911,
			 6927, 6943, 6959, 6975, 6991, 7007, 7023, 7039, 7055, 7071, 7087, 7103, 7119, 7135, 7151, 7167,
			 7183, 7199, 7215, 7231, 7247, 7263, 7279, 7295, 7311, 7327, 7343, 7359, 7375, 7391, 7407, 7423,
			 7439, 7455, 7471, 7487, 7503, 7519, 7535, 7551, 7567, 7583, 7599, 7615, 7631, 7647, 7663, 7679,
			 7695, 7711, 7727, 7743, 7759, 7775, 7791, 7807, 7823, 7839, 7855, 7871, 7887, 7903, 7919, 7935,
			 7951, 7967, 7983, 7999, 8015, 8031, 8047, 8063, 8079, 8095, 8111, 8127, 8143, 8159, 8175, 8191
		}

		map_ctrl.line_intr           = 0;
		map_ctrl.wch_burst_length_yp = D_IM_LTM_BRST_512;
		map_ctrl.wch_burst_length_yn = D_IM_LTM_BRST_512;
		map_ctrl.mask_write_yp       = D_IM_LTM_MASK_WRITE_ENABLE;
		map_ctrl.mask_write_yn       = D_IM_LTM_MASK_WRITE_ENABLE;
		map_ctrl.write_select_yp     = D_IM_LTM_PORT_DEFAULT;
		map_ctrl.write_select_yn     = D_IM_LTM_PORT_DEFAULT;
		map_ctrl.data_type_yp        = D_IM_LTM_DTYP_UNPACK12;
		map_ctrl.data_type_yn        = D_IM_LTM_DTYP_UNPACK12;
		map_ctrl.map_user_handler    = User_MAP_Handler;
		map_ctrl.user_param          = 0;

		map_ctrl_sdram_in.burst_length         = D_IM_LTM_BRST_512;
		map_ctrl_sdram_in.input_global         = D_USER_RGB_IN_LINEFEED_BYTES;
		map_ctrl_sdram_in.input_rect.img_left  = D_USER_RGB_TOP_X_PIXS;
		map_ctrl_sdram_in.input_rect.img_top   = D_USER_RGB_TOP_Y_PIXS;
		map_ctrl_sdram_in.input_rect.img_width = D_USER_RGB_WIDTH_PIXS;
		map_ctrl_sdram_in.input_rect.img_lines = D_USER_RGB_LINES_PIXS;
		map_ctrl_sdram_in.data_type            = D_IM_LTM_DTYP_UNPACK12;

		input_data_info.addr           = (VOID*)D_USER_RGB_IN_ADDR;
		input_data_info.top_bit_offset = 0;

		out_data_info.addr_yp         = (VOID*)D_USER_YP_OUT_ADDR;
		out_data_info.global_width_yp = D_USER_Y_OUT_LINEFEED_BYTES;
		out_data_info.addr_yn         = (VOID*)D_USER_YN_OUT_ADDR;
		out_data_info.global_width_yp = D_USER_Y_OUT_LINEFEED_BYTES;

		ylog_ctrl.use_tbl         = D_IM_LTM_YLOG_TBL_SIDE_0;
		ylog_ctrl.end_point0_tbl0 = 0x3FFF;
		ylog_ctrl.end_point0_tbl1 = 0x3FFF;
		ylog_ctrl.end_point1_tbl0 = 0x3FFF;
		ylog_ctrl.end_point1_tbl1 = 0x3FFF;

		Im_LTM_MAP_Ctrl_Common( D_IM_LTM_PIPE1, &map_ctrl );
		Im_LTM_MAP_Ctrl_ModeSDRAMInput( D_IM_LTM_PIPE1, &map_ctrl_sdram_in );
		Im_LTM_MAP_Set_InData_Info( D_IM_LTM_PIPE1, &input_addr );
		Im_LTM_MAP_Set_OutData_Info( D_IM_LTM_PIPE1, &out_data_info );
		Im_LTM_MAP_Ctrl_Ylog_Tbl( D_IM_LTM_PIPE1, &ylog_ctrl );
		Im_LTM_MAP_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_00, ylog_tbl );
		Im_LTM_MAP_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_01, ylog_tbl );

		// Start MAP
		Im_LTM_MAP_Start( D_IM_LTM_PIPE1 );

		// Wait MAP is normal end.
		Im_LTM_MAP_WaitEnd( NULL, D_IM_LTM_MAP1_INT_FLG_MAP_END, 60 );

		Im_LTM_MAP_Stop( D_IM_LTM_PIPE1 );
	}

	VOID User_MAP_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_LTM_MAP1_INT_FLG_AXR_ERR:
			case D_IM_LTM_MAP1_INT_FLG_AXW_ERR:
			case D_IM_LTM_MAP1_INT_FLG_YLOG_ERR:
				printf( "LTM MAP Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>

@section im_ltm_sample_section6	Sample code of MAP block's data setting for RBK direct input mode.
	@code
	#define D_USER_RGB_TOP_X_PIXS 0
	#define D_USER_RGB_TOP_Y_PIXS 0
	#define D_USER_RGB_WIDTH_PIXS 640
	#define D_USER_RGB_LINES_PIXS 480

	#define D_USER_Y_OUT_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 2)
	#define D_USER_YP_OUT_ADDR 0x10000000
	#define D_USER_YN_OUT_ADDR 0x20000000

	VOID User_MAP_Handler( UINT32* status, UINT32 user_param );

	VOID User_RBK_Direct_Input_MAP_Process( VOID ) {
		T_IM_LTM_MAP_CTRL_COMMON map_ctrl;
		T_IM_LTM_MAP_OUTDATA_INFO out_data_info;
		T_IM_LTM_CTRL_YLOG ylog_ctrl;
		USHORT ylog_tbl[512] = {
			    0,   31,   47,   63,   79,   95,  111,  127,  143,  159,  175,  191,  207,  223,  239,  255,
			  271,  287,  303,  319,  335,  351,  367,  383,  399,  415,  431,  447,  463,  479,  495,  511,
			  527,  543,  559,  575,  591,  607,  623,  639,  655,  671,  687,  703,  719,  735,  751,  767,
			  783,  799,  815,  831,  847,  863,  879,  895,  911,  927,  943,  959,  975,  991, 1007, 1023,
			 1039, 1055, 1071, 1087, 1103, 1119, 1135, 1151, 1167, 1183, 1199, 1215, 1231, 1247, 1263, 1279,
			 1295, 1311, 1327, 1343, 1359, 1375, 1391, 1407, 1423, 1439, 1455, 1471, 1487, 1503, 1519, 1535,
			 1551, 1567, 1583, 1599, 1615, 1631, 1647, 1663, 1679, 1695, 1711, 1727, 1743, 1759, 1775, 1791,
			 1807, 1823, 1839, 1855, 1871, 1887, 1903, 1919, 1935, 1951, 1967, 1983, 1999, 2015, 2031, 2047,
			 2063, 2079, 2095, 2111, 2127, 2143, 2159, 2175, 2191, 2207, 2223, 2239, 2255, 2271, 2287, 2303,
			 2319, 2335, 2351, 2367, 2383, 2399, 2415, 2431, 2447, 2463, 2479, 2495, 2511, 2527, 2543, 2559,
			 2575, 2591, 2607, 2623, 2639, 2655, 2671, 2687, 2703, 2719, 2735, 2751, 2767, 2783, 2799, 2815,
			 2831, 2847, 2863, 2879, 2895, 2911, 2927, 2943, 2959, 2975, 2991, 3007, 3023, 3039, 3055, 3071,
			 3087, 3103, 3119, 3135, 3151, 3167, 3183, 3199, 3215, 3231, 3247, 3263, 3279, 3295, 3311, 3327,
			 3343, 3359, 3375, 3391, 3407, 3423, 3439, 3455, 3471, 3487, 3503, 3519, 3535, 3551, 3567, 3583,
			 3599, 3615, 3631, 3647, 3663, 3679, 3695, 3711, 3727, 3743, 3759, 3775, 3791, 3807, 3823, 3839,
			 3855, 3871, 3887, 3903, 3919, 3935, 3951, 3967, 3983, 3999, 4015, 4031, 4047, 4063, 4079, 4095,
			 4111, 4127, 4143, 4159, 4175, 4191, 4207, 4223, 4239, 4255, 4271, 4287, 4303, 4319, 4335, 4351,
			 4367, 4383, 4399, 4415, 4431, 4447, 4463, 4479, 4495, 4511, 4527, 4543, 4559, 4575, 4591, 4607,
			 4623, 4639, 4655, 4671, 4687, 4703, 4719, 4735, 4751, 4767, 4783, 4799, 4815, 4831, 4847, 4863,
			 4879, 4895, 4911, 4927, 4943, 4959, 4975, 4991, 5007, 5023, 5039, 5055, 5071, 5087, 5103, 5119,
			 5135, 5151, 5167, 5183, 5199, 5215, 5231, 5247, 5263, 5279, 5295, 5311, 5327, 5343, 5359, 5375,
			 5391, 5407, 5423, 5439, 5455, 5471, 5487, 5503, 5519, 5535, 5551, 5567, 5583, 5599, 5615, 5631,
			 5647, 5663, 5679, 5695, 5711, 5727, 5743, 5759, 5775, 5791, 5807, 5823, 5839, 5855, 5871, 5887,
			 5903, 5919, 5935, 5951, 5967, 5983, 5999, 6015, 6031, 6047, 6063, 6079, 6095, 6111, 6127, 6143,
			 6159, 6175, 6191, 6207, 6223, 6239, 6255, 6271, 6287, 6303, 6319, 6335, 6351, 6367, 6383, 6399,
			 6415, 6431, 6447, 6463, 6479, 6495, 6511, 6527, 6543, 6559, 6575, 6591, 6607, 6623, 6639, 6655,
			 6671, 6687, 6703, 6719, 6735, 6751, 6767, 6783, 6799, 6815, 6831, 6847, 6863, 6879, 6895, 6911,
			 6927, 6943, 6959, 6975, 6991, 7007, 7023, 7039, 7055, 7071, 7087, 7103, 7119, 7135, 7151, 7167,
			 7183, 7199, 7215, 7231, 7247, 7263, 7279, 7295, 7311, 7327, 7343, 7359, 7375, 7391, 7407, 7423,
			 7439, 7455, 7471, 7487, 7503, 7519, 7535, 7551, 7567, 7583, 7599, 7615, 7631, 7647, 7663, 7679,
			 7695, 7711, 7727, 7743, 7759, 7775, 7791, 7807, 7823, 7839, 7855, 7871, 7887, 7903, 7919, 7935,
			 7951, 7967, 7983, 7999, 8015, 8031, 8047, 8063, 8079, 8095, 8111, 8127, 8143, 8159, 8175, 8191
		}

		map_ctrl.line_intr        = 0;
		map_ctrl.burst_length     = D_IM_LTM_BRST_512;
		map_ctrl.mask_write       = D_IM_LTM_MASK_WRITE_ENABLE;
		map_ctrl.out_select       = D_IM_LTM_RBK_OUT_SDRAM;
		map_ctrl.data_type        = D_IM_LTM_DTYP_UNPACK12;
		map_ctrl.reduction_enable = D_IM_LTM_ENABLE_OFF;
		map_ctrl.reduction_mode   = D_IM_LTM_RBK_RDC_MODE_DIV_2;	// dummy
		map_ctrl.rbk_user_handler = User_RBK_Handler;
		map_ctrl.user_param       = 0;

		out_data_info.addr_yp         = (VOID*)D_USER_YP_OUT_ADDR;
		out_data_info.global_width_yp = D_USER_Y_OUT_LINEFEED_BYTES;
		out_data_info.addr_yn         = (VOID*)D_USER_YN_OUT_ADDR;
		out_data_info.global_width_yp = D_USER_Y_OUT_LINEFEED_BYTES;

		ylog_ctrl.use_tbl         = D_IM_LTM_YLOG_TBL_SIDE_0;
		ylog_ctrl.end_point0_tbl0 = 0x3FFF;
		ylog_ctrl.end_point0_tbl1 = 0x3FFF;
		ylog_ctrl.end_point1_tbl0 = 0x3FFF;
		ylog_ctrl.end_point1_tbl1 = 0x3FFF;

		Im_LTM_MAP_Ctrl_Common( D_IM_LTM_PIPE1, &map_ctrl );
		Im_LTM_MAP_Ctrl_ModeRBKDirect( D_IM_LTM_PIPE1 );
		Im_LTM_MAP_Set_OutData_Info( D_IM_LTM_PIPE1, &out_data_info );
		Im_LTM_MAP_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_00, ylog_tbl );
		Im_LTM_MAP_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_01, ylog_tbl );

		// Start MAP
		Im_LTM_MAP_Start( D_IM_LTM_PIPE1 );

		// Wait MAP is normal end.
		Im_LTM_MAP_WaitEnd( NULL, D_IM_LTM_MAP1_INT_FLG_MAP_END, 60 );

		Im_LTM_MAP_Stop( D_IM_LTM_PIPE1 );
	}

	VOID User_MAP_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_LTM_MAP1_INT_FLG_AXR_ERR:
			case D_IM_LTM_MAP1_INT_FLG_AXW_ERR:
			case D_IM_LTM_MAP1_INT_FLG_YLOG_ERR:
				printf( "LTM MAP Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>


@section im_ltm_sample_section7	Sample code of LTM block's data setting for SDRAM input mode.
	@code
	#define D_USER_YN_TOP_X_PIXS 0
	#define D_USER_YN_TOP_Y_PIXS 0
	#define D_USER_YN_WIDTH_PIXS 640
	#define D_USER_YN_LINES_PIXS 480

	#define D_USER_YP_TOP_X_PIXS 0
	#define D_USER_YP_TOP_Y_PIXS 0
	#define D_USER_YP_WIDTH_PIXS 640
	#define D_USER_YP_LINES_PIXS 480

	#define D_USER_RGB_TOP_X_PIXS 0
	#define D_USER_RGB_TOP_Y_PIXS 0
	#define D_USER_RGB_WIDTH_PIXS 640
	#define D_USER_RGB_LINES_PIXS 480

	#define D_USER_YN_IN_LINEFEED_BYTES (D_USER_YN_WIDTH_PIXS * 2)
	#define D_USER_YP_IN_LINEFEED_BYTES (D_USER_YP_WIDTH_PIXS * 2)
	#define D_USER_RGB_IN_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 3 * 2)
	#define D_USER_RGB_OUT_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 2)
	#define D_USER_YN_IN_ADDR 0x00000000
	#define D_USER_YP_IN_ADDR 0x10000000
	#define D_USER_R_IN_ADDR 0x20000000
	#define D_USER_G_IN_ADDR 0x30000000
	#define D_USER_B_IN_ADDR 0x40000000
	#define D_USER_R_OUT_ADDR 0x50000000
	#define D_USER_G_OUT_ADDR 0x60000000
	#define D_USER_B_OUT_ADDR 0x70000000

	VOID User_LTM_Handler( UINT32* status, UINT32 user_param );

	VOID User_SDRAM_Input_LTM_Process( VOID ) {
		T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl;
		T_IM_LTM_LTM_CTRL_SDRAM_INPUT ltm_ctrl_sdram_in;
		T_IM_LTM_INADDR_INFO in_addr;
		T_IM_LTM_LTM_INDATA_INFO input_data_info;
		T_IM_LTM_LTM_OUTDATA_INFO out_data_info;
		T_IM_LTM_LTM_MAP_RESIZE_PITCH resize_pitch;
		T_IM_LTM_INTERNAL_PARAM internal_param;
		T_IM_LTM_CTRL_YLOG ylog_ctrl;
		USHORT ylog_tbl[512] = {
			    0,   31,   47,   63,   79,   95,  111,  127,  143,  159,  175,  191,  207,  223,  239,  255,
			  271,  287,  303,  319,  335,  351,  367,  383,  399,  415,  431,  447,  463,  479,  495,  511,
			  527,  543,  559,  575,  591,  607,  623,  639,  655,  671,  687,  703,  719,  735,  751,  767,
			  783,  799,  815,  831,  847,  863,  879,  895,  911,  927,  943,  959,  975,  991, 1007, 1023,
			 1039, 1055, 1071, 1087, 1103, 1119, 1135, 1151, 1167, 1183, 1199, 1215, 1231, 1247, 1263, 1279,
			 1295, 1311, 1327, 1343, 1359, 1375, 1391, 1407, 1423, 1439, 1455, 1471, 1487, 1503, 1519, 1535,
			 1551, 1567, 1583, 1599, 1615, 1631, 1647, 1663, 1679, 1695, 1711, 1727, 1743, 1759, 1775, 1791,
			 1807, 1823, 1839, 1855, 1871, 1887, 1903, 1919, 1935, 1951, 1967, 1983, 1999, 2015, 2031, 2047,
			 2063, 2079, 2095, 2111, 2127, 2143, 2159, 2175, 2191, 2207, 2223, 2239, 2255, 2271, 2287, 2303,
			 2319, 2335, 2351, 2367, 2383, 2399, 2415, 2431, 2447, 2463, 2479, 2495, 2511, 2527, 2543, 2559,
			 2575, 2591, 2607, 2623, 2639, 2655, 2671, 2687, 2703, 2719, 2735, 2751, 2767, 2783, 2799, 2815,
			 2831, 2847, 2863, 2879, 2895, 2911, 2927, 2943, 2959, 2975, 2991, 3007, 3023, 3039, 3055, 3071,
			 3087, 3103, 3119, 3135, 3151, 3167, 3183, 3199, 3215, 3231, 3247, 3263, 3279, 3295, 3311, 3327,
			 3343, 3359, 3375, 3391, 3407, 3423, 3439, 3455, 3471, 3487, 3503, 3519, 3535, 3551, 3567, 3583,
			 3599, 3615, 3631, 3647, 3663, 3679, 3695, 3711, 3727, 3743, 3759, 3775, 3791, 3807, 3823, 3839,
			 3855, 3871, 3887, 3903, 3919, 3935, 3951, 3967, 3983, 3999, 4015, 4031, 4047, 4063, 4079, 4095,
			 4111, 4127, 4143, 4159, 4175, 4191, 4207, 4223, 4239, 4255, 4271, 4287, 4303, 4319, 4335, 4351,
			 4367, 4383, 4399, 4415, 4431, 4447, 4463, 4479, 4495, 4511, 4527, 4543, 4559, 4575, 4591, 4607,
			 4623, 4639, 4655, 4671, 4687, 4703, 4719, 4735, 4751, 4767, 4783, 4799, 4815, 4831, 4847, 4863,
			 4879, 4895, 4911, 4927, 4943, 4959, 4975, 4991, 5007, 5023, 5039, 5055, 5071, 5087, 5103, 5119,
			 5135, 5151, 5167, 5183, 5199, 5215, 5231, 5247, 5263, 5279, 5295, 5311, 5327, 5343, 5359, 5375,
			 5391, 5407, 5423, 5439, 5455, 5471, 5487, 5503, 5519, 5535, 5551, 5567, 5583, 5599, 5615, 5631,
			 5647, 5663, 5679, 5695, 5711, 5727, 5743, 5759, 5775, 5791, 5807, 5823, 5839, 5855, 5871, 5887,
			 5903, 5919, 5935, 5951, 5967, 5983, 5999, 6015, 6031, 6047, 6063, 6079, 6095, 6111, 6127, 6143,
			 6159, 6175, 6191, 6207, 6223, 6239, 6255, 6271, 6287, 6303, 6319, 6335, 6351, 6367, 6383, 6399,
			 6415, 6431, 6447, 6463, 6479, 6495, 6511, 6527, 6543, 6559, 6575, 6591, 6607, 6623, 6639, 6655,
			 6671, 6687, 6703, 6719, 6735, 6751, 6767, 6783, 6799, 6815, 6831, 6847, 6863, 6879, 6895, 6911,
			 6927, 6943, 6959, 6975, 6991, 7007, 7023, 7039, 7055, 7071, 7087, 7103, 7119, 7135, 7151, 7167,
			 7183, 7199, 7215, 7231, 7247, 7263, 7279, 7295, 7311, 7327, 7343, 7359, 7375, 7391, 7407, 7423,
			 7439, 7455, 7471, 7487, 7503, 7519, 7535, 7551, 7567, 7583, 7599, 7615, 7631, 7647, 7663, 7679,
			 7695, 7711, 7727, 7743, 7759, 7775, 7791, 7807, 7823, 7839, 7855, 7871, 7887, 7903, 7919, 7935,
			 7951, 7967, 7983, 7999, 8015, 8031, 8047, 8063, 8079, 8095, 8111, 8127, 8143, 8159, 8175, 8191
		}

		ltm_ctrl.line_intr            = 0;
		ltm_ctrl.rch_burst_length_yp  = D_IM_LTM_BRST_512;
		ltm_ctrl.rch_burst_length_yn  = D_IM_LTM_BRST_512;
		ltm_ctrl.wch_burst_length_r   = D_IM_LTM_BRST_512;
		ltm_ctrl.wch_burst_length_g   = D_IM_LTM_BRST_512;
		ltm_ctrl.wch_burst_length_b   = D_IM_LTM_BRST_512;
		ltm_ctrl.mask_write_r         = D_IM_LTM_MASK_WRITE_ENABLE;
		ltm_ctrl.mask_write_g         = D_IM_LTM_MASK_WRITE_ENABLE;
		ltm_ctrl.mask_write_b         = D_IM_LTM_MASK_WRITE_ENABLE;
		ltm_ctrl.read_select_yp       = D_IM_LTM_PORT_DEFAULT;
		ltm_ctrl.read_select_yn       = D_IM_LTM_PORT_DEFAULT;
		ltm_ctrl.in_data_type_yp      = D_IM_LTM_DTYP_UNPACK12;
		ltm_ctrl.in_data_type_yn      = D_IM_LTM_DTYP_UNPACK12;
		ltm_ctrl.out_select           = D_IM_LTM_LTM_OUT_SDRAM;
		ltm_ctrl.out_data_type        = D_IM_LTM_DTYP_UNPACK12;
		ltm_ctrl.input_rect.img_left  = D_USER_YN_TOP_X_PIXS;
		ltm_ctrl.input_rect.img_top   = D_USER_YN_TOP_Y_PIXS;
		ltm_ctrl.input_rect.img_width = D_USER_YN_WIDTH_PIXS;
		ltm_ctrl.input_rect.img_lines = D_USER_YN_LINES_PIXS;
		ltm_ctrl.global_width_yp      = D_USER_Y_IN_LINEFEED_BYTES;
		ltm_ctrl.global_width_yn      = D_USER_Y_IN_LINEFEED_BYTES;
		ltm_ctrl.ltm_user_handler     = User_LTM_Handler;
		ltm_ctrl.user_param           = 0;

		ltm_ctrl_sdram_in.rch_burst_length_r   = D_IM_LTM_BRST_512;
		ltm_ctrl_sdram_in.rch_burst_length_g   = D_IM_LTM_BRST_512;
		ltm_ctrl_sdram_in.rch_burst_length_b   = D_IM_LTM_BRST_512;
		ltm_ctrl_sdram_in.input_global_rgb     = D_USER_RGB_IN_LINEFEED_BYTES;
		ltm_ctrl_sdram_in.input_rect.img_left  = D_USER_RGB_TOP_X_PIXS;
		ltm_ctrl_sdram_in.input_rect.img_top   = D_USER_RGB_TOP_Y_PIXS;
		ltm_ctrl_sdram_in.input_rect.img_width = D_USER_RGB_WIDTH_PIXS;
		ltm_ctrl_sdram_in.input_rect.img_lines = D_USER_RGB_LINES_PIXS;

		in_addr.addr_r = D_USER_R_IN_ADDR;
		in_addr.addr_g = D_USER_G_IN_ADDR;
		in_addr.addr_b = D_USER_B_IN_ADDR;

		input_data_info.addr_yp           = (VOID*)D_USER_YN_IN_ADDR;
		input_data_info.addr_yn           = (VOID*)D_USER_YP_IN_ADDR;
		input_data_info.top_bit_offset_yp = 0;
		input_data_info.top_bit_offset_yn = 0;

		out_data_info.addr[D_IM_LTM_PORT_R]         = (VOID*)D_USER_R_OUT_ADDR;
		out_data_info.addr[D_IM_LTM_PORT_G]         = (VOID*)D_USER_G_OUT_ADDR;
		out_data_info.addr[D_IM_LTM_PORT_B]         = (VOID*)D_USER_B_OUT_ADDR;
		out_data_info.global_width[D_IM_LTM_PORT_R] = D_USER_RGB_OUT_LINEFEED_BYTES;
		out_data_info.global_width[D_IM_LTM_PORT_G] = D_USER_RGB_OUT_LINEFEED_BYTES;
		out_data_info.global_width[D_IM_LTM_PORT_B] = D_USER_RGB_OUT_LINEFEED_BYTES;

		resize_pitch.hsta = 0;
		resize_pitch.hpit = 0x4000;
		resize_pitch.vsta = 0;
		resize_pitch.vpit = 0x4000;

		Im_LTM_MAP_Get_Internal_Param( D_IM_LTM_PIPE1, &internal_param );

		ylog_ctrl.use_tbl         = D_IM_LTM_YLOG_TBL_SIDE_0;
		ylog_ctrl.end_point0_tbl0 = 0x3FFF;
		ylog_ctrl.end_point0_tbl1 = 0x3FFF;
		ylog_ctrl.end_point1_tbl0 = 0x3FFF;
		ylog_ctrl.end_point1_tbl1 = 0x3FFF;

		Im_LTM_LTM_Ctrl_Common( D_IM_LTM_PIPE1, &ltm_ctrl );
		Im_LTM_LTM_Ctrl_ModeSDRAMInput( D_IM_LTM_PIPE1, &ltm_ctrl_sdram_in );
		Im_LTM_LTM_Set_InAddr_Info_Rgb( D_IM_LTM_PIPE1, &in_addr );
		Im_LTM_LTM_Set_InData_Info( D_IM_LTM_PIPE1, &input_addr );
		Im_LTM_LTM_Set_OutData_Info( D_IM_LTM_PIPE1, &out_data_info );
		Im_LTM_LTM_Set_MapImage_Resize_Pitch( D_IM_LTM_PIPE1, &resize_pitch );
		Im_LTM_LTM_Set_Internal_Param( D_IM_LTM_PIPE1, &internal_param );
		Im_LTM_LTM_Ctrl_Ylog_Tbl( D_IM_LTM_PIPE1, &ylog_ctrl );
		Im_LTM_LTM_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_00, ylog_tbl );
		Im_LTM_LTM_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_01, ylog_tbl );

		// Start LTM
		Im_LTM_LTM_Start( D_IM_LTM_PIPE1 );

		// Wait LTM is normal end.
		Im_LTM_LTM_WaitEnd( NULL, D_IM_LTM_LTM1_INT_FLG_LTM_END, 60 );

		Im_LTM_LTM_Stop( D_IM_LTM_PIPE1 );
	}

	VOID User_LTM_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_LTM_LTM1_INT_FLG_AXR_ERR:
			case D_IM_LTM_LTM1_INT_FLG_AXW_ERR:
			case D_IM_LTM_LTM1_INT_FLG_YLOG_ERR:
				printf( "LTM LTM Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>

@section im_ltm_sample_section8	Sample code of LTM block's data setting for B2R direct input mode.
	@code
	#define D_USER_YN_TOP_X_PIXS 0
	#define D_USER_YN_TOP_Y_PIXS 0
	#define D_USER_YN_WIDTH_PIXS 640
	#define D_USER_YN_LINES_PIXS 480

	#define D_USER_YP_TOP_X_PIXS 0
	#define D_USER_YP_TOP_Y_PIXS 0
	#define D_USER_YP_WIDTH_PIXS 640
	#define D_USER_YP_LINES_PIXS 480

	#define D_USER_RGB_TOP_X_PIXS 0
	#define D_USER_RGB_TOP_Y_PIXS 0
	#define D_USER_RGB_WIDTH_PIXS 640
	#define D_USER_RGB_LINES_PIXS 480

	#define D_USER_YN_IN_LINEFEED_BYTES (D_USER_YN_WIDTH_PIXS * 2)
	#define D_USER_YP_IN_LINEFEED_BYTES (D_USER_YP_WIDTH_PIXS * 2)
	#define D_USER_RGB_IN_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 3 * 2)
	#define D_USER_RGB_OUT_LINEFEED_BYTES (D_USER_RGB_WIDTH_PIXS * 2)
	#define D_USER_YN_IN_ADDR 0x00000000
	#define D_USER_YP_IN_ADDR 0x10000000
	#define D_USER_R_IN_ADDR 0x20000000
	#define D_USER_G_IN_ADDR 0x30000000
	#define D_USER_B_IN_ADDR 0x40000000
	#define D_USER_R_OUT_ADDR 0x50000000
	#define D_USER_G_OUT_ADDR 0x60000000
	#define D_USER_B_OUT_ADDR 0x70000000

	VOID User_LTM_Handler( UINT32* status, UINT32 user_param );

	VOID User_SDRAM_Input_LTM_Process( VOID ) {
		T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl;
		T_IM_LTM_LTM_CTRL_B2R_DIRECT ltm_ctrl_direct;
		T_IM_LTM_LTM_INDATA_INFO input_data_info;
		T_IM_LTM_LTM_OUTDATA_INFO out_data_info;
		T_IM_LTM_LTM_MAP_RESIZE_PITCH resize_pitch;
		T_IM_LTM_INTERNAL_PARAM internal_param;
		T_IM_LTM_CTRL_YLOG ylog_ctrl;
		USHORT ylog_tbl[512] = {
			    0,   31,   47,   63,   79,   95,  111,  127,  143,  159,  175,  191,  207,  223,  239,  255,
			  271,  287,  303,  319,  335,  351,  367,  383,  399,  415,  431,  447,  463,  479,  495,  511,
			  527,  543,  559,  575,  591,  607,  623,  639,  655,  671,  687,  703,  719,  735,  751,  767,
			  783,  799,  815,  831,  847,  863,  879,  895,  911,  927,  943,  959,  975,  991, 1007, 1023,
			 1039, 1055, 1071, 1087, 1103, 1119, 1135, 1151, 1167, 1183, 1199, 1215, 1231, 1247, 1263, 1279,
			 1295, 1311, 1327, 1343, 1359, 1375, 1391, 1407, 1423, 1439, 1455, 1471, 1487, 1503, 1519, 1535,
			 1551, 1567, 1583, 1599, 1615, 1631, 1647, 1663, 1679, 1695, 1711, 1727, 1743, 1759, 1775, 1791,
			 1807, 1823, 1839, 1855, 1871, 1887, 1903, 1919, 1935, 1951, 1967, 1983, 1999, 2015, 2031, 2047,
			 2063, 2079, 2095, 2111, 2127, 2143, 2159, 2175, 2191, 2207, 2223, 2239, 2255, 2271, 2287, 2303,
			 2319, 2335, 2351, 2367, 2383, 2399, 2415, 2431, 2447, 2463, 2479, 2495, 2511, 2527, 2543, 2559,
			 2575, 2591, 2607, 2623, 2639, 2655, 2671, 2687, 2703, 2719, 2735, 2751, 2767, 2783, 2799, 2815,
			 2831, 2847, 2863, 2879, 2895, 2911, 2927, 2943, 2959, 2975, 2991, 3007, 3023, 3039, 3055, 3071,
			 3087, 3103, 3119, 3135, 3151, 3167, 3183, 3199, 3215, 3231, 3247, 3263, 3279, 3295, 3311, 3327,
			 3343, 3359, 3375, 3391, 3407, 3423, 3439, 3455, 3471, 3487, 3503, 3519, 3535, 3551, 3567, 3583,
			 3599, 3615, 3631, 3647, 3663, 3679, 3695, 3711, 3727, 3743, 3759, 3775, 3791, 3807, 3823, 3839,
			 3855, 3871, 3887, 3903, 3919, 3935, 3951, 3967, 3983, 3999, 4015, 4031, 4047, 4063, 4079, 4095,
			 4111, 4127, 4143, 4159, 4175, 4191, 4207, 4223, 4239, 4255, 4271, 4287, 4303, 4319, 4335, 4351,
			 4367, 4383, 4399, 4415, 4431, 4447, 4463, 4479, 4495, 4511, 4527, 4543, 4559, 4575, 4591, 4607,
			 4623, 4639, 4655, 4671, 4687, 4703, 4719, 4735, 4751, 4767, 4783, 4799, 4815, 4831, 4847, 4863,
			 4879, 4895, 4911, 4927, 4943, 4959, 4975, 4991, 5007, 5023, 5039, 5055, 5071, 5087, 5103, 5119,
			 5135, 5151, 5167, 5183, 5199, 5215, 5231, 5247, 5263, 5279, 5295, 5311, 5327, 5343, 5359, 5375,
			 5391, 5407, 5423, 5439, 5455, 5471, 5487, 5503, 5519, 5535, 5551, 5567, 5583, 5599, 5615, 5631,
			 5647, 5663, 5679, 5695, 5711, 5727, 5743, 5759, 5775, 5791, 5807, 5823, 5839, 5855, 5871, 5887,
			 5903, 5919, 5935, 5951, 5967, 5983, 5999, 6015, 6031, 6047, 6063, 6079, 6095, 6111, 6127, 6143,
			 6159, 6175, 6191, 6207, 6223, 6239, 6255, 6271, 6287, 6303, 6319, 6335, 6351, 6367, 6383, 6399,
			 6415, 6431, 6447, 6463, 6479, 6495, 6511, 6527, 6543, 6559, 6575, 6591, 6607, 6623, 6639, 6655,
			 6671, 6687, 6703, 6719, 6735, 6751, 6767, 6783, 6799, 6815, 6831, 6847, 6863, 6879, 6895, 6911,
			 6927, 6943, 6959, 6975, 6991, 7007, 7023, 7039, 7055, 7071, 7087, 7103, 7119, 7135, 7151, 7167,
			 7183, 7199, 7215, 7231, 7247, 7263, 7279, 7295, 7311, 7327, 7343, 7359, 7375, 7391, 7407, 7423,
			 7439, 7455, 7471, 7487, 7503, 7519, 7535, 7551, 7567, 7583, 7599, 7615, 7631, 7647, 7663, 7679,
			 7695, 7711, 7727, 7743, 7759, 7775, 7791, 7807, 7823, 7839, 7855, 7871, 7887, 7903, 7919, 7935,
			 7951, 7967, 7983, 7999, 8015, 8031, 8047, 8063, 8079, 8095, 8111, 8127, 8143, 8159, 8175, 8191
		}

		ltm_ctrl.line_intr            = 0;
		ltm_ctrl.rch_burst_length_yp  = D_IM_LTM_BRST_512;
		ltm_ctrl.rch_burst_length_yn  = D_IM_LTM_BRST_512;
		ltm_ctrl.wch_burst_length_r   = D_IM_LTM_BRST_512;
		ltm_ctrl.wch_burst_length_g   = D_IM_LTM_BRST_512;
		ltm_ctrl.wch_burst_length_b   = D_IM_LTM_BRST_512;
		ltm_ctrl.mask_write_r         = D_IM_LTM_MASK_WRITE_ENABLE;
		ltm_ctrl.mask_write_g         = D_IM_LTM_MASK_WRITE_ENABLE;
		ltm_ctrl.mask_write_b         = D_IM_LTM_MASK_WRITE_ENABLE;
		ltm_ctrl.read_select_yp       = D_IM_LTM_PORT_DEFAULT;
		ltm_ctrl.read_select_yn       = D_IM_LTM_PORT_DEFAULT;
		ltm_ctrl.in_data_type_yp      = D_IM_LTM_DTYP_UNPACK12;
		ltm_ctrl.in_data_type_yn      = D_IM_LTM_DTYP_UNPACK12;
		ltm_ctrl.out_select           = D_IM_LTM_LTM_OUT_SDRAM;
		ltm_ctrl.out_data_type        = D_IM_LTM_DTYP_UNPACK12;
		ltm_ctrl.input_rect.img_left  = D_USER_YN_TOP_X_PIXS;
		ltm_ctrl.input_rect.img_top   = D_USER_YN_TOP_Y_PIXS;
		ltm_ctrl.input_rect.img_width = D_USER_YN_WIDTH_PIXS;
		ltm_ctrl.input_rect.img_lines = D_USER_YN_LINES_PIXS;
		ltm_ctrl.global_width_yp      = D_USER_Y_IN_LINEFEED_BYTES;
		ltm_ctrl.global_width_yn      = D_USER_Y_IN_LINEFEED_BYTES;
		ltm_ctrl.ltm_user_handler     = User_LTM_Handler;
		ltm_ctrl.user_param           = 0;

		ltm_ctrl_direct.input_rect.img_left  = D_USER_RGB_TOP_X_PIXS;
		ltm_ctrl_direct.input_rect.img_top   = D_USER_RGB_TOP_Y_PIXS;
		ltm_ctrl_direct.input_rect.img_width = D_USER_RGB_WIDTH_PIXS;
		ltm_ctrl_direct.input_rect.img_lines = D_USER_RGB_LINES_PIXS;
		ltm_ctrl_direct.bypass               = D_IM_LTM_ENABLE_OFF;

		input_data_info.addr_yp           = (VOID*)D_USER_YN_IN_ADDR;
		input_data_info.addr_yn           = (VOID*)D_USER_YP_IN_ADDR;
		input_data_info.top_bit_offset_yp = 0;
		input_data_info.top_bit_offset_yn = 0;

		out_data_info.addr[D_IM_LTM_PORT_R]         = (VOID*)D_USER_R_OUT_ADDR;
		out_data_info.addr[D_IM_LTM_PORT_G]         = (VOID*)D_USER_G_OUT_ADDR;
		out_data_info.addr[D_IM_LTM_PORT_B]         = (VOID*)D_USER_B_OUT_ADDR;
		out_data_info.global_width[D_IM_LTM_PORT_R] = D_USER_RGB_OUT_LINEFEED_BYTES;
		out_data_info.global_width[D_IM_LTM_PORT_G] = D_USER_RGB_OUT_LINEFEED_BYTES;
		out_data_info.global_width[D_IM_LTM_PORT_B] = D_USER_RGB_OUT_LINEFEED_BYTES;

		resize_pitch.hsta = 0;
		resize_pitch.hpit = 0x4000;
		resize_pitch.vsta = 0;
		resize_pitch.vpit = 0x4000;

		Im_LTM_MAP_Get_Internal_Param( D_IM_LTM_PIPE1, &internal_param );

		ylog_ctrl.use_tbl         = D_IM_LTM_YLOG_TBL_SIDE_0;
		ylog_ctrl.end_point0_tbl0 = 0x3FFF;
		ylog_ctrl.end_point0_tbl1 = 0x3FFF;
		ylog_ctrl.end_point1_tbl0 = 0x3FFF;
		ylog_ctrl.end_point1_tbl1 = 0x3FFF;

		Im_LTM_LTM_Ctrl_Common( D_IM_LTM_PIPE1, &ltm_ctrl );
		Im_LTM_LTM_Ctrl_ModeB2RDirect( D_IM_LTM_PIPE1, &ltm_ctrl_direct );
		Im_LTM_LTM_Set_InData_Info( D_IM_LTM_PIPE1, &input_addr );
		Im_LTM_LTM_Set_OutData_Info( D_IM_LTM_PIPE1, &out_data_info );
		Im_LTM_LTM_Set_MapImage_Resize_Pitch( D_IM_LTM_PIPE1, &resize_pitch );
		Im_LTM_LTM_Set_Internal_Param( D_IM_LTM_PIPE1, &internal_param );
		Im_LTM_LTM_Ctrl_Ylog_Tbl( D_IM_LTM_PIPE1, &ylog_ctrl );
		Im_LTM_LTM_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_00, ylog_tbl );
		Im_LTM_LTM_Set_YLOG_Table( D_IM_LTM_PIPE1, D_LTM_YLOG_TBL_01, ylog_tbl );

		// Start LTM
		Im_LTM_LTM_Start( D_IM_LTM_PIPE1 );

		// Wait LTM is normal end.
		Im_LTM_LTM_WaitEnd( NULL, D_IM_LTM_LTM1_INT_FLG_LTM_END, 60 );

		Im_LTM_LTM_Stop( D_IM_LTM_PIPE1 );
	}

	VOID User_LTM_Handler( UINT32* status, UINT32 user_param )
	{
		switch( *status ) {
			case D_IM_LTM_LTM1_INT_FLG_AXR_ERR:
			case D_IM_LTM_LTM1_INT_FLG_AXW_ERR:
			case D_IM_LTM_LTM1_INT_FLG_YLOG_ERR:
				printf( "LTM LTM Error. %lx\n", user_param );
				break;
			default:
				// Nothing to do.
				break;
		}
	}
	@endcode
	<br><br>

@}*/	// weakgroup im_ltm

