/**
 * @file		ip_pd.c
 * @brief		API for Fotonation PersonDetect (ObjectDetect)
 * @author		Hattori
 * @since		2016/01/19
 * @date		2016/09/15
 * @note		
 * @attention	
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT 2016</I></B>
 */

#pragma GCC optimize ("O0")

// ****************************************************************************
//                          Include
// ****************************************************************************
#define GT_CUSTOM_INT
#include <string.h>
#include <stdio.h>
#include <stdatomic.h>

#include "ddim_typedef.h"
#include "ip_pd.h"
#include "LibAHOF_PD.h"
#include "AHOFDetector.h"
#include "AHOFManager.h"
#include "ahip.h"

#ifdef EVAL
#include "measure_time.h"
#endif
#include "os_user_custom.h"

// ****************************************************************************
//                          Define
// ****************************************************************************
//#define	EVAL
//#define	HIGHRESO
//#define D_DETECT_INFO_PRINT

typedef int	bool;
#define	false	0
#define	true	1

#define	D_IP_PD_STATUS_NONE			    0x0000
#define	D_IP_PD_STATUS_HEAP			    0x0001
#define	D_IP_PD_STATUS_MANAGER		    0x0002
#define	D_IP_PD_STATUS_DETECTOR		    0x0004
#define	D_IP_PD_STATUS_REGISTER		    0x0008
#define	D_IP_PD_STATUS_AHIP_PRE_START	0x0010
#define	D_IP_PD_STATUS_AHIP_NORM_START	0x0020
#define	D_IP_PD_STATUS_PRE_START		0x0040
#define	D_IP_PD_STATUS_NORM_START		0x0080
#define	D_IP_PD_STATUS_START			0x0100

#define FLG_AHIP_PD_START			(0x00000001)
#define	FLG_AHIP_PD_TERMINATE		(0x10000000)
#define FLG_AHIP_PD_WAITPTN			( FLG_AHIP_PD_START | FLG_AHIP_PD_TERMINATE )

#define D_IP_PD_MEM_PRE_BUF_SIZE	(0xf00000)		// T.B.D

#define PD_CONV_POS( x, real_size, map_size)	((INT32)((x) * (real_size) / (map_size)))

typedef uintptr_t atomic_t;
#define atomic_set(a, v)    atomic_store_explicit( a, v, memory_order_relaxed )
#define atomic_get( a )     atomic_load_explicit( a, memory_order_relaxed )

static __inline int atomic_add( atomic_t *a, uintptr_t v )
{
    // returns NEW value
    return atomic_fetch_add( a, v ) + v;
}

#define atomic_inc(a)           atomic_add( (a), 1 )
#define atomic_dec(a)           atomic_add( (a), -1 )

#ifndef GT_MIN
#   define GT_MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef GT_MAX
#   define GT_MAX(x,y) ((x) > (y) ? (x) : (y))
#endif
// ****************************************************************************
//                           Structure
// ****************************************************************************
typedef struct {
    UINT32  frame_no;
    void   *y;
    void   *u;
    void   *v;
    void   *map1;
    void   *map2;
    void   *map3;
    void   *map4;
    UINT32  map5;
} PRE_DATA;

// ****************************************************************************
//                           Global Data
// ****************************************************************************
static 	USHORT			gIpPD_Status = D_IP_PD_STATUS_NONE;
static	GT_HEAP*		gIpFNheapPtrPD ;
static	GT_HEAP*		gIpFNheapPtrMgr ;
#ifdef EVAL
static	ULONG			ex_time = 0;
#endif

// This is set to true when the preview mode should close
volatile bool bEndPDThread = false;

AHOF_MANAGER*               gIpPDmgr		= NULL;		// The AHOF Manager object
AHOF_MANAGER_RESULTS*		gIpPDresults	= NULL;		// Pointer to the results, after the image was processed
AHOF_PERSON_DETECTOR*       gIpPDpd			= NULL;		// The AHOF Person detector object
UINT32                      gIpPDpdId		= 0;		// The registered ID of the Person detector (has bit 0 set, as is the only detector registered)
GT_IMAGE					gIpPDimage;
INT32						gIpPDMode		= 0;
VOID*						gIpPDPreBuf		= 0;
BOOL 						gIpPDIsStarted	= 0;
static UINT32 				gIpPDNumDetections = 0;
static T_IP_PD_PERSON		gIpPDPersons[D_IP_PD_MAX_PERSONS] = {{{0}}};

