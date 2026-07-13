/**
 * @file		MSC_wrapos.c
 * @brief		REALOS System call wrap function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "msc_wrapos.h"
#include "os_user_custom.h"
#include "dd_arm.h"
#include "msclib.h"
#include "sdram_map_play_define.h"
#include "sdram_map_play.h"

#define USB_BUFFER_NO_REALOS

//#define MPFID_EXTBUF_HIGH_SIZE        (0x10000)
//#define MPFID_EXTBUF_HIGH_SIZE        (0x200)
#define MPFID_EXTBUF_HIGH_SIZE      (0x10000)

unsigned long W_MPFID_EXTBUF_HIGH_SIZE = MPFID_EXTBUF_HIGH_SIZE; /* less than MPFID_EXTBUF_HIGH_SIZE */

#ifdef USB_BUFFER_NO_REALOS
#define D_USB_BUF_NUMBER    (2)
//#define D_USB_BUF_NUMBER  (128)

#if 0 /* M-7M */
#if 1 // RAM sharing

// Definition for RAM sharing
typedef struct{
    char USB_BUFFER[MPFID_EXTBUF_HIGH_SIZE*D_USB_BUF_NUMBER];       // 64kByte*4 USB Buffer keep
} T_BF_MSC_WRAPOS_BUFF;

T_BF_MSC_WRAPOS_BUFF* gBF_Msc_Wrapos_Buff = (T_BF_MSC_WRAPOS_BUFF *)(SDRAM_DCASH_OR | SDRAM_ADR_PLAY_MSC_BUFFER_AREA);      // 64kByte*4 USB Buffer keep

//char USB_BUFFER[MPFID_EXTBUF_HIGH_SIZE];      // 64kByte USB Buffer keep
#else
//char USB_BUFFER[MPFID_EXTBUF_HIGH_SIZE*D_USB_BUF_NUMBER];     // 64kByte*4 USB Buffer keep
#endif
//#define D_USB_BUF_ADDRESS ( (char *)0x83F00000)
#endif /* M-7M */

typedef struct{
    unsigned char *buf_addr;
    char status;
}T_USB_MEM_CTRL;

#define UNLOCK_USB_BUF  (0)
#define LOCK_USB_BUF    (1)
void Init_USB_buffer(char *buf_top, unsigned long size, unsigned int max_number);
static int usb_buffer_get(void **buf, T_USB_MEM_CTRL *mem_ctrl, unsigned int max_buffer);
static int usb_buffer_release(void *buf, T_USB_MEM_CTRL *mem_ctrl, unsigned int max_buffer);

T_USB_MEM_CTRL  gUsb_buf_info[D_USB_BUF_NUMBER];
static unsigned long gUsb_Buffer_Num = D_USB_BUF_NUMBER;
#endif

void MSC_sta_tsk(void)
{
    OS_User_Sta_Tsk((OS_USER_ID)TID_MSCP_RCV, 0);   /*  MSCP_Rcv_Request USB Comnuication */
    OS_User_Sta_Tsk((OS_USER_ID)TID_MSCP_SND, 0);   /*  MSCP_Snd_Request Media Acccess */

}

void MSC_ter_tsk(void)
{
    OS_User_Ter_Tsk((OS_USER_ID)TID_MSCP_RCV);      /*  MSCP_Rcv_Request */
    OS_User_Ter_Tsk((OS_USER_ID)TID_MSCP_SND);      /*  MSCP_Snd_Request */
}

void MSC_ext_tsk(void)
{
    OS_User_Ext_Tsk();
}

