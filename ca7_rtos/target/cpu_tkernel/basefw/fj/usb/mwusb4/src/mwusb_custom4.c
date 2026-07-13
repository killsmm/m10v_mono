/**
 * @file		mwusb_custom4.c
 * @brief		USB-Driver for F_USB20HDC_LSP Macro user custom function part
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USB_Section.h"  /* pragma setting */
#include "usb_user_wrapper.h"

#include "os_user_custom.h"
#include "mwusb_driver4.h"
#include "mwusb_custom4.h"
#ifdef USE_MS_CLASS
#include "msclib.h"
#endif


/* for Configuration Descriptor */
#define CONFIG_DEF_STORAGE 1
#define CONFIG_DEF_HID     1
#define CONFIG_DEF_PBRIDGE 1

/* for association between Report Descriptor and Interface Number */
#define INDEX_DUMMY             0
#define INDEX_RESPONDING_TO_IF0 0


/* switch to ptp class use */
#ifdef USE_PTP_CLASS
    #include "PTP.h"
//    #include "SIC.h"
    #include "MessageQueue.h"       /* PTP/PictBridge message queue header */
    #include "fj_pictbridge.h"      /* PictBridge main header */
#endif

extern unsigned char MSCS_LUN;				/*  Number of drives add 2011.01.04 by tomida */


#ifdef USE_MS_CLASS
/* Device Descriptor Data for Mass Storage mode */
static const MWUSB_DEVICE_DESCRIPTOR   DeviceDescriptor_storage __attribute__((aligned(4))) = {
    0x12,   /* Discriptor size      */
    0x01,   /* Discriptor type      */
    0x0200, /* USB Release number   */
    0,      /* Class code           */
    0,      /* Sub class code       */
    0,      /* Protocol code        */
    64,      /* Packet size          */
    0x2B88, /* VendorID   (0x2B88:Vendor Socionext) */
    0x0004, /* ProductsID (0x0004) */
    0x0100, /* ReleaseNum           */
    1,      /* Index of Strings for Manufacturer  */
    2,      /* Index of Strings for Product       */
    3,      /* Index of Strings for S/N           */
    1       /* Number of Config     */
} ;


/* ConfigDescriptor Packet for Storage (FullSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE ConfigDescriptor_storage __attribute__((aligned(4))) = {
  { /* ConfigDescriptor */
    0x09,   /* size                 */
    0x02,   /* type                 */
    (9 + 9 + 7 + 7),    /* 9 + 9 + 7 + 7 */
    1,      /* interface count      */
    CONFIG_DEF_STORAGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
    0x32    /* bMaxPower */  /* Set bMaxPower to non-zero value */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    2,      /* count of endpoint */
    0x08,   /* Class = Mass Storage class*/
    0x06,   /* sub class = SCSI */
    0x50,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    0       /* bInterval */
  },
  { /* EP2_Descriptor : Bulk OUT */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    0       /* bInterval */
  }
};

/* ConfigDescriptor Packet for Storage (HighSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE ConfigDescriptor_storage_HS __attribute__((aligned(4))) = {
  { /* ConfigDescriptor */
    0x09,   /* size                 */
    0x02,   /* type                 */
    (9 + 9 + 7 + 7),    /* 9 + 9 + 7 + 7 */
    1,      /* interface count      */
    CONFIG_DEF_STORAGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
//	0xFA    /* bMaxPower */  /* Set bMaxPower to non-zero value */
    0x32    /* bMaxPower */  /* Set bMaxPower to 100mA */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    2,      /* count of endpoint */
    0x08,   /* Class = Mass Storage class*/
    0x06,   /* sub class = SCSI */
    0x50,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    0       /* bInterval */
  },
  { /* EP2_Descriptor : Bulk OUT */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    0       /* bInterval */
  }
};

/* Other ConfigDescriptor Packet for Storage (FullSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE OtherConfigDescriptor_storage __attribute__((aligned(4))) = {
  {
    0x09,   /* size                 */
    0x07,   /* type                 */
    (9 + 9 + 7 + 7),    /* 9 + 9 + 7 + 7 */
    1,      /* interface count      */
    CONFIG_DEF_STORAGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
    0x32    /* bMaxPower */  /* Set bMaxPower to non-zero value */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    2,      /* count of endpoint */
    0x08,   /* Class = Mass Storage class*/
    0x06,   /* sub class = SCSI */
    0x50,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN : High Speed */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    0       /* bInterval */
  },
  { /* EP2_Descriptor : Bulk OUT : High Speed */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    0       /* bInterval */
  }
} ;

/* Other ConfigDescriptor Packet for Storage (HighSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE OtherConfigDescriptor_storage_HS __attribute__((aligned(4))) = {
  {
    0x09,   /* size                 */
    0x07,   /* type                 */
    (9 + 9 + 7 + 7),    /* 9 + 9 + 7 + 7 */
    1,      /* interface count      */
    CONFIG_DEF_STORAGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
//	0xFA    /* bMaxPower */  /* Set bMaxPower to non-zero value */
    0x32    /* bMaxPower */  /* Set bMaxPower to 100mA */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    2,      /* count of endpoint */
    0x08,   /* Class = Mass Storage class*/
    0x06,   /* sub class = SCSI */
    0x50,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN : FullSpeed */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    0       /* bInterval */
  },
  { /* EP2_Descriptor : Bulk OUT : FullSpeed */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    0       /* bInterval */
  }
};
#endif

#ifdef USE_HID_CLASS
/* Device Descriptor Data for HID mode */
static const MWUSB_DEVICE_DESCRIPTOR   DeviceDescriptor_HID __attribute__((aligned(4))) = {
    0x12,   /* Discriptor size      */
    0x01,   /* Discriptor type      */
    0x0200, /* USB Release number   */
    0,      /* Class code           */
    0,      /* Sub class code       */
    0,      /* Protocol code        */
    64,      /* Packet size          */
    0x2B88, /* VendorID   (0x2B88:Vendor Socionext) */
    0x0002, /* ProductsID (0x0002) */
    0x0100, /* ReleaseNum           */
    1,      /* Index of Strings for Manufacturer  */
    2,      /* Index of Strings for Product       */
    3,      /* Index of Strings for S/N           */
    1       /* Number of Config     */
} ;

/* ConfigDescriptor Packet for HID (Battery function) mode */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_HID ConfigDescriptor_HID_BAT __attribute__((aligned(4))) = {
  { /* ConfigDescriptor */
    0x09,   /* size                 */
    0x02,   /* type                 */
    (9 + 9 + 9 + 7 ),    /* 9 + 9 + 9 + 7        */
    1,      /* interface count      */
    CONFIG_DEF_HID,      /* configration number  */
    0,      /* string index */
//	0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
	0xA0,   /* bmAttributes(b7:"1" BusPower)  */
	0xFA    /* bMaxPower */  /* Set bMaxPower to non-zero value */
  },
  { /* IF_Descriptor */
    0x09,   /* size                 */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    1,      /* count of endpoint */
    0x03,   /* Class = HID Interface */
    0,      /* sub class = No Subclass */
    0,      /* protocol = None */
    0       /* strings index */
  },
  { /* HID_Descriptor */
    0x09,   /* size                 */
    0x21,   /* type */
    0x0111, /* HID Release number   */
    0x00,   /* Contry code (not supported) */
    0x01,   /* Number of descriptor (must be 1) */
    0x22,   /* Class Specific Descriptor Type = Report Descriptor */
    0x0013  /* Class Specific Descriptor Length = 0x0013 */
  },
  { /* EP1_Descriptor : Interrupt IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x3,    /* Interrupt mode */
    0x0008, /* packet size (8)*/
    10      /* 10msec */
  }
} ;

/* Other ConfigDescriptor Packet for HID (Battery function) mode */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_HID OtherConfigDescriptor_HID_BAT __attribute__((aligned(4))) = {
  {
    0x09,   /* size                 */
    0x07,   /* type                 */
    (9 + 9 + 9 + 7 ),    /* 9 + 9 + 9 + 7        */
    1,      /* interface count      */
    CONFIG_DEF_HID,      /* configration number  */
    0,      /* string index */
//    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
	0xA0,   /* bmAttributes(b7:"1" BusPower)  */
    0xFA    /* bMaxPower */  /* Set bMaxPower to non-zero value */
  },
  { /* IF_Descriptor */
    0x09,   /* size                 */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    1,      /* count of endpoint */
    0x03,   /* Class = HID Interface */
    0,      /* sub class = No Subclass */
    0,      /* protocol = None */
    0       /* strings index */
  },
  { /* HID_Descriptor */
    0x09,   /* size                 */
    0x21,   /* type */
    0x0111, /* HID Release number   */
    0x00,   /* Contry code (not supported) */
    0x01,   /* Number of descriptor (must be 1) */
    0x22,   /* Class Specific Descriptor Type = Report Descriptor */
    0x0013  /* Class Specific Descriptor Length = 0x0013 */
  },
  { /* EP1_Descriptor : Interrupt IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x3,    /* Interrupt mode */
    0x0008, /* packet size (8)*/
    10      /* 10msec */
  }
} ;

