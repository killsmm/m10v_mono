/**
 * @file		ct_dd_relc.c
 * @brief		This is test code for RELC driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#pragma arm section rwdata="BASEFW_LAST_INIT"
//#pragma arm section rodata="BASEFW_LAST_CONST"
//#pragma arm section code="BASEFW_LAST_CODE"

//#include "COMMAND_ON_OPTION.h"
#include <stdlib.h>
#include <string.h>
#include "_relc.h"
#include "dd_relc.h"
#include "exstop.h"
#include "chiptop.h"
#include "dd_exs.h"

//#define PC_DEBUG
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
static ULONG gRELC_Desc_Table1[] = {
	0x00000000, 0x58100000, 0x00000000, 0x58200000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x58300000, 0x00000000, 0x58400000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x58500000, 0x00000000, 0x58600000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Continuous mode judge data */
static volatile UCHAR gRELC_Cont_Mode;
/* Working buffer */
#ifdef PC_DEBUG
static volatile ULONG buf[ 2048 ];
#endif
/*----------------------------------------------------------------------*/
/* Local Function												        */
/*----------------------------------------------------------------------*/

static VOID ct_relc_register_dump( VOID )
{
#ifdef PC_DEBUG
	Ddim_Print(("RELC REGISTER INFO.\n"));
	Ddim_Print(("IO_RELC.RELC_RESET.bit.SRST        = %d\n",	IO_RELC.RELC_RESET.bit.SRST));
	Ddim_Print(("IO_RELC.RELC_MODE1.bit.PRUN        = %d\n",	IO_RELC.RELC_MODE1.bit.PRUN));
	Ddim_Print(("IO_RELC.RELC_MODE1.bit.CRMD        = %d\n",	IO_RELC.RELC_MODE1.bit.CRMD));
	Ddim_Print(("IO_RELC.RELC_MODE1.bit.DSMD        = %d\n",	IO_RELC.RELC_MODE1.bit.DSMD));
	Ddim_Print(("IO_RELC.RELC_MODE2.bit.IMAX        = %d\n",	IO_RELC.RELC_MODE2.bit.IMAX));
	Ddim_Print(("IO_RELC.RELC_MODE2.bit.OMAX        = %d\n",	IO_RELC.RELC_MODE2.bit.OMAX));
	Ddim_Print(("IO_RELC.RELC_MODE2.bit.IAAI        = %d\n",	IO_RELC.RELC_MODE2.bit.IAAI));
	Ddim_Print(("IO_RELC.RELC_MODE2.bit.OAAI        = %d\n",	IO_RELC.RELC_MODE2.bit.OAAI));
	Ddim_Print(("IO_RELC.RELC_INT_EN.bit.SINT       = %d\n",	IO_RELC.RELC_INT_EN.bit.SINT));
	Ddim_Print(("IO_RELC.RELC_INT_EN.bit.FINT       = %d\n",	IO_RELC.RELC_INT_EN.bit.FINT));
	Ddim_Print(("IO_RELC.RELC_INT_EN.bit.EINT       = %d\n",	IO_RELC.RELC_INT_EN.bit.EINT));
	Ddim_Print(("IO_RELC.RELC_INT_EN.bit.CINT       = %d\n",	IO_RELC.RELC_INT_EN.bit.CINT));
	Ddim_Print(("IO_RELC.RELC_INT_ST.bit.__SINS     = %d\n",	IO_RELC.RELC_INT_ST.bit.__SINS));
	Ddim_Print(("IO_RELC.RELC_INT_ST.bit.__FINS     = %d\n",	IO_RELC.RELC_INT_ST.bit.__FINS));
	Ddim_Print(("IO_RELC.RELC_INT_ST.bit.__EINS     = %d\n",	IO_RELC.RELC_INT_ST.bit.__EINS));
	Ddim_Print(("IO_RELC.RELC_INT_ST.bit.__CINS     = %d\n",	IO_RELC.RELC_INT_ST.bit.__CINS));
	Ddim_Print(("IO_RELC.RELC_STATUS.bit.RUNS       = %d\n",	IO_RELC.RELC_STATUS.bit.RUNS));
	Ddim_Print(("IO_RELC.RELC_STATUS.bit.SLPS       = %d\n",	IO_RELC.RELC_STATUS.bit.SLPS));
	Ddim_Print(("IO_RELC.RELC_STATUS.bit.FINS       = %d\n",	IO_RELC.RELC_STATUS.bit.FINS));
	Ddim_Print(("IO_RELC.RELC_STATUS.bit.SRES       = %d\n",	IO_RELC.RELC_STATUS.bit.SRES));
	Ddim_Print(("IO_RELC.RELC_STATUS.bit.EFLG       = %d\n",	IO_RELC.RELC_STATUS.bit.EFLG));
	Ddim_Print(("IO_RELC.RELC_IN_START_ADDR         = %08LX\n",	IO_RELC.RELC_IN_START_ADDR));
	Ddim_Print(("IO_RELC.RELC_IN_END_ADDR           = %08LX\n",	IO_RELC.RELC_IN_END_ADDR));
	Ddim_Print(("IO_RELC.RELC_OUT_START_ADDR        = %08LX\n",	IO_RELC.RELC_OUT_START_ADDR));
	Ddim_Print(("IO_RELC.RELC_OUT_END_ADDR          = %08LX\n",	IO_RELC.RELC_OUT_END_ADDR));
	Ddim_Print(("IO_RELC.RELC_DESCRIPTOR_ADDR       = %08LX\n",	IO_RELC.RELC_DESCRIPTOR_ADDR));
	Ddim_Print(("IO_RELC.RELC_SEQ_NUM               = %d\n",	IO_RELC.RELC_SEQ_NUM));
	Ddim_Print(("IO_RELC.RELC_SEQ_CNT               = %d\n",	IO_RELC.RELC_SEQ_CNT));
	Ddim_Print(("IO_RELC.RELC_BUF_SIZE.bit.IBFS     = %d\n",	IO_RELC.RELC_BUF_SIZE.bit.IBFS));
	Ddim_Print(("IO_RELC.RELC_BUF_SIZE.bit.RBFS     = %d\n",	IO_RELC.RELC_BUF_SIZE.bit.RBFS));
	Ddim_Print(("IO_RELC.RELC_ERROR_NUM.bit.ENUM    = %d\n",	IO_RELC.RELC_ERROR_NUM.bit.ENUM));
	Ddim_Print(("IO_RELC.RELC_READ_BYTE.bit.DRNM    = %d\n",	IO_RELC.RELC_READ_BYTE.bit.DRNM));
	Ddim_Print(("IO_RELC.RELC_WRITE_BYTE.bit.DWNM   = %d\n",	IO_RELC.RELC_WRITE_BYTE.bit.DWNM));
	Ddim_Print(("IO_RELC.RELC_BLOCK.bit.BLKN        = %d\n",	IO_RELC.RELC_BLOCK.bit.BLKN));
	Ddim_Print(("IO_RELC.RELC_INPUT_BYTE            = %08LX\n",	IO_RELC.RELC_INPUT_BYTE));
	Ddim_Print(("IO_RELC.RELC_OUTPUT_BYTE           = %08LX\n",	IO_RELC.RELC_OUTPUT_BYTE));
	Ddim_Print(("IO_RELC.RELC_INPUT_BUF_DATA_MIRROR = %08LX\n",	IO_RELC.RELC_INPUT_BUF_DATA_MIRROR));
	Ddim_Print(("IO_RELC.RELC_INPUT_BUF_DATA        = %08LX\n",	IO_RELC.RELC_INPUT_BUF_DATA));
	Ddim_Print(("IO_RELC.RELC_REF_BUF_DATA          = %08LX\n",	IO_RELC.RELC_REF_BUF_DATA));
#endif
}

#ifdef PC_DEBUG
static void put_dump( ULONG *buffer, ULONG ofs, ULONG adr, USHORT cnt )
{
	unsigned short n;

	Ddim_Print(("\r\n%08lX  %08lX ", ofs, adr));

	for ( n = 0 ; n < cnt ; n++ ) {
		Ddim_Print((" %08lX", buf[n]));
	}
}
#endif

static VOID ct_relc_buffer_dump( VOID )
{
#ifdef PC_DEBUG
	LONG *ptr;
	ULONG addr;
	ULONG ofs;
	ULONG adr;

	// RELC Input data Buffer Mirror [2k byte] Base Address + 1000h
	addr = 0x3A001000;
	memcpy( (ULONG*)buf, (VOID*)IO_RELC.RELC_INPUT_BUF_DATA_MIRROR, 8192 );

	Ddim_Print(("\n RELC Input data Buffer Mirror [2k byte] Base Address + 1000h"));
	Ddim_Print(("\n OFFSET    ADDRESS   0 1 2 3   4 5 6 7   8 9 A B   C D E F  "));
	Ddim_Print(("\n ---------+---------+---------+---------+---------+---------+"));

	for ( ptr = (LONG*)buf, ofs = 0, adr = addr ; adr < addr + 0x800 ; ptr+=16, ofs+=16, adr+=16 ) {
		put_dump( (ULONG*)ptr, ofs, adr, 4 );
	}

	// RELC Input data Buffer        [2k byte] Base Address + 1800h
	addr = 0x3A001800;
	memcpy( (ULONG*)buf, (VOID*)IO_RELC.RELC_INPUT_BUF_DATA, 8192 );

	Ddim_Print(("\n RELC Input data Buffer        [2k byte] Base Address + 1800h"));
	Ddim_Print(("\n OFFSET    ADDRESS   0 1 2 3   4 5 6 7   8 9 A B   C D E F  "));
	Ddim_Print(("\n ---------+---------+---------+---------+---------+---------+"));

	for ( ptr = (LONG*)buf, ofs = 0, adr = addr ; adr < addr + 0x800 ; ptr+=16, ofs+=16, adr+=16 ) {
		put_dump((ULONG*)ptr, ofs, adr, 4);
	}

	// RELC Reference Data Buffer    [8k byte] Base Address + 2000h
	addr = 0x3A002000;
	memcpy( (ULONG*)buf, (VOID*)IO_RELC.RELC_REF_BUF_DATA, 8192 );

	Ddim_Print(("\n RELC Reference Data Buffer    [8k byte] Base Address + 2000h"));
	Ddim_Print(("\n OFFSET    ADDRESS   0 1 2 3   4 5 6 7   8 9 A B   C D E F  "));
	Ddim_Print(("\n ---------+---------+---------+---------+---------+---------+"));

	for ( ptr = (LONG*)buf, ofs = 0, adr = addr ; adr < addr + 0x2000 ; ptr+=16, ofs+=16, adr+=16 ) {
		put_dump( (ULONG*)ptr, ofs, adr, 4 );
	}
#endif
}

