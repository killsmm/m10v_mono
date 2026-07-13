/**
 * @file		mwusb_driver4.c
 * @brief		USB-Driver for F_USB20HDC_LSP Macro
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USB_Section.h"  /* pragma setting */


#include <string.h>
#include "os_user_custom.h"
#include "mwusb_driver4.h"
#include "mwusb_custom4.h"
#include "mwusb_dma.h"
#include "USBHDC_StdDef.h"
#include "USBHDC_Dev_Api.h"
#include "USBHDC_Dev_Global.h"

//#define USB_DRIVER_PERFORMANCE_EVALUATION
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
#include "measure_time.h"
#endif
/************************************************/
/************************************************/
#define USB_WAIT_CYCLE (200)

/*  DMA Use Flag  */
#ifdef NON_USB_DMA_IN
static unsigned char IN_DMA_Mode = MWUSB_DMA_NOUSE; /* Not Use DMA */
#else
static unsigned char IN_DMA_Mode = MWUSB_DMA_USE;   /* Use DMA */
#endif

#ifdef NON_USB_DMA_OUT
static unsigned char OUT_DMA_Mode = MWUSB_DMA_NOUSE;    /* Not Use DMA */
#else
static unsigned char OUT_DMA_Mode = MWUSB_DMA_USE;      /* Use DMA */
#endif

/* add 2011.4.20 tomida */
/* DMA Setting Status */
#define DMA_INIT_STAT   0
#define DMA_USE1_CHNL   10
#define DMA_USE2_CHNL   20
#define DMA_DEFA_CHNL   30

static  int mwusb_dma_read_stat     = DMA_INIT_STAT;
static  int mwusb_dma_write_stat    = DMA_INIT_STAT;

/* add 2011.02.22 buffer for dummy-read */
unsigned char DmyBuf[BULK_PACKET_SIZE_HI] __attribute__((aligned(4))); /* dummy buffer */

/* 20100414 by tomida */
static unsigned char Configuration_Mode_Status = MWUSB_CONFIG_MANUAL; /* default manual */

//unsigned char SDRAM_PAR[2048] __attribute__((aligned(4)));        // 2kB for sdram.par
//unsigned char USB_DAT[8*1024*1024] __attribute__((aligned(4)));    // 8MB for USB.DAT

static unsigned char InterID = 0;
/* Callback function Entry ID */
#define BULK_ENDP1_ID00      0
#define BULK_ENDP2_ID01      1
#define INTR_ENDP3_ID02      2
#define VEND_ENDP5_ID04      4
#define VEND_ENDP6_ID05      5
#define SET_CONFIG_ID17     17
#define SETUP_CLAS_ID22     22
#define TRAN_I_CLS_ID23     23
#define TRAN_O_CLS_ID24     24
#define SETUP_VEND_ID25     25
#define TRAN_I_VND_ID26     26
#define TRAN_O_VND_ID27     27
#define BUSRST_STR_ID28     28
#define BUSRST_END_ID29     29
#define SUSPND_STR_ID30     30
#define SUSPND_END_ID31     31
#define PHY_HUNGUP_ID32     32
#define END_DMACH1_ID34     34      /// for DMA add k.maeda
#define END_DMACH2_ID35     35      /// for DMA add k.maeda
#define SOF_GETPAK_ID42     42
#define STALL_INTR_ID44     44      /// for Stall Interrupt 20111115
#define EPBF_EMPTY_ID46     46
#define BLK_EP2_FN_ID51     51
#define VND_EP5_FN_ID54     54
#define VBUS_RISED_ID72     72
#define VBUS_FALLD_ID73     73

/* device status */
#define DEVICE_STATE_ATTACHED   0
#define DEVICE_STATE_POWERED    1
#define DEVICE_STATE_DEFAULT    2
#define DEVICE_STATE_ADDRESS    3
#define DEVICE_STATE_CONFIGURED 4
#define DEVICE_STATE_SUSPEND    5
#define DEVICE_STATE_UNKOUWN   -1

/* global result return flag pattern add 2011.01.06 tomida */
static OS_USER_FLGPTN global_out_ptn;
static OS_USER_FLGPTN global_in_ptn;

/* BusReset control status(doing/none) */
#define BUS_RESET_DOING    1
#define BUS_RESET_NONE     0
static unsigned char Control_BusReset_Status = BUS_RESET_NONE;

/* never referenced in M-7M */
/* Vbus plug control status(rise/fall) */
/* static unsigned char Control_Vbus_Status = MWUSB_PLUG_RELEASE;*/

/* Function status */
static unsigned char Control_Func_Status = MWUSB_FUNC_DISABLE;

/* BusSpeed setting(Hi-speed/Full-Speed) */
static int  Control_BusSpeed_setting = -1;  /* default(-1) */

/* driver initialize flag */
int mwusb_InitializeDriver = MWUSB_NG;

/* USB hardmacro Power On flag */
#ifdef USE_LOW_POWER
static int mwusb_PowerOn = MWUSB_NG;
#endif /* USE_LOW_POWER */

/* if Data Size > DMA_USE_LIMIT_LENGTH then Use Demand DMA */
#define DMA_USE_LIMIT_LENGTH    (512)

/* Configuration Descriptor Packet */
static DESCRIPTOR_PACKET  *ConfigDescp=NULL;

/* The following global variables are never used */
#if 0 /* M-7M */
/* Device Descriptor Packet  */
static DESCRIPTOR_PACKET  *DeviceDescp=NULL;
/* String Descriptor Packet */
static DESCRIPTOR_PACKET  *LANGID=NULL;
static DESCRIPTOR_PACKET  *Index1=NULL;
static DESCRIPTOR_PACKET  *Index2=NULL;
static DESCRIPTOR_PACKET  *Index3=NULL;
/* OtherConfiguration Descriptor Packet */
static DESCRIPTOR_PACKET  *OtherConfigDescp=NULL;
/* OtherConfiguration Descriptor Packet */
static DESCRIPTOR_PACKET  *DeviceQualDescp=NULL;
/* Report Descriptor Packet */
static DESCRIPTOR_PACKET  *ReportDescp=NULL;
#endif /* M-7M */

/* Max_Configuration_Number */
static unsigned char       Max_Config_Number = 0;

/* Transfer Phase Status */
#define USB_IDLE                (0)
#define USB_SETUP_STAGE         (1)
#define USB_DATAIN_STAGE        (2)
#define USB_DATAOUT_STAGE       (3)
#define USB_STATUSIN_STAGE      (4)
#define USB_STATUSOUT_STAGE     (5)
#define USB_SEND                (6)
#define USB_LENGTHB             (7)
#define USB_LENGTHE             (8)
#define USB_RECV                (9)

/* buffer plane number */
#define USB_EP_SINGLE_BUFFER    (0)
#define USB_EP_DOUBLE_BUFFER    (1)
#define USB_EP_TRIPLE_BUFFER    (2)
#define USB_EP_QUADLE_BUFFER    (3)

/* Out-Transaction Management Data (Host PC -> Device) */
static OUT_TRAN_DATA    CntlOut_Trans;  /* for EP0o Data ptr */
static OUT_TRAN_DATA    BulkOut_Trans;
static OUT_TRAN_DATA    VendOut_Trans;
static OUT_TRAN_DATA    IsocOut_Trans;

/* IN-Transaction Management Data  (Device -> Host PC) */
static IN_TRAN_DATA     CntlIn_Trans;   /* for EP0i Data    */
static IN_TRAN_DATA     BulkIn_Trans;
static IN_TRAN_DATA     VendIn_Trans;
static IN_TRAN_DATA     IntrIn_Trans;
static IN_TRAN_DATA     IsocIn_Trans;

static unsigned char    CntlTrans_phase;  /* for Cntl phase  */

/* IN-Transaction Begin Flag */
//static unsigned char  CntlIn_begin = 1; /* for Cntl phase  */
//static unsigned char    BulkIn_begin = 1; /* for Bulk phase  */
//static unsigned char    IntrIn_begin = 1; /* for Intr phase  */


/* Null Packet Send Request Flag */
static unsigned char fReqCntl_Null = 0;
static unsigned char fReqBulk_Null = 0;
static unsigned char fReqIntr_Null = 0;
/*  Null Packet Auto Send Flag */
static unsigned char fFIFOAuto_Null = 0;
static unsigned char fBulkAuto_Null = 0;
static unsigned char fIntrAuto_Null = 0;

/* Buffer of Contorol Transfer Packets */
static unsigned char OutPacket_Buffer[512+2]  __attribute__((aligned(4)));
static unsigned char InPacket_Buffer[512+2]  __attribute__((aligned(4)));

/* (Bulk-OUT) 1Packet Read Data Lengthead */
static short BulkOut_RecvLength = -1;
/* Bulk OUT Total Required Receive Data Size */
static long BulkOut_TotalLength = -1;

/* Save DeviceRequset Packets  **/
static DEVICE_REQUEST DeviceRequest ;  /* 8 byte data */

/* USB Connecting Speed request flg */
static unsigned long ulReqSpeed = MWUSB_HISPEED;

#if 0 /* M-7M */
/*  Bulk IN 2packet Flag for BYPASS Mode  */
static unsigned char BULKIN_2pkt=0;
#endif /* M-7M */

/* For BYPASS Protocol mode */
//static unsigned char *BypsReadBuf_TopPtr=NULL;    /* Pointer to Read buffer for BYPASS mode       */
static unsigned char BYPS_Mode = 0;             /* Protocol Mode  0:normal (default) , 1:Bypass */
//static unsigned char BYPS_Phase = 0;          /* Read phase flag                              */
//static unsigned char BYPS_Phase2 = 0;         /* Write phase flag                             */
//static unsigned char BYPS_ContWrite_flg = 0;  /* BYPASS Continurous write flag */
static long BypsReadSize=0;

#ifdef USE_PTP_CLASS
static unsigned char byClass0x67[4] = { 0x04, 0x00, 0x01, 0x20 };
const DESCRIPTOR_PACKET Class0x67[] = {
        { (unsigned char *) &byClass0x67 },
        { (unsigned char *) NULL}    /* end of data */
};
#endif 

#define MWUSB_HS_CONFIG_MAX      CONFIG_MAX_NUM
#define TYPE_NULL                -1

static MWUSB_DESCRIPTOR_ADDRESS FS_descriptor_cntl[MWUSB_HS_CONFIG_MAX] __attribute__((aligned(4)));
static MWUSB_DESCRIPTOR_ADDRESS HS_descriptor_cntl[MWUSB_HS_CONFIG_MAX] __attribute__((aligned(4)));
static MWUSB_DESCRIPTOR_ADDRESS FS_bps_descriptor_cntl[MWUSB_HS_CONFIG_MAX] __attribute__((aligned(4)));
static MWUSB_DESCRIPTOR_ADDRESS HS_bps_descriptor_cntl[MWUSB_HS_CONFIG_MAX] __attribute__((aligned(4)));


/******************************/
/** Local Function Prototype **/
/******************************/
/** local utility Functions **/
static int mwusb_initialize(unsigned char conf_mode, unsigned char boot_mode);
static long mwusb_CheckConnect(unsigned char chatteringFlag);
static void usb_descriptor_cntl_init(void);
static short mwusb_FifoNo_2_EpNo(unsigned char fifo_no);
static void mwusb_SetEP_buffer_info(void);
static void mwusb_SetEP_buffer_num(void);
static void mwusb_intdisconnect(unsigned char mode);

/** local EP control Functions  **/
static long mwusb_EPStalled( unsigned char EpNo );
static long mwusb_EPStallClr( unsigned char EpNo );
static long mwusb_EPStallCancel( unsigned char EpNo );
static long mwusb_EPInit(unsigned char EpNo);
static long mwusb_SendNullPacket(unsigned char endp);

/** local IO Functions **/
static short ControlFIFORead(void);
static unsigned long BulkFIFORead(OUT_TRAN_DATA * Out_Trans);
static short Bulk_Read1Packet(unsigned char *buf, OUT_TRAN_DATA * Out_Trans);
/* static long BypassRecvLength(OUT_TRAN_DATA * Out_Trans); */ /* M-7M */
static long Bulk_Read(unsigned char *buf, unsigned long Size, unsigned char nfifo, OUT_TRAN_DATA * Out_Trans);
/* static long BypassSendLength(unsigned long Size, IN_TRAN_DATA * In_Trans); */ /* M-7M */
static long Bulk_Write(unsigned char *buf, unsigned long Size, unsigned char nfifo, IN_TRAN_DATA * In_Trans);

/* Endpoint Ready Callback */
void InterContReadReadyCB(void);
void InterContWriteReadyCB(void);
void InterBulkReadReadyCB(void);
void InterBulkReadWaitCB(void);
void InterBulkWriteReadyCB(void);
void InterIntrWriteReadyCB(void);
void InterVendReadReadyCB(void);
void InterVendReadWaitCB(void);
void InterVendWriteReadyCB(void);
/* void InterIsocReadReadyCB(void); */ /* M-7M */
/* void InterIsocWriteReadyCB(void); */ /* M-7M */
/* void InterControlStallCB(void) */ /* M-7M */

void IDint_EP0_Write(void);
void IDint_EP0_Read(void);
void IDint_Bulk_Write(IN_TRAN_DATA * In_Trans);
void IDint_Bulk_Read(OUT_TRAN_DATA * Out_Trans);
void IDint_Intr_Write(IN_TRAN_DATA * In_Trans);

/* Request Callback */
void IDint_SetConfiguration(void);
void IDint_BusReset_START(void);
void IDint_BusReset_END(void);
void IDint_Phy_Hungup(void);
void IDint_Suspend_START(void);
void IDint_Suspend_END(void);
void IDint_Setup_CLSVND_Request(void);
void IDint_Sof_Trans(void);

/* 2011.02.16 add tomida */
/* add control Read,Write function control */
typedef void (* MWUSB_CALLBACK)(void);
static MWUSB_CALLBACK idint_ep0_read = NULL;


/************************/
/** USB driver UTILITY **/
/************************/
/********************************************************************/
/*  <Function>  static void usb_descriptor_cntl_init                */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Descriptor/Endpoint-transfer control initialize   */
/********************************************************************/
static void usb_descriptor_cntl_init(void)
{
    /* Descriptor control */
    memset((void*)FS_descriptor_cntl,0,(size_t)(sizeof(MWUSB_DESCRIPTOR_ADDRESS)*MWUSB_HS_CONFIG_MAX));
    memset((void*)HS_descriptor_cntl,0,(size_t)(sizeof(MWUSB_DESCRIPTOR_ADDRESS)*MWUSB_HS_CONFIG_MAX));
    memset((void*)FS_bps_descriptor_cntl,0,(size_t)(sizeof(MWUSB_DESCRIPTOR_ADDRESS)*MWUSB_HS_CONFIG_MAX));
    memset((void*)HS_bps_descriptor_cntl,0,(size_t)(sizeof(MWUSB_DESCRIPTOR_ADDRESS)*MWUSB_HS_CONFIG_MAX));
    
    /* endpoint transfer control */
    CntlOut_Trans.sizeOfpacket  = CONT_PACKET_SIZE;
    CntlOut_Trans.sizeOfdata    = 0;
    CntlOut_Trans.phase         = USB_IDLE;
    CntlOut_Trans.epno          = 0;
    CntlOut_Trans.stat          = MWUSB_EP_USE;

    CntlIn_Trans.sizeOfpacket   = CONT_PACKET_SIZE;
    CntlIn_Trans.sizeOfdata     = 0;
    CntlIn_Trans.phase          = USB_IDLE;
    CntlIn_Trans.epno           = 0;
    CntlIn_Trans.stat           = MWUSB_EP_USE;

    BulkIn_Trans.sizeOfdata     = 0;
    BulkIn_Trans.phase          = USB_IDLE;
    BulkIn_Trans.stat           = MWUSB_EP_NOUSE;
    
    BulkOut_Trans.sizeOfdata    = 0;
    BulkOut_Trans.phase         = USB_IDLE;
    BulkOut_Trans.stat          = MWUSB_EP_NOUSE;

    VendIn_Trans.sizeOfdata     = 0;
    VendIn_Trans.phase          = USB_IDLE;
    VendIn_Trans.stat           = MWUSB_EP_NOUSE;
    
    VendOut_Trans.sizeOfdata    = 0;
    VendOut_Trans.phase         = USB_IDLE;
    VendOut_Trans.stat          = MWUSB_EP_NOUSE;

    IntrIn_Trans.sizeOfdata     = 0;
    IntrIn_Trans.phase          = USB_IDLE;
    IntrIn_Trans.stat           = MWUSB_EP_NOUSE;

    IsocIn_Trans.sizeOfdata     = 0;
    IsocIn_Trans.phase          = USB_IDLE;
    IsocIn_Trans.stat           = MWUSB_EP_NOUSE;

    IsocOut_Trans.sizeOfdata    = 0;
    IsocOut_Trans.phase         = USB_IDLE;
    IsocOut_Trans.stat          = MWUSB_EP_NOUSE;

    /* Reset Max Configuration Number */
    Max_Config_Number        = 0;
    
    return;
}

/********************************************************************/
/*  <Function>  static short mwusb_FifoNo_2_EpNo                    */
/*  <Arguments> unsigned char FIFO                                  */
/*                  Index of Endpoint                               */
/*  <Return  >  Endpoint No                                         */
/*  <Description> Conversion from Index of Endpoint to Endpoint No  */
/********************************************************************/
static short mwusb_FifoNo_2_EpNo(unsigned char fifo_no)
{
    short EpNo = MWUSB_ERR_BADEP;
    
    if(fifo_no == CNTL_READ_FIFO)
    {
        EpNo = (short)CntlOut_Trans.epno;
    }
    else if(fifo_no == CNTL_WRITE_FIFO)
    {
        EpNo = (short)CntlIn_Trans.epno;
    }
    else if((BulkOut_Trans.stat == MWUSB_EP_USE) && (fifo_no == BULK_READ_FIFO))
    {
        EpNo = (short)BulkOut_Trans.epno;
    }
    else if((VendOut_Trans.stat == MWUSB_EP_USE) && (fifo_no == VEND_READ_FIFO))
    {
        EpNo = (short)VendOut_Trans.epno;
    }
    else if((IsocOut_Trans.stat == MWUSB_EP_USE) && (fifo_no == ISOC_READ_FIFO))
    {
        EpNo = (short)IsocOut_Trans.epno;
    }
    else if((BulkIn_Trans.stat == MWUSB_EP_USE) && (fifo_no == BULK_WRITE_FIFO))
    {
        EpNo = (short)BulkIn_Trans.epno;
    }
    else if((VendIn_Trans.stat == MWUSB_EP_USE) && (fifo_no == VEND_WRITE_FIFO))
    {
        EpNo = (short)VendIn_Trans.epno;
    }
    else if((IntrIn_Trans.stat == MWUSB_EP_USE) && (fifo_no == INTR_WRITE_FIFO))
    {
        EpNo = (short)IntrIn_Trans.epno;
    }
    else if((IsocIn_Trans.stat == MWUSB_EP_USE) && (fifo_no == ISOC_WRITE_FIFO))
    {
        EpNo = (short)IsocIn_Trans.epno;
    }
    
    return( EpNo );
}

/** function prototype **/
static short BulkOutRcvSize(OUT_TRAN_DATA * Out_Trans);

static long DEND1_flg=-1;   // Realos Bug?
static long DEND2_flg=-1;   // Realos Bug?
static long DEND3_flg=-1;   // Realos Bug?

/***********************/
/* OS wrapped function */
/***********************/
#ifdef USB_NO_REALOS
#define FID_USB     (0)
#define TWF_CLR     (0x001)
#define TWF_ORW     (0x010)
#define TWF_ANDW    (0x100)

static volatile unsigned long USBFLG=0x00000000;

void usb_set_flg(unsigned long flg , unsigned long ptn )
{
    USBFLG |= ptn;
}

void usb_clr_flg(unsigned long flg , unsigned long ptn )
{
    USBFLG &= ptn;
}

//edit argument 4 unsigned char -> unsigned int tomida 20101206
//void usb_wai_flg(unsigned long *flg_ptn,unsigned long flg,unsigned long ptn,unsigned char wait)
void usb_wai_flg(unsigned long *flg_ptn,unsigned long flg,unsigned long ptn,unsigned int wait)
{
    unsigned long wk;
    
    if ((wait & TWF_ORW) == TWF_ORW)        // OR Wait
    {
        while( 1 )
        {
            wk = USBFLG;
            if( (wk & ptn) != 0 )
                break;
        }
    } 
    else if ((wait & TWF_ANDW) == TWF_ANDW)     // AND Wait
    {
        while( 1 )
        {
            wk = USBFLG;
            if( (wk & ptn) == ptn )
                break;
        }
    }
    if( flg_ptn != NULL )
        *flg_ptn = wk;
    if ((wait & TWF_CLR)==TWF_CLR)
        USBFLG &= ~ptn;

}
#endif


/********************************************************************/
/*  <Function>  static void In_Interrupt_UnMask                     */
/*  <Arguments> epno    Endpoint Number                             */
/*                  0:control                                       */
/*                  1:Bulk                                          */
/*                  3:Interrupt                                     */
/*  <Return  >  none                                                */
/*  <Description> Interrupt Ready UnMask                            */
/********************************************************************/
static void In_Interrupt_UnMask(unsigned char epno)
{
    if( epno == CntlIn_Trans.epno )
    {
        IDint_EP0_Write();
    }
    else if( (epno == BulkIn_Trans.epno) && (BulkIn_Trans.stat == MWUSB_EP_USE) )
    {
        InterID = BULK_ENDP1_ID00; /* Interrupt ID */
        IDint_Bulk_Write(&BulkIn_Trans);
    }
    else if( (epno == VendIn_Trans.epno) && (VendIn_Trans.stat == MWUSB_EP_USE) )
    {
        InterID = VEND_ENDP5_ID04; /* Interrupt ID */
        IDint_Bulk_Write(&VendIn_Trans);
    }
    else if( (epno == IntrIn_Trans.epno) && (IntrIn_Trans.stat == MWUSB_EP_USE) )
    {
        InterID = INTR_ENDP3_ID02; /* Interrupt ID */
        IDint_Intr_Write(&IntrIn_Trans);
    }
    return;
}



/********************************************************************/
/*  <Function>  static void Out_Interrupt_UnMask                    */
/*  <Arguments> epno    Endpoint Number                             */
/*                  0:control                                       */
/*                  2:control                                       */
/*  <Return  >  none                                                */
/*  <Description> Interrupt Ready UnMask                            */
/********************************************************************/
static void Out_Interrupt_UnMask(unsigned char epno)
{
    if( epno == CntlOut_Trans.epno )
    {
        //IDint_EP0_Read();
        idint_ep0_read = IDint_EP0_Read; /* set interrupt function */
    }
    else if( (epno == BulkOut_Trans.epno) && (BulkOut_Trans.stat == MWUSB_EP_USE) )
    {
        InterID = BULK_ENDP2_ID01; /* Interrupt ID */
        IDint_Bulk_Read(&BulkOut_Trans);
    }
    else if( (epno == VendOut_Trans.epno) && (VendOut_Trans.stat == MWUSB_EP_USE) )
    {
        InterID = VEND_ENDP6_ID05; /* Interrupt ID */
        IDint_Bulk_Read(&VendOut_Trans);
    }
    return;
}



/********************************************************************/
/*  <Function>  void InterContReadReadyCB                           */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Control Read Ready             */
/********************************************************************/
void InterContReadReadyCB(void)
{
    /* Control EndPoint Ready Interrupt Flag */
#if 0
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_ContReadRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_ContReadRdy);
#endif
#else
    if( idint_ep0_read != NULL )
    {
        idint_ep0_read();
        idint_ep0_read = NULL;
    }
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterContWriteReadyCB                          */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Control Write Ready            */
/********************************************************************/
void InterContWriteReadyCB(void)
{
#if 0 /* M-7M */
    /* Control EndPoint Write Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_ContWriteRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_ContWriteRdy);
#endif
#endif /* M-7M */

    /* Execution : data in stage or status out stage */
    /* In_Interrupt_UnMask(CntlIn_Trans.epno); */

    return;
}



