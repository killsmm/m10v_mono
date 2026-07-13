/**
 * @file		fj_memory.h
 * @brief		Memory Control API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FJ_MEMORY_H
#define _FJ_MEMORY_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "os_user_custom.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_memory_overview
@{
This chapter describes API related to memory control.<BR>
	- Function <br>
	. SDRAM map mode <br>
	. memory pool <br>
	. memory location pointer <br>
	. cache <br>
	. memory copy using by DMA <br>
   
	- About SDRAM Map <br>
		You can see the details of default memory allocation of the SDK with RK board as follows. <br>
        <br>
		    .target\cpu_tkernel\board\sdram\map\xls\SDRAM_MAP_Generate_Tool_CMOS.xlsb <br>
        <br>
		<b>[ Attention ]</b>
		<b>Memory Maps are under consideration yet.</b>
   
	- Please see following tables about the releationshiop between SDRAM Map Mode and SDRAM Map sheet.<br>

	<table>
		<tr>
			<th>FJ_SDRAM_MAP_MODE</th>
			<th>VIEW_MAP_IDX</th>
			<th>MOVIE_RECORD_MAP_IDX</th>
			<th>MXIC_SDI</th>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_VIDEO_0</td>
			<td>1</td>
			<td>0</td>
			<td>0</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_VIDEO_1</td>
			<td>0</td>
			<td>0</td>
			<td>5</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_VIDEO_2</td>
			<td>1</td>
			<td>0</td>
			<td>6</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_VIDEO_H264_ONLY</td>
			<td>0</td>
			<td>1</td>
			<td>0</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_PHOTO</td>
			<td>5</td>
			<td>0</td>
			<td>1</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_BURST</td>
			<td>6</td>
			<td>0</td>
			<td>1</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_PIV</td>
			<td>2</td>
			<td>0</td>
			<td>1</td>
		</tr>
		<tr>
			<td>@ref FJ_SDRAM_MAP_MODE_STITCH</td>
			<td>7</td>
			<td>0</td>
			<td>7</td>
		</tr>

	</table>
    <br>
    <table>
		<tr>
			<th bgcolor="#d3d3d3">VIEW_MAP_IDX</th>
			<th bgcolor="#d3d3d3">Sheet Name</th>
		</tr>
		<tr>
			<td>0</td>
			<td>VideoMode_4K2K</td>
		</tr>
		<tr>
			<td>1</td>
			<td>VideoMode_NORMAL</td>
		</tr>
		<tr>
			<td>2</td>
			<td>VideoMode_PIV</td>
		</tr>
		<tr>
			<td>3</td>
			<td>-</td>
		</tr>
		<tr>
			<td>4</td>
			<td>-</td>
		</tr>
		<tr>
			<td>5</td>
			<td>Still_12M</td>
		</tr>
		<tr>
			<td>6</td>
			<td>Still_Burst</td>
		</tr>
		<tr>
			<td>7</td>
			<td>VideoMode_Stitch</td>
		</tr>
	</table>
    <br>
    <table>
		<tr>
			<th bgcolor="#d3d3d3">MOVIE_RECORD_MAP_IDX</th>
			<th bgcolor="#d3d3d3">Sheet Name</th>
		</tr>
		<tr>
			<td>0</td>
			<td>Rec_Pattern0</td>
		</tr>
		<tr>
			<td>1</td>
			<td>Rec_Pattern1</td>
		</tr>
	</table>

@}*//* --- end of fj_memory_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_memory_sequence
@{
@}*//* --- end of fj_memory_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_memory_definition
@{*/

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/** SDRAM MAP Mode */
typedef enum _FJ_SDRAM_MAP_MODE {
	FJ_SDRAM_MAP_MODE_VIDEO_0					= 0,	/**< Set Video Mode(Normal Mode) */
	FJ_SDRAM_MAP_MODE_VIDEO_1,							/**< Set Video mode(High performance mode) */
	FJ_SDRAM_MAP_MODE_VIDEO_H264_ONLY,					/**< Set Video Mode(H264 Only Mode) */
	FJ_SDRAM_MAP_MODE_PHOTO,							/**< Set Photo(Still Capture) Mode */
	FJ_SDRAM_MAP_MODE_BURST,							/**< Set Burst Mode */
	FJ_SDRAM_MAP_MODE_STILL_HDR,						/**< Set Still HDR  Mode */
	FJ_SDRAM_MAP_MODE_PB,								/**< Set PlayBack Mode */
	FJ_SDRAM_MAP_MODE_SETUP,							/**< Set Setup Mode */
	FJ_SDRAM_MAP_MODE_LIVE,								/**< Set Live view mode */
	FJ_SDRAM_MAP_MODE_PIV,								/**< Set Picture In Video mode */
	FJ_SDRAM_MAP_MODE_VIDEO_2,							/**< Set Video Mode(Normal Mode + Special Arbitration) */
	FJ_SDRAM_MAP_MODE_STITCH,							/**< Set Stitch mode */
	FJ_SDRAM_MAP_MODE_STILL_STITCH,						/**< Set Still Stitch Mode */
	FJ_SDRAM_MAP_MODE_MULTI_OUT,						/**< Set Multi Out(2sensor 2stream output) mode */
	FJ_SDRAM_MAP_MODE_MAX
} FJ_SDRAM_MAP_MODE;

