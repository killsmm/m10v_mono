/**
 * @file		USBHDC_Dev_EpControl.c
 * @brief		USB Device endpoint control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_Dev_EpControl.h"

#include "USBHDC_MemMap.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"

#include "USBHDC_Cmn_Control.h"
#include "USBHDC_Cmn_Otg.h"

#include "USBHDC_Dev_Api.h"
#include "USBHDC_Dev_Control.h"


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   set data for IN transaction.                                *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep               : endpoint number(0-15)      *
 *   unsigned char *p_in_trans_data : pointer of data buffer     *
 *   unsigned char packet_size      : in transaction packet size *
 *                                                               *
 *****************************************************************/
void Dev_SetInTransactionData(unsigned char ep, unsigned char *p_in_trans_data, unsigned short packet_size)
{
	unsigned short offset;

	/* set data size */
	Dev_SetEpWriteSize(ep, packet_size);

	/* write data */
	if (packet_size != 0) {
		/* get current endpoint offset */
		offset = Dev_GetEpBufferOffset(ep, IN);

		Cmn_WriteEpBuffer(offset, p_in_trans_data, packet_size);
	}

	/* set write comlete */
	Cmn_SetWriteComplete(ep);
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get data of OUT transaction.                                *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep                : endpoint number(0-15)     *
 *   unsigned char *p_out_trans_data : pointer of data buffer    *
 *   unsigned char packet_size       : out transaction packet size *
 *                                                               *
 *****************************************************************/
void Dev_GetOutTransactionData(unsigned char ep, unsigned char *p_out_trans_data, unsigned short packet_size)
{
	unsigned short offset;

	/* read data */
	if (packet_size != 0) {
		/* get current endpoint offset */
		offset = Dev_GetEpBufferOffset(ep, OUT);

		Cmn_ReadEpBuffer(offset, p_out_trans_data, packet_size);
	}

	/* set read comlete */
	Cmn_SetReadComplete(ep);
}


/*** EPCMD for USB Device ***/
/*****************************************************************
 * [ Function ]		set endpoint disable.                        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep        : endpoint number(0 - 15).      *
 *                  u8 wait_flag : 0 = time wait nothing.        *
 *                                 1 = time wait.                *
 *****************************************************************/
void Dev_SetEpDisable(u8 ep, u8 wait_flag)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set endpoint disable */
	WriteRegister(reg_epcmdx_addr, EPCMDx_STOP);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* Time wait. Hard spcification. */
	if (wait_flag) {
		/* high speed */
		if (Dev_GetBusSpeed() == 0) {
			/* wait 2micro frame(250micro sec) */
#ifdef USE_ON_THE_GO
			Cmn_WaitTime(25, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
			WaitTime(250);
#endif
		/* full & low speed */
		} else {
			/* wait 2frame (2mm sec) */
#ifdef USE_ON_THE_GO
			Cmn_WaitTime(200, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
			WaitTime(2000);
#endif
		}
	}
}


/*****************************************************************
 * [ Function ]		initialize endpoint.                         *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Dev_InitEp(u8 ep)
{
	u32 reg_epcmdx_addr;
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;
	u32 tmp;
	u8 ep_enable_flag = 0;

	/* calculate EPCTRLx register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* mask ready interrupt */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x40000);
	if ((reg_epctrlx_val & 0x00040000) != 0) {
		Dev_SetEpIntDis(ep, EPCMDx_READY_INTEN);
	}

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	if (Dev_IsEpEnable(ep) == 1) {
		ep_enable_flag = 1;
		/* ep disable */
//		Dev_SetEpDisable(ep, 0); /* change no wait 2010.08.19 */ /* change 2013.03.19 */
		Dev_SetEpDisable(ep, 1);
	}

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set initilization endpoint. */
	WriteRegister(reg_epcmdx_addr, EPCMDx_INIT);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* OUT transfer ready clear */
	if (ep == 0) {
		/* ready state clear */
		Dev_SetEpIntStateClr(ep, EPCMD0_READYO_INTCLR);
	} else if ((ep > 0) && ((reg_epctrlx_val & 0x8) == 0x0)) {
		/* ready state clear */
		Dev_SetEpIntStateClr(ep, EPCMDx_READY_INTCLR);
	} else {
		;
	}

	/* enpoint0 or endpointx IN transfer */
	if (ep == 0 || (ep > 0 && ((reg_epctrlx_val & 0x8) == 0x8))) {
		/* ready state clear */
		while(1) {
			tmp = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x4000000);
			if((tmp & 0x4000000) != 0) {
				Dev_SetEpIntStateClr(ep, EPCMDx_READY_INTCLR);
				break;
			}
		}
	}

	/* int enable ready interrupt */
	if ((reg_epctrlx_val & 0x00040000) != 0) {
		Dev_SetEpIntEn(ep, EPCMDx_READY_INTEN);
	}

	/* ep enable */
	if (ep_enable_flag == 1) {
		Cmn_SetEpEnable(ep);
	}
}


