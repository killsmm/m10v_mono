/**
 * @file		command.c
 * @brief		Observe Terminal Command
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "dd_uart.h"
#include "debug.h"
#include "command.h"
#include "command_entry.h"
#include "os_user_custom.h"
#include "category_parameter.h"
#include "measure_time.h"

extern INT32 io_getchar(VOID);
extern INT32 io_gets(char* str);

typedef enum {
	E_COMMAND_MODE_NORAML = 0,
	E_COMMAND_MODE_CT = 1,
	E_COMMAND_MODE_ST = 2,
	E_COMMAND_MODE_HCE = 3,
	E_COMMAND_MODE_IP = 4,
	E_COMMAND_MODE_MP = 5,
#ifdef CO_DEBUG_ON_PC
	E_COMMAND_MODE_EXIT = 6,
#endif
	E_COMMAND_MODE_CATE = 8,
}E_COMMAND_MODE;

static E_COMMAND_MODE gCommand_Mode = E_COMMAND_MODE_NORAML;

/**
 * allocate argv memory
 * @param io:char** argv
 */
static VOID argv_allocate(char** argv)
{
	static char argv_tbl[NUM_OF_ARG][1];
	INT32 arg_cnt;

	for( arg_cnt = 0; arg_cnt < NUM_OF_ARG; arg_cnt++ ) {
		argv_tbl[arg_cnt][0] = '\0';
		argv[arg_cnt] = &argv_tbl[arg_cnt][0];
	}
}

static VOID  wait_command_mode(VOID)
{
	char str;

	if (gCommand_Mode != E_COMMAND_MODE_NORAML) {
		return ;
	}

	while(1){
		str = io_getchar();
		if( str ==  'c') {		// Component Test
			gCommand_Mode = E_COMMAND_MODE_CT;
			printf("\n<<CT command mode start>>\n");
			break;
		}
		else if( str ==  's') {	// System Mode Test
			gCommand_Mode = E_COMMAND_MODE_ST;
			printf("\n<<ST command mode start>>\n");
			break;
		}
		else if( str ==  'h') {
			gCommand_Mode = E_COMMAND_MODE_HCE;
			printf("\n<<HCE command mode start>>\n");
			break;
		}
		else if( str == 'i') {
			gCommand_Mode = E_COMMAND_MODE_IP;
			printf("\n<<IP command mode start>>\n");
			break;
		}
		else if( str == 'm') {
			gCommand_Mode = E_COMMAND_MODE_MP;
			printf("\n<<MP command mode start>>\n");
			break;
		}
#ifdef CO_DEBUG_ON_PC
		else if( str == 'e') {
			gCommand_Mode = E_COMMAND_MODE_EXIT;
			printf("\n<<PCSim end>>\n");

			break;
		}
#endif
		else if( str ==  'd') {
			gCommand_Mode = E_COMMAND_MODE_CATE;
			printf("\n<<cate para mode start>>\n");
			break;
		}
		else {
			;
		}
		if( (str == '\n') || (str == '\r') ) {
			OS_User_Dly_Tsk(500);
		}
		else {
			OS_User_Dly_Tsk(10);
		}
	}
}

static INT32 is_command_mode_end(char* cmd_string)
{
	if (strncmp(cmd_string, "exit", sizeof("exit")) != 0) {
		return FALSE;
	}

	gCommand_Mode = E_COMMAND_MODE_NORAML;
	printf("<<command mode end>>\n");
	return TRUE;
}

extern const  T_CMD_ENTRY gCommand_Entry_Table[] ;

/**
 * check command help
 */
#define IS_HELP(argv_1) (((strcmp(argv_1, "help") == 0)||(strcmp(argv_1, "h") == 0)) ? TRUE : FALSE)



/**
 * convert command to function
 * @param i:const char* cmd
 * @return T_CMD_FUNC
 */
static T_CMD_FUNC command_2_func( const T_CMD_ENTRY cmd_tbl[], const char* cmd)
{
	UCHAR entry_cnt=0;
	
	while (cmd_tbl[entry_cnt].cmd_string != NULL){
		if (strcmp(cmd_tbl[entry_cnt].cmd_string, cmd) != 0){
			++entry_cnt;
			continue;
		}
		break;
	}
	return cmd_tbl[entry_cnt].func;
}

