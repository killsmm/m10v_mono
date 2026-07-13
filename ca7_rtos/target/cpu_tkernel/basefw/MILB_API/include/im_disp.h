/**
 * @file		im_disp.h
 * @brief		LCD, HDMI, MIPI display process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/**
@weakgroup im_disp
@{

<hr>

@section im_disp_sequence Sequence Chart
- @ref Disp_Start_1
- @ref Disp_Start_2
- @ref Disp_Stop_1
- @ref Disp_Stop_2
- @ref Disp_Stop_3
- @ref Disp_Stop_4
- @ref Disp_Stop_5
- @ref Disp_Int
- @ref Disp_Face_Frame


@section Disp_Start_1					Initialize and start driver.
@image html im_disp_start.png

@section Disp_Start_2					The specified layer is started.
@image html im_disp_start_specified_layer.png

@section Disp_Stop_1					All layer stop (no wait).
@image html im_disp_stop_all_stop_no_wait.png

@section Disp_Stop_2					All layer stop (wait, progressive).
@image html im_disp_stop_all_stop_wait_prog.png

@section Disp_Stop_3					All layer stop (wait, interlace).
@image html im_disp_stop_all_stop_wait_int.png

@section Disp_Stop_4					The specified layer is stoped (no wait).
@image html im_disp_stop_specified_layer_no_wait.png

@section Disp_Stop_5					The specified layer is stoped (wait).
@image html im_disp_stop_specified_layer_wait.png

@section Disp_Int						Set interrupt.
@image html im_disp_set_int.png

@section Disp_Face_Frame				Data read and setting (ex. face frame).
@image html im_disp_face_frame.png

@}
*/
/**
@addtogroup im_disp
@{
*/

#ifndef _IM_DISP_H_
#define _IM_DISP_H_

#include "driver_common.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define D_DD_TOP_PCKDISPSTP_BIT	(0x00002000)	/**< PCLK_DISP */
#define D_DD_TOP_HCKDISPSTP_BIT	(0x00004000)	/**< HCLK_DISP */
// Return error as parameter error
#define	D_IM_DISP_INPUT_PARAM_ERROR					(D_IM_DISP | D_DDIM_INPUT_PARAM_ERROR)	/**< Processing result code.<br> Input parameter error.	*/
#define D_IM_DISP_MACRO_BUSY_NG						(D_IM_DISP | D_DDIM_MACRO_BUSY_NG)		/**< Processing result code.<br> Macro busy.			*/
#define D_IM_DISP_TIMEOUT							(D_IM_DISP | D_DDIM_TIMEOUT)			/**< Processing result code.<br> Time out error			*/
#define D_IM_DISP_STATUS_ABNORMAL					(D_IM_DISP | D_DDIM_STATUS_ABNORMAL)	/**< Processing result code.<br> Abnormal Status		*/
#define D_IM_DISP_ACCESS_ERR						(D_IM_DISP | D_DDIM_ACCESS_ERR)			/**< Processing result code.<br> TRG cannot be set up.	*/

#define	D_IM_DISP_BLOCK_NUM							(2)		/**< Common block number.					*/
#define	D_IM_DISP_DWCH_CHANNEL_NUM					(2)		/**< DWCH(PWCH) channel number				*/
#define	D_IM_DISP_MAIN_ADDR_BANK_SIZE				(4)		/**< Bank size of main image address.		*/
#define	D_IM_DISP_MATRIX_SIZE						(3)		/**< Size of matrix coefficient.			*/
#define	D_IM_DISP_COLOR_BAR_COUNT					(16)	/**< Maximum number of color bars.			*/
#define	D_IM_DISP_FACE_FRAME_COUNT					(42)	/**< Maximum number of face frame.			*/
#define	D_IM_DISP_OSD_DISPLAY_AREA_COUNT			(10)	/**< Maximum number of OSD display area.	*/
#define	D_IM_DISP_OSD_ADDR_0_BANK_SIZE				(4)		/**< Bank size of main image address 0.		*/
#define	D_IM_DISP_OSD_ADDR_BANK_SIZE				(9)		/**< Bank size of main image address.		*/
#define	D_IM_DISP_GRBLINK_SIZE						(5)		/**< Word size of GRBLINK.					*/

// General-purpose definition of register value (Effectiveness selection)
#define D_IM_DISP_ENABLE_OFF						(0)		/**< OFF	*/
#define D_IM_DISP_ENABLE_ON							(1)		/**< ON		*/

// General-purpose definition of register value (Polarity selection)
#define D_IM_DISP_POLARITY_POSITIVE					(0)		/**< Positive polarity.	*/
#define D_IM_DISP_POLARITY_NEGATIVE					(1)		/**< Negative polarity.	*/

// General-purpose definition of register value (Soft reset)
#define D_IM_DISP_SR_CANCEL							(0)		/**< Soft reset cancel.	*/
#define D_IM_DISP_SR_RESET							(1)		/**< Soft reset.		*/

// General-purpose definition of register value (Table surface)
#define D_IM_DISP_TABLE_SURFACE_A					(0)		/**< Table A surface.	*/
#define D_IM_DISP_TABLE_SURFACE_B					(1)		/**< Table B surface.	*/

// General-purpose definition of register value (IP selection)
#define D_IM_DISP_IP_HDMI							(1)		/**< HDMI IP.		*/
#define D_IM_DISP_IP_MIPI							(2)		/**< MIPI-DSI IP.	*/

// General-purpose definition of register value (Video format selection)
#define D_IM_DISP_FORMAT_NORMAL						(0)		/**< Normal format.	*/
#define D_IM_DISP_FORMAT_VIDEO						(1)		/**< Video format.	*/

// LIDT0 Register - IFMT (Input format selection)
#define D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT			(0)		/**< LIDT.IFMT Register<br> YCbCr422 respect point 8bit format.			*/
#define D_IM_DISP_LIDT_IFMT_YCC422_RP_12BIT_PACK	(1)		/**< LIDT.IFMT Register<br> YCbCr422 respect point 12bit format.		*/
#define D_IM_DISP_LIDT_IFMT_YCC422_RP_16BIT			(2)		/**< LIDT.IFMT Register<br> YCbCr422 respect point 16bit format.		*/
#define D_IM_DISP_LIDT_IFMT_YCC422_RP_10BIT_PACK	(3)		/**< LIDT.IFMT Register<br> YCbCr422 respect point 10bit format.		*/
#define D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT			(4)		/**< LIDT.IFMT Register<br> YCbCr420 respect point 8bit format.			*/
#define D_IM_DISP_LIDT_IFMT_YCC420_RP_10BIT_PACK	(7)		/**< LIDT.IFMT Register<br> YCbCr420 respect point 10bit format.		*/
#define D_IM_DISP_LIDT_IFMT_GENERAL_8BIT			(8)		/**< LIDT.IFMT Register<br> General purpose 8bit input data.			*/
#define D_IM_DISP_LIDT_IFMT_GENERAL_16BIT			(10)	/**< LIDT.IFMT Register<br> General purpose 16bit input data.			*/
#define D_IM_DISP_LIDT_IFMT_GENERAL_10BIT			(11)	/**< LIDT.IFMT Register<br> General purpose 10bit input data.			*/
#define D_IM_DISP_LIDT_IFMT_GENERAL_10BITx2			(27)	/**< LIDT.IFMT Register<br> General purpose 10bit input data (10bitx2).	*/

// LIDT0 Register - NBT (Burst size selection)
// GRIDT Register - NBT (Burst size selection)
#define D_IM_DISP_NBT_128BYTE						(0)		/**< LIDT.NBT Register<br> 128 byte forwarding. */
#define D_IM_DISP_NBT_256BYTE						(1)		/**< LIDT.NBT Register<br> 256 byte forwarding. */
#define D_IM_DISP_NBT_512BYTE						(2)		/**< LIDT.NBT Register<br> 512 byte forwarding. */

// LIDT0 Register - IFEW (Input format effective bit width selection)
#define D_IM_DISP_IFEW_16BIT						(0)		/**< LIDT.IFEW Register<br> 16bit.	*/
#define D_IM_DISP_IFEW_14BIT						(1)		/**< LIDT.IFEW Register<br> 14bit.	*/
#define D_IM_DISP_IFEW_12BIT						(2)		/**< LIDT.IFEW Register<br> 12bit.	*/
#define D_IM_DISP_IFEW_10BIT						(3)		/**< LIDT.IFEW Register<br> 10bit.	*/

// LIDT1 Register - YSLVSL, CSLVSL (Master interface selection)
#define D_IM_DISP_SLVSL_MASTER_INTERFACE_0			(0)		/**< LIDT.YWHMD, LIDT.CSLVSL Register<br> Master interface 0.	*/
#define D_IM_DISP_SLVSL_MASTER_INTERFACE_1			(1)		/**< LIDT.YWHMD, LIDT.CSLVSL Register<br> Master interface 1.	*/

// LBLTMR Register - BTIMD (Warning display period mode)
#define D_IM_DISP_BTIMD_NORMAL						(0)		/**< LBLTMR.BTIMD Register<br> Normal mode (Always display).	*/
#define D_IM_DISP_BTIMD_BLINK						(1)		/**< LBLTMR.BTIMD Register<br> Blink mode.						*/

// LBOST Register - BOMD (Warning display operation mode)
#define D_IM_DISP_BOMD_MASTER						(0)		/**< LBOST.BOMD Register<br> Master.	*/
#define D_IM_DISP_BOMD_SLAVE						(1)		/**< LBOST.BOMD Register<br> Slave.		*/

// LGMMD Register - GMMD (Gamma correct mode)
#define D_IM_DISP_GMMD_EQUAL_INTERVAL_MODE			(0)		/**< LGMMD.GMMD Register<br> Equal interval mode.		*/
#define D_IM_DISP_GMMD_UNEQUAL_INTERVALS_MODE		(1)		/**< LGMMD.GMMD Register<br> Unequal intervals mode.	*/

// LREVDISP Register - HREV (Horizontal reversing display selection)
// LREVDISP Register - VREV	(Vertical reversing display selection)
#define D_IM_DISP_REV_NORMAL						(0)		/**< LREVDISP.HREV, LREVDISP.VREV Register<br> Normal display.		*/
#define D_IM_DISP_REV_REVERSE						(1)		/**< LREVDISP.HREV, LREVDISP.VREV Register<br> Reversing display.	*/

// TOCTL Register - CON (Connection destination selection)
#define D_IM_DISP_CON_EXTERNAL						(0)		/**< TOCTL.CON Register<br> Connects relay terminal of the chip.	*/
#define D_IM_DISP_CON_INTERNAL						(1)		/**< TOCTL.CON Register<br> Connects other internal blocks.			*/

// IFS Register - IFS (Display I/F selection)*/
#define D_IM_DISP_IFS_NONE							(0)		/**< IFS.IFS Register<br> Not select.	*/
#define D_IM_DISP_IFS_LCD							(1)		/**< IFS.IFS Register<br> D-LCD I/F.	*/
#define D_IM_DISP_IFS_HDMI							(2)		/**< IFS.IFS Register<br> HDMI.			*/
#define D_IM_DISP_IFS_MIPI							(3)		/**< IFS.IFS Register<br> MIPI-DSI.		*/

// INTC Register - VFTMG (Vsync detection interruption flag generation timing)
#define D_IM_DISP_VFTMG_NONE						(0)		/**< INTC.VFTMG Register<br> Flag is not generated.												*/
#define D_IM_DISP_VFTMG_1ST							(1)		/**< INTC.VFTMG Register<br> Flag is generated detecting the Vsync of the 1st field.			*/
#define D_IM_DISP_VFTMG_2ND							(2)		/**< INTC.VFTMG Register<br> Flag is generated detecting the Vsync of the 2nd field.			*/
#define D_IM_DISP_VFTMG_1ST_2ND						(3)		/**< INTC.VFTMG Register<br> Flag is generated detecting the Vsync of the 1st and 2nd field.	*/

// POLSEL Register - CKEG (Output clock edge selection)
#define D_IM_DISP_CKEG_FALL							(0)		/**< POLSEL.CKEG Register<br> Clock standing fall for external output. */
#define D_IM_DISP_CKEG_RISE							(1)		/**< POLSEL.CKEG Register<br> Clock standing up for external output. */

// TSL Register - TSL (Transmission mode)
#define D_IM_DISP_TSL_PROGRESSIVE					(0)		/**< TSL.TSL: Progressive.	*/
#define D_IM_DISP_TSL_INTERLACE						(1)		/**< TSL.TSL: Interlace.	*/

// VCYC Register - VDSFT (Vsync output 0.5H shift selection)
#define D_IM_DISP_VDSFT_NORMAL						(0)		/**< VCYC.VDSFT Register<br> Normal output.		*/
#define D_IM_DISP_VDSFT_SHIFT						(1)		/**< VCYC.VDSFT Register<br> 0.5H shift output.	*/

// OVPW Register - OVPWU (Vsync pulse width unit selection)
#define D_IM_DISP_OVPWU_CLOCK_UNIT					(0)		/**< OVPW.OVPWU Register<br> Output clock unit.	*/
#define D_IM_DISP_OVPWU_LINE_UNIT					(1)		/**< OVPW.OVPWU Register<br> Line count unit.	*/

// VRFCTL Register - VENS (Vertical enable signal assert timing selection)
#define D_IM_DISP_VENS_H_ENABLE						(0)		/**< VRFCTL.VENS Register<br> Horizontal enable signal.	*/
#define D_IM_DISP_VENS_HSYNC						(1)		/**< VRFCTL.VENS Register<br> Hsync.					*/

// HRFCTL Register - HENS (Selection of operation of the horizontal enable signal for period of vertical blank)
#define D_IM_DISP_HENS_ASSERT_ON					(0)		/**< HRFCTL.HENS Register<br> Asserted for the period of a vertical blank.		*/
#define D_IM_DISP_HENS_ASSERT_OFF					(1)		/**< HRFCTL.HENS Register<br> Not asserted for the period of a vertical blank.	*/

// DOMD Register - MODE (Output mode)
#define D_IM_DISP_MODE_RGB888_24BIT					(0x00)	/**< DOMD.MODE Register<br> RGB888 24bit									*/
#define D_IM_DISP_MODE_RGB888_30BIT					(0x01)	/**< DOMD.MODE Register<br> RGB888 30bit(10bit x3)							*/
#define D_IM_DISP_MODE_RGB888_36BIT					(0x02)	/**< DOMD.MODE Register<br> RGB888 36bit(12bit x3)							*/
#define D_IM_DISP_MODE_RGB888_8BIT_1				(0x04)	/**< DOMD.MODE Register<br> RGB888 8bit 1 (R0->G1->B2->R3->G4->B5->...)		*/
#define D_IM_DISP_MODE_RGB888_8BIT_2				(0x05)	/**< DOMD.MODE Register<br> RGB888 8bit 2 (R0->G0->B0->R1->G1->B1->...)		*/
#define D_IM_DISP_MODE_RGB888_8BIT_3				(0x06)	/**< DOMD.MODE Register<br> RGB888 8bit 3 (RGB-dummy)						*/
#define D_IM_DISP_MODE_RGB888_16BIT_1				(0x08)	/**< DOMD.MODE Register<br> RGB888 16bit 1 (R0G0->B0R1->G1B1->R2G2->...)	*/
#define D_IM_DISP_MODE_RGB888_16BIT_2				(0x09)	/**< DOMD.MODE Register<br> RGB888 16bit 2 (RGB-dummy)						*/
#define D_IM_DISP_MODE_RGB565_8BIT					(0x0C)	/**< DOMD.MODE Register<br> RGB565 8bit										*/
#define D_IM_DISP_MODE_RGB565_16BIT					(0x0D)	/**< DOMD.MODE Register<br> RGB565 16bit									*/
#define D_IM_DISP_MODE_RGB666_18BIT					(0x0E)	/**< DOMD.MODE Register<br> RGB666 18bit									*/
#define D_IM_DISP_MODE_YCC422_8BIT					(0x40)	/**< DOMD.MODE Register<br> YCbCr422 8bit									*/
#define D_IM_DISP_MODE_YCC422_16BIT					(0x41)	/**< DOMD.MODE Register<br> YCbCr422 16bit									*/
#define D_IM_DISP_MODE_YCC444_24BIT					(0x42)	/**< DOMD.MODE Register<br> YCbCr444 24bit									*/
#define D_IM_DISP_MODE_YCC422_20BIT					(0x43)	/**< DOMD.MODE Register<br> YCbCr422 20bit(10bit x2)						*/
#define D_IM_DISP_MODE_YCC422_24BIT					(0x44)	/**< DOMD.MODE Register<br> YCbCr422 24bit(10bit x2)						*/
#define D_IM_DISP_MODE_DEEP_YCC444_30BIT			(0x50)	/**< DOMD.MODE Register<br> Deep Color YCbCr444 30bit(10bit x3)				*/
#define D_IM_DISP_MODE_DEEP_YCC444_36BIT			(0x51)	/**< DOMD.MODE Register<br> Deep Color YCbCr444 36bit(12bit x3)				*/
#define D_IM_DISP_MODE_DUAL_YCC444_12BIT			(0x60)	/**< DOMD.MODE Register<br> Dual Edge YCbCr444 12bit						*/
#define D_IM_DISP_MODE_YCC420_24BIT					(0x70)	/**< DOMD.MODE Register<br> YCbCr420 24bit									*/
#define D_IM_DISP_MODE_YCC420_30BIT					(0x71)	/**< DOMD.MODE Register<br> YCbCr420 30bit(10bit x3)						*/
#define D_IM_DISP_MODE_GENERAL_8BIT					(0xF0)	/**< DOMD.MODE Register<br> General purpose 8bit							*/
#define D_IM_DISP_MODE_GENERAL_16BIT				(0xF1)	/**< DOMD.MODE Register<br> General purpose 16bit							*/
#define D_IM_DISP_MODE_GENERAL_10BIT_1				(0xF2)	/**< DOMD.MODE Register<br> General purpose 10bit 1							*/
#define D_IM_DISP_MODE_GENERAL_10BIT_2				(0xFA)	/**< DOMD.MODE Register<br> General purpose 10bit 2 (10bit x2)				*/

// DOMD Register - OBPSL (Output bit position selection)
#define D_IM_DISP_OBPSL_LSB							(0)		/**< DOMD.OBPSL Register<br> Outputs by LSB stuff.	*/
#define D_IM_DISP_OBPSL_MSB							(1)		/**< DOMD.OBPSL Register<br> Outputs by MSB stuff.	*/

// DOMD Register - OBUSL (Output bit unit selection)
#define D_IM_DISP_OBUSL_8BIT						(0)		/**< DOMD.OBUSL Register<br> Outputs by 8bit unit.	*/
#define D_IM_DISP_OBUSL_12BIT						(2)		/**< DOMD.OBUSL Register<br> Outputs by 12bit unit.	*/
#define D_IM_DISP_OBUSL_16BIT						(3)		/**< DOMD.OBUSL Register<br> Outputs by 16bit unit.	*/

// BLDCTL Register - BLD1~4 (Blend order layer selection)
#define D_IM_DISP_BLD_GRID							(0)		/**< BLDCTL.BLD1~4 Register<br> Grid layer			*/
#define D_IM_DISP_BLD_FACE							(1)		/**< BLDCTL.BLD1~4 Register<br> Face frame layer	*/
#define D_IM_DISP_BLD_OSD0							(2)		/**< BLDCTL.BLD1~4 Register<br> OSD data 0 layer	*/
#define D_IM_DISP_BLD_OSD1							(3)		/**< BLDCTL.BLD1~4 Register<br> OSD data 1 layer	*/

// R2RCTL Register - R2REN (RGB->RGB matrix enable)
#define D_IM_DISP_R2REN_DISABLE						(0)		/**< R2RCTL.R2REN Register<br> RGB->RGB matrix disable	*/
#define D_IM_DISP_R2REN_ENABLE						(1)		/**< R2RCTL.R2REN Register<br> RGB->RGB matrix enable	*/

// YCTL Register - YLPF (Y data LPF)
#define D_IM_DISP_YLPF_DISABLE						(0)		/**< YCTL.YLPF Register<br> Y data LPF disable	*/
#define D_IM_DISP_YLPF_ENABLE						(1)		/**< YCTL.YLPF Register<br> Y data LPF enable	*/

// DOCTL0 Register - DTO1~6 (Data output order selection)
#define D_IM_DISP_DTO_R_Y0							(0)		/**< DOCTL0.DTO1~6 Register<br> R (or Y0)	*/
#define D_IM_DISP_DTO_G_CB							(1)		/**< DOCTL0.DTO1~6 Register<br> G (or CB)	*/
#define D_IM_DISP_DTO_B_CR							(2)		/**< DOCTL0.DTO1~6 Register<br> B (or CR)	*/
#define D_IM_DISP_DTO_0_Y1							(3)		/**< DOCTL0.DTO1~6 Register<br> 00h (or Y1)	*/

// DOCTL2 Register - CLVS (CbCr data output level selection)
#define D_IM_DISP_CLVS_NORMAL						(0)		/**< DOCTL2.CLVS Register<br> Normal output	*/
#define D_IM_DISP_CLVS_SIGNED						(1)		/**< DOCTL2.CLVS Register<br> Signed output	*/

// DOCTL2 Register - CTOH (Horizontal CbCr off selection)
#define D_IM_DISP_CTOH_LEFT							(0)		/**< DOCTL2.CTOH Register<br> Left CbCr data is selected.						*/
#define D_IM_DISP_CTOH_RIGHT						(1)		/**< DOCTL2.CTOH Register<br> Right CbCr data is selected.						*/
#define D_IM_DISP_CTOH_AVERAGE						(2)		/**< DOCTL2.CTOH Register<br> Average of right and left CbCr data is selected.	*/
#define D_IM_DISP_CTOH_FILTER						(3)		/**< DOCTL2.CTOH Register<br> 1:2:1 filter Calculation CbCr data is selected.	*/

// DOCTL2 Register - CTOV (Vertical CbCr off selection)
#define D_IM_DISP_CTOV_TOP							(0)		/**< DOCTL2.CTOV Register<br> Top CbCr data is selected.						*/
#define D_IM_DISP_CTOV_AVERAGE						(1)		/**< DOCTL2.CTOV Register<br> Average of top and bottom CbCr data is selected.	*/

// DOCTL2 Register - TRSOS (SAV/EAV code output selection)
#define D_IM_DISP_TRSOS_NONE						(0)		/**< DOCTL2.TRSOS Register<br> SAV/EAV code is not output.					*/
#define D_IM_DISP_TRSOS_15_8						(1)		/**< DOCTL2.TRSOS Register<br> SAV/EAV code is output to [15:8].			*/
#define D_IM_DISP_TRSOS_7_0							(2)		/**< DOCTL2.TRSOS Register<br> SAV/EAV code is output to [7:0].				*/
#define D_IM_DISP_TRSOS_BOTH						(3)		/**< DOCTL2.TRSOS Register<br> SAV/EAV code is output to [15:8] and [7:0].	*/


// GRIDT Register - IFMT (Input format selection)
#define D_IM_DISP_GRIDT_IFMT_RGBA8888				(0)		/**< GRIDT.IFMT Register<br> RGBA8888 format.				*/
#define D_IM_DISP_GRIDT_IFMT_YCC422					(1)		/**< GRIDT.IFMT Register<br> YCbCr422 format.				*/
#define D_IM_DISP_GRIDT_IFMT_RGBA4444				(2)		/**< GRIDT.IFMT Register<br> RGBA4444 format.				*/
#define D_IM_DISP_GRIDT_IFMT_RGB565					(3)		/**< GRIDT.IFMT Register<br> RGB565 format.					*/
#define D_IM_DISP_GRIDT_IFMT_RGBA5551				(4)		/**< GRIDT.IFMT Register<br> RGBA5551 format.				*/
#define D_IM_DISP_GRIDT_IFMT_YCC422_A4				(3)		/**< GRIDT.IFMT Register<br> YCbCr422 A4 surface format.	*/
#define D_IM_DISP_GRIDT_IFMT_YCC422_A8				(4)		/**< GRIDT.IFMT Register<br> YCbCr422 A8 surface format.	*/

// GRIDT Register - IFBTMU (Input format bottom-up selection)
#define D_IM_DISP_IFBTMU_UP_TO_BOTTOM				(0)		/**< GRIDT.IFBTMU Register<br> Format arranged from up to bottom.	*/
#define D_IM_DISP_IFBTMU_BOTTOM_TO_UP				(1)		/**< GRIDT.IFBTMU Register<br> Format arranged from bottom to up.	*/

// GRIPO Register - IPO1~4 (Input data arrangement order setting) 
#define D_IM_DISP_IPO_RGBA8888_R					(0)		/**< GRIPO.IPO1~4 Register<br> R data in RGBA8888 format.				*/
#define D_IM_DISP_IPO_RGBA8888_G					(1)		/**< GRIPO.IPO1~4 Register<br> G data in RGBA8888 format.				*/
#define D_IM_DISP_IPO_RGBA8888_B					(2)		/**< GRIPO.IPO1~4 Register<br> B data in RGBA8888 format.				*/
#define D_IM_DISP_IPO_RGBA8888_A					(3)		/**< GRIPO.IPO1~4 Register<br> A data in RGBA8888 format.				*/
#define D_IM_DISP_IPO_YCC422_CB						(0)		/**< GRIPO.IPO1~4 Register<br> Cb data in YCbCr422 format.				*/
#define D_IM_DISP_IPO_YCC422_Y0						(1)		/**< GRIPO.IPO1~4 Register<br> Y0 data in YCbCr422 format.				*/
#define D_IM_DISP_IPO_YCC422_CR						(2)		/**< GRIPO.IPO1~4 Register<br> Cr data in YCbCr422 format.				*/
#define D_IM_DISP_IPO_YCC422_Y1						(3)		/**< GRIPO.IPO1~4 Register<br> Y1 data in YCbCr422 format.				*/
#define D_IM_DISP_IPO_RGBA4444_B0_A0				(0)		/**< GRIPO.IPO1~4 Register<br> {B0, A0} data in RGBA4444 format.		*/
#define D_IM_DISP_IPO_RGBA4444_R0_G0				(1)		/**< GRIPO.IPO1~4 Register<br> {R0, G0} data in RGBA4444 format.		*/
#define D_IM_DISP_IPO_RGBA4444_B1_A1				(2)		/**< GRIPO.IPO1~4 Register<br> {B1, A1} data in RGBA4444 format.		*/
#define D_IM_DISP_IPO_RGBA4444_R1_G1				(3)		/**< GRIPO.IPO1~4 Register<br> {R1, G1} data in RGBA4444 format.		*/
#define D_IM_DISP_IPO_RGB565_G0_B0					(0)		/**< GRIPO.IPO1~4 Register<br> {G0[2:0],B0} data in RGB565 format.		*/
#define D_IM_DISP_IPO_RGB565_R0_G0					(1)		/**< GRIPO.IPO1~4 Register<br> {R0,G0[5:3]} data in RGB565 format.		*/
#define D_IM_DISP_IPO_RGB565_G1_B1					(2)		/**< GRIPO.IPO1~4 Register<br> {G1[2:0],B1} data in RGB565 format.		*/
#define D_IM_DISP_IPO_RGB565_R1_G1					(3)		/**< GRIPO.IPO1~4 Register<br> {R1,G1[5:3]} data in RGB565 format.		*/
#define D_IM_DISP_IPO_RGBA5551_G0_B0_A0				(0)		/**< GRIPO.IPO1~4 Register<br> {G0[1:0],B0,A0} data in RGBA5551 format.	*/
#define D_IM_DISP_IPO_RGBA5551_R0_G0				(1)		/**< GRIPO.IPO1~4 Register<br> {R0,G0[4:2]} data in RGBA5551 format.	*/
#define D_IM_DISP_IPO_RGBA5551_G1_B1_A1				(2)		/**< GRIPO.IPO1~4 Register<br> {G1[1:0],B1,A1} data in RGBA5551 format.	*/
#define D_IM_DISP_IPO_RGBA5551_R1_G1				(3)		/**< GRIPO.IPO1~4 Register<br> {R1,G1[4:2]} data in RGBA5551 format.	*/

// GRBITRG Register - BITRG (Blink sync trigger)
#define D_IM_DISP_BITRG_NON							(0)		/**< GRBITRG.BITRG Register<br> Write:No operation, Read: -.						*/
#define D_IM_DISP_BITRG_WRITE_SYNC_WAIT				(1)		/**< GRBITRG.BITRG Register<br> Write:Blink sync reflect wait ON, Read: -.			*/
#define D_IM_DISP_BITRG_READ_STOP					(2)		/**< GRBITRG.BITRG Register<br> Write:No operation, Read:stop.						*/
#define D_IM_DISP_BITRG_READ_SYNC_WAIT				(3)		/**< GRBITRG.BITRG Register<br> Write:No operation, Read:Blink sync reflect wait.	*/

// MDSIC1 Register - MDSUDC (UpDateConfiguration input control)
#define D_IM_DISP_MDSUDC_NON						(0)		/**< MDSIC1.MDSUDC Register<br> Write:No operation, Read: -.							*/
#define D_IM_DISP_MDSUDC_WRITE_1SHOT				(1)		/**< MDSIC1.MDSUDC Register<br> Write:DPIUPDATECFG_I 1shot signal input, Read: -.		*/
#define D_IM_DISP_MDSUDC_READ_STOP					(2)		/**< MDSIC1.MDSUDC Register<br> Write:No operation, Read:stop.							*/
#define D_IM_DISP_MDSUDC_READ_UPDATE_WAIT			(3)		/**< MDSIC1.MDSUDC Register<br> Write:No operation, Read:DPI configuration update wait.	*/

// Clock On/Off
#define D_IM_DISP_CLKSTOP3_DISPAP_ON				(0x00008000)	/**< CLKSTOP3 - DISPAP[15:14] = 10	*/
#define D_IM_DISP_CLKSTOP3_DISPAP_OFF				(0x0000C000)	/**< CLKSTOP3 - DISPAP[15:14] = 11	*/
#define D_IM_DISP_CLKSTOP3_DISPAH_ON				(0x00020000)	/**< CLKSTOP3 - DISPAH[17:16] = 10	*/
#define D_IM_DISP_CLKSTOP3_DISPAH_OFF				(0x00030000)	/**< CLKSTOP3 - DISPAH[17:16] = 11	*/

// Interrupt flag for DWCH
#define D_IM_DISP_DWCH_FLG_PWF 						(0x00000001)	/**< DWCH(PWCH) PWF interrupt flag	*/
#define D_IM_DISP_DWCH_FLG_PWEF 					(0x00000010)	/**< DWCH(PWCH) PWEF interrupt flag	*/
#define D_IM_DISP_DWCH_FLG_PWXF				 		(0x00000100)	/**< DWCH(PWCH) PWXF interrupt flag	*/
#define	D_IM_DISP_INTFLG_CLEAR						(1)				/**< Clear the DISP interrupt flag  */

