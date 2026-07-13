/**
 * @file		MessageQueue.h
 * @brief		MessageQueue header
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _MESSAGEQUEUE_H_
#define _MESSAGEQUEUE_H_

// ****************************************************************************
//       Library Area (DO NOT modify any prototype and "define" in this area )
// ****************************************************************************

#define kKeepMessage		(1)
#define kRemoveMessage		(0)

#define kMsgQueue_OK			(0)
#define kErr_MsgQueueNotExist	(-1)
#define kErr_MsgQueueNoSpace	(-2)  
#define kErr_MsgQueueNoMsg		(-3)  
#define kErr_MsgQueueTimeout	(-4)
#define kErr_MsgQueueErrMalloc	(-5)
#define kErr_MsgQueueUnknown	(-100)

#define kMsgWaitInfinity	(0xffffffff)

typedef struct _MSG_{
ULONG msgid;
ULONG ulPara1;
VOID *vpPara2;
	union  {
	ULONG	ulPara3;
	VOID * 	vpPara3;
	struct {
		UCHAR subid;
		UCHAR time;
		USHORT usPara3;
		}bit;
	}para3;
}MSG;

typedef ULONG eMsgQueueID;
INT32 MsgQueue_init(ULONG totalqueue);
INT32 SendMessage(MSG msg, eMsgQueueID QueueID, ULONG timeout);
//INT32 SendMessageEx(INT32 msg_id, eMsgQueueID queueID, ULONG timeout);
INT32 PostMessage(MSG msg, eMsgQueueID QueueID);
INT32 GetMessage(MSG *pmsg, eMsgQueueID QueueID, ULONG timeout);
INT32 PeekMessage(MSG *pmsg, eMsgQueueID QueueID, INT32 type);
INT32 MsgQueue_ClearAll(eMsgQueueID queueID);
INT32 GetMessageRemainSize(eMsgQueueID queueID);
INT32 MsgQueue_Reset(eMsgQueueID queueID);
INT32 MsgQueue_RemoveMsgRange(eMsgQueueID queueID, ULONG min_msgid, ULONG max_msgid );
INT32 MsgQueue_RemoveRepeatMsg(eMsgQueueID queueID, ULONG msgid);
INT32 MsgQueue_IsMsgInQueue(eMsgQueueID queueID, ULONG msgid);

enum {
eDpsSubSysMQ,
eMaxMsgQueue, 
};


#endif 
