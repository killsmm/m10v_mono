/*
 * Copyright 2011, Marvell Semiconductor Inc.
 * Lei Wen <leiwen@marvell.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Back ported to the 8xx platform (from the 8260 platform) by
 * Murray.Jensen@cmst.csiro.au, 27-Jan-01.
 */

#include "mmc.h"
#include "sdhci.h"
#include "types.h"
#include "asm/utils.h"
#include "bitops.h"
#include "asm/armv7.h"
#include "cache_v7.h"

#ifdef CONFIG_MLB01
#define IO_SDIO_WP_CD_LED_SET		0x0210
#define IO_SDIO_SOFT_RESET		0x0200
#define IO_SDIO_CR_SET			0x0220
#define  SDIO_CR_SET_CR_TOCLKUNIT       (1 << 24)
#define  SDIO_CR_SET_CR_TOCLKFREQ_SFT   1
#define  SDIO_CR_SET_CR_TOCLKFREQ_MASK  (0x3F << SDIO_CR_SET_CR_TOCLKFREQ_SFT)
#define  SDIO_CR_SET_CR_BCLKFREQ_SFT    8
#define  SDIO_CR_SET_CR_BCLKFREQ_MASK   (0xFF << SDIO_CR_SET_CR_BCLKFREQ_SFT)
#define  SDIO_CR_SET_CR_RTUNTIMER_SFT   4
#endif


#ifdef CONFIG_MMC_ADMA
static unsigned int sdhci_adma_table_pre(struct mmc *mmc, struct mmc_data *data, unsigned int start_addr)
{
	struct adma_descriptor {
		unsigned int attr;
		unsigned int addr;
	};

	int i = 0;
	unsigned int total;
	unsigned int offset;
	struct adma_descriptor *table;

	if (data->blocks == 0)
		return 0;

	total = data->blocks * data->blocksize;
	offset = 0;

	table = malloc((total / 0x10000 + 1) * sizeof(struct adma_descriptor));
	if (table == NULL)
		return 0;

	while (offset < total) {
		unsigned int size = total - offset;
		if (size >= 0x10000)
			size = 0x10000;

		table[i].addr = start_addr + offset;
		table[i].attr = (size << 16) | 0x21;

		offset += size;
		i++;
	}

	table[i - 1].attr |= 0x2;

	/* cache write */
	flush_cache((unsigned long)table, i * sizeof(struct adma_descriptor));

	return (unsigned int)table;
}

static void sdhci_adma_table_post(unsigned int adma_table)
{
	/* No need to really free it in stubby */
	if (adma_table != 0)
		adma_table = 0;
}
#endif

static void sdhci_reset(struct sdhci_host *host, u8 mask)
{
	unsigned long timeout;
	u32 val;

	sdhci_writeb(host, sdhci_readb(host, SDHCI_CLOCK_CONTROL) |
		     1, SDHCI_CLOCK_CONTROL);
	/* Wait max 100 ms */
	timeout = 100;
	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("Reset 0x%x never completed.\n", (int)mask);
			return;
		}
		timeout--;
		udelay(1000);
	}
}

static void sdhci_cmd_done(struct sdhci_host *host, struct mmc_cmd *cmd)
{
	int i;
	if (cmd->resp_type & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			cmd->response[i] = sdhci_readl(host,
					SDHCI_RESPONSE + (3-i)*4) << 8;
			if (i != 3)
				cmd->response[i] |= sdhci_readb(host,
						SDHCI_RESPONSE + (3-i)*4-1);
		}
	} else {
		cmd->response[0] = sdhci_readl(host, SDHCI_RESPONSE);
	}
}

static void sdhci_transfer_pio(struct sdhci_host *host, struct mmc_data *data)
{
	int i;
	char *offs;
	for (i = 0; i < data->blocksize; i += 4) {
		offs = data->dest + i;
		if (data->flags == MMC_DATA_READ)
			*(u32 *)offs = sdhci_readl(host, SDHCI_BUFFER);
		else
			sdhci_writel(host, *(u32 *)offs, SDHCI_BUFFER);
	}
}

