/**
 * @file		myheapctrl.c
 * @brief		heap contorl functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <reent.h>
#include <stdlib.h>
#include <string.h>

/* ////////////////////////////////////////////////////////////////////////////
; DEFINE
//////////////////////////////////////////////////////////////////////////// */
#define MDF_HEAP_SIZE		0x00100000		/* for MDF. byte size. align 64 byte */

#define MYHEAP_ALIGN	(64)			/* 64 byte */
#define HEADER_SIZE		MYHEAP_ALIGN	/* aligned header size */

#if 1
#define ROUND(var)	((var+MYHEAP_ALIGN-1)/MYHEAP_ALIGN)*MYHEAP_ALIGN;
#else
#define ROUND(var,type)	((var+sizeof(type)-1)/sizeof(type))*sizeof(type);
#endif


#ifdef _DEBUG
#include <stdio.h>
#define ErrorTrap( message )	fprintf( stderr, message ); for(;;){}
#else
#define ErrorTrap(x)
#endif

#ifndef NULL
#define NULL		0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

/* ////////////////////////////////////////////////////////////////////////////
; INCLUDE
//////////////////////////////////////////////////////////////////////////// */
#include "dd_arm.h"

/* ////////////////////////////////////////////////////////////////////////////
; TYPEDEF
//////////////////////////////////////////////////////////////////////////// */
/* Queue */
typedef struct tqueue
{
	struct tqueue	*next;
	struct tqueue	*previous;
} TQueue;

/* Memory */
typedef int		TMemory;

/* Variable Size Memory Pool Control Block */
typedef struct
{
	TQueue		memoryblockqueue;
} TVariableSizeMemoryControlBlock;

typedef int		BOOL;

/* Variable Size Memory Block */
typedef struct
{
	TQueue		blockqueue;		/* memory block queue */
	unsigned	blocksize;
	BOOL		usedflag;
} TVariableSizeMemoryBlock;

/* ////////////////////////////////////////////////////////////////////////////
; INTERNAL WORK AREA
//////////////////////////////////////////////////////////////////////////// */
/* normal heap */
ULONG gMyHeapSpinVar __attribute__((section(".LOCK_SECTION"), aligned(64)));
TVariableSizeMemoryControlBlock xMyHeapCtrlBlock;

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
/* heap for MDF */
ULONG gMdfHeapSpinVar __attribute__((section(".LOCK_SECTION"), aligned(64)));
UCHAR gMdfHeap[MDF_HEAP_SIZE] __attribute__((section(".MDF_HEAP_SECTION"), aligned(64)));
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
TVariableSizeMemoryControlBlock xMdfHeapCtrlBlock;

/* ////////////////////////////////////////////////////////////////////////////
; PROGRAM
//////////////////////////////////////////////////////////////////////////// */
/*=============================================================================
; Queue Handling Functions
=============================================================================*/
/*-----------------------------------------------------------------------------
; Unic queue [primitive]
-----------------------------------------------------------------------------*/
static void UnicQueue( TQueue *queue )
{
	queue->next		= queue;
	queue->previous = queue;
}

/*-----------------------------------------------------------------------------
; Add it to queue [primitive]
-----------------------------------------------------------------------------*/
static void AddQueueNext( TQueue *controlqueue, TQueue *aqueue )
{
	aqueue->next					= controlqueue->next;
	aqueue->previous				= controlqueue;
	controlqueue->next->previous	= aqueue;
	controlqueue->next				= aqueue;
}

static void AddQueuePrevious( TQueue *controlqueue, TQueue *aqueue )
{
	aqueue->next					= controlqueue;
	aqueue->previous				= controlqueue->previous;
	controlqueue->previous->next	= aqueue;
	controlqueue->previous			= aqueue;
}

/*-----------------------------------------------------------------------------
; Delete it from queue [primitive]
-----------------------------------------------------------------------------*/
static void DeleteQueue( TQueue *queue )
{
	queue->next->previous = queue->previous;
	queue->previous->next = queue->next;
}


/*=============================================================================
; variable size memory
=============================================================================*/
/*-----------------------------------------------------------------------------
; Initialize variable size memory pool
-----------------------------------------------------------------------------*/
void InitializeVariableSizeMemory(
	TVariableSizeMemoryControlBlock *variablesizememorycontrolblock,
	TMemory *startaddress,
	TMemory *endaddress )
{
	TVariableSizeMemoryBlock *variablesizememoryblock;

#ifdef _DEBUG
	if( variablesizememorycontrolblock == NULL )
	{
		ErrorTrap( "InitializeVariableSizeMemory : variablesizememorycontrolblock" );
	}
	if( startaddress == NULL )
	{
		ErrorTrap( "InitializeVariableSizeMemory : startaddress" );
	}
	if( endaddress == NULL )
	{
		ErrorTrap( "InitializeVariableSizeMemory : endaddress" );
	}
	if( (unsigned long)startaddress + (unsigned long)HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */
		>= (unsigned long)endaddress )
	{
		ErrorTrap( "InitializeVariableSizeMemory : memory size" );
	}
#endif

	UnicQueue( &(variablesizememorycontrolblock->memoryblockqueue) );
	AddQueuePrevious( &(variablesizememorycontrolblock->memoryblockqueue),
			  (TQueue *)startaddress );
	variablesizememoryblock = (TVariableSizeMemoryBlock *)startaddress;
	variablesizememoryblock->blocksize = (unsigned)
		((unsigned long)endaddress -
		 (unsigned long)startaddress -
		 (unsigned long)HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */ );
	variablesizememoryblock->usedflag = FALSE;
}