/********************************************************************/
/*  <Function>  void InterBulkReadReadyCB                           */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Bulk Read Ready                */
/********************************************************************/
void InterBulkReadReadyCB(void)
{
#if 0
    printf(" ID01 BulkReadReady.\n\r");
    OS_User_Tslp_Tsk(1);
#endif
    /* Bulk EndPoint Read Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_BulkReadRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_BulkReadRdy);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterBulkReadWaitCB                            */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Bulk Read Ready Wait           */
/********************************************************************/
void InterBulkReadWaitCB(void)
{
#if 0
    printf(" ID51 BulkReadWait.\n\r");
    OS_User_Tslp_Tsk(1);
#endif
    /* Bulk EndPoint Read Interrupt Wait Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_BulkReadWai);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_BulkReadWai);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterBulkWriteReadyCB                          */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Bulk Write Ready               */
/********************************************************************/
void InterBulkWriteReadyCB(void)
{
#if 0
    printf(" ID01 BulkWriteReady.\n\r");
    OS_User_Tslp_Tsk(1);
#endif
    /* Bulk EndPoint Write Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_BulkWriteRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_BulkWriteRdy);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterVendReadReadyCB                           */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Vend Read Ready                */
/********************************************************************/
void InterVendReadReadyCB(void)
{
#if 0
    printf(" ID04 VendReadReady.\n\r");
    OS_User_Tslp_Tsk(1);
#endif
    /* Vend EndPoint Read Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_VendReadRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_VendReadRdy);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterVendReadWaitCB                            */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Vend Read Ready Wait           */
/********************************************************************/
void InterVendReadWaitCB(void)
{
#if 0
    printf(" ID54 VendReadWait.\n\r");
    OS_User_Tslp_Tsk(1);
#endif
    /* Vend EndPoint Read Interrupt Wait Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_VendReadWai);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_VendReadWai);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterVendWriteReadyCB                          */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Vend Write Ready               */
/********************************************************************/
void InterVendWriteReadyCB(void)
{
#if 0
    printf(" ID03 VendWriteReady.\n\r");
    OS_User_Tslp_Tsk(1);
#endif
    /* Bulk EndPoint Write Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_VendWriteRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_VendWriteRdy);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterIntrWriteReadyCB                          */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Interrupt Write Ready          */
/********************************************************************/
void InterIntrWriteReadyCB(void)
{
    /* Interrupt EndPoint Write Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_IntrWriteRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_IntrWriteRdy);
#endif

    return;
}



/********************************************************************/
/*  <Function>  void InterIsocReadReadyCB                           */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Isochrnous Read Ready          */
/********************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void InterIsocReadReadyCB(void)
{
    /* Isochrounus EndPoint Read Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_IsocReadRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_IsocReadRdy);
#endif

    return;
}
#endif


/********************************************************************/
/*  <Function>  void InterIsocWriteReadyCB                          */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Isochrnous Write Ready         */
/********************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void InterIsocWriteReadyCB(void)
{
    /* Isochrnous EndPoint Write Interrupt Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_IsocWriteRdy);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_IsocWriteRdy);
#endif

    return;
}
#endif


/********************************************************************/
/*  <Function>  void IDint_DMA_1                                    */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <History >  k.maeda 2011.01.07                                  */
/*  <Description> Interrupt DMA Ready                               */
/********************************************************************/
#if 0
void IDint_DMA_1(void)
{
#ifdef USB_NO_REALOS
	usb_set_flg( FID_USB , FLG_USB_BulkReadWai);
	usb_set_flg( FID_USB , FLG_USB_VendReadWai);
#else
	OS_User_Set_Flg( FID_USB , FLG_USB_BulkReadWai);
	OS_User_Set_Flg( FID_USB , FLG_USB_VendReadWai);
#endif
	return;
}
#endif



/********************************************************************/
/*  <Function>  void IDint_DMA_2                                    */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <History >  k.maeda 2011.01.07                                  */
/*  <Description> Interrupt DMA Ready                               */
/********************************************************************/
#if 0
void IDint_DMA_2(void)
{
#ifdef USB_NO_REALOS
	usb_set_flg( FID_USB , FLG_USB_BulkWriteWai);
	usb_set_flg( FID_USB , FLG_USB_VendWriteWai);
#else
	OS_User_Set_Flg( FID_USB , FLG_USB_BulkWriteWai);
	OS_User_Set_Flg( FID_USB , FLG_USB_VendWriteWai);
#endif
	return;
}
#endif


#if 0 /* M-7M */
#ifdef BUG_OF_F_USB20HDC_LASP_MACRO_20111115
/********************************************************************/
/*  <Function>  void InterControlStallCB                            */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Interrupt CallBack Stalled                        */
/********************************************************************/
void InterControlStallCB(void)
{
    unsigned char stall_ep;
    
    stall_ep = HdcDev_GetInterruptEp();

    /* ep 0 */
    if( stall_ep == 0 )
    {
        /* Flag is set, if driver isn't made to STALL by itself */
        if( Control_EpStall_setting == MWUSB_NG )
        {
#ifdef USB_NO_REALOS
            usb_set_flg(FID_USB_CONTROL_STALL,FLG_USB_HWStall);
#else
            OS_User_Set_Flg(FID_USB_CONTROL_STALL,FLG_USB_HWStall);
#endif
        }
    }
    return;
}
#endif
#endif /* M-7M */


/********************************************************************/
/*  <Function>  int mwusb_initialize                                */
/*  <Arguments> uchar conf_mode # don't care                        */
/*                  manual mode = 0 / auto mode = 1                 */
/*              uchar boot_mode                                     */
/*                  reboot mode = 0 (not support) / init mode = 1   */
/*  <Return  >  status                                              */
/*                  MWUSB_OK  normal_end                            */
/*                  MWUSB_NG  error_end                             */
/*  <Description> Initialize Driver.                                */
/********************************************************************/
static int mwusb_initialize(unsigned char conf_mode, unsigned char boot_mode)
{
    int stat;
    
    if (boot_mode) {
        /** init mode only  **/
        
        /* LLD initialize */
        idint_ep0_read = NULL;      /* reset control interrupt function */
        stat = HdcDev_InitializeDriver(NULL, 0);
        if (stat == HDC_FAILURE) {
            printf("  ERROR : Failed LLD Initialized. on mwusb_initialize. \n\r"); 
            return MWUSB_NG;
        } else {
            mwusb_InitializeDriver = MWUSB_OK;
        }
        
        MWUSB_SetConfigurationMode(conf_mode); /* set configuration mode */

        /* Descriptor setting */
        usb_descriptor_cntl_init();  /* Clear Descriptor Entry */
        stat = MWUSBC_RegisterDescriptor(); /* Entry Descriptor */
        if (stat != MWUSB_OK) {
            mwusb_InitializeDriver = MWUSB_NG;
            HdcDev_UninitializeDriver();
            printf("  ERROR : Failed setting descriptor. on mwusb_initialize. \n\r"); 
            return MWUSB_NG;
        }
        
        /* Callback function setting */
        stat = MWUSB_DriverEntryCallback(SET_CONFIG_ID17, (void *)IDint_SetConfiguration);
        /* stat |= MWUSB_DriverEntryCallback(SET_INFACE_ID18, (void *)IDint_SetInterface); */
        stat |= MWUSB_DriverEntryCallback(SETUP_CLAS_ID22, (void *)IDint_Setup_CLSVND_Request);
        stat |= MWUSB_DriverEntryCallback(SETUP_VEND_ID25, (void *)IDint_Setup_CLSVND_Request);
        stat |= MWUSB_DriverEntryCallback(BUSRST_STR_ID28, (void *)IDint_BusReset_START);
        stat |= MWUSB_DriverEntryCallback(BUSRST_END_ID29, (void *)IDint_BusReset_END);
        stat |= MWUSB_DriverEntryCallback(SUSPND_STR_ID30, (void *)IDint_Suspend_START);
        stat |= MWUSB_DriverEntryCallback(SUSPND_END_ID31, (void *)IDint_Suspend_END);
        stat |= MWUSB_DriverEntryCallback(PHY_HUNGUP_ID32, (void *)IDint_Phy_Hungup);
        
        stat |= MWUSB_DriverEntryCallback(TRAN_I_CLS_ID23, (void *)InterContWriteReadyCB);
        stat |= MWUSB_DriverEntryCallback(TRAN_O_CLS_ID24, (void *)InterContReadReadyCB);
        stat |= MWUSB_DriverEntryCallback(TRAN_I_VND_ID26, (void *)InterContWriteReadyCB);
        stat |= MWUSB_DriverEntryCallback(TRAN_O_VND_ID27, (void *)InterContReadReadyCB);
        stat |= MWUSB_DriverEntryCallback(BULK_ENDP1_ID00, (void *)InterBulkWriteReadyCB);
        stat |= MWUSB_DriverEntryCallback(BULK_ENDP2_ID01, (void *)InterBulkReadReadyCB);
        stat |= MWUSB_DriverEntryCallback(INTR_ENDP3_ID02, (void *)InterIntrWriteReadyCB);
        stat |= MWUSB_DriverEntryCallback(VEND_ENDP5_ID04, (void *)InterVendWriteReadyCB);
        stat |= MWUSB_DriverEntryCallback(VEND_ENDP6_ID05, (void *)InterVendReadReadyCB);
        stat |= MWUSB_DriverEntryCallback(BLK_EP2_FN_ID51, (void *)InterBulkReadWaitCB);
        stat |= MWUSB_DriverEntryCallback(VND_EP5_FN_ID54, (void *)InterVendReadWaitCB);
        
        if (stat != MWUSB_OK) {
            mwusb_InitializeDriver = MWUSB_NG;
            HdcDev_UninitializeDriver();
            printf("  ERROR : Failed EntryCallback. on mwusb_initialize. \n\r"); 
            return MWUSB_NG;
        }
    }

    /* HDMAC initialize */
    MWUSB_Init_Dma();

    /* Usb Macro initialize */
    stat = HdcDev_InitializeDevice();
    if (stat == HDC_FAILURE) {
        mwusb_InitializeDriver = MWUSB_NG;
        HdcDev_UninitializeDriver();
        printf("  ERROR : Failed Device Initialized. on mwusb_initialize. \n\r"); 
        return MWUSB_NG;
    }
    
    if (boot_mode) {
        /** init mode only  **/
        
        /* Clear all the event flags */
        OS_User_Clr_Flg(FID_USB, 0);

        /* set number of Ep Buffer plane */
        mwusb_SetEP_buffer_num();

        /* USB function start , & go interrupt main roop */
        stat = HdcDev_StartFunction();
        if( stat == HDC_FAILURE ) {
            mwusb_InitializeDriver = MWUSB_NG;
            HdcDev_UninitializeDriver();
            printf("  ERROR : Failed to start function. on mwusb_initialize. \n\r"); 
            return MWUSB_NG;
        }

        /* wait for configure */
        if (MWUSB_WaitConfig() == MWUSB_OK) {
            Control_Func_Status = MWUSB_FUNC_ENABLE; /* Function status change */
        } else {
            mwusb_InitializeDriver = MWUSB_NG;
            HdcDev_UninitializeDriver();
            return MWUSB_NG;
        }
        
    } else {
        /** reboot mode only  **/
        HdcDev_StartFunction();  /* USB function start , & go interrupt main roop */
    }
    
    return MWUSB_OK;
}


/********************************************************************/
/*  <Function>  void MWUSB_Initialize                               */
/*  <Arguments> long conf_mode                                      */
/*                  manual mode = 0 / auto mode = 1                 */
/*  <Return  >  none                                                */
/*  <Description> Initialize Driver.                                */
/********************************************************************/
extern void MWUSB_Initialize(unsigned char conf_mode)
{
    mwusb_InitializeDriver = MWUSB_NG;
    HdcDev_Initialize();
    
#ifndef USE_LOW_POWER
    /* Clock of Device --> ON */
    HdcDev_PowerOnDevice();
#else
    mwusb_PowerOn = MWUSB_NG;
#endif /* USE_LOW_POWER */
    
    return;
}


/********************************************************************/
/*  <Function>  void  MWUSB_Uninitialize                            */
/*  <Arguments> void                                                */
/*  <Return  >  none                                                */
/*  <Description> Uninitialize Driver.                              */
/********************************************************************/
void  MWUSB_Uninitialize(void)
{
#ifndef USE_LOW_POWER
    /* Clock of Device --> OFF */
    (void)HdcDev_PowerOffDevice();
#endif /* USE_LOW_POWER */

    HdcDev_Uninitialize();
    
    return;
}


/************************************************************************/
/*  <Function>  long MWUSB_WaitConnect                                  */
/*  <Arguments> void                                                    */
/*  <Return  >  non                                                     */
/*  <Description> wait for USB Plug Connected                           */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_WaitConnect( void )
{
    unsigned int flg_ptn;
    OS_USER_FLGPTN   rtn_ptn;
    long         rcd = MWUSB_OK;

#ifdef  USE_UART_MESSAGE    
    printf(" MWUSB_WaitConnect\n\r");
    OS_User_Tslp_Tsk(1);
#endif

    /* Wait Event Set */
    flg_ptn = FLG_USB_VbusRise | FLG_USB_VbusFall ;
#ifdef USB_NO_REALOS
    usb_clr_flg(FID_USB, ~(flg_ptn));
#else
    OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
    rtn_ptn = 0;

    flg_ptn = FLG_USB_VbusRise | FLG_USB_VbusFall | FLG_USB_UnConf |
                FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;

    /* wait for usb-plug connected */
    if( MWUSB_CheckConnect() != MWUSB_PLUG_CONNECT )
    {
        while( !(rtn_ptn & FLG_USB_VbusRise) )
        {
#ifdef USB_NO_REALOS
            usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
#else
            OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
#endif
            if( rtn_ptn & (FLG_USB_UnConf | FLG_USB_DisConnected |
                           FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                           FLG_USB_TimeOut | FLG_USB_BusReset) )
            {
                rcd = MWUSB_NG;
                break; /* forcible wait cancel */
            }
        }
#ifdef USB_NO_REALOS
        usb_clr_flg(FID_USB, ~(flg_ptn));
#else
        OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
    }


    /* check plug connection */
    if( rcd == MWUSB_OK )
    {
        Control_Func_Status = MWUSB_FUNC_UNCONF; /* Function status change */
    }

    return( rcd );
}
#endif /* M-7M */



/************************************************************************/
/*  <Function>  long MWUSB_WaitConfig                                   */
/*  <Arguments> void                                                    */
/*  <Return  >  status : normal end(MWUSB_OK) / error end(other)        */
/*  <Description> wait for Configure                                    */
/************************************************************************/
long MWUSB_WaitConfig( void )
{
    unsigned int  flg_ptn;
    OS_USER_FLGPTN   rtn_ptn;
    OS_USER_ER       ret;  /* for M-7M */
    long             rcd = MWUSB_OK;

#ifdef  USE_UART_MESSAGE    
    printf(" MWUSB_WaitConfig. Start.\n\r");
#endif

    /* Wait Event Set */
    flg_ptn = FLG_USB_Config;
#ifdef USB_NO_REALOS
    usb_clr_flg(FID_USB, ~(flg_ptn));
#else
    OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
    rtn_ptn = 0;

    flg_ptn = FLG_USB_Config | FLG_USB_UnConf | FLG_USB_DisConnected |
                FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                FLG_USB_TimeOut | FLG_USB_VbusFall ;

    /* wait for configure */
    if( Control_Func_Status < MWUSB_FUNC_CONFIG )
    {
        while( !(rtn_ptn & FLG_USB_Config) )
        {
#ifdef USB_NO_REALOS
            usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
#else
#if 0 /* M-7M */
            OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
#endif /* M-7M */
            while (1) {
#ifdef  USE_UART_MESSAGE    
                printf(" MWUSB_WaitConfig. Wait for flags (FID_USB).\n\r");
#endif
                ret = OS_User_Twai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn, 500);  /* 500ms */
                if (ret == D_OS_USER_E_TMOUT) {
                    if (mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE) {
                        return ( MWUSB_NG );  /* SetConfig Interrupt didn't occur */
                    }
                } else {  /* ret == E_OK */
                    break;
                }
            }
#endif
            if( rtn_ptn & (FLG_USB_UnConf | FLG_USB_DisConnected | FLG_USB_ClassTerm |
                           FLG_USB_ClassReset | FLG_USB_Cancel | FLG_USB_TimeOut |
                           FLG_USB_VbusFall ) )
            {
#ifdef  USE_UART_MESSAGE    
                printf(" MWUSB_WaitConfig. Error End.\n\r");
#endif
                rcd = MWUSB_NG;
                break;
            }
        }
#ifdef USB_NO_REALOS
        usb_clr_flg(FID_USB, ~(flg_ptn));
#else
        OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
    }

#ifdef  USE_UART_MESSAGE    
    printf(" MWUSB_WaitConfig. Normal End.\n\r");
#endif

    return( rcd );
}



/********************************************************************/
/*  <Function>      void USB_VBUSCHECK_Task()                       */
/*  <Arguments>     None                                            */
/*  <Return  >      void                                            */
/*  <Description>   GPIO porling to check vbus                      */
/********************************************************************/
/* 
 * Do not use this task in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void USB_VBUSCHECK_Task(INT stacd)
{
    //check usb plug connect
    while(1)
    {
        //tslp_tsk(1); /* wait cycle time 5msec */
        if(MWUSBC_CheckConnect() == MWUSB_PLUG_CONNECT)
        {
            if( Control_Vbus_Status != MWUSB_PLUG_CONNECT )
            {
                IDint_Vbus_Rise(); /** interrupt process **/
            }
        }
        else
        {
            if( Control_Vbus_Status != MWUSB_PLUG_RELEASE )
            {
                IDint_Vbus_Fall(); /** interrupt process **/
                MWUSB_ReleaseWaitCancel(); /* Set Cancel Event Flag */
                MWUSB_AbortReset();
            }
        }
        OS_User_Tslp_Tsk(100); // wait cycle time 100msec
    }
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  long MWUSB_CheckConnect                                 */
/*  <Arguments> void                                                    */
/*  <Return  >  MWUSB_PLUG_CONNECT : Connect                            */
/*              MWUSB_PLUG_RELEASE : Release                            */
/*  <Description>   Check USB Plug Connected                            */
/************************************************************************/
long MWUSB_CheckConnect( void ) 
{
    return( mwusb_CheckConnect(1) );
}


/************************************************************************/
/*  <Function>  long mwusb_CheckConnect                                 */
/*  <Arguments> unsigned char chatteringFlag                            */
/*                  chattering signal removal flag                      */
/*  <Return  >  MWUSB_PLUG_CONNECT : Connect                            */
/*              MWUSB_PLUG_RELEASE : Release                            */
/*  <Description>   Check USB Plug Connected                            */
/************************************************************************/
static long mwusb_CheckConnect(unsigned char chatteringFlag) 
{
    long state;
    
    state = (long)HdcDev_CheckConnect(chatteringFlag);
    if (state == HDC_FAILURE) {
        state = MWUSBC_CheckConnect(chatteringFlag);
    }
    
    return state;
}


/************************************************************************/
/*  <Function>  int MWUSB_RegisterDescriptor                            */
/*  <Arguments> const DESCRIPTOR PACKET *Descriptor                     */
/*                  Descriptor address                                  */
/*              const int cno                                           */
/*                  configuration number(1-n)                           */
/*              const int kind                                          */
/*                  descriptor kind                                     */
/*              const unsigned long speed                               */
/*                  bus speed                                           */
/*              const long mode                                         */
/*                  Tune mode/Normal mode define.                       */
/*              const unsigned short index                              */
/*                  Interface number responding to Report Descriptor    */
/*  <Return  >  status                                                  */
/*                  MWUSB_OK  normal_end                                */
/*                  MWUSB_NG  error_end                                 */
/*  <Description> Entry Descriptor address                              */
/************************************************************************/
int MWUSB_RegisterDescriptor(const DESCRIPTOR_PACKET *Descriptor,
                             const int cno,
                             const int kind,
                             const unsigned long speed,
                             const long mode,
                             const unsigned short index)
{
    int stat = MWUSB_OK;
    int config_max;
    int type = TYPE_NULL;
    MWUSB_DESCRIPTOR_ADDRESS *descriptor_cntl;
    
    /* before driver initialize */
    if (mwusb_InitializeDriver == MWUSB_NG) {
        stat = MWUSB_OK;
        return stat;
    }
    
    if ((speed == MWUSB_FULLSPEED) || (speed == MWUSB_HISPEED)) {
        config_max = MWUSB_HS_CONFIG_MAX;
        
    } else {
        /* error */
        config_max = 0;
    }
    
    if ((cno == 0) || (cno > config_max)) {
        stat = MWUSB_NG;
    
    } else {
        /* Set descriptor control table */
        if (speed == MWUSB_FULLSPEED) {
            descriptor_cntl = FS_descriptor_cntl;
            
        } else {
            descriptor_cntl = HS_descriptor_cntl;
            
        }
        
        /* Set descriptor address */
        switch (kind) {
        case MWUSB_KIND_DEVICE:
            descriptor_cntl[cno - 1].device = (void *)Descriptor;
            type = DES_TYPE_DEVICE;
            break;
        case MWUSB_KIND_CONFIG:
            descriptor_cntl[cno - 1].config = (void *)Descriptor;
            type = DES_TYPE_CONFIGURATION;
            break;
        case MWUSB_KIND_LANGID:
            descriptor_cntl[cno - 1].langid = (void *)Descriptor;
            type = DES_TYPE_STRING;
            break;
        case MWUSB_KIND_STRING1:
            descriptor_cntl[cno - 1].string1 = (void *)Descriptor;
            type = DES_TYPE_STRING;
            break;
        case MWUSB_KIND_STRING2:
            descriptor_cntl[cno - 1].string2 = (void *)Descriptor;
            type = DES_TYPE_STRING;
            break;
        case MWUSB_KIND_STRING3:
            descriptor_cntl[cno - 1].string3 = (void *)Descriptor;
            type = DES_TYPE_STRING;
            break;
        case MWUSB_KIND_DEVICE_QUALIFIER:
            descriptor_cntl[cno - 1].dev_qual = (void *)Descriptor;
            type = DES_TYPE_DEVICE_QUALIFIER;
            break;
        case MWUSB_KIND_OTHER_SPEED_CONFIG:
            descriptor_cntl[cno - 1].other_sp = (void *)Descriptor;
            type = DES_TYPE_OTHER_SPEED;
            break;
        case MWUSB_KIND_REPORT:
            descriptor_cntl[cno - 1].report = (void *)Descriptor;
            type = DES_TYPE_REPORT;
            break;
        default:
            stat = MWUSB_NG;
            break;
        }
        
        if (type != TYPE_NULL) {
            /* Entry Descriptor address */
            stat = HdcDev_EntryDescriptor(type, (void *)Descriptor, speed, index);
            descriptor_cntl[cno - 1].mode  = mode;
            
            if (stat == 0) {
                stat = MWUSB_OK;
            } else {
                stat = MWUSB_NG;
            }
        }
    }
    
    return (stat);
}



/************************************************************************/
/*  <Function>  void MWUSB_DriverEntryCallback                          */
/*  <Arguments> unsigned char callbackID                                */
/*                  callbackID No.                                      */
/*              void *pcallback                                         */
/*                  callback function pointer                           */
/*              unsigned char busSpeed                                  */
/*                  bus speed                                           */
/*  <Return  >  status                                                  */
/*                  MWUSB_OK  normal_end                                */
/*                  MWUSB_NG  error_end                                 */
/*  <Description> Entry Callback function                               */
/************************************************************************/
int MWUSB_DriverEntryCallback(unsigned char callbackID, void *pcallback)
{
    int stat;
    
    /* Entry Callback Pointer */
    stat = HdcDev_EntryCallback(callbackID,pcallback);
    
    if (stat == HDC_SUCCESS) { 
        stat = MWUSB_OK;
    } else { 
        stat = MWUSB_NG;
    }
    
    return stat;
}


#if 0
/********************************************************************/
/*  <Function>      void wait_reset()                               */
/*  <Arguments>     None                                            */
/*  <Return  >      void                                            */
/*  <Description>   Reset Waite Loop                                */
/********************************************************************/
static void wait_reset()
{
    volatile int i,a;

    for(i = 0; i < USB_WAIT_CYCLE; i++)
    {
        a=i;
    }
}
#endif



/************************************************************************/
/*  <Function>      long MWUSB_BulkRead_Dummy                           */
/*  <Arguments>     unsigned char FIFO                                  */
/*                      Index of Endpoint                               */
/*                  unsigned long size                                  */
/*                      size of dummy read                              */
/*  <Return  >      none                                                */
/*  <Description>   Request Endpoint to stall condition                 */
/*                  2011.02.22 add for USBCV 2.00 MSC Test              */
/************************************************************************/
void MWUSB_BulkRead_Dummy(unsigned char nfifo, unsigned long size)
{
    return;
}


/************************************************************************/
/*  <Function>      long MWUSB_EPStalled                                */
/*  <Arguments>     unsigned char FIFO                                  */
/*                      Index of Endpoint                               */
/*  <Return  >      MWUSB_OK    Normal End  :                           */
/*                  MWUSB_NG    Error                                   */
/*  <Description>   Request Endpoint to stall condition                 */
/************************************************************************/
long MWUSB_EPStalled( unsigned char FIFO )
{
    short check;
    long  rcd;
    
    /* Low Level Driver is already initialized. */
    if (mwusb_InitializeDriver == MWUSB_OK)
    {
        check = mwusb_FifoNo_2_EpNo(FIFO);

        if( check != MWUSB_ERR_BADEP )
        {
#ifdef  USE_UART_MESSAGE    
            printf("    ==> MWUSB_EPStalled(%d)\n\r",check);
#endif
            if( FIFO == CNTL_READ_FIFO )
            {
                idint_ep0_read = NULL;      /* reset interrupt function */
            }
            rcd = mwusb_EPStalled((unsigned char)check);
            
            /* Wait to be stalled */
            OS_User_Dly_Tsk(2);
        }
        else
        {
            rcd = (long)check;
        }
    }
    else
    {
        rcd = MWUSB_NG;
    }
    
    return( rcd );
}



/************************************************************************/
/*  <Function>      static long mwusb_EPStalled                         */
/*  <Arguments>     unsigned char epno                                  */
/*                      Endpoint no                                     */
/*  <Return  >      MWUSB_OK    Normal End  :                           */
/*                  MWUSB_NG    Error                                   */
/*  <Description>   Request Endpoint to stall condition                 */
/************************************************************************/
static long mwusb_EPStalled( unsigned char EpNo )
{
    long ret = MWUSB_NG;
    
    if(EpNo == CntlOut_Trans.epno){ret = MWUSB_OK;}
    else if(EpNo == CntlIn_Trans.epno){ret = MWUSB_OK;}
    else if((BulkOut_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkOut_Trans.epno)){ret = MWUSB_OK;}
    else if((VendOut_Trans.stat == MWUSB_EP_USE) && (EpNo == VendOut_Trans.epno)){ret = MWUSB_OK;}
    else if((IsocOut_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocOut_Trans.epno)){ret = MWUSB_OK;}
    else if((BulkIn_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkIn_Trans.epno)){ret = MWUSB_OK;}
    else if((VendIn_Trans.stat == MWUSB_EP_USE) && (EpNo == VendIn_Trans.epno)){ret = MWUSB_OK;}
    else if((IntrIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IntrIn_Trans.epno)){ret = MWUSB_OK;}
    else if((IsocIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocIn_Trans.epno)){ret = MWUSB_OK;}

    if( ret == MWUSB_OK )
    {
#ifdef USB_DEBUG2
        printf("EPstallset : %d \n\r",EpNo);
#endif
        if (HdcDev_SetEndpointHalt(EpNo,1)!=0)
        {
            ret = MWUSB_NG;
        }
    }

    return ret;
}



/********************************************************************/
/*  <Function>      long MWUSB_EPStallClr                           */
/*  <Arguments>     unsigned char FIFO                              */
/*                      Index of FIFO                               */
/*  <Return  >      MWUSB_OK    Normal End                          */
/*                  MWUSB_ERR_BADEP     Bad Endpoint                */
/*  <Description>   Clear stall condition of Endpoint               */
/*********************************************************************/
long MWUSB_EPStallClr( unsigned char FIFO )
{
    short check;
    long  rcd;
    
    /* Low Level Driver is already initialized. */
    if (mwusb_InitializeDriver == MWUSB_OK) {
        check = mwusb_FifoNo_2_EpNo(FIFO);
        
        if ( check != MWUSB_ERR_BADEP ) {
            rcd = mwusb_EPStallClr((unsigned char)check);
        } else {
            rcd = (long)check;
        }
    } else {
        rcd = MWUSB_NG;
    }
    
    return rcd;
}



/********************************************************************/
/*  <Function>      static long mwusb_EPStallClr                    */
/*  <Arguments>     unsigned char EpNo                              */
/*                      Endpoint No                                 */
/*  <Return  >      MWUSB_OK    Normal End                          */
/*                  MWUSB_ERR_BADEP     Bad Endpoint                */
/*  <Description>   Clear stall condition of Endpoint               */
/*********************************************************************/
static long mwusb_EPStallClr( unsigned char EpNo )
{
    long ret = MWUSB_NG;

    if(EpNo == CntlOut_Trans.epno){ret =MWUSB_OK;}
    else if((BulkOut_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkOut_Trans.epno)){ret =MWUSB_OK;}
    else if((VendOut_Trans.stat == MWUSB_EP_USE) && (EpNo == VendOut_Trans.epno)){ret =MWUSB_OK;}
    else if((IsocOut_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocOut_Trans.epno)){ret =MWUSB_OK;}
    else if((BulkIn_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkIn_Trans.epno)){ret =MWUSB_OK;}
    else if((VendIn_Trans.stat == MWUSB_EP_USE) && (EpNo == VendIn_Trans.epno)){ret =MWUSB_OK;}
    else if((IntrIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IntrIn_Trans.epno)){ret =MWUSB_OK;}
    else if((IsocIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocIn_Trans.epno)){ret =MWUSB_OK;}

    if( ret == MWUSB_OK )
    {
#ifdef USB_DEBUG2
        printf("EPstallclr : %d \n\r",EpNo);
#endif
        if (HdcDev_SetEndpointHalt(EpNo,0)!=0)
        {
            ret = MWUSB_NG;
        }
    }

    return ret;
}


/************************************************************************/
/*  <Function>      long MWUSB_EPStallCancel                            */
/*  <Arguments>     unsigned char FIFP                                  */  
/*                      Index of Endpoint                               */
/*  <Return  >      MWUSB_OK    Normal End  :                           */
/*                  MWUSB_NG    Error                                   */
/*  <Description>   Cancel stall request if Endpoint is not stall       */
/*                  condition.                                          */
/************************************************************************/
long MWUSB_EPStallCancel( unsigned char FIFO )
{
    short check;
    long  rcd;
    
    /* Low Level Driver is already initialized. */
    if (mwusb_InitializeDriver == MWUSB_OK)
    {
        check = mwusb_FifoNo_2_EpNo(FIFO);

        if( check != MWUSB_ERR_BADEP )
        {
            rcd = mwusb_EPStallCancel((unsigned char)check);
        }
        else
        {
            rcd = MWUSB_NG;
        }
    }
    else
    {
        rcd = MWUSB_NG;
    }
    
    return( rcd );
}


/************************************************************************/
/*  <Function>      static long mwusb_EPStallCancel                     */
/*  <Arguments>     unsigned char EpNo                                  */
/*                      Endpoint No                                     */
/*  <Return  >      MWUSB_OK    Normal End  :                           */
/*                  MWUSB_NG    Error                                   */
/*  <Description>   Cancel stall request if Endpoint is not stall       */
/*                  condition.                                          */
/************************************************************************/
static long mwusb_EPStallCancel( unsigned char EpNo )
{
    long ret = MWUSB_NG;
    unsigned char haltFlag;

    if(EpNo == CntlOut_Trans.epno){ret =MWUSB_OK;}
    else if((BulkOut_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkOut_Trans.epno)){ret =MWUSB_OK;}
    else if((VendOut_Trans.stat == MWUSB_EP_USE) && (EpNo == VendOut_Trans.epno)){ret =MWUSB_OK;}
    else if((IsocOut_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocOut_Trans.epno)){ret =MWUSB_OK;}
    else if((BulkIn_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkIn_Trans.epno)){ret =MWUSB_OK;}
    else if((VendIn_Trans.stat == MWUSB_EP_USE) && (EpNo == VendIn_Trans.epno)){ret =MWUSB_OK;}
    else if((IntrIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IntrIn_Trans.epno)){ret =MWUSB_OK;}
    else if((IsocIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocIn_Trans.epno)){ret =MWUSB_OK;}

    if( ret == MWUSB_OK )
    {
#ifdef USB_DEBUG2
        printf("EPstallcan : %d \n\r",EpNo);
#endif
        if( HdcDev_IsEndpointHalt(EpNo,&haltFlag) != 0 )
        {
            ret = MWUSB_NG; /* Error */
        }
        else
        {
            if( haltFlag == 1 )
            {
                /* Cancel Halt */
#ifdef  USE_UART_MESSAGE
                printf("  EP %d Halted Cancel\n\r",EpNo);
#endif
                if( HdcDev_SetEndpointHalt(EpNo,0) != 0 )
                {
#ifdef  USE_UART_MESSAGE
                    printf("  --->Result NG : %d \n\r",EpNo);
#endif
                    ret=MWUSB_NG;
                }
#ifdef  USE_UART_MESSAGE
                else
                {
                    printf("  --->Result OK : %d \n\r",EpNo);
                }
#endif
            }
        }
    }

    return ret;
}


/************************************************************************/
/*  <Function>      long MWUSB_EPInit                                   */
/*  <Arguments>     unsigned char FIFO                                  */  
/*                      Index of Endpoint                               */
/*  <Return  >      MWUSB_OK    Normal End  :                           */
/*                  MWUSB_NG    Error                                   */
/*  <Description>   Initialize EP FIFO                                  */
/************************************************************************/
long MWUSB_EPInit(unsigned char FIFO)
{
    short check;
    long  rcd;
    
    /* Low Level Driver is already initialized. */
    if (mwusb_InitializeDriver == MWUSB_OK)
    {
        check = mwusb_FifoNo_2_EpNo(FIFO);

        if( check != MWUSB_ERR_BADEP )
        {
            if( FIFO == CNTL_READ_FIFO )
            {
                idint_ep0_read = NULL;      /* reset interrupt function */
            }
            rcd = mwusb_EPInit((unsigned char)check);
        }
        else
        {
            rcd = MWUSB_NG;
        }
    }
    else
    {
        rcd = MWUSB_NG;
    }
    
    return( rcd );
}


/************************************************************************/
/*  <Function>      long mwusb_EPInit                                   */
/*  <Arguments>     unsigned char EpNo                                  */
/*                      Endpoint No                                     */
/*  <Return  >      MWUSB_OK    Normal End  :                           */
/*                  MWUSB_NG    Error                                   */
/*  <Description>   Initialize EP FIFO                                  */
/************************************************************************/
static long mwusb_EPInit(unsigned char EpNo)
{
    long ret = MWUSB_OK;
    unsigned char haltFlag;
    unsigned char emptyFlag;
    long err;

    if ((EpNo == CntlOut_Trans.epno) ||
        ((BulkOut_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkOut_Trans.epno)) ||
        ((VendOut_Trans.stat == MWUSB_EP_USE) && (EpNo == VendOut_Trans.epno)) ||
        ((IsocOut_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocOut_Trans.epno))   ) {
        
        if (HdcDev_IsEndpointHalt(EpNo, &haltFlag) == 0) {
            if (haltFlag == 0) {
                /* No Stall */
                if( HdcDev_GetTransferredState(EpNo, 0) == HDC_TRN_ST_BUSY) {
                    /* CPU Transfer */
                    HdcDev_StopTransfer(EpNo, 0);
                    HdcDev_ClearEpBuffer(EpNo,(unsigned char)1);
                    
                } else if ((EpNo == BulkOut_Trans.epno) && 
                           (HdcDev_GetDmaTransferredState(DMA_CH_BULK_OUT) == HDC_TRN_ST_BUSY)) {
                    /*  DMA Transfer */
                    HdcDev_StopDmaTransfer(DMA_CH_BULK_OUT);
                    MWUSB_Abort_Dma(DMA_CH_BULK_OUT);
                    
                } else {
                    err = HdcDev_IsEpBufferEmpty(EpNo, &emptyFlag);
                    if ((err == HDC_SUCCESS) && (emptyFlag == 0)) {
                        OS_User_Clr_Flg( FID_USB , ~FLG_USB_BulkReadRdy);
                        HdcDev_ClearEpBuffer(EpNo,(unsigned char)1);
                    }
                }
            } else {
                /* Clear Stalled */
                HdcDev_ClearEpBuffer(EpNo,(unsigned char)1);
                mwusb_EPStallCancel(EpNo);
            }
        } else {
            ret = MWUSB_NG; /* Error */
        }
    } else if (((BulkIn_Trans.stat == MWUSB_EP_USE) && (EpNo == BulkIn_Trans.epno)) ||
               ((VendIn_Trans.stat == MWUSB_EP_USE) && (EpNo == VendIn_Trans.epno)) ||
               ((IntrIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IntrIn_Trans.epno)) ||
               ((IsocIn_Trans.stat == MWUSB_EP_USE) && (EpNo == IsocIn_Trans.epno))   ) {
        
        if (HdcDev_IsEndpointHalt(EpNo, &haltFlag) == 0) {
            if( haltFlag == 0 ) {
                /* No Stall */
                if (HdcDev_GetTransferredState(EpNo, 1) == HDC_TRN_ST_BUSY) {
                    /* CPU Transfer */
                    HdcDev_StopTransfer(EpNo, 1);
                    HdcDev_ClearEpBuffer(EpNo,(unsigned char)1);
                    
                } else if ((EpNo == BulkIn_Trans.epno) &&
                           (HdcDev_GetDmaTransferredState(DMA_CH_BULK_IN) == HDC_TRN_ST_BUSY)) {
                    /*  DMA Transfer */
                    HdcDev_StopDmaTransfer(DMA_CH_BULK_IN);
                    MWUSB_Abort_Dma(DMA_CH_BULK_IN);
                }
            } else {
                /* Clear Stalled */
                HdcDev_ClearEpBuffer(EpNo,(unsigned char)1);
                mwusb_EPStallCancel(EpNo);
            }
        } else {
            ret = MWUSB_NG; /* Error */
        }
    } else {
        ret = MWUSB_NG;
    }
    
    return (ret);
}



/************************************************************************/
/*  <Function>  static long mwusb_SendNullPacket                        */
/*  <Argments>  unsigned short endp     :Endpoint No.                   */
/*  <Return>    MWUSB_OK    Normal End                                  */
/*              MWUSB_ERR_RESET  Bus Reset                              */
/*              MWUSB_ERR_SUSPEND Bus Seupended                         */
/*              MWUSB_ERR_STALL     Endpoint Stalled                    */
/*              MWUSB_ERR_NOTIDLE Transfer phase NTO IDLE               */
/*              MWUSB_ERR_BADEP     Bad Endpoint No.                    */
/*  <Description>  Send Null Packet if Readyi Bit is High               */
/*************************************************************************/
static long mwusb_SendNullPacket(unsigned char endp)
{
    long ret;
    long dmy_buf = 1;

    ret = MWUSB_OK;

    if( ((BulkIn_Trans.stat == MWUSB_EP_USE) && (endp == BulkIn_Trans.epno)) ||
        ((VendIn_Trans.stat == MWUSB_EP_USE) && (endp == VendIn_Trans.epno)) ||
        ((IntrIn_Trans.stat == MWUSB_EP_USE) && (endp == IntrIn_Trans.epno)) ||
        ((IsocIn_Trans.stat == MWUSB_EP_USE) && (endp == IsocIn_Trans.epno))    )
    {
        if( mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE )
        {
            ret = MWUSB_ERR_QUIT; /* Disconnect or Terminate */
        }
        else if( MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG )
        {   /* Not Configured */
            ret = MWUSB_ERR_UNCONF;
        }
        else if( Control_BusReset_Status == BUS_RESET_DOING )
        {   /* Bus Reset */
            ret = MWUSB_ERR_RESET;
        }
        if( ret < 0 )
        {   /* Error */
#ifdef USE_UART_MESSAGE
            printf("USB Out Error mwusb_SendNullPacket\n\r");
#endif
            return ret;
        }
        
        if( fReqBulk_Null == 1 )
        {   // Send Null
            ret = HdcDev_SetInTransferData(endp,(unsigned char *)&dmy_buf,0,0);
            if( ret == -1 )
            {
                /* Error End */
                if( Control_BusReset_Status == BUS_RESET_DOING )
                {
                    ret = MWUSB_ERR_RESET; /* Bus Reset */
                }
            }
            fReqBulk_Null = 0;
        }
    }
    else
    {
        ret = MWUSB_ERR_BADEP;
    }

    return (ret);
}



/************************************************************************/
/*  <Function>  MWUSB_SendNullPacket                                    */
/*  <Argments>  unsigned short FIFO     :Endpoint of Index.             */
/*  <Return>    MWUSB_OK    Normal End                                  */
/*              MWUSB_ERR_RESET  Bus Reset                              */
/*              MWUSB_ERR_SUSPEND Bus Seupended                         */
/*              MWUSB_ERR_STALL     Endpoint Stalled                    */
/*              MWUSB_ERR_NOTIDLE Transfer phase NTO IDLE               */
/*              MWUSB_ERR_BADEP     Bad Endpoint No.                    */
/*  <Description>  Send Null Packet if Readyi Bit is High               */
/*************************************************************************/
long MWUSB_SendNullPacket(unsigned char FIFO)
{
    short check;
    long  rcd;
    
    fReqBulk_Null = 1;
    
    check = mwusb_FifoNo_2_EpNo(FIFO);

    if( check != MWUSB_ERR_BADEP )
    {
        rcd = mwusb_SendNullPacket((unsigned char)check);
    }
    else
    {
        rcd = (long)check;
    }
    
    return( rcd );
}



/************************************************************************/
/*  <Function>  void mwusb_SetEP_buffer_num                             */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Set Endpoint buffer numbers                         */
/************************************************************************/
static void mwusb_SetEP_buffer_num(void)
{
    unsigned char   epno, ifno, atns, confval;

    if( BulkOut_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   BulkOut_Trans.epno;
        ifno    =   BulkOut_Trans.ifno;
        atns    =   BulkOut_Trans.atns;
        confval =   (unsigned char)BulkOut_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }

    if( VendOut_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   VendOut_Trans.epno;
        ifno    =   VendOut_Trans.ifno;
        atns    =   VendOut_Trans.atns;
        confval =   (unsigned char)VendOut_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }

    if( BulkIn_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   BulkIn_Trans.epno;
        ifno    =   BulkIn_Trans.ifno;
        atns    =   BulkIn_Trans.atns;
        confval =   (unsigned char)BulkIn_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }

    if( VendIn_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   VendIn_Trans.epno;
        ifno    =   VendIn_Trans.ifno;
        atns    =   VendIn_Trans.atns;
        confval =   (unsigned char)VendIn_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }

    if( IntrIn_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   IntrIn_Trans.epno;
        ifno    =   IntrIn_Trans.ifno;
        atns    =   IntrIn_Trans.atns;
        confval =   (unsigned char)IntrIn_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }

#if 0
    if( IsocOut_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   IsocOut_Trans.epno;
        ifno    =   IsocOut_Trans.ifno;
        atns    =   IsocOut_Trans.atns;
        confval =   (unsigned char)IsocOut_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }

    if( IsocIn_Trans.stat == MWUSB_EP_USE )
    {
        epno    =   IsocIn_Trans.epno;
        ifno    =   IsocIn_Trans.ifno;
        atns    =   IsocIn_Trans.atns;
        confval =   (unsigned char)IsocIn_Trans.confval;

        HdcDev_SetEpBufferNum(confval, ifno, atns, epno, HDC_DOUBLE_BUF);
    }
#endif

    return;
}



/************************************************************************/
/*  <Function>  void mwusb_SetEP_buffer_info                            */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Set Endpoint buffer plane numbers                   */
/************************************************************************/
static void mwusb_SetEP_buffer_info(void)
{
    DESCRIPTOR_PACKET          * data;
    unsigned char              * desc;
    unsigned short             total_desc_len;
    unsigned char              desc_len;
    MWUSB_ENDPOINT_DESCRIPTOR  * Epd;
    MWUSB_INTERFACE_DESCRIPTOR * Ifd;
    unsigned int                 psize=0;
    unsigned char                all_infno=0, Ifno=0, ats=0;
    unsigned char                epno=0, dir=0, eptype=0;
    unsigned char                confval=0;

    data = ConfigDescp;
    
    while ( data->Descriptor != NULL )
    {
        desc = data->Descriptor;
        total_desc_len = ((MWUSB_CONFIG_DESCRIPTOR *)desc)->wTotalLength; /* Little Endian */
        desc_len = 0;
        all_infno = 0;
        
        while (total_desc_len != 0) {
        
            switch(desc[1])
            {
            case 2: /* configuration */
                confval   = ((MWUSB_CONFIG_DESCRIPTOR *)desc)->bConfigurationValue;
                desc_len  = ((MWUSB_CONFIG_DESCRIPTOR *)desc)->bLength;
                break;
            case 4: /* interface */
                Ifd       = (MWUSB_INTERFACE_DESCRIPTOR *)desc;
                ats       = Ifd->bAlternateSetting;
                all_infno = Ifd->bNumEndpoints;
                Ifno      = Ifd->bInterfaceNumber;
                desc_len  = Ifd->bLength;
                break;
            case 5: /* endpoint */
                if( all_infno > 0 )
                {
                
                    /* read Endpoint descriptor */
                    Epd    = (MWUSB_ENDPOINT_DESCRIPTOR *)desc;
                    epno   = Epd->bEndPointAddress & 0xf;
                    dir    = Epd->bEndPointAddress >> 7 & 0x1;
                    eptype = Epd->bmAttributes & 0x3; /* eptype( 0=control, 1=Isochronous, 2=bulk, 3=interrupt) */
/* Endian change (to Little Endian for M-7M) */
/*                  psize  = ((Epd->wMaxPacketSize << 8) & 0xFF00) | ((Epd->wMaxPacketSize >> 8) & 0x00FF); */
                    psize  = Epd->wMaxPacketSize;
                    desc_len = Epd->bLength;
                    
                    /* switch at endpoint-type */
                    switch (eptype)
                    {
                    case MWUSB_EP_TYPE_CONTROL :
                        break;
                    case MWUSB_EP_TYPE_BULK :
                        if( dir == 0 )
                        {
                            /* Out transfer */
                            if( epno < 3 )
                            {
                                BulkOut_Trans.sizeOfpacket  = psize;
                                BulkOut_Trans.epno          = epno;
                                BulkOut_Trans.ifno          = Ifno;
                                BulkOut_Trans.atns          = ats;
                                BulkOut_Trans.confval       = confval;
                                BulkOut_Trans.stat          = MWUSB_EP_USE;
                            }
                            else
                            {
                                VendOut_Trans.sizeOfpacket  = psize;
                                VendOut_Trans.epno          = epno;
                                VendOut_Trans.ifno          = Ifno;
                                VendOut_Trans.atns          = ats;
                                VendOut_Trans.confval       = confval;
                                VendOut_Trans.stat          = MWUSB_EP_USE;
                            }
                        }
                        else
                        {
                            /* In transfer */
                            if( epno < 3 )
                            {
                                BulkIn_Trans.sizeOfpacket   = psize;
                                BulkIn_Trans.epno           = epno;
                                BulkIn_Trans.ifno           = Ifno;
                                BulkIn_Trans.atns           = ats;
                                BulkIn_Trans.confval        = confval;
                                BulkIn_Trans.stat           = MWUSB_EP_USE;
                            }
                            else
                            {
                                VendIn_Trans.sizeOfpacket   = psize;
                                VendIn_Trans.epno           = epno;
                                VendIn_Trans.ifno           = Ifno;
                                VendIn_Trans.atns           = ats;
                                VendIn_Trans.confval        = confval;
                                VendIn_Trans.stat           = MWUSB_EP_USE;
                            }
                        }
                        break;
                    case MWUSB_EP_TYPE_INTERRUPT :
                        if( dir == 0 )
                        {
                            /* Out transfer */
                        }
                        else
                        {
                            /* In transfer */
                            IntrIn_Trans.sizeOfpacket   = psize;
                            IntrIn_Trans.epno           = epno;
                            IntrIn_Trans.ifno           = Ifno;
                            IntrIn_Trans.atns           = ats;
                            IntrIn_Trans.confval        = confval;
                            IntrIn_Trans.stat           = MWUSB_EP_USE;
                        }
                        break;
#if 0
                    case MWUSB_EP_TYPE_ISOCHRONOUS :
                        if( dir == 0 )
                        {
                            /* Out transfer */
                            IsocOut_Trans.sizeOfpacket  = psize;
                            IsocOut_Trans.epno          = epno;
                            IsocOut_Trans.ifno          = Ifno;
                            IsocOut_Trans.atns          = ats;
                            IsocOut_Trans.confval       = confval;
                            IsocOut_Trans.stat          = MWUSB_EP_USE;
                        }
                        else
                        {
                            /* In transfer */
                            IsocIn_Trans.sizeOfpacket   = psize;
                            IsocIn_Trans.epno           = epno;
                            IsocIn_Trans.ifno           = Ifno;
                            IsocIn_Trans.atns           = ats;
                            IsocIn_Trans.confval        = confval;
                            IsocIn_Trans.stat           = MWUSB_EP_USE;
                        }
                        break;
#endif
                    }
                
                    all_infno--;
                }
                break;
            
            default:
                desc_len  = desc[0];
            }
            
            desc = desc + desc_len; /* next Descriptor */
            total_desc_len = total_desc_len - desc_len;
            
        }
        data++; /* data address update */
    }
    return;
}



/********************************************************************/
/*  <Function>  void MWUSB_DefineDescriptor                         */
/*  <Arguments>     const DESCRIPTOR_PACKET *Device                 */
/*                      Device Descriptor                           */
/*                  const DESCRIPTOR_PACKET *Config                 */
/*                      Configuration Descriptor                    */
/*                  const DESCRIPTOR_PACKET *pLangID                */
/*                      String Descriptor(Language ID)              */
/*                  const DESCRIPTOR_PACKET *pIndex1                */
/*                      String Descriptor(Index1)                   */
/*                  const DESCRIPTOR_PACKET *pIndex2                */
/*                      String Descriptor(Index2)                   */
/*                  const DESCRIPTOR_PACKET *pIndex3                */
/*                      String Descriptor(Index3)                   */
/*                  const DESCRIPTOR_PACKET *pDeviceQual            */
/*                      Device Qualifier Descriptor                 */
/*                  const DESCRIPTOR_PACKET *pOther                 */
/*                      Other Speed Configuration Descriptor        */
/*                  const DESCRIPTOR_PACKET *pReport                */
/*                      Report Descriptor                           */
/*  <Return  >      void                                            */
/*  <Description>   Setting Descriptor Data pointer                 */
/********************************************************************/
void  MWUSB_DefineDescriptor(const DESCRIPTOR_PACKET *Device,
                             const DESCRIPTOR_PACKET *Config,
                             const DESCRIPTOR_PACKET *pLangID,
                             const DESCRIPTOR_PACKET *pIndex1,
                             const DESCRIPTOR_PACKET *pIndex2,
                             const DESCRIPTOR_PACKET *pIndex3,
                             const DESCRIPTOR_PACKET *pDeviceQual,
                             const DESCRIPTOR_PACKET *pOther,
                             const DESCRIPTOR_PACKET *pReport)
{
    ConfigDescp      = (DESCRIPTOR_PACKET*)Config;
    
/* The following global variables are never used */
#if 0 /* M-7M */
    DeviceDescp      = (DESCRIPTOR_PACKET*)Device;
    LANGID           = (DESCRIPTOR_PACKET*)pLangID;
    Index1           = (DESCRIPTOR_PACKET*)pIndex1;
    Index2           = (DESCRIPTOR_PACKET*)pIndex2;
    Index3           = (DESCRIPTOR_PACKET*)pIndex3;
    DeviceQualDescp  = (DESCRIPTOR_PACKET*)pDeviceQual;
    OtherConfigDescp = (DESCRIPTOR_PACKET*)pOther;
    ReportDescp      = (DESCRIPTOR_PACKET*)pReport;
#endif /* M-7M */
    
    if (MWUSB_GetConfigurationMode() == MWUSB_CONFIG_AUTO) {
        if (Max_Config_Number < ((MWUSB_CONFIG_DESCRIPTOR *)Config->Descriptor)->bConfigurationValue) {
            Max_Config_Number = ((MWUSB_CONFIG_DESCRIPTOR *)Config->Descriptor)->bConfigurationValue;
            mwusb_SetEP_buffer_info();  /* Set Endpoint Buffer */
        }
    } else {
        mwusb_SetEP_buffer_info();  /* Set Endpoint Buffer */
    }
    
    return;
}



/********************************************************************/
/*  <Function>  void MWUSB_SetMode                                  */
/*  <Arguments>     long mode                                       */
/*                      0:Normal , 1:Bypass                         */
/*  <Return  >      void                                            */
/*  <Description>                                                   */
/*      Protocol mode switch                                        */
/********************************************************************/
void MWUSB_SetMode(long mode)
{
    /* MWUSB_BYPASS_MODE is not support in M-7M. */
    BYPS_Mode = MWUSB_NORMAL_MODE;
    return;
}


/********************************************************************/
/*  <Function>  long MWUSB_GetMode( void )                          */
/*  <Arguments>     void                                            */
/*  <Return  >      int mode    0:Normal , 1:Bypass                 */
/*  <Description>                                                   */
/********************************************************************/
long MWUSB_GetMode(void)
{
    return (BYPS_Mode);
}



/********************************************************************/
/*  <Function>  void MWUSB_SetConfigurationMode                     */
/*  <Arguments>     long mode                                       */
/*                      0:manual , 1:auto                           */
/*  <Return  >      void                                            */
/*  <Description>                                                   */
/*      Protocol mode switch                                        */
/********************************************************************/
void MWUSB_SetConfigurationMode(unsigned char mode)
{

    if( mode == MWUSB_CONFIG_AUTO )
    {
        Configuration_Mode_Status = MWUSB_CONFIG_AUTO; /* auto */
    }
    else
    {
        Configuration_Mode_Status = MWUSB_CONFIG_MANUAL; /* manual */
    }
}


/********************************************************************/
/*  <Function>  long MWUSB_GetConfigurationMode( void )             */
/*  <Arguments>     void                                            */
/*  <Return  >      long mode   0:manual , 1:auto                   */
/*  <Description>                                                   */
/********************************************************************/
unsigned char MWUSB_GetConfigurationMode(void)
{
    return (Configuration_Mode_Status);
}



/****************************************************************************/
/*  <Function>  short MWUSB_GetMaxPacketSize                                */
/*  <Arguments>     unsigned char FIFO : Endpoint of index                  */
/*                          CNTL_READ_FIFO                                  */
/*                          CNTL_WRITE_FIFO                                 */
/*                          BULK_READ_FIFO                                  */
/*                          BULK_WRITE_FIFO                                 */
/*                          INTR_WRITE_FIFO                                 */
/*                          ISOC_READ_FIFO                                  */
/*                          ISOC_WRITE_FIFO                                 */
/*                          VEND_READ_FIFO                                  */
/*                          VEND_WRITE_FIFO                                 */
/*  <Return  >      Positive Value  : MaxPacketSize of specified endpoint   */
/*                  MWUSB_ERR_BADEP : Invalide Endpoint No                  */
/*  <Description> Get endpoint MaxPacketSize                                */
/****************************************************************************/
short MWUSB_GetMaxPacketSize(unsigned char FIFO)
{

    short MaxSize;

    if(FIFO == CNTL_READ_FIFO)
    {
        MaxSize = CntlOut_Trans.sizeOfpacket;
    }
    else if(FIFO == CNTL_WRITE_FIFO)
    {
        MaxSize = CntlOut_Trans.sizeOfpacket;
    }
    else if((BulkOut_Trans.stat == MWUSB_EP_USE) && (FIFO == BULK_READ_FIFO))
    {
        MaxSize = BulkOut_Trans.sizeOfpacket;
    }
    else if((VendOut_Trans.stat == MWUSB_EP_USE) && (FIFO == VEND_READ_FIFO))
    {
        MaxSize = VendOut_Trans.sizeOfpacket;
    }
    else if((IsocOut_Trans.stat == MWUSB_EP_USE) && (FIFO == ISOC_READ_FIFO))
    {
        MaxSize = IsocOut_Trans.sizeOfpacket;
    }
    else if((BulkIn_Trans.stat == MWUSB_EP_USE) && (FIFO == BULK_WRITE_FIFO))
    {
        MaxSize = BulkIn_Trans.sizeOfpacket;
    }
    else if((VendIn_Trans.stat == MWUSB_EP_USE) && (FIFO == VEND_WRITE_FIFO))
    {
        MaxSize = VendIn_Trans.sizeOfpacket;
    }
    else if((IntrIn_Trans.stat == MWUSB_EP_USE) && (FIFO == INTR_WRITE_FIFO))
    {
        MaxSize = IntrIn_Trans.sizeOfpacket;
    }
    else if((IsocIn_Trans.stat == MWUSB_EP_USE) && (FIFO == ISOC_WRITE_FIFO))
    {
        MaxSize = IsocIn_Trans.sizeOfpacket;
    }
    else
    {
        MaxSize = MWUSB_ERR_BADEP;
    }
    
    return MaxSize;
}



/************************************************************************/
/*  <Function>  void MWUSB_GetSpeed                                     */
/*  <Arguments>     unsigned long spped                                 */
/*                      Connecting speed                                */
/*                      MWUSB_HISPEED   : HiSpeed                       */
/*                      MWUSB_FULLSPEED : FullSpeed                     */
/*  <Return  >      void                                                */
/*  <Description> Set USB Connecting speed.                             */
/*                  If you will change USB connecting speed, yuu must   */
/*                  call this function before usb pliug attach.         */
/************************************************************************/
void MWUSB_SetReqSpeed(unsigned long speed)
{
    if ( (speed == MWUSB_FULLSPEED) ||
         (speed == MWUSB_HISPEED)) {
        ulReqSpeed = speed;
    } else {
        ulReqSpeed = MWUSB_HISPEED;
    }
}


/************************************************************************/
/*  <Function>  unsigned long MWUSB_GetReqSpeed                         */
/*  <Arguments>     void                                                */
/*  <Return  >      MWUSB_HISPEED   : HiSpeed                           */
/*                  MWUSB_FULLSPEED : FullSpeed                         */
/*  <Description> Get USB Connecting speed.                             */
/************************************************************************/
unsigned long MWUSB_GetReqSpeed(void)
{
    return ulReqSpeed;
}



/************************************************************************/
/*  <Function>  unsigned long MWUSB_GetSpeed                            */
/*  <Arguments>     void                                                */
/*  <Return  >      MWUSB_HISPEED   : HiSpeed                           */
/*                  MWUSB_FULLSPEED : FullSpeed                         */
/*  <Description> Current connected speed                               */
/************************************************************************/
unsigned long MWUSB_GetSpeed(void)
{
    unsigned long CrtSpeed;
    
    switch (Control_BusSpeed_setting) {
    case HDC_HIGH_SPEED:
        CrtSpeed = MWUSB_HISPEED;
        break;
    case HDC_FULL_SPEED:
        CrtSpeed = MWUSB_FULLSPEED;
        break;
    default:
        CrtSpeed = MWUSB_FULLSPEED;
        break;
    }
    
    return( CrtSpeed );
}



/************************************************************************/
/*  <Function>  void MWUSB_AbortReset                                   */
/*  <Arguments>     void                                                */
/*   <Return  >     void                                                */
/*  <Description>  Abort all Transfer                                   */
/************************************************************************/
void MWUSB_AbortReset(void)
{
    unsigned char emptyFlag = 0;
    int state;
#ifdef USB_DEBUG2
    printf(" MWUSB_AbortReset\n\r");
#endif
    
    /* Clear All Event Flag */
#ifdef USB_NO_REALOS
    usb_clr_flg( FID_USB , 0x00000000);
#else
    OS_User_Clr_Flg( FID_USB , 0x00000000);
#endif
    
    /* Abort DMA Transfer */
    if (HdcDev_GetDmaTransferredState(DMA_CH_BULK_IN) == HDC_TRN_ST_BUSY) {
        HdcDev_StopDmaTransfer(DMA_CH_BULK_IN);
        MWUSB_Abort_Dma(DMA_CH_BULK_IN);
    }
    if (HdcDev_GetDmaTransferredState(DMA_CH_BULK_OUT) == HDC_TRN_ST_BUSY) {
        HdcDev_StopDmaTransfer(DMA_CH_BULK_OUT);
        MWUSB_Abort_Dma(DMA_CH_BULK_OUT);
    }
    
    /* Stop CPU Transfer (IN) */
    if (BulkIn_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(BulkIn_Trans.epno, 1) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(BulkIn_Trans.epno, 1);
        }
    }
    if (VendIn_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(VendIn_Trans.epno, 1) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(VendIn_Trans.epno, 1);
        }
    }
    if (IntrIn_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(IntrIn_Trans.epno, 1) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(IntrIn_Trans.epno, 1);
        }
    }
    if (IsocIn_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(IsocIn_Trans.epno, 1) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(IsocIn_Trans.epno, 1);
        }
    }
    
    /* Stop CPU Transfer (OUT) and then Clear EpBuffer */
    if (BulkOut_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(BulkOut_Trans.epno, 0) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(BulkOut_Trans.epno, 0);
        }
        
        state = HdcDev_IsEpBufferEmpty(BulkOut_Trans.epno, &emptyFlag);
        if ((state == HDC_SUCCESS) && (emptyFlag == 0)) {
            (void)HdcDev_ClearEpBuffer(BulkOut_Trans.epno, 1);
        }
    }
    if (VendOut_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(VendOut_Trans.epno, 0) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(VendOut_Trans.epno, 0);
        }
        state = HdcDev_IsEpBufferEmpty(VendOut_Trans.epno, &emptyFlag);
        if ((state == HDC_SUCCESS) && (emptyFlag == 0)) {
            (void)HdcDev_ClearEpBuffer(VendOut_Trans.epno, 1);
        }
    }
    if (IsocOut_Trans.stat == MWUSB_EP_USE) {
        if (HdcDev_GetTransferredState(IsocOut_Trans.epno, 0) == HDC_TRN_ST_BUSY) {
            HdcDev_StopTransfer(IsocOut_Trans.epno, 0);
        }
        state = HdcDev_IsEpBufferEmpty(IsocOut_Trans.epno,&emptyFlag);
        if ((state == HDC_SUCCESS) && (emptyFlag == 0)) {
            (void)HdcDev_ClearEpBuffer(IsocOut_Trans.epno, 1);
        }
    }
    
    /* Initialize global variables */
    CntlTrans_phase          = USB_IDLE;
    CntlIn_Trans.sizeOfdata  = 0;
    CntlIn_Trans.phase       = USB_IDLE;
    CntlOut_Trans.sizeOfdata = 0;
    CntlOut_Trans.phase      = USB_IDLE;
    idint_ep0_read           = NULL;      /* reset interrupt function */
    
    BulkIn_Trans.sizeOfdata  = 0;
    BulkIn_Trans.phase       = USB_IDLE;
    BulkOut_Trans.sizeOfdata = 0;
    BulkOut_Trans.phase      = USB_IDLE;
    
    VendIn_Trans.sizeOfdata  = 0;
    VendIn_Trans.phase       = USB_IDLE;
    VendOut_Trans.sizeOfdata = 0;
    VendOut_Trans.phase      = USB_IDLE;
    
    IntrIn_Trans.sizeOfdata  = 0;
    IntrIn_Trans.phase       = USB_IDLE;
    
    IsocIn_Trans.sizeOfdata  = 0;
    IsocIn_Trans.phase       = USB_IDLE;
    IsocOut_Trans.sizeOfdata = 0;
    IsocOut_Trans.phase      = USB_IDLE;
    
    /*  For Tuning Mode */
