/**
 * @file		im_jpeg.h
 * @brief		Jpeg macro driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

/**
@weakgroup im_jpeg
@{

<hr>
@}
*/
/**
@addtogroup im_jpeg
@{
	- @ref JPEG_DECODE
	- @ref JPEG_ENCODE
	- @ref JPEG_DECODE_UTIL
*/

#ifndef _IM_JPEG_H_
#define _IM_JPEG_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Return value of processing result
*/
#define D_IM_JPEG_OK					(D_DDIM_OK)								/**< Normal end */
#define	D_IM_JPEG_PARAM_ERROR			(D_IM_JPEG | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error */
#define	D_IM_JPEG_SEM_NG				(D_IM_JPEG | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */
#define	D_IM_JPEG_SEM_TIMEOUT			(D_IM_JPEG | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition timeout */
#define D_IM_JPEG_TIMEOUT				(D_IM_JPEG | D_DDIM_TIMEOUT)			/**< Processing timeout */
#define D_IM_JPEG_SYSTEMCALL_ERR		(D_IM_JPEG | D_DDIM_SYSTEM_ERROR)		/**< System call error */
#define D_IM_JPEG_RUNNING_NG			(D_IM_JPEG | D_DDIM_MACRO_BUSY_NG)		/**< Jpeg macro running */
#define D_IM_JPEG_ENCODE_ERR			(D_IM_JPEG | 0x00002001)				/**< Jpeg encode NG */
#define D_IM_JPEG_ENCODE_PAUSE			(D_IM_JPEG | 0x00002002)				/**< Jpeg encode pause (or limit size over) */
#define D_IM_JPEG_DECODE_ERR			(D_IM_JPEG | 0x00004001)				/**< Jpeg decode NG */
#define D_IM_JPEG_DECODE_PAUSE			(D_IM_JPEG | 0x00004002)				/**< Jpeg decode pause (or limit size over) */
#define D_IM_JPEG_AXI_ERR				(D_IM_JPEG | 0x00008001)				/**< AXI error */

/* Process result
*/
#define D_IM_JPEG_NML_END				(1<<0)									/**< jpeg core end */
#define D_IM_JPEG_PAUSE					(1<<1)									/**< Pause end */
#define D_IM_JPEG_CORRECT_END			(1<<2)									/**< The correction process end */
#define D_IM_JPEG_CORSIZINFO			(1<<3)									/**< As a result of the correction process, Target data is larger than total size */
#define D_IM_JPEG_CORDRIINFO			(1<<4)									/**< As a result of the correction process, There is the data larger than the spacing DRI */
#define D_IM_JPEG_JCLRFMER				(1<<5)									/**< Unsupported format erorr */
#define D_IM_JPEG_JJBINT				(1<<6)									/**< AXI error(JBUF)  */
#define D_IM_JPEG_JPBINT				(1<<7)									/**< AXI error(PBUF)  */
#define D_IM_JPEG_PLINEINT				(1<<8)									/**< AXI error(JBUF)  */
#define D_IM_JPEG_PSECTINT				(1<<9)									/**< AXI error(PBUF)  */
#define D_IM_JPEG_ERR					(1<<10)									/**< Encode data error */
#define D_IM_JPEG_SIZERR				(1<<11)									/**< The total number of data anomalies */
#define D_IM_JPEG_RSTERR				(1<<12)									/**< RST marker interval abnormalities */
#define D_IM_JPEG_SMPL_TYPE_NG			(1<<13)									/**< Target sampling type not matching */
#define D_IM_JPEG_CUTOUT_ERR			(1<<14)									/**< Format can not cutout */
#define D_IM_JPEG_REGRDINT				(1<<15)									/**< Size read enable */
#define D_IM_JPEG_UMKSINFO				(1<<16)									/**< Unknown marker detection */
#define D_IM_JPEG_COMMKINFO				(1<<17)									/**< Com marker detection */
#define D_IM_JPEG_APPMKINFO				(1<<18)									/**< App marker detection */
#define D_IM_JPEG_JIF					(1<<19)									/**< JIF interrupt */
#define D_IM_JPEG_JCF					(1<<20)									/**< JCF interrupt */
#define D_IM_JPEG_QTWINT				(1<<21)									/**< Update quantization table */

/** Process normal end result pattern
*/
#define D_IM_JPEG_PROC_END			(D_IM_JPEG_NML_END | D_IM_JPEG_JIF | D_IM_JPEG_JCF)

/* Interrupt flag
*/
#define D_IM_JPEG_FLG_WAIT_END			(0x00001000)							/**< Interrupt flag of process end */

/* Function Enable
*/
#define	D_IM_JPEG_ENABLE_OFF			(0)										/**< Disable */
#define	D_IM_JPEG_ENABLE_ON				(1)										/**< Enable */

/* Jpeg down sampling type
*/
#define	D_IM_JPEG_DOWNSP_NONE			(0)										/**< Not used the down sampling. */
#define	D_IM_JPEG_DOWNSP_1_2			(1)										/**< Horizontal and vertical 1/2 down sampling (1 block skipped transfer) */
#define	D_IM_JPEG_DOWNSP_1_4			(2)										/**< Horizontal and vertical 1/4 down sampling (3 block skipped transfer) */
#define	D_IM_JPEG_DOWNSP_1_8			(3)										/**< Horizontal and vertical 1/8 down sampling (7 blocks skipped transfer) */

/* AXI Error state
*/
#define	D_IM_JPEG_AXI_OKAY			(0)											/**< AXI success */
#define	D_IM_JPEG_AXI_SLVERR		(2)											/**< AXI slave error  */
#define	D_IM_JPEG_AXI_DECERR		(3)											/**< AXI decode error */

/* AXI chache type
*/
#define D_IM_JPEG_NON_CACHE_NON_BUF				(0)								/**< Cache Support: Non Cacheable & Non Bufferable */
#define D_IM_JPEG_ON_BUF						(1)								/**< Cache Support: Bufferable */
#define D_IM_JPEG_ON_CACHE_NO_ALLOCATE			(2)								/**< Cache Support: Cacheable & No Allocate */
#define D_IM_JPEG_ON_CACHE_ON_BUF_NO_ALLOCATE	(3)								/**< Cache Support: Cacheable & Bufferable & No Allocate */
#define D_IM_JPEG_ON_CACHE_R_WRITE_THROUGH		(6)								/**< Cache Support: Cacheable & Write Through(Allocate Read only) */
#define D_IM_JPEG_ON_CACHE_R_WRITE_BACK			(7)								/**< Cache Support: Cacheable & Write Back(Allocate Read only) */
#define D_IM_JPEG_ON_CACHE_W_WRITE_THROUGH		(10)							/**< Cache Support: Cacheable & Write Through(Allocate Write only) */
#define D_IM_JPEG_ON_CACHE_W_WRITE_BACK			(11)							/**< Cache Support: Cacheable & Write Back(Allocate Write only) */
#define D_IM_JPEG_ON_CACHE_RW_WRITE_THROUGH		(14)							/**< Cache Support: Cacheable & Write Through(Allocate Read & Write) */
#define D_IM_JPEG_ON_CACHE_RW_WRITE_BACK		(15)							/**< Cache Support: Cacheable & Write Back(Allocate Read & Write) */

