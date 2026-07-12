/*
 * warp_helper_lib.h
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 */

#define ERR_SUCCESS			0
#define ERR_WARP_SUCCESS		1
#define ERR_CONFIG_FILE			10
#define ERR_IDENT_RX_FILE		11
#define ERR_WAPRD_RX_TIMEOUT		12
#define ERR_WAPRD_RX_Q			13
#define ERR_IDENT_TX_FILE		14
#define ERR_WAPRD_TX_TIMEOUT		15
#define ERR_WAPRD_TX_Q			16
#define ERR_SEND_FAIL			17
#define ERR_RECV_FAIL			18
#define ERR_SNAP_FAIL			19
#define ERR_PROCNUM_OVER		20
#define ERR_PROCNUM_ZERO		21


// library initialization
int warp_helper_libinit( char *full_path, int pno);

// freeze
int warp_helper_enter_freeze( void);

// error inquiry
void warp_helper_err_to_message( int err);

