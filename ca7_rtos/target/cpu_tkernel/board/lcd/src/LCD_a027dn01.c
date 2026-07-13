/**
 * @file		LCD_a027dn01.c
 * @brief 		LCD driver for AUO27DN01
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "dd_top.h"
#include "LCDCtrl.h"
#include "debug.h"
#if( LCD_TYPE_KIND == D_LCDTYPE_A027CN01)

//#include "lcd_a027dn01.h"

/*--------------------------------------------------------------------------*/
/* Structure				 												*/
/*--------------------------------------------------------------------------*/
union au_reg_r0 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR VCOM_AC	:3;
		UCHAR VCAC		:1;
		UCHAR 			:2;
		UCHAR CCIR601	:1;
		UCHAR YCbCr		:1;
	}bit;
};

union au_reg_r1 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR VCOM_DC	:6;
		UCHAR			:1;
		UCHAR VCDCE		:1;
	}bit;
};

union au_reg_r3 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR BRIGHT	:8;
	}bit;
};

union au_reg_r4 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR HDIR		:1;
		UCHAR VDIR		:1;
		UCHAR NTSCPAL	:2;
		UCHAR SEL		:2;
		UCHAR YUV		:1;
		UCHAR NARROW	:1;
	}bit;
};

union au_reg_r5 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR STB		:1;
		UCHAR SHDB1		:1;
		UCHAR SHDB2		:1;
		UCHAR PFM_DUTY	:3;
		UCHAR GRB		:1;
		UCHAR DRV_FREQ	:1;
	}bit;
};

union au_reg_r6 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR VBLK		:5;
		UCHAR LED_CURR	:2;
		UCHAR HBLK_EN	:1;
	}bit;
};

union au_reg_r7 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR HBLK		:8;
	}bit;
};

union au_reg_r8 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR 			:6;
		UCHAR BL_DRV	:2;
	}bit;
};

union au_reg_r12 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR DCLKPOL	:1;
		UCHAR HDPOL		:1;
		UCHAR VDPOL		:1;
		UCHAR 			:1;
		UCHAR CbCr		:1;
		UCHAR 			:1;
		UCHAR PAIR		:2;
	}bit;
};

union au_reg_r13 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR CONTRAST_RGB	:8;
	}bit;
};

union au_reg_r14 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR SUB_CONTRAST_R	:7;
		UCHAR 					:1;
	}bit;
};

union au_reg_r15 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR SUB_BRIGHT_R	:7;
		UCHAR 				:1;
	}bit;
};

union au_reg_r16 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR SUB_CONTRAST_B	:7;
		UCHAR 					:1;
	}bit;
};

union au_reg_r17 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR SUB_BRIGHT_B	:7;
		UCHAR 				:1;
	}bit;
};

union au_reg_r21 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR LED_ON_RATIO	:4;
		UCHAR LED_ON_CYC	:4;
	}bit;
};

union au_reg_r22 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR			:2;
		UCHAR GAMMA		:1;
		UCHAR 			:5;
	}bit;
};

union au_reg_r23 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR 			:2;
		UCHAR GMA_V4	:2;
		UCHAR			:2;
		UCHAR GMA_V8	:2;
	}bit;
};

union au_reg_r24 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR 			:2;
		UCHAR GMA_V16	:2;
		UCHAR			:2;
		UCHAR GMA_V25	:2;
	}bit;
};

union au_reg_r25 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR 			:2;
		UCHAR GMA_V36	:2;
		UCHAR			:2;
		UCHAR GMA_V48	:2;
	}bit;
};

union au_reg_r26 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR 			:2;
		UCHAR GMA_V55	:2;
		UCHAR			:2;
		UCHAR GMA_V60	:2;
	}bit;
};

union au_reg_r86 {
	UCHAR byte;
	struct 
	{
		/*Data Bits*/
		UCHAR VGH_SEL	:2;
		UCHAR 			:6;
	}bit;
};

