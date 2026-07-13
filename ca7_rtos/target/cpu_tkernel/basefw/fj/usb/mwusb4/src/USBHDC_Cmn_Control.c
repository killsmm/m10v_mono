/**
 * @file		USBHDC_Cmn_Control.c
 * @brief		USB Host & Device control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#include "debug.h"

#include "USBHDC_Cmn_Control.h"

#include "USBHDC_Dev_Global.h"

#include "USBHDC_MemMap.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"


/***  CONF register access  ***/
/*****************************************************************
 * [ Function ]		set byte order.                              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 byte_order : 0 = little endian            *
 *                                  else = big endian            *
 *****************************************************************/
void Cmn_SetByteOrder(u8 byte_order)
{
	/* set little endian */
	if( byteOrder == 0 )
	{
		/* little endian */
		if( byte_order != 0 )
		{
			SetBitRegister(CONF, CONF_BYTE_ORDER); /* set big endian */
			byteOrder = byte_order;
		}
	}
	else
	{
		/* big endian */
		if( byte_order == 0 )
		{
			ClearBitRegister(CONF, CONF_BYTE_ORDER); /* set little endian */
			byteOrder = byte_order;
		}
	}
}


/*****************************************************************
 * [ Function ]		set burst wait.                              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 wait cycle : 0 = 0 cycle wait             *
 *                                  else = 1 cycle wait          *
 *****************************************************************/
void Cmn_SetBurstWait(u8 wait_cycle)
{
	//u32 set_val;
	/* set 0 cycle wait */
	if( wait_cycle == 0 )
	{
		ClearBitRegister(CONF, CONF_BURST_WAIT);
	/* set 1 cycle wait */
	}
	else
	{
		SetBitRegister(CONF, CONF_BURST_WAIT);
	}
}


/*****************************************************************
 * [ Function ]		set soft reset.                              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 * update 2010.10.04 by J.Tomida                                 *
 *    add reset byteOrder reset(=0)                              *
 *****************************************************************/
s32 Cmn_SoftReset(u8 byte_order)
{
	u32 reg_conf_val;
	u32 i;

	WriteRegister(CONF, CONF_SOFT_RESET); /* soft reset */
	byteOrder = 0; /** add 2010.10.04 by tomida reset little endian **/

	Cmn_SetByteOrder(byte_order); /* set byte order */

	/* wait device reset release */
	for(i=0; i<SOFT_RESET_LOOP_MAX; i++) {
		reg_conf_val = ReadRegister(CONF);
		if ((reg_conf_val & CONF_SOFT_RESET) == 0)
			break;
	}
	
	if (i == SOFT_RESET_LOOP_MAX) {
		return HDC_FAILURE;
	}
	
	return HDC_SUCCESS;
}

/***  MODE register access  ***/
/*****************************************************************
 * [ Function ]		set active mode of USB macro                 *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 mode : 0 = USB Host function              *
 *                            else = USB Device function         *
 *****************************************************************/
void Cmn_SetActiveMode(u8 mode)
{
	/* set USB Host function mode */
	if (mode == 0x0)
	{
		ClearBitRegister(MODE, 0x2);
	/* set USB Device function mode */
	}
	else
	{
		SetBitRegister(MODE, 0x2);
	}
}


/*****************************************************************
 * [ Function ]		get active mode of USB macro                 *
 * [ Return code ]	u8	: (0) USB Device has not been activated. *
 *                        (2) USB Device has been activated.     *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Cmn_GetActiveMode(void)
{
	u32 reg_mode_val;

	/* get USB active mode(Host or Device) */
	reg_mode_val = ReadRegister(MODE);
	
	BF_Debug_Print_Error(("Cmn_GetActiveMode() MODE = 0x%08lx\n", reg_mode_val));
	
	return (u8)(reg_mode_val & 0x2);
}

/***  INTS register access  ***/
/*****************************************************************
 * [ Function ]		clear global interrupt state.                *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 int_pos : bit position of interrupt state*
 *****************************************************************/
void Cmn_ClearIntState(u32 int_pos)
{
	/* write INTS register */
	WriteRegister(INTS, ~int_pos & 0x00000318);
}

#if 0
/***  UTMIVC register access  ***/
/*****************************************************************
 * [ Function ]		set vload_n field value of UTMIVC register.  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 vload : vload_n field value.              *
 *****************************************************************/
void Cmn_SetUtmiVload(u8 vload)
{
	/* set utmi vload */
	SetBitRegister(UTMIVC, UTMIVC_VLOAD_N);
}


/*****************************************************************
 * [ Function ]		get vload_n field value of UTMIVC register.  *
 * [ Return code ]	u8 : vload_n field value.                    *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Cmn_GetUtmiVload(void)
{
	u32 reg_utmivc_val;

	/* get utmi vload */
	reg_utmivc_val = ReadRegister(UTMIVC);
	return (u8)(reg_utmivc_val & 0x1);
}


