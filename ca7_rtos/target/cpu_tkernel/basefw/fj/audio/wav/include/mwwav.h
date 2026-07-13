/**
 * @file		mwwav.h
 * @brief		api for WAV file making
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _MWWAV_H
#define _MWWAV_H

/*==========================================================================*/
typedef struct{
	unsigned long	sps;		/* sample per second */
	unsigned long	sec;		/* total recording number of seconds[ms] */
	short*	pcm;				/* Top address of PCM	 */
	/* infomation */
	char*	icrd;	   			/* filming date(time nothing)*/
	/* The following is a comment of the exif file */
	unsigned short	wFormatTag;
	unsigned short	wBitsPerSample;
	unsigned char	channel;
	unsigned short  block_size;
	unsigned long	BytePerSec;
	unsigned long	file_size;
	unsigned long	data_size;
}mwwav_wav_info;

/*==========================================================================*/
/*  Function end status														*/
/*==========================================================================*/
enum {
	MWWAV_OK = 0,			/* nomarl end */
	MWWAV_NG = -1,			/* error end */
	MWWAV_CHUNK_NOT_FIND,	/* Chunk is not found */
	MWWAV_ID_NOT_FIND		/* ID is not found */
};

/*==========================================================================*/
/*
 *  Function to play/recording wav file
 */
/*==========================================================================*/

/** Get WAV Header Information */
extern int mwwav_wav_header_analysis( mwwav_wav_info *wav_info);

/*==========================================================================*/
/* File system Access Function												*/
/*==========================================================================*/

typedef struct{
	int (*f_write)(void *buf, unsigned long bytes);	// file write
	int (*f_read)(void *buf, unsigned long bytes);	// file read
	int (*f_seek)(long pos);					// file The number of bytes from the top
	int (*f_seek_cur)(long pow);				// file The number of bytes from the current
}WAV_Header_Func;

extern WAV_Header_Func FS_Func;
/*************************************************************************
TAG abauot WAVE FORMAT
*************************************************************************/

/* WAVE FORMAT Type */

/* wFormatTag */
#define PCM_TYPE		(0x0001)
#define a_Law_TYPE		(0x0006)
#define u_Law_TYPE		(0x0007)
#define IMA_ADPCM_TYPE	(0x0011)
#define NO_AUDIO		(0xFFFF)


#endif	//_MWWAV_H
