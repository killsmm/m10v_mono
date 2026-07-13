/**
 * ogma_basic_access.c
 *
 *     Copyright (c) 2015 Socionext Inc.
 *     All rights reserved.
 *
 */

#include "ogma_config.h"
#include "ogma_internal.h"
#include "ogma_core.h"
#include "ogma_basic_access.h"
#include "ogma_misc_internal.h"
#include "ogma_reg.h"

#undef STATIC
#ifdef MODULE_TEST
#define STATIC
#else
#define STATIC static
#endif

STATIC const ogma_uint32 desc_ring_irq_status_reg_addr[OGMA_DESC_RING_ID_MAX + 1] = {
	0,
	0,
	0,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_STATUS,
	0,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_STATUS,
	0,
	0,
	0,
	0,
	0,
	0,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_STATUS,
	0,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_STATUS
};

STATIC const ogma_uint32 channel_config_reg_addr[OGMA_DESC_RING_ID_MAX + 1] = {
	0,
	0,
	0,
	(ogma_uint32)OGMA_REG_ADDR_LPBE_RX_CONFIG,
	0,
	(ogma_uint32)OGMA_REG_ADDR_LPBR_RX_CONFIG,
	0,
	0,
	0,
	0,
	0,
	0,
	(ogma_uint32)OGMA_REG_ADDR_VSUB_TX_CONFIG,
	0,
	(ogma_uint32)OGMA_REG_ADDR_TS_TX_CONFIG
};

ogma_global_t ogma_global;

/* Internal function definition*/

STATIC void ogma_global_init ( void);

STATIC ogma_err_t ogma_probe_hardware (
    void *base_addr
    );

STATIC void ogma_reset_hardware (
    ogma_ctrl_t *ctrl_p
    );

STATIC void ogma_set_microcode(
    ogma_ctrl_t *ctrl_p
    );

STATIC ogma_uint32 ogma_calc_pkt_ctrl_reg_param (
    const ogma_pkt_ctrl_param_t *pkt_ctrl_param_p
    );

STATIC void ogma_internal_terminate (
    ogma_ctrl_t *ctrl_p
    );

/* Internal Function */

STATIC void ogma_global_init ( void)
{
    ogma_global.valid_flag = OGMA_TRUE;
}

STATIC ogma_err_t ogma_probe_hardware (
    void *base_addr
    )
{
    ogma_uint32 value;
    void *addr;

    addr = (void *) (( pfdep_cpu_addr_t)base_addr + OGMA_REG_ADDR_OGMA_VER);

    /* Read Hardware Version Register */
    value = pfdep_iomem_read( addr);

    if ( value != OGMA_HARDWARE_VER) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "Unexpected initial Hardware version value - 0x%08x\n",
                     value);
        return OGMA_ERR_NOTAVAIL;
    }

    return OGMA_ERR_OK;
}

STATIC void ogma_reset_hardware (
    ogma_ctrl_t *ctrl_p
    )
{

    if (ctrl_p->core_enabled_flag) {
		
		// (1)
        ogma_write_reg( ctrl_p,
                        OGMA_REG_ADDR_DMA_HM_CTRL,
                        OGMA_DMA_CTRL_REG_SW_RST_CM);

        ogma_write_reg( ctrl_p,
                        OGMA_REG_ADDR_DMA_MH_CTRL,
                        OGMA_DMA_CTRL_REG_SW_RST_CM);

        while ( ( ogma_read_reg( ctrl_p, OGMA_REG_ADDR_DMA_HM_CTRL)
                  & OGMA_DMA_CTRL_REG_SW_RST_CM) != 0) {
            ;
        }

        while ( ( ogma_read_reg( ctrl_p, OGMA_REG_ADDR_DMA_MH_CTRL)
                  & OGMA_DMA_CTRL_REG_SW_RST_CM) != 0) {
            ;
        }
		
		// (4)
		ogma_write_reg( ctrl_p,
						OGMA_REG_ADDR_SOFT_RST,
						OGMA_SOFT_RST_REG_RESET);
		// (5)
		ogma_write_reg( ctrl_p,
						OGMA_REG_ADDR_SOFT_RST,
						OGMA_SOFT_RST_REG_RUN);
    }

	// (6)
	ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_COM_INIT,
                    OGMA_COM_INIT_REG_DB);

	while ( ( ogma_read_reg( ctrl_p, OGMA_REG_ADDR_COM_INIT)
			  & OGMA_COM_INIT_REG_DB) != 0) {
		;
	}

}