/*****************************************************************
 * [ Function ]		set endpoint stall.                          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Dev_SetEpStall(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set endpoint stall */
	WriteRegister(reg_epcmdx_addr, EPCMDx_STALL_SET);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		clear endpoint stall.                        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Dev_ClearEpStall(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* clear endpoint stall */
	WriteRegister(reg_epcmdx_addr, EPCMDx_STALL_CLR);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set null response.                           *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep  : endpoint number(1 - 15).            *
 *                  u8 val : 0 = normal response                 *
 *                           1 = null response every time        *
 *****************************************************************/
void Dev_SetNullResp(u8 ep, u8 val)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set write en */
	reg_epcmdx_val = EPCMDx_WRITE_EN;
	/* set null resp */
	if (val == 0) {
		reg_epcmdx_val &= ~(EPCMDx_NULLRESP);
	} else {
		reg_epcmdx_val |= EPCMDx_NULLRESP;
	}

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTRL */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xFFC03E);

	/* nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000400);
	/* inten */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000);
	/* et, dir, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0F800000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set nullresp */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set nack response.                           *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep  : endpoint number(1 - 15).            *
 *                  u8 val : 0 = normal response                 *
 *                           1 = nack response every time        *
 *****************************************************************/
void Dev_SetNackResp(u8 ep, u8 val)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set write en */
	reg_epcmdx_val = EPCMDx_WRITE_EN;
	/* set nackresp */
	if (val == 0) {
		reg_epcmdx_val &= ~(EPCMDx_NACKRESP);
	} else {
		reg_epcmdx_val |= EPCMDx_NACKRESP;
	}

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTRL */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xFFC03E);

	/* nullresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000200);
	/* inten */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000);
	/* et, dir, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0F800000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set nackresp */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set endpoint interrupt enable.               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep          : endpoint number(1 - 15).    *
 *                  u8 cmd_int_pos : bit position interrupt      *
 *****************************************************************/
void Dev_SetEpIntEn(u8 ep, u32 cmd_int_pos)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set inten */
	reg_epcmdx_val = EPCMDx_WRITE_EN;

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTR */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x00FFC03E);

	/* int en */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000) | cmd_int_pos;
	/* nullresp, nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000600);
	/* et, dir, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0F800000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int enable */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set endpoint interrupt disable.              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep          : endpoint number(1 - 15).    *
 *                  u8 cmd_int_pos : bit position interrupt      *
 *****************************************************************/
void Dev_SetEpIntDis(u8 ep, u32 cmd_int_pos)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set write en */
	reg_epcmdx_val = EPCMDx_WRITE_EN;

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTRL */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x00FFC03E);

	/* set int dis */
	reg_epcmdx_val |= (((reg_epctrlx_val >> 6) & 0x0001F000) & ~(cmd_int_pos));
	/* nullresp, nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000600);
	/* et, dir, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0F800000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int enable */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		clear endpoint interrupt state.              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep             : endpoint number(1 - 15)  *
 *                  u8 cmd_intclr_pos : bit position             *
 *                                      interrupt state          *
 *****************************************************************/
void Dev_SetEpIntStateClr(u8 ep, u32 cmd_intclr_pos)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int clear */
	WriteRegister(reg_epcmdx_addr, cmd_intclr_pos);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

}


/*****************************************************************
 * [ Function ]		set endpoint transfer type.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep            : endpoint number(1 - 15)   *
 *                  u8 transfer_type : transfer type.            *
 *                                     0=Control, 1=Iso          *
 *                                     2=Bulk, 3=Interrupt       *
 *****************************************************************/
void Dev_SetTransferType(u8 ep, u8 transfer_type)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set transfer type */
	reg_epcmdx_val = (EPCMDx_WRITE_EN | ((transfer_type << 23) & 0x01800000));

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTRL */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xFFC03E);
	/* nullresp, nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000600);
	/* inten */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000);
	/* dir, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0E000000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int enable */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set endpoint direction.                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep        : endpoint number(1 - 15)       *
 *                  u8 direction : endpoint direction.           *
 *                                 0=OUT, 1=IN                   *
 *****************************************************************/
