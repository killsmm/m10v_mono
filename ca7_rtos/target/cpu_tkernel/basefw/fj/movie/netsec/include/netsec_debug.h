#ifndef TEST_DEBUG_H
#define TEST_DEBUG_H

#include "ogma_api.h"

#define D_MAX_KEY_LEN 64 
#define D_MAX_HASH_KEY_LEN 256

extern ogma_handle_t gBF_Netsec_cur_ogma_handle;

extern void dump_desc_ring_entry(
	ogma_desc_ring_id_t desc_id,
	ogma_uint32 start_idx,
	ogma_uint32 end_idx);

extern int print_str(char *str);
extern int print_udec(unsigned int num);
extern int print_0Xudec(unsigned int num,
						unsigned int X);
extern int print_Xudec(unsigned int num,
					   unsigned int X);
extern int print_dec(int num);
extern int print_hex(unsigned int num);
extern int print_0Xhex(unsigned int num,
					   unsigned int X);

extern void debug_write_reg(
    ogma_uint32 reg_addr,
	ogma_uint32 value
	);

extern ogma_uint32 debug_read_reg(
    ogma_uint32 reg_addr
	);

extern void dump_sdb_item(void);

extern void debug_set_exs_top_reg(void);

extern void debug_set_gic(void); 
extern void debug_dis_uart_gic(void);

#endif /* TEST_DEBUG_H */
