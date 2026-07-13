/**
 * @file		mprofile.c
 * @brief		Milbeaut Profile
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include <stdlib.h>
#include "command_entry.h"
#include "driver_common.h"
#include "dd_arm.h"
#include "dd_gic.h"
#include "dd_tmr32.h"
#include "fs_if.h"
#include "l1l2cache.h"
#include "interrupt_set.h"

#ifdef CO_MPROFILE_ON
extern VOID cmd_mp_main(int argc, char** argv);
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_MPROFILE_TIMER32_CH4				(4)
#define D_MPROFILE_TIMER32_CH4_INTID		(E_DD_GIC_INTID_TIMER_CH0_INT + D_MPROFILE_TIMER32_CH4)
#define D_MPROFILE_TIMER32_CH4_ICDISR_BIT	(1 << (D_MPROFILE_TIMER32_CH4_INTID % 32))
#define D_MPROFILE_TIMER32_CH11				(11)
#define D_MPROFILE_TIMER32_CH11_INTID		(E_DD_GIC_INTID_TIMER_CH0_INT + D_MPROFILE_TIMER32_CH11)
#define D_MPROFILE_TIMER32_CH11_ICDISR_BIT	(1 << (D_MPROFILE_TIMER32_CH11_INTID % 32))
#define D_MPROFILE_TIMER32_CH12				(12)
#define D_MPROFILE_TIMER32_CH12_INTID		(E_DD_GIC_INTID_TIMER_CH0_INT + D_MPROFILE_TIMER32_CH12)
#define D_MPROFILE_TIMER32_CH12_ICDISR_BIT	(1 << (D_MPROFILE_TIMER32_CH12_INTID % 32))
#define D_TIMER_200uS						(200)
#define D_TIMER_1mS							(1000)
#define D_MPROFILE_PERIOD					(D_TIMER_200uS)

#define D_MPROFILE_LOG_SIZE					(0x00100000)

#define D_MPROFILE_CPU0						(1)
#define D_MPROFILE_CPU1						(2)
#define D_MPROFILE_CPU2						(4)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
volatile UINT32 gProfile_Address0 = 0;
volatile UINT32 gProfile_Address1 = 0;
volatile UINT32 gProfile_Address2 = 0;
volatile UINT32 gProfile_Start_Address0 = 0;
volatile UINT32 gProfile_Start_Address1 = 0;
volatile UINT32 gProfile_Start_Address2 = 0;
volatile UINT32 gProfile_End_Address0 = 0;
volatile UINT32 gProfile_End_Address1 = 0;
volatile UINT32 gProfile_End_Address2 = 0;

volatile UINT32 gProfile_usec = D_MPROFILE_PERIOD;

const  T_CMD_ENTRY gCmd_MP_Entry_Table[] = {
#ifdef CO_MPROFILE_ON
	{"mprofile", {cmd_mp_main, NULL}},
#endif
	{NULL, {NULL, NULL}}
};

#ifdef __GNUC__
static volatile unsigned char gMprofile_Log_Area0[D_MPROFILE_LOG_SIZE] __attribute__((aligned(32)));
static volatile unsigned char gMprofile_Log_Area1[D_MPROFILE_LOG_SIZE] __attribute__((aligned(32)));
static volatile unsigned char gMprofile_Log_Area2[D_MPROFILE_LOG_SIZE] __attribute__((aligned(32)));
#else
__align(32) static volatile unsigned char gMprofile_Log_Area0[D_MPROFILE_LOG_SIZE];
__align(32) static volatile unsigned char gMprofile_Log_Area1[D_MPROFILE_LOG_SIZE];
__align(32) static volatile unsigned char gMprofile_Log_Area2[D_MPROFILE_LOG_SIZE];
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_MPROFILE_ON
/**
 * mprofile_gic_init
 */
