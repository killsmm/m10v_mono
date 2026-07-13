/**
 * @file		mba_log.c
 * @brief		MBA Log Control.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; FUJITSU SEMICONDUCTOR LIMITED 2014</I></B>
 */

//#pragma arm section rwdata="MBA_INIT"
//#pragma arm section rodata="MBA_CONST"
//#pragma arm section code="MBA_CODE"
//#pragma arm section rwdata="MBA_DATA"


// ****************************************************************************
//                          Compile Options for MBA Timer (usec order)
// ****************************************************************************
#define CO_MBA_TIME_UNIT_1US

// ****************************************************************************
//                          Type Define
// ****************************************************************************
typedef int			BOOL;
typedef int			MBA_USER_ID;
typedef int			MBA_USER_ER;

// ****************************************************************************
//                          Include
// ****************************************************************************
#include "mba_log.h"
#include "mba_if.h"

// ****************************************************************************
//                          Define
// ****************************************************************************
#define MBA_IF_FSIF_OK			(0)		// OK

#ifndef TRUE
#define TRUE					(1)		// True
#endif

#ifndef FALSE
#define FALSE					(0)		// False
#endif

#ifndef NULL
#define NULL					(0)		// None
#endif

// ***************************************************************************
//                          Definition Macro
// ***************************************************************************
// Nothing Special

// ****************************************************************************
//                          Enumeration
// ****************************************************************************
typedef enum{
	E_NORMAL = 0,				// Normal status
	E_RING_FULL,				// Ring full (No Log overwrite)
	E_RING_FULL_OVERWRITE		// Ring full (Log Overwrite)
} E_OUTPUT_RING_STATUS;

// ***************************************************************************
//                          Structure
// ***************************************************************************
// MBA log header structure
typedef struct{
	volatile unsigned int	start;										// Log start pointer
	volatile unsigned int	end;										// Log end pointer
} T_MBA_LOG_RW_POINTER;

// MBA Log ID Name List Structure
typedef struct 
{
	char func_id_name[MBALOG_FUNC_ID_MAX][MBALOG_FUNC_ID_LENGTH];			//Function ID Name
} T_MBALOG_ID_LIST;

// ***************************************************************************
//                           Global Data
// ***************************************************************************
// Log header variable
static T_MBA_LOG_RW_POINTER		gMBA_LogRwPointer[D_MBALOG_AREA_MAX];
static T_MBA_LOG_RW_POINTER*	gMBA_p_lrwp = &gMBA_LogRwPointer[0];

// Log record enable
static unsigned char		gMBA_enable;

// Log type array
static const char*			gMBA_TypeStrTbl[] = {"point", "start", "end"};

// MBA Log ID Name List Data
static T_MBALOG_ID_LIST*	gId_List;

// MBA Log Output Ring Buffer Full Flag
static E_OUTPUT_RING_STATUS	gMBALog_Output_Ring_Full_Flag = E_NORMAL;

// MBA of time buffer
static unsigned long		gMBA_Timer_buf = 0;

// Kind of MBA log
extern unsigned int 		gMBA_Kind;

// MBA log output buffer current point
static unsigned short		gMBA_out_buf_pos;

static unsigned char		gMBA_surface_no = 0;

//*****************************************************************************
//           Function Prototype Definition
//*****************************************************************************
//extern signed int MBA_IF_FSIF_Create( const char* const fname );
//extern signed int MBA_IF_FSIF_Close( signed int dNo );
//extern signed int MBA_IF_FSIF_Write( signed int fNo, const void* addr, unsigned long count, unsigned long* length );
//extern signed int MBA_IF_FSIF_Remove( const char* fname );
//extern BOOL MBA_IF_User_Sns_Ctx( void );
//extern MBA_USER_ER MBA_IF_User_Get_Tid( MBA_USER_ID* p_tskid );
//extern void MBA_IF_User_EI( const unsigned int lock_key );
//extern unsigned int MBA_IF_User_DI( void );
//extern unsigned long MBA_IF_Get_Time_us( void );

extern unsigned long FJ_Get_OK_Version( void );

static void Mbalog_make_print_time( char* p_dst, unsigned long time );

//extern const T_MBALOG_ID_LIST gMBALog_Id_List;

