/**
 * @file		mp4_common.c
 * @brief		mp4 common function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/
#include <string.h>
#include "fj_mux.h"
#include "fj_filesystem.h"
#include "fs_if.h"
#include "fj_memory.h"
#include "debug.h"
#include "mp4_common.h"
#include "mux.h"
#include "sdram_map_movie_record.h"
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
#include "libmp4f.h"
#else	// CO_MOVIE_FF_AUDIO_PATTERN
#include "libmovf.h"
#endif	// CO_MOVIE_FF_AUDIO_PATTERN


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Buffer Control Data Definition */
#define D_MP4_COMMON_FMT_MALLOC_TBL_NUM		(3500)		// (MPL)+(SDRAM) 240fps 2hour
#define D_MP4_COMMON_FMT_SDRAM_TBL_NUM		(1535)		//               240fps 2hour

#define D_MP4_COMMON_FMT_THRESHOLD_NUM		(1000)		// Threshold SDRAM or System malloc

/* Memory Control Definition */
#define D_MP4_COMMON_MALLOC					(1)			// malloc
#define D_MP4_COMMON_REALLOC				(2)			// realloc
#define D_MP4_COMMON_FREE					(3)			// free

/* File Seek Type */
#define D_MP4_COMMON_SEEK_SET				(0)			// set position
#define D_MP4_COMMON_SEEK_CUR				(1)			// current position
#define D_MP4_COMMON_SEEK_END				(2)			// end position

/* Mem Copy Size */
#define D_MP4_COMMON_MEMCPY_SIZE			(128)

/* DMA use Channel */
#define D_MP4_COMMON_DMA_USE_CH				(FJ_MEMORY_COPY_XDMACFP_CH_1)

/* Keep vacant size */
#define D_MP4_COMMON_KEEP_MARGIN_SIZE		(0x4000)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{
	E_MP4_COMMON_SYSTEM_TO_MP4			= 0,	// System Time(year,day,etc) to MP4 Time(sec)
	E_MP4_COMMON_MP4_TO_SYSTEM			= 1,	// MP4 Time(sec) to System Time(year,day,etc)
} E_MP4_COMMON_TIME_CONV;

/* Get Date Info timing */
typedef enum{
	E_MP4_COMMON_DATA_TIMING_CURRENT	= 0,	// Current timing (RTC)
	E_MP4_COMMON_DATA_TIMING_CREATE		= 1		// File Create timing (FileSystem)
} E_MP4_COMMON_DATA_TIMING;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// --- SDRAM memory using management table (COMMON) ------------------------
typedef struct{
	VOID*					buf_pointer;	// Buffer Address
	ULONG					buf_size;		// Buffer Size
	BYTE					used_flg;		// Use Flag
} T_MP4_COMMON_SDRAM_MANAGE_TBL;


// --- Buffer Management Table (COMMON) ------------------------------------
typedef struct{
	VOID*					buf_pointer;	// Buffer Address
	ULONG					buf_size;		// Buffer Size
	BYTE					sdram_flg;		// ON:SDRAM / OFF:MPL
} T_MP4_COMMON_MALLOC_MANAGE_TBL;


// --- Time Information ----------------------------------------------------
typedef struct {
	UCHAR					year;			// date year low (ex.2010 -> 10)
	UCHAR					month;			// date month
	UCHAR					day;			// date day
	UCHAR					hour;			// date hour
	UCHAR					min;			// date minute
	UCHAR					sec;			// date second
	USHORT					ad_year;		// date A.D. year
} T_MP4_COMMON_DATE;

// --- FS memory controll --------------------------------------------------
typedef struct {
	INT32					file_id;		// File ID
	INT32					fs_ch;			// FS channel
	UCHAR					fs_ctrl_flg;	// ON:WriteDown / OFF:Write
	UINT32					fs_memory_size;	// Current use size
} T_MP4_COMMON_FS_MEMORY_CTRL;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// --- SDRAM Memory Control -----------------------------------------------
static	volatile	T_MP4_COMMON_MEMORY_INFO 			gMP4_Memory_Info;
static	volatile	T_MP4_COMMON_SDRAM_MANAGE_TBL		gMP4_Sdram_Tbl[D_MP4_COMMON_FMT_SDRAM_TBL_NUM];
static	volatile	T_MP4_COMMON_MALLOC_MANAGE_TBL		gMP4_Malloc_Tbl[D_MP4_COMMON_FMT_MALLOC_TBL_NUM];
static	T_BF_FS_IF_STAT 								gMP4_File_State;

static	volatile	INT32								gMP4_Fs_Request_Count = 0;			// only Mux
static	T_MP4_COMMON_FS_MEMORY_CTRL						gMP4_Fs_Mem_Ctrl[E_FJ_MUX_CH_MAX];	// only Mux
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
static	BOOL			 								gMP4_WIFI_Out_EMMC = TRUE;
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static	INT32	bf_mp4_common_time_convert( E_MP4_COMMON_TIME_CONV convert, T_MP4_COMMON_DATE* date_info, ULONG* mp4_time );
static	VOID	bf_mp4_common_date_get( T_MP4_COMMON_DATE* date_info, E_MP4_COMMON_DATA_TIMING timing );
static	INT32	bf_mp4_common_fs_write( INT32 file_id, const VOID* buf, ULONG count, UINT8 sync_flag );
static	VOID	bf_mp4_common_fs_write_result( INT32 result );
static	LONG	bf_mp4_common_fs_read( INT32 file_id, VOID *buffer, ULONG count );
static	T_MP4_COMMON_FS_MEMORY_CTRL*	bf_mp4_common_fs_memory_get_ctrl( INT32 file_id );
static	UCHAR*	bf_mp4_common_fs_bank_get( INT32 file_id );
static	VOID	bf_mp4_common_fs_bank_change( INT32 file_id );
static	VOID	bf_mp4_common_sdram_memory_ctrl( UINT32 request, ULONG size, VOID** pointer );
static	VOID*	bf_mp4_common_system_malloc( ULONG size );
static	VOID	bf_mp4_common_system_free( VOID* ptr );
static	VOID	bf_mp4_common_dma_copy( ULONG dma_ch, const VOID* src_addr, const VOID* dst_addr, ULONG size );
static	VOID	bf_mp4_common_cpu_copy( const VOID* src_addr, const VOID* dst_addr, ULONG size );