static VOID mprofile_gic_init(VOID)
{
	INT32	i;
	UINT32	Group_bit;
	UINT32	sr;
	
	sr = Dd_ARM_DI();
	Interrupt_ReSet_Enable(D_MPROFILE_TIMER32_CH4_INTID, D_DD_GIC_PRI01, D_MPROFILE_CPU0);
	Interrupt_ReSet_Enable(D_MPROFILE_TIMER32_CH11_INTID, D_DD_GIC_PRI01, D_MPROFILE_CPU1);
	Interrupt_ReSet_Enable(D_MPROFILE_TIMER32_CH12_INTID, D_DD_GIC_PRI01, D_MPROFILE_CPU2);

	for( i = 1; i < D_DD_GIC_INTID_MAX / 32; i++ ){
		Group_bit = 0xFFFFFFFF;
		if( (D_MPROFILE_TIMER32_CH4_INTID >= 32 * i) && (D_MPROFILE_TIMER32_CH4_INTID < 32 * (i + 1)) ){
			Group_bit = Group_bit & ~D_MPROFILE_TIMER32_CH4_ICDISR_BIT;
			Dd_GIC_DIST_Set_IGroupr( i, Group_bit );
		}
		if( (D_MPROFILE_TIMER32_CH11_INTID >= 32 * i) && (D_MPROFILE_TIMER32_CH11_INTID < 32 * (i + 1)) ){
			Group_bit = Group_bit & ~D_MPROFILE_TIMER32_CH11_ICDISR_BIT;
			Dd_GIC_DIST_Set_IGroupr( i, Group_bit );
		}
		if( (D_MPROFILE_TIMER32_CH12_INTID >= 32 * i) && (D_MPROFILE_TIMER32_CH12_INTID < 32 * (i + 1)) ){
			Group_bit = Group_bit & ~D_MPROFILE_TIMER32_CH12_ICDISR_BIT;
			Dd_GIC_DIST_Set_IGroupr( i, Group_bit );
		}
	}
	
	Dd_ARM_EI(sr);
}

/**
 * cmd_mp_init
 */
static VOID cmd_mp_init(VOID)
{
	mprofile_gic_init();
	Dd_TMR32_Open( D_MPROFILE_TIMER32_CH4, D_DDIM_USER_SEM_WAIT_POL );
	Dd_TMR32_Open( D_MPROFILE_TIMER32_CH11, D_DDIM_USER_SEM_WAIT_POL );
	Dd_TMR32_Open( D_MPROFILE_TIMER32_CH12, D_DDIM_USER_SEM_WAIT_POL );
	gProfile_usec = D_MPROFILE_PERIOD;
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH4, gProfile_usec, NULL );
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH11, gProfile_usec, NULL );
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH12, gProfile_usec, NULL );
	gProfile_Address0 = (UINT32)&gMprofile_Log_Area0;
	gProfile_Address1 = (UINT32)&gMprofile_Log_Area1;
	gProfile_Address2 = (UINT32)&gMprofile_Log_Area2;
	gProfile_Start_Address0 = (UINT32)&gMprofile_Log_Area0;
	gProfile_Start_Address1 = (UINT32)&gMprofile_Log_Area1;
	gProfile_Start_Address2 = (UINT32)&gMprofile_Log_Area2;
	gProfile_End_Address0 = (UINT32)&gMprofile_Log_Area0[D_MPROFILE_LOG_SIZE];
	gProfile_End_Address1 = (UINT32)&gMprofile_Log_Area1[D_MPROFILE_LOG_SIZE];
	gProfile_End_Address2 = (UINT32)&gMprofile_Log_Area2[D_MPROFILE_LOG_SIZE];
}

/**
 * cmd_mp_reset
 */
