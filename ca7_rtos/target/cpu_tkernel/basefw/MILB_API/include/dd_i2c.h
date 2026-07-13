/**
 * @file		dd_i2c.h
 * @brief		I2C driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_i2c
@{
*/

#ifndef _DD_I2C_H_
#define _DD_I2C_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of I2C processing result
#define	D_DD_I2C_INPUT_PARAM_ERROR	(D_DD_I2C | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error	*/
#define	D_DD_I2C_SEM_NG				(D_DD_I2C | D_DDIM_SEM_NG) 				/**< Semaphore acquisition NG	*/
#define	D_DD_I2C_TIMEOUT			(D_DD_I2C | D_DDIM_TIMEOUT) 			/**< Timeout (need to Dd_I2C_Close())	*/
#define	D_DD_I2C_BUS_ERROR			(D_DD_I2C | D_DDIM_BUS_ERROR)			/**< Bus error */
#define D_DD_I2C_STATUS_ABNORMAL	(D_DD_I2C | D_DDIM_STATUS_ABNORMAL)		/**< Abnormal Status */
#define	D_DD_I2C_ARBITRATION_LOST	(D_DD_I2C | D_DDIM_ARBITRATION_LOST)	/**< Arbitration lost detect */
#define	D_DD_I2C_COMM_ERROR			(D_DD_I2C | D_DDIM_COMM_ERROR)			/**< Communication error */
#define	D_DD_I2C_PEC_ERROR			(D_DD_I2C | D_DDIM_PARITY_DATA)			/**< Packet Error */

#define D_DD_I2C_SEND_DATA_2_MASTER	(1)			/**<Send data to master*/
#define D_DD_I2C_SEND_ACK_MASTER	(2)			/**<Send ACK to master*/
#define D_DD_I2C_SEND_NO_ACK_MASTER (3)			/**<Send NO ACK to master*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Recieve action from master side */
typedef enum {
	E_DD_I2C_RECV_FROM_MASTER_NO_ACK = 0,	/**< Receive NO ACK from Master	*/
	E_DD_I2C_RECV_FROM_MASTER_ACK,			/**< Receive ACK from Master	*/
	E_DD_I2C_RECV_FROM_MASTER_DATA,			/**< Receive Data from Master	*/
	E_DD_I2C_RECV_FROM_MASTER_ADDRESS,		/**< Receive Address from Master.*/
	E_DD_I2C_RECV_FROM_MASTER_ERROR,		/**< Error occured.*/
} E_DD_I2C_RECV_FROM_MASTER;

/** Communication Mode */
typedef enum {
	E_DD_I2C_RW_MODE_READ = 0,					/**< Read from slave only	*/
	E_DD_I2C_RW_MODE_WRITE,						/**< Write to slave only	*/
} E_DD_I2C_RW_MODE;

/** Communication rate */
typedef enum {
	E_DD_I2C_BPS_100 = 0,						/**< 100kbps	*/
	E_DD_I2C_BPS_400							/**< 400kbps	*/
} E_DD_I2C_BPS;

/** Address length */
typedef enum {
	E_DD_I2C_ADDR_LEN_7 = 0,					/**< 7 bit address	*/
	E_DD_I2C_ADDR_LEN_10						/**< 10 bit address	*/
} E_DD_I2C_ADDR_LEN;

/** Timeout Divider */
typedef enum {
	E_DD_I2C_TO_DIV_NO_CHECK = 0,				/**< Timer Disabled			*/
	E_DD_I2C_TO_DIV_4,							/**< Divide by 4			*/
	E_DD_I2C_TO_DIV_8,							/**< Divide by 8			*/
	E_DD_I2C_TO_DIV_16,							/**< Divide by 16			*/
} E_DD_I2C_TO_DIV;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Send Action from slave side */
typedef struct {
	UCHAR action_mode;	/**< What do at slave.<br> */
						/**< @ref D_DD_I2C_SEND_DATA_2_MASTER : Send data<br> */
						/**< @ref D_DD_I2C_SEND_ACK_MASTER    : Send ACK <br> */
						/**< @ref D_DD_I2C_SEND_NO_ACK_MASTER : Send NO ACK<br> */
	UCHAR send_data;	/**< Send data */
}T_DD_I2C_SLAVE_ACTION;