/**
 *	@brief		Time Convaert.\n
 *	@param		i:convert				Convert type
 *	@param		i/o:date_info			System Time(year,day,etc)
 *	@param		i/o:mp4_time			MP4 Time(sec)
 *	@return		MP4_COMMON_OK					(0)Normal End
 *	@return		H264_ERR_INPUT_PARAM	(1)Input Parameter Error
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mp4_common_time_convert( E_MP4_COMMON_TIME_CONV convert, T_MP4_COMMON_DATE* date_info, ULONG* mp4_time )
{
	INT32	i;
	ULONG	temp_date;
	ULONG	year;
	ULONG	month;
	ULONG	day;
	ULONG	hour;
	ULONG	min;
	ULONG	sec;
	UCHAR	leap_year_flg;
	
	if( (date_info == NULL) || (mp4_time == NULL) ){
		return MP4_COMMON_NG_INPUT_PARAM;
	}
	
	if( convert == E_MP4_COMMON_SYSTEM_TO_MP4 ){
		/* 1904/01/01 00:00:00 - 1970/01/01 00:00:00 (2082844800 sec)
		   1970/01/01 00:00:00 - 2000/01/01 00:00:00 ( 946684800 sec)
		   Total : 3029529600(=0xB492F400)sec */
		if( date_info->year > 100 ){
			date_info->year = 0;
		}
		if( (date_info->month < 1) || (date_info->month > 12) ){
			date_info->month = 1;
		}
		if( (date_info->day < 1) || (date_info->day > 31) ){
			date_info->day = 1;
		}
		if( date_info->hour > 23 ){
			date_info->hour = 0;
		}
		if( date_info->min > 59 ){
			date_info->min = 0;
		}
		if( date_info->sec > 59 ){
			date_info->sec = 0;
		}
		
		// 1. Calculate Number of days or sec of 2000/01/01 00:00:00 - 20xx/01/01 00:00:00
		temp_date = 0;
		for( i=0; i<date_info->year; i++ ){
			// Judge a leap year
			if( (i%4) == 0 ){
				if( (i%100) == 0 ){
					if( (i%400) == 0 ){
						temp_date += 366;		// Special leap year once every 400 years
					}
					else{
						temp_date += 365;		// Year that is not leap year once every 100 years
					}
				}
				else{
					temp_date += 366;			// normal leap year
				}
			}
			else{
				temp_date += 365;				// not a leap year
			}
		}
		
		*mp4_time = temp_date * 60 * 60 * 24;		// Days*60(sec)*60(min)*24(hour)
		
		// 2. Calculate seconds of 20xx/01/01 00:00:00 - 20xx/xx/xx 00:00:00
		temp_date = 0;
		for( i=1; i<date_info->month; i++ ){
			if( i == 2 ){
				if( (date_info->year%4) == 0 ){
					// a leap year
					temp_date += 29;
				}
				else{
					// Because it is a range of 2003-2050, it doesn't think in detail.
					temp_date += 28;
				}
			}
			else if( (i==1) ||
					 (i==3) ||
					 (i==5) ||
					 (i==7) ||
					 (i==8) ||
					 (i==10) ||
					 (i==12) ){
				temp_date += 31;
			}
			else if( (i==4) ||
					 (i==6) ||
					 (i==9) ||
					 (i==11) ){
				temp_date += 30;
			}
			else{
				BF_Debug_Print_Error(("MP4F_time Err.\n"));
			}
		}
		
		// Add Number of days
		temp_date += (ULONG)(date_info->day - 1);
		
		*mp4_time += (ULONG)(temp_date * 60 * 60 * 24);	// Number of days*60(sec)*60(min)*24(hour)
		
		// 3. Calculate sec for 00:00:00 - xx:xx:xx
		*mp4_time += (ULONG)((date_info->hour * 60 * 60) + (date_info->min * 60 ) + date_info->sec);
		
		// 4. Add sec for 1904/01/01 00:00:00 - 2000/01/01 00:00:00
		*mp4_time += (ULONG)0xB492F400;
		
	}
	else{
		// MP4 time get
		sec = *mp4_time;
		
		// year
		year = 1904;
		while(1){
			if( ((year%4 == 0) && (year%100 != 0)) || (year%400 == 0) ){
				if(sec < 0x1E28500){
					// leap year
					leap_year_flg = 1;
					break;
				}
				
				sec -= 0x1E28500;		// [leap year] sec of one year.(366*24*60*60)
			}
			else{
				if(sec < 0x1E13380){
					// normal year
					leap_year_flg = 0;
					break;
				}
				
				sec -= 0x1E13380;		// [normal year] sec of one year.(365*24*60*60)
			}
			year++;
		}
		// month
		month = 1;
		while(1){
			if( month == 2 ){
				if( leap_year_flg == 1 ){
					if(sec < 0x263B80){
						break;
					}
					sec -= 0x263B80;	// [leap year] sec of one month.(29*24*60*60)
				}
				else{
					if(sec < 0x24EA00){
						break;
					}
					sec -= 0x24EA00;	// [normal year] sec of one month.(28*24*60*60)
				}
			}
			else if( ( month == 1 ) ||
					 ( month == 3 ) ||
					 ( month == 5 ) ||
					 ( month == 7 ) ||
					 ( month == 8 ) ||
					 ( month == 10 ) ||
					 ( month == 12 ) ){
				if(sec < 0x28DE80){
					break;
				}
				sec -= 0x28DE80;		// sec of one month.(31*24*60*60)
			}
			else if( ( month == 4 ) ||
					 ( month == 6 ) ||
					 ( month == 9 ) ||
					 ( month == 11 ) ){
				if(sec < 0x278D00){
					break;
				}
				sec -= 0x278D00;		// sec of one month.(30*24*60*60)
			}
			else{
				BF_Debug_Print_Error(("h264_format_time_convert error. [%lu]\n", sec));
				return MP4_COMMON_NG_INPUT_PARAM;
			}
			month++;
		}
		// day
		day =  sec/(24*60*60);
		sec -= day*(24*60*60);
		day += 1;						// 1day added.(Because "Day" starts from "1")
		
		
		// hour
		hour = sec/(60*60);
		sec -= hour*(60*60);
		
		// min
		min =  sec/60;
		sec -= min*60;
		
		// system time set
		if( year < 2000 ){
			date_info->year = year - 1900;
		}
		else{
			date_info->year = year - 2000;
		}
		date_info->month	= month;
		date_info->day		= day;
		date_info->hour	= hour;
		date_info->min		= min;
		date_info->sec		= sec;
		date_info->ad_year	= year;
	}
	
	return MP4_COMMON_OK;
}

/**
 *  @brief		Movie Record Get Date Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_mp4_common_date_get( T_MP4_COMMON_DATE* date_info, E_MP4_COMMON_DATA_TIMING timing )
{
	USHORT	temp_year;
	
	if( timing == E_MP4_COMMON_DATA_TIMING_CREATE ){
		temp_year = (gMP4_File_State.m_date & 0xFE00) >> 9;
		temp_year += 1980;
		
		date_info->year    = temp_year - 2000;
		date_info->month   = (gMP4_File_State.m_date & 0x01E0) >> 5;
		date_info->day     = gMP4_File_State.m_date & 0x001F;
		date_info->hour    = (gMP4_File_State.m_time & 0xF800) >> 11;
		date_info->min     = (gMP4_File_State.m_time & 0x07E0) >> 5;
		date_info->sec     = (gMP4_File_State.m_time & 0x001F) * 2;
		date_info->ad_year = temp_year;
	}
	else if( timing == E_MP4_COMMON_DATA_TIMING_CURRENT ){
#if 0 // need_poring		
		// use RTC case (Current Date&Time Information)
		ret = Dd_RTC_Get_Time( &rtc_data );
		if( ret != 0 ){
			BF_Debug_Print_Error(("---Dd_RTC_Get_Time RTC Error. (%d) \n", ret));
		}
		date_info->year    = rtc_data.YEAR - 2000;
		date_info->month   = rtc_data.MONTH;
		date_info->day     = rtc_data.DAY;
		date_info->hour    = rtc_data.HOUR;
		date_info->min     = rtc_data.MINUTE;
		date_info->sec     = rtc_data.SECOND;
		date_info->ad_year = rtc_data.YEAR;
#endif
	}
	else{
		BF_Debug_Print_Error(("--- fj_mux_date_get Error! timing=%d\n",timing ));
	}
	return;
}

/**
 *	@brief		FS write wrapper.\n
 *	@param		i:file_id	file Identifier
 *	@param		i:buf		write buffer pointer
 *	@param		i:count		write size
 *	@return		MP4_COMMON_OK						Normal End
 *	@return		MP4_COMMON_NG_INPUT_PARAM		Input Parameter Error
 *	@return		MP4_COMMON_NG_SYSTEM				System Error
 *	@note		None
 *	@attention	None
*/
static INT32 bf_mp4_common_fs_write( INT32 file_id, const VOID* buf, ULONG count, UINT8 sync_flag )
{
	INT32	ret;
	ULONG	length;
	
	if( file_id == 0 ){
		BF_Debug_Print_Error(("--- FS Write Error. file_id=0\n"));
		return MP4_COMMON_NG_INPUT_PARAM;
	}
	if( buf == NULL ){
		BF_Debug_Print_Error(("--- FS Write Error. buf=NULL\n"));
		return MP4_COMMON_NG_INPUT_PARAM;
	}
	if( count == 0 ){
		BF_Debug_Print_Error(("--- FS Write. count=0\n"));
		return MP4_COMMON_OK;
	}
	
	if( sync_flag == D_BF_MUX_OFF ){
		// Async
		ret = FJ_Fs_WriteAsync( (FsEntryCB)bf_mp4_common_fs_write_result, file_id, (UINT32*)buf, count );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Error(("--- FJ_Fs_WriteAsync.Error=%d\n", ret ));
			return MP4_COMMON_NG_SYSTEM;
		}
		
		gMP4_Fs_Request_Count++;
		OS_User_Tslp_Tsk(1);		// for dispatch
	}
	else{
		// Sync
		ret = FJ_Fs_Write( file_id, (UINT32*)buf, count, &length );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Error(("--- FJ_Fs_Write.Error=%d\n", ret ));
			return MP4_COMMON_NG_SYSTEM;
		}
		if( count != length ){
			BF_Debug_Print_Error(("--- FJ_Fs_Write. not all write count=%lu, length=%lu\n", count, length ));
		}
	}
	return MP4_COMMON_OK;
}