#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Definition
@{
*/
//---------------------- utility section -------------------------------
/** CC information */
//---------------------- colabo  section -------------------------------
/*		@}*/ /* end of im_disp_util */
#endif // CO_DDIM_UTILITY_USE
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/** Control block selection. (Bit flag) */
typedef enum {
	E_IM_DISP_HDMI = 0,		/**< HDMI block     (Common block 0).	*/
	E_IM_DISP_LCD_MIPI		/**< LCD/MIPI block (Common block 1).	*/
} E_IM_DISP_SEL;

/** Control layer selection. (Bit flag) */
typedef enum {
	// None
	E_IM_DISP_SEL_LAYER_NONE      = 0x0000,								/**< None							*/

	E_IM_DISP_SEL_LAYER_MAIN      = 0x0001,								/**< Main data input block.			*/
	E_IM_DISP_SEL_LAYER_DCORE     = 0x0002,								/**< Data output block.				*/
	E_IM_DISP_SEL_LAYER_OSD_0     = 0x0004,								/**< OSD-0 data input block.		*/
	E_IM_DISP_SEL_LAYER_OSD_1     = 0x0008,								/**< OSD-1 data input block.		*/

	// OSD
	E_IM_DISP_SEL_LAYER_OSD_ALL   = (E_IM_DISP_SEL_LAYER_OSD_0  | 
									 E_IM_DISP_SEL_LAYER_OSD_1),		/**< ALL OSD data input blocks.		*/

	// ALL
	E_IM_DISP_SEL_LAYER_ALL       = (E_IM_DISP_SEL_LAYER_MAIN  | 
									 E_IM_DISP_SEL_LAYER_DCORE | 
									 E_IM_DISP_SEL_LAYER_OSD_0 | 
									 E_IM_DISP_SEL_LAYER_OSD_1)			/**< ALL blocks.					*/
} E_IM_DISP_SEL_LAYER;

/** Callback function kind selection. */
typedef enum {
	E_IM_DISP_INT_CB_VE = 0,	/**< VE: Vsync detection interruption.											*/
	E_IM_DISP_INT_CB_VE2,		/**< VE2: Vsync detection interruption for 2nd field.							*/
	E_IM_DISP_INT_CB_GR0EE,		/**< GR0EE: OSD data 0 input block forwarding error interruption.				*/
	E_IM_DISP_INT_CB_GR1EE,		/**< GR1EE: OSD data 1 input block forwarding error interruption.				*/
	E_IM_DISP_INT_CB_GA0EE,		/**< GA0EE: OSD data 0 input block A component forwarding error interruption.	*/
	E_IM_DISP_INT_CB_GA1EE,		/**< GA1EE: OSD data 1 input block A component forwarding error interruption.	*/
	E_IM_DISP_INT_CB_LEE,		/**< LEE: Main data input block forwarding error interruption.					*/

	E_IM_DISP_INT_CB_GR0REE,	/**< GR0REE: OSD data 0 input block AXI error interruption.						*/
	E_IM_DISP_INT_CB_GR1REE,	/**< GR1REE: OSD data 1 input block AXI error interruption.						*/
	E_IM_DISP_INT_CB_GA0REE,	/**< GA0REE: OSD data 0 input block A component AXI error interruption.			*/
	E_IM_DISP_INT_CB_GA1REE,	/**< GA1REE: OSD data 1 input block A component AXI error interruption.			*/
	E_IM_DISP_INT_CB_LREE,		/**< LREE: Main data input block AXI error interruption.						*/
	E_IM_DISP_INT_CB_MAX		/**< Stopper */
} E_IM_DISP_INT_CB;

	/** Callback function kind selection.(DWCH) */
typedef enum {
	E_IM_DISP_DWCH_INT_CB_PWE = 0,	/**< PWE: PWCH successful completion interrupt request permission.			*/
	E_IM_DISP_DWCH_INT_CB_PWEE,     /**< PWEE: PWCH transmission error interrupt request permission. 			*/
	E_IM_DISP_DWCH_INT_CB_PWXE,     /**< PWXE: PWCH AXI error interrupt request permission. 					*/
	E_IM_DISP_DWCH_INT_CB_MAX		/**< Stopper */
} E_IM_DISP_DWCH_INT_CB;

/** Selection whether stop of channel is waited or do not wait. */
typedef enum {
	E_IM_DISP_STOP_TYPE_WITHOUT_WAIT	= 0x00000000,	/**< Forced stop.		*/
	E_IM_DISP_STOP_TYPE_WITH_WAIT		= 0x00000002	/**< Stop with wait.	*/
} E_IM_DISP_STOP_TYPE;

/** Frequency in which VSYNC is waited. */
typedef enum {
	E_IM_DISP_WAIT_1_TIME = 1,		/**< Wait one time	*/
	E_IM_DISP_WAIT_2_TIME			/**< Wait two times	*/
} E_IM_DISP_WAIT;

/** Bank index. */
typedef enum {
	E_IM_DISP_BANK_00 = 0,	/**< Bank 0	*/
	E_IM_DISP_BANK_01,		/**< Bank 1 */
	E_IM_DISP_BANK_10,		/**< Bank 2 */
	E_IM_DISP_BANK_11,		/**< Bank 3 */
} E_IM_DISP_BANK;

/** Matrix kind. */
typedef enum {
	E_IM_DISP_MATRIX_KIND_Y2R = 0,		/**< YCbCr -> RGB's matrix		*/
	E_IM_DISP_MATRIX_KIND_R2Y,			/**< RGB -> YCbCr's matrix		*/
	E_IM_DISP_MATRIX_KIND_CC,			/**< Color space convert matrix	*/
} E_IM_DISP_MATRIX_KIND;

/** Vertical sync signal detection interrupt flag generation timing. */
typedef enum {
	E_IM_DISP_VSYNC_FLAG_NONE = 0,	/**< No any flag						*/
	E_IM_DISP_VSYNC_FLAG_FIRST,		/**< Generate a flag at No. 1 field		*/
	E_IM_DISP_VSYNC_FLAG_SECOND,	/**< Generate a flag at No. 2 field		*/
	E_IM_DISP_VSYNC_FLAG_BOTH,		/**< Generate a flag at No. 1 and No. 2	*/
} E_IM_DISP_VSYNC_FLAG;

/** Gamma correct or Anti-Gamma correct or Luminance correct or Chroma correct. */
typedef enum {
	E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA			= 0x00000001,	/**< Anti-gamma correct						*/
	E_IM_DISP_CORRECT_SELECT_GAMMA_IN			= 0x00000002,	/**< Gamma correct (Main data input block)	*/
	E_IM_DISP_CORRECT_SELECT_GAMMA_OUT			= 0x00000004,	/**< Gamma correct (Data output block)		*/
	E_IM_DISP_CORRECT_SELECT_LUMINANCE			= 0x00000008,	/**< Luminance correct						*/
	E_IM_DISP_CORRECT_SELECT_CHROMA				= 0x00000010,	/**< Chroma correct							*/
} E_IM_DISP_CORRECT_SELECT;

/** Table access enable setting of Gamma correct or Anti-Gamma correct or gradation adjustment. */
typedef enum {
	E_IM_DISP_TABLE_ACCESS_DISABLE = 0,		/**< Table access disable	*/
	E_IM_DISP_TABLE_ACCESS_ENABLE			/**< Table access enable	*/
} E_IM_DISP_TABLE_ACCESS;

/** Processing enable setting of Gamma correct or Anti-Gamma correct or gradation adjustment. */
typedef enum {
	E_IM_DISP_CORRECT_PROC_DISABLE = 0,		/**< Processing disable	*/
	E_IM_DISP_CORRECT_PROC_ENABLE			/**< Processing enable	*/
} E_IM_DISP_CORRECT_PROC;

/** Select interruption demand permission. */
typedef enum {
	E_IM_DISP_INTERRUPTION_SELECT_VE2    = 0x00100000,	/**< Vertical syschronization signal interruption for 2nd field permission */
	E_IM_DISP_INTERRUPTION_SELECT_VE     = 0x00010000,	/**< Vertical syschronization signal interruption permission*/

	E_IM_DISP_INTERRUPTION_SELECT_GA1REE = 0x00008000,	/**< GR1 A component AXI error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_GA0REE = 0x00004000,	/**< GR0 A component AXI error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_GR1REE = 0x00002000,	/**< GR1 AXI error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_GR0REE = 0x00001000,	/**< GR0 AXI error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_LREE   = 0x00000100,	/**< Main AXI error interruption permission */

	E_IM_DISP_INTERRUPTION_SELECT_GA1EE  = 0x00000080,	/**< GR1 A component transmitte error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_GA0EE  = 0x00000040,	/**< GR0 A component transmitte error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_GR1EE  = 0x00000020,	/**< GR1 transmitte error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_GR0EE  = 0x00000010,	/**< GR0 transmitte error interruption permission */
	E_IM_DISP_INTERRUPTION_SELECT_LEE    = 0x00000001,	/**< Main transmitte error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_VE		=	E_IM_DISP_INTERRUPTION_SELECT_VE |
											E_IM_DISP_INTERRUPTION_SELECT_VE2,/**< All Vertical syschronization signal interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_GRREE	=	E_IM_DISP_INTERRUPTION_SELECT_GA1REE |
											E_IM_DISP_INTERRUPTION_SELECT_GA0REE |
											E_IM_DISP_INTERRUPTION_SELECT_GR1REE |
											E_IM_DISP_INTERRUPTION_SELECT_GR0REE,/**< All GR's AXI error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_GREE	=	E_IM_DISP_INTERRUPTION_SELECT_GA1EE |
											E_IM_DISP_INTERRUPTION_SELECT_GA0EE |
											E_IM_DISP_INTERRUPTION_SELECT_GR1EE |
											E_IM_DISP_INTERRUPTION_SELECT_GR0EE,/**< All GR's transmitte error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_REE	=	E_IM_DISP_CORRECT_SELECT_ALL_GREE | 
											E_IM_DISP_INTERRUPTION_SELECT_LREE,/**< Main and all GR's AXI error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_EE		=	E_IM_DISP_CORRECT_SELECT_ALL_GREE | 
											E_IM_DISP_INTERRUPTION_SELECT_LEE,/**< Main and all GR's transmitte error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_GR_ERROR =	E_IM_DISP_CORRECT_SELECT_ALL_GREE |
											E_IM_DISP_CORRECT_SELECT_ALL_GRREE,/**< All GR's transmitte error and AXI error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_MIAN_ERROR=E_IM_DISP_INTERRUPTION_SELECT_LREE | 
											E_IM_DISP_INTERRUPTION_SELECT_LEE,/**< Main's transmitte error interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL_ERROR	=	E_IM_DISP_CORRECT_SELECT_ALL_GR_ERROR | 
											E_IM_DISP_CORRECT_SELECT_ALL_MIAN_ERROR,/**< Main and all GRs all errors interruption permission */

	E_IM_DISP_CORRECT_SELECT_ALL		=	E_IM_DISP_CORRECT_SELECT_ALL_VE	|
											E_IM_DISP_CORRECT_SELECT_ALL_ERROR	/**< All interruption permission */
} E_IM_DISP_INTERRUPTION_SELECT;


/** Select interruption demand permission. */
typedef enum {
	E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE	= 0x00000001,	/**< PWCH successful completion interrupt request permission */
	E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE	= 0x00000010,	/**< PWCH transmission error interrupt request permission */
	E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE	= 0x00000100,	/**< PWCH AXI error interrupt request permission */

	E_IM_DISP_DWCH_CORRECT_SELECT_ALL	=	E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE  |
											E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE |
											E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE	/**< All interruption permission */
} E_IM_DISP_DWCH_INTERRUPTION_SELECT;

/** Interruption demand permission state. */
typedef enum {
	E_IM_DISP_INTERRUPTION_STATE_NONE = 0x00000000,						/**< Set no  permission*/
	E_IM_DISP_INTERRUPTION_STATE_SET  = E_IM_DISP_CORRECT_SELECT_ALL,	/**< Set permission*/

} E_IM_DISP_INTERRUPTION_STATE;

/** OSD start address no. */
typedef enum {
	E_IM_DISP_OSD_SA_BANK_NO_0_0 = 0,	/**< OSD start address bank no 0_0	*/
	E_IM_DISP_OSD_SA_BANK_NO_0_1,		/**< OSD start address bank no 0_1	*/
	E_IM_DISP_OSD_SA_BANK_NO_0_2,		/**< OSD start address bank no 0_2	*/
	E_IM_DISP_OSD_SA_BANK_NO_0_3,		/**< OSD start address bank no 0_3	*/
	E_IM_DISP_OSD_SA_BANK_NO_1,			/**< OSD start address bank no 1	*/
	E_IM_DISP_OSD_SA_BANK_NO_2,			/**< OSD start address bank no 2	*/
	E_IM_DISP_OSD_SA_BANK_NO_3,			/**< OSD start address bank no 3	*/
	E_IM_DISP_OSD_SA_BANK_NO_4,			/**< OSD start address bank no 4	*/
	E_IM_DISP_OSD_SA_BANK_NO_5,			/**< OSD start address bank no 5	*/
	E_IM_DISP_OSD_SA_BANK_NO_6,			/**< OSD start address bank no 6	*/
	E_IM_DISP_OSD_SA_BANK_NO_7,			/**< OSD start address bank no 7	*/
	E_IM_DISP_OSD_SA_BANK_NO_8,			/**< OSD start address bank no 8	*/
	E_IM_DISP_OSD_SA_BANK_NO_9,			/**< OSD start address bank no 9	*/
} E_IM_DISP_OSD_SA_BANK_NO;

/** LRPGCTL Register - RPGTMG (Register parameter internal taking timing selection)<br>
 RPGCTL Register - RPGTMG (Register parameter internal taking timing selection)<br>
 GRRPGCTL Register - RPGTMG (Register parameter internal taking timing selection)*/
typedef enum {
	E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP = 0,	/**< LRPGCTL.RPGTMG, RPGCTL.RPGTMG, GRRPGCTL.RPGTMG Register<br> 
												 When vertical synchronizing signal is generated at frame head.	*/
	E_IM_DISP_RPGTMG_VSYNC_VSYNC			/**< LRPGCTL.RPGTMG, RPGCTL.RPGTMG, GRRPGCTL.RPGTMG Register<br>
												 When vertical synchronizing signal is generated.				*/
}E_IM_DISP_RPGTMG;

/** LRSZ0 Register - RSZSL (Horizontal resize method selection)<br>
 GRRSZ0 Register - RSZSL (Horizontal resize method selection)*/
typedef enum {
	E_IM_DISP_RSZSL_PADDING_THINNING = 0,	/**< LRSZ0.RSZSL, GRRSZ0.RSZSL Register<br> Padding and thinning out processing.	*/
	E_IM_DISP_RSZSL_BILINEAR				/**< LRSZ0.RSZSL, GRRSZ0.RSZSL Register<br> Bilinear interpolation processing.		*/
}E_IM_DISP_RSZSL;

/** LCEPMD Register - CEPMD (Color difference data padding mode) */
typedef enum {
	E_IM_DISP_CEPMD_UPPER_SIDE			= 0,	/**< LCEPMD.CEPMD Register<br> Select the upper side of the color difference data							*/
	E_IM_DISP_CEPMD_AVERAGE				= 2,	/**< LCEPMD.CEPMD Register<br> Select the average value of the top and bottom of the color difference data	*/
	E_IM_DISP_CEPMD_3TO1_1TO3_RATE		= 3		/**< LCEPMD.CEPMD Register<br> Select a value calculated the top and bottom
																			   of the color difference data at a rate of 3:1 and 1:3						*/
} E_IM_DISP_CEPMD;

/** LYHLPCTL Register - YHLPMD (YHLP mode) */
typedef enum {
	E_IM_DISP_YHLPMD_HORIZONTAL_SPACE_FILTER	= 0,	/**< LYHLPCTL.YHLPMD Register<br> Horizontal space filter	*/
	E_IM_DISP_YHLPMD_OUTLINE_ENHANCEMENT		= 1,	/**< LYHLPCTL.YHLPMD Register<br> Outline enhancement		*/
	E_IM_DISP_YHLPMD_OUTLINE_COLOR				= 2		/**< LYHLPCTL.YHLPMD Register<br> Outline color				*/
} E_IM_DISP_YHLPMD;

/** LYWCTL Register - YWSL (Warning type select) */
typedef enum {
	E_IM_DISP_YWSL_BRIGHTNESS_SPECIFIED_COLOR	= 0,	/**< LYWCTL.YWSL Register<br> Brightness warning - Specified color	*/
	E_IM_DISP_YWSL_BRIGHTNESS_ZEBRA				= 1,	/**< LYWCTL.YWSL Register<br> Brightness warning - Zebra			*/
	E_IM_DISP_YWSL_RGB_SPECIFIED_COLOR			= 2,	/**< LYWCTL.YWSL Register<br> RGB warning - Specified color			*/
	E_IM_DISP_YWSL_RGB_ZEBRA					= 3		/**< LYWCTL.YWSL Register<br> RGB warning - Zebra					*/
} E_IM_DISP_YWSL;

/** LYW0ST Register - YW0MD (Range 0 - warning processing mode select)<br>
 LYW1ST Register - YW1MD (Range 0 - warning processing mode select) */
typedef enum {
	E_IM_DISP_YWMD_BR_RGB = 0,	/**< LYW0ST.YW0MD, LYW1ST.YW1MD Register<br> Brightness reverse display or RGB pixel warning display.	*/
	E_IM_DISP_YWMD_BW_R,		/**< LYW0ST.YW0MD, LYW1ST.YW1MD Register<br> Brightness warning display or R pixel warning display.		*/
	E_IM_DISP_YWMD_G,			/**< LYW0ST.YW0MD, LYW1ST.YW1MD Register<br> Brightness G pixel warning display.						*/
	E_IM_DISP_YWMD_B			/**< LYW0ST.YW0MD, LYW1ST.YW1MD Register<br> Brightness B pixel warning display.						*/
}E_IM_DISP_YWMD;

/** FDOEN Register - (Enable force data output or not)*/
typedef enum {
	E_IM_DISP_FDOEN_ORDINARY_OUT = 0,	/**< FDOEN Register<br> Ordinary output.	*/
	E_IM_DISP_FDOEN_FORCE_DATA_OUT		/**< FDOEN Register<br> Force data output.	*/
}E_IM_DISP_FDOEN;

/** GDISPEN Register - (Grid display enable)*/
typedef enum {
	E_IM_DISP_GDISPEN_NOT_DISPLAY = 0,	/**< GDISPEN Register<br> Not Display.	*/
	E_IM_DISP_GDISPEN_DISPLAY			/**< GDISPEN Register<br> Display.		*/
}E_IM_DISP_GDISPEN;

/** GRSCCTL Register - IDSET (Input data area setting), IDM (Input data area monitor)*/
typedef enum {
	E_IM_DISP_INPUT_DATA_AREA_0 = 0,	/**< GRSCCTL.IDSET, GRSCCTL.IDM Register<br> Input data area 0.	*/
	E_IM_DISP_INPUT_DATA_AREA_1,		/**< GRSCCTL.IDSET, GRSCCTL.IDM Register<br> Input data area 1.	*/
	E_IM_DISP_INPUT_DATA_AREA_2,		/**< GRSCCTL.IDSET, GRSCCTL.IDM Register<br> Input data area 2.	*/
	E_IM_DISP_INPUT_DATA_AREA_3			/**< GRSCCTL.IDSET, GRSCCTL.IDM Register<br> Input data area 3.	*/
} E_IM_DISP_INPUT_DATA_AREA;

/** GRAREN Register - AREN (Display area enable setting)<br>
 GRBSL  Register - BSL (Display area blink setting)*/
typedef enum {
	E_IM_DISP_AREA_0   = 0x0001,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 0	*/
	E_IM_DISP_AREA_1   = 0x0002,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 1	*/
	E_IM_DISP_AREA_2   = 0x0004,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 2	*/
	E_IM_DISP_AREA_3   = 0x0008,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 3	*/
	E_IM_DISP_AREA_4   = 0x0010,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 4	*/
	E_IM_DISP_AREA_5   = 0x0020,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 5	*/
	E_IM_DISP_AREA_6   = 0x0040,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 6	*/
	E_IM_DISP_AREA_7   = 0x0080,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 7	*/
	E_IM_DISP_AREA_8   = 0x0100,	/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 8	*/
	E_IM_DISP_AREA_9   = 0x0200		/**< GRAREN.AREN, GRBSL.BSL Register<br> Display area 9	*/
} E_IM_DISP_AREA;

/** TRG Register - TGKMD (External start-up mode) */
typedef enum {
	E_IM_DISP_TGKMD_NORMAL		= 0,	/**< TRG.TGKMD Register<br> Normal start-up		*/
	E_IM_DISP_TGKMD_EXTERNAL	= 1		/**< TRG.TGKMD Register<br> External start-up	*/
} E_IM_DISP_TGKMD;

/** Write channel block number 0/1. */
typedef enum {
	E_IM_DISP_WC_NUM_0	= 0,	/**< Write channel 0 (Luminance data)	*/
	E_IM_DISP_WC_NUM_1,			/**< Write channel 1 (Chrominance data)	*/
} E_IM_DISP_WC_NUM;

/** P2MCTL Register - PKNEMD (KNEE compression mode) */
typedef enum {
	E_IM_DISP_PKNEMD_NOT	= 0,	/**< not KNEE compression processing.				*/
	E_IM_DISP_PKNEMD_12_8,			/**< KNEE compression processing(12bit to 8bit).	*/
	E_IM_DISP_PKNEMD_12_10			/**< KNEE compression processing(12bit to 10bit).	*/
} E_IM_DISP_PKNEMD;

/** P2MCTL Register - PLPF (LPF recording mode) */
typedef enum {
	E_IM_DISP_PLPF_INVALID	= 0,	/**< LPF invalid.	*/
	E_IM_DISP_PLPF_SHORT,			/**< LPF(Short).	*/
	E_IM_DISP_PLPF_MIDDLE,			/**< LPF(Middle).	*/
	E_IM_DISP_PLPF_LONG				/**< LPF(Long).		*/
} E_IM_DISP_PLPF;

/** P2MCTL Register - P2MTYP (P2M recording mode) */
typedef enum {
	E_IM_DISP_P2MTYP_8	= 0,	/**< 8 bit recording mode.			*/
	E_IM_DISP_P2MTYP_10_PACK,	/**< 10 bit pack recording mode.	*/
	E_IM_DISP_P2MTYP_12_PACK,	/**< 12 bit pack recording mode.	*/
	E_IM_DISP_P2MTYP_16			/**< 16 bit recording mode.			*/
} E_IM_DISP_P2MTYP;

/** P2M2PMD Register (P2M 1 pixel / 2 pixels input mode selection) */
typedef enum {
	E_IM_DISP_P2M2PMD_1	= 0,	/**< 1 pixel input mode.	*/
	E_IM_DISP_P2M2PMD_2			/**< 2 pixels input mode.	*/
} E_IM_DISP_P2M2PMD;

#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Enumeration
@{
*/
//---------------------- utility section -------------------------------
/** CC information */
typedef enum {
	E_IM_DISP_CC_MATRIX_NOCONVERT = 0, /**< No any convert */
	E_IM_DISP_CC_MATRIX_709			/**< Convert to 709 */
} E_IM_DISP_CC_MATRIX;
//---------------------- colabo  section -------------------------------
/*		@}*/ /* end of im_disp_util */
#endif // CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/** LIDT0/LIDT1 register structure. */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		/* LIDT0 */
		ULONG	IFMT	:5;		/**< Input format selection.<br>
									 <ul><li>@ref D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT
										 <li>@ref D_IM_DISP_LIDT_IFMT_YCC422_RP_12BIT_PACK
										 <li>@ref D_IM_DISP_LIDT_IFMT_YCC422_RP_16BIT
										 <li>@ref D_IM_DISP_LIDT_IFMT_YCC422_RP_10BIT_PACK
										 <li>@ref D_IM_DISP_LIDT_IFMT_YCC420_RP_8BIT
										 <li>@ref D_IM_DISP_LIDT_IFMT_YCC420_RP_10BIT_PACK
										 <li>@ref D_IM_DISP_LIDT_IFMT_GENERAL_8BIT
										 <li>@ref D_IM_DISP_LIDT_IFMT_GENERAL_16BIT
										 <li>@ref D_IM_DISP_LIDT_IFMT_GENERAL_10BIT
										 <li>@ref D_IM_DISP_LIDT_IFMT_GENERAL_10BITx2</ul>							*/
		ULONG			:3;		/**< Reserve																		*/
		ULONG	NBT		:2;		/**< Burst size selection.<br>
									 <ul><li>@ref D_IM_DISP_NBT_128BYTE
										 <li>@ref D_IM_DISP_NBT_256BYTE
										 <li>@ref D_IM_DISP_NBT_512BYTE</ul>										*/
		ULONG			:2;		/**< Reserve																		*/
		ULONG	IFEW	:2;		/**< Input format effective bit width selection.<br>
									 <ul><li>@ref D_IM_DISP_IFEW_16BIT
									     <li>@ref D_IM_DISP_IFEW_14BIT
										 <li>@ref D_IM_DISP_IFEW_12BIT
										 <li>@ref D_IM_DISP_IFEW_10BIT</ul>											*/
		ULONG			:2;		/**< Reserve																		*/
		ULONG	LVFM	:1;		/**< LVFM: Video format selection.<br>
									 <ul><li>@ref D_IM_DISP_FORMAT_NORMAL
										 <li>@ref D_IM_DISP_FORMAT_VIDEO</ul>											*/
		ULONG			:15;	/**< Reserve																		*/
		/* LIDT1 */
		ULONG	PKGDV	:4;		/**< PKGDV: Continuous forwarding demand control.<br>
									 Value : 0 (fix).																*/
		ULONG	CACHE	:4;		/**< CACHE: Cache type.<br>
									 d[0]:Bufferable bit. Value 0:Nonbufferable, 1:Bufferable.<br>
									 d[1]:Cacheable bit. Value 0:Noncacheable, 1:Cacheable.<br>
									 d[2]:Read allocate bit. Value 0:Read through, 1:Allocate on Reads.<br>
									 d[3]:Write allocate bit. Value 0:Write through, 1:Allocate on writes.<br>		*/
		ULONG	PROT	:3;		/**< PROT: Protect type.<br>
									 d[0]:Normal or privileged. Value 0:Normal access, 1:Privileged access.<br>
									 d[1]:Secure or non-secure. Value 0:Secure access, 1:Non-secure access.<br>
									 d[2]:Instruction or Data. Value 0:Data access, 1:Instrauction access.<br>		*/
		ULONG			:1;		/**< Reserve																		*/
		ULONG	YSLVSL	:1;		/**< YSLVSL: Master interface selection of Y.<br>
									 <ul><li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_0
										 <li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_1</ul>							*/
		ULONG	CSLVSL	:1;		/**< CSLVSL: Master interface selection of Cb/Cr.<br>
									 <ul><li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_0
										 <li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_1</ul>							*/
		ULONG			:18;	/**< Reserve																		*/
	} bit;	/**< Bit field */
} U_IM_DISP_LIDT;

/** LRSZ1, GRRSZ1 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HRSZM	:5;		/**< HRSZM: Horizontal resize magnification setting (M of M/N)<br>
									 Value range:1~31.<br>
									 Resize magnification range:M/N=1/2~8.																*/
		ULONG			:3;		/**< Reserve																							*/
		ULONG	HRSZN	:5;		/**< HRSZM: Horizontal resize magnification setting (N of M/N)<br>
									 Value range:1~31.<br>
									 Resize magnification range:M/N=1/2~8.																*/
		ULONG			:11;	/**< Reserve																							*/
		ULONG	HRSZOF	:5;		/**< HRSZOF: Horizontal resize offset.<br>
									 Value range:0~31.<br>
									 fulfill the following conditions.<br>
									 HRSZOF < (pixel count before resizing) * HRSZM - (pixel count before resizing - 1) * HRSZM.<br>
									 HRSZOF < HRSZM.<br>
									 0 is set up when resize magnification is 1 or LRSZ0.RSZSL=0.<br>									*/
		ULONG			:3;		/**< Reserve																							*/
	} bit;	/**< Bit field */
} U_IM_DISP_RSZ1;

/** LRSZ2, GRRSZ2 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VRSZM	:5;		/**< VRSZM: Vertical resize magnification setting (M of M/N)<br>
									 Value range:1~31.<br>
									 Resize magnification range:M/N=1/2~8.																*/
		ULONG			:3;		/**< Reserve																							*/

		ULONG	VRSZN	:5;		/**< VRSZN: Vertical resize magnification setting (N of M/N)<br>
									 Value range:1~31.<br>
									 Resize magnification range:M/N=1/2~8.																*/
		ULONG			:11;	/**< Reserve																							*/
		ULONG	VRSZOF	:5;		/**< VRSZOF: Vertical resize offset.<br>
									 Value range:0~31.<br>
									 fulfill the following conditions.<br>
									 VRSZOF < (pixel count before resizing) * VRSZM - (pixel count before resizing - 1) * VRSZM.<br>
									 VRSZOF < VRSZM.<br>
									 0 is set up when resize magnification is 1 or LRSZ0.RSZSL(GRRSZ0.RSZSL)=0.<br>						*/
		ULONG			:3;		/**< Reserve																							*/
	} bit;	/**< Bit field */
} U_IM_DISP_RSZ2;

/** LRSZ3, GRRSZ3 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HCSTA	:6;		/**< HCSTA: Horizontal chrominance start position adjustment.<br>
									 Value range:0~63.												*/
		ULONG			:2;		/**< Reserve														*/

		ULONG	VCSTA	:6;		/**< VCSTA: Vertical chrominance start position adjustment.<br>
									 Value range:0~63.												*/
		ULONG			:18;	/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_DISP_RSZ3;

/** LYHLPCTL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	YHLPEN	:1;	/**< YHLPEN: YHLP enable<br>
								 Value 0:disable, 1:enable.<br>								*/
		ULONG			:7;	/**< Reserve													*/
		ULONG	YHLPMD	:2;	/**< YHLPMD: YHLP mode<br>
								 <ul><li>@ref E_IM_DISP_YHLPMD_HORIZONTAL_SPACE_FILTER
									 <li>@ref E_IM_DISP_YHLPMD_OUTLINE_ENHANCEMENT
									 <li>@ref E_IM_DISP_YHLPMD_OUTLINE_COLOR</ul>			*/
		ULONG			:22;/**< Reserve													*/
	} bit;	/**< Bit field */
} U_IM_DISP_LYHLPCTL;

/** LYHLPK0/1 register structure. */
typedef union {
	ULONG		word[2];	/**< All bits */
	struct {
		ULONG	YHLPK0	:9;	/**< YHLPK0: YHLP filter coefficient 0<br>
								 Value range:0~511.											*/
		ULONG			:7;	/**< Reserve													*/
		ULLONG	YHLPK1	:8;	/**< YHLPK1: YHLP filter coefficient 1<br>
								 Value range:0~255.											*/
		ULONG			:8;/**< Reserve														*/
		ULONG	YHLPK2	:8;	/**< YHLPK0: YHLP filter coefficient 2<br>
								 Value range:0~255.											*/
		ULONG			:8;	/**< Reserve													*/
		ULONG	YHLPK3	:8;	/**< YHLPK3: YHLP filter coefficient 3<br>
								 Value range:0~255.											*/
		ULONG			:8;/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_DISP_LYHLPK;

/** LYHLPOL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	YHLPCOR	:16;	/**< YHLPCOR: Outline coring value <br>
									 Value range:0~65535.									*/
		ULONG	YHLPTH	:16;	/**< YHLPTH: Outline color threshold<br>
									 Value range:0~65535.									*/
	} bit;	/**< Bit field */
} U_IM_DISP_LYHLPOL;

/** LYHLPCLR register structure. */
typedef union {
	ULLONG		dword;	/**< All bits */
	struct {
		ULLONG	YHLPY	:16;	/**< YHLPY: Outline color pixel Y.<br>
									 Value range:0~65535.									*/
		ULLONG	YHLPCB	:16;	/**< YHLPCB: Outline color pixel Cb.<br>
									 Value range:0~65535.									*/
		ULLONG	YHLPCR	:16;	/**< YHLPCR: Outline color pixel Cr.<br>
									 Value range:0~65535.									*/
		ULLONG			:16;	/**< Reserve												*/
	} bit;	/**< Bit field */
} U_IM_DISP_LYHLPCLR;

/**	 LYWCTL	register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	YWEN	:1;		/**< YWEN: Warning enable.<br>
									 <ul><li>@ref D_IM_DISP_ENABLE_OFF
										 <li>@ref D_IM_DISP_ENABLE_ON</ul>					*/
		ULONG			:7;		/**< Reserve												*/
		ULONG	YWSL	:2;		/**< YWSL: Warning type select.<br>
									 <ul><li>@ref E_IM_DISP_YWSL_BRIGHTNESS_SPECIFIED_COLOR
										 <li>@ref E_IM_DISP_YWSL_BRIGHTNESS_ZEBRA
										 <li>@ref E_IM_DISP_YWSL_RGB_SPECIFIED_COLOR
										 <li>@ref E_IM_DISP_YWSL_RGB_ZEBRA</ul>				*/
		ULONG			:22;	/**< Reserve												*/
	}bit;	/**< Bit field */
} U_IM_DISP_LYWCTL;

