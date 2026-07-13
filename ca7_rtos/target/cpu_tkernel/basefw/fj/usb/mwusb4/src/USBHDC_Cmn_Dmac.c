/**
 * @file		USBHDC_Cmn_Dmac.c
 * @brief		USB Host & Device dma control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_Cmn_Dmac.h"
#include "USBHDC_Dev_Api.h"
#include "USBHDC_MemMap.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_StdDef.h"


/*****************************************************************
 * [ Function ]		start dmac.                                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *****************************************************************/
void Cmn_StartDmac(u8 dma_ch)
{
	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* set dmac start */
		SetBitRegister(DMAC1, DMAC_DMA_ST);

	/* dma ch2 */
	} else {
		/* set dmac start */
		SetBitRegister(DMAC2, DMAC_DMA_ST);
	}
}


/*****************************************************************
 * [ Function ]		stop dmac.                                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *****************************************************************/
void Cmn_StopDmac(u8 dma_ch)
{
	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* clear dmac start */
		ClearBitRegister(DMAC1, DMAC_DMA_ST);

	/* dma ch2 */
	} else {
		/* clear dmac start */
		ClearBitRegister(DMAC2, DMAC_DMA_ST);
	}
}


/*****************************************************************
 * [ Function ]		inquire dmac busy.                           *
 * [ Return code ]	0 : dma no busy.                             *
 *                  1 : dma busy.                                *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *****************************************************************/
u8 Cmn_IsBusyDmac(u8 dma_ch)
{
	u32 reg_dmac_val;
	u8  ret;

	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* read register */	
		reg_dmac_val = ReadRegister(DMAC1);

	/* dma ch2 */
	} else {
		/* read register */	
		reg_dmac_val = ReadRegister(DMAC2);
	}

	/* return endpoint is starting or not starting */
	if ((reg_dmac_val & DMAC_DMA_ST) != 0) {
		ret = HDC_TRUE;  /* dma is busy */
	} else {
		ret = HDC_FALSE; /* dma is not busy */
	}
	return ret;


}


/*****************************************************************
 * [ Function ]		set dmac transfer mode.                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *              	u8 mode : 0 = demand transfer.               *
 *                            1 = block transfer.                *
 *****************************************************************/
void Cmn_SetDmacTransferMode(u8 dma_ch, u8 mode)
{
	/* dma ch1 */
	if (dma_ch == 0) {
		/* set transfer mode */
		if (mode == 0) {
			/* demand transfer */
			ClearBitRegister(DMAC1, DMAC_DMA_MODE);
		} else {
			/* block transfer */
			SetBitRegister(DMAC1, DMAC_DMA_MODE);
		}

	/* dma ch2 */
	} else {
		/* set transfer mode */
		if (mode == 0) {
			/* demand transfer */
			ClearBitRegister(DMAC2, DMAC_DMA_MODE);
		} else {
			/* block transfer */
			SetBitRegister(DMAC2, DMAC_DMA_MODE);
		}
	}
}


/*****************************************************************
 * [ Function ]		set null packet tarnasmission for dmac IN    *
 *                  transfer when last packet is max packet size.*
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *              	u8 val : 0 = not null packet tarnasmission.  *
 *                           1 = null packet tarnasmission.      *
 *****************************************************************/
void Cmn_SetDmacNullTransfer(u8 dma_ch, u8 val)
{
	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* set null packet tarnasmission */
		if (val == 0) {
			/* not null packet tarnasmission */
			ClearBitRegister(DMAC1, DMAC_DMA_SENDNULL);
		} else {
			/* null packet tarnasmission */
			SetBitRegister(DMAC1, DMAC_DMA_SENDNULL);
		}

	/* dma ch2 */
	} else {
		/* set null packet tarnasmission */
		if (val == 0) {
			/* not null packet tarnasmission */
			ClearBitRegister(DMAC2, DMAC_DMA_SENDNULL);
		} else {
			/* null packet tarnasmission */
			SetBitRegister(DMAC2, DMAC_DMA_SENDNULL);
		}
	}
}


/*****************************************************************
 * [ Function ]		set stop condition of dmac IN transfer.      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *              	u8 val : 0 = not stop by buffer empty.       *
 *                           1 = stop by buffer empty.           *
 *****************************************************************/
void Cmn_SetDmacEndCondition(u8 dma_ch, u8 val)
{
	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* set stop condition */
		if (val == 0) {
			/* not stop by buffer empty */
			ClearBitRegister(DMAC1, DMAC_DMA_INT_EMPTY);
		} else {
			/* stop by buffer empty */
			SetBitRegister(DMAC1, DMAC_DMA_INT_EMPTY);	
		}

	/* dma ch2 */
	} else {
		/* set stop condition */
		if (val == 0) {
			/* not stop by buffer empty */
			ClearBitRegister(DMAC2, DMAC_DMA_INT_EMPTY);
		} else {
			/* stop by buffer empty */
			SetBitRegister(DMAC2, DMAC_DMA_INT_EMPTY);	
		}
	}
}


/*****************************************************************
 * [ Function ]		set stop condition of dmac OUT transfer.     * 
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *                u8 val : 0 = not stop by short packet received.*
 *                         1 = stop by short packet received.    *
 *****************************************************************/
