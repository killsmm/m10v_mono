/****************************************************************************/
/*  <Contents>   Definition RS M9MO ROM MAP                                 */
/*  <Filename>   b2y2_map.h                                                 */
/*  <Enviroment> RVDS for ARM946E-S                                         */
/*  <Macro Ver.> 1.47                                                       */
/*  <Creation Date/author>  2017/03/17 17:08:21                             */
/*  <Latest modify Date/author>                                             */
/*                                                                          */
/*                                  COPYRIGHT(C) FUJITSU LIMITED 2011       */
/*                                                  FUJITSU Confidential    */
/****************************************************************************/

#ifndef  B2Y2_MAP_H_INC
#define  B2Y2_MAP_H_INC

extern const struct _B2Y2_map B2Y2_MEM;

/*=========================================================================*/
/*  Define of struct                                                       */
/*                                                                         */
/*=========================================================================*/


/*==========================================================================*/
/*  Tone switch condition													*/
/*  																		*/
/*==========================================================================*/
struct _tone_sw_con {
	unsigned short 				TONE_BV_THREH1_H;				/*TONE_BV_THREH1_H						0x00007E00	(2byte)			 */
	unsigned short 				TONE_BV_THREH1_L;				/*TONE_BV_THREH1_L						0x00007E02	(2byte)			 */
	unsigned short 				TONE_BV_THREL1_H;				/*TONE_BV_THREL1_H						0x00007E04	(2byte)			 */
	unsigned short 				TONE_BV_THREL1_L;				/*TONE_BV_THREL1_L						0x00007E06	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE1_H;				/*TONE_CTEMP_THRE1_H					0x00007E08	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE1_L;				/*TONE_CTEMP_THRE1_L					0x00007E0A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X1_H;				/*TONE_CTEMP_X1_H						0x00007E0C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X1_L;				/*TONE_CTEMP_X1_L						0x00007E0E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y1_H;				/*TONE_CTEMP_Y1_H						0x00007E10	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y1_L;				/*TONE_CTEMP_Y1_L						0x00007E12	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_1;				/*TONE_CTEMP_THREH_1					0x00007E14	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_1;				/*TONE_CTEMP_THREL_1					0x00007E16	(2byte)			 */
	unsigned short 				TONE_BV_THREH2_H;				/*TONE_BV_THREH2_H						0x00007E18	(2byte)			 */
	unsigned short 				TONE_BV_THREH2_L;				/*TONE_BV_THREH2_L						0x00007E1A	(2byte)			 */
	unsigned short 				TONE_BV_THREL2_H;				/*TONE_BV_THREL2_H						0x00007E1C	(2byte)			 */
	unsigned short 				TONE_BV_THREL2_L;				/*TONE_BV_THREL2_L						0x00007E1E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE2_H;				/*TONE_CTEMP_THRE2_H					0x00007E20	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE2_L;				/*TONE_CTEMP_THRE2_L					0x00007E22	(2byte)			 */
	unsigned short 				TONE_CTEMP_X2_H;				/*TONE_CTEMP_X2_H						0x00007E24	(2byte)			 */
	unsigned short 				TONE_CTEMP_X2_L;				/*TONE_CTEMP_X2_L						0x00007E26	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y2_H;				/*TONE_CTEMP_Y2_H						0x00007E28	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y2_L;				/*TONE_CTEMP_Y2_L						0x00007E2A	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_2;				/*TONE_CTEMP_THREH_2					0x00007E2C	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_2;				/*TONE_CTEMP_THREL_2					0x00007E2E	(2byte)			 */
	unsigned short 				TONE_BV_THREH3_H;				/*TONE_BV_THREH3_H						0x00007E30	(2byte)			 */
	unsigned short 				TONE_BV_THREH3_L;				/*TONE_BV_THREH3_L						0x00007E32	(2byte)			 */
	unsigned short 				TONE_BV_THREL3_H;				/*TONE_BV_THREL3_H						0x00007E34	(2byte)			 */
	unsigned short 				TONE_BV_THREL3_L;				/*TONE_BV_THREL3_L						0x00007E36	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE3_H;				/*TONE_CTEMP_THRE3_H					0x00007E38	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE3_L;				/*TONE_CTEMP_THRE3_L					0x00007E3A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X3_H;				/*TONE_CTEMP_X3_H						0x00007E3C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X3_L;				/*TONE_CTEMP_X3_L						0x00007E3E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y3_H;				/*TONE_CTEMP_Y3_H						0x00007E40	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y3_L;				/*TONE_CTEMP_Y3_L						0x00007E42	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_3;				/*TONE_CTEMP_THREH_3					0x00007E44	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_3;				/*TONE_CTEMP_THREL_3					0x00007E46	(2byte)			 */
	unsigned short 				TONE_BV_THREH4_H;				/*TONE_BV_THREH4_H						0x00007E48	(2byte)			 */
	unsigned short 				TONE_BV_THREH4_L;				/*TONE_BV_THREH4_L						0x00007E4A	(2byte)			 */
	unsigned short 				TONE_BV_THREL4_H;				/*TONE_BV_THREL4_H						0x00007E4C	(2byte)			 */
	unsigned short 				TONE_BV_THREL4_L;				/*TONE_BV_THREL4_L						0x00007E4E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE4_H;				/*TONE_CTEMP_THRE4_H					0x00007E50	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE4_L;				/*TONE_CTEMP_THRE4_L					0x00007E52	(2byte)			 */
	unsigned short 				TONE_CTEMP_X4_H;				/*TONE_CTEMP_X4_H						0x00007E54	(2byte)			 */
	unsigned short 				TONE_CTEMP_X4_L;				/*TONE_CTEMP_X4_L						0x00007E56	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y4_H;				/*TONE_CTEMP_Y4_H						0x00007E58	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y4_L;				/*TONE_CTEMP_Y4_L						0x00007E5A	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_4;				/*TONE_CTEMP_THREH_4					0x00007E5C	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_4;				/*TONE_CTEMP_THREL_4					0x00007E5E	(2byte)			 */
	unsigned short 				TONE_BV_THREH5_H;				/*TONE_BV_THREH5_H						0x00007E60	(2byte)			 */
	unsigned short 				TONE_BV_THREH5_L;				/*TONE_BV_THREH5_L						0x00007E62	(2byte)			 */
	unsigned short 				TONE_BV_THREL5_H;				/*TONE_BV_THREL5_H						0x00007E64	(2byte)			 */
	unsigned short 				TONE_BV_THREL5_L;				/*TONE_BV_THREL5_L						0x00007E66	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE5_H;				/*TONE_CTEMP_THRE5_H					0x00007E68	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE5_L;				/*TONE_CTEMP_THRE5_L					0x00007E6A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X5_H;				/*TONE_CTEMP_X5_H						0x00007E6C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X5_L;				/*TONE_CTEMP_X5_L						0x00007E6E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y5_H;				/*TONE_CTEMP_Y5_H						0x00007E70	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y5_L;				/*TONE_CTEMP_Y5_L						0x00007E72	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_5;				/*TONE_CTEMP_THREH_5					0x00007E74	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_5;				/*TONE_CTEMP_THREL_5					0x00007E76	(2byte)			 */
	unsigned short 				TONE_BV_THREH6_H;				/*TONE_BV_THREH6_H						0x00007E78	(2byte)			 */
	unsigned short 				TONE_BV_THREH6_L;				/*TONE_BV_THREH6_L						0x00007E7A	(2byte)			 */
	unsigned short 				TONE_BV_THREL6_H;				/*TONE_BV_THREL6_H						0x00007E7C	(2byte)			 */
	unsigned short 				TONE_BV_THREL6_L;				/*TONE_BV_THREL6_L						0x00007E7E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE6_H;				/*TONE_CTEMP_THRE6_H					0x00007E80	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE6_L;				/*TONE_CTEMP_THRE6_L					0x00007E82	(2byte)			 */
	unsigned short 				TONE_CTEMP_X6_H;				/*TONE_CTEMP_X6_H						0x00007E84	(2byte)			 */
	unsigned short 				TONE_CTEMP_X6_L;				/*TONE_CTEMP_X6_L						0x00007E86	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y6_H;				/*TONE_CTEMP_Y6_H						0x00007E88	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y6_L;				/*TONE_CTEMP_Y6_L						0x00007E8A	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_6;				/*TONE_CTEMP_THREH_6					0x00007E8C	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_6;				/*TONE_CTEMP_THREL_6					0x00007E8E	(2byte)			 */
	unsigned short 				TONE_BV_THREH7_H;				/*TONE_BV_THREH7_H						0x00007E90	(2byte)			 */
	unsigned short 				TONE_BV_THREH7_L;				/*TONE_BV_THREH7_L						0x00007E92	(2byte)			 */
	unsigned short 				TONE_BV_THREL7_H;				/*TONE_BV_THREL7_H						0x00007E94	(2byte)			 */
	unsigned short 				TONE_BV_THREL7_L;				/*TONE_BV_THREL7_L						0x00007E96	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE7_H;				/*TONE_CTEMP_THRE7_H					0x00007E98	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE7_L;				/*TONE_CTEMP_THRE7_L					0x00007E9A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X7_H;				/*TONE_CTEMP_X7_H						0x00007E9C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X7_L;				/*TONE_CTEMP_X7_L						0x00007E9E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y7_H;				/*TONE_CTEMP_Y7_H						0x00007EA0	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y7_L;				/*TONE_CTEMP_Y7_L						0x00007EA2	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_7;				/*TONE_CTEMP_THREH_7					0x00007EA4	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_7;				/*TONE_CTEMP_THREL_7					0x00007EA6	(2byte)			 */
	unsigned short 				TONE_BV_THREH8_H;				/*TONE_BV_THREH8_H						0x00007EA8	(2byte)			 */
	unsigned short 				TONE_BV_THREH8_L;				/*TONE_BV_THREH8_L						0x00007EAA	(2byte)			 */
	unsigned short 				TONE_BV_THREL8_H;				/*TONE_BV_THREL8_H						0x00007EAC	(2byte)			 */
	unsigned short 				TONE_BV_THREL8_L;				/*TONE_BV_THREL8_L						0x00007EAE	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE8_H;				/*TONE_CTEMP_THRE8_H					0x00007EB0	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE8_L;				/*TONE_CTEMP_THRE8_L					0x00007EB2	(2byte)			 */
	unsigned short 				TONE_CTEMP_X8_H;				/*TONE_CTEMP_X8_H						0x00007EB4	(2byte)			 */
	unsigned short 				TONE_CTEMP_X8_L;				/*TONE_CTEMP_X8_L						0x00007EB6	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y8_H;				/*TONE_CTEMP_Y8_H						0x00007EB8	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y8_L;				/*TONE_CTEMP_Y8_L						0x00007EBA	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_8;				/*TONE_CTEMP_THREH_8					0x00007EBC	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_8;				/*TONE_CTEMP_THREL_8					0x00007EBE	(2byte)			 */
	unsigned short 				TONE_BV_THREH9_H;				/*TONE_BV_THREH9_H						0x00007EC0	(2byte)			 */
	unsigned short 				TONE_BV_THREH9_L;				/*TONE_BV_THREH9_L						0x00007EC2	(2byte)			 */
	unsigned short 				TONE_BV_THREL9_H;				/*TONE_BV_THREL9_H						0x00007EC4	(2byte)			 */
	unsigned short 				TONE_BV_THREL9_L;				/*TONE_BV_THREL9_L						0x00007EC6	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE9_H;				/*TONE_CTEMP_THRE9_H					0x00007EC8	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE9_L;				/*TONE_CTEMP_THRE9_L					0x00007ECA	(2byte)			 */
	unsigned short 				TONE_CTEMP_X9_H;				/*TONE_CTEMP_X9_H						0x00007ECC	(2byte)			 */
	unsigned short 				TONE_CTEMP_X9_L;				/*TONE_CTEMP_X9_L						0x00007ECE	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y9_H;				/*TONE_CTEMP_Y9_H						0x00007ED0	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y9_L;				/*TONE_CTEMP_Y9_L						0x00007ED2	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_9;				/*TONE_CTEMP_THREH_9					0x00007ED4	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_9;				/*TONE_CTEMP_THREL_9					0x00007ED6	(2byte)			 */
	unsigned short 				TONE_BV_THREH10_H;				/*TONE_BV_THREH10_H						0x00007ED8	(2byte)			 */
	unsigned short 				TONE_BV_THREH10_L;				/*TONE_BV_THREH10_L						0x00007EDA	(2byte)			 */
	unsigned short 				TONE_BV_THREL10_H;				/*TONE_BV_THREL10_H						0x00007EDC	(2byte)			 */
	unsigned short 				TONE_BV_THREL10_L;				/*TONE_BV_THREL10_L						0x00007EDE	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE10_H;			/*TONE_CTEMP_THRE10_H					0x00007EE0	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE10_L;			/*TONE_CTEMP_THRE10_L					0x00007EE2	(2byte)			 */
	unsigned short 				TONE_CTEMP_X10_H;				/*TONE_CTEMP_X10_H						0x00007EE4	(2byte)			 */
	unsigned short 				TONE_CTEMP_X10_L;				/*TONE_CTEMP_X10_L						0x00007EE6	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y10_H;				/*TONE_CTEMP_Y10_H						0x00007EE8	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y10_L;				/*TONE_CTEMP_Y10_L						0x00007EEA	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_10;			/*TONE_CTEMP_THREH_10					0x00007EEC	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_10;			/*TONE_CTEMP_THREL_10					0x00007EEE	(2byte)			 */
	unsigned short 				TONE_BV_THREH11_H;				/*TONE_BV_THREH11_H						0x00007EF0	(2byte)			 */
	unsigned short 				TONE_BV_THREH11_L;				/*TONE_BV_THREH11_L						0x00007EF2	(2byte)			 */
	unsigned short 				TONE_BV_THREL11_H;				/*TONE_BV_THREL11_H						0x00007EF4	(2byte)			 */
	unsigned short 				TONE_BV_THREL11_L;				/*TONE_BV_THREL11_L						0x00007EF6	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE11_H;			/*TONE_CTEMP_THRE11_H					0x00007EF8	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE11_L;			/*TONE_CTEMP_THRE11_L					0x00007EFA	(2byte)			 */
	unsigned short 				TONE_CTEMP_X11_H;				/*TONE_CTEMP_X11_H						0x00007EFC	(2byte)			 */
	unsigned short 				TONE_CTEMP_X11_L;				/*TONE_CTEMP_X11_L						0x00007EFE	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y11_H;				/*TONE_CTEMP_Y11_H						0x00007F00	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y11_L;				/*TONE_CTEMP_Y11_L						0x00007F02	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_11;			/*TONE_CTEMP_THREH_11					0x00007F04	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_11;			/*TONE_CTEMP_THREL_11					0x00007F06	(2byte)			 */
	unsigned short 				TONE_BV_THREH12_H;				/*TONE_BV_THREH12_H						0x00007F08	(2byte)			 */
	unsigned short 				TONE_BV_THREH12_L;				/*TONE_BV_THREH12_L						0x00007F0A	(2byte)			 */
	unsigned short 				TONE_BV_THREL12_H;				/*TONE_BV_THREL12_H						0x00007F0C	(2byte)			 */
	unsigned short 				TONE_BV_THREL12_L;				/*TONE_BV_THREL12_L						0x00007F0E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE12_H;			/*TONE_CTEMP_THRE12_H					0x00007F10	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE12_L;			/*TONE_CTEMP_THRE12_L					0x00007F12	(2byte)			 */
	unsigned short 				TONE_CTEMP_X12_H;				/*TONE_CTEMP_X12_H						0x00007F14	(2byte)			 */
	unsigned short 				TONE_CTEMP_X12_L;				/*TONE_CTEMP_X12_L						0x00007F16	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y12_H;				/*TONE_CTEMP_Y12_H						0x00007F18	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y12_L;				/*TONE_CTEMP_Y12_L						0x00007F1A	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_12;			/*TONE_CTEMP_THREH_12					0x00007F1C	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_12;			/*TONE_CTEMP_THREL_12					0x00007F1E	(2byte)			 */
	unsigned short 				TONE_BV_THREH13_H;				/*TONE_BV_THREH13_H						0x00007F20	(2byte)			 */
	unsigned short 				TONE_BV_THREH13_L;				/*TONE_BV_THREH13_L						0x00007F22	(2byte)			 */
	unsigned short 				TONE_BV_THREL13_H;				/*TONE_BV_THREL13_H						0x00007F24	(2byte)			 */
	unsigned short 				TONE_BV_THREL13_L;				/*TONE_BV_THREL13_L						0x00007F26	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE13_H;			/*TONE_CTEMP_THRE13_H					0x00007F28	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE13_L;			/*TONE_CTEMP_THRE13_L					0x00007F2A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X13_H;				/*TONE_CTEMP_X13_H						0x00007F2C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X13_L;				/*TONE_CTEMP_X13_L						0x00007F2E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y13_H;				/*TONE_CTEMP_Y13_H						0x00007F30	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y13_L;				/*TONE_CTEMP_Y13_L						0x00007F32	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_13;			/*TONE_CTEMP_THREH_13					0x00007F34	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_13;			/*TONE_CTEMP_THREL_13					0x00007F36	(2byte)			 */
	unsigned short 				TONE_BV_THREH14_H;				/*TONE_BV_THREH14_H						0x00007F38	(2byte)			 */
	unsigned short 				TONE_BV_THREH14_L;				/*TONE_BV_THREH14_L						0x00007F3A	(2byte)			 */
	unsigned short 				TONE_BV_THREL14_H;				/*TONE_BV_THREL14_H						0x00007F3C	(2byte)			 */
	unsigned short 				TONE_BV_THREL14_L;				/*TONE_BV_THREL14_L						0x00007F3E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE14_H;			/*TONE_CTEMP_THRE14_H					0x00007F40	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE14_L;			/*TONE_CTEMP_THRE14_L					0x00007F42	(2byte)			 */
	unsigned short 				TONE_CTEMP_X14_H;				/*TONE_CTEMP_X14_H						0x00007F44	(2byte)			 */
	unsigned short 				TONE_CTEMP_X14_L;				/*TONE_CTEMP_X14_L						0x00007F46	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y14_H;				/*TONE_CTEMP_Y14_H						0x00007F48	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y14_L;				/*TONE_CTEMP_Y14_L						0x00007F4A	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_14;			/*TONE_CTEMP_THREH_14					0x00007F4C	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_14;			/*TONE_CTEMP_THREL_14					0x00007F4E	(2byte)			 */
	unsigned short 				TONE_BV_THREH15_H;				/*TONE_BV_THREH15_H						0x00007F50	(2byte)			 */
	unsigned short 				TONE_BV_THREH15_L;				/*TONE_BV_THREH15_L						0x00007F52	(2byte)			 */
	unsigned short 				TONE_BV_THREL15_H;				/*TONE_BV_THREL15_H						0x00007F54	(2byte)			 */
	unsigned short 				TONE_BV_THREL15_L;				/*TONE_BV_THREL15_L						0x00007F56	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE15_H;			/*TONE_CTEMP_THRE15_H					0x00007F58	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE15_L;			/*TONE_CTEMP_THRE15_L					0x00007F5A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X15_H;				/*TONE_CTEMP_X15_H						0x00007F5C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X15_L;				/*TONE_CTEMP_X15_L						0x00007F5E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y15_H;				/*TONE_CTEMP_Y15_H						0x00007F60	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y15_L;				/*TONE_CTEMP_Y15_L						0x00007F62	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_15;			/*TONE_CTEMP_THREH_15					0x00007F64	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_15;			/*TONE_CTEMP_THREL_15					0x00007F66	(2byte)			 */
	unsigned short 				TONE_BV_THREH16_H;				/*TONE_BV_THREH16_H						0x00007F68	(2byte)			 */
	unsigned short 				TONE_BV_THREH16_L;				/*TONE_BV_THREH16_L						0x00007F6A	(2byte)			 */
	unsigned short 				TONE_BV_THREL16_H;				/*TONE_BV_THREL16_H						0x00007F6C	(2byte)			 */
	unsigned short 				TONE_BV_THREL16_L;				/*TONE_BV_THREL16_L						0x00007F6E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE16_H;			/*TONE_CTEMP_THRE16_H					0x00007F70	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE16_L;			/*TONE_CTEMP_THRE16_L					0x00007F72	(2byte)			 */
	unsigned short 				TONE_CTEMP_X16_H;				/*TONE_CTEMP_X16_H						0x00007F74	(2byte)			 */
	unsigned short 				TONE_CTEMP_X16_L;				/*TONE_CTEMP_X16_L						0x00007F76	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y16_H;				/*TONE_CTEMP_Y16_H						0x00007F78	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y16_L;				/*TONE_CTEMP_Y16_L						0x00007F7A	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_16;			/*TONE_CTEMP_THREH_16					0x00007F7C	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_16;			/*TONE_CTEMP_THREL_16					0x00007F7E	(2byte)			 */
	unsigned short 				TONE_BV_THREH17_H;				/*TONE_BV_THREH17_H						0x00007F80	(2byte)			 */
	unsigned short 				TONE_BV_THREH17_L;				/*TONE_BV_THREH17_L						0x00007F82	(2byte)			 */
	unsigned short 				TONE_BV_THREL17_H;				/*TONE_BV_THREL17_H						0x00007F84	(2byte)			 */
	unsigned short 				TONE_BV_THREL17_L;				/*TONE_BV_THREL17_L						0x00007F86	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE17_H;			/*TONE_CTEMP_THRE17_H					0x00007F88	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE17_L;			/*TONE_CTEMP_THRE17_L					0x00007F8A	(2byte)			 */
	unsigned short 				TONE_CTEMP_X17_H;				/*TONE_CTEMP_X17_H						0x00007F8C	(2byte)			 */
	unsigned short 				TONE_CTEMP_X17_L;				/*TONE_CTEMP_X17_L						0x00007F8E	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y17_H;				/*TONE_CTEMP_Y17_H						0x00007F90	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y17_L;				/*TONE_CTEMP_Y17_L						0x00007F92	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_17;			/*TONE_CTEMP_THREH_17					0x00007F94	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_17;			/*TONE_CTEMP_THREL_17					0x00007F96	(2byte)			 */
	unsigned short 				TONE_BV_THREH18_H;				/*TONE_BV_THREH18_H						0x00007F98	(2byte)			 */
	unsigned short 				TONE_BV_THREH18_L;				/*TONE_BV_THREH18_L						0x00007F9A	(2byte)			 */
	unsigned short 				TONE_BV_THREL18_H;				/*TONE_BV_THREL18_H						0x00007F9C	(2byte)			 */
	unsigned short 				TONE_BV_THREL18_L;				/*TONE_BV_THREL18_L						0x00007F9E	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE18_H;			/*TONE_CTEMP_THRE18_H					0x00007FA0	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE18_L;			/*TONE_CTEMP_THRE18_L					0x00007FA2	(2byte)			 */
	unsigned short 				TONE_CTEMP_X18_H;				/*TONE_CTEMP_X18_H						0x00007FA4	(2byte)			 */
	unsigned short 				TONE_CTEMP_X18_L;				/*TONE_CTEMP_X18_L						0x00007FA6	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y18_H;				/*TONE_CTEMP_Y18_H						0x00007FA8	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y18_L;				/*TONE_CTEMP_Y18_L						0x00007FAA	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_18;			/*TONE_CTEMP_THREH_18					0x00007FAC	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_18;			/*TONE_CTEMP_THREL_18					0x00007FAE	(2byte)			 */
	unsigned short 				TONE_BV_THREH19_H;				/*TONE_BV_THREH19_H						0x00007FB0	(2byte)			 */
	unsigned short 				TONE_BV_THREH19_L;				/*TONE_BV_THREH19_L						0x00007FB2	(2byte)			 */
	unsigned short 				TONE_BV_THREL19_H;				/*TONE_BV_THREL19_H						0x00007FB4	(2byte)			 */
	unsigned short 				TONE_BV_THREL19_L;				/*TONE_BV_THREL19_L						0x00007FB6	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE19_H;			/*TONE_CTEMP_THRE19_H					0x00007FB8	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE19_L;			/*TONE_CTEMP_THRE19_L					0x00007FBA	(2byte)			 */
	unsigned short 				TONE_CTEMP_X19_H;				/*TONE_CTEMP_X19_H						0x00007FBC	(2byte)			 */
	unsigned short 				TONE_CTEMP_X19_L;				/*TONE_CTEMP_X19_L						0x00007FBE	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y19_H;				/*TONE_CTEMP_Y19_H						0x00007FC0	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y19_L;				/*TONE_CTEMP_Y19_L						0x00007FC2	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_19;			/*TONE_CTEMP_THREH_19					0x00007FC4	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_19;			/*TONE_CTEMP_THREL_19					0x00007FC6	(2byte)			 */
	unsigned short 				TONE_BV_THREH20_H;				/*TONE_BV_THREH20_H						0x00007FC8	(2byte)			 */
	unsigned short 				TONE_BV_THREH20_L;				/*TONE_BV_THREH20_L						0x00007FCA	(2byte)			 */
	unsigned short 				TONE_BV_THREL20_H;				/*TONE_BV_THREL20_H						0x00007FCC	(2byte)			 */
	unsigned short 				TONE_BV_THREL20_L;				/*TONE_BV_THREL20_L						0x00007FCE	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE20_H;			/*TONE_CTEMP_THRE20_H					0x00007FD0	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE20_L;			/*TONE_CTEMP_THRE20_L					0x00007FD2	(2byte)			 */
	unsigned short 				TONE_CTEMP_X20_H;				/*TONE_CTEMP_X20_H						0x00007FD4	(2byte)			 */
	unsigned short 				TONE_CTEMP_X20_L;				/*TONE_CTEMP_X20_L						0x00007FD6	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y20_H;				/*TONE_CTEMP_Y20_H						0x00007FD8	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y20_L;				/*TONE_CTEMP_Y20_L						0x00007FDA	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_20;			/*TONE_CTEMP_THREH_20					0x00007FDC	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_20;			/*TONE_CTEMP_THREL_20					0x00007FDE	(2byte)			 */
	unsigned short 				TONE_BV_THREH21_H;				/*TONE_BV_THREH21_H						0x00007FE0	(2byte)			 */
	unsigned short 				TONE_BV_THREH21_L;				/*TONE_BV_THREH21_L						0x00007FE2	(2byte)			 */
	unsigned short 				TONE_BV_THREL21_H;				/*TONE_BV_THREL21_H						0x00007FE4	(2byte)			 */
	unsigned short 				TONE_BV_THREL21_L;				/*TONE_BV_THREL21_L						0x00007FE6	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE21_H;			/*TONE_CTEMP_THRE21_H					0x00007FE8	(2byte)			 */
	unsigned short 				TONE_CTEMP_THRE21_L;			/*TONE_CTEMP_THRE21_L					0x00007FEA	(2byte)			 */
	unsigned short 				TONE_CTEMP_X21_H;				/*TONE_CTEMP_X21_H						0x00007FEC	(2byte)			 */
	unsigned short 				TONE_CTEMP_X21_L;				/*TONE_CTEMP_X21_L						0x00007FEE	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y21_H;				/*TONE_CTEMP_Y21_H						0x00007FF0	(2byte)			 */
	unsigned short 				TONE_CTEMP_Y21_L;				/*TONE_CTEMP_Y21_L						0x00007FF2	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREH_21;			/*TONE_CTEMP_THREH_21					0x00007FF4	(2byte)			 */
	unsigned short 				TONE_CTEMP_THREL_21;			/*TONE_CTEMP_THREL_21					0x00007FF6	(2byte)			 */
};


/*==========================================================================*/
/*  ROM Define																*/
/*  																		*/
/*==========================================================================*/
struct _B2Y2_map {
	unsigned short				TC_TBL_MON[21][256];			/*Tone Control(Monitor)					0x00000000	(10752byte)		 */
	unsigned short				TC_TBL_STILL[21][256];			/*Tone Control(Still)					0x00002A00	(10752byte)		 */
	unsigned short				TC_TBL_HD[21][256];				/*Tone Control(HD)						0x00005400	(10752byte)		 */
	struct _tone_sw_con			TONE_SW_CON;					/*Tone switch condition					0x00007E00	(504byte)		 */
	unsigned short				HST_COND[21][5];				/*Histogram condition					0x00007FF8	(210byte)		 */
	unsigned short				HST_THRE[21][10];				/*Histogram threshold					0x000080CA	(420byte)		 */
	unsigned short				HST_HYS[21][5];					/*Histogram Hysteresis					0x0000826E	(210byte)		 */
	unsigned short				AE_TARGET[21];					/*AE Target								0x00008340	(42byte)		 */
	unsigned short				dummy;							/*dummy									0x0000836A	(2byte)			 */
};


#endif  /* B2Y2_MAP_H_INC */
