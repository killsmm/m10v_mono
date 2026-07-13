/**
 * @file		hdmi.h
 * @brief		HDMI I/F API
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _HDMI_H_
#define _HDMI_H_

#include "ddim_typedef.h"
#include "fj_disp.h"
#include "fj_std.h"

/*----------------------------------------------------------------------*/
/* Definition
 */
/*----------------------------------------------------------------------*/
#define D_HDMI_VIDEO_FORMAT_MAX (16)

//#define CO_HDMI_COMPLIANCE_TEST

/*----------------------------------------------------------------------*/
/* Enumeration
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure
 */
/*----------------------------------------------------------------------*/
/** AUD_Nx Register configuration */
typedef struct {
  USHORT sample32;      /**< Sampling Frequency 32kHz	*/
  USHORT sample32_chg;  /**< 1000/1001 value(32kHz)		*/
  USHORT sample48;      /**< Sampling Frequency 48kHz	*/
  USHORT sample48_chg;  /**< 1000/1001 value(48kHz)		*/
  USHORT sample96;      /**< Sampling Frequency 96kHz	*/
  USHORT sample96_chg;  /**< 1000/1001 value(96kHz)		*/
  USHORT sample441;     /**< Sampling Frequency 44.1kHz	*/
  USHORT sample441_chg; /**< 1000/1001 value(44.1kHz)	*/
} FJ_HDMI_N_VAL;

/*----------------------------------------------------------------------*/
/* Macro
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition
 */
/*----------------------------------------------------------------------*/

extern VOID fj_hdmi_init(HDMI_CALLBACK interrupt_callback);
#if 0
extern VOID fj_hdmi_close( VOID );
#endif
#if 0
extern FJ_HDMI_HPD fj_hdmi_detect( VOID );
#else
extern FJ_ERR_CODE fj_hdmi_detect(VOID);
#endif
extern FJ_ERR_CODE fj_hdmi_set_outformat(UCHAR format);
extern FJ_ERR_CODE fj_hdmi_connect_hdmi(UINT32 video_format);
extern FJ_ERR_CODE fj_hdmi_disconnect_hdmi(VOID);

extern VOID fj_hdmi_start_sync(VOID);
extern VOID fj_hdmi_stop_sync(VOID);

extern FJ_HDMI_HPD fj_hdmi_get_edid(FJ_HDMI_DTD_MAN **dtd_addr,
                                    UCHAR edid_data_block_tag_flags);
extern FJ_ERR_CODE fj_hdmi_get_dtd_no(UINT32 video_format, UINT32 *dtd_no);
extern FJ_ERR_CODE fj_hdmi_get_format_tbl_addr(UINT32 video_format,
                                               FJ_HDMI_EDID_DTD **edid_addr);
extern FJ_ERR_CODE fj_hdmi_get_system_status(FJ_HDMI_SYS_STAT *status);
extern FJ_ERR_CODE
fj_hdmi_set_ctrl_common(FJ_HDMI_CTRL_COMMON const *const ctrl);
extern FJ_ERR_CODE fj_hdmi_get_ctrl_common(FJ_HDMI_CTRL_COMMON *const ctrl);
extern FJ_ERR_CODE fj_hdmi_set_ctrl_video(FJ_HDMI_CTRL_VIDEO const *const ctrl);
extern FJ_ERR_CODE fj_hdmi_get_ctrl_video(FJ_HDMI_CTRL_VIDEO *const ctrl);
extern FJ_ERR_CODE fj_hdmi_set_ctrl_audio(FJ_HDMI_CTRL_AUDIO const *const ctrl);
extern FJ_ERR_CODE fj_hdmi_get_ctrl_audio(FJ_HDMI_CTRL_AUDIO *const ctrl);
extern FJ_ERR_CODE
fj_hdmi_set_ctrl_i2s(FJ_HDMI_CTRL_AUDIO_I2S const *const ctrl);
extern FJ_ERR_CODE fj_hdmi_get_ctrl_i2s(FJ_HDMI_CTRL_AUDIO_I2S *const ctrl);
#if 0
extern FJ_ERR_CODE fj_hdmi_avi_ctrl( USHORT clock, USHORT hsize, USHORT vsize, USHORT hblank, USHORT himgsize, UCHAR form );
#endif
extern FJ_ERR_CODE fj_hdmi_audio_ctrl(unsigned long samp_freq);
extern VOID fj_hdmi_audio_start(VOID);
extern VOID fj_hdmi_audio_stop(VOID);
extern VOID fj_hdmi_audio_set_start(ULONG sample_freq);
extern FJ_ERR_CODE fj_hdmi_audio_mute(BOOL enable);

#if 0
//cec
extern FJ_ERR_CODE fj_hdmi_cec_init(VOID);
extern VOID fj_hdmi_cec_stop(VOID);
#if 0
extern FJ_ERR_CODE fj_hdmi_cec_get_user_control_key_event(FJ_CEC_USER_CONTROL_KEY_EVENT * const keyEvent);
#endif
extern FJ_ERR_CODE fj_hdmi_cec_send_message(FJ_CEC_OPCODE opcode, BYTE operand[],BYTE operandCount);
extern FJ_ERR_CODE fj_hdmi_cec_inactive_source(VOID);
extern FJ_ERR_CODE fj_hdmi_cec_one_touch_play(VOID);
extern FJ_ERR_CODE fj_hdmi_cec_send_ping(USHORT *pingMap);
extern FJ_ERR_CODE fj_hdmi_cec_menu_status(FJ_CEC_MENUSTATE menuStatue);
#if 0
extern FJ_ERR_CODE fj_hdmi_cec_report_power_status(FJ_CEC_POWERSTATUS powerStatus);//delete
extern FJ_ERR_CODE fj_hdmi_cec_give_device_power_status(VOID);//delete
extern FJ_ERR_CODE fj_hdmi_cec_request_active_source(VOID);//delete
extern FJ_ERR_CODE fj_hdmi_cec_get_cec_version(VOID);//delete ->to BFW
extern FJ_ERR_CODE fj_hdmi_cec_menu_request(FJ_CEC_MENUREQ requestType);//delete
extern FJ_ERR_CODE fj_hdmi_cec_give_physical_addr(FJ_CEC_LOG_ADDR dest);//delete
#endif
extern FJ_ERR_CODE fj_hdmi_cec_give_device_vendor_id(VOID);
extern USHORT fj_hdmi_cec_get_physical_address(VOID);


VOID BF_HDMI_CEC_Receive_Simulate(FJ_CEC_CPIDATA message);
#endif
extern FJ_ERR_CODE fj_hdmi_mpeg_enable(BOOL enable);
extern BOOL fj_hdmi_get_mpeg_enable(VOID);
extern FJ_ERR_CODE
fj_hdmi_set_mpeg_infoframe(FJ_HDMI_MPEG_3D_STRUCTURE mpeg_3d_type);

extern VOID
BF_HDMI_Get_Vnder_ID(FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID *vendor_product_id);
extern ULONG BF_HDMI_Get_Short_Video_Type(VOID);
extern FJ_ERR_CODE
BF_HDMI_Get_Format_Tbl_Top_Addr(FJ_HDMI_EDID_DTD **edid_addr);
extern VOID BF_HDMI_Set_Phy_TXCTRL(USHORT regi_data);
extern VOID BF_HDMI_Set_Phy_VLEVCTRL(USHORT regi_data);
extern INT32 BF_HDMI_Configure_VGA_DVI_Video_Mode(VOID);

#endif // _HDMI_H_
