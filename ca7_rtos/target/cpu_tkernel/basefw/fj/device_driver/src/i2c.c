/**
 * @file		interface.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include <string.h> 
#include "i2c.h"
#include "dd_i2c.h"
#include "dd_top.h"
#include "os_user_custom.h"
#include "fj_host.h"
#include "debug.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_INTERFACE_I2C_CH_MAX		(3)
#define D_BF_I2C_PCLK_25_0				(25000000)		// PCLK 25MHz
#define D_BF_I2C_PCLK_50_0				(50000000)		// PCLK 50MHz

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct{
	FJ_I2C_TRANSFER_DATA_BUFFER* trn_data;
	E_DD_I2C_RW_MODE             Read_Write_Mode;
	UINT32                       timeout_threshold;
	UINT32                       retry_num;
}T_BF_INTERFACE_START_I2C;//interface_rw_data's interface

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
volatile static T_DD_I2C_CTRL_MASTER gBF_I2C_Ctrl_Master[D_BF_INTERFACE_I2C_CH_MAX];
volatile static T_DD_I2C_CTRL_SLAVE gBF_I2C_Ctrl_Slave[D_BF_INTERFACE_I2C_CH_MAX];
volatile static I2C_CALLBACK gBF_Interface_Callback[D_BF_INTERFACE_I2C_CH_MAX];

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static T_DD_I2C_SLAVE_ACTION interface_callback(UCHAR ch, E_DD_I2C_RECV_FROM_MASTER receive_sig, UCHAR data)
{
	FJ_I2C_ACTION_SLAVE ret;
	T_DD_I2C_SLAVE_ACTION dd_ret = { D_DD_I2C_SEND_NO_ACK_MASTER, 0 };

	if(gBF_Interface_Callback[ch] != NULL){
		ret = (*gBF_Interface_Callback[ch])( (FJ_PERI_CH)ch, (FJ_I2C_RECEIVE_FROM_MASTER)receive_sig, data);
		switch(ret.mode){
			case FJ_I2C_SEND_DATA:
				dd_ret.action_mode = D_DD_I2C_SEND_DATA_2_MASTER;
				break;
			case FJ_I2C_SEND_ACK:
				dd_ret.action_mode = D_DD_I2C_SEND_ACK_MASTER;
				break;
			case FJ_I2C_SEND_NO_ACK:
				dd_ret.action_mode = D_DD_I2C_SEND_NO_ACK_MASTER;
				break;
			default:
				break;
		}
		dd_ret.send_data = ret.data;
	}

	return dd_ret;
}

static FJ_ERR_CODE interface_rw_data( FJ_PERI_CH ch, T_BF_INTERFACE_START_I2C* start_i2c )
{
	INT32 ret = D_DDIM_OK;
	USHORT i;
	UCHAR retry_num = start_i2c->retry_num;
	T_DD_I2C_START_INFO start_info;

	for(i = 0; i < retry_num; i++) {
		//Retry loop
		start_info.rw_mode		= start_i2c->Read_Write_Mode;
		start_info.rw_data_len	= start_i2c->trn_data->data_length;
		start_info.rw_data		= start_i2c->trn_data->data;
		start_info.timeout		= start_i2c->timeout_threshold;

		ret = Dd_I2C_Start_Master( ch, &start_info );
		if( ret == D_DDIM_OK ){
			//Communication OK
			break;
		}
		else{
			//Restart
			Dd_I2C_Stop_Master( ch );//Send Stop
			Dd_I2C_Terminate( ch );
			Dd_I2C_Ctrl_Master( ch, (T_DD_I2C_CTRL_MASTER*)&gBF_I2C_Ctrl_Master[ch] );
		}
	}

	if( ret != D_DDIM_OK ){
		//Communication NG
		BF_Debug_Print_Error(("I2C Error:=%x\n", ret));
		return FJ_ERR_NG_COMMUNICATON;
	}

	return FJ_ERR_OK;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 *	@brief		Open I2C.
 *	@param		[in] ch					Channel number(range of 0..2).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG				Channel open error. (System using designated channel number)
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_i2c_open( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_BF_INTERFACE_I2C_CH_MAX ){
		BF_Debug_Print_Warning(("fj_i2c_open() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open I2C
	ret = Dd_I2C_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ) {	// I2C Open NG?
		BF_Debug_Print_Warning(("fj_i2c_open() - Error! : ret = 0x%x\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Close I2C.
 *	@param		[in] ch					Channel number(range of 0..2).
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG				Channel open error. (System using designated channel number)
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_i2c_close( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_BF_INTERFACE_I2C_CH_MAX ){
		BF_Debug_Print_Warning(("fj_i2c_close() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Close I2C
	ret = Dd_I2C_Close( ch );
	if( ret != D_DDIM_OK ) {	// I2C Close NG?
		BF_Debug_Print_Warning(("fj_i2c_close() - Error! : ret = 0x%x\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		The operation condition of I2C is set.
 *	@param		[in] ch					Channel number(range of 0..2).
 *	@param		[in] i2c_ctrl			Slave address. See @ref FJ_I2C_CTRL
 *	@param		[in] pcallback			Slave callback function.
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG				Error occurred.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_i2c_ctrl( FJ_PERI_CH ch, FJ_I2C_CTRL i2c_ctrl, I2C_CALLBACK pcallback )
{
	INT32 ret;

	if( ch >= D_BF_INTERFACE_I2C_CH_MAX ){
		BF_Debug_Print_Warning(("fj_i2c_ctrl() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( i2c_ctrl.type == FJ_I2C_TYPE_MASTER ) {
		gBF_I2C_Ctrl_Master[ch].bps					= (E_DD_I2C_BPS)i2c_ctrl.speed;
		gBF_I2C_Ctrl_Master[ch].dest_slave_addr_len	= (E_DD_I2C_ADDR_LEN)i2c_ctrl.address_format;
		gBF_I2C_Ctrl_Master[ch].dest_slave_addr		= i2c_ctrl.slave_address;
		gBF_Interface_Callback[ch] = NULL;

		// Set Control I2C Master
		ret = Dd_I2C_Ctrl_Master( ch, (T_DD_I2C_CTRL_MASTER*)&gBF_I2C_Ctrl_Master[ch] );
		if( ret != D_DDIM_OK ) {	// I2C Ctrl NG?
			BF_Debug_Print_Warning(("fj_i2c_ctrl() - Dd_I2C_Ctrl_Master Error! : ret = 0x%x\n", ret));
			return FJ_ERR_NG;
		}
	}
	else {	// Slave
		gBF_I2C_Ctrl_Slave[ch].own_slave_addr_len	= (E_DD_I2C_ADDR_LEN)i2c_ctrl.address_format;
		gBF_I2C_Ctrl_Slave[ch].own_slave_addr		= i2c_ctrl.slave_address;
		gBF_I2C_Ctrl_Slave[ch].global_call_en		= 0;
		gBF_I2C_Ctrl_Slave[ch].callback				= (VP_I2C_CALLBACK)interface_callback;
		gBF_Interface_Callback[ch] = pcallback;

		// Set Control I2C Slave
		ret = Dd_I2C_Ctrl_Slave( ch, (T_DD_I2C_CTRL_SLAVE*)&gBF_I2C_Ctrl_Slave[ch] );
		if( ret != D_DDIM_OK ) {	// I2C Ctrl NG?
			BF_Debug_Print_Warning(("fj_i2c_ctrl() - Dd_I2C_Ctrl_Slave Error! : ret = 0x%x\n", ret));
			return FJ_ERR_NG;
		}

		// Start I2C Slave
		ret = Dd_I2C_Start_Slave( ch );
		if( ret != D_DDIM_OK ) {	// I2C Ctrl NG?
			BF_Debug_Print_Warning(("fj_i2c_ctrl() - Dd_I2C_Start_Slave Error! : ret = 0x%x\n", ret));
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Send or receive data to slave side by I2C.
 *	@param		[in] ch					Channel number(range of 0..2).
 *	@param		[in,out]  trans_ctrl	Send or receive data array. See @ref FJ_I2C_TRANSFER_DATA
 *	@param		[in] length				data's lenght.
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG				Error occurred.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_i2c_transfer( FJ_PERI_CH ch, FJ_I2C_TRANSFER_DATA trans_ctrl[], UINT32 length )
{
	T_BF_INTERFACE_START_I2C    start_i2c;
	FJ_I2C_TRANSFER_DATA_BUFFER send_data;
	UCHAR                       temp_data[64];
	INT32                       i;
	FJ_ERR_CODE                 ret=FJ_ERR_OK;

	if( ch >= D_BF_INTERFACE_I2C_CH_MAX ){
		BF_Debug_Print_Warning(("fj_i2c_transfer() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Transfer I2C
	for(i = 0; i < length; i++) {
		//Data loop
		start_i2c.Read_Write_Mode = E_DD_I2C_RW_MODE_WRITE;
		start_i2c.timeout_threshold = trans_ctrl[i].timeout_threshold;
		start_i2c.retry_num = trans_ctrl[i].retry_num;

		if(trans_ctrl[i].dirction == 0) {
			//Write
			if(trans_ctrl[i].trn_data.data_length < 64) {
				memcpy(&temp_data[0],
				       &trans_ctrl[i].slave_register_address.data[0],
				       trans_ctrl[i].slave_register_address.data_length);
				memcpy(&temp_data[trans_ctrl[i].slave_register_address.data_length],
				       &trans_ctrl[i].trn_data.data[0],
				       trans_ctrl[i].trn_data.data_length);

				send_data.data = temp_data;
				send_data.data_length = (USHORT)(trans_ctrl[i].trn_data.data_length + trans_ctrl[i].slave_register_address.data_length);

				start_i2c.trn_data = &send_data;

				ret = interface_rw_data( ch, &start_i2c );
				if(ret != FJ_ERR_OK){
					//Communication NG
					break;
				}
			}
			else{
				//Communication NG
				ret = FJ_ERR_NG_INPUT_PARAM;
				break;
			}
		}
		else {
			//Read
			start_i2c.trn_data = &(trans_ctrl[i].slave_register_address);

			ret = interface_rw_data( ch, &start_i2c );
			if(ret == FJ_ERR_OK){
				//Communication OK
				start_i2c.Read_Write_Mode = E_DD_I2C_RW_MODE_READ;
				start_i2c.trn_data = &(trans_ctrl[i].trn_data);

				ret = interface_rw_data( ch, &start_i2c ); 
				if(ret != FJ_ERR_OK){
					//Communication NG
					break;
				}
			}
			else{
				//Communication NG
				break;
			}
		}
	}

	Dd_I2C_Stop_Master( ch );//Send Stop
	Dd_I2C_Terminate( ch );

	return ret;
}

/**
 *	@brief		Set I2C serial clock frequency.
 *	@param		[in] ch					Channel number(range of 0..2).
 *	@param		[in] req_clock			Request clock of I2C serial clock frequency (KHz)
 *	@param		[out] act_clock			Actual clock of I2C serial clock frequency (KHz)
 *	@param		[in] under_flag			Approximation flag (=TRUE:Under, =FALSE:Over)
 *	@return		FJ_ERR_OK				Success.
 *	@return		FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_i2c_set_approxclock( FJ_PERI_CH ch, UINT32 req_clock, UINT32* act_clock, BOOL under_flag )
{
	ULONG  pclk;
	UINT32 cal_clock;
	UINT32 scl32;
	INT32  ret;
	UCHAR  scl;

	if( ch >= D_BF_INTERFACE_I2C_CH_MAX ){
		BF_Debug_Print_Warning(("fj_i2c_set_approxclock() - Error! : ch = %d\n", ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( req_clock == 0 ){
		BF_Debug_Print_Warning(("fj_i2c_set_approxclock() - Error! : req_clock = %d\n", req_clock));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	pclk = Dd_Top_Get_PCLK();
	scl32 = pclk / (req_clock * 1000 * 4);
	if( ( scl32 < 4 ) || (scl32 > 127 ) ){
		BF_Debug_Print_Warning(("fj_i2c_set_approxclock() - Error! : scl32 = %d\n", scl32));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	scl = (UCHAR)scl32;

	cal_clock = pclk / (scl * 1000 * 4);
	if( ( cal_clock > req_clock ) && ( under_flag ) ){
		if( scl == 127 ){
			BF_Debug_Print_Warning(("fj_i2c_set_approxclock() - Error! : scl = %d\n", scl));
			return FJ_ERR_NG_INPUT_PARAM;
		}
		scl += 1;
		cal_clock = pclk / (scl * 1000 * 4);
	}

	*act_clock = cal_clock;

	ret = Dd_I2C_Set_SCL( ch, scl );
	if( ret != D_DDIM_OK ) {	// I2C Set SCL NG?
		return FJ_ERR_NG_INPUT_PARAM;
	}

	return FJ_ERR_OK;
}