/**
 *	@brief		FS write entry.\n
 *	@param		i:result	FS write Result
 *	@return		None
 *	@note		None
 *	@attention	None
*/
static VOID bf_mp4_common_fs_write_result( INT32 result )
{
	gMP4_Fs_Request_Count--;
	
	if( result != D_OS_USER_E_OK ){
		/* Report error to Main Process */
		BF_Debug_Print_Error(("--- fs_write err=%d\n", result));
		return ;
	}
	return ;
}

/**
 *	@brief		FS read wrapper.\n
 *	@param		i:file_id	File Descripter
 *	@param		i:buffer	reading address
 *	@param		i:count		reading size(Byte)
 *	@return		reading size(result)	Number of reading size(Byte)
 *	@return		-1						Faulure
 *	@note		None
 *	@attention	None
*/
static LONG bf_mp4_common_fs_read( INT32 file_id, VOID *buffer, ULONG count )
{
	INT32	ret;
	ULONG	read_size;
	
	if( file_id == 0 || buffer == NULL || count == 0 ){
		BF_Debug_Print_Error(("--- input parameter error on bf_mp4_common_fs_read\n"));
		return -1;
	}
	
	ret = FJ_Fs_Read( file_id, buffer, count, &read_size );
	
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("--- FJ_Fs_Read error.ret=%d\n",ret));
		return -1;
	}
	
	if( count != read_size ){
		BF_Debug_Print_Error(("--- FJ_Fs_Read size error.count=%lu, read_size=%lu\n",count, read_size ));
		return -1;
	}
	return (LONG)read_size;
}

/**
 *	@brief		Fine fs contrller by file id.\n
 *	@param		file_id
 *	@return		Pointer to ctrl			Normal End
 *	@return		NULL					Not find
 *	@note		None
 *	@attention	None
*/
static T_MP4_COMMON_FS_MEMORY_CTRL* bf_mp4_common_fs_memory_get_ctrl( INT32 file_id )
{
	UCHAR	ch;
	for( ch=0; ch<E_FJ_MUX_CH_MAX; ch++ ){
		if(gMP4_Fs_Mem_Ctrl[ch].file_id == file_id){
			return &gMP4_Fs_Mem_Ctrl[ch];
		}
	}
	return NULL;
}

/**
 *	@brief		Fs Bank(Muxing Bank) Get.\n
 *	@param		file_id
 *	@return		bank address
 *	@note		None
 *	@attention	None
*/
static UCHAR* bf_mp4_common_fs_bank_get( INT32 file_id )
{
	return (UCHAR*)gMP4_Memory_Info.fs_bank_addr[file_id][gMP4_Memory_Info.fs_bank_current[file_id]];
}

/**
 *	@brief		Fs Bank(Muxing Bank) Change.\n
 *	@param		file_id
 *	@return		None
 *	@note		None
 *	@attention	None
*/
static VOID bf_mp4_common_fs_bank_change( INT32 file_id )
{
	++gMP4_Memory_Info.fs_bank_current[file_id];
	if( gMP4_Memory_Info.fs_bank_current[file_id] >= MP4_COMMON_FS_BANK_MAX_NUM ){
		gMP4_Memory_Info.fs_bank_current[file_id] = 0;
	}
	return;
}

/**
 *	@brief		SDRAM Memory Buffer Control.\n
 *	@param		i:request		Request\n
 *						D_MP4_COMMON_MALLOC		(1)malloc
 *						D_MP4_COMMON_REALLOC	(2)realloc
 *						D_MP4_COMMON_FREE		(3)free\n
 *	@param		i:size			Size\n
 *						Only malloc and realloc.\n
 *	@param		i/o:pointer		Buffer Pointer\n
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_mp4_common_sdram_memory_ctrl(
				UINT32		request,
				ULONG		size,
				VOID**		pointer
				)
{
	UINT32 sdram_index;
	UCHAR* dummy_address;
	
	// Input Parameter Check
	switch( request ){
		case D_MP4_COMMON_MALLOC :
			// malloc
			if( pointer == NULL ){
				BF_Debug_Print_Error(("SDRAM malloc pointer error.\n"));
				return;
			}
			if( (size == 0) || (size > D_MP4_COMMON_SDRAM_BANK_SIZE) ){
				BF_Debug_Print_Error(("SDRAM malloc size error. size=%lu\n",size));
				*pointer = NULL;
				return;
			}
			// Hunt Vacant Area
			for( sdram_index=0; sdram_index<gMP4_Memory_Info.header_mem_table_count; sdram_index++ ){
				if( gMP4_Sdram_Tbl[sdram_index].used_flg == FJ_MUX_OFF ){
					*pointer = gMP4_Sdram_Tbl[sdram_index].buf_pointer;
					gMP4_Sdram_Tbl[sdram_index].buf_size = size;
					gMP4_Sdram_Tbl[sdram_index].used_flg = FJ_MUX_ON;
					memset( *pointer, 0, size );

//					printf("sd:%d\n", sdram_index);

					return;
				}
			}
			BF_Debug_Print_Error(("SDRAM malloc Hunt Error.\n"));
			*pointer = NULL;
			break;
			
		case D_MP4_COMMON_REALLOC :
			// realloc
			if( pointer == NULL ){
				BF_Debug_Print_Error(("SDRAM realloc pointer error.\n"));
				return;
			}
			if( (size == 0) || (size > D_MP4_COMMON_SDRAM_BANK_SIZE) ){
				BF_Debug_Print_Error(("SDRAM realloc size error. size=%lu\n",size));
				*pointer = NULL;
				return;
			}
			if( *pointer == NULL ){
				// In this case, it same as malloc request
				for( sdram_index=0; sdram_index<gMP4_Memory_Info.header_mem_table_count; sdram_index++ ){
					if( gMP4_Sdram_Tbl[sdram_index].used_flg == FJ_MUX_OFF ){
						*pointer = gMP4_Sdram_Tbl[sdram_index].buf_pointer;
						gMP4_Sdram_Tbl[sdram_index].buf_size = size;
						gMP4_Sdram_Tbl[sdram_index].used_flg = FJ_MUX_ON;
						memset( *pointer, 0, size );
						return;
					}
				}
				BF_Debug_Print_Error(("SDRAM realloc(malloc) Hunt Error.\n"));
				return;
			}
			else{
				// realloc case
				for( sdram_index=0; sdram_index<gMP4_Memory_Info.header_mem_table_count; sdram_index++ ){
					if( (gMP4_Sdram_Tbl[sdram_index].buf_pointer == *pointer)
					 && (gMP4_Sdram_Tbl[sdram_index].used_flg == FJ_MUX_ON) ){
						if( gMP4_Sdram_Tbl[sdram_index].buf_size == size ){
							// It same as request size, so it treats as normal end
							return;
						}
						if( gMP4_Sdram_Tbl[sdram_index].buf_size > size ){
							// I cannot change to small size
							BF_Debug_Print_Error(("SDRAM realloc illegal size.\n"));
							return;
						}
						dummy_address = (UCHAR*)gMP4_Sdram_Tbl[sdram_index].buf_pointer;
						dummy_address += gMP4_Sdram_Tbl[sdram_index].buf_size;
						memset( (VOID*)dummy_address, 0, (size - gMP4_Sdram_Tbl[sdram_index].buf_size) );
						gMP4_Sdram_Tbl[sdram_index].buf_size = size;
						return;
					}
				}
				BF_Debug_Print_Error(("SDRAM realloc Hunt Error.\n"));
				*pointer = NULL;
			}
			break;
			
		case D_MP4_COMMON_FREE :
			// free
			if( (pointer == NULL) || (*pointer == NULL) ){
				BF_Debug_Print_Error(("SDRAM free pointer=NULL.\n"));
				break;
			}
			for( sdram_index=0; sdram_index<gMP4_Memory_Info.header_mem_table_count; sdram_index++ ){
				if( (gMP4_Sdram_Tbl[sdram_index].buf_pointer == *pointer)
					 && (gMP4_Sdram_Tbl[sdram_index].used_flg == FJ_MUX_ON) ){
					gMP4_Sdram_Tbl[sdram_index].buf_size = 0;
					gMP4_Sdram_Tbl[sdram_index].used_flg = FJ_MUX_OFF;
					return;
				}
			}
			BF_Debug_Print_Error(("SDRAM free not object.\n"));
			break;
			
		default :
			BF_Debug_Print_Error(("SDRAM request abnormal.\n"));
			break;
	}
	return;
}

/**
 *	@brief		Hunt the memory area.\n
 *	@param		i:size				Memory Area Size
 *	@return		Not equall NULL		Address of huntted area
 *	@return		NULL				Hunt NG
 *	@note		None
 *	@attention	None
*/
VOID* bf_mp4_common_system_malloc( ULONG size )
{
	OS_USER_ER	ercd;
	OS_USER_VP	blk = (OS_USER_VP)0;
	INT32*		pointer;
	
	pointer = NULL ;
	
	ercd = OS_User_Pget_Mpl( MPKID_H264, (INT32)size, &blk );
	if( ercd != D_OS_USER_E_OK ){
		BF_Debug_Print_Information(("------ malloc NG on File Format area ----\n"));
		pointer = 0;
	}
	
	pointer = (INT32*)blk;
	
	if( pointer != 0 ){
		memset( (VOID*)pointer, 0, size );
	}
	
	return pointer;
}

