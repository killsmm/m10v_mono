/**
 * @file		XXXX.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */


//★goodby_RS_System
// could not goodby because of compile
#include "ddim_typedef.h"
#include "ipc_iip.h"
BOOL g_ip_eis_hre_mode = 0;
//E_IP_3DNR_GMD_MODE g_ip_gmd_mode = E_IP_3DNR_GMD_MODE_GYRO;
UCHAR g3AInfoInstance;
UCHAR gAeInfoOnOff;
UCHAR gAwbInfoOnOff;
UCHAR gAdapAeInfoOnOff;
UCHAR g3AInfoFileOutOnOff;
INT32 gMANUAL_COLOR_TEMP;
void IP_Pano_Main( INT32 stacd ){
	IPC_IIP_Get_Version(NULL);//★コンパイルが通らないので…
}
