/*
 * warp_helper_config.h
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 */

#define MAX_IDENT			128
#define BUFFER_SIZE			256
#define CONF_TYPE_ALL			1
#define CONF_TYPE_STRING		2
#define CONF_TYPE_NUMERIC		3
#define MAX_TIME_WAIT			10

// keyword conversion map
typedef struct tagWARP_CONF_KEYWORD{
	char ident[ MAX_IDENT];
	int type;
	void *ptr;
} WARP_CONF_KEYWORD;

extern char warp_ipc_key[ BUFFER_SIZE];
extern char warp_script[ BUFFER_SIZE];
extern int warp_daemon_id;
extern int warp_client_id;
extern int warp_processes;
extern int warp_delay;
extern int warp_emulation;
extern int warp_waittable;
extern int warp_helper_debug;
extern int warp_wait_time;

int read_conffile(char *full_path);