/**
 *	@brief		Free memory area.\n
 *	@param		i:ptr		Pointer
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID bf_mp4_common_system_free( VOID* ptr )
{
	INT32	err;
	
	err = OS_User_Rel_Mpl( MPKID_H264, (OS_USER_VP)ptr );
	if( err != D_OS_USER_E_OK ){
		err = err;
	}
	return ;
}


/**
 *	@brief	dma copy wrapper
*/
static VOID bf_mp4_common_dma_copy( ULONG dma_ch, const VOID* src_addr, const VOID* dst_addr, ULONG size )
{
	ULONG cpu_copy_size;
	ULONG dma_copy_size;

	// Address adjust
	cpu_copy_size = size % 128;
	dma_copy_size = size - cpu_copy_size;

	if( dma_copy_size > 0 ){
		FJ_MEMORY_Copy_Sdram_Sync( (FJ_MEMORY_COPY_DMA_CH)dma_ch, (UINT32)src_addr, (UINT32)dst_addr, dma_copy_size, FJ_MEMORY_WAITMODE_EVENT );
	}
	if( cpu_copy_size > 0 ){
		bf_mp4_common_cpu_copy( (VOID *)((ULONG)src_addr+dma_copy_size), (VOID *)((ULONG)dst_addr + dma_copy_size), cpu_copy_size );
	}
	
	return;
}

/**
 *	@brief	cpu copy wrapper
*/
static VOID bf_mp4_common_cpu_copy( const VOID* src_addr, const VOID* dst_addr, ULONG size )
{
	memcpy( (VOID *)dst_addr, src_addr, size );
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)dst_addr, size );
	
	return;
}

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

/**
 *	@brief		Set Bit Stream Pointer Address.\n
 *	@param		i:bit_stream       :Bit Stream Information
 *				i:pointer_address  :Pointer Address
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_BS_Set_Pointer_Address( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR* pointer_address )
{
	bit_stream->bit_remain = 8;
	bit_stream->pointer = pointer_address;
	return;
}

/**
 *	@brief		Skip Bit Stream Pointer Address.\n
 *	@param		i:bit_stream    :Bit Stream Information
 *				i:skip_bytes    :Skip Bytes
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_BS_Skip_Pointer_Address( T_MP4_COMMON_STREAM_CONTROL* bit_stream, ULONG skip_bytes )
{
	bit_stream->bit_remain = 8;
	bit_stream->pointer += skip_bytes;
	return;
}

/**
 *	@brief		Skip Bit Stream Pointer Scope Bit.\n
 *	@param		i:bit_stream   :Bit Stream Information
 *				i:skip_bits    :Skip Bits
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, ULONG skip_bits )
{
	ULONG  skip_loop  = 0;
	for( skip_loop=0; skip_loop < skip_bits; skip_loop++ ){
		bit_stream->bit_remain--;
		if( bit_stream->bit_remain == 0 ){
			bit_stream->pointer++;
			bit_stream->bit_remain = 8;
		}
	}
	return;
}

/**
 *	@brief		Get Bit Stream Pointer.\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		Pointer Address
 *	@note		None
 *	@attention	None
 */
UCHAR* BF_MP4_Common_BS_Get_Pointer( T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	return bit_stream->pointer;
}

/**
 *	@brief		Get Bit Stream Remain bits.\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		Remain bits
 *	@note		None
 *	@attention	None
 */
UCHAR BF_MP4_Common_BS_Get_Remain_Bits( T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	return bit_stream->bit_remain;
}

