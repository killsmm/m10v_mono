/*************************************************************************************\
*                                                                               *
*Copyright (C) CEVA Inc. All rights reserved                                         *
*                                                                               *
*                                                                               *
*THIS PRODUCT OR SOFTWARE IS MADE AVAILABLE EXCLUSIVELY TO LICENSEES THAT HAVE       *
*RECEIVED EXPRESS WRITTEN AUTHORIZATION FROM CEVA TO DOWNLOAD OR RECEIVE THE         *
*PRODUCT OR SOFTWARE AND HAVE AGREED TO THE END USER LICENSE AGREEMENT (EULA).       *
*IF YOU HAVE NOT RECEIVED SUCH EXPRESS AUTHORIZATION AND AGREED TO THE               *
*CEVA EULA, YOU MAY NOT DOWNLOAD, INSTALL OR USE THIS PRODUCT OR SOFTWARE.           *
*                                                                               *
*The information contained in this document is subject to change without notice and  *
*does not represent a commitment on any part of CEVA®, Inc. CEVA®, Inc. and its      *
*subsidiaries make no warranty of any kind with regard to this material, including,  *
*but not limited to implied warranties of merchantability and fitness for a          *
*particular purpose whether arising out of law, custom, conduct or otherwise.        *
*                                                                               *
*While the information contained herein is assumed to be accurate, CEVA®, Inc.       *
*assumes no responsibility for any errors or omissions contained herein, and         *
*assumes no liability for special, direct, indirect or consequential damage,         *
*losses, costs, charges, claims, demands, fees or expenses, of any nature or kind,   *
*which are incurred in connection with the furnishing, performance or use of this    *
*material.                                                                           *
*                                                                               *
*This document contains proprietary information, which is protected by U.S. and      *
*international copyright laws. All rights reserved. No part of this document may be  *
*reproduced, photocopied, or translated into another language without the prior      *
*written consent of CEVA®, Inc.                                                      *
*/

#ifndef _CEVA_CV_SMARTFRAME_COMMON_H_
#define _CEVA_CV_SMARTFRAME_COMMON_H_

/** 
* \addtogroup SmartFrame
*  @{
*/


/**
 * \brief Sequence handle
 */
typedef void* sf_seq_handle;

/**
 *	\brief Data types
 */
typedef	enum {  SMARTFRAME_DATA_TYPE_U8	 = 0x0001,		//!< Unsigned 8 bits data type
				SMARTFRAME_DATA_TYPE_S8	 = 0x0002,		//!< Signed 8 bits data type
				SMARTFRAME_DATA_TYPE_U16 = 0x0004,		//!< Unsigned 16 bits data type
				SMARTFRAME_DATA_TYPE_S16 = 0x0008,		//!< Signed 16 bits data type
				SMARTFRAME_DATA_TYPE_U32 = 0x0010,		//!< Unsigned 32 bits data type
				SMARTFRAME_DATA_TYPE_S32 = 0x0020		//!< Signed 32 bits data type
} sf_data_type_e;

/**
 * \brief Error status enumeration
 */
typedef	enum {  SMARTFRAME_SUCCEED						    = 0x0000,	//!< Function returned successfully
				SMARTFRAME_FAILED_INVALID_MEMSPACE		    = 0x0001,	//!< Memory space invalid
				SMARTFRAME_FAILED_INVALID_SEQ			    = 0x0002,	//!< Invalid sequence handle
				SMARTFRAME_FAILED_INVALID_KERNEL		    = 0x0004,	//!< Invalid kernel handle
				SMARTFRAME_FAILED_LINK					    = 0x0008,	//!< Failed to link
				SMARTFRAME_FAILED_DATA_TYPE					= 0x0010,	//!< Invalid data type
				SMARTFRAME_FAILED_NUM_DST_SRC				= 0x0020,	//!< Invalid number
				SMARTFRAME_FAILED_COUNT_OF_PADDING			= 0x0040,	//!< Invalid padding count
				SMARTFRAME_FAILED_LIMITED_DATA				= 0x0080,	//!< Ivalid limited data size
				SMARTFRAME_FAILED_TILE_SIZE					= 0x0100,	//!< Invalid tile size
				SMARTFRAME_FAILED_INVALID_NUM_OF_SET_PARAMS = 0x0200,	//!< Failed to set parameter
				SMARTFRAME_FAILED_INVALID_SIZE				= 0x0400,	//!< Invalid size
				SMARTFRAME_FAILED_NOT_VERIFIED				= 0x0800,	//!< Sequence should be verified first
				SMARTFRAME_FAILED_INVALID_NUM_OF_GET_PARAMS = 0x1000,	//!< Failed to get parameter
				SMARTFRAME_FAILED						    = 0xffff	//!< General failure
} sf_error_e;