// ***************************************************************************
//                    Local Method definition
// ***************************************************************************
static signed int Mbalog_open_file( const char* const file_path )
{
	signed int file_id;

	file_id = MBA_IF_FSIF_Create( file_path );

	gMBA_out_buf_pos = 0;

	return file_id;
}

static void Mbalog_flush_file( signed int file_id )
{
	unsigned long	written_size;

	if( file_id == 0 ) {
		return;
	}

	if( gMBA_out_buf_pos != 0 ) {
		MBA_IF_FSIF_Write( file_id, (char*)gMBA_Outbuf, gMBA_out_buf_pos, &written_size );
		gMBA_out_buf_pos = 0;
	}
}

static void Mbalog_close_file( signed int file_id )
{
	if( file_id == 0 ) {
		return;
	}

	Mbalog_flush_file( file_id );

	MBA_IF_FSIF_Close( file_id );
}

static void Mbalog_write_buf_file( signed int file_id, char* buf, unsigned int buf_bytes, unsigned long* written_size )
{
	if( file_id == 0 ) {
		return;
	}

	memcpy( &gMBA_Outbuf[gMBA_out_buf_pos], buf, buf_bytes );
	gMBA_out_buf_pos += buf_bytes;

	if( gMBA_out_buf_pos >= gMBA_Outbuf_Size_Main ) {
		MBA_IF_FSIF_Write( file_id, (char*)gMBA_Outbuf, gMBA_Outbuf_Size_Main, written_size );

		gMBA_out_buf_pos -= gMBA_Outbuf_Size_Main;
		memcpy( &gMBA_Outbuf[0], &gMBA_Outbuf[gMBA_Outbuf_Size_Main], gMBA_out_buf_pos );
	}
}

/**
 * Start measure timer
 */
static void Mbalog_timer_start( void )
{
#ifdef CO_MBA_TIME_UNIT_1US
	gMBA_Timer_buf = MBA_IF_Get_Time_us();
#else
	gMBA_Timer_buf = MBA_IF_Get_Time_us() / 1000;
#endif
}

/**
 * Get current timer data
 */
static void Mbalog_get_currrent_timer( unsigned long* time )
{
#ifdef CO_MBA_TIME_UNIT_1US
	*time = MBA_IF_Get_Time_us() - gMBA_Timer_buf;
#else
	*time = (MBA_IF_Get_Time_us() / 1000) - gMBA_Timer_buf;
#endif
}

/**
 * Update log pointer
 */
static void Mbalog_update(void)
{
	gMBA_p_lrwp->end += 1;														// Update log end
	if(gMBA_p_lrwp->end >= gMBA_Log_Max){
		gMBA_p_lrwp->end = 0;
	}
	*gMBA_Write_Pointer = gMBA_p_lrwp->end;

	if(gMBA_Overwrite_Flag == TRUE) {
		// MBA log lotation
		if( gMBA_p_lrwp->start == gMBA_p_lrwp->end ) {							// Ring full?
			if(gMBALog_Output_Ring_Full_Flag != E_RING_FULL_OVERWRITE) {
//				printf("Ring buffer is full. After that, log will overwrite the beginning of the buffer.\n");
			}
			gMBA_p_lrwp->start += 1;											// Update log start (oldest log is destroy)
			if(gMBA_p_lrwp->start >= gMBA_Log_Max){
				gMBA_p_lrwp->start = 0;
			}
			gMBALog_Output_Ring_Full_Flag = E_RING_FULL_OVERWRITE;
			*gMBA_Overlap_Flg = 1;
		}
		else {
			// nothing to do
		}
	}
	else {
		// Not log overwrite
		if( gMBA_p_lrwp->start == gMBA_p_lrwp->end ) {							// Ring full?
			// MBA log record stop
			// ring full
			gMBA_enable = FALSE;
			gMBA_p_lrwp->end = gMBA_Log_Max;
			*gMBA_Write_Pointer = gMBA_p_lrwp->end;

			if(gMBALog_Output_Ring_Full_Flag != E_RING_FULL) {
				gMBALog_Output_Ring_Full_Flag = E_RING_FULL;
//				printf("Ring buffer is full. After that, Stop the logging.\n");
			}
		}
		else {
			// not full
		}
	}
}

/**
 * Record log to memory
 */