typedef struct {
	union au_reg_r0		R0;
	union au_reg_r1		R1;
	union au_reg_r3		R3;
	union au_reg_r4		R4;
	union au_reg_r5		R5;
	union au_reg_r6		R6;
	union au_reg_r7		R7;
	union au_reg_r8		R8;
	union au_reg_r12	R12;
	union au_reg_r13	R13;
	union au_reg_r14	R14;
	union au_reg_r15	R15;
	union au_reg_r16	R16;
	union au_reg_r17	R17;
	union au_reg_r21	R21;
	union au_reg_r22	R22;
	union au_reg_r23	R23;
	union au_reg_r24	R24;
	union au_reg_r25	R25;
	union au_reg_r26	R26;
	union au_reg_r86	R86;
} au_reg ;
/*--------------------------------------------------------------------------*/
/* Definition				 												*/
/*--------------------------------------------------------------------------*/
/* Serial I/F definition */
#define SERIAL_DATA_READ_WRITE			0x40
#define TIME_1FIELD					(17)	// 1VD period : 1Field period of 15fps(16.68msec) (Even & Odd frame of 30fps)
#define LCD_ADDR_DATA_WIDTH		16	/**< Address + Data = 16bits */
#define LCD_ADDR_WIDTH			 8	/**< Address = 5Bits */
#define LCD_DATA_WIDTH			8	/**< Data = 8bits */

/* AU 27DN01  Register address*/
#define REG_ADDR0		(0x00)
#define REG_ADDR1		(0x01)
#define REG_ADDR3		(0x03)
#define REG_ADDR4		(0x04)
#define REG_ADDR5		(0x05)
#define REG_ADDR6		(0x06)
#define REG_ADDR7		(0x07)
#define REG_ADDR8		(0x08)
#define REG_ADDR12		(0x0C)
#define REG_ADDR13		(0x0D)
#define REG_ADDR14		(0x0E)
#define REG_ADDR15		(0x0F)
#define REG_ADDR16		(0x10)
#define REG_ADDR17		(0x11)
#define REG_ADDR21		(0x15)
#define REG_ADDR22		(0x16)
#define REG_ADDR23		(0x17)
#define REG_ADDR24		(0x18)
#define REG_ADDR25		(0x19)
#define REG_ADDR26		(0x1A)
#define REG_ADDR86		(0x96)

/*CSIO channel number*/
#define CSIO_CH_1		(1)		/**<CSIO channel 1*/
#define CSIO_CH_2		(2)		/**<CSIO channel 2*/
#define CSIO_CH_3		(3)		/**<CSIO channel 3*/
#define CSIO_CH_4		(4)		/**<CSIO channel 4*/
#define CSIO_CH_5		(5)		/**<CSIO channel 5*/
#define CSIO_CH_6		(6)		/**<CSIO channel 6*/

