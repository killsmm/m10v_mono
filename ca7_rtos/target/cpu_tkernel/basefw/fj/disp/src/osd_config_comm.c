/**
 * @file		osd_config_comm.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "osd_config_comm.h"
#include "dd_top.h"
#include "disp.h"
#include "fj_custom.h" // enable BF_Debug_Print
#include "hdmi.h"
#include "interrupt.h"
#include "osd_attr.h"
#include "osd_config.h"
#ifdef CO_DEBUG_ON_PC
#include "jdsdisp3a.h"
#endif // CO_DEBUG_ON_PC
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition
 */
/*----------------------------------------------------------------------*/
#define D_BF_OSD_CONFIG_COMM_MAX_EXPONENT_FLOAT (127)
#define D_BF_OSD_CONFIG_COMM_SIGNIFICANT_PART_FLOAT (23)
#define D_BF_OSD_CONFIG_COMM_EXPONENT_BIT_NUM_FLOAT (8)

/*----------------------------------------------------------------------*/
/* Enumeration
 */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure
 */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data
 */
/*----------------------------------------------------------------------*/
static E_IM_DISP_SEL gDisp_sel = E_IM_DISP_LCD_MIPI;

/*----------------------------------------------------------------------*/
/* Macro
 */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function
 */
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"

/**
Get resize rate for LCD
@param[in]	layer		OSD layer selection.<br>
                                                <ul><li>@ref
E_IM_DISP_SEL_LAYER_MAIN <li>@ref E_IM_DISP_SEL_LAYER_OSD_0 <li>@ref
E_IM_DISP_SEL_LAYER_OSD_1</ul>
@param[in/out]	input_size	LCD's input size.
@param[out]	resize		Resize rate.
*/
static VOID osd_config_comm_get_resize_rate(E_IM_DISP_SEL_LAYER layer,
                                            U_IM_DISP_SIZE *input_size,
                                            T_IM_DISP_RESIZE *resize) {
  UINT32 hrszm = WIDTH_VGA;
  UINT32 hrszn = WIDTH_VGA;
  UINT32 vrszm = LINES_VGA;
  UINT32 vrszn = LINES_VGA;
  ULONG out_vsize;
  ULONG out_hsize;
  U_IM_DISP_DSTA position;

  Im_DISP_Get_Output_Size(gDisp_sel, &out_vsize, &out_hsize);
  if (out_vsize == 0 || out_hsize == 0) {
    // Output size error
    resize->rsz1.bit.HRSZM = 1;
    resize->rsz1.bit.HRSZN = 1;
    resize->rsz2.bit.VRSZN = 1;
    resize->rsz2.bit.VRSZM = 1;
    return;
  }

  // Get effective output size
  switch (layer) {
  case E_IM_DISP_SEL_LAYER_MAIN:
    Im_DISP_Get_Display_Position(gDisp_sel, &position);
    break;

  case E_IM_DISP_SEL_LAYER_OSD_0:
  case E_IM_DISP_SEL_LAYER_OSD_1:
    Im_DISP_Get_OSD_Territory_Position(gDisp_sel, layer, &position);
    break;

  default:
    position.bit.DSH = 0;
    position.bit.DSV = 0;
    break;
  }

  // Get Width M/N = output_size.size.width/input_size.size.width
  hrszm = out_hsize - (position.bit.DSH * 2); /* pgr0872 */
  hrszn = input_size->size.width;

  // Get Lines M/N = output_size.size.lines/input_size.size.lines
  vrszm = out_vsize - (position.bit.DSV * 2); /* pgr0872 */
  vrszn = input_size->size.lines;

  if ((double)hrszm / hrszn > 8) {
    hrszm = 8;
    hrszn = 1;
  }
  if ((double)hrszm / hrszn < 0.5) {
    hrszm = 1;
    hrszn = 2;
  }
  if ((double)vrszm / vrszn > 8) {
    vrszm = 8;
    vrszn = 1;
  }
  if ((double)vrszm / vrszn < 0.5) {
    vrszm = 1;
    vrszn = 2;
  }

  BF_Osd_General_Fraction(&hrszm, &hrszn);
  resize->rsz1.bit.HRSZM = hrszm;
  resize->rsz1.bit.HRSZN = hrszn;

  BF_Osd_General_Fraction(&vrszm, &vrszn);
  resize->rsz2.bit.VRSZN = vrszn;
  resize->rsz2.bit.VRSZM = vrszm;

  // display upper left of input image.
  if (out_hsize < input_size->size.width * hrszm / hrszn) {
    input_size->size.width = out_hsize * hrszn / hrszm;
  }
  if (out_vsize < input_size->size.lines * vrszm / vrszn) {
    input_size->size.lines = out_vsize * vrszn / vrszm;
  }
}