//  BYPS_Phase = 0;             /* Read phase flag          */
    
    return;
}



/************************************************************************/
/*  <Function>  void MWUSB_IntDisconnect                                */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Execution of Disconnecting                            */
/************************************************************************/
void MWUSB_IntDisconnect(void)
{
    /* Low Level Driver is already initialized. */
    if (mwusb_InitializeDriver == MWUSB_OK) {
        mwusb_intdisconnect(0);
    }
#ifdef USE_LOW_POWER
    /* Clock of Device --> OFF */
    (void)HdcDev_PowerOffDevice();
    mwusb_PowerOn = MWUSB_NG;
#endif /* USE_LOW_POWER */
    return;
}


/************************************************************************/
/*  <Function>  void mwusb_intdisconnect                                */
/*  <Arguments> mode normal=0, only_enumeration=1 (not support)         */
/*  <Return  >  void                                                    */
/*  <Description> Execution of Disconnecting                            */
/************************************************************************/
static void mwusb_intdisconnect(unsigned char mode)
{
    int stat;
    
#ifdef  USE_UART_MESSAGE    
    printf("USB IntDisconnect. start.\n\r");
#endif
    
    /* Abort Bulk or Interrupt Transfer */
    MWUSB_ReleaseWaitBulkIO();
    
    /* change function state */
    Control_Func_Status = MWUSB_FUNC_DISABLE;
    
    /* Re-confirm Flag of Initialize */
    /* The driver may be terminated on the connect task   */
    /* due to the execution of MWUSB_ReleaseWaitBulkIO(). */
    stat = HDC_SUCCESS;
    if (mwusb_InitializeDriver == MWUSB_OK) {
        /* LLD UnInitialize */
        stat = HdcDev_UninitializeDriver();
        if ( stat == HDC_SUCCESS ) {
            /* Flag of Initialize --> OFF */
            mwusb_InitializeDriver = MWUSB_NG;
        }
    }
    if ( stat == HDC_SUCCESS ) {
        
        /* Set DisConnect Event Flag */
#ifdef USB_NO_REALOS
        usb_set_flg( FID_USB , FLG_USB_DisConnected);
#else
        OS_User_Set_Flg( FID_USB , FLG_USB_DisConnected);
#endif
        
        /* Call User Customize Function */
        MWUSBC_IntDisconnect();
        
    } else {
#ifdef  USE_UART_MESSAGE
        printf("  ERROR : HdcDev_UninitializeDriver. on MWUSB_IntDisonnect. \n\r");
#endif
    }
    
#ifdef  USE_UART_MESSAGE    
    printf("USB IntDisconnect. End.\n\r");
#endif
    
    return;
}



