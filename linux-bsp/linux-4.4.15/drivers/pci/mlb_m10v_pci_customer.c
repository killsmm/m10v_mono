/*
 * PCIe device driver for Socionext SoCs
 *
 * Copyright (C) 2017 Socionext Inc.
 *              http://www.socionext.com
 *
 * Author: caspar lin <caspar.lin@socionext.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef CONFIG_M10V_AS_EP

#include <linux/resource.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/acpi.h>
#include <linux/pci_regs.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm-generic/io.h>
#include <linux/workqueue.h>

/*MSI*/
#define MSI_CTRL_INT_0_EN_OFF		0x828
#define MSI_CTRL_INT_0_MASK_OFF		0x82C

/*MISC CONTROL1 OFF*/
#define IO_PPL_MC1 0x8BC
#define   DBI_RO_WR_EN 	BIT(0)

/*Slot Capabilities Register*/
#define IO_PCIECR_SCR 0x84
#define   PCIE_CAP_ATTENTION_INDICATOR_BUTTON	BIT(0)
#define   PCIE_CAP_POWER_CONTROLLER		BIT(1)
#define   PCIE_CAP_MRL_SENSOR			BIT(2)

#define IATU_VIEWPORT_OFF 0x900
#define IO_PPL_IRC1	0x904
#define IO_PPL_IRC2     0x908
#define IO_PPL_ILBA	0x90c
#define IO_PPL_IUBA	0x910
#define IO_PPL_ILA	0x914
#define IO_PPL_ILTAO	0x918
#define IO_PPL_IUTA	0x91c

#define DMA_CTRL_OFF 			0x978
#define DMA_WRITE_ENGINE_EN_OFF 	0x97C
#define DMA_VIEWPORT_SEL_OFF 		0xA6C
#define DMA_READ_ENGINE_EN_OFF		0x99C
#define DMA_CH_CONTROL1_OFF_WRCH0 	0xA70
#define DMA_TRANSFER_SIZE_OFFWRCH0	0xA78
#define DMA_SAR_LOW_OFF_WRCH0		0xA7C
#define DMA_SAR_HIGHT_OFF_WRCH0		0xA80
#define DMA_DAR_LOW_WRCH0		0xA84
#define DMA_DAR_HIGH_OFF_WRCH0		0xA88
#define DMA_WRITE_DOORBELL_OFF		0x980
#define DMA_READ_DOORBELL_OFF		0x9A0

#define DMA_WRITE_INT_STATUS_OFF	0x9BC
#define DMA_WRITE_INT_MASK_OFF		0x9C4
#define DMA_WRITE_INT_CLEAR_OFF		0x9C8

#define DMA_READ_INT_STATUS_OFF		0xA10
#define DMA_READ_INT_MASK_OFF		0xA18
#define DMA_READ_INT_CLEAR_OFF		0xA1C

/*EXS 0x1B111000*/
#define IO_PCIE_PM_SELECT_1 0x54
#define   WAKE_SEL		BIT(0)

#define IO_PCIE_INT_CONTROL_2 0x28
#define   WAKE_EN		BIT(20)
#define   WAKE_CLEAR_EN		BIT(21)
#define	  WAKE_MASK		BIT(23)

#define IO_EXSTOP_BUSCKSTPCTRL 0x60
#define   PCIE0_CKSTP		BIT(1)
#define   PCIE1_CKSTP		BIT(2)

#define IO_EXSTOP_RAMPDCTRL	0x40
#define   RAMPD_PCIE0		 BIT(8)
#define   RAMPD_PCIE1		 BIT(9)

#define TARGET_ADDR 0x5f000000
#define DMA_BUF_SIZE SZ_4M //0x100000

static struct timeval tv1, tv2, tv3, tv4, tv99;
static u32 tv_count1, tv_count2;
static u32 rc_ddr_test_region_size = (SZ_4M - SZ_16K);
static void *rc_ddr_test_reg1, *rc_ddr_test_reg2;

struct milb_m10v{
	struct pci_dev *pdev;
	struct device *dev;
	void __iomem            *ctrl_base;

