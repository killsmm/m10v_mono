/**
 * ogma_reh.h
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#ifndef OGMA_REG_H
#define OGMA_REG_H

#define OGMA_REG_ADDR_SOFT_RST                  (0x104)
#define OGMA_REG_ADDR_COM_INIT                  (0x120)
#define OGMA_REG_ADDR_DMAC_HM_CMD_BUF           (0x210)
#define OGMA_REG_ADDR_DIS_CORE                  (0x218)
#define OGMA_REG_ADDR_DMA_HM_CTRL               (0x214)
#define OGMA_REG_ADDR_DMA_MH_CTRL               (0x220)
#define OGMA_REG_ADDR_CLK_EN                    (0x100)
#define OGMA_REG_ADDR_PKT_CTRL                  (0x140)
#define OGMA_REG_ADDR_TX_DESC_START             (0x408) 
#define OGMA_REG_ADDR_RX_DESC_START				(0x448)
#define OGMA_REG_ADDR_LPBE_RX_DESC_START		(0x4c8)
#define OGMA_REG_ADDR_LPBR_RX_DESC_START		(0x548)
#define OGMA_REG_ADDR_VSUB_TX_DESC_START		(0x708)
#define OGMA_REG_ADDR_TS_TX_DESC_START			(0x788)

#define OGMA_REG_ADDR_DMA_MH_MICRO_CODE_ADDR    (0x224)
#define OGMA_REG_ADDR_DMA_MH_MICRO_CODE_SIZE    (0x228)
#define OGMA_REG_ADDR_PKTC_MICRO_CODE_ADDR      (0x170)
#define OGMA_REG_ADDR_PKTC_MICRO_CODE_SIZE      (0x174)
#define OGMA_REG_ADDR_STRM_CTRL                 (0x108)
#define OGMA_REG_ADDR_TX_CONFIG                 (0x430)
#define OGMA_REG_ADDR_RX_CONFIG				    (0x470)
#define OGMA_REG_ADDR_LPBE_RX_CONFIG		    (0x4f0)
#define OGMA_REG_ADDR_LPBR_RX_CONFIG		    (0x570)
#define OGMA_REG_ADDR_VSUB_TX_CONFIG		    (0x730)
#define OGMA_REG_ADDR_TS_TX_CONFIG			    (0x7b0)

#define OGMA_REG_ADDR_TOP_STATUS                (0x200)

#if 1
#define OGMA_REG_ADDR_TOP_INTEN                 (0x204) // (INTEN_A)
#define OGMA_REG_ADDR_TOP_INTEN_SET             (0x234) // (INTEN_A)
#define OGMA_REG_ADDR_TOP_INTEN_CLR             (0x238) // (INTEN_A)
#else
#define OGMA_REG_ADDR_TOP_INTEN                 (0x23c) // (INTEN_B)
#define OGMA_REG_ADDR_TOP_INTEN_SET				(0x240) // (INTEN_B)
#define OGMA_REG_ADDR_TOP_INTEN_CLR				(0x244) // (INTEN_B)
#endif

#define OGMA_REG_ADDR_PKT_STATUS                (0xc)
#define OGMA_REG_ADDR_PKT_INTEN					(0x10)
#define OGMA_REG_ADDR_PKT_INTEN_SET				(0x1c)
#define OGMA_REG_ADDR_PKT_INTEN_CLR				(0x20)

#define OGMA_REG_ADDR_STRM_STATUS				 (0x14)
#define OGMA_REG_ADDR_STRM_INTEN				 (0x18)
#define OGMA_REG_ADDR_STRM_INTEN_SET			 (0x24)
#define OGMA_REG_ADDR_STRM_INTEN_CLR			 (0x28)

#define OGMA_REG_ADDR_SUB_STATUS				 (0x2c)
#define OGMA_REG_ADDR_SUB_INTEN				     (0x30)
#define OGMA_REG_ADDR_SUB_INTEN_SET			     (0x34)
#define OGMA_REG_ADDR_SUB_INTEN_CLR			     (0x38)

#define OGMA_REG_ADDR_MAC_STATUS				(0x1024)
#define OGMA_REG_ADDR_MAC_INTEN					(0x1028)
#define OGMA_REG_ADDR_MAC_TXRX_INFO_STATUS		(0x1218)
#define OGMA_REG_ADDR_MAC_TXRX_INFO_INTEN	    (0x121c)


#define OGMA_REG_ADDR_PKT_TX_STATUS				(0x400)
#define OGMA_REG_ADDR_PKT_TX_INTEN				(0x404)
#define OGMA_REG_ADDR_PKT_TX_INTEN_SET			(0x428)
#define OGMA_REG_ADDR_PKT_TX_INTEN_CLR			(0x42c)

#define OGMA_REG_ADDR_PKT_RX_STATUS				(0x440)
#define OGMA_REG_ADDR_PKT_RX_INTEN				(0x444)
#define OGMA_REG_ADDR_PKT_RX_INTEN_SET			(0x468)
#define OGMA_REG_ADDR_PKT_RX_INTEN_CLR			(0x46c)

#define OGMA_REG_ADDR_LPBE_RX_STATUS		(0x4c0)
#define OGMA_REG_ADDR_LPBE_RX_INTEN			(0x4c4)
#define OGMA_REG_ADDR_LPBE_RX_INTEN_SET		(0x4e8)
#define OGMA_REG_ADDR_LPBE_RX_INTEN_CLR		(0x4ec)

#define OGMA_REG_ADDR_LPBR_RX_STATUS		(0x540)
#define OGMA_REG_ADDR_LPBR_RX_INTEN			(0x544)
#define OGMA_REG_ADDR_LPBR_RX_INTEN_SET		(0x568)
#define OGMA_REG_ADDR_LPBR_RX_INTEN_CLR		(0x56c)

#define OGMA_REG_ADDR_VSUB_TX_STATUS			(0x700)
#define OGMA_REG_ADDR_VSUB_TX_INTEN				(0x704)
#define OGMA_REG_ADDR_VSUB_TX_INTEN_SET			(0x728)
#define OGMA_REG_ADDR_VSUB_TX_INTEN_CLR			(0x72c)

#define OGMA_REG_ADDR_TS_TX_STATUS				(0x780)
#define OGMA_REG_ADDR_TS_TX_INTEN				(0x784)
#define OGMA_REG_ADDR_TS_TX_INTEN_SET			(0x7a8)
#define OGMA_REG_ADDR_TS_TX_INTEN_CLR			(0x7ac)

#define OGMA_REG_ADDR_PKT_TX_CNT			(0x410)
#define OGMA_REG_ADDR_VSUB_TX_CNT			(0x710)
#define OGMA_REG_ADDR_TS_TX_CNT				(0x790)

#define OGMA_REG_ADDR_PKT_TX_DONE_CNT        	(0x414)
#define OGMA_REG_ADDR_VSUB_TX_DONE_CNT			(0x714)
#define OGMA_REG_ADDR_TS_TX_DONE_CNT			(0x794)

#define OGMA_REG_ADDR_PKT_TX_DONE_TXINT_CNT     	(0x418)
#define OGMA_REG_ADDR_VSUB_TX_DONE_TXINT_CNT		(0x718)
#define OGMA_REG_ADDR_TS_TX_DONE_TXINT_CNT			(0x798)

#define OGMA_REG_ADDR_PKT_TX_TMR			(0x41c)
#define OGMA_REG_ADDR_PKT_TX_TXINT_TMR		(0x420)

#define OGMA_REG_ADDR_PKT_RX_PKTCNT 		(0x454)
#define OGMA_REG_ADDR_LPBE_RX_CNT 		(0x4d4)
#define OGMA_REG_ADDR_LPBR_RX_CNT 		(0x554)

#define OGMA_REG_ADDR_PKT_RX_RXINT_PKTCNT	(0x458)
#define OGMA_REG_ADDR_LPBE_RX_RXINT_CNT	(0x4d8)
#define OGMA_REG_ADDR_LPBR_RX_RXINT_CNT	(0x558)

#define OGMA_REG_ADDR_PKT_RX_TMR			(0x45c)
#define OGMA_REG_ADDR_PKT_RX_RXINT_TMR		(0x460)

#define OGMA_REG_ADDR_SDB_CMD_ST        	(0x1d0) 
#define OGMA_REG_ADDR_SDB_DATA       		(0x1d4) 

#define OGMA_REG_ADDR_MAC_CMD               (0x11c4)
#define OGMA_REG_ADDR_MAC_DATA              (0x11c0)

#define OGMA_REG_ADDR_MAC_INTF_SEL              (0x11d4)
#define OGMA_REG_ADDR_MAC_DESC_INIT             (0x11fc)
#define OGMA_REG_ADDR_MAC_DESC_SOFT_RST         (0x1204) //OGMA_REG_ADDR_MAC_RSTX
#define OGMA_REG_ADDR_MAC_FLOW_TH               (0x11cc)

#define OGMA_REG_ADDR_IV_INIT_VAL               (0x114)

#define OGMA_REG_ADDR_CNT90K_32                 (0x120c)
#define OGMA_REG_ADDR_CNTADO_32                 (0x1220)
#define OGMA_REG_ADDR_CNT27M_U32                (0x1224)
#define OGMA_REG_ADDR_CNT27M_L32                (0x1228)
#define OGMA_REG_ADDR_CNT_EN                    (0x1210)
#define OGMA_REG_ADDR_CNT_ADO_DIV               (0x1214)

#define OGMA_REG_ADDR_MUTEX_ST               (0x194)
#define OGMA_REG_ADDR_MUTEX_REQ_0            (0x198)
#define OGMA_REG_ADDR_MUTEX_REQ_1            (0x19c)
#define OGMA_REG_ADDR_OGMA_MC_VER            (0x22c)
#define OGMA_REG_ADDR_OGMA_VER               (0x230)

/*bit fields for PKT_CTRL*/
#define OGMA_PKT_CTRL_REG_ETS                   (1UL << 31)
#define OGMA_PKT_CTRL_REG_VJS                   (1UL << 30)
#define OGMA_PKT_CTRL_REG_EN_JUMBO              (1UL << 27)
#define OGMA_PKT_CTRL_REG_EN_ID_REP             (1UL << 26)
#define OGMA_PKT_CTRL_REG_LOG_CHKSUM_ER         (1UL << 3 )
#define OGMA_PKT_CTRL_REG_LOG_HD_INCOMPLETE     (1UL << 2 )
#define OGMA_PKT_CTRL_REG_LOG_HD_ER             (1UL << 1 )