static VOID ct_exs_register_dump( VOID )
{
#ifdef PC_DEBUG
	Ddim_Print(("EXS REGISTER INFO.\n"));
	Ddim_Print(("IO_EXSTOP.RLC_HPT_CTL.bit.RERHPT = %d\n", IO_EXSTOP.RLC_HPT_CTL.bit.RERHPT));
	Ddim_Print(("IO_EXSTOP.RLC_HPT_CTL.bit.REWHPT = %d\n", IO_EXSTOP.RLC_HPT_CTL.bit.REWHPT));
#endif
}

#if 0
static VOID ct_clkstop_register_dump( VOID )
{
	Ddim_Print(("CLKSTOP REGISTER INFO.\n"));
	Ddim_Print(("IO_CHIPTOP.CLKSTOP_B.bit.RELCAH = %d\n", IO_CHIPTOP.CLKSTOP_B.bit.RELCAH));
}
#endif
/*----------------------------------------------------------------------*/
/* CallBack Function													*/
/*----------------------------------------------------------------------*/
static VOID cmd_ct_relc_callback( UINT32 int_factor )
{
	switch ( int_factor ) {
		case D_DD_RELC_ERR:
			/* Error interrupt */
			Ddim_Print(("[RELC INT] Error interrupt \n"));
			break;

		case D_DD_RELC_SLEEP:
			/* Sleep interrupt */
			Ddim_Print(("[RELC INT] Sleep interrupt \n"));
			break;

		case D_DD_RELC_FINISH:
			/* Finish interrupt */
			Ddim_Print(("[RELC INT] Finish interrupt \n"));
			break;

		case D_DD_RELC_CR_FINISH:
			/* Continuous run finish interrupt */
			Ddim_Print(("[RELC INT] Continuous run finish interrupt \n"));
			break;

		default:
			/* Unknown factor */
			Ddim_Print(("[RELC INT] Unknown factor. factor=%d \n", int_factor));
			break;
	}
}

/*----------------------------------------------------------------------*/
/* CallBack Function													*/
/*----------------------------------------------------------------------*/
/*
 * sample command help
 */

VOID ct_relc_help()
{
	Ddim_Print(("--- RELC --- (This command help for real machine test)\n"));
	Ddim_Print(("USAGE>relc init write_hprot read_hprot\n"));
	Ddim_Print(("                    # Initialize RELC macro.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc open\n"));
	Ddim_Print(("                    # Open RELC macro.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc close\n"));
	Ddim_Print(("                    # Close RELC macro.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc start [sync/async]\n"));
	Ddim_Print(("                    # Start RELC macro.\n"));
	Ddim_Print(("                    #   [sync/async] sync : Synchronization of RELC macro start\n"));
	Ddim_Print(("                    #                async: Asynchronous of RELC macro start\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc stop\n"));
	Ddim_Print(("                    # Stop RELC macro.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc ctrlcomm [reg/desc] [nmod/cmod] [seq_num]\n"));
	Ddim_Print(("                    # Set control common data.\n"));
	Ddim_Print(("                    #   [reg/des]          reg  : register \n"));
	Ddim_Print(("                    #                      desc : Output disable\n"));
	Ddim_Print(("                    #   [nmod/cmod]        nmod : Normal mode\n"));
	Ddim_Print(("                    #                      smod : Continuous mode\n"));
	Ddim_Print(("                    #   [seq_num]          Sequential run number of times\n"));
	Ddim_Print(("                    #                      When 0 or 1 is specified, a value is 1.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc ctrlreg [imaxd/imaxe] [omaxd/omaxe] [in_sta_addr] [in_end_addr] [out_sta_addr] [out_end_addr]\n"));
	Ddim_Print(("                    # Set control register data.\n"));
	Ddim_Print(("                    #   [imaxd/imaxe]      imaxd : Input end address disable\n"));
	Ddim_Print(("                    #                      imaxe : Input end address enable\n"));
	Ddim_Print(("                    #   [omaxd/omaxe]      omaxd : Output end address disable\n"));
	Ddim_Print(("                    #                      omaxe : Output end address enable\n"));
	Ddim_Print(("                    #   [in_sta_addr]      Input start address\n"));
	Ddim_Print(("                    #   [out_sta_addr]     Output start address\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc ctrldesc [desc_addr]\n"));
	Ddim_Print(("                    # Set control descriptor data.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc set in_sta_addr [in_sta_addr]\n"));
	Ddim_Print(("                    # Set control input start address.\n"));
	Ddim_Print(("                    #   [in_sta_addr]       Input start address\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc set in_end_addr [in_end_addr]\n"));
	Ddim_Print(("                    # Set control input end address.\n"));
	Ddim_Print(("                    #   [in_end_addr]       Input end address\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc set out_sta_addr [out_sta_addr]\n"));
	Ddim_Print(("                    # Set control output start address.\n"));
	Ddim_Print(("                    #   [out_sta_addr]      Output start address\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc set out_end_addr [out_end_addr]\n"));
	Ddim_Print(("                    # Set control output end address.\n"));
	Ddim_Print(("                    #   [out_end_addr]      Output end address\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get ctrlcomm\n"));
	Ddim_Print(("                    # Get control common data.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get ctrlreg\n"));
	Ddim_Print(("                    # Get control register data.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get ctrldesc\n"));
	Ddim_Print(("                    # Get control descriptor data.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get msta\n"));
	Ddim_Print(("                    # Get RELC macro statusa.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get sleep\n"));
	Ddim_Print(("                    # Get RELC sleep status.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get errsta\n"));
	Ddim_Print(("                    # Get RELC error status.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get err\n"));
	Ddim_Print(("                    # Get RELC error number.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get psta\n"));
	Ddim_Print(("                    # Get RELC process status.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get bufsiz\n"));
	Ddim_Print(("                    # Get RELC buffer size.\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc set [inbuf/inbufm/refbuf] [in_addr] [in_size] [out_addr\n"));
	Ddim_Print(("                    # Set data to RELC buffer .\n"));
	Ddim_Print(("                    #   [inbuf/inbufm/refbuf]  inbuf  : input data buffer\n"));
	Ddim_Print(("                    #                          inbufm : input data buffer(mirror erea)\n"));
	Ddim_Print(("                    #                          refbuf : reference data buffer\n"));
	Ddim_Print(("                    #   [in_addr]              input data address on RAM\n"));
	Ddim_Print(("                    #   [in_size]              input data size on RAM\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc get [inbuf/inbufm/refbuf] [out_addr] [out_size]\n"));
	Ddim_Print(("                    # Get data from RELC buffer .\n"));
	Ddim_Print(("                    #   [inbuf/inbufm/refbuf]  inbuf  : input data buffer\n"));
	Ddim_Print(("                    #                          inbufm : input data buffer(mirror erea)\n"));
	Ddim_Print(("                    #                          refbuf : reference data buffer\n"));
	Ddim_Print(("                    #   [out_addr]             outputdata address from input data buffer to SDRAM\n"));
	Ddim_Print(("                    #   [out_size]             output data size\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc norm async [test_num]\n"));
	Ddim_Print(("                    # Set and Start RELC normal mode.(Async type)\n"));
	Ddim_Print(("                    #   [test_num]             test pattern number\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc desc async [test_num]\n"));
	Ddim_Print(("                    # Set and Start RELC descriptor mode.(Async type)\n"));
	Ddim_Print(("                    #   [test_num]            test pattern number\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc norm sync [test_num]\n"));
	Ddim_Print(("                    # Set and Start RELC normal mode.(Sync type)\n"));
	Ddim_Print(("                    #   [test_num]             test pattern number\n"));
	Ddim_Print(("\n"));
	Ddim_Print(("USAGE>relc desc sync [test_num]\n"));
	Ddim_Print(("                    # Set and Start RELC descriptor mode.(Sync type)\n"));
	Ddim_Print(("                    #   [test_num]            test pattern number\n"));
	Ddim_Print(("\n"));
}

