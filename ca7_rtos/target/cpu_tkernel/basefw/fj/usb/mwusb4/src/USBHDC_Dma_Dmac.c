/**
 * @file		USBHDC_Dma_Dmac.c
 * @brief		DMAC control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "os_user_custom.h"
#include "dd_exs.h"
#include "debug.h"

#include "USBHDC_Dma_Dmac.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_Dev_Api.h"
#include "USBHDC_Cmn_Dmac.h"
#include "USBHDC_MemMap.h"
#include "USBHDC_Dev_Global.h"
#include "USBHDC_Dev_Control.h"

/* base address of exs hdmac */
#define EXS_HDMAC_TOP_REG_BASE		0x1B040000

/* HDMAC registers */
#define DMACR			(EXS_HDMAC_TOP_REG_BASE + 0x0000)
#define DMACA0			(EXS_HDMAC_TOP_REG_BASE + 0x0010)
#define DMACB0			(EXS_HDMAC_TOP_REG_BASE + 0x0014)
#define DMACSA0			(EXS_HDMAC_TOP_REG_BASE + 0x0018)
#define DMACDA0			(EXS_HDMAC_TOP_REG_BASE + 0x001C)
#define DMACA1			(EXS_HDMAC_TOP_REG_BASE + 0x0020)
#define DMACB1			(EXS_HDMAC_TOP_REG_BASE + 0x0024)
#define DMACSA1			(EXS_HDMAC_TOP_REG_BASE + 0x0028)
#define DMACDA1			(EXS_HDMAC_TOP_REG_BASE + 0x002C)

/* bit definition */
/* DMACR */
#define DMACR_DE			0x80000000
#define DMACR_DS			0x40000000

/* DMACA */
#define DMACA_EB			0x80000000
#define DMACA_PB			0x40000000
#define DMACA_IS_MASK		0x1F000000
#define DMACA_IS_DREQ_H		0x0E000000
#define DMACA_BT_MASK		0x00F00000
#define DMACA_BT_NORMAL		0x00000000
#define DMACA_BT_INCR		0x00800000
#define DMACA_BT_INCR16		0x00F00000
#define DMACA_BC_MASK		0x000F0000
#define DMACA_TC_MASK		0x0000FFFF

/* DMACB */
#define DMACB_TW_MASK		0x0C000000
#define DMACB_TW_WORD		0x08000000
#define DMACB_FS			0x02000000
#define DMACB_FD			0x01000000
#define DMACB_RC			0x00800000
#define DMACB_RS			0x00400000
#define DMACB_RD			0x00200000
#define DMACB_EI			0x00100000
#define DMACB_CI			0x00080000
#define DMACB_SS_MASK		0x00070000
#define DMACB_SS_NOR_END	0x00050000
#define DMACB_SS_DST_ERR	0x00040000
#define DMACB_SS_SRC_ERR	0x00030000
#define DMACB_SS_TRN_STP	0x00020000
#define DMACB_SS_ADD_OVR	0x00010000
#define DMACB_SP_MASK		0x0000F000
#define DMACB_SP_CACHE_EN	0x00008000
#define DMACB_DP_MASK		0x00000F00
#define DMACB_DP_CACHE_EN	0x00000800

/* internal function */
void Dma_AbortDmac(u8 dma_ch);

/*****************************************************************
 * [ Function ]		Abort DMA transfer.                          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = HDC_DMA_CH1                  *
 *                              1 = HDC_DMA_CH2                  *
 *****************************************************************/
void Dma_AbortDmac(u8 dma_ch)
{
	u32 dmaca;
	u32 dmacb;
	s32 i;
	u32 reg;
	
	if (dma_ch == HDC_DMA_CH1) {
		dmaca = DMACA0;
		dmacb = DMACB0;
	} else {
		dmaca = DMACA1;
		dmacb = DMACB1;
	}
	
	/* Check DMA active */
	if ((ReadRegister(dmaca) & DMACA_EB) != 0) {
		/* DMA int disable */
		ClearBitRegister(dmacb, DMACB_EI);
		
		/* stop DMA transfer */
		ClearBitRegister(dmaca, DMACA_EB);
		
		for (i = 0; i <DMA_STOP_LOOP_MAX; i++) {
			reg = ReadRegister(dmacb);
			if (((reg & DMACB_SS_MASK) == DMACB_SS_NOR_END) ||
			    ((reg & DMACB_SS_MASK) == DMACB_SS_TRN_STP)) {
			    /* clear interrupt */
			    ClearBitRegister(dmacb, DMACB_SS_MASK);
				break;
			}
		}
		
		/* DMA int enable */
		SetBitRegister(dmacb, DMACB_EI);
	}
}