/*bit fields for STRM_CTRL*/
#define OGMA_STRM_CTRL_BUF_CLR                   (1UL << 31)
#define OGMA_STRM_CTRL_REG_TTS                   (1UL << 29)
#define OGMA_STRM_CTRL_REG_RFM                   (1UL << 28)
#define OGMA_STRM_CTRL_REG_JRM                   (1UL << 27)
#define OGMA_STRM_CTRL_REG_PPM                   (1UL << 26)

#define OGMA_CLK_EN_REG_DOM_MAC  (1UL << 5)
#define OGMA_CLK_EN_REG_DOM_REDC (1UL << 2)

/*bit fields for PKT_STATUS*/
#define OGMA_PKT_IRQ_REG_TX_DONE_Vsub  (1UL << 26) 
#define OGMA_PKT_IRQ_REG_TX_DONE_TS    (1UL << 28) 


/************************************************************
 * Bit fields
 ************************************************************/
/* bit fields for com_init */
#define OGMA_COM_INIT_REG_DB   (1UL << 2)
#define OGMA_COM_INIT_REG_ALL  (OGMA_COM_INIT_REG_DB)

/* bit fields for soft_rst */
#define OGMA_SOFT_RST_REG_RESET (0)
#define OGMA_SOFT_RST_REG_RUN   (1UL << 31)

/* bit fields for dma_hm_ctrl */
#define OGMA_DMA_CTRL_REG_SW_RST_CM 1UL

/* bit fields for count enable */
#define OGMA_CNT_EN_CNT90K_EN  (1UL << 0)
#define OGMA_CNT_EN_CNT_ADO_EN (1UL << 1)
#define OGMA_CNT_EN_CNT27M_EN  (1UL << 2)

/* bit fields for db_cmd_st */
#define OGMA_DB_CMD_ST_READ   (0)
#define OGMA_DB_CMD_ST_WRITE  (1)
#define OGMA_DB_CMD_ST_BUSY   (1UL << 31)



/* bit fields for tmr_cmd_st */
#define OGMA_TMR_CMD_ST_READ  (0)
#define OGMA_TMR_CMD_ST_WRITE (1)
#define OGMA_TMR_CMD_ST_BUSY  (1UL << 31)

#define OGMA_HARDWARE_VER (0x60001)


#endif /*OGMA_REG_H*/
