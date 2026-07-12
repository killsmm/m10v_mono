/*
 * serial common function
 */

#include "asm/utils.h"
#include "io.h"
#include "serial.h"

int getc(void)
{
	return _getc();
}

void putc(const char c)
{
	if (c == '\n')
		_putc('\r');

	_putc(c);
}

void puts(const char *s)
{
	while (*s)
		putc(*s++);
}

void serial_init(void)
{
	uart_init();

#ifdef CONFIG_LAVA_DELAY_SECONDS
	puts("\n -- Stubby LAVA delay ");
	printdec(CONFIG_LAVA_DELAY_SECONDS);
	puts("s ... --\n");
	{
		int n = (CONFIG_LAVA_DELAY_SECONDS) * 1000;
		while (n--)
			udelay(570);
	}	
#endif

	puts("\n"BUILD_STAMP " ");
#ifdef CONFIG_MB86S7X_MRBC
	puts("SoC rev ");
	print32_noleading((__raw_readl(CONFIG_MB86S7X_MRBC) >> 4) & 0xf);
#endif
	puts("\n");
}