STATIC void ogma_set_microcode (
    ogma_ctrl_t *ctrl_p
    )
{
    ogma_uint i;
    ogma_uint32 value;

    if (! ctrl_p->core_enabled_flag) {

		// (1)
		// (2)
		/* Loads Microcode to MicroEngine. */
		for( i = 0; i < ( sizeof( dmac_hm_cmd_data) >> 2); i++) {
			ogma_write_reg( ctrl_p,
							OGMA_REG_ADDR_DMAC_HM_CMD_BUF,
							dmac_hm_cmd_data[i] );
		}

		pfdep_memcpy( ctrl_p->dmac_mh_mcr_data_cpu_addr,
					  dmac_mh_cmd_data,
					  sizeof( dmac_mh_cmd_data) );

		pfdep_memcpy( ctrl_p->pktc_mcr_data_cpu_addr,
					  core_cmd_data,
					  sizeof( core_cmd_data) );

		pfdep_clean_invalidate_dcache_all();		
		
		// (3)
		ogma_write_reg( ctrl_p,
						OGMA_REG_ADDR_DMA_MH_MICRO_CODE_ADDR,
						( ogma_uint32)ctrl_p->dmac_mh_mcr_data_phys_addr);

		// (4)
		ogma_write_reg( ctrl_p,
						OGMA_REG_ADDR_DMA_MH_MICRO_CODE_SIZE,
						( ogma_uint32) ( sizeof( dmac_mh_cmd_data) >> 2) );

		// (5)
		ogma_write_reg( ctrl_p,
						OGMA_REG_ADDR_PKTC_MICRO_CODE_ADDR,
						( ogma_uint32) ctrl_p->pktc_mcr_data_phys_addr);

		// (6)
		ogma_write_reg( ctrl_p,
						OGMA_REG_ADDR_PKTC_MICRO_CODE_SIZE,
						( ogma_uint32) ( sizeof( core_cmd_data) >> 2) );

	}

	// (7)
    /* start microengines */
    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_DIS_CORE, 0);

    if (! ctrl_p->core_enabled_flag) {

		// (8)
		/* Polling for microengines (1sec) */
		for(i=0; i<1000; i++) {
			value = ogma_read_reg( ctrl_p, OGMA_REG_ADDR_TOP_STATUS);
			if((value & OGMA_TOP_IRQ_REG_ME_START) != 0) {
				break;
			} else {
				pfdep_msleep(1);
			}
		}

		if ( ( value & OGMA_TOP_IRQ_REG_ME_START) == 0) {
			printf("An error occurred at ogma_init.\n"
				   "Microcode loading Failed.\n");
		}

		//(9)
		// Clear IRQ TOP STATUS
		ogma_write_reg(ctrl_p,
					   OGMA_REG_ADDR_TOP_STATUS,
					   OGMA_TOP_IRQ_REG_ME_START);
	}
}