/* AXI chache protect type
*/
#define D_IM_JPEG_D_SEC_NORMAL_ACCESS			(0)								/**< Cache Protect: Data       /Secure    /Normal     Access */
#define D_IM_JPEG_D_SEC_PRIVILEGED_ACCESS		(1)								/**< Cache Protect: Data       /Secure    /Privileged Access */
#define D_IM_JPEG_D_NSEC_NORMAL_ACCESS			(2)								/**< Cache Protect: Data       /Non-Secure/Normal     Access */
#define D_IM_JPEG_D_NSEC_PRIVILEGED_ACCESS		(3)								/**< Cache Protect: Data       /Non-Secure/Privileged Access */
#define D_IM_JPEG_I_SEC_NORMAL_ACCESS			(4)								/**< Cache Protect: Instruction/Secure    /Normal     Access */
#define D_IM_JPEG_I_SEC_PRIVILEGED_ACCESS		(5)								/**< Cache Protect: Instruction/Secure    /Privileged Access */
#define D_IM_JPEG_I_NSEC_NORMAL_ACCESS			(6)								/**< Cache Protect: Instruction/Non-Secure/Normal     Access */
#define D_IM_JPEG_I_NSEC_PRIVILEGED_ACCESS		(7)								/**< Cache Protect: Instruction/Non-Secure/Privileged Access */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Jpeg sampling ratio type
*/
typedef enum {
	E_IM_JPEG_SMPL_TYPE_YCC444			= 0,									/**< YCbCr 4:4:4 */
	E_IM_JPEG_SMPL_TYPE_YCC422			= 1,									/**< YCbCr 4:2:2 */
	E_IM_JPEG_SMPL_TYPE_YCC420			= 2,									/**< YCbCr 4:2:0 */
	E_IM_JPEG_SMPL_TYPE_YCC400			= 3										/**< YCbCr 4:0:0 */
} E_IM_JPEG_SMPL_TYPE;

/** Jpeg memory format type
*/
typedef enum {
	E_IM_JPEG_MEM_FORM_PLANE			= 0,									/**< Planar format */
	E_IM_JPEG_MEM_FORM_PLANE_DOT		= 1										/**< Planar format of Y and Dot sequential format of CbCr */
} E_IM_JPEG_MEM_FORM;

/** Jpeg AXI(PBUF/JBUF) status
*/
typedef enum {
	E_IM_JPEG_AXI_ST_INACTIVE_BOTH		= 0,									/**< PBUF and JBUF is not running */
	E_IM_JPEG_AXI_ST_ACTIVE_BOTH		= 1,									/**< PBUF and JBUF is running */
	E_IM_JPEG_AXI_ST_ACTIVE_PBUF		= 2,									/**< PBUF is running */
	E_IM_JPEG_AXI_ST_ACTIVE_JBUF		= 3										/**< JBUF is running */
} E_IM_JPEG_AXI_ST;

/** Setting the Jpeg dummy area in decoding.
*/
typedef enum {
	E_IM_JPEG_RESIZE_EXT_DIRECT			= 0,									/**< Directly outputs the pixel containing the dummy area.  */
	E_IM_JPEG_RESIZE_EXT_BAND			= 1										/**< Outputs the color band to pixel containing the dummy area. */
} E_IM_JPEG_RESIZE_EXT;

/** Number of transactions that can be issued by a continuous
*/
typedef enum {
	E_IM_JPEG_ISSUE_TRAN_8				= 0,									/**< Issued eight times a transaction */
	E_IM_JPEG_ISSUE_TRAN_4				= 1,									/**< Issued four times a transaction */
	E_IM_JPEG_ISSUE_TRAN_2				= 2										/**< Issued twice a transaction */
} E_IM_JPEG_ISSUE_TRAN;

/** AXI(JBUF/PBUF) endian setting
*/
typedef enum {
	E_IM_JPEG_ENDIAN_LITTLE				= 0,									/**< AXI(JBUF/PBUF) little endian */
	E_IM_JPEG_ENDIAN_BIG				= 1										/**< AXI(JBUF/PBUF) big endian */
} E_IM_JPEG_ENDIAN;

/** JBUF/PBUF(AXI) burst rength
*/
typedef enum {
	E_IM_JPEG_BURST_INCR_8				= 0,									/**< 64 byte transfer */
	E_IM_JPEG_BURST_INCR_16				= 1										/**< 128 byte transfer */
} E_IM_JPEG_BURST_INCR;

/** Burst alignment control on/off setting of the code data.
*/
typedef enum {
	E_IM_JPEG_BURST_AL_ON				= 0,									/**< Burst alignment control ON */
	E_IM_JPEG_BURST_AL_OFF				= 1										/**< Burst alignment control OFF */
} E_IM_JPEG_BURST_AL;

/** Bit depth.
*/
typedef enum {
	E_IM_JPEG_BIT_DEPTH_8BIT			= 0,									/**< 8 bit */
	E_IM_JPEG_BIT_DEPTH_12BIT			= 1										/**< 12 bit */
} E_IM_JPEG_BIT_DEPTH;

/** IIP cooperation mode.
*/
typedef enum {
	E_IM_JPEG_IIP_PORT_MODE_OFF			= 0,									/**< IIP cooperation mode OFF */
	E_IM_JPEG_IIP_PORT_MODE_1			= 4,									/**< IIP 1 port (P0) */
	E_IM_JPEG_IIP_PORT_MODE_2			= 5,									/**< IIP 2 port (P0~P1) */
	E_IM_JPEG_IIP_PORT_MODE_3			= 6,									/**< IIP 3 port (P0~P2) */
	E_IM_JPEG_IIP_PORT_MODE_4			= 7										/**< IIP 4 port (P0~P3) */
} E_IM_JPEG_IIP_PORT_MODE;

/** IIP cooperation mode.
*/
typedef enum {
	E_IM_JPEG_IIP_BLOCK_64x8			= 0,									/**< 1 block is 64 * 8 (format:420 8->16) */
	E_IM_JPEG_IIP_BLOCK_128x8			= 1,									/**< 1 block is 128 * 8 (format:420 8->16) */
	E_IM_JPEG_IIP_BLOCK_256x8			= 2,									/**< 1 block is 256 * 8 (format:420 8->16) */
	E_IM_JPEG_IIP_BLOCK_HSIZEx8			= 3										/**< 1 block is Horizontal size * 8 (format:420 8->16) */
} E_IM_JPEG_IIP_BLOCK;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** YCC image address */
typedef struct {
	ULONG						y;						/**< Y address */
	ULONG						c;						/**< C address */
} T_IM_JPEG_ADDR_YCC;

/** Jpeg Component table
*/
typedef struct {
	UCHAR						quant_tbl_no;			/**< Quantization table number.<br>
															:value range[0 - 3] */
	UCHAR						huf_dc_tbl_no;			/**< DC huffman table number.<br>
															:value range[0 - 1] */
	UCHAR						huf_ac_tbl_no;			/**< AC huffman table number.<br>
															:value range[0 - 1] */
} T_IM_JPEG_COMPONENT_TBL;

