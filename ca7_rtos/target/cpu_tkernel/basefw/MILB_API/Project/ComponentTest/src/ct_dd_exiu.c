/**
 * @file		ct_dd_exiu.c
 * @brief		dd_exiu Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "dd_exiu.h"

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
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief  Command main function for EXIU test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 *		The meaning of parameters
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| P1          | P2            | P3  | P4  |Meaning                                              |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| display     | -             | -   | -   |Display all registers in external interrupt Unit.    |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eimask_rele   | num | -   |It set Interrupt Mask Release.                       |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eimask_mask   | num | -   |It set Interrupt Mask.                               |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eimask_val    | val | -   |It set Interrupt is Mask or Release.                 |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eisrcsel_ext  | num | -   |It set Interrupt External Source.                    |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eisrcsel_soft | num | -   |It set Interrupt Software Source.                    |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eireqclr_ch   | num | -   |It set Interrupt Cleared Channel.                    |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eireqclr_val  | val | -   |It set Interrupt Cleared Value.                      |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eilvl_lo      | num | -   |It set Interrupt Low level or Falling edge.          |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eilvl_hi      | num | -   |It set Interrupt High level or Rising edge.          |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eiedg_lvl     | num | -   |It set Interrupt Level Detection.                    |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eiedg_edg     | num | -   |It set Interrupt Edge Detection.                     |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| set         | eisir_gen     | num | -   |It set Software Interrupt Generate.                  |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 *		| err         | -             | -   | -   |Parameter Error Test.                                |
 *		+-------------+---------------+-----+-----+-----------------------------------------------------+
 */