/********************************************************************/
/*  <Function>  int MWUSB_InitConnection                            */
/*  <Arguments>     void                                            */
/*  <Return  >      int 0:NormalEnd, Other:Error                    */
/*  <Description> Initialize USB Macro.                             */
/*                Initialize a FIFO Address Counter.                */
/*                Setting a ENDPOINT Buffer.                        */
/********************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_InitConnection( void )
{
#ifdef  USE_UART_MESSAGE    
    printf("USB InitConnectgion Start \n\r");
    OS_User_Tslp_Tsk(1); /* wait time 50msec */
#endif

    MWUSB_AbortReset();    /* abort transfer */
    MWUSB_IntDisconnect(); /* disconnect&Macro Reset */

    OS_User_Tslp_Tsk(1); /* wait time 1msec */
    /* USB function start , & go interrupt main roop */
    mwusb_initialize(0,0);

#ifdef  USE_UART_MESSAGE    
    printf("USB InitConnectgion End \n\r");
    OS_User_Tslp_Tsk(1); /* wait time 50msec */
#endif

    return;
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  long MWUSB_IntConnect                                   */
/*  <Arguments> void                                                    */
/*  <Return  >  MWUSB_PLUG_RELEASE : USB Plug Released                  */
/*              MWUSB_PLUG_CONNECT : USB Plug Connected                 */
/*  <Description> Execution of Connecting and Disconnecting             */
/************************************************************************/
long MWUSB_IntConnect(void)
{
    long con;
    /* Check Connect or Disconnect */

#ifdef  USE_UART_MESSAGE    
    printf(" MWUSB_IntConnect. Start.\n\r");
#endif
    
    con = mwusb_CheckConnect(0);
    if( con == MWUSB_PLUG_RELEASE )
    {
        /*  Plug Release */
        /*  Call Release function */
        MWUSB_IntDisconnect();

#ifdef  USE_UART_MESSAGE    
        printf("==>USB Released.\n\r");
#endif
    }
    else
    {
        /* Low Level Driver isn't initialized. */
        if (mwusb_InitializeDriver == MWUSB_NG)
        {
            /*  Plug Connected  */

            /* Call User Customize Function */
            MWUSBC_IntConnect();

            /*  Call Connect function */
            (void)MWUSB_PlugWait();

#ifdef USE_UART_MESSAGE
            printf("==>USB Connected.\n\r");
#endif
        }
    }
    return con;
}



/************************************************************************/
/*  <Function>  long MWUSB_PlugWait                                     */
/*  <Arguments> void                                                    */
/*  <Return  >  MWUSB_OK : Normal End                                   */
/*              MWUSB_NG : Error End                                    */
/*  <Description> Execution of Connecting                               */
/************************************************************************/
int MWUSB_PlugWait( void )
{
    int RetCode = MWUSB_NG;
    int ret = HDC_SUCCESS;
    unsigned char mode;
    
#ifdef USE_LOW_POWER
    /* Clock of Device --> ON */
    if (mwusb_PowerOn == MWUSB_NG) {
        ret = HdcDev_PowerOnDevice();
    }
    if (ret == HDC_SUCCESS) {
        mwusb_PowerOn = MWUSB_OK;
    }
#endif /* USE_LOW_POWER */
    if (ret == HDC_SUCCESS) {
        MWUSBC_PlugWait(); /* User Custom function add. 2011.04.04 */
        mode = MWUSB_GetConfigurationMode(); /* Get configuration mode */
        RetCode = mwusb_initialize(mode, 1);
#ifdef USE_LOW_POWER
        /* Clock of Device --> OFF */
        if (RetCode == MWUSB_NG) {
            (void)HdcDev_PowerOffDevice();
            mwusb_PowerOn = MWUSB_NG;
        }
#endif /* USE_LOW_POWER */
    }
    
    return  RetCode;
}



/************************************************************************/
/*  <Function>  long MWUSB_CheckConfig                                  */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Check Configured or Unconfigured                      */
/************************************************************************/
long MWUSB_CheckConfig(void)
{
    int stat;
    long rst;
    
    stat = HdcDev_GetDeviceState();

    switch(stat)
    {
    case DEVICE_STATE_CONFIGURED:
        rst = MWUSB_STAT_CONFIG;
        break;
    case DEVICE_STATE_SUSPEND:
        rst = MWUSB_STAT_SUSPEND;
        break;
    case DEVICE_STATE_POWERED:
        rst = MWUSB_STAT_BUSRESET;
        break;
    default:
        rst = MWUSB_STAT_UNCONF;
    }
    
    return( rst );
}



/************************************************************************/
/*  <Function>  unsigned long MWUSB_GetConfigurationNumber              */
/*  <Arguments> void                                                    */
/*  <Return  >  configuration number    >= 0    error < 0               */
/*  <Description> Get configuration number                              */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S21A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_GetConfigurationNumber(void)
{
    unsigned char hdcno;
    long stat;
    
    /* Get configuration number */
    stat = HdcDev_GetConfiguredNumber(&hdcno);

    if ( stat == 0 )
    {
        stat = (long)hdcno; /* normal */
    }
    else
    {
        stat = -1; /* error */
    }
    
    return( stat );
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  void IDint_SetConfiguration                             */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   SetConfiguration Interrupt Handler(ID17)            */
/*                  Get Configuration Number.                           */
/************************************************************************/
void IDint_SetConfiguration(void)
{
    unsigned char       confval = 0;

    InterID = SET_CONFIG_ID17; /* Interrupt ID */

#ifdef USE_UART_MESSAGE
    printf("  SET_CONFIGURATION top. DeviceState(%d)\n\r",HdcDev_GetDeviceState());
#endif
    (void)HdcDev_GetConfiguredNumber(&confval);

    if( confval == 0x00 )
    {   /* UnConfigured */
        Control_Func_Status = MWUSB_FUNC_UNCONF; /* Function status change */
        /* Clear Send Null Packet Request Flag  */
//      fReqBulk_Null = 0;
        fReqIntr_Null = 0;
//      fBulkAuto_Null = 0;
        fIntrAuto_Null = 0;

        /* Abort Bulk or Interrupt Transfer */
        MWUSB_AbortReset();

        DEND1_flg |= FLG_USB_UnConf;
        DEND2_flg |= FLG_USB_UnConf;
        DEND3_flg |= FLG_USB_UnConf;
        /* Set Unconf Event Flag */
#ifdef USB_NO_REALOS
        usb_set_flg( FID_USB ,  FLG_USB_UnConf );
        usb_clr_flg( FID_USB , ~FLG_USB_Config );
#else
        OS_User_Set_Flg( FID_USB ,  FLG_USB_UnConf );
        OS_User_Clr_Flg( FID_USB , ~FLG_USB_Config );
#endif

        MWUSBC_SetConfig(confval); /* set configured number */
    }
    else
    {
        Control_Func_Status = MWUSB_FUNC_CONFIG; /* Function status change */
        DEND1_flg &= ~(FLG_USB_UnConf|FLG_USB_BusReset);
        DEND2_flg &= ~(FLG_USB_UnConf|FLG_USB_BusReset);
        DEND3_flg &= ~(FLG_USB_UnConf|FLG_USB_BusReset);

        /* get configured descriptors */
        MWUSBC_DefineDescriptor();

        /* Clear EventFlag to UnConf,BusReset */
#ifdef USB_NO_REALOS
        usb_clr_flg( FID_USB , ~(FLG_USB_BusReset | FLG_USB_DisConnected |
                                 FLG_USB_UnConf | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                                 FLG_USB_Cancel | FLG_USB_TimeOut) );
        usb_set_flg( FID_USB , FLG_USB_Config);
#else
        OS_User_Clr_Flg( FID_USB , ~(FLG_USB_BusReset | FLG_USB_DisConnected |
                                     FLG_USB_UnConf | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                                     FLG_USB_Cancel | FLG_USB_TimeOut) );
        OS_User_Set_Flg( FID_USB , FLG_USB_Config );
#endif

        MWUSBC_SetConfig(confval); /* set configured number */

    }

#ifdef USE_UART_MESSAGE
    printf("  SET CONFIGURATION end. DeviceState(%d)\n\r",HdcDev_GetDeviceState());
#endif
    return;
}



/************************************************************************/
/*  <Function>  void IDint_Sof_Trans                                    */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Sof Interrupt Handler(ID42)                         */
/************************************************************************/
void IDint_Sof_Trans(void)
{
    InterID = SOF_GETPAK_ID42; /* Interrupt ID */

#ifdef USE_UART_MESSAGE
    printf("USB Sofpacket get \n\r");
    OS_User_Tslp_Tsk(1); /* wait time 100msec */
#endif
    /* Call User Customize function */
    MWUSBC_SoFRes();

    return;
}



/************************************************************************/
/*  <Function>  void IDint_BusReset_START                               */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   START BusReset Interrupt Handler(ID28)              */
/************************************************************************/
void IDint_BusReset_START(void)
{
    InterID = BUSRST_STR_ID28; /* Interrupt ID */
    
#ifdef USE_UART_MESSAGE
    printf("USB BusReset Start \n\r");
    OS_User_Tslp_Tsk(1); /* wait time 50msec */
#endif

    /* Release Bulk and Interrupt transfer wait */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_BusReset );
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_BusReset );
#endif
    
    /* Call User Customize function */
    MWUSBC_UsbBusResetStart();
    
    /* Set Bus Reset status */
    Control_BusReset_Status = BUS_RESET_DOING;
    
    Control_Func_Status     = MWUSB_FUNC_DISABLE;
    
    mwusb_dma_read_stat  = DMA_INIT_STAT;
    mwusb_dma_write_stat = DMA_INIT_STAT;
    
    return;
}



/************************************************************************/
/*  <Function>  void IDint_BusReset_END                                 */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   END BusReset Interrupt Handler(ID29)                */
/*                  Get BusSpeed.                                       */
/************************************************************************/
void IDint_BusReset_END(void)
{
    int speed;
    
    InterID = BUSRST_END_ID29; /* Interrupt ID */
    
    /* Set Bus Reset status */
    Control_BusReset_Status  = BUS_RESET_NONE;
    
    speed = HdcDev_GetBusSpeed();
    if ((speed == HDC_HIGH_SPEED) ||
        (speed == HDC_FULL_SPEED)) {
        /* Set Bus Speed */
        Control_BusSpeed_setting = speed;
        
        /* Common Setting for Trans_DATA  */
        CntlOut_Trans.outDataPoint = OutPacket_Buffer;
        BulkOut_Trans.outDataPoint = NULL;
        VendOut_Trans.outDataPoint = NULL;
        IsocOut_Trans.outDataPoint = NULL;
        
        CntlIn_Trans.inDataPoint   = NULL;
        BulkIn_Trans.inDataPoint   = NULL;
        VendIn_Trans.inDataPoint   = NULL;
        IntrIn_Trans.inDataPoint   = NULL;
        IsocIn_Trans.inDataPoint   = NULL;
        
#ifdef USE_UART_MESSAGE
        printf("USB BusReset END \n\r");
#endif
        switch (Control_BusSpeed_setting) {
        case HDC_HIGH_SPEED:
            CntlOut_Trans.sizeOfpacket = CONT_PACKET_SIZE_HI;
            CntlIn_Trans.sizeOfpacket  = CONT_PACKET_SIZE_HI;
            BulkOut_Trans.sizeOfpacket = BULK_PACKET_SIZE_HI;
            BulkIn_Trans.sizeOfpacket  = BULK_PACKET_SIZE_HI;
            VendOut_Trans.sizeOfpacket = BULK_PACKET_SIZE_HI;
            VendIn_Trans.sizeOfpacket  = BULK_PACKET_SIZE_HI;
            IntrIn_Trans.sizeOfpacket  = INTR_PACKET_SIZE_HI;
            IsocOut_Trans.sizeOfpacket = ISOC_PACKET_SIZE_HI;
            IsocIn_Trans.sizeOfpacket  = ISOC_PACKET_SIZE_HI;
#ifdef USE_UART_MESSAGE
            printf("  bus is Hi-speed \n\r");
#endif
            break;
        case HDC_FULL_SPEED:
            CntlOut_Trans.sizeOfpacket = CONT_PACKET_SIZE;
            CntlIn_Trans.sizeOfpacket  = CONT_PACKET_SIZE;
            BulkOut_Trans.sizeOfpacket = BULK_PACKET_SIZE;
            BulkIn_Trans.sizeOfpacket  = BULK_PACKET_SIZE;
            VendOut_Trans.sizeOfpacket = BULK_PACKET_SIZE;
            VendIn_Trans.sizeOfpacket  = BULK_PACKET_SIZE;
            IntrIn_Trans.sizeOfpacket  = INTR_PACKET_SIZE;
            IsocOut_Trans.sizeOfpacket = ISOC_PACKET_SIZE;
            IsocIn_Trans.sizeOfpacket  = ISOC_PACKET_SIZE;
#ifdef USE_UART_MESSAGE
            printf("  bus is Full-speed \n\r");
#endif
            break;
        default:
#ifdef USE_UART_MESSAGE
            printf("  undefined bus speed \n\r");
#endif
            break;
        }

#ifndef multi_configuration
        /* Set Descriptor */
        MWUSBC_DefineDescriptor();
#endif
        
        /* Call User Custom Function */
        MWUSBC_UsbBusResetEnd();
        
    }
    
    return;
}


/************************************************************************/
/*  <Function>  void IDint_Phy_Hungup                                   */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Phy Hungup Interrupt Handler(ID32)                  */
/************************************************************************/
void IDint_Phy_Hungup(void)
{
    InterID = PHY_HUNGUP_ID32; /* Interrupt ID */
    
    /* stop transfer */
    MWUSB_AbortReset();
    
    /* device reset */
    if (HdcDev_InitializeDevice() == HDC_FAILURE) {
#ifdef USE_UART_MESSAGE
        printf("  ERROR : Failed Device Initialized. on IDint_Phy_Hungup. \n\r"); 
#endif
        return;
    }
#ifdef USE_UART_MESSAGE
    printf("USB Phy hungup\n\r");
#endif
    
    Control_Func_Status  = MWUSB_FUNC_DISABLE;
    
    mwusb_dma_read_stat  = DMA_INIT_STAT;
    mwusb_dma_write_stat = DMA_INIT_STAT;
    
    MWUSBC_PhyHungup();
    
    return;
}


/************************************************************************/
/*  <Function>  void IDint_Vbus_Rise                                    */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   USB Plug-IN Interrupt Handler(ID72)                 */
/*                  Reset USB Macro.                                    */
/*                  POWER ext.Port reset.                               */
/*                  Change Interrupt detect level.                      */
/************************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void IDint_Vbus_Rise(void)
{
    InterID = VBUS_RISED_ID72; /* Interrupt ID */

    Control_Vbus_Status = MWUSB_PLUG_CONNECT;

#ifdef USB_NO_REALOS
    usb_clr_flg( FID_USB , ~FLG_USB_VbusRise);
    usb_set_flg( FID_USB ,  FLG_USB_VbusRise);
#else
    OS_User_Clr_Flg( FID_USB , ~FLG_USB_VbusRise);
    OS_User_Set_Flg( FID_USB ,  FLG_USB_VbusRise);
#endif

#ifdef  USE_UART_MESSAGE    
    printf("USB Vbus Rise\n\r");
    OS_User_Tslp_Tsk(1); /* wait time 20msec */
#endif
    return;
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  void IDint_Vbus_Fall                                    */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   USB Plug-OUT Interrupt Handler(ID73)                */
/*                  Reset USB Macro.                                    */
/*                  POWER ext.Port reset.                               */
/*                  Change Interrupt detect level.                      */
/************************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void IDint_Vbus_Fall(void)
{
    InterID = VBUS_FALLD_ID73; /* Interrupt ID */

    Control_Vbus_Status = MWUSB_PLUG_RELEASE;

#ifdef USB_NO_REALOS
    usb_clr_flg( FID_USB , ~FLG_USB_VbusFall);
    usb_set_flg( FID_USB ,  FLG_USB_VbusFall);
#else
    OS_User_Clr_Flg( FID_USB , ~FLG_USB_VbusFall);
    OS_User_Set_Flg( FID_USB ,  FLG_USB_VbusFall);
#endif
    
#ifdef  USE_UART_MESSAGE    
    printf("USB Vbus Fall\n\r");
    OS_User_Tslp_Tsk(1); /* wait time 20msec */
#endif
    return;
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  void IDint_Suspend_START                                */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Suspend Start Interrupt Handler(ID30)               */
/************************************************************************/
void IDint_Suspend_START(void)
{
    /* Call User Customize function */
    MWUSBC_SuspendStart();
    
    return;
}


/************************************************************************/
/*  <Function>  void IDint_Suspend_END                                  */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Suspend End Interrupt Handler(ID31)                 */
/************************************************************************/
void IDint_Suspend_END(void)
{
    /* Call User Customize function */
    MWUSBC_SuspendEnd();
    
    return;
}


/************************************************************************/
/*  <Function>  void IDint_Setup_CLSVND_Request                         */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Class Request Interrupt Handler(ID22)               */
/*  <Description>   Vendor Request Interrupt Handler(ID25)              */
/************************************************************************/
void IDint_Setup_CLSVND_Request(void)
{
    unsigned char RequestType;  /* 0=Standard,1=Class,2=Vendor Request */
    long rtn;
    short DataLen=0;
#if 0 /* M-7M (Endian change) */
    union {
        unsigned short In;
        unsigned char Out[2];
    } Data;
#endif /* M-7M */
    
    DEVICE_REQUEST request;
    
    /* read request */
    request = *((DEVICE_REQUEST *)HdcDev_GetDeviceRequest());
    
    {
        /* Length */
#if 0 /* M-7M (Endian change) */
        Data.In = request.wLength;
        request.wLength =  ((Data.Out[1] << 8) | Data.Out[0]);
        request.wIndex = ((request.wIndex >> 8) | (request.wIndex << 8)) ;
        request.wValue = ((request.wValue >> 8) | (request.wValue << 8)) ;
#endif /* M-7M */
    
        /* copy request */
        memcpy((void*)&DeviceRequest, (void*)&request, sizeof(DEVICE_REQUEST));
        
        /* Get Reqest Type */
        RequestType = (request.bmRequestType & 0x60 ) >> 5;
        
        if ((request.bmRequestType & 0x80) == 0x80) {
            /* In transfer */
            switch (RequestType) {
            case CLASS_REQUEST :
                InterID = SETUP_CLAS_ID22; /* Interrupt ID */
                rtn = MWUSBC_ClassRequest(&request, InPacket_Buffer, &DataLen);
                break ;
            case VENDOR_REQUEST :
                InterID = SETUP_VEND_ID25; /* Interrupt ID */
                rtn = MWUSBC_VendorRequest(&request, InPacket_Buffer, &DataLen);
                break ;
            default:    /* Reserved */
                /* Error */
                rtn = MWUSB_NG;
                break ;
            }
            
            if (rtn == MWUSB_OK) {
                CntlIn_Trans.sizeOfdata  = DataLen;
                CntlIn_Trans.inDataPoint = InPacket_Buffer;
                CntlTrans_phase          = USB_DATAIN_STAGE;
                
                /* UnMask IntReady0i Interrupt */
                (void)In_Interrupt_UnMask(CntlIn_Trans.epno);
                
            } else {
                CntlTrans_phase = USB_IDLE;
                mwusb_EPStalled(CntlIn_Trans.epno);
            }
            
        } else {
            /* Out transfer */
            if (request.wLength > 0) {
                /* Exist Data Stage */
                CntlOut_Trans.sizeOfdata   = request.wLength;
                CntlOut_Trans.outDataPoint = OutPacket_Buffer;
                CntlTrans_phase            = USB_DATAOUT_STAGE;
                /* UnMask IntReady0 Interrupt */
                Out_Interrupt_UnMask(CntlOut_Trans.epno);
            } else {
                switch (RequestType) {
                case CLASS_REQUEST :
                    InterID = SETUP_CLAS_ID22; /* Interrupt ID */
                    rtn = MWUSBC_ClassRequest(&request, NULL, &DataLen);
                    break ;
                case VENDOR_REQUEST :       /* Vendor Request */
                    InterID = SETUP_VEND_ID25; /* Interrupt ID */
                    rtn = MWUSBC_VendorRequest(&request, NULL, &DataLen);
                    break ;
                default :                   /* Reserved */
                    /* Error */
                    rtn = MWUSB_NG;
                    break ;
                }
                
                if (rtn == MWUSB_OK) {
                    /* status stage(null packet in-transfer) */
                    CntlIn_Trans.sizeOfdata  = 0;
                    CntlIn_Trans.inDataPoint = NULL;
                    CntlTrans_phase = USB_STATUSOUT_STAGE;
                    
                    /* UnMask IntReady0i Interrupt */
                    (void)In_Interrupt_UnMask(CntlIn_Trans.epno);
                    
                } else {
                    CntlTrans_phase = USB_IDLE;
                    mwusb_EPStalled(CntlIn_Trans.epno);
                }
            }
        }
    }
    
    return;
}


