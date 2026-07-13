
#include <stdio.h>
#define _M10V_
#define ISP_FIFO

#ifdef WIN32
    #define _X86_
    #include <Synchapi.h>
    extern VOID WriterGeneric(char *, unsigned long );
    #define kRxBuf_Size (256) // must be 2^x
    #define kTxBuf_Size (256)  // must be 2^x
    #define		_Disable_Int()	{}
    #define		_Enable_Int()	 {}
    #define	_Sleep(ms)		(Sleep(ms))

#elif defined(_M12MO_)

    #include "common.h"
    #include "_arm_swi.h"
    #include "rt_misc.h"
    #include "MACRO.h"
    #include "GNOS.h"
    #include "Task_System.h"
    //#include "Task_UART.h"
    #include "AlTypStd.h"
    #include "AlFix.h"
    #include <math.h>
    #include "AlAwbWrap.h"
    #include "AlAwblib.h"
    #include "log.h"
    #include "memory.h"
    #include <string.h>
    #include "_usio.h"
    #include "log.h"
    #include "FRTimer_driver.h"
    static int sStatus_reg;
    UCHAR ret=0;
    #define		_Disable_Int()	{sStatus_reg = __DI();}
    #define		_Enable_Int()	{ __EI(sStatus_reg);}


#elif defined(_M10V_)
    #include <string.h>
    #include "../include/im_tuning_buffer.h"
    #include "../include/im_tuning_custom.h"
    #include "dd_arm.h"
    #include "uart_csio.h"
    #include "dd_uart.h"
    #include "dd_csio.h"
    #include "dd_hdmac1.h"
    #include "dd_top.h"
    #include "ddim_user_custom.h"
    #include "dd_arm.h"
    #include "dd_gic.h"
    #include "os_user_custom.h"
    #include <stdarg.h>
    #include <stdlib.h>

    static int sStatus_reg;
    UCHAR ret=0;
    #define BUFF_SIZE	128
    #define		_Disable_Int()	{sStatus_reg = Dd_ARM_DI();}
    #define		_Enable_Int()	{ Dd_ARM_EI(sStatus_reg);}
    #define UART_CH     2

#endif

#define kBAUDRATE	(57600)

#define kAsciiCmdBuf_Size	(128)
#define kBinCmdBuf_Size		(128)


#define  _TX_FIFO_SIZE	(120)
#define  _RX_FIFO_SIZE	(120)
#define MAX_UART_FIFO_BUF   2

UCHAR grxbuf[kRxBuf_Size];
static ULONG grxbuf_rp=0;
static ULONG grxbuf_wp=0;
static ULONG grxbuf_overcnt=0;
static UCHAR grxbuf_full=0;
static UCHAR gtxbuf[kTxBuf_Size];
static ULONG gtxbuf_rp=0;
static ULONG gtxbuf_wp=0;
static ULONG gtxbuf_overcnt=0;
static UCHAR gtxbuf_full=0;

VOID __ISR_txbuf2fifo(VOID);    //Update txbuf to FIFO
VOID __ISR_rxfifo2buf(VOID);	//Update FIFO to rxbuf
static VOID TriggerTx(VOID);
/*

e.g. write pointer & read pointer  relation, circular buff cases,
X => valid data, written but not read yet
- => space
Case wp > rp: - space buffer top & bottom :

  -------
wp-------
  XXXXXXX
  XXXXXXX
rpXXXXXXX
  -------
=>  space = buf_size - (wp-rp);
top_space = buf_size  - |wp|
down_space = |rp|

Case rp > wp:  - space at buffer center:
  XXXXXXX
rpXXXXXXX
  -------
  -------
wp-------
  XXXXXXX
  XXXXXXX

=> space = buf_size - (wp-rp);
top_space = space;
down_space = 0
case  rp = wp  & full = 0
   Whole buffer empty
case rp = wp & full = 1
   Whole buffer full
*/


static UCHAR sBufAsciicmd[kAsciiCmdBuf_Size];
static ULONG sBufAsciicmd_size;
static UCHAR sBufBincmd[kBinCmdBuf_Size];
static ULONG sBufBincmd_size;
static ULONG gIQ_DbgMsgMask = D_DBGMASK;