/** Jpeg Quatization table
*/
typedef struct {
	UCHAR						quant_value[64];		/**< Quantization Value */
} T_IM_JPEG_QUAT_TBL;

/** Jpeg Quatization table pack
*/
typedef struct {
	T_IM_JPEG_QUAT_TBL*			quantization_tbl_0;		/**< Quantization table 0 */
	T_IM_JPEG_QUAT_TBL*			quantization_tbl_1;		/**< Quantization table 1 */
	T_IM_JPEG_QUAT_TBL*			quantization_tbl_2;		/**< Quantization table 2 */
	T_IM_JPEG_QUAT_TBL*			quantization_tbl_3;		/**< Quantization table 3 */
} T_IM_JPEG_QUAT_TBL_PACK;

/** Jpeg Color band
*/
typedef struct {
	UCHAR						y_band;					/**< Color band of the Y component */
	UCHAR						cb_band;				/**< Color band of the Cb component */
	UCHAR						cr_band;				/**< Color band of the Cr component */
} T_IM_JPEG_COLOR_BAND;

/** JBUF/PBUF(AXI) Contoroll table
*/
typedef struct {
	E_IM_JPEG_ENDIAN			endian;					/**< Endian setting. */
	E_IM_JPEG_ISSUE_TRAN		issue_tran_num;			/**< Continuous issuing number of the transaction. */
	UCHAR						cache_type;				/**< Cache type.<br>
															<ul>
																<li>@ref D_IM_JPEG_NON_CACHE_NON_BUF
																<li>@ref D_IM_JPEG_ON_BUF
																<li>@ref D_IM_JPEG_ON_CACHE_NO_ALLOCATE
																<li>@ref D_IM_JPEG_ON_CACHE_ON_BUF_NO_ALLOCATE
																<li>@ref D_IM_JPEG_ON_CACHE_R_WRITE_THROUGH
																<li>@ref D_IM_JPEG_ON_CACHE_R_WRITE_BACK
																<li>@ref D_IM_JPEG_ON_CACHE_W_WRITE_THROUGH
																<li>@ref D_IM_JPEG_ON_CACHE_W_WRITE_BACK
																<li>@ref D_IM_JPEG_ON_CACHE_RW_WRITE_THROUGH
																<li>@ref D_IM_JPEG_ON_CACHE_RW_WRITE_BACK
															</ul> */
	UCHAR						prot_type;				/**< Protection unit type.<br>
															<ul>
																<li>@ref D_IM_JPEG_D_SEC_NORMAL_ACCESS
																<li>@ref D_IM_JPEG_D_SEC_PRIVILEGED_ACCESS
																<li>@ref D_IM_JPEG_D_NSEC_PRIVILEGED_ACCESS
																<li>@ref D_IM_JPEG_I_SEC_NORMAL_ACCESS
																<li>@ref D_IM_JPEG_I_SEC_PRIVILEGED_ACCESS
																<li>@ref D_IM_JPEG_I_NSEC_NORMAL_ACCESS
																<li>@ref D_IM_JPEG_I_NSEC_PRIVILEGED_ACCESS
															</ul> */
	UCHAR						err_state;				/**< AXI error status. [Members for the output]<br>
															<ul>
																<li>@ref D_IM_JPEG_AXI_OKAY
																<li>@ref D_IM_JPEG_AXI_SLVERR
																<li>@ref D_IM_JPEG_AXI_DECERR
															</ul> */
} T_IM_JPEG_AXI_CTRL;

/** Jpeg encode base management structure
*/
typedef struct {
	E_IM_JPEG_SMPL_TYPE			ycc_smpl;				/**< Sampling type. */
	E_IM_JPEG_MEM_FORM			mem_format;				/**< Memory format type. */
	USHORT						width;					/**< Horizontal size.<br>
															 :value range[64 - 65520] */
	USHORT						lines;					/**< Vertical size.<br>
															 :value range[64 - 65280] */
	T_IM_JPEG_COMPONENT_TBL		component[3];			/**< Use number of the component table. */
	UCHAR						skip_mk_flg;			/**< Flag to use the marker skipping mode.
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul>*/
	USHORT						dri_mk_num;				/**< MCU number to insert the DRI marker. */
	UCHAR						exif_fmt_flg;			/**< Flag to use the Exif format output.
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	USHORT						pint_line;				/**< Periodic line interrupt function.<br>
															 Generate an interrupt after setting lines processed. */
	ULONG						pint_sect;				/**< Periodic sector interrupt function.<br>
															 Generate an interrupt after setting sector processed. */
	T_IM_JPEG_AXI_CTRL			pbuf_ctrl;				/**< PBUF(AXI) controll. */
	T_IM_JPEG_AXI_CTRL			jbuf_ctrl;				/**< JBUF(AXI) controll. */
	E_IM_JPEG_BURST_INCR		pburst_length;			/**< PBUF burst length. */
	E_IM_JPEG_BURST_INCR		jburst_length;			/**< JBUF burst length. */
	E_IM_JPEG_BIT_DEPTH			bit_depth;				/**< Bit depth. */
	E_IM_JPEG_BURST_AL			burst_alignment;		/**< Burst alignment control on/off setting of the code data. */
	E_IM_JPEG_IIP_PORT_MODE		port_mode;				/**< IIP cooperation mode. */
	UCHAR						ring_on;				/**< Ring enable/disable selection.
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	E_IM_JPEG_IIP_BLOCK			block_size;				/**< Block size selection. */
	UCHAR						ring_size;				/**< Capacity of the ring buffer.
															 value is "block count - 1" , and range[1 - 255] */
	ULONG						code_size;				/**< Size of Jpeg compressed data. [Members for the output] */
	INT32						result;					/**< Processing result. [Members for the output] */
	VP_CALLBACK					pcallback;				/**< Callback function pointer.<br>
															 Sample of callback function.<br>
															 @code
															 VOID JPEG_User_Encode_Callback( T_IM_JPEG_ENC_MNG* jpeg_mng )
															 {
																// code
															 }
															 @endcode
														   */
} T_IM_JPEG_ENC_MNG;

/** Jpeg encode frame management structure
*/
typedef struct {
	ULONG						global_y_width;			/**< Global Y width.<br>
															 Should be aligned on 8.<br>
															 :value range[64 - 1048512]  */
	ULONG						global_c_width;			/**< Global C width.<br>
															 Should be aligned on 8.<br>
															 :value range[64 - 1048512]  */
	T_IM_JPEG_ADDR_YCC			ycc_addr;				/**< Header address of YCC.<br>
															 Should be aligned on 8.<br> */
	ULONG						code_addr;				/**< Header address of compressed data.<br>
															 Should be aligned on 8.<br> */
	UCHAR						code_count_flg;			/**< Flag to use the function of code count(pass-1 mode).
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	UCHAR						code_over_count_flg;	/**< Flag to use the function of code over count.
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	ULLONG						limit_size;				/**< Encode limit size [byte] <br>
															 :value range[0 - 0x1FFFFFFFC00] <br>
															 Converted to units 512byte(Truncation) by inside.<br>
															 Pause the coding in the size you set.<br>
															 Please set when there is an upper limit and set 0 otherwise. */
} T_IM_JPEG_ENC_FRAME_MNG;

