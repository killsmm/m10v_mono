#ifndef _VDI_USERFUNC_H_
#define _VDI_USERFUNC_H_

extern void  vdi_user_lock( void );
extern void  vdi_user_unlock( void );
extern void  vdi_user_vmem_lock( void );
extern void  vdi_user_vmem_unlock( void );
extern void  vdi_user_irq_flag_clear( void );
extern void  vdi_user_irq_flag_set( int interrupt_reason );
extern void  vdi_user_wait_flag( int *interrupt_reason, int timeout );

extern void  vdi_user_clean_cache( unsigned int dst, unsigned int count );
extern void  vdi_user_logprintf( char *fmt, ... );
extern void *vdi_user_malloc( int size );
extern void  vdi_user_free( void *p);

// vpu dram memory
void vdi_set_vpu_dram_ch0_physical_base( int addr );
void vdi_set_vpu_dram_ch0_size( int size );
void vdi_set_vpu_dram_ch1_physical_base( int addr );
void vdi_set_vpu_dram_ch1_size( int size );

// irq hander
void vpu_irq_handler( void );

#endif //#ifndef _VDI_USERFUNC_H_