/**
 * @brief  Command main function for RELC test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| P1            | P2               | P3            | P4            | P5            | P6            | P7            | Meaning                                     |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| init          | write_hprot      | read_hprot    |               |               |               |               | Initialize RELC macro.                      |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Init                              |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| open          |                  |               |               |               |               |               | Open RELC macro.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Open                              |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| close         |                  |               |               |               |               |               | Close RELC macro.                           |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Close                             |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| start         | sync             |               |               |               |               |               | Start RELC macro.                           |
 *	|               | async            |               |               |               |               |               |  sync : Synchronization of RELC macro start |
 *	|               |                  |               |               |               |               |               |  async: Asynchronous of RELC macro start    |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Start_Sync                        |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Start_Async                       |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| stop          |                  |               |               |               |               |               | Stop RELC macro.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Stop                              |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| ctrlcomm      | reg              | nmod          | seq_num       |               |               |               | Set control common data.                    |
 *	|               | desc             | cmod          |               |               |               |               |  reg  : register                            |
 *	|               |                  |               |               |               |               |               |  desc : Output disable                      |
 *	|               |                  |               |               |               |               |               |  nmod : Normal mode                         |
 *	|               |                  |               |               |               |               |               |  smod : Continuous mode                     |
 *	|               |                  |               |               |               |               |               |  seq_num : Sequential run number of times   |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Common                       |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| ctrlreg       | imaxd            | omaxd         | in_sta_addr   | in_end_addr   | out_sta_addr  | out_end_addr  | Set control register data.                  |
 *	|               | imaxe            | omaxe         |               |               |               |               |  imaxd : Input end address disable          |
 *	|               |                  |               |               |               |               |               |  imaxe : Input end address enable           |
 *	|               |                  |               |               |               |               |               |  omaxd : Output end address disable         |
 *	|               |                  |               |               |               |               |               |  omaxe : Output end address enable          |
 *	|               |                  |               |               |               |               |               |  Input start address                        |
 *	|               |                  |               |               |               |               |               |  Input end address                          |
 *	|               |                  |               |               |               |               |               |  Output start address                       |
 *	|               |                  |               |               |               |               |               |  Output end address                         |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Register                     |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| ctrldesc      | desc_addr        |               |               |               |               |               | Set control descriptor data.                |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Descriptor                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| set           | in_sta_addr      | in_sta_addr   |               |               |               |               | Set control input start address.            |
 *	|               |                  |               |               |               |               |               |  in_sta_addr : Input start address          |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_Start_Addr                 |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| set           | in_end_addr      | in_end_addr   |               |               |               |               | Set control input end address.              |
 *	|               |                  |               |               |               |               |               |  in_end_addr : Input end address            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_End_Addr                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| set           | out_sta_addr     | out_sta_addr  |               |               |               |               | Set control output start address.           |
 *	|               |                  |               |               |               |               |               |  out_sta_addr : Output start address        |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_Out_Start_Addr                |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| set           | out_end_addr     | out_end_addr  |               |               |               |               | Set control output end address.             |
 *	|               |                  |               |               |               |               |               |  out_end_addr : Output end address          |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_Out_End_Addr                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | ctrlcomm         |               |               |               |               |               | Get control common data.                    |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Common                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | ctrlreg          |               |               |               |               |               | Get control register data.                  |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Register                 |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | ctrldesc         |               |               |               |               |               | Get control descriptor data.                |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Descriptor               |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | msta             |               |               |               |               |               | Get RELC macro status.                      |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Status                        |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | sleep            |               |               |               |               |               | Get RELC sleep status.                      |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Sleep_Reason                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | errsta           |               |               |               |               |               | Get RELC error status.                      |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Error_Status                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | err              |               |               |               |               |               | Get RELC error number.                      |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Error                         |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | psta             |               |               |               |               |               | Get RELC process status.                    |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Process_Status                |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | bufsiz           |               |               |               |               |               | Get RELC buffer size.                       |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Buf_Size                      |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| set           | inbuf            | in_addr       | in_size       |               |               |               | Set data to RELC buffer.                    |
 *	|               | inbufm           |               |               |               |               |               |  inbuf  : input data buffer                 |
 *	|               | refbuf           |               |               |               |               |               |  inbufm : input data buffer(mirror erea)    |
 *	|               |                  |               |               |               |               |               |  refbuf : reference data buffer             |
 *	|               |                  |               |               |               |               |               |  in_addr : input data address on RAM        |
 *	|               |                  |               |               |               |               |               |  in_size : input data size on RAM           |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_Buf_Data                   |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_Buf_Data_Mirror            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_Ref_Buf_Data                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| get           | inbuf            | out_addr      | out_size      |               |               |               | Get data from RELC buffer.                  |
 *	|               | inbufm           |               |               |               |               |               |  inbuf  : input data buffer                 |
 *	|               | refbuf           |               |               |               |               |               |  inbufm : input data buffer(mirror erea)    |
 *	|               |                  |               |               |               |               |               |  refbuf : reference data buffer             |
 *	|               |                  |               |               |               |               |               |  out_addr : outputdata address from         |
 *	|               |                  |               |               |               |               |               |             input data buffer to SDRAM      |
 *	|               |                  |               |               |               |               |               |  out_size : output data size                |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_In_Buf_Data                   |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_In_Buf_Data_Mirror            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ref_Buf_Data                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| norm          | async            | test_num      |               |               |               |               | Set and Start RELC normal mode.             |
 *	|               |                  |               |               |               |               |               |  test_num : test pattern number             |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Utility_Register                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| desc          | async            | test_num      |               |               |               |               | Set and Start RELC descriptor mode.         |
 *	|               |                  |               |               |               |               |               |  test_num : test pattern number             |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Utility_Descriptor                |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| norm          | sync             | test_num      |               |               |               |               | Set and Start RELC normal mode.             |
 *	|               |                  |               |               |               |               |               |  test_num : test pattern number             |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Init                              |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Open                              |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Common                       |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Common                   |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Register                     |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Register                 |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Start_Sync                        |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Status                        |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Process_Status                |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Error_Status                  |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Error                         |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Wait_End                          |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Close                             |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| desc          | sync             | test_num      |               |               |               |               | Set and Start RELC descriptor mode.         |
 *	|               |                  |               |               |               |               |               |  test_num : test pattern number             |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Init                              |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Open                              |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Common                       |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Common                   |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Descriptor                   |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Descriptor               |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Status                        |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Process_Status                |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Error_Status                  |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Error                         |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Start_Sync                        |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Wait_End                          |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Close"                            |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| inthdr        | test_num         |               |               |               |               |               | Interrupt Handler.                          |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Int_Handler                       |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | open             |               |               |               |               |               | Open error.                                 |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Open                              |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | ctrlcomm         |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Common                       |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | ctrlreg          |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Register                     |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | ctrldesc         |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Ctrl_Descriptor                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_in_sta_addr  |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_Start_Addr                 |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_in_end_addr  |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_End_Addr                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_out_sta_addr |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_Out_Start_Addr                |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_out_end_addr |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_Out_End_Addr                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_inbuf        |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_Buf_Data                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_inbufm       |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_In_Buf_Data_Mirror            |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | set_refbuf       |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Set_Ref_Buf_Data                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_ctrlcomm     |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Common                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_ctrlreg      |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Register                 |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_ctrldesc     |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ctrl_Descriptor               |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_psta         |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Process_Status                |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_bufsize1     |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Buf_Size                      |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_bufsize2     |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Buf_Size                      |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_inbuf        |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_In_Buf_Data                   |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_inbufm       |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_In_Buf_Data_Mirror            |
 *	+---------------+------------------+---------------+---------------+---------------+---------------+---------------+---------------------------------------------+
 *	| err           | get_refbuf       |               |               |               |               |               | Parameter error.                            |
 *	|               |                  |               |               |               |               |               | >>Dd_RELC_Get_Ref_Buf_Data                  |
 *	+---------------+------------------+---------------+---------------+---------------+---------------;---------------+---------------------------------------------+
*/

/**
 * @brief  Command main function for RELC macro test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 * @return VOID
 */
