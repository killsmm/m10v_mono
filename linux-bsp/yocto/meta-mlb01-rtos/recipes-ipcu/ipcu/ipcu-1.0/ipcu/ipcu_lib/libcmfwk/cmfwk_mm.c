/******************************************************************************/
/**
 *  @file   cmfwk_mm.c
 *  @brief  Memory Control API
 */
/*
 *  Copyright 2015 Socionext Inc.
 ******************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "cmfwk_ipcu.h"
#include "cmfwk_mm.h"

/* Information for SDRAM MAP */
struct sn_platform_mmap {
	UINT32          vaddr;
	UINT32          map_vaddr;
	UINT32          map_size;
	UINT32          paddr;
	UINT32          size;
	const char      id;//for other func to get top address
	const char*     dev_addr;
	const char*     dev_size;
};
static unsigned long registMemMax;
#define MMAP_CFG_MAX IPCU_MM_ID_MAX
static struct sn_platform_mmap mmap_cfgs[MMAP_CFG_MAX] =
{
	{
		.id = IPCU_MM_BUFF, 
		.dev_addr = "/proc/device-tree/shared_commem/buf-addr",
		.dev_size = "/proc/device-tree/shared_commem/buf-size",
	},
	{
		.id = IPCU_MM_SYNC, 
		.dev_addr = "/proc/device-tree/shared_commem/sync-addr",
		.dev_size = "/proc/device-tree/shared_commem/sync-size",
	},
	{
		.id = IPCU_MM_RDPT, 
		.dev_addr = "/proc/device-tree/shared_commem/rd-pointer-addr",
		.dev_size = "/proc/device-tree/shared_commem/rd-pointer-size",
	},
	{
		.id = IPCU_MM_WTPT, 
		.dev_addr = "/proc/device-tree/shared_commem/wt-pointer-addr",
		.dev_size = "/proc/device-tree/shared_commem/wt-pointer-size",
	},
	{
		.id = IPCU_MM_MREC,
		.dev_addr = "/proc/device-tree/shared_commem/movie-rec-addr",
		.dev_size = "/proc/device-tree/shared_commem/movie-rec-size",
	},
	{
		.id = IPCU_MM_TERMIO, 
		.dev_addr = "/proc/device-tree/shared_commem/term-io-addr",
		.dev_size = "/proc/device-tree/shared_commem/term-io-size",
	},
	{
		.id = IPCU_MM_STR, 
		.dev_addr = "/proc/device-tree/shared_commem/string-addr",
		.dev_size = "/proc/device-tree/shared_commem/string-size",
	},
	{
		.id = IPCU_MM_RBRY, 
		.dev_addr = "/proc/device-tree/shared_commem/ribery-addr",
		.dev_size = "/proc/device-tree/shared_commem/ribery-size",
	},
	{
		.id = IPCU_MM_AUDI, 
		.dev_addr = "/proc/device-tree/shared_commem/audio-addr",
		.dev_size = "/proc/device-tree/shared_commem/audio-size",
	},
	{
		.id = IPCU_MM_RAW, 
		.dev_addr = "/proc/device-tree/shared_commem/raw-addr",
		.dev_size = "/proc/device-tree/shared_commem/raw-size",
	},
	{
		.id = IPCU_MM_YUV, 
		.dev_addr = "/proc/device-tree/shared_commem/yuv-addr",
		.dev_size = "/proc/device-tree/shared_commem/yuv-size",
	},
	{
		.id = IPCU_MM_HEVC, 
		.dev_addr = "/proc/device-tree/shared_commem/hevc-addr",
		.dev_size = "/proc/device-tree/shared_commem/hevc-size",
	},
	{
		.id = IPCU_MM_JPEG, 
		.dev_addr = "/proc/device-tree/shared_commem/mjpeg-addr",
		.dev_size = "/proc/device-tree/shared_commem/mjpeg-size",
	},
	{
		.id = IPCU_MM_CAP, 
		.dev_addr = "/proc/device-tree/shared_commem/cap-addr",
		.dev_size = "/proc/device-tree/shared_commem/cap-size",
	},
	{
		.id = IPCU_MM_OSD,
		.dev_addr = "/proc/device-tree/shared_commem/osd-addr",
		.dev_size = "/proc/device-tree/shared_commem/osd-size",
	},
	{
		.id = IPCU_MM_AOUT, 
		.dev_addr = "/proc/device-tree/shared_commem/audio-out-addr",
		.dev_size = "/proc/device-tree/shared_commem/audio-out-size",
	},
	{
		.id = IPCU_MM_META, 
		.dev_addr = "/proc/device-tree/shared_commem/metadata-addr",
		.dev_size = "/proc/device-tree/shared_commem/metadata-size",
	},
	{
		.id = IPCU_MM_LCD0_OSD,
		.dev_addr = "/proc/device-tree/shared_commem/lcd-osd-gr0-frame-addr",
		.dev_size = "/proc/device-tree/shared_commem/lcd-osd-gr0-frame-size",
	},
	{
		.id = IPCU_MM_HDMI0_OSD,
		.dev_addr = "/proc/device-tree/shared_commem/hdmi-osd-gr0-frame-addr",
		.dev_size = "/proc/device-tree/shared_commem/hdmi-osd-gr0-frame-size",
	},
	{
		.id = IPCU_MM_LCD1_OSD,
		.dev_addr = "/proc/device-tree/shared_commem/lcd-osd-gr1-frame-addr",
		.dev_size = "/proc/device-tree/shared_commem/lcd-osd-gr1-frame-size",
	},
	{
		.id = IPCU_MM_HDMI1_OSD,
		.dev_addr = "/proc/device-tree/shared_commem/hdmi-osd-gr1-frame-addr",
		.dev_size = "/proc/device-tree/shared_commem/hdmi-osd-gr1-frame-size",
	},
	{
		.id = IPCU_MM_RIBERY_S, 
		.dev_addr = "/proc/device-tree/shared_commem/ribery-stream-s-addr",
		.dev_size = "/proc/device-tree/shared_commem/ribery-stream-s-size",
	},
	{
		.id = IPCU_MM_FW_UPDATE,
		.dev_addr = "/proc/device-tree/shared_commem/fwupdate-addr",
		.dev_size = "/proc/device-tree/shared_commem/fwupdate-size",
	},
};

