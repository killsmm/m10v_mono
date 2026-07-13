#include "pfdep.h"
#include "ogma_api.h"
#include <string.h>
#include <stdio.h>
#include "ddim_typedef.h"
#include "os_user_custom.h"

//#include "netsec_test.h"
//#include "formatter.h"
#include "fj_formatter.h"
#include "ns_lpbr_data.h"

#include "l1l2cache.h"
#include "sdram_map_movie_record.h"
#include "sdram_map_cache_define.h"


BOOL	gBF_Netsec_ast_int_test;
BOOL	gBF_Netsec_tx_done_test;
BOOL gBF_Netsec_eth_lpb_test;
BOOL gBF_Netsec_rtp_lpb_test;
BOOL gBF_Netsec_secure_flag;
unsigned int gBF_Netsec_mac_setting;
BOOL gBF_Netsec_ipv6_test;
BOOL gBF_Netsec_ts_lpbk_test;

BOOL gBF_Netsec_mac_irq_disable;
BOOL gBF_Netsec_jpeg_strm_test;
BOOL gBF_Netsec_lpbr_nocomp;

BOOL gBF_Netsec_large_test;


static __inline int char2hex(int c);
static int strconvert2bin(unsigned char *str,
                          unsigned int len,
                          unsigned char *output);

static __inline int char2hex(int c)
{
	int result;
	if ('0' <= c && c <= '9') {
		result = c - '0';
	} else if ('a' <= c && c <= 'f') {
		result = c - 'a' + 10;
	} else {
		result = c - 'A' + 10;
	}
	return result;
}

static int strconvert2bin(unsigned char *str,
                          unsigned int len,
                          unsigned char *output)
{
	int i;
	for(i=0;i<len;i++) {
		*(output+i) =
			(char2hex(*(str+(i<<1)))<<4)+char2hex(*(str+(i<<1)+1));
	}
	return i;
}