static CHAR sEcho = 0;

#if defined(_M12MO_)
static VOID UART_Trigger_Transmit(VOID)
{
    //IO_USIO.UART[ch].SCR.bit.TXE = 1;
    IO_USIO.UART[ch].FCR.bit.FTIE = 1;
}

static VOID UART_Trigger_Transmit_OFF(VOID)
{
    //IO_USIO.UART[ch].SCR.bit.TXE = 0;
    IO_USIO.UART[ch].FCR.bit.FTIE = 0;
}

//static VOID UART_FDRQ_SET(VOID)
//{
//    IO_USIO.UART[UART_CH].FCR.bit.FDRQ = 1;
//}

static VOID UART_FDRQ_CLR(VOID)
{
    IO_USIO.UART[UART_CH].FCR.bit.FDRQ = 0;
}

VOID uart_FIFO_Init(VOID)
{
	IO_USIO.UART[UART_CH].FCR.bit.FSEL  = 0; //0: FE1: Tx , FE2: Rx
	IO_USIO.UART[UART_CH].FCR.bit.FE1   = 1;
	IO_USIO.UART[UART_CH].FCR.bit.FE2   = 1;
	IO_USIO.UART[UART_CH].FCR.bit.FRIIE   = 1;
	IO_USIO.UART[UART_CH].FCR.bit.FTIE  = 1;

	IO_USIO.UART[UART_CH].FBYTE.byte[0]  = MAX_UART_FIFO_SIZE;
	IO_USIO.UART[UART_CH].FBYTE.byte[1]  = MAX_UART_FIFO_SIZE;
}

ULONG test_grxbuf_rp = 0;
UCHAR fifo_rxsize=0;
UCHAR drop_fifo_cnt=0;
UCHAR drop_fifo_val=0;
UCHAR tx_fifo= 0 ;

UCHAR readbuf2[200]={0};
LONG readbuf_outsize2=0;

VOID uart_TxBuf_To_Terminal()
{
    //uart_RxBuf_GetData(&readbuf2[0],200,&readbuf_outsize2,1);
    //uart_TxBuf_write(&readbuf2[0],readbuf_outsize2);
}
#elif defined (_M10V_)

static VOID UART_Trigger_Transmit(VOID)
{
    //IO_USIO.UART[ch].SCR.bit.TXE = 1;
    IO_USIO.UART[UART_CH].FCR.bit.FTIE = 1;
}

static VOID UART_Trigger_Transmit_OFF(VOID)
{
    //IO_USIO.UART[ch].SCR.bit.TXE = 0;
    IO_USIO.UART[UART_CH].FCR.bit.FTIE = 0;
}

static VOID UART_FDRQ_CLR(VOID)
{
    IO_USIO.UART[UART_CH].FCR.bit.FDRQ = 0;
}

VOID uart_FIFO_Init(VOID)
{
    IO_USIO.UART[UART_CH].SCR.byte = 0x80;
	IO_USIO.UART[UART_CH].SMR.byte = 0x01;
	IO_USIO.UART[UART_CH].SSR.byte = 0x80;
	IO_USIO.UART[UART_CH].ESCR.byte = 0x00;
	//IO_USIO.UART[UART_CH].BGR.hword = (USHORT)(PERI_CLK/baudrate-1);
	IO_USIO.UART[UART_CH].FCR.hword = 0x000C;
	IO_USIO.UART[UART_CH].SCR.byte |= 0x13;

	IO_USIO.UART[UART_CH].FCR.bit.FSEL  = 0; //0: FE1: Tx , FE2: Rx
	IO_USIO.UART[UART_CH].FCR.bit.FE1   = 1;
	IO_USIO.UART[UART_CH].FCR.bit.FE2   = 1;
	IO_USIO.UART[UART_CH].FCR.bit.FRIIE   = 1;
	IO_USIO.UART[UART_CH].FCR.bit.FTIE  = 1;

	IO_USIO.UART[UART_CH].FBYTE.byte[0]  = MAX_UART_FIFO_SIZE;
	IO_USIO.UART[UART_CH].FBYTE.byte[1]  = MAX_UART_FIFO_SIZE;
}

