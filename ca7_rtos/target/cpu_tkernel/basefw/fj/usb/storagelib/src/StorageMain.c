/**
 * @file		StorageMain.c
 * @brief		Storage Main program
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "os_user_custom.h"
#include "msclib.h"
#include "MSCA.h"
#include "msca_mediachange.h"

extern MSCA_MEDIA_FLAG_T MediaChangeFlag[];
static long StorageInit_Flg = 0;

void wait_storage_initialized(void)
{
	while (!StorageInit_Flg) OS_User_Dly_Tsk(5);
}
#if 0
void wait_cycle(void)
{
	int lp = 10;
	while(lp){lp--;}
}
#endif
void storage(int stacd)
{
//	int i;
#ifdef USB_DEBUG2
	RS_printf("A");
#endif
	
	/* USB Mass storage class Initializa */
	while( MSCM_Init() != MSC_OK);
	MSCM_Reset();
	
	StorageInit_Flg = 1;
// 20051103 Modify for media change problem
	while( 1 )
	{
		/* USB Mass Storage class main routine */
		if( MSCM_Main() == MSC_TERM )
		{
			break;
		}
		//wait_cycle();
//		dly_tsk(1);
	}
	StorageInit_Flg = 0;
	OS_User_Ext_Tsk();
}
