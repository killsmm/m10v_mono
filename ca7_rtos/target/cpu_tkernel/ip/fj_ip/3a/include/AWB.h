/*############################################################################**
**  Copyright 2015 Socionext Inc.
**  Name    : AE.c
**  Function: Auto White Balance
**  Last Update: 2002/01/09
**  Version : 1.4
**############################################################################*/
#ifndef _AWB_H_
#define _AWB_H_

/*#include "ddim_typedef.h"*/
#include "Monitor_data.h"

/******************************************************************************/
/* Function county where registers ********************************************/
extern void MLNA_AWB_Main(unsigned long *);		/* "Auto White Balance" Main Function */


extern void MLNA_AWB_Init(unsigned long ,unsigned long );	/* Setting of work area */


extern unsigned short MLNA_AWB_Set_ManXY(unsigned short , float );	/* XorY(ColorRatioPlane) is set up */

extern float MLNA_AWB_Get_XY(unsigned short , unsigned short );	/* XorY(ColorRatioPlane) value is acquired */

#define AWB_XYKIND_SOR			0x00	/* AWB SOuRce XorY      */
#define AWB_XYKIND_TAR			0x01	/* AWB TARget XorY      */
#define AWB_XYKIND_SET			0x02	/* AWB SETting XorY     */

#define AWB_XYKIND_X			0x00	/* AWB X(=R/G) */
#define AWB_XYKIND_Y			0x01	/* AWB Y(=B/G) */


extern void MLNA_AWB_Set_ManGain(unsigned short , unsigned short );	/* WB-Gain value is set up */

extern unsigned short MLNA_AWB_Get_Gain(unsigned short );	/* WB-Gain value is acquired */

#define AWB_GAIN_R				0x00	/* AWB RedGain   */
#define AWB_GAIN_G				0x01	/* AWB GreenGain */
#define AWB_GAIN_B				0x02	/* AWB BlueGain  */


extern unsigned short MLNA_AWB_Get_InputLight(void);	/* Obtained source of light number */
extern unsigned short MLNA_AWB_Get_TargetLight(void);	/* Source of light number under use */


extern void MLNA_AWB_Set_Speed( unsigned short );	/* Movement value is set up */
extern unsigned short MLNA_AWB_Get_Speed(void);	/* Movement value is acquired */


extern void MLNA_AWB_Set_TransitionSpeed(unsigned short );	/* Change rate of source of light is set up */
extern unsigned short MLNA_AWB_Get_TransitionSpeed(void);	/* Change rate of source of light is acquired */


extern void MLNA_AWB_Set_Mode(unsigned short );	/* AWB mode is set */
extern unsigned short MLNA_AWB_Get_Mode(void);	/* AWB mode is acquired */

#define AWB_ALLINTEGRAL_OFF		0x00	/* AWB All integral ColorFrame off */
#define AWB_ALLINTEGRAL_ON		0x01	/* AWB All integral ColorFrame on */
#define AWB_LIGHTSOURCE			0x02	/* AWB Light source (backward compatible) */
#define AWB_LIGHTESTIMATE			0x02	/* AWB Light estimate */
#define AWB_MANUALWB			0x03	/* AWB ManualWB     */
#define AWB_MANUALXY			0x04	/* AWB ManualXY     */


extern unsigned short MLNA_AWB_Get_AmbientLight(void);	/* Light-ambience(mode) is acquired */


extern void MLNA_AWB_Set_MinGateRate(unsigned short );	/* Setting of gate lower bound value */
extern unsigned short MLNA_AWB_Get_MinGateRate(void);	/* Acquisition of gate lower bound set value */


extern void MLNA_AWB_Set_LightBoundary(unsigned short , float );	/* Setting of source of light standard value */
extern float MLNA_AWB_Get_LightBoundary(unsigned short );	/* Acquisition of source of light standard, set value */

#define AWB_LIGHTBOUNDARY_HI	0x00	/* Hi-standerd brightness  */
#define AWB_LIGHTBOUNDARY_LOW	0x01	/* Low-standerd brightness */


extern void MLNA_AWB_Oneshot( void );	/* The one shot flag is hoisted */


extern void MLNA_AWB_Set_CRPaddr( unsigned long );	/* ColorRatioPlane-address is set */
extern unsigned long MLNA_AWB_Get_CRPaddr( void );	/* ColorRatioPlane-address is acquired */


extern unsigned long MLNA_AWB_Get_IntegralValue(unsigned short );	/* R(or G, or B)luminance is acquired */
extern float MLNA_AWB_Get_QoLight(void);	/* Quantity of light is acquired */
extern unsigned short MLNA_AWB_Get_GateRate(void);	/* Gatecount is acquired */


void MLNA_AWB_Get_Monitordata(AWB_MONITOR *);	/* Batch acquisition of monitor data */

#endif

/* End Of File */