STATIC ogma_uint32 ogma_calc_pkt_ctrl_reg_param (
    const ogma_pkt_ctrl_param_t *pkt_ctrl_param_p
    )
{
    ogma_uint32 param = 0;

    if ( pkt_ctrl_param_p->ets) {
        param |= OGMA_PKT_CTRL_REG_ETS;
    }

    if ( pkt_ctrl_param_p->vjs) {
        param |= OGMA_PKT_CTRL_REG_VJS;
    }

    if ( pkt_ctrl_param_p->en_jumbo) {
        param |= OGMA_PKT_CTRL_REG_EN_JUMBO;
    }

    if ( pkt_ctrl_param_p->en_id_rep) {
        param |= OGMA_PKT_CTRL_REG_EN_ID_REP;
    }

    if ( pkt_ctrl_param_p->log_chksum_er_flag) {
        param |= OGMA_PKT_CTRL_REG_LOG_CHKSUM_ER;
    }

    if ( pkt_ctrl_param_p->log_hd_imcomplete_flag) {
        param |= OGMA_PKT_CTRL_REG_LOG_HD_INCOMPLETE;
    }

    if ( pkt_ctrl_param_p->log_hd_er_flag) {
        param |= OGMA_PKT_CTRL_REG_LOG_HD_ER;
    }

    return param;
}

STATIC ogma_uint32 ogma_calc_strm_ctrl_reg_param (
    const ogma_pkt_ctrl_param_t *pkt_ctrl_param_p
    )
{
    ogma_uint32 param = 0;

    if ( pkt_ctrl_param_p->tts) {
        param |= OGMA_STRM_CTRL_REG_TTS;
    }

    if ( pkt_ctrl_param_p->rfm) {
        param |= OGMA_STRM_CTRL_REG_RFM;
    }

    if ( pkt_ctrl_param_p->jrm) {
        param |= OGMA_STRM_CTRL_REG_JRM;
    }
	
	if ( pkt_ctrl_param_p->ppm) {
        param |= OGMA_STRM_CTRL_REG_PPM;
    }

    return param;
}



STATIC void ogma_set_channel_config (
    ogma_ctrl_t *ctrl_p,
    const ogma_desc_ring_param_t *desc_ring_param_p
    )
{
	ogma_uint16 ring_id;
	ogma_uint32 value = 0;

	for(ring_id=0;ring_id<=OGMA_DESC_RING_ID_MAX; ring_id++) {
		if(channel_config_reg_addr[ring_id] != 0) {
			// Endian
			if(desc_ring_param_p[ring_id].little_endian == OGMA_TRUE) {
				value |= 0x1;
			} 

			// Timer mode
			if(desc_ring_param_p[ring_id].tmr_mode == OGMA_TRUE) {
				value |= 0x10;
			} 

			// Update bit
			value |= 0x80000000;

			ogma_write_reg(ctrl_p,
						   channel_config_reg_addr[ring_id],
						   value);
		}
	}
}