/**	LYW0TH/LYW1TH register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	YWTHH	:16;	/**< YW0THH/YW1THH: Warning level upper limit.<br>
									 Value range:0~65535.									*/
		ULONG	YWTHL	:16;	/**< YW0THL/YW1THL: Warning level lower limit.<br>
									 Value range:0~65535.									*/
	}bit;	/**< Bit field */
} U_IM_DISP_LYWTH;

/** LYW0CA/LYW0CB/LYW1CA/LYW1CB register structure. */
typedef union {
	ULLONG		dword;	/**< All bits */
	struct {
		ULLONG	YWY		:16;	/**< YW0AY/YW0BY/YW1AY/YW1BY: Warning color Y(R).<br>
									 LYWCTL.YWSL=0or1 -> Y data.<br>
									 LYWCTL.YWSL=2or3 -> R data.<br>
									 Value range:0~65535.									*/
		ULLONG	YWCB	:16;	/**< YW0ACB/YW0BCB/YW1ACB/YW1BCB: Warning color Cb(G).<br>
									 LYWCTL.YWSL=0or1 -> Cb data.<br>
									 LYWCTL.YWSL=2or3 -> G data.<br>
									 Value range:0~65535.									*/
		ULLONG	YWCR	:16;	/**< YW0ACR/YW0BCR/YW1ACR/YW1BCR: Warning color Cr(B).<br>
									 LYWCTL.YWSL=0or1 -> Cr data.<br>
									 LYWCTL.YWSL=2or3 -> B data.<br>
									 Value range:0~65535.									*/
		ULLONG	YWA		:8;		/**< YW0AA/YW0BA/YW1AA/YW1BA: Warning color A.<br>
									 Value range:0~255.										*/
		ULLONG			:8;		/**< Reserve												*/
	} bit;	/**< Bit field */
} U_IM_DISP_LYWC;

/**	LBLTMR register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	BTIMH	:8;	/**< BTIMH: Setting of luminance warning display period.<br>
								 Value range:0~255.<br>
								 BTIMH = frame cout - 1.<br>									*/
		ULONG	BTIML	:8;	/**< BTIML: Setting of luminance warning non-display period.<br>
								 Value range:0~255.<br>
								 BTIML = frame cout - 1.<br>									*/
		ULONG	BTIMD	:1;	/**< BTIMD: Warning display period mode.<br>
									 <ul><li>@ref D_IM_DISP_BTIMD_NORMAL
										 <li>@ref D_IM_DISP_BTIMD_BLINK</ul>					*/
		ULONG			:15;/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_DISP_LBLTMR;

/** LZBWID register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ZBHH	:8;		/**< ZBHH: Zebra warning horizontal high brightness width.<br>
									 Value range : 2~255.<br>									*/
		ULONG	ZBHL	:8;		/**< ZBHL: Zebra warning horizontal low brightness width.<br>
									 Value range : 2~255.<br>									*/
		ULONG			:16;	/**< Reserve													*/
	} bit;	/**< Bit field */
} U_IM_DISP_LZBWID;

/** LZBV register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ZBVSTA	:1;		/**< ZBVSTA: Zebra warning dispay vertical start position.<br>
									 Value :0 or 1.												*/
		ULONG			:7;		/**< Reserve													*/
		ULONG	ZBVCYC	:1;		/**< ZBVCYC: Zebra warning dispay vertical cycle.<br>
									 Value 0:a pixel cycle, 1:2 pixels cycle.<br>				*/
		ULONG			:23;	/**< Reserve													*/
	} bit;	/**< Bit field */
} U_IM_DISP_LZBV;

/** LZBPT register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ZBDIR	:1;	/**< ZBDIR: Zebra warning dispay pattern direction.<br>
								 Value 0:right down, 1:left down.<br>							*/
		ULONG			:31;/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_DISP_LZBPT;

/**	LTBLASET register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	IGTAEN	:1;		/**< IGTAEN : Inverse gamma table access enable.<br>
									 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
										 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>		*/
		ULONG	IGEN	:1;		/**< IGEN: Inverse gamma correction enable.<br>
									 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
										 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>		*/
		ULONG	IGTSL	:1;		/**< IGTSL: Inverse gamma correction table surface.<br>
									 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
										 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>			*/
		ULONG			:5;		/**< Reserve												*/
		ULONG	GTAEN	:1;		/**< GTAEN : Gamma table access enable.<br>
									 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
										 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>		*/
		ULONG	GMEN	:1;		/**< GMEN: Gamma correction enable.<br>
									 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
										 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>		*/
		ULONG	GMTSL	:1;		/**< GMTSL: Gamma correction table surface.<br>
									 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
										 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>			*/
		ULONG			:5;		/**< Reserve												*/
		ULONG	IGTMN	:1;		/**< IGTMN: Inverse gamma correction monitor.<br>
									 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
										 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>			*/
		ULONG			:7;		/**< Reserve												*/
		ULONG	GMTMN   :1;		/**< GMTMN: Gamma correction monitor.<br>
									 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
										 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>			*/
		ULONG			:7;		/**< Reserve												*/
	} bit;	/**< Bit field */
} U_IM_DISP_LTBLASET;

/** LGMMD register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GMMD	:1;		/**< GMMD: Gamma correct mode.<br>
									 <ul><li>@ref D_IM_DISP_GMMD_EQUAL_INTERVAL_MODE
										 <li>@ref D_IM_DISP_GMMD_UNEQUAL_INTERVALS_MODE</ul>	*/
		ULONG			:31;	/**< Reserve													*/
	} bit;	/**< Bit field */
} U_IM_DISP_LGMMD;

/** LALP register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	ALP		:8;		/**< ALP: alpha value setting.<br>
									 Value range:0~255				*/
		ULONG			:24;	/**< Reserve						*/
	} bit;	/**< Bit field */
} U_IM_DISP_LALP;

/**	LDSTA, GRDSTA, GHDSTA, GRTDSTA, FFDSTA register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DSH		:16;/**< DSH: Horizontal display beginning position.<br>
								 Value range:0~65534 (2-pixel unit)				*/
		ULONG	DSV		:14;/**< DSV: Vertical display beginning position.<br>
								 Value range:0~16382 (2-pixel unit)				*/
		ULONG			:2;	/**< Reserve										*/
	} bit;	/**< Bit field */
} U_IM_DISP_DSTA;

/**	LREVDISP register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	HREV	:1;	/**< HREV: Horizontal reversing display selection.
								 <ul><li>@ref D_IM_DISP_REV_NORMAL
									 <li>@ref D_IM_DISP_REV_REVERSE</ul>		*/
		ULONG			:7;	/**< Reserve										*/
		ULONG	VREV	:1;	/**< VREV: Vertical reversing display selection.
								 <ul><li>@ref D_IM_DISP_REV_NORMAL
									 <li>@ref D_IM_DISP_REV_REVERSE</ul>		*/
		ULONG			:23;/**< Reserve										*/
	} bit;	/**< Bit field */
} U_IM_DISP_LREVDISP;

/**	TOCTL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CON		:1;	/**< CON: Connection destination selection.
								 <ul><li>@ref D_IM_DISP_CON_EXTERNAL
									 <li>@ref D_IM_DISP_CON_INTERNAL</ul>		*/
		ULONG			:7;	/**< Reserve										*/
		ULONG	CKOEN	:1;	/**< CKOEN: Output clock oscillation permission (for external output)
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	CKICEN	:1;	/**< CKICEN: Output clock oscillation permission (for internal connection)
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG			:6;	/**< Reserve										*/
		ULONG	WBEN	:1;	/**< WBEN: Write back permission
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG			:15;/**< Reserve										*/
	} bit;	/**< Bit field */
} U_IM_DISP_TOCTL;

/**	INTE register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	LEE		:1;	/**< LEE: Main data input block forwarding error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	GR0EE	:1;	/**< GR0EE: OSD data 0 input block forwarding error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	GR1EE	:1;	/**< GR1EE: OSD data 1 input block forwarding error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	GA0EE	:1;	/**< GR0EE: OSD data 0 input block A component forwarding error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	GA1EE	:1;	/**< GR1EE: OSD data 1 input block A component forwarding error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	LREE	:1;	/**< LREE: Main data input block AXI error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	GR0REE	:1;	/**< GR0REE: OSD data 0 input block AXI error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	GR1REE	:1;	/**< GR1REE: OSD data 1 input block AXI error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	GA0REE	:1;	/**< GR0REE: OSD data 0 input block A component AXI error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	GA1REE	:1;	/**< GR1REE: OSD data 1 input block A component AXI error interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG	VE		:1;	/**< VE: Vsync detection interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	VE2		:1;	/**< VE: Vsync detection interruption permission.
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>			*/
		ULONG			:11;/**< Reserve										*/
	} bit;	/**< Bit field */
} U_IM_DISP_INTE;

/**	POLSEL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VDPS	:1;	/**< VDPS: Vsync polarity selection.
								 <ul><li>@ref D_IM_DISP_POLARITY_POSITIVE
									 <li>@ref D_IM_DISP_POLARITY_NEGATIVE</ul>	*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	HDPS	:1;	/**< HDPS: Hsync polarity selection.
								 <ul><li>@ref D_IM_DISP_POLARITY_POSITIVE
									 <li>@ref D_IM_DISP_POLARITY_NEGATIVE</ul>	*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	VEPS	:1;	/**< VEPS: Vertical enable signal polarity selection.
								 <ul><li>@ref D_IM_DISP_POLARITY_POSITIVE
									 <li>@ref D_IM_DISP_POLARITY_NEGATIVE</ul>	*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	HEPS	:1;	/**< HEPS: Horizontal enable signal polarity selection.
								 <ul><li>@ref D_IM_DISP_POLARITY_POSITIVE
									 <li>@ref D_IM_DISP_POLARITY_NEGATIVE</ul>	*/
		ULONG			:3;	/**< Reserve										*/
		ULONG	CKEG	:1;	/**< CKEG: Output clock edge selection.
								 <ul><li>@ref D_IM_DISP_CKEG_FALL
									 <li>@ref D_IM_DISP_CKEG_RISE</ul>			*/
		ULONG			:15;/**< Reserve										*/
	} bit;	/**< Bit field */
} U_IM_DISP_POLSEL;

/**	VCYC register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VCYC1	:14;/**< VCYC1: Vertical cycle 1.(Setting at vertical cycle of the 1st field.)<br>
								 Value range:0~16383<br>
								 Progressive : VCYC1 >= VBLK.VBLK1 + OVSIZE.OVSIZE + 1.<br>
								 Interlace : VCYC1 >= VBLK.VBLK1 + OVSIZE.OVSIZE / 2 + 1.					*/
		ULONG			:2;	/**< Reserve																	*/
		ULONG	VCYC2	:14;/**< VCYC2: Vertical cycle 2.(Setting at vertical cycle of the 2nd field.)<br>
								 Value range:0~16383<br>
								 Progressive : VCYC2 >= VBLK.VBLK2 + OVSIZE.OVSIZE + 1.<br>
								 Interlace : VCYC2 >= VBLK.VBLK2 + OVSIZE.OVSIZE / 2 + 1.					*/
		ULONG			:1;	/**< Reserve																	*/
		ULONG	VDSFT	:1;	/**< VDSFT: Vsync output 0.5H shift selection.
								 <ul><li>@ref D_IM_DISP_VDSFT_NORMAL
									 <li>@ref D_IM_DISP_VDSFT_SHIFT</ul>									*/
	} bit;	/**< Bit field */
} U_IM_DISP_VCYC;

/**	OVPW register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	OVPW	:10;/**< VPW: Vsync pluse width.<br>
								 Value range:1~1023							*/
		ULONG			:6;	/**< Reserve									*/
		ULONG	OVPWU	:1;	/**< VPWU: Vsync pluse width unit selection.
								 <ul><li>@ref D_IM_DISP_OVPWU_CLOCK_UNIT
									 <li>@ref D_IM_DISP_OVPWU_LINE_UNIT</ul>	*/
		ULONG			:15;/**< Reserve									*/
	} bit;	/**< Bit field */
} U_IM_DISP_OVPW;

/**	VBLK register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VBLK1	:8;	/**< VBLK1: Period of vertical blank.(1st field)<br>
								 Value range:1~255								*/
		ULONG	VBLK2	:8;	/**< VBLK2: Period of vertical blank.(2nd field)<br>
								 Value range:1~255								*/
		ULONG			:16;/**< Reserve										*/
	} bit;	/**< Bit field */
} U_IM_DISP_VBLK;

/**	DOMD register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	MODE	:8;	/**< MODE: Output mode.
								 <ul><li>@ref D_IM_DISP_MODE_RGB888_24BIT
									 <li>@ref D_IM_DISP_MODE_RGB888_30BIT
									 <li>@ref D_IM_DISP_MODE_RGB888_36BIT
									 <li>@ref D_IM_DISP_MODE_RGB888_8BIT_1
									 <li>@ref D_IM_DISP_MODE_RGB888_8BIT_2
									 <li>@ref D_IM_DISP_MODE_RGB888_8BIT_3
									 <li>@ref D_IM_DISP_MODE_RGB888_16BIT_1
									 <li>@ref D_IM_DISP_MODE_RGB888_16BIT_2
									 <li>@ref D_IM_DISP_MODE_RGB565_8BIT
									 <li>@ref D_IM_DISP_MODE_RGB565_16BIT
									 <li>@ref D_IM_DISP_MODE_RGB666_18BIT
									 <li>@ref D_IM_DISP_MODE_YCC422_8BIT
									 <li>@ref D_IM_DISP_MODE_YCC422_16BIT
									 <li>@ref D_IM_DISP_MODE_YCC444_24BIT
									 <li>@ref D_IM_DISP_MODE_YCC422_20BIT
									 <li>@ref D_IM_DISP_MODE_YCC422_24BIT
									 <li>@ref D_IM_DISP_MODE_DEEP_YCC444_30BIT
									 <li>@ref D_IM_DISP_MODE_DEEP_YCC444_36BIT
									 <li>@ref D_IM_DISP_MODE_DUAL_YCC444_12BIT
									 <li>@ref D_IM_DISP_MODE_YCC420_24BIT
									 <li>@ref D_IM_DISP_MODE_YCC420_30BIT
									 <li>@ref D_IM_DISP_MODE_GENERAL_8BIT
									 <li>@ref D_IM_DISP_MODE_GENERAL_16BIT
									 <li>@ref D_IM_DISP_MODE_GENERAL_10BIT_1
									 <li>@ref D_IM_DISP_MODE_GENERAL_10BIT_2</ul>	*/
		ULONG			:8;	/**< Reserve											*/
		ULONG	OBPSL	:1;	/**< OBPSL: Output bit position selection.
								 <ul><li>@ref D_IM_DISP_OBPSL_LSB
									 <li>@ref D_IM_DISP_OBPSL_MSB</ul>				*/
		ULONG			:7;	/**< Reserve											*/
		ULONG	OBUSL	:2;	/**< OBUSL: Output bit unit selection.
								 <ul><li>@ref D_IM_DISP_OBUSL_8BIT
									 <li>@ref D_IM_DISP_OBUSL_12BIT
									 <li>@ref D_IM_DISP_OBUSL_16BIT</ul>			*/
		ULONG			:6;	/**< Reserve											*/
	} bit;	/**< Bit field */
} U_IM_DISP_DOMD;

/**	FODATA register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FOD_0	:8;	/**< FOD_0: R (or Y)<br>
								 Value range:0~255	*/
		ULONG	FOD_1	:8;	/**< FOD_1: G (or Cb)<br>
								 Value range:0~255	*/
		ULONG	FOD_2	:8;	/**< FOD_2: B (or Cr)<br>
								 Value range:0~255	*/
		ULONG			:8;	/**< Reserve			*/
	} bit;	/**< Bit field */
} U_IM_DISP_FODATA;

/**	BLDCTL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	BLD1	:2;		/**< BLD1: Blend order 1 layer selection.<br>
									 <ul><li>@ref D_IM_DISP_BLD_GRID
										 <li>@ref D_IM_DISP_BLD_FACE
										 <li>@ref D_IM_DISP_BLD_OSD0
										 <li>@ref D_IM_DISP_BLD_OSD1</ul>		*/
		ULONG			:2;		/**< Reserve									*/
		ULONG	BLD2	:2;		/**< BLD2: Blend order 2 layer selection.<br>
									 <ul><li>@ref D_IM_DISP_BLD_GRID
										 <li>@ref D_IM_DISP_BLD_FACE
										 <li>@ref D_IM_DISP_BLD_OSD0
										 <li>@ref D_IM_DISP_BLD_OSD1</ul>		*/
		ULONG			:2;		/**< Reserve									*/
		ULONG	BLD3	:2;		/**< BLD3: Blend order 3 layer selection.<br>
									 <ul><li>@ref D_IM_DISP_BLD_GRID
										 <li>@ref D_IM_DISP_BLD_FACE
										 <li>@ref D_IM_DISP_BLD_OSD0
										 <li>@ref D_IM_DISP_BLD_OSD1</ul>		*/
		ULONG			:2;		/**< Reserve									*/
		ULONG	BLD4	:2;		/**< BLD4: Blend order 4 layer selection.<br>
									 <ul><li>@ref D_IM_DISP_BLD_GRID
										 <li>@ref D_IM_DISP_BLD_FACE
										 <li>@ref D_IM_DISP_BLD_OSD0
										 <li>@ref D_IM_DISP_BLD_OSD1</ul>		*/
		ULONG			:18;	/**< Reserve									*/
	} bit;	/**< Bit field */
} U_IM_DISP_BLDCTL;

/**	YCAL register structure. */
typedef struct {
	ULONG	ygain;		/**< YGAIN, CBGAIN, CRGAIN: Gain value.(Y)<br>
							 Value range:0,1~131071(0.000015~1.999985)<br>
							 Gain value is 2(=131072/65536) when a GAIN value is 0.	*/
	ULONG	yofs;		/**< YOFS: Offset value.(Y)<br>
							 In case of YOFS<br>
							 Value range:0~8191<br>									*/
} T_IM_DISP_YCAL;

/**	CBCAL, CRCAL register structure. */
typedef struct {
	USHORT	cgain;		/**< CBGAIN, CRGAIN: Gain value.(Cb or Cr)<br>
							 Value range:0,1~65565(0.00003~1.99997)<br>
							 Gain value is 2(=65536/32768) when a GAIN value is 0. */
	SHORT	cofs;		/**< CBOFS, CROFS: Offset value.(Cb or Cr)<br>
							 In case of CBOFS, CROFS<br>
							 Value range:-8192~8191<br>							*/
} T_IM_DISP_CCAL;

/**	YCLIP, CBCLIP, CRCLIP register structure. */
typedef struct {
	USHORT	cph;		/**< YCPH, CBCPH, CRCPH: Clip upper bound value.<br>
							 Value range:0x0000~0xFFFF<br>
							 0xFFFF is set up when clip processing is not performed.	*/
	USHORT	cpl;		/**< YCPL, CBCPL, CRCPL: Clip lower bound value.<br>
							 Value range:0x0000~0xFFFF<br>
							 0x0000 is set up when clip processing is not performed.	*/
} T_IM_DISP_CLIP;

/**	DOCTL0 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	DTO1	:2;	/**< DTO1: Data output order selection 1.
								 <ul><li>@ref D_IM_DISP_DTO_R_Y0
									 <li>@ref D_IM_DISP_DTO_G_CB
									 <li>@ref D_IM_DISP_DTO_B_CR
									 <li>@ref D_IM_DISP_DTO_0_Y1</ul>	*/
		ULONG			:2;	/**< Reserve								*/
		ULONG	DTO2	:2;	/**< DTO2: Data output order selection 2.
								 <ul><li>@ref D_IM_DISP_DTO_R_Y0
									 <li>@ref D_IM_DISP_DTO_G_CB
									 <li>@ref D_IM_DISP_DTO_B_CR
									 <li>@ref D_IM_DISP_DTO_0_Y1</ul>	*/
		ULONG			:2;	/**< Reserve								*/
		ULONG	DTO3	:2;	/**< DTO3: Data output order selection 3.
								 <ul><li>@ref D_IM_DISP_DTO_R_Y0
									 <li>@ref D_IM_DISP_DTO_G_CB
									 <li>@ref D_IM_DISP_DTO_B_CR
									 <li>@ref D_IM_DISP_DTO_0_Y1</ul>	*/
		ULONG			:2;	/**< Reserve								*/
		ULONG	DTO4	:2;	/**< DTO4: Data output order selection 4.
								 <ul><li>@ref D_IM_DISP_DTO_R_Y0
									 <li>@ref D_IM_DISP_DTO_G_CB
									 <li>@ref D_IM_DISP_DTO_B_CR
									 <li>@ref D_IM_DISP_DTO_0_Y1</ul>	*/
		ULONG			:2;	/**< Reserve								*/
		ULONG	DTO5	:2;	/**< DTO5: Data output order selection 5.
								 <ul><li>@ref D_IM_DISP_DTO_R_Y0
									 <li>@ref D_IM_DISP_DTO_G_CB
									 <li>@ref D_IM_DISP_DTO_B_CR
									 <li>@ref D_IM_DISP_DTO_0_Y1</ul>	*/
		ULONG			:2;	/**< Reserve								*/
		ULONG	DTO6	:2;	/**< DTO6: Data output order selection 6.
								 <ul><li>@ref D_IM_DISP_DTO_R_Y0
									 <li>@ref D_IM_DISP_DTO_G_CB
									 <li>@ref D_IM_DISP_DTO_B_CR
									 <li>@ref D_IM_DISP_DTO_0_Y1</ul>	*/
		ULONG			:10;/**< Reserve								*/
	} bit;	/**< Bit field */
} U_IM_DISP_DOCTL0;

/**	DOCTL1 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	SOO		:1;	/**< SOO: 0.5pix shift output (Odd line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG			:3;	/**< Reserve											*/
		ULONG	SOE		:1;	/**< SOE: 0.5pix shift output (Even line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG			:3;	/**< Reserve											*/
		ULONG	ODO0	:1;	/**< ODO0: Data output delay selection 0 (Odd line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG	ODO1	:1;	/**< ODO1: Data output delay selection 1 (Odd line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG	ODO2	:1;	/**< ODO2: Data output delay selection 2 (Odd line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG			:1;	/**< Reserve											*/
		ULONG	ODE0	:1;	/**< ODE0: Data output delay selection 0 (Even line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG	ODE1	:1;	/**< ODE1: Data output delay selection 1 (Even line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG	ODE2	:1;	/**< ODE2: Data output delay selection 2 (Even line)<br>
								 <ul><li>@ref D_IM_DISP_ENABLE_OFF
									 <li>@ref D_IM_DISP_ENABLE_ON</ul>				*/
		ULONG			:17;/**< Reserve											*/
	} bit;	/**< Bit field */
} U_IM_DISP_DOCTL1;

/**	DOCTL2 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	CLVS	:1;	/**< CLVS: CbCr data output level selection.<br>
								 <ul><li>@ref D_IM_DISP_CLVS_NORMAL
									 <li>@ref D_IM_DISP_CLVS_SIGNED</ul>	*/
		ULONG			:3;	/**< Reserve									*/
		ULONG	CTOH	:2;	/**< CTOH: Horizontal CbCr  off selection.<br>
								 <ul><li>@ref D_IM_DISP_CTOH_LEFT
									 <li>@ref D_IM_DISP_CTOH_RIGHT
									 <li>@ref D_IM_DISP_CTOH_AVERAGE
									 <li>@ref D_IM_DISP_CTOH_FILTER</ul>	*/
		ULONG			:2;	/**< Reserve									*/
		ULONG	CTOV	:1;	/**< CTOV: Vertical CbCr  off selection.<br>
								 <ul><li>@ref D_IM_DISP_CTOV_TOP
									 <li>@ref D_IM_DISP_CTOV_AVERAGE</ul>	*/
		ULONG			:7;	/**< Reserve									*/
		ULONG	TRSOS	:2;	/**< TRSOS: SAV/EAV code output selection.<br>
								 <ul><li>@ref D_IM_DISP_TRSOS_NONE
									 <li>@ref D_IM_DISP_TRSOS_15_8
									 <li>@ref D_IM_DISP_TRSOS_7_0
									 <li>@ref D_IM_DISP_TRSOS_BOTH</ul>		*/
		ULONG			:14;/**< Reserve									*/
	} bit;	/**< Bit field */
} U_IM_DISP_DOCTL2;

/**	TRSCODE0, TRSCODE1 register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	EAV0	:8;	/**< EAV0: EAV code 00 (or 10)<br>
								 Value range:0~255				*/
		ULONG	SAV0	:8;	/**< SAV0: SAV code 00 (or 10)<br>
								 Value range:0~255				*/
		ULONG	EAV1	:8;	/**< EAV1: EAV code 01 (or 11)<br>
								 Value range:0~255				*/
		ULONG	SAV1	:8;	/**< SAV1: SAV code 01 (or 11)<br>
								 Value range:0~255				*/
	} bit;	/**< Bit field */
} U_IM_DISP_TRSCODE;

/**	TBLASET register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GMTAEN	:1;	/**< GMTAEN: Gamma correction access enable.<br>
								 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
									 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>				*/
		ULONG	GMEN	:1;	/**< GMEN: Gamma correction enable.<br>
								 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
									 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>				*/
		ULONG	GMTSL	:1;	/**< GMTSL: Gamma correction table surface.<br>
								 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
									 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>					*/
		ULONG			:5;	/**< Reserve														*/
		ULONG	YSATTA	:1;	/**< YSATTA: Luminance and chroma correction access enable.<br>
								 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
									 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>				*/
		ULONG	YSATEN	:1;	/**< YSATEN: Luminance and chroma correction enable.<br>
								 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
									 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>				*/
		ULONG	YSATSL	:1;	/**< YSATSL: Luminance and chroma correction table surface.<br>
								 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
									 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>					*/
		ULONG			:5;	/**< Reserve														*/
		ULONG	GMTMN	:1;	/**< GMTMN: Gamma correction table monitor.<br>
								 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
									 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>					*/
		ULONG			:7;	/**< Reserve														*/
		ULONG	YSATMN	:1;	/**< YSATMN:  Luminance and chroma correction table monitor.<br>
								 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
									 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>					*/
		ULONG			:7;	/**< Reserve														*/
	} bit;	/**< Bit field */
} U_IM_DISP_TBLASET;

/**	GLENGTH register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GHLEN	:16;	/**< GHLEN: Horizontal grid line length.<br>
									 Value range:8~65534 (2-pixel unit)		*/
		ULONG	GVLEN	:14;	/**< GVLEN: Vertical grid line length.<br>
									 Value range:8~16382 (2-pixel unit)		*/
		ULONG			:2;		/**< Reserve								*/
	} bit;	/**< Bit field */
} U_IM_DISP_GLENGTH;

/**	GWIDTH register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GHWID	:6;		/**< GHWID: Horizontal grid line width.<br>
									 Value range:2~62 (2-pixel unit)		*/
		ULONG			:10;	/**< Reserve								*/
		ULONG	GVWID	:6;		/**< GVWID: Vertical grid line width.<br>
									 Value range:2~62 (2-pixel unit)		*/
		ULONG			:10;	/**< Reserve								*/
	} bit;	/**< Bit field */
} U_IM_DISP_GWIDTH;

/**	GITVL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GHITV	:14;	/**< GHITV: Horizontal grid line interval.<br>
									 Value range:4~16382 (2-pixel unit)<br>
									 GHITV >= GHWID + 2							*/
		ULONG			:2;		/**< Reserve									*/
		ULONG	GVITV	:16;	/**< GVITV: Vertical grid line interval.<br>
									 Value range:4~65534 (2-pixel unit)<br>
									 GVITV >= GVWID + 2							*/
	} bit;	/**< Bit field */
} U_IM_DISP_GITVL;

/**	GNUM register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GHNUM	:3;		/**< GHNUM: Number of horizontal grid line.<br>
									 Value range:0~7<br>
									 Horizontal grid line num = GHNUM + 1		*/
		ULONG			:13;	/**< Reserve									*/
		ULONG	GVNUM	:3;		/**< GVNUM: Number of vertical grid line.<br>
									 Value range:0~7<br>
									 Vertical grid line num = GVNUM + 1			*/
		ULONG			:13;	/**< Reserve									*/
	} bit;	/**< Bit field */
} U_IM_DISP_GNUM;

/**	GDCTL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	GCLRR	:8;	/**< GCLRR: Grid line display color R.<br>
								 Value range:0~255								*/
		ULONG	GCLRG	:8;	/**< GCLRG: Grid line display color G.<br>
								 Value range:0~255								*/
		ULONG	GCLRB	:8;	/**< GCLRB: Grid line display color B.<br>
								 Value range:0~255								*/
		ULONG	GALP	:2;	/**< GALP: Grid line alpha value setting.<br>
								 GALP=00b:grid line=25%, low rank layer=75%<br>
								 GALP=01b:grid line=50%, low rank layer=50%<br>
								 GALP=10b:grid line=75%, low rank layer=25%<br>
								 GALP=11b:grid line=100%, low rank layer=0%		*/
		ULONG			:6;	/**< Reserve										*/
	}bit;	/**< Bit field */
} U_IM_DISP_GDCTL;

/**	FFSIZE register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FFHSZ	:16;	/**< FFHSZ: Face frame horizontal size.<br>
									 Value range:8~65534 (2-pixel unit)		*/
		ULONG	FFVSZ	:14;	/**< FFVSZ: Face frame vertical size.<br>
									 Value range:8~16382 (2-pixel unit)		*/
		ULONG			:2;		/**< Reserve								*/
	}bit;	/**< Bit field */
} U_IM_DISP_FFSIZE;

/**	FFWIDTH register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FFHWID	:6;		/**< FFHSZ: Horizontal face frame width.<br>
									 Value range:2~62 (2-pixel unit)		*/
		ULONG			:10;	/**< Reserve								*/
		ULONG	FFVWID	:6;		/**< FFHSZ: Vertical face frame width.<br>
									 Value range:2~62 (2-pixel unit)		*/
		ULONG			:10;	/**< Reserve								*/
	}bit;	/**< Bit field */
} U_IM_DISP_FFWIDTH;

/**	FFCLR register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	FFCLRR	:8;	/**< FFCLRR: Face frame display color R.<br>
								 Value range:0~255						*/
		ULONG	FFCLRG	:8;	/**< FFCLRG: Face frame display color G.<br>
								 Value range:0~255						*/
		ULONG	FFCLRB	:8;	/**< FFCLRB: Face frame display color B.<br>
								 Value range:0~255						*/
		ULONG			:8;	/**< Reserve								*/
	}bit;	/**< Bit field */
} U_IM_DISP_FFCLR;

