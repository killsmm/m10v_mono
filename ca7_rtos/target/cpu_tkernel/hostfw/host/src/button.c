/**
 * @file		button.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "fj_host.h"
#ifdef CO_HOST_PROC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "button.h"
#include "button_gpio.h"

#define BUTTON_INPUT_DEV_NUM 1

static OS_USER_ER_ID	g_button_cyc_id = 0;
static T_OS_USER_CCYC	g_button_ccyc;
static ButtonDevCtx_t 	g_ButtonDevCtx[BUTTON_INPUT_DEV_NUM];

/**
 * A Cyclic handler to sample button and send message to HOST.
 */
static void FJ_ButtonSample(void)
{
	int cnt;

	for (cnt = 0; cnt < BUTTON_INPUT_DEV_NUM; cnt++) {
		g_ButtonDevCtx[cnt].FnSample();
		g_ButtonDevCtx[cnt].FnSendMessage();
	}
}

void FJ_ButtonInit(USHORT SampleTime)
{
	int cnt = 0;
	
	memset(&g_ButtonDevCtx, 0 , sizeof(g_ButtonDevCtx));
	g_ButtonDevCtx[cnt++] = g_ButtonGpioCtx;

	for (cnt = 0; cnt < BUTTON_INPUT_DEV_NUM; cnt++) {
		g_ButtonDevCtx[cnt].FnInit();
	}

	g_button_ccyc.exinf	= 0;
	g_button_ccyc.cycatr = D_OS_USER_TA_HLNG | D_OS_USER_TA_STA;
	g_button_ccyc.cychdr = FJ_ButtonSample;
	g_button_ccyc.cyctim = SampleTime;
}

void FJ_ButtonStartSample(void)
{
	g_button_cyc_id = OS_User_Acre_Cyc(&g_button_ccyc);	// Handler generate of cycle
}

void FJ_ButtonStopSample(void)
{
	OS_User_Stp_Cyc(g_button_cyc_id);
}

#endif