/* Global Function */
ogma_err_t ogma_init(
    void *base_addr,
    pfdep_dev_handle_t dev_handle,
    const ogma_param_t *param_p,
    ogma_handle_t *ogma_handle_p
    )
{
    ogma_int i;
    ogma_uint32 value;
    ogma_err_t ogma_err;
    ogma_ctrl_t *ctrl_p = NULL;

	pfdep_err_t pfdep_err;
	
    if ( ( param_p == NULL) ||
         ( ogma_handle_p == NULL) ) {
        return OGMA_ERR_PARAM;
    }

	if(( !param_p->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].valid_flag) &&
	   ( !param_p->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].valid_flag) &&
	   ( !param_p->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].valid_flag) &&
	   ( !param_p->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].valid_flag)) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_init.\n"
                     "Please set valid at least one valid_flag.\n");
        return OGMA_ERR_DATA;
    }
	
    ogma_err = ogma_probe_hardware( base_addr);

    if ( ogma_err != OGMA_ERR_OK) {
        return ogma_err;
    }

    if ( !ogma_global.valid_flag) {
        ogma_global_init();
    }

    if ( ( ctrl_p = pfdep_malloc( sizeof( ogma_ctrl_t) ) ) == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_init.\n"
                     "Failed to ogma_handle memory allocation.\n");
        return OGMA_ERR_ALLOC;
    }

    ogma_global.list_head_p = ctrl_p;

    pfdep_memset( ctrl_p, 0, sizeof( ogma_ctrl_t) );

    ctrl_p->base_addr = base_addr;

    ctrl_p->dev_handle = dev_handle;

    pfdep_memcpy( ( void *)&ctrl_p->param,
                  ( void *)param_p,
                  sizeof( ogma_param_t) );

	/* Allocate Dummy Desc Entry's Memory */
    pfdep_err = pfdep_dma_malloc( dev_handle,
                                  OGMA_DUMMY_DESC_ENTRY_LEN,
                                  &ctrl_p->dummy_desc_entry_addr,
                                  &ctrl_p->dummy_desc_entry_phys_addr);
		
    if ( pfdep_err != PFDEP_ERR_OK) {
        ogma_err = OGMA_ERR_ALLOC;
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_init.\n"
                     "Failed to dummy_desc_entry's memory allocation.\n");
        goto err;
    }

    /* clear dummy desc entry */
    pfdep_memset( ctrl_p->dummy_desc_entry_addr,
                  0,
                  OGMA_DUMMY_DESC_ENTRY_LEN);

	ctrl_p->dmac_mh_mcr_data_cpu_addr  = pfdep_malloc(sizeof( dmac_mh_cmd_data));
	ctrl_p->dmac_mh_mcr_data_phys_addr = (pfdep_phys_addr_t) ctrl_p->dmac_mh_mcr_data_cpu_addr;

	ctrl_p->pktc_mcr_data_cpu_addr  = pfdep_malloc(sizeof( core_cmd_data));
	ctrl_p->pktc_mcr_data_phys_addr = (pfdep_phys_addr_t) ctrl_p->pktc_mcr_data_cpu_addr;
	
	// (1)
    /* Initialize Hardware */
    ogma_reset_hardware( ctrl_p);


	// (2)
	 /* Count Audio Divider Setting */
    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_CNT_ADO_DIV,
                    0x80000bff);

	 /* Count Enable Setting */
    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_CNT_EN,
                    OGMA_CNT_EN_CNT90K_EN|OGMA_CNT_EN_CNT_ADO_EN|OGMA_CNT_EN_CNT27M_EN);

    /* set PKT_CTRL */
    value = ogma_calc_pkt_ctrl_reg_param( &param_p->pkt_ctrl_param);

    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_PKT_CTRL,
                    value);

    /* set STRM_CTRL */
    value = ogma_calc_strm_ctrl_reg_param( &param_p->pkt_ctrl_param);

    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_STRM_CTRL,
                    value);

	/* Set channel config */
	ogma_set_channel_config(ctrl_p, 
							param_p->desc_ring_param);
	
    /* alloc desc_ring & set desc_start address*/
    for( i = 0; i <= OGMA_DESC_RING_ID_MAX; i++) {
        ogma_err = ogma_alloc_desc_ring( ctrl_p, (ogma_desc_ring_id_t)i);
        if ( ogma_err != OGMA_ERR_OK) {
            pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                         "An error occurred at ogma_init.\n"
                         "Failed to ring id NO.%d memory allocation.\n",
                         i);
            goto err;
        }
    }

	// (3)
    /* set microengines */
	ogma_set_microcode(ctrl_p);

    ctrl_p->core_enabled_flag = OGMA_TRUE;

    ctrl_p->next_p = ogma_global.list_head_p;

    ++ogma_global.list_entry_num;

    *ogma_handle_p = ctrl_p;

    /* Print hardware version information. */
    pfdep_print( PFDEP_DEBUG_LEVEL_NOTICE,
                 "hardware version: %08x\n",
                 ogma_read_reg(ctrl_p, OGMA_REG_ADDR_OGMA_VER));

    return OGMA_ERR_OK;

err:
	ogma_internal_terminate( ctrl_p);

    pfdep_free( ctrl_p);

    return ogma_err;
}