/**	GRIDT register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	IFMT	:3;	/**< IFMT: Input format selection.<br>
								 <ul><li>@ref D_IM_DISP_GRIDT_IFMT_RGBA8888
								     <li>@ref D_IM_DISP_GRIDT_IFMT_YCC422
								     <li>@ref D_IM_DISP_GRIDT_IFMT_RGBA4444
								     <li>@ref D_IM_DISP_GRIDT_IFMT_RGB565
									 <li>@ref D_IM_DISP_GRIDT_IFMT_RGBA5551
									 <li>@ref D_IM_DISP_GRIDT_IFMT_YCC422_A4
									 <li>@ref D_IM_DISP_GRIDT_IFMT_YCC422_A8</ul>								*/
		ULONG			:1;	/**< Reserve																		*/
		ULONG	NBT		:2;	/**< NBT: Burst size selection.<br>
								 <ul><li>@ref D_IM_DISP_NBT_128BYTE
									 <li>@ref D_IM_DISP_NBT_256BYTE</ul>										*/
		ULONG			:10;/**< Reserve																		*/
		ULONG	CACHE	:4;	/**< CACHE: Cache type.<br>
								 d[0]:Bufferable bit. Value 0:Nonbufferable, 1:Bufferable.<br>
								 d[1]:Cacheable bit. Value 0:Noncacheable, 1:Cacheable.<br>
								 d[2]:Read allocate bit. Value 0:Read through, 1:Allocate on Reads.<br>
								 d[3]:Write allocate bit. Value 0:Write through, 1:Allocate on writes.<br>		*/
		ULONG	PROT	:3;	/**< PROT: Protect type.<br>
								 d[0]:Normal or privileged. Value 0:Normal access, 1:Privileged access.<br>
								 d[1]:Secure or non-secure. Value 0:Secure access, 1:Non-secure access.<br>
								 d[2]:Instruction or Data. Value 0:Data access, 1:Instrauction access.<br>		*/
		ULONG			:1;	/**< Reserve																		*/
		ULONG	SLVSL	:1;	/**< SLVSL: Master interface selection.<br> D_IM_DISP_SLVSL_MASTER_INTERFACE_0
								 <ul><li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_0
									 <li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_1</ul>							*/
		ULONG	ASLVSL	:1;	/**< ASLVSL: A component master interface selection.<br>
								 If format is YCbCr422 A4 surface or YCbCr422 A8 surface, this bit is enable.<br>
								 <ul><li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_0
									 <li>@ref D_IM_DISP_SLVSL_MASTER_INTERFACE_1</ul>							*/
		ULONG			:2;	/**< Reserve																		*/
		ULONG	IFBTMU	:1;	/**< IFBTMU: Input format bottom-up selection.<br>
								 <ul><li>@ref D_IM_DISP_IFBTMU_UP_TO_BOTTOM
									 <li>@ref D_IM_DISP_IFBTMU_BOTTOM_TO_UP</ul>								*/
		ULONG			:3;	/**< Reserve																		*/
	} bit;	/**< Bit field */
} U_IM_DISP_GRIDT;

/**	GRIPO register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	IPO1	:2;	/**< IPO1: Input data arrangement order 1 setting.<br>
								 <ul><li>@ref D_IM_DISP_IPO_RGBA8888_R
									 <li>@ref D_IM_DISP_IPO_RGBA8888_G
									 <li>@ref D_IM_DISP_IPO_RGBA8888_B
									 <li>@ref D_IM_DISP_IPO_RGBA8888_A
									 <li>@ref D_IM_DISP_IPO_YCC422_CB
									 <li>@ref D_IM_DISP_IPO_YCC422_Y0
									 <li>@ref D_IM_DISP_IPO_YCC422_CR
									 <li>@ref D_IM_DISP_IPO_YCC422_Y1
									 <li>@ref D_IM_DISP_IPO_RGBA4444_B0_A0
									 <li>@ref D_IM_DISP_IPO_RGBA4444_R0_G0
									 <li>@ref D_IM_DISP_IPO_RGBA4444_B1_A1
									 <li>@ref D_IM_DISP_IPO_RGBA4444_R1_G1
									 <li>@ref D_IM_DISP_IPO_RGB565_G0_B0
									 <li>@ref D_IM_DISP_IPO_RGB565_R0_G0
									 <li>@ref D_IM_DISP_IPO_RGB565_G1_B1
									 <li>@ref D_IM_DISP_IPO_RGB565_R1_G1
									 <li>@ref D_IM_DISP_IPO_RGBA5551_G0_B0_A0
									 <li>@ref D_IM_DISP_IPO_RGBA5551_R0_G0
									 <li>@ref D_IM_DISP_IPO_RGBA5551_G1_B1_A1
									 <li>@ref D_IM_DISP_IPO_RGBA5551_R1_G1</ul>		*/
		ULONG			:6;	/**< Reserve											*/
		ULONG	IPO2	:2;	/**< IPO2: Input data arrangement order 2 setting.<br>
								 The value that can be selected is the same as IPO1.*/
		ULONG			:6;	/**< Reserve											*/
		ULONG	IPO3	:2;	/**< IPO3: Input data arrangement order 3 setting.<br>
								 The value that can be selected is the same as IPO1.*/
		ULONG			:6;	/**< Reserve											*/
		ULONG	IPO4	:2;	/**< IPO4: Input data arrangement order 4 setting.<br>
								 The value that can be selected is the same as IPO1.*/
		ULONG			:6;	/**< Reserve											*/
	} bit;	/**< Bit field */
} U_IM_DISP_GRIPO;

/**	GRBLINK register structure. */
typedef union {
	ULONG		word[5];	/**< All bits */
	struct {
		USHORT	BTIMH_0	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_0	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_1	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_1	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_2	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_2	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_3	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_3	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_4	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_4	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_5	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_5	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_6	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_6	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_7	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_7	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_8	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_8	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIMH_9	:6;	/**< BTIMH: Setting at blink display period.<br>
								 Value range:0~63<br>
								 BTIMH = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
		USHORT	BTIML_9	:6;	/**< BTIML: Setting at blink non-display period.<br>
								 Value range:0~63<br>
								 BTIML = frame cout - 1.						*/
		USHORT			:2;	/**< Reserve										*/
	}bit;	/**< Bit field */
} U_IM_DISP_GRBLINK;

/** Image color. */
typedef union {
	ULONG word;	/**< All bits */
	struct {
		ULONG	r	:8;		/**< R<br> Value range:0~255	*/
		ULONG	g	:8;		/**< G<br> Value range:0~255	*/
		ULONG	b	:8;		/**< R<br> Value range:0~255	*/
		ULONG		:8;		/**< Reserve					*/
	} rgb;	/**< bit field of rgb */
	struct {
		ULONG	y	:8;		/**< Y<br> Value range:0~255	*/
		ULONG	cb	:8;		/**< Cb<br> Value range:0~255	*/
		ULONG	cr	:8;		/**< Cr<br> Value range:0~255	*/
		ULONG		:8;		/**< Reserve					*/
	} ycc;	/**< bit field of ycc */
} U_IM_DISP_IMAGE_COLOR;

/** Image color(16 bit). */
typedef union {
	unsigned long	word[2];	/**< All bits */
	struct {
		ULONG	r	:16;	/**< R<br> Value range:0~65535	*/
		ULONG	g	:16;	/**< G<br> Value range:0~65535	*/
		ULONG	b	:16;	/**< R<br> Value range:0~65535	*/
		ULONG		:16;	/**< Reserve					*/
	} rgb;	/**< bit field of rgb */
	struct {
		ULONG	y	:16;	/**< Y<br> Value range:0~65535	*/
		ULONG	cb	:16;	/**< Cb<br> Value range:0~65535	*/
		ULONG	cr	:16;	/**< Cr<br> Value range:0~65535	*/
		ULONG		:16;	/**< Reserve					*/
	} ycc;	/**< bit field of ycc */
} U_IM_DISP_IMAGE_COLOR16;

/** Channel input buffer size. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	width	:16;	/**< Horizontal size<br>
									 Value range:<br>
									 LISIZE.IHSIZE=128~65534 (2-byte unit)<br>
									 GRTISIZE.TIHSIZE=8~65534 (2-byte unit)<br>
									 GRISIZE.IHSIZE=8~65532 (4-byte unit)		*/
		ULONG	lines	:14;	/**< Vertical size<br>
									 Value range:<br>
									 LISIZE.IVSIZE=16~16382 (2-byte unit)<br>
									 GRTISIZE.TIVSIZE=8~16382 (2-byte unit)<br>
									 GRISIZE.IVSIZE=8~16382 (2-byte unit)		*/
		ULONG			:2;		/**< Reserve									*/
	}size;	/**< bit field */
} U_IM_DISP_SIZE;

/** Matrix coefficient (YCbCr <-> RGB). */
typedef union {
	LLONG		dword;	/**< All bits */
	struct {
		LLONG	COEFFICIENT0	:16;	/**< Matrix coefficient 0<br>
											 Value range:-128~127(-2.0~1.984)<br>
											 Regiser:<br>
											 LY2R0.YR00/LY2R1.YR10/LY2R2.YR20<br>
											 R2Y0.RY00/R2Y1.RY10/R2Y2.RY20<br>
											 Y2R0.YR00/Y2R1.YR10/Y2R2.YR20			*/
		LLONG	COEFFICIENT1	:16;	/**< Matrix coefficient 1<br>
											 Value range:-128~127(-2.0~1.984)<br>
											 Regiser:<br>
											 LY2R0.YR01/LY2R1.YR11/LY2R2.YR21<br>
											 R2Y0.RY01/R2Y1.RY11/R2Y2.RY21<br>
											 Y2R0.YR01/Y2R1.YR11/Y2R2.YR21			*/
		LLONG	COEFFICIENT2	:16;	/**< Matrix coefficient 2<br>
											 Value range:-128~127(-2.0~1.984)<br>
											 Regiser:<br>
											 LY2R0.YR02/LY2R1.YR12/LY2R2.YR22<br>
											 R2Y0.RY02/R2Y1.RY12/R2Y2.RY22<br>
											 Y2R0.YR02/Y2R1.YR12/Y2R2.YR22			*/
		LLONG					:16;	/**< Reserve								*/
	}bit;	/**< Bit field */
}U_IM_DISP_YR_MATRIX_COEFFICIENT;

/** LCC0/LCC1/LCC2. */
typedef union {
	LLONG		dword;	/**< All bits */
	struct {
		LLONG	CC0		:12;	/**< CC00/: Color space conversion matrix coefficient 0.<br>
									 Value range:-128~127(-2.0~1.984)						*/
		LLONG			:4;		/**< Reserve												*/
		LLONG	CC1		:12;	/**< CC1: Color space conversion matrix coefficient 1.<br>
									 Value range:-128~127(-2.0~1.984)						*/
		LLONG			:4;		/**< Reserve												*/
		LLONG	CC2		:12;	/**< CC2: Color space conversion matrix coefficient 2.<br>
									 Value range:-128~127(-2.0~1.984)						*/
		LLONG			:20;	/**< Reserve												*/
	}bit;	/**< Bit field */
}U_IM_DISP_LCC;

/** GRY2R0/GRY2R1/GRY2R2. */
typedef union {
	LONG		word;	/**< All bits */
	struct {
		LONG	YR0		:8;	/**< YR00/YR10/YR20: YCbCr -> RGB matrix coefficient 0.<br>
								 Value range:-128~127(-2.0~1.984)						*/
		LONG	YR1		:8;	/**< YR01/YR11/YR21: YCbCr -> RGB matrix coefficient 0.<br>
								 Value range:-128~127(-2.0~1.984)						*/
		LONG	YR2		:8;	/**< YR02/YR12/YR22: YCbCr -> RGB matrix coefficient 0.<br>
								 Value range:-128~127(-2.0~1.984)						*/
		LONG			:8;	/**< Reserve												*/
	}bit;	/**< Bit field */
}U_IM_DISP_GRY2R;

/**	AXISTS	*/
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	LRESP		:2;		/**< Main response<br>
										 Value:<br>
										 00b:Normal access success or Exclusive failure.<br>
										 01b:Exclusive access.<br>
										 10b:Slave error.<br>
										 11b:Decode error										*/
		ULONG				:14;	/**< Reserve												*/
		ULONG	GR0RESP		:2;		/**< GR0's response<br>
										 The value that can be selected is the same as LRESP.	*/
		ULONG				:2;		/**< Reserve												*/
		ULONG	GR1RESP		:2;		/**< GR1's response<br>
										 The value that can be selected is the same as LRESP.	*/
		ULONG				:2;		/**< Reserve												*/
		ULONG	GRA0RESP	:2;		/**< GR0's A component response<br>
										 The value that can be selected is the same as LRESP.	*/
		ULONG				:2;		/**< Reserve												*/
		ULONG	GRA1RESP	:2;		/**< GR1's A component response<br>
										 The value that can be selected is the same as LRESP.	*/
		ULONG				:2;		/**< Reserve												*/
	}AXI_Rep;	/**< Bit field */
}U_IM_DISP_AXISTS;

/** DBGSL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	VDMSL	:1;		/**< VDMSL: Vertical sync signal selection for debugging.<br>
									 Value 0:HDMI, 1:MIPI-DSI										*/
		ULONG			:3;		/**< Reserve														*/
		ULONG	HDMSL	:1;		/**< HDMSL: Horizontal sync signal selection for debugging.<br>
									 Value 0:HDMI, 1:MIPI-DSI										*/
		ULONG			:3;		/**< Reserve														*/
		ULONG	HDEMSL	:1;		/**< HDEMSL: Horizontal enable signal selection for debugging.<br>
									 Value 0:HDMI, 1:MIPI-DSI										*/
		ULONG			:23;	/**< Reserve														*/
	}bit;	/**< Bit field */
}U_IM_DISP_DBGSL;

/** PWAXCTL register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	AWCACHE	:4;	/**< AWCACHE: Cache type.<br>
								 d[0]:Bufferable bit. Value 0:Nonbufferable, 1:Bufferable.<br>
								 d[1]:Cacheable bit. Value 0:Noncacheable, 1:Cacheable.<br>
								 d[2]:Read allocate bit. Value 0:Read through, 1:Allocate on Reads.<br>
								 d[3]:Write allocate bit. Value 0:Write through, 1:Allocate on writes.<br>	*/
		ULONG			:4;	/**< Reserve																	*/
		ULONG	AWPROT	:3;	/**< AWPROT: Protect type.<br>
								 d[0]:Normal or privileged. Value 0:Normal access, 1:Privileged access.<br>
								 d[1]:Secure or non-secure. Value 0:Secure access, 1:Non-secure access.<br>
								 d[2]:Instruction or Data. Value 0:Data access, 1:Instrauction access.<br>	*/
		ULONG			:21;	/**< Reserve																*/
	}bit;	/**< Bit field */
}U_IM_DISP_PWAXCTL;

/** PWCHINTENB register structure. */
typedef union {
	ULONG		word;	/**< All bits */
	struct {
		ULONG	PWE		:1;		/**< PWE: PWCH successful completion interrupt request permission.<br>
									 <ul><li>@ref D_IM_DISP_ENABLE_OFF
										 <li>@ref D_IM_DISP_ENABLE_ON</ul>								*/
		ULONG			:3;		/**< Reserve															*/
		ULONG	PWEE	:1;		/**< PWEE: PWCH transmission error interrupt request permission.<br>
									 <ul><li>@ref D_IM_DISP_ENABLE_OFF
										 <li>@ref D_IM_DISP_ENABLE_ON</ul>								*/
		ULONG			:3;		/**< Reserve															*/
		ULONG	PWXE	:1;		/**< PWXE: PWCH AXI error interrupt request permission.<br>
									 <ul><li>@ref D_IM_DISP_ENABLE_OFF
										 <li>@ref D_IM_DISP_ENABLE_ON</ul>								*/
		ULONG			:23;	/**< Reserve															*/
	}bit;	/**< Bit field */
}U_IM_DISP_PWCHINTENB;

/** Image address. */
typedef struct {
	ULONG	y_addr;			/**< Y address<br>
								 Value range:0x00000000~0xFFFFFFFF<br>
								 Bit depth=8bit -> 2-byte unit<br>
								 Bit depth=12bit -> 3-byte unit<br>
								 Bit depth=16bit -> 4-byte unit		 		*/
	ULONG	c_addr;			/**< CbCr address<br>
								 Value range:0x00000000~0xFFFFFFFF<br>
								 Bit depth=8bit -> 2-byte unit<br>
								 Bit depth=12bit -> 3-byte unit<br>
								 Bit depth=16bit -> 4-byte unit		 		*/
} T_IM_DISP_IMAGE_ADDR;

/** Setting of resize value. */
typedef struct {
	E_IM_DISP_RSZSL		rsz0;		/**< Resize method<br>
										 <ul><li>@ref E_IM_DISP_RSZSL_PADDING_THINNING
											 <li>@ref E_IM_DISP_RSZSL_BILINEAR</ul>		*/
	U_IM_DISP_RSZ1		rsz1;		/**< Resize data 1 (Horizontal)						*/
	U_IM_DISP_RSZ2		rsz2;		/**< Resize data 2 (Vertical)						*/
	U_IM_DISP_RSZ3		rsz3;		/**< Resize data 3 (chrominance start position)		*/
} T_IM_DISP_RESIZE;

/** Setting of YHLP data. */
typedef struct {
	U_IM_DISP_LYHLPCTL	lyhlpcl;	/**< YHLP processing setting register.				*/
	U_IM_DISP_LYHLPK	lyhlpk;		/**< YHLP filter coefficient setting 0/1 register.	*/
	U_IM_DISP_LYHLPOL	lyhlpol;	/**< Outline setting register.						*/
	U_IM_DISP_LYHLPCLR	lyhlpclr;	/**< Outline color setting register.				*/
} T_IM_DISP_YHLP;

/** Setting of Warning processing data. */
typedef struct {
	U_IM_DISP_LYWCTL	lywctl;		/**< Warning processing setting register.				*/
	U_IM_DISP_LYWTH		lyw0th;		/**< Range 0 - warning level setting register.			*/
	U_IM_DISP_LYWTH		lyw1th;		/**< Range 1 - warning level setting register.			*/
	E_IM_DISP_YWMD		lyw0st;		/**< Range 0 - warning processing mode select.<br>
										 <ul><li>@ref E_IM_DISP_YWMD_BR_RGB
											 <li>@ref E_IM_DISP_YWMD_BW_R
											 <li>@ref E_IM_DISP_YWMD_G
											 <li>@ref E_IM_DISP_YWMD_B</ul>					*/
	U_IM_DISP_LYWC		lyw0ca;		/**< Range 0 - warning color A.							*/
	U_IM_DISP_LYWC		lyw0cb;		/**< Range 0 - warning color B.							*/
	E_IM_DISP_YWMD		lyw1st;		/**< Range 1 - warning processing mode select.<br>
										 <ul><li>@ref E_IM_DISP_YWMD_BR_RGB
											 <li>@ref E_IM_DISP_YWMD_BW_R
											 <li>@ref E_IM_DISP_YWMD_G
											 <li>@ref E_IM_DISP_YWMD_B</ul>					*/
	U_IM_DISP_LYWC		lyw1ca;		/**< Range 1 - warning color A.							*/
	U_IM_DISP_LYWC		lyw1cb;		/**< Range 1 - warning color B.							*/
	ULONG				lbost;		/**< Warning display operation setting.<br>
										 <ul><li>@ref D_IM_DISP_BOMD_MASTER
											 <li>@ref D_IM_DISP_BOMD_SLAVE</ul>				*/
} T_IM_DISP_WARNING;

/** Setting of zebra warning (width, vertical, pattern). */
typedef struct {
	U_IM_DISP_LZBWID	lzbwid;		/**< LZBWID: Zebra width setting register.	*/
	U_IM_DISP_LZBV		lzbv;		/**< LZBV: Zebra vertical setting register.	*/
	U_IM_DISP_LZBPT		lzbpt;		/**< LZBPT: Zebra pattern setting register.	*/
} T_IM_DISP_ZEBRA;

/** Setting of table access enable and processing enable. */
typedef struct {
	E_IM_DISP_TABLE_ACCESS	access;		/**< Table access enable.<br>
				 							 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
												 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>	*/
	E_IM_DISP_CORRECT_PROC	proc;		/**< Processing enable.<br>
				 							 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
												 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>	*/
	BYTE					surface;	/**< Table surface.<br>
											 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
												 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>		*/
} T_IM_DISP_TBL_ACCESS_SET;

/** Setting of gain, clip, offset. */
typedef struct {
	T_IM_DISP_YCAL	y_cal;		/**< Gain and offset setting of Y data		*/
	T_IM_DISP_CLIP	y_clip;		/**< Clip setting of Y data					*/
	T_IM_DISP_CCAL	cb_cal;		/**< Gain and offset setting of Cb data		*/
	T_IM_DISP_CLIP	cb_clip;	/**< Clip setting of Cb data				*/
	T_IM_DISP_CCAL	cr_cal;		/**< Gain and offset setting of Cr data		*/
	T_IM_DISP_CLIP	cr_clip;	/**< Clip setting of Cr data				*/
} T_IM_DISP_CLIP_CAL;

/** Setting of grid. */
typedef struct {
	U_IM_DISP_DSTA		ghdsta;		/**< Display beginning position of horizontal grid.	*/
	U_IM_DISP_DSTA		gvdsta;		/**< Display beginning position of vertical grid.	*/
	U_IM_DISP_GLENGTH	glength;	/**< Grid length									*/
	U_IM_DISP_GWIDTH	gwidth;		/**< Grid width										*/
	U_IM_DISP_GITVL		gitvl;		/**< Grid interval									*/
	U_IM_DISP_GNUM		gnum;		/**< The number of Grid line						*/
	U_IM_DISP_GDCTL		gdctl;		/**< Grid setting									*/
} T_IM_DISP_GRID;

/** Setting of face frame parameter. */
typedef struct {
	U_IM_DISP_DSTA		ffdsta;		/**< Display beginning position.	*/
	U_IM_DISP_FFSIZE	ffsize;		/**< Face frame size.				*/
	U_IM_DISP_FFWIDTH	ffwidth;	/**< Face frame width.				*/
	U_IM_DISP_FFCLR		ffclr;		/**< Face frame color.				*/
} T_IM_DISP_FACE_FRAME_PARAM;

/** Setting of face frame. */
typedef struct {
	T_IM_DISP_FACE_FRAME_PARAM	param[D_IM_DISP_FACE_FRAME_COUNT];	/**< Face frame parameter.								*/
	unsigned long long			ffden;								/**< FFDISPEN.FFDEN: Face frame display enable.<br>
																		 bit d[0]:Face frame 0 ~ d[41]:Face frame 41<br>
																		 Value 0:disable, 1:enable<br>						*/
	BYTE						msff;								/**< FFDO.MSFF: Top face frame setting register.<br>
																		 Value range:0~41 (face frame 0 ~ face frame 41)	*/
} T_IM_DISP_FACE_FRAME;

