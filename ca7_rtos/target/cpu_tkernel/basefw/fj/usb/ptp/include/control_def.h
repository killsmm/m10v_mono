/**
 * @file		control_def.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __system_def_H__
#define __system_def_H__

//SCENE MODE Definition

typedef ULONG _SceneMode;
enum
{
	eAutoMode,
	ePMode,
	ePortraitMode,
	eLandscapeMode,
	eSportsMode,
	eNightLandscapeMode,
	eNightPortraitMode,
	eMuseumMode,
	eSnowMode,
	eSunsetMode,
	eFireworkMode,
	eCopyMode,
	eBacklightMode,
	eAudioMode,
	eVideoMode,
	eSceneModeUnknown,
} ;


/***************************Draft Setting Definition START************************************/
// Strobe mode definition.
typedef ULONG _StrobeMode;
enum 
{
	eStrobeAuto,			// Strobe auto mode
	eStrobeOff,				// Force strobe off mode
	eStrobeOn,				// Fill flash
	eStrobeAutoRedEyes,		// Strobe auto mode with red eyes function
	eStrobeUnknown,
} ;

//Close up on/off
typedef ULONG _MacroFlag;
enum
{
	eMacroOff,
	eMacroOn,
} ;

//Burst mode setting
typedef ULONG _BurstMode;
#ifndef _TOPEIGHT_
enum
{
	eBurstOff,
	e10Sec,
	e2Sec,
	eBurstOn,
};
#else /* TopEight */
enum
{
	eBurstOff,
	eBurstOn,
};
typedef ULONG _SelfTimerMode;
enum
{
	eSelfTimerOff,
	e5Sec,
	e10Sec,
	e15Sec,
	e2Sec,
};
#endif
typedef ULONG _IconFlag;
enum
{
	eIconOn,
	eIconOff,
} ;


// ISO setting definition.
typedef ULONG _ISO_Value;
enum
{
	eISOAuto,				// ISO Auto mode.
	eISO100,				// ISO	set to 100.
	eISO200,				// ISO	set to 200.
	eISO400,				// ISO	set to 400.
	eISOUnknown,
} ;

// Picture resoution definition.
typedef ULONG _Image_Resolution;
enum
{
#ifdef _TOPEIGHT_ /* Andy Jing 070509 */
	e3648x2736,			// 3648 * 2736, Around 10M pixels.
	e3264x2448,			// 3264 * 2448, Around  8M pixels.
#endif
	e3072x2304,			// 3680 * 2760, Around 7M pixels.
	e3072x2048,			// 3264 * 2448, Around 7M(3:2) pixels.
	e2816x2112,			// 2816 * 2112, Around 6M pixels.
	e2832x1888,			// 2832 * 1888, Around 6M(3:2) pixels.
	e2576x1932,			// 2576 * 1932, Around 5M pixels.
	e2304x1728,			// 2304 * 1728, Around 4M pixels.
	e2304x1536,			// 2304 * 1536, Around 4M(3:2) pixels.
	e2048x1536,			// 2048 * 1536, Around 3M pixels.
	e1600x1200,			// 1600 * 1704, Around 2M pixels.
	e1584x1056,			// 1584 * 1056, Around 2M(3:2) pixels.
	e1280x960,			// 1280 * 960,  Around 1M pxiels.
	e1296x864,			// 1296 * 864,  Around 1M(3:2) pxiels.
	eVGA,				// 640 * 480, Around 300K pixels.
	e639x426,			// 639 * 426, Around 300K(3:2) pixels.
	eQVGA,
	eImgResUnknown,
} ;

// Video size definition.
typedef ULONG _Video_Size;
enum
{
	eVGA_movie,			// 640x480
	eQVGA_movie,		// 320x240
} ;


typedef ULONG _Video_Quality;
enum
{
	eMovie_Good,	
	eMovie_Best,		
};

typedef ULONG _Video_FrameRate;
enum
{
	eMovie_15fps,
	eMovie_30fps,
	eMovie_10fps,
	eMovie_4fps,
};

typedef ULONG _Video_AudioOnoff;
enum
{
	eMovie_AudioOff,
	eMovie_AudioOn,
};

// JPEG quality definition.
typedef ULONG _JPEG_QULAITY;
enum
{
	eJpegBest,				// Best JPEG quality, around 1/4.
	eJpegBetter,				// Better JPEG quality, around 1/8.
	eJpegGood,				// Good JPEG quality, around 1/16.A
	eJpegUnknown,
} ;

// EV setting definition.
typedef ULONG _EV_Setting;
enum
{
	eEV00,					// EV	0.
	eEV13,					// EV +1/3
	eEV23,					// EV +2/3
	eEV33,					// EV +3/3
	eEV43,					// EV +4/3
	eEV53,					// EV +5/3
	eEV63,					// EV +6/3
	eEVn13,					// EV -1/3
	eEVn23,					// EV -2/3
	eEVn33,					// EV -3/3
	eEVn43,					// EV -4/3
	eEVn53,					// EV -5/3
	eEVn63,					// EV -6/3
	eEVUnknown,
} ;

typedef ULONG _WB_Mode;
enum
{
	eWBAuto,			// MWB off mode, the same as AWB auto mode.
	eWBSunny,		// MWB for sunny.
	eWBTungsten,	// MWB for Tungsten.
	eWBFluor,		// MWB for fluorescent.
	eWBCloudy,		// MWB for cloudy mode.
	eWBCustom,		// MWB Internal IQ
	eWBUnknown,
} ;		// MWB setting mode.

