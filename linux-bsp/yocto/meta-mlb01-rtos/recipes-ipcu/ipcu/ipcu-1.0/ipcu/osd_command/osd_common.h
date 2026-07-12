#ifndef _OSD_COMMON_H
#define _OSD_COMMON_H

#define OSD_FREE_TEXT			0
#define OSD_SYSTEM_TEXT_CAMID	1
#define OSD_SYSTEM_TEXT_IPADR	2
#define OSD_SYSTEM_TEXT_TIME	3


typedef struct{
	unsigned int osd_id;
	unsigned int cam_id;
	unsigned short x_pos;
	unsigned short y_pos;
	unsigned char fontcolor;
	unsigned char fontsize;
	unsigned char parency;
	unsigned char dumm;
	char text[32];
}T_OSD_OPTION;

#endif
