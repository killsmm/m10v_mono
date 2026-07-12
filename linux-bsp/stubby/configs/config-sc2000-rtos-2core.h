#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_SC2000A
#define CONFIG_MLB01
#define CONFIG_BUILD_NAME "SC2000A"

#define CONFIG_ARMV7 1 /* ARMv7 / A7 */
//#define CONFIG_SYS_DCACHE_OFF
//#define CONFIG_SYS_ICACHE_OFF

//#define CONFIG_GLOBALTIMER_PHYS 0xf8100208

#define CONFIG_SYS_UART_USIO
#define CONFIG_SYS_USIO_CH	(0x1)//Use UART channel 2
#define CONFIG_SYS_USIO		(0x1E700000 + CONFIG_SYS_USIO_CH * 0x10)

#define CONFIG_SYS_SDRAM_BASE	0x40000000  /* Start address of DDR */
#define CONFIG_TRAPOLINE_ADDRESS    (0x4C000100) //Secondary core jump address saving area

#define OFFSET_STUBBY_DDR_VARS 	0x00004000
#define OFFSET_DDR_STACK 	0x00007F80

#define CONFIG_MULTI_CORE_LINUX   //multi core are worked by Liux
#define CONFIG_GIC_FIQ
#ifdef CONFIG_M8M
	#define CONFIG_SYS_PL310_BASE	0xf8102000
#endif //CONFIG_M8M
#ifndef CONFIG_MULTI_CORE_LINUX
    #define CONFIG_KILL_SMP
#endif //CONFIG_MULTI_CORE_LINUX
#define CONFIG_RTOS_OWNS_IPS
/* fudge factor for around 1us in spin loop */
#define US_SPIN_FACTOR 3

#define CONFIG_F_SDH30_HOSTS_INIT \
{ \
	.ioaddr = (void *)0x1B020000, \
	.mmc = { .host_caps = MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT }, \
		.max_clk = 175000000 \
	}

#define CONFIG_FILEPATH_KERNEL "boot/Image-nolpae"
#define CONFIG_FILEPATH_INITRAMFS "boot/initramfs"
#define CONFIG_FILEPATH_DTB "boot/sc2000.dtb"
#define CONFIG_FILEPATH_BOOTLOADER "boot/u-boot.bin"

#define NOR_ROMFS_OFFSET 0

/* memory-mapped areas to check for ROMFS */
#define CONFIG_LINUX_ROMFS_ADDRESS	(0x01008000)
#define CONFIG_ROMFS_POSSIBLE_ADDRESSES 	(CONFIG_SYS_SDRAM_BASE + CONFIG_LINUX_ROMFS_ADDRESS + NOR_ROMFS_OFFSET)

/* 
 * allow stubby to modify dtb at runtime from this file
 * NOTE -- included in sigature if using enforcing crypto...
 */
#define CONFIG_FILEPATH_DTB_CONFIG "boot/dtb.cfg"

//#define CONFIG_INITRAMFS_OFFSET		(0xA9000000-CONFIG_SYS_SDRAM_BASE) /* == 0xA9000000 PA */
//#define CONFIG_INITRAMFS_OFFSET		(0x49000000-CONFIG_SYS_SDRAM_BASE) /* == 0x49000000 PA */
#define CONFIG_INITRAMFS_OFFSET		(0x43000000-CONFIG_SYS_SDRAM_BASE) /* == 0x43000000 PA */

#define CONFIG_FOLLOW_PARTITION_BOOT_FLAG

/* allow runtime boot source selection */
//#define CONFIG_BOOT_SOURCE_MENU


/* for update dtb string */

//#define CONFIG_SD_DEVICE "mmcblk0"
/* 
 * if CONFIG_RSA_SIGNATURE, include signature verification code...
 * !!! not enforced unless _ENFORCING below also enabled !!!
 */
//#define CONFIG_RSA_SIGNATURE

/* choose one or the other hash */
//#define CONFIG_HASH_TYPE_SHA1
#define CONFIG_HASH_TYPE_SHA256

/* 
 * if RSA_SIGNATURE but not below, when sig fails that's just
 * reported but kernel still used.  If _ENFORCING is given, signature fail
 * means the kernel is rejected and will not be used.
 *
 * non-enforcing is interesting for development since it lets you
 * confirm files + sig will work on a bootloader with the same public key
 * but enforcing, wthout having to change your bootloader
 */