static unsigned char ReportDescriptor_HID_BAT[] = {
    0x05, 0x01,    /* UsagePage (Generic Desktop)     */
    0x09, 0x00,    /* Usage (Undefined)               */
    0xA1, 0x01,    /* Collection (Application)        */
    0x09, 0x00,    /* Usage (Undefined)               */
    0x15, 0x00,    /* Logical Minimum (0)             */
    0x25, 0x01,    /* Logical Maximum (1)             */
    0x75, 0x08,    /* Report Size (8)                 */
    0x95, 0x01,    /* Report Count (1)                */
    0x81, 0x02,    /* Input (Data Variable Absolute)  */
    0xC0        /* End Collection                  */
} ;
#endif

#ifdef USE_PTP_CLASS
/* Device Descriptor Data for PictBridge mode */
static const MWUSB_DEVICE_DESCRIPTOR   DeviceDescriptor_pictbridge __attribute__((aligned(4))) = {
    0x12,   /* Discriptor size      */
    0x01,   /* Discriptor type      */
    0x0200, /* USB Release number   */
    0,      /* Class code           */
    0,      /* Sub class code       */
    0,      /* Protocol code        */
    64,      /* Packet size          */
    0x2B88, /* VendorID   (0x2B88:Vendor Socionext) */
    0x0001, /* ProductsID (0x0001) */
    0x0100, /* ReleaseNum           */
    1,      /* Index of Strings for Manufacturer  */
    2,      /* Index of Strings for Product       */
    3,      /* Index of Strings for S/N           */
    1       /* Number of Config     */
} ;

/* ConfigDescriptor Packet for PictBridge mode (FullSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE ConfigDescriptor_pictbridge __attribute__((aligned(4))) = {
  { /* ConfigDescriptor */
    0x09,   /* size                 */
    0x02,   /* type                 */
    (9 + 9 + 7 + 7 + 7 ),    /* 9 + 9 + 7 + 7 + 7        */
    1,      /* interface count      */
    CONFIG_DEF_PBRIDGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
    0x32    /* bMaxPower */  /* Set bMaxPower to non-zero value */
  },
  { /* IF_Descriptor */
    0x09,   /* size                 */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    3,      /* count of endpoint */
    0x06,   /* Class = Image Interface */
    0x01,   /* sub class = Still Image Capture Device */
    0x01,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    30      /* 30msec */
  },
  { /* EP2_Descriptor : Bulk OUT */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    30      /* 10msec */
  },
  { /* EP3_Descriptor : Interrupt IN */
    0x07,   /* size */
    0x05,   /* type */
    0x83,   /* IN endpoint = 3 (FIFO3)*/
    0x3,    /* INTERRUPT mode */
    0x0040, /* packet size (64)*/
    30      /* 10msec */
  }
} ;

/* ConfigDescriptor Packet for PictBridge mode (HighSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE ConfigDescriptor_pictbridge_HS __attribute__((aligned(4))) = {
  { /* ConfigDescriptor */
    0x09,   /* size                 */
    0x02,   /* type                 */
    (9 + 9 + 7 + 7 + 7 ),    /* 9 + 9 + 7 + 7 + 7        */
    1,      /* interface count      */
    CONFIG_DEF_PBRIDGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
//	0xFA    /* bMaxPower */  /* Set bMaxPower to non-zero value */
	0x32    /* bMaxPower */  /* Set bMaxPower to 100mA */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    3,      /* count of endpoint */
    0x06,   /* Class = Image Interface */
    0x01,   /* sub class = Still Image Capture Device */
    0x01,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    10      /* bInterval */
  },
  { /* EP2_Descriptor : Bulk OUT */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    10      /* bInterval */
  },
  { /* EP3_Descriptor : Interrupt IN */
    0x07,   /* size */
    0x05,   /* type */
    0x83,   /* IN endpoint = 3 (FIFO3)*/
    0x3,    /* INTERRUPT mode */
    0x0040, /* packet size (64)*/
    9       /* bInterval */
  }
} ;

/* Other ConfigDescriptor Packet for PictBridge mode (FullSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE OtherConfigDescriptor_pictbridge __attribute__((aligned(4))) = {
  {
    0x09,   /* size                 */
    0x07,   /* type                 */
    (9 + 9 + 7 + 7 + 7 ),    /* 9 + 9 + 7 + 7 + 7        */
    1,      /* interface count      */
    CONFIG_DEF_PBRIDGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
    0x32    /* bMaxPower */  /* Set bMaxPower to non-zero value */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    3,      /* count of endpoint */
    0x06,   /* Class = Image Interface */
    0x01,   /* sub class = Still Image Capture Device */
    0x01,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    10      /* bInterval */
  },
  { /* EP2_Descriptor : Bulk OUT */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0200, /* packet size (512)*/
    10      /* bInterval */
  },
  { /* EP3_Descriptor : Interrupt IN */
    0x07,   /* size */
    0x05,   /* type */
    0x83,   /* IN endpoint = 3 (FIFO3)*/
    0x3,    /* INTERRUPT mode */
    0x0040, /* packet size (64)*/
    9       /* bInterval */
  }
} ;

/* Other ConfigDescriptor Packet for PictBridge mode (HighSpeed) */
/* It is necessary to store the descriptor in a consecutive memory area. */
static MWUSBC_CONFIG_DESCRIPTOR_STORAGE OtherConfigDescriptor_pictbridge_HS __attribute__((aligned(4))) = {
  {
    0x09,   /* size                 */
    0x07,   /* type                 */
    (9 + 9 + 7 + 7 + 7 ),    /* 9 + 9 + 7 + 7 + 7        */
    1,      /* interface count      */
    CONFIG_DEF_PBRIDGE,      /* configration number  */
    0,      /* string index */
    0xC0,   /* 0xC0,*/   /* Power : Self Power only & No remote wakeup */
//	0xFA    /* bMaxPower */  /* Set bMaxPower to non-zero value */
	0x32    /* bMaxPower */  /* Set bMaxPower to 100mA */
  },
  { /* IF_Descriptor */
    0x09,   /* size */
    0x04,   /* type */
    0,      /* index of interface */
    0,      /* AlternateSetting */
    3,      /* count of endpoint */
    0x06,   /* Class = Image Interface */
    0x01,   /* sub class = Still Image Capture Device */
    0x01,   /* protocol = Bulk-Only */
    0       /* strings index */
  },
  { /* EP1_Descriptor : Bulk IN */
    0x07,   /* size */
    0x05,   /* type */
    0x81,   /* IN endpoint = 1 (FIFO1)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    30      /* 30msec */
  },
  { /* EP2_Descriptor : Bulk OUT */
    0x07,   /* size */
    0x05,   /* type */
    0x02,   /* OUT endpoint = 2 (FIFO2)*/
    0x2,    /* BULK mode */
    0x0040, /* packet size (64)*/
    30      /* 10msec */
  },
  { /* EP3_Descriptor : Interrupt IN */
    0x07,   /* size */
    0x05,   /* type */
    0x83,   /* IN endpoint = 3 (FIFO3)*/
    0x3,    /* INTERRUPT mode */
    0x0040, /* packet size (64)*/
    30      /* 10msec */
  }
} ;
#endif


/* DeviceQualifier  Descriptor */
static const DEVICE_Q_DESCRIPTOR   DeviceQualifierDescriptor __attribute__((aligned(4))) = {
    0x0A,     /* size                 */
    0x06,     /* type                 */
    0x0200,   /* USB Release number   */
    0,        /* Class code           */
    0,        /* Sub class code       */
    0,        /* Protocol code        */
    64,       /* Packet size          */
    1,        /* Number  */
    0         /* Reserve */
} ;