/*****************************************************************
 * [ Function ]		set vcntl field value of UTMIVC register.    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 vload : vcntl field value.                *
 *****************************************************************/
void Cmn_SetUtmiVcntl(u8 vcntl)
{
	u32 reg_utmivc_val;

	/* read utmivc reg */
	reg_utmivc_val = ReadRegister(UTMIVC);

	/* write utmivc reg */
	reg_utmivc_val = ((reg_utmivc_val & 0xFF0F) | ((vcntl << 4) & 0xF0));
	WriteRegister(UTMIVC, reg_utmivc_val);
}


/*****************************************************************
 * [ Function ]		get vcntl field value of UTMIVC register.    *
 * [ Return code ]	u8 : vcntl field value.                      *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Cmn_GetUtmiVcntl(void)
{
	u32 reg_utmivc_val;

	/* get utmi vcntl */
	reg_utmivc_val = ReadRegister(UTMIVC);
	return (u8)((reg_utmivc_val >> 4) & 0xF);
}


/*****************************************************************
 * [ Function ]		get vstatus field value of UTMIVC register.  *
 * [ Return code ]	u8 : vstatus field value.                    *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Cmn_GetUtmiVstatus(void)
{
	u32 reg_utmivc_val;

	/* get utmi vstatus */
	reg_utmivc_val = ReadRegister(UTMIVC);
	return (u8)((reg_utmivc_val >> 8) & 0xFF);
}


/***  GPCNT register access  ***/
/*****************************************************************
 * [ Function ]		set gpcnt field value of GPCNT register.     *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 gpcnt : gpcnt field value.                *
 *****************************************************************/
void Cmn_SetGpcnt(u8 gpcnt)
{
	/* write utmivc reg */
	WriteRegister(GPCNT, (gpcnt & 0xFF));
}


/*****************************************************************
 * [ Function ]		get gpcnt field value of GPCNT register.     *
 * [ Return code ]	u8 : gpcnt field value.                      *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Cmn_GetGpcnt(void)
{
	u32 reg_gpcnt_val;

	/* get utmi vstatus */
	reg_gpcnt_val = ReadRegister(GPCNT);
	return (u8)(reg_gpcnt_val & 0xFF);
}
#endif

/***  EPCMD register access (host & device Commonness)  ***/
/*****************************************************************
 * [ Function ]		set endpoint enable.                         *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Cmn_SetEpEnable(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set endpoint enable */
	WriteRegister(reg_epcmdx_addr, EPCMDx_START);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}

#if 0
// host only
/*****************************************************************
 * [ Function ]		set endpoint disable. Time wait nothing.     *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Cmn_SetEpDisable(u8 ep)
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
}
#endif

/*****************************************************************
 * [ Function ]		set writing completetion.                    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Cmn_SetWriteComplete(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set writing completetion */
	WriteRegister(reg_epcmdx_addr, EPCMDx_BUFWR);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

}

/*****************************************************************
 * [ Function ]		set reading completetion.                    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Cmn_SetReadComplete(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set reading completetion */
	WriteRegister(reg_epcmdx_addr, EPCMDx_BUFRD);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}

/*****************************************************************
 * [ Function ]		set toggle bit.                              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Cmn_SetToggleBit(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* set toggle bit */
	WriteRegister(reg_epcmdx_addr, EPCMDx_TOGGLE_SET);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}

/*****************************************************************
 * [ Function ]		clear toggle bit.                            *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
void Cmn_ClearToggleBit(u8 ep)
{
	u32 reg_epcmdx_addr;

	/* calculate register address */
	reg_epcmdx_addr = EPCMD0 + (ep * 4);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);

	/* clear toggle bit */
	WriteRegister(reg_epcmdx_addr, EPCMDx_TOGGLE_CLR);

	/* wait command busy */
	Cmn_WaitEpCommand(reg_epcmdx_addr);
}


/*****************************************************************
 * [ Function ]		inquire endpoint command busy.               *
 * [ Return code ]	u8 : (0) endpoint command no busy.           *
 *                       (1) endpoint command busy.              *
 * [ Parameter ]	u8 ep : endpoint number(0 - 15).             *
 *****************************************************************/
u8 Cmn_IsEpCommandBusy(u32 reg_epcmdx_offset)
{
	u32 reg_epcmdx_val;

	reg_epcmdx_val = ReadRegister(reg_epcmdx_offset);

	return (u8)((reg_epcmdx_val>>31) & 0x1); 
}


/*****************************************************************
 * [ Function ]		wait endpoint command busy.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 reg_epcmdx_addr : epcmd register address.*
 *****************************************************************/
void Cmn_WaitEpCommand(u32 reg_epcmdx_addr)
{
	/* wait command busy */
	while (Cmn_IsEpCommandBusy(reg_epcmdx_addr) == HDC_TRUE){
		;
	}
}

/***  endpoint buffer access  ***/
/*****************************************************************
 * [ Function ]		read data from endpoint buffer.              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 offset : data offset                     *
 *                  u8 *data : pointer of data storage area.     *
 *                  u16 size : read data size.                   *
 *****************************************************************/
