/*======================================================================*/
/*	Filename	: WaveDetection.h										*/
/*	Target		: M3 Series												*/
/*======================================================================*/

#ifndef _WAVEDETECTION_H
#define _WAVEDETECTION_H

#define MLNA_WD_DRAFT	0
#define MLNA_WD_STILL	1

#define MLNA_WD_RGR		(0)
#define MLNA_WD_BGB		(1)

typedef struct {
	unsigned long long Y_Result;
	unsigned short Ct;
} MLNA_WD_AE_Result;

typedef struct {
	unsigned long R_Result;
	unsigned long G_Result;
	unsigned long B_Result;
	unsigned long GateCt;
	float LightVolume;
	unsigned long Ct;
} MLNA_WD_AWB_Result;

/*=====================< Function Prototype >==========================*/
//extern unsigned short MLNA_WD_Main(unsigned char inst_id, float latestE);
extern unsigned short MLNA_WD_Main(unsigned char inst_id, float latestE, unsigned char multi_wd_mode );
extern unsigned short MLNA_WD_OB_ForStill(unsigned char inst_id, signed short offset,unsigned short scale);
extern void MLNA_WD_Init(unsigned char inst_id, unsigned long Work_Address);
extern void MLNA_WD_Set_OB_Offset(unsigned char inst_id, signed short offset);
extern signed short MLNA_WD_Get_OB_Offset(unsigned char inst_id);
extern void MLNA_WD_Set_OB_Scale(unsigned char inst_id, unsigned short scale);
extern unsigned short MLNA_WD_Get_OB_Scale(unsigned char inst_id);

extern void MLNA_WD_ChgMode(unsigned char inst_id, unsigned char DraftOrStill);
extern void MLNA_WD_Set_AE_Ref_Addr(
		unsigned char inst_id,
		unsigned long JDSTFR_AEDATA_ForwardAddr,
		unsigned long WD_AE_ResultAddr);
extern void MLNA_WD_Set_AWB_Ref_Addr(
		unsigned char inst_id,
		unsigned long JDSTFR_AWBDATA_ForwardAddr,
		unsigned long WD_AWB_ResultAddr);
extern void MLNA_WD_Set_DelayFrame(unsigned char inst_id, unsigned short DelayFrame);
extern void MLNA_WD_Set_OnOff(unsigned char inst_id, unsigned short kind,unsigned short OnOff);
extern unsigned short MLNA_WD_Get_OnOff(unsigned char inst_id);
extern void MLNA_WD_Set_AE_BW_Addr(unsigned char inst_id, unsigned long AE_BlockWeight_Addr);
extern void MLNA_WD_Set_AWB_BW_Addr(unsigned char inst_id, unsigned long AWB_BlockWeight_Addr);
extern void MLNA_WD_Set_BlockWeight(
		unsigned char inst_id,
		unsigned short AEorAWB,
		unsigned short X,unsigned short Y,
		unsigned char Dat);
extern unsigned char MLNA_WD_Get_BlockWeight(
		unsigned char inst_id,
		unsigned short AEorAWB,
		unsigned short X,unsigned short Y);