static const MWUSB_STRING_DESCRIPTOR sLANGID __attribute__((aligned(4))) = {
    0x04,    /* 2 + 2 */
    0x03,    /* fixed */
    {0x09,0x04}    /* 0x0409 */
};
static const DESCRIPTOR_PACKET LANGID[] __attribute__((aligned(4))) = {
    { (unsigned char *)&sLANGID } ,
    { (unsigned char *)NULL}    /* end of data */
};

static const MWUSB_STRING_DESCRIPTOR sIndex1 __attribute__((aligned(4))) = {                /* Manufacturer */
    18+2,    /* 18 + 2 */
    0x03,    /* fixed */
    {0x53,0x00,0x4F,0x00,0x43,0x00,0x49,0x00,0x4F,0x00,0x4E,0x00,0x45,0x00,0x58,0x00,0x54,0x00} /* SOCIONEXT by UNICODE */
};
static const DESCRIPTOR_PACKET Index1[] __attribute__((aligned(4))) = {
    { (unsigned char *)&sIndex1 },
    { (unsigned char *)NULL}    /* end of data */
};

static const MWUSB_STRING_DESCRIPTOR sIndex2 __attribute__((aligned(4))) = {    /* Product */
    58+2,
    0x03,
    {
		0x53,0x00,0x4F,0x00,0x43,0x00,0x49,0x00,0x4F,0x00,0x4E,0x00,0x45,0x00,0x58,0x00,0x54,0x00,0x20,0x00,	/* SOCIONEXT (20byte)	*/
		0x44,0x00,0x53,0x00,0x43,0x00,0x20,0x00,																/* DSC (8byte)			*/
		0x4D,0x00,0x69,0x00,0x6C,0x00,0x62,0x00,0x65,0x00,0x61,0x00,0x75,0x00,0x74,0x00,						/* Milbeaut(16byte)		*/
		0x20,0x00,0x53,0x00,0x65,0x00,0x72,0x00,0x69,0x00,0x65,0x00,0x73,0x00,									/*  Series(14byte)		*/
	},
};
static const DESCRIPTOR_PACKET Index2[] __attribute__((aligned(4))) = {
    { (unsigned char *)&sIndex2 },
    { (unsigned char *)NULL}    /* end of data */
};


static const MWUSB_STRING_DESCRIPTOR sIndex3 __attribute__((aligned(4))) = {                /* Product */
    32+2,
    0x03,
    {
		0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x31,0x00,
		0x30,0x00,0x34,0x00,0x43,0x00,0x35,0x00,0x32,0x00,0x30,0x00,0x32,0x00,0x31,0x00
	},
    /* 0000000104C52021 (32byte) */
};
static const DESCRIPTOR_PACKET Index3[] __attribute__((aligned(4))) = {
    { (unsigned char *)&sIndex3 },
    { (unsigned char *)NULL}    /* end of data */
};