static VOID cmd_mp_reset(VOID)
{
	gProfile_usec = D_MPROFILE_PERIOD;
	gProfile_Address0 = (UINT32)&gMprofile_Log_Area0;
	gProfile_Address1 = (UINT32)&gMprofile_Log_Area1;
	gProfile_Address2 = (UINT32)&gMprofile_Log_Area2;
	gProfile_End_Address0 = (UINT32)&gMprofile_Log_Area0[D_MPROFILE_LOG_SIZE];
	gProfile_End_Address1 = (UINT32)&gMprofile_Log_Area1[D_MPROFILE_LOG_SIZE];
	gProfile_End_Address2 = (UINT32)&gMprofile_Log_Area2[D_MPROFILE_LOG_SIZE];
	memset( (VOID*)gProfile_Address0, 0x00, (gProfile_End_Address0 - gProfile_Address0) );
	memset( (VOID*)gProfile_Address1, 0x00, (gProfile_End_Address1 - gProfile_Address1) );
	memset( (VOID*)gProfile_Address2, 0x00, (gProfile_End_Address2 - gProfile_Address2) );
}

/**
 * cmd_mp_period
 */
static VOID cmd_mp_period(VOID)
{
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH4, gProfile_usec, NULL );
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH11, gProfile_usec, NULL );
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH12, gProfile_usec, NULL );
}

/**
 * cmd_mp_start
 */
static VOID cmd_mp_start(VOID)
{
	Dd_TMR32_StartReload( D_MPROFILE_TIMER32_CH4 );
	Dd_TMR32_StartReload( D_MPROFILE_TIMER32_CH11 );
	Dd_TMR32_StartReload( D_MPROFILE_TIMER32_CH12 );
}

/**
 * cmd_mp_stop
 */
static VOID cmd_mp_stop(VOID)
{
	Dd_TMR32_Stop( D_MPROFILE_TIMER32_CH4 );
	Dd_TMR32_Stop( D_MPROFILE_TIMER32_CH11 );
	Dd_TMR32_Stop( D_MPROFILE_TIMER32_CH12 );
}

/**
 * cmd_mp_dump
 */
static VOID cmd_mp_dump(const CHAR* file_name)
{
	CHAR	file_path0[16] = "I:\\";
	CHAR	file_path1[16] = "I:\\";
	CHAR	file_path2[16] = "I:\\";
	INT32	file_id0 = 0;
	INT32	file_id1 = 0;
	INT32	file_id2 = 0;
	ULONG	written_size;
	
	if( file_name == NULL ){
		printf( "cmd_mp_dump() File name error!\n" );
		return;
	}
	else{
		snprintf( (CHAR*)&file_path0[3], 13, "%s0.bin", (CHAR*)file_name );
		snprintf( (CHAR*)&file_path1[3], 13, "%s1.bin", (CHAR*)file_name );
		snprintf( (CHAR*)&file_path2[3], 13, "%s2.bin", (CHAR*)file_name );
	}
	
	BF_L1l2cache_Clean_Flush_All();
	
//	file_id0 = FS_create( file_path0, D_BF_FS_IF_ATTR_NORMAL );
	file_id0 = BF_Fs_If_Create( file_path0, D_BF_FS_IF_ATTR_NORMAL);
	
	if( file_id0 == 0 ){
		printf( "cmd_mp_dump() FSIF_Create %s failed! (%d)\n", file_path0, file_id0 );
		return;
	}
	else{
//		FS_write( file_id0, (VOID*)gProfile_Start_Address0, (gProfile_Address0 - gProfile_Start_Address0), &written_size );
//		FS_close( file_id0 );
		BF_Fs_If_Write( file_id0, (VOID*)gProfile_Start_Address0, (gProfile_Address0 - gProfile_Start_Address0), &written_size );
		BF_Fs_If_Close( file_id0 );
	}
	
	file_id1 = BF_Fs_If_Create( file_path1, D_BF_FS_IF_ATTR_NORMAL );
	if( file_id1 == 0 ){
		printf( "cmd_mp_dump() FSIF_Create %s failed! (%d)\n", file_path1, file_id1 );
		return;
	}
	else{
		BF_Fs_If_Write( file_id1, (VOID*)gProfile_Start_Address1, (gProfile_Address1 - gProfile_Start_Address1), &written_size );
		BF_Fs_If_Close( file_id1 );
	}
	
	file_id2 = BF_Fs_If_Create( file_path2, D_BF_FS_IF_ATTR_NORMAL );
	if( file_id2 == 0 ){
		printf( "cmd_mp_dump() FSIF_Create %s failed! (%d)\n", file_path2, file_id2 );
		return;
	}
	else{
		BF_Fs_If_Write( file_id2, (VOID*)gProfile_Start_Address2, (gProfile_Address2 - gProfile_Start_Address2), &written_size );
		BF_Fs_If_Close( file_id2 );
	}
}

