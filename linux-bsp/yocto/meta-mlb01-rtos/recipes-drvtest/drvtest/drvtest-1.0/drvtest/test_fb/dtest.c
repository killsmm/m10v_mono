#include <stdio.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "mlb_fb_user.h"

#define DIV_BYTE 8

#define X_PIXEL_MAX 320
#define Y_LINE_MAX  240

#define BORDER1 80
#define BORDER2 160

#define COLOR_RED    0xf800
#define COLOR_GREEN  0x07e0
#define COLOR_BLUE   0x001f
#define COLOR_WHITE  0xffff
#define COLOR_BLACK  0x0000
#define COLOR_YELLOW 0xffe0

static char DeviceName[32];
/* function prototype */
void send_current_error_msg(char *ptr);
void send_current_information(char *ptr);

/*
void jdisp3a_draw(int w, int h, int fmt, char *img, int fd_framebuffer)
{
    struct fb_var_screeninfo vinfo;
	unsigned long screensize;
    char *fbptr ;
    int x, y;

	vinfo.xres = w;
	vinfo.yres = h;
	vinfo.bits_per_pixel = fmt;
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / DIV_BYTE ;
    fbptr = (char *)mmap(0,screensize,PROT_READ | PROT_WRITE,MAP_SHARED,fd_framebuffer,0);
	ioctl( fd_framebuffer , 0x46FF, &vinfo );

    for ( y = 0 ; y < vinfo.xres * vinfo.yres ; y++ ) {
            *((unsigned long *)fbptr + y) = *((unsigned long *)img + y);
    }
}*/
static int Drawpaint(char *fbptr,const struct fb_var_screeninfo* vinfo)
{
	int retangle_with=vinfo->xres_virtual/2;
	int retangle_height=vinfo->yres_virtual/2;
	int pixel_byte= vinfo->bits_per_pixel/8;

	//clear all;
	memset(fbptr,0,vinfo->xres_virtual * vinfo->yres_virtual*pixel_byte);
	if(vinfo->bits_per_pixel == 32){
		unsigned int abgr=0x80<<24;
		abgr|=0xFF;//Red
		unsigned int *pBuffer=(unsigned int*)fbptr;

		//Draw a retangle
		for(int i=0;i<retangle_height;i++){
			for(int j=0;j<retangle_with;j++){
				*(pBuffer+j)=abgr;
			}
			pBuffer+=vinfo->xres_virtual;
		}
		//Draw a circle
		abgr=0x80<<24;
		abgr |=(0xFF<<8);//Green
		pBuffer=(unsigned int*)fbptr;
		pBuffer+=retangle_with;
		int circle_heart_x=vinfo->xres_virtual/4;
		int circle_heart_y=vinfo->yres_virtual/4;
		int rr=retangle_with*retangle_with/4;
		if(rr > retangle_height*retangle_height/4){
			rr=retangle_height*retangle_height/4;
		}
		for(int i=0;i<retangle_height;i++){
			int yy=circle_heart_y-i;
			yy=yy*yy;
			for(int j=0;j<retangle_with;j++){
				int x=circle_heart_x-j;
				if(x*x+yy<rr){
					*(pBuffer+j)=abgr;
				}
			}
			pBuffer+=vinfo->xres_virtual;
		}
		//Draw triangle
		abgr=0x80<<24;
		abgr|=(0xFF<<16);//Blue
		pBuffer=(unsigned int*)fbptr;
		pBuffer+=vinfo->yres_virtual*vinfo->xres_virtual/2;
		for(int i=0;i<retangle_height;i++){
			for(int j=0;j<retangle_with;j++){
				*(pBuffer+j)=abgr;
			}
			retangle_with-=2;
			pBuffer+=(vinfo->xres_virtual+1);
		}
		//Draw triangle 2
		abgr=0x80<<24;
		abgr|=((0xFF<<16)|(0xFF<<8));
		pBuffer=(unsigned int*)fbptr;
		pBuffer+=(vinfo->yres_virtual*vinfo->xres_virtual/2);
		pBuffer+=(vinfo->xres_virtual*3/4);
		retangle_with=1;
		for(int i=0;i<retangle_height;i++){
			int j;
			for(j=0;j<retangle_with;j++){
				*(pBuffer+j)=abgr;
			}
			if(j/2+(vinfo->xres_virtual*3/4)>=vinfo->xres_virtual){
				break;
			}
			retangle_with+=2;
			pBuffer+=(vinfo->xres_virtual-1);
		}
	}
	else{
		unsigned short abgr=0x8;
		abgr|=(0xF<<12);//Red
		unsigned short *pBuffer=(unsigned short*)fbptr;

		//Draw a retangle
		for(int i=0;i<retangle_height;i++){
			for(int j=0;j<retangle_with;j++){
				*(pBuffer+j)=abgr;
			}
			pBuffer+=vinfo->xres_virtual;
		}
		//Draw a circle
		abgr=0x8;
		abgr |=(0xF<<8);//Green
		pBuffer=(unsigned short*)fbptr;
		pBuffer+=retangle_with;
		int circle_heart_x=vinfo->xres_virtual/4;
		int circle_heart_y=vinfo->yres_virtual/4;
		int rr=retangle_with*retangle_with/4;
		if(rr > retangle_height*retangle_height/4){
			rr=retangle_height*retangle_height/4;
		}
		for(int i=0;i<retangle_height;i++){
			int yy=circle_heart_y-i;
			yy=yy*yy;
			for(int j=0;j<retangle_with;j++){
				int x=circle_heart_x-j;
				if(x*x+yy<rr){
					*(pBuffer+j)=abgr;
				}
			}
			pBuffer+=vinfo->xres_virtual;
		}
		//Draw triangle
		abgr=0x8;
		abgr|=(0xF<<4);//Blue
		pBuffer=(unsigned short*)fbptr;
		pBuffer+=vinfo->yres_virtual*vinfo->xres_virtual/2;
		for(int i=0;i<retangle_height;i++){
			for(int j=0;j<retangle_with;j++){
				*(pBuffer+j)=abgr;
			}
			retangle_with-=2;
			pBuffer+=(vinfo->xres_virtual+1);
		}
		//Draw triangle 2
		abgr=0x8;
		abgr|=((0xF<<8)|(0xF<<4));
		pBuffer=(unsigned short*)fbptr;
		pBuffer+=(vinfo->yres_virtual*vinfo->xres_virtual/2);
		pBuffer+=(vinfo->xres_virtual*3/4);
		retangle_with=1;
		for(int i=0;i<retangle_height;i++){
			int j;
			for(j=0;j<retangle_with;j++){
				*(pBuffer+j)=abgr;
			}
			if(j/2+(vinfo->xres_virtual*3/4)>=vinfo->xres_virtual){
				break;
			}
			retangle_with+=2;
			pBuffer+=(vinfo->xres_virtual-1);
		}

	}
	return 0;
}