#ifdef USE_MS_CLASS
/* Device Descriptor Packet for Storage (Full/High Speed) */
static DESCRIPTOR_PACKET   DeviceDescp_storage[] __attribute__((aligned(4))) = {
    { (unsigned char *)&DeviceDescriptor_storage},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Configuration Descriptor Packet for Storage */
static DESCRIPTOR_PACKET   ConfigDescp_storage[] __attribute__((aligned(4))) = {
    { (unsigned char *)&ConfigDescriptor_storage},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Configuration Descriptor Packet for Storage(HiSpeed) */
static DESCRIPTOR_PACKET   ConfigDescp_storage_HI[] __attribute__((aligned(4))) = {
    { (unsigned char *)&ConfigDescriptor_storage_HS},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Other Configuration Descriptor Packet for Storage(HiSpeed) */
static DESCRIPTOR_PACKET   Other_ConfigDescp_storage_HI[] __attribute__((aligned(4))) = {
    { (unsigned char *)&OtherConfigDescriptor_storage_HS},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Other Configuration Descriptor Packet for Storage*/
static DESCRIPTOR_PACKET   Other_ConfigDescp_storage[] __attribute__((aligned(4))) = {
    { (unsigned char *)&OtherConfigDescriptor_storage},
    { (unsigned char *)NULL}    /* end of data */
} ;
#endif

#ifdef USE_HID_CLASS
/* Device Descriptor Packet for HID mode */
static DESCRIPTOR_PACKET   DeviceDescp_HID[] __attribute__((aligned(4))) = {
    { (unsigned char *)&DeviceDescriptor_HID},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Configuration Descriptor Packet for HID (Battery function) mode */
static DESCRIPTOR_PACKET   ConfigDescp_HID_BAT[] __attribute__((aligned(4))) = {
    { (unsigned char *)&ConfigDescriptor_HID_BAT},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Other Configuration Descriptor Packet for HID (Battery function) mode */
static DESCRIPTOR_PACKET   Other_ConfigDescp_HID_BAT[] __attribute__((aligned(4))) = {
    { (unsigned char *)&OtherConfigDescriptor_HID_BAT},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Report Descriptor Packet for HID (Battery function) mode */
static DESCRIPTOR_PACKET   ReportDescp_HID_BAT[] __attribute__((aligned(4))) = {
    { (unsigned char *)&ReportDescriptor_HID_BAT},
    { (unsigned char *)NULL}    /* end of data */
} ;
#endif

#ifdef USE_PTP_CLASS
/* Device Descriptor Packet for PictBridge mode (Full/High Speed) */
static DESCRIPTOR_PACKET   DeviceDescp_pictbridge[] __attribute__((aligned(4))) = {
    { (unsigned char *)&DeviceDescriptor_pictbridge},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Configuration Descriptor Packet for PictBridge mode */
static DESCRIPTOR_PACKET   ConfigDescp_pictbridge[] __attribute__((aligned(4))) = {
    { (unsigned char *)&ConfigDescriptor_pictbridge},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Configuration Descriptor Packet for PictBridge mode (HiSpeed) */
static DESCRIPTOR_PACKET   ConfigDescp_pictbridge_HI[] __attribute__((aligned(4))) = {
    { (unsigned char *)&ConfigDescriptor_pictbridge_HS},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Other Configuration Descriptor Packet for PictBridge mode (HiSpeed) */
static DESCRIPTOR_PACKET   Other_ConfigDescp_pictbridge_HI[] __attribute__((aligned(4))) = {
    { (unsigned char *)&OtherConfigDescriptor_pictbridge_HS},
    { (unsigned char *)NULL}    /* end of data */
} ;

/* Other Configuration Descriptor Packet for PictBridge mode  */
static DESCRIPTOR_PACKET   Other_ConfigDescp_pictbridge[] __attribute__((aligned(4))) = {
    { (unsigned char *)&OtherConfigDescriptor_pictbridge},
    { (unsigned char *)NULL}    /* end of data */
} ;
#endif


/* Device Qualifer Descriptor Packet  */
static const DESCRIPTOR_PACKET   DeviceQualDescp[] __attribute__((aligned(4))) = {
    { (unsigned char *)&DeviceQualifierDescriptor},
    { (unsigned char *)NULL}    /* end of data */
} ;

static DESCRIPTOR_PACKET   *DeviceDescp_normal;
static DESCRIPTOR_PACKET   *ConfigDescp_normal;
static DESCRIPTOR_PACKET   *ConfigDescp_normal_HI;
static DESCRIPTOR_PACKET   *Other_ConfigDescp_normal;
static DESCRIPTOR_PACKET   *Other_ConfigDescp_normal_HI;
static DESCRIPTOR_PACKET   *ReportDescp_normal;

long UsbDeviceClass = USB_MODE_NONE;

#ifdef USE_PTP_CLASS
static unsigned char ucBusReadySignal = 0;

extern long MTP_ClassResponse(DEVICE_REQUEST *DeviceRequest, unsigned char *buf, short *size);

unsigned char GetBusReadySignal()
{
    return ucBusReadySignal;
}
#endif 

/****************************************************************************/
/*   <Function>     MWUSBC_GetUsbClass                                      */
/*   <Arguments>    void                                                    */
/*   <Return  >     Device Class Mode                                       */
/*   <Description>  Get the USB device class mode                           */
/****************************************************************************/
unsigned char MWUSBC_GetUsbClass(void)
{
	unsigned char UsbClass;
	UsbClass = (unsigned char)UsbDeviceClass;
	return UsbClass;
}

/****************************************************************************/
/*   <Function>     MWUSBC_SetUsbClass                                      */
/*   <Arguments>    unsigned char mode                                      */
/*                      Device Class Mode                                   */
/*   <Return  >     void                                                    */
/*   <Description>  Set the USB device class mode                           */
/****************************************************************************/
void MWUSBC_SetUsbClass(unsigned char mode)
{
	extern int mwusb_InitializeDriver;
	
    if (mwusb_InitializeDriver == MWUSB_NG) {
        switch (mode) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            UsbDeviceClass = (long)mode;
            break;
#endif
#ifdef USE_HID_CLASS
        case USB_MODE_HID:
            UsbDeviceClass = (long)mode;
            break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_SIC:
        case USB_MODE_WDM:
        case USB_MODE_PICTBRIDGE:
            UsbDeviceClass = (long)mode;
            break;
#endif
#ifdef USE_TEST_MODE
        case USB_MODE_INTRTEST:
            UsbDeviceClass = (long)mode;
            break;
#endif
        default:
            break;
        }
    }
    return;
}

/****************************************************************************/
/*   <Function>     MWUSBC_RegisterDescriptor								*/
/*   <Arguments>    void													*/
/*	 <Return  >     MWUSB_OK  normal_end									*/
/*					MWUSB_NG  error_end										*/
/*   <Description>  Register the USB device class descriptors				*/
/****************************************************************************/
int MWUSBC_RegisterDescriptor(void)
{
    int    stat = MWUSB_OK;
    
    if (MWUSB_GetMode() == MWUSB_BYPASS_MODE) {
        /* not support */
    } else {
        /* Normal Mode */
        switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            /* Full-Speed Storage Mode */
            stat=MWUSB_RegisterDescriptor((void*)&DeviceDescriptor_storage,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_DEVICE,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ConfigDescriptor_storage,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_CONFIG,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sLANGID,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_LANGID,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex1,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_STRING1,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex2,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_STRING2,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex3,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_STRING3,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&DeviceQualifierDescriptor,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_DEVICE_QUALIFIER,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&OtherConfigDescriptor_storage,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_OTHER_SPEED_CONFIG,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);

            /* Hi-Speed Storage Mode */
            stat=MWUSB_RegisterDescriptor((void*)&DeviceDescriptor_storage,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_DEVICE,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ConfigDescriptor_storage_HS,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_CONFIG,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sLANGID,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_LANGID,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex1,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_STRING1,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex2,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_STRING2,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex3,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_STRING3,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&DeviceQualifierDescriptor,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_DEVICE_QUALIFIER,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&OtherConfigDescriptor_storage_HS,
                                            CONFIG_DEF_STORAGE,MWUSB_KIND_OTHER_SPEED_CONFIG,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);

            MWUSB_DefineDescriptor((DESCRIPTOR_PACKET *)DeviceDescp_storage,
                                   (DESCRIPTOR_PACKET *)ConfigDescp_storage_HI,
                                   (DESCRIPTOR_PACKET *)LANGID,
                                   (DESCRIPTOR_PACKET *)Index1,
                                   (DESCRIPTOR_PACKET *)Index2,
                                   (DESCRIPTOR_PACKET *)Index3,
                                   (DESCRIPTOR_PACKET *)DeviceQualDescp,
                                   (DESCRIPTOR_PACKET *)Other_ConfigDescp_storage_HI,
                                   (DESCRIPTOR_PACKET *)NULL);
            break;
#endif
#ifdef USE_HID_CLASS
        case USB_MODE_HID:
            /* Full-Speed HID Mode */
            stat=MWUSB_RegisterDescriptor((void*)&DeviceDescriptor_HID,
                                            CONFIG_DEF_HID,MWUSB_KIND_DEVICE,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ConfigDescriptor_HID_BAT,
                                            CONFIG_DEF_HID,MWUSB_KIND_CONFIG,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sLANGID,
                                            CONFIG_DEF_HID,MWUSB_KIND_LANGID,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex1,
                                            CONFIG_DEF_HID,MWUSB_KIND_STRING1,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex2,
                                            CONFIG_DEF_HID,MWUSB_KIND_STRING2,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex3,
                                            CONFIG_DEF_HID,MWUSB_KIND_STRING3,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&DeviceQualifierDescriptor,
                                            CONFIG_DEF_HID,MWUSB_KIND_DEVICE_QUALIFIER,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&OtherConfigDescriptor_HID_BAT,
                                            CONFIG_DEF_HID,MWUSB_KIND_OTHER_SPEED_CONFIG,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ReportDescriptor_HID_BAT,
                                            CONFIG_DEF_HID,MWUSB_KIND_REPORT,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_RESPONDING_TO_IF0);
            if( stat != MWUSB_OK )return(stat);

            /* Hi-Speed HID Mode */
            stat=MWUSB_RegisterDescriptor((void*)&DeviceDescriptor_HID,
                                            CONFIG_DEF_HID,MWUSB_KIND_DEVICE,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ConfigDescriptor_HID_BAT,
                                            CONFIG_DEF_HID,MWUSB_KIND_CONFIG,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sLANGID,
                                            CONFIG_DEF_HID,MWUSB_KIND_LANGID,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex1,
                                            CONFIG_DEF_HID,MWUSB_KIND_STRING1,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex2,
                                            CONFIG_DEF_HID,MWUSB_KIND_STRING2,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex3,
                                            CONFIG_DEF_HID,MWUSB_KIND_STRING3,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&DeviceQualifierDescriptor,
                                            CONFIG_DEF_HID,MWUSB_KIND_DEVICE_QUALIFIER,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&OtherConfigDescriptor_HID_BAT,
                                            CONFIG_DEF_HID,MWUSB_KIND_OTHER_SPEED_CONFIG,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ReportDescriptor_HID_BAT,
                                            CONFIG_DEF_HID,MWUSB_KIND_REPORT,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_RESPONDING_TO_IF0);
            if( stat != MWUSB_OK )return(stat);

            MWUSB_DefineDescriptor((DESCRIPTOR_PACKET *)DeviceDescp_HID,
                                   (DESCRIPTOR_PACKET *)ConfigDescp_HID_BAT,
                                   (DESCRIPTOR_PACKET *)LANGID,
                                   (DESCRIPTOR_PACKET *)Index1,
                                   (DESCRIPTOR_PACKET *)Index2,
                                   (DESCRIPTOR_PACKET *)Index3,
                                   (DESCRIPTOR_PACKET *)DeviceQualDescp,
                                   (DESCRIPTOR_PACKET *)Other_ConfigDescp_HID_BAT,
                                   (DESCRIPTOR_PACKET *)ReportDescp_HID_BAT);
            break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_SIC:
        case USB_MODE_WDM:
        case USB_MODE_PICTBRIDGE:
            /* Full-Speed PictBridge Mode */
            stat=MWUSB_RegisterDescriptor((void*)&DeviceDescriptor_pictbridge,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_DEVICE,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ConfigDescriptor_pictbridge,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_CONFIG,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sLANGID,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_LANGID,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex1,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_STRING1,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex2,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_STRING2,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex3,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_STRING3,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&DeviceQualifierDescriptor,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_DEVICE_QUALIFIER,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&OtherConfigDescriptor_pictbridge,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_OTHER_SPEED_CONFIG,
                                            MWUSB_FULLSPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);

            /* Hi-Speed PictBridge Mode */
            stat=MWUSB_RegisterDescriptor((void*)&DeviceDescriptor_pictbridge,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_DEVICE,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&ConfigDescriptor_pictbridge_HS,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_CONFIG,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sLANGID,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_LANGID,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex1,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_STRING1,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex2,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_STRING2,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&sIndex3,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_STRING3,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&DeviceQualifierDescriptor,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_DEVICE_QUALIFIER,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);
            stat=MWUSB_RegisterDescriptor((void*)&OtherConfigDescriptor_pictbridge_HS,
                                            CONFIG_DEF_PBRIDGE,MWUSB_KIND_OTHER_SPEED_CONFIG,
                                            MWUSB_HISPEED,MWUSB_NORMAL_MODE,
                                            INDEX_DUMMY);
            if( stat != MWUSB_OK )return(stat);

            MWUSB_DefineDescriptor((DESCRIPTOR_PACKET *)DeviceDescp_pictbridge,
                                   (DESCRIPTOR_PACKET *)ConfigDescp_pictbridge_HI,
                                   (DESCRIPTOR_PACKET *)LANGID,
                                   (DESCRIPTOR_PACKET *)Index1,
                                   (DESCRIPTOR_PACKET *)Index2,
                                   (DESCRIPTOR_PACKET *)Index3,
                                   (DESCRIPTOR_PACKET *)DeviceQualDescp,
                                   (DESCRIPTOR_PACKET *)Other_ConfigDescp_pictbridge_HI,
                                   (DESCRIPTOR_PACKET *)NULL);
            break;
#endif
        default:
            break;
        }
    }

    return( stat );
}



/****************************************************************************/
/*   <Function>     MWUSBC_DefineInterfaceDescriptor_storage				*/
/*   <Arguments>    long usb_mode											*/
/*	 <Return  >     condition : normal=0 / invalid usb_mode is (-1)			*/
/*   <Description>  Exchange IF Descriptot in Storage or PictBride mode		*/
/****************************************************************************/
long MWUSBC_DefineInterfaceDescriptor_storage(long usb_mode)
{
    long ret = MWUSB_OK;
    
    if ( MWUSB_GetMode() == MWUSB_BYPASS_MODE) {
        /* not support  */
    } else {
        switch(usb_mode) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            /* Storage Class */
//            UsbDeviceClass              = USB_MODE_STORAGE;
            DeviceDescp_normal	        = DeviceDescp_storage;
            ConfigDescp_normal          = ConfigDescp_storage;
            ConfigDescp_normal_HI       = ConfigDescp_storage_HI;
            Other_ConfigDescp_normal    = Other_ConfigDescp_storage;
            Other_ConfigDescp_normal_HI = Other_ConfigDescp_storage_HI;
            ReportDescp_normal          = NULL;
            break;
#endif
#ifdef USE_HID_CLASS
        case USB_MODE_HID:
            /* HID Class */
//            UsbDeviceClass              = USB_MODE_HID;
            DeviceDescp_normal          = DeviceDescp_HID;
            ConfigDescp_normal          = ConfigDescp_HID_BAT;
            ConfigDescp_normal_HI       = ConfigDescp_HID_BAT;
            Other_ConfigDescp_normal    = Other_ConfigDescp_HID_BAT;
            Other_ConfigDescp_normal_HI = Other_ConfigDescp_HID_BAT;
            ReportDescp_normal          = ReportDescp_HID_BAT;
            break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_SIC :
        case USB_MODE_WDM :
        case USB_MODE_PICTBRIDGE :
            DeviceDescp_normal          = DeviceDescp_pictbridge;
            ConfigDescp_normal          = ConfigDescp_pictbridge;
            ConfigDescp_normal_HI       = ConfigDescp_pictbridge_HI;
            Other_ConfigDescp_normal    = Other_ConfigDescp_pictbridge;
            Other_ConfigDescp_normal_HI = Other_ConfigDescp_pictbridge_HI;
            ReportDescp_normal          = NULL;
            break;
#endif
        default:
            ret = MWUSB_NG;
            break;
        }
    }
    
    return ret;
}


/****************************************************************************/
/*   <Function>     void MWUSBC_DefineDescriptor							*/
/*   <Arguments>    void									              	*/
/*	 <Return  >     void													*/
/*   <Description>  Process of Interrupt at connecting						*/
/****************************************************************************/
void MWUSBC_DefineDescriptor(void)
{
    unsigned long crtSpeed;
    
    crtSpeed = MWUSB_GetSpeed();
    
    if (MWUSB_GetMode() == MWUSB_BYPASS_MODE) {
        /* not support  */
    } else  {   //  Normal(Storage or PictBridge[PTP][SIC] or Vendor USB_TEST_FUNCTION_MODE)
        MWUSBC_DefineInterfaceDescriptor_storage(UsbDeviceClass);
        
        if (crtSpeed == MWUSB_HISPEED) {
            MWUSB_DefineDescriptor((DESCRIPTOR_PACKET *)DeviceDescp_normal,
                                   (DESCRIPTOR_PACKET *)ConfigDescp_normal_HI,
                                   (DESCRIPTOR_PACKET *)LANGID,
                                   (DESCRIPTOR_PACKET *)Index1,
                                   (DESCRIPTOR_PACKET *)Index2,
                                   (DESCRIPTOR_PACKET *)Index3,
                                   (DESCRIPTOR_PACKET *)DeviceQualDescp,
                                   (DESCRIPTOR_PACKET *)Other_ConfigDescp_normal_HI,
                                   (DESCRIPTOR_PACKET *)ReportDescp_normal);
        
        } else { /* crtSpeed == MWUSB_FULLSPEED */
            MWUSB_DefineDescriptor((DESCRIPTOR_PACKET *)DeviceDescp_normal,
                                   (DESCRIPTOR_PACKET *)ConfigDescp_normal,
                                   (DESCRIPTOR_PACKET *)LANGID,
                                   (DESCRIPTOR_PACKET *)Index1,
                                   (DESCRIPTOR_PACKET *)Index2,
                                   (DESCRIPTOR_PACKET *)Index3,
                                   (DESCRIPTOR_PACKET *)DeviceQualDescp,
                                   (DESCRIPTOR_PACKET *)Other_ConfigDescp_normal,
                                   (DESCRIPTOR_PACKET *)ReportDescp_normal);
        }
    }
    
    return;
}



/************************************************************************/
/*	<Function>		long MWUSBC_ClassRequest							*/
/*	<Arguments>		DEVICE_REQUEST *DeviceRequest						*/
/*						Device Request									*/
/*					unsigned char *buf									*/
/*						Receive or Send Data Buffer						*/
/*					short *size											*/
/*						Receive or Send Data size						*/
/*	<Return  >		MWUSB_OK	Support Request or Process OK			*/
/*					MWUSB_NG	UnSupport Request or Process NG			*/
/*						This value causes EP0 Stall						*/
/*	<Description> Operates Class Request(Mass Storage & PictBridge)		*/
/*					Mass Storage Class									*/
/*					1)Bulk-Only Protocol								*/
/*						a)Bulk-Only Mass Storage Reset					*/
/*						b)Get Max LUN									*/
/*					PictBridge											*/
/*						Call SIC_ClassRequest							*/
/*	<edit>			2011.01.04 j.tomida									*/
/*						MSCS_LUN extern reference.						*/
/************************************************************************/
long MWUSBC_ClassRequest(DEVICE_REQUEST *DeviceRequest, 
						 unsigned char *buf, short *size)
{
	long ret=MWUSB_OK;
#ifdef USE_MS_CLASS
	unsigned char MaxLUN = (MSCS_LUN-1);	/* MSCS_LUN is defined by MSC_Customxxx.c 2011.01.04 */
#endif

    switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
    case USB_MODE_STORAGE:
    	//  Mass Storage
    	switch( DeviceRequest->bRequest ) {
		case 0xFF:	/* Bulk-Only Mass storage Reset(Control OUT No Data Stage) */
			if( (DeviceRequest->wIndex == 0x00) &&
			   (DeviceRequest->wValue ==0x00) &&
			   (DeviceRequest->wLength == 0x00) )	// 2006.11.14 add for USBCV 1.3 MSC Test
			{	// OK
				/* To Do */
#ifdef    USE_UART_MESSAGE    
                printf("    CLSrequest => MSC Reset. \n\r");
                OS_User_Tslp_Tsk(50); /* wait time 100msec */
#endif
                /* Process at Mass storage Reset */
				MWUSB_AbortReset();		/* USB Bulk Transfer Abort */
                MWUSB_ReleaseWaitAbort();
				/* Stall Clear Bulk IN and Bulk OUT */
				MWUSB_EPStallClr(BULK_READ_FIFO);
				MWUSB_EPStallClr(BULK_WRITE_FIFO);
				/* Mass storage library reset */
				MSCM_Reset();
				ret = MWUSB_OK;
			}
			else
			{	// NG
				ret = MWUSB_NG;
			}
			break;

		case 0xFE:	/* Get Max LUN(Control IN) */
			/* Prepare Send Data */
			*size = DeviceRequest->wLength;
			if( (DeviceRequest->wIndex == 0x00)&&
			   (DeviceRequest->wValue ==0x00)&&
			   (DeviceRequest->wLength == 0x01) )	// 2006.11.14 add for USBCV 1.3 MSC Test
			{	// OK
				*buf = MaxLUN;
				ret = MWUSB_OK;
#ifdef    USE_UART_MESSAGE    
                printf("    CLSrequest => MSC Get MaxLUN(%d). \n\r",*buf);
                OS_User_Tslp_Tsk(50); /* wait time 100msec */
#endif
            }
			else
			{	// NG
				ret = MWUSB_NG;
			}
			break;

 		default:
			ret = MWUSB_NG;		/* Will be EP0 STALL */
			break;
		}
		break;
#endif

#ifdef USE_HID_CLASS
    case USB_MODE_HID:
        // HID (Battery function)
        switch( DeviceRequest->bRequest ) {
        case 0x01:  /* Get_Report Request */
            *size = DeviceRequest->wLength;
            if ( (DeviceRequest->wIndex == 0x0000) && 
                 (DeviceRequest->wValue == 0x0100) && 
                 (DeviceRequest->wLength == 0x0001) )
            {    // OK
                *buf = 0;
                ret = MWUSB_OK;
            }
            else
            {    // NG
                ret = MWUSB_NG;
            }
            break;
         default:
            ret = MWUSB_NG;
            break;
        }
        break;
#endif

#ifdef USE_PTP_CLASS
    case USB_MODE_SIC:
    case USB_MODE_WDM:
    case USB_MODE_PICTBRIDGE:
        // PictBridge[PTP][SIC]
        switch( DeviceRequest->bRequest ) {
        case 0x66:
            if ((DeviceRequest->wIndex == 0x00) && (DeviceRequest->wValue == 0x00) && (DeviceRequest->wLength == 0x00))
            {
                ret = MWUSB_OK;
                InitialPTP();
            }
            else
            {
                ret = MWUSB_NG;
            }
            break;
        case 0x67:
            if ((DeviceRequest->wIndex == 0x00) && (DeviceRequest->wValue == 0x00) && (DeviceRequest->wLength == 0x24))
            {
                ret = MTP_ClassResponse(DeviceRequest, buf, size);
            }
            else
            {
                ret = MWUSB_NG;
            }
            break;
         default:
            break;
        }

        /* In case of Control-IN
           Prepare data to be send in buff .
           Set size.
           
           In case od Control-OUT,
           Received data & size were set in buff & size.
        */   
//        ret = SIC_ClassRequest(DeviceRequest->bmRequestType, DeviceRequest->bRequest, 
//                               DeviceRequest->wLength, buf, size);
        break;
#endif
    default:
        ret = MWUSB_NG;
        break;
    }

	return ret;
}

/****************************************************************************/
/*	<Function>	long MWUSBC_VendorRequest									*/
/*	<Arguments>		DEVICE_REQUEST *DeviceRequest							*/
/*						Device Request										*/
/*					unsigned char *buf										*/
/*						Receive or Send Data Buffer							*/
/*					short *size												*/
/*						Receive or Send Data size							*/
/*	<Return  >		MWUSB_OK	Support Request or Process OK				*/
/*					MWUSB_NG	UnSupport Request or Process NG				*/
/*						This value causes EP0 Stall							*/
/*	<Description>	Operates Vendor Request(Tunning Mode) as followed.		*/
/*						Inquiry of Max Block Size to Receive				*/
/*						Inquiry of Vendor Request from Host PC				*/
/*						Inquiry of Timeout									*/
/*																			*/
/*						Bypass Send Request									*/
/*						Bypass Receive Request								*/
/****************************************************************************/
long MWUSBC_VendorRequest(DEVICE_REQUEST *DeviceRequest, 
						  unsigned char *buf, short *size)
{
    long ret = MWUSB_OK;
	
#if 0 /* not supported */
	/* Usb Tuning Mode */
	union _TransType {
		double			dbl;
		float			df[2];
		unsigned long	dd[2];
		unsigned short	dw[4];
		unsigned char	db[8];
	} *transtype;
	
	/* Vendor Request Analyze  */
	switch( DeviceRequest->bRequest )
	{
	case Cancel :	// Control OUT with No DATA Stage
		MWUSB_AbortReset();
		MWUSB_ReleaseWaitCancel();
		Cancel_flg = 1; /* Cancel Flag Set */
		ret = MWUSB_OK;
		break;
	case MSBInq :	// Inquiry of Max Block Size to Receive(Control IN)
		transtype = (union _TransType *)&buf[0];
		/* MAXBLOCK_SIZE; */
		transtype->dw[0] = (unsigned short)MAXBLOCKSIZE;
		/* REPEAT_SIZE; */
		transtype->dw[1] = (unsigned short)BLOCK_SIZE;
		*size = DeviceRequest->wLength;
		ret = MWUSB_OK;
		break;
	case BYPSON :	// Change Hi-speed BYPASS protocol mode(Control OUT wo Data)
		/* set to Bypass protocol mode */
		MWUSB_SetMode(MWUSB_BYPASS_MODE);
		MWUSBC_DefineDescriptor();
		ret = MWUSB_OK;
		break;
	case BYPSOFF :	// Change Normal protocol mode(Contol OUT wo Data)
		/* set to Normal protocol mode */
		MWUSB_SetMode(MWUSB_NORMAL_MODE);
		MWUSBC_DefineDescriptor();
		ret = MWUSB_OK;
		break;
	case Timeout:	// Control OUT wo Data
		/* Timeout occur during read/write by BYPASS Protocol */
		MWUSB_AbortReset();
		MWUSB_ReleaseWaitTout();
		TimeOut_flg = 1;
		Cancel_flg = 0;	/* Cancel Flag Reset */
		ret = MWUSB_OK;
		break;
 	default:
		ret = MWUSB_NG;
		break;
    }
#endif /* M-7M */
	return ret;
}



/****************************************************************************/
/*  <Function>  MWUSBC_ModeTest_Main										*/
/*  <Arguments>     														*/
/*  <Return  >      														*/
/*  <Description>   Main Task for Usb Test Mode								*/
/****************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSBC_ModeTest_Main(void)
{
	unsigned char KeepFlag = 1, status = 0, CurSts = USB_TEST_IDLE;
	
	while(KeepFlag)
	{
		OS_User_Dly_Tsk(1);
		status = MWUSBC_GetModeTest_Status(); /* Get status */

		if( CurSts != status )
		{
			switch(status)
			{
		    case USB_TEST_OPEN:
				/* Open Command */
		        if(gUsbTest_Callback != NULL)
				{
		            gUsbTest_Callback(0x41, USB_TEST_OPEN); //callback
				}
				{
		            //disconnect
		            MWUSB_IntDisconnect();
		            MSCM_End();
        
		            //change Class
		            MWUSBC_SetUsbClass(USB_MODE_TEST);
		            MWUSBC_ChangeUsbDescriptor();
		            //MWUSBC_DefineDescriptor();
            
		            //connect
		            MWUSB_IntConnect();
		        }
        
		        break;
        
		    case USB_TEST_CLOSE:
				/* Close Command */
		        if(gUsbTest_Callback != NULL)
				{
		            gUsbTest_Callback(0x41, USB_TEST_CLOSE); //callback
				}
				{
		            //disconnect
		            MWUSB_IntDisconnect();
        
		            //change Class
		            MWUSBC_SetUsbClass(USB_MODE_STORAGE);
		            MWUSBC_ChangeUsbDescriptor();
		            //MWUSBC_DefineDescriptor();
        
		            //connect
		            MWUSB_IntConnect();
		        }
	        	KeepFlag = 0; /* End of USB_MODE_TEST */
				MWUSBC_SetModeTest_Status(USB_TEST_IDLE); //set status
			
		        break;
        
		    case USB_TEST_START:
				/* Start Command */
	            if(gUsbTest_Callback != NULL)
				{
	                gUsbTest_Callback(0x41, USB_TEST_START); //callback
	            }
		        break;
        
		    default:
		        /* IDLE status */
	        	KeepFlag = 1; /* End of USB_MODE_TEST */
		    }
			CurSts = status;
		}
	}
	
    return;
}
#endif /* M-7M */



/****************************************************************************/
/*	<Function>		long MWUSB_CheckCancel( void )							*/
/*	<Arguments>		void													*/
/*	<Return  >		1 Cancel Request										*/
/*	<Description>	Check the cancel requested in bypass mode				*/
/****************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_CheckCancel( void )
{
	if (Cancel_flg == 1)
	{
		Cancel_flg = 0; /* flag Clear */
        return( 1 );    /* indicate that transfer has cancelled */
    }
	else
	{
		return( 0 );    /* indicate that transfer has not cancelled */
	}
}
#endif /* M-7M */



/****************************************************************************/
/*	<Function>		long MWUSB_CheckTimeOut( void )							*/
/*	<Arguments>		void													*/
/*	<Return  		1:Timeout Request										*/
/*	<Description>	Check the time-out requested in bypass mode				*/
/****************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
long MWUSB_CheckTimeOut( void )
{
	if (TimeOut_flg == 1)
	{
		TimeOut_flg = 0; /* flag Clear */
		return( 1 );    /* indicate that transfer has timeout */
	}
	else
	{
		return( 0 );    /* indicate that transfer has not timeout */
	}
}
#endif /* M-7M */



