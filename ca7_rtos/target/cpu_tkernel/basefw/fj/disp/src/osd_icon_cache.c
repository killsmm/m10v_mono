/**
 * @file 		osd_icon_cache.c
 * @brief		Cache for resource data
 * @details
 * - A linked list implementation of Least Recently Used (LRU) data cache 
 * delicated for caching per-icon RELC decompressed icon data. 
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_FIRST_INIT"
//#pragma arm section rodata="BASEFW_FIRST_CONST"
//#pragma arm section code="BASEFW_FIRST_CODE"

#include "fj_custom.h"	// enable BF_Debug_Print
#include "osd_icon_comm.h"
#include "osd_general.h"
#include "osd_icon_cache.h"
//#include "decompress.h"
#include "TKernel_id.h"
//#include "mba_if.h"
#include <stdlib.h>

#define M_MBA_PRINTF(arg)
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
 *  Maximum total number of allocatable cache memory
 *  @note 
 *  - this size should *synchronize* with the actual size of the variable-size memorypool
 */
#define MAX_CACHE_SIZE 				(SDRAM_SIZ_ICON_UNCOMPRESS_CACHE) // 3M // the heap should be 3M larger
#define D_OSD_ICON_CACHE_ICON_NUM	(1200)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	INT32 tableID_iconID;
	ULONG iconAddr;
	ULONG size;
} T_OSD_ICON_CACHE;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static T_OSD_ICON_CACHE gOsd_Icon_Cache_Address_List[D_OSD_ICON_CACHE_ICON_NUM];
static USHORT gOsd_Icon_Cache_Address_List_Current_Counter = 0;
static UINT32 gOsd_Icon_Cache_Used_Size =0;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//#pragma arm section rwdata="BASEFW_FIRSTB_INIT"
//#pragma arm section rodata="BASEFW_FIRSTB_CONST"
//#pragma arm section code="BASEFW_FIRSTB_CODE"
/**
Hunt memory from memory pool
@param[in]	size	buffer size
@return		buffer address
*/
static VOID * icon_buffer_alloc(INT32 size)
{
	// Variable-size memory pool allocation
	OS_USER_ER ercd;
	OS_USER_VP blk;

//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "B:icon_buffer_alloc" ));

	ercd = OS_User_Pget_Mpl( MPKID_ICONCACHE, size, &blk);
	if(ercd != D_OS_USER_E_OK){
		//T_RMPL k_rmpl;
		//if( ref_mpl(MPKID_ICONCACHE,&k_rmpl) == E_OK){
		//	BF_Debug_Print_Information(("Hunt size=%u,Free size=%u,Total Free=%u, UsedSize=%u\n", size,k_rmpl.fblksz,k_rmpl.fmplsz,gOsd_Icon_Cache_Used_Size));
		//}
		blk = NULL;
	}
	else{
		gOsd_Icon_Cache_Used_Size += size;
	}
	
	return blk;
}

//#pragma arm section rwdata="BASEFW_SECOND_INIT"
//#pragma arm section rodata="BASEFW_SECOND_CONST"
//#pragma arm section code="BASEFW_SECOND_CODE"
/**
Free memory to memory pool
@param[in]	addr	buffer address
@param[in]	size	buffer size
*/
static VOID icon_buffer_free(VOID * addr, UINT32 size)
{
//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "2:icon_buffer_free" ));

	if (addr){
		if (OS_User_Rel_Mpl(MPKID_ICONCACHE, addr) != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("icon_buffer_free failed(%08lX)!!\n", (ULONG)addr));
		}
		else{
			gOsd_Icon_Cache_Used_Size -= size;
		}
	}
}


//#pragma arm section rwdata="BASEFW_SECOND_INIT"
//#pragma arm section rodata="BASEFW_SECOND_CONST"
//#pragma arm section code="BASEFW_SECOND_CODE"
/**
Compare 2 T_OSD_ICON_CACHE's object for sort and search
@param[in]	a	icon cache table
@param[in]	b	icon cache table
@return		compare result
*/
static int  Osd_Icon_Cache_Compare_T_OSD_ICON_CACHE(const void* a, const void* b)
{
	T_OSD_ICON_CACHE* data1 =(T_OSD_ICON_CACHE*)a;
	T_OSD_ICON_CACHE* data2 =(T_OSD_ICON_CACHE*)b;

//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "2:e_T_OSD_ICON_CACHE" ));

	return (data1->tableID_iconID - data2->tableID_iconID);
}

//#pragma arm section rwdata="BASEFW_SECOND_INIT"
//#pragma arm section rodata="BASEFW_SECOND_CONST"
//#pragma arm section code="BASEFW_SECOND_CODE"
/**
Compare 2 T_OSD_ICON_CACHE's by tableID for search
@param[in]	a	icon cache table
@param[in]	b	icon cache table
@return		compare result
*/
static int  osd_icon_cache_compare_t_osd_icon_cache_search_first(const void* a, const void* b)
{
	T_OSD_ICON_CACHE* data1 =(T_OSD_ICON_CACHE*)a;
	T_OSD_ICON_CACHE* data2 =(T_OSD_ICON_CACHE*)b;
	
//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "2:cache_search_first" ));

	return ((data1->tableID_iconID >>16) - (data2->tableID_iconID>>16));
}