/************************************************************************/
/*  <Function>  tatic unsigned long SetNullPacket                       */
/*  <Arguments> IN_TRAN_DATA    In_Trans                                */
/*  <Return  >  Null Packet send Flag                                   */
/*  <Description>   on the max packet alignment is 1, another 0         */
/************************************************************************/
static unsigned long SetNullPacket(IN_TRAN_DATA * In_Trans)
{
    unsigned long null_pkt = 0;
    
    if( (In_Trans->sizeOfdata > 0) && (fFIFOAuto_Null == 1) )
    {
        if( (In_Trans->sizeOfdata % In_Trans->sizeOfpacket) == 0 )
        {
            null_pkt = 1;
        }
    }
    return( null_pkt );
}


/****************************************************************************/
/*  <Function>  unsigned short MakeInFIFO                                   */
/*  <Arguments>     IN_TRAN_DATA *data                                      */
/*                      Pointer to Transaction Management Data              */
/*  <Return  >      unsigned short      size of sent data                   */
/*  <Description>   Control the transaction management data.                */
/*                  Call function to write FIFO.                            */
/*  <edit>          2011.01.04 j.tomida                                     */
/*                  delete case to sizeOfdata grater than sizeOfpacket.     */
/*                  add wait for IntReady interrupt flag.                   */
/****************************************************************************/
long MakeInFIFO(IN_TRAN_DATA *data)
{
    int stat,retry;
    long size;
    unsigned long  pTransByte = 0;
    unsigned int   flg_ptn = 0;
    OS_USER_FLGPTN rtn_ptn;
    
    /* check send complete */
    if (data->sizeOfdata <= 0) 
    {
        return(0) ; /* Send Complete*/
    }
    
    {
        /* Clear old event flag */
        if (data->epno == BulkIn_Trans.epno) {
            flg_ptn = FLG_USB_BulkWriteRdy;
        } else if (data->epno == IntrIn_Trans.epno) {
            flg_ptn = FLG_USB_IntrWriteRdy;
        } else if (data->epno == VendIn_Trans.epno) {
            flg_ptn = FLG_USB_VendWriteRdy;
        }
        if (data->epno != 0) {
#ifdef USB_NO_REALOS
            usb_clr_flg(FID_USB, ~(flg_ptn));
#else
            OS_User_Clr_Flg(FID_USB, ~(flg_ptn));	//pgr0000
#endif
        }

        /* transfer memory to EPbuffer */
        fReqCntl_Null=SetNullPacket(data); /* set send null packet */
        stat = HdcDev_SetInTransferData(data->epno, data->inDataPoint,
                                        data->sizeOfdata, fReqCntl_Null);
        /* add retry state 2010.12.09 tomida */
        if( stat != 0 )
        {
            retry=MWUSB_WRITE_RETRY_MAX;
            while(retry > 0 )
            {
//              OS_User_Tslp_Tsk(MWUSB_WRITE_RETRY_WAIT); /* retry wait */
                if (data->epno == BulkIn_Trans.epno) {
                    if (OS_User_Twai_Flg(FID_USB, FLG_USB_ClassReset, D_OS_USER_TWF_ORW,
                        &rtn_ptn, MWUSB_WRITE_RETRY_WAIT) == D_OS_USER_E_OK) {
                        /* Class reset occurred */
                        return(0);
                    }
                } else if (data->epno != CntlIn_Trans.epno) {
                    OS_User_Tslp_Tsk(MWUSB_WRITE_RETRY_WAIT); /* retry wait */
                }
                stat = HdcDev_SetInTransferData(data->epno, data->inDataPoint,
                                                data->sizeOfdata, fReqCntl_Null);
                if( stat == 0 )
                {
#ifdef  USE_UART_MESSAGE
                    printf("    <<MWUSB4>> MakeInFIFO WRITE. wait time %d(ms).\n\r",
                               (MWUSB_WRITE_RETRY_MAX+1-retry)*MWUSB_WRITE_RETRY_WAIT);
#endif
                    break;
                }
                retry--;
            }
            if(stat != 0) {
#ifdef  USE_UART_MESSAGE
                printf("    <<MWUSB4>> MakeInFIFO ERROR(%d).\n\r",stat);
#endif
                global_in_ptn = FLG_USB_ClassTerm; /* error transfer */
                return( 0 );  /* Retrun value is 0 for error case */
            }
        }
        /* Transfer Length less equal to Endopint Pakcet Size */
        /* Set Data management in order to Retransmit */
        size = data->sizeOfdata ;
    
        /* Clear Endpoint management structure member(sizeOfdata) */
        data->sizeOfdata = 0 ;
    }

    /* add wait IntReadyi interrupt flag. 2011.1.4 by tomida */
    if( size > data->sizeOfpacket )
    {
        if( (data->epno != CntlIn_Trans.epno) && ((InterID == BULK_ENDP1_ID00) || (InterID == VEND_ENDP5_ID04) || (InterID == INTR_ENDP3_ID02)) )
        {
            if( InterID == BULK_ENDP1_ID00 )
            {
                flg_ptn   = FLG_USB_BulkWriteRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
                            FLG_USB_ClassTerm | FLG_USB_ClassReset |FLG_USB_Cancel |
                            FLG_USB_TimeOut | FLG_USB_BusReset;
            }
            else if( InterID == INTR_ENDP3_ID02 )
            {
                flg_ptn   = FLG_USB_IntrWriteRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
                            FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                            FLG_USB_TimeOut | FLG_USB_BusReset;
            }
            else if( InterID == VEND_ENDP5_ID04 )
            {
                flg_ptn   = FLG_USB_VendWriteRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
                            FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                            FLG_USB_TimeOut | FLG_USB_BusReset;
            }
#ifdef USB_NO_REALOS
            usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
            usb_clr_flg(FID_USB, ~(flg_ptn));
#else
            OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);//pgr0000
            OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
            /* add global return flag 2011.02.03 tomida */
            global_in_ptn = rtn_ptn;
            
            if ((rtn_ptn & FLG_USB_BulkWriteRdy ) || (rtn_ptn & FLG_USB_IntrWriteRdy ) ||(rtn_ptn & FLG_USB_VendWriteRdy )){
                ;
            } else { //error
                return (0) ;
            }

            /* check transferred data size */
            HdcDev_GetTransferredDataSize(data->epno, 1, &pTransByte );
            if( pTransByte < size )
            {
                retry=MWUSB_WRITE_RETRY_MAX;
                while(retry > 0 )
                {
                    if( HdcDev_GetDeviceState() == DEVICE_STATE_SUSPEND )
                    {
                        /* break at suspend. 2012.1.10 */
                        break;
                    }
                    OS_User_Tslp_Tsk(1);
                    HdcDev_GetTransferredDataSize(data->epno, 1, &pTransByte );
                    if( pTransByte >= size )break;
                    retry--;
                }
                if( retry == 0 )
                {
                    global_in_ptn = FLG_USB_ClassTerm; /* error transfer */
                    return( 0 );  /* Retrun value is 0 for error case */
                }
            }
        }
    }
    else
    {
        if( (data->epno != CntlIn_Trans.epno) && ((InterID == BULK_ENDP1_ID00) || (InterID == VEND_ENDP5_ID04) || (InterID == INTR_ENDP3_ID02)) )
        {
            if( InterID == BULK_ENDP1_ID00 )
            {
                flg_ptn   = FLG_USB_BulkWriteRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
                            FLG_USB_ClassTerm | FLG_USB_ClassReset |FLG_USB_Cancel |
                            FLG_USB_TimeOut | FLG_USB_BusReset;
            }
            else if( InterID == INTR_ENDP3_ID02 )
            {
                flg_ptn   = FLG_USB_IntrWriteRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
                            FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                            FLG_USB_TimeOut | FLG_USB_BusReset;
            }
            else if( InterID == VEND_ENDP5_ID04 )
            {
                flg_ptn   = FLG_USB_VendWriteRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
                            FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                            FLG_USB_TimeOut | FLG_USB_BusReset;
            }
#ifdef USB_NO_REALOS
            usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
            usb_clr_flg(FID_USB, ~(flg_ptn));
#else
            OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);//pgr0000
            OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
            /* add global return flag 2011.02.03 tomida */
            global_in_ptn = rtn_ptn;
            
            if ((rtn_ptn & FLG_USB_BulkWriteRdy ) || (rtn_ptn & FLG_USB_IntrWriteRdy ) ||(rtn_ptn & FLG_USB_VendWriteRdy )){
                ;
            } else { //error
                return (0) ;
            }

            /* check transferred data size */
            HdcDev_GetTransferredDataSize(data->epno, 1, &pTransByte );
            if( pTransByte < size )
            {
                retry=MWUSB_WRITE_RETRY_MAX;
                while(retry > 0 )
                {
                    OS_User_Tslp_Tsk(1);
                    HdcDev_GetTransferredDataSize(data->epno, 1, &pTransByte );
                    if( pTransByte >= size )break;
                    retry--;
                }
                if( retry == 0 )
                {
                    global_in_ptn = FLG_USB_ClassTerm; /* error transfer */
                    return( 0 );
                }
            }
        }
    }

    return(size) ;
}


/************************************************************************/
/*  <Function>  static short ControlFIFORead                            */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   Read Control OUT Data from Control OUT Endpoint     */
/************************************************************************/
static short ControlFIFORead(void)
{
    /* unsigned long sizeOfdata ; */
    short           sizeOfdata = 0;
    unsigned short  packetSize = 0;
    unsigned long   readsize = 0;
    unsigned long   pTransByte;
    int             retry;
    int             rcd=-1;

    /* Get Receive Length */
    rcd = HdcDev_GetOutTransactionDataSize(CntlOut_Trans.epno,&packetSize);
    if( rcd == -1 )
    {
        return( MWUSB_NG ); /* ERROR(-1) */
    }
    
    if( packetSize == 0 )
    {   /* Receive NullPacket */
        return 0;
    }
    else
    {
        readsize = (unsigned long)CntlOut_Trans.sizeOfdata;
        CntlOut_Trans.sizeOfdata = 0;
    }
    
    if( readsize > 0 )
    {
        /* read data */
        rcd = HdcDev_GetOutTransferData( CntlOut_Trans.epno,
                                         CntlOut_Trans.outDataPoint, 
                                         readsize );
        if( rcd == HDC_FAILURE )
        {
            return( MWUSB_NG ); /* ERROR(-1) */
        }
        
        /* wait for transfer */
        rcd = HdcDev_GetTransferredState( CntlOut_Trans.epno, 0 );
        retry = 0;
        while ((rcd == HDC_TRN_ST_BUSY) && (retry < MWUSB_READ_RETRY_WAIT))
        {
            rcd = HdcDev_GetTransferredState( CntlOut_Trans.epno, 0 );
            retry++;
        }
        if (rcd != HDC_TRN_ST_NORMAL) {
            return( MWUSB_NG ); /* ERROR(-1) */
        }
        CntlOut_Trans.outDataPoint += readsize;

        /* check transferred data size */
        rcd = HdcDev_GetTransferredDataSize( CntlOut_Trans.epno, 0, &pTransByte );
        if( rcd != HDC_SUCCESS )
        {
            sizeOfdata = MWUSB_NG;
        }
        else
        {
            if( sizeOfdata != pTransByte )
            {
#ifdef  USE_UART_MESSAGE
                printf(" ERROR : Control Read. Data Size(%ld) is not Request Size(%d).\n\r",pTransByte,sizeOfdata);
#endif
            }
            sizeOfdata = (unsigned short)pTransByte;
        }
    }
    return sizeOfdata;
    
}



/************************************************************************/
/*  <Function>  void IDint_EP0_Read                                     */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   EP0 Read Interrupt Handler(ID24,ID27)               */
/************************************************************************/
void IDint_EP0_Read(void)
{
    unsigned short datalen = 0;
    short DataLen;
    unsigned char RequestType = 0;
    long rtn;
    
    switch( CntlTrans_phase )
    {
    case USB_SETUP_STAGE :      /* Idle */
        CntlTrans_phase = USB_IDLE;
        break;
    case USB_DATAIN_STAGE :     /* Control IN Data Stage */
        /* Get Receive Length */
        HdcDev_GetOutTransactionDataSize(CntlOut_Trans.epno,&datalen);
        /* Init FIFO 0i */
        HdcDev_ClearEpBuffer(CntlIn_Trans.epno, 1);
        /* Clear Null Packet Send Request */
        fReqCntl_Null = 0;
        /* Mask IntReady0i Interrupt */
        /* (void)HdcDev_SetEpIntDisable(CntlIn_Trans.epno, 0x1); */
        CntlTrans_phase = USB_IDLE;
        CntlIn_Trans.sizeOfdata = 0;
        CntlIn_Trans.inDataPoint = NULL;
        if( datalen != 0 )
        {   /* Bad Data */
            /* Stall EP0 */
            mwusb_EPStalled(CntlIn_Trans.epno);
        }
        break;
    case USB_DATAOUT_STAGE :    /* Control OUT Data Stage */
        /* Receive Control OUT Data */
        DataLen = ControlFIFORead();
        //if( datalen < CntlOut_Trans.sizeOfpacket )
        if( DataLen >= 0 )
        {   /* Data Transfer Complete */
            DataLen = (short)DeviceRequest.wLength - CntlOut_Trans.sizeOfdata;
            RequestType = (DeviceRequest.bmRequestType & 0x60 ) >> 5;
            switch( RequestType )
            {
            case STANDARD_REQUEST :     /* Standard Request */
                /* No valid request */
                rtn = MWUSB_NG;
                break;
            case CLASS_REQUEST :    /* Class Request */
                InterID = SETUP_CLAS_ID22; /* Interrupt ID */
                rtn = MWUSBC_ClassRequest(&DeviceRequest, OutPacket_Buffer, &DataLen);
                break ;
            case VENDOR_REQUEST :       /* Vendor Request */
                InterID = SETUP_VEND_ID25; /* Interrupt ID */
                rtn = MWUSBC_VendorRequest(&DeviceRequest, OutPacket_Buffer, &DataLen);
                break ;
            default :                   /* Reserved */
                /* Error */
                rtn = MWUSB_NG;
            }
            if( rtn == MWUSB_OK )
            {
                CntlTrans_phase = USB_STATUSOUT_STAGE;
            }
            else
            {
                CntlTrans_phase = USB_STATUSOUT_STAGE;
                /* Reset interrupt function */
                idint_ep0_read = NULL;
                /* Stall EP0 */
                mwusb_EPStalled(CntlOut_Trans.epno);
            }
        }
        else
        {
            /* packet read error */
            CntlTrans_phase = USB_IDLE;
            CntlOut_Trans.sizeOfdata = 0;
            CntlOut_Trans.outDataPoint = NULL;
            /* Reset interrupt function */
            idint_ep0_read = NULL;
            /* Stall EP0 */
            mwusb_EPStalled(CntlOut_Trans.epno);
        }
        break;
    case USB_STATUSIN_STAGE :   /* Control IN Status Stage */
        /* Get Receive Length */
        HdcDev_GetOutTransactionDataSize(CntlOut_Trans.epno,&datalen);
        CntlTrans_phase = USB_IDLE;
        CntlIn_Trans.sizeOfdata = 0;
        CntlIn_Trans.inDataPoint = NULL;
        if( (datalen != 0)&&( InterID == SETUP_CLAS_ID22 || InterID == SETUP_VEND_ID25 ) )
        {   /* Bad Data */
            /* Reset interrupt function */
            idint_ep0_read = NULL;
            /* Stall EP0 */
            mwusb_EPStalled(CntlOut_Trans.epno);
        }
        break;
    case USB_STATUSOUT_STAGE :  /* Control OUT Status Stage */
        /* Invalid Data */
        CntlTrans_phase = USB_IDLE;
        CntlOut_Trans.sizeOfdata = 0;
        CntlOut_Trans.outDataPoint = NULL;
        /* Reset interrupt function */
        idint_ep0_read = NULL;
        /* Stall EP0 */
        mwusb_EPStalled(CntlOut_Trans.epno);
        break;
    }
    
    return;
}



/************************************************************************/
/*  <Function>  void IDint_EP0_Write                                    */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   IDint_EP0_Write Interrupt Handler(ID23,ID26)        */
/************************************************************************/
void IDint_EP0_Write(void)
{
    long datalen;

#if 0
    printf("    Control Write. Phase.\n\r");
    OS_User_Tslp_Tsk(10); /* wait time 100msec */
#endif

    fFIFOAuto_Null = 1; /** auto null packet send **/

    switch( CntlTrans_phase )
    {
    case USB_DATAIN_STAGE :     /* Control IN Data Stage */
        if( CntlIn_Trans.sizeOfdata > 0 )
        {   /* Exsist Not Send Data */
            /* Send Data */
#if 0
            printf("    Control Write. Start.\n\r");
            OS_User_Tslp_Tsk(10); /* wait time 100msec */
#endif
            datalen = MakeInFIFO(&CntlIn_Trans);
            if( datalen < CntlIn_Trans.sizeOfpacket )
            {   /* Short Packet Sending */
                CntlTrans_phase = USB_STATUSIN_STAGE;
                /* Mask IntReady0i Interrupt */
                /* nop */
            }
#if 0
            else
            {   /* MaxPacketSize Sending */
                if( CntlIn_Trans.sizeOfdata <= 0 )
                {   /* No data rest */
                    fReqCntl_Null = 1;
                }
            }
#endif
#if 0
            printf("    Control Write. End.\n\r");
            OS_User_Tslp_Tsk(1); /* wait time 100msec */
#endif
        }
        else
        {   /* No data rest */
            CntlTrans_phase = USB_STATUSIN_STAGE;
            /* Mask IntReady0i Interrupt */
            /* nop */
            if( fReqCntl_Null == 1 )
            {
                /* Send Null Packet */
                HdcDev_SetInTransferData(CntlIn_Trans.epno, CntlIn_Trans.inDataPoint,
                                         CntlIn_Trans.sizeOfdata, fReqCntl_Null);
                fReqCntl_Null = 0;
            }
        }
        break;
    case USB_STATUSOUT_STAGE :      /* Control OUT Status Stage */
        CntlTrans_phase = USB_IDLE;
        /* Mask IntReady0i Interrupt */
        /* nop */
        /* Send Null Packet */
        HdcDev_SetInTransferData(CntlIn_Trans.epno,
                                 CntlIn_Trans.inDataPoint, 0, 1);
        break;
#if 0
    default :                       /* Invalid phase */
        /* Mask IntReady0i Interrupt */
        /* nop */
#endif
    }
    
    return;
}



/************************************************************************/
/*  <Function>  IDintBulk_Write                                         */
/*  <Arguments> IN_TRAN_DATA * In_Trans                                 */
/*  <Return  >  void                                                    */
/*  <Description>   IntReady1 Interrupt Handler                         */
/*                  Bulk IN CPU Write                                   */
/************************************************************************/
static void IDintBulk_Write(IN_TRAN_DATA * In_Trans)
{
    long datalen;
    //int   stat;

    fFIFOAuto_Null = 0; /** auto null packet send off **/
    
    switch( In_Trans->phase )
    {
    case USB_IDLE :
        /* Mask IntReady1 Interrupt */
        //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
        break;
    case USB_SEND :
        if( In_Trans->sizeOfdata > 0 )
        {   /* Exist rest data */
            datalen = MakeInFIFO(In_Trans);
            if( datalen < In_Trans->sizeOfpacket )
            {   /* Short Packet Sending : Send Complete */
                In_Trans->phase = USB_IDLE;
                In_Trans->inDataPoint = NULL;
                In_Trans->sizeOfdata = 0;
                /* Mask IntReady1 Interrupt */
                //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                /* Mask EP1 Interrupt */
                // _USB_EpIC |= _bEP1_MASK_FLG;
            }
            else
            {   /* MaxPacketSize Sending */
                if( In_Trans->sizeOfdata <= 0)
                {   /* No data rest */
                    if( fBulkAuto_Null == 1 )
                    {   // Require Send Null
                        fReqBulk_Null = 1;
                        In_Trans->sizeOfdata = 0;
                    }
                    else
                    {   // Transfer Complete
                        In_Trans->phase = USB_IDLE;
                        In_Trans->inDataPoint = NULL;
                        In_Trans->sizeOfdata = 0;
                        /* Mask IntReady1 Interrupt */
                        //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                        /* Mask EP1 Interrupt */
                        // _USB_EpIC |= _bEP1_MASK_FLG;
                    }
                }
            }
        }
        else
        {   /* No data rest */
            if( fReqBulk_Null == 1 )
            {   // Send Null
                In_Trans->phase = USB_IDLE;
                In_Trans->inDataPoint = NULL;
                In_Trans->sizeOfdata = 0;
                HdcDev_SetInTransferData(In_Trans->epno, In_Trans->inDataPoint,
                                         In_Trans->sizeOfdata,fReqBulk_Null);
                /* Mask IntReady1 Interrupt */
                //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                /* Mask EP1 Interrupt */
                // _USB_EpIC |= _bEP1_MASK_FLG;
                // _USB_EpS1 = _USB_EpS1_READ | _bEPS1_READY_FLG;
                fReqBulk_Null = 0;
                /* add global return flag */
                if( InterID == BULK_ENDP1_ID00 )
                {
                    global_in_ptn = FLG_USB_BulkWriteRdy;
                }
                else if( InterID == VEND_ENDP5_ID04 )
                {
                    global_in_ptn = FLG_USB_VendWriteRdy;
                }
            }
        }
        break;
    default :   /* May be Bug */
        /* Mask IntReady1 Interrupt */
        //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
        In_Trans->phase = USB_IDLE;
    }
    
    return;
}



/************************************************************************/
/*  <Function>  void IDintByps_Write                                    */
/*  <Arguments> IN_TRAN_DATA * In_Trans                                 */
/*  <Return  >  void                                                    */
/*  <Description>   IntEP1write Interrupt Handler for ByPass Mode       */
/*                  Bulk IN CPU Write                                   */
/************************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
static void IDintByps_Write(IN_TRAN_DATA * In_Trans)
{
    long datalen;
    unsigned int   DataLen;
    unsigned char *pInTransData;
    unsigned long  inTransByte;
    unsigned char  nullTransFlag = 0;
    int stat;

    fFIFOAuto_Null = 0; /** auto null packet send **/

    if( BULKIN_2pkt == 1 )
    {   // Require Send Length
        DataLen = In_Trans->sizeOfdata;
        pInTransData = (unsigned char *)&DataLen;
        inTransByte = 4;
        BULKIN_2pkt = 0;
        /* Send Data */
        stat = HdcDev_SetInTransferData(In_Trans->epno, pInTransData,
                                        inTransByte, nullTransFlag);
        if( stat != 0 )
        {
            /* Error Set */
            In_Trans->phase = USB_IDLE;
            /* Stall EP1 */
            mwusb_EPStalled(In_Trans->epno);
        }
        global_in_ptn = FLG_USB_BulkWriteRdy;
    }
    else
    {   // Data Send
        if( In_Trans->sizeOfdata > 0 )
        {   // Exist data to send
            datalen = MakeInFIFO(In_Trans);
            if( datalen < In_Trans->sizeOfpacket )
            {   /* Short Packet Sending : Send Complete */
                In_Trans->phase = USB_IDLE;
                In_Trans->inDataPoint = NULL;
                In_Trans->sizeOfdata = 0;
                /* Mask IntReady1 Interrupt */
                //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                /* Mask EP1 Interrupt */
                // _USB_EpIC |= _bEP1_MASK_FLG;
            }
            else
            {   /* MaxPacketSize Sending */
                if( In_Trans->sizeOfdata <= 0)
                {   /* No data rest */
                    // Transfer Complete
                    In_Trans->phase = USB_IDLE;
                    In_Trans->inDataPoint = NULL;
                    In_Trans->sizeOfdata = 0;
                    /* Mask IntReady1 Interrupt */
                    //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                    /* Mask EP1 Interrupt */
                    // _USB_EpIC |= _bEP1_MASK_FLG;
                }
            }
        }
        else
        {
            /* Mask IntReady1 Interrupt */
            //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
            In_Trans->phase = USB_IDLE;
        }
    }
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  IDint_Bulk_Write                                        */
/*  <Arguments> IN_TRAN_DATA * In_Trans                                 */
/*  <Return  >  void                                                    */
/*  <Description>   IntReady1 Interrupt Handler                         */
/*                  Bulk IN CPU Write                                   */
/************************************************************************/
void IDint_Bulk_Write(IN_TRAN_DATA * In_Trans)
{
    if(BYPS_Mode == MWUSB_NORMAL_MODE)
    {
#if 0
        printf(" IDintBulk_Write.\n\r");
        OS_User_Tslp_Tsk(1);
#endif
        IDintBulk_Write(In_Trans); /* Normal Mode Interrupt */
    }
    else
    {
#if 0
        printf(" IDintByps_Write.\n\r");
        OS_User_Tslp_Tsk(1);
#endif
#if 0 /* M-7M */
        IDintByps_Write(In_Trans); /* Bypass Mode Interrupt */
#endif /* M-7M */
    }
    return;
}



/************************************************************************/
/*  <Function>  IDint_Intr_Write                                        */
/*  <Arguments> IN_TRAN_DATA * In_Trans                                 */
/*  <Return  >  void                                                    */
/*  <Description>   IntReady3 Interrupt Handler                         */
/*                  Bulk or Interrupt IN CPU Write                      */
/************************************************************************/
void IDint_Intr_Write(IN_TRAN_DATA * In_Trans)
{
    long datalen;

    InterID = INTR_ENDP3_ID02; /* Interrupt ID */
#if 0
    printf(" IDintBulk_Write.\n\r");
    OS_User_Tslp_Tsk(1);
#endif

    switch( In_Trans->phase )
    {
    case USB_IDLE :
        /* Mask IntReady1 Interrupt */
        //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
        break;
    case USB_SEND :
        if( In_Trans->sizeOfdata > 0 )
        {   /* Exist rest data */
            datalen = MakeInFIFO(In_Trans);
            if( datalen < In_Trans->sizeOfpacket )
            {   /* Short Packet Sending : Send Complete */
                In_Trans->phase = USB_IDLE;
                In_Trans->inDataPoint = NULL;
                In_Trans->sizeOfdata = 0;
                /* Mask IntReady3 Interrupt */
                //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                /* Mask EP3 Interrupt */
                /* Set IntrWriteReady Event Flag */
            } 
            else
            {   /* MaxPacketSize Sending */
                if( In_Trans->sizeOfdata <= 0)
                {   /* No data rest */
                    if( fIntrAuto_Null == 1 )
                    {   // Require Send Null
                        fReqIntr_Null = 1;
                        In_Trans->sizeOfdata = 0;
                    }
                    else
                    {   // Transfer Complete
                        In_Trans->phase = USB_IDLE;
                        In_Trans->inDataPoint = NULL;
                        In_Trans->sizeOfdata = 0;
                        /* Mask IntReady3 Interrupt */
                        //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                        /* Mask EP3 Interrupt */
                        /* Set IntrWriteReady Event Flag */
                    }
                }
            }
        }
        else
        {   /* No data rest */
            if( fReqIntr_Null == 1 )
            {   // Send Null
                In_Trans->phase = USB_IDLE;
                In_Trans->inDataPoint = NULL;
                In_Trans->sizeOfdata = 0;
                HdcDev_SetInTransferData(In_Trans->epno, In_Trans->inDataPoint,
                                         In_Trans->sizeOfdata,fReqBulk_Null);
                /* Mask IntReady1 Interrupt */
                //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
                /* Mask EP1 Interrupt */
                // _USB_EpIC |= _bEP1_MASK_FLG;
                // _USB_EpS1 = _USB_EpS1_READ | _bEPS1_READY_FLG;
                fReqIntr_Null = 0;
                /* Set IntrWriteReady Event Flag */
                global_in_ptn = FLG_USB_IntrWriteRdy;
            }
        }
        break;
    default :   /* May be Bug */
        /* Mask IntReady1 Interrupt */
        //HdcDev_SetEpIntDisable(In_Trans->epno, 0x1);
        In_Trans->phase = USB_IDLE;
    }
    
    return;
}