/*-----------------------------------------------------------------------------
; Merge memory block
-----------------------------------------------------------------------------*/
static void MergeMemoryBlock(
	TVariableSizeMemoryBlock *before,
	TVariableSizeMemoryBlock *after )
{
	if( !(before->usedflag) && !(after->usedflag) )
	{
		if( (unsigned long)before
			+ (unsigned long)HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */
			+ (unsigned long)(before->blocksize)
			== (unsigned long)after )
		{
			before->blocksize +=
				(HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */ + after->blocksize);
			DeleteQueue( (TQueue *)after );
		}
	}
}

/*-----------------------------------------------------------------------------
; Search free memory block
-----------------------------------------------------------------------------*/
static TVariableSizeMemoryBlock *SearchFreeMemory(
	TVariableSizeMemoryControlBlock *variablesizememorycontrolblock,
	unsigned requestsize )
{
	TVariableSizeMemoryBlock *variablesizememoryblock;

	for(
		variablesizememoryblock =
			(TVariableSizeMemoryBlock *)(variablesizememorycontrolblock->memoryblockqueue.next);
		variablesizememoryblock !=
			(TVariableSizeMemoryBlock *)&(variablesizememorycontrolblock->memoryblockqueue);
		variablesizememoryblock =
			(TVariableSizeMemoryBlock *)(variablesizememoryblock->blockqueue.next) )
	{
		if( variablesizememoryblock->usedflag )
		{
			continue;
		}
		if( variablesizememoryblock->blocksize >= requestsize )
		{
			return variablesizememoryblock;
		}
	}
	return NULL;
}

/*-----------------------------------------------------------------------------
; Split memory block
-----------------------------------------------------------------------------*/
static void SplitMemoryBlock(
	TVariableSizeMemoryBlock *variablesizememoryblock,
	unsigned requestsize )
{
	int newsize;
	TVariableSizeMemoryBlock *newvariablesizememoryblock;
	
	newsize =
		(int)(variablesizememoryblock->blocksize) -
		HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */ -
		(int)requestsize;
	if( newsize > 0  )
	{
		variablesizememoryblock->blocksize = requestsize;
		newvariablesizememoryblock = (TVariableSizeMemoryBlock *)
			((unsigned long)variablesizememoryblock + 
			 (unsigned long)HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */  +
			 (unsigned long)requestsize);
		newvariablesizememoryblock->blocksize = newsize;
		newvariablesizememoryblock->usedflag = FALSE;
		AddQueueNext( (TQueue *) &(variablesizememoryblock->blockqueue),
				  (TQueue *)newvariablesizememoryblock );
	}
}

/*-----------------------------------------------------------------------------
; Release variable size memory
-----------------------------------------------------------------------------*/
void ReleaseVariableSizeMemory(
	TVariableSizeMemoryControlBlock *variablesizememorycontrolblock,
	TMemory *memoryblock )
{
	TVariableSizeMemoryBlock *variablesizememoryblock;
	TVariableSizeMemoryBlock *nextvariablesizememoryblock;
	TVariableSizeMemoryBlock *previousvariablesizememoryblock;
//	unsigned statusregister;

	if( memoryblock == NULL )
	{
		return;
	}

//	statusregister = GetStatusRegisterAndDisableInterrupt();

#ifdef _DEBUG
	if( variablesizememorycontrolblock == NULL )
	{
		ErrorTrap( "ReleaseVariableSizeMemory : variablesizememorycontrolblock" );
	}
#endif

	variablesizememoryblock = (TVariableSizeMemoryBlock *)
		((unsigned long)memoryblock -
		 (unsigned long)HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */ );
	
	if( !(variablesizememoryblock->usedflag) )
	{
		return;
	}

	variablesizememoryblock->usedflag = FALSE;

	nextvariablesizememoryblock =
		(TVariableSizeMemoryBlock *)(variablesizememoryblock->blockqueue.next);
	if( nextvariablesizememoryblock !=
		(TVariableSizeMemoryBlock *)&(variablesizememorycontrolblock->memoryblockqueue) )
	{
		MergeMemoryBlock( variablesizememoryblock, nextvariablesizememoryblock );
	}

	previousvariablesizememoryblock =
		(TVariableSizeMemoryBlock *)(variablesizememoryblock->blockqueue.previous);
	if( previousvariablesizememoryblock !=
		(TVariableSizeMemoryBlock *)&(variablesizememorycontrolblock->memoryblockqueue) )
	{
		MergeMemoryBlock( previousvariablesizememoryblock, variablesizememoryblock );
	}

//	SetStatusRegister( statusregister );
}