/****************************************************************************/
/*   <Function>     void MWUSBC_IntConnect(void)							*/
/*   <Arguments>    void									              	*/
/*	 <Return  >     void													*/
/*   <Description>  Process of Interrupt at connecting						*/
/****************************************************************************/
void MWUSBC_IntConnect(void)
{
#ifdef USE_MS_CLASS
    T_OS_USER_RTSK flag;
#endif
    
    if( MWUSB_GetMode() == MWUSB_BYPASS_MODE )
    {
        /* not support  */
    }
    else
    {
        switch(UsbDeviceClass)
        {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            MWUSB_SetMode(MWUSB_NORMAL_MODE); /* Normal mode */
            OS_User_Ref_Tsk(TID_USB_STORAGE, &flag);	
            if ((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT) {
                // Start Storage Task
                OS_User_Sta_Tsk(TID_USB_STORAGE,0);
            }
            break;
#endif
#ifdef USE_HID_CLASS
        case USB_MODE_HID:
            MWUSB_SetMode(MWUSB_NORMAL_MODE); /* Normal mode */
            break;
#endif
#ifdef USE_PTP_CLASS
       case USB_MODE_SIC:
       case USB_MODE_WDM:  /* PTP */
       case USB_MODE_PICTBRIDGE:
            MWUSB_SetMode(MWUSB_NORMAL_MODE); /* Normal mode */
            //SIC_NotifyStatus(SIC_NOTIFY_ATTACH, NULL);
            break;
#endif
        default:
            break;
        }
    }

    return;
}



/****************************************************************************/
/*   <Function>     void MWUSBC_IntDisconnect(void)							*/
/*   <Arguments>    void									              	*/
/*	 <Return  >     void													*/
/*   <Description>  Process of Interrupt at disconnecting					*/
/****************************************************************************/
void MWUSBC_IntDisconnect(void)
{
    switch(UsbDeviceClass) {
#ifdef USE_MS_CLASS
    case USB_MODE_STORAGE:
        MSCM_NotifyStatus(MSC_STS_UNCONF);
        break;
#endif
#ifdef USE_HID_CLASS
    case USB_MODE_HID:
        break;
#endif
#ifdef USE_PTP_CLASS
    case USB_MODE_SIC:
    case USB_MODE_WDM :
    case USB_MODE_PICTBRIDGE:
        //SIC_NotifyStatus(SIC_NOTIFY_DETACH, NULL);
        break;
#endif
    default:
        break;
    }
    
    /////// k.maeda 2011/02/21
    OS_User_Dly_Tsk(5); //wait for clear softreset
    
    return;
}


/****************************************************************************/
/*	<Function>		void MWUSBC_SoFRes( void )								*/
/*	<Arguments>		void													*/
/*	<Return  >		void													*/
/*	<Description>	SOF Interrupt											*/
/****************************************************************************/
void MWUSBC_SoFRes(void)
{
	return;
}


/****************************************************************************/
/*	<Function>		void MWUSBC_UsbBusResetStart( void )					*/
/*	<Arguments>		void													*/
/*	<Return  >		void													*/
/*	<Description>	Bus Reset Start Interrupt								*/
/****************************************************************************/
void MWUSBC_UsbBusResetStart(void)
{
#ifdef USE_MS_CLASS
	T_OS_USER_RTSK flag;
#endif
	
	if( MWUSB_GetMode() != MWUSB_BYPASS_MODE )
	{
        switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            //  Mass Storage
			/* Start task */
			OS_User_Ref_Tsk(TID_USB_STORAGE, &flag);	
			if((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT)
			{
				/* Start task */
				OS_User_Sta_Tsk(TID_USB_STORAGE, 0);
			}
			MSCM_NotifyStatus(MSC_STS_UNCONF);
			break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_PICTBRIDGE:
			//  PictBridge
			ucBusReadySignal = 0;
			ResetDPS();
			//SIC_NotifyStatus(SIC_NOTIFY_BUSRESET, NULL);
			break;
#endif
        default:
            break;
		}
    }

	return;
}



/****************************************************************************/
/*	<Function>		void MWUSBC_UsbBusResetEnd( void )						*/
/*	<Arguments>		void													*/
/*	<Return  >		void													*/
/*	<Description>	Bus Reset End Interrupt									*/
/****************************************************************************/
void MWUSBC_UsbBusResetEnd(void)
{
#ifdef USE_MS_CLASS
	T_OS_USER_RTSK flag;
#endif

	if( MWUSB_GetMode() != MWUSB_BYPASS_MODE )
	{
		switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
			//  Mass Storage
			/* Start task */
			OS_User_Ref_Tsk(TID_USB_STORAGE, &flag);	
			if((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT)
			{
				/* Start task */
				OS_User_Sta_Tsk(TID_USB_STORAGE, 0);
			}
			break;
#endif
        default:
            break;
		}
	}
	
	return;
}


/****************************************************************************/
/*	<Function>		void MWUSBC_SetConfig									*/
/*	<Arguments>		unsigned char confval									*/
/*						Current Configuration Value							*/
/*	<Return  >		void													*/
/*	<Description>	Set Configuration Interrupt								*/
/****************************************************************************/
void MWUSBC_SetConfig(unsigned char confval)
{
#ifdef USE_MS_CLASS
	T_OS_USER_RTSK flag;
#endif

    // API TEST
#ifdef USE_UART_MESSAGE
    short PkSize[5];
    
    printf("USB Configuerd %d \n\r",confval);
    PkSize[0] = MWUSB_GetMaxPacketSize(CNTL_READ_FIFO);
    PkSize[1] = MWUSB_GetMaxPacketSize(CNTL_WRITE_FIFO);
    PkSize[2] = MWUSB_GetMaxPacketSize(BULK_READ_FIFO);
    PkSize[3] = MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO);
    PkSize[4] = MWUSB_GetMaxPacketSize(INTR_WRITE_FIFO);
    
    printf("Cntl OUT EPBUF:%d bytes\n", PkSize[0]);
    printf("Cntl IN  EPBUF:%d bytes\n", PkSize[1]);
    printf("Bulk OUT EPBUF:%d bytes\n", PkSize[2]);
    printf("Bulk IN  EPBUF:%d bytes\n", PkSize[3]);
    printf("Intr IN  EPBUF:%d bytes\n", PkSize[4]);
    OS_User_Tslp_Tsk(10); /* wait time 50msec */
#endif    
    //if( Get_Act_Sysmode() == E_SYSMODE_USB )
    if( MWUSB_GetMode() != MWUSB_BYPASS_MODE )
    {
        switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            //  Mass Storage
			MWUSB_SetMode(MWUSB_NORMAL_MODE); /* Normal mode */
			MWUSBC_DefineDescriptor();
			if( confval == 1 )
			{	// Configured
				OS_User_Ref_Tsk(TID_USB_STORAGE, &flag);	
				if((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT)
				{
#ifdef    USE_UART_MESSAGE    
                    printf(" Begin  MassStorageClass Task.\n\r");
                    OS_User_Tslp_Tsk(10);
#endif
					/* Start task */
					OS_User_Sta_Tsk(TID_USB_STORAGE,0);
#ifdef    USE_UART_MESSAGE    
                    printf(" Wakeup MassStorageClass Task.\n\r");
                    OS_User_Tslp_Tsk(10);
#endif
				}
				//wait_storage_initialized();
				MSCM_NotifyStatus(MSC_STS_CONFIG);
			}
			else
			{	// Unconfigured
				MSCM_NotifyStatus(MSC_STS_UNCONF);
			}
			break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_SIC:
        case USB_MODE_WDM:
        case USB_MODE_PICTBRIDGE:
            //  PictBridge[PTP][SIC]
            MWUSB_SetMode(MWUSB_NORMAL_MODE); /* Normal mode */
            MWUSBC_DefineDescriptor();
            if( confval == 1 )
            {
                MSG outmsg;
                //SIC_NotifyStatus(SIC_NOTIFY_SETCONFIG, NULL);
                ucBusReadySignal = 1;
                outmsg.msgid = ePictBridgeSubState_Ptp_Connected;
                FJ_PictBridgeSendHostMessage(outmsg);
            }
            else
            {
                //SIC_NotifyStatus(SIC_NOTIFY_ATTACH, NULL);
            }
            break;
#endif
        default:
            break;
        }
    } 

	return;
}