void Cmn_SetDmacShortPacketRcv(u8 dma_ch, u8 val)
{
	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* set stop condition */
		if (val == 0) {
			/* not stop by short packet received. */
			ClearBitRegister(DMAC1, DMAC_DMA_SPR);
		} else {
			/* stop by short packet received. */
			SetBitRegister(DMAC1, DMAC_DMA_SPR);
		}

	/* dma ch2 */
	} else {
		/* set stop condition */
		if (val == 0) {
			/* not stop by short packet received. */
			ClearBitRegister(DMAC2, DMAC_DMA_SPR);
		} else {
			/* stop by short packet received. */
			SetBitRegister(DMAC2, DMAC_DMA_SPR);
		}
	}
}


/*****************************************************************
 * [ Function ]		set endpoint of dmac transfer object.        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *              	u8 ep : endpoint of transfer object          *
 *****************************************************************/
void Cmn_SetDmacEndpoint(u8 dma_ch, u8 ep)
{
	u32 reg_dmac_val;
	u32 tmp;

	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* read register */
		reg_dmac_val = ReadRegister(DMAC1);

		/* set endpoint */	
		tmp = ((reg_dmac_val & 0xFFFFF0FF) | ((ep & 0xF) << 8));

		/* write register */	
		WriteRegister(DMAC1, tmp);

	/* dma ch2 */
	} else {
		/* read register */
		reg_dmac_val = ReadRegister(DMAC2);

		/* set endpoint */	
		tmp = ((reg_dmac_val & 0xFFFFF0FF) | ((ep & 0xF) << 8));

		/* write register */	
		WriteRegister(DMAC2, tmp);
	}
}

#if 0
/*****************************************************************
 * [ Function ]		get endpoint number of dmac setting.         *
 * [ Return code ]	u8 : endpoint number.                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *****************************************************************/
u8 Cmn_GetDmacEndpoint(u8 dma_ch)
{
	u32 reg_dmacx_addr;
	u32 reg_dmacx_val;

	/* calculate register address */
	reg_dmacx_addr = DMAC1 + (dma_ch * 0x20);

	/* read DMACx register */
	reg_dmacx_val = ReadRegister(reg_dmacx_addr);

	/* return endpoint enable or disable */
	return (u8)((reg_dmacx_val >> 8) & 0xF);
}
#endif

/*****************************************************************
 * [ Function ]		set block tarnsfer size of dmac.             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *              	u16 size : block tarnsfer size.              *
 *****************************************************************/
void Cmn_SetDmacBlockTransferSize(u8 dma_ch, u16 size)
{
	u32 reg_dmac_val;
	u32 tmp;

	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* read register */	
		reg_dmac_val = ReadRegister(DMAC1);

		/* set block tarnsfer size */
		tmp = ((reg_dmac_val & 0xF800FFFF) | ((size & 0x7FF) << 16));

		/* write register */
		WriteRegister(DMAC1, tmp);

	/* dma ch2 */
	} else {
		/* read register */	
		reg_dmac_val = ReadRegister(DMAC2);

		/* set block tarnsfer size */
		tmp = ((reg_dmac_val & 0xF800FFFF) | ((size & 0x7FF) << 16));

		/* write register */
		WriteRegister(DMAC2, tmp);
	}
}


/*****************************************************************
 * [ Function ]		get dma transfer state.                      *
 * [ Return code ]	u8 : dma state. 1 = Null packet found.       *
 *                                  2 = Short packet found.      *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *****************************************************************/
u8 Cmn_GetDmaTransferState(u8 dma_ch)
{
	u32 reg_dmas_val;
	u8  ret;

	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* read DMAS register */
		reg_dmas_val = ReadRegister(DMAS1);

	/* dma ch2 */
	} else {
		/* read DMAS register */
		reg_dmas_val = ReadRegister(DMAS2);
	}

	if ((reg_dmas_val & DMAS_DMA_NP) != 0) {
		ret = HDC_TRN_ST_NULLPKT;
	} else {
		if ((reg_dmas_val & DMAS_DMA_SP) != 0) {
			ret = HDC_TRN_ST_SHORTPKT;
		} else {
			ret = HDC_TRN_ST_NORMAL;
		}
	}
	
	return ret;
}


/*****************************************************************
 * [ Function ]		set tarnsfer size of dmac.                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *              	u32 size : tarnsfer size.                    *
 *****************************************************************/
void Cmn_SetDmacTransferSize(u8 dma_ch, u32 size)
{
	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* set tarnsfer size */
		WriteRegister(DMATCI1, size);

	/* dma ch2 */
	} else {
		/* set tarnsfer size */
		WriteRegister(DMATCI2, size);
	}
}


/*****************************************************************
 * [ Function ]		get transfer(Forwarding completion) size of dmac.*
 * [ Return code ]	u32 size :  transmitted size.                *
 * [ Parameter ]	u8 dma_ch : 0 = dma ch1, 1 = dma ch2.        *
 *****************************************************************/
u32 Cmn_GetDmacTransferSize(u8 dma_ch)
{
	u32 reg_dmatc_val;

	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* read register */
		reg_dmatc_val = ReadRegister(DMATC1);

	/* dma ch2 */
	} else {
		/* read register */
		reg_dmatc_val = ReadRegister(DMATC2);
	}

	return reg_dmatc_val;
}