/*--------------------------------------------------------------------------*/
/* Static Variables			 												*/
/*--------------------------------------------------------------------------*/
#if (LCD_MODE_KIND == D_LCDMODE_RGB_DELTA)
static au_reg AU_REG ={
	{0x03},//AU_REG.R0
	{0x8A},//AU_REG.R1
	{0x40},//AU_REG.R3
	{0x0B},//AU_REG.R4
	{0x5E},//AU_REG.R5
	{0x15},//AU_REG.R6
	{0x46},//AU_REG.R7
	{0x00},//AU_REG.R8
	{0x06},//AU_REG.R12
	{0x40},//AU_REG.R13
	{0x40},//AU_REG.R14
	{0x40},//AU_REG.R15
	{0x40},//AU_REG.R16
	{0x40},//AU_REG.R17
	{0x7F},//AU_REG.R21
	{0x04},//AU_REG.R22
	{0x11},//AU_REG.R23
	{0x22},//AU_REG.R24
	{0x22},//AU_REG.R25
	{0x22},//AU_REG.R26
	{0x07},//AU_REG.R86
};
#endif
//static int glcd_mode;
/*--------------------------------------------------------------------------*/
/* Static Function			 												*/
/*--------------------------------------------------------------------------*/
/**
 * Write register of a027dn01 LCD.
 *
 * @param addr - Address of register.
 * @param data - Data to write in register.
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
#if D_LCDCTRL_WITH_GPIO//control lcd with GPIO port
static INT32 wr_reg( UCHAR addr, UCHAR data )
{
	USHORT addr_data = 0;
	USHORT mask = 0x8000;
	INT32 bitcnt;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	/* Clear D11 for write operation */
    addr = addr&(~SERIAL_DATA_READ_WRITE);
	LCD_SDA_EPCR_SET(0);
	LCD_SCK_EPCR_SET(0);
	/* Combine address and data */
	addr_data = (USHORT)((addr << 8) | data);

	Dd_ARM_Critical_Section_Start( critseq );
	/* Enable serial port date transfer */
	LCD_SEN_PDR_SET(0);
	
	/* Set LCD_SDA to output direction */
	LCD_SDA_DDR_SET(1);
	for (bitcnt = 0; bitcnt < LCD_ADDR_DATA_WIDTH; bitcnt++) 
	{
		LCD_SCK_PDR_SET(0);
		(addr_data & mask) ? (LCD_SDA_PDR_SET(1)) : (LCD_SDA_PDR_SET(0));
		/* Data Setup Time */
#ifdef __GNUC__
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
#else
		__asm
		 {
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
		 }
#endif

		LCD_SCK_PDR_SET(1);

#ifdef __GNUC__
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
#else
		__asm
		{
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
		}
#endif
		mask = mask >> 1;
	}
	/* Disable serial port data transfer*/
	LCD_SEN_PDR_SET(1);
	/* Pulse Width */
	{
		INT32 i;
		for(i=0;i<15;i++) {
#ifdef __GNUC__
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
#else
			__asm
			{
				nop
				nop
				nop
				nop
				nop
			}
#endif
		}
		LCD_SDA_PDR_SET(0);
		for(i=0;i<15;i++) {
#ifdef __GNUC__
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
#else
			__asm
			{
				nop
				nop
				nop
				nop
				nop
			}
#endif
		}
	}
	Dd_ARM_Critical_Section_End( critseq );
	return (LCD_ERR_OK);
}
#else//control lcd with CSIO 
static INT32 wr_reg( UCHAR addr, UCHAR data )
{
	INT32 ret = LCD_ERR_NG;
	UCHAR send_data[2];
	FJ_SIO_CTRL csio_ctrl;
	FJ_SIO_CTRL_TRANSFER trans_ctrl;

	addr = addr&(~SERIAL_DATA_READ_WRITE);
	send_data[0] = addr;
	send_data[1] = data;
	
	csio_ctrl.type = FJ_SIO_TYPE_MASTER;
	csio_ctrl.mode = FJ_SIO_MODE_NORMAL_SC_H;
	csio_ctrl.bit_direction = FJ_SIO_BIT_DIR_MSB_FIRST;//MSB first
	csio_ctrl.data_length = FJ_SIO_LENGTH_8;
	csio_ctrl.baudrate = 115200;
	csio_ctrl.pcallback = NULL;	

	/* Set LCD corresponding ports to external functionality */
	LCD_SDA_EPCR_SET(1);
	LCD_SCK_EPCR_SET(1);
	
	/* Set LCD corresponding ports to output direction */
	LCD_SCK_DDR_SET(1);
	LCD_SDA_DDR_SET(1);
	LCD_SEN_DDR_SET(1);

	/*Open and ctrl CSIO*/
	ret = FJ_SIO_Open(FJ_PERI_CH_3);
	if(ret == FJ_ERR_OK){	
		/*CSIO settting*/
		ret = FJ_SIO_Ctrl(FJ_PERI_CH_3,&csio_ctrl);
		if(ret == FJ_ERR_OK){		
			/* Enable serial port date transfer */
			LCD_SEN_PDR_SET(0);
			trans_ctrl.direction = FJ_SIO_DATA_DIR_SEND;
//			trans_ctrl.port = FJ_GPIO_PORT_MAX;
//			trans_ctrl.status = FJ_GPIO_STATUS_HIGH;
			trans_ctrl.data_ptr = send_data;
			trans_ctrl.data_count = 2;
			ret = FJ_SIO_Transfer( FJ_PERI_CH_3, &trans_ctrl, FJ_SIO_SEND_DMA_MODE_DEMAND );
			if(ret != FJ_ERR_OK){
				BF_Debug_Print_Error(("LCD control: FJ_SIO_Send:NG!!\n"));
			}
			LCD_SEN_PDR_SET(1);			
		}
		else{
			BF_Debug_Print_Error(("LCD control: FJ_SIO_Ctrl:Error!!\n"));
		}
		
		/*Close and ctrl CSIO*/
		if(FJ_ERR_OK != FJ_SIO_Close(FJ_PERI_CH_3)){
			BF_Debug_Print_Error(("LCD control: FJ_SIO_Close:Error!!\n"));
		}
	}
	return ret;
}
#endif//LCD_CTRL_WITH_GPIO

