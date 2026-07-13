/**
 * @file		MessageQueue.c
 * @brief		Message Queue Module
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include <stdlib.h>
#include "os_user_custom.h"
#include "ddim_typedef.h"
#include "MessageQueue.h"



#define kMaxMessage  (32)    //  be 2 of power
#define kMsg_InvalidMessageID	(0xfffffffe)


typedef struct _MsgQueue_{
	MSG		msg[kMaxMessage];
	ULONG readptr;
	ULONG writeptr;
}MsgQueue;


static MsgQueue*	queue=0;
static ULONG sTotalQueue=0;

INT32 MsgQueue_init(ULONG totalqueue)
{
	INT32 i;

	if (totalqueue ==0)
		return kMsgQueue_OK;
	if (queue != NULL)
		FREE (queue);

	queue = (MsgQueue*)malloc(totalqueue * sizeof(MsgQueue));
	if (queue == NULL)
		return kErr_MsgQueueErrMalloc;

	sTotalQueue = totalqueue;
	for (i = 0; i < sTotalQueue; i++){
		queue[i].readptr = queue[i].writeptr =0;
	}
	return kMsgQueue_OK;
}


INT32 SendMessage(MSG msg, eMsgQueueID queueID, ULONG timeout)
{
	//ULONG writepointer;
	MsgQueue *q;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;

	q = &queue[queueID];

	// wait space of the message buffer
//	while( (q->readptr +kMaxMessage ) <= q->writeptr){
//		OS_User_Tslp_Tsk(20);
//	}

	for( ; timeout >4 ; timeout -=5){
		if ( (q->readptr +kMaxMessage ) < q->writeptr){
			OS_User_Tslp_Tsk(1); // no keyspace, wait 5 ms
		}else {
			break;
		}
	}

	if ( (q->readptr +kMaxMessage ) < q->writeptr){
		OS_User_Tslp_Tsk(timeout); // no keyspace, wait reminder ms
		if ( (q->readptr +kMaxMessage ) < q->writeptr){
			return kErr_MsgQueueTimeout;
				
		}
	}


	OS_User_Dis_Dsp();
	q->msg[q->writeptr%kMaxMessage] = msg;
	//writepointer = q->writeptr;
	q->writeptr++;

	if (q->writeptr > 0xF0000000){ // avoid pointer over-flow
		q->writeptr -= 0x80000000;
		q->readptr -= 0x80000000;
	}

	OS_User_Ena_Dsp();
	
	// wait until user receive the Message.
////////////////////////////////
////////////////////////////////
///////////////////////////Need to modify
////////////////////////////////

	//This part is not used
#if 0
	while(q->readptr <= writepointer){
		OS_User_Tslp_Tsk(20);
	}
#endif

	return kMsgQueue_OK;
}

//INT32 SendMessageEx(INT32 msg_id, eMsgQueueID queueID, ULONG timeout){
//	MSG msg;
//	msg.msgid = msg_id;
//	return SendMessage(msg, queueID, timeout);
//}

INT32 PostMessage(MSG msg, eMsgQueueID queueID)
{
	//ULONG writepointer;
	MsgQueue *q;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];

	// wait space of the message buffer
	if ((q->readptr +kMaxMessage ) <= q->writeptr){
		return kErr_MsgQueueNoSpace;
	}
	
	OS_User_Dis_Dsp();
	q->msg[q->writeptr%kMaxMessage] = msg;
	//writepointer = q->writeptr;
	q->writeptr++;

	if (q->writeptr > 0xF0000000){ // avoid pointer over-flow
		q->writeptr -= 0x80000000;
		q->readptr -= 0x80000000;
	}
	OS_User_Ena_Dsp();
	
	return kMsgQueue_OK;

}



INT32 GetMessage(MSG *pmsg, eMsgQueueID queueID, ULONG timeout){
	MsgQueue *q;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	
	// wait space of the message buffer
//	while((q->readptr) >= q->writeptr){
//		OS_User_Tslp_Tsk(20);
//	}


	for( ; timeout >4 ; timeout -=5){
		if ( (q->readptr) >= q->writeptr){
			OS_User_Tslp_Tsk(1); // no key, wait 5 ms
		}else {
			break;
		}
	}

	if ( (q->readptr) >= q->writeptr){
		OS_User_Tslp_Tsk(timeout); // no key, wait reminder ms
		if ( (q->readptr) >= q->writeptr){
			return kErr_MsgQueueTimeout;
		}
	}

	OS_User_Dis_Dsp();
	*pmsg = q->msg[q->readptr%kMaxMessage] ;
	q->readptr++;
	OS_User_Ena_Dsp();
	
	return kMsgQueue_OK;
}

INT32 GetMessageRemainSize(eMsgQueueID queueID)
{
	MsgQueue *q;
	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
		
	return ( (q->writeptr) - (q->readptr));
}


INT32 PeekMessage(MSG *pmsg, eMsgQueueID queueID, INT32 type){
	MsgQueue *q;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	
	// wait space of the message buffer
	if((q->readptr) >= q->writeptr){
		return kErr_MsgQueueNoMsg;  // no Msg exist
	}

	OS_User_Dis_Dsp();
	*pmsg = q->msg[q->readptr%kMaxMessage];
	if (type ==kRemoveMessage) q->readptr++;
	OS_User_Ena_Dsp();
	
	return kMsgQueue_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"
// Do not use this function until OS_User_Loc/Unl_Cpu is enabled. 
INT32 MsgQueue_ClearAll(eMsgQueueID queueID){
	MsgQueue *q;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	
//	OS_User_Loc_Cpu();
	q->readptr = q->writeptr;
//	OS_User_Unl_Cpu();
	return kMsgQueue_OK;
}
// Do not use this function until OS_User_Loc/Unl_Cpu is enabled. 
INT32 MsgQueue_Reset(eMsgQueueID queueID){
	MsgQueue *q;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	
//	OS_User_Loc_Cpu();
	q->readptr = q->writeptr=0;
//	OS_User_Unl_Cpu();
	return kMsgQueue_OK;
}

// Do not use this function until OS_User_Loc/Unl_Cpu is enabled. 
INT32 MsgQueue_RemoveMsgRange(eMsgQueueID queueID, ULONG min_msgid, ULONG max_msgid ){
	MsgQueue *q;
	ULONG index;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	
//	OS_User_Loc_Cpu();

	for (index = q->readptr ; index < q->writeptr ; index++)
	{
		ULONG msgid;
		msgid =  q->msg[index%kMaxMessage].msgid;

		if ((msgid >= min_msgid) && (msgid <= max_msgid)){
			q->msg[index%kMaxMessage].msgid = kMsg_InvalidMessageID;
		}
	}

//	OS_User_Unl_Cpu();
	return kMsgQueue_OK;
}


// Keep message appear once in a queue
INT32 MsgQueue_RemoveRepeatMsg(eMsgQueueID queueID, ULONG msgid){
	MsgQueue *q;
	ULONG index;
	ULONG msgcnt;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	msgcnt =0;
//	OS_User_Loc_Cpu();

	for (index = q->readptr ; index < q->writeptr ; index++)
	{
		if (msgid == q->msg[index%kMaxMessage].msgid){
			msgcnt++;
			break;
		}
	}
	index++;
	for (; index < q->writeptr ; index++)
	{

		if (msgid == q->msg[index%kMaxMessage].msgid){
			q->msg[index%kMaxMessage].msgid = kMsg_InvalidMessageID;
			msgcnt++;
		}
	}
//	OS_User_Unl_Cpu();
	return msgcnt;
}



// Is message already in unread buffer
INT32 MsgQueue_IsMsgInQueue(eMsgQueueID queueID, ULONG msgid){
	MsgQueue *q;
	ULONG index;

	if (queueID >= sTotalQueue)
		return kErr_MsgQueueNotExist;
	
	q = &queue[queueID];
	
	for (index = q->readptr ; index < q->writeptr ; index++)
	{
		if (msgid == q->msg[index%kMaxMessage].msgid){
			return 1;
		}
	}
	return 0;
}

