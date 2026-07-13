/**
 * @file		memory.c
 * @brief		Memory Control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include <stdlib.h>
#include "ddim_typedef.h"
#include "dd_arm.h"
#include "sdram_map_play.h"
#include "sdram_map_play.h"
#include "memory.h"
#include "fs_user_func.h"
#include "debug.h"
#include "sdram_map_manager.h"
#include "sdram_map_common.h"
#include "sdram_map_define.h"
#include "sdram_map_movie_record.h"
//#include "sdram_map_still_mode0_define.h"
#include "sdram_map_cache_define.h"
#include "dd_cache.h"

#include "dd_hdmac.h"
#include "dd_hdmac0.h"
#include "dd_hdmac1.h"
#include "dd_xdmac.h"
#include "dd_xdmasnap.h"
#include "l1l2cache.h"
#include "os_wrapper.h"
#include "os_user_custom.h"
#include "fj_host.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
#ifdef CO_HEAP_MONITOR_ON
extern INT32 g_heap_size;
#endif

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
; Memory copy sdram
-----------------------------------------------------------------------------*/
static VOID bf_memory_copy_sdram_sync_hdmac_callback(UINT8 ch, USHORT* status)
{
	if(( *status != D_DD_HDMAC_SS_NONE ) && ( *status != D_DD_HDMAC_SS_NORMAL_END ) && ( *status != D_DD_HDMAC_SS_PAUSE )){
		BF_Debug_Print_Error(("bf_memory_copy_sdram_sync_hdmac_callback : transfer err ch(%d)\n", ch));
	}
}

static VOID bf_memory_copy_sdram_sync_hdmac0_callback(UINT8 ch, USHORT* status)
{
	if(( *status != D_DD_HDMAC0_SS_NONE ) && ( *status != D_DD_HDMAC0_SS_NORMAL_END ) && ( *status != D_DD_HDMAC0_SS_PAUSE )){
		BF_Debug_Print_Error(("bf_memory_copy_sdram_sync_hdmac0_callback : transfer err ch(%d)\n", ch));
	}
}

static VOID bf_memory_copy_sdram_sync_hdmac1_callback(UINT8 ch, USHORT* status)
{
	if(( *status != D_DD_HDMAC1_SS_NONE ) && ( *status != D_DD_HDMAC1_SS_NORMAL_END ) && ( *status != D_DD_HDMAC1_SS_PAUSE )){
		BF_Debug_Print_Error(("bf_memory_copy_sdram_sync_hdmac1_callback : transfer err ch(%d)\n", ch));
	}
}

static VOID bf_memory_copy_sdram_sync_xdmac_callback(UINT8 ch, USHORT* status)
{
	if(( *status != D_DD_XDMAC_XDDSD_IS_NONE ) && ( *status != D_DD_XDMAC_XDDSD_IS_NORMAL_END )){
		BF_Debug_Print_Error(("bf_memory_copy_sdram_sync_xdmac_callback : transfer err ch(%d)\n", ch));
	}
}

static VOID bf_memory_copy_sdram_sync_xdmafasp_callback(UINT8 ch, USHORT* status)
{
	if(( *status != D_DD_XDMASNAP_XDDSD_IS_NONE ) && ( *status != D_DD_XDMASNAP_XDDSD_IS_NORMAL_END )){
		BF_Debug_Print_Error(("bf_memory_copy_sdram_sync_xdmafasp_callback : transfer err ch(%d)\n", ch));
	}
}