/**
 *	@brief		Bit Stream Read(unsigend int 1bit).\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
ULONG BF_MP4_Common_BS_Read_U_1bit( T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32 read_value = 0;
	
	bit_stream->bit_remain--;
	read_value = ( *(bit_stream->pointer) >> bit_stream->bit_remain ) & 0x1;
	if( bit_stream->bit_remain == 0 ){
		bit_stream->pointer++;
		bit_stream->bit_remain = 8;
	}
	
	return read_value;
}

/**
 *	@brief		Bit Stream Read(unsigned n bit).\n
 *	@param		i:bit_stream  :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
ULONG BF_MP4_Common_BS_Read_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR read_num )
{
	UINT32 read_value = 0;
	UINT32 target_bit = 0;
	UCHAR  read_loop  = 0;
	
	for( read_loop=0; read_loop < read_num; read_loop++ ){
		target_bit = read_num - read_loop - 1;
		read_value |= ( BF_MP4_Common_BS_Read_U_1bit(bit_stream) << target_bit );
	}
	
	return read_value;
}

/**
 *	@brief		Bit Stream Read(unsigned n bit : max 64bit).\n
 *	@param		i:bit_stream  :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
UINT64 BF_MP4_Common_BS_Read_64_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR read_num )
{
	UINT64 read_value = 0;
	UINT32 target_bit = 0;
	UCHAR  read_loop  = 0;
	
	for( read_loop=0; read_loop < read_num; read_loop++ ){
		target_bit = read_num - read_loop - 1;
		read_value |= ( BF_MP4_Common_BS_Read_U_1bit(bit_stream) << target_bit );
	}
	
	return read_value;
}



/**
 *	@brief		Bit Stream Read(unsined int Exp-Golomb-coded).\n
 *	@param		i:bit_stream   :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 BF_MP4_Common_BS_Read_U_E( T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32 read_value = 0;
	INT32  read_loop  = 0;
	
	for( read_loop = -1; read_value == 0; read_loop++ ){
		read_value = BF_MP4_Common_BS_Read_U_1bit(bit_stream);
	}
	
	read_value = ((1 << read_loop) - 1) + BF_MP4_Common_BS_Read_U_Nbit(bit_stream, read_loop);
	
    return read_value;
}

/**
 *	@brief		Bit Stream Read(sined int Exp-Golomb-coded).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
INT32 BF_MP4_Common_BS_Read_S_E( T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32 read_value = 0;
	
	read_value = BF_MP4_Common_BS_Read_U_E(bit_stream);
	if( read_value & 0x1 ){
		read_value = (read_value+1)/2;
	}
	else{
		read_value = -(read_value/2);
	}
	
	return read_value;
}


/**
 *	@brief		Bit Stream Write(unsigend int 1bit).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_BS_Write_U_1bit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, INT32 write_value )
{
	bit_stream->bit_remain--;
	
	if( (write_value & 0x01) == 1 ){
		// set value is "1"
		(*(bit_stream->pointer)) |= (1 << bit_stream->bit_remain);
	}
	else{
		// set value is "0"
		(*(bit_stream->pointer)) &= ~(1 << bit_stream->bit_remain);
	}
	if( bit_stream->bit_remain == 0 ){
		bit_stream->pointer++;
		bit_stream->bit_remain = 8;
	}
	
	return;
}


/**
 *	@brief		Bit Stream Write(unsigned n bit).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_BS_Write_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR write_num, INT32 write_value )
{
	INT32  write_loop = 0;
	UINT32 target_bit = 0;
	
	for( write_loop = 0; write_loop < write_num; write_loop++ ) {
		target_bit = write_num - write_loop - 1;
		BF_MP4_Common_BS_Write_U_1bit( bit_stream, (write_value >> target_bit) & 0x01 );
	}
	
	return;
}


/**
 *	@brief		Bit Stream Write(unsigned n bit : max 64bit).\n
 *	@param		i:bit_stream    :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_BS_Write_64_U_Nbit( T_MP4_COMMON_STREAM_CONTROL* bit_stream, UCHAR write_num, INT64 write_value )
{
	INT32  write_loop = 0;
	UINT32 target_bit = 0;
	
	for( write_loop = 0; write_loop < write_num; write_loop++ ) {
		target_bit = write_num - write_loop - 1;
		BF_MP4_Common_BS_Write_U_1bit( bit_stream, (write_value >> target_bit) & 0x01 );
	}
	
	return;
}


/**
 *	@brief		Start Code Emuration Convert.\n
 *	@param		i:stream_address   :Stream Address
 *	@param		i/o:stream_size    :Stream Size
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_Start_Code_Emuration_Convert( UINT8* src_stream_address, UINT32 src_stream_size, UINT8* dst_stream_address, UINT32* dst_stream_size )
{
	UINT32	read_offset		= 0;
	UINT32	write_offset	= 0;
	
	if( src_stream_size <= 3 ){
		BF_Debug_Print_Error(("BF_MP4_Common_Start_Code_Emuration_Convert : src size is too small(3 bytes or less).\n"));
		*dst_stream_size = 0;
		return;
	}
	
	for( read_offset = 0; read_offset < (src_stream_size - 3); read_offset++ ){
		
		if( (src_stream_address[read_offset] == 0) && (src_stream_address[read_offset+1] == 0) && (src_stream_address[read_offset+2] == 3) ){
			
			dst_stream_address[write_offset]	= src_stream_address[read_offset];		// 00 write
			dst_stream_address[write_offset+1]	= src_stream_address[read_offset+1];	// 00 write
			
			write_offset += 2;
			read_offset  += 2;
		}
		else{
			
			dst_stream_address[write_offset] = src_stream_address[read_offset];
			write_offset++;
		}
	}
	
	// Flush the last 3 bytes.
	dst_stream_address[write_offset]	= src_stream_address[read_offset];
	dst_stream_address[write_offset+1]	= src_stream_address[read_offset+1];
	dst_stream_address[write_offset+2]	= src_stream_address[read_offset+2];
	write_offset += 3;
	
	*dst_stream_size = write_offset;
	
	return;
}


/**
 *	@brief		Initialize File Fomat Memory for MUX.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_Memory_Initialize( T_MP4_COMMON_MEMORY_INFO* mem_info )
{
	UINT32	i;
	UCHAR*	bufPtr;
	
	memcpy( (VOID*)&gMP4_Memory_Info, mem_info, sizeof(gMP4_Memory_Info) );
	
	// Initialize Buffer Management Data for File Format
	for( i=0; i<D_MP4_COMMON_FMT_MALLOC_TBL_NUM; i++ ){
		gMP4_Malloc_Tbl[i].buf_pointer	= NULL;
		gMP4_Malloc_Tbl[i].buf_size		= 0;
		gMP4_Malloc_Tbl[i].sdram_flg	= 0;
	}
	
	// Initialize SDRAM Area for File Format
	gMP4_Memory_Info.header_mem_table_count = gMP4_Memory_Info.header_mem_size / D_MP4_COMMON_SDRAM_BANK_SIZE;
	if( gMP4_Memory_Info.header_mem_table_count > D_MP4_COMMON_FMT_SDRAM_TBL_NUM ){
		gMP4_Memory_Info.header_mem_table_count = D_MP4_COMMON_FMT_SDRAM_TBL_NUM;
	}
	
	bufPtr = (UCHAR*)gMP4_Memory_Info.header_mem_addr;
	for( i=0; i<gMP4_Memory_Info.header_mem_table_count; i++ ){
		gMP4_Sdram_Tbl[i].buf_pointer = (VOID*)bufPtr;
		gMP4_Sdram_Tbl[i].buf_size = 0;
		gMP4_Sdram_Tbl[i].used_flg = 0;
		
		bufPtr += D_MP4_COMMON_SDRAM_BANK_SIZE;
	}
	
	return;
}

/**
 *	@brief		Free All memory area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_MP4_Common_Memory_Finalize( VOID )
{
	INT32	table_index;
	
	for( table_index=0; table_index<D_MP4_COMMON_FMT_MALLOC_TBL_NUM; table_index++ ){
		if( gMP4_Malloc_Tbl[table_index].buf_pointer != NULL ){
			if( gMP4_Malloc_Tbl[table_index].sdram_flg != FJ_MUX_ON ){
				// System Memory
				bf_mp4_common_system_free( gMP4_Malloc_Tbl[table_index].buf_pointer );
			}
			gMP4_Malloc_Tbl[table_index].buf_pointer	= NULL;
			gMP4_Malloc_Tbl[table_index].buf_size		= 0;
			gMP4_Malloc_Tbl[table_index].sdram_flg		= FJ_MUX_OFF;
		}
	}
	
	// SDRAM
	for( table_index=0; table_index<gMP4_Memory_Info.header_mem_table_count; table_index++ ){
		if( gMP4_Sdram_Tbl[table_index].used_flg == FJ_MUX_ON ){
			gMP4_Sdram_Tbl[table_index].buf_size = 0;
			gMP4_Sdram_Tbl[table_index].used_flg = FJ_MUX_OFF;
		}
	}
	return;
}

/**
 *	@brief		File Create Time Initialize.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID BF_MP4_Common_Fs_Create_Time_Initialize( INT32 file_id )
{
	INT32	ret;
	
	ret = BF_Fs_If_Fstat( file_id, &gMP4_File_State );
	if( ret != FSDCF_ERRNO_OK ){
		BF_Debug_Print_Error(("--- BF_Fs_If_Fstat error. ret=%d\n",ret));
	}
	
	return;
}

// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
/**
 *	@brief		Get MEDIA out put device for Wi-Fi.\n
 *	@param		TRUE: eMMC, FALSE: SD card
 *	@return		None
 *	@note		None
 *	@attention	None
*/
VOID BF_MP4_Common_WIFI_Out_EMMC( BOOL enable )
{
	gMP4_WIFI_Out_EMMC = enable;
}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

/**
 *	@brief		Get MEDIA Size wrapper.\n
 *	@param		None
 *	@return		File Size(Byte)
 *	@note		None
 *	@attention	None
*/
ULLONG BF_MP4_Common_Fs_Mediasize_Get( VOID )
{
	INT32	ret;
	ULLONG	media_size = 0;
	FJ_FS_GEOMETRY	geometry;
	
#if 1	// RTOS media
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifndef CO_WIFI_EMMC_SAVE
// --- REMOVE_WIFI_EMMC_SAVE END ---
{
	FJ_MEDIA_ID media_id;
	UCHAR detect = 0;
	
	FJ_Get_MediaDetect(1, &detect);
	if( detect == 0 ){
		media_id = (FJ_MEDIA_ID)FJ_Get_EMMC_Storage_ID();
	}else{
		media_id = FJ_MEDIA_SD1;
	}
	ret = FJ_GetStorageMediaGeometryEx( media_id, &geometry );
}
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#else
	if (gMP4_WIFI_Out_EMMC == FALSE) {
		ret = FJ_GetStorageMediaGeometryEx( FJ_MEDIA_SD1, &geometry );
	}
	else {
		ret = FJ_GetStorageMediaGeometryEx( FJ_MEDIA_EMMC_STORAGE2, &geometry );
	}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---
#else
	ret = FJ_GetStorageMediaGeometryEx( FJ_MEDIA_SD0, &geometry );
#endif
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("--- FJ_GetStorageMediaGeometryEx error. ret=%d\n",ret));
	}
	
	media_size = (ULLONG)geometry.FreeClusters * geometry.SectorPerCluster * geometry.BytePerSector;
	
	// margin
	if( media_size < D_MP4_COMMON_KEEP_MARGIN_SIZE ){
		media_size = 0;
	}
	else{
		media_size -= D_MP4_COMMON_KEEP_MARGIN_SIZE;
	}
	
	return media_size;
}