ULONG test_grxbuf_rp = 0;
UCHAR fifo_rxsize=0;
UCHAR tx_fifo= 0 ;

#endif

VOID uart_RxTxBuf_init()
{
	grxbuf_rp = 0;
	grxbuf_wp = 0;
	gtxbuf_rp = 0;
	gtxbuf_wp = 0;
	grxbuf_overcnt = 0;
	gtxbuf_overcnt = 0;
	gtxbuf_full = 0;
	grxbuf_full = 0;
	sBufBincmd_size = 0;
}

/*
in PC to ISR routineS, Call this in UART receive ISR
return: byte count that write to buffer
*/
INT32 uart_RxBuf_write(UCHAR *in_p, ULONG in_size)
{
	INT32 wr_cnt;
	ULONG tspace, wsize, i;
	UCHAR *p;
	ULONG rp, wp;

#ifdef ISP_FIFO
	volatile CHAR* v_in_p= (volatile CHAR*)in_p;
#endif
	if (in_size == 0)
		return 0;
	if (in_p == NULL)
		return -2;
	//if (grxbuf_full)
		//return -1;

	_Disable_Int();
	rp = grxbuf_rp;
	wp = grxbuf_wp;

	_Enable_Int();

	if (wp >= rp)
	{  // space split to top & bottom
		// fill top buffer
		tspace = kRxBuf_Size - wp;
		wsize = (in_size > tspace) ? tspace : in_size;
		p = &grxbuf[wp];
		for (i = 0; i <wsize; i++) {
#ifdef ISP_FIFO
			*p++ = *v_in_p;
#else
			*p++ = *in_p++;
#endif
		}
		wr_cnt = wsize;
		if (in_size > wsize) {
			// fill bottom buffer
			if ((in_size - wsize) > rp) {
				wsize = rp;
			}
			else {
				wsize = (in_size - wsize);
			}
			p = &grxbuf[0];
			for (i = 0; i <wsize; i++) {
#ifdef ISP_FIFO
			*p++ = *v_in_p;
#else
			*p++ = *in_p++;
#endif
			}
			wr_cnt += wsize;
		}
	}
	else {
		// space at buffer center
		tspace = rp - wp;
		wsize = (in_size > tspace) ? tspace : in_size;
		p = &grxbuf[wp];
		for (i = 0; i <wsize; i++) {
#ifdef ISP_FIFO
			*p++ = *v_in_p;
#else
			*p++ = *in_p++;
#endif
		}
		wr_cnt = wsize;
	}

	wp += wr_cnt;
	if (wp >= kRxBuf_Size){
		grxbuf_wp = wp - kRxBuf_Size;
		grxbuf_overcnt++;
	}
	else {
		grxbuf_wp = wp;
	}
	if (grxbuf_wp == grxbuf_rp)
		grxbuf_full = 1;

	return wr_cnt;
}


