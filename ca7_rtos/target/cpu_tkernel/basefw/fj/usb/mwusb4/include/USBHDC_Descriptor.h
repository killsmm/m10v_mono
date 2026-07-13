/**
 * @file		USBHDC_Descriptor.h
 * @brief		This is a header file for USB descriptor
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DESCRIPTOR
#define H_USBHDC_DESCRIPTOR

/* macro definition declaration */
/* descriptor types */
//#define DEVICE						1
//#define CONFIGURATION				2
//#define STRING						3
//#define INTERFACE					4
//#define ENDPOINT					5
//#define DEVICE_QUALIFIER			6
//#define OTHER_SPEED_CONFIGURATION	7
//#define HID							33
//#define REPORT						34

/* Device descriptor structure */
typedef struct device_descriptor {
    u8		bLength;				/* size of this descriptor in bytes */
    u8		bDescpritorType;		/* device descriptor type (0x1) */
    u16		bcdUSB;					/* USB specification release number */
    u8		bDeviceClass;			/* class code */
    u8		bDeviceSubClass;		/* subclass code */
    u8		bDeviceProtocol;		/* protocol code */
    u8		bMaxPacketSize0;		/* maximum packet size for endponit0 */
    u16		idVendor;				/* vendor id */
    u16		idProduct;				/* product id */
    u16		bcdDevice;				/* device relase number */
    u8		iManufacturer;			/* index of string decsriptor describing manufacturer */
    u8		iProduct;				/* index of string decsriptor describing product */
    u8		iSerialNumber;			/* index of string decsriptor describing the serial number */
    u8		bNumConfigurations;		/* number of possible configurations */
} __attribute__((__packed__)) DEVICE_DESCRIPTOR;


/* Configuration descriptor structure */
typedef struct configuration_descriptor {
    u8		bLength;				/* size of descriptor in bytes */
    u8		bDescpritorType;		/* configuration descriptor type (0x2 or 0x7(other speed)) */
    u16		wTotalLength;			/* total length of data returned */
    u8		bNumInterfaces;			/* number of interfaces supported */
    u8		bConfigurationValue;	/* value to use to select configration */
    u8		iConfiguration;			/* index of string descriptor describing configuration */
    u8		bmAttributes;			/* power & remote wakeup setting */
    u8		MaxPower;				/* half of bus current */
} __attribute__((__packed__)) CONFIGURATION_DESCRIPTOR;

/* String descriptor structure */
typedef struct string_descriptor {
    u8		bLength;				/* size of descriptor in bytes */
    u8		bDescpritorType;		/* string descriptor type(0x3) */
    u8		bString[256];			/* unicode string */
} __attribute__((__packed__)) STRING_DESCRIPTOR ;

/* Interface descriptor structure */
typedef struct interface_descriptor {
    u8		bLength;				/* size of descriptor in bytes */
    u8		bDescpritorType;		/* interface descriptor type (0x4) */
    u8		bInterfaceNumber;		/* number of this interface */
    u8		bAlternateSetting;		/* value used to select alternate setting */
    u8		bNumEndpoints;			/* number of endpoints used */
    u8		bInterfaceClass;		/* class code */
    u8		bInterfaceSubClass;		/* sub class code */
    u8		bInterfaceProtocol;		/* protocol code */
    u8		iInterface;				/* index of string descriptor */
} __attribute__((__packed__)) INTERFACE_DESCRIPTOR;

/* Endpoint descriptor struct */
typedef struct endpoint_descriptor {
    u8		bLength;				/* size of descriptor in bytes */
    u8		bDescpritorType;		/* endpoint descriptor type(0x5) */
    u8		bEndPointAddress;		/* address of endpoint */
    u8		bmAttributes;			/* endpoint's attributes */
    u16		wMaxPacketSize;			/* max packet size */
    u8		bInterval;				/* interval for polling endpoint */
} __attribute__((__packed__)) ENDPOINT_DESCRIPTOR;

/* Device qualifier descriptor structure */
typedef struct device_qualifier_descriptor{
    u8		bLength;				/* size of descriptor in bytes */
    u8		bDescpritorType;		/* device qualifier type(0x6) */
    u16		bcdUSB;					/* USB release number */
    u8		bDeviceClass;			/* class code */
    u8		bDeviceSubClass;		/* sub class code */
    u8		bDeviceProtocol;		/* protocol code */
    u8		bMaxPacketSize0;		/* max packet size for endpoint0 */
    u8		bNumConfigurations;		/* number of configurations */
    u8		Reserved;				/* reserved for future use, must be zero */
} __attribute__((__packed__)) DEVICE_QUALIFIER_DESCRIPTOR;

#endif