/**
Get display position
@param[in]	input_size	Input size.
@param[out]	position	Display position.
*/
static VOID
osd_config_comm_get_display_position(const U_IM_DISP_SIZE *input_size,
                                     U_IM_DISP_DSTA *position) {
  ULONG out_vsize, out_hsize;
  UINT32 hrszm, hrszn;
  UINT32 vrszm, vrszn;
  INT32 dsh, dsv;
  DOUBLE h_pitch, v_pitch;

  Im_DISP_Get_Output_Size(gDisp_sel, &out_vsize, &out_hsize);

  hrszm = out_hsize;
  hrszn = input_size->size.width;
  vrszm = out_vsize;
  vrszn = input_size->size.lines;

  h_pitch = (DOUBLE)hrszm / hrszn;
  v_pitch = (DOUBLE)vrszm / vrszn;
#if (_DLCD_MODE == D_LCD_DRV_AUO27_RGB_DELTA ||                                \
     _DLCD_MODE == D_LCD_DRV_CASIO_RGB_DELTA)
  if ((BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) ==
       E_FJ_DISP_IFS_LCD) ||
      (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) ==
       E_FJ_DISP_IFS_MIPI)) {
    // 960x480 -> 640x480.
    h_pitch = h_pitch * 2 / 3;
  }
#endif

  if ((double)hrszm / hrszn > 8) {
    hrszm = 8;
    hrszn = 1;
  }
  if ((double)hrszm / hrszn < 0.5) {
    hrszm = 1;
    hrszn = 2;
  }
  if ((double)vrszm / vrszn > 8) {
    vrszm = 8;
    vrszn = 1;
  }
  if ((double)vrszm / vrszn < 0.5) {
    vrszm = 1;
    vrszn = 2;
  }

  if (h_pitch < v_pitch) {
    dsh = (INT32)(out_hsize - (input_size->size.width * hrszm / hrszn)) / 2;
    dsv = (INT32)(out_vsize - (input_size->size.lines * vrszm / vrszn)) / 2;
#if (_DLCD_MODE == D_LCD_DRV_AUO27_RGB_DELTA ||                                \
     _DLCD_MODE == D_LCD_DRV_CASIO_RGB_DELTA)
    if ((BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) ==
         E_FJ_DISP_IFS_LCD) ||
        (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_LCD) ==
         E_FJ_DISP_IFS_MIPI)) {
      dsv =
          (out_vsize - (input_size->size.lines * (hrszm * 2 / 3) / hrszn)) / 2;
    }
#endif
    position->bit.DSH = (dsh > 0) ? dsh : 0;
    position->bit.DSV = (dsv > 0) ? dsv : 0;
  } else {
    dsh = (INT32)(out_hsize - (input_size->size.width * hrszm / hrszn)) / 2;
    dsv = (INT32)(out_vsize - (input_size->size.lines * vrszm / vrszn)) / 2;
    position->bit.DSH = (dsh > 0) ? dsh : 0;
    position->bit.DSV = (dsv > 0) ? dsv : 0;
  }
}

