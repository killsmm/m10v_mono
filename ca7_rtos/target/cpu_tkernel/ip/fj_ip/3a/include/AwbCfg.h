/*#########################################################################
											Copyright 2015 Socionext Inc.
#########################################################################*/
//----------------------------------------------------------------------
#ifndef _AWBCFG_H_INCLUDE
#define _AWBCFG_H_INCLUDE

typedef struct t_Awbcfg{
	float	mThlSunLight;
	struct {
		float x;
		float y;
	} mInit[2];
	unsigned short	mGateRate[2];			// min 0 - 1000 max
	struct {
		unsigned char	mRL;
		unsigned char	mRH;
		unsigned char	mGL;
		unsigned char	mGH;
		unsigned char	mBL;
 		unsigned char	mBH;
	}mThl[2];
	struct {
		float mR;
		float mGr;
		float mGb;
		float mB;
	} mWBpower[2];
	unsigned char mLevelWheightTbl[2][32];
} TAWB_Cfg;

#endif