static int sdhci_transfer_data(struct sdhci_host *host, struct mmc_data *data,
				unsigned int start_addr)
{
	unsigned int stat, rdy, mask, timeout, block = 0;
#if 0  /* Causes "Error detected in status(0x108100)!" on MB8AC0300 */
#ifdef CONFIG_MMC_SDMA
	unsigned char ctrl;
	ctrl = sdhci_readl(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	ctrl |= SDHCI_CTRL_SDMA;
	sdhci_writel(host, ctrl, SDHCI_HOST_CONTROL);
#endif
#endif
	timeout = 1000000;
	rdy = SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL;
	mask = SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE;
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			puts(" a error status\n");
			print32(stat);
			putc('\n');
			return -1;
		}
		if (stat & rdy) {
			if (!(sdhci_readl(host, SDHCI_PRESENT_STATE) & mask))
				continue;
			sdhci_writel(host, rdy, SDHCI_INT_STATUS);
			sdhci_transfer_pio(host, data);
			data->dest += data->blocksize;
#ifdef CONFIG_F_SDH30_SDHCI
			/*  
			 * It's potential that the SDHCI_INT_DATA_END bit
			 * is not set when the real transfer data is completed.
			 * We check the SDHCI_INT_DATA_END is set before quit.
			 */
			if (++block >= data->blocks) {
				unsigned int timeout_cnt = 10000;
				while (1) {
					stat = sdhci_readl(host,
							   SDHCI_INT_STATUS);
					if (stat & SDHCI_INT_ERROR) {
						puts(" b error status\n");
						print32(stat);
						putc('\n');
						return -1;
					}
					
					if (stat & SDHCI_INT_DATA_END)
						break;

					/* kind of paranoia for checking */
					if (timeout_cnt-- > 0)
						udelay(10);
					else {
						puts("timeout1\n");
						return -1;
					}
				}
				break;
			}
#else
			if (++block >= data->blocks)
				break;
#endif
		}
#ifdef CONFIG_MMC_SDMA
		if (stat & SDHCI_INT_DMA_END) {
			sdhci_writel(host, SDHCI_INT_DMA_END, SDHCI_INT_STATUS);
			start_addr &= ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1);
			start_addr += SDHCI_DEFAULT_BOUNDARY_SIZE;
			sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		}
#endif
		if (timeout-- > 0)
			udelay(10);
		else {
			puts("timeout2\n");
			return -1;
		}
	} while (!(stat & SDHCI_INT_DATA_END));
	return 0;
}

int sdhci_send_command(struct mmc *mmc, struct mmc_cmd *cmd,
		       struct mmc_data *data)
{
	struct sdhci_host *host = mmc->priv;
	unsigned int stat = 0;
	int ret = 0;
	int trans_bytes = 0, is_aligned = 1;
	u32 mask, flags, mode;
	unsigned int timeout, start_addr = 0;
	unsigned int retry = 10000;
	void *aligned_buffer = (void *)ALIGNED_BUFFER_ADDR;
	unsigned int adma_table = 0;

