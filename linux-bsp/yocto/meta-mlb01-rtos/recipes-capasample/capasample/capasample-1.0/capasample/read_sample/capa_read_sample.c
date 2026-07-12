//******************************************************************************/
//
//  @file   fs_capa.c
//  @brief  test sample for Capability info
//
//
//  Copyright 2017 Socionext Inc.
//******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "cap_command.h"

/******************************************************************************
 *  @brief  test app
 *  @fn int main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 ******************************************************************************/
int main( int argc, char** argv )
{
	int ret;

	generate_capa_info();

	unsigned long type;
	unsigned long num;
	struct cameraif_string mode[4];
	memset(&mode, 0 , sizeof(mode));

	ret = get_Sensor_type(&type, &num, mode);
	if (ret == D_CM_RESULT_NG) {
		printf("Sensor type acquisition failed\n");
		delete_capa_info();
		return D_CM_RESULT_NG;
	}
	printf("Sensor type : %s\n", 
		type == 0 ? "4 Sensor" :
		type == 1 ? "Dual" :
		type == 2 ? "Single DOL 8M" :
		type == 3 ? "Single DOL 5M" :
		type == 4 ? "IMX277" :
		type == 5 ? "IMX377" : 
		type == 6 ? "IMX477" :
		"Error");

	for (int i = 0; i < num; i++)
		printf("%s -> [%lu]\n", mode[i].str, mode[i].camif);
	printf(">> Select stream type : ");
	unsigned long sn;
	scanf("%lu", &sn);

	unsigned long chnum;
	unsigned long chlst[4];
	memset(&chlst, 0 , sizeof(chlst));
	ret = get_Channel_type(&chnum, chlst);
	if (ret == D_CM_RESULT_NG) {
		printf("Channel type acquisition failed\n");
		delete_capa_info();
		return D_CM_RESULT_NG;
	}
	printf("Select stream\n");
	for (int j = 0; j < chnum; j++)
		printf("%lu : %s\n", chlst[j],
			chlst[j] == 0 ? "Main" :
			chlst[j] == 1 ? "Sub 1" :
			chlst[j] == 2 ? "Sub 2" :
			chlst[j] == 3 ? "Sub 3" :
			"Error");
	printf(">> Which stream are you using ? : ");
	unsigned long ss;
	scanf("%lu", &ss);

	printf("Make stream : %lu\n", ss);
	ret = make_Video_channel_info(ss, sn);
	if (ret == D_CM_RESULT_NG) {
		printf("Video channel acquisition failed\n");
		delete_capa_info();
		return D_CM_RESULT_NG;
	}

	printf("Video size\n");
	unsigned long vnum;
	struct cameraif_size size[15];
	memset(&size, 0 , sizeof(size));
	ret = get_Size(ss, &vnum, size);
	if (ret == D_CM_RESULT_NG) {
		printf("Video size acquisition failed\n");
		delete_capa_info();
		return D_CM_RESULT_NG;
	}
	for (int k = 0; k < vnum; k++)
		printf("%4lu - %4lu %10s : %2lu\n",
			size[k].width, size[k].height, size[k].str, size[k].camif);

	printf("Frame rate\n");
	unsigned long fnum;
	struct cameraif_number fr[15];
	memset(&size, 0 , sizeof(size));
	ret = get_Frame_rate(ss, &fnum, fr);
	if (ret == D_CM_RESULT_NG) {
		printf("Frame rate acquisition failed\n");
		delete_capa_info();
		return D_CM_RESULT_NG;
	}
	for (int l = 0; l < fnum; l++)
		printf("%5lu : %2lu\n", fr[l].num, fr[l].camif);

	delete_capa_info();

	return D_CM_RESULT_OK;
}