	u16 vid; /* 0x00 ~ 0x01*/
	u16 did; /* 0x02 ~ 0x03*/

	u16 cmd; /* 0x4 ~ 0x5 */
	u16 status; /* 0x6 ~ 0x7 */
	u32 cmd_status;

	u32 __iomem *base_addr;
	unsigned int io_len;
	u64 dma_mask;
	dma_addr_t dma_handle1;
	dma_addr_t dma_handle2;
	char* buf_to_dev;
	char* buf_from_dev;
	u32 msi_addr;
	u32 msi_data;
	bool need_dma_cmp;

	struct delayed_work work;
};

typedef enum {
	PCIE_DMA_DIR_DTOS = 0, 	/*dest to source(read)*/
	PCIE_DMA_DIR_STOD	/*source to dest(write)*/
}PCIE_DMA_DIR;

static const char milbpci_name[] = "milb-m10v";
static int milb_test_dmamode(struct milb_m10v *milb);
static int milb_dma_init(struct milb_m10v *milb, int dir);
struct milb_m10v* g_milb;

static int milb_dump_all(struct milb_m10v *milb)
{
	int i;
	u32 tmp;

	pr_info("milb: start to dump all registers\n");
	for(i = 0; i < 0xA90; i = i + 4){
		pci_read_config_dword(milb->pdev, i, &tmp);
		pr_info("milb: 0x%02x=0x%08x\t", i, tmp);
	}
	return 0;
}

static ssize_t store_regdump(struct device *d, struct device_attribute *attr, const char *buf, size_t count){
	struct pci_dev *dev = to_pci_dev(d);
	int ret;
	u32 tmp;
	unsigned int reg;

	sscanf(buf, "%x", &reg);

	ret = pci_read_config_dword(dev, reg, &tmp);
	pr_info("milb: regdump:%08x = 0x%08x \n", reg, tmp);	
	
	return strnlen(buf, count);
}
/*read: from ep to rc*/
static ssize_t dmaR(struct device *d, struct device_attribute *attr, char *buf)
{
	pr_info("milb: %s() - read from ep\n", __func__);

	kfree(g_milb->buf_from_dev);
	g_milb->buf_from_dev = kzalloc(DMA_BUF_SIZE, GFP_KERNEL | GFP_DMA);

	if(g_milb->buf_from_dev){
		g_milb->dma_handle2 = pci_map_single(g_milb->pdev, g_milb->buf_from_dev, DMA_BUF_SIZE, PCI_DMA_FROMDEVICE);
		if(pci_dma_mapping_error(g_milb->pdev, g_milb->dma_handle2))
			pr_info("milb: pci map single error\n");
		milb_dma_init(g_milb, PCIE_DMA_DIR_STOD);
	}
	return 0;
}
/*write: from rc to ep*/
static ssize_t dmaW(struct device *d, struct device_attribute *attr, char *buf)
{
	int i;
	u8 data;
	pr_info("milb: %s() - write to ep and will read back after msi interrupt\n", __func__);
	g_milb->need_dma_cmp = 1;
	kfree(g_milb->buf_to_dev);
	g_milb->buf_to_dev = kzalloc(DMA_BUF_SIZE, GFP_KERNEL | GFP_DMA);


	do_gettimeofday(&tv99);
	data = (u8)tv99.tv_sec;
	pr_info("milb: data to ep:%02x\n", data);

	if(g_milb->buf_to_dev){ 
		for ( i = 0; i < DMA_BUF_SIZE; i++ ){ 
			writeb(data, (char*) g_milb->buf_to_dev + i);
		}

		g_milb->dma_handle1 = pci_map_single(g_milb->pdev,g_milb->buf_to_dev, DMA_BUF_SIZE, PCI_DMA_TODEVICE);
		if(pci_dma_mapping_error(g_milb->pdev, g_milb->dma_handle1))
			pr_info("milb: pci map single error\n");

		milb_dma_init(g_milb, PCIE_DMA_DIR_DTOS);
	}
	return 0;
}

