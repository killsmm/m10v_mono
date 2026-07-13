/**
 * @file		ct_dd_ipcu.c
 * @brief		dd_ipcu Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "dd_ipcu.h"

extern VOID Ct_Dd_IPCU_Main(int argc, char** argv);
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID ct_dd_ipcu_callback_ack1( UCHAR mb_num )
{
	Ddim_Print(("ct_dd_ipcu_callback_ack1(mb_num=%u)\n", mb_num ));
}

static VOID ct_dd_ipcu_callback_req1( UCHAR mb_num )
{
	Ddim_Print(("ct_dd_ipcu_callback_req1(mb_num=%u)\n", mb_num ));
}

VOID ct_dd_ipcu_pctest_wrap(CHAR* cmd)
{
	INT32	argc = 0;
	CHAR*	argv[32];
	CHAR	arg[32][32];
	INT32	pos = 0;
	INT32	len = 0;
	INT32	max = strlen(cmd);
	INT32	i;

	printf("CMD> %s\n", cmd);

	// Clear arg buffer
	for(i = 0; i < 32; i++){
		argv[i] = arg[i];
		memset(argv[i], 0, 32);
	}

	// Set arg buffer
	for(i = 0; i < max; i++){
		if(cmd[i] == ' '){
			if(len){
				memcpy(argv[argc], &cmd[pos], len);
				argc++;
				len = 0;
			}
			pos = i + 1;
		}
		else if(i == max - 1){
			memcpy(argv[argc], &cmd[pos], len + 1);
			argc++;
			len = 0;
			pos = i + 1;
		}
		else{
			len++;
		}
	}

	// Execute command
	Ct_Dd_IPCU_Main(argc, argv);
}

void ct_dd_test_all_2(void)
{
	INT32	seq_no = 0;

	/* [CT]01-02-001 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Src()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu get srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu get srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu get srcr 16 4");

	/* [CT]01-02-002 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Mode()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu get mr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get mr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get mr 16");

	/* [CT]01-02-003 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 16 2");

	/* [CT]01-02-004 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 16 3");

	/* [CT]01-02-005 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Send()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu get sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get sndr 16");

	/* [CT]01-02-006 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 16 2");

	/* [CT]01-02-007 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu get dstsr 16 3");

	/* [CT]01-02-008 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 16 2");

	/* [CT]01-02-009 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 16 3");

	/* [CT]01-02-010 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 16 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 16 2");

	/* [CT]01-02-011 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 16 3");

	/* [CT]01-02-012 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 16 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 16 2");

	/* [CT]01-02-013 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 16 3");

	/* [CT]01-02-014 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 16 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 16 2");

	/* [CT]01-02-015 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mcru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu get msr 16 3");

	/* [CT]01-02-016 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 0 0 0x12345678");
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 0 8 0x12345678");
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 8 0 0x12345678");
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 8 8 0x12345678");
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 16 0 0x12345678");
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 16 8 0x12345678");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 8 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 16 8");

	/* [CT]01-02-017 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Src()\n", seq_no));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 24 0x1");
	
	/* [CT]01-02-018 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Mode()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 24 0x1");
	
	/* [CT]01-02-019 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Send()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 24");
	
	/* [CT]01-02-020 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Send()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	
	/* [CT]01-02-021 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 24 0x100");

	/* [CT]01-02-022 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x4");

	/* [CT]01-02-023 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x8");

	/* [CT]01-02-024 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 16");

	/* [CT]01-02-025 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 16 3");

	/* [CT]01-02-026 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 16 3");

	/* [CT]01-02-027 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 24 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 5");

	/* [CT]01-02-028 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 0x4");

	/* [CT]01-02-029 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 0x4");

	/* [CT]01-02-030 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 0 16");

	/* [CT]01-02-031 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 8 6");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 16 2");

	/* [CT]01-02-032 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Dst_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 16 3");

	/* [CT]01-02-033 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 24 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 0 16");

	/* [CT]01-02-034 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 16 0x4");

	/* [CT]01-02-035 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 16 0x4");

	/* [CT]01-02-036 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 0 16");

	/* [CT]01-02-037 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 0 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set msru 16 3");

	/* [CT]01-02-038 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set msr 16 0x4");

	/* [CT]01-02-039 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 24 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 0 0x300");

	/* [CT]01-02-040 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 16 0x40");

	/* [CT]01-02-041 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcr 0 16");
	
	/* [CT]01-02-042 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set mcru 16 3");

	/* [CT]01-02-043 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Msk_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x6");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x6");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x6");

	/* [CT]01-02-044 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x8");

	/* [CT]01-02-045 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x8");

	/* [CT]01-02-046 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x8");

	/* [CT]01-02-047 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x8");

	/* [CT]01-02-048 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 16");

	/* [CT]01-02-049 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 16 3");

	/* [CT]01-02-050 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 16 3");

	/* [CT]01-02-051 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 16 3");

	/* [CT]01-02-052 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set asru 16 3");

	/* [CT]01-02-053 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 24");

	/* [CT]01-02-054 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 16 0x8");

	/* [CT]01-02-055 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set acr 16 0x8");

	/* [CT]01-02-056 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 5");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 5");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 5");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 0x8");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 0x200");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 0x80");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 0x8");

	/* [CT]01-02-057 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 0 16");

	/* [CT]01-02-058 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 16 3");

	/* [CT]01-02-059 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set acru 16 3");

	/* [CT]01-02-060 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Clr_MB_Ack_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 5");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 5");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 5");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsru 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set dcr 16 3");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 0 9");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 8 7");
	ct_dd_ipcu_pctest_wrap("ddipcu set asr 16 3");

	/* [CT]01-02-061 */
	Ddim_Print(("*** [CT] 01-02-%03d : Dd_IPCU_Set_MB_Data()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 24 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set dr 0 9 1");
}