/** Jpeg decode base management structure
*/
typedef struct {
	E_IM_JPEG_MEM_FORM			mem_format;				/**< Memory format type. */
	E_IM_JPEG_RESIZE_EXT		ext_mode;				/**< Method of treatment of the dummy region. */
	UCHAR						corr_mode;				/**< Correction mode setting.<br>
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	UCHAR						skip_mk_flg;			/**< Flag to use the marker skipping mode.<br>
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
															</ul>*/
	USHORT						pint_line;				/**< Periodic line interrupt function.<br>
															 Generate an interrupt after setting lines processed */
	ULONG						pint_sect;				/**< Periodic sector interrupt function.<br>
															 Generate an interrupt after setting sector processed */
	T_IM_JPEG_COLOR_BAND		color_band;				/**< Color band setting. */
	T_IM_JPEG_AXI_CTRL			pbuf_ctrl;				/**< PBUF(AXI) controll. */
	T_IM_JPEG_AXI_CTRL			jbuf_ctrl;				/**< JBUF(AXI) controll. */
	E_IM_JPEG_BURST_INCR		pburst_length;			/**< PBUF burst length. */
	E_IM_JPEG_BURST_INCR		jburst_length;			/**< JBUF burst length. */
	E_IM_JPEG_BURST_AL			burst_alignment;		/**< Burst alignment control on/off setting of the code data. */
	E_IM_JPEG_IIP_PORT_MODE		port_mode;				/**< IIP cooperation mode. */
	UCHAR						ring_on;				/**< Ring enable/disable selection.
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	E_IM_JPEG_IIP_BLOCK			block_size;				/**< Block size selection. */
	UCHAR						ring_size;				/**< Capacity of the ring buffer.
															 value is "block count - 1" , and range[1 - 255] */
	E_IM_JPEG_SMPL_TYPE			smpl_type;				/**< Sampling type.<br>
															 If different from the sampling type of the target file is the error */
	USHORT						org_width;				/**< Horizontal of the original image data. [Members for the output] */
	USHORT						org_lines;				/**< Vertical of the original image data. [Members for the output] */
	INT32						result;					/**< Processing result. [Members for the output] */
	UCHAR						err_code;				/**< Decompression error code. [Members for the output]<br>
															<ul>
																<li>0x00 :(Normal)	Decode OK.
																<li>0x01 :(Marker)	EOI is not detected.
																<li>0x02 :(Marker)	SOI is not detected.
																<li>0x03 :(Marker)	SOS is not detected.
																<li>0x04 :(Marker)	When SOS is detected, SOF0, DQT, and DHT is undetected.
																<li>0x05 :(Marker)	Detection of SOF1 ~ SOFn.
																<li>0x06 :(Marker)	Double detection of SOI.
																<li>0x07 :(Marker)	After detection SOI. RSTm or DNL detect before detection SOS.
																<li>0x08 :(Marker)	Detecting a non-marker EOI or DNL or, RST after detecting the SOI or SOS.
																<li>0x09 :(SOF0)	Incorrect marker segment length.
																<li>0x0A :(SOF0)	Accuracy is abnormal. It detects it excluding "8".
																<li>0x0B :(SOF0)	X size is "0".
																<li>0x0C :(SOF0)	The number of components of SOF0 headers detects it excluding "1","3","4".
																<li>0x0D :(SOF0)	Thinning out off the subject is detected.
																<li>0x0E :(SOF0)	Quantization table selector is outside the range of 0-3.
																<li>0x0F :(SOS)		Incorrect marker segment length.
																<li>0x10 :(SOS)		SOS number of components is different to the SOF0 number of components.
																<li>0x11 :(SOS)		Tdi or Tai of the entropy coding table selector is 0-1 is outside the range.
																<li>0x12 :(SOS)		Is not "0" the start of spectral selection.
																<li>0x13 :(SOS)		Is not "63" the end of spectral selection.
																<li>0x14 :(SOS)		Is not "0" the value of successive approximation.
																<li>0x15 :(DQT)		Incorrect marker segment length.
																<li>0x16 :(DQT)		The DQT accuracy is abnormal. It detects it excluding "0".
																<li>0x17 :(DQT)		Tq of quantization table identifier is 0-1 is outside the range.
																<li>0x18 :(DQT)		Quantization table element is "0"(Qk=0).
																<li>0x19 :(DQT)		Detection at SOS, DQT is not in the table of all of the table selector Tq1 of SOF.
																<li>0x1A :(DHT)		Table class or "Tc", "Th" Huffman table identifier outside the range of 0-1.
																<li>0x1B :(DHT)		"Li"(Number Huffman code of the length "i") is outside the range of numerical values represented by "i" (the each bit length)
																<li>0x1C :(DHT)		If the DC, group number "Vij" (No.G) is outside the range of 00 ~ 0B.
																<li>0x1D :(DHT)		If the AC, outside the scope of the No.G / run length.["Vij" 3:0=00(EOB) or "Vij"=F0(ZRL)].
																<li>0x1E :(DHT)		No table number in the DHT (Tc, Th) in all entropy coding selector(Tdi, Tai) in SOS.
																<li>0x1F :(DRI)		Incorrect marker segment length.
																<li>0x20 :(DNL)		Incorrect marker segment length.
																<li>0x21 :(EXP)		Incorrect marker segment length.
																<li>0x22 :(Marker)	Segment length of the marker with a marker segment is less than "2".
																<li>0x23 :(RSTm)	RSTm has detected when DRI is undefined.
																<li>0x24 :(RSTm)	"m" in the RSTm do not occur in the serial number of modulo.
																<li>0x25 :(Code)	Detection of unauthorized fill bits.
																<li>0x26 :(Code)	If you exceed the "64" number of pixels block.
																<li>0x27 :(Code)	If the code does not match the Huffman code has been detected.
																<li>0x28 :(Code)	If a portion of the additional bits normally can not get in front of the marker.
																<li>0x29 :(Code)	If the data has been detected since long before the EOI marker marker + DNL segment.
																<li>0x3F :			In the register of private, interrupt detection.
															</ul> */

	VP_CALLBACK					pcallback;				/**< Callback function pointer.<br>
															 Sample of callback function.<br>
															 @code
															 VOID JPEG_User_Decode_Callback( T_IM_JPEG_DEC_MNG* jpeg_mng )
															 {
																// code
															 }
															 @endcode
														   */
} T_IM_JPEG_DEC_MNG;