/* call by systm*/
/*
return
-1: timeout
0:  no command
1:  ASCII command
2:  ASCII command not complete
3:  binary command
4:  binary command not complete

<0:   error
*/
#if 0
INT32 uart_RxBuf_GetData(UCHAR *in_p, ULONG in_maxsize, LONG *out_psize)
{
	ULONG rp, wp, overcnt, i, full;
	ULONG bufcnt, oldascii_idx, oldbin_idx;
	UCHAR c;
	INT32 ret=0;
	if (in_p == NULL || in_maxsize == 0 || out_psize == NULL)
		return -1;

	_Disable_Int();
	rp = grxbuf_rp;
	wp = grxbuf_wp;volatile CHAR*
	overcnt = grxbuf_overcnt;
	full = grxbuf_full;
	_Enable_Int();
	if ((rp == wp) && !full)
	{
		return 0; // buffer empty
	}

	if (wp > rp) {
		bufcnt = wp - rp;
	}
	else {
		bufcnt = kRxBuf_Size - rp + wp;
	}

	c = grxbuf[rp];
	if (c < 0x80)
	{
		ret = 0;
		// ascii
		oldascii_idx = sBufAsciicmd_size;
		for (i = 0; i < bufcnt; i++)
		{
			//scan buf until /r/n, '/0'   or   buffer end or non-ASCII, 0x80;
			c = grxbuf[rp++];
			if (rp >= kRxBuf_Size)
				rp = 0;
			if (c < 0x80) {
				sBufAsciicmd[sBufAsciicmd_size++] = c;
				if (c == 0 || c == '\n' || c== '\r') {
					// scan end
					ret = 1;
					break;
				}
				else {
				}
				if (sBufAsciicmd_size >= kAsciiCmdBuf_Size) {
					// scan end
					ret = 2;
					break;
				}

			} else {
				//end command, it should not be happen
				rp--;
				ret = 2;
				break;
			}
		}
		if (sEcho) {
			static ULONG txcnt = 0;
#ifdef _DEBUG_BUFFERING_
			if (oldascii_idx == 0) { // new command
				CHAR buf[16];
				int len;
				len = sprintf_s(buf, 16, "%04i", txcnt++);
				uart_TxBuf_write(buf, len);
			}
#endif
			uart_TxBuf_write(sBufAsciicmd + oldascii_idx, sBufAsciicmd_size - oldascii_idx);
		}
		if (ret)
		{
			// return ascii commmand

			memcpy(in_p, sBufAsciicmd, sBufAsciicmd_size);
			*out_psize = sBufAsciicmd_size;
			sBufAsciicmd_size = 0;
		}
		else {
			// wait next CHARacter. no return
		}

	}
	else {
		ret = 0;
		// Bin Command
		oldbin_idx = sBufBincmd_size;
		for (i = 0; i < bufcnt; i++)
		{
			//scan buf until 0xFx, '/0'   or   buffer end or ASCII, 0x80;
			c = grxbuf[rp++];
			if (rp >= kRxBuf_Size)
				rp = 0;
			if (c < 0x80) {
				rp--;
				ret = 4;
				break;
			} else {
				// 0x80~0xEF,  valid data
				sBufBincmd[sBufBincmd_size++] = c;
				if (c >= 0xF0) {
					ret=3;
					break;
				}
				if (sBufBincmd_size >= kBinCmdBuf_Size) {
					// scan end
					ret = 4;
					break;
				}
			}
		}

		if (ret)
		{
			// return ascii commmand
			memcpy(in_p, sBufBincmd, sBufBincmd_size);
			*out_psize = sBufBincmd_size;
			sBufBincmd_size = 0;
		}
		else {
			// wait next CHARacter. no return
		}
	}
	grxbuf_rp = rp;
	grxbuf_full = 0;
	return ret;
}
#else
INT32 uart_RxBuf_GetData(UCHAR *in_p, ULONG in_maxsize, LONG *out_psize, INT32 mode) // mode- 0: command mode,  1: normal mode with , 2~~0xFFFFFFFF, time out in us
{
	ULONG rp, wp, i, full;
	ULONG bufcnt, oldascii_idx;
	UCHAR c;
	INT32 ret = 0;
	if (in_p == NULL || in_maxsize == 0 || out_psize == NULL)
		return -1;

	_Disable_Int();
	rp = grxbuf_rp;
	wp = grxbuf_wp;
//	overcnt = grxbuf_overcnt;
	full = grxbuf_full;
	_Enable_Int();
	if ((rp == wp) && !full)
	{
		return 0; // buffer empty
	}

	if (wp > rp) {
		bufcnt = wp - rp;
	}
	else {
		bufcnt = kRxBuf_Size - rp + wp;
	}

	c = grxbuf[rp];
	if (mode == 0) {
		if (c < 0x80)
		{
			ret = 0;
			// ascii
			oldascii_idx = sBufAsciicmd_size;
			for (i = 0; i < bufcnt; i++)
			{
				//scan buf until /r/n, '/0'   or   buffer end or non-ASCII, 0x80;
				c = grxbuf[rp++];
				if (rp >= kRxBuf_Size)
					rp = 0;
				if (c < 0x80) {
					sBufAsciicmd[sBufAsciicmd_size++] = c;
					if (c == 0 || c == '\n' || c == '\r') {
						// scan end
						if ((grxbuf[rp] == '\n') || (grxbuf[rp] == '\r'))
                        {
                            sBufAsciicmd[sBufAsciicmd_size++] = grxbuf[rp++];   // in case \r\n or \r or \n came, Buf can also receive whole string
                        }
						ret = 1;
						break;
					}
					else {
					}
					if (sBufAsciicmd_size >= kAsciiCmdBuf_Size) {
						// scan end
						ret = 2;
						break;
					}

				}
				else {
					//end command, it should not be happen
					rp--;
					ret = 2;
					break;
				}
			}
			if (sEcho) {

#ifdef _DEBUG_BUFFERING_
                static ULONG txcnt = 0;
				if (oldascii_idx == 0) { // new command
					CHAR buf[16];
					int len;
					len = sprintf_s(buf, 16, "%04i", txcnt++);
					uart_TxBuf_write(buf, len);
				}
#endif
				uart_TxBuf_write(sBufAsciicmd + oldascii_idx, sBufAsciicmd_size - oldascii_idx);
			}
			if (ret)
			{
				// return ascii commmand

				memcpy(in_p, sBufAsciicmd, sBufAsciicmd_size);
				*out_psize = sBufAsciicmd_size;
				sBufAsciicmd_size = 0;
			}
			else {
				// wait next CHARacter. no return
			}

		}
		else {
			ret = 0;
			// Bin Command
			//oldbin_idx = sBufBincmd_size;
			for (i = 0; i < bufcnt; i++)
			{
				//scan buf until 0xFx, '/0'   or   buffer end or ASCII, 0x80;
				c = grxbuf[rp++];
				if (rp >= kRxBuf_Size)
					rp = 0;
				if (c < 0x80) {
					rp--;
					ret = 4;
					break;
				}
				else {
					// 0x80~0xEF,  valid data
					sBufBincmd[sBufBincmd_size++] = c;
					if (c >= 0xF0) {
						ret = 3;
						break;
					}
					if (sBufBincmd_size >= kBinCmdBuf_Size) {
						// scan end
						ret = 4;
						break;
					}
				}
			}

			if (ret)
			{
				// return ascii commmand
				memcpy(in_p, sBufBincmd, sBufBincmd_size);
				*out_psize = sBufBincmd_size;
				sBufBincmd_size = 0;
			}
			else {
				// wait next CHARacter. no return
			}
		}
	} else {
		// raw mode
		ULONG waittime = mode;
		ULONG blankwait=0;//totalwait = 0
		//ULONG half_ms = 0;
		ULONG size, bufsize,  rem_size = in_maxsize;
		UCHAR *pcurr = in_p;
		/*Calc the polling period,  arround  half of buffer*/
		//half_ms = get_uart_send_ideatime(kRxBuf_Size / 2);

		// try to read out the all the buffer first.
		while (1)
		{
			if (wp > rp){
				bufsize = (wp - rp);
			} else if (rp > wp){
				bufsize = (kRxBuf_Size - rp);
			} else {
				bufsize = (full) ? (kRxBuf_Size) : 0;
			}
			size = (bufsize > rem_size) ? rem_size : bufsize;
			memcpy(pcurr, &grxbuf[rp], size);
			rp += size;
			if (rp >= kRxBuf_Size) rp = 0;
			pcurr += size;
			rem_size -= size;
			grxbuf_rp = rp;
			grxbuf_full = 0;

			if (rem_size == 0) {
				break;
			}
			if (blankwait > waittime)
			{
				ret = -1; // time out
				break;
			}
			if ( wp == rp)
			{  // buffer empty, wait and try to get buffer again
				//wait half_ms;vvvvvv
				//OS_User_Dly_Tsk(500);
				//if (size == 0) blankwait += 500;
				//totalwait += 500;
				// update all the rp/wp
				_Disable_Int();
				rp = grxbuf_rp;
				wp = grxbuf_wp;
				full = grxbuf_full;
				_Enable_Int();
				break;
			}
		}
		*out_psize = in_maxsize - rem_size;
	}
	grxbuf_rp = rp;
	grxbuf_full = 0;
	return ret;
}

