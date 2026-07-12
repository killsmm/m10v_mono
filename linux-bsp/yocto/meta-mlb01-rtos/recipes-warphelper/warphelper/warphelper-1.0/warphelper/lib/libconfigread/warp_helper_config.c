/*
 * Warp helper config parser library
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "warp_helper_config.h"
char warp_ipc_key[ BUFFER_SIZE];
char warp_script[ BUFFER_SIZE];
int warp_daemon_id;
int warp_client_id;
int warp_processes;
int warp_delay;
int warp_emulation;
int warp_waittable;
int warp_helper_debug;
int warp_wait_time;

WARP_CONF_KEYWORD warp_conf_keyword[]={
	{	"WARP_IPC_KEY", CONF_TYPE_STRING, warp_ipc_key},
	{	"WARP_DAEMON_ID", CONF_TYPE_ALL, &warp_daemon_id},
	{	"WARP_CLIENT_ID", CONF_TYPE_ALL, &warp_client_id},
	{	"WARP_PROCESSES", CONF_TYPE_NUMERIC, &warp_processes},
	{	"WARP_INTERPROCESS_DELAY", CONF_TYPE_NUMERIC, &warp_delay},
	{	"WARP_SNAPSHOT_SCRIPT", CONF_TYPE_STRING, warp_script},
	{	"WARP_EMULATION_FLAG", CONF_TYPE_NUMERIC, &warp_emulation},
	{	"WARP_WAITING_TABLE", CONF_TYPE_NUMERIC, &warp_waittable},
	{	"WARP_HELPER_DEBUG", CONF_TYPE_NUMERIC, &warp_helper_debug},
	{	"WARP_WAIT_TIME", CONF_TYPE_NUMERIC, &warp_wait_time},
	{	"", 0},
};

/*------------------------------------------------------------------*/
/* Config reader                                                    */
/*------------------------------------------------------------------*/
int delete_space( char *dst, char *src)
{
	int i;
	int ret;

	ret = 0;

	for ( i = 0; i < BUFFER_SIZE; i++){
		if (*src == ' ') {
			src ++;
			continue;
		}

		if ( ( *src == 0x0d) || ( *src == 0x0a) || ( *src == 0x00)){
			*dst = 0x00;
			break;
		}
		*dst = *src;
		dst++;
		src++;
		ret++;
	}
	return ret;
}

void get_conf_value( char *buffer, int index, int line)
{
	int is_numeric_flag;
	int value;
	int i;
	char val_string[BUFFER_SIZE];

	is_numeric_flag = 0;
	value = -1;

	// if starting with double quotes...
	if ( buffer[ 0] == '\"' ){
		for( i = 1; i < BUFFER_SIZE; i ++){
			if ( ( buffer[ i] == 0 ) || (buffer[ i]=='\"')){
				val_string[ i - 1] = 0;
				break;
			}else{
				val_string[ i - 1] = buffer[ i];
			}
		}
		is_numeric_flag = 0;
	}else{
		is_numeric_flag = 1;
		if ( ( 0x30 <= buffer[0] ) && ( buffer[0] <= 0x39 )){
			value = atoi( buffer);
		}else{
			if ( warp_helper_debug) printf("Warning : Right value ? %s line %d\n", warp_conf_keyword[ index].ident, line);
			value = 0;
		}
	}
	if ( ( warp_conf_keyword[ index].type == CONF_TYPE_STRING) && ( is_numeric_flag == 1)){
#ifdef WARP_HELPER_SYSLOG
		syslog(LOG_ERR, "Error : Need string for %s line %d\n", warp_conf_keyword[ index].ident, line);
#else
		if ( warp_helper_debug) printf("Error : Need string for %s line %d\n", warp_conf_keyword[ index].ident, line);
#endif
	}else
	if ( ( warp_conf_keyword[ index].type == CONF_TYPE_STRING) && ( is_numeric_flag == 0)){
		strcpy( (char *)warp_conf_keyword[ index].ptr, val_string);
		if ( warp_helper_debug) printf("String Data %s = %s\n", warp_conf_keyword[ index].ident, (char *)warp_conf_keyword[ index].ptr);
	}else
	if ( ( warp_conf_keyword[ index].type == CONF_TYPE_ALL) && ( is_numeric_flag == 0)){
		*(int *)warp_conf_keyword[ index].ptr = val_string[0];
		if ( warp_helper_debug) printf("Value Data %s = %d\n", warp_conf_keyword[ index].ident, val_string[0]);
	}else{
		if (value == -1){
			if ( warp_helper_debug) printf("Warning : Right value ? %s line %d\n", warp_conf_keyword[ index].ident, line);
			value = 0;
		}
		*(int *)warp_conf_keyword[ index].ptr = value;
		if ( warp_helper_debug) printf("Value Data %s = %d\n", warp_conf_keyword[ index].ident, *(int *)warp_conf_keyword[ index].ptr);
	}
}

void parse_conf( char *buffer, int line)
{
	int i,len;
	char *ptr;

	// exit if the line satrts with '#'
	if ( buffer[0]=='#' ) return;

	i = 0;
	while( 1){
		if ( warp_conf_keyword[ i].ident[0] == 0 ){
#ifdef WARP_HELPER_SYSLOG
			syslog(LOG_ERR, "Warning : Unkown keyword %s : line %d\n", buffer, i);
#else
			if ( warp_helper_debug) printf("Warning : Unkown keyword %s : line %d\n", buffer, i);
#endif
			break;
		}

		ptr = strstr( buffer, warp_conf_keyword[i].ident);
		if ( ptr){
			// keyword string is present

			// find '='
			len = strlen( warp_conf_keyword[ i].ident);
			if ( buffer[len] == '='){
				// get config value
				get_conf_value( &buffer[len+1], i, line);
			}else{
#ifdef WARP_HELPER_SYSLOG
				syslog(LOG_ERR, "Error : Need '=' : line %d\n", line);
#else
				if ( warp_helper_debug) printf("Error : Need '=' : line %d\n", line);
#endif
			}
			break;
		}
		i++;
	}
}

int read_conffile(char *full_path)
{
	FILE *conf_file;
	int ret;
	int i;
	char read_buf[BUFFER_SIZE];
	char parse_buf[BUFFER_SIZE];

	conf_file = fopen( full_path, "r");
	if (conf_file){
		i = 1;
		memset(warp_ipc_key, 0, BUFFER_SIZE);
		memset(warp_script, 0, BUFFER_SIZE);
		warp_daemon_id = 0;
		warp_client_id = 0;
		warp_processes = 0;
		warp_delay = 0;
		warp_emulation = 0;
		warp_waittable = 0;
		warp_helper_debug = 0;
		warp_wait_time = 0;

		// read config file
		while( fgets( read_buf , sizeof( read_buf) , conf_file ) != NULL ) {

			// delete extra space
			ret = delete_space( parse_buf, read_buf);

			if ( ret != 0 ){
				// parse
				parse_conf( parse_buf, i);
			}
			i ++;
		}
		fclose( conf_file);
	}else{
#ifdef WARP_HELPER_SYSLOG
		syslog(LOG_ERR, "Error : Configuration file %s not found\n", full_path);
#else
		if ( warp_helper_debug) printf("Error : Configuration file %s not found\n", full_path);
#endif
		return -1;
	}
	return 0;
}
