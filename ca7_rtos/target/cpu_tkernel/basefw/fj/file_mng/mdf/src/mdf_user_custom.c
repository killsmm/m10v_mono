/**
 * @file		mdf_user_custom.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>

#include "ddim_typedef.h"
#include "l1l2cache.h"
#include "FS_lib.h"
#include "fs_user_custom.h"
#include "fj_common.h"
#include "fj_arm.h"
#include "system_attr.h"
//#include "dd_rtc.h"
//#include "os_user_custom.h"

extern void *mdf_malloc(size_t size);
extern void mdf_free(void *ptr);


VOID FS_ipcu_callback_dummy(UCHAR id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
{
	printf( "IPCU request : id=%d, pdata=0x%X, length=%d, continue=%d, total length=%d\n", id, pdata, length, cont, total_length );
}

FS_VOID FS_localtime(tm *time)
{
#if 0
	// Provisional Proccess
	T_DD_RTC_DATA		rtc_data;
	
	Dd_RTC_Get_Time( &rtc_data );
	
	time->tm_sec = rtc_data.SECOND;
	time->tm_min = rtc_data.MINUTE;
	time->tm_hour = rtc_data.HOUR;
	time->tm_mday = rtc_data.DAY;
	time->tm_mon = rtc_data.MONTH - 1;		// tm_mon must be in 0..11
	time->tm_year = rtc_data.YEAR - 1900;	// tm_year is count since 1900
#else

#if 1
	T_FJ_ARM_IPCU_CALENDAR calendar;
	FJ_ERR_CODE ercd;
	UCHAR id;
	static BOOL fs_localtime_first = TRUE;

	if (time != NULL) {
		CHAR	mdr = (*(( unsigned long * )0x1DFF4000) & 0x3F);
		// initialize
		time->tm_sec = 0;
		time->tm_min = 0;
		time->tm_hour = 0;
		time->tm_mday = 1;
		time->tm_wday = 0;
		time->tm_mon = 0;		// tm_mon must be in 0..11
		time->tm_year = 100;	// tm_year is count since 1900

		if (mdr == 0 || mdr == 1) {
			// not ICE boot CM0 active
			ercd = FJ_IPCU_Open( E_FJ_IPCU_MAILBOX_TYPE_14, &id );
			if (ercd != FJ_ERR_OK) {
				BF_Debug_Print_Error(("--- FS_localtime(). FJ_IPCU_Open() ret=%d\n", (int)ercd ));
			}

			ercd = FJ_IPCU_Get_Calendar_CM0( &calendar );

			if (ercd == FJ_ERR_OK) {
				time->tm_sec = calendar.ipcu_second;
				time->tm_min = calendar.ipcu_minute;
				time->tm_hour = calendar.ipcu_hour;
				time->tm_mday = calendar.ipcu_day;
				time->tm_wday = calendar.ipcu_day_of_week;
				time->tm_mon = calendar.ipcu_month - 1;		// tm_mon must be in 0..11
				time->tm_year = calendar.ipcu_year + 100;	// tm_year is count since 1900
			}
			else {
				BF_Debug_Print_Error(("--- FS_localtime(). ret=%d\n", (int)ercd ));
			}

			ercd = FJ_IPCU_Close( E_FJ_IPCU_MAILBOX_TYPE_14 );
			if (ercd != FJ_ERR_OK) {
				BF_Debug_Print_Error(("--- FS_localtime(). FJ_IPCU_Close() ret=%d\n", (int)ercd ));
			}
		}
		else {
			// ICE boot
			if (fs_localtime_first == TRUE) {
				BF_Debug_Print_Warning(("--- FS_localtime(). ICE boot\n" ));
			}
		}
	}
	else {
		BF_Debug_Print_Error(("--- FS_localtime(). parameter err\n"));
	}
	fs_localtime_first = FALSE;
#else
	T_FJ_COMMON_DATE_TIME date_time;
	if (time != NULL) {
		// initialize
		time->tm_sec = 0;
		time->tm_min = 0;
		time->tm_hour = 0;
		time->tm_mday = 1;
		time->tm_wday = 0;
		time->tm_mon = 0;		// tm_mon must be in 0..11
		time->tm_year = 100;	// tm_year is count since 1900

		fj_getbasedatetime(&date_time);

		if (date_time.sec > 59){
			return;
		}else if (date_time.min > 59){
			return;
		}else if (date_time.hour > 23){
			return;
		}else if (date_time.tm_mday > 31){
			return;
		}else if (date_time.tm_mon > 12){
			return;
		}else if (date_time.tm_year > 2107){
			return;
		}
		
		time->tm_sec  = (ULONG)date_time.sec;
		time->tm_min  = (ULONG)date_time.min;
		time->tm_hour = (ULONG)date_time.hour;
		time->tm_mday = (ULONG)date_time.day;
		time->tm_mon  = (ULONG)(date_time.month - 1);	// tm_mon must be in 0..11
		time->tm_year = (ULONG)(date_time.year - 1900);	// tm_year is count since 1900
	}
#endif

#endif


}

FS_VOID FS_SD_getTaskID(FS_DWORD *Taskid)
{
	FW_INT32 tmp_id;
	BF_Fs_User_Custom_Get_Tid(&tmp_id);
	*Taskid = (FS_DWORD)tmp_id;
//	BF_Debug_Print_Error(("FS_SD_getTaskID() Taskid = %d\n", *Taskid));
}

FS_VOID *FS_SD_malloc(FS_DWORD size)
{
#if 1
	return mdf_malloc(size);
#else
	FW_INT32	ercd;
	FW_VOID*	blk;
	if ((ercd = BF_Fs_User_Custom_Pget_Mpl(MPKID_FS, (FW_UINT32)size, &blk)) != D_BF_FS_USER_CUSTOM_OK) {
		BF_Debug_Print_Warning(("FS_SD_malloc failed --- error=%d\n", ercd));
		return NULL;
	}
//	BF_Debug_Print_Information(("FS_SD_malloc ok --- addr=0x%08x - %d[byte]\n", blk, size));
	return (FS_VOID*)blk;
#endif
}

FS_VOID FS_SD_free(FS_VOID *address)
{
#if 1
	mdf_free(address);
#else
	FW_INT32 ercd;
	
	if (address == NULL) return;
	
	if ((ercd = BF_Fs_User_Custom_Rel_Mpl(MPKID_FS, (FW_VOID*)address)) != D_BF_FS_USER_CUSTOM_OK) {
		BF_Debug_Print_Warning(("FS_SD_free failed(%d)!!\n", ercd));
	}
	return;
#endif
}

void *FS_memcpy(void *buf1, const void *buf2, size_t n)
{
	void *ans;
	ans = memcpy( buf1, buf2, n );
	if( 0 != n ) {
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)buf1, (UINT32)n );
	}
	return ans;
}