extern void MLNA_WD_Calc_Const(unsigned char inst_id);
extern void MLNA_WD_FuncReset(unsigned char inst_id, float ExposureValue);
extern void MLNA_WD_Set_LDW(unsigned char inst_id, unsigned short LDW);
extern void MLNA_WD_Set_AEV(unsigned char inst_id, unsigned short AEV,unsigned char DraftOrStill);
extern void MLNA_WD_Set_AEH(unsigned char inst_id, unsigned short AEH);
extern void MLNA_WD_Set_AEVW(unsigned char inst_id, unsigned short AEVW,unsigned char DraftOrStill);
extern void MLNA_WD_Set_AEHW(unsigned char inst_id, unsigned short AEHW);
extern void MLNA_WD_Set_AEVBC(unsigned char inst_id, unsigned short AEVBC,unsigned char DraftOrStill);
extern void MLNA_WD_Set_AEHBC(unsigned char inst_id, unsigned short AEHBC);
extern void MLNA_WD_Set_AEYKR(unsigned char inst_id, signed short AEYKR);
extern void MLNA_WD_Set_AEYKGR(unsigned char inst_id, signed short AEYKGR);
extern void MLNA_WD_Set_AEYKGB(unsigned char inst_id, signed short AEYKGB);
extern void MLNA_WD_Set_AEYKB(unsigned char inst_id, signed short AEYKB);
extern unsigned short MLNA_WD_Get_LDW(unsigned char inst_id);
extern unsigned short MLNA_WD_Get_AEV(unsigned char inst_id, unsigned char DraftOrStill);
extern unsigned short MLNA_WD_Get_AEH(unsigned char inst_id);
extern unsigned short MLNA_WD_Get_AEVW(unsigned char inst_id, unsigned char DraftOrStill);
extern unsigned short MLNA_WD_Get_AEHW(unsigned char inst_id);
extern unsigned short MLNA_WD_Get_AEVBC(unsigned char inst_id, unsigned char DraftOrStill);
extern unsigned short MLNA_WD_Get_AEHBC(unsigned char inst_id);
extern signed short MLNA_WD_Get_AEYKR(unsigned char inst_id);
extern signed short MLNA_WD_Get_AEYKGR(unsigned char inst_id);
extern signed short MLNA_WD_Get_AEYKGB(unsigned char inst_id);
extern signed short MLNA_WD_Get_AEYKB(unsigned char inst_id);

extern void MLNA_WD_Set_AWTDUY(unsigned char inst_id, unsigned char AWTDUY);
extern void MLNA_WD_Set_AWTDUR(unsigned char inst_id, unsigned char AWTDUR);
extern void MLNA_WD_Set_AWTDUG(unsigned char inst_id, unsigned char AWTDUG);
extern void MLNA_WD_Set_AWTDUB(unsigned char inst_id, unsigned char AWTDUB);
extern void MLNA_WD_Set_AWTDLY(unsigned char inst_id, unsigned char AWTDLY);
extern void MLNA_WD_Set_AWTDLR(unsigned char inst_id, unsigned char AWTDLR);
extern void MLNA_WD_Set_AWTDLG(unsigned char inst_id, unsigned char AWTDLG);
extern void MLNA_WD_Set_AWTDLB(unsigned char inst_id, unsigned char AWTDLB);

extern unsigned char MLNA_WD_Get_AWTDUY(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDUR(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDUG(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDUB(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDLY(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDLR(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDLG(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDLB(unsigned char inst_id);

extern void MLNA_WD_Set_OBH(unsigned char inst_id, unsigned short OBH);
extern void MLNA_WD_Set_OBHW(unsigned char inst_id, unsigned short OBHW);
extern void MLNA_WD_Set_OBTHBIT(unsigned char inst_id, unsigned short OBTHBIT);
extern unsigned short MLNA_WD_Get_OBH(unsigned char inst_id);
extern unsigned short MLNA_WD_Get_OBHW(unsigned char inst_id);
extern unsigned short MLNA_WD_Get_OBTHBIT(unsigned char inst_id);
extern unsigned long long MLNA_WD_Get_YMAX(unsigned char inst_id);


extern void MLNA_WD_Set_AEYKG(unsigned char inst_id, signed short AEYKG);
extern signed short MLNA_WD_Get_AEYKG(unsigned char inst_id);
extern void MLNA_WD_Set_AWTDY(unsigned char inst_id, unsigned char AWTDY);
extern void MLNA_WD_Set_AWTDC(unsigned char inst_id, unsigned char AWTDC);
extern unsigned char MLNA_WD_Get_AWTDY(unsigned char inst_id);
extern unsigned char MLNA_WD_Get_AWTDC(unsigned char inst_id);

extern unsigned short MLNA_WD_Main2(unsigned char inst_id, float latestE);

unsigned short MLNA_WD_SetOB_ForStill(unsigned char inst_id, signed short offset,unsigned short scale);
void MLNA_WD_GetOB_ForStill(unsigned char inst_id, unsigned short frame, unsigned short RGrOrBGb);
void MLNA_WD_SetStillFrame(unsigned char inst_id, unsigned short frame);

#endif /* end of "_WAVEDETECTION_H" */