/**
 * cmd_mp_term
 */
static VOID cmd_mp_term(VOID)
{
	UINT32*	Profile_Pointer0 = (UINT32*)&gMprofile_Log_Area0;
	UINT32*	Profile_Pointer1 = (UINT32*)&gMprofile_Log_Area1;
	UINT32*	Profile_Pointer2 = (UINT32*)&gMprofile_Log_Area2;
	
	printf( "mprofile dump -->\n" );
	printf( "cpu0 -->\n" );
	while( (UINT32)Profile_Pointer0 < gProfile_Address0 ){
		printf( "%08x\n", *Profile_Pointer0 );
		Profile_Pointer0++;
	}
	printf( "cpu1 -->\n" );
	while( (UINT32)Profile_Pointer1 < gProfile_Address1 ){
		printf( "%08x\n", *Profile_Pointer1 );
		Profile_Pointer1++;
	}
	printf( "cpu2 -->\n" );
	while( (UINT32)Profile_Pointer2 < gProfile_Address2 ){
		printf( "%08x\n", *Profile_Pointer2 );
		Profile_Pointer2++;
	}
	printf( "<-- mprofile dump\n" );
}

/**
 * cmd_mp_close
 */
static VOID cmd_mp_close(VOID)
{
	INT32 i;
	UINT32	sr;
	
	sr = Dd_ARM_DI();
	Dd_TMR32_Stop( D_MPROFILE_TIMER32_CH4 );
	Dd_TMR32_Close( D_MPROFILE_TIMER32_CH4 );
	Dd_TMR32_Stop( D_MPROFILE_TIMER32_CH11 );
	Dd_TMR32_Close( D_MPROFILE_TIMER32_CH11 );
	Dd_TMR32_Stop( D_MPROFILE_TIMER32_CH12 );
	Dd_TMR32_Close( D_MPROFILE_TIMER32_CH12 );
	
	for( i = 1; i < D_DD_GIC_INTID_MAX / 32; i++ ){
		Dd_GIC_DIST_Set_IGroupr( i, 0xFFFFFFFF );
	}
	
	Dd_ARM_EI(sr);
}
#endif	// CO_MPROFILE_ON

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
#ifdef CO_MPROFILE_ON
/**
 * cmd_mp_main
 * @param i:int argc
 * @param i:char** argv
 */