//#pragma arm section rwdata="BASEFW_SECOND_INIT"
//#pragma arm section rodata="BASEFW_SECOND_CONST"
//#pragma arm section code="BASEFW_SECOND_CODE"
/**
Remove the tail of the icon cache. Release the memory held
by last icon cache linked-list node and its icon buffer.
@return		the removed node; NULL if no node is removed
*/
static USHORT remove_tail(VOID)
{
//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "2:S remove_tail" ));

	if(gOsd_Icon_Cache_Address_List_Current_Counter > 0) {
		//Can be delete
		gOsd_Icon_Cache_Address_List_Current_Counter--;
		icon_buffer_free((BYTE *)(gOsd_Icon_Cache_Address_List[gOsd_Icon_Cache_Address_List_Current_Counter].iconAddr),
									gOsd_Icon_Cache_Address_List[gOsd_Icon_Cache_Address_List_Current_Counter].size);
	}
	return gOsd_Icon_Cache_Address_List_Current_Counter;
}

//#pragma arm section rwdata="BASEFW_FIRSTB_INIT"
//#pragma arm section rodata="BASEFW_FIRSTB_CONST"
//#pragma arm section code="BASEFW_FIRSTB_CODE"
/**
Search the icon from cache list.
@param[in]	icon_infor	Icon
@return		The index of gOsd_Icon_Cache_Address_List.
*/
static VOID Osd_Icon_Cache_Insert_icon(const T_OSD_ICON_CACHE* icon_infor)
{
//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "B:_Cache_Insert_icon" ));

	if(gOsd_Icon_Cache_Address_List_Current_Counter < (D_OSD_ICON_CACHE_ICON_NUM -1)) {
		//Can be inserted
		gOsd_Icon_Cache_Address_List[gOsd_Icon_Cache_Address_List_Current_Counter] = *icon_infor;
	}
	else{
		//Manage list is full
		//Remove the last element
		remove_tail();
		gOsd_Icon_Cache_Address_List[gOsd_Icon_Cache_Address_List_Current_Counter] = *icon_infor;
	}
	gOsd_Icon_Cache_Address_List_Current_Counter++;
	//Sort
	qsort(gOsd_Icon_Cache_Address_List, gOsd_Icon_Cache_Address_List_Current_Counter, sizeof(T_OSD_ICON_CACHE), Osd_Icon_Cache_Compare_T_OSD_ICON_CACHE);
}