static void Mbalog_record( unsigned int kind, unsigned char task_id, unsigned char func_id, unsigned char type, const char* fmt, va_list arg )
{
	T_MBA_LOG_DATA *p_cell;
	signed int status_reg;

	if( ( kind & gMBA_Kind ) == 0 ){
		return;
	}

	status_reg = MBA_IF_User_DI();

	p_cell = &gMBA_Log_Data[gMBA_surface_no][gMBA_p_lrwp->end];
	Mbalog_get_currrent_timer( &p_cell->time );
	Mbalog_update();

	MBA_IF_User_EI( status_reg );

	if( task_id >= MBALOG_TASK_ID_MAX ){
		p_cell->task_id = 0;
	}
	else{
		p_cell->task_id = task_id;
	}
	p_cell->func_id = func_id;
	p_cell->type = type;
	
	p_cell->fmt = fmt;
	p_cell->arg[0] = va_arg( arg, unsigned int );
	p_cell->arg[1] = va_arg( arg, unsigned int );
	p_cell->arg[2] = va_arg( arg, unsigned int );
	p_cell->arg[3] = va_arg( arg, unsigned int );
}

/**
 * Print log header
 */
static void Mbalog_print_header( unsigned char type, signed int file_id )
{
	unsigned long	written_size;
	char	buf[128];
	char	id_name_buf[D_MBA_TASK_NAME_MAX];
	int		index;
	const char*		id_name;

	// Version information & Overflow information output
	if(type == 0){	// to terminal
		printf( "// OK Version \"%08d\"\n", (int)(FJ_Get_OK_Version()) );
		printf( "// Log Status \"%d\"\n", gMBALog_Output_Ring_Full_Flag );
	}
	else{			// to file
		snprintf( (char*)buf, sizeof(buf), "// OK Version \"%08d\"\n", (int)(FJ_Get_OK_Version()) );
		Mbalog_write_buf_file( file_id, buf, strlen((char*)buf), &written_size);

		snprintf( (char*)buf, sizeof(buf), "// Log Status \"%d\"\n", gMBALog_Output_Ring_Full_Flag );
		Mbalog_write_buf_file( file_id, buf, strlen((char*)buf), &written_size);
	}

	// Headr infomation output
	for( index = 0; index < MBALOG_TASK_ID_MAX; index++ ){
		if( index < MBALOG_TASK_ID_LINE_MAX ){
			snprintf( id_name_buf, sizeof(id_name_buf), "%02d", index );
			id_name = id_name_buf;
		}else{
			MBA_IF_User_Get_TaskNameFromMbaTid( index-MBALOG_TASK_ID_LINE_MAX, id_name_buf );
			id_name = id_name_buf;
		}
		if(type == 0){	// to terminal
			printf( "linename%02d=\"%s\"\n", index, id_name );
		}
		else{			// to file
			snprintf( (char*)buf, sizeof(buf), "linename%02d=\"%s\"\n", index, id_name );
			Mbalog_write_buf_file( file_id, buf, strlen((char*)buf), &written_size);
		}
	}

	for( index = 0; index < MBALOG_FUNC_ID_MAX; index++ ){
		id_name = gId_List->func_id_name[index];
		if( id_name == NULL ) {
			snprintf( id_name_buf, sizeof(buf), "Func%u", index );
			id_name = id_name_buf;
		}
		if(type == 0){	// to terminal
			printf( "identname%02d=\"%s\"\n", index, id_name );
		}
		else{			// to file
			snprintf( (char*)buf, sizeof(buf), "identname%02d=\"%s\"\n", index, id_name );
			Mbalog_write_buf_file( file_id, buf, strlen((char*)buf), &written_size);
		}
	}
}

/**
 * Make print time log
 */
static void Mbalog_make_print_time( char* p_dst, unsigned long time )
{
#ifdef CO_MBA_TIME_UNIT_1US
	sprintf( (char*)p_dst, "%lu.%06lu", time / 1000000, time % 1000000 );
#else
	sprintf( (char*)p_dst, "%lu.%03lu", time / 1000, time%1000 );
#endif
}

/**
 * Print 1 cell log
 */
