/**
 * @file		msca_sdio0.c
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
#include "msca_sdio0.h"
#include "mwsdio.h"
#include "sd.h"
#include "mw_user_custom.h"

//#define USE_EVENT_TIME
#ifdef USE_EVENT_TIME
	#include "measure_time.h"
	unsigned long bnT,edT;
#endif


volatile T_SD0_MNG *SDIO0_DevInfo = &gSD0_dev_mng;

static unsigned char DriveNo0;


void sdio0_changeDrive(unsigned char driveno)
{
    MWSDIO0_ChangeRCA(SDIO0_DevInfo->rca);

}

int MSCA_SDIO0_Init(unsigned char driveno)
{
	int ret = D_SD0_ERR;

	DriveNo0 = driveno;
#if 0 /* NEED_2_CHANGE */
	IO_SDIO1.HostControl.bit.LEDX= 1;				// SDIO Power ON
#endif
	
//	sdio0_changeDrive(driveno);
//	
//	if( MWSDIO0_Get_SDCDX() == 1/*D_SD0_ANY_CARD*/)
//	{
//		if( MSCA_MDChkChange_Drive(DriveNo0) == MSC_CHANGED )
//		{
			ret = SD0_sdio_init();
//		}
//	}
//	else
//	{
//		MSCA_MDChangeClr_Drive(DriveNo0);
//	}
	
	if( ret == D_SD0_OK )
	{
		return(MSC_OK);
	}
	else
	{
		return(MSC_ERROR);
	}
}

int MSCA_SDIO0_Reset(void)
{
	return (MSC_OK);
}

int MSCA_SDIO0_GetDriveInfo(MSCA_IDI *lpidi)
{
	lpidi->bytepersector	= SDIO0_DevInfo->bytepersector;
	lpidi->current_cylinder	= SDIO0_DevInfo->cylinder ;
	lpidi->current_head		= SDIO0_DevInfo->head ;
	lpidi->current_sectrack	= SDIO0_DevInfo->sectrack ;
	lpidi->lba_sector		= SDIO0_DevInfo->totalsector;
	return( MSC_OK );
}

//  20051103 Modify for media change problem
int MSCA_SDIO0_CheckMedia( void)
{
    if( MWSDIO0_CardCheck() == 0 )
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

int MSCA_SDIO0_CheckWriteProtect( void )
{
	if( MWSDIO0_CardCheckWP() != D_SD0_OK )
	{	// Not Protected
		return(MSC_OK);
	}
	else
	{	// Protected
		return(MSC_ERROR);
	}
}

int MSCA_SDIO0_LockMedia( void )
{
	return(MSC_OK);
}

int MSCA_SDIO0_UnlockMedia( void )
{
	return(MSC_OK);
}

int MSCA_SDIO0_ReadLBA(
					  unsigned long	lba_sector ,
					  unsigned char	rdSecN ,
					  unsigned char	*lpbuf )
{
	unsigned long	errorSecNo;
	unsigned char	status ;
	unsigned short	retry_cnt=MSCS_SDIO0_ReadRetryCount;
	int	ret;

	if( rdSecN == 0 ) return(0);

	ret=MWSDIO0_ReadSectors( lba_sector, rdSecN, lpbuf, &errorSecNo, &status );

	if( ret != 0 )
	{
		do
		{
			MWSDIO0_CardReset();  // Change form MSCA_SDIO0_Reset() to MWSDIO0_CardReset() '04.07.22

			if( (ret = MWSDIO0_ReadSectors( lba_sector, rdSecN,
										  lpbuf, &errorSecNo, &status )) == 0 ) break;

		} while( (--retry_cnt) != 0 );
	}

	return(ret);
}

int MSCA_SDIO0_WriteLBA(
					   unsigned long	lba_sector ,
					   unsigned char	wtSecN ,
					   unsigned char	*lpbuf )
{
	unsigned long	errorSecNo;
	unsigned char	status ;
	unsigned short	retry_cnt=MSCS_SDIO0_WriteRetryCount;
	int ret;

	if( wtSecN == 0 ) return(0);
	
	ret=MWSDIO0_WriteSectors( lba_sector, wtSecN, lpbuf, &errorSecNo, &status );

	if( ret != 0 )
	{
		do
		{
			MWSDIO0_CardReset();  // Change form MSCA_SDIO0_Reset() to MWSDIO0_CardReset() '04.07.22
			if( (ret = MWSDIO0_WriteSectors(lba_sector, wtSecN,
										   lpbuf, &errorSecNo, &status )) == 0 ) break;
		} while( (--retry_cnt) != 0 );
	}

	return(ret);
}

int MSCA_SDIO0_MediaIntHandler(void) {
	
	int ret;
	
	/* removal or insertion interrupt */
	ret = MWSDIO0_CardCheck() ;
	
	if(ret != D_SD0_ERR_NOCARD)
	{
		MSCA_SDIO0_Init(DriveNo0);
		MSCM_MediaChanged(DriveNo0);
		BF_Debug_Print_Information(("Chg SDIO0\n"));
	}
	else
	{
		BF_Debug_Print_Information(("No Chg SDIO0\n"));
	}
	
	return MSC_OK;
	
//	return MSCA_SDx_MediaIntHandler(FJ_MEDIA_SD0, FJ_MEDIA_SD1);
}

int MSCA_SDIO0_TaskTerm(void)
{
	return(MSC_OK);
}

/************************************************/
/*  Get Media Capacity                          */
/************************************************/
unsigned long MSCA_SDIO0_Capacity(void)
{
	return SDIO0_DevInfo->totalsector;
}