void Dev_SetDirection(u8 ep, u8 direction)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set direction */
	reg_epcmdx_val = (EPCMDx_WRITE_EN | ((direction << 25) & 0x02000000));

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTRL */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xFFC03E);
	/* nullresp, nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000600);
	/* inten */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000);
	/* et, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0D800000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int enable */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set number of endpoint bufferes.             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep         : endpoint number(1 - 15)      *
 *                  u8 buffer_num : number of bufferes(0 - 3).   * 
 *****************************************************************/
void Dev_SetBufferNum(u8 ep, u8 buffer_num)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set buffer num */
	reg_epcmdx_val = (EPCMDx_WRITE_EN | ((buffer_num << 26) & 0x0C000000));

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* read EPCTRL  */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xFFC03E);
	/* nullresp, nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000600);
	/* inten */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000);
	/* et, direction */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x03800000);
	/* hiband */
	reg_epcmdx_val |= ((reg_epctrlx_val << 14) & 0x30000000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int enable */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		set endpoint hiband.                         *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep     : endpoint number(1 - 15)          *
 *                  u8 hiband : hiband.                          * 
 *****************************************************************/
void Dev_SetHiband(u8 ep, u8 hiband)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);
	/* set hiband */
	reg_epcmdx_val = (EPCMDx_WRITE_EN | ((hiband << 28) & 0x30000000));

	/* set current EPCTRLx val */
	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);
	/* EPCTRL read */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xFFC03E);
	/* nullresp, nackresp */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 7) & 0x00000600);
	/* inten */
	reg_epcmdx_val |= ((reg_epctrlx_val >> 6) & 0x0001F000);
	/* et,dir, bunm */
	reg_epcmdx_val |= ((reg_epctrlx_val << 22) & 0x0F800000);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set int enable */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		clear endpoint control.                      *
 * [ Return code ]	u8 : (0) endpoint command no busy.           *
 *                       (1) endpoint command busy.              *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Dev_ClearEpControl(u8 ep)
{
	u32 reg_epcmdx_addr;
	u32 reg_epcmdx_val = 0;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* init ep */
	Dev_InitEp(ep);

	/* set EPCMD reg for ep control clear */
	/* clear hiband, buffer, dir, et, int state, int en, toggle, stall, & ep disable */
	reg_epcmdx_val = 0x007C0942;

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set clear ep control */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* set EPCMD reg for ep control clear */
	/* clear hiband, buffer, dir, et, int state, int en, toggle, stall, & ep disable */
	reg_epcmdx_val = 0x007C0142;

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set clear ep control */
	WriteRegister(reg_epcmdx_addr, reg_epcmdx_val);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}




/*****************************************************************
 * [ Function ]		read EPCTRLx register.                       *
 * [ Return code ]	u32 : EPCTRLx value.                         *
 * [ Parameter ]	u32 reg_epctrlx_addr : EPCTRLx reg address   *
 *                  u32 check_bit        : verify check bit      *
 *****************************************************************/
u32 Dev_ReadEpctrlxReg(u32 reg_epctrlx_addr, u32 check_bit)
{
	volatile u32 reg_epctrlx_val1, reg_epctrlx_val2;

	/* read EPCTRLx register */
	while (1){
		/* read EPCTRLx register */
		reg_epctrlx_val1 = ReadRegister(reg_epctrlx_addr);
		reg_epctrlx_val2 = ReadRegister(reg_epctrlx_addr);

		if ((reg_epctrlx_val1 & check_bit) == (reg_epctrlx_val2 & check_bit)) {
			break;
		}
	}

	/* return endpoint enable or disable */
	return reg_epctrlx_val1;
}


/*****************************************************************
 * [ Function ]		inquire endpoint enable.                     *
 * [ Return code ]	u8 : (0) endpoint disable.                   *
 *                       (1) endpoint enable.                    *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_IsEpEnable(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x1);

	/* return endpoint enable or disable */
	return (u8)(reg_epctrlx_val & 0x1);
}



/*****************************************************************
 * [ Function ]		get endpoint transfer type.                  *
 * [ Return code ]	u8 : (0) control transfer.                   *
 *                       (1) isochronous transfer.               *
 *                       (2) bulk transfer.                      *
 *                       (3) interrupt transfer.                 *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_GetTransferType(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x6);

	/* return transfer type */
	return (u8)((reg_epctrlx_val >> 1) & 0x3);
}