#endif

// ISP to PC
// Content:  command reply (binary or ASCII command),  echo of ASCII command, general Debug printf(ASCII),
INT32 uart_TxBuf_write(UCHAR *in_p, ULONG in_size)
{
	INT32 wr_cnt;
	ULONG tspace, wsize, i;
	ULONG rp, wp ;
	//ULONG overcnt,full;

	UCHAR *p;
	if (in_size == 0)
		return 0;
	if (in_p == NULL)
		return -2;
	if (gtxbuf_full)
		return -1;
	/* Must be Mutex Exclusive*/

	_Disable_Int();
	//// lock
	rp = gtxbuf_rp;
	wp = gtxbuf_wp;
	//overcnt = gtxbuf_overcnt;
	//full = gtxbuf_full;
	_Enable_Int();


	if (wp >= rp)
	{  // space split to top & bottom
	   // fill top buffer
		tspace = kTxBuf_Size - wp;
		wsize = (in_size > tspace) ? tspace : in_size;
		p = &gtxbuf[wp];
		for (i = 0; i <wsize; i++) {
			*p++ = *in_p++;
		}
		wr_cnt = wsize;
		if (in_size > wsize) {
			// fill bottom buffer
			if ((in_size - wsize) > rp) {
				wsize = rp;
			}
			else {
				wsize = (in_size - wsize);
			}
			p = &gtxbuf[0];
			for (i = 0; i <wsize; i++) {
				*p++ = *in_p++;
			}
			wr_cnt += wsize;
		}
		wp += wr_cnt;
		if (wp > kTxBuf_Size) {
			wp -= kTxBuf_Size;
			gtxbuf_overcnt++;
		}
	}
	else {
		// space at buffer center
		tspace = rp - wp;
		wsize = (in_size > tspace) ? tspace : in_size;
		p = &gtxbuf[wp];
		for (i = 0; i <wsize; i++) {
			*p++ = *in_p++;
		}
		wr_cnt = wsize;
		wp += wr_cnt;
	}

	_Disable_Int();

	if (wp == gtxbuf_rp) // must check the latest gtxbuf_rp may that may be already uopdated.
		gtxbuf_full = 1;
	if (wp>= kTxBuf_Size)
		wp = 0;
	gtxbuf_wp = wp;

	//UART_Log_printfln("TxBuf w %d r %d wr_cnt %d",wp,rp,wr_cnt);

	_Enable_Int();

	TriggerTx();

	return wr_cnt;
}