/** Jpeg decode frame management structure
*/
typedef struct {
	ULONG						global_y_width;			/**< Global Y width.<br>
															 Should be aligned on 8.<br>
															 :value range[64 - 1048512]  */
	ULONG						global_c_width;			/**< Global C width.<br>
															 Should be aligned on 8.<br>
															 :value range[64 - 1048512]  */
	UCHAR						cutout_flg;				/**< Flag to use the cutout function of image.
															<ul>
																<li>@ref D_IM_JPEG_ENABLE_OFF
																<li>@ref D_IM_JPEG_ENABLE_ON
															</ul> */
	USHORT						cutout_width;			/**< Horizontal of cutout.<br>
															 Should be aligned on 16.<br>
															 :value range[64 - 65280]<br>
															 But, Please set to 0 if you do not use the cutout function. */
	USHORT						cutout_lines;			/**< Vertical of cutout.<br>
															 Should be aligned on 16.<br>
															 :value range[64 - 65520]<br>
																But, Please set to 0 if you do not use the cutout function. */
	USHORT						cutout_offset_h;		/**< Horizontal offset of cutout.<br>
															 Should be aligned on 16.<br>
															 :value range[0 - 65216] */
	USHORT						cutout_offset_v;		/**< Vertical offset of cutout.<br>
															 Should be aligned on 16.<br>
															 :value range[0 - 65456] */
	T_IM_JPEG_ADDR_YCC			ycc_addr;				/**< Header address of YCC.<br>
															 Should be aligned on 8.<br> */
	ULONG						code_addr;				/**< Header address of compressed data.<br>
															 Should be aligned on 8.<br> */
	ULLONG						code_size;				/**< Pause size of compressed data [byte] <br>
															 :value range[0 - 0x1FFFFFFFC00] <br>
															 Converted to units 512byte(abbreviate) by inside. <br>
															 Pause the coding in the size you set.<br>
															 Please set 0 if you do not use the pause function. */
} T_IM_JPEG_DEC_FRAME_MNG;

/*  Explanation of parameters

 [Sample image of Jpeg decode]

         [ Jpeg ]                   [ YUV ]

       |           |           |      *1       |
       |           |           |<------------->|
       |           |           |               |
       |           |           |               |
 ------@a----------+           +---------------+--------
       |***********|           | @b----------+ |----
       |***********|  Decode   | |***********| |
       |***********| --------> | |***********| |
       |***********|           | |**** Y ****| |
       |***********|           | |***********| |
       |***********|           | |***********| |
 ------+-----------+           | |***********| |
                               | +-----------+ |----
                               +---------------+--------

       Fig.1 Sample of normal decode setting
       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


         [ Jpeg ]                     [ YUV ]

       |               |
       |               |           |     *1    |
       | *4|  *2 |     |           |<--------->|
       |<->|<--->|     |           |           |
 ------@a--------------+           +-----------+
     *5|               |           |           |
    ---|   +-----+     |  Decode   |           |
     *3|   |*****| --------+       @b----+     |---
       |   |*****|     |   |       |*****|     | *3
    ---|   +-----+     |   +-----> |*****|     |
       |               |           +-----+-----+---
 ------+---------------+           |  *2 |
                                   |<--->|

       Fig.2 Sample of decode cut-off setting
       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


 [Meaning of parameters]
   *1 :global_y_width   -> JIMGYGH (frame area width of the Y data)
   *2 :cutout_width     -> JIMGSH
   *3 :cutout_lines     -> JIMGSV
   *4 :cutout_offset_h  -> HOFFSET
   *5 :cutout_offset_v  -> VOFFSET
   @a :code_addr        -> JCODA
   @b :ycc_addr         -> JIMGAY

 [Usage]
   1. When Jpeg decode cut-off is not used(it means normal decode). (See Fig.1)
       - the value of cut_h_offset and cut_v_offset has to be 0.
       - width and lines is used download value.
   2. When Jpeg decode cut-off is used. (See Fig.2)

*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** Jpeg decode input table 
 * @remarks This is for utility
*/
typedef struct {
	ULONG				global_y_width;		/**< Global Y width (must 8 byte aligned value)<br>
												 :value range[64 - 1048512] */
	ULONG				global_c_width;		/**< Global CbCr width (must 8 byte aligned value)<br>
												 :value range[64 - 1048512] */
	E_IM_JPEG_MEM_FORM	mem_format;			/**< Memory format type */
	E_IM_JPEG_SMPL_TYPE	smpl_type;			/**< Sampling type */
	ULLONG				code_size;			/**< Size of Jpeg compressed data<br>
												 :value range[0 - 0x1FFFFFFFC00] */
	ULONG				code_addr;			/**< Header address of compressed data (must 8 byte aligned value) */
	T_IM_JPEG_ADDR_YCC	dst_ycc_addr;		/**< Header address of YCC data (must 8 byte aligned value) */
} T_IM_JPEG_DEC_INPUT;

/** Jpeg decode output table 
 * @remarks This is for utility
*/
typedef struct {
	E_IM_JPEG_SMPL_TYPE	smpl_type;			/**< Sampling type */
	USHORT				org_width;			/**< Horizontal of the original image data */
	USHORT				org_lines;			/**< Vertical of the original image data */
	UCHAR				err_code;			/**< Decompression error code */
} T_IM_JPEG_DEC_OUTPUT;

//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
This function cancels the Sleep state of Jpeg macro
*/
extern	VOID	Im_JPEG_Init( VOID );

/**
  This function open (acquire semaphore) of Jpeg macro.
  @param [in]	tmout			: Time of timeout<br>
									<ul>
										<li>Positive Value(Time of timeout)
										<li>@ref D_DDIM_USER_SEM_WAIT_POL
										<li>@ref D_DDIM_USER_SEM_WAIT_FEVR
									</ul>
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
  @retval		D_IM_JPEG_SEM_NG		: Semaphoore acquisition error.
  @retval		D_IM_JPEG_SEM_TIMEOUT	: Semaphoore acquisition TimeOut.
  @remarks		This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
				This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Im_JPEG_Open( INT32 tmout );

/**
  This function close (release semaphore) of Jpeg macro.
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_SEM_NG		: Semaphoore acquisition error.
  @remarks		This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Im_JPEG_Close( VOID );

/**
  This function set quantization table.
  @param [in]	p_quant_tbl				: Pointer of quantization table pack.<br>
  @param [in]	next_frm_flg			: Flag to determine whether a quantization table for the next frame.<br>
  											0: Current frame / 1: Next frame
  @remarks		"p_quant_tbl" is NULL, use the default table.<br>
*/
extern	VOID	Im_JPEG_Set_QTbl( T_IM_JPEG_QUAT_TBL_PACK* p_quant_tbl, UCHAR next_frm_flg );

/**
  This function set Jpeg quality values depend on quantization table.
  @param [in]	qua_value				: Quality value (format: S7.10)
  @param [in]	next_frm_flg			: Flag to determine whether a quantization table for the next frame.<br>
  											0: Current frame / 1: Next frame
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
*/
extern	INT32	Im_JPEG_Set_Quality( INT32 qua_value, UCHAR next_frm_flg );

