/**
 * @file		osd_config_comm.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_CONFIG_COMM_H_
#define _OSD_CONFIG_COMM_H_

#include "osd_draw_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#if FJ_OSD_COMM_RGBAXXXX == 4
#define D_BF_OSD_CONFIG_COMM_HGR_FMT		(0)			/**< RGBA8888 */
#else
#define D_BF_OSD_CONFIG_COMM_HGR_FMT		(0x2000)	/**< RGBA8888 */
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Horizontal padded setting of HGR */
typedef enum {
	E_BF_OSD_CONFIG_COMM_HDMI_GR_HRSZ_1080I		= 0x00000409,	/**< M/N=9/4 640=>1440	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_HRSZ_720P		= 0x00000203,	/**< M/N=3/2 640=>960	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_HRSZ_480P60HZ	= 0x00000809,	/**< M/N=9/8 640=>720	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_HRSZ_480P50HZ	= 0x00000809,	/**< M/N=9/8 640=>720	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_HRSZ_VGA		= 0x00000101,	/**< M/N=1/1 640		*/
} E_BF_OSD_CONFIG_COMM_HDMI_GR_HRSZ;

/** Vertical padded setting of HGR */
typedef enum {
	E_BF_OSD_CONFIG_COMM_HDMI_GR_VRSZ_1080I		= 0x00000409,	/**< M/N=9/4 480=>1080	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_VRSZ_720P		= 0x00000203,	/**< M/N=3/2 480=>720	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_VRSZ_480P60HZ	= 0x00000101,	/**< M/N=1/1 480		*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_VRSZ_480P50HZ	= 0x00000506,	/**< M/N=6/5 480=>576	*/
	E_BF_OSD_CONFIG_COMM_HDMI_GR_VRSZ_VGA		= 0x00000101,	/**< 480				*/
} E_BF_OSD_CONFIG_COMM_HDMI_GR_VRSZ;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Set display parameter for LCD.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
extern FJ_ERR_CODE BF_Osd_Config_Comm_Set_Display_Parameter_LCD(VOID);

/**
Set display parameter for HDMI.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
extern FJ_ERR_CODE BF_Osd_Config_Comm_Set_Display_Parameter_HDMI(const T_HDMI_SET* hdmi_setting_data);

/**
Start DCORE and OSD (Main:black back)
@param[in]	bb_enable TRUE:BB-ON, FALSE:BB-OFF
*/
extern VOID BF_Osd_Config_Comm_Start_All_Layer(BOOL bb_enable);

/**
Stop all layer
*/
extern VOID BF_Osd_Config_Comm_Stop_Osd_Layer(VOID);

/**
Change Main size.
@param[in]	display_rect display change rect data.
*/
extern VOID BF_Osd_Config_Comm_Change_Main_Size(E_OSDLayer layer_id, const T_Rect* display_rect);

/**
Change diplay to LCD.<br>
Function that controls switch to LCD.<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
extern FJ_ERR_CODE BF_Osd_Config_Comm_Change_To_LCD(VOID);

/**
Change diplay to HDMI.<br>
Function that controls switch to HDMI.<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
extern FJ_ERR_CODE BF_Osd_Config_Comm_Change_To_HDMI(const T_HDMI_SET* hdmi_setting_data);

/**
Turn off both D-LCD and HDMI output.
*/
extern VOID BF_Osd_Config_Comm_Change_To_OFF(FJ_DISP_TYPE disp_type);

/**
Start HD and HOC.
@param[in]	bb_enable	TRUE:black back ON, FALSE:black back OFF
@param[in]	layer		layer selection.<br>
						(Two or more layer can be specified by the OR value.)<br>
						<ul><li>@ref E_IM_DISP_SEL_LAYER_MAIN
							<li>@ref E_IM_DISP_SEL_LAYER_DCORE
							<li>@ref E_IM_DISP_SEL_LAYER_OSD_0
							<li>@ref E_IM_DISP_SEL_LAYER_OSD_1
							<li>@ref E_IM_DISP_SEL_LAYER_OSD_ALL
							<li>@ref E_IM_DISP_SEL_LAYER_ALL</ul>
*/
extern VOID BF_Osd_Config_Comm_Start_HDMI_Layer(BOOL bb_enable, UINT32 layer);

/**
Set HD's black back.
@param[in]	bb_enable	TRUE:black back ON, FALSE:black back OFF
*/
extern VOID BF_Osd_Config_Comm_Set_HDMI_Black_Back_Enable(BOOL bb_enable);

/**
Get the data of 2' complement from float data.
@param[in]	decimalnum	float type data
@param[in]	figures	valid decimal figures
@param[in]	vaild_bits totaly valid figures no inclue sign bit
@param[out]  None
@return      Convert result
@note        2' complement.
@attention   The float is IEEE745 format. There is a integer bit at mantissa part.
            31 30      22                     0
			+-+--------+---------------------+
            |s|exponent|significant part     |
			+-+--------+---------------------+
              [0   ]  [1   ] [2]
*/
extern unsigned short BF_Osd_Config_Comm_Conv_Decimal_To_2complement_Float(FLOAT decimalnum, INT32 figures, INT32 valid_bits );

/**
Set HDMI data for display.
@param[in]	hdmi_setting_data	HMDI data sent by EDID.
@param[out]	hif_main			Control data of HDch.
@param[out]	hif_osd				Control data of HOCch.
@retval		0: Progressive
@retval		1: Interace
*/
extern VOID BF_Osd_Config_Comm_Set_HDMI_Layer_Ctrl_Data(const T_HDMI_SET* hdmi_setting_data
												, T_IM_DISP_CTRL_MAIN_LAYER* hif_main
												, T_IM_DISP_CTRL_OSD_LAYER hgr_ctrl[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END]
												, T_IM_DISP_CTRL_OUTPUT* hdmi_output);

/**
Set HDMI's clock.
@param[in]	resolution	HMDI TV's resolution and frequence.
*/
extern VOID BF_Osd_Config_Comm_Set_HDMI_Clock(E_HD_RESOLUTION resolution);

/**
Set Enable/Disable each HIF Display Layer.
@param[in] layer_id The layer OS_USER_ID which will be enable to show or not.<br>
@param[in] enable TRUE mean enable to show and FALSE mean disable show.
*/
extern VOID BF_Osd_Config_Comm_Act_HDMI(UINT32 layer, BOOL enable);

/**
Check the LV/VV state before set automatic restoration when the forwarding failure is generated by the display macro.<br>
@param[in] layer			Target layer of automatic restoration.<br>
@retval FJ_ERR_OK					Normal End.
@retval FJ_ERR_OSD_CH_STATE_ERROR	Channel state error.
*/
extern FJ_ERR_CODE BF_Osd_Config_Comm_Check_Whether_Can_Set_Errcv(E_OSDLayer layer);

/**
Get HDMI Format.
@return See @ref E_HD_RESOLUTION.
@remarks Effective only during HDMI connection.<br>
*/
extern E_HD_RESOLUTION BF_Osd_Config_Comm_Get_HDMI_Format(VOID);

#ifdef DEBUG_ON_PC
/**
Print display parameter for debug.
*/
extern VOID BF_Osd_Config_Comm_Debug_Display_Parameter(VOID);
#endif // DEBUG_ON_PC

#endif // _OSD_CONFIG_COMM_H_