static ssize_t show_all_reg(struct device *d, struct device_attribute *attr, char *buf)
{
	milb_dump_all(g_milb);
	return 0;
}
static DEVICE_ATTR(regdump, S_IWUSR | S_IRUGO, NULL, store_regdump);
static DEVICE_ATTR(dmaRtest, S_IWUSR | S_IRUGO, dmaR, NULL);
static DEVICE_ATTR(dmaWtest, S_IWUSR | S_IRUGO, dmaW, NULL);
static DEVICE_ATTR(dumpall, S_IWUSR | S_IRUGO, show_all_reg, NULL);

static struct attribute* milb_attributes[] = {
	&dev_attr_regdump.attr,
	&dev_attr_dmaRtest.attr,
	&dev_attr_dmaWtest.attr,
	&dev_attr_dumpall.attr,
	NULL
};
static const struct attribute_group milb_group = {
	.attrs = milb_attributes,
};

/**
 * function: milb_test_init()                           
 *
 * this function will initial registers for PCIe access 
 */
static int milb_test_init(struct milb_m10v * milb){
	struct pci_dev *dev = milb->pdev;
	int ret;
	u32 tmp;

	milb->msi_addr = TARGET_ADDR;
	/*set region 0 as current region*/
	//0x"8"000 000"0" means region 0 as current; bit31 means region dir(0:ob; 1:ib)
	pci_write_config_dword(dev, IATU_VIEWPORT_OFF, 0x80000000);//0x900
	ret = pci_read_config_dword(dev, IATU_VIEWPORT_OFF, &tmp);
	pr_info("milb: viewport/region idx: 0x%08x\n", tmp);

	/*target addr*/
	pci_write_config_dword(dev, IO_PPL_ILTAO, milb->msi_addr);//lower target addr
	ret = pci_read_config_dword(dev, IO_PPL_ILTAO, &tmp); //0x918
	pr_info("milb: lower target addr: 0x%08x\n", tmp);

	pci_write_config_dword(dev, IO_PPL_IUTA, 0x00000000);//upper target addr
	ret = pci_read_config_dword(dev, IO_PPL_IUTA, &tmp);//0x91c
	pr_info("milb: upper target addr: 0x%08x\n", tmp);

	/*xls: iatu region ctrl 1 off */
	pci_write_config_dword(dev, IO_PPL_IRC1, 0x00000000);//0x0000 000"0", 0 means mem; 2 means i/o
	ret = pci_read_config_dword(dev, IO_PPL_IRC1, &tmp);//0x904
	pr_info("milb: IO_PPL_IRC1: 0x%08x\n", tmp);

	/*enable region*/
	pci_write_config_dword(dev, IO_PPL_IRC2, 0xc0000000);//c(bit31)means enable region 
	ret = pci_read_config_dword(dev, IO_PPL_IRC2, &tmp);//0x908
	pr_info("milb: enable region: 0x%08x\n", tmp);
	
	return 0;
}

/**
 * function: milb_test_cpumode()                 
 *
 * this function will do PCIe read and write via BAR
 * and it's non-DMA mode   			
 */