	/* Wait max 10 ms */
	timeout = 10;

	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		mask &= ~SDHCI_DATA_INHIBIT;

	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & mask) {
		if (timeout == 0) {
			puts("z\n");
			return COMM_ERR;
		}
		timeout--;
		udelay(1000);
	}

	mask = SDHCI_INT_RESPONSE;
	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (cmd->resp_type & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		mask |= SDHCI_INT_DATA_END;
	} else
		flags = SDHCI_CMD_RESP_SHORT;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data)
		flags |= SDHCI_CMD_DATA;
	/* CMD19 is special in that the Data Present Select should be set */
	if (cmd->cmdidx== MMC_CMD_SEND_TUNING_BLOCK ||
	    cmd->cmdidx== MMC_CMD_SEND_TUNING_BLOCK_HS200) {
		flags |= SDHCI_CMD_DATA;
		mask = SDHCI_INT_DATA_AVAIL;
	}

	/*Set Transfer mode regarding to data flag*/
	if (data) {
		unsigned int ctrl;

		sdhci_writeb(host, 0xb, SDHCI_TIMEOUT_CONTROL);

#ifdef CONFIG_F_SDH30_SDHCI
		mode = 0;
#else
		mode = SDHCI_TRNS_BLK_CNT_EN;
#endif
		trans_bytes = data->blocks * data->blocksize;
		if (data->blocks > 1) {
			mode |= SDHCI_TRNS_MULTI;
			mode |= SDHCI_TRNS_BLK_CNT_EN;

			if (host->quirks & SDHCI_QUIRK_AUTO_CMD23) {
				if (!IS_SD(mmc) || mmc->scr[0] & 0x2) {
					mode |= SDHCI_TRNS_ACMD23; 
					sdhci_writel(host, data->blocks, SDHCI_ARGUMENT2);
				}
			} else if (host->quirks & SDHCI_QUIRK_AUTO_CMD12) {
				mode |= SDHCI_TRNS_ACMD12;
			}
		}

		if (data->flags == MMC_DATA_READ)
			mode |= SDHCI_TRNS_READ;

#if defined(CONFIG_MMC_SDMA) || defined(CONFIG_MMC_ADMA)
		if (data->flags == MMC_DATA_READ)
			start_addr = (unsigned int)data->dest;
		else
			start_addr = (unsigned int)data->src;
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
				(start_addr & 0x7) != 0x0) {
			is_aligned = 0;
			start_addr = (unsigned int)aligned_buffer;
			if (data->flags != MMC_DATA_READ)
				memcpy(aligned_buffer, data->src, trans_bytes);
		}

		mode |= SDHCI_TRNS_DMA;

#ifdef CONFIG_MMC_ADMA
		ctrl = sdhci_readl(host, SDHCI_HOST_CONTROL);
		ctrl &= ~SDHCI_CTRL_DMA_MASK;
		ctrl |= SDHCI_CTRL_ADMA32;
		sdhci_writel(host, ctrl, SDHCI_HOST_CONTROL);

		adma_table = sdhci_adma_table_pre(mmc, data, start_addr);
		sdhci_writel(host, adma_table, SDHCI_ADMA_ADDRESS);
#else
		sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
#endif
#endif

		sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
				data->blocksize),
				SDHCI_BLOCK_SIZE);
		sdhci_writew(host, data->blocks, SDHCI_BLOCK_COUNT);
		sdhci_writew(host, mode, SDHCI_TRANSFER_MODE);
	}

	sdhci_writel(host, cmd->cmdarg, SDHCI_ARGUMENT);
#if defined(CONFIG_MMC_SDMA) || defined(CONFIG_MMC_ADMA)
	flush_cache(start_addr, trans_bytes);
#endif
	sdhci_writew(host, SDHCI_MAKE_CMD(cmd->cmdidx, flags), SDHCI_COMMAND);
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR)
			break;
	} while ((stat & mask) != mask);

	if (--retry == 0) {
#ifdef CONFIG_MMC_ADMA
		sdhci_adma_table_post(adma_table);
#endif
		if (host->quirks & SDHCI_QUIRK_BROKEN_R1B)
			return 0;
		else {
			puts("z2\n");
			return TIMEOUT;
		}
	}

	if ((stat & (SDHCI_INT_ERROR | mask)) == mask) {
		sdhci_cmd_done(host, cmd);
		sdhci_writel(host, mask, SDHCI_INT_STATUS);
	} else {
		ret = -1;
//		puts("error mask\n");
//		print32(stat);
//		puts("\n");
	}

	if (!ret && data) {
		ret = sdhci_transfer_data(host, data, start_addr);
		if (ret)
			puts("txfer fail\n");
	}

	if (host->quirks & SDHCI_QUIRK_WAIT_SEND_CMD)
		udelay(2000);