/****************************************************************************/
/*	<Function>		void MWUSBC_SuspendStart( void )						*/
/*	<Arguments>		void													*/
/*	<Return  >		void													*/
/*	<Description>	Suspend Start Interrupt									*/
/*						Notify suspend start to Class Layer.				*/
/****************************************************************************/
void MWUSBC_SuspendStart(void)
{
	if( MWUSB_GetMode() != MWUSB_BYPASS_MODE )
	{
        switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
			// Mass Storage
			MSCM_NotifyStatus(MSC_STS_SUSPEND_START);
            break;
#endif
#ifdef USE_HID_CLASS
    case USB_MODE_HID:
        break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_PICTBRIDGE:
            //  PictBridge[PTP][SIC]
            ucBusReadySignal = 0;
            ResetDPS();
            //SIC_NotifyStatus(SIC_NOTIFY_SUSPEND, NULL);
            break;
#endif
        default:
            break;
		}
	}
#ifdef USE_UART_MESSAGE
    printf("  >Suspend Start\n");
    OS_User_Tslp_Tsk(10);
#endif
	return;
}

/****************************************************************************/
/*	<Function>		void MWUSBC_SuspendEnd( void )							*/
/*	<Arguments>		void													*/
/*	<Return  >		void													*/
/*	<Description>	Suspend End Interrupt									*/
/*						Notify suspend end to Class Layer.					*/
/****************************************************************************/
void MWUSBC_SuspendEnd(void)
{
	if( MWUSB_GetMode() != MWUSB_BYPASS_MODE )
	{
        switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
			// Mass Storage
			MSCM_NotifyStatus(MSC_STS_SUSPEND_END);
            break;
#endif
#ifdef USE_HID_CLASS
    case USB_MODE_HID:
        break;
#endif
#ifdef USE_PTP_CLASS
        case USB_MODE_SIC:
        case USB_MODE_WDM:
        case USB_MODE_PICTBRIDGE:
            //  PictBridge[PTP][SIC]
            //SIC_NotifyStatus(SIC_NOTIFY_SOF, NULL);
            break;
#endif
        default:
            break;
		}
	}
