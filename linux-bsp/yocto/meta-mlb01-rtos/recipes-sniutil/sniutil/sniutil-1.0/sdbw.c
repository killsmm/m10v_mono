#define _GNU_SOURCE		/* syscall() is not POSIX */

#include <getopt.h>
#include <asm/errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <aio.h>
#include <time.h>

#include <sys/syscall.h>	/* for __NR_* definitions */
#include <linux/aio_abi.h>	/* for AIO types and constants */

#if 1
#define MMAP_DEVICE "/dev/mem"
#define MMAP_OFFSET 0x4C000000
#define MMAP_FLAGS MAP_SHARED
#else
#define MMAP_DEVICE "/dev/uio0"
#define MMAP_OFFSET 0
#define MMAP_FLAGS MAP_SHARED
#endif

#define COUNT_DEFAULT	64
#define LENGTH_DEFAULT	(4*1024*1024)

/*
 * Engine 1: blocking writes performed sequentially
 */

int write_engine_1(int fd_sd, int type, int count, void *addr, int length)
{
	ssize_t s;

	while (count--) {
		if (type == IOCB_CMD_PREAD)
			s = read(fd_sd, addr, length);
		else
			s = write(fd_sd, addr, length);
		if (s == length)
			continue;
		if (s == -1) {
			fprintf(stderr, "Write failed\n");
			return 1;
		}
		fprintf(stderr, "Incomplete write\n");
		return 1;
	}

	return 0;
}

/*
 * Engine 2: kernel async io (not libaio...)
 */
 
inline int io_setup(unsigned nr, aio_context_t *ctxp)
{
 	return syscall(__NR_io_setup, nr, ctxp);
}

inline int io_destroy(aio_context_t ctx) 
{
	return syscall(__NR_io_destroy, ctx);
}
 
inline int io_submit(aio_context_t ctx, long nr,  struct iocb **iocbpp) 
{
	return syscall(__NR_io_submit, ctx, nr, iocbpp);
}

inline int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
	struct io_event *events, struct timespec *timeout)
{
	return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
}

static unsigned long long offset = 0;

void fill_cb(struct iocb *cb, int type, int fd_sd, void *addr, int length)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts); 

	cb->aio_data = ((unsigned long long)ts.tv_sec << 32) | (ts.tv_nsec / 1000);
	cb->aio_fildes = fd_sd;
	cb->aio_lio_opcode = type;
	cb->aio_buf = (uint64_t)(unsigned long)addr;
	cb->aio_offset = offset;
	cb->aio_nbytes = length;

	offset += length;
}

#define MAX_IN_FLIGHT 2

int write_engine_2(int fd_sd, int type, int count, void *addr, int length)
{
	aio_context_t ctx = 0;
	struct iocb cb[MAX_IN_FLIGHT];
	struct iocb *cbs[MAX_IN_FLIGHT], *comp;
	struct io_event events[MAX_IN_FLIGHT];
	int ret, err = 0, submitted = 0, completed = 0;
	struct timeval tv, tv2, dur;
	struct timespec ts2;
	int n, ms;
	int pending_bitmap = 0;

	memset(&cb, 0, sizeof(cb));

	ret = io_setup(count, &ctx);
	if (ret < 0) {
		perror("io_setup error");
		return -1;
	}

	for (n = 0; n < MAX_IN_FLIGHT; n++)
		cbs[n] = &cb[n];

	while (completed < count) {
		while (submitted < count &&
		    pending_bitmap != (1 << MAX_IN_FLIGHT) - 1)
			for (n = 0; n < MAX_IN_FLIGHT; n++)
				if (!(pending_bitmap & (1 << n))) {
					fill_cb(&cb[n], type, fd_sd, addr,
						length);
					ret = io_submit(ctx, 1, &cbs[n]);
					pending_bitmap |= 1 << n;
					fprintf(stderr, "started buf %d\n", n);
					submitted++;
					break;
				}

		ret = io_getevents(ctx, 1, 1, events, NULL);
		if (ret < 1) {
			fprintf(stderr, "io_getevents failed\n");
			err = 1;
			goto bail;
		}
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		tv2.tv_sec = ts2.tv_sec;
		tv2.tv_usec = ts2.tv_nsec / 1000;

		for (n = 0; n < ret; n++) {
			comp = (struct iocb *)(unsigned long)events[n].obj;
			tv.tv_sec = events[n].data >> 32;
			tv.tv_usec = events[n].data & 0xffffffff;

			timersub(&tv2, &tv, &dur);
			ms = (dur.tv_sec * 1000) + (dur.tv_usec / 1000);

			fprintf(stdout, "%lld, %d, buf %d, %lld\n",
					comp->aio_offset / length, ms, 
					(comp - &cb[0]),
					events[n].res);

			pending_bitmap &= ~(1 << (comp - cbs[0]));
		}
		completed += ret;
	}
			

bail:
	ret = io_destroy(ctx);
	if (ret < 0) {
		perror("io_destroy error");
		return -1;
	}

	return err;
}

static struct option opts[] = {
	{"file", 1, 0, 'f'},
	{"o_direct", 0, 0, 'd'},
	{"usemalloc", 0, 0, 'u'},
	{"readtest", 0, 0, 'r'},
	{"engine", 1, 0, 'e'},
	{0, 0, 0, 0},
};

void usage()
{
	printf("Usage:\n");
	printf("sdbw [-r] [-o] [-u] [-l length] [-c count] [-e write_engine_no] [-f filename]\n");
	printf("  -r : read test, default is write test\n");
	printf("  -o : use O_DIRECT\n");
	printf("  -u : use user memory (malloc), default use mmap\n");
	printf("  -l : specify read/write burst length, default 4MB\n");
	printf("  -c : specify repeat count, default 64 times\n");
	printf("  -e : write_engine, default 2(sync) \n");
	printf("  -f : read/write file name, default sdbw-dump\n");
	printf("Example:\n");
	printf("  sdbw -o -u \n");
}