//#define CONFIG_RSA_SIGNATURE_ENFORCING

#define CONFIG_FILEPATH_KERNEL_SIG "boot/kernel+dtb.sig"
#define CONFIG_FILEPATH_BOOTLOADER_SIG "boot/u-boot.sig"

//#define CONFIG_NONSECURE_ACCESS

#define CONFIG_ARM_GIC_BASE_ADDRESS (0x1D002000)//IO_GIC_CPU

#define PRIMARY_CPU 0x08 //CPU3

#ifdef CONFIG_M8M
	/* put PL310 register memory map and mask value here */
	#define PL310_AUX_CTRL_ASSOCIATIVITY_MASK	(1 << 16)

	#define PL310_BASE              (CONFIG_SYS_PL310_BASE)
	#define PL310_CTRL              (PL310_BASE + 0x100)
	#define PL310_AUX_CTRL          (PL310_BASE + 0x104)
	#define PL310_CACHE_SYNC        (PL310_BASE + 0x730)
	#define PL310_INV_LINE_PA       (PL310_BASE + 0x770)
	#define PL310_INV_BY_WAY        (PL310_BASE + 0x77C)
	#define PL310_CLEAN_INV_LINE_PA (PL310_BASE + 0x7F0)
	#define PL310_CLEAN_INV_WAY     (PL310_BASE + 0x7FC)
#endif //CONFIG_M8M
// Real OS loaded at 0x60000000 so size must bigger than 0x20000000+RealOS-size
//# define DRAM_SIZE		0x20000000 // 512MiB
//# define DRAM_SIZE		0x60000000 // 1536MiB
# define DRAM_CH0_SIZE	0x20000000

#define CONFIG_STUBBY_ALREADY_IN_SRAM
#define SRAM_BASE		0x01000000
#define SRAM_SIZE		0x00010000
#define SRAM_CODE1		0x0100C000
#define SRAM_CODE0		0x0100C100

/* Execute kernel, dtb, and initramfs in-place */
#define ASSETS_XIP
#define COMBINE_KERNEL_INITRAMFS_2_1_FILE//Independent of the INITRAMFS from ROMFS

#ifdef CONFIG_M8M
	#define ADDR_RTOS_TOP		0x60000000 /** RealOS placement Address(start here) */
	#define ADDR_RTOS_ENTRY		0x60000000 /** Entry point of RealOS (almost Same as ADDR_RTOS_TOP) */
#endif //CONFIG_M8M
/** The address which indicates start message */
#define COOPERATION_AREA	(SRAM_BASE + 0xFE80)	/** cpu1 check this value to wait cpu0 */

#ifdef CONFIG_M8M
	#define PERIPHBASE		0xF8100000
	#define SCU_BASE		(PERIPHBASE + 0x0000)
	#define L2C_310_BASE		(PERIPHBASE + 0x2000)
	#define MCSWFI_PHY		(0xFFF6A010)	/** Standby WFI monitor register */

	#define IMAGE_AHB_BASE		(0x24000000)
	#define IMAGE_APB_BASE		(0x20000000)
	#define MXIC_AHB_BASE		(0x24130000)
	#define MXIC_APB_BASE		(0x20010000)
	#define IOADR_MXIC_TMIRST	(MXIC_APB_BASE + 0x0000)	// リセット制御レジスタ
	#define IOADR_MXIC_TSMTRG	(MXIC_APB_BASE + 0x0100)	// スレーブ領域取り込みトリガ
	#define IOADR_MXIC_TREGION	(MXIC_APB_BASE + 0x0120)	// スレーブ領域設定


	#define pPUDCR			0x18002400
	#define	pPUDER			0x18002300
	#define pEPCR			0x18002200
	#define pPDR_S			0x18002050
	#define pPDR_C			0x180020A0
	#define pDDR			0x18002100
	#define pPDR			0x18002000//CHIPTOP's PDR

	#define IOADR_CLKSTOP_F		0x1800103C
	#define IOADR_CLKSTOP_E 	0x18001038
	#define IOADR_CLKSTOP_D 	0x18001034
	#define IOADR_CLKSTOP_C 	0x18001030
	#define IOADR_CLKSTOP_B 	0x1800102C
	#define IOADR_CLKSTOP_A 	0x18001028
	#define IOADR_CLKSTOPS_X(n)	(0x18001128 + ((n) << 2))
	#define IOADR_CLKSTOPC_X(n)	(0x18001228 + ((n) << 2))
	#define IOADR_PLLCNTL2		0x18001024
	#define IOADR_PLLCNTL		0x18001020
	#define IOADR_CLKSEL_H		0x1800101C
	#define IOADR_CLKSEL_G		0x18001018
	#define IOADR_CLKSEL_F		0x18001014
	#define IOADR_CLKSEL_E		0x18001010
	#define IOADR_CLKSEL_D		0x1800100C
	#define IOADR_CLKSEL_C		0x18001008
	#define IOADR_CLKSEL_B		0x18001004
	#define IOADR_CLKSEL_A		0x18001000
	#define IOADR_DDRPLLCNT2	0x18001024

	#define PTIMER			0xF8100600