ogma_err_t ogma_soft_init(
    void *base_addr,
    pfdep_dev_handle_t dev_handle,
    const ogma_param_t *param_p,
    ogma_handle_t *ogma_handle_p
    )
{
    ogma_int i;
    ogma_err_t ogma_err;
    ogma_ctrl_t *ctrl_p = NULL;

    if ( ( param_p == NULL) ||
         ( ogma_handle_p == NULL) ) {
        return OGMA_ERR_PARAM;
    }


	if(( !param_p->desc_ring_param[OGMA_DESC_RING_ID_LPBR_RX].valid_flag) &&
	   ( !param_p->desc_ring_param[OGMA_DESC_RING_ID_LPBE_RX].valid_flag) &&
	   ( !param_p->desc_ring_param[OGMA_DESC_RING_ID_TS_TX].valid_flag) &&
	   ( !param_p->desc_ring_param[OGMA_DESC_RING_ID_VSUB_TX].valid_flag)) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_init.\n"
                     "Please set valid at least one valid_flag.\n");
        return OGMA_ERR_DATA;
    }

    ogma_err = ogma_probe_hardware( base_addr);

    if ( ogma_err != OGMA_ERR_OK) {
        return ogma_err;
    }

    if ( !ogma_global.valid_flag) {
        ogma_global_init();
    }

    if ( ( ctrl_p = pfdep_malloc( sizeof( ogma_ctrl_t) ) ) == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_init.\n"
                     "Failed to ogma_handle memory allocation.\n");
        return OGMA_ERR_ALLOC;
    }

    ogma_global.list_head_p = ctrl_p;

    pfdep_memset( ctrl_p, 0, sizeof( ogma_ctrl_t) );

    ctrl_p->base_addr = base_addr;

    ctrl_p->dev_handle = dev_handle;

    pfdep_memcpy( ( void *)&ctrl_p->param,
                  ( void *)param_p,
                  sizeof( ogma_param_t) );


    /* clear dummy desc entry */
    pfdep_memset( ctrl_p->dummy_desc_entry_addr,
                  0,
                  OGMA_DUMMY_DESC_ENTRY_LEN);

	/* Set channel config */
	ogma_set_channel_config(ctrl_p, 
							param_p->desc_ring_param);
	
    /* alloc desc_ring & set desc_start address*/
    for( i = 0; i <= OGMA_DESC_RING_ID_MAX; i++) {
        ogma_err = ogma_alloc_desc_ring( ctrl_p, (ogma_desc_ring_id_t)i);
        if ( ogma_err != OGMA_ERR_OK) {
            pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                         "An error occurred at ogma_init.\n"
                         "Failed to ring id NO.%d memory allocation.\n",
                         i);
            goto err;
        }
    }

    ctrl_p->core_enabled_flag = OGMA_TRUE;

    ctrl_p->next_p = ogma_global.list_head_p;

    ++ogma_global.list_entry_num;

    *ogma_handle_p = ctrl_p;

    /* Print hardware version information. */
    pfdep_print( PFDEP_DEBUG_LEVEL_NOTICE,
                 "hardware version: %08x\n",
                 ogma_read_reg(ctrl_p, OGMA_REG_ADDR_OGMA_VER));

    return OGMA_ERR_OK;

err:
	ogma_internal_terminate( ctrl_p);

    pfdep_free( ctrl_p);

    return ogma_err;
}

STATIC void ogma_internal_terminate (
    ogma_ctrl_t *ctrl_p
    )
{
    ogma_int i;
    ogma_reset_hardware( ctrl_p);

    /* free desc_ring */
    for( i = 0; i <= OGMA_DESC_RING_ID_MAX; i++) {
        ogma_free_desc_ring( ctrl_p, &ctrl_p->desc_ring[i] );
    }
    if ( ctrl_p->dummy_desc_entry_addr != NULL) {
        pfdep_dma_free( ctrl_p->dev_handle,
                        OGMA_DUMMY_DESC_ENTRY_LEN,
                        ctrl_p->dummy_desc_entry_addr,
                        ctrl_p->dummy_desc_entry_phys_addr);
    }
}
 
