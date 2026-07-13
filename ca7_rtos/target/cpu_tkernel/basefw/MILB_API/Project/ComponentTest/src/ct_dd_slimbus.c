/**
 * @file		ct_dd_slimbus.c
 * @brief		ct code for dd_slimbus
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "ddim_user_custom.h"
#include "dd_slimbus.h"
#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	CONFIG_MSG_NUM	(16)
#define	WRITE_DATA_NUM	(64)
#define	READ_DATA_NUM	(64)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	UINT32	size;
	UINT32	msg[4];
} T_MC_FIFO_MSG;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static UCHAR gWriteDataPos = 0;
static UCHAR g_Tx_Int_Flag[4];

// ch0->ch1
static T_MC_FIFO_MSG	gDD_MC_FIFO_Tx_Msg_01[] = {
// ASSIGN_LOGICAL_ADDRESS
	// ch1 Interface Device (LA=0x40)	DT 11 MI 5 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x40010000, 0x00000050}},	// 0
	// ch1 Generic Device (LA=0x42)	DT 11 MI 5 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x42010200, 0x00000050}},	// 1
	// ch0 Interface Device (LA=0x30)	DT 11 MI 14 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x30000000, 0x000000D0}},	// 2
	// ch0 Generic Device (LA=0x32)	DT 11 MI 7 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x32000200, 0x00000070}},	// 3
	// ch0 Framer Device (LA=0x31)	DT 11 MI 8 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x31000100, 0x00000080}},	// 4
// REQUEST_SELF_ANNOUNCEMENT
	{0x8,  {0x0C03FFF3, 0x00000035, 0x00000000, 0x00000000}},	// 5
// BEGIN_RECONFIGURATION
	{0x8,  {0x4003FFF3, 0x00000035, 0x00000000, 0x00000000}},	// 6
// NEXT_ROOT_FREQUENCY
	{0x8,  {0x4704FFF3, 0x00300133, 0x00000000, 0x00000000}},	// 7
// NEXT_CLOCK_GEAR
	{0x8,  {0x4604FFF3, 0x00B00936, 0x00000000, 0x00000000}},	// 8
// NEXT_SUBFRAME_MODE
	{0x8,  {0x4504FFF3, 0x00C01339, 0x00000000, 0x00000000}},	// 9
// CONNECT_SOURCE
	{0xc,  {0x1006FFF3, 0x1B00320C, 0x000000A0, 0x00000000}},	// 10
// CONNECT_SINK
	{0xc,  {0x1106FFF3, 0x1B004209, 0x000000F0, 0x00000000}},	// 11
// NEXT_DEFINE_CHANNEL	CN 27 TP 1 SD 0xC46 SL 9
	{0xc,  {0x5007FFF3, 0x1C461B39, 0x00009009, 0x00000000}},	// 12
// NEXT_DEFINE_CONTENT	CN 27 FL 0 PR 17 AF 0 DT 1 CL 1 DL 8
	{0xc,  {0x5107FFF3, 0x01111B37, 0x00005008, 0x00000000}},	// 13	DL=8 32bit transfer
// ACTIVATE_CHANNEL
	{0x8,  {0x5404FFF3, 0x00B01B33, 0x00000000, 0x00000000}},	// 14
// RECONFIGURE_NOW
	{0x8,  {0x5F03FFF3, 0x0000003C, 0x00000000, 0x00000000}},	// 15
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// DISCONNECT_PORT(SOURCE)
	{0xc,  {0x1405FFF3, 0xA000420D, 0x00000000, 0x00000000}},	// 16
// DISCONNECT_PORT(SINK)
	{0xc,  {0x1405FFF3, 0xB000320D, 0x00000000, 0x00000000}},	// 17
	{0x0,  {0x00000000, 0x00000000, 0x00000000, 0x00000000}},	// terminator
};

// ch2->ch3
static T_MC_FIFO_MSG	gDD_MC_FIFO_Tx_Msg_23[] = {
// ASSIGN_LOGICAL_ADDRESS
	// ch3 Interface Device (LA=0x40)	DT 11 MI 5 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x40010000, 0x00000050}},	// 0
	// ch3 Generic Device (LA=0x42)	DT 11 MI 5 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x42010200, 0x00000050}},	// 1
	// ch2 Interface Device (LA=0x30)	DT 11 MI 14 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x30000000, 0x000000D0}},	// 2
	// ch2 Generic Device (LA=0x32)	DT 11 MI 7 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x32000200, 0x00000070}},	// 3
	// ch2 Framer Device (LA=0x31)	DT 11 MI 8 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x31000100, 0x00000080}},	// 4
// REQUEST_SELF_ANNOUNCEMENT
	{0x8,  {0x0C03FFF3, 0x00000035, 0x00000000, 0x00000000}},	// 5
// BEGIN_RECONFIGURATION
	{0x8,  {0x4003FFF3, 0x00000035, 0x00000000, 0x00000000}},	// 6
// NEXT_ROOT_FREQUENCY
	{0x8,  {0x4704FFF3, 0x00300133, 0x00000000, 0x00000000}},	// 7
// NEXT_CLOCK_GEAR
	{0x8,  {0x4604FFF3, 0x00B00936, 0x00000000, 0x00000000}},	// 8
// NEXT_SUBFRAME_MODE
	{0x8,  {0x4504FFF3, 0x00C01339, 0x00000000, 0x00000000}},	// 9
// CONNECT_SOURCE
	{0xc,  {0x1006FFF3, 0x1B00320C, 0x000000A0, 0x00000000}},	// 10
// CONNECT_SINK
	{0xc,  {0x1106FFF3, 0x1B004209, 0x000000F0, 0x00000000}},	// 11
// NEXT_DEFINE_CHANNEL	CN 27 TP 1 SD 0xC46 SL 9
	{0xc,  {0x5007FFF3, 0x1C461B39, 0x00009009, 0x00000000}},	// 12
// NEXT_DEFINE_CONTENT	CN 27 FL 0 PR 17 AF 0 DT 1 CL 1 DL 8
	{0xc,  {0x5107FFF3, 0x01111B3F, 0x00005008, 0x00000000}},	// 13	DL=8 32bit transfer
// ACTIVATE_CHANNEL
	{0x8,  {0x5404FFF3, 0x00B01B33, 0x00000000, 0x00000000}},	// 14
// RECONFIGURE_NOW
	{0x8,  {0x5F03FFF3, 0x0000003C, 0x00000000, 0x00000000}},	// 15
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// DISCONNECT_PORT(SOURCE)
//	{0xc,  {0xA003120D, 0xA000420D, 0x00000000, 0x00000000}},	// 16
	{0xc,  {0x1405FFF3, 0xA000420D, 0x00000000, 0x00000000}},	// 16
// DISCONNECT_PORT(SINK)
	{0xc,  {0x1405FFF3, 0xB000320D, 0x00000000, 0x00000000}},	// 17
	{0x0,  {0x00000000, 0x00000000, 0x00000000, 0x00000000}},	// terminator
};

static const UINT32	gDD_Write_Data[WRITE_DATA_NUM] =
{
	0x12345678,0x23456789,0x3456789a,0x456789ab,	// 0
	0x55555555,0x66666666,0x77777777,0x88888888,	// 1
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 2
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 3
	0x11111111,0x22222222,0x33333333,0x44444444,	// 4
	0x55555555,0x66666666,0x77777777,0x88888888,	// 5
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 6
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 7
	0x11111111,0x22222222,0x33333333,0x44444444,	// 8
	0x55555555,0x66666666,0x77777777,0x88888888,	// 9
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 10
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 11
	0x11111111,0x22222222,0x33333333,0x44444444,	// 12
	0x55555555,0x66666666,0x77777777,0x88888888,	// 13
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 14
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 15
};
static UINT32	gDD_Write_Tmp_Data[WRITE_DATA_NUM] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));

static UINT32	gDD_Read_Data[READ_DATA_NUM] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static INT32 ct_dd_slimbus_read_msg_rx_fifo(E_DD_SLIMBUS_CH ch)
{
	INT32	ret;
	UINT32	size;
	UINT32	rx_msg[16];
	UCHAR*	s;
	UCHAR	loop, i;

	memset(rx_msg, 0, sizeof(rx_msg));
	ret =Dd_Slimbus_Read_Msg(ch, rx_msg, &size);
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_Slimbus_Read_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		return ret;
	}

	loop = (size >> 2);
	if(size & 0x3){
		loop++;
	}
	Ddim_Print(("-- Read Message size:%d(byte) --\n", size));
	for(i=0;i<loop;i++){
		Ddim_Print(("MC_FIFO[%d]=0x%08X\n", i, rx_msg[i]));
	}
	s = (UCHAR*)rx_msg;
#if 0
	Ddim_Print(("=====================\n"));
	for (i = 0; i < size; i ++){
		Ddim_Print(("data[%d]=0x%02X\n", i, *(s + i)));
	}
	Ddim_Print(("=====================\n"));
#endif

	if (*(s + 3) == 0x00) {
		Ddim_Print(("REPORT_PRESENT\n"));
		if (*(s + 4) == 0x00 && *(s + 5) == 0x02 && *(s + 6) == 0x00) {
			// ch0 Generic Device (LA=0x32)
			gDD_MC_FIFO_Tx_Msg_01[3].msg[1] &= 0x000000FF;
			gDD_MC_FIFO_Tx_Msg_01[3].msg[1] += ((*(s + 2) << 8) + *(s + 1)) << 8;
			Ddim_Print(("ch%d Generic Device\n", ch));
		}
		if (*(s + 4) == 0x00 && *(s + 5) == 0x00 && *(s + 6) == 0x00) {
			// ch0 Interface Device (LA=0x30)
			gDD_MC_FIFO_Tx_Msg_01[2].msg[1] &= 0x000000FF;
			gDD_MC_FIFO_Tx_Msg_01[2].msg[1] += ((*(s + 2) << 8) + *(s + 1)) << 8;
			Ddim_Print(("ch%d Interface Device\n", ch));
		}
		if (*(s + 4) == 0x00 && *(s + 5) == 0x00 && *(s + 6) == 0x01) {
			// ch1 Interface Device (LA=0x40)
			gDD_MC_FIFO_Tx_Msg_01[0].msg[1] &= 0x000000FF;
			gDD_MC_FIFO_Tx_Msg_01[0].msg[1] += ((*(s + 2) << 8) + *(s + 1)) << 8;
			Ddim_Print(("ch%d Interface Device\n", ch+1));
		}
		if (*(s + 4) == 0x00 && *(s + 5) == 0x01 && *(s + 6) == 0x00) {
			// ch0 Framer Device (LA=0x31)
			gDD_MC_FIFO_Tx_Msg_01[4].msg[1] &= 0x000000FF;
			gDD_MC_FIFO_Tx_Msg_01[4].msg[1] += ((*(s + 2) << 8) + *(s + 1)) << 8;
			Ddim_Print(("ch%d Framer Device\n", ch));
		}
		if (*(s + 4) == 0x00 && *(s + 5) == 0x02 && *(s + 6) == 0x01) {
			// ch1 Generic Device (LA=0x42)
			gDD_MC_FIFO_Tx_Msg_01[1].msg[1] &= 0x000000FF;
			gDD_MC_FIFO_Tx_Msg_01[1].msg[1] += ((*(s + 2) << 8) + *(s + 1)) << 8;
			Ddim_Print(("ch%d Generic Device\n", ch+1));
		}
	}

	return D_DDIM_OK;
}

static VOID ct_dd_slimbus_manager_int_cb(UCHAR ch, INT32 status)
{
	Ddim_Print(("ct_dd_slimbus_manager_int_cb called. ch=%d, Status=0x%08X\n", ch, status));

	if (status & 0x80) {	// 7 PORT_INT R
		Ddim_Print(("\t PORT_INT\n"));
	}
	if (status & 0x40) {	// 6 MCH_INT R/C
		Ddim_Print(("\t MCH_INT\n"));
	}
	if (status & 0x20) {	// 5 RCFG_INT R/C
		Ddim_Print(("\t RCFG_INT\n"));
	}
	if (status & 0x10) {	// 4 SYNC_LOST R/C
		Ddim_Print(("\t SYNC_LOST\n"));
	}
	if (status & 0x08) {	// 3 TX_ERR R/C
		Ddim_Print(("\t TX_ERR\n"));
	}
	if (status & 0x04) {	// 2 TX_INT R/C
		Ddim_Print(("\t TX_INT\n"));
	}
	if (status & 0x02) {	// 1 RX_INT R/C
		Ddim_Print(("\t RX_INT\n"));
	}
	if(status & 0x00000002){	// RX_INT
		ct_dd_slimbus_read_msg_rx_fifo(ch);
	}
	if(status & 0x00000004){	// TX_INT
		g_Tx_Int_Flag[ch] = 1;
	}
}

static VOID ct_dd_slimbus_data_port_int_cb(UCHAR ch, INT32 status)
{
	Ddim_Print(("ct_dd_slimbus_data_port_int_cb called. ch=%d, Status=0x%08X\n", ch, status));
	if (status & 0x20) {	// 5 UND_INT R/C
		Ddim_Print(("\t UND_INT\n"));
	}
	if (status & 0x10) {	// 4 OVF_INT R/C
		Ddim_Print(("\t OVF_INT\n"));
	}
	if (status & 0x08) {	// 3 DMA_INT R/C
		Ddim_Print(("\t DMA_INT\n"));
	}
	if (status & 0x04) {	// 2 CHAN_INT R/C
		Ddim_Print(("\t CHAN_INT\n"));
	}
	if (status & 0x02) {	// 1 CON_INT R/C
		Ddim_Print(("\t CON_INT\n"));
	}
	if (status & 0x01) {	// 0 ACT_INT R/C
		Ddim_Print(("\t ACT_INT\n"));
	}
#if 0
{
	int i;

	Ddim_Print(("=====================\n"));
	for (i = 0; i < WRITE_DATA_NUM; i ++) {
		Ddim_Print(("gDD_Write_Data[%d]=0x%08X\n", i, gDD_Write_Data[i]));
	}
	Ddim_Print(("=====================\n"));

	for (i = 0; i < READ_DATA_NUM; i ++) {
		Ddim_Print(("gDD_Read_Data[%d]=0x%08X\n", i, gDD_Read_Data[i]));
	}
	Ddim_Print(("=====================\n"));
}
#endif
}

static VOID ct_dd_slimbus_enumeration(E_DD_SLIMBUS_CH ch, BOOL dir)
{
	INT32			ret;
	UCHAR			index;
	DDIM_USER_ER	ercd;
	UINT32			msg;
	UINT32			la;
	T_MC_FIFO_MSG	t_msg;

	for(index=0 ; index<CONFIG_MSG_NUM ; index++){
		g_Tx_Int_Flag[ch] = 0;
		if (ch == E_DD_SLIMBUS_CH0 || ch == E_DD_SLIMBUS_CH1) {
			t_msg = gDD_MC_FIFO_Tx_Msg_01[index];
			if(dir == TRUE){	// sink -> source
				if(index==10){		// CONNECT_SOURCE

//					Ddim_Print(("B:t_msg.size = 0x%08X\n", t_msg.size));
//					Ddim_Print(("B:t_msg.msg[0] = 0x%08X\n", t_msg.msg[0]));
//					Ddim_Print(("B:t_msg.msg[1] = 0x%08X\n", t_msg.msg[1]));
//					Ddim_Print(("B:t_msg.msg[2] = 0x%08X\n", t_msg.msg[2]));
//					Ddim_Print(("B:t_msg.msg[3] = 0x%08X\n", t_msg.msg[3]));

					msg = gDD_MC_FIFO_Tx_Msg_01[index+1].msg[1];
					la = msg & 0x0000FF00;
					t_msg.msg[1] &= 0xFFFF00FF;
					t_msg.msg[1] |= la;

					Ddim_Print(("A:t_msg.size = 0x%08X\n", t_msg.size));
					Ddim_Print(("A:t_msg.msg[0] = 0x%08X\n", t_msg.msg[0]));
					Ddim_Print(("A:t_msg.msg[1] = 0x%08X\n", t_msg.msg[1]));
					Ddim_Print(("A:t_msg.msg[2] = 0x%08X\n", t_msg.msg[2]));
					Ddim_Print(("A:t_msg.msg[3] = 0x%08X\n", t_msg.msg[3]));
				}
				else if(index==11){	// CONNECT_SINK

//					Ddim_Print(("B:t_msg.size = 0x%08X\n", t_msg.size));
//					Ddim_Print(("B:t_msg.msg[0] = 0x%08X\n", t_msg.msg[0]));
//					Ddim_Print(("B:t_msg.msg[1] = 0x%08X\n", t_msg.msg[1]));
//					Ddim_Print(("B:t_msg.msg[2] = 0x%08X\n", t_msg.msg[2]));
//					Ddim_Print(("B:t_msg.msg[3] = 0x%08X\n", t_msg.msg[3]));

					msg = gDD_MC_FIFO_Tx_Msg_01[index-1].msg[1];
					la = msg & 0x0000FF00;
					t_msg.msg[1] &= 0xFFFF00FF;
					t_msg.msg[1] |= la;

					Ddim_Print(("A:t_msg.size = 0x%08X\n", t_msg.size));
					Ddim_Print(("A:t_msg.msg[0] = 0x%08X\n", t_msg.msg[0]));
					Ddim_Print(("A:t_msg.msg[1] = 0x%08X\n", t_msg.msg[1]));
					Ddim_Print(("A:t_msg.msg[2] = 0x%08X\n", t_msg.msg[2]));
					Ddim_Print(("A:t_msg.msg[3] = 0x%08X\n", t_msg.msg[3]));
				}

				ret = Dd_Slimbus_Write_Msg(ch, &t_msg.msg[0], t_msg.size);
				if(ret != D_DDIM_OK){
					Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
				}
			}
			else{
				if (index < 5) {
					Ddim_Print(("A:t_msg.size = 0x%08X\n", t_msg.size));
					Ddim_Print(("A:t_msg.msg[0] = 0x%08X\n", t_msg.msg[0]));
					Ddim_Print(("A:t_msg.msg[1] = 0x%08X\n", t_msg.msg[1]));
					Ddim_Print(("A:t_msg.msg[2] = 0x%08X\n", t_msg.msg[2]));
					Ddim_Print(("A:t_msg.msg[3] = 0x%08X\n", t_msg.msg[3]));
				}
				ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg_01[index].msg[0], gDD_MC_FIFO_Tx_Msg_01[index].size);
				if(ret != D_DDIM_OK){
					Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
				}
			}
		}
		else {	// sink -> source
			t_msg = gDD_MC_FIFO_Tx_Msg_23[index];
			if(dir == TRUE){	// sink -> source
				if(index==10){		// CONNECT_SOURCE
					msg = gDD_MC_FIFO_Tx_Msg_23[index+1].msg[1];
					la = msg & 0x0000FF00;
					t_msg.msg[1] &= 0xFFFF00FF;
					t_msg.msg[1] |= la;
				}
				else if(index==11){	// CONNECT_SINK
					msg = gDD_MC_FIFO_Tx_Msg_23[index-1].msg[1];
					la = msg & 0x0000FF00;
					t_msg.msg[1] &= 0xFFFF00FF;
					t_msg.msg[1] |= la;
				}
				ret = Dd_Slimbus_Write_Msg(ch, &t_msg.msg[0], t_msg.size);
				if(ret != D_DDIM_OK){
					Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
				}
			}
			else{
				if (index < 5) {
					Ddim_Print(("A:t_msg.size = 0x%08X\n", t_msg.size));
					Ddim_Print(("A:t_msg.msg[0] = 0x%08X\n", t_msg.msg[0]));
					Ddim_Print(("A:t_msg.msg[1] = 0x%08X\n", t_msg.msg[1]));
					Ddim_Print(("A:t_msg.msg[2] = 0x%08X\n", t_msg.msg[2]));
					Ddim_Print(("A:t_msg.msg[3] = 0x%08X\n", t_msg.msg[3]));
				}
				ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg_23[index].msg[0], gDD_MC_FIFO_Tx_Msg_23[index].size);
				if(ret != D_DDIM_OK){
					Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
				}
			}
		}

		while(1){
			ercd = DDIM_User_Dly_Tsk(1);	// 1ms wait
			if (ercd != D_DDIM_USER_E_OK){
				Ddim_Print(("DDIM_User_Dly_Tsk Error. ercd = %d\n", ercd));
			}
			if(g_Tx_Int_Flag[ch] == 1){
				break;
			}
		}
	}
}

static VOID ct_dd_slimbus_disconnect(E_DD_SLIMBUS_CH ch)
{
	INT32			ret;
	UCHAR			index;
	DDIM_USER_ER	ercd;

	for(index=CONFIG_MSG_NUM ; index<CONFIG_MSG_NUM+2 ; index++){
		g_Tx_Int_Flag[ch] = 0;
		if (ch == E_DD_SLIMBUS_CH0 || ch == E_DD_SLIMBUS_CH1) {
			ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg_01[index].msg[0], gDD_MC_FIFO_Tx_Msg_01[index].size);
		}
		else {	// sink -> source
			ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg_23[index].msg[0], gDD_MC_FIFO_Tx_Msg_23[index].size);
		}
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		}

		while(1){
			ercd = DDIM_User_Dly_Tsk(1);	// 1ms wait
			if (ercd != D_DDIM_USER_E_OK){
				Ddim_Print(("DDIM_User_Dly_Tsk Error. ercd = %d\n", ercd));
			}
			if(g_Tx_Int_Flag[ch] == 1){
				break;
			}
		}
	}
}

#if 0
static VOID ct_dd_slimbus_enumeration_1(E_DD_SLIMBUS_CH ch)
{
	INT32			ret;
	UCHAR			index;
	DDIM_USER_ER	ercd;

	for(index=0 ; index<15 ; index++){
		g_Tx_Int_Flag[ch] = 0;
		ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg[index].msg[0], gDD_MC_FIFO_Tx_Msg[index].size);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		}

		while(1){
			ercd = DDIM_User_Dly_Tsk(1);	// 1ms wait
			if (ercd != D_DDIM_USER_E_OK){
				Ddim_Print(("DDIM_User_Dly_Tsk Error. ercd = %d\n", ercd));
			}
			if(g_Tx_Int_Flag[ch] == 1){
				break;
			}
		}
	}
}
static VOID ct_dd_slimbus_enumeration_2(E_DD_SLIMBUS_CH ch)
{
	INT32			ret;
	UCHAR			index;
	DDIM_USER_ER	ercd;

	for(index=15 ; index<18 ; index++){
		g_Tx_Int_Flag[ch] = 0;
		ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg[index].msg[0], gDD_MC_FIFO_Tx_Msg[index].size);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		}

		while(1){
			ercd = DDIM_User_Dly_Tsk(1);	// 1ms wait
			if (ercd != D_DDIM_USER_E_OK){
				Ddim_Print(("DDIM_User_Dly_Tsk Error. ercd = %d\n", ercd));
			}
			if(g_Tx_Int_Flag[ch] == 1){
				break;
			}
		}
	}
}
#endif

static VOID ct_dd_slimbus_dma_int_write_cb(UCHAR dma_ch, INT32 status)
{
	Ddim_Print(("ct_dd_slimbus_dma_int_write_cb called. dma_ch=%d, Status=0x%08X\n", dma_ch, status));
#if 1
{
	int i;

	Ddim_Print(("=====================\n"));
	for (i = 0; i < WRITE_DATA_NUM; i ++) {
		Ddim_Print(("gDD_Write_Data[%d]=0x%08X\n", i, gDD_Write_Data[i]));
	}
	Ddim_Print(("=====================\n"));
}
#endif
}

static VOID ct_dd_slimbus_dma_int_read_cb(UCHAR dma_ch, INT32 status)
{
	Ddim_Print(("ct_dd_slimbus_dma_int_read_cb called. dma_ch=%d, Status=0x%08X\n", dma_ch, status));
#if 1
{
	int i;

	Ddim_Print(("=====================\n"));
	for (i = 0; i < READ_DATA_NUM; i ++) {
		Ddim_Print(("gDD_Read_Data[%d]=0x%08X\n", i, gDD_Read_Data[i]));
	}
	Ddim_Print(("=====================\n"));
}
#endif
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief	Command main function for PCIe test.
 *	@param	int argc	:The number of parameters
 *			char** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| P1	| P2	| P3 	| P4	| P5	| P6	| P7	| P8	| P9	| P10	| P11	| Meaning										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| open	| ch	| tout	|		|		|		|		|		|		|		|		| SLIMBus open									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| close	| ch	|		|		|		|		|		|		|		|		|		| SLIMBus close									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| ch	| mgr_en| fr_en	|m_i_cb	|dp_i_cb|		|		|		|		|		| Set SLIMBus Control Data						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| start	| ch	|		|		|		|		|		|		|		|		|		| Start SLIMBus									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| stop	| ch	|		|		|		|		|		|		|		|		|		| Stop SLIMBus									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| data	| msgNo	|data	|		|		|		|		|		|		|		|		| Set Message for Debug							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| disc	| ch	|		|		|		|		|		|		|		|		|		| Disconnect SLIMBus							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| r_msg	| ch	|		|		|		|		|		|		|		|		|		| Read message from RX_FIFO						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| w_msg	| ch	|		|		|		|		|		|		|		|		|		| Write message to TX_FIFO						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| reg	| offset| regdat|		|		|		|		|		|		|		| Set SLIMBus Register							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| reg	| offset|		|		|		|		|		|		|		|		| Get SLIMBus Register							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	tout	: 0(Wait by Polling), 1~n(timeout sec)
 *	ch		: 0(ch0), 1(ch1), 2(ch2), 3(ch3)
 *	mger_en	: 0(Manager mode disable), 1(Manager mode enable)
 *	fr_en	: 0(framer disable), 1(framer enable)
 *	m_i_cb	: 0(manager interrupt callback disable), 1(manager interrupt callback enable)
 *	dp_i_cb	: 0(data port interrupt callback disable), 1(data port interrupt callback enable)
 *	offset	: SLIMBus Register Offset (Hex String)
 *	regdat	: SLIMBus Register Value (Hex String)
 *
 *	@return VOID
 */