VOID uart_RxBuf_getcnt(ULLONG *wr, ULLONG *rd)
{
	ULLONG tmp;
	ULONG rp, wp, overcnt, full;

	_Disable_Int();
	rp = grxbuf_rp;
	wp = grxbuf_wp;
	overcnt = grxbuf_overcnt;
	full = grxbuf_full;
	_Enable_Int()
	tmp = overcnt  * kRxBuf_Size;

	if (wr)
		*wr = tmp + wp;
	if (rd == NULL)
		return;
	*rd = tmp + rp;

	if (full || wp > rp)
	{
		*rd -= kRxBuf_Size;
	}
	return;
}

VOID uart_TxBuf_getcnt(ULLONG *wr, ULLONG *rd)
{
	ULLONG tmp;
	ULONG rp, wp, overcnt, full;
	_Disable_Int();
	rp = gtxbuf_rp;
	wp = gtxbuf_wp;
	overcnt = gtxbuf_overcnt;
	full = gtxbuf_full;
	_Enable_Int()
	tmp = overcnt  * kTxBuf_Size;

	if (wr)
		*wr = tmp + wp;
	if (rd == NULL)
		return;
	*rd = tmp + rp;

	if (full || wp > rp)
	{
		*rd -= kTxBuf_Size;
	}
	return;
}