// The maximum number of buffers that can be processed 
static atomic_t nbBuffers = 3;
static const PRE_DATA *s_pre_data = NULL;

// ****************************************************************************
//                        External Method Definition
// ****************************************************************************
// Driver functions
extern VOID 	pre_addref_data(const PRE_DATA *data);
extern VOID 	pre_release_data(const PRE_DATA *data);
extern GTSTATUS pre_start();
extern void 	pre_stop();

// ****************************************************************************
//                        Local Method Definition
// ****************************************************************************

// ****************************************************************************
//                          	Method
// ****************************************************************************

/******************************************************************************
 * @brief	�����pen
 *
 * @retval	D_IP_PD_OK
 * @retval	D_IP_PD_NG_SEQUENCE
 */
INT32 IP_PD_Open( VOID )
{
	INT32	ret;

	ret = IP_FNC_SetFlag( D_IP_FNC_STATUS_PD );
	if( ret ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_ALREADY_OPEN);
	}

	return D_IP_PD_OK;
}

/******************************************************************************
 * @brief	�����lose
 *
 * @retval	D_IP_PD_OK
 * @retval	D_IP_PD_NG_SEQUENCE
 */
INT32 IP_PD_Close( VOID )
{
	SHORT	flag;
	INT32		ret;

	flag = IP_FNC_GetFlag();
	if ( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	ret = IP_FNC_ResetFlag( D_IP_FNC_STATUS_PD );
	if( ret ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	// In case we locked maps from PRE and haven't released them
	if (s_pre_data)
	{
		pre_release_data(s_pre_data);
		s_pre_data = NULL;
	}
	nbBuffers = 3;

	//======= Destroy  ============
	if(gIpPD_Status & D_IP_PD_STATUS_REGISTER){
		gIpPDmgr->vft->UnregisterDetectors(gIpPDmgr, gIpPDpdId);
		gIpPD_Status &= ~(D_IP_PD_STATUS_REGISTER);
	}

	if(gIpPD_Status & D_IP_PD_STATUS_MANAGER){
		gIpPDmgr->vft->Destroy(gIpPDmgr);
		gIpPD_Status &= ~(D_IP_PD_STATUS_MANAGER);
	}

	if(gIpPD_Status & D_IP_PD_STATUS_DETECTOR){
		gIpPDpd->vft->Destroy(gIpPDpd);
		gIpPD_Status &= ~(D_IP_PD_STATUS_DETECTOR);
	}

	if(gIpPD_Status & D_IP_PD_STATUS_HEAP){
		gIpFNheapPtrMgr->vft->Destroy( gIpFNheapPtrMgr );
		gIpFNheapPtrPD->vft->Destroy( gIpFNheapPtrPD );
		gIpPD_Status &= ~(D_IP_PD_STATUS_HEAP);
	}

	if (gIpPD_Status & D_IP_PD_STATUS_AHIP_NORM_START){
		AhipNormStop();
		gIpPD_Status &= ~D_IP_PD_STATUS_AHIP_NORM_START;
	}

	if (gIpPD_Status & D_IP_PD_STATUS_PRE_START){
		pre_stop();
		gIpPD_Status &= ~D_IP_PD_STATUS_PRE_START;
	}

	if (gIpPD_Status & D_IP_PD_STATUS_AHIP_PRE_START){
		AhipPreStop();
		gIpPD_Status &= ~D_IP_PD_STATUS_AHIP_PRE_START;
	}

	if(gIpPD_Status == D_IP_PD_STATUS_NONE){
		return D_IP_PD_OK;
	} else {
		gIpPD_Status = D_IP_PD_STATUS_NONE;
		return D_IP_PD_NG_SEQUENCE;
	}
	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		������
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_GetLibInfo( AHOF_LIB_INFO *info )
{
	AHOF_LIB*				ahofLib	= AhofLibGet();         // The interface to AHOF Manager library
	const	AHOF_LIB_INFO*	libInfo	= ahofLib->GetLibInfo( );

	*info = *(AHOF_LIB_INFO *)libInfo;
	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		������
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
UINT32 IP_PD_GetLibHeapSize( T_IP_PD_DETECTOR_CFG *cfg )
{
	AHOF_MANAGER_CFG			mgrCfg  = {0};					// The structure that holds the configuration of the AHOF Manager object
	AHOF_LIB*					ahofLib	= AhofLibGet();     	// The interface to AHOF Manager library
	AHOF_PERSON_LIB*			pdLib	= AhofPersonLibGet();	// The interface to AHOF Person library
	AHOF_PERSON_DETECTOR_CFG	pdCfg	= {0};					// The structure that holds the configuration of the AHOF Person Detector object
	UINT32						heap_size = 0;

	mgrCfg.structSize = sizeof(mgrCfg);

	pdCfg.structSize				= sizeof(AHOF_PERSON_DETECTOR_CFG);
	pdCfg.flags						= cfg->flags;
	pdCfg.minPersonSize				= cfg->minPersonSize;
	pdCfg.maxPersonSize				= cfg->maxPersonSize;
	pdCfg.maxPersonCount			= cfg->maxPersonCount;
	pdCfg.boostDRvsFP				= cfg->boostDRvsFP;
	pdCfg.boostDRvsSPEED			= cfg->boostDRvsSPEED;
	pdCfg.maxPersonCountForTracking = 4;

	heap_size += ahofLib->GetRequiredHeapSize(&mgrCfg);
	heap_size += pdLib->GetRequiredHeapSize(&pdCfg);
	heap_size += 10 * 10 * 64 * 64 * 9 * 3;

	return heap_size;
}

/******************************************************************************
 *	@brief		��������
 *	@param		None
 *	@return		D_IP_PD_NG_SEQUENCE
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Init( T_IP_PD_DETECTOR_CFG	*cfg )
{
	AHOF_LIB*					ahofLib		= AhofLibGet();			// The interface to AHOF Manager library
	AHOF_PERSON_LIB*			pdLib		= AhofPersonLibGet();	// The interface to AHOF Person library
	AHOF_MANAGER_CFG			mgrCfg		= {0};					// The structure that holds the configuration of the AHOF Manager object
	AHOF_PERSON_DETECTOR_CFG	pdCfg		= {0};					// The structure that holds the configuration of the AHOF Person Detector object
	GTSTATUS					status		= GTS_OK;				// The status code of the operations
	AHIP_NORM_CONFIG			norm_cfg	= {0};

	VOID					*heapBufferMgr;
	VOID					*heapBufferPD;
	VOID					*heapBufferNorm;

	UINT32					heapSizeMgr;
	UINT32					heapSizePD;
	UINT32					heapSizeNorm;
	USHORT					flag;

	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	mgrCfg.structSize = sizeof(mgrCfg);
	// Create the AHOF person detector
	pdCfg.structSize				= sizeof(AHOF_PERSON_DETECTOR_CFG);
	pdCfg.flags						= cfg->flags;
	pdCfg.minPersonSize				= cfg->minPersonSize;
	pdCfg.maxPersonSize				= cfg->maxPersonSize;
	pdCfg.maxPersonCount			= cfg->maxPersonCount;
	pdCfg.boostDRvsFP				= cfg->boostDRvsFP;
	pdCfg.boostDRvsSPEED			= cfg->boostDRvsSPEED;
    pdCfg.maxPersonCountForTracking = 4;

	gIpPDimage.format				= GT_FMT_GRAY;
	gIpPDimage.width				= cfg->width;
	gIpPDimage.height				= cfg->height;
	gIpPDimage.planes[0].stride		= cfg->planes[0].stride;
	gIpPDimage.planes[0].data		= cfg->planes[0].data;
	gIpPDimage.planes[1].stride		= cfg->planes[1].stride;
	gIpPDimage.planes[1].data		= cfg->planes[1].data;
	gIpPDimage.planes[2].stride		= cfg->planes[2].stride;
	gIpPDimage.planes[2].data		= cfg->planes[2].data;
	gIpPDimage.planes[3].stride		= cfg->planes[3].stride;
	gIpPDimage.planes[3].data		= cfg->planes[3].data;

	//======= Get Heap Size  ============
	heapSizeMgr		= ahofLib->GetRequiredHeapSize(&mgrCfg);
	heapSizePD		= pdLib->GetRequiredHeapSize(&pdCfg);
	heapSizeNorm	= 10 * 10 * 64 * 64 * 9 * 3;
	heapBufferMgr	= cfg->heapBuffer;
	heapBufferPD	= heapBufferMgr + heapSizeMgr;
	heapBufferNorm	= heapBufferPD + heapSizePD;
	cfg->heapBuffer	= heapBufferNorm + heapSizeNorm;
	gIpPDPreBuf		= cfg->heapBuffer;

	//======== Start NORM ============
	norm_cfg.fillValue   = 0x80;
	norm_cfg.stepX       = 1;
    norm_cfg.stepY       = 1;
    norm_cfg.shift       = 1;
    norm_cfg.select[0]   = 0x00801801; //1
    norm_cfg.select[1]   = 0x00006006; //2
    norm_cfg.select[2]   = 0x00018018; //3
    norm_cfg.select[3]   = 0x00060060; //4
    norm_cfg.select[4]   = 0x00180180; //5
    norm_cfg.select[5]   = 0x00600600; //6
    norm_cfg.select[6]   = 0x00000000; //7
    norm_cfg.select[7]   = 0x00000000; //8

	status = AhipNormStart( &norm_cfg, heapBufferNorm, heapSizeNorm );
	if( status != GTS_OK && status != GTS_BUSY ){
		printf("AhipNormStart() : NG =0x%x\n", status);
		return	(D_IP_PD_NG_START_PREVIEW | (status & 0xFF));
	}
	
	if (status == GTS_OK) {
		gIpPD_Status |= D_IP_PD_STATUS_AHIP_NORM_START;
	}

	//======= Create Heap  ============
	status = ahofLib->CreateHeap( heapBufferMgr, heapSizeMgr, &gIpFNheapPtrMgr );
	if( status != GTS_OK ){
		return	(D_IP_PD_NG_CREATE_HEAP | (status & 0xFF));
	}
	status = ahofLib->CreateHeap( heapBufferPD, heapSizePD, &gIpFNheapPtrPD );
	if( status != GTS_OK ){
		return	(D_IP_PD_NG_CREATE_HEAP | (status & 0xFF));
	}
	gIpPD_Status |= D_IP_PD_STATUS_HEAP;

	//======= Create AHOF person detector  ============
	status = pdLib->CreateDetector( gIpFNheapPtrPD, &pdCfg, &gIpPDpd );
	if( status != GTS_OK ){
		return	(D_IP_PD_NG_CREATE_DETECTOR | (status & 0xFF));
	}
	gIpPD_Status |= D_IP_PD_STATUS_DETECTOR;

	//======= Create AHOF Manager  ============
	mgrCfg.structSize = sizeof(AHOF_MANAGER_CFG);

	status = ahofLib->CreateManager(gIpFNheapPtrMgr, &mgrCfg, &gIpPDmgr);
	if (status != GTS_OK){
		return	(D_IP_PD_NG_CREATE_MANAGER | (status & 0xFF));
	}
	gIpPD_Status |= D_IP_PD_STATUS_MANAGER;

	// Register the person detector
	status = gIpPDmgr->vft->RegisterDetector(gIpPDmgr, (AHOF_DETECTOR*)gIpPDpd, NULL, &gIpPDpdId);
	if (status != GTS_OK){
		return	(D_IP_PD_NG_RESISTER_DETECTOR | (status & 0xFF));
	}
	gIpPD_Status |= D_IP_PD_STATUS_REGISTER;
	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		Object Detect Start
 *	@param		None
 *	@return		D_IP_PD_NG_SEQUENCE
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Start( INT32 mode )
{
	GTSTATUS			status	= GTS_OK;				// The status code of the operations
	AHOF_PREVIEW_CFG	previewCfg = {0};	// The structure that holds the preview configuration
	USHORT				flag;

	flag = IP_FNC_GetFlag( );
	if( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	if( gIpPD_Status & D_IP_PD_STATUS_START ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_ALREADY_START);
	}

	gIpPDMode = mode;

	if(gIpPDMode == E_IP_PD_MODE_PREVIEW){
		// Start the preview mode at VGA with triple buffering
		previewCfg.structSize   		= sizeof(AHOF_PREVIEW_CFG);
		previewCfg.flags        		= AHOF_PREVIEW_MAP_YUV;    
		previewCfg.size.width   		= D_IP_PD_DET_W;	// 640;
		previewCfg.size.height  		= D_IP_PD_DET_H;	// 360;
#ifdef HIGHRESO
        previewCfg.sizeHighRes.width	= D_IP_PD_DET_W * 2;
        previewCfg.sizeHighRes.height	= D_IP_PD_DET_H * 2;
#else
        previewCfg.sizeHighRes.width	= 0;
        previewCfg.sizeHighRes.height	= 0;
#endif
		status = gIpPDmgr->vft->StartPreview(gIpPDmgr, gIpPDpdId, &previewCfg);
		if (status != GTS_OK){
			return	(D_IP_PD_NG_START_PREVIEW | (status & 0xFF));
		}
	}
	gIpPD_Status |= D_IP_PD_STATUS_START;

	gIpPDIsStarted = 1;

	return D_IP_PD_OK ;
}

/******************************************************************************
 *	@brief		Object Detect Stop
 *	@param		None
 *	@return		D_IP_PD_NG_SEQUENCE
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Stop( VOID )
{
	GTSTATUS			status	= GTS_OK;				// The status code of the operations
	USHORT				flag;

	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	if( ~gIpPD_Status & D_IP_PD_STATUS_START ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_START);
	}
	gIpPD_Status &= ~D_IP_PD_STATUS_START;

	if(gIpPDMode == E_IP_PD_MODE_PREVIEW){
		status = gIpPDmgr->vft->StopPreview(gIpPDmgr);
		if (status != GTS_OK){
			return	(D_IP_PD_NG_STOP_PREVIEW | (status & 0xFF));
		}
	}

	gIpPDIsStarted = 0;

	return D_IP_PD_OK ;
}

/******************************************************************************
 *	@brief		Object Detect Main
 *	@param		i:level (0:Low/1:High))
 *	@return		D_IP_PD_NG_SEQUENCE
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Detect( VOID )
{
	GTSTATUS			status	= GTS_OK;				// The status code of the operations
#ifdef EVAL
	OS_USER_SYSTIM		t_end;
#endif
	USHORT				flag;

	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	if( ~gIpPD_Status & D_IP_PD_STATUS_START ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_START);
	}

#ifdef EVAL
//	chg_pri ( TSK_SELF, 1 ) ;
	BF_Measure_Time_Start(E_BF_MEASURE_TIME_ID_DEBUG_3);
#endif
	if(gIpPDMode == E_IP_PD_MODE_PLAYBACK){
		// Start the playback mode on the input image
		status = gIpPDmgr->vft->StartPlayback(gIpPDmgr, gIpPDpdId, &gIpPDimage, NULL);
		if (status != GTS_OK){
			return	(D_IP_PD_NG_START_PLAYBACK | (status & 0xFF));
		}
		// Wait for the results
		status = gIpPDmgr->vft->WaitPlaybackResults(gIpPDmgr, &gIpPDresults);
		if (status != GTS_OK){
			return	(D_IP_PD_NG_PLAYBACK_RESULT | (status & 0xFF));
		}
	} else {
		status = gIpPDmgr->vft->WaitPreviewResults(gIpPDmgr, &gIpPDresults);
		if (status != GTS_OK){
			return	(D_IP_PD_NG_PREVIEW_RESULT | (status & 0xFF));
		}
	}
#ifdef EVAL
	BF_Measure_Time_Get(E_BF_MEASURE_TIME_ID_DEBUG_3, &t_end);
//	chg_pri ( TSK_SELF, TPRI_INI ) ;
	ex_time += t_end;
#endif

	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		IP_PD_Get_Person
 *	@param		None
 *	@return		D_IP_PD_NG_SEQUENCE
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Get_Person( T_IP_PD_PERSON *person, UINT32 *np )
{
	// Print results
	UINT32          nbPersons   = gIpPDresults->nbDetections[0];
	USHORT			flag;
	UINT32			ret;
	AHOF_PERSON		ps;

	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	if( ~gIpPD_Status & D_IP_PD_STATUS_START ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_START);
	}

	OS_User_Wai_Sem(SID_AHIP_PD);

	if( nbPersons > D_IP_PD_MAX_PERSONS) {
		nbPersons = D_IP_PD_MAX_PERSONS;
	}
	*np = nbPersons;
	for (int i = 0; i < nbPersons; i++){
		ps = ((AHOF_PERSON*)gIpPDresults->detArray[0])[i];
		person[i].rect.left		= ps.center.x - ps.size.width  / 2;
		person[i].rect.top		= ps.center.y - ps.size.height / 2;
		person[i].rect.width	= ps.size.width;
		person[i].rect.height	= ps.size.height;
		person[i].rect.x		= ps.center.x;
		person[i].rect.y		= ps.center.y;
		person[i].yawAngle		= ps.yawAngle;
		person[i].pitchAngle	= ps.pitchAngle;
		person[i].rollAngle		= ps.rollAngle;
		person[i].type			= ps.type;
		person[i].trackid		= ps.trackId;		
		person[i].confidence	= ps.confidence;
	}

	OS_User_Sig_Sem(SID_AHIP_PD);

	ret = gIpPDmgr->vft->FreeResults(gIpPDmgr, gIpPDresults);
	if(ret){
		return (D_IP_PD_NG_FREE_RESULT | (ret & 0xFF));
	}else{
		return D_IP_PD_OK;
	}
}

INT32 IP_PD_Get_Person_List( T_IP_PD_PERSON *person, UINT32 *np )
{
	USHORT			flag;

	flag = IP_FNC_GetFlag( );
	if ( (~flag & D_IP_FNC_STATUS_PD) ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_OPEN);
	}

	if( ~gIpPD_Status & D_IP_PD_STATUS_START ){
		return (D_IP_PD_NG_SEQUENCE | D_IP_PD_NOT_YET_START);
	}

	OS_User_Wai_Sem(SID_AHIP_PD);
	*np = gIpPDNumDetections;
	if( *np > D_IP_PD_MAX_PERSONS) {
		*np = D_IP_PD_MAX_PERSONS;
	}
	for(int i=0; i<*np; i++){
		#if 0
#ifdef HIGHRESO
		person[i].rect.left    = PD_CONV_POS(gIpPDPersons[i].rect.left  , gIpPDimage.width , D_IP_PD_DET_W*2);
		person[i].rect.top     = PD_CONV_POS(gIpPDPersons[i].rect.top   , gIpPDimage.height, D_IP_PD_DET_H*2);
		person[i].rect.width   = PD_CONV_POS(gIpPDPersons[i].rect.width , gIpPDimage.width , D_IP_PD_DET_W*2);
		person[i].rect.height  = PD_CONV_POS(gIpPDPersons[i].rect.height, gIpPDimage.height, D_IP_PD_DET_H*2);
#else
		person[i].rect.left    = PD_CONV_POS(gIpPDPersons[i].rect.left  , gIpPDimage.width , D_IP_PD_DET_W);
		person[i].rect.top     = PD_CONV_POS(gIpPDPersons[i].rect.top   , gIpPDimage.height, D_IP_PD_DET_H);
		person[i].rect.width   = PD_CONV_POS(gIpPDPersons[i].rect.width , gIpPDimage.width , D_IP_PD_DET_W);
		person[i].rect.height  = PD_CONV_POS(gIpPDPersons[i].rect.height, gIpPDimage.height, D_IP_PD_DET_H);
#endif
		#else
		person[i].rect.left    = gIpPDPersons[i].rect.left;
		person[i].rect.top     = gIpPDPersons[i].rect.top;
		person[i].rect.width   = gIpPDPersons[i].rect.width;
		person[i].rect.height  = gIpPDPersons[i].rect.height;
		#endif
		person[i].yawAngle     = gIpPDPersons[i].yawAngle;
		person[i].pitchAngle   = gIpPDPersons[i].pitchAngle;
		person[i].rollAngle    = gIpPDPersons[i].rollAngle;
		person[i].type         = gIpPDPersons[i].type;
		person[i].trackid      = gIpPDPersons[i].trackid;
		person[i].confidence   = gIpPDPersons[i].confidence;

		person[i].rect.left   = GT_MIN(gIpPDimage.width, GT_MAX(0, person[i].rect.left));
		person[i].rect.top    = GT_MIN(gIpPDimage.height, GT_MAX(0, person[i].rect.top));
		person[i].rect.width  = GT_MIN(gIpPDimage.width  - person[i].rect.left, GT_MAX(0, person[i].rect.width ));
		person[i].rect.height = GT_MIN(gIpPDimage.height - person[i].rect.top , GT_MAX(0, person[i].rect.height));
	}

	OS_User_Sig_Sem(SID_AHIP_PD);

	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		IP_PD_Get_Time
 *	@param		None
 *	@return		D_IP_PD_OK
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Get_Time( UINT32 *time )
{
#ifdef EVAL
	*time = ex_time;
#endif
	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		IP_PD_Reset_Time
 *	@param		None
 *	@return		D_IP_PD_OK
 *	@note		None
 *	@attention	None
 */
INT32 IP_PD_Reset_Time( VOID )
{
#ifdef EVAL
	ex_time = 0;
#endif
	return D_IP_PD_OK;
}

/******************************************************************************
 *	@brief		Get Version
 *	@param		o:**str (version strings)
 *	@return		D_IP_PD_OK / D_IP_PD_NG_PARAM
 *	@note		None
 *	@attention	None
 */

INT32 IP_PD_Get_Version( CHAR **str )
{
	if ( str == NULL ) {
		return D_IP_PD_NG_PARAM ;
	}

//    *str = (CHAR*)strchr( V_Ip_Pd_Version, ':' ) + 1;
	*str = (CHAR*)(&V_Ip_Pd_Version[13]);
	return D_IP_PD_OK;
}
INT32 IP_PD_Task_Detect_Start(T_IP_PD_PLANE_DATA plane_data)
{
	OS_USER_ER result = 0;

	gIpPDimage.planes[0].stride		= plane_data.plane[0].stride;
	gIpPDimage.planes[0].data		= plane_data.plane[0].data;
	gIpPDimage.planes[1].stride		= plane_data.plane[1].stride;
	gIpPDimage.planes[1].data		= plane_data.plane[1].data;
	gIpPDimage.planes[2].stride		= plane_data.plane[2].stride;
	gIpPDimage.planes[2].data		= plane_data.plane[2].data;

	result = OS_User_Set_Flg( FID_AHIP_PD, FLG_AHIP_PD_START );

	return result;
}

INT32 IP_PD_Task_Start(VOID)
{
	T_OS_USER_RTSK	pk_rtsk;
	OS_USER_ER result = 0;

	result = OS_User_Clr_Flg( FID_AHIP_PD, ~FLG_AHIP_PD_WAITPTN );
	if (result) {
		printf("OS_User_Clr_Flg FID_AHIP_PD errod:%d", result);
		return result;
	}
	result = OS_User_Sta_Tsk(TID_AHIP_PD, 0);
	if (result) {
		printf("OS_User_Sta_Tsk TID_AHIP_PD errod:%d", result);
	}

	OS_User_Ref_Tsk( TID_AHIP_PD, &pk_rtsk );
	while( pk_rtsk.tskstat != D_OS_USER_TTS_WAI ) {
		OS_User_Dly_Tsk( 1 );
		OS_User_Ref_Tsk( TID_AHIP_PD, &pk_rtsk );
	}

	return result;
}

void IP_PD_Task_End()
{
	T_OS_USER_RTSK	pk_rtsk;

	OS_User_Ref_Tsk( TID_AHIP_PD, &pk_rtsk );

	if( pk_rtsk.tskstat == D_OS_USER_TTS_DMT ) {
		return;
	}

	OS_User_Set_Flg( FID_AHIP_PD, FLG_AHIP_PD_TERMINATE );

	// wait for terminate
	OS_User_Ref_Tsk( TID_AHIP_PD, &pk_rtsk );

	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ) {
		OS_User_Dly_Tsk( 1 );
		OS_User_Ref_Tsk( TID_AHIP_PD, &pk_rtsk );
	}
}

void IP_PD_Main( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn;
	INT32			ret;
	OS_USER_ER		result;

	ret = IP_PD_Start(E_IP_PD_MODE_PLAYBACK);
	if( ret == D_IP_PD_OK ){
		while ( 1 ) {
			result = OS_User_Wai_Flg(FID_AHIP_PD, FLG_AHIP_PD_START|FLG_AHIP_PD_TERMINATE, D_OS_USER_TWF_ORW, &flgptn);
			// terminate
			if( result == D_OS_USER_E_OK && (flgptn & FLG_AHIP_PD_TERMINATE) ) {
				OS_User_Clr_Flg( FID_AHIP_PD, ~FLG_AHIP_PD_TERMINATE );
				break;
			}

			if( result == D_OS_USER_E_OK && (flgptn & FLG_AHIP_PD_START) ) {
				OS_User_Clr_Flg( FID_AHIP_PD, ~FLG_AHIP_PD_START );
				// PD detect
				ret = IP_PD_Detect();
				if( ret != D_IP_PD_OK ){
					gIpPDresults->nbDetections[0] = 0;
					printf( "ip_pd_start_pd_detect Error: ret=0x%x\n", ret );
				}
				if (ret == D_IP_PD_OK ) {
					ret = IP_PD_Get_Person(gIpPDPersons, &gIpPDNumDetections);
					if( ret != D_IP_PD_OK ){
						printf( "IP_PD_Get_Person Error: ret=0x%x\n", ret );
					}
#ifdef D_DETECT_INFO_PRINT
					static UINT32 print_cnt = 0;
					if( print_cnt % 3 == 0 ) {
						if( gIpPDNumDetections ){
							for(UINT32 print_count=0; print_count<gIpPDNumDetections; print_count++){
								printf("gIpPDDetectData[%d].rect = %dx%d(%d, %d) conf:%d\n",
										print_count,
										gIpPDPersons[print_count].rect.width, gIpPDPersons[print_count].rect.height,
										gIpPDPersons[print_count].rect.left, gIpPDPersons[print_count].rect.top, gIpPDPersons[print_count].confidence);
							}
						}
					}
					print_cnt++;
#endif	// D_DETECT_INFO_PRINT
				}
			}
		}
	} else {
		printf( "IP_PD_Start Error: ret=0x%x\n", ret );
	}

	// PD stop
	ret = IP_PD_Stop();
	if( ret != D_IP_PD_OK ){
		printf( "IP_PD_Stop Error: ret=0x%x\n", ret );
	}
	OS_User_Ext_Tsk();
}

// PRE WR interrupt callback
void pd_data_callback( const PRE_DATA *data )
{
	if (gIpPDIsStarted)
	{
		if (s_pre_data != NULL)
		{
			const PRE_DATA *pre_data_tmp = s_pre_data;
			s_pre_data = NULL;
			pre_release_data(pre_data_tmp);
		}

		pre_addref_data(data);
		s_pre_data = data;
		OS_User_Set_Flg( FID_AHIP_PD, FLG_AHIP_PD_START );
	}
}

// PD callback
GTSTATUS pd_get_maps(OUT PRE_DATA** maps)
{
	const PRE_DATA *pre_data = NULL;

	do
	{
		OS_USER_FLGPTN	flgptn;

		// Wait for new frame from camera
		OS_User_Wai_Flg( FID_AHIP_PD, FLG_AHIP_PD_START, D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_AHIP_PD, ~FLG_AHIP_PD_START );

		// Do this while we don't have any buffer available
	} while (nbBuffers == 0 || (pre_data = s_pre_data) == NULL);
	s_pre_data = NULL;
	atomic_dec(&nbBuffers);

	*maps = (PRE_DATA *)pre_data;

	return GTS_OK;
}

// PD callback
GTSTATUS pd_release_maps(IN PRE_DATA* maps)
{
	pre_release_data(maps);
	atomic_inc(&nbBuffers);

	return GTS_OK;
}