/* Information for non-shared memory area */
struct sn_platform_shmem {
	UINT32          id;
	UINT32          vaddr;
	const UINT32    paddr;
	const UINT32    size;
};

static struct sn_platform_shmem nonshm_cfgs[] = {
	{
		.paddr = 0x00000000,
		.size = 0x40000000,
	},
	{
		.paddr = 0x80000000,
		.size = 0x20000000,
	},
	{
		.paddr = 0xE0000000,
		.size = 0x20000000,
	},
	{
		.paddr = 0x40000000,
		.size = 0xC700000,
	},
	{
		.paddr = 0x4C700000,
		.size = 0x3200000,
	},
	{
		.paddr = 0x4F900000,
		.size = 0x500000,
	},
	{
		.paddr = 0x4FF00000,
		.size = 0x100000,
	},
	{
		.paddr = 0x50000000,
		.size = 0x70000,
	},
	{
		.paddr = 0x50070000,
		.size = 0x10000,
	},
	{
		.paddr = 0x50080000,
		.size = 0x180000,
	},
	{
		.paddr = 0xA0000000,
		.size = 0x4000000,
	},
	{
		.paddr = 0xA4000000,
		.size = 0x5000000,
	},
	{
		.paddr = 0xA93D4000,
		.size = (0x96000 * 4),
	},
	{
		.paddr = 0xA962C000,
		.size = (0x2A3000 * 2),
	},
	{
		.paddr = 0xA9C66240,
		.size = 0x927C0,
	},
	{
		.paddr = 0xAAD00000,
		.size = 0x96000,
	},
	{
		.paddr = 0xAAD96000,
		.size = 0x2A3000,
	},
	{
		.paddr = 0xAB039000,
		.size = 0x1400000,
	},
};