static int milb_test_cpumode(struct milb_m10v *milb){
	int i;
	pr_info("milb: %s\n", __func__);

	milb_test_init(milb);
	rc_ddr_test_region_size = milb->io_len;

	/* Prepare the test regions and data */	
	rc_ddr_test_reg1 = kzalloc(rc_ddr_test_region_size, GFP_KERNEL);
	if (!rc_ddr_test_reg1)
		pr_err("milb: PCIe EP: can't alloc the test region1.\n");

	rc_ddr_test_reg2 = kzalloc(rc_ddr_test_region_size, GFP_KERNEL);
	if (!rc_ddr_test_reg2) {
		kfree(rc_ddr_test_reg1);
		pr_err("milb: PCIe EP: can't alloc the test region2.\n");
	}

	for (i = 0; i < rc_ddr_test_region_size; i++) {
		writeb(0x55, (char*)rc_ddr_test_reg1 + i);
		writeb(0x55, (char*)rc_ddr_test_reg2 + i);
	}
	if (memcmp(rc_ddr_test_reg2, rc_ddr_test_reg1, rc_ddr_test_region_size) != 0) 
		pr_info("milb: PCIe EP: initial memcmp failed\n");

	/* PCIe EP start the data transfer after link up */
	pr_info("milb: PCIe: Starting data transfer...\n");
	do_gettimeofday(&tv1);

	/* write data*/
	for (i = 0; i < rc_ddr_test_region_size; i++) {
		writeb(*((char*)(rc_ddr_test_reg1) + i), (char*)milb->base_addr + i);
	}

	do_gettimeofday(&tv2);

	for (i = 0; i < rc_ddr_test_region_size; i++) {
		*((char*)rc_ddr_test_reg2 + i) = readb((char*)milb->base_addr + i);
	}

	do_gettimeofday(&tv3);

	if (memcmp(rc_ddr_test_reg2, rc_ddr_test_reg1, rc_ddr_test_region_size) != 0) {
		pr_info("milb: PCIe memcmp is failed.\n");
	} else {
		tv_count1 = (tv2.tv_sec - tv1.tv_sec) * USEC_PER_SEC
			+ tv2.tv_usec - tv1.tv_usec;
		tv_count2 = (tv3.tv_sec - tv2.tv_sec) * USEC_PER_SEC
			+ tv3.tv_usec - tv2.tv_usec;

		pr_info("milb: PCIe memcmp is successful. cpu mode\n");
		pr_info("milb: PCIe: Data write speed is %ldMB/s.\n",
				((((rc_ddr_test_region_size/1024) * MSEC_PER_SEC)))
				/(tv_count1));
		pr_info("milb: PCIe: Data read speed is %ldMB/s.\n",
				((((rc_ddr_test_region_size/1024) * MSEC_PER_SEC)))
				/(tv_count2));
	}
	return 0;
}

/**
 * function: milb_dma_common_init()                    
 *
 * this function will initial registers for the common
 * part of DAM read and write				
 */
static int milb_dma_common_init(struct milb_m10v * milb){
	milb->buf_to_dev = kzalloc(DMA_BUF_SIZE, GFP_KERNEL | GFP_DMA);
	milb->buf_from_dev = kzalloc(DMA_BUF_SIZE, GFP_KERNEL | GFP_DMA);

	//remote interrupt enable
//	pci_write_config_dword(milb->pdev, DMA_CH_CONTROL1_OFF_WRCH0, 0x00000068);//0xa70
	pci_write_config_dword(milb->pdev, DMA_WRITE_INT_STATUS_OFF, 0xffffffff);//0x9bc write status
	pci_write_config_dword(milb->pdev, DMA_READ_INT_STATUS_OFF, 0xffffffff);//0xa10 read status
	pci_write_config_dword(milb->pdev, MSI_CTRL_INT_0_EN_OFF, 0xFFFFFFFF);//0x828 MSI_CTRL_INT_0_EN_OFF
	pci_write_config_dword(milb->pdev, MSI_CTRL_INT_0_MASK_OFF, 0);//0x82c msi mask
	pci_write_config_dword(milb->pdev, DMA_WRITE_ENGINE_EN_OFF, 0x00000001); /*write engine enable*/
	pci_write_config_dword(milb->pdev, DMA_READ_ENGINE_EN_OFF, 0x00000001); /*read engine enable*/

	return 0;
}

/**
 * function: milb_dma_init()
 *  
 * this function will initial registers based on DMA
 * read or write. and start to DMA transfer finially.
 */