/**
 * Read register of a027dn01 LCD.
 *
 * @param addr - Address of register.
 * @param *data - Data pointer to read from register.
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 rd_reg( UCHAR addr, UCHAR *data )
{
	UCHAR regaddr = 0;
	UCHAR mask = 0x80;
	INT32 bitcnt;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	UCHAR status;
	/* Set D11 to 1 for read operation */
    regaddr = addr|SERIAL_DATA_READ_WRITE;
	LCD_SDA_EPCR_SET(0);
	LCD_SCK_EPCR_SET(0);

	//regaddr = addr;
	*data = 0;

	Dd_ARM_Critical_Section_Start( critseq );
	/* Enable serial port date transfer */
	LCD_SEN_PDR_SET(0);
	/* Set LCD_SDA to output direction */
	LCD_SDA_DDR_SET(1);
	for (bitcnt = 0; bitcnt < LCD_ADDR_WIDTH; bitcnt++) 
	{
		LCD_SCK_PDR_SET(0);
		(regaddr & mask) ? (LCD_SDA_PDR_SET(1)) : (LCD_SDA_PDR_SET(0));
		/* Data Setup Time */
#ifdef __GNUC__
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
#else
		__asm
		{
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
		}
#endif

		LCD_SCK_PDR_SET(1);
		/* Data Hold Time */
#ifdef __GNUC__
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
#else
		__asm
		{
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
		}
#endif
		mask = mask >> 1;
	}

	mask = 0x80; //11 bits mask
	/* Set LCD_SDA to input direction */
	LCD_SDA_PDR_SET(0);
	LCD_SDA_DDR_SET(0);
	for (bitcnt = 0; bitcnt < LCD_DATA_WIDTH; bitcnt++) {
		LCD_SCK_PDR_SET(0);
		/* Data Setup Time */
#ifdef __GNUC__
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
#else
		__asm
		{
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
		}
#endif
		LCD_SCK_PDR_SET(1);
		/* Data Hold Time */
#ifdef __GNUC__
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
		__asm__ volatile ("nop");
#else
		__asm
		{
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
			nop
		}
#endif
		LCD_SDA_PDR_GET(status);
		*data = status ? (*data | mask) : (*data);
		mask = mask >> 1;
	}
	/* Disable serial port data transfer*/
	LCD_SEN_PDR_SET(1);
	/* Pulse Width */
	{
		INT32 i;
		for(i=0;i<30;i++)
		{
#ifdef __GNUC__
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
			__asm__ volatile ("nop");
#else
			__asm
			{
				nop
				nop
				nop
				nop
				nop
			}
#endif
		}
	}
	Dd_ARM_Critical_Section_End( critseq );

	return (LCD_ERR_OK);
}

