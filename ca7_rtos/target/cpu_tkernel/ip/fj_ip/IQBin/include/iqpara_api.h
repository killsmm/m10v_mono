/**
 * @file		iqpara_api.h
 * @brief		None
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _IQPARA_API_H_
#define _IQPARA_API_H_

#include "ddim_typedef.h"
#include "im_pro.h"
#include "im_shdr.h"
/*----------------------------------------------------------------------*/
/* Definition
 */
/*----------------------------------------------------------------------*/

/*Error code*/
#define IQ_NoErr (0)
#define IQ_Err_Head (0xF123000)
#define IQ_Err_OpenBinFailure (IQ_Err_Head | 0x001)
#define IQ_Err_MemoryNotEnough (IQ_Err_Head | 0x002)
#define IQ_Err_InvalidHeader (IQ_Err_Head | 0x003)
#define IQ_Err_BinNotInit (IQ_Err_Head | 0x004)
#define IQ_Err_Invalid_StreamUnit (IQ_Err_Head | 0x005)

/*----------------------------------------------------------------------*/
/* Enumeration
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure
 */
/*----------------------------------------------------------------------*/

typedef struct {
  INT8 saturation_ctl;
  INT8 sharpness_ctl;
  INT8 noise_reduction;
  INT8 gamma_switch_ctl;
} T_IQ__CUSTOM_PARAM;

#define kScene_Max (8)
typedef struct _scene_2dtbl {
  UCHAR stream[kScene_Max];
  UCHAR maxstream;
  UCHAR comment[256];
} _T_Scene_2D_Tbl;

typedef struct {
  T_IM_SHDR_CTRL *shdr_ctrl;
  T_IM_SHDR_PRE *shdr_pre;
  T_IM_SHDR_FMD *shdr_fmd;
  T_IM_SHDR_SPNR *shdr_spnr;
  T_IM_SHDR_MSK *shdr_msk;
  T_IM_SHDR_PMSK *shdr_pmsk;
  UINT32 *shdr_in;
} T_IQ_3DNR_IM_ARG;

typedef struct {
  UCHAR EXSPRMODE;
  UCHAR EXSPRCNTL;
  UCHAR CNRMODE;
  UCHAR CNR_ONOFF;
} T_IQ_PARAM_CNR_CTRL;

/*----------------------------------------------------------------------*/
/* Global Data
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro
 */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function
 */
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*
API  for the  system initialization.
*/
INT32 IQPara_Init(ULONG *piqbin, ULONG binsize,
                  BOOL cap_mode); // pass the IQ binary buffer
VOID IQPara_Scene_Stream_Table(
    UCHAR *pscenetable,
    UCHAR max_stream); // When Mode Changed,  update the scene vs stream  table.
                       // #scene = Tbl[#stream]
VOID IQPara_Lens_Stream_Table(
    UCHAR *plenstable,
    UCHAR max_stream); // When Mode Changed, update the lens vs stream table.
/*
Call API  for mode switch or AE updating.
*/
INT32 IQPara_UpdateGain(FLOAT gain); // different stream with same gain;
INT32 IQPara_UpdateGainArray(
    FLOAT gain[],
    FLOAT delta); // different stream with different gain;  delta is fixed EV
                  // threshold  for each gain level, x1,x2,x4,8.

/*
Call API  for  WB gain update .   for WB gain dependent parameter.
*/
INT32 IQPara_UpdateRGBGain(FLOAT rgain, FLOAT ggain,
                           FLOAT bgain); // different stream with same AWB gain;
INT32 IQPara_UpdateRGBGainArray(
    FLOAT rgain[], FLOAT ggain[],
    FLOAT bgain[]); // different stream with differntRGB gain;

/*
Call API  for mode switch or Zoom position updating.
*/
INT32 IQPara_UpdateZoomPositionArray(
    UINT8 zoompos[]); // different stream with zoom position;
INT32 IQPara_UpdateZoomPosition(UINT8 zoompos);

/*
Call API  to update parameter for each HW unit  when Image pipeline is running.
Parameter:
UCHAR stream:  Stream#  for current pipeline
UCHAR ch:  LSI pipeline number 0~2 => pipe#0, #1,  #1&#2
*/
INT32 IQPara_SEN(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,	1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_SRO(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_B2B(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_B2R(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_R2Y(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_R2Y_Tbl(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_CNR(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_3DNR(
    UCHAR stream,
    T_IQ_3DNR_IM_ARG
        *im_arg); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
                  // channel): 0:ch#0,  1: ch#1, 3: ch#0 & ch #1.
INT32 IQPara_LTM(
    UCHAR stream,
    UCHAR ch); // strm(stream): 0~2,  0:main-stream; ch(LSI img pipeline
               // channel): 0:ch#0,	1: ch#1, 3: ch#0 & ch #1.

/*Tune Bin switching sequence
stop update,  IQPara_Tune_UpdateOnOff(0)
Load Tune-bin file  to somewhere ,  e.g. IQ1.bin,  by other API ,
Set the iqtune bin locaton ,  IQPara_Tune_SetTuneBin
Set the scene mapping,   IQPara_TuneSceneMapping
start the Tune bin,  IQPara_Tune_TuningBinMode
*/
INT32 IQPara_Tune_SetTuneBin(
    ULONG *piqbin,
    ULONG binsize); // set the tune bin location, set NULL/0 for clear
INT32 IQPara_Tune_UpdateOnOff(int isOn); //  pause the IQ update
INT32 IQPara_Tune_TuningBinMode(); // switch to Tuning Bin mode, ( the scene
                                   // register must be match)
INT32 IQPara_Tune_NormalBinMode(); // switch to normal bin mode.  use the
                                   // regular IQ binary
void IQPara_Tune_Reset();
void IQPara_Tune_SceneMapping(UCHAR targetscene, UCHAR mappingscene);
void IQPara_Tune_ForceGainNumber(float newgain); // <=0.0, disable
void IQPara_Tune_PrintState();

void IQPara_Get_CagCtrl(T_IM_PRO_CAG_CTRL *ctrl_0, T_IM_PRO_CAG_CTRL *ctrl_1);
void IQPara_Get_IQ_CNR_Ctrl(T_IQ_PARAM_CNR_CTRL *ctrl_0,
                            T_IQ_PARAM_CNR_CTRL *ctrl_1);
unsigned short *IQPara_Get_gamma(void);
unsigned char *IQPara_Get_rgamma(void);
/*
USAGE:
[Prepare IQ binary]
only need one IQ_0.bin file .

[System init]

Call
        IQPara_Init();   // if IQBin is store in Filesystem,  system need to
reserve the binary memory and read the whole iq binary

[Before Camera Mode Start/ Camera Mode Change]
Call
        IQPara_Scene_Stream_Table();  // System need update the scene stream
table .  *This table will be config in IQ tool . Get the control informaiton
(not implement yet).

[During Image pipeline is runing]
Call
        IQPara_UpdateGain() or IQPara_UpdateGainArray().      Update the current
ISO gain IQPara_xxx     Update the LSI register base on stream# and unit#.
*/

UINT32 IQPara_R2Y_GetUpdateToneTbl(void);

#ifdef __cplusplus
}
#endif
#endif //_IQPARA_API_H_