/**
grid line and face frame parameter reset.
*/
static VOID osd_config_comm_glid_and_face_disable(VOID) {
  // gridline disable and initialize.
  Im_DISP_Set_Grid_Enable(gDisp_sel,
                          E_IM_DISP_GDISPEN_NOT_DISPLAY); // Hide grid line
  T_IM_DISP_GRID grid_setting;
  Im_DISP_Get_Grid(gDisp_sel, &grid_setting);
  grid_setting.glength.bit.GHLEN = 8;
  grid_setting.glength.bit.GVLEN = 8;
  grid_setting.gwidth.bit.GHWID = 2;
  grid_setting.gwidth.bit.GVWID = 2;
  grid_setting.gitvl.bit.GHITV = 4;
  grid_setting.gitvl.bit.GVITV = 4;
  Im_DISP_Set_Grid(gDisp_sel, &grid_setting);

  // faceframe disable and initialize.
  Im_DISP_Set_Face_Frame_Enable(gDisp_sel, 0, 0); // Disable face frame
  T_IM_DISP_FACE_FRAME face_setting;
  USHORT i;
  Im_DISP_Get_Face_Frame(gDisp_sel, &face_setting);
  for (i = 0; i < D_IM_DISP_FACE_FRAME_COUNT; i++) {
    face_setting.param[i].ffsize.bit.FFHSZ = 8;
    face_setting.param[i].ffsize.bit.FFVSZ = 8;
    face_setting.param[i].ffwidth.bit.FFHWID = 2;
    face_setting.param[i].ffwidth.bit.FFVWID = 2;
  }
  Im_DISP_Set_Face_Frame(gDisp_sel, D_IM_DISP_FACE_FRAME_COUNT, &face_setting);
}

/*----------------------------------------------------------------------*/
/* Global Function
 */
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
FJ_ERR_CODE BF_Osd_Config_Comm_Set_Display_Parameter_LCD(VOID) {
  return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Osd_Config_Comm_Set_Display_Parameter_HDMI(
    const T_HDMI_SET *hdmi_setting_data) {
  return FJ_ERR_OK;
}

// MILB not used.
VOID BF_Osd_Config_Comm_Start_All_Layer(BOOL bb_enable) {
  U_IM_DISP_LALP lalp;
  UINT32 layer = (E_IM_DISP_SEL_LAYER_DCORE | E_IM_DISP_SEL_LAYER_OSD_ALL);

  if (gDisp_sel == E_IM_DISP_HDMI) {
    // Start HDMI.
    Im_DISP_Start(layer, E_IM_DISP_SEL_LAYER_NONE, 0);
  } else {
    // Start LCD.
    Im_DISP_Start(E_IM_DISP_SEL_LAYER_NONE, layer, 0);
  }

  // Set Blackback
  //	Im_DISP_Change_BB_Color_Enable( bb_enable );
  lalp.word = 0;

  if (bb_enable) {
    lalp.bit.ALP = 0;
  } else {
    lalp.bit.ALP = 0xFF;
  }

  Im_DISP_Set_Alpha_Value(gDisp_sel, lalp);
}

// MILB not used.
VOID BF_Osd_Config_Comm_Stop_Osd_Layer(VOID) {
  // Stop OSD layer of common bock
  if (gDisp_sel == E_IM_DISP_HDMI) {
    // Stop HDMI.
    Im_DISP_Stop(E_IM_DISP_SEL_LAYER_OSD_ALL, E_IM_DISP_SEL_LAYER_NONE, 0,
                 E_IM_DISP_STOP_TYPE_WITH_WAIT);
  } else {
    // Stop LCD.
    Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_OSD_ALL, 0,
                 E_IM_DISP_STOP_TYPE_WITH_WAIT);
  }
}