int main(int argc, char *argv[])
{
	struct timeval tv1, tv2, dur;
	struct timespec ts2;
	unsigned long long size;
	int length = LENGTH_DEFAULT;
	unsigned long long ms;
	int fd_map, fd_sd;
	int count = COUNT_DEFAULT;
	int engine = 2;
	int type = IOCB_CMD_PWRITE;
	const char *file = "sdbw-dump";
	int ret = 1;
	char *addr;
	int mode;
	int n, c, use_mmap = 1;

	mode = O_CREAT | O_TRUNC | O_WRONLY;
	while ((c = getopt_long(argc, argv, "e:f:rduc:l:h", opts, NULL)) != -1) {
		switch (c) {
		case 'f': /* file to read/write */
			file = optarg;
			break;
		case 'r': /* read test */
			mode &= ~(O_CREAT | O_TRUNC | O_WRONLY);
			mode |= O_RDONLY;
			type = IOCB_CMD_PREAD;
			fprintf(stderr, "Read mode - dropping caches\n");
			fd_sd = open("/proc/sys/vm/drop_caches", O_WRONLY);
			write(fd_sd, "3\n", 2);
			close(fd_sd);
			break;
		case 'd': /* use O_DIRECT */
			mode |= O_DIRECT;
			break;
		case 'e': /* Engine to use */
			engine = atoi(optarg);
			if (engine != 1 || engine != 2)
				engine = 2;
			break;
		case 'u': /* userspace malloc SHM */
			use_mmap = 0;
			break;
		case 'c': /* count */
			count = atoi(optarg);
			if (count <= 0)
				count = COUNT_DEFAULT;
			break;
		case 'l': /* length */
			length = atoi(optarg);
			if (length <= 0)
				length = LENGTH_DEFAULT;
			break;
		case 'h': /* file to read/write */
		default:
			usage();
			return 0;
		}
	}

	fprintf(stderr, "Writing %d x %dKiB mmap region (%dMiB) to SD\n",
			count, length / 1024, (count * length) >> 20);
	fprintf(stderr, "Using "MMAP_DEVICE" at offset 0x%x file: %s %s\n",
		MMAP_OFFSET, file, (mode & O_DIRECT) ? "O_DIRECT" : "");

	/*
	 * we will mmap the first 4MB of MMAP_DEVICE and
	 * create a file on SD with the mmap region copied many
	 * times sequentially.  From that we will estimate write
	 * throughput.
	 */

	fd_map = open(MMAP_DEVICE, O_RDWR);
	if (fd_map == -1) {
		fprintf(stderr, "Unable to open mmap file\n");
		return 1;
	}

	if (use_mmap) {
		addr = mmap(NULL, length, PROT_READ | PROT_WRITE,
				MMAP_FLAGS, fd_map, MMAP_OFFSET);
		if (addr == MAP_FAILED) {
			fprintf(stderr, "Unable to mmap %d bytes\n", length);
			goto bail;
		}
	} else {
		unsigned val = (unsigned) malloc(length + 1024);
		val += (1024 - val % 1024); /* align to 1KB */
		addr = (char *) val;
		if (!addr)
			goto bail;
		for (n = 0; n < length; n++)
			addr[n] = n;
	}

	fprintf(stderr, "mmap got %x ", (unsigned) addr);
	for (n = 0; n < 256; n++) {
		fprintf(stderr, "%02X ", *(addr + n));
		if ((n & 15) == 15)
			fprintf(stderr, "\n");
	}

	fd_sd = open(file, mode, 0644);
	if (fd_sd == -1) {
		fprintf(stderr, "Unable to open dump file in cwd\n");
		goto bail1;
	}

	clock_gettime(CLOCK_MONOTONIC, &ts2);
	tv1.tv_sec = ts2.tv_sec;
	tv1.tv_usec = ts2.tv_nsec / 1000;

	switch (engine) {
	case 1:
		n = write_engine_1(fd_sd, type, count, addr, length);
		break;
	case 2:
		n = write_engine_2(fd_sd, type, count, addr, length);
		break;
	}

	if (n)
		goto bail2;

	/* 
	 * close flushes the filesystem cache for this file,
	 * which will normally be considerable if the output file
	 * was not opened O_SYNC, but almost nothing if O_SYNC
	 */

	if (type == IOCB_CMD_PWRITE)
		fdatasync(fd_sd);
	close(fd_sd);
	clock_gettime(CLOCK_MONOTONIC, &ts2);
	tv2.tv_sec = ts2.tv_sec;
	tv2.tv_usec = ts2.tv_nsec / 1000;

	timersub(&tv2, &tv1, &dur);

	size = length * count;
	ms = (dur.tv_sec * 1000) + (dur.tv_usec / 1000);

	fprintf(stderr, "starting closing\n");

	for (n = 0; n < 256; n++) {
		fprintf(stderr, "%02X ", *(addr + n));
		if ((n & 15) == 15)
			fprintf(stderr, "\n");
	}

	if (type == IOCB_CMD_PWRITE)
		fprintf(stderr, "Wrote %lldKiB in %lldms, %lldKiB/sec\n",
			size / 1024, ms, ((size * 1000) / 1024) / ms);
	else
		fprintf(stderr, "Read %lldKiB in %lldms, %lldKiB/sec\n",
			size / 1024, ms, ((size * 1000) / 1024) / ms);
		

	ret = 0;
	goto bail1;

bail2:
	close(fd_sd);
bail1:
	if (use_mmap)
		munmap(addr, length);
bail:
	close(fd_map);

	return ret;
}
