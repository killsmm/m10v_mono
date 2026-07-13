/**
 * @file		button.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef CO_HOST_PROC

#include "user.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define BUTTON_RELEASE	(0) /**< button release definition */
#define BUTTON_PUSH		(1) /**< button push definition */
#define BUTTON_REPEAT	(2) /**< button repeat definition */
#define FJ_HM_BUTTON	(0x00400000) /**< HOST message of button ID */

/**
Button message definitions
*/
#define MSG_BUTTON_MODE_AUTO		0x00001000
#define MSG_BUTTON_MODE_PROG		0x00002000
#define MSG_BUTTON_MODE_MOVIE		0x00003000
#define MSG_BUTTON_MODE_SCN			0x00004000
#define MSG_BUTTON_MODE_PORT		0x00005000
#define MSG_BUTTON_MODE_LAND		0x00006000
#define MSG_BUTTON_MODE_SSNAP		0x00007000
#define MSG_BUTTON_MODE_SMILE		0x00008000
#define MSG_BUTTON_MODE_ISO			0x00009000
#define MSG_BUTTON_MODE_EASY		0x0000A000
#define MSG_BUTTON_SPECIAL_UPDATE	0x0000B000

//ADC(Release ID)
#define	MSG_BUTTON_UP_RELEASE 			0x00000100
#define	MSG_BUTTON_DOWN_RELEASE 		0x00000200
#define	MSG_BUTTON_LEFT_RELEASE 		0x00000300
#define	MSG_BUTTON_RIGHT_RELEASE 		0x00000400
#define	MSG_BUTTON_ENTER_RELEASE 		0x00000500
#define	MSG_BUTTON_ZOOM_IN_RELEASE 		0x00000600
#define	MSG_BUTTON_ZOOM_OUT_RELEASE		0x00000700
#define	MSG_BUTTON_MENU_RELEASE			0x00000800
#define	MSG_BUTTON_MODE_RELEASE 		0x00000900
#define	MSG_BUTTON_HOME_RELEASE 		0x00000A00
//GPIO(Release ID)
#define	MSG_BUTTON_HALF_SHUTTER_RELEASE 0x00000B00
#define	MSG_BUTTON_FULL_SHUTTER_RELEASE 0x00000C00
#define	MSG_BUTTON_PLAYBACK_RELEASE 	0x00000D00
#define	MSG_BUTTON_RECORD_RELEASE 		0x00000E00

//ADC(Push ID)
#define	MSG_BUTTON_UP_PUSH 				(MSG_BUTTON_UP_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_DOWN_PUSH 			(MSG_BUTTON_DOWN_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_LEFT_PUSH 			(MSG_BUTTON_LEFT_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_RIGHT_PUSH 			(MSG_BUTTON_RIGHT_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_ENTER_PUSH 			(MSG_BUTTON_ENTER_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_ZOOM_IN_PUSH 		(MSG_BUTTON_ZOOM_IN_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_ZOOM_OUT_PUSH 		(MSG_BUTTON_ZOOM_OUT_RELEASE | BUTTON_PUSH)
#define MSG_BUTTON_MENU_PUSH 			(MSG_BUTTON_MENU_RELEASE | BUTTON_PUSH)	
#define	MSG_BUTTON_MODE_PUSH 			(MSG_BUTTON_MODE_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_HOME_PUSH 			(MSG_BUTTON_HOME_RELEASE | BUTTON_PUSH)
//GPIO(Push ID)
#define	MSG_BUTTON_HALF_SHUTTER_PUSH 	(MSG_BUTTON_HALF_SHUTTER_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_FULL_SHUTTER_PUSH 	(MSG_BUTTON_FULL_SHUTTER_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_PLAYBACK_PUSH 		(MSG_BUTTON_PLAYBACK_RELEASE | BUTTON_PUSH)
#define	MSG_BUTTON_RECORD_PUSH 			(MSG_BUTTON_RECORD_RELEASE | BUTTON_PUSH)

//ADC(Repeat ID)
#define	MSG_BUTTON_UP_REPEAT 			(MSG_BUTTON_UP_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_DOWN_REPEAT 			(MSG_BUTTON_DOWN_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_LEFT_REPEAT 			(MSG_BUTTON_LEFT_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_RIGHT_REPEAT 		(MSG_BUTTON_RIGHT_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_ENTER_REPEAT 		(MSG_BUTTON_ENTER_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_ZOME_IN_REPEAT 		(MSG_BUTTON_ZOOM_IN_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_ZOME_OUT_REPEAT 		(MSG_BUTTON_ZOOM_OUT_RELEASE | BUTTON_REPEAT)
#define MSG_BUTTON_MENU_REPEAT 			(MSG_BUTTON_MENU_RELEASE | BUTTON_REPEAT)	
#define	MSG_BUTTON_MODE_REPEAT 			(MSG_BUTTON_MODE_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_HOME_REPEAT 			(MSG_BUTTON_HOME_RELEASE | BUTTON_REPEAT)
//GPIO(Repeat ID)
#define	MSG_BUTTON_HALF_SHUTTER_REPEAT 	(MSG_BUTTON_HALF_SHUTTER_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_FULL_SHUTTER_REPEAT 	(MSG_BUTTON_FULL_SHUTTER_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_PLAYBACK_REPEAT 		(MSG_BUTTON_PLAYBACK_RELEASE | BUTTON_REPEAT)
#define	MSG_BUTTON_RECORD_REPEAT 		(MSG_BUTTON_RECORD_RELEASE | BUTTON_REPEAT)

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/**
Value of button debounce.
*/
typedef struct ButtonDebounce_s {
	USHORT push; /**< Debounce value when button pushed */
	USHORT release; /**< Debounce value when button released */
	USHORT repeat; /**< Debounce value when button repeated */
} ButtonDebounce_t;

/**
Button controller which is constructor of button,<br>
implement each type of button and link to these function pointer.
*/
typedef struct ButtonDevCtx_s {
    int (*FnInit)(void); /**< Button initialized function pointer */
    int (*FnSample)(void); /**< Button sampled function pointer */
    int (*FnSendMessage)(void); /**< Button message sending function pointer */
} ButtonDevCtx_t;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Initialize button controller and each button devices.
@param [in] SampleTime: Sample rate value in ms.
*/
void FJ_ButtonInit(USHORT SampleTime);

/**
To start sample button device.
*/
void FJ_ButtonStartSample(void);

/**
To stop sample button device.
*/
void FJ_ButtonStopSample(void);

#ifdef __cplusplus
}
#endif

#endif
#endif

