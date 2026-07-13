/**
 * @file		sr.h
 * @brief		SIRIUS Function Driver header file
 * @author		SOCIONEXT CNI
 * @since		2015/10/14
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT LIMITED 2015</I></B>
 */

#ifndef _SR_H_
#define _SR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//[Master Definition] *compile option
#define CO_SR_ACT_CHIPTOP		// CHIPTOP Control
#define CO_SR_HOST_FUNC			// Host defined function
//#define CO_SR_RRODUCT_MAP_EXTERN	// product map (External definition)
	
//----------------------
// return value
//----------------------
#define SR_OK						( 0)			// Normal end
#define SR_INVALID_PARAM			(-1)			// parameter error
#define SR_COMMAND_ERROR			(-2)			// Command Error

#define SR_FM_ALLOC_OK				(0)				// FM Alloc OK
#define SR_FM_ALLOC_NG				(0xFFFFFFFF)	// FM Alloc NG

//----------------------
// common
//----------------------
#define SR_ON						(1)				// ON
#define SR_OFF						(0)				// OFF

#define SR_ENCODE_NUM				(2)				//encode number
#define SR_DECODE_NUM				(1)				//decode number

#define SR_ST_BOOT					(0x00000001)	//Boot Done
#define SR_ST_READY					(0x00000002)	//Init Done
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define SR_ST_ENC_READY				(0x00000010)	//Encode Ready
#define SR_ST_ENC_RUN				(0x00000020)	//Encode Run
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define SR_ST_DEC_READY				(0x00000100)	//Decode Ready
#define SR_ST_DEC_RUN				(0x00000200)	//Decode Run

#ifdef CO_SR_RRODUCT_MAP_EXTERN
#define SR_PRODUCT_MAP_1GB			(0x1)			//1GB map
#define SR_PRODUCT_MAP_2GB			(0x2)			//2GB map
#define SR_PRODUCT_MAP_3GB			(0x3)			//3GB map
#endif

//----------------------
//	INIT_PARAM
//----------------------
// Encode Init Definition
#define ENC_FM_NUM					(0xF)			//Encode Frame number
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define SR_NAL_UNIT_SIZE_TBL_NUM	(32) 			//NAL UNIT SIZE TABLE NUM
#define PARTIAL_NUM					(2) 			//PARTIAL QP NUM

#define SR_ENCODE_MODE_0			(0)				//NHD  (1280x960)
#define SR_ENCODE_MODE_1			(1)				//HD   (1920x1504)
#define SR_ENCODE_MODE_2			(2)				//4k2k (4096x2160)
#define SR_ENCODE_MODE_3			(3)				//3k   (3000x3000)
#define SR_ENCODE_MODE_4			(4)				//4k3k (4000x3000)
#define SR_ENCODE_MODE_5			(5)				//6k3k (6000x3000)
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// Decode Init Definition
#define SR_DECODE_MODE_0			(0)			//NHD  (1280x960)
#define SR_DECODE_MODE_1			(1)			//1080p(1920x1080)
#define SR_DECODE_MODE_2			(2)			//HD   (1920x1504)
#define SR_DECODE_MODE_3			(3)			//2.7k(16:9) (2704x1520)
#define SR_DECODE_MODE_4			(4)			//2.7k(4:3)  (2704x2028)
#define SR_DECODE_MODE_5			(5)			//4k   (3840x2160)
#define SR_DECODE_MODE_6			(6)			//4kCinema   (4096x2160)
#define SR_DECODE_MODE_7			(7)			//3k   (3000x3000)
#define SR_DECODE_MODE_8			(8)			//4k3k (4000x3000)
#define SR_DECODE_MODE_9			(9)			//6k3k (6000x3000)
	
//Decode Playback Mode
#define SR_DECODE_FORWARD			(0)				//Forward Playback
#define SR_DECODE_BACKWARD			(1)				//Backward Playback
#define SR_DECODE_FF				(2)				//Fast Forward Playback
#define SR_DECODE_REW				(3)				//Fast Backward Playback