#define NONSHM_CFG_MAX (sizeof(nonshm_cfgs) / sizeof(struct sn_platform_shmem))
static INT32 mem_fd = -1;


/******************************************************************************/
/*
 * Private functions
 */
/******************************************************************************/
UINT32 fj_mm_get_shared_val(char *str)
{
	volatile unsigned char *iomap;
	int ofs;
	FILE *fp;
	int proc_paddr;
	unsigned long proc_page;
	int share_data;
	if( (fp = fopen(str,"rb")) == NULL )
	{
		printf("cannot open %s\n",str);
		share_data = -1;
	}
	else
	{
		proc_paddr = 0;
		proc_paddr  = (((UINT32)getc(fp) << 24 ) & 0xFF000000);
		proc_paddr |= (((UINT32)getc(fp) << 16 ) & 0x00FF0000);
		proc_paddr |= (((UINT32)getc(fp) <<  8 ) & 0x0000FF00);
		proc_paddr |= (((UINT32)getc(fp) <<  0 ) & 0x000000FF);
		fclose(fp);
		if (mem_fd == -1) {
			mem_fd=open(COMMEM_PATH, O_RDWR);
			if( mem_fd <= 0 )
			{
				perror(COMMEM_PATH);
				exit(1);
			}
		}
		proc_page = proc_paddr & PAGE_MASK;
		ofs = proc_paddr - proc_page;
	
		iomap = mmap(0, (ofs + 4), PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, proc_page);
		share_data = *((int*)(iomap + ofs));
		munmap((void*)iomap, ofs + 4);
	}

//	printf("0x%08X\n",proc_paddr);
	return share_data;
}
void fj_mm_make_mmap_cfg()
{
	int i;

	registMemMax = MMAP_CFG_MAX;
	for( i = 0; i < MMAP_CFG_MAX; i++ )
	{
		mmap_cfgs[i].paddr = fj_mm_get_shared_val((char*)mmap_cfgs[i].dev_addr);
		mmap_cfgs[i].size  = fj_mm_get_shared_val((char*)mmap_cfgs[i].dev_size);
		if( mmap_cfgs[i].paddr == 0xFFFFFFFF )
		{
			registMemMax = i;
			printf("[INFO] Share memory num %lx\n",registMemMax);
			break;
		}
	}
	
	#if 0
	for( i = 0; i < FJ_MM_Get_SharedMemory_Max(); i++ )
	{
		printf("addr:0x%08X ,size:0x%08X\n",mmap_cfgs[i].paddr,mmap_cfgs[i].size);
	}
	#endif

	return;
}                                         
void *fj_mm_mmap(UINT32 paddr, UINT32 length)
{
	void *vaddr, *page_vaddr;
	UINT32 page_paddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	if (mem_fd < 0) {
		LOGE("%s: not ready \n", __func__);
		return MAP_FAILED;
	}
	
	page_paddr = paddr & PAGE_MASK;
	page_offset = paddr & ~PAGE_MASK;
	page_length = length + page_offset;

	LOGV("%s: mmap set: paddr=0x%x offset=0x%x length=0x%x\n",
		__func__, page_paddr, page_offset, page_length);

	page_vaddr = mmap(0, page_length, PROT_READ | PROT_WRITE, 
			MAP_SHARED, mem_fd, page_paddr);
	if (page_vaddr == MAP_FAILED) {
		LOGE("%s: mmap failed paddr=0x%x offset=0x%x length=0x%x\n",
			__func__, page_paddr, page_offset, page_length);
		return MAP_FAILED;
	}
	vaddr = page_vaddr + page_offset;
	
	LOGV("%s: paddr=0x%08x vaddr=%p length=0x%x \n", __func__, paddr, vaddr, length);
	LOGV("%s: page_paddr=0x%08x page_vaddr=%p page_length=0x%x \n", __func__, page_paddr, page_vaddr, page_length);
	
	return vaddr;
}