/** Anti-gamma correction table data. */
typedef struct {
	//Note !!! Don't change the following data's order
	USHORT*			r_anti;		/**< Pointer of R data anti gamma table.<br>
									 Please specify the address of the array of USHORT[32].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*			g_anti;		/**< Pointer of G data anti gamma table.<br>
									 Please specify the address of the array of USHORT[32].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*			b_anti;		/**< Pointer of B data anti gamma table.<br>
									 Please specify the address of the array of USHORT[32].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
} T_IM_DISP_ANTI_GAMMA_TBL;

/** Gamma correction table data (main data input block). */
typedef struct {
	//Note !!! Don't change the following data's order
	USHORT*			r_data;		/**< Pointer of R data gamma table.<br>
									 Please specify the address of the array of USHORT[32].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*			g_data;		/**< Pointer of G data gamma table.<br>
									 Please specify the address of the array of USHORT[32].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*			b_data;		/**< Pointer of B data gamma table.<br>
									 Please specify the address of the array of USHORT[32].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
} T_IM_DISP_GAMMA_TBL_IN;

/** Gamma correction table data (data output block). */
typedef struct {
	//Note !!! Don't change the following data's order
	USHORT*			r_data;		/**< Pointer of R data gamma table.<br>
									 Please specify the address of the array of USHORT[33].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*			g_data;		/**< Pointer of G data gamma table.<br>
									 Please specify the address of the array of USHORT[33].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*			b_data;		/**< Pointer of B data gamma table.<br>
									 Please specify the address of the array of USHORT[33].<br>
									 If NULL is specified, this setting is skipped.<br>				*/
} T_IM_DISP_GAMMA_TBL_OUT;

/** main data input block table setting data. */
typedef struct {
	T_IM_DISP_ANTI_GAMMA_TBL	anti_a;		/**< Anti gamma correction table A	*/
	T_IM_DISP_ANTI_GAMMA_TBL	anti_b;		/**< Anti gamma correction table B	*/
	T_IM_DISP_GAMMA_TBL_IN		gamma_a;	/**< Gamma correction table A		*/
	T_IM_DISP_GAMMA_TBL_IN		gamma_b;	/**< Gamma correction table B		*/
} T_IM_DISP_CTRL_MAIN_TBL;

/** data output block table setting data. */
typedef struct {
	T_IM_DISP_GAMMA_TBL_OUT	gamma_a;		/**< Gamma correction table A										*/
	T_IM_DISP_GAMMA_TBL_OUT	gamma_b;		/**< Gamma correction table B										*/
	USHORT*					luminance_a;	/**< Pointer of Luminance correction table A.<br>
												 Please specify the address of the array of USHORT[33].<br>
												 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*					chroma_a;		/**< Pointer of Chroma correction gain table A.<br>
												 Please specify the address of the array of USHORT[33].<br>
												 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*					luminance_b;	/**< Pointer of Luminance correction table B.<br>
												 Please specify the address of the array of USHORT[33].<br>
												 If NULL is specified, this setting is skipped.<br>				*/
	USHORT*					chroma_b;		/**< Pointer of Chroma correction gain table B.<br>
												 Please specify the address of the array of USHORT[33].<br>
												 If NULL is specified, this setting is skipped.<br>				*/
} T_IM_DISP_CTRL_OUTPUT_TBL;

/**
Control data of main data input block.<br>
It is not possible to rewrite it while main data input block is operating.
*/
typedef struct {
	// Whole control register area.
	E_IM_DISP_RPGTMG		lrpgctl;			/**< LRPGCTL: Register parameter taking control register.<br>
													 <ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
														 <li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>						*/
	// Data processing setting register area.
	E_IM_DISP_TABLE_ACCESS	ltblaset_igtaen;	/**< LTBLASET.IGTAEN: Anti-gamma correct table access enable.<br>
													 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
														 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>					*/
	E_IM_DISP_TABLE_ACCESS	ltblaset_gtaen;		/**< LTBLASET.GTAEN: Gamma correct table access enable.<br>
													 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
														 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>					*/
} T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT;

/**
Control data of main data input block.<br>
It is possible to rewrite it while main data input block is operating.
*/
typedef struct {
	// Data input control register area.
	U_IM_DISP_LIDT					lidt;									/**< LIDT0~1: Input data forwarding setting register.<br>						*/
	U_IM_DISP_SIZE					lisize;									/**< LISIZE: Input data size setting register.<br>								*/
	T_IM_DISP_IMAGE_ADDR			laddr[D_IM_DISP_MAIN_ADDR_BANK_SIZE];	/**< LYSA0~3, LCSA0~3: Input address setting register.<br>						*/
	ULONG							y_hga;									/**< Horizontal global area of Y data.<br>
																				 Value range:2~65535<br>
																				 Bit depth=8bit -> 2-byte unit<br>
																				 Bit depth=12bit -> 3-byte unit<br>
																				 Bit depth=16bit -> 4-byte unit		 										*/
	ULONG							c_hga;									/**< Horizontal global area of CbCr data.<br>
																				 Value range:2~65535<br>
																				 Bit depth=8bit -> 2-byte unit<br>
																				 Bit depth=12bit -> 3-byte unit<br>
																				 Bit depth=16bit -> 4-byte unit		 										*/
	UINT32							libctl_iben;							/**< LIBCTL.IBEN: Input bank specification method selection (0 fixation).<br>	*/
	E_IM_DISP_BANK					libctl_ibset;							/**< LIBCTL.IBSET: Input bank specification.<br>
																				 <ul><li>@ref E_IM_DISP_BANK_00
																				     <li>@ref E_IM_DISP_BANK_01
																					 <li>@ref E_IM_DISP_BANK_10
																					 <li>@ref E_IM_DISP_BANK_11</ul>										*/
	// Data processing setting register area.
	E_IM_DISP_RSZSL					lrsz0;									/**< LRSZ0: Horizontal resize setting register - 0.<br>
																				 <ul><li>@ref E_IM_DISP_RSZSL_PADDING_THINNING
																					 <li>@ref E_IM_DISP_RSZSL_BILINEAR</ul>									*/
	U_IM_DISP_RSZ1					lrsz1;									/**< LRSZ1: Resize data 1 (Horizontal)											*/
	U_IM_DISP_RSZ2					lrsz2;									/**< LRSZ2: Resize data 2 (Vertical)											*/
	U_IM_DISP_RSZ3					lrsz3;									/**< LRSZ3: Resize data 3 (chrominance start position)							*/
	T_IM_DISP_YHLP					yhlp;									/**< YHLP data register.														*/
	T_IM_DISP_WARNING				warning;								/**< Warning processing setting register.										*/
	U_IM_DISP_LBLTMR				lbltmr;									/**< Luminance warning display period setting register.							*/
	T_IM_DISP_ZEBRA					zebra;									/**< LZBWID/LZBV/LZBPT: Zebra data register.									*/
	U_IM_DISP_YR_MATRIX_COEFFICIENT	ly2r[D_IM_DISP_MATRIX_SIZE];			/**< LY2R0/1/2: YCbCr->RGB matrix coefficient setting register.					*/
	E_IM_DISP_CORRECT_PROC			ltblaset_igen;							/**< LTBLASET.IGEN: Anti-gamma correction enable.<br>
																				 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
																					 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>							*/
	BYTE							ltblaset_igtsl;							/**< LTBLASET.IGTSL: Inverse gamma correction table surface.<br>
																				 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
																					 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>								*/
	E_IM_DISP_CORRECT_PROC			ltblaset_gmen;							/**< LTBLASET.GMEN: Gamma correction enable.<br>
																				 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
																					 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>							*/
	BYTE							ltblaset_gmtsl;							/**< LTBLASET.GMTSL: Gamma correction table surface.<br>
																				 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
																					 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>								*/
	U_IM_DISP_LCC					lcc[D_IM_DISP_MATRIX_SIZE];				/**< LCC0/1/2: Color space conversion matrix coefficient setting register.		*/
	U_IM_DISP_LGMMD					lgmmd;									/**< LGMMD: Gamma correct mode . */
	U_IM_DISP_LALP					lalp;									/**< LALP: Alpha value setting register. Value range:0x000~0xFF.				*/
	// Display area setting register area.
	U_IM_DISP_DSTA					ldsta;									/**< LDSTA: Display starting position setting register.							*/
	U_IM_DISP_LREVDISP				lrevdisp;								/**< LREVDISP: Reversing display selection register.							*/
} T_IM_DISP_CTRL_MAIN_LAYER;

/**
Control data of data output block.<br>
It is not possible to rewrite it while data output block is operating.
*/
typedef struct {
	// Whole control register area.
	E_IM_DISP_TGKMD			tgkmd;									/**< TRG.TGKMD: Trigger register - External start-up mode.<br>
																		 <ul><li>@ref E_IM_DISP_TGKMD_NORMAL
																			 <li>@ref E_IM_DISP_TGKMD_EXTERNAL</ul>								*/
	ULONG					tgkst;									/**< TGKST: Operation start register.<br>
																		 Value range:0~4095.													*/
	U_IM_DISP_TOCTL			toctl;									/**< TOCTL: Whole output control register.									*/
	// Timing control register area.
	E_IM_DISP_RPGTMG		rpgctl;									/**< RPGCTL: Register parameter taking control register.<br>
																		 <ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
																			 <li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>							*/
	U_IM_DISP_POLSEL		polsel;									/**< POLSEL: External output signal polarity selection register.<br>		*/
	ULONG					tsl;									/**< TSL: Forwarding method setting register.
																		 <ul><li>@ref D_IM_DISP_TSL_PROGRESSIVE
																			 <li>@ref D_IM_DISP_TSL_INTERLACE</ul>								*/
	U_IM_DISP_VCYC			vcyc;									/**< VCYC: Vertical cycle setting register.<br>								*/
	ULONG					hcyc;									/**< HCYC: Horizontal cycle setting register.<br>
																		 Value range:8~65535<br>
																		 HCYC >= HBLK.HBLK + (Output data horizontal cycle) + 8.				*/
	U_IM_DISP_OVPW			ovpw;									/**< OVPW: Vsync pulse width setting register.<br>							*/
	ULONG					hpw;									/**< HPW: Hsync pulse width setting register.<br>
																		 Value range:1~1023														*/
	U_IM_DISP_VBLK			vblk;									/**< VBLK: Period of vertical blank setting register.<br>					*/
	ULONG					hblk;									/**< HBLK: Period of horizontal blank setting register.<br>
																		 Value range:16~1023<br>
																		 16 <= HBLK <= HCYC.HCYC - (Output data horizontal cycle) - 8.			*/
	ULONG					vdly;									/**< VDLY: Vsync output delay setting register.<br>
																		 Value range:0~255<br>
																		 VDLY <= HCYC.HCYC / 2 - 1.												*/
	ULONG					hdly;									/**< HDLY: Hsync output delay setting register.<br>
																		 Value range:0~255<br>
																		 HDLY <= HCYC.HCYC / 2 - 1.												*/
	ULONG					ovsize;									/**< OVSIZE: Output data vertical size setting register.<br>
																		 Value range:0~16383													*/
	ULONG					ohsize;									/**< OHSIZE: Output data horizontal size setting register.<br>
																		 Value range:0~65535													*/
	ULONG					vrfctl;									/**< VRFCTL: Vertical enable signal operation setting register.<br>
																		 <ul><li>@ref D_IM_DISP_VENS_H_ENABLE
																			 <li>@ref D_IM_DISP_VENS_HSYNC</ul>									*/
	ULONG					hrfctl;									/**< HRFCTL: Horizontal enable signal operation setting register.<br>
																		 <ul><li>@ref D_IM_DISP_HENS_ASSERT_ON
																			 <li>@ref D_IM_DISP_HENS_ASSERT_OFF</ul>							*/
	// Output data control register area.
	U_IM_DISP_DOMD			domd;									/**< DOMD: Data output mode setting register.								*/
	E_IM_DISP_TABLE_ACCESS	tblaset_gmtaen;							/**< TBLASET.GMTAEN: Gamma correct table access enable.<br>
																		 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
																			 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>						*/
	E_IM_DISP_TABLE_ACCESS	tblaset_ysatta;							/**< TBLASET.YSATTA: Luminance and chroma correct table access enable.<br>
																		 <ul><li>@ref E_IM_DISP_TABLE_ACCESS_DISABLE
																			 <li>@ref E_IM_DISP_TABLE_ACCESS_ENABLE</ul>						*/
} T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT;

/**
Control data of data output block.<br>
It is possible to rewrite it while data output block is operating.
*/
typedef struct {
	// Whole control register area.
	ULONG							intc;									/**< INTC: Interruption control register.<br>
																				 <ul><li>@ref D_IM_DISP_VFTMG_NONE
																				     <li>@ref D_IM_DISP_VFTMG_1ST
																					 <li>@ref D_IM_DISP_VFTMG_2ND
																					 <li>@ref D_IM_DISP_VFTMG_1ST_2ND</ul>								*/
	U_IM_DISP_INTE					inte;									/**< INTE: Interruption enable register.<br>								*/
	// Timing control register area.
	ULONG							hablk;									/**< HABLK: Horizontal blank adjustment setting register.<br>
																				 Value range:0~4095														*/
	// Output data control register area.
	ULONG							fdoen;									/**< FDOEN: Fource data output enable register.<br>
																				 <ul><li>@ref D_IM_DISP_ENABLE_OFF
																					 <li>@ref D_IM_DISP_ENABLE_ON</ul>									*/
	U_IM_DISP_IMAGE_COLOR			fodata;									/**< FODATA: Fource output data setting register.<br>						*/
	U_IM_DISP_IMAGE_COLOR16			blankdt;								/**< BLANKDT: Blank data setting register.<br>								*/
	ULONG							clbhsize;								/**< CLBHSIZE: Color bar horizontal size setting register.<br>
																				 Value range:0~65535<br>												*/
	U_IM_DISP_IMAGE_COLOR			clbdt[D_IM_DISP_COLOR_BAR_COUNT];		/**< CLBDT: Color bar display color setting register.<br>					*/
	U_IM_DISP_BLDCTL				bldctl;									/**< BLDCTL: Blend order 1~4 layer selection.<br>							*/
	U_IM_DISP_YR_MATRIX_COEFFICIENT	r2y[D_IM_DISP_MATRIX_SIZE];				/**< R2Y0/1/2: RGB->YCbCr matrix coefficient setting register.<br>			*/
	T_IM_DISP_CLIP_CAL				clip_cal;								/**< YCAL  : Y data operation setting.<br>
																				 YCLIP : Y data clip setting.<br>
																				 CBCAL : Cb data operation setting.<br>
																				 CBCLIP: Cb data clip setting.<br>
																				 CRCAL : Cr data operation setting.<br>
																				 CRCLIP: Cr data clip setting.<br>										*/
	U_IM_DISP_DOCTL0				doctl0;									/**< DOCTL0: Data output control 0 register.<br>							*/
	U_IM_DISP_DOCTL1				doctl1;									/**< DOCTL1: Data output control 1 register.<br>							*/
	U_IM_DISP_DOCTL2				doctl2;									/**< DOCTL2: Data output control 2 register.<br>							*/
	U_IM_DISP_TRSCODE				trscode[2];								/**< TRSCODE0: SAV/EAV code setting 0 register.<br>
																				 TRSCODE1: SAV/EAV code setting 1 register.<br>							*/
	U_IM_DISP_YR_MATRIX_COEFFICIENT	y2r[D_IM_DISP_MATRIX_SIZE];				/**< Y2R0/1/2: YCbCr->RGB matrix coefficient setting register.				*/
	E_IM_DISP_CORRECT_PROC			tblaset_gmen;							/**< TBLASET.GMEN: Gamma correction enable.<br>
																				 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
																					 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>						*/
	BYTE							tblaset_gmtsl;							/**< TBLASET.GMTSL: Gamma correction table surface.<br>
																				 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
																					 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>							*/
	E_IM_DISP_CORRECT_PROC			tblaset_ysaten;							/**< TBLASET.YSATEN: Luminance and chroma correction enable.<br>
																				 <ul><li>@ref E_IM_DISP_CORRECT_PROC_DISABLE
																					 <li>@ref E_IM_DISP_CORRECT_PROC_ENABLE</ul>						*/
	BYTE							tblaset_ysatsl;							/**< TBLASET.YSATSL: Luminance and chroma correction table surface.<br>
																				 <ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
																					 <li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>							*/
	// Face frame
	T_IM_DISP_FACE_FRAME			face;									/**< Face frame data.<br>													*/
	// Interrupt callback functions.
	VP_CALLBACK						int_callback[E_IM_DISP_INT_CB_MAX];		/**< Interrupt callback functions.											*/
} T_IM_DISP_CTRL_OUTPUT;

/**
Control data of Grid.<br>
It is possible to rewrite it while data output block is operating.
*/
typedef struct {
	T_IM_DISP_GRID			grid;		/**< Grid display parameter.<br>					*/
	E_IM_DISP_GDISPEN		gdispen;	/**< GDISPEN: Grid display enable register.<br>
											 <ul><li>@ref E_IM_DISP_GDISPEN_NOT_DISPLAY
												 <li>@ref E_IM_DISP_GDISPEN_DISPLAY</ul>	*/
} T_IM_DISP_CTRL_GRID_LAYER;

/**
Control data of OSD data input block.<br>
It is possible to rewrite it while OSD data input block is operating.
*/
typedef struct {
	// Data input control register area.
	U_IM_DISP_GRIDT					gridt;										/**< GRIDT: Input data forwarding setting register.<br>								*/
	U_IM_DISP_SIZE					grtisize;									/**< GRTISIZE: Input area size setting register.<br>								*/
	U_IM_DISP_DSTA					grtdsta;									/**< GRTDSTA: Area display start position setting register.<br>						*/
	U_IM_DISP_GRIPO					gripo;										/**< GRIPO: Input data arrangement order setting register.<br>						*/
	E_IM_DISP_INPUT_DATA_AREA		grscctl_idset;								/**< GRSCCTL.IDSET: Input data arrangement order setting register.<br>
																					 <ul><li>@ref E_IM_DISP_INPUT_DATA_AREA_0
																						 <li>@ref E_IM_DISP_INPUT_DATA_AREA_1
																						 <li>@ref E_IM_DISP_INPUT_DATA_AREA_2
																						 <li>@ref E_IM_DISP_INPUT_DATA_AREA_3</ul>									*/
	// Display area setting register area.
	U_IM_DISP_SIZE					grisize[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];	/**< GRISIZE: Input data size setting register.<br>									*/
	ULONG							grsa0[D_IM_DISP_OSD_ADDR_0_BANK_SIZE];		/**< GRSA0: Processing start address setting 0 register.<br>
																					 Value range:0x00000000~0xFFFFFFFF (8-bytel unit)								*/
	ULONG							grsa[D_IM_DISP_OSD_ADDR_BANK_SIZE];			/**< GRSA: Processing start address setting register.<br>
																					 Value range:0x00000000~0xFFFFFFFF (8-bytel unit)								*/
	ULONG							grasa[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];	/**< GRASA: A deta Processing start address setting register.<br>
																					 Value range:0x00000000~0xFFFFFFFF (8-bytel unit)								*/
	ULONG							grhga[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];	/**< GRHGA: Horizontal global area.<br>
																					 Value range:8~65528 (8-bytel unit)												*/
	ULONG							grahga[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];	/**< GRAHGA: A deta horizontal global area.<br>
																					 Value range:8~65528 (8-bytel unit)												*/
	U_IM_DISP_DSTA					grdsta[D_IM_DISP_OSD_DISPLAY_AREA_COUNT];	/**< GRDSTA: Display starting position setting registers.<br>						*/
	ULONG							graren;										/**< GRAREN: Display enable setting register.<br>
																					 bit d[0]:AREN0 ~ d[9]:AREN9<br>
																					 Value 0:disable, 1:enable<br>													*/
	ULONG							grbsl;										/**< GRBSL: Blink method setting register.<br>
																					 bit d[0]:BSL0 ~ d[9]:BSL9<br>
																					 Value 0:not blink, 1:blink<br>													*/
	U_IM_DISP_GRBLINK				grblink;									/**< GRBLINK: Blink setting register.<br>											*/
	ULONG							grbinit;									/**< GRBINIT: Blink sync setting register.<br>
																					 bit d[0]:AREN0 ~ d[9]:AREN9<br>
																					 Value 0:not blink sync, 1:blink sync<br>										*/
	ULONG							grbitrg;									/**< GRBITRG: Blink sync trigger register.<br>
																					 <ul><li>@ref D_IM_DISP_BITRG_NON
																						 <li>@ref D_IM_DISP_BITRG_WRITE_SYNC_WAIT
																						 <li>@ref D_IM_DISP_BITRG_READ_STOP
																						 <li>@ref D_IM_DISP_BITRG_READ_SYNC_WAIT</ul>								*/
	// Data processing setting register area.
	E_IM_DISP_RSZSL					grrsz0;										/**< GRRSZ0: Horizontal resize setting register - 0.<br>
																					 <ul><li>@ref E_IM_DISP_RSZSL_PADDING_THINNING
																						 <li>@ref E_IM_DISP_RSZSL_BILINEAR</ul>										*/
	U_IM_DISP_RSZ1					grrsz1;										/**< GRRSZ1: Resize data 1 (Horizontal).<br>										*/
	U_IM_DISP_RSZ2					grrsz2;										/**< GRRSZ2: Resize data 2 (Vertical).<br>											*/
	U_IM_DISP_RSZ3					grrsz3;										/**< GRRSZ3: Resize data 3 (chrominance start position).<br>						*/
	U_IM_DISP_GRY2R					gry2r[D_IM_DISP_MATRIX_SIZE];				/**< GRY2R0/1/2: YCbCr->RGB matrix coefficient setting register.<br>				*/
	ULONG							gralp;										/**< GRALP: Alpha value setting register.<br>
																					 Value range:0~255																*/
} T_IM_DISP_CTRL_OSD_LAYER;

/**
Control data of P2M macro in write channel block.
*/
typedef struct {
	ULONG				p2mpaen;		/**< P2MPAEN: P2M-chip RAM access enable register.<br>
											 <ul><li>@ref D_IM_DISP_ENABLE_OFF
												 <li>@ref D_IM_DISP_ENABLE_ON</ul>								*/
	E_IM_DISP_PLPF		p2mctl_plpf;	/**< P2MCTL.PLPF: LPF recording mode.<br>
											 <ul><li>@ref E_IM_DISP_PLPF_INVALID
												 <li>@ref E_IM_DISP_PLPF_SHORT
												 <li>@ref E_IM_DISP_PLPF_MIDDLE
												 <li>@ref E_IM_DISP_PLPF_LONG</ul>								*/
	E_IM_DISP_P2MTYP	p2mctl_p2mtyp;	/**< P2MCTL.P2MTYP: P2M recording mode.<br>
											 <ul><li>@ref E_IM_DISP_P2MTYP_8
												 <li>@ref E_IM_DISP_P2MTYP_10_PACK
												 <li>@ref E_IM_DISP_P2MTYP_12_PACK
												 <li>@ref E_IM_DISP_P2MTYP_16</ul>								*/
	USHORT				p2mofs;			/**< P2MOFS: P2M input offset register.<br>
											 Value range:0~32767.												*/
	BYTE				pbsft;			/**< PBSFT: P2M right bit shift amount register.<br>
											 Value range:0~8.													*/
	USHORT				pclph;			/**< PCLPH: P2M on the clip amount register.<br>
											 Value range:0~65535.												*/
	USHORT				ptrmv;			/**< PTRMV: P2M trimming vertical start position register.<br>
											 Value range:0~8191.												*/
	USHORT				ptrmh;			/**< PTRMH: P2M trimming horizontal start position register.<br>
											 Value range:0~12287.												*/
	USHORT				ptrmvw;			/**< PTRMVW: P2M trimming vertical processing line number register.<br>
											 Value range:0~65535.												*/
	ULONG				ptrmhw;			/**< PTRMHW: P2M trimming horizontal processing line number register.<br>
											 Value range:0~16777215.											*/
	BYTE				ptrmxvcyc;		/**< PTRMXVCYC: P2M trimming vertical thinning cycle register.<br>
											 Value 0:cycle 64 Lines, 1:cycle 1 line~63:cycle 63 lines.			*/
	BYTE				ptrmxhcyc;		/**< PTRMXHCYC: P2M trimming horizontal thinning cycle register.<br>
											 Value 0:cycle 64 pixels, 1:cycle 1 pixel~63:cycle 63 pixels.		*/
	UINT64				ptrmxven;		/**< PTRMXVEN: P2M trimming vertical uptake line register.<br>
											 bit d[0]:64 lines ~ d[63]:1 line.<br>
											 MSB is the beginning of one period,<br>
											 LSB will correspond to the end of one period<br>					*/
	UINT64				ptrmxhen;		/**< PTRMXHEN: P2M trimming horizontal capture pixel register.<br>
											 bit d[0]:64 pixel ~ d[63]:1 pixel.<br>
											 MSB is the beginning of one period,<br>
											 LSB will correspond to the end of one period<br>					*/
	BYTE				p2mmir;			/**< P2MMIR: P2M left and right inverted enable register.<br>
											 <ul><li>@ref D_IM_DISP_ENABLE_OFF
												 <li>@ref D_IM_DISP_ENABLE_ON</ul>								*/
	E_IM_DISP_P2M2PMD	p2m2pmd;		/**< P2M2PMD: P2M 1 pixel / 2 pixel input mode selection register.<br>
											 <ul><li>@ref E_IM_DISP_P2M2PMD_1
												 <li>@ref E_IM_DISP_P2M2PMD_2</ul>								*/
} T_IM_DISP_CTRL_P2M;

/**
Control data of PWCH macro in write channel block.
*/
typedef struct {
	U_IM_DISP_PWAXCTL		pwaxctl;		/**< PWAXCTL: PWCH AXI control register.					*/
	BYTE					pwmw;			/**< PWCHCTL.PWMW: PWCH mask with writing control.<br>
												 <ul><li>@ref D_IM_DISP_ENABLE_OFF
													 <li>@ref D_IM_DISP_ENABLE_ON</ul>					*/
	U_IM_DISP_PWCHINTENB	pwchintenb;		/**< PWCHINTENB: PWCH interrupt enable register.			*/
	ULONG					pwsa;			/**< PWSA: PWCH top address setting register.<br>
												 Value range:0~4294967288.								*/
	ULONG					pwlsize;		/**< PWLSIZE: PWCH line feed size setting register.<br>
												 Value range:0~2147483640.<br>
												 d[31]:Sign bit, this register set in two's complement.	*/
	BYTE					pwvfm;			/**< PWVFM: PWCH video format selection register.<br>
												 <ul><li>@ref D_IM_DISP_FORMAT_NORMAL
													 <li>@ref D_IM_DISP_FORMAT_VIDEO</ul>				*/
} T_IM_DISP_CTRL_PWCH;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif // CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data  														*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_PCLOCK
#ifdef CO_DEBUG_DISP
// PCLK_Disp setting task counter (0~255)
volatile UCHAR gIM_DISP_APB_Clock_Counter  = 0;
#else
// PCLK_Disp setting task counter (0~255)
static volatile UCHAR gIM_DISP_APB_Clock_Counter  = 0;
#endif // CO_DEBUG_DISP
#endif // CO_ACT_PCLOCK

#ifdef CO_ACT_HCLOCK
#ifdef CO_DEBUG_DISP
// HCLK_Disp setting task counter (0~255)
volatile UCHAR gIM_DISP_AHB_Clock_Counter  = 0;
#else
// HCLK_Disp setting task counter (0~255)
static volatile UCHAR gIM_DISP_AHB_Clock_Counter  = 0;
#endif // CO_DEBUG_DISP
#endif // CO_ACT_HCLOCK

/*----------------------------------------------------------------------*/
/* Macro  																*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_PCLOCK
// PCLK On
#define	Im_DISP_APB_Clock_On()		(Dd_Top_Start_Clock((UCHAR*)&gIM_DISP_APB_Clock_Counter, &Dd_Top_Get_CLKSTOP3(), D_IM_DISP_CLKSTOP3_DISPAP_ON))

// PCLK Off
#define	Im_DISP_APB_Clock_Off()		(Dd_Top_Stop_Clock((UCHAR*)&gIM_DISP_APB_Clock_Counter, &Dd_Top_Get_CLKSTOP3(), D_IM_DISP_CLKSTOP3_DISPAP_OFF))
#endif	// CO_ACT_PCLOCK

#ifdef CO_ACT_HCLOCK
// HCLK On
#define	Im_DISP_AHB_Clock_On()		(Dd_Top_Start_Clock((UCHAR*)&gIM_DISP_AHB_Clock_Counter, &Dd_Top_Get_CLKSTOP3(), D_IM_DISP_CLKSTOP3_DISPAH_ON))

// HCLK Off
#define	Im_DISP_AHB_Clock_Off()		(Dd_Top_Stop_Clock((UCHAR*)&gIM_DISP_AHB_Clock_Counter, &Dd_Top_Get_CLKSTOP3(), D_IM_DISP_CLKSTOP3_DISPAH_OFF))
#endif	// CO_ACT_HCLOCK

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

//---------------------- driver  section -------------------------------
/**
Initialize DISP macro to clear soft-reset bit.
*/
extern VOID Im_DISP_Init(VOID);

/**
Set control data of main data input block.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param [in] ctrl_trg_limit	Setting of main data input block.<br>
							It is not possible to rewrite it while main data input block is operating.<br>
@param[in]	ctrl			Setting of main data input block.<br>
							It is possible to rewrite it while main data input block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
@remarks	If NULL is specified for the argument, the setting of the item can be skipped.<br>
			It is necessary to fulfill the following conditions. <br>
			<ul><li>LISIZE.IHSIZE >= 128
				<li>LISIZE.IHSIZE is a multiple of 2
				<li>LDSTA.DSH + LISIZE.IHSIZE * (LRSZ1.HRSZM / LRSZ1.HRSZN) <= OHSIZE
				<li>LISIZE.IVSIZE >= 16
				<li>LISIZE.IVSIZE is a multiple of 2
				<li>LDSTA.DSV + LISIZE.IVSIZE * (LRSZ2.VRSZM / LRSZ2.VRSZN) <= OVSIZE
				<li>LYHGA > 0
				<li>LCHGA > 0
				<li>LRSZ1.HRSZM / LRSZ1.HRSZN >= 0.5
				<li>LRSZ1.HRSZM / LRSZ1.HRSZN <= 8
				<li>(in the case of LRSZ1.HRSZM / LRSZ1.HRSZN = 1.0) LRSZ1.HRSZOF = 0
				<li>(in the case of LHRSZ0 = E_IM_DISP_RSZSL_PADDING_THINNING) LRSZ1.HRSZOF = 0
				<li>LRSZ1.HRSZOF < LISIZE.IHSIZE * LRSZ1.HRSZM - (OHSIZE - 1) * LRSZ1.HRSZN
				<li>LRSZ1.HRSZOF < LRSZ1.HRSZM
				<li>LRSZ2.VRSZM / LRSZ2.VRSZN >= 0.5
				<li>LRSZ2.VRSZM / LRSZ2.VRSZN <= 8
				<li>LYWTH.YWTHH > LYWTH.YWTHL
				<li>LDSTA.DSH is a multiple of 2
				<li>LDSTA.DSV is a multiple of 2
				<li>LTCCTL.TCRES < 3
				<li>(in the case of LTCCTL.TCRES = 1 and LTCCTL.TCBLEN = 0) LTCCTL.TCTBL < 2
				<li>(in the case of LTCCTL.TCRES = 2 and LTCCTL.TCBLEN = 1) LTCCTL.TCTBL < 2
			</ul>
*/
extern INT32 Im_DISP_Ctrl_Main_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT const *const ctrl_trg_limit, T_IM_DISP_CTRL_MAIN_LAYER const *const ctrl);

/**
Get control data of main data input block.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out] ctrl_trg_limit	Setting of main data input block.<br>
							It is not possible to rewrite it while main data input block is operating.<br>
@param[out]	ctrl			Setting of main data input block.<br>
							It is possible to rewrite it while main data input block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
*/
extern INT32 Im_DISP_Get_Ctrl_Main_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT *const ctrl_trg_limit, T_IM_DISP_CTRL_MAIN_LAYER *const ctrl);

/**
Set control data of data output block.<br>
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ctrl_trg_limit		Setting of data output block.<br>
								It is not possible to rewrite it while data output block is operating.<br>
@param[in]	ctrl				Setting of data output block.<br>
								It is possible to rewrite it while data output block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy.
@remarks	If NULL is specified for the argument, the setting of the item can be skipped.<br>
			It is necessary to fulfill the following conditions. <br>
			<ul><li>(in the case of IFS = D_IM_DISP_IFS_LCD) TOCTL.CON = D_IM_DISP_CON_EXTERNAL
				<li>(in the case of IFS = D_IM_DISP_IFS_HDMI) POLSEL.VEPS = D_IM_DISP_POLARITY_POSITIVE
				<li>(in the case of TSL = D_IM_DISP_TSL_PROGRESSIVE) VCYC.VCYC1 >= VBLK.VBLK1 + OVSIZE + 1
				<li>(in the case of TSL = D_IM_DISP_TSL_PROGRESSIVE) VCYC.VCYC2 >= VBLK.VBLK2 + OVSIZE + 1
				<li>(in the case of TSL = D_IM_DISP_TSL_INTERLACE) VCYC.VCYC1 >= VBLK.VBLK1 + (OVSIZE / 2) + 1
				<li>(in the case of TSL = D_IM_DISP_TSL_INTERLACE) VCYC.VCYC2 >= VBLK.VBLK2 + (OVSIZE / 2) + 1
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_24BIT) HCYC >= HBLK + OHSIZE + 8
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_24BIT) HCYC >= OHSIZE + 28
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_8BIT_1) HCYC >= HBLK + OHSIZE + 8
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_8BIT_1) HCYC >= OHSIZE + 28
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_8BIT_2) HCYC >= HBLK + (OHSIZE * 3) + 8
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_8BIT_2) HCYC >= (OHSIZE * 3) + 28
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_8BIT_3) HCYC >= HBLK + (OHSIZE * 2) + 8
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_8BIT_3) HCYC >= (OHSIZE * 2) + 28
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_8BIT) HCYC >= HBLK + (OHSIZE * 2) + 8
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_8BIT) HCYC >= (OHSIZE * 2) + 28
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_16BIT) HCYC >= HBLK + OHSIZE + 8
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_16BIT) HCYC >= OHSIZE + 28
				<li>OVPW.OVPW > 0
				<li>HPW > 0
				<li>VBLK.VBLK1 > 0
				<li>VBLK.VBLK2 > 0
				<li>(in the case of HDLY = 0) HBLK >= 16
				<li>(in the case of HDLY > 0) HBLK >= 8
				<li>VDLY <= (HCYC / 2) - 1
				<li>HDLY <= HBLK - 8
				<li>OVSIZE < 8192
				<li>(in the case of TSL = D_IM_DISP_TSL_INTERLACE then OVSIZE is a multiple of 2
				<li>OHSIZE >= 256
				<li>(in the case of IFS = D_IM_DISP_IFS_LCD) OHSIZE <= 1024
				<li>(in the case of IFS = D_IM_DISP_IFS_HDMI) OHSIZE <= 1920
				<li>(in the case of IFS = D_IM_DISP_IFS_HDMI) VRFCTL.VEPS = D_IM_DISP_POLARITY_POSITIVE
				<li>(in the case of IFS = D_IM_DISP_IFS_LCD) DOMD.MODE = 0x00 or 0x04 or 0x05 or 0x06 or 0x40 or 0x41
				<li>(in the case of IFS = D_IM_DISP_IFS_HDMI) DOMD.MODE = 0x41
				<li>CLBHSIZE * 16 >= OHSIZE
				<li>BLDCTL.BLD1, BLDCTL.BLD2, BLDCTL.BLD3 and BLDCTL.BLD4 do not overlap
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_24BIT) DOCTL1.ODO1 = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_24BIT) DOCTL1.ODO2 = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_24BIT) DOCTL1.ODE1 = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_RGB888_24BIT) DOCTL1.ODE2 = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_8BIT) DOCTL2.CLVS = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_8BIT) DOCTL2.CAVES = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_16BIT) DOCTL2.CLVS = 0
				<li>(in the case of DOMD.MODE = D_IM_DISP_MODE_YCC422_16BIT) DOCTL2.CAVES = 0
				<li>FFDSTA.FFDSH is a multiple of 2
				<li>FFDSTA.FFDSV is a multiple of 2
				<li>FFSIZE.FFHSZ is a multiple of 2
				<li>FFSIZE.FFHSZ >= 8
				<li>FFSIZE.FFVSZ is a multiple of 2
				<li>FFSIZE.FFVSZ >= 8
				<li>FFWIDTH.FFHWID is a multiple of 2
				<li>FFWIDTH.FFHWID >= 2
				<li>FFWIDTH.FFVWID is a multiple of 2
				<li>FFWIDTH.FFVWID >= 2
			</ul>
*/
extern INT32 Im_DISP_Ctrl_Output(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT const *const ctrl_trg_limit, T_IM_DISP_CTRL_OUTPUT const *const ctrl);

/**
Get control data of data output block.<br>
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ctrl_trg_limit		Setting of data output block.<br>
								It is not possible to rewrite it while data output block is operating.<br>
@param[out]	ctrl				Setting of data output block.<br>
								It is possible to rewrite it while data output block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
*/
extern INT32 Im_DISP_Get_Ctrl_Output(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT *const ctrl_trg_limit, T_IM_DISP_CTRL_OUTPUT *const ctrl);

/**
Set control data of Grid in common block.<br>
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ctrl				Setting of grid data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
			It is necessary to fulfill the following conditions. <br>
			<ul><li>GHDSTA.DSH is a multiple of 2
				<li>GHDSTA.DSV is a multiple of 2
				<li>GVDSTA.DSH is a multiple of 2
				<li>GVDSTA.DSV is a multiple of 2
				<li>GLENGTH.GHLEN is a multiple of 2
				<li>GLENGTH.GHLEN >= 8
				<li>GLENGTH.GVLEN is a multiple of 2
				<li>GLENGTH.GVLEN >= 8
				<li>GWIDTH.GHWID is a multiple of 2
				<li>GWIDTH.GHWID >= 2
				<li>GWIDTH.GVWID is a multiple of 2
				<li>GWIDTH.GVWID >= 2
				<li>GITVL.GHITV is a multiple of 2
				<li>GITVL.GHITV >= GWIDTH.GHWID + 2
				<li>GITVL.GVITV is a multiple of 2
				<li>GITVL.GVITV >= GWIDTH.GVWID + 2
			</ul>
*/
extern INT32 Im_DISP_Ctrl_Grid_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_GRID_LAYER const *const ctrl);

/**
Get control data of Grid in common block.<br>
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ctrl				Setting of grid data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
*/
extern INT32 Im_DISP_Get_Ctrl_Grid_Layer(E_IM_DISP_SEL block, T_IM_DISP_CTRL_GRID_LAYER *const ctrl);

/**
Set control data of OSD data input block.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	grrpgctl		Register paramter taking control register.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@param[in]	ctrl			Setting of OSD data input block.<br>
							It is possible to rewrite it while OSD data input block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy.
@remarks	If NULL is specified for the argument, the setting of the item can be skipped.<br>
			It is necessary to fulfill the following conditions. <br>
			<ul><li>GRIDT.IFMT <= @ref D_IM_DISP_GRIDT_IFMT_RGBA5551
				<li>GRTISIZE.TIHSIZE is a multiple of 2
				<li>GRTISIZE.TIHSIZE >= 8
				<li>GRDSTA.DSH + GRTISIZE.TIHSIZE * (GRRSZ1.HRSZM / GRRSZ1.HRSZN) <= OHSIZE
				<li>GRTISIZE.TIVSIZE is a multiple of 2
				<li>GRTISIZE.TIVSIZE >= 8
				<li>GRDSTA.DSV + GRTISIZE.TIVSIZE * (GRRSZ2.VRSZM / GRRSZ2.VRSZN) <= OVSIZE
				<li>GRTDSTA.DSH is a multiple of 2
				<li>GRTDSTA.DSV is a multiple of 2
				<li>GRIPO.IPO1, GRIPO.IPO2, GRIPO.IPO3 and GRIPO.IPO4 do not overlap
				<li>GRISIZE.IHSIZE is a multiple of 4
				<li>GRISIZE.IHSIZE >= 8
				<li>GRISIZE.IVSIZE is a multiple of 2
				<li>GRISIZE.IVSIZE >= 8
				<li>GRDSTA.DSH + GRISIZE.IHSIZE <= GRTISIZE.TIHSIZE
				<li>GRDSTA.DSV + GRISIZE.IVSIZE <= GRTISIZE.TIVSIZE
				<li>GRSA0 is a multiple of 8
				<li>GRSA is a multiple of 8
				<li>GRHGA > 0
				<li>GRHGA <= 0xFFF8
				<li>GRHGA is a multiple of 8
				<li>GRDSTA.DSH is a multiple of 2
				<li>GRDSTA.DSV is a multiple of 2
				<li>GRDSTA.DSH_0 <= DSH_1 <= DSH_2 ... <= DSH_9 (in order of bank_no)
				<li>(in case of same DSH) GRDSTA.DSV_0 <= DSV_1 <= DSV_2 ... <= DSV_9
				<li>GRRSZ1.HRSZM / GRRSZ1.HRSZN >= 0.5
				<li>GRRSZ1.HRSZM / GRRSZ1.HRSZN <= 8
				<li>(in the case of GRRSZ1.HRSZM / GRRSZ1.HRSZN = 1.0) GRRSZ1.HRSZOF = 0
				<li>(in the case of GRRSZ0 = E_IM_DISP_RSZSL_PADDING_THINNING) GRRSZ1.HRSZOF = 0
				<li>GRRSZ1.HRSZOF < GRISIZE.IHSIZE * GRRSZ1.HRSZM - (pixel count after horizontal resize -1) * GRRSZ1.HRSZN
				<li>GRRSZ1.HRSZOF < GRRSZ1.HRSZM
				<li>GRRSZ2.VRSZM / GRRSZ2.VRSZN >= 0.5
				<li>GRRSZ2.VRSZM / GRRSZ2.VRSZN <= 8
				<li>GRTCCTL.TCRES < 3
				<li>(in the case of GRTCCTL.TCRES = 1 and GRTCCTL.TCBLEN = 0) GRTCCTL.TCTBL < 2
				<li>(in the case of GRTCCTL.TCRES = 2 and GRTCCTL.TCBLEN = 1) GRTCCTL.TCTBL < 2
			</ul>
*/
extern INT32 Im_DISP_Ctrl_OSD_Layer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG* grrpgctl, T_IM_DISP_CTRL_OSD_LAYER const *const ctrl);