/**
Type is defined to Callback function pointer.<br>
@param [in] UCHAR						Channel number.
@param [in] E_DD_I2C_RECV_FROM_MASTER	Recieved action from master side.
@param [in] UCHAR						Received data from master side.
@retval T_DD_I2C_SLAVE_ACTION	Send action from slave side.
*/
typedef T_DD_I2C_SLAVE_ACTION (*VP_I2C_CALLBACK)(UCHAR, E_DD_I2C_RECV_FROM_MASTER, UCHAR);

/** Control information for Master */
typedef struct {
	E_DD_I2C_BPS		bps;					/**< Communication rate	*/
	E_DD_I2C_ADDR_LEN	dest_slave_addr_len;	/**< Destination slave address length */
	USHORT				dest_slave_addr;		/**< Destination slave address */
} T_DD_I2C_CTRL_MASTER;

/** Control information for Slave */
typedef struct {
	E_DD_I2C_ADDR_LEN	own_slave_addr_len;	/**< Own Slave address length */
	USHORT				own_slave_addr;		/**< Own Slave address */
	UCHAR 				global_call_en;		/**< Global Call Match Enable */
	VP_I2C_CALLBACK		callback;			/**< Callback function pointer. See @ref VP_I2C_CALLBACK.<br>
												 Please must set a callback in the case of Slave operation. */
} T_DD_I2C_CTRL_SLAVE;

/** Control information for SMBus (for Master and Slave) */
typedef struct {
	UCHAR 				alert_resp_en;		/**< Alert Response Match Enable (Slave only) */
	UINT32				pec_num;			/**< PEC(Packet Error Checking) number of bytes (0 is disable) */
	E_DD_I2C_TO_DIV		timeout_div;		/**< Timeout Divider */
	UCHAR				timeout_presc;		/**< Timeout Prescaler ( 0 to 255 ) */
} T_DD_I2C_CTRL_SMBUS;

/** Start information */
typedef struct {
	E_DD_I2C_RW_MODE	rw_mode;			/**< Read or Write */
	UINT32				rw_data_len;		/**< Read/Write data length */
	UCHAR* 				rw_data;			/**< Read/Write data */
	UINT32				timeout;			/**< Communication timeout value (ms) */
} T_DD_I2C_START_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Execute exclusive control for designated I2C channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_I2C_SEM_NG".<br>
When timeout occurs, return "D_DD_I2C_TIMEOUT".
@param [in]	ch		Channel number(0 to 2)
@param [in] timeout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval	D_DDIM_OK					: OK
@retval	D_DD_I2C_INPUT_PARAM_ERROR	: Input parameter error
@retval	D_DD_I2C_TIMEOUT			: Semaphore acquisition Time Out
@retval	D_DD_I2C_SEM_NG				: Semaphore acquisition NG
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Dd_I2C_Open( UCHAR ch, INT32 timeout );

/**
Cancel exclusive control for designated I2C channel.
@param[in] ch		I2C channel number ( 0 to 2 )
@retval	D_DDIM_OK					: OK
@retval	D_DD_I2C_INPUT_PARAM_ERROR	: Input parameter error
@retval	D_DD_I2C_SEM_NG				: Semaphore acquisition NG
@remarks	Please note that stop & exclusive control is compulsorily released when I2C is starting.
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Dd_I2C_Close( UCHAR ch );

/**
Set I2C master contorol information.
@param[in] ch				I2C channel number ( 0 to 2 )
@param[in] *ctrl_master		Master control info. @ref T_DD_I2C_CTRL_MASTER.<br>
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
@remarks Please call either the Dd_I2C_Ctrl_Master() or Dd_I2C_Ctrl_Slave().
*/
extern INT32 Dd_I2C_Ctrl_Master( UCHAR ch, const T_DD_I2C_CTRL_MASTER* const ctrl_master );

/**
Get I2C master contorol information.
@param[in]  ch				I2C channel number ( 0 to 2 )
@param[out] *ctrl_master	Master control info. @ref T_DD_I2C_CTRL_MASTER.<br>
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
*/
extern INT32 Dd_I2C_Get_Ctrl_Master( UCHAR ch, T_DD_I2C_CTRL_MASTER* const ctrl_master );

/**
Set I2C slave contorol information.
@param[in] ch				I2C channel number ( 0 to 2 )
@param[in] *ctrl_slave		Slave control info. @ref T_DD_I2C_CTRL_SLAVE.<br>
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
@remarks Please call either the Dd_I2C_Ctrl_Master() or Dd_I2C_Ctrl_Slave().
*/
extern INT32 Dd_I2C_Ctrl_Slave( UCHAR ch, const T_DD_I2C_CTRL_SLAVE* const ctrl_slave );