#ifdef CONFIG_MMC_ADMA
	sdhci_adma_table_post(adma_table);
#endif

	stat = sdhci_readl(host, SDHCI_INT_STATUS);
	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	if (!ret) {
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
				!is_aligned && (data->flags == MMC_DATA_READ))
			memcpy(data->dest, aligned_buffer, trans_bytes);
		return 0;
	}

	sdhci_reset(host, SDHCI_RESET_CMD);
	sdhci_reset(host, SDHCI_RESET_DATA);
	if (stat & SDHCI_INT_TIMEOUT) {
	//	puts("timeout\n");
		return TIMEOUT;
	}
	else {
//		puts("z3\n");
		return COMM_ERR;
	}
}

static int sdhci_set_clock(struct mmc *mmc, unsigned int clock)
{
	struct sdhci_host *host = mmc->priv;
	unsigned int div, clk, timeout;

	sdhci_writew(host, 0, SDHCI_CLOCK_CONTROL);

	if (clock == 0)
		return 0;

#ifdef CONFIG_MLB01
	/* !!! */
	if (clock > 32000000)
		clock = 32000000;
#endif

	if ((host->version & SDHCI_SPEC_VER_MASK) >= SDHCI_SPEC_300) {
		/* Version 3.00 divisors must be a multiple of 2. */
		if (mmc->f_max <= clock)
			div = 1;
		else {
			for (div = 2; div < SDHCI_MAX_DIV_SPEC_300; div += 2) {
				if ((mmc->f_max / div) <= clock)
					break;
			}
		}
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
			if ((mmc->f_max / div) <= clock)
				break;
		}
	}
	div >>= 1;

	clk = (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
	clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
		<< SDHCI_DIVIDER_HI_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

#if 0
	puts("clock = ");
	printdec(clock);
	puts(", div = ");
	printdec(div);
	puts(", mmc->f_max = ");
	printdec(mmc->f_max);
	puts("\n");
#endif
	/* Wait max 20 ms */
	timeout = 20;
	do {
		clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
		if (timeout != 20)
			udelay(1000);
	}
	while (--timeout && !(clk & SDHCI_CLOCK_INT_STABLE));
	if (!timeout) {
		puts("clk fail\n");
		return -1;
	}

	clk |= SDHCI_CLOCK_CARD_EN;
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

	return 0;
}

static void sdhci_set_power(struct sdhci_host *host, unsigned short power)
{
	u8 pwr = 0;

	if (power != (unsigned short)-1) {
		switch (1 << power) {
		case MMC_VDD_165_195:
			pwr = SDHCI_POWER_180;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			pwr = SDHCI_POWER_300;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			pwr = SDHCI_POWER_330;
			break;
		}
	}

	if (pwr == 0) {
		sdhci_writeb(host, 0, SDHCI_POWER_CONTROL);
		return;
	}

	pwr |= SDHCI_POWER_ON;

	sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);
}

int sdhci_set_signal_voltage(struct mmc *mmc, int voltage)
{
	u16 ctrl;
	u16 en_low_voltage = 0;
	struct sdhci_host *host = mmc->priv;
	
	ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);

	switch (voltage) {
		case MMC_SELECT_VDD_180:
			ctrl |= MMC_SELECT_VDD_180;
			en_low_voltage = MMC_SELECT_VDD_180;
			break;
		case MMC_SELECT_VDD_330:
			ctrl &= ~MMC_SELECT_VDD_180;
			en_low_voltage = 0;
			break;
		default:
			return -2;
			break;
	}

	
	sdhci_writew(host, ctrl, SDHCI_HOST_CONTROL2);
	
	/* Wait for 5ms */
	udelay(5000);

	/* 1.8V or 3.3V regulator output should be stable within 5 ms */
	ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	if ((ctrl & SDHCI_CTRL_VDD_180) == en_low_voltage)
		return 0;

	return -1;
}