/*****************************************************************
 * [ Function ]		DMA Interrupt Handler.                       *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch : 0 = HDC_DMA_CH1                  *
 *                              1 = HDC_DMA_CH2                  *
 *****************************************************************/
void HdcDev_DmaIntHandler(unsigned char dma_ch)
{
	u32 reg;
	
	if (dma_ch == HDC_DMA_CH1) {
		reg = ReadRegister(DMACB0);
		switch((reg & DMACB_SS_MASK)) {
		case DMACB_SS_TRN_STP:
			OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH1_ABORT);
			break;
		case DMACB_SS_ADD_OVR:
		case DMACB_SS_SRC_ERR:
		case DMACB_SS_DST_ERR:
			OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH1_ERROR_END);
			break;
		default:
			break;
		}
		/* clear interrupt factor */
		ClearBitRegister(DMACB0, DMACB_SS_MASK);
	} else {
		reg = ReadRegister(DMACB1);
		switch((reg & DMACB_SS_MASK)) {
		case DMACB_SS_TRN_STP:
			OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH2_ABORT);
			break;
		case DMACB_SS_ADD_OVR:
		case DMACB_SS_SRC_ERR:
		case DMACB_SS_DST_ERR:
			OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH2_ERROR_END);
			break;
		default:
			break;
		}
		/* clear interrupt factor */
		ClearBitRegister(DMACB1, DMACB_SS_MASK);
	}
	
}


/*****************************************************************
 * [ Function ]		initialize DMA controller.                   *
 * [ Return code ]                                               *
 *   int: function status. (0) success, (-1) failure             *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *****************************************************************/
int HdcDev_InitDmac(void)
{
	s32 i;
	
	/* check DMA controller */
	if ((ReadRegister(DMACR) & DMACR_DE) != 0) {
		ClearBitRegister(DMACR, DMACR_DE);
		
		for (i = 0; i < DMA_STOP_LOOP_MAX; i++) {
			if ((ReadRegister(DMACR) & DMACR_DS) != 0) {
				break;
			}
		}
		
		/* ERROR */
		if (i == DMA_STOP_LOOP_MAX) {
			return HDC_FAILURE;
		}
	}
	
	/* enable DMA controller */
	WriteRegister(DMACR, DMACR_DE); /* other bits is all 0. */
	
	return HDC_SUCCESS;
}


/*****************************************************************
 * [ Function ]		start DMA transfer.                          *
 * [ Return code ]                                               *
 *   int: function status. (0) success, (-1) failure             *
 * [ Parameter ]                                                 *
 *   unsigned char dma_ch   : dma channel                        *
 *   unsigned char dir      : direction of transfer              *
 *   unsigned long addr     : buffer address                     *
 *   unsigned long size     : data size                          *
 *   unsigned long block    : block size (64 or 4)               *
 *****************************************************************/