VOID Ct_Dd_RELC_Main( int argc, char** argv )
{
	INT32						ret = D_DD_RELC_OK;
	T_DD_RELC_CTRL_CMN			ctrl_com;
	T_DD_RELC_CTRL_REG			ctrl_reg;
	T_DD_RELC_CTRL_DESC			ctrl_desc;
	T_DD_RELC_SET_MOD_NORMAL	set_mod_norm;
	T_DD_RELC_SET_MOD_DESC		set_mod_desc;
	T_DD_RELC_DEC_INFO			dec_info;
	ULONG						set_addr;
	UINT32						set_size = 0;
	USHORT						in_buf_size;
	USHORT						ref_buf_size = 0;
	UCHAR						test_num;
	char*						endstr;
	char*						hprot;
	ULONG						write_hprot;
	ULONG						read_hprot;
#ifdef PC_DEBUG
	ULONG						test_pat;
#endif

	if ( strcmp( argv[ 1 ], "init" ) == 0 ) {
		/* Initialize RELC */
		write_hprot	= strtoul( argv[ 2 ], &hprot, 16 );
		read_hprot	= strtoul( argv[ 3 ], &hprot, 16 );

		Dd_RELC_Init( (UCHAR)write_hprot, (UCHAR)read_hprot );		/* pgr0539 */
		Ddim_Print(("RELC init OK\n"));

		ct_relc_register_dump();
		ct_exs_register_dump();
	}
	else if ( strcmp(argv[ 1 ], "open" ) == 0 ) {
		/* Open RELC */
		ret = Dd_RELC_Open();

		if ( ret != D_DD_RELC_OK ) {
			Ddim_Print(("RELC open ERR. ret=0x%x\n", ret));
		}
		else {
			Ddim_Print(("RELC open OK\n"));
		}
	}
	else if ( strcmp( argv[ 1 ], "close" ) == 0 ) {
		/* Close RELC */
		Dd_RELC_Close();
		Ddim_Print(("RELC close OK\n"));
	}
	else if ( strcmp( argv[ 1 ], "start" ) == 0 ) {
		/* Synchronous start of RELC */
		if ( strcmp( argv[ 2 ], "sync" ) == 0 ) {
			ret = Dd_RELC_Start_Sync();

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Synchronous RELC start ERR. ret=0x%x\n", ret));
			}
			else {
				Ddim_Print(("Synchronous RELC start OK\n"));
			}
		}
		/* Asynchronous start of RELC */
		else if ( strcmp( argv[ 2 ], "async" ) == 0 ) {
			Dd_RELC_Start_Async();
		}
		else {
		}

		ct_relc_register_dump();
	}
	else if( strcmp( argv[ 1 ], "stop" ) == 0 ) {
		/* Stop RELC */
#ifdef PC_DEBUG
		IO_RELC.RELC_MODE1.bit.PRUN = 1;
#endif
		Dd_RELC_Stop();
		Ddim_Print(("RELC stop OK\n"));

		ct_relc_register_dump();
	}
	else if ( strcmp( argv[ 1 ], "ctrlcomm" ) == 0 ) {
		/* Control common of RELC */
		if ( strcmp( argv[ 2 ], "reg" ) == 0 ) {			/* Normal mode(register) */
			ctrl_com.desc_mode = D_DD_RELC_RUN_MODE_NORMAL;
		}
		else if ( strcmp( argv[ 2 ], "desc" ) == 0 ) {		/* Descriptor mode */
			ctrl_com.desc_mode = D_DD_RELC_RUN_MODE_DESC;
		}
		else {
			ctrl_com.desc_mode = D_DD_RELC_RUN_MODE_NORMAL;
		}

		if ( strcmp( argv[ 3 ], "nmod" ) == 0 ) {			/* Normal mode(once) */
			ctrl_com.cont_run_mode = D_DD_RELC_CONT_MODE_NORMAL;
			gRELC_Cont_Mode = 0;
		}
		else if ( strcmp( argv[ 3 ], "cmod" ) == 0 ) {		/* Continuous Mode */
			ctrl_com.cont_run_mode = D_DD_RELC_CONT_MODE_CONT;
			gRELC_Cont_Mode = 1;
		}
		else {
			ctrl_com.cont_run_mode = D_DD_RELC_CONT_MODE_NORMAL;
			gRELC_Cont_Mode = 0;
		}

		ctrl_com.seq_num	= (ULONG)atoi( argv[ 4 ] );
		ctrl_com.callback 	= (RELK_CALLBACK)cmd_ct_relc_callback;

		ret = Dd_RELC_Ctrl_Common( &ctrl_com );
		if ( ret != D_DD_RELC_OK ) {
			Ddim_Print(("Execution common control setting error. ret=%d\n", ret));
			return;
		}

		ct_relc_register_dump();
	}
	else if ( strcmp( argv[ 1 ], "ctrlreg" ) == 0 ) {
		/* Control register of RELC */
		if ( strcmp( argv[ 2 ], "imaxd" ) == 0 ) {		/* Input end address disable */
			ctrl_reg.in_end_addr_en = D_DD_RELC_ADDR_DISABLE;
		}
		else if ( strcmp( argv[ 2 ], "imaxe" ) == 0 ) {	/* Input end address enable */
			ctrl_reg.in_end_addr_en = D_DD_RELC_ADDR_ENABLE;
		}
		else {
			ctrl_reg.in_end_addr_en = D_DD_RELC_ADDR_DISABLE;
		}

		if ( strcmp( argv[ 3 ], "omaxd" ) == 0 ) {		/* Output end address disable */
			ctrl_reg.out_end_addr_en = D_DD_RELC_ADDR_DISABLE;
		}
		else if ( strcmp( argv[ 3 ], "omaxe" ) == 0 ) {	/* Output end address disable */
			ctrl_reg.out_end_addr_en = D_DD_RELC_ADDR_ENABLE;
		}
		else {
			ctrl_reg.out_end_addr_en = D_DD_RELC_ADDR_DISABLE;
		}

		if ( gRELC_Cont_Mode != 0 ) {				/* Address auto increment on */
			ctrl_reg.in_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_ON;
			ctrl_reg.out_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_ON;
		}
		else {										/* Address auto increment off */
			ctrl_reg.in_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_OFF;
			ctrl_reg.out_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_OFF;
		}

		ctrl_reg.in_start_addr	= strtoul( argv[ 4 ], &endstr, 16 );
		ctrl_reg.in_end_addr	= strtoul( argv[ 5 ], &endstr, 16 );
		ctrl_reg.out_start_addr	= strtoul( argv[ 6 ], &endstr, 16 );
		ctrl_reg.out_end_addr	= strtoul( argv[ 7 ], &endstr, 16 );

#ifdef PC_DEBUG
		if ( ctrl_reg.in_end_addr_en == D_DD_RELC_ADDR_DISABLE ) {
			IO_RELC.RELC_MODE2.bit.IMAX = 1;
			IO_RELC.RELC_MODE2.bit.OMAX = 1;
			IO_RELC.RELC_MODE2.bit.IAAI = 1;
			IO_RELC.RELC_MODE2.bit.OAAI = 1;
		}
		else {
			IO_RELC.RELC_MODE2.bit.IMAX = 0;
			IO_RELC.RELC_MODE2.bit.OMAX = 0;
			IO_RELC.RELC_MODE2.bit.IAAI = 1;
			IO_RELC.RELC_MODE2.bit.OAAI = 1;
		}
#endif

		ret = Dd_RELC_Ctrl_Register( &ctrl_reg );

		if ( ret != D_DD_RELC_OK ) {
			Ddim_Print(("Execution register control setting error. ret=%d\n", ret));
			return;
		}

		ct_relc_register_dump();
	}
	else if ( strcmp(argv[ 1 ], "ctrldesc" ) == 0 ) {
		/* Control descriptor of RELC */
		ctrl_desc.descriptor_addr = strtoul( argv[ 2 ], &endstr, 16 );

		ret = Dd_RELC_Ctrl_Descriptor( &ctrl_desc );

		if ( ret != D_DD_RELC_OK ) {
			Ddim_Print(("Execution descriptor control setting error. ret=%d\n", ret));
			return;
		}

		ct_relc_register_dump();
	}
	else if ( strcmp( argv[ 1 ], "set" ) == 0 ) {

		if ( strcmp( argv[ 2 ], "in_sta_addr" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			ret = Dd_RELC_Set_In_Start_Addr( set_addr );	/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input start address setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "in_end_addr" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			ret = Dd_RELC_Set_In_End_Addr( set_addr );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input end address setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "out_sta_addr" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			ret = Dd_RELC_Set_Out_Start_Addr( set_addr );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution output start address setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "out_end_addr" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			ret = Dd_RELC_Set_Out_End_Addr( set_addr );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution output end address setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "inbuf" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			set_size = (UINT32)atoi( (const char *)argv[ 4 ] );
			ret = Dd_RELC_Set_In_Buf_Data( set_addr, set_size );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input buffer setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "inbufm" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			set_size = (UINT32)atoi( (const char *)argv[ 4 ] );
			ret = Dd_RELC_Set_In_Buf_Data_Mirror( set_addr, set_size );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input buffer(mirror) setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "refbuf" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			set_size = (UINT32)atoi( (const char *)argv[ 4 ] );
			ret = Dd_RELC_Set_Ref_Buf_Data( set_addr, set_size );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution reference buffer setting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
		}
	}
	else if ( strcmp( argv[ 1 ], "get" ) == 0 ) {
		if ( strcmp( argv[ 2 ], "ctrlcomm" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_MODE1.bit.DSMD	= 1;
			IO_RELC.RELC_MODE1.bit.CRMD	= 1;
			IO_RELC.RELC_SEQ_NUM		= 726;
#endif
			ret = Dd_RELC_Get_Ctrl_Common( &ctrl_com );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
				return;
			}

			Ddim_Print(("Common control information\n"));
			Ddim_Print(("# Descriptor Mode = %d\n", ctrl_com.desc_mode));
			Ddim_Print(("# Continuous Run Mode = %d\n", ctrl_com.cont_run_mode));
			Ddim_Print(("# Sequential Run Number of Times = %lu\n", ctrl_com.seq_num));

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "ctrlreg" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_MODE2.bit.IMAX	= 1;
			IO_RELC.RELC_MODE2.bit.OMAX	= 1;
			IO_RELC.RELC_MODE2.bit.IAAI	= 1;
			IO_RELC.RELC_MODE2.bit.OAAI	= 1;
			IO_RELC.RELC_IN_START_ADDR	= 0x41012560;
			IO_RELC.RELC_IN_END_ADDR	= 0x41024568;
			IO_RELC.RELC_OUT_START_ADDR	= 0x42302530;
			IO_RELC.RELC_OUT_END_ADDR	= 0x44332020;
#endif
			ret = Dd_RELC_Get_Ctrl_Register( &ctrl_reg );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution register control getting error. ret=%d\n", ret));
				return;
			}

			Ddim_Print(("Register control information\n"));
			Ddim_Print(("# Input End Address Enable = %d\n", ctrl_reg.in_end_addr_en));
			Ddim_Print(("# Output End Address Enable = %d\n", ctrl_reg.out_end_addr_en));
			Ddim_Print(("# Input Address Auto Increment = %d\n", ctrl_reg.in_addr_auto_inc));
			Ddim_Print(("# Output Address Auto Increment = %d\n", ctrl_reg.out_addr_auto_inc));
			Ddim_Print(("# Input start address = 0x%lx\n", ctrl_reg.in_start_addr));
			Ddim_Print(("# Input end address = 0x%lx\n", ctrl_reg.in_end_addr));
			Ddim_Print(("# Output start address = 0x%lx\n", ctrl_reg.out_start_addr));
			Ddim_Print(("# Output end address = 0x%lx\n", ctrl_reg.out_end_addr));

			ct_relc_register_dump();
		}
		else if ( strcmp(argv[ 2 ], "ctrldesc" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_DESCRIPTOR_ADDR = 0x44802210;
#endif
			ret = Dd_RELC_Get_Ctrl_Descriptor( &ctrl_desc );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution descriptor control getting error. ret=0x%x\n", ret));
				return;
			}

			Ddim_Print(("Discriptor control information\n"));
			Ddim_Print(("# Descriptor address = 0x%lx\n", ctrl_desc.descriptor_addr));

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "msta" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_STATUS.word = 4;
#endif
			ret = Dd_RELC_Get_Status();
			Ddim_Print(("RELC decode status\n"));
			Ddim_Print(("# RELC macro status = 0x%x\n", ret));

			ct_relc_register_dump();
		}
		else if ( strcmp(argv[ 2 ], "sleep" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_STATUS.bit.SRES = 2;
#endif
			ret = Dd_RELC_Get_Sleep_Reason();
			Ddim_Print(("RELC sleep reason\n"));
			Ddim_Print(("# RELC sleep reason = 0x%x\n", ret));

			ct_relc_register_dump();
		}
		else if ( strcmp(argv[ 2 ], "errsta") == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_STATUS.bit.EFLG = 1;
#endif
			ret = Dd_RELC_Get_Error_Status();

			Ddim_Print(("RELC error status information\n"));
			Ddim_Print(("# RELC error = 0x%x\n", ret));

			ct_relc_register_dump();
		}
		else if ( strcmp(argv[ 2 ], "err") == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_ERROR_NUM.word = 0x00008234;
#endif
			ret = Dd_RELC_Get_Error();

			Ddim_Print(("RELC error information\n"));
			Ddim_Print(("# RELC error = 0x%x\n", ret));

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "psta" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_READ_BYTE.bit.DRNM		= 1234;
			IO_RELC.RELC_WRITE_BYTE.bit.DWNM	= 2345;
			IO_RELC.RELC_BLOCK.bit.BLKN			= 3456;
			IO_RELC.RELC_INPUT_BYTE				= 876;
			IO_RELC.RELC_OUTPUT_BYTE			= 985;
			IO_RELC.RELC_SEQ_CNT				= 65;
#endif
			ret = Dd_RELC_Get_Process_Status( &dec_info );
			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC processing status getting error. ret=0x%x\n", ret));
				return;
			}

			Ddim_Print(("RELC processing status\n"));
			Ddim_Print(("# Number of bytes read = %d\n", dec_info.read_byte));
			Ddim_Print(("# Number of bytes written = %d\n", dec_info.write_byte));
			Ddim_Print(("# Number of processing blocks = %d\n", dec_info.proc_block));
			Ddim_Print(("# Decode the total number of bytes to read = %lu\n", dec_info.dec_total_read_bytes));
			Ddim_Print(("# Decode the total number of bytes to write = %lu\n", dec_info.dec_total_write_bytes));
			Ddim_Print(("# Sequential run counter = %lu\n", dec_info.seq_cnt));

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "bufsize" ) == 0 ) {
#ifdef PC_DEBUG
			IO_RELC.RELC_BUF_SIZE.bit.IBFS = 7;
			IO_RELC.RELC_BUF_SIZE.bit.RBFS = 1;
#endif
			ret = Dd_RELC_Get_Buf_Size( &in_buf_size, &ref_buf_size );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC buffer size getting error. ret=0x%x\n", ret));
				return;
			}

			Ddim_Print(("RELC buffer size information\n"));
			Ddim_Print(("# RELC input buffer size = %d\n", in_buf_size));
			Ddim_Print(("# RELC reference buffer size = %d\n", ref_buf_size));

			ct_relc_register_dump();
		}
		else if ( strcmp( argv[ 2 ], "inbuf" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			set_size = (UINT32)atoi( (const char *)argv[ 4 ] );
			ret = Dd_RELC_Get_In_Buf_Data( set_addr, set_size );		/* pgr0539 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC input buffer data getting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
			ct_relc_buffer_dump();
		}
		else if ( strcmp( argv[ 2 ], "inbufm" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			set_size = (UINT32)atoi( (const char *)argv[ 4 ] );
			ret = Dd_RELC_Get_In_Buf_Data_Mirror( set_addr, set_size );		/* pgr0539 */

			if( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC input data buffer getting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
			ct_relc_buffer_dump();
		}
		else if ( strcmp( argv[ 2 ], "refbuf" ) == 0 ) {

			set_addr = strtoul( argv[ 3 ], &endstr, 16 );
			set_size = (UINT32)atoi( (const char *)argv[ 4 ] );
			ret = Dd_RELC_Get_Ref_Buf_Data( set_addr, set_size );		/* pgr0539 */

			if( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC reference data buffer getting error. ret=%d\n", ret));
				return;
			}

			ct_relc_register_dump();
			ct_relc_buffer_dump();
		}
	}
	else if ( strcmp( argv[ 1 ], "norm" ) == 0 ) {
		if ( strcmp( argv[ 2 ], "async" ) == 0 ) {

			test_num = (UCHAR)atoi( (const char *)argv[ 3 ] );
			memset( &set_mod_norm, 0, sizeof(T_DD_RELC_SET_MOD_NORMAL) );

			if ( test_num == 1 ) {		/* Normal mode */
				set_mod_norm.write_hprot	= 11;
				set_mod_norm.read_hprot		= 11;
				set_mod_norm.in_start_addr	= 0x58100000;
				set_mod_norm.out_start_addr	= 0x58200000;
				set_mod_norm.seq_num = 1;
				set_mod_norm.callback = (RELK_CALLBACK)cmd_ct_relc_callback;

				ret = Dd_RELC_Utility_Register( &set_mod_norm );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Set and Start RELC normal mode error. ret=%d\n", ret));
					return;
				}

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
			else if ( test_num == 2 ) {		/* Continuous run mode */
				set_mod_norm.write_hprot	= 11;
				set_mod_norm.read_hprot		= 11;
				set_mod_norm.in_start_addr	= 0x58100000;
				set_mod_norm.out_start_addr	= 0x58200000;
				set_mod_norm.seq_num		= 3;
				set_mod_norm.callback		= (RELK_CALLBACK)cmd_ct_relc_callback;

				ret = Dd_RELC_Utility_Register( &set_mod_norm );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Set and Start RELC normal mode error. ret=%d\n", ret));
					return;
				}

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
		}
		else if ( strcmp( argv[ 2 ], "sync" ) == 0 ) {
			test_num = (UCHAR)atoi( (const char *)argv[ 3 ] );

			if ( test_num == 1) {		/* Normal mode */
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				memset( &ctrl_reg, 0, sizeof(T_DD_RELC_CTRL_REG) );

				ctrl_com.write_hprot	= 11;
				ctrl_com.read_hprot		= 11;

				// Initialize RELC macro.
				Dd_RELC_Init( ctrl_com.write_hprot, ctrl_com.read_hprot );	/* After power on and the reset, this is executed only once. */
				Ddim_Print(("RELC init OK\n"));

				// Open RELC macro.
				ret = Dd_RELC_Open();

				if ( ret != D_DDIM_OK ) {
					Ddim_Print( ("Error RELC Open !!\n" ) );
					return;
				}

				ctrl_com.desc_mode			= D_DD_RELC_RUN_MODE_NORMAL;
				ctrl_com.callback			= (RELK_CALLBACK)cmd_ct_relc_callback;
				ctrl_com.seq_num			= 1;		/* Normal mode */
				ctrl_com.cont_run_mode		= D_DD_RELC_CONT_MODE_NORMAL;
				ctrl_reg.in_end_addr_en		= D_DD_RELC_ADDR_ENABLE;
				ctrl_reg.out_end_addr_en	= D_DD_RELC_ADDR_ENABLE;
				ctrl_reg.in_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_OFF;
				ctrl_reg.out_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_OFF;
				ctrl_reg.in_start_addr		= 0x58100000;
				ctrl_reg.in_end_addr		= 0x581166B5;
				ctrl_reg.out_start_addr		= 0x58200000;
				ctrl_reg.out_end_addr		= 0x58218A5C;

				// Set control common data.
				ret = Dd_RELC_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution common control setting error. ret=%d\n", ret));
					return;
				}

				// Get control common data.
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				ret = Dd_RELC_Get_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
					return;
				}

				Ddim_Print(("Common control information\n"));
				Ddim_Print(("# Descriptor Mode = %d\n", ctrl_com.desc_mode));
				Ddim_Print(("# Continuous Run Mode = %d\n", ctrl_com.cont_run_mode));
				Ddim_Print(("# Sequential Run Number of Times = %lu\n", ctrl_com.seq_num));

				// Set control register data.
				ret = Dd_RELC_Ctrl_Register( &ctrl_reg );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution register control setting error. ret=%d\n", ret));
					return;
				}

				// Get control register data.
				memset( &ctrl_reg, 0, sizeof(T_DD_RELC_CTRL_REG) );
				ret = Dd_RELC_Get_Ctrl_Register( &ctrl_reg );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution register control getting error. ret=%d\n", ret));
					return;
				}

				Ddim_Print(("Register control information\n"));
				Ddim_Print(("# Input End Address Enable = %d\n", ctrl_reg.in_end_addr_en));
				Ddim_Print(("# Output End Address Enable = %d\n", ctrl_reg.out_end_addr_en));
				Ddim_Print(("# Input Address Auto Increment = %d\n", ctrl_reg.in_addr_auto_inc));
				Ddim_Print(("# Output Address Auto Increment = %d\n", ctrl_reg.out_addr_auto_inc));
				Ddim_Print(("# Input start address = 0x%lx\n", ctrl_reg.in_start_addr));
				Ddim_Print(("# Input end address = 0x%lx\n", ctrl_reg.in_end_addr));
				Ddim_Print(("# Output start address = 0x%lx\n", ctrl_reg.out_start_addr));
				Ddim_Print(("# Output end address = 0x%lx\n", ctrl_reg.out_end_addr));

				// Start RELC macro.(Synchronization of RELC macro start)
				ret = Dd_RELC_Start_Sync();

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Synchronous RELC start ERR. ret=0x%x\n", ret));
				}
				else {
					Ddim_Print(("Synchronous RELC start OK\n"));
				}

				// Get RELC macro status.
				ret = Dd_RELC_Get_Status();

				Ddim_Print(("RELC decode status\n"));
				Ddim_Print(("# RELC macro status = 0x%x\n", ret));

				// Get RELC process status.
				ret = Dd_RELC_Get_Process_Status( &dec_info );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution RELC processing status getting error. ret=0x%x\n", ret));
					return;
				}

				Ddim_Print(("RELC processing status\n"));
				Ddim_Print(("# Number of bytes read = %d\n", dec_info.read_byte));
				Ddim_Print(("# Number of bytes written = %d\n", dec_info.write_byte));
				Ddim_Print(("# Number of processing blocks = %d\n", dec_info.proc_block));
				Ddim_Print(("# Decode the total number of bytes to read = %lu\n", dec_info.dec_total_read_bytes));
				Ddim_Print(("# Decode the total number of bytes to write = %lu\n", dec_info.dec_total_write_bytes));
				Ddim_Print(("# Sequential run counter = %lu\n", dec_info.seq_cnt));

				// Get RELC error status.
				ret = Dd_RELC_Get_Error_Status();

				Ddim_Print(("RELC error status information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Get RELC error number. 
				ret = Dd_RELC_Get_Error();

				Ddim_Print(("RELC error information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Close RELC macro.
				Dd_RELC_Close();

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
			else if ( test_num == 2 ) {		/* Continuous run mode */

				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				memset( &ctrl_reg, 0, sizeof(T_DD_RELC_CTRL_REG) );

				ctrl_com.write_hprot	= 11;
				ctrl_com.read_hprot		= 11;

				// Initialize RELC macro.
				Dd_RELC_Init( ctrl_com.write_hprot, ctrl_com.read_hprot );	/* After power on and the reset, this is executed only once. */
				Ddim_Print(("RELC init OK\n"));

				// Open RELC macro.
				ret = Dd_RELC_Open();

				if ( ret != D_DDIM_OK ) {
					Ddim_Print( ("Error RELC Open !!\n" ) );
					return;
				}

				ctrl_com.desc_mode			= D_DD_RELC_RUN_MODE_NORMAL;
				ctrl_com.callback			= (RELK_CALLBACK)cmd_ct_relc_callback;
				ctrl_com.seq_num			= 3;		/* Continuous run mode */
				ctrl_com.cont_run_mode		= D_DD_RELC_CONT_MODE_CONT;
				ctrl_reg.in_end_addr_en		= D_DD_RELC_ADDR_DISABLE;
				ctrl_reg.out_end_addr_en	= D_DD_RELC_ADDR_DISABLE;
				ctrl_reg.in_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_ON;
				ctrl_reg.out_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_ON;
				ctrl_reg.in_start_addr		= 0x58100000;
				ctrl_reg.in_end_addr		= 0;
				ctrl_reg.out_start_addr		= 0x58200000;
				ctrl_reg.out_end_addr		= 0;

				// Set control common data.
				ret = Dd_RELC_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution common control setting error. ret=%d\n", ret));
					return;
				}

				// Get control common data.
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				ret = Dd_RELC_Get_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
					return;
				}

				Ddim_Print(("Common control information\n"));
				Ddim_Print(("# Descriptor Mode = %d\n", ctrl_com.desc_mode));
				Ddim_Print(("# Continuous Run Mode = %d\n", ctrl_com.cont_run_mode));
				Ddim_Print(("# Sequential Run Number of Times = %lu\n", ctrl_com.seq_num));

				// Set control register data.
				ret = Dd_RELC_Ctrl_Register( &ctrl_reg );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution register control setting error. ret=%d\n", ret));
					return;
				}

				// Get control register data.
				memset( &ctrl_reg, 0, sizeof(T_DD_RELC_CTRL_REG) );
				ret = Dd_RELC_Get_Ctrl_Register( &ctrl_reg );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution register control getting error. ret=%d\n", ret));
					return;
				}

				Ddim_Print(("Register control information\n"));
				Ddim_Print(("# Input End Address Enable = %d\n", ctrl_reg.in_end_addr_en));
				Ddim_Print(("# Output End Address Enable = %d\n", ctrl_reg.out_end_addr_en));
				Ddim_Print(("# Input Address Auto Increment = %d\n", ctrl_reg.in_addr_auto_inc));
				Ddim_Print(("# Output Address Auto Increment = %d\n", ctrl_reg.out_addr_auto_inc));
				Ddim_Print(("# Input start address = 0x%lx\n", ctrl_reg.in_start_addr));
				Ddim_Print(("# Input end address = 0x%lx\n", ctrl_reg.in_end_addr));
				Ddim_Print(("# Output start address = 0x%lx\n", ctrl_reg.out_start_addr));
				Ddim_Print(("# Output end address = 0x%lx\n", ctrl_reg.out_end_addr));

				// Start RELC macro.(Synchronization of RELC macro start)
				ret = Dd_RELC_Start_Sync();

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Synchronous RELC start ERR. ret=0x%x\n", ret));
				}
				else {
					Ddim_Print(("Synchronous RELC start OK\n"));
				}

				// Get RELC macro status.
				ret = Dd_RELC_Get_Status();

				Ddim_Print(("RELC decode status\n"));
				Ddim_Print(("# RELC macro status = 0x%x\n", ret));

				// Get RELC process status.
				ret = Dd_RELC_Get_Process_Status( &dec_info );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution RELC processing status getting error. ret=0x%x\n", ret));
					return;
				}

				Ddim_Print(("RELC processing status\n"));
				Ddim_Print(("# Number of bytes read = %d\n", dec_info.read_byte));
				Ddim_Print(("# Number of bytes written = %d\n", dec_info.write_byte));
				Ddim_Print(("# Number of processing blocks = %d\n", dec_info.proc_block));
				Ddim_Print(("# Decode the total number of bytes to read = %lu\n", dec_info.dec_total_read_bytes));
				Ddim_Print(("# Decode the total number of bytes to write = %lu\n", dec_info.dec_total_write_bytes));
				Ddim_Print(("# Sequential run counter = %lu\n", dec_info.seq_cnt));

				// Get RELC error status.
				ret = Dd_RELC_Get_Error_Status();

				Ddim_Print(("RELC error status information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Get RELC error number. 
				ret = Dd_RELC_Get_Error();

				Ddim_Print(("RELC error information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Close RELC macro.
				Dd_RELC_Close();

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
		}
	}
	else if ( strcmp( argv[ 1 ], "desc" ) == 0 ) {
		if ( strcmp( argv[ 2 ], "async" ) == 0 ) {

			test_num = (UCHAR)atoi( (const char *)argv[ 3 ] );
			memset( &set_mod_desc, 0, sizeof(T_DD_RELC_SET_MOD_DESC) );

			if ( test_num == 1 ) {		/* Normal mode */

				set_mod_desc.write_hprot			= 11;
				set_mod_desc.read_hprot				= 11;
				set_mod_desc.relc_descriptor_addr	= 0x58000000;
				set_mod_desc.seq_num				= 1;
				set_mod_desc.callback				= (RELK_CALLBACK)cmd_ct_relc_callback;

				memcpy( (UCHAR *)0x58000000, gRELC_Desc_Table1, sizeof(gRELC_Desc_Table1) );

				ret = Dd_RELC_Utility_Descriptor( &set_mod_desc );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Set and Start RELC descriptor mode error. ret=%d\n", ret));
					return;
				}

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
			else if ( test_num == 2 ) {		/* Continuous run mode */

				set_mod_desc.write_hprot			= 11;
				set_mod_desc.read_hprot				= 11;
				set_mod_desc.relc_descriptor_addr	= 0x58000000;
				set_mod_desc.seq_num				= 3;
				set_mod_desc.callback				= (RELK_CALLBACK)cmd_ct_relc_callback;

				memcpy( (UCHAR *)0x58000000, gRELC_Desc_Table1, sizeof(gRELC_Desc_Table1) );

				ret = Dd_RELC_Utility_Descriptor( &set_mod_desc );
				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Set and Start RELC descriptor mode error. ret=%d\n", ret));
					return;
				}

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
		}
		else if ( strcmp( argv[ 2 ], "sync" ) == 0 ) {

			test_num = (UCHAR)atoi( (const char *)argv[ 3 ] );

			if ( test_num == 1 ) {		/* Normal mode */

				memcpy( (UCHAR *)0x58000000, gRELC_Desc_Table1, sizeof(gRELC_Desc_Table1) );
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				memset( &ctrl_desc, 0, sizeof(T_DD_RELC_CTRL_DESC) );

				ctrl_com.write_hprot	= 11;
				ctrl_com.read_hprot		= 11;

				// Initialize RELC macro.
				Dd_RELC_Init( ctrl_com.write_hprot, ctrl_com.read_hprot );	/* After power on and the reset, this is executed only once. */
				Ddim_Print(("RELC init OK\n"));

				// Open RELC macro.
				ret = Dd_RELC_Open();

				if ( ret != D_DDIM_OK ) {
					Ddim_Print( ("Error RELC Open !!\n" ) );
					return;
				}

				ctrl_com.desc_mode			= D_DD_RELC_RUN_MODE_DESC;
				ctrl_com.callback			= (RELK_CALLBACK)cmd_ct_relc_callback;
				ctrl_com.seq_num			= 1;		/* Normal mode */
				ctrl_com.cont_run_mode		= D_DD_RELC_CONT_MODE_NORMAL;
				ctrl_desc.descriptor_addr	= 0x58000000;

				// Set control common data.
				ret = Dd_RELC_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution common control setting error. ret=%d\n", ret));
					return;
				}

				// Get control common data.
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				ret = Dd_RELC_Get_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
					return;
				}

				Ddim_Print(("Common control information\n"));
				Ddim_Print(("# Descriptor Mode = %d\n", ctrl_com.desc_mode));
				Ddim_Print(("# Continuous Run Mode = %d\n", ctrl_com.cont_run_mode));
				Ddim_Print(("# Sequential Run Number of Times = %lu\n", ctrl_com.seq_num));

				// Set control descriptor data.
				ret = Dd_RELC_Ctrl_Descriptor( &ctrl_desc );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution descriptor control setting error. ret=%d\n", ret));
					return;
				}

				// Get control descriptor data.
				ret = Dd_RELC_Get_Ctrl_Descriptor( &ctrl_desc );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution descriptor control getting error. ret=0x%x\n", ret));
					return;
				}

				Ddim_Print(("Discriptor control information\n"));
				Ddim_Print(("# Descriptor address = 0x%lx\n", ctrl_desc.descriptor_addr));

				// Start RELC macro.(Synchronization of RELC macro start)
				ret = Dd_RELC_Start_Sync();

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Synchronous RELC start ERR. ret=0x%x\n", ret));
				}
				else {
					Ddim_Print(("Synchronous RELC start OK\n"));
				}

				// Get RELC macro status.
				ret = Dd_RELC_Get_Status();

				Ddim_Print(("RELC decode status\n"));
				Ddim_Print(("# RELC macro status = 0x%x\n", ret));

				// Get RELC process status.
				ret = Dd_RELC_Get_Process_Status( &dec_info );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution RELC processing status getting error. ret=0x%x\n", ret));
					return;
				}

				Ddim_Print(("RELC processing status\n"));
				Ddim_Print(("# Number of bytes read = %d\n", dec_info.read_byte));
				Ddim_Print(("# Number of bytes written = %d\n", dec_info.write_byte));
				Ddim_Print(("# Number of processing blocks = %d\n", dec_info.proc_block));
				Ddim_Print(("# Decode the total number of bytes to read = %lu\n", dec_info.dec_total_read_bytes));
				Ddim_Print(("# Decode the total number of bytes to write = %lu\n", dec_info.dec_total_write_bytes));
				Ddim_Print(("# Sequential run counter = %lu\n", dec_info.seq_cnt));

				// Get RELC error status.
				ret = Dd_RELC_Get_Error_Status();

				Ddim_Print(("RELC error status information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Get RELC error number. 
				ret = Dd_RELC_Get_Error();

				Ddim_Print(("RELC error information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Close RELC macro.
				Dd_RELC_Close();

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
			else if ( test_num == 2 ) {		/* Continuous run mode */

				memcpy( (UCHAR *)0x58000000, gRELC_Desc_Table1, sizeof(gRELC_Desc_Table1) );
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				memset( &ctrl_desc, 0, sizeof(T_DD_RELC_CTRL_DESC) );

				ctrl_com.write_hprot	= 11;
				ctrl_com.read_hprot		= 11;

				// Initialize RELC macro.
				Dd_RELC_Init( ctrl_com.write_hprot, ctrl_com.read_hprot );	/* After power on and the reset, this is executed only once. */
				Ddim_Print(("RELC init OK\n"));

				// Open RELC macro.
				ret = Dd_RELC_Open();

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print( ("Error RELC Open !!\n" ) );
					return;
				}

				ctrl_com.desc_mode			= D_DD_RELC_RUN_MODE_DESC;
				ctrl_com.callback			= (RELK_CALLBACK)cmd_ct_relc_callback;
				ctrl_com.seq_num			= 3;		/* Continuous run mode */
				ctrl_com.cont_run_mode		= D_DD_RELC_CONT_MODE_CONT;
				ctrl_desc.descriptor_addr	= 0x58000000;

				// Set control common data.
				ret = Dd_RELC_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution common control setting error. ret=%d\n", ret));
					return;
				}

				// Get control common data.
				memset( &ctrl_com, 0, sizeof(T_DD_RELC_CTRL_CMN) );
				ret = Dd_RELC_Get_Ctrl_Common( &ctrl_com );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
					return;
				}

				Ddim_Print(("Common control information\n"));
				Ddim_Print(("# Descriptor Mode = %d\n", ctrl_com.desc_mode));
				Ddim_Print(("# Continuous Run Mode = %d\n", ctrl_com.cont_run_mode));
				Ddim_Print(("# Sequential Run Number of Times = %lu\n", ctrl_com.seq_num));

				// Set control descriptor data.
				ret = Dd_RELC_Ctrl_Descriptor( &ctrl_desc );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution descriptor control setting error. ret=%d\n", ret));
					return;
				}

				// Get control descriptor data.
				ret = Dd_RELC_Get_Ctrl_Descriptor( &ctrl_desc );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution descriptor control getting error. ret=0x%x\n", ret));
					return;
				}

				Ddim_Print(("Discriptor control information\n"));
				Ddim_Print(("# Descriptor address = 0x%lx\n", ctrl_desc.descriptor_addr));

				// Start RELC macro.(Synchronization of RELC macro start)
				ret = Dd_RELC_Start_Sync();

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Synchronous RELC start ERR. ret=0x%x\n", ret));
				}
				else {
					Ddim_Print(("Synchronous RELC start OK\n"));
				}

				// Get RELC macro status.
				ret = Dd_RELC_Get_Status();

				Ddim_Print(("RELC decode status\n"));
				Ddim_Print(("# RELC macro status = 0x%x\n", ret));

				// Get RELC process status.
				ret = Dd_RELC_Get_Process_Status( &dec_info );

				if ( ret != D_DD_RELC_OK ) {
					Ddim_Print(("Execution RELC processing status getting error. ret=0x%x\n", ret));
					return;
				}

				Ddim_Print(("RELC processing status\n"));
				Ddim_Print(("# Number of bytes read = %d\n", dec_info.read_byte));
				Ddim_Print(("# Number of bytes written = %d\n", dec_info.write_byte));
				Ddim_Print(("# Number of processing blocks = %d\n", dec_info.proc_block));
				Ddim_Print(("# Decode the total number of bytes to read = %lu\n", dec_info.dec_total_read_bytes));
				Ddim_Print(("# Decode the total number of bytes to write = %lu\n", dec_info.dec_total_write_bytes));
				Ddim_Print(("# Sequential run counter = %lu\n", dec_info.seq_cnt));

				// Get RELC error status.
				ret = Dd_RELC_Get_Error_Status();

				Ddim_Print(("RELC error status information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Get RELC error number. 
				ret = Dd_RELC_Get_Error();

				Ddim_Print(("RELC error information\n"));
				Ddim_Print(("# RELC error = 0x%x\n", ret));

				// Close RELC macro.
				Dd_RELC_Close();

				ct_relc_register_dump();
				ct_relc_buffer_dump();
			}
		}
	}
	else if ( strcmp( argv[ 1 ], "inthdr" ) == 0 ) {
#ifdef PC_DEBUG
		test_pat = strtoul( argv[ 2 ], &endstr, 16 );
		switch ( test_pat ) {
			case 1:
				IO_RELC.RELC_INT_EN.bit.SINT	= 1;
				IO_RELC.RELC_INT_ST.bit.__SINS	= 1;
				break;
			case 2:
				IO_RELC.RELC_INT_EN.bit.FINT	= 1;
				IO_RELC.RELC_INT_ST.bit.__FINS	= 1;
				break;
			case 3:
				IO_RELC.RELC_INT_EN.bit.EINT	= 1;
				IO_RELC.RELC_INT_ST.bit.__EINS	= 1;
				break;
			default:
				IO_RELC.RELC_INT_EN.bit.CINT	= 1;
				IO_RELC.RELC_INT_ST.bit.__CINS	= 1;
				break;
		}
#endif
		Dd_RELC_Int_Handler();
#ifdef PC_DEBUG
		ct_relc_register_dump();
#endif
	}
	else if ( strcmp( argv[ 1 ], "err" ) == 0 ) {
		if ( strcmp( argv[ 2 ], "open" ) == 0 ) {

			ret = Dd_RELC_Open();
			ret = Dd_RELC_Open();

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Open error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "ctrlcomm" ) == 0 ) {

			ret = Dd_RELC_Ctrl_Common( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "ctrlreg" ) == 0 ) {

			ret = Dd_RELC_Ctrl_Register( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution register control getting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "ctrldesc" ) == 0 ) {

			ret = Dd_RELC_Ctrl_Descriptor( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution descriptor control getting error. ret=0x%x\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_in_sta_addr" ) == 0 ) {

			ret = Dd_RELC_Set_In_Start_Addr( 0 );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input start address setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_in_end_addr" ) == 0 ) {

			ret = Dd_RELC_Set_In_End_Addr( 0 );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input end address setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_out_sta_addr" ) == 0 ) {

			ret = Dd_RELC_Set_Out_Start_Addr( 0 );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution output start address setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_out_end_addr" ) == 0 ) {

			ret = Dd_RELC_Set_Out_End_Addr( 0 );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution output end address setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_inbuf" ) == 0 ) {

			ret = Dd_RELC_Set_In_Buf_Data( 0, ref_buf_size );		/* pgr0039 */

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input buffer setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_inbufm" ) == 0 ) {

			ret = Dd_RELC_Set_In_Buf_Data_Mirror( 0, 256 );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution input buffer(mirror) setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "set_refbuf" ) == 0 ) {

			ret = Dd_RELC_Set_Ref_Buf_Data( 0, 256 );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution reference buffer setting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_ctrlcomm" ) == 0 ) {

			ret = Dd_RELC_Get_Ctrl_Common( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution comon control getting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_ctrlreg" ) == 0 ) {

			ret = Dd_RELC_Get_Ctrl_Register( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution register control getting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_ctrldesc" ) == 0 ) {

			ret = Dd_RELC_Get_Ctrl_Descriptor( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution descriptor control getting error. ret=0x%x\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_psta" ) == 0 ) {

#ifdef PC_DEBUG
			IO_RELC.RELC_READ_BYTE.bit.DRNM		= 1234;
			IO_RELC.RELC_WRITE_BYTE.bit.DWNM	= 2345;
			IO_RELC.RELC_BLOCK.bit.BLKN			= 3456;
			IO_RELC.RELC_INPUT_BYTE				= 876;
			IO_RELC.RELC_OUTPUT_BYTE			= 985;
			IO_RELC.RELC_SEQ_CNT				= 65;
#endif

			ret = Dd_RELC_Get_Process_Status( NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC processing status getting error. ret=0x%x\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_bufsize1" ) == 0 ) {

			ret = Dd_RELC_Get_Buf_Size( NULL, &ref_buf_size );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC buffer size getting error. ret=0x%x\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_bufsize2" ) == 0 ) {

			ret = Dd_RELC_Get_Buf_Size( &in_buf_size, NULL );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC buffer size getting error. ret=0x%x\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_inbuf" ) == 0 ) {

			ret = Dd_RELC_Get_In_Buf_Data( 0, set_size );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC input buffer data getting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_inbufm" ) == 0 ) {

			ret = Dd_RELC_Get_In_Buf_Data_Mirror( 0, set_size );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC input data buffer getting error. ret=%d\n", ret));
				return;
			}
		}
		else if ( strcmp( argv[ 2 ], "get_refbuf" ) == 0 ) {

			ret = Dd_RELC_Get_Ref_Buf_Data( 0, set_size );

			if ( ret != D_DD_RELC_OK ) {
				Ddim_Print(("Execution RELC reference data buffer getting error. ret=%d\n", ret));
				return;
			}
		}
	}
	else{
		ct_relc_help();
	}
}

void Ct_Dd_RELC_PC_Sim( void )
{
	INT32	argc;
	CHAR*	argv[ 32 ];

	argv[ 0 ] = (CHAR*)( "relc" );

	Ddim_Print(("[CT]01-01-1 : Dd_RELC_Init()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "init" );
	argv[ 2 ] = (CHAR*)( "F" );
	argv[ 3 ] = (CHAR*)( "F" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-2 : Dd_RELC_Open()\n"));
	argc = 2;
	argv[ 1 ] = (CHAR*)( "open" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-3 : Dd_RELC_Close()\n"));
	argc = 2;
	argv[ 1 ] = (CHAR*)( "close" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-4 : Dd_RELC_Start_Sync()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "start" );
	argv[ 2 ] = (CHAR*)( "sync" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-5 : Dd_RELC_Start_Async()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "start" );
	argv[ 2 ] = (CHAR*)( "async" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-6 : Dd_RELC_Stop()\n"));
	argc = 2;
	argv[ 1 ] = (CHAR*)( "stop" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-7 : Dd_RELC_Ctrl_Common()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "ctrlcomm" );
	argv[ 2 ] = (CHAR*)( "reg" );
	argv[ 3 ] = (CHAR*)( "nmod" );
	argv[ 4 ] = (CHAR*)( "8" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-8 : Dd_RELC_Ctrl_Common()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "ctrlcomm" );
	argv[ 2 ] = (CHAR*)( "desc" );
	argv[ 3 ] = (CHAR*)( "cmod" );
	argv[ 4 ] = (CHAR*)( "256" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-9 : Dd_RELC_Ctrl_Register()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "ctrlcomm" );
	argv[ 2 ] = (CHAR*)( "reg" );
	argv[ 3 ] = (CHAR*)( "nmod" );
	argv[ 4 ] = (CHAR*)( "8" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
	argc = 8;
	argv[ 1 ] = (CHAR*)( "ctrlreg" );
	argv[ 2 ] = (CHAR*)( "imaxd" );
	argv[ 3 ] = (CHAR*)( "omaxd" );
	argv[ 4 ] = (CHAR*)( "0x40105000" );
	argv[ 5 ] = (CHAR*)( "0x40107000" );
	argv[ 6 ] = (CHAR*)( "0x40207000" );
	argv[ 7 ] = (CHAR*)( "0x4020B000" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-10 : Dd_RELC_Ctrl_Register()\n"));
	argc = 8;
	argv[ 1 ] = (CHAR*)( "ctrlreg" );
	argv[ 2 ] = (CHAR*)( "imaxe" );
	argv[ 3 ] = (CHAR*)( "omaxe" );
	argv[ 4 ] = (CHAR*)( "0x40105000" );
	argv[ 5 ] = (CHAR*)( "0x40106000" );
	argv[ 6 ] = (CHAR*)( "0x40207000" );
	argv[ 7 ] = (CHAR*)( "0x40209000" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-11 : Dd_RELC_Ctrl_Descriptor()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "ctrldesc" );
	argv[ 2 ] = (CHAR*)( "0x40408000" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-12 : Dd_RELC_Set_In_Start_Addr()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "in_sta_addr" );
	argv[ 3 ] = (CHAR*)( "0x40302000" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-13 : Dd_RELC_Set_In_End_Addr()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "in_end_addr" );
	argv[ 3 ] = (CHAR*)( "0x40103000" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-14 : Dd_RELC_Set_Out_Start_Addr()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "out_sta_addr" );
	argv[ 3 ] = (CHAR*)( "0x40802000" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-15 : Dd_RELC_Set_Out_End_Addr()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "out_end_addr" );
	argv[ 3 ] = (CHAR*)( "0x40907012" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-16 : Dd_RELC_Get_Ctrl_Common()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "ctrlcomm" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-17 : Dd_RELC_Get_Ctrl_Register()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "ctrlreg" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-18 : Dd_RELC_Get_Ctrl_Descriptor()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "ctrldesc" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-19 : Dd_RELC_Get_Status()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "msta" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-20 : Dd_RELC_Get_Sleep_Reason()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "sleep" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-21 : Dd_RELC_Get_Error_Status()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "errsta" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-22 : Dd_RELC_Get_Error()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "err" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-23 : Dd_RELC_Get_Process_Status()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "psta" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-24 : Dd_RELC_Get_Buf_Size()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "bufsize" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
/*
	Ddim_Print(("[CT]01-01-25 : Dd_RELC_Set_In_Buf_Data()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "inbuf" );
	argv[ 3 ] = (CHAR*)( "0x3A001800" );
	argv[ 4 ] = (CHAR*)( "8192" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-26 : Dd_RELC_Set_In_Buf_Data_Mirror()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "inbufm" );
	argv[ 3 ] = (CHAR*)( "0x44900000" );
	argv[ 4 ] = (CHAR*)( "512" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-27 : Dd_RELC_Set_Ref_Buf_Data()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "set" );
	argv[ 2 ] = (CHAR*)( "relbuf" );
	argv[ 3 ] = (CHAR*)( "0x44800000" );
	argv[ 4 ] = (CHAR*)( "256" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-28 : Dd_RELC_Get_In_Buf_Data()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "inbuf" );
	argv[ 3 ] = (CHAR*)( "0x44900000" );
	argv[ 4 ] = (CHAR*)( "512" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-29 : Dd_RELC_Get_In_Buf_Data_Mirror()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "inbufm" );
	argv[ 3 ] = (CHAR*)( "0x44A00000" );
	argv[ 4 ] = (CHAR*)( "1024" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-30 : Dd_RELC_Get_Ref_Buf_Data()\n"));
	argc = 5;
	argv[ 1 ] = (CHAR*)( "get" );
	argv[ 2 ] = (CHAR*)( "relbuf" );
	argv[ 3 ] = (CHAR*)( "0x44B00000" );
	argv[ 4 ] = (CHAR*)( "256" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-31 : Dd_RELC_Utility_Register()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "norm" );
	argv[ 2 ] = (CHAR*)( "async" );
	argv[ 3 ] = (CHAR*)( "1" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-32 : Dd_RELC_Utility_Register()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "norm" );
	argv[ 2 ] = (CHAR*)( "async" );
	argv[ 3 ] = (CHAR*)( "2" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-33 : Dd_RELC_Utility_Descriptor()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "desc" );
	argv[ 2 ] = (CHAR*)( "async" );
	argv[ 3 ] = (CHAR*)( "1" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-34 : Dd_RELC_Utility_Descriptor()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "desc" );
	argv[ 2 ] = (CHAR*)( "async" );
	argv[ 3 ] = (CHAR*)( "2" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-35 : Dd_RELC_Utility_Register()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "norm" );
	argv[ 2 ] = (CHAR*)( "sync" );
	argv[ 3 ] = (CHAR*)( "1" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-36 : Dd_RELC_Utility_Register()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "norm" );
	argv[ 2 ] = (CHAR*)( "sync" );
	argv[ 3 ] = (CHAR*)( "2" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-37 : Dd_RELC_Utility_Descriptor()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "desc" );
	argv[ 2 ] = (CHAR*)( "sync" );
	argv[ 3 ] = (CHAR*)( "1" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-01-38 : Dd_RELC_Utility_Descriptor()\n"));
	argc = 4;
	argv[ 1 ] = (CHAR*)( "desc" );
	argv[ 2 ] = (CHAR*)( "sync" );
	argv[ 3 ] = (CHAR*)( "2" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
*/
	Ddim_Print(("[CT]01-01-39 : Dd_RELC_Int_Handler()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "inthdr" );
	argv[ 2 ] = (CHAR*)( "1" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
	argv[ 2 ] = (CHAR*)( "2" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
	argv[ 2 ] = (CHAR*)( "3" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
	argv[ 2 ] = (CHAR*)( "4" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-1 : Dd_RELC_Open()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "open" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-2 : Dd_RELC_Ctrl_Common()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "ctrlcomm" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-3 : Dd_RELC_Ctrl_Register()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "ctrlreg" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-4 : Dd_RELC_Ctrl_Descriptor()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "ctrldesc" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-5 : Dd_RELC_Set_In_Start_Addr()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_in_sta_addr" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-6 : Dd_RELC_Set_In_End_Addr()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_in_end_addr" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-7 : Dd_RELC_Set_Out_Start_Addr()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_out_sta_addr" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-8 : Dd_RELC_Set_Out_End_Addr()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_out_end_addr" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-9 : Dd_RELC_Set_In_Buf_Data()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_inbuf" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-10 : Dd_RELC_Set_In_Buf_Data_Mirror()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_inbufm" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-11 : Dd_RELC_Set_Ref_Buf_Data()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "set_refbuf" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-12 : Dd_RELC_Get_Ctrl_Common()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_ctrlcomm" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-13 : Dd_RELC_Get_Ctrl_Register()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_ctrlreg" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-14 : Dd_RELC_Get_Ctrl_Descriptor()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_ctrldesc" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-15 : Dd_RELC_Get_Status()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_psta" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-16 : Dd_RELC_Get_Buf_Size()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_bufsize1" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-17 : Dd_RELC_Get_Buf_Size()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_bufsize2" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-18 : Dd_RELC_Get_In_Buf_Data()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_inbuf" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-19 : Dd_RELC_Get_In_Buf_Data_Mirror()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_inbufm" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));

	Ddim_Print(("[CT]01-02-20 : Dd_RELC_Get_Ref_Buf_Data()\n"));
	argc = 3;
	argv[ 1 ] = (CHAR*)( "err" );
	argv[ 2 ] = (CHAR*)( "get_refbuf" );
	Ct_Dd_RELC_Main( argc, argv );
	Ddim_Print(("\n"));
}