#define MAX_TUNING_LOOP 40

int sdhci_execute_tuning(struct mmc *mmc, u32 opcode)
{
	struct sdhci_host *host = mmc->priv;
	u16 ctrl;
	int tuning_loop_counter = MAX_TUNING_LOOP;
	unsigned long timeout;
	int err = 0;

	puts("  execute tuning\n");

#ifdef CONFIG_F_SDH30_SDHCI
	/* hack for s73 */
	u32 vendor_ctrl;
	vendor_ctrl = sdhci_readl(host,SDHCI_VENDOR_CTRL);
	vendor_ctrl |= 0x01000000;
	sdhci_writel(host, vendor_ctrl, SDHCI_VENDOR_CTRL);

	/* tuning setting: disable CMD conflict */
	vendor_ctrl = sdhci_readl(host,SDHCI_TUNING_SETTING);
	vendor_ctrl |= 0x00010000;
	sdhci_writel(host, vendor_ctrl, SDHCI_TUNING_SETTING);
#endif

	ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);

	ctrl |= SDHCI_CTRL_EXEC_TUNING;
#ifdef CONFIG_F_SDH30_SDHCI
	/* workaround for tuning process */
	ctrl |= SDHCI_CTRL_TUNED_CLK;
#endif
	sdhci_writew(host, ctrl, SDHCI_HOST_CONTROL2);

	/*
	 * Issue CMD19 repeatedly till Execute Tuning is set to 0 or the number
	 * of loops reaches 40 times or a timeout of 150ms occurs.
	 */
	timeout = 150;
	do {
		struct mmc_cmd cmd;

		if (!tuning_loop_counter && !timeout)
			break;

		cmd.cmdidx = opcode;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1 | R1_APP_CMD;

		/*
		 * In response to CMD19, the card sends 64 bytes of tuning
		 * block to the Host Controller. So we set the block size
		 * to 64 here.
		 */
		if (cmd.cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200) {
			if (mmc->bus_width == 8)
				sdhci_writew(host, SDHCI_MAKE_BLKSZ(7, 128),
					     SDHCI_BLOCK_SIZE);
			else if (mmc->bus_width == 4)
				sdhci_writew(host, SDHCI_MAKE_BLKSZ(7, 64),
					     SDHCI_BLOCK_SIZE);
		} else {
			sdhci_writew(host, SDHCI_MAKE_BLKSZ(7, 64),
				     SDHCI_BLOCK_SIZE);
		}

		/*
		 * The tuning block is sent by the card to the host controller.
		 * So we set the TRNS_READ bit in the Transfer Mode register.
		 * This also takes care of setting DMA Enable and Multi Block
		 * Select in the same register to 0.
		 */
		sdhci_writew(host, SDHCI_TRNS_READ, SDHCI_TRANSFER_MODE);
		
		err = sdhci_send_command(mmc, &cmd, NULL);
		if (err) {
			printf("send tuning command fail:0x%x\n", err);
			return err;
		}		

		ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		tuning_loop_counter--;
		timeout--;
		udelay(1000);
	} while (ctrl & SDHCI_CTRL_EXEC_TUNING);

	/*
	 * The Host Driver has exhausted the maximum number of loops allowed,
	 * so use fixed sampling frequency.
	 */
	if (!tuning_loop_counter || !timeout) {
		ctrl &= ~SDHCI_CTRL_TUNED_CLK;
		sdhci_writew(host, ctrl, SDHCI_HOST_CONTROL2);
	} else {
		if (!(ctrl & SDHCI_CTRL_TUNED_CLK)) {
			printf("tuning timeout\n");
			err = -5;
		}
	}
	
	return err;
}