/*-----------------------------------------------------------------------------
; Request variable size memory
-----------------------------------------------------------------------------*/
TMemory *RequestVariableSizeMemory(
	TVariableSizeMemoryControlBlock *variablesizememorycontrolblock,
	unsigned requestsize )
{
	TMemory *answer;
	TVariableSizeMemoryBlock *variablesizememoryblock;
	
//	unsigned statusregister;

//	statusregister = GetStatusRegisterAndDisableInterrupt();

#ifdef _DEBUG
	if( variablesizememorycontrolblock == NULL )
	{
		ErrorTrap( "RequestvariableariableSizeMemory : variablesizememorycontrolblock" );
	}
	if( requestsize == 0 )
	{
		ErrorTrap( "RequestvariableariableSizeMemory : requestsize" );
	}
#endif

#if 1
	requestsize = ROUND( requestsize );
#else
	requestsize = ROUND( requestsize, int );
#endif

	variablesizememoryblock =
		SearchFreeMemory( variablesizememorycontrolblock, requestsize );
	
	if( variablesizememoryblock != NULL )
	{
		SplitMemoryBlock( variablesizememoryblock, requestsize );
		variablesizememoryblock->usedflag = TRUE;
		answer = (TMemory *)
			((unsigned long)variablesizememoryblock +
			 (unsigned long)HEADER_SIZE /* sizeof( TVariableSizeMemoryBlock ) */ );
	}
	else
	{
		answer = NULL;
	}
	
//	SetStatusRegister( statusregister );
	return answer;
}

extern unsigned __basefw_heap_start;
extern unsigned __basefw_heap_end;

void myheap_init()
{
	InitializeVariableSizeMemory(
		&xMyHeapCtrlBlock,
		(TMemory *) &__basefw_heap_start,
		(TMemory *) &__basefw_heap_end);
#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
	InitializeVariableSizeMemory(
		&xMdfHeapCtrlBlock,
		(TMemory *) gMdfHeap,
		(TMemory *) &gMdfHeap[MDF_HEAP_SIZE]);
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---
}

void *malloc(size_t size)
{
	void *ans;
	Dd_ARM_Critical_Section_Start( gMyHeapSpinVar );
	ans = RequestVariableSizeMemory( &xMyHeapCtrlBlock, size );
	Dd_ARM_Critical_Section_End( gMyHeapSpinVar );
	return ans;
}

void *_malloc_r(struct _reent *reent_ptr, size_t size)
{
	void *ans;
	Dd_ARM_Critical_Section_Start( gMyHeapSpinVar );
	ans = RequestVariableSizeMemory( &xMyHeapCtrlBlock, size );
	Dd_ARM_Critical_Section_End( gMyHeapSpinVar );
	return ans;
}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
void *mdf_malloc(size_t size)
{
	void *ans;
	Dd_ARM_Critical_Section_Start( gMdfHeapSpinVar );
	ans = RequestVariableSizeMemory( &xMdfHeapCtrlBlock, size );
	Dd_ARM_Critical_Section_End( gMdfHeapSpinVar );
	return ans;
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

void free(void *ptr)
{
	Dd_ARM_Critical_Section_Start( gMyHeapSpinVar );
	ReleaseVariableSizeMemory( &xMyHeapCtrlBlock, ptr );
	Dd_ARM_Critical_Section_End( gMyHeapSpinVar );
}

void _free_r(struct _reent *reent_ptr, void *ptr)
{
	Dd_ARM_Critical_Section_Start( gMyHeapSpinVar );
	ReleaseVariableSizeMemory( &xMyHeapCtrlBlock, ptr );
	Dd_ARM_Critical_Section_End( gMyHeapSpinVar );
}

#if defined(CO_FS_USED_MDF)				// --- REMOVE_MDF BEGIN ---
void mdf_free(void *ptr)
{
	Dd_ARM_Critical_Section_Start( gMdfHeapSpinVar );
	ReleaseVariableSizeMemory( &xMdfHeapCtrlBlock, ptr );
	Dd_ARM_Critical_Section_End( gMdfHeapSpinVar );
}
#endif // !CO_FS_USED_MDF				// --- REMOVE_MDF END ---

void *_calloc_r(struct _reent *reent_ptr, size_t nmemb, size_t size)
{
	size_t sz = nmemb * size;
	void *p;
	p = _malloc_r(reent_ptr, sz);
	if( NULL == p ) {
		return NULL;
	}
	return memset(p, 0, sz);
}

void *_realloc_r(struct _reent *reent_ptr, void *ptr, size_t size)
{
	void *p;
	p = _malloc_r(reent_ptr, size);
	if( NULL == p ) {
		return NULL;
	}
	memcpy( p, ptr, size );
	_free_r( reent_ptr, ptr );
	return p;
}

/* end of file ///////////////////////////////////////////////////////////// */