static VOID TriggerTx()
{
#ifdef WIN32
	// assume no FIFO empty interrupt, direct send  unit buffer empty.
	while (gtxbuf_full || gtxbuf_wp!= gtxbuf_rp)
		__ISR_txbuf2fifo();

#elif defined(_M12MO_)
	if ((IO_USIO.UART[UART_CH].SSR.bit.TBI == 1)
    //(IO_USIO.UART[UART_CH].SSR.bit.TDRE == 1) &&
    //  || ((IO_USIO.UART[UART_CH].SSR.bit.TDRE == 0) && (IO_USIO.UART[UART_CH].FBYTE.bit.FIFO1 == 0))
     )
        UART_Trigger_Transmit();
#elif defined (_M10V_)
	if ((IO_USIO.UART[UART_CH].SSR.bit.__TBI == 1))
    //if ((IO_USIO.UART[UART_CH].SSR.bit.TDRE == 1) &&
    //  || ((IO_USIO.UART[UART_CH].SSR.bit.TDRE == 0) && (IO_USIO.UART[UART_CH].FBYTE.bit.FIFO1 == 0)))
        UART_Trigger_Transmit();
#else
	// Error
#pragma  "Error"
#endif
}

UCHAR  bTrigger_Rx = 0;
VOID SET_Trigger_Rx_Event(char val)
{
    bTrigger_Rx = val;
}

BOOL GET_Trigger_Rx_Event(VOID)
{
    return bTrigger_Rx;
}




UCHAR btoggle = 1;
VOID Dd_UART_IQ_Int_Handler_Rx(UCHAR ch)
{

	// IRQ: Full, IDLE
	//// lock
	//ULONG rxsize=0;
	//UCHAR *p=NULL;

#ifdef WIN32

#elif defined(_M12MO_)
	if( IO_USIO.UART[UART_CH].SSR.bit.RDRF && IO_USIO.UART[UART_CH].SCR.bit.RIE ){
        fifo_rxsize = IO_USIO.UART[UART_CH].FBYTE.bit.FIFO2;
        ret = uart_RxBuf_write((UCHAR*)&(IO_USIO.UART[UART_CH].DR.byte[1]),fifo_rxsize);
        GNOS_Send_Dtq_From_Irq_Mode(TASK_ID_IQTUNE, E_EVENT_UART_RX_END); // TASK_UART handle the receive data
	}
#elif defined (_M10V_)
	if( IO_USIO.UART[2].SSR.bit.__RDRF && IO_USIO.UART[2].SCR.bit.RIE ){
        fifo_rxsize = IO_USIO.UART[2].FBYTE.bit.__FIFO2;
        ret = uart_RxBuf_write((UCHAR*)&(IO_USIO.UART[2].DR.byte[0]),fifo_rxsize);
	}

#else
		// Error
#pragma  "Error"
#endif
	//uart_RxBuf_write(p, rxsize);

}