/**
 * \brief Sequence parameter enumeration
 */
typedef enum {  SMARTFRAME_SEQ_PADD_TYPE	= 0,	//!< Padding type parameter
				SMARTFRAME_SEQ_PADD_VALUE	= 1,	//!< Padding value parameter
				SMARTFRAME_SEQ_PRIORITY		= 2		//!< Sequence priority parameter
} sf_param_seq_e;

/**
 * \brief Padding type
 */
typedef enum {  SMARTFRAME_PADDING_REPLICATE = 0, //!< aaaaaa|abcdefgh|hhhhhhh
				SMARTFRAME_PADDING_MIRROR    = 1, //!< fedcba|abcdefgh|hgfedcb
				SMARTFRAME_PADDING_CONSTANT  = 2  //!< iiiiii|abcdefgh|iiiiiii  with some specified 'i'
} sf_padding_type_e;


/**
* \brief Kernel index enumeration
*/
typedef enum {  SMARTFRAME_KERNEL_INDEX_0 = 0, //!< Index to kernel 0
				SMARTFRAME_KERNEL_INDEX_1 = 1, //!< Index to kernel 1
				SMARTFRAME_KERNEL_INDEX_2 = 2  //!< Index to kernel 2
} sf_kernel_index_e;


/**
 * \brief Kernel parameter enumeration
 */
typedef enum {  SMARTFRAME_PARAM_0	= 0,	//!< Index parameter 0
				SMARTFRAME_PARAM_1	= 1,	//!< Index parameter 1
				SMARTFRAME_PARAM_2	= 2,	//!< Index parameter 2
				SMARTFRAME_PARAM_3	= 3,	//!< Index parameter 3
				SMARTFRAME_PARAM_4	= 4,	//!< Index parameter 4
				SMARTFRAME_PARAM_5	= 5,	//!< Index parameter 5
				SMARTFRAME_PARAM_6	= 6,	//!< Index parameter 6
				SMARTFRAME_PARAM_7	= 7,	//!< Index parameter 7
				SMARTFRAME_PARAM_8	= 8,	//!< Index parameter 8
				SMARTFRAME_PARAM_9	= 9		//!< Index parameter 9
} sf_param_num_e;

/**
 * \brief Result conversion policy 
 */
typedef enum {  
	CEVA_CV_CONVERT_POLICY_TRUNC = 0, //!< Results are the least significant bits of the output operand, as if stored in two's complement binary format in the size of its bit-depth
	CEVA_CV_CONVERT_POLICY_CLIP  = 1  //!< Results are saturated to the bit depth of the output operand.

} sf_convert_policy_e;

/**
 * \brief Kernel plugin enumeration
 */
typedef enum {
    FEATURE_TABLE_IDX,
    GEOMETRIC_TABLE_IDX,
    IMAGE_TABLE_IDX,
    MATH_TABLE_IDX,
    MEMOPS_TABLE_IDX,
    OBJECT_TABLE_IDX,
    FORMAT_TABLE_IDX,
    OTHER_TABLE_IDX,
	USER_TABLE_IDX
} sf_plugin_e;


/**
 * \brief Kernel type enumeration
 */