//----------------------
//	INT
//----------------------
// System Interrupt Status
#define SR_SYSTEM_VIDEOBOOT			(0x00000001)	// Video Boot
#define SR_SYSTEM_SYSTEMBOOT		(0x40000000)	// Sysetm Boot

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
// Encode Interrupt Status
#define SR_ENCODE_START_DONE		(0x00000400)	// Encode Start
#define SR_ENCODE_STOP_DONE			(0x00000800)	// Encode Stop
#define SR_ENCODE_SLICE_DONE		(0x00001000)	// Slice Encode End
#define SR_ENCODE_FRAME_DONE		(0x00002000)	// Frame Encode End
#define SR_ENCODE_ERROR				(0x00004000)	// Encode Error
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// Decode Interrupt Status
#define SR_DECODE_START_DONE		(0x00000001)	// Decode Satrt
#define SR_DECODE_ERROR				(0x00000002)	// Decode Error
#define SR_DECODE_FRAME_DONE		(0x00000004)	// Frame Decode End
#define SR_DECODE_STOP_DONE			(0x00000008)	// Decode Stop 
#define SR_DECODE_TIMEOUT			(0x00000080)	// Timer Out
#define SR_DECODE_EOB_DETECT		(0x00000100)	// EOB Detect

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//----------------------
// Common
//----------------------
typedef struct{
 unsigned int	start_addr;
 unsigned int	end_addr;
} T_SR_BRIDGE_DDRAREA;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
//----------------------
// Encode
//----------------------

typedef struct{
 unsigned int	gopn;
 unsigned int	gopm;
 unsigned int	level;
 unsigned int	venchsize;
 unsigned int	vencvsize;
 unsigned int	videomaxbitrate;
 unsigned int	videoavrgbitrate;
 unsigned int	frame_rate_denominator;
 unsigned int	frame_rate_numerator;
 unsigned int	aspectratio;
 unsigned int	sarwidth;
 unsigned int	sarheight;
 unsigned int	insert_pic_struct;
 unsigned int	buf_ctrl_param;
 unsigned int	filler_en;
 unsigned int	insert_pes_header;
 unsigned int	video_signal_type_present_flag;
 unsigned int	video_format;
 unsigned int	video_full_range_flag;
 unsigned int	colour_description_present_flag;
 unsigned int	colour_primaries;
 unsigned int	transfer_characteristics;
 unsigned int	matrix_coeffs;
 unsigned int	chroma_loc_info_present_flag;
 unsigned int	chroma_sample_loc_type_top;
 unsigned int	chroma_sample_loc_type_bottom;
 unsigned int	ratectl;
 unsigned int	gopstructure;
 unsigned int	encstart_delay;
 unsigned int	pic_crop_right_offset;
 unsigned int	pic_crop_bottom_offset;
 unsigned int	power_mode;
 unsigned int	rec_format;
 unsigned int	partial_qpmod;
} T_SR_ENC_INITPARAM;

typedef struct{
 unsigned int	sumy_top;
 unsigned int	sumy_bot;
 unsigned int	ssqy_top;
 unsigned int	ssqy_bot;
 unsigned int	sumcb_top;
 unsigned int	sumcb_bot;
 unsigned int	ssqcb_top;
 unsigned int	ssqcb_bot;
 unsigned int	sumcr_top;
 unsigned int	sumcr_bot;
 unsigned int	ssqcr_top;
 unsigned int	ssqcr_bot;
 unsigned int	frame_param_flag;
 unsigned int	aspectratio;
 unsigned int	sarwidth;
 unsigned int	sarheight;
 unsigned int	videoavrgbitrate;
 unsigned int	gopstructure;
 unsigned int	gopn;
 unsigned int	gopm;
 unsigned int	slice_num;
 unsigned int	partial_adapqpen[PARTIAL_NUM];
 unsigned int	partial_qdownrange[PARTIAL_NUM];
 unsigned int	partial_qpmin[PARTIAL_NUM];
 unsigned int	partial_startpos[PARTIAL_NUM];
 unsigned int	partial_widthsize[PARTIAL_NUM];
 unsigned int	partial_height[PARTIAL_NUM];
 unsigned int	partial_exptadapqpen;
 unsigned int	partial_qpuprange;
 unsigned int	partial_qpmax;
 unsigned int	serial_num;
 unsigned int	frame_count_in_gop;
} T_SR_ENC_PARAM;