/**
Get I2C slave contorol information.
@param[in]  ch				I2C channel number ( 0 to 2 )
@param[out] *ctrl_slave		Slave control info. @ref T_DD_I2C_CTRL_SLAVE.<br>
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
*/
extern INT32 Dd_I2C_Get_Ctrl_Slave( UCHAR ch, T_DD_I2C_CTRL_SLAVE* const ctrl_slave );

/**
Set SMBus slave contorol information.
@param[in] ch				I2C channel number ( 0 to 2 )
@param[in] *ctrl_smbus		SMBus Control info. @ref T_DD_I2C_CTRL_SMBUS.<br>
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
@retval D_DD_I2C_STATUS_ABNORMAL		: Abnormal Status
@remarks Please call this API as necessary after the Dd_I2C_Ctrl_Master() or Dd_I2C_Ctrl_Slave().
@remarks If you want to enable the PEC, please set the transmit and receive data byte number(T_DD_I2C_START_INFO.rw_data_len) + 1.
*/
extern INT32 Dd_I2C_Ctrl_SMBus( UCHAR ch, const T_DD_I2C_CTRL_SMBUS* const ctrl_smbus );

/**
Get SMBus slave contorol information.
@param[in]  ch				I2C channel number ( 0 to 2 )
@param[out] *ctrl_smbus		SMBus Control info. @ref T_DD_I2C_CTRL_SMBUS.<br>
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
@retval D_DD_I2C_STATUS_ABNORMAL		: Abnormal Status
*/
extern INT32 Dd_I2C_Get_Ctrl_SMBus( UCHAR ch, T_DD_I2C_CTRL_SMBUS* const ctrl_smbus );

/**
Send start condition.
@param[in] ch 			2C's ch number ( 0 to 2 )
@param[in] start_info	Start information
@retval D_DDIM_OK					: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR	: Input Parameter Error
@retval D_DD_I2C_ERROR				: System Error
@retval D_DD_I2C_BUS_ERROR			: Bus Error
@retval D_DD_I2C_ARBITRATION_LOST	: Arbitration lost detect
@retval D_DD_I2C_COMM_ERROR			: Communication Error
@retval D_DD_I2C_TIMEOUT			: Process Timeout
@retval D_DD_I2C_STATUS_ABNORMAL	: Abnormal Status
@remarks This API uses DDIM_User_Clr_Flg().
@remarks This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Dd_I2C_Start_Master( UCHAR ch, const T_DD_I2C_START_INFO* const start_info );

/**
Send stop condition.
@param[in] ch	I2C channel number ( 0 to 2 )
@retval D_DDIM_OK					: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR	: Input Parameter Error
@retval D_DD_I2C_STATUS_ABNORMAL	: Abnormal Status
*/
extern INT32 Dd_I2C_Stop_Master( UCHAR ch );

/**
Terminate the I2C.
@param[in] ch	I2C channel number ( 0 to 2 )
*/
extern VOID Dd_I2C_Terminate( UCHAR ch );

/**
Start the communication waiting from the master.
@param[in] ch 			2C's ch number ( 0 to 2 )
@retval D_DDIM_OK					: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR	: Input Parameter Error
@retval D_DD_I2C_STATUS_ABNORMAL	: Abnormal Status
*/
extern INT32 Dd_I2C_Start_Slave( UCHAR ch );

/**
Stop thje communication waiting from the master.
@param[in] ch	I2C channel number ( 0 to 2 )
@retval D_DDIM_OK					: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR	: Input Parameter Error
*/
extern INT32 Dd_I2C_Stop_Slave( UCHAR ch );

/**
Get Test SDA.
@param[in] ch		I2C channel number ( 0 to 2 )
@param[out] sda		Test SDA value ( 0:Low, 1:High )
@retval D_DDIM_OK					: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR	: Input Parameter Error
*/
extern INT32 Dd_I2C_Get_Test_SDA( UCHAR ch, UCHAR* sda );

/**
Set Toggle SCL.
When SDA is low, setting 1 to this API toggles SCL for one cycle.<br>
Setting 1 to this API while SDA is high is ignored.<br>
@param[in] ch		I2C channel number ( 0 to 2 )
@retval D_DDIM_OK					: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR	: Input Parameter Error
*/
extern INT32 Dd_I2C_Set_Toggle_SCL( UCHAR ch );

