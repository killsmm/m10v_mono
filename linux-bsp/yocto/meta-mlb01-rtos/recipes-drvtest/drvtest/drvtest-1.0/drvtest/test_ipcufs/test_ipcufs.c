/******************************************************************************
 *  @file   test_ipcufs.c
 ******************************************************************************/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

//#define IPCUFS_TRACE

#define SD_PATH  "/mnt/sdcard"
#define RTOS_SD_PATH  "/mnt/sdcard/ipcu"
#define COPY_BASE_FILE  "/mnt/sdcard/ipcu/base.dat"
#define COPY_FILE  "/run/SD/mmcblk0p1/copy.dat"
#define FROM_TEST_FILE_L  "/run/SD/mmcblk0p1/ipcu.bin"
#define FROM_TEST_FILE_R  "/mnt/sdcard/ipcu/ipcu.bin"
#define FROM_WRT_FILE_L1  "/run/SD/mmcblk0p1/write1.bin"
#define FROM_WRT_FILE_R1  "/mnt/sdcard/ipcu/write1.bin"
#define FROM_WRT_FILE_L2  "/run/SD/mmcblk0p1/write2.bin"
#define FROM_WRT_FILE_R2  "/mnt/sdcard/ipcu/write2.bin"

#define FILE_MAX  100000
#define BUF_SIZE  16
#define BLOCKSIZE 512
#define MAX_LOOP_NUM 10

#define DROP_CASHES  system("echo 3 > /proc/sys/vm/drop_caches")

static int long_file_name = 1;