/************************************************************************/
/*  <Function>  IDint_EPbuf_Empty                                       */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   IntReady1 Interrupt Handler                         */
/*                  Bulk IN CPU Write                                   */
/************************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void IDint_EPbuf_Empty(void)
{
    unsigned char epno;
    InterID = EPBF_EMPTY_ID46; /* Interrupt ID */
    epno = HdcDev_GetInterruptEp();
    if( (epno == BulkIn_Trans.epno) && (BulkIn_Trans.stat == MWUSB_EP_USE) )
    {
#ifdef  USE_UART_MESSAGE
        printf(" IDint_Bulk_Write(IDint_EPbuf_Empty).\n\r");
        OS_User_Tslp_Tsk(1);
#endif
        InterID = BULK_ENDP1_ID00; /* Interrupt ID */
        IDint_Bulk_Write(&BulkIn_Trans);
    }
    else if( (epno == VendIn_Trans.epno) && (VendIn_Trans.stat == MWUSB_EP_USE) )
    {
#ifdef  USE_UART_MESSAGE
        printf(" IDint_Bulk_Write(IDint_EPbuf_Empty).\n\r");
        OS_User_Tslp_Tsk(1);
#endif
        InterID = VEND_ENDP5_ID04; /* Interrupt ID */
        IDint_Bulk_Write(&VendIn_Trans);
    }
    else if( (epno == IntrIn_Trans.epno) && (IntrIn_Trans.stat == MWUSB_EP_USE) )
    {
#ifdef  USE_UART_MESSAGE
        printf(" IDint_Intr_Write(IDint_EPbuf_Empty).\n\r");
        OS_User_Tslp_Tsk(1);
#endif
        /* Interrupt IN(EP3) transfer callback */
        IDint_Intr_Write(&IntrIn_Trans);
    }
    else if( (epno == IsocIn_Trans.epno) && (IsocIn_Trans.stat == MWUSB_EP_USE) )
    {
#ifdef  USE_UART_MESSAGE
        printf(" IDint_Intr_Write(IDint_EPbuf_Empty).\n\r");
        OS_User_Tslp_Tsk(1);
#endif
        /* Interrupt IN(EP3) transfer callback */
        IDint_Intr_Write(&IntrIn_Trans);
    }
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  static short BulkFIFORead                               */
/*  <Arguments> OUT_TRAN_DATA * Out_Trans                               */
/*  <Return  >  Receive Length                                          */
/*  <Description> Bulk Out Data Receive                                 */
/*                  Read data from FIFO.                                */
/*                  Management Receive Data buffer.                     */
/************************************************************************/
static unsigned long BulkFIFORead(OUT_TRAN_DATA * Out_Trans)
{
    unsigned long  receive_count = 0, requier = 0;
    unsigned short readsize = 0;
    unsigned int   flg_ptn;
    OS_USER_FLGPTN rtn_ptn;
    int            retry;
    int            rcd=-1;

    /* wait interrupt */
    flg_ptn    = FLG_USB_BulkReadRdy | FLG_USB_VendReadRdy | FLG_USB_UnConf |
                 FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                 FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;    /* Bulk Out ReadReady */
#ifdef USB_NO_REALOS
    usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
    usb_clr_flg(FID_USB, ~(flg_ptn));
#else
    OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
    OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif

    global_out_ptn = rtn_ptn; /* set global result flag pattern add 2011.01.06 tomida */

    /* Clear flag Bulk Out Read finish Wait */
#ifdef USB_NO_REALOS
    usb_clr_flg(FID_USB, ~FLG_USB_BulkReadWai);
#else
    OS_User_Clr_Flg(FID_USB, ~FLG_USB_BulkReadWai);
#endif
    
    if( (rtn_ptn & FLG_USB_BulkReadRdy) || (rtn_ptn & FLG_USB_VendReadRdy) )
    {
#ifdef USE_UART_MESSAGE
        printf("  <BulkFIFORead> flag [FLG_USB_BulkReadRdy|FLG_USB_VendReadRdy] \n\r");
#endif
    }
    else
    {
#ifdef USE_UART_MESSAGE
        if( rtn_ptn & FLG_USB_ClassTerm )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_ClassTerm] \n\r");
        }
        if( rtn_ptn & FLG_USB_ClassReset )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_ClassReset] \n\r");
        }
        if( rtn_ptn & FLG_USB_Cancel )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_Cancel] \n\r");
        }
        if( rtn_ptn & FLG_USB_TimeOut )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_TimeOut] \n\r");
        }
        if( rtn_ptn & FLG_USB_BusReset )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_BusReset] \n\r");
        }
        if( rtn_ptn & FLG_USB_DisConnected )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_DisConnected] \n\r");
        }
        if( rtn_ptn & FLG_USB_UnConf )
        {
            printf("  <BulkFIFORead> flag [FLG_USB_UnConf] \n\r");
        }
#endif
        return receive_count;
    }

    /* Get Receive Length */
    (void)HdcDev_GetOutTransactionDataSize(Out_Trans->epno,&readsize);
    requier = Out_Trans->sizeOfdata;

    if( readsize > 0 )
    {
        /* now efficiency */
        rcd = HdcDev_GetOutTransferData(Out_Trans->epno,
                                        Out_Trans->outDataPoint, requier);
        if( rcd == HDC_FAILURE )
        {
#ifdef  USE_UART_MESSAGE
            unsigned char CheckFlag, EpNo=Out_Trans->epno;
            HdcDev_IsEndpointHalt(EpNo,&CheckFlag);
            if( CheckFlag == 1 )
            {
                printf(" EP%d is Halt. in BulkFIFORead.\n\r",EpNo);
            }
            HdcDev_IsEpBufferEmpty(EpNo,&CheckFlag);
            if( CheckFlag == 1 )
            {
                printf( " EP%d is empty. in BulkFIFORead.\n\r",EpNo);
            }
#endif
            global_out_ptn = FLG_USB_TimeOut;   /* Transfer Error */
        }
        else
        {
//          OS_User_Dly_Tsk(1);
            if( requier > Out_Trans->sizeOfpacket )
            {
                flg_ptn = FLG_USB_BulkReadWai | FLG_USB_VendReadWai | FLG_USB_UnConf |
                          FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                          FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;    /* Bulk Out Read finish Wait */
#ifdef USB_NO_REALOS
                usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
                usb_clr_flg(FID_USB, ~(flg_ptn));
#else
                OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
                OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
            }

            /* wait for end of transfer */
            rcd = HdcDev_GetTransferredState( Out_Trans->epno, 0 );
            retry = 0;
            while ((rcd == HDC_TRN_ST_BUSY) && (retry < MWUSB_READ_RETRY_WAIT))
            {
                OS_User_Dly_Tsk(5);
                rcd = HdcDev_GetTransferredState( Out_Trans->epno, 0 );
                retry++;
            }
            /* check status */
            if( rcd == HDC_TRN_ST_NORMAL )
            {
                if( requier > Out_Trans->sizeOfpacket )
                {
                    /* normal end. transferred multi packet. */
                    global_out_ptn = (FLG_USB_BulkReadWai | FLG_USB_VendReadWai);
                }
            }
            else if( (rcd == HDC_TRN_ST_NULLPKT) || (rcd == HDC_TRN_ST_SHORTPKT) || 
                     (rcd == HDC_TRN_ST_SOFT_STOP) || (rcd == HDC_TRN_ST_BUSY) )
            {
                global_out_ptn = FLG_USB_TimeOut;       /* Transfer Error */
            }
            else if( rcd == HDC_TRN_ST_END_BRST )
            {
                global_out_ptn = FLG_USB_BusReset;      /* BusReset */
            }
#if 0 /* M-7M */
            /* When rcd is HDC_TRN_ST_SETCONFIG/HDC_TRN_ST_SETINTERFACE, 
             * the event flag FLG_USB_ClassReset is not needed, FLG_USB_ClassTerm needed.
             */
            else if( (rcd == HDC_TRN_ST_SETCONFIG) || (rcd == HDC_TRN_ST_SETINTERFACE) )
            {
                global_out_ptn = FLG_USB_ClassReset;    /* SetConfig,SetInterface */
            }
#endif /* M-7M */
            else
            {
                global_out_ptn = FLG_USB_ClassTerm;     /* Error */
            }
        }
        Out_Trans->sizeOfdata = 0;
        Out_Trans->outDataPoint += requier;
        receive_count = requier;
    }

    return receive_count;
}



/************************************************************************/
/*  <Function>  IDintBulk_Read                                          */
/*  <Arguments> OUT_TRAN_DATA * Out_Trans                               */
/*  <Return  >  void                                                    */
/*  <Description>   IntReady2 Interrupt Handler                         */
/*                  Bulk OUT CPU Read                                   */
/************************************************************************/
static void IDintBulk_Read(OUT_TRAN_DATA * Out_Trans)
{
    unsigned short pTransByte;
    unsigned long  datalen;
    int            stat;
    unsigned int   flg_ptn;
    OS_USER_FLGPTN rtn_ptn;
    
    switch( Out_Trans->phase )
    {
    case USB_IDLE :         /* Maybe Bug */
        global_out_ptn = 0; /* for MSC Reset */
        Out_Trans->phase = USB_IDLE;
        break;
    case USB_LENGTHB :      /* Get Length */
        global_out_ptn = 0; /* clear global flag. add 2011.01.07 tomida */
        flg_ptn = FLG_USB_BulkReadRdy | FLG_USB_VendReadRdy | FLG_USB_UnConf |
                  FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset | 
                  FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset; /* Bulk Out ReadReady   */
#ifdef USB_NO_REALOS
        usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
        usb_clr_flg(FID_USB, ~(flg_ptn));
#else
        OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
        OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif

        if( (rtn_ptn & FLG_USB_BulkReadRdy) || (rtn_ptn & FLG_USB_VendReadRdy) )
        {
            /* Get Receive Length */
            stat=HdcDev_GetOutTransactionDataSize(Out_Trans->epno,&pTransByte);
            if( stat != 0 )
            {
                global_out_ptn = FLG_USB_ClassTerm; /* Set Flag to Error Quit */
                return; /* error */
            }
            BulkOut_RecvLength  = (short)pTransByte;
            Out_Trans->phase = USB_LENGTHE;
            rtn_ptn = FLG_USB_BulkReadSize; /* Set Flag to Bulk Read Size */
        }
        global_out_ptn = rtn_ptn; /* set global result flag pattern add 2011.01.06 tomida */
        break;
    case USB_RECV :         /* Data Receive */
        /* Read Bulk Data */
        global_out_ptn = 0; /* clear global flag. add 2011.01.07 tomida */
        datalen = BulkFIFORead(Out_Trans);
        if( !(global_out_ptn & FLG_USB_BulkReadRdy) && !(global_out_ptn & FLG_USB_VendReadRdy) &&
            !(global_out_ptn & FLG_USB_BulkReadWai) && !(global_out_ptn & FLG_USB_VendReadWai)    )
        {
            /* not ready interrupt. */
            Out_Trans->phase = USB_IDLE;
            return;
        }
        if( Out_Trans->sizeOfdata > 0 )
        {   /* Exist Not Receive Data */
            if( (datalen == 0)&&(BulkOut_TotalLength != Out_Trans->sizeOfdata) )
            {   /* NullPacket receive */
                Out_Trans->phase = USB_IDLE;
                /* Set NullPacket Receive Event Flag */
                global_out_ptn = (global_out_ptn | FLG_USB_BulkRecvNull);
            }
            else if( datalen < Out_Trans->sizeOfpacket )
            {   /* ShortPacket Receive */
                Out_Trans->phase = USB_IDLE;
                /* Set ShortPacket Receive Event Flag */
                global_out_ptn = (global_out_ptn | FLG_USB_BulkRecvShort);
            }
        }
        else
        {   /* Complete Receive */
            Out_Trans->phase = USB_IDLE;
        }
        break;
        
    }
    return;
}



/************************************************************************/
/*  <Function>  void IDintByps_Read                                     */
/*  <Arguments> OUT_TRAN_DATA * Out_Trans                               */
/*  <Return  >  void                                                    */
/*  <Description>   IDintEp2 Read Interrupt Handler for ByPass mode     */
/*                  Bulk OUT CPU Read                                   */
/************************************************************************/
/* 
 * Not supported function(Interrupt handler) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
static void IDintByps_Read(OUT_TRAN_DATA * Out_Trans)
{
	unsigned long  recv_length = 0;
	unsigned int   data_size = 0;
	unsigned short pTransByte = 0;
    int            stat;
    unsigned int   flg_ptn;
    OS_USER_FLGPTN rtn_ptn;
    unsigned long  requier=0;

    global_out_ptn = 0; /* clear global flag. add 2011.01.07 tomida */

    flg_ptn	= FLG_USB_BulkReadRdy | FLG_USB_VendReadRdy | FLG_USB_UnConf | FLG_USB_DisConnected |
              FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
              FLG_USB_TimeOut | FLG_USB_BusReset;	/* Wait Bulk Out ReadReady */
#ifdef USB_NO_REALOS
    usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
    usb_clr_flg(FID_USB, ~(flg_ptn));
#else
    OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
    OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif

    global_out_ptn = rtn_ptn; /* set global result flag pattern add 2011.01.06 tomida */

    /* Clear flag Bulk Out Read finish Wait */
#ifdef USB_NO_REALOS
    usb_clr_flg(FID_USB, ~FLG_USB_BulkReadWai);
#else
    OS_User_Clr_Flg(FID_USB, ~FLG_USB_BulkReadWai);
#endif

    HdcDev_GetOutTransactionDataSize(Out_Trans->epno, &pTransByte);
    recv_length = (unsigned long)pTransByte;
//  BYPS_ContWrite_flg = 0;

    if( BYPS_Phase == 0 )
    {   // Preparation
        if( recv_length == 4 )
        {   // Data Length
            stat = HdcDev_GetOutTransferData(Out_Trans->epno,
                                             (unsigned char *)&data_size, 
                                             recv_length);
            if( stat != 0 ){ return; }
            Out_Trans->sizeOfdata = data_size;
            BypsReadSize = data_size;
            BYPS_Phase = 1;
        }
        else
        {   // Data Read & Transfer Complete
            requier = (unsigned long)Out_Trans->sizeOfdata;
            stat = HdcDev_GetOutTransferData(Out_Trans->epno,
                                             Out_Trans->outDataPoint, 
                                             requier);
            if( requier > Out_Trans->sizeOfpacket )
            {
                flg_ptn = FLG_USB_BulkReadWai | FLG_USB_VendReadWai | FLG_USB_UnConf | FLG_USB_DisConnected |
                          FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                          FLG_USB_TimeOut | FLG_USB_BusReset; /* Bulk Out Read finish Wait */
#ifdef USB_NO_REALOS
                usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
                usb_clr_flg(FID_USB, ~(flg_ptn));
#else
                OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
                OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
                global_out_ptn = rtn_ptn; /* set global result flag pattern add 2011.01.06 tomida */
            }
            Out_Trans->sizeOfdata = 0;
            Out_Trans->outDataPoint += requier;
            Out_Trans->phase = USB_IDLE;
        }
    }
    else
    {   // Data Read Phase
        requier = (unsigned long)Out_Trans->sizeOfdata;
        stat = HdcDev_GetOutTransferData(Out_Trans->epno,
                                         Out_Trans->outDataPoint, 
                                         requier);
        if( requier > Out_Trans->sizeOfpacket )
        {
            flg_ptn	= FLG_USB_BulkReadWai | FLG_USB_VendReadWai | FLG_USB_UnConf | FLG_USB_DisConnected |
                      FLG_USB_ClassTerm | FLG_USB_ClassReset | FLG_USB_Cancel |
                      FLG_USB_TimeOut | FLG_USB_BusReset;	/* Bulk Out Read finish Wait */
#ifdef USB_NO_REALOS
            //usb_wai_flg(FID_USB, flg_ptn, TWF_ORW, &rtn_ptn); /* bad argument 2012.08.10 */
            usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
            usb_clr_flg(FID_USB, ~(flg_ptn));
#else
            OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
            OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
            global_out_ptn = rtn_ptn; /* set global result flag pattern add 2011.01.06 tomida */
        }
        Out_Trans->sizeOfdata -= requier;
        Out_Trans->outDataPoint += requier;
        if( Out_Trans->sizeOfdata <= 0 )
        {   // Transfer Complete
            Out_Trans->sizeOfdata = 0;
            Out_Trans->outDataPoint = NULL;
            BYPS_Phase = 0;
            Out_Trans->phase = USB_IDLE;
        }
    }

    if( (Out_Trans->phase != USB_IDLE) && ( recv_length != 4 ) )
    {   /* Not IDLE State */
#ifdef  USE_UART_MESSAGE
        printf("     [ERROR]:not idle in IDintByps_Read. requier[%d]remain[%d]Epbufsize[%ld]\n\r",
                          (int)requier, (int)Out_Trans->sizeOfdata, recv_length) ;
        OS_User_Tslp_Tsk(1);
#endif
    }

    return;
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  IDint_Bulk_Read                                         */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>   IntReady2 Interrupt Handler                         */
/*                  Bulk OUT CPU Read                                   */
/************************************************************************/
void IDint_Bulk_Read(OUT_TRAN_DATA * Out_Trans)
{
    InterID = BULK_ENDP2_ID01; /* Interrupt ID */
    if(BYPS_Mode == MWUSB_NORMAL_MODE)
    {
#if 0
        printf(" IDintBulk_Read.\n\r");
        OS_User_Tslp_Tsk(1);
#endif
        IDintBulk_Read(Out_Trans); /* Normal Mode Interrupt */
    }
    else
    {
#if 0
        printf(" IDintByps_Read.\n\r");
        OS_User_Tslp_Tsk(1);
#endif
#if 0 /* M-7M */
        IDintByps_Read(Out_Trans); /* Bypass Mode Interrupt */
#endif /* M-7M */
    }
    return;
}



/************************************************************************/
/* <Function>   static short BulkOutRcvSize                             */
/* <Arguments>  (OUT_TRAN_DATA * Out_Trans                              */
/* <Return  >   Positive Value      Received data size                  */
/*              Negative Value      Error                               */
/* <Description> Get Received Bulk Out Data Size from Host              */
/************************************************************************/
static short BulkOutRcvSize(OUT_TRAN_DATA * Out_Trans)
{
//  unsigned long  flg_ptn, rtn_ptn;
//  unsigned short pTransByte;
    short          Size=-99;

    /* reset grobal flag */
    global_out_ptn = 0;
    
    /* Unmask IntReady2 */
    Out_Interrupt_UnMask(Out_Trans->epno);
    
    if( (global_out_ptn & (FLG_USB_DisConnected | FLG_USB_ClassTerm)) != 0x00 )
    {   /* DisConnect or Terminate */
        Size = MWUSB_ERR_QUIT;
    }
    else if( (global_out_ptn & FLG_USB_BusReset) != 0x00 )
    {   /* Bus Reset */
        Size = MWUSB_ERR_RESET;
    }
    else if( (global_out_ptn & FLG_USB_UnConf) != 0x00 )
    {   /* UnConfigured */
        Size = MWUSB_ERR_UNCONF;
    }
    else if( (global_out_ptn & FLG_USB_ClassReset) != 0x00 )
    {   /* Class Reset */
        Size = MWUSB_ERR_CLSRESET;
    }
    else if( (global_out_ptn & FLG_USB_Cancel) != 0x00 )
    {   /* Cancel */
        Size = MWUSB_ERR_CANCEL;
    }
    else if( (global_out_ptn & FLG_USB_TimeOut) != 0x00 )
    {   /* Cancel */
        Size = MWUSB_ERR_TOUT;
    }
    else if( global_out_ptn & FLG_USB_BulkReadSize )
    {   /* Normal End */
        Size = (short)BulkOut_RecvLength;
    }

    if( Size < 0 )
    {   /* Error */
        Out_Trans->phase = USB_IDLE;
    }

    return Size;
    
}

/************************************************************************/
/*  <Function>      void usb_FIFO_read_dma                              */
/*  <Arguments>     OUT_TRAN_DATA *Out_Trans                            */
/*                      Pointer to Transaction Management Data          */
/*  <Return  >      long        size of receive data                    */
/*  <Description>   Read data from Bulk Out FIFO by using DMA.          */
/*                  This function uses Bulk Out Transfer ONLY.          */
/************************************************************************/
static long usb_FIFO_read_dma(OUT_TRAN_DATA *Out_Trans)
{
    unsigned long Size, Remain, Dmax, Sadr, blk_siz;
    unsigned long flg_ptn, rtn_ptn, abn_ptn;
    long rtn=0;
    unsigned long trn_size;
    
    Size = Out_Trans->sizeOfdata;
    if (Size <= 0)
    {
        /* No Data */
        return(0) ;
    }
    else
    {
        /* add 2012.01.17 tomida */
        Size   = ((unsigned long)(Out_Trans->sizeOfdata / Out_Trans->sizeOfpacket)*Out_Trans->sizeOfpacket);
        Remain =  (unsigned long)(Out_Trans->sizeOfdata % Out_Trans->sizeOfpacket);
        Dmax   =  Size;
        Sadr   =  (unsigned long)Out_Trans->outDataPoint;
    }
    

#ifdef USB_DEBUG_CAPTURE
printf("HOST -> DSC DMA : Size = %d, Dmax = %d \n\r", Size, Dmax);	// Debug Capture
for (unsigned long i = 0; i < Size; i++) {

	if((i % 16) == 0) {
		printf("\n\r");	// Debug Capture
	}

	if((i % 4) == 0) {
		printf(" ");	// Debug Capture
	}
	
	char* aaa = (char*)Sadr + i;
	printf("%02X", *aaa);	// Debug Capture
}

printf("\n\r\n\r");	// Debug Capture

for (unsigned long i = 0; i < Remain; i++) {

	if((i % 16) == 0) {
		printf("\n\r");	// Debug Capture
	}

	if((i % 4) == 0) {
		printf(" ");	// Debug Capture
	}
	
	char* aaa = (char*)((unsigned long)(Out_Trans->outDataPoint+Size) + i);
	printf("%02X", *aaa);	// Debug Capture
}

printf("\n\r\n\r");	// Debug Capture
#endif

    /* Set block size */
    if ((unsigned long)Out_Trans->sizeOfdata < MWUSB_DMA_INCR_OUT_BLK)
    {
        blk_siz = MWUSB_DMA_INCR_OUT_BLK_SHORT;
    }
    else
    {
        blk_siz = MWUSB_DMA_INCR_OUT_BLK;
    }
    
    if( mwusb_dma_read_stat != DMA_USE2_CHNL )
    {
        rtn = HdcDev_SetupDma(DMA_CH_BULK_OUT, 1, 0);
        if( rtn < 0 )
        {   // Error
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            return rtn;
        }
        mwusb_dma_read_stat = DMA_USE2_CHNL;
    }
    
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
	printf("usb_FIFO_read_dma(): Size = %lu, Remain = %lu, Dmax = %lu, blk_siz = %lu\n", Size, Remain, Dmax, blk_siz);
#endif

    /** transfer loop **/
    while( Dmax > 0 )
    {
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
		unsigned long timestamp1, timestamp2, timestamp3, timestamp4;
#endif
        /* waiting for event flag */
        global_out_ptn = 0; /* clear global flag. add 2011.01.07 tomida */
        flg_ptn = FLG_USB_BulkReadRdy | FLG_USB_VendReadRdy | FLG_USB_UnConf |
                  FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                  FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;    /* Bulk Out ReadReady    */
#ifdef USB_NO_REALOS
        usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
        usb_clr_flg(FID_USB, ~(flg_ptn));
#else
        OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
        OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
        global_out_ptn = rtn_ptn; /* set global flag */
        
        abn_ptn = FLG_USB_UnConf | FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                  FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;
        if ((global_out_ptn & abn_ptn) != 0x00)
        {
            /* process aborted */
            return rtn;
        }

        if (Dmax >= MWUSB_DMA_TRANS_DATASIZE_MAX)
        {
            trn_size = MWUSB_DMA_TRANS_DATASIZE_MAX;
        }
        else
        {
            trn_size = Dmax;
        }
        
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
		timestamp1 = BF_Get_Time_us();
		printf("usb_FIFO_read_dma(): trn_size = %lu\n", trn_size);
#endif
        /* begin dma transfer */
        rtn = MWUSB_Set_Dma_Read(DMA_CH_BULK_OUT, Sadr, trn_size, blk_siz);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : MWUSB_Set_Dma_Read (%ld) address=%lx size=%ld block=%ld\n",
                       rtn, Sadr, trn_size, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            return rtn;
        }

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
		timestamp2 = BF_Get_Time_us();
#endif
        /* Start dma transfer */
        rtn = HdcDev_StartDmaTransfer(DMA_CH_BULK_OUT, Out_Trans->epno, trn_size, blk_siz, 0, 0);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : HdcDev_StartDmaTransfer (%ld) address=%lx size=%ld block=%ld\n",
                       rtn, Sadr, trn_size, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            return rtn;
        }

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
		timestamp3 = BF_Get_Time_us();
#endif
        /* wait for end of dma */
        rtn = MWUSB_Wait_Dma_End(DMA_CH_BULK_OUT);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : MWUSB_Wait_Dma_End (%ld) address=%lx size=%ld block=%ld\n",
                       rtn, Sadr, trn_size, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            if (mwusb_InitializeDriver == MWUSB_OK) {
                /* DMA Abort Sequence */
                /* The USB clock may be terminated on the connect task,   */
                /* so the below process may cause any abnormal behavior   */
                /* of USB macro.                                          */
                /* In this case, The initialize flag cannot be MWUSB_OK.  */
                HdcDev_StopDmaTransfer(DMA_CH_BULK_OUT);
                MWUSB_Abort_Dma(DMA_CH_BULK_OUT);
            }
            
            return rtn;
        }
        Dmax -= trn_size;
        Sadr += trn_size;

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
		timestamp4 = BF_Get_Time_us();
		printf("usb_FIFO_read_dma(): Timestamp1 - 4 = %lu\t%lu\t%lu\t%lu\n", timestamp1, timestamp2, timestamp3, timestamp4);