static int ShowBuffer(int argc, char* argv[],
		int fd_framebuffer,
		struct fb_var_screeninfo* vinfo,
		unsigned int ioctl_code)
{
    FILE* fd;
	unsigned int screensize = 320*240*4*3;
  	int ret =0;
    char *fbptr ;
	screensize = vinfo->xres_virtual * vinfo->yres_virtual * 4;
	fbptr = (char *)mmap(0,screensize,PROT_READ | 	
					PROT_WRITE,MAP_SHARED,fd_framebuffer,0);
	if ( (int)fbptr == -1 ) {
	    send_current_error_msg("Don't get framebuffer device to memory !");
		ret =-1;
	}
	else {
		send_current_information("The framebuffer device was mapped !");
		printf("xres_virtual:%d,yres_virtual:%d\n",
			vinfo->xres_virtual, vinfo->yres_virtual);
		fd=fopen(argv[2+1],"rb");
		if(fd) {
			fread(fbptr,4,
				vinfo->xres_virtual * vinfo->yres_virtual,
				fd);
			fclose(fd);
		}
		else{
			//Darw by self
			Drawpaint(fbptr,vinfo);
		}
		switch(ioctl_code) {
		case FBIOSWICH_DROW_BUFFER://Pan switch buffer
			printf("ioctl_code:%x\n",ioctl_code);
			ret =  ioctl( fd_framebuffer , ioctl_code , &vinfo ) ;
			if ( ret ) {
				printf("ioctl err ! err:%d",ret);
			}
			break;
		case FBIOPAN_DISPLAY: 
			{
				if(argc == 7) {
					//Slide show 
					int xStep=strtoul(argv[3+1],NULL,0);
					int yStep=strtoul(argv[4+1],NULL,0);
					int sleep_ms =strtoul(argv[5+1],NULL,0);
					int xEnd = (vinfo->xres_virtual - vinfo->xres) ;
					int yEnd = (vinfo->yres_virtual - vinfo->yres) ;
		
					vinfo->xoffset = 0;
					vinfo->yoffset = 0 ;
					while(vinfo->xoffset< xEnd || vinfo->yoffset< yEnd) {
						ret =  ioctl( fd_framebuffer , ioctl_code , vinfo ) ;
						if ( ret ) {
							printf("ioctl err ! err:%d xoffset:%d yoffset:%d"
								,ret,vinfo->xoffset,vinfo->yoffset);
							break;
						}
						usleep(sleep_ms*1000);
						if(vinfo->xoffset< xEnd) {
							vinfo->xoffset += xStep;
						}
						if(vinfo->yoffset< yEnd) {
							vinfo->yoffset += yStep ;
						}
					}
				}
				else if(argc ==6){
					vinfo->xoffset=strtoul(argv[3+1],NULL,10);
					vinfo->yoffset=strtoul(argv[4+1],NULL,10);
					ret =  ioctl( fd_framebuffer , ioctl_code , vinfo ) ;
					if ( ret ) {
						printf("ioctl err ! err:%d xoffset:%d yoffset:%d"
							,ret,vinfo->xoffset,vinfo->yoffset);
					}
				}
				break;
			}
		}
		munmap(fbptr,screensize);
	}
	return ret;
}
static int SetioData(int argc, char* argv[],
		int fd_framebuffer,
		struct fb_var_screeninfo* vinfo,
		unsigned int ioctl_code)
{
  	int ret =0;
	switch(ioctl_code) {
		case FBIOPUT_VSCREENINFO://0x4601
		{

			printf("Before:xres_virtual=%u,yres_virtual=%u\n",
				vinfo->xres_virtual,vinfo->yres_virtual);
			vinfo->xres_virtual =strtoul(argv[2+1],NULL,0);
			vinfo->yres_virtual =strtoul(argv[3+1],NULL,0);
			ret =  ioctl( fd_framebuffer , ioctl_code , vinfo ) ;
			if ( ret ) {
				printf("Put ioctl err ! err:%d \n"
					,ret);
			}
			ret = ioctl( fd_framebuffer , FBIOGET_VSCREENINFO , vinfo ); 
			printf("After:xres_virtual=%u,yres_virtual=%u\n",
				vinfo->xres_virtual,vinfo->yres_virtual);
			break;
		}
		case FBIOPUT_TEST_VSCREENINFO://Test FBIOPUT_VSCREENINFO
			vinfo->xres =strtoul(argv[2+1],NULL,0);
			vinfo->yres =strtoul(argv[3+1],NULL,0);
			ret =  ioctl( fd_framebuffer , FBIOPUT_VSCREENINFO , vinfo ) ;
			if ( ret ) {
				printf("ioctl err ! err:%d \n"
					,ret);
			}
			break;
	}
	return ret;
}
static int SetParameter(int read_line_index,char read_buf[1024],struct fb_disp* gr)
{
	int ret =0;
	switch(read_line_index) {
	case 2:
		gr->iput_trans_setting.word = strtoul(read_buf,NULL,10);
		printf("iput_trans_setting:%lx\n",gr->iput_trans_setting.word);
		break;
	case 4:
		gr->start_pos.bit.dsh = strtoul(read_buf,NULL,10);
		break;
	case 6:
		gr->start_pos.bit.dsv = strtoul(read_buf,NULL,10);
		printf("start_pos:%lx,read_buf:%s\n",gr->start_pos.word,read_buf);
		break;
	case 8:
		gr->input_area_size.bit.ihsize = strtoul(read_buf,NULL,10);
		printf("input_area_size:%lx read_buf:%s\n",gr->input_area_size.word,read_buf);
		break;
	case 10:
		gr->input_area_size.bit.ivsize = strtoul(read_buf,NULL,10);
		printf("input_area_size:%lx ,read_buf:%s\n",gr->input_area_size.word,read_buf);
		break;
	case 12:
		gr->horizonal_resize.bit.rszm = strtoul(read_buf,NULL,10);
		break;
	case 14:
		gr->horizonal_resize.bit.rszn = strtoul(read_buf,NULL,10);
		printf("horizonal_resize:%lx\n",gr->horizonal_resize.word);
		break;
	case 16:
		gr->vertical_resize.bit.rszm = strtoul(read_buf,NULL,10);
		break;
	case 18:
		gr->vertical_resize.bit.rszn = strtoul(read_buf,NULL,10);
		printf("vertical_resize:%lx\n",gr->vertical_resize.word);
		break;
	case 20:
	case 22:
	case 24:
	case 26:
	case 28:
	case 30:
	case 32:
	case 34:
	case 36:
	case 38:
		gr->enable_show_area[read_line_index/2-10] = strtoul(read_buf,NULL,10);
		printf("enable_show_area[%d]=%d\n",read_line_index/2-10,gr->enable_show_area[read_line_index/2-10]);
		break;
	case 40:
	case 41:
	case 43:
	case 44:
	case 46:
	case 47:
	case 49:
	case 50:
	case 52:
	case 53:
	case 55:
	case 56:
	case 58:
	case 59:
	case 61:
	case 62:
	case 64:
	case 65:
	case 67:
	case 68:
	{
		read_line_index -=40;
		if((read_line_index%3)==0) {
			gr->input_image_position[read_line_index/3].bit.dsh = strtoul(read_buf,NULL,10);
		}
		else {
			gr->input_image_position[read_line_index/3].bit.dsv = strtoul(read_buf,NULL,10);
		}
		printf("input_image_position[%d]=%lx\n",
			read_line_index/3,gr->input_image_position[read_line_index/3].word);
		break;
	}
	case 70:
	case 71:
	case 73:
	case 74:
	case 76:
	case 77:
	case 79:
	case 80:
	case 82:
	case 83:
	case 85:
	case 86:
	case 88:
	case 89:
	case 91:
	case 92:
	case 94:
	case 95:
	case 97:
	case 98:
		read_line_index -=70;
		if((read_line_index%3) ==0){
			gr->input_image_size[read_line_index/3].bit.ihsize = strtoul(read_buf,NULL,10);
		}
		else {
			gr->input_image_size[read_line_index/3].bit.ivsize = strtoul(read_buf,NULL,10);
		}
		printf("input_image_size[%d]=%lx read_buf:%s\n",
			read_line_index/3,gr->input_image_size[read_line_index/3].word,read_buf);
		break;
	case 100:
	case 101:
	case 102:
	case 103:
	case 105:
	case 106:
	case 107:
	case 108:
	case 110:
	case 111:
	case 112:
	case 113:
	case 115:
	case 116:
	case 117:
	case 118:
	case 120:
	case 121:
	case 122:
	case 123:
	case 125:
	case 126:
	case 127:
	case 128:
	case 130:
	case 131:
	case 132:
	case 133:
	case 135:
	case 136:
	case 137:
	case 138:
	case 140:
	case 141:
	case 142:
	case 143:
	case 145:
	case 146:
	case 147:
	case 148:
		read_line_index -=100;
		gr->input_image_ph_address[read_line_index/5].address[read_line_index%5]
			 = strtoul(read_buf,NULL,16);
		printf("input_image_ph_address[%d][%d]=%x read_buf:%s\n",
			read_line_index/5,read_line_index%5,
			gr->input_image_ph_address[read_line_index/5].address[read_line_index%5],
			read_buf);
		break;
	case 150:
		gr->gri_trg
			 = strtoul(read_buf,NULL,10);
		printf("gri_trg=%d\n",
			gr->gri_trg);
		break;
	case 152:
	case 154:
	case 156:
	case 158:
	case 160:
	case 162:
	case 164:
	case 166:
	case 168:
	case 170:
		read_line_index /=2;
		read_line_index -=76;
		gr->input_image_horizonal_bytes[read_line_index]
			 = strtoul(read_buf,NULL,0);
		printf("input_image_horizonal_bytes[%d]=%lx\n",
			read_line_index,
			gr->input_image_horizonal_bytes[read_line_index]);
		break;
	case 172:
	case 173:
		read_line_index -=172;
		gr->res_virtual[read_line_index] = strtoul(read_buf,NULL,10);
		break;
	case 175:
	case 176:
		read_line_index -=175;
		gr->pan_step[read_line_index] = strtoul(read_buf,NULL,10);
		break;
	case 178:
	case 179:
		read_line_index -=178;
		gr->drawing_buffer_top_address_size[read_line_index] = strtoul(read_buf,NULL,16);
		break;
	default:
		ret=-1;
		printf("error line:%d\n",read_line_index);
		break;
	}
	return ret;
}
//All setting data is set at a file
static int GetAllData(int fd_framebuffer,unsigned int ioctl_code)
{
	struct fb_disp gr;
	int i;
	int ret = ioctl( fd_framebuffer , ioctl_code , &gr) ;

	if(ret==0) {
		printf("iput_trans_setting:%lu\n",
			gr.iput_trans_setting.word);
		printf("input_area_size:(%u,%u)\n",
			gr.input_area_size.bit.ihsize,
			gr.input_area_size.bit.ivsize);
		printf("start_pos:(%u,%u)\n",
			gr.start_pos.bit.dsh,
			gr.start_pos.bit.dsv);
		printf("horizonal_resize:%u/%u\n",
			gr.horizonal_resize.bit.rszm,
			gr.horizonal_resize.bit.rszn);
		printf("vertical_resize:%u/%u\n",
			gr.vertical_resize.bit.rszm,
			gr.vertical_resize.bit.rszn);
		for(i=0;i<MAX_DRAW_AREA_NUM;i++) {
			printf("input_image_size[%d]:(%u,%u)\n",
			i,gr.input_image_size[i].bit.ihsize,
			gr.input_image_size[i].bit.ivsize);
			printf("input_image_position[%d]:(%u,%u)\n",
			i,gr.input_image_position[i].bit.dsh,
			gr.input_image_position[i].bit.dsv);
			printf("enable_show_area[%d]:%u\n",
			i,gr.enable_show_area[i]);
			printf("input_image_horizonal_bytes[%d]:%lu\n",
			i,gr.input_image_horizonal_bytes[i]);
			printf("input_image_ph_address[%d]:%x,%x,%x\n",
					i,gr.input_image_ph_address[i].address[0],
					gr.input_image_ph_address[i].address[1],
					gr.input_image_ph_address[i].address[2]);
		}
		printf("drawing_buffer_top_address_size:(%x,%x)\n",
			gr.drawing_buffer_top_address_size[0],
			gr.drawing_buffer_top_address_size[1]);
		printf("gri_trg:%u\n",
				gr.gri_trg);
		printf("valid_input_area:0x%x\n",gr.valid_input_area);
		printf("current_buffer_index:%d\n",gr.current_buffer_index);
		printf("valid_buffer_number:%u\n",gr.valid_buffer_number);
		printf("res_virtual:(%u,%u)\n",gr.res_virtual[0],gr.res_virtual[1]);
		printf("byte_num_line:%u\n",gr.byte_num_line);
		printf("pan_step:%u,%u\n",gr.pan_step[0],gr.pan_step[1]);
		printf("setting_name:%s\n",gr.setting_name);
	}
	return ret; 
}
static int GetDeviceName(const char* devicename)
{
	int ret = -1;
    int fd_framebuffer ;
    struct fb_fix_screeninfo finfo;
	const char fb_device_name[4][16] = {
		"/dev/fb0",
		"/dev/fb1",
		"/dev/fb2",
		"/dev/fb3"
	};
	const char fb_device[4][16] = {
		"lcd-gr0",
		"lcd-gr1",
		"hdmi-gr0",
		"hdmi-gr1"
	};
	for(int i=0;i<4;i++) {
		if(strcmp(devicename,fb_device[i])==0){
			ret = 0;
			break;
		}
	}
	if(ret !=0){
        send_current_error_msg("Device name error !");
		return ret;
	}

	for(int i=0;i<4;i++) {
	    fd_framebuffer = open( fb_device_name[i] , O_RDWR);
	    if ( !fd_framebuffer ) {
	        send_current_error_msg("Framebuffer device open error !");
			break;
		}
		ret = ioctl( fd_framebuffer , FBIOGET_FSCREENINFO , &finfo ); 
		if(ret !=0){		
			close(fd_framebuffer);
	        send_current_error_msg("Get fix parameter error !");
			break;
		}
		if(strcmp(devicename,finfo.id) == 0){
			ret=0;
			close(fd_framebuffer);
			strcpy(DeviceName,fb_device_name[i]);
			break;
		}
		close(fd_framebuffer);
    }
	printf("DeviceName:%s\n",devicename);
	return ret;
}
//All setting data is set at a file
static int SetAllData(int fd_framebuffer,unsigned int ioctl_code ,const char* parameter_file)
{
	int ret = 0;
    FILE* fd=fopen(parameter_file,"r");
	char read_buf[1024];
	if(fd) {
		struct fb_disp gr;
		int read_line_index=1;
		memset(&gr,0,sizeof(gr));
		while(fgets(read_buf,sizeof(read_buf),
			fd) !=NULL) {
			if(read_buf[0] !='#' ) {
			//# is comment
				ret = SetParameter(read_line_index,read_buf,&gr);
				if( ret !=0) {
					break;
				}
			}
			read_line_index++;
		}
		fclose(fd);
		if(ret == 0) {
			ret = ioctl( fd_framebuffer , ioctl_code , &gr ) ;
		}
	}
	else {
		printf("Error! File %s is not exist\n",parameter_file);
	}
	return ret;
}
static int GetDrawArea(int fd_framebuffer,
						draw_area* user_draw_area)
{
	ioctl( fd_framebuffer , FBIOGET_DRAWAREA , user_draw_area );
	for(int i=0;i<MAX_DRAW_AREA_NUM;i++) {
		printf("Curent[%d](%u,%u,%u,%u),Enable:%d,bytes:%lu\n",
			i,
			user_draw_area->input_image_position[i].bit.dsh,
			user_draw_area->input_image_position[i].bit.dsv,
			user_draw_area->input_image_size[i].bit.ihsize,
			user_draw_area->input_image_size[i].bit.ivsize,
			user_draw_area->enable_show_area[i],
			user_draw_area->input_image_horizonal_bytes[i]);

	}
	return 0;	
}
static int SetDrawArea(int fd_framebuffer,
						struct fb_var_screeninfo* vinfo,
						unsigned int AreaNum)
{
	draw_area user_draw_area;
	int loop_num,i;
	GetDrawArea(fd_framebuffer,
				&user_draw_area);
	unsigned int horizonal=vinfo->xres/AreaNum;
	printf("xres:%d,yres:%d\n",vinfo->xres,vinfo->yres);
	if((AreaNum * horizonal == vinfo->xres) && 
		(horizonal % 4) == 0) {
		loop_num = AreaNum;
	}
	else{
		loop_num = AreaNum -1;
		horizonal = horizonal / 4 * 4;
	}
	for(i=0;i<loop_num;i++) {
		user_draw_area.input_image_position[i].bit.dsv=0;
		user_draw_area.input_image_position[i].bit.dsh = i*horizonal;
		user_draw_area.enable_show_area[i] = 1,
		user_draw_area.input_image_size[i].bit.ihsize =  horizonal;
		user_draw_area.input_image_size[i].bit.ivsize = vinfo->yres;
	}
	if(loop_num<AreaNum) {
		user_draw_area.input_image_position[loop_num].bit.dsv=0;
		user_draw_area.input_image_position[loop_num].bit.dsh = 
			loop_num*horizonal;
		user_draw_area.enable_show_area[loop_num] = 1,
		user_draw_area.input_image_size[loop_num].bit.ihsize = 
			vinfo->xres - loop_num*horizonal;
		user_draw_area.input_image_size[loop_num].bit.ivsize = 
			vinfo->yres;
	}
	for(i=AreaNum;i<MAX_DRAW_AREA_NUM;i++) {
		user_draw_area.enable_show_area[i] = 0;
	}
	for(int i=0;i<MAX_DRAW_AREA_NUM;i++) {
		printf("future[%d](%u,%u,%u,%u),Enable:%d,bytes:%lu\n",
			i,
			user_draw_area.input_image_position[i].bit.dsh,
			user_draw_area.input_image_position[i].bit.dsv,
			user_draw_area.input_image_size[i].bit.ihsize,
			user_draw_area.input_image_size[i].bit.ivsize,
			user_draw_area.enable_show_area[i],
			user_draw_area.input_image_horizonal_bytes[i]);

	}
	return ioctl( fd_framebuffer , FBIOSET_DRAWAREA , &user_draw_area ) ;
}
int main(int argc, char* argv[])
{
    struct fb_var_screeninfo vinfo;
    int fd_framebuffer ;
  	int ret = GetDeviceName(argv[1]);
	
	//Setting
	if( ret !=0)
	{
		return -1;
	}

    /* Open the FB device */
    fd_framebuffer = open( DeviceName , O_RDWR);
    if ( !fd_framebuffer ) {
        send_current_error_msg("Framebuffer device open error !");
    }
	else {
		unsigned int ioctl_code = strtoul(argv[1+1],NULL,16);
		send_current_information("The framebuffer device was opened !");
		ret = ioctl( fd_framebuffer , FBIOGET_VSCREENINFO , &vinfo ); 
		if ( ret ) {
		    printf("Variable information not gotton ! err:%d",ret);
		}
		else {
			switch(ioctl_code) {
			case FBIOSWICH_DROW_BUFFER://Pan switch buffer 0x5000
			case FBIOPAN_DISPLAY: //Slide show 0x4606
				ShowBuffer(argc, argv,
					fd_framebuffer,
					&vinfo,
					ioctl_code);
				break;
			case FBIOPUT_VSCREENINFO://0x4601
			case FBIOPUT_TEST_VSCREENINFO://5500
				printf("Want to set to\n");
				printf("res:(%u,%u),ures_virtual(%u,%u),offset(%u,%u),bits_per_pixel:%u,grayscale:%u,R(%u,%u,%u),G(%u,%u,%u),B(%u,%u,%u),A(%u,%u,%u),nonstd:%u,activate:%u,height:%u,width:%u,accel_flags:%u,",
				vinfo.xres,vinfo.yres,
				vinfo.xres_virtual,vinfo.yres_virtual,
				vinfo.xoffset,vinfo.yoffset,
				vinfo.bits_per_pixel,vinfo.grayscale,
				vinfo.red.offset,vinfo.red.length,vinfo.red.msb_right,
				vinfo.green.offset,vinfo.green.length,vinfo.green.msb_right,
				vinfo.blue.offset,vinfo.blue.length,vinfo.blue.msb_right,
				vinfo.transp.offset,vinfo.transp.length,vinfo.transp.msb_right,
				vinfo.nonstd,vinfo.activate,
				vinfo.height,vinfo.width,
				vinfo.accel_flags
				);
				printf("pixclock:%u,left_margin:%u,right_margin:%u,upper_margin:%u,lower_margin:%u,hsync_len:%u,vsync_len:%u,sync:%u,vmode:%u,rotate:%u,colorspace:%u\n",
				vinfo.pixclock,
				vinfo.left_margin,vinfo.right_margin,
				vinfo.upper_margin,vinfo.lower_margin,
				vinfo.hsync_len,
				vinfo.vsync_len,
				vinfo.sync,vinfo.vmode,
				vinfo.rotate,vinfo.colorspace);
				SetioData(argc, argv,
					fd_framebuffer,
					&vinfo,
					ioctl_code);
				break;
			case FBIOBLANK: {//Blank 0x4611
				unsigned int blank=strtoul(argv[2+1],NULL,0);//1: blank 0: No-blank
				ret =  ioctl( fd_framebuffer , ioctl_code , blank ) ;
				if ( ret ) {
					printf("ioctl err ! err:%d blank:%d \n"
						,ret,blank);
				}
			}
				break;
			case FBIOENABLE_DROW_AREA: //0x5001
			{
				unsigned int EnableArea =strtoul(argv[2+1],NULL,16);
				ret =  ioctl( fd_framebuffer , ioctl_code , EnableArea ) ;
				if ( ret ) {
					printf("ioctl err ! err:%d \n"
						,ret);
				}
				break;
			}
			case FBIOFULL_GETTING://(0x5007)Get full GR
				ret = GetAllData(fd_framebuffer, ioctl_code );
				if(ret !=0) {
					printf("ioctl err ! err:%d \n"
						,ret);
				}
				break;
			case FBIOFULL_SETTING://0x5002
				ret = SetAllData(fd_framebuffer, ioctl_code , argv[2+1]);
				if(ret !=0) {
					printf("ioctl err ! err:%d \n"
						,ret);
				}
				break;				
			case FBIOSET_TRIGER://Set triger 0x5003
			case FBIOSWITCH_GR_SETTING://0x5004
				ret =  ioctl( fd_framebuffer , ioctl_code , argv[2+1][0]-0x30 ) ;
				break;
			case FBIOGET_DRAWAREA:{//0x5006
				draw_area user_draw_area;
				ret = GetDrawArea(fd_framebuffer,
							&user_draw_area);
				break;
			}
			case FBIOSET_DRAWAREA:{//0x5005
				unsigned int AreaNum=strtoul(argv[2+1],NULL,10);
				if(AreaNum<MAX_DRAW_AREA_NUM) {
					ret = SetDrawArea(fd_framebuffer, &vinfo,AreaNum);
				}
				else {
					printf("Area num should be little than 10\n");
				}
				break;
			}
			case FBIOGET_VSCREENINFO://0x4600
				printf("res:(%u,%u),ures_virtual(%u,%u),offset(%u,%u),bits_per_pixel:%u,grayscale:%u,R(%u,%u,%u),G(%u,%u,%u),B(%u,%u,%u),A(%u,%u,%u),nonstd:%u,activate:%u,height:%u,width:%u,accel_flags:%u,",
				vinfo.xres,vinfo.yres,
				vinfo.xres_virtual,vinfo.yres_virtual,
				vinfo.xoffset,vinfo.yoffset,
				vinfo.bits_per_pixel,vinfo.grayscale,
				vinfo.red.offset,vinfo.red.length,vinfo.red.msb_right,
				vinfo.green.offset,vinfo.green.length,vinfo.green.msb_right,
				vinfo.blue.offset,vinfo.blue.length,vinfo.blue.msb_right,
				vinfo.transp.offset,vinfo.transp.length,vinfo.transp.msb_right,
				vinfo.nonstd,vinfo.activate,
				vinfo.height,vinfo.width,
				vinfo.accel_flags
				);
				printf("pixclock:%u,left_margin:%u,right_margin:%u,upper_margin:%u,lower_margin:%u,hsync_len:%u,vsync_len:%u,sync:%u,vmode:%u,rotate:%u,colorspace:%u\n",
				vinfo.pixclock,
				vinfo.left_margin,vinfo.right_margin,
				vinfo.upper_margin,vinfo.lower_margin,
				vinfo.hsync_len,
				vinfo.vsync_len,
				vinfo.sync,vinfo.vmode,
				vinfo.rotate,vinfo.colorspace);
				break;
		case FBIOGET_FSCREENINFO:{//0x4602
			struct fb_fix_screeninfo finfo;
			ret = ioctl( fd_framebuffer , FBIOGET_FSCREENINFO , &finfo ); 
			if ( ret ) {
				printf("Variable information not gotton ! err:%d",ret);
			}
			 printf ("Fixed screen info:\n"  
            "\tid: %s\n"  
            "\tsmem_start: 0x%lx\n"  
            "\tsmem_len: %d\n"  
            "\ttype: %d\n"  
            "\ttype_aux: %d\n"  
            "\tvisual: %d\n"  
            "\txpanstep: %d\n"  
            "\typanstep: %d\n"  
            "\tywrapstep: %d\n"  
            "\tline_length: %d\n"  
            "\tmmio_start: 0x%lx\n"  
            "\tmmio_len: %d\n"  
            "\taccel: %d\n"  
            "\n",  
            finfo.id, finfo.smem_start, finfo.smem_len, finfo.type,  
            finfo.type_aux, finfo.visual, finfo.xpanstep, finfo.ypanstep,  
            finfo.ywrapstep, finfo.line_length, finfo.mmio_start,  
            finfo.mmio_len, finfo.accel);  
			break;
		}
		case FBIOPUT_TEST_IPO:{
			switch(argv[2+1][0]){
			case 'r':
				vinfo.red.offset = strtoul(argv[3+1],NULL,10);
				vinfo.red.msb_right = strtoul(argv[4+1],NULL,10);
				break;
			case 'g':
				vinfo.green.offset = strtoul(argv[3+1],NULL,10);
				vinfo.green.msb_right = strtoul(argv[4+1],NULL,10);
				break;
			case 'b':
				vinfo.blue.offset = strtoul(argv[3+1],NULL,10);
				vinfo.blue.msb_right = strtoul(argv[4+1],NULL,10);
				break;
			default:
				vinfo.transp.offset = strtoul(argv[3+1],NULL,10);
				vinfo.transp.msb_right = strtoul(argv[4+1],NULL,10);
				break;
			}
			ret = ioctl( fd_framebuffer , FBIOPUT_VSCREENINFO , &vinfo ); 
			break;
		}
		default:
			printf("USAG: %s P1 P2 P3 P4 P5 P6\n",argv[0] );
			printf("P1: specify device\n");
			printf("    lcd-gr0: LCD’s GR0\n");
			printf("    lcd-gr1: LCD’s GR1\n");
			printf("	hdmi-gr0: LCD’s GR0\n");
			printf("	hdmi-gr1: LCD’s GR1\n");
			printf("P2: Command code\n");
			printf("	0x4600:Get fb_var_screeninfo\n");
			printf("	0x4601:Change virtual screen size\n");
			printf("	     P3:Horizontal size\n");
			printf("	        P4:Vertical size\n");
			printf("	0x4602:Get fb_fix_screeninfo\n");
			printf("	0x4611:Set all OSD’s sub-drawing area to off\n");
			printf("	     P3:1=Blank 0=None Blank\n");
			printf("	0x4606:Load a file or draw some geometry to virtual display and show them to display by specified position.\n");
			printf("	     P3:A file path want to show. The file will be loaded to drawing buffer.Some geometry object will be drawn if the file don’t exist.\n");
			printf("	        P4:Horizontal offset\n");
			printf("	           P5:Vertical offset\n");
			printf("	              [P6]:Slide show timer. A slide show will be done if this parameter is specified. It can be omitted\n");
			printf("	0x5000:Load a file or draw some geometry to drawing buffer and show them to display\n");
			printf("	     P3:A file path want to show. The file will be loaded to drawing buffer.Some geometry object will be drawn if the file don’t exist.\n");
			printf("	0x5001:Enable/Disable sub-drawing area\n");
			printf("	     P3:A bitmap specified area. 0~9 bit mean area0~area9. 1: Enable. 0: Disable\n");
			printf("	0x5003:Switch OSD pattern\n");
			printf("	     P3:Pattern index\n");
			printf("	0x5005:Reset all sub-drawing area\n");
			printf("	     P3:Sub-drawing area number.\n");
			printf("	0x5002:Set all GR’s data from a setting file\n");
			printf("	     P3:Setting file’s path.\n");
			printf("	0x5007:Get all GR’s data\n");
				break;
			}
			close(fd_framebuffer);
		}	
	}
    return 0;
}

void send_current_error_msg(char *ptr)
{
    fprintf( stderr , "%s\n" , ptr );
}

void send_current_information(char *ptr)
{
    fprintf( stdout , "%s\n" , ptr );

}


