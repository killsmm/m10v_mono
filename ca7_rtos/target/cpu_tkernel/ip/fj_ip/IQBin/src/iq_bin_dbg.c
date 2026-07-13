
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "iq_bin_dbg.h"
#include "im_pro.h"
#include "im_b2r.h"
#include "im_r2y.h"
#include "im_cnr.h"
#include "im_shdr.h"

#include "jdsr2y.h"
#include "jdscnr.h"

#include "fj_filesystem.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
//#define D_IQ_DBG_SAVE_FILE

#ifdef D_IQ_DBG_SAVE_FILE
#define D_IQ_DUMP_DPC
#define D_IQ_DUMP_ELF
#define D_IQ_DUMP_CIA
#define D_IQ_DUMP_HFA
#define D_IQ_DUMP_TC
#define D_IQ_DUMP_CCM1
#define D_IQ_DUMP_GAM
#define D_IQ_DUMP_EE
#define D_IQ_DUMP_CLPF
#define D_IQ_DUMP_CSCO
#define D_IQ_DUMP_CNR_OTF
#define D_IQ_DUMP_CNR_OFL
#define D_IQ_DUMP_CAG
#define D_IQ_DUMP_FSHD
#define D_IQ_DUMP_TBL
#endif

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
E_IQ_DBG_DUMP gIQDump_Flag = E_IQ_DBG_DUMP_NONE;
UCHAR gIQDump_Dol_Enable = 0;

