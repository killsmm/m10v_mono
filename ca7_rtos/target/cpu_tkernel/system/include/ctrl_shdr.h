/**
 * @file		ctrl_shdr.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _CTRL_SHDR_H_
#define _CTRL_SHDR_H_

#include "share.h"
#include "share_data.h"

// delete_basefw\fj\iq
//#include "iq_3dnr.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IPU_3DNR_MARGIN		(32)

#define D_IPU_3DNR_PROCESS_BY_TASK

#define D_IPU_GYRO_BUFF_NUM		(65536)

#define D_IPU_GE_HEAP_SIZE		(6*1024*1024)
#define D_IPU_GMD_HEAP_SIZE		(9*1024*1024)

#define D_SGDE_MEM_BANK_YUV		(4)
#define D_SHDR_MEM_BANK_YUV		(3)
#ifdef CO_MEMORY_1GB // can be "#if 1"
#define D_VHDR_MEM_BANK_YUV		(2)
#else
#define D_VHDR_MEM_BANK_YUV		(3)
#endif

#define D_IPU_PROC_STS_CB_STOP		(0)
#define D_IPU_PROC_STS_CB_START		(1)

#define D_IPU_CMP_TABLE_SIZE		(18432)

#define D_IPU_ALIGN_MEM_SIZE		(128)
#define D_IPU_ALIGN_ME_VEC_ADDR		(1024)
#define D_IPU_ALIGN_ME_IMG_ADDR		(16384)
#define D_IPU_ALIGN_ME_G_WIDTH		(512)
#define D_IPU_ALIGN_VIDEO_ADDR		(16384)
#define D_IPU_ALIGN_VIDEO_G_WIDTH	(512)
#define D_IPU_ALIGN_VIDEO_G_LINE	(32)

#ifdef D_IPU_3DNR_PROCESS_BY_TASK
#define FLG_IPU_3DNR_GMD_START		(0x00000001)
#define FLG_IPU_3DNR_ME_START		(0x00000010)
#define FLG_IPU_3DNR_SHDR_START		(0x00000100)
#define FLG_IPU_3DNR_SHDR_RECONFIG	(0x00000200)
#define FLG_IPU_3DNR_STOP			(0x01000000)

#define FLG_IPU_HDR_VHDR_START		(0x00000001)
#define FLG_IPU_HDR_STOP			(0x01000000)

#define FLG_IIP_SCALE_START			(0x00000010)
#define FLG_IIP_SCALE_STOP			(0x00000100)
#endif
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_SGDE_OUT_0 = 0,
	E_SGDE_OUT_1,
	E_SGDE_OUT_2,
	E_SGDE_OUT_MAX,
	E_SGDE_OUT_NONE = 0xFF,
} E_SGDE_IMAGE_OUT;

typedef enum {
	E_SHDR_OUT_RBK0_H265 = 0,
	E_SHDR_OUT_RBK1_ME,
	E_SHDR_OUT_RBK2_H264_0,
	E_SHDR_OUT_RBK3_H264_1,
	E_SHDR_OUT_RBK4_YUV_0,
	E_SHDR_OUT_RBK5_YUV_1,
	E_SHDR_OUT_MAX,
	E_SHDR_OUT_NONE = 0xFF,
} E_SHDR_OUT_RBK;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG   			mem_addr_y;
	ULONG   			mem_addr_c;
	USHORT				yuv_h_byte;
	USHORT				yuv_h_size;
	USHORT				yuv_v_size;
	E_SHARE_YUV_FORMAT	yuv_format;
} T_SHDR_MNG_IN;

typedef struct {
	ULONG   			mem_addr_y;
	ULONG   			mem_addr_c;
	USHORT				yuv_h_byte;
	USHORT				yuv_h_size;
	USHORT				yuv_v_size;
	E_SHARE_YUV_FORMAT	yuv_format;
	E_SHARE_YUV_VIDEO	yuv_video;
} T_SHDR_MNG_OUT;

typedef struct {
	ULONG   			mem_addr_r;
	ULONG   			mem_addr_g;
	ULONG   			mem_addr_b;
	USHORT				rgb_h_byte;
	USHORT				rgb_h_size;
	USHORT				rgb_v_size;
	USHORT				bit_depth;
} T_VHDR_MNG_OUT;

typedef struct {
	SHORT				shift_x;
	SHORT				shift_y;
} T_SHDR_MOTION;

typedef struct {
	ULONG   			mem_addr_yp;
	ULONG   			mem_addr_yn;
	USHORT				h_byte;
	USHORT				h_size;
	USHORT				v_size;
} T_LTM_MAP_MNG_OUT;

typedef struct {
	// SGDE
	ULONG   			sgde_mem_addr_y[E_SGDE_OUT_MAX][D_SGDE_MEM_BANK_YUV];
	ULONG   			sgde_mem_addr_c[E_SGDE_OUT_MAX][D_SGDE_MEM_BANK_YUV];
	UCHAR   			sgde_mem_bank_y[E_SGDE_OUT_MAX];
	UCHAR   			sgde_mem_bank_c[E_SGDE_OUT_MAX];
	// GMD
	ULONG   			gmd_heap_addr;
	// IIP	
	ULONG   			iip_mem_addr_y[D_SHDR_MEM_BANK_YUV];
	ULONG   			iip_mem_addr_c[D_SHDR_MEM_BANK_YUV];
	UCHAR   			iip_mem_bank_y[E_MEM_BANK_MAX];
	UCHAR   			iip_mem_bank_c[E_MEM_BANK_MAX];
	// ME	
	ULONG   			me_mem_addr_y[D_SHDR_MEM_BANK_YUV];
	ULONG   			me_mem_addr_c[D_SHDR_MEM_BANK_YUV];
	UCHAR   			me_mem_bank_y[E_MEM_BANK_MAX];
	UCHAR   			me_mem_bank_c[E_MEM_BANK_MAX];
	// SHDR	
	ULONG   			shdr_mem_addr_y[E_SHDR_OUT_MAX][D_SHDR_MEM_BANK_YUV];
	ULONG   			shdr_mem_addr_c[E_SHDR_OUT_MAX][D_SHDR_MEM_BANK_YUV];
	UCHAR   			shdr_mem_bank_y[E_SHDR_OUT_MAX][E_MEM_BANK_MAX];
	UCHAR   			shdr_mem_bank_c[E_SHDR_OUT_MAX][E_MEM_BANK_MAX];
	ULONG   			shdr_cmp_mem_addr_y[D_SHDR_MEM_BANK_YUV];
	ULONG   			shdr_cmp_mem_addr_c[D_SHDR_MEM_BANK_YUV];
	UCHAR   			shdr_cmp_mem_bank_y[E_MEM_BANK_MAX];
	UCHAR   			shdr_cmp_mem_bank_c[E_MEM_BANK_MAX];
	// vHDR	
	ULONG   			vhdr_heap_addr;
	ULONG   			vhdr_heap_size;
	ULONG   			vhdr_mem_addr_r[D_VHDR_MEM_BANK_YUV];
	ULONG   			vhdr_mem_addr_g[D_VHDR_MEM_BANK_YUV];
	ULONG   			vhdr_mem_addr_b[D_VHDR_MEM_BANK_YUV];
	UCHAR   			vhdr_mem_bank_r[E_MEM_BANK_MAX];
	UCHAR   			vhdr_mem_bank_g[E_MEM_BANK_MAX];
	UCHAR   			vhdr_mem_bank_b[E_MEM_BANK_MAX];
	
	// SGDE
	T_SHDR_MNG_IN		sgde_in;
	T_SHDR_MNG_OUT  	sgde_out[E_SGDE_OUT_MAX];
	T_SHDR_MNG_IN		hre_ref;
	
	// GMD	
	T_SHDR_MNG_OUT		gmd_in[2];
	T_SHDR_MNG_OUT		gmd_ref[2];
	
	// IIP	
	T_SHDR_MNG_OUT		iip_in;
	T_SHDR_MNG_OUT		iip_out;
	
	// ME	
	T_SHDR_MNG_IN		me_in;
	T_SHDR_MNG_IN		me_out;
	
	// SHDR	
	T_SHDR_MNG_IN		ref_in[2];
	T_SHDR_MNG_IN		shdr_in;
	T_SHDR_MNG_IN		cmptbl_in;
	T_SHDR_MNG_OUT  	shdr_out[E_SHDR_OUT_MAX];
	T_SHDR_MNG_OUT		cmptbl_out;
	
	// vHDR	
	BOOL				vhdr_on;
	T_SHDR_MNG_IN		vhdr_in[3]; // 0:Short Exposure, 1:Long Exposure, 2:Extra
	T_VHDR_MNG_OUT		vhdr_out;

	T_SEN_STREAM_MAPPING sen_stream;
} T_IPU2SHDR_MNG;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define M_IPU_MAKE_ALIGNMENT( val, align )	(((UINT32)(val) + (align -1)) & (~(align -1)))

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

//3DNR
extern VOID IPU_3DNR_IQ_SetParam( VOID );
extern void Init_GMD_Proc( ULONG heap_addr, const T_SHDR_MNG_OUT* img_red, const T_SHDR_MNG_OUT* img_full  );
extern void Start_GMD_Proc( T_IPU2SHDR_MNG* in, E_SHARE_OP_3DNR_MODE mode, ULLONG frame_no );
extern void Close_GMD_Proc( void );
extern void Init_ME_Proc( void );
extern void Start_ME_Proc( T_IPU2SHDR_MNG* in, ULLONG frame_no );
extern void Close_ME_Proc( void );
extern void Init_SHDR_Proc( void );
extern void Start_SHDR_Proc( T_IPU2SHDR_MNG* in, E_SHARE_OP_3DNR_MODE mode, BOOL compMode, ULLONG frame_no, UCHAR cbcr_global_x2 );
extern void Close_SHDR_Proc( void );

#ifdef D_IPU_3DNR_PROCESS_BY_TASK
void Tsk_IPU_3DNR_GMD_Main( INT32 stacd );
void Tsk_IPU_3DNR_ME_Main( INT32 stacd );
void Tsk_IPU_3DNR_SHDR_Main( INT32 stacd );
VOID Tsk_IIP_Image_Scale( INT32 stacd );

#endif

//vHDR
extern void Init_VHDR_Proc(ULONG heap_addr, ULONG heap_size, USHORT yuv_h_size, USHORT yuv_v_size );
extern void Start_VHDR_Proc( T_IPU2SHDR_MNG* in, T_SHARE_R2Y_DOL_HIST* hist, ULLONG frame_no );
extern void Close_VHDR_Proc( void );
#ifdef D_IPU_3DNR_PROCESS_BY_TASK
void Tsk_IPU_HDR_VHDR_Main( INT32 stacd );
#endif

// Debug
extern void Debug_3DNR( ULONG prm0, ULONG prm1 );

#ifdef __cplusplus
}
#endif

#endif	// _CTRL_SHDR_H_