/** Memory Location */
typedef enum _FJ_MEMORY_LOCATION {
	FJ_HOST_GENERIC_MEM_LOCATION			= 0,	/**< generic heap for HOST. On cache-on area. */
	FJ_HOST_LCD_OSD_FRAME_MEM_LOCATION			,	/**< OSD frame buffer of LCD for HOST. On cache-on area. */
	FJ_HOST_HDMI_OSD_FRAME_MEM_LOCATION			,	/**< OSD frame buffer of HDMI for HOST. On cache-on area. */
	FJ_HOST_MUXER_INDEX_BUF_STREAM1_MEM_LOCATION,	/**< muxer index buffer (1st stream) for HOST. On cache-on area. */
	FJ_HOST_MUXER_INDEX_BUF_STREAM2_MEM_LOCATION,	/**< muxer index buffer (2nd stream) for HOST. On cache-on area. */
	FJ_HOST_VIDEO_DFS_MEM_LOCATION				,	/**< video dfs buffer for HOST. On cache-on area. */
	FJ_HOST_STILL_DFS_MEM_LOCATION				,	/**< still dfs buffer for HOST. On cache-on area. */
	FJ_HOST_JPEG_DEMUXER_READ_MEM_LOCATION		,	/**< JPEG Demuxer read buffer for HOST. On cache-on area. */
	FJ_HOST_MP4_DEMUXER_READ_MEM_LOCATION		,	/**< MP4 Demuxer read buffer for HOST. On cache-on area. */
	FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION		,	/**< MP4 Demuxer parsed buffer for HOST. On cache-on area. */
	FJ_HOST_WAV_READ_MEM_LOCATION				,	/**< WAV read buffer for HOST. On cache-on area. */
	FJ_HOST_ENCODED_BITS_BUFFER_MEM_LOCATION	,	/**< Encoded Bits buffer for HOST. On cache-on area. */
	FJ_HOST_DECODED_YUV_MAIN_MEM_LOCATION		,	/**< Decoded YUV Main buffer for HOST. On cache-on area. */
	FJ_HOST_DECODED_YUV_THUMBNAIL_MEM_LOCATION	,	/**< Decoded YUV Thumbnail buffer for HOST. On cache-on area. */
	FJ_STITCH_WORKAREA_MEM_LOCATION,				/**< Set Stitch WORK MEMORY LOCATION */
	FJ_HOST_PFC_WORKAREA_MEM_LOCATION,				/**< Set PFC WORK MEMORY LOCATION */
	FJ_MEMORY_LOCATION_MAX
} FJ_MEMORY_LOCATION;

