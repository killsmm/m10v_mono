/**
 * @file		palladium_test_cnr.c
 * @brief		test code on palladium
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_user_custom.h"
#include "palladium_test.h"
#include "palladium_test_common.h"
#include "palladium_test_interrupt.h"
//#include "_timer.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "im_cnr.h"
#include "im_rdma.h"
#include "jdscnr.h"

// test sample


#define D_IM_CNR_TEST_H_PIXS	(640)
#define D_IM_CNR_TEST_V_PIXS	(480)

#define D_IM_CNR_TEST_IMG_SRC_ADDR	(0x48000000UL)

// Src address(YCC422 Planar format)
#define D_IM_CNR_TEST_IMG_SRC_Y_ADDR	(D_IM_CNR_TEST_IMG_SRC_ADDR)
#define D_IM_CNR_TEST_IMG_SRC_CB_ADDR	(D_IM_CNR_TEST_IMG_SRC_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS)				// 0x4804B000
#define D_IM_CNR_TEST_IMG_SRC_CR_ADDR	(D_IM_CNR_TEST_IMG_SRC_CB_ADDR + ((D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS) / 2))	// 0x48070800
#define D_IM_CNR_TEST_IMG_SRC_BYTES		(D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS * 2)										// 0x96000

// Dst address(YCC422 Planar format)
#define D_IM_CNR_TEST_IMG_DST_ADDR		(D_IM_CNR_TEST_IMG_SRC_ADDR + D_IM_CNR_TEST_IMG_SRC_BYTES)								// 0x48096000

#define D_IM_CNR_TEST_IMG_DST_Y_ADDR	(D_IM_CNR_TEST_IMG_DST_ADDR)
#define D_IM_CNR_TEST_IMG_DST_CB_ADDR	(D_IM_CNR_TEST_IMG_DST_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS)				// 0x480E1000
#define D_IM_CNR_TEST_IMG_DST_CR_ADDR	(D_IM_CNR_TEST_IMG_DST_CB_ADDR + D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS /2)		// 0x48106800
#define D_IM_CNR_TEST_IMG_DST_BYTES		(D_IM_CNR_TEST_H_PIXS * D_IM_CNR_TEST_V_PIXS * 2)										// 0x96000

// Temporary address(for CNR macro use)
#define D_IM_CNR_TEST_IMG_TMP_ADDR		(D_IM_CNR_TEST_IMG_DST_ADDR + D_IM_CNR_TEST_IMG_DST_BYTES)								// 0x4812C000
//#define D_IM_CNR_TEST_IMG_TMP_ADDR		(0x30000000UL)																			// 0x30000000
#define D_IM_CNR_TEST_IMG_TMP_BYTES		(D_IM_CNR_TEST_V_PIXS * 576)															// 0x43800

#define	IO_CNR(ch)		((D_IM_CNR_CH_0 == ((ch))) ? &IO_CNR1 : (D_IM_CNR_CH_1 == ((ch))) ? &IO_CNR2 : &IO_CNR3)

static UCHAR gIM_CNR_Clk_Ctrl_Cnt1;
static UCHAR gIM_CNR_Clk_Ctrl_Cnt2;
static UCHAR gIM_CNR_Iclk_Ctrl_Cnt1;
static UCHAR gIM_CNR_Iclk_Ctrl_Cnt2;


static VOID pt_im_cnr_ofl_reg_rw_print( UCHAR ch )
{
	Ddim_Print(("OFL_REG_RW ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.SPRSR.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRSR.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.ACESEN.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.ACESEN.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.SPRTRG.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRTRG.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.SPRICE.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRICE.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.SPRICF.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRICF.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.VDIV.word      = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.VDIV.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.SPRAXISET.word = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRAXISET.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.SPRMOD.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRMOD.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.HSIZE.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.HSIZE.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.VSIZE.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.VSIZE.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.RYTA.word      = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RYTA.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.RYDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RYDEF.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.RCBTA.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RCBTA.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.RCRTA.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RCRTA.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.RCDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RCDEF.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.WYTA.word      = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WYTA.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.WYDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WYDEF.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.WCBTA.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WCBTA.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.WCRTA.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WCRTA.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.WCDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WCDEF.word ));
	Ddim_Print(("IO_CNR(%d)->OFL_REG_RW.TMPTA.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.TMPTA.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_reg_rw_print( UCHAR ch )
{
	Ddim_Print(("OTF_REG_RW ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.SPRSR.word     = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRSR.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.ACESEN.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.ACESEN.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.SPRTRG.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRTRG.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.SPRICE.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRICE.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.SPRICF.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRICF.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.HDIV.word      = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.HDIV.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.SPRAXISET.word = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRAXISET.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.SPRMOD.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRMOD.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.HSIZE.word     = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.HSIZE.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.VSIZE.word     = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.VSIZE.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.WCTA.word      = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.WCTA.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.WCDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.WCDEF.word ));
	Ddim_Print(("IO_CNR(%d)->OTF_REG_RW.CNRWOFSX.word  = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.CNRWOFSX.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_cspr_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CSPREN.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CSPREN.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CGDKEN.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CGDKEN.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCEN.word     = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCEN.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYDYM.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYDYM.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYDYMDT.word = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYDYMDT.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCL.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCL.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCL.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCEN.word     = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCEN.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYDYM.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYDYM.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYDYMDT.word = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYDYMDT.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCL.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCL.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCL.word   = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.WHTMD.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.WHTMD.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.BLEND.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.BLEND.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CSPREN.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CSPREN.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CGDKEN.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CGDKEN.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCEN.word     = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCEN.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYDYM.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYDYM.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYDYMDT.word = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYDYMDT.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCL.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCL.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCL.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCL.word ));

	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCEN.word     = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCEN.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYDYM.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYDYM.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYDYMDT.word = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYDYMDT.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCL.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCL.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCL.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCL.word   = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCL.word ));

	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.WHTMD.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.WHTMD.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.BLEND.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.BLEND.word ));
	Im_CNR_Off_Pclk( ch );
}


static VOID pt_im_cnr_ofl_yspr_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_YSPR_REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPREN.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPREN.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRMODE.word  = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRMODE.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEY.word  = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEY.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEC.word  = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRFXEC.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRALPBD.word = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRALPBD.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_yspr_tbl_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_YSPR_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYBD.YSPRDYEYBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYOF.YSPRDYEYOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_REG_YSPR.YSPRDYEYGA.YSPRDYEYGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_cspr_mid_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_MID_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCLBD.MCYSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCLOF.MCYSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCYSCLGA.MCYSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_mid_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_MID_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCLBD.MCYSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCLOF.MCYSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCYSCLGA.MCYSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_cspr_mid_cc_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_MID_CC_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCLBD.MCCSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCLOF.MCCSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCCSCLGA.MCCSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_mid_cc_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_MID_CC_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCLBD.MCCSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCLOF.MCCSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCCSCLGA.MCCSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_cspr_mid_cc_c_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_MID_CC_C_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCLBD.MCSSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCLOF.MCSSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.MCSSCLGA.MCSSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_mid_cc_c_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_MID_CC_C_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCLBD.MCSSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCLOF.MCSSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.MCSSCLGA.MCSSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}


static VOID pt_im_cnr_ofl_cspr_low_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_LOW_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCLBD.LCYSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCLOF.LCYSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCYSCLGA.LCYSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_low_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_LOW_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCLBD.LCYSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCLOF.LCYSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCYSCLGA.LCYSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}


static VOID pt_im_cnr_ofl_cspr_low_cc_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_LOW_CC_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCLBD.LCCSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCLOF.LCCSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCCSCLGA.LCCSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_low_cc_y_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_LOW_CC_Y_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCLBD.LCCSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCLOF.LCCSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCCSCLGA.LCCSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}


static VOID pt_im_cnr_ofl_cspr_low_cc_c_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_LOW_CC_C_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCLBD.LCSSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCLOF.LCSSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.LCSSCLGA.LCSSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_low_cc_c_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_LOW_CC_C_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCLBD.LCSSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCLOF.LCSSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.LCSSCLGA.LCSSCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}


static VOID pt_im_cnr_ofl_cspr_edge_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_EDGE_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CESCLBD.CESCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CESCLOF.CESCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CESCLGA.CESCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_edge_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_EDGE_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CESCLBD.CESCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CESCLOF.CESCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CESCLGA.CESCLGA2.word ));
	Im_CNR_Off_Pclk( ch );
}


static VOID pt_im_cnr_ofl_cspr_hue_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_HUE_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CPHPT.word  = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CPHPT.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CPHPTK.word = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CPHPTK.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLBD.CDSCLBD3.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLOF.CDSCLOF3.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.CDSCLGA.CDSCLGA3.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_hue_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_HUE_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CPHPT.word  = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CPHPT.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CPHPTK.word = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CPHPTK.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLBD.CDSCLBD3.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLOF.CDSCLOF3.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.CDSCLGA.CDSCLGA3.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_cspr_sprs_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_SPRS_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	// Cb
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBBD.SPRSCBBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBOF.SPRSCBOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCBGA.SPRSCBGA3.word ));
	// Cr
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCRBD.SPRSCRBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCROF.SPRSCROF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRSCRGA.SPRSCRGA3.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_sprs_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_SPRS_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	// Cb
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBBD.SPRSCBBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBOF.SPRSCBOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCBGA.SPRSCBGA3.word ));
	// Cr
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCRBD.SPRSCRBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCROF.SPRSCROF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRSCRGA.SPRSCRGA3.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_ofl_cspr_sprd_reg_print( UCHAR ch )
{
	Ddim_Print(("OFL_CSPR_SPRD_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	// Cb
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBBD.SPRDCBBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBOF.SPRDCBOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCBGA.SPRDCBGA3.word ));
	// Cr
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCRBD.SPRDCRBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCROF.SPRDCROF2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA1.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA2.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA3.word    = 0x%lx\n", IO_CNR(ch)->OFL_CSPR_REG.SPRDCRGA.SPRDCRGA3.word ));
	Im_CNR_Off_Pclk( ch );
}

static VOID pt_im_cnr_otf_cspr_sprd_reg_print( UCHAR ch )
{
	Ddim_Print(("OTF_CSPR_SPRD_TABLE REG ch%d print -----------\n", ch ));
	Im_CNR_On_Pclk( ch );
	// Cb
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBBD.SPRDCBBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBOF.SPRDCBOF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCBGA.SPRDCBGA3.word ));
	// Cr
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCRBD.SPRDCRBD2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCROF.SPRDCROF2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA1.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA1.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA2.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA2.word ));
	Ddim_Print(("IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA3.word    = 0x%lx\n", IO_CNR(ch)->OTF_CSPR_REG.SPRDCRGA.SPRDCRGA3.word ));
	Im_CNR_Off_Pclk( ch );
}



#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "1_1"
static VOID pt_im_cnr_ofl_1_1( UCHAR ch )
{
	INT32 ret;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	ret = Im_CNR_OFL_Init(ch);

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "Im_CNR_Init = %d\n", ret));

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "1_1"
static VOID pt_im_cnr_otf_1_1( UCHAR ch )
{
	INT32 ret;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	ret = Im_CNR_OTF_Init(ch);

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	Ddim_Print(( "Im_CNR_Init = %d\n", ret));

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "1_2"
static VOID pt_im_cnr_ofl_1_2( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_CNR_TEST_IMG_SRC_ADDR, D_IM_CNR_TEST_IMG_SRC_BYTES );

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));


	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Open error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl_C error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl_Y error = 0x%x\n", im_ercd ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	im_ercd = Im_CNR_OFL_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Start error = 0x%x\n", im_ercd ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	im_ercd = Im_CNR_OFL_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Stop error = 0x%x\n", im_ercd ));
	}


	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Close error = 0x%x\n", im_ercd ));
	}

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_CNR_TEST_IMG_DST_ADDR, D_IM_CNR_TEST_IMG_DST_BYTES );

	pt_im_cnr_ofl_reg_rw_print( ch );

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "1_2"
static VOID pt_im_cnr_otf_1_2( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num= 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_CNR_TEST_IMG_SRC_ADDR, D_IM_CNR_TEST_IMG_SRC_BYTES );

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));


	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Open error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl_C error = 0x%x\n", im_ercd ));
		return ;
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_set( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("ct_im_cnr_set_im_r2y_set error = 0x%x\n", im_ercd ));
		return ;
	}
	im_ercd = ct_im_cnr_set_im_r2y_start( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("ct_im_cnr_set_im_r2y_start error = 0x%x\n", im_ercd ));
		return ;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	im_ercd = Im_CNR_OTF_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Start error = 0x%x\n", im_ercd ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	im_ercd = Im_CNR_OTF_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Stop error = 0x%x\n", im_ercd ));
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_stop( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("ct_im_cnr_set_im_r2y_stop error = 0x%x\n", im_ercd ));
		return ;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Close error = 0x%x\n", im_ercd ));
	}

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_CNR_TEST_IMG_DST_ADDR, D_IM_CNR_TEST_IMG_DST_BYTES );

	pt_im_cnr_otf_reg_rw_print( ch );

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "1_3"
static VOID pt_im_cnr_ofl_1_3( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OFL_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OFL_YCC_MODE_YCC422,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.r_y_width = D_IM_CNR_TEST_H_PIXS,
		.r_y_addr = D_IM_CNR_TEST_IMG_SRC_Y_ADDR,

		.r_c_width = D_IM_CNR_TEST_H_PIXS,
		.r_cb_addr = D_IM_CNR_TEST_IMG_SRC_CB_ADDR,
		.r_cr_addr = D_IM_CNR_TEST_IMG_SRC_CR_ADDR,

		.w_y_width = D_IM_CNR_TEST_H_PIXS,
		.w_y_addr = D_IM_CNR_TEST_IMG_DST_Y_ADDR,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,
		.w_cb_addr = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
		.w_cr_addr = D_IM_CNR_TEST_IMG_DST_CR_ADDR,

		.work_addr = D_IM_CNR_TEST_IMG_TMP_ADDR,

		.divide_top = E_IM_CNR_VPROCESS_NORM,
		.divide_bot = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_OFL_CTRL_Y spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0,
	};

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_CNR_TEST_IMG_SRC_ADDR, D_IM_CNR_TEST_IMG_SRC_BYTES );

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	im_ercd = Im_CNR_OFL_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Open error = 0x%x\n", im_ercd ));
		return;
	}

	im_ercd = Im_CNR_OFL_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", im_ercd ));
		return;
	}

	im_ercd = Im_CNR_OFL_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl_C error = 0x%x\n", im_ercd ));
		return;
	}

	im_ercd = Im_CNR_OFL_Ctrl_Y( ch, &spr_ctrl_y );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl_Y error = 0x%x\n", im_ercd ));
		return;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	im_ercd = Im_CNR_OFL_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Start error = 0x%x\n", im_ercd ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	im_ercd = Im_CNR_OFL_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_CNR_OFL_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Close error = 0x%x\n", im_ercd ));
	}

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_CNR_TEST_IMG_DST_ADDR, D_IM_CNR_TEST_IMG_DST_BYTES );

	pt_im_cnr_ofl_reg_rw_print( ch );

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "1_3"
static VOID pt_im_cnr_otf_1_3( UCHAR ch )
{
	INT32 im_ercd;
	T_IM_CNR_OTF_CTRL spr_ctrl = {
		.mode = E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE,

		.width = D_IM_CNR_TEST_H_PIXS,
		.lines = D_IM_CNR_TEST_V_PIXS,

		.w_c_width = D_IM_CNR_TEST_H_PIXS,

		.divide_left = E_IM_CNR_VPROCESS_NORM,
		.divide_right = E_IM_CNR_VPROCESS_NORM,

		.pCallBack = NULL,
	};
	T_IM_CNR_OTF_OUTADDR_INFO addr_info = {
		.use_bank_num= 1,
		.w_c_addr[0] = D_IM_CNR_TEST_IMG_DST_CB_ADDR,
	};
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_DISABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};

	// 入力データ
	Palladium_Set_In_Localstack( D_IM_CNR_TEST_IMG_SRC_ADDR, D_IM_CNR_TEST_IMG_SRC_BYTES );

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	im_ercd = Im_CNR_OTF_Open( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Open error = 0x%x\n", im_ercd ));
		return;
	}

	im_ercd = Im_CNR_OTF_Ctrl( ch, &spr_ctrl );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", im_ercd ));
		return;
	}

	im_ercd = Im_CNR_OTF_Set_OutAddr_Info( ch, &addr_info );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(( "Set OutAddrInfo error = 0x%x\n", im_ercd ));
		return ;
	}

	im_ercd = Im_CNR_OTF_Ctrl_C( ch, &spr_ctrl_c );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Ctrl_C error = 0x%x\n", im_ercd ));
		return;
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_set( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("ct_im_cnr_set_im_r2y_set error = 0x%x\n", im_ercd ));
		return ;
	}
	im_ercd = ct_im_cnr_set_im_r2y_start( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("ct_im_cnr_set_im_r2y_start error = 0x%x\n", im_ercd ));
		return ;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	im_ercd = Im_CNR_OTF_Start_Sync( ch, 100 );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Start error = 0x%x\n", im_ercd ));
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	im_ercd = Im_CNR_OTF_Stop(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Stop error = 0x%x\n", im_ercd ));
	}

#ifdef D_IM_CNR_R2Y_MACRO_ENABLE
	im_ercd = ct_im_cnr_set_im_r2y_stop( ch );
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("ct_im_cnr_set_im_r2y_stop error = 0x%x\n", im_ercd ));
		return ;
	}
#endif	// D_IM_CNR_R2Y_MACRO_ENABLE

	im_ercd = Im_CNR_OTF_Close(ch);
	if( im_ercd != D_DDIM_OK ) {
		Ddim_Print(("Close error = 0x%x\n", im_ercd ));
	}

	// 出力ダンプ
	Palladium_Set_Out_Localstack( D_IM_CNR_TEST_IMG_DST_ADDR, D_IM_CNR_TEST_IMG_DST_BYTES );

	pt_im_cnr_otf_reg_rw_print( ch );

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_1"
static VOID pt_im_cnr_ofl_2_1( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_RDMA_DATA_CTRL_C	rdma_ctrl_c;
	const  T_IM_CNR_RDMA_ADDR_CTRL_C*	cnr_ctrl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_Ctrl_C( ch, &cnr_ctrl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_C error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_Ctrl_C( &spr_ctrl_c, &rdma_ctrl_c );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_C error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)cnr_ctrl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_ctrl_c;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_reg_print( 0 );
		pt_im_cnr_ofl_cspr_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_1"
static VOID pt_im_cnr_otf_2_1( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_C spr_ctrl_c = {
		.enable = E_IM_CNR_ONOFF_ENABLE,
		.mid_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
		.low_freq_nr = {
			.enable = E_IM_CNR_ONOFF_ENABLE,
			.h_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_y_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.h_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.v_c_threshold = {
				0xFF, 0xFF, 0xFF, 0xFF,
			},
			.y_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
			.c_threshold_scl_en = E_IM_CNR_ONOFF_DISABLE,
		},
	};
	T_IM_CNR_RDMA_DATA_CTRL_C	rdma_ctrl_c;
	const  T_IM_CNR_RDMA_ADDR_CTRL_C*	cnr_ctrl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_Ctrl_C( ch, &cnr_ctrl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_Ctrl_C error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_Ctrl_C( &spr_ctrl_c, &rdma_ctrl_c );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_Ctrl_C error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)cnr_ctrl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_ctrl_c;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_reg_print( 0 );
		pt_im_cnr_otf_cspr_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}



#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_2"
static VOID pt_im_cnr_ofl_2_2( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_OFL_CTRL_Y			spr_ctrl_y = {
		.enable = E_IM_CNR_ONOFF_DISABLE,
		.threshold_type = E_IM_CNR_THRES_REGISTER,
		.level = E_IM_CNR_LEVEL_HIGH,
		.h_y_threshold = 0xFF,
		.v_y_threshold = 0xFF,
		.h_c_threshold = 0xFF,
		.v_c_threshold = 0xFF,
		.alpha_blend_ratio = 0x1F,
	};
	T_IM_CNR_RDMA_DATA_OFL_CTRL_Y	rdma_ctrl_y;
	const  T_IM_CNR_RDMA_ADDR_OFL_CTRL_Y*	cnr_ctrl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_Ctrl_Y( ch, &cnr_ctrl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_Y error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_Ctrl_Y( &spr_ctrl_y, &rdma_ctrl_y );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_Ctrl_Y error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)cnr_ctrl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_ctrl_y;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_yspr_reg_print( 0 );
		pt_im_cnr_ofl_yspr_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_yspr_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_3"
static VOID pt_im_cnr_ofl_2_3( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	yspr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_OFL_YSPR_TBL	rdma_yspr_tbl;
	const  T_IM_CNR_RDMA_ADDR_OFL_YSPR_TBL*	yspr_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_YSPR_Table( ch, &yspr_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_YSPR_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_YSPR_Table( &yspr_table, &rdma_yspr_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_YSPR_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)yspr_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_yspr_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_yspr_tbl_reg_print( 0 );
		pt_im_cnr_ofl_yspr_tbl_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_yspr_tbl_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_4"
static VOID pt_im_cnr_ofl_2_4( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL	rdma_mid_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL*	mid_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table( ch, &mid_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table( &cnr_table, &rdma_mid_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_mid_y_reg_print( 0 );
		pt_im_cnr_ofl_cspr_mid_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_mid_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_4"
static VOID pt_im_cnr_otf_2_4( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_Y_TBL	rdma_mid_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_Y_TBL*	mid_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table( ch, &mid_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table( &cnr_table, &rdma_mid_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_mid_y_reg_print( 0 );
		pt_im_cnr_otf_cspr_mid_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_mid_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_5"
static VOID pt_im_cnr_ofl_2_5( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL	rdma_mid_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL*	mid_cc_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_Y_Table( ch, &mid_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_Y_Table( &cnr_table, &rdma_mid_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_mid_cc_y_reg_print( 0 );
		pt_im_cnr_ofl_cspr_mid_cc_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_mid_cc_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_5"
static VOID pt_im_cnr_otf_2_5( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_Y_TBL	rdma_mid_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_Y_TBL*	mid_cc_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_Y_Table( ch, &mid_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_Y_Table( &cnr_table, &rdma_mid_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_mid_cc_y_reg_print( 0 );
		pt_im_cnr_otf_cspr_mid_cc_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_mid_cc_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_6"
static VOID pt_im_cnr_ofl_2_6( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL	rdma_mid_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL*	mid_cc_c_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table( ch, &mid_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table( &cnr_table, &rdma_mid_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_mid_cc_c_reg_print( 0 );
		pt_im_cnr_ofl_cspr_mid_cc_c_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_mid_cc_c_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_6"
static VOID pt_im_cnr_otf_2_6( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_MID_CC_C_TBL	rdma_mid_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_MID_CC_C_TBL*	mid_cc_c_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table( ch, &mid_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table( &cnr_table, &rdma_mid_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Mid_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)mid_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_mid_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_mid_cc_c_reg_print( 0 );
		pt_im_cnr_otf_cspr_mid_cc_c_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_mid_cc_c_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_7"
static VOID pt_im_cnr_ofl_2_7( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL	rdma_low_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL*	low_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table( ch, &low_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table( &cnr_table, &rdma_low_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_low_y_reg_print( 0 );
		pt_im_cnr_ofl_cspr_low_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_low_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_7"
static VOID pt_im_cnr_otf_2_7( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_Y_TBL	rdma_low_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_Y_TBL*	low_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table( ch, &low_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table( &cnr_table, &rdma_low_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Low_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_low_y_reg_print( 0 );
		pt_im_cnr_otf_cspr_low_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_low_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_8"
static VOID pt_im_cnr_ofl_2_8( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL	rdma_low_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL*	low_cc_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table( ch, &low_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table( &cnr_table, &rdma_low_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_low_cc_y_reg_print( 0 );
		pt_im_cnr_ofl_cspr_low_cc_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_low_cc_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_8"
static VOID pt_im_cnr_otf_2_8( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_Y_TBL	rdma_low_cc_y_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_Y_TBL*	low_cc_y_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table( ch, &low_cc_y_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table( &cnr_table, &rdma_low_cc_y_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_Y_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_y_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_y_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_low_cc_y_reg_print( 0 );
		pt_im_cnr_otf_cspr_low_cc_y_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_low_cc_y_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_9"
static VOID pt_im_cnr_ofl_2_9( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL	rdma_low_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL*	low_cc_c_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table( ch, &low_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table( &cnr_table, &rdma_low_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_low_cc_c_reg_print( 0 );
		pt_im_cnr_ofl_cspr_low_cc_c_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_low_cc_c_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_9"
static VOID pt_im_cnr_otf_2_9( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_LOW_CC_C_TBL	rdma_low_cc_c_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_LOW_CC_C_TBL*	low_cc_c_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table( ch, &low_cc_c_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table( &cnr_table, &rdma_low_cc_c_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Low_CC_C_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)low_cc_c_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_low_cc_c_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_low_cc_c_reg_print( 0 );
		pt_im_cnr_otf_cspr_low_cc_c_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_low_cc_c_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_10"
static VOID pt_im_cnr_ofl_2_10( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL	rdma_edge_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL*	edge_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table( ch, &edge_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table( &cnr_table, &rdma_edge_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)edge_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_edge_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_edge_reg_print( 0 );
		pt_im_cnr_ofl_cspr_edge_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_edge_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_10"
static VOID pt_im_cnr_otf_2_10( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_TABLE	cnr_table = {
		.bd1 = 0xFF,
		.bd2 = 0xFF,
		.bd3 = 0xFF,
		.bd4 = 0xFF,
		.bd5 = 0xFF,
		.of0 = 0xFF,
		.of1 = 0xFF,
		.of2 = 0xFF,
		.of3 = 0xFF,
		.of4 = 0xFF,
		.of5 = 0xFF,
		.gain0 = 0x7F,
		.gain1 = 0x7F,
		.gain2 = 0x7F,
		.gain3 = 0x7F,
		.gain4 = 0x7F,
		.gain5 = 0x7F,
	};
	T_IM_CNR_RDMA_DATA_CSPR_EDGE_TBL	rdma_edge_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_EDGE_TBL*	edge_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table( ch, &edge_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table( &cnr_table, &rdma_edge_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Edge_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)edge_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_edge_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_edge_reg_print( 0 );
		pt_im_cnr_otf_cspr_edge_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_edge_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_11"
static VOID pt_im_cnr_ofl_2_11( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_HUE_SPEC_TABLE	spr_cspr_hue = {
		.cd_cb = 0xFF,
		.cd_cr = 0xFF,
		.k_cb = 0x3FF,
		.k_cr = 0x3FF,
		.bd1 = 0x7FF,
		.bd2 = 0x7FF,
		.bd3 = 0x7FF,
		.bd4 = 0x7FF,
		.bd5 = 0x7FF,
		.of0 = 0x100,
		.of1 = 0x100,
		.of2 = 0x100,
		.of3 = 0x100,
		.of4 = 0x100,
		.of5 = 0x100,
		.gain0 = 0x07FF,
		.gain1 = 0x07FF,
		.gain2 = 0x07FF,
		.gain3 = 0x07FF,
		.gain4 = 0x07FF,
		.gain5 = 0x07FF
	};
	T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL	rdma_hue_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL*	hue_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table( ch, &hue_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table( &spr_cspr_hue, &rdma_hue_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)hue_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_hue_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_hue_reg_print( 0 );
		pt_im_cnr_ofl_cspr_hue_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_hue_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_11"
static VOID pt_im_cnr_otf_2_11( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_HUE_SPEC_TABLE	spr_cspr_hue = {
		.cd_cb = 0xFF,
		.cd_cr = 0xFF,
		.k_cb = 0x3FF,
		.k_cr = 0x3FF,
		.bd1 = 0x7FF,
		.bd2 = 0x7FF,
		.bd3 = 0x7FF,
		.bd4 = 0x7FF,
		.bd5 = 0x7FF,
		.of0 = 0x100,
		.of1 = 0x100,
		.of2 = 0x100,
		.of3 = 0x100,
		.of4 = 0x100,
		.of5 = 0x100,
		.gain0 = 0x07FF,
		.gain1 = 0x07FF,
		.gain2 = 0x07FF,
		.gain3 = 0x07FF,
		.gain4 = 0x07FF,
		.gain5 = 0x07FF
	};
	T_IM_CNR_RDMA_DATA_CSPR_HUE_TBL	rdma_hue_tbl;
	const  T_IM_CNR_RDMA_ADDR_CSPR_HUE_TBL*	hue_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table( ch, &hue_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table( &spr_cspr_hue, &rdma_hue_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_Hue_Spec_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)hue_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_hue_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_hue_reg_print( 0 );
		pt_im_cnr_otf_cspr_hue_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_hue_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_12"
static VOID pt_im_cnr_ofl_2_12( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprs_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRS_CC_TBL	rdma_sprs_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL*	sprs_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table( ch, &sprs_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table( &cspr_sprs_cc, &rdma_sprs_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprs_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprs_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_sprs_reg_print( 0 );
		pt_im_cnr_ofl_cspr_sprs_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_sprs_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_12"
static VOID pt_im_cnr_otf_2_12( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprs_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRS_CC_TBL	rdma_sprs_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRS_CC_TBL*	sprs_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table( ch, &sprs_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table( &cspr_sprs_cc, &rdma_sprs_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_SPRS_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprs_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprs_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_sprs_reg_print( 0 );
		pt_im_cnr_otf_cspr_sprs_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_sprs_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}


#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_13"
static VOID pt_im_cnr_ofl_2_13( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprd_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRD_CC_TBL	rdma_sprd_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL*	sprd_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table( ch, &sprd_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OFL_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table( &cspr_sprd_cc, &rdma_sprd_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OFL_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprd_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprd_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_ofl_cspr_sprd_reg_print( 0 );
		pt_im_cnr_ofl_cspr_sprd_reg_print( 1 );
	}
	else{
		pt_im_cnr_ofl_cspr_sprd_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OFL test " D_IM_CNR_TEST_NO " end\n" ));
}

#undef D_IM_CNR_TEST_NO
#define D_IM_CNR_TEST_NO "2_13"
static VOID pt_im_cnr_otf_2_13( UCHAR ch )
{
	INT32 ret;
	T_IM_RDMA_CTRL rdma_ctrl;
	T_IM_CNR_CTRL_CC_SRC_DST	cspr_sprd_cc = {
		.enable = (E_IM_CNR_ONOFF)1,
		.cb_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		},
		.cr_table = {
			.bd1 = 0xFF,
			.bd2 = 0xFF,
			.bd3 = 0xFF,
			.bd4 = 0xFF,
			.bd5 = 0xFF,
			.of0 = 0xFF,
			.of1 = 0xFF,
			.of2 = 0xFF,
			.of3 = 0xFF,
			.of4 = 0xFF,
			.of5 = 0xFF,
			.gain0 = 0x07FF,
			.gain1 = 0x07FF,
			.gain2 = 0x07FF,
			.gain3 = 0x07FF,
			.gain4 = 0x07FF,
			.gain5 = 0x07FF
		}
	};
	T_IM_CNR_RDMA_DATA_SPRD_CC_TBL	rdma_sprd_tbl;
	const  T_IM_CNR_RDMA_ADDR_SPRD_CC_TBL*	sprd_tbl_addr;
	ULONG	size;

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " begin\n" ));

	ret = Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table( ch, &sprd_tbl_addr, &size );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Get_RdmaAddr_OTF_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	ret = Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table( &cspr_sprd_cc, &rdma_sprd_tbl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Im_CNR_Set_RdmaValue_OTF_CSPR_SPRD_CC_Table error = 0x%x\n", ret ));
		return ;
	}

	// タイマー起動
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_START );

	rdma_ctrl.transfer_byte		= size;
	rdma_ctrl.reg_addr_tbl_addr	= (ULONG)sprd_tbl_addr;
	rdma_ctrl.reg_data_top_addr	= (ULONG)&rdma_sprd_tbl;
	rdma_ctrl.req_threshold		= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode			= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack			= NULL;
	ret = Im_RDMA_Ctrl_Quick_Start_Sync( D_DDIM_WAIT_END_TIME, &rdma_ctrl );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("Ctrl error = 0x%x\n", ret ));
		return ;
	}

	// タイマー停止
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_END );

	// 出力ダンプ
	if( ch == D_IM_CNR_CH_2 ){
		pt_im_cnr_otf_cspr_sprd_reg_print( 0 );
		pt_im_cnr_otf_cspr_sprd_reg_print( 1 );
	}
	else{
		pt_im_cnr_otf_cspr_sprd_reg_print( ch );
	}

	Ddim_Print(( "*** CNR OTF test " D_IM_CNR_TEST_NO " end\n" ));
}

static VOID pt_im_cnr_run_5( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			pt_im_cnr_ofl_1_1(0);
			break;
		case 2:
			pt_im_cnr_ofl_1_1(0);
			pt_im_cnr_ofl_1_2(0);
			break;
		case 3:
			pt_im_cnr_ofl_1_1(0);
			pt_im_cnr_ofl_1_3(0);
			break;
		case 4:
			pt_im_cnr_ofl_1_1(1);
			break;
		case 5:
			pt_im_cnr_ofl_1_1(1);
			pt_im_cnr_ofl_1_2(1);
			break;
		case 6:
			pt_im_cnr_ofl_1_1(1);
			pt_im_cnr_ofl_1_3(1);
			break;
		case 7:
			pt_im_cnr_otf_1_1(0);
			break;
		case 8:
			pt_im_cnr_otf_1_1(0);
			pt_im_cnr_otf_1_2(0);
			break;
		case 9:
			pt_im_cnr_otf_1_1(0);
			pt_im_cnr_otf_1_3(0);
			break;
		case 10:
			pt_im_cnr_otf_1_1(1);
			break;
		case 11:
			pt_im_cnr_otf_1_1(1);
			pt_im_cnr_otf_1_2(1);
			break;
		case 12:
			pt_im_cnr_otf_1_1(1);
			pt_im_cnr_otf_1_3(1);
			break;

		default:
			Ddim_Print(("*** CNR test skip\n" ));
			break;
	}
}

static VOID pt_im_cnr_run_6( const UINT32 type1 )
{
	UCHAR			type2;

	type2	= gDDIM_Info.com._6b;

	switch( type2 ) {
		case 1:
			// Ctrl_C (OFL)
			pt_im_cnr_ofl_2_1(0);
			pt_im_cnr_ofl_2_1(1);
			pt_im_cnr_ofl_2_1(2);
			break;
		case 2:
			// Ctrl_Y (OFL)
			pt_im_cnr_ofl_2_2(0);
			pt_im_cnr_ofl_2_2(1);
			pt_im_cnr_ofl_2_2(2);
			break;
		case 3:
			// YSPR Table (OFL)
			pt_im_cnr_ofl_2_3(0);
			pt_im_cnr_ofl_2_3(1);
			pt_im_cnr_ofl_2_3(2);
			break;
		case 4:
			// CSPR Mid-Y Table (OFL)
			pt_im_cnr_ofl_2_4(0);
			pt_im_cnr_ofl_2_4(1);
			pt_im_cnr_ofl_2_4(2);
			break;
		case 5:
			// CSPR Mid-CC-Y Table (OFL)
			pt_im_cnr_ofl_2_5(0);
			pt_im_cnr_ofl_2_5(1);
			pt_im_cnr_ofl_2_5(2);
			break;
		case 6:
			// CSPR Mid-CC-C Table (OFL)
			pt_im_cnr_ofl_2_6(0);
			pt_im_cnr_ofl_2_6(1);
			pt_im_cnr_ofl_2_6(2);
			break;
		case 7:
			// CSPR Low-Y Table (OFL)
			pt_im_cnr_ofl_2_7(0);
			pt_im_cnr_ofl_2_7(1);
			pt_im_cnr_ofl_2_7(2);
			break;
		case 8:
			// CSPR Low-CC-Y Table (OFL)
			pt_im_cnr_ofl_2_8(0);
			pt_im_cnr_ofl_2_8(1);
			pt_im_cnr_ofl_2_8(2);
			break;
		case 9:
			// CSPR Low-CC-C Table (OFL)
			pt_im_cnr_ofl_2_9(0);
			pt_im_cnr_ofl_2_9(1);
			pt_im_cnr_ofl_2_9(2);
			break;
		case 10:
			// CSPR Edge Specified Table (OFL)
			pt_im_cnr_ofl_2_10(0);
			pt_im_cnr_ofl_2_10(1);
			pt_im_cnr_ofl_2_10(2);
			break;
		case 11:
			// CSPR Hue Specified Table (OFL)
			pt_im_cnr_ofl_2_11(0);
			pt_im_cnr_ofl_2_11(1);
			pt_im_cnr_ofl_2_11(2);
			break;
		case 12:
			// CSPR SPRS_CC Table (OFL)
			pt_im_cnr_ofl_2_12(0);
			pt_im_cnr_ofl_2_12(1);
			pt_im_cnr_ofl_2_12(2);
			break;
		case 13:
			// CSPR SPRD_CC Table (OFL)
			pt_im_cnr_ofl_2_13(0);
			pt_im_cnr_ofl_2_13(1);
			pt_im_cnr_ofl_2_13(2);
			break;
		case 14:
			// Ctrl_C (OTF)
			pt_im_cnr_otf_2_1(0);
			pt_im_cnr_otf_2_1(1);
			pt_im_cnr_otf_2_1(2);
			break;
		case 15:
			// CSPR Mid-Y Table (OTF)
			pt_im_cnr_otf_2_4(0);
			pt_im_cnr_otf_2_4(1);
			pt_im_cnr_otf_2_4(2);
			break;
		case 16:
			// CSPR Mid-CC-Y Table (OTF)
			pt_im_cnr_otf_2_5(0);
			pt_im_cnr_otf_2_5(1);
			pt_im_cnr_otf_2_5(2);
			break;
		case 17:
			// CSPR Mid-CC-C Table (OTF)
			pt_im_cnr_otf_2_6(0);
			pt_im_cnr_otf_2_6(1);
			pt_im_cnr_otf_2_6(2);
			break;
		case 18:
			// CSPR Low-Y Table (OTF)
			pt_im_cnr_otf_2_7(0);
			pt_im_cnr_otf_2_7(1);
			pt_im_cnr_otf_2_7(2);
			break;
		case 19:
			// CSPR Low-CC-Y Table (OTF)
			pt_im_cnr_otf_2_8(0);
			pt_im_cnr_otf_2_8(1);
			pt_im_cnr_otf_2_8(2);
			break;
		case 20:
			// CSPR Low-CC-C Table (OTF)
			pt_im_cnr_otf_2_9(0);
			pt_im_cnr_otf_2_9(1);
			pt_im_cnr_otf_2_9(2);
			break;
		case 21:
			// CSPR Edge Specified Table (OTF)
			pt_im_cnr_otf_2_10(0);
			pt_im_cnr_otf_2_10(1);
			pt_im_cnr_otf_2_10(2);
			break;
		case 22:
			// CSPR Hue Specified Table (OTF)
			pt_im_cnr_otf_2_11(0);
			pt_im_cnr_otf_2_11(1);
			pt_im_cnr_otf_2_11(2);
			break;
		case 23:
			// CSPR SPRS_CC Table (OTF)
			pt_im_cnr_otf_2_12(0);
			pt_im_cnr_otf_2_12(1);
			pt_im_cnr_otf_2_12(2);
			break;
		case 24:
			// CSPR SPRD_CC Table (OTF)
			pt_im_cnr_otf_2_13(0);
			pt_im_cnr_otf_2_13(1);
			pt_im_cnr_otf_2_13(2);
			break;

		default:
			Ddim_Print(("*** CNR test skip\n" ));
			break;
	}
}

VOID Pt_Im_CNR_Main(void)
{
	UCHAR	type1;

//	Dd_GIC_Init();
	Dd_GIC_Ctrl( E_DD_GIC_INTID_TIMER_CH0_INT, 1, D_DD_GIC_PRI30, 1 );		//cpu=0, intid=118, enable, pri=30
	Dd_GIC_Ctrl( E_DD_GIC_INTID_CNR_UNIT1_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable
	Dd_GIC_Ctrl( E_DD_GIC_INTID_CNR_UNIT2_INT, 1, D_DD_GIC_PRI30, 1 );		// int enable

	Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1CK_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Clk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2CK_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt1, &Dd_Top_Get_CLKSTOP11(), ~D_DD_TOP_CNR1AX_BIT );
	Dd_Top_Start_Clock( (UCHAR*)&gIM_CNR_Iclk_Ctrl_Cnt2, &Dd_Top_Get_CLKSTOP13(), ~D_DD_TOP_CNR2AX_BIT );


	Ddim_Print(("CNR START\n"));

	// タイマー初期化
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_INIT );

	// テストパラメータ取得
	type1	= gDDIM_Info.com._6a;

	switch (type1){
		case 5:
			pt_im_cnr_run_5(type1);
			break;

		case 6:
			pt_im_cnr_run_6(type1);
			break;

		default:
			break;
	}

	// タイマーログ出力
	Palladium_Timer( P_TIMER_ID_COMMON0, P_TIMER_PRINT );

	Ddim_Print(("CNR END\n"));

	// テスト強制終了処理
	Palladium_Force_Stop();
}

