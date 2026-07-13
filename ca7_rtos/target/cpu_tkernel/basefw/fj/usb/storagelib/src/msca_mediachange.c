/**
 * @file		msca_mediachange.c
 * @brief		USB Mass Storage Class Media Change Functions
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#define USB_EN_SECTION

//#include "mlna.h"
#include "msclib.h"
#include "media_state.h"
#include "msca_mediachange.h"

#include "MSC_Section.h"

/*******************/
/* Global Variable */
/*******************/
extern unsigned char MSCS_LUN;
extern MSCA_MEDIA_FLAG_T MediaChangeFlag[];

/****************************************/
/*  Media Chage Set by Drive No         */
/****************************************/
int MSCA_MDChangeSet_Drive(unsigned char dno)
{
	if( dno >= MSCS_LUN )
		return MSC_ERROR;
	MediaChangeFlag[dno].change = 0;
	return MSC_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"

/****************************************/
/*  Media Chage Clear by Drive No       */
/****************************************/
int MSCA_MDChangeClr_Drive(unsigned char dno)
{
	if( dno >= MSCS_LUN )
		return MSC_ERROR;
	MediaChangeFlag[dno].change = -1;
	return MSC_OK;
}

/****************************************/
/*  Media Chage Set by Media Type       */
/****************************************/
int MSCA_MDChangeSet_Media(E_BF_MEDIA_STATE_MEDIA_TYPE mtype)
{
	unsigned int i;
	
	for(i=0; i<MSCS_LUN; i++)
	{
		if( mtype == MediaChangeFlag[i].type )
		{
			MediaChangeFlag[i].change = 0;
			return MSC_OK;
		}
	}
	return MSC_ERROR;
}

/****************************************/
/*  Media Chage Clear by Media Type     */
/****************************************/
int MSCA_MDChangeClr_Media(E_BF_MEDIA_STATE_MEDIA_TYPE mtype)
{
	unsigned int i;
	
	for(i=0; i<MSCS_LUN; i++)
	{
		if( mtype == MediaChangeFlag[i].type )
		{
			MediaChangeFlag[i].change = -1;
			return MSC_OK;
		}
	}
	return MSC_ERROR;
}

/***************************************************/
/*  Media Chage Set by Drive No & Media Type       */
/***************************************************/
int MSCA_MDChangeSet_DriveMedia(unsigned char dno, E_BF_MEDIA_STATE_MEDIA_TYPE mtype)
{

	if( dno >= MSCS_LUN )
		return MSC_ERROR;
	
	MediaChangeFlag[dno].type = mtype;
	MediaChangeFlag[dno].change = 0;
	
	return MSC_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"

// add 040810 for Media Change Problem Win2k, WinMe
/***************************************************/
/*  Check Media Chage Flag by Drive No             */
/***************************************************/
int MSCA_MDChkChange_Drive(unsigned char dno)
{
	int ret;
	
	if( dno >= MSCS_LUN )
		return MSC_ERROR;
	if( MediaChangeFlag[dno].change >= 0 )
	{
		ret = MSC_CHANGED;
	}
	else 
	{
		ret = MSC_OK;
	}
	return ret;
}

/***************************************************/
/*  Media Chage Update media type                  */
/***************************************************/
int MSCA_MDChangeUpdateType(unsigned char dno, E_BF_MEDIA_STATE_MEDIA_TYPE mtype) {
	
	if (dno >= MSCS_LUN) return MSC_ERROR;
	
	MediaChangeFlag[dno].type = mtype;
	
	printf("MSCA_MDChangeUpdateType() dno = %d, type = %d\n", dno, mtype);
	
	return MSC_OK;
}

