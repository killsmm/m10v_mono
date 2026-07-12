/*
 * (C) Copyright 2015 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include "asm/utils.h"
#include "io.h"
#include "serial.h"

void uart_init(void)
{
	//Set Ch1 to UART by GPIO 
#ifdef CONFIG_MLB01
	switch(CONFIG_SYS_USIO_CH) {
	case 1://UART ch 1
	    writel(0x00000020|readl(IO_EPCR_PE), IO_EPCR_PE);//PE4,5
		break;
	case 2://UART ch 2
	    writel(0x00000002|readl(IO_EPCR_PE), IO_EPCR_PE);//PE0,PE1
		break;
	}
#endif
    writew(0x363, CONFIG_SYS_USIO + 0x06);//BGR 115200
    writew(0, CONFIG_SYS_USIO + 0x0C);//FCR
    writeb(0, CONFIG_SYS_USIO + 0x00);//SMR
    writeb(0, CONFIG_SYS_USIO + 0x02);//ESCR
    writeb(0x80, CONFIG_SYS_USIO + 0x03);//SSR
    writeb(3, CONFIG_SYS_USIO + 0x01);//SCR
    writeb(1, CONFIG_SYS_USIO + 0x00);//SMR:SOE->1 mean set SOUT
}

void _putc(char c)
{
        while (!(readb(CONFIG_SYS_USIO + 3) & 2))
                ;
        writew(c & 0xff, CONFIG_SYS_USIO + 4);
}

int _getc(void)
{
	if (readb(CONFIG_SYS_USIO + 3) & 0x4)
		return readb(CONFIG_SYS_USIO + 4);

	return -1;
}