#ifdef D_IQ_DBG_SAVE_FILE
UCHAR gIQDump_Data_Buf[0x0400];
#endif
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
#ifdef D_IQ_DBG_SAVE_FILE
static void iqdbg_save_file( CHAR* fname, ULONG addr, ULONG size );
#endif

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 IQDbg_Dump_IQ_DPC(void)
{
	UCHAR unit_no;
#ifdef D_IQ_DUMP_DPC
	ULONG address, size;
#endif

	UPRINTF( "[SRO]DPC Macro:\n" );
	for(unit_no = 0; unit_no < 2; unit_no++)
	{
	    UPRINTF( "DPCTRG[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCTRG.word );
	    UPRINTF( "DPCPAEN[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCPAEN.word );
	    UPRINTF( "DPCGWP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCGWP.word );
	    UPRINTF( "DPCGWS[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCGWS.word );
	    UPRINTF( "DPCWP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCWP.word );
	    UPRINTF( "DPCWS[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCWS.word );
	    UPRINTF( "DPCMD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCMD.word );
	    UPRINTF( "DPCBLP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCBLP.word );
	    UPRINTF( "DPCBLM[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DPCBLM.word );
	    UPRINTF( "DFCTL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCTL.word );
		UPRINTF( "DFS.DFS1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS1.word );
		UPRINTF( "DFS.DFS2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS2.word );
		UPRINTF( "DFS.DFS3[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFS.DFS3.word );
		UPRINTF( "DFO.DFO1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO1.word );
		UPRINTF( "DFO.DFO2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO2.word );
		UPRINTF( "DFO.DFO3[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFO.DFO3.word );
		UPRINTF( "DFK.DFK1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK1.word );
		UPRINTF( "DFK.DFK2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK2.word );
		UPRINTF( "DFK.DFK3[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFK.DFK3.word );
		UPRINTF( "DFH.DFH1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH1.word );
		UPRINTF( "DFH.DFH2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH2.word );
		UPRINTF( "DFH.DFH3[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFH.DFH3.word );
		UPRINTF( "DFCSR[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFCSR.word );
		UPRINTF( "DFLSTH.DFLSTH1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFLSTH.DFLSTH1.word );
		UPRINTF( "DFLSTH.DFLSTH2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFLSTH.DFLSTH2.word );
		UPRINTF( "DFAGEF[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGEF.word );
		UPRINTF( "DFAGEF2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGEF2.word );
		UPRINTF( "DFAGTH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGTH.word );
		UPRINTF( "DFAGTHK[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DFAGTHK.word );
		UPRINTF( "DMCTL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMCTL.word );
		UPRINTF( "DMWKS[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMWKS.word );
		UPRINTF( "DMWKD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMWKD.word );
		UPRINTF( "DMAGK[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DMAGK.word );
		UPRINTF( "DECTL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DECTL.word );
		UPRINTF( "DENSS[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSS.word );
		UPRINTF( "DENSO[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSO.word );
		UPRINTF( "DENSK[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSK.word );
		UPRINTF( "DENSH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSH.word );
		UPRINTF( "DENSMGN[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENSMGN.word );
		UPRINTF( "DENDCTH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DENDCTH.word );
		UPRINTF( "DEAGK[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DEAGK.word );
		UPRINTF( "DCAGEF[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DCAGEF.word );
		UPRINTF( "DCAGTH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.DPC.DCAGTH.word );
	}

#ifdef D_IQ_DUMP_DPC
	size = 0x0100;
	address = (ULONG)&IO_PRO.IMG_PIPE[0].SRO.DPC;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_DPC_0.BIN", address, size);
	address = (ULONG)&IO_PRO.IMG_PIPE[1].SRO.DPC;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_DPC_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_ELF(void)
{
	UCHAR unit_no;
#ifdef D_IQ_DUMP_ELF
	ULONG address, size;
#endif

	UPRINTF( "[B2B]ELF Macro:\n" );
	for(unit_no = 0; unit_no < 2; unit_no++)
	{
	    UPRINTF( "ELFTRG[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.word );
	    UPRINTF( "NSLTRG[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NSLTRG.word );
	    UPRINTF( "ELFPAEN[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFPAEN.word );
	    UPRINTF( "ELFGWP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFGWP.word );
	    UPRINTF( "ELFGWS[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFGWS.word );
	    UPRINTF( "ELFWP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFWP.word );
	    UPRINTF( "ELFWS[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFWS.word );
	    UPRINTF( "ELFCTL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.word );
	    UPRINTF( "ELFZP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFZP.word );
		UPRINTF( "ELFL.ELFL1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFL.ELFL1.word );
		UPRINTF( "ELFL.ELFL2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFL.ELFL2.word );
		UPRINTF( "ANBOR.ANBOR1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOR.ANBOR1.word );
		UPRINTF( "ANBOR.ANBOR2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOR.ANBOR2.word );
		UPRINTF( "ANBKR.ANBKR1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKR.ANBKR1.word );
		UPRINTF( "ANBKR.ANBKR2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKR.ANBKR2.word );
		UPRINTF( "ANBDR.ANBDR1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDR.ANBDR1.word );
		UPRINTF( "ANBDR.ANBDR2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDR.ANBDR2.word );
		UPRINTF( "ANBOG.ANBOG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOG.ANBOG1.word );
		UPRINTF( "ANBOG.ANBOG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOG.ANBOG2.word );
		UPRINTF( "ANBKG.ANBKG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKG.ANBKG1.word );
		UPRINTF( "ANBKG.ANBKG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKG.ANBKG2.word );
		UPRINTF( "ANBDG.ANBDG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDG.ANBDG1.word );
		UPRINTF( "ANBDG.ANBDG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDG.ANBDG2.word );
		UPRINTF( "ANBOB.ANBOB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOB.ANBOB1.word );
		UPRINTF( "ANBOB.ANBOB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOB.ANBOB2.word );
		UPRINTF( "ANBKB.ANBKB[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKB.ANBKB1.word );
		UPRINTF( "ANBKB.ANBKB[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKB.ANBKB2.word );
		UPRINTF( "ANBDB.ANBDB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDB.ANBDB1.word );
		UPRINTF( "ANBDB.ANBDB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDB.ANBDB2.word );
		UPRINTF( "LGTW.LGTW1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTW.LGTW1.word );
		UPRINTF( "LGTW.LGTW2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTW.LGTW2.word );
		UPRINTF( "LGT[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGT.word );
		UPRINTF( "LGTE[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTE.word );
		UPRINTF( "EDG2ANBA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDG2ANBA.word );
		UPRINTF( "EDGSCLA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGSCLA.word );
		UPRINTF( "EDG2ANBB[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDG2ANBB.word );
		UPRINTF( "EDGSCLB[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGSCLB.word );
		UPRINTF( "EDG2ANBC[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDG2ANBC.word );
		UPRINTF( "EDGSCLC[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGSCLC.word );
		UPRINTF( "EFTH.EFTH1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EFTH.EFTH1.word );
		UPRINTF( "EFTH.EFTH2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EFTH.EFTH2.word );
		UPRINTF( "LFTH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LFTH.word );
		UPRINTF( "ELCOME[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELCOME.word );
		UPRINTF( "ELCOMEN[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELCOMEN.word );
		UPRINTF( "A1S5E.A1S5E1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5E.A1S5E1.word );
		UPRINTF( "A1S5E.A1S5E2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5E.A1S5E2.word );
		UPRINTF( "A1S5L1.A1S5L11[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L1.A1S5L11.word );
		UPRINTF( "A1S5L1.A1S5L12[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L1.A1S5L12.word );
		UPRINTF( "A1S5L2.A1S5L21[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L2.A1S5L21.word );
		UPRINTF( "A1S5L2.A1S5L22[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L2.A1S5L22.word );
		UPRINTF( "A1S5L3.A1S5L31[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L3.A1S5L31.word );
		UPRINTF( "A1S5L3.A1S5L32[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L3.A1S5L32.word );
		UPRINTF( "A1S5L4.A1S5L41[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L4.A1S5L41.word );
		UPRINTF( "A1S5L4.A1S5L42[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L4.A1S5L42.word );
		UPRINTF( "NR1S5RGB.NR1S5RGB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR1S5RGB.NR1S5RGB1.word );
		UPRINTF( "NR1S5RGB.NR1S5RGB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR1S5RGB.NR1S5RGB2.word );
		UPRINTF( "NS1S5LM[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS1S5LM.word );
		UPRINTF( "A5S9E.A5S9E1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9E.A5S9E1.word );
		UPRINTF( "A5S9E.A5S9E2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9E.A5S9E2.word );
		UPRINTF( "A5S9L1.A5S9L11[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L1.A5S9L11.word );
		UPRINTF( "A5S9L1.A5S9L12[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L1.A5S9L12.word );
		UPRINTF( "A5S9L2.A5S9L21[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L2.A5S9L21.word );
		UPRINTF( "A5S9L2.A5S9L22[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L2.A5S9L22.word );
		UPRINTF( "A5S9L3.A5S9L31[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L3.A5S9L31.word );
		UPRINTF( "A5S9L3.A5S9L32[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L3.A5S9L32.word );
		UPRINTF( "A5S9L4.A5S9L41[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L4.A5S9L41.word );
		UPRINTF( "A5S9L4.A5S9L42[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L4.A5S9L42.word );
		UPRINTF( "NR5S5RGB.NR5S9RGB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR5S9RGB.NR5S9RGB1.word );
		UPRINTF( "NR5S5RGB.NR5S9RGB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR5S9RGB.NR5S9RGB2.word );
		UPRINTF( "NS5S9LM[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS5S9LM.word );
		UPRINTF( "FS9W.FS9W1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W1.word );
		UPRINTF( "FS9W.FS9W2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W2.word );
		UPRINTF( "FS9YC.FS9YC1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC1.word );
		UPRINTF( "FS9YC.FS9YC2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC2.word );
		UPRINTF( "FS9YC.FS9YC3[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC3.word );
		UPRINTF( "FS9YC.FS9YC4[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC4.word );
		UPRINTF( "FS9YC.FS9YC5[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC5.word );
		UPRINTF( "FASTR[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FASTR.word );
		UPRINTF( "CRVAF[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVAF.word );
		UPRINTF( "FCRVABTOF[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTOF.word );
		UPRINTF( "FCRVABTGA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTGA.word );
		UPRINTF( "FCRVABTBD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTBD.word );
		UPRINTF( "FCRVABTCP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTCP.word );
		UPRINTF( "FYABTGA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTGA.word );
		UPRINTF( "FYABTBD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTBD.word );
		UPRINTF( "FYABTCLP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTCLP.word );
		UPRINTF( "FAEHH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FAEHH.word );
		UPRINTF( "FBSTR[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FBSTR.word );
		UPRINTF( "CRVBF[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVBF.word );
		UPRINTF( "FCRVBBTOF[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTOF.word );
		UPRINTF( "FCRVBBTGA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTGA.word );
		UPRINTF( "FCRVBBTBD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTBD.word );
		UPRINTF( "FCRVBBTCP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTCP.word );
		UPRINTF( "FYBBTGA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTGA.word );
		UPRINTF( "FYBBTBD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTBD.word );
		UPRINTF( "FYBBTCLP[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FBEHH.word );
		UPRINTF( "FBEHH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS5S9LM.word );
		UPRINTF( "ELFMNNRRL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRRL.word );
		UPRINTF( "ELFMNNRGL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRGL.word );
		UPRINTF( "ELFMNNRBL[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRBL.word );
		UPRINTF( "ELFAFNR[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFAFNR.word );
		UPRINTF( "ZELADCMD[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZELADCMD.word );
		UPRINTF( "ZANBOR.ZANBOR1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOR.ZANBOR1.word );
		UPRINTF( "ZANBOR.ZANBOR2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOR.ZANBOR2.word );
		UPRINTF( "ZANBKR.ZANBKR1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKR.ZANBKR1.word );
		UPRINTF( "ZANBKR.ZANBKR2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKR.ZANBKR2.word );
		UPRINTF( "ZANBDR.ZANBDR1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDR.ZANBDR1.word );
		UPRINTF( "ZANBDR.ZANBDR2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDR.ZANBDR2.word );
		UPRINTF( "ZANBOG.ZANBOG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOG.ZANBOG1.word );
		UPRINTF( "ZANBOG.ZANBOG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOG.ZANBOG2.word );
		UPRINTF( "ZANBKG.ZANBKG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKG.ZANBKG1.word );
		UPRINTF( "ZANBKG.ZANBKG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKG.ZANBKG2.word );
		UPRINTF( "ZANBDG.ZANBDG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDG.ZANBDG1.word );
		UPRINTF( "ZANBDG.ZANBDG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDG.ZANBDG2.word );
		UPRINTF( "ZANBOB.ZANBOB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOB.ZANBOB1.word );
		UPRINTF( "ZANBOB.ZANBOB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOB.ZANBOB2.word );
		UPRINTF( "ZANBKB.ZANBKB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKB.ZANBKB1.word );
		UPRINTF( "ZANBKB.ZANBKB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKB.ZANBKB2.word );
		UPRINTF( "ZANBDB.ZANBDB1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDB.ZANBDB1.word );
		UPRINTF( "ZANBDB.ZANBDB2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDB.ZANBDB2.word );
		UPRINTF( "ZLGTW.ZLGTW1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTW.ZLGTW1.word );
		UPRINTF( "ZLGTW.ZLGTW2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTW.ZLGTW2.word );
		UPRINTF( "ZLGT[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGT.word );
		UPRINTF( "ZLGTE[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTE.word );
		UPRINTF( "ZEDG2ANBA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDG2ANBA.word );
		UPRINTF( "ZEDGSCLA[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDGSCLA.word );
		UPRINTF( "ZEDG2ANBB[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDG2ANBB.word );
		UPRINTF( "ZEDGSCLB[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDGSCLB.word );
		UPRINTF( "ZALNGE.ZALNGE1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNGE.ZALNGE1.word );
		UPRINTF( "ZALNGE.ZALNGE2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNGE.ZALNGE2.word );
		UPRINTF( "ZALNG.ZALNG1[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG1.word );
		UPRINTF( "ZALNG.ZALNG2[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG2.word );
		UPRINTF( "ZALNG.ZALNG3[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG3.word );
		UPRINTF( "ZALNG.ZALNG4[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG4.word );
		UPRINTF( "ZALNG.ZALNG5[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG5.word );
		UPRINTF( "ZALNG.ZALNG6[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG6.word );
		UPRINTF( "ZLFTH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLFTH.word );
		UPRINTF( "ZLCOME[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLCOME.word );
		UPRINTF( "ZLCOMEN[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLCOMEN.word );
		UPRINTF( "ZNSLNGLM[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZNSLNGLM.word );
		UPRINTF( "EDGCATH[%d] 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGCATH.word );
	}

#ifdef D_IQ_DUMP_ELF
	size = 0x0400;
	memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)&IO_PRO.IMG_PIPE[0].B2B.ELF, size);
	address = (ULONG)&gIQDump_Data_Buf;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_ELF_0.BIN", address, size);
	memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)&IO_PRO.IMG_PIPE[1].B2B.ELF, size);
	address = (ULONG)&gIQDump_Data_Buf;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_ELF_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CIA(void)
{
#ifdef D_IQ_DUMP_CIA
	ULONG address, size;
#endif

	UPRINTF( "[B2R]CIA Macro:\n" );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPA      = 0x%x\n", IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPA );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPB      = 0x%x\n", IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPB );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPC      = 0x%x\n", IO_B2R_P1.F_B2R.CIA.SHARP.bit.SHARPC );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYA    = 0x%x\n", IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYA );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYB    = 0x%x\n", IO_B2R_P1.F_B2R.CIA.AIRESY.bit.AIRESYB );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.LF.bit.LFSTR          = 0x%x\n", IO_B2R_P1.F_B2R.CIA.LF.bit.LFSTR );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.LF.bit.LFTH           = 0x%x\n", IO_B2R_P1.F_B2R.CIA.LF.bit.LFTH );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCSCL        = 0x%x\n", IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCSCL );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCLMT        = 0x%x\n", IO_B2R_P1.F_B2R.CIA.CMC.bit.CMCLMT );
	UPRINTF( "IO_B2R_P1.F_B2R.CIA.DIRRLITH.bit.DIRRLITH = 0x%x\n", IO_B2R_P1.F_B2R.CIA.DIRRLITH.bit.DIRRLITH );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPA      = 0x%x\n", IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPA );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPB      = 0x%x\n", IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPB );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPC      = 0x%x\n", IO_B2R_P2.F_B2R.CIA.SHARP.bit.SHARPC );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYA    = 0x%x\n", IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYA );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYB    = 0x%x\n", IO_B2R_P2.F_B2R.CIA.AIRESY.bit.AIRESYB );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.LF.bit.LFSTR          = 0x%x\n", IO_B2R_P2.F_B2R.CIA.LF.bit.LFSTR );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.LF.bit.LFTH           = 0x%x\n", IO_B2R_P2.F_B2R.CIA.LF.bit.LFTH );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCSCL        = 0x%x\n", IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCSCL );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCLMT        = 0x%x\n", IO_B2R_P2.F_B2R.CIA.CMC.bit.CMCLMT );
	UPRINTF( "IO_B2R_P2.F_B2R.CIA.DIRRLITH.bit.DIRRLITH = 0x%x\n", IO_B2R_P2.F_B2R.CIA.DIRRLITH.bit.DIRRLITH );

#ifdef D_IQ_DUMP_CIA
	size = 0x20;
	address = (ULONG)&IO_B2R_P1.F_B2R.CIA;
	iqdbg_save_file((CHAR *)"I:\\IQDUMPI\\IQDUMP_CIA_0.BIN", address, size);
	address = (ULONG)&IO_B2R_P2.F_B2R.CIA;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CIA_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_HFA(void)
{
#ifdef D_IQ_DUMP_HFA
	ULONG address, size;
#endif

	UPRINTF( "[B2R]HFA Macro:\n" );
	UPRINTF( "IO_B2R_P1.HFEEN		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACTL.bit.HFEEN );
	UPRINTF( "IO_B2R_P1.HFAFEN		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACTL.bit.HFAFEN );
	UPRINTF( "IO_B2R_P1.HFAYEN		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACTL.bit.HFAYEN );
	UPRINTF( "IO_B2R_P1.HFK[0]		   = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[0] );
	UPRINTF( "IO_B2R_P1.HFK[1]		   = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[1] );
	UPRINTF( "IO_B2R_P1.HFK[2]		   = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[2] );
	UPRINTF( "IO_B2R_P1.HFK[3]		   = 0x%08lx\n", IO_B2R_P1.F_B2R.HFA.HFK.word[3] );
	UPRINTF( "IO_B2R_P1.HFK0		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK0 );
	UPRINTF( "IO_B2R_P1.HFK1		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK1 );
	UPRINTF( "IO_B2R_P1.HFK2		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK2 );
	UPRINTF( "IO_B2R_P1.HFK3		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK3 );
	UPRINTF( "IO_B2R_P1.HFK4		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK4 );
	UPRINTF( "IO_B2R_P1.HFK5		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK5 );
	UPRINTF( "IO_B2R_P1.HFK6		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK6 );
	UPRINTF( "IO_B2R_P1.HFK7		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK7 );
	UPRINTF( "IO_B2R_P1.HFK8		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK8 );
	UPRINTF( "IO_B2R_P1.HFK9		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFK.bit.HFK9 );
	UPRINTF( "IO_B2R_P1.HFEFCLPP	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFEFCLP.bit.HFEFCLPP );
	UPRINTF( "IO_B2R_P1.HFEFCLPM	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFEFCLP.bit.HFEFCLPM );
	UPRINTF( "IO_B2R_P1.HFAFSL		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAFST.bit.HFAFSL );
	UPRINTF( "IO_B2R_P1.HFAFTH		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAFST.bit.HFAFTH );
	UPRINTF( "IO_B2R_P1.HFAFGU		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAFGU.bit.HFAFGU );
	UPRINTF( "IO_B2R_P1.HFAFCLPP	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAFCLP.bit.HFAFCLPP );
	UPRINTF( "IO_B2R_P1.HFAFCLPM	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAFCLP.bit.HFAFCLPM );
	UPRINTF( "IO_B2R_P1.HFAYC[0]	   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.word[0] );
	UPRINTF( "IO_B2R_P1.HFAYC[1]	   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.word[1] );
	UPRINTF( "IO_B2R_P1.HFAYC[2]	   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.word[2] );
	UPRINTF( "IO_B2R_P1.HFAYC[3]	   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.word[3] );
	UPRINTF( "IO_B2R_P1.HFAYC00 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC00 );
	UPRINTF( "IO_B2R_P1.HFAYC01 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC01 );
	UPRINTF( "IO_B2R_P1.HFAYC02 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC02 );
	UPRINTF( "IO_B2R_P1.HFAYC10 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC10 );
	UPRINTF( "IO_B2R_P1.HFAYC11 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC11 );
	UPRINTF( "IO_B2R_P1.HFAYC12 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC12 );
	UPRINTF( "IO_B2R_P1.HFAYC20 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC20 );
	UPRINTF( "IO_B2R_P1.HFAYC21 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC21 );
	UPRINTF( "IO_B2R_P1.HFAYC22 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYC.bit.HFAYC22 );
	UPRINTF( "IO_B2R_P1.HFAYOF_0	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_0 );
	UPRINTF( "IO_B2R_P1.HFAYOF_1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_1 );
	UPRINTF( "IO_B2R_P1.HFAYOF_2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_2 );
	UPRINTF( "IO_B2R_P1.HFAYOF_3	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYOF.bit.HFAYOF_3 );
	UPRINTF( "IO_B2R_P1.HFAYGA		   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFAYGA.word );
	UPRINTF( "IO_B2R_P1.HFAYGA_0	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_0 );
	UPRINTF( "IO_B2R_P1.HFAYGA_1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_1 );
	UPRINTF( "IO_B2R_P1.HFAYGA_2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_2 );
	UPRINTF( "IO_B2R_P1.HFAYGA_3	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYGA.bit.HFAYGA_3 );
	UPRINTF( "IO_B2R_P1.HFAYBD_0	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_0 );
	UPRINTF( "IO_B2R_P1.HFAYBD_1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_1 );
	UPRINTF( "IO_B2R_P1.HFAYBD_2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_2 );
	UPRINTF( "IO_B2R_P1.HFAYBD_3	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBD.bit.HFAYBD_3 );
	UPRINTF( "IO_B2R_P1.HFAYDIV1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYDIV.bit.HFAYDIV1 );
	UPRINTF( "IO_B2R_P1.HFAYDIV2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYDIV.bit.HFAYDIV2 );
	UPRINTF( "IO_B2R_P1.HFAYCLPP	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYCLP.bit.HFAYCLPP );
	UPRINTF( "IO_B2R_P1.HFAYCLPM	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYCLP.bit.HFAYCLPM );
	UPRINTF( "IO_B2R_P1.HFACORP 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACOR.bit.HFACORP );
	UPRINTF( "IO_B2R_P1.HFACORM 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACOR.bit.HFACORM );
	UPRINTF( "IO_B2R_P1.HFACLPR 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACLP.bit.HFACLPR );
	UPRINTF( "IO_B2R_P1.HFACLPG 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACLP.bit.HFACLPG );
	UPRINTF( "IO_B2R_P1.HFACLPB 	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACLP.bit.HFACLPB );
	UPRINTF( "IO_B2R_P1.HFACAE		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVCTL.bit.HFACAE );
	UPRINTF( "IO_B2R_P1.HFACBE		   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVCTL.bit.HFACBE );
	UPRINTF( "IO_B2R_P1.HFACRVMRG	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVCTL.bit.HFACRVMRG );
	UPRINTF( "IO_B2R_P1.HFACRVAFX	   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFACVAFX.word );
	UPRINTF( "IO_B2R_P1.HFACRVAFX1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX1 );
	UPRINTF( "IO_B2R_P1.HFACRVAFY1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY1 );
	UPRINTF( "IO_B2R_P1.HFACRVAFX2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX2 );
	UPRINTF( "IO_B2R_P1.HFACRVAFY2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY2 );
	UPRINTF( "IO_B2R_P1.HFACRVASCLOF1  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLOF.bit.HFACRVASCLOF1 );
	UPRINTF( "IO_B2R_P1.HFACRVASCLGA   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLGA.word );
	UPRINTF( "IO_B2R_P1.HFACRVASCLGA_0 = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_0 );
	UPRINTF( "IO_B2R_P1.HFACRVASCLGA_1 = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_1 );
	UPRINTF( "IO_B2R_P1.HFACRVASCLBD1  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLBD.bit.HFACRVASCLBD1 );
	UPRINTF( "IO_B2R_P1.HFACRVASCLCPL  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPL );
	UPRINTF( "IO_B2R_P1.HFACRVASCLCPH  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPH );
	UPRINTF( "IO_B2R_P1.HFAYASCLGA_0   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_0 );
	UPRINTF( "IO_B2R_P1.HFAYASCLGA_1   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_1 );
	UPRINTF( "IO_B2R_P1.HFAYASCLBD_0   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_0 );
	UPRINTF( "IO_B2R_P1.HFAYASCLBD_1   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_1 );
	UPRINTF( "IO_B2R_P1.HFAYASCLCLPL   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPL );
	UPRINTF( "IO_B2R_P1.HFAYASCLCLPH   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPH );
	UPRINTF( "IO_B2R_P1.HFACRVBFX	   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFACVBFX.word );
	UPRINTF( "IO_B2R_P1.HFACRVBFX1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX1 );
	UPRINTF( "IO_B2R_P1.HFACRVBFY1	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY1 );
	UPRINTF( "IO_B2R_P1.HFACRVBFX2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX2 );
	UPRINTF( "IO_B2R_P1.HFACRVBFY2	   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY2 );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLOF1  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLOF.bit.HFACRVBSCLOF1 );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLGA   = 0x%lx\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLGA.word );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLGA_0 = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_0 );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLGA_1 = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_1 );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLBD1  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLBD.bit.HFACRVBSCLBD1 );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLCPL  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPL );
	UPRINTF( "IO_B2R_P1.HFACRVBSCLCPH  = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPH );
	UPRINTF( "IO_B2R_P1.HFAYBSCLGA_0   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_0 );
	UPRINTF( "IO_B2R_P1.HFAYBSCLGA_1   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_1 );
	UPRINTF( "IO_B2R_P1.HFAYBSCLBD_0   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_0 );
	UPRINTF( "IO_B2R_P1.HFAYBSCLBD_1   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_1 );
	UPRINTF( "IO_B2R_P1.HFAYBSCLCLPL   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPL );
	UPRINTF( "IO_B2R_P1.HFAYBSCLCLPH   = 0x%x\n",	 IO_B2R_P1.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPH );
	UPRINTF( "IO_B2R_P2.HFEEN		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACTL.bit.HFEEN );
	UPRINTF( "IO_B2R_P2.HFAFEN		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACTL.bit.HFAFEN );
	UPRINTF( "IO_B2R_P2.HFAYEN		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACTL.bit.HFAYEN );
	UPRINTF( "IO_B2R_P2.HFK[0]		   = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[0] );
	UPRINTF( "IO_B2R_P2.HFK[1]		   = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[1] );
	UPRINTF( "IO_B2R_P2.HFK[2]		   = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[2] );
	UPRINTF( "IO_B2R_P2.HFK[3]		   = 0x%08lx\n", IO_B2R_P2.F_B2R.HFA.HFK.word[3] );
	UPRINTF( "IO_B2R_P2.HFK0		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK0 );
	UPRINTF( "IO_B2R_P2.HFK1		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK1 );
	UPRINTF( "IO_B2R_P2.HFK2		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK2 );
	UPRINTF( "IO_B2R_P2.HFK3		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK3 );
	UPRINTF( "IO_B2R_P2.HFK4		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK4 );
	UPRINTF( "IO_B2R_P2.HFK5		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK5 );
	UPRINTF( "IO_B2R_P2.HFK6		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK6 );
	UPRINTF( "IO_B2R_P2.HFK7		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK7 );
	UPRINTF( "IO_B2R_P2.HFK8		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK8 );
	UPRINTF( "IO_B2R_P2.HFK9		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFK.bit.HFK9 );
	UPRINTF( "IO_B2R_P2.HFEFCLPP	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFEFCLP.bit.HFEFCLPP );
	UPRINTF( "IO_B2R_P2.HFEFCLPM	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFEFCLP.bit.HFEFCLPM );
	UPRINTF( "IO_B2R_P2.HFAFSL		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAFST.bit.HFAFSL );
	UPRINTF( "IO_B2R_P2.HFAFTH		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAFST.bit.HFAFTH );
	UPRINTF( "IO_B2R_P2.HFAFGU		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAFGU.bit.HFAFGU );
	UPRINTF( "IO_B2R_P2.HFAFCLPP	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAFCLP.bit.HFAFCLPP );
	UPRINTF( "IO_B2R_P2.HFAFCLPM	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAFCLP.bit.HFAFCLPM );
	UPRINTF( "IO_B2R_P2.HFAYC[0]	   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.word[0] );
	UPRINTF( "IO_B2R_P2.HFAYC[1]	   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.word[1] );
	UPRINTF( "IO_B2R_P2.HFAYC[2]	   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.word[2] );
	UPRINTF( "IO_B2R_P2.HFAYC[3]	   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.word[3] );
	UPRINTF( "IO_B2R_P2.HFAYC00 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC00 );
	UPRINTF( "IO_B2R_P2.HFAYC01 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC01 );
	UPRINTF( "IO_B2R_P2.HFAYC02 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC02 );
	UPRINTF( "IO_B2R_P2.HFAYC10 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC10 );
	UPRINTF( "IO_B2R_P2.HFAYC11 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC11 );
	UPRINTF( "IO_B2R_P2.HFAYC12 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC12 );
	UPRINTF( "IO_B2R_P2.HFAYC20 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC20 );
	UPRINTF( "IO_B2R_P2.HFAYC21 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC21 );
	UPRINTF( "IO_B2R_P2.HFAYC22 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYC.bit.HFAYC22 );
	UPRINTF( "IO_B2R_P2.HFAYOF_0	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_0 );
	UPRINTF( "IO_B2R_P2.HFAYOF_1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_1 );
	UPRINTF( "IO_B2R_P2.HFAYOF_2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_2 );
	UPRINTF( "IO_B2R_P2.HFAYOF_3	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYOF.bit.HFAYOF_3 );
	UPRINTF( "IO_B2R_P2.HFAYGA		   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFAYGA.word );
	UPRINTF( "IO_B2R_P2.HFAYGA_0	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_0 );
	UPRINTF( "IO_B2R_P2.HFAYGA_1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_1 );
	UPRINTF( "IO_B2R_P2.HFAYGA_2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_2 );
	UPRINTF( "IO_B2R_P2.HFAYGA_3	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYGA.bit.HFAYGA_3 );
	UPRINTF( "IO_B2R_P2.HFAYBD_0	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_0 );
	UPRINTF( "IO_B2R_P2.HFAYBD_1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_1 );
	UPRINTF( "IO_B2R_P2.HFAYBD_2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_2 );
	UPRINTF( "IO_B2R_P2.HFAYBD_3	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBD.bit.HFAYBD_3 );
	UPRINTF( "IO_B2R_P2.HFAYDIV1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYDIV.bit.HFAYDIV1 );
	UPRINTF( "IO_B2R_P2.HFAYDIV2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYDIV.bit.HFAYDIV2 );
	UPRINTF( "IO_B2R_P2.HFAYCLPP	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYCLP.bit.HFAYCLPP );
	UPRINTF( "IO_B2R_P2.HFAYCLPM	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYCLP.bit.HFAYCLPM );
	UPRINTF( "IO_B2R_P2.HFACORP 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACOR.bit.HFACORP );
	UPRINTF( "IO_B2R_P2.HFACORM 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACOR.bit.HFACORM );
	UPRINTF( "IO_B2R_P2.HFACLPR 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACLP.bit.HFACLPR );
	UPRINTF( "IO_B2R_P2.HFACLPG 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACLP.bit.HFACLPG );
	UPRINTF( "IO_B2R_P2.HFACLPB 	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACLP.bit.HFACLPB );
	UPRINTF( "IO_B2R_P2.HFACAE		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVCTL.bit.HFACAE );
	UPRINTF( "IO_B2R_P2.HFACBE		   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVCTL.bit.HFACBE );
	UPRINTF( "IO_B2R_P2.HFACRVMRG	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVCTL.bit.HFACRVMRG );
	UPRINTF( "IO_B2R_P2.HFACRVAFX	   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFACVAFX.word );
	UPRINTF( "IO_B2R_P2.HFACRVAFX1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX1 );
	UPRINTF( "IO_B2R_P2.HFACRVAFY1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY1 );
	UPRINTF( "IO_B2R_P2.HFACRVAFX2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFX2 );
	UPRINTF( "IO_B2R_P2.HFACRVAFY2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVAFX.bit.HFACRVAFY2 );
	UPRINTF( "IO_B2R_P2.HFACRVASCLOF1  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLOF.bit.HFACRVASCLOF1 );
	UPRINTF( "IO_B2R_P2.HFACRVASCLGA   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLGA.word );
	UPRINTF( "IO_B2R_P2.HFACRVASCLGA_0 = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_0 );
	UPRINTF( "IO_B2R_P2.HFACRVASCLGA_1 = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLGA.bit.HFACRVASCLGA_1 );
	UPRINTF( "IO_B2R_P2.HFACRVASCLBD1  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLBD.bit.HFACRVASCLBD1 );
	UPRINTF( "IO_B2R_P2.HFACRVASCLCPL  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPL );
	UPRINTF( "IO_B2R_P2.HFACRVASCLCPH  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVASCLCP.bit.HFACRVASCLCPH );
	UPRINTF( "IO_B2R_P2.HFAYASCLGA_0   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_0 );
	UPRINTF( "IO_B2R_P2.HFAYASCLGA_1   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYASCLGA.bit.HFAYASCLGA_1 );
	UPRINTF( "IO_B2R_P2.HFAYASCLBD_0   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_0 );
	UPRINTF( "IO_B2R_P2.HFAYASCLBD_1   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYASCLBD.bit.HFAYASCLBD_1 );
	UPRINTF( "IO_B2R_P2.HFAYASCLCLPL   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPL );
	UPRINTF( "IO_B2R_P2.HFAYASCLCLPH   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYASCLCLP.bit.HFAYASCLCLPH );
	UPRINTF( "IO_B2R_P2.HFACRVBFX	   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFACVBFX.word );
	UPRINTF( "IO_B2R_P2.HFACRVBFX1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX1 );
	UPRINTF( "IO_B2R_P2.HFACRVBFY1	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY1 );
	UPRINTF( "IO_B2R_P2.HFACRVBFX2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFX2 );
	UPRINTF( "IO_B2R_P2.HFACRVBFY2	   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACVBFX.bit.HFACRVBFY2 );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLOF1  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLOF.bit.HFACRVBSCLOF1 );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLGA   = 0x%lx\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLGA.word );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLGA_0 = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_0 );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLGA_1 = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLGA.bit.HFACRVBSCLGA_1 );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLBD1  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLBD.bit.HFACRVBSCLBD1 );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLCPL  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPL );
	UPRINTF( "IO_B2R_P2.HFACRVBSCLCPH  = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFACRVBSCLCP.bit.HFACRVBSCLCPH );
	UPRINTF( "IO_B2R_P2.HFAYBSCLGA_0   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_0 );
	UPRINTF( "IO_B2R_P2.HFAYBSCLGA_1   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBSCLGA.bit.HFAYBSCLGA_1 );
	UPRINTF( "IO_B2R_P2.HFAYBSCLBD_0   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_0 );
	UPRINTF( "IO_B2R_P2.HFAYBSCLBD_1   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBSCLBD.bit.HFAYBSCLBD_1 );
	UPRINTF( "IO_B2R_P2.HFAYBSCLCLPL   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPL );
	UPRINTF( "IO_B2R_P2.HFAYBSCLCLPH   = 0x%x\n",	 IO_B2R_P2.F_B2R.HFA.HFAYBSCLCLP.bit.HFAYBSCLCLPH );

#ifdef D_IQ_DUMP_HFA
	size = 0x0200;
	memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)&IO_B2R_P1.F_B2R.HFA, size);
	address = (ULONG)&gIQDump_Data_Buf;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_HFA_0.BIN", address, size);
	memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)&IO_B2R_P2.F_B2R.HFA, size);
	address = (ULONG)&gIQDump_Data_Buf;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_HFA_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_TC(void)
{
#ifdef D_IQ_DUMP_TC
	ULONG address, size;
#endif

	UPRINTF( "[R2Y]TC Macro:\n" );
	UPRINTF( "PIPE1\n" );
	UPRINTF( "TCEN	   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCEN );
	UPRINTF( "TCYBEN   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCYBEN );
	UPRINTF( "TCBLEN   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCBLEN );
	UPRINTF( "TCRES    = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCRES );
	UPRINTF( "TCTBL    = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCTBL );
	UPRINTF( "TCYOUT   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCYOUT );
	UPRINTF( "TCINTBIT = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCINTBIT );
	UPRINTF( "TCBLND   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCTL.bit.TCBLND );
	UPRINTF( "TCYC_0_0 = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCYC.bit.TCYC_0_0 );
	UPRINTF( "TCYC_0_1 = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCYC.bit.TCYC_0_1 );
	UPRINTF( "TCYC_0_2 = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCYC.bit.TCYC_0_2 );
	UPRINTF( "TCEP_0   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCEP.bit.TCEP_0 );
	UPRINTF( "TCEP_1   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCEP.bit.TCEP_1 );
	UPRINTF( "TCEP_2   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCEP.bit.TCEP_2 );
	UPRINTF( "TCEP_3   = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCEP.bit.TCEP_3 );
	UPRINTF( "TCCLPRP  = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCLPR.bit.TCCLPRP );
	UPRINTF( "TCCLPRM  = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCLPR.bit.TCCLPRM );
	UPRINTF( "TCCLPGP  = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCLPG.bit.TCCLPGP );
	UPRINTF( "TCCLPGM  = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCLPG.bit.TCCLPGM );
	UPRINTF( "TCCLPBP  = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCLPB.bit.TCCLPBP );
	UPRINTF( "TCCLPBM  = 0x%x\n", IO_R2Y_P1.F_R2Y.TC.TCCLPB.bit.TCCLPBM );
	UPRINTF( "PIPE2\n" );
	UPRINTF( "TCEN	   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCEN );
	UPRINTF( "TCYBEN   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCYBEN );
	UPRINTF( "TCBLEN   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCBLEN );
	UPRINTF( "TCRES    = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCRES );
	UPRINTF( "TCTBL    = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCTBL );
	UPRINTF( "TCYOUT   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCYOUT );
	UPRINTF( "TCINTBIT = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCINTBIT );
	UPRINTF( "TCBLND   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCTL.bit.TCBLND );
	UPRINTF( "TCYC_0_0 = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCYC.bit.TCYC_0_0 );
	UPRINTF( "TCYC_0_1 = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCYC.bit.TCYC_0_1 );
	UPRINTF( "TCYC_0_2 = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCYC.bit.TCYC_0_2 );
	UPRINTF( "TCEP_0   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCEP.bit.TCEP_0 );
	UPRINTF( "TCEP_1   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCEP.bit.TCEP_1 );
	UPRINTF( "TCEP_2   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCEP.bit.TCEP_2 );
	UPRINTF( "TCEP_3   = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCEP.bit.TCEP_3 );
	UPRINTF( "TCCLPRP  = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCLPR.bit.TCCLPRP );
	UPRINTF( "TCCLPRM  = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCLPR.bit.TCCLPRM );
	UPRINTF( "TCCLPGP  = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCLPG.bit.TCCLPGP );
	UPRINTF( "TCCLPGM  = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCLPG.bit.TCCLPGM );
	UPRINTF( "TCCLPBP  = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCLPB.bit.TCCLPBP );
	UPRINTF( "TCCLPBM  = 0x%x\n", IO_R2Y_P2.F_R2Y.TC.TCCLPB.bit.TCCLPBM );

#ifdef D_IQ_DUMP_TC
	size = 0x20;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.TC;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_TC_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.TC;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_TC_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CCM1(void)
{
#ifdef D_IQ_DUMP_CCM1
	ULONG address, size;
#endif

	UPRINTF( "[R2Y]CCM1 Macro:\n" );
	UPRINTF( "PIPE1\n" );
	UPRINTF( "CC1K_0_0	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_0_0 );
	UPRINTF( "CC1K_0_1	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_0_1 );
	UPRINTF( "CC1K_0_2	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_0_2 );
	UPRINTF( "CC1K_1_0	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_1_0 );
	UPRINTF( "CC1K_1_1	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_1_1 );
	UPRINTF( "CC1K_1_2	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_1_2 );
	UPRINTF( "CC1K_2_0	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_2_0 );
	UPRINTF( "CC1K_2_1	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_2_1 );
	UPRINTF( "CC1K_2_2	   = 0x%x\n", IO_R2Y_P1.F_R2Y.CCA1.CC1K.bit.CC1K_2_2 );
	UPRINTF( "PIPE2\n" );
	UPRINTF( "CC1K_0_0	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_0_0 );
	UPRINTF( "CC1K_0_1	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_0_1 );
	UPRINTF( "CC1K_0_2	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_0_2 );
	UPRINTF( "CC1K_1_0	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_1_0 );
	UPRINTF( "CC1K_1_1	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_1_1 );
	UPRINTF( "CC1K_1_2	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_1_2 );
	UPRINTF( "CC1K_2_0	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_2_0 );
	UPRINTF( "CC1K_2_1	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_2_1 );
	UPRINTF( "CC1K_2_2	   = 0x%x\n", IO_R2Y_P2.F_R2Y.CCA1.CC1K.bit.CC1K_2_2 );

#ifdef D_IQ_DUMP_CCM1
	size = 0x80;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.CCA1;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CCM1_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.CCA1;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CCM1_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_GAM(void)
{
#ifdef D_IQ_DUMP_GAM
	ULONG address, size;
#endif

	UPRINTF( "[R2Y]GAM Macro:\n" );
	UPRINTF( "PIPE1\n" );
	UPRINTF( "GMEN	= 0x%x\n", IO_R2Y_P1.F_R2Y.GAM.GMCTL.bit.GMEN );
	UPRINTF( "GMMD	= 0x%x\n", IO_R2Y_P1.F_R2Y.GAM.GMCTL.bit.GMMD );
	UPRINTF( "GAMSW = 0x%x\n", IO_R2Y_P1.F_R2Y.GAM.GMCTL.bit.GAMSW );
	UPRINTF( "PIPE2\n" );
	UPRINTF( "GMEN	= 0x%x\n", IO_R2Y_P2.F_R2Y.GAM.GMCTL.bit.GMEN );
	UPRINTF( "GMMD	= 0x%x\n", IO_R2Y_P2.F_R2Y.GAM.GMCTL.bit.GMMD );
	UPRINTF( "GAMSW = 0x%x\n", IO_R2Y_P2.F_R2Y.GAM.GMCTL.bit.GAMSW );

#ifdef D_IQ_DUMP_GAM
	size = 0x20;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.GAM;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_GAM_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.GAM;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_GAM_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_EE(void)
{
#ifdef D_IQ_DUMP_EE
	ULONG address, size;
#endif

	UPRINTF( "[R2Y]EE Macro:\n" );
	UPRINTF( "PIPE1\n" );
	UPRINTF( "EGSMT	   = 0x%x\n", IO_R2Y_P1.F_R2Y.EGSM.EGSMCTL.bit.EGSMT );
	UPRINTF( "EGSMTTH   = 0x%x\n", IO_R2Y_P1.F_R2Y.EGSM.EGSMTT.bit.EGSMTTH );
	UPRINTF( "EGSMTTXGA = 0x%x\n", IO_R2Y_P1.F_R2Y.EGSM.EGSMTT.bit.EGSMTTXGA );
	UPRINTF( "EGHWEN 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWEN );
	UPRINTF( "EGHWMP 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWMP );
	UPRINTF( "EGHWTC 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWTC );
	UPRINTF( "EGHWSCLRES   = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWSCLRES );
	UPRINTF( "EGHWSCLTBL   = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWSCLTBL );
	UPRINTF( "EGHWTCRES	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWTCRES );
	UPRINTF( "EGHWTCTBL	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWTCTBL );
	UPRINTF( "EGHWNRLV	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGHW.EGHWCTL.bit.EGHWNRLV );
	UPRINTF( "EGHWHPFK[0]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.word[0] );
	UPRINTF( "EGHWHPFK[1]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.word[1] );
	UPRINTF( "EGHWHPFK0	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK0 );
	UPRINTF( "EGHWHPFK1	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK1 );
	UPRINTF( "EGHWHPFK2	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK2 );
	UPRINTF( "EGHWHPFK3	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK3 );
	UPRINTF( "EGHWHPFK4	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK4 );
	UPRINTF( "EGHWHPFK5	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK5 );
	UPRINTF( "EGHWCORPOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_0 );
	UPRINTF( "EGHWCORPOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_1 );
	UPRINTF( "EGHWCORPOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_2 );
	UPRINTF( "EGHWCORPOF_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_3 );
	UPRINTF( "EGHWCORPGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPGA.word );
	UPRINTF( "EGHWCORPGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_0 );
	UPRINTF( "EGHWCORPGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_1 );
	UPRINTF( "EGHWCORPGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_2 );
	UPRINTF( "EGHWCORPGA_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_3 );
	UPRINTF( "EGHWCORPBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPBD.bit.EGHWCORPBD_1 );
	UPRINTF( "EGHWCORPBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPBD.bit.EGHWCORPBD_2 );
	UPRINTF( "EGHWCORPBD_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORPBD.bit.EGHWCORPBD_3 );
	UPRINTF( "EGHWCORMOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_0 );
	UPRINTF( "EGHWCORMOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_1 );
	UPRINTF( "EGHWCORMOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_2 );
	UPRINTF( "EGHWCORMOF_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_3 );
	UPRINTF( "EGHWCORMGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMGA.word );
	UPRINTF( "EGHWCORMGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_0 );
	UPRINTF( "EGHWCORMGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_1 );
	UPRINTF( "EGHWCORMGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_2 );
	UPRINTF( "EGHWCORMGA_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_3 );
	UPRINTF( "EGHWCORMBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMBD.bit.EGHWCORMBD_1 );
	UPRINTF( "EGHWCORMBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMBD.bit.EGHWCORMBD_2 );
	UPRINTF( "EGHWCORMBD_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCORMBD.bit.EGHWCORMBD_3 );
	UPRINTF( "EGHWSCLSUP   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWSCLSUP.bit.EGHWSCLSUP );
	UPRINTF( "EGHWCLPPOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPOF.bit.EGHWCLPPOF_0 );
	UPRINTF( "EGHWCLPPOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPOF.bit.EGHWCLPPOF_1 );
	UPRINTF( "EGHWCLPPOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPOF.bit.EGHWCLPPOF_2 );
	UPRINTF( "EGHWCLPPGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPGA.word );
	UPRINTF( "EGHWCLPPGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPGA.bit.EGHWCLPPGA_0 );
	UPRINTF( "EGHWCLPPGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPGA.bit.EGHWCLPPGA_1 );
	UPRINTF( "EGHWCLPPGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPGA.bit.EGHWCLPPGA_2 );
	UPRINTF( "EGHWCLPPBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPBD.bit.EGHWCLPPBD_1 );
	UPRINTF( "EGHWCLPPBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPPBD.bit.EGHWCLPPBD_2 );
	UPRINTF( "EGHWCLPMOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMOF.bit.EGHWCLPMOF_0 );
	UPRINTF( "EGHWCLPMOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMOF.bit.EGHWCLPMOF_1 );
	UPRINTF( "EGHWCLPMOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMOF.bit.EGHWCLPMOF_2 );
	UPRINTF( "EGHWCLPMGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMGA.word );
	UPRINTF( "EGHWCLPMGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMGA.bit.EGHWCLPMGA_0 );
	UPRINTF( "EGHWCLPMGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMGA.bit.EGHWCLPMGA_1 );
	UPRINTF( "EGHWCLPMGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMGA.bit.EGHWCLPMGA_2 );
	UPRINTF( "EGHWCLPMBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMBD.bit.EGHWCLPMBD_1 );
	UPRINTF( "EGHWCLPMBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGHW.EGHWCLPMBD.bit.EGHWCLPMBD_2 );
	UPRINTF( "EGMWEN 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWEN );
	UPRINTF( "EGMWMP 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWMP );
	UPRINTF( "EGMWTC 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWTC );
	UPRINTF( "EGMWSCLRES   = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWSCLRES );
	UPRINTF( "EGMWSCLTBL   = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWSCLTBL );
	UPRINTF( "EGMWTCRES	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWTCRES );
	UPRINTF( "EGMWTCTBL	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWTCTBL );
	UPRINTF( "EGMWNRLV	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGMW.EGMWCTL.bit.EGMWNRLV );
	UPRINTF( "EGMWHPFK[0]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.word[0] );
	UPRINTF( "EGMWHPFK[1]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.word[1] );
	UPRINTF( "EGMWHPFK0	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK0 );
	UPRINTF( "EGMWHPFK1	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK1 );
	UPRINTF( "EGMWHPFK2	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK2 );
	UPRINTF( "EGMWHPFK3	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK3 );
	UPRINTF( "EGMWHPFK4	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK4 );
	UPRINTF( "EGMWHPFK5	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK5 );
	UPRINTF( "EGMWCORPOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_0 );
	UPRINTF( "EGMWCORPOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_1 );
	UPRINTF( "EGMWCORPOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_2 );
	UPRINTF( "EGMWCORPOF_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_3 );
	UPRINTF( "EGMWCORPGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPGA.word );
	UPRINTF( "EGMWCORPGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_0 );
	UPRINTF( "EGMWCORPGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_1 );
	UPRINTF( "EGMWCORPGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_2 );
	UPRINTF( "EGMWCORPGA_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_3 );
	UPRINTF( "EGMWCORPBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPBD.bit.EGMWCORPBD_1 );
	UPRINTF( "EGMWCORPBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPBD.bit.EGMWCORPBD_2 );
	UPRINTF( "EGMWCORPBD_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORPBD.bit.EGMWCORPBD_3 );
	UPRINTF( "EGMWCORMOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_0 );
	UPRINTF( "EGMWCORMOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_1 );
	UPRINTF( "EGMWCORMOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_2 );
	UPRINTF( "EGMWCORMOF_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_3 );
	UPRINTF( "EGMWCORMGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMGA.word );
	UPRINTF( "EGMWCORMGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_0 );
	UPRINTF( "EGMWCORMGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_1 );
	UPRINTF( "EGMWCORMGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_2 );
	UPRINTF( "EGMWCORMGA_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_3 );
	UPRINTF( "EGMWCORMBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMBD.bit.EGMWCORMBD_1 );
	UPRINTF( "EGMWCORMBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMBD.bit.EGMWCORMBD_2 );
	UPRINTF( "EGMWCORMBD_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCORMBD.bit.EGMWCORMBD_3 );
	UPRINTF( "EGMWSCLSUP   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWSCLSUP.bit.EGMWSCLSUP );
	UPRINTF( "EGMWCLPPOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPOF.bit.EGMWCLPPOF_0 );
	UPRINTF( "EGMWCLPPOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPOF.bit.EGMWCLPPOF_1 );
	UPRINTF( "EGMWCLPPOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPOF.bit.EGMWCLPPOF_2 );
	UPRINTF( "EGMWCLPPGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPGA.word );
	UPRINTF( "EGMWCLPPGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPGA.bit.EGMWCLPPGA_0 );
	UPRINTF( "EGMWCLPPGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPGA.bit.EGMWCLPPGA_1 );
	UPRINTF( "EGMWCLPPGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPGA.bit.EGMWCLPPGA_2 );
	UPRINTF( "EGMWCLPPBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPBD.bit.EGMWCLPPBD_1 );
	UPRINTF( "EGMWCLPPBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPPBD.bit.EGMWCLPPBD_2 );
	UPRINTF( "EGMWCLPMOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMOF.bit.EGMWCLPMOF_0 );
	UPRINTF( "EGMWCLPMOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMOF.bit.EGMWCLPMOF_1 );
	UPRINTF( "EGMWCLPMOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMOF.bit.EGMWCLPMOF_2 );
	UPRINTF( "EGMWCLPMGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMGA.word );
	UPRINTF( "EGMWCLPMGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMGA.bit.EGMWCLPMGA_0 );
	UPRINTF( "EGMWCLPMGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMGA.bit.EGMWCLPMGA_1 );
	UPRINTF( "EGMWCLPMGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMGA.bit.EGMWCLPMGA_2 );
	UPRINTF( "EGMWCLPMBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMBD.bit.EGMWCLPMBD_1 );
	UPRINTF( "EGMWCLPMBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGMW.EGMWCLPMBD.bit.EGMWCLPMBD_2 );
	UPRINTF( "EGLWEN 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWEN );
	UPRINTF( "EGLWMP 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWMP );
	UPRINTF( "EGLWTC 	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWTC );
	UPRINTF( "EGLWSCLRES   = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWSCLRES );
	UPRINTF( "EGLWSCLTBL   = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWSCLTBL );
	UPRINTF( "EGLWTCRES	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWTCRES );
	UPRINTF( "EGLWTCTBL	  = %u\n",	  IO_R2Y_P1.F_R2Y.EGLW.EGLWCTL.bit.EGLWTCTBL );
	UPRINTF( "EGLWHPFK[0]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.word[0] );
	UPRINTF( "EGLWHPFK[1]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.word[1] );
	UPRINTF( "EGLWHPFK[2]  = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.word[2] );
	UPRINTF( "EGLWHPFK0	  = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK0 );
	UPRINTF( "EGLWHPFK_1   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK1 );
	UPRINTF( "EGLWHPFK_2   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK2 );
	UPRINTF( "EGLWHPFK_3   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK3 );
	UPRINTF( "EGLWHPFK_4   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK4 );
	UPRINTF( "EGLWHPFK_5   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK5 );
	UPRINTF( "EGLWHPFK_6   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK6 );
	UPRINTF( "EGLWHPFK_7   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK7 );
	UPRINTF( "EGLWHPFK_8   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK8 );
	UPRINTF( "EGLWHPFK_9   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK9 );
	UPRINTF( "EGLWCORPOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_0 );
	UPRINTF( "EGLWCORPOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_1 );
	UPRINTF( "EGLWCORPOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_2 );
	UPRINTF( "EGLWCORPOF_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_3 );
	UPRINTF( "EGLWCORPGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPGA.word );
	UPRINTF( "EGLWCORPGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_0 );
	UPRINTF( "EGLWCORPGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_1 );
	UPRINTF( "EGLWCORPGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_2 );
	UPRINTF( "EGLWCORPGA_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_3 );
	UPRINTF( "EGLWCORPBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPBD.bit.EGLWCORPBD_1 );
	UPRINTF( "EGLWCORPBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPBD.bit.EGLWCORPBD_2 );
	UPRINTF( "EGLWCORPBD_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORPBD.bit.EGLWCORPBD_3 );
	UPRINTF( "EGLWCORMOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_0 );
	UPRINTF( "EGLWCORMOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_1 );
	UPRINTF( "EGLWCORMOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_2 );
	UPRINTF( "EGLWCORMOF_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_3 );
	UPRINTF( "EGLWCORMGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMGA.word );
	UPRINTF( "EGLWCORMGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_0 );
	UPRINTF( "EGLWCORMGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_1 );
	UPRINTF( "EGLWCORMGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_2 );
	UPRINTF( "EGLWCORMGA_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_3 );
	UPRINTF( "EGLWCORMBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMBD.bit.EGLWCORMBD_1 );
	UPRINTF( "EGLWCORMBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMBD.bit.EGLWCORMBD_2 );
	UPRINTF( "EGLWCORMBD_3 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCORMBD.bit.EGLWCORMBD_3 );
	UPRINTF( "EGLWSCLSUP   = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWSCLSUP.bit.EGLWSCLSUP );
	UPRINTF( "EGLWCLPPOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPOF.bit.EGLWCLPPOF_0 );
	UPRINTF( "EGLWCLPPOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPOF.bit.EGLWCLPPOF_1 );
	UPRINTF( "EGLWCLPPOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPOF.bit.EGLWCLPPOF_2 );
	UPRINTF( "EGLWCLPPGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPGA.word );
	UPRINTF( "EGLWCLPPGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPGA.bit.EGLWCLPPGA_0 );
	UPRINTF( "EGLWCLPPGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPGA.bit.EGLWCLPPGA_1 );
	UPRINTF( "EGLWCLPPGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPGA.bit.EGLWCLPPGA_2 );
	UPRINTF( "EGLWCLPPBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPBD.bit.EGLWCLPPBD_1 );
	UPRINTF( "EGLWCLPPBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPPBD.bit.EGLWCLPPBD_2 );
	UPRINTF( "EGLWCLPMOF_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMOF.bit.EGLWCLPMOF_0 );
	UPRINTF( "EGLWCLPMOF_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMOF.bit.EGLWCLPMOF_1 );
	UPRINTF( "EGLWCLPMOF_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMOF.bit.EGLWCLPMOF_2 );
	UPRINTF( "EGLWCLPMGA   = 0x%lx\n", IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMGA.word );
	UPRINTF( "EGLWCLPMGA_0 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMGA.bit.EGLWCLPMGA_0 );
	UPRINTF( "EGLWCLPMGA_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMGA.bit.EGLWCLPMGA_1 );
	UPRINTF( "EGLWCLPMGA_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMGA.bit.EGLWCLPMGA_2 );
	UPRINTF( "EGLWCLPMBD_1 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMBD.bit.EGLWCLPMBD_1 );
	UPRINTF( "EGLWCLPMBD_2 = 0x%x\n",  IO_R2Y_P1.F_R2Y.EGLW.EGLWCLPMBD.bit.EGLWCLPMBD_2 );
	UPRINTF( "PIPE2\n" );
	UPRINTF( "EGSMT	   = 0x%x\n", IO_R2Y_P2.F_R2Y.EGSM.EGSMCTL.bit.EGSMT );
	UPRINTF( "EGSMTTH   = 0x%x\n", IO_R2Y_P2.F_R2Y.EGSM.EGSMTT.bit.EGSMTTH );
	UPRINTF( "EGSMTTXGA = 0x%x\n", IO_R2Y_P2.F_R2Y.EGSM.EGSMTT.bit.EGSMTTXGA );
	UPRINTF( "EGHWEN 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWEN );
	UPRINTF( "EGHWMP 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWMP );
	UPRINTF( "EGHWTC 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWTC );
	UPRINTF( "EGHWSCLRES   = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWSCLRES );
	UPRINTF( "EGHWSCLTBL   = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWSCLTBL );
	UPRINTF( "EGHWTCRES	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWTCRES );
	UPRINTF( "EGHWTCTBL	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWTCTBL );
	UPRINTF( "EGHWNRLV	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGHW.EGHWCTL.bit.EGHWNRLV );
	UPRINTF( "EGHWHPFK[0]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.word[0] );
	UPRINTF( "EGHWHPFK[1]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.word[1] );
	UPRINTF( "EGHWHPFK0	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK0 );
	UPRINTF( "EGHWHPFK1	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK1 );
	UPRINTF( "EGHWHPFK2	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK2 );
	UPRINTF( "EGHWHPFK3	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK3 );
	UPRINTF( "EGHWHPFK4	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK4 );
	UPRINTF( "EGHWHPFK5	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWHPFK.bit.EGHWHPFK5 );
	UPRINTF( "EGHWCORPOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_0 );
	UPRINTF( "EGHWCORPOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_1 );
	UPRINTF( "EGHWCORPOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_2 );
	UPRINTF( "EGHWCORPOF_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPOF.bit.EGHWCORPOF_3 );
	UPRINTF( "EGHWCORPGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPGA.word );
	UPRINTF( "EGHWCORPGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_0 );
	UPRINTF( "EGHWCORPGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_1 );
	UPRINTF( "EGHWCORPGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_2 );
	UPRINTF( "EGHWCORPGA_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPGA.bit.EGHWCORPGA_3 );
	UPRINTF( "EGHWCORPBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPBD.bit.EGHWCORPBD_1 );
	UPRINTF( "EGHWCORPBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPBD.bit.EGHWCORPBD_2 );
	UPRINTF( "EGHWCORPBD_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORPBD.bit.EGHWCORPBD_3 );
	UPRINTF( "EGHWCORMOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_0 );
	UPRINTF( "EGHWCORMOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_1 );
	UPRINTF( "EGHWCORMOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_2 );
	UPRINTF( "EGHWCORMOF_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMOF.bit.EGHWCORMOF_3 );
	UPRINTF( "EGHWCORMGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMGA.word );
	UPRINTF( "EGHWCORMGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_0 );
	UPRINTF( "EGHWCORMGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_1 );
	UPRINTF( "EGHWCORMGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_2 );
	UPRINTF( "EGHWCORMGA_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMGA.bit.EGHWCORMGA_3 );
	UPRINTF( "EGHWCORMBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMBD.bit.EGHWCORMBD_1 );
	UPRINTF( "EGHWCORMBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMBD.bit.EGHWCORMBD_2 );
	UPRINTF( "EGHWCORMBD_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCORMBD.bit.EGHWCORMBD_3 );
	UPRINTF( "EGHWSCLSUP   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWSCLSUP.bit.EGHWSCLSUP );
	UPRINTF( "EGHWCLPPOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPOF.bit.EGHWCLPPOF_0 );
	UPRINTF( "EGHWCLPPOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPOF.bit.EGHWCLPPOF_1 );
	UPRINTF( "EGHWCLPPOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPOF.bit.EGHWCLPPOF_2 );
	UPRINTF( "EGHWCLPPGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPGA.word );
	UPRINTF( "EGHWCLPPGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPGA.bit.EGHWCLPPGA_0 );
	UPRINTF( "EGHWCLPPGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPGA.bit.EGHWCLPPGA_1 );
	UPRINTF( "EGHWCLPPGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPGA.bit.EGHWCLPPGA_2 );
	UPRINTF( "EGHWCLPPBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPBD.bit.EGHWCLPPBD_1 );
	UPRINTF( "EGHWCLPPBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPPBD.bit.EGHWCLPPBD_2 );
	UPRINTF( "EGHWCLPMOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMOF.bit.EGHWCLPMOF_0 );
	UPRINTF( "EGHWCLPMOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMOF.bit.EGHWCLPMOF_1 );
	UPRINTF( "EGHWCLPMOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMOF.bit.EGHWCLPMOF_2 );
	UPRINTF( "EGHWCLPMGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMGA.word );
	UPRINTF( "EGHWCLPMGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMGA.bit.EGHWCLPMGA_0 );
	UPRINTF( "EGHWCLPMGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMGA.bit.EGHWCLPMGA_1 );
	UPRINTF( "EGHWCLPMGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMGA.bit.EGHWCLPMGA_2 );
	UPRINTF( "EGHWCLPMBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMBD.bit.EGHWCLPMBD_1 );
	UPRINTF( "EGHWCLPMBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGHW.EGHWCLPMBD.bit.EGHWCLPMBD_2 );
	UPRINTF( "EGMWEN 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWEN );
	UPRINTF( "EGMWMP 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWMP );
	UPRINTF( "EGMWTC 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWTC );
	UPRINTF( "EGMWSCLRES   = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWSCLRES );
	UPRINTF( "EGMWSCLTBL   = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWSCLTBL );
	UPRINTF( "EGMWTCRES	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWTCRES );
	UPRINTF( "EGMWTCTBL	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWTCTBL );
	UPRINTF( "EGMWNRLV	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGMW.EGMWCTL.bit.EGMWNRLV );
	UPRINTF( "EGMWHPFK[0]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.word[0] );
	UPRINTF( "EGMWHPFK[1]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.word[1] );
	UPRINTF( "EGMWHPFK0	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK0 );
	UPRINTF( "EGMWHPFK1	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK1 );
	UPRINTF( "EGMWHPFK2	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK2 );
	UPRINTF( "EGMWHPFK3	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK3 );
	UPRINTF( "EGMWHPFK4	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK4 );
	UPRINTF( "EGMWHPFK5	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWHPFK.bit.EGMWHPFK5 );
	UPRINTF( "EGMWCORPOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_0 );
	UPRINTF( "EGMWCORPOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_1 );
	UPRINTF( "EGMWCORPOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_2 );
	UPRINTF( "EGMWCORPOF_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPOF.bit.EGMWCORPOF_3 );
	UPRINTF( "EGMWCORPGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPGA.word );
	UPRINTF( "EGMWCORPGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_0 );
	UPRINTF( "EGMWCORPGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_1 );
	UPRINTF( "EGMWCORPGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_2 );
	UPRINTF( "EGMWCORPGA_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPGA.bit.EGMWCORPGA_3 );
	UPRINTF( "EGMWCORPBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPBD.bit.EGMWCORPBD_1 );
	UPRINTF( "EGMWCORPBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPBD.bit.EGMWCORPBD_2 );
	UPRINTF( "EGMWCORPBD_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORPBD.bit.EGMWCORPBD_3 );
	UPRINTF( "EGMWCORMOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_0 );
	UPRINTF( "EGMWCORMOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_1 );
	UPRINTF( "EGMWCORMOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_2 );
	UPRINTF( "EGMWCORMOF_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMOF.bit.EGMWCORMOF_3 );
	UPRINTF( "EGMWCORMGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMGA.word );
	UPRINTF( "EGMWCORMGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_0 );
	UPRINTF( "EGMWCORMGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_1 );
	UPRINTF( "EGMWCORMGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_2 );
	UPRINTF( "EGMWCORMGA_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMGA.bit.EGMWCORMGA_3 );
	UPRINTF( "EGMWCORMBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMBD.bit.EGMWCORMBD_1 );
	UPRINTF( "EGMWCORMBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMBD.bit.EGMWCORMBD_2 );
	UPRINTF( "EGMWCORMBD_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCORMBD.bit.EGMWCORMBD_3 );
	UPRINTF( "EGMWSCLSUP   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWSCLSUP.bit.EGMWSCLSUP );
	UPRINTF( "EGMWCLPPOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPOF.bit.EGMWCLPPOF_0 );
	UPRINTF( "EGMWCLPPOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPOF.bit.EGMWCLPPOF_1 );
	UPRINTF( "EGMWCLPPOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPOF.bit.EGMWCLPPOF_2 );
	UPRINTF( "EGMWCLPPGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPGA.word );
	UPRINTF( "EGMWCLPPGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPGA.bit.EGMWCLPPGA_0 );
	UPRINTF( "EGMWCLPPGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPGA.bit.EGMWCLPPGA_1 );
	UPRINTF( "EGMWCLPPGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPGA.bit.EGMWCLPPGA_2 );
	UPRINTF( "EGMWCLPPBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPBD.bit.EGMWCLPPBD_1 );
	UPRINTF( "EGMWCLPPBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPPBD.bit.EGMWCLPPBD_2 );
	UPRINTF( "EGMWCLPMOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMOF.bit.EGMWCLPMOF_0 );
	UPRINTF( "EGMWCLPMOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMOF.bit.EGMWCLPMOF_1 );
	UPRINTF( "EGMWCLPMOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMOF.bit.EGMWCLPMOF_2 );
	UPRINTF( "EGMWCLPMGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMGA.word );
	UPRINTF( "EGMWCLPMGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMGA.bit.EGMWCLPMGA_0 );
	UPRINTF( "EGMWCLPMGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMGA.bit.EGMWCLPMGA_1 );
	UPRINTF( "EGMWCLPMGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMGA.bit.EGMWCLPMGA_2 );
	UPRINTF( "EGMWCLPMBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMBD.bit.EGMWCLPMBD_1 );
	UPRINTF( "EGMWCLPMBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGMW.EGMWCLPMBD.bit.EGMWCLPMBD_2 );
	UPRINTF( "EGLWEN 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWEN );
	UPRINTF( "EGLWMP 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWMP );
	UPRINTF( "EGLWTC 	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWTC );
	UPRINTF( "EGLWSCLRES   = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWSCLRES );
	UPRINTF( "EGLWSCLTBL   = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWSCLTBL );
	UPRINTF( "EGLWTCRES	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWTCRES );
	UPRINTF( "EGLWTCTBL	  = %u\n",	  IO_R2Y_P2.F_R2Y.EGLW.EGLWCTL.bit.EGLWTCTBL );
	UPRINTF( "EGLWHPFK[0]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.word[0] );
	UPRINTF( "EGLWHPFK[1]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.word[1] );
	UPRINTF( "EGLWHPFK[2]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.word[2] );
	UPRINTF( "EGLWHPFK0	  = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK0 );
	UPRINTF( "EGLWHPFK_1   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK1 );
	UPRINTF( "EGLWHPFK_2   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK2 );
	UPRINTF( "EGLWHPFK_3   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK3 );
	UPRINTF( "EGLWHPFK_4   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK4 );
	UPRINTF( "EGLWHPFK_5   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK5 );
	UPRINTF( "EGLWHPFK_6   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK6 );
	UPRINTF( "EGLWHPFK_7   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK7 );
	UPRINTF( "EGLWHPFK_8   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK8 );
	UPRINTF( "EGLWHPFK_9   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWHPFK.bit.EGLWHPFK9 );
	UPRINTF( "EGLWCORPOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_0 );
	UPRINTF( "EGLWCORPOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_1 );
	UPRINTF( "EGLWCORPOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_2 );
	UPRINTF( "EGLWCORPOF_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPOF.bit.EGLWCORPOF_3 );
	UPRINTF( "EGLWCORPGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPGA.word );
	UPRINTF( "EGLWCORPGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_0 );
	UPRINTF( "EGLWCORPGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_1 );
	UPRINTF( "EGLWCORPGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_2 );
	UPRINTF( "EGLWCORPGA_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPGA.bit.EGLWCORPGA_3 );
	UPRINTF( "EGLWCORPBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPBD.bit.EGLWCORPBD_1 );
	UPRINTF( "EGLWCORPBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPBD.bit.EGLWCORPBD_2 );
	UPRINTF( "EGLWCORPBD_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORPBD.bit.EGLWCORPBD_3 );
	UPRINTF( "EGLWCORMOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_0 );
	UPRINTF( "EGLWCORMOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_1 );
	UPRINTF( "EGLWCORMOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_2 );
	UPRINTF( "EGLWCORMOF_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMOF.bit.EGLWCORMOF_3 );
	UPRINTF( "EGLWCORMGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMGA.word );
	UPRINTF( "EGLWCORMGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_0 );
	UPRINTF( "EGLWCORMGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_1 );
	UPRINTF( "EGLWCORMGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_2 );
	UPRINTF( "EGLWCORMGA_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMGA.bit.EGLWCORMGA_3 );
	UPRINTF( "EGLWCORMBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMBD.bit.EGLWCORMBD_1 );
	UPRINTF( "EGLWCORMBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMBD.bit.EGLWCORMBD_2 );
	UPRINTF( "EGLWCORMBD_3 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCORMBD.bit.EGLWCORMBD_3 );
	UPRINTF( "EGLWSCLSUP   = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWSCLSUP.bit.EGLWSCLSUP );
	UPRINTF( "EGLWCLPPOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPOF.bit.EGLWCLPPOF_0 );
	UPRINTF( "EGLWCLPPOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPOF.bit.EGLWCLPPOF_1 );
	UPRINTF( "EGLWCLPPOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPOF.bit.EGLWCLPPOF_2 );
	UPRINTF( "EGLWCLPPGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPGA.word );
	UPRINTF( "EGLWCLPPGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPGA.bit.EGLWCLPPGA_0 );
	UPRINTF( "EGLWCLPPGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPGA.bit.EGLWCLPPGA_1 );
	UPRINTF( "EGLWCLPPGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPGA.bit.EGLWCLPPGA_2 );
	UPRINTF( "EGLWCLPPBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPBD.bit.EGLWCLPPBD_1 );
	UPRINTF( "EGLWCLPPBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPPBD.bit.EGLWCLPPBD_2 );
	UPRINTF( "EGLWCLPMOF_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMOF.bit.EGLWCLPMOF_0 );
	UPRINTF( "EGLWCLPMOF_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMOF.bit.EGLWCLPMOF_1 );
	UPRINTF( "EGLWCLPMOF_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMOF.bit.EGLWCLPMOF_2 );
	UPRINTF( "EGLWCLPMGA   = 0x%lx\n", IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMGA.word );
	UPRINTF( "EGLWCLPMGA_0 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMGA.bit.EGLWCLPMGA_0 );
	UPRINTF( "EGLWCLPMGA_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMGA.bit.EGLWCLPMGA_1 );
	UPRINTF( "EGLWCLPMGA_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMGA.bit.EGLWCLPMGA_2 );
	UPRINTF( "EGLWCLPMBD_1 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMBD.bit.EGLWCLPMBD_1 );
	UPRINTF( "EGLWCLPMBD_2 = 0x%x\n",  IO_R2Y_P2.F_R2Y.EGLW.EGLWCLPMBD.bit.EGLWCLPMBD_2 );

#ifdef D_IQ_DUMP_EE
	size = 0x20;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.EGSM;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_EE_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.EGSM;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_EE_1.BIN", address, size);

	size = 0x0300;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.EGHW;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_EE_2200_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.EGHW;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_EE_2200_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CLPF(void)
{
#ifdef D_IQ_DUMP_CLPF
	ULONG address, size;
#endif

	UPRINTF( "PIPE1\n" );
	UPRINTF( "CLPFEN	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFCTL.bit.CLPFEN );
	UPRINTF( "CLPFYA	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFCTL.bit.CLPFYA );
	UPRINTF( "CLPFK[0]	= 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFK.word[0] );
	UPRINTF( "CLPFK[1]	= 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFK.word[1] );
	UPRINTF( "CLPFK[2]	= 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFK.word[2] );
	UPRINTF( "CLPFK[3]	= 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFK.word[3] );
	UPRINTF( "CLPFK[4]	= 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFK.word[4] );
	UPRINTF( "CLPFK_0	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_0 );
	UPRINTF( "CLPFK_1	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_1 );
	UPRINTF( "CLPFK_2	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_2 );
	UPRINTF( "CLPFK_3	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_3 );
	UPRINTF( "CLPFK_4	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_4 );
	UPRINTF( "CLPFK_5	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_5 );
	UPRINTF( "CLPFK_6	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_6 );
	UPRINTF( "CLPFK_7	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_7 );
	UPRINTF( "CLPFK_8	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFK.bit.CLPFK_8 );
	UPRINTF( "CLPFOF_0	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_0 );
	UPRINTF( "CLPFOF_1	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_1 );
	UPRINTF( "CLPFOF_2	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_2 );
	UPRINTF( "CLPFOF_3	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_3 );
	UPRINTF( "CLPFGA[0] = 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFGA.word[0] );
	UPRINTF( "CLPFGA[1] = 0x%lx\n", IO_R2Y_P1.F_R2Y.CLPF.CLPFGA.word[1] );
	UPRINTF( "CLPFGA_0	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_0 );
	UPRINTF( "CLPFGA_1	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_1 );
	UPRINTF( "CLPFGA_2	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_2 );
	UPRINTF( "CLPFGA_3	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_3 );
	UPRINTF( "CLPFBD_1	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFBD.bit.CLPFBD_1 );
	UPRINTF( "CLPFBD_2	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFBD.bit.CLPFBD_2 );
	UPRINTF( "CLPFBD_3	= 0x%x\n",	IO_R2Y_P1.F_R2Y.CLPF.CLPFBD.bit.CLPFBD_3 );
	UPRINTF( "PIPE2\n" );
	UPRINTF( "CLPFEN	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFCTL.bit.CLPFEN );
	UPRINTF( "CLPFYA	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFCTL.bit.CLPFYA );
	UPRINTF( "CLPFK[0]	= 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFK.word[0] );
	UPRINTF( "CLPFK[1]	= 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFK.word[1] );
	UPRINTF( "CLPFK[2]	= 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFK.word[2] );
	UPRINTF( "CLPFK[3]	= 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFK.word[3] );
	UPRINTF( "CLPFK[4]	= 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFK.word[4] );
	UPRINTF( "CLPFK_0	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_0 );
	UPRINTF( "CLPFK_1	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_1 );
	UPRINTF( "CLPFK_2	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_2 );
	UPRINTF( "CLPFK_3	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_3 );
	UPRINTF( "CLPFK_4	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_4 );
	UPRINTF( "CLPFK_5	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_5 );
	UPRINTF( "CLPFK_6	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_6 );
	UPRINTF( "CLPFK_7	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_7 );
	UPRINTF( "CLPFK_8	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFK.bit.CLPFK_8 );
	UPRINTF( "CLPFOF_0	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_0 );
	UPRINTF( "CLPFOF_1	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_1 );
	UPRINTF( "CLPFOF_2	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_2 );
	UPRINTF( "CLPFOF_3	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFOF.bit.CLPFOF_3 );
	UPRINTF( "CLPFGA[0] = 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFGA.word[0] );
	UPRINTF( "CLPFGA[1] = 0x%lx\n", IO_R2Y_P2.F_R2Y.CLPF.CLPFGA.word[1] );
	UPRINTF( "CLPFGA_0	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_0 );
	UPRINTF( "CLPFGA_1	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_1 );
	UPRINTF( "CLPFGA_2	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_2 );
	UPRINTF( "CLPFGA_3	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFGA.bit.CLPFGA_3 );
	UPRINTF( "CLPFBD_1	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFBD.bit.CLPFBD_1 );
	UPRINTF( "CLPFBD_2	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFBD.bit.CLPFBD_2 );
	UPRINTF( "CLPFBD_3	= 0x%x\n",	IO_R2Y_P2.F_R2Y.CLPF.CLPFBD.bit.CLPFBD_3 );

#ifdef D_IQ_DUMP_CLPF
	size = 0x0100;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.CLPF;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CLPF_2500_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.CLPF;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CLPF_2500_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CSCO(void)
{
#ifdef D_IQ_DUMP_CSCO
	ULONG address, size;
#endif

	UPRINTF( "PIPE1\n" );
	UPRINTF( "CSYEN    = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYCTL.bit.CSYEN );
	UPRINTF( "CSYKY    = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYCTL.bit.CSYKY );
	UPRINTF( "CSYTBL   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYCTL.bit.CSYTBL );
	UPRINTF( "CSYOF_0  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYOF.bit.CSYOF_0 );
	UPRINTF( "CSYOF_1  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYOF.bit.CSYOF_1 );
	UPRINTF( "CSYOF_2  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYOF.bit.CSYOF_2 );
	UPRINTF( "CSYOF_3  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYOF.bit.CSYOF_3 );
	UPRINTF( "CSYGA_0  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYGA.bit.CSYGA_0 );
	UPRINTF( "CSYGA_1  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYGA.bit.CSYGA_1 );
	UPRINTF( "CSYGA_2  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYGA.bit.CSYGA_2 );
	UPRINTF( "CSYGA_3  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYGA.bit.CSYGA_3 );
	UPRINTF( "CSYBD_1  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYBD.bit.CSYBD_1 );
	UPRINTF( "CSYBD_2  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYBD.bit.CSYBD_2 );
	UPRINTF( "CSYBD_3  = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CSYBD.bit.CSYBD_3 );
	UPRINTF( "YRV	   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.YCRVFX.bit.YRV );
	UPRINTF( "CRV	   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.YCRVFX.bit.CRV );
	UPRINTF( "CFIXEN   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.YCRVFX.bit.CFIXEN );
	UPRINTF( "CFIX	   = 0x%lx\n", IO_R2Y_P1.F_R2Y.CSP.CFIX.word );
	UPRINTF( "CFIXB    = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CFIX.bit.CFIXB );
	UPRINTF( "CFIXR    = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.CFIX.bit.CFIXR );
	UPRINTF( "YOF	   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.YCOF.bit.YOF );
	UPRINTF( "COFB	   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.YCOF.bit.COFB );
	UPRINTF( "COFR	   = 0x%x\n",  IO_R2Y_P1.F_R2Y.CSP.YCOF.bit.COFR );
	UPRINTF( "PIPE2\n" );
	UPRINTF( "CSYEN    = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYCTL.bit.CSYEN );
	UPRINTF( "CSYKY    = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYCTL.bit.CSYKY );
	UPRINTF( "CSYTBL   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYCTL.bit.CSYTBL );
	UPRINTF( "CSYOF_0  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYOF.bit.CSYOF_0 );
	UPRINTF( "CSYOF_1  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYOF.bit.CSYOF_1 );
	UPRINTF( "CSYOF_2  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYOF.bit.CSYOF_2 );
	UPRINTF( "CSYOF_3  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYOF.bit.CSYOF_3 );
	UPRINTF( "CSYGA[0] = 0x%lx\n", IO_R2Y_P2.F_R2Y.CSP.CSYGA.word[0] );
	UPRINTF( "CSYGA[1] = 0x%lx\n", IO_R2Y_P2.F_R2Y.CSP.CSYGA.word[1] );
	UPRINTF( "CSYGA_0  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYGA.bit.CSYGA_0 );
	UPRINTF( "CSYGA_1  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYGA.bit.CSYGA_1 );
	UPRINTF( "CSYGA_2  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYGA.bit.CSYGA_2 );
	UPRINTF( "CSYGA_3  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYGA.bit.CSYGA_3 );
	UPRINTF( "CSYBD_1  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYBD.bit.CSYBD_1 );
	UPRINTF( "CSYBD_2  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYBD.bit.CSYBD_2 );
	UPRINTF( "CSYBD_3  = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CSYBD.bit.CSYBD_3 );
	UPRINTF( "YRV	   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.YCRVFX.bit.YRV );
	UPRINTF( "CRV	   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.YCRVFX.bit.CRV );
	UPRINTF( "CFIXEN   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.YCRVFX.bit.CFIXEN );
	UPRINTF( "CFIX	   = 0x%lx\n", IO_R2Y_P2.F_R2Y.CSP.CFIX.word );
	UPRINTF( "CFIXB    = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CFIX.bit.CFIXB );
	UPRINTF( "CFIXR    = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.CFIX.bit.CFIXR );
	UPRINTF( "YCOF[0]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.CSP.YCOF.word[0] );
	UPRINTF( "YCOF[1]  = 0x%lx\n", IO_R2Y_P2.F_R2Y.CSP.YCOF.word[1] );
	UPRINTF( "YOF	   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.YCOF.bit.YOF );
	UPRINTF( "COFB	   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.YCOF.bit.COFB );
	UPRINTF( "COFR	   = 0x%x\n",  IO_R2Y_P2.F_R2Y.CSP.YCOF.bit.COFR );

#ifdef D_IQ_DUMP_CSCO
	size = 0x0100;
	address = (ULONG)&IO_R2Y_P1.F_R2Y.CSP;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CSP_0.BIN", address, size);
	address = (ULONG)&IO_R2Y_P2.F_R2Y.CSP;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CSP_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CNR_OTF(void)
{
	#define	IO_CNR(ch)	((D_IM_CNR_CH_0 == ((ch))) ? &IO_CNR1 : (D_IM_CNR_CH_1 == ((ch))) ? &IO_CNR2 : &IO_CNR3)

	UCHAR ch;
#ifdef D_IQ_DUMP_CNR_OTF
	ULONG address_1, address_2, cnt;
	ULONG fid_1, fid_2;
	ULONG write_size = 0;
#endif

	for(ch = 0; ch < 2; ch++)
	{
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.SPRSR.word	   = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRSR.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.ACESEN.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.ACESEN.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.SPRTRG.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRTRG.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.SPRICE.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRICE.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.SPRICF.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRICF.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.HDIV.word	   = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.HDIV.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.SPRAXISET.word = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRAXISET.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.SPRMOD.word    = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.SPRMOD.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.HSIZE.word	   = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.HSIZE.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.VSIZE.word	   = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.VSIZE.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.WCTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.WCTA.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.WCDEF.word	   = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.WCDEF.word );
		UPRINTF("IO_CNR(%d)->OTF_REG_RW.CNRWOFSX.word  = 0x%lx\n", ch, IO_CNR(ch)->OTF_REG_RW.CNRWOFSX.word );

		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.CSPREN.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.CSPREN.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.CGDKEN.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.CGDKEN.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCEN.word 	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCEN.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCYTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCYTHH.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCYTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCYTHV.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCCTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCCTHH.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCCTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCCTHV.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCYDYM.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCYDYM.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCYDYMDT.word = 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCYDYMDT.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCYSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCYSCL.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCSSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCSSCL.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.MCCSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.MCCSCL.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCEN.word 	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCEN.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCYTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCYTHH.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCYTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCYTHV.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCCTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCCTHH.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCCTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCCTHV.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCYDYM.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCYDYM.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCYDYMDT.word = 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCYDYMDT.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCYSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCYSCL.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCSSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCSSCL.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.LCCSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.LCCSCL.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.WHTMD.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.WHTMD.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.BLEND.word	= 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.BLEND.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.SPRS.word	    = 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.SPRS.word );
		UPRINTF("IO_CNR(%d)->OTF_CSPR_REG.SPRD.word	    = 0x%lx\n", ch, IO_CNR(ch)->OTF_CSPR_REG.SPRD.word );
	}

#ifdef D_IQ_DUMP_CNR_OTF
	fid_1 = FJ_Fs_Open ( (CHAR *)"I:\\IQDUMP\\IQDUMP_CNR_OTF_0.BIN", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	fid_2 = FJ_Fs_Open ( (CHAR *)"I:\\IQDUMP\\IQDUMP_CNR_OTF_1.BIN", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	cnt = 0x2000 / sizeof(gIQDump_Data_Buf);
	address_1 = (ULONG)&IO_CNR1.OTF_REG_RW;
	address_2 = (ULONG)&IO_CNR2.OTF_REG_RW;
	for(; cnt > 0; cnt--)
	{
		memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)address_1, sizeof(gIQDump_Data_Buf));
		FJ_Fs_Write ( fid_1, (VOID*)gIQDump_Data_Buf, sizeof(gIQDump_Data_Buf), &write_size );
		memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)address_2, sizeof(gIQDump_Data_Buf));
		FJ_Fs_Write ( fid_2, (VOID*)gIQDump_Data_Buf, sizeof(gIQDump_Data_Buf), &write_size );
		address_1 = address_1 + sizeof(gIQDump_Data_Buf);
		address_2 = address_2 + sizeof(gIQDump_Data_Buf);
	}
	FJ_Fs_Close( fid_1 );
	FJ_Fs_Close( fid_2 );
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CNR_OFL(void)
{
	#define	IO_CNR(ch)	((D_IM_CNR_CH_0 == ((ch))) ? &IO_CNR1 : (D_IM_CNR_CH_1 == ((ch))) ? &IO_CNR2 : &IO_CNR3)

	UCHAR ch;
#ifdef D_IQ_DUMP_CNR_OFL
	ULONG address_1, address_2, cnt;
	ULONG fid_1, fid_2;
	ULONG write_size = 0;
#endif

	for(ch = 0; ch < 2; ch++)
	{
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.SPRSR.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRSR.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.ACESEN.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.ACESEN.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.SPRTRG.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRTRG.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.SPRICE.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRICE.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.SPRICF.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRICF.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.VDIV.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.VDIV.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.SPRAXISET.word = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRAXISET.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.SPRMOD.word    = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.SPRMOD.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.HSIZE.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.HSIZE.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.VSIZE.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.VSIZE.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.RYTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RYTA.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.RYDEF.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RYDEF.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.RCBTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RCBTA.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.RCRTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RCRTA.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.RCDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.RCDEF.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.WYTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WYTA.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.WYDEF.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WYDEF.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.WCBTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WCBTA.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.WCRTA.word	   = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WCRTA.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.WCDEF.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.WCDEF.word );
		UPRINTF("IO_CNR(%d)->OFL_REG_RW.TMPTA.word     = 0x%lx\n", ch, IO_CNR(ch)->OFL_REG_RW.TMPTA.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.CSPREN.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.CSPREN.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.CGDKEN.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.CGDKEN.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCEN.word 	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCEN.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCYTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCYTHH.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCYTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCYTHV.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCCTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCCTHH.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCCTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCCTHV.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCYDYM.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCYDYM.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCYDYMDT.word = 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCYDYMDT.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCYSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCYSCL.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCSSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCSSCL.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.MCCSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.MCCSCL.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCEN.word 	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCEN.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCYTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCYTHH.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCYTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCYTHV.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCCTHH.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCCTHH.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCCTHV.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCCTHV.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCYDYM.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCYDYM.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCYDYMDT.word = 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCYDYMDT.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCYSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCYSCL.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCSSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCSSCL.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.LCCSCL.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.LCCSCL.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.WHTMD.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.WHTMD.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.BLEND.word	= 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.BLEND.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.SPRS.word	    = 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.SPRS.word );
		UPRINTF("IO_CNR(%d)->OFL_CSPR_REG.SPRD.word	    = 0x%lx\n", ch, IO_CNR(ch)->OFL_CSPR_REG.SPRD.word );
	}

#ifdef D_IQ_DUMP_CNR_OFL
	fid_1 = FJ_Fs_Open ( (CHAR *)"I:\\IQDUMP\\IQDUMP_CNR_OFL_0.BIN", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	fid_2 = FJ_Fs_Open ( (CHAR *)"I:\\IQDUMP\\IQDUMP_CNR_OFL_1.BIN", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	cnt = 0x2000 / sizeof(gIQDump_Data_Buf);
	address_1 = (ULONG)&IO_CNR1.OFL_REG_RW;
	address_2 = (ULONG)&IO_CNR2.OFL_REG_RW;
	for(; cnt > 0; cnt--)
	{
		memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)address_1, sizeof(gIQDump_Data_Buf));
		FJ_Fs_Write ( fid_1, (VOID*)gIQDump_Data_Buf, sizeof(gIQDump_Data_Buf), &write_size );
		memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)address_2, sizeof(gIQDump_Data_Buf));
		FJ_Fs_Write ( fid_2, (VOID*)gIQDump_Data_Buf, sizeof(gIQDump_Data_Buf), &write_size );
		address_1 = address_1 + sizeof(gIQDump_Data_Buf);
		address_2 = address_2 + sizeof(gIQDump_Data_Buf);
	}
	FJ_Fs_Close( fid_1 );
	FJ_Fs_Close( fid_2 );
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_CAG(void)
{
	UCHAR unit_no;
#ifdef D_IQ_DUMP_CAG
	ULONG address, size;
#endif

	for(unit_no = 0; unit_no < 2; unit_no++)
	{
		UPRINTF( "CAGTRG[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGTRG.word );
		UPRINTF( "PAEN[%d]		 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGPAEN.word );
		UPRINTF( "CAGMD[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.CAGMD.word );
		UPRINTF( "RBR[%d]		 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.RBR.word );
		UPRINTF( "ABOARV[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOARV.word );
		UPRINTF( "ABOARH[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOARH.word );
		UPRINTF( "ABOAGV[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOAGV.word );
		UPRINTF( "ABOAGH[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOAGH.word );
		UPRINTF( "ABOABV[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOABV.word );
		UPRINTF( "ABOABH[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOABH.word );
		UPRINTF( "ABGAR[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABGAR.word );
		UPRINTF( "ABGAG[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABGAG.word );
		UPRINTF( "ABGAB[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABGAB.word );
		UPRINTF( "ABLRV[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLRV.word );
		UPRINTF( "ABLRH[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLRH.word );
		UPRINTF( "ABLGV[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLGV.word );
		UPRINTF( "ABLGH[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLGH.word );
		UPRINTF( "ABLBV[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLBV.word );
		UPRINTF( "ABLBH[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABLBH.word );
		UPRINTF( "ABOFSRV[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSRV.word );
		UPRINTF( "ABOFSRH[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSRH.word );
		UPRINTF( "ABOFSGV[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSGV.word );
		UPRINTF( "ABOFSGH[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSGH.word );
		UPRINTF( "ABOFSBV[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSBV.word );
		UPRINTF( "ABOFSBH[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABOFSBH.word );
		UPRINTF( "ABNLTHR1[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR1.word );
		UPRINTF( "ABNLTHG1[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG1.word );
		UPRINTF( "ABNLTHB1[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB1.word );
		UPRINTF( "ABNLGAR1[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR1.word );
		UPRINTF( "ABNLGAG1[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG1.word );
		UPRINTF( "ABNLGAB1[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB1.word );
		UPRINTF( "ABNLTHR2[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR2.word );
		UPRINTF( "ABNLTHG2[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG2.word );
		UPRINTF( "ABNLTHB2[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB2.word );
		UPRINTF( "ABNLGAR2[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR2.word );
		UPRINTF( "ABNLGAG2[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG2.word );
		UPRINTF( "ABNLGAB2[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB2.word );
		UPRINTF( "ABNLTHR3[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR3.word );
		UPRINTF( "ABNLTHG3[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG3.word );
		UPRINTF( "ABNLTHB3[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB3.word );
		UPRINTF( "ABNLGAR3[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR3.word );
		UPRINTF( "ABNLGAG3[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG3.word );
		UPRINTF( "ABNLGAB3[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB3.word );
		UPRINTF( "ABNLTHR4[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHR4.word );
		UPRINTF( "ABNLTHG4[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHG4.word );
		UPRINTF( "ABNLTHB4[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLTHB4.word );
		UPRINTF( "ABNLGAR4[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAR4.word );
		UPRINTF( "ABNLGAG4[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAG4.word );
		UPRINTF( "ABNLGAB4[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.ABNLGAB4.word );
		UPRINTF( "BFGLMT[%d]	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.BFGLMT.word );
		UPRINTF( "BFGTH[%d] 	 0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].SRO.CAG.BFGTH.word );
	}

#ifdef D_IQ_DUMP_CAG
	size = 0x0100;
	address = (ULONG)&IO_PRO.IMG_PIPE[0].SRO.CAG;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CAG_0.BIN", address, size);
	address = (ULONG)&IO_PRO.IMG_PIPE[1].SRO.CAG;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_CAG_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_FSHD(void)
{
	UCHAR unit_no;
#ifdef D_IQ_DUMP_FSHD
	ULONG address, size;
#endif

	for(unit_no = 0; unit_no < 2; unit_no++)
	{
        UPRINTF( "FSHDTRG[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDTRG.word );
        UPRINTF( "FSHDPAEN[%d]      0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDPAEN.word );
        UPRINTF( "FSHDCTL[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDCTL.word );
        UPRINTF( "FSHDV[%d]         0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDV.word );
        UPRINTF( "FSHDH[%d]         0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDH.word );
        UPRINTF( "FSHDVW[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDVW.word );
        UPRINTF( "FSHDHW[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHW.word );
        UPRINTF( "FSHDHWPH[%d]      0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHWPH.word );
        UPRINTF( "FSHDBC[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDBC.word );
        UPRINTF( "FSUL1[%d]         0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSUL1.word );
        UPRINTF( "FSUL2[%d]         0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSUL2.word );
        UPRINTF( "FSCTL[%d]         0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSCTL.word );
        UPRINTF( "FSSUBPR[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBPR.word );
        UPRINTF( "FSSUBSP[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBSP.word );
        UPRINTF( "FSSUBDR[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBDR.word );
        UPRINTF( "FSSUBST[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBST.word );
        UPRINTF( "FSA1[%d]          0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSA1.word );
        UPRINTF( "FSA2[%d]          0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSA2.word );
        UPRINTF( "CSCTL1[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL1.word );
        UPRINTF( "CSCTL2[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL2.word );
        UPRINTF( "CSTBLST[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLST.word );
        UPRINTF( "CSTBLS1[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLS1.word );
        UPRINTF( "CSTBLS2[%d]       0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLS2.word );
        UPRINTF( "CSHDD[%d]         0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSHDD.word );
        UPRINTF( "CSHDOA[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSHDOA.word );
        UPRINTF( "CSA1[%d]          0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSA1.word );
        UPRINTF( "CSA2[%d]          0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSA2.word );
        UPRINTF( "FSHDHCCTL[%d]     0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHCCTL.word );
        UPRINTF( "FSSLP1[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSLP1.word );
        UPRINTF( "FSSLP2[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSLP2.word );
        UPRINTF( "FSWRIC[%d]        0x%08lx\n", unit_no, IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSWRIC.word );
	}

#ifdef D_IQ_DUMP_FSHD
	size = 0x0100;
	address = (ULONG)&IO_PRO.IMG_PIPE[0].B2B.FSHD;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_FSHD_0.BIN", address, size);
	address = (ULONG)&IO_PRO.IMG_PIPE[1].B2B.FSHD;
	iqdbg_save_file((CHAR *)"I:\\IQDUMP\\IQDUMP_FSHD_1.BIN", address, size);
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_TBL(void)
{
#ifdef D_IQ_DUMP_TBL
	ULONG address_1, address_2, cnt;
	ULONG fid_1, fid_2;
	ULONG write_size = 0;
#endif

#ifdef D_IQ_DUMP_TBL
	fid_1 = FJ_Fs_Open ( (CHAR *)"I:\\IQDUMP\\IQDUMP_R2YTBL_0.BIN", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	fid_2 = FJ_Fs_Open ( (CHAR *)"I:\\IQDUMP\\IQDUMP_R2YTBL_1.BIN", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	cnt = sizeof(IO_R2Y_TBL_P1) / sizeof(gIQDump_Data_Buf);
	address_1 = (ULONG)&IO_R2Y_TBL_P1;
	address_2 = (ULONG)&IO_R2Y_TBL_P2;
	for(; cnt > 0; cnt--)
	{
		memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)address_1, sizeof(gIQDump_Data_Buf));
		FJ_Fs_Write ( fid_1, (VOID*)gIQDump_Data_Buf, sizeof(gIQDump_Data_Buf), &write_size );
		memcpy((UCHAR *)&gIQDump_Data_Buf, (UCHAR *)address_2, sizeof(gIQDump_Data_Buf));
		FJ_Fs_Write ( fid_2, (VOID*)gIQDump_Data_Buf, sizeof(gIQDump_Data_Buf), &write_size );
		address_1 = address_1 + sizeof(gIQDump_Data_Buf);
		address_2 = address_2 + sizeof(gIQDump_Data_Buf);
	}
	FJ_Fs_Close( fid_1 );
	FJ_Fs_Close( fid_2 );
#endif
	
	return 0;
}

INT32 IQDbg_Dump_IQ_SetFlag(E_IQ_DBG_DUMP flag, UCHAR dol)
{
	gIQDump_Flag = flag;
	gIQDump_Dol_Enable = dol;
	
	return 0;
}

INT32 IQDbg_Dump_IQ_MCC(void)
{
#ifdef D_IQ_DUMP_CIA
	ULONG address, size;
#endif

	UPRINTF( "[R2Y] MCC Macro:\n" );
	UPRINTF( "IO_R2Y_P1.F_R2Y.MCC.MCKD.MCK_0.bit.MCK_1_1 = 0x%x\n", IO_R2Y_P1.F_R2Y.MCC.MCKD.MCK_0.bit.MCK_1_1 );

	return 0;
}

INT32 IQDbg_Dump_IQ_PrintOut(UCHAR stream, UCHAR dol)
{

	if(gIQDump_Dol_Enable != dol) return 0;

	switch(gIQDump_Flag){
		case E_IQ_DBG_DUMP_DPC:
			IQDbg_Dump_IQ_DPC();
		break;
		case E_IQ_DBG_DUMP_ELF:
			IQDbg_Dump_IQ_ELF();
		break;
		case E_IQ_DBG_DUMP_CIA:
			IQDbg_Dump_IQ_CIA();
		break;
		case E_IQ_DBG_DUMP_HFA:
			IQDbg_Dump_IQ_HFA();
		break;
		case E_IQ_DBG_DUMP_TC:
			IQDbg_Dump_IQ_TC();
		break;
		case E_IQ_DBG_DUMP_CCM1:
			IQDbg_Dump_IQ_CCM1();
		break;
		case E_IQ_DBG_DUMP_GAM:
			IQDbg_Dump_IQ_GAM();
		break;
		case E_IQ_DBG_DUMP_EE:
			IQDbg_Dump_IQ_EE();
		break;
		case E_IQ_DBG_DUMP_CLPF:
			IQDbg_Dump_IQ_CLPF();
		break;
		case E_IQ_DBG_DUMP_CSCO:
			IQDbg_Dump_IQ_CSCO();
		break;
		case E_IQ_DBG_DUMP_CNR_OTF:
			IQDbg_Dump_IQ_CNR_OTF();
		break;
		case E_IQ_DBG_DUMP_CNR_OFL:
			IQDbg_Dump_IQ_CNR_OFL();
		break;
		case E_IQ_DBG_DUMP_CAG:
			IQDbg_Dump_IQ_CAG();
		break;
		case E_IQ_DBG_DUMP_FSHD:
			IQDbg_Dump_IQ_FSHD();
		break;
		case E_IQ_DBG_DUMP_TBL:
			IQDbg_Dump_IQ_TBL();
		break;
		case E_IQ_DBG_DUMP_MCC:
			IQDbg_Dump_IQ_MCC();
		break;
		case E_IQ_DBG_DUMP_ALL:
			IQDbg_Dump_IQ_DPC();
			IQDbg_Dump_IQ_ELF();
			IQDbg_Dump_IQ_CIA();
			IQDbg_Dump_IQ_HFA();
			IQDbg_Dump_IQ_TC();
			IQDbg_Dump_IQ_CCM1();
			IQDbg_Dump_IQ_GAM();
			IQDbg_Dump_IQ_EE();
			IQDbg_Dump_IQ_CLPF();
			IQDbg_Dump_IQ_CNR_OTF();
			IQDbg_Dump_IQ_CNR_OFL();
			IQDbg_Dump_IQ_CAG();
			IQDbg_Dump_IQ_FSHD();
			IQDbg_Dump_IQ_TBL();
		break;
		default:
		break;
	}

	gIQDump_Flag = E_IQ_DBG_DUMP_NONE;
	
	return 0;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
#ifdef D_IQ_DBG_SAVE_FILE
static void iqdbg_save_file( CHAR* fname, ULONG addr, ULONG size )
{
	ULONG fid;
	ULONG write_size = 0;

	fid = FJ_Fs_Open ( fname, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
	FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
	FJ_Fs_Close( fid );
}
#endif