#ifdef USE_UART_MESSAGE
    printf("  <Suspend End\n");
    OS_User_Tslp_Tsk(10);
#endif
	return;
}



/****************************************************************************/
/*	<Function>		long MWUSBC_CheckConnect(unsigned char chatteringFlag)	*/
/*                      chattering signal removal flag                      */
/*	<Return  >		MWUSB_PLUG_CONNECT	Plug Connected,						*/
/*					MWUSB_PLUG_RELEASE	Plug Released						*/
/*	<Description>	Check USB Plug Conneted									*/
/*					This function strongly depends on the board and 		*/
/*					the system.												*/
/****************************************************************************/
long MWUSBC_CheckConnect(unsigned char chatteringFlag)
{
    long  retcond;
    long  tmp_chattering_flag;
    long  tmp_ret;
    
    if (chatteringFlag == 0) {
        tmp_chattering_flag = D_USB_USER_WRAP_OFF;
    }
    else
    {
        tmp_chattering_flag = D_USB_USER_WRAP_ON;
    }
    
    tmp_ret = USB_User_Wrapper_Check_Connect(tmp_chattering_flag);
    if(tmp_ret == D_USB_USER_PLUG_CONNECT)
    {
        retcond = MWUSB_PLUG_CONNECT;
    }
    else
    {
        retcond = MWUSB_PLUG_RELEASE;
    }
    
    return retcond;
}



