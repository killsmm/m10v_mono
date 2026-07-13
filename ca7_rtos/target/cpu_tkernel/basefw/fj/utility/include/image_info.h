/**
 * @file		image_info.h
 * @brief		Image Information (size and address) definition
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _IMAGE_INFO_H_
#define _IMAGE_INFO_H_

#include "ddim_typedef.h"

/**
@{
*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_WIDTH_14M				(4320)
#define D_LINES_14M				(3240)
#define D_PIX_SIZ_14M			(D_WIDTH_14M * D_LINES_14M)

#define D_WIDTH_12M				(4000)
#define D_LINES_12M				(3000)
#define D_PIX_SIZ_12M			(D_WIDTH_12M * D_LINES_12M)

#define D_WIDTH_12M_3_2			(4000)
#define D_LINES_12M_3_2			(2672)
#define D_PIX_SIZ_12M_3_2		(D_WIDTH_12M_3_2 * D_LINES_12M_3_2)

#define D_WIDTH_12M_16_9		(4000)
#define D_LINES_12M_16_9		(2248)
#define D_PIX_SIZ_12M_16_9		(D_WIDTH_12M_16_9 * D_LINES_12M_16_9)

#define D_WIDTH_10M				(3648)
#define D_LINES_10M				(2736)
#define D_PIX_SIZ_10M			(D_WIDTH_10M * D_LINES_10M)

#define D_WIDTH_10M_3_2			(3648)
#define D_LINES_10M_3_2			(2432)
#define D_PIX_SIZ_10M_3_2		(D_WIDTH_10M_3_2 * D_LINES_10M_3_2)

#define D_WIDTH_10M_16_9		(3648)
#define D_LINES_10M_16_9		(2056)
#define D_PIX_SIZ_10M_16_9		(D_WIDTH_10M_16_9 * D_LINES_10M_16_9)

#define D_WIDTH_8M				(3264)
#define D_LINES_8M				(2448)
#define D_PIX_SIZ_8M			(D_WIDTH_8M * D_LINES_8M)

#define D_WIDTH_8M_3_2			(3264)
#define D_LINES_8M_3_2			(2176)
#define D_PIX_SIZ_8M_3_2		(D_WIDTH_8M_3_2 * D_LINES_8M_3_2)

#define D_WIDTH_8M_16_9			(3264)
#define D_LINES_8M_16_9			(1836)
#define D_PIX_SIZ_8M_16_9		(D_WIDTH_8M_16_9 * D_LINES_8M_16_9)

#define D_WIDTH_7M				(3072)
#define D_LINES_7M				(2304)
#define D_PIX_SIZ_7M			(D_WIDTH_7M * D_LINES_7M)

#define D_WIDTH_6M				(2816)
#define D_LINES_6M				(2112)
#define D_PIX_SIZ_6M			(D_WIDTH_6M * D_LINES_6M)

#define D_WIDTH_5M				(2592)
#define D_LINES_5M				(1944)
#define D_PIX_SIZ_5M			(D_WIDTH_5M * D_LINES_5M)

#define D_WIDTH_4M				(2304)
#define D_LINES_4M				(1728)
#define D_PIX_SIZ_4M			(D_WIDTH_4M * D_LINES_4M)

#define D_WIDTH_3M				(2048)
#define D_LINES_3M				(1536)
#define D_PIX_SIZ_3M			(D_WIDTH_3M * D_LINES_3M)

#define D_WIDTH_3M_16_9			(1920)
#define D_LINES_3M_16_9			(1080)
#define D_PIX_SIZ_3M_16_9		(D_WIDTH_3M_16_9 * D_LINES_3M_16_9)

#define D_WIDTH_2M				(1600)
#define D_LINES_2M				(1200)
#define D_PIX_SIZ_2M			(D_WIDTH_2M * D_LINES_2M)

#define D_WIDTH_1_2M			(1280)
#define D_LINES_1_2M			(960)
#define D_PIX_SIZ_1_2M			(D_WIDTH_1_2M * D_LINES_1_2M)

#define D_WIDTH_1M				(1024)
#define D_LINES_1M				(768)
#define D_PIX_SIZ_1M			(D_WIDTH_1M * D_LINES_1M)

#define D_WIDTH_VGA				(640)
#define D_LINES_VGA				(480)
#define D_LINES_VGA_3_2			(426)
#define D_LINES_VGA_16_9		(360)
#define D_PIX_SIZ_VGA			(D_WIDTH_VGA * D_LINES_VGA)
#define D_PIX_SIZ_VGA_3_2		(D_WIDTH_VGA * D_LINES_VGA_3_2)
#define D_PIX_SIZ_VGA_16_9		(D_WIDTH_VGA * D_LINES_VGA_16_9)

#define D_WIDTH_HVGA			(640)
#define D_LINES_HVGA			(240)
#define D_PIX_SIZ_HVGA			(D_WIDTH_HVGA * D_LINES_HVGA)

#define D_WIDTH_QVGA			(320)
#define D_LINES_QVGA			(240)
#define D_LINES_QVGA_3_2		(213)
#define D_LINES_QVGA_16_9		(180)
#define D_PIX_SIZ_QVGA			(D_WIDTH_QVGA * D_LINES_QVGA)
#define D_PIX_SIZ_QVGA_3_2		(D_WIDTH_QVGA * D_LINES_QVGA_3_2)
#define D_PIX_SIZ_QVGA_16_9		(D_WIDTH_QVGA * D_LINES_QVGA_16_9)

#define D_WIDTH_THUMB			(160)
#define D_LINES_THUMB			(120)
#define D_LINES_THUMB_3_2		(106)
#define D_LINES_THUMB_16_9		(90)
#define D_PIX_SIZ_THUMB			(D_WIDTH_THUMB * D_LINES_THUMB)
#define D_PIX_SIZ_THUMB_3_2		(D_WIDTH_THUMB * D_LINES_THUMB_3_2)
#define D_PIX_SIZ_THUMB_16_9	(D_WIDTH_THUMB * D_LINES_THUMB_16_9)

#define D_WIDTH_PAL_VGA			(D_WIDTH_VGA)
#define D_LINES_PAL_VGA			(576)
#define D_PIX_SIZ_PAL_VGA		(D_WIDTH_PAL_VGA * D_LINES_PAL_VGA)

#define D_WIDTH_PAL_QVGA		(D_WIDTH_QVGA)
#define D_LINES_PAL_QVGA		(288)
#define D_PIX_SIZ_PAL_QVGA		(D_WIDTH_PAL_QVGA * D_LINES_PAL_QVGA)

#define	D_WIDTH_THM				(160)
#define	D_LINES_THM				(120)
#define D_PIX_SIZ_THM			(D_WIDTH_THM * D_LINES_THM)

#define	D_WIDTH_RGB_DELTA		(640)
#define	D_LINES_RGB_DELTA		(234)
#define D_PIX_SIZ_RGB_DELTA		(D_WIDTH_RGB_DELTA * D_LINES_RGB_DELTA)

#define	D_LINES_FILL_16_9		(306)
#define	D_LINES_FILL_3_2		(136)
#define	D_LINES_FILL_16_9_VGA	(60)
#define	D_LINES_FILL_3_2_VGA	(28)
#define	D_LINES_FILL_16_9_QVGA	(30)
#define	D_LINES_FILL_3_2_QVGA	(14)
#define	D_LINES_FILL_16_9_THUMB	(15)
#define	D_LINES_FILL_3_2_THUMB	(7)

// --- REMOVE_AHA BEGIN ---
#define D_WIDTH_AUTO_HORIZONTAL_ADJUSTMENT			(4416)
// --- REMOVE_AHA END ---

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** YCC image address */
typedef struct {
	ULONG	y;		/**< Y  address */
	ULONG	cb;		/**< Cb address */
	ULONG	cr;		/**< Cr address */
} T_IMAGE_ADDR_YCC;

/** RGB image address */
typedef struct {
	ULONG	r;		/**< R address */
	ULONG	g;		/**< G address */
	ULONG	b;		/**< B address */
} T_IMAGE_ADDR_RGB;

/** RGBA image address */
typedef struct {
	ULONG	y_g;	/**< Y(G)  address */
	ULONG	cb_b;	/**< Cb(B) address */
	ULONG	cr_r;	/**< Cr(R) address */
	ULONG	alpha;	/**< Alpha address */
} T_IMAGE_ADDR_RGBA;


/** Bayer image address */
typedef struct {
	ULONG	r;		/**< R  address */
	ULONG	gr;		/**< Gr address */
	ULONG	gb;		/**< Gb address */
	ULONG	b;		/**< B address */
} T_IMAGE_ADDR_BAYER;

/** image size */
typedef struct {
	USHORT	width;			/**< image width */
	USHORT	lines;			/**< image lines */
} T_IMAGE_SIZE;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

/*@}*/

#endif	/* _IMAGE_INFO_H_ */
