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
	int param_position = 0;
	int param_format = 0;
	int param_size_index = -1;
	int param_size_mag = 1;
	int param_size_array = -1;
	int param_body = -1;
	char param_header[32];
	int param_write = 0;
	
	// read options
	while((result=getopt(argc,argv,"rwh:m:o:t:i:p:f:s:n:a:z:b:"))!=-1){
		switch(result){

			case 'w':
			param_write = 1;
			break;
			case 'r':
			param_write = 0;
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
			param_index = atoi(optarg);
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
			
			case 'a':
			param_size_array = atoi(optarg);
			break;
			
			case 'b':
			param_body = atoi(optarg);
			break;
			
		}
	}
	argc -= optind;
	argv += optind;

	if ( param_write==1 && argv[0]==NULL ){
		LOGE("error param_write=%d, no arg\n",param_write);
		return 1;
	}
	if ( param_write==0 && argv[0]!=NULL ){
		LOGE("error param_write=%d, but have arg\n",param_write);
		return 1;
	}
	
	LOGV("mem=%x, index_offset=%d, index=%d, position=%d, arg = %s\n", 
		 param_mem, param_offset_index, param_index, param_position, argv[0]);
	
//	if ( param_mem<0 ){
//		LOGE("error param_mem=%x\n",param_mem);
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
	if ( param_size_array<0 ){
		LOGE("error param_array=%d\n",param_size_array);
		return 1;
	}

	char *pmem_bin;
	pmem_bin = (char*)tool_initialize(param_mem, param_mem_size);
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
	
	LOGV("pmem_bin=%x, %x, %x, %x\n", 
		   pmem_bin[0], pmem_bin[1], pmem_bin[2], pmem_bin[3]);
	
	UINT16 *index_table = NULL;
	if ( param_body==-1 ){
		index_table = (UINT16 *)(pmem_bin + param_offset_index);
		LOGV("index_table=%d, %d, %d, %d, %d\n", 
			   index_table[0], index_table[1], index_table[2], index_table[3], index_table[4]);
	}
	
	UINT32 *index_table_body = NULL;
	if ( param_body!=-1 ){
		index_table_body = (UINT32 *)(pmem_bin + param_offset_index);
		LOGV("index_table=%d, %d, %d, %d, %d\n", 
			   index_table[0], index_table[1], index_table[2], index_table[3], index_table[4]);
	}
	
	UINT32 read_size = 1;
	if ( param_size_index==-1 ){
	}else{
		UINT32 *pdata_size = NULL;
		pdata_size = (UINT32 *)(pmem_bin + param_size_index + index_table[param_index]);
		read_size = *pdata_size;
	}
	UCHAR *pdata;
	if ( param_body==-1 ){
		pdata = (UCHAR *)(pmem_bin + param_position + index_table[param_index]);
		LOGV("pmem_bin=%lx, index_table[param_index=%d]=%d, param_position=%d\n",
			   (long unsigned int)pmem_bin, param_index, index_table[param_index], param_position);
	}else{
		pdata = (UCHAR *)(pmem_bin + param_position + index_table_body[param_index]);
		LOGV("pmem_bin=%lx, index_table[param_index=%d]=%d, param_position=%d\n",
			   (long unsigned int)pmem_bin, param_index, index_table[param_index], param_position);
	}
	
	LOGV("param_mem=%x, param_index=%d, param_position=%d, pmem_bin=%lx, pdata=%lx, read_size=%lx, param_size_mag=%ld, param_size_array=%ld\n",
		   param_mem, param_index, param_position, (long unsigned int)pmem_bin, (long unsigned int)pdata, (long unsigned int)read_size, (long unsigned int)param_size_mag, (long unsigned int)param_size_array);
	if ( param_write==0 ){
		if ( read_size==1 && param_size_mag==1 ){
			int value;
			value = *pdata;
			if ( param_format==1 ){
				printf("%f",(float)value/(float)0x10000);
			}else if ( param_format==2 ){
				INT16 *pdata2;
				pdata2 = (UINT16*)pdata;
				short value;
				value = *pdata2;
				printf("%d",value);
			}else if ( param_format==4 ){
				UINT32 *pdata4;
				pdata4 = (UINT32*)pdata;
				int value;
				value = *pdata4;
				printf("%d",value);
			}else{
				printf("%d",value);
			}
	
		}else{
			if (param_format == 2) {
				INT16 *pdata2 = (INT16*)pdata;
				int i = 0;

				// Stdout the first contents
				printf("[%d", pdata2[0]);
				
				// Stdout the contents
				for (i = 1; i < read_size * param_size_mag; i++) {
					printf(",%d", pdata2[i]);
				}

				// Stdout the last token
				printf("]\n");
			} else if (param_format == 4) {
				UINT32 *pdata4 = (UINT32*)pdata;
				int i = 0;

				// Stdout the first contents
				printf("[%d", pdata4[0]);
				
				// Stdout the contents
				for (i = 1; i < read_size * param_size_mag; i++) {
					printf(",%d", pdata4[i]);
				}

				// Stdout the last token
				printf("]\n");
			} else {
				int array[64];
				int array0[64];
				int i;
				for ( i=0 ; i<read_size*param_size_mag ; i++ ){
					//printf("%d: %d\n",i, pdata[i]);
					array[i] = (int)pdata[i];
				}
				for ( i=0 ; i<read_size*param_size_mag ; i++ ){
					//printf("%d: %d\n",i+param_size_array, i*param_size_array);
					array0[i] = i*param_size_array;
				}
				
				int n;
				n = read_size * param_size_mag;
				printf("[");
				for ( i=0 ; i<n ; i++ ){
					if ( i==n-1 ){
						printf("%d",array[i]-array0[i]);
					}else{
						printf("%d,",array[i]-array0[i]);
					}

				}
				printf("]");
				printf("\n");
			}	
		}
	}else{

		if ( read_size==1 && param_size_mag==1 ){
			int value;
			if ( param_format==1 ){
				if ( argv[0][0]=='\"' ){
					value = (int)(atof(argv[0]+1)*((float)0x10000));
				}else{
					value = (int)(atof(argv[0])*((float)0x10000));
				}
				*pdata = (UCHAR)value;
			}else if ( param_format==2){
				UINT16 *pdata2;
				UINT16 value2 = 0;
				pdata2 = (UINT16*)pdata;

				// Treat short value
				value = ( argv[0][0]=='\"' )
					  ? (UINT16)((atoi(argv[0]+1)) & 0xFFFF)
					  : (UINT16)((atoi(argv[0])) & 0xFFFF);

				*pdata2 = (UINT16)value;
			}else if ( param_format==4 ){
				if ( argv[0][0]=='\"' ){
					value = (int)(atoi(argv[0]+1));
				}else{
					value = (int)(atoi(argv[0]));
				}
				UINT32 *pdata4;
				pdata4 = (UINT32*)pdata;
				*pdata4 = (UINT32)value;
			}else{
				if ( argv[0][0]=='\"' ){
					value = atoi(argv[0]+1);
				}else{
					value = atoi(argv[0]);
				}
				*pdata = (UCHAR)value;		
			}
			LOGI("%d",value);
		}else{
			if (param_format == 2) {
				UINT16 *pdata2;
				char *p;
				int index = 0;

				pdata2 = (UINT16*)pdata;

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
						pdata2[index++] = (UINT16)(atoi(p) & 0xFFFF);
					}
				}
			} else if (param_format == 4) { 
				UINT32 *pdata4;
				char *p;
				int index = 0;

				pdata4 = (UINT32*)pdata;

				// get first addr
				p = strtok(argv[0],"\n");

				// Treat long register array
				while (p != NULL) {
					// get next token
					p = strtok(NULL, "\n");
					
					if (!(p != NULL && p[0] != ']' && index < param_size_mag)) {
						break;
					} else {
						// Regist byte (by order to little endian)
						LOGI("i=%d, %s\n", index, p);
						pdata4[index++] = (UINT32)(atol(p));
					}
				}
			} else if (param_format == 8) { 
				UINT64 *pdata8;
				char *p;
				int index = 0;

				pdata8 = (UINT64*)pdata;

				// get first addr
				p = strtok(argv[0],"\n");

				// Treat long register array
				while (p != NULL) {
					// get next token
					p = strtok(NULL, "\n");
					
					if (!(p != NULL && p[0] != ']' && index < param_size_mag)) {
						break;
					} else {
						// Regist byte (by order to little endian)
						LOGI("i=%d, %s\n", index, p);
						pdata8[index++] = (UINT64)(atol(p));
					}
				}
			} else {
				int array[64];
				int array0[64];
				int i=0, n=0;
				for ( i=0 ; i<read_size*param_size_mag ; i++ ){
					printf("%d: %d\n",i, pdata[i]);
					array[i] = (int)pdata[i];
				}
				for ( i=0 ; i<read_size*param_size_mag ; i++ ){
					printf("%d: %d\n",i+param_size_array, pdata[i+param_size_array]);
					array0[i] = i*param_size_array;
				}
				
				i = 0;
				char *p;
				p = strtok(argv[0],"\n");
				//1�ڂ̃g�[�N���\��
				//�g�[�N����NULL�ɂȂ��܂Ń��[�v
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
					if ( i>=64 ){
						break;
					}
				}
				n = i;
				
				for ( int index=0 ; index<n ; index++ ){
					pdata[index] = (UCHAR)(array[index] + array0[index]);
					printf("i=%d, %d\n",index,pdata[index]);
				}
			}
		}
	}
	
	tool_terminate(pmem_bin,param_mem_size);

	return 0;
}