void Cmn_ReadEpBuffer(u16 offset, u8 *data, u16 size)
{
	u32 i;
	u16 remainder;
	u32 tmp;
	
#ifdef USBHDC_DEBUG_CAPTURE
printf("HOST -> DSC size = %d\n\r", size);	// Debug Capture
#endif
	
//	remainder = size % 4;
	remainder = size & 0x0003;
	
	if (((u32)data & 0x3) == 0) {  /* 4 byte alignment check */
		for(i = 0; i < (size - remainder); i += 4) {
			*((u32 *)data) = ReadMemory32((u32)(USBHDC_EP_REG_BASE + offset + i));
#ifdef USBHDC_DEBUG_CAPTURE
char* aaa = data;
char* bbb = data+1;
char* ccc = data+2;
char* ddd = data+3;
printf(" %02X%02X%02X%02X", *aaa, *bbb,*ccc,*ddd);	// Debug Capture

if((i % 16) == 12) {
printf("\n\r");	// Debug Capture
}
#endif
			data += 4;
		}
	} else {
		for(i = 0; i < (size - remainder); i += 4) {
			tmp = ReadMemory32((u32)(USBHDC_EP_REG_BASE + offset + i));
			*data = (u8)(tmp & 0x000000FF);
			*(data+1) = (u8)((tmp >> 8) & 0x000000FF);
			*(data+2) = (u8)((tmp >> 16) & 0x000000FF);
			*(data+3) = (u8)((tmp >> 24) & 0x000000FF);
#ifdef USBHDC_DEBUG_CAPTURE
char* aaa = data;
char* bbb = data+1;
char* ccc = data+2;
char* ddd = data+3;
printf(" %02X%02X%02X%02X", *aaa, *bbb,*ccc,*ddd);	// Debug Capture

if((i % 16) == 12) {
printf("\n\r");	// Debug Capture
}
#endif
			data += 4;  /* count up */
		}
	}

#ifdef USBHDC_DEBUG_CAPTURE
printf(" ");	// Debug Capture
#endif
	/* byte access */
	if (remainder != 0) {
		for (i = 0; i < remainder; i++) {
			*data = ReadMemory8((u32)(USBHDC_EP_REG_BASE + offset + size - remainder + i));

#ifdef USBHDC_DEBUG_CAPTURE
char* aaa = data;
printf("%02X", *aaa);	// Debug Capture
#endif
			data++;
		}
	}
#ifdef USBHDC_DEBUG_CAPTURE
printf("\n\r\n\r");	// Debug Capture
#endif
}

/*****************************************************************
 * [ Function ]		write data to endpoint buffer.               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 offset : data offset                     *
 *                  u8 *data : pointer of data storage area.     *
 *                  u16 size : write data size.                  *
 *****************************************************************/
void Cmn_WriteEpBuffer(u16 offset, u8 *data, u16 size)
{
	u32 i;
	u16 remainder;
	u32 tmp;

#ifdef USBHDC_DEBUG_CAPTURE
printf("DSC -> HOST size = %d\n\r", size);	// Debug Capture
#endif

//	remainder = size % 4;
	remainder = size & 0x0003;
	
	if (((u32)data & 0x3) == 0) {  /* 4 byte alignment check */
		for (i = 0; i < (size - remainder); i += 4) {
			WriteMemory32((u32)(USBHDC_EP_REG_BASE + offset + i), *((u32 *)data));
#ifdef USBHDC_DEBUG_CAPTURE
		printf(" %08X",*((u32 *)data));	// Debug Capture
if((i % 16) == 12) {
printf("\n\r");
}
#endif
			data += 4;
		}
	} else {
		for (i = 0; i < (size - remainder); i += 4) {
			tmp = (u32)(*data);
			tmp |= (u32)((u32)(*(data+1)) << 8);
			tmp |= (u32)((u32)(*(data+2)) << 16);
			tmp |= (u32)((u32)(*(data+3)) << 24);
			WriteMemory32((u32)(USBHDC_EP_REG_BASE + offset + i), tmp);
#ifdef USBHDC_DEBUG_CAPTURE
		printf(" %08X",*((u32 *)tmp));	// Debug Capture
if((i % 16) == 12) {
printf("\n\r");
}
#endif
			data += 4;
		}
	}
	
#ifdef USBHDC_DEBUG_CAPTURE
printf(" ");	// Debug Capture
#endif

	/* byte access */
	if (remainder != 0) {
		for (i = 0; i < remainder; i++) {
			WriteMemory8((u32)(USBHDC_EP_REG_BASE + offset + size - remainder + i), *data);
#ifdef USBHDC_DEBUG_CAPTURE
char* aaa = data+i;
printf("%02X", *aaa);
#endif
			data++;
		}
	}
#ifdef USBHDC_DEBUG_CAPTURE
printf("\n\r\n\r");	// Debug Capture
#endif
}