INT32 fj_mm_munmap(void *vaddr, UINT32 length)
{
	UINT32 page_vaddr;
	UINT32 page_offset;
	UINT32 page_length;
	
	page_vaddr = (UINT32)vaddr & PAGE_MASK;
	page_offset = (UINT32)vaddr & ~PAGE_MASK;
	page_length = length + page_offset;
	
	LOGV("%s: vaddr=%p length=0x%x \n", __func__, vaddr, length);
	LOGV("%s: page_vaddr=0x%08x page_length=0x%x \n", __func__, page_vaddr, page_length);
	
	return munmap((void*)page_vaddr, page_length);
}


/******************************************************************************/
/**
 *  @brief  Open Memory Manager
 *  @fn FJ_ERR_CODE fj_mm_open(VOID)
 *  @param
 *  @retval FJ_ERR_OK		: Success
 *  @retval FJ_ERR_NG		: Error
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
FJ_ERR_CODE fj_mm_open(VOID)
{
	INT32 i;
	struct sn_platform_mmap *mmap_cfg;
	
	/* Map a shared memory segment */
	if (mem_fd == -1) {
		mem_fd = open(COMMEM_PATH, (O_RDWR | O_SYNC));
		if (mem_fd < 0) {
			LOGE("%s: failed to open %s \n", __func__, COMMEM_PATH);
			return FJ_ERR_NG;
		}
		fj_mm_make_mmap_cfg();
		for (i = 0; i < FJ_MM_Get_SharedMemory_Max(); i++) {
			mmap_cfg = &mmap_cfgs[i];
			if( mmap_cfg->paddr != 0 ) {
				mmap_cfg->vaddr = (UINT32) fj_mm_mmap(mmap_cfg->paddr, mmap_cfg->size);
				if (mmap_cfg->vaddr == (UINT32)MAP_FAILED) {
					LOGE("%s: failed to mmap 0x%08x (%s) \n", __func__, mmap_cfg->paddr, strerror(errno));
					return FJ_ERR_NG;
				}
			}
			else {
				mmap_cfg->vaddr = 0;
				LOGV("%s: 0x%08X[%02d] does not use (%s) \n", __func__, mmap_cfg->paddr, i, strerror(errno));
			}
		}
	}
	return FJ_ERR_OK;
}

/******************************************************************************/
/**
 *  @brief  Close Memory Manager
 *  @fn FJ_ERR_CODE fj_mm_close(VOID)
 *  @param
 *  @retval FJ_ERR_OK		: Success
 *  @retval FJ_ERR_NG		: Error
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
FJ_ERR_CODE fj_mm_close(VOID)
{
	INT32 i;
	struct sn_platform_mmap *mmap_cfg;
	
	if (mem_fd != -1) {
		for (i = 0; i < FJ_MM_Get_SharedMemory_Max(); i++) {
			mmap_cfg = &mmap_cfgs[i];
			if( mmap_cfg->paddr != 0 ) {
				fj_mm_munmap((void*)mmap_cfg->vaddr, mmap_cfg->size);
				LOGV("munmap#%d: map_vaddr=0x%08x \n", i, mmap_cfg->vaddr);
			
				mmap_cfg->map_vaddr = (UINT32)NULL;
				mmap_cfg->map_size = 0;
				mmap_cfg->paddr = (UINT32)NULL;
				mmap_cfg->vaddr = (UINT32)NULL;
			}else{
				LOGV("%s: 0x%08X[%02d] does not use (%s) \n", __func__, mmap_cfg->paddr, i, strerror(errno));
			}
		}
		
		close(mem_fd);
		mem_fd = -1;
		LOGV("%s: close %s \n", __func__, COMMEM_PATH);
	}
	
	return FJ_ERR_OK;
}

/******************************************************************************/
/**
 *  @brief  convert virtual address to physical address
 *  @fn UINT32 fj_mm_virt_to_phys(UINT32 vaddr)
 *  @param  [in]    vaddr       : virtual address
 *  @retval physical address
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
UINT32 fj_mm_virt_to_phys(UINT32 vaddr)
{
	/* To be implemented */
	INT32 i;
	UINT32 offset;

	for (i = 0; i < FJ_MM_Get_SharedMemory_Max(); i++) {
		if ((mmap_cfgs[i].vaddr <= vaddr) && 
			(vaddr < mmap_cfgs[i].vaddr + mmap_cfgs[i].size)) {
			
			offset = vaddr - mmap_cfgs[i].vaddr;
			return mmap_cfgs[i].paddr + offset;
		}
	}
	return (UINT32)MAP_FAILED;
}