#endif
    }

    /* transaction data at under the max packet */
    if (Remain > 0)
    {
        /* waiting for event flag */
        global_out_ptn = 0; /* clear global flag. add 2011.01.07 tomida */
        flg_ptn = FLG_USB_BulkReadRdy | FLG_USB_VendReadRdy | FLG_USB_UnConf |
                  FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                  FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;    /* Bulk Out ReadReady    */
#ifdef USB_NO_REALOS
        usb_wai_flg(&rtn_ptn, FID_USB, flg_ptn, TWF_ORW);
        usb_clr_flg(FID_USB, ~(flg_ptn));
#else
        OS_User_Wai_Flg(FID_USB, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
        OS_User_Clr_Flg(FID_USB, ~(flg_ptn));
#endif
        global_out_ptn = rtn_ptn; /* set global flag */
        
        abn_ptn = FLG_USB_UnConf | FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_ClassReset |
                  FLG_USB_Cancel | FLG_USB_TimeOut | FLG_USB_BusReset;
        if ((global_out_ptn & abn_ptn) != 0x00)
        {
            /* process aborted */
            return rtn;
        }

        /* the alignment of datasize is on 4byte */
        blk_siz = MWUSB_DMA_INCR_OUT_BLK_SHORT; /* Set Block size */

        /* begin dma */
        rtn = (long)MWUSB_Set_Dma_Read(DMA_CH_BULK_OUT, (unsigned long)(Out_Trans->outDataPoint+Size), Remain, blk_siz);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : MWUSB_Set_Dma_Read (%ld) address=%lx  size=%ld  block=%ld\n",
                       rtn, (unsigned long)(Out_Trans->outDataPoint+Size), Remain, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            return rtn;
        }

        /* start dma */
        rtn = HdcDev_StartDmaTransfer(DMA_CH_BULK_OUT, Out_Trans->epno, Remain, blk_siz, 0, 0);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : HdcDev_StartDmaTransfer (%ld) address=%lx  size=%ld  block=%ld\n",
                       rtn, (unsigned long)(Out_Trans->outDataPoint+Size), Remain, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            return rtn;
        }

        /* wait for end of dma */
        rtn = (long)MWUSB_Wait_Dma_End(DMA_CH_BULK_OUT);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : MWUSB_Wait_Dma_End (%ld) address=%lx  size=%ld  block=%ld\n",
                       rtn, (unsigned long)(Out_Trans->outDataPoint+Size), Remain, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            Out_Trans->phase = USB_IDLE;
            Out_Trans->sizeOfdata = 0;
            if (mwusb_InitializeDriver == MWUSB_OK) {
                /* DMA Abort Sequence */
                /* The USB clock may be terminated on the connect task,   */
                /* so the below process may cause any abnormal behavior   */
                /* of USB macro.                                          */
                /* In this case, The initialize flag cannot be MWUSB_OK.  */
                HdcDev_StopDmaTransfer(DMA_CH_BULK_OUT);
                MWUSB_Abort_Dma(DMA_CH_BULK_OUT);
            }
            
            return rtn;
        }
    }

    Out_Trans->phase = USB_IDLE;
    rtn = Out_Trans->sizeOfdata;
    Out_Trans->sizeOfdata = 0;            /* Send size (Byte)        */

    return( rtn );
}


/****************************************************************************/
/*  <Function>  unsigned short usb_FIFO_write_dma                           */
/*  <Arguments>     IN_TRAN_DATA *data                                      */
/*                      Pointer to Transaction Management Data              */
/*  <Return  >      long        size of send data                           */
/*  <Description>   Write data to Bulk IN FIFO by using DMA.                */
/*                  This function uses Bulk IN Transfer ONLY.               */
/****************************************************************************/
static long usb_FIFO_write_dma(IN_TRAN_DATA *In_Trans)
{
    unsigned long Size, Remain, Dmax, Sadr, blk_siz;
    long rtn=0;
    
    Size = In_Trans->sizeOfdata;
    if (Size <= 0)
    {
        /* No Data */
        return(0) ;
    }
    else
    {
        /* add 2012.01.17 tomida */
        Size   = ((unsigned long)(In_Trans->sizeOfdata / In_Trans->sizeOfpacket)*In_Trans->sizeOfpacket);
        Remain =  (unsigned long)(In_Trans->sizeOfdata % In_Trans->sizeOfpacket);
        Dmax   =  Size;
        Sadr   =  (unsigned long)In_Trans->inDataPoint;
    }

#ifdef USB_DEBUG_CAPTURE
printf("DSC -> HOST DMA : Size = %d, Dmax = %d \n\r", Size, Dmax);	// Debug Capture

for (unsigned long i = 0; i < Size; i++) {

	if((i % 16) == 0) {
		printf("\n\r");	// Debug Capture
	}

	if((i % 4) == 0) {
		printf(" ");	// Debug Capture
	}
	
	char* aaa = (char*)Sadr + i;
	printf("%02X", *aaa);	// Debug Capture
}
printf("\n\r\n\r");	// Debug Capture

for (unsigned long i = 0; i < Remain; i++) {

	if((i % 16) == 0) {
		printf("\n\r");	// Debug Capture
	}

	if((i % 4) == 0) {
		printf(" ");	// Debug Capture
	}
	
	char* aaa = (char*)((unsigned long)(In_Trans->inDataPoint+Size) + i);
	printf("%02X", *aaa);	// Debug Capture
}

printf("\n\r\n\r");	// Debug Capture
#endif


    /* Set block size */
    if ((unsigned long)In_Trans->sizeOfdata < MWUSB_DMA_INCR_IN_BLK)
    {
        blk_siz = MWUSB_DMA_INCR_IN_BLK_SHORT;
    }
    else
    {
        blk_siz = MWUSB_DMA_INCR_IN_BLK;
    }
    
    if( mwusb_dma_write_stat != DMA_USE1_CHNL )
    {
        rtn = HdcDev_SetupDma(DMA_CH_BULK_IN, 1, 0);
        if( rtn < 0 )
        {   // Error
            In_Trans->phase = USB_IDLE;
            In_Trans->sizeOfdata = 0;
            return rtn;
        }
        mwusb_dma_write_stat = DMA_USE1_CHNL;
    }

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
	printf("usb_FIFO_write_dma(): Size = %lu, Remain = %lu, Dmax = %lu, blk_siz = %lu\n", Size, Remain, Dmax, blk_siz);
#endif

    //printf(" write DMA transfer size = %ld\n\r",Size);
    {
        unsigned long trn_size;
        while( Dmax > 0 )
        {
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
			unsigned long timestamp1, timestamp2, timestamp3, timestamp4;
#endif
            if (Dmax >= MWUSB_DMA_TRANS_DATASIZE_MAX)
            {
                trn_size = MWUSB_DMA_TRANS_DATASIZE_MAX;
            }
            else
            {
                trn_size = Dmax;
            }
            
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
			timestamp1 = BF_Get_Time_us();
			printf("usb_FIFO_write_dma(): trn_size = %lu\n", trn_size);
#endif
            /* begin dma transfer */
            rtn = MWUSB_Set_Dma_Write(DMA_CH_BULK_IN, Sadr, trn_size, blk_siz);
            if( rtn < 0 )
            {   // Error
#ifdef USE_UART_MESSAGE
                printf("*************************************************************************\n\r");
                printf(" Error : MWUSB_Set_Dma_Write (%ld) address=%lx size=%ld block=%ld\n",
                           rtn, Sadr, trn_size, blk_siz);
                printf("*************************************************************************\n\r");
#endif
                In_Trans->phase = USB_IDLE;
                In_Trans->sizeOfdata = 0; /* Send size (Byte) */
                return rtn;
            }

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
			timestamp2 = BF_Get_Time_us();
#endif
            /* Start dma transfer */
            rtn = HdcDev_StartDmaTransfer(DMA_CH_BULK_IN, In_Trans->epno, trn_size, blk_siz, 0, 0);
            if( rtn < 0 )
            {   // Error
#ifdef USE_UART_MESSAGE
                printf("*************************************************************************\n\r");
                printf(" Error : HdcDev_StartDmaTransfer (%ld) address=%lx size=%ld block=%ld\n",
                           rtn, Sadr, trn_size, blk_siz);
                printf("*************************************************************************\n\r");
#endif
                In_Trans->phase = USB_IDLE;
                In_Trans->sizeOfdata = 0; /* Send size (Byte) */
                return rtn;
            }

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
			timestamp3 = BF_Get_Time_us();
#endif
            /* wait for end of dma */
            rtn = MWUSB_Wait_Dma_End(DMA_CH_BULK_IN);
            if( rtn < 0 )
            {   // Error
#ifdef USE_UART_MESSAGE
                printf("*************************************************************************\n\r");
                printf(" Error : MWUSB_Wait_Dma_End (%ld) address=%lx size=%ld block=%ld\n",
                           rtn,Sadr,trn_size,blk_siz);
                printf("*************************************************************************\n\r");
#endif
                In_Trans->phase = USB_IDLE;
                In_Trans->sizeOfdata = 0; /* Send size (Byte) */
                if (mwusb_InitializeDriver == MWUSB_OK) {
                    /* DMA Abort Sequence */
                    /* The USB clock may be terminated on the connect task,   */
                    /* so the below process may cause any abnormal behavior   */
                    /* of USB macro.                                          */
                    /* In this case, The initialize flag cannot be MWUSB_OK.  */
                    HdcDev_StopDmaTransfer(DMA_CH_BULK_IN);
                    MWUSB_Abort_Dma(DMA_CH_BULK_IN);
                }
                
                return rtn;
            }
            Dmax -= trn_size;
            Sadr += trn_size;

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
			timestamp4 = BF_Get_Time_us();
			printf("usb_FIFO_write_dma(): Timestamp1 - 4 = %lu\t%lu\t%lu\t%lu\n", timestamp1, timestamp2, timestamp3, timestamp4);
#endif
        }
    }
    
    /* add 2012.01.12 tomida */
    if( Remain > 0 )
    {
        /* the alignment of datasize is on 4byte */
        blk_siz = MWUSB_DMA_INCR_IN_BLK_SHORT; /* Set Block size */

        /* begin dma transfer */
        rtn = MWUSB_Set_Dma_Write(DMA_CH_BULK_IN, (unsigned long)(In_Trans->inDataPoint+Size), Remain, blk_siz);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : MWUSB_Set_Dma_Write (%ld) address=%lx size=%ld block=%ld\n",
                       rtn, (unsigned long)(In_Trans->inDataPoint+Size), Remain, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            In_Trans->phase = USB_IDLE;
            In_Trans->sizeOfdata = 0; /* Send size (Byte) */
            return rtn;
        }

        /* Start dma transfer */
        rtn = HdcDev_StartDmaTransfer(DMA_CH_BULK_IN, In_Trans->epno, Remain, blk_siz, 0, 0);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : HdcDev_StartDmaTransfer (%ld) address=%lx size=%ld block=%ld\n",
                       rtn, (unsigned long)(In_Trans->inDataPoint+Size), Remain, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            In_Trans->phase = USB_IDLE;
            In_Trans->sizeOfdata = 0; /* Send size (Byte) */
            return rtn;
        }

        /* wait for end of dma */
        rtn = MWUSB_Wait_Dma_End(DMA_CH_BULK_IN);
        if( rtn < 0 )
        {   // Error
#ifdef USE_UART_MESSAGE
            printf("*************************************************************************\n\r");
            printf(" Error : MWUSB_Wait_Dma_End (%ld) address=%lx size=%ld block=%ld\n",
                       rtn, (unsigned long)(In_Trans->inDataPoint+Size), Remain, blk_siz);
            printf("*************************************************************************\n\r");
#endif
            In_Trans->phase = USB_IDLE;
            In_Trans->sizeOfdata = 0; /* Send size (Byte) */
            if (mwusb_InitializeDriver == MWUSB_OK) {
                /* DMA Abort Sequence */
                /* The USB clock may be terminated on the connect task,   */
                /* so the below process may cause any abnormal behavior   */
                /* of USB macro.                                          */
                /* In this case, The initialize flag cannot be MWUSB_OK.  */
                HdcDev_StopDmaTransfer(DMA_CH_BULK_IN);
                MWUSB_Abort_Dma(DMA_CH_BULK_IN);
            }
            
            return rtn;
        }
    }
        
    /* set global flag */
    if( InterID == BULK_ENDP1_ID00 )
    {
        global_in_ptn = FLG_USB_BulkWriteRdy;
    }
    else if( InterID == INTR_ENDP3_ID02 )
    {
        global_in_ptn = FLG_USB_IntrWriteRdy;
    }
    else if( InterID == VEND_ENDP5_ID04 )
    {
        global_in_ptn = FLG_USB_VendWriteRdy;
    }
    
    In_Trans->phase = USB_IDLE;
    rtn = In_Trans->sizeOfdata;
    In_Trans->sizeOfdata = 0;            /* Send size (Byte)        */
    
    return( rtn );
}



