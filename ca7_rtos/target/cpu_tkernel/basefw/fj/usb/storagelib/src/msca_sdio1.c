/**
 * @file		msca_sdio1.c
 * @brief		USB Mass Storage class Media Access Function for SD
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

//#include "mlna.h"
#include "msclib.h"
#include "msca_mediachange.h"  //  add 040810
#include "MSCA.h"
#include "msca_sdio1.h"
#include "mwsdio.h"
#include "sd.h"
#include "mw_user_custom.h"

//#define USE_EVENT_TIME
#ifdef USE_EVENT_TIME
	#include "measure_time.h"
	unsigned long bnT,edT;
#endif


volatile T_SD1_MNG *SDIO1_DevInfo = &gSD1_dev_mng;

static unsigned char DriveNo1;


void sdio1_changeDrive(unsigned char driveno)
{
    MWSDIO1_ChangeRCA(SDIO1_DevInfo->rca);

}

int MSCA_SDIO1_Init(unsigned char driveno)
{
	int ret = D_SD1_ERR;

	DriveNo1 = driveno;
#if 0 /* NEED_2_CHANGE */
	IO_SDIO1.HostControl.bit.LEDX= 1;				// SDIO Power ON
#endif
	
//	sdio1_changeDrive(driveno);
//	
//	if( MWSDIO1_Get_SDCDX() == 1/*D_SD1_ANY_CARD*/ )
//	{
//		if( MSCA_MDChkChange_Drive(DriveNo1) == MSC_CHANGED )
//		{
			ret = SD1_sdio_init();
//		}
//	}
//	else
//	{
//		MSCA_MDChangeClr_Drive(DriveNo1);
//	}
	
	if( ret == D_SD1_OK )
	{
		return(MSC_OK);
	}
	else
	{
		return(MSC_ERROR);
	}
}

int MSCA_SDIO1_Reset(void)
{
	return (MSC_OK);
}

int MSCA_SDIO1_GetDriveInfo(MSCA_IDI *lpidi)
{
	lpidi->bytepersector	= SDIO1_DevInfo->bytepersector;
	lpidi->current_cylinder	= SDIO1_DevInfo->cylinder ;
	lpidi->current_head		= SDIO1_DevInfo->head ;
	lpidi->current_sectrack	= SDIO1_DevInfo->sectrack ;
	lpidi->lba_sector		= SDIO1_DevInfo->totalsector;
	return( MSC_OK );
}

//  20051103 Modify for media change problem
int MSCA_SDIO1_CheckMedia( void)
{
    if( MWSDIO1_CardCheck() == 0 )
	{
		/* Set Sensekey NOSENSE */
		return(MSC_OK);
	}
	else
	{
		/* Error */
		return(MSC_ERROR);
	}
}

int MSCA_SDIO1_CheckWriteProtect( void )
{
	if( MWSDIO1_CardCheckWP() != D_SD1_OK )
	{	// Not Protected
		return(MSC_OK);
	}
	else
	{	// Protected
		return(MSC_ERROR);
	}
}

int MSCA_SDIO1_LockMedia( void )
{
	return(MSC_OK);
}

int MSCA_SDIO1_UnlockMedia( void )
{
	return(MSC_OK);
}

int MSCA_SDIO1_ReadLBA(
					  unsigned long	lba_sector ,
					  unsigned char	rdSecN ,
					  unsigned char	*lpbuf )
{
	unsigned long	errorSecNo;
	unsigned char	status ;
	unsigned short	retry_cnt=MSCS_SDIO1_ReadRetryCount;
	int	ret;

	if( rdSecN == 0 ) return(0);

	ret=MWSDIO1_ReadSectors( lba_sector, rdSecN, lpbuf, &errorSecNo, &status );

	if( ret != 0 )
	{
		do
		{
			MWSDIO1_CardReset();  // Change form MSCA_SDIO1_Reset() to MWSDIO1_CardReset() '04.07.22

			if( (ret = MWSDIO1_ReadSectors( lba_sector, rdSecN,
										  lpbuf, &errorSecNo, &status )) == 0 ) break;

		} while( (--retry_cnt) != 0 );
	}

	return(ret);
}

int MSCA_SDIO1_WriteLBA(
					   unsigned long	lba_sector ,
					   unsigned char	wtSecN ,
					   unsigned char	*lpbuf )
{
	unsigned long	errorSecNo;
	unsigned char	status ;
	unsigned short	retry_cnt=MSCS_SDIO1_WriteRetryCount;
	int ret;

	if( wtSecN == 0 ) return(0);
	
	ret=MWSDIO1_WriteSectors( lba_sector, wtSecN, lpbuf, &errorSecNo, &status );

	if( ret != 0 )
	{
		do
		{
			MWSDIO1_CardReset();  // Change form MSCA_SDIO1_Reset() to MWSDIO1_CardReset() '04.07.22
			if( (ret = MWSDIO1_WriteSectors(lba_sector, wtSecN,
										   lpbuf, &errorSecNo, &status )) == 0 ) break;
		} while( (--retry_cnt) != 0 );
	}

	return(ret);
}

int MSCA_SDIO1_MediaIntHandler(void) {
	
	int ret;
	
	/* removal or insertion interrupt */
	ret = MWSDIO1_CardCheck() ;
	
	if(ret != D_SD1_ERR_NOCARD)
	{
		MSCA_SDIO1_Init(DriveNo1);
		MSCM_MediaChanged(DriveNo1);
		BF_Debug_Print_Information(("Chg SDIO1\n"));
	}
	else
	{
		BF_Debug_Print_Information(("No Chg SDIO1\n"));
	}
	
	return MSC_OK;

//	return MSCA_SDx_MediaIntHandler(FJ_MEDIA_SD1, FJ_MEDIA_SD0);
}

int MSCA_SDIO1_TaskTerm(void)
{
	return(MSC_OK);
}

/************************************************/
/*  Get Media Capacity                          */
/************************************************/
unsigned long MSCA_SDIO1_Capacity(void)
{
	return SDIO1_DevInfo->totalsector;
}

