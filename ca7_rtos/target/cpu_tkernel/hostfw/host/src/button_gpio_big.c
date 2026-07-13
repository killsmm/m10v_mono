/**
 * @file		button_gpio_big.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#if defined(BOARD_TYPE_SNI) // TODO:

#include "fj_host.h"
#ifdef CO_HOST_PROC
#include <stdlib.h>
#include "button.h"
#include "fj_message.h"

#define HSHUTTER_GPIO 	E_DD_TOP_GPIO_CM0_P02
#define FSHUTTER_GPIO 	E_DD_TOP_GPIO_CM0_P03

#define NUM_OF_BUTTON 	2

typedef enum _E_GpioButton
{
	BUTTON_HALF_SHUTTER = 0,
	BUTTON_FULL_SHUTTER ,
}E_GpioButton;

/**
 * A structure to check current GPIO button push or release.
 */
typedef union ButtonGPIOPush_s {
	UINT32	word;
	UCHAR	byte[4];
	struct {
	    ULONG h_shutter	:1;
		ULONG f_shutter	:1;
		ULONG resv		:30;
	} bit;
} ButtonGPIOPush_t;

/**
 * To record eahc button sampling count.
 */
typedef struct ButtonGPIOSampleCount_s {
	USHORT push; 	/**< Push count */
	USHORT release; /**< Release count */
} ButtonGPIOSampleCount_t;

/**
 * To record update status of each button.
 */
typedef union ButtonGPIOUpdate_s {
	ULONG word;
	struct {
		ULONG h_shutter	:1;
		ULONG f_shutter	:1;
		ULONG resv 		:30;
	} bit;
} ButtonGPIOUpdate_t;

/**
 * To record each current button value.
 */
typedef struct ButtonGPIOStatus_s {
	union {
		ULONG word;
		struct {
			ULONG h_shutter	:2;
			ULONG f_shutter	:2;
			ULONG resv 		:28;
		} bit;
	} sta;

	ButtonGPIOSampleCount_t SampleCnt[NUM_OF_BUTTON];
	ButtonGPIOUpdate_t 	update;
} ButtonGPIOStatus_t;

static ButtonGPIOStatus_t g_BStatus;

/**
 * Button messages that mapping to above button order.
 */
static ULONG g_MsgButtonGPIO[NUM_OF_BUTTON] = {
	MSG_BUTTON_HALF_SHUTTER_RELEASE,
	MSG_BUTTON_FULL_SHUTTER_RELEASE,
};
/**
 * Button debounce value.
 * Ex: If sampling rate is set to 1ms, and push:10, release:1, repeat:400.
 * the push debounce: 	 10  * 1 =  10ms.
 * the release debounce:  5  * 1 =   5ms.
 * the repeat debounce:	400  * 1  = 400ms.
 */
static const ButtonDebounce_t g_Debounce = {15, 1, 80};

/**
 * Initialize button GPIO device and g_BStatus set to 0.
 *
 * returns - 0 for successful, otherwise failure.
 */
static int ButtonGpioInit(void)
{
	Dd_Top_Set_Gpio_CM0_Direction(HSHUTTER_GPIO, FJ_GPIO_DIR_IN);
	Dd_Top_Set_Gpio_CM0_Direction(FSHUTTER_GPIO, FJ_GPIO_DIR_IN);

	memset(&g_BStatus, 0, sizeof(g_BStatus));
	return 0;
}

/**
 * Comparing previous button status to identify the current button
 * updating status.
 *
 * @param Push - TRUE means button been pressed.
 * @param Index - Index of button sampling count table.
 * @param Update - Set to 1 if button current status changed, otherwise set to 0
 * #return - Current status of button.
 */
static int ButtonGPIOSampling(ULONG Push, UCHAR Index, ULONG CurState, UCHAR *Update)
{
	*Update = 0;

	if (Push) {
		if (CurState != BUTTON_REPEAT)
			++g_BStatus.SampleCnt[Index].push;

		if (g_BStatus.SampleCnt[Index].push == g_Debounce.push) {
			g_BStatus.SampleCnt[Index].release = 0;
			UPRINTF("BUTTON[%d] PUSH\n", Index);
			*Update = 1;
			return BUTTON_PUSH;
		} else if ((CurState != BUTTON_REPEAT) &&
			  (g_BStatus.SampleCnt[Index].push == g_Debounce.repeat)) {
			g_BStatus.SampleCnt[Index].release = 0;
			UPRINTF("BUTTON[%d] REPEAT\n", Index);
			*Update = 1;
			return BUTTON_REPEAT;
			}
	} else {
		if (CurState == BUTTON_RELEASE) return CurState;

		++g_BStatus.SampleCnt[Index].release;

		if (g_BStatus.SampleCnt[Index].release == g_Debounce.release) {

			g_BStatus.SampleCnt[Index].push = 0;
			g_BStatus.SampleCnt[Index].release = 0;
			UPRINTF("BUTTON[%d] RELEASE\n", Index);
			*Update = 1;
			return BUTTON_RELEASE;
		}
	}
	return CurState;
}

/**
 * Update current button status.
 *
 * @param BPush - Stored current button push or release status.
 */
static inline void ButtonGPIOUpdate(ButtonGPIOPush_t *BPush)
{
	UCHAR Update;

	g_BStatus.update.word = 0;

	g_BStatus.sta.bit.h_shutter =
	ButtonGPIOSampling(BPush->bit.h_shutter, BUTTON_HALF_SHUTTER, g_BStatus.sta.bit.h_shutter, &Update);
	if (Update) g_BStatus.update.bit.h_shutter = 1;

	g_BStatus.sta.bit.f_shutter =
	ButtonGPIOSampling(BPush->bit.f_shutter, BUTTON_FULL_SHUTTER, g_BStatus.sta.bit.f_shutter, &Update);
	if (Update) g_BStatus.update.bit.f_shutter = 1;
}

/**
 * Sampling button on ADC device.
 */
static int ButtonGpioSample(void)
{
	UCHAR gpio_sts;
	ButtonGPIOPush_t BPush;

	memset(&BPush, 0, sizeof(ButtonGPIOPush_t));
	Dd_Top_Get_Gpio_CM0_Status(HSHUTTER_GPIO, &gpio_sts);
	BPush.bit.h_shutter = gpio_sts == FJ_GPIO_STATUS_HIGH ? BUTTON_PUSH : BUTTON_RELEASE;
	Dd_Top_Get_Gpio_CM0_Status(FSHUTTER_GPIO, &gpio_sts);
	BPush.bit.f_shutter = gpio_sts == FJ_GPIO_STATUS_HIGH ? BUTTON_PUSH : BUTTON_RELEASE;

	ButtonGPIOUpdate(&BPush);

	return 0;
}

/**
 * Send button ID message to HOST.
 */
static int ButtonGpioSendMsg(void)
{
	int i;
	int shift = 0;

	for (i = 0; i < NUM_OF_BUTTON; i++, shift += 2) {
		if (g_BStatus.update.word & (ULONG)(0x00000001 << i)) {
			FJ_HostPostMessage(FJ_HM_BUTTON, g_MsgButtonGPIO[i] | 
		                   ((g_BStatus.sta.word >> shift) & 0x03));
		}
	}

	return 0;
}

/**
 * A function pointers table that export to g_ButtonDevCtx.
 */
ButtonDevCtx_t g_ButtonGpioCtx = {
	ButtonGpioInit,
	ButtonGpioSample,
	ButtonGpioSendMsg
};
#endif

#endif

