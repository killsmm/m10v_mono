/* Copyright 2000 MSG Corporation */

/* Structural body for Createcolorframe */

#ifndef _CRPDEFH_
#define _CRPDEFH_

typedef struct  { 
unsigned char cr_plane[4][128][128];					/* Createcolorframe table */
									/* the 1st Index:  0:daytime 1:room 2:nighttime 4:All-integration-colorframe */
									/* the 2nd Index :Createcolorframe x coordinates */
									/* the 3rd Index :Createcolorframe y coordinates */
									/* value:Source of light number */
float color_source[12][2];			/* Source of light information table */
									/* the 1st Index :Source of light number */
									/* the 2nd Index:  0:x coordinates  1:y coordinates */
									/* value:Coordinates value */
} CRP;

extern void AWB_Createcolorframe(void);

#endif