typedef enum
{
	CEVA_CV_CORNERHARRIS3X3_S8_INDEX = (FEATURE_TABLE_IDX<<16),
	CEVA_CV_CORNERHARRIS3X3_S16_INDEX,
    CEVA_CV_CORNERHARRIS5X5_S8_INDEX,			
	CEVA_CV_CORNERHARRIS5X5_S16_INDEX,			
    CEVA_CV_CORNERHARRIS7X7_INDEX,			
	CEVA_CV_CORNERFAST9_INDEX,				
	CEVA_CV_CORNERFAST9THRESHOLD_INDEX,	
    CEVA_CV_CORNERFAST9MAXSUPPRESSION_INDEX,
    CEVA_CV_CORNERFAST9LISTRES_INDEX,
	CEVA_CV_CORNERFAST9MAXSUPPRESSION_2LIST_INDEX,
	CEVA_CV_CALCMINEIGENVAL_INDEX,			
	CEVA_CV_NONMAXSUPPRESSCARTL1_INDEX,		
	CEVA_CV_NONMAXSUPPRESSCARTL2_INDEX,		
	CEVA_CV_NONMAXSUPPRESS3X3_INDEX,		
	CEVA_CV_NONMAXSUPPRESS3X3X3_INDEX,
	CEVA_CV_NONMAXSUPPRESS3X3X3_2LIST_INDEX,
	CEVA_CV_LBP_INDEX,						
	CEVA_CV_CORNERMINEIGENVAL3X3_INDEX,	
	CEVA_CV_FEATURELISTCORNERCOLLECT_INDEX,
	CEVA_CV_KEYPOINTCOLLECT_INDEX,

	CEVA_CV_SCALEDOWNLINBY2_INDEX     = (GEOMETRIC_TABLE_IDX<<16),
	CEVA_CV_SCALEDOWNNEARBY2_INDEX,		
	CEVA_CV_SCALEDOWNNEARBY4_INDEX,		
	CEVA_CV_SCALEDOWNLINBY4_INDEX,			
	CEVA_CV_SCALEUPNEARBY2_INDEX,			
	CEVA_CV_SCALEUPLINBY2_INDEX,			
	CEVA_CV_SCALEDOWNGAUSSIAN3X3BY2_INDEX,
	CEVA_CV_SCALEDOWNGAUSSIAN5X5BY2_INDEX,	
	CEVA_CV_SCALEDOWNNEARBY2XAXIS_INDEX,	
	CEVA_CV_SCALEUPGAUSSIAN5X5BY2_INDEX,

	CEVA_CV_AVERAGEFILTER5X5_INDEX = (IMAGE_TABLE_IDX<<16),
	CEVA_CV_MEDIAN3X3_INDEX,	
	CEVA_CV_MEDIAN5X5_INDEX,	
	CEVA_CV_MEDIANSEP3X3_INDEX,	
	CEVA_CV_MEDIANSEP5X5_INDEX,
	CEVA_CV_RESERVED_001, // CEVA_CV_SIGMA_7X7_INDEX,
	CEVA_CV_SIGMAFILTER3X3_INDEX,
    CEVA_CV_SIGMAFILTER15X15_INDEX,
	CEVA_CV_FILTER15X15_INDEX,
	CEVA_CV_FILTER19X19_INDEX,
	CEVA_CV_SOBEL3X3DX_INDEX,				
	CEVA_CV_SOBEL3X3DY_INDEX,				
	CEVA_CV_SOBEL3X3DXDY_INDEX,			
	CEVA_CV_SOBEL3X3GRAD_INDEX,	
	CEVA_CV_SOBEL5X5DXDY_INDEX,	
	CEVA_CV_SOBEL7X7DXDY_INDEX,	
	CEVA_CV_DILATE2X2_INDEX,				
	CEVA_CV_DILATE3X3_INDEX,				
	CEVA_CV_ERODE3X3_INDEX,				
	CEVA_CV_FIR3X3_INDEX,					
	CEVA_CV_FIR1X3HOR_INDEX,				
	CEVA_CV_FIR1X3VER_INDEX,				
	CEVA_CV_INTEGRALLINE_HOR_INDEX,		
	CEVA_CV_INTEGRALLINE_VER_INDEX,		
	CEVA_CV_INTEGRALPATCH_INDEX,			
	CEVA_CV_MIN3X3_INDEX,					
	CEVA_CV_MAX3X3_INDEX,					
	CEVA_CV_MINMAX3X3_INDEX,				
	CEVA_CV_GAUSSIAN3X3_INDEX,				
	CEVA_CV_GAUSSIAN5X5_INDEX,
	CEVA_CV_GAUSSIAN5X5NONSEP_INDEX,
	CEVA_CV_GAUSSIAN5X5_SIGMA3_INDEX,				
	CEVA_CV_GAUSSIAN7X7_INDEX,				
	CEVA_CV_GAUSSIAN9X9_INDEX,				
	CEVA_CV_GAUSSIAN11X11_INDEX,			
	CEVA_CV_CORR3X3_INDEX,					
	CEVA_CV_CORRSEP9X9_INDEX,				
	CEVA_CV_CORRSEP11X11_INDEX,			
	CEVA_CV_CORRSEP13X13_INDEX,			
	CEVA_CV_CORRSEP15X15_INDEX,			
	CEVA_CV_CORRSEP17X17_INDEX,			
	CEVA_CV_HISTOGRAM_INDEX,	
    CEVA_CV_HISTOGRAM_MANIPULATE_INDEX,
	CEVA_CV_HISTOGRAMEQUALIZATIONLUT_INDEX,
	CEVA_CV_GRADIENT_2D_INDEX,				
	CEVA_CV_LAPLACIAN3X3_INDEX,			
	CEVA_CV_FILTER2D_INDEX,				
    CEVA_CV_BILATERALFILTER3X3_INDEX,		
	CEVA_CV_BILATERALFILTER5X5_INDEX,		
	CEVA_CV_RESERVED_000, // CEVA_CV_BILATERALFILTER_NXM_INDEX,
    CEVA_CV_BILATERALFILTERSEPARABLE3X3_INDEX,
    CEVA_CV_BILATERALFILTERSEPARABLE5X5_INDEX,
    CEVA_CV_BILATERALFILTERSEPARABLE7X7_INDEX,
    CEVA_CV_BILATERALFILTERSEPARABLE9X9_INDEX,
	CEVA_CV_BILATERALFILTERSEPARABLE15X15_INDEX,
	CEVA_CV_BILATERALFILTER_GENERIC_INDEX,
	CEVA_CV_BOXFILTER3x3_INDEX,
	CEVA_CV_NAGAOMATSUYAMAFILTER_INDEX,
	CEVA_CV_OPEN3X3_INDEX,
	CEVA_CV_CLOSE3X3_INDEX,
    CEVA_CV_MAGNITUDE_INDEX,
	CEVA_CV_PHASE_INDEX,

	CEVA_CV_ABS_INDEX     = (MATH_TABLE_IDX<<16),			
	CEVA_CV_ABSDIFF_INDEX,							
	CEVA_CV_ABSDIFFSCALAR_INDEX,		
	CEVA_CV_ADD_INDEX,					
	CEVA_CV_ADDSCALAR_INDEX,			
	CEVA_CV_SUBTRACT_INDEX,			
	CEVA_CV_SUBTRACTSCALAR_INDEX,		
	CEVA_CV_ADDWEIGHTED_INDEX,			
	CEVA_CV_ACCWEIGHTED_INDEX,
	CEVA_CV_MAX_INDEX,					
	CEVA_CV_MAXSCALAR_INDEX,			
	CEVA_CV_MIN_INDEX,					
	CEVA_CV_MINSCALAR_INDEX,			
	CEVA_CV_MINMAXLOC_INDEX,			
	CEVA_CV_MULTIPLY_INDEX,			
	CEVA_CV_MULTIPLYSCALAR_INDEX,		
	CEVA_CV_SUM_INDEX,	
    CEVA_CV_MEAN_AND_STANDARDDEVIATION_INDEX,
	CEVA_CV_BITWISEANDIMM_INDEX,			
	CEVA_CV_BITWISEAND_INDEX,			
	CEVA_CV_BITWISENOT_INDEX,			
	CEVA_CV_BITWISEORIMM_INDEX,			
	CEVA_CV_BITWISEOR_INDEX,			
	CEVA_CV_BITWISEXORIMM_INDEX,			
	CEVA_CV_BITWISEXOR_INDEX,			
	CEVA_CV_COUNTNONZERO_INDEX,		
	CEVA_CV_THRESHOLD_INDEX,			
	CEVA_CV_LOOKUPTABLE_INDEX,			
	CEVA_CV_EXP_MINUS_X_INDEX,			
	CEVA_CV_COUNTBITS_INDEX,			
	CEVA_CV_KMAXLOCVAL_INDEX,			
	CEVA_CV_KMINLOCVAL_INDEX,			
	CEVA_CV_KMAXLOCVALSORTED_INDEX,	
	CEVA_CV_KMINLOCVALSORTED_INDEX,
	CEVA_CV_SCALEADD_INDEX,

	CEVA_CV_COPYPATCH2D_INDEX = (MEMOPS_TABLE_IDX<<16),	
	CEVA_CV_FORMATCONVERT,	
	CEVA_CV_MERGE2TO1_INDEX,	
	CEVA_CV_SPLIT1TO2_INDEX,

	CEVA_CV_BW_CENTROID_INDEX = (OBJECT_TABLE_IDX<<16),	
	CEVA_CV_BW_SHRINK_INDEX,
	CEVA_CV_BW_PERIMETER8_INDEX,

	CEVA_CV_YUV2RGB_INDEX = (FORMAT_TABLE_IDX<<16),		
	CEVA_CV_RGB2YUV_INDEX,	
	CEVA_CV_RGB2YUVFR_INDEX,				
	CEVA_CV_RGB2YIQ_INDEX,
	CEVA_CV_RGB2Y_INDEX,					
	CEVA_CV_RGB2HSV_INDEX,					
	CEVA_CV_HSV2RGB_INDEX,					
	CEVA_CV_RGB2RGBA_INDEX,				
	CEVA_CV_RGBA2RGB_INDEX,				
	CEVA_CV_RGBA2YUV_INDEX,				
	CEVA_CV_YUV2RGBA_INDEX,				
	CEVA_CV_YUVFR2RGB_INDEX,				

	CEVA_CV_IMAGEWRAP_INDEX		  = (OTHER_TABLE_IDX<<16),
	CEVA_CV_HOUGHTRANSFORM_INDEX,
	CEVA_CV_IMAGERESIZE_INDEX,
	CEVA_CV_ORB_INDEX,
	CEVA_CV_ORB_DESCRIPTOR_EXTRACTOR_INDEX,
	OTHER_RESERVED_011, 	// CEVA_CV_CCL_INDEX,
	OTHER_RESERVED_000,		// IEA_SAT_COMP_INDEX,
	OTHER_RESERVED_001,		// IEA_DRC_INDEX,
	OTHER_RESERVED_002,		// IEA_HF_ANALYZE_INDEX,
	OTHER_RESERVED_003,		// IEA_SAT_CORRECTION_INDEX,
	OTHER_RESERVED_004,		// IEA_RGB_LEVELS_INDEX,
	OTHER_RESERVED_005,		// IEA_CHROMA_DENOISE_INDEX,
	OTHER_RESERVED_006,		// HOG_GRAD_INDEX,
	HOG_DESCRIPTOR_INDEX,
	OTHER_RESERVED_007,		// CEVA_SURF_INDEX,
	OTHER_RESERVED_008,		// CEVA_SURF_DESC_INDEX,
	OTHER_RESERVED_009,		// CEVA_SURF_MATCH_INDEX,
	KLT_INDEX,
	OTHER_RESERVED_010,		// LIN_SVM_INDEX,
	PRESPECTIVE_INDEX,
	PRESPECTIVE_NV12_INDEX,
	CANNY_INDEX,
	GOODFEAUTRES_INDEX,
	PYR_INDEX,
	DESCRIPTOR_INDEX,
	OTHER_RESERVED_012,		// MATCHER_INDEX,
	FFT_INDEX,
	IFFT_INDEX,
	RGB8882RGB_INDEX,
	YUYV2RGB_INDEX,
	USER_BASE_INDEX = (USER_TABLE_IDX<<16)
} sf_internal_kernel_e;


/** @}*/

#endif//_CEVA_CV_SMARTFRAME_COMMON_H_
