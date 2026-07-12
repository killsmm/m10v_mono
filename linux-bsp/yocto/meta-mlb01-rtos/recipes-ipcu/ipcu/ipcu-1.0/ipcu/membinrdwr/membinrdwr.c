#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<getopt.h>

#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<sys/mman.h>

#include <linux/ipcu_userland.h>
#include "cmfwk_std.h"

char *tool_initialize(UINT32 param_mem, UINT32 param_mem_length);
void tool_terminate(char *pmem_bin, UINT32 param_mem_length);


int main(int argc, char *argv[]) 
{
	int result;
	UINT32 param_mem = 0;
	UINT32 param_mem_size = 512;
	int param_offset_index = -1;
	int param_index = -1;
	int param_position = -1;
	int param_format = 0;
	int param_size_index = -1;
	int param_size_mag = 1;
	char param_header[32];
	int param_write = 0;
	
	// read options
	while((result=getopt(argc,argv,"wrh:m:z:o:t:i:p:f:s:n:"))!=-1){
		switch(result){

			case 'r':
			param_write = 0;
			break;
			case 'w':
			param_write = 1;
			break;
			
			case 'h':
			strcpy(param_header,optarg);
			break;
			
			case 'm':
			if ( optarg[1]=='x' ){
				sscanf(&optarg[2],"%x,%x",&param_mem,&param_mem_size);
			}else{
				param_mem = (UINT32)atoi(optarg);
			}
			break;
			
			case 'z':
			param_mem_size = atoi(optarg);
			break;
			
			case 'o':
			param_offset_index = atoi(optarg);
			break;
			
			case 't':
			printf("t:%s\n",optarg);
			break;
			
			case 'i':
			switch(atoi(optarg)){
				case 0:
				param_index = 0;
				break;
				case 1:
				param_index = 1;
				break;
				case 2:
				param_index = 2;
				break;
				case 3:
				break;
				case 4:
				break;
			}
			break;

			case 'p':
			param_position = atoi(optarg);
			break;

			case 's':
			param_size_index = atoi(optarg);
			break;

			case 'f':
			param_format = atoi(optarg);
			break;

			case 'n':
			param_size_mag = atoi(optarg);
			break;
			
			
		}
	}
	argc -= optind;
	argv += optind;

	if ( param_write==1 && argc==0 ){
		LOGE("error param_write=%d, no arg\n",param_write);
		return 1;
	}
	if ( param_write==0 && argc!=0 ){
		LOGE("error param_write=%d, but have arg %d - %s\n",param_write,argc,argv[0]);
		return 1;
	}

	LOGV("mem=%x, index_offset=%d, index=%d, position=%d, arg = %s\n", 
		 param_mem, param_offset_index, param_index, param_position, argv[0]);
	
//	if ( param_mem<0 ){
//		LOGE("error param_mem=%d\n",param_mem);
//		return 1;
//	}
	if ( param_offset_index<0 ){
		LOGE("error param_offset_index=%d\n",param_offset_index);
		return 1;
	}
	if ( param_index<0 ){
		LOGE("error param_index=%d\n",param_index);
		return 1;
	}
	if ( param_position<0 ){
		LOGE("error param_position=%d\n",param_position);
		return 1;
	}

	char *pmem_bin;
	pmem_bin = (char*)tool_initialize(param_mem,param_mem_size);
	if ( pmem_bin==NULL ){
		printf("error tool_initialize(%x)\n",param_mem);
		return 2;
	}
	
	
	char mem_header[32];
	mem_header[0] = pmem_bin[0];
	mem_header[1] = pmem_bin[1];
	mem_header[2] = pmem_bin[2];
	mem_header[3] = pmem_bin[3];
	mem_header[4] = 0;
	LOGV("pmem_bin=%x \n", pmem_bin);
	LOGV("mem_header=%s \n", mem_header);
	// if ( strcmp(param_header,mem_header)!=0 ){
	//	LOGE("%s: mem header wrong %x,%x,%x,%x ... %s\n", __func__, 
	//		mem_header[0],mem_header[1],mem_header[2],mem_header[3], mem_header);
	//	return 1;
	//}
	
	UINT32 *index_table;
	index_table = (UINT32 *)(pmem_bin + param_offset_index);
	LOGV("index_table=%x, %x, %x, %x\n", 
		   index_table[0], index_table[1], index_table[2], index_table[3]);
	
	UINT32 *pdata_size = NULL;
	UINT32 read_size = 1;
	if ( param_size_index==-1 ){
	}else{
		
		pdata_size = (UINT32 *)(pmem_bin + param_size_index + index_table[param_index*2]);
		read_size = *pdata_size;
	}
	UINT32 *pdata;
	pdata = (UINT32 *)(pmem_bin + param_position + index_table[param_index*2]);
	LOGV("pmem_bin=%lx, index_table[1+param_index*2=%d]=%d, param_position=%d\n",
		   (long unsigned int)pmem_bin, 1+param_index*2, index_table[1+param_index*2], param_position);
	
	LOGV("param_mem=%x, param_index=%d, param_position=%d, pmem_bin=%lx, pdata=%lx\n",
		   param_mem, param_index, param_position, (long unsigned int)pmem_bin, (long unsigned int)pdata);
	if ( param_write==0 ){
		if ( read_size==1 && param_size_mag==1 ){
			int value;
			value = *pdata;
			if ( param_format==64 ){
				printf("%f",(float)value/(float)0x10000);
			}else{
				printf("%d",value);
			}
	
		}else{
			if (param_format == 1) {
				UINT8 *pdata1;
				char *p;
				int index = 0;

				pdata1 = (UINT8*)pdata;

				// get first addr
				p = strtok(argv[0],"\n");

				// Treat short register array
				while (p != NULL) {
					// get next token
					p = strtok(NULL, "\n");
					
					if (!(p != NULL && p[0] != ']' && index < param_size_mag)) {
						break;
					} else {
						// Regist byte (by order to little endian)
						LOGI("i=%d, %s\n", index, p);
						pdata1[index++] = (UINT8)(atoi(p) & 0xFF);
					}
				}
			}
			else if ( param_format==2 ){
				int i, n;
				UCHAR *pdata2;
				pdata2 = (UCHAR*)pdata;
				
				n = read_size * param_size_mag;
				printf("[");
				for ( i=0 ; i<n/2 ; i++ ){
					if ( i==n/2-1 ){
						printf("%d,%d", (pdata2[i]>>4)&0x0F,(pdata2[i]&0x0F));
					}else{
						printf("%d,%d,",(pdata2[i]>>4)&0x0F,(pdata2[i]&0x0F));
	
					}
				}
				printf("]");
				printf("\n");
			}
			else {
				int i = 0;

				// Stdout the first contents
				printf("[%d", pdata[0]);
					
				// Stdout the contents
				for (i = 1; i < read_size * param_size_mag; i++) {
					printf (",%d", pdata[i]);
				}

				// Stdout the last token
				printf("]\n");
			}
		}
	}else{
		if ( read_size==1 && param_size_mag==1 ){
			int value;
			if ( param_format==64 ){
				if ( argv[0][0]=='\"' ){
					value = (int)(atof(argv[0]+1)*((float)0x10000));
				}else{
					value = (int)(atof(argv[0])*((float)0x10000));
				}
			}else{
				if ( argv[0][0]=='\"' ){
					value = atoi(argv[0]+1);
				}else{
					value = atoi(argv[0]);
				}
			}
			LOGI("%d",value);
			*pdata = value;
		}else{
			if (param_format == 1) {
				UINT8 *pdata1;
				char *p;
				int index = 0;
				
				pdata1 = (UINT8*)pdata;

				// get first addr
				p = strtok(argv[0], "\n");

				// Treat integer register array
				while (p != NULL) {
					// get next token
					p = strtok(NULL, "\n");
					
					if (!(p != NULL && p[0] != ']')) {
						break;
					} else {
						// Regist byte (by order to little endian)
						LOGI("i=%d, %s\n", index, p);
						pdata1[index++] = (UINT8)(atoi(p) & 0xFF);
					}
				}
			}
			else if ( param_format==2 ){
			    char *p;
				int i=0, n=0;
				int array[32*32];
				UCHAR *pdata2;
				pdata2 = (UCHAR*)pdata;
			
			    p = strtok(argv[0],"\n");
			    //1つ目のトークン表示
			    //トークンがNULLになるまでループ
			    while(p!=NULL){
					//1
			        p = strtok(NULL,"\n");
			        if(p!=NULL){
						if ( p[0]==']' ){
							break;
						}else{
							array[i] = atoi(p);
						}
			        }else{
						break;
					}
					printf("i=%d, %s\n",i,p);
					i++;
					if ( i>=32*32 ){
						break;
					}
			    }
				n = i;
			
				if ( pdata_size ){
					pdata_size[0] = n / param_size_mag;
				}
				for ( int index=0 ; index<n/2 ; index++ ){
					pdata2[index] = (UCHAR)(((array[index*2+0]<<4)&0xF0) | (array[index*2+1]&0x0F));
				}
			}
			else {
				char *p;
				int index = 0;
				
				// get first addr
				p = strtok(argv[0], "\n");

				// Treat integer register array
				while (p != NULL) {
					// get next token
					p = strtok(NULL, "\n");
					
					if (!(p != NULL && p[0] != ']')) {
						break;
					} else {
						// Regist byte (by order to little endian)
						LOGI("i=%d, %s\n", index, p);
						pdata[index++] = (UINT32)(atoi(p) & 0xFFFFFFFF);
					}
				}
				
				pdata_size[0] = index / param_size_mag;
			}
		}

	}
	tool_terminate(pmem_bin,param_mem_size);
	
	return 0;
}

