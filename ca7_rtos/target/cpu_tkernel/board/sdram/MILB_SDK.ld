MEMORY
{
  ROM(rwx)  : ORIGIN = 0xA0000000, LENGTH = 0x02000000
  RAM(rwx)  : ORIGIN = 0xA2000000, LENGTH = 0x04800000
}

SECTIONS
{
	.IO_GPV				0x19000000 (NOLOAD): {*(.GPV)}
	.IO_NF_REG			0x19100000 (NOLOAD): {*(.NF_REG)}
	.IO_EMMC			0x19120000 (NOLOAD): {*(.EMMC)}
	.IO_SDXC32			0x19130000 (NOLOAD): {*(.SDXC32)}
	.IO_XDMAC			0x19140000 (NOLOAD): {*(.XDMAC)}
	.IO_PCIE0_CORE		0x19150000 (NOLOAD): {*(.PCIE0_CORE)}
	.IO_PCIE1_CORE		0x19160000 (NOLOAD): {*(.PCIE1_CORE)}
	.IO_SDXC2			0x1B000000 (NOLOAD): {*(.SDXC2)}
	.IO_SDXC4			0x1B010000 (NOLOAD): {*(.SDXC4)}
	.IO_SDXC31			0x1B020000 (NOLOAD): {*(.SDXC31)}
	.IO_RELC			0x1B030000 (NOLOAD): {*(.RELC)}
	.IO_HDMAC			0x1B040000 (NOLOAD): {*(.HDMAC)}
	.IO_EXSTOP			0x1B110000 (NOLOAD): {*(.EXSTOP)}
	.IO_PCIE0			0x1B111000 (NOLOAD): {*(.PCIE0)}
	.IO_PCIE1			0x1B112000 (NOLOAD): {*(.PCIE1)}
	.IO_XDMACS			0x1C250000 (NOLOAD): {*(.XDMACS)}
	.IO_IPCU0			0x1C251000 (NOLOAD): {*(.IPCU0)}
	.IO_IPCU1			0x1C252000 (NOLOAD): {*(.IPCU1)}
	.IO_MCC				0x1C26A000 (NOLOAD): {*(.MCC)}
	.IO_WDOG			0x1C26C000 (NOLOAD): {*(.WDOG)}
	.IO_EXIU			0x1C26F000 (NOLOAD): {*(.EXIU)}
	.IO_GIC_DIST		0x1D001000 (NOLOAD): {*(.GIC_DIST)}
	.IO_GIC_CPU			0x1D002000 (NOLOAD): {*(.GIC_CPU)}
	.IO_CHIPTOP			0x1D020000 (NOLOAD): {*(.CHIPTOP)}
	.IO_NIC_ADDRCTRL	0x1D100000 (NOLOAD): {*(.NIC_ADDRCTRL)}
	.IO_NIC_ASIB		0x1D142000 (NOLOAD): {*(.NIC_ASIB)}
	.IO_TIMESTAMP		0x1D010000 (NOLOAD): {*(.TIMESTAMP)}
	.IO_BMH				0x1D400000 (NOLOAD): {*(.BMH)}
	.IO_PMC				0x1DC00000 (NOLOAD): {*(.PMC)}
	.IO_PERI			0x1E000000 (NOLOAD): {*(.PERI)}
	.IO_PDM				0x1E004000 (NOLOAD): {*(.PDM)}
	.IO_HDMAC0			0x1E100000 (NOLOAD): {*(.HDMAC0)}
	.IO_HDMAC1			0x1E110000 (NOLOAD): {*(.HDMAC1)}
	.IO_PWMDMA			0x1E120000 (NOLOAD): {*(.PWMDMA)}
	.IO_AUDIO			0x1E200000 (NOLOAD): {*(.AUDIO)}
	.IO_SLIMBUS0		0x1E300000 (NOLOAD): {*(.SLIMBUS0)}
	.IO_SLIMBUS1		0x1E400000 (NOLOAD): {*(.SLIMBUS1)}
	.IO_USIO			0x1E700000 (NOLOAD): {*(.USIO)}
	.IO_SPI				0x1E800000 (NOLOAD): {*(.SPI)}
	.IO_SLIMBUS2		0x1E900000 (NOLOAD): {*(.SLIMBUS2)}
	.IO_SLIMBUS3		0x1EA00000 (NOLOAD): {*(.SLIMBUS3)}
	.IO_DSP_PERI		0x1F000000 (NOLOAD): {*(.DSP_PERI)}
	.IO_PRO				0x28008000 (NOLOAD): {*(.PRO)}
	.IO_MXIC0			0x28000000 (NOLOAD): {*(.MXIC0)}
	.IO_MXIC1			0x28400000 (NOLOAD): {*(.MXIC1)}
	.IO_B2R_P1			0x28412000 (NOLOAD): {*(.B2R_P1)}
	.IO_LTM_P1			0x28413000 (NOLOAD): {*(.LTM_P1)}
	.IO_R2Y_P1			0x28418000 (NOLOAD): {*(.R2Y_P1)}
	.IO_CNR1			0x28420000 (NOLOAD): {*(.CNR_1)}
	.IO_MXIC2			0x28500000 (NOLOAD): {*(.MXIC2)}
	.IO_B2R_P2			0x28512000 (NOLOAD): {*(.B2R_P2)}
	.IO_LTM_P2			0x28513000 (NOLOAD): {*(.LTM_P2)}
	.IO_R2Y_P2			0x28518000 (NOLOAD): {*(.R2Y_P2)}
	.IO_CNR2			0x28520000 (NOLOAD): {*(.CNR_2)}
	.IO_MXIC3			0x28810000 (NOLOAD): {*(.MXIC3)}
	.IO_B2R_P3			0x28612000 (NOLOAD): {*(.B2R_P3)}
	.IO_LTM_P3			0x28613000 (NOLOAD): {*(.LTM_P3)}
	.IO_R2Y_P3			0x28618000 (NOLOAD): {*(.R2Y_P3)}
	.IO_CNR3			0x28620000 (NOLOAD): {*(.CNR_3)}
	.IO_IMG				0x28800000 (NOLOAD): {*(.IMG)}
	.IO_MXIC4			0x28814000 (NOLOAD): {*(.MXIC4)}
	.IO_MXIC5			0x28818000 (NOLOAD): {*(.MXIC5)}
	.IO_MXIC6			0x2881C000 (NOLOAD): {*(.MXIC6)}
	.IO_SDRAMC			0x28820000 (NOLOAD): {*(.UMC)}
	.IO_RDMA			0x28869000 (NOLOAD): {*(.RDMA)}
	.IO_SHDR			0x2886A000 (NOLOAD): {*(.SHDR)}
	.IO_ME				0x28870000 (NOLOAD): {*(.ME)}
	.IO_IIP				0x28880000 (NOLOAD): {*(.IIP)}
	.IO_RAW				0x288A0000 (NOLOAD): {*(.RAW)}
	.IO_ELA				0x288B0000 (NOLOAD): {*(.ELA)}
	.IO_XCH				0x288C0000 (NOLOAD): {*(.Xch)}
	.IO_FPT				0x288D0000 (NOLOAD): {*(.FPT)}
	.IO_DISP			0x28900000 (NOLOAD): {*(.DISP)}
	.IO_PRO_TBL			0x29000000 (NOLOAD): {*(.PRO_TBL)}
	.IO_MXIC1_TBL		0x29400000 (NOLOAD): {*(.MXIC1_TBL)}
	.IO_B2R_TBL_P1		0x29482000 (NOLOAD): {*(.B2R_TBL_P1)}
	.IO_LTM_TBL_P1		0x29490000 (NOLOAD): {*(.LTM_TBL_P1)}
	.IO_R2Y_TBL_P1		0x294A0000 (NOLOAD): {*(.R2Y_TBL_P1)}
	.IO_MXIC2_TBL		0x29500000 (NOLOAD): {*(.MXIC2_TBL)}
	.IO_B2R_TBL_P2		0x29582000 (NOLOAD): {*(.B2R_TBL_P2)}
	.IO_LTM_TBL_P2		0x29590000 (NOLOAD): {*(.LTM_TBL_P2)}
	.IO_R2Y_TBL_P2		0x295A0000 (NOLOAD): {*(.R2Y_TBL_P2)}
	.IO_B2R_TBL_P3		0x29682000 (NOLOAD): {*(.B2R_TBL_P3)}
	.IO_LTM_TBL_P3		0x29690000 (NOLOAD): {*(.LTM_TBL_P3)}
	.IO_R2Y_TBL_P3		0x296A0000 (NOLOAD): {*(.R2Y_TBL_P3)}
	.IO_MXIC3_TBL		0x29800000 (NOLOAD): {*(.MXIC3_TBL)}
	.IO_SHDR_TBL		0x29815000 (NOLOAD): {*(.SHDR_TBL)}
	.IO_IIP_TBL			0x29820000 (NOLOAD): {*(.IIP_TBL)}
	.IO_JPG7			0x298C0000 (NOLOAD): {*(.JPG7)}
	.IO_FPT_AHB			0x298C1000 (NOLOAD): {*(.FPT_AHB)}
	.IO_DISP_TBL		0x298E0000 (NOLOAD): {*(.DISP_TBL)}


	.CPU_COMMON_AREA	0x0100FFF0 (NOLOAD):
	{
		__cpu_common_start = .;
		. += 0x10; */
		__cpu_common_end = .;
	}
	.EXS_SRAM 0x18000000 (NOLOAD) :
	{
		KEEP(*mw_exs_iram.o(.EXS_IRAM))
	}
	.LINUX_RTOS_COMMON_BUFF 0x4FE00000 (NOLOAD):
	{
		*(.LINUX_RTOS_COMMON_BUFF)
	}
	.LINUX_RTOS_COMMON_SYNC 0x4FE90000 (NOLOAD):
	{
		*(.LINUX_RTOS_COMMON_SYNC)
	}
	.PARTITION_BLOCK 0x50070000 (NOLOAD):
	{
		KEEP(*mw_user_custom.o(.PARTITION_BLOCK_SECTION))
	}
	.NAND_PARAMETER 0x50070100 (NOLOAD):
	{
		KEEP(*mw_user_custom.o(.NAND_PARAMETER_SECTION))
	}
	.PARTITION_TABLE 0x50071000 (NOLOAD):
	{
		KEEP(*mw_user_custom.o(.PARTITION_TABLE_SECTION))
	}
	USER_DATA_AREA 0xA7300000 (NOLOAD):
	{
		__user_data_start = .;
		. += 0x20000;
		__user_data_end = .;
	}
	RESOURCE_AREA 0xA6900000 (NOLOAD):
	{
		__resource_start = .;
		. += 0xA00000;
		__resource_end = .;
	}

	. = 0xA0000000;
	.text :
	{
		RAM_AREA = .;
		KEEP(*vector.o(.INIT_VECTOR_TBL))
		KEEP(*boot.o(.BOOT_GCC))
		KEEP(*init.o(.INIT_CODE))
		KEEP(*gic.o(.INIT_GIC))
		KEEP(*icrt0.o(.ICRT0))
    	KEEP(*init_io.o(.INIT_IO))
    	. = ALIGN(0x4000);
		KEEP(*mmu.o(.MMU_CODE))
		KEEP(*cacheops-v7.o(.CACHE_OPERATION_V7))
		KEEP(*pmu.o(.PMU_CODE))
		KEEP(*disp_osd_copy_by_src_offset.o(.DISP_OSD))
		KEEP(*im_fpt_asm.o(.FPT_CODE))
		KEEP(*dd_arm_asm.o(.DD_ARM_ASM))
		KEEP(*dd_cache_asm.o(.DD_CACHE_ASM))
		*(.text)
		*(.text.*)
	} > ROM

	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
	} > ROM

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} > ROM

	. = ALIGN(__exidx_end, 64);
	__data_org = .;

	.data : AT (__data_org)
	{
		__data_start = .;
		*(.LOCK_SECTION)
		. = ALIGN(64);
		*(.DCACHE_ALIGN_SECTION)
		. = ALIGN(64);
		*(.data)
		*(.data.*)
		*(MPKID_FS_ZI_AREA)
		. = ALIGN(64);
		__data_end = .;
	} > RAM
	__data_size = SIZEOF(.data);

	__data_org_end = __data_org + __data_size;
	ASSERT(__data_start >= __data_org_end, "Init-DATA Section size overflow!")

	. = ALIGN(__data_end, 64);
	.bss (NOLOAD) :
	{
		__bss_start = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		__bss_end = ALIGN(4);
	} > RAM

	.heap ALIGN(64) (NOLOAD):
	{
		__heap_start = .;
		. += 0x40;
		__heap_end = .;
	} > RAM

	.basefw_heap ALIGN(64) (NOLOAD):
	{
		__basefw_heap_start = .;
		. += 0x02000000;
		__basefw_heap_end = .;
	} > RAM

	.stack ALIGN(64) (NOLOAD) :
	{
		__kernel_memory_start = .;
		. += 0xA00000;
		__kernel_memory_end = .;
	} > RAM
	ASSERT(__kernel_memory_end<=0xA6800000, "Section size overflow!")

	.MDF_HEAP_SECTION 0xA6800000 (NOLOAD):
	{
		*(.MDF_HEAP_SECTION)
	}
}
