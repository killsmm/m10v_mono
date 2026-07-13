/**
 * @file		ip_ahip.h
 * @brief		Header file of AHIP's wrapper layer
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IP_AHIP_H
#define	_IP_AHIP_H

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_ahip_overview
@{
	This chapter describes API related to AHIP's wrapper layer.
@}*//* --- end of ip_ahip_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_ahip_sequence
@{
	Please reference "Still Sequence(Video Loop)".
@}*//* --- end of ip_ahip_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_ahip_definition
@{*/
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define IP_AHIP_Open			IP_AHIP_OpenPlayback // Rename to avoid confusing

/** APIs return values.
*/
#define D_IP_AHIP_OK					(0)		/**< Success.								 		*/
#define D_IP_AHIP_NG					(-1)	/**< Unspecified error. 							*/
#define D_IP_AHIP_INVALID_ARG			(-2)	/**< One of the passed arguments is invalid.	 	*/
#define D_IP_AHIP_INVALID_MODE			(-3)	/**< The API cannot be used in this mode. 			*/
#define D_IP_AHIP_STOPPED				(-4)	/**< The operation was cancelled by application.	*/
#define D_IP_AHIP_BUSY					(-5)	/**< The HW block is busy. 							*/
#define D_IP_AHIP_HW_ERROR				(-6)	/**< A hardware error has occurred. 				*/
#define D_IP_AHIP_BUFFER_TOO_SMALL		(-7)	/**< The buffer size is too small. 					*/
#define D_IP_AHIP_NO_RESOURCES			(-8)	/**< Not enough resources. 							*/

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
/** Flag
*/
typedef enum {
	E_IP_AHIP_FLAG_PRE	= 1,					/**< Preview					*/
	E_IP_AHIP_FLAG_VIS,							/**< VIS						*/
} E_IP_AHIP_FLAG;

/** Preview mode
*/
typedef enum {
    E_IP_AHIP_PRV_MODE_SENSOR_A         = 0,          ///< Only sensor A is used.
    E_IP_AHIP_PRV_MODE_SENSOR_B         = 1,          ///< Only sensor B is used.
    E_IP_AHIP_PRV_MODE_SENSOR_AB_MERGED = 6,          ///< Input merged from sensors A and B.
    E_IP_AHIP_PRV_MODE_SENSOR_BA_MERGED = 7,          ///< Input merged from sensors B and A.
} E_IP_AHIP_PRV_MODE;

/** Image format
*/
typedef enum {
	E_IP_AHIP_FMT_YUV420P	= 0,				/**< YUV420 planar				*/
	E_IP_AHIP_FMT_YUV422P,						/**< YUV422 planar				*/
	E_IP_AHIP_FMT_YUV444P,						/**< YUV444 planar				*/
	E_IP_AHIP_FMT_YUV420SP,						/**< YUV420 semi-planar			*/
	E_IP_AHIP_FMT_YUV422SP,						/**< YUV422 semi-planar			*/
	E_IP_AHIP_FMT_YUV420B,						/**< YUV420 block semi-planar	*/
} E_IP_AHIP_FMT;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
/** Rectangle information
*/
typedef struct {
	UINT32			sta_x;					/**< position x of the rectangle		*/
	UINT32			sta_y;					/**< position y of the rectangle		*/
	UINT32			width;					/**< horizontal size of the rectangle	*/
	UINT32			lines;					/**< vertical size of the rectangle		*/
} T_IP_AHIP_RECT;

/** Size information
*/
typedef struct {
	UINT32			width;					/**< horizontal size.					*/
	UINT32			lines;					/**< vertical size.						*/
} T_IP_AHIP_SIZE;

/** Address of each component
*/
typedef struct {
	ULONG			y;						/**< Y  address							*/
	ULONG			cb;						/**< Cb address							*/
	ULONG			cr;						/**< Cr address							*/
} T_IP_AHIP_ADDR;