/**
Get control data of OSD layer in common block.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	grrpgctl		Register paramter taking control register.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@param[out]	ctrl			Setting of OSD data input block.<br>
							It is possible to rewrite it while OSD layer in common block is operating.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
*/
extern INT32 Im_DISP_Get_Ctrl_OSD_Layer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG* grrpgctl, T_IM_DISP_CTRL_OSD_LAYER *const ctrl);

/**
Set Gamma table of main data input block.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ctrl_tbl		Table setting data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
*/
extern INT32 Im_DISP_Ctrl_Main_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_TBL const *const ctrl_tbl);

/**
Get Gamma table of main data input block.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ctrl_tbl		Table setting data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Ctrl_Main_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_MAIN_TBL *const ctrl_tbl);

/**
Set Gamma table of data output block.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ctrl_tbl		Table setting data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks If NULL is specified for the argument, the acquisition of the item can be skipped.<br>
*/
extern INT32 Im_DISP_Ctrl_Output_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TBL const *const ctrl_tbl);

/**
Get Gamma table of data output block.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ctrl_tbl		Table setting data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Ctrl_Output_Tbl(E_IM_DISP_SEL block, T_IM_DISP_CTRL_OUTPUT_TBL *const ctrl_tbl);

/**
The block is started. 
@param[in]	hdmi_layer		Layer selection for HDMI block(common block 0).<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[in]	lcd_mipi_layer	Layer selection for LCD or MIPI block(common block 1).<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[in]	write_channel	Write channel block selection.<br>
							Value:<br>
							0 : not select<br>
							1 : Write channel 0 (Luminance data)<br>
							2 : Write channel 1 (Chrominance data)<br>
							3 : Write channel 0 & 1<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_ACCESS_ERR			TRG cannot be set up.
@retval	D_IM_DISP_TIMEOUT				The channel cannot stop within 33ms.
@remarks	@ref Im_DISP_Start or @ref Im_DISP_Stop can be called 1 time only at same frame.<br>
			It must be interval (system's cycle/CPU's cycle + 2*CPU's cycle + 1) if the TRG was written continuously.<br>
			That is 266 / 27 + 2 + 1 = 19<br>
			It's no any problem because the following instructions are over 19 assemble instruction.<br>
*/
extern INT32 Im_DISP_Start(UINT32 hdmi_layer, UINT32 lcd_mipi_layer, BYTE write_channel);

/**
The block is stopped.
@param[in]	hdmi_layer		Layer selection for HDMI block(common block 0).<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[in]	lcd_mipi_layer	Layer selection for LCD or MIPI block(common block 1).<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[in]	write_channel	Write channel block selection.<br>
							Value:<br>
							0 : not select<br>
							1 : Write channel 0 (Luminance data)<br>
							2 : Write channel 1 (Chrominance data)<br>
							3 : Write channel 0 & 1<br>
@param[in]	stop_type		Selection whether stop of block is waited or do not wait.<br>
							<ul><li>@ref E_IM_DISP_STOP_TYPE_WITHOUT_WAIT
								<li>@ref E_IM_DISP_STOP_TYPE_WITH_WAIT</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_ACCESS_ERR			TRG cannot be set up.
@retval	D_IM_DISP_TIMEOUT				The channel cannot stop within 33ms.
@remarks	If it's necessary, call the @ref Im_DISP_Wait_Stop to wait the channel stopping <br>
			when the @ref E_IM_DISP_STOP_TYPE_WITH_WAIT was used.<br>
			@ref Im_DISP_Start or @ref Im_DISP_Stop can be called 1 time only at same frame.<br>
			It must be interval (system's cycle/CPU's cycle + 2*CPU's cycle + 1) if the SR was written continuously.<br>
			That is 266 / 27 + 2 + 1 = 19<br>
			It's no any problem because the following instructions are over 19 assemble instruction.<br>
*/
extern INT32 Im_DISP_Stop(UINT32 hdmi_layer, UINT32 lcd_mipi_layer, BYTE write_channel, E_IM_DISP_STOP_TYPE stop_type);

/**
Get startup status of all blocks. 
@param[out]	hdmi_layer		Layer status for HDMI block(common block 0).<br>
							The value corresponding to each block starting layer is set up.<br>
							(When two or more layers are starting, OR value of them is set up.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[out]	lcd_mipi_layer	Layer status for LCD or MIPI block(common block 1).<br>
							The value corresponding to each block starting layer is set up.<br>
							(When two or more layers are starting, OR value of them is set up.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Status(UINT32* hdmi_layer, UINT32* lcd_mipi_layer);

/**
Wait VSYNC interruption.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	interruption_select	Bit map to select wait item. <br>
							<ul><li>@ref E_IM_DISP_INTERRUPTION_SELECT_VE
								<li>@ref E_IM_DISP_INTERRUPTION_SELECT_VE2
								<li>@ref E_IM_DISP_CORRECT_SELECT_ALL_VE</ul>
@param[in]	wait_time		Wait frequency.<br>
							<ul><li>@ref E_IM_DISP_WAIT_1_TIME
								<li>@ref E_IM_DISP_WAIT_2_TIME</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_STATUS_ABNORMAL		Can not wait because INTE was not set to 1 or<br>
										INTC was 0.
*/
extern INT32 Im_DISP_Wait_Vsync(E_IM_DISP_SEL block, E_IM_DISP_INTERRUPTION_SELECT interruption_select, E_IM_DISP_WAIT wait_time);

/**
Wait the block stopping after the @ref Im_DISP_Stop was called if it was necessary.
@param [in]	hdmi_layer		Layer selection for HDMI block(common block 0).<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param [in]	lcd_mipi_layer	Layer selection for LCD or MIPI block(common block 1).<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_TIMEOUT				Time-out occur when waitng the block stop.
*/
extern INT32 Im_DISP_Wait_Stop(UINT32 hdmi_layer, UINT32 lcd_mipi_layer);

/**
Wait DWCH(PWHC) interruption.
@param[in]	interruption_select	Bit map to select wait item. <br>
							<ul><li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE
								<li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE
								<li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE</ul>
@param[in]	wait_time		Wait frequency.<br>
							<ul><li>@ref E_IM_DISP_WAIT_1_TIME
								<li>@ref E_IM_DISP_WAIT_2_TIME</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_STATUS_ABNORMAL		Can not wait because INTE was not set to 1 or<br>
										INTC was 0.
*/
extern INT32 Im_DISP_Wait_PWCH(E_IM_DISP_DWCH_INTERRUPTION_SELECT interruption_select, E_IM_DISP_WAIT wait_time);

/**
Wait DWCH(PWHC) interruption. Detect
@param[in]	interruption_select	Bit map to select wait item. <br>
							<ul><li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE
								<li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE
								<li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_PWCH_Detect(E_IM_DISP_DWCH_INTERRUPTION_SELECT interruption_select );

/**
Wait DWCH(PWHC) interruption. Detect Clear
@param[in]	interruption_select	Bit map to select wait item. <br>
							<ul><li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWE
								<li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWEE
								<li>@ref E_IM_DISP_DWCH_INTERRUPTION_SELECT_PWXE</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_PWCH_Detect_Clear(E_IM_DISP_DWCH_INTERRUPTION_SELECT interruption_select );

/**
DISP interrupt handler.
*/
extern VOID Im_DISP_Int_Handler(VOID);

/**
Set SR.<br>
@param [in]	hdmi_layer		Layer SR status for HDMI block(common block 0).<br>
							The value corresponding to each block SR layer is set up.<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param [in]	lcd_mipi_layer	Layer SR status for LCD or MIPI block(common block 1).<br>
							The value corresponding to each block SR layer is set up.<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[in]	sr				SR to set.<br>
							<ul><li>@ref D_IM_DISP_SR_CANCEL
								<li>@ref D_IM_DISP_SR_RESET</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It must be interval (system's cycle/CPU's cycle + 2*CPU's cycle + 1) if the SR was written continuously.<br>
			That is 266 / 27 + 2 + 1 = 19<br>
			It's no any problem because the following instructions are over 19 assemble instruction.<br>
*/
extern INT32 Im_DISP_Set_SR(UINT32 hdmi_layer, UINT32 lcd_mipi_layer, BYTE sr);

/**
Get SR.<br>
@param[out]	hdmi_layer		Layer SR status for HDMI block(common block 0).<br>
							The value corresponding to each block SR layer is set up.<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@param[out]	lcd_mipi_layer	Layer SR status for LCD or MIPI block(common block 1).<br>
							The value corresponding to each block SR layer is set up.<br>
							(Two or more layer can be specified by the OR value.)<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_NONE
								<li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
								<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
@retval	D_DDIM_OK						Success
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_SR(UINT32* hdmi_layer, UINT32* lcd_mipi_layer);

/**
Set parameter reflect timing.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	timing			Register parameter reflect timing.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG timing);

/**
Get parameter reflect timing.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	timing			Register parameter reflect timing.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG *timing);

/**
Set input data transfer.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	lidt			Input data forwarding setting register.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Input_Data_Transfer(E_IM_DISP_SEL block, U_IM_DISP_LIDT lidt);

/**
Get input data transfer.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	lidt			Input data forwarding setting register.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Input_Data_Transfer(E_IM_DISP_SEL block, U_IM_DISP_LIDT *lidt);

/**
Set input data size of MAIN.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	input_size		width and height of input data buffer.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>LISIZE.IHSIZE >= 128
				<li>LISIZE.IHSIZE is a multiple of 2
				<li>LDSTA.DSH + LISIZE.IHSIZE * (LRSZ1.HRSZM / LRSZ1.HRSZN) <= OHSIZE
				<li>LISIZE.IVSIZE >= 16
				<li>LISIZE.IVSIZE is a multiple of 2
				<li>LDSTA.DSV + LISIZE.IVSIZE * (LRSZ2.VRSZM / LRSZ2.VRSZN) <= OVSIZE
			</ul>
*/
extern INT32 Im_DISP_Set_Input_Size(E_IM_DISP_SEL block, U_IM_DISP_SIZE input_size);

/**
Get input data size of MAIN.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	input_size		Width and height of input data buffer.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Input_Size(E_IM_DISP_SEL block, U_IM_DISP_SIZE *input_size);

/**
Connect MAIN input data address with the bank index.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	bank_no			Bank No.<br>
							<ul><li>@ref E_IM_DISP_BANK_00
								<li>@ref E_IM_DISP_BANK_01
								<li>@ref E_IM_DISP_BANK_10
								<li>@ref E_IM_DISP_BANK_11</ul>
@param[in]	address			Data address.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>(in case of bit depth is 8-bit) LYSA and LCSA are 2-byte unit.<br>
				<li>(in case of bit depth is 12-bit) LYSA and LCSA are 3-byte unit.<br>
				<li>(in case of bit depth is 16-bit) LYSA and LCSA are 4-byte unit.<br>
			</ul>
*/
extern INT32 Im_DISP_Set_Addr(E_IM_DISP_SEL block, E_IM_DISP_BANK bank_no, T_IM_DISP_IMAGE_ADDR const *const address);

/**
Get MAIN input data address of the bank index specified.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	bank_no			Bank No.<br>
							<ul><li>@ref E_IM_DISP_BANK_00
								<li>@ref E_IM_DISP_BANK_01
								<li>@ref E_IM_DISP_BANK_10
								<li>@ref E_IM_DISP_BANK_11</ul>
@param[out]	address			Data address.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Addr(E_IM_DISP_SEL block, E_IM_DISP_BANK bank_no, T_IM_DISP_IMAGE_ADDR *const address);

/**
Set the global size.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	y_hga			Global width of Y.<br>
@param[in]	c_hga			Global width of CbCr.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>LYHGA > 0
				<li>LCHGA > 0
				<li>(in case of bit depth is 8-bit) LYHGA and LCHGA are 2-byte unit.<br>
				<li>(in case of bit depth is 12-bit) LYHGA and LCHGA are 3-byte unit.<br>
				<li>(in case of bit depth is 16-bit) LYHGA and LCHGA are 4-byte unit.<br>
			</ul>
*/
extern INT32 Im_DISP_Set_Lfd(E_IM_DISP_SEL block, ULONG y_hga, ULONG c_hga);

/**
Get the global size.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	y_hga			Global width of Y.<br>
@param[out]	c_hga			Global width of CbCr.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Lfd(E_IM_DISP_SEL block, ULONG* y_hga, ULONG* c_hga);

/**
Set main bank No. to show.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	bank_no			Bank No.<br>
							<ul><li>@ref E_IM_DISP_BANK_00
								<li>@ref E_IM_DISP_BANK_01
								<li>@ref E_IM_DISP_BANK_10
								<li>@ref E_IM_DISP_BANK_11</ul>
*/
extern VOID Im_DISP_Set_Main_Bank(E_IM_DISP_SEL block, E_IM_DISP_BANK bank_no);

/**
Get main bank No. to show or showing.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	bank_no			Bank No.<br>
							<ul><li>@ref E_IM_DISP_BANK_00
								<li>@ref E_IM_DISP_BANK_01
								<li>@ref E_IM_DISP_BANK_10
								<li>@ref E_IM_DISP_BANK_11</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Main_Bank(E_IM_DISP_SEL block, E_IM_DISP_BANK *bank_no);

/**
Get main bank No. to showing.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	bank_no			Bank No.<br>
							<ul><li>@ref E_IM_DISP_BANK_00
								<li>@ref E_IM_DISP_BANK_01
								<li>@ref E_IM_DISP_BANK_10
								<li>@ref E_IM_DISP_BANK_11</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Main_Bank_Monitor(E_IM_DISP_SEL block, E_IM_DISP_BANK *bank_no);

/**
Set resize value.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	resize			Resize value.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>LRSZ1.HRSZM / LRSZ1.HRSZN >= 0.5
				<li>LRSZ1.HRSZM / LRSZ1.HRSZN <= 8
				<li>(in the case of LRSZ1.HRSZM / LRSZ1.HRSZN = 1.0) LRSZ1.HRSZOF = 0
				<li>(in the case of LHRSZ0 = E_IM_DISP_RSZSL_PADDING_THINNING) LRSZ1.HRSZOF = 0
				<li>LRSZ1.HRSZOF < LISIZE.IHSIZE * LRSZ1.HRSZM - (OHSIZE - 1) * LRSZ1.HRSZN
				<li>LRSZ1.HRSZOF < LRSZ1.HRSZM
				<li>LRSZ2.VRSZM / LRSZ2.VRSZN >= 0.5
				<li>LRSZ2.VRSZM / LRSZ2.VRSZN <= 8
			</ul>
*/
extern INT32 Im_DISP_Set_Resize(E_IM_DISP_SEL block, T_IM_DISP_RESIZE const *const resize);

/**
Get resize value.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	resize			Resize value.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Resize(E_IM_DISP_SEL block, T_IM_DISP_RESIZE *const resize);

/**
Set YHLP data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	yhlp			YHLP data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Yhlp(E_IM_DISP_SEL block, T_IM_DISP_YHLP const *const yhlp);

/**
Get YHLP data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	yhlp			YHLP data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Yhlp(E_IM_DISP_SEL block, T_IM_DISP_YHLP *const yhlp);

/**
Set warning processing data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	warning			Warning processing data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>LYW0TH.YW0THH >= LYW0TH.YW0THL
				<li>LYW1TH.YW1THH >= LYW1TH.YW1THL
				<li>If LYWCTL.YWSL=0 or 1(Brightness warning),<br>
					don't set to 2 and 3 in LYW0ST.YW0MD/LYW1ST.YW1MD.
			</ul>
*/
extern INT32 Im_DISP_Set_Warning(E_IM_DISP_SEL block, T_IM_DISP_WARNING const *const warning);

/**
Get warning processing data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	warning			Warning processing data
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Warning(E_IM_DISP_SEL block, T_IM_DISP_WARNING *const warning);

/**
Set blink timer control data.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	lbltmr			Blinking timer.
*/
extern VOID Im_DISP_Set_Blink_Timer(E_IM_DISP_SEL block, U_IM_DISP_LBLTMR lbltmr);

/**
Get blink timer control data.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	lbltmr			Blinking timer.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Blink_Timer(E_IM_DISP_SEL block, U_IM_DISP_LBLTMR *lbltmr);


/**
Set zebra warning data.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	zebra			Zebra warning data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>LZBWID.ZBHH >= 2
				<li>LZBWID.ZBHL >= 2
			</ul>
*/
extern INT32 Im_DISP_Set_Zebra(E_IM_DISP_SEL block, T_IM_DISP_ZEBRA const *const zebra);

/**
Get zebra warning data.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	zebra			Zebra warning data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Zebra(E_IM_DISP_SEL block, T_IM_DISP_ZEBRA *const zebra);

/**
Set YCbCr -> RGB matrix.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	matrix			Matrix data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>Value range of COEFFICIENT0 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT0/64.<br>
				<li>Value range of COEFFICIENT1 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT1/64.<br>
				<li>Value range of COEFFICIENT2 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT2/64.<br>
			</ul>
*/
extern INT32 Im_DISP_Set_Matrix(E_IM_DISP_SEL block, const U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE]);

/**
Get YCbCr -> RGB matrix.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	matrix			Matrix data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Matrix(E_IM_DISP_SEL block, U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE]);

/**
Set gamma correct enable.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param [in] set_item		Correct selection.<br>
							<ul><li>@ref E_IM_DISP_CORRECT_SELECT_ANTI_GAMMA
								<li>@ref E_IM_DISP_CORRECT_SELECT_GAMMA_IN
								<li>@ref E_IM_DISP_CORRECT_SELECT_GAMMA_OUT
								<li>@ref E_IM_DISP_CORRECT_SELECT_LUMINANCE
								<li>@ref E_IM_DISP_CORRECT_SELECT_CHROMA</ul>
@param [in] correct			Correction table and correction enable.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Gamma_Enable(E_IM_DISP_SEL block, E_IM_DISP_CORRECT_SELECT set_item, T_IM_DISP_TBL_ACCESS_SET const *const correct);

/**
Get gamma correct enable.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	lch_ltblaset	Table RAM access setting (Main data input block).
@param[out]	dcore_tblaset	Table RAM access setting (Data output block).
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Gamma_Enable(E_IM_DISP_SEL block, U_IM_DISP_LTBLASET *lch_ltblaset, U_IM_DISP_TBLASET *dcore_tblaset);

/**
Set gamma correct mode.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	lgmmd			Gamma correct mode.
*/
extern VOID Im_DISP_Set_Gamma_Mode(E_IM_DISP_SEL block, U_IM_DISP_LGMMD lgmmd);

/**
Get gamma correct mode.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	lgmmd			Gamma correct mode.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Gamma_Mode(E_IM_DISP_SEL block, U_IM_DISP_LGMMD* lgmmd);

/**
Set Alpha value.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	lalp			Alpha value.
*/
extern VOID Im_DISP_Set_Alpha_Value(E_IM_DISP_SEL block, U_IM_DISP_LALP lalp);

/**
Get Alpha value.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	lalp			Alpha value.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Alpha_Value(E_IM_DISP_SEL block, U_IM_DISP_LALP *lalp);

/**
Set display position.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	position		Display position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>LDSTA.DSH is a multiple of 2
				<li>LDSTA.DSV is a multiple of 2
			</ul>
*/
extern INT32 Im_DISP_Set_Display_Position(E_IM_DISP_SEL block, U_IM_DISP_DSTA position);

/**
Get display position.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	position		Display position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Display_Position(E_IM_DISP_SEL block, U_IM_DISP_DSTA *position);

/**
Set reverse display setting.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	reverse			Reverse display selection.<br>
*/
extern VOID Im_DISP_Set_Reverse_Display(E_IM_DISP_SEL block, U_IM_DISP_LREVDISP reverse);

/**
Get reverse display setting.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	reverse			Reverse display selection.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Reverse_Display(E_IM_DISP_SEL block, U_IM_DISP_LREVDISP *reverse);

/**
Set display interface selection.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ifs				Display I/F Selection .<br>
							<ul><li>@ref D_IM_DISP_IFS_NONE
								<li>@ref D_IM_DISP_IFS_LCD
								<li>@ref D_IM_DISP_IFS_HDMI
								<li>@ref D_IM_DISP_IFS_MIPI</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_STATUS_ABNORMAL		Can not set because RESET.SR was not set to 1.
@remarks	The data output unit needs to be in a reset state.
*/
extern INT32 Im_DISP_Set_Display_Interface(E_IM_DISP_SEL block, BYTE ifs);

/**
Get display interface selection.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ifs				Display I/F Selection .<br>
							<ul><li>@ref D_IM_DISP_IFS_NONE
								<li>@ref D_IM_DISP_IFS_LCD
								<li>@ref D_IM_DISP_IFS_HDMI
								<li>@ref D_IM_DISP_IFS_MIPI</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Display_Interface(E_IM_DISP_SEL block, BYTE *ifs);


/**
Set external start-up mode.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	tgkmd			External start-up mode.<br>
							<ul><li>@ref E_IM_DISP_TGKMD_NORMAL
								<li>@ref E_IM_DISP_TGKMD_EXTERNAL</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_STATUS_ABNORMAL		Can not set because TRG.TRG is not stoped.
@remarks	The data output unit needs to be in a reset state.
*/
extern INT32 Im_DISP_Set_External_Startup_Mode(E_IM_DISP_SEL block, E_IM_DISP_TGKMD tgkmd);

/**
Get external start-up mode.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	tgkmd			External start-up mode.<br>
							<ul><li>@ref E_IM_DISP_TGKMD_NORMAL
								<li>@ref E_IM_DISP_TGKMD_EXTERNAL</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_External_Startup_Mode(E_IM_DISP_SEL block, E_IM_DISP_TGKMD* tgkmd);

/**
Set external start-up operation start timing.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	tgkst			Operation start setting.<br>
							Value range:0~4095.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_STATUS_ABNORMAL		Can not set because TRG.TRG is not stoped.
*/
extern INT32 Im_DISP_Set_External_Startup_Timing(E_IM_DISP_SEL block, ULONG tgkst);

/**
Get external start-up operation start timing.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	tgkst			Operation start setting.<br>
							Value range:0~4095.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_External_Startup_Timing(E_IM_DISP_SEL block, ULONG* tgkst);

/**
Set a flag generation timing of vertical synchronization signal.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	timing			Generation timing.<br>
							<ul><li>@ref E_IM_DISP_VSYNC_FLAG_NONE
								<li>@ref E_IM_DISP_VSYNC_FLAG_FIRST
								<li>@ref E_IM_DISP_VSYNC_FLAG_SECOND
								<li>@ref E_IM_DISP_VSYNC_FLAG_BOTH</ul>
*/
extern VOID Im_DISP_Set_Vsync_Int_Timing(E_IM_DISP_SEL block, E_IM_DISP_VSYNC_FLAG timing);

/**
Get a flag generation timing of vertical synchronization signal.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	timing			Generation timing.<br>
							<ul><li>@ref E_IM_DISP_VSYNC_FLAG_NONE
								<li>@ref E_IM_DISP_VSYNC_FLAG_FIRST
								<li>@ref E_IM_DISP_VSYNC_FLAG_SECOND
								<li>@ref E_IM_DISP_VSYNC_FLAG_BOTH</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Vsync_Int_Timing(E_IM_DISP_SEL block, E_IM_DISP_VSYNC_FLAG *timing);

/**
Set interruption demand permission setting.<br>
@param[in]	block					Common block selection.<br>
									<ul><li>@ref E_IM_DISP_HDMI
										<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	interruption_select		Bit map to select set item. <br>
									It must be set to a member of @ref E_IM_DISP_INTERRUPTION_SELECT or their OR value.<br>
@param[in]	interruption_state		The interruption demand permission setting.<br>
									<ul><li>@ref E_IM_DISP_INTERRUPTION_STATE_NONE
										<li>@ref E_IM_DISP_INTERRUPTION_STATE_SET</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Int(E_IM_DISP_SEL block, UINT32 interruption_select, E_IM_DISP_INTERRUPTION_STATE interruption_state);

/**
Get interruption demand permission setting.<br>
@param[in]	block					Common block selection.<br>
									<ul><li>@ref E_IM_DISP_HDMI
										<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	interruption_state		The interruption demand permission setting.<br>
									It should be AND with E_IM_DISP_INTERRUPTION_SELECT's member to get<br>
									the result.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Int(E_IM_DISP_SEL block, UINT32* interruption_state);

/**
Set interruption's call-back function pointer.<br>
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	interruption_select	Bit map to select set item. <br>
								It must be set to a member of @ref E_IM_DISP_INTERRUPTION_SELECT or their OR value.<br>
@param[in]	vp_callback			The call-back pointer to set. <br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Int_Callback(E_IM_DISP_SEL block, UINT32 interruption_select, VP_CALLBACK vp_callback);

/**
Get interruption's call-back function pointer.<br>
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	interruption_select	Bit map to select set item. <br>
								It must be set to a value exclude @ref E_IM_DISP_CORRECT_SELECT_ALL.<br>
@param[out]	vp_callback			The call-back pointer.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Int_Callback(E_IM_DISP_SEL block, E_IM_DISP_INTERRUPTION_SELECT interruption_select, VP_CALLBACK *vp_callback);

/**
Get AXI state.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	axi_state		AXI state.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_AXI_Status(E_IM_DISP_SEL block, U_IM_DISP_AXISTS *axi_state);

/**
Set parameter reflect timing.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	timing			Register parameter reflect timing.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy.
*/
extern INT32 Im_DISP_Set_Output_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG timing);

/**
Get parameter reflect timing.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	timing			Register parameter reflect timing.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Output_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_RPGTMG *timing);

/**
Set parameter reflect enable.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	enable			Register parameter reflect enable.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Output_Parameter_Reflect_Enable(E_IM_DISP_SEL block, UINT32 enable);

/**
Get parameter reflect enable.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	enable			Register parameter reflect enable.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Output_Parameter_Reflect_Enable(E_IM_DISP_SEL block, UINT32 *enable);

/**
Set transmission mode.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	tsl				Register parameter transmission mode.<br>
							<ul><li>@ref D_IM_DISP_TSL_PROGRESSIVE
								<li>@ref D_IM_DISP_TSL_INTERLACE</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy.
*/
extern INT32 Im_DISP_Set_TSL(E_IM_DISP_SEL block, BYTE tsl);

/**
Get transmission mode.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	tsl				Register parameter transmission mode.<br>
							<ul><li>@ref D_IM_DISP_TSL_PROGRESSIVE
								<li>@ref D_IM_DISP_TSL_INTERLACE</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_TSL(E_IM_DISP_SEL block, BYTE *tsl);

/**
Get output size.<br>
@param[in]	block		Common block selection.<br>
						<ul><li>@ref E_IM_DISP_HDMI
							<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ovsize		Vertical output size
@param[out]	ohsize		Horizonal output size
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Output_Size(E_IM_DISP_SEL block, ULONG *ovsize, ULONG *ohsize);

/**
Switch force data out.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	is_enable		0 mean disable and 1 mean enable.<br>
							<ul><li>@ref E_IM_DISP_FDOEN_ORDINARY_OUT
								<li>@ref E_IM_DISP_FDOEN_FORCE_DATA_OUT</ul>
*/
extern VOID Im_DISP_Set_Force_Out_Data_Enable(E_IM_DISP_SEL block, E_IM_DISP_FDOEN is_enable);

/**
Get force data out state.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	is_enable		0 mean disable and 1 mean enable.<br>
							<ul><li>@ref E_IM_DISP_FDOEN_ORDINARY_OUT
								<li>@ref E_IM_DISP_FDOEN_FORCE_DATA_OUT</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32  Im_DISP_Get_Force_Out_Data_Enable(E_IM_DISP_SEL block, E_IM_DISP_FDOEN *is_enable);

/**
Set force output data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	color_data		Force output color data.
*/
extern VOID Im_DISP_Set_Force_Out_Data(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR color_data);

/**
Get force output data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	color_data		Force output color data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Force_Out_Data(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR *color_data);

/**
Set color bar's width.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	clbhsize		Color bar's width.<br>
							Value range:0~65535<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>CLBHSIZE * 16 >= OHSIZE
			</ul>
*/
extern INT32 Im_DISP_Set_Color_Bar_Size(E_IM_DISP_SEL block, ULONG clbhsize);

/**
Get color bar's width.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	clbhsize		Color bar's width.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Color_Bar_Size(E_IM_DISP_SEL block, ULONG *clbhsize);

/**
Set color bar's color.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	clbdt			Color bar display color.<br>
@param[in]	clbdt_size		clbdt's size that must not be over 16.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Color_Bar(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR const *const clbdt, UINT32 clbdt_size);

/**
Get color bar's color.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	clbdt			The array of color bar
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Color_Bar(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR clbdt[D_IM_DISP_COLOR_BAR_COUNT]);

/**
Set blend order.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	blend			Blend order data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Blend(E_IM_DISP_SEL block, U_IM_DISP_BLDCTL blend);

/**
Get blend order.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	blend			Blend order data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Blend(E_IM_DISP_SEL block, U_IM_DISP_BLDCTL *blend);

/**
Set YCbCr <-> RGB matrix data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	kind			Matrix kind.<br>
							<ul><li>@ref E_IM_DISP_MATRIX_KIND_Y2R
								<li>@ref E_IM_DISP_MATRIX_KIND_R2Y</ul>
@param[in]	matrix			Matrix data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>Value range of COEFFICIENT0 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT0/64.<br>
				<li>Value range of COEFFICIENT1 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT1/64.<br>
				<li>Value range of COEFFICIENT2 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT2/64.<br>
			</ul>
*/
extern INT32 Im_DISP_Set_Output_Matrix(E_IM_DISP_SEL block, E_IM_DISP_MATRIX_KIND kind, const U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE]);

/**
Get YCbCr <-> RGB matrix data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	kind			Matrix kind.<br>
							<ul><li>@ref E_IM_DISP_MATRIX_KIND_Y2R
								<li>@ref E_IM_DISP_MATRIX_KIND_R2Y</ul>
@param[out]	matrix			Matrix data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Output_Matrix(E_IM_DISP_SEL block, E_IM_DISP_MATRIX_KIND kind, U_IM_DISP_YR_MATRIX_COEFFICIENT matrix[D_IM_DISP_MATRIX_SIZE]);

/**
Set clip/gain/offset.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	clip_cal		clip/gain/offset data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>Value range of y_cal.gain is 0~255.<br>
					The gain value is y_cal.gain/256.<br>
					The gain value is 1(=256/256) when y_cal.gain is 0.<br>
				<li>Value range of cb_cal.gain is 0~255.<br>
					The gain value is cb_cal.gain/128.<br>
					The gain value is 2(=256/128) when cb_cal.gain is 0.<br>
				<li>Value range of cr_cal.gain is 0~255.<br>
					The gain value is cr_cal.gain/128.<br>
					The gain value is 2(=256/128) when cr_cal.gain is 0.<br>
				<li>Value range of y_cal.ofs is 0~8191.<br>
				<li>Value range of cb_cal.ofs is -4096~4095.<br>
				<li>Value range of cr_cal.ofs is -4096~4095.<br>
			</ul>
*/
extern INT32 Im_DISP_Set_Clip(E_IM_DISP_SEL block, T_IM_DISP_CLIP_CAL const *const clip_cal);

/**
Get clip/gain/offset.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	clip_cal		clip/gain/offset data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Clip(E_IM_DISP_SEL block, T_IM_DISP_CLIP_CAL *const clip_cal);

/**
Set RGB/YCbCr's output order.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	doctl0			RGB/YCbCr's output order.
*/
extern VOID Im_DISP_Set_Output_Order(E_IM_DISP_SEL block, U_IM_DISP_DOCTL0 doctl0);