typedef struct{		
 unsigned int	use_flag[ENC_FM_NUM];							// Flag indicating that using the ENC FM
 unsigned int	pts[ENC_FM_NUM];								// Picture displaying time
 unsigned int   physical_bank_idx[ENC_FM_NUM];
} T_SR_ENC_FM_TABLE;

typedef struct{
 unsigned int		frame_end_veb_pointer;						// VB pointer of the frame termination
 unsigned int		flag;										// Frame information flag
 unsigned int		type;										// Registration information type
 unsigned int		period;										// Video AU display period
 unsigned int		cpb_over_flag;								// Notify the buffer underflow the picture
 unsigned long long	dts;										// Picture decode time
 unsigned long long	pts;										// Picture display time
 unsigned int		picture_size;								// Video frame size
 unsigned int		nal_unit_size[SR_NAL_UNIT_SIZE_TBL_NUM];	// NAL Unit size
 unsigned int		nal_unit_num;								// Effective number of nal_unit_size array
 unsigned int		serial_num;									// The total number of EncodeFrame
 unsigned int		frame_count_in_gop;							// Frame number of the current GOP
 int				poc_val;									// POC value
} T_SR_ENC_VINFO;

typedef struct{
 unsigned int	encctlrrlevel;									// Error level of encode control layer
 unsigned int	vencerrlevel;									// Error level of the video encoder
 unsigned int	encctlerrfactor;								// Error factor of encode control layer
 unsigned int	vencerrfactor;									// Error factor of the video encoder
} T_SR_ENC_ERROR_FACTOR;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// TBD Scheduled to be deleted
typedef struct{
 T_SR_BRIDGE_DDRAREA	encbufy[ENC_FM_NUM];					// Encode Y data Buffer
 T_SR_BRIDGE_DDRAREA	encbufc[ENC_FM_NUM];					// Encode C data Buffer
 T_SR_BRIDGE_DDRAREA	dpb;									// Encode Buffer
 T_SR_BRIDGE_DDRAREA	veb;									// Video Elementary Buffer
 T_SR_BRIDGE_DDRAREA	dpb_cmprs;								// Encode Buffer Compression Buffer
 T_SR_BRIDGE_DDRAREA	encbuf_cmprs;							// Encode Y/C data Buffer Compression Buffer
 unsigned int dpb_cmprs_mode;
 unsigned int encbuf_cmprs_mode;
 unsigned int encode_mode;
 unsigned int frame_width;
 unsigned int frame_height;
} T_SR_BRIDGE_MAP_ENC;


//----------------------
// Decode
//----------------------
typedef struct{
 T_SR_BRIDGE_DDRAREA	param;			// parameter area
 T_SR_BRIDGE_DDRAREA	colpic;			// colpic area
 T_SR_BRIDGE_DDRAREA	vbb;			// VBB area
 T_SR_BRIDGE_DDRAREA	fm_y;			// FM Y area
 T_SR_BRIDGE_DDRAREA	fm_c;			// FM C area
 unsigned int decode_mode;
} T_SR_BRIDGE_MAP_DEC;

typedef struct{
 unsigned int	pic_addr; 
 unsigned int	pic_size;
 unsigned int	pts; 
} T_SR_DEC_PICTURE_INFO;

typedef struct{
 unsigned int	dec_info;				// Video Decode information
 unsigned int	codec_type;				// Video code type
 unsigned int	sei_info_start_addr;	// Video SEI Information Start Address
 unsigned int	sei_info_size;			// Video SEI Information Size
} T_SR_INFO_FM;