//#pragma arm section code="BASEFW_BOOT_CODE"
VOID BF_Osd_Config_Comm_Change_Main_Size(E_OSDLayer layer_id,
                                         const T_Rect *disp_rect) {
  U_IM_DISP_SIZE input_size;
  U_IM_DISP_DSTA display_position;
  ULONG y_global_width, c_global_width;
  T_IM_DISP_RESIZE resize = {.rsz0 = E_IM_DISP_RSZSL_PADDING_THINNING,
                             .rsz1 = {0x00000101},
                             .rsz2 = {0x00000101},
                             .rsz3 = {0x00000000}};
  U_IM_DISP_LALP lalp;

  // E_OSDLayer -> disp_sel
  if ((layer_id & E_OSDLayer_LCD) == E_OSDLayer_LCD) {
    gDisp_sel = E_IM_DISP_LCD_MIPI;
  } else {
    gDisp_sel = E_IM_DISP_HDMI;
  }

  //	Im_DISP_Change_BB_Color_Enable(TRUE);
  lalp.word = 0;
  lalp.bit.ALP = 0;
  Im_DISP_Set_Alpha_Value(gDisp_sel, lalp);

  // Set size parameter
  Im_DISP_Get_Input_Size(gDisp_sel, &input_size);
  input_size.size.width = disp_rect->Width;
  input_size.size.lines = disp_rect->Height;

  y_global_width = disp_rect->Width;
  c_global_width = disp_rect->Width;
  Im_DISP_Set_Lfd(gDisp_sel, y_global_width, c_global_width);
  gBF_Osd_Draw_Comm_Osd_Attr[gDisp_sel].hdmi.main.global_width = y_global_width;

  display_position.word = 0;
  //	display_position.bit.DSH = disp_rect->Postion.StartX;
  //	display_position.bit.DSV = disp_rect->Postion.StartY;
  osd_config_comm_get_display_position(&input_size, &display_position);
  Im_DISP_Set_Display_Position(gDisp_sel, display_position);

  osd_config_comm_get_resize_rate(E_IM_DISP_SEL_LAYER_MAIN, &input_size,
                                  &resize);
  Im_DISP_Set_Resize(gDisp_sel, &resize);
  Im_DISP_Set_Input_Size(gDisp_sel, input_size);
}

FJ_ERR_CODE BF_Osd_Config_Comm_Change_To_LCD(VOID) {
  // Start clock
  Dd_Top_Set_CLKSTOP3_LCDCK(E_BF_OSD_GENERAL_CLK_ON);

  // Display setting for LCD
#ifndef CO_BOARD_TYPE_CHOPES_EDB1
  if (BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_LCD) !=
      FJ_ERR_OK) {
    return FJ_ERR_NG;
  }
#else
  if (BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_LCD, E_FJ_DISP_IFS_MIPI) !=
      FJ_ERR_OK) {
    return FJ_ERR_NG;
  }
#endif
  Im_DISP_Init();
  if (BF_Osd_Config_Comm_Set_Display_Parameter_LCD() != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }
  return FJ_ERR_OK;
}

FJ_ERR_CODE
BF_Osd_Config_Comm_Change_To_HDMI(const T_HDMI_SET *hdmi_setting_data) {
  // Mode check
  FJ_MODE current_mod = FJ_Get_Act_Sysmode();
  BF_Osd_Draw_Comm_Get_Mode_Group(current_mod);
  if (gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.pGet_SDRAM_HDch ==
      NULL) {
    return FJ_ERR_NG;
  }

  // Set clock
  Dd_Top_Set_CLKSTOP3_HIFCK(E_BF_OSD_GENERAL_CLK_OFF); // Stop HIFCK
  BF_Osd_Config_Comm_Set_HDMI_Clock(
      (E_HD_RESOLUTION)hdmi_setting_data->HD_resolution);
  Dd_Top_Set_CLKSTOP3_HIFCK(E_BF_OSD_GENERAL_CLK_ON); // Start HIFCK

  // Display setting for HDMI
  if (BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE_HDMI,
                                        E_FJ_DISP_IFS_HDMI) != FJ_ERR_OK) {
    return FJ_ERR_NG;
  }
  Im_DISP_Init();
  if (BF_Osd_Config_Comm_Set_Display_Parameter_HDMI(hdmi_setting_data) !=
      FJ_ERR_OK) {
    return FJ_ERR_NG;
  }
  return FJ_ERR_OK;
}