static BOOL bf_memory_copy_sdram_sync_hdmac(UINT32 ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32					ret;
	USHORT					status = 0;
	T_DD_HDMAC_CTRL_TRNS	hdmac_ctrl_trns;
	T_DD_HDMAC_CTRL			hdmac_common;
	UCHAR					tmp_width;
	
	// Check transfer size
	if (((src_addr & 0x03) == 0) && ((dst_addr & 0x03) == 0) && ((size & 0x03) == 0)) {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_WORD;
	}
	else if (((src_addr & 0x01) == 0) && ((dst_addr & 0x01) == 0) && ((size & 0x01) == 0)) {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_HWORD;
	}
	else {
		hdmac_ctrl_trns.size.trns_width = D_DD_HDMAC_TW_BYTE;
	}
	
	// Parameter Setting
	hdmac_ctrl_trns.mode.input_sel = D_DD_HDMAC_IS_SOFT;
	hdmac_ctrl_trns.mode.mode_sel  = D_DD_HDMAC_MS_BLK;
	hdmac_ctrl_trns.mode.src_fix   = D_DD_HDMAC_FS_INCR;
	hdmac_ctrl_trns.mode.dst_fix   = D_DD_HDMAC_FD_INCR;
	tmp_width = 0x01 << hdmac_ctrl_trns.size.trns_width;
	if (size > (tmp_width * 16)) {
		hdmac_ctrl_trns.mode.beat_type = D_DD_HDMAC_BT_INCR16;
	}
	else {
		hdmac_ctrl_trns.mode.beat_type = D_DD_HDMAC_BT_NORMAL;
	}
	
	hdmac_ctrl_trns.size.trns_size = size;
	hdmac_ctrl_trns.size.src_addr = src_addr;
	hdmac_ctrl_trns.size.dst_addr = dst_addr;
	hdmac_ctrl_trns.int_handler = (VP_CALLBACK)bf_memory_copy_sdram_sync_hdmac_callback;
	
	hdmac_common.config_b.bit.ei = 1;
	
	ret = Dd_HDMAC_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("HDMAC Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	ret = Dd_HDMAC_Ctrl_Common(ch, &hdmac_common);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("HDMAC Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	while (1) {
		ret = Dd_HDMAC_Ctrl_Trns(ch, &hdmac_ctrl_trns);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("HDMAC Util Copy Sync. Ctrl ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_HDMAC_Start_Sync(ch, &status, wait_mode);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("HDMAC Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}
	
	ret = Dd_HDMAC_Close(ch);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("HDMAC Util Copy Sync. Close ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	if(( status != D_DD_HDMAC_SS_NONE ) && ( status != D_DD_HDMAC_SS_NORMAL_END ) && ( status != D_DD_HDMAC_SS_PAUSE )){
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_memory_copy_sdram_sync_hdmac0(UINT32 ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32					ret;
	USHORT					status = 0;
	T_DD_HDMAC0_CTRL_TRNS	hdmac0_ctrl_trns;
	T_DD_HDMAC0_CTRL		hdmac0_common;
	UCHAR					tmp_width;
	
	// Check transfer size
	if (((src_addr & 0x03) == 0) && ((dst_addr & 0x03) == 0) && ((size & 0x03) == 0)) {
		hdmac0_ctrl_trns.size.trns_width = D_DD_HDMAC0_TW_WORD;
	}
	else if (((src_addr & 0x01) == 0) && ((dst_addr & 0x01) == 0) && ((size & 0x01) == 0)) {
		hdmac0_ctrl_trns.size.trns_width = D_DD_HDMAC0_TW_HWORD;
	}
	else {
		hdmac0_ctrl_trns.size.trns_width = D_DD_HDMAC0_TW_BYTE;
	}
	
	// Parameter Setting
	hdmac0_ctrl_trns.mode.input_sel = D_DD_HDMAC0_IS_SOFT;
	hdmac0_ctrl_trns.mode.mode_sel  = D_DD_HDMAC0_MS_BLK;
	hdmac0_ctrl_trns.mode.src_fix   = D_DD_HDMAC0_FS_INCR;
	hdmac0_ctrl_trns.mode.dst_fix   = D_DD_HDMAC0_FD_INCR;
	tmp_width = 0x01 << hdmac0_ctrl_trns.size.trns_width;
	if (size > (tmp_width * 16)) {
		hdmac0_ctrl_trns.mode.beat_type = D_DD_HDMAC0_BT_INCR16;
	}
	else {
		hdmac0_ctrl_trns.mode.beat_type = D_DD_HDMAC0_BT_NORMAL;
	}
	
	hdmac0_ctrl_trns.size.trns_size = size;
	hdmac0_ctrl_trns.size.src_addr = src_addr;
	hdmac0_ctrl_trns.size.dst_addr = dst_addr;
	hdmac0_ctrl_trns.int_handler = (VP_CALLBACK)bf_memory_copy_sdram_sync_hdmac0_callback;
	
	hdmac0_common.config_b.bit.ei = 1;
	
	ret = Dd_HDMAC0_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("HDMAC0 Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	ret = Dd_HDMAC0_Ctrl_Common(ch, &hdmac0_common);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("HDMAC0 Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	while (1) {
		ret = Dd_HDMAC0_Ctrl_Trns(ch, &hdmac0_ctrl_trns);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("HDMAC0 Util Copy Sync. Ctrl ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_HDMAC0_Start_Sync(ch, &status, wait_mode);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("HDMAC0 Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}
	
	ret = Dd_HDMAC0_Close(ch);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("HDMAC0 Util Copy Sync. Close ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	if(( status != D_DD_HDMAC0_SS_NONE ) && ( status != D_DD_HDMAC0_SS_NORMAL_END ) && ( status != D_DD_HDMAC0_SS_PAUSE )){
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_memory_copy_sdram_sync_hdmac1(UINT32 ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32					ret;
	USHORT					status = 0;
	T_DD_HDMAC1_CTRL_TRNS	hdmac1_ctrl_trns;
	T_DD_HDMAC1_CTRL		hdmac1_common;
	UCHAR					tmp_width;
	
	// Check transfer size
	if (((src_addr & 0x03) == 0) && ((dst_addr & 0x03) == 0) && ((size & 0x03) == 0)) {
		hdmac1_ctrl_trns.size.trns_width = D_DD_HDMAC1_TW_WORD;
	}
	else if (((src_addr & 0x01) == 0) && ((dst_addr & 0x01) == 0) && ((size & 0x01) == 0)) {
		hdmac1_ctrl_trns.size.trns_width = D_DD_HDMAC1_TW_HWORD;
	}
	else {
		hdmac1_ctrl_trns.size.trns_width = D_DD_HDMAC1_TW_BYTE;
	}
	
	// Parameter Setting
	hdmac1_ctrl_trns.mode.input_sel = D_DD_HDMAC1_IS_SOFT;
	hdmac1_ctrl_trns.mode.mode_sel  = D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl_trns.mode.src_fix   = D_DD_HDMAC1_FS_INCR;
	hdmac1_ctrl_trns.mode.dst_fix   = D_DD_HDMAC1_FD_INCR;
	tmp_width = 0x01 << hdmac1_ctrl_trns.size.trns_width;
	if (size > (tmp_width * 16)) {
		hdmac1_ctrl_trns.mode.beat_type = D_DD_HDMAC1_BT_INCR16;
	}
	else {
		hdmac1_ctrl_trns.mode.beat_type = D_DD_HDMAC1_BT_NORMAL;
	}
	
	hdmac1_ctrl_trns.size.trns_size = size;
	hdmac1_ctrl_trns.size.src_addr = src_addr;
	hdmac1_ctrl_trns.size.dst_addr = dst_addr;
	hdmac1_ctrl_trns.int_handler = (VP_CALLBACK)bf_memory_copy_sdram_sync_hdmac1_callback;
	
	hdmac1_common.config_b.bit.ei = 1;
	
	ret = Dd_HDMAC1_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("HDMAC1 Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	ret = Dd_HDMAC1_Ctrl_Common(ch, &hdmac1_common);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("HDMAC1 Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	while (1) {
		ret = Dd_HDMAC1_Ctrl_Trns(ch, &hdmac1_ctrl_trns);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("HDMAC1 Util Copy Sync. Ctrl ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_HDMAC1_Start_Sync(ch, &status, wait_mode);
		if (ret != D_DDIM_OK) {
			BF_Debug_Print_Warning(("HDMAC1 Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}
	
	ret = Dd_HDMAC1_Close(ch);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("HDMAC1 Util Copy Sync. Close ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	if(( status != D_DD_HDMAC1_SS_NONE ) && ( status != D_DD_HDMAC1_SS_NORMAL_END ) && ( status != D_DD_HDMAC1_SS_PAUSE )){
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_memory_copy_sdram_sync_xdmac(UINT32 ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32					ret;
	USHORT					status = 0;
	T_DD_XDMAC_CTRL			dmac_ctrl_trns;
	T_DD_XDMAC_COMMON		dmac_common;
	
	// Check transfer size
	if((( src_addr & 0x07) == 0 ) && ((dst_addr & 0x07) == 0 ) && ( (size & 0x07) == 0) ){
		dmac_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_DOUBLEWORD;
		dmac_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_DOUBLEWORD;
	}
	else if(( (src_addr & 0x03) == 0 ) && ( (dst_addr & 0x03) == 0 ) && ( (size & 0x03) == 0 )){
		dmac_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_WORD;
		dmac_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_WORD;
	}
	else if(( (src_addr & 0x01) == 0 ) && ( (dst_addr & 0x01) == 0 ) && ( (size & 0x01) == 0 )){
		dmac_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_HALFWORD;
		dmac_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_HALFWORD;
	}
	else{
		dmac_ctrl_trns.config_1.bit.SBS = D_DD_XDMAC_XDSAC_SBS_BYTE;
		dmac_ctrl_trns.config_1.bit.DBS = D_DD_XDMAC_XDDAC_DBS_BYTE;
	}
	
	// Parameter Setting
	dmac_ctrl_trns.trns_size       = size;
	dmac_ctrl_trns.src_addr        = src_addr;
	dmac_ctrl_trns.dst_addr        = dst_addr;
	dmac_ctrl_trns.int_handler     = (VP_CALLBACK)bf_memory_copy_sdram_sync_xdmac_callback;
	
	dmac_ctrl_trns.config_1.bit.SRL = D_DD_XDMAC_XDSAC_SRL_DISABLE;
	dmac_ctrl_trns.config_1.bit.SAF = D_DD_XDMAC_XDSAC_SAF_NOTFIX;
	dmac_ctrl_trns.config_1.bit.SBL = D_DD_XDMAC_XDSAC_SBL_LENGTH_16;
	dmac_ctrl_trns.config_1.bit.DRL = D_DD_XDMAC_XDDAC_DRL_DISABLE;
	dmac_ctrl_trns.config_1.bit.DAF = D_DD_XDMAC_XDDAC_DAF_NOTFIX;
	dmac_ctrl_trns.config_1.bit.DBL = D_DD_XDMAC_XDDAC_DBL_LENGTH_16;
	dmac_ctrl_trns.config_1.bit.EI = D_DD_XDMAC_XDDES_EI_ENABLE;
	
	if( wait_mode == D_DD_XDMAC_WAITMODE_EVENT ){
		dmac_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_ENABLE;
	}
	else{
		dmac_ctrl_trns.config_1.bit.TI = D_DD_XDMAC_XDDES_TI_DISABLE;
	}
	
	dmac_ctrl_trns.config_1.bit.AT = D_DD_XDMAC_XDDES_AT_SOURCE;
	dmac_ctrl_trns.config_1.bit.BR = D_DD_XDMAC_XDDES_BR_DISABLE;
	dmac_ctrl_trns.config_1.bit.BT = D_DD_XDMAC_XDDES_BT_ENABLE;
	dmac_ctrl_trns.config_1.bit.SA = D_DD_XDMAC_XDDES_SA_ENABLE;
	dmac_ctrl_trns.config_1.bit.TF = D_DD_XDMAC_XDDES_TF_SOFT;
	dmac_ctrl_trns.config_1.bit.SE = D_DD_XDMAC_XDDES_SE_ENABLE;
	dmac_ctrl_trns.config_2.bit.DCN = D_DD_XDMAC_XDDCC_DCN_INVALID;
	dmac_ctrl_trns.config_2.bit.DCA = 0;
	
	dmac_common.common_config.bit.XE = D_DD_XDMAC_XDACS_XE_ENABLE;
	dmac_common.common_config.bit.CP = D_DD_XDMAC_XDACS_CP_FIXED;
	
	ret = Dd_XDMAC_Open(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	ret = Dd_XDMAC_Ctrl_Common(&dmac_common);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	while(1){
		ret = Dd_XDMAC_Ctrl_Trns(ch, &dmac_ctrl_trns);
		if( ret != D_DDIM_OK ){
			BF_Debug_Print_Warning(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_XDMAC_Start_Sync(ch, &status, wait_mode);
		if( ret != D_DDIM_OK ){
			BF_Debug_Print_Warning(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}
	
	ret = Dd_XDMAC_Close(ch);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	if(( status != D_DD_XDMAC_XDDSD_IS_NONE ) && ( status != D_DD_XDMAC_XDDSD_IS_NORMAL_END )){
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_memory_copy_sdram_sync_xdmafasp(UINT32 ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	INT32					ret;
	USHORT					status = 0;
	T_DD_XDMASNAP_CTRL		dmasnap_ctrl_trns;
	T_DD_XDMASNAP_COMMON	dmasnap_common;
	
	// Check transfer size
	if((( src_addr & 0x07) == 0 ) && ((dst_addr & 0x07) == 0 ) && ( (size & 0x07) == 0) ){
		dmasnap_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_DOUBLEWORD;
		dmasnap_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_DOUBLEWORD;
	}
	else if(( (src_addr & 0x03) == 0 ) && ( (dst_addr & 0x03) == 0 ) && ( (size & 0x03) == 0 )){
		dmasnap_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_WORD;
		dmasnap_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_WORD;
	}
	else if(( (src_addr & 0x01) == 0 ) && ( (dst_addr & 0x01) == 0 ) && ( (size & 0x01) == 0 )){
		dmasnap_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_HALFWORD;
		dmasnap_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_HALFWORD;
	}
	else{
		dmasnap_ctrl_trns.config_1.bit.SBS = D_DD_XDMASNAP_XDSAC_SBS_BYTE;
		dmasnap_ctrl_trns.config_1.bit.DBS = D_DD_XDMASNAP_XDDAC_DBS_BYTE;
	}
	
	// Parameter Setting
	dmasnap_ctrl_trns.trns_size       = size;
	dmasnap_ctrl_trns.src_addr        = src_addr;
	dmasnap_ctrl_trns.dst_addr        = dst_addr;
	dmasnap_ctrl_trns.int_handler     = (VP_CALLBACK)bf_memory_copy_sdram_sync_xdmafasp_callback;
	
	dmasnap_ctrl_trns.config_1.bit.SRL = D_DD_XDMASNAP_XDSAC_SRL_DISABLE;
	dmasnap_ctrl_trns.config_1.bit.SAF = D_DD_XDMASNAP_XDSAC_SAF_NOTFIX;
	dmasnap_ctrl_trns.config_1.bit.SBL = D_DD_XDMASNAP_XDSAC_SBL_LENGTH_16;
	dmasnap_ctrl_trns.config_1.bit.DRL = D_DD_XDMASNAP_XDDAC_DRL_DISABLE;
	dmasnap_ctrl_trns.config_1.bit.DAF = D_DD_XDMASNAP_XDDAC_DAF_NOTFIX;
	dmasnap_ctrl_trns.config_1.bit.DBL = D_DD_XDMASNAP_XDDAC_DBL_LENGTH_16;
	dmasnap_ctrl_trns.config_1.bit.EI = D_DD_XDMASNAP_XDDES_EI_ENABLE;
	
	if( wait_mode == D_DD_XDMASNAP_WAITMODE_EVENT ){
		dmasnap_ctrl_trns.config_1.bit.TI = D_DD_XDMASNAP_XDDES_TI_ENABLE;
	}
	else{
		dmasnap_ctrl_trns.config_1.bit.TI = D_DD_XDMASNAP_XDDES_TI_DISABLE;
	}
	
	dmasnap_ctrl_trns.config_1.bit.AT = D_DD_XDMASNAP_XDDES_AT_SOURCE;
	dmasnap_ctrl_trns.config_1.bit.BR = D_DD_XDMASNAP_XDDES_BR_DISABLE;
	dmasnap_ctrl_trns.config_1.bit.BT = D_DD_XDMASNAP_XDDES_BT_ENABLE;
	dmasnap_ctrl_trns.config_1.bit.SA = D_DD_XDMASNAP_XDDES_SA_ENABLE;
	dmasnap_ctrl_trns.config_1.bit.TF = D_DD_XDMASNAP_XDDES_TF_SOFT;
	dmasnap_ctrl_trns.config_1.bit.SE = D_DD_XDMASNAP_XDDES_SE_ENABLE;
	dmasnap_ctrl_trns.config_2.bit.DCN = D_DD_XDMASNAP_XDDCC_DCN_INVALID;
	dmasnap_ctrl_trns.config_2.bit.DCA = 0;
	
	dmasnap_common.common_config.bit.XE = D_DD_XDMASNAP_XDACS_XE_ENABLE;
	dmasnap_common.common_config.bit.CP = D_DD_XDMASNAP_XDACS_CP_FIXED;
	
	ret = Dd_XDMASNAP_Open(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("DMA Util Copy Sync. Open ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	ret = Dd_XDMASNAP_Ctrl_Common(&dmasnap_common);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("DMA Util Copy Sync. Common Setting ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	while(1){
		ret = Dd_XDMASNAP_Ctrl_Trns(ch, &dmasnap_ctrl_trns);
		if( ret != D_DDIM_OK ){
			BF_Debug_Print_Warning(("DMA Util Copy Sync. Ctrl Setting ERR : ret=0x%x\n", ret));
			break;
		}
		ret = Dd_XDMASNAP_Start_Sync(ch, &status, wait_mode);
		if( ret != D_DDIM_OK ){
			BF_Debug_Print_Warning(("DMA Util Copy Sync. Start ERR : ret=0x%x, status=0x%x\n", ret, status));
		}
		break;
	}
	
	ret = Dd_XDMASNAP_Close(ch);
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("DMA Util Copy Sync. Close ERR : ret=0x%x\n", ret));
		return FALSE;
	}
	
	if(( status != D_DD_XDMASNAP_XDDSD_IS_NONE ) && ( status != D_DD_XDMASNAP_XDDSD_IS_NORMAL_END )){
		return FALSE;
	}
	
	return TRUE;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
FJ_ERR_CODE fj_setsdrammapmode( FJ_SDRAM_MAP_MODE wSdramMapMode )
{
	switch( wSdramMapMode ) {
		case FJ_SDRAM_MAP_MODE_VIDEO_0:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_VIDEO );		// Normal Mode ### MILB_Porting
			break;
		case FJ_SDRAM_MAP_MODE_VIDEO_1:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_VIDEO );		// High Performance Mode ### MILB_Porting
			break;
		case FJ_SDRAM_MAP_MODE_VIDEO_2:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_3DNR_EXT );	// Normal Mode + special MXIC set
			break;
		case FJ_SDRAM_MAP_MODE_PHOTO:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_CAP );
			break;
		case FJ_SDRAM_MAP_MODE_BURST:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_BURST );
			break;
		case FJ_SDRAM_MAP_MODE_PIV:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_PIV );
			break;
		case FJ_SDRAM_MAP_MODE_STITCH:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_STITCH );
			break;
		case FJ_SDRAM_MAP_MODE_STILL_HDR:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_STILL_HDR );
			break;
		case FJ_SDRAM_MAP_MODE_STILL_STITCH:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_STILL_STITCH );
			break;
		case FJ_SDRAM_MAP_MODE_MULTI_OUT:
			Sdram_Map_Mgr_Set_SdramMap_Mode( E_SDRAM_MAP_MGR_MODE_MULTI_OUT );
			break;
		case FJ_SDRAM_MAP_MODE_PB:
			break;
		case FJ_SDRAM_MAP_MODE_SETUP:
		case FJ_SDRAM_MAP_MODE_LIVE:
			break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
//			break;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		This function allocated memory buffer of a requested size.\n
 *	@param		i:ulSizeInBytes		The size in bytes of the memory buffer to allocate.
 *	@return		memory buffer pointer	(Success)
 *	@return		NULL					(Failure)
 *	@note		None
 *	@attention	None
 */
UINT32 fj_directpoolalloc( ULONG ulSizeInBytes )
{
	UINT32	pointer;

	pointer = (UINT32)malloc( ulSizeInBytes );
	return pointer;
}

/**
 *	@brief		This function frees previously allocated memory.\n
 *	@param		i:pbyBuffer		The pointer to the memory buffer that should be freed.
 *	@return		TRUE		(Success)
 *	@return		FALSE		(Failure)
 *	@note		None
 *	@attention	None
 */
BOOL fj_directpoolfree( UINT32 pbyBuffer )
{
	if( 0 == pbyBuffer ) {
		return FALSE;
	}
	free( (void*)pbyBuffer );
	return TRUE;
}

/**
 *	@brief	Get Memory Location.
 *	@param		i:wLocation		Parameter ID  (values for fj_setparam)
 *								PRMID_TL_SYSTEM_DATA_ERROR_CALLBACK_FUNC
 *	@return		TRUE		(Success)
 *	@return		FALSE		(Failure)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_getmem( FJ_MEMORY_LOCATION wLocation, UINT32* pBuffer, UINT32 *pdwSize )
{
	ULONG	movie_map_mode;
	
	if( 0 == pBuffer ) {
		BF_Debug_Print_Error(("FJ_GetMem ERROR : parameter pBuffer=0\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( 0 == pdwSize ) {
		BF_Debug_Print_Error(("FJ_GetMem ERROR : parameter pdwSize=0\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	switch( wLocation ){
		case FJ_HOST_GENERIC_MEM_LOCATION:
			{
				VOID *ptr;
				ptr = malloc( *pdwSize );
				if( !ptr ) {
					return FJ_ERR_NG_CONDITION;
				}
				*pBuffer = (UINT32)ptr;
			}
			break;
		case FJ_HOST_LCD_OSD_FRAME_MEM_LOCATION:
			if( (SDRAM_SIZ_HOST_LCD_OSD_FRAME_DATA_AREA * SDRAM_BNK_HOST_LCD_OSD_FRAME_DATA_AREA) < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_LCD_OSD_FRAME_DATA_AREA * SDRAM_BNK_HOST_LCD_OSD_FRAME_DATA_AREA;
			*pBuffer = SDRAM_ADR_HOST_LCD_OSD_FRAME_DATA_AREA_CACHE_ON;
			break;
		case FJ_HOST_HDMI_OSD_FRAME_MEM_LOCATION:
			if( (SDRAM_SIZ_HOST_HDMI_OSD_FRAME_DATA_AREA * SDRAM_BNK_HOST_HDMI_OSD_FRAME_DATA_AREA) < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_HDMI_OSD_FRAME_DATA_AREA * SDRAM_BNK_HOST_HDMI_OSD_FRAME_DATA_AREA;
			*pBuffer = SDRAM_ADR_HOST_HDMI_OSD_FRAME_DATA_AREA_CACHE_ON;
			break;

		case FJ_HOST_MUXER_INDEX_BUF_STREAM1_MEM_LOCATION:
			if( SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			movie_map_mode = Sdram_Map_Mgr_Get_Movie_Record_Table_Idx();
			if( movie_map_mode == 0 ){
				*pdwSize = SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA;
				*pBuffer = SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA;
			}
			else if( movie_map_mode == 1 ){
				*pdwSize = SDRAM_SIZ_MOVIE_RECORDS_HOST_MUXER_INDEX_STREAM1_AREA;
				*pBuffer = SDRAM_ADR_MOVIE_RECORDS_HOST_MUXER_INDEX_STREAM1_AREA;
			}
			else{
				BF_Debug_Print_Error(("FJ_GetMem : map mode error!!\n"));
			}
			break;
			
		case FJ_HOST_MUXER_INDEX_BUF_STREAM2_MEM_LOCATION:
			if( SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			movie_map_mode = Sdram_Map_Mgr_Get_Movie_Record_Table_Idx();
			if( movie_map_mode == 0 ){
				*pdwSize = SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA;
				*pBuffer = SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA;
			}
			else if( movie_map_mode == 1 ){
				*pdwSize = SDRAM_SIZ_MOVIE_RECORDS_HOST_MUXER_INDEX_STREAM2_AREA;
				*pBuffer = SDRAM_ADR_MOVIE_RECORDS_HOST_MUXER_INDEX_STREAM2_AREA;
			}
			else{
				BF_Debug_Print_Error(("FJ_GetMem : map mode error!!\n"));
			}
			break;
			
		case FJ_HOST_VIDEO_DFS_MEM_LOCATION:
			if( SDRAM_SIZ_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			movie_map_mode = Sdram_Map_Mgr_Get_Movie_Record_Table_Idx();
			if( movie_map_mode == 0 ){
				*pdwSize = SDRAM_SIZ_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA;
				*pBuffer = SDRAM_ADR_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA;
			}
			else if( movie_map_mode == 1 ){
				*pdwSize = SDRAM_SIZ_MOVIE_RECORDS_HOST_VIDEO_DFS_AREA;
				*pBuffer = SDRAM_ADR_MOVIE_RECORDS_HOST_VIDEO_DFS_AREA;
			}
			else{
				BF_Debug_Print_Error(("FJ_GetMem : map mode error!!\n"));
			}
			break;
		case FJ_HOST_JPEG_DEMUXER_READ_MEM_LOCATION:
			if( SDRAM_SIZ_HOST_JPEG_DEMUXER_READ_DATA_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_JPEG_DEMUXER_READ_DATA_AREA;
			*pBuffer = SDRAM_ADR_HOST_JPEG_DEMUXER_READ_DATA_AREA_CACHE_ON;
			break;
		case FJ_HOST_MP4_DEMUXER_READ_MEM_LOCATION:
			if( SDRAM_SIZ_HOST_MP4_DEMUXER_READ_DATA_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_MP4_DEMUXER_READ_DATA_AREA;
			*pBuffer = SDRAM_ADR_HOST_MP4_DEMUXER_READ_DATA_AREA_CACHE_ON;
			break;
		case FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION:
			if( SDRAM_SIZ_HOST_MP4_DEMUXER_PARSED_DATA_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_MP4_DEMUXER_PARSED_DATA_AREA;
			*pBuffer = SDRAM_ADR_HOST_MP4_DEMUXER_PARSED_DATA_AREA_CACHE_ON;
			break;
		case FJ_HOST_WAV_READ_MEM_LOCATION:
			if( SDRAM_SIZ_HOST_WAV_READ_MEM_LOCATION_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_WAV_READ_MEM_LOCATION_AREA;
			*pBuffer = SDRAM_ADR_HOST_WAV_READ_MEM_LOCATION_AREA_CACHE_ON;
			break;
		case FJ_HOST_ENCODED_BITS_BUFFER_MEM_LOCATION:
			if( SDRAM_SIZ_HOST_ENCODED_BITS_DATA_AREA < *pdwSize ) {
				BF_Debug_Print_Error(("FJ_GetMem : too large (size=%d)\n", *pdwSize));
				return FJ_ERR_NG_CONDITION;
			}
			*pdwSize = SDRAM_SIZ_HOST_ENCODED_BITS_DATA_AREA;
			*pBuffer = SDRAM_ADR_HOST_ENCODED_BITS_DATA_AREA_CACHE_ON;
			break;
		case FJ_STITCH_WORKAREA_MEM_LOCATION:
			break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
//			break;
	}
	return FJ_ERR_OK;	/* pgr0524 */
}

/**
 *	@brief	Unget Memory Location.
 *	@param		i:wLocation		Parameter ID  (values for fj_setparam)
 *								PRMID_TL_SYSTEM_DATA_ERROR_CALLBACK_FUNC
 *	@return		TRUE		(Success)
 *	@return		FALSE		(Failure)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_releasemem( FJ_MEMORY_LOCATION wLocation, UINT32 Buffer )
{
	switch( wLocation ){
		case FJ_HOST_GENERIC_MEM_LOCATION:
			if( Buffer ) {
				free( (void*)Buffer );
				Buffer = 0;
			}
			break;
		case FJ_HOST_LCD_OSD_FRAME_MEM_LOCATION:
		case FJ_HOST_HDMI_OSD_FRAME_MEM_LOCATION:
		case FJ_HOST_MUXER_INDEX_BUF_STREAM1_MEM_LOCATION:
		case FJ_HOST_MUXER_INDEX_BUF_STREAM2_MEM_LOCATION:
		case FJ_HOST_VIDEO_DFS_MEM_LOCATION:
		case FJ_HOST_STILL_DFS_MEM_LOCATION:
		case FJ_HOST_JPEG_DEMUXER_READ_MEM_LOCATION:
		case FJ_HOST_MP4_DEMUXER_READ_MEM_LOCATION:
		case FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION:
		case FJ_HOST_WAV_READ_MEM_LOCATION:
		case FJ_HOST_ENCODED_BITS_BUFFER_MEM_LOCATION:
		case FJ_HOST_DECODED_YUV_MAIN_MEM_LOCATION:
		case FJ_HOST_DECODED_YUV_THUMBNAIL_MEM_LOCATION:
		case FJ_STITCH_WORKAREA_MEM_LOCATION:
		case FJ_HOST_PFC_WORKAREA_MEM_LOCATION:
			break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
//			break;
	}
	return FJ_ERR_OK;
}

FJ_ERR_CODE	fj_clearmem( FJ_MEMORY_LOCATION wLocation )
{
	switch( wLocation ){
		case FJ_HOST_LCD_OSD_FRAME_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_LCD_OSD_FRAME_DATA_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_LCD_OSD_FRAME_DATA_AREA * SDRAM_BNK_HOST_LCD_OSD_FRAME_DATA_AREA );
			break;
		case FJ_HOST_HDMI_OSD_FRAME_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_HDMI_OSD_FRAME_DATA_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_HDMI_OSD_FRAME_DATA_AREA * SDRAM_BNK_HOST_HDMI_OSD_FRAME_DATA_AREA);
			break;
		case FJ_HOST_MUXER_INDEX_BUF_STREAM1_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA_CACHE_ON, 0, SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM1_AREA );
			break;
		case FJ_HOST_MUXER_INDEX_BUF_STREAM2_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA_CACHE_ON, 0, SDRAM_SIZ_MOVIE_RECORD0_HOST_MUXER_INDEX_STREAM2_AREA );
			break;
		case FJ_HOST_VIDEO_DFS_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA_CACHE_ON, 0, SDRAM_SIZ_MOVIE_RECORD0_HOST_VIDEO_DFS_AREA );
			break;
		case FJ_HOST_JPEG_DEMUXER_READ_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_JPEG_DEMUXER_READ_DATA_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_JPEG_DEMUXER_READ_DATA_AREA );
			break;
		case FJ_HOST_MP4_DEMUXER_READ_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_MP4_DEMUXER_READ_DATA_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_MP4_DEMUXER_READ_DATA_AREA );
			break;
		case FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_MP4_DEMUXER_PARSED_DATA_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_MP4_DEMUXER_PARSED_DATA_AREA );
			break;
		case FJ_HOST_WAV_READ_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_WAV_READ_MEM_LOCATION_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_WAV_READ_MEM_LOCATION_AREA );
			break;
		case FJ_HOST_ENCODED_BITS_BUFFER_MEM_LOCATION:
			memset( (void *)SDRAM_ADR_HOST_ENCODED_BITS_DATA_AREA_CACHE_ON, 0, SDRAM_SIZ_HOST_ENCODED_BITS_DATA_AREA );
			break;
		case FJ_STITCH_WORKAREA_MEM_LOCATION:
			break;
		default:
			return FJ_ERR_NG_INPUT_PARAM;
//			break;
	}
	return FJ_ERR_OK;
}

VOID fj_memory_dmb_poc( VOID )
{
	Dd_ARM_Dmb_Poc();
}

VOID fj_memory_dsb_poc( VOID )
{
	Dd_ARM_Dsb_Poc();
}

VOID fj_memory_dsb_pou( VOID )
{
	Dd_ARM_Dsb_Pou();
}

VOID	fj_memory_dcache_all( FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation )
{
	switch(level) {
		case FJ_MEMORY_CACHE_L1:
			switch(operation) {
				case FJ_MEMORY_CACHE_FLUSH:
					Dd_CACHE_L1_Flush_Dcache_All();
					break;
				case FJ_MEMORY_CACHE_CLEAN:
					Dd_CACHE_L1_Clean_Dcache_All();
					break;
				case FJ_MEMORY_CACHE_CLEAN_FLUSH:
					Dd_CACHE_L1_Clean_Flush_Dcache_All();
					break;
				default:
					BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [operation] = %d\n", operation));
					break;
			}
			break;
		case FJ_MEMORY_CACHE_L2:
			switch(operation) {
				case FJ_MEMORY_CACHE_FLUSH:
					Dd_CACHE_L2_Flush_Dcache_All();
					break;
				case FJ_MEMORY_CACHE_CLEAN:
					Dd_CACHE_L2_Clean_Dcache_All();
					break;
				case FJ_MEMORY_CACHE_CLEAN_FLUSH:
					Dd_CACHE_L2_Clean_Flush_Dcache_All();
					break;
				default:
					BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [operation] = %d\n", operation));
					break;
			}
			break;
		case FJ_MEMORY_CACHE_L1_L2:
			switch(operation) {
				case FJ_MEMORY_CACHE_FLUSH:
					BF_L1l2cache_Flush_All();
					break;
				case FJ_MEMORY_CACHE_CLEAN:
					BF_L1l2cache_Clean_All();
					break;
				case FJ_MEMORY_CACHE_CLEAN_FLUSH:
					BF_L1l2cache_Clean_Flush_All();
					break;
				default:
					BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [operation] = %d\n", operation));
					break;
			}
			break;
		default:
			BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [level] = %d\n", level));
			break;
	}
}

VOID	fj_memory_dcache_addr( FJ_MEMORY_CACHE_LEVEL level, FJ_MEMORY_CACHE_OPE operation, UINT32 address, UINT32 size )
{
	switch(level) {
		case FJ_MEMORY_CACHE_L1:
			switch(operation) {
				case FJ_MEMORY_CACHE_FLUSH:
					Dd_CACHE_L1_Flush_Dcache_Addr( address, size );
					break;
				case FJ_MEMORY_CACHE_CLEAN:
					Dd_CACHE_L1_Clean_Dcache_Addr( address, size );
					break;
				case FJ_MEMORY_CACHE_CLEAN_FLUSH:
					Dd_CACHE_L1_Clean_Flush_Dcache_Addr( address, size );
					break;
				default:
					BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [operation] = %d\n", operation));
					break;
			}
			break;
		case FJ_MEMORY_CACHE_L2:
			switch(operation) {
				case FJ_MEMORY_CACHE_FLUSH:
					Dd_CACHE_L2_Flush_Dcache_Addr( address, size );
					break;
				case FJ_MEMORY_CACHE_CLEAN:
					Dd_CACHE_L2_Clean_Dcache_Addr( address, size );
					break;
				case FJ_MEMORY_CACHE_CLEAN_FLUSH:
					BF_L1l2cache_Clean_Flush_Addr( address, size );
					break;
				default:
					BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [operation] = %d\n", operation));
					break;
			}
			break;
		case FJ_MEMORY_CACHE_L1_L2:
			switch(operation) {
				case FJ_MEMORY_CACHE_FLUSH:
					BF_L1l2cache_Flush_Addr( address, size );
					break;
				case FJ_MEMORY_CACHE_CLEAN:
					BF_L1l2cache_Clean_Addr( address, size );
					break;
				case FJ_MEMORY_CACHE_CLEAN_FLUSH:
					BF_L1l2cache_Clean_Flush_Addr( address, size );
					break;
				default:
					BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [operation] = %d\n", operation));
					break;
			}
			break;
		default:
			BF_Debug_Print_Warning(("fj_memory_dcache_all: input param error. [level] = %d\n", level));
			break;
	}
}

VOID	fj_get_memory_attribute( UINT32 addr, U_MEMORY_ATTRIBUTE *attribute )
{
	UINT32 ttbaddr;
	UINT32 ttbdata;
	
	ttbaddr = Dd_ARM_Get_TTB();
	// Translation table base Convert to PageTable
	ttbaddr = (ttbaddr & 0xFFFFC000) | ((addr & 0xFFF00000) >> 20) * 4;
	ttbdata = *((volatile unsigned int*)(ttbaddr));
	
	memset(attribute, 0, sizeof(U_MEMORY_ATTRIBUTE));
	// Attribute Set
	switch((ttbdata >> 14) & 0x1){
		case 0:
			switch((ttbdata >> 12) & 0x3){
			case 0:
				switch((ttbdata >> 2) & 0x3){
					case 0:
						attribute->bit.strongly_ordered = 1;
						break;
					case 1:
						attribute->bit.device = 1;
						break;
				}
				break;
			case 2:
				attribute->bit.device = 1;
				break;
			}
			break;
		case 1:
			attribute->bit.normal = 1;
			switch((ttbdata >> 2) & 0x3){
				case 1:
				case 2:
				case 3:
					attribute->bit.l1_cache = 1;
					break;
			}
			switch((ttbdata >> 12) & 0x3){
				case 1:
				case 2:
				case 3:
					attribute->bit.l2_cache = 1;
					break;
			}
			break;
	}
	if ((((ttbdata >> 15) & 0x1) == 0) &&
	    (((ttbdata >> 11) & 0x1) == 0) &&
	    (((ttbdata >> 10) & 0x1) == 0)){
	    // APX=0 and AP1=0 and AP0=0
	    // Faults
	    attribute->bit.access_permission = 0;
	}
	if ((((ttbdata >> 15) & 0x1) == 1) &&
	    (((ttbdata >> 11) & 0x1) == 0) &&
	    (((ttbdata >> 10) & 0x1) == 1)){
	    // APX=1 and AP1=0 and AP0=1
	    // Read Only
	    attribute->bit.access_permission = 1;
	}
	if ((((ttbdata >> 15) & 0x1) == 0) &&
	    (((ttbdata >> 11) & 0x1) == 1) &&
	    (((ttbdata >> 10) & 0x1) == 1)){
	    // APX=0 and AP1=1 and AP0=1
	    // Full Access
	    attribute->bit.access_permission = 2;
	}
	// Shared Memory
	if (((ttbdata >> 16) & 0x1) == 1) {
	    attribute->bit.shared = 1;
	}
}

INT32	fj_get_mpf_fblkcnt( OS_USER_ID mpfid )
{
	INT32 ret;
	
	ret = BF_Os_Wrapper_ref_mpf_cnt(mpfid);
	if (ret == -1){
		return FJ_ERR_NG;
	}
	
	return ret;
}

INT32	fj_get_mpl_fmplsz( OS_USER_ID mplid )
{
	INT32 ret;
	
	ret = BF_Os_Wrapper_ref_mpl_frsz(mplid);
	if (ret == -1){
		return FJ_ERR_NG;
	}
	return ret;
}

INT32	fj_get_heap_size( VOID )
{
#ifdef CO_HEAP_MONITOR_ON
	return g_heap_size;
#else
	return 0;
#endif
}

FJ_ERR_CODE	fj_memory_copy_sdram_sync(FJ_MEMORY_COPY_DMA_CH ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode)
{
	UINT32					dmach;
	UINT32					dmatype;
	UINT32					src_phy_addr;
	UINT32					dst_phy_addr;
	
	dmatype = ch & 0xFFFF0000;
	dmach = ch & 0x0000FFFF;
	switch(dmatype) {
		case FJ_MEMORY_COPY_HDMAC_CH_0:
			if( (ch < FJ_MEMORY_COPY_HDMAC_CH_0) || (ch > FJ_MEMORY_COPY_HDMAC_CH_3) ){
				BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [ch] = 0x%08x\n", ch));
				return FJ_ERR_NG_INPUT_PARAM;
			}
			break;
		case FJ_MEMORY_COPY_HDMAC0_CH_0:
			if( (ch < FJ_MEMORY_COPY_HDMAC0_CH_0) || (ch > FJ_MEMORY_COPY_HDMAC0_CH_7) ){
				BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [ch] = 0x%08x\n", ch));
				return FJ_ERR_NG_INPUT_PARAM;
			}
			break;
		case FJ_MEMORY_COPY_HDMAC1_CH_0:
			if( (ch < FJ_MEMORY_COPY_HDMAC1_CH_0) || (ch > FJ_MEMORY_COPY_HDMAC1_CH_7) ){
				BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [ch] = 0x%08x\n", ch));
				return FJ_ERR_NG_INPUT_PARAM;
			}
			break;
		case FJ_MEMORY_COPY_XDMAC_CH_0:
			if( (ch < FJ_MEMORY_COPY_XDMAC_CH_0) || (ch > FJ_MEMORY_COPY_XDMAC_CH_3) ){
				BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [ch] = 0x%08x\n", ch));
				return FJ_ERR_NG_INPUT_PARAM;
			}
			break;
		case FJ_MEMORY_COPY_XDMACFP_CH_0:
			if( (ch < FJ_MEMORY_COPY_XDMACFP_CH_0) || (ch > FJ_MEMORY_COPY_XDMACFP_CH_3) ){
				BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [ch] = 0x%08x\n", ch));
				return FJ_ERR_NG_INPUT_PARAM;
			}
			break;
		default:
			BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [ch] = 0x%08x\n", ch));
			return FJ_ERR_NG_INPUT_PARAM;
	}
	if(( src_addr == 0 ) || ( dst_addr == 0 )){
		BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [addr] = 0\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( size == 0 ){
		BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [size] = 0\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if(( wait_mode != FJ_MEMORY_WAITMODE_CPU ) && ( wait_mode != FJ_MEMORY_WAITMODE_EVENT )){
		BF_Debug_Print_Warning(("fj_memory_copy_sdram_sync: input param error. [wait_mode] = %x\n", wait_mode));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	src_phy_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(src_addr);
	dst_phy_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(dst_addr);
	
	BF_L1l2cache_Clean_Flush_Addr( src_phy_addr, size );
	BF_L1l2cache_Clean_Flush_Addr( dst_phy_addr, size );
	
	switch(dmatype) {
		case FJ_MEMORY_COPY_HDMAC_CH_0:
			if( bf_memory_copy_sdram_sync_hdmac(dmach, src_phy_addr, dst_phy_addr, size, wait_mode) == FALSE ){
				return FJ_ERR_NG;
			}
			break;
		case FJ_MEMORY_COPY_HDMAC0_CH_0:
			if( bf_memory_copy_sdram_sync_hdmac0(dmach, src_phy_addr, dst_phy_addr, size, wait_mode) == FALSE ){
				return FJ_ERR_NG;
			}
			break;
		case FJ_MEMORY_COPY_HDMAC1_CH_0:
			if( bf_memory_copy_sdram_sync_hdmac1(dmach, src_phy_addr, dst_phy_addr, size, wait_mode) == FALSE ){
				return FJ_ERR_NG;
			}
			break;
		case FJ_MEMORY_COPY_XDMAC_CH_0:
			if( bf_memory_copy_sdram_sync_xdmac(dmach, src_phy_addr, dst_phy_addr, size, wait_mode) == FALSE ){
				return FJ_ERR_NG;
			}
			break;
		case FJ_MEMORY_COPY_XDMACFP_CH_0:
			if( bf_memory_copy_sdram_sync_xdmafasp(dmach, src_phy_addr, dst_phy_addr, size, wait_mode) == FALSE ){
				return FJ_ERR_NG;
			}
			break;
		default:
			return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}