static int milb_dma_init(struct milb_m10v *milb, int dir){

	milb->msi_addr = TARGET_ADDR;
	
	pci_write_config_dword(milb->pdev, DMA_WRITE_INT_CLEAR_OFF, 0xffffffff);//0x9c8 write int clear
	pci_write_config_dword(milb->pdev, DMA_READ_INT_CLEAR_OFF, 0xffffffff);//0xa1c read int clear

	if(dir==PCIE_DMA_DIR_STOD){//write, test ok
		pci_write_config_dword(milb->pdev, DMA_VIEWPORT_SEL_OFF, 0x00000000); /*channel direction - write*/
	}else{//read
		pci_write_config_dword(milb->pdev, DMA_VIEWPORT_SEL_OFF, 0x80000000); /*channel direction - read, 8 means dir, 1:ch*/
	}

	pci_write_config_dword(milb->pdev, DMA_TRANSFER_SIZE_OFFWRCH0, DMA_BUF_SIZE); //trans size

	if(dir==PCIE_DMA_DIR_STOD){//write(ep to rc), test ok
		pci_write_config_dword(milb->pdev, DMA_SAR_LOW_OFF_WRCH0, milb->msi_addr); //src addr low
		pci_write_config_dword(milb->pdev, DMA_SAR_HIGHT_OFF_WRCH0, 0x00000000); //src addr high
		pci_write_config_dword(milb->pdev, DMA_DAR_LOW_WRCH0, milb->dma_handle2); //dst addr low
		pci_write_config_dword(milb->pdev, DMA_DAR_HIGH_OFF_WRCH0, 0x00000000); //dst addr high
	}else{	//read (rc to ep)
		pci_write_config_dword(milb->pdev, DMA_SAR_LOW_OFF_WRCH0, milb->dma_handle1); //src addr low
		pci_write_config_dword(milb->pdev, DMA_SAR_HIGHT_OFF_WRCH0, 0x00000000); //src addr high
		pci_write_config_dword(milb->pdev, DMA_DAR_LOW_WRCH0, milb->msi_addr); //dst addr low
		pci_write_config_dword(milb->pdev, DMA_DAR_HIGH_OFF_WRCH0, 0x00000000); //dst addr high
	}

	pci_write_config_dword(milb->pdev, DMA_WRITE_INT_MASK_OFF, 0x00000000);//0x9c4 write mask
	pci_write_config_dword(milb->pdev, DMA_READ_INT_MASK_OFF, 0x00000000);//0xa18 read mask
	pci_write_config_dword(milb->pdev, DMA_WRITE_INT_CLEAR_OFF, 0xffffffff);//write clear
	pci_write_config_dword(milb->pdev, DMA_READ_INT_CLEAR_OFF, 0xffffffff);//read clear
	pci_write_config_dword(milb->pdev, DMA_CH_CONTROL1_OFF_WRCH0, 0x00000068);//0xa70

	//start dma
	if(dir==PCIE_DMA_DIR_STOD){//write, test ok
		do_gettimeofday(&tv3); //for rc, it start to read
		pci_write_config_byte(milb->pdev, DMA_WRITE_DOORBELL_OFF, 0x00000000);
	}else{ //read
		//for rc, it starts to write
		do_gettimeofday(&tv1);
		pci_write_config_byte(milb->pdev, DMA_READ_DOORBELL_OFF, 0x00000000);//ch1
	}
	
	return 0;
}

/**
 * function: milb_test_dmamode()			
 *
 * this function is the entry point for DMA transfer	
 * it will trigger a DMA write from rc to ep(Buffer A)	 
 * then DMA read from ep to rc(Buffer B). 		
 * and finally memory compare Buffer A and B		
 */
static int milb_test_dmamode(struct milb_m10v *milb)
{
	int i;

	u8 data;

	pr_info("milb: start to test dma\n");
	if(milb->buf_to_dev == NULL || milb->buf_from_dev == NULL){
		pr_info("milb: dma buf or tmpbuf is null \n");
	}

	//initial buf as random
	do_gettimeofday(&tv99);
	data = (u8)tv99.tv_sec;
	pr_info("milb: data to ep:%02x\n", data);
	if(milb->buf_to_dev){
		for ( i = 0; i < DMA_BUF_SIZE; i++ ){
			writeb(data, (char*) milb->buf_to_dev + i);
		}
	}
	pr_info("milb: initial buf as = 0x%02x\n", readb((char*)milb->buf_to_dev));


	//1. write from RC to EP
	milb->dma_handle1 = pci_map_single(milb->pdev, milb->buf_to_dev, DMA_BUF_SIZE, PCI_DMA_TODEVICE);
	if(pci_dma_mapping_error(milb->pdev, milb->dma_handle1))
		pr_info("milb: pci map single error 1 \n");

	milb_dma_init(milb, PCIE_DMA_DIR_DTOS);//for ep, it's "read" from rc side
	
	return 0;
}