/******************************************************************************/
/**
 *  @brief  Check whether the memory area is shared or non-shared
 *  @fn UINT32 fj_mm_is_shared_region(UINT32 paddr)
 *  @param  [in]    paddr       : physical address
 *  @retval 0: Non-shared  1:Shared
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
UINT32 fj_mm_is_shared_region(UINT32 paddr)
{
	INT32 i;
	
	if (paddr == (UINT32)MAP_FAILED) {
		return D_NON_SHARED;
	}
	
	for (i = 0; i < NONSHM_CFG_MAX; i++) {
		if ((nonshm_cfgs[i].paddr <= paddr) && 
				(paddr <= nonshm_cfgs[i].paddr + nonshm_cfgs[i].size - 1)) {
			
			return D_NON_SHARED;
		}
	}
	return D_SHARED;
}

/******************************************************************************/
/**
 *  @brief  convert physical address to virtual address
 *  @fn UINT32 fj_mm_phys_to_virt(UINT32 paddr)
 *  @param  [in]    paddr       : physical address
 *  @retval virtual address
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
UINT32 fj_mm_phys_to_virt(UINT32 paddr)
{
	INT32 i;
	UINT32 offset;

	for (i = 0; i < FJ_MM_Get_SharedMemory_Max(); i++) {
		//remove checking addre within bootstrap mmap
		if ((mmap_cfgs[i].paddr <= paddr) && 
			(paddr < mmap_cfgs[i].paddr + mmap_cfgs[i].size)) {
			
			if (mmap_cfgs[i].vaddr == (UINT32)MAP_FAILED) {
				continue;
			}
			if( mmap_cfgs[i].paddr != 0 ) {
				offset = paddr - mmap_cfgs[i].paddr;
				return mmap_cfgs[i].vaddr + offset;
			}
		}
	}
	printf("%s, failed to do mem phys to virt, paddr:0x%08x\n", __func__, paddr);
	return (UINT32)MAP_FAILED;
}

/******************************************************************************/
/*
 * Public functions
 */
/******************************************************************************/
UINT32 FJ_MM_getPhysAddrInfo(UINT32 id, UINT32* pa, UINT32* sz)
{
	UINT32 ret;
	if( id < FJ_MM_Get_SharedMemory_Max() )
	{
		*pa = mmap_cfgs[id].paddr;
		*sz = mmap_cfgs[id].size;
		ret = FJ_ERR_OK;
	}
	else
	{
		ret = FJ_ERR_NG;
	}
	return ret;
}
/******************************************************************************/
/**
 *  @brief  convert virtual address to physical address
 *  @fn UINT32 FJ_MM_virt_to_phys(UINT32 vaddr)
 *  @param  [in]    vaddr       : virtual address
 *  @retval physical address
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
UINT32 FJ_MM_virt_to_phys(UINT32 vaddr)
{
	return fj_mm_virt_to_phys(vaddr);
}

/******************************************************************************/
/**
 *  @brief  convert physical address to virtual address
 *  @fn UINT32 FJ_MM_phys_to_virt(UINT32 paddr)
 *  @param  [in]    paddr       : physical address
 *  @retval virtual address
 *  @note
 *  @author
 *  @version
 *  @attention
 */
/******************************************************************************/
UINT32 FJ_MM_phys_to_virt(UINT32 paddr)
{
	return fj_mm_phys_to_virt(paddr);
}
UINT32 FJ_MM_Get_SharedMemory_Max(VOID)
{
	return registMemMax;
}

