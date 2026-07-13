/**
 * @file		im_pro.h
 * @brief		preprocess macro api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IM_PRO_H_
#define _IM_PRO_H_

#include "jdspro.h"
#include "driver_common.h"
#include "ddim_user_custom.h"

/** @addtogroup im_pro_overview overview
@{
	This chapter describes about overview of PRO macro functions.<br>
	PRO macro is processer of bayer(RAW) data readout from external sensor modules.<br>
	PRO macro has function of resize, noise reduction, optical property correction, and can measure statistic value of each bayer pixels.<br>
	<br>
	- @ref im_pro_overview_hw_arch
	- @ref im_pro_overview_driver_arch

	<hr>
	@section im_pro_overview_hw_arch	H/W architecure of PRO macro
		@image html im_pro_hw_arch.png

		<table>
			<tr>
				<th>Unit</th>
				<th>Number of Unit</th>
				<th>Number of Interrupt</th>
				<th>Details</th>
				<th>Restrictions</th>
			</tr>
			<tr>
				<td>SEN</td>
				<td>1</td>
				<td>7</td>
				<td>joint unit between external SENsor module.<br>
					SEN unit has 4 interfaces(SLVS / LVDS / MIPI-D-PHY / MIPI-C-PHY).<br>
					About more detail of this unit, please refer to @ref im_pro_sen.</td>
				<td>Maximum processing size: 11840(H)x 8960(V)</td>
			</tr>
			<tr>
				<td>SRO</td>
				<td>2</td>
				<td>6</td>
				<td>Pre process unit of bayer(Sensor ReadOut unit).<br>
					About more detail of this unit, please refer to @ref im_pro_sro.</td>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
				<td>Maximum processing size:  2496(H)x 8960(V)</td>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
				<td>Maximum processing size:  2900(H)x 8960(V)</td>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			</tr>
			<tr>
				<td>B2B</td>
				<td>2</td>
				<td>6</td>
				<td>Post process unit of bayer(Bayer To Bayer unit)<br>
					About more detail of this unit, please refer to @ref im_pro_b2b.</td>
				<td>Maximum processing size:  2496(H)x 8960(V)</td>
			</tr>
			<tr>
				<td>STAT</td>
				<td>1</td>
				<td>1</td>
				<td>measurement STATistic of bayer pixels.<br>
					About more detail of this unit, please refer to @ref im_pro_stat.</td>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
				<td>Maximum processing size:  2496(H)x 8960(V)</td>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
				<td>Maximum processing size:  2900(H)x 8960(V)</td>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			</tr>
			<tr>
				<td>PAS</td>
				<td>1</td>
				<td>3</td>
				<td>performing processing for PDAF pixels.<br>
					About more detail of this unit, please refer to @ref im_pro_pas.</td>
				<td>Maximum processing size: 3825(H)x 3825(V)</td>
			</tr>
		</table>
	<br>
	<hr>
	@section im_pro_overview_driver_arch	Driver architecure of PRO macro
		@image html im_pro_driver_arch.png
		Each H/W unit has some macros as above figure. and some macros scattered on each units.<br>
		In addition, each H/W unit(macros) issues some IRQ.<br>
		<br>
		Therefore, PRO macro driver classify each macros I/F into 7 categorys of SEN / SRO / B2B / STAT / PAS / Common macro / IRQ handler.<br>
		Common macro I/F controls each scattered macro with common API I/F.<br>
		IRQ handler I/F controls each interrupt handler process.<br>
		<br>

		<table>
			<tr>
				<th>Unit</th>
				<th>Macro</th>
				<th>Functional overview</th>
			<tr>
				<td rowspan="9">@ref im_pro_sen</td>
				<td>@ref im_pro_sen_top</td>
				<td>Performs global control of SEN unit.(Operation mode select, SR/CLK/PAT/Interrupt/VDHD interrupt control)</td></tr>
			<tr>
				<td>@ref im_pro_sen_sg</td>
				<td>Generates synchronous signals of VD/HD and outputs them to the outside.</td></tr>
			<tr>
				<td>@ref im_pro_sen_lvds</td>
				<td>subLVDS of Data-Clock format.</td></tr>
			<tr>
				<td>@ref im_pro_sen_slvs</td>
				<td>Receives image data from SLVS-EC sensor.</td></tr>
			<tr>
				<td>@ref im_pro_sen_dmipi</td>
				<td>Receives image data from MIPI (D-PHY I/F) sensor.</td></tr>
			<tr>
				<td>MIPI-CPHY</td>
				<td>Receives image data from MIPI (C-PHY I/F) sensor.</td></tr>
			<tr>
				<td>@ref im_pro_sen_ldiv</td>
				<td>Converts the output of the sensor I/F Macro (@ref im_pro_sen_lvds, @ref im_pro_sen_slvs, @ref im_pro_sen_dmipi, C-PHY I/F)to 2 lines: odd line and even line, divides them into up to 6 rectangles and then outputs.</td></tr>
			<tr>
				<td>@ref im_pro_sen_obd</td>
				<td>Calculates the evaluation value of OB.</td></tr>
			<tr>
				<td>@ref im_pro_sen_obt</td>
				<td>Detects and corrects a TOP OB. Removal of a fixed OB is also supported.</td></tr>
			<tr>
				<td rowspan="11">@ref im_pro_sro</td>
				<td>@ref im_pro_sro_top</td>
				<td>Performs global control of SRO unit. (Operation mode select (SROISW and etc.), SR/CLK/PAT/Interrupt/VDHD interrupt control)</td></tr>
			<tr>
				<td>@ref im_pro_sro_ffsh</td>
				<td>Generates coefficients for gain correction of PDAF surrounding pixels. </td></tr>
			<tr>
				<td>@ref im_pro_sro_ff</td>
				<td>Performs dark frame subtraction processing in cooperation with @ref im_pro_common_prch/@ref im_pro_common_m2p. </td></tr>
			<tr>
				<td>@ref im_pro_sro_sdc</td>
				<td>Replaces the static defective pixel with an iso-color adjacent pixel in cooperation with @ref im_pro_common_prch, or performs gain correction.<br>Also generates PDAF pixel pattern information and performs gain corrections for the surrounding pixels.</td></tr>
			<tr>
				<td>@ref im_pro_sro_dpc</td>
				<td>Detects/Corrects the defective pixels and extracts the PDAF pixels. </td></tr>
			<tr>
				<td>@ref im_pro_sro_afpb</td>
				<td>Packs the discontinuous PDAF information for @ref im_pro_common_p2m/@ref im_pro_common_pwch. </td></tr>
			<tr>
				<td>@ref im_pro_sro_fshdl</td>
				<td>Supports shading correction using field table by the color in cooperation with @ref im_pro_common_prch/@ref im_pro_common_m2p. </td></tr>
			<tr>
				<td>@ref im_pro_sro_ffc</td>
				<td>Generates frame correction coefficients for @ref im_pro_sro_cag. </td></tr>
			<tr>
				<td>@ref im_pro_sro_cag</td>
				<td>Performs magnification chromatic aberration correction of the lens and gradient correction of Gr/Gb pixels. </td></tr>
			<tr>
				<td>@ref im_pro_sro_vrb</td>
				<td>Performs size reduction processing in vertical direction and pixel gap equalization. </td></tr>
			<tr>
				<td>@ref im_pro_sro_hrb</td>
				<td>Performs size reduction processing in horizontal direction and pixel gap equalization. </td></tr>
			<tr>
				<td rowspan="3">@ref im_pro_b2b</td>
				<td>@ref im_pro_b2b_top</td>
				<td>Performs global control of B2B unit. (Operation mode select(B2BISW, CAGSL), SR/CLK/PAT/Interrupt/VDHD interrupt control)</td></tr>
			<tr>
				<td>@ref im_pro_b2b_elf</td>
				<td>Implemented with a ELF4.6 filter to perform noise reduction. </td></tr>
			<tr>
				<td>@ref im_pro_b2b_fshd</td>
				<td>Performs shading correction by the color using frame table in cooperation with @ref im_pro_common_prch/@ref im_pro_common_m2p. Concentric circles correction using table RAM is also supported.</td></tr>
			<tr>
				<td rowspan="5">@ref im_pro_stat</td>
				<td>@ref im_pro_stat_top</td>
				<td>Performs global control of STAT unit.(Input select (AFSW*, AESW*, HISW*, FLSW*), SR/CLK/Interrupt)</td></tr>
			<tr>
				<td>@ref im_pro_stat_af</td>
				<td>Calculates the evaluation value of AF.AF 15 frames*14 types of evaluation result can be obtained.</td></tr>
			<tr>
				<td>@ref im_pro_stat_aeawb</td>
				<td>Calculates evaluation values of AE and AWB. The evaluation result can be automatically transferred to SDRAM in cooperation with @ref im_pro_common_pwch.<br>Only AEAWB0/1 can support AWB up to 8 levels.</td></tr>
			<tr>
				<td>@ref im_pro_stat_hist</td>
				<td>Calculates the evaluation value of histogram. </td></tr>
			<tr>
				<td>@ref im_pro_stat_flck</td>
				<td>Calculates evaluation value of flicker. The evaluation result can be automatically transferred to SDRAM in cooperation with @ref im_pro_common_pwch. </td></tr>
			<tr>
				<td rowspan="2">@ref im_pro_pas</td>
				<td>@ref im_pro_pas_top</td>
				<td>Performs global control of PAS unit.</td></tr>
			<tr>
				<td>@ref im_pro_pas_spt</td>
				<td>Performs shading correction for PDAF by the color using frame table in cooperation with @ref im_pro_common_prch/@ref im_pro_common_m2p. Concentric circles correction using table RAM is also supported.</td></tr>
			<tr>
				<td rowspan="9">@ref im_pro_common</td>
				<td>@ref im_pro_common_pg</td>
				<td>Re-inputs images in cooperation with @ref im_pro_common_prch/@ref im_pro_common_m2p. Adds VD/HD to image data stored in SDRAM. </td></tr>
			<tr>
				<td>@ref im_pro_common_pgain</td>
				<td>Supports offset adjustment and multiplication adjustment for each color. </td></tr>
			<tr>
				<td>@ref im_pro_common_pzsft</td>
				<td>Offset +/- shift </td></tr>
			<tr>
				<td>@ref im_pro_common_lnr</td>
				<td>Corrects linearity of a sensor by the color.This macro uses 7-dot-8-section registration format.</td></tr>
			<tr>
				<td>@ref im_pro_common_moni</td>
				<td>Monitors the output of sensor I/F, and can compare it with the specified expected value then output an error interrupt.</td></tr>
			<tr>
				<td>@ref im_pro_common_p2m</td>
				<td>Transfers image data to SDRAM in cooperation with @ref im_pro_common_pwch. In P2M macro, as the processing just before data transfer, the following processing are supported.<br>(Right bit shift, Knee processing, Horizontal LPF, Trimming, Subsampling, PACKing)</td></tr>
			<tr>
				<td>@ref im_pro_common_pwch</td>
				<td>Transfers Bayer data that is the output of this macro to SDRAM. </td></tr>
			<tr>
				<td>@ref im_pro_common_m2p</td>
				<td>Transfers image data stored in SDRAM into this macro in cooperation with @ref im_pro_common_prch. In M2P macro, as the processing immediately after data input, the following processing are supported.<br>(UNPACKing, Dekeen processing, Left/Right bit shift) M2P L does not support Deknee function. </td></tr>
			<tr>
				<td>@ref im_pro_common_prch</td>
				<td>Inputs image data and correction coefficients from SDRAM. </td></tr>
			<tr>
				<td rowspan="1">@ref im_pro_handler</td>
				<td>-</td>
				<td>IRQ Handler I/F. </td></tr>
		</table>
*/

/*	@}*/	// im_pro_overview

/** @addtogroup im_pro_basic_seq Basic operation sequence
@{
	This chapter describes about basic operation sequence of PRO macro driver.<br>
	some unique sequences are described at each macro section, please refer to them.
	<br>
	- @ref initialization_sequence
	- @ref basic_operation_sequence

	<hr>
	@section initialization_sequence	Initialization sequence.
	@image html im_pro_init_seq.png

	<hr>
	@section basic_operation_sequence	Basic Operation Sequence
	@image html im_pro_basic_operate_seq.png
*/

/*	@}*/	// im_pro_basic_seq

/** @addtogroup im_pro_sen SEN unit
@{
	@image html im_pro_sen_arch.png
	Structure of SEN unit is as above figure.<br>
	About each function block's detail, please refer to chapters of them.<br>
	Moreover, about	@ref im_pro_common_lnr, 
					@ref im_pro_common_moni,
					@ref im_pro_common_pwch,
					and @ref im_pro_common_p2m functions, please refer to @ref im_pro_common chapter.
*/

/*	@}*/	// im_pro_sen

/** @weakgroup im_pro_sen
@{*/
	/**
	@defgroup im_pro_sen_top	SENTOP
	@defgroup im_pro_sen_sg		SG
	@defgroup im_pro_sen_slvs	SLVS-EC
	@defgroup im_pro_sen_lvds	LVDS
	@defgroup im_pro_sen_dmipi	MIPI-DPHY
	@defgroup im_pro_sen_ldiv	LDIV
	@defgroup im_pro_sen_obd	OBD
	@defgroup im_pro_sen_obt	OBT
	*/

/*@}*/


/** @addtogroup im_pro_sro SRO unit
@{
	@image html im_pro_sro_arch.png
	Structure of SRO unit is as above figure.<br>
	About each function block's detail, please refer to chapters of them.<br>
	Moreover, about	@ref im_pro_common_prch, 
				@ref im_pro_common_m2p, 
				@ref im_pro_common_pg, 
				@ref im_pro_common_pgain, 
				@ref im_pro_common_pzsft, 
				@ref im_pro_common_pwch, 
				and @ref im_pro_common_p2m functions, please refer to @ref im_pro_common chapter.
*/

/*	@}*/	// im_pro_sro

/** @weakgroup im_pro_sro
@{*/
	/**
	@defgroup im_pro_sro_top	SROTOP
	@defgroup im_pro_sro_ffsh	FFSH
	@defgroup im_pro_sro_ff		FF
	@defgroup im_pro_sro_sdc	SDC
	@defgroup im_pro_sro_dpc	DPC
	@defgroup im_pro_sro_afpb	AFPB
	@defgroup im_pro_sro_fshdl	FSHDL
	@defgroup im_pro_sro_ffc	FFC
	@defgroup im_pro_sro_cag	CAG
	@defgroup im_pro_sro_vrb	VRB
	@defgroup im_pro_sro_hrb	HRB
// --- REMOVE_BUZ BEGIN
// @cond
	@defgroup im_pro_sro_buztop	BUZTOP
// @endcond
// --- REMOVE_BUZ END
	*/
/*@}*/


/** @addtogroup im_pro_b2b B2B unit
@{
	@image html im_pro_b2b_arch.png
	Structure of B2B unit is as above figure.<br>
	About each function block's detail, please refer to chapters of them.<br>
	Moreover, about	@ref im_pro_common_prch, 
				@ref im_pro_common_m2p, 
				@ref im_pro_common_pg, 
				@ref im_pro_common_pgain, 
				@ref im_pro_common_pzsft, 
				@ref im_pro_common_pwch, 
				and @ref im_pro_common_p2m functions, please refer to @ref im_pro_common chapter.
*/

/*	@}*/	// im_pro_b2b

/** @weakgroup im_pro_b2b
@{*/
	/**
	@defgroup im_pro_b2b_top	B2BTOP
	@defgroup im_pro_b2b_elf	ELF
	@defgroup im_pro_b2b_fshd	FSHD
	*/
/*@}*/


/** @addtogroup im_pro_stat STAT unit
@{
	@image html im_pro_stat_arch.png
	Structure of STAT unit is as above figure.<br>
	About each function block's detail, please refer to chapters of them.<br>
	Moreover, about @ref im_pro_common_pwch functions, please refer to @ref im_pro_common chapter.
*/


/*	@}*/	// im_pro_stat

/** @weakgroup im_pro_stat
@{*/
	/**
	@defgroup im_pro_stat_top	STATTOP
	@defgroup im_pro_stat_af	AF
	@defgroup im_pro_stat_aeawb	AEAWB
	@defgroup im_pro_stat_hist	HIST
	@defgroup im_pro_stat_flck	FLCK
	*/
/*@}*/


/** @addtogroup im_pro_pas PAS unit
@{
	@image html im_pro_pas_arch.png
	Structure of PAS unit is as above figure.<br>
	About each function block's detail, please refer to chapters of them.<br>
	Moreover, about	@ref im_pro_common_prch, 
				@ref im_pro_common_m2p, 
				@ref im_pro_common_pg, 
				@ref im_pro_common_pgain, 
				@ref im_pro_common_lnr, 
				@ref im_pro_common_pwch, 
				and @ref im_pro_common_p2m functions, please refer to @ref im_pro_common chapter.
*/

/*	@}*/	// im_pro_pas

/** @weakgroup im_pro_pas
@{*/
	/**
	@defgroup im_pro_pas_top	PASTOP
	@defgroup im_pro_pas_spt	SPT
	*/
/*@}*/


/** @addtogroup im_pro_common Common unit
@{
	This chapter describes about Common macro(scattered to each units) functions.<br>
*/

/*	@}*/	// im_pro_common

/** @weakgroup im_pro_common
@{*/
	/**
	@defgroup im_pro_common_p2m		P2M
	@defgroup im_pro_common_pwch	PWCH
	@defgroup im_pro_common_m2p		M2P
	@defgroup im_pro_common_prch	PRCH
	@defgroup im_pro_common_lnr		LNR
	@defgroup im_pro_common_pg		PG
	@defgroup im_pro_common_pgain	PGAIN
	@defgroup im_pro_common_pzsft	PZSFT
	@defgroup im_pro_common_moni	MONI
	*/
/*@}*/


/** @addtogroup im_pro_handler Interrupt Handler
@{
	This chapter describes about Interrupt handler functions.<br>
*/

/*	@}*/	// im_pro_handler

/** @weakgroup im_pro_handler
@{*/
	/**
	@defgroup im_pro_handler_sen	SEN Interrupt
	@defgroup im_pro_handler_sro	SRO Interrupt
	@defgroup im_pro_handler_b2b	B2B Interrupt
	@defgroup im_pro_handler_stat	STAT Interrupt
	@defgroup im_pro_handler_pas	PAS Interrupt
	@defgroup im_pro_handler_common	Common Interrupt
	*/

/*@}*/	// im_pro_handler

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** PRO Return value of processing result
*/
#define D_IM_PRO_INPUT_PARAM_ERROR	(D_IM_PRO | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error	*/
#define D_IM_PRO_NG					(D_IM_PRO | D_DDIM_NO_RECEIVE)			/**< Error					*/
#define D_IM_PRO_MACRO_BUSY_NG		(D_IM_PRO | D_DDIM_MACRO_BUSY_NG)		/**< Clock is enable */
#define	D_IM_PRO_TIMEOUT			(D_IM_PRO | D_DDIM_TIMEOUT)				/**< timeout				*/
#define	D_IM_PRO_SYSTEM_ERROR		(D_IM_PRO | D_DDIM_SYSTEM_ERROR)		/**< System call error		*/

/** @addtogroup im_pro_handler_sen	SEN Interrupt
@{
*/
#define	D_IM_PRO_INT_VDE00								(1<< 0)											/**< SENCORE0 VD0 interrupt enable	*/
#define	D_IM_PRO_INT_VDE01								(1<< 4)											/**< SENCORE0 VD1 interrupt enable	*/
#define	D_IM_PRO_INT_VDE10								(1<< 8)											/**< SENCORE1 VD0 interrupt enable	*/
#define	D_IM_PRO_INT_VDE11								(1<<12)											/**< SENCORE1 VD1 interrupt enable	*/
#define	D_IM_PRO_INT_VDE20								(1<<16)											/**< SENCORE2 VD0 interrupt enable	*/
#define	D_IM_PRO_INT_VDE21								(1<<20)											/**< SENCORE2 VD1 interrupt enable	*/
#define	D_IM_PRO_INT_VDE30								(1<<24)											/**< SENCORE3 VD0 interrupt enable	*/
#define	D_IM_PRO_INT_VDE31								(1<<28)											/**< SENCORE3 VD1 interrupt enable	*/
#define	D_IM_PRO_INT_HDE00								(1<< 0)											/**< SENCORE0 HD0 interrupt enable	*/
#define	D_IM_PRO_INT_HDE01								(1<< 4)											/**< SENCORE0 HD1 interrupt enable	*/
#define	D_IM_PRO_INT_HDE10								(1<< 8)											/**< SENCORE1 HD0 interrupt enable	*/
#define	D_IM_PRO_INT_HDE11								(1<<12)											/**< SENCORE1 HD1 interrupt enable	*/
#define	D_IM_PRO_INT_HDE20								(1<<16)											/**< SENCORE2 HD0 interrupt enable	*/
#define	D_IM_PRO_INT_HDE21								(1<<20)											/**< SENCORE2 HD1 interrupt enable	*/
#define	D_IM_PRO_INT_HDE30								(1<<24)											/**< SENCORE3 HD0 interrupt enable	*/
#define	D_IM_PRO_INT_HDE31								(1<<28)											/**< SENCORE3 HD1 interrupt enable	*/

#define	D_IM_PRO_INT_VDF00								(D_IM_PRO_INT_VDE00)							/**< SENCORE0 VD0 interrupt flag	*/
#define	D_IM_PRO_INT_VDF01								(D_IM_PRO_INT_VDE01)							/**< SENCORE0 VD1 interrupt flag	*/
#define	D_IM_PRO_INT_VDF10								(D_IM_PRO_INT_VDE10)							/**< SENCORE1 VD0 interrupt flag	*/
#define	D_IM_PRO_INT_VDF11								(D_IM_PRO_INT_VDE11)							/**< SENCORE1 VD1 interrupt flag	*/
#define	D_IM_PRO_INT_VDF20								(D_IM_PRO_INT_VDE20)							/**< SENCORE2 VD0 interrupt flag	*/
#define	D_IM_PRO_INT_VDF21								(D_IM_PRO_INT_VDE21)							/**< SENCORE2 VD1 interrupt flag	*/
#define	D_IM_PRO_INT_VDF30								(D_IM_PRO_INT_VDE30)							/**< SENCORE3 VD0 interrupt flag	*/
#define	D_IM_PRO_INT_VDF31								(D_IM_PRO_INT_VDE31)							/**< SENCORE3 VD1 interrupt flag	*/
#define	D_IM_PRO_INT_HDF00								(D_IM_PRO_INT_HDE00)							/**< SENCORE0 HD0 interrupt flag	*/
#define	D_IM_PRO_INT_HDF01								(D_IM_PRO_INT_HDE01)							/**< SENCORE0 HD1 interrupt flag	*/
#define	D_IM_PRO_INT_HDF10								(D_IM_PRO_INT_HDE10)							/**< SENCORE1 HD0 interrupt flag	*/
#define	D_IM_PRO_INT_HDF11								(D_IM_PRO_INT_HDE11)							/**< SENCORE1 HD1 interrupt flag	*/
#define	D_IM_PRO_INT_HDF20								(D_IM_PRO_INT_HDE20)							/**< SENCORE2 HD0 interrupt flag	*/
#define	D_IM_PRO_INT_HDF21								(D_IM_PRO_INT_HDE21)							/**< SENCORE2 HD1 interrupt flag	*/
#define	D_IM_PRO_INT_HDF30								(D_IM_PRO_INT_HDE30)							/**< SENCORE3 HD0 interrupt flag	*/
#define	D_IM_PRO_INT_HDF31								(D_IM_PRO_INT_HDE31)							/**< SENCORE3 HD1 interrupt flag	*/

#define	D_IM_PRO_INT_SGVDE0								(1<< 0)											/**< SG0 VD interrupt enable	*/
#define	D_IM_PRO_INT_SGHDE0								(1<< 4)											/**< SG0 HD interrupt enable	*/
#define	D_IM_PRO_INT_SGVDE1								(1<< 8)											/**< SG1 VD interrupt enable	*/
#define	D_IM_PRO_INT_SGHDE1								(1<<12)											/**< SG1 HD interrupt enable	*/
#define	D_IM_PRO_INT_SGVDE2								(1<<16)											/**< SG2 VD interrupt enable	*/
#define	D_IM_PRO_INT_SGHDE2								(1<<20)											/**< SG2 HD interrupt enable	*/
#define	D_IM_PRO_INT_SGVDE3								(1<<24)											/**< SG3 VD interrupt enable	*/
#define	D_IM_PRO_INT_SGHDE3								(1<<28)											/**< SG3 HD interrupt enable	*/

#define	D_IM_PRO_INT_SGVDF0								(D_IM_PRO_INT_SGVDE0)							/**< SG0 VD interrupt flag	*/
#define	D_IM_PRO_INT_SGHDF0								(D_IM_PRO_INT_SGHDE0)							/**< SG0 HD interrupt flag	*/
#define	D_IM_PRO_INT_SGVDF1								(D_IM_PRO_INT_SGVDE1)							/**< SG1 VD interrupt flag	*/
#define	D_IM_PRO_INT_SGHDF1								(D_IM_PRO_INT_SGHDE1)							/**< SG1 HD interrupt flag	*/
#define	D_IM_PRO_INT_SGVDF2								(D_IM_PRO_INT_SGVDE2)							/**< SG2 VD interrupt flag	*/
#define	D_IM_PRO_INT_SGHDF2								(D_IM_PRO_INT_SGHDE2)							/**< SG2 HD interrupt flag	*/
#define	D_IM_PRO_INT_SGVDF3								(D_IM_PRO_INT_SGVDE3)							/**< SG3 VD interrupt flag	*/
#define	D_IM_PRO_INT_SGHDF3								(D_IM_PRO_INT_SGHDE3)							/**< SG3 HD interrupt flag	*/

#define	D_IM_PRO_INT_SGVD_ALL							(D_IM_PRO_INT_SGVDF0 | D_IM_PRO_INT_SGVDF1 | D_IM_PRO_INT_SGVDF2 | D_IM_PRO_INT_SGVDF3)	/**< All SGVD interrupt flag	*/
#define	D_IM_PRO_INT_SGHD_ALL							(D_IM_PRO_INT_SGHDF0 | D_IM_PRO_INT_SGHDF1 | D_IM_PRO_INT_SGHDF2 | D_IM_PRO_INT_SGHDF3)	/**< All SGHD interrupt flag	*/

#define	D_IM_PRO_INT_GYRO_SOFE0							(1<< 0)											/**< for Gyro SOF0 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_SOFE1							(1<< 4)											/**< for Gyro SOF1 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_SOFE2							(1<< 8)											/**< for Gyro SOF2 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_SOFE3							(1<<12)											/**< for Gyro SOF3 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_EOFE0							(1<<16)											/**< for Gyro EOF0 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_EOFE1							(1<<20)											/**< for Gyro EOF1 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_EOFE2							(1<<24)											/**< for Gyro EOF2 interrupt enable	*/
#define	D_IM_PRO_INT_GYRO_EOFE3							(1<<28)											/**< for Gyro EOF3 interrupt enable	*/

#define	D_IM_PRO_INT_GYRO_SOFF0							(D_IM_PRO_INT_GYRO_SOFE0)						/**< for Gyro SOF0 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_SOFF1							(D_IM_PRO_INT_GYRO_SOFE1)						/**< for Gyro SOF1 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_SOFF2							(D_IM_PRO_INT_GYRO_SOFE2)						/**< for Gyro SOF2 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_SOFF3							(D_IM_PRO_INT_GYRO_SOFE3)						/**< for Gyro SOF3 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_EOFF0							(D_IM_PRO_INT_GYRO_EOFE0)						/**< for Gyro EOF0 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_EOFF1							(D_IM_PRO_INT_GYRO_EOFE1)						/**< for Gyro EOF1 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_EOFF2							(D_IM_PRO_INT_GYRO_EOFE2)						/**< for Gyro EOF2 interrupt flag	*/
#define	D_IM_PRO_INT_GYRO_EOFF3							(D_IM_PRO_INT_GYRO_EOFE3)						/**< for Gyro EOF3 interrupt flag	*/

#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE7_BIT		(1<<23)											/**< Lane7 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE6_BIT		(1<<22)											/**< Lane6 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE5_BIT		(1<<21)											/**< Lane5 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE4_BIT		(1<<20)											/**< Lane4 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE3_BIT		(1<<19)											/**< Lane3 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE2_BIT		(1<<18)											/**< Lane2 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE1_BIT		(1<<17)											/**< Lane1 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE0_BIT		(1<<16)											/**< Lane0 Invalid Code interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE7_BIT		(1<<15)											/**< Lane7 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE6_BIT		(1<<14)											/**< Lane6 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE5_BIT		(1<<13)											/**< Lane5 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE4_BIT		(1<<12)											/**< Lane4 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE3_BIT		(1<<11)											/**< Lane3 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE2_BIT		(1<<10)											/**< Lane2 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE1_BIT		(1<< 9)											/**< Lane1 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE0_BIT		(1<< 8)											/**< Lane0 Running Disparity Error interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE7_BIT		(1<< 7)											/**< Lane7 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE6_BIT		(1<< 6)											/**< Lane6 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE5_BIT		(1<< 5)											/**< Lane5 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE4_BIT		(1<< 4)											/**< Lane4 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE3_BIT		(1<< 3)											/**< Lane3 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE2_BIT		(1<< 2)											/**< Lane2 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE1_BIT		(1<< 1)											/**< Lane1 Illegal Point Comma interrupt flag	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE0_BIT		(1<< 0)											/**< Lane0 Illegal Point Comma interrupt flag	*/

#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE7_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE7_BIT	)	/**< Lane7 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE6_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE6_BIT	)	/**< Lane6 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE5_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE5_BIT	)	/**< Lane5 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE4_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE4_BIT	)	/**< Lane4 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE3_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE3_BIT	)	/**< Lane3 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE2_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE2_BIT	)	/**< Lane2 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE1_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE1_BIT	)	/**< Lane1 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE0_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IC_LANE0_BIT	)	/**< Lane0 Invalid Code interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE7_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE7_BIT	)	/**< Lane7 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE6_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE6_BIT	)	/**< Lane6 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE5_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE5_BIT	)	/**< Lane5 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE4_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE4_BIT	)	/**< Lane4 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE3_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE3_BIT	)	/**< Lane3 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE2_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE2_BIT	)	/**< Lane2 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE1_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE1_BIT	)	/**< Lane1 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE0_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_RDE_LANE0_BIT	)	/**< Lane0 Running Disparity Error interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE7_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE7_BIT	)	/**< Lane7 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE6_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE6_BIT	)	/**< Lane6 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE5_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE5_BIT	)	/**< Lane5 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE4_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE4_BIT	)	/**< Lane4 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE3_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE3_BIT	)	/**< Lane3 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE2_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE2_BIT	)	/**< Lane2 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE1_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE1_BIT	)	/**< Lane1 Illegal Point Comma interrupt enable	*/
#define	D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE0_BIT		(D_IM_PRO_SLVS_COMMON_PEINTST_IPC_LANE0_BIT	)	/**< Lane0 Illegal Point Comma interrupt enable	*/

#define	D_IM_PRO_SLVS_INTEN_FSI_BIT						(1<< 7)											/**< FrameStart is detected in the preprocessing of Line Buffer interrupt enable	*/
#define	D_IM_PRO_SLVS_INTEN_FSO_BIT						(1<< 6)											/**< FrameStart is detected in the postprocessing of Line Buffer interrupt enable	*/
#define	D_IM_PRO_SLVS_INTEN_FEI_BIT						(1<< 5)											/**< FrameEnd is detected in the preprocessing of Line Buffer interrupt enable	*/
#define	D_IM_PRO_SLVS_INTEN_FEO_BIT						(1<< 4)											/**< FrameEnd is detected in the postprocessing of Line Buffer interrupt enable	*/
#define	D_IM_PRO_SLVS_INTEN_RDY_BIT						(1<< 1)											/**< Training Sequence is ended interrupt enable	*/
#define	D_IM_PRO_SLVS_INTEN_STBY_BIT					(1<< 0)											/**< Standby Sequence is detected interrupt enable	*/

#define	D_IM_PRO_SLVS_INTST_FSI_BIT						(D_IM_PRO_SLVS_INTEN_FSI_BIT)					/**< FrameStart is detected in the preprocessing of Line Buffer interrupt flag	*/
#define	D_IM_PRO_SLVS_INTST_FSO_BIT						(D_IM_PRO_SLVS_INTEN_FSO_BIT)					/**< FrameStart is detected in the postprocessing of Line Buffer interrupt flag	*/
#define	D_IM_PRO_SLVS_INTST_FEI_BIT						(D_IM_PRO_SLVS_INTEN_FEI_BIT)					/**< FrameEnd is detected in the preprocessing of Line Buffer interrupt flag	*/
#define	D_IM_PRO_SLVS_INTST_FEO_BIT						(D_IM_PRO_SLVS_INTEN_FEO_BIT)					/**< FrameEnd is detected in the postprocessing of Line Buffer interrupt flag	*/
#define	D_IM_PRO_SLVS_INTST_RDY_BIT						(D_IM_PRO_SLVS_INTEN_RDY_BIT)					/**< Training Sequence is ended interrupt flag	*/
#define	D_IM_PRO_SLVS_INTST_STBY_BIT					(D_IM_PRO_SLVS_INTEN_STBY_BIT)					/**< Standby Sequence is detected interrupt flag	*/

#define	D_IM_PRO_SLVS_LEINTEN_LBOVF_BIT					(1<< 9)											/**< Line Buffer Over Flow Error interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_LNE_BIT					(1<< 8)											/**< Line Number Error interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_ECCE_BIT					(1<< 7)											/**< ECC UnCorrectable interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_ECC2C_BIT					(1<< 6)											/**< ECC 2Symbol Corrected interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_ECC1C_BIT					(1<< 5)											/**< ECC 1Symbol Corrected interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_HCRCE_BIT					(1<< 4)											/**< Header CRC UnCorrectable Error interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_HCRC2C_BIT				(1<< 3)											/**< Header CRC Error interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_HCRC1C_BIT				(1<< 2)											/**< Header CRC Error interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_PCRCE_BIT					(1<< 1)											/**< Payload CRC Error interrupt enable	*/
#define	D_IM_PRO_SLVS_LEINTEN_LLE_BIT					(1<< 0)											/**< Line Length Error interrupt enable	*/

#define	D_IM_PRO_SLVS_LEINTST_LBOVF_BIT					(D_IM_PRO_SLVS_LEINTEN_LBOVF_BIT)				/**< Line Buffer Over Flow Error interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_LNE_BIT					(D_IM_PRO_SLVS_LEINTEN_LNE_BIT)					/**< Line Number Error interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_ECCE_BIT					(D_IM_PRO_SLVS_LEINTEN_ECCE_BIT)				/**< ECC UnCorrectable interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_ECC2C_BIT					(D_IM_PRO_SLVS_LEINTEN_ECC2C_BIT)				/**< ECC 2Symbol Corrected interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_ECC1C_BIT					(D_IM_PRO_SLVS_LEINTEN_ECC1C_BIT)				/**< ECC 1Symbol Corrected interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_HCRCE_BIT					(D_IM_PRO_SLVS_LEINTEN_HCRCE_BIT)				/**< Header CRC UnCorrectable Error interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_HCRC2C_BIT				(D_IM_PRO_SLVS_LEINTEN_HCRC2C_BIT)				/**< Header CRC Error interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_HCRC1C_BIT				(D_IM_PRO_SLVS_LEINTEN_HCRC1C_BIT)				/**< Header CRC Error interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_PCRCE_BIT					(D_IM_PRO_SLVS_LEINTEN_PCRCE_BIT)				/**< Payload CRC Error interrupt flag	*/
#define	D_IM_PRO_SLVS_LEINTST_LLE_BIT					(D_IM_PRO_SLVS_LEINTEN_LLE_BIT)					/**< Line Length Error interrupt flag	*/

#define	D_IM_PRO_SLVS_MEINTEN0_DSD_BIT					(1<<24)											/**< Dynamic Skew Detect interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN0_DCL_BIT					(1<<16)											/**< Deskew Code Lost interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN0_SCL_BIT					(1<< 8)											/**< Start Code Lost interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN0_ECL_BIT					(1<< 0)											/**< End Code Lost interrupt enable	*/

#define	D_IM_PRO_SLVS_MEINTST0_DSD_BIT					(D_IM_PRO_SLVS_MEINTEN0_DSD_BIT)				/**< Dynamic Skew Detect interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST0_DCL_BIT					(D_IM_PRO_SLVS_MEINTEN0_DCL_BIT)				/**< Deskew Code Lost interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST0_SCL_BIT					(D_IM_PRO_SLVS_MEINTEN0_SCL_BIT)				/**< Start Code Lost interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST0_ECL_BIT					(D_IM_PRO_SLVS_MEINTEN0_ECL_BIT)				/**< End Code Lost interrupt flag	*/

#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE7_BIT			(1<<15)											/**< Lane7 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE6_BIT			(1<<14)											/**< Lane6 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE5_BIT			(1<<13)											/**< Lane5 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE4_BIT			(1<<12)											/**< Lane4 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE3_BIT			(1<<11)											/**< Lane3 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE2_BIT			(1<<10)											/**< Lane2 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE1_BIT			(1<< 9)											/**< Lane1 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE0_BIT			(1<< 8)											/**< Lane0 FIFO Under Flow interrupt enable.	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE7_BIT			(1<< 7)											/**< Lane7 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE6_BIT			(1<< 6)											/**< Lane6 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE5_BIT			(1<< 5)											/**< Lane5 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE4_BIT			(1<< 4)											/**< Lane4 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE3_BIT			(1<< 3)											/**< Lane3 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE2_BIT			(1<< 2)											/**< Lane2 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE1_BIT			(1<< 1)											/**< Lane1 FIFO Over Flow interrupt enable	*/
#define	D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE0_BIT			(1<< 0)											/**< Lane0 FIFO Over Flow interrupt enable	*/

#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE7_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE7_BIT)			/**< Lane7 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE6_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE6_BIT)			/**< Lane6 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE5_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE5_BIT)			/**< Lane5 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE4_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE4_BIT)			/**< Lane4 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE3_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE3_BIT)			/**< Lane3 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE2_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE2_BIT)			/**< Lane2 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE1_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE1_BIT)			/**< Lane1 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FUNF_LANE0_BIT			(D_IM_PRO_SLVS_MEINTEN1_FUNF_LANE0_BIT)			/**< Lane0 FIFO Under Flow interrupt flag.	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE7_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE7_BIT)			/**< Lane7 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE6_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE6_BIT)			/**< Lane6 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE5_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE5_BIT)			/**< Lane5 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE4_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE4_BIT)			/**< Lane4 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE3_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE3_BIT)			/**< Lane3 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE2_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE2_BIT)			/**< Lane2 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE1_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE1_BIT)			/**< Lane1 FIFO Over Flow interrupt flag	*/
#define	D_IM_PRO_SLVS_MEINTST1_FOVF_LANE0_BIT			(D_IM_PRO_SLVS_MEINTEN1_FOVF_LANE0_BIT)			/**< Lane0 FIFO Over Flow interrupt flag	*/

#define	D_IM_PRO_SLVS_STREAM_A_INTST_FSI_WAITEND_FLG	(1<< 5)											/**< Stream A FrameStart is detected in the preprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_A_INTST_FSO_WAITEND_FLG	(1<< 4)											/**< Stream A FrameStart is detected in the postprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_A_INTST_FEI_WAITEND_FLG	(1<< 3)											/**< Stream A FrameEnd is detected in the preprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_A_INTST_FEO_WAITEND_FLG	(1<< 2)											/**< Stream A FrameEnd is detected in the postprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_A_INTST_RDY_WAITEND_FLG	(1<< 1)											/**< Stream A Training Sequence is ended interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_A_INTST_STBY_WAITEND_FLG	(1<< 0)											/**< Stream A Standby Sequence is detected interrupt wait flag	*/

#define	D_IM_PRO_SLVS_STREAM_B_INTST_FSI_WAITEND_FLG	(1<<13)											/**< Stream B FrameStart is detected in the preprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_B_INTST_FSO_WAITEND_FLG	(1<<12)											/**< Stream B FrameStart is detected in the postprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_B_INTST_FEI_WAITEND_FLG	(1<<11)											/**< Stream B FrameEnd is detected in the preprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_B_INTST_FEO_WAITEND_FLG	(1<<10)											/**< Stream B FrameEnd is detected in the postprocessing of Line Buffer interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_B_INTST_RDY_WAITEND_FLG	(1<< 9)											/**< Stream B Training Sequence is ended interrupt wait flag	*/
#define	D_IM_PRO_SLVS_STREAM_B_INTST_STBY_WAITEND_FLG	(1<< 8)											/**< Stream B Standby Sequence is detected interrupt wait flag	*/

#define	D_IM_PRO_SLVS_INTST_ALL_WAITEND_FLG				(D_IM_PRO_SLVS_STREAM_A_INTST_FSI_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_A_INTST_FSO_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_A_INTST_FEI_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_A_INTST_FEO_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_A_INTST_RDY_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_A_INTST_STBY_WAITEND_FLG | \
														 D_IM_PRO_SLVS_STREAM_B_INTST_FSI_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_B_INTST_FSO_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_B_INTST_FEI_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_B_INTST_FEO_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_B_INTST_RDY_WAITEND_FLG  | \
														 D_IM_PRO_SLVS_STREAM_B_INTST_STBY_WAITEND_FLG	)		/**< All wait flags of INTST	*/

#define	D_IM_PRO_LVDSINTENB_SOFE0						(1<<  0)										/**< LVDS SOF0 interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_EOFE0						(1<<  4)										/**< LVDS EOF0 interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_SOLE0						(1<<  8)										/**< LVDS SOL0 Interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_EOLE0						(1<< 12)										/**< LVDS EOL0 Interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_SOFE1						(1<<  0)										/**< LVDS SOF1 interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_EOFE1						(1<<  4)										/**< LVDS EOF1 interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_SOLE1						(1<<  8)										/**< LVDS SOL1 Interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_EOLE1						(1<< 12)										/**< LVDS EOL1 Interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_SOFE2						(1<<  0)										/**< LVDS SOF2 interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_EOFE2						(1<<  4)										/**< LVDS EOF2 interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_SOLE2						(1<<  8)										/**< LVDS SOL2 Interrupt enable	*/
#define	D_IM_PRO_LVDSINTENB_EOLE2						(1<< 12)										/**< LVDS EOL2 Interrupt enable	*/

#define	D_IM_PRO_LVDSINTFLG_SOFF0						(D_IM_PRO_LVDSINTENB_SOFE0)						/**< LVDS SOFF0 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_EOFF0						(D_IM_PRO_LVDSINTENB_EOFE0)						/**< LVDS EOFF0 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_SOLF0						(D_IM_PRO_LVDSINTENB_SOLE0)						/**< LVDS SOLF0 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_EOLF0						(D_IM_PRO_LVDSINTENB_EOLE0)						/**< LVDS EOLF0 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_SOFF1						(D_IM_PRO_LVDSINTENB_SOFE1)						/**< LVDS SOFF1 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_EOFF1						(D_IM_PRO_LVDSINTENB_EOFE1)						/**< LVDS EOFF1 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_SOLF1						(D_IM_PRO_LVDSINTENB_SOLE1)						/**< LVDS SOLF1 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_EOLF1						(D_IM_PRO_LVDSINTENB_EOLE1)						/**< LVDS EOLF1 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_SOFF2						(D_IM_PRO_LVDSINTENB_SOFE2)						/**< LVDS SOFF2 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_EOFF2						(D_IM_PRO_LVDSINTENB_EOFE2)						/**< LVDS EOFF2 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_SOLF2						(D_IM_PRO_LVDSINTENB_SOLE2)						/**< LVDS SOLF2 Interrupt flag	*/
#define	D_IM_PRO_LVDSINTFLG_EOLF2						(D_IM_PRO_LVDSINTENB_EOLE2)						/**< LVDS EOLF2 Interrupt flag	*/

#define	D_IM_PRO_MPIINTE_DEC3							(1UL<<  0)										/**< MIPI-DPHY DECE3 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEC2							(1UL<<  1)										/**< MIPI-DPHY DECE2 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEC1							(1UL<<  2)										/**< MIPI-DPHY DECE1 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEC0							(1UL<<  3)										/**< MIPI-DPHY DECE0 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEE3							(1UL<<  4)										/**< MIPI-DPHY DEEE3 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEE2							(1UL<<  5)										/**< MIPI-DPHY DEEE2 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEE1							(1UL<<  6)										/**< MIPI-DPHY DEEE1 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DEE0							(1UL<<  7)										/**< MIPI-DPHY DEEE0 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESSH3							(1UL<<  8)										/**< MIPI-DPHY DESSHE3 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESSH2							(1UL<<  9)										/**< MIPI-DPHY DESSHE2 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESSH1							(1UL<< 10)										/**< MIPI-DPHY DESSHE1 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESSH0							(1UL<< 11)										/**< MIPI-DPHY DESSHE0 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESHE3							(1UL<< 12)										/**< MIPI-DPHY DESHE3 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESHE2							(1UL<< 13)										/**< MIPI-DPHY DESHE2 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESHE1							(1UL<< 14)										/**< MIPI-DPHY DESHE1 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_DESHE0							(1UL<< 15)										/**< MIPI-DPHY DESHE0 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FEO1							(1UL<< 16)										/**< MIPI-DPHY FEO1E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FEO0							(1UL<< 17)										/**< MIPI-DPHY FEO0E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FEI1							(1UL<< 18)										/**< MIPI-DPHY FEI1E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FEI0							(1UL<< 19)										/**< MIPI-DPHY FEI0E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FSO1							(1UL<< 20)										/**< MIPI-DPHY FSO1E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FSO0							(1UL<< 21)										/**< MIPI-DPHY FSO0E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FSI1							(1UL<< 22)										/**< MIPI-DPHY FSI1E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_FSI0							(1UL<< 23)										/**< MIPI-DPHY FSI0E interrupt enable	*/
#define	D_IM_PRO_MPIINTE_LZE3							(1UL<< 24)										/**< MIPI-DPHY LZEE3 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_LZE2							(1UL<< 25)										/**< MIPI-DPHY LZEE2 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_LZE1							(1UL<< 26)										/**< MIPI-DPHY LZEE1 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_LZE0							(1UL<< 27)										/**< MIPI-DPHY LZEE0 interrupt enable	*/
#define	D_IM_PRO_MPIINTE_EFS							(1UL<< 28)										/**< MIPI-DPHY EFSE interrupt enable	*/
#define	D_IM_PRO_MPIINTE_CRCE							(1UL<< 29)										/**< MIPI-DPHY CRCEE interrupt enable	*/
#define	D_IM_PRO_MPIINTE_ECCW							(1UL<< 30)										/**< MIPI-DPHY ECCWE interrupt enable	*/
#define	D_IM_PRO_MPIINTE_ECCE							(1UL<< 31)										/**< MIPI-DPHY ECCEE interrupt enable	*/

#define	D_IM_PRO_MPIINTF_DEC3							(D_IM_PRO_MPIINTE_DEC3)							/**< MIPI-DPHY DECE3 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEC2							(D_IM_PRO_MPIINTE_DEC2)							/**< MIPI-DPHY DECE2 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEC1							(D_IM_PRO_MPIINTE_DEC1)							/**< MIPI-DPHY DECE1 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEC0							(D_IM_PRO_MPIINTE_DEC0)							/**< MIPI-DPHY DECE0 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEE3							(D_IM_PRO_MPIINTE_DEE3)							/**< MIPI-DPHY DEEE3 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEE2							(D_IM_PRO_MPIINTE_DEE2)							/**< MIPI-DPHY DEEE2 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEE1							(D_IM_PRO_MPIINTE_DEE1)							/**< MIPI-DPHY DEEE1 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DEE0							(D_IM_PRO_MPIINTE_DEE0)							/**< MIPI-DPHY DEEE0 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_DESSH3							(D_IM_PRO_MPIINTE_DESSH3)						/**< MIPI-DPHY DESSHE3 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESSH2							(D_IM_PRO_MPIINTE_DESSH2)						/**< MIPI-DPHY DESSHE2 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESSH1							(D_IM_PRO_MPIINTE_DESSH1)						/**< MIPI-DPHY DESSHE1 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESSH0							(D_IM_PRO_MPIINTE_DESSH0)						/**< MIPI-DPHY DESSHE0 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESH3							(D_IM_PRO_MPIINTE_DESHE3)						/**< MIPI-DPHY DESHE3 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESH2							(D_IM_PRO_MPIINTE_DESHE2)						/**< MIPI-DPHY DESHE2 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESH1							(D_IM_PRO_MPIINTE_DESHE1)						/**< MIPI-DPHY DESHE1 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_DESH0							(D_IM_PRO_MPIINTE_DESHE0)						/**< MIPI-DPHY DESHE0 interrupt flag	*/
#define	D_IM_PRO_MPIINTF_FEO1							(D_IM_PRO_MPIINTE_FEO1)							/**< MIPI-DPHY FEO1E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FEO0							(D_IM_PRO_MPIINTE_FEO0)							/**< MIPI-DPHY FEO0E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FEI1							(D_IM_PRO_MPIINTE_FEI1)							/**< MIPI-DPHY FEI1E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FEI0							(D_IM_PRO_MPIINTE_FEI0)							/**< MIPI-DPHY FEI0E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FSO1							(D_IM_PRO_MPIINTE_FSO1)							/**< MIPI-DPHY FSO1E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FSO0							(D_IM_PRO_MPIINTE_FSO0)							/**< MIPI-DPHY FSO0E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FSI1							(D_IM_PRO_MPIINTE_FSI1)							/**< MIPI-DPHY FSI1E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_FSI0							(D_IM_PRO_MPIINTE_FSI0)							/**< MIPI-DPHY FSI0E interrupt flag		*/
#define	D_IM_PRO_MPIINTF_LZE3							(D_IM_PRO_MPIINTE_LZE3)							/**< MIPI-DPHY LZEE3 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_LZE2							(D_IM_PRO_MPIINTE_LZE2)							/**< MIPI-DPHY LZEE2 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_LZE1							(D_IM_PRO_MPIINTE_LZE1)							/**< MIPI-DPHY LZEE1 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_LZE0							(D_IM_PRO_MPIINTE_LZE0)							/**< MIPI-DPHY LZEE0 interrupt flag		*/
#define	D_IM_PRO_MPIINTF_EFS							(D_IM_PRO_MPIINTE_EFS)							/**< MIPI-DPHY EFSE interrupt flag		*/
#define	D_IM_PRO_MPIINTF_CRCE							(D_IM_PRO_MPIINTE_CRCE)							/**< MIPI-DPHY CRCEE interrupt flag		*/
#define	D_IM_PRO_MPIINTF_ECCW							(D_IM_PRO_MPIINTE_ECCW)							/**< MIPI-DPHY ECCWE interrupt flag		*/
#define	D_IM_PRO_MPIINTF_ECCE							(D_IM_PRO_MPIINTE_ECCE)							/**< MIPI-DPHY ECCEE interrupt flag		*/

#define	D_IM_PRO_LDIVINTENB_LDIVEE						(1<<  0)										/**< LDIV LDIVEE interrupt enable			*/
#define	D_IM_PRO_LDIVINTFLG_LDIVEF						(D_IM_PRO_LDIVINTENB_LDIVEE)					/**< LDIV LDIVEF interrupt flag				*/

#define	D_IM_PRO_OBDINTENB_OBDE							(1<<  0)										/**< OBD OBDE interrupt enable				*/
#define	D_IM_PRO_OBDINTFLG_OBDF							(D_IM_PRO_OBDINTENB_OBDE)						/**< OBD OBDF interrupt flag				*/

#define	D_IM_PRO_OBTINTENB_OBTE							(1<<  4)										/**< OBD OBTE interrupt enable				*/
#define	D_IM_PRO_OBTINTFLG_OBTF							(D_IM_PRO_OBTINTENB_OBTE)						/**< OBD OBDF interrupt flag				*/

/*@}*/	// im_pro_handler_sen
#define	D_IM_PRO_MPICINTE1_CEC2							(1UL<<  1)										/**< MIPI-CPHY CECE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CEC1							(1UL<<  2)										/**< MIPI-CPHY CECE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CEC0							(1UL<<  3)										/**< MIPI-CPHY CECE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CEE2							(1UL<<  5)										/**< MIPI-CPHY CEEE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CEE1							(1UL<<  6)										/**< MIPI-CPHY CEEE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CEE0							(1UL<<  7)										/**< MIPI-CPHY CEEE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CESSH2						(1UL<<  9)										/**< MIPI-CPHY CESSHE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CESSH1						(1UL<< 10)										/**< MIPI-CPHY CESSHE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CESSH0						(1UL<< 11)										/**< MIPI-CPHY CESSHE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CESH2						(1UL<< 13)										/**< MIPI-CPHY CESHE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CESH1						(1UL<< 14)										/**< MIPI-CPHY CESHE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CESH0						(1UL<< 15)										/**< MIPI-CPHY CESHE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FEO1							(1UL<< 16)										/**< MIPI-CPHY FEO1E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FEO0							(1UL<< 17)										/**< MIPI-CPHY FEO0E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FEI1							(1UL<< 18)										/**< MIPI-CPHY FEI1E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FEI0							(1UL<< 19)										/**< MIPI-CPHY FEI0E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FSO1							(1UL<< 20)										/**< MIPI-CPHY FSO1E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FSO0							(1UL<< 21)										/**< MIPI-CPHY FSO0E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FSI1							(1UL<< 22)										/**< MIPI-CPHY FSI1E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_FSI0							(1UL<< 23)										/**< MIPI-CPHY FSI0E interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_LZE2							(1UL<< 25)										/**< MIPI-CPHY LZEE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_LZE1							(1UL<< 26)										/**< MIPI-CPHY LZEE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_LZE0							(1UL<< 27)										/**< MIPI-CPHY LZEE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_EFS							(1UL<< 28)										/**< MIPI-CPHY EFSE interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_CRCDTE						(1UL<< 29)										/**< MIPI-CPHY CRCDTEE interrupt enable		*/
#define	D_IM_PRO_MPICINTE1_PHCRC2E						(1UL<< 30)										/**< MIPI-CPHY PHCRC2EE interrupt enable	*/
#define	D_IM_PRO_MPICINTE1_PHCRC1E						(1UL<< 31)										/**< MIPI-CPHY PHCRC1EE interrupt enable	*/

#define	D_IM_PRO_MPICINTF1_CECF2						(D_IM_PRO_MPICINTE1_CEC2)						/**< MIPI-CPHY CECE2 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CECF1						(D_IM_PRO_MPICINTE1_CEC1)						/**< MIPI-CPHY CECE1 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CECF0						(D_IM_PRO_MPICINTE1_CEC0)						/**< MIPI-CPHY CECE0 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CEEF2						(D_IM_PRO_MPICINTE1_CEE2)						/**< MIPI-CPHY CEEE2 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CEEF1						(D_IM_PRO_MPICINTE1_CEE1)						/**< MIPI-CPHY CEEE1 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CEEF0						(D_IM_PRO_MPICINTE1_CEE0)						/**< MIPI-CPHY CEEE0 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CESSHF2						(D_IM_PRO_MPICINTE1_CESSH2)						/**< MIPI-CPHY CESSHE2 interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_CESSHF1						(D_IM_PRO_MPICINTE1_CESSH1)						/**< MIPI-CPHY CESSHE1 interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_CESSHF0						(D_IM_PRO_MPICINTE1_CESSH0)						/**< MIPI-CPHY CESSHE0 interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_CESHF2						(D_IM_PRO_MPICINTE1_CESH2)						/**< MIPI-CPHY CESHE2 interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_CESHF1						(D_IM_PRO_MPICINTE1_CESH1)						/**< MIPI-CPHY CESHE1 interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_CESHF0						(D_IM_PRO_MPICINTE1_CESH0)						/**< MIPI-CPHY CESHE0 interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_FEO1F						(D_IM_PRO_MPICINTE1_FEO1)						/**< MIPI-CPHY FEO1E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FEO0F						(D_IM_PRO_MPICINTE1_FEO0)						/**< MIPI-CPHY FEO0E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FEI1F						(D_IM_PRO_MPICINTE1_FEI1)						/**< MIPI-CPHY FEI1E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FEI0F						(D_IM_PRO_MPICINTE1_FEI0)						/**< MIPI-CPHY FEI0E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FSO1F						(D_IM_PRO_MPICINTE1_FSO1)						/**< MIPI-CPHY FSO1E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FSO0F						(D_IM_PRO_MPICINTE1_FSO0)						/**< MIPI-CPHY FSO0E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FSI1F						(D_IM_PRO_MPICINTE1_FSI1)						/**< MIPI-CPHY FSI1E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_FSI0F						(D_IM_PRO_MPICINTE1_FSI0)						/**< MIPI-CPHY FSI0E interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_LZEF2						(D_IM_PRO_MPICINTE1_LZE2)						/**< MIPI-CPHY LZEE2 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_LZEF1						(D_IM_PRO_MPICINTE1_LZE1)						/**< MIPI-CPHY LZEE1 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_LZEF0						(D_IM_PRO_MPICINTE1_LZE0)						/**< MIPI-CPHY LZEE0 interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_EFSF							(D_IM_PRO_MPICINTE1_EFS)						/**< MIPI-CPHY EFSE interrupt flag			*/
#define	D_IM_PRO_MPICINTF1_CRCDTEF						(D_IM_PRO_MPICINTE1_CRCDTE)						/**< MIPI-CPHY CRCDTEE interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_PHCRC2EF						(D_IM_PRO_MPICINTE1_PHCRC2E)					/**< MIPI-CPHY PHCRC2EE interrupt flag		*/
#define	D_IM_PRO_MPICINTF1_PHCRC1EF						(D_IM_PRO_MPICINTE1_PHCRC1E)					/**< MIPI-CPHY PHCRC1EE interrupt flag		*/

#define	D_IM_PRO_MPICINTE2_CICHSE2						(1<<  1)										/**< MIPI-CPHY CICHSE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTE2_CICHSE1						(1<<  2)										/**< MIPI-CPHY CICHSE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTE2_CICHSE0						(1<<  3)										/**< MIPI-CPHY CICHSE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTE2_PHCRCAEE						(1<<  4)										/**< MIPI-CPHY PHCRCAEE interrupt enable	*/

#define	D_IM_PRO_MPICINTF2_CICHSF2						(D_IM_PRO_MPICINTE2_CICHSE2)					/**< MIPI-CPHY CICHSE2 interrupt enable		*/
#define	D_IM_PRO_MPICINTF2_CICHSF1						(D_IM_PRO_MPICINTE2_CICHSE1)					/**< MIPI-CPHY CICHSE1 interrupt enable		*/
#define	D_IM_PRO_MPICINTF2_CICHSF0						(D_IM_PRO_MPICINTE2_CICHSE0)					/**< MIPI-CPHY CICHSE0 interrupt enable		*/
#define	D_IM_PRO_MPICINTF2_PHCRCAEF						(D_IM_PRO_MPICINTE2_PHCRCAEE)					/**< MIPI-CPHY PHCRCAEE interrupt enable	*/


/** @addtogroup im_pro_handler_sro SRO Interrupt
@{
*/

#define	D_IM_PRO_SDCINTENB_SDCE							(1<<  0)										/**< SDCINTENB SDCE interrupt enable		*/
#define	D_IM_PRO_SDCINTFLG_SDCF							(D_IM_PRO_SDCINTENB_SDCE)						/**< SDCINTFLG SDCF interrupt flag			*/

/*	@}*/	// im_pro_handler_sro

/** @addtogroup im_pro_handler_common	Common Interrupt
@{
*/
#define	D_IM_PRO_MONIINTENB_MONIHDE						(1<<  0)										/**< MONI MONIHDE interrupt enable			*/
#define	D_IM_PRO_MONIINTENB_MONIEPFE					(1<<  4)										/**< MONI MONIEPFE interrupt enable			*/
#define	D_IM_PRO_MONIINTENB_MONIEPLE					(1<<  8)										/**< MONI MONIEPLE interrupt enable			*/
#define	D_IM_PRO_MONIINTENB_MONIBLLMINE					(1<< 12)										/**< MONI MONIBLLMINE interrupt enable		*/
#define	D_IM_PRO_MONIINTENB_MONIBLLMAXE					(1<< 16)										/**< MONI MONIBLLMAXE interrupt enable		*/

#define	D_IM_PRO_MONIINTFLG_MONIHDF						(D_IM_PRO_MONIINTENB_MONIHDE)					/**< MONI MONIHDF interrupt flag			*/
#define	D_IM_PRO_MONIINTFLG_MONIEPFF					(D_IM_PRO_MONIINTENB_MONIEPFE)					/**< MONI MONIEPFF interrupt flag			*/
#define	D_IM_PRO_MONIINTFLG_MONIEPLF					(D_IM_PRO_MONIINTENB_MONIEPLE)					/**< MONI MONIEPLF interrupt flag			*/
#define	D_IM_PRO_MONIINTFLG_MONIBLLMINF					(D_IM_PRO_MONIINTENB_MONIBLLMINE)				/**< MONI MONIBLLMINF interrupt flag		*/
#define	D_IM_PRO_MONIINTFLG_MONIBLLMAXF					(D_IM_PRO_MONIINTENB_MONIBLLMAXE)				/**< MONI MONIBLLMAXF interrupt flag		*/

#define	D_IM_PRO_PWCHINTENB_PWE							(1<<  0)										/**< PWCH PWE interrupt enable				*/
#define	D_IM_PRO_PWCHINTENB_PWEE						(1<<  4)										/**< PWCH PWEE interrupt enable				*/
#define	D_IM_PRO_PWCHINTENB_PWXE						(1<<  8)										/**< PWCH PWXE interrupt enable				*/
#define	D_IM_PRO_PWCHINTFLG_PWF							(D_IM_PRO_PWCHINTENB_PWE)						/**< PWCH PWF interrupt flag				*/
#define	D_IM_PRO_PWCHINTFLG_PWEF						(D_IM_PRO_PWCHINTENB_PWEE)						/**< PWCH PWEF interrupt flag				*/
#define	D_IM_PRO_PWCHINTFLG_PWXF						(D_IM_PRO_PWCHINTENB_PWXE)						/**< PWCH PWXF interrupt flag				*/

#define	D_IM_PRO_PRCHINTENB_PRE							(1<<  0)										/**< PRCH PRE interrupt enable				*/
#define	D_IM_PRO_PRCHINTENB_PREE						(1<<  4)										/**< PRCH PREE interrupt enable				*/
#define	D_IM_PRO_PRCHINTENB_PRXE						(1<<  8)										/**< PRCH PRXE interrupt enable				*/
#define	D_IM_PRO_PRCHINTFLG_PRF							(D_IM_PRO_PRCHINTENB_PRE)						/**< PRCH PRF interrupt flag				*/
#define	D_IM_PRO_PRCHINTFLG_PREF						(D_IM_PRO_PRCHINTENB_PREE)						/**< PRCH PREF interrupt flag				*/
#define	D_IM_PRO_PRCHINTFLG_PRXF						(D_IM_PRO_PRCHINTENB_PRXE)						/**< PRCH PRXF interrupt flag				*/

#define	D_IM_PRO_VMIVADR_MIN							(0)		/**< Minimum setting of VMIVADR	*/
#define	D_IM_PRO_VMIVADR_MAX							(16383)	/**< Maximum setting of VMIVADR	*/

#define	D_IM_PRO_VMIHADR_MIN							(0)		/**< Minimum setting of VMIHADR	*/
#define	D_IM_PRO_VMIHADR_MAX							(12288)	/**< Maximum setting of VMIHADR	*/

#define	D_IM_PRO_HMIHADR_MIN							(0)		/**< Minimum setting of HMIHADR	*/
#define	D_IM_PRO_HMIHADR_MAX							(12288)	/**< Maximum setting of HMIHADR	*/

/*@}*/	// im_pro_handler_common

/** @addtogroup im_pro_handler_stat	STAT Interrupt
@{
*/

#define	D_IM_PRO_LINEINTENB_LINEE						(1<<  0)										/**< AEAWB LINEINTENB interrupt enable		*/
#define	D_IM_PRO_LINEINTFLG_LINEF						(D_IM_PRO_LINEINTENB_LINEE)						/**< AEAWB LINEINTFLG interrupt flag		*/

#define	D_IM_PRO_AFINTENB_AFE							(1<<  0)										/**< AF AFINTENB interrupt enable			*/
#define	D_IM_PRO_AFINTFLG_AFF							(D_IM_PRO_AFINTENB_AFE)							/**< AF AFINTFLG interrupt flag				*/

#define	D_IM_PRO_HISTINTENB_HISTE						(1<<  0)										/**< HIST HISTINTENB interrupt enable		*/
#define	D_IM_PRO_HISTINTFLG_HISTF						(D_IM_PRO_HISTINTENB_HISTE)						/**< HIST HISTINTFLG interrupt flag			*/

/*@}*/	// im_pro_handler_stat


/** @addtogroup im_pro_sen_top SENTOP
@{
*/
#define	D_IM_PRO_SEN_SENCORE_NUM		(4)									/**< Number of SENCORE		*/
#define	D_IM_PRO_SEN_BS_NUM				(4)									/**< Number of BS			*/
#define	D_IM_PRO_SENTOP_BLOB_NUM		(4)									/**< Number of BLOB			*/
#define	D_IM_PRO_SENTOP_PWSW_NUM		(4)									/**< Number of PWSW			*/
#define	D_IM_PRO_SENTOP_CPHY_NUM		(4)									/**< Number of ComboPHY		*/
#define	D_IM_PRO_SEN_SG_NUM				(4)									/**< Number of SG			*/
#define	D_IM_PRO_SEN_LDIV_NUM			(4)									/**< Number of LDIV			*/
#define	D_IM_PRO_SEN_OBD_CH_NUM			(2)									/**< Number of OBD			*/
#define	D_IM_PRO_SEN_OBT_CH_NUM			(2)									/**< Number of OBT			*/
#define D_IM_PRO_SEN_OBT_CNT			(E_IM_PRO_OBT_CH_3_1 + 1)			/**< Total Number of OBD	*/
#define D_IM_PRO_SEN_OBD_CNT			(E_IM_PRO_OBD_CH_3_1 + 1)			/**< Total Number of OBT	*/
                                                                            
#define D_IM_PRO_SEN_SLVS_CH_NUM		(E_IM_PRO_SLVS_STREAM_TYPE_B + 1)	/**< Number of SEN SLVS		*/
#define D_IM_PRO_SEN_LVDS_CH_NUM		(E_IM_PRO_LVDS_CH_3 + 1)			/**< Number of SEN LVDS		*/
#define D_IM_PRO_SEN_LDIV_CH_NUM		(E_IM_PRO_LDIV_CH_3 + 1)			/**< Number of SEN LDIV		*/
#define D_IM_PRO_SEN_DMIPI_CH_NUM		(E_IM_PRO_DMIPI_CH3 + 1)			/**< Number of SEN DMIPI	*/
#define D_IM_PRO_SEN_CMIPI_CH_NUM		(E_IM_PRO_CMIPI_CH3 + 1)			/**< Number of SEN CMIPI	*/
#define D_IM_PRO_SEN_MONI_CH_NUM		(E_IM_PRO_MONI_CH_3 + 1)			/**< Number of SEN MONI		*/
#define D_IM_PRO_SEN_LNR_CH_NUM			(E_IM_PRO_LNR_CH_3 + 1)				/**< Number of SEN LNR		*/
#define D_IM_PRO_SEN_PWCH_CH_NUM		(E_IM_PRO_PWCH_7 + 1)				/**< Number of SEN PWCH		*/
#define D_IM_PRO_SEN_P2M_CH_NUM			(E_IM_PRO_P2M_CH7 + 1)				/**< Number of SEN P2M		*/

#define D_IM_PRO_SRO_P2M_CH_NUM			(E_IM_PRO_P2M_CH5 + 1)				/**< Number of SRO P2M		*/
#define D_IM_PRO_SRO_PWCH_CH_NUM		(E_IM_PRO_PWCH_5 + 1)				/**< Number of SRO PWCH		*/
#define D_IM_PRO_SRO_M2P_CH_NUM			(E_IM_PRO_M2P_MAX)					/**< Number of SRO M2P		*/
#define D_IM_PRO_SRO_PRCH_CH_NUM		(E_IM_PRO_PRCH_MAX)					/**< Number of SRO PRCH		*/
#define D_IM_PRO_SRO_SDC_CH_NUM			(E_IM_PRO_SDC_CH1 + 1)				/**< Number of SRO SDC		*/
#define D_IM_PRO_SRO_AFPB_CH_NUM		(E_IM_PRO_AFPB_CH_G + 1)			/**< Number of SRO AFPB		*/
#define D_IM_PRO_SRO_FSHDL_CH_NUM		(E_IM_PRO_FSHDL_CH_1 + 1)			/**< Number of SRO FSHDL	*/
#define D_IM_PRO_SRO_FFSH_CH_NUM		(E_IM_PRO_FFSH_CH1 + 1)				/**< Number of SRO FFSH		*/
#define D_IM_PRO_SRO_FF_CH_NUM			(E_IM_PRO_FF_CH1 + 1)				/**< Number of SRO FF		*/
#define D_IM_PRO_SRO_PZSFT_CH_NUM		(4)									/**< Number of SRO PZSFT	*/

#define D_IM_PRO_B2B_P2M_CH_NUM			(E_IM_PRO_P2M_CH1 + 1)				/**< Number of B2B P2M		*/
#define D_IM_PRO_B2B_PWCH_CH_NUM		(E_IM_PRO_PWCH_1 + 1)				/**< Number of B2B PWCH		*/
#define D_IM_PRO_B2B_M2P_CH_NUM			(E_IM_PRO_M2P_3 + 1)				/**< Number of B2B M2P		*/
#define D_IM_PRO_B2B_PRCH_CH_NUM		(E_IM_PRO_PRCH_4 + 1)				/**< Number of B2B PRCH		*/
#define D_IM_PRO_B2B_PZSFT_CH_NUM		(3)									/**< Number of B2B PZSFT	*/
#define D_IM_PRO_B2B_PGAIN_CH_NUM		(2)									/**< Number of B2B PGAIN	*/


#define D_IM_PRO_STAT_PWCH_CH_NUM		(E_IM_PRO_PWCH_5 + 1)				/**< Number of STAT PWCH	*/

#define	D_IM_PRO_SENTOP_VHD_CH_NUM	(2)		/**< Number of VD/HD ch	*/

#define	D_IM_PRO_SENTOP_LVDS_CLK_PHY0		(0x00000001)	/**< LVDS PHY0 clock lane	*/
#define	D_IM_PRO_SENTOP_LVDS_CLK_PHY1		(0x00000002)	/**< LVDS PHY1 clock lane	*/
#define	D_IM_PRO_SENTOP_LVDS_CLK_PHY2		(0x00000004)	/**< LVDS PHY2 clock lane	*/
#define	D_IM_PRO_SENTOP_LVDS_CLK_PHY3		(0x00000008)	/**< LVDS PHY3 clock lane	*/

#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY0_0	(0x00010000)	/**< LVDS PHY0 data lane0	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY0_1	(0x00020000)	/**< LVDS PHY0 data lane1	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY0_2	(0x00040000)	/**< LVDS PHY0 data lane2	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY0_3	(0x00080000)	/**< LVDS PHY0 data lane3	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY1_0	(0x00100000)	/**< LVDS PHY1 data lane0	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY1_1	(0x00200000)	/**< LVDS PHY1 data lane1	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY1_2	(0x00400000)	/**< LVDS PHY1 data lane2	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY1_3	(0x00800000)	/**< LVDS PHY1 data lane3	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY2_0	(0x01000000)	/**< LVDS PHY2 data lane0	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY2_1	(0x02000000)	/**< LVDS PHY2 data lane1	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY2_2	(0x04000000)	/**< LVDS PHY2 data lane2	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY2_3	(0x08000000)	/**< LVDS PHY2 data lane3	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY3_0	(0x10000000)	/**< LVDS PHY3 data lane0	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY3_1	(0x20000000)	/**< LVDS PHY3 data lane1	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY3_2	(0x40000000)	/**< LVDS PHY3 data lane2	*/
#define	D_IM_PRO_SENTOP_LVDS_DATA_PHY3_3	(0x80000000)	/**< LVDS PHY3 data lane3	*/
                                                                     
/*	@}*/	// im_pro_sen_top

/** @addtogroup im_pro_sro_top SROTOP
@{
*/
#define	D_IM_PRO_SROTOP_VHD_CH_NUM	(2)		/**< Number of VD/HD ch	*/
#define	D_IM_PRO_SROTOP_AFPB_CH_NUM	(2)		/**< Number of AFPB ch	*/

#define	D_IM_PRO_SROTOP_INT_VDE0						(1<< 0)											/**< SROTOP VD0 interrupt enable	*/
#define	D_IM_PRO_SROTOP_INT_VDE1						(1<< 4)											/**< SROTOP VD1 interrupt enable	*/
#define	D_IM_PRO_SROTOP_INT_HDE0						(1<< 8)											/**< SROTOP HD0 interrupt enable	*/
#define	D_IM_PRO_SROTOP_INT_HDE1						(1<<12)											/**< SROTOP HD1 interrupt enable	*/
#define	D_IM_PRO_SROTOP_INT_VDF0						(D_IM_PRO_SROTOP_INT_VDE0)						/**< SROTOP VD0 interrupt flag	*/
#define	D_IM_PRO_SROTOP_INT_VDF1						(D_IM_PRO_SROTOP_INT_VDE1)						/**< SROTOP VD1 interrupt flag	*/
#define	D_IM_PRO_SROTOP_INT_HDF0						(D_IM_PRO_SROTOP_INT_HDE0)						/**< SROTOP HD0 interrupt flag	*/
#define	D_IM_PRO_SROTOP_INT_HDF1						(D_IM_PRO_SROTOP_INT_HDE1)						/**< SROTOP HD1 interrupt flag	*/

#define	D_IM_PRO_SROTOP_INT_VD_ALL						(D_IM_PRO_SROTOP_INT_VDF0 | D_IM_PRO_SROTOP_INT_VDF1)	/**< All VD interrupt flag	*/
#define	D_IM_PRO_SROTOP_INT_HD_ALL						(D_IM_PRO_SROTOP_INT_HDF0 | D_IM_PRO_SROTOP_INT_HDF1)	/**< All HD interrupt flag	*/

#define	D_IM_PRO_SROTOP_GENV_MIN			(2)			/**< Minimum setting of GENV	*/
#define	D_IM_PRO_SROTOP_GENV_MAX			(16382)		/**< Maximum setting of GENV	*/
#define	D_IM_PRO_SROTOP_GENH_MIN			(2)			/**< Minimum setting of GENH	*/
#define	D_IM_PRO_SROTOP_GENH_MAX			(16382)		/**< Maximum setting of GENH	*/

/*	@}*/	// im_pro_sro_top

/** @addtogroup im_pro_b2b_top B2BTOP
@{
*/
#define	D_IM_PRO_B2BTOP_VHD_CH_NUM	(2)		/**< Number of VD/HD ch	*/

#define	D_IM_PRO_B2BTOP_INT_VDE0						(1<< 0)											/**< B2BTOP VD0 interrupt enable	*/
#define	D_IM_PRO_B2BTOP_INT_VDE1						(1<< 4)											/**< B2BTOP VD1 interrupt enable	*/
#define	D_IM_PRO_B2BTOP_INT_HDE0						(1<< 8)											/**< B2BTOP HD0 interrupt enable	*/
#define	D_IM_PRO_B2BTOP_INT_HDE1						(1<<12)											/**< B2BTOP HD1 interrupt enable	*/
#define	D_IM_PRO_B2BTOP_INT_VDF0						(D_IM_PRO_B2BTOP_INT_VDE0)						/**< B2BTOP VD0 interrupt flag	*/
#define	D_IM_PRO_B2BTOP_INT_VDF1						(D_IM_PRO_B2BTOP_INT_VDE1)						/**< B2BTOP VD1 interrupt flag	*/
#define	D_IM_PRO_B2BTOP_INT_HDF0						(D_IM_PRO_B2BTOP_INT_HDE0)						/**< B2BTOP HD0 interrupt flag	*/
#define	D_IM_PRO_B2BTOP_INT_HDF1						(D_IM_PRO_B2BTOP_INT_HDE1)						/**< B2BTOP HD1 interrupt flag	*/

#define	D_IM_PRO_B2BTOP_INT_VD_ALL						(D_IM_PRO_B2BTOP_INT_VDF0 | D_IM_PRO_B2BTOP_INT_VDF1)	/**< All VD interrupt flag	*/
#define	D_IM_PRO_B2BTOP_INT_HD_ALL						(D_IM_PRO_B2BTOP_INT_HDF0 | D_IM_PRO_B2BTOP_INT_HDF1)	/**< All HD interrupt flag	*/
/*	@}*/	// im_pro_b2b_top

/** @addtogroup im_pro_stat_top STATTOP
@{
*/
#define	D_IM_PRO_STAT_AF_CH_NUM		(2)		/**< Number of VD/HD ch	*/
#define	D_IM_PRO_STAT_AEAWB_CH_NUM	(4)		/**< Number of VD/HD ch	*/
#define	D_IM_PRO_STAT_HIST_CH_NUM	(4)		/**< Number of VD/HD ch	*/
#define	D_IM_PRO_STAT_FLCK_CH_NUM	(2)		/**< Number of VD/HD ch	*/
/*	@}*/	// im_pro_stat_top

/** @addtogroup im_pro_pas_top PASTOP
@{
*/
#define	D_IM_PRO_PASTOP_VHD_CH_NUM	(2)		/**< Number of VD/HD ch	*/

#define	D_IM_PRO_PASTOP_INT_VDE0						(1<< 0)											/**< PASTOP VD0 interrupt enable	*/
#define	D_IM_PRO_PASTOP_INT_VDE1						(1<< 4)											/**< PASTOP VD1 interrupt enable	*/
#define	D_IM_PRO_PASTOP_INT_HDE0						(1<< 8)											/**< PASTOP HD0 interrupt enable	*/
#define	D_IM_PRO_PASTOP_INT_HDE1						(1<<12)											/**< PASTOP HD1 interrupt enable	*/
#define	D_IM_PRO_PASTOP_INT_VDF0						(D_IM_PRO_PASTOP_INT_VDE0)						/**< PASTOP VD0 interrupt flag	*/
#define	D_IM_PRO_PASTOP_INT_VDF1						(D_IM_PRO_PASTOP_INT_VDE1)						/**< PASTOP VD1 interrupt flag	*/
#define	D_IM_PRO_PASTOP_INT_HDF0						(D_IM_PRO_PASTOP_INT_HDE0)						/**< PASTOP HD0 interrupt flag	*/
#define	D_IM_PRO_PASTOP_INT_HDF1						(D_IM_PRO_PASTOP_INT_HDE1)						/**< PASTOP HD1 interrupt flag	*/

#define	D_IM_PRO_PASTOP_INT_VD_ALL						(D_IM_PRO_PASTOP_INT_VDF0 | D_IM_PRO_PASTOP_INT_VDF1)	/**< All VD interrupt flag	*/
#define	D_IM_PRO_PASTOP_INT_HD_ALL						(D_IM_PRO_PASTOP_INT_HDF0 | D_IM_PRO_PASTOP_INT_HDF1)	/**< All HD interrupt flag	*/
/*	@}*/	// im_pro_pas_top

/** @addtogroup im_pro_common_pwch PWCH
@{
*/

#define	D_IM_PRO_PWCH_BUFFER_MIN	(1)			/**< PWch Store Buffer Address Table Min index	*/
#define	D_IM_PRO_PWCH_BUFFER_MAX	(8)			/**< PWch Store Buffer Address Table Max index	*/

#define	D_IM_PRO_PWCH_AWCACHE_MAX	(0xF)		/**< Maximum setting of AWCACHE	*/
#define	D_IM_PRO_PWCH_AWPROT_MAX	(0x7)		/**< Maximum setting of AWPROT	*/

/*	@}*/	// im_pro_common_pwch

/** @addtogroup im_pro_stat_aeawb AEAWB
@{
*/

#define	D_IM_PRO_AEAWB_AEHBC_MIN			(1)		/**< Minimum setting of AEHBC	*/
#define	D_IM_PRO_AEAWB_AEHBC_MAX			(64)	/**< Maximum setting of AEHBC	*/
#define	D_IM_PRO_AEAWB_AEVBC_MIN			(1)		/**< Minimum setting of AEVBC	*/
#define	D_IM_PRO_AEAWB_AEVBC_MAX			(64)	/**< Maximum setting of AEVBC	*/
#define	D_IM_PRO_AEAWB_AEV_MIN				(0)		/**< Minimum setting of AEV	*/
#define	D_IM_PRO_AEAWB_AEV_MAX				(16382)	/**< Maximum setting of AEV	*/
#define	D_IM_PRO_AEAWB_AEH_MIN				(0)		/**< Minimum setting of AEH	*/
#define	D_IM_PRO_AEAWB_AEH_MAX				(8190)	/**< Maximum setting of AEH	*/
#define	D_IM_PRO_AEAWB_AEVW_MIN				(2)		/**< Minimum setting of AEVW	*/
#define	D_IM_PRO_AEAWB_AEVW_MAX				(256)	/**< Maximum setting of AEVW	*/
#define	D_IM_PRO_AEAWB_AEHW_MIN				(6)		/**< Minimum setting of AEHW	*/
#define	D_IM_PRO_AEAWB_AEHW_MAX				(512)	/**< Maximum setting of AEHW	*/
#define	D_IM_PRO_AEAWB_AETW_MIN				(2)		/**< Minimum setting of AETW	*/
#define	D_IM_PRO_AEAWB_AETW_MAX				(256)	/**< Maximum setting of AETW	*/
#define	D_IM_PRO_AEAWB_AEBW_MIN				(2)		/**< Minimum setting of AEBW	*/
#define	D_IM_PRO_AEAWB_AEBW_MAX				(256)	/**< Maximum setting of AEBW	*/
#define	D_IM_PRO_AEAWB_AELW_MIN				(6)		/**< Minimum setting of AELW	*/
#define	D_IM_PRO_AEAWB_AELW_MAX				(512)	/**< Maximum setting of AELW	*/
#define	D_IM_PRO_AEAWB_AERW_MIN				(6)		/**< Minimum setting of AERW	*/
#define	D_IM_PRO_AEAWB_AERW_MAX				(512)	/**< Maximum setting of AERW	*/
/*	@}*/	// im_pro_stat_aeawb

/** @addtogroup im_pro_stat_flck FLCK
@{
*/

#define	D_IM_PRO_FLCK_FLV_MIN		(0)		/**< Minimum setting of FLV			*/
#define	D_IM_PRO_FLCK_FLV_MAX		(16382)	/**< Minimum setting of FLV			*/
#define	D_IM_PRO_FLCK_FLH_MIN		(0)		/**< Minimum setting of FLH			*/
#define	D_IM_PRO_FLCK_FLH_MAX		(8190)	/**< Maximum setting of FLH			*/
#define	D_IM_PRO_FLCK_FLVW_MIN		(2)		/**< Minimum setting of FLVW		*/
#define	D_IM_PRO_FLCK_FLVW_MAX		(1024)	/**< Maximum setting of FLVW		*/
#define	D_IM_PRO_FLCK_FLCVW_MIN		(2)		/**< Minimum setting of FLCVW		*/
#define	D_IM_PRO_FLCK_FLCVW_MAX		(1024)	/**< Maximum setting of FLCVW		*/
#define	D_IM_PRO_FLCK_FLHW_MIN		(6)		/**< Minimum setting of FLHW		*/
#define	D_IM_PRO_FLCK_FLHW_MAX		(8192)	/**< Maximum setting of FLHW		*/
#define	D_IM_PRO_FLCK_FLBC_MIN		(1)		/**< Minimum setting of FLBC		*/
#define	D_IM_PRO_FLCK_FLBC_MAX		(4096)	/**< Maximum setting of FLBC		*/

/*	@}*/	// im_pro_stat_flck

/** @addtogroup im_pro_stat_hist HIST
@{
*/
#define	D_IM_PRO_HIST_HISTV_MIN		(0)			/**< Minimum setting of HISTV	*/
#define	D_IM_PRO_HIST_HISTV_MAX		(16382)		/**< Maximum setting of HISTV	*/
#define	D_IM_PRO_HIST_HISTH_MIN		(0)			/**< Minimum setting of HISTH	*/
#define	D_IM_PRO_HIST_HISTH_MAX		(16382)		/**< Maximum setting of HISTH	*/
#define	D_IM_PRO_HIST_HISTVW_MIN	(2)			/**< Minimum setting of HISTVW	*/
#define	D_IM_PRO_HIST_HISTVW_MAX	(16382)		/**< Maximum setting of HISTVW	*/
#define	D_IM_PRO_HIST_HISTHW_MIN	(2)			/**< Minimum setting of HISTHW	*/
#define	D_IM_PRO_HIST_HISTHW_MAX	(16382)		/**< Maximum setting of HISTHW	*/
/*	@}*/	// im_pro_stat_hist

/** @addtogroup im_pro_pas_spt SPT
@{
*/
#define	D_IM_PRO_SPT_SPTBSV_MIN		(2)			/**< Minimum setting of SPTBSV	*/
#define	D_IM_PRO_SPT_SPTBSV_MAX		(255)		/**< Maximum setting of SPTBSV	*/
#define	D_IM_PRO_SPT_SPTBSH_MIN		(2)			/**< Minimum setting of SPTBSH	*/
#define	D_IM_PRO_SPT_SPTBSH_MAX		(255)		/**< Maximum setting of SPTBSH	*/
#define	D_IM_PRO_SPT_SPTBDE_MIN		(0)			/**< Minimum setting of SPTBDE	*/
#define	D_IM_PRO_SPT_SPTBDE_MAX		(14)		/**< Maximum setting of SPTBDE	*/
#define	D_IM_PRO_SPT_SPTV_MIN		(0)			/**< Minimum setting of SPTV	*/
#define	D_IM_PRO_SPT_SPTV_MAX		(4095)		/**< Maximum setting of SPTV	*/
#define	D_IM_PRO_SPT_SPTH_MIN		(0)			/**< Minimum setting of SPTH	*/
#define	D_IM_PRO_SPT_SPTH_MAX		(8191)		/**< Maximum setting of SPTH	*/
#define	D_IM_PRO_SPT_SPTVBC_MIN		(1)			/**< Minimum setting of SPTVBC	*/
#define	D_IM_PRO_SPT_SPTVBC_MAX		(15)		/**< Maximum setting of SPTVBC	*/
#define	D_IM_PRO_SPT_SPTHBC_MIN		(1)			/**< Minimum setting of SPTHBC	*/
#define	D_IM_PRO_SPT_SPTHBC_MAX		(15)		/**< Maximum setting of SPTHBC	*/
/*	@}*/	// im_pro_pas_spt

/** @addtogroup im_pro_common_p2m P2M
@{
*/
#define	D_IM_PRO_P2M_PBSFT_MIN		(0)			/**< Minimum setting of PBSFT	*/
#define	D_IM_PRO_P2M_PBSFT_MAX		(8)			/**< Maximum setting of PBSFT	*/
#define	D_IM_PRO_P2M_PTRMV_MIN		(0)			/**< Minimum setting of PTRMV	*/
#define	D_IM_PRO_P2M_PTRMV_MAX		(8191)		/**< Maximum setting of PTRMV	*/
#define	D_IM_PRO_P2M_PTRMH_MIN		(0)			/**< Minimum setting of PTRMH	*/
#define	D_IM_PRO_P2M_PTRMH_MAX		(12287)		/**< Maximum setting of PTRMH	*/
/*	@}*/	// im_pro_common_p2m

/** @addtogroup im_pro_common_m2p M2P
@{
*/
#define	D_IM_PRO_M2P_M2PBT_MIN		(8)			/**< Minimum setting of M2PBT	*/
#define	D_IM_PRO_M2P_M2PBT_MAX		(16)		/**< Maximum setting of M2PBT	*/
/*	@}*/	// im_pro_common_m2p

/** @addtogroup im_pro_sen_sg SG
@{
*/
#define	D_IM_PRO_SG_VHD_CH_NUM		(2)			/**< Number of SG VD/HD ch	*/
#define	D_IM_PRO_SG_VDOCYC_MIN		(0x0004)	/**< Minimum setting of VDOCYC	*/
#define	D_IM_PRO_SG_VDDLY_MAX		(255)		/**< Maximum setting of VDDLY	*/
#define	D_IM_PRO_SG_HDOCYC_MAX		(0x3FFF)	/**< Maximum setting of HDOCYC	*/
#define	D_IM_PRO_SG_HDOW_MAX		(0x1FF)		/**< Maximum setting of HDOW	*/
#define	D_IM_PRO_SG_VDOW_MAX		(0x1FF)		/**< Maximum setting of VDOW	*/


/*	@}*/	// im_pro_sen_sg

/** @addtogroup im_pro_sen_lvds LVDS
@{
*/
#define	D_IM_PRO_LVDS_LHSIZE_MIN		(4)			/**< Minimum setting of LHSIZE		*/
#define	D_IM_PRO_LVDS_LVDSINTADR_MAX	(8960)		/**< Maximum setting of LVDSINTADR	*/
/*	@}*/	// im_pro_sen_lvds

/** @addtogroup im_pro_sen_dmipi	MIPI-DPHY
@{
*/
/*	@}*/	// im_pro_sen_dmipi

/** @addtogroup im_pro_common_prch PRCH
@{
*/
#define	D_IM_PRO_PRCH_ARCACHE_MAX		(0xF)		/**< Maximum setting of ARCACHE	*/
#define	D_IM_PRO_PRCH_ARPROT_MAX		(0x7)		/**< Maximum setting of ARPROT	*/
/*	@}*/	// im_pro_common_prch


/** @addtogroup im_pro_handler_sen SEN Interrupt
@{
*/
#define	D_IM_PRO_SENTOP_GYROINT_NUM		(4)			/**< Number of GYRO_INT.	*/

/*	@}*/	// im_pro_handler_sen

/** @addtogroup im_pro_sro_sdc SDC
@{
*/
#define	D_IM_PRO_SDC_GAIN_TBL_NUM		(128)		/**< index Number of SDC gain table.	*/
/*	@}*/	// im_pro_sro_sdc

/** @addtogroup im_pro_sen_obd OBD
@{
*/
#define	D_IM_PRO_OBD_OBDV_MIN			(0)			/**< Minimum setting of OBDV	*/
#define	D_IM_PRO_OBD_OBDV_MAX			(8191)		/**< Maximum setting of OBDV	*/
#define	D_IM_PRO_OBD_OBDH_MIN			(0)			/**< Minimum setting of OBDH	*/
#define	D_IM_PRO_OBD_OBDH_MAX			(12287)		/**< Maximum setting of OBDH	*/
#define	D_IM_PRO_OBD_OBDVW_MIN			(2)			/**< Minimum setting of OBDVW	*/
#define	D_IM_PRO_OBD_OBDVW_MAX			(8192)		/**< Maximum setting of OBDVW	*/
#define	D_IM_PRO_OBD_OBDHW_MIN			(2)			/**< Minimum setting of OBDHW	*/
#define	D_IM_PRO_OBD_OBDHW_MAX			(12288)		/**< Maximum setting of OBDHW	*/
/*	@}*/	// im_pro_sen_obd

/** @addtogroup im_pro_sen_obt OBT
@{
*/
#define	D_IM_PRO_OBT_OBTV_MIN			(0)			/**< Minimum setting of OBTV	*/
#define	D_IM_PRO_OBT_OBTV_MAX			(8191)		/**< Maximum setting of OBTV	*/
#define	D_IM_PRO_OBT_OBTH_MIN			(0)			/**< Minimum setting of OBTH	*/
#define	D_IM_PRO_OBT_OBTH_MAX			(12287)		/**< Maximum setting of OBTH	*/
#define	D_IM_PRO_OBT_OBTVW_MIN			(1)			/**< Minimum setting of OBTVW	*/
#define	D_IM_PRO_OBT_OBTVW_MAX			(127)		/**< Maximum setting of OBTVW	*/
#define	D_IM_PRO_OBT_OBTHW_MIN			(1)			/**< Minimum setting of OBTHW	*/
#define	D_IM_PRO_OBT_OBTHW_MAX			(12288)		/**< Maximum setting of OBTHW	*/
/*	@}*/	// im_pro_sen_obt

/** @addtogroup im_pro_common_pg PG
@{
*/
#define	D_IM_PRO_PG_PGVW_MIN			(2)			/**< Minimum setting of PGVW	*/
#define	D_IM_PRO_PG_PGVW_MAX			(8192)		/**< Maximum setting of PGVW	*/
#define	D_IM_PRO_PG_PGHW_MIN			(2)			/**< Minimum setting of PGHW	*/
#define	D_IM_PRO_PG_PGHW_MAX			(12288)		/**< Maximum setting of PGHW	*/
#define	D_IM_PRO_PG_PGVBLNK_MIN			(1)			/**< Minimum setting of PGVBLNK	*/
#define	D_IM_PRO_PG_PGVBLNK_MAX			(8192)		/**< Maximum setting of PGVBLNK	*/
#define	D_IM_PRO_PG_PGHBLNK_MIN			(0)			/**< Minimum setting of PGHBLNK	*/
#define	D_IM_PRO_PG_PGHBLNK_MAX			(8192)		/**< Maximum setting of PGHBLNK	*/
#define	D_IM_PRO_PG_PGVHBLNK_MIN		(0)			/**< Minimum setting of PGVHBLNK	*/
#define	D_IM_PRO_PG_PGVHBLNK_MAX		(16383)		/**< Maximum setting of PGVHBLNK	*/
/*	@}*/	// im_pro_common_pg

/** @addtogroup im_pro_sro_ffsh FFSH
@{
*/
#define	D_IM_PRO_FFSH_FFSHV_MIN			(2)			/**< Minimum setting of FFSHV	*/
#define	D_IM_PRO_FFSH_FFSHV_MAX			(8960)		/**< Maximum setting of FFSHV	*/
#define	D_IM_PRO_FFSH_FFSHH_MIN			(0)			/**< Minimum setting of FFSHH	*/
#define	D_IM_PRO_FFSH_FFSHH_MAX			(4095)		/**< Maximum setting of FFSHH	*/
#define	D_IM_PRO_FFSH_FFSHVW_MIN		(4)			/**< Minimum setting of FFSHVW	*/
#define	D_IM_PRO_FFSH_FFSHVW_MAX		(8960)		/**< Maximum setting of FFSHVW	*/
#define	D_IM_PRO_FFSH_FFSHHW_MIN		(4)			/**< Minimum setting of FFSHHW	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_FFSH_FFSHHW_MAX		(2496)		/**< Maximum setting of FFSHHW	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_FFSH_FFSHHW_MAX		(2900)		/**< Maximum setting of FFSHHW	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_FFSH_FFSHSUBPRH_MIN	(4)			/**< Minimum setting of FFSHSUBPRH	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_FFSH_FFSHSUBPRH_MAX	(314)		/**< Maximum setting of FFSHSUBPRH	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_FFSH_FFSHSUBPRH_MAX	(366)		/**< Maximum setting of FFSHSUBPRH	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_FFSH_FFSHSUBPRV_MIN	(4)			/**< Minimum setting of FFSHSUBPRV	*/
#define	D_IM_PRO_FFSH_FFSHSUBPRV_MAX	(1122)		/**< Maximum setting of FFSHSUBPRV	*/
#define	D_IM_PRO_FFSH_FFSHSUBSPH_MIN	(16)		/**< Minimum setting of FFSHSUBSPH	*/
#define	D_IM_PRO_FFSH_FFSHSUBSPH_MAX	(724)		/**< Maximum setting of FFSHSUBSPH	*/
#define	D_IM_PRO_FFSH_FFSHSUBSPV_MIN	(16)		/**< Minimum setting of FFSHSUBSPV	*/
#define	D_IM_PRO_FFSH_FFSHSUBSPV_MAX	(724)		/**< Maximum setting of FFSHSUBSPV	*/
#define	D_IM_PRO_FFSH_FFSHSUBSTH_MIN	(0)			/**< Minimum setting of FFSHSUBSTH	*/
#define	D_IM_PRO_FFSH_FFSHSUBSTH_MAX	(722)		/**< Maximum setting of FFSHSUBSTH	*/
#define	D_IM_PRO_FFSH_FFSHSUBSTV_MIN	(0)			/**< Minimum setting of FFSHSUBSTV	*/
#define	D_IM_PRO_FFSH_FFSHSUBSTV_MAX	(722)		/**< Maximum setting of FFSHSUBSTV	*/
#define	D_IM_PRO_FFSH_FFSHWRIC_MIN		(4)			/**< Minimum setting of FFSHWRIC	*/
#define	D_IM_PRO_FFSH_FFSHWRIC_MAX		(1023)		/**< Maximum setting of FFSHWRIC	*/
/*	@}*/	// im_pro_sro_ffsh

/** @addtogroup im_pro_sro_ff FF
@{
*/
#define	D_IM_PRO_FF_FFV_MIN				(0)			/**< Minimum setting of FFV	*/
#define	D_IM_PRO_FF_FFV_MAX				(8191)		/**< Maximum setting of FFV	*/
#define	D_IM_PRO_FF_FFH_MIN				(0)			/**< Minimum setting of FFH	*/
#define	D_IM_PRO_FF_FFH_MAX				(12287)		/**< Maximum setting of FFH	*/
#define	D_IM_PRO_FF_FFVW_MIN			(4)			/**< Minimum setting of FFVW	*/
#define	D_IM_PRO_FF_FFVW_MAX			(8192)		/**< Maximum setting of FFVW	*/
#define	D_IM_PRO_FF_FFHW_MIN			(4)			/**< Minimum setting of FFHW	*/
#define	D_IM_PRO_FF_FFHW_MAX			(12288)		/**< Maximum setting of FFHW	*/
/*	@}*/	// im_pro_sro_ff

/** @addtogroup im_pro_sro_sdc SDC
@{
*/
#define	D_IM_PRO_SDC_SDCPHW_MIN			(0)			/**< Minimum setting of SDCPHW	*/
#define	D_IM_PRO_SDC_SDCPHW_MAX			(64)		/**< Maximum setting of SDCPHW	*/
#define	D_IM_PRO_SDC_SDCPxHW_MIN		(0)			/**< Minimum setting of SDCPxHW	*/
#define	D_IM_PRO_SDC_SDCPxHW_MAX		(8191)		/**< Maximum setting of SDCPxHW	*/
#define	D_IM_PRO_SDC_SDCNxHW_MIN		(0)			/**< Minimum setting of SDCNxHW	*/
#define	D_IM_PRO_SDC_SDCNxHW_MAX		(8191)		/**< Maximum setting of SDCNxHW	*/
#define	D_IM_PRO_SDC_SDCPC_MIN			(0)			/**< Minimum setting of SDCPC	*/
#define	D_IM_PRO_SDC_SDCPC_MAX			(8)			/**< Maximum setting of SDCPC	*/
#define	D_IM_PRO_SDC_SDCPCW_MIN			(0)			/**< Minimum setting of SDCPCW	*/
#define	D_IM_PRO_SDC_SDCPCW_MAX			(64)		/**< Maximum setting of SDCPCW	*/
#define	D_IM_PRO_SDC_SDCPxCW_MIN		(0)			/**< Minimum setting of SDCPxCW	*/
#define	D_IM_PRO_SDC_SDCPxCW_MAX		(8191)		/**< Maximum setting of SDCPxCW	*/
#define	D_IM_PRO_SDC_SDCNxCW_MIN		(0)			/**< Minimum setting of SDCNxCW	*/
#define	D_IM_PRO_SDC_SDCNxCW_MAX		(8191)		/**< Maximum setting of SDCNxCW	*/
#define	D_IM_PRO_SDC_SDCN2CW_MIN		(1)			/**< Minimum setting of SDCN2CW	*/
#define	D_IM_PRO_SDC_SDCN2CW_MAX		(8191)		/**< Maximum setting of SDCN2CW	*/
#define	D_IM_PRO_SDC_SDCNMAX_MIN		(0)			/**< Minimum setting of SDCNMAX	*/
#define	D_IM_PRO_SDC_SDCNMAX_MAX		(3)			/**< Maximum setting of SDCNMAX	*/
/*	@}*/	// im_pro_sro_sdc

/** @addtogroup im_pro_sro_dpc DPC
@{
*/
#define	D_IM_PRO_DPC_DPCGV_MIN			(0)			/**< Minimum setting of DPCGV	*/
#define	D_IM_PRO_DPC_DPCGV_MAX			(8960)		/**< Maximum setting of DPCGV	*/
#define	D_IM_PRO_DPC_DPCGH_MIN			(0)			/**< Minimum setting of DPCGH	*/
#define	D_IM_PRO_DPC_DPCGVW_MIN			(10)		/**< Minimum setting of DPCGVW	*/
#define	D_IM_PRO_DPC_DPCGVW_MAX			(8960)		/**< Maximum setting of DPCGVW	*/
#define	D_IM_PRO_DPC_DPCGHW_MIN			(10)		/**< Minimum setting of DPCGHW	*/
#define	D_IM_PRO_DPC_DPCV_MIN			(0)			/**< Minimum setting of DPCV	*/
#define	D_IM_PRO_DPC_DPCV_MAX			(8950)		/**< Maximum setting of DPCV	*/
#define	D_IM_PRO_DPC_DPCH_MIN			(0)			/**< Minimum setting of DPCH	*/
#define	D_IM_PRO_DPC_DPCVW_MIN			(10)		/**< Minimum setting of DPCVW	*/
#define	D_IM_PRO_DPC_DPCVW_MAX			(8960)		/**< Maximum setting of DPCVW	*/
#define	D_IM_PRO_DPC_DPCHW_MIN			(10)		/**< Minimum setting of DPCHW	*/
#define	D_IM_PRO_DPC_DPCBLW_MIN			(0)			/**< Minimum setting of DPCBLWP/M	*/
#define	D_IM_PRO_DPC_DPCBLW_MAX			(15)		/**< Maximum setting of DPCBLWP/M	*/

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_DPC_DPCGH_MAX			(2496)		/**< Maximum setting of DPCGH	*/
#define	D_IM_PRO_DPC_DPCGHW_MAX			(2496)		/**< Maximum setting of DPCGHW	*/
#define	D_IM_PRO_DPC_DPCH_MAX			(2486)		/**< Maximum setting of DPCH	*/
#define	D_IM_PRO_DPC_DPCHW_MAX			(2496)		/**< Maximum setting of DPCHW	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_DPC_DPCGH_MAX			(2900)		/**< Maximum setting of DPCGH	*/
#define	D_IM_PRO_DPC_DPCGHW_MAX			(2900)		/**< Maximum setting of DPCGHW	*/
#define	D_IM_PRO_DPC_DPCH_MAX			(2890)		/**< Maximum setting of DPCH	*/
#define	D_IM_PRO_DPC_DPCHW_MAX			(2900)		/**< Maximum setting of DPCHW	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*	@}*/	// im_pro_sro_dpc

/** @addtogroup im_pro_sro_afpb AFPB
@{
*/
#define	D_IM_PRO_AFPB_AFPBV_MIN			(0)			/**< Minimum setting of AFPBV	*/
#define	D_IM_PRO_AFPB_AFPBV_MAX			(16380)		/**< Maximum setting of AFPBV	*/
#define	D_IM_PRO_AFPB_AFPBH_MIN			(0)			/**< Minimum setting of AFPBH	*/
#define	D_IM_PRO_AFPB_AFPBH_MAX			(16382)		/**< Maximum setting of AFPBH	*/
#define	D_IM_PRO_AFPB_AFPBVW_MIN		(2)			/**< Minimum setting of AFPBVW	*/
#define	D_IM_PRO_AFPB_AFPBVW_MAX		(16382)		/**< Maximum setting of AFPBVW	*/
#define	D_IM_PRO_AFPB_AFPBHW_MIN		(1)			/**< Minimum setting of AFPBHW	*/
#define	D_IM_PRO_AFPB_AFPBHW_MAX		(16383)		/**< Maximum setting of AFPBHW	*/
/*	@}*/	// im_pro_sro_afpb

/** @addtogroup im_pro_sro_fshdl FSHDL
@{
*/
#define	D_IM_PRO_FSHDL_FFSHV_MIN		(2)			/**< Minimum setting of FFSHV	*/
#define	D_IM_PRO_FSHDL_FFSHV_MAX		(8960)		/**< Maximum setting of FFSHV	*/
#define	D_IM_PRO_FSHDL_FFSHH_MIN		(0)			/**< Minimum setting of FFSHH	*/
#define	D_IM_PRO_FSHDL_FFSHH_MAX		(4095)		/**< Maximum setting of FFSHH	*/
#define	D_IM_PRO_FSHDL_FFSHVW_MIN		(4)			/**< Minimum setting of FFSHVW	*/
#define	D_IM_PRO_FSHDL_FFSHVW_MAX		(8960)		/**< Maximum setting of FFSHVW	*/
#define	D_IM_PRO_FSHDL_FFSHHW_MIN		(4)			/**< Minimum setting of FFSHHW	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_FSHDL_FFSHHW_MAX		(2496)		/**< Maximum setting of FFSHHW	*/
#define	D_IM_PRO_FSHDL_FFSHHWPH_MAX		(2496)		/**< Maximum setting of FFSHHWPH	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_FSHDL_FFSHHW_MAX		(2900)		/**< Maximum setting of FFSHHW	*/
#define	D_IM_PRO_FSHDL_FFSHHWPH_MAX		(2900)		/**< Maximum setting of FFSHHWPH	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_FSHDL_FFSHULRR_MIN		(0)			/**< Minimum setting of FFSHULRR	*/
#define	D_IM_PRO_FSHDL_FFSHULRR_MAX		(16383)		/**< Maximum setting of FFSHULRR	*/
#define	D_IM_PRO_FSHDL_FFSHULGR_MIN		(0)			/**< Minimum setting of FFSHULGR	*/
#define	D_IM_PRO_FSHDL_FFSHULGR_MAX		(16383)		/**< Maximum setting of FFSHULGR	*/
#define	D_IM_PRO_FSHDL_FFSHULGB_MIN		(0)			/**< Minimum setting of FFSHULGB	*/
#define	D_IM_PRO_FSHDL_FFSHULGB_MAX		(16383)		/**< Maximum setting of FFSHULGB	*/
#define	D_IM_PRO_FSHDL_FFSHULBB_MIN		(0)			/**< Minimum setting of FFSHULBB	*/
#define	D_IM_PRO_FSHDL_FFSHULBB_MAX		(16383)		/**< Maximum setting of FFSHULBB	*/
#define	D_IM_PRO_FSHDL_FFSHSUBPRH_MIN	(4)			/**< Minimum setting of FFSHSUBPRH	*/
#define	D_IM_PRO_FSHDL_FFSHHWPH_MIN		(4)			/**< Minimum setting of FFSHHWPH	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_FSHDL_FFSHSUBPRH_MAX	(314)		/**< Maximum setting of FFSHSUBPRH	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_FSHDL_FFSHSUBPRH_MAX	(366)		/**< Maximum setting of FFSHSUBPRH	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_FSHDL_FFSHSUBPRV_MIN	(4)			/**< Minimum setting of FFSHSUBPRV	*/
#define	D_IM_PRO_FSHDL_FFSHSUBPRV_MAX	(1122)		/**< Maximum setting of FFSHSUBPRV	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSPH_MIN	(16)		/**< Minimum setting of FFSHSUBSPH	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSPH_MAX	(724)		/**< Maximum setting of FFSHSUBSPH	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSPV_MIN	(16)		/**< Minimum setting of FFSHSUBSPV	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSPV_MAX	(724)		/**< Maximum setting of FFSHSUBSPV	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSTH_MIN	(0)			/**< Minimum setting of FFSHSUBSTH	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSTH_MAX	(722)		/**< Maximum setting of FFSHSUBSTH	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSTV_MIN	(0)			/**< Minimum setting of FFSHSUBSTV	*/
#define	D_IM_PRO_FSHDL_FFSHSUBSTV_MAX	(722)		/**< Maximum setting of FFSHSUBSTV	*/
#define	D_IM_PRO_FSHDL_FFSHWRIC_MIN		(4)			/**< Minimum setting of FFSHWRIC	*/
#define	D_IM_PRO_FSHDL_FFSHWRIC_MAX		(1023)		/**< Maximum setting of FFSHWRIC	*/
/*	@}*/	// im_pro_sro_fshdl

/** @addtogroup im_pro_sro_cag CAG
@{
*/
#define	D_IM_PRO_CAG_CAGGV_MIN			(0)			/**< Minimum setting of CAGGV	*/
#define	D_IM_PRO_CAG_CAGGV_MAX			(8960)		/**< Maximum setting of CAGGV	*/
#define	D_IM_PRO_CAG_CAGGH_MIN			(0)			/**< Minimum setting of CAGGH	*/
#define	D_IM_PRO_CAG_CAGGVW_MIN			(10)		/**< Minimum setting of CAGGVW	*/
#define	D_IM_PRO_CAG_CAGGVW_MAX			(8960)		/**< Maximum setting of CAGGVW	*/
#define	D_IM_PRO_CAG_CAGGHW_MIN			(18)		/**< Minimum setting of CAGGHW	*/
#define	D_IM_PRO_CAG_CAGV_MIN			(4)			/**< Minimum setting of CAGV	*/
#define	D_IM_PRO_CAG_CAGV_MAX			(8954)		/**< Maximum setting of CAGV	*/
#define	D_IM_PRO_CAG_CAGH_MIN			(8)			/**< Minimum setting of CAGH	*/
#define	D_IM_PRO_CAG_CAGVW_MIN			(2)			/**< Minimum setting of CAGVW	*/
#define	D_IM_PRO_CAG_CAGVW_MAX			(8952)		/**< Maximum setting of CAGVW	*/
#define	D_IM_PRO_CAG_CAGHW_MIN			(1)			/**< Minimum setting of CAGHW	*/

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_CAG_CAGGH_MAX			(2496)		/**< Maximum setting of CAGGH	*/
#define	D_IM_PRO_CAG_CAGGHW_MAX			(2496)		/**< Maximum setting of CAGGHW	*/
#define	D_IM_PRO_CAG_CAGH_MAX			(2487)		/**< Maximum setting of CAGH	*/
#define	D_IM_PRO_CAG_CAGHW_MAX			(2480)		/**< Maximum setting of CAGHW	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_CAG_CAGGH_MAX			(2900)		/**< Maximum setting of CAGGH	*/
#define	D_IM_PRO_CAG_CAGGHW_MAX			(2900)		/**< Maximum setting of CAGGHW	*/
#define	D_IM_PRO_CAG_CAGH_MAX			(2891)		/**< Maximum setting of CAGH	*/
#define	D_IM_PRO_CAG_CAGHW_MAX			(2884)		/**< Maximum setting of CAGHW	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/*	@}*/	// im_pro_sro_cag

/** @addtogroup im_pro_sro_ffc FFC
@{
*/
#define	D_IM_PRO_FFC_FCV_MIN			(2)			/**< Minimum setting of FCV			*/
#define	D_IM_PRO_FFC_FCV_MAX			(8960)		/**< Maximum setting of FCV			*/
#define	D_IM_PRO_FFC_FCH_MIN			(0)			/**< Minimum setting of FCH			*/
#define	D_IM_PRO_FFC_FCH_MAX			(4095)		/**< Maximum setting of FCH			*/
#define	D_IM_PRO_FFC_FCVW_MIN			(4)			/**< Minimum setting of FCVW		*/
#define	D_IM_PRO_FFC_FCVW_MAX			(8960)		/**< Maximum setting of FCVW		*/
#define	D_IM_PRO_FFC_FCHW_MIN			(4)			/**< Minimum setting of FCHW		*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_FFC_FCHW_MAX			(2496)		/**< Maximum setting of FCHW		*/
#define	D_IM_PRO_FFC_FCSUBPRH_MAX		(314)		/**< Maximum setting of FCSUBPRH	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_FFC_FCHW_MAX			(2900)		/**< Maximum setting of FCHW		*/
#define	D_IM_PRO_FFC_FCSUBPRH_MAX		(366)		/**< Maximum setting of FCSUBPRH	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_FFC_FCSUBPRH_MIN		(4)			/**< Minimum setting of FCSUBPRH	*/
#define	D_IM_PRO_FFC_FCSUBPRV_MIN		(4)			/**< Minimum setting of FCSUBPRV	*/
#define	D_IM_PRO_FFC_FCSUBPRV_MAX		(1122)		/**< Maximum setting of FCSUBPRV	*/
#define	D_IM_PRO_FFC_FCSUBSPH_MIN		(16)		/**< Minimum setting of FCSUBSPH	*/
#define	D_IM_PRO_FFC_FCSUBSPH_MAX		(724)		/**< Maximum setting of FCSUBSPH	*/
#define	D_IM_PRO_FFC_FCSUBSPV_MIN		(16)		/**< Minimum setting of FCSUBSPV	*/
#define	D_IM_PRO_FFC_FCSUBSPV_MAX		(724)		/**< Maximum setting of FCSUBSPV	*/
#define	D_IM_PRO_FFC_FCSUBSTH_MIN		(0)			/**< Minimum setting of FCSUBSTH	*/
#define	D_IM_PRO_FFC_FCSUBSTH_MAX		(722)		/**< Maximum setting of FCSUBSTH	*/
#define	D_IM_PRO_FFC_FCSUBSTV_MIN		(0)			/**< Minimum setting of FCSUBSTV	*/
#define	D_IM_PRO_FFC_FCSUBSTV_MAX		(722)		/**< Maximum setting of FCSUBSTV	*/
#define	D_IM_PRO_FFC_FCWRIC_MIN			(4)			/**< Minimum setting of FCWRIC		*/
#define	D_IM_PRO_FFC_FCWRIC_MAX			(1023)		/**< Maximum setting of FCWRIC		*/
/*	@}*/	// im_pro_sro_ffc

/** @addtogroup im_pro_sro_vrb VRB
@{
*/
#define	D_IM_PRO_VRB_VRBV_MIN			(0)			/**< Minimum setting of VRBV		*/
#define	D_IM_PRO_VRB_VRBV_MAX			(8190)		/**< Maximum setting of VRBV		*/
#define	D_IM_PRO_VRB_VRBH_MIN			(0)			/**< Minimum setting of VRBH		*/
#define	D_IM_PRO_VRB_VRBH_MAX			(16382)		/**< Maximum setting of VRBH		*/
#define	D_IM_PRO_VRB_VRBVW_MIN			(1)			/**< Minimum setting of VRBVW		*/
#define	D_IM_PRO_VRB_VRBVW_MAX			(8191)		/**< Maximum setting of VRBVW		*/
#define	D_IM_PRO_VRB_VRBHW_MIN			(2)			/**< Minimum setting of VRBHW		*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_VRB_VRBHW_MAX			(2496)		/**< Maximum setting of VRBHW		*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_VRB_VRBHW_MAX			(2900)		/**< Maximum setting of VRBHW		*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_VRB_VRBSCYC_MIN		(1)			/**< Minimum setting of VRBSCYC		*/
#define	D_IM_PRO_VRB_VRBSCYC_MAX		(10)		/**< Maximum setting of VRBSCYC		*/
/*	@}*/	// im_pro_sro_vrb

/** @addtogroup im_pro_sro_hrb HRB
@{
*/
#define	D_IM_PRO_HRB_HRBV_MIN			(0)			/**< Minimum setting of HRBV		*/
#define	D_IM_PRO_HRB_HRBV_MAX			(8190)		/**< Maximum setting of HRBV		*/
#define	D_IM_PRO_HRB_HRBH_MIN			(0)			/**< Minimum setting of HRBH		*/
#define	D_IM_PRO_HRB_HRBH_MAX			(16382)		/**< Maximum setting of HRBH		*/
#define	D_IM_PRO_HRB_HRBVW_MIN			(1)			/**< Minimum setting of HRBVW		*/
#define	D_IM_PRO_HRB_HRBVW_MAX			(8191)		/**< Maximum setting of HRBVW		*/
#define	D_IM_PRO_HRB_HRBHW_MIN			(2)			/**< Minimum setting of HRBHW		*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_HRB_HRBHW_MAX			(2496)		/**< Maximum setting of HRBHW		*/
#define	D_IM_PRO_HRB_HRBOHW_MAX			(2496)		/**< Maximum setting of HRBOHW		*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_HRB_HRBHW_MAX			(2900)		/**< Maximum setting of HRBHW		*/
#define	D_IM_PRO_HRB_HRBOHW_MAX			(2900)		/**< Maximum setting of HRBOHW		*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_HRB_HRBOHW_MIN			(2)			/**< Minimum setting of HRBOHW		*/
#define	D_IM_PRO_HRB_HRBSCYC_MIN		(1)			/**< Minimum setting of HRBSCYC		*/
#define	D_IM_PRO_HRB_HRBSCYC_MAX		(20)		/**< Maximum setting of HRBSCYC		*/
#define	D_IM_PRO_HRB_HRBOSM_MIN			(0)			/**< Minimum setting of HRBOSM		*/
#define	D_IM_PRO_HRB_HRBOSM_MAX			(8191)		/**< Maximum setting of HRBOSM		*/
/*	@}*/	// im_pro_sro_hrb

// --- REMOVE_BUZ BEGIN
// @cond
/** @addtogroup im_pro_sro_buztop BUZTOP
@{
*/
#define	D_IM_PRO_BUZ_BUZV_MIN			(0)			/**< Minimum setting of BUZV		*/
#define	D_IM_PRO_BUZ_BUZV_MAX			(16382)		/**< Maximum setting of BUZV		*/
#define	D_IM_PRO_BUZ_BUZH_MIN			(0)			/**< Minimum setting of BUZH		*/
#define	D_IM_PRO_BUZ_BUZH_MAX			(4095)		/**< Maximum setting of BUZH		*/
#define	D_IM_PRO_BUZ_BUZVW_MIN			(2)			/**< Minimum setting of BUZVW		*/
#define	D_IM_PRO_BUZ_BUZVW_MAX			(4094)		/**< Maximum setting of BUZVW		*/
#define	D_IM_PRO_BUZ_BUZHW_MIN			(1)			/**< Minimum setting of BUZHW		*/
#define	D_IM_PRO_BUZ_BUZHW_MAX			(4095)		/**< Maximum setting of BUZHW		*/
#define	D_IM_PRO_BUZ_BUZOHW_MIN			(1)			/**< Minimum setting of BUZOHW		*/
#define	D_IM_PRO_BUZ_BUZOHW_MAX			(4095)		/**< Maximum setting of BUZOHW		*/
/*	@}*/	// im_pro_sro_buztop
// @endcond
// --- REMOVE_BUZ END

/** @addtogroup im_pro_b2b_elf ELF
@{
*/
#define	D_IM_PRO_ELF_ELFGV_MIN			(0)			/**< Minimum setting of ELFGV	*/
#define	D_IM_PRO_ELF_ELFGV_MAX			(8960)		/**< Maximum setting of ELFGV	*/
#define	D_IM_PRO_ELF_ELFGH_MIN			(0)			/**< Minimum setting of ELFGH	*/
#define	D_IM_PRO_ELF_ELFGH_MAX			(2496)		/**< Maximum setting of ELFGH	*/
#define	D_IM_PRO_ELF_ELFGVW_MIN			(10)		/**< Minimum setting of ELFGVW	*/
#define	D_IM_PRO_ELF_ELFGVW_MAX			(8960)		/**< Maximum setting of ELFGVW	*/
#define	D_IM_PRO_ELF_ELFGHW_MIN			(10)		/**< Minimum setting of ELFGHW	*/
#define	D_IM_PRO_ELF_ELFGHW_MAX			(2496)		/**< Maximum setting of ELFGHW	*/
#define	D_IM_PRO_ELF_ELFV_MIN			(4)			/**< Minimum setting of ELFV	*/
#define	D_IM_PRO_ELF_ELFV_MAX			(8954)		/**< Maximum setting of ELFV	*/
#define	D_IM_PRO_ELF_ELFH_MIN			(4)			/**< Minimum setting of ELFH	*/
#define	D_IM_PRO_ELF_ELFH_MAX			(2491)		/**< Maximum setting of ELFH	*/
#define	D_IM_PRO_ELF_ELFVW_MIN			(2)			/**< Minimum setting of ELFVW	*/
#define	D_IM_PRO_ELF_ELFVW_MAX			(8952)		/**< Maximum setting of ELFVW	*/
#define	D_IM_PRO_ELF_ELFHW_MIN			(2)			/**< Minimum setting of ELFHW	*/
#define	D_IM_PRO_ELF_ELFHW_MAX			(2488)		/**< Maximum setting of ELFHW	*/
/*	@}*/	// im_pro_sro_hrb

/** @addtogroup im_pro_b2b_fshd FSHD
@{
*/
#define	D_IM_PRO_FSHD_FSHDV_MIN			(2)			/**< Minimum setting of FSHDV		*/
#define	D_IM_PRO_FSHD_FSHDV_MAX			(8960)		/**< Maximum setting of FSHDV		*/
#define	D_IM_PRO_FSHD_FSHDH_MIN			(0)			/**< Minimum setting of FSHDH		*/
#define	D_IM_PRO_FSHD_FSHDH_MAX			(4095)		/**< Maximum setting of FSHDH		*/
#define	D_IM_PRO_FSHD_FSHDVW_MIN		(4)			/**< Minimum setting of FSHDVW		*/
#define	D_IM_PRO_FSHD_FSHDVW_MAX		(8960)		/**< Maximum setting of FSHDVW		*/
#define	D_IM_PRO_FSHD_FSHDHW_MIN		(4)			/**< Minimum setting of FSHDHW		*/
#define	D_IM_PRO_FSHD_FSHDHW_MAX		(2496)		/**< Maximum setting of FSHDHW		*/
#define	D_IM_PRO_FSHD_FSHDHWPH_MIN		(4)			/**< Minimum setting of FSHDHWPH	*/
#define	D_IM_PRO_FSHD_FSHDHWPH_MAX		(2494)		/**< Maximum setting of FSHDHWPH	*/
#define	D_IM_PRO_FSHD_FSULRR_MIN		(0)			/**< Minimum setting of FSULRR		*/
#define	D_IM_PRO_FSHD_FSULRR_MAX		(4095)		/**< Maximum setting of FSULRR		*/
#define	D_IM_PRO_FSHD_FSULGR_MIN		(0)			/**< Minimum setting of FSULGR		*/
#define	D_IM_PRO_FSHD_FSULGR_MAX		(4095)		/**< Maximum setting of FSULGR		*/
#define	D_IM_PRO_FSHD_FSULGB_MIN		(0)			/**< Minimum setting of FSULGB		*/
#define	D_IM_PRO_FSHD_FSULGB_MAX		(4095)		/**< Maximum setting of FSULGB		*/
#define	D_IM_PRO_FSHD_FSULBB_MIN		(0)			/**< Minimum setting of FSULBB		*/
#define	D_IM_PRO_FSHD_FSULBB_MAX		(4095)		/**< Maximum setting of FSULBB		*/
#define	D_IM_PRO_FSHD_FSSUBPRH_MIN		(4)			/**< Minimum setting of FSSUBPRH	*/
#define	D_IM_PRO_FSHD_FSSUBPRH_MAX		(314)		/**< Maximum setting of FSSUBPRH	*/
#define	D_IM_PRO_FSHD_FSSUBPRV_MIN		(4)			/**< Minimum setting of FSSUBPRV	*/
#define	D_IM_PRO_FSHD_FSSUBPRV_MAX		(1122)		/**< Maximum setting of FSSUBPRV	*/
#define	D_IM_PRO_FSHD_FSSUBSPH_MIN		(16)		/**< Minimum setting of FSSUBSPH	*/
#define	D_IM_PRO_FSHD_FSSUBSPH_MAX		(724)		/**< Maximum setting of FSSUBSPH	*/
#define	D_IM_PRO_FSHD_FSSUBSPV_MIN		(16)		/**< Minimum setting of FSSUBSPV	*/
#define	D_IM_PRO_FSHD_FSSUBSPV_MAX		(724)		/**< Maximum setting of FSSUBSPV	*/
#define	D_IM_PRO_FSHD_FSSUBDRE_MIN		(0)			/**< Minimum setting of FSSUBDRE	*/
#define	D_IM_PRO_FSHD_FSSUBDRE_MAX		(11)		/**< Maximum setting of FSSUBDRE	*/
#define	D_IM_PRO_FSHD_FSSUBSTH_MIN		(0)			/**< Minimum setting of FSSUBSTH	*/
#define	D_IM_PRO_FSHD_FSSUBSTH_MAX		(722)		/**< Maximum setting of FSSUBSTH	*/
#define	D_IM_PRO_FSHD_FSSUBSTV_MIN		(0)			/**< Minimum setting of FSSUBSTV	*/
#define	D_IM_PRO_FSHD_FSSUBSTV_MAX		(722)		/**< Maximum setting of FSSUBSTV	*/
#define	D_IM_PRO_FSHD_FSARR_MIN			(0)			/**< Minimum setting of FSARR		*/
#define	D_IM_PRO_FSHD_FSARR_MAX			(511)		/**< Maximum setting of FSARR		*/
#define	D_IM_PRO_FSHD_FSAGR_MIN			(0)			/**< Minimum setting of FSAGR		*/
#define	D_IM_PRO_FSHD_FSAGR_MAX			(511)		/**< Maximum setting of FSAGR		*/
#define	D_IM_PRO_FSHD_FSAGB_MIN			(0)			/**< Minimum setting of FSAGB		*/
#define	D_IM_PRO_FSHD_FSAGB_MAX			(511)		/**< Maximum setting of FSAGB		*/
#define	D_IM_PRO_FSHD_FSABB_MIN			(0)			/**< Minimum setting of FSABB		*/
#define	D_IM_PRO_FSHD_FSABB_MAX			(511)		/**< Maximum setting of FSABB		*/
#define	D_IM_PRO_FSHD_CSHDPARV_MIN		(16)		/**< Minimum setting of CSHDPARV	*/
#define	D_IM_PRO_FSHD_CSHDPARV_MAX		(64)		/**< Maximum setting of CSHDPARV	*/
#define	D_IM_PRO_FSHD_CSARR_MIN			(0)			/**< Minimum setting of CSARR		*/
#define	D_IM_PRO_FSHD_CSARR_MAX			(511)		/**< Maximum setting of CSARR		*/
#define	D_IM_PRO_FSHD_CSAGR_MIN			(0)			/**< Minimum setting of CSAGR		*/
#define	D_IM_PRO_FSHD_CSAGR_MAX			(511)		/**< Maximum setting of CSAGR		*/
#define	D_IM_PRO_FSHD_CSAGB_MIN			(0)			/**< Minimum setting of CSAGB		*/
#define	D_IM_PRO_FSHD_CSAGB_MAX			(511)		/**< Maximum setting of CSAGB		*/
#define	D_IM_PRO_FSHD_CSABB_MIN			(0)			/**< Minimum setting of CSABB		*/
#define	D_IM_PRO_FSHD_CSABB_MAX			(511)		/**< Maximum setting of CSABB		*/
#define	D_IM_PRO_FSHD_FSWRIC_MIN		(4)			/**< Minimum setting of FSWRIC		*/
#define	D_IM_PRO_FSHD_FSWRIC_MAX		(1023)		/**< Maximum setting of FSWRIC		*/
/*	@}*/	// im_pro_b2b_fshd

/** @addtogroup im_pro_stat_af AF
@{
*/
#define	D_IM_PRO_AF_AFGV_MIN			(0)			/**< Minimum setting of AFGV	*/
#define	D_IM_PRO_AF_AFGV_MAX			(16382)		/**< Maximum setting of AFGV	*/
#define	D_IM_PRO_AF_AFGH_MIN			(0)			/**< Minimum setting of AFGH	*/
#define	D_IM_PRO_AF_AFGH_MAX			(8190)		/**< Maximum setting of AFGH	*/
#define	D_IM_PRO_AF_AFGVW_MIN			(2)			/**< Minimum setting of AFGVW	*/
#define	D_IM_PRO_AF_AFGVW_MAX			(16382)		/**< Maximum setting of AFGVW	*/
#define	D_IM_PRO_AF_AFGHW_MIN			(2)			/**< Minimum setting of AFGHW	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	D_IM_PRO_AF_AFGHW_MAX			(2496)		/**< Maximum setting of AFGHW	*/
#define	D_IM_PRO_AF_AFVSH_MAX			(2496)		/**< Maximum setting of AFVSH	*/
#define	D_IM_PRO_AF_AFVSHW_MAX			(312)		/**< Maximum setting of AFVSHW	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define	D_IM_PRO_AF_AFGHW_MAX			(2900)		/**< Maximum setting of AFGHW	*/
#define	D_IM_PRO_AF_AFVSH_MAX			(2900)		/**< Maximum setting of AFVSH	*/
#define	D_IM_PRO_AF_AFVSHW_MAX			(362)		/**< Maximum setting of AFVSHW	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	D_IM_PRO_AF_AFVSH_MIN			(0)			/**< Minimum setting of AFVSH	*/
#define	D_IM_PRO_AF_AFVSHW_MIN			(2)			/**< Minimum setting of AFVSHW	*/
#define	D_IM_PRO_AF_AFV_MIN				(0)			/**< Minimum setting of AFV		*/
#define	D_IM_PRO_AF_AFV_MAX				(16380)		/**< Maximum setting of AFV		*/
#define	D_IM_PRO_AF_AFH_MIN				(0)			/**< Minimum setting of AFH		*/
#define	D_IM_PRO_AF_AFH_MAX				(8190)		/**< Maximum setting of AFH		*/
#define	D_IM_PRO_AF_AFVW_MIN			(2)			/**< Minimum setting of AFVW	*/
#define	D_IM_PRO_AF_AFVW_MAX			(4094)		/**< Maximum setting of AFVW	*/
#define	D_IM_PRO_AF_AFHW_MIN			(2)			/**< Minimum setting of AFHW	*/
#define	D_IM_PRO_AF_AFHW_MAX			(4094)		/**< Maximum setting of AFHW	*/
/*	@}*/	// im_pro_stat_af

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
//Common

/** @addtogroup im_pro_sen_top SENTOP
@{
*/

/** Sensor connect type
*/
typedef enum {
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS = 0,		/**< Connect to LVDS	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS,			/**< Connect to SLVS-EC	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI,		/**< Connect to MIPI D-PHY	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_CMIPI,		/**< Connect to MIPI C-PHY	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_SLVS,		/**< Connect to other SENCORE's SLVS-EC for Multi stream	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_DMIPI_VC,	/**< Connect to other SENCORE's MIPI D-PHY for Virtual channel	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_CMIPI_VC,	/**< Connect to other SENCORE's MIPI C-PHY for Virtual channel	*/
	E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_LVDS,		/**< Connect to other SENCORE's LVDS	*/
} E_IM_PRO_SENTOP_CONN_TYPE_SEL;


/** Macro block selection of input to BLOB.
*/
typedef enum {
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0 = 0,	/**< LDIV0 of output0 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1,		/**< LDIV0 of output1 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_2,		/**< LDIV0 of output2 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_3,		/**< LDIV0 of output3 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0,		/**< LDIV1 of output0 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_1,		/**< LDIV1 of output1 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0,		/**< LDIV2 of output0 connect to BLOB	*/
	E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0,		/**< LDIV3 of output0 connect to BLOB	*/
} E_IM_PRO_SENTOP_INPUT_TO_BLOB;

/** Output type for P2M macro.
*/
typedef enum {
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_PRE_BLOB = 0,		/**< LDIV0 of output0 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB,		/**< LDIV0 of output0 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_PRE_BLOB,			/**< LDIV0 of output1 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB,		/**< LDIV0 of output1 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_2_PRE_BLOB,			/**< LDIV0 of output2 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_2_POST_BLOB,		/**< LDIV0 of output2 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_3_PRE_BLOB,			/**< LDIV0 of output3 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_3_POST_BLOB,		/**< LDIV0 of output3 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_PRE_BLOB,			/**< LDIV1 of output0 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB,		/**< LDIV1 of output0 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_PRE_BLOB,			/**< LDIV1 of output1 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_POST_BLOB,		/**< LDIV1 of output1 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV2_0_PRE_BLOB,			/**< LDIV2 of output0 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV2_0_POST_BLOB,		/**< LDIV2 of output0 (post-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV3_0_PRE_BLOB,			/**< LDIV3 of output0 (pre-BLOB)	*/
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV3_0_POST_BLOB,		/**< LDIV3 of output0 (post-BLOB)	*/
} E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE;

/** Output selection to SRO2 I/F
*/
typedef enum {
	E_IM_PRO_SENTOP_OUT_SEL_LDIV0 = 0,				/**< Output from LDIV0	*/
	E_IM_PRO_SENTOP_OUT_SEL_LDIV1,					/**< Output from LDIV1	*/
} E_IM_PRO_SENTOP_OUT_SEL;

/** SENCORE maximum horizontal size of RAM
*/
typedef enum {
	E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_0 = 0,	/**< SENCORE0:2960  SENCORE1:2960        SENCORE2:2960        SENCORE3:2960	*/
	E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1,		/**< SENCORE0:5920  SENCORE1:5920        SENCORE2:Unavailable SENCORE3:1024	*/
	E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_2,		/**< SENCORE0:11840 SENCORE1:Unavailable SENCORE2:Unavailable SENCORE3:1024	*/
	E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_3,		/**< Setting prohibited(Reserved)	*/
} E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL;

/** LDIV maximum horizontal size of RAM
*/
typedef enum {
	E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_0 = 0,		/**< LDIV0:2960  LDIV1:2960        LDIV2:2960        LDIV3:2960	*/
	E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1,			/**< LDIV0:5920  LDIV1:5920        LDIV2:Unavailable LDIV3:1024	*/
	E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_2,			/**< LDIV0:11840 LDIV1:Unavailable LDIV2:Unavailable LDIV3:1024	*/
	E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_3,			/**< Setting prohibited(Reserved)	*/
} E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL;

/** LDIV multi channel input control 
*/
typedef enum {
	E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL_DISABLE = 0,	/**< Separated operation	*/
	E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL_2CH,			/**< 2ch multi connect operation	*/
	E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL_4CH,			/**< 4ch multi connect operation	*/
} E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL;


/** SENTOP Clock Type
*/
typedef enum {
	E_IM_PRO_SENTOP_CLK_TYPE_SENSORCLK = 0,		/**< SENSORCLK		*/
	E_IM_PRO_SENTOP_CLK_TYPE_APCLK0,			/**< APCLK_0(SG0)	*/
	E_IM_PRO_SENTOP_CLK_TYPE_APCLK1,			/**< APCLK_1(SG1)	*/
	E_IM_PRO_SENTOP_CLK_TYPE_APCLK2,			/**< APCLK_2(SG2)	*/
	E_IM_PRO_SENTOP_CLK_TYPE_APCLK3,			/**< APCLK_3(SG3)	*/
} E_IM_PRO_SENTOP_CLK_TYPE;

/** BitShift macro shift value
*/
typedef enum {
	E_IM_PRO_SENTOP_BS_SHIFT_0BIT	= 0,		/**< 0bit shift	*/
	E_IM_PRO_SENTOP_BS_SHIFT_1BIT,				/**< 1bit shift	*/
	E_IM_PRO_SENTOP_BS_SHIFT_2BIT,				/**< 2bit shift	*/
	E_IM_PRO_SENTOP_BS_SHIFT_3BIT,				/**< 3bit shift	*/
	E_IM_PRO_SENTOP_BS_SHIFT_4BIT,				/**< 4bit shift	*/
	E_IM_PRO_SENTOP_BS_SHIFT_5BIT,				/**< 5bit shift	*/
	E_IM_PRO_SENTOP_BS_SHIFT_6BIT,				/**< 6bit shift	*/
} E_IM_PRO_SENTOP_BS_SHIFT;

/** Saturation compensation when bit shift
*/
typedef enum {
	E_IM_PRO_SENTOP_BS_SAT_NOCOMP	= 0,		/**< saturation no compensation	*/
	E_IM_PRO_SENTOP_BS_SAT_COMP					/**< saturation compensation	*/
} E_IM_PRO_SENTOP_BS_SAT;

/** Bit shift direction
*/
typedef enum {
	E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR_L	= 0,	/**< Left bit shift	*/
	E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR_R			/**< Right bit shift	*/
} E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR;

/** SG I/F output select contrpl
*/
typedef enum {
	E_IM_PRO_SENTOP_SG_IF_OUTPUT_SEL_SG0 = 0,	/**< SG macro output 0 connected	*/
	E_IM_PRO_SENTOP_SG_IF_OUTPUT_SEL_SG1,		/**< SG macro output 1 connected	*/
	E_IM_PRO_SENTOP_SG_IF_OUTPUT_SEL_SG2,		/**< SG macro output 2 connected	*/
	E_IM_PRO_SENTOP_SG_IF_OUTPUT_SEL_SG3,		/**< SG macro output 3 connected	*/
} E_IM_PRO_SENTOP_SG_IF_OUTPUT_SEL;

/** OBT communication parttern
*/
typedef enum {
	E_IM_PRO_SENTOP_OBT_COMM_PARTTERN_ANY_2SETS = 0,	/**< Communication between any 2sets	*/
	E_IM_PRO_SENTOP_OBT_COMM_PARTTERN_ANY_3SETS,		/**< Communication between any 3sets	*/
	E_IM_PRO_SENTOP_OBT_COMM_PARTTERN_4SETS,			/**< Communication between 4sets	*/
	E_IM_PRO_SENTOP_OBT_COMM_PARTTERN_BLOB0_1_BLOB2_3,	/**< Communication between BLOB0 and BLOB1, and BLOB2 and BLOB3	*/
} E_IM_PRO_SENTOP_OBT_COMM_PARTTERN;


/** SLVS Link connect control
*/
typedef enum {
	E_IM_PRO_SENTOP_SLVS_LINK_CONN_PHY0 = 0,			/**< Connect PHY0's 20 divided clock to LINK.	*/
	E_IM_PRO_SENTOP_SLVS_LINK_CONN_PHY1,				/**< Connect PHY1's 20 divided clock to LINK.	*/
} E_IM_PRO_SENTOP_SLVS_LINK_CONN;

/*	@}*/	// im_pro_sen_top


/** @addtogroup im_pro_sro_top SROTOP
@{
*/

/** SROTOP Clock Type
*/
typedef enum {
	E_IM_PRO_SROTOP_CLK_TYPE_SROCLK = 0,		/**< SROCLK		*/
	E_IM_PRO_SROTOP_CLK_TYPE_SROCLK2,			/**< SROCLK2	*/
} E_IM_PRO_SROTOP_CLK_TYPE;

/** mode of SRO unit.
*/
typedef enum {
	E_IM_PRO_SROTOP_MODE0 = 0,		/**< FF0/1: valid   FSHDL0: valid   FSHDL1: invalid	*/
	E_IM_PRO_SROTOP_MODE1,			/**< FF0/1: valid   FSHDL0: invalid FSHDL1: valid	*/
	E_IM_PRO_SROTOP_MODE2,			/**< FF0/1: invalid FSHDL0: valid   FSHDL1: valid	*/
} E_IM_PRO_SROTOP_MODE;

/** select in which unit FFC/CAG macro are used, SRO or B2B.
*/
typedef enum {
	E_IM_PRO_SROTOP_CAG_TARGET_BLOCK_SRO = 0,	/**< FFC/CAG macro are used in SRO unit	*/
	E_IM_PRO_SROTOP_CAG_TARGET_BLOCK_B2B,		/**< FFC/CAG macro are used in B2B unit */
} E_IM_PRO_SROTOP_CAG_TARGET_BLOCK;

/** select a path for P2M0/1 and PWCH0/1 of SRO unit.
*/
typedef enum {
	E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_WITHOUT_BR = 0,	/**< Path without BayerResize	*/
	E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_WITH_BR,			/**< Path with BayerResize */
	E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_PG_OUT,			/**< Outputs of PG is inputted */
	E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_CAG_OUT,			/**< Outputs of CAG is inputted */
} E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT;

/** select a path for B2B direct connection I/F.
*/
typedef enum {
	E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN_WITHOUT_BR = 0,	/**< Path without BayerResize	*/
	E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN_WITH_BR,			/**< Path with BayerResize */
} E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN;

/** select a path for LTM direct connection I/F.
*/
typedef enum {
	E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN_WITHOUT_BR = 0,	/**< Path without BayerResize	*/
	E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN_WITH_BR,			/**< Path with BayerResize */
} E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN;

/** input mode of SRO unit.
*/
typedef enum {
	E_IM_PRO_SROTOP_INPUT_MODE_DIRECT = 0,	/**< SEN direct connection mode	*/
	E_IM_PRO_SROTOP_INPUT_MODE_REINPUT,		/**< Bayer data re-input mode	*/
} E_IM_PRO_SROTOP_INPUT_MODE;

/** WEITGEN channel number.
*/
typedef enum {
	E_IM_PRO_WEITGEN_CH0 = 0,				/**< ch0		*/
	E_IM_PRO_WEITGEN_CH1,					/**< ch1		*/
} E_IM_PRO_WEITGEN_CH;

/** WEITGEN Enable/Disable
*/
typedef enum {
	E_IM_PRO_WEITGEN_DIS = 0,				/**< Disable	*/
	E_IM_PRO_WEITGEN_EN						/**< Enable		*/
} E_IM_PRO_WEITGEN_ENABLE;

/*	@}*/	// im_pro_sro_top

/** @addtogroup im_pro_b2b_top B2BTOP
@{
*/

/** B2BTOP Clock Type
*/
typedef enum {
	E_IM_PRO_B2BTOP_CLK_TYPE_B2BCLK = 0,		/**< B2BCLK		*/
} E_IM_PRO_B2BTOP_CLK_TYPE;

/** input mode of SRO unit.
*/
typedef enum {
	E_IM_PRO_B2BTOP_INPUT_MODE_DIRECT = 0,	/**< SRO direct connection mode	*/
	E_IM_PRO_B2BTOP_INPUT_MODE_REINPUT,		/**< Bayer data re-input mode	*/
	E_IM_PRO_B2BTOP_INPUT_MODE_MAX			/**< MAX value	*/
} E_IM_PRO_B2BTOP_INPUT_MODE;

/** select in which unit FFC/CAG macro are used, SRO or B2B.
*/
typedef enum {
	E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK_SRO = 0,	/**< FFC/CAG macro are used in SRO unit	*/
	E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK_B2B,		/**< FFC/CAG macro are used in B2B unit */
	E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK_MAX		/**< MAX value	*/
} E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK;


/*	@}*/	// im_pro_b2b_top

/** @addtogroup im_pro_stat_top STATTOP
@{
*/

/** STATTOP Clock Type
*/
typedef enum {
	E_IM_PRO_STATTOP_CLK_TYPE_STATCLK = 0,		/**< STATCLK		*/
} E_IM_PRO_STATTOP_CLK_TYPE;

/** select the input to each AF/AEAWB macro.
*/
typedef enum {
	E_IM_PRO_STATTOP_INPUT_SEL_SRO1_PZSFTC2 = 0,		/**< PZSFTC2 output of SRO unit1	*/
	E_IM_PRO_STATTOP_INPUT_SEL_SRO2_PZSFTC2,			/**< PZSFTC2 output of SRO unit2	*/
	E_IM_PRO_STATTOP_INPUT_SEL_B2B1_PGAIN2,				/**< PGAIN2 output of B2B unit1		*/
	E_IM_PRO_STATTOP_INPUT_SEL_B2B2_PGAIN2,				/**< PGAIN2 output of B2B unit2		*/
	E_IM_PRO_STATTOP_INPUT_SEL_SEN_STAT_IF1,			/**< Output1 for STAT of SEN		*/
	E_IM_PRO_STATTOP_INPUT_SEL_SEN_STAT_IF2,			/**< Output2 for STAT of SEN		*/
	E_IM_PRO_STATTOP_INPUT_SEL_SEN_STAT_IF3,			/**< Output3 for STAT of SEN		*/
	E_IM_PRO_STATTOP_INPUT_SEL_SEN_STAT_IF4,			/**< Output4 for STAT of SEN		*/
} E_IM_PRO_STATTOP_INPUT_SEL;

/** select the input to each FLCK macro.
*/
typedef enum {
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SRO1_PZSFTC2 = 0,	/**< PZSFTC2 output of SRO unit1	*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SRO2_PZSFTC2,		/**< PZSFTC2 output of SRO unit2	*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_B2B1_PGAIN2,		/**< PGAIN2 output of B2B unit1		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_B2B2_PGAIN2,		/**< PGAIN2 output of B2B unit2		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SEN_STAT_IF1,		/**< Output1 for STAT of SEN		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SEN_STAT_IF2,		/**< Output2 for STAT of SEN		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SEN_STAT_IF3,		/**< Output3 for STAT of SEN		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SEN_STAT_IF4,		/**< Output4 for STAT of SEN		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SRO1_FSHDL0,		/**< FSHDL0 output of SRO unit1		*/
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL_SRO2_FSHDL0,		/**< FSHDL0 output of SRO unit2		*/
} E_IM_PRO_STATTOP_FLCK_INPUT_SEL;

/*	@}*/	// im_pro_stat_top

/** @addtogroup im_pro_pas_top PASTOP
@{
*/

/** PASTOP Clock Type
*/
typedef enum {
	E_IM_PRO_PASTOP_CLK_TYPE_PASCLK = 0,		/**< PASCLK		*/
} E_IM_PRO_PASTOP_CLK_TYPE;

/** Select L or R pixel side of PD in frame.
*/
typedef enum {
	E_IM_PRO_PASTOP_ORG_L_PDAF = 0,		/**< PDAF-L pixel	*/
	E_IM_PRO_PASTOP_ORG_R_PDAF,			/**< PDAF-L pixel	*/
} E_IM_PRO_PASTOP_ORG;

/*	@}*/	// im_pro_pas_top

/** @addtogroup im_pro_sen_slvs SLVS-EC
@{
*/

/** SLVS stream type.
*/
typedef enum {
	E_IM_PRO_SLVS_STREAM_TYPE_A = 0,		/**< Stream A		*/
	E_IM_PRO_SLVS_STREAM_TYPE_B,			/**< Stream B		*/
	E_IM_PRO_SLVS_STREAM_TYPE_BOTH,			/**< Stream A & B	*/
} E_IM_PRO_SLVS_STREAM_TYPE;

/** SLVS input differential data value inversion setting.
*/
typedef enum {
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION_NORMAL = 0,			/**< Normal	*/
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION_INVERT				/**< Invert	*/
} E_IM_PRO_SLVS_INPUT_DATA_INVERSION;

/** SLVS Multiplication factor control.
*/
typedef enum {
	E_IM_PRO_SLVS_MULTIPLICATION_FACTOR_REF_CLK_72MHZ = 0,	/**< Reference Clock 72MHz Setting for (Data Rate 2.304Gbps)	*/
	E_IM_PRO_SLVS_MULTIPLICATION_FACTOR_RESERVED			/**< Reserved	*/
} E_IM_PRO_SLVS_MULTIPLICATION_FACTOR;

/** SLVS trimming Enable/Disable
*/
typedef enum {
	E_IM_PRO_SLVS_TRIM_DIS = 0,			/**< Disable	*/
	E_IM_PRO_SLVS_TRIM_EN				/**< Enable		*/
} E_IM_PRO_SLVS_TRIM_ENABLE;

/** SLVS Power Down State select
*/
typedef enum {
	E_IM_PRO_SLVS_PD_STATE_POWER_DOWN = 0,			/**< Power Down state	*/
	E_IM_PRO_SLVS_PD_STATE_NORMAL					/**< power is being supplied state.	*/
} E_IM_PRO_SLVS_PD_STATE;

/** SLVS number of bits per pixel
*/
typedef enum {
	E_IM_PRO_SLVS_PIXEL_BIT_8BIT = 0,	/**< 8bit	*/
	E_IM_PRO_SLVS_PIXEL_BIT_10BIT,		/**< 10bit	*/
	E_IM_PRO_SLVS_PIXEL_BIT_12BIT,		/**< 12bit	*/
	E_IM_PRO_SLVS_PIXEL_BIT_14BIT,		/**< 14bit	*/
	E_IM_PRO_SLVS_PIXEL_BIT_16BIT,		/**< 16bit	*/
	E_IM_PRO_SLVS_PIXEL_BIT_MAX			/**< number of bits per pixel Max value	*/
} E_IM_PRO_SLVS_PIXEL_BIT;

/** SLVS number of valid Lanes
*/
typedef enum {
	E_IM_PRO_SLVS_LANE_NUM_1LANE = 0,	/**< 1Lane	*/
	E_IM_PRO_SLVS_LANE_NUM_2LANE,		/**< 2Lane	*/
	E_IM_PRO_SLVS_LANE_NUM_4LANE,		/**< 4Lane	*/
	E_IM_PRO_SLVS_LANE_NUM_6LANE,		/**< 6Lane	*/
	E_IM_PRO_SLVS_LANE_NUM_8LANE,		/**< 8Lane	*/
	E_IM_PRO_SLVS_LANE_NUM_MAX			/**< number of valid Lanes Max value	*/
} E_IM_PRO_SLVS_LANE_NUM;

/** SLVS output format of the image data interface.
*/
typedef enum {
	E_IM_PRO_SLVS_OUT_FORMAT_1POINT = 0,	/**< 1-point output	*/
	E_IM_PRO_SLVS_OUT_FORMAT_2POINT,		/**< 2-point output	*/
	E_IM_PRO_SLVS_OUT_FORMAT_4POINT,		/**< 4-point output	*/
	E_IM_PRO_SLVS_OUT_FORMAT_8POINT,		/**< 8-point output	*/
	E_IM_PRO_SLVS_OUT_FORMAT_MAX			/**< output format Max value	*/
} E_IM_PRO_SLVS_OUT_FORMAT;

/** SLVS input port select for Lane swap.
*/
typedef enum {
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_0 = 0,	/**< L0IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_1,		/**< L1IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_2,		/**< L2IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_3,		/**< L3IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_4,		/**< L4IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_5,		/**< L5IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_6,		/**< L6IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_7,		/**< L7IN	*/
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_MAX		/**< input port select Max value	*/
} E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL;

/** SLVS Lane Enable/Disable
*/
typedef enum {
	E_IM_PRO_SLVS_LANE_ENABLE_DIS = 0,			/**< Disable	*/
	E_IM_PRO_SLVS_LANE_ENABLE_EN				/**< Enable		*/
} E_IM_PRO_SLVS_LANE_ENABLE;

/** SLVS VDOUT control when frame stop is set
*/
typedef enum {
	E_IM_PRO_SLVS_VDOUT_CTRL_NO_OUTPUT = 0,			/**< Next VDOUT after frame stop is set is not output.	*/
	E_IM_PRO_SLVS_VDOUT_CTRL_OUTPUT,				/**< Next VDOUT after frame stop is set is output.	*/
} E_IM_PRO_SLVS_VDOUT_CTRL;

/** SLVS ECC option / CRC Option of payload.
*/
typedef enum {
	E_IM_PRO_SLVS_ECCCRC_OPT_WO_ECC_CRC = 0,		/**< Without ECC/CRC.	*/
	E_IM_PRO_SLVS_ECCCRC_OPT_W_CRC,					/**< With CRC.	*/
	E_IM_PRO_SLVS_ECCCRC_OPT_W_ECC_2BYTE_PARITY,	/**< With ECC(2byte parity).	*/
	E_IM_PRO_SLVS_ECCCRC_OPT_W_ECC_4BYTE_PARITY,	/**< With ECC(4byte parity).	*/
	E_IM_PRO_SLVS_ECCCRC_OPT_MAX,					/**< ECC/CRC option select Max value	*/
} E_IM_PRO_SLVS_ECCCRC_OPT;

/** SLVS expected value when Line Number Error of the line next to the state the Line Number extracted from the packet header reaches the MAX value (13’h1FFF) is detected.
*/
typedef enum {
	E_IM_PRO_SLVS_EXPECT_VAL_H_0000 = 0,			/**< expected value when Line Number Error occurred is 0x0000.	*/
	E_IM_PRO_SLVS_EXPECT_VAL_H_1FFF,				/**< expected value when Line Number Error occurred is 0x1FFF.	*/
} E_IM_PRO_SLVS_EXPECT_VAL;

/** SLVS symbol type
*/
typedef enum {
	E_IM_PRO_SLVS_SYMBOL_TYPE_DATA = 0,			/**< code type is Data.	*/
	E_IM_PRO_SLVS_SYMBOL_TYPE_K_CODE,			/**< code type is K Code.	*/
} E_IM_PRO_SLVS_SYMBOL_TYPE;

/** SLVS status of indicate whether Embedded Data is detected in the frame.
*/
typedef enum {
	E_IM_PRO_SLVS_STATUS_EBD_NO_EBD = 0,		/**< no Embedded Data.	*/
	E_IM_PRO_SLVS_STATUS_EBD_DETECTED,			/**< Embedded Data is detected.	*/
} E_IM_PRO_SLVS_STATUS_EBD;


/*	@}*/	// im_pro_sen_slvs

/** @addtogroup im_pro_sen_lvds LVDS
@{
*/

/** LVDS channel number.
*/
typedef enum {
	E_IM_PRO_LVDS_CH_0 = 0,						/**< ch0	*/
	E_IM_PRO_LVDS_CH_1,							/**< ch1	*/
	E_IM_PRO_LVDS_CH_2,							/**< ch1	*/
	E_IM_PRO_LVDS_CH_3,							/**< ch1	*/
} E_IM_PRO_LVDS_CH;

/** Sensor Mode
*/
typedef enum {
	E_IM_PRO_LVDS_INPUT_SERIAL = 0,		/**< 8lane/12bit */
	E_IM_PRO_LVDS_INPUT_PARALLEL		/**< 8lane/10bit */
} E_IM_PRO_LVDS_INPUT;

/** Channel number of sensor output
*/
typedef enum {
	E_IM_PRO_LVDS_OUT_1CH = 0,		/**< 1ch output sensor */
} E_IM_PRO_LVDS_OUT_CH;

/** LVDS Output data bit shift
*/
typedef enum {
	E_IM_PRO_LVDS_SFT_NONE = 0,			/**< bit shift none		*/
	E_IM_PRO_LVDS_SFT_1BIT_SHIFT_R,		/**< 1bit right shift	*/
	E_IM_PRO_LVDS_SFT_2BIT_SHIFT_R,		/**< 2bit right shift	*/
	E_IM_PRO_LVDS_SFT_3BIT_SHIFT_R,		/**< 3bit right shift	*/
	E_IM_PRO_LVDS_SFT_4BIT_SHIFT_R,		/**< 4bit right shift	*/
	E_IM_PRO_LVDS_SFT_5BIT_SHIFT_R,		/**< 5bit right shift	*/
	E_IM_PRO_LVDS_SFT_6BIT_SHIFT_R,		/**< 6bit right shift	*/
	E_IM_PRO_LVDS_SFT_7BIT_SHIFT_R,		/**< 7bit right shift	*/
	E_IM_PRO_LVDS_SFT_8BIT_SHIFT_L,		/**< 8bit left shift	*/
	E_IM_PRO_LVDS_SFT_7BIT_SHIFT_L,		/**< 7bit left shift	*/
	E_IM_PRO_LVDS_SFT_6BIT_SHIFT_L,		/**< 6bit left shift	*/
	E_IM_PRO_LVDS_SFT_5BIT_SHIFT_L,		/**< 5bit left shift	*/
	E_IM_PRO_LVDS_SFT_4BIT_SHIFT_L,		/**< 4bit left shift	*/
	E_IM_PRO_LVDS_SFT_3BIT_SHIFT_L,		/**< 3bit left shift	*/
	E_IM_PRO_LVDS_SFT_2BIT_SHIFT_L,		/**< 2bit left shift	*/
	E_IM_PRO_LVDS_SFT_1BIT_SHIFT_L		/**< 1bit left shift	*/
} E_IM_PRO_LVDS_SFT;

/** LVDS synchronous code detection type
*/
typedef enum {
	E_IM_PRO_LVDS_SYNC_TYPE_TYPE0 = 0, 	/**< Sync code type0(SAV/EAV/SAB/EAB)	*/
	E_IM_PRO_LVDS_SYNC_TYPE_TYPE1		/**< Sync code type1(SOF/SOL/EOF/EOL)	*/
} E_IM_PRO_LVDS_SYNC_TYPE;

/** LVDS Input size type
*/
typedef enum {
	E_IM_PRO_LVDS_INPUT_TYPE_SYNC_CODE = 0,		/**< A synchronous code is followed.	*/
	E_IM_PRO_LVDS_INPUT_TYPE_HSZIE				/**< LHSIZE/LHSADD is followed.	*/
} E_IM_PRO_LVDS_INPUT_TYPE;


/** LVDS use lane number
*/
typedef enum {
	E_IM_PRO_LVDS_LANE_1 = 1,			/**< 1lane	*/
	E_IM_PRO_LVDS_LANE_2,				/**< 2lane	*/
	E_IM_PRO_LVDS_LANE_3,				/**< 3lane	*/
	E_IM_PRO_LVDS_LANE_4,				/**< 4lane	*/
	E_IM_PRO_LVDS_LANE_5,				/**< 5lane	*/
	E_IM_PRO_LVDS_LANE_6,				/**< 6lane	*/
	E_IM_PRO_LVDS_LANE_7,				/**< 7lane	*/
	E_IM_PRO_LVDS_LANE_8,				/**< 8lane	*/
	E_IM_PRO_LVDS_LANE_10 = 10,			/**< 10lane	*/
	E_IM_PRO_LVDS_LANE_12 = 12,			/**< 12lane	*/
	E_IM_PRO_LVDS_LANE_14 = 14,			/**< 14lane	*/
	E_IM_PRO_LVDS_LANE_16 = 16			/**< 16lane	*/
} E_IM_PRO_LVDS_LANE;


/** LVDS output data bit width
*/
typedef enum {
	E_IM_PRO_LVDS_BIT_WIDTH_RAW_16_BIT = 0,	/**< RAW 16bit	*/
	E_IM_PRO_LVDS_BIT_WIDTH_RAW_14_BIT,		/**< RAW 14bit	*/
	E_IM_PRO_LVDS_BIT_WIDTH_RAW_12_BIT,		/**< RAW 12bit	*/
	E_IM_PRO_LVDS_BIT_WIDTH_RAW_10_BIT		/**< RAW 10bit	*/
} E_IM_PRO_LVDS_BIT_WIDTH;


/** LVDS output data bit width
*/
typedef enum {
	E_IM_PRO_LVDS_SYNC_BIT_SAME_LDW = 0,	/**< same as LDW	*/
	E_IM_PRO_LVDS_SYNC_BIT_16BIT_FIX		/**< 16bit fix		*/
} E_IM_PRO_LVDS_SYNC_BIT;

/** LVDS Division output
*/
typedef enum {
	E_IM_PRO_LVDS_DIV_OUT_2 = 1,	/**< 2-point division output		*/
	E_IM_PRO_LVDS_DIV_OUT_4,		/**< 4-point division output		*/
	E_IM_PRO_LVDS_DIV_OUT_8,		/**< 8-point division output		*/
} E_IM_PRO_LVDS_DIV_OUT;

/** LVDS Inverted output Enable/Disable
*/
typedef enum {
	E_IM_PRO_LVDS_INVERTED_DIS = 0,			/**< Disable	*/
	E_IM_PRO_LVDS_INVERTED_EN				/**< Enable		*/
} E_IM_PRO_LVDS_INVERTED;

/** LVDS output mode.
*/
typedef enum {
	E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL = 0,		/**< Normal output mode.<br>
														synchronous code detection is performed using only synchronous code0 (SAV[0]/EAV[0]/SAB[0]/EAB[0]),<br>
														the detected data is output from LVDS output0.	*/
	E_IM_PRO_LVDS_OUTPUT_MODE_DOL1,				/**< DOL output mode 1.<br>
														synchronous code detection is performed using synchronous code0(SAV[0]/EAV[0]/SAB[0]/EAB[0]) and code1(SAV[1]/EAV[1]/SAB[1]/EAB[1]),<br>
														the data of the line detected by synchronous code0 is output from LVDS output0,<br>
														and the data of the line detected by synchronous code1 is output from LVDS output1.	*/
	E_IM_PRO_LVDS_OUTPUT_MODE_DOL2,				/**< DOL output mode 2. synchronous code detection is performed using 3 groups of synchronous code:<br>
														code0(SAV[0]/EAV[0]/SAB[0]/EAB[0]), code1(SAV[1]/EAV[1]/SAB[1]/EAB[1]) and code2(SAV[2]/EAV[2]/SAB[2]/EAB[2]),<br>
														the data of the line detected by synchronous code0 is output from LVDS output0,<br>
														the data of the line detected by synchronous code1 is output from LVDS output1,<br>
														and the data of the line detected by synchronous code2 is output from LVDS output2.	*/
	E_IM_PRO_LVDS_OUTPUT_MODE_MAX,				/**< MAX value	*/
} E_IM_PRO_LVDS_OUTPUT_MODE;

/** LVDS select serial clock
*/
typedef enum {
	E_IM_PRO_LVDS_SNCLK_SEL_STOP = 0,		/**< clock stop	*/
	E_IM_PRO_LVDS_SNCLK_SEL_SNCLK0,			/**< SNCLK0		*/
	E_IM_PRO_LVDS_SNCLK_SEL_SNCLK1,			/**< SNCLK1		*/
	E_IM_PRO_LVDS_SNCLK_SEL_SNCLK2,			/**< SNCLK2		*/
	E_IM_PRO_LVDS_SNCLK_SEL_SNCLK3			/**< SNCLK3		*/
} E_IM_PRO_LVDS_SNCLK_SEL;


/** LVDS select serial data
*/
typedef enum {
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA0 = 0,	/**< SNDATA0	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA1,		/**< SNDATA1	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA2,		/**< SNDATA2	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA3,		/**< SNDATA3	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA4,		/**< SNDATA4	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA5,		/**< SNDATA5	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA6,		/**< SNDATA6	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA7,		/**< SNDATA7	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA8,		/**< SNDATA8	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA9,		/**< SNDATA9	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA10,		/**< SNDATA10	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA11,		/**< SNDATA11	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA12,		/**< SNDATA12	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA13,		/**< SNDATA13	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA14,		/**< SNDATA14	*/
	E_IM_PRO_LVDS_SNDATA_SEL_SNDATA15		/**< SNDATA15	*/
} E_IM_PRO_LVDS_SNDATA_SEL;

/** LVDS sync code lane select
*/
typedef enum {
	E_IM_PRO_LVDS_SYNC_SEL_SPX_D0 = 0,		/**< Enable select side D0 of SPx	*/
	E_IM_PRO_LVDS_SYNC_SEL_SPX_D1			/**< Enable select side D1 of SPx	*/
} E_IM_PRO_LVDS_SYNC_SEL;


/** LVDS sort output pixel selection
*/
typedef enum {
	E_IM_PRO_LVDS_PIXEL_POS_0 = 0,			/**< pixel position 0	*/
	E_IM_PRO_LVDS_PIXEL_POS_1,				/**< pixel position 1	*/
	E_IM_PRO_LVDS_PIXEL_POS_2,				/**< pixel position 2	*/
	E_IM_PRO_LVDS_PIXEL_POS_3,				/**< pixel position 3	*/
	E_IM_PRO_LVDS_PIXEL_POS_4,				/**< pixel position 4	*/
	E_IM_PRO_LVDS_PIXEL_POS_5,				/**< pixel position 5	*/
	E_IM_PRO_LVDS_PIXEL_POS_6,				/**< pixel position 6	*/
	E_IM_PRO_LVDS_PIXEL_POS_7				/**< pixel position 7	*/
} E_IM_PRO_LVDS_PIXEL_POS;

/*	@}*/	// im_pro_sen_lvds


/** @addtogroup im_pro_sen_dmipi	MIPI-DPHY
@{
*/

/** MIPI-DPHY channel number.
*/
typedef enum {
	E_IM_PRO_DMIPI_CH0 = 0,		/**< MIPI-DPHY0 */
	E_IM_PRO_DMIPI_CH1,			/**< MIPI-DPHY1 */
	E_IM_PRO_DMIPI_CH2,			/**< MIPI-DPHY2 */
	E_IM_PRO_DMIPI_CH3,			/**< MIPI-DPHY3 */
} E_IM_PRO_DMIPI_CH;


/** Number of input lanes selection.
*/
typedef enum {
	E_IM_PRO_DMIPI_LANE_1 = 0,	/**< 1-lane input */
	E_IM_PRO_DMIPI_LANE_2,		/**< 2-lane input */
	E_IM_PRO_DMIPI_LANE_4 = 3,	/**< 4-lane input */
} E_IM_PRO_DMIPI_LANE;

/** select the Virtual Channel value for the packets output to the OUT0/1 side.
*/
typedef enum {
	E_IM_PRO_DMIPI_VC_0 = 0,	/**< Virtual Channel0 */
	E_IM_PRO_DMIPI_VC_1,		/**< Virtual Channel1 */
	E_IM_PRO_DMIPI_VC_2,		/**< Virtual Channel2 */
	E_IM_PRO_DMIPI_VC_3,		/**< Virtual Channel3 */
} E_IM_PRO_DMIPI_VC;

/** select the input data mode.
*/
typedef enum {
	E_IM_PRO_DMIPI_IN_DATA_MODE_8BIT = 0,				/**< RAW 8-bit mode */
	E_IM_PRO_DMIPI_IN_DATA_MODE_10BIT,					/**< RAW 10-bit mode */
	E_IM_PRO_DMIPI_IN_DATA_MODE_12BIT,					/**< RAW 12-bit mode */
	E_IM_PRO_DMIPI_IN_DATA_MODE_14BIT,					/**< RAW 14-bit mode */
	E_IM_PRO_DMIPI_IN_DATA_MODE_LEGACY_YUV420,			/**< Legacy YUV420 8-bit  */
	E_IM_PRO_DMIPI_IN_DATA_MODE_YUV422,					/**< YUV422 8-bit */
	E_IM_PRO_DMIPI_IN_DATA_MODE_USER_DEF,				/**< User Defined/Embedded Data */
} E_IM_PRO_DMIPI_IN_DATA_MODE;

/** Output data mode selection.
*/
typedef enum {
	E_IM_PRO_DMIPI_OUT_MODE_NO_OUTPUT = 0,				/**< No output */
	E_IM_PRO_DMIPI_OUT_MODE_1LINE = 2,					/**< 1-line output mode */
	E_IM_PRO_DMIPI_OUT_MODE_2POINT,						/**< 2-point output mode */
	E_IM_PRO_DMIPI_OUT_MODE_4POINT,						/**< 4-point output mode */
	E_IM_PRO_DMIPI_OUT_MODE_8POINT,						/**< 8-point output mode */
} E_IM_PRO_DMIPI_OUT_MODE;

/** select the DataType mode for the packets output.
*/
typedef enum {
	E_IM_PRO_DMIPI_DT_OUT_DIS = 0,						/**< DT selection mode is off */
	E_IM_PRO_DMIPI_DT_OUT_EN,							/**< DT selection mode is on */
} E_IM_PRO_DMIPI_DT_OUT;

/** RxSyncMode value setting.
*/
typedef enum {
	E_IM_PRO_DMIPI_RXSYNCMODE_PERFECTLY_MATCHED = 0,	/**< Only perfectly-matched sync code is allowed. */
	E_IM_PRO_DMIPI_RXSYNCMODE_1BIT_DISCREPANCY,			/**< One bit of discrepancy between sync codes is allowed. */
} E_IM_PRO_DMIPI_RXSYNCMODE;

/** RxEnable value setting.
*/
typedef enum {
	E_IM_PRO_DMIPI_RXENABLE_ALL_OUT0 = 0,				/**< All RxEnable0~3 signals output "0". */
	E_IM_PRO_DMIPI_RXENABLE_OUT1,						/**< The corresponding RxEnable signal outputs "1". */
} E_IM_PRO_DMIPI_RXENABLE;

/**  select the cycle between horizontal detect pulse and output pixel data.
*/
typedef enum {
	E_IM_PRO_DMIPI_SEL_CYCLE_0 = 0,						/**< The cycle between HDP and pixel data is 0 cycle. */
	E_IM_PRO_DMIPI_SEL_CYCLE_2,							/**< The cycle between HDP and pixel data is 2 cycle. */
} E_IM_PRO_DMIPI_SEL_CYCLE;


/*	@}*/	// im_pro_sen_dmipi


/** MIPI-CPHY channel number.
*/
typedef enum {
	E_IM_PRO_CMIPI_CH0 = 0,		/**< MIPI-DPHY0 */
	E_IM_PRO_CMIPI_CH1,			/**< MIPI-DPHY1 */
	E_IM_PRO_CMIPI_CH2,			/**< MIPI-DPHY2 */
	E_IM_PRO_CMIPI_CH3,			/**< MIPI-DPHY3 */
} E_IM_PRO_CMIPI_CH;

/** Number of input lanes selection.
*/
typedef enum {
	E_IM_PRO_CMIPI_LANE_1 = 0,	/**< 1-lane input */
	E_IM_PRO_CMIPI_LANE_2,		/**< 2-lane input */
	E_IM_PRO_CMIPI_LANE_4 = 3,	/**< 4-lane input */
} E_IM_PRO_CMIPI_LANE;

/** select the Virtual Channel value for the packets output to the OUT0/1 side.
*/
typedef enum {
	E_IM_PRO_CMIPI_VC_0 = 0,	/**< Virtual Channel0 */
	E_IM_PRO_CMIPI_VC_1,		/**< Virtual Channel1 */
	E_IM_PRO_CMIPI_VC_2,		/**< Virtual Channel2 */
	E_IM_PRO_CMIPI_VC_3,		/**< Virtual Channel3 */
	E_IM_PRO_CMIPI_VC_INVALID,	/**< Virtual Channel invalid ( All output ) */
} E_IM_PRO_CMIPI_VC;

/** select the input data mode.
*/
typedef enum {
	E_IM_PRO_CMIPI_IN_DATA_MODE_8BIT = 0,				/**< RAW 8-bit mode */
	E_IM_PRO_CMIPI_IN_DATA_MODE_10BIT,					/**< RAW 10-bit mode */
	E_IM_PRO_CMIPI_IN_DATA_MODE_12BIT,					/**< RAW 12-bit mode */
	E_IM_PRO_CMIPI_IN_DATA_MODE_14BIT,					/**< RAW 14-bit mode */
	E_IM_PRO_CMIPI_IN_DATA_MODE_LEGACY_YUV420,			/**< Legacy YUV420 8-bit  */
	E_IM_PRO_CMIPI_IN_DATA_MODE_YUV422,					/**< YUV422 8-bit */
	E_IM_PRO_CMIPI_IN_DATA_MODE_USER_DEF,				/**< User Defined/Embedded Data */
} E_IM_PRO_CMIPI_IN_DATA_MODE;

/** Output data mode selection.
*/
typedef enum {
	E_IM_PRO_CMIPI_OUT_MODE_NO_OUTPUT = 0,				/**< No output */
	E_IM_PRO_CMIPI_OUT_MODE_1LINE = 2,					/**< 1-line output mode */
	E_IM_PRO_CMIPI_OUT_MODE_2POINT,						/**< 2-point output mode */
	E_IM_PRO_CMIPI_OUT_MODE_4POINT,						/**< 4-point output mode */
	E_IM_PRO_CMIPI_OUT_MODE_8POINT,						/**< 8-point output mode */
} E_IM_PRO_CMIPI_OUT_MODE;

/** select the DataType mode for the packets output.
*/
typedef enum {
	E_IM_PRO_CMIPI_DT_OUT_DIS = 0,						/**< DT selection mode is off */
	E_IM_PRO_CMIPI_DT_OUT_EN,							/**< DT selection mode is on */
} E_IM_PRO_CMIPI_DT_OUT;


/**  select the cycle between horizontal detect pulse and output pixel data.
*/
typedef enum {
	E_IM_PRO_CMIPI_SEL_CYCLE_0 = 0,						/**< The cycle between HDP and pixel data is 0 cycle. */
	E_IM_PRO_CMIPI_SEL_CYCLE_2,							/**< The cycle between HDP and pixel data is 2 cycle. */
} E_IM_PRO_CMIPI_SEL_CYCLE;

/** RxEnable value setting.
*/
typedef enum {
	E_IM_PRO_CMIPI_RXENABLE_ALL_OUT0 = 0,				/**< All RxEnable0~3 signals output "0". */
	E_IM_PRO_CMIPI_RXENABLE_OUT1,						/**< The corresponding RxEnable signal outputs "1". */
} E_IM_PRO_CMIPI_RXENABLE;



/** @addtogroup im_pro_sro_ffsh FFSH
@{
*/

/** FFSH channel number.
*/
typedef enum {
	E_IM_PRO_FFSH_CH0 = 0,		/**< FFSH0 */
	E_IM_PRO_FFSH_CH1			/**< FFSH1 */
} E_IM_PRO_FFSH_CH;


/*	@}*/	// im_pro_sro_ffsh


/** @addtogroup im_pro_sro_ff FF
@{
*/

/** FF channel number.
*/
typedef enum {
	E_IM_PRO_FF_CH0 = 0,	/**< FF0 */
	E_IM_PRO_FF_CH1			/**< FF1 */
} E_IM_PRO_FF_CH;

/*	@}*/	// im_pro_sro_ff


/** @addtogroup im_pro_sro_sdc SDC
@{
*/

/** SDC channel number.
*/
typedef enum {
	E_IM_PRO_SDC_CH0 = 0,		/**< SDC0 */
	E_IM_PRO_SDC_CH1			/**< SDC1 */
} E_IM_PRO_SDC_CH;


/** Defect correction mode select
*/
typedef enum {
	E_IM_PRO_SDC_MODE_SDC = 0,				/**< Static defect correction */
	E_IM_PRO_SDC_MODE_SDC_GAIN,				/**< Static defect correction + Gain correction  */
	E_IM_PRO_SDC_MODE_COOP_DPC_AFPB,		/**< DPC macro cooperative defect correction + Phase difference AF pixel output */
	E_IM_PRO_SDC_MODE_COOP_DPC_AFPB_GAIN1,	/**< DPC macro cooperative defect correction + Phase difference AF pixel output + Gain correction1 */
	E_IM_PRO_SDC_MODE_COOP_DPC_AFPB_GAIN2,	/**< DPC macro cooperative defect correction + Phase difference AF pixel output + Gain correction2 */
} E_IM_PRO_SDC_MODE;

/** Phase difference AF pixel pattern output setting.
*/
typedef enum {
	E_IM_PRO_SDC_AFPB_OUTPUT_PTN_ALL_OR_SDC_ONLY = 0,		/**< (Phase difference AF pixel pattern output + Static defect correction) processing or static defect correction only.	*/
	E_IM_PRO_SDC_AFPB_OUTPUT_PTN_AFOB_ONLY,					/**< Phase difference AF pixel pattern output only.  */
} E_IM_PRO_SDC_AFPB_OUTPUT_PTN;

/** Defect correction monochrome mode select.
*/
typedef enum {
	E_IM_PRO_SDC_MONOCHROME_MODE_BAYER_CORRECTION = 0,		/**< Static defect correction Bayer correction mode.	*/
	E_IM_PRO_SDC_MONOCHROME_MODE_MONOCHROME_CORRECTION,		/**< Static defect correction  monochrome correction mode.	*/
} E_IM_PRO_SDC_MONOCHROME_MODE;

/** Phase difference AF pixel pattern output enable.
*/
typedef enum {
	E_IM_PRO_SDC_AFPB_DIS = 0,								/**< Phase difference AF pixel pattern output is OFF.	*/
	E_IM_PRO_SDC_AFPB_EN,									/**< Phase difference AF pixel pattern output is ON.	*/
} E_IM_PRO_SDC_AFPB_ENABLE;

/** SDC Gain Table Select.
*/
typedef enum {
	E_IM_PRO_SDC_GAIN_TABLE_SEL_0 = 0,						/**< Gain coefficient table RAM0 (SDCGDTBL0) is selected.	*/
	E_IM_PRO_SDC_GAIN_TABLE_SEL_1,							/**< Gain coefficient table RAM1 (SDCGDTBL1) is selected.	*/
} E_IM_PRO_SDC_GAIN_TABLE_SEL;

/*	@}*/	// im_pro_sro_sdc


/** @addtogroup im_pro_sro_dpc DPC
@{
*/

/** Defect correction mode.<br>
	<table>
		<tr>
			<th>Mode</th>
			<th>Correction method of<br>dynamic detection pixel</th>
			<th>Correction method of<br>registration defective pixel</th>
			<th>Correction method of<br>registration defect and<br>dynamic detection pixel</th>
			<th>Recommended DPCRTZ setting</th>
		<tr>
			<td>0</td>
			<td>Correction OFF</td>
			<td>Correction OFF</td>
			<td>Correction OFF</td>
			<td>No effect</td></tr>

		<tr>
			<td>1</td>
			<td>alpha blend detect correction</td>
			<td>Correction OFF</td>
			<td>alpha blend detect correction</td>
			<td>0</td></tr>

		<tr>
			<td>2</td>
			<td>Correction OFF</td>
			<td>alpha blend detect correction</td>
			<td>alpha blend detect correction</td>
			<td>1</td></tr>

		<tr>
			<td>3</td>
			<td>alpha blend detect correction</td>
			<td>alpha blend detect correction</td>
			<td>alpha blend detect correction</td>
			<td>1</td></tr>

		<tr>
			<td>4</td>
			<td>alpha blend detect correction</td>
			<td>Edge storage defect correction</td>
			<td>Edge storage defect correction</td>
			<td>No effect</td></tr>

		<tr>
			<td>5</td>
			<td>alpha blend detect correction</td>
			<td>Edge storage defect correction</td>
			<td>alpha blend detect correction</td>
			<td>1</td></tr>
	</table>
*/
typedef enum {
	E_IM_PRO_DPC_MODE_0 = 0,		/**< Defect correction mode0.	*/
	E_IM_PRO_DPC_MODE_1,			/**< Defect correction mode1.	*/
	E_IM_PRO_DPC_MODE_2,			/**< Defect correction mode2.	*/
	E_IM_PRO_DPC_MODE_3,			/**< Defect correction mode3.	*/
	E_IM_PRO_DPC_MODE_4,			/**< Defect correction mode4.	*/
	E_IM_PRO_DPC_MODE_5,			/**< Defect correction mode5.	*/
} E_IM_PRO_DPC_MODE;

/** Dynamic defect determination threshold zero conversion for registration defective pixel.<br>
*/
typedef enum {
	E_IM_PRO_DPC_THRE_CONV_0_DIS = 0,	/**< Dynamic defect determination threshold is not converted to zero.	*/
	E_IM_PRO_DPC_THRE_CONV_0_EN,		/**< Dynamic defect determination threshold is converted to zero.	*/
} E_IM_PRO_DPC_THRE_CONV_TO_ZERO;


/** reference mode of Gr/Gb pixel in the defect dynamic detection and pixel value magnitude defect correction.
*/
typedef enum {
	E_IM_PRO_DPC_DD_GRGB_UNITED = 0,				/**< GrGb united.	*/
	E_IM_PRO_DPC_DD_GRGB_INDEPENDENT,				/**< GrGb independent.	*/
} E_IM_PRO_DPC_DD_GRGB;

/** dynamic detection mode of G pixel in the defect dynamic detection.
*/
typedef enum {
	E_IM_PRO_DPC_DD_MODE_G_SINGLE_DEFECT = 0,		/**< G pixel single defect support.	*/
	E_IM_PRO_DPC_DD_MODE_G_2_CONSECUTIVE_DEFECT,	/**< G pixel 2-consecutive defects support.	*/
} E_IM_PRO_DPC_DD_MODE_G;

/** dynamic detection mode of R/B pixel in the defect dynamic detection.
*/
typedef enum {
	E_IM_PRO_DPC_DD_MODE_RB_SINGLE_DEFECT = 0,		/**< R/B pixel single defect support.	*/
	E_IM_PRO_DPC_DD_MODE_RB_2_CONSECUTIVE_DEFECT,	/**< R/B pixel 2-consecutive defects support.	*/
} E_IM_PRO_DPC_DD_MODE_RB;

/** Use of mis-determination degree for correction.
*/
typedef enum {
	E_IM_PRO_DPC_DD_USE_MDD_DIS = 0,				/**< Mis-determination degree for correction is not used.	*/
	E_IM_PRO_DPC_DD_USE_MDD_EN,						/**< Mis-determination degree for correction is used.	*/
} E_IM_PRO_DPC_DD_USE_MDD;

/** Mis-determination degree for correction  horizontal and vertical 2-concecutive defect support.
*/
typedef enum {
	E_IM_PRO_DPC_DD_MDD_2CONSECUTIVE_DEFECT_DIS = 0,/**< When mis-determination degree for correction is used, horizontal and vertical 2-consecutive defect support is not included.	*/
	E_IM_PRO_DPC_DD_MDD_2CONSECUTIVE_DEFECT_EN,		/**< When mis-determination degree for correction is used, horizontal and vertical 2-consecutive defect support is included.	*/
} E_IM_PRO_DPC_DD_MDD_2CONSECUTIVE_DEFECT;

/** Use of over-correction degree for pixel value magnitude defect correction.
*/
typedef enum {
	E_IM_PRO_DPC_MDC_USE_OCD_DIS = 0,				/**< Over-correction degree for correction is not used in pixel value magnitude defect correction.	*/
	E_IM_PRO_DPC_MDC_USE_OCD_EN,					/**< Over-correction degree for correction is used in pixel value magnitude defect correction.	*/
} E_IM_PRO_DPC_MDC_USE_OCD;

/** reference mode of Gr/Gb pixel in the edge storage defect correction.<br>
	<table>
		<tr>
			<th>mode</th>
			<th>For correction means judgment</th>
			<th>For correction value calculation</th>
		<tr>
			<td>0</td>
			<td>GrGb united</td>
			<td>GrGb united</td>

		<tr>
			<td>1</td>
			<td>GrGb independent</td>
			<td>GrGb united</td>

		<tr>
			<td>2</td>
			<td>GrGb united</td>
			<td>GrGb independent</td>

		<tr>
			<td>3</td>
			<td>GrGb independent</td>
			<td>GrGb independent</td>

	</table>
*/
typedef enum {
	E_IM_PRO_DPC_ESD_MODE_0 = 0,		/**< Edge storage defect correction mode0.	*/
	E_IM_PRO_DPC_ESD_MODE_1,			/**< Edge storage defect correction mode1.	*/
	E_IM_PRO_DPC_ESD_MODE_2,			/**< Edge storage defect correction mode2.	*/
	E_IM_PRO_DPC_ESD_MODE_3,			/**< Edge storage defect correction mode3.	*/
} E_IM_PRO_DPC_ESD_MODE;

/** Use of G pixel in RB judgment of edge storage defect correction.
*/
typedef enum {
	E_IM_PRO_DPC_ESD_USE_RB_JUDGE_DIS = 0,				/**< G pixel is not used in RB judgment of edge storage defect correction.	*/
	E_IM_PRO_DPC_ESD_USE_RB_JUDGE_EN,					/**< G pixel is used in RB judgment of edge storage defect correction.	*/
} E_IM_PRO_DPC_ESD_USE_RB_JUDGE;

/** Use of G pixel in RB judgment of edge storage defect correction.
*/
typedef enum {
	E_IM_PRO_DPC_ESD_USE_OCD_DIS = 0,					/**< Over-correction degree for correction is not used in edge storage defect correction.	*/
	E_IM_PRO_DPC_ESD_USE_OCD_EN,						/**< Over-correction degree for correction is used in edge storage defect correction.	*/
} E_IM_PRO_DPC_ESD_USE_OCD;

/*	@}*/	// im_pro_sro_dpc


/** @addtogroup im_pro_sen_obt OBT
@{
*/

/** OBT channel number
*/
typedef enum {
	E_IM_PRO_OBT_CH_0_0 = 0,			/**< BLOB0 OBT ch0	*/
	E_IM_PRO_OBT_CH_0_1,				/**< BLOB0 OBT ch1	*/
	E_IM_PRO_OBT_CH_1_0,				/**< BLOB1 OBT ch0	*/
	E_IM_PRO_OBT_CH_1_1,				/**< BLOB1 OBT ch1	*/
	E_IM_PRO_OBT_CH_2_0,				/**< BLOB2 OBT ch0	*/
	E_IM_PRO_OBT_CH_2_1,				/**< BLOB2 OBT ch1	*/
	E_IM_PRO_OBT_CH_3_0,				/**< BLOB3 OBT ch0	*/
	E_IM_PRO_OBT_CH_3_1,				/**< BLOB3 OBT ch1	*/
} E_IM_PRO_OBT_CH;

/** PRO TOP OB Enable/Disable
*/
typedef enum {
	E_IM_PRO_OBT_DIS = 0,				/**< Disable	*/
	E_IM_PRO_OBT_EN						/**< Enable		*/
} E_IM_PRO_OBT_ENABLE;

/** PRO TOP OB statistics select
*/
typedef enum {
	E_IM_PRO_OBT_MODE_ALL = 0,			/**< TOP OB common all pixel mode	*/
	E_IM_PRO_OBT_MODE_COLOR				/**< TOP OB each color mode			*/
} E_IM_PRO_OBT_MODE;

/** PRO TOP OB Correction select
*/
typedef enum {
	E_IM_PRO_OBT_SELECT_FIX = 0,		/**< TOP OB correction fix value use		*/
	E_IM_PRO_OBT_SELECT_EVALUATION		/**< TOP OB correction evaluation value use	*/
} E_IM_PRO_OBT_SELECT;

/** PRO TOP OB Intercommunication Enable/Disable
*/
typedef enum {
	E_IM_PRO_OBT_INTERCOMM_DIS = 0,		/**< Inter-OBT communication is disabled.		*/
	E_IM_PRO_OBT_INTERCOMM_EN			/**< OBT communication is enabled.				*/
} E_IM_PRO_OBT_INTERCOMM_ENABLE;

/*	@}*/	// im_pro_sen_obt

/** @addtogroup im_pro_sen_obd OBD
@{
*/
/** OBD channel number
*/
typedef enum {
	E_IM_PRO_OBD_CH_0_0 = 0,			/**< BLOB0 OBD ch0	*/
	E_IM_PRO_OBD_CH_0_1,				/**< BLOB0 OBD ch1	*/
	E_IM_PRO_OBD_CH_1_0,				/**< BLOB1 OBD ch0	*/
	E_IM_PRO_OBD_CH_1_1,				/**< BLOB1 OBD ch1	*/
	E_IM_PRO_OBD_CH_2_0,				/**< BLOB2 OBD ch0	*/
	E_IM_PRO_OBD_CH_2_1,				/**< BLOB2 OBD ch1	*/
	E_IM_PRO_OBD_CH_3_0,				/**< BLOB3 OBD ch0	*/
	E_IM_PRO_OBD_CH_3_1,				/**< BLOB3 OBD ch1	*/
} E_IM_PRO_OBD_CH;

/*	@}*/	// im_pro_sen_obd

/** @addtogroup im_pro_b2b_fshd FSHD
@{
*/

/** FSHD Enable/Disable
*/
typedef enum {
	E_IM_PRO_FSHD_PAEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_FSHD_PAEN_EN			/**< Enable		*/
} E_IM_PRO_FSHD_PAEN;

/** Mode surface shading correction factor
*/
typedef enum {
	E_IM_PRO_FSHD_FRAME_ONLY = 0,					/**< Frame Only							*/
	E_IM_PRO_FSHD_CONCENTRIC_ONLY,					/**< Concentric Only					*/
	E_IM_PRO_FSHD_FRAME_CONCENTRIC_MULTIPLICATION,	/**< Frame Multiplication Concentric	*/
	E_IM_PRO_FSHD_FRAME_CONCENTRIC_ADDITION			/**< Frame Addition Concentric			*/
} E_IM_PRO_FSHD_SHADING_MODE;


/** Shading correction factor for the detection mode setting surface AWB
*/
typedef enum {
	E_IM_PRO_FSHD_FSMODE = 0,		/**< According to the setting of bit FSMODE	*/
	E_IM_PRO_FSHD_FRAME,			/**< Only use the correction factor frame	*/
} E_IM_PRO_FSHD_SHADING_MODE_AWB;

/** Frame shading montage image processing mode.
*/
typedef enum {
	E_IM_PRO_FSHD_MONTAGE_MODE_OFF = 0,		/**< Montage image processing mode is OFF.	*/
	E_IM_PRO_FSHD_MONTAGE_MODE_ON,			/**< Montage image processing mode is ON.	*/
} E_IM_PRO_FSHD_MONTAGE_MODE;

/** Saturation correction Enable/Disable
*/
typedef enum {
	E_IM_PRO_FSHD_SAT_CORRECTION_DIS = 0,	/**< Disable	*/
	E_IM_PRO_FSHD_SAT_CORRECTION_EN			/**< Enable		*/
} E_IM_PRO_FSHD_SAT_CORRECTION;



/** Concentric correction LUT selection
*/
typedef enum {
	E_IM_PRO_FSHD_LUT_SEL_RAM0 = 0,		/**< RAM0(FSHDCTBL0)	*/
	E_IM_PRO_FSHD_LUT_SEL_RAM1			/**< RAM1(FSHDCTBL1)	*/
} E_IM_PRO_FSHD_LUT_SEL;


/** Formatting correction factor concentric
*/
typedef enum {
	E_IM_PRO_FSHD_CORRECTION_FORMAT_0 = 0,	/**< XXXX.XXXX_XXXX_XXXX	*/
	E_IM_PRO_FSHD_CORRECTION_FORMAT_1		/**< 1+XXXX.XXXX_XXXX_XXXX	*/
} E_IM_PRO_FSHD_CORRECTION_FORMAT;
/*	@}*/

/** @addtogroup im_pro_stat_aeawb AEAWB
@{
*/

/** AEAWB channel number.
*/
typedef enum {
	E_IM_PRO_AEAWB_CH0 = 0,		/**< AEAWB ch0 */
	E_IM_PRO_AEAWB_CH1,			/**< AEAWB ch1 */
	E_IM_PRO_AEAWB_CH2,			/**< AEAWB ch2 */
	E_IM_PRO_AEAWB_CH3,			/**< AEAWB ch3 */
} E_IM_PRO_AEAWB_CH;

/** AEAWB Enable/Disable
*/
typedef enum {
	E_IM_PRO_AEAWB_PAEN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_AEAWB_PAEN_EN			/**< Enable		*/
} E_IM_PRO_AEAWB_PAEN;

/** AEAWB detecting mode
*/
typedef enum {
	E_IM_PRO_AEAWB_MODE_AE = 0,			/**< detecting target : AE						*/
	E_IM_PRO_AEAWB_MODE_AWB,			/**< detecting target : AWB						*/
	E_IM_PRO_AEAWB_MODE_AE_AWB,			/**< detecting target : AE / AWB				*/
	E_IM_PRO_AEAWB_MODE_LWB,			/**< detecting target : Level AWB				*/
	E_IM_PRO_AEAWB_MODE_AE_LWB,			/**< detecting target : AE / Level AWB			*/
	E_IM_PRO_AEAWB_MODE_AWB_LWB,		/**< detecting target : AWB / Level AWB			*/
	E_IM_PRO_AEAWB_MODE_AE_AWB_LWB,		/**< detecting target : AE / AWB / Level AWB	*/
} E_IM_PRO_AEAWB_MODE;

/** AEAWB detecting data type
*/
typedef enum {
	E_IM_PRO_AEAWB_TYPE_AE = 0,			/**< AE data		*/
	E_IM_PRO_AEAWB_TYPE_AWB,			/**< AWB data		*/
	E_IM_PRO_AEAWB_TYPE_LAWB,			/**< LAWB data		*/
} E_IM_PRO_AEAWB_TYPE;

/** AWB detecting mode
*/
typedef enum {
	E_IM_PRO_AWB_MODE_RGB_CNT = 0,		/**< 4 pixels of R pixel/G pixel(average)/B pixel/Passing count after passing through the level gate	*/
	E_IM_PRO_AWB_MODE_R_GR_GB_B			/**< 4 pixels of R pixel/GR pixel/B pixel/ GB pixel after passing through the level gate	*/
} E_IM_PRO_AWB_MODE;

/** AWB detecting window mode
*/
typedef enum {
	E_IM_PRO_AWB_WD_WINDOW_MODE_SAME = 0,	/**< The detection block horizontal/vertical sizes and the intra-block enable horizontal/vertical sizes are all in the same size.	*/
	E_IM_PRO_AWB_WD_WINDOW_MODE_SEPARATE 	/**< The individual setting of the detection block at top/bottom/left/right side is valid.	*/
} E_IM_PRO_AEAWB_WD_WINDOW_MODE;

/*	@}*/	// im_pro_stat_aeawb

/** @addtogroup im_pro_stat_af AF
@{
*/

/** AF channel number.
*/
typedef enum {
	E_IM_PRO_AF_CH0 = 0,		/**< AF ch0 */
	E_IM_PRO_AF_CH1,			/**< AF ch1 */
} E_IM_PRO_AF_CH;

/** AF Enable/Disable
*/
typedef enum {
	E_IM_PRO_AF_PAEN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_AF_PAEN_EN			/**< Enable		*/
} E_IM_PRO_AF_PAEN;

/** AF Window Number
*/
typedef enum {
	E_IM_PRO_AF_WINDOW_0 = 0,			/**< AF Window 0			*/
	E_IM_PRO_AF_WINDOW_1,				/**< AF Window 1			*/
	E_IM_PRO_AF_WINDOW_2,				/**< AF Window 2			*/
	E_IM_PRO_AF_WINDOW_3,				/**< AF Window 3			*/
	E_IM_PRO_AF_WINDOW_4,				/**< AF Window 4			*/
	E_IM_PRO_AF_WINDOW_5,				/**< AF Window 5			*/
	E_IM_PRO_AF_WINDOW_6,				/**< AF Window 6			*/
	E_IM_PRO_AF_WINDOW_7,				/**< AF Window 7			*/
	E_IM_PRO_AF_WINDOW_8,				/**< AF Window 8			*/
	E_IM_PRO_AF_WINDOW_9,				/**< AF Window 9			*/
	E_IM_PRO_AF_WINDOW_10,				/**< AF Window 10			*/
	E_IM_PRO_AF_WINDOW_11,				/**< AF Window 11			*/
	E_IM_PRO_AF_WINDOW_12,				/**< AF Window 12			*/
	E_IM_PRO_AF_WINDOW_13,				/**< AF Window 13			*/
	E_IM_PRO_AF_WINDOW_14,				/**< AF Window 14			*/
	E_IM_PRO_AF_WINDOW_MAX				/**< AF Window Max			*/
} E_IM_PRO_AF_WINDOW_NO;

/** AF Enable/Disable
*/
typedef enum {
	E_IM_PRO_AF_ENABLE_DIS = 0,			/**< Disable	*/
	E_IM_PRO_AF_ENABLE_EN				/**< Enable		*/
} E_IM_PRO_AF_ENABLE;

/** AF TAP number
*/
typedef enum {
	E_IM_PRO_AF_TAP_0 = 0,				/**< TAP 0						*/
	E_IM_PRO_AF_TAP_1,					/**< TAP 1						*/
	E_IM_PRO_AF_TAP_2					/**< TAP 2						*/
} E_IM_PRO_AF_TAP;

/** select a AF detection evaluated value calculation unit.
*/
typedef enum {
	E_IM_PRO_AF_EVAL_CALC_UNIT_0 = 0,		/**< AF detection evaluated value calculation unit 0.	*/
	E_IM_PRO_AF_EVAL_CALC_UNIT_1,			/**< AF detection evaluated value calculation unit 1.	*/
	E_IM_PRO_AF_EVAL_CALC_UNIT_2			/**< AF detection evaluated value calculation unit 2.	*/
} E_IM_PRO_AF_EVAL_CALC_UNIT;

/** AF Window horizon select
*/
typedef enum {
	E_IM_PRO_AF_WIN_H0_A = 0,			/**< horizon 0 group A output	*/
	E_IM_PRO_AF_WIN_H0_B,				/**< horizon 0 group B output	*/
	E_IM_PRO_AF_WIN_H1_A,				/**< horizon 1 group A output	*/
	E_IM_PRO_AF_WIN_H1_B,				/**< horizon 1 group B output	*/
	E_IM_PRO_AF_WIN_H2_A,				/**< horizon 2 group A output	*/
	E_IM_PRO_AF_WIN_H2_B				/**< horizon 2 group B output	*/
} E_IM_PRO_AF_WIN_HSEL;

/** select the input for the minimum/maximum value of the evaluation value.
*/
typedef enum {
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_H_UNIT0_A = 0,		/**< The output of AF horizontal detection processing unit 0 group A is selected.	*/
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_H_UNIT0_B,			/**< The output of AF horizontal detection processing unit 0 group B is selected.	*/
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_H_UNIT1_A,			/**< The output of AF horizontal detection processing unit 1 group A is selected.	*/
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_H_UNIT1_B,			/**< The output of AF horizontal detection processing unit 1 group B is selected.	*/
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_H_UNIT2_A,			/**< The output of AF horizontal detection processing unit 2 group A is selected.	*/
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_H_UNIT2_B,			/**< The output of AF horizontal detection processing unit 2 group B is selected.	*/
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL_V_UNIT = 9,			/**< The output of AF vertical detection processing unit is selected.	*/
} E_IM_PRO_AF_MIN_MAX_DETECT_SEL;

/*	@}*/	// im_pro_stat_af

/** @addtogroup im_pro_stat_flck FLCK
@{
*/

/** FLCK channel number.
*/
typedef enum {
	E_IM_PRO_FLCK_CH0 = 0,		/**< FLCK ch0 */
	E_IM_PRO_FLCK_CH1,			/**< FLCK ch1 */
} E_IM_PRO_FLCK_CH;

/** FLCK detecting mode
*/
typedef enum {
	E_IM_PRO_FLCK_MODE_NORMAL = 0,			/**< Normal detection									*/
	E_IM_PRO_FLCK_MODE_LINE,				/**< Even/odd processing line distinguish detection 	*/
} E_IM_PRO_FLCK_MODE;

/** FLCK output data type
*/
typedef enum {
	E_IM_PRO_FLCK_TYPE_RGB = 0,				/**< Passing count, R pixel, B pixel, G pixel (average).	*/
	E_IM_PRO_FLCK_TYPE_YG,					/**< Passing count, luminance, Gr pixel, Gb pixel.	*/
} E_IM_PRO_FLCK_TYPE;

/** Luminance generation coefficient type
*/
typedef enum {
	E_IM_PRO_FLCK_FLYK_TYPE_R = 0,			/**< Luminance generation RR coefficient	*/
	E_IM_PRO_FLCK_FLYK_TYPE_GR,				/**< Luminance generation GR coefficient	*/
	E_IM_PRO_FLCK_FLYK_TYPE_GB,				/**< Luminance generation GB coefficient	*/
	E_IM_PRO_FLCK_FLYK_TYPE_B				/**< Luminance generation BB coefficient	*/
} E_IM_PRO_FLCK_FLYK_TYPE;

/*	@}*/

/** @addtogroup im_pro_stat_hist HIST
@{
*/

/** HIST channel number.
*/
typedef enum {
	E_IM_PRO_HIST_CH0 = 0,		/**< HIST ch0 */
	E_IM_PRO_HIST_CH1,			/**< HIST ch1 */
	E_IM_PRO_HIST_CH2,			/**< HIST ch2 */
	E_IM_PRO_HIST_CH3,			/**< HIST ch3 */
} E_IM_PRO_HIST_CH;

/** HIST internal-RAM access Enable/Disable.
*/
typedef enum {
	E_IM_PRO_HIST_PAEN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_HIST_PAEN_EN		/**< Enable		*/
} E_IM_PRO_HIST_PAEN;

/** Histogram detection mode.
*/
typedef enum {
	E_IM_PRO_HIST_MODE_1_FRAME_DETECT = 0,		/**< One frame detection mode						*/
	E_IM_PRO_HIST_MODE_CONTINUOUS,				/**< Continuous frame cumulative detection mode 	*/
} E_IM_PRO_HIST_MODE;

/** HIST horizontal subsampling cycle.
*/
typedef enum {
	E_IM_PRO_HIST_CYCLE_NORMAL = 0,				/**< Detection without horizontal subsampling		*/
	E_IM_PRO_HIST_CYCLE_2PIXEL_SUBSAMPLING,		/**< Detection with horizontal 2-pixel subsampling	*/
	E_IM_PRO_HIST_CYCLE_4PIXEL_SUBSAMPLING,		/**< Detection with horizontal 4-pixel subsampling	*/
	E_IM_PRO_HIST_CYCLE_8PIXEL_SUBSAMPLING,		/**< Detection with horizontal 8-pixel subsampling	*/
} E_IM_PRO_HIST_CYCLE;

/** Color and luminance data shift amount for HIST sum calculation.
*/
typedef enum {
	E_IM_PRO_HIST_SHIFT_VAL_NO_SHIFT = 0,		/**< No shift			*/
	E_IM_PRO_HIST_SHIFT_VAL_1BIT_RIGHT,			/**< 1-bit right shift	*/
	E_IM_PRO_HIST_SHIFT_VAL_2BIT_RIGHT,			/**< 2-bit right shift	*/
	E_IM_PRO_HIST_SHIFT_VAL_3BIT_RIGHT,			/**< 3-bit right shift	*/
	E_IM_PRO_HIST_SHIFT_VAL_4BIT_RIGHT,			/**< 4-bit right shift	*/
} E_IM_PRO_HIST_SHIFT_VAL;

/*	@}*/	// im_pro_stat_hist

/** @addtogroup im_pro_pas_spt SPT
@{
*/

/** Select shading correction table.
*/
typedef enum {
	E_IM_PRO_SPT_TBL_SEL_SPTTBL0 = 0,			/**< RAM0(SPTTBL0)	*/
	E_IM_PRO_SPT_TBL_SEL_SPTTBL1,				/**< RAM1(SPTTBL1)	*/
} E_IM_PRO_SPT_TBL_SEL;

/*	@}*/	// im_pro_pas_spt

/** @addtogroup im_pro_common_p2m P2M
@{
*/

/** SDC channel number.
*/
typedef enum {
	E_IM_PRO_P2M_CH0 = 0,		/**< P2M0 */
	E_IM_PRO_P2M_CH1,			/**< P2M1 */
	E_IM_PRO_P2M_CH2,			/**< P2M2 */
	E_IM_PRO_P2M_CH3,			/**< P2M3 */
	E_IM_PRO_P2M_CH4,			/**< P2M4 */
	E_IM_PRO_P2M_CH5,			/**< P2M5 */
	E_IM_PRO_P2M_CH6,			/**< P2M6 */
	E_IM_PRO_P2M_CH7,			/**< P2M7 */
	E_IM_PRO_P2M_MAX,			/**< Max value */
} E_IM_PRO_P2M_CH;

/** P2M Enable/Disable
*/
typedef enum {
	E_IM_PRO_P2M_DIS = 0,		/**< Disable	*/
	E_IM_PRO_P2M_EN				/**< Enable		*/
} E_IM_PRO_P2M_ENABLE;

/** KNEE compression mode
*/
typedef enum {
	E_IM_PRO_P2M_KNEE_DIS	= 0,			/**< Disable Knee	*/
	E_IM_PRO_P2M_KNEE_EN_12BIT_TO_8BIT,		/**< KNEE compression is performed (12bit to 8bit)	*/
	E_IM_PRO_P2M_KNEE_EN_12BIT_TO_10BIT,	/**< KNEE compression is performed (12bit to 10bit)	*/
} E_IM_PRO_P2M_KNEE_ENABLE;

/** P2M LPF record mode
*/
typedef enum {
	E_IM_PRO_P2M_LPF_MODE_OFF = 0,		/**< LPF OFF		*/
	E_IM_PRO_P2M_LPF_MODE_SHORT,		/**< LPF Short		*/
	E_IM_PRO_P2M_LPF_MODE_MIDDLE,		/**< LPF Middle		*/
	E_IM_PRO_P2M_LPF_MODE_LONG			/**< LPF Long		*/
} E_IM_PRO_P2M_LPF_MODE;

/** P2M record mode
*/
typedef enum {
	E_IM_PRO_P2M_REC_MODE_8BIT	= 0,		/**< 8bit data			*/
	E_IM_PRO_P2M_REC_MODE_10BIT,			/**< 10bit pack data	*/
	E_IM_PRO_P2M_REC_MODE_12BIT,			/**< 12bit pack data	*/
	E_IM_PRO_P2M_REC_MODE_16BIT				/**< 16bit data			*/
} E_IM_PRO_P2M_REC_MODE;

/** P2M output data right shift
*/
typedef enum {
	E_IM_PRO_P2M_SHIFT_0BIT	= 0,		/**< 0bit shift	*/
	E_IM_PRO_P2M_SHIFT_1BIT,			/**< 1bit shift	*/
	E_IM_PRO_P2M_SHIFT_2BIT,			/**< 2bit shift	*/
	E_IM_PRO_P2M_SHIFT_3BIT,			/**< 3bit shift	*/
	E_IM_PRO_P2M_SHIFT_4BIT,			/**< 4bit shift	*/
	E_IM_PRO_P2M_SHIFT_5BIT,			/**< 5bit shift	*/
	E_IM_PRO_P2M_SHIFT_6BIT,			/**< 6bit shift	*/
	E_IM_PRO_P2M_SHIFT_7BIT,			/**< 7bit shift	*/
	E_IM_PRO_P2M_SHIFT_8BIT				/**< 8bit shift	*/
} E_IM_PRO_P2M_SHIFT;

/** P2M Enable Mirror
*/
typedef enum {
	E_IM_PRO_P2M_MIRROR_ENABLE_DIS = 0,	/**< Disable mirror	*/
	E_IM_PRO_P2M_MIRROR_ENABLE_EN		/**< Enable mirror	*/
} E_IM_PRO_P2M_MIRROR_ENABLE;

/** select input mode.
*/
typedef enum {
	E_IM_PRO_P2M_INPUT_MODE_1POINT = 0,	/**< 1-point input mode	*/
	E_IM_PRO_P2M_INPUT_MODE_2POINT		/**< 2-point input mode	*/
} E_IM_PRO_P2M_INPUT_MODE;

/*	@}*/

/** @addtogroup im_pro_common_pwch PWCH
@{
*/

/** PRO PWch Kind
*/
typedef enum {
	E_IM_PRO_PWCH_0	 = 0,		/**< PWch0			*/
	E_IM_PRO_PWCH_1	 = 1,		/**< PWch1			*/
	E_IM_PRO_PWCH_2	 = 2,		/**< PWch2			*/
	E_IM_PRO_PWCH_3	 = 3,		/**< PWch3			*/
	E_IM_PRO_PWCH_4	 = 4,		/**< PWch4			*/
	E_IM_PRO_PWCH_5	 = 5,		/**< PWch5			*/
	E_IM_PRO_PWCH_6	 = 6,		/**< PWch6			*/
	E_IM_PRO_PWCH_7	 = 7,		/**< PWch7			*/
	E_IM_PRO_PWCH_MAX	= 8		/**< PWch Max Value */
} E_IM_PRO_PWCH;

/** PWch transfer burst mode
*/
typedef enum {
	E_IM_PRO_PWCH_BURST_512BYTE = 2,	/**< 512byte burst transfer	*/
} E_IM_PRO_PWCH_BURST;

/** PRO PWch transfer demand point
*/
typedef enum {
	E_IM_PRO_PWCH_TRANS_IO_0 = 0,	/**< PWch transfer demand point master I/F 0 */
	E_IM_PRO_PWCH_TRANS_RESERVED	/**< Reserved(Setting is prohibited) */
} E_IM_PRO_PWCH_TRANS;

/** PRO PWch precedent request threshold
*/
typedef enum {
	E_IM_PRO_PWCH_CNT_NOLIMIT = 0,	/**< PWch no limit		*/
	E_IM_PRO_PWCH_CNT_MAX2,			/**< PWch max 2 times	*/
	E_IM_PRO_PWCH_CNT_MAX3,			/**< PWch max 3 times	*/
	E_IM_PRO_PWCH_CNT_MAX4,			/**< PWch max 4 times	*/
	E_IM_PRO_PWCH_CNT_MAX5,			/**< PWch max 5 times	*/
	E_IM_PRO_PWCH_CNT_MAX6,			/**< PWch max 6 times	*/
	E_IM_PRO_PWCH_CNT_MAX7,			/**< PWch max 7 times	*/
	E_IM_PRO_PWCH_CNT_NOLIMIT2		/**< PWch no limit		*/
} E_IM_PRO_PWCH_CNT;

/** writing with mask control
*/
typedef enum {
	E_IM_PRO_PWCH_WRITE_MASK_DIS = 0,	/**< Writing with mask is disabled */
	E_IM_PRO_PWCH_WRITE_MASK_EN			/**< Writing with mask is enabled */
} E_IM_PRO_PWCH_WRITE_MASK;

/** PWch burst alignment size
*/
typedef enum {
	E_IM_PRO_PWCH_BURST_ALIGN_SIZE_32BYTE	= 1,	/**< 32byte alignment burst	*/
	E_IM_PRO_PWCH_BURST_ALIGN_SIZE_128BYTE	= 3,	/**< 128byte alignment burst	*/
} E_IM_PRO_PWCH_BURST_ALIGN_SIZE;

/** Select Output format
*/
typedef enum {
	E_IM_PRO_PWCH_OUTPUT_FORMAT_NORMAL = 0,		/**< Normal format data output */
	E_IM_PRO_PWCH_OUTPUT_FORMAT_VIDEO			/**< Video format data output */
} E_IM_PRO_PWCH_OUTPUT_FORMAT;


/*	@}*/

/** @addtogroup im_pro_b2b_elf ELF
@{
*/

/** ELF Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_PAEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_PAEN_EN			/**< Enable		*/
} E_IM_PRO_ELF_PAEN;


/** ELF 1~5NR Enable/Disabl
*/
typedef enum {
	E_IM_PRO_ELF_N1S5EN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_ELF_N1S5EN_EN			/**< Enable		*/
} E_IM_PRO_ELF_N1S5EN;

/** ELF 5~9NR Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_N5S9EN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_ELF_N5S9EN_EN 			/**< Enable		*/
} E_IM_PRO_ELF_N5S9EN;

/** ELF F9NR Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_NFS9EN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_NFS9EN_EN 			/**< Enable		*/
} E_IM_PRO_ELF_NFS9EN;

/** ELF A0 function NR Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_NLA0EN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_ELF_NLA0EN_EN 			/**< Enable		*/
} E_IM_PRO_ELF_NLA0EN;

/** ELF ring pixcel padding Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_PDD_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_PDD_EN				/**< Enable		*/
} E_IM_PRO_ELF_PDD_ENABLE;

/** ELF ring pixcel padding mode.
*/
typedef enum {
	E_IM_PRO_ELF_PDD_MODE_SURROUND_4PIX = 0,	/**< ELF processing is executed with reference to surrounding 4 pixels.	*/
	E_IM_PRO_ELF_PDD_MODE_SURROUND_8PIX			/**< ELF processing is executed with reference to surrounding 8 pixels.	*/
} E_IM_PRO_ELF_PDD_MODE;



/** ELF F5 filter mode
*/
typedef enum {
	E_IM_PRO_ELF_F5_MD_STRONG = 0,	/**< Filter is a strong	*/
	E_IM_PRO_ELF_F5_MD_WEAK			/**< Filter is weak		*/
} E_IM_PRO_ELF_F5_MD;

/** ELF F9 filter mode
*/
typedef enum {
	E_IM_PRO_ELF_F9_MD_STRONG = 0,	/**< Filter is a strong	*/
	E_IM_PRO_ELF_F9_MD_WEAK			/**< Filter is weak		*/
} E_IM_PRO_ELF_F9_MD;

/** ELF pixel spacing equalization Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_PGEEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_PGEEN_EN			/**< Enable		*/
} E_IM_PRO_ELF_PGEEN;

/** ELF pixel spacing equalization upper left pixel
*/
typedef enum {
	E_IM_PRO_ELF_PGELT_R = 0,		/**< R pixel 	*/
	E_IM_PRO_ELF_PGELT_B,           /**< B pixel 	*/
	E_IM_PRO_ELF_PGELT_GR,          /**< Gr pixel	*/
	E_IM_PRO_ELF_PGELT_GB           /**< Gb pixel	*/
} E_IM_PRO_ELF_PGELT;

/** ELF D filter Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_ELDFEN_DIS = 0,	/**< Disable	*/
	E_IM_PRO_ELF_ELDFEN_EN			/**< Enable		*/
} E_IM_PRO_ELF_ELDFEN;

/** ELF defective pixel indivisual GrGb
*/
typedef enum {
	E_IM_PRO_ELF_DFGRGB_ALL = 0,	/**< GrGb all			*/
	E_IM_PRO_ELF_DFGRGB_INDIVIDUAL	/**< GrGb individual	*/
} E_IM_PRO_ELF_DFGRGB;

/** ELF defective pixel mode G
*/
typedef enum {
	E_IM_PRO_ELF_DFMDG_SINGLE = 0,	/**< G correct(single pixel	*/
	E_IM_PRO_ELF_DFMDG_DOUBLE		/**< G correct(double pixel	*/
} E_IM_PRO_ELF_DFMDG;

/** ELF defective pixel mode RB
*/
typedef enum {
	E_IM_PRO_ELF_DFMDRB_SINGLE = 0,	/**< G correct(single pixel	*/
	E_IM_PRO_ELF_DFMDRB_DOUBLE		/**< G correct(double pixel	*/
} E_IM_PRO_ELF_DFMDRB;

/** ELF defective pixel mode RB use G
*/
typedef enum {
	E_IM_PRO_ELF_DFRBUG_NOT_USE = 0,/**< not use	*/
	E_IM_PRO_ELF_DFRBUG_USE			/**< use		*/
} E_IM_PRO_ELF_DFRBUG;

/** ELF E Filter mode
*/
typedef enum {
	E_IM_PRO_ELF_EFMD_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_EFMD_EN			/**< Enable		*/
} E_IM_PRO_ELF_EFMD;

/** ELF L Filter mode
*/
typedef enum {
	E_IM_PRO_ELF_LFMD_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_LFMD_EN      		/**< Enable		*/
} E_IM_PRO_ELF_LFMD;

/** ELF Long-wavelength mode noise data
*/
typedef enum {
	E_IM_PRO_ELF_NSLMD_REDUCTION = 0,	/**< reduction mode		*/
	E_IM_PRO_ELF_NSLMD_MAGNIFICATION	/**< magnification mode	*/
} E_IM_PRO_ELF_NSLMD;

/** ELF Long-wavelength noise precision data
*/
typedef enum {
	E_IM_PRO_ELF_NSLRES_0 = 0,		/**< High-precision data	*/
	E_IM_PRO_ELF_NSLRES_1,			/**<	|					*/
	E_IM_PRO_ELF_NSLRES_2,			/**<	|					*/
	E_IM_PRO_ELF_NSLRES_3			/**< Low-precision data		*/
} E_IM_PRO_ELF_NSLRES;

/** ELF Long-wavelength noise compression Enable/Disable
*/
typedef enum {
	E_IM_PRO_ELF_NSLKEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_ELF_NSLKEN_EN				/**< Enable		*/
} E_IM_PRO_ELF_NSLKEN;

/** NR AF input selection path
*/
typedef enum {
	E_IM_PRO_ELF_ELFAFS_MAIN_PATH = 0,	/**< Main line path output signal is selected.	*/
	E_IM_PRO_ELF_ELFAFS_NR_PATH			/**< Synthesis noise reduction signal is selected.	*/
} E_IM_PRO_ELF_ELFAFS;

/**  A0 function defective pixel correction mode.
*/
typedef enum {
	E_IM_PRO_ELF_A0_DPC_MODE_DISABLE = 0,		/**< No defect correction.	*/
	E_IM_PRO_ELF_A0_DPC_MODE_ENABLE_LOW,		/**< Tolerance to defects is supported and relatively low.<br>
														There is no problem with single defect.	*/
	E_IM_PRO_ELF_A0_DPC_MODE_ENABLE_HIGH = 3,	/**< Tolerance to defects is supported and relatively high.	*/
} E_IM_PRO_ELF_A0_DPC_MODE;

/*	@}*/	// im_pro_b2b_elf


/** @addtogroup im_pro_common_prch PRCH
@{
*/

/** PRO PRch Kind
*/
typedef enum {
	E_IM_PRO_PRCH_0	 	= 0,		/**< PRch0			*/
	E_IM_PRO_PRCH_1,				/**< PRch1			*/
	E_IM_PRO_PRCH_2,				/**< PRch2			*/
	E_IM_PRO_PRCH_3,				/**< PRch3			*/
	E_IM_PRO_PRCH_4,				/**< PRch4			*/
	E_IM_PRO_PRCH_5,				/**< PRch5			*/
	E_IM_PRO_PRCH_6,				/**< PRch6			*/
	E_IM_PRO_PRCH_7,				/**< PRch7			*/
	E_IM_PRO_PRCH_8,				/**< PRch8			*/
	E_IM_PRO_PRCH_9,				/**< PRch9			*/
	E_IM_PRO_PRCH_MAX				/**< Max Value	*/
} E_IM_PRO_PRCH;

/** PRO PRch transfer burst mode
*/
typedef enum {
	E_IM_PRO_PRCH_BURST_512BYTE = 0,	/**< 512byte burst transfer	*/
} E_IM_PRO_PRCH_BURST;

/** PRO PRch transfer demand point
*/
typedef enum {
	E_IM_PRO_PRCH_TRANS_IO_0 = 0,	/**< PRch transfer demand point master I/F 0 */
	E_IM_PRO_PRCH_TRANS_RESERVED	/**< Reserved(Setting is prohibited) */
} E_IM_PRO_PRCH_TRANS;

/** PRO PRch precedent request threshold
*/
typedef enum {
	E_IM_PRO_PRCH_CNT_NOLIMIT = 0,	/**< PRch no limit		*/
	E_IM_PRO_PRCH_CNT_MAX1,			/**< PRch max 1 time	*/
	E_IM_PRO_PRCH_CNT_MAX2,			/**< PRch max 2 times	*/
	E_IM_PRO_PRCH_CNT_MAX3,			/**< PRch max 3 times	*/
	E_IM_PRO_PRCH_CNT_MAX,			/**< Max value	*/
} E_IM_PRO_PRCH_CNT;

/*	@}*/

/** @addtogroup im_pro_common_m2p M2P
@{
*/

/** PRO PRch Kind
*/
typedef enum {
	E_IM_PRO_M2P_0	 = 0,		/**< M2P ch0		*/
	E_IM_PRO_M2P_1	 = 1,		/**< M2P ch1		*/
	E_IM_PRO_M2P_2	 = 2,		/**< M2P ch2		*/
	E_IM_PRO_M2P_3	 = 3,		/**< M2P ch3		*/
	E_IM_PRO_M2P_6	 = 6,		/**< M2P ch6		*/
	E_IM_PRO_M2P_7	 = 7,		/**< M2P ch7		*/
	E_IM_PRO_M2P_8	 = 8,		/**< M2P ch8		*/
	E_IM_PRO_M2P_9	 = 9,		/**< M2P ch9		*/
	E_IM_PRO_M2P_MAX = 10		/**< Max Value	*/
} E_IM_PRO_M2P_CH;

/** M2P Enable/Disable
*/
typedef enum {
	E_IM_PRO_M2P_PAEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_M2P_PAEN_EN			/**< Enable		*/
} E_IM_PRO_M2P_PAEN;

/** M2P data type
*/
typedef enum {
	E_IM_PRO_M2P_DATA_TYPE_8BIT		= 0,	/**< 8bit data			*/
	E_IM_PRO_M2P_DATA_TYPE_10BIT,			/**< 10bit pack data	*/
	E_IM_PRO_M2P_DATA_TYPE_12BIT,			/**< 12bit pack data	*/
	E_IM_PRO_M2P_DATA_TYPE_16BIT			/**< 16bit data			*/
} E_IM_PRO_M2P_DATA_TYPE;

/** M2P Deknee enable
*/
typedef enum {
	E_IM_PRO_M2P_DEKNEE_DIS	= 0,			/**< Disable Knee	*/
	E_IM_PRO_M2P_DEKNEE_EN_8BIT_TO_12BIT,	/**< Knee compression is performed (8bit to 12bit)	*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_IM_PRO_M2P_DEKNEE_EN_10BIT_TO_12BIT,	/**< Knee compression is performed (10bit to 12bit)	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_IM_PRO_M2P_DEKNEE_MAX,				/**< MAX value	*/
} E_IM_PRO_M2P_DEKNEE_ENABLE;


/** M2P output data left shift
*/
typedef enum {
	E_IM_PRO_M2P_SHIFT_0BIT	= 0,		/**< 0bit shift	*/
	E_IM_PRO_M2P_SHIFT_1BIT,			/**< 1bit shift	*/
	E_IM_PRO_M2P_SHIFT_2BIT,			/**< 2bit shift	*/
	E_IM_PRO_M2P_SHIFT_3BIT,			/**< 3bit shift	*/
	E_IM_PRO_M2P_SHIFT_4BIT,			/**< 4bit shift	*/
	E_IM_PRO_M2P_SHIFT_5BIT,			/**< 5bit shift	*/
	E_IM_PRO_M2P_SHIFT_6BIT,			/**< 6bit shift	*/
	E_IM_PRO_M2P_SHIFT_7BIT,			/**< 7bit shift	*/
	E_IM_PRO_M2P_SHIFT_8BIT				/**< 8bit shift	*/
} E_IM_PRO_M2P_SHIFT;

/** M2P saturation compensation
*/
typedef enum {
	E_IM_PRO_M2P_SAT_NOCOMP	= 0,		/**< saturation no compensation	*/
	E_IM_PRO_M2P_SAT_COMP				/**< saturation compensation	*/
} E_IM_PRO_M2P_SAT;

/** M2P bit shift direction
*/
typedef enum {
	E_IM_PRO_M2P_BIT_SHIFT_L	= 0,		/**< Processing bit Left shift	*/
	E_IM_PRO_M2P_BIT_SHIFT_R				/**< Processing bit Right shift	*/
} E_IM_PRO_M2P_BIT_SHIFT;

/*	@}*/	// im_pro_common_m2p

/** @addtogroup im_pro_common_moni MONI
@{
*/

/** MONI channel number
*/
typedef enum {
	E_IM_PRO_MONI_CH_0 = 0,					/**< MONI ch0	*/
	E_IM_PRO_MONI_CH_1,						/**< MONI ch1	*/
	E_IM_PRO_MONI_CH_2,						/**< MONI ch2	*/
	E_IM_PRO_MONI_CH_3,						/**< MONI ch3	*/
} E_IM_PRO_MONI_CH;

/**  Output monitor input mode setting
*/
typedef enum {
	E_IM_PRO_MONI_MODE_2POINT_INPUT = 1,	/**< 2-point input mode.	*/
	E_IM_PRO_MONI_MODE_4POINT_INPUT,		/**< 4-point input mode.	*/
	E_IM_PRO_MONI_MODE_8POINT_INPUT,		/**< 8-point input mode.	*/
} E_IM_PRO_MONI_MODE;

/*	@}*/	// im_pro_common_moni

/** @addtogroup im_pro_sen_ldiv LDIV
@{
*/

/** Line division processing first line mode setting.
*/
typedef enum {
	E_IM_PRO_LDIV_LDSTMD_LINE = 0,			/**< Division processing is started from the first line of a frame.	*/
	E_IM_PRO_LDIV_LDSTMD_LINESKIP			/**< Division processing is started by skipping the first line of a frame.	*/
} E_IM_PRO_LDIV_LDSTMD;


/** Line division processing input point mode setting.
*/
typedef enum {
	E_IM_PRO_LDIV_LDIPMD_2POINT_INPUT = 0,		/**< 2-point input		*/
	E_IM_PRO_LDIV_LDIPMD_4POINT_INPUT,			/**< 4-point input		*/
	E_IM_PRO_LDIV_LDIPMD_8POINT_INPUT,			/**< 8-point input	*/
} E_IM_PRO_LDIV_LDIPMD;

/** Line division processing VD subsampling mode setting.<br>
	set the VD subsampling mode of the output side.
*/
typedef enum {
	E_IM_PRO_LDIV_LDVDMD_NO_SUBSAMPLING = 0,	/**< No VD subsampling	*/
	E_IM_PRO_LDIV_LDVDMD_HARF_SUBSAMPLING,		/**< 1/2 VD subsampling	*/
	E_IM_PRO_LDIV_LDVDMD_ONE_THIRD_SUBSAMPLING,	/**< 1/3 VD subsampling	*/
	E_IM_PRO_LDIV_LDVDMD_QUARTER_SUBSAMPLING,	/**< 1/4 VD subsampling	*/
} E_IM_PRO_LDIV_LDVDMD;

/** Line division processing HD insertion mode setting.<br>
	set the HD signal insertion mode right after VD.
*/
typedef enum {
	E_IM_PRO_LDIV_HD_INSERTION_ENABLE_DIS = 0,			/**< Disable	*/
	E_IM_PRO_LDIV_HD_INSERTION_ENABLE_EN,				/**< Enable		*/
} E_IM_PRO_LDIV_HD_INSERTION_ENABLE;

/** Line division processing multi-CH combination mode setting.<br>
	set the multi-CH combination mode of the line division processing.
*/
typedef enum {
	E_IM_PRO_LDIV_LDMCH_NO_COMBI = 0,			/**< No combination	*/
	E_IM_PRO_LDIV_LDMCH_2_COMBI,				/**< 2 combination	*/
	E_IM_PRO_LDIV_LDMCH_4_COMBI,				/**< 4 combination	*/
} E_IM_PRO_LDIV_LDMCH;

/** Line division processing rectangle output mode setting.<br>
	 set the rectangle output mode ofthe output side of the line division processing.
*/
typedef enum {
	E_IM_PRO_LDIV_LDDIVMD_SINGLE_OUTPUT = 0,	/**< Single output	*/
	E_IM_PRO_LDIV_LDDIVMD_2RECT_OUTPUT,			/**< 2-rectangle output	*/
	E_IM_PRO_LDIV_LDDIVMD_3RECT_OUTPUT,			/**< 3-rectangle output	*/
	E_IM_PRO_LDIV_LDDIVMD_4RECT_OUTPUT,			/**< 4-rectangle output	*/
} E_IM_PRO_LDIV_LDDIVMD;

/** Line division processing output point mode setting.
*/
typedef enum {
	E_IM_PRO_LDIV_LDOPMD_2LINE_1OUT = 0,			/**< 2-line 1-point output	*/
	E_IM_PRO_LDIV_LDOPMD_2LINE_2OUT,				/**< 2-line 2-point output	*/
} E_IM_PRO_LDIV_LDOPMD;


/** LDIV channel number
*/
typedef enum {
	E_IM_PRO_LDIV_CH_0 = 0,						/**< ch0	*/
	E_IM_PRO_LDIV_CH_1,							/**< ch1	*/
	E_IM_PRO_LDIV_CH_2,							/**< ch1	*/
	E_IM_PRO_LDIV_CH_3,							/**< ch1	*/
} E_IM_PRO_LDIV_CH;

/*	@}*/	// im_pro_sen_ldiv


/** @addtogroup im_pro_common_pg PG
@{
*/

/** PRO PG mode
*/
typedef enum {
	E_IM_PRO_PG_MODE_1LINE = 0,		/**< 1line processing mode	*/
	E_IM_PRO_PG_MODE_2LINE			/**< 2line processing mode	*/
} E_IM_PRO_PG_MODE;

/*	@}*/	// im_pro_common_pg

/** @addtogroup im_pro_sro_cag CAG
@{
*/

/** CAG Enable/Disable
*/
typedef enum {
	E_IM_PRO_CAG_PAEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_CAG_PAEN_EN			/**< Enable		*/
} E_IM_PRO_CAG_PAEN;

/** Clip select after correction
*/
typedef enum {
	E_IM_PRO_CAG_CLIP_MODE_14BIT = 0,	/**< Data is clipped to 14bit.	*/
	E_IM_PRO_CAG_CLIP_MODE_14BIT_MAX	/**< Data is clipped to 12bit.	*/
} E_IM_PRO_CAG_CLIP_MODE;

/** CAG Gr/Gb gap correction
*/
typedef enum {
	E_IM_PRO_CAG_COR_MODE_GBGB = 0,	/**< GrGb use	*/
	E_IM_PRO_CAG_COR_MODE_G			/**< G use		*/
} E_IM_PRO_CAG_COR_MODE;

/** CAG Processing pixel padding Enable/Disable
*/
typedef enum {
	E_IM_PRO_CAG_PAD_MODE_DIS = 0,	/**< Disable	*/
	E_IM_PRO_CAG_PAD_MODE_EN		/**< Enable		*/
} E_IM_PRO_CAG_PAD_MODE;

/** CAG gap correction mode
*/
typedef enum {
	E_IM_PRO_CAG_MODE_OPT_CENTER_ONLY = 0,			/**< Optical center distance only  */
	E_IM_PRO_CAG_MODE_SURFACE_ONLY,					/**< Surface correction only */
	E_IM_PRO_CAG_MODE_OPT_CENTER_SURFACE_ADD		/**< Additional value of Optical center distance and Surface correction */
} E_IM_PRO_CAG_MODE;

/*	@}*/	// im_pro_sro_cag

/** @addtogroup im_pro_sro_afpb AFPB
@{
*/
/** AFPB channel number
*/
typedef enum {
	E_IM_PRO_AFPB_CH_A = 0,						/**< ch A	*/
	E_IM_PRO_AFPB_CH_G,							/**< ch G	*/
} E_IM_PRO_AFPB_CH;

/*	@}*/	// im_pro_sro_afpb

/** @addtogroup im_pro_sro_fshdl FSHDL
@{
*/

/** FSHDL channel number
*/
typedef enum {
	E_IM_PRO_FSHDL_CH_0 = 0,					/**< ch 0	*/
	E_IM_PRO_FSHDL_CH_1,						/**< ch 1	*/
} E_IM_PRO_FSHDL_CH;

/** Formatting correction factor concentric
*/
typedef enum {
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_0 = 0,	/**<   XXXX.XXXX_XXXX_XXXX		*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_1,		/**< 1+XXXX.XXXX_XXXX_XXXX		*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_2,		/**<    XXX.XXXX_XXXX_XXXX_X	*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_3,		/**< 1+ XXX.XXXX_XXXX_XXXX_X	*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_4,		/**<     XX.XXXX_XXXX_XXXX_XX	*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_5,		/**< 1+  XX.XXXX_XXXX_XXXX_XX	*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_6,		/**<      X.XXXX_XXXX_XXXX_XXX	*/
	E_IM_PRO_FSHDL_CORRECTION_FORMAT_7,		/**< 1+   X.XXXX_XXXX_XXXX_XXX	*/
} E_IM_PRO_FSHDL_CORRECTION_FORMAT;

/** Saturation correction Enable/Disable
*/
typedef enum {
	E_IM_PRO_FSHDL_SAT_CORRECTION_DIS = 0,	/**< Disable	*/
	E_IM_PRO_FSHDL_SAT_CORRECTION_EN		/**< Enable		*/
} E_IM_PRO_FSHDL_SAT_CORRECTION;

/*	@}*/	// im_pro_sro_fshdl

/** @addtogroup im_pro_sro_ffc FFC
@{
*/

/** FFC Enable/Disable
*/
typedef enum {
	E_IM_PRO_FFC_PAEN_DIS = 0,		/**< Disable	*/
	E_IM_PRO_FFC_PAEN_EN			/**< Enable		*/
} E_IM_PRO_FFC_PAEN;

/*	@}*/	// im_pro_sro_ffc

/** @addtogroup im_pro_common_pzsft PZSFT
@{
*/

/** PZSFT Channel Number
*/
typedef enum {
	E_IM_PRO_PZSFT_CH_B1 = 0,		/**< SRO block PZSFT B1		*/
	E_IM_PRO_PZSFT_CH_B2,			/**< SRO block PZSFT B2		*/
	E_IM_PRO_PZSFT_CH_C1,			/**< SRO block PZSFT C1		*/
	E_IM_PRO_PZSFT_CH_C2,			/**< SRO block PZSFT C2		*/
	E_IM_PRO_PZSFT_CH_A1,			/**< B2B block PZSFT A1		*/
	E_IM_PRO_PZSFT_CH_A2,			/**< B2B block PZSFT A2		*/
	E_IM_PRO_PZSFT_CH_A2A,			/**< B2B block PZSFT A2A	*/
	E_IM_PRO_PZSFT_CH_MAX,		/**< MAX value	*/
} E_IM_PRO_PZSFT_CH;

/*	@}*/	// im_pro_common_pzsft


/** @addtogroup im_pro_handler Interrupt Handler
@{
*/

/** PRO Interrupt output mode
*/
typedef enum {
	E_IM_PRO_INT_INTMD_OR = 0,		/**< Logical sum of an interruption flag		 */
	E_IM_PRO_INT_INTMD_AND,			/**< The logical product of an interruption flag */
	E_IM_PRO_INT_INTMD_ORAND		/**< The logical product of an interruption flag,
										 and the logical sum of abnormal end interruption */
} E_IM_PRO_INT_INTMD;

/*	@}*/	// im_pro_handler

/** @addtogroup im_pro_sen_sg SG
@{
*/
/** SG channel number
*/
typedef enum {
	E_IM_PRO_SG_CH_0 = 0,			/**< ch0	*/
	E_IM_PRO_SG_CH_1,				/**< ch1	*/
	E_IM_PRO_SG_CH_2,				/**< ch1	*/
	E_IM_PRO_SG_CH_3,				/**< ch1	*/
} E_IM_PRO_SG_CH;
/*	@}*/	// im_pro_sen_sg

/** @addtogroup im_pro_common_pgain PGAIN
@{
*/
/** PGAIN decimal point position
*/
typedef enum {
	E_IM_PRO_PGAIN_DECI_POS_8 = 0,			/**<  5-bit integer part,8-bit fractional part	*/
	E_IM_PRO_PGAIN_DECI_POS_7,				/**<  6-bit integer part,7-bit fractional part	*/
	E_IM_PRO_PGAIN_DECI_POS_6,				/**<  7-bit integer part,6-bit fractional part	*/
	E_IM_PRO_PGAIN_DECI_POS_5,				/**<  8-bit integer part,5-bit fractional part	*/
	E_IM_PRO_PGAIN_DECI_POS_4,				/**<  9-bit integer part,4-bit fractional part	*/
	E_IM_PRO_PGAIN_DECI_POS_3,				/**< 10-bit integer part,3-bit fractional part	*/
} E_IM_PRO_PGAIN_DECI_POS;

/** PRO PGAIN select
*/
typedef enum {
	E_IM_PRO_PGAIN0_SELECT = 0,		/**< PGAIN0	*/
	E_IM_PRO_PGAIN1_SELECT,			/**< PGAIN1	*/
	E_IM_PRO_PGAIN2_SELECT			/**< PGAIN2	*/
} E_IM_PRO_PGAIN_SELECT;


/*	@}*/	// im_pro_common_pgain

/** @addtogroup im_pro_common_lnr LNR
@{
*/

/** LNR channel number
*/
typedef enum {
	E_IM_PRO_LNR_CH_0 = 0,			/**< ch0	*/
	E_IM_PRO_LNR_CH_1,				/**< ch1	*/
	E_IM_PRO_LNR_CH_2,				/**< ch1	*/
	E_IM_PRO_LNR_CH_3,				/**< ch1	*/
} E_IM_PRO_LNR_CH;

/*	@}*/	// im_pro_common_lnr


/** @addtogroup im_pro_handler_sen SEN Interrupt
@{
*/
/** SLVS-EC Interrupt type
*/
typedef enum {
	E_IM_PRO_SLVS_INT_TYPE_INTN_O = 0,		/**< interrupt cause of INTn_O (Normal operation).	 */
	E_IM_PRO_SLVS_INT_TYPE_LINK_ERR,		/**< interrupt cause of EINTn_O (Link Error related).	 */
	E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1,		/**< interrupt cause of EINTn_O (MAC Error1 related).	 */
	E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2,		/**< interrupt cause of EINTn_O (MAC Error2 related).	 */
} E_IM_PRO_SLVS_INT_TYPE;

/** MIPI-CPHY interrupt category.
*/
typedef enum {
	E_IM_PRO_CMIPI_INT_CATE_0 = 0,		/**< interrupt cause type0.	 */
	E_IM_PRO_CMIPI_INT_CATE_1,			/**< interrupt cause type1.	 */
} E_IM_PRO_CMIPI_INT_CATE;

/*	@}*/	// im_pro_handler_sen

/** @addtogroup im_pro_common Common unit
@{
*/
/** PRO block type
*/
typedef enum {
	E_IM_PRO_BLOCK_TYPE_SEN = 0,	/**< SEN block	*/
	E_IM_PRO_BLOCK_TYPE_SRO,		/**< SRO block	*/
	E_IM_PRO_BLOCK_TYPE_B2B,		/**< B2B block	*/
	E_IM_PRO_BLOCK_TYPE_STAT,		/**< STAT block	*/
	E_IM_PRO_BLOCK_TYPE_PAS,		/**< PAS block	*/
	E_IM_PRO_BLOCK_TYPE_MAX,		/**< MAX value	*/
} E_IM_PRO_BLOCK_TYPE;

/** PRO Clock block type
*/
typedef enum {
	E_IM_PRO_CLK_BLOCK_TYPE_SEN = 0,	/**< SEN block	*/
	E_IM_PRO_CLK_BLOCK_TYPE_SRO,		/**< SRO block	*/
	E_IM_PRO_CLK_BLOCK_TYPE_SRO2,		/**< SRO2 block(behind HRB)	*/
	E_IM_PRO_CLK_BLOCK_TYPE_B2B,		/**< B2B block	*/
	E_IM_PRO_CLK_BLOCK_TYPE_STAT,		/**< STAT block	*/
	E_IM_PRO_CLK_BLOCK_TYPE_PAS,		/**< PAS block	*/
	E_IM_PRO_CLK_BLOCK_TYPE_MAX,		/**< MAX value	*/
} E_IM_PRO_CLK_BLOCK_TYPE;

/** PRO Unit Number
*/
typedef enum {
	E_IM_PRO_UNIT_NUM_1 = 0,	/**< UNIT number 1 block	*/
	E_IM_PRO_UNIT_NUM_2,		/**< UNIT number 2 block	*/
	E_IM_PRO_BOTH_UNIT,			/**< UNIT 1 & 2 block	*/
	E_IM_PRO_UNIT_NUM_MAX,		/**< MAX value	*/
} E_IM_PRO_UNIT_NUM;

/*	@}*/	// im_pro_common

// --- REMOVE_BUZ BEGIN
// @cond
/** @addtogroup im_pro_sro_buztop BUZTOP
@{
*/
/** PRO Bayer resize select.
*/
typedef enum {
	E_IM_PRO_BUZTOP_SEL_HRB_VRB = 0,	/**< Using HRB/VRB resizer	*/
	E_IM_PRO_BUZTOP_SEL_BUZTOP,			/**< Using BUZTOP(Buzzard) resizer	*/
	E_IM_PRO_BUZTOP_SEL_MAX,			/**< MAX value	*/
} E_IM_PRO_BUZTOP_SEL;
/*	@}*/	// im_pro_sro_buztop
// @endcond
// --- REMOVE_BUZ END

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------

/** @addtogroup im_pro_common Common unit
@{
*/
/** PRO Bayer first pixel
*/
typedef enum {
	E_IM_PRO_ORG_R = 0,				/**< R pixel standard	*/
	E_IM_PRO_ORG_B,					/**< B pixel standard	*/
	E_IM_PRO_ORG_GR,				/**< Gr pixel standard	*/
	E_IM_PRO_ORG_GB					/**< Gb pixel standard	*/
} E_IM_PRO_ORG;

/** PRO Get RGB ULONG data
*/
typedef struct {
	ULONG		R;		/**< R		*/
	ULONG		Gr;		/**< Gr		*/
	ULONG		Gb;		/**< Gb		*/
	ULONG		B;		/**< B		*/
} T_IM_PRO_RGB4;

/** PRO Get RGB SHORT data
*/
typedef struct {
	SHORT		R;		/**< R		*/
	SHORT		Gr;		/**< Gr		*/
	SHORT		Gb;		/**< Gb		*/
	SHORT		B;		/**< B		*/
} T_IM_PRO_RGB2;

/** Area information for macro processing 
*/
typedef struct {
	USHORT		pos_x;	/**< Start position x */
	USHORT		pos_y;	/**< Start position y */
	USHORT		width;	/**< Width of macro processing area */
	USHORT		lines;	/**< Line of macro processing area */
} T_IM_PRO_AREA_INFO;

/** AXI control setting.
*/
typedef struct {
	UCHAR		cache_type;		/**< cache type for read channel	@@ARCACHE/@@AWCACHE */
	UCHAR		protect_type;	/**< protect type for read channel	@@ARPROT/@@AWPROT */
} T_IM_PRO_AXI_CTRL;


/*	@}*/	// im_pro_common

/** @addtogroup im_pro_sen_sg SG
@{
*/
/** SG Control information
*/
typedef struct {
	UCHAR		vhdo;		/**< VD/HD I/O control.	<br>
								value range		:[0:Input 1:Output]<br>
								target registor	:@@VHDO		*/
	UCHAR		vdot;		/**< VD output timing.<br>
								value range		:[0:HD0 2cycle 1:HD0 1cycle]<br>
								target registor :@@VDOT		*/
	USHORT		vdinv;		/**< VD inverted output.<br>
								value range		:[0:L out 1:H out]<br>
								target registor	:@@VDINV	*/
	UCHAR		hdinv;		/**< HD inverted output.<br>
								value range		:[0:L out 1:H out]<br>
								target registor :@@HDINV	*/
	UCHAR		hdchg;		/**< Last HDO output switch mode.<br>
								value range		:[0:HDO output cycle is always constant 1:HDO output is switched for only the last line]<br>
								target registor :@@HDCHG	*/
	UCHAR		vddly;		/**< VD output delay.<br>
								value range		:[0-255]<br>
								target registor :@@VDDLY	*/
	ULONG		vdocyc;		/**< VD0 output cycle.<br>
								value range		:[0(=0x1000000) 0x000004 - 0xFFFFFF]<br>
								target registor	:@@VDOCYC	*/
	USHORT		vdow;		/**< VD0 output width.<br>
								value range		:[0(=512) 1 - 511]<br>
								target registor	:@@VDOW		*/
	USHORT		hdocyc;		/**< HD0 output cycle.<br>
								value range		:[0(=0x4000) 0x0001 - 0x3FFF]<br>
								target registor	:@@HDOCYC	*/
	USHORT		hdow;		/**< HD0 output width.<br>
								value range		:[0(=512) 1 - 511]<br>
								target registor	:@@HDOW		*/
	USHORT		hdocyc2;	/**< HD0 output cycle2.<br>
								value range		:[0(=0x4000) 0x0001 - 0x3FFF]<br>
								target registor	:@@HDOCYC2	*/
	USHORT		hdow2;		/**< VD0 output width2.<br>
								value range		:[0(=512) 1 - 511]<br>
								target registor	:@@HDOW2	*/
} T_IM_PRO_SG_CTRL;

/** SG monitor information
*/
typedef struct {
	UCHAR	input_vd_signal;	/**< monitor the input VD signal from the outside.	 */
	UCHAR	input_hd_signal;	/**< monitor the input HD signal from the outside.	 */
} T_IM_PRO_SG_MONI_INFO;

/*	@}*/	// im_pro_sen_sg


/** @addtogroup im_pro_common_pgain PGAIN
@{
*/
/** PRO PGAIN Macro RGB USHORT data
*/
typedef struct {
	USHORT						R;							/**< R gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINR */
	USHORT						Gr;							/**< Gr gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINGR */
	USHORT						Gb;							/**< Gb gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINGB */
	USHORT						B;							/**< B gain<br>
																value range		:[0 - 8191(=31.124511719)]<br>
																target registor	:@@PGAINB */
	E_IM_PRO_PGAIN_DECI_POS		decimal_point_position;		/**< Decimal Point Position<br>
																target registor	:@@PGAINDP */
	SHORT						R_offset;					/**< R offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSR */
	SHORT						Gr_offset;					/**< Gr offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSGR */
	SHORT						Gb_offset;					/**< Gb offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSGB */
	SHORT						B_offset;					/**< B offset<br>
																value range		:[0x1000(=-4096) - 0x0FFF(=+4095)]<br>
																target registor	:@@POFSB */
} T_IM_PRO_PGAIN_CTRL;

/*	@}*/	// im_pro_common_pgain


/** @addtogroup im_pro_common_pg PG
@{
*/

/** PRO PG Macro Control information
*/
typedef struct {
	E_IM_PRO_PG_MODE	mode;					/**< PG processing mode<br>
													target registor	:@@PGMD */
	USHORT				input_img_lines;		/**< The number of input pixel image lines<br>
													value range		:[2 - 8192]<br>
													target registor	:@@PGVW<br>
													remarks			:The number of the values which can be set up is even. */
	USHORT				input_img_width;		/**< The number of input pixel image width<br>
													value range		:[2 - 12288]<br>
													target registor	:@@PGHW<br>
													remarks			:The number of the values which can be set up is even. */
	USHORT				vertical_blanking;		/**< The number of vertical level blanking<br>
													value range		:[1 - 8192]<br>
													target registor	:@@PGVBLNK */
	USHORT				horizontal_blanking;	/**< The number of horizontal level blanking<br>
													value range		:[0 - 8192]<br>
													target registor	:@@PGHBLNK */
	USHORT				start_blanking;			/**< The number of the blanking between VD-HD <br>
													value range		:[0 - 16383]<br>
													target registor	:@@PGVHBLNK */
} T_IM_PRO_PG_CTRL;

/*	@}*/	// im_pro_common_pg

/** @addtogroup im_pro_common_moni MONI
@{
*/

/** MONI Macro Control information
*/
typedef struct {
	E_IM_PRO_MONI_MODE	monitor_mode;							/**< monitor input mode setting.<br>
																	target registor	:@@MONIMD */
	USHORT				hd_cnt_expect_val;						/**< Expect value of HD count.<br>
																	target registor	:@@MONIHDEXP */
	ULONG				valid_pixel_expect_cnt_in_1frame;		/**< Expect value of intra-frame valid pixel count.<br>
																	target registor	:@@MONIEPFEXP */
	ULONG				lower_limit_except_h_blank;				/**< Lower limit of the expect value of the horizontal blank cycle.<br>
																	target registor	:@@MONIBLLMINEXP */
	ULONG				upper_limit_except_h_blank;				/**< Upper limit of the expect value of the horizontal blank cycle.<br>
																	target registor	:@@MONIBLLMAXEXP */
	USHORT				additional_hd_cnt;						/**< Additional value of HD count.<br>
																	value range[0~4095]<br>
																	target registor	:@@MONIADHD */
	ULLONG				additional_hd_gap_cnt;					/**< Cycle value of HD gap.<br>
																	value range[0x0~0xFFFFFFFFFFFF]<br>
																	target registor	:@@MONIADHW */
} T_IM_PRO_MONI_CTRL;

/** Monitoring result format of MONI macro.
*/
typedef struct {
	USHORT				vd_cnt;									/**< accumulation value of VD signal after this macro is started.<br>
																	target registor	:@@MONIVD */
	USHORT				hd_cnt;									/**< accumulation value of HD signal after this macro is started.<br>
																	target registor	:@@MONIHD */
	ULONG				pixel_cnt_in_1frame;					/**< valid pixel count inside the frame.<br>
																	target registor	:@@MONIEPF */
	USHORT				min_val_h_valid_pixel_cnt;				/**< minimum value of the horizontal valid pixel count inside a frame.<br>
																	target registor	:@@MONIEPLMIN */
	USHORT				max_val_h_valid_pixel_cnt;				/**< maximum value of the horizontal valid pixel count inside a frame.<br>
																	target registor	:@@MONIEPLMAX */
	ULLONG				intra_frame_blank_cycle;				/**< cycle count (core clock cycle count) of the blank period inside a frame.<br>
																	target registor	:@@MONIBLF */
	ULONG				min_val_h_blank_cycle;					/**< minimum value of the horizontal blank cycle (core clock cycle count) inside a frame.<br>
																	target registor	:@@MONIBLLMIN */
	ULONG				max_val_h_blank_cycle;					/**< maximum value of the horizontal blank cycle (core clock cycle count) inside a frame.<br>
																	target registor	:@@MONIBLLMAX */
} T_IM_PRO_MONI_RESULT;

/*	@}*/	// im_pro_common_moni

/** @addtogroup im_pro_sen_ldiv LDIV
@{
*/

/** PRO LDIV Macro Control information
*/
typedef struct {
	E_IM_PRO_LDIV_LDSTMD				mode;						/**< Line division processing first line mode setting.<br>
																		target registor	:LDIVCTL@@LDSTMD */
	E_IM_PRO_LDIV_LDIPMD				ldipmd;						/**< Line division processing input point mode setting.<br>
																		target registor	:LDIVCTL@@LDIPMD */
	E_IM_PRO_LDIV_LDVDMD				ldvdmd;						/**< Line division processing VD subsampling mode setting.<br>
																		target registor	:LDIVCTL@@LDVDMD */
	E_IM_PRO_LDIV_HD_INSERTION_ENABLE	hd_insertion_en;			/**< Line division processing HD insertion mode setting.<br>
																		target registor	:LDIVCTL@@LDHDMD */
	E_IM_PRO_LDIV_LDMCH					ldmch;						/**< Line division processing multi-CH combination mode setting.<br>
																		target registor	:LDIVCTL@@LDMCH */
	E_IM_PRO_LDIV_LDDIVMD				lddivmd;					/**< Line division processing rectangle output mode setting.<br>
																		target registor	:LDIVCTL@@LDDIVMD */
	E_IM_PRO_LDIV_LDOPMD				ldopmd;						/**< Line division processing output point mode setting.<br>
																		target registor	:LDIVCTL@@LDOPMD */
} T_IM_PRO_LDIV_CTRL;

/** LDIV Area information
*/
typedef struct {
	USHORT		in_width;		/**< Line division processing input horizontal pixel count.<br>
									value range[2~***]<br>
									Maximum setting value depends on the horizontal maximum size of @ref Im_PRO_SENTOP_Ctrl() argument of @ref  E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL.<br>
									target registor	:LDIVIHW@@LDIVIHW */
	USHORT		out_pos_x[4];	/**< Line Division Processing Output Horizontal Start Position.<br>
									value range[2~16382]<br>
									target registor	:LDIVOH@@LDIVOH[x] */
	USHORT		out_width[4];	/**< Line Division Processing Output Horizontal Pixel Count Register.<br>
									value range[2~***]<br>
									Maximum setting value depends on the horizontal maximum size of @ref Im_PRO_SENTOP_Ctrl() argument of @ref  E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL.<br>
									target registor	:LDIVOHW@@LDIVOHW[x] */
} T_IM_PRO_LDIV_AREA;

/*	@}*/	// im_pro_sen_ldiv


/** @addtogroup im_pro_sen_slvs SLVS-EC
@{
*/

/** Valid Lane select information */
typedef struct {
	E_IM_PRO_SLVS_LANE_ENABLE	lane0:1;	/**< Lane0 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane1:1;	/**< Lane1 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane2:1;	/**< Lane2 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane3:1;	/**< Lane3 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane4:1;	/**< Lane4 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane5:1;	/**< Lane5 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane6:1;	/**< Lane6 */
	E_IM_PRO_SLVS_LANE_ENABLE	lane7:1;	/**< Lane7 */
	ULONG						dummy:24;	/**< Not used */
} T_IM_PRO_SLVS_VALID_LANE_SEL;

/** Input differential data value inversion setting. */
typedef struct {
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane0:1;	/**< Lane0 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane1:1;	/**< Lane1 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane2:1;	/**< Lane2 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane3:1;	/**< Lane3 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane4:1;	/**< Lane4 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane5:1;	/**< Lane5 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane6:1;	/**< Lane6 */
	E_IM_PRO_SLVS_INPUT_DATA_INVERSION	lane7:1;	/**< Lane7 */
} T_IM_PRO_SLVS_INPUT_DATA_INVERSION_SEL;

/** Control the ICPCODE port of PMA macro (common to each PMA) and the REFSEL port of the termination resistor adjustment circuit.<br>
	*Caution* Changing the setting value in normal use is prohibited. */
typedef struct {
	UCHAR	enabled;								/**< Optional parameter setting enabled.[0: disabled  other: enabled]<br>
														If this parameter set disabled, H/W initial value applied to PHYCR0@@REFSEL and PHYCR0@@ICPCODE. */
	UCHAR	refsel;									/**< Termination resistor adjustment circuit mode select. PHYCR0@@REFSEL */
	UCHAR	icpcode;								/**< Charge pump control code PHYCR0@@ICPCODE */
} T_IM_PRO_SLVS_PMA_INIT_OPT;

/** Physical Media Attachment initialization setting. */
typedef struct {
	T_IM_PRO_SLVS_INPUT_DATA_INVERSION_SEL	invert_sel;				/**< input differential data value inversion setting. PHYCR1@@INVDATA */
	E_IM_PRO_SLVS_MULTIPLICATION_FACTOR		multiplication_factor;	/**< Multiplication factor control setting. PHYCR1@@IFVDCNT */
	T_IM_PRO_SLVS_PMA_INIT_OPT				opt_init_param;			/**< Optional setting parameter for PMA macro. */
} T_IM_PRO_SLVS_PMA_INIT;

/** Physical Media Attachment Power Down parameters
*/
typedef struct {
	E_IM_PRO_SLVS_PD_STATE		lane0_pd_state;		/**< Lane0 Power Down state. PHYCR2@@IRXPDX0	*/
	E_IM_PRO_SLVS_PD_STATE		lane1_pd_state;		/**< Lane1 Power Down state. PHYCR2@@IRXPDX1	*/
	E_IM_PRO_SLVS_PD_STATE		lane2_pd_state;		/**< Lane2 Power Down state. PHYCR2@@IRXPDX2	*/
	E_IM_PRO_SLVS_PD_STATE		lane3_pd_state;		/**< Lane3 Power Down state. PHYCR2@@IRXPDX3	*/
	E_IM_PRO_SLVS_PD_STATE		lane4_pd_state;		/**< Lane4 Power Down state. PHYCR2@@IRXPDX4	*/
	E_IM_PRO_SLVS_PD_STATE		lane5_pd_state;		/**< Lane5 Power Down state. PHYCR2@@IRXPDX5	*/
	E_IM_PRO_SLVS_PD_STATE		lane6_pd_state;		/**< Lane6 Power Down state. PHYCR2@@IRXPDX6	*/
	E_IM_PRO_SLVS_PD_STATE		lane7_pd_state;		/**< Lane7 Power Down state. PHYCR2@@IRXPDX7	*/
} T_IM_PRO_SLVS_PMA_PD;

/** SLVS-EC Common Configuration parameters
*/
typedef struct {
	UCHAR						StandbyLength;		/**< transfer count of Standby Code in Standby Sequence in the range of 18~255. */
	E_IM_PRO_SLVS_SYMBOL_TYPE	StandbySymbolType;	/**< Symbol code type (D/K) following Comma Symbol inside the Standby Code. */
	UCHAR						StandbySymbol;		/**< Symbol following Comma Symbol inside the Standby Code. */
	UCHAR						DeskewInterval;		/**< Deskew Interval (Idle Code continuous transfer count between Deskew Code in Training Sequence) in the range of 20~255. */
	UCHAR						DeskewLength;		/**< Deskew Length (transfer count of Deskew Code in Training Sequence) in the range of 3~255. */
	E_IM_PRO_SLVS_SYMBOL_TYPE	DeskewSymbolType;	/**< Code type (D/K) of the Symbol following Comma Symbol inside the Deskew Code. */
	UCHAR						DeskewSymbol;		/**< Symbol following Comma Symbol inside the Deskew Code. */
	E_IM_PRO_SLVS_SYMBOL_TYPE	IdleSymbolType1;	/**< Code type (D/K) of the Symbol(2nd) of the Idle Code. */
	UCHAR						IdleSymbol1;		/**< Symbol(2nd) of idle code.  */
	E_IM_PRO_SLVS_SYMBOL_TYPE	IdleSymbolType0;	/**< Code type (D/K) of the Symbol(1st) of the Idle Code. */
	UCHAR						IdleSymbol0;		/**< Symbol(1st) of Idle Code */
	E_IM_PRO_SLVS_SYMBOL_TYPE	IdleSymbolType3;	/**< Code type (D/K) of the Symbol (4th) of the Idle Code. */
	UCHAR						IdleSymbol3;		/**< Symbol(4th) of idle code. */
	E_IM_PRO_SLVS_SYMBOL_TYPE	IdleSymbolType2;	/**< Code type (D/K) of the Symbol (3rd) of the Idle Code. */
	UCHAR						IdleSymbol2;		/**< Symbol (3rd) of Idle Code */
} T_IM_PRO_SLVS_COMMON_CFG;

/** SLVS-EC Control Information
*/
typedef struct {
	E_IM_PRO_SLVS_PD_STATE		ram_pd_state;	/**< External RAM(Line Buffer) Power Down state.  RAMPD@@RAMPDX	*/
	E_IM_PRO_SLVS_VDOUT_CTRL	vdout_ctrl;		/**< Control the VDOUT output when frame stop is set. TRGR@@VDEN	*/
	E_IM_PRO_SLVS_ECCCRC_OPT	ecccrc_opt;		/**< ECC option / CRC Option of payload. CFGR0@@ECCCRC	*/
	E_IM_PRO_SLVS_EXPECT_VAL	expect_val;		/**< Expected value when Line Number Error occurred. MMODE@@LNMAXCLP	*/
} T_IM_PRO_SLVS_CTRL;

/** Mode Configuration parameters
*/
typedef struct {
	E_IM_PRO_SLVS_PIXEL_BIT					pixelbit;												/**< SLVS number of bits per pixel. CFGR0@@PixelBit */
	E_IM_PRO_SLVS_LANE_NUM					lanenum;												/**< SLVS number of valid Lanes. CFGR0@@LaneNum */
	USHORT									linelength;												/**< SLVS number of bits per pixel(4 pixel boundary). CFGR1@@LineLength */
	E_IM_PRO_SLVS_OUT_FORMAT				output_format;											/**< SLVS output format of the image data interface. OUTMD@@OUTMD */
	E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL	swap_lane[E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_MAX];	/**< SLVS swap lane configuration. LSWAP@@LxOUT<br>
																												@image html im_pro_slvs_swap.png	*/
	T_IM_PRO_SLVS_VALID_LANE_SEL			valid_lane;												/**< SLVS valid Lane select. VLANE@@LANEx */
} T_IM_PRO_SLVS_MODE_CFG;

/** SLVS-EC Mode Trimming parameters
*/
typedef struct {
	E_IM_PRO_SLVS_TRIM_ENABLE	enabled;		/**< SLVS trimming mode enabled. OUTMD@@TRMG */
	USHORT						start_x;		/**< SLVS trimming start X position(16 pixel boundary). HSADD@@HSADD */
	USHORT						width;			/**< SLVS trimming width(16 pixel boundary). HSIZE@@HSIZE */
} T_IM_PRO_SLVS_TRIM_CFG;

/** SLVS-EC status
*/
typedef struct {
	E_IM_PRO_SLVS_STATUS_EBD	embedded_data_detect;			/**< indicate whether Embedded Data is detected in the frame. EBDST@@EBD */
	USHORT						ebd_stored_line_num;			/**< indicate the number of lines in which Embedded Data is stored. EBDST@@LNUM */
	USHORT						expect_value_discontinuity;		/**< indicate the expected value of the number of lines in which discontinuity is detected. LNEST@@EXPLN */
	USHORT						line_num_discontinuity;			/**< indicate the number of lines in which discontinuity is detected. LNEST@@DETLN */
} T_IM_PRO_SLVS_STATUS;

/*	@}*/	// im_pro_sen_slvs

/** @addtogroup im_pro_sen_lvds LVDS
@{
*/

/** LVDS Control Information
*/
typedef struct {
	E_IM_PRO_LVDS_INPUT			trans_mode;			/**< LVDS input mode<br>
														target registor	:@@LMD */
	E_IM_PRO_LVDS_OUT_CH		output_ch_no;		/**< LVDS sensor number of output channel<br>
														target registor	:@@LCH */
	E_IM_PRO_LVDS_LANE			lane;				/**< LVDS use lane number<br>
														target registor	:@@LANE */
	E_IM_PRO_LVDS_BIT_WIDTH		data_bit_width;		/**< LVDS output data bit width<br>
														target registor	:@@LDW */
	E_IM_PRO_LVDS_SFT			lsft;				/**< LVDS Output data bit shift<br>
														target registor	:@@LSFT */
	E_IM_PRO_LVDS_SYNC_TYPE		sync_type;			/**< LVDS synchronous code detection type<br>
														target registor	:@@SYNT */
	E_IM_PRO_LVDS_INPUT_TYPE	data_input_type;	/**< LVDS Input size type.<br>
														target registor	:@@SZEN */
	E_IM_PRO_LVDS_SYNC_BIT		sync_bit_fix;		/**< LVDS Sync code bit 16bit fix<br>
														target registor	:@@SYN16 */
	E_IM_PRO_LVDS_DIV_OUT		divide_output;		/**< LVDS divide output for use 1ch sensor<br>
														target registor	:@@DIVOT */
	E_IM_PRO_LVDS_INVERTED		inverted_output;	/**< LVDS Inverted output for use 1ch sensor<br>
														target registor	:@@LMIR */
	E_IM_PRO_LVDS_OUTPUT_MODE	output_mode;		/**< LVDS synchronous code detection mode.<br>
														target registor	:@@DOLMD */
	USHORT						lane_mask;			/**< LVDS sync code mask setting<br>
														value range		:[0x00 - 0xFFFF]<br>
														target registor	:@@LMSK */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp0;	/**< LVDS select serial clock for SP0<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp1;	/**< LVDS select serial clock for SP1<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp2;	/**< LVDS select serial clock for SP2<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp3;	/**< LVDS select serial clock for SP3<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp4;	/**< LVDS select serial clock for SP4<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp5;	/**< LVDS select serial clock for SP5<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp6;	/**< LVDS select serial clock for SP6<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNCLK_SEL		clock_select_sp7;	/**< LVDS select serial clock for SP7<br>
														target registor	:@@LVDSCLKS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp0_0;	/**< LVDS select serial data for SP0 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp0_1;	/**< LVDS select serial data for SP0 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp1_0;	/**< LVDS select serial data for SP1 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp1_1;	/**< LVDS select serial data for SP1 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp2_0;	/**< LVDS select serial data for SP2 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp2_1;	/**< LVDS select serial data for SP2 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp3_0;	/**< LVDS select serial data for SP3 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp3_1;	/**< LVDS select serial data for SP3 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp4_0;	/**< LVDS select serial data for SP4 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp4_1;	/**< LVDS select serial data for SP4 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp5_0;	/**< LVDS select serial data for SP5 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp5_1;	/**< LVDS select serial data for SP5 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp6_0;	/**< LVDS select serial data for SP6 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp6_1;	/**< LVDS select serial data for SP6 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp7_0;	/**< LVDS select serial data for SP7 data 0<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SNDATA_SEL	data_select_sp7_1;	/**< LVDS select serial data for SP7 data 1<br>
														target registor	:@@LVDSDATS */
	E_IM_PRO_LVDS_SYNC_SEL		lane_select;		/**< LVDS sync code lane select<br>
														target registor	:@@SYNCSEL */
} T_IM_PRO_LVDS_CTRL;

/** LVDS Lane select information */
typedef struct {
	E_IM_PRO_LVDS_PIXEL_POS		ptn_a:3;	/**< LVDS output line A<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELA */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_b:3;	/**< LVDS output line B<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELB */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_c:3;	/**< LVDS output line C<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELC */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_d:3;	/**< LVDS output line D<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor :@@SELD */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_e:3;	/**< LVDS output line E<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELE */
	USHORT	dummy01:1;						/**< Not used */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_f:3;	/**< LVDS output line F
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELF */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_g:3;	/**< LVDS output line G 
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELG */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_h:3;	/**< LVDS output line H<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELH */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_i:3;	/**< LVDS output line I<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELI */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_j:3;	/**< LVDS output line J<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELJ */
	USHORT	dummy02:1;						/**< Not used */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_k:3;	/**< LVDS output line K<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELK */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_l:3;	/**< LVDS output line L<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELL */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_m:3;	/**< LVDS output line M<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELM */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_n:3;	/**< LVDS output line N<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELN */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_o:3;	/**< LVDS output line O<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELO */
	USHORT	dummy03:1;						/**< Not used */
	E_IM_PRO_LVDS_PIXEL_POS		ptn_p:3;	/**< LVDS output line P<br>
												value range		:See @ref E_IM_PRO_LVDS_PIXEL_POS<br>
												target registor	:@@SELP */
	USHORT	dummy04:13;						/**< Not used */
} T_IM_PRO_LVDS_PIXEL_ORDER;

/** LVDS Sync Code Information */
typedef struct {
	USHORT	sav[4];							/**< Effective pixel start(SAV)<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@SAV_0 - @@SAV_3 */
	USHORT	eav[4];							/**< Effective pixel end(EAV)<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@EAV_0 - @@EAV_3 */
	USHORT	sab[4];							/**< V blank start(SAB)<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@SAB_0 - @@SAB_3 */
	USHORT	eab[4];							/**< V blank end(EAB)<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@EAB_0 - @@EAB_3 */
} T_IM_PRO_LVDS_SYNC_CODE;

/** LVDS Sync Code Control */
typedef struct {
	T_IM_PRO_LVDS_SYNC_CODE	sync_code[ E_IM_PRO_LVDS_OUTPUT_MODE_MAX ];		/**< the synchronous code of LVDS.<br>
																				LVDS can set up to 3 groups of synchronous code and can output the data with<br>
																				different synchronous codes as the data of different frames separately.	*/
} T_IM_PRO_LVDS_SYNC_CODE_CTRL;

/** LVDS Sync Code Mask Information */
typedef struct {
	USHORT	msav[4];						/**< SAV Mask bit<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@MSAV_0 - @@MSAV_3 */
	USHORT	meav[4];						/**< EAV Mask bit<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@MEAV_0 - @@MEAV_3 */
	USHORT	msab[4];						/**< SAB Mask bit<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@MSAB_0 - @@MSAB_3 */
	USHORT	meab[4];						/**< EAB Mask bit<br>
												value range		:[0x0000 - 0xFFFF]<br>
												target registor	:@@MEAB_0 - @@MEAB_3 */
} T_IM_PRO_LVDS_SYNC_MASK;

/** LVDS Sync Code Mask Control */
typedef struct {
	T_IM_PRO_LVDS_SYNC_MASK	sync_mask[ E_IM_PRO_LVDS_OUTPUT_MODE_MAX ];		/**< exclude the specific bit from comparison object in the synchronous code match detection processing.	*/
} T_IM_PRO_LVDS_SYNC_MASK_CTRL;

/** SOL/EOL detection interrupt vertical address. */
typedef struct {
	USHORT sol_addr;						/**< SOL detection interrupt vertical address.<br>
												 value range:[0 - 8960]<br>
												 target registor:@@SOLVADR */
	USHORT eol_addr;						/**< EOL detection interrupt vertical address.<br>
												 value range:[0 - 8960]<br>
												 target registor:@@EOLVADR */
} T_IM_PRO_LVDS_INT_ADDR;

/** SOL/EOL detection interrupt vertical address Control */
typedef struct {
	T_IM_PRO_LVDS_INT_ADDR	int_addr[ E_IM_PRO_LVDS_OUTPUT_MODE_MAX ];		/**< SOL/EOL detection interrupt vertical address.	*/
} T_IM_PRO_LVDS_INT_ADDR_CTRL;


/*	@}*/	// im_pro_sen_lvds


/** @addtogroup im_pro_sen_dmipi	MIPI-DPHY
@{
*/

/** MIPI-DPHY Information.
*/
typedef struct {
	E_IM_PRO_DMIPI_RXSYNCMODE		rxsyncmode;				/**< RxSyncMode value setting.<br>
																target registor	:@@DPSM	*/
	UCHAR							rxsmt_d_val;			/**< value of output data RxSMT_D. Set DPSMTD to 4'h7<br>
																RxSMT_D is an output signal for setting the same phase input mask in the data lane of D-PHY.<br>
																target registor	:@@DPSMTD	*/
	UCHAR							rxsmt_c_val;			/**< value of output data RxSMT_C. Set DPSMTD to 4'hE<br>
																RxSMT_C is an output signal for setting the same phase input mask in the clock lane of D-PHY.<br>
																target registor	:@@DPSMTC	*/
	E_IM_PRO_DMIPI_RXENABLE			rxenable;				/**< RxEnable value setting.<br>
																target registor	:@@DPEN	*/
} T_IM_PRO_DMIPI_PHY_CTRL;

/** MIPI-DPHY Configuration.
*/
typedef struct {
	E_IM_PRO_DMIPI_LANE				lane_sel;				/**< Number of input lanes selection.<br>
																target registor	:@@LANE	*/
	E_IM_PRO_DMIPI_VC				vc_mode_sel[2];			/**< select the Virtual Channel value for the packets output to the OUT0/1 side.<br>
																target registor	:@@VCOUT0/1	*/
	E_IM_PRO_DMIPI_IN_DATA_MODE		input_data_mode[2];		/**< select the input data mode.<br>
																target registor	:@@DTMD0/1	*/
	E_IM_PRO_DMIPI_OUT_MODE			output_data_mode[2];	/**< Output data mode selection.<br>
																target registor	:@@OUTMD0/1	*/
	E_IM_PRO_DMIPI_DT_OUT			output_dt_sel[2];		/**< Output DT selection mode.<br>
																target registor	:@@DTCOMD0/1	*/
	UCHAR							output_dt_setting[2];	/**< DataType value for the packets output to the OUT0/1 side.<br>
																target registor	:@@DTCO0/1	*/
	T_IM_PRO_DMIPI_PHY_CTRL			phy_ctrl;				/**< MIPI D-PHY control. */
	E_IM_PRO_DMIPI_SEL_CYCLE		out_cycle[2];			/**< Setting the cycle for OUT0/1 side.<br>
																target registor	:@@HDPDSEL0/1	*/
} T_IM_PRO_DMIPI_CTRL;

/** MIPI-DPHY Status Information.
*/
typedef struct {
	ULONG							input_signal_status;		/**< status of the signals input to D-PHY.<br>
																target registor	:@@MPIDPVR	*/
	ULONG							skew_calibration_status;	/**< status of the skew calibration to D-PHY.<br>
																target registor	:@@MPISCS	*/
} T_IM_PRO_DMIPI_STATUS;

/*	@}*/	// im_pro_sen_dmipi


/** MIPI-CPHY Information.<br>
	detail of this setting value, please refer to C-PHY macro specifications.
*/
typedef struct {
	E_IM_PRO_CMIPI_RXENABLE			rxenable;				/**< RxEnable value setting.				target registor	:@@CPEN	*/
	UCHAR							hs_settlecnt;			/**< Settle count setting.					target registor	:@@CP_HS_SETTLECNT	*/
	UCHAR							hs_stopcnt;				/**< Stop count setting.					target registor	:@@CP_HS_STOPCNT	*/
	UCHAR							hs_syncmode;			/**< Sync detection mode setting.			target registor	:@@CP_HS_SYNCMODE	*/
	UCHAR							rlr_lconfig;			/**< Lane configuration.					target registor	:@@CP_RLR_LCONFIG	*/
	UCHAR							rlr_tpsel;				/**< Test pattern select.					target registor	:@@CP_RLR_TPSEL		*/
	UCHAR							rlr_prbssp;				/**< PRBS seed value.						target registor	:@@CP_RLR_PRBSSP	*/
	ULLONG							tgr_progseq;			/**< Preamble configuration sequence value.	target registor	:@@CP_TGR_PROGSEQ	*/
	UCHAR							forcerxmode2;			/**< ForceRxmode(lane2) value.				target registor	:@@CP_FORCERXMODE2	*/
	UCHAR							forcerxmode1;			/**< ForceRxmode(lane1) value.				target registor	:@@CP_FORCERXMODE1	*/
	UCHAR							forcerxmode0;			/**< ForceRxmode(lane0) value.				target registor	:@@CP_FORCERXMODE0	*/
} T_IM_PRO_CMIPI_PHY_CTRL;

/** MIPI-CPHY Configuration.
*/
typedef struct {
	E_IM_PRO_CMIPI_LANE				lane_sel;				/**< Number of input lanes selection.<br>
																target registor	:@@LANE	*/
	E_IM_PRO_CMIPI_VC				vc_mode_sel[2];			/**< select the Virtual Channel value for the packets output to the OUT0/1 side.<br>
																target registor	:@@VCOUT0/1	*/
	E_IM_PRO_CMIPI_IN_DATA_MODE		input_data_mode[2];		/**< select the input data mode.<br>
																target registor	:@@DTMD0/1	*/
	E_IM_PRO_CMIPI_OUT_MODE			output_data_mode[2];	/**< Output data mode selection.<br>
																target registor	:@@OUTMD0/1	*/
	E_IM_PRO_CMIPI_DT_OUT			output_dt_sel[2];		/**< Output DT selection mode.<br>
																target registor	:@@DTCOMD0/1	*/
	UCHAR							output_dt_setting[2];	/**< DataType value for the packets output to the OUT0/1 side.<br>
																target registor	:@@DTCO0/1	*/
	E_IM_PRO_CMIPI_SEL_CYCLE		out_cycle[2];			/**< Setting the cycle for OUT0/1 side.<br>
																target registor	:@@HDPDSEL0/1	*/
	T_IM_PRO_CMIPI_PHY_CTRL			phy_ctrl;				/**< MIPI C-PHY control. */
} T_IM_PRO_CMIPI_CTRL;

/** MIPI-CPHY Status Information.
*/
typedef struct {
	UCHAR							rxstopstate_l0;			/**< RxStopstate_L0 signal status.	target registor	:@@SSL0				*/
	UCHAR							rxstopstate_l1;			/**< RxStopstate_L1 signal status.	target registor	:@@SSL1				*/
	UCHAR							rxstopstate_l2;			/**< RxStopstate_L2 signal status.	target registor	:@@SSL2				*/
	UCHAR							lstatus0;				/**< Lane0 status.					target registor	:@@RLR_LSTATUS0		*/
	UCHAR							lstatus1;				/**< Lane1 status.					target registor	:@@RLR_LSTATUS1		*/
	UCHAR							lstatus2;				/**< Lane2 status.					target registor	:@@RLR_LSTATUS2		*/
	UCHAR							werrcnt0;				/**< Lane0 Word error count.		target registor	:@@RLR_WERRCNT0		*/
	UCHAR							werrcnt1;				/**< Lane1 Word error count.		target registor	:@@RLR_WERRCNT1		*/
	UCHAR							werrcnt2;				/**< Lane2 Word error count.		target registor	:@@RLR_WERRCNT2		*/
	ULLONG							wcnt0;					/**< Lane0 Word count.				target registor	:@@RLR_WCNT0		*/
	ULLONG							wcnt1;					/**< Lane1 Word count.				target registor	:@@RLR_WCNT1		*/
	ULLONG							wcnt2;					/**< Lane2 Word count.				target registor	:@@RLR_WCNT2		*/
	UCHAR							syerrcnt0;				/**< Lane0 Symbol error count.		target registor	:@@RLR_SYERRCNT0	*/
	UCHAR							syerrcnt1;				/**< Lane1 Symbol error count.		target registor	:@@RLR_SYERRCNT1	*/
	UCHAR							syerrcnt2;				/**< Lane2 Symbol error count.		target registor	:@@RLR_SYERRCNT2	*/
	ULLONG							syerrloc0;				/**< Lane0 error location.			target registor	:@@RLR_SYERRLOC0	*/
	ULLONG							syerrloc1;				/**< Lane1 error location.			target registor	:@@RLR_SYERRLOC1	*/
	ULLONG							syerrloc2;				/**< Lane2 error location.			target registor	:@@RLR_SYERRLOC2	*/
} T_IM_PRO_CMIPI_STATUS;


/** @addtogroup im_pro_common_lnr LNR
@{
*/

/** LNR Information.
*/
typedef struct {

	USHORT	lnr_br[7];			/**< Boundary value for the pixel LNR R<br>
									value range		:[0 - 16383]
									Limit :LNRBR[1]<=LNRBR[2]<=LNRBR[3]...<=LNRBR[7]
									target registor	:@@LNRBR[1] - @@LNRBR[7]		*/

	USHORT	lnr_incr[8];		/**< Slope setting for pixel LNR R<br>
									value range		:[0 - 1023(2-bit integer part,8-bit fractional part)]
									target registor	:@@LNRINCR[0] - @@LNRINCR[7]	*/

	USHORT	lnr_ofsr[8];		/**< Pixel offset setting for LNR R<br>
									value range		:[0 - 16383]
									target registor	:@@LNROFSR[0] - @@LNROFSR[7]	*/

	USHORT	lnr_bgr[7];			/**< Boundary value for the pixel LNR Gr<br>
									value range		:[0 - 16383]
									Limit :LNRBGR[1]<=LNRBGR[2]<=LNRBGR[3]...<=LNRBGR[7]
									target registor	:@@LNRBGR[1] - @@LNRBGR[7]		*/

	USHORT	lnr_incgr[8];		/**< Slope setting for pixel LNR Gr<br>
									value range		:[0 - 1023(2-bit integer part,8-bit fractional part)]
									target registor	:@@LNRINCGR[0] - @@LNRINCGR[7]	*/

	USHORT	lnr_ofsgr[8];		/**< Pixel offset setting for LNR Gr<br>
									value range		:[0 - 16383]
									target registor	:@@LNROFSGR[0] - @@LNROFSGR[7]	*/

	USHORT	lnr_bgb[7];			/**< Boundary value for the pixel LNR Gb<br>
									value range		:[0 - 16383]
									Limit :LNRBGB[1]<=LNRBGB[2]<=LNRBGB[3]...<=LNRBGB[7]
									target registor	:@@LNRBGB[1] - @@LNRBGB[7]		*/

	USHORT	lnr_incgb[8];		/**< Slope setting for pixel LNR Gb<br>
									value range		:[0 - 1023(2-bit integer part,8-bit fractional part)]
									target registor	:@@LNRINCGB[0] - @@LNRINCGB[7]	*/

	USHORT	lnr_ofsgb[8];		/**< Pixel offset setting for LNR Gb<br>
									value range		:[0 - 16383]
									target registor	:@@LNROFSGB[0] - @@LNROFSGB[7]	*/

	USHORT	lnr_bb[7];			/**< Boundary value for the pixel LNR B<br>
									value range		:[0 - 16383]
									Limit :LNRBB[1]<=LNRBB[2]<=LNRBB[3]...<=LNRBB[7]
									target registor	:@@LNRBB[1] - @@LNRBB[7]		*/

	USHORT	lnr_incb[8];		/**< Slope setting for pixel LNR B<br>
									value range		:[0 - 1023(2-bit integer part,8-bit fractional part)]
									target registor	:@@LNRINCB[0] - @@LNRINCB[7]	*/

	USHORT	lnr_ofsb[8];		/**< Pixel offset setting for LNR B<br>
									value range		:[0 - 16383]
									target registor	:@@LNROFSB[0] - @@LNROFSB[7]	*/

} T_IM_PRO_LNR_CTRL;

/*	@}*/	// im_pro_common_lnr

/** @addtogroup im_pro_sen_obt OBT
@{
*/
/** TOP OB Control information
*/
typedef struct {
	E_IM_PRO_OBT_ENABLE				obt_enable;			/**< OBT Correction enable<br>
															target registor	:@@OBTEN<br>
															remarks :Even if value set to 0(disable), OBT Detection performed. */
	E_IM_PRO_OBT_MODE				mode;				/**< TopOB Detection mode selection<br>
															target registor	:@@OBTMD */
	E_IM_PRO_OBT_SELECT				sel;				/**< Top OB Correction select<br>
															target registor	:@@OBTSL */
	E_IM_PRO_OBT_INTERCOMM_ENABLE	intercomm_enable;	/**< OBT Intercommunication Enable/Disable<br>
															target registor	:@@OBTCEN */
	USHORT							ob_max_value;		/**< OB Input upper threshold value<br>
															value range		:[0 - 16383]<br>
															target registor	:@@OBTHTHBIT */
	USHORT							ob_min_value;		/**< OB Input lower threshold value<br>
															value range		:[0 - 16383]<br>
															target registor	:@@OBTLTHBIT */
} T_IM_PRO_OBT_CTRL;

/*	@}*/	// im_pro_sen_obt

/** @addtogroup im_pro_common_pzsft PZSFT
@{
*/

/** PZSFT Control information
*/
typedef struct {
	SHORT			offset_shift;	/**< input FF zero shift value<br>
										value range		:[-32768 - +32767]<br>
										target registor	:@@PZOFST	*/
} T_IM_PRO_PZSFT_CTRL;

/*	@}*/	// im_pro_common_pzsft


/** @addtogroup im_pro_sro_ffsh FFSH
@{
*/

/** FFSH Blend Control information
*/
typedef struct {
	SHORT							frame_correction_blend_0;			/**< Frame correction expansion blend ratio 0<br>
																			value range		:[-512 - +511] <br>
																			target registor	:@@FFSHBC0 */
	SHORT							frame_correction_blend_1;			/**< Frame correction expansion blend ratio 1<br>
																			value range		:[-512 - +511] <br>
																			target registor	:@@FFSHBC1 */
} T_IM_PRO_FFSH_BLEND_CTRL;

/** FFSH Control information
*/
typedef struct {
	USHORT							h_size_correction_coeff;			/**< horizontal data size of the correction coefficient of the frame correction expansion processing in units of 2 pixels.<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
																			value range		:[4 - 314]<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
																			value range		:[4 - 366]<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
																			target registor	:@@FFSHSUBPRH */
	USHORT							v_size_correction_coeff;			/**< vertical data size of the correction coefficient of the frame correction expansion processing in units of 2 pixels.<br>
																			value range		:[4 - 1122]<br>
																			target registor	:@@FFSHSUBPRV */
	USHORT							h_pixel_unit_interpolation;			/**< horizontal pixel unit in units of 2 pixels when performing bilinear interpolation for the correction coefficient data in the frame correction expansion processing.<br>
																			value range		:[16 - 724]<br>
																			target registor	:@@FFSHSUBSPH */
	USHORT							v_pixel_unit_interpolation;			/**< vertical pixel unit in units of 2 pixels when performing bilinear interpolation for the correction coefficient data in the frame correction expansion processing.<br>
																			value range		:[16 - 724<br>
																			target registor	:@@FFSHSUBSPV */
	USHORT							rad_inverse_coeff_interpolation;	/**< radix part of the reciprocal number of the divisor which is used in the bilinear interpolation for the correction coefficient data in the frame correction expansion processing.<br>
																			value range		:[0(=1.0) - 65535(=1.9999847412109375)]<br>
																			target registor	:@@FFSHSUBDRM */
	UCHAR							exp_inverse_coeff_interpolation;	/**< exponential part of the reciprocal number of the divisor which is used in the bilinear interpolation for the correction coefficient data in the frame correction expansion processing.<br>
																			value range		:[0 - 11]<br>
																			target registor	:@@FFSHSUBDRE */
	USHORT							h_start_pos_coeff;					/**< horizontal start position of the expanded upper left grid in units of 2 pixels in the frame correction expansion processing.<br>
																			value range		:[0 - 722]<br>
																			target registor	:@@FFSHSUBSTH */
	USHORT							v_start_pos_coeff;					/**< vertical start position of the expanded upper left grid in units of 2 pixels in the frame correction expansion processing.<br>
																			value range		:[0 - 722]<br>
																			target registor	:@@FFSHSUBSTV */
} T_IM_PRO_FFSH_CTRL;

/** FFSH Area Control information
*/
typedef struct {
	T_IM_PRO_AREA_INFO		ffsh_area;		/**< FFSH Window information<br>
												value range  :pos_x[0 - 4095]<br>
												target registor  :@@FFSHH<br><br>
												value range  :pos_y[2 - 8960]<br>
												target registor  :@@FFSHV<br><br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
												value range  :width[4 - 2496] 2pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
												value range  :width[4 - 2900] 2pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
												target registor  :@@FFSHHW<br><br>
												value range  :lines[4 - 8960] 2pixel boundary<br>
												target registor  :@@FFSHVW */
} T_IM_PRO_FFSH_AREA;

/*	@}*/	// im_pro_sro_ffsh


/** @addtogroup im_pro_sro_ff FF
@{
*/

/** FF Control information
*/
typedef struct {
	USHORT ffdefts;				/**< threshold to perform white noise determination of the input image during the frame filter processing.<br>
									value range :[0 - 16383]<br>
									target registor :@@FFDEFTS	*/
	SHORT mnwgt;				/**< Main line input fixed coefficient.<br>
									value range :[-512 - 511]<br>
									target registor :@@FFMNWGT			*/
	SHORT sbwgt;				/**< FF input fixed coefficient.<br>
									value range :[-512 - 511]<br>
									target registor :@@FFSBWGT			*/
} T_IM_PRO_FF_CTRL;

/** FF LPF Control information.<br>
Set these 3 registers so that they can meet the following condition.<br>
Constraint: FFLPB0 + 2*(FFLPB1 + FFLPB2) = 128
*/
typedef struct {
	UCHAR	b0;					 /**< Frame filter coefficient b0 for LPF<br>
									value range :[0 - 255]<br>
									target registor :@@FFLPB0	*/
	UCHAR	b1;					 /**< Frame filter coefficient b1 for LPF<br>
									value range :[0 - 255]<br>
									target registor :@@FFLPB1	*/
	UCHAR	b2;					 /**< Frame filter coefficient b2 for LPF<br>
									value range :[0 - 255]<br>
									target registor :@@FFLPB2	*/
} T_IM_PRO_FF_LPF_CTRL;

/*	@}*/	// im_pro_sro_ff


/** @addtogroup im_pro_sro_sdc SDC
@{
*/

/** SDC Control information
*/
typedef struct {
	E_IM_PRO_SDC_MODE					sdc_mode;			/**< Defect correction mode select.<br>
																target registor	:@@SDCMD	*/
	E_IM_PRO_SDC_AFPB_OUTPUT_PTN		afpb_output_ptn;	/**< Phase difference AF pixel pattern output setting.<br>
																target registor	:@@SDCPTO	*/
	E_IM_PRO_SDC_MONOCHROME_MODE		monochrome_mode;	/**< Defect correction monochrome mode select.<br>
																target registor	:@@SDCMONO	*/
	E_IM_PRO_SDC_AFPB_ENABLE			afpb_enabled;		/**< Phase difference AF pixel pattern output enable.<br>
																target registor	:@@SDCPDAFEN	*/
} T_IM_PRO_SDC_CTRL;

/** SDC AFPB Control information
*/
typedef struct {
	ULLONG						h_pattern0[9];							/**< horizontal pattern0  of AF generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 0xFFFF_FFFF_FFFF_FFFF]<br>
																			target registor	:@@SDCPH0[0]~0[8] */
	ULLONG						h_pattern1[9];							/**< horizontal pattern1  of AF generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 0xFFFF_FFFF_FFFF_FFFF]<br>
																			target registor	:@@SDCPH1[0]~1[8] */
	UCHAR						h_pattern0_valid_width[9];				/**< horizontal pattern0 valid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 64]<br>
																			target registor	:@@SDCPHW0[0]~0[8] */
	UCHAR						h_pattern1_valid_width[9];				/**< horizontal pattern1 valid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 64]<br>
																			target registor	:@@SDCPHW1[0]~1[8] */
	USHORT						h_pattern0_r1_valid_width[9];			/**< horizontal pattern0 repetition1 valid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP1HW0[0]~0[8] */
	USHORT						h_pattern1_r1_valid_width[9];			/**< horizontal pattern1 repetition1 valid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP1HW1[0]~1[8] */
	USHORT						h_pattern0_r1_invalid_width[9];			/**< horizontal pattern0 repetition1 invalid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCN1HW0[0]~0[8] */
	USHORT						h_pattern1_r1_invalid_width[9];			/**< horizontal pattern1 repetition1 invalid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCN1HW1[0]~1[8] */
	USHORT						h_pattern0_r2_valid_width[9];			/**< horizontal pattern0 repetition2 valid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP2HW0[0]~0[8] */
	USHORT						h_pattern1_r2_valid_width[9];			/**< horizontal pattern1 repetition2 valid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP2HW1[0]~1[8] */
	USHORT						h_pattern0_r2_invalid_width[9];			/**< horizontal pattern0 repetition2 invalid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP2HW0[0]~0[8] */
	USHORT						h_pattern1_r2_invalid_width[9];			/**< horizontal pattern1 repetition2 invalid width of the phase difference AF pixel information generation pattern in units of 1 pixel.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP2HW1[0]~1[8] */
	UCHAR						h_column_pattern0[64];					/**< column pattern0 of the horizontal pattern of the phase difference AF pixel information generation pattern.<br>
																			value range		:[0 - 8]<br>
																			target registor	:@@SDCPC0[0]~0[63] */
	UCHAR						h_column_pattern1[64];					/**< column pattern1 of the horizontal pattern of the phase difference AF pixel information generation pattern.<br>
																			value range		:[0 - 8]<br>
																			target registor	:@@SDCPC1[0]~1[63] */
	UCHAR						h_column_pattern_valid_width0;			/**< column pattern valid width0 of the horizontal pattern of the phase difference AF pixel information generation pattern.<br>
																			value range		:[0 - 64]<br>
																			target registor	:@@SDCPCW0 */
	UCHAR						h_column_pattern_valid_width1;			/**< column pattern valid width1 of the horizontal pattern of the phase difference AF pixel information generation pattern.<br>
																			value range		:[0 - 64]<br>
																			target registor	:@@SDCPCW1 */
	USHORT						h_column_pattern1_valid_width0;			/**< column pattern1 valid width0 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP1CW0 */
	USHORT						h_column_pattern1_valid_width1;			/**< column pattern1 valid width1 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP1CW1 */
	USHORT						h_column_pattern1_invalid_width0;		/**< column pattern1 invalid width0 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCN1CW0 */
	USHORT						h_column_pattern1_invalid_width1;		/**< column pattern1 invalid width1 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCN1CW1 */
	USHORT						h_column_pattern2_valid_width0;			/**< column pattern2 valid width0 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP2CW0 */
	USHORT						h_column_pattern2_valid_width1;			/**< column pattern2 valid width1 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCP2CW1 */
	USHORT						h_column_pattern2_invalid_width0;		/**< column pattern2 invalid width0 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCN2CW0 */
	USHORT						h_column_pattern2_invalid_width1;		/**< column pattern2 invalid width1 of the horizontal pattern of the phase difference AF pixel information generation pattern in units of 1 column.<br>
																			value range		:[0 - 8191]<br>
																			target registor	:@@SDCN2CW1 */
	USHORT						afpb_gain_correction_u_threshold;		/**< judgment threshold (upper-limit) of the values used in the gain correction when performing the surrounding pixel gain correction of the phase difference AF pixel.<br>
																			value range		:[0 - 4095]<br>
																			target registor	:@@SDCGMAX */
	USHORT						af_gain_correction_l_threshold;			/**< judgment threshold (lower-limit) of the values used in the gain correction when performing the surrounding pixel gain correction of the phase difference AF pixel.<br>
																			value range		:[0 - 4095]<br>
																			target registor	:@@SDCGMIN */
	UCHAR						afpb_neighbor_pixel_correction_level;	/**< When the number of the phase difference AF pixels of neighboring pixels on the top, bottom, left and right side is equal to or larger than (SDCNMAX+1), the static defective pixel correction is performed.<br>
																			value range		:[0 - 3]<br>
																			target registor	:@@SDCNMAX */
} T_IM_PRO_SDC_CTRL_AFPB;

/** SDC gain table information
*/
typedef struct {
	USHORT	gain_table[D_IM_PRO_SDC_GAIN_TBL_NUM];	/**< Coefficients for gain correction.<br>
														value range		:[0 - 255(7.96875)]<br>
														target registor	:@@SDC0GD1/2	*/
} T_IM_PRO_SDC_GAIN_TABLE;

/*	@}*/	// im_pro_sro_sdc


/** @addtogroup im_pro_sro_dpc DPC
@{
*/

/** DPC Area information
*/
typedef struct {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	USHORT	g_pos_x;		/**< global Compensation horizontal direction start position<br>
								value range		:[0 - 2496]<br>
								target registor	:@@DPCGH	*/
	USHORT	g_pos_y;		/**< global Compensation vertical direction start position<br>
								value range		:[0 - 8960] 2pixel boundary<br>
								target registor	:@@DPCGV	*/
	USHORT	g_width;		/**< global horizontal size<br>
								value range		:[10 - 2496] 2byte boundary<br>
								target registor	:@@DPCGHW	*/
	USHORT	g_lines;		/**< global vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@DPCGVW	*/
	USHORT	pos_x;			/**< Compensation horizontal direction start position<br>
								value range		:[0 - 2486] 2pixel boundary<br>
								target registor	:@@DPCH		*/
	USHORT	pos_y;			/**< Compensation vertical direction start position<br>
								value range		:[0 - 8950] 2pixel boundary<br>
								target registor	:@@DPCV		*/
	USHORT	width;			/**< horizontal size<br>
								value range		:[10 - 2496] 2byte boundary<br>
								target registor	:@@DPCHW	*/
	USHORT	lines;			/**< vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@DPCVW	*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	USHORT	g_pos_x;		/**< global Compensation horizontal direction start position<br>
								value range		:[0 - 2900]<br>
								target registor	:@@DPCGH	*/
	USHORT	g_pos_y;		/**< global Compensation vertical direction start position<br>
								value range		:[0 - 8960] 2pixel boundary<br>
								target registor	:@@DPCGV	*/
	USHORT	g_width;		/**< global horizontal size<br>
								value range		:[10 - 2900] 2byte boundary<br>
								target registor	:@@DPCGHW	*/
	USHORT	g_lines;		/**< global vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@DPCGVW	*/
	USHORT	pos_x;			/**< Compensation horizontal direction start position<br>
								value range		:[0 - 2890] 2pixel boundary<br>
								target registor	:@@DPCH		*/
	USHORT	pos_y;			/**< Compensation vertical direction start position<br>
								value range		:[0 - 8950] 2pixel boundary<br>
								target registor	:@@DPCV		*/
	USHORT	width;			/**< horizontal size<br>
								value range		:[10 - 2900] 2byte boundary<br>
								target registor	:@@DPCHW	*/
	USHORT	lines;			/**< vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@DPCVW	*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
} T_IM_PRO_DPC_AREA;


/** DPC Control information
*/
typedef struct {
	E_IM_PRO_DPC_MODE					dpc_mode;							/**< Defect correction mode.<br>
																				target registor	:@@DPCMD */
	E_IM_PRO_DPC_THRE_CONV_TO_ZERO		thre_conv_0;						/**< Dynamic defect determination threshold zero conversion for registration defective pixel.<br>
																				target registor	:@@DPCRTZ */
} T_IM_PRO_DPC_CTRL;

/** Alpha blend Control information
*/
typedef struct {
	UCHAR								alpha_blend_transition_width_p;		/**< alpha blend transition width (plus side)<br>
																				value range		:[0 - 15]<br>
																				target registor	:@@DPCBLWP */
	USHORT								alpha_blend_start_pos_p;			/**< alpha blend start position (plus side)<br>
																				value range		:[0 - 16383]<br>
																				target registor	:@@DPCBLSP */
	UCHAR								alpha_blend_transition_width_m;		/**< alpha blend transition width (minus side)<br>
																				value range		:[0 - 15]<br>
																				target registor	:@@DPCBLWM */
	USHORT								alpha_blend_start_pos_m;			/**< alpha blend transition position (minus side)<br>
																				value range		:[0 - 16383]<br>
																				target registor	:@@DPCBLSM */
} T_IM_PRO_DPC_AB_CTRL;

/** Defect Dynamic Detection Control information
*/
typedef struct {
	E_IM_PRO_DPC_DD_GRGB						grgb_independent_setting;				/**< reference mode of Gr/Gb pixel in the defect dynamic detection and pixel value magnitude defect correction.<br>
																							target registor	:@@DFGRGB */
	E_IM_PRO_DPC_DD_MODE_G						g_mode;									/**< dynamic detection mode of G pixel in the defect dynamic detection.<br>
																							target registor	:@@DFMDG */
	E_IM_PRO_DPC_DD_MODE_RB						rb_mode;								/**< dynamic detection mode of R/B pixel in the defect dynamic detection.<br>
																							target registor	:@@DFMDRB */
	UCHAR										upper_limit_Local_registration;			/**< limit value of the registration defective pixel existing in the surrounding of the focus pixel in the defect dynamic detection.<br>
																							value range		:[0 - 6]<br>
																							target registor	:@@DFDCNT */
	E_IM_PRO_DPC_DD_USE_MDD						use_mdd;								/**< Use of mis-determination degree for correction.<br>
																							target registor	:@@DFRBUG */
	E_IM_PRO_DPC_DD_MDD_2CONSECUTIVE_DEFECT		support_2concecutive_defect;			/**< Mis-determination degree for correction  horizontal and vertical 2-concecutive defect support.<br>
																							target registor	:@@DFAGND */
	USHORT										p_threshold_start_pos[3];				/**< value of the start position related to the dynamic defect determination threshold for each R/G/B pixel at the plus side.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DFSRP/GP/BP */
	USHORT										m_threshold_start_pos[3];				/**< value of the start position related to the dynamic defect determination threshold for each R/G/B pixel at the minus side.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DFSRM/GM/BM */
	USHORT										p_threshold_start_offset[3];			/**< offset value related to the dynamic defect determination threshold for each R/G/B pixel at the plus side.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DFORP/GP/BP */
	USHORT										m_threshold_start_offset[3];			/**< offset value related to the dynamic defect determination threshold for each R/G/B pixel at the minus side.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DFORM/GM/BM */
	UCHAR										p_threshold_gain[3];					/**< gain value related to the dynamic defect determination threshold for each R/G/B pixel at the plus side.<br>
																							value range		:[0 - 63(=3.9375)]<br>
																							target registor	:@@DFKRP/GP/BP */
	UCHAR										m_threshold_gain[3];					/**< gain value related to the dynamic defect determination threshold for each R/G/B pixel at the minus side.<br>
																							value range		:[0 - 63(=3.9375)]<br>
																							target registor	:@@DFKRM/GM/BM */
	USHORT										p_threshold_upper_limit[3];				/**< upper limit value of the dynamic defect determination threshold for each R/G/B pixel at the plus side.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DFHRP/GP/BP */
	USHORT										m_threshold_upper_limit[3];				/**< upper limit value of the dynamic defect determination threshold for each R/G/B pixel at the minus side.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DFHRM/GM/BM */

	UCHAR										p_threshold_2consecutive;				/**<  plus side setting value of the threshold used to determine the 2-consecutive defect pixels.<br>
																							value range		:[0 - 63]<br>
																							target registor	:@@DFCSRP */
	UCHAR										m_threshold_2consecutive;				/**< minus side setting value of the threshold used to determine the 2-consecutive defect pixels.<br>
																							value range		:[0 - 63]<br>
																							target registor	:@@DFCSRM */
	USHORT										threshold_2consecutive_bright_pix[3];	/**< setting values of the threshold used to determine the 2-consecutive bright spot pixels.<br>
																							value range		:[0 - 4095]<br>
																							target registor	:@@DFLSTHR/G/B */
} T_IM_PRO_DPC_DDD_CTRL;

/** Calculation of mis-determination degree for correction Control information
*/
typedef struct {
	UCHAR										p_mdd_effect;							/**< parameter of the plus side related to the calculation of mis-determination degree effect for correction.<br>
																							value range		:[0 - 7]<br>
																							target registor	:@@DFAGEFP */
	UCHAR										m_mdd_effect;							/**< parameter of the minus side related to the calculation of mis-determination degree effect for correction.<br>
																							value range		:[0 - 7]<br>
																							target registor	:@@DFAGEFM */
	UCHAR										p_mdd_effect2;							/**< parameter of the plus side related to the calculation of mis-determination degree effect2 for correction.<br>
																							value range		:[0 - 3]<br>
																							target registor	:@@DFAGEFP2 */
	UCHAR										m_mdd_effect2;							/**< parameter of the minus side related to the calculation of mis-determination degree effect2 for correction.<br>
																							value range		:[0 - 3]<br>
																							target registor	:@@DFAGEFM2 */
	USHORT										p_threshold_offset;						/**< offset value of the plus side related to the mis-determination degree threshold for correction.<br>
																							value range		:[0 - 1023]<br>
																							target registor	:@@DFAGTHP */
	USHORT										m_threshold_offset;						/**< offset value of the minus side related to the mis-determination degree threshold for correction.<br>
																							value range		:[0 - 1023]<br>
																							target registor	:@@DFAGTHM */
	UCHAR										p_threshold_gain;						/**< gain value of the plus side related to the mis-determination degree threshold for correction.<br>
																							value range		:[0 - 63(=0.984375)]<br>
																							target registor	:@@DFAGTHKP */
	UCHAR										m_threshold_gain;						/**< gain value of the minus side related to the mis-determination degree threshold for correction.<br>
																							value range		:[0 - 63(=0.984375)]<br>
																							target registor	:@@DFAGTHKM */
} T_IM_PRO_DPC_MDD_CTRL;

/** Pixel value magnitude defect correction Control information
*/
typedef struct {
	E_IM_PRO_DPC_MDC_USE_OCD					use_ocd;								/**< Use of over-correction degree for pixel value magnitude defect correction.<br>
																							target registor	:@@DMGUG */
	USHORT										p_manual_adj_val_single;				/**< amount to soften the correction in single defective pixel correction of pixel value(plus side) magnitude defect correction.<br>
																							value range		:[0 - 1023]<br>
																							target registor	:@@DMWKSP */
	USHORT										m_manual_adj_val_single;				/**< amount to soften the correction in single defective pixel correction of pixel value(minus side) magnitude defect correction.<br>
																							value range		:[0 - 1023]<br>
																							target registor	:@@DMWKSM */
	USHORT										p_manual_adj_val_2consecutive;			/**< amount to soften the correction in 2-concecutive defective pixel correction of pixel value(plus side) magnitude defect correction.<br>
																							value range		:[0 - 1023]<br>
																							target registor	:@@DMWKDP */
	USHORT										m_manual_adj_val_2consecutive;			/**< amount to soften the correction in 2-concecutive defective pixel correction of pixel value(minus side) magnitude defect correction.<br>
																							value range		:[0 - 1023]<br>
																							target registor	:@@DMWKDM */
	UCHAR										p_correction_level_on_ocd;				/**< correction level (the degree to soften the correction) based on the over-correction degree for correction in the pixel value(plus side) magnitude defect correction.<br>
																							value range		:[0 - 63(=3.9375)]<br>
																							target registor	:@@DMAGKP */
	UCHAR										m_correction_level_on_ocd;				/**< correction level (the degree to soften the correction) based on the over-correction degree for correction in the pixel value(minus side) magnitude defect correction.<br>
																							value range		:[0 - 63(=3.9375)]<br>
																							target registor	:@@DMAGKM */
} T_IM_PRO_DPC_MDC_CTRL;

/** Edge strage defect correction Control information
*/
typedef struct {
	E_IM_PRO_DPC_ESD_MODE						esd_mode;								/**< reference mode of Gr/Gb pixel in the edge storage defect correction.<br>
																							target registor	:@@DEGRGB */
	E_IM_PRO_DPC_ESD_USE_RB_JUDGE				use_rb_jdge;							/**< Use of G pixel in RB judgment of edge storage defect correction.<br>
																							target registor	:@@DERBUG */
	E_IM_PRO_DPC_ESD_USE_OCD					use_ocd;								/**< Use of G pixel in RB judgment of edge storage defect correction.<br>
																							target registor	:@@DEGUG */
	USHORT										threshold_start_pos;					/**< start position related to the noise determination threshold of the edge storage defect correction.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DENSS */
	USHORT										threshold_offset;						/**< offset related to the noise determination threshold of the edge storage defect correction.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DENSO */
	UCHAR										threshold_gain;							/**< gain related to the noise determination threshold of the edge storage defect correction.<br>
																							value range		:[0 - 255(=3.984375)]<br>
																							target registor	:@@DENSK */
	USHORT										threshold_upper_limit;					/**< upper limit value related to the noise determination threshold of the edge storage defect correction.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DENSH */
	UCHAR										noise_tolerance;						/**< value related to the noise tolerance of the edge storage defect correction.<br>
																							value range		:[1 - 5]<br>
																							target registor	:@@DENSMGN */
	USHORT										threshold_calculation;					/**< threshold used to calculate the correction value in the edge storage defect correction.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DENDCTH */
	UCHAR										p_correction_level_on_ocd;				/**< correction level (the degree to soften the correction) based on the over-correction degree(plus side) for correction in the edge storage defect correction.<br>
																							value range		:[0 - 63(=3.9375)]<br>
																							target registor	:@@DEAGKP */
	UCHAR										m_correction_level_on_ocd;				/**< correction level (the degree to soften the correction) based on the over-correction degree(minus side) for correction in the edge storage defect correction.<br>
																							value range		:[0 - 63(=3.9375)]<br>
																							target registor	:@@DEAGKM */
} T_IM_PRO_DPC_ESDC_CTRL;

/** Calculation of over-correction degree for correction Control information
*/
typedef struct {
	UCHAR										p_ocd_effect;							/**< parameters related to the calculation of over-correction degree(plus side) for correction.<br>They are used both in the pixel value magnitude defect correction and the edge storage defect correction.<br>
																							value range		:[0 - 7]<br>
																							target registor	:@@DCAGEFP */
	UCHAR										m_ocd_effect;							/**< parameters related to the calculation of over-correction degree(minus side) for correction.<br>They are used both in the pixel value magnitude defect correction and the edge storage defect correction.<br>
																							value range		:[0 - 7]<br>
																							target registor	:@@DCAGEFM */
	USHORT										p_threshold;							/**< threshold for over-correction degree(plus side) for correction. They are used both in the pixel value magnitude defect correction and the edge storage defect correction.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DCAGTHP */
	USHORT										m_threshold;							/**< threshold for over-correction degree(minus side) for correction. They are used both in the pixel value magnitude defect correction and the edge storage defect correction.<br>
																							value range		:[0 - 16383]<br>
																							target registor	:@@DCAGTHM */
} T_IM_PRO_DPC_OCD_CTRL;


/*	@}*/	// im_pro_sro_dpc


/** @addtogroup im_pro_b2b_elf ELF
@{
*/

/** ELF Filter control common part
*/
typedef struct {
	E_IM_PRO_ELF_N1S5EN		n1s5_enable			:1;		/**< Reference to the noise component Ns1s5<br>
															target registor	:@@N1S5EN */
	E_IM_PRO_ELF_N5S9EN		n5s9_enable			:1;		/**< Reference to the noise component Ns5s9<br>
															target registor	:@@N5S9EN */
	E_IM_PRO_ELF_NFS9EN		f9_enable			:1;		/**< Byr9 to blend in noise suppression<br>
															target registor	:@@NFS9EN */
	E_IM_PRO_ELF_NLA0EN		a0_enable			:1;		/**< Reference to the noise component Ns9s17<br>
															target registor	:@@NLA0EN */
	USHORT	dummy02								:4;		/**< Not used									*/
	
	E_IM_PRO_ELF_F5_MD		f5_filter_mode		:1;		/**< F5 Filter mode<br>
															target registor	:@@F5MD */
	E_IM_PRO_ELF_F9_MD		f9_filter_mode		:1;		/**< F9 Filter mode<br>
															target registor	:@@F9MD */
	USHORT	dummy03								:12;		/**< Not used									*/
	
	E_IM_PRO_ELF_EFMD		e_filter_mode		:1;		/**< E Filter mode<br>
															target registor	:@@EFMD */
	E_IM_PRO_ELF_LFMD		l_filter_mode		:1;		/**< L Filter mode<br>
															target registor	:@@LFMD */
	USHORT	dummy04								:5;		/**< Not used									*/
	USHORT	dummy07								:3;		/**< Not used									*/
	
} T_IM_PRO_ELF_COMMON1;

/** ELF Filter control common part
*/
typedef struct {
	USHORT	elfl1;		/**< One global horizontal size<br>
							value range		:[0 - 4095] ELFL1<=ELFL2<=ELFL3<=ELFL4<br>
							target registor	:@@ELFL1	*/
	USHORT	elfl2;		/**< Two global horizontal size<br>
							value range		:[0 - 4095]<br>
							target registor	:@@ELFL2	*/
	USHORT	elfl3;		/**< Three global horizontal size<br>
							value range		:[0 - 4095]<br>
							target registor	:@@ELFL3	*/
	USHORT	elfl4;		/**< 4 global horizontal size<br>
							value range		:[0 - 4095]<br>
							target registor	:@@ELFL4	*/
} T_IM_PRO_ELF_COMMON2;


/** PRO ELF noise information
*/
typedef struct {
	USHORT	offset[4];		/**< One common brightness threshold ELF<br>
								value range		:[0 - 4095]
								target registor	:@@ANBOR/@@ANBOG/@@ANBOB	*/
	USHORT	gain[4];		/**< noise table gain<br>
								value range		:[0 - 4095((12-bit fractional part))]
								target registor	:@@ANBKR/@@ANBKG/@@ANBKB	*/
	USHORT	boundary[4];	/**< noise boundary<br>
								value range		:[0 - 4095] 16byte boundary
								target registor	:@@ANBDR/@@ANBDG/@@ANBDB	*/
} T_IM_PRO_ELF_NOISE_FUNC;

/** PRO ELF Area information
*/
typedef struct {
	USHORT	g_width;		/**< global horizontal size<br>
								value range		:[10 - 2496] 2byte boundary<br>
								target registor	:@@ELFGHW	*/
	USHORT	g_lines;		/**< global vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@ELFGVW	*/
	USHORT	g_pos_x;		/**< global Compensation horizontal direction start position<br>
								value range		:[0 - 2496]<br>
								target registor	:@@ELFGH	*/
	USHORT	g_pos_y;		/**< global Compensation vertical direction start position<br>
								value range		:[0 - 8960] 2pixel boundary<br>
								target registor	:@@ELFGV	*/
	USHORT	width;			/**< horizontal size<br>
								value range		:[2 - 2488] 2byte boundary<br>
								target registor	:@@ELFHW	*/
	USHORT	lines;			/**< vertical size<br>
								value range		:[2 - 8952] 2pixel boundary<br>
								target registor	:@@ELFVW	*/
	USHORT	pos_x;			/**< Compensation horizontal direction start position<br>
								value range		:[4 - 2491]<br>
								target registor	:@@ELFH		*/
	USHORT	pos_y;			/**< Compensation vertical direction start position<br>
								value range		:[4 - 8954] 2pixel boundary<br>
								target registor	:@@ELFV		*/
} T_IM_PRO_ELF_AREA;

/** PRO ELF Common Control information
*/
typedef struct {
	E_IM_PRO_ELF_PDD_ENABLE	pix_padding;		/**< input around padding<br>
													target registor	:@@ELFPDD */
	E_IM_PRO_ELF_PDD_MODE	padding_mode;		/**< Pixel padding mode.<br>
													target registor	:@@ELFRNG */
	E_IM_PRO_ELF_ELFAFS		nr_select;			/**< NR AF input selection path<br>
													target registor	:@@ELFAFS */
} T_IM_PRO_ELF_COMMON_CTRL;

/** PRO ELF Control information
*/
typedef struct {
	T_IM_PRO_ELF_COMMON1		cmn1;		/**< ELF Filter control common part 1	*/
	T_IM_PRO_ELF_COMMON2		cmn2;		/**< ELF Filter control common part 2	*/
} T_IM_PRO_ELF_CTRL;

/** NSL Control information
*/
typedef struct {
	E_IM_PRO_ELF_NSLMD		l_noise_mode;			/**< Data mode of the long-wavelength noise data which is input from SDRAM.<br>
															target registor	:@@NSLMD */
	E_IM_PRO_ELF_NSLRES		l_noise_precision;		/**< Specify the accuracy of the long-wavelength noise data which is input from SDRAM.<br>
															target registor	:@@NSLRES */
	E_IM_PRO_ELF_NSLKEN		long_noise_comp;		/**< Compression enable of the long-wavelength noise data which is input from SDRAM.<br>
															target registor	:@@NSLKEN */
} T_IM_PRO_NSL_CTRL;

/** Light/Outlines filter
*/
typedef struct {
	USHORT	zero_point;		/**< Zero point level<br>
								value range		:[0 - 4095] <br>
								target registor	:@@ELFZP	*/
	USHORT	gain_r;			/**< R Gain for brightness<br>
								value range		:[0 - 1023(=3.99609375)] <br>
								target registor	:@@LGTWR	*/
	USHORT	gain_g;			/**< G Gain for brightness<br>
								value range :[0 - 1023(=3.99609375)] <br>
								target registor	:@@LGTWG	*/
	USHORT	gain_b;			/**< B Gain for brightness<br>
								value range		:[0 - 1023(=3.99609375)] <br>
								target registor	:@@LGTWB	*/
	UCHAR	factor_r;		/**< R factor for brightness<br>
								value range		:[0 - 31(=1.9375)] <br>
								target registor	:@@LGTR		*/
	UCHAR	factor_g;		/**< G factor for brightness<br>
								value range		:[0 - 31(=1.9375)] <br>
								target registor	:@@LGTG		*/
	UCHAR	factor_b;		/**< B factor for brightness<br>
								value range		:[0 - 31(=1.9375)] <br>
								target registor	:@@LGTB		*/
	UCHAR	color_para;		/**< Parameters for brightness color difference<br>
								value range		:[0 - 16(=1.0)] <br>
								target registor	:@@LGTC		*/
	UCHAR	contour_r;		/**< R factor for the degree of weight contour<br>
								value range		:[0 - 16(=1.0)] <br>
								target registor	:@@LGTER	*/
	UCHAR	contour_g;		/**< G factor for the degree of weight contour<br>
								value range		:[0 - 16(=1.0)] <br>
								target registor	:@@LGTEG	*/
	UCHAR	contour_b;		/**< B factor for the degree of weight contour<br>
								value range		:[0 - 16(=1.0)] <br>
								target registor	:@@LGTEB	*/
	USHORT	contour_a_1;	/**< A contour-based one time adjustment factor<br>
								value range		:[0 - 255(=3.984375)] <br>
								target registor	:@@EDG2ANBA	*/
	USHORT	contour_a_2;	/**< A contour-based two time adjustment factor<br>
								value range		:[0 - 1023] <br>
								target registor	:@@EDGSCLA	*/
	USHORT	contour_b_1;	/**< B contour-based one time adjustment factor<br>
								value range		:[0 - 1023(=15.984375)] <br>
								target registor	:@@EDG2ANBB	*/
	USHORT	contour_b_2;	/**< B contour-based two time adjustment factor<br>
								value range		:[0 - 1023] <br>
								target registor	:@@EDGSCLB	*/
	USHORT	contour_c_1;	/**< C contour-based one time adjustment factor<br>
								value range		:[0 - 1023(=15.984375)] <br>
								target registor	:@@EDG2ANBC	*/
	USHORT	contour_c_2;	/**< C contour-based two time adjustment factor<br>
								value range		:[0 - 1023] <br>
								target registor	:@@EDGSCLC	*/
} T_IM_PRO_ELF_LGT_EDG_FUNC;

/** L/E filter
*/
typedef struct {
	USHORT	e_threshold_1;		/**< E threshold filter (L1 brightness)<br>
									value range		:[0 - 1023] <br>
									target registor	:@@EFL1TH	*/
	USHORT	e_threshold_2;		/**< E threshold filter (L2 brightness)<br>
									value range		:[0 - 1023] <br>
									target registor	:@@EFL2TH	*/
	USHORT	e_threshold_3;		/**< E threshold filter (L3 brightness)<br>
									value range		:[0 - 1023] <br>
									target registor	:@@EFL3TH	*/
	USHORT	e_threshold_4;		/**< E threshold filter (L4 brightness)<br>
									value range		:[0 - 1023] <br>
									target registor	:@@EFL4TH	*/
	SHORT	l_threshold;		/**< L threshold filter<br>
									value range		:[-1024 - +1023] <br>
									target registor	:@@LFTH		*/
	USHORT	l_contour_1;		/**< Applicable threshold contour L 1 degree<br>
									value range		:[0 - 4095] ELCOME1<=ELCOME2<br>
									target registor	:@@ELCOME1	*/
	USHORT	l_contour_2;		/**< Applicable threshold contour L 2 degree<br>
									value range		:[0 - 4095] <br>
									target registor	:@@ELCOME2	*/
	USHORT	l_cintensity_1;		/**< Applied intensity L (E1 degree contour)<br>
									value range		:[0 - 256(=1.0)] <br>
									target registor	:@@ELCOME1N	*/
	USHORT	l_cintensity_2;		/**< Applied intensity L (E2 degree contour)<br>
									value range		:[0 - 256(=1.0)] <br>
									target registor	:@@ELCOME2N	*/
} T_IM_PRO_ELF_L_E_FUNC;

/** 1s5 Noise Sampling information
*/
typedef struct {
	USHORT	s1s5_threshold[3];			/**< 1-5 degree threshold contour 1-3<br>
											value range		:[0 - 4095] A1S5E1<=A1S5E2<=A1S5E3<br>
											target registor	:@@A1S5E1/@@A1S5E2/@@A1S5E3	*/
	USHORT	s1s5_nr_l1_E13[3];			/**< 1-5 NR Intensity  (L1 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A1S5L1E1N/@@A1S5L1E2N/@@A1S5L1E3N	*/
	USHORT	s1s5_nr_l2_E13[3];			/**< 1-5 NR Intensity  (L2 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A1S5L2E1N/@@A1S5L2E2N/@@A1S5L2E3N	*/
	USHORT	s1s5_nr_l3_E13[3];			/**< 1-5 NR Intensity  (L3 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A1S5L3E1N/@@A1S5L3E2N/@@A1S5L3E3N	*/
	USHORT	s1s5_nr_l4_E13[3];			/**< 1-5 NR Intensity  (L4 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A1S5L4E1N/@@A1S5L4E2N/@@A1S5L4E3N	*/
	USHORT	s1s5_nr_adjustment[3];		/**< 1-5 NR (R/G/B) component intensity adjustment<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@NR1S5R/@@NR1S5G/@@NR1S5B	*/
	USHORT	s1s5_noise_limit_plus;		/**< 1-5 Noise limit (The positive side)<br>
											value range		:[0 - 4095] <br>
											target registor	:@@NS1S5LMP	*/
	USHORT	s1s5_noise_limit_minus;		/**< 1-5 Noise limit (The minus side)<br>
											value range		:[0 - 4095] <br>
											target registor	:@@NS1S5LMM	*/
} T_IM_PRO_ELF_1s5_SAMPLING;

/** F9 filter Noise Reduction Sampling information
*/
typedef struct {
	USHORT	strength;								/**< F9 filter NR strength.<br>
														value range		:[0(=0.0) - 256(=1.0)]<br>
														target registor	:@@FASTR	*/
	CHAR	e_focus_coord[4];						/**< F9 filter coordinates (Cb,Cr) of the F9 filter elliptic focus 1 and 2.<br>
														value range		:[-128 - 127]<br>
														target registor	:@@CRVAFX1/@@CRVAFY1/@@CRVAFX2/@@CRVAFY2	*/
	USHORT	e_polygonal_line_offset1;				/**< F9 filter polygonal line offset 1of the F9 filter beta value color difference elliptic reference.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FCRVABTOF1	*/
	USHORT	e_polygonal_line_minus_inclination[2];	/**< F9 filter polygonal line minus inclination 0 and 1 of the F9 filter β value color difference elliptic reference.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FCRVABTGA0/@@FCRVABTGA1	*/
	UCHAR	e_polygonal_line_boundary1;				/**< F9 filter polygonal line boundary 1 of the F9 filter beta value color difference elliptic reference.<br>
														value range		:[0 - 127]<br>
														target registor	:@@FCRVABTBD1	*/
	USHORT	e_upper_clip;							/**< F9 filter upper clip value of the F9 filter beta value color difference elliptic reference.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FCRVABTCPH	*/
	USHORT	e_lower_clip;							/**< F9 filter lower clip value of the F9 filter beta value color difference elliptic reference.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FCRVABTCPL	*/
	UCHAR	l_polygonal_line_plus_inclination0;		/**< F9 filter polygonal line plus inclination0 of the F9 filter beta value luminance reference.<br>
														value range		:[0 - 255]<br>
														target registor	:@@FYABTGA0	*/
	UCHAR	l_polygonal_line_minus_inclination1;	/**< F9 filter polygonal line minus inclination1 of the F9 filter beta value luminance reference.<br>
														value range		:[0 - 255]<br>
														target registor	:@@FYABTGA1	*/
	USHORT	l_polygonal_line_boundary[2];			/**< F9 filter polygonal line boundary 0 and 1 of the F9 filter beta value luminance reference.<br>
														value range		:[0 - 4095]<br>
														target registor	:@@FYABTBD0/@@FYABTBD1	*/
	USHORT	l_upper_clip;							/**< F9 filter upper clip value of the F9 filter beta value color difference luminance reference.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FYABTCLPH	*/
	USHORT	l_lower_clip;							/**< F9 filter lower clip value of the F9 filter beta value color difference luminance reference.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FYABTCLPL	*/
	UCHAR	t_f_profile_transition;					/**< F9 filter working method (transition method) of the threshold of the profile tolerance F.<br>
														value range		:[0 - 15]<br>
														target registor	:@@FAECHH	*/
	USHORT	t_f_profile_threshold;					/**< F9 filter threshold of the profile tolerance F.<br>
														value range		:[0 - 511]<br>
														target registor	:@@FAETHH	*/
} T_IM_PRO_ELF_f9_FILTER;

/** F9 filter Gain.
*/
typedef struct {
	USHORT gain[3];									/**< Gain coefficient of each pixel(R/G/B) for F9 NR.<br>
													    value range		:[0 - 1023]<br>
														target registor	:@@FS9WR/@@FS9WG/@@FS9WB	*/
} T_IM_PRO_ELF_f9_RGB_GAIN;

/** F9 filter Noise Sampling parameters.
*/
typedef struct {
	T_IM_PRO_ELF_f9_RGB_GAIN	rgb_gain;				/**< Gain coefficient of each pixel(R/G/B) for F9 NR.<br> */
	SHORT						yc_conv_matrix[9];		/**<  YC conversion matrix coefficient for F9 NR.<br>
															value range		:[-256 - 255]<br>
															target registor	:@@FS9YC	*/
	T_IM_PRO_ELF_f9_FILTER		f9a_group;				/**< F9 filter information of F9-A group.	*/
	T_IM_PRO_ELF_f9_FILTER		f9b_group;				/**< F9 filter information of F9-B group.	*/
} T_IM_PRO_ELF_f9_SAMPLING;

/** 5s9 Noise Sampling information
*/
typedef struct {
	USHORT	s5s9_threshold[3];			/**< 1-5 degree threshold contour 1-3<br>
											value range		:[0 - 4095] A5s9E1<=A5s9E2<=A5s9E3<br>
											target registor	:@@A5S9E1/@@A5S9E2/@@A5S9E3	*/
	USHORT	s5s9_nr_l1_E13[3];			/**< 1-5 NR Intensity  (L1 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A5S9L1E1N/@@A5S9L1E2N/@@A5S9L1E3N	*/
	USHORT	s5s9_nr_l2_E13[3];			/**< 1-5 NR Intensity  (L2 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A5S9L2E1N/@@A5S9L2E2N/@@A5S9L2E3N	*/
	USHORT	s5s9_nr_l3_E13[3];			/**< 1-5 NR Intensity  (L3 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A5S9L3E1N/@@A5S9L3E2N/@@A5S9L3E3N	*/
	USHORT	s5s9_nr_l4_E13[3];			/**< 1-5 NR Intensity  (L4 brightness, E1-3 degree contour)<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@A5S9L4E1N/@@A5S9L4E2N/@@A5S9L4E3N	*/
	USHORT	s5s9_nr_adjustment[3];		/**< 1-5 NR (R/G/B) component intensity adjustment<br>
											value range		:[0 - 256(=1.0)] <br>
											target registor	:@@NR5S9R/@@NR5S9G/@@NR5S9B	*/
	USHORT	s5s9_noise_limit_plus;		/**< 1-5 Noise limit (The positive side)<br>
											value range		:[0 - 4095] <br>
											target registor	:@@NS5S9LMP	*/
	USHORT	s5s9_noise_limit_minus;		/**< 1-5 Noise limit (The minus side)<br>
											value range		:[0 - 4095] <br>
											target registor	:@@NS5S9LMM	*/
} T_IM_PRO_ELF_5s9_SAMPLING;

/** Adjust the intensity noise suppression information
*/
typedef struct {
	USHORT	nr_strength_l1[3];			/**< (R/G/B) component strength NR main line path (L1 brightness)<br>
											value range		:[0 - 255(=1.0)] <br>
											target registor	:@@ELFMNNRRL1/@@ELFMNNRGL1/@@ELFMNNRBL1	*/
	USHORT	nr_strength_l2[3];			/**< (R/G/B) component strength NR main line path (L2 brightness)<br>
											value range		:[0 - 255(=1.0)] <br>
											target registor	:@@ELFMNNRRL2/@@ELFMNNRGL2/@@ELFMNNRBL2	*/
	USHORT	nr_strength_l3[3];			/**< (R/G/B) component strength NR main line path (L3 brightness)<br>
											value range		:[0 - 255(=1.0)] <br>
											target registor	:@@ELFMNNRRL3/@@ELFMNNRGL3/@@ELFMNNRBL3	*/
	USHORT	nr_strength_l4[3];			/**< (R/G/B) component strength NR main line path (L4 brightness)<br>
											value range		:[0 - 255(=1.0)] <br>
											target registor	:@@ELFMNNRRL4/@@ELFMNNRGL4/@@ELFMNNRBL4	*/
	USHORT	af_path_nr_strength;		/**< AF Path NR strength<br>
											value range		:[0 - 255(=1.0)] <br>
											target registor	:@@ELFAFNR	*/
} T_IM_PRO_ELF_Noise_Adj;

/** A0 filter Noise Characteristic information.
*/
typedef struct {
	USHORT	offset[4];					/**< offset value of the noise characteristic enumeration function for the area 0-3 of A0 function.<br>
											value range		:[0 - 4095] <br>
											target registor	:@@ZANBOR/@@ZANBOG/@@ZANBOB	*/
	USHORT	gain[4];					/**< gain value of the noise characteristic enumeration function for the area 0-3 of A0 function.<br>
											value range		:[0 - 4095] <br>
											target registor	:@@ZANBKR/@@ZANBKG/@@ZANBKB	*/
	USHORT	area_boundary[4];			/**< area  boundary value of the noise characteristic enumeration function of A0 function in units of 16.<br>
											value range		:[0 - 4095] <br>
											target registor	:@@ZANBDR/@@ZANBDG/@@ZANBDB<br>
											Make sure to set this register to maintain the following relationship.<br>
											ZANBDB[0]<=ZANBDB[1]<=ZANBDB[2]<=ZANBDB[3]	*/
} T_IM_PRO_ELF_a0_NC;

/** A0 Function Profile Tolerance Adjustment information	.
*/
typedef struct {
	USHORT	coeff1;						/**< profile tolerance adjustment coefficient1 of A0 function.<br>
											value range		:[0 - 1023] <br>
											target registor	:@@ZEDG2ANBA	*/
	USHORT	coeff2;						/**< profile tolerance adjustment coefficient2 of A0 function.<br>
											value range		:[0 - 1023] <br>
											target registor	:@@ZEDGSCLA	*/
} T_IM_PRO_ELF_a0_TOLERANCE_ADJ;

/** A0 filter Noise Reduction Sampling information
*/
typedef struct {
	E_IM_PRO_ELF_A0_DPC_MODE		dpc_mode;								/**< defective pixel correction mode in A0 function.<br>
																				target registor	:@@ZELADCMD	*/
	T_IM_PRO_ELF_a0_NC				nce[3];									/**< Noise Characteristic Enumeration parameters.	*/
	USHORT							lumi_rgb_gain[3];							/**< gain value for luminance for each pixel (R/G/B) of A0 function.<br>
																				value range		:[0 - 1023]<br>
																				target registor	:@@ZLGTWR/@@ZLGTWG/@@ZLGTWB	*/
	USHORT							lumi_rgb_coeff[3];						/**< coefficient for luminance for each pixel (R/G/B) of A0 function.<br>
																				value range		:[0 - 31(=1.0)]<br>
																				target registor	:@@ZLGTR/@@ZLGTG/@@ZLGTB	*/
	USHORT							lumi_c_coeff;							/**< difference parameter for luminance for each pixel (R/G/B) of A0 function.<br>
																				value range		:[0 - 31(=1.0)]<br>
																				target registor	:@@ZLGTC	*/
	UCHAR							t_weight[3];							/**< weight coefficient for profile tolerance for each pixel (R/G/B) of A0 function.<br>
																				value range		:[0 - 31(=1.0)]<br>
																				target registor	:@@ZLGTER/@@ZLGTEG/@@ZLGTEB	*/
	T_IM_PRO_ELF_a0_TOLERANCE_ADJ	t_adj_a;								/**< tolerance adjustment parameter of A-system.	*/
	T_IM_PRO_ELF_a0_TOLERANCE_ADJ	t_adj_b;								/**< tolerance adjustment parameter of B-system.	*/
	USHORT							t_long_wavelength_threshold[3];			/**< long wavelength profile tolerance threshold 1-3 of A0 function.<br>
																				value range		:[0 - 4095]<br>
																				target registor	:@@ZALNGE1/@@ZALNGE2/@@ZALNGE3<br>
																				Make sure to set these bits to maintain the following relationship.<br>
																				ZALNGE1<=ZALNGE2<=ZALNGE3	*/
	USHORT							t_long_wavelength_nr_strength[4][3];	/**< long wavelength NR strength respectively in the luminance Lx and profile tolerance Ey (x=1,2,3,4; y=1,2,3) of A0 function.<br>
																				value range		:[0 - 511]<br>
																				target registor	:<br>
																									@@ZALNGL1E1N/@@ZALNGL1E2N/@@ZALNGL1E3N<br>
																									@@ZALNGL2E1N/@@ZALNGL2E2N/@@ZALNGL2E3N<br>
																									@@ZALNGL3E1N/@@ZALNGL3E2N/@@ZALNGL3E3N<br>
																									@@ZALNGL4E1N/@@ZALNGL4E2N/@@ZALNGL4E3N	*/
	USHORT							l_filter_extraction_threshold;			/**< L filter extraction threshold of A0 function.<br>
																				value range		:[0 - 4095]<br>
																				target registor	:@@ZLFTH	*/
	USHORT							l_app_tolerance_threshold[2];			/**< L application profile tolerance threshold 1 and 2 of A0 function.<br>
																				value range		:[0 - 4095]<br>
																				target registor	:@@ZLCOME1/@@ZLCOME2	*/
	USHORT							l_app_strength[2];						/**< L application strength 1 and 2 of A0 function.<br>
																				value range		:[0 - 4095(=1.0)]<br>
																				target registor	:@@ZLCOME1N/@@ZLCOME2N	*/
	USHORT							long_wavelength_noise_p_limiter;		/**< plus side clip value for the extraction noise data of A0 function.<br>
																				value range		:[0 - 4095]<br>
																				target registor	:@@ZNSLNGLMP	*/
	USHORT							long_wavelength_noise_m_limiter;		/**< minus side clip value for the extraction noise data of A0 function.<br>
																				value range		:[0 - 4095]<br>
																				target registor	:@@ZNSLNGLMM	*/
} T_IM_PRO_ELF_a0_FILTER;

/** Edge level adjustment parameter for high frequency area.
*/
typedef struct {
	USHORT							high_freq_area_adj_coeff;				/**< coefficient of Edge level adjustment parameter for high frequency area.<br>
																				value range		:[0 - 4095]<br>
																				target registor	:@@EDGCATHA	*/
	UCHAR							test_para_1;							/**< Test parameter1. Please do not set a value other than 0xFF.	*/
	UCHAR							test_para_2;							/**< Test parameter2. Please do not set a value other than 0x09.	*/
} T_IM_PRO_ELF_HIGH_FREQ_AREA_EDGE_ADJ;

/*	@}*/	// im_pro_b2b_elf

/** @addtogroup im_pro_b2b_fshd FSHD
@{
*/

/** Frame shading correction blend ratio information
*/
typedef struct {
	SHORT							shading_blend_0;		/**< Frame shading blend ratio 0<br>
																value range		:[-512 - +511] <br>
																target registor	:@@FSHDBC0 */
	SHORT							shading_blend_1;		/**< Frame shading blend ratio 1<br>
																value range		:[-512 - +511] <br>
																target registor	:@@FSHDBC1 */
} T_IM_PRO_FSHD_BLEND_CTRL;

/** Frame shading compensation control information
*/
typedef struct {
	E_IM_PRO_FSHD_SHADING_MODE		correction_mode;		/**< Correction mode<br>
																target registor	:@@FSMODE */
	E_IM_PRO_FSHD_SHADING_MODE_AWB	correction_mode_awb;	/**< Horizontal interpolation pixel unit<br>
																target registor	:@@FWMODE */
	E_IM_PRO_FSHD_MONTAGE_MODE		montage_mode;			/**< Montage mode<br>
																target registor	:@@FSHDPTMD */
	USHORT							clip_r;					/**< Clip surface shading correction value after (R)<br>
																value range		:[0 - 4095] <br>
																target registor	:@@FSULRR */
	USHORT							clip_gr;				/**< Clip surface shading correction value after (GR)<br>
																value range		:[0 - 4095] <br>
																target registor	:@@FSULGR */
	USHORT							clip_gb;				/**< Clip surface shading correction value after (GB)<br>
																value range		:[0 - 4095] <br>
																target registor	:@@FSULGB */
	USHORT							clip_b;					/**< Clip surface shading correction value after (B)<br>
																value range		:[0 - 4095] <br>
																target registor	:@@FSULBB */
} T_IM_PRO_FSHD_COMMON_CTRL;


/** Frame shading correction control information
*/
typedef struct {
	E_IM_PRO_FSHD_CORRECTION_FORMAT	fsctl;					/**< Formatted frame correction factor<br>
																value range		:See @ref E_IM_PRO_FSHD_CORRECTION_FORMAT<br>
																target registor	:@@FSFMT */
	USHORT							frame_horizontal_size;	/**< Frame horizontal size correction factor<br>
																value range		:[4 - 314] 2pixel boundary<br>
																target registor	:@@FSSUBPRH */
	USHORT							frame_vertical_size;	/**< Frame vertical size correction factor<br>
																value range		:[4 - 1122] 2pixel boundary<br>
																target registor	:@@FSSUBPRV */
	USHORT							horizontal_pixel;		/**< Correction of horizontal pixels per frame interpolation<br>
																value range		:[16 - 724] 2pixel boundary<br>
																target registor	:@@FSSUBSPH */
	USHORT							vertical_pixel;			/**< Correction of vertical pixels per frame interpolation<br>
																value range		:[16 - 724] 2pixel boundary<br>
																target registor	:@@FSSUBSPV */
	USHORT							Inverse_base;			/**< Inverse coefficient correction interpolation frame (base unit)<br>
																value range		:[0 - 65535] <br>
																target registor	:@@FSSUBDRM */
	USHORT							Inverse_exponent;		/**< Inverse coefficient correction interpolation frame (Exponent part)<br>
																value range		:[0 - 11] <br>
																target registor	:@@FSSUBDRE */
	USHORT							montage_pos_h;			/**< horizontal montage position of the montage image in units of 2 pixels<br>
																value range		:[4 - 2496] 2pixel boundary<br>
																Set the value with reference to the left side of the setting processing window<br>
																and inside the setting value range of FSHDHW and satisfy the relationship shown below.<br>
																Moreover, make sure to set FSHDPTMD=1 to use this setting. <br>
																FSSUBSTH + FSHDHWPH = n * FSSUBSPH + 2<br>
																※n: integer<br>
																target registor	:@@FSHDHWPH */
	USHORT							start_pos_h;			/**< Horizontal start position correction factor frame<br>
																value range		:[0 - 722] 2pixel boundary<br>
																target registor	:@@FSSUBSTH */
	USHORT							start_pos_v;			/**< Vertical start position correction factor frame<br>
																value range		:[0 - 722] 2pixel boundary<br>
																target registor	:@@FSSUBSTV */
	USHORT							magnification_r;		/**< Magnification correction factor frame (pixel R)<br>
																value range		:[0 - 511] <br>
																target registor	:@@FSARR */
	USHORT							magnification_gr;		/**< Magnification correction factor frame (pixel GR)<br>
																value range		:[0 - 511] <br>
																target registor	:@@FSAGR */
	USHORT							magnification_gb;		/**< Magnification correction factor frame (pixel GB)<br>
																value range		:[0 - 511] <br>
																target registor	:@@FSAGB */
	USHORT							magnification_b;		/**< Magnification correction factor frame (pixel B)<br>
																value range		:[0 - 511] <br>
																target registor	:@@FSABB */
	E_IM_PRO_FSHD_SAT_CORRECTION	sat_correction_enabled;	/**< Saturation correction Enable/Disable<br>
																target registor	:@@FSHCEN */
	USHORT							sat_slope_gain[4];		/**< SLOPE gain for Saturation correction.<br>
																value range		:[0 - 4095(=255.9375)]<br>
																target registor	:[0]@@FSSLPRR, [1]@@FSSLPGR, [2]@@FSSLPGB, [3]@@FSSLPBB */
} T_IM_PRO_FSHD_FRAME_CTRL;

/** Concentric Correction control information
*/
typedef struct {
	E_IM_PRO_FSHD_LUT_SEL			select_ram;			/**< Selection correction LUT concentric<br>
															target registor	:@@CSSTBL */
	E_IM_PRO_FSHD_CORRECTION_FORMAT	select_format;		/**< Formatting correction factor concentric<br>
															target registor	:@@CSFMT */
	UCHAR							interpolated_val;	/**< Correction factor correction LUT value concentric<br>
															value range		:[0 - 7] <br>
															target registor	:@@CSHDLIR */
	UCHAR							interval_h;			/**< Concentric horizontal adjacent pixel spacing correction<br>
															value range		:[16 - 64] <br>
															target registor	:@@CSHDPARH */
	UCHAR							interval_v;			/**< Adjacent concentric vertical pixel spacing correction<br>
															value range		:[16 - 64] <br>
															target registor	:@@CSHDPARV */
	USHORT							start_address_r;	/**< Concentric correction RAM start address table (pixels R)<br>
															value range		:[pixels R Start Address 10bit] (Multiples of 2)<br>
															target registor	:@@CSTBLSRR */
	USHORT							start_address_gr;	/**< Concentric correction RAM start address table (pixels GR)<br>
															value range		:[pixels GR Start Address 10bit] (Multiples of 2)<br>
															target registor	:@@CSTBLSGR */
	USHORT							start_address_gb;	/**< Concentric correction RAM start address table (pixels GB)<br>
															value range		:[pixels GB Start Address 10bit] (Multiples of 2)<br>
															target registor	:@@CSTBLSGB */
	USHORT							start_address_b;	/**< Concentric correction RAM start address table (pixels B)<br>
															value range		:[pixels B Start Address 10bit] (Multiples of 2)<br>
															target registor	:@@CSTBLSBB */
	USHORT							offset;				/**< Concentric optical center offset correction<br>
															value range		:[0 - 16383] <br>
															target registor	:@@CSHDDO */
	USHORT							clip_value;			/**< Optical center of the concentric maximum clip value correction<br>
															value range		:[0 - 16383] <br>
															target registor	:@@CSHDDM */
	USHORT							pos_h;				/**< Horizontal position the optical center concentric correction<br>
															value range		:[0 - 16383] <br>
															target registor	:@@CSHDOAH */
	USHORT							pos_v;				/**< Vertical position of the optical center concentric correction<br>
															value range		:[0 - 16383] <br>
															target registor	:@@CSHDOAV */
	USHORT							magnification_r;	/**< Magnification correction factor concentric circles (pixels R)<br>
															value range		:[0 - 511] <br>
															target registor	:@@CSARR */
	USHORT							magnification_gr;	/**< Magnification correction factor concentric circles (pixels GR)<br>
															value range		:[0 - 511] <br>
															target registor	:@@CSAGR */
	USHORT							magnification_gb;	/**< Magnification correction factor concentric circles (pixels GB)<br>
															value range		:[0 - 511] <br>
															target registor	:@@CSAGB */
	USHORT							magnification_b	;	/**< Magnification correction factor concentric circles (pixels B)<br>
															value range		:[0 - 511] <br>
															target registor	:@@CSABB */
} T_IM_PRO_FSHD_CONCENTRIC_CTRL;

/** Frame shading Table address information
*/
typedef struct {
	SHORT*	pshd_tbl;		/**< table address pointer for FSHDCTBL	*/
	USHORT	size;			/**< set table size for FSHDCTBL<br>
								value range		:[0 - 1024] number of array	*/
} T_IM_PRO_FSHD_TABLE;

/*	@}*/	// im_pro_b2b_fshd


/** @addtogroup im_pro_sro_afpb AFPB
@{
*/

/** AFPB Control information
*/
typedef struct {
	T_IM_PRO_AREA_INFO		afpb_area;		/**< AFPB Window information<br>
												value range  :pos_x[0 - 16382]<br>
												target registor  :@@AFPBH<br><br>
												value range  :pos_y[0 - 16380]<br>
												target registor  :@@AFPBV<br><br>
												value range  :width[2 - 16382]<br>
												target registor  :@@AFPBHW<br><br>
												value range  :lines[2 - 16382]<br>
												target registor  :@@AFPBVW */
} T_IM_PRO_AFPB_CTRL;

/*	@}*/	// im_pro_sro_afpb

/** @addtogroup im_pro_sro_fshdl FSHDL
@{
*/

/** Light-Frame Correction blend ratio information
*/
typedef struct {
	SHORT							shading_blend_0;		/**< Frame shading blend ratio 0<br>
																value range		:[-512 - +511] <br>
																target registor	:@@FSHDBC0 */
	SHORT							shading_blend_1;		/**< Frame shading blend ratio 1<br>
																value range		:[-512 - +511] <br>
																target registor	:@@FSHDBC1 */
} T_IM_PRO_FSHDL_BLEND_CTRL;

/** Light-Frame Correction common control information
*/
typedef struct {
	E_IM_PRO_FSHD_MONTAGE_MODE			montage_mode;			/**< Montage mode<br>
																	target registor	:@@FFSHPTMD */
	USHORT								clip_r;					/**< Clip surface shading correction value after (R)<br>
																	value range		:[0 - 16383] <br>
																	target registor	:@@FFSHULRR */
	USHORT								clip_gr;				/**< Clip surface shading correction value after (GR)<br>
																	value range		:[0 - 16383] <br>
																	target registor	:@@FFSHULGR */
	USHORT								clip_gb;				/**< Clip surface shading correction value after (GB)<br>
																	value range		:[0 - 16383] <br>
																	target registor	:@@FFSHULGB */
	USHORT								clip_b;					/**< Clip surface shading correction value after (B)<br>
																	value range		:[0 - 16383] <br>
																	target registor	:@@FFSHULBB */
} T_IM_PRO_FSHDL_COMMON_CTRL;

/** Light-Frame Correction common control information
*/
typedef struct {
	E_IM_PRO_FSHDL_CORRECTION_FORMAT	select_format;			/**< Formatting correction factor concentric<br>
																	target registor	:@@FFSHGFMT */
	USHORT								frame_horizontal_size;	/**< Frame horizontal size correction factor<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
																	value range		:[4 - 314] 2pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
																	value range		:[4 - 366] 2pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
																	target registor	:@@FFSHSUBPRH */
	USHORT								frame_vertical_size;	/**< Frame vertical size correction factor<br>
																	value range		:[4 - 1122] 2pixel boundary<br>
																	target registor	:@@FFSHSUBPRV */
	USHORT								horizontal_pixel;		/**< Correction of horizontal pixels per frame interpolation<br>
																	value range		:[16 - 724] 2pixel boundary<br>
																	target registor	:@@FFSHSUBSPH */
	USHORT								vertical_pixel;			/**< Correction of vertical pixels per frame interpolation<br>
																	value range		:[16 - 724] 2pixel boundary<br>
																	target registor	:@@FFSHSUBSPV */
	USHORT								Inverse_base;			/**< Inverse coefficient correction interpolation frame (base unit)<br>
																	value range		:[0 - 65535] <br>
																	target registor	:@@FFSHSUBDRM */
	USHORT								Inverse_exponent;		/**< Inverse coefficient correction interpolation frame (Exponent part)<br>
																	value range		:[0 - 11] <br>
																	target registor	:@@FFSHSUBDRE */
	USHORT								start_pos_h;			/**< Horizontal start position correction factor frame<br>
																	value range		:[0 - 722] 2pixel boundary<br>
																	target registor	:@@FFSHSUBSTH */
	USHORT								start_pos_v;			/**< Vertical start position correction factor frame<br>
																	value range		:[0 - 722] 2pixel boundary<br>
																	target registor	:@@FFSHSUBSTV */
	USHORT								montage_pos_h;			/**< horizontal montage position of the montage image in units of 2 pixels<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
																	value range		:[4 - 2496] 2pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
																	value range		:[4 - 2900] 2pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
																	Set the value with reference to the left side of the setting processing window<br>
																	and inside the setting value range of FSHDHW and satisfy the relationship shown below.<br>
																	Moreover, make sure to set FSHDPTMD=1 to use this setting. <br>
																	FSSUBSTH + FSHDHWPH = n * FSSUBSPH + 2<br>
																	※n: integer<br>
																	target registor	:@@FFSHHWPH */
	E_IM_PRO_FSHDL_SAT_CORRECTION		sat_correction_enabled;	/**< Saturation correction Enable/Disable<br>
																	target registor	:@@FFSHCEN */
	USHORT								sat_slope_gain[4];		/**< SLOPE gain for Saturation correction.<br>
																	value range		:[0 - 4095(=255.9375)]<br>
																	target registor	:[0]@@FFSHLPRR, [1]@@FFSHLPGR, [2]@@FFSHLPGB, [3]@@FFSHLPBB */
} T_IM_PRO_FSHDL_FRAME_CTRL;

/*	@}*/	// im_pro_sro_fshdl


/** @addtogroup im_pro_sro_cag CAG
@{
*/
/** PRO CAG Area information
*/
typedef struct {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	USHORT	g_pos_x;		/**< global Compensation horizontal direction start position<br>
								value range		:[0 - 2496]<br>
								target registor	:@@CAGGH */
	USHORT	g_pos_y;		/**< global Compensation vertical direction start position<br>
								value range		:[0 - 8960] 2pixel boundary<br>
								target registor	:@@CAGGV */
	USHORT	g_width;		/**< global horizontal size <br>
								value range		:[18 - 2496] 2pixel boundary<br>
								target registor	:@@CAGGHW */
	USHORT	g_lines;		/**< global vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@CAGGVW */
	USHORT	pos_x;			/**< Compensation horizontal direction start position<br>
								value range		:[8 - 2487]<br>
								target registor	:@@CAGH */
	USHORT	pos_y;			/**< Compensation vertical direction start position<br>
								value range		:[4 - 8954] 2pixel boundary<br>
								target registor	:@@CAGV */
	USHORT	width;			/**< horizontal size<br>
								value range		:[1 - 2480]<br>
								target registor	:@@CAGHW */
	USHORT	lines;			/**< vertical size<br>
								value range		:[2 - 8952] 2pixel boundary<br>
								target registor	:@@CAGVW */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	USHORT	g_pos_x;		/**< global Compensation horizontal direction start position<br>
								value range		:[0 - 2900]<br>
								target registor	:@@CAGGH */
	USHORT	g_pos_y;		/**< global Compensation vertical direction start position<br>
								value range		:[0 - 8960] 2pixel boundary<br>
								target registor	:@@CAGGV */
	USHORT	g_width;		/**< global horizontal size <br>
								value range		:[18 - 2900] 2pixel boundary<br>
								target registor	:@@CAGGHW */
	USHORT	g_lines;		/**< global vertical size<br>
								value range		:[10 - 8960] 2pixel boundary<br>
								target registor	:@@CAGGVW */
	USHORT	pos_x;			/**< Compensation horizontal direction start position<br>
								value range		:[8 - 2891]<br>
								target registor	:@@CAGH */
	USHORT	pos_y;			/**< Compensation vertical direction start position<br>
								value range		:[4 - 8954] 2pixel boundary<br>
								target registor	:@@CAGV */
	USHORT	width;			/**< horizontal size<br>
								value range		:[1 - 2884]<br>
								target registor	:@@CAGHW */
	USHORT	lines;			/**< vertical size<br>
								value range		:[2 - 8952] 2pixel boundary<br>
								target registor	:@@CAGVW */
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
} T_IM_PRO_CAG_AREA;

/** Optical Color aberration control
*/
typedef struct {
	USHORT	v_binning_ratio_reciprocal_num;	/**< Reciprocal number of the binning ratio of the vertical direction.<br>
												value range		:[0 - 4095]<br>
												target registor	:@@RBRV	*/
	USHORT	h_binning_ratio_reciprocal_num;	/**< eciprocal number of the binning ratio of the horizontal direction.<br>
												value range		:[0 - 4095]<br>
												target registor	:@@RBRH	*/
	SHORT	opt_center_pos_h[3];			/**< Optical center position and offset(horizontal)<br>
												value range		:[0 - 65535(-16384.0 - +16383.5)]<br>
												target registor	:@@ABOARH/@@ABOAGH/@@ABOABH	*/
	SHORT	opt_center_pos_v[3];			/**< Optical center position and offset(vertical)<br>
												value range		:[0 - 65535(-16384.0 - +16383.5)]<br>
												target registor	:@@ABOARV/@@ABOAGV/@@ABOABV	*/
	SHORT	gain[3];						/**< Gain<br>
												value range		:[0 - 8191(-0.00000190 - +0.00000190)]<br>
												target registor	:@@ABGAR/@@ABGAG/@@ABGAB	*/
	SHORT	transfer_u_limit_h[3];			/**< horizontal Transfer upper limit<br>
												value range		:[0x1800(=-8.0) - 0x0800(=+8)]<br>
												target registor	:@@ABLRHU/@@ABLGHU/@@ABLBHU	*/
	SHORT	transfer_l_limit_h[3];			/**< horizontal Transfer lower limit<br>
												value range		:[0x1800(=-8.0) - 0x0800(=+8)]<br>
												target registor	:@@ABLRHL/@@ABLGHL/@@ABLBHL	*/
	SHORT	transfer_u_limit_v[3];			/**< vertical Transfer upper limit<br>
												value range		:[0x0C00(=-4.0) - 0x0400(=+4.0)]<br>
												target registor	:@@ABLRVU/@@ABLGVU/@@ABLBVU	*/
	SHORT	transfer_l_limit_v[3];			/**< vertical Transfer lower limit<br>
												value range		:[0x0C00(=-4.0) - 0x0400(=+4.0)]<br>
												target registor	:@@ABLRVL/@@ABLGVL/@@ABLBVL	*/

	SHORT	transfer_offset_h[3];			/**< horizontal Transfer offset<br>
												value range		:[0 - 4095(-8.0 - +7.99609375)]<br>
												target registor	:@@ABOFSRH/@@ABOFSGH/@@ABOFSBH	*/
	SHORT	transfer_offset_v[3];			/**< vertical Transfer offset<br>
												value range		:[0 - 2047(-4.0 - +3.99609375)]<br>
												target registor	:@@ABOFSRV/@@ABOFSGV/@@ABOFSBV	*/
	USHORT	corner_thresh_1[3];				/**< corner threshold 1<br>
												value range		:[0 - 0xFFFF(15-bit integer part,1-bit fractional part)]<br>
												target registor	:@@ABNLTHR1/@@ABNLTHG1/@@ABNLTHB1	*/
	SHORT	corner_gain_1[3];				/**< corner gain 1<br>
												value range		:[0 - 32767(-0.99993896484375 - +0.99993896484375)]<br>
												target registor	:@@ABNLGAR1/@@ABNLGAG1/@@ABNLGAB1	*/
	USHORT	corner_thresh_2[3];				/**< corner threshold 2<br>
												value range		:[0 - 0xFFFF(15-bit integer part,1-bit fractional part)]<br>
												target registor	:@@ABNLTHR2/@@ABNLTHG2/@@ABNLTHB2	*/
	SHORT	corner_gain_2[3];				/**< corner gain 2<br>
												value range		:[0 - 32767(-0.99993896484375 - +0.99993896484375)]<br>
												target registor	:@@ABNLGAR2/@@ABNLGAG2/@@ABNLGAB2	*/
	USHORT	corner_thresh_3[3];				/**< corner threshold 3<br>
												value range		:[0 - 0xFFFF(15-bit integer part,1-bit fractional part)]<br>
												target registor	:@@ABNLTHR3/@@ABNLTHG3/@@ABNLTHB3	*/
	SHORT	corner_gain_3[3];				/**< corner gain 3<br>
												value range		:[0 - 32767(-0.99993896484375 - +0.99993896484375)]<br>
												target registor	:@@ABNLGAR3/@@ABNLGAG3/@@ABNLGAB3	*/
	USHORT	corner_thresh_4[3];				/**< corner threshold 4<br>
												value range		:[0 - 0xFFFF(15-bit integer part,1-bit fractional part)]<br>
												target registor	:@@ABNLTHR4/@@ABNLTHG4/@@ABNLTHB4	*/
	SHORT	corner_gain_4[3];				/**< corner gain 4<br>
												value range		:[0 - 32767(-0.99993896484375 - +0.99993896484375)]<br>
												target registor	:@@ABNLGAR4/@@ABNLGAG4/@@ABNLGAB4	*/
} T_IM_PRO_CAG_CTRL;

/** CAG Gr/Gb gap correction control
*/
typedef struct {
	E_IM_PRO_CAG_CLIP_MODE	clip_mode;				/**< Clip select after correction<br>
														target registor	:@@CLPSEL */
	E_IM_PRO_CAG_COR_MODE	g_gap_cor_mode;			/**< Gr/Gb gap correction mode register<br>
														target registor	:@@GCAMD */
	E_IM_PRO_CAG_PAD_MODE	g_gap_pad_mode;			/**< Processing pixel padding ring register<br>
														target registor	:@@CAGPDD */
	E_IM_PRO_CAG_MODE		g_gap_mode;				/**< Gap correction mode register<br>
														target registor	:@@CAMD */
	USHORT	gradient_u_limit;						/**< upper limit to calculate the limit value of GrGb gradient correction amount.<br>
														value range		:[0 - 16383]<br>
														This value must be set to maintain the relationship: BFGLMTL<=BFGLMTH.<br>
														target registor	:@@BFGLMTH	*/
	USHORT	gradient_l_limit;						/**< lower limit to calculate the limit value of GrGb gradient correction amount.<br>
														value range		:[0 - 16383]<br>
														This value must be set to maintain the relationship: BFGLMTL<=BFGLMTH.<br>
														target registor	:@@BFGLMTL	*/
	USHORT	gradient_threshold;						/**< threshold to calculate the limit value of GrGb gradient correction amount.<br>
														value range		:[0 - 16383]<br>
														target registor	:@@BFGTHA	*/
	UCHAR	gradient_threshold_gain;				/**< gain value to calculate the limit value of GrGb gradient correction amount.<br>
														value range		:[0x00 - 0xFF(=3.984375)]<br>
														target registor	:@@BFGTHK	*/
} T_IM_PRO_CAG_CTRL_GLEVEL_DIFF;

/*	@}*/	// im_pro_sro_cag

/** @addtogroup im_pro_sro_ffc FFC
@{
*/
/** PRO FFC Area information
*/
typedef struct {
	USHORT	pos_x;		/**< Frame correction expansion horizontal start position<br>
							value range		:[0 - 4095]<br>
							target registor	:@@FCH */
	USHORT	pos_y;		/**< Frame correction expansion vertical start position<br>
							value range		:[2 - 8960] 2line boundary<br>
							target registor	:@@FCV */
	USHORT	width;		/**< horizontal size<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
							value range		:[4 - 2496] 4pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
							value range		:[4 - 2900] 4pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
							target registor	:@@FCHW */
	USHORT	lines;		/**< vertical size<br>
							value range		:[4 - 8960] 4pixel boundary<br>
							target registor	:@@FCVW */
} T_IM_PRO_FFC_AREA;

/** PRO FFC Control information
*/
typedef struct {
	SHORT	blend_ratio_0;				/**< Frame correction expansion blend ratio 0<br>
											value range		:[-512 - +511]<br>
											target registor	:@@FCBC0	*/
	SHORT	blend_ratio_1;				/**< Frame correction expansion blend ratio 1<br>
											value range		:[-512 - +511]<br>
											target registor	:@@FCBC1	*/
} T_IM_PRO_FFC_BLEND_CTRL;

/** PRO FFC Control information
*/
typedef struct {
	USHORT					coef_horizontal_size;	/**< Frame correction coefficient horizontal size<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
														value range		:[4 - 314] 2pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
														value range		:[4 - 366] 2pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
														target registor	:@@FCSUBPRH	*/
	USHORT					coef_vertical_size;		/**< Frame correction coefficient vertical size<br>
														value range		:[4 - 1122] 2pixel boundary<br>
														target registor	:@@FCSUBPRV	*/
	USHORT					horizontal_pixel;		/**< Frame correction interpolation horizontal pixels unit<br>
														value range		:[16 - 724] 2pixel boundary<br>
														target registor	:@@FCSUBSPH */
	USHORT					vertical_pixel;			/**< Frame correction interpolation vertical pixels unit<br>
														value range		:[16 - 724] 2pixel boundary<br>
														target registor	:@@FCSUBSPV */
	USHORT					inverse_base;			/**< Inverse coefficient correction interpolation (base unit)<br>
														value range		:[0 - 65535] <br>
														target registor	:@@FCSUBDRM */
	USHORT					inverse_exponent;		/**< Inverse coefficient correction interpolation (Exponent part)<br>
														value range		:[0 - 11] <br>
														target registor	:@@FCSUBDRE */
	USHORT					coef_pos_x;				/**< Frame correction coefficient horizontal start position<br>
														value range		:[0 - 722] 2pixel boundary<br>
														target registor	:@@FCSUBSTH	*/
	USHORT					coef_pos_y;				/**< Frame correction coefficient vertical start position<br>
														value range		:[0 - 722] 2pixel boundary<br>
														target registor	:@@FCSUBSTV	*/
} T_IM_PRO_FFC_CTRL;

/*	@}*/	// im_pro_sro_ffc

/** @addtogroup im_pro_sro_hrb HRB
@{
*/

/** HRB Information
*/
typedef struct {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	USHORT			in_pos_x;			/**< Start position of HRB window from a valid VD edge.<br>
											Set the interlace coordinates after 2-line division which is input to HRB/1 core respectively.<br>
											If the value of this register is doubled, it becomes the progressive image coordinates.<br>
											value range :[0 - 16382]<br>
											target registor :@@HRBH	*/
	USHORT			in_pos_y;			/**< Start position of HRB window from a valid HD edge.<br>
											value range :[0 - 8190]<br>
											target registor :@@HRBV	*/
	USHORT			in_width;		/**< HRB Horizon INC<br>
										value range :[2 - 2496]<br>
										target registor :@@HRBHW */
	USHORT			in_lines;		/**< HRB Vertical INC<br>
										value range :[1 - 8191]<br>
										target registor :@@HRBVW */
	USHORT			out_width;		/**< HRB Output Horizon INC<br>
										value range :[2 - 2496]<br>
										target registor :@@HRBOHW */
	UCHAR			cycle;			/**< HRB input cycle<br>
										value range :[0 - 14]<br>
										target registor :@@HRBSCYC */
	USHORT			out_timing;		/**< HRB Output Timing<br>
										value range :[0 - 8191]<br>
										target registor :@@HRBOSM */
	USHORT			coefficient[20][5];	/**< HRB Offset<br>
										value range :[0 - 65535(1.999969482)]<br>
										target registor :@@HRBK_0_0-@@HRBK_19_4		*/
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	USHORT			in_pos_x;			/**< Start position of HRB window from a valid VD edge.<br>
											Set the interlace coordinates after 2-line division which is input to HRB/1 core respectively.<br>
											If the value of this register is doubled, it becomes the progressive image coordinates.<br>
											value range :[0 - 16382]<br>
											target registor :@@HRBH	*/
	USHORT			in_pos_y;			/**< Start position of HRB window from a valid HD edge.<br>
											value range :[0 - 8190]<br>
											target registor :@@HRBV	*/
	USHORT			in_width;		/**< HRB Horizon INC<br>
										value range :[2 - 2900]<br>
										target registor :@@HRBHW */
	USHORT			in_lines;		/**< HRB Vertical INC<br>
										value range :[1 - 8191]<br>
										target registor :@@HRBVW */
	USHORT			out_width;		/**< HRB Output Horizon INC<br>
										value range :[2 - 2900]<br>
										target registor :@@HRBOHW */
	UCHAR			cycle;			/**< HRB input cycle<br>
										value range :[0 - 14]<br>
										target registor :@@HRBSCYC */
	USHORT			out_timing;		/**< HRB Output Timing<br>
										value range :[0 - 8191]<br>
										target registor :@@HRBOSM */
	USHORT			coefficient[20][5];	/**< HRB Offset<br>
										value range :[0 - 65535(1.999969482)]<br>
										target registor :@@HRBK_0_0-@@HRBK_19_4		*/
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
} T_IM_PRO_HRB_CTRL;

/*	@}*/	// im_pro_sro_hrb

/** @addtogroup im_pro_sro_vrb VRB
@{
*/

/** VRB Information
*/
typedef struct {
	USHORT			in_pos_x;		/**< Start position of VRB window from a valid VD edge.<br>
										Set the interlace coordinates after 2-line division which is input to VRB0/1 core respectively.<br>
										If the value of this register is doubled, it becomes the progressive image coordinates.<br>
										value range :[0 - 16382]<br>
										target registor :@@VRBH	*/
	USHORT			in_pos_y;		/**< Start position of VRB window from a valid HD edge.<br>
										value range :[0 - 8190] 2pixel boundary.<br>
										target registor :@@VRBV	*/
	USHORT			in_width;		/**< VRB Horizon INC<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
										value range :[2 - 2496]<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
										value range :[2 - 2900]<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
										target registor :@@VRBHW */
	USHORT			in_lines;		/**< VRB Vertical INC<br>
										value range :[1 - 8191]<br>
										target registor :@@VRBVW */
	UCHAR			cycle;			/**< VRB input cycle<br>
										value range :[1 - 10]<br>
										target registor :@@VRBSCYC */
	USHORT			coefficient0[10][3];	/**< VRB0 Input Offset<br>
										value range :[0 - 65535(1.999969482)]<br>
										target registor :@@VRB0K_0_0-@@VRB0K_9_2		*/
	USHORT			coefficient1[10][3];	/**< VRB1 Input Offset<br>
										value range :[0 - 65535(1.999969482)]<br>
										target registor :@@VRB1K_0_0-@@VRB1K_9_2		*/
} T_IM_PRO_VRB_CTRL;

/*	@}*/

// --- REMOVE_BUZ BEGIN
// @cond
/** @addtogroup im_pro_sro_buztop BUZTOP
@{
*/
/** BUZTOP Information
*/
typedef struct {
	USHORT			in_pos_x;			/**< Start position of BUZTOP window from a valid VD edge.<br>
											Set the interlace coordinates after 2-line division which is input to BUZTOP0/1 core respectively.<br>
											If the value of this register is doubled, it becomes the progressive image coordinates.<br>
											value range :[0 - 4095]<br>
											target registor :@@BUZH	*/
	USHORT			in_pos_y;			/**< Start position of BUZTOP window from a valid HD edge.<br>
											value range :[0 - 16382]<br>
											target registor :@@BUZV	*/
	USHORT			in_width;		/**< BUZTOP Horizon INC<br>
										value range :[1 - 4095]<br>
										target registor :@@BUZHW */
	USHORT			in_lines;		/**< BUZTOP Vertical INC<br>
										value range :[2 - 4094]<br>
										target registor :@@BUZVW */
	USHORT			out_width;		/**< BUZTOP Output Horizon INC<br>
										value range :[1 - 4095]<br>
										target registor :@@BUZOHW */
} T_IM_PRO_BUZTOP_CTRL;
/*	@}*/	// im_pro_sro_buztop
// @endcond
// --- REMOVE_BUZ END

/** @addtogroup im_pro_stat_aeawb AEAWB
@{
*/
/** PRO AEAWB Area information
*/
typedef struct {
	USHORT	pos_x;			/**< Compensation horizontal direction start position<br>
								value range		:[0 - 8190](Multiples of 2)<br>
								target registor	:@@AEH */
	USHORT	pos_y;			/**< Compensation vertical direction start position<br>
								value range		:[0 - 16382](Multiples of 2)<br>
								target registor	:@@AEV */
	USHORT	width;			/**< horizontal size<br>
								value range		:[6 - 512] 2pixel boundary<br>
								target registor	:@@AEHW */
	USHORT	lines;			/**< vertical size<br>
								value range		:[2 - 256] 2pixel boundary<br>
								target registor	:@@AEVW */
	USHORT	top_lines;		/**< top vertical size<br>
								value range		:[2 - 256] 2pixel boundary<br>
								target registor	:@@AETW */
	USHORT	bottom_lines;	/**< bottom vertical size<br>
								value range		:[2 - 256] 2pixel boundary<br>
								target registor	:@@AEBW */
	USHORT	left_width;		/**< left horizontal size<br>
								value range		:[6 - 512] 2pixel boundary<br>
								target registor	:@@AELW */
	USHORT	right_width;	/**< right horizontal size<br>
								value range		:[6 - 512] 2pixel boundary<br>
								target registor	:@@AERW */
} T_IM_PRO_AEAWB_AREA;

/** PRO AEAWB generation mode
*/
typedef struct {
	E_IM_PRO_AEAWB_MODE				aeawb_mode;				/**< AEAWB detecting mode<br>
																value range		:See @ref E_IM_PRO_AEAWB_MODE
																target registor	:@@AEAWBWMD */
	E_IM_PRO_AWB_MODE				awb_mode;				/**< AWB detecting mode<br>
																value range		:See @ref E_IM_PRO_AWB_MODE
																target registor	:@@AWBGMD */
	E_IM_PRO_AEAWB_WD_WINDOW_MODE	awb_wd_window_mode;		/**< AWB detecting window mode<br>
																value range		:See @ref E_IM_PRO_AEAWB_WD_WINDOW_MODE
																target registor	:@@AEEWMD */
	T_IM_PRO_AEAWB_AREA				awb_area;				/**< AWB Window information<br>
																value range		:pos_x:[0 - 8190](Multiples of 2)<br>
																target registor	:@@AEH<br>

																value range		:pos_y:[0 - 16382](Multiples of 2)<br>
																target registor	:@@AEV<br>

																value range		:width:[6 - 512] 2pixel boundary<br>
																target registor	:@@AEHW<br>

																value range		:lines:[2 - 256] 2pixel boundary<br>
																target registor	:@@AEVW<br>

																value range		:top_lines:[2 - 256] 2pixel boundary<br>
																target registor	:@@AETW<br>

																value range		:bottom_lines:[2 - 256] 2pixel boundary<br>
																target registor	:@@AEBW<br>

																value range		:left_width:[6 - 512] 2pixel boundary<br>
																target registor	:@@AELW<br>

																value range		:right_width:[6 - 512] 2pixel boundary<br>
																target registor	:@@AERW */
	USHORT							h_bc_num;				/**< horizontal AEAWB Window block number<br>
																value range		:[1 - 64].<br>
																Note that if level WB is enabled (aeawb_mode=E_IM_PRO_AEAWB_MODE_LWB~E_IM_PRO_AEAWB_MODE_AE_AWB_LWB),
																the upper limit of this register is 32.

																target registor	:@@AEHBC */
	USHORT							v_bc_num;				/**< vertical AEAWB Window block number<br> 
																value range		:[1 - 64]<br>
																target registor	:@@AEVBC */
} T_IM_PRO_AEAWB_CTRL;

/** PRO AE/AWB Addition permission size in a block information
*/
typedef struct {
	USHORT	aecvw;		/**< AE Addition permission size in a block V<br>
							value range		:[2 - 510] 2pixel boundary<br>
							target registor	:@@AECVW */
	USHORT	aechw;		/**< AE Addition permission size in a block H<br>
							value range		:[2 - 1022] 2pixel boundary<br>
							target registor	:@@AECHW */
	USHORT	aectw;		/**< AE Addition permission size in a block Top<br>
							value range		:[2 - 510] 2pixel boundary<br>
							target registor	:@@AECTW */
	USHORT	aecbw;		/**< AE Addition permission size in a block Bottom<br>
							value range		:[2 - 510] 2pixel boundary<br>
							target registor	:@@AECBW */
	USHORT	aeclw;		/**< AE Addition permission size in a block Left<br>
							value range		:[2 - 1022] 2pixel boundary<br>
							target registor	:@@AECLW */
	USHORT	aecrw;		/**< AE Addition permission size in a block Right<br>
							value range		:[2 - 1022] 2pixel boundary<br>
							target registor	:@@AECRW */
	USHORT	awcvw;		/**< AWB Addition permission size in a block V<br>
							value range		:[2 - 510] 2pixel boundary<br>
							target registor	:@@AWBCVW */
	USHORT	awchw;		/**< AWB Addition permission size in a block H<br>
							value range		:[2 - 1022] 2pixel boundary<br>
							target registor	:@@AWBCHW */
	USHORT	awctw;		/**< AWB Addition permission size in a block Top<br>
							value range		:[2 - 510] 2pixel boundary<br>
							target registor	:@@AWBCTW */
	USHORT	awcbw;		/**< AWB Addition permission size in a block Bottom<br>
							value range		:[2 - 510] 2pixel boundary<br>
							target registor	:@@AWBCBW */
	USHORT	awclw;		/**< AWB Addition permission size in a block Left<br>
							value range		:[2 - 1022] 2pixel boundary<br>
							target registor	:@@AWBCLW */
	USHORT	awcrw;		/**< AWB Addition permission size in a block Right<br>
							value range		:[2 - 1022] 2pixel boundary<br>
							target registor	:@@AWBCRW */
} T_IM_PRO_AEAWB_CW;

/** PRO AWB RGB gate threshold Information
*/
typedef struct {
	ULONG	awtduy;		/**< AWB RGB Level gate value Y(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDUY */
	ULONG	awtdur;		/**< AWB RGB Level gate value R(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDUR */
	ULONG	awtdug;		/**< AWB RGB Level gate value G(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDUG */
	ULONG	awtdub;		/**< AWB RGB Level gate value B(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDUB */
	ULONG	awtdly;		/**< AWB RGB Level gate value Y(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDLY */
	ULONG	awtdlr;		/**< AWB RGB Level gate value R(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDLR */
	ULONG	awtdlg;		/**< AWB RGB Level gate value G(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDLG */
	ULONG	awtdlb;		/**< AWB RGB Level gate value B(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWTDLB */
} T_IM_PRO_AEAWB_LV_GATE;

/** PRO AWB matrix level gate Information
*/
typedef struct {
	UCHAR	awmlucy;	/**< AWB matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWMLUCY	*/
	UCHAR	awmllcy;	/**< AWB matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWMLLCY	*/
	UCHAR	awmlucb;	/**< AWB matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWMLUCB	*/
	UCHAR	awmllcb;	/**< AWB matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWMLLCB	*/
	UCHAR	awmlucr;	/**< AWB matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWMLUCR	*/
	UCHAR	awmllcr;	/**< AWB matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@AWMLLCR	*/
	SHORT	awmc[3][3];	/**< AWB matrix level gate value<br>
							value range		:[0 - 511(-2.0 - +1.9921875)]<br>
							target registor	:@@AWMC00-@@AWMC22	*/
} T_IM_PRO_AEAWB_MATRIX_LV_GATE;

/** PRO Line Info. for flicker
*/
typedef struct {
	USHORT		start_line[4];			/**< Line luminosity evaluation vertical address<br>
											value range		:[0 - 32766] 2pixel boundary<br>
											target registor	:@@LINEV_0 - @@LINEV_3 */
} T_IM_PRO_AEAWB_LINE_INFO;

/** PRO AWB evaluated Block Information
*/
typedef struct {
	ULONG	AWBDATA_CNT;	/**< evaluated value of integration value count */
	ULONG	AWBDATA_R;		/**< evaluated value of R  */
	ULONG	AWBDATA_G_AVE;	/**< evaluated value of G Average */
	ULONG	AWBDATA_B;		/**< evaluated value of B  */
} T_IM_PRO_AEAWB_AWB_EVAL_BLK_MD0;

/** PRO AWB evaluated Block Information
*/
typedef struct {
	ULONG	AWBDATA_GB;	/**< evaluated value of GB */
	ULONG	AWBDATA_R;	/**< evaluated value of R  */
	ULONG	AWBDATA_GR;	/**< evaluated value of GR */
	ULONG	AWBDATA_B;	/**< evaluated value of B  */
} T_IM_PRO_AEAWB_AWB_EVAL_BLK_MD1;


/** PRO IAWB evaluated Block Information
*/
typedef struct {
	ULONG	IAWBDATA_CNT;		/**< evaluated value Count */
	ULONG	AWBDATA_R;			/**< evaluated value of R */
	ULONG	AWBDATA_G_AVERAGE;	/**< evaluation mean value of G */
	ULONG	AWBDATA_B;			/**< evaluated value of B  */
} T_IM_PRO_AEAWB_IAWB_EVAL_BLK;

/** Luminosity generation coefficient Information
*/
typedef struct {
	SHORT ykr;	/**< Luminosity generation coefficient R<br>
						value range :[0x100(=-1.0) - 0x0FF(=+0.99609375)]<br>
						target registor :@@xxYKRR */
	SHORT ykgr;	/**< Luminosity generation coefficient Gr<br>
						value range :[0x100(=-1.0) - 0x0FF(=+0.99609375)]<br>
						target registor :@@xxYKGR */
	SHORT ykgb;	/**< Luminosity generation coefficient Gb<br>
						value range :[0x100(=-1.0) - 0x0FF(=+0.99609375)]<br>
						target registor :@@xxYKGB */
	SHORT ykb;	/**< Luminosity generation coefficient B<br>
						value range :[0x100(=-1.0) - 0x0FF(=+0.99609375)]<br>
						target registor :@@xxYKBB */
} T_IM_PRO_STAT_YK;

/** Luminosity generation coefficient Information
*/
typedef struct {
	E_IM_PRO_AEAWB_MODE	mode;					/**< AEAWB Detecting mode */
	E_IM_PRO_AEAWB_TYPE	type;					/**< AEAWB data Type */
	UCHAR				aehbc;					/**< horizontal AEAWB Window block number */
	ULONG*				address;				/**< Wave Detection Data Address */
	ULONG*				global_data_width;		/**< horizontal width of AEAWB detection data */
} T_IM_PRO_AEAWB_WD_INFO;


/*	@}*/	// im_pro_stat_aeawb


/** @addtogroup im_pro_stat_af AF
@{
*/

/** AF Information
*/
typedef struct {
	E_IM_PRO_AF_ENABLE		eval_calc0_enable;				/**< AF detecting enable for evaluated value calculation unit0.<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENM0 */
	E_IM_PRO_AF_ENABLE		eval_calc1_enable;				/**< AF detecting enable for evaluated value calculation unit1.<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENM1 */
	E_IM_PRO_AF_ENABLE		eval_calc2_enable;				/**< AF detecting enable for evaluated value calculation unit2.<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENM2 */
	E_IM_PRO_AF_ENABLE		vertical_enable;				/**< AF detecting enable for vertical<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENV0 */
	E_IM_PRO_AF_ENABLE		horizon0_enable;				/**< AF detecting enable for horizon0<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENH0 */
	E_IM_PRO_AF_ENABLE		horizon1_enable;				/**< AF detecting enable for horizon1<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENH1 */
	E_IM_PRO_AF_ENABLE		horizon2_enable;				/**< AF detecting enable for horizon2<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENH2 */
	E_IM_PRO_AF_ENABLE		enable[E_IM_PRO_AF_WINDOW_MAX];	/**< AF detecting enable for AF Window<br>
																value range		: See @ref E_IM_PRO_AF_ENABLE<br>
																target registor	:@@ENW0-@@ENW19 */
} T_IM_PRO_AF_CTRL;

/** AF detection evaluated value Information
*/
typedef struct {
	SHORT				r;						/**< AF MTRX operation R pixel coefficient<br>
													value range		:[0 - 1023(-2.0 - +1.99609375)]<br>
													target registor	:@@AFMTRXRR */
	SHORT				gr;						/**< AF MTRX operation Gr pixel coefficient<br>
													value range		:[0 - 1023(-2.0 - +1.99609375)]<br>
													target registor	:@@AFMTRXGR */
	SHORT				gb;						/**< AF MTRX operation Gb pixel coefficient<br>
													value range		:[0 - 1023(-2.0 - +1.99609375)]<br>
													target registor	:@@AFMTRXGB */
	SHORT				b;						/**< AF MTRX operation B pixel coefficient<br>
													value range		:[0 - 1023(-2.0 - +1.99609375)]<br>
													target registor	:@@AFMTRXBB */
	E_IM_PRO_AF_ENABLE	median_enable;			/**< AF Median filter enable<br>
													value range		: See @ref E_IM_PRO_AF_ENABLE<br>
													target registor	:@@AFMDEN */
	E_IM_PRO_AF_ENABLE	knee_enable;			/**< AF Knee enable<br>
													value range		: See @ref E_IM_PRO_AF_ENABLE<br>
													target registor	:@@AFKNEN */
	USHORT				knee_boundary[7];		/**< AF Knee table boundary<br>
													value range		:[0 - 0x3fff] AFKNB[1]<=AFKNB[2]<=AFKNB[3]<=...<=AFKNB[7]<br>
													target registor	:@@AFKNB */
	USHORT				knee_slope[8];			/**< AF Knee table slope<br>
													value range		:[0 - 0x0fff(4-bit integer part,8-bit fractional part)] <br>
													target registor	:@@AFKNINC */
	USHORT				knee_offset[8];			/**< AF Knee table offset<br>
													value range		:[0 - 0x0fff] <br>
													target registor	:@@AFKNOFS */
} T_IM_PRO_AF_EVAL_CALC_CTRL;

/** AF vertical detection Information
*/
typedef struct {
	UCHAR						update_cycle;		/**< data update cycle for horizontal direction<br>
														value range		:[0(=16) 1 - 15]<br>
														target registor	:@@AFVSCYC */
	E_IM_PRO_AF_EVAL_CALC_UNIT	eval_cal_unit;		/**< select result of detection evaluated value<br>
														value range		: See @ref E_IM_PRO_AF_EVAL_CALC_UNIT<br>
														target registor	:@@AFSLV */
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	USHORT						start_addr;			/**< start address<br>
														value range		:[0 - 2496] 2byte boundary<br>
														target registor	:@@AFVSH */
	USHORT						horizon_size;		/**< horizontal area size<br>
														value range		:[2 - 312] 2pixel boundary<br>
														target registor	:@@AFVSHW */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	USHORT						start_addr;			/**< start address<br>
														value range		:[0 - 2900] 2byte boundary<br>
														target registor	:@@AFVSH */
	USHORT						horizon_size;		/**< horizontal area size<br>
														value range		:[2 - 362] 2pixel boundary<br>
														target registor	:@@AFVSHW */
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	INT16						iir_k;				/**<IIR filter gain coefficient<br>
														value range		:[0 - 32767(-64.0 - +63.99609375)]<br>
														target registor	:@@AFIIRVK */
	INT8						iir_a0;				/**<IIR filter A0 coefficient<br>
														value range		:[0 - 255(-4.0 - +3.96875)]<br>
														target registor	:@@AFIIRVA0 */
	INT8						iir_a1;				/**<IIR filter A1 coefficient<br>
														value range		:[0 - 255(-4.0 - +3.96875)]<br>
														target registor	:@@AFIIRVA1 */
	INT8						iir_a2;				/**<IIR filter A2 coefficient<br>
														value range		:[0 - 255(-4.0 - +3.96875)]<br>
														target registor	:@@AFIIRVA2 */
	INT16						iir_b1;				/**< IIR filter B1 coefficient<br>
														value range		:[0 - 32767(-64.0 - +63.99609375)]<br>
														target registor	:@@AFIIRVB1 */
	INT16						iir_b2;				/**< IIR filter B2 coefficient<br>
														value range		:[0 - 32767(-64.0 - +63.99609375)]<br>
														target registor	:@@AFIIRVB2 */
	USHORT						cor_threshold;		/**< coring threshold after IIR filter<br>
														value range		:[0 - 0x0FFF]<br>
														target registor	:@@AFCOR2LVV */
	USHORT						cor_level;			/**< coring level after IIR filter<br>
														value range		:[0 - 0x0FFF]<br>
														target registor	:@@AFCOR2V */
	SHORT						fir_filter[3];		/**<  FIR filter<br>
														value range		:[0 - 255(-2.0 - +1.984375)]<br>
														target registor	:@@AFFIRVB1/@@AFFIRVB2/@@AFFIRVB3 */
} T_IM_PRO_AF_VERTICAL_CTRL;

/** AF horizontal detection group Information
*/
typedef struct {
	E_IM_PRO_AF_ENABLE		iir_enable;			/**< IIR filter enable<br>
													value range		: See @ref E_IM_PRO_AF_ENABLE<br>
													target registor	:@@AFIHA, @@AFIHB */
	E_IM_PRO_AF_ENABLE		fir_enable;			/**< FIR filter enable<br>
													value range		: See @ref E_IM_PRO_AF_ENABLE<br>
													target registor	:@@AFFHA, @@AFFHB */
	E_IM_PRO_AF_TAP			input_select;		/**< select input detection evaluated value<br>
													value range		: See @ref E_IM_PRO_AF_TAP<br>
													target registor	:@@AFSLHA, @@AFSLHB */
	INT8					fir_coefficient[5];	/**< FIR coefficient 1<br>
													value range		:[0 - 255(-2.0 - +1.984375)]<br>
													target registor	:@@AFFIRHBA_1-@@AFFIRHBA_5, @@AFFIRHBB_1-@@AFFIRHBB_5 */
	USHORT					fir_level;			/**< coring level after FIR<br>
													value range		:[0 - 0x0FFF]<br>
													target registor	:@@AFCOR1HA, @@AFCOR1HB */
	UCHAR					gain;				/**< gain<br>
													value range		:[0x00 - 0xFF(=3.984375)]<br>
													target registor	:@@AFGAINHA, @@AFGAINHB */
	INT16					iir_k;				/**<IIR filter gain coefficient<br>
													value range		:[0 - 32767(-64.0 - +63.99609375)]<br>
													target registor	:@@AFIIRHKA, @@AFIIRHKB */
	INT8					iir_a0;				/**<IIR filter A0 coefficient<br>
													value range		:[0 - 255(-4.0 - +3.96875)]<br>
													target registor	:@@AFIIRHA0A, @@AFIIRHA0B */
	INT8					iir_a1;				/**<IIR filter A1 coefficient<br>
													value range		:[0 - 255(-4.0 - +3.96875)]<br>
													target registor	:@@AFIIRHA1A, @@AFIIRHA1B */
	INT8					iir_a2;				/**<IIR filter A2 coefficient<br>
													value range		:[0 - 255(-4.0 - +3.96875)]<br>
													target registor	:@@AFIIRHA2A, @@AFIIRHA2B */
	INT16					iir_b1;				/**< IIR filter B1 coefficient<br>
													value range		:[0 - 32767(-64.0 - +63.99609375)]<br>
													target registor	:@@AFIIRHB1A, @@AFIIRHB1B */
	INT16					iir_b2;				/**< IIR filter B2 coefficient<br>
													value range		:[0 - 32767(-64.0 - +63.99609375)]<br>
													target registor	:@@AFIIRHB2A, @@AFIIRHB2B */
	USHORT					coring_bounder[3];	/**< Boundary value register coring horizontal AF detection process<br>
													value range		:[0 - 0x0FFF]<br>
													target registor	:@@AFCORBHA, @@AFCORBHB */
	INT16					coring_slope[4];	/**< Slope value register coring horizontal AF detection process<br>
													value range		:[0 - 4095(-2.0 - +1.9990234375)]<br>
													target registor	:@@AFCORINCHA, @@AFCORINCHB */
	USHORT					coring_offset[4];	/**< Offset value register coring horizontal AF detection process<br>
													value range		:[0 - 0x0FFF]<br>
													target registor	:@@AFCOROFSHA, @@AFCOROFSHB */

} T_IM_PRO_AF_HORIZON_GROUP;

/** AF horizontal detection Information
*/
typedef struct {
	T_IM_PRO_AF_HORIZON_GROUP	group_a;			/**< for group A */
	T_IM_PRO_AF_HORIZON_GROUP	group_b;			/**< for group B */
} T_IM_PRO_AF_HORIZON_CTRL;

/** AF Window Information
*/
typedef struct {
	E_IM_PRO_AF_WIN_HSEL			h_sel0;					/**< horizon 0 select<br>
																value range		:See @ref E_IM_PRO_AF_WIN_HSEL<br>
																target registor	:@@AFSLWH0 */
	E_IM_PRO_AF_WIN_HSEL			h_sel1;					/**< horizon 1 select<br>
																value range		:See @ref E_IM_PRO_AF_WIN_HSEL<br>
																target registor	:@@AFSLWH1 */
	E_IM_PRO_AF_WIN_HSEL			h_sel2;					/**< horizon 2 select<br>
																value range		:See @ref E_IM_PRO_AF_WIN_HSEL<br>
																target registor	:@@AFSLWH2 */
	E_IM_PRO_AF_EVAL_CALC_UNIT		eval_cal_unit;			/**< Input select for AF detection window processing unit evaluated value accumulation.<br>
																target registor	:@@AFSLWC */
	USHORT							bright_threshold;		/**< Bright threshold<br>
																value range		:[0 - 4095]<br>
																target registor	:@@AFCLVGE */
	USHORT							dark_threshold;			/**< Dark threshold<br>
																value range		:[0 - 4095]<br>
																target registor	:@@AFCLVL */
	USHORT							start_pos_x;			/**< Window Start Position X<br>
																value range		:[0 - 8190] 2pixel boundary<br>
																target registor	:@@AFH */
	USHORT							start_pos_y;			/**< Window Start Position Y<br>
																value range		:[0 - 16380] 2pixel boundary<br>
																target registor	:@@AFV */
	USHORT							width;					/**< Window horizontal size<br>
																value range		:[2 - 4094] 2pixel boundary<br>
																target registor	:@@AFHW */
	USHORT							height;					/**< Window vertical size<br>
																value range		:[2 - 4094] 2pixel boundary<br>
																target registor	:@@AFVW */
	E_IM_PRO_AF_MIN_MAX_DETECT_SEL	max_mix_select;			/**< select the input for the minimum/maximum value of the evaluation value.<br>
																target registor	:@@AFSLWM */
} T_IM_PRO_AF_WIN_CTRL;

/*	@}*/	// im_pro_stat_af

/** @addtogroup im_pro_stat_hist HIST
@{
*/
/** HIST Area information
*/
typedef struct {
	USHORT	pos_x;		/**< Compensation horizontal direction start position<br>
							value range		:[0 - 16382](Multiples of 2)<br>
							target registor	:@@HISTH */
	USHORT	pos_y;		/**< Compensation vertical direction start position<br>
							value range		:[0 - 16382](Multiples of 2)<br>
							target registor	:@@HISTV */
	USHORT	width;		/**< horizontal size<br>
							value range		:[2 - 16382] 2pixel boundary<br>
							target registor	:@@HISTHW */
	USHORT	lines;		/**< vertical size<br>
							value range		:[2 - 12288] 2pixel boundary
							target registor	:@@HISTVW */
} T_IM_PRO_HIST_AREA;

/** HIST Gain Coefficient before Detection Register
*/
typedef struct {
	USHORT	r_gain;				/**< R gain<br>
									value range		:[0 - 1023(=7.9921875)]<br>
									target registor	:@@HISTWBGR */
	USHORT	g_gain;				/**< R gain<br>
									value range		:[0 - 1023(=7.9921875)]<br>
									target registor	:@@HISTWBGG */
	USHORT	b_gain;				/**< R gain<br>
									value range		:[0 - 1023(=7.9921875)]<br>
									target registor	:@@HISTWBGB */
} T_IM_PRO_HIST_GAIN;

/** control parameter of the histogram detection.
*/
typedef struct {
	E_IM_PRO_HIST_MODE			hist_mode;				/**< Histogram detection mode.<br>
															target registor	:@@HISTCT */
	E_IM_PRO_HIST_CYCLE			hist_cycle;				/**< HIST horizontal subsampling cycle.<br>
															target registor	:@@HISTHCYC */
	E_IM_PRO_HIST_SHIFT_VAL		hist_shift_val;			/**< Color and luminance data shift amount for HIST sum calculation.<br>
															target registor	:@@HISTSUMS */
} T_IM_PRO_HIST_CTRL;

/** Histgram result data format.
*/
typedef struct {
	ULONG						most_frequent_value[4];	/**< most frequent value inside the HIST window.<br>
															target registor	:[0]@@HISTRMODE [1]@@HISTGMODE [2]@@HISTBMODE [3]@@HISTYMODE */
	ULONG						accumulation_value[4];	/**< accumulation value inside the HIST window.<br>
															target registor	:[0]@@HISTRSUM [1]@@HISTGSUM [2]@@HISTBSUM [3]@@HISTYSUM */
} T_IM_PRO_HIST_DATA;

/** Histgram result RAM data format.
*/
typedef struct {
	ULONG						r[256];	/**< frequency of each pixel level of R.<br>
															target registor	@@HISTRDATA */
	ULONG						g[256];	/**< frequency of each pixel level of G.<br>
															target registor	@@HISTGDATA */
	ULONG						b[256];	/**< frequency of each pixel level of B.<br>
															target registor	@@HISTBDATA */
	ULONG						y[256];	/**< frequency of each pixel level of Y.<br>
															target registor	@@HISTYDATA */
} T_IM_PRO_HIST_RAM_DATA;


/*	@}*/	// im_pro_stat_hist


/** @addtogroup im_pro_pas_spt SPT
@{
*/

/** SPT Control information
*/
typedef struct {
	E_IM_PRO_SPT_TBL_SEL		table_sel;				/**< Select shading correction table.<br>
															target registor	:@@SPTTBL */
	USHORT						horizontal_pixel;		/**< Correction of horizontal pixels per frame interpolation<br>
															value range		:[2 - 255]<br>
															target registor	:@@SPTBSH */
	USHORT						vertical_pixel;			/**< Correction of vertical pixels per frame interpolation<br>
															value range		:[2 - 255]<br>
															target registor	:@@SPTBSV */
	USHORT						Inverse_base;			/**< Inverse coefficient correction interpolation frame (base unit)<br>
															value range		:[0 - 511] <br>
															target registor	:@@SPTBDM */
	USHORT						Inverse_exponent;		/**< Inverse coefficient correction interpolation frame (Exponent part)<br>
															value range		:[0 - 14] <br>
															target registor	:@@SPTBDE */
	USHORT						pos_y;					/**< Compensation vertical direction start position<br>
															value range		:[0 - 4095](Multiples of 2)<br>
															target registor	:@@SPTV */
	USHORT						pos_x;					/**< Compensation horizontal direction start position<br>
															value range		:[0 - 8191]<br>
															target registor	:@@SPTH */
	UCHAR						v_block_size;			/**< Vertical block size setting.<br>
															value range		:[1 - 15]<br>
															target registor	:@@SPTVBC */
	UCHAR						h_block_size;			/**< Horizontal block size setting.<br>
															value range		:[1 - 15]<br>
															target registor	:@@SPTHBC */
	USHORT						upper_clip_value;		/**< upper limit clip value used in clip processing.<br>
															value range		:[0 - 32767]<br>
															target registor	:@@SPTCLPH */
	USHORT						lower_clip_value;		/**< lower limit clip value used in clip processing.<br>
															value range		:[0 - 32767]<br>
															target registor	:@@SPTCLPL */
	USHORT						correction_through_val;	/**< The value that does not the correction.<br>
															value range		:[0 - 32767]<br>
															target registor	:@@SPTNOPV */
} T_IM_PRO_SPT_CTRL;

/** Shading Table address information
*/
typedef struct {
	SHORT*	pshd_tbl;		/**< table address pointer for SPTTBL0	*/
	USHORT	size;			/**< set table size for SPTTBL0<br>
								value range		:[0 - 256] number of array	*/
} T_IM_PRO_SPT_TABLE;

/*	@}*/	// im_pro_pas_spt

/** @addtogroup im_pro_stat_flck FLCK
@{
*/
/** PRO FLCK Area information
*/
typedef struct {
	USHORT	pos_x;		/**< Compensation horizontal direction start position<br>
							value range		:[0 - 8190](Multiples of 2)<br>
							target registor	:@@FLH */
	USHORT	pos_y;		/**< Compensation vertical direction start position<br>
							value range		:[0 - 16382](Multiples of 2)<br>
							target registor	:@@FLV */
	USHORT	width;		/**< horizontal size<br>
							value range		:[6 - 8192] 2pixel boundary<br>
							target registor	:@@FLHW */
	USHORT	lines;		/**< vertical size<br>
							value range		:[2 - 1024] 2pixel boundary(FLWLMD=0)<br>
														4pixel boundary(FLWLMD=1)<br>
							target registor	:@@FLVW */
} T_IM_PRO_FLCK_AREA;

/** PRO FLCK generation mode
*/
typedef struct {
	E_IM_PRO_FLCK_TYPE			flck_type;								/**< FLCK output data type<br>
																			value range		:See @ref E_IM_PRO_FLCK_TYPE
																			target registor	:@@FLWPMD */
	E_IM_PRO_FLCK_MODE			flck_mode;								/**< FLCK detecting mode<br>
																			value range		:See @ref E_IM_PRO_FLCK_MODE
																			target registor	:@@FLWLMD */
	USHORT						flcvw;									/**< FLCK Addition permission size in a block information<br>
																			 value range :[2 - 1024] 2pixel boundary(FLWLMD=0)<br>
																									 4pixel boundary(FLWLMD=1)<br>
																			 target registor :@@FLCVW */
	USHORT						bc_num;									/**< FLCK Window block number<br>
																			value range		:[1 - 4096]<br>
																			target registor	:@@FLBC */
} T_IM_PRO_FLCK_CTRL;

/** PRO FLCK RGB gate threshold Information
*/
typedef struct {
	ULONG	fltduy;		/**< FLCK RGB Level gate value Y(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDUY */
	ULONG	fltdur;		/**< FLCK RGB Level gate value R(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDUR */
	ULONG	fltdug;		/**< FLCK RGB Level gate value G(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDUG */
	ULONG	fltdub;		/**< FLCK RGB Level gate value B(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDUB */
	ULONG	fltdly;		/**< FLCK RGB Level gate value Y(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDLY */
	ULONG	fltdlr;		/**< FLCK RGB Level gate value R(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDLR */
	ULONG	fltdlg;		/**< FLCK RGB Level gate value G(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDLG */
	ULONG	fltdlb;		/**< FLCK RGB Level gate value B(upper)<br>
							value range		:[0 - 255]<br>
							target registor	:@@FLTDLB */
} T_IM_PRO_FLCK_LV_GATE;

/** PRO FLCK matrix level gate Information
*/
typedef struct {
	UCHAR	flmlucy;	/**< FLCK matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@@@FLMLUCY	*/
	UCHAR	flmllcy;	/**< FLCK matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@@@FLMLLCY	*/
	UCHAR	flmlucb;	/**< FLCK matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@@@FLMLUCB	*/
	UCHAR	flmllcb;	/**< FLCK matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@@@FLMLLCB	*/
	UCHAR	flmlucr;	/**< FLCK matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@@@FLMLUCR	*/
	UCHAR	flmllcr;	/**< FLCK matrix level gate threshold value<br>
							value range		:[0 - 255]<br>
							target registor	:@@@@FLMLLCR	*/
	SHORT	flmc[3][3];	/**< FLCK matrix level gate value<br>
							value range		:[0 - 511(-2.0 - +1.9921875)]<br>
							target registor	:@@FLMC00-@@FLMC22 	*/
} T_IM_PRO_FLCK_MATRIX_LV_GATE;


/*	@}*/	// im_pro_stat_flck

/** @addtogroup im_pro_sen_obd OBD
@{
*/

/** PRO OB Control information
*/
typedef struct {
	USHORT				ob_max_value;	/**< OB Input upper threshold value<br>
											value range		:[0 - 16383]<br>
											target registor	:@@OBDHTHBIT */
	USHORT				ob_min_value;	/**< OB Input lower threshold value<br>
											value range		:[0 - 16383]<br>
											target registor	:@@OBDLTHBIT */
} T_IM_PRO_OBD_CTRL;

/*	@}*/	// im_pro_sen_obd

/** @addtogroup im_pro_common_p2m P2M
@{
*/

/** P2M Control Information.
*/
typedef struct {
	E_IM_PRO_P2M_KNEE_ENABLE	knee_enable;	/**< P2M Knee enable<br>
													target registor	:P2MCTL@@PKNEMD */
	E_IM_PRO_P2M_LPF_MODE		lpf_mode;		/**< P2M LPF record mode<br>
													target registor	:P2MCTL@@PLPF */
	E_IM_PRO_P2M_REC_MODE		rec_mode;		/**< P2M record mode<br>
													target registor	:P2MCTL@@P2MTYP */
	SHORT						shift_value;	/**< P2M output data shift value<br>
													value range		:[-16384 - 16383]<br>
													target registor	:@@P2MOFS */
	E_IM_PRO_P2M_SHIFT			shift_bit;		/**< P2M output data right shift<br>
													target registor	:@@PBSFT */
	USHORT						clip_value;		/**< P2M upper limit clip<br>
													value range		:[0 - 65535]<br>
													target registor	:@@PCLPH */
	USHORT						start_y;		/**< P2M trimming start Y position<br>
													value range		:[0 - 8191]<br>
													target registor	:@@PTRMV */
	USHORT						start_x;		/**< P2M trimming start X position<br>
													value range		:[0 - 12287]<br>
													target registor	:@@PTRMH */
	USHORT						height;			/**< P2M trimming height<br>
													value range		:[1 - 8192]<br>
													target registor	:@@PTRMVW */
	USHORT						width;			/**< P2M trimming width<br>
													value range		:[1 - 12288]<br>
													target registor	:@@PTRMHW */
	UCHAR						vertical_thin;	/**< P2M vertical thinning out cycle<br>
													value range		:[0:64line, 1 - 63]<br>
													target registor	:@@PTRMXVCYC */
	UCHAR						horizon_thin;	/**< P2M horizon thinning out cycle<br>
													value range		:[0:64pixel, 1 - 63]<br>
													target registor	:@@PTRMXHCYC */
	ULLONG						sample_line;	/**< P2M sampling use line<br>
													value range		:[0 - 0xFFFF_FFFF_FFFF_FFFF]<br>
													target registor	:@@PTRMXVEN */
	ULLONG						sample_width;	/**< P2M sampling use pixel<br>
													value range		:[0 - 0xFFFF_FFFF_FFFF_FFFF]<br>
													target registor	:@@PTRMXHEN */
	E_IM_PRO_P2M_MIRROR_ENABLE	enable_mirror;	/**< enable the mode which writes the trimmed image to SDRAM with horizontal reversed.<br>
													target registor	:@@P2MMIR */
	E_IM_PRO_P2M_INPUT_MODE		input_mode;		/**< P2M input mode setting.<br>
													target registor	:@@P2M2PMD */
} T_IM_PRO_P2M_CTRL;

/** P2M LUT Table address information
*/
typedef struct {
	USHORT*	pp2m_tbl;		/**< table address pointer for PKNETBL	*/
	USHORT	size;			/**< set table size for PKNETBL<br>
								value range		:[1 - 257] number of array	*/
} T_IM_PRO_P2M_TABLE;

/*	@}*/

/** @addtogroup im_pro_common_pwch PWCH
@{
*/


/** PRO PWch Control information
*/
typedef struct {
	E_IM_PRO_PWCH_TRANS				trans_to;			/**< PWch transfer demand point<br>
														target registor	:@@PWS */
	E_IM_PRO_PWCH_CNT				req_threshold;		/**< precedent request threshold<br>
														target registor	:@@PWLV */
	E_IM_PRO_PWCH_WRITE_MASK		write_mask;			/**< writing with mask control.<br>
														target registor	:@@PWMW */
	E_IM_PRO_PWCH_BURST_ALIGN_SIZE	burst_align_size;	/**< burst alignment size.<br>
														target registor	:@@PWDW */
	E_IM_PRO_PWCH_OUTPUT_FORMAT		output_format;		/**< Output format select.<br>.<br>
														target registor	:@@PWVFM */
} T_IM_PRO_PWCH_CTRL;

/** PRO PWch storing management information
*/
typedef struct {
	UCHAR	buffer_num;							/**< The Number of Bayer Buffering<br>
													value range		:[1-D_IM_PRO_PWCH_BUFFER_MAX]	*/
	ULONG	buff_addr[D_IM_PRO_PWCH_BUFFER_MAX];/**< Buffer address @@PWSA 8byte boundary,<br>
														but as the alignment processing is performed in 32byte unit, it is recommended to set this value to a multiple of 32byte.<br>
														To perform the alignment processing properly when this value is set to a value which is not a multiple of 32byte,<br>
														write with mask must be enabled.(@ref Im_PRO_PWch_Ctrl() argument of @ref E_IM_PRO_PWCH_WRITE_MASK.)	*/
} T_IM_PRO_PWCH_INFO;

/*	@}*/


/** @addtogroup im_pro_common_prch PRCH
@{
*/

/** PRO PRch Control information
*/
typedef struct {
	E_IM_PRO_PRCH_BURST	trans_mode;				/**< PRch transfer burst mode<br>
													target registor	:@@PRBMD */
	E_IM_PRO_PRCH_CNT	req_threshold;			/**< precedent request threshold<br>
													target registor	:@@PRLV */
	E_IM_PRO_PRCH_TRANS	trans_to;				/**< PRch transfer demand point<br>
													target registor	:@@PRS */
} T_IM_PRO_PRCH_CTRL;

/** PRO PRch Area information
*/
typedef struct {
	ULONG	width;				/**< PRch input data width(byte)<br>
									value range		: 24bit length [0 - 16777215]<br>
									target registor	:@@PRHSIZE */
	USHORT	height;				/**< PRch input data height(line number)<br>
									value range		: 14bit length [0 - 16383]<br>
									target registor	:@@PRVSIZE */
	ULONG	global_width;		/**< Level size of the whole input data domain(byte)<br>
									value range		: 32bit length [0 - 4294967295]<br>
									target registor	:@@PRLSIZE */
} T_IM_PRO_PRCH_AREA;

/*	@}*/

/** @addtogroup im_pro_common_m2p M2P
@{
*/

/** PRO M2P Control information
*/
typedef struct {
	E_IM_PRO_M2P_DATA_TYPE		data_type;					/**< M2P data type<br>
																target registor	:@@M2PTYP */
	E_IM_PRO_M2P_DEKNEE_ENABLE	deknee_enable;				/**< M2P Deknee enable<br>
																target registor	:@@MDKNEN */
	E_IM_PRO_M2P_SHIFT			shift_bit;					/**< M2P output data left shift<br>
																value range		:See @ref E_IM_PRO_M2P_SHIFT<br>
																target registor	:@@M2PBSFT */
	E_IM_PRO_M2P_SAT			shift_sat;					/**< M2P shift saturation compensation<br>
																value range		:See @ref E_IM_PRO_M2P_SAT<br>
																target registor	:@@M2PBMD */
	E_IM_PRO_M2P_BIT_SHIFT		bit_shift;					/**< M2P bit shift direction<br>
																value range		:See @ref E_IM_PRO_M2P_BIT_SHIFT<br>
																target registor	:@@M2PBLR */
	UCHAR						valid_bit_count;			/**< valid bit count after bit shift processing.<br>
																value range		:[8 - 16]<br>
																target registor	:@@M2PBT */
	SHORT						shift_value;				/**< M2P output data shift value<br>
																value range		:[-16384 - 16383]<br>
																target registor	:@@M2POFS */
	USHORT						upper_limit_clip_value;		/**< upper limit clip value used in clip processing.<br>
																value range		:[0 - 65535]<br>
																target registor	:@@M2PCLIPHI */
	SHORT						lower_limit_clip_value;		/**< lower limit clip value used in clip processing.<br>
																value range		:[-32768 - 32767]<br>
																target registor	:@@M2PCLIPLW */
} T_IM_PRO_M2P_CTRL;

/** M2P DEKNEE Table address information
*/
typedef struct {
	SHORT*	pm2p_tbl;		/**< table address pointer for M2P0DKNTBL	*/
	USHORT	size;			/**< set table size for M2P0DKNTBL<br>
								value range		:[1 - 256] number of array	*/
} T_IM_PRO_M2P_TABLE;

/*	@}*/

/** @addtogroup im_pro_sen_top SENTOP
@{
*/

/** SENTOP Macro Control Information
*/
typedef struct {
	E_IM_PRO_SENTOP_CONN_TYPE_SEL			sen_connect_type_sel[D_IM_PRO_SEN_SENCORE_NUM];	/**< Select an input sensor type of each SENCORE. SENTOPCTL1@@SENIN0-@@SENIN3 */
	E_IM_PRO_SENTOP_INPUT_TO_BLOB			blob_input_sel[D_IM_PRO_SENTOP_BLOB_NUM];			/**< Select an input block type of each BLOB. SENTOPCTL1@@BLOBSET0-@@BLOBSET3 */
	E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE		sdram_output_type[D_IM_PRO_SENTOP_PWSW_NUM];		/**< Select a type of output to SDRAM from LDIV. SENTOPCTL1@@PWSW0-@@PWSW3 */
	E_IM_PRO_SENTOP_OUT_SEL					output_sel_sro2;									/**< Select an output block to SRO2 I/F. SENTOPCTL1@@PIPE2S */
	E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL	sencore_hmax_size_ctrl;								/**< Select a SENCORE horizontal maxumum size control. SENTOPCTL1@@SENRAMSW */
	E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL		ldiv_hmax_size_ctrl;								/**< Select a LDIV horizontal maxumum size control. SENTOPCTL1@@LDIVRAMSW */
	E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL	ldiv_multi_ch_input_ctrl;							/**< Select a LDIV multi channel input control. SENTOPCTL1@@LDIVMCH */
	E_IM_PRO_ORG							first_pixel[D_IM_PRO_SENTOP_BLOB_NUM][2];			/**< Select a Bayer first pixel for each PAT block input SENTOPCTL2@@ORG00-@@ORG31 */
} T_IM_PRO_SENTOP_CTRL;

/** SENTOP BitShift Macro Control Information
*/
typedef struct {
	E_IM_PRO_SENTOP_BS_SHIFT				shift_value[D_IM_PRO_SEN_BS_NUM];				/**< Bit shift value for BS0 ~ BS3 macro.<br>
																									target registor	:BSAMNTx@@BSCTL	*/
	E_IM_PRO_SENTOP_BS_SAT					sat_compensation[D_IM_PRO_SEN_BS_NUM];			/**< Saturation compensation for BS0 ~ BS3 macro. [0:Disable 1:Enable]<br>
																									target registor	:BSLMDx@@BSCTL	*/
	E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR		shift_dir[D_IM_PRO_SEN_BS_NUM];					/**< Bit shift direction value for BS0 ~ BS3 macro.<br>
																									target registor	:BSLRx@@BSCTL	*/
} T_IM_PRO_SENTOP_BS_CTRL;

/** SENTOP OBT Macro Control Information
*/
typedef struct {
	E_IM_PRO_SENTOP_OBT_COMM_PARTTERN		obt_comm_perttern;									/**< Select an OBT macro communication perttern. OBTCDCCTL1@@OBTCPAT */
} T_IM_PRO_SENTOP_OBT_CTRL;

/** OBT communication parttern
*/
typedef struct {
	UCHAR									coeff[4][2];										/**< OBT weight coefficient <br>
																									target registor	:OBTCDCCTL2@@OBTDWC00 - @@OBTDWC31	*/
} T_IM_PRO_SENTOP_OBT_WEIGHT_COEFF;

/** SENTOP LVDS Macro Power-Down Control
*/
typedef struct {
	USHORT								clock_lane_powerdown;									/**< LVDS PHY clock lane control. LVDSCTL@@LVDSCKPD */
	USHORT								data_lane_powerdown;									/**< LVDS PHY data lane control. LVDSCTL@@LVDSDLPD */
} T_IM_PRO_SENTOP_LVDS_CTRL;

/** SENTOP SLVS Macro Control
*/
typedef struct {
	BOOL								clk_ctrl_enabled;										/**< Enable Clock control.<br>
																									FALSE: skip the setting the phy0_rxpclk2aen and phy1_rxpclk2aen.<br>
																									TRUE:  set the phy0_rxpclk2aen and phy1_rxpclk2aen.  */
	E_IM_PRO_SENTOP_SLVS_LINK_CONN		link_mode;												/**< SLVS link connect control. SLVSECCTL@@SLVSECSEL */
	UCHAR								phy0_rxpdck;											/**< ComboPHY0 "I_RXPDCK" pin control. SLVSECCTL@@PHY0_RXPDCK<br>
																									detail of this setting value, please refer to ComboPHY specifications. */
	UCHAR								phy1_rxpdck;											/**< ComboPHY0 "I_RXPDCK" pin control. SLVSECCTL@@PHY1_RXPDCK<br>
																									detail of this setting value, please refer to ComboPHY specifications. */
	UCHAR								phy2_rxpdck;											/**< ComboPHY0 "I_RXPDCK" pin control. SLVSECCTL@@PHY2_RXPDCK<br>
																									detail of this setting value, please refer to ComboPHY specifications. */
	UCHAR								phy3_rxpdck;											/**< ComboPHY0 "I_RXPDCK" pin control. SLVSECCTL@@PHY3_RXPDCK<br>
																									detail of this setting value, please refer to ComboPHY specifications. */
	UCHAR								phy0_rxpclk2aen;										/**< ComboPHY0 "I_RXPCLK2AEN" pin control. SLVSECCTL@@PHY0_RXPCLK2AEN<br>
																									detail of this setting value, please refer to ComboPHY specifications. */
	UCHAR								phy1_rxpclk2aen;										/**< ComboPHY1 "I_RXPCLK2AEN" pin control. SLVSECCTL@@PHY1_RXPCLK2AEN<br>
																									detail of this setting value, please refer to ComboPHY specifications. */
} T_IM_PRO_SENTOP_SLVS_CTRL;

/** SENTOP SGHDEG0 Macro Control
*/
typedef struct {
	E_IM_PRO_SENTOP_SG_IF_OUTPUT_SEL	sg_if_output_sel[D_IM_PRO_SEN_SG_NUM];					/**< Select an SG I/F output control. SGOUTCTL@@SGSEL0 - @@SGSEL3 */
} T_IM_PRO_SENTOP_SG_CTRL;

/** ComboPHY Common I/F Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								pregcnt;												/**< ComboPHY "I_PREGCNT" pin control.   @@PHY_PREGCNT. */
	UCHAR								freqcnt;												/**< ComboPHY "I_FREQCNT" pin control.   @@PHY_FREQCNT. */
	UCHAR								pdccnt;													/**< ComboPHY "I_PDCNT" pin control.     @@PHY_PDCCNT. */
	UCHAR								setdf;													/**< ComboPHY "I_SETDF" pin control.     @@PHY_SETDF. */
	UCHAR								picodetst;												/**< ComboPHY "I_PICODETST" pin control. @@PHY_PICODETST. */
	UCHAR								pmonsel;												/**< ComboPHY "I_PMONSEL" pin control.   @@PHY_PMONSEL. */
	UCHAR								dmonsel;												/**< ComboPHY "I_DMONSEL" pin control.   @@PHY_DMONSEL. */
	UCHAR								cntsel;													/**< ComboPHY "I_CNTSEL" pin control.    @@PHY_CNTSEL. */
	UCHAR								mulgainf;												/**< ComboPHY "I_MULGAINF" pin control.  @@PHY_MULGAINF. */
	UCHAR								mulgainp;												/**< ComboPHY "I_MULGAINP" pin control.  @@PHY_MULGAINP. */
	UCHAR								cdrfunc;												/**< ComboPHY "I_CDRFUNC" pin control.   @@PHY_CDRFUNC. */
} T_IM_PRO_SENTOP_CPHY_COMMON_IF_CTRL;

/** ComboPHY Clock Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								clklanesel;												/**< ComboPHY "I_CLKLANESEL" pin control. @@PHYx_CLKLANESEL. */
	UCHAR								clkmodesel;												/**< ComboPHY "I_CLKMODESEL" pin control. @@PHYx_CLKMODESEL. */
	UCHAR								clkoutren;												/**< ComboPHY "I_CLKOUTREN" pin control.  @@PHYx_CLKOUTREN. */
	UCHAR								clkoutlen;												/**< ComboPHY "I_CLKOUTLEN" pin control.  @@PHYx_CLKOUTLEN. */
} T_IM_PRO_SENTOP_CPHY_CLK_CTRL;

/** ComboPHY Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								bitwidthsel;											/**< ComboPHY "I_BITWIDTHSEL" pin control. @@PHYx_BITWIDTHSEL. */
	UCHAR								rxmode;													/**< ComboPHY "I_RXMODE" pin control.      @@PHYx_RXMODE. */
	UCHAR								pimode;													/**< ComboPHY "I_PIMODE" pin control.      @@PHYx_PIMODE. */
	UCHAR								picnt;													/**< ComboPHY "I_PICNT" pin control.       @@PHYx_PICNT. */
} T_IM_PRO_SENTOP_CPHY_MODE_CTRL;

/** ComboPHY Common Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								bitwidthsel;											/**< ComboPHY "I_BITWIDTHSEL" pin control. @@PHYx_BITWIDTHSEL. */
	UCHAR								rxmode;													/**< ComboPHY "I_RXMODE" pin control.      @@PHYx_RXMODE. */
	UCHAR								d0setsel;												/**< ComboPHY "I_D0SETSEL" pin control.    @@PHYx_D0SETSEL. */
	UCHAR								d1setsel;												/**< ComboPHY "I_D1SETSEL" pin control.    @@PHYx_D1SETSEL. */
	UCHAR								d2setsel;												/**< ComboPHY "I_D2SETSEL" pin control.    @@PHYx_D2SETSEL. */
	UCHAR								d3setsel;												/**< ComboPHY "I_D3SETSEL" pin control.    @@PHYx_D3SETSEL. */
} T_IM_PRO_SENTOP_CPHY_COMMON_MODE_CTRL;

/** ComboPHY SLVS-EC Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_COMMON_MODE_CTRL	common_ctrl;										/**< ComboPHY Common Mode Control. */
	UCHAR									pimode;												/**< ComboPHY "I_PIMODE" pin control.      @@PHYx_PIMODE. */
	UCHAR									picnt;												/**< ComboPHY "I_PICNT" pin control.       @@PHYx_PICNT. */
} T_IM_PRO_SENTOP_CPHY_SLVS_MODE_CTRL;

/** ComboPHY Sub-LVDS Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_COMMON_MODE_CTRL	common_ctrl;										/**< ComboPHY Common Mode Control. */
} T_IM_PRO_SENTOP_CPHY_LVDS_MODE_CTRL;

/** ComboPHY MIPI D-PHY Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_COMMON_MODE_CTRL	common_ctrl;										/**< ComboPHY Common Mode Control. */
	UCHAR									rxspden;											/**< ComboPHY "I_RXSPDEN" pin control.  @@PHYx_RXSPDEN. */
	UCHAR									dcntnum;											/**< ComboPHY "I_DCNTNUM" pin control.  @@PHYx_DCNTNUM. */
	UCHAR									grdwidth;											/**< ComboPHY "I_GRDWIDTH" pin control.  @@PHYx_GRDWIDTH. */
	UCHAR									rdwait;												/**< ComboPHY "I_RDWAIT" pin control.  @@PHYx_RDWAIT. */
	UINT32									funcseld0;											/**< ComboPHY "I_FUNCSELD0" pin control.   @@PHYx_FUNCSELD0. */
	UCHAR									vldprd;												/**< ComboPHY "I_VLDPRD" pin control.  @@PHY0_VLDPRD. */
} T_IM_PRO_SENTOP_CPHY_DMIPI_MODE_CTRL;

/** ComboPHY MIPI D-PHY Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_COMMON_MODE_CTRL	common_ctrl;										/**< ComboPHY Common Mode Control. */
	UCHAR									eq_en_cp0;											/**< ComboPHY "I_EQ_EN_CP0" pin control.    @@PHYx_EQ_EN_CP0. */
	UCHAR									eq_en_cp1;											/**< ComboPHY "I_EQ_EN_CP1" pin control.    @@PHYx_EQ_EN_CP1. */
	UCHAR									eq_en_cp2;											/**< ComboPHY "I_EQ_EN_CP2" pin control.    @@PHYx_EQ_EN_CP2. */
	UCHAR									eq_set_cp0;											/**< ComboPHY "I_EQ_SET_CP0" pin control.   @@PHYx_EQ_SET_CP0. */
	UCHAR									eq_set_cp1;											/**< ComboPHY "I_EQ_SET_CP1" pin control.   @@PHYx_EQ_SET_CP1. */
	UCHAR									eq_set_cp2;											/**< ComboPHY "I_EQ_SET_CP2" pin control.   @@PHYx_EQ_SET_CP2. */
	UCHAR									skwsel_cp0;											/**< ComboPHY "I_SKEWADJSE0" pin control. @@PHYx_SKWSEL_CP0. */
	UCHAR									skwsel_cp1;											/**< ComboPHY "I_SKEWADJSE1" pin control. @@PHYx_SKWSEL_CP1. */
	UCHAR									skwsel_cp2;											/**< ComboPHY "I_SKEWADJSE2" pin control. @@PHYx_SKWSEL_CP2. */
	UCHAR									zerop3uicp0;										/**< ComboPHY "I_ZEROP3UI_CP0" pin control. @@PHYx_ZEROP3UICP0. */
	UCHAR									zerop4uicp0;										/**< ComboPHY "I_ZEROP4UI_CP0" pin control. @@PHYx_ZEROP4UICP0. */
	UCHAR									zerop5uicp0;										/**< ComboPHY "I_ZEROP5UI_CP0" pin control. @@PHYx_ZEROP5UICP0. */
	UCHAR									zerop6uicp0;										/**< ComboPHY "I_ZEROP6UI_CP0" pin control. @@PHYx_ZEROP6UICP0. */
	UCHAR									zerop3uicp1;										/**< ComboPHY "I_ZEROP3UI_CP1" pin control. @@PHYx_ZEROP3UICP1. */
	UCHAR									zerop4uicp1;										/**< ComboPHY "I_ZEROP4UI_CP1" pin control. @@PHYx_ZEROP4UICP1. */
	UCHAR									zerop5uicp1;										/**< ComboPHY "I_ZEROP5UI_CP1" pin control. @@PHYx_ZEROP5UICP1. */
	UCHAR									zerop6uicp1;										/**< ComboPHY "I_ZEROP6UI_CP1" pin control. @@PHYx_ZEROP6UICP1. */
	UCHAR									zerop3uicp2;										/**< ComboPHY "I_ZEROP3UI_CP2" pin control. @@PHYx_ZEROP3UICP2. */
	UCHAR									zerop4uicp2;										/**< ComboPHY "I_ZEROP4UI_CP2" pin control. @@PHYx_ZEROP4UICP2. */
	UCHAR									zerop5uicp2;										/**< ComboPHY "I_ZEROP5UI_CP2" pin control. @@PHYx_ZEROP5UICP2. */
	UCHAR									zerop6uicp2;										/**< ComboPHY "I_ZEROP6UI_CP2" pin control. @@PHYx_ZEROP6UICP2. */
	UCHAR									detcnt_cp0;											/**< ComboPHY "I_DETCNT_CP0" pin control.   @@PHYx_DETCNT_CP0. */
	UCHAR									detcnt_cp1;											/**< ComboPHY "I_DETCNT_CP1" pin control.   @@PHYx_DETCNT_CP1. */
	UCHAR									detcnt_cp2;											/**< ComboPHY "I_DETCNT_CP2" pin control.   @@PHYx_DETCNT_CP1. */
} T_IM_PRO_SENTOP_CPHY_CMIPI_MODE_CTRL;

/** ComboPHY CP Mode Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								zerop3uicp0;											/**< ComboPHY "I_ZEROP3UI_CP0" pin control. @@PHYx_ZEROP3UICP0. */
	UCHAR								zerop4uicp0;											/**< ComboPHY "I_ZEROP4UI_CP0" pin control. @@PHYx_ZEROP4UICP0. */
	UCHAR								zerop5uicp0;											/**< ComboPHY "I_ZEROP5UI_CP0" pin control. @@PHYx_ZEROP5UICP0. */
	UCHAR								zerop6uicp0;											/**< ComboPHY "I_ZEROP6UI_CP0" pin control. @@PHYx_ZEROP6UICP0. */
	UCHAR								zerop3uicp1;											/**< ComboPHY "I_ZEROP3UI_CP1" pin control. @@PHYx_ZEROP3UICP1. */
	UCHAR								zerop4uicp1;											/**< ComboPHY "I_ZEROP4UI_CP1" pin control. @@PHYx_ZEROP4UICP1. */
	UCHAR								zerop5uicp1;											/**< ComboPHY "I_ZEROP5UI_CP1" pin control. @@PHYx_ZEROP5UICP1. */
	UCHAR								zerop6uicp1;											/**< ComboPHY "I_ZEROP6UI_CP1" pin control. @@PHYx_ZEROP6UICP1. */
	UCHAR								zerop3uicp2;											/**< ComboPHY "I_ZEROP3UI_CP2" pin control. @@PHYx_ZEROP3UICP2. */
	UCHAR								zerop4uicp2;											/**< ComboPHY "I_ZEROP4UI_CP2" pin control. @@PHYx_ZEROP4UICP2. */
	UCHAR								zerop5uicp2;											/**< ComboPHY "I_ZEROP5UI_CP2" pin control. @@PHYx_ZEROP5UICP2. */
	UCHAR								zerop6uicp2;											/**< ComboPHY "I_ZEROP6UI_CP2" pin control. @@PHYx_ZEROP6UICP2. */
	UCHAR								detcnt_cp0;												/**< ComboPHY "I_DETCNT_CP0" pin control.   @@PHYx_DETCNT_CP0. */
	UCHAR								detcnt_cp1;												/**< ComboPHY "I_DETCNT_CP1" pin control.   @@PHYx_DETCNT_CP1. */
	UCHAR								detcnt_cp2;												/**< ComboPHY "I_DETCNT_CP2" pin control.   @@PHYx_DETCNT_CP1. */
	UCHAR								eq_en_cp0;												/**< ComboPHY "I_EQ_EN_CP0" pin control.    @@PHYx_EQ_EN_CP0. */
	UCHAR								eq_en_cp1;												/**< ComboPHY "I_EQ_EN_CP1" pin control.    @@PHYx_EQ_EN_CP1. */
	UCHAR								eq_en_cp2;												/**< ComboPHY "I_EQ_EN_CP2" pin control.    @@PHYx_EQ_EN_CP2. */
	UCHAR								eq_set_cp0;												/**< ComboPHY "I_EQ_SET_CP0" pin control.   @@PHYx_EQ_SET_CP0. */
	UCHAR								eq_set_cp1;												/**< ComboPHY "I_EQ_SET_CP1" pin control.   @@PHYx_EQ_SET_CP1. */
	UCHAR								eq_set_cp2;												/**< ComboPHY "I_EQ_SET_CP2" pin control.   @@PHYx_EQ_SET_CP2. */
} T_IM_PRO_SENTOP_CPHY_CPMODE_CTRL;

/** ComboPHY Delay line Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								d0rxspeed;												/**< ComboPHY "I_D0RXSPEED" pin control.  @@PHYx_D0RXSPEED. */
	UCHAR								d1rxspeed;												/**< ComboPHY "I_D1RXSPEED" pin control.  @@PHYx_D1RXSPEED. */
	UCHAR								d2rxspeed;												/**< ComboPHY "I_D2RXSPEED" pin control.  @@PHYx_D2RXSPEED. */
	UCHAR								d3rxspeed;												/**< ComboPHY "I_D3RXSPEED" pin control.  @@PHYx_D3RXSPEED. */
	UCHAR								skwsel_cp0;												/**< ComboPHY "I_SKEWADJSE0" pin control. @@PHYx_SKWSEL_CP0. */
	UCHAR								skwsel_cp1;												/**< ComboPHY "I_SKEWADJSE1" pin control. @@PHYx_SKWSEL_CP1. */
	UCHAR								skwsel_cp2;												/**< ComboPHY "I_SKEWADJSE2" pin control. @@PHYx_SKWSEL_CP2. */
} T_IM_PRO_SENTOP_CPHY_DELAY_CTRL;

/** ComboPHY IF Extension Signals Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								clksetsel;												/**< ComboPHY "I_CLKSETSEL" pin control.   @@PHYx_CLKSETSEL. */
	UCHAR								cphyset_cp0;											/**< ComboPHY "I_CPHYSET_CP0" pin control. @@PHYx_CPHYSET_CP0. */
	UCHAR								cphyset_cp1;											/**< ComboPHY "I_CPHYSET_CP1" pin control. @@PHYx_CPHYSET_CP1. */
	UCHAR								cphyset_cp2;											/**< ComboPHY "I_CPHYSET_CP2" pin control. @@PHYx_CPHYSET_CP2. */
	USHORT								funcselck;												/**< ComboPHY "I_FUNCSELCK" pin control.   @@PHYx_FUNCSELCK. */
	UINT32								funcseld0;												/**< ComboPHY "I_FUNCSELD0" pin control.   @@PHYx_FUNCSELD0. */
	UINT32								funcseld1;												/**< ComboPHY "I_FUNCSELD1" pin control.   @@PHYx_FUNCSELD1. */
	UINT32								funcseld2;												/**< ComboPHY "I_FUNCSELD2" pin control.   @@PHYx_FUNCSELD2. */
	UINT32								funcseld3;												/**< ComboPHY "I_FUNCSELD3" pin control.   @@PHYx_FUNCSELD3. */
} T_IM_PRO_SENTOP_CPHY_IF_EXT_SIGNAL_CTRL;

/** ComboPHY DeSkew Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								rxspden;												/**< ComboPHY "I_RXSPDEN" pin control.  @@PHYx_RXSPDEN. */
	UCHAR								dcntnum;												/**< ComboPHY "I_DCNTNUM" pin control.  @@PHYx_DCNTNUM. */
	UCHAR								grdwidth;												/**< ComboPHY "I_GRDWIDTH" pin control.  @@PHYx_GRDWIDTH. */
	UCHAR								rdwait;													/**< ComboPHY "I_RDWAIT" pin control.  @@PHYx_RDWAIT. */
	UCHAR								vldprd;													/**< ComboPHY "I_VLDPRD" pin control.  @@PHY0_VLDPRD. */
} T_IM_PRO_SENTOP_CPHY_DESKNEW_CTRL;

/** ComboPHY PLL0 Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								ibcnt;													/**< ComboPHY "I_IBCNT" pin control.  @@PLL0_IBCNT. */
	UCHAR								ilfsel;													/**< ComboPHY "I_ILFSEL" pin control.  @@PLL0_ILFSEL. */
	UCHAR								ioclken;												/**< ComboPHY "I_IOCLKEN" pin control.  @@PLL0_IOCLKEN. */
	UCHAR								itxpclk2aen;											/**< ComboPHY "I_ITXPCLK2AEN" pin control.  @@PLL0_ITXPCLK2AEN. */
} T_IM_PRO_SENTOP_CPHY_PLL0_CTRL;

/** ComboPHY BIAS Control.<br>
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								phy0_xbgrpd;											/**< ComboPHY "I_XBGRPD" pin control.  @@PHY0_XBGRPD. */
	UCHAR								phy1_xbgrpd;											/**< ComboPHY "I_XBGRPD" pin control.  @@PHY1_XBGRPD. */
	UCHAR								phy2_xbgrpd;											/**< ComboPHY "I_XBGRPD" pin control.  @@PHY2_XBGRPD. */
	UCHAR								phy3_xbgrpd;											/**< ComboPHY "I_XBGRPD" pin control.  @@PHY3_XBGRPD. */
} T_IM_PRO_SENTOP_CPHY_BIAS_CTRL;

/** ComboPHY Control
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_COMMON_IF_CTRL	common_if_ctrl;											/**< ComboPHY Common I/F Control. */
	T_IM_PRO_SENTOP_CPHY_CPMODE_CTRL	cpmode_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];					/**< ComboPHY CP mode Control. */
	T_IM_PRO_SENTOP_CPHY_DELAY_CTRL		delay_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];					/**< ComboPHY Delay line Control. */
	T_IM_PRO_SENTOP_CPHY_DESKNEW_CTRL	desknew_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];					/**< ComboPHY DeSkew I/F Control. */
	T_IM_PRO_SENTOP_CPHY_PLL0_CTRL		pll0_ctrl;												/**< ComboPHY PLL0 I/F Control. */
} T_IM_PRO_SENTOP_CPHY_CTRL;

/** ComboPHY MIPI D-PHY DesKnew Monitor pin status.
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	UCHAR								vldwindow2	:7;											/**< ComboPHY "I_VLDWINDOW2" pin control.  @@PHYx_VLDWINDOW2.	*/
	USHORT								dummy01		:1;											/**< Not used													*/
	UCHAR								vldwindow3	:7;											/**< ComboPHY "I_VLDWINDOW3" pin control.  @@PHYx_VLDWINDOW3.	*/
	ULONG								dummy02		:17;										/**< Not used													*/
	UCHAR								dtypemon0	:2;											/**< ComboPHY "I_DTYPEMON0" pin control.  @@PHYx_DTYPEMON0.		*/
	USHORT								dummy03		:2;											/**< Not used													*/
	UCHAR								dtypemon1	:2;											/**< ComboPHY "I_DTYPEMON1" pin control.  @@PHYx_DTYPEMON1.		*/
	USHORT								dummy04		:2;											/**< Not used													*/
	UCHAR								dtypemon2	:2;											/**< ComboPHY "I_DTYPEMON2" pin control.  @@PHYx_DTYPEMON2.		*/
	USHORT								dummy05		:2;											/**< Not used													*/
	UCHAR								dtypemon3	:2;											/**< ComboPHY "I_DTYPEMON3" pin control.  @@PHYx_DTYPEMON3. 	*/
	USHORT								dummy06		:2;											/**< Not used													*/
	UCHAR								vldwindow0	:7;											/**< ComboPHY "I_VLDWINDOW0" pin control.  @@PHYx_VLDWINDOW0.	*/
	USHORT								dummy07		:1;											/**< Not used													*/
	UCHAR								vldwindow1	:7;											/**< ComboPHY "I_VLDWINDOW1" pin control.  @@PHYx_VLDWINDOW1.	*/
	USHORT								dummy08		:1;											/**< Not used													*/
} T_IM_PRO_SENTOP_CPHY_DESKNEW_MONI_STATUS;

/** ComboPHY DesKnew Monitor information.
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_DESKNEW_MONI_STATUS	desknew_moni[4];								/**< ComboPHY0~3 DesKnew Monitor information. */
} T_IM_PRO_SENTOP_CPHY_DESKNEW_MONITOR;

/** ComboPHY Delayline Monitor pin status.
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	ULONG								phy_code_cp2	:18;									/**< ComboPHY "O_CODE_CP2" pin control.  @@PHYx_CODE_CP2.		*/
	USHORT								dummy02			:14;									/**< Not used													*/
	ULONG								phy_code_cp1	:18;									/**< ComboPHY "O_CODE_CP1" pin control.  @@PHYx_CODE_CP1.		*/
	USHORT								dummy01			:14;									/**< Not used													*/
	ULONG								phy_code_cp0	:18;									/**< ComboPHY "O_CODE_CP0" pin control.  @@PHYx_CODE_CP0.		*/
	USHORT								dummy00			:14;									/**< Not used													*/
} T_IM_PRO_SENTOP_CPHY_DELAYLINE_MONI_STATUS;

/** ComboPHY Delayline Monitor information.
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	T_IM_PRO_SENTOP_CPHY_DELAYLINE_MONI_STATUS	delayline_moni[4];								/**< ComboPHY0~3 Delayline Monitor information. */
} T_IM_PRO_SENTOP_CPHY_DELAYLINE_MONITOR;

/** ComboPHY Common I/F Monitor information.
	detail of these setting value, please refer to ComboPHY specifications.
*/
typedef struct {
	ULLONG								phy0_cdrmon;											/**< ComboPHY "O_CDRMON" pin control.  @@PHY0_CDRMON.		*/
	ULLONG								phy1_cdrmon;											/**< ComboPHY "1_CDRMON" pin control.  @@PHY1_CDRMON.		*/
	ULLONG								phy2_cdrmon;											/**< ComboPHY "2_CDRMON" pin control.  @@PHY2_CDRMON.		*/
	ULLONG								phy3_cdrmon;											/**< ComboPHY "3_CDRMON" pin control.  @@PHY2_CDRMON.		*/
} T_IM_PRO_SENTOP_CPHY_COMMON_IF_MONITOR;

/*	@}*/	// im_pro_sen_top


/** @addtogroup im_pro_sro_top SROTOP
@{
*/

/** SROTOP Macro Control Information
*/
typedef struct {
	E_IM_PRO_SROTOP_INPUT_MODE			input_mode;					/**< Select the input mode of SRO unit. SROTOPCTL1@@SROISW */
	E_IM_PRO_SROTOP_MODE				sro_mode_sel;				/**< Select an mode of SRO unit. SROTOPCTL1@@SROMD */
	E_IM_PRO_SROTOP_CAG_TARGET_BLOCK	cag_target_sel;				/**< Select in which unit FFC/CAG macro are used, SRO or B2B. SROTOPCTL1@@CAGSL */
	E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT	bay_out_path_sel;			/**< Select a path for P2M0/1 and PWCH0/1 of SRO unit. SROTOPCTL1@@BRPSW0 */
	E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN	b2b_connect_path_sel;		/**< Select a path for B2B direct connection I/F. SROTOPCTL1@@BRPSW1 */
	E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN	ltm_connect_path_sel;		/**< Select a path for LTM direct connection I/F. SROTOPCTL1@@BRPSW2 */
	E_IM_PRO_ORG						first_pixel[2];				/**< Select an Bayer first pixel for each PAT block input SROTOPCTL2@@ORG0-@@ORG1 */
} T_IM_PRO_SROTOP_CTRL;

/** WEITGEN Macro Control Information
*/
typedef struct {
	USHORT								v_wait_signal_pos;			/**< vertical position of WAIT signal occurs during vertical blanking. @@GENVx <br>
																		value range  :[2 - 16382] */
	USHORT								h_wait_signal_pos;			/**< horizontal position of WAIT signal occurs during horizontal blanking. @@GENHx  <br>
																		value range  :[2 - 16382] */
} T_IM_PRO_SROTOP_WEITGEN_CTRL;

/*	@}*/	// im_pro_sro_top


/** @addtogroup im_pro_b2b_top B2BTOP
@{
*/
/** B2BTOP Macro Control Information
*/
typedef struct {
	E_IM_PRO_B2BTOP_INPUT_MODE			input_mode;					/**< Select the input mode of B2B unit. B2BTOPCTL1@@B2BISW */
	E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK	cag_target_sel;				/**< Select in which unit FFC/CAG macro are used, SRO or B2B. B2BTOPCTL1@@CAGSL */
	E_IM_PRO_ORG						first_pixel[2];				/**< Select an Bayer first pixel for each PAT block input B2BTOPCTL2@@ORG0-@@ORG1 */
} T_IM_PRO_B2BTOP_CTRL;

/*	@}*/	// im_pro_b2b_top

/** @addtogroup im_pro_stat_top STATTOP
@{
*/
/** STATTOP Macro Control Information
*/
typedef struct {
	E_IM_PRO_STATTOP_INPUT_SEL			af_input_sel[D_IM_PRO_STAT_AF_CH_NUM];		/**< select the input to each AF macro. STATTOPCTL1@@AFSW0~@@AFSW1 */
	E_IM_PRO_STATTOP_FLCK_INPUT_SEL		flck_input_sel[D_IM_PRO_STAT_FLCK_CH_NUM];	/**< select the input to each FLCK macro. STATTOPCTL1@@FLSW0~@@FLSW1 */
	E_IM_PRO_STATTOP_INPUT_SEL			aeawb_input_sel[D_IM_PRO_STAT_AEAWB_CH_NUM];	/**< select the input to each AEAWB macro. STATTOPCTL1@@AESW0~@@AESW3 */
	E_IM_PRO_STATTOP_INPUT_SEL			hist_input_sel[D_IM_PRO_STAT_HIST_CH_NUM];	/**< select the input to each HIST macro. STATTOPCTL1@@HISW0~@@HISW3 */
} T_IM_PRO_STATTOP_CTRL;

/*	@}*/	// im_pro_stat_top

/** @addtogroup im_pro_pas_top PASTOP
@{
*/

/** PASTOP Macro Control Information
*/
typedef struct {
	E_IM_PRO_PASTOP_ORG		org;	/**< Select L or R pixel side of PD in frame. PASTOPCTL1@@ORG */
} T_IM_PRO_PASTOP_CTRL;

/*	@}*/	// im_pro_pas_top

/** @addtogroup im_pro_handler Interrupt Handler
@{
*/


/** PRO callback identifer information.
*/
typedef struct {
	UCHAR unit_no;		/**< Unit number. same as @ref E_IM_PRO_UNIT_NUM	 */
	UCHAR block_type;	/**< Block type. same as @ref E_IM_PRO_BLOCK_TYPE	 */
	UCHAR channel;		/**< Channel number.	 */
} T_CALLBACK_ID;

typedef VOID (*T_CALLBACK_LONG)( T_CALLBACK_ID* id, ULONG result, ULONG user_param );	/**< Callback function.Type of argument result is LONG.	*/

/** PRO Interrupt configuration
*/
typedef struct {
	ULONG	interrupt_bit;		/**< interrupt_bit : Specified interrupt bit */
	UCHAR	permission_flg;		/**< Interrupt enable or disable flg value range :[0:disable 1:enable] */
} T_IM_PRO_INT_CFG;

/** PRO Interrupt callback configuration
*/
typedef struct {
	T_CALLBACK_LONG	inthandler;	/**< Interrupt handler address pointer */
	ULONG			user_param;	/**<  This set value will return to callback argument value when interrupt occurs. */
} T_IM_PRO_CALLBACK_CFG;


/** VD/HD dealy control
*/
typedef struct {
	USHORT hd_delay;											/**< HD delay(0-12288) @@HMIHADRxx */
	USHORT vd_h_delay;											/**< VD H delay(0-12288) @@VMIHADRxx */
	USHORT vd_v_delay;											/**< VD V delay(0-16383) @@VMIVADRxx */
} T_IM_PRO_INT_VHD_DELAY_CTRL;


/*	@}*/	// im_pro_handler


/** @addtogroup im_pro_handler_sen SEN Interrupt
@{
*/
/** SENTOP interrupt control
*/
typedef struct {
	E_IM_PRO_INT_INTMD		int_mode;			/**< Interruption output mode INTCTL@@INTMD */
	T_IM_PRO_INT_CFG		vd_enable;			/**< VD edge interruption permission VHDINTENB@@VDE00-@@VDE31 */
	T_IM_PRO_INT_CFG		hd_enable;			/**< HD edge interruption permission VHDINTENB@@HDE00-@@HDE31 */
} T_IM_PRO_INT_SENTOP_CTRL;

/** SG Sync Type
*/
typedef struct {
	UCHAR		vdeg[D_IM_PRO_SEN_SG_NUM];	/**< PRO, SG out VD edge direction<br>
							value range		:[0:Rise 1:Falling]<br>
							target registor	:VHDSGCTL@@SGVDEG0 - @@SGVDEG3	*/
	UCHAR		hdeg[D_IM_PRO_SEN_SG_NUM];	/**< PRO, SG out HD edge direction<br>
							value range		:[0:Rise 1:Falling]<br>
							target registor	:VHDSGCTL@@SGHDEG0 - @@SGHDEG3	*/
} T_IM_PRO_INT_SG_SYNC;


/** SG interrupt control
*/
typedef struct {
	T_IM_PRO_INT_SG_SYNC	sg_sync;			/**< SG VD HD Sync type. VHDSGCTL@@SGVDEG/@@SGHDEG */
	T_IM_PRO_INT_CFG		sg_vd_enable;		/**< VD edge interruption permission SGVHDINTENB@@VDE_SG0-@@VDE_SG3 */
	T_IM_PRO_INT_CFG		sg_hd_enable;		/**< HD edge interruption permission SGVHDINTENB@@HDE_SG0-@@HDE_SG3 */
} T_IM_PRO_INT_SG_CTRL;

/** GYRO interrupt control
*/
typedef struct {
	T_IM_PRO_INT_CFG		gyro_sof_enable;	/**< SOF interruption permission GINTENB@@SOFE0-@@SOFE3 */
	T_IM_PRO_INT_CFG		gyro_eof_enable;	/**< EOF interruption permission GINTENB@@EOFE0-@@EOFE3 */
} T_IM_PRO_INT_GYRO_CTRL;

/** LVDS interrupt control
*/
typedef struct {
	T_IM_PRO_INT_CFG		lvds_int_enable[ E_IM_PRO_LVDS_OUTPUT_MODE_MAX ];	/**< LVDS interruption permission @@LVDSINTENB0/1/2 */
} T_IM_PRO_INT_LVDS_CTRL;

/*	@}*/	// im_pro_handler_sen

/** CPHY-MIPI interrupt control
*/
typedef struct {
	T_IM_PRO_INT_CFG		cmipi_int1_enable;	/**< CPHY MIPI interruption permission MPICINTE1 */
	T_IM_PRO_INT_CFG		cmipi_int2_enable;	/**< CPHY MIPI interruption permission MPICINTE2 */
} T_IM_PRO_INT_CMIPI_CTRL;


/** @addtogroup im_pro_handler_sro SRO Interrupt
@{
*/

/** SROTOP interrupt control
*/
typedef struct {
	E_IM_PRO_INT_INTMD	int_mode;								/**< Interruption output mode INTCTL@@INTMD */
	T_IM_PRO_INT_CFG	vd_enable[D_IM_PRO_SROTOP_VHD_CH_NUM];	/**< VD edge interruption permission VHDINTENB@@VDE00-@@VDE31 */
	T_IM_PRO_INT_CFG	hd_enable[D_IM_PRO_SROTOP_VHD_CH_NUM];	/**< HD edge interruption permission VHDINTENB@@HDE00-@@HDE31 */
} T_IM_PRO_INT_SROTOP_CTRL;

/*	@}*/	// im_pro_handler_sro

/** @addtogroup im_pro_handler_b2b B2B Interrupt
@{
*/

/** B2BTOP interrupt control
*/
typedef struct {
	E_IM_PRO_INT_INTMD	int_mode;								/**< Interruption output mode INTCTL@@INTMD */
	T_IM_PRO_INT_CFG	vd_enable[D_IM_PRO_B2BTOP_VHD_CH_NUM];	/**< VD edge interruption permission VHDINTENB@@VDE0-@@VDE1 */
	T_IM_PRO_INT_CFG	hd_enable[D_IM_PRO_B2BTOP_VHD_CH_NUM];	/**< HD edge interruption permission VHDINTENB@@HDE0-@@HDE1 */
} T_IM_PRO_INT_B2BTOP_CTRL;

/*	@}*/	// im_pro_handler_b2b

/** @addtogroup im_pro_handler_stat STAT Interrupt
@{
*/

/** STATTOP interrupt control
*/
typedef struct {
	E_IM_PRO_INT_INTMD	int_mode;								/**< Interruption output mode INTCTL@@INTMD */
} T_IM_PRO_INT_STATTOP_CTRL;

/*	@}*/	// im_pro_handler_stat

/** @addtogroup im_pro_handler_pas PAS Interrupt
@{
*/

/** PASTOP interrupt control
*/
typedef struct {
	E_IM_PRO_INT_INTMD	int_mode;								/**< Interruption output mode INTCTL@@INTMD */
	T_IM_PRO_INT_CFG	vd_enable[D_IM_PRO_PASTOP_VHD_CH_NUM];	/**< VD edge interruption permission VHDINTENB@@VDE0-@@VDE1 */
	T_IM_PRO_INT_CFG	hd_enable[D_IM_PRO_PASTOP_VHD_CH_NUM];	/**< HD edge interruption permission VHDINTENB@@HDE0-@@HDE1 */
} T_IM_PRO_INT_PASTOP_CTRL;

/*	@}*/	// im_pro_handler_pas

/** RDMA I/F LNR register address information
*/
typedef struct {
	ULONG	LNRBR0;				/**< 2800_(AC10 - AC13h) */
	ULONG	LNRBR1;				/**< 2800_(AC14 - AC17h) */
	ULONG	LNRBR2;				/**< 2800_(AC18 - AC1Bh) */
	ULONG	LNRBR3;				/**< 2800_(AC1C - AC1Fh) */
	ULONG	LNRINCR0;			/**< 2800_(AC20 - AC23h) */
	ULONG	LNRINCR1;			/**< 2800_(AC24 - AC27h) */
	ULONG	LNRINCR2;			/**< 2800_(AC28 - AC2Bh) */
	ULONG	LNRINCR3;			/**< 2800_(AC2C - AC2Fh) */
	ULONG	LNROFSR0;			/**< 2800_(AC30 - AC33h) */
	ULONG	LNROFSR1;			/**< 2800_(AC34 - AC37h) */
	ULONG	LNROFSR2;			/**< 2800_(AC38 - AC3Bh) */
	ULONG	LNROFSR3;			/**< 2800_(AC3C - AC3Fh) */
	ULONG	LNRBGR0;			/**< 2800_(AC40 - AC43h) */
	ULONG	LNRBGR1;			/**< 2800_(AC44 - AC47h) */
	ULONG	LNRBGR2;			/**< 2800_(AC48 - AC4Bh) */
	ULONG	LNRBGR3;			/**< 2800_(AC4C - AC4Fh) */
	ULONG	LNRINCGR0;			/**< 2800_(AC50 - AC53h) */
	ULONG	LNRINCGR1;			/**< 2800_(AC54 - AC57h) */
	ULONG	LNRINCGR2;			/**< 2800_(AC58 - AC5Bh) */
	ULONG	LNRINCGR3;			/**< 2800_(AC5C - AC5Fh) */
	ULONG	LNROFSGR0;			/**< 2800_(AC60 - AC63h) */
	ULONG	LNROFSGR1;			/**< 2800_(AC64 - AC67h) */
	ULONG	LNROFSGR2;			/**< 2800_(AC68 - AC6Bh) */
	ULONG	LNROFSGR3;			/**< 2800_(AC6C - AC6Fh) */
	ULONG	LNRBGB0; 			/**< 2800_(AC70 - AC73h) */
	ULONG	LNRBGB1; 			/**< 2800_(AC74 - AC77h) */
	ULONG	LNRBGB2; 			/**< 2800_(AC78 - AC7Bh) */
	ULONG	LNRBGB3; 			/**< 2800_(AC7C - AC7Fh) */
	ULONG	LNRINCGB0;			/**< 2800_(AC80 - AC83h) */
	ULONG	LNRINCGB1;			/**< 2800_(AC84 - AC87h) */
	ULONG	LNRINCGB2;			/**< 2800_(AC88 - AC8Bh) */
	ULONG	LNRINCGB3;			/**< 2800_(AC8C - AC8Fh) */
	ULONG	LNROFSGB0;			/**< 2800_(AC90 - AC93h) */
	ULONG	LNROFSGB1;			/**< 2800_(AC94 - AC97h) */
	ULONG	LNROFSGB2;			/**< 2800_(AC98 - AC9Bh) */
	ULONG	LNROFSGB3;			/**< 2800_(AC9C - AC9Fh) */
	ULONG	LNRBB0;				/**< 2800_(ACA0 - ACA3h) */
	ULONG	LNRBB1;				/**< 2800_(ACA4 - ACA7h) */
	ULONG	LNRBB2;				/**< 2800_(ACA8 - ACABh) */
	ULONG	LNRBB3;				/**< 2800_(ACAC - ACAFh) */
	ULONG	LNRINCB0;			/**< 2800_(ACB0 - ACB3h) */
	ULONG	LNRINCB1;			/**< 2800_(ACB4 - ACB7h) */
	ULONG	LNRINCB2;			/**< 2800_(ACB8 - ACBBh) */
	ULONG	LNRINCB3;			/**< 2800_(ACBC - ACBFh) */
	ULONG	LNROFSB0;			/**< 2800_(ACC0 - ACC3h) */
	ULONG	LNROFSB1;			/**< 2800_(ACC4 - ACC7h) */
	ULONG	LNROFSB2;			/**< 2800_(ACC8 - ACCBh) */
	ULONG	LNROFSB3;			/**< 2800_(ACCC - ACCFh) */
} T_IM_PRO_RDMA_LNR_ADDR;

/** RDMA I/F OBT register address information
*/
typedef struct {
	ULONG	OBTCTL;				/**< 2800_(B8C0 - B8C3h) */
	ULONG	OBTTHBIT;			/**< 2800_(B8D4 - B8D7h) */
	ULONG	OBTOFSR;			/**< 2800_(B8E8 - B8EBh) */
	ULONG	OBTOFSGR;			/**< 2800_(B8EC - B8EFh) */
	ULONG	OBTOFSGB;			/**< 2800_(B8F0 - B8F3h) */
	ULONG	OBTOFSB;			/**< 2800_(B8F4 - B8F7h) */
} T_IM_PRO_RDMA_OBT_ADDR;

/** RDMA I/F FFSH register address information
*/
typedef struct {
	ULONG	FFSHBC;				/**< 2840_(821C - 821Fh) */
	ULONG	FFSHSUBPR;			/**< 2840_(8230 - 8233h) */
	ULONG	FFSHSUBSP;			/**< 2840_(8234 - 8237h) */
	ULONG	FFSHSUBDR;			/**< 2840_(8238 - 823Bh) */
	ULONG	FFSHSUBST;			/**< 2840_(823C - 824Fh) */
} T_IM_PRO_RDMA_FFSH_ADDR;

/** RDMA I/F SDC register address information
*/
typedef struct {
	ULONG	SDCCTL;				/**< 2840_(8808 - 880Bh) */
	ULONG	SDCSGD;				/**< 2840_(8814 - 8817h) */
	ULONG	SDCPH01_0;			/**< 2840_(8880 - 8883h) */
	ULONG	SDCPH01_1;			/**< 2840_(8884 - 8887h) */
	ULONG	SDCPH02_0;			/**< 2840_(8888 - 888Bh) */
	ULONG	SDCPH02_1;			/**< 2840_(888C - 888Fh) */
	ULONG	SDCPH03_0;			/**< 2840_(8890 - 8893h) */
	ULONG	SDCPH03_1;			/**< 2840_(8894 - 8897h) */
	ULONG	SDCPH04_0;			/**< 2840_(8898 - 889Bh) */
	ULONG	SDCPH04_1;			/**< 2840_(889C - 889Fh) */
	ULONG	SDCPH05_0;			/**< 2840_(88A0 - 88A3h) */
	ULONG	SDCPH05_1;			/**< 2840_(88A4 - 88A7h) */
	ULONG	SDCPH06_0;			/**< 2840_(88A8 - 88ABh) */
	ULONG	SDCPH06_1;			/**< 2840_(88AC - 88AFh) */
	ULONG	SDCPH07_0;			/**< 2840_(88B0 - 88B3h) */
	ULONG	SDCPH07_1;			/**< 2840_(88B4 - 88B7h) */
	ULONG	SDCPH08_0;			/**< 2840_(88B8 - 88BBh) */
	ULONG	SDCPH08_1;			/**< 2840_(88BC - 88BFh) */
	ULONG	SDCPH09_0;			/**< 2840_(88C0 - 88C3h) */
	ULONG	SDCPH09_1;			/**< 2840_(88C4 - 88C7h) */
	ULONG	SDCPH11_0;			/**< 2840_(8900 - 8903h) */
	ULONG	SDCPH11_1;			/**< 2840_(8904 - 8907h) */
	ULONG	SDCPH12_0;			/**< 2840_(8908 - 890Bh) */
	ULONG	SDCPH12_1;			/**< 2840_(890C - 890Fh) */
	ULONG	SDCPH13_0;			/**< 2840_(8910 - 8913h) */
	ULONG	SDCPH13_1;			/**< 2840_(8914 - 8917h) */
	ULONG	SDCPH14_0;			/**< 2840_(8918 - 891Bh) */
	ULONG	SDCPH14_1;			/**< 2840_(891C - 891Fh) */
	ULONG	SDCPH15_0;			/**< 2840_(8920 - 8923h) */
	ULONG	SDCPH15_1;			/**< 2840_(8924 - 8927h) */
	ULONG	SDCPH16_0;			/**< 2840_(8928 - 892Bh) */
	ULONG	SDCPH16_1;			/**< 2840_(892C - 892Fh) */
	ULONG	SDCPH17_0;			/**< 2840_(8930 - 8933h) */
	ULONG	SDCPH17_1;			/**< 2840_(8934 - 8937h) */
	ULONG	SDCPH18_0;			/**< 2840_(8938 - 893Bh) */
	ULONG	SDCPH18_1;			/**< 2840_(893C - 893Fh) */
	ULONG	SDCPH19_0;			/**< 2840_(8940 - 8943h) */
	ULONG	SDCPH19_1;			/**< 2840_(8944 - 8947h) */
	ULONG	SDCPHW01;			/**< 2840_(8980 - 8983h) */
	ULONG	SDCPHW02;			/**< 2840_(8984 - 8987h) */
	ULONG	SDCPHW03;			/**< 2840_(8988 - 898Bh) */
	ULONG	SDCPHW10;			/**< 2840_(8990 - 8993h) */
	ULONG	SDCPHW11;			/**< 2840_(8994 - 8997h) */
	ULONG	SDCPHW12;			/**< 2840_(8998 - 899Bh) */
	ULONG	SDCP1HW01;			/**< 2840_(89A0 - 89A3h) */
	ULONG	SDCP1HW02;			/**< 2840_(89A4 - 89A7h) */
	ULONG	SDCP1HW03;			/**< 2840_(89A8 - 89ABh) */
	ULONG	SDCP1HW04;			/**< 2840_(89AC - 89AFh) */
	ULONG	SDCP1HW05;			/**< 2840_(89B0 - 89B3h) */
	ULONG	SDCP1HW11;			/**< 2840_(89C0 - 89C3h) */
	ULONG	SDCP1HW12;			/**< 2840_(89C4 - 89C7h) */
	ULONG	SDCP1HW13;			/**< 2840_(89C8 - 89CBh) */
	ULONG	SDCP1HW14;			/**< 2840_(89CC - 89CFh) */
	ULONG	SDCP1HW15;			/**< 2840_(89D0 - 89D3h) */
	ULONG	SDCN1HW01;			/**< 2840_(89E0 - 89E3h) */
	ULONG	SDCN1HW02;			/**< 2840_(89E4 - 89E7h) */
	ULONG	SDCN1HW03;			/**< 2840_(89E8 - 89EBh) */
	ULONG	SDCN1HW04;			/**< 2840_(89EC - 89EFh) */
	ULONG	SDCN1HW05;			/**< 2840_(89F0 - 89F3h) */
	ULONG	SDCN1HW11;			/**< 2840_(8A00 - 8A03h) */
	ULONG	SDCN1HW12;			/**< 2840_(8A04 - 8A07h) */
	ULONG	SDCN1HW13;			/**< 2840_(8A08 - 8A0Bh) */
	ULONG	SDCN1HW14;			/**< 2840_(8A0C - 8A0Fh) */
	ULONG	SDCN1HW15;			/**< 2840_(8A10 - 8A13h) */
	ULONG	SDCP2HW01;			/**< 2840_(8A20 - 8A23h) */
	ULONG	SDCP2HW02;			/**< 2840_(8A24 - 8A27h) */
	ULONG	SDCP2HW03;			/**< 2840_(8A28 - 8A2Bh) */
	ULONG	SDCP2HW04;			/**< 2840_(8A2C - 8A2Fh) */
	ULONG	SDCP2HW05;			/**< 2840_(8A30 - 8A33h) */
	ULONG	SDCP2HW11;			/**< 2840_(8A40 - 8A43h) */
	ULONG	SDCP2HW12;			/**< 2840_(8A44 - 8A47h) */
	ULONG	SDCP2HW13;			/**< 2840_(8A48 - 8A4Bh) */
	ULONG	SDCP2HW14;			/**< 2840_(8A4C - 8A4Fh) */
	ULONG	SDCP2HW15;			/**< 2840_(8A50 - 8A53h) */
	ULONG	SDCN2HW01;			/**< 2840_(8A60 - 8A63h) */
	ULONG	SDCN2HW02;			/**< 2840_(8A64 - 8A67h) */
	ULONG	SDCN2HW03;			/**< 2840_(8A68 - 8A6Bh) */
	ULONG	SDCN2HW04;			/**< 2840_(8A6C - 8A6Fh) */
	ULONG	SDCN2HW05;			/**< 2840_(8A70 - 8A73h) */
	ULONG	SDCN2HW11;			/**< 2840_(8A80 - 8A83h) */
	ULONG	SDCN2HW12;			/**< 2840_(8A84 - 8A87h) */
	ULONG	SDCN2HW13;			/**< 2840_(8A88 - 8A8Bh) */
	ULONG	SDCN2HW14;			/**< 2840_(8A8C - 8A8Fh) */
	ULONG	SDCN2HW15;			/**< 2840_(8A90 - 8A93h) */
	ULONG	SDCPC01;			/**< 2840_(8AC0 - 8AC3h) */
	ULONG	SDCPC02;			/**< 2840_(8AC4 - 8AC7h) */
	ULONG	SDCPC03;			/**< 2840_(8AC8 - 8ACBh) */
	ULONG	SDCPC04;			/**< 2840_(8ACC - 8ACFh) */
	ULONG	SDCPC05;			/**< 2840_(8AD0 - 8AD3h) */
	ULONG	SDCPC06;			/**< 2840_(8AD4 - 8AD7h) */
	ULONG	SDCPC07;			/**< 2840_(8AD8 - 8ADBh) */
	ULONG	SDCPC08;			/**< 2840_(8ADC - 8ADFh) */
	ULONG	SDCPC09;			/**< 2840_(8AE0 - 8AE3h) */
	ULONG	SDCPC010;			/**< 2840_(8AE4 - 8AE7h) */
	ULONG	SDCPC011;			/**< 2840_(8AE8 - 8AEBh) */
	ULONG	SDCPC012;			/**< 2840_(8AEC - 8AEFh) */
	ULONG	SDCPC013;			/**< 2840_(8AF0 - 8AF3h) */
	ULONG	SDCPC014;			/**< 2840_(8AF4 - 8AF7h) */
	ULONG	SDCPC015;			/**< 2840_(8AF8 - 8AFBh) */
	ULONG	SDCPC016;			/**< 2840_(8AFC - 8AFFh) */
	ULONG	SDCPC11;			/**< 2840_(8B00 - 8B03h) */
	ULONG	SDCPC12;			/**< 2840_(8B04 - 8B07h) */
	ULONG	SDCPC13;			/**< 2840_(8B08 - 8B0Bh) */
	ULONG	SDCPC14;			/**< 2840_(8B0C - 8B0Fh) */
	ULONG	SDCPC15;			/**< 2840_(8B10 - 8B13h) */
	ULONG	SDCPC16;			/**< 2840_(8B14 - 8B17h) */
	ULONG	SDCPC17;			/**< 2840_(8B18 - 8B1Bh) */
	ULONG	SDCPC18;			/**< 2840_(8B1C - 8B1Fh) */
	ULONG	SDCPC19;			/**< 2840_(8B20 - 8B23h) */
	ULONG	SDCPC110;			/**< 2840_(8B24 - 8B27h) */
	ULONG	SDCPC111;			/**< 2840_(8B28 - 8B2Bh) */
	ULONG	SDCPC112;			/**< 2840_(8B2C - 8B2Fh) */
	ULONG	SDCPC113;			/**< 2840_(8B30 - 8B33h) */
	ULONG	SDCPC114;			/**< 2840_(8B34 - 8B37h) */
	ULONG	SDCPC115;			/**< 2840_(8B38 - 8B3Bh) */
	ULONG	SDCPC116;			/**< 2840_(8B3C - 8B3Fh) */
	ULONG	SDCPCW0;			/**< 2840_(8B40 - 8B43h) */
	ULONG	SDCPCW1;			/**< 2840_(8B44 - 8B47h) */
	ULONG	SDCP1CW0;			/**< 2840_(8B48 - 8B4Bh) */
	ULONG	SDCP1CW1;			/**< 2840_(8B4C - 8B4Fh) */
	ULONG	SDCN1CW0;			/**< 2840_(8B50 - 8B53h) */
	ULONG	SDCN1CW1;			/**< 2840_(8B54 - 8B57h) */
	ULONG	SDCP2CW0;			/**< 2840_(8B58 - 8B5Bh) */
	ULONG	SDCP2CW1;			/**< 2840_(8B5C - 8B5Fh) */
	ULONG	SDCN2CW0;			/**< 2840_(8B60 - 8B63h) */
	ULONG	SDCN2CW1;			/**< 2840_(8B64 - 8B67h) */
	ULONG	SDCGMAXMIN; 		/**< 2840_(8B9C - 8B9Fh) */
	ULONG	SDCNMAX;			/**< 2840_(8BA0 - 8BA3h) */
} T_IM_PRO_RDMA_SDC_ADDR;

/** RDMA I/F DPC register address information
*/
typedef struct {
	ULONG	DPCMD;				/**< 2840_(9118 - 911Bh) */
	ULONG	DPCBLP; 			/**< 2840_(911C - 911Fh) */
	ULONG	DPCBLM; 			/**< 2840_(9120 - 9123h) */
	ULONG	DFCTL;				/**< 2840_(9124 - 9127h) */
	ULONG	DFS1;				/**< 2840_(9130 - 9133h) */
	ULONG	DFS2;				/**< 2840_(9134 - 9137h) */
	ULONG	DFS3;				/**< 2840_(9138 - 913Bh) */
	ULONG	DFO1;				/**< 2840_(9140 - 9143h) */
	ULONG	DFO2;				/**< 2840_(9144 - 9147h) */
	ULONG	DFO3;				/**< 2840_(9148 - 914Bh) */
	ULONG	DFK1;				/**< 2840_(9150 - 9153h) */
	ULONG	DFK2;				/**< 2840_(9154 - 9157h) */
	ULONG	DFK3;				/**< 2840_(9158 - 915Bh) */
	ULONG	DFH1;				/**< 2840_(9160 - 9163h) */
	ULONG	DFH2;				/**< 2840_(9164 - 9167h) */
	ULONG	DFH3;				/**< 2840_(9168 - 916Bh) */
	ULONG	DFCSR;				/**< 2840_(9170 - 9173h) */
	ULONG	DFLSTH1; 			/**< 2840_(9178 - 917Bh) */
	ULONG	DFLSTH2; 			/**< 2840_(917C - 917Fh) */
	ULONG	DFAGEF; 			/**< 2840_(9180 - 9183h) */
	ULONG	DFAGEF2;			/**< 2840_(9184 - 9187h) */
	ULONG	DFAGTH; 			/**< 2840_(9188 - 918Bh) */
	ULONG	DFAGTHK;			/**< 2840_(918C - 918Fh) */
	ULONG	DMCTL;				/**< 2840_(9190 - 9193h) */
	ULONG	DMWKS;				/**< 2840_(9194 - 9197h) */
	ULONG	DMWKD;				/**< 2840_(9198 - 919Bh) */
	ULONG	DMAGK;				/**< 2840_(919C - 919Fh) */
	ULONG	DECTL;				/**< 2840_(91A0 - 91A3h) */
	ULONG	DENSS;				/**< 2840_(91A4 - 91A7h) */
	ULONG	DENSO;				/**< 2840_(91A8 - 91ABh) */
	ULONG	DENSK;				/**< 2840_(91AC - 91AFh) */
	ULONG	DENSH;				/**< 2840_(91B0 - 91B3h) */
	ULONG	DENSMGN;			/**< 2840_(91B4 - 91B7h) */
	ULONG	DENDCTH;			/**< 2840_(91B8 - 91BBh) */
	ULONG	DEAGK;				/**< 2840_(91BC - 91BFh) */
	ULONG	DCAGEF; 			/**< 2840_(91C0 - 91C3h) */
	ULONG	DCAGTH; 			/**< 2840_(91C4 - 91C7h) */
} T_IM_PRO_RDMA_DPC_ADDR;

/** RDMA I/F FSHDL register address information
*/
typedef struct {
	ULONG	FFSHBC;     		/**< 2840_(931C - 931Fh) */
	ULONG	FFSHUL1;    		/**< 2840_(9320 - 9323h) */
	ULONG	FFSHUL2;    		/**< 2840_(9324 - 9327h) */
	ULONG	FFSHGFMT;   		/**< 2840_(9328 - 932Bh) */
	ULONG	FFSHSUBPR;  		/**< 2840_(9330 - 9333h) */
	ULONG	FFSHSUBSP;  		/**< 2840_(9334 - 9337h) */
	ULONG	FFSHSUBDR;  		/**< 2840_(9338 - 933Bh) */
	ULONG	FFSHSUBST;  		/**< 2840_(933C - 933Fh) */
	ULONG	FFSHCTL;    		/**< 2840_(9340 - 9343h) */
	ULONG	FFSHHWPH;   		/**< 2840_(9344 - 9347h) */
	ULONG	FFSHCCTL;   		/**< 2840_(9350 - 9353h) */
	ULONG	FFSHLP1;    		/**< 2840_(9354 - 9357h) */
	ULONG	FFSHLP2;    		/**< 2840_(9358 - 935Bh) */
} T_IM_PRO_RDMA_FSHDL_ADDR;

/** RDMA I/F FFC register address information
*/
typedef struct {
	ULONG	FCBC;				/**< 2840_(971C - 971Fh) */
	ULONG	FCSUBPR;			/**< 2840_(9734 - 9737h) */
	ULONG	FCSUBSP;			/**< 2840_(9738 - 973Bh) */
	ULONG	FCSUBDR;			/**< 2840_(973C - 973Fh) */
	ULONG	FCSUBST;			/**< 2840_(9740 - 9743h) */
} T_IM_PRO_RDMA_FFC_ADDR;

/** RDMA I/F PGAIN register address information
*/
typedef struct {
	ULONG	PGAINR;				/**< 2840_(8604 - 8607h) */
	ULONG	PGAINGR;			/**< 2840_(8608 - 860Bh) */
	ULONG	PGAINGB;			/**< 2840_(860C - 860Fh) */
	ULONG	PGAINB;				/**< 2840_(8610 - 8613h) */
	ULONG	POFSR;				/**< 2840_(8614 - 8617h) */
	ULONG	POFSGR;				/**< 2840_(8618 - 861Bh) */
	ULONG	POFSGB;				/**< 2840_(861C - 861Fh) */
	ULONG	POFSB;				/**< 2840_(8620 - 8623h) */
	ULONG	PGAINDP;			/**< 2840_(8624 - 8627h) */
} T_IM_PRO_RDMA_PGAIN_ADDR;

/** RDMA I/F PZSFT register address information
*/
typedef struct {
	ULONG	PZOFST;				/**< 2840_(9004 - 9007h) */
} T_IM_PRO_RDMA_PZSFT_ADDR;

/** RDMA I/F CAG register address information
*/
typedef struct {
	ULONG	CAGMD;				/**< 2840_(9818 - 981Bh) */
	ULONG	RBR;				/**< 2840_(981C - 981Fh) */
	ULONG	ABOARV; 			/**< 2840_(9820 - 9823h) */
	ULONG	ABOARH; 			/**< 2840_(9824 - 9827h) */
	ULONG	ABOAGV; 			/**< 2840_(9828 - 982Bh) */
	ULONG	ABOAGH; 			/**< 2840_(982C - 982Fh) */
	ULONG	ABOABV; 			/**< 2840_(9830 - 9833h) */
	ULONG	ABOABH; 			/**< 2840_(9834 - 9837h) */
	ULONG	ABGAR;				/**< 2840_(9838 - 983Bh) */
	ULONG	ABGAG;				/**< 2840_(983C - 983Fh) */
	ULONG	ABGAB;				/**< 2840_(9840 - 9843h) */
	ULONG	ABLRV;				/**< 2840_(9844 - 9847h) */
	ULONG	ABLRH;				/**< 2840_(9848 - 984Bh) */
	ULONG	ABLGV;				/**< 2840_(984C - 984Fh) */
	ULONG	ABLGH;				/**< 2840_(9850 - 9853h) */
	ULONG	ABLBV;				/**< 2840_(9854 - 9857h) */
	ULONG	ABLBH;				/**< 2840_(9858 - 985Bh) */
	ULONG	ABOFSRV;			/**< 2840_(985C - 985Fh) */
	ULONG	ABOFSRH;			/**< 2840_(9860 - 9863h) */
	ULONG	ABOFSGV;			/**< 2840_(9864 - 9867h) */
	ULONG	ABOFSGH;			/**< 2840_(9868 - 986Bh) */
	ULONG	ABOFSBV;			/**< 2840_(986C - 986Fh) */
	ULONG	ABOFSBH;			/**< 2840_(9870 - 9873h) */
	ULONG	ABNLTHR1;			/**< 2840_(9874 - 9877h) */
	ULONG	ABNLTHG1;			/**< 2840_(9878 - 987Bh) */
	ULONG	ABNLTHB1;			/**< 2840_(987C - 987Fh) */
	ULONG	ABNLGAR1;			/**< 2840_(9880 - 9883h) */
	ULONG	ABNLGAG1;			/**< 2840_(9884 - 9887h) */
	ULONG	ABNLGAB1;			/**< 2840_(9888 - 988Bh) */
	ULONG	ABNLTHR2;			/**< 2840_(988C - 988Fh) */
	ULONG	ABNLTHG2;			/**< 2840_(9890 - 9893h) */
	ULONG	ABNLTHB2;			/**< 2840_(9894 - 9897h) */
	ULONG	ABNLGAR2;			/**< 2840_(9898 - 989Bh) */
	ULONG	ABNLGAG2;			/**< 2840_(989C - 989Fh) */
	ULONG	ABNLGAB2;			/**< 2840_(98A0 - 98A3h) */
	ULONG	ABNLTHR3;			/**< 2840_(98A4 - 98A7h) */
	ULONG	ABNLTHG3;			/**< 2840_(98A8 - 98ABh) */
	ULONG	ABNLTHB3;			/**< 2840_(98AC - 98AFh) */
	ULONG	ABNLGAR3;			/**< 2840_(98B0 - 98B3h) */
	ULONG	ABNLGAG3;			/**< 2840_(98B4 - 98B7h) */
	ULONG	ABNLGAB3;			/**< 2840_(98B8 - 98BBh) */
	ULONG	ABNLTHR4;			/**< 2840_(98BC - 98BFh) */
	ULONG	ABNLTHG4;			/**< 2840_(98C0 - 98C3h) */
	ULONG	ABNLTHB4;			/**< 2840_(98C4 - 98C7h) */
	ULONG	ABNLGAR4;			/**< 2840_(98C8 - 98CBh) */
	ULONG	ABNLGAG4;			/**< 2840_(98CC - 98CFh) */
	ULONG	ABNLGAB4;			/**< 2840_(98D0 - 98D3h) */
	ULONG	BFGLMT; 			/**< 2840_(98D4 - 98D7h) */
	ULONG	BFGTH;				/**< 2840_(98D8 - 98DBh) */
} T_IM_PRO_RDMA_CAG_ADDR;

/** RDMA I/F ELF register address information
*/
typedef struct {
	ULONG	ELFCTL; 			/**< 2841_(0420 - 0423h) */
	ULONG	ELFZP;				/**< 2841_(0424 - 0427h) */
	ULONG	ELFL1;				/**< 2841_(0428 - 042Bh) */
	ULONG	ELFL2;				/**< 2841_(042C - 042Fh) */
	ULONG	ANBOR1;				/**< 2841_(0430 - 0433h) */
	ULONG	ANBOR2;				/**< 2841_(0434 - 0437h) */
	ULONG	ANBKR1;				/**< 2841_(0438 - 043Bh) */
	ULONG	ANBKR2;				/**< 2841_(043C - 043Fh) */
	ULONG	ANBDR1;				/**< 2841_(0440 - 0443h) */
	ULONG	ANBDR2;				/**< 2841_(0444 - 0447h) */
	ULONG	ANBOG1;				/**< 2841_(0448 - 044Bh) */
	ULONG	ANBOG2;				/**< 2841_(044C - 044Fh) */
	ULONG	ANBKG1;				/**< 2841_(0450 - 0453h) */
	ULONG	ANBKG2;				/**< 2841_(0454 - 0457h) */
	ULONG	ANBDG1;				/**< 2841_(0458 - 045Bh) */
	ULONG	ANBDG2;				/**< 2841_(045C - 045Fh) */
	ULONG	ANBOB1;				/**< 2841_(0460 - 0463h) */
	ULONG	ANBOB2;				/**< 2841_(0464 - 0467h) */
	ULONG	ANBKB1;				/**< 2841_(0468 - 046Bh) */
	ULONG	ANBKB2;				/**< 2841_(046C - 046Fh) */
	ULONG	ANBDB1;				/**< 2841_(0470 - 0473h) */
	ULONG	ANBDB2;				/**< 2841_(0474 - 0477h) */
	ULONG	LGTW1;				/**< 2841_(0478 - 047Bh) */
	ULONG	LGTW2;				/**< 2841_(047C - 047Fh) */
	ULONG	LGT;				/**< 2841_(0480 - 0483h) */
	ULONG	LGTE;				/**< 2841_(0484 - 0487h) */
	ULONG	EDG2ANBA;			/**< 2841_(0488 - 048Bh) */
	ULONG	EDGSCLA;			/**< 2841_(048C - 048Fh) */
	ULONG	EDG2ANBB;			/**< 2841_(0490 - 0493h) */
	ULONG	EDGSCLB;			/**< 2841_(0494 - 0497h) */
	ULONG	EDG2ANBC;			/**< 2841_(0498 - 049Bh) */
	ULONG	EDGSCLC;			/**< 2841_(049C - 049Fh) */
	ULONG	EFTH1;				/**< 2841_(04A0 - 04A3h) */
	ULONG	EFTH2;				/**< 2841_(04A4 - 04A7h) */
	ULONG	LFTH;				/**< 2841_(04A8 - 04ABh) */
	ULONG	ELCOME; 			/**< 2841_(04AC - 04AFh) */
	ULONG	ELCOMEN;			/**< 2841_(04B0 - 04B3h) */
	ULONG	A1S5E1;				/**< 2841_(04B8 - 04BBh) */
	ULONG	A1S5E2;				/**< 2841_(04BC - 04BFh) */
	ULONG	A1S5L11; 			/**< 2841_(04C0 - 04C3h) */
	ULONG	A1S5L12; 			/**< 2841_(04C4 - 04C7h) */
	ULONG	A1S5L21; 			/**< 2841_(04C8 - 04CBh) */
	ULONG	A1S5L22; 			/**< 2841_(04CC - 04CFh) */
	ULONG	A1S5L31; 			/**< 2841_(04D0 - 04D3h) */
	ULONG	A1S5L32; 			/**< 2841_(04D4 - 04D7h) */
	ULONG	A1S5L41; 			/**< 2841_(04D8 - 04DBh) */
	ULONG	A1S5L42; 			/**< 2841_(04DC - 04DFh) */
	ULONG	NR1S5RGB1;			/**< 2841_(04E0 - 04E3h) */
	ULONG	NR1S5RGB2;			/**< 2841_(04E4 - 04E7h) */
	ULONG	NS1S5LM;			/**< 2841_(04E8 - 04EBh) */
	ULONG	A5S9E1;				/**< 2841_(04F0 - 04F3h) */
	ULONG	A5S9E2;				/**< 2841_(04F4 - 04F7h) */
	ULONG	A5S9L11;			/**< 2841_(04F8 - 04FBh) */
	ULONG	A5S9L12;			/**< 2841_(04FC - 04FFh) */
	ULONG	A5S9L21;			/**< 2841_(0500 - 0503h) */
	ULONG	A5S9L22;			/**< 2841_(0504 - 0507h) */
	ULONG	A5S9L31;			/**< 2841_(0508 - 050Bh) */
	ULONG	A5S9L32;			/**< 2841_(050C - 050Fh) */
	ULONG	A5S9L41;			/**< 2841_(0510 - 0513h) */
	ULONG	A5S9L42;			/**< 2841_(0514 - 0517h) */
	ULONG	NR5S9RGB1;			/**< 2841_(0518 - 051Bh) */
	ULONG	NR5S9RGB2;			/**< 2841_(051C - 051Fh) */
	ULONG	NS5S9LM;			/**< 2841_(0520 - 0523h) */
	ULONG	FS9W1;				/**< 2841_(0528 - 052Bh) */
	ULONG	FS9W2;				/**< 2841_(052C - 052Fh) */
	ULONG	FS9YC1;				/**< 2841_(0540 - 0543h) */
	ULONG	FS9YC2;				/**< 2841_(0544 - 0547h) */
	ULONG	FS9YC3;				/**< 2841_(0548 - 054Bh) */
	ULONG	FS9YC4;				/**< 2841_(054C - 054Fh) */
	ULONG	FS9YC5;				/**< 2841_(0550 - 0553h) */
	ULONG	FASTR;				/**< 2841_(0560 - 0563h) */
	ULONG	CRVAF;				/**< 2841_(0564 - 0567h) */
	ULONG	FCRVABTOF;			/**< 2841_(0568 - 056Bh) */
	ULONG	FCRVABTGA;			/**< 2841_(056C - 056Fh) */
	ULONG	FCRVABTBD;			/**< 2841_(0570 - 0573h) */
	ULONG	FCRVABTCP;			/**< 2841_(0574 - 0577h) */
	ULONG	FYABTGA;			/**< 2841_(0578 - 057Bh) */
	ULONG	FYABTBD;			/**< 2841_(057C - 057Fh) */
	ULONG	FYABTCLP;			/**< 2841_(0580 - 0583h) */
	ULONG	FAEHH;				/**< 2841_(0584 - 0587h) */
	ULONG	FBSTR;				/**< 2841_(0588 - 058Bh) */
	ULONG	CRVBF;				/**< 2841_(058C - 058Fh) */
	ULONG	FCRVBBTOF;			/**< 2841_(0590 - 0593h) */
	ULONG	FCRVBBTGA;			/**< 2841_(0594 - 0597h) */
	ULONG	FCRVBBTBD;			/**< 2841_(0598 - 059Bh) */
	ULONG	FCRVBBTCP;			/**< 2841_(059C - 059Fh) */
	ULONG	FYBBTGA;			/**< 2841_(05A0 - 05A3h) */
	ULONG	FYBBTBD;			/**< 2841_(05A4 - 05A7h) */
	ULONG	FYBBTCLP;			/**< 2841_(05A8 - 05ABh) */
	ULONG	FBEHH;				/**< 2841_(05AC - 05AFh) */
	ULONG	ELFMNNRRL;			/**< 2841_(05B0 - 05B3h) */
	ULONG	ELFMNNRGL;			/**< 2841_(05B4 - 05B7h) */
	ULONG	ELFMNNRBL;			/**< 2841_(05B8 - 05BBh) */
	ULONG	ELFAFNR;			/**< 2841_(05BC - 05BFh) */
	ULONG	ZELADCMD;			/**< 2841_(05C0 - 05C3h) */
	ULONG	ZANBOR1;			/**< 2841_(05C8 - 05CBh) */
	ULONG	ZANBOR2;			/**< 2841_(05CC - 05CFh) */
	ULONG	ZANBKR1;			/**< 2841_(05D0 - 05D3h) */
	ULONG	ZANBKR2;			/**< 2841_(05D4 - 05D7h) */
	ULONG	ZANBDR1;			/**< 2841_(05D8 - 05DBh) */
	ULONG	ZANBDR2;			/**< 2841_(05DC - 05DFh) */
	ULONG	ZANBOG1;			/**< 2841_(05E0 - 05E3h) */
	ULONG	ZANBOG2;			/**< 2841_(05E4 - 05E7h) */
	ULONG	ZANBKG1;			/**< 2841_(05E8 - 05EBh) */
	ULONG	ZANBKG2;			/**< 2841_(05EC - 05EFh) */
	ULONG	ZANBDG1;			/**< 2841_(05F0 - 05F3h) */
	ULONG	ZANBDG2;			/**< 2841_(05F4 - 05F7h) */
	ULONG	ZANBOB1;			/**< 2841_(05F8 - 05FBh) */
	ULONG	ZANBOB2;			/**< 2841_(05FC - 05FFh) */
	ULONG	ZANBKB1;			/**< 2841_(0600 - 0603h) */
	ULONG	ZANBKB2;			/**< 2841_(0604 - 0607h) */
	ULONG	ZANBDB1;			/**< 2841_(0608 - 060Bh) */
	ULONG	ZANBDB2;			/**< 2841_(060C - 060Fh) */
	ULONG	ZLGTW1;				/**< 2841_(0610 - 0613h) */
	ULONG	ZLGTW2;				/**< 2841_(0614 - 0617h) */
	ULONG	ZLGT;				/**< 2841_(0618 - 061Bh) */
	ULONG	ZLGTE;				/**< 2841_(061C - 061Fh) */
	ULONG	ZEDG2ANBA;			/**< 2841_(0620 - 0623h) */
	ULONG	ZEDGSCLA;			/**< 2841_(0624 - 0627h) */
	ULONG	ZEDG2ANBB;			/**< 2841_(0628 - 062Bh) */
	ULONG	ZEDGSCLB;			/**< 2841_(062C - 062Fh) */
	ULONG	ZALNGE1; 			/**< 2841_(0630 - 0633h) */
	ULONG	ZALNGE2; 			/**< 2841_(0634 - 0637h) */
	ULONG	ZALNG1;				/**< 2841_(0640 - 0643h) */
	ULONG	ZALNG2;				/**< 2841_(0644 - 0647h) */
	ULONG	ZALNG3;				/**< 2841_(0648 - 064Bh) */
	ULONG	ZALNG4;				/**< 2841_(064C - 064Fh) */
	ULONG	ZALNG5;				/**< 2841_(0650 - 0653h) */
	ULONG	ZALNG6;				/**< 2841_(0654 - 0657h) */
	ULONG	ZLFTH;				/**< 2841_(0660 - 0663h) */
	ULONG	ZLCOME; 			/**< 2841_(0664 - 0667h) */
	ULONG	ZLCOMEN;			/**< 2841_(0668 - 066Bh) */
	ULONG	ZNSLNGLM;			/**< 2841_(066C - 066Fh) */
	ULONG	EDGCATH;			/**< 2841_(0684 - 0687h) */
} T_IM_PRO_RDMA_ELF_ADDR;

/** RDMA I/F FSHD register address information
*/
typedef struct {
	ULONG	FSHDCTL;			/**< 2841_(0A08 - 0A0Bh) */
	ULONG	FSHDHWPH;			/**< 2841_(0A1C - 0A1Fh) */
	ULONG	FSHDBC; 			/**< 2841_(0A20 - 0A23h) */
	ULONG	FSUL1;				/**< 2841_(0A24 - 0A27h) */
	ULONG	FSUL2;				/**< 2841_(0A28 - 0A2Bh) */
	ULONG	FSCTL;				/**< 2841_(0A30 - 0A33h) */
	ULONG	FSSUBPR;			/**< 2841_(0A34 - 0A37h) */
	ULONG	FSSUBSP;			/**< 2841_(0A38 - 0A3Bh) */
	ULONG	FSSUBDR;			/**< 2841_(0A3C - 0A3Fh) */
	ULONG	FSSUBST;			/**< 2841_(0A40 - 0A43h) */
	ULONG	FSA1;				/**< 2841_(0A44 - 0A47h) */
	ULONG	FSA2;				/**< 2841_(0A48 - 0A4Bh) */
	ULONG	CSCTL1; 			/**< 2841_(0A50 - 0A53h) */
	ULONG	CSCTL2; 			/**< 2841_(0A54 - 0A57h) */
	ULONG	CSTBLST;			/**< 2841_(0A58 - 0A5Bh) */
	ULONG	CSTBLS1;			/**< 2841_(0A5C - 0A5Fh) */
	ULONG	CSTBLS2;			/**< 2841_(0A60 - 0A63h) */
	ULONG	CSHDD;				/**< 2841_(0A64 - 0A67h) */
	ULONG	CSHDOA; 			/**< 2841_(0A68 - 0A6Bh) */
	ULONG	CSA1;				/**< 2841_(0A6C - 0A6Fh) */
	ULONG	CSA2;				/**< 2841_(0A70 - 0A73h) */
	ULONG	FSHDHCCTL;			/**< 2841_(0A80 - 0A83h) */
	ULONG	FSSLP1; 			/**< 2841_(0A84 - 0A87h) */
	ULONG	FSSLP2; 			/**< 2841_(0A88 - 0A8Bh) */
} T_IM_PRO_RDMA_FSHD_ADDR;

/** RDMA I/F P2M knee table register address information
*/
typedef struct {
	ULONG	P2MKNTBL;			/**< knee table register address */
} T_IM_PRO_RDMA_P2M_KNEE_TBL_ADDR;

/** RDMA I/F M2P deknee table register address information
*/
typedef struct {
	ULONG	M2PDKNTBL;			/**< deknee table register address */
} T_IM_PRO_RDMA_M2P_DEKNEE_TBL_ADDR;

/** RDMA I/F SDC gain coefficient table register address information
*/
typedef struct {
	ULONG	SDCGD;				/**< gain coefficient table register address */
} T_IM_PRO_RDMA_SDC_GAIN_TBL_ADDR;

/** RDMA I/F Frame Shading Concentric Circles Correction table register address information
*/
typedef struct {
	ULONG	FSHDCTBL;			/**< Frame Shading Concentric Circles Correction table register address */
} T_IM_PRO_RDMA_FSHD_CTBL_ADDR;

/** RDMA I/F SPT Shading Correction table register address information
*/
typedef struct {
	ULONG	SPTTBL;				/**< SPT Shading Correction table register address */
} T_IM_PRO_RDMA_SPT_SHD_TBL_ADDR;

/** RDMA I/F LNR register write value information
*/
typedef struct {
	struct io_lnrbr		LNRBR;			/**< 2800_(AC10 - AC1Fh) */
	struct io_lnrincr	LNRINCR;		/**< 2800_(AC20 - AC2Fh) */
	struct io_lnrofsr	LNROFSR;		/**< 2800_(AC30 - AC3Fh) */
	struct io_lnrbgr	LNRBGR;			/**< 2800_(AC40 - AC4Fh) */
	struct io_lnrincgr	LNRINCGR;		/**< 2800_(AC50 - AC5Fh) */
	struct io_lnrofsgr	LNROFSGR;		/**< 2800_(AC60 - AC6Fh) */
	struct io_lnrbgb	LNRBGB;			/**< 2800_(AC70 - AC7Fh) */
	struct io_lnrincgb	LNRINCGB;		/**< 2800_(AC80 - AC8Fh) */
	struct io_lnrofsgb	LNROFSGB;		/**< 2800_(AC90 - AC9Fh) */
	struct io_lnrbb		LNRBB;			/**< 2800_(ACA0 - ACAFh) */
	struct io_lnrincb	LNRINCB;		/**< 2800_(ACB0 - ACBFh) */
	struct io_lnrofsb	LNROFSB;		/**< 2800_(ACC0 - ACCFh) */
} T_IM_PRO_RDMA_LNR_VAL;

/** RDMA I/F OBT register write value information
*/
typedef struct {
	union io_obtctl     OBTCTL;			/**< 2800_(B8C0 - B8C3h) */
	union io_obtthbit   OBTTHBIT;		/**< 2800_(B8D4 - B8D7h) */
	union io_obtofsr    OBTOFSR;		/**< 2800_(B8E8 - B8EBh) */
	union io_obtofsgr   OBTOFSGR;		/**< 2800_(B8EC - B8EFh) */
	union io_obtofsgb   OBTOFSGB;		/**< 2800_(B8F0 - B8F3h) */
	union io_obtofsb    OBTOFSB;		/**< 2800_(B8F4 - B8F7h) */
} T_IM_PRO_RDMA_OBT_VAL;

/** RDMA I/F FFSH register write value information
*/
typedef struct {
	union io_ffshbc		FFSHBC;			/**< 2840_(821C - 821Fh) */
	union io_ffshsubpr	FFSHSUBPR;		/**< 2840_(8230 - 8233h) */
	union io_ffshsubsp	FFSHSUBSP;		/**< 2840_(8234 - 8237h) */
	union io_ffshsubdr	FFSHSUBDR;		/**< 2840_(8238 - 823Bh) */
	union io_ffshsubst	FFSHSUBST;		/**< 2840_(823C - 824Fh) */
} T_IM_PRO_RDMA_FFSH_VAL;

/** RDMA I/F SDC register write value information
*/
typedef struct {
	union  io_sdcctl	 SDCCTL;	 	/**< 2840_(8808 - 880Bh) */
	union  io_sdcsgd	 SDCSGD;	 	/**< 2840_(8814 - 8817h) */
	struct io_sdcph0	 SDCPH0;	 	/**< 2840_(8880 - 88C7h) */
	struct io_sdcph1	 SDCPH1;	 	/**< 2840_(8900 - 8947h) */
	struct io_sdcphw0	 SDCPHW0;	 	/**< 2840_(8980 - 898Bh) */
	struct io_sdcphw1	 SDCPHW1;	 	/**< 2840_(8990 - 899Bh) */
	struct io_sdcp1hw0	 SDCP1HW0;	 	/**< 2840_(89A0 - 89B3h) */
	struct io_sdcp1hw1	 SDCP1HW1;	 	/**< 2840_(89C0 - 89D3h) */
	struct io_sdcn1hw0	 SDCN1HW0;	 	/**< 2840_(89E0 - 89F3h) */
	struct io_sdcn1hw1	 SDCN1HW1;	 	/**< 2840_(8A00 - 8A13h) */
	struct io_sdcp2hw0	 SDCP2HW0;	 	/**< 2840_(8A20 - 8A33h) */
	struct io_sdcp2hw1	 SDCP2HW1;	 	/**< 2840_(8A40 - 8A53h) */
	struct io_sdcn2hw0	 SDCN2HW0;	 	/**< 2840_(8A60 - 8A73h) */
	struct io_sdcn2hw1	 SDCN2HW1;	 	/**< 2840_(8A80 - 8A93h) */
	struct io_sdcpc0	 SDCPC0;	 	/**< 2840_(8AC0 - 8AFFh) */
	struct io_sdcpc1	 SDCPC1;	 	/**< 2840_(8B00 - 8B3Fh) */
	union  io_sdcpcw0	 SDCPCW0;	 	/**< 2840_(8B40 - 8B43h) */
	union  io_sdcpcw1	 SDCPCW1;	 	/**< 2840_(8B44 - 8B47h) */
	union  io_sdcp1cw0	 SDCP1CW0;	 	/**< 2840_(8B48 - 8B4Bh) */
	union  io_sdcp1cw1	 SDCP1CW1;	 	/**< 2840_(8B4C - 8B4Fh) */
	union  io_sdcn1cw0	 SDCN1CW0;	 	/**< 2840_(8B50 - 8B53h) */
	union  io_sdcn1cw1	 SDCN1CW1;	 	/**< 2840_(8B54 - 8B57h) */
	union  io_sdcp2cw0	 SDCP2CW0;	 	/**< 2840_(8B58 - 8B5Bh) */
	union  io_sdcp2cw1	 SDCP2CW1;	 	/**< 2840_(8B5C - 8B5Fh) */
	union  io_sdcn2cw0	 SDCN2CW0;	 	/**< 2840_(8B60 - 8B63h) */
	union  io_sdcn2cw1	 SDCN2CW1;	 	/**< 2840_(8B64 - 8B67h) */
	union  io_sdcgmaxmin SDCGMAXMIN; 	/**< 2840_(8B9C - 8B9Fh) */
	union  io_sdcnmax	 SDCNMAX;	 	/**< 2840_(8BA0 - 8BA3h) */
} T_IM_PRO_RDMA_SDC_VAL;

/** RDMA I/F DPC register write value information
*/
typedef struct {
	union  io_dpcmd 	DPCMD;			/**< 2840_(9118 - 911Bh) */
	union  io_dpcblp	DPCBLP; 		/**< 2840_(911C - 911Fh) */
	union  io_dpcblm	DPCBLM; 		/**< 2840_(9120 - 9123h) */
	union  io_dfctl 	DFCTL;			/**< 2840_(9124 - 9127h) */
	struct io_dfs		DFS;			/**< 2840_(9130 - 913Bh) */
	struct io_dfo		DFO;			/**< 2840_(9140 - 914Bh) */
	struct io_dfk		DFK;			/**< 2840_(9150 - 915Bh) */
	struct io_dfh		DFH;			/**< 2840_(9160 - 916Bh) */
	union  io_dfcsr 	DFCSR;			/**< 2840_(9170 - 9173h) */
	struct io_dflsth	DFLSTH; 		/**< 2840_(9178 - 917Fh) */
	union  io_dfagef	DFAGEF; 		/**< 2840_(9180 - 9183h) */
	union  io_dfagef2	DFAGEF2;		/**< 2840_(9184 - 9187h) */
	union  io_dfagth	DFAGTH; 		/**< 2840_(9188 - 918Bh) */
	union  io_dfagthk	DFAGTHK;		/**< 2840_(918C - 918Fh) */
	union  io_dmctl 	DMCTL;			/**< 2840_(9190 - 9193h) */
	union  io_dmwks 	DMWKS;			/**< 2840_(9194 - 9197h) */
	union  io_dmwkd 	DMWKD;			/**< 2840_(9198 - 919Bh) */
	union  io_dmagk 	DMAGK;			/**< 2840_(919C - 919Fh) */
	union  io_dectl 	DECTL;			/**< 2840_(91A0 - 91A3h) */
	union  io_denss 	DENSS;			/**< 2840_(91A4 - 91A7h) */
	union  io_denso 	DENSO;			/**< 2840_(91A8 - 91ABh) */
	union  io_densk 	DENSK;			/**< 2840_(91AC - 91AFh) */
	union  io_densh 	DENSH;			/**< 2840_(91B0 - 91B3h) */
	union  io_densmgn	DENSMGN;		/**< 2840_(91B4 - 91B7h) */
	union  io_dendcth	DENDCTH;		/**< 2840_(91B8 - 91BBh) */
	union  io_deagk 	DEAGK;			/**< 2840_(91BC - 91BFh) */
	union  io_dcagef	DCAGEF; 		/**< 2840_(91C0 - 91C3h) */
	union  io_dcagth	DCAGTH; 		/**< 2840_(91C4 - 91C7h) */
} T_IM_PRO_RDMA_DPC_VAL;

/** RDMA I/F FSHDL register write value information
*/
typedef struct {
	union  io_fshdlbc		FFSHBC;		/**< 2840_(931C - 931Fh) */
	union  io_fshdlul1		FFSHUL1;	/**< 2840_(9320 - 9323h) */
	union  io_fshdlul2		FFSHUL2;	/**< 2840_(9324 - 9327h) */
	union  io_fshdlgfmt 	FFSHGFMT;	/**< 2840_(9328 - 932Bh) */
	union  io_fshdlsubpr	FFSHSUBPR;	/**< 2840_(9330 - 9333h) */
	union  io_fshdlsubsp	FFSHSUBSP;	/**< 2840_(9334 - 9337h) */
	union  io_fshdlsubdr	FFSHSUBDR;	/**< 2840_(9338 - 933Bh) */
	union  io_fshdlsubst	FFSHSUBST;	/**< 2840_(933C - 933Fh) */
	union  io_fshdlctl		FFSHCTL;	/**< 2840_(9340 - 9343h) */
	union  io_fshdlhwph 	FFSHHWPH;	/**< 2840_(9344 - 9347h) */
	union  io_fshdlcctl 	FFSHCCTL;	/**< 2840_(9350 - 9353h) */
	union  io_fshdllp1		FFSHLP1;	/**< 2840_(9354 - 9357h) */
	union  io_fshdllp2		FFSHLP2;	/**< 2840_(9358 - 935Bh) */
} T_IM_PRO_RDMA_FSHDL_VAL;

/** RDMA I/F FFC register write value information
*/
typedef struct {
	union  io_ffc_fcbc		 FCBC;		/**< 2840_(971C - 971Fh) */
	union  io_ffc_fcsubpr	 FCSUBPR;	/**< 2840_(9734 - 9737h) */
	union  io_ffc_fcsubsp	 FCSUBSP;	/**< 2840_(9738 - 973Bh) */
	union  io_ffc_fcsubdr	 FCSUBDR;	/**< 2840_(973C - 973Fh) */
	union  io_ffc_fcsubst	 FCSUBST;	/**< 2840_(9740 - 9743h) */
} T_IM_PRO_RDMA_FFC_VAL;

/** RDMA I/F PGAIN register write value information
*/
typedef struct {
	union  io_pgainr		PGAINR;		/**< 2840_(8604 - 8607h) */
	union  io_pgaingr		PGAINGR;	/**< 2840_(8608 - 860Bh) */
	union  io_pgaingb		PGAINGB;	/**< 2840_(860C - 860Fh) */
	union  io_pgainb		PGAINB;		/**< 2840_(8610 - 8613h) */
	union  io_pofsr 		POFSR;		/**< 2840_(8614 - 8617h) */
	union  io_pofsgr		POFSGR;		/**< 2840_(8618 - 861Bh) */
	union  io_pofsgb		POFSGB;		/**< 2840_(861C - 861Fh) */
	union  io_pofsb 		POFSB;		/**< 2840_(8620 - 8623h) */
	union  io_pgaindp		PGAINDP;	/**< 2840_(8624 - 8627h) */
} T_IM_PRO_RDMA_PGAIN_VAL;

/** RDMA I/F PZSFT register write value information
*/
typedef struct {
	union  io_pzofst		PZOFST;	   /**< 2840_(9204 - 9207h) */
} T_IM_PRO_RDMA_PZSFT_VAL;

/** RDMA I/F CAG register write value information
*/
typedef struct {
	union  io_cagmd			CAGMD;		/**< 2840_(9818 - 981Bh) */
	union  io_rbr			RBR;		/**< 2840_(981C - 981Fh) */
	union  io_aboarv		ABOARV;		/**< 2840_(9820 - 9823h) */
	union  io_aboarh		ABOARH;		/**< 2840_(9824 - 9827h) */
	union  io_aboagv		ABOAGV;		/**< 2840_(9828 - 982Bh) */
	union  io_aboagh		ABOAGH;		/**< 2840_(982C - 982Fh) */
	union  io_aboabv		ABOABV;		/**< 2840_(9830 - 9833h) */
	union  io_aboabh		ABOABH;		/**< 2840_(9834 - 9837h) */
	union  io_abgar 		ABGAR;		/**< 2840_(9838 - 983Bh) */
	union  io_abgag 		ABGAG;		/**< 2840_(983C - 983Fh) */
	union  io_abgab 		ABGAB;		/**< 2840_(9840 - 9843h) */
	union  io_ablrv 		ABLRV;		/**< 2840_(9844 - 9847h) */
	union  io_ablrh 		ABLRH;		/**< 2840_(9848 - 984Bh) */
	union  io_ablgv 		ABLGV;		/**< 2840_(984C - 984Fh) */
	union  io_ablgh 		ABLGH;		/**< 2840_(9850 - 9853h) */
	union  io_ablbv 		ABLBV;		/**< 2840_(9854 - 9857h) */
	union  io_ablbh 		ABLBH;		/**< 2840_(9858 - 985Bh) */
	union  io_abofsrv		ABOFSRV;	/**< 2840_(985C - 985Fh) */
	union  io_abofsrh		ABOFSRH;	/**< 2840_(9860 - 9863h) */
	union  io_abofsgv		ABOFSGV;	/**< 2840_(9864 - 9867h) */
	union  io_abofsgh		ABOFSGH;	/**< 2840_(9868 - 986Bh) */
	union  io_abofsbv		ABOFSBV;	/**< 2840_(986C - 986Fh) */
	union  io_abofsbh		ABOFSBH;	/**< 2840_(9870 - 9873h) */
	union  io_abnlthr1		ABNLTHR1;	/**< 2840_(9874 - 9877h) */
	union  io_abnlthg1		ABNLTHG1;	/**< 2840_(9878 - 987Bh) */
	union  io_abnlthb1		ABNLTHB1;	/**< 2840_(987C - 987Fh) */
	union  io_abnlgar1		ABNLGAR1;	/**< 2840_(9880 - 9883h) */
	union  io_abnlgag1		ABNLGAG1;	/**< 2840_(9884 - 9887h) */
	union  io_abnlgab1		ABNLGAB1;	/**< 2840_(9888 - 988Bh) */
	union  io_abnlthr2		ABNLTHR2;	/**< 2840_(988C - 988Fh) */
	union  io_abnlthg2		ABNLTHG2;	/**< 2840_(9890 - 9893h) */
	union  io_abnlthb2		ABNLTHB2;	/**< 2840_(9894 - 9897h) */
	union  io_abnlgar2		ABNLGAR2;	/**< 2840_(9898 - 989Bh) */
	union  io_abnlgag2		ABNLGAG2;	/**< 2840_(989C - 989Fh) */
	union  io_abnlgab2		ABNLGAB2;	/**< 2840_(98A0 - 98A3h) */
	union  io_abnlthr3		ABNLTHR3;	/**< 2840_(98A4 - 98A7h) */
	union  io_abnlthg3		ABNLTHG3;	/**< 2840_(98A8 - 98ABh) */
	union  io_abnlthb3		ABNLTHB3;	/**< 2840_(98AC - 98AFh) */
	union  io_abnlgar3		ABNLGAR3;	/**< 2840_(98B0 - 98B3h) */
	union  io_abnlgag3		ABNLGAG3;	/**< 2840_(98B4 - 98B7h) */
	union  io_abnlgab3		ABNLGAB3;	/**< 2840_(98B8 - 98BBh) */
	union  io_abnlthr4		ABNLTHR4;	/**< 2840_(98BC - 98BFh) */
	union  io_abnlthg4		ABNLTHG4;	/**< 2840_(98C0 - 98C3h) */
	union  io_abnlthb4		ABNLTHB4;	/**< 2840_(98C4 - 98C7h) */
	union  io_abnlgar4		ABNLGAR4;	/**< 2840_(98C8 - 98CBh) */
	union  io_abnlgag4		ABNLGAG4;	/**< 2840_(98CC - 98CFh) */
	union  io_abnlgab4		ABNLGAB4;	/**< 2840_(98D0 - 98D3h) */
	union  io_bfglmt		BFGLMT; 	/**< 2840_(98D4 - 98D7h) */
	union  io_bfgth			BFGTH;		/**< 2840_(98D8 - 98DBh) */
} T_IM_PRO_RDMA_CAG_VAL;

/** RDMA I/F ELF register write value information
*/
typedef struct {
	union  io_elfctl		ELFCTL; 	/**< 2841_(0420 - 0423h) */
	union  io_elfzp 		ELFZP;		/**< 2841_(0424 - 0427h) */
	struct io_elfl			ELFL;		/**< 2841_(0428 - 042Fh) */
	struct io_anbor 		ANBOR;		/**< 2841_(0430 - 0437h) */
	struct io_anbkr 		ANBKR;		/**< 2841_(0438 - 043Fh) */
	struct io_anbdr 		ANBDR;		/**< 2841_(0440 - 0447h) */
	struct io_anbog 		ANBOG;		/**< 2841_(0448 - 044Fh) */
	struct io_anbkg 		ANBKG;		/**< 2841_(0450 - 0457h) */
	struct io_anbdg 		ANBDG;		/**< 2841_(0458 - 045Fh) */
	struct io_anbob 		ANBOB;		/**< 2841_(0460 - 0467h) */
	struct io_anbkb 		ANBKB;		/**< 2841_(0468 - 046Fh) */
	struct io_anbdb 		ANBDB;		/**< 2841_(0470 - 0477h) */
	struct io_lgtw			LGTW;		/**< 2841_(0478 - 047Fh) */
	union  io_lgt			LGT;		/**< 2841_(0480 - 0483h) */
	union  io_lgte			LGTE;		/**< 2841_(0484 - 0487h) */
	union  io_edg2anba		EDG2ANBA;	/**< 2841_(0488 - 048Bh) */
	union  io_edgscla		EDGSCLA;	/**< 2841_(048C - 048Fh) */
	union  io_edg2anbb		EDG2ANBB;	/**< 2841_(0490 - 0493h) */
	union  io_edgsclb		EDGSCLB;	/**< 2841_(0494 - 0497h) */
	union  io_edg2anbc		EDG2ANBC;	/**< 2841_(0498 - 049Bh) */
	union  io_edgsclc		EDGSCLC;	/**< 2841_(049C - 049Fh) */
	struct io_efth			EFTH;		/**< 2841_(04A0 - 04A7h) */
	union  io_lfth			LFTH;		/**< 2841_(04A8 - 04ABh) */
	union  io_elcome		ELCOME; 	/**< 2841_(04AC - 04AFh) */
	union  io_elcomen		ELCOMEN;	/**< 2841_(04B0 - 04B3h) */
	struct io_a1s5e 		A1S5E;		/**< 2841_(04B8 - 04BFh) */
	struct io_a1s5l1		A1S5L1; 	/**< 2841_(04C0 - 04C7h) */
	struct io_a1s5l2		A1S5L2; 	/**< 2841_(04C8 - 04CFh) */
	struct io_a1s5l3		A1S5L3; 	/**< 2841_(04D0 - 04D7h) */
	struct io_a1s5l4		A1S5L4; 	/**< 2841_(04D8 - 04DFh) */
	struct io_nr1s5rgb		NR1S5RGB;	/**< 2841_(04E0 - 04E7h) */
	union  io_ns1s5lm		NS1S5LM;	/**< 2841_(04E8 - 04EBh) */
	struct io_a5s9e 		A5S9E;		/**< 2841_(04F0 - 04F7h) */
	struct io_a5s9l1		A5S9L1; 	/**< 2841_(04F8 - 04FFh) */
	struct io_a5s9l2		A5S9L2; 	/**< 2841_(0500 - 0507h) */
	struct io_a5s9l3		A5S9L3; 	/**< 2841_(0508 - 050Fh) */
	struct io_a5s9l4		A5S9L4; 	/**< 2841_(0510 - 0517h) */
	struct io_nr5s9rgb		NR5S9RGB;	/**< 2841_(0518 - 051Fh) */
	union  io_ns5s9lm		NS5S9LM;	/**< 2841_(0520 - 0523h) */
	struct io_fs9w			FS9W;		/**< 2841_(0528 - 052Fh) */
	struct io_fs9yc 		FS9YC;		/**< 2841_(0540 - 0553h) */
	union  io_fastr 		FASTR;		/**< 2841_(0560 - 0563h) */
	union  io_crvaf 		CRVAF;		/**< 2841_(0564 - 0567h) */
	union  io_fcrvabtof 	FCRVABTOF;	/**< 2841_(0568 - 056Bh) */
	union  io_fcrvabtga 	FCRVABTGA;	/**< 2841_(056C - 056Fh) */
	union  io_fcrvabtbd 	FCRVABTBD;	/**< 2841_(0570 - 0573h) */
	union  io_fcrvabtcp 	FCRVABTCP;	/**< 2841_(0574 - 0577h) */
	union  io_fyabtga		FYABTGA;	/**< 2841_(0578 - 057Bh) */
	union  io_fyabtbd		FYABTBD;	/**< 2841_(057C - 057Fh) */
	union  io_fyabtclp		FYABTCLP;	/**< 2841_(0580 - 0583h) */
	union  io_faehh 		FAEHH;		/**< 2841_(0584 - 0587h) */
	union  io_fbstr 		FBSTR;		/**< 2841_(0588 - 058Bh) */
	union  io_crvbf 		CRVBF;		/**< 2841_(058C - 058Fh) */
	union  io_fcrvbbtof 	FCRVBBTOF;	/**< 2841_(0590 - 0593h) */
	union  io_fcrvbbtga 	FCRVBBTGA;	/**< 2841_(0594 - 0597h) */
	union  io_fcrvbbtbd 	FCRVBBTBD;	/**< 2841_(0598 - 059Bh) */
	union  io_fcrvbbtcp 	FCRVBBTCP;	/**< 2841_(059C - 059Fh) */
	union  io_fybbtga		FYBBTGA;	/**< 2841_(05A0 - 05A3h) */
	union  io_fybbtbd		FYBBTBD;	/**< 2841_(05A4 - 05A7h) */
	union  io_fybbtclp		FYBBTCLP;	/**< 2841_(05A8 - 05ABh) */
	union  io_fbehh 		FBEHH;		/**< 2841_(05AC - 05AFh) */
	union  io_elfmnnrrl 	ELFMNNRRL;	/**< 2841_(05B0 - 05B3h) */
	union  io_elfmnnrgl 	ELFMNNRGL;	/**< 2841_(05B4 - 05B7h) */
	union  io_elfmnnrbl 	ELFMNNRBL;	/**< 2841_(05B8 - 05BBh) */
	union  io_elfafnr		ELFAFNR;	/**< 2841_(05BC - 05BFh) */
	union  io_zeladcmd		ZELADCMD;	/**< 2841_(05C0 - 05C3h) */
	struct io_zanbor		ZANBOR; 	/**< 2841_(05C8 - 05CFh) */
	struct io_zanbkr		ZANBKR; 	/**< 2841_(05D0 - 05D7h) */
	struct io_zanbdr		ZANBDR; 	/**< 2841_(05D8 - 05DFh) */
	struct io_zanbog		ZANBOG; 	/**< 2841_(05E0 - 05E7h) */
	struct io_zanbkg		ZANBKG; 	/**< 2841_(05E8 - 05EFh) */
	struct io_zanbdg		ZANBDG; 	/**< 2841_(05F0 - 05F7h) */
	struct io_zanbob		ZANBOB; 	/**< 2841_(05F8 - 05FFh) */
	struct io_zanbkb		ZANBKB; 	/**< 2841_(0600 - 0607h) */
	struct io_zanbdb		ZANBDB; 	/**< 2841_(0608 - 060Fh) */
	struct io_zlgtw 		ZLGTW;		/**< 2841_(0610 - 0617h) */
	union  io_zlgt			ZLGT;		/**< 2841_(0618 - 061Bh) */
	union  io_zlgte 		ZLGTE;		/**< 2841_(061C - 061Fh) */
	union  io_zedg2anba 	ZEDG2ANBA;	/**< 2841_(0620 - 0623h) */
	union  io_zedgscla		ZEDGSCLA;	/**< 2841_(0624 - 0627h) */
	union  io_zedg2anbb 	ZEDG2ANBB;	/**< 2841_(0628 - 062Bh) */
	union  io_zedgsclb		ZEDGSCLB;	/**< 2841_(062C - 062Fh) */
	struct io_zalnge		ZALNGE; 	/**< 2841_(0630 - 0637h) */
	struct io_zalng 		ZALNG;		/**< 2841_(0640 - 0657h) */
	union  io_zlfth 		ZLFTH;		/**< 2841_(0660 - 0663h) */
	union  io_zlcome		ZLCOME; 	/**< 2841_(0664 - 0667h) */
	union  io_zlcomen		ZLCOMEN;	/**< 2841_(0668 - 066Bh) */
	union  io_znslnglm		ZNSLNGLM;	/**< 2841_(066C - 066Fh) */
	union  io_edgcath		EDGCATH;	/**< 2841_(0684 - 0687h) */
} T_IM_PRO_RDMA_ELF_VAL;

/** RDMA I/F FSHD register write value information
*/
typedef struct {
	union  io_fshdctl		FSHDCTL;	/**< 2841_(0A08 - 0A0Bh) */
	union  io_fshdhwph		FSHDHWPH;	/**< 2841_(0A1C - 0A1Fh) */
	union  io_fshdbc		FSHDBC; 	/**< 2841_(0A20 - 0A23h) */
	union  io_fsul1			FSUL1;		/**< 2841_(0A24 - 0A27h) */
	union  io_fsul2			FSUL2;		/**< 2841_(0A28 - 0A2Bh) */
	union  io_fsctl			FSCTL;		/**< 2841_(0A30 - 0A33h) */
	union  io_fssubpr		FSSUBPR;	/**< 2841_(0A34 - 0A37h) */
	union  io_fssubsp		FSSUBSP;	/**< 2841_(0A38 - 0A3Bh) */
	union  io_fssubdr		FSSUBDR;	/**< 2841_(0A3C - 0A3Fh) */
	union  io_fssubst		FSSUBST;	/**< 2841_(0A40 - 0A43h) */
	union  io_fsa1			FSA1;		/**< 2841_(0A44 - 0A47h) */
	union  io_fsa2			FSA2;		/**< 2841_(0A48 - 0A4Bh) */
	union  io_csctl1		CSCTL1; 	/**< 2841_(0A50 - 0A53h) */
	union  io_csctl2		CSCTL2; 	/**< 2841_(0A54 - 0A57h) */
	union  io_cstblst		CSTBLST;	/**< 2841_(0A58 - 0A5Bh) */
	union  io_cstbls1		CSTBLS1;	/**< 2841_(0A5C - 0A5Fh) */
	union  io_cstbls2		CSTBLS2;	/**< 2841_(0A60 - 0A63h) */
	union  io_cshdd 		CSHDD;		/**< 2841_(0A64 - 0A67h) */
	union  io_cshdoa		CSHDOA; 	/**< 2841_(0A68 - 0A6Bh) */
	union  io_csa1			CSA1;		/**< 2841_(0A6C - 0A6Fh) */
	union  io_csa2			CSA2;		/**< 2841_(0A70 - 0A73h) */
	union  io_fshdhcctl 	FSHDHCCTL;	/**< 2841_(0A80 - 0A83h) */
	union  io_fsslp1		FSSLP1; 	/**< 2841_(0A84 - 0A87h) */
	union  io_fsslp2		FSSLP2; 	/**< 2841_(0A88 - 0A8Bh) */
} T_IM_PRO_RDMA_FSHD_VAL;

/** RDMA I/F P2M knee table register write value information
*/
typedef struct {
	union io_p2mkntbl	P2MKNTBL;			/**< knee table register address */
} T_IM_PRO_RDMA_P2M_KNEE_TBL_VAL;

/** RDMA I/F M2P deknee table register write value information
*/
typedef struct {
	union io_m2pdkntbl	M2P0DKNTBL;			/**< deknee table register address */
} T_IM_PRO_RDMA_M2P_DEKNEE_TBL_VAL;

/** RDMA I/F SDC gain coefficient table register write value information
*/
typedef struct {
	union io_sro_sdcgd	SDCGD;				/**< gain coefficient table register address */
} T_IM_PRO_RDMA_SDC_GAIN_TBL_VAL;

/** RDMA I/F Frame Shading Concentric Circles Correction table register write value information
*/
typedef struct {
	union io_b2b_fshdctbl	FSHDCTBL;		/**< Frame Shading Concentric Circles Correction table register address */
} T_IM_PRO_RDMA_FSHD_CTBL_VAL;

/** RDMA I/F SPT Shading Correction table register write value information
*/
typedef struct {
	union io_spttbl		SPTTBL;				/**< SPT Shading Correction table register address */
} T_IM_PRO_RDMA_SPT_SHD_TBL_VAL;




/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#ifdef __cplusplus
	extern "C" {
#endif


/** @addtogroup im_pro_sen_top SENTOP
@{
*/

/**
SENTOP Macro initialize
@remarks	Please Call at the time of system starting.
*/
extern	VOID Im_PRO_SENTOP_Init( VOID );

/**
SENTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
extern	INT32 Im_PRO_SENTOP_SW_Reset( VOID );

/**
SENTOP Macro clock control
@param[in]	clk_type	: SENTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SENTOP_Control_Clock( E_IM_PRO_SENTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip );

/**
SENTOP macro control data setting
@param[in]	ctrl : SENTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SENTOP_Ctrl( T_IM_PRO_SENTOP_CTRL* ctrl );

/**
SENTOP BitShift macro control data setting
@param[in]	ctrl : SENTOP BitShift macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SENTOP_BS_Ctrl( T_IM_PRO_SENTOP_BS_CTRL* ctrl );

/**
SENTOP OBT macro setting
@param[in]	ctrl : SENTOP BitShift macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_OBT_Ctrl( T_IM_PRO_SENTOP_OBT_CTRL* ctrl );

/**
SENTOP OBT macro weight coefficient setting
@param[in]	coeff : SENTOP OBT macro weight coefficient information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_OBT_Set_Weight_Coeff( T_IM_PRO_SENTOP_OBT_WEIGHT_COEFF* coeff );

/**
SENTOP LVDS macro setting
@param[in]	ctrl : SENTOP LVDS macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_LVDS_Ctrl( T_IM_PRO_SENTOP_LVDS_CTRL* ctrl );

/**
SENTOP SLVS macro setting
@param[in]	ctrl : SENTOP SLVS macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_SLVS_Ctrl( T_IM_PRO_SENTOP_SLVS_CTRL* ctrl );

/**
SENTOP SG macro setting
@param[in]	ctrl : SG macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_SG_Ctrl( T_IM_PRO_SENTOP_SG_CTRL* ctrl );

/**
SENTOP ComboPHY macro setting
@param[in]	cphy_ctrl : ComboPHY control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Ctrl( T_IM_PRO_SENTOP_CPHY_CTRL* cphy_ctrl );

/**
SENTOP ComboPHY macro Clock setting
@param[in]	clk_ctrl : ComboPHY clock control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Clk_Ctrl( T_IM_PRO_SENTOP_CPHY_CLK_CTRL* clk_ctrl );

/**
SENTOP ComboPHY macro Bias setting
@param[in]	bias_ctrl : ComboPHY Bias control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Bias_Ctrl( T_IM_PRO_SENTOP_CPHY_BIAS_CTRL* bias_ctrl );

/**
SENTOP ComboPHY SLVS-EC mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Slvs_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_SLVS_MODE_CTRL* mode_ctrl );

/**
SENTOP ComboPHY Sub-LVDS mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Lvds_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_LVDS_MODE_CTRL* mode_ctrl );

/**
SENTOP ComboPHY MIPI D-PHY mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_DMIPI_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_DMIPI_MODE_CTRL* mode_ctrl );

/**
SENTOP ComboPHY MIPI C-PHY mode setting
@param[in]	mode_ctrl : ComboPHY mode control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_CMIPI_Mode_Ctrl( T_IM_PRO_SENTOP_CPHY_CMIPI_MODE_CTRL* mode_ctrl );

/**
SENTOP ComboPHY Extension Signals setting
@param[in]	ext_signal : ComboPHY extension signals control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Ext_Signal_Ctrl( T_IM_PRO_SENTOP_CPHY_IF_EXT_SIGNAL_CTRL* ext_signal );

/**
Get CPHY DesKnew Monitor pin status.
@param[out]	cphy_moni : CPHY DesKnew Monitor pin status information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Get_DesKnew_Monitor( T_IM_PRO_SENTOP_CPHY_DESKNEW_MONITOR* cphy_moni );

/**
Get CPHY DelayLine Monitor pin status.
@param[out]	cphy_moni : CPHY DelayLine Monitor pin status information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Get_DelayLine_Monitor( T_IM_PRO_SENTOP_CPHY_DELAYLINE_MONITOR* cphy_moni );

/**
Get CPHY Common I/F Monitor pin status.
@param[out]	cphy_moni : CPHY Common I/F Monitor pin status information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_SENTOP_CPHY_Get_Common_If_Monitor( T_IM_PRO_SENTOP_CPHY_COMMON_IF_MONITOR* cphy_moni );

// --- REMOVE_BUZ BEGIN
// @cond
/**
SENTOP Pseudo VD Issue setting
@retval		D_DDIM_OK					: Setting OK
*/
extern INT32 Im_PRO_SENTOP_Pseudo_VD_Issue( VOID );
// @endcond
// --- REMOVE_BUZ END

/*	@}*/	// im_pro_sen_top

/** @addtogroup im_pro_sen_sg SG
@{
*/
/**
The SG operation(Trigger) started.
@param[in]	ch : SG ch number
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SG_Start( E_IM_PRO_SG_CH ch );

/**
The SG operation(Trigger) stopped(Frame).
@param[in]	ch : SG ch number
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SG_Stop( E_IM_PRO_SG_CH ch, UCHAR force );

/**
The SG operation condition is set.
@param[in]	ch : SG ch number
@param[in]	sg_ctrl : SG Control info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SG_Ctrl( E_IM_PRO_SG_CH ch, T_IM_PRO_SG_CTRL* sg_ctrl );

/**
The SG operation condition is get.
@param[in]	ch : SG ch number
@param[out]	sg_ctrl : SG Control info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SG_Get_Cycle( E_IM_PRO_SG_CH ch, T_IM_PRO_SG_CTRL* sg_ctrl );

/**
Get monitoring result of the input VD and HD signals.
@param[in]	ch : SG ch number
@param[out]	sg_moni_info : SG monitor information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SG_Get_Monitor_Info( E_IM_PRO_SG_CH ch, T_IM_PRO_SG_MONI_INFO* sg_moni_info );
/*	@}*/	// im_pro_sen_sg


/** @addtogroup im_pro_sro_top SROTOP
@{
*/

/**
SROTOP Macro initialize
@param[in]	unit_no : Unit number.
@remarks	Please Call at the time of system starting.
*/
extern	VOID Im_PRO_SROTOP_Init( E_IM_PRO_UNIT_NUM unit_no );

/**
SROTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
extern	INT32 Im_PRO_SROTOP_SW_Reset( E_IM_PRO_UNIT_NUM unit_no );

/**
SROTOP Macro clock control
@param[in]	unit_no : Unit number.
@param[in]	clk_type	: SROTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SROTOP_Control_Clock( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SROTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip );

/**
SROTOP macro control data setting
@param[in]	unit_no : Unit number.
@param[in]	ctrl : SROTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SROTOP_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_SROTOP_CTRL* ctrl );

/**
WEITGEN macro control data setting
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	weitgen_ctrl : WEITGEN macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SROTOP_WEITGEN_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_WEITGEN_CH ch, T_IM_PRO_SROTOP_WEITGEN_CTRL* weitgen_ctrl );

/**
setup of enable to WEITGEN macro.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	enabled : setup of enable to WEITGEN macro.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SROTOP_WEITGEN_Enable( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_WEITGEN_CH ch, E_IM_PRO_WEITGEN_ENABLE enabled );

/*	@}*/	// im_pro_sro_top

/** @addtogroup im_pro_b2b_top B2BTOP
@{
*/

/**
B2BTOP Macro initialize
@param[in]	unit_no : Unit number.
@remarks	Please Call at the time of system starting.
*/
extern	VOID Im_PRO_B2BTOP_Init( E_IM_PRO_UNIT_NUM unit_no );

/**
B2BTOP Macro software reset
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
extern	INT32 Im_PRO_B2BTOP_SW_Reset( E_IM_PRO_UNIT_NUM unit_no );

/**
B2BTOP Macro clock control
@param[in]	unit_no : Unit number.
@param[in]	clk_type	: B2BTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_B2BTOP_Control_Clock( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_B2BTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip );

/**
B2BTOP macro control data setting
@param[in]	unit_no : Unit number.
@param[in]	ctrl : B2BTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_B2BTOP_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_B2BTOP_CTRL* ctrl );

/*	@}*/	// im_pro_b2b_top


/** @addtogroup im_pro_stat_top STATTOP
@{
*/

/**
STATTOP Macro initialize
@remarks	Please Call at the time of system starting.
*/
extern	VOID Im_PRO_STATTOP_Init( VOID );

/**
STATTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
extern	INT32 Im_PRO_STATTOP_SW_Reset( VOID );

/**
STATTOP Macro clock control
@param[in]	clk_type	: STATTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_STATTOP_Control_Clock( E_IM_PRO_STATTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip );

/**
STATTOP macro control data setting
@param[in]	ctrl : STATTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_STATTOP_Ctrl( T_IM_PRO_STATTOP_CTRL* ctrl );

/*	@}*/	// im_pro_stat_top

/** @addtogroup im_pro_pas_top PASTOP
@{
*/

/**
PASTOP Macro initialize
@remarks	Please Call at the time of system starting.
*/
extern	VOID Im_PRO_PASTOP_Init( VOID );

/**
PASTOP Macro software reset
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
extern	INT32 Im_PRO_PASTOP_SW_Reset( VOID );

/**
PASTOP Macro clock control
@param[in]	clk_type	: PASTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PASTOP_Control_Clock( E_IM_PRO_PASTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip );

/**
PASTOP macro control data setting
@param[in]	ctrl : PASTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PASTOP_Ctrl( T_IM_PRO_PASTOP_CTRL* ctrl );


/*	@}*/	// im_pro_pas_top

/** @addtogroup im_pro_common_pg PG
@{
*/

/**
PG macro start.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not running NG
*/
extern	INT32 Im_PRO_PG_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type );

/**
PG macro stop.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not stopped NG
*/
extern	INT32 Im_PRO_PG_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type );

/**
PG macro control parameter is set. 
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	pg_ctrl : PG control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PG_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, T_IM_PRO_PG_CTRL* pg_ctrl);

/*	@}*/	// im_pro_common_pg

/** @addtogroup im_pro_sen_slvs SLVS-EC
@{
*/

/**
@weakgroup im_pro_sen_slvs_seq Control Sequence
@{
	- @ref initialization_sequence_slvs
	- @ref standby_sequence_slvs
	- @ref mode_change_sequence_slvs

	<hr>
	@section initialization_sequence_slvs	Initialization sequence.
	@image html im_pro_slvs_init_seq.png

	<hr>
	@section standby_sequence_slvs			Standby sequence.
	@image html im_pro_slvs_standby_seq.png

	<hr>
	@section mode_change_sequence_slvs		Mode Change sequence.
	@image html im_pro_slvs_mode_change_seq.png

@}*/

/**
Software reset of SLVS-EC RX macro(excluding PMA, termination resistor adjustment circuit and CNT block).
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_SLVS_SR( E_IM_PRO_SLVS_STREAM_TYPE stream_type );

/**
Software reset release of SLVS-EC.
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_SLVS_SR_Rlease( E_IM_PRO_SLVS_STREAM_TYPE stream_type );

/**
Update of the SLVS-EC configuration parameters.
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_SLVS_Update_Cfg( E_IM_PRO_SLVS_STREAM_TYPE stream_type );

/**
Physical Media Attachment initialization.
@param[in]	pma_init : PMA initialization parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Pma_Init( T_IM_PRO_SLVS_PMA_INIT* pma_init );

/**
Physical Media Attachment Power Down control.
@param[in]	pma_pd : PMA Power Down control parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Pma_Power_Down( T_IM_PRO_SLVS_PMA_PD* pma_pd );

/**
Physical Media Attachment PLL Power Down control.
@param[in]	pd_state : PMA PLL Power Down control parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Pma_Pll_Power_Down( E_IM_PRO_SLVS_PD_STATE pd_state );

/**
SLVS-EC Common Configuration.
@param[in]	common_cfg : Common configuration parameters.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Common_Cfg( T_IM_PRO_SLVS_COMMON_CFG* common_cfg );

/**
SLVS-EC Configuration.
@param[in]	stream_type : stream type of SLVS
@param[in]	slvs_ctrl : configuration parameters for each stream.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Ctrl( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_CTRL* slvs_ctrl );

/**
SLVS-EC Mode Configuration.
@param[in]	stream_type : stream type of SLVS
@param[in]	mode_ctrl : mode configuration parameters for each stream.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Mode_Cfg( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_MODE_CFG* mode_ctrl );

/**
SLVS-EC Trimming Configuration.
@param[in]	stream_type : stream type of SLVS
@param[in]	trim_ctrl : trimming configuration parameters for each stream.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NG					: Processing NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Trim_Cfg( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_TRIM_CFG* trim_ctrl );


/**
Get SLVS-EC status.
@param[in]	stream_type : stream type of SLVS
@param[out]	status : status parameter pointer.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Get_Status( E_IM_PRO_SLVS_STREAM_TYPE stream_type, T_IM_PRO_SLVS_STATUS* status );

/**
SLVS-EC macro start.
@param[in]	stream_type : stream type of SLVS
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Start( E_IM_PRO_SLVS_STREAM_TYPE stream_type );

/**
Stop SLVS-EC information
@param[in]	stream_type : stream type of SLVS
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Parameter invalid
*/
extern	INT32 Im_PRO_SLVS_Stop( E_IM_PRO_SLVS_STREAM_TYPE stream_type, UCHAR force );

/**
wait end of SLVS-EC macro.
@param[in]		waiptn   : waiting end cause. (bit field)
						<ul><li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FSI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FSO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FEI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FEO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_RDY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_STBY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FSI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FSO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FEI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FEO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_RDY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_STBY_WAITEND_FLG</ul>
@param[in]		tmout    : timeout.
@param[out]		p_flgptn : end cause. (bit field)
						<ul><li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FSI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FSO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FEI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_FEO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_RDY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_A_INTST_STBY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FSI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FSO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FEI_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_FEO_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_RDY_WAITEND_FLG
							<li>@ref D_IM_PRO_SLVS_STREAM_B_INTST_STBY_WAITEND_FLG</ul>
@retval			D_DDIM_OK                  : SLVS-EC normal end. and set end cause to p_flgptn.
@retval			D_IM_PRO_INPUT_PARAM_ERROR : parameter error.
@retval			D_IM_PRO_TIMEOUT     : wait timeout.
@remarks		This API uses DDIM_User_Clr_Flg().
@remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Im_PRO_SLVS_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, const DDIM_USER_FLGPTN waiptn, const DDIM_USER_TMO tmout );

/*	@}*/	// im_pro_sen_slvs

/** @addtogroup im_pro_sen_lvds LVDS
@{
*/
/**
LVDS macro start.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: LVDS has not stopped NG
*/
extern	INT32 Im_PRO_LVDS_Start( E_IM_PRO_LVDS_CH ch );

/**
LVDS macro stop.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
extern	INT32 Im_PRO_LVDS_Stop( E_IM_PRO_LVDS_CH ch );

/**
The control parameter of LVDS is set.
@param[in]	ch : Channel No.
@param[in]	lvds_ctrl : LVDS control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_LVDS_Ctrl( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_CTRL* lvds_ctrl );

/**
LVDS Input starting position setup .
@param[in]	ch : Channel No.
@param[in]	input_pos	: LVDS input position<br>
						 value range:[0 - 12288pixel](Multiples of 4)<br>
						 target registor:@@LHSADD
@param[in]	input_size : LVDS input size<br>
						 value range:[4 - ](Multiples of 4)<br>
						 maximum size that can be set by each SENCORE block set by SENTOP.SENRAMSW.<br>
						 target registor:@@LHSIZE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@remarks	Only in the case of @@SZEN = 1 (see Im_PRO_LVDS_Ctrl), the value set up by this API is effective. 
*/
extern	INT32 Im_PRO_LVDS_Set_Area( E_IM_PRO_LVDS_CH ch, USHORT input_pos, USHORT input_size );

/**
LVDS output data rearrangement setup.
@param[in]	ch : Channel No.
@param[in]	lvds_order : LVDS pixel order information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_LVDS_Set_PixelOrder( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_PIXEL_ORDER* lvds_order );

/**
The sync code parameter of LVDS is set.
@param[in]	ch : Channel No.
@param[in]	sync_code : LVDS sync code information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@code
		//sample code
		T_IM_PRO_LVDS_SYNC_CODE		sync_code;
		
		sync_code.sav[0] = 0x0FFF;
		sync_code.sav[1] = 0x0000;
		sync_code.sav[2] = 0x0000;
		sync_code.sav[3] = 0x0800;
		
		sync_code.eav[0] = 0x0FFF;
		sync_code.eav[1] = 0x0000;
		sync_code.eav[2] = 0x0000;
		sync_code.eav[3] = 0x09D0;

		sync_code.sab[0] = 0x0FFF;
		sync_code.sab[1] = 0x0000;
		sync_code.sab[2] = 0x0000;
		sync_code.sab[3] = 0x0AB0;

		sync_code.eab[0] = 0x0FFF;
		sync_code.eab[1] = 0x0000;
		sync_code.eab[2] = 0x0000;
		sync_code.eab[3] = 0x0B60;
		
		Im_PRO_LVDS_Set_SyncCode( &sync_code );
@endcode
*/
extern	INT32 Im_PRO_LVDS_Set_SyncCode( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_SYNC_CODE_CTRL* sync_code );

/**
LVDS sync code Mask setup.
@param[in]	ch : Channel No.
@param[in]	sync_mask : LVDS sync mask information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@code
		//sample code
		T_IM_PRO_LVDS_SYNC_MASK		sync_code;
		
		sync_mask.msav[0] = 0x0001;
		sync_mask.msav[1] = 0x0001;
		sync_mask.msav[2] = 0x0001;
		sync_mask.msav[3] = 0x0001;
		
		sync_mask.meav[0] = 0x0001;
		sync_mask.meav[1] = 0x0001;
		sync_mask.meav[2] = 0x0001;
		sync_mask.meav[3] = 0x0001;

		sync_mask.msab[0] = 0x0001;
		sync_mask.msab[1] = 0x0001;
		sync_mask.msab[2] = 0x0001;
		sync_mask.msab[3] = 0x0001;

		sync_mask.meab[0] = 0x0001;
		sync_mask.meab[1] = 0x0001;
		sync_mask.meab[2] = 0x0001;
		sync_mask.meab[3] = 0x0001;
		
		Im_PRO_LVDS_Set_SyncMask( &sync_mask );
@endcode
*/
extern	INT32 Im_PRO_LVDS_Set_SyncMask( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_SYNC_MASK_CTRL* sync_mask );

/**
LVDS Input starting position setup .
@param[in]	ch : Channel No.
@param[in]	int_addr	: SOL/EOL interrupt address information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_LVDS_Set_IntAdr( E_IM_PRO_LVDS_CH ch, T_IM_PRO_LVDS_INT_ADDR_CTRL* int_addr );

/**
A setup of enable access to the built-in RAM of LVDS.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_LVDS_Set_PAEN( E_IM_PRO_LVDS_CH ch, UCHAR paen_trg );

/*	@}*/	// im_pro_sen_lvds


/** @addtogroup im_pro_sen_dmipi	MIPI-DPHY
@{
*/

/**
MIPI-DPHY initialize
@param[in]	ch : MIPI-DPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DMIPI_Init( E_IM_PRO_DMIPI_CH ch );

/**
Software reset of MIPI-DPHY
@param[in]	ch : MIPI-DPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DMIPI_SR( E_IM_PRO_DMIPI_CH ch );

/**
MIPI-DPHY macro start.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: D-MIPI has not stopped NG
*/
extern	INT32 Im_PRO_DMIPI_Start( E_IM_PRO_DMIPI_CH ch );

/**
MIPI-DPHY macro stop.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
extern	INT32 Im_PRO_DMIPI_Stop( E_IM_PRO_DMIPI_CH ch, UCHAR force );

/**
The control parameter of MIPI-DPHY is set.
@param[in]	ch : Channel No.
@param[in]	dmipi_ctrl : MIPI-DPHY control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_DMIPI_Ctrl( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_DMIPI_CTRL* dmipi_ctrl );

/**
A setup of enable access to the built-in RAM of MIPI-DPHY.
@param[in]	ch : Channel No.
@param[in]	paen0_trg : RAM0 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@param[in]	paen1_trg : RAM1 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_DMIPI_Set_PAEN( E_IM_PRO_DMIPI_CH ch, UCHAR paen0_trg, UCHAR paen1_trg );

/**
Get MIPI D-PHY status..
@param[in]	ch : Channel No.
@param[out]	status : MIPI-DPHY status information
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
extern	INT32 Im_PRO_DMIPI_Get_Status( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_DMIPI_STATUS* status );

/*	@}*/	// im_pro_sen_dmipi


/**
MIPI-CPHY initialize
@param[in]	ch : MIPI-CPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_CMIPI_Init( E_IM_PRO_CMIPI_CH ch );

/**
Software reset of MIPI-CPHY
@param[in]	ch : MIPI-CPHY channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_CMIPI_SR( E_IM_PRO_CMIPI_CH ch );

/**
MIPI-CPHY macro start.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: D-MIPI has not stopped NG
*/
extern	INT32 Im_PRO_CMIPI_Start( E_IM_PRO_CMIPI_CH ch );

/**
MIPI-CPHY macro stop.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
extern	INT32 Im_PRO_CMIPI_Stop( E_IM_PRO_CMIPI_CH ch, UCHAR force );

/**
The control parameter of MIPI-CPHY is set.
@param[in]	ch : Channel No.
@param[in]	cmipi_ctrl : MIPI-CPHY control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_CMIPI_Ctrl( E_IM_PRO_CMIPI_CH ch, T_IM_PRO_CMIPI_CTRL* cmipi_ctrl );

/**
A setup of enable access to the built-in RAM of MIPI-CPHY.
@param[in]	ch : Channel No.
@param[in]	paen0_trg : RAM0 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@param[in]	paen1_trg : RAM1 access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_CMIPI_Set_PAEN( E_IM_PRO_CMIPI_CH ch, UCHAR paen0_trg, UCHAR paen1_trg );

/**
Get MIPI C-PHY status..
@param[in]	ch : Channel No.
@param[out]	status : MIPI-CPHY status information
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
extern	INT32 Im_PRO_CMIPI_Get_Status( E_IM_PRO_CMIPI_CH ch, T_IM_PRO_CMIPI_STATUS* status );



/** @addtogroup im_pro_common_moni MONI
@{
*/
/**
MONI macro start.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
*/
extern	INT32 Im_PRO_MONI_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch );

/**
Stop MONI information
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[in]	force : force stop option
*/
extern	INT32 Im_PRO_MONI_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, UCHAR force );

/**
MONI macro control parameter is set. 
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[in]	moni_ctrl : MONI control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_MONI_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, T_IM_PRO_MONI_CTRL* moni_ctrl );

/**
Get monitoring result of MONI macro.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[out]	moni_result : Monitoring result of MONI macro.
*/
extern	INT32 Im_PRO_MONI_Get_Monitor_Count( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, T_IM_PRO_MONI_RESULT* moni_result );

/*	@}*/	// im_pro_common_moni


/** @addtogroup im_pro_sen_ldiv LDIV
@{
*/

/**
LDIV macro start.
@param[in]	ch : Channel No.
*/
extern	INT32 Im_PRO_LDIV_Start( E_IM_PRO_LDIV_CH ch );

/**
Stop LDIV information
@param[in]	ch : Channel No.
@param[in]	force : force stop option
*/
extern	INT32 Im_PRO_LDIV_Stop( E_IM_PRO_LDIV_CH ch, UCHAR force );

/**
LDIV macro control parameter is set. 
@param[in]	ch : Channel No.
@param[in]	ldiv_ctrl : LDIV control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_LDIV_Ctrl( E_IM_PRO_LDIV_CH ch, T_IM_PRO_LDIV_CTRL* ldiv_ctrl );

/**
The area for Top OB detection is set up. 
@param[in]	ch : Channel No.
@param[in]	ldiv_area	:The area of LDIV processing. See @ref T_IM_PRO_LDIV_AREA.
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_LDIV_Set_Area( E_IM_PRO_LDIV_CH ch, T_IM_PRO_LDIV_AREA* ldiv_area );

/**
A setup of enable access to the built-in RAM of LDIV.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_LDIV_Set_PAEN( E_IM_PRO_LDIV_CH ch, UCHAR paen_trg );

/*	@}*/	// im_pro_sen_ldiv

/** @addtogroup im_pro_common_lnr LNR
@{
*/
/**
Start LNR information
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped NG
*/
extern	INT32 Im_PRO_LNR_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_LNR_CH ch );

/**
Stop LNR information
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
extern	INT32 Im_PRO_LNR_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_LNR_CH ch, UCHAR force );

/**
Set LNR information
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[in]	lnr_ctrl	: LNR Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_LNR_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_LNR_CH ch, T_IM_PRO_LNR_CTRL* lnr_ctrl );

/*	@}*/	// im_pro_common_lnr

/** @addtogroup im_pro_sen_obt OBT
@{
*/
/**
Start TOP OB macro.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_OBT_Start( E_IM_PRO_OBT_CH ch );

/**
Stop TOP OB macro.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBT_Stop( E_IM_PRO_OBT_CH ch, UCHAR force );

/**
The control parameter of TOP OB compensation is set.
@param[in]	ch : Channel No.
@param[in]	obt_ctrl	:	TOP OB Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBT_Ctrl( E_IM_PRO_OBT_CH ch, T_IM_PRO_OBT_CTRL* obt_ctrl );

/**
The area for Top OB detection is set up. 
@param[in]	ch : Channel No.
@param[in]	obt_area	:The area for Top OB detection. See @ref T_IM_PRO_AREA_INFO<br>
							value range  :pos_x[0 - 12287]<br>
							target registor  :@@OBTH<br>
							value range  :pos_y[0 - 8191]<br>
							target registor  :@@OBTV<br>
							value range  :width[1 - 12288]	1pixel boundary(OBTMD=0)<br>
															2pixel boundary(OBTMD=1)<br>
							target registor  :@@OBTHW<br>
							value range  :lines[1 - 127]	1pixel boundary(OBTMD=0)<br>
															2pixel boundary(OBTMD=1)<br>
							target registor  :@@OBTVW
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_OBT_Set_Area( E_IM_PRO_OBT_CH ch, T_IM_PRO_AREA_INFO* obt_area );

/**
Set TOP OB offset
@param[in]	ch : Channel No.<br>
				 value range:[0 - 1]<br>
@param[in]	offset : TOP OB offset<br>
					 value range : R :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSR<br>
					 value range : Gr :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSGR<br>
					 value range : Gb :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSGB<br>
					 value range : B :[0x4000(=-16384) - 0x3FFF(=+16383)]<br>
					 target registor :@@OBTOFSB
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@remarks	Please set up a minus value by a complement. bit15 is sign bit 
*/
extern	INT32 Im_PRO_OBT_Set_Offset( E_IM_PRO_OBT_CH ch, T_IM_PRO_RGB2* offset );

/**
Get TOP OB detection data
@param[in]	ch : Channel No.<br>
				 value range:[0 - 1]<br>
@param[out]	obtdata : OBTDATA
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBT_Get_OBTData( E_IM_PRO_OBT_CH ch, T_IM_PRO_RGB2* obtdata );

/*	@}*/	// im_pro_sen_obt

/** @addtogroup im_pro_b2b_elf ELF
@{
*/
/**
Start ELF
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH not running NG
*/
extern	INT32 Im_PRO_ELF_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Stop ELF
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_ELF_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
A setup of enable access to the built-in RAM of ELF.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_ELF_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
ELF control common setup 
@param[in]	unit_no : Unit number.
@param[in]	elf_common_ctrl : ELF control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Ctrl_Common( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_COMMON_CTRL* elf_common_ctrl );

/**
ELF control setup 
@param[in]	unit_no : Unit number.
@param[in]	elf_ctrl : ELF control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_CTRL* elf_ctrl );

/**
A setup of ELF effective area 
@param[in]	unit_no : Unit number.
@param[in]	elf_area : ELF area information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_AREA* elf_area );

/**
Set noise func information.
@param[in]	unit_no : Unit number.
@param[in]	nfunc : ELF noise func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_Noisefunc( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_NOISE_FUNC* nfunc );

/**
Set Light/Outlines filter information.
@param[in]	unit_no : Unit number.
@param[in]	lgt_filter : ELF Light/Outlines filter func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_LGT_EDG_Filter( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_LGT_EDG_FUNC* lgt_filter );

/**
Set E/L filter information.
@param[in]	unit_no : Unit number.
@param[in]	e_l_filter : ELF E/L filter func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_E_L_Filter( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_L_E_FUNC* e_l_filter );

/**
Set F9 filter information.
@param[in]	unit_no : Unit number.
@param[in]	f9_filter : ELF F9 filter func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_F9_Filter( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_f9_SAMPLING* f9_filter );

/**
Set F9 filter RGB gain.
@param[in]	unit_no : Unit number
@param[in]	rgb_gain : White balance gain
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern INT32 Im_PRO_ELF_Set_F9_RGBGain( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_f9_RGB_GAIN* rgb_gain );

/**
Set 1s5 sampling information.
@param[in]	unit_no : Unit number.
@param[in]	s1s5_sampling : ELF 1s5 sampling information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_1s5_Sampling( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_1s5_SAMPLING* s1s5_sampling );

/**
Set 5s9 sampling information.
@param[in]	unit_no : Unit number.
@param[in]	s5s9_sampling : ELF 5s9 sampling information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_5s9_Sampling( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_5s9_SAMPLING* s5s9_sampling );

/**
Set Adjust the intensity noise suppression information.
@param[in]	unit_no : Unit number.
@param[in]	noise_adj_filter : ELF Adjust the intensity noise suppression information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_Noise_Adj( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_Noise_Adj* noise_adj_filter );

/**
Set A0-function noise suppression information.
@param[in]	unit_no : Unit number.
@param[in]	a0_filter : A0-function noise suppression information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_A0_Sampling( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_a0_FILTER* a0_filter );

/**
Start NSL
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH not running NG
*/
extern	INT32 Im_PRO_ELF_Start_NSL( E_IM_PRO_UNIT_NUM unit_no );

/**
Stop NSL
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_ELF_Stop_NSL( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
NSL control setup 
@param[in]	unit_no : Unit number.
@param[in]	nsl_ctrl : ELF control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Ctrl_NSL( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_NSL_CTRL* nsl_ctrl );

/**
A setup of enable access to the built-in RAM of NSL.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_ELF_Set_NSL_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
A setup of enable access to the built-in RAM of A0 function.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_ELF_Set_A0_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );
/**
Set Edge level adjustment parameter for high frequency area
@param[in]	unit_no : Unit number.
@param[in]	edge_adj : Edge level adjustment parameter
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_ELF_Set_Edge_Adj( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_HIGH_FREQ_AREA_EDGE_ADJ* edge_adj );


/*	@}*/	// im_pro_b2b_elf


/** @addtogroup im_pro_common_pzsft PZSFT
@{
*/
/**
Start optical zero level shift.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: MAcro has not stopped NG
*/
extern	INT32 Im_PRO_PZSFT_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_PZSFT_CH ch );

/**
Stop optical zero level shift.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
extern	INT32 Im_PRO_PZSFT_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_PZSFT_CH ch, UCHAR force );

/**
Set optical zero level shift
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	pzsft_ctrl : Level Shift offset value
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PZSFT_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_PZSFT_CH ch, T_IM_PRO_PZSFT_CTRL* pzsft_ctrl );

/*	@}*/	// im_pro_common_pzsft

/** @addtogroup im_pro_b2b_fshd FSHD
@{
*/
/**
Flat Shading Start
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not running NG
@remarks	Flat Shading Compensation function is started(enable).
*/
extern	INT32 Im_PRO_FSHD_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Flat Shading Stop
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not stopped NG
@remarks	Flat Shading Compensation function is stopped(disable).
*/
extern	INT32 Im_PRO_FSHD_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
Flat Shading Control
@param[in]	unit_no : Unit number.
@param[in]	fshd_ctrl : Shading Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHD_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_COMMON_CTRL* fshd_ctrl );

/**
A setup of enable access to the built-in RAM of FSHD.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_FSHD_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
A Setup is Flat Shading Area
@param[in]	unit_no : Unit number.
@param[in]	fshd_area : Area for Shading compensation See @ref T_IM_PRO_AREA_INFO<br>
 												value range  :pos_x[0 - 4095]<br>
 												target registor  :@@FSHDH<br>
 												value range  :pos_y[2 - 8960]<br>
 												target registor  :@@FSHDV<br>
 												value range  :width[4 - 2496] 2pixel boundary<br>
 												target registor  :@@FSHDHW<br>
 												value range  :lines[4 - 8960] 4pixel boundary<br>
 												target registor  :@@FSHDVW
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHD_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_AREA_INFO* fshd_area );

/**
Flat Shading (Frame Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	blend_ctrl : Frame correction blend ratio information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHD_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_BLEND_CTRL* blend_ctrl );

/**
Frame Shading (Frame Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	fshd_ctrl : Frame Correction Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHD_Set_Frame( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_FRAME_CTRL* fshd_ctrl );

/**
Frame Shading (Concentric Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	fshd_ctrl : Concentric Correction Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHD_Set_Concentric( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_CONCENTRIC_CTRL* fshd_ctrl );

/**
Frame Shading (Concentric Correction) Table Number Acquisition
@param[in]	unit_no : Unit number.
@retval		Concentric Correction Table Number.<br>
			value range :[0:RAM0 1:RAM1]:@@CSTBST
*/
extern	E_IM_PRO_FSHD_LUT_SEL Im_PRO_FSHD_Get_Ctbl_Number( E_IM_PRO_UNIT_NUM unit_no );

/**
Frame shading compensation table is registered.
@param[in]	unit_no : Unit number.
@param[in]	tbl_sel : table select.
@param[in]	shd_tbl		: Shading table info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@note		None
*/
extern	INT32 Im_PRO_FSHD_Set_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHD_LUT_SEL tbl_sel, T_IM_PRO_FSHD_TABLE* shd_tbl );

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
			Always set this register to maintain the following relationship:<br>
			(FCWRIC < ((Input horizontal size - (4*FSSUBPRH)) - 10)/3)<br>
			target registor	:@@FCWRIC
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FSHD_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, USHORT req_interval_clk );
/*	@}*/	// im_pro_b2b_fshd


/** @addtogroup im_pro_common_pgain PGAIN
@{
*/
/**
Start PGain
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	select		: select PGAIN 0/1/2.
@retval		D_DDIM_OK					: Macro Start OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: MAcro has not stopped NG
*/
extern	INT32 Im_PRO_PGAIN_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PGAIN_SELECT select );

/**
Stop PGain
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	select		: select PGAIN 0/1/2.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Macro Stop OK
@retval		D_IM_PRO_NG					: Macro Stop NG
*/
extern	INT32 Im_PRO_PGAIN_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PGAIN_SELECT select, UCHAR force );

/**
Set PGain
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	select		: select PGAIN 0/1/2.
@param[in]	pgain_ctrl : Pgain param<br>
						value range :R[0 - 8191(5-bit integer part,8-bit fractional part)]<br>
						target registor :@@PGAINR<br>
						value range :Gr[0 - 8191(5-bit integer part,8-bit fractional part)]<br>
						target registor :@@PGAINGR<br>
						value range :Gb[0 - 8191(5-bit integer part,8-bit fractional part)]<br>
						target registor :@@PGAINGB<br>
						value range :B[0 - 8191(5-bit integer part,8-bit fractional part)]<br>
						target registor :@@PGAINB
						value range :R_offset[-4096 - +4095(12-bit signed)]<br>
						target registor :@@POFSR<br>
						value range :Gr_offset[-4096 - +4095(12-bit signed)]<br>
						target registor :@@POFSGR<br>
						value range :Gb_offset[-4096 - +4095(12-bit signed)]<br>
						target registor :@@POFSGB<br>
						value range :B_offset[-4096 - +4095(12-bit signed)]<br>
						target registor :@@POFSB
						
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PGAIN_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PGAIN_SELECT select, T_IM_PRO_PGAIN_CTRL* pgain_ctrl );

/*	@}*/	// im_pro_common_pgain


/** @addtogroup im_pro_sro_afpb AFPB
@{
*/

/**
Start AFPB
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
*/
extern	VOID Im_PRO_AFPB_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_AFPB_CH ch );

/**
Stop AFPB
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_AFPB_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_AFPB_CH ch, UCHAR force );

/**
Set AFPB control information
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	afpb_ctrl	: AFPB control Information.
*/
extern	INT32 Im_PRO_AFPB_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_AFPB_CH ch, T_IM_PRO_AFPB_CTRL* afpb_ctrl );

/*	@}*/	// im_pro_sro_afpb

/** @addtogroup im_pro_sro_fshdl FSHDL
@{
*/

/**
Flat Shading Start
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not running NG
@remarks	Flat Shading Compensation function is started(enable).
*/
extern	INT32 Im_PRO_FSHDL_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch );

/**
Flat Shading Stop
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not stopped NG
@remarks	Flat Shading Compensation function is stopped(disable).
*/
extern	INT32 Im_PRO_FSHDL_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, UCHAR force );

/**
Flat Shading Control
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	fshdl_ctrl : Shading Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHDL_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_FSHDL_COMMON_CTRL* fshdl_ctrl );

/**
Flat Shading Blend ratio Control
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	blend_ctrl : Shading Blend Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHDL_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_FSHDL_BLEND_CTRL* blend_ctrl );

/**
A setup of enable access to the built-in RAM of FSHDL.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_FSHDL_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, UCHAR paen_trg );

/**
A Setup is Flat Shading Area
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	fshdl_area : Area for Shading compensation See @ref T_IM_PRO_AREA_INFO<br>
 												value range  :pos_x[0 - 4095]<br>
 												target registor  :@@FFSHH<br>
 												value range  :pos_y[2 - 8960] 2line boundary<br>
 												target registor  :@@FFSHV<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
 												value range  :width[4 - 2496] 4pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
												value range  :width[4 - 2900] 2pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
 												target registor  :@@FFSHHW<br>
 												value range  :lines[4 - 8960] 4pixel boundary<br>
 												target registor  :@@FFSHVW
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHDL_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_AREA_INFO* fshdl_area );

/**
Flat Shading (Frame Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	fshdl_ctrl : Frame Correction Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_FSHDL_Set_Frame( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, T_IM_PRO_FSHDL_FRAME_CTRL* fshdl_ctrl );

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	ch : Channel No.
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
			Always set this parameter to maintain the following relationship:<br>
			(FFSHWRIC < ((Input horizontal size - (4* FFSHSUBPRH)) - 10)/3)<br>
			target registor	:@@FFSHWRIC
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FSHDL_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, USHORT req_interval_clk );

/*	@}*/	// im_pro_sro_fshdl


/** @addtogroup im_pro_sro_cag CAG
@{
*/

/**
Start CAG
@param[in]	unit_no : Unit number.
*/
extern	VOID Im_PRO_CAG_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Stop CAG
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_CAG_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
Set CAG control information
@param[in]	unit_no : Unit number.
@param[in]	cag_ctrl	: CAG control Information.
*/
extern	INT32 Im_PRO_CAG_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CAG_CTRL* cag_ctrl );

/**
A setup of enable access to the built-in RAM of CAG.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg :  control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_CAG_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
Set CAG area
@param[in]	unit_no : Unit number.
@param[in]	cag_area	: CAG area.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_CAG_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CAG_AREA* cag_area );

/**
Set CAG opt_center_pos_h  control
@param[in]	unit_no : Unit number.
@param[in]	opt_center_pos_h	: CAG control Information.
*/
extern INT32 Im_PRO_CAG_Opt_Center_Pos_H_Ctrl( E_IM_PRO_UNIT_NUM unit_no, SHORT opt_center_pos_h [3] );

/**
Set CAG G level difference information
@param[in]	unit_no : Unit number.
@param[in]	cag_ctrl_glevel_diff	: CAG G level difference Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_CAG_Ctrl_Glv_Diff( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CAG_CTRL_GLEVEL_DIFF* cag_ctrl_glevel_diff );

/*	@}*/	// im_pro_sro_cag

/** @addtogroup im_pro_sro_ffc FFC
@{
*/

/**
Start FFC
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH/M2P not running NG
*/
extern	INT32 Im_PRO_FFC_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Stop FFC
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH/M2P not stopped NG
*/
extern	INT32 Im_PRO_FFC_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
Set FFC control information
@param[in]	unit_no : Unit number.
@param[in]	ffc_ctrl	: FFC control Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFC_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FFC_CTRL* ffc_ctrl );

/**
Set FFC blend control information
@param[in]	unit_no : Unit number.
@param[in]	blend_ctrl	: FFC blend control Information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFC_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FFC_BLEND_CTRL* blend_ctrl );

/**
A setup of enable access to the built-in RAM of FFC.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_FFC_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
Set FFC area
@param[in]	unit_no : Unit number.
@param[in]	ffc_area	: FFC area.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFC_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FFC_AREA* ffc_area );

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
			Always set this register to maintain the following relationship:<br>
			(FCWRIC < ((Input horizontal size - (4* FCSUBPRH)) - 10)/3) <br>
			target registor	:@@FCWRIC
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFC_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, USHORT req_interval_clk );

/*	@}*/	// im_pro_sro_ffc

/** @addtogroup im_pro_sro_hrb HRB
@{
*/
/**
Start HRB
@param[in]	unit_no : Unit number.
*/
extern	VOID Im_PRO_HRB_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Stop HRB
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_HRB_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
HRB control data setting
@param[in]	unit_no : Unit number.
@param[in]	hrb_ctrl : HRB control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HRB_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_HRB_CTRL* hrb_ctrl );

/**
A setup of enable access to the built-in RAM of HRB.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_HRB_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/*	@}*/	// im_pro_sro_hrb


/** @addtogroup im_pro_sro_vrb VRB
@{
*/
/**
Start VRB
@param[in]	unit_no : Unit number.
*/
extern	VOID Im_PRO_VRB_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Stop VRB
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_VRB_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
VRB control data setting
@param[in]	unit_no : Unit number.
@param[in]	vrb_ctrl : VRB control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_VRB_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_VRB_CTRL* vrb_ctrl );

/**
A setup of enable access to the built-in RAM of VRB.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_VRB_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );
/*	@}*/	// im_pro_sro_vrb


// --- REMOVE_BUZ BEGIN
// @cond
/** @addtogroup im_pro_sro_buztop BUZTOP
@{
*/
/**
Software reset of BUZTOP macro.
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_BUZTOP_SR( E_IM_PRO_UNIT_NUM unit_no );

/**
Software reset release of BUZTOP.
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_BUZTOP_SR_Rlease( E_IM_PRO_UNIT_NUM unit_no );

/**
BUZTOP control data setting
@param[in]	unit_no : Unit number.
@param[in]	buz_ctrl : BUZTOP control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_BUZTOP_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_BUZTOP_CTRL* buz_ctrl );

/**
A setup of enable access to the built-in RAM of BUZTOP.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_BUZTOP_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
A setup of Resizer select.
@param[in]	unit_no : Unit number.
@param[in]	sel     : Raw resizer select.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_BUZTOP_Set_Resizer_Sel( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BUZTOP_SEL sel );
/*	@}*/	// im_pro_sro_buztop
// @endcond
// --- REMOVE_BUZ END

/** @addtogroup im_pro_stat_aeawb AEAWB
@{
*/
/**
AEAWB macro start
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
extern	INT32 Im_PRO_AEAWB_Start( E_IM_PRO_AEAWB_CH ch );

/**
AEAWB macro stop
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
extern	INT32 Im_PRO_AEAWB_Stop( E_IM_PRO_AEAWB_CH ch, UCHAR force );

/**
AE/AWB data SDRAM write enable set
@param[in]	ch : Channel No.
@param[in]	aeawb_ctrl : AEAWB Information @ref T_IM_PRO_AEAWB_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Ctrl( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_CTRL* aeawb_ctrl );

/**
A setup of enable access to the built-in RAM of AEAWB.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_AEAWB_Set_PAEN( E_IM_PRO_AEAWB_CH ch, UCHAR paen_trg );

/**
AWB detect data format select.
@param[in]	ch : Channel No.
@param[in]	awb_format : AWB detect data format @ref E_IM_PRO_AWB_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_AWBDataFormat( E_IM_PRO_AEAWB_CH ch, E_IM_PRO_AWB_MODE awb_format );

/**
AE/AWB detecting mode set
@param[in]	ch : Channel No.
@param[in]	wb_mode : AEAWB Detecting mode @ref E_IM_PRO_AEAWB_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_WD_Mode( E_IM_PRO_AEAWB_CH ch, E_IM_PRO_AEAWB_MODE wb_mode );

/**
AE/AWB detecting window mode set
@param[in]	ch : Channel No.
@param[in]	wb_win_mode : AEAWB Detecting window mode @ref E_IM_PRO_AEAWB_WD_WINDOW_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_WD_Window_Mode( E_IM_PRO_AEAWB_CH ch, E_IM_PRO_AEAWB_WD_WINDOW_MODE wb_win_mode );

/**
Set AE/AWB Window size
@param[in]	ch : Channel No.
@param[in]	aeawb_area :aeawb window information.

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_Area( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_AREA* aeawb_area );

/**
Set AE/AWB Window block number
@param[in]	ch : Channel No.
@param[in]	h_bc_num :horizontal AEAWB Window block number<br>
						value range  :[1 - 64]<br>
						target registor  :@@AEHBC<br>
						Note that if level WB is enabled (aeawb_mode=E_IM_PRO_AEAWB_MODE_LWB~E_IM_PRO_AEAWB_MODE_AE_AWB_LWB),
						the upper limit of this register is 32.

@param[in]	v_bc_num :vertical AEAWB Window block number<br> 
					  value range  :[1 - 64]<br>
					  target registor  :@@AEVBC
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32	Im_PRO_AEAWB_Set_Blocks( E_IM_PRO_AEAWB_CH ch, USHORT h_bc_num, USHORT v_bc_num );

/**
Set calculation permission in AE/AWB Window block.
@param[in]	ch : Channel No.
@param[in]	aeawb_cw :AE/AWB Addition permission size in a block information<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_CW( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_CW* aeawb_cw );

/**
AWB RGB level gate threshold
@param[in]	ch : Channel No.
@param[in]	lv_gate	:	AWB RGB gate threshold
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_Lv_Gate( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_LV_GATE* lv_gate );

/**
AWB matrix level gate threshold
@param[in]	ch : Channel No.
@param[in]	mt_lv_gate	:AWB Matrix gate
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_Matrix_Lv_Gate( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_MATRIX_LV_GATE* mt_lv_gate );

/**
AE Y coefficient
@param[in]	ch : Channel No.
@param[in]	aeyk	:Luminosity generation coefficient Information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_AEYK( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_STAT_YK* aeyk );

/**
Get Luminance generation coefficient.
@param[in]	ch : Channel No.
@param[out]	aeyk	:Luminosity generation coefficient Information
@retval		Luminance generation coefficient.
*/
extern	INT32	Im_PRO_AEAWB_Get_AEYK( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_STAT_YK* aeyk );

/**
Luminosity is integrated from the specified line in the AEAWB detection Window
@param[in]	ch : Channel No.
@param[in]	line_info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_Line_Data_Position( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_LINE_INFO* line_info );

/**
Get flicker result line data. Result is ULONG [4] address.
@param[in]	ch : Channel No.
@param[out]	line_data	:line data Result of ULONG [4].
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Get_Line_Data( E_IM_PRO_AEAWB_CH ch, ULONG* line_data );

/**
Get AE Data
@param[in]	ch : Channel No.
@retval		AE Data Address	:	AE Data Storing Area Address
*/
extern	ULONG Im_PRO_AEAWB_Get_AEAWBData( E_IM_PRO_AEAWB_CH ch );

/**
Get AE Data last VD for AFE long exposure
@param[in]	ch : Channel No.
@retval		ULONG AE Data Storing Area Address for AFE long exposure
*/
extern	ULONG Im_PRO_AEAWB_Get_LastAEAWBData( E_IM_PRO_AEAWB_CH ch );

/**
Get Wave Detection Data information
@param[in]	ch : Channel No.
@param[out]	wd_info : Wave Detection Data information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Get_WD_Info( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_WD_INFO* wd_info );

/*	@}*/	// im_pro_stat_aeawb

/** @addtogroup im_pro_stat_af AF
@{
*/
/**
AF macro start
@param[in]	ch : Channel No.
*/
extern	VOID Im_PRO_AF_Start( E_IM_PRO_AF_CH ch );

/**
AF macro stop
@param[in]	ch : Channel No.
@param[in]	force : force stop option
*/
extern	VOID Im_PRO_AF_Stop( E_IM_PRO_AF_CH ch, UCHAR force );

/**
AF Information Setting
@param[in]	ch : Channel No.
@param[in]	af_ctrl	:	Window Information See @ref T_IM_PRO_AF_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Ctrl( E_IM_PRO_AF_CH ch, T_IM_PRO_AF_CTRL* af_ctrl );

/**
A setup of enable access to the built-in RAM of AF.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
			value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_AF_Set_PAEN( E_IM_PRO_AF_CH ch, UCHAR paen_trg );

/**
Set AF Global Window 
@param[in]	ch : Channel No.
@param[in]	global_win :AF global area information<br>
							value range :pos_x[0 - 8190](Multiples of 2)<br>
							target registor :@@AFGH<br>
							value range :pos_y[0 - 16382](Multiples of 2)<br>
							target registor :@@AFGV<br>
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
							value range :width[2 - 2496] 2pixel boundary<br>
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
							value range :width[2 - 2900] 2pixel boundary<br>
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
							target registor :@@AFGHW<br>
							value range :lines[2 - 16382] 2pixel boundary<br>
							target registor :@@AFGVW<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@attention	None
*/
extern	INT32 Im_PRO_AF_Set_Global_Area( E_IM_PRO_AF_CH ch, T_IM_PRO_AREA_INFO* global_win );

/**
AF Surface color Information
@param[in]	ch : Channel No.
@param[in]	unit_no	:				Evaluated value calculation unit number. See @ref E_IM_PRO_AF_EVAL_CALC_UNIT<br>
@param[in]	af_eval_calc_ctrl	:	Evaluated value calculation unit Information. See @ref T_IM_PRO_AF_EVAL_CALC_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Ctrl_Eval_Calc( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_EVAL_CALC_UNIT unit_no, T_IM_PRO_AF_EVAL_CALC_CTRL* af_eval_calc_ctrl );

/**
AF Vertical detection Information
@param[in]	ch : Channel No.
@param[in]	af_vertical_ctrl	:	Vertical detection Information See @ref T_IM_PRO_AF_VERTICAL_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Ctrl_Vertical( E_IM_PRO_AF_CH ch, T_IM_PRO_AF_VERTICAL_CTRL* af_vertical_ctrl );

/**
AF Horizontal detection Information
@param[in]	ch : Channel No.
@param[in]	tap_no	:			Horizontal detection TAP number See @ref E_IM_PRO_AF_TAP<br>
@param[in]	af_horizon_ctrl	:	Horizontal detection Information See @ref T_IM_PRO_AF_HORIZON_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Ctrl_Horizon( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_TAP tap_no, T_IM_PRO_AF_HORIZON_CTRL* af_horizon_ctrl );

/**
AF Window Information Setting
@param[in]	ch : Channel No.
@param[in]	win_no		:	Window No. See @ref E_IM_PRO_AF_WINDOW_NO
@param[in]	af_win_ctrl	:	Window Information See @ref T_IM_PRO_AF_WIN_CTRL<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_AF_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Ctrl_Win( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no, T_IM_PRO_AF_WIN_CTRL* af_win_ctrl );

/**
Get AF evaluated calculation Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Eval_Calc_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Bright block Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Bright_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Dark block Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Dark_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Horizontal Detection 0 Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The AF Horizontal 0 data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal0_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Horizontal Detection 1 Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The AF Horizontal 1 data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal1_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Horizontal Detection 2 Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The AF Horizontal 2 data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal2_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Vertical Detection Data
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The AF Vertical data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Vertical_Data( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result maximum evaluated calculation value AF detection window
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The maximum color value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Max_Eval_Calc_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result minimum evaluated calculation value AF detection window
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The minimum color value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Min_Eval_Calc_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result maximum assessment value AF detection window
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Max_Assess_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result minimum assessment value AF detection window
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The minimum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Min_Assess_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Detection Window Horizontal0 Detection Each Line Maximum Value Accumulation result.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal0_Line_Accum_Max_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Detection Window Horizontal1 Detection Each Line Maximum Value Accumulation result.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal1_Line_Accum_Max_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Detection Window Horizontal2 Detection Each Line Maximum Value Accumulation result.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal2_Line_Accum_Max_Value( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF evaluated calculation Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Eval_Calc_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF evaluated calculation Bright block Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Bright_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF evaluated calculation Dark block Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Dark_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Horizontal 0 Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal0_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Horizontal 1 Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal1_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Horizontal 2 Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal2_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Vertical Data for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG brightness data address pointer
@remarks	The brightness data is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Vertical_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result maximum evaluated calculation value AF detection window for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum color value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Max_Eval_Calc_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result minimum evaluated calculation value AF detection window for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The minimum color value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Min_Eval_Calc_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result maximum assessment value AF detection window for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Max_Assess_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get As a result minimum assessment value AF detection window for AFE long exposure
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The minimum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Min_Assess_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Detection Window Horizontal0 Detection Each Line Maximum Value Accumulation result for AFE long exposure.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal0_Line_Accum_Max_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Detection Window Horizontal1 Detection Each Line Maximum Value Accumulation result for AFE long exposure.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal1_Line_Accum_Max_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/**
Get AF Detection Window Horizontal2 Detection Each Line Maximum Value Accumulation result for AFE long exposure.
@param[in]	ch : Channel No.
@param[in]	win_no	:	Window Number( AF0 - 14 )
@retval		ULONG* data address pointer
@remarks	The maximum assessment value is acquired. 
*/
extern	ULONG Im_PRO_AF_Get_Horizontal2_Line_Accum_Max_LastData( E_IM_PRO_AF_CH ch, E_IM_PRO_AF_WINDOW_NO win_no );

/*	@}*/	// im_pro_stat_af

/** @addtogroup im_pro_stat_hist HIST
@{
*/
/**
HIST macro start
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
extern	INT32 Im_PRO_HIST_Start( E_IM_PRO_HIST_CH ch );

/**
HIST macro stop
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
extern	INT32 Im_PRO_HIST_Stop( E_IM_PRO_HIST_CH ch, UCHAR force );

/**
HIST detecting information set
@param[in]	ch : Channel No.
@param[in]	hist_ctrl : HIST Information @ref T_IM_PRO_HIST_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Ctrl( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_CTRL* hist_ctrl );

/**
A setup of enable access to the built-in RAM of HIST.
@param[in]	ch : Channel No.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_HIST_Set_PAEN( E_IM_PRO_HIST_CH ch, UCHAR paen_trg );

/**
HIST detecting mode set
@param[in]	ch : Channel No.
@param[in]	hist_cycle : HIST horizontal subsampling cycle. @ref E_IM_PRO_HIST_CYCLE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Set_H_Cycle( E_IM_PRO_HIST_CH ch, E_IM_PRO_HIST_CYCLE hist_cycle );

/**
Set HIST Window size
@param[in]	ch : Channel No.
@param[in]	hist_area : HIST window information<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Set_Area( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_AREA* hist_area );

/**
Set HIST Window size
@param[in]	ch : Channel No.
@param[in]	hist_gain : HIST window information<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Set_Gain( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_GAIN* hist_gain );

/**
HIST Y coefficient
@param[in]	ch : Channel No.
@param[in]	histyk	:Luminosity generation coefficient Information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Set_HISTYK( E_IM_PRO_HIST_CH ch, T_IM_PRO_STAT_YK* histyk );

/**
Get HIST result Data
@param[in]	ch : Channel No.
@param[out]	hist_data :Histgram result value inside the HIST window.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Get_HIST_Data( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_DATA* hist_data );

/**
Get HIST result RAM Data
@param[in]	ch : Channel No.
@param[out]	hist_ram_data :Histgram result RAM value inside the HIST window.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Get_HIST_Ram( E_IM_PRO_HIST_CH ch, T_IM_PRO_HIST_RAM_DATA* hist_ram_data );

/*	@}*/	// im_pro_stat_hist

/** @addtogroup im_pro_stat_flck FLCK
@{
*/
/**
FLCK macro start
@param[in]	ch : Channel No.
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
extern	INT32 Im_PRO_FLCK_Start( E_IM_PRO_FLCK_CH ch );

/**
FLCK macro stop
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
extern	INT32 Im_PRO_FLCK_Stop( E_IM_PRO_FLCK_CH ch, UCHAR force );

/**
FLCK detecting information set
@param[in]	ch : Channel No.
@param[in]	flck_ctrl : FLCK Information @ref T_IM_PRO_FLCK_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Ctrl( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_CTRL* flck_ctrl );

/**
FLCK detect data format select.
@param[in]	ch : Channel No.
@param[in]	flck_format : FLCK detect data format @ref E_IM_PRO_FLCK_TYPE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_FLCKDataFormat( E_IM_PRO_FLCK_CH ch, E_IM_PRO_FLCK_TYPE flck_format );

/**
FLCK detecting mode set
@param[in]	ch : Channel No.
@param[in]	flck_mode : FLCK Detecting mode @ref E_IM_PRO_FLCK_MODE
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_WD_Mode( E_IM_PRO_FLCK_CH ch, E_IM_PRO_FLCK_MODE flck_mode );

/**
Set FLCK Window size
@param[in]	ch : Channel No.
@param[in]	flck_area : flck window information<br>
						value range :[0 - 8190] 2pixel boundary<br>
						target registor :@@FLH<br>
						value range :[0 - 8190] 2pixel boundary<br>
						target registor :@@FLV<br>
						value range :[6 - 8192] 2pixel boundary<br>
						target registor :@@FLHW<br>
						value range :[2 - 1024] 2pixel boundary(FLWLMD=0)<br>
												4pixel boundary(FLWLMD=1)<br>
						target registor :@@FLVW<br>

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_Area( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_AREA* flck_area );

/**
Set FLCK Window block number
@param[in]	ch : Channel No.
@param[in]	bc_num :FLCK Window block number<br>
					value range :[1 - 4096]<br>
					target registor :@@FLBC<br>

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_Blocks( E_IM_PRO_FLCK_CH ch, USHORT bc_num );

/**
Set calculation permission in FLCK Window block
@param[in]	ch : Channel No.
@param[in]	flcvw : FLCK Addition permission size in a block information<br>
					 value range :[2 - 1024] 2pixel boundary(FLWLMD=0)<br>
											 4pixel boundary(FLWLMD=1)<br>
					 target registor :@@FLCVW<br>

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_CW( E_IM_PRO_FLCK_CH ch, USHORT flcvw );

/**
FLCK RGB level gate threshold
@param[in]	ch : Channel No.
@param[in]	lv_gate	:	FLCK RGB gate threshold
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_Lv_Gate( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_LV_GATE* lv_gate );

/**
FLCK matrix level gate threshold
@param[in]	ch : Channel No.
@param[in]	mt_lv_gate	:FLCK Matrix gate
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_Matrix_Lv_Gate( E_IM_PRO_FLCK_CH ch, T_IM_PRO_FLCK_MATRIX_LV_GATE* mt_lv_gate );

/**
FLCK Y coefficient
@param[in]	ch : Channel No.
@param[in]	flckyk	:Luminosity generation coefficient Information

@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FLCK_Set_FLYK( E_IM_PRO_FLCK_CH ch, T_IM_PRO_STAT_YK* flckyk );

/**
Get FLCK Data
@retval		FLCK Data Address	:	FLCK Data Storing Area Address
*/
extern	ULONG Im_PRO_FLCK_Get_FLCKData( E_IM_PRO_FLCK_CH ch );

/**
Get FLCK Last Data
@param[in]	ch : Channel No.
@retval		FLCK Data Address	:	FLCK Data Storing Area Address(Last address)
*/
extern	ULONG Im_PRO_FLCK_Get_LastFLCKData( E_IM_PRO_FLCK_CH ch );

/*	@}*/

/** @addtogroup im_pro_sen_obd OBD
@{
*/
/**
Start OBD
@param[in]	ch : Channel No.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBD_Start( E_IM_PRO_OBD_CH ch );

/**
Stop OBD
@param[in]	ch : Channel No.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBD_Stop( E_IM_PRO_OBD_CH ch, UCHAR force );

/**
The control parameter of OBD compensation is set.
@param[in]	ch : Channel No.
@param[in]	ob_ctrl	:	OBD Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBD_Ctrl( E_IM_PRO_OBD_CH ch, T_IM_PRO_OBD_CTRL* ob_ctrl );

/**
The area for OBD detection is set up. 
@param[in]	ch : Channel No.
@param[in]	ob_area	:The area for OB detection<br>
					value range :pos_x[0 - 12287]<br>
					target registor :@@OBDH<br>
					value range :pos_y[0 - 8191]<br>
					target registor :@@OBDV<br>
					value range :width[2 - 12288] 2pixel boundary<br>
					target registor :@@OBDHW<br>
					value range :lines[2 - 8192] 2pixel boundary<br>
					target registor :@@OBDVW<br>
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
extern	INT32 Im_PRO_OBD_Set_Area( E_IM_PRO_OBD_CH ch, T_IM_PRO_AREA_INFO* ob_area );

/**
Get OBD count
@param[in]	ch : Channel No.
@param[out]	obcnt :	OBCNT<br>
			each colors count value range :0 - 0xFFFFF<br>
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
extern	INT32 Im_PRO_OBD_Get_OBCNT( E_IM_PRO_OBD_CH ch, T_IM_PRO_RGB4* obcnt );

/**
Get OBD data
@param[in]	ch : Channel No.
@param[out]	obdata : OBD Data
@retval		D_DDIM_OK					: Getting OK
@retval		D_IM_PRO_NG					: Getting NG
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Getting NG
*/
extern	INT32 Im_PRO_OBD_Get_OBData( E_IM_PRO_OBD_CH ch, T_IM_PRO_RGB4* obdata );

/*	@}*/	// im_pro_sen_obd

/** @addtogroup im_pro_pas_spt SPT
@{
*/

/**
SPT macro start
@retval		D_DDIM_OK				: Processing OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PWCH not running NG
*/
extern	INT32 Im_PRO_SPT_Start( VOID );

/**
SPT macro stop
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
extern	INT32 Im_PRO_SPT_Stop( UCHAR force );

/**
Setup of SPT control parameter.
@param[in]	spt_ctrl : SPT Information @ref T_IM_PRO_SPT_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SPT_Ctrl( T_IM_PRO_SPT_CTRL* spt_ctrl );

/**
A setup of enable access to the built-in RAM of SPT.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_SPT_Set_PAEN( UCHAR paen_trg );

/**
shading compensation table is registered.
@param[in]	tbl_sel : table select.
@param[in]	shd_tbl		: Shading table info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@note		None
*/
extern	INT32 Im_PRO_SPT_Set_Table( E_IM_PRO_SPT_TBL_SEL tbl_sel, T_IM_PRO_SPT_TABLE* shd_tbl );

/*	@}*/	// im_pro_pas_spt

/** @addtogroup im_pro_common_p2m P2M
@{
*/

/**
Start P2M
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.<br>
				 value range:[0 - 5]<br>
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not running NG
*/
extern	INT32 Im_PRO_P2M_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch );

/**
Stop P2M
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.<br>
				 value range:[0 - 5]<br>
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PWCH not stopped NG
*/
extern	INT32 Im_PRO_P2M_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, UCHAR force );

/**
The control parameter of P2M compensation is set.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.<br>
				 value range:[0 - 5]<br>
@param[in]	p2m_ctrl	:	P2M Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_P2M_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, T_IM_PRO_P2M_CTRL* p2m_ctrl );

/**
A setup of enable access to the built-in RAM of P2N.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.<br>
				 value range:[0 - 5]<br>
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_P2M_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, UCHAR paen_trg );

/**
The LUT Table parameter of P2M compensation is set.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[in]	p2m_tbl	:	P2M LUT Table information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_P2M_Set_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, T_IM_PRO_P2M_TABLE* p2m_tbl );



/*	@}*/	// im_pro_common_p2m

/** @addtogroup im_pro_common_pwch PWCH
@{
*/

/**
PWch macro start
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch );

/**
PWch macro stop
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, UCHAR force );

/**
PWch Control information
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	pw_ctrl : PWch macro control information See @ref T_IM_PRO_PWCH_CTRL
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_PWCH_CTRL* pw_ctrl );

/**
PWch Control information set AXI bus I/F Control
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	axi_ctrl : AXI control setting.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Ctrl_Axi( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_AXI_CTRL* axi_ctrl );

/**
PWCH storing address setup
@param[in]	unit_no : Unit number.
@param[in]	block_type	: PRO block type
@param[in]	ch			: PWch number
@param[in]	pwch_info	:	PWch Bayer storing management information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Set_Buf_Addr( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_PWCH_INFO* pwch_info );

/**
Get PWCH storing buffer address
@param[in]	unit_no : Unit number.
@param[in]	block_type	: PRO block type
@param[in]	ch			: PWch number
@retval		ULONG Bayer Data Storing Area Address
*/
extern	ULONG Im_PRO_PWch_Get_Buf_Data( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch );

/**
PRO PWch Bayer Buffer Index Update
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Update_Buf_Index( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch );

/**
Bayer data storing place address setup
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	line_feed	:	Line feed information. 8byte boundary,<br>
													but as the alignment processing is performed in 32byte unit, it is recommended to set this value to a multiple of 32byte.<br>
													To perform the alignment processing properly when this value is set to a value which is not a multiple of 32byte,<br>
													write with mask must be enabled.(@ref Im_PRO_PWch_Ctrl() argument of @ref E_IM_PRO_PWCH_WRITE_MASK.)
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Set_LineFeed( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, ULONG line_feed );

/**
Gets AXI response data
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[out]	axi_response	: AXI response data
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Get_AXI_Response( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, UCHAR* axi_response );

/**
Enable PCLK for PRO
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@remarks		For debugging use only (for register dump on debugger)
*/
extern	VOID Im_PRO_On_Pclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type );

/**
Enable HCLK for PRO
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@remarks		For debugging use only (for register dump on debugger)
*/
extern	VOID Im_PRO_On_Hclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type );

/**
Disable PCLK for PRO
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@remarks		For debugging use only (for register dump on debugger)
*/
extern	VOID Im_PRO_Off_Pclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type );

/**
Disable HCLK for PRO
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@remarks		For debugging use only (for register dump on debugger)
*/
extern	VOID Im_PRO_Off_Hclk( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type );

/*	@}*/	// im_pro_common_pwch


/** @addtogroup im_pro_sro_ffsh FFSH
@{
*/

/**
FFSH start
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not running NG
*/
extern	INT32 Im_PRO_FFSH_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch );

/**
FFSH stop
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not stopped NG
*/
extern	INT32 Im_PRO_FFSH_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, UCHAR force );

/**
FFSH control setup 
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ffsh_ctrl : FFSH control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFSH_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, T_IM_PRO_FFSH_CTRL* ffsh_ctrl );

/**
A setup of FFSH blend ratio.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	blend_ctrl : FFSH blend control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFSH_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, T_IM_PRO_FFSH_BLEND_CTRL* blend_ctrl );

/**
A setup of FFSH effective area 
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ffsh_area : FFSH area information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFSH_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, T_IM_PRO_FFSH_AREA* ffsh_area );

/**
A setup of enable access to the built-in RAM of FFSH.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_FFSH_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, UCHAR paen_trg );

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
			Always set this register to maintain the following relationship:<br>
			(FFSHWRIC < ((Input horizontal size - (4* FFSHSUBPRH)) - 10)/3)<br>
			target registor	:@@FFSHWRIC
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FFSH_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, USHORT req_interval_clk );

/*	@}*/	// im_pro_sro_ffsh


/** @addtogroup im_pro_sro_ff FF
@{
*/

/**
FF start.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FF_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch );

/**
FF stop.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FF_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, UCHAR force );

/**
Setup of FF control parameter.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ff_ctrl : FF Controller
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FF_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, T_IM_PRO_FF_CTRL* ff_ctrl );

/**
Setup of input position .
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	ff_area	:The area for OB detection<br>
					value range :pos_x[0 - 12287]<br>
					target registor :@@FFH<br>
					value range :pos_y[0 - 8191]<br>
					target registor :@@FFV<br>
					value range :width[4 - 12288] 4pixel boundary<br>
					target registor :@@FFHW<br>
					value range :lines[4 - 8192] 4pixel boundary<br>
					target registor :@@FFVW<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FF_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, T_IM_PRO_AREA_INFO* ff_area );

/**
FIR coefficient setting.
@param[in]	unit_no : Unit number.
@param[in]	ch : FFSH channel
@param[in]	lpf_ctrl :LPF Control information.<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_FF_Set_FIR_Filter( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FF_CH ch, T_IM_PRO_FF_LPF_CTRL* lpf_ctrl );


/*	@}*/	// im_pro_sro_ff


/** @addtogroup im_pro_sro_sdc SDC
@{
*/
/**
Compensation mode start
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not running NG
*/
extern	INT32 Im_PRO_SDC_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch );

/**
Compensation mode stop
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not stopped NG
*/
extern	INT32 Im_PRO_SDC_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, UCHAR force );

/**
SDC control setup 
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	sdc_ctrl : SDC control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SDC_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_SDC_CTRL* sdc_ctrl );


/**
Select SDC gain table.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	tbl_sel : SDC gain table select.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SDC_Select_Gain_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, E_IM_PRO_SDC_GAIN_TABLE_SEL tbl_sel );

/**
set SDC gain table.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	sdc_afpb_ctrl : SDC AFPB control information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SDC_Ctrl_Afpb( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_SDC_CTRL_AFPB* sdc_afpb_ctrl );

/**
set SDC gain table.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	tbl_sel : SDC gain table select.
@param[in]	gain_table : SDC gain table information.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SDC_Set_Gain_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, E_IM_PRO_SDC_GAIN_TABLE_SEL tbl_sel, T_IM_PRO_SDC_GAIN_TABLE* gain_table );

/**
A setup of enable access to the built-in RAM of SDC.
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_SDC_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, UCHAR paen_trg );

/*	@}*/	// im_pro_sro_sdc


/** @addtogroup im_pro_sro_dpc DPC
@{
*/

/**
Compensation mode start
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_DPC_Start( E_IM_PRO_UNIT_NUM unit_no );

/**
Compensation mode stop
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Processing OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Processing NG
*/
extern	INT32 Im_PRO_DPC_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force );

/**
DPC control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_ctrl : DPC control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_CTRL* dpc_ctrl );

/**
A setup of enable access to the built-in RAM of DPC.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_DPC_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg );

/**
DPC control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_area : DPC control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_AREA* dpc_area );

/**
Alpha blend control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_ab_ctrl : Alpha blend Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Alpha_Blend_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_AB_CTRL* dpc_ab_ctrl );

/**
Defect Dynamic Detection control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_ddd_ctrl : Defect Dynamic Detection Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Dynamic_Detect_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_DDD_CTRL* dpc_ddd_ctrl );

/**
Calculation of mis-determination degree control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_mdd_ctrl : Calculation of mis-determination degree for correction Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Mis_Determination_Calc_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_MDD_CTRL* dpc_mdd_ctrl );

/**
Pixel value magnitude defect correction control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_mdc_ctrl : Pixel value magnitude defect correction control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Magnitude_Defect_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_MDC_CTRL* dpc_mdc_ctrl );

/**
Edge storage defect correction control setup 
@param[in]	unit_no : Unit number.
@param[in]	dpc_esdc_ctrl : Edge storage defect correction Control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Edge_Storage_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_ESDC_CTRL* dpc_esdc_ctrl );

/**
Calculation of over-correction degree for correction control setup 	
@param[in]	unit_no : Unit number.
@param[in]	dpc_ocd_ctrl : Calculation of over-correction degree for correction control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DPC_Over_Correction_Detect_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_DPC_OCD_CTRL* dpc_ocd_ctrl );


/*	@}*/	// im_pro_sro_dpc


/** @addtogroup im_pro_common_prch PRCH
@{
*/

/**
PRch macro start.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch );

/**
PRch macro stop.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, UCHAR force );

/**
PRch macro  set AXI bus I/F Control
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	pr_ctrl : PRch control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_PRCH_CTRL* pr_ctrl );

/**
The control parameter of PRch is set.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	axi_ctrl : AXI control setting.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Ctrl_Axi( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_AXI_CTRL* axi_ctrl );


/**
The transmission start address of PRch is set up.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	pr_addr : PRch trans addr
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Set_Addr( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, ULONG pr_addr );

/**
The pixel size inputted into PRch is set up.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	pr_area : PRch Area information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Set_Area( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_PRCH_AREA* pr_area );

/**
Get AXI response for PRch
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	axi_response : AXI response value
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Get_AXI_Response( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, UCHAR* axi_response );

/*	@}*/

/** @addtogroup im_pro_common_m2p M2P
@{
*/

/**
M2P macro start.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : M2P channel<br>
	 			 value range:[0 - 4]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not running NG
*/
extern	INT32 Im_PRO_M2P_Start( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_M2P_CH ch );

/**
M2P macro stop.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : M2P channel<br>
	 			 value range:[0 - 4]<br>
@param[in]	force : force stop option
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: PRCH not stopped NG
*/
extern	INT32 Im_PRO_M2P_Stop( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_M2P_CH ch, UCHAR force );

/**
The control parameter of M2P is set.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : M2P channel<br>
	 			 value range:[0 - 4]<br>
@param[in]	m2p_ctrl : M2P control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_M2P_Ctrl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_M2P_CH ch, T_IM_PRO_M2P_CTRL* m2p_ctrl );

/**
A setup of enable access to the built-in RAM of M2P.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.<br>
	 			 value range:[0 - 4]<br>
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_M2P_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_M2P_CH ch, UCHAR paen_trg );

/**
The DEKNEE Table parameter of M2P compensation is set.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.<br>
	 			 value range:[0 - 4]<br>
@param[in]	m2p_tbl	:	M2P DEKNEE Table information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@retval		D_IM_PRO_MACRO_BUSY_NG		: Macro has not stopped.
*/
extern	INT32 Im_PRO_M2P_Set_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_M2P_CH ch, T_IM_PRO_M2P_TABLE* m2p_tbl );

/*	@}*/	// im_pro_common_m2p

/** @addtogroup im_pro_handler_sen SEN Interrupt
@{
*/

/**
SENTOP Interrupt setting
@param[in]	int_ctrl : SENTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_SENTOP_Interrupt_Ctrl( T_IM_PRO_INT_SENTOP_CTRL* int_ctrl );

/**
SG Interrupt setting
@param[in]	sg_int_ctrl : SG interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_SENTOP_SG_Interrupt_Ctrl( T_IM_PRO_INT_SG_CTRL* sg_int_ctrl );

/**
GYRO Interrupt setting
@param[in]	gyro_int_ctrl : GYRO interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_SENTOP_Gyro_Interrupt_Ctrl( T_IM_PRO_INT_GYRO_CTRL* gyro_int_ctrl );

/**
SENTOP macro interrupt delay setting
@param[in]	block_num : SENCORE block number(0:SENCORE0, 1:SENCORE1, 2:SENCORE2, 3:SENCORE3)
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1, 2:HD/VD2, 3:HD/VD3)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SENTOP_Set_VHD_Delay( UCHAR block_num, UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl );

/**
SENTOP macro GYRO interrupt delay setting
@param[in]	block_num : SENCORE block number(0:SENCORE0, 1:SENCORE1, 2:SENCORE2, 3:SENCORE3)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SENTOP_Set_Gyro_Int_Delay( UCHAR block_num, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl );

/**
SEN block VD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SEN_Set_VD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
SEN block SG VD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SEN_Set_SG_VD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
SEN block SG HD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SEN_Set_SG_HD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
SEN block Gyro Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SEN_Set_Gyro_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );


/**
SEN block HD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SEN_Set_HD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
SEN block Int handler except VD, HD, and Sensor I/F interrupts.
*/
extern	VOID Im_PRO_SEN_Int_Handler( VOID );
/**
SEN block VD Int handler.
*/
extern	VOID Im_PRO_SEN_VD_Int_Handler( VOID );
/**
SEN block HD Int handler.
*/
extern	VOID Im_PRO_SEN_HD_Int_Handler( VOID );
/**
SEN block Sensor I/F 0 Int handler.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_PRO_SEN_Sensor_If_Int_Handler_0( VOID );
/**
SEN block Sensor I/F 1 Int handler.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_PRO_SEN_Sensor_If_Int_Handler_1( VOID );
/**
SEN block Sensor I/F 2 Int handler.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_PRO_SEN_Sensor_If_Int_Handler_2( VOID );
/**
SEN block Sensor I/F 3 Int handler.
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern	VOID Im_PRO_SEN_Sensor_If_Int_Handler_3( VOID );

/**
Set SLVS-EC Common Interrupt flg
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@code
	//
	//sample code
	//

	T_IM_PRO_INT_CFG int_cfg;

	// for Lane0~3 error interrupt enabled setting
	int_cfg.interrupt_bit	= ( D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE0 | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE1 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE2 | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE3 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE0 | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE1 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE2 | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE3 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE0  | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE1  |
								D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE2  | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE3 );
	int_cfg.permission_flg	= 1;	// interrupt enabled

	Im_PRO_SLVS_Set_Interrupt( &int_cfg );


	// for Lane0~3 error interrupt disabled setting
	int_cfg.interrupt_bit	= ( D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE0 | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE1 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE2 | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE3 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE0 | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE1 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE2 | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE3 |
								D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE0  | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE1  |
								D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE2  | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE3 );
	int_cfg.permission_flg	= 0;	// interrupt disabled

	Im_PRO_SLVS_Set_Interrupt( &int_cfg );

@endcode
*/
extern	INT32 Im_PRO_SLVS_Set_Common_Interrupt( T_IM_PRO_INT_CFG* int_cfg );

/**
SLVS-EC Common Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SLVS_Set_Common_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
Set SLVS-EC Interrupt flg
@param[in]	stream_type : stream type of SLVS
@param[in]	int_type : interrupt type of SLVS
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@code
	//
	//sample code
	//

	T_IM_PRO_INT_CFG int_cfg;

	// INTn_O (Normal operation) interrupt enabled setting.
	int_cfg.interrupt_bit	= ( D_IM_PRO_SLVS_INTEN_FSI_BIT | D_IM_PRO_SLVS_INTEN_FSO_BIT |
								D_IM_PRO_SLVS_INTEN_FEI_BIT | D_IM_PRO_SLVS_INTEN_FEO_BIT |
								D_IM_PRO_SLVS_INTEN_RDY_BIT | D_IM_PRO_SLVS_INTEN_STBY_BIT );
	int_cfg.permission_flg	= 1;	// interrupt enabled

	Im_PRO_SLVS_Set_Interrupt( E_IM_PRO_SLVS_STREAM_TYPE_A, E_IM_PRO_SLVS_INT_TYPE_INTN_O, &int_cfg );

	// EINTn_O (PCS Error related) interrupt enabled setting.
	int_cfg.interrupt_bit	= ( D_IM_PRO_SLVS_LEINTEN_LBOVF_BIT | D_IM_PRO_SLVS_LEINTEN_LNE_BIT |
								D_IM_PRO_SLVS_LEINTEN_ECCE_BIT | D_IM_PRO_SLVS_LEINTEN_PCRCE_BIT | D_IM_PRO_SLVS_LEINTEN_LLE_BIT );
	int_cfg.permission_flg	= 1;	// interrupt enabled

	Im_PRO_SLVS_Set_Interrupt( E_IM_PRO_SLVS_STREAM_TYPE_A, E_IM_PRO_SLVS_INT_TYPE_PCS_ERR, &int_cfg );


	// INTn_O (Normal operation) interrupt disabled setting.
	int_cfg.interrupt_bit	= ( D_IM_PRO_SLVS_INTEN_FSI_BIT | D_IM_PRO_SLVS_INTEN_FSO_BIT |
								D_IM_PRO_SLVS_INTEN_FEI_BIT | D_IM_PRO_SLVS_INTEN_FEO_BIT |
								D_IM_PRO_SLVS_INTEN_RDY_BIT | D_IM_PRO_SLVS_INTEN_STBY_BIT );
	int_cfg.permission_flg	= 0;	// interrupt disabled

	Im_PRO_SLVS_Set_Interrupt( E_IM_PRO_SLVS_STREAM_TYPE_A, E_IM_PRO_SLVS_INT_TYPE_INTN_O, &int_cfg );

	// EINTn_O (PCS Error related) interrupt disabled setting.
	int_cfg.interrupt_bit	= ( D_IM_PRO_SLVS_LEINTEN_LBOVF_BIT | D_IM_PRO_SLVS_LEINTEN_LNE_BIT |
								D_IM_PRO_SLVS_LEINTEN_ECCE_BIT | D_IM_PRO_SLVS_LEINTEN_PCRCE_BIT | D_IM_PRO_SLVS_LEINTEN_LLE_BIT );
	int_cfg.permission_flg	= 0;	// interrupt disabled

	Im_PRO_SLVS_Set_Interrupt( E_IM_PRO_SLVS_STREAM_TYPE_A, E_IM_PRO_SLVS_INT_TYPE_PCS_ERR, &int_cfg );

@endcode
*/
extern	INT32 Im_PRO_SLVS_Set_Interrupt( E_IM_PRO_SLVS_STREAM_TYPE stream_type, E_IM_PRO_SLVS_INT_TYPE int_type, T_IM_PRO_INT_CFG* int_cfg );

/**
SLVS-EC Int handler setting.
@param[in]	stream_type : stream type of SLVS
@param[in]	int_type : interrupt type of SLVS
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SLVS_Set_Int_Handler( E_IM_PRO_SLVS_STREAM_TYPE stream_type, E_IM_PRO_SLVS_INT_TYPE int_type, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
LVDS Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@code
	//sample code
	USHORT interrupt_bit;
	
	interrupt_bit = (D_IM_PRO_LVDSINTENB_EOLE0|D_IM_PRO_LVDSINTENB_SOLE0|D_IM_PRO_LVDSINTENB_EOFE0|D_IM_PRO_LVDSINTENB_SOFE0);
	
	Im_PRO_LVDS_Set_Interrupt( interrupt_bit, 1 );
@endcode
*/
extern	INT32 Im_PRO_LVDS_Set_Interrupt( E_IM_PRO_LVDS_CH ch, T_IM_PRO_INT_LVDS_CTRL* int_cfg );

/**
CallBackFunction which Call to the interruption timing of LVDS is registered
@param[in]	ch : Channel No.
@param[in]	out_mode : select the callback index of Normal output mode or DOL1 or DOL2.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_LVDS_Set_Int_Handler( E_IM_PRO_LVDS_CH ch, E_IM_PRO_LVDS_OUTPUT_MODE out_mode, T_IM_PRO_CALLBACK_CFG* callback_cfg );


/**
DMIPI Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@code
	//sample code
	USHORT interrupt_bit;
	
	interrupt_bit = (D_IM_PRO_DMIPIINTENB_EOLE|D_IM_PRO_DMIPIINTENB_SOLE|D_IM_PRO_DMIPIINTENB_EOFE|D_IM_PRO_DMIPIINTENB_SOFE);
	
	Im_PRO_DMIPI_Set_Interrupt( interrupt_bit, 1 );
@endcode
*/
extern	INT32 Im_PRO_DMIPI_Set_Interrupt( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of DMIPI is registered
@param[in]	ch : Channel No.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_DMIPI_Set_Int_Handler( E_IM_PRO_DMIPI_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );


/**
LDIV Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_LDIV_Set_Interrupt( E_IM_PRO_LDIV_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of LDIV is registered
@param[in]	ch : Channel No.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_LDIV_Set_Int_Handler( E_IM_PRO_LDIV_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
OBT Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBT_Set_Interrupt( E_IM_PRO_OBT_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of OBT is registered
@param[in]	ch : Channel No.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBT_Set_Int_Handler( E_IM_PRO_OBT_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
OBD Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBD_Set_Interrupt( E_IM_PRO_OBD_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of OB is registered
@param[in]	ch : Channel No.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_OBD_Set_Int_Handler( E_IM_PRO_OBD_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
MONI Macro interrupt permission set
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_MONI_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of MONI is registered
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: channel
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_MONI_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_MONI_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/*	@}*/	// im_pro_handler_sen


/** @addtogroup im_pro_handler_sro SRO Interrupt
@{
*/

/**
SROTOP Interrupt setting
@param[in]	unit_no : Unit number.
@param[in]	int_ctrl : SROTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_SROTOP_Interrupt_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_INT_SROTOP_CTRL* int_ctrl );

/**
SROTOP macro interrupt delay setting
@param[in]	unit_no : Unit number.
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SROTOP_Set_VHD_Delay( E_IM_PRO_UNIT_NUM unit_no, UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl );


/**
SRO pipe VD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_SROTOP_Pipe_Set_VD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
SRO pipe HD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_SROTOP_Pipe_Set_HD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
SRO Macro Pipe1 Int handler.
*/
extern	VOID Im_PRO_SROTOP_Pipe1_Int_Handler( VOID );
/**
SRO Macro Pipe1 VD Int handler.
*/
extern	VOID Im_PRO_SROTOP_Pipe1_VD_Int_Handler( VOID );
/**
SRO Macro Pipe1 HD Int handler.
*/
extern	VOID Im_PRO_SROTOP_Pipe1_HD_Int_Handler( VOID );
/**
SRO Macro Pipe2 Int handler.
*/
extern	VOID Im_PRO_SROTOP_Pipe2_Int_Handler( VOID );
/**
SRO Macro Pipe2 VD Int handler.
*/
extern	VOID Im_PRO_SROTOP_Pipe2_VD_Int_Handler( VOID );
/**
SRO Macro Pipe2 HD Int handler.
*/
extern	VOID Im_PRO_SROTOP_Pipe2_HD_Int_Handler( VOID );

/**
Set SDC Interrupt flg
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel<br>
	 			 value range:[0 - 1]<br>
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK				:setting ok
@retval		D_IM_PRO_PARAM_ERROR	:setting ng
*/
extern	INT32 Im_PRO_SDC_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of SDC is registered
@param[in]	unit_no : Unit number.
@param[in]	ch : SDC channel<br>
	 			 value range:[0 - 1]<br>
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_NCH_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_SDC_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );


/*	@}*/	// im_pro_handler_sro

/** @addtogroup im_pro_handler_b2b B2B Interrupt
@{
*/

/**
B2BTOP Interrupt setting
@param[in]	unit_no : Unit number.
@param[in]	int_ctrl : B2BTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_B2BTOP_Interrupt_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_INT_B2BTOP_CTRL* int_ctrl );

/**
B2BTOP macro interrupt delay setting
@param[in]	unit_no : Unit number.
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_B2BTOP_Set_VHD_Delay( E_IM_PRO_UNIT_NUM unit_no, UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl );

/**
B2B pipe VD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_B2BTOP_Pipe_Set_VD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
B2B pipe HD Int handler setting.
@param[in]	unit_no : Unit number.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_B2BTOP_Pipe_Set_HD_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
B2B Macro Pipe1 Int handler.
*/
extern	VOID Im_PRO_B2BTOP_Pipe1_Int_Handler( VOID );
/**
B2B Macro Pipe1 VD Int handler.
*/
extern	VOID Im_PRO_B2BTOP_Pipe1_VD_Int_Handler( VOID );
/**
B2B Macro Pipe1 HD Int handler.
*/
extern	VOID Im_PRO_B2BTOP_Pipe1_HD_Int_Handler( VOID );
/**
B2B Macro Pipe2 Int handler.
*/
extern	VOID Im_PRO_B2BTOP_Pipe2_Int_Handler( VOID );
/**
B2B Macro Pipe2 VD Int handler.
*/
extern	VOID Im_PRO_B2BTOP_Pipe2_VD_Int_Handler( VOID );
/**
B2B Macro Pipe2 HD Int handler.
*/
extern	VOID Im_PRO_B2BTOP_Pipe2_HD_Int_Handler( VOID );

/*	@}*/	// im_pro_handler_b2b


/** @addtogroup im_pro_handler_stat STAT Interrupt
@{
*/
/**
STATTOP Interrupt setting
@param[in]	int_ctrl : STATTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_STATTOP_Interrupt_Ctrl( T_IM_PRO_INT_STATTOP_CTRL* int_ctrl );

/**
STAT Macro Int handler.
*/
extern	VOID Im_PRO_STAT_Int_Handler( VOID );

/**
AEAWB Macro interrupt permission set
@param[in]	ch : channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AEAWB_Set_Interrupt( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of AEAWB is registered
@param[in]	ch : channel
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_AEAWB_Set_Int_Handler( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
AF Macro interrupt permission set
@param[in]	ch : channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Set_Interrupt( E_IM_PRO_AF_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of AF is registered
@param[in]	ch : channel
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_AF_Set_Int_Handler( E_IM_PRO_AF_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
HIST Macro interrupt permission set
@param[in]	ch : channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Set_Interrupt( E_IM_PRO_HIST_CH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of HIST is registered
@param[in]	ch : channel
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_HIST_Set_Int_Handler( E_IM_PRO_HIST_CH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/*	@}*/	// im_pro_handler_stat


/** @addtogroup im_pro_handler_pas PAS Interrupt
@{
*/
/**
PASTOP Interrupt setting
@param[in]	int_ctrl : PASTOP interrupt control information structure
@retval		D_DDIM_OK					:setting ok
@retval		D_IM_PRO_PARAM_ERROR		:setting ng
*/
extern	INT32 Im_PRO_PASTOP_Interrupt_Ctrl( T_IM_PRO_INT_PASTOP_CTRL* int_ctrl );

/**
PASTOP macro interrupt delay setting
@param[in]	ch : channel(0:HD/VD0, 1:HD/VD1)
@param[in]	delay_ctrl : delay setting
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PASTOP_Set_VHD_Delay( UCHAR ch, T_IM_PRO_INT_VHD_DELAY_CTRL* delay_ctrl );

/**
PAS block VD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PASTOP_Set_VD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
PAS block HD Int handler setting.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PASTOP_Set_HD_Int_Handler( T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
PAS Macro Int handler.
*/
extern	VOID Im_PRO_PASTOP_Int_Handler( VOID );
/**
PAS Macro VD Int handler.
*/
extern	VOID Im_PRO_PASTOP_VD_Int_Handler( VOID );
/**
PAS Macro HD Int handler.
*/
extern	VOID Im_PRO_PASTOP_HD_Int_Handler( VOID );

/*	@}*/	// im_pro_handler_pas


/** @addtogroup im_pro_handler_common	Common Interrupt
@{
*/
/**
PWch Macro interrupt permission set
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of PWch is registered
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch			: PWch number
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PWch_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PWCH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );

/**
The permission setting of the PRch interrupt control is done.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	int_cfg : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Set_Interrupt( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_INT_CFG* int_cfg );

/**
CallBackFunction which Call to the interruption timing of PRch is registered
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : PRch channel
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_PRch_Set_Int_Handler( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PRCH ch, T_IM_PRO_CALLBACK_CFG* callback_cfg );


/*	@}*/	// im_pro_handler_common

/**
CMIPI Macro interrupt permission set
@param[in]	ch : Channel No.
@param[in]	cmipi_int_ctrl : interrupt configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@code
	//sample code
	T_IM_PRO_INT_CMIPI_CTRL cmipi_int_ctrl;
	
	cmipi_int_ctrl.cmipi_int1_enable.interrupt_bit	= (D_IM_PRO_MPIINTF_FEO0|D_IM_PRO_MPIINTF_FEI0);
	cmipi_int_ctrl.cmipi_int1_enable.permission_flg	= 1;
	cmipi_int_ctrl.cmipi_int2_enable.interrupt_bit	= (D_IM_PRO_MPICINTE2_CICHSE0|D_IM_PRO_MPICINTF2_CICHSF1|D_IM_PRO_MPICINTF2_CICHSF2|D_IM_PRO_MPICINTF2_PHCRCAEF);
	cmipi_int_ctrl.cmipi_int2_enable.permission_flg	= 1;

	Im_PRO_CMIPI_Set_Interrupt( E_IM_PRO_CMIPI_CH0, &cmipi_int_ctrl );
@endcode
*/
extern	INT32 Im_PRO_CMIPI_Set_Interrupt( E_IM_PRO_CMIPI_CH ch, T_IM_PRO_INT_CMIPI_CTRL* cmipi_int_ctrl );

/**
CallBackFunction which Call to the interruption timing of CMIPI is registered
@param[in]	ch : Channel No.
@param[in]	category : interrupt factor category.
@param[in]	callback_cfg : callback configuration
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
extern	INT32 Im_PRO_CMIPI_Set_Int_Handler( E_IM_PRO_CMIPI_CH ch, E_IM_PRO_CMIPI_INT_CATE category, T_IM_PRO_CALLBACK_CFG* callback_cfg );



/**
Get the top address of the address array of LNR control.
@param[in]		unit_no : Unit number.
@param[in]		block_type	 : PRO block type
@param[in]		ch : Channel No.
@param[out]		addr					: Top address of the address array of LNR control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_LNR_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_LNR_CH ch, const T_IM_PRO_RDMA_LNR_ADDR** addr );

/**
Get the top address of the address array of OBT control.
@param[in]		ch : Channel No.
@param[out]		addr					: Top address of the address array of OBT control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_OBT_Cntl( E_IM_PRO_OBT_CH ch, const T_IM_PRO_RDMA_OBT_ADDR** addr );

/**
Get the top address of the address array of FFSH control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of FFSH control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_FFSH_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FFSH_CH ch, const T_IM_PRO_RDMA_FFSH_ADDR** addr );

/**
Get the top address of the address array of SDC control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of SDC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_SDC_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, const T_IM_PRO_RDMA_SDC_ADDR** addr );

/**
Get the top address of the address array of DPC control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of SDC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_DPC_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_DPC_ADDR** addr );

/**
Get the top address of the address array of FSHDL control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of FSHDL control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_FSHDL_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHDL_CH ch, const T_IM_PRO_RDMA_FSHDL_ADDR** addr );

/**
Get the top address of the address array of FFC control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of FFC control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_FFC_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_FFC_ADDR** addr );

/**
Get the top address of the address array of PGAIN control.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	select		: select PGAIN 0/1/2.
@param[out]		addr					: Top address of the address array of PGAIN control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_PGAIN_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_PGAIN_SELECT select, const T_IM_PRO_RDMA_PGAIN_ADDR** addr );

/**
Get the top address of the address array of PZSFT control.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[out]		addr					: Top address of the address array of PZSFT control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_PZSFT_Cntl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_PZSFT_CH ch, const T_IM_PRO_RDMA_PZSFT_ADDR** addr );

/**
Get the top address of the address array of CAG control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of CAG control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_CAG_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_CAG_ADDR** addr );

/**
Get the top address of the address array of ELF control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of ELF control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_ELF_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_ELF_ADDR** addr );

/**
Get the top address of the address array of FSHD control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of FSHD control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_FSHD_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_FSHD_ADDR** addr );

/**
Get the top address of the address array of P2M knee table.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[out]	addr					: Top address of the address array of P2M knee table.
@retval		D_DDIM_OK				: success.
@retval		D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_P2M_Knee_Tbl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, const T_IM_PRO_RDMA_P2M_KNEE_TBL_ADDR** addr );

/**
Get the top address of the address array of M2P deknee table.
@param[in]	unit_no : Unit number.
@param[in]	block_type	 : PRO block type
@param[in]	ch : Channel No.
@param[out]	addr					: Top address of the address array of M2P deknee table.
@retval		D_DDIM_OK				: success.
@retval		D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_M2P_Deknee_Tbl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_BLOCK_TYPE block_type, E_IM_PRO_P2M_CH ch, const T_IM_PRO_RDMA_M2P_DEKNEE_TBL_ADDR** addr );

/**
Get the top address of the address array of SDC gain table.
@param[in]		unit_no : Unit number.
@param[in]		ch : Channel No
@param[in]		tbl_sel : SDC gain table select.
@param[out]		addr					: Top address of the address array of SDC gain table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_SDC_Gain_Tbl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_SDC_CH ch, E_IM_PRO_SDC_GAIN_TABLE_SEL tbl_sel, const T_IM_PRO_RDMA_SDC_GAIN_TBL_ADDR** addr );

/**
Get the top address of the address array of FSHD control.
@param[in]		unit_no : Unit number.
@param[in]		tbl_sel : table select.
@param[out]		addr					: Top address of the address array of FSHD Concentric Circles Correction table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_FSHD_Ctbl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHD_LUT_SEL tbl_sel, const T_IM_PRO_RDMA_FSHD_CTBL_ADDR** addr );

/**
Get the top address of the address array of FSHD Concentric Circles Correction table.
@param[in]		tbl_sel					: table select.
@param[out]		addr					: Top address of the address array of FSHD Concentric Circles Correction table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	INT32 Im_PRO_Get_RdmaAddr_SPT_SHD_Tbl( E_IM_PRO_SPT_TBL_SEL tbl_sel, const T_IM_PRO_RDMA_SPT_SHD_TBL_ADDR** addr );

/** @addtogroup im_pro_common Common unit
@{
*/

/**
Get the Macro core clock information.
@param[in]		unit_no			: Unit number.
@param[in]		block_type		: PRO block type
@param[out]		macro_clk_hz	: macro clock number.(e.g. 300MHz: return 300,000,000)
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
extern	VOID Im_PRO_Get_Current_Clk_Hz( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_CLK_BLOCK_TYPE block_type, UINT32* macro_clk_hz );
/*	@}*/	// im_pro_common


#ifdef __cplusplus
	}
#endif

#endif	// _IM_PRO_H_