static void Mbalog_print_cell( unsigned char type, unsigned int surface_no, signed int file_id, unsigned int index )
{
	T_MBA_LOG_DATA	*p_cell;
	char	time_buf[16];
	char	out_buf[64];
	char	work_buf[64];
	unsigned long	written_size;

	p_cell = &gMBA_Log_Data[surface_no][index];
	Mbalog_make_print_time( time_buf, p_cell->time );
	snprintf( (char*)out_buf, sizeof(out_buf), "%s:%d,%d,%s,\"",
				time_buf, (int)p_cell->task_id, p_cell->func_id, gMBA_TypeStrTbl[p_cell->type] );
	sprintf( work_buf, p_cell->fmt, p_cell->arg[0], p_cell->arg[1], p_cell->arg[2], p_cell->arg[3] );
	strncat( (char*)out_buf, (const char*)work_buf, sizeof(out_buf)-strlen(out_buf)-1 );
	strncat( (char*)out_buf, "\"\n", sizeof(out_buf)-strlen(out_buf)-1 );

	if(type == 0){	// to terminal
		printf( (char*)out_buf );
	}
	else if(type == 1){	// to file
		Mbalog_write_buf_file( file_id, out_buf, strlen((char*)out_buf), &written_size);
	}
}

/**
 * Output Memory Log
 */
static void Mbalog_output( unsigned char type, unsigned int surface_no, const char* file_name, unsigned char all_out_flg )
{
	signed int		file_id = 0;
	unsigned int	index;
	char			stop_flg = 0;
	unsigned long	written_size;
	unsigned long	write_size;
	unsigned long	table_size;
	const char		lavel[] = "//////////////////////////////\n";
	unsigned int	time_unit;
	unsigned int	count;
	unsigned int	count_max;
	T_MBA_LOG_RW_POINTER*	pmba_lrwp;

	gId_List = (void*)&gMBALog_Id_List;

	// Log file open
	if( (type == 1) || (type == 2) ) {	// to file
		// Parameter check
		if( (file_name == NULL) || (file_name[0] == '\0') ) {
			printf("MBA_Output() File name error!\n");
			return;
		}

		file_id = Mbalog_open_file( file_name );
		if( file_id == 0 ){  
			printf("MBA_Output() FSIF_Create %s failed! (%d)\n", file_name, file_id);
			return;
		}
	}

	if( gMBA_enable == 1 ){
		gMBA_enable = 0;
		stop_flg = 1;
	}

	// Headr infomation output
	Mbalog_print_header( type, file_id );

	if( all_out_flg ){
		count = 0;
		count_max = gMBA_Surface_Max;
	}
	else{
		count = surface_no;
		count_max = surface_no + 1;
	}

	// Log data output
	if( type == 2 ) {	// to binary file
		Mbalog_flush_file( file_id );

		// Data start label output
		MBA_IF_FSIF_Write( file_id, lavel, sizeof(lavel), &written_size );

		// Time unit output
#ifdef CO_MBA_TIME_UNIT_1US
		time_unit = 1000000;
#else
		time_unit = 1000;
#endif
		MBA_IF_FSIF_Write( file_id, &time_unit, 4, &written_size);

		// Data output
		table_size = sizeof( T_MBA_LOG_DATA );
		for( ; count < count_max; count++ ){
			pmba_lrwp = &gMBA_LogRwPointer[count];
			if( pmba_lrwp->start > pmba_lrwp->end ){
				write_size = (gMBA_Log_Max - (pmba_lrwp->start - 1)) * table_size;
				MBA_IF_FSIF_Write( file_id, &gMBA_Log_Data[count][pmba_lrwp->start - 1], write_size, &written_size );
			}
			write_size = pmba_lrwp->end * table_size;
			MBA_IF_FSIF_Write( file_id, gMBA_Log_Data[count], write_size, &written_size );
		}
	}
	else{
		for( ; count < count_max; count++ ){
			pmba_lrwp = &gMBA_LogRwPointer[count];
			if( pmba_lrwp->start > pmba_lrwp->end ){
				for( index = pmba_lrwp->start - 1; index < gMBA_Log_Max; index++ ){
					Mbalog_print_cell( type, count, file_id, index );
				}
			}
			for( index = 0; index < pmba_lrwp->end; index++ ){
				Mbalog_print_cell( type, count, file_id, index );
			}
		}
	}

	if( (type == 1) || (type == 2) ){	// to file
		Mbalog_close_file( file_id );
	}

	if( (stop_flg == 1) && ((gMBA_Overwrite_Flag == TRUE ) || (gMBALog_Output_Ring_Full_Flag == E_NORMAL)) ) {
		gMBA_enable = 1;
	}
}

