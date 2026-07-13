#include <stdio.h>
#include "ogma_reg.h"
#include "ogma_internal.h"
#include "ogma_basic_access.h"
//#include "netsec_test.h"
#include "dd_gic.h"
#include "debug.h"

ogma_handle_t gBF_Netsec_cur_ogma_handle;

void dump_desc_ring_entry(
	ogma_desc_ring_id_t desc_id,
	ogma_uint32 start_idx,
	ogma_uint32 end_idx)
{
	ogma_uint32 i, j;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)gBF_Netsec_cur_ogma_handle;
	ogma_uint32 value;
	
	printf("Dumping Descriptor Entry");
	
	for(i=start_idx; i<end_idx+1; i++) {
		printf(" ----------- ring_id = %2d ---------\n", i);
		for(j=0;j<ctrl_p->desc_ring[desc_id].desc_entry_len/4;j++) {
			value = pfdep_iomem_read(ctrl_p->desc_ring[desc_id].desc_ring_cpu_addr);
			printf("desc_id[%-2d] = %08x\n", j, value);
		}
		printf(" ----------------------------------\n\n");
	}
}

int print_str(char *str) {
  int i=0;
  while(*(str+i) != (char)0) {
    printf("%c",*(str+i));
    i++;
  }
  return 0;
}

int print_udec(unsigned int num) {
  unsigned int rem;
  char value[64];
  int i=0;
  int j=0;

  if(num == 0) {
    value[0] = '0';
    i++;
  } else {
    while(num > 0) {
      rem=num%10;
      num=num/10;
      value[i]='0'+rem;
      i++;
    }
  }
  for(j=0;j<i;j++) {
    printf("%c",value[i-j-1]);	/* pgr0000 */
  }
  return 0;
}

int print_0Xudec(unsigned int num,
                 unsigned int X) {
  unsigned int rem;
  char value[64];
  int i=0;
  int j=0;

  if(num == 0) {
    value[0] = '0';
    i++;
  } else {
    while(num > 0) {
      rem=num%10;
      num=num/10;
      value[i]='0'+rem;
      i++;
    }
  }
  if(X>i) {
    for(j=0;j<X-i;j++) {
      printf("0");
    }
  }
  for(j=0;j<i;j++) {
    printf("%c",value[i-j-1]);	/* pgr0000 */
  }
  return 0;
}

int print_Xudec(unsigned int num,
                unsigned int X) {
  unsigned int rem;
  char value[64];
  int i=0;
  int j=0;

  if(num == 0) {
    value[0] = '0';
    i++;
  } else {
    while(num > 0) {
      rem=num%10;
      num=num/10;
      value[i]='0'+rem;
      i++;
    }
  }
  if(X>i) {
    for(j=0;j<X-i;j++) {
      printf(" ");
    }
  }
  for(j=0;j<i;j++) {
    printf("%c",value[i-j-1]);	/* pgr0000 */
  }
  return 0;
}

int print_dec(int num) {
  unsigned int rem;
  char value[64];
  int i=0;
  int j=0;
  int negative=0;
  
  if(num < 0) {
    num = num*(-1);
    negative=1;
  }

  if(num == 0) {
    value[0] = '0';
    i++;
  } else {
    while(num > 0) {
      rem=num%10;
      num=num/10;
      value[i]='0'+rem;
      i++;
    }
  }
  if(negative == 1){
    printf("-");
  }
  for(j=0;j<i;j++) {
    printf("%c",value[i-j-1]);	/* pgr0000 */
  }
  return 0;
}

int print_hex(unsigned int num) {
  int rem;
  char value[64];
  int i=0;
  int j=0;

  if(num == 0) {
    value[0] = '0';
    i++;
  } else {
    while(num > 0) {
      rem=num%16;
      num=num/16;
      if((rem >= 0) && (rem <= 9)) {
        value[i]='0'+rem;
      } else if((rem > 9) && (rem <= 15)) {
        value[i]='a'+rem-10;
      }
      i++;
    }
  }
  for(j=0;j<i;j++) {
    printf("%c",value[i-j-1]);	/* pgr0000 */
  }
  return 0;
}

int print_0Xhex(unsigned int num,
                unsigned int X) {
  int rem;
  char value[64];
  int i=0;
  int j=0;

  if(num == 0) {
    value[0] = '0';
    i++;
  } else {
    while(num > 0) {
      rem=num%16;
      num=num/16;
      if((rem >= 0) && (rem <= 9)) {
        value[i]='0'+rem;
      } else if((rem > 9) && (rem <= 15)) {
        value[i]='a'+rem-10;
      }
      i++;
    }
  }
  if(X>i) {
    for(j=0;j<X-i;j++) {
      printf("0");
    }
  }
  for(j=0;j<i;j++) {
    printf("%c",value[i-j-1]);	/* pgr0000 */
  }
  return 0;
}


void debug_write_reg(
    ogma_uint32 reg_addr,
	ogma_uint32 value
)
{
	if(gBF_Netsec_cur_ogma_handle != NULL) {
		ogma_write_reg(gBF_Netsec_cur_ogma_handle, 
					   reg_addr,
					   value);
	} else {
		printf("Error: debug_write_reg(). Not found Hardware.");
	}
}

ogma_uint32 debug_read_reg(
    ogma_uint32 reg_addr
)
{
	ogma_uint32 value = 0;
	
	if(gBF_Netsec_cur_ogma_handle != NULL) {
		value = ogma_read_reg(gBF_Netsec_cur_ogma_handle, 
							  reg_addr);
	} else {
		printf("Error: debug_read_reg(). Not found Hardware.");
	}
	
	return value;
}


void dump_sdb_item()
{
  ogma_uint8 id;
  ogma_uint8 item;
  unsigned int read_data;
  
  for(id=0;id<63;id++) {
      for(item=0;item<16;item++) {
		  read_data = ogma_get_session_db_item((ogma_ctrl_t *)gBF_Netsec_cur_ogma_handle,
											   id,
											   item,
											   OGMA_FALSE);
			  printf("id=%2d:  item=%2d : value=0x%08x\n",
					 id,item,read_data);
      }
  }
}


void debug_set_exs_top_reg()
{
/***
 * EXS TOP Register Setting at MILB
 */
    pfdep_iomem_write((void *)(0x1b110040), 0x7d7); // RAM PD Control Register
    pfdep_iomem_write((void *)(0x1b110070), 0x0);   // NETSEC PTP Clock Stop Control Register
    pfdep_iomem_write((void *)(0x1b11000c), 0x107); // Soft Reset Register
    pfdep_iomem_write((void *)(0x19000000), 0xff);  // EXS-NIC Remap
}

void debug_set_gic() 
{
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_NETSEC_A_INT,1, D_DD_GIC_PRI30, 0x1);
//	Dd_GIC_Ctrl(E_DD_GIC_INTID_NETSEC_B_INT,1, D_DD_GIC_PRI30, 0x1);
}


void debug_dis_uart_gic() {
	/* Dd_GIC_Ctrl(E_DD_GIC_INTID_UART_CSIO_CH0_SEND_INT,0, D_DD_GIC_PRI30, 0x2); */
    /* Dd_GIC_Ctrl(E_DD_GIC_INTID_UART_CSIO_CH0_RECEIVE_INT,0, D_DD_GIC_PRI30, 0x2); */
}

void debug_invalidate_dcache_addr(UINT32 address, UINT32 size) {
	fj_memory_dcache_addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_FLUSH, address, size );
}

void debug_clean_dcache_addr(UINT32 address, UINT32 size) {
	fj_memory_dcache_addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN, address, size );
}