/**
 *	@brief		Initialize FS control data.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
INT32 BF_MP4_Common_Fs_Memory_Initialize( INT32 file_id[], INT32 file_cnt )
{
	INT32 ch;
	T_MP4_COMMON_FS_MEMORY_CTRL* pCtrl;
	
	if( file_cnt > E_FJ_MUX_CH_MAX ) {
		return MP4_COMMON_NG;
	}
	
	for( ch=0; ch<file_cnt; ch++ ){
		pCtrl = &(gMP4_Fs_Mem_Ctrl[ch]);
		pCtrl->fs_memory_size	= 0;
		pCtrl->fs_ctrl_flg		= FJ_MUX_OFF;
		pCtrl->file_id			= file_id[ch];
		pCtrl->fs_ch			= ch;
	}
	return MP4_COMMON_OK;
}

/**
 *	@brief		Update FS control data.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
*/
INT32 BF_MP4_Common_Fs_Memory_Update( INT32 file_id, UCHAR opt )
{
	INT32 ch;
	INT32 ret = MP4_COMMON_OK;
	volatile T_MP4_COMMON_FS_MEMORY_CTRL* pCtrl;

	if( opt == FJ_MUX_MEMOPT_ADD ){
		for( ch=0; ch<E_FJ_MUX_CH_MAX; ch++ ){
			pCtrl = &(gMP4_Fs_Mem_Ctrl[ch]);
			if(pCtrl->file_id == 0){
				pCtrl->file_id = file_id;
				pCtrl->fs_memory_size	= 0;
				pCtrl->fs_ctrl_flg		= FJ_MUX_OFF;
				pCtrl->fs_ch			= ch;
				break;
			}
		}
	}
	else if( opt == FJ_MUX_MEMOPT_DELETE ){
		for( ch=0; ch<E_FJ_MUX_CH_MAX; ch++ ){
			pCtrl = &(gMP4_Fs_Mem_Ctrl[ch]);
			if(pCtrl->file_id == file_id){
				pCtrl->file_id = 0;
				pCtrl->fs_memory_size	= 0;
				pCtrl->fs_ctrl_flg		= FJ_MUX_OFF;
				pCtrl->fs_ch			= 0;
				break;
			}
		}
	}
	else{
		ch = E_FJ_MUX_CH_MAX;
	}

	if(ch == E_FJ_MUX_CH_MAX){
		BF_Debug_Print_Error(("--- BF_MP4_Common_Fs_Memory_Update error.\n"));
		ret = MP4_COMMON_NG;
	}
	
	return ret;
}

/**
 *	@brief		Write to FS memory or MEDIA.\n
 *	@param		i:fd		File Descripter
 *	@param		i:buffer	write data buffer pointer
 *	@param		i:count		write Size(Request)
 *	@return		Writing Size(Result)	Number of writing size(Byte)
 *	@return		-1						Faulure
 *	@note		None
 *	@attention	None
*/
INT32 BF_MP4_Common_Fs_Memory_Write( INT32 file_id, const VOID *buffer, ULONG count )
{
	UCHAR*	dummy_address;
	UCHAR*	dummy_address2;
	INT32	ret ;
	ULONG	size ;
	T_MP4_COMMON_FS_MEMORY_CTRL* pCtrl;

	pCtrl = bf_mp4_common_fs_memory_get_ctrl( file_id );
	if( pCtrl == NULL ) {
		return MP4_COMMON_NG;
	}
	
	/* --- End Sequence (All buffer data write down --- */
	if( pCtrl->fs_ctrl_flg == FJ_MUX_ON ){
		ret = bf_mp4_common_fs_write( file_id, buffer, count, D_BF_MUX_ON );
		if( ret != MP4_COMMON_OK ){
			return MP4_COMMON_NG;
		}
		return (LONG)count;
	}
	
	/* --- SDRAM FS momery + User Request --- */
	if( count > gMP4_Memory_Info.fs_bank_size ){
		/* Request size is already over. - SDRAM->request */
		/* SDRAM FS Memory */
		dummy_address = (UCHAR*)bf_mp4_common_fs_bank_get( pCtrl->fs_ch );
		ret = bf_mp4_common_fs_write( file_id, dummy_address, pCtrl->fs_memory_size, D_BF_MUX_OFF );
		if( ret != MP4_COMMON_OK ){
			return MP4_COMMON_NG;
		}
		
		pCtrl->fs_memory_size = 0;
		bf_mp4_common_fs_bank_change( pCtrl->fs_ch );
		
		/* Request */
		ret = bf_mp4_common_fs_write( file_id, buffer, count, D_BF_MUX_OFF );
		if( ret != MP4_COMMON_OK ){
			return MP4_COMMON_NG;
		}
		
		return (LONG)count;
	}
	
	/* --- No need to execute FS write. it keeps on SDRAM FS memory area --- */
	if( (pCtrl->fs_memory_size + count) < gMP4_Memory_Info.fs_bank_size ){
		/* In this case, no need to execute FS write. it keeps on SDRAM FS memory area. */
		dummy_address = (UCHAR*)bf_mp4_common_fs_bank_get( pCtrl->fs_ch );
		dummy_address += pCtrl->fs_memory_size ;

		if( count < D_MP4_COMMON_MEMCPY_SIZE ){
			bf_mp4_common_cpu_copy( buffer, dummy_address , count );
		}
		else{
			bf_mp4_common_dma_copy( D_MP4_COMMON_DMA_USE_CH, buffer, dummy_address, count );
		}
		pCtrl->fs_memory_size += count;
		return (LONG)count ;
	}
	
	/* --- Exeute FS write and Keep on SDRAM FS memory area  --- */
	/* SDRAM area are overflow -> Execute FS Write */
	dummy_address = (UCHAR*)bf_mp4_common_fs_bank_get(pCtrl->fs_ch);
	
	/* Copy to FS memory area */
	if(( gMP4_Memory_Info.fs_bank_size - pCtrl->fs_memory_size ) < D_MP4_COMMON_MEMCPY_SIZE ){
		bf_mp4_common_cpu_copy(
			(UCHAR*)buffer,
			(UCHAR*)(dummy_address + pCtrl->fs_memory_size),
			(gMP4_Memory_Info.fs_bank_size - pCtrl->fs_memory_size));
	}
	else{
		bf_mp4_common_dma_copy(
			D_MP4_COMMON_DMA_USE_CH,
			(UCHAR*)buffer,
			(UCHAR*)(dummy_address + pCtrl->fs_memory_size),
			(gMP4_Memory_Info.fs_bank_size - pCtrl->fs_memory_size) );
	}
	
	/* FS memory area write down to MEDIA */
	ret = bf_mp4_common_fs_write( file_id, dummy_address, gMP4_Memory_Info.fs_bank_size, D_BF_MUX_OFF );
	if( ret != MP4_COMMON_OK ){
		return MP4_COMMON_NG;
	}
	
	/* keep on FS memory area */
	bf_mp4_common_fs_bank_change( pCtrl->fs_ch );
	
	if( count == (gMP4_Memory_Info.fs_bank_size - pCtrl->fs_memory_size) ){
		pCtrl->fs_memory_size = 0 ;
	}
	else{
		dummy_address = (UCHAR*)bf_mp4_common_fs_bank_get(pCtrl->fs_ch);
		dummy_address2 = (UCHAR*)buffer;
		dummy_address2 += (gMP4_Memory_Info.fs_bank_size - pCtrl->fs_memory_size);
		size = count;
		size -= (gMP4_Memory_Info.fs_bank_size - pCtrl->fs_memory_size);
		if( size < D_MP4_COMMON_MEMCPY_SIZE ){
			bf_mp4_common_cpu_copy( (UCHAR*)dummy_address2, (UCHAR*)dummy_address, size );
		}
		else{
			bf_mp4_common_dma_copy( D_MP4_COMMON_DMA_USE_CH, (UCHAR*)dummy_address2, (UCHAR*)dummy_address, size );
		}
		pCtrl->fs_memory_size = size;
	}
	
	return (LONG)count;
}

/**
 *	@brief		Write down FS memory to MEDIA.\n
 *	@param		i:file_id	file Identifier
 *	@param		i:end_flg	End Flag
 *							FJ_MUX_OFF	Not End
 *							FJ_MUX_ON		End
 *	@return		None
 *	@note		None
 *	@attention	None
*/
INT32 BF_MP4_Common_Fs_Memory_Write_Down( INT32 file_id, UINT32 end_flg )
{
	UCHAR*	dummy;
	INT32	ret ;
	T_MP4_COMMON_FS_MEMORY_CTRL* pCtrl;

	pCtrl = bf_mp4_common_fs_memory_get_ctrl( file_id );
	if( pCtrl == NULL ) {
		return MP4_COMMON_NG;
	}
	
	ret = 0;
	if( pCtrl->fs_memory_size != 0 ){
		dummy = (UCHAR*)bf_mp4_common_fs_bank_get( pCtrl->fs_ch );
		
		ret = bf_mp4_common_fs_write( file_id, dummy, pCtrl->fs_memory_size, D_BF_MUX_ON );
		if( ret != MP4_COMMON_OK ){
			return MP4_COMMON_NG;
		}
		pCtrl->fs_memory_size = 0;
		bf_mp4_common_fs_bank_change( pCtrl->fs_ch );
	}
	
	if( end_flg == FJ_MUX_ON ){
		// Change to no using SDRAM mode
		pCtrl->fs_ctrl_flg = FJ_MUX_ON;
	}
	return ret;
}