/************************************************************************/
/*	<Function>	long MWUSBC_PlugWait									*/
/*	<Arguments>	void													*/
/*	<Return  >	void													*/
/*	<Description> USB Plug-in Wait & Reconnect	UserCustom				*/
/************************************************************************/
void MWUSBC_PlugWait( void )
{
    return;
}



/************************************************************************/
/*	<Function>	void MWUSBC_Clock_On   									*/
/*	<Arguments>	void													*/
/*	<Return  >	void													*/
/*	<Description> USB macro clock on  									*/
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 * It controls in HdcDev_PowerOnDevice in M-7M.
 */
#if 0 /* M-7M */
void MWUSBC_Clock_On(void)
{
#ifdef USB_LOW_POWER
    unsigned long reg_val;
    /***************************
    *   USB Clock On            *
    ***************************/
    // You can't use IO_USB, because register define overlaps with USBHDC_MemMap.h.
    // You can't use WriteRegister, because Big/Little is changed by byteOrder.
    //WriteRegister((USBHDC_REG_BASE + 0x0600), 0x03000000);    //HCKEN, PCKEN on
    //*(unsigned long *)0x84000600 = 0x03000000;                //HCKEN, PCKEN on

    WriteMemory32((u32)(USBHDC_CLKCTL), 0x03000000);                    //CLKCTL.HCKEN=1,CLKCTL.PCKEN=1
    {
        int i;
        for(i = 0; i < 5; i++){
            __nop();
        }
    }

    reg_val = ReadRegister(USBHDC_CONF);                                //Read CONF
    reg_val = (reg_val | 0x4);                                            //CONF.soft_reset=1
    WriteRegister(USBHDC_CONF, reg_val);                                //Write CONF
#endif
}
#endif  /* M-7M */


/************************************************************************/
/*	<Function>	void MWUSBC_Clock_Off   								*/
/*	<Arguments>	void													*/
/*	<Return  >	void													*/
/*	<Description> USB macro clock off  									*/
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 * It controls in HdcDev_PowerOffDevice in M-7M.
 */
#if 0 /* M-7M */
void MWUSBC_Clock_Off(void)
{
#ifdef USB_LOW_POWER
    unsigned long reg_val;
    /***************************
    *   USB Clock Off            *
    ***************************/
    // You can't use IO_USB, because register define overlaps with USBHDC_MemMap.h
    // You can't use WriteRegister, because Big/Little is changed by byteOrder
    //WriteRegister((USBHDC_REG_BASE + 0x0604), 0x00000000);    //SFTRST 
    //WriteRegister((USBHDC_REG_BASE + 0x0600), 0x00000000);    //HCKEN, PCKEN off

    WriteMemory32((u32)(USBHDC_RSTCTL), 0x0);                //SFTRST on
    WriteMemory32((u32)(USBHDC_RSTCTL), 0x01000000);         //SFTRST off

    dly_tsk(1);                                                //Wait(1ms)

    byteOrder = 0;                                          // LittleEndian

    reg_val = ReadRegister(USBHDC_MODE);                    //Read MODE
    reg_val = (reg_val | 0x2);                                //MODE.dev_en=1
    WriteRegister(USBHDC_MODE, reg_val);                    //Write MODE

    reg_val = ReadRegister(USBHDC_DEVC);                    //Read DEVC
    reg_val = (reg_val | 0x00010000);                        //DEVC.physusp=1
    WriteRegister(USBHDC_DEVC, reg_val);                    //Write DEVC

    //48MHz,20cycle=0.4us WAIT
    {
        int i;
        for(i = 0; i < 20; i++){
            __nop();
        }
    }

    WriteMemory32((u32)(USBHDC_CLKCTL), 0x0);                //CLKCTL.HCKEN=0,CLKCTL.PCKEN=0
#endif
}
#endif  /* M-7M */


/************************************************************************/
/*	<Function>	void MWUSBC_PhyHungup  	 								*/
/*	<Arguments>	void													*/
/*	<Return  >	void													*/
/*	<Description> Notification of Phy Hungup Interrupt					*/
/************************************************************************/
void MWUSBC_PhyHungup(void)
{
#ifdef USE_MS_CLASS
    T_OS_USER_RTSK flag;
#endif
    
    if( MWUSB_GetMode() != MWUSB_BYPASS_MODE ) {
        switch (UsbDeviceClass) {
#ifdef USE_MS_CLASS
        case USB_MODE_STORAGE:
            /* Mass Storage */
            OS_User_Ref_Tsk(TID_USB_STORAGE, &flag);
            if ((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT) {
                /* Start task */
                OS_User_Sta_Tsk(TID_USB_STORAGE, 0);
            }
            MSCM_NotifyStatus(MSC_STS_UNCONF);
            break;
#endif
        default:
            break;
        }
    }
}