/**
 * function: m10v_intr_msi()                            
 *
 * this is a MSI ISR, it'll trigger DMA read		
 * and memory comparison				
 */
static irqreturn_t m10v_intr_msi(int irq, void* data)
{		
	struct milb_m10v *milb = data;
	//u32 tmp;
	u32 wintr, rintr;

	pci_read_config_dword(milb->pdev, DMA_WRITE_INT_STATUS_OFF, &wintr);//0x9bc
	//pr_info("milb: dma write int status - 0x9bc: 0x%08x\n", tmp);

	pci_read_config_dword(milb->pdev, DMA_READ_INT_STATUS_OFF, &rintr);//0xa10
	//pr_info("milb: dma write int status - 0xa10: 0x%08x\n", tmp);

	if((wintr & BIT(0)) && (rintr & BIT(0))){
		pr_info("milb: both w/r are trigged \n");
	
	}else if(wintr & BIT(0)){
		do_gettimeofday(&tv4);
		pr_info("milb: read done interrupt \n");//for rc, it's read
		pci_write_config_dword(milb->pdev, DMA_WRITE_INT_CLEAR_OFF, 0xFFFFFFFF);
		pci_unmap_single(milb->pdev, milb->dma_handle2, DMA_BUF_SIZE, PCI_DMA_FROMDEVICE);
		//if need to cmp
		if(milb->need_dma_cmp){
			if (memcmp(milb->buf_to_dev, milb->buf_from_dev,  DMA_BUF_SIZE) != 0) {
				pr_info("milb: PCIe dma memcpy failed.\n");
			}else{
				pr_info("milb: first:%02x, last:%02x \n", readb(milb->buf_from_dev), readb(milb->buf_from_dev+DMA_BUF_SIZE-1));
				pr_info("milb: PCIe dma memcpy is successful\n");
				tv_count1 = (tv2.tv_sec - tv1.tv_sec) * USEC_PER_SEC
					+ tv2.tv_usec - tv1.tv_usec;
				tv_count2 = (tv4.tv_sec - tv3.tv_sec) * USEC_PER_SEC
					+ tv4.tv_usec - tv3.tv_usec;

				pr_info("milb: PCIe: Data write speed is %ldMB/s.\n",
						((((DMA_BUF_SIZE/1024) * MSEC_PER_SEC)))
						/(tv_count1));
				pr_info("milb: PCIe: Data read speed is %ldMB/s.\n",
						((((DMA_BUF_SIZE/1024) * MSEC_PER_SEC)))
						/(tv_count2));
				
			}
			milb->need_dma_cmp = 0;
		}
	} else if(rintr & BIT(0)){
		do_gettimeofday(&tv2); //write done timestamp
		pr_info("milb: write done interrupt \n");//for rc, it's write
		pci_write_config_dword(milb->pdev, DMA_READ_INT_CLEAR_OFF, 0xFFFFFFFF);
		pci_unmap_single(milb->pdev, milb->dma_handle1, DMA_BUF_SIZE, PCI_DMA_TODEVICE);
		if(milb->need_dma_cmp)
			schedule_delayed_work(&milb->work, 0);	//schedule a work for dma read
	}
	pr_info("**********milb: %s done**********\n", __func__);
	return IRQ_HANDLED;
}

/**
 * function: dmar_work()	        
 *                
 * this workqueue function works as a DMA read		
 * and it'll be called after a DMA write		
 * so, the step will be:				
 * 1. DMA write	-->	2. DMA read	
 */
static void dmar_work(struct work_struct *work)
{
	struct milb_m10v *milb =
		container_of(work, struct milb_m10v,
					 work.work);
	//pr_info("milb: dmar_work\n");

	//1. read from EP to RC
	if(milb->buf_from_dev){
		milb->dma_handle2 = pci_map_single(milb->pdev, milb->buf_from_dev, DMA_BUF_SIZE, PCI_DMA_FROMDEVICE);
		if(pci_dma_mapping_error(milb->pdev, milb->dma_handle2))
			pr_info("milb: pci map single error\n");

		milb_dma_init(milb, PCIE_DMA_DIR_STOD);//for ep, it's "write" from rc side
	}
}

