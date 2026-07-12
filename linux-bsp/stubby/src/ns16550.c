/*
 * COM1 NS16550 support
 * originally from linux source (arch/powerpc/boot/ns16550.c)
 * modified to use CONFIG_SYS_ISA_MEM and new defines
 */
#ifdef CONFIG_SYS_NS16550

#include "asm/utils.h"
#include "io.h"
#include "serial.h"

#define u8 unsigned char

enum {
	UART_RBR,
	UART_IER,
	UART_FCR,
	UART_LCR,
	UART_MCR
};

#define UART_LCR_8N1    	0x03
#define UART_LCR_BKSE   	0x80	/* Bank select enable */
#define UART_MCR_DTR    	0x01	/* DTR   */
#define UART_MCR_RTS    	0x02	/* RTS   */
#define UART_FCR_FIFO_EN	0x01	/* Fifo enable */
#define UART_FCR_RXSR		0x02 	/* Receiver soft reset */
#define UART_FCR_TXSR		0x04 	/* Transmitter soft reset */
#define UART_LSR_THRE   	0x20	/* Xmit holding register empty */

static volatile u32 *p = (volatile u32 *)CONFIG_SYS_NS16550_UART0;

int _getc(void)
{
	if (!(p[5] & 1))
		return -1;

	return *p;
}

void _putc(char c)
{
	while ((p[5] & UART_LSR_THRE) == 0)
		;
	p[0] = c;
}

void uart_init(void)
{
	int baud_divisor =
			(CONFIG_SYS_NS16550_CLK + (CONFIG_BAUDRATE * (16 / 2))) /
			(16 * CONFIG_BAUDRATE);
		volatile u32 n;
	
	p[UART_IER] = 0;
	p[UART_LCR] = UART_LCR_BKSE | UART_LCR_8N1;
	p[UART_RBR] = 0;
	p[UART_LCR] = UART_LCR_8N1;
	p[UART_MCR] = UART_MCR_DTR | UART_MCR_RTS;
	p[UART_FCR] = UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR;
	p[UART_LCR] = UART_LCR_BKSE | UART_LCR_8N1;
	p[UART_RBR] = baud_divisor & 0xff;
	p[UART_IER] = (baud_divisor >> 8) & 0xff;
	p[UART_LCR] = UART_LCR_8N1;
	n = p[0]; /* clear any read character */

}

#endif