/** Memory Attribute */
typedef union {
    UINT32	word;
	struct{
		UINT32	l1_cache			: 1;			/**< Set L1 CACHEABLE FLAG */
		UINT32	l2_cache			: 1;			/**< Set L2 CACHEABLE FLAG */
		UINT32	strongly_ordered	: 1;			/**< Set STRONGLY ORDERED MEMORY FLAG */
		UINT32	device				: 1;			/**< Set DEVICE MEMORY FLAG */
		UINT32	normal				: 1;			/**< Set NORMAL MEMORY FLAG */
		UINT32	access_permission	: 2;			/**< Set ACCESS PERMISSION (0:Faults, 1:Read Only, 2:Full Access) */
		UINT32	shared				: 1;			/**< Set SHARED MEMORY (0:Non Shared, 1:Shared) */
		UINT32	dummy1				: 27;			/**< Reserved */
	}bit;
} U_MEMORY_ATTRIBUTE;

/** Cache Level */
typedef enum _FJ_MEMORY_CACHE_LEVEL {
	FJ_MEMORY_CACHE_L1 				= 0,			/**< Cache Level 1 */
	FJ_MEMORY_CACHE_L2 					,			/**< Cache Level 2 */
	FJ_MEMORY_CACHE_L1_L2				,			/**< Cache Level 1 and Level2 */
	FJ_MEMORY_CACHE_LEVEL_MAX
} FJ_MEMORY_CACHE_LEVEL;

/** Cache Operation */
typedef enum _FJ_MEMORY_CACHE_OPE {
	FJ_MEMORY_CACHE_FLUSH			= 0,			/**< Cache Operation Flush */
	FJ_MEMORY_CACHE_CLEAN				,			/**< Cache Operation Clean */
	FJ_MEMORY_CACHE_CLEAN_FLUSH			,			/**< Cache Operation Clean and Flush */
	FJ_MEMORY_CACHE_OPE_MAX
} FJ_MEMORY_CACHE_OPE;

/** Memory Copy of DMA Channel */
typedef enum _FJ_MEMORY_COPY_DMA_CH {
	FJ_MEMORY_COPY_HDMAC_CH_0 		= 0x00010000,	/**< External Storage HDMAC Channel 0 */
	FJ_MEMORY_COPY_HDMAC_CH_1 					,	/**< External Storage HDMAC Channel 1 */
	FJ_MEMORY_COPY_HDMAC_CH_2 					,	/**< External Storage HDMAC Channel 2 */
	FJ_MEMORY_COPY_HDMAC_CH_3 					,	/**< External Storage HDMAC Channel 3 */
	FJ_MEMORY_COPY_HDMAC0_CH_0 		= 0x00020000,	/**< Peripherals HDMAC0 Channel 0 */
	FJ_MEMORY_COPY_HDMAC0_CH_1 					,	/**< Peripherals HDMAC0 Channel 1 */
	FJ_MEMORY_COPY_HDMAC0_CH_2 					,	/**< Peripherals HDMAC0 Channel 2 */
	FJ_MEMORY_COPY_HDMAC0_CH_3 					,	/**< Peripherals HDMAC0 Channel 3 */
	FJ_MEMORY_COPY_HDMAC0_CH_4 					,	/**< Peripherals HDMAC0 Channel 4 */
	FJ_MEMORY_COPY_HDMAC0_CH_5 					,	/**< Peripherals HDMAC0 Channel 5 */
	FJ_MEMORY_COPY_HDMAC0_CH_6 					,	/**< Peripherals HDMAC0 Channel 6 */
	FJ_MEMORY_COPY_HDMAC0_CH_7 					,	/**< Peripherals HDMAC0 Channel 7 */
	FJ_MEMORY_COPY_HDMAC1_CH_0 		= 0x00040000,	/**< Peripherals HDMAC1 Channel 0 */
	FJ_MEMORY_COPY_HDMAC1_CH_1 					,	/**< Peripherals HDMAC1 Channel 1 */
	FJ_MEMORY_COPY_HDMAC1_CH_2 					,	/**< Peripherals HDMAC1 Channel 2 */
	FJ_MEMORY_COPY_HDMAC1_CH_3 					,	/**< Peripherals HDMAC1 Channel 3 */
	FJ_MEMORY_COPY_HDMAC1_CH_4 					,	/**< Peripherals HDMAC1 Channel 4 */
	FJ_MEMORY_COPY_HDMAC1_CH_5 					,	/**< Peripherals HDMAC1 Channel 5 */
	FJ_MEMORY_COPY_HDMAC1_CH_6 					,	/**< Peripherals HDMAC1 Channel 6 */
	FJ_MEMORY_COPY_HDMAC1_CH_7 					,	/**< Peripherals HDMAC1 Channel 7 */
	FJ_MEMORY_COPY_XDMAC_CH_0 		= 0x00080000,	/**< External Storage XDMAC Channel 0 */
	FJ_MEMORY_COPY_XDMAC_CH_1 					,	/**< External Storage XDMAC Channel 1 */
	FJ_MEMORY_COPY_XDMAC_CH_2 					,	/**< External Storage XDMAC Channel 2 */
	FJ_MEMORY_COPY_XDMAC_CH_3 					,	/**< External Storage XDMAC Channel 3 */
	FJ_MEMORY_COPY_XDMACFP_CH_0 	= 0x00100000,	/**< FASP Peripheral XDMAC Channel 0 */
	FJ_MEMORY_COPY_XDMACFP_CH_1 				,	/**< FASP Peripheral XDMAC Channel 1 */
	FJ_MEMORY_COPY_XDMACFP_CH_2 				,	/**< FASP Peripheral XDMAC Channel 2 */
	FJ_MEMORY_COPY_XDMACFP_CH_3 				,	/**< FASP Peripheral XDMAC Channel 3 */
} FJ_MEMORY_COPY_DMA_CH;