void sdhci_set_ios(struct mmc *mmc)
{
	u32 ctrl;
	struct sdhci_host *host = mmc->priv;

	if (mmc->clock != host->clock)
		sdhci_set_clock(mmc, mmc->clock);

	/* Set bus width */
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	if (mmc->bus_width == 8) {
		ctrl &= ~SDHCI_CTRL_4BITBUS;
		if ((host->version & SDHCI_SPEC_VER_MASK) >= SDHCI_SPEC_300)
			ctrl |= SDHCI_CTRL_8BITBUS;
	} else {
		if ((host->version & SDHCI_SPEC_VER_MASK) >= SDHCI_SPEC_300)
			ctrl &= ~SDHCI_CTRL_8BITBUS;
		if (mmc->bus_width == 4)
			ctrl |= SDHCI_CTRL_4BITBUS;
		else
			ctrl &= ~SDHCI_CTRL_4BITBUS;
	}

	if (mmc->clock > 26000000)
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;

	if (host->quirks & SDHCI_QUIRK_NO_HISPD_BIT)
		ctrl &= ~SDHCI_CTRL_HISPD;

	if (host->version >= SDHCI_SPEC_300) {
		u16 clk, ctrl_2;

		/* In case of UHS-I modes, set High Speed Enable */
		if (mmc->timing)
			ctrl |= SDHCI_CTRL_HISPD;

		ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);

		/* Reset SD Clock Enable */
		clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
		clk &= ~SDHCI_CLOCK_CARD_EN;
		sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

		sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
		
		ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		/* Select Bus Speed Mode for host */
		ctrl_2 &= ~SDHCI_CTRL_UHS_MASK;
		if (mmc->timing)
			ctrl_2 |= SDHCI_CTRL_HS_SDR200;
			
		sdhci_writew(host, ctrl_2, SDHCI_HOST_CONTROL2);

		/* Re-enable SD Clock */
		clk &= SDHCI_CLOCK_CARD_EN;
		sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
		sdhci_set_clock(mmc, mmc->clock);

	} else
		sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
}

int sdhci_sw_reset(struct mmc *mmc)
{
	struct sdhci_host *host = mmc->priv;
	
	sdhci_reset(host, SDHCI_RESET_ALL);
	
#if defined(CONFIG_F_SDH30_SDHCI) && !defined(CONFIG_MLB01)
	/* use vendor register to reset for emmc, sd has no effect doing this. */	
	sdhci_writel(host, 0x2, SDHCI_VENDOR_CTRL);
#endif

	return 0;
}

int sdhci_init(struct mmc *mmc)
{
	struct sdhci_host *host;

	host = mmc->priv;

	sdhci_reset(host, SDHCI_RESET_ALL);
#ifndef CONFIG_MLB01
	sdhci_writel(host, 0x2, SDHCI_VENDOR_CTRL);
#endif
	mmc_set_clock(mmc, mmc->f_min);


	sdhci_set_power(host, fls(mmc->voltages) - 1);

#ifdef CONFIG_F_SDH30_SDHCI
	/*
 	 * Reference to Part1 Physical Layer Simplified Specification Ver 3.01
	 * 6.4.1 Power Up
	 * This delay must be at least 74 clock sizes, or 1 ms.
 	 */
	udelay(1000);
#endif

//	while(1);

	if (!(sdhci_readl(host, SDHCI_PRESENT_STATE) & SDHCI_CARD_DETECT_PIN_LEVEL)) {
		puts("SD absent\n");
		return 1;
	}
	if (host->quirks & SDHCI_QUIRK_NO_CD) {
		unsigned int status;

		sdhci_writel(host, SDHCI_CTRL_CD_TEST_INS | SDHCI_CTRL_CD_TEST,
			SDHCI_HOST_CONTROL);

		status = sdhci_readl(host, SDHCI_PRESENT_STATE);
		while ((!(status & SDHCI_CARD_PRESENT)) ||
		    (!(status & SDHCI_CARD_STATE_STABLE)) ||
		    (!(status & SDHCI_CARD_DETECT_PIN_LEVEL)))
			status = sdhci_readl(host, SDHCI_PRESENT_STATE);
	}

	/* Eable all state */
	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_ENABLE);
	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_SIGNAL_ENABLE);

	return 0;
}

