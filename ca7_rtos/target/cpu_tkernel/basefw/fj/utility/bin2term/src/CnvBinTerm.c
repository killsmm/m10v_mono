/**
 * @file		CnvBinTerm.c
 * @brief		Convert Bin to Terminal text / Terminal text to Bin
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* DEFINE ////////////////////////////////////////////////////////// */

/* INCLUDE ///////////////////////////////////////////////////////// */
#include <stdio.h>
#include <string.h>
#include "ddim_typedef.h"

/* DEFINE ////////////////////////////////////////////////////////// */

/* EXTERNAL //////////////////////////////////////////////////////// */
/* Subroutine ====================================================== */
extern INT32 io_gets(char* str);

/* Variable ======================================================== */

/* WORK AREA /////////////////////////////////////////////////////// */

/* PROGRAM ///////////////////////////////////////////////////////// */
/* Bin2Term routine ================================================ */
VOID Bin2Term(ULONG adrs, ULONG sz, CHAR* filename)
{
	unsigned char *p;
	unsigned long filesize;
	unsigned long cnt;
	unsigned long cnt1;
	char* fname;			/* file name */
	int checksum;
	int i;
	unsigned char indata;

	p = (unsigned char *)adrs;
	cnt1 = filesize = sz;
	fname = filename;

	printf("!=*=*= CnvBinTerm <START> =*=*=!\n");
	printf("%s\n", fname);
	printf("%08lx\n", filesize);
	
	for(cnt=(filesize+32-1)/32; cnt>0; cnt--){
		checksum = 0;
		for(i=0; i<32; i++){
			if(0!=cnt1){
				cnt1--;
				indata = *p;
				p++;
			} else {
				indata = 0;
			}
			checksum += (int)indata;
			printf("%02x", (int)indata);
		}
		checksum &= 0x00FF;
		printf("%02x\n", checksum);
	}
	
	printf("!=*=*= CnvBinTerm <END> =*=*=!\n");
}

/* Term2Bin routine ================================================ */
VOID Term2Bin(ULONG adrs)
{
	unsigned char *p;
	char in_line[255];
	int i;
	int j;
	int checksum;
	unsigned long filesize;
	int cnt;
	char wkdata[3] = {'\0', '\0', '\0'};
	
	p = (unsigned char *)adrs;

	do {
		if((i=io_gets(in_line)) == 0){
   	    	printf("\nERROR : Can not input !!\n");
			return;
		}
		in_line[i] = '\0';
		i--;
		if((i > 0) && (in_line[i] == '\r')){
			in_line[i] = '\0';
		}
	}while(strcmp(in_line, "!=*=*= CnvBinTerm <START> =*=*=!"));

/* get file name */
	if(io_gets(in_line) == 0){
		printf("\nERROR : Can not find output file name !!\n");
		return;
	}

/* get file length */
	if((i=io_gets(in_line)) == 0){
		printf("\nERROR : Can not find file length !!\n");
		return;
	}
	in_line[i] = '\0';
	sscanf(in_line, "%lx", &filesize);

	for(cnt=(filesize+32-1)/32; cnt>0; cnt--){
		if((i=io_gets(in_line)) == 0){
   	    	printf("\nERROR : Can not find end !!\n");
			return;
   	    }
		in_line[i] = '\0';
		if(i != 66){
   	    	printf("\nERROR : CnvBinTerm format !!\n");
			return;
   	    }
		checksum = 0;
		for(i=0; i<32*2; i+=2){
			if(filesize == 0) break;
			filesize--;
			wkdata[0] = in_line[i];
			wkdata[1] = in_line[i+1];
			sscanf(wkdata, "%x", &j);
			j &= 0x00FF;
			*p = (unsigned char)j;
			p++;
			checksum += j;
		}

		checksum &= 0x00FF;
		sscanf(in_line+64, "%x", &j);
		j &= 0x00FF;
		if(checksum != j){
			printf("\nERROR : CnvBinTerm checksum !!\n");
			return;
   	    }
	}

	if((i=io_gets(in_line)) == 0){
		printf("\nERROR : Can not find end-mark !!\n");
		return;
	}
	in_line[i] = '\0';
	i--;
	if((i > 0) && (in_line[i] == '\r')){
		in_line[i] = '\0';
	}
	if(strcmp(in_line, "!=*=*= CnvBinTerm <END> =*=*=!")){
		printf("\nERROR : Can not find end-mark !!\n");
		return;
	}
}

/* End of File ///////////////////////////////////////////////////// */