typedef struct{
 unsigned int	id;						// id
 unsigned int	y_addr;					// Y Address
 unsigned int	c_addr;					// C Address
 unsigned int	pts;					// PTS
} T_SR_FM_OUT_INFO;

typedef struct{
 unsigned int	decctlerrfactor;
} T_SR_DEC_ERROR_FACTOR;

//----------------------
// System
//----------------------

typedef struct{
 unsigned int	ddr_ch0_top_addr; 			// DDR ch0 Top address
 unsigned int	ddr_ch1_top_addr; 			// DDR ch1 Top address
 unsigned int	ddr_ch0_top_common_addr;  	// DDR ch0 Top Common address
 unsigned int	ddr_ch0_common_size;	  	// DDR ch0 Common size
 unsigned int	ddr_ch1_top_common_addr;  	// DDR ch1 Top Common address
 unsigned int	ddr_ch1_common_size;	  	// DDR ch1 Common size
#ifdef CO_SR_RRODUCT_MAP_EXTERN
 unsigned int	ddr_product_map_flag;		// DDR Product Map Flag(1GB or 2GB/3GB)
#endif
	
} T_SR_BRIDGE_DDR_CONFIG;

typedef struct{
 T_SR_BRIDGE_DDR_CONFIG	ddr_config;
 T_SR_BRIDGE_MAP_ENC	map_enc[SR_ENCODE_NUM];
 T_SR_BRIDGE_MAP_DEC	map_dec[SR_DECODE_NUM];
} T_SR_BRIDGE_MAP_PARAM;

//---------------------------------------
// Changes Definition to User Function
//---------------------------------------
#ifdef CO_SR_ACT_CHIPTOP
#define SR_Power_Set(arg) HEVC_Sirius_Power_Set(arg) // user definition
extern void HEVC_Sirius_Power_Set(unsigned char arg);
#else
#define SR_Power_Set(arg)
#endif

#ifdef CO_SR_ACT_CHIPTOP
#define SR_Reset_Set(arg) HEVC_Sirius_Reset_Set(arg) // user definition
extern void HEVC_Sirius_Reset_Set(unsigned char arg);
#else
#define SR_Reset_Set(arg)
#endif

#ifdef CO_SR_ACT_CHIPTOP
#define SR_Clock_Set(arg) HEVC_Sirius_Clock_Set(arg) // user definition
extern void HEVC_Sirius_Clock_Set(unsigned char arg);
#else
#define SR_Clock_Set(arg)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_DSB(void) HEVC_Sirius_DSB(void)	// user definition
extern  void HEVC_Sirius_DSB( void );
#else
#define SR_DSB(void)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_READ(address) HEVC_Sirius_READ(address)	// user definition
extern  unsigned int HEVC_Sirius_READ( unsigned int address);
#else
#define SR_READ(address)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_WRITE(address, data) HEVC_Sirius_WRITE(address, data)	// user definition
extern  void HEVC_Sirius_WRITE( unsigned int address, unsigned int data );
#else
#define SR_WRITE(address, data)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_CLEAN(address, size) HEVC_Sirius_Clean(address, size)	// user definition
extern  void HEVC_Sirius_Clean (unsigned int address, unsigned int size );
#else
#define SR_CLEAN(address, size)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_FLUSH(address, size) HEVC_Sirius_Flush(address, size)	// user definition
extern  void HEVC_Sirius_Flush (unsigned int address, unsigned int size );
#else
#define SR_FLUSH(address, size)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_DPD(void) HEVC_Sirius_Dpd(void)
extern unsigned int HEVC_Sirius_Dpd (void);
#else
#define SR_DPD(void)
#endif

#ifdef CO_SR_HOST_FUNC
#define SR_DPE(status) HEVC_Sirius_Dpe(status)
extern void HEVC_Sirius_Dpe (unsigned int status);
#else
#define SR_DPE(status)
#endif