int add_sdhci(struct sdhci_host *host, u32 max_clk, u32 min_clk)
{
	unsigned int caps;
	struct mmc *mmc = &host->mmc;
	int val;

	unsigned int ddr76_data;

	mmc->version = 0;

#ifdef CONFIG_MLB01
	{
#if 0
		volatile u32 *p = (volatile u32 *)0x18002000;

		/* force PC2 high output to reset */

		p[(0x50 + 0x20) >> 2] = 4; /* set data */
#endif
		volatile u32 *p = (volatile u32 *)0x1D022000;
		p[(0x0100 + 0x0c) >> 2] = 0x00200000; /* get data */
		ddr76_data = p[(0x0100 + 0x10)>> 2];
		ddr76_data |= 0x00000020;
		p[(0x0100 + 0x0c) >> 2] = ddr76_data; /* set data */
					   
		p[(0x00 + 0x0c) >> 2] = 0x00200000;	/* set data */
	}

        /* hold the sdhci IP in reset */
        sdhci_writel(host, 0, IO_SDIO_SOFT_RESET);
        udelay(10);
        val = sdhci_readl(host, IO_SDIO_CR_SET);
        val &= ~(SDIO_CR_SET_CR_TOCLKFREQ_MASK | SDIO_CR_SET_CR_BCLKFREQ_MASK);
        val |= SDIO_CR_SET_CR_TOCLKUNIT | 
                       ((host->mmc.f_max / 16 / 1000000) << SDIO_CR_SET_CR_TOCLKFREQ_SFT);
        val |= ((host->mmc.f_max / 4 / 1000000) << SDIO_CR_SET_CR_BCLKFREQ_SFT);
        val |= ((host->mmc.f_max + 3999999) / 4 / 1000000) << SDIO_CR_SET_CR_RTUNTIMER_SFT;
        sdhci_writel(host, val, IO_SDIO_CR_SET);

        /* remove the reset on the sdhci IP */
        sdhci_writel(host, 1, IO_SDIO_SOFT_RESET);
#endif

	mmc->priv = host;
	caps = sdhci_readl(host, SDHCI_CAPABILITIES);
	sdhci_set_power(host, 0);

	if (!caps)
		caps = SDHCI_CAN_VDD_330;

#ifdef CONFIG_MMC_SDMA
	if (!(caps & SDHCI_CAN_DO_SDMA)) {
		puts("Your controller don't support sdma!!\n");
		return -1;
	}
#endif
	if (host->max_clk) {
		mmc->f_max = host->max_clk;
	} else {
		if ((host->version & SDHCI_SPEC_VER_MASK) >= SDHCI_SPEC_300) {
			mmc->f_max = (caps & SDHCI_CLOCK_V3_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;			
		}
		else {
			mmc->f_max = (caps & SDHCI_CLOCK_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;				
		}
		mmc->f_max *= 1000000;
	}
	if (mmc->f_max == 0) {
		puts("Hardware doesn't specify base clock frequency\n");	
		return -1;
	}

 
	mmc->f_min = 400000;

	mmc->voltages = 0;
	if (caps & SDHCI_CAN_VDD_330)
		mmc->voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		mmc->voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		mmc->voltages |= MMC_VDD_165_195;

#ifdef CONFIG_MLB01
	{
#if 0
		volatile u32 *p = (volatile u32 *)0x18002000;

		udelay(50000);
		/* force PC2 low output to provide power */
		p[(0xa0 + 0x20) >> 2] = 4; /* set data */
#endif
		volatile u32 *p = (volatile u32 *)0x1D022000;

		udelay(50000);
		/* force PC2 low output to provide power */
		p[(0x00 + 0x0c) >> 2] = 0x00200020;	/* set data */
	}
#endif



#ifdef CONFIG_F_SDH30_SDHCI
	/* before we reset, host clock should be set first */
	mmc_set_clock(mmc, mmc->f_min);
#endif



	return mmc_init(mmc);
}