/************************************************************************/
/* <Function>   long MWUSB_SetBulkInDMAUse                              */
/* <Arguments>  void                                                    */
/* <Return  >   Condition OK=MWUSB_OK NG=MWUSB_NG                       */
/* <Description> Start In transfer by DMA                               */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S21A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_SetBulkInDMAUse(unsigned char flag)
{
    long RetCode = MWUSB_OK;
    if( (flag != MWUSB_DMA_NOUSE) && (flag != MWUSB_DMA_USE) )
    {
        RetCode = MWUSB_NG;
    }
    else
    {
        IN_DMA_Mode = flag; /* Set flag */
    }
    
    return( RetCode );
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   uchar MWUSB_GetBulkInDMAUse                             */
/* <Arguments>  void                                                    */
/* <Return  >   ungined char DMA use flag                               */
/* <Description> Get Bulk In Transfer flag                              */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
unsigned char MWUSB_GetBulkInDMAUse(void)
{
    return( IN_DMA_Mode );
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   long MWUSB_SetBulkOutDMAUse                             */
/* <Arguments>  void                                                    */
/* <Return  >   Condition OK=MWUSB_OK NG=MWUSB_NG                       */
/* <Description> Start Out transfer by DMA                              */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_SetBulkOutDMAUse(unsigned char flag)
{
    long RetCode = MWUSB_OK;
    if( (flag != MWUSB_DMA_NOUSE) && (flag != MWUSB_DMA_USE) )
    {
        RetCode = MWUSB_NG;
    }
    else
    {
        OUT_DMA_Mode = flag;    /* Set flag */
    }
    
    return( RetCode );
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   uchar MWUSB_GetBulkOutDMAUse                            */
/* <Arguments>  void                                                    */
/* <Return  >   ungined char DMA use flag                               */
/* <Description> Get Bulk Out Transfer flag                             */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
unsigned char MWUSB_GetBulkOutDMAUse(void)
{
    return( OUT_DMA_Mode );
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   short MWUSB_RecvBulkNull                                */
/* <Arguments>  void                                                    */
/* <Return  >   void                                                    */
/* <Description> Receive Null Packet from Bulk Out EP                   */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_RecvBulkNull(void)
{
    (void)BulkOutRcvSize(&BulkOut_Trans);
    return;
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   short MWUSB_RecvVendNull                                */
/* <Arguments>  void                                                    */
/* <Return  >   void                                                    */
/* <Description> Receive Null Packet from Vend Out EP                   */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_RecvVendNull(void)
{
    (void)BulkOutRcvSize(&VendOut_Trans);
    return;
}
#endif /* M-7M */


/********************************************************************/
/*  <Function>  void MWUSB_BulkSetNackResponse                      */
/*  <Arguments> epno    Endpoint Number                             */
/*                  0:control                                       */
/*                  1:Bulk                                          */
/*                  3:Interrupt                                     */
/*  <Return  >  none                                                */
/*  <Description> Always Nack Response from host request            */
/********************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_BulkSetNackResponse(void)
{
    (void)HdcDev_SetNackResponse(BulkOut_Trans.epno,1);
    return;
}
#endif /* M-7M */


/********************************************************************/
/*  <Function>  void MWUSB_BulkResetNackResponse                    */
/*  <Arguments> epno    Endpoint Number                             */
/*                  0:control                                       */
/*                  1:Bulk                                          */
/*                  3:Interrupt                                     */
/*  <Return  >  none                                                */
/*  <Description> Reset always Nack Response from host request      */
/********************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_BulkResetNackResponse(void)
{
    (void)HdcDev_SetNackResponse(BulkOut_Trans.epno,0);
    return;
}
#endif /* M-7M */


/********************************************************************/
/*  <Function>  void MWUSB_VendSetNackResponse                      */
/*  <Arguments> epno    Endpoint Number                             */
/*                  0:control                                       */
/*                  1:Bulk                                          */
/*                  3:Interrupt                                     */
/*  <Return  >  none                                                */
/*  <Description> Always Nack Response from host request            */
/********************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_VendSetNackResponse(void)
{
    (void)HdcDev_SetNackResponse(VendOut_Trans.epno,1);
    return;
}
#endif /* M-7M */


/********************************************************************/
/*  <Function>  void MWUSB_VendResetNackResponse                    */
/*  <Arguments> epno    Endpoint Number                             */
/*                  0:control                                       */
/*                  1:Bulk                                          */
/*                  3:Interrupt                                     */
/*  <Return  >  none                                                */
/*  <Description> Reset always Nack Response from host request      */
/********************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSB_VendResetNackResponse(void)
{
    (void)HdcDev_SetNackResponse(VendOut_Trans.epno,0);
    return;
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   long MWUSB_CheckFifoEmpty                               */
/* <Arguments>  unsigned char endp                                      */
/*                  Endpoint No                                         */
/*                          CNTL_READ_FIFO  :   EP0                     */
/*                          CNTL_WRITE_FIFO :   EP0                     */
/*                          BULK_READ_FIFO  :   EP2                     */
/*                          BULK_WRITE_FIFO :   EP1                     */
/*                          INTR_WRITE_FIFO :   EP3                     */
/* <Return  >   MWUSB_OK    FIFO is Empty                               */
/*              MWUSB_NG    FIFO is Not Empty or Err                    */
/* <Description> Check FIFO is Empty                                    */
/************************************************************************/
long MWUSB_CheckFifoEmpty(unsigned char FIFO)
{
    long  ret;
    short check;
    unsigned char endp, emptyFlag = 0;
    int  stat;
    
    /* Low Level Driver is already initialized. */
    if (mwusb_InitializeDriver == MWUSB_OK)
    {
        check = mwusb_FifoNo_2_EpNo(FIFO); /* add 2010.11.26 by tomida */
        if( check != MWUSB_ERR_BADEP )
        {
            endp = (unsigned char)check;
            if( (endp == CntlOut_Trans.epno) ||
                ((BulkOut_Trans.stat == MWUSB_EP_USE) && (endp == BulkOut_Trans.epno)) ||
                ((VendOut_Trans.stat == MWUSB_EP_USE) && (endp == VendOut_Trans.epno)) ||
                ((IsocOut_Trans.stat == MWUSB_EP_USE) && (endp == IsocOut_Trans.epno))    )
            {
                stat = HdcDev_IsEpBufferEmpty(endp, &emptyFlag);
                if( (emptyFlag != 1) || (stat != HDC_SUCCESS) )
                {
                    ret=MWUSB_NG;
                }
                else
                {
                    ret=MWUSB_OK;
                }
            } else if( (endp == CntlIn_Trans.epno) ||
                ((BulkIn_Trans.stat == MWUSB_EP_USE) && (endp == BulkIn_Trans.epno)) ||
                ((VendIn_Trans.stat == MWUSB_EP_USE) && (endp == VendIn_Trans.epno)) ||
                ((IntrIn_Trans.stat == MWUSB_EP_USE) && (endp == IntrIn_Trans.epno)) ||
                ((IsocIn_Trans.stat == MWUSB_EP_USE) && (endp == IsocIn_Trans.epno))    )
            {
                ret = MWUSB_OK;
            }
            else
            {
                ret = MWUSB_ERR_BADEP;
            }
        }
        else
        {
            ret = (long)check;
        }
    }
    else
    {
        ret = MWUSB_NG;
    }
    
    return (ret);
}



/************************************************************************/
/* <Function>   static short Bulk_Read1Packet                           */
/* <Arguments>  unsigned char *buf                                      */
/*                  Buffer pointer of receive data.                     */
/*              OUT_TRAN_DATA * Out_Trans                               */
/*                  Out Transfer control table pointer                  */
/* <Return  >   Positive Value      Received data size                  */
/*              Negative Value      Error                               */
/* <Description> Receive Bulk Out data(1Packet) from Host               */
/************************************************************************/
static short Bulk_Read1Packet(unsigned char *buf, OUT_TRAN_DATA * Out_Trans)
{
    int stat;
    short Size = 0;
    // unsigned char haltFlag;
    
#ifdef USB_DEBUG2
    printf("Q");
#endif
    
    /* Buffer address alignment check */
    if( ((unsigned long)buf & 0x00000003) != 0x00000000 ) 
    {
#ifdef USE_UART_MESSAGE
        printf("USB Out Alignment Error\n\r");
#endif
        Size = MWUSB_ERR_ALLIGN;
    }
    else if( mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE )
    {
        Size = MWUSB_ERR_QUIT; /* Disconnect or Terminate */
    }
    else if( Out_Trans->phase != USB_IDLE )
    {   /* Not IDLE State */
        Size = MWUSB_ERR_NOTIDLE;
    }
    else if( MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG )
    {   /* Not Configured */
        Size = MWUSB_ERR_UNCONF;
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        Size = MWUSB_ERR_RESET;
    }
#if 0
    else if( HdcDev_GetDeviceState() == DEVICE_STATE_SUSPEND )
    {
        Size = MWUSB_ERR_SUSPEND; /* suspend */
    }
    else if( HdcDev_IsEndpointHalt(Out_Trans->epno,&haltFlag) == 0 )
    {
        if( haltFlag == 1 )
        {
            Size = MWUSB_ERR_STALL; /* halt */
        }
    }
#endif
    if( Size < 0 )
    {   /* Error */
#ifdef USE_UART_MESSAGE
        printf("USB Out Error Bulk_Read1Packet(%d)\n\r", Size);
#endif
        return Size;
    }
    
    /* reset global flag */
    global_out_ptn = 0;
    
    /* set STAGE fot Get Receive Length */
    Out_Trans->phase = USB_LENGTHB;
    
    /* Get Receive Data Size */
    Size = BulkOutRcvSize(Out_Trans);
    
    if( Size <= 0 )
    {   /* Error or Null Packet */
        Out_Trans->phase = USB_IDLE;
        /* was always analized error */
        return Size;
    }

    /* if( (OUT_DMA_Mode == MWUSB_DMA_NOUSE)||(Size < DMA_USE_LIMIT_LENGTH) ) */
    {   /* Use CPU Transfer */
        stat=HdcDev_GetOutTransferData(Out_Trans->epno, buf, Size);
        if( stat != 0 )
        {
            /* Error */
            Size = MWUSB_ERR_BADEP;
        }   
        /* Data Transfer Complete */
        Out_Trans->phase = USB_IDLE;
    }

    if( Size < 0 )
    {   /* Error */
        Out_Trans->phase = USB_IDLE;
    }

    return Size;
}



/************************************************************************/
/* <Function>   short MWUSB_Read1Packet                                 */
/* <Arguments>  unsigned char *buf                                      */
/*                  Buffer pointer of receive data.                     */
/* <Return  >   Positive Value      Received data size                  */
/*              Negative Value      Error                               */
/* <Description> Receive Bulk Out data(1Packet) from Host               */
/************************************************************************/
short MWUSB_Read1Packet( unsigned char *buf )
{
    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    return( Bulk_Read1Packet(buf, &BulkOut_Trans) );
}



/************************************************************************/
/* <Function>   short MWUSB_VendRead1Packet                             */
/* <Arguments>  unsigned char *buf                                      */
/*                  Buffer pointer of receive data.                     */
/* <Return  >   Positive Value      Received data size                  */
/*              Negative Value      Error                               */
/* <Description> Receive Bulk Out data(1Packet) from Host               */
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
short MWUSB_VendRead1Packet( unsigned char *buf )
{
    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    return( Bulk_Read1Packet(buf, &VendOut_Trans) );
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   long BypassRecvLength                                   */
/* <Arguments>  OUT_TRAN_DATA * Out_Trans                               */
/*                  Out Transfer control table pointer                  */
/* <Return  >   MWUSB_OK        OK                                      */
/*              Negative Value      Error                               */
/* <Description> Receive ByPass Mode Length                             */
/************************************************************************/
/* 
 * Do not use this function in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
static long BypassRecvLength(OUT_TRAN_DATA * Out_Trans)
{
    //unsigned char* buf;
    long Size = 0;

    /* Get Receive Length */
    Out_Trans->phase = USB_LENGTHB;
    //buf = BypsReadBuf_TopPtr;
    
    /* Get Receive Data Size */
    Size = BulkOutRcvSize(Out_Trans);
    if( Size <= 0 )
    {   /* Error or Null Packet */
        Out_Trans->phase = USB_IDLE;
        return Size;
    }

    return Size;
}
#endif /* M-7M */


/********************************************************************************/
/*  <Function>  long Bulk_Read                                                  */
/*  <Arguments>     unsigned char *buf                                          */
/*                      Buffer pointer of receive data.                         */
/*                  unsigned long Size                                          */
/*                      Receive data size (byte)                                */
/*                  unsigned char nfifo                                         */
/*                      FIFO Number                                             */
/*                  OUT_TRAN_DATA * Out_Trans                                   */
/*                      Out Transfer control table pointer                      */
/*  <Return  >      Positive Value      Received data size                      */
/*                  Negative Value      Error                                   */
/*  <Description> Receive Bulk Out data from Hos                                */
/********************************************************************************/
static long Bulk_Read(unsigned char *buf,
                      unsigned long Size,
                      unsigned char nfifo,
                      OUT_TRAN_DATA * Out_Trans)
{
    // unsigned long flg_ptn, rtn_ptn;
    long rtn=0;
    // unsigned char haltFlag;
    
#ifdef USB_DEBUG2
    printf("O");
#endif
    
    global_out_ptn = 0; /* clear global flag. add 2011.01.07 tomida */

    /* Buffer address alignment check */
    if( ((unsigned long)buf & 0x00000003) != 0x00000000 ) 
    {
#ifdef USE_UART_MESSAGE
        printf("USB Out Alignment Error\n\r");
#endif
        rtn = MWUSB_ERR_ALLIGN;
    }
    else if( mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE )
    {
        rtn = MWUSB_ERR_QUIT; /* Disconnect or Terminate */
    }
    else if( (nfifo != BULK_READ_FIFO) && (nfifo != VEND_READ_FIFO) )
    {   /* Bad Endpoint */
        rtn = MWUSB_ERR_BADEP;
    }
    else if( ((BYPS_Mode == MWUSB_NORMAL_MODE)&&((Size == 0)||(Size >= 0x10000000))) )
    {   /* DMA Limitation 16MB */
        rtn = MWUSB_ERR_SIZE;
    }
    else if( ((Size >= 0x10000000)&&(BYPS_Mode == MWUSB_BYPASS_MODE)) )
    {   /* DMA Limitation 16MB */
        rtn = MWUSB_ERR_SIZE;
    }
    else if( Out_Trans->phase != USB_IDLE )
    {   /* Not IDLE State */
        rtn = MWUSB_ERR_NOTIDLE;
    }
    else if( MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG )
    {   /* Not Configured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
#if 0
    else if( HdcDev_GetDeviceState() == DEVICE_STATE_SUSPEND )
    {
        rtn = MWUSB_ERR_SUSPEND; /* suspend */
    }
    else if( HdcDev_IsEndpointHalt(Out_Trans->epno,&haltFlag) == 0 )
    {
        if( haltFlag == 1 )
        {
            rtn = MWUSB_ERR_STALL; /* halt */
        }
    }
#endif
    if( rtn < 0 )
    {   /* Error */
#ifdef USE_UART_MESSAGE
        printf("USB Out Error Bulk_Read\n\r");
#endif
        return rtn;
    }

    if( BYPS_Mode == MWUSB_BYPASS_MODE )
    {
#if 0 /* M-7M */
        /***************/
        /* BYPASS Mode */
        /***************/
        /* Get Receive Length */
        Out_Trans->phase = USB_LENGTHB;
//      BypsReadBuf_TopPtr = buf;
        BYPS_Phase = 0;
    
        /* Get Receive Data Size */
        Size = BypassRecvLength(Out_Trans);
        if( Size <= 0 )
        {   /* Error or Null Packet */
            Out_Trans->phase = USB_IDLE;
            return Size;
        }
        Size = (unsigned long)BypsReadSize;
        BYPS_Phase = 1;
#endif /* M-7M */
    }
    else
    {   /* Normal Mode */
        BypsReadSize = (long)Size;
    }

    /* Initialize OUT-Trans. Mangement Data */
    BulkOut_TotalLength     = (long)Size;
    Out_Trans->phase        = USB_RECV;
    Out_Trans->outDataPoint = buf;      /* Receive buffer pointer   */
    Out_Trans->sizeOfdata   = Size;     /* Receive size (Byte)      */


    if( (OUT_DMA_Mode == MWUSB_DMA_NOUSE) || (Size < Out_Trans->sizeOfpacket) || (((unsigned long)buf % 4) > 0) )
    {   /* CPU Transfer */
        /* UnMask IntReady2 Interrupt */
        Out_Interrupt_UnMask(Out_Trans->epno);
    }
    //////////k.maeda 2011/01/07
    else
    {   /* DMA Transfer */
        if( usb_FIFO_read_dma(Out_Trans) <= 0 )
        {
            /* Function Error */
            rtn = MWUSB_ERR_QUIT;
            return(rtn);
        }
        
        /* UnMask EP2 Interrupt *//* will be start transfer */
        //_USB_EpIC &= ~(_bEP2_MASK_FLG);
    }

    //BYPS_Phase = 0;

    if( (global_out_ptn & (FLG_USB_DisConnected | FLG_USB_ClassTerm)) != 0x00 )
    {   /* DisConnect or Terminate */
        rtn = MWUSB_ERR_QUIT;
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
    else if( (MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG) || (global_out_ptn & FLG_USB_UnConf) )
    {   /* UnConfigured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( (global_out_ptn & FLG_USB_BulkRecvShort) != 0x00 )
    {   /* ShortPacket Detect */
        rtn = MWUSB_ERR_SHORTPACKET;
    }
    else if( (global_out_ptn & FLG_USB_BulkRecvNull) != 0x00 )
    {   /* NullPacket Detect */
        rtn = MWUSB_ERR_NULLPACKET;
    }
    else if( (global_out_ptn & FLG_USB_ClassReset) != 0x00 )
    {   /* Class Reset */
        rtn = MWUSB_ERR_CLSRESET;
    }
    else if( (global_out_ptn & FLG_USB_Cancel) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_CANCEL;
    }
    else if( (global_out_ptn & FLG_USB_TimeOut) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_TOUT;
    }
    else if( ( (global_out_ptn & FLG_USB_BulkReadRdy) != 0x00 ) || ( (global_out_ptn & FLG_USB_VendReadRdy) != 0x00 ) ||
             ( (global_out_ptn & FLG_USB_BulkReadWai) != 0x00 ) || ( (global_out_ptn & FLG_USB_VendReadWai) != 0x00 )    )
    {   /* Transfer Complete */
        rtn = BypsReadSize ;
    }

    if( rtn <= 0 )
    {   /* Error */
#ifdef USE_UART_MESSAGE
        printf("OUT Err:%ld ptn:%lx\n", rtn, global_out_ptn);
#endif
        Out_Trans->phase = USB_IDLE;
    }

    if( Out_Trans->phase != USB_IDLE )
    {   /* Not IDLE State */
#ifdef USE_UART_MESSAGE
        printf("     [ERROR]:not idle in MWUSB_read.\n\r") ;
        OS_User_Tslp_Tsk(1);
#endif
    }

#ifdef USB_DEBUG2

    printf("o");
#endif

    return rtn;
}



/********************************************************************************/
/*  <Function>  long MWUSB_Read                                                 */
/*  <Arguments>     unsigned char *buf                                          */
/*                      Buffer pointer of receive data.                         */
/*                  unsigned long Size                                          */
/*                      Receive data size (byte)                                */
/*                  unsigned char nfifo                                         */
/*                      FIFO Number                                             */
/*  <Return  >      Positive Value      Received data size                      */
/*                  Negative Value      Error                                   */
/*  <Description> Receive Bulk Out data from Hos                                */
/********************************************************************************/
long MWUSB_Read( unsigned char *buf, unsigned long Size, unsigned char nfifo)
{
//    /* Pointer-type argument NULL check */
//    if (buf == NULL) {
//        return MWUSB_ERR_NULLPOINTER;
//    }
//    return( Bulk_Read(buf, Size, nfifo, &BulkOut_Trans) );

    long ret;

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
    ULONG  starttime = BF_Get_Time_us();
    ULONG  endtime;
#endif
    
    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    
    ret = Bulk_Read(buf, Size, nfifo, &BulkOut_Trans);
    
#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
    endtime = BF_Get_Time_us();
    printf("MWUSB_Read(): Size = %lu, Time = %lu.\n", Size, endtime-starttime) ;
#endif
    return ret;
}



/********************************************************************************/
/*  <Function>  long MWUSB_VendRead                                             */
/*  <Arguments>     unsigned char *buf                                          */
/*                      Buffer pointer of receive data.                         */
/*                  unsigned long Size                                          */
/*                      Receive data size (byte)                                */
/*                  unsigned char nfifo                                         */
/*                      FIFO Number                                             */
/*  <Return  >      Positive Value      Received data size                      */
/*                  Negative Value      Error                                   */
/*  <Description> Receive Bulk Out data from Hos                                */
/********************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_VendRead( unsigned char *buf, unsigned long Size, unsigned char nfifo)
{
    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    return( Bulk_Read(buf, Size, nfifo, &VendOut_Trans) );
}
#endif /* M-7M */


/************************************************************************/
/* <Function>   long BypassSendLength                                   */
/* <Arguments>  unsigned long Size                                      */
/*                      Bypass In Transfer Size                         */
/*              IN_TRAN_DATA * In_Trans                                 */
/*                      In Transfer control table pointer               */
/* <Return  >   MWUSB_OK        OK                                      */
/*              Negative Value      Error                               */
/* <Description> Send ByPass Mode Length                                */
/************************************************************************/
/* 
 * Do not use this function in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
static long BypassSendLength(unsigned long Size, IN_TRAN_DATA * In_Trans)
{
    long rtn = 0;
    // unsigned long flg_ptn, rtn_ptn;
    unsigned long Length;
    //int  stat;
    
    Length = Size;
    In_Trans->phase = USB_SEND;
    In_Trans->inDataPoint  = (unsigned char*)&Length;       /* Send buffer pointer  */
    In_Trans->sizeOfdata = 4;           /* Send size (Byte)     */

    /* reset global flag */
    global_in_ptn = 0;
    
    /* CPU Transfer */
    /* UnMask IntReady1 Interrupt */
    In_Interrupt_UnMask(In_Trans->epno);
    
    if( (global_in_ptn & (FLG_USB_DisConnected | FLG_USB_ClassTerm | FLG_USB_UnConf)) != 0x00 )
    {   /* DisConnect or Terminate */
        rtn = MWUSB_ERR_QUIT;
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
    else if( MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG )
    {   /* UnConfigured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( (global_in_ptn & FLG_USB_ClassReset) != 0x00 )
    {   /* Class Reset */
        rtn = MWUSB_ERR_CLSRESET;
    }
    else if( (global_in_ptn & FLG_USB_Cancel) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_CANCEL;
    }
    else if( (global_in_ptn & FLG_USB_TimeOut) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_TOUT;
    }
    else if( (global_in_ptn & FLG_USB_BulkWriteRdy) != 0x00 )
    {   /* Transfer Complete */
        rtn = MWUSB_OK;
    }
    if( rtn != MWUSB_OK )
    {   /* Error */
#ifdef USE_UART_MESSAGE
//      RS_printf("IN Err:%d ptn:%x\n", rtn, rtn_ptn);
#endif
        In_Trans->phase = USB_IDLE;
    }

    return rtn;
}
#endif /* M-7M */


/********************************************************************************/
/*  <Function>  long Bulk_Write                                                 */
/*  <Arguments> unsigned char *buf                                              */
/*                  Buffer pointer of receive data.                             */
/*              unsigned short Size                                             */
/*                   Send data size (byte)                                      */
/*              unsigned char nfifo                                             */
/*                  FIFO Number [BULK_WRITE_FIFO only]                          */
/*              IN_TRAN_DATA * In_Trans                                         */
/*                      In Transfer control table pointer                       */
/*  <Return  >  Positive Value      sent data size                              */
/*              Negative Value      Error                                       */
/*  <Description> Send Bulk In data to Host                                     */
/********************************************************************************/
static long Bulk_Write(unsigned char *buf,
                       unsigned long Size,
                       unsigned char nfifo,
                       IN_TRAN_DATA * In_Trans)
{
#if 0
    typedef struct {
        long             lTelegramsSize;
        long             lReturnValue;
        unsigned char    chFunctionID;    // Function ID
        char             chReserved[3];   // reserved
        long             lResultSize;     // data size
    } S_RES_TEST_TOOL_H;
#endif
    
    //int  stat;
    long rtn=0;
    // unsigned long flg_ptn, rtn_ptn;
    
    
#ifdef USB_DEBUG2
    printf("I");
#endif
    
    /* Buffer address alignment check */
    if( ((unsigned long)buf & 0x00000003) != 0x00000000 ) 
    {
#ifdef USE_UART_MESSAGE
        printf("USB In Alignment Error\n\r");
#endif
        rtn = MWUSB_ERR_ALLIGN;
    }
    else if( mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE )
    {
        rtn = MWUSB_ERR_QUIT; /* Disconnect or Terminate */
    }
    else if( (nfifo != BULK_WRITE_FIFO) && (nfifo != VEND_WRITE_FIFO) )
    {   /* Bad Endpoint */
        rtn = MWUSB_ERR_BADEP;
    }
    else if( (Size == 0)||(Size >= 0x10000000) )
    {   /* DMA Limitation 16MB */
        rtn = MWUSB_ERR_SIZE;
    }
    else if( In_Trans->phase != USB_IDLE )
    {   /* Not IDLE State */
        rtn = MWUSB_ERR_NOTIDLE;
    }
    else if( MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG )
    {   /* Not Configured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
    if( rtn < 0 )
    {   /* Error */
#ifdef USE_UART_MESSAGE
        printf("USB In Error\n\r");
#endif
        return rtn;
    }

#if 0 /* M-7M */
    if( BYPS_Mode == MWUSB_BYPASS_MODE )
    {
        /***************/
        /* BYPASS Mode */
        /***************/
        
        /* Trans Data Length */
        rtn = BypassSendLength(Size, In_Trans);
        if( rtn < 0 )
        {   // Error
            return rtn;
        }
    }
#endif /* M-7M */
    
    /* Initialize IN-Trans. Mangement Data */
    In_Trans->phase = USB_SEND;
    In_Trans->inDataPoint  = buf;       /* Send buffer pointer  */
    In_Trans->sizeOfdata = Size;            /* Send size (Byte)     */


    if( (IN_DMA_Mode == MWUSB_DMA_NOUSE) || (Size < In_Trans->sizeOfpacket) || (((unsigned long)buf % 4) > 0) )
    {   /* CPU Transfer */
        /* UnMask EP interrupt */
        (void)In_Interrupt_UnMask(In_Trans->epno);
    }
    //////////k.maeda 2011/01/07
    else
    {   /* DMA Transfer */
        if( usb_FIFO_write_dma(In_Trans) < 0 )
        {
            /* Function Error */
            rtn = MWUSB_ERR_QUIT;
            return(rtn);
        }
    }

    if( mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE )
    {
        rtn = MWUSB_ERR_QUIT; /* Disconnect or Terminate */
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
    else if( (global_in_ptn & FLG_USB_UnConf) != 0x00 )
    {   /* UnConfigured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( (global_in_ptn & FLG_USB_ClassReset) != 0x00 )
    {   /* Class Reset */
        rtn = MWUSB_ERR_CLSRESET;
    }
    else if( (global_in_ptn & FLG_USB_Cancel) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_CANCEL;
    }
    else if( (global_in_ptn & FLG_USB_TimeOut) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_TOUT;
    }
    else if( ( (global_in_ptn & FLG_USB_BulkWriteRdy) != 0x00 ) ||
             ( (global_in_ptn & FLG_USB_VendWriteRdy) != 0x00 )    )
    {   /* Transfer Complete */
        rtn = (long)Size;
    }
    if( rtn <= 0 )
    {   /* Error */
#ifdef USE_UART_MESSAGE
        printf("IN Err:%ld ptn:%lx\n", rtn, global_in_ptn);
#endif
        In_Trans->phase = USB_IDLE;
    }


#ifdef USB_DEBUG2
    printf("i");
#endif

    return( rtn );
}



/********************************************************************************/
/*  <Function>  long MWUSB_Write                                                */
/*  <Arguments> unsigned char *buf                                              */
/*                  Buffer pointer of receive data.                             */
/*              unsigned short Size                                             */
/*                   Send data size (byte)                                      */
/*              unsigned char nfifo                                             */
/*                  FIFO Number [BULK_WRITE_FIFO only]                          */
/*  <Return  >  Positive Value      sent data size                              */
/*              Negative Value      Error                                       */
/*  <Description> Send Bulk In data to Host                                     */
/********************************************************************************/
long MWUSB_Write( unsigned char *buf, unsigned long Size, unsigned char nfifo )
{
//    /* Pointer-type argument NULL check */
//    if (buf == NULL) {
//        return MWUSB_ERR_NULLPOINTER;
//    }
//    InterID = BULK_ENDP1_ID00; /* Interrupt ID */
//    return( Bulk_Write(buf, Size, nfifo, &BulkIn_Trans) );

    long ret;

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
    ULONG  starttime = BF_Get_Time_us();
    ULONG  endtime;
#endif

    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    InterID = BULK_ENDP1_ID00; /* Interrupt ID */
    ret = Bulk_Write(buf, Size, nfifo, &BulkIn_Trans);

#ifdef USB_DRIVER_PERFORMANCE_EVALUATION
    endtime = BF_Get_Time_us();
    printf("MWUSB_Write(): Size = %lu, Time = %lu.\n", Size, endtime-starttime) ;
#endif

    return ret;
}



/********************************************************************************/
/*  <Function>  long MWUSB_VendWrite                                            */
/*  <Arguments> unsigned char *buf                                              */
/*                  Buffer pointer of receive data.                             */
/*              unsigned short Size                                             */
/*                   Send data size (byte)                                      */
/*              unsigned char nfifo                                             */
/*                  FIFO Number [BULK_WRITE_FIFO only]                          */
/*  <Return  >  Positive Value      sent data size                              */
/*              Negative Value      Error                                       */
/*  <Description> Send Bulk In data to Host                                     */
/********************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_VendWrite( unsigned char *buf, unsigned long Size, unsigned char nfifo )
{
    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    InterID = VEND_ENDP5_ID04; /* Interrupt ID */
    return( Bulk_Write(buf, Size, nfifo, &VendIn_Trans) );
}
#endif /* M-7M */


/************************************************************************/
/*  <Function>  long MWUSB_InterruptWrite                               */
/*  <Arguments> unsigned char *buf                                      */
/*                  Buffer pointer of send data.                        */
/*              unsigned short Size                                     */
/*                  Send data size (byte)                               */
/*  <Return  >  Positive Value      sent data size                      */
/*              Negative Value      Error                               */
/*  <Description> Send Bulk In data to Host                             */
/************************************************************************/
long MWUSB_InterruptWrite( unsigned char *buf, unsigned long Size )
{
    
    long rtn=0;
    // unsigned long flg_ptn, rtn_ptn;
    
    InterID = INTR_ENDP3_ID02; /* Interrupt ID */
    
    /* Pointer-type argument NULL check */
    if (buf == NULL) {
        return MWUSB_ERR_NULLPOINTER;
    }
    /* Buffer address alignment check */
    if( ((unsigned long)buf & 0x00000003) != 0x00000000 ) 
    {
#ifdef USE_UART_MESSAGE
        printf("USB Interrupt In Alignment Error\n\r");
#endif
        rtn = MWUSB_ERR_ALLIGN;
    }
    else if( Size == 0 )
    {   /* Can't Send Null Packet */
        rtn = MWUSB_ERR_SIZE;
    }
    else if( IntrIn_Trans.phase != USB_IDLE )
    {   /* Not IDLE State */
        rtn = MWUSB_ERR_NOTIDLE;
    }
    else if( mwusb_CheckConnect(0) == MWUSB_PLUG_RELEASE )
    {
        rtn = MWUSB_ERR_QUIT; /* Disconnect or Terminate */
    }
    else if( MWUSB_CheckConfig()!=(long)MWUSB_STAT_CONFIG )
    {   /* Not Configured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( Control_BusReset_Status == BUS_RESET_DOING )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
    if( rtn < 0 )
    {   /* Error */
#ifdef USE_UART_MESSAGE
        printf("USB Interrupt In Error\n\r");
#endif
        return rtn;
    }
    
    /* reset global flag */
    global_in_ptn = 0;
    
    /* Initialize IN-Trans. Mangement Data */
    IntrIn_Trans.phase = USB_SEND;
    IntrIn_Trans.inDataPoint  = buf;        /* Send buffer pointer  */
    IntrIn_Trans.sizeOfdata = Size;         /* Send size (Byte)     */

    /* UnMask EP interrupt */
    (void)In_Interrupt_UnMask(IntrIn_Trans.epno);

    if( (global_in_ptn & (FLG_USB_DisConnected | FLG_USB_ClassTerm)) != 0x00 )
    {   /* DisConnect or Terminate */
        rtn = MWUSB_ERR_QUIT;
    }
    else if( (global_in_ptn & FLG_USB_BusReset) != 0x00 )
    {   /* Bus Reset */
        rtn = MWUSB_ERR_RESET;
    }
    else if( (global_in_ptn & FLG_USB_UnConf) != 0x00 )
    {   /* UnConfigured */
        rtn = MWUSB_ERR_UNCONF;
    }
    else if( (global_in_ptn & FLG_USB_ClassReset) != 0x00 )
    {   /* Class Reset */
        rtn = MWUSB_ERR_CLSRESET;
    }
    else if( (global_in_ptn & FLG_USB_Cancel) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_CANCEL;
    }
    else if( (global_in_ptn & FLG_USB_TimeOut) != 0x00 )
    {   /* Cancel */
        rtn = MWUSB_ERR_TOUT;
    }
    else if( (global_in_ptn & FLG_USB_IntrWriteRdy) != 0x00 )
    {   /* Transfer Complete */
        rtn = (long)Size;
    }
    if( rtn <= 0 )
    {   /* Error */
        IntrIn_Trans.phase = USB_IDLE;
    }
    
    return rtn;
}



/************************************************************************/
/*  <Function>  void MWUSB_ReleaseWaitBulkIO                            */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Release Waiting Transfer End                          */
/************************************************************************/
void MWUSB_ReleaseWaitBulkIO(void)
{
    
    /* Mask NullPacket Receive Interrupt */
    //_USB_NULL_RCV = 0x00;
    
    /* Abort Transfer */
    MWUSB_AbortReset();
    
    /* Set Term Event Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_ClassTerm);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_ClassTerm);
#endif
    
    return;
}



/************************************************************************/
/*  <Function>  void MWUSB_ReleaseWaitAbort                             */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Release Waiting Transfer End                          */
/************************************************************************/
void MWUSB_ReleaseWaitAbort(void)
{
    
    /* Mask NullPacket Receive Interrupt */
    //_USB_NULL_RCV = 0x00;
    
    /* Set Term Event Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_ClassReset);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_ClassReset);
#endif
    
    return;
}



/************************************************************************/
/*  <Function>  void MWUSB_ReleaseWaitCancel                            */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Release Waiting Transfer End                          */
/************************************************************************/
void MWUSB_ReleaseWaitCancel(void)
{
    
    /* Mask NullPacket Receive Interrupt */
    // _USB_NULL_RCV = 0x00;
    
    /* Set Term Event Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_Cancel);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_Cancel);
#endif
    
    return;
}

/************************************************************************/
/*  <Function>  void MWUSB_ReleaseWaitTout                              */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Release Waiting Transfer End                          */
/************************************************************************/
void MWUSB_ReleaseWaitTout(void)
{
    
    /* Mask NullPacket Receive Interrupt */
    // _USB_NULL_RCV = 0x00;
    
    /* Set Term Event Flag */
#ifdef USB_NO_REALOS
    usb_set_flg( FID_USB , FLG_USB_TimeOut);
#else
    OS_User_Set_Flg( FID_USB , FLG_USB_TimeOut);
#endif
    
    return;
}


/*************************************************************************/
/*  <Function>  int MWUSB_CheckChargingPort                              */
/*  <Arguments> void                                                     */
/*  <Return  >  MWUSB_PORT_SDP      : Standard Downstream Port           */
/*              MWUSB_PORT_CDP      : Charging Downstream Port           */
/*              MWUSB_PORT_DCP      : Dedicated Charging Port            */
/*              MWUSB_PORT_SDP_TOUT : Standard Downstream Port & Timeout */
/*              MWUSB_PORT_CDP_TOUT : Charging Downstream Port & Timeout */
/*              MWUSB_PORT_DCP_TOUT : Dedicated Charging Port & Timeout  */
/*              MWUSB_NG            : Error                              */
/*  <Description>   Check the USB host port type                         */
/*************************************************************************/
//int MWUSB_CheckChargingPort(void)
//{
//    int RetCode;
//    int ret;
//    
//#ifdef USE_LOW_POWER
//    /* Clock of Device --> ON */
//    if (mwusb_PowerOn == MWUSB_NG) {
//        ret = HdcDev_PowerOnDevice();
//        if (ret != HDC_SUCCESS) {
//            return MWUSB_NG;
//        }
//        mwusb_PowerOn = MWUSB_OK;
//    }
//#endif /* USE_LOW_POWER */
//
//    ret = HdcDev_CheckChargingPort();
//    switch (ret) {
//    case HDC_PORT_SDP:
//        RetCode = MWUSB_PORT_SDP;
//        break;
//    case HDC_PORT_CDP:
//        RetCode = MWUSB_PORT_CDP;
//        break;
//    case HDC_PORT_DCP:
//        RetCode = MWUSB_PORT_DCP;
//        break;
//    case HDC_PORT_SDP_TOUT:
//        RetCode = MWUSB_PORT_SDP_TOUT;
//        break;
//    case HDC_PORT_CDP_TOUT:
//        RetCode = MWUSB_PORT_CDP_TOUT;
//        break;
//    case HDC_PORT_DCP_TOUT:
//        RetCode = MWUSB_PORT_DCP_TOUT;
//        break;
//    default:
//        RetCode = MWUSB_NG;
//        break;
//    }
//#ifdef USE_LOW_POWER
//    /* Clock of Device --> OFF */
//    (void)HdcDev_PowerOffDevice();
//    mwusb_PowerOn = MWUSB_NG;
//#endif /* USE_LOW_POWER */
//
//    return RetCode;
//}


/************************************************************************/
/*  <Function>  long MWUSB_PhyConnect                                   */
/*  <Arguments> void                                                    */
/*  <Return  >  MWUSB_PLUG_RELEASE : USB Plug Released                  */
/*              MWUSB_PLUG_CONNECT : USB Plug Connected                 */
/*  <Description> Connecting between LINK and PHY                       */
/************************************************************************/
long MWUSB_PhyConnect(void)
{
    long con;
#ifdef USE_LOW_POWER
    int  ret;
#endif /* USE_LOW_POWER */
    /* Check Connect or Disconnect */

#ifdef  USE_UART_MESSAGE    
    printf(" MWUSB_PhyConnect. Start.\n\r");
#endif
    
    con = mwusb_CheckConnect(0);
    if( con == MWUSB_PLUG_RELEASE )
    {
        /*  Plug Release */
        /*  Call Release function */
        MWUSB_PhyDisconnect();

#ifdef  USE_UART_MESSAGE    
        printf("==>USB Released.\n\r");
#endif
    }
    else
    {
        /*  Plug Connected  */
#ifdef USE_LOW_POWER
        /* Clock of Device --> ON */
        if (mwusb_PowerOn == MWUSB_NG) {
            ret = HdcDev_PowerOnDevice();
            if (ret == HDC_SUCCESS) {
                mwusb_PowerOn = MWUSB_OK;
            }
        }
#endif /* USE_LOW_POWER */

        HdcDev_PhyConnect();

#ifdef USE_UART_MESSAGE
        printf("==>USB Connected.\n\r");
#endif
    }
    return con;
}


/************************************************************************/
/*  <Function>  void MWUSB_PhyDisconnect                                */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description> Disconnecting between LINK and PHY                    */
/************************************************************************/
void MWUSB_PhyDisconnect(void)
{
#ifdef USE_LOW_POWER
	if (mwusb_PowerOn == MWUSB_OK) {
	    HdcDev_PhyDisconnect();
	}

    /* Clock of Device --> OFF */
    (void)HdcDev_PowerOffDevice();
    mwusb_PowerOn = MWUSB_NG;
#else
    HdcDev_PhyDisconnect();
#endif /* USE_LOW_POWER */
    return;
}


#ifdef USE_PTP_CLASS
/****************************************************************************************/
/*  <Function>  static short Make_DescriptorPacket                                      */
/*  <Arguments>     DESCRIPTOR_PACKET *Desc_Packet                                      */
/*                      Pointer to the device descriptor data                           */
/*                  unsigned short wLength                                              */
/*                      Required Data size                                              */
/*                  unsigned char *buf                                                  */
/*                      Send Data Buffer                                                */
/*  <Return  >      Length to Send Data                                                 */
/*  <Description>  Prepare to device descriptor data to buffer                          */
/****************************************************************************************/
static short Make_DescriptorPacket(DESCRIPTOR_PACKET *Desc_Packet,
                                   unsigned short wLength, unsigned char *buf)
{
    unsigned char i, len;
    unsigned char *dat, *dst;
    short size;
    
/* << 2005.01.21 Modify End */
    dst = buf;
    size = 0;
    while(1) 
    {
        if (!Desc_Packet->Descriptor) 
        {
            break ;
        }
        dat = Desc_Packet->Descriptor ;
        len = Desc_Packet->Descriptor[0];
        for (i = 0; i < len; i++) 
        {
             *dst = *dat ;
            size++;
            dat++ ;
            dst++;
        }
        if( size > wLength )
        {
            size = wLength;
            break;
        }
        Desc_Packet++ ;
    }
    return size;
}


// Because declared static didn't use extern for other program.
long MTP_ClassResponse(DEVICE_REQUEST *DeviceRequest, unsigned char *buf, short *size)
{
    long rtn = MWUSB_OK;
    short sendcount;

    switch( DeviceRequest->bRequest & 0x0ff)
    {
        case 0x67:
            sendcount = Make_DescriptorPacket((DESCRIPTOR_PACKET *) &Class0x67, 4, buf);
            break;
        default:
            rtn = MWUSB_NG;
            *buf = '\0';
            sendcount = 0;
            break ;
    }
    
    *size = sendcount;
    return rtn;
}
#endif 


/* 
 * Do not use this task in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
#ifdef BUG_OF_F_USB20HDC_LASP_MACRO_20111115
/************************************************************************/
/*  <Function>  void USB_ControlStall_Task                              */
/*  <Arguments> void                                                    */
/*  <Return  >  void                                                    */
/*  <Description>                                                       */
/************************************************************************/
void USB_ControlStall_Task(void)
{
    unsigned int   flg_ptn;
    OS_USER_FLGPTN rtn_ptn;
    unsigned char mode = 1;
    
    while(1)
    {
        flg_ptn = FLG_USB_HWStall; /* wait flag pattern */

        OS_User_Wai_Flg(FID_USB_CONTROL_STALL, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
        OS_User_Clr_Flg(FID_USB_CONTROL_STALL, ~(flg_ptn));

        // The following processings are performed, when driver isn't made to STALL by itself
        // test
        /* Stall setting by Macro for ControlEndpoint */
        if((FLG_USB_HWStall & rtn_ptn) == rtn_ptn)
        {
#ifdef BUG_CHECK_LED_RK6M_CONTROL_20111115
            /* universal port 11 rise */
            IO_CHIPTOP.PDR.bit.P31 = 1; /* LED on */
#endif
            /* Driver Disconnet(only enumeration) */
            mwusb_intdisconnect(mode);

            OS_User_Tslp_Tsk(TMOUT_1MS); /* wait 1ms */

            /* Driver Reconnect */
            mwusb_initialize(0,0);
        }
    }
}
#else
void USB_ControlStall_Task(void)
{
}
#endif
#endif /* M-7M */