/* diff files */
int diff_files_fp(const char *f_file, const char *t_file)
{
	FILE *from_fp, *to_fp;
	int dff1, dff2;

	DROP_CASHES;

	printf("File comparison start\n");
	from_fp = fopen(f_file, "r");
	to_fp = fopen(t_file,"r");
	if (from_fp == NULL || to_fp == NULL) {
		printf("File open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}
	while (1) {
		dff1 = fgetc(from_fp);
		dff2 = fgetc(to_fp);
		if (dff1 != dff2) {
			printf("File comparison [NG]\n");
			fclose(from_fp);
			fclose(to_fp);
			return -1;
		}
		if (dff1 == EOF) break;
	}
	fclose(from_fp);
	fclose(to_fp);
	printf("File comparison [OK]\n");

	return 0;
}

/* create copy base file */
int create_base_file()
{
	FILE *fp;
	int i;
	char str[16];

	DROP_CASHES;

	printf("Create File start\n");
	fp = fopen(COPY_BASE_FILE,"w");
	if (fp == NULL) {
		printf("File open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}
	srand(1024);
	for (i = 1; i <= FILE_MAX; i++) {
		sprintf(str, "%05x%05x%05x%01x", rand(), rand(), rand(), rand());
		fputs(str, fp);
	}
	fputs("\n", fp);
	fclose(fp);
	printf("Create File end\n");

	system("ls -lha /mnt/sdcard/ipcu/base.dat");

	return 0;
}

/* copy file from RTOS */
int copy_file_from_rtos()
{
	FILE *from_fp, *to_fp;
	int ret, rw;

	DROP_CASHES;

	printf("File copy start\n");
	from_fp = fopen(COPY_BASE_FILE, "r");
	if (from_fp == NULL) {
		printf("Base File open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}
	to_fp = fopen(COPY_FILE,"w");
	if (to_fp == NULL) {
		printf("Copy File open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}
	while ((rw = fgetc(from_fp)) != EOF) {
		fputc(rw, to_fp);
	}
	fclose(from_fp);
	fclose(to_fp);

	system("ls -lha /run/SD/mmcblk0p1/copy.dat");

	ret = diff_files_fp(COPY_BASE_FILE, COPY_FILE);
	if (ret == -1) 
		return ret;

	printf("File copy end\n\n");
	return 0;
}

/* Read Use fseek for fp */
int read_seek_fp()
{
	FILE *from_fp, *to_fp;
	int i;
	long f_offset, disp_f_offset;
	char from_cha[BUF_SIZE], to_cha[BUF_SIZE];

	DROP_CASHES;

	printf("fseek using file read start\n");
	memset(from_cha, 0, sizeof(from_cha));
	memset(to_cha, 0, sizeof(to_cha));
	from_fp = fopen(COPY_BASE_FILE, "r");
	to_fp = fopen(COPY_FILE,"r");
	if (from_fp == NULL || to_fp == NULL) {
		printf("File open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}

	printf("-Sequential seek-\n");
	disp_f_offset = 0;
	srand((unsigned) time(NULL));
	for (i=0; i < MAX_LOOP_NUM; i++) {
		f_offset = (rand()%30+1) * 5200;
		fseek(from_fp, f_offset, SEEK_CUR);
		fseek(to_fp, f_offset, SEEK_CUR);

		disp_f_offset = disp_f_offset + f_offset;
		printf("offset %12ld : ", disp_f_offset);

		fgets(from_cha, sizeof(from_cha), from_fp);
		fgets(to_cha, sizeof(to_cha), to_fp);
		if (strncmp(from_cha, to_cha, BUF_SIZE) != 0) {
			printf("%s != %s\n", from_cha, to_cha);
			printf("fseek using file read [NG]\n");
			fclose(from_fp);
			fclose(to_fp);
			return -1;
		}
		printf("%s = %s\n", from_cha, to_cha);
	}

	DROP_CASHES;

	printf("-Random seek-\n");
	disp_f_offset = 0;
	for (i=0; i < MAX_LOOP_NUM; i++) {
		f_offset = rand()%35+1;
		if ((f_offset % 2) == 0) {
			f_offset = f_offset*(-1);
		}
		f_offset = f_offset * 5200;
		fseek(from_fp, f_offset, SEEK_CUR);
		fseek(to_fp, f_offset, SEEK_CUR);

		disp_f_offset = disp_f_offset + f_offset;
		printf("offset %12ld : ", disp_f_offset);

		fgets(from_cha, sizeof(from_cha), from_fp);
		fgets(to_cha, sizeof(to_cha), to_fp);
		if (strncmp(from_cha, to_cha, BUF_SIZE) != 0) {
			printf("%s != %s\n", from_cha, to_cha);
			printf("fseek using file read [NG] \n");
			fclose(from_fp);
			fclose(to_fp);
			return -1;
		}
		printf("%s = %s\n", from_cha, to_cha);
	}

	fclose(from_fp);
	fclose(to_fp);
	printf("fseek using file read [OK]\n\n");

	return 0;
}

/* Read Use fseek for fd */
int read_seek_fd()
{
	int from_fd, to_fd, i;
	off_t l_offset, disp_l_offset;
	char from_cha[BUF_SIZE], to_cha[BUF_SIZE];

	DROP_CASHES;

	printf("lseek using file read start\n");
	memset(from_cha, 0, sizeof(from_cha));
	memset(to_cha, 0, sizeof(to_cha));

	from_fd = open(COPY_BASE_FILE, O_RDONLY);
	to_fd = open(COPY_FILE,O_RDONLY);
	if (from_fd < 0 || to_fd < 0) {
		printf("File open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}

	printf("-Sequential seek-\n");
	disp_l_offset = 0;
	srand((unsigned) time(NULL));
	for (i=0; i < MAX_LOOP_NUM; i++) {
		l_offset = (rand()%27+1) * 5200;
		lseek(from_fd, l_offset, SEEK_CUR);
		lseek(to_fd, l_offset, SEEK_CUR);

		disp_l_offset = disp_l_offset + l_offset;
		printf("offset %12ld : ", disp_l_offset);

		read(from_fd, from_cha, sizeof(from_cha));
		read(to_fd, to_cha, sizeof(to_cha));

		if (strncmp(from_cha, to_cha, BUF_SIZE) != 0) {
			printf("%.*s ", 16, from_cha);
			printf(" != %.*s\n", 16, to_cha);
			printf("lseek using file read [NG]\n");
			close(from_fd);
			close(to_fd);
			return -1;
		}
		printf("%.*s", 16, from_cha);
		printf(" = %.*s\n", 16, to_cha);
	}

	DROP_CASHES;

	printf("-Random seek-\n");
	disp_l_offset = 0;
	for (i=0; i < MAX_LOOP_NUM; i++) {
		l_offset = (rand()%28+1) * 5200;
		if ((l_offset % 2) == 0) {
			l_offset = l_offset*(-1);
		}
		l_offset = l_offset * 5200;
		lseek(from_fd, l_offset, SEEK_CUR);
		lseek(to_fd, l_offset, SEEK_CUR);

		disp_l_offset = disp_l_offset + l_offset;
		printf("offset %12ld : ", disp_l_offset);

		read(from_fd, from_cha, sizeof(from_cha));
		read(to_fd, to_cha, sizeof(to_cha));

		if (strncmp(from_cha, to_cha, BUF_SIZE) != 0) {
			printf("%.*s ", 16, from_cha);
			printf(" != %.*s\n", 16, to_cha);
			printf("lseek using file read [NG]\n");
			close(from_fd);
			close(to_fd);
			return -1;
		}
		printf("%.*s", 16, from_cha);
		printf(" = %.*s\n", 16, to_cha);
	}

	close(from_fd);
	close(to_fd);
	printf("lseek using file read [OK]\n\n");

	return 0;
}

/* Read random O_DIRECT */
int read_seek_o_dir()
{
	int from_fd_l, to_fd_l, from_fd_r, to_fd_r, i;
	off_t offset[] = {5000, -2000, -300, 7000 , 200, -1000, 700, -1200, 100, -700};
	off_t ofst;
	char from_cha_l[BUF_SIZE], from_cha_r[BUF_SIZE];
	char to_cha_l[BUF_SIZE], to_cha_r[BUF_SIZE];
	char *buf_l, *buf_r;
	int size = 1 * 128 * 128;	/* 4KB */
	char filename[64];
	char str[256];

	DROP_CASHES;
	buf_l = 0;
	buf_r = 0;

	sprintf(str, "cp %s %s", COPY_BASE_FILE, FROM_TEST_FILE_L);
	system(str);
	sprintf(str, "cp %s %s", COPY_BASE_FILE, FROM_TEST_FILE_R);
	system(str);
	printf("lseek using file read of O_DIRECT start\n");
	memset(from_cha_l, 0, sizeof(from_cha_l));
	memset(from_cha_r, 0, sizeof(from_cha_r));
	memset(to_cha_l, 0, sizeof(to_cha_l));
	memset(to_cha_r, 0, sizeof(to_cha_r));

	from_fd_l = open(FROM_TEST_FILE_L, O_RDONLY | O_DIRECT);
	if (from_fd_l < 0) {
		printf("Read file open NG 1\n");
		printf("error %d : %s\n", errno, strerror(errno));
		goto error;
	}
	from_fd_r = open(FROM_TEST_FILE_R, O_RDONLY | O_DIRECT);
	if (from_fd_r < 0) {
		printf("Read file open NG 2\n");
		printf("error %d : %s\n", errno, strerror(errno));
		goto error_l;
	}

	posix_memalign((void **)&buf_l, BLOCKSIZE, size);
	posix_memalign((void **)&buf_r, BLOCKSIZE, size);

	for (i = 0; i < MAX_LOOP_NUM; i++) {
		sprintf(filename, "/mnt/sdcard/ipcu/ipcu_%02d", i);
		to_fd_l = open(filename, O_WRONLY | O_CREAT | O_DIRECT | O_TRUNC);
		if (to_fd_l < 0) {
			printf("Write file open NG\n");
			printf("error %d : %s\n", errno, strerror(errno));
			goto error_r;
		}
		sprintf(filename, "/run/SD/mmcblk0p1/ipcu_%02d", i);
		to_fd_r = open(filename, O_WRONLY | O_CREAT | O_DIRECT | O_TRUNC);
		if (to_fd_r < 0) {
			printf("Write file open NG\n");
			printf("error %d : %s\n", errno, strerror(errno));
			goto error_to;
		}

		ofst = offset[i] * BLOCKSIZE;
		lseek(from_fd_l, ofst, SEEK_CUR);
		lseek(from_fd_r, ofst, SEEK_CUR);

		read(from_fd_l, buf_l, size);
		read(from_fd_r, buf_r, size);

		printf("offset %12ld : ", ofst);

		if (strncmp(buf_l, buf_r, 16) != 0) {
			printf("%.*s ", 16, buf_l);
			printf(" != %.*s\n", 16, buf_r);
			printf("lseek using file read [NG]\n");
			goto error_all;
		}
		printf("%.*s ", 16, buf_l);
		printf(" = %.*s\n", 16, buf_r);

		write(to_fd_l, buf_l, size);
		write(to_fd_r, buf_r, size);

		close(to_fd_l);
		close(to_fd_r);
	}
	close(from_fd_l);
	close(from_fd_r);

	printf("lseek using file read of O_DIRECT [OK]\n\n");
	return 0;

error_all:
	close(to_fd_r);
error_to:
	close(to_fd_l);
error_r:
	close(from_fd_r);
error_l:
	close(from_fd_l);
error:
	return -1;
}

/* Write files of seek */
int write_files_of_seek()
{
	int from_fd, to_fd, i, ret;
	off_t l_offset;
	char str[256];


	DROP_CASHES;
	printf("lseek using file write start\n");
	sprintf(str, "cp %s %s", COPY_BASE_FILE, FROM_WRT_FILE_L1);
	system(str);
	sprintf(str, "cp %s %s", COPY_BASE_FILE, FROM_WRT_FILE_R1);
	system(str);

	from_fd = open(FROM_WRT_FILE_R1, O_WRONLY);
	to_fd = open(FROM_WRT_FILE_L1, O_WRONLY);
	if (from_fd < 0 || to_fd < 0) {
		printf("Write file open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}

	printf("-Sequential seek write-\n");
	srand((unsigned) time(NULL));
	for (i=0; i < MAX_LOOP_NUM; i++) {
		l_offset = (rand()%10+1) * 5200;
		lseek(from_fd, l_offset, SEEK_CUR);
		lseek(to_fd, l_offset, SEEK_CUR);
		printf("offset %12ld \n", l_offset);

		write(from_fd, "Z", 1);
		write(to_fd, "Z", 1);
	}
	close(from_fd);
	close(to_fd);
	DROP_CASHES;

	ret = diff_files_fp(FROM_WRT_FILE_R1, FROM_WRT_FILE_L1);
	if (ret == -1) 
		return ret;

	DROP_CASHES;
	sprintf(str, "cp %s %s", COPY_BASE_FILE, FROM_WRT_FILE_L2);
	system(str);
	sprintf(str, "cp %s %s", COPY_BASE_FILE, FROM_WRT_FILE_R2);
	system(str);
	from_fd = open(FROM_WRT_FILE_R2, O_WRONLY);
	to_fd = open(FROM_WRT_FILE_L2, O_WRONLY);
	if (from_fd < 0 || to_fd < 0) {
		printf("Write file open NG\n");
		printf("error %d : %s\n", errno, strerror(errno));
		return -1;
	}

	printf("-Random seek write-\n");
	srand((unsigned) time(NULL));
	for (i=0; i < MAX_LOOP_NUM; i++) {
		l_offset = (rand()%9+1);
		if (i != 0) {
			if ((l_offset % 2) == 0)
				l_offset = l_offset*(-1);
		}
		l_offset= l_offset * 5200;
		lseek(from_fd, l_offset, SEEK_CUR);
		lseek(to_fd, l_offset, SEEK_CUR);
		printf("offset %12ld\n", l_offset);

		write(from_fd, "X", 1);
		write(to_fd, "X", 1);
	}

	close(from_fd);
	close(to_fd);
	DROP_CASHES;

	ret = diff_files_fp(FROM_WRT_FILE_R2, FROM_WRT_FILE_L2);
	if (ret == -1) 
		return ret;

	printf("lseek using file write end\n\n");
	return 0;
}

/* File and Dir rename */
int rename_test()
{
	int i;
	char filename[128], filename2[128];

	DROP_CASHES;

	printf("File rename start\n");
	printf("ipcu_xx -> re_xx\n");
	if (long_file_name) {
		system("ls -lha /mnt/sdcard/ipcu/ipcu_*");
	}
	else {
		system("ls -lha /mnt/sdcard/ipcu/IPCU_*");
	}

	printf("----------------------------------\n");
	for (i = 0; i < MAX_LOOP_NUM; i++) {
		sprintf(filename, "/mnt/sdcard/ipcu/ipcu_%02d", i);
		sprintf(filename2, "/mnt/sdcard/ipcu/re_%02d", i);
		if (rename(filename, filename2) != 0) 
			return -1;
	}

	if (long_file_name) {
		system("ls -lha /mnt/sdcard/ipcu/re_*");
	}
	else {
		system("ls -lha /mnt/sdcard/ipcu/RE_*");
	}

	printf("File rename end\n\n");

	printf("Dir rename start\n");
	printf("ipcu -> rename\n");
	system("ls -lha /mnt/sdcard");
	printf("----------------------------------\n");
	sprintf(filename, "/mnt/sdcard/ipcu");
	sprintf(filename2, "/mnt/sdcard/rename");
	if (rename(filename, filename2) != 0) 
		return -1;
	system("ls -lha /mnt/sdcard");
	printf("Dir rename end\n\n");

	return 0;
}

/* File and Dir remove */
int remove_test()
{
	int i;
	char filename[128];

	DROP_CASHES;

	printf("File remove start\n");

	if (long_file_name) {
		system("ls -lha /mnt/sdcard/rename");
	}
	else {
		system("ls -lha /mnt/sdcard/RENAME");
	}

	printf("----------------------------------\n");
	for (i = 0; i < MAX_LOOP_NUM; i++) {
		if (long_file_name) {
			sprintf(filename, "/mnt/sdcard/rename/re_%02d", i);
		}
		else {
			sprintf(filename, "/mnt/sdcard/RENAME/RE_%02d", i);
		}

		if (remove(filename) != 0) 
			return -1;
	}

	if (long_file_name) {
		system("ls -lha /mnt/sdcard/rename");
	}
	else {
		system("ls -lha /mnt/sdcard/RENAME");
	}

	printf("File remove end\n\n");

	printf("Dir remove start\n");
	system("ls -lha /mnt/sdcard");
	printf("----------------------------------\n");

	if (long_file_name) {
		system("rm -rf /mnt/sdcard/rename");
	}
	else {
		system("rm -rf /mnt/sdcard/RENAME");
	}

	system("ls -lha /mnt/sdcard");
	printf("Dir remove end\n\n");

	return 0;
}

int yes_no_check()
{
	char str[32];
	int i, c, ret;

	printf("Do you want to continue? (Y/N)\n");
	for (i = 0; i < (sizeof(str) - 1) && (c = getchar()) != '\n'; ++i)
		str[i] = c;
	str[i] = '\0';

	switch(str[0]) {
	case 'Y':
	case 'y':
		ret = 1;
		break;
	default:
		ret = 0;
	}
	return ret;
}

/******************************************************************************
 *  MAIN
 ******************************************************************************/
int main(int argc, char** argv)
{
	int ret = 0;
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	struct stat buf;


#ifdef IPCUFS_TRACE
/* mount debugfs */
	system("mount -t debugfs debugfs /sys/kernel/debug");
#endif /* IPCUFS_TRACE */

	if (argc >= 2) {
		if (strcmp(argv[1], "s") == 0) {
			long_file_name = 0;
		}
	}

/* make dir for mount */
	ret = stat(SD_PATH, &buf);
	if (ret != 0) {
		DROP_CASHES;
		ret = mkdir(SD_PATH, mode);
		if (ret != 0) {
			printf("mkdir NG\n");
			return -1;
		}
	}

/* ipcufs mount */
	ret = system("mount -t ipcufs mnt /mnt/sdcard -ofs=8");
	if (ret != 0) {
		printf("[NG] Mount ipcufs\n");
		goto end;
	}

/* make dir for RTOS SD */
	ret = stat(RTOS_SD_PATH, &buf);
	if(ret != 0){
		DROP_CASHES;
		system("mkdir /mnt/sdcard/ipcu");
	}

	printf("Create a new file on SD card on RTOS side\n");
	ret = create_base_file();
	if (ret == -1) {
		printf("[NG] Create file\n\n");
		goto end;
	}

	ret = copy_file_from_rtos();
	if (ret == -1) {
		printf("[NG] File Copy from RTOS\n\n");
		goto end;
	}

#ifdef IPCUFS_TRACE
/* trace on */
	system("echo 1 > /sys/kernel/debuf/trasing/events/ipcufs/enable");
#endif /* IPCUFS_TRACE */

	ret = read_seek_fp();
	if (ret == -1) {
		printf("[NG] Read seek with file pointer\n");
		if (yes_no_check() == 0)
			goto end;
	}

	ret = read_seek_fd();
	if (ret == -1) {
		printf("[NG] Read seek with file descriptor\n");
		if (yes_no_check() == 0)
			goto end;
	}

	ret = read_seek_o_dir();
	if (ret == -1) {
		printf("Read seek O_DIRECT NG\n");
		if (yes_no_check() == 0)
			goto end;
	}

	ret = write_files_of_seek();
	if (ret == -1) {
		printf("[NG] Write seek with file descriptor\n");
		if (yes_no_check() == 0)
			goto end;
	}

	ret = rename_test();
	if (ret == -1) {
		printf("[NG] Rename File and Dir\n\n");
		if (yes_no_check() == 0)
			goto end;
	}

	ret = remove_test();
	if (ret == -1)
		printf("[NG] Remove File and Dir\n\n");

end:
#ifdef IPCUFS_TRACE
/* trace off amd copy trace file to SD */
	system("echo 0 > /sys/kernel/debuf/trasing/events/ipcufs/enable");
	system("cp /sys/kernel/debug/tracing/trace /run/SD/mnt/sdcard");
#endif /* IPCUFS_TRACE */


/* unmount ipcufs */
	system("umount /mnt/sdcard");
#ifdef IPCUFS_TRACE
/* unmount debugfs */
	system("umount /sys/kernel/debug");
#endif /* IPCUFS_TRACE */
/* temp file delete */
	system("rm /run/SD/mmcblk0p1/ipcu*");
	system("rm /run/SD/mmcblk0p1/copy.dat");

	return ret;
}