//
//
// // SCU設定を AMPにする。
//#define CO_PROCESSOR_MODE_AMP
//
//
// // as per "4Gb_1_35V_DDR3L.pdf", Table 53: DDR3L-1600 Speed Bins:
// // "The -125 speed grade is backward compatible with 1333, CL = 9 (-15E)"
//
	#define DDRCFG1_tCK (1000000/660)	// [pico-sec] DDR-PLL=500MHz,660MHz,800MHz(1033,1333,1600に近い最大値)
	#define DDRCFG1_tRFC		(260000) // Table 56: Electrical Characteristics and AC Operating Conditions
	#define DDRCFG1_tRFC_cyc		(DDRCFG1_tRFC / DDRCFG1_tCK) // [cyc] 
	#define DDRCFG1_tXPR		(DDRCFG1_tRFC + 10000)	// tRFC + 10nSec
	#define DDRCFG1_tDLLK	(DDRCFG1_tCK * 512)
	#define DDRCFG1_tZQinit	(DDRCFG1_tCK * 512)
	#define DDRCFG1_tMRD		(DDRCFG1_tCK * 4)
	#define DDRCFG1_tMOD		(DDRCFG1_tCK * MAX(12,(15000/DDRCFG1_tCK)))
	#define DDRCFG1_tMRSPDEN	(DDRCFG1_tMOD)
	#define DDRCFG1_tRCD		(13500)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG1_tRAS		(36000)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG1_tRRD		(MAX((DDRCFG1_tCK * 4),6000))		// 容量依存
	#define DDRCFG1_tFAW		(30000)						// 容量依存,tCK依存
	#define DDRCFG1_tRP			(13500)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG1_tWR			(15000)	// all speeds are same.
	#define DDRCFG1_tPRPDEN	(DDRCFG1_tCK * 1)
	#define DDRCFG1_CWL_cyc	(7)	// Speed Bins(DDR3L-1333,15E(9-9-9)))
	#define DDRCFG1_BL_cyc	(8)
	#define DDRCFG1_tWTR_cyc	MAX(RU(7500,DDRCFG1_tCK),4)
	#define DDRCFG1_tWR_cyc	RU(DDRCFG1_tWR,DDRCFG1_tCK)
	#define DDRCFG1_CL_cyc	(9)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG1_tRTP		(RU(7500,DDRCFG1_tCK)>=4 ? (7500) : (DDRCFG1_tCK*4) )
	#define DDRCFG1_tRTP_cyc		(MAX(RU(7500,DDRCFG1_tCK),4))
	#define DDRCFG1_tXPDLL_cyc	(MAX(10,RU(24000,DDRCFG1_tCK)))
	#define DDRCFG1_tXP_cyc		(MAX(3,RU(6000,DDRCFG1_tCK)))		//  @ Fast exit mode
	#define DDRCFG1_tZQCS_cyc	(64)
	#define DDRCFG1_tZQoper_cyc	(256)
	#define DDRCFG1_tCKE_cyc		(MAX(3,RU(5000,DDRCFG1_tCK)))
	#define DDRCFG1_tCKESR_cyc	(DDRCFG1_tCKE_cyc+1)
	#define DDRCFG1_tDLLK_cyc	(512)
	#define DDRCFG1_tXSDLL_cyc	(DDRCFG1_tDLLK_cyc)
	#define DDRCFG1_tREFI_cyc	(7800000/DDRCFG1_tCK)
	#define DDRCFG1_tRCD_cyc	(RU(DDRCFG1_tRCD,DDRCFG1_tCK))
	//#define DDRCFG1_ZQ_calcyc	(100000000000ul/DDRCFG1_tCK)	// #of "tCK cycles" 100mSec => 80000000
	//
	#define DDRCFG2_tCK (1000000/660)	// [pico-sec] DDR-PLL=500MHz,660MHz,800MHz(1033,1333,1600に近い最大値)
	#define DDRCFG2_tRFC		(260000) // Table 56: Electrical Characteristics and AC Operating Conditions
	#define DDRCFG2_tRFC_cyc		(DDRCFG2_tRFC / DDRCFG2_tCK) // [cyc] 
	#define DDRCFG2_tXPR		(DDRCFG2_tRFC + 10000)	// tRFC + 10nSec
	#define DDRCFG2_tDLLK	(DDRCFG2_tCK * 512)
	#define DDRCFG2_tZQinit	(DDRCFG2_tCK * 512)
	#define DDRCFG2_tMRD		(DDRCFG2_tCK * 4)
	#define DDRCFG2_tMOD		(DDRCFG2_tCK * MAX(12,(15000/DDRCFG2_tCK)))
	#define DDRCFG2_tMRSPDEN	(DDRCFG2_tMOD)
	#define DDRCFG2_tRCD		(13500)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG2_tRAS		(36000)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG2_tRRD		(MAX((DDRCFG2_tCK * 4),7500))		// 容量依存
	#define DDRCFG2_tFAW		(45000)						// 容量依存,tCK依存
	#define DDRCFG2_tRP			(13500)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG2_tWR			(15000)	// all speeds are same.
	#define DDRCFG2_tPRPDEN	(DDRCFG2_tCK * 1)
	#define DDRCFG2_CWL_cyc	(7)	// Speed Bins(DDR3L-1333,15E(9-9-9)))
	#define DDRCFG2_BL_cyc	(8)
	#define DDRCFG2_tWTR_cyc	MAX(RU(7500,DDRCFG2_tCK),4)
	#define DDRCFG2_tWR_cyc	RU(DDRCFG2_tWR,DDRCFG2_tCK)
	#define DDRCFG2_CL_cyc	(9)	// Speed Bins(DDR3L-1333,15E(9-9-9))
	#define DDRCFG2_tRTP		(RU(7500,DDRCFG2_tCK)>=4 ? (7500) : (DDRCFG2_tCK*4) )
	#define DDRCFG2_tRTP_cyc		(MAX(RU(7500,DDRCFG2_tCK),4))
	#define DDRCFG2_tXPDLL_cyc	(MAX(10,RU(24000,DDRCFG2_tCK)))
	#define DDRCFG2_tXP_cyc		(MAX(3,RU(6000,DDRCFG2_tCK)))		//  @ Fast exit mode
	#define DDRCFG2_tZQCS_cyc	(64)
	#define DDRCFG2_tZQoper_cyc	(256)
	#define DDRCFG2_tCKE_cyc		(MAX(3,RU(5000,DDRCFG2_tCK)))
	#define DDRCFG2_tCKESR_cyc	(DDRCFG2_tCKE_cyc+1)
	#define DDRCFG2_tDLLK_cyc	(512)
	#define DDRCFG2_tXSDLL_cyc	(DDRCFG2_tDLLK_cyc)
	#define DDRCFG2_tREFI_cyc	(7800000/DDRCFG2_tCK)
#endif //CONFIG_M8M

#ifdef CONFIG_MLB01
	#define MCSWFI_PHY		(0x1C26A010)	/** Standby WFI monitor register */
	#define IO_EPCR	(0x1D022200)//EPCR
	#define IO_EPCR_PE	(IO_EPCR+0x1C)//EPCR's PE
#endif //CONFIG_MLB01

//
#ifdef CONFIG_M8M  
#define CONFIG_ARCH_DTB_PREP {\
	static const char *path = "/cpus/cpu@0/operating-points"; \
	int asv = 640000 + (readl(0x1800003c) & 0x1f) * 10000;\
\
	if (asv >= 840000 && asv <= 900000) {\
		p = fdt_find_path(load_ads_dtb, path, &n); \
		if (!p || n < 8) {\
			puts(path);\
			puts(" missing\n");\
			return 1;\
		} else {\
			while (n > 0) {\
				uaw_be32(p + 4, asv);\
				p += 8;\
				n -= 8;\
			}\
		}\
		puts("  ASV set to ");\
		printdec(asv / 1000);\
		puts("mV\n");\
	}\
}
#endif
#endif /* __CONFIG_H */
