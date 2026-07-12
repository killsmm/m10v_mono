//******************************************************************************/
//
//  @file   fs_test.c
//  @brief  test sample for FS
//
//
//  Copyright 2015 Socionext Inc.
//******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>


/******************************************************************************
 *  @brief  driver test app
 *  @fn int main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 ******************************************************************************/
int main( int argc, char** argv )
{
	int ret;
	int len; 
	int fd;
	int flags = O_RDWR;
	char wr_buff[512] = {0};
	char rd_buff[512] = {0};
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	/* File Access : open, close, write, read, seek, verify*/
	if(strcmp(argv[1], "test_file") == 0) {
		printf("------------------------------\n");
		printf("  file test\n");
		printf("------------------------------\n");

		/* Open Processing */
 		flags |= O_CREAT;
		fd = open("/SD/fs_test1.txt", flags );
		if(fd < 0) {
			printf("[ERROR] failed to open.\n");
		    return 0;
		}
		else {
			printf("[OK] open completed.\n");
		}

		/* Write Processing */
		memcpy(wr_buff, "Hello Linux World.", 18);

		ret = write(fd, wr_buff, 18);
		if(ret < 0) {
			printf("[ERROR] failed to write %d \n", ret);
		    return 0;
		}
		else {
			printf("[OK] write completed. write length=%d\n", ret);
			printf("    write = %s\n", wr_buff);
		}
		
		/* Seek Processing */
		ret = lseek(fd, 0, SEEK_SET); /* file top */
		if(ret < 0) {
			printf("[ERROR] failed to lseek %d \n", ret);
		    return 0;
		}
		else {
			printf("[OK] lseek completed.\n");
		}

		/* Read Processing */
		ret = read(fd, rd_buff, 18);
		if(ret < 0) {
			printf("[ERROR] failed to read %d \n", ret);
		    return 0;
		}
		else {
			printf("[OK] read completed. read length=%d\n", ret);
			printf("    read = %s\n", rd_buff);
		}

		/* Verify Processing */
		len = ret;
		ret = memcmp(rd_buff, wr_buff, len);
		if(ret != 0) {
			printf("[ERROR] failed to verify %d \n", ret);
		    return 0;
		}
		else {
			printf("[OK] verify completed.\n");

		}

		/* Close Processing */
		ret = close(fd);
		if(ret != 0) {
			printf("[ERROR] failed to close %d \n", ret);
		}
		else {
			printf("[OK] close completed.\n");
		}
	}

	/* Dir Access : chdir, mkdir, rmdir */
	else if(strcmp(argv[1], "test_dir") == 0) {
		printf("------------------------------\n");
		printf("  directory test\n");
		printf("------------------------------\n");


		ret = rmdir("/SD/test_dir1");
		ret = rmdir("/SD/test_dir2");

		/* make dir Processing */
		ret = mkdir("/SD/test_dir1", mode);
		if(ret != 0) {
			printf("[ERROR] failed to mkdir%d \n", ret);
		}
		else {
			printf("[OK] mkdir completed.\n");
		}

		ret = mkdir("/SD/test_dir2", mode);
		if(ret != 0) {
			printf("[ERROR] failed to mkdir%d \n", ret);
		}
		else {
			printf("[OK] mkdir completed.\n");
		}
		
		/* remove dir Processing */
		ret = rmdir("/SD/test_dir2");
		if(ret != 0) {
			printf("[ERROR] failed to rmdir%d \n", ret);
		}
		else {
			printf("[OK] rmdir completed.\n");
		}
		
		/* change dir Processing */
		ret = chdir("/SD/test_dir1");
		if(ret != 0) {
			printf("[ERROR] failed to chdir%d \n", ret);
		}
		else {
			printf("[OK] chdir completed.\n");
		}
	}

    return 0;
}