VOID BF_Osd_Config_Comm_Change_To_OFF(FJ_DISP_TYPE disp_type) {

  if (disp_type == FJ_DISP_TYPE_HDMI) {
    gDisp_sel = E_IM_DISP_HDMI;
  } else {
    gDisp_sel = E_IM_DISP_LCD_MIPI;
  }

  //	FJ_DISP_TYPE disp_type;
  osd_config_comm_glid_and_face_disable();
#ifdef CO_DEBUG_ON_PC
  IO_DISP.MAIN[gDisp_sel].DCORE.TRG.word = 2;
#endif // CO_DEBUG_ON_PC

  if (disp_type == FJ_DISP_TYPE_HDMI) {
    // Stop HDMI.
    Im_DISP_Stop(E_IM_DISP_SEL_LAYER_ALL, E_IM_DISP_SEL_LAYER_NONE, 0,
                 E_IM_DISP_STOP_TYPE_WITH_WAIT);
    Im_DISP_Wait_Stop(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE);

    Dd_Top_Set_CLKSTOP3_LCDCK(E_BF_OSD_GENERAL_CLK_OFF);
    Dd_Top_Set_CLKSTOP3_HIFCK(E_BF_OSD_GENERAL_CLK_OFF);

    Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_DCORE, E_IM_DISP_SEL_LAYER_NONE,
                   D_IM_DISP_SR_RESET);
  } else {
    // Stop LCD.
    Im_DISP_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_ALL, 0,
                 E_IM_DISP_STOP_TYPE_WITH_WAIT);
    Im_DISP_Wait_Stop(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE);

    Dd_Top_Set_CLKSTOP3_LCDCK(E_BF_OSD_GENERAL_CLK_OFF);
    Dd_Top_Set_CLKSTOP3_HIFCK(E_BF_OSD_GENERAL_CLK_OFF);
    Dd_Top_Set_CLKSTOP3_MIFCK(E_BF_OSD_GENERAL_CLK_OFF);

    Im_DISP_Set_SR(E_IM_DISP_SEL_LAYER_NONE, E_IM_DISP_SEL_LAYER_DCORE,
                   D_IM_DISP_SR_RESET);
  }

  BF_Osd_Attr_Set_Display_Interface(disp_type, E_FJ_DISP_IFS_NONE);
}

VOID BF_Osd_Config_Comm_Start_HDMI_Layer(BOOL bb_enable, UINT32 layer) {
  if (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI) ==
      E_FJ_DISP_IFS_HDMI) {
    // Set Black Back
    BF_Osd_Config_Comm_Set_HDMI_Black_Back_Enable(bb_enable);
    // Start common block 0 layer
    Im_DISP_Start(layer, E_IM_DISP_SEL_LAYER_NONE, 0);
  }
}

VOID BF_Osd_Config_Comm_Set_HDMI_Black_Back_Enable(BOOL bb_enable) {
  U_IM_DISP_LALP lalp;

  // Set Black Back
  //	Im_DISP_Change_BB_Color_Enable( bb_enable );
  lalp.word = 0;

  if (bb_enable) {
    lalp.bit.ALP = 0;
  } else {
    lalp.bit.ALP = 0xFF;
  }

  Im_DISP_Set_Alpha_Value(E_IM_DISP_HDMI, lalp);
}