VOID cmd_mp_main(int argc, char** argv)
{
	UINT32 record;
	
	if( strcmp( argv[1], "--init" ) == 0 ){
		printf( "START mprofile init\n" );
		cmd_mp_init();
		printf( "END mprofile init\n" );
	}
	else if( strcmp( argv[1], "--reset" ) == 0 ){
		printf( "START mprofile reset\n" );
		cmd_mp_reset();
		printf( "END mprofile reset\n" );
	}
	else if( strcmp( argv[1], "--period" ) == 0 ){
		if( argc != 3 ){
			printf( "needs parameter\n" );
			return;
		}
		printf( "START mprofile period\n" );
		gProfile_usec = atoi(argv[2]);
		cmd_mp_period();
		printf( "END mprofile period\n" );
	}
	else if( strcmp( argv[1], "--start" ) == 0 ){
		printf( "START mprofile start\n" );
		cmd_mp_start();
		printf( "END mprofile start\n" );
	}
	else if( strcmp( argv[1], "--stop" ) == 0 ){
		printf( "START mprofile stop\n" );
		cmd_mp_stop();
		record = (gProfile_Address0 - gProfile_Start_Address0) / 4;
		printf( "%d records stored cpu0\n", record);
		printf( "record time = %d[ms]\n", gProfile_usec * record / 1000);
		record = (gProfile_Address1 - gProfile_Start_Address1) / 4;
		printf( "%d records stored cpu1\n", record);
		printf( "record time = %d[ms]\n", gProfile_usec * record / 1000);
		record = (gProfile_Address2 - gProfile_Start_Address2) / 4;
		printf( "%d records stored cpu2\n", record);
		printf( "record time = %d[ms]\n", gProfile_usec * record / 1000);
		printf( "END mprofile stop\n" );
	}
	else if( strcmp( argv[1], "--dump" ) == 0 ){
		if( argc != 3 ){
			printf( "needs parameter\n" );
			return;
		}
		printf( "START mprofile dump\n" );
		cmd_mp_dump(argv[2]);
		printf( "END mprofile dump\n" );
	}
	else if( strcmp( argv[1], "--term" ) == 0 ){
		printf( "START mprofile terminal out\n" );
		cmd_mp_term();
		printf( "END mprofile terminal out\n" );
	}
	else if( strcmp( argv[1], "--close" ) == 0 ){
		printf( "START mprofile close\n" );
		cmd_mp_close();
		printf( "END mprofile close\n" );
	}
	else{
		printf( "input parameter abnoraml\n" );
		printf( " mprofile --init\n" );
		printf( " mprofile --start\n" );
		printf( " mprofile --stop\n" );
		printf( " mprofile --dump [filename]\n" );
		printf( " mprofile --reset\n" );
		printf( " mprofile --period [time[us]]\n" );
		printf( " mprofile --term\n" );
		printf( " mprofile --close\n" );
	}
}

VOID Mprofile_Init(VOID)
{
	gProfile_Address0 = (UINT32)&gMprofile_Log_Area0;
	gProfile_Address1 = (UINT32)&gMprofile_Log_Area1;
	gProfile_Address2 = (UINT32)&gMprofile_Log_Area2;
	gProfile_Start_Address0 = (UINT32)&gMprofile_Log_Area0;
	gProfile_Start_Address1 = (UINT32)&gMprofile_Log_Area1;
	gProfile_Start_Address2 = (UINT32)&gMprofile_Log_Area2;
	gProfile_End_Address0 = (UINT32)&gMprofile_Log_Area0[D_MPROFILE_LOG_SIZE];
	gProfile_End_Address1 = (UINT32)&gMprofile_Log_Area1[D_MPROFILE_LOG_SIZE];
	gProfile_End_Address2 = (UINT32)&gMprofile_Log_Area2[D_MPROFILE_LOG_SIZE];
}

VOID Mprofile_Reset(VOID)
{
	cmd_mp_reset();
}

VOID Mprofile_Start(VOID)
{
	cmd_mp_start();
}

VOID Mprofile_Stop(VOID)
{
	cmd_mp_stop();
}

VOID Mprofile_Open(VOID)
{
	mprofile_gic_init();
	Dd_TMR32_Open( D_MPROFILE_TIMER32_CH4, D_DDIM_USER_SEM_WAIT_POL );
	Dd_TMR32_Open( D_MPROFILE_TIMER32_CH11, D_DDIM_USER_SEM_WAIT_POL );
	Dd_TMR32_Open( D_MPROFILE_TIMER32_CH12, D_DDIM_USER_SEM_WAIT_POL );
	gProfile_usec = D_MPROFILE_PERIOD;
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH4, gProfile_usec, NULL );
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH11, gProfile_usec, NULL );
	Dd_TMR32_SetTimer( D_MPROFILE_TIMER32_CH12, gProfile_usec, NULL );
}

VOID Mprofile_Close(VOID)
{
	cmd_mp_close();
}

VOID Mprofile_Dump(const CHAR* file_name)
{
	cmd_mp_dump(file_name);
}
#endif	// CO_MPROFILE_ON
