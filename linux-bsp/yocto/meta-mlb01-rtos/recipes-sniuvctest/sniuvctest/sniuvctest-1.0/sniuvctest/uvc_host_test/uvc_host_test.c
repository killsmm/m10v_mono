#include <linux/videodev2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>



#define CAMERA_WIDTH  640
#define CAMERA_HEIGHT 480
#define MMAP_COUNT    2
#define PICTURE_NUM   30

static int writefile(int fd_out, void *addr, int length)
{
	ssize_t s;

	s = write(fd_out, addr, length);
	if( s != length ) {
		printf("write file : ERROR=length is short\n");
		return -11;
	}
	if (s == -1) {
		printf("Write failed\n");
		return -1;
	}

	printf("write file : ok\n");
	return 0;
}

static void
usage(const char *argv0)
{
	fprintf(stderr, " -f <format>    Select frame format\n\t"
				"0 = V4L2_PIX_FMT_YUYV\n\t"
				"2 = V4L2_PIX_FMT_H264\n");
	fprintf(stderr, " -F <filename>  Select Filename to write frames\n");
	fprintf(stderr, " -v <device>    Video Capture device(V4L2)\n");
}

int main(int argc, char *argv[])
{
	int fd, width, height, total_write_length, total_pix, count, ret, i, opt;
	int fd_sd;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct v4l2_capability cap;
	enum v4l2_buf_type type;
	void *mmap_p[MMAP_COUNT];
	__u32 mmap_l[MMAP_COUNT];
	uint8_t *yuyvbuf, *pyuyv;
	struct rusage ru;
	double t;
	char *file = "v4l2out.yuyv";
	char *v4l2_devname = "/dev/video0"; 
	int f_mode;
	int default_format = 0;	/* 0:YUYV, 2:H264 */

	while ((opt = getopt(argc, argv, "f:F:v:")) != -1) {
	switch (opt) {
		case 'f':
			if (atoi(optarg) < 0 && atoi(optarg) != 0 && atoi(optarg) != 2) {
				usage(argv[0]);
				return 1;
			}

			default_format = atoi(optarg);
			break;
		case 'F':
			file = optarg;
			break;
		case 'v':
			v4l2_devname = optarg;
			break;
		default:
			printf("Invalid option '-%c'\n", opt);
			usage(argv[0]);
			return 1;
		}
	}

	/* open file to save stream */
	f_mode = O_CREAT | O_TRUNC | O_WRONLY;
	fd_sd = open(file, f_mode, 0644);
	if (fd_sd == -1) {
		fprintf(stderr, "Unable to open dump file in cwd\n");
		return -1;
	}
	printf("open file %s\n", file);
	
	/* open v4l2 device */
	fd = open(v4l2_devname, O_RDWR | O_NONBLOCK, 0);
		if (fd < 0) {
	    printf("open failed %s\n", v4l2_devname);
		close(fd_sd);
	    return -1;
	}

	/* check capability of capture and streaming */
	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0) {
		printf ("V4L2: VIDIOC_QUERYCAP failed: %s (%d).\n",
				strerror(errno), errno);
	    return -1;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("V4L2: %s is no video capture device\n", v4l2_devname);
	    return -1;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		printf("V4L2: %s does not support streaming i/o\n",
					v4l2_devname);
	    return -1;
	}

	printf("V4L2 device is %s on bus %s\n", cap.card, cap.bus_info);

	/* Set the format to v4l2 device */
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = CAMERA_WIDTH;
	fmt.fmt.pix.height = CAMERA_HEIGHT;
	switch (default_format)
	{
	case 2:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
		fmt.fmt.pix.field = V4L2_FIELD_ANY;
		break;

	case 0:
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		break;
	default:
		printf("option of f is invalid\n");
		close(fd_sd);
		return -1;
	}
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		printf("ioctl(VIDIOC_S_FMT) failed (ret=%d)\n", ret);
		return -1;
	}
	if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV &&
		fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_H264 ) {
		printf("ioctl(VIDIOC_S_FMT) failed(fmt error)\n");
		return -1;
	}
	if (fmt.fmt.pix.width <= 0 || fmt.fmt.pix.height <= 0) {
		printf("ioctl(VIDIOC_S_FMT) failed(size error)\n");
		return -1;
	}
	width = fmt.fmt.pix.width;
	height = fmt.fmt.pix.height;
	total_pix = width * height;

	/* prepare buffer */
	yuyvbuf = malloc(2 * total_pix * PICTURE_NUM);
	if (!yuyvbuf) {
		printf("malloc");
		return -1;
	}

	memset(&req, 0, sizeof(req));
	req.count = MMAP_COUNT;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret < 0) {
		printf("ioctl(VIDIOC_REQBUFS) failed\n");
		return -1;
	}
	count = req.count;

	for (i = 0; i < count; i++) {
		memset(&buf, 0, sizeof(buf));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			printf("ioctl(VIDIOC_QUERYBUF) failed\n");
			return -1;
		}

		mmap_p[i] = mmap(NULL, buf.length, PROT_READ, MAP_SHARED, fd, buf.m.offset);
		if (mmap_p[i] == MAP_FAILED) {
			printf("mmap");
			return -1;
		}
		mmap_l[i] = buf.length;
	}

	/* enqueue buffer to videostream for 1st image */
	for (i = 0; i < count; i++) {
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			printf("ioctl(VIDIOC_QBUF) failed\n");
			return -1;
		}
	}

	/* start streaming */
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret < 0) {
		printf("ioctl(VIDIOC_STREAMON) failed\n");
		return -1;
	}

	for (i = 0, pyuyv = yuyvbuf, total_write_length = 0; i < PICTURE_NUM; i++) {
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		
		/* wait for camera to fill buffer */
		for (; ; ) {
			ret = select(fd + 1, &fds, NULL, NULL, NULL);
			if (ret < 0) {
				if (errno == EINTR)
					continue;
				printf("select failed\n");
				return -1;
			}
			break;
		}

	    if (FD_ISSET(fd, &fds)) {
			memset(&buf, 0, sizeof(buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			
			/* dequeue buffer from videostream */
			ret = ioctl(fd, VIDIOC_DQBUF, &buf);
			if (ret < 0) {
				printf("ioctl(VIDOC_DQBUF) failed(ret=%d)\n", ret);
				return -1;
		    }
			if( default_format == 0 ) {	/* YUYV */
				if (buf.bytesused < (__u32)(2 * total_pix)) {
					printf("ioctl(VIDOC_DQBUF) failed(size failed at YUYV)\n");
					return -1;
			    }
			}
			printf("frame=%d : size=%d\n", i, buf.bytesused);
			total_write_length += (int)buf.bytesused;

		    memcpy(pyuyv, mmap_p[buf.index], buf.bytesused);
		    pyuyv += buf.bytesused;

			/* enqueue buffer to videostream for next image */
		    ret = ioctl(fd, VIDIOC_QBUF, &buf);
		    if (ret < 0) {
				printf("ioctl(VIDIOC_QBUF) failed\n");
				return -1;
		    }
		}
	}

	/* stop streaming */
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd, VIDIOC_STREAMOFF, &type);
	for (i = 0; i < count; i++)
		munmap(mmap_p[i], mmap_l[i]);
	close(fd);

	getrusage(RUSAGE_SELF, &ru);
	t = ((double)ru.ru_utime.tv_sec * 1e+3 +
	   (double)ru.ru_utime.tv_usec * 1e-3);

	ret = writefile(fd_sd, yuyvbuf, total_write_length);
	if(ret < 0) {
		return -1;
	}

	close(fd_sd);

	getrusage(RUSAGE_SELF, &ru);
	t = ((double)ru.ru_utime.tv_sec * 1e+3 +
	   (double)ru.ru_utime.tv_usec * 1e-3) - t;
	printf("convert time: %3.3lf msec/flame\n", t / (double)PICTURE_NUM);

	free(yuyvbuf);

	return 0;
}