/**
  This function is setting of the block downsampling.
  @param [in]	downsp_type		: downsampling type
									<ul>
										<li>@ref D_IM_JPEG_DOWNSP_NONE
										<li>@ref D_IM_JPEG_DOWNSP_1_2
										<li>@ref D_IM_JPEG_DOWNSP_1_4
										<li>@ref D_IM_JPEG_DOWNSP_1_8
									</ul>
  @retval		USHORT			: Proportion of remainder after adjusting for size when decimated.( format: 8.8 )
  @remarks		Please call this function after you have made the setting in Im_JPEG_Ctrl_Enc()
*/
extern	USHORT	Im_JPEG_Set_Down_Sampling_Rate( UCHAR downsp_type );

/**
  This function set Jpeg encode base management table data before encode process start.
  @param [in]	p_jpg_enc_mng			: Pointer to Jpeg encode base management table
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
  @retval		D_IM_JPEG_BUSY_NG		: Macro busy.
*/
extern	INT32	Im_JPEG_Ctrl_Enc( T_IM_JPEG_ENC_MNG* p_jpg_enc_mng );

/**
  This function set Jpeg encode frame management table data.
  @param [in]	p_jpg_enc_frm_mng		: Pointer to Jpeg encode frame management table
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
  @remarks		Please call this function after you have made the setting in Im_JPEG_Ctrl_Enc().<br>
				It can set during the encoding process, If the same base configuration to encode.<br>
				If you want to set in during Jpeg encoding process, please set after PBUF and JBUF is running.<br>
*/
extern	INT32	Im_JPEG_Ctrl_Enc_Frame( T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng );

/**
  This function get the base settings for Jpeg encode.
  @param [out]	p_jpg_enc_mng			: Pointer to Jpeg encode base management table
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
*/
extern	INT32	Im_JPEG_Get_Ctrl_Enc( T_IM_JPEG_ENC_MNG* p_jpg_enc_mng );

/**
  This function get the frame settings for Jpeg encode.
  @param [out]	p_jpg_enc_frm_mng		: Pointer to Jpeg encode frame management table
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
*/
extern	INT32	Im_JPEG_Get_Ctrl_Enc_Frame( T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng );

/**
  This function starts as asynchronous processing Jpeg encoded.
  @retval		D_IM_JPEG_OK				: Success.
  @retval		D_IM_JPEG_PARAM_ERROR		: Parameter error.
  @retval		D_IM_JPEG_SYSTEMCALL_ERR	: Systemcall error.
  @remarks		This API uses DDIM_User_Clr_Flg().<br><br>
  				If you want to synchronize, please call Im_JPEG_Wait_End_Enc().
*/
extern	INT32	Im_JPEG_Start_Enc( VOID );

/**
  This function wait end of Jpeg encode process.
  @param [out]	p_jpg_enc_mng				: Pointer to Jpeg encode result table
  @param [in]	time_out					: Time-out period
  @retval		D_IM_JPEG_OK				: Normal end.
  @retval		D_IM_JPEG_ENCODE_PAUSE		: Pause end (or limit size over end)
  @retval		D_IM_JPEG_PARAM_ERROR		: Parameter error.
  @retval		D_IM_JPEG_AXI_ERROR			: AXI bus error.
  @retval		D_IM_JPEG_TIMEOUT			: Encode timeout.
  @retval		D_IM_JPEG_ENCODE_ERR		: Encode error.
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Im_JPEG_Wait_End_Enc( T_IM_JPEG_ENC_MNG* p_jpg_enc_mng, INT32 time_out );

/**
  This function is restarted from the paused state of Jpeg macro.
  @param [in]	p_jpg_enc_frm_mng			: Pointer to Jpeg encode frame management table
  @retval		D_IM_JPEG_OK				: Normal end.
  @retval		D_IM_JPEG_RUNNING_NG		: Macro running error.
  @retval		D_IM_JPEG_PARAM_ERROR		: Parameter error.
  @retval		D_IM_JPEG_SYSTEMCALL_ERR	: System call error.
  @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern	INT32	Im_JPEG_Restart_Enc( T_IM_JPEG_ENC_FRAME_MNG* p_jpg_enc_frm_mng );

/**
  This function set Jpeg decode for skip marker mode.
  @retval		D_IM_JPEG_OK			: Normal end.
  @retval		D_IM_JPEG_RUNNING_NG	: Macro is running error.
  @remarks		Please use the skip marker mode after decoding normally once.<br>
  				It can be used to decode the compressed file from the skip marker mode.<br>
*/
extern	INT32	Im_JPEG_Set_Skip_Marker_Dec( VOID );

/**
  This function set Jpeg decode base management table data before decode process start.
  @param [in]	p_jpg_dec_mng			: pointer to Jpeg decode base management table
  @retval		D_IM_JPEG_OK			: Normal end.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
*/
extern	INT32	Im_JPEG_Ctrl_Dec( T_IM_JPEG_DEC_MNG* p_jpg_dec_mng );

/**
  This function set Jpeg decode frame management table data.
  @param [in]	p_jpg_dec_frm_mng		: pointer to Jpeg decode frame management table
  @retval		D_IM_JPEG_OK			: Normal end.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
  @remarks		Please call this function after you have made the setting in Im_JPEG_Ctrl_Dec()<br>
				It can also set during the decoding process, If the same base configuration to decode in a row<br>
				If you want to set in during Jpeg encoding process, please set after PBUF and JBUF is running.<br>
  @attention	Cutout function is only supported the following format.<br>
				:: YCC444(RGB) planer format<br>
				:: YCC422 planer format<br>
				:: YCC422 planer Y and dot sequential CbCr format<br>
				:: YCC422 tile format<br>
				:: YCC400 planer format
*/
extern	INT32	Im_JPEG_Ctrl_Dec_Frame( T_IM_JPEG_DEC_FRAME_MNG* p_jpg_dec_frm_mng );

/**
  This function get the base settings for Jpeg decode.
  @param [out]	p_jpg_dec_mng			: pointer to Jpeg decode base management table
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
*/
extern	INT32	Im_JPEG_Get_Ctrl_Dec( T_IM_JPEG_DEC_MNG* p_jpg_dec_mng );

/**
  This function get the frame settings for Jpeg decode.
  @param [out]	p_jpg_dec_frm_mng		: pointer to Jpeg decode frame management table
  @retval		D_IM_JPEG_OK			: Success.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
  @remarks		Please call this function after you have made the setting in Im_JPEG_Ctrl_Dec()<br>
				It can set during the decoding process, If the same base configuration to decode<br>
				If you want to set in during Jpeg decoding process, please set after PBUF and JBUF is running.<br>
*/
extern	INT32	Im_JPEG_Get_Ctrl_Dec_Frame( T_IM_JPEG_DEC_FRAME_MNG* p_jpg_dec_frm_mng );