// ***************************************************************************
//                    Global Method definition
// ***************************************************************************
/**
 * Start Memory Log
 */
void MBA_Log_Start( void* id_list )
{
	gMBA_p_lrwp = &gMBA_LogRwPointer[0];
	memset((void*)gMBA_p_lrwp, 0x00, sizeof(T_MBA_LOG_RW_POINTER) * D_MBALOG_AREA_MAX);
	gMBA_enable				= 1;
	gMBA_surface_no			= 0;
	*gMBA_Write_Pointer		= 0;
	*gMBA_Overlap_Flg		= 0;
	
	gId_List = id_list;

	gMBALog_Output_Ring_Full_Flag = E_NORMAL;

	Mbalog_timer_start();
}

/**
 * End Memory Log
 */
void MBA_Log_End( void )
{
	gMBA_enable	= 0;
}

/**
 * Print Memory Log
 */
void MBA_Log_Printf( unsigned int kind, unsigned char func_id, unsigned char type, const char* fmt, va_list arg )
{
	OS_USER_ID tskid;
	MBA_USER_ID mba_tskid;
	BOOL state;

	if( gMBA_enable == 1 ){
		state = MBA_IF_User_Sns_Ctx();
		if( state == FALSE ) {
			MBA_IF_User_Get_Tid( &tskid );
			mba_tskid = OS_User_Get_MbaTidFromTid( tskid ); // takes time to find id...
		}
		else {
			tskid = 0;
			mba_tskid = 0;
		}
		Mbalog_record( kind, (unsigned char)mba_tskid+MBALOG_TASK_ID_LINE_MAX, func_id, type, fmt, arg );
	}
}

extern void MBA_Log_Printf_LineID( const unsigned int kind, const unsigned char line_id, const unsigned char func_id, const unsigned char type, const char* const fmt, va_list arg )
{
	if( gMBA_enable == 1 ){
		Mbalog_record( kind, (unsigned char)line_id, func_id, type, fmt, arg );
	}
}

/**
 * Output Memory Log(current area)
 */
void MBA_Log_Output( unsigned char type, const char* file_name )
{
	Mbalog_output( type, gMBA_surface_no, file_name, 0 );
}

/**
 * Output Memory Log(select area)
 */
void MBA_Log_Output_Select( unsigned char type, unsigned int surface_no, const char* file_name )
{
	Mbalog_output( type, surface_no, file_name, 0 );
}

/**
 * Output Memory Log(all area)
 */
void MBA_Log_Output_All( unsigned char type, const char* file_name )
{
	Mbalog_output( type, 0, file_name, 1 );
}

/**
 * Clear Memory Log
 */
void MBA_Log_Clear( void )
{
	gMBA_p_lrwp->start	= 0;
	gMBA_p_lrwp->end	= 0;
	*gMBA_Write_Pointer	= 0;
	*gMBA_Overlap_Flg	= 0;

	gMBALog_Output_Ring_Full_Flag = E_NORMAL;
}

/**
 * Remove Log File
 */
void MBA_Log_Remove( const char* const file_name )
{
	signed int	ret;

	if( (file_name == NULL) || (file_name[0] == '\0' ) ){
		printf("MBA_Remove() File name error!\n");
		return;
	}

	ret = MBA_IF_FSIF_Remove( file_name );
	if( ret != MBA_IF_FSIF_OK ){
		printf("MBA_Remove() File remove error! (%d)\n", ret);
	}
}

/**
 * Update Surface No.
 */
void MBA_Log_Update_Surface_No( void )
{
	if( ((gMBA_surface_no + 1) % gMBA_Surface_Max) == 0 ){
		gMBA_Write_Pointer -= (gMBA_Surface_Max - 1);
		gMBA_Overlap_Flg   -= (gMBA_Surface_Max - 1);
	}
	else{
		gMBA_Write_Pointer++;
		gMBA_Overlap_Flg++;
	}

	gMBA_surface_no = (gMBA_surface_no + 1) % gMBA_Surface_Max;
	gMBA_p_lrwp = &gMBA_LogRwPointer[gMBA_surface_no];
	if( gMBA_Surface_Max != 1 ){
		gMBA_p_lrwp->start	= 0;
		gMBA_p_lrwp->end	= 0;
		*gMBA_Write_Pointer	= 0;
		*gMBA_Overlap_Flg	= 0;
	}
}