void ct_dd_test_all_3(void)
{
	INT32	seq_no = 0;
	
	/* [CT]01-03-001,002 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_Int_Stat()\n", seq_no++));
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Ack(),Dd_IPCU_Get_MB_Req(),Dd_IPCU_Get_MB_Adr()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 0 0x100");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get isr 0 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbar 0 8");
	
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 8 0x40");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get isr 1 6 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbar 1 6");
	
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set dsr 16 0x4");
	ct_dd_ipcu_pctest_wrap("ddipcu set sndr 16");
	ct_dd_ipcu_pctest_wrap("ddipcu get isr 2 2 16");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbar 2 2");
	
	seq_no = 11;

	/* [CT]01-03-012 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_Int_Stat()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 8 2");
	ct_dd_ipcu_pctest_wrap("ddipcu set srcr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbsr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbsr 1 8");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbsr 2 16");

	/* [CT]01-03-013 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_Int_Stat(), Dd_IPCU_Get_Int_Stat_MB()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get isr 3 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu get isr 0 16 1");

	/* [CT]01-03-014 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Ack(), Dd_IPCU_Get_MB_Req(), Dd_IPCU_Get_MB_Adr()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get mbar 3 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbar 0 16 1");

	/* [CT]01-03-015 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Src(), Dd_IPCU_Get_MB_Src_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get srcr 0 24 0");

	/* [CT]01-03-016 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Mode()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get mr 24");

	/* [CT]01-03-017 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Send()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get sndr 24");

	/* [CT]01-03-018 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Dst_Stat(), Dd_IPCU_Get_MB_Dst_Stat_Unit()\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu dstsr 24 2");
	ct_dd_ipcu_pctest_wrap("ddipcu dstsr 1 16");

	/* [CT]01-03-019 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Msk_Stat(), Dd_IPCU_Get_MB_Msk_Stat_Unit())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu msr 24 2");
	ct_dd_ipcu_pctest_wrap("ddipcu msr 1 16");

	/* [CT]01-03-020 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Ack_Stat(), Dd_IPCU_Get_MB_Ack_Stat_Unit())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu msr 24 2");
	ct_dd_ipcu_pctest_wrap("ddipcu msr 1 16");

	/* [CT]01-03-021 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Ack_Stat(), Dd_IPCU_Get_MB_Ack_Stat_Unit())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 4");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 4");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 8 1");
	ct_dd_ipcu_pctest_wrap("ddipcu get astsr 16 1");

	/* [CT]01-03-022 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Ack_Src(), Dd_IPCU_Get_MB_Ack_Src_Unit())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu asrcr 24 2");
	ct_dd_ipcu_pctest_wrap("ddipcu asrcr 1 16");

	/* [CT]01-03-023 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Ack_Src(), Dd_IPCU_Get_MB_Ack_Src_Unit())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 0 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 8 0");
	ct_dd_ipcu_pctest_wrap("ddipcu set mr 16 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 0 1");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 8 1");
	ct_dd_ipcu_pctest_wrap("ddipcu get asrcr 16 1");

	/* [CT]01-03-024 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Data())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 24 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get dr 0 9");

	/* [CT]01-03-025 */
	Ddim_Print(("*** [CT] 01-03-%03d : Dd_IPCU_Get_MB_Stat(), Dd_IPCU_Get_MB_Stat_MB())\n", seq_no++));
	ct_dd_ipcu_pctest_wrap("ddipcu get mbsr 3 0");
	ct_dd_ipcu_pctest_wrap("ddipcu get mbsr 0 8");
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for IPCU test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *		The meaning of parameters
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		| P1    | P2    | P3    | P4    | P5    | P6    | P7    | P7    |  Meaning                                 |
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		| open  | mb_num|    	|       |       |       |       |       | Open                                     |
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		| close | id    |    	|       |       |       |       |       | Close                                    |
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		| ctrl  | id 	| src_ch|dstbit |mskbit |mode	|       |       | Control Setting						   |
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		| set   | srcr  | id    | src   |       |       |       |       | Source Register                          |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mr    | id    | md    |       |       |       |       | Mode Register                            |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | sndr  | id    |       |       |       |       |       | Send Register                            |
 *		|       +-------+-------+-------+-------+-----------------------+------------------------------------------+
 *		|       | dsr   | id    | val   |       |       |       |		| Destination Set Register                 |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dsru  | id    | uni   |       |       |       | 		| Destination Set Register (Unit)          |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dcr   | id    | val   |       |       |       | 		| Destination Clear Register               |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dcru  | id    | uni   |       |       |       |		| Destination Clear Register (Unit)        |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | msr   | id    | val   |       |       |       | 		| Mask Set Register                        |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | msru  | id    | uni   |       |       |       |		| Mask Set Register (Unit)                 |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mcr   | id	| val   |       |       |       | 		| Mask Clear Register                      |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mcru  | id    | uni   |       |       |       | 		| Mask Clear Register (Unit)               |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | asr   | id    | val   |       |       |       |		| Acknowledge Set Register                 |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | asru  | id    | uni   |       |       |       |		| Acknowledge Set Register (Unit)          |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | acr   | id    | val   |       |       |       |		| Acknowledge Clear Register               |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | acru  | id    | uni   |       |       |       |		| Acknowledge Clear Register (Unit)        |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dr    | id    | dat   | val   |       |       | 		| Data Register                            |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | ack_cb| id    | func  |       |       |       |       | Ack callback(set:func=1,clear:func=0)    |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | req_cb| unit  | ch    | func  |       |       |       | Req callback(set:func=1,clear:func=0)    |
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		| get   | isr   | unit  | ch    | num   |       |       |       | Interrupt Status Register                |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | isr   | unit  | all   |       |       |       |       | Interrupt Status Register (All)          |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mbar  | unit  | ch    |       |       |       |       | Mailbox Address Register                 |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mbar  | unit  | all   |       |       |       |       | Mailbox Address Register (All)           |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | srcr  | id    | uni   |       |       |       |		| Source Register                          |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | srcr  | id    | all   |       |       |       |       | Source Register (All)                    |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mr    | id    |       |       |       |       |       | Mode Register                            |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mr    | all   |       |       |       |       |       | Mode Register (All)                      |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | sndr  | id    |       |       |       |       | 		| Send Register                            |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | sndr  | all   |       |       |       |       |		| Send Register (All)                      |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dstsr | id    | uni   |       |       |       | 		| Destination Status Register              |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dstsr | all   |       |       |       |       |		| Destination Status Register (All)        |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | msr   | id    | uni   |       |       |       |		| Mask Status Register                     |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | msr   | all   |       |       |       |       |		| Mask Status Register (All)               |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | astar | id    | uni   |       |       |       | 		| Acknowledge Status Register              |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | astar | all   |       |       |       |       | 		| Acknowledge Status Register (All)        |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | asrcr | id 	| uni   |       |       |       | 		| Acknowledge Source Register              |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | asrcr | all   |       |       |       |       | 		| Acknowledge Source Register (All)        |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dr    | id    | dat   |       |       |       | 		| Data Register                            |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | dr    | all   |       |       |       |       | 		| Data Register (All)                      |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mbsr  | unit  | num   |       |       |       |       | MailBox Status Register                  |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | mbsr  | unit  | all   |       |       |       |       | MailBox Status Register (All)            |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | reqd  | id    | length|       |       |       |       | MailBox Data register                    |
 *		|       +-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 *		|       | ctrl  |       |       |       |       |       |       | Control Setting				           |
 *		+-------+-------+-------+-------+-------+-------+-------+-------+------------------------------------------+
 */                                      
VOID Ct_Dd_IPCU_Main(int argc, char** argv)
{
	UCHAR mb_num;
	UCHAR length;
	UCHAR id;
	INT32 ret;
	INT32 num;
	INT32 uni;
	ULONG ch;
	ULONG unit;
	ULONG dat;
	ULONG src;
	ULONG md;
	ULONG val;
	ULONG send;
	ULONG state;
	ULONG ack;
	ULONG req;
	ULONG mbar;
	ULONG dst;
	ULONG msk;
//	static ULONG data[9] = { 0x1111U, 0x2222U, 0x3333U, 0x4444U, 0x5555U, 0x6666U, 0x7777U, 0x8888U, 0x9999U };
	static ULONG data[9] = { 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U, 0x0000U };
	ULONG mb_stat;
	static T_DD_IPCU_MNG ipcu_mng;
	
	if(0 == strcmp(argv[1], "open")){
		Ddim_Print(("--- Open process ---\n"));
		mb_num = strtol(argv[2], NULL, 0);		/* pgr0539 */
		ret = Dd_IPCU_Open( mb_num, D_DDIM_WAIT_END_TIME, &id );		/* pgr0539 */
		Ddim_Print(("Dd_IPCU_Open:[MB No = %d][id = %d], ret = %d\n", mb_num, id, ret ));
	}else if(0 == strcmp(argv[1], "close")){
		Ddim_Print(("--- Close process ---\n"));
		id = strtol(argv[2], NULL, 0);
		ret = Dd_IPCU_Close( id );				/* pgr0539 */
		Ddim_Print(("Dd_IPCU_Close:[id = %d], ret = %d\n", id, ret ));
	}else if(0 == strcmp(argv[1], "ctrl")){
		Ddim_Print(("--- Control setting ---\n"));
		id = strtol(argv[2], NULL, 0);
		ipcu_mng.src_ch_num = strtol(argv[3], NULL, 10);
		ipcu_mng.dst_ch_bit = strtol(argv[4], NULL, 0);
		ipcu_mng.mask_ch_bit = strtol(argv[5], NULL, 0);
		ipcu_mng.mode = strtol(argv[6], NULL, 16);
		memcpy( &( ipcu_mng.send_data ), data, D_DD_IPCU_DATA_LENGTH );
		ipcu_mng.data_size = ( D_DD_IPCU_DATA_BLK_MAX * 4 );
		ret = Dd_IPCU_Ctrl( id, &ipcu_mng );		/* pgr0539 */
		Ddim_Print(("Dd_IPCU_Ctrl:[id = %d], ret = %d\n", id, ret ));
	}else if(0 == strcmp(argv[1], "start")){
		Ddim_Print(("--- Start process ---\n"));
		id = strtol(argv[2], NULL, 10);
		ret = Dd_IPCU_Start( id );		/* pgr0539 */
		Ddim_Print(("Dd_IPCU_Start:[id = %d], ret = %d\n", id, ret ));
	}else if(0 == strcmp(argv[1], "set")){
		if(0 == strcmp(argv[2], "ack_cb")){
			Ddim_Print(("--- Set Acknowledge callback function ---\n"));
			id = strtol(argv[3], NULL, 10);
			if( strtol(argv[4], NULL, 10)){
				Dd_IPCU_Set_Ack_CB( id, ct_dd_ipcu_callback_ack1 );		/* pgr0539 */
				Ddim_Print(("Dd_IPCU_Set_Ack_CB:[id = %d], func = 1, callback = 0x%x\n", id, (UINT32)&ct_dd_ipcu_callback_ack1 ));
			}
			else{
				Dd_IPCU_Set_Ack_CB( id, 0 );		/* pgr0539 */
				Ddim_Print(("Dd_IPCU_Set_Ack_CB:[id = %d], func = 0, callback = 0\n", id ));
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "req_cb")){
			Ddim_Print(("--- Set Request callback function ---\n"));
			unit = strtol(argv[3], NULL, 10);
			ch = strtol(argv[4], NULL, 10);	
			if( strtol(argv[5], NULL, 10)){
				Dd_IPCU_Set_Req_CB( unit, ch, ct_dd_ipcu_callback_req1 );	/* pgr0539 */
				Ddim_Print(("Dd_IPCU_Set_Req_CB:[unit = %ld][ch = %ld], func = 1, callback = 0x%x\n", unit, ch, (UINT32)&ct_dd_ipcu_callback_req1 ));
			}
			else{
				Dd_IPCU_Set_Req_CB( unit, ch, 0 );	/* pgr0539 */
				Ddim_Print(("Dd_IPCU_Set_Req_CB:[unit = %ld][ch = %ld], func = 0, callback = 0\n", unit, ch ));
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "srcr")){
			Ddim_Print(("--- Source Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			src = strtol(argv[4], NULL, 0);
			Dd_IPCU_Set_MB_Src( id, src );			/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "mr")){
			Ddim_Print(("--- Mode Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			md = strtol(argv[4], NULL, 0);
			ret = Dd_IPCU_Set_MB_Mode( id, md );	/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "sndr")){
			Ddim_Print(("--- Send Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			Dd_IPCU_Set_MB_Send( id );				/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dsr")){
			Ddim_Print(("--- Destination Set Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			dst = strtol(argv[4], NULL, 0);
			Dd_IPCU_Set_MB_Dst( id, dst );			/* pgr0539, pgr0039 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dsru")){
			Ddim_Print(("--- Destination Set Register (Unit) ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			Dd_IPCU_Set_MB_Dst_Unit( id, uni );		/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dcr")){
			Ddim_Print(("--- Destination Clear Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			val = strtol(argv[4], NULL, 0);
			Dd_IPCU_Clr_MB_Dst(id, val);			/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dcru")){
			Ddim_Print(("--- Destination Clear Register (Unit) ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			Dd_IPCU_Clr_MB_Dst_Unit( id, uni );		/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "msr")){
			Ddim_Print(("--- Mask Set Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			val = strtol(argv[4], NULL, 0);		/* pgr0539 */
			Dd_IPCU_Set_MB_Msk( id, val );			/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "msru")){
			Ddim_Print(("--- Mask Set Register (Unit) ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			Dd_IPCU_Set_MB_Msk_Unit( id, uni );		/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "mcr")){
			Ddim_Print(("--- Mask Clear Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			val = strtol(argv[4], NULL, 0);
			Dd_IPCU_Clr_MB_Msk( id, val );			/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "mcru")){
			Ddim_Print(("--- Mask Clear Register (Unit) ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			Dd_IPCU_Clr_MB_Msk_Unit( id, uni );		/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "asr")){
			Ddim_Print(("--- Acknowledge Set Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			val = strtol(argv[4], NULL, 0);
			Dd_IPCU_Set_MB_Ack( id, val );			/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "asru")){
			Ddim_Print(("--- Acknowledge Set Register (Unit) ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			Dd_IPCU_Set_MB_Ack_Unit( id, uni );		/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "acr")){
			Ddim_Print(("--- Acknowledge Clear Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			val = strtol(argv[4], NULL, 0);		/* pgr0539 */
			Dd_IPCU_Clr_MB_Ack( id, val );			/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "acru")){
			Ddim_Print(("--- Acknowledge Clear Register (Unit) ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			Dd_IPCU_Clr_MB_Ack_Unit( id, uni );		/* pgr0539 */
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dr")){
			Ddim_Print(("--- Data Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			dat = strtol(argv[4], NULL, 0);
			val = strtoul(argv[5], NULL, 0);
			Dd_IPCU_Set_MB_Data( id, dat, val );	/* pgr0539 */
			Ddim_Print(("---\n"));
		}else{
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}else if(0 == strcmp(argv[1], "get")){
		if(0 == strcmp(argv[2], "isr")){
			Ddim_Print(("--- Interrupt Status Register ---\n"));
			unit = strtol(argv[3], NULL, 0);
			ch = strtol(argv[4], NULL, 0);
			for(ch=0; ch<=D_DD_IPCU_INT_MAX; ch++){
				if((0 == strcmp(argv[4], "all"))||(ch == strtol(argv[4], NULL, 16))){
					ret = Dd_IPCU_Get_Int_Stat( unit, ch, &state );		/* pgr0539 */
					Ddim_Print(("ISR[unit = %ld][ch = %ld][state = %ld], ret = %d\n", unit, ch, state, ret));
					
					for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
						if((0 == strcmp(argv[4], "all"))||(num == strtol(argv[5], NULL, 16))){
							ret = Dd_IPCU_Get_Int_Stat_MB( unit, ch, num, &state );
							Ddim_Print(("ISR[unit = %ld][ch = %ld][mb = %d][state = %ld], ret = %d\n", unit, ch, num, state, ret ));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "mbar")){
			Ddim_Print(("--- Mailbox Address Register ---\n"));
			unit = strtol(argv[3], NULL, 0);
			ch = strtol(argv[4], NULL, 0);
			for(ch=0; ch<=D_DD_IPCU_INT_MAX; ch++){
				if((0 == strcmp(argv[4], "all"))||(ch == strtol(argv[4], NULL, 16))){
					ret = Dd_IPCU_Get_MB_Ack( unit, ch, &ack );			/* pgr0539 */
					Ddim_Print(("MBADR[unit = %ld][ch = %ld][ACK = %ld], ret = %d\n", unit, ch, ack, ret));
					ret = Dd_IPCU_Get_MB_Req( unit, ch, &req );
					Ddim_Print(("MBADR[unit = %ld][ch = %ld][REQ = %ld], ret = %d\n", unit, ch, req, ret));
					ret = Dd_IPCU_Get_MB_Adr( unit, ch, &mbar );
					Ddim_Print(("MBADR[unit = %ld][ch = %ld][MBA = 0x%x], ret = %d\n", unit, ch, (UINT32)mbar, ret));
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "srcr")){
			Ddim_Print(("--- Source Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == strtol(argv[4], NULL, 16))){
					ret = Dd_IPCU_Get_MB_Src( id, &src );				/* pgr0539 */
					Ddim_Print(("MAILBOX[id = %d]SRC = %ld], ret = %d\n", id, src, ret));
					for(uni=0; uni<=D_DD_IPCU_INT_MAX; uni++){
						if((0 == strcmp(argv[4], "all"))||((uni == strtol(argv[4], NULL, 16)))){
							ret = Dd_IPCU_Get_MB_Src_Unit( id, uni, &src );
							Ddim_Print(("MAILBOX[id = %d][ch = %d], ret = %d\n", id, uni, ret));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "mr")){
			Ddim_Print(("--- Mode Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){
					ret = Dd_IPCU_Get_MB_Mode( id, &md );				/* pgr0539 */
					Ddim_Print(("MAILBOX[id = %d][MODE = %ld], ret = %d\n", id, md, ret));
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "sndr")){
			Ddim_Print(("--- Send Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){	/* pgr0539 */
					ret = Dd_IPCU_Get_MB_Send( id, &send );				/* pgr0539 */
					Ddim_Print(("MAILBOX[id = %d][SEND = %ld], ret = %d\n", id, send, ret));
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dstsr")){
			Ddim_Print(("--- Destination Status Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){
					ret = Dd_IPCU_Get_MB_Dst_Stat( id, &dst );			/* pgr0539 */
					Ddim_Print(("MAILBOX[id = %d][DSTSTAT = 0x%x], ret = %d\n", id, (UINT32)dst, ret));
					for(uni=0; uni<=D_DD_IPCU_INT_MAX; uni++){
						if((0 == strcmp(argv[3], "all"))||(uni == strtol(argv[4], NULL, 16))){
							ret = Dd_IPCU_Get_MB_Dst_Stat_Unit( id, uni, &dst );
							Ddim_Print(("MAILBOX[id = %d][ch = %d][DSTSTAT = %ld], ret = %d\n", id, uni, dst, ret));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "msr")){
			Ddim_Print(("--- Mask Status Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){
					ret = Dd_IPCU_Get_MB_Msk_Stat( id, &msk );			/* pgr0539 */
					Ddim_Print(("MAILBOX[[id = %d][MASKSTAT = 0x%x], ret = %d\n", id, (UINT32)msk, ret));
					for(uni=0; uni<=D_DD_IPCU_INT_MAX; uni++){
						if((0 == strcmp(argv[3], "all"))||(uni == strtol(argv[4], NULL, 16))){
							ret = Dd_IPCU_Get_MB_Msk_Stat_Unit(num, uni, &msk);
							Ddim_Print(("MAILBOX[id = %d][ch = %d][MASKSTAT = %ld], ret = %d\n", id, uni, msk, ret));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "astar")){
			Ddim_Print(("--- Acknowledge Status Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){
					ret = Dd_IPCU_Get_MB_Ack_Stat( id, &ack );			/* pgr0539 */
					Ddim_Print(("MAILBOX[id = %d][ACKSTAT = 0x%x], ret = %d\n", id, (UINT32)ack, ret));
					for(uni=0; uni<=D_DD_IPCU_INT_MAX; uni++){
						if((0 == strcmp(argv[3], "all"))||(uni == strtol(argv[4], NULL, 16))){
							ret = Dd_IPCU_Get_MB_Ack_Stat_Unit(num, uni, &ack);
							Ddim_Print(("MAILBOX[id = %d][ch = %d][ACKSTAT = %ld], ret = %d\n", id, uni, ack, ret));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "asrcr")){
			Ddim_Print(("--- Acknowledge Source Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			uni = strtol(argv[4], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){
					ret = Dd_IPCU_Get_MB_Ack_Src( id, &ack );			/* pgr0539 */
					Ddim_Print(("MAILBOX[id = %d][ACKSRC = 0x%x], ret = %d\n", id, (UINT32)ack, ret));
					for(uni=0; uni<=D_DD_IPCU_INT_MAX; uni++){
						if((0 == strcmp(argv[3], "all"))||(uni == strtol(argv[4], NULL, 16))){
							ret = Dd_IPCU_Get_MB_Ack_Src_Unit( id, uni, &ack);
							Ddim_Print(("MAILBOX[id = %d][ch = %d][ACKSRC = %ld], ret = %d\n", id, uni, ack, ret));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "dr")){
			Ddim_Print(("--- Data Register ---\n"));
			id = strtol(argv[3], NULL, 0);
			dat = strtol(argv[4], NULL, 0);
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == id)){
					for(dat=0; dat<=D_DD_IPCU_MB_DATA_MAX; dat++){
						if((0 == strcmp(argv[3], "all"))||(dat == strtol(argv[4], NULL, 16))){
							ret = Dd_IPCU_Get_MB_Data( id, dat, &val );	/* pgr0539 */
							Ddim_Print(("MAILBOX[[id = %d][DATA[%ld] = 0x%x], ret = %d\n", id, dat, (UINT32)val, ret));
						}
					}
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "mbsr")){
			Ddim_Print(("--- MailBox Status Register ---\n"));
			unit = strtol(argv[3], NULL, 0);
			mb_num = strtol(argv[3], NULL, 0);
			ret = Dd_IPCU_Get_MB_Stat( unit, &mb_stat );		/* pgr0539 */
			Ddim_Print(("[unit = %ld][MBSTAT = %ld], ret = %d\n", unit, mb_stat, ret));
			for(num=0; num<=D_DD_IPCU_MB_MAX; num++){
				if((0 == strcmp(argv[3], "all"))||(num == mb_num)){
					ret = Dd_IPCU_Get_MB_Stat_MB( unit, num, &mb_stat );
					Ddim_Print(("[unit = %ld][MBSTAT[%d] = %ld], ret = %d\n", unit, num, mb_stat, ret));
				}
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "reqd")){
		Ddim_Print(("--- Get MailBox Data ---\n"));
			UCHAR dat_num = 0;
			id = strtol(argv[3], NULL, 0);
			length = strtol(argv[4], NULL, 0);
			ret = Dd_IPCU_Get_Req_Data( id, data, length );	/* pgr0539 */
			while( length > 0 ) {
				Ddim_Print(("MAILBOX[id = %d][dat_num = %d][data = %ld], ret = %d\n", id, dat_num, data[dat_num], ret));
				length = length - 4;
				dat_num++;
			}
			Ddim_Print(("---\n"));
		}else if(0 == strcmp(argv[2], "ctrl")){
			Ddim_Print(("--- Control setting ---\n"));
			id = strtol(argv[3], NULL, 0);
			ret = Dd_IPCU_Get_Ctrl( id, &ipcu_mng );		/* pgr0539 */
			Ddim_Print(("[id = %d][mode = %d] ret = %d\n", id, ipcu_mng.mode, ret ));
			Ddim_Print(("[src ch = %ld][dst ch bit = 0x%04lx]\n", ipcu_mng.src_ch_num, ipcu_mng.dst_ch_bit ));
			Ddim_Print(("[msk ch bit = 0x%04lx]\n", ipcu_mng.mask_ch_bit ));
			Ddim_Print(("---\n"));
		}else{
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}else if(0 == strcmp(argv[1], "test_2")){
		ct_dd_test_all_2();
	}else if(0 == strcmp(argv[1], "test_3")){
		ct_dd_test_all_3();
	}else{
		Ddim_Print(("please check 1st parameter!!\n"));
	}
}