#ifdef CO_SR_RRODUCT_MAP_EXTERN
#ifdef CO_SR_HOST_FUNC
#define SR_BootFirm_Download(flag,addr,size) HEVC_Sirius_BootFirm_Download(flag,addr,size)
extern void HEVC_Sirius_BootFirm_Download( unsigned int flag, unsigned int addr, unsigned int size );
#else
#define SR_BootFirm_Download(flag,addr,size)
#endif
#endif

#ifdef CO_SR_RRODUCT_MAP_EXTERN
#ifdef CO_SR_HOST_FUNC
#define SR_UcodeFirm_Download(flag,addr,size) HEVC_Sirius_UcodeFirm_Download(flag,addr,size)
extern void HEVC_Sirius_UcodeFirm_Download( unsigned int flag, unsigned int addr, unsigned int size );
#else
#define SR_UcodeFirm_Download(flag,addr,size)
#endif
#endif

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
//----------------------
// Interrupt Handler
//----------------------
extern unsigned int SR_Int_CpueintRead_Sys( void );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
extern unsigned int SR_Int_CpueintRead_Enc( void );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern unsigned int SR_Int_CpueintRead_Dec( void );

extern void SR_Int_Cpueint_Sys( unsigned int );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
extern void SR_Int_Cpueint_Enc( unsigned int );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern void SR_Int_Cpueint_Dec( unsigned int );

//----------------------
// Bridge
//----------------------
extern int SR_Bridge_Set_MemoryMap ( T_SR_BRIDGE_MAP_PARAM* map_param );

//----------------------
// CPU
//----------------------
extern int SR_Cpu_System_Boot( void (*sys_handler)(unsigned int ,unsigned int), void (*enc_handler)(unsigned int ,unsigned int) , void (*dec_handler)(unsigned int ,unsigned int) ,unsigned int code_dl_cut);
extern int SR_Cpu_Video_Boot( void );
extern int SR_Cpu_System_Reset( void );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
//----------------------
// ENCODE
//----------------------
extern int SR_Encode_Start (unsigned int ch, T_SR_ENC_INITPARAM* initparam, T_SR_ENC_FM_TABLE* fmtable);
extern int SR_Encode_Stop (unsigned int ch );
extern int SR_Encode_Fm_Free_Get (unsigned int ch, T_SR_ENC_FM_TABLE* fmtable);
extern int SR_Encode_Frame (unsigned int ch, T_SR_ENC_PARAM *vencparam, T_SR_ENC_FM_TABLE* fmtable);	// slice_num:TBD
extern int SR_Encode_Veb_Info_Set (unsigned int ch, unsigned int vstrm_size);
extern int SR_Encode_Frame_Info_Get (unsigned int ch, T_SR_ENC_VINFO* vinfom);
extern int SR_Encode_Error_Info_Get (unsigned int ch, T_SR_ENC_ERROR_FACTOR* errfactor);
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

//----------------------
// DECODE
//----------------------
extern int SR_Decode_Start(unsigned int ch, unsigned int playback_mode);
extern int SR_Decode_Frame(unsigned int ch, T_SR_DEC_PICTURE_INFO* picture_info, unsigned int *alloc_result);
extern int SR_Decode_Stop(unsigned int ch);
extern int SR_Decode_Frame_Alloc(unsigned int ch, T_SR_FM_OUT_INFO* fminfo, unsigned int *alloc_result);
extern int SR_Decode_Frame_Keepid(unsigned int ch, unsigned int id);
extern int SR_Decode_Frame_Free(unsigned int ch, unsigned int id);
extern int SR_Decode_Info_Get(unsigned int ch, T_SR_INFO_FM* decinfo);
extern int SR_Decode_Error_Info_Get(unsigned int ch, T_SR_DEC_ERROR_FACTOR* errfactor);

#ifdef __cplusplus
}
#endif

#endif /* _SR_H_ */
