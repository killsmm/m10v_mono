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
	char param_header[32];
	int param_write = 0;
	
	// read options
	while((result=getopt(argc,argv,"wrh:m:z:o:t:i:p:f:"))!=-1){
		switch(result){
			case 'a':
			case 'b':
			case 'c':
			break;

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

			case 'f':
			param_format = atoi(optarg);
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
		LOGE("error param_write=%d, but have arg %s\n",param_write,argv[0]);
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
	
	LOGV("pmem_bin=%x, %x, %x, %x\n", 
		   pmem_bin[0], pmem_bin[1], pmem_bin[2], pmem_bin[3]);
	
	UINT32 *index_table;
	index_table = (UINT32 *)(pmem_bin + param_offset_index);
	LOGV("index_table=%x, %x, %x, %x\n", 
		   index_table[0], index_table[1], index_table[2], index_table[3]);
	
	UINT32 *pdata;
	pdata = (UINT32 *)(pmem_bin + param_position + index_table[param_index*2]);
	LOGV("pmem_bin=%lx, index_table[1+param_index*2=%d]=%d, param_position=%d\n",
		   (long unsigned int)pmem_bin, 1+param_index*2, index_table[1+param_index*2], param_position);
	
	LOGV("param_index=%d, param_position=%d, pmem_bin=%lx, pdata=%lx\n",
		   param_index, param_position, (long unsigned int)pmem_bin, (long unsigned int)pdata);
	if ( param_write==0 ){
		if ( param_index==0 ){
			int value;
			value = *pdata;
			if ( param_format==1 ){
				printf("%f",(float)value/(float)0x10000);
			}else{
				printf("%d",value);
			}
	
		}else if ( param_index==1 ){
			int i=0;
			int n;
			struct {
				float board;
				int c_temp;
				float lo_bv_r_gain;
				float lo_bv_b_gain;
				float hi_bv_r_gain;
				float hi_bv_b_gain;
			} array[16];
			
			n = pdata[1]; 
			
			for ( int index=0 ; index<n ; index++ ){
				array[index].board  = (float)pdata[6+6*index+0]/(float)0x10000;
				array[index].c_temp = pdata[6+6*index+1];
				array[index].lo_bv_r_gain = (float)pdata[6+6*index+2]/(float)0x10000;
				array[index].lo_bv_b_gain = (float)pdata[6+6*index+3]/(float)0x10000;
				array[index].hi_bv_r_gain = (float)pdata[6+6*index+4]/(float)0x10000;
				array[index].hi_bv_b_gain = (float)pdata[6+6*index+5]/(float)0x10000;
			}
	
			printf("[");
			for ( i=0 ; i<n ; i++ ){
				printf("[%d,%f,%f,%f,%f]",
					   array[i].c_temp,
					   array[i].lo_bv_r_gain, array[i].lo_bv_b_gain,
					   array[i].hi_bv_r_gain, array[i].hi_bv_b_gain);
				if ( i==n-1 ){
				}else{
					printf(",");
				}
			}
			printf("]");
			
		}else if ( param_index==2 ){
			int i=0;
			int n;
			struct {
				float cur_x;
				float cur_y;
			} array[11];
			
			n = pdata[0];
			
			for ( int index=0 ; index<n ; index++ ){
				array[index].cur_x = (float)pdata[4+4*index+0]/(float)0x10000;
				array[index].cur_y = (float)pdata[4+4*index+1]/(float)0x10000;
			}
			
			printf("[");
			for ( i=0 ; i<n ; i++ ){
				printf("[%f,%f]",
					   array[i].cur_x, array[i].cur_y);
				if ( i==n-1 ){
				}else{
					printf(",");
				}
			}
			printf("]");
			
			
		}
	}else{

		if ( param_index==0 ){
			int value;
			if ( param_format==1 ){
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
			LOGI("awb_speed=%d\n",value);
			*pdata = value;
		}else if ( param_index==1 ){
			int i=0;
			int n;
			struct {
				float board;
				int c_temp;
				float lo_bv_r_gain;
				float lo_bv_b_gain;
				float hi_bv_r_gain;
				float hi_bv_b_gain;
			} array[16];
			
		    char *p;
		    p = strtok(argv[0],"\n");
		    //1�ڂ̃g�[�N���\��
		    //�g�[�N����NULL�ɂȂ�܂Ń��[�v
		    while(p!=NULL){
				//0
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//1
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					int board;
					board = atoi(p);
					array[i].board = 10000.0/(float)board;
					array[i].c_temp = board;
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//2
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					array[i].lo_bv_r_gain = atof(p);
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//3
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					array[i].lo_bv_b_gain = atof(p);
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//4
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					array[i].hi_bv_r_gain = atof(p);
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//5
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					array[i].hi_bv_b_gain = atof(p);
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//6
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
		        }else{
					break;
				}
				i++;
				if ( i>=16 ){
					break;
				}
		    }
			n = i;
			
			pdata[1] = n;
			for ( int index=0 ; index<n ; index++ ){
				pdata[6+6*index+0] = (UINT32)(array[index].board*0x10000);
				pdata[6+6*index+1] = array[index].c_temp;
				pdata[6+6*index+2] = (UINT32)(array[index].lo_bv_r_gain*0x10000);
				pdata[6+6*index+3] = (UINT32)(array[index].lo_bv_b_gain*0x10000);
				pdata[6+6*index+4] = (UINT32)(array[index].hi_bv_r_gain*0x10000);
				pdata[6+6*index+5] = (UINT32)(array[index].hi_bv_b_gain*0x10000);
			}
			
		}else if ( param_index==2 ){
			int i=0;
			int n;
			struct {
				float cur_x;
				float cur_y;
			} array[11];
			
		    char *p;
		    p = strtok(argv[0],"\n");
		    //1�ڂ̃g�[�N���\��
		    //�g�[�N����NULL�ɂȂ�܂Ń��[�v
		    while(p!=NULL){
				//0
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//2
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					array[i].cur_x = atof(p);
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//3
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
					array[i].cur_y = atof(p);
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				//4
		        p = strtok(NULL,"\n");
		        if(p!=NULL){
		        }else{
					break;
				}
				printf("i=%d, %s\n",i,p);
				i++;
				if ( i>=16 ){
					break;
				}
		    }
			n = i;
			
			pdata[0] = n;
			for ( int index=0 ; index<n ; index++ ){
				pdata[4+4*index+0] = (UINT32)(array[index].cur_x*0x10000);
				pdata[4+4*index+1] = (UINT32)(array[index].cur_y*0x10000);
			}
			
			
		}
	
	}

	tool_terminate(pmem_bin,param_mem_size);
	
	return 0;
}