VOID Ct_Dd_EXIU_Main(int argc, char** argv)
{
	INT32  ercd;
	UINT32 ch;
	UINT32 dat;
	
	if( 0 == strcmp( argv[1], "display" ) ) {
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EIMASK(ch, &dat);
			Ddim_Print(("Interrupt Mask(EIMASK) of Channel[%d] = %d\n", ch, dat));
		}
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EISRCSEL(ch, &dat);
			Ddim_Print(("Interrupt Source(EISRCSEL) of Channel[%d] = %d\n", ch, dat));
		}
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EIREQSTA(ch, &dat);
			Ddim_Print(("Internal Interrupt Request(EIREQSTA) of Channel[%d] = %d\n", ch, dat));
		}
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EIRAWREQSTA(ch, &dat);
			Ddim_Print(("Raw Interrupt Request(EIRAWREQSTA) of Channel[%d] = %d\n", ch, dat));
		}
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EILVL(ch, &dat);
			Ddim_Print(("External Interrupt Level(EILVL) of Channel[%d] = %d\n", ch, dat));
		}
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EIEDG(ch, &dat);
			Ddim_Print(("Interrupt Detection Method(EIEDG) of Channel[%d] = %d\n", ch, dat));
		}
		for( ch=D_DD_EXIU_CH_MIN; ch < D_DD_EXIU_CH_MAX+1; ch++ ) {
			Dd_EXIU_Get_EISIR(ch, &dat);
			Ddim_Print(("Software Interrupt(EISIR) of Channel[%d] = %d\n", ch, dat));
		}
	}
	else if( 0 == strcmp( argv[1], "set" ) ) {
		if( 0 == strcmp( argv[2], "eimask_rele" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EIMASK_Release(num);
			Ddim_Print(("It set Interrupt Mask Release.\n"));
		}
		else if( 0 == strcmp( argv[2], "eimask_mask" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EIMASK_Mask(num);
			Ddim_Print(("It set Interrupt Mask.\n"));
		}
		else if( 0 == strcmp( argv[2], "eimask_val" ) ) {
			ULONG val;
			val = (ULONG)atof(argv[3]);
			Dd_EXIU_Set_EIMASK_Val(val);
			Ddim_Print(("It set Interrupt is Mask or Release. \n"));
		}
		else if( 0 == strcmp( argv[2], "eisrcsel_ext" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EISRCSEL_Ext(num);
			Ddim_Print(("It set Interrupt External Source.\n"));
		}
		else if( 0 == strcmp( argv[2], "eisrcsel_soft" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EISRCSEL_Soft(num);
			Ddim_Print(("It set Interrupt Software Source.\n"));
		}
		else if( 0 == strcmp( argv[2], "eireqclr_ch" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EIREQCLR_Ch(num);
			Ddim_Print(("It set Interrupt Cleared Channel.\n"));
		}
		else if( 0 == strcmp( argv[2], "eireqclr_val" ) ) {
			ULONG val;
			val = (ULONG)atof(argv[3]);
			Dd_EXIU_Set_EIREQCLR_Val(val);
			Ddim_Print(("It set Interrupt Cleared Value.\n"));
		}
		else if( 0 == strcmp( argv[2], "eilvl_lo" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EILVL_Lo(num);
			Ddim_Print(("It set Interrupt Low level or Falling edge.\n"));
		}
		else if( 0 == strcmp( argv[2], "eilvl_hi" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EILVL_Hi(num);
			Ddim_Print(("It set Interrupt High level or Rising edge.\n"));
		}
		else if( 0 == strcmp( argv[2], "eiedg_lvl" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EIEDG_Level(num);
			Ddim_Print(("It set Interrupt Level Detection.\n"));
		}
		else if( 0 == strcmp( argv[2], "eiedg_edg" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EIEDG_Edge(num);
			Ddim_Print(("It set Interrupt Edge Detection.\n"));
		}
		else if( 0 == strcmp( argv[2], "eisir_gen" ) ) {
			int num;
			num = atoi(argv[3]);
			Dd_EXIU_Set_EISIR_Generate(num);
			Ddim_Print(("It set Software Interrupt Generate.\n"));
		}
		else{
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
	else if( 0 == strcmp( argv[1], "err" ) ) {
		ercd = Dd_EXIU_Get_EIMASK(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("Interrupt Mask(EIMASK) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIMASK(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("Interrupt Mask(EIMASK) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIMASK(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("Interrupt Mask(EIMASK) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Get_EISRCSEL(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("Interrupt Source(EISRCSEL) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EISRCSEL(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("Interrupt Source(EISRCSEL) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EISRCSEL(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("Interrupt Source(EISRCSEL) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Get_EIREQSTA(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("Internal Interrupt Request(EIREQSTA) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIREQSTA(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("Internal Interrupt Request(EIREQSTA) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIREQSTA(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("Internal Interrupt Request(EIREQSTA) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Get_EIRAWREQSTA(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("Raw Interrupt Request(EIRAWREQSTA) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIRAWREQSTA(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("Raw Interrupt Request(EIRAWREQSTA) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIRAWREQSTA(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("Raw Interrupt Request(EIRAWREQSTA) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Get_EILVL(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("External Interrupt Level(EILVL) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EILVL(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("External Interrupt Level(EILVL) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EILVL(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("External Interrupt Level(EILVL) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Get_EIEDG(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("Interrupt Detection Method(EIEDG) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIEDG(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("Interrupt Detection Method(EIEDG) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EIEDG(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("Interrupt Detection Method(EIEDG) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Get_EISIR(D_DD_EXIU_CH_MIN - 1, &dat);
		Ddim_Print(("Software Interrupt(EISIR) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EISIR(D_DD_EXIU_CH_MAX + 1, &dat);
		Ddim_Print(("Software Interrupt(EISIR) ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Get_EISIR(D_DD_EXIU_CH_MAX, NULL);
		Ddim_Print(("Software Interrupt(EISIR) ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIMASK_Release(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Mask Release ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIMASK_Release(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Mask Release ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIMASK_Mask(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Mask ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIMASK_Mask(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Mask ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIMASK_Val(D_DD_EXIU_VAL_MIN - 1);
		Ddim_Print(("Set Interrupt is Mask or Release ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIMASK_Val(D_DD_EXIU_VAL_MAX + 1);
		Ddim_Print(("Set Interrupt is Mask or Release ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EISRCSEL_Ext(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt External Source ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EISRCSEL_Ext(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt External Source ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EISRCSEL_Soft(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Software Source ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EISRCSEL_Soft(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Software Source ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIREQCLR_Ch(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Cleared Channel ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIREQCLR_Ch(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Cleared Channel ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIREQCLR_Val(D_DD_EXIU_VAL_MIN - 1);
		Ddim_Print(("Set Interrupt is Cleared Value ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIREQCLR_Val(D_DD_EXIU_VAL_MAX + 1);
		Ddim_Print(("Set Interrupt is Cleared Value ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EILVL_Lo(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Low level or Falling edge ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EILVL_Lo(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Low level or Falling edge ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EILVL_Hi(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt High level or Rising edge ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EILVL_Hi(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt High level or Rising edge ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIEDG_Level(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Level Detection ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIEDG_Level(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Level Detection edge ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EIEDG_Edge(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Level Edge Detection ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EIEDG_Edge(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Level Edge Detection ercd = 0x%08x\n", ercd));
		
		ercd = Dd_EXIU_Set_EISIR_Generate(D_DD_EXIU_CH_MIN - 1);
		Ddim_Print(("Set Interrupt Software Interrupt Generate ercd = 0x%08x\n", ercd));
		ercd = Dd_EXIU_Set_EISIR_Generate(D_DD_EXIU_CH_MAX + 1);
		Ddim_Print(("Set Interrupt Software Interrupt Generate ercd = 0x%08x\n", ercd));
	}
	else{
		Ddim_Print(("please check 1st parameter!!\n"));
	}
}
