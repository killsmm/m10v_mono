/**
 * @file		USBHDC_Dev_Callback.h
 * @brief		This is a header file for default callback function.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_CALLBACK
#define H_USBHDC_DEV_CALLBACK

#include "USBHDC_StdDef.h"

/* macro definition declaration */
/* max count of callback funtions */
#define DEV_MAX_CALLBACK_ENTRY		74

/* callback function ID */
#define DEV_ID_CLBCK_EP1TRANS					0
#define DEV_ID_CLBCK_EP2TRANS					1
#define DEV_ID_CLBCK_EP3TRANS					2
#define DEV_ID_CLBCK_EP4TRANS					3
#define DEV_ID_CLBCK_EP5TRANS					4
#define DEV_ID_CLBCK_EP6TRANS					5
#define DEV_ID_CLBCK_EP7TRANS					6
#define DEV_ID_CLBCK_EP8TRANS					7
#define DEV_ID_CLBCK_EP9TRANS					8
#define DEV_ID_CLBCK_EP10TRANS					9
#define DEV_ID_CLBCK_EP11TRANS					10
#define DEV_ID_CLBCK_EP12TRANS					11
#define DEV_ID_CLBCK_EP13TRANS					12
#define DEV_ID_CLBCK_EP14TRANS					13
#define DEV_ID_CLBCK_EP15TRANS					14
#define DEV_ID_CLBCK_CLEARFEATURE				15
#define DEV_ID_CLBCK_SETFEATURE					16
#define DEV_ID_CLBCK_SETCONFIGURATION			17
#define DEV_ID_CLBCK_SETINTERFACE				18
/* Reserved ID 19 - 21 */
#define DEV_ID_CLBCK_CLASSREQUEST				22
#define DEV_ID_CLBCK_CLASSREQUEST_IN			23
#define DEV_ID_CLBCK_CLASSREQUEST_OUT			24
#define DEV_ID_CLBCK_VENDORREQUEST				25
#define DEV_ID_CLBCK_VENDORREQUEST_IN			26
#define DEV_ID_CLBCK_VENDORREQUEST_OUT			27
#define DEV_ID_CLBCK_BUSRESETBEGIN				28
#define DEV_ID_CLBCK_BUSRESETEND				29
#define DEV_ID_CLBCK_SUSPENDBEGIN				30
#define DEV_ID_CLBCK_SUSPENDEND 				31
#define DEV_ID_CLBCK_HUNGUPRECOVER				32
/* Reserved ID 33 */
#define DEV_ID_CLBCK_DMA1END					34
#define DEV_ID_CLBCK_DMA2END					35
/* Reserved ID 36 - 37 */
#define DEV_ID_CLBCK_CLASSREQUEST_NORMAL_END	38
#define DEV_ID_CLBCK_CLASSREQUEST_FAIL_END		39
#define DEV_ID_CLBCK_VENDORREQUEST_NORMAL_END	40
#define DEV_ID_CLBCK_VENDORREQUEST_FAIL_END		41
#define DEV_ID_CLBCK_SOFINT						42
#define DEV_ID_CLBCK_NACKINT					43
#define DEV_ID_CLBCK_STALLINT					44
#define DEV_ID_CLBCK_PINGINT					45
#define DEV_ID_CLBCK_EMPTYINT					46
/* Reserved ID 47 - 49 */
#define DEV_ID_CLBCK_EP1TRANSFER				50
#define DEV_ID_CLBCK_EP2TRANSFER				51
#define DEV_ID_CLBCK_EP3TRANSFER				52
#define DEV_ID_CLBCK_EP4TRANSFER				53
#define DEV_ID_CLBCK_EP5TRANSFER				54
#define DEV_ID_CLBCK_EP6TRANSFER				55
#define DEV_ID_CLBCK_EP7TRANSFER				56
#define DEV_ID_CLBCK_EP8TRANSFER				57
#define DEV_ID_CLBCK_EP9TRANSFER				58
#define DEV_ID_CLBCK_EP10TRANSFER				59
#define DEV_ID_CLBCK_EP11TRANSFER				60
#define DEV_ID_CLBCK_EP12TRANSFER				61
#define DEV_ID_CLBCK_EP13TRANSFER				62
#define DEV_ID_CLBCK_EP14TRANSFER				63
#define DEV_ID_CLBCK_EP15TRANSFER				64
/* Reserved ID 65 - 69 */
#define DEV_ID_CLBCK_CLASSREQUEST_OUT_TRANSFER	70
#define DEV_ID_CLBCK_VENDORREQUEST_OUT_TRANSFER	71
#define DEV_ID_CLBCK_CONNECT					72
#define DEV_ID_CLBCK_DISCONNECT					73


/* function prototype declaration */
/* default callback no processing function */
extern void Dev_DefClBck_NoProcess(void);

/* default callback function called when reception of class request finished */
extern void Dev_DefClBck_ClassRequestSetupEnd(void);

/* default callback function called when reception of vendor request finished */
extern void Dev_DefClBck_VendorRequestSetupEnd(void);

#endif