/**
 * Configure the LCD device.
 *
 * @param mode - Input format of LCD.
 * @param ar - Display mode of LCD.
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 config(INT32 mode, LCD_DISPLAY_AR ar )
{
	INT32 rval = 0;

	switch (mode) {
		case D_LCDMODE_RGB_DELTA:
		    AU_REG.R4.bit.SEL = 0;
	    	AU_REG.R0.bit.CCIR601 = 0;
		    AU_REG.R4.bit.YUV	=0;
			break;
		//case D_LCDMODE_RGB_DUMMY_320_NTSC:
		//    AU_REG.R4.bit.SEL = 1;
		//    AU_REG.R0.bit.CCIR601 = 0;
	 //   	AU_REG.R4.bit.YUV	=0;
		//	break;
		//case D_LCDMODE_RGB_DUMMY_320_PAL:
		//    AU_REG.R4.bit.SEL = 1;
		//    AU_REG.R0.bit.CCIR601 = 0;
	 //   	AU_REG.R4.bit.YUV	=0;
		//	break;
		//case D_LCDMODE_RGB_DUMMY_360_NTSC:
		//    AU_REG.R4.bit.SEL = 2;
		//    AU_REG.R0.bit.CCIR601 = 0;
	 //   	AU_REG.R4.bit.YUV	=0;
		//	break;
		//case D_LCDMODE_RGB_DUMMY_360_PAL:
	 //   	AU_REG.R4.bit.SEL = 2;
		//    AU_REG.R0.bit.CCIR601 = 0;
		//    AU_REG.R4.bit.YUV	=0;
		//	break;
		//case D_LCDMODE_CCIR656:
		//    AU_REG.R4.bit.SEL = 2;
	 //   	AU_REG.R0.bit.CCIR601 = 0;
		//    AU_REG.R4.bit.YUV	=1;
		//break;
		//case D_LCDMODE_YUV640_NTSC:
		//    AU_REG.R4.bit.SEL = 0;
	 //   	AU_REG.R0.bit.CCIR601 = 1;
		//    AU_REG.R4.bit.YUV	=1;
		//	break;
		//case D_LCDMODE_YUV640_PAL:
	 //   	AU_REG.R4.bit.SEL = 0;
		//    AU_REG.R0.bit.CCIR601 = 1;
		//    AU_REG.R4.bit.YUV	=1;
		//	break;
		//case D_LCDMODE_YUV720_NTSC:
		//    AU_REG.R4.bit.SEL = 2;
		//    AU_REG.R0.bit.CCIR601 = 1;
	 //   	AU_REG.R4.bit.YUV	=1;
		//	break;
		default:
			BF_Debug_Print_Error(("[LCD] config() wrong argument!\n"));
			rval = -1;
			break;
	}
	wr_reg(REG_ADDR0, AU_REG.R0.byte);
	wr_reg(REG_ADDR4, AU_REG.R4.byte);
	return (rval);
	//return (aspect_ratio(ar));
}

// For LED(Baklight) control
#define EXT_BL_CTRL

/**
 * Enable/Disalbe LCD backlight.
 *
 * @param on_off - TRUE for enablbe, false for disable.
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 backlight_ctrl( UCHAR on_off)//, UCHAR led_level )
{
#ifdef EXT_BL_CTRL
	on_off ? (LCD_BLC_PDR = 1) : (LCD_BLC_PDR = 0);

#else	// For SHDB(Backlight power converter) Control
	if(on_off){
		AU_REG.R5.bit.SHDB1 = 1; //turn on the led backlight
		wr_reg(REG_ADDR5, AU_REG.R5.byte);  // send cmd to turn on the backlight
	}
	else {
		AU_REG.R5.bit.SHDB1 = 0;
		wr_reg(REG_ADDR5, AU_REG.R5.byte);
	}
#endif
	return (LCD_ERR_OK);
}

/**
 * Set brightness of LCD.
 *
 * @param brightness - 00h for dark(-256), 40h for center(0), 7Fh for bright(+256)
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 brightness( UCHAR brightness_level )
{
	AU_REG.R3.bit.BRIGHT = brightness_level;
	return wr_reg(REG_ADDR3, AU_REG.R3.byte);
}

/**
 * Set the contrast of LCD.
 *
 * @param contrast - Contrast gain value, 0h:0gain, 8h:1gain, Fh:1.875gain
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 contrast( UCHAR contrast_level )
{
	AU_REG.R13.bit.CONTRAST_RGB = contrast_level;
	return wr_reg(REG_ADDR13, AU_REG.R13.byte);
}

/**
 * Enable/Disable power mode of LCD.
 *
 * @param on_off - TRUE for normal mode, false for standby mode.
 * @return - 0 for success, otherwise failure.
 */