void ogma_terminate (
    ogma_handle_t ogma_handle
    )
{

    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_ctrl_t *tmp_ctrl_p = NULL, *old_tmp_ctrl_p = NULL;

    if ( ( ctrl_p == NULL) ||
         ( ogma_global.list_entry_num == 0) ) {
       return;
    }

    pfdep_assert( ogma_global.list_head_p != NULL);
    tmp_ctrl_p = ogma_global.list_head_p;

    while(1) {
        if ( tmp_ctrl_p == NULL) {
            /* Could not found ctrl_p specified from the list */
            return;
        }
        if ( ctrl_p == tmp_ctrl_p) {
            if ( old_tmp_ctrl_p != NULL) {
                old_tmp_ctrl_p->next_p = ctrl_p->next_p;
            } else {
                ogma_global.list_head_p = ctrl_p->next_p;
            }
            break;
        }
        old_tmp_ctrl_p = tmp_ctrl_p;
        tmp_ctrl_p = tmp_ctrl_p->next_p;
    }

    ogma_internal_terminate( ctrl_p);

    --ogma_global.list_entry_num;

    pfdep_free( ctrl_p);

}

ogma_err_t ogma_enable_top_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        return OGMA_ERR_PARAM;
    }

    value = irq_factor;

    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_TOP_INTEN_SET, value);

    return OGMA_ERR_OK;
}

ogma_err_t ogma_disable_top_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    )
{

    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        return OGMA_ERR_PARAM;
    }

    /* Write TOP_ITNEN REG*/
    value = irq_factor;
    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_TOP_INTEN_CLR, value);

    return OGMA_ERR_OK;
}

ogma_err_t ogma_enable_desc_ring_irq (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_uint32 irq_factor
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;
    ogma_desc_ring_t *desc_ring_p;    

    pfdep_soft_lock_ctx_t soft_lock_ctx;

    if ( ctrl_p == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_enable_desc_ring_irq.\n"
                     "Please set valid ogma_handle.\n");
        return OGMA_ERR_PARAM;
    }

    if ( ring_id > OGMA_DESC_RING_ID_MAX) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_enable_desc_ring_irq.\n"
                     "Please select ring id number between 0 and %d.\n",
                     OGMA_DESC_RING_ID_MAX);
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_enable_desc_ring_irq.\n"
                     "Please select valid desc ring.\n");
        return OGMA_ERR_NOTAVAIL;
    }

    desc_ring_p = &ctrl_p->desc_ring[ring_id];

    if ( !desc_ring_p->running_flag) {
        pfdep_release_soft_lock( &desc_ring_p->soft_lock,
                                 &soft_lock_ctx);
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_enable_desc_ring_irq.\n"
                     "Please select running desc ring.\n");
        return OGMA_ERR_NOTAVAIL;
    }

    value = irq_factor;

    ogma_write_reg( ctrl_p, desc_ring_irq_inten_set_reg_addr[ring_id], value);
    return OGMA_ERR_OK;
}

ogma_err_t ogma_disable_desc_ring_irq (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_uint32 irq_factor
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_disable_desc_ring_irq.\n"
                     "Please set valid ogma_handle.\n");
        return OGMA_ERR_PARAM;
    }

    if ( ring_id > OGMA_DESC_RING_ID_MAX) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_disable_desc_ring_irq.\n"
                     "Please select ring id number between 0 and %d.\n",
                     OGMA_DESC_RING_ID_MAX);
        return OGMA_ERR_PARAM;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_disable_desc_ring_irq.\n"
                     "Please select valid desc ring.\n");
        return OGMA_ERR_NOTAVAIL;
    }

	value = irq_factor;

    ogma_write_reg( ctrl_p, desc_ring_irq_inten_clr_reg_addr[ring_id], value);

    return OGMA_ERR_OK;
}

