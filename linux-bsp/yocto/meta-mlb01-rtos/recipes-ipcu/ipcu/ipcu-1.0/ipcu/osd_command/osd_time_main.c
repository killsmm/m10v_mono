#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include "osd_font.h"
#include "cmfwk_mm.h"
#include "cmfwk_std.h"

typedef struct{
	UINT32 osd_id;
	char text[TEXT_LEN_MAX];
}T_OSD_TIME_OPTION;

extern FJ_ERR_CODE fj_mm_open(VOID);

void disp_usage( void )
{
	printf("[Usage]\n");
	printf("osdcmd [osd id],[operation]\n");
	printf("\n");
	printf("[osd id]:  osd id: 2,5,8,11\n");
	printf("[text]: operation: ON or OFF\n");
}

int optstrtok(char **ostr, char* istr)
{
    char *tok;
    int cnt;
    cnt = 0;
    tok = strtok(istr,",");
    while( tok != NULL )
    {
        *ostr = tok;
        tok = strtok(NULL,",");
        *ostr++;
        cnt++;
    }
    return cnt;
}


int illegal_option_check( T_OSD_TIME_OPTION *option )
{
	int ret=1;
	
	if( (option->osd_id != 2)&&(option->osd_id != 5 )&&
		 (option->osd_id != 8)&&(option->osd_id != 11 )){
		printf("illegal osd_id\n");
		ret=0;
	}
/*	if( option->cam_id > CAM_ID_MAX ){
		printf("illegal cam_id\n");
		ret=0;
	}
	if( option->x_pos > X_POS_MAX ){
		printf("illegal x_pos\n");
		ret=0;
	}
	if( option->y_pos > Y_POS_MAX ){
		printf("illegal y_pos\n");
		ret=0;
	}
	if( option->fontcolor > FONTCOLOR_MAX ){
		printf("illegal fontcolor\n");
		ret=0;
	}
	if( option->fontsize > FONTSIZE_MAX ){
		printf("illegal fontsize\n");
		ret=0;
	}
	if( option->parency > PARENCY_MAX ){
		printf("illegal parency\n");
		ret=0;
	}*/
	if( strlen(option->text) > TEXT_LEN_MAX ){
		printf("text is too long\n");
		ret=0;
	}
	return ret;
}

int arg_parse(char *str, T_OSD_TIME_OPTION *option)
{
	INT32 client_num;
	char *op_src[9];

	client_num = optstrtok(op_src,str);
	if( client_num > 1 ){
		option->osd_id = atoi(op_src[0]);
		strcpy( &(option->text[0]), op_src[1]);
	}
	return client_num;
}

// ----------------------------------------------
// return cam_id corresponds system-texted osd_id.
// return CAM_ID_INVALID if osd_id is not system-texted.
// ----------------------------------------------
unsigned int lat_osdid2camid( unsigned int osd_id )
{
	unsigned int cam_id;
	
	switch( osd_id ){
		case OSD_ID_0:
		case OSD_ID_1:
		case OSD_ID_2:
			cam_id = CAM_ID_0;
			break;
		case OSD_ID_3:
		case OSD_ID_4:
		case OSD_ID_5:
			cam_id = CAM_ID_1;
			break;
		case OSD_ID_6:
		case OSD_ID_7:
		case OSD_ID_8:
			cam_id = CAM_ID_2;
			break;
		case OSD_ID_9:
		case OSD_ID_10:
		case OSD_ID_11:
			cam_id = CAM_ID_3;
			break;
		default:
			cam_id = CAM_ID_INVALID; // owner is user
			break;
	}
	return cam_id;
}

int main(int argc, char *argv[]){
//  osdcmd [osd id],[cam id],[x-pos],[y-pos],[fontcolor],[fontsize],[parency],[text or format]
//	FILE *fpw;
//	UCHAR fname_w[] = "out.yuv";	// fileout debug
//	UCHAR str[]="RTOS_LINUX"; // drawing character
//	UCHAR *buf;
//	UCHAR timeout_cnt=0;
//	UCHAR osd_off_flg = 0;
	T_OSD_TIME_OPTION option;
	T_OSDCMD_COMMON_MEM *memarea;
	UINT32 phys_adr_top, m_size;
	INT32 client_num;

	// GET OPTION
	client_num = arg_parse( argv[1], &option );
	if( client_num < 2){
		disp_usage();
		return 0;
	}
	if( illegal_option_check( &option ) == 0 ){
		disp_usage();
		return 0;
	}
	
	// get & asign share memory
	if (fj_mm_open() != FJ_ERR_OK) {
		printf("share mem open error.\n");
		return FJ_ERR_NG;
	}
	FJ_MM_getPhysAddrInfo(IPCU_MM_OSD, &phys_adr_top, &m_size);
//	printf("phys_adr=0x%08x, size=0x%08x\n",phys_adr_top,m_size);
	memarea = (T_OSDCMD_COMMON_MEM*)FJ_MM_phys_to_virt( phys_adr_top );
	
	// OSD ON/OFF
	if( (strcmp(option.text,"ON")==0) || (strcmp(option.text,"On")==0) || (strcmp(option.text,"on")==0)){
		memarea->osdih[option.osd_id].linux_busy_flg = 1;
		memarea->osdih[option.osd_id].cam_id = lat_osdid2camid( option.osd_id );
	}else if((strcmp(option.text,"OFF")==0) || (strcmp(option.text,"Off")==0) || (strcmp(option.text,"off")==0)){
		memarea->osdih[option.osd_id].cam_id = 0xFFFFFFFF;
	}else{
//		printf("invalid operation\n");
		disp_usage();
	}
	return 0;
}


