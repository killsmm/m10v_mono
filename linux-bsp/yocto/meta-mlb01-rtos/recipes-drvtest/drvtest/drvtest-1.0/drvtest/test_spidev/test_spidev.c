/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev2.0";
static uint32_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;
static int verbose;
static int size;
static uint8_t *pbuffer = NULL;
static uint16_t *pbuffer_16 = NULL;
static uint32_t *pbuffer_32 = NULL;

uint8_t default_rx[32] = {0, };
uint16_t default_rx_16[32] = {0, };
uint32_t default_rx_32[32] = {0, };
char *input_tx;

static void hex_dump(const void *src, size_t length, size_t line_size, char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	const uint16_t *address_16 = src;
	const uint16_t *line_16 = address_16;
	const uint32_t *address_32 = src;
	const uint32_t *line_32 = address_32;
	unsigned char c;
	size_t data_nums;

	if (bits > 16)
		data_nums = length / 4;
	else if (bits > 8)
		data_nums = length / 2;
	else
		data_nums = length;

	printf("%s | ", prefix);
	while (data_nums-- > 0) {
		if (bits > 16)
			printf("%08X ", *address_32++);
		else if (bits > 8)
			printf("%04X ", *address_16++);
		else
			printf("%02X ", *address++);

		if (!(++i % line_size) || (data_nums == 0 && i % line_size)) {
			if (data_nums == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" | ");  /* right close */
			if (bits > 16) {
				while (line_32 < address_32) {
					c = *line_32++;
					printf("%c", (c < 33 || c == 255) ? 0x2E : c);
				}
			}
			else if (bits > 8) {
				while (line_16 < address_16) {
					c = *line_16++;
					printf("%c", (c < 33 || c == 255) ? 0x2E : c);
				}
			}
			else {
				while (line < address) {
					c = *line++;
					printf("%c", (c < 33 || c == 255) ? 0x2E : c);
				}
			}
			printf("\n");
			if (data_nums > 0)
				printf("%s | ", prefix);
		}
	}
}

/*
 *  Unescape - process hexadecimal escape character
 *      converts shell input "\x23" -> 0x23
 */
static int unescape(char *_dst, char *_src, size_t len)
{
	int ret = 0;
	char *src = _src;
	char *dst = _dst;
	unsigned int ch;

	while (*src) {
		if (*src == '\\' && *(src+1) == 'x') {
			sscanf(src + 2, "%2x", &ch);
			src += 4;
			*dst++ = (unsigned char)ch;
		} else {
			*dst++ = *src++;
		}
		ret++;
	}
	return ret;
}

static void transfer(int fd, const void *tx, const void *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	if (verbose){
		hex_dump(tx, len, 32, "TX");
	}
	hex_dump(rx, len, 32, "RX");
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3vpNR24trX]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev2.0)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delay    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n"
	     "  -v --verbose  Verbose (show tx buffer)\n"
	     "  -p            Send data (e.g. \"1234\\xde\\xad\")\n"
	     "  -N --no-cs    no chip select\n"
	     "  -R --ready    slave pulls low to pause\n"
	     "  -X --Xdata    hexadecimal data\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	int i, index;
	char *p;

	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "delay",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ "verbose", 0, 0, 'v' },
			{ "Xdata",   1, 0, 'X' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NRp:vX", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		case 'p':
			input_tx = optarg;
			break;
		case 'X':
			i = 0;
			if (bits > 16) {
				size = (argc - optind) * 4;
				pbuffer_32 = malloc(size);
				for(index = optind; index < argc; index++, i++)
				{
					pbuffer_32[i] = strtoul(argv[index], &p, 0);
				}
			}
			else if (bits > 8) {
				size = (argc - optind) * 2;
				pbuffer_16 = malloc(size);
				for(index = optind; index < argc; index++, i++)
				{
					pbuffer_16[i] = strtoul(argv[index], &p, 0);
				}
			}
			else {
				size = argc - optind;
				pbuffer = malloc(size);
				for(index = optind; index < argc; index++, i++)
				{
					pbuffer[i] = strtoul(argv[index], &p, 0);
				}
			}
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint8_t *tx;

	parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	if (input_tx) {
		size = strlen(input_tx+1);
		tx = malloc(size);
		size = unescape((char *)tx, input_tx, size);
		transfer(fd, tx, default_rx, size);
	}
	else if (!pbuffer && !pbuffer_16 && !pbuffer_32) {
		printf("please set input addr\n");
	}
	else {
		if (bits > 16)
			transfer(fd, pbuffer_32, default_rx_32, size);
		else if (bits > 8)
			transfer(fd, pbuffer_16, default_rx_16, size);
		else
			transfer(fd, pbuffer, default_rx, size);
	}

	if (input_tx)
		free(tx);

	if (pbuffer)
		free(pbuffer);

	if (pbuffer_16)
		free(pbuffer_16);

	if (pbuffer_32)
		free(pbuffer_32);

	close(fd);

	return ret;
}
