/**
 * @file		msca_mediachange.h
 * @brief		USB Mass Storage class Media Change Structure
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __MSCA_MEDIACHANGE_H
#define __MSCA_MEDIACHANGE_H

#include "media_state.h"

/****************************************/
/*  Media Change Structure Definition   */
/****************************************/
typedef struct {
  E_BF_MEDIA_STATE_MEDIA_TYPE	type;		//  Media Type
  int			change;		//  Change Flag  -1:No Change  0:Change
} MSCA_MEDIA_FLAG_T;

/****************************************/
/*  Media Change Function Prototype     */
/****************************************/
int MSCA_MDChangeSet_Drive(unsigned char dno);
int MSCA_MDChangeClr_Drive(unsigned char dno);
int MSCA_MDChangeSet_Media(E_BF_MEDIA_STATE_MEDIA_TYPE mtype);
int MSCA_MDChangeClr_Media(E_BF_MEDIA_STATE_MEDIA_TYPE mtype);
int MSCA_MDChangeSet_DriveMedia(unsigned char dno, E_BF_MEDIA_STATE_MEDIA_TYPE mtype);
//--> add 040810 for Media Change problem on Win2K, WinME
int MSCA_MDChkChange_Drive(unsigned char dno);
#define MSC_CHANGED 9
//<--
int MSCA_MDChangeUpdateType(unsigned char dno, E_BF_MEDIA_STATE_MEDIA_TYPE mtype);

#endif /* __MSCA_MEDIACHANGE_H */
