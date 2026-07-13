
#include "share.h"
#include "share_data.h"
#include "camera_sensor_sht30.h"
#include "fj_peripheral.h"
#include "os_user_custom.h"

#define I2C_CHANNEL_NUM FJ_PERI_CH_0
#define ADDR    0x88 

static int i2c_write(UCHAR* data, UCHAR size)
{

    FJ_I2C_CTRL				ctrl = {0};
    FJ_I2C_TRANSFER_DATA	trans = {0};
    int						ret = 0;
    if ( FJ_I2C_Open(I2C_CHANNEL_NUM) != FJ_ERR_OK)
    {
        return -1;
    }

    ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= ADDR >> 1;

    if (FJ_I2C_Ctrl(I2C_CHANNEL_NUM, ctrl, NULL) != FJ_ERR_OK)
    {

        FJ_I2C_Close(I2C_CHANNEL_NUM);
        return -1;
    }
	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 0;
	trans.timeout_threshold						= 500; //ms
	trans.retry_num								= 3;
    if( FJ_I2C_Transfer(I2C_CHANNEL_NUM, &trans, 1) != FJ_ERR_OK ){
		ret = -1;
	}

    FJ_I2C_Close(I2C_CHANNEL_NUM);
    return ret;
}
static int i2c_read( UCHAR* cmd, UCHAR cmd_size,  UCHAR* data, UCHAR size)
{

    FJ_I2C_CTRL				ctrl = {0};
    FJ_I2C_TRANSFER_DATA	trans = {0};
    int						ret = 0;
    if ( FJ_I2C_Open(I2C_CHANNEL_NUM) != FJ_ERR_OK)
    {
        return -1;
    }

    ctrl.type				= FJ_I2C_TYPE_MASTER;
	ctrl.speed				= FJ_I2C_SPEED_FAST;
	ctrl.address_format		= FJ_I2C_ADDRESS_FORMAT_7_BIT;
	ctrl.slave_address		= ADDR >> 1;
	printf("1----------------------\n");

    if (FJ_I2C_Ctrl(I2C_CHANNEL_NUM, ctrl, NULL) != FJ_ERR_OK)
    {
		FJ_I2C_Close(I2C_CHANNEL_NUM);
        return -1;
    }

	trans.slave_register_address.data_length = cmd_size;
	trans.slave_register_address.data = cmd;
	//memcpy( trans.slave_register_address.data, cmd, cmd_size );

	trans.trn_data.data_length					= size;
	trans.trn_data.data							= data;
	trans.dirction								= 1;
	trans.retry_num								= 3;
	trans.timeout_threshold						= 500; //ms
	printf("2----------------------\n");
    if( FJ_I2C_Transfer(I2C_CHANNEL_NUM, &trans, 1) != FJ_ERR_OK ){
		ret = -1;
	}
	printf("3----------------------\n");
    FJ_I2C_Close(I2C_CHANNEL_NUM);
    return ret;
}



int SHT30_Init()
{
    //0x2130  表示周期模式 周期1ms
   	// IIC_Start();
   	// IIC_Send_Byte(0x88);
   	// IIC_Wait_Ack();
   	// IIC_Send_Byte(0x21);
   	// IIC_Wait_Ack();
   	// IIC_Send_Byte(0x30);
   	// IIC_Wait_Ack();
    //  IIC_Stop();
	int ret = 0;
	UCHAR c1[] = {0x21, 0x30};

	FJ_Gpio_Set_Direction(FJ_GPIO_PC0L, FJ_GPIO_DIR_IN);
	FJ_Gpio_Set_Function(FJ_GPIO_PC0L,FJ_GPIO_FUNC_HWMODE);

	FJ_Gpio_Set_Direction(FJ_GPIO_PC1L, FJ_GPIO_DIR_IN);
	FJ_Gpio_Set_Function(FJ_GPIO_PC1L,FJ_GPIO_FUNC_HWMODE);

    ret =i2c_write(c1, 2);
	if (  ret != 0 )
	{

		return -1;
	}
    return 0;
}
int SHT30_Read_Result(UINT32* temperature, UINT32* humidity)
{
	int ret = 0;
	UINT32 tem, hum;
	UCHAR buff[6] = {0};
	UCHAR cmd[] = {0x2C, 0x06};
	ret = i2c_read(cmd , 2, buff, 6);
	if (  ret != 0 )
	{

		return -1;
	}
	for (int i =0; i < 6; i++)
	{

		printf("%02x ", buff[i]);
	}
	printf("\n");
	tem = ((buff[0]<<8) | buff[1]);//温度拼接
	hum = ((buff[3]<<8) | buff[4]);//湿度拼接

	tem = ((21875 * tem) >> 13) - 45000;//温度计算
    hum = ((12500 * hum) >> 13);//湿度计算

	*temperature= tem;
	*humidity = hum;

	return 0;
}

