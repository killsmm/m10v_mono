/**
 * @file		ct_dd_pdm.h
 * @brief		ct header for dd_pdm.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _CT_DD_PDM_H_
#define _CT_DD_PDM_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern VOID CT_Dd_Pdm_OpenTest1( UINT8 ch );
extern VOID CT_Dd_Pdm_OpenTest2( UINT8 ch );
extern VOID CT_Dd_Pdm_OpenTest3( UINT8 ch );
extern VOID CT_Dd_Pdm_OpenTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_OpenTest_e2( UINT8 ch );
extern VOID CT_Dd_Pdm_CloseTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Dma0_Int_HandlerTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Dma1_Int_HandlerTest( UINT8 ch );
extern VOID CT_Dd_Pdm_OverFlow_Int_HandlerTest( UINT8 ch );
extern VOID CT_Dd_Pdm_InitTest( VOID );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e2( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e3( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e4( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e5( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e6( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e7( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e8( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_CoreTest_e9( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_Ctrl_CoreTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_Ctrl_CoreTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Start_StreamingTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Stop_StreamingTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest_e2( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest_e3( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest_e4( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest_e5( UINT8 ch );
extern VOID CT_Dd_Pdm_Ctrl_DmaTest_e6( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_Ctrl_DmaTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_Ctrl_DmaTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_CallbackDma0IntrTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_CallbackDma1IntrTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_CallbackOverFlowIntrTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableDma0IntrTest1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableDma0IntrTest2( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableDma0IntrTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableDma1IntrTest1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableDma1IntrTest2( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableDma1IntrTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableOverFlowIntrTest1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableOverFlowIntrTest2( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_EnableOverFlowIntrTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_DMA0_Dst_AddrTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_DMA1_Dst_AddrTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_Dma_TransLengthTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e2( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e3( UINT8 ch );
extern VOID CT_Dd_Pdm_Set_Dma_TransLengthTest_e4( UINT8 ch );
extern VOID CT_Dd_Pdm_Flush_Dma_FifoTest( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_StatusTest1( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_StatusTest2( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_StatusTest3( UINT8 ch );
extern VOID CT_Dd_Pdm_Get_StatusTest_e1( UINT8 ch );
extern VOID CT_Dd_Pdm_Clear_StatusTest1( UINT8 ch );
extern VOID CT_Dd_Pdm_Clear_StatusTest2( UINT8 ch );
extern VOID CT_Dd_Pdm_Clear_StatusTest3( UINT8 ch );
extern VOID CT_Dd_PDM_3_Reg_RW( VOID );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
extern VOID CT_Dd_PDM_3_001(VOID);
extern VOID CT_Dd_PDM_3_002(VOID);
extern VOID CT_Dd_PDM_3_003(VOID);
extern VOID CT_Dd_PDM_3_004(VOID);
extern VOID CT_Dd_PDM_3_005(VOID);
extern VOID CT_Dd_PDM_3_006(VOID);
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
extern VOID CT_Dd_PDM_3_007(VOID);
extern VOID CT_Dd_PDM_3_008(VOID);
extern VOID CT_Dd_PDM_3_009(VOID);
extern VOID CT_Dd_PDM_3_010(VOID);
extern VOID CT_Dd_PDM_3_011(VOID);
extern VOID CT_Dd_PDM_3_012(VOID);
extern VOID CT_Dd_PDM_3_013(VOID);
extern VOID CT_Dd_PDM_3_014(VOID);
extern VOID CT_Dd_PDM_3_015(VOID);
extern VOID CT_Dd_PDM_3_016(VOID);
extern VOID CT_Dd_PDM_3_017(VOID);
extern VOID CT_Dd_PDM_3_018(VOID);
extern VOID CT_Dd_PDM_3_019(VOID);
extern VOID CT_Dd_PDM_3_020(VOID);
extern VOID CT_Dd_PDM_3_021(VOID);
extern VOID CT_Dd_PDM_3_022(VOID);
extern VOID CT_Dd_PDM_3_023(VOID);
extern VOID CT_Dd_PDM_3_024(VOID);
extern VOID CT_Dd_PDM_3_025(VOID);
extern VOID CT_Dd_PDM_3_026(VOID);
extern VOID CT_Dd_PDM_3_027(VOID);
extern VOID CT_Dd_PDM_3_028(VOID);
extern VOID CT_Dd_PDM_3_029(VOID);
extern VOID CT_Dd_PDM_3_030(VOID);
extern VOID CT_Dd_PDM_3_031(VOID);
extern VOID CT_Dd_PDM_3_032(VOID);
extern VOID CT_Dd_PDM_3_033(VOID);
extern VOID CT_Dd_PDM_3_034(VOID);
extern VOID CT_Dd_PDM_3_035(VOID);
extern VOID CT_Dd_PDM_3_036(VOID);
extern VOID CT_Dd_PDM_3_037(VOID);
extern VOID CT_Dd_PDM_3_038(VOID);
extern VOID CT_Dd_PDM_3_039(VOID);
extern VOID CT_Dd_PDM_3_040(VOID);
extern VOID CT_Dd_PDM_3_041(VOID);
extern VOID CT_Dd_PDM_3_042(VOID);
extern VOID CT_Dd_PDM_3_043(VOID);
extern VOID CT_Dd_PDM_3_044(VOID);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern VOID Ct_Dd_Pdm_Main(int argc, char** argv);

#endif	// _CT_DD_PDM_H_