/*****************************************************************
 * [ Function ]		get endpoint transfer direction.             *
 * [ Return code ]	u8 : (0) out direction.                      *
 *                       (1) in direction.                       *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_GetTransferDirection(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x8);

	/* return direction */
	return (u8)((reg_epctrlx_val >> 3) & 0x1);
}


/*****************************************************************
 * [ Function ]		get number of endpoint buffers.              *
 * [ Return code ]	u8 : (0) single buffer.                      *
 *                       (1) double buffer.                      *
 *                       (2) triple buffer.                      *
 *                       (3) quad buffer.                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_GetEpBufferNumber(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x30);

	/* return number of endpoint buffers */
	return (u8)((reg_epctrlx_val >> 4) & 0x3);
}


/*****************************************************************
 * [ Function ]		get current write endpoint buffer number.    *
 * [ Return code ]	u8 : (0) first buffer.                       *
 *                       (1) second buffer.                      *
 *                       (2) third buffer.                       *
 *                       (3) fourth buffer.                      *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_GetCurrentEpBuffer(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;
	u8 current_ep;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0xC0);

	/* return current endpoint buffer number(appptr) */
	current_ep = ((reg_epctrlx_val >> 6) & 0x3);

	return current_ep;
}


/*****************************************************************
 * [ Function ]		inquire endpoint buffer empty.               *
 * [ Return code ]	u8 : (0) endpoint buffer no empty.           *
 *                       (1) endpoint buffer empty.              *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_IsEpEmpty(u8 ep, u8 ep0_dir)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;
	u8 status = 0;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x400);

	/* endpoint 0 */
	if (ep == 0) {
		/* OUT direction */
		if (ep0_dir == OUT) {
			status = (reg_epctrlx_val >> 10) & 0x1;
		/* IN direction */
		} else {
			status = (reg_epctrlx_val >> 8) & 0x1;
		}
	/* endpoint 1 - 15 */
	} else {
		status = (reg_epctrlx_val >> 10) & 0x1;
	}

	/* empty */
	if (status == 1) {
		return HDC_TRUE;
	/* no empty */
	} else {
		return HDC_FALSE;
	}
}


/*****************************************************************
 * [ Function ]		inquire endpoint buffer full.                *
 * [ Return code ]	u8 : (0) endpoint buffer no full.            *
 *                       (1) endpoint buffer full.               *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_IsEpFull(u8 ep, u8 ep0_dir)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;
	u8 status = 0;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x800);

	/* endpoint 0 */
	if (ep == 0) {
		/* OUT direction */
		if (ep0_dir == OUT) {
			status = (reg_epctrlx_val >> 11) & 0x1;
		/* IN direction */
		} else {
			status = (reg_epctrlx_val >> 9) & 0x1;
		}
	/* endpoint 1 - 15 */
	} else {
		status = (reg_epctrlx_val >> 11) & 0x1;
	}

	/* full */
	if (status == 1) {
		return HDC_TRUE;
	/* no full */
	} else {
		return HDC_FALSE;
	}
}


/*****************************************************************
 * [ Function ]		inquire endpoint stall.                      *
 * [ Return code ]	u8 : (0) endpoint buffer no stall.           *
 *                       (1) endpoint buffer stall.              *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_IsEpStall(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x1000);

	/* return endpoint stall or no stall */
	return (u8)((reg_epctrlx_val >> 12) & 0x1);
}

#if 0
/*****************************************************************
 * [ Function ]		get toggle bit.                              *
 * [ Return code ]	u8 : (0) toggle bit 0.                       *
 *                       (1) toggle bit 1.                       *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Dev_GetToggleBit(u8 ep)
{
	u32 reg_epctrlx_addr;
	u32 reg_epctrlx_val;

	/* calculate register address */
	reg_epctrlx_addr = EPCTRL0 + (ep * 4);

	/* read EPCTRLx register */
	reg_epctrlx_val = Dev_ReadEpctrlxReg(reg_epctrlx_addr, 0x2000);

	/* return toggle bit */
	return (u8)((reg_epctrlx_val >> 13) & 0x1);
}
#endif

/*****************************************************************
 * [ Function ]		get endpoint buffer size.                    *
 * [ Return code ]	u16 : endpoint buffer size.                  *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u16 Dev_GetEpBufferSize(u8 ep)
{
	u32 reg_epconfx_addr;
	u32 reg_epconfx_val;

	/* calculate register address */
	reg_epconfx_addr = EPCONF0 + (ep * 4);

	/* read EPCONFx register */
	reg_epconfx_val = ReadRegister(reg_epconfx_addr);

	/* return endpoint buffer size */
	return ((reg_epconfx_val >> 13) & 0x7FF);
}



