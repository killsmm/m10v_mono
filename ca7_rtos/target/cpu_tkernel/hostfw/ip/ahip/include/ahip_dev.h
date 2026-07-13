/**
 * @file		ahip_dev.h
 * @brief		Header file
 * @author	 	SOCIONEXT
 * @since		2013/04/04
 * @note		None
 * @attention	None
 */

#ifndef _AHIP_DEV_H_
#define _AHIP_DEV_H_

typedef enum {
	E_AHIP_DEV_IMG_FMT_YCC422 = 0,		/**< YCC422			*/
	E_AHIP_DEV_IMG_FMT_YCC420,			/**< YCC420			*/
} E_AHIP_DEV_IMG_FMT;

typedef struct {
	UINT32 width;						/**< width */
	UINT32 lines;						/**< lines */
} T_AHIP_DEV_SIZE;

typedef struct {
	ULONG AddressY;						/**< Y Addr		*/
	ULONG AddressCb;					/**< Cb Addr	*/
	ULONG AddressCr;					/**< Cr Addr	*/
} T_AHIP_DEV_ADDR_YCC;

typedef struct {
	E_AHIP_DEV_IMG_FMT	format;
	T_AHIP_DEV_SIZE		yuv_size;
	USHORT				g_y_width;		/**< horizontal global size	*/
	USHORT				g_c_width;		/**< horizontal global size	*/
	T_AHIP_DEV_ADDR_YCC	addr;
	ULLONG				frame_no;
} T_AHIP_DEV_YUV_INFO;

extern VOID Ahip_Dev_Init_CB( const UCHAR demosaic_id, ULONG heap_addr, T_AHIP_DEV_YUV_INFO* yuv_info );
extern VOID Ahip_Dev_Yuv_Output_CB( UCHAR demosaic_id, T_AHIP_DEV_YUV_INFO* yuv_info );

extern VOID Ahip_Dev_Vis_Enabled( UCHAR demosaic_id, UCHAR enabled );
extern VOID Ahip_Dev_Fd_Enabled( UCHAR demosaic_id, UCHAR enabled );
extern VOID Ahip_Dev_Fd_AutoLog_Enabled( UCHAR demosaic_id, UCHAR enabled );
extern DOUBLE Ahip_Dev_Get_Sensor_Exp_Time( UCHAR sro_id );
extern ULONG Ahip_Dev_Get_Sensor_Readout_Time_us( UCHAR sro_id, ULLONG sensor_vd_no );
extern ULONG Ahip_Dev_Get_Sensor_Sof_Timestamp( UCHAR sro_id, ULLONG sensor_vd_no );
extern ULONG Ahip_Dev_Get_Time_us( VOID );

#endif	// _AHIP_DEV_H_