/**
 *	@brief		Initialize File Fomat Memory for MUX.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID fj_mux_demux_common_memory_initialize( T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG* mem_config )
{
	static T_MP4_COMMON_MEMORY_INFO	mem_info;
	UCHAR	stream_ch;
	UCHAR	stream_max = 0;
	
	if( mem_config->header_mem_addr == 0 ){
		BF_Debug_Print_Error(("- fj_mux_demux_common_memory_initialize : header_mem_addr Error\n"));
		return;
	}
	if( mem_config->header_mem_size < D_MP4_COMMON_SDRAM_BANK_SIZE ){
		BF_Debug_Print_Error(("- fj_mux_demux_common_memory_initialize : header_mem_size Error\n"));
		return;
	}
	
	if( (mem_config->muxing_mem_addr != 0) && (mem_config->muxing_mem_size != 0) ){
		if( mem_config->muxing_mem_size < 4*1024*1024 ){
			BF_Debug_Print_Error(("- fj_mux_demux_common_memory_initialize : muxing_mem_size Error\n"));
			return;
		}
	}
	
	memset( &mem_info, 0, sizeof(mem_info) );
	
	mem_info.header_mem_addr	= mem_config->header_mem_addr;
	mem_info.header_mem_size	= mem_config->header_mem_size;
	
	if( (mem_config->muxing_mem_addr != 0) && (mem_config->muxing_mem_size != 0) ){
		mem_info.fs_bank_size		= mem_config->muxing_mem_size;
		mem_info.fs_bank_size		/= MP4_COMMON_FS_BANK_MAX_NUM;
		mem_info.fs_bank_size		/= mem_config->muxing_file_num;

		if(mem_config->muxing_file_num >= E_FJ_MUX_CH_MAX){
			stream_max = E_FJ_MUX_CH_MAX;
		}
		else{
			stream_max = mem_config->muxing_file_num;
		}
		
#if 0
		mem_info.fs_bank_addr[0][0]	= mem_config->muxing_mem_addr;
		mem_info.fs_bank_addr[0][1]	= mem_info.fs_bank_addr[0][0] + mem_info.fs_bank_size;
		mem_info.fs_bank_addr[0][2]	= mem_info.fs_bank_addr[0][1] + mem_info.fs_bank_size;
		mem_info.fs_bank_addr[0][3]	= mem_info.fs_bank_addr[0][2] + mem_info.fs_bank_size;
		
		if( mem_config->muxing_file_num > 1 ){
			mem_info.fs_bank_addr[1][0]	= mem_info.fs_bank_addr[0][3] + mem_info.fs_bank_size;
			mem_info.fs_bank_addr[1][1]	= mem_info.fs_bank_addr[1][0] + mem_info.fs_bank_size;
			mem_info.fs_bank_addr[1][2]	= mem_info.fs_bank_addr[1][1] + mem_info.fs_bank_size;
			mem_info.fs_bank_addr[1][3]	= mem_info.fs_bank_addr[1][2] + mem_info.fs_bank_size;
		}
#else
		for(stream_ch = 0; stream_ch < stream_max; stream_ch++ ){
			if(stream_ch == 0){
				mem_info.fs_bank_addr[stream_ch][0]	= mem_config->muxing_mem_addr;
			}
			else{
				mem_info.fs_bank_addr[stream_ch][0]	= mem_info.fs_bank_addr[stream_ch-1][3] + mem_info.fs_bank_size;
			}
			mem_info.fs_bank_addr[stream_ch][1]	= mem_info.fs_bank_addr[stream_ch][0] + mem_info.fs_bank_size;
			mem_info.fs_bank_addr[stream_ch][2]	= mem_info.fs_bank_addr[stream_ch][1] + mem_info.fs_bank_size;
			mem_info.fs_bank_addr[stream_ch][3]	= mem_info.fs_bank_addr[stream_ch][2] + mem_info.fs_bank_size;
		}
#endif
	}
	
	// Initialize Buffer Management Data for File Format
	BF_MP4_Common_Memory_Initialize( &mem_info );
	
	return;
}

/**
 *	@brief		Free All memory area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID fj_mux_demux_common_memory_finalize( VOID )
{
	BF_MP4_Common_Memory_Finalize();
	return;
}


/*************************************************************************************************/
/**********    User Method for MP4F File Format Library                                ***********/
/*************************************************************************************************/
/**
 *	@brief		Hunt the memory area.\n
 *	@param		i:size		Memory Size
 *	@return		Not equall NULL		Address of huntted area
 *	@return		NULL				Hunt NG
 *	@note		None
 *	@attention	MP4 File Format user has to define This Method.
 */
VOID *MP4F_malloc( ULONG size )
{
	INT32 *pointer;
	INT32 table_index;
	UINT8 setting_flg = FJ_MUX_OFF;
	UINT8 sdram_flag;
	
	if( size == 0 ){
		// input parameter Error
		BF_Debug_Print_Error(("Input parameter error in MP4F_malloc!\n"));
		return NULL;
	}
	
//	printf("malloc %ld\n", size);
	
	if( size > D_MP4_COMMON_FMT_THRESHOLD_NUM ){
		/******** Use SDRAM Area ********/
		bf_mp4_common_sdram_memory_ctrl( D_MP4_COMMON_MALLOC, size, (VOID**)&pointer );
		sdram_flag = FJ_MUX_ON;
	}
	else{
		/******** Use System MPL Area ********/
		pointer = (INT32*)bf_mp4_common_system_malloc( size );
		sdram_flag = FJ_MUX_OFF;
	}
	
	if( pointer != NULL ){
		for( table_index=0; table_index<D_MP4_COMMON_FMT_MALLOC_TBL_NUM; table_index++ ){
			if( gMP4_Malloc_Tbl[table_index].buf_pointer == NULL ){
				gMP4_Malloc_Tbl[table_index].buf_pointer	= pointer;
				gMP4_Malloc_Tbl[table_index].buf_size		= size;
				gMP4_Malloc_Tbl[table_index].sdram_flg		= sdram_flag;
				setting_flg = FJ_MUX_ON;

//				printf("ma:%d\n", table_index);

				break;
			}
		}
		if( setting_flg == FJ_MUX_OFF ){
			/* We Can not to do buffer management.
			   But we will not be buffer error, however, in this case,
			   user can not reallocate new buffer. */
			BF_Debug_Print_Error(("MP4 Buffer Management Table FULL!!\n"));
		}
	}
	else{
		BF_Debug_Print_Error(("CAN NOT ALLOCATE NEW MEMORY.\n"));
	}
	return pointer;
}

/**
 *	@brief		Change the memory area size.\n
 *	@param		i:pointer	Memory Area Address(OLD)
 *	@param		i:size		Memory Size
 *	@return		Not equall NULL		Address of huntted area
 *	@return		NULL				Hunt NG
 *	@note		None
 *	@attention	MP4 File Format user has to define This Method.\n
 *				If pointer is NULL, system treats as malloc request.
 */