/**
  This function starts as asynchronous processing Jpeg decoded.
  @retval		D_IM_JPEG_OK									: Normal end.
  @retval		D_IM_JPEG_RUNNING_NG							: Macro is running error.
  @retval		D_IM_JPEG_SYSTEMCALL_ERR						: System call error.
  @remarks		If you want to synchronize, please call Im_JPEG_Wait_End_Dec().<br>
  				This API uses DDIM_User_Clr_Flg().
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32 Im_JPEG_Start_Dec( VOID );

/**
  This function starts as asynchronous processing Jpeg decoded for Marker Skip.
  @retval		D_IM_JPEG_OK									: Normal end.
  @retval		D_IM_JPEG_RUNNING_NG							: Macro is running error.
  @retval		D_IM_JPEG_SYSTEMCALL_ERR						: System call error.
  @remarks		Please use the skip marker mode after decoding normally once.<br>
  				It can be used to decode the compressed file from the skip marker mode.<br>
  				If you want to synchronize, please call Im_JPEG_Wait_End_Dec().<br>
  				This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_JPEG_Start_Skip_Marker_Dec( VOID );

/**
  This function wait end of Jpeg decode process.
  @param [out]	p_jpg_dec_mng				: pointer to Jpeg decode result table
  @param [in]	time_out					: Time-out period
  @retval		D_IM_JPEG_OK				: Normal end.
  @retval		D_IM_JPEG_DECODE_PAUSE		: Pause end (or limit size over end)
  @retval		D_IM_JPEG_PARAM_ERROR		: Parameter error.
  @retval		D_IM_JPEG_AXI_ERROR			: AXI bus error
  @retval		D_IM_JPEG_TIMEOUT			: Decode timeout.
  @retval		D_IM_JPEG_DECODE_ERR		: Decode error
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Im_JPEG_Wait_End_Dec( T_IM_JPEG_DEC_MNG* p_jpg_dec_mng, INT32 time_out );

/**
  This function is restarted from the paused state of Jpeg macro.
  @param [in]	p_jpg_dec_frm_mng			: Pointer to Jpeg decode frame management table
  @retval		D_IM_JPEG_OK				: Normal end.
  @retval		D_IM_JPEG_RUNNING_NG		: Macro running error.
  @retval		D_IM_JPEG_PARAM_ERROR		: Parameter error.
  @retval		D_IM_JPEG_SYSTEMCALL_ERR	: System call error.
  @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern	INT32	Im_JPEG_Restart_Dec( T_IM_JPEG_DEC_FRAME_MNG* p_jpg_dec_frm_mng );

/**
  This function forcibly stop Jpeg encode process.
  @remarks		Please call to process error. In addition, please call Im_JPEG_Close() after call this function.
*/
extern	VOID	Im_JPEG_Stop( VOID );

/**
  This function is an interrupt handler.
  @remarks		This API uses DDIM_User_Set_Flg().
*/
extern	VOID	Im_JPEG_Int_Handler( VOID );

/**
  This function get AXI (PBUF and JBUF) status of frame processing.
  @param [out]	p_jp_axi_state			: pointer to AXI (JBUF and PBUF) state
  @retval		D_IM_JPEG_OK			: Normal end.
  @retval		D_IM_JPEG_PARAM_ERROR	: Parameter error.
*/
extern	INT32	Im_JPEG_Get_Axi_State( E_IM_JPEG_AXI_ST* p_jp_axi_state );

/**
  This function get processed image size (number of lines).
  @retval		USHORT 	: processed vertical image lines
*/
extern	USHORT	Im_JPEG_Get_Line_Cnt( VOID );

/**
  This function get processed image size (number of sector).
  @retval		ULONG 	: processed sector size (512 byte unit)
*/
extern	ULONG	Im_JPEG_Get_Sect_Cnt( VOID );

#ifdef CO_DDIM_UTILITY_USE
/** @name Utility Functions
@{*/

//---------------------- utility section -------------------------------

/**
  This function performs a Jpeg decoding process.
  @param [in]	input_param 				: pointer to Jpeg decode input parameters
  @param [out]	output_param				: pointer to Jpeg decode output parameters
  @retval		D_IM_JPEG_OK				: Normal end.
  @retval		D_IM_JPEG_PARAM_ERROR		: Parameter error.
  @retval		D_IM_JPEG_SYSTEMCALL_ERR	: System call error.
  @retval		D_IM_JPEG_DECODE_PAUSE		: limit size over.
  @retval		D_IM_JPEG_AXI_ERROR			: AXI bus error
  @retval		D_IM_JPEG_DECODE_ERR		: Decode error
  @retval		D_IM_JPEG_TIMEOUT			: Decode time out
*/
extern	INT32 Im_JPEG_Decode_Sync( T_IM_JPEG_DEC_INPUT* input_param, T_IM_JPEG_DEC_OUTPUT* output_param );

//---------------------- colabo  section -------------------------------

/*  @}*/
#endif	// CO_DDIM_UTILITY_USE

#ifdef __cplusplus
}
#endif