/**
 * display all command help
 */
VOID command_help_all(const T_CMD_ENTRY cmd_tbl[])
{
	UCHAR entry_cnt=2;
	
	printf("[43;30m");
	while (cmd_tbl[entry_cnt].func.cmd_entry != NULL){
		if(cmd_tbl[entry_cnt].func.help_entry != NULL){
			cmd_tbl[entry_cnt].func.help_entry();
		}
		++entry_cnt;
	}
	
	printf("[0m\n");
}


/**
 * command procedure
 * @param INT32 argc
 * @param char** argv
 * @attention this is external API, call from Command_Observer()
 */
static VOID command_procedure(const T_CMD_ENTRY cmd_tbl[], INT32 argc, char** argv){
	T_CMD_FUNC cmd_func;
	
	if (IS_HELP(argv[0])) {
		command_help_all( cmd_tbl);
		return;
	}
	
	cmd_func = command_2_func( cmd_tbl, argv[0]);
	
	if (IS_HELP(argv[1])){
		if (cmd_func.help_entry == NULL){
			BF_Debug_Print_Error(("command error\n"));
			return;
		}
		// call command help function
		cmd_func.help_entry();
		return;
	}
	
	if (cmd_func.cmd_entry == NULL){
		BF_Debug_Print_Error(("command error\n"));
		return;
	}
	// call command function
	cmd_func.cmd_entry(argc, argv);
}

/**
 * command observe task
 * @param i:INT32 stacd
 */
VOID Command_Observer(INT32 stacd)
{
	char cmd_string[MAX_OF_CMD_LINE];
	INT32 cmd_str_cnt;
	char* argv[NUM_OF_ARG];
	INT32 argc = 0;
	
	BF_Debug_Print_Information(("Command_Observer start\n"));
	
	for (;;){
		wait_command_mode();
#ifdef CO_DEBUG_ON_PC
		if( gCommand_Mode == E_COMMAND_MODE_EXIT ) {
			break;
		}
#endif

		printf(": You can quit this mode when input \'exit\'.\n");
		
		// debug mode is command.
		cmd_str_cnt = 0;
		memset(cmd_string, 0, MAX_OF_CMD_LINE);
		
		// get terminal inout
		while (io_gets(cmd_string) <= 0) BF_Debug_Print_Warning(("no input command\n"));
		
//		BF_Debug_Print_Information(("detect command: %s\n", cmd_string));
		
		// if 'exit' input from terminal, finish command mode.
		if (is_command_mode_end(cmd_string)) continue;

		argv_allocate(argv);
		while (cmd_string[cmd_str_cnt] != '\0'){
			// is space?
			if (cmd_string[cmd_str_cnt] == ' '){
				cmd_string[cmd_str_cnt] = '\0';
				++cmd_str_cnt;
				continue;
			}

			argv[argc] = &cmd_string[cmd_str_cnt];
			++argc;
			if(argc >= NUM_OF_ARG) break;

			while(cmd_string[cmd_str_cnt] != ' ') {
				if (cmd_string[cmd_str_cnt] == '\0') break;
				++cmd_str_cnt;
			}
		}
		BF_Measure_Time_Start_usec(0);
		// call external function
		if( gCommand_Mode == E_COMMAND_MODE_CT){ 
			// delete_basefw\fj\command
			//command_procedure( gCmd_CT_Entry_Table, argc, argv);
		}
		else if( gCommand_Mode == E_COMMAND_MODE_ST){
			
		}
		else if( gCommand_Mode == E_COMMAND_MODE_IP){
			command_procedure( gCmd_IP_Entry_Table, argc, argv);
		}
		else if( gCommand_Mode == E_COMMAND_MODE_MP){
			command_procedure( gCmd_MP_Entry_Table, argc, argv);
		}
		else if( gCommand_Mode == E_COMMAND_MODE_CATE ){
			Cate_Decode_String(argc, argv);
			
		}
		printf("execution time = %ld\n", BF_Measure_Time_Get_Long_usec(0));
		argc = 0;
		OS_User_Dly_Tsk(100);
	}
}