MSC_ER MSC_ref_tsk(T_MSC_RTSK *rtsk, MSC_ID tid)
{
    MSC_ER ret= -1;
    
    switch( tid )
    {
    case W_TID_MSCP_RCV:
        ret = (MSC_ER)OS_User_Ref_Tsk((OS_USER_ID)TID_MSCP_RCV, (T_OS_USER_RTSK *)rtsk);
        break;
    case W_TID_MSCP_SND:
        ret = (MSC_ER)OS_User_Ref_Tsk((OS_USER_ID)TID_MSCP_SND, (T_OS_USER_RTSK *)rtsk);
        break;
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}


/**** Fixed memory pool ****/
MSC_ER MSC_get_blf( MSC_VP *blf, MSC_ID mpfid)
{
    MSC_ER ret;
    
    if( mpfid == W_MPFID_EXTBUF_HIGH )
    {
#ifdef USB_BUFFER_NO_REALOS
        ret = (MSC_ER)usb_buffer_get( (MSC_VP *)(blf), gUsb_buf_info, gUsb_Buffer_Num );
#else
        ret = (MSC_ER)OS_User_Get_Mpf( (OS_USER_ID)MPFID_DPS_BIGBUF, (OS_USER_VP *)(blf) );
#endif
    }
    else
    {
        ret = (MSC_ER)OS_User_Get_Mpf( (OS_USER_ID)MPFID_DPS_MSGBUF, (OS_USER_VP *)(blf) );
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}

MSC_ER MSC_rel_blf(MSC_ID mpfid, MSC_VP blf)
{
    MSC_ER ret;
    
    if( mpfid == W_MPFID_EXTBUF_HIGH )
    {
#ifdef USB_BUFFER_NO_REALOS
        ret = (MSC_ER)usb_buffer_release( blf, gUsb_buf_info, gUsb_Buffer_Num );
#else
        ret = (MSC_ER)OS_User_Rel_Mpf( (OS_USER_ID)MPFID_DPS_BIGBUF, blf );
#endif
    }
    else 
    {
        ret = (MSC_ER)OS_User_Rel_Mpf( (OS_USER_ID)MPFID_DPS_MSGBUF, blf );
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}

/**** Mail Box ****/
MSC_ER MSC_snd_msg(MSC_ID mbxid,T_MSC_MSG *pk_msg)
{
    MSC_ER ret = -999;
    
    switch(mbxid)
    {
    case W_MID_MSCP_RCV_REQUEST:
        ret = (MSC_ER)OS_User_Snd_Mbx( (OS_USER_ID)MID_MSCP_RCV_REQUEST , (T_OS_USER_MSG *)pk_msg );
        break;
    case W_MID_MSCP_RCV_RESPONSE:
        ret = (MSC_ER)OS_User_Snd_Mbx( (OS_USER_ID)MID_MSCP_RCV_RESPONSE , (T_OS_USER_MSG *)pk_msg );
        break;
    case W_MID_MSCP_SND_REQUEST:
        ret = (MSC_ER)OS_User_Snd_Mbx( (OS_USER_ID)MID_MSCP_SND_REQUEST , (T_OS_USER_MSG *)pk_msg );
        break;
    case W_MID_MSCP_SND_RESPONSE:
        ret = (MSC_ER)OS_User_Snd_Mbx( (OS_USER_ID)MID_MSCP_SND_RESPONSE , (T_OS_USER_MSG *)pk_msg );
        break;
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}

MSC_ER MSC_rcv_msg(T_MSC_MSG **pk_msg, MSC_ID mbxid)
{
    MSC_ER ret = -999;
    switch(mbxid)
    {
    case W_MID_MSCP_RCV_REQUEST:
        ret = (MSC_ER)OS_User_Rcv_Mbx( (OS_USER_ID)MID_MSCP_RCV_REQUEST, (T_OS_USER_MSG **)pk_msg );
        break;
    case W_MID_MSCP_RCV_RESPONSE:
        ret = (MSC_ER)OS_User_Rcv_Mbx( (OS_USER_ID)MID_MSCP_RCV_RESPONSE, (T_OS_USER_MSG **)pk_msg );
        break;
    case W_MID_MSCP_SND_REQUEST:
        ret = (MSC_ER)OS_User_Rcv_Mbx( (OS_USER_ID)MID_MSCP_SND_REQUEST, (T_OS_USER_MSG **)pk_msg );
        break;
    case W_MID_MSCP_SND_RESPONSE:
        ret = (MSC_ER)OS_User_Rcv_Mbx( (OS_USER_ID)MID_MSCP_SND_RESPONSE, (T_OS_USER_MSG **)pk_msg );
        break;
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}

MSC_ER MSC_prcv_msg(T_MSC_MSG **pk_msg, MSC_ID mbxid)
{
    MSC_ER ret = -999;
    
    switch(mbxid)
    {
    case W_MID_MSCP_RCV_REQUEST:
        ret = (MSC_ER)OS_User_Prcv_Mbx( (OS_USER_ID)MID_MSCP_RCV_REQUEST, (T_OS_USER_MSG **)pk_msg );
        break;
    case W_MID_MSCP_RCV_RESPONSE:
        ret = (MSC_ER)OS_User_Prcv_Mbx( (OS_USER_ID)MID_MSCP_RCV_RESPONSE, (T_OS_USER_MSG **)pk_msg );
        break;
    case W_MID_MSCP_SND_REQUEST:
        ret = (MSC_ER)OS_User_Prcv_Mbx( (OS_USER_ID)MID_MSCP_SND_REQUEST, (T_OS_USER_MSG **)pk_msg );
        break;
    case W_MID_MSCP_SND_RESPONSE:
        ret = (MSC_ER)OS_User_Prcv_Mbx( (OS_USER_ID)MID_MSCP_SND_RESPONSE, (T_OS_USER_MSG **)pk_msg );
        break;
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}

MSC_ER MSC_ref_mbx(T_MSC_RMBX *pref, MSC_ID mbxid)
{
    MSC_ER ret = -999;
    switch( mbxid )
    {
    case W_MID_MSCP_RCV_REQUEST:
        ret = (MSC_ER)OS_User_Ref_Mbx((OS_USER_ID)MID_MSCP_RCV_REQUEST, (T_OS_USER_RMBX *)pref);
        break;
    case W_MID_MSCP_RCV_RESPONSE:
        ret = (MSC_ER)OS_User_Ref_Mbx((OS_USER_ID)MID_MSCP_RCV_RESPONSE, (T_OS_USER_RMBX *)pref);
        break;
    case W_MID_MSCP_SND_REQUEST:
        ret = (MSC_ER)OS_User_Ref_Mbx((OS_USER_ID)MID_MSCP_SND_REQUEST, (T_OS_USER_RMBX *)pref);
        break;
    case W_MID_MSCP_SND_RESPONSE:
        ret = (MSC_ER)OS_User_Ref_Mbx((OS_USER_ID)MID_MSCP_SND_RESPONSE, (T_OS_USER_RMBX *)pref);
        break;
    }
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}


/**** Interrupt level mask ****/
MSC_ER MSC_chg_ilm(MSC_UINT ilmask)
{
    return MSC_OK;
}
MSC_ER MSC_ref_ilm(MSC_UINT *ilmask)
{
    return MSC_OK;
}
/**** Sleep ****/
MSC_ER MSC_tslp_tsk(MSC_TMO tout)
{
    MSC_ER ret;
    ret = (MSC_ER)OS_User_Tslp_Tsk((OS_USER_TMO)tout);
    
    if (ret != D_OS_USER_E_OK) {
        ret = MSC_ERROR;  /* time out */
    } else {
        ret = MSC_OK;
    }
    
    return ret;
}

#ifdef USB_BUFFER_NO_REALOS

//#pragma arm section code="BASEFW_COMM_CODE"

void Init_USB_buffer(char *buf_top, unsigned long size, unsigned int max_number){
    int i;

    W_MPFID_EXTBUF_HIGH_SIZE  = size;
    gUsb_Buffer_Num  = max_number;

    for( i = 0 ; i < max_number; i++){
        gUsb_buf_info[ i ].status = UNLOCK_USB_BUF;
        gUsb_buf_info[ i ].buf_addr =(unsigned char *)( (unsigned long)buf_top + (size * i ));
    }
}


int usb_buffer_get(void **buf, T_USB_MEM_CTRL *mem_ctrl, unsigned int max_buffer)
{
    int i;
    unsigned short sr;
    for( i = 0 ; i < max_buffer; i++){
        if( (mem_ctrl + i)->status == UNLOCK_USB_BUF){
            sr = Dd_ARM_DI();
            (mem_ctrl + i)->status = LOCK_USB_BUF;
            *buf = (void *)(mem_ctrl + i)->buf_addr;
            Dd_ARM_EI(sr);
            return 0; 
        }
    }
    return E_NOMEM_USB;
}

int usb_buffer_release(void *buf, T_USB_MEM_CTRL *mem_ctrl, unsigned int max_buffer)
{
    int i;
    unsigned short sr;
    for( i = 0 ; i < max_buffer; i++){
        if( buf == (void *)(mem_ctrl + i)->buf_addr){
            sr = Dd_ARM_DI();
            (mem_ctrl + i)->status = UNLOCK_USB_BUF;
            Dd_ARM_EI(sr);
            return 0; 
        }
    }
    return -1;
}

//#pragma arm section code="BASEFW_COMM_CODE"

void USB_Buffer_Init(void){
    Init_USB_buffer( ( char *)(SDRAM_ADR_PLAY_MSC_BUFFER_AREA), W_MPFID_EXTBUF_HIGH_SIZE , D_USB_BUF_NUMBER);
}

#else
//#pragma arm section code="BASEFW_COMM_CODE"
void USB_Buffer_Init(void){}
#endif
