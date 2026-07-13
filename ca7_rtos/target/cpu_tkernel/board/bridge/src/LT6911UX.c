/**
 * @file		LT6911UX.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "dd_top.h"
#include "share.h"
#include "share_data.h"
#include "LT6911UX.h"
#include "fj_peripheral.h"
#include "os_user_custom.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define SLAVE_ADDR_7BIT 0x2B

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define BRIDGE_PRINT_ERROR(ercd)	do { if( ercd )printf( "(x_x) %s, %d, ercd=0x%x\n", __FUNCTION__, __LINE__, ercd );}while(0);
#define SWAP(x) SwapBytes(&x, sizeof(x));

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	unsigned short	Htotal;
	unsigned short	Hactive;
	unsigned short	Hfp;
	unsigned short	Hsw;
	unsigned short	Hbp;
	unsigned short	Vtotal;
	unsigned short	Vactive;
	unsigned char	Vbp;
	unsigned char	Vfp;
	unsigned char	Vsw;
} T_RESO_TIMING;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static int Bridge_I2C_Write(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size);
static int Bridge_I2C_Read(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size);
static int LT6911UX_en_ex_i2c(FJ_PERI_CH i2c_ch, BOOL en);
static int round(float x);
static void SwapBytes(void *pv, size_t n);
/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void LT6911UX_GPIO_init(void)
{
	USHORT reset;

#if defined(BOARD_TYPE_SNI)
	reset = E_DD_TOP_GPIO_PD6;
#else
	reset = E_DD_TOP_GPIO_P82;
#endif
	// Release Reset
	Dd_Top_Set_Gpio_Function(  reset, 0 );	// use as GPIO
	Dd_Top_Set_Gpio_Direction( reset, 1 );	// output
    OS_User_Dly_Tsk(50);
	Dd_Top_Set_Gpio_Status(    reset, 1 );
}

int LT6911UX_inqury_va_info(FJ_PERI_CH i2c_ch, T_SHARE_HDMI_IN_VA_INFO* va_info)
{
	T_RESO_TIMING reso_tm;
	ULONG half_pixel_clock_khz = 0, byte_clock_khz = 0;
	UCHAR data;
	UCHAR rdata[4];
	int rval = 0;

	rval = LT6911UX_en_ex_i2c(i2c_ch, TRUE);
	UPRINTF_ERR(rval);
	if( rval ) return rval;
	
	// write bank to 0x86
	data = 0x86;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
	UPRINTF_ERR(rval);
	if( rval ) return rval;

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x7C, (UCHAR*)&reso_tm.Htotal, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Htotal)

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x80, (UCHAR*)&reso_tm.Hactive, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Hactive)

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x78, (UCHAR*)&reso_tm.Hfp, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Hfp)

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x72, (UCHAR*)&reso_tm.Hsw, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Hsw)

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x76, (UCHAR*)&reso_tm.Hbp, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Hbp);

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x7A, (UCHAR*)&reso_tm.Vtotal, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Vtotal)

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x7E, (UCHAR*)&reso_tm.Vactive, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(reso_tm.Vactive)

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x74, (UCHAR*)&reso_tm.Vbp, 1);
	UPRINTF_ERR(rval); if( rval ) return rval;

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x75, (UCHAR*)&reso_tm.Vfp, 1);
	UPRINTF_ERR(rval); if( rval ) return rval;

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x71, (UCHAR*)&reso_tm.Vsw, 1);
	UPRINTF_ERR(rval); if( rval ) return rval;


	// write bank to 0x85
	data = 0x85;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
	UPRINTF_ERR(rval);
	if( rval ) return rval;
	// Select Ad_half_pix_clock
	data = 0x21;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0x40, &data, 1);
	UPRINTF_ERR(rval);
	if( rval ) return rval;
	OS_User_Dly_Tsk(10);
	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x48, (UCHAR*)&rdata[2], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x49, (UCHAR*)&rdata[1], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x4a, (UCHAR*)&rdata[0], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	half_pixel_clock_khz = rdata[2] << 16 | rdata[1] << 8 | rdata[0];
	half_pixel_clock_khz &= 0x0FFFFF;
	
	// write bank to 0x85
	data = 0x85;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
	UPRINTF_ERR(rval);
	if( rval ) return rval;
	// Select Ad_lmtx_write_clk
	data = 0x1B;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0x40, &data, 1);
	UPRINTF_ERR(rval);
	if( rval ) return rval;
	OS_User_Dly_Tsk(10);
	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x48, (UCHAR*)&rdata[2], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x49, (UCHAR*)&rdata[1], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x4a, (UCHAR*)&rdata[0], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	byte_clock_khz = rdata[2] << 16 | rdata[1] << 8 | rdata[0];
	byte_clock_khz &= 0x0FFFFF;

	reso_tm.Hactive *= 2;
	reso_tm.Htotal *= 2;
	reso_tm.Hbp *= 2;
	reso_tm.Hfp *= 2;
	reso_tm.Hsw *= 2;
	UPRINTF("Htotal:%d\n", reso_tm.Htotal);
	UPRINTF("Vtotal:%d\n", reso_tm.Vtotal);
	UPRINTF("Hactive:%d\n", reso_tm.Hactive);
	UPRINTF("Vactive:%d\n", reso_tm.Vactive);
	UPRINTF("Hfp:%d\n", reso_tm.Hfp);
	UPRINTF("Hbp:%d\n", reso_tm.Hbp);
	UPRINTF("Hsw:%d\n", reso_tm.Hsw);
	UPRINTF("Vfp:%d\n", reso_tm.Vfp);
	UPRINTF("Vbp:%d\n", reso_tm.Vbp);
	UPRINTF("Vsw:%d\n", reso_tm.Vsw);
	UPRINTF("half_pixel_clock_khz:%ld\n", half_pixel_clock_khz);
	UPRINTF("byte_clock_khz:%ld\n", byte_clock_khz);
	
	va_info->video.width	= reso_tm.Hactive;
	va_info->video.height	= reso_tm.Vactive;
	va_info->video.fps		= (float)((double)(half_pixel_clock_khz * 1000 * 2) / (double)(reso_tm.Htotal * reso_tm.Vtotal));
	va_info->video.fps		= (float)round(va_info->video.fps);
	va_info->mipi_rate		= (float)(byte_clock_khz * 4 * 2 / 1000);
	UPRINTF("fps:%f\n", va_info->video.fps);
	UPRINTF("mipi_rate:%f Mhz\n", va_info->mipi_rate);


	if( va_info->video.width == 1920 && va_info->video.height == 2160 && va_info->video.fps > 40.0) {
		// if input 4Kp60, bridge seperate left/right
		va_info->video.width = 3840;
	}
	// write bank to 0xb0
	data = 0xb0;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
	UPRINTF_ERR(rval);
	if( rval ) return rval;

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0xaa, (UCHAR *)&va_info->audio.sampling_rate, 2);
	UPRINTF_ERR(rval); if( rval ) return rval;
	SWAP(va_info->audio.sampling_rate)
	va_info->audio.sampling_rate *= 1000;
	UPRINTF("sampling rate:%d\n", va_info->audio.sampling_rate);

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x98, (UCHAR *)&rdata[0], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	UPRINTF("audio compressed?:%d\n", rdata[1] = rdata[0] & 0x10 ? 1 : 0);

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0xb2, (UCHAR *)&rdata[0], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	UPRINTF("audio more than 2ch?:%d\n", rdata[1] = rdata[0] & 0x40 ? 1 : 0);

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x92, (UCHAR *)&rdata[0], 1);
	UPRINTF_ERR(rval); if( rval ) return rval;
	if( (rdata[0] & 0xE0) == 0x60 ) {
		UPRINTF("YUV420\n");
		va_info->video.fmt = E_SHARE_YUV_FORMAT_420_SEMI;
	} else if( (rdata[0] & 0xE0) == 0x20 ) {
		UPRINTF("YUV422:%x\n", rdata[0]);
		va_info->video.fmt = E_SHARE_YUV_FORMAT_422_SEMI;
	} else if( (rdata[0] & 0xE0) == 0x40 ) {
		UPRINTF("YUV444:%x\n", rdata[0]);
		va_info->video.fmt = E_SHARE_YUV_FORMAT_422_SEMI;
	} else if( (rdata[0] & 0xE0) == 0x00 ) {
		UPRINTF("RGB:%x\n", rdata[0]);
		va_info->video.fmt = E_SHARE_YUV_FORMAT_422_SEMI;
	} else {
		UPRINTF("Unknown:%x\n", rdata[0]);
		va_info->video.fmt = E_SHARE_YUV_FORMAT_422_SEMI;
	}

	rval = LT6911UX_en_ex_i2c(i2c_ch, FALSE);
	UPRINTF_ERR(rval);
	if( rval ) return rval;

	return rval;
}

void LT6911UX_StartStop_Straming(FJ_PERI_CH i2c_ch, BOOL startstop)
{
	UCHAR data;
	int rval = 0;

	rval = LT6911UX_en_ex_i2c(i2c_ch, TRUE);
	UPRINTF_ERR(rval);

	// write bank to 0x81
	data = 0x81;
	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
	UPRINTF_ERR(rval);

	rval = Bridge_I2C_Read(i2c_ch, SLAVE_ADDR_7BIT, 0x1d, &data, 1);
	UPRINTF_ERR(rval);
	data = startstop ? data | 0x08 : data & 0xF7;

	rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0x1d, &data, 1);
	UPRINTF_ERR(rval);

	rval = LT6911UX_en_ex_i2c(i2c_ch, FALSE);
	UPRINTF_ERR(rval);
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static int round(float x)
{
    return (int)(x + 0.5);
}

static void SwapBytes(void *pv, size_t n)
{
	char *p = pv;
    size_t lo, hi;
    for(lo=0, hi=n-1; hi>lo; lo++, hi--)
    {
        char tmp=p[lo];
        p[lo] = p[hi];
        p[hi] = tmp;
    }
}

static int LT6911UX_en_ex_i2c(FJ_PERI_CH i2c_ch, BOOL en)
{
	UCHAR data;
	int rval = 0;

	if( en ) {
		// Enable external I2C control
		data = 0x80;
		rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
		UPRINTF_ERR(rval); if( rval ) return rval;
		data = 0x01;
		rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xEE, &data, 1);
		UPRINTF_ERR(rval); if( rval ) return rval;
	} else {
		// Disable external I2C control
		data = 0x80;
		rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xFF, &data, 1);
		UPRINTF_ERR(rval); if( rval ) return rval;
		data = 0x00;
		rval = Bridge_I2C_Write(i2c_ch, SLAVE_ADDR_7BIT, 0xEE, &data, 1);
		UPRINTF_ERR(rval); if( rval ) return rval;
	}

	return rval;
}

static int Bridge_I2C_Write(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size)
{
	FJ_I2C_CTRL				ctrl;
	FJ_I2C_TRANSFER_DATA	trans;
	UCHAR					address_buf;
	int						ret = 0;
	
	BRIDGE_PRINT_ERROR( FJ_I2C_Open(ch) );
	
	ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= slave7bit;
	BRIDGE_PRINT_ERROR( FJ_I2C_Ctrl(ch, ctrl, NULL) );
	
	address_buf = address;
	trans.slave_register_address.data_length	= 1;
	trans.slave_register_address.data			= &address_buf;
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 0;
	trans.timeout_threshold						= 3; //ms
	trans.retry_num								= 3;
	if( FJ_I2C_Transfer(ch, &trans, 1) != FJ_ERR_OK ){
		ret = 1;
	}
	
	BRIDGE_PRINT_ERROR( FJ_I2C_Close(ch) );
	
	return ret;
}

static int Bridge_I2C_Read(FJ_PERI_CH ch, UCHAR slave7bit, UCHAR address, UCHAR* data, UCHAR size)
{
	FJ_I2C_CTRL				ctrl;
	FJ_I2C_TRANSFER_DATA	trans;
	UCHAR					address_buf;
	int						ret = 0;
	
	BRIDGE_PRINT_ERROR( FJ_I2C_Open(ch) );
	
	ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= slave7bit;
	BRIDGE_PRINT_ERROR( FJ_I2C_Ctrl(ch, ctrl, NULL) );
	
	address_buf = address;
	trans.slave_register_address.data_length	= 1;
	trans.slave_register_address.data			= &address_buf;
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 1;
	trans.timeout_threshold						= 3; //ms
	trans.retry_num								= 3;
	if( FJ_I2C_Transfer(ch, &trans, 1) != FJ_ERR_OK ){
		ret = 1;
	}
	
	BRIDGE_PRINT_ERROR( FJ_I2C_Close(ch) );
	
	return ret;
}