int HdcDev_StartDmac(unsigned char dma_ch, unsigned char dir, unsigned long addr,
                     unsigned long size, unsigned long block)
{
	u32 dmaca_tc;
	
	if (dma_ch > 1) {
		return HDC_FAILURE;
	}
	
	if (dir > 1) {
		return HDC_FAILURE;
	}
	
	if (size == 0) {
		return HDC_FAILURE;
	}
	
	if (dma_ch == HDC_DMA_CH1) {
		if ((ReadRegister(DMACA0) & DMACA_EB) != 0) {
			return HDC_FAILURE;
		}
		
		/* dma setting */
		if (block == 64) {
			dmaca_tc = DMACA_TC_MASK & ((size/block) - 1);
			WriteRegister(DMACA0, (DMACA_IS_DREQ_H | DMACA_BT_INCR16 | dmaca_tc));
		} else {
			dmaca_tc = DMACA_TC_MASK & (((size+3)/4) - 1);
			WriteRegister(DMACA0, (DMACA_IS_DREQ_H | DMACA_BT_INCR | dmaca_tc));
		}
		
		WriteRegister(DMACB0, (DMACB_TW_WORD | DMACB_EI | DMACB_SP_CACHE_EN | DMACB_DP_CACHE_EN));
		
		if (dir == 0) {  /* RAM -> USB macro */
			WriteRegister(DMACSA0, addr);
			WriteRegister(DMACDA0, DMA_CH1_ADRS);
		} else {
			WriteRegister(DMACSA0, DMA_CH1_ADRS);
			WriteRegister(DMACDA0, addr);
		}
		
		/* set event flag */
		OS_User_Clr_Flg(FID_USBLLD, ~(DMA_FLAG_CH1_NORMAL_END | DMA_FLAG_CH1_ERROR_END | DMA_FLAG_CH1_ABORT));
		
		/* start dma */
		SetBitRegister(DMACA0, DMACA_EB);
	} else {
		if ((ReadRegister(DMACA1) & DMACA_EB) != 0) {
			return HDC_FAILURE;
		}
		
		/* dma setting */
		if (block == 64) {
			dmaca_tc = DMACA_TC_MASK & ((size/block) - 1);
			WriteRegister(DMACA1, (DMACA_IS_DREQ_H | DMACA_BT_INCR16 | dmaca_tc));
		} else {
			dmaca_tc = DMACA_TC_MASK & (((size+3)/4) - 1);
			WriteRegister(DMACA1, (DMACA_IS_DREQ_H | DMACA_BT_INCR | dmaca_tc));
		}
		
		WriteRegister(DMACB1, (DMACB_TW_WORD | DMACB_EI | DMACB_SP_CACHE_EN | DMACB_DP_CACHE_EN));
		
		if (dir == 0) {  /* RAM -> USB macro */
			WriteRegister(DMACSA1, addr);
			WriteRegister(DMACDA1, DMA_CH2_ADRS);
		} else {
			WriteRegister(DMACSA1, DMA_CH2_ADRS);
			WriteRegister(DMACDA1, addr);
		}
		
		/* set event flag */
		OS_User_Clr_Flg(FID_USBLLD, ~(DMA_FLAG_CH2_NORMAL_END | DMA_FLAG_CH2_ERROR_END | DMA_FLAG_CH2_ABORT));
		
		/* start dma */
		SetBitRegister(DMACA1, DMACA_EB);
	}
	
	return HDC_SUCCESS;
}


/*****************************************************************
 * [ Function ]		abort DMA transfer.                          *
 * [ Return code ]                                               *
 *   int: function status. (0) success, (-1) failure             *
 * [ Parameter ]                                                 *
 *   unsigned char dma_ch   : dma channel                        *
 *****************************************************************/