void Dd_UART_IQ_Int_Handler_Tx(unsigned char ch)
{
	// IRQ: Empty (FIFO empty)
	ULONG rp, wp, full;
	//ULONG overcnt;
	ULONG txsize;
	UCHAR i =0;

	_Disable_Int();
	rp = gtxbuf_rp;
	wp = gtxbuf_wp;
	//overcnt = gtxbuf_overcnt;
	full = gtxbuf_full;
	//full = gtxbuf_full;
	_Enable_Int();

	if (wp > rp)
	{
		txsize = ((wp-rp) > _TX_FIFO_SIZE) ? _TX_FIFO_SIZE : (wp - rp);
	} else if (rp > wp) {
		txsize = ((kTxBuf_Size - rp) > _TX_FIFO_SIZE) ? _TX_FIFO_SIZE : (kTxBuf_Size - rp);
	} else { //rp == wp
		if (full)
		{
			txsize = _TX_FIFO_SIZE;
		}
		else {
			// send nothing
			txsize = 0;
			//UART_Log_printfln("tx none");
			// STOP  TXE?
			#if defined(_M12MO_)
				UART_Trigger_Transmit_OFF();
			#elif defined(_M10V_)
                UART_Trigger_Transmit_OFF();
			#endif
			return;
		}
	}
	if (txsize)
	{
#ifdef WIN32
		// no FIFO in window, call direct send
		WriterGeneric(&gtxbuf[rp], txsize);
#elif defined(_M12MO_)
		// Copy 'txsize' byte from &gtxbuf[rp]   to FIFO
            for (i = 0; i < txsize; i++)
            {
                IO_USIO.UART[UART_CH].DR.byte[1] = gtxbuf[rp+i];
            }
            UART_FDRQ_CLR();
#elif defined (_M10V_)
            for (i = 0; i < txsize; i++)
            {
                IO_USIO.UART[UART_CH].DR.byte[0] = gtxbuf[rp+i];
            }
            UART_FDRQ_CLR();
#else
		// Error
#pragma  "Error"
#endif
		rp += txsize;
		if (rp >= kTxBuf_Size)
			rp = 0;
		gtxbuf_rp = rp;
		if (full)
			gtxbuf_full = 0;

	}
}

int RX_HaveNewData()
{
	return (grxbuf_rp != grxbuf_wp);
}


ULONG get_uart_send_ideatime(ULONG size)
{
	ULONG ms;

	/*Calc the polling period,  arround  half of buffer*/
	ms = (ULONG)((float)size * (1000 * 10) / kBAUDRATE + 0.5);// 10=>8N1,   1start + 8bit + 1stop

	return ms;
}

ULONG Get_gtxbuf_rp(VOID)
{
    return gtxbuf_rp;
}

ULONG Get_gtxbuf_wp(VOID)
{
    return gtxbuf_wp;
}

UCHAR bUartMode = DEBUG_GO;
UCHAR getUartMode()
{
    return bUartMode;
}

VOID setUartMode(UCHAR val)
{
    bUartMode = val;
}

VOID Set_Print_MsgMask_Bit(ULONG mask)
{
    gIQ_DbgMsgMask |= (mask);
}

VOID Clr_Print_MsgMask_Bit(ULONG mask)
{
    gIQ_DbgMsgMask &= ~(mask);
}

VOID Set_Print_MsgMask(ULONG val)
{
    gIQ_DbgMsgMask = val;
}

ULONG Get_Print_MsgMask()
{
    return gIQ_DbgMsgMask;
}

VOID IQ_PRINTLN(ULONG MASK, const char *format, ...)
{
	va_list	ap;
//	INT32	status_reg;
    UINT32  str_len;
	CHAR   s_buff[BUFF_SIZE];
	UCHAR r = '\r';
	UCHAR n = '\n';
    if (gIQ_DbgMsgMask & MASK)   //Don't print if MASK not defined
    {
    //    status_reg = Dd_ARM_DI();

        va_start(ap, format);
        str_len = vsnprintf(s_buff, BUFF_SIZE, format, ap);
        if (getUartMode() != DEBUG_BLOCK)
            uart_TxBuf_write((UCHAR*)s_buff,str_len);
        va_end(ap);

        if (getUartMode() != DEBUG_BLOCK)
        {
            uart_TxBuf_write(&r,1);
            uart_TxBuf_write(&n,1);
        }
//        Dd_ARM_EI( status_reg );
    }
}

VOID IQ_PRINT(ULONG MASK, const char *format, ...)
{
	va_list	ap;
//	INT32	status_reg;
    UINT32  str_len;
	CHAR   s_buff[BUFF_SIZE];

    if (gIQ_DbgMsgMask & MASK)   //Don't print if MASK not defined
    {
  //      status_reg = Dd_ARM_DI();

        va_start(ap, format);
        str_len = vsnprintf(s_buff, BUFF_SIZE, format, ap);
        if (getUartMode() != DEBUG_BLOCK)
            uart_TxBuf_write((UCHAR*)s_buff,str_len);
        va_end(ap);

 //       Dd_ARM_EI( status_reg );
    }
}