/**
Get Error Cause.
@param[in] ch		I2C channel number ( 0 to 2 )
@retval D_DDIM_OK					: OK
@retval	D_DD_I2C_TIMEOUT			: Timeout (need to Dd_I2C_Close())
@retval	D_DD_I2C_BUS_ERROR			: Bus error
@retval	D_DD_I2C_ARBITRATION_LOST	: Arbitration lost detect
@retval	D_DD_I2C_COMM_ERROR			: Communication error
@retval	D_DD_I2C_PEC_ERROR			: Packet Error
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern UINT32 Dd_I2C_Get_Error_Cause( UCHAR ch );

/**
Intterupt handler.
The interruption flag is cleared, and the CallBack function is called when slave operatrion.<br>
(Only an interruption clear flag of CallBack when it is unregistered.)
@param [in] ch	I2C channel number ( 0 to 2 )
*/
extern VOID Dd_I2C_Int_Handler( UCHAR ch );


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** @name Utility Functions
@{*/
/**
Set SCL frequency utility function.<br>
@param [in] ch	I2C channel number ( 0 to 2 )
@param [in] scl	SCL frequency (range of 4..127)
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
@remarks Selecting a value of less than 4 will automatically default by adding an offset of 4.
*/
extern INT32 Dd_I2C_Set_SCL( UCHAR ch, UCHAR scl );

/**
Get SCL frequency utility function.<br>
@param [in]  ch		I2C channel number ( 0 to 2 )
@param [out] scl	SCL frequency
@retval D_DDIM_OK						: OK
@retval D_DD_I2C_INPUT_PARAM_ERROR		: Input Parameter Error
*/
extern INT32 Dd_I2C_Get_SCL( UCHAR ch, UCHAR* scl );

/*@}*/
#endif

#ifdef __cplusplus
}
#endif
/*@}*/