// wait mode
#define	FJ_MEMORY_WAITMODE_CPU		(0)				/**< CPU observes register mode  (interrupt no use) */
#define	FJ_MEMORY_WAITMODE_EVENT	(1)				/**< Event flag wait mode  (interrupt use) */

///////////////////////////////////////////////////////////////////////////////
// external variable
///////////////////////////////////////////////////////////////////////////////


/*@} --- end of fj_memory_definition */
///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////

/** @weakgroup fj_memory_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function sets the SDRAM MAP mode.
When the change of Operation Mode, this will be called to change memory map.
@param[in]	wSdramMapMode	See @ref FJ_SDRAM_MAP_MODE
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
*/
extern FJ_ERR_CODE FJ_SetSDRAMMapMode( FJ_SDRAM_MAP_MODE wSdramMapMode );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function allocated memory buffer of a requested size
@param[in]	ulSizeInBytes	The size in bytes of the memory buffer to allocate
@return	Pointer to the memory buffer
*/
extern UINT32	FJ_DirectPoolAlloc( ULONG ulSizeInBytes );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function frees previously allocated memory
@param[in]	pbyBuffer	The pointer to the memory buffer that should be freed
@retval		TRUE Normal End
@retval		FALSE Abnormal End
*/
extern BOOL	FJ_DirectPoolFree( UINT32 pbyBuffer );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets an inforamtion of the memory area (the start address and the size)
@param[in]	wLocation	See @ref FJ_MEMORY_LOCATION
@param[out]	pBuffer	start address of allocated memory.
@param[in,out]	pdwSize	A pointer to the variable for a request size.<br>
				You should set the request size on *pdwSize.<br>
				And you will be able to get the allocated size if the fixed size pool.<br>
				If the variable size pool (heap memory), you will not get any information.
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
*/
extern FJ_ERR_CODE	FJ_GetMem( FJ_MEMORY_LOCATION wLocation, UINT32* pBuffer, UINT32 *pdwSize );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Unallocate Memory Location.
@param[in]	wLocation See @ref FJ_MEMORY_LOCATION
@param[in]	Buffer	top address to release memory for heap type only. A fixed size buffer does not use this.
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
*/
extern FJ_ERR_CODE	FJ_ReleaseMem( FJ_MEMORY_LOCATION wLocation, UINT32 Buffer );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Fill the indicated Memory Location up with 0.
@param[in]	wLocation See @ref FJ_MEMORY_LOCATION
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@remarks	A fixed size buffer can use this, while a buffer for heap type can not use this.
*/
extern FJ_ERR_CODE	FJ_ClearMem( FJ_MEMORY_LOCATION wLocation );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
All D-Cache Operation function
@param[in]		level		cache level @ref FJ_MEMORY_CACHE_LEVEL
@param[in]		operation	cache operation @ref FJ_MEMORY_CACHE_OPE
@remarks	D-Cache enable is need to enable the I bit of system control register.
@remarks	Cacheable setting is need to enable the MMU.
@remarks	Level2 cache operation is need to enable the L2C register.
@remarks	Level1 cache operation is use the SWI.
*/
VOID	FJ_MEMORY_Dcache_All(FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
D-Cache Operation by Addressing & size function
@param[in]		level		cache level @ref FJ_MEMORY_CACHE_LEVEL
@param[in]		operation	cache operation @ref FJ_MEMORY_CACHE_OPE
@param[in]		address		target start address
@param[in]		size		target size
@remarks	D-Cache enable is need to enable the I bit of system control register.
@remarks	Cacheable setting is need to enable the MMU.
@remarks	Level2 cache operation is need to enable the L2C register.
@remarks	Level1 cache operation is use the SWI.
*/
VOID	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation, UINT32 address, UINT32 size);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function is Get Memory attribute.
@param[in]	addr	Memory address
@param[out]	attribute	Memory attribute @ref U_MEMORY_ATTRIBUTE
*/
VOID FJ_Get_Memory_Attribute(UINT32 addr, U_MEMORY_ATTRIBUTE *attribute);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function is Data Memory Barrier and L2 Cache Sync
*/
VOID FJ_MEMORY_DMB_POC( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function is Data Synchronization Barrier and L2 Cache Sync.
*/
VOID FJ_MEMORY_DSB_POC( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function is Data Synchronization Barrier.
*/
VOID FJ_MEMORY_DSB_POU( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get a free memory block count of Fixed-sized Memory Pool. <br>
Block count of the remaining area of the memory pool.
@param[in]	mpfid	Fixed-sized Memory Pool ID
@retval		Positive value	Free memory block count
@retval		FJ_ERR_NG	Abnormal End
*/
INT32	FJ_Get_MPF_FBLKCNT( OS_USER_ID mpfid );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get a total free area size of Variable-sized Memory Pool. <br>
Byte count of the remaining area of the memory pool.
@param[in]	mplid	Variable-sized Memory Pool ID
@retval		Positive value	Total free area size
@retval		FJ_ERR_NG	Abnormal End
*/
INT32	FJ_Get_MPL_FMPLSZ( OS_USER_ID mplid );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get a using area size of Heap Memory. <br>
Byte count of Heap area.
@retval		Heap area size
@attention
<b>
Please change to enable the compile option 'CO_HEAP_MONITOR_ON' in "cpu_rtos/project", and build. <br>
</b>
*/
INT32	FJ_Get_HEAP_SIZE( VOID );

/**
DMA forwarding between SDRAM-SDRAM.<br>
It waits until forwarding ends. (synchronization)

@param [in] ch			See @ref FJ_MEMORY_COPY_DMA_CH
@param [in] src_addr	source address
@param [in] dst_addr	destination address
@param [in] size		transfer size
@param [in] wait_mode	DMA end wait mode (FJ_MEMORY_WAITMODE_CPU or FJ_MEMORY_WAITMODE_EVENT)
@retval FJ_ERR_OK		OK
@retval FJ_ERR_NG		Abnormal End
@remarks The unit of forwarding makes a large size give priority.<br>
         <br>
         Transfer MAX size of this API as following.<br>
         	DOUBLE WORD transfer is XDMA Only (source address and destination address are 8byte alignment) : MAX size is 8MByte.<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
*/
FJ_ERR_CODE	FJ_MEMORY_Copy_Sdram_Sync(FJ_MEMORY_COPY_DMA_CH ch, UINT32 src_addr, UINT32 dst_addr, UINT32 size, UINT32 wait_mode);

/*@} --- end of fj_memory_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_memory_message
@{*/

/*@} --- end of fj_memory_message */
#endif	// _FJ_MEMORY_H