static int milb_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	struct milb_m10v *milb;
	int retval=0;
	int ret = 0;
	int bar_idx;
	
	bar_idx = 0;
	pr_info("milb: %s()\n", __func__);

	milb = kzalloc(sizeof(struct milb_m10v), GFP_KERNEL);
	milb->pdev = dev;

	ret = sysfs_create_group(&dev->dev.kobj, &milb_group);
	if(ret)
		pr_info("milb: can't create sysfs \n");	

	if(pci_assign_resource(dev, bar_idx)){
		pr_err("milb: failed to assign resource 0 \n");
		return -ENODEV;
	}

	if(pci_assign_resource(dev, 2)){
		pr_err("milb: failed to assign resource 2\n");
		return -ENODEV;
	}

	if(pci_assign_resource(dev, 4)){
		pr_err("milb: failed to assign resource 4 \n");
		return -ENODEV;
	}


	if (pci_enable_device(dev)){
		pr_err("milb: pci enable device failed  - %s \n", pci_name(dev));
		return -ENODEV;
	}


	milb->io_len = pci_resource_len(milb->pdev, bar_idx);
	pr_info("milb: io_len = 0x%08x , bar_idx = %d\n", milb->io_len, bar_idx);

	/*request IO regions*/
	ret = pci_request_regions(dev, milbpci_name);
	if(ret){
		pr_err("milb: failed to request regions \n");
	}

	milb->base_addr = pci_iomap(milb->pdev, bar_idx, milb->io_len);
	if(!milb->base_addr){
		pr_err("milb: failed to iomap \n");
		goto out_release_mem_region;
	}

	//pci_set_master(dev);
	if (!pci_set_dma_mask(milb->pdev, DMA_BIT_MASK(32))
		&& !pci_set_consistent_dma_mask(milb->pdev,
										DMA_BIT_MASK(32))) {
		milb->dma_mask = DMA_BIT_MASK(32);
		pr_info("milb: 32 BIT PCI BUS DMA ADDRESSING SUPPORTED\n");
	}

	pci_disable_msi(milb->pdev);
	if(pci_enable_msi(milb->pdev)){
		pr_err("milb: failed to enable msi \n");
	}
	
	/* request and enable interrupt */
	ret = request_irq(milb->pdev->irq, m10v_intr_msi, IRQF_SHARED, milbpci_name, milb);
	if(ret)
		pr_info("milb: unable to allocate interrupt, error: %d\n", ret);

	INIT_DELAYED_WORK(&milb->work, dmar_work);
	//schedule_delayed_work(&milb->work, HZ / 2);

	/*cpumode test*/
	milb->need_dma_cmp = 1;
	milb_dma_common_init(milb);
	milb_test_cpumode(milb);

	/*dma init*/
	//	milb_test_dmamode(milb);

	g_milb = milb;//WA for unknow crash issue.

out_release_mem_region:
	pci_release_regions(dev);

	return retval;
}

static void milb_pci_remove(struct pci_dev *dev)
{
	sysfs_remove_group(&dev->dev.kobj, &milb_group);
	disable_irq(dev->irq);
	pci_release_regions(dev);
	pci_disable_device(dev);
}

static const struct pci_device_id pci_ids[] = { {
		PCI_DEVICE_CLASS(PCI_CLASS_BRIDGE_PCI, 0),
		PCI_DEVICE(0x50C1, 0xabcd),
	},
	{ /* end: all zeroes */ }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static struct pci_driver milb_pci_driver = {
	.name =		milbpci_name,
	.id_table =	pci_ids,

	.probe =	milb_pci_probe,
	.remove =	milb_pci_remove,
};

static int __init milb_pci_init(void)
{
	return pci_register_driver(&milb_pci_driver);
}
module_init(milb_pci_init);


static void __exit milb_pci_exit(void)
{
	pci_unregister_driver(&milb_pci_driver);
}
module_exit(milb_pci_exit);

MODULE_AUTHOR("caspar lin <caspar.lin@socionext.com>");
MODULE_DESCRIPTION("milb M10V PCI Driver");
MODULE_LICENSE("GPL");

#endif