/*****************************************************************
 * [ Function ]		get current endpoint buffer offset.          *
 * [ Return code ]	u16 : current endpoint buffer offset.        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *                  u8 dir : endpoint direction. 0 = OUT, 1 = IN *
 *****************************************************************/
u16 Dev_GetEpBufferOffset(u8 ep, u8 dir) 
{
	u8 cur_buf;
	u16 buf_offset;
	u16 buf_size;
	u16 cur_offset;
	u32 reg_epconfx_addr;
	u32 reg_epconfx_val;

	/* calculate register address */
	reg_epconfx_addr = EPCONF0 + (ep * 4);

	/* read EPCONFx register  */
	reg_epconfx_val = ReadRegister(reg_epconfx_addr);

	/* top buffer offset */
	buf_offset = (reg_epconfx_val & 0x1FFF) << 2;

	/* buffer size */
	buf_size = (reg_epconfx_val >> 13) & 0x7FF;
	/* 4Byte alignment */
	if ((buf_size & 0x3) != 0) {
		 buf_size = ((buf_size & 0xFFFC) + 4);
	}

	/* endpoint0 */
	if (ep ==0) {
		/* IN direction */
		if (dir == IN) {
			cur_offset = buf_offset;
		/* OUT direction */
		} else {
			cur_offset = buf_offset + buf_size;
		}
	/* endpoint1 - 15 */
	} else {
		/* get current endpoint buffer */
		cur_buf = Dev_GetCurrentEpBuffer(ep);
		cur_offset = buf_offset + (buf_size * cur_buf);
	}

	return cur_offset;
}


/*****************************************************************
 * [ Function ]		set transfer data size.                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *                  u8 size : transfer data size.                *
 *****************************************************************/
void Dev_SetEpWriteSize(u8 ep, u16 size)
{
	u8 index;
	u8 cur_buf;
	u32 reg_epconfx_addr;
	u32 reg_epconfx_val;
	u32 reg_epcountx_addr;
	u32 reg_epcountx_val;

	/* calculate register address */
	reg_epconfx_addr = EPCONF0 + (ep * 4);

	/* read EPCONFx register */
	reg_epconfx_val = ReadRegister(reg_epconfx_addr);

	/* endpoint0 */
	if (ep == 0) {
		/* get index for EPCOUNT register */
		index = ((reg_epconfx_val >> 24) & 0x1F);
	/* endpoint 1 - 15 */
	} else {
		/* get current endpoint buffer */
		cur_buf = Dev_GetCurrentEpBuffer(ep);
		/* get index for EPCOUNT register */
		index = ((reg_epconfx_val >> 24) & 0x1F) + cur_buf;
	}

	/* calculate register address */
	reg_epcountx_addr = EPCOUNT0 + (index * 4);
	/* read EPCOUNTx register */
	reg_epcountx_val = ReadRegister(reg_epcountx_addr);
	/* set transfer data size */
	reg_epcountx_val = (reg_epcountx_val & 0xFFFF0000) | (size & 0x7FF);
	/* wriet EPCOUNTx register */
	WriteRegister(reg_epcountx_addr, reg_epcountx_val);
}


/*****************************************************************
 * [ Function ]		get transfer data size.                      *
 * [ Return code ]	u16 : transfer data size.                    *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u16 Dev_GetEpReadSize(u8 ep)
{
	u8 index;
	u8 cur_buf;
	u32 reg_epconfx_addr;
	u32 reg_epconfx_val;
	u32 reg_epcountx_addr;
	u32 reg_epcountx_val;

	/* calculate register address */
	reg_epconfx_addr = EPCONF0 + (ep * 4);

	/* read EPCONFx register */
	reg_epconfx_val = ReadRegister(reg_epconfx_addr);

	/* endpoint0 */
	if (ep == 0) {
		/* get index for EPCOUNT register */
		index = ((reg_epconfx_val >> 24) & 0x1F) + 1;
	/* endpoint1 - 15 */
	} else {
		/* get current endpoint buffer */
		cur_buf = Dev_GetCurrentEpBuffer(ep);

		/* get index for EPCOUNT register */
		index = ((reg_epconfx_val >> 24) & 0x1F) + cur_buf;
	}

	/* calculate register address */
	reg_epcountx_addr = EPCOUNT0 + (index * 4);
	/* read EPCOUNTx register */
	reg_epcountx_val = ReadRegister(reg_epcountx_addr);

	/* return transfer data size */
	return (reg_epcountx_val >> 16 & 0x7FF);
}