#endif // _IM_JPEG_H_
/*@}*/
/**
@weakgroup im_jpeg
@{
<hr>
@section JPEG_DECODE	Sample code for jpeg decode.
@code
// SAMPLE CODE //
INT32 jpeg_decode_sample()
{
	INT32					ret = 0;
	INT32					result = 0;
	T_IM_JPEG_DEC_MNG		jpgmng;
	T_IM_JPEG_DEC_FRAME_MNG	jpg_frm_mng;

	memset( &jpgmng, 0, sizeof(T_IM_JPEG_DEC_MNG) );
	memset( &jpg_frm_mng, 0, sizeof(T_IM_JPEG_DEC_FRAME_MNG) );

	jpgmng.ext_mode				= E_IM_JPEG_RESIZE_EXT_DIRECT;
	jpgmng.corr_mode			= D_IM_JPEG_ENABLE_OFF;
	jpgmng.skip_mk_flg			= D_IM_JPEG_ENABLE_OFF;
	jpgmng.smpl_type			= E_IM_JPEG_SMPL_TYPE_YCC422;
	jpgmng.pcallback			= NULL;

	jpg_frm_mng.global_y_width	= 4000;
	jpg_frm_mng.global_c_width	= 2000;
	jpg_frm_mng.cutout_flg		= D_IM_JPEG_ENABLE_OFF;
	jpg_frm_mng.ycc_addr.y		= 0x83000000;	// Y
	jpg_frm_mng.ycc_addr.c		= 0x83200000;	// C
	jpg_frm_mng.code_addr		= 0x81000000;
	jpg_frm_mng.code_size		= 0;

	ret = Im_JPEG_Open( D_DDIM_SEM_WAIT_FEVR );
	if (ret != D_IM_JPEG_OK) {
		Ddim_Print(("Im_JPEG_Open error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Ctrl_Dec(&jpgmng);
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Ctrl_Dec error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Ctrl_Dec_Frame(&jpg_frm_mng);
	if (ret == D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Ctrl_Dec_Frame error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Start_Dec();
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Start_Dec error! ercd = %d\n", ret));
		return ret;
	}

	ret = Im_JPEG_Wait_End_Dec(&jpgmng, D_DDIM_WAIT_END_TIME);
	if (ret == D_IM_JPEG_OK) {
		if ( ret == D_IM_JPEG_AXI_ERROR ) {
			Ddim_Print(("Im_JPEG_Wait_End_Dec AXI error! ercd = %d\n", ret));
		}
		else if ( ret == D_IM_JPEG_DECODE_PAUSE ) {
			Ddim_Print(("Im_JPEG_Wait_End_Dec Code size over error! ercd = %d\n", ret));
		}
		else {
			Ddim_Print(("Im_JPEG_Wait_End_Dec Decode error! ercd = %d\n", ret));
		}
		Im_JPEG_Stop();
	}

	// JPEG Close
	ret = Im_JPEG_Close();
	if (ret != D_IM_JPEG_OK) {
		Ddim_Print(("Im_JPEG_Close error! ercd = %d\n", result));
	}

	return ret;
}
@endcode

@section JPEG_ENCODE	Sample code for jpeg encode.
@code
// SAMPLE CODE //
INT32 jpeg_encode_sample()
{
	INT32					ret = 0;
	T_IM_JPEG_ENC_MNG		jpg_mng;
	T_IM_JPEG_ENC_FRAME_MNG	jpg_frm_mng;

	memset( &jpg_mng, 0, sizeof(T_IM_JPEG_ENC_MNG));
	memset( &jpg_frm_mng, 0, sizeof(T_IM_JPEG_ENC_FRAME_MNG));

	// set encode param
	jpg_mng.ycc_smpl			= E_IM_JPEG_SMPL_TYPE_YCC422;
	jpg_mng.width				= 4000;
	jpg_mng.lines				= 3000;
	jpg_mng.skip_mk_flg			= D_IM_JPEG_ENABLE_OFF;
	jpg_mng.dri_mk_num			= 0;
	jpg_mng.exif_fmt_flg		= D_IM_JPEG_ENABLE_ON;
	jpg_mng.pint_line			= 0;
	jpg_mng.pint_sect			= 0;
	jpg_mng.jburst_length		= E_IM_JPEG_BURST_INCR_8;
	jpg_mng.pburst_length		= E_IM_JPEG_BURST_INCR_8;

	// Y Component parameter
	jpg_mng.component[0].quant_tbl_no  = 0;	// Select Qunatization table No. for Y
	jpg_mng.component[0].huf_dc_tbl_no = 0;	// Select DC Huffman table No. for Y
	jpg_mng.component[0].huf_ac_tbl_no = 0;	// Select AC Huffman table No. for Y
	// Cb Component parameter
	jpg_mng.component[1].quant_tbl_no  = 1;	// Select Qunatization table No. for Cb
	jpg_mng.component[1].huf_dc_tbl_no = 1;	// Select DC Huffman table No. for Cb
	jpg_mng.component[1].huf_ac_tbl_no = 1;	// Select AC Huffman table No. for Cb
	// Cr Component parameter
	jpg_mng.component[2].quant_tbl_no  = 1;	// Select Qunatization table No. for Cr
	jpg_mng.component[2].huf_dc_tbl_no = 1;	// Select DC Huffman table No. for Cr
	jpg_mng.component[2].huf_ac_tbl_no = 1;	// Select AC Huffman table No. for Cr

	jpg_frm_mng.global_y_width		= 4000;
	jpg_frm_mng.global_c_width		= 2000;
	jpg_frm_mng.ycc_addr.y			= 0x83000000;
	jpg_frm_mng.ycc_addr.c			= 0x83200000;
	jpg_frm_mng.code_addr			= 0x81000000;
	jpg_frm_mng.code_count_flg		= D_IM_JPEG_ENABLE_OFF;
	jpg_frm_mng.code_over_count_flg	= D_IM_JPEG_ENABLE_OFF;
	jpg_frm_mng.limit_size			= 0x20000;

	ret = Im_JPEG_Open( D_DDIM_SEM_WAIT_FEVR );
	if (ret != D_IM_JPEG_OK) {
		Ddim_Print(("Im_JPEG_Open error! ercd = %d\n", ret));
		return ret;
	}

	// set quantization table of the default.
	Im_JPEG_Set_QTbl( NULL, 0 );

	// set base configuration for encode
	ret = Im_JPEG_Ctrl_Enc(&jpg_mng);
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Ctrl_Enc error! ercd = %d\n", ret));
		return ret;
	}

	// set frame configuration for encode
	ret = Im_JPEG_Ctrl_Enc_Frame(&jpg_frm_mng);
	if (ret == D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Ctrl_Enc_Frame error! ercd = %d\n", ret));
		return ret;
	}

	// encode start
	ret = Im_JPEG_Start_Enc();
	if (ret != D_IM_JPEG_OK) {
		Im_JPEG_Close();
		Ddim_Print(("Im_JPEG_Start_Enc error! ercd = %d\n", ret));
		return ret;
	}

	// wait the process end
	ret = Im_JPEG_Wait_End_Enc(&jpg_mng, D_DDIM_WAIT_END_TIME);
	if (ret == D_IM_JPEG_OK) {
		if ( ret == D_IM_JPEG_AXI_ERROR ) {
			Ddim_Print(("Im_JPEG_Wait_End_Enc AXI error! ercd = %d\n", ret));
		}
		else if ( ret == D_IM_JPEG_ENCODE_PAUSE ) {
			Ddim_Print(("Im_JPEG_Wait_End_Enc Code size over error! ercd = %d\n", ret));
		}
		else {
			Ddim_Print(("Im_JPEG_Wait_End_Enc Encode error! ercd = %d\n", ret));
		}
		Im_JPEG_Stop();
	}

	ret = Im_JPEG_Close();
	if (ret != D_IM_JPEG_OK) {
		Ddim_Print(("Im_JPEG_Close error! ercd = %d\n", ret));
		return ret;
	}

	return D_IM_JPEG_OK;
}
@endcode

@section JPEG_DECODE_UTIL	Sample code for jpeg decode(util).
@code
// SAMPLE CODE //
INT32 jpeg_simplicity_decode_sample()
{
	INT32					result;
	T_IM_JPEG_DEC_INPUT		input_param;
	T_IM_JPEG_DEC_OUTPUT	output_param;

	memset( &input_param, 0, sizeof(T_IM_JPEG_DEC_INPUT) );
	memset( &output_param, 0, sizeof(T_IM_JPEG_DEC_OUTPUT) );

	// set jpeg addr
	input_param.global_y_width	= 3648;
	input_param.global_c_width	= (3648 >> 1);
	input_param.code_addr		= 0x83000000;
	input_param.smpl_type		= E_IM_JPEG_SMPL_TYPE_YCC422;

	// set output(Y,Cb,Cr) addr
	input_param.dst_ycc_addr.y  = 0x81000000;
	input_param.dst_ycc_addr.c	= input_param.dst_ycc_addr.y + (3648 * 2736);

	// set code size
	input_param.code_size		= 0;	// no limit

	// decode start
	result = Im_JPEG_Decode_Sync(&input_param, &output_param);
	if(result != D_IM_JPEG_OK) {

		// Analysis of the "output_param.err_code"

		// Error processing
		Ddim_Print(("Im_JPEG_Decode_Sync error! ercd = %d\n", result));
		return result;
	}
	return D_IM_JPEG_OK;
}
@endcode

@}*/

/*	@}*/
/*@}*/
