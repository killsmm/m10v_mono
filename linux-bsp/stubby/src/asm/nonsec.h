#ifndef __ASSEMBLY__
int armv7_init_nonsec(void);
int armv7_update_dt(void *fdt);

/* defined in assembly file */
unsigned int _nonsec_init(void);
void _do_nonsec_entry(void *target_pc, unsigned long r0,
                      unsigned long r1, unsigned long r2);
void _smp_pen(void);
int get_boot_secure(void);

#endif

/* ID_PFR1 feature fields */
#define CPUID_ARM_SEC_SHIFT		4
#define CPUID_ARM_SEC_MASK		(0xF << CPUID_ARM_SEC_SHIFT)
#define CPUID_ARM_VIRT_SHIFT		12
#define CPUID_ARM_VIRT_MASK		(0xF << CPUID_ARM_VIRT_SHIFT)
#define CPUID_ARM_GENTIMER_SHIFT	16
#define CPUID_ARM_GENTIMER_MASK		(0xF << CPUID_ARM_GENTIMER_SHIFT)

#if defined(CONFIG_MB86S70) || defined(CONFIG_MB86S73)
#define CONFIG_CCI_SECURE_ACCESS_SETVALUE (0x1) /* Enable NonSecure Access */
#define CONFIG_CCI_SECURE_ACCESS_REGISTER (0x2C090008)

#define CONFIG_SMMU_SSDR0_SETVALUE     (0xFFFFFFFF)
#define CONFIG_SMMU_SSDR0_CSS_GPU_0    (0x2B804000)
#define CONFIG_SMMU_SSDR0_CSS_DMA330   (0x2FC04000)
#define CONFIG_SMMU_SSDR0_HSIO_USB30_1 (0x34E04000)
#define CONFIG_SMMU_SSDR0_HSIO_USB20_1 (0x34F04000)
#define CONFIG_SMMU_SSDR0_HSIO_PCIE_0  (0x35A04000)
#define CONFIG_SMMU_SSDR0_HSIO_PCIE_1  (0x35B04000)
#define CONFIG_SMMU_SSDR0_HSIO_USB30_0 (0x35C04000)
#define CONFIG_SMMU_SSDR0_HSIO_USB20_0 (0x35D04000)
#define CONFIG_SMMU_SSDR0_MPB          (0x37C04000)
#endif

#define CMD_ENTER_NONSEC_MODE   0
#define CMD_SET_SECONDARY_ENTRY 1

