/**
 * @file		osd_icon_cache.h
 * @brief		Cache for resource data
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_ICON_CACHE_H_
#define _OSD_ICON_CACHE_H_

#include "osd_icon_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Clear all nodes belonging to the specific table in the Icon Cache.
@param[in]	tableID	the table ID of the specific table
*/
extern VOID BF_Osd_Icon_Cache_Clear_Nodes(USHORT tableID);

/**
Get the address of the icon data buffer.<br>
If the target icon data buffer is in cache, its address will be returned.<br>
If it is not in cache, it is RELC decompressed and moved into the cache.<br>
Its address in the cache will then be returned. <br>
@param[in]	uncompress_infor	icon parameter
@return		the address to the icon data
*/
extern CHAR * BF_Osd_Icon_Cache_Get_icon(const T_BF_OSD_ICON_COMM_PARAMETER4CACHE* uncompress_infor);


#endif//_OSD_ICON_CACHE_H_