static INT32 power_saving( UCHAR on_off )
{
	UCHAR stb;
	stb = on_off ? (1) : (0);

	AU_REG.R5.bit.STB = stb;
	return wr_reg(REG_ADDR5, AU_REG.R5.byte);
}

/**
 * Set the display mode of LCD(normal, narrow or wide).
 *
 * @param ar - Display mode of LCD.
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 aspect_ratio( LCD_DISPLAY_AR ar )
{
	AU_REG.R4.bit.NARROW = ar;
	return wr_reg(REG_ADDR4, AU_REG.R4.byte);
}

/**
 * Set the Horizontal/Vertical scan direction setting.
 *
 * @param h_dir - Horizontal Scan Direction
 *                =0: Right to Left scan, =1: Left to Right scan(default)
 * @param v_dir - Vertical Scan Direction
 *                =0: Down to Up scan, =1: Up to Down scan(default)
 * @return (LCD_ERR_OK) for success, otherwise failure.
 */
static INT32 display_direction( UCHAR h_dir, UCHAR v_dir )
{
	AU_REG.R4.bit.HDIR = (h_dir & 0x01);	// Horizontal Scan Direction
	AU_REG.R4.bit.VDIR = (v_dir & 0x01);	// Vertical Scan Direction
	return wr_reg(REG_ADDR4, AU_REG.R4.byte);
}

/*
*	A027DN01 LCD Power Sequence Command sending
*/
static void lcd_pwr_init()
{
	
	//if( (mode==D_LCDMODE_YUV640_NTSC) || (mode==D_LCDMODE_YUV640_PAL) ){
	//	wr_reg(REG_ADDR0,AU_REG.R0.byte);
	//}
	wr_reg(REG_ADDR4,AU_REG.R4.byte);
	wr_reg(REG_ADDR1,0x88);
	wr_reg(REG_ADDR6,AU_REG.R6.byte);
	wr_reg(REG_ADDR86,AU_REG.R86.byte);
//	rd_reg(REG_ADDR86,&temp_read);

//	AU_REG.R86.byte = temp_r86;
}

/*--------------------------------------------------------------------------*/
/* Global Function			 												*/
/*--------------------------------------------------------------------------*/
void lcd_set_hook( )
{
	struct lcd_drv_s lcd;
	/* Hook up the function pointers */
	memset(&lcd, 0x0, sizeof(struct lcd_drv_s));
	strcpy(lcd.name, "a027cn01");
	lcd.backlight_ctrl 	    = backlight_ctrl;
	lcd.config 		        = config;//a024cn02_config;
	lcd.brightness 		    = brightness;
	lcd.contrast 		    = contrast;
	lcd.rb_sub_contrast	    = NULL;
	lcd.rb_sub_brightness 	= NULL;
	lcd.power_saving 	    = power_saving;
	lcd.aspect_ratio 	    = aspect_ratio;
	lcd.saturation 	        = NULL;
	lcd.hue          	    = NULL;
	lcd.display_direction	= display_direction;
	lcd.wr_reg		        = wr_reg;
	lcd.rd_reg		        = rd_reg;
	FJ_LCD_Drv_Attach(&lcd);

}
/**
 * Initial Senquence of the LCD a027dn01 device.
 */