VOID BF_Netsec_test_func()
{
	T_FJ_FORMATTER_DATA in_data;
	T_FJ_FORMATTER_DATA out_data;
	ULONG out_len;
	ULONG exp_len;
	UINT32 lti;
	UINT32 recv_stid, recv_dtid;
	T_FJ_FORMATTER_PID pid_pg0;
	T_FJ_FORMATTER_PID pid_pg1;
	int data_idx = 0;
	int j;
	char *in_buf  = malloc(5000);
	char *out_buf = malloc(5000);
	char *exp_data = malloc(5000);
	
	pid_pg0.pid_pmt   = 2;
	pid_pg0.pid_pcr   = 3;
	pid_pg0.pid_video = 10;
	pid_pg0.pid_audio = 20;
	pid_pg0.pid_mtdat = 30;
	
	pid_pg1.pid_pmt   = 2;
	pid_pg1.pid_pcr   = 3;
	pid_pg1.pid_video = 10;
	pid_pg1.pid_audio = 20;
	pid_pg1.pid_mtdat = 30;
	
	FJ_Formatter_Convert_Initialize();
	
	FJ_Formatter_Convert_Set_Stream( FJ_FORMATTER_MPTS_STRM, 1480,  (T_FJ_FORMATTER_PID *)0 );
	
	FJ_Formatter_Convert_Set_Program( FJ_FORMATTER_MPTS_PG_0, 1480, &pid_pg0 );
	FJ_Formatter_Convert_Set_Program( FJ_FORMATTER_MPTS_PG_1, 1480, &pid_pg1 );
	
	out_data.addr = (UINT32) out_buf;
	out_data.size = 5000;
	
	while(pes_data[data_idx]) {
		in_data.size = (UINT32) strlen(pes_data[data_idx])/2;
		in_data.addr = (UINT32) in_buf;
		
		exp_len = strlen(lpbr_data[data_idx])/2;
		
		strconvert2bin((unsigned char *)pes_data[data_idx],
					   (unsigned int) in_data.size,
					   (unsigned char *)in_data.addr);
		
		strconvert2bin((unsigned char *)lpbr_data[data_idx],
					   (unsigned int)   exp_len,
					   (unsigned char *)exp_data);
		
		printf("id=%d :", data_idx);
		
		FJ_Formatter_Convert_PES_Send0(stid0[data_idx], dtid[data_idx], &in_data, 1000);
//		printf( "send0 : stid(%d), dtid(%d)\n", stid0[data_idx], dtid[data_idx] );
		
		FJ_Formatter_Convert_TS_Recv0(&out_data, &out_len, &recv_stid, &recv_dtid, &lti, 1000);
//		printf( "recv0 : out_lon(%d), recv_stid(%d), recv_dtid(%d) lti(%d)\n", out_len, recv_stid, recv_dtid, lti );
		
		if(out_len != exp_len) {
			printf("  ERROR : Received data length is mismatched.\n");
			printf("          Received data length = %d\n", (int)out_len);
			printf("          Expected data length = %d\n", (int)exp_len);
		} else {
			if((pfdep_memcmp((VOID *)(out_data.addr),
							 (VOID *)(exp_data),
							 exp_len) != 0)) {
				printf("-------------------------------------\n");
				printf("      output            expected\n");
				for(j=0;j<exp_len;j++) {
					if(*((UCHAR *)out_data.addr+j) !=
					   *((UCHAR *)exp_data+j)) {	/* pgr0520 */
						printf("[%4d] = %02x     ", j, ((UCHAR *)out_data.addr)[j]);
						printf("[%4d] = %02x	 ", j, ((UCHAR *)exp_data)[j]);
						printf("    NG !!\n");
					}
				}
				printf("-------------------------------------\n");
			} else {
				printf("OK\n");				
			}
		}
		data_idx++;
	}
	
	data_idx = 0;
	while(pes_data[data_idx]) {
		in_data.size = (UINT32) strlen(pes_data[data_idx])/2;
		in_data.addr = (UINT32) in_buf;
		
		exp_len = strlen(lpbr_data[data_idx])/2;
		
		strconvert2bin((unsigned char *)pes_data[data_idx],
					   (unsigned int) in_data.size,
					   (unsigned char *)in_data.addr);
		
		strconvert2bin((unsigned char *)lpbr_data[data_idx],
					   (unsigned int)   exp_len,
					   (unsigned char *)exp_data);
		
		printf("id=%d :", data_idx);
		
		FJ_Formatter_Convert_PES_Send1(stid1[data_idx], dtid[data_idx], &in_data, 1000);
//		printf( "send1 : stid(%d), dtid(%d)\n", stid1[data_idx], dtid[data_idx] );
		
		FJ_Formatter_Convert_TS_Recv1(&out_data, &out_len, &recv_stid, &recv_dtid, &lti, 1000);
//		printf( "recv1 : out_lon(%d), recv_stid(%d), recv_dtid(%d) lti(%d)\n", out_len, recv_stid, recv_dtid, lti );
		
		if(out_len != exp_len) {
			printf("  ERROR : Received data length is mismatched.\n");
			printf("          Received data length = %d\n", (int)out_len);
			printf("          Expected data length = %d\n", (int)exp_len);
		} else {
			if((pfdep_memcmp((VOID *)(out_data.addr),
							 (VOID *)(exp_data),
							 exp_len) != 0)) {
				printf("-------------------------------------\n");
				printf("      output            expected\n");
				for(j=0;j<exp_len;j++) {
					if(*((UCHAR *)out_data.addr+j) !=
					   *((UCHAR *)exp_data+j)) {	/* pgr0520 */
						printf("[%4d] = %02x     ", j, ((UCHAR *)out_data.addr)[j]);
						printf("[%4d] = %02x	 ", j, ((UCHAR *)exp_data)[j]);
						printf("    NG !!\n");
					}
				}
				printf("-------------------------------------\n");
			} else {
				printf("OK\n");				
			}
		}
		data_idx++;
	}
	
	FJ_Formatter_Convert_Finalize();
	
	free(in_buf);
	free(out_buf);
	free(exp_data);
	
}