/** playback mode configration
*/
typedef struct {
	E_IP_AHIP_FMT		fmt;				/**< image format.						*/
	UINT32				g_y_width;			/**< global horizontal size	 for Y.<br>
												 In case of ::E_IP_AHIP_FMT_YUV420B format <br>
												 it must be aligned on 64, otherwise it must be aligned on 8.
											*/
	UINT32				g_c_width;			/**< global horizontal size	 for C.		*/
	T_IP_AHIP_SIZE		size;				/**< image size.						*/
	T_IP_AHIP_ADDR		addr;				/**< image address.<br>
												 In case of ::E_IP_AHIP_FMT_YUV420B format it must be aligned on 2.
											*/
} T_IP_AHIP_PLAYBACK_CFG;

/** preview mode configration
*/
typedef struct {
	E_IP_AHIP_FLAG		flags;				/**< flags.											*/
	E_IP_AHIP_FMT		fmt;				/**< image format.
												 Only ::E_IP_AHIP_FMT_YUV420P or ::E_IP_AHIP_FMT_YUV422P formats can be set.
											*/
	E_IP_AHIP_PRV_MODE  mode;				/**< preview sensor mode 							*/
	T_IP_AHIP_SIZE		size_a;				/**< image size a.									*/
	T_IP_AHIP_SIZE		size_b;				/**< image size a.									*/
} T_IP_AHIP_PREVIEW_CFG;

/*==============================================================================*/
/* Global variable declaration													*/
/*==============================================================================*/
// nothing special.

/*@} --- end of ip_ahip_definition */

/*==============================================================================*/
/* Function declaration															*/
/*==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/** @weakgroup ip_ahip_api
@{*/

/**
Opens the ahip driver (playback mode).
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@return		If the function succeeds, the return value is ::D_IP_AHIP_OK.
@remarks	The driver can not be opened more than once.<br>
			The application must call ::IP_AHIP_Close in order to close the driver before opening it again.<br>
*/
extern INT32 IP_AHIP_OpenPlayback( UCHAR demosaic_id );

/**
Opens the ahip driver (preview mode).
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@param[in]	cfg			: preview mode configration.<br>
@return		If the function succeeds, the return value is ::D_IP_AHIP_OK.
@remarks	The driver can not be opened more than once.<br>
			The application must call ::IP_AHIP_Close in order to close the driver before opening it again.<br>
			<br>
			Possible to specify multiple mode.<br>
			 @code
			 {
				T_IP_AHIP_PREVIEW_CFG cfg;
				
				IP_AHIP_OpenPreview( demosaic_id, &cfg );
			 }
			 @endcode

*/
extern INT32 IP_AHIP_OpenPreview( UCHAR demosaic_id, const T_IP_AHIP_PREVIEW_CFG* const cfg );


/**
Close the ahip driver.
@param[in]	demosaic_id	: Demosaic instance ID.
*/
extern VOID IP_AHIP_Close( UCHAR demosaic_id );

/**
Obtains the current value of a free running frame counter.
@param[in]	demosaic_id	: Demosaic instance ID.<br>
@return		The return value is a 32-bit counter value.
@remarks	This counter will increment on start of frame.
*/
extern UINT32 IP_AHIP_Get_Frame_No( UCHAR demosaic_id );

/**
Obtains the ahip driver opened.
@param[in]	demosaic_id	: Demosaic instance ID.
@return		TRUE: opened FALSE: not opened
*/
extern BOOL IP_AHIP_Get_Open_Status( UCHAR demosaic_id );

/**
Interrupt handler function.
@param[in]	demosaic_id	: Demosaic instance ID.
@remarks	Should be called from the HW interrupt context.
*/
extern VOID IP_AHIP_Int_Handler( UCHAR demosaic_id );

extern VOID IP_AHIP_LockSem( UCHAR demosaic_id );

extern VOID IP_AHIP_UnlockSem( UCHAR demosaic_id );

extern ULONG AHIP_User_Get_Time_us( VOID );

/*@} --- end of ip_ahip_api */

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IP_AHIP_H