/**
Get RGB/YCbCr's output order.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	doctl0			RGB/YCbCr's output order.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Output_Order(E_IM_DISP_SEL block, U_IM_DISP_DOCTL0 *doctl0);

/**
Set grid data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	grid			Grid data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GHDSTA.DSH is a multiple of 2
				<li>GHDSTA.DSV is a multiple of 2
				<li>GVDSTA.DSH is a multiple of 2
				<li>GVDSTA.DSV is a multiple of 2
				<li>GLENGTH.GHLEN is a multiple of 2
				<li>GLENGTH.GHLEN >= 8
				<li>GLENGTH.GVLEN is a multiple of 2
				<li>GLENGTH.GVLEN >= 8
				<li>GWIDTH.GHWID is a multiple of 2
				<li>GWIDTH.GHWID >= 2
				<li>GWIDTH.GVWID is a multiple of 2
				<li>GWIDTH.GVWID >= 2
				<li>GITVL.GHITV is a multiple of 2
				<li>GITVL.GHITV >= GWIDTH.GHWID + 2
				<li>GITVL.GVITV is a multiple of 2
				<li>GITVL.GVITV >= GWIDTH.GVWID + 2
			</ul>
*/
extern INT32 Im_DISP_Set_Grid(E_IM_DISP_SEL block, T_IM_DISP_GRID const *const grid);

/**
Get grid data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	grid			Grid data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Grid(E_IM_DISP_SEL block, T_IM_DISP_GRID *const grid);

/**
Set grid line display enable.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	gdispen			Grid display enable.<br>
							<ul><li>@ref E_IM_DISP_GDISPEN_NOT_DISPLAY
								<li>@ref E_IM_DISP_GDISPEN_DISPLAY</ul>
*/
extern VOID Im_DISP_Set_Grid_Enable(E_IM_DISP_SEL block, E_IM_DISP_GDISPEN gdispen);

/**
Get grid line display enable.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	gdispen			Grid display enable.<br>
							<ul><li>@ref E_IM_DISP_GDISPEN_NOT_DISPLAY
								<li>@ref E_IM_DISP_GDISPEN_DISPLAY</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Grid_Enable(E_IM_DISP_SEL block, E_IM_DISP_GDISPEN *gdispen);

/**
Set face frame data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	frame_count		Face frame data count.<br>
							Value range:1-42<br>
@param[in]	face			Face frame data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>FFDSTA.FFDSH is a multiple of 2
				<li>FFDSTA.FFDSV is a multiple of 2
				<li>FFSIZE.FFHSZ is a multiple of 2
				<li>FFSIZE.FFHSZ >= 8
				<li>FFSIZE.FFVSZ is a multiple of 2
				<li>FFSIZE.FFVSZ >= 8
				<li>FFWIDTH.FFHWID is a multiple of 2
				<li>FFWIDTH.FFHWID >= 2
				<li>FFWIDTH.FFVWID is a multiple of 2
				<li>FFWIDTH.FFVWID >= 2
			</ul>
*/
extern INT32 Im_DISP_Set_Face_Frame(E_IM_DISP_SEL block, UINT32 frame_count, T_IM_DISP_FACE_FRAME const *const face);

/**
Get face frame data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	face			Face frame data.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Face_Frame(E_IM_DISP_SEL block, T_IM_DISP_FACE_FRAME *const face);

/**
Set face frame display enable.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ffden			Face frame display enable.<br>
							bit d[0]:Face frame 0 ~ d[41]:Face frame 41<br>
							value 0:disable, 1:enable<br>
							(Two or more frame can be specified by the OR value.)<br>
@param[in]	msff			FFDISPEN.MSFF: Top face frame setting register.<br>
							Value range:0~41 (face frame 0 ~ face frame 41)
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Face_Frame_Enable(E_IM_DISP_SEL block, UINT64 ffden, BYTE msff);

/**
Get face frame display enable.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	ffden			Face frame display enable.<br>
							bit d[0]:Face frame 0 ~ d[41]:Face frame 41<br>
							value 0:disable, 1:enable<br>
							(Two or more frame can be specified by the OR value.)<br>
@param[out]	msff			FFDISPEN.MSFF: Top face frame setting register.<br>
							Value range:0~41 (face frame 0 ~ face frame 41)
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Face_Frame_Enable(E_IM_DISP_SEL block, UINT64 *ffden, BYTE *msff);

/**
Set parameter reflect timing of a OSD layer.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	timing			Register parameter reflect timing.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_OSD_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG timing);

/**
Get parameter reflect timing of a OSD layer.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	timing			Register parameter reflect timing.<br>
							<ul><li>@ref E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP
								<li>@ref E_IM_DISP_RPGTMG_VSYNC_VSYNC</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Parameter_Reflect_Timing(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_RPGTMG *timing);

/**
Set input data transfer of a OSD layer.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	gridt			Input data forwarding setting register.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRIDT.IFMT <= @ref D_IM_DISP_GRIDT_IFMT_RGBA5551
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Input_Data_Transfer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRIDT gridt);

/**
Get input data transfer of a OSD layer.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	gridt			Input data forwarding setting register.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Input_Data_Transfer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRIDT *gridt);

/**
Set OSD data territory size.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	territory_size	Width and height of OSD data territory.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRTISIZE.TIHSIZE is a multiple of 2
				<li>GRTISIZE.TIHSIZE >= 8
				<li>GRDSTA.DSH + GRTISIZE.TIHSIZE * (GRRSZ1.HRSZM / GRRSZ1.HRSZN) <= OHSIZE
				<li>GRTISIZE.TIVSIZE is a multiple of 2
				<li>GRTISIZE.TIVSIZE >= 8
				<li>GRDSTA.DSV + GRTISIZE.TIVSIZE * (GRRSZ2.VRSZM / GRRSZ2.VRSZN) <= OVSIZE
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Territory_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_SIZE territory_size);

/**
Get OSD data territory size.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	territory_size	Width and height of OSD data territory.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Territory_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_SIZE *territory_size);

/**
Set display position for OSD.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	position		Display position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRTDSTA.DSH is a multiple of 2
				<li>GRTDSTA.DSV is a multiple of 2
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Territory_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_DSTA position);

/**
Get display position for OSD.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	position		Display position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Territory_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_DSTA* position);

/**
Set input data area to access.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	area			Input data area.<br>
							<ul><li>@ref E_IM_DISP_INPUT_DATA_AREA_0
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_1
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_2
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_3</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_OSD_Area0_Switch(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_INPUT_DATA_AREA area);

/**
Get input data area to access.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	area			Input data area.<br>
							<ul><li>@ref E_IM_DISP_INPUT_DATA_AREA_0
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_1
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_2
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_3</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Area0_Switch(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_INPUT_DATA_AREA *area);

/**
Get input data area inder processing.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	area			Input data area.<br>
							<ul><li>@ref E_IM_DISP_INPUT_DATA_AREA_0
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_1
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_2
								<li>@ref E_IM_DISP_INPUT_DATA_AREA_3</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Area0_Monitor(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_INPUT_DATA_AREA *area);

/**
Set input data size of OSD.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	input_size		Width and height of input data buffer.<br>
@param[in]	input_size_count input_size's number.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRISIZE.IHSIZE is a multiple of 4
				<li>GRISIZE.IHSIZE >= 8
				<li>GRISIZE.IVSIZE is a multiple of 2
				<li>GRISIZE.IVSIZE >= 8
				<li>GRDSTA.DSH + GRISIZE.IHSIZE <= GRTISIZE.TIHSIZE
				<li>GRDSTA.DSV + GRISIZE.IVSIZE <= GRTISIZE.TIVSIZE
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Area_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, 
								U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT],
								UINT32 input_size_count);
/**
Get input data size of OSD.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	input_size		Width and height of input data buffer.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Area_Size(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_SIZE input_size[D_IM_DISP_OSD_DISPLAY_AREA_COUNT]);

/**
Connect OSD input data address with the bank index.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	bank_no			OSD start address bank no.<br>
							<ul><li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_0
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_1
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_2
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_3
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_1
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_2
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_3
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_4
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_5
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_6
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_7
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_8
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_9</ul>
@param[in]	address			OSD start address.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRSA0 is a multiple of 8
				<li>GRSA is a multiple of 8
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Area_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_OSD_SA_BANK_NO bank_no, ULONG address);

/**
Get OSD input data address of the bank index specified.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	bank_no			OSD start address bank no.<br>
							<ul><li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_0
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_1
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_2
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_0_3
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_1
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_2
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_3
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_4
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_5
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_6
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_7
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_8
								<li>@ref E_IM_DISP_OSD_SA_BANK_NO_9</ul>
@param[out]	address			OSD start address.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Area_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_OSD_SA_BANK_NO bank_no, ULONG *address);

/**
Connect OSD input data address (A data) with the bank index.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	bank_no			Bank No. Value range:0~9.<br>
@param[in]	address			OSD A data start address.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRSA0 is a multiple of 8
				<li>GRSA is a multiple of 8
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Area_Adata_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG address);

/**
Get OSD input data address (A data) of the bank index specified.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	bank_no			Bank No. Value range:0~9.<br>
@param[out]	address			OSD A data start address.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Area_Adata_Addr(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG *address);

/**
Set the horizontal global area width.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	bank_no			Bank No. Value range:0~9.<br>
@param[in]	width			Horizontal global area width.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRHGA > 0
				<li>GRHGA <= 0xFFF8
				<li>GRHGA is a multiple of 8
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG width);

/**
Get the horizontal global area width.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	bank_no			Bank No.<br>
@param[out]	width			Horizontal global area width.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG *width);

/**
Set the A data horizontal global area width.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	bank_no			Bank No. Value range:0~9.<br>
@param[in]	width			A data horizontal global area width.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRHGA > 0
				<li>GRHGA <= 0xFFF8
				<li>GRHGA is a multiple of 8
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Adata_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG width);

/**
Get the A data horizontal global area width.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	bank_no			Bank No.<br>
@param[out]	width			A data horizontal global area width.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Adata_Lfd(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, ULONG *width);

/**
Set input start position.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	bank_no			Bank No. Value range:0~9.
@param[in]	position		Input start position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRDSTA.DSH is a multiple of 2
				<li>GRDSTA.DSH is a multiple of 2
				<li>GRDSTA.DSH_0 <= DSH_1 <= DSH_2 ... <= DSH_9 (in order of bank_no)
				<li>(in case of same DSH) GRDSTA.DSV_0 <= DSV_1 <= DSV_2 ... <= DSV_9
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Display_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, U_IM_DISP_DSTA position);

/**
Get input start position.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	bank_no			Bank No.
@param[out]	position		Input start position.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Display_Position(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, UINT32 bank_no, U_IM_DISP_DSTA* position);

/**
Set input data's order.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in]	ipo				Input data order.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRIPO.IPO1, GRIPO.IPO2, GRIPO.IPO3 and GRIPO.IPO4 do not overlap
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_IPO(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRIPO ipo);

/**
Get input data's order.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	ipo		Input data order.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_IPO(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRIPO* ipo);

/**
Set GR's horizontal rectangle to showing or hiden.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	area_enable		Enable show area.<br>
							bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
							value 0:disable, 1:enable<br>
							(Two or more area can be specified by the OR value.)<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_OSD_Area_Enable(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG area_enable);

/**
Get GR's horizontal rectangle to showing or hiden.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	area_enable		Enable show area.<br>
							bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
							value 0:disable, 1:enable<br>
							(Two or more area can be specified by the OR value.)<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Area_Enable(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG *area_enable);

/**
Set blink method selection.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	blink			Blink method selection.<br>
							bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
							value 0:not blink, 1:blink<br>
							(Two or more area can be specified by the OR value.)<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_OSD_Blink(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG blink);

/**
Get blink method selection.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	blink			Blink method selection.<br>
							bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
							value 0:not blink, 1:blink<br>
							(Two or more area can be specified by the OR value.)<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Blink(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG *blink);

/**
Set GR's blink period.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	grblink			Set data which specified by set_item.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks The item which was not specified by set_item would not be set to the register.
*/
extern INT32 Im_DISP_Set_OSD_Blink_Timer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRBLINK grblink);

/**
Get GR's blink period.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	grblink			Get data which specified by set_item.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Blink_Timer(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRBLINK *grblink);

/**
Set GR's blink sync setting.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	grbinit			Blink sync setting.<br>
							bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
							value 0:not blink sync, 1:blink sync<br>
							(Two or more area can be specified by the OR value.)<br>
@param[in]	grbitrg			Blink sync trigger.<br>
							<ul><li>@ref D_IM_DISP_BITRG_NON
								<li>@ref D_IM_DISP_BITRG_WRITE_SYNC_WAIT</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks Can not D_IM_DISP_BITRG_WRITE_SYNC_WAIT writing to Blink sync trigger in the blink sync reflect wait state.
*/
extern INT32 Im_DISP_Set_OSD_Blink_Sync(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG grbinit, ULONG grbitrg);

/**
Get GR's blink sync setting.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	grbinit			Blink sync setting.<br>
							bit d[0]:Display area 0 ~ d[9]:Display area 9<br>
							value 0:not blink sync, 1:blink sync<br>
							(Two or more area can be specified by the OR value.)<br>
@param[out]	grbitrg			Blink sync trigger.<br>
							<ul><li>@ref D_IM_DISP_BITRG_READ_STOP
								<li>@ref D_IM_DISP_BITRG_READ_SYNC_WAIT</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Blink_Sync(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG* grbinit, ULONG* grbitrg);

/**
Set resize value.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	resize			Resize value.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>GRRSZ1.HRSZM / GRRSZ1.HRSZN >= 0.5
				<li>GRRSZ1.HRSZM / GRRSZ1.HRSZN <= 8
				<li>(in the case of GRRSZ1.HRSZM / GRRSZ1.HRSZN = 1.0) GRRSZ1.HRSZOF = 0
				<li>(in the case of GRHRSZ0 = E_IM_DISP_RSZSL_PADDING_THINNING) GRRSZ1.HRSZOF = 0
				<li>GRRSZ1.HRSZOF < GRISIZE.IHSIZE * GRRSZ1.HRSZM - (pixel count after horizontal resize -1) * GRRSZ1.HRSZN
				<li>GRRSZ1.HRSZOF < GRRSZ1.HRSZM
				<li>GRRSZ2.VRSZM / GRRSZ2.VRSZN >= 0.5
				<li>GRRSZ2.VRSZM / GRRSZ2.VRSZN <= 8
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Resize(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, T_IM_DISP_RESIZE const *const resize);

/**
Get resize value.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	resize			Resize value.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Resize(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, T_IM_DISP_RESIZE *const resize);

/**
Set YCbCr -> RGB matrix data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	gry2r			Matrix data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@remarks	It is necessary to fulfill the following conditions. <br>
			<ul><li>Value range of COEFFICIENT0 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT0/64.<br>
				<li>Value range of COEFFICIENT1 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT1/64.<br>
				<li>Value range of COEFFICIENT2 is -128~127.<br>
					The matrix coefficient value is COEFFICIENT2/64.<br>
			</ul>
*/
extern INT32 Im_DISP_Set_OSD_Matrix(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRY2R gry2r[D_IM_DISP_MATRIX_SIZE]);

/**
Get YCbCr -> RGB matrix data.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	gry2r			Matrix data.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Matrix(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, U_IM_DISP_GRY2R gry2r[D_IM_DISP_MATRIX_SIZE]);

/**
Set GR's Alpha.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL</ul>
@param[in]	alpha			GR's Alpha. Value range:0~255<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_OSD_Alpha(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG alpha);

/**
Get GR's Alpha.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			OSD layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[out]	alpha			GR's Alpha.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_OSD_Alpha(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, ULONG* alpha);

/**
Set Anti-gamma table.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Anti-gamma table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[in]	tbl				Anti gamma table.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Anti_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_ANTI_GAMMA_TBL const *const tbl);

/**
Get Anti-gamma table.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Anti-gamma table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[out]	tbl				Anti gamma table.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Get_Anti_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_ANTI_GAMMA_TBL *const tbl);

/**
Set Gamma table (Main data input block).
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Gamma table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[in]	tbl				Gamma table (Main data input block).
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Main_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_IN const *const tbl);

/**
Get Gamma table (Main data input block).
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Gamma table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[out]	tbl				Gamma table (Main data input block).
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Get_Main_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_IN *const tbl);

/**
Set Gamma table (Data output block).
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Gamma table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[in]	tbl				Gamma table (Data output block).
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Dcore_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_OUT const *const tbl);

/**
Get Gamma table (Data output block).
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Gamma table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[out]	tbl				Gamma table (Data output block).
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Get_Dcore_Gamma_Table(E_IM_DISP_SEL block, BYTE surface, T_IM_DISP_GAMMA_TBL_OUT *const tbl);

/**
Set Luminance correct table.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Luminance correct table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[in]	tbl				Luminance correct table.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Luminance_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl);

/**
Get Luminance correct table.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Luminance correct table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[out]	tbl				Luminance correct table.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Get_Luminance_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl);

/**
Set Chroma correct gain table.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Chroma correct gain table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[in]	tbl				Chroma correct gain table.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Chroma_Gain_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl);

/**
Get Chroma correct gain table.
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	surface			Chroma correct gain table surface.<br>
							<ul><li>@ref D_IM_DISP_TABLE_SURFACE_A
								<li>@ref D_IM_DISP_TABLE_SURFACE_B</ul>
@param[out]	tbl				Chroma correct gain table.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Get_Chroma_Gain_Table(E_IM_DISP_SEL block, BYTE surface, USHORT* tbl);

/**
Set MIPI-DSI IP input pin control 0.
@param[in]	shudown			Snutdown input control.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@param[in]	color_mode		Color mode input control.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
@retval	D_IM_DISP_MACRO_BUSY_NG			Fail - Macro busy
*/
extern INT32 Im_DISP_Set_Mipi_Dsi_Ctrl0(BYTE shudown, BYTE color_mode);

/**
Get MIPI-DSI IP input pin control 0.
@param[out]	shudown			Snutdown input control.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@param[out]	color_mode		Color mode input control.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Mipi_Dsi_Ctrl0(BYTE* shudown, BYTE* color_mode);

/**
Set MIPI-DSI IP input pin control 1 (DPIUPDATECFG_I 1shot generation).
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_STATUS_ABNORMAL		DPI configuration update waiting.
*/
extern INT32 Im_DISP_Set_Mipi_Dsi_Ctrl1(void);

/**
Get MIPI-DSI IP input pin control 1.
@param[out]	update_config	Update configuration input control.<br>
							<ul><li>@ref D_IM_DISP_MDSUDC_READ_STOP
								<li>@ref D_IM_DISP_MDSUDC_READ_UPDATE_WAIT</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Mipi_Dsi_Ctrl1(BYTE* update_config);

/**
Set SR of IP (HDMI, MIPI).
@param[in]	ip	IP selection.<br>
				<ul><li>@ref D_IM_DISP_IP_HDMI
					<li>@ref D_IM_DISP_IP_MIPI</ul>
@param[in]	sr	SR to set.<br>
				<ul><li>@ref D_IM_DISP_SR_CANCEL
					<li>@ref D_IM_DISP_SR_RESET</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_SR_IP(BYTE ip, BYTE sr);

/**
Get SR of IP (HDMI, MIPI).
@param[out]	sr	SR status.<br>
				The value corresponding to each IP SR is set up.<br>
				(Two or more IP can be specified by the OR value.)<br>
				<ul><li>@ref D_IM_DISP_IP_HDMI
					<li>@ref D_IM_DISP_IP_MIPI</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_SR_IP(BYTE *sr);

/**
Set Write Channel control.
@param[in]	number	Write channel block number selection.<br>
					<ul><li>@ref E_IM_DISP_WC_NUM_0
						<li>@ref E_IM_DISP_WC_NUM_1</ul>
@param[in]	p2m		P2M macro setting parameters.<br>
@param[in]	pwch	PWCH macro setting parameters.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Set_Write_Channel_Ctrl(E_IM_DISP_WC_NUM number, T_IM_DISP_CTRL_P2M const *const p2m, T_IM_DISP_CTRL_PWCH const *const pwch);

/**
Get Write Channel control.
@param[in]	number	Write channel block number selection.<br>
					<ul><li>@ref E_IM_DISP_WC_NUM_0
						<li>@ref E_IM_DISP_WC_NUM_1</ul>
@param[out]	p2m		Register set value of P2M macro.<br>
@param[out]	pwch	Register set value of PWCH macro.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Write_Channel_Ctrl(E_IM_DISP_WC_NUM number, T_IM_DISP_CTRL_P2M  *const p2m, T_IM_DISP_CTRL_PWCH *const pwch);

/**
Get write channel response.
@param[in]	number		Write channel block number selection.<br>
						<ul><li>@ref E_IM_DISP_WC_NUM_0
							<li>@ref E_IM_DISP_WC_NUM_1</ul>
@param[out]	response	PWCH macro of AXI status (response value).<br>
						Value:<br>
						00b:Normal access success or Exclusive failure.<br>
						01b:Exclusive access.<br>
						10b:Slave error.<br>
						11b:Decode error.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Get_Write_Channel_Response(E_IM_DISP_WC_NUM number, ULONG *response);


#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Fucntions
@{
*/
//---------------------- utility section -------------------------------
/**
Set CSC matrix to covert from BT.601 to BT.709's RGB or no any convert.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	layer			layer selection.<br>
							<ul><li>@ref E_IM_DISP_SEL_LAYER_MAIN
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
								<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
								<li>@ref E_IM_DISP_SEL_LAYER_DCORE</ul>
@param[in]	convert			Convert type.<br>
							<ul><li>@ref E_IM_DISP_CC_MATRIX_NOCONVERT
								<li>@ref E_IM_DISP_CC_MATRIX_709</ul>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Change_CSC_Matrix(E_IM_DISP_SEL block, E_IM_DISP_SEL_LAYER layer, E_IM_DISP_CC_MATRIX convert);

/**
Change color bar's color which was used as black back.
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	black_back_color	The color used as black back.
*/
extern VOID Im_DISP_Change_BB_Color(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR black_back_color);

/**
Get the color bar's color which was used as black back.
@param[in]	block				Common block selection.<br>
								<ul><li>@ref E_IM_DISP_HDMI
									<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[out]	black_back_color	The color used as black back.
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Check_BB_Color(E_IM_DISP_SEL block, U_IM_DISP_IMAGE_COLOR* black_back_color);

/**
Change grid.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	ghnum			Number of horizontal grid line.<br>
							Value range:0~7.<br>
@param[in]	gvnum			Number of vertical grid line.<br>
							Value range:0~7.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Change_Grid(E_IM_DISP_SEL block, UINT32 ghnum, UINT32 gvnum);

/**
Draw single face frame.<br>
@param[in]	block			Common block selection.<br>
							<ul><li>@ref E_IM_DISP_HDMI
								<li>@ref E_IM_DISP_LCD_MIPI</ul>
@param[in]	index			Face frame data index.<br>
							Value range:0~15.<br>
@param[in]	enable			Face frame display enable.<br>
							<ul><li>@ref D_IM_DISP_ENABLE_OFF
								<li>@ref D_IM_DISP_ENABLE_ON</ul>
@param[in]	face			Face frame data parameter.<br>
@retval	D_DDIM_OK						Success.
@retval	D_IM_DISP_INPUT_PARAM_ERROR		Input parameter error.
*/
extern INT32 Im_DISP_Draw_Face_Frame_Single(E_IM_DISP_SEL block, INT32 index, INT32 enable, T_IM_DISP_FACE_FRAME_PARAM const *const face);

//---------------------- colabo  section -------------------------------
/*		@}*/ /* end of im_disp_util */
#endif // CO_DDIM_UTILITY_USE
#ifdef __cplusplus
}
#endif	// __cplusplus

#endif//_IM_DISP_H_

/** @}*//* end of im_disp*/