int HdcDev_AbortDmac(unsigned char dma_ch)
{
	T_OS_USER_RSYS k_rsys;
	OS_USER_FLGPTN ret_ptn;
	OS_USER_FLGPTN flg_ptn;
	s32 i;
	u32 dmaca_reg;
	u32 dmacb_reg;
	u32 dmacb_ss;
	VOID (*funcSetDmafsmFsmrst)(ULONG);
	int ret = HDC_SUCCESS;
	
	BF_Debug_Print_Information(("HdcDev_AbortDmac(): dma_ch = %d\n", dma_ch));
	
	if (dma_ch > 1) {
		return HDC_FAILURE;
	}
	
	if (dma_ch == HDC_DMA_CH1) {
		dmaca_reg = DMACA0;
		dmacb_reg = DMACB0;
		funcSetDmafsmFsmrst = Dd_Exs_Set_USB2_DMAFSM1_FSMRST1;
		flg_ptn = DMA_FLAG_CH1_ABORT;
	} else {
		dmaca_reg = DMACA1;
		dmacb_reg = DMACB1;
		funcSetDmafsmFsmrst = Dd_Exs_Set_USB2_DMAFSM2_FSMRST2;
		flg_ptn = DMA_FLAG_CH2_ABORT;
	}

	if ((ReadRegister(dmaca_reg) & DMACA_EB) == 0) {  /* DMA is stopped */
		
		BF_Debug_Print_Information(("HdcDev_AbortDmac(): DMA is stopped.\n"));
		
		/* clear FMS reset */
		funcSetDmafsmFsmrst(DMAFSM_FSMRST_RESET_OFF);
		return HDC_SUCCESS;
	}
		
	/* abort DMA */
	ClearBitRegister(dmaca_reg, DMACA_EB);
	
	if (OS_User_Ref_Sys(&k_rsys) == D_OS_USER_E_OK) {
		if (k_rsys.sysstat == 0) {
			if (OS_User_Twai_Flg(FID_USBLLD, flg_ptn, D_OS_USER_TWF_ORW, &ret_ptn, 300) != D_OS_USER_E_OK) {
				ret = HDC_FAILURE;
			} else {
				OS_User_Clr_Flg(FID_USBLLD, ~flg_ptn);
			}
		} else {
			for (i = 0; i < DMA_STOP_LOOP_MAX; i++) {
				dmacb_ss = ReadRegister(dmacb_reg) & DMACB_SS_MASK;
				if ((dmacb_ss == DMACB_SS_TRN_STP) || (dmacb_ss == DMACB_SS_NOR_END)) {
					/* Clear SS bit */
					ClearBitRegister(dmacb_reg, DMACB_SS_MASK);
					break;
				}
			}
			if (i == DMA_STOP_LOOP_MAX) {
				ret = HDC_FAILURE;
			}
		}
	} else {
		for (i = 0; i < DMA_STOP_LOOP_MAX; i++) {
			dmacb_ss = ReadRegister(dmacb_reg) & DMACB_SS_MASK;
			if ((dmacb_ss == DMACB_SS_TRN_STP) || (dmacb_ss == DMACB_SS_NOR_END)) {
				/* Clear SS bit */
				ClearBitRegister(dmacb_reg, DMACB_SS_MASK);
				break;
			}
		}
		if (i == DMA_STOP_LOOP_MAX) {
			ret = HDC_FAILURE;
		}
	}
		
	/* clear FSM reset */
	funcSetDmafsmFsmrst(DMAFSM_FSMRST_RESET_OFF);
	
	return ret;
}


/*****************************************************************
 * [ Function ]		wait DMA transfer.                           *
 * [ Return code ]                                               *
 *   int: function status. (0) success, (-1) failure             *
 * [ Parameter ]                                                 *
 *   unsigned char dma_ch   : dma channel                        *
 *****************************************************************/
int HdcDev_WaitDmac(unsigned char dma_ch, unsigned long timeout)
{
	OS_USER_FLGPTN ret_ptn = 0;
	OS_USER_FLGPTN flg_ptn;
	int ret;
	
	if (dma_ch > 1) {
		return HDC_FAILURE;
	}
	
	if (dma_ch == HDC_DMA_CH1) {
		flg_ptn = DMA_FLAG_CH1_NORMAL_END | DMA_FLAG_CH1_ERROR_END;
	} else {
		flg_ptn = DMA_FLAG_CH2_NORMAL_END | DMA_FLAG_CH2_ERROR_END;
	}
	
	/* wait event flag */
	if (OS_User_Twai_Flg(FID_USBLLD, flg_ptn, D_OS_USER_TWF_ORW, &ret_ptn, timeout) != D_OS_USER_E_OK) {
		/* ERROR */
		ret = HDC_FAILURE;
	} else {
		/* clear event flag */
		OS_User_Clr_Flg(FID_USBLLD, ~flg_ptn);
		if (dma_ch == HDC_DMA_CH1) {
			if ((ret_ptn & DMA_FLAG_CH1_NORMAL_END) == DMA_FLAG_CH1_NORMAL_END) {
				ret = HDC_SUCCESS;
			} else {
				/* DMA error end */
				ret = HDC_FAILURE;
			}
		} else {
			if ((ret_ptn & DMA_FLAG_CH2_NORMAL_END) == DMA_FLAG_CH2_NORMAL_END) {
				ret = HDC_SUCCESS;
			} else {
				/* DMA error end */
				ret = HDC_FAILURE;
			}
		}
	}
	
	return ret;
}