/************************************************************************/
/* Sample Code															*/
/************************************************************************/
/** @weakgroup dd_i2c
@{
	- @ref I2C_Sample_Master_Transmit
	- @ref I2C_Sample_Master_Receive
	- @ref I2C_Sample_Slave_Operation
	<hr>

	@section I2C_Sample_Master_Transmit		I2C Sample Code for Master Transmit
	<table  border="1">
	<tr>
	  <th width="15.75">S</th>
	  <th width="74.25">Slave Address</th>
	  <th width="15.75">W</th>
	  <th width="22.5">Ack</th>
	  <th width="28.5">Data</th>
	  <th width="23.25">Ack</th>
	  <th width="20.25">...</th>
	  <th width="26.25">Data</th>
	  <th width="24.75">Ack</th>
	  <th width="18.75">P</th>
	</tr>
	</table>
	@code
	// Master Send Data to Slave
	VOID master_send_data_to_slave( VOID )
	{
		INT32 ret;
		T_DD_I2C_CTRL_MASTER ctrl_master;
		T_DD_I2C_START_INFO start_info;
		UCHAR SendData[10] = { 0,1,2,3,4,5,6,7,8,9 };

		ret = Dd_I2C_Open( 0, D_DDIM_SEM_WAIT_FEVR ); // Open I2C Channel
		if( ret == D_DDIM_OK ){

			while(1){ // Retry loop as error occur

				ctrl_master.bps = E_DD_I2C_BPS_400;
				ctrl_master.dest_slave_addr_len = E_DD_I2C_ADDR_LEN_7;
				ctrl_master.dest_slave_addr = 0x45;

				ret = Dd_I2C_Ctrl_Master( 0, &ctrl_master ); // Set Control for Master Operation
				if( ret == D_DDIM_OK ){

					start_info.rw_mode = E_DD_I2C_RW_MODE_WRITE;
					start_info.rw_data_len = sizeof(SendData);
					start_info.rw_data = SendData;

					ret = Dd_I2C_Start_Master( 0, &start_info ); // Start Send
					if( ret == D_DDIM_OK ){
						// Send OK
						Dd_I2C_Stop_Master( 0 ); // Stop Send
						break;
					}
				}
			}
			Dd_I2C_Close( 0 ); // Close I2C Channel
		}
	}
	@endcode
	<br>
	<br>

	@section I2C_Sample_Master_Receive		I2C Sample Code for Master Receive
	<table  border="1">
	<tr>
	  <th width="17.25">S</th>
	  <th width="75.75">Slave Address</th>
	  <th width="12.75">R</th>
	  <th width="20.25">Ack</th>
	  <th width="28.5">Data</th>
	  <th width="23.25">Ack</th>
	  <th width="20.25">...</th>
	  <th width="26.25">Data</th>
	  <th width="30.75">NoAck</th>
	  <th width="18.75">P</th>
	</tr>
	</table>
	@code
	// Master Receive Data from Slave
	VOID master_receive_data_from_slave(USHORT SlaveAddress)
	{
		INT32 ret;
		T_DD_I2C_CTRL_MASTER ctrl_master;
		T_DD_I2C_START_INFO start_info;
		UCHAR RecvData[10];

		ret = Dd_I2C_Open( 0, D_DDIM_SEM_WAIT_FEVR ); // Open I2C Channel
		if( ret == D_DDIM_OK ){

			while(1){ // Retry loop as error occur

				ctrl_master.bps = E_DD_I2C_BPS_400;
				ctrl_master.dest_slave_addr_len = E_DD_I2C_ADDR_LEN_7;
				ctrl_master.dest_slave_addr = 0x45;

				ret = Dd_I2C_Ctrl_Master( 0, &ctrl_master ); // Set Control for Master Operation
				if( ret == D_DDIM_OK ){

					start_info.rw_mode = E_DD_I2C_RW_MODE_READ;
					start_info.rw_data_len = sizeof(RecvData);
					start_info.rw_data = RecvData;

					ret = Dd_I2C_Start_Master( 0, &start_info ); // Start Receive
					if( ret == D_DDIM_OK ){
						// Receive OK
						Dd_I2C_Stop_Master( 0 ); // Stop Receive
						break;
					}
				}
			}
			Dd_I2C_Close( 0 ); // Close I2C Channel
		}
	}
	@endcode
	<br>
	<br>

	@section I2C_Sample_Slave_Operation		I2C Sample Code for Slave Operation
	@code
	// I2C Callback Function
	T_DD_I2C_SLAVE_ACTION i2c_callback_func( UCHAR ch, E_DD_I2C_RECV_FROM_MASTER receive_sig, UCHAR data )
	{
		T_DD_I2C_SLAVE_ACTION ret_act = { D_DD_I2C_SEND_NO_ACK_MASTER, 0 };

		switch( receive_sig ){
			case E_DD_I2C_RECV_FROM_MASTER_ADDRESS:
				// receive slave address
				gI2c_slave_send_address = 0x7000A000;
				gI2c_slave_recv_address = 0x7000B000;
				gI2c_slave_recv_counter = 0;
				gI2c_slave_send_counter = 0;
				break;

			case E_DD_I2C_RECV_FROM_MASTER_DATA:
				// received data from mastr
				if( gI2c_slave_recv_counter < 10 ){
					// Replay ACK to master
					ret_act.send_data = D_DD_I2C_SEND_ACK_MASTER;

					// Get receive data
					*gI2c_slave_recv_address = data;
					gI2c_slave_recv_address++;
					gI2c_slave_recv_counter++;
				}
				else {
					// Replay NOACK to master
					ret_act.action_mode = D_DD_I2C_SEND_NO_ACK_MASTER;
				}
				break;

			case E_DD_I2C_RECV_FROM_MASTER_ACK:
				if( gI2c_slave_send_counter < 10 ){
					//Replay ACK to master
					ret_act.action_mode = D_DD_I2C_SEND_DATA_2_MASTER;

					// Set send data
					ret_act.send_data = *gI2c_slave_send_address;
					gI2c_slave_send_address++;
					gI2c_slave_send_counter++;
				}
				break;

			default:
				break;
		}
	}

	// Set Slave Operation
	VOID slave_set_operation( VOID )
	{
		INT32 ret;
		T_DD_I2C_CTRL_SLAVE ctrl_slave;
		UCHAR RecvData[10];

		ret = Dd_I2C_Open( 1, D_DDIM_SEM_WAIT_FEVR ); // Open I2C Channel
		if( ret == D_DDIM_OK ){

			while(1){ // Retry loop as error occur

				ctrl_slave.own_slave_addr_len = E_DD_I2C_ADDR_LEN_7;
				ctrl_slave.own_slave_addr = 0x45;
				ctrl_slave.global_call_en = 0;
				ctrl_slave.callback = i2c_callback_func;

				ret = Dd_I2C_Ctrl_Slave( 1, &ctrl_slave ); // Set Control for Slave Operation
				if( ret == D_DDIM_OK ){

					ret = Dd_I2C_Start_Slave( 1 ); // Start Slave
					if( ret == D_DDIM_OK ){
						break;
					}
				}
			}
		}
	}
	@endcode

*/


#endif

/*@}*/