ogma_err_t ogma_enable_pkt_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        return OGMA_ERR_PARAM;
    }

    value = irq_factor;

    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_PKT_INTEN_SET, value);

    return OGMA_ERR_OK;
}

ogma_err_t ogma_disable_pkt_irq (
    ogma_handle_t ogma_handle,
    ogma_uint32 irq_factor
    )
{

    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        return OGMA_ERR_PARAM;
    }

    value = irq_factor;
    ogma_write_reg( ctrl_p, OGMA_REG_ADDR_PKT_INTEN_CLR, value);

    return OGMA_ERR_OK;
}


ogma_uint32 ogma_get_top_irq_status (
    ogma_handle_t ogma_handle,
    ogma_bool mask_flag
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_top_irq_status.\n"
                     "Please set valid ogma_handle.\n");
        return 0;
    }

    value = ogma_read_reg( ctrl_p, OGMA_REG_ADDR_TOP_STATUS);

    /* Write clear irq top  status */
    ogma_write_reg( ctrl_p,
                    OGMA_REG_ADDR_TOP_STATUS,
                    ( value & OGMA_TOP_IRQ_REG_ME_START) );


    if ( mask_flag) {
        value &= ogma_read_reg( ctrl_p, OGMA_REG_ADDR_TOP_INTEN);
    }

    return value;
}

ogma_uint32 ogma_get_top_irq_status_non_clear (
    ogma_handle_t ogma_handle,
    ogma_bool mask_flag
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_top_irq_status.\n"
                     "Please set valid ogma_handle.\n");
        return 0;
    }

    value = ogma_read_reg( ctrl_p, OGMA_REG_ADDR_TOP_STATUS);

    if ( mask_flag) {
        value &= ogma_read_reg( ctrl_p, OGMA_REG_ADDR_TOP_INTEN);
    }

    return value;
}

ogma_uint32 ogma_get_desc_ring_irq_status (
    ogma_handle_t ogma_handle,
    ogma_desc_ring_id_t ring_id,
    ogma_bool mask_flag
    )
{
    ogma_uint32 value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_desc_ring_irq_status.\n"
                     "Please set valid ogma_handle.\n");
        return 0;
    }

    if ( ring_id > OGMA_DESC_RING_ID_MAX) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_desc_ring_irq_status.\n"
                     "Please select ring id number between 0 and %d.\n",
                     OGMA_DESC_RING_ID_MAX);
        return 0;
    }

    if ( !ctrl_p->desc_ring[ring_id].param.valid_flag) {
        pfdep_print( PFDEP_DEBUG_LEVEL_FATAL,
                     "An error occurred at ogma_get_desc_ring_irq_status.\n"
                     "Please select valid desc ring.\n");
        return 0;
    }

    value = ogma_read_reg( ctrl_p,
                           desc_ring_irq_status_reg_addr[ring_id]);

    ogma_write_reg( ctrl_p,
                    desc_ring_irq_status_reg_addr[ring_id],
                    ( value & ( OGMA_CH_IRQ_REG_EMPTY |
                                OGMA_CH_IRQ_REG_ERR) ) );

    if ( mask_flag) {
        value &= ogma_read_reg( ctrl_p,
                                desc_ring_irq_inten_reg_addr[ring_id]);
    }

    return value;
}

ogma_uint32 ogma_get_pkt_irq (
    ogma_handle_t ogma_handle
    )
{
    ogma_uint32   value;
    ogma_ctrl_t *ctrl_p = (ogma_ctrl_t *)ogma_handle;

    if ( ctrl_p == NULL) {
        return OGMA_ERR_PARAM;
    }

    value = ogma_read_reg( ctrl_p, OGMA_REG_ADDR_PKT_STATUS);
	ogma_write_reg( ctrl_p, OGMA_REG_ADDR_PKT_STATUS, value);	
	
    return value;
}