INT32 InitSequence( )
{
//	UCHAR temp_read;

//	OS_dly_tsk(TIME_1FIELD);        // wait for 50ms +1 fields 
	wr_reg(REG_ADDR5,	0x1E);  // Reset all to default
	wr_reg(REG_ADDR5,	0x5E);  // Set to STB
//	rd_reg(REG_ADDR5, &temp_read);
	//wr_reg(REG_ADDR5,	0x5F);  // Start the LCD out of standby mode
	//OS_dly_tsk(TIME_1FIELD*10);        // wait for 8 fields 

	lcd_pwr_init();

	wr_reg(REG_ADDR1, AU_REG.R1.byte);
	//rd_reg(REG_ADDR1, &temp_read);
	//if(temp_read != AU_REG.R1.byte){
	//	printf("REG 1 write error!!\n");
	//	return (LCD_ERR_NG);
	//}
		
	wr_reg(REG_ADDR7, AU_REG.R7.byte);
	//rd_reg(REG_ADDR7, &temp_read);
	//if(temp_read != AU_REG.R7.byte){
	//	printf("REG 7 write error!!\n");
	//	return(LCD_ERR_NG);
	//}
	
	wr_reg(REG_ADDR12, AU_REG.R12.byte);
//	rd_reg(REG_ADDR12, &temp_read);
	//if(temp_read != AU_REG.R12.byte){
	//	printf("REG 12 write error!!\n");
	//	return(LCD_ERR_NG);
	//}
	wr_reg(REG_ADDR5,0x5F);
	return LCD_ERR_OK;
}

/**
 * Initialize the LCD a027dn01 device.
 */
INT32 lcd_init( INT32 bl_enable )
{
	INT32 lcd_error;
	/* Set LCD corresponding ports to GPIO functionality */
#if D_LCDCTRL_WITH_GPIO//control lcd with GPIO port
	LCD_SCK_EPCR_SET(0);
	LCD_SDA_EPCR_SET(0);
#else//control lcd with CSIO 
	LCD_SCK_EPCR_SET(1);
	LCD_SDA_EPCR_SET(1);
#endif//LCD_CTRL_WITH_GPIO
	LCD_BLC_EPCR = 0;

	/* Set LCD corresponding ports to output direction */
	LCD_SCK_DDR_SET(1);
	LCD_SDA_DDR_SET(1);
	LCD_SEN_DDR_SET(1);
	LCD_BLC_DDR = 1;

	/* LCD signal initialization */
	LCD_SEN_PDR_SET(1);
	LCD_SDA_PDR_SET(0);
	LCD_SCK_PDR_SET(0);
	if (bl_enable) {
		LCD_BLC_PDR = 1;
	}

	/* AUO lcd initialize sequence */ 
    lcd_error = InitSequence();

	if(lcd_error){
    	return (LCD_ERR_NG);
	}
	else{
		lcd_set_hook();
	}

	return (LCD_ERR_OK);
}


/*
*	Links static functions to variable in LCDCTRl
*/
//int lcd_fn_config(T_LCD_STRUCT * lcd)
//{
//		/* Hook up the function pointers */
//	memset(lcd, 0x0, sizeof(T_LCD_STRUCT));
//	strcpy(lcd->name, "A027DN01");
//	lcd->lcd_init				= lcd_init;
//	lcd->backlight_ctrl 	    = backlight_ctrl;
//	lcd->config 		        = config;
//	lcd->brightness 		    = brightness;
//	lcd->contrast 		    	= contrast;
//	lcd->power_saving 	    	= power_saving;
//	lcd->aspect_ratio 	    	= aspect_ratio;
//	lcd->wr_reg		        = wr_reg;
//	lcd->rd_reg		        = rd_reg;
//	return 0;
//}
#endif