VOID *MP4F_realloc( VOID *pointer, ULONG size )
{
	INT32 *new_pointer;
	INT32 table_index;
	ULONG old_size=0;
	BYTE  old_sdram=0;
	UINT8 sdram_flag;
	
//	printf("realloc %ld\n", size);
	
	if( size == 0 ){
		// Input parameter error
		BF_Debug_Print_Error(("Input parameter error in MP4 Buffer malloc!\n"));
		return NULL;
	}
	
	if( pointer == NULL ){
		// It same as malloc request.
		new_pointer = (INT32*)MP4F_malloc( size );
		return new_pointer;
	}
	
	// Serach Buffer Management Table
	for( table_index=0; table_index<D_MP4_COMMON_FMT_MALLOC_TBL_NUM; table_index++ ){
		// serach old memory map
		if( gMP4_Malloc_Tbl[table_index].buf_pointer == pointer ){
			old_size  = gMP4_Malloc_Tbl[table_index].buf_size;
			old_sdram = gMP4_Malloc_Tbl[table_index].sdram_flg;
			
			if( old_size == 0 ){
				// Table Broken. Initialize Buffer management table.
				BF_Debug_Print_Information(("old_size=0\n"));
				gMP4_Malloc_Tbl[table_index].buf_pointer	= NULL;
				gMP4_Malloc_Tbl[table_index].sdram_flg		= 0;
			}
			break;
		}
	}
	if( old_size == 0 ){
		// System can not control Buffer Management
		BF_Debug_Print_Error(("System can not control MP4 buffer management!\n"));
		return NULL;
	}
	if( old_size == size ){
		// request size is same as old size
		BF_Debug_Print_Error(("Request size is same as old size in MP4 realloc.\n"));
		return pointer;
	}
	
	if( old_size > size ){
		// request size is small as old size
		BF_Debug_Print_Error(("Request size is small as old size in MP4 realloc.\n"));
		return pointer;
	}
	
	if( old_sdram == FJ_MUX_ON ){
		/**** SDRAM->SDRAM ****/
		// now MP4Lib is not use route
		new_pointer = pointer;
		bf_mp4_common_sdram_memory_ctrl( D_MP4_COMMON_REALLOC, size, (VOID**)&new_pointer );
		if( new_pointer == NULL ){
			BF_Debug_Print_Error(("System can not reallocate new memory!\n"));
			return NULL;
		}
		// sdram_flg and buf_pointer are not changed
		gMP4_Malloc_Tbl[table_index].buf_size = size;
		return new_pointer;
	}
	else{
		if( size > D_MP4_COMMON_FMT_THRESHOLD_NUM ){
			/**** System memory->SDRAM ****/
			// now MP4Lib is not use route
			bf_mp4_common_sdram_memory_ctrl( D_MP4_COMMON_MALLOC, size, (VOID**)&new_pointer);
			sdram_flag = FJ_MUX_ON;
		}
		else{
			/**** System memory->System memory ****/
			new_pointer = (INT32*)bf_mp4_common_system_malloc( size );
			sdram_flag = FJ_MUX_OFF;
		}
		
		if( new_pointer != NULL ){
			// copy old memory -> new memory
			memcpy( new_pointer, pointer, old_size );
			// old memory free
			bf_mp4_common_system_free( pointer );
			
			// Update Buffer Management Table
			gMP4_Malloc_Tbl[table_index].buf_pointer	= new_pointer;
			gMP4_Malloc_Tbl[table_index].buf_size		= size;
			gMP4_Malloc_Tbl[table_index].sdram_flg		= sdram_flag;
		}
		else{
			BF_Debug_Print_Error(("System can not reallocate new memory.\n"));
		}
		
		return new_pointer;
	}
}

/**
 *	@brief		Free memory area.\n
 *	@param		i:pointer	Memory Area Address
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID MP4F_free( VOID *pointer )
{
	INT32	table_index;
	
//	printf("free \n");
	
	if( pointer == NULL ){
		return;
	}
	
	for( table_index=0; table_index<D_MP4_COMMON_FMT_MALLOC_TBL_NUM; table_index++ ){
		if( gMP4_Malloc_Tbl[table_index].buf_pointer == pointer ){
			if( gMP4_Malloc_Tbl[table_index].sdram_flg == FJ_MUX_ON ){
				// SDRAM Area
				bf_mp4_common_sdram_memory_ctrl( D_MP4_COMMON_FREE,0,&pointer );
			}
			else{
				// System Memory
				bf_mp4_common_system_free( pointer );
			}
			gMP4_Malloc_Tbl[table_index].buf_pointer	= NULL;
			gMP4_Malloc_Tbl[table_index].buf_size		= 0;
			gMP4_Malloc_Tbl[table_index].sdram_flg		= FJ_MUX_OFF;
			break;
		}
	}
	return;
}

/**
 *	@brief		Get Current Time.\n
 *	@param		i:creation_time		creation time (for mvhd,tkhd,mdhd atom)
 *	@param		i:modification_time	modification time (for mvhd,tkhd,mdhd atom)
 *	@return		Time		Current Time
 *	@note		None
 *	@attention	None
 */
ULONG MP4F_time( ULONG* creation_time, ULONG* modification_time )
{
	T_MP4_COMMON_DATE		date_info;
	ULONG					mp4_time;
	
	bf_mp4_common_date_get( &date_info, E_MP4_COMMON_DATA_TIMING_CREATE );
	bf_mp4_common_time_convert( E_MP4_COMMON_SYSTEM_TO_MP4, &date_info, &mp4_time );
	*creation_time = mp4_time;
	*modification_time = mp4_time;
	
	return 0;
}

/**
 *	@brief		Change File Position.\n
 *	@param		i:fd		File Descripter
 *	@param		i:offset	offset from whence
 *	@param		i:whence	Based Position\n
 *							0 : top position\n
 *							1 : current position\n
 *							2 : last position\n
 *	@return		position	File position(Number of Byte from File top)
 *	@return		-1			Faulure
 *	@note		None
 *	@attention	None
 */
LONG MP4F_lseek( INT32 fd, ULONG offset, INT32 whence, ULONG* position )
{
	FJ_ERR_CODE	ret;
	USHORT	fj_whence;
	INT64	position64;
	
	if( position == NULL ){
		BF_Debug_Print_Error(("MP4F_lseek : parameter(position) error!\n"));
		return -1;
	}
	
	BF_MP4_Common_Fs_Memory_Write_Down( fd, FJ_MUX_OFF );
	
	switch( whence ){
		case 0 :	// SEEK_SET
			fj_whence = FJ_FS_SEEK_SET;
			break;
		case 1 :	// SEEK_CUR
			fj_whence = FJ_FS_SEEK_CUR;
			break;
		case 2 :	// SEEK_END
			fj_whence = FJ_FS_SEEK_END;
			break;
		default :
			BF_Debug_Print_Error(("MP4F_lseek : parameter(whence) error!\n"));
			return -1;
	}

	ret = FJ_Fs_Lseek64( fd, offset, fj_whence, &position64 );
	if( ret != FJ_ERR_OK ){
		BF_Debug_Print_Error(("MP4F_lseek: FJ_Fs_Lseek error!!\n"));
		return -1;
	}
	
	*position = (ULONG)position64;
	
	return 0;
}

/**
*	@brief		Read Data(header) From MEDIA.\n
 *	@param		i:fd		File Descripter
 *	@param		i:buffer	Read area buffer pointer
 *	@param		i:count		Read Size(Request)
 *	@return		Read Size(Result)	Number of reading size(Byte)
 *	@return		-1			Faulure
 *	@note		None
 *	@attention	None
 */
LONG MP4F_read_header( INT32 fd, VOID *buffer, ULONG count )
{
	ULONG read_size;
	
	read_size = 0;
	
	read_size = (ULONG)bf_mp4_common_fs_read( fd, buffer, count );
	if( count != read_size ){
		BF_Debug_Print_Error(("bf_mp4_common_fs_read. count=%lu,read_size=%lu\n", count, read_size));
	}
	
	return (LONG)read_size;
}


/**
*	@brief		Read Data(sample) From MEDIA(32byte alignment read).\n
 *	@param		i:fd		File Descripter
 *	@param		i:buffer	Read area buffer pointer
*	@param		o:buffer_adjustment	Read area buffer pointer(32byte alignment)
 *	@param		i:count		Read Size(Request)
 *	@return		Read Size(Result)	Number of reading size(Byte)
 *	@return		-1			Faulure
 *	@note		None
 *	@attention	None
 */
LONG MP4F_read_sample( INT32 fd, VOID *buffer, VOID **buffer_adjustment, ULONG count )
{
	ULONG read_size;
	
	read_size = 0;
	
	read_size = (ULONG)bf_mp4_common_fs_read( fd, buffer, count );
	if( count != read_size ){
		BF_Debug_Print_Error(("bf_mp4_common_fs_read. count=%lu,read_size=%lu\n", count, read_size));
	}
	
	return (LONG)read_size;
}


/**
 *	@brief		Write Data to MEDIA.\n
 *	@param		i:fd		File Descripter
 *	@param		i:buffer	write data buffer pointer
 *	@param		i:count		write Size(Request)
 *	@return		Writing Size(Result)	Number of writing size(Byte)
 *	@return		-1						Faulure
 *	@note		None
 *	@attention	None
 */
LONG MP4F_write( INT32 fd, const VOID *buffer, ULONG count )
{
	ULONG	write_size;
	
	write_size = (ULONG)BF_MP4_Common_Fs_Memory_Write( fd, buffer, count );
	if( write_size != count ){
		BF_Debug_Print_Error(("count=%lu,write_size=%lu\n",count, write_size ));
		return -1;
	}
	return (LONG)count;
}