USHORT BF_Osd_Config_Comm_Conv_Decimal_To_2complement_Float(FLOAT decimalnum,
                                                            INT32 figures,
                                                            INT32 valid_bits) {
  UINT32 mantissa = 0;
  UINT32 intger_data = 1; // integer part
  UINT32 decimal_data;    // decimal part
  UINT32 out_put_data;
  BYTE barr[sizeof(FLOAT)];
  INT32 sign_flag;
  INT32 exponent;
  INT32 j;

  // Copy IEEE745 format to byte
  memcpy(barr, &decimalnum, sizeof(decimalnum));

  // Get sign bit
  sign_flag = (barr[0] >> 7);
  // Get exponent part
  exponent = (barr[0] & 0x7f);
  exponent <<= 8;
  exponent |= barr[1]; // barr[1]'s 4 bit
  exponent >>= 7;

  exponent -= D_BF_OSD_CONFIG_COMM_MAX_EXPONENT_FLOAT;

  // Get mantissa. In barr[1], it has 7 bits
  mantissa = (barr[1] & 0x7f); // In mantissa, default 1 bit integer
  for (j = 2; j < sizeof(barr); j++) {
    mantissa <<= 8;
    mantissa |= barr[j];
  }
  decimal_data = mantissa; // decimal part
  if (exponent > 0) {
    // Get integer data from mantissa because there are some bits was integer
    int shift_index = (D_BF_OSD_CONFIG_COMM_SIGNIFICANT_PART_FLOAT - 1);
    for (j = 0; j < exponent; j++) {
      intger_data <<= 1;
      intger_data |= ((mantissa >> shift_index) & 1);
      shift_index--;
    }
    // delete the 0 at top bits
    decimal_data <<=
        (exponent + (D_BF_OSD_CONFIG_COMM_EXPONENT_BIT_NUM_FLOAT + 1));
  } else if (exponent < 0) {
    // Should expand the decimal part
    intger_data = 0;
    decimal_data = (0x00800000 | mantissa) >> (exponent * (-1));
    decimal_data <<= (D_BF_OSD_CONFIG_COMM_EXPONENT_BIT_NUM_FLOAT + 1);
  } else {
    decimal_data <<= (D_BF_OSD_CONFIG_COMM_EXPONENT_BIT_NUM_FLOAT + 1);
  }

#if 0 // for debug : convert check
	{
		double test_decimal = intger_data;
		int shift_bit = 2;
		for (j = sizeof(barr) * 8 - 1; j >= 0; j--) {
			int OneBiit = (decimal_data >> j) & 1;
			if (OneBiit == 1) {
				test_decimal += ((flaot)1 / (flaot)shift_bit);
			}
			shift_bit <<= 1;

		}
		if (sign_flag == 1) {
			test_decimal *= (-1);
		}
		STD_printf("test_decimal=%f, DecimalNum=%f\n", test_decimal, decimalnum);
	}
#endif

  // rest "figures" bits decimal
  decimal_data >>= (sizeof(barr) * 8 - figures);
  out_put_data = (intger_data << figures);
  out_put_data |= decimal_data;
  if (sign_flag == 1) {
    // Get 2's complent
    out_put_data = (~(out_put_data)) + 1;
    out_put_data <<= (sizeof(barr) * 8 - (valid_bits + 1));
    out_put_data >>= (sizeof(barr) * 8 - (valid_bits + 1));
  }
  return (USHORT)out_put_data;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
VOID BF_Osd_Config_Comm_Set_HDMI_Clock(E_HD_RESOLUTION resolution) {
  switch (resolution) {
  case E_HD_RESOLUTION_720P_60HZ: // FALL THROUGH
  case E_HD_RESOLUTION_720P_50HZ: // FALL THROUGH
  case E_HD_RESOLUTION_1080I_60H: // FALL THROUGH
  case E_HD_RESOLUTION_1080I_50H: // FALL THROUGH
  case E_HD_RESOLUTION_1080P_30H: // FALL THROUGH
  case E_HD_RESOLUTION_1080P_25H: // FALL THROUGH
  case E_HD_RESOLUTION_720P_30HZ: // FALL THROUGH
  case E_HD_RESOLUTION_720P_25HZ:
    // Change HIFCLK to 74.25MHz
    Dd_Top_Set_CLKSEL2_HIFCLK(
        E_BF_OSD_GENERAL_CLK_SEL_74_25); // CLKSEL2.HIFCLK==0xB ==> 74.25MHz
                                         // (=1188MHz/16)
    break;

  case E_HD_RESOLUTION_480P_60HZ: // FALL THROUGH
  case E_HD_RESOLUTION_576P_50HZ:
    // Change HIFCLK to 27MHz
    Dd_Top_Set_CLKSEL2_HIFCLK(
        E_BF_OSD_GENERAL_CLK_SEL_27); // CLKSEL2.HIFCLK==0xC ==> 27MHz
                                      // (=1188MHz/44)
    break;

  case E_HD_RESOLUTION_1080P_60H: // FALL THROUGH
  case E_HD_RESOLUTION_1080P_50H:
    // Change HIFCLK to 148.5MHz
    Dd_Top_Set_CLKSEL2_HIFCLK(
        E_BF_OSD_GENERAL_CLK_SEL_148_5); // CLKSEL2.HIFCLK=0xA ==> 148.5MHz
                                         // (=1188MHz/8)
    break;

  case E_HD_RESOLUTION_2160P_25H: // FALL THROUGH
  case E_HD_RESOLUTION_2160P_30H:
    // Change HIFCLK to 297MHz
    Dd_Top_Set_CLKSEL2_HIFCLK(
        E_BF_OSD_GENERAL_CLK_SEL_297); // CLKSEL2.HIFCLK=0x9 ==> 297MHz
                                       // (=1188MHz/4)
    break;
  case E_HD_RESOLUTION_2160P_50H: // FALL THROUGH
  case E_HD_RESOLUTION_2160P_60H: // FALL THROUGH
    // Change HIFCLK to 594MHz
    Dd_Top_Set_CLKSEL2_HIFCLK(
        E_BF_OSD_GENERAL_CLK_SEL_594); // CLKSEL2.HIFCLK=0x8 ==> 594MHz
                                       // (=1188MHz/2)
    break;

  default:
    // DO NOTHING
    break;
  }
}

VOID BF_Osd_Config_Comm_Act_HDMI(UINT32 layer, BOOL enable) {
  if (enable == TRUE) {
    // Start.
    Im_DISP_Start(layer, E_IM_DISP_SEL_LAYER_NONE, 0);
  } else {
    // Stop.
    Im_DISP_Stop(layer, E_IM_DISP_SEL_LAYER_NONE, 0,
                 E_IM_DISP_STOP_TYPE_WITH_WAIT);
  }
}

// MILB not used.
FJ_ERR_CODE BF_Osd_Config_Comm_Check_Whether_Can_Set_Errcv(E_OSDLayer layer) {
  FJ_ERR_CODE ret = FJ_ERR_OK;
  BYTE ifs;

  // Get display interface selection
  if (Im_DISP_Get_Display_Interface(gDisp_sel, &ifs) == D_DDIM_OK) {
    if (layer & E_OSDLayer_LCD) {
      // Set LCD to automatically recover when transfer error occur
      if ((ifs != D_IM_DISP_IFS_LCD) && (ifs != D_IM_DISP_IFS_MIPI)) {
        ret = FJ_ERR_OSD_CH_STATE_ERROR;
      }
    }

    if (layer & E_OSDLayer_HDMI) {
      // Set VV/VOC to automatically recover when transfer error occur
      if ((ifs == D_IM_DISP_IFS_LCD) || (ifs == D_IM_DISP_IFS_MIPI)) {
        ret = FJ_ERR_OSD_CH_STATE_ERROR;
      }
    }
  } else {
    ret = FJ_ERR_OSD_CH_STATE_ERROR;
  }
  return ret;
}

E_HD_RESOLUTION BF_Osd_Config_Comm_Get_HDMI_Format(VOID) {
  return gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.format;
}

#ifdef CO_DEBUG_ON_PC
VOID BF_Osd_Config_Comm_Debug_Display_Parameter(VOID) {
  UINT32 bank = IO_DISP.MAIN[0].LCH.LIBCTL.bit.IBSET;
  BF_Debug_Print_Information(("------- Display Parameter(HDMI) -------\n"));
  BF_Debug_Print_Information(("- Bank%u Addr  = Y:%08lX C:%08lX\n", bank,
                              IO_DISP.MAIN[0].LCH.LYSA[bank].word,
                              IO_DISP.MAIN[0].LCH.LCSA[bank].word));
  BF_Debug_Print_Information(("- Global Size = Y:%lu C:%lu\n",
                              IO_DISP.MAIN[0].LCH.LYHGA.word,
                              IO_DISP.MAIN[0].LCH.LCHGA.word));
  BF_Debug_Print_Information(("- Input Size  = H:%lu V:%lu\n",
                              IO_DISP.MAIN[0].LCH.LISIZE.bit.IHSIZE,
                              IO_DISP.MAIN[0].LCH.LISIZE.bit.IVSIZE));
  BF_Debug_Print_Information(
      ("- Resize Rate = H:%d/%d V:%d/%d\n", IO_DISP.MAIN[0].LCH.LRSZ1.bit.HRSZM,
       IO_DISP.MAIN[0].LCH.LRSZ1.bit.HRSZN, IO_DISP.MAIN[0].LCH.LRSZ2.bit.VRSZM,
       IO_DISP.MAIN[0].LCH.LRSZ2.bit.VRSZN));
  BF_Debug_Print_Information(("- Device Size = H:%lu V:%lu\n",
                              IO_DISP.MAIN[0].DCORE.OHSIZE.word,
                              IO_DISP.MAIN[0].DCORE.OVSIZE.word));
  UINT32 iH = IO_DISP.MAIN[0].LCH.LISIZE.bit.IHSIZE *
              IO_DISP.MAIN[0].LCH.LRSZ1.bit.HRSZM /
              IO_DISP.MAIN[0].LCH.LRSZ1.bit.HRSZN;
  UINT32 iV = IO_DISP.MAIN[0].LCH.LISIZE.bit.IVSIZE *
              IO_DISP.MAIN[0].LCH.LRSZ2.bit.VRSZM /
              IO_DISP.MAIN[0].LCH.LRSZ2.bit.VRSZN;
  BF_Debug_Print_Information(("- Output Rect = X:%d Y:%d W:%d H:%d\n",
                              IO_DISP.MAIN[0].LCH.LDSTA.bit.DSH,
                              IO_DISP.MAIN[0].LCH.LDSTA.bit.DSV, iH, iV));

  bank = IO_DISP.MAIN[1].LCH.LIBCTL.bit.IBSET;
  BF_Debug_Print_Information(("------- Display Parameter(LCD) -------\n"));
  BF_Debug_Print_Information(("- Bank%u Addr  = Y:%08lX C:%08lX\n", bank,
                              IO_DISP.MAIN[1].LCH.LYSA[bank].word,
                              IO_DISP.MAIN[1].LCH.LCSA[bank].word));
  BF_Debug_Print_Information(("- Global Size = Y:%lu C:%lu\n",
                              IO_DISP.MAIN[1].LCH.LYHGA.word,
                              IO_DISP.MAIN[1].LCH.LCHGA.word));
  BF_Debug_Print_Information(("- Input Size  = H:%lu V:%lu\n",
                              IO_DISP.MAIN[1].LCH.LISIZE.bit.IHSIZE,
                              IO_DISP.MAIN[1].LCH.LISIZE.bit.IVSIZE));
  BF_Debug_Print_Information(
      ("- Resize Rate = H:%d/%d V:%d/%d\n", IO_DISP.MAIN[1].LCH.LRSZ1.bit.HRSZM,
       IO_DISP.MAIN[1].LCH.LRSZ1.bit.HRSZN, IO_DISP.MAIN[1].LCH.LRSZ2.bit.VRSZM,
       IO_DISP.MAIN[1].LCH.LRSZ2.bit.VRSZN));
  BF_Debug_Print_Information(("- Device Size = H:%lu V:%lu\n",
                              IO_DISP.MAIN[1].DCORE.OHSIZE.word,
                              IO_DISP.MAIN[1].DCORE.OVSIZE.word));
  iH = IO_DISP.MAIN[1].LCH.LISIZE.bit.IHSIZE *
       IO_DISP.MAIN[1].LCH.LRSZ1.bit.HRSZM /
       IO_DISP.MAIN[1].LCH.LRSZ1.bit.HRSZN;
  iV = IO_DISP.MAIN[1].LCH.LISIZE.bit.IVSIZE *
       IO_DISP.MAIN[1].LCH.LRSZ2.bit.VRSZM /
       IO_DISP.MAIN[1].LCH.LRSZ2.bit.VRSZN;
  BF_Debug_Print_Information(("- Output Rect = X:%d Y:%d W:%d H:%d\n",
                              IO_DISP.MAIN[1].LCH.LDSTA.bit.DSH,
                              IO_DISP.MAIN[1].LCH.LDSTA.bit.DSV, iH, iV));
}
#endif // CO_DEBUG_ON_PC