/** @weakgroup im_disp
@{
<hr>
@section im_disp_Initialize Initialize DISP
This is a sample for Initialize DISP while system start up<br>
@code
// SAMPLE CODE //
VOID im_disp_sample_initialize(VOID)
{
	INT32									ret;
	INT32									i;
	T_IM_DISP_CTRL_MAIN_TBL					ctrl_tbl_main_lcd;
	T_IM_DISP_CTRL_OUTPUT_TBL				ctrl_tbl_out_lcd;
	
	T_IM_DISP_CTRL_MAIN_LAYER_TRG_LIMIT		ctrl_trg_limit_main_lcd;
	T_IM_DISP_CTRL_MAIN_LAYER				ctrl_main_lcd;
	
	T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT			ctrl_trg_limit_out_lcd;
	T_IM_DISP_CTRL_OUTPUT					ctrl_out_lcd;
	
	E_IM_DISP_RPGTMG						grrpgctl;
	T_IM_DISP_CTRL_OSD_LAYER				ctrl_osd_lcd;
	
	UINT32 r_anti_tbl_a[32/2] = {	0x00040000, 0x000C0008, 0x00140010, 0x001C0018,
									0x00240020, 0x002C0028, 0x002C0028, 0x003C0038,
									0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
									0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 g_anti_tbl_a[32/2] = {	0x00080004, 0x0010000C, 0x00180014, 0x0020001C,
									0x00280024, 0x0030002C, 0x00380034, 0x0040003C,
									0x00480044, 0x0050004C, 0x00580054, 0x0060005C,
									0x00680064, 0x0070006C, 0x00780074, 0x0080007C
	};
	UINT32 b_anti_tbl_a[32/2] = {	0x00040000, 0x000C0008, 0x00140010, 0x001C0018,
									0x00240020, 0x002C0028, 0x00340030, 0x003C0038,
									0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
									0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 r_anti_tbl_b[32/2] = {	0x00040000, 0x000C0008, 0x00140010, 0x001C0018,
									0x00240020, 0x002C0028, 0x002C0028, 0x003C0038,
									0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
									0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 g_anti_tbl_b[32/2] = {	0x00080004, 0x0010000C, 0x00180014, 0x0020001C,
									0x00280024, 0x0030002C, 0x00380034, 0x0040003C,
									0x00480044, 0x0050004C, 0x00580054, 0x0060005C,
									0x00680064, 0x0070006C, 0x00780074, 0x0080007C
	};
	UINT32 b_anti_tbl_b[32/2] = {	0x00040000, 0x000C0008, 0x00140010, 0x001C0018,
									0x00240020, 0x002C0028, 0x00340030, 0x003C0038,
									0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
									0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 r_gamma_tbl_a_in[32/2] = {	0x00040000, 0x000C0008, 0x000C0008, 0x001C0018,
										0x00240020, 0x002C0028, 0x00340030, 0x003C0038,
										0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
										0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 g_gamma_tbl_a_in[32/2] = {	0x00080004, 0x0010000C, 0x00180014, 0x0020001C,
										0x00280024, 0x0030002C, 0x00380034, 0x0040003C,
										0x00480044, 0x0050004C, 0x00580054, 0x0060005C,
										0x00680064, 0x0070006C, 0x00780074, 0x0080007C
	};
	UINT32 b_gamma_tbl_a_in[32/2] = {	0x00040000, 0x000C0008, 0x00140010, 0x001C0018,
										0x00240020, 0x002C0028, 0x00340030, 0x003C0038,
										0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
										0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 r_gamma_tbl_b_in[32/2] = {	0x00040000, 0x000C0008, 0x000C0008, 0x001C0018,
										0x00240020, 0x002C0028, 0x00340030, 0x003C0038,
										0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
										0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	UINT32 g_gamma_tbl_b_in[32/2] = {	0x00080004, 0x0010000C, 0x00180014, 0x0020001C,
										0x00280024, 0x0030002C, 0x00380034, 0x0040003C,
										0x00480044, 0x0050004C, 0x00580054, 0x0060005C,
										0x00680064, 0x0070006C, 0x00780074, 0x0080007C
	};
	UINT32 b_gamma_tbl_b_in[32/2] = {	0x00040000, 0x000C0008, 0x00140010, 0x001C0018,
										0x00240020, 0x002C0028, 0x00340030, 0x003C0038,
										0x00440040, 0x004C0048, 0x00540050, 0x005C0058,
										0x00640060, 0x006C0068, 0x00740070, 0x007C0078
	};
	USHORT r_gamma_tbl_a_out[33] = {	0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C,
										0x0020, 0x0024, 0x0028, 0x002C, 0x0030, 0x0034, 0x0038, 0x003C,
										0x0040, 0x0044, 0x0048, 0x004C, 0x0050, 0x0054, 0x0058, 0x005C,
										0x0060, 0x0064, 0x0068, 0x006C, 0x0070, 0x0074, 0x0078, 0x007C, 0x0080
	};
	USHORT g_gamma_tbl_a_out[33] = {	0x0080, 0x0084, 0x0088, 0x008C, 0x0090, 0x0094, 0x0098, 0x009C,
										0x00A0, 0x00A4, 0x00A8, 0x00AC, 0x00B0, 0x00B4, 0x00B8, 0x00BC,
										0x00C0, 0x00C4, 0x00C8, 0x00CC, 0x00D0, 0x00D4, 0x00D8, 0x00DC,
										0x00E0, 0x00E4, 0x00E8, 0x00EC, 0x00F0, 0x00F4, 0x00F8, 0x00FC, 0x00FF
	};
	USHORT b_gamma_tbl_a_out[33] = {	0x0010, 0x0014, 0x0018, 0x001C, 0x0030, 0x0034, 0x0038, 0x003C,
										0x0050, 0x0054, 0x0058, 0x005C, 0x0070, 0x0074, 0x0078, 0x007C,
										0x0020, 0x0024, 0x0028, 0x002C, 0x0040, 0x0044, 0x0048, 0x004C,
										0x0060, 0x0064, 0x0068, 0x006C, 0x0080, 0x0084, 0x0088, 0x008C, 0x0090
	};
	USHORT r_gamma_tbl_b_out[33] = {	0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C,
										0x0020, 0x0024, 0x0028, 0x002C, 0x0030, 0x0034, 0x0038, 0x003C,
										0x0040, 0x0044, 0x0048, 0x004C, 0x0050, 0x0054, 0x0058, 0x005C,
										0x0060, 0x0064, 0x0068, 0x006C, 0x0070, 0x0074, 0x0078, 0x007C, 0x0080
	};
	USHORT g_gamma_tbl_b_out[33] = {	0x0080, 0x0084, 0x0088, 0x008C, 0x0090, 0x0094, 0x0098, 0x009C,
										0x00A0, 0x00A4, 0x00A8, 0x00AC, 0x00B0, 0x00B4, 0x00B8, 0x00BC,
										0x00C0, 0x00C4, 0x00C8, 0x00CC, 0x00D0, 0x00D4, 0x00D8, 0x00DC,
										0x00E0, 0x00E4, 0x00E8, 0x00EC, 0x00F0, 0x00F4, 0x00F8, 0x00FC, 0x00FF
	};
	USHORT b_gamma_tbl_b_out[33] = {	0x0010, 0x0014, 0x0018, 0x001C, 0x0030, 0x0034, 0x0038, 0x003C,
										0x0050, 0x0054, 0x0058, 0x005C, 0x0070, 0x0074, 0x0078, 0x007C,
										0x0020, 0x0024, 0x0028, 0x002C, 0x0040, 0x0044, 0x0048, 0x004C,
										0x0060, 0x0064, 0x0068, 0x006C, 0x0080, 0x0084, 0x0088, 0x008C, 0x0090
	};
	USHORT luminance_a[33] = {	0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C,
								0x0020, 0x0024, 0x0028, 0x002C, 0x0030, 0x0034, 0x0038, 0x003C,
								0x0040, 0x0044, 0x0048, 0x004C, 0x0050, 0x0054, 0x0058, 0x005C,
								0x0060, 0x0064, 0x0068, 0x006C, 0x0070, 0x0074, 0x0078, 0x007C, 0x0080
	};
	USHORT luminance_b[33] = {	0x0080, 0x0084, 0x0088, 0x008C, 0x0090, 0x0094, 0x0098, 0x009C,
								0x00A0, 0x00A4, 0x00A8, 0x00AC, 0x00B0, 0x00B4, 0x00B8, 0x00BC,
								0x00C0, 0x00C4, 0x00C8, 0x00CC, 0x00D0, 0x00D4, 0x00D8, 0x00DC,
								0x00E0, 0x00E4, 0x00E8, 0x00EC, 0x00F0, 0x00F4, 0x00F8, 0x00FC, 0x00FF
	};
	USHORT chroma_a[33] = {	0x0000, 0x0004, 0x0008, 0x000C, 0x0010, 0x0014, 0x0018, 0x001C,
							0x0020, 0x0024, 0x0028, 0x002C, 0x0030, 0x0034, 0x0038, 0x003C,
							0x0040, 0x0044, 0x0048, 0x004C, 0x0050, 0x0054, 0x0058, 0x005C,
							0x0060, 0x0064, 0x0068, 0x006C, 0x0070, 0x0074, 0x0078, 0x007C, 0x0080
	};
	USHORT chroma_b[33] = {	0x0080, 0x0084, 0x0088, 0x008C, 0x0090, 0x0094, 0x0098, 0x009C,
							0x00A0, 0x00A4, 0x00A8, 0x00AC, 0x00B0, 0x00B4, 0x00B8, 0x00BC,
							0x00C0, 0x00C4, 0x00C8, 0x00CC, 0x00D0, 0x00D4, 0x00D8, 0x00DC,
							0x00E0, 0x00E4, 0x00E8, 0x00EC, 0x00F0, 0x00F4, 0x00F8, 0x00FC, 0x00FF
	};

	// Set display I/F selection
	Im_DISP_Set_Display_Interface(E_IM_DISP_LCD_MIPI, D_IM_DISP_IFS_LCD);

	// Clear software reset of DISP
	Im_DISP_Init();

	// DISP is initialize by default setting
	// Set main data input block table
	memset(&ctrl_tbl_main_lcd, 0, sizeof(ctrl_tbl_main_lcd));
	ctrl_tbl_main_lcd.anti_a.r_anti		= (USHORT*)r_anti_tbl_a;
	ctrl_tbl_main_lcd.anti_a.g_anti		= (USHORT*)g_anti_tbl_a;
	ctrl_tbl_main_lcd.anti_a.b_anti		= (USHORT*)b_anti_tbl_a;
	ctrl_tbl_main_lcd.anti_b.r_anti		= (USHORT*)r_anti_tbl_b;
	ctrl_tbl_main_lcd.anti_b.g_anti		= (USHORT*)g_anti_tbl_b;
	ctrl_tbl_main_lcd.anti_b.b_anti		= (USHORT*)b_anti_tbl_b;
	ctrl_tbl_main_lcd.gamma_a.r_data	= (USHORT*)r_gamma_tbl_a_in;
	ctrl_tbl_main_lcd.gamma_a.g_data	= (USHORT*)g_gamma_tbl_a_in;
	ctrl_tbl_main_lcd.gamma_a.b_data	= (USHORT*)b_gamma_tbl_a_in;
	ctrl_tbl_main_lcd.gamma_b.r_data	= (USHORT*)r_gamma_tbl_b_in;
	ctrl_tbl_main_lcd.gamma_b.g_data	= (USHORT*)g_gamma_tbl_b_in;
	ctrl_tbl_main_lcd.gamma_b.b_data	= (USHORT*)b_gamma_tbl_b_in;
	
	ret = Im_DISP_Ctrl_Main_Tbl(E_IM_DISP_LCD_MIPI, &ctrl_tbl_main_lcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Ctrl_Main_Tbl error ret=%08X\n", ret));
	}
	
	// Set main data input block table
	memset(&ctrl_tbl_out_lcd, 0, sizeof(ctrl_tbl_out_lcd));
	ctrl_tbl_out_lcd.gamma_a.r_data	= (USHORT*)r_gamma_tbl_a_out;
	ctrl_tbl_out_lcd.gamma_a.g_data	= (USHORT*)g_gamma_tbl_a_out;
	ctrl_tbl_out_lcd.gamma_a.b_data	= (USHORT*)b_gamma_tbl_a_out;
	ctrl_tbl_out_lcd.gamma_b.r_data	= (USHORT*)r_gamma_tbl_b_out;
	ctrl_tbl_out_lcd.gamma_b.g_data	= (USHORT*)g_gamma_tbl_b_out;
	ctrl_tbl_out_lcd.gamma_b.b_data	= (USHORT*)b_gamma_tbl_b_out;
	ctrl_tbl_out_lcd.luminance_a	= (USHORT*)&luminance_a;
	ctrl_tbl_out_lcd.chroma_a		= (USHORT*)&chroma_a;
	ctrl_tbl_out_lcd.luminance_b	= (USHORT*)&luminance_b;
	ctrl_tbl_out_lcd.chroma_b		= (USHORT*)&chroma_b;
	
	ret = Im_DISP_Ctrl_Output_Tbl(E_IM_DISP_LCD_MIPI, &ctrl_tbl_out_lcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Ctrl_Output_Tbl error ret=%08X\n", ret));
	}
	
	// Set main data input block
	memset(&ctrl_trg_limit_main_lcd, 0, sizeof(ctrl_trg_limit_main_lcd));
	ctrl_trg_limit_main_lcd.lrpgctl			= E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP;
	ctrl_trg_limit_main_lcd.ltblaset_igtaen	= E_IM_DISP_TABLE_ACCESS_ENABLE;
	ctrl_trg_limit_main_lcd.ltblaset_gtaen	= E_IM_DISP_TABLE_ACCESS_ENABLE;
	
	memset(&ctrl_main_lcd, 0, sizeof(ctrl_main_lcd));
	ctrl_main_lcd.lidt.bit.IFMT			= D_IM_DISP_LIDT_IFMT_YCC422_RP_8BIT;
	ctrl_main_lcd.lidt.bit.NBT			= D_IM_DISP_NBT_128BYTE;
	ctrl_main_lcd.lidt.bit.IFEW			= D_IM_DISP_IFEW_16BIT;
	ctrl_main_lcd.lidt.bit.LVFM			= D_IM_DISP_FORMAT_NORMAL;
	ctrl_main_lcd.lidt.bit.PKGDV		= 0;
	ctrl_main_lcd.lidt.bit.CACHE		= 0;
	ctrl_main_lcd.lidt.bit.PROT			= 0;
	ctrl_main_lcd.lidt.bit.YSLVSL		= D_IM_DISP_SLVSL_MASTER_INTERFACE_0;
	ctrl_main_lcd.lidt.bit.CSLVSL		= D_IM_DISP_SLVSL_MASTER_INTERFACE_0;
	ctrl_main_lcd.lisize.size.width		= 640;
	ctrl_main_lcd.lisize.size.lines		= 480;
	ctrl_main_lcd.laddr[0].y_addr		= 0x50000000;
	ctrl_main_lcd.laddr[0].c_addr		= 0x50100000;
	ctrl_main_lcd.laddr[1].y_addr		= 0x50200000;
	ctrl_main_lcd.laddr[1].c_addr		= 0x50300000;
	ctrl_main_lcd.laddr[2].y_addr		= 0x50400000;
	ctrl_main_lcd.laddr[2].c_addr		= 0x50500000;
	ctrl_main_lcd.laddr[3].y_addr		= 0x50600000;
	ctrl_main_lcd.laddr[3].c_addr		= 0x50700000;
	ctrl_main_lcd.y_hga					= 640;
	ctrl_main_lcd.c_hga					= 480;
	ctrl_main_lcd.libctl_iben			= 0;
	ctrl_main_lcd.libctl_ibset			= E_IM_DISP_BANK_00;
	ctrl_main_lcd.lercv					= E_IM_DISP_ERCV_ON;
	ctrl_main_lcd.lrsz0					= E_IM_DISP_RSZSL_PADDING_THINNING;
	ctrl_main_lcd.lrsz1.bit.HRSZM		= 1;
	ctrl_main_lcd.lrsz1.bit.HRSZN		= 1;
	ctrl_main_lcd.lrsz1.bit.HRSZOF		= 0;
	ctrl_main_lcd.lrsz2.bit.VRSZM		= 1;
	ctrl_main_lcd.lrsz2.bit.VRSZN		= 1;
	ctrl_main_lcd.lrsz2.bit.VRSZOF		= 0;
	ctrl_main_lcd.lrsz3.bit.HCSTA		= 0;
	ctrl_main_lcd.lrsz3.bit.VCSTA		= 0;
	ctrl_main_lcd.yhlp.lyhlpcl.bit.YHLPEN		= 0;
	ctrl_main_lcd.yhlp.lyhlpcl.bit.YHLPMD		= E_IM_DISP_YHLPMD_HORIZONTAL_SPACE_FILTER;
	ctrl_main_lcd.yhlp.lyhlpk.bit.YHLPK0		= 0;
	ctrl_main_lcd.yhlp.lyhlpk.bit.YHLPK1		= 0;
	ctrl_main_lcd.yhlp.lyhlpk.bit.YHLPK2		= 0;
	ctrl_main_lcd.yhlp.lyhlpk.bit.YHLPK3		= 0;
	ctrl_main_lcd.yhlp.lyhlpol.bit.YHLPCOR		= 0;
	ctrl_main_lcd.yhlp.lyhlpol.bit.YHLPTH		= 0;
	ctrl_main_lcd.yhlp.lyhlpclr.bit.YHLPY		= 0;
	ctrl_main_lcd.yhlp.lyhlpclr.bit.YHLPCB		= 0;
	ctrl_main_lcd.yhlp.lyhlpclr.bit.YHLPCR		= 0;
	ctrl_main_lcd.warning.lywctl.bit.YWEN		= D_IM_DISP_ENABLE_OFF;
	ctrl_main_lcd.warning.lywctl.bit.YWSL		= E_IM_DISP_YWSL_BRIGHTNESS_SPECIFIED_COLOR;
	ctrl_main_lcd.warning.lyw0th.bit.YWTHH		= 0;
	ctrl_main_lcd.warning.lyw0th.bit.YWTHL		= 0;
	ctrl_main_lcd.warning.lyw1th.bit.YWTHH		= 0;
	ctrl_main_lcd.warning.lyw1th.bit.YWTHL		= 0;
	ctrl_main_lcd.warning.lyw0st				= E_IM_DISP_YWMD_BR_RGB;
	ctrl_main_lcd.warning.lyw0ca.bit.YWY		= 0;
	ctrl_main_lcd.warning.lyw0ca.bit.YWCB		= 0;
	ctrl_main_lcd.warning.lyw0ca.bit.YWCR		= 0;
	ctrl_main_lcd.warning.lyw0ca.bit.YWA		= 0;
	ctrl_main_lcd.warning.lyw0cb.bit.YWY		= 0;
	ctrl_main_lcd.warning.lyw0cb.bit.YWCB		= 0;
	ctrl_main_lcd.warning.lyw0cb.bit.YWCR		= 0;
	ctrl_main_lcd.warning.lyw0cb.bit.YWA		= 0;
	ctrl_main_lcd.warning.lyw1st				= E_IM_DISP_YWMD_BR_RGB;
	ctrl_main_lcd.warning.lyw1ca.bit.YWY		= 0;
	ctrl_main_lcd.warning.lyw1ca.bit.YWCB		= 0;
	ctrl_main_lcd.warning.lyw1ca.bit.YWCR		= 0;
	ctrl_main_lcd.warning.lyw1ca.bit.YWA		= 0;
	ctrl_main_lcd.warning.lyw1cb.bit.YWY		= 0;
	ctrl_main_lcd.warning.lyw1cb.bit.YWCB		= 0;
	ctrl_main_lcd.warning.lyw1cb.bit.YWCR		= 0;
	ctrl_main_lcd.warning.lyw1cb.bit.YWA		= 0;
	ctrl_main_lcd.warning.lbost					= D_IM_DISP_BOMD_MASTER;
	ctrl_main_lcd.lbltmr.bit.BTIMH		= 0;
	ctrl_main_lcd.lbltmr.bit.BTIML		= 0;
	ctrl_main_lcd.lbltmr.bit.BTIMD		= D_IM_DISP_BTIMD_NORMAL;
	ctrl_main_lcd.zebra.lzbwid.bit.ZBHH	= 2;
	ctrl_main_lcd.zebra.lzbwid.bit.ZBHL	= 2;
	ctrl_main_lcd.zebra.lzbv.bit.ZBVSTA	= 0;
	ctrl_main_lcd.zebra.lzbv.bit.ZBVCYC	= 0;
	ctrl_main_lcd.zebra.lzbpt.bit.ZBDIR	= 0;
	ctrl_main_lcd.ly2r[0].dword			= 0x0000001f00eb002a;
	ctrl_main_lcd.ly2r[1].dword			= 0x000000d300f70048;
	ctrl_main_lcd.ly2r[2].dword			= 0x000000fc00720046;
	ctrl_main_lcd.ltblaset_igen			= E_IM_DISP_CORRECT_PROC_ENABLE;
	ctrl_main_lcd.ltblaset_igtsl		= D_IM_DISP_TABLE_SURFACE_A;
	ctrl_main_lcd.ltblaset_gmen			= E_IM_DISP_CORRECT_PROC_ENABLE;
	ctrl_main_lcd.ltblaset_gmtsl		= D_IM_DISP_TABLE_SURFACE_A;
	ctrl_main_lcd.lcc[0].dword			= 0x000000f300f90040;
	ctrl_main_lcd.lcc[1].dword			= 0x0000000700410000;
	ctrl_main_lcd.lcc[2].dword			= 0x0000004100040000;
	ctrl_main_lcd.lgmmd.bit.GMMD		= D_IM_DISP_GMMD_EQUAL_INTERVAL_MODE;
	ctrl_main_lcd.lalp.bit.ALP			= 255;
	ctrl_main_lcd.ldsta.bit.DSH			= 0;
	ctrl_main_lcd.ldsta.bit.DSV			= 0;
	ctrl_main_lcd.lrevdisp.bit.HREV		= D_IM_DISP_REV_NORMAL;
	ctrl_main_lcd.lrevdisp.bit.VREV		= D_IM_DISP_REV_NORMAL;
	
	ret = Im_DISP_Ctrl_Main_Layer(E_IM_DISP_LCD_MIPI, &ctrl_trg_limit_main_lcd, &ctrl_main_lcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Ctrl_Main_Layer error ret=%08X\n", ret));
	}
	
	// Set data output block
	memset(&ctrl_trg_limit_out_lcd, 0, sizeof(ctrl_trg_limit_out_lcd));
	ctrl_trg_limit_out_lcd.tgkmd			= E_IM_DISP_TGKMD_NORMAL;
	ctrl_trg_limit_out_lcd.tgkst			= 0;
	ctrl_trg_limit_out_lcd.toctl.bit.CON	= D_IM_DISP_CON_EXTERNAL;
	ctrl_trg_limit_out_lcd.toctl.bit.CKOEN	= D_IM_DISP_ENABLE_OFF;
	ctrl_trg_limit_out_lcd.toctl.bit.CKICEN	= D_IM_DISP_ENABLE_OFF;
	ctrl_trg_limit_out_lcd.polsel.bit.VDPS	= D_IM_DISP_POLARITY_POSITIVE;
	ctrl_trg_limit_out_lcd.polsel.bit.HDPS	= D_IM_DISP_POLARITY_POSITIVE;
	ctrl_trg_limit_out_lcd.polsel.bit.VEPS	= D_IM_DISP_POLARITY_POSITIVE;
	ctrl_trg_limit_out_lcd.polsel.bit.HEPS	= D_IM_DISP_POLARITY_POSITIVE;
	ctrl_trg_limit_out_lcd.polsel.bit.CKEG	= D_IM_DISP_CKEG_FALL;
	ctrl_trg_limit_out_lcd.rpgctl			= E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP;
	ctrl_trg_limit_out_lcd.tsl				= D_IM_DISP_TSL_PROGRESSIVE;
	ctrl_trg_limit_out_lcd.vcyc.bit.VCYC1	= 482;
	ctrl_trg_limit_out_lcd.vcyc.bit.VCYC2	= 482;
	ctrl_trg_limit_out_lcd.vcyc.bit.VDSFT	= D_IM_DISP_VDSFT_NORMAL;
	ctrl_trg_limit_out_lcd.hcyc				= 668;
	ctrl_trg_limit_out_lcd.ovpw.bit.OVPW	= 0x0010;
	ctrl_trg_limit_out_lcd.ovpw.bit.OVPWU	= D_IM_DISP_OVPWU_CLOCK_UNIT;
	ctrl_trg_limit_out_lcd.hpw				= 0x0010;
	ctrl_trg_limit_out_lcd.vblk.bit.VBLK1	= 1;
	ctrl_trg_limit_out_lcd.vblk.bit.VBLK2	= 1;
	ctrl_trg_limit_out_lcd.hblk				= 16;
	ctrl_trg_limit_out_lcd.vdly				= 0;
	ctrl_trg_limit_out_lcd.hdly				= 0;
	ctrl_trg_limit_out_lcd.ovsize			= 480;
	ctrl_trg_limit_out_lcd.ohsize			= 640;
	ctrl_trg_limit_out_lcd.vrfctl			= D_IM_DISP_VENS_H_ENABLE;
	ctrl_trg_limit_out_lcd.hrfctl			= D_IM_DISP_HENS_ASSERT_ON;
	ctrl_trg_limit_out_lcd.domd.bit.MODE	= D_IM_DISP_MODE_RGB888_24BIT;
	ctrl_trg_limit_out_lcd.domd.bit.OBPSL	= D_IM_DISP_OBPSL_LSB;
	ctrl_trg_limit_out_lcd.domd.bit.OBUSL	= D_IM_DISP_OBUSL_8BIT;
	ctrl_trg_limit_out_lcd.tblaset_gmtaen	= E_IM_DISP_TABLE_ACCESS_ENABLE;
	ctrl_trg_limit_out_lcd.tblaset_ysatta	= E_IM_DISP_TABLE_ACCESS_ENABLE;
	
	memset(&ctrl_out_lcd, 0, sizeof(ctrl_out_lcd));
	ctrl_out_lcd.intc						= D_IM_DISP_VFTMG_1ST;
	ctrl_out_lcd.inte.bit.LEE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GR0EE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GR1EE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GA0EE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GA1EE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.LREE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GR0REE			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GR1REE			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GA0REE			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.GA1REE			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.VE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.inte.bit.VE2				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.hablk						= 0;
	ctrl_out_lcd.fdoen						= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.fodata.rgb.r				= 0;
	ctrl_out_lcd.fodata.rgb.g				= 0;
	ctrl_out_lcd.fodata.rgb.b				= 0;
	ctrl_out_lcd.blankdt.ycc.y				= 0;
	ctrl_out_lcd.blankdt.ycc.cb				= 0;
	ctrl_out_lcd.blankdt.ycc.cr				= 0;
	ctrl_out_lcd.clbhsize					= 40;
	ctrl_out_lcd.clbdt[0].rgb.r				= 0;
	ctrl_out_lcd.clbdt[0].rgb.g				= 0;
	ctrl_out_lcd.clbdt[0].rgb.b				= 0;
	ctrl_out_lcd.clbdt[1].rgb.r				= 0;
	ctrl_out_lcd.clbdt[1].rgb.g				= 0;
	ctrl_out_lcd.clbdt[1].rgb.b				= 0;
	ctrl_out_lcd.clbdt[2].rgb.r				= 0;
	ctrl_out_lcd.clbdt[2].rgb.g				= 0;
	ctrl_out_lcd.clbdt[2].rgb.b				= 0;
	ctrl_out_lcd.bldctl.bit.BLD1			= D_IM_DISP_BLD_GRID;
	ctrl_out_lcd.bldctl.bit.BLD2			= D_IM_DISP_BLD_FACE;
	ctrl_out_lcd.bldctl.bit.BLD3			= D_IM_DISP_BLD_OSD0;
	ctrl_out_lcd.bldctl.bit.BLD4			= D_IM_DISP_BLD_OSD1;
	ctrl_out_lcd.r2y[0].bit.COEFFICIENT0	= 0;
	ctrl_out_lcd.r2y[0].bit.COEFFICIENT1	= 0;
	ctrl_out_lcd.r2y[0].bit.COEFFICIENT2	= 0;
	ctrl_out_lcd.r2y[1].bit.COEFFICIENT0	= 0;
	ctrl_out_lcd.r2y[1].bit.COEFFICIENT1	= 0;
	ctrl_out_lcd.r2y[1].bit.COEFFICIENT2	= 0;
	ctrl_out_lcd.r2y[2].bit.COEFFICIENT0	= 0;
	ctrl_out_lcd.r2y[2].bit.COEFFICIENT1	= 0;
	ctrl_out_lcd.r2y[2].bit.COEFFICIENT2	= 0;
	ctrl_out_lcd.clip_cal.y_cal.ygain		= 0;
	ctrl_out_lcd.clip_cal.y_cal.yofs		= 0;
	ctrl_out_lcd.clip_cal.y_clip.cph		= 0x0000;
	ctrl_out_lcd.clip_cal.y_clip.cpl		= 0x0000;
	ctrl_out_lcd.clip_cal.cb_cal.cgain		= 0;
	ctrl_out_lcd.clip_cal.cb_cal.cofs		= 0;
	ctrl_out_lcd.clip_cal.cb_clip.cph		= 0x0000;
	ctrl_out_lcd.clip_cal.cb_clip.cpl		= 0x0000;
	ctrl_out_lcd.clip_cal.cr_cal.cgain		= 0;
	ctrl_out_lcd.clip_cal.cr_cal.cofs		= 0;
	ctrl_out_lcd.clip_cal.cr_clip.cph		= 0x0000;
	ctrl_out_lcd.clip_cal.cr_clip.cpl		= 0x0000;
	ctrl_out_lcd.doctl0.bit.DTO1			= D_IM_DISP_DTO_R_Y0;
	ctrl_out_lcd.doctl0.bit.DTO2			= D_IM_DISP_DTO_R_Y0;
	ctrl_out_lcd.doctl0.bit.DTO3			= D_IM_DISP_DTO_R_Y0;
	ctrl_out_lcd.doctl0.bit.DTO4			= D_IM_DISP_DTO_G_CB;
	ctrl_out_lcd.doctl0.bit.DTO5			= D_IM_DISP_DTO_G_CB;
	ctrl_out_lcd.doctl0.bit.DTO6			= D_IM_DISP_DTO_G_CB;
	ctrl_out_lcd.doctl1.bit.SOO				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.SOE				= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.ODO0			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.ODO1			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.ODO2			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.ODE0			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.ODE1			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl1.bit.ODE2			= D_IM_DISP_ENABLE_ON;
	ctrl_out_lcd.doctl2.bit.CLVS			= D_IM_DISP_CLVS_NORMAL;
	ctrl_out_lcd.doctl2.bit.CTOH			= D_IM_DISP_CTOH_LEFT;
	ctrl_out_lcd.doctl2.bit.CTOV			= D_IM_DISP_CTOV_TOP;
	ctrl_out_lcd.doctl2.bit.TRSOS			= D_IM_DISP_TRSOS_NONE;
	ctrl_out_lcd.trscode[0].bit.EAV0		= 0xB6;
	ctrl_out_lcd.trscode[0].bit.SAV0		= 0xAB;
	ctrl_out_lcd.trscode[0].bit.EAV1		= 0x9D;
	ctrl_out_lcd.trscode[0].bit.SAV1		= 0x80;
	ctrl_out_lcd.trscode[1].bit.EAV0		= 0xF1;
	ctrl_out_lcd.trscode[1].bit.SAV0		= 0xEC;
	ctrl_out_lcd.trscode[1].bit.EAV1		= 0xDA;
	ctrl_out_lcd.trscode[1].bit.SAV1		= 0xC7;
	ctrl_out_lcd.y2r[0].bit.COEFFICIENT0	= 0;
	ctrl_out_lcd.y2r[0].bit.COEFFICIENT1	= 0;
	ctrl_out_lcd.y2r[0].bit.COEFFICIENT2	= 0;
	ctrl_out_lcd.y2r[1].bit.COEFFICIENT0	= 0;
	ctrl_out_lcd.y2r[1].bit.COEFFICIENT1	= 0;
	ctrl_out_lcd.y2r[1].bit.COEFFICIENT2	= 0;
	ctrl_out_lcd.y2r[2].bit.COEFFICIENT0	= 0;
	ctrl_out_lcd.y2r[2].bit.COEFFICIENT1	= 0;
	ctrl_out_lcd.y2r[2].bit.COEFFICIENT2	= 0;
	ctrl_out_lcd.tblaset_gmen				= E_IM_DISP_CORRECT_PROC_ENABLE;
	ctrl_out_lcd.tblaset_gmtsl				= D_IM_DISP_TABLE_SURFACE_A;
	ctrl_out_lcd.tblaset_ysaten				= E_IM_DISP_CORRECT_PROC_ENABLE;
	ctrl_out_lcd.tblaset_ysatsl				= D_IM_DISP_TABLE_SURFACE_A;
	for(i = 0; i < D_IM_DISP_FACE_FRAME_COUNT; i++) {
		ctrl_out_lcd.face.param[i].ffdsta.bit.DSH		= 0;
		ctrl_out_lcd.face.param[i].ffdsta.bit.DSV		= 0;
		ctrl_out_lcd.face.param[i].ffsize.bit.FFHSZ		= 8;
		ctrl_out_lcd.face.param[i].ffsize.bit.FFVSZ		= 8;
		ctrl_out_lcd.face.param[i].ffwidth.bit.FFHWID	= 2;
		ctrl_out_lcd.face.param[i].ffwidth.bit.FFVWID	= 2;
		ctrl_out_lcd.face.param[i].ffclr.bit.FFCLRR		= 0;
		ctrl_out_lcd.face.param[i].ffclr.bit.FFCLRG		= 0;
		ctrl_out_lcd.face.param[i].ffclr.bit.FFCLRB		= 0;
	}
	ctrl_out_lcd.face.ffden					= 0x0000;
	ctrl_out_lcd.face.msff					= 0;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_VE]		= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_VE2]		= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GR0EE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GR1EE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GA0EE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GA1EE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_LEE]		= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GR0REE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GR1REE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GA0REE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_GA1REE]	= NULL;
	ctrl_out_lcd.int_callback[E_IM_DISP_INT_CB_LREE]	= NULL;

	ret = Im_DISP_Ctrl_Output(E_IM_DISP_LCD_MIPI, &ctrl_trg_limit_out_lcd, &ctrl_out_lcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Ctrl_Output error ret=%08X\n", ret));
	}
	
	// Set OSD data input block
	grrpgctl = E_IM_DISP_RPGTMG_VSYNC_FRAME_TOP;
	
	memset(&ctrl_osd_lcd, 0, sizeof(ctrl_osd_lcd));
	ctrl_osd_lcd.gridt.bit.IFMT				= D_IM_DISP_GRIDT_IFMT_RGBA8888;
	ctrl_osd_lcd.gridt.bit.NBT				= D_IM_DISP_NBT_128BYTE;
	ctrl_osd_lcd.gridt.bit.CACHE			= 0;
	ctrl_osd_lcd.gridt.bit.PROT				= 0;
	ctrl_osd_lcd.gridt.bit.SLVSL			= D_IM_DISP_SLVSL_MASTER_INTERFACE_0;
	ctrl_osd_lcd.gridt.bit.ASLVSL			= D_IM_DISP_SLVSL_MASTER_INTERFACE_0;
	ctrl_osd_lcd.gridt.bit.IFBTMU			= D_IM_DISP_IFBTMU_UP_TO_BOTTOM;
	ctrl_osd_lcd.grtisize.size.width		= 640;
	ctrl_osd_lcd.grtisize.size.lines		= 480;
	ctrl_osd_lcd.grtdsta.bit.DSH			= 0;
	ctrl_osd_lcd.grtdsta.bit.DSV			= 0;
	ctrl_osd_lcd.gripo.bit.IPO1				= D_IM_DISP_IPO_RGBA8888_R;
	ctrl_osd_lcd.gripo.bit.IPO2				= D_IM_DISP_IPO_RGBA8888_G;
	ctrl_osd_lcd.gripo.bit.IPO3				= D_IM_DISP_IPO_RGBA8888_B;
	ctrl_osd_lcd.gripo.bit.IPO4				= D_IM_DISP_IPO_RGBA8888_A;
	ctrl_osd_lcd.grscctl_idset				= E_IM_DISP_INPUT_DATA_AREA_0;
	for(i = 0; i < D_IM_DISP_OSD_DISPLAY_AREA_COUNT; i++) {
		ctrl_osd_lcd.grisize[i].size.width	= 8;
		ctrl_osd_lcd.grisize[i].size.lines	= 8;
		ctrl_osd_lcd.grhga[i]				= 8;
		ctrl_osd_lcd.grahga[i]				= 8;
		ctrl_osd_lcd.grdsta[i].bit.DSH		= 0;
		ctrl_osd_lcd.grdsta[i].bit.DSV		= 0;
	}
	ctrl_osd_lcd.grsa0[0]					= 0x51001000;
	ctrl_osd_lcd.grsa0[1]					= 0x51002000;
	ctrl_osd_lcd.grsa0[2]					= 0x51003000;
	ctrl_osd_lcd.grsa0[3]					= 0x51004000;
	ctrl_osd_lcd.grsa[0]					= 0x51005000;
	ctrl_osd_lcd.grsa[8]					= 0x5100D000;
	ctrl_osd_lcd.grasa[0]					= 0x51006000;
	ctrl_osd_lcd.grasa[9]					= 0x5100E000;
	ctrl_osd_lcd.graren						= 0;
	ctrl_osd_lcd.grbsl						= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_0		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_0		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_1		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_1		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_2		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_2		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_3		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_3		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_4		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_4		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_5		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_5		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_6		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_6		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_7		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_7		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_8		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_8		= 0;
	ctrl_osd_lcd.grblink.bit.BTIMH_9		= 0;
	ctrl_osd_lcd.grblink.bit.BTIML_9		= 0;
	ctrl_osd_lcd.grbinit					= 0;
	ctrl_osd_lcd.grbitrg					= D_IM_DISP_BITRG_NON;
	ctrl_osd_lcd.grrsz0						= E_IM_DISP_RSZSL_PADDING_THINNING;
	ctrl_osd_lcd.grrsz1.bit.HRSZM			= 1;
	ctrl_osd_lcd.grrsz1.bit.HRSZN			= 1;
	ctrl_osd_lcd.grrsz1.bit.HRSZOF			= 0;
	ctrl_osd_lcd.grrsz2.bit.VRSZM			= 1;
	ctrl_osd_lcd.grrsz2.bit.VRSZN			= 1;
	ctrl_osd_lcd.grrsz2.bit.VRSZOF			= 0;
	ctrl_osd_lcd.grrsz3.bit.HCSTA			= 0;
	ctrl_osd_lcd.grrsz3.bit.VCSTA			= 0;
	ctrl_osd_lcd.gry2r[0].bit.YR0			= 0;
	ctrl_osd_lcd.gry2r[0].bit.YR1			= 0;
	ctrl_osd_lcd.gry2r[0].bit.YR2			= 0;
	ctrl_osd_lcd.gry2r[1].bit.YR0			= 0;
	ctrl_osd_lcd.gry2r[1].bit.YR1			= 0;
	ctrl_osd_lcd.gry2r[1].bit.YR2			= 0;
	ctrl_osd_lcd.gry2r[2].bit.YR0			= 0;
	ctrl_osd_lcd.gry2r[2].bit.YR1			= 0;
	ctrl_osd_lcd.gry2r[2].bit.YR2			= 0;
	ctrl_osd_lcd.gralp						= 0;
	
	ret = Im_DISP_Ctrl_OSD_Layer(E_IM_DISP_LCD_MIPI, E_IM_DISP_SEL_LAYER_OSD_0, &grrpgctl, &ctrl_osd_lcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Ctrl_OSD_Layer error ret=%08X\n", ret));
	}
	
	ret = Im_DISP_Ctrl_OSD_Layer(E_IM_DISP_LCD_MIPI, E_IM_DISP_SEL_LAYER_OSD_1, &grrpgctl, &ctrl_osd_lcd);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Ctrl_OSD_Layer error ret=%08X\n", ret));
	}
	
	// Start.
	ret = Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_ALL, 0);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Start error ret=%08X\n", ret));
	}
	
	// Wait VSYNC
	ret = Im_DISP_Wait_Vsync(E_IM_DISP_LCD_MIPI, E_IM_DISP_INTERRUPTION_SELECT_VE, E_IM_DISP_WAIT_1_TIME);
	if (ret != D_DDIM_OK){
		Ddim_Print(("I:Im_DISP_Wait_Vsync error ret=%08X\n", ret));
	}
}
@endcode 
@}*//* end of im_disp*/