//#pragma arm section rwdata="BASEFW_FIRSTB_INIT"
//#pragma arm section rodata="BASEFW_FIRSTB_CONST"
//#pragma arm section code="BASEFW_FIRSTB_CODE"
/**
Uncompress the icon data.
@param[in/out]	uncompress_infor	Icon
@param[in]		bufferAddr		Icon address compressed
*/
static VOID osd_icon_cache_load_icon(const T_BF_OSD_ICON_COMM_PARAMETER4CACHE* uncompress_infor, ULONG bufferAddr)
{
#if 0
	INT32 err;
	BYTE * iconList = (BYTE *)uncompress_infor->iconList;
	// run-time decode to tmp buffer
	// Get encoded data address and size
	BYTE * dataStartAddr = iconList + uncompress_infor->iconList[uncompress_infor->IconKind_id.short_data.iconID].mIcon_offset;

//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "B:on cache_load_icon" ));

	err = Decompress(dataStartAddr, uncompress_infor->compressed_size, (BYTE *)bufferAddr,uncompress_infor->uncompressed_size);
	if( err	!=0) {
		BF_Debug_Print_Error(("Icon uncompress error (ErrCode=%d,TableID=%d, IconID=%d)\n", err,uncompress_infor->IconKind_id.short_data.tableID, uncompress_infor->IconKind_id.short_data.iconID));
	}
//	if(fj_run_relc(dataStartAddr, (BYTE *)bufferAddr, bufferSize) <= 0){
#ifdef DEBUG_ICON_CACHE
	BF_Debug_Print_Information(("OSD Draw: error run-time decode icon (TableID=%d, IconID=%d)\n", uncompress_infor->IconKind_id.short_data.tableID, uncompress_infor->IconKind_id.short_data.iconID));
#endif
#endif
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//#pragma arm section rwdata="BASEFW_FIRSTB_INIT"
//#pragma arm section rodata="BASEFW_FIRSTB_CONST"
//#pragma arm section code="BASEFW_FIRSTB_CODE"
VOID BF_Osd_Icon_Cache_Clear_Nodes(USHORT tableID)
{

	T_OSD_ICON_CACHE* match_icon_data = NULL;
	T_OSD_ICON_CACHE icon_data = {
		.tableID_iconID = (tableID<<16),
		.iconAddr = 0,
		.size = 0
	};

//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "B: Cache_Clear_Nodes" ));

	if(gOsd_Icon_Cache_Address_List_Current_Counter > 0) {
		match_icon_data = (T_OSD_ICON_CACHE*) bsearch(&icon_data, gOsd_Icon_Cache_Address_List, gOsd_Icon_Cache_Address_List_Current_Counter, sizeof(T_OSD_ICON_CACHE), osd_icon_cache_compare_t_osd_icon_cache_search_first);
		if(match_icon_data) {
			USHORT delete_counter = 0;
			T_OSD_ICON_CACHE* match_icon_data_save =  match_icon_data;
			T_OSD_ICON_CACHE* end_address = &gOsd_Icon_Cache_Address_List[gOsd_Icon_Cache_Address_List_Current_Counter-1];
			T_OSD_ICON_CACHE* move_item_start; //Move item begin index after delete
			T_OSD_ICON_CACHE* move_item_to;//move item destination 
			while((match_icon_data->tableID_iconID>>16) == tableID && match_icon_data <= end_address){
				icon_buffer_free((BYTE *)(match_icon_data->iconAddr), match_icon_data->size);
//				match_icon_data->tableID_iconID = 0x4FFFFFFF;//Set to MAX value
				match_icon_data++;
				delete_counter++;
			}
			move_item_start = match_icon_data;
			match_icon_data =  match_icon_data_save;
			match_icon_data--;
			end_address = &gOsd_Icon_Cache_Address_List[0];
			while((match_icon_data->tableID_iconID>>16) == tableID && match_icon_data>= end_address){
				icon_buffer_free((BYTE *)(match_icon_data->iconAddr), match_icon_data->size);
//				match_icon_data->tableID_iconID = 0x4FFFFFFF;//Set to MAX value
				match_icon_data--;
				delete_counter++;
			}
			move_item_to =  match_icon_data;
			move_item_to++;
		//Sort
			//qsort(gOsd_Icon_Cache_Address_List,
			//gOsd_Icon_Cache_Address_List_Current_Counter,
			//sizeof(T_OSD_ICON_CACHE),
			//Osd_Icon_Cache_Compare_T_OSD_ICON_CACHE);
			//Move the data to deletion position
			end_address = &gOsd_Icon_Cache_Address_List[gOsd_Icon_Cache_Address_List_Current_Counter];
			while(move_item_start < end_address) {
				*move_item_to = *move_item_start;
				move_item_to++;
				move_item_start++;
			}
			gOsd_Icon_Cache_Address_List_Current_Counter -=delete_counter;
		}
	}
}


//#pragma arm section rwdata="BASEFW_FIRSTB_INIT"
//#pragma arm section rodata="BASEFW_FIRSTB_CONST"
//#pragma arm section code="BASEFW_FIRSTB_CODE"
CHAR* BF_Osd_Icon_Cache_Get_icon(const T_BF_OSD_ICON_COMM_PARAMETER4CACHE* uncompress_infor)
{
	T_OSD_ICON_CACHE icon_data ={
		.tableID_iconID = uncompress_infor->IconKind_id.tableID_iconID,
		.iconAddr = 0,
		.size = 0
	};
	T_OSD_ICON_CACHE* match_icon_data = NULL;
	CHAR *ret = NULL;

	//M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "B:con_Cache_Get_icon" ));

	if(gOsd_Icon_Cache_Address_List_Current_Counter > 0) {
		match_icon_data = (T_OSD_ICON_CACHE*) bsearch(&icon_data, gOsd_Icon_Cache_Address_List, gOsd_Icon_Cache_Address_List_Current_Counter, sizeof(T_OSD_ICON_CACHE), Osd_Icon_Cache_Compare_T_OSD_ICON_CACHE);
	}
	if(match_icon_data == NULL){
		// cache miss
		match_icon_data = &icon_data;
		while((match_icon_data->iconAddr = (ULONG)icon_buffer_alloc(uncompress_infor->uncompressed_size)) == 0){ //pgr0689: No problem as match_icon_data->iconAddr can be NULL
			if( remove_tail() == 0) {
				//No space 
				BF_Debug_Print_Information(("No space to uncompress. size=%u)\n", uncompress_infor->uncompressed_size));
				break;
			}
		}
		if(match_icon_data->iconAddr !=0) {
			//Hunt memory success
			match_icon_data->size = uncompress_infor->uncompressed_size;
			Osd_Icon_Cache_Insert_icon(match_icon_data);
			osd_icon_cache_load_icon(uncompress_infor, match_icon_data->iconAddr);
		}
	}
	
	//	Check the pointer if it's NULL. No problem for pgr0689
	ret = (CHAR *)match_icon_data->iconAddr;
	return ret;
}