//AE metering mode definition
typedef ULONG _AE_Mode;
enum
{
	eAE_Center,
	eAE_Spot,
	eAE_Average,
	eAE_Unknown,
} ;

//focus mode definition
typedef ULONG _Focus_Mode;
enum
{
	eSelected,
	eMultizone,
	eCenter,
	eHyper,
} ;

//AF position definition
typedef ULONG _AF_Position;
enum
{
	eAFCenter,
	eAFTopLeft,
	eAFTop,
	eAFTopRight,
	eAFLeft,
	eAFRight,
	eAFBottomLeft,
	eAFBottom,
	eAFBottomRight,
} ;


//Color mode definition
typedef ULONG _Color_Mode;
enum
{
	eColorEffect,
	eVividEffect,
	eBlackWhiteEffect,
	eSepiaEffect,
	eUnknownEffect,
} ;	

//Sharpness setting definition
typedef ULONG _Sharpness_level;
enum
{
	eSharpness_Medium,
	eSharpness_High,
	eSharpness_Low,
	eSharpness_Unknown,
} ;

//Saturation setting definition
typedef ULONG _Saturation_level;
enum
{
	eSaturation_Medium,
	eSaturation_High,
	eSaturation_Low,
	eSaturation_Unknown,
};

//Contrast setting definition
typedef ULONG _Contrast_level;
enum
{
	eContrast_Medium,
	eContrast_High,
	eContrast_Low,
	eContrast_Unknown,
};


////////////////////////////////////
/***************************Draft Defintion Setting END************************************/

//Andy Setup UI///////////////////////
typedef ULONG _Freq;
enum
{
	eFreq_50,
	eFreq_60,
} ;

typedef ULONG _FileNo;
enum
{
	eFileNoContinuous,
	eFileNoReset,
} ;


typedef ULONG _DigitalZoomFlag;
enum
{
	eDigitalZoomOff,
	eDigitalZoomOn,
} ;

typedef ULONG _QuickView;
enum
{
	eQuickView_On,
	eQuickView_Off,
} ;

typedef ULONG _ImageStorage;
enum
{
	eImageStorage_Auto,
	eImageStorage_Internal,
} ;

typedef ULONG _DateStamp;
enum
{
	eDateStamp_Off,
	eDateStamp_YMD,
	eDateStamp_MDY,
	eDateStamp_DMY,
} ;

typedef ULONG _DateTime;
enum
{
	eDateTime_Off,
	eDateTime_On,
} ;
typedef ULONG _CameraSound;
enum
{
	eCameraSound_Low,
	eCameraSound_Off,
	eCameraSound_High,
} ;

typedef ULONG _VideoOut;
enum
{
	eVideoOut_NTSC,
	eVideoOut_PAL,
} ;
 
typedef ULONG _LCDOff;
enum
{
eLCDOff_Off,
eLCDOff_10s,
eLCDOff_30s,
eLCDOff_1min,
} ;
 
typedef ULONG _AutoPowerOff;
#ifndef _TOPEIGHT_
enum
{
/* org
eAutoPowerOff_Off,
eAutoPowerOff_On,
*/
eAutoPowerOff_On_60,
eAutoPowerOff_On_120,
eAutoPowerOff_On_180,
eAutoPowerOff_Off,
} ;
#else
enum
{
eAutoPowerOff_On_60,
eAutoPowerOff_On_180,
eAutoPowerOff_On_300,
eAutoPowerOff_Off,
} ;
#endif 
typedef ULONG _Language;
enum
{
 eEnglish,
 eGerman,
 eFrench,
 eItalian,
 eSpanish,
 eDutch,
 ePortuguese,
 eSChinese,
 eTChinese,
 eJapanese,
 eRussian,
 eArabic,
} ;


typedef ULONG _VoiceMemo;
enum
{
eVoiceMemo_Off,
eVoiceMemo_On,
};

//Andy Setup UI///////////////////////

typedef ULONG _InfoFlag;
enum
{
	eInfoOff,
	eInfoOn,
} ;

typedef ULONG _SlideShowLoopFlag;
enum
{
	eLoopOff,
	eLoopOn,
};

typedef ULONG _OpenScreen;
enum
{
	eScreenFactory,
	eScreenUser1,
	eScreenUser2,
	eScreenUser3,
	eScreenUser4,
};

//Capture mode status structure
typedef struct
{
	_Freq				freq;
	_ISO_Value			isoValue;
	_Image_Resolution	imageResolution;
	_JPEG_QULAITY		jpegQuality;
	_EV_Setting			evSetting;
	_WB_Mode			wbMode;
	_AE_Mode			aeMode;
	_AF_Position		afPosition;
	_Focus_Mode			focusMode;
	_Color_Mode			colorMode;
	_Sharpness_level	sharpnessLevel;
	_Saturation_level	saturationLevel;
	_Contrast_level		contrastLevel;
	_DigitalZoomFlag	digitalZoomFlag;
	_StrobeMode			strobeMode;			// To indicate camera strobe mode setting
	_MacroFlag			macroFlag;
	_BurstMode			burstMode;
} CaptureControl;

typedef struct
{
	_Video_Size			videoSize;
	_Video_Quality		videoQuality;
	_Video_FrameRate videoFrameRate;
	_Video_AudioOnoff videoAudioOnoff;
} VideoUsrPara;

#endif