VOID Ct_Dd_Slimbus_Main(int argc, char** argv)
{
	INT32				ret=0;
	E_DD_SLIMBUS_CH		ch;
	T_DD_SLIMBUS_CTRL	slimbus_ctrl;
	USHORT				offset;
	UINT32				data;
	UINT32				index;
	UCHAR				dma_ch;
	UINT32				count;

	if(strcmp(argv[1], "open") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ret = Dd_Slimbus_Open(ch, (INT32)atoi(argv[3]));
		Ddim_Print(("Dd_Slimbus_Open(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "close") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ret = Dd_Slimbus_Close(ch);
		Ddim_Print(("Dd_Slimbus_Close(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "ctrl") == 0){
		if (strcmp(argv[2], "err") != 0) {
			slimbus_ctrl.ch				= (E_DD_SLIMBUS_CH)atoi(argv[2]);
			slimbus_ctrl.manager_mode	= (E_DD_SLIMBUS_MANAGER_MODE)atoi(argv[3]);
			slimbus_ctrl.fr_en			= (E_DD_SLIMBUS_FR_EN)atoi(argv[4]);
			slimbus_ctrl.src_thr		= (UCHAR)atoi(argv[5]);	// source threshold
			slimbus_ctrl.sink_thr		= (UCHAR)atoi(argv[6]);	// sink threshold
			if(strcmp(argv[7], "1") == 0){
				slimbus_ctrl.manager_int_cb = (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_manager_int_cb;
			}
			else{
				slimbus_ctrl.manager_int_cb = NULL;
			}
			if(strcmp(argv[8], "1") == 0){
				slimbus_ctrl.data_port_int_cb = (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_data_port_int_cb;
			}
			else{
				slimbus_ctrl.data_port_int_cb = NULL;
			}
			ret = Dd_Slimbus_Ctrl(&slimbus_ctrl);
			Ddim_Print(("Dd_Slimbus_Ctrl(%d) completed. Return Value=0x%08X\n", slimbus_ctrl.ch, ret));
		}
		else {
			ret = Dd_Slimbus_Ctrl(NULL);
			Ddim_Print(("Dd_Slimbus_Ctrl() completed. Return Value=0x%08X\n", ret));
		}
	}
	else if(strcmp(argv[1], "start") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ret = Dd_Slimbus_Start(ch);
		Ddim_Print(("Dd_Slimbus_Start(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "stop") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ret = Dd_Slimbus_Stop(ch);
		Ddim_Print(("Dd_Slimbus_Stop(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "enu_sta") == 0){	// enumeration
		BOOL dir;
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		dir = (BOOL)atoi(argv[3]);
		ct_dd_slimbus_enumeration(ch, dir);
		Ddim_Print(("enu_sta(%d) completed.\n", ch));
	}
#if 0
	else if(strcmp(argv[1], "enu_sta1") == 0){	// enumeration
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ct_dd_slimbus_enumeration_1(ch);
		Ddim_Print(("enu_sta(%d) completed.\n", ch));
	}
	else if(strcmp(argv[1], "enu_sta2") == 0){	// enumeration
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ct_dd_slimbus_enumeration_2(ch);
		Ddim_Print(("enu_sta(%d) completed.\n", ch));
	}
	else if(strcmp(argv[1], "r_msg") == 0){
		if (strcmp(argv[2], "err") != 0) {
			ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
			ret = ct_dd_slimbus_read_msg_rx_fifo(ch);
			Ddim_Print(("Dd_Slimbus_Read_Msg(%d) completed. Return Value=0x%08X\n", ch, ret));
		}
		else {
			ret =Dd_Slimbus_Read_Msg(0, NULL, &size);
			Ddim_Print(("Dd_Slimbus_Read_Msg(%d) completed. Return Value=0x%08X\n", 0, ret));
			ret =Dd_Slimbus_Read_Msg(0, rx_msg, &NULL);
			Ddim_Print(("Dd_Slimbus_Read_Msg(%d) completed. Return Value=0x%08X\n", 0, ret));
		}
	}
#else
	else if(strcmp(argv[1], "r_msg") == 0){
		if (strcmp(argv[2], "err") == 0) {
			UINT32	size;
			UINT32	rx_msg[16];

			ret =Dd_Slimbus_Read_Msg(0, NULL, &size);
			Ddim_Print(("Dd_Slimbus_Read_Msg(%d) completed. Return Value=0x%08X\n", 0, ret));
			ret =Dd_Slimbus_Read_Msg(0, rx_msg, NULL);
			Ddim_Print(("Dd_Slimbus_Read_Msg(%d) completed. Return Value=0x%08X\n", 0, ret));
		}
	}
#endif
	else if(strcmp(argv[1], "data") == 0){	// disconnect
		index = 0;
		while (gDD_MC_FIFO_Tx_Msg_01[index].size != 0) {
			index ++;
		}
		count = atoi(argv[2]);
		if (count < index){
			data = strtol(argv[3], NULL, 0);
			gDD_MC_FIFO_Tx_Msg_01[count].msg[1] = data;		/* pgr0539 */
			Ddim_Print(("msg[0]= (0x%08x) .\n", gDD_MC_FIFO_Tx_Msg_01[count].msg[0]));
			Ddim_Print(("msg[1]= (0x%08x) .\n", gDD_MC_FIFO_Tx_Msg_01[count].msg[1]));
			Ddim_Print(("data (0x%08x) completed.\n", data));
		}
	}
	else if(strcmp(argv[1], "disc") == 0){	// disconnect

		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		ct_dd_slimbus_disconnect(ch);
		Ddim_Print(("disconnect(%d) completed.\n", ch));
	}
	else if(strcmp(argv[1], "w_cnt") == 0){	// next write position

		gWriteDataPos = (UCHAR)atoi(argv[2]);
		Ddim_Print(("next write position(%d) completed.\n", gWriteDataPos));
	}
	else if(strcmp(argv[1], "w_msg") == 0){
		if (strcmp(argv[2], "err") != 0) {
			ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
			index = atoi(argv[3]);
			if(ch == E_DD_SLIMBUS_CH0 || ch == E_DD_SLIMBUS_CH1){
				ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg_01[index].msg[0], gDD_MC_FIFO_Tx_Msg_01[index].size);
			}
			else{
				ret = Dd_Slimbus_Write_Msg(ch, &gDD_MC_FIFO_Tx_Msg_23[index].msg[0], gDD_MC_FIFO_Tx_Msg_23[index].size);
			}
			Ddim_Print(("Dd_Slimbus_Write_Msg(%d) completed. Return Value=0x%08X\n", ch, ret));
		}
		else {
			ret = Dd_Slimbus_Write_Msg(E_DD_SLIMBUS_CH0, NULL, gDD_MC_FIFO_Tx_Msg_01[0].size);
			Ddim_Print(("Dd_Slimbus_Write_Msg(%d) completed. Return Value=0x%08X\n", 0, ret));
			ret = Dd_Slimbus_Write_Msg(E_DD_SLIMBUS_CH0, &gDD_MC_FIFO_Tx_Msg_01[0].msg[0], 65);
			Ddim_Print(("Dd_Slimbus_Write_Msg(%d) completed. Return Value=0x%08X\n", 0, ret));
		}
	}
	else if(strcmp(argv[1], "w_dat") == 0){
		if (strcmp(argv[2], "err") != 0) {
			ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
			dma_ch = (UCHAR)atoi(argv[3]);
			count = (UINT32)atoi(argv[4]);
			ret = Dd_Slimbus_Write_Data(ch, dma_ch, (UINT32*)gDD_Write_Data, count, (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_dma_int_write_cb);
			Ddim_Print(("Dd_Slimbus_Write_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
		}
		else {
			ret = Dd_Slimbus_Write_Data(0, 3, NULL, 16, (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_dma_int_write_cb);
			Ddim_Print(("Dd_Slimbus_Write_Data(%d) completed. Return Value=0x%08X\n", 0, ret));
			ret = Dd_Slimbus_Write_Data(0, 3, (UINT32*)gDD_Write_Data, 16, NULL);
			Ddim_Print(("Dd_Slimbus_Write_Data(%d) completed. Return Value=0x%08X\n", 0, ret));
		}
	}
	else if(strcmp(argv[1], "w_dat2") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		dma_ch = (UCHAR)atoi(argv[3]);
		count = (UINT32)atoi(argv[4]);
		ret = Dd_Slimbus_Write_Data(ch, dma_ch, (UINT32*)gDD_Write_Data, count, NULL);
		Ddim_Print(("Dd_Slimbus_Write_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "w_dat3") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		dma_ch = (UCHAR)atoi(argv[3]);
		count = (UINT32)atoi(argv[4]);
		index = (UINT32)atoi(argv[5]);
		ret = Dd_Slimbus_Write_Data(ch, dma_ch, (UINT32*)&gDD_Write_Data[index], count, (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_dma_int_write_cb);
		Ddim_Print(("Dd_Slimbus_Write_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "w_datc") == 0){
		for (index = 0; index < WRITE_DATA_NUM; index ++) {
			gDD_Write_Tmp_Data[index % WRITE_DATA_NUM] = gDD_Write_Data[(index + gWriteDataPos) % WRITE_DATA_NUM];
		}
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		dma_ch = (UCHAR)atoi(argv[3]);
		count = (UINT32)atoi(argv[4]);
		ret = Dd_Slimbus_Write_Data(ch, dma_ch, (UINT32*)&gDD_Write_Tmp_Data[gWriteDataPos % WRITE_DATA_NUM], count, (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_dma_int_write_cb);
		Ddim_Print(("Dd_Slimbus_Write_Data(%d) position=%d completed. Return Value=0x%08X\n", ch, gWriteDataPos, ret));
	}
	else if(strcmp(argv[1], "r_dat") == 0){
		if (strcmp(argv[2], "err") != 0) {
			ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
			dma_ch = (UCHAR)atoi(argv[3]);
			count = (UINT32)atoi(argv[4]);
			memset(gDD_Read_Data, 0, sizeof(gDD_Read_Data));
			DDIM_User_L1l2cache_Clean_Flush_Addr((UINT32)gDD_Read_Data, sizeof(gDD_Read_Data));
			ret = Dd_Slimbus_Read_Data(ch, dma_ch, gDD_Read_Data, count, (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_dma_int_read_cb);
			Ddim_Print(("Dd_Slimbus_Read_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
		}
		else {
			ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
			ret = Dd_Slimbus_Read_Data(0, 3, NULL, 16, (VP_SLIMBUS_CALLBACK)ct_dd_slimbus_dma_int_read_cb);
			Ddim_Print(("Dd_Slimbus_Read_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
			ret = Dd_Slimbus_Read_Data(0, 3, gDD_Read_Data, 16, NULL);
			Ddim_Print(("Dd_Slimbus_Read_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
		}
	}
	else if(strcmp(argv[1], "r_dat2") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[2]);
		dma_ch = (UCHAR)atoi(argv[3]);
		count = (UINT32)atoi(argv[4]);
		memset(gDD_Read_Data, 0, sizeof(gDD_Read_Data));
		DDIM_User_L1l2cache_Clean_Flush_Addr((UINT32)gDD_Read_Data, sizeof(gDD_Read_Data));
		ret = Dd_Slimbus_Read_Data(ch, dma_ch, gDD_Read_Data, count, NULL);
		Ddim_Print(("Dd_Slimbus_Read_Data(%d) completed. Return Value=0x%08X\n", ch, ret));
	}
	else if(strcmp(argv[1], "set") == 0){
		if(strcmp(argv[2], "reg") == 0){
			ch = (E_DD_SLIMBUS_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			sscanf(argv[5], "%lx", (ULONG*)&data);
			ret = Dd_Slimbus_Set_Reg(ch, offset, data);
			Ddim_Print(("SLIMBus ch%d Register 0x%04X 0x%08X\n", ch, offset, data));
			Ddim_Print(("Dd_Slimbus_Set_Reg(%d) completed. Return Value=0x%08X\n", ch, ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "get") == 0){
		ch = (E_DD_SLIMBUS_CH)atoi(argv[3]);
		if(strcmp(argv[2], "ctrl") == 0){
			if (strcmp(argv[3], "err") != 0) {
				ret = Dd_Slimbus_Get_Ctrl(ch, &slimbus_ctrl);
				if(ret == D_DDIM_OK){
					Ddim_Print(("ch               : 0x%X\n", slimbus_ctrl.ch));
					Ddim_Print(("manager_mode     : 0x%X\n", slimbus_ctrl.manager_mode));
					Ddim_Print(("fr_en            : 0x%X\n", slimbus_ctrl.fr_en));
					Ddim_Print(("src_thr          : 0x%X\n", slimbus_ctrl.src_thr));
					Ddim_Print(("sink_thr         : 0x%X\n", slimbus_ctrl.sink_thr));
					Ddim_Print(("manager_int_cb   : 0x%08lX\n", (ULONG)slimbus_ctrl.manager_int_cb));
					Ddim_Print(("data_port_int_cb : 0x%08lX\n", (ULONG)slimbus_ctrl.data_port_int_cb));
				}
				Ddim_Print(("Dd_Slimbus_Get_Ctrl(%d) completed. Return Value=0x%08X\n", ch, ret));
			}
			else {
				ret = Dd_Slimbus_Get_Ctrl(0, NULL);
				Ddim_Print(("Dd_Slimbus_Get_Ctrl(0) completed. Return Value=0x%08X\n", ret));
			}
		}
		else if(strcmp(argv[2], "reg") == 0){
			if (strcmp(argv[3], "err") != 0) {
				sscanf(argv[4], "%lx", (ULONG*)&offset);
				ret = Dd_Slimbus_Get_Reg(ch, offset, &data);
				Ddim_Print(("SLIMBus ch%d Register 0x%04X 0x%08X\n", ch, offset, data));
				Ddim_Print(("Dd_Slimbus_Get_Reg(%d) completed. Return Value=0x%08X\n", ch, ret));
			}
			else {
				ret = Dd_Slimbus_Get_Reg(0, 0, NULL);
				Ddim_Print(("SLIMBus ch%d Register 0x%04X NULL\n", 0, 0));
				Ddim_Print(("Dd_Slimbus_Get_Reg(%d) completed. Return Value=0x%08X\n", 0, ret));
			}
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "err") == 0){
		if(Dd_Slimbus_Close(0) != D_DD_SLIMBUS_SEM_NG){
			Ddim_Print(("Dd_Slimbus_Close D_DD_SLIMBUS_SEM_NG Check NG.\n"));
			return;
		}

		Dd_Slimbus_Open(0, 0);
		if(Dd_Slimbus_Open(0, 20) != D_DD_SLIMBUS_SEM_TIMEOUT){
			Ddim_Print(("Dd_Slimbus_Open ch0 D_DD_SLIMBUS_SEM_TIMEOUT Check NG.\n"));
			return;
		}

		UINT32 data;
		if(Dd_Slimbus_Get_Reg(0, 0x0001, &data) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Get_Reg(0, 0x0014, &data) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Get_Reg(0, 0x0030, &data) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Get_Reg(0, 0x34, &data) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Get_Reg(0, 0x0084, &data) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Get_Reg(0, 0x00BC, &data) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Set_Reg(0, 0x00C4, 0x0) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Set_Reg(0, 0x00FC, 0x0) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Set_Reg(0, 0x0108, 0x0) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Set_Reg(0, 0x0FFC, 0x0) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Slimbus_Set_Reg(0, 0x1040, 0x0) != D_DD_SLIMBUS_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Slimbus_Get_Reg ch1 D_DD_SLIMBUS_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}

		Ddim_Print(("Error Check OK.\n"));
	}
	else{
		Ddim_Print(("please check parameter!!\n" ));
	}
}
