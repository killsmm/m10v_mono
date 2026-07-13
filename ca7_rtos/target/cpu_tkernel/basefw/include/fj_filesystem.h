/**
 * @file		fj_filesystem.h
 * @brief		FileSsystem Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_FILESYSTEM_H
#define _FJ_FILESYSTEM_H

#include "fj_std.h"
#include "fj_custom.h"
///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_filesystem_overview
@{
	- FS (RTOS version) <br>
		A File System (FS) is for RTOS version.<br>
		And it is used to control how data is stored and retrieved.<br>
	<br>
		[Feature]<br>
		<br>
		- Please refer to API description and sequence in File System module.<br>
		<br>
		@image html fs_file_system_overview.png
		<br>
	- VFS (Linux version) <br>
	    A Virtual File System (VFS) is an abstraction layer in RTOS side.<br>
	    And it allows RTOS applications to access concrete file systems in a uniform way.<br>
	<br>
		[Feature]<br>
		<br>
		- An applications can call Fie Sytem API in RTOS.(It means not to recognize the VFS function.)<br>
		- The Write or Read data can be transferred using either Shared-memory or Non-Shared-memory.<br>
		- VFS has internal data buffer and mechanism to transfer the data efficiently.<br>
		<br>
		@image html fs_virtual_file_system_overview.png
		<br>
		
@}*//* --- end of fj_filesystem_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_filesystem_sequence
@{
	- @ref fs_sequence_mount_sd
	- @ref fs_sequence_mount_nf
	- @ref fs_sequence_mount_ng
	- @ref fs_sequence_unmount_sd
	- @ref fs_sequence_unmount_nf
	- @ref fs_sequence_file_access
	- @ref fs_sequence_file_access_async
	- @ref fs_sequence_remove
	- @ref fs_sequence_format
	- @ref fs_sequence_set_attribute
	- @ref fs_sequence_entry_get
	- @ref fs_sequence_entry_search
	- @ref fs_sequence_entry_write_cb
	- @ref fs_sequence_start_write_cb
	- @ref fs_sequence_get_user_setting
	- @ref fs_sequence_set_user_setting
	
	<hr>
	@section fs_sequence_mount_sd Media Mount (SD Card)
	This is media(SD Card) mount sequence.<br>
	@image html fs_sequence_mount_sd.png
	@code
		int  ercd;
		unsigned char  wp_status;
		unsigned int   mode;
		unsigned long  clock;

		// Already initialize SD card controller(HW macro).

		ercd = FJ_SD_Card_Init(0);
		if (ercd != 0) {
			// SD Card initialize error
		}

		ercd = FJ_SD_GetCardInfo(0, &mode, &clock, &wp_status);
		if (ercd != 0) {
			// Get SD Card information error
		}

		ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
		if (ercd != 0) {
			// mount failed
			if (ercd == FJ_ERR_NOT_FAT) {
				// The TargetMedia is formatted excluding the FAT filesystem
				// -> Target Media need to Full-Format (FJ_FullFormatStorageMedia() API)
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_mount_nf Media Mount (NandFlash)
	This is media(NandFlash) mount sequence.<br>
	@image html fs_sequence_mount_nf.png
	@code
		int  ercd;

		// Already initialize NandFlash controller(HW macro) and Device.

		ercd = FJ_Mount(FJ_MEDIA_NF_STORAGE0, "J");
		if (ercd != 0) {
			// mount failed
			if (ercd == FJ_ERR_NOT_FAT) {
				// The TargetMedia is formatted excluding the FAT filesystem
				// -> Target Media need to Full-Format (FJ_FullFormatStorageMedia() API)
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_mount_ng Media Mount (Mount Error)
	This is media mount error sequence.<br>
	@image html fs_sequence_mount_ng.png
	@code
		int  ercd;
		unsigned long progress;
		unsigned short result;

		ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
		if (ercd != 0) {
			// mount failed
			if (ercd == FJ_ERR_NOT_FAT) {
				// The TargetMedia is formatted excluding the FAT filesystem
				// -> Target Media need to Full-Format (FJ_FullFormatStorageMedia() API)

				ercd = FJ_FullFormatStorageMedia(FJ_MEDIA_SD0);
				if (ercd == 0) {
					while (1) {
						OS_dly_tsk(100);					// some times delay task for FileSystem process
						ercd = FJ_GetFormatProgress(&progress);
						if (ercd != 0) {
							// Full Format Failed
							break;
						}
						printf("now progress = %d\r\n", progress);
						if (progress == 100) {
							// Full Format Successfull
							// Mount Storage Media
							ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
							break;
						}
					}
				}
			}
			else {
				// This device can not be used.
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_unmount_sd Media UnMount (SD Card)
	This is media(SD Card) unmount sequence.<br>
	@image html fs_sequence_unmount_sd.png
	@code
		int  ercd;

		ercd = FJ_UnMount("I");
		if (ercd != 0) {
			// unmount failed
		}
	@endcode
	
	@section fs_sequence_unmount_nf Media UnMount (NandFlash)
	This is media(NandFlash) unmount sequence.<br>
	@image html fs_sequence_unmount_nf.png
	@code
		int  ercd;

		ercd = FJ_UnMount("J");
		if (ercd != 0) {
			// unmount failed
		}
	@endcode
	
	<hr>
	@section fs_sequence_file_access File Access (Sync)
	This is file access(sync) sequence.<br>
	@image html fs_sequence_file_access.png
	@code
		int ercd;
		unsigned long FileID, length;
		unsigned char read_buf[];

		ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
		if (ercd != 0) {
			// mount failed
		}

		FileID = FJ_Fs_Open("I:\\test.bin", FJ_FS_OPEN_RDWR);
		if (FileID == 0) {
			// file open error
		}

		ercd = FJ_Fs_Read(FileID, read_buf, 512, &length);
		if (ercd != 0) {
			// file read error
		}

		ercd = FJ_Fs_Close(FileID);
		if (ercd != 0) {
			// file close error
		}
	@endcode
	
	<hr>
	@section fs_sequence_file_access_async File Access (Async)
	This is file access(async) sequence.<br>
	@image html fs_sequence_file_access_async.png
	@code
		VOID Sample_Callback()
		{
			unsigned long length;

			ercd = FJ_Fs_GetWriteLengthAs(&length);
			if (ercd != 0) {
				// file write error
			}

			ercd = FJ_Fs_Close(FileID);
			if (ercd != 0) {
				// file close error
			}
		}

		VOID Sample_Code()
		{
			int ercd;
			unsigned char write_buf[];

			ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
			if (ercd != 0) {
				// mount failed
			}

			FileID = FJ_Fs_Open("I:\\test.bin", FJ_FS_OPEN_RDWR);
			if (FileID == 0) {
				// file open error
			}

			ercd = FJ_Fs_WriteAsync(Sample_Callback, FileID, write_buf, 512);
			if (ercd != 0) {
				// file write error
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_remove Remove
	This is file remove sequence.<br>
	@image html fs_sequence_remove.png
	@code
		VOID Sample_Callback()
		{
			// multi remove complete
		}

		VOID Sample_Code()
		{
			int ercd;
			char file_name[3];

			ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
			if (ercd != 0) {
				// mount failed
			}

			ercd = FJ_Fs_Remove("I:\\test.bin");
			if (ercd != 0) {
				// file remove error
			}

			file_name[0] = "test1.bin";
			file_name[1] = "test2.bin";
			file_name[2] = "test3.bin";

			ercd = FJ_Fs_RemoveMulti("I:\\TEST", file_name, 3, Sample_Callback);
			if (ercd != 0) {
				// file remove error
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_format Format
	This is format storage media sequence.<br>
	@image html fs_sequence_format.png
	@code
		int ercd;
		unsigned long progress;

		ercd = FJ_SetVolLabel("I", "DigitalCam");	// volume label set
		// If you wants to delete volume label settings, please call this function as follow
		ercd = FJ_SetVolLabel("I", "\0");			// volume label delete

		ercd = FJ_FormatStorageMedia(FJ_MEDIA_SD0);
		if (ercd != 0) {
			// Quick Format Failed
		}
	@endcode
	@code
		// Quick Format or Mount Media failed
		ercd = FJ_SetVolLabel("I", "DigitalCam");	// volume label set
		// If you wants to delete volume label settings, please call this function as follow
		ercd = FJ_SetVolLabel("I", "\0");			// volume label delete

		ercd = FJ_FullFormatStorageMedia(FJ_MEDIA_SD0);
		if (ercd == 0) {
			while (1) {
				OS_dly_tsk(100);					// some times delay task for FileSystem process
				ercd = FJ_GetFormatProgress(&progress);
				if (ercd != 0) {
					// Full Format Failed
					break;
				}
				printf("now progress = %d\r\n", progress);
				if (progress == 100) {
					// Full Format Successfull
					// Mount Storage Media
					ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
					break;
				}
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_set_attribute Set Attribute
	This is file access(async) sequence.<br>
	@image html fs_sequence_set_attribute.png
	@code
		VOID Sample_Callback()
		{
			// multi set complete
		}

		VOID Sample_Code()
		{
			int ercd;
			char file_name[3];

			ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
			if (ercd != 0) {
				// mount failed
			}

			ercd = FJ_Fs_SetAttribute("I:\\test.bin", FJ_FS_ATTR_READONLY);
			if (ercd != 0) {
				// file remove error
			}

			file_name[0] = "test1.bin";
			file_name[1] = "test2.bin";
			file_name[2] = "test3.bin";

			ercd = FJ_Fs_RemoveMulti("I:\\TEST", FJ_FS_ATTR_READONLY, FJ_FS_ATTR_MODE_SET, file_name, 3, Sample_Callback);
			if (ercd != 0) {
				// file remove error
			}
		}
	@endcode
	
	<hr>
	@section fs_sequence_entry_get Get Current Entry
	This is get current entry sequence.<br>
	@image html fs_sequence_entry_get.png
	@code
		int ercd;
		unsigned int  attr;
		unsigned long size, entry_cnt=0;
		unsigned char entry[100][];

		ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
		if (ercd != 0) {
			// mount failed
		}

		ercd = FJ_Fs_Chdir("I:\\");
		if (ercd != 0) {
			// change directory error
		}

		ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_START, &entry[entry_cnt], 16, &attr, &size);
		if (ercd == FJ_ERR_NG) {
			// get entry error
		}
		entry_cnt++;

		while(ercd == FJ_ERR_LAST_ENTRY) {
			ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_NEXT, &entry[entry_cnt], 16, &attr, &size);
			if (ercd == FJ_ERR_NG) {
				// get entry error
			}
			entry_cnt++;
		}

		ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_END, NULL, NULL, &attr, &size);
		if (ercd == FJ_ERR_NG) {
			// get entry error
		}
	@endcode
	
	<hr>
	@section fs_sequence_entry_search Get Current Entry (search files)
	This is get current entry of search files sequence.<br>
	@image html fs_sequence_entry_search.png
	@code
		int ercd;
		unsigned int  attr;
		unsigned long size, entry_cnt=0;
		unsigned char entry[100][];

		ercd = FJ_Mount(FJ_MEDIA_SD0, "I");
		if (ercd != 0) {
			// mount failed
		}

		ercd = FJ_Fs_Chdir("I:\\");
		if (ercd != 0) {
			// change directory error
		}

		ercd = FJ_Fs_SetSearchEntry("TEST*");
		if (ercd == FJ_ERR_NG) {
			// set search entry error
		}

		ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_START, &entry[entry_cnt], 16, &attr, &size);
		if (ercd == FJ_ERR_NG) {
			// get entry error
		}
		entry_cnt++;

		while(ercd == FJ_ERR_LAST_ENTRY) {
			ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_NEXT, &entry[entry_cnt], 16, &attr, &size);
			if (ercd == FJ_ERR_NG) {
				// get entry error
			}
			entry_cnt++;
		}

		ercd = FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE_END, NULL, NULL, &attr, &size);
		if (ercd == FJ_ERR_NG) {
			// get entry error
		}
	@endcode
	
	<hr>
	@section fs_sequence_entry_write_cb Entry callback function for writing error.(for Linux version)
	This is entry callback to handle VFS error sequence.<br>
	@image html fs_sequence_vfs_callback.png
	
	<hr>
	@section fs_sequence_start_write_cb Start callback function for writing error.(for Linux version)
	This is VFS callback start sequence.<br>
	@image html fs_sequence_set_vfs_callback.png
	
	<hr>
	@section fs_sequence_get_user_setting Load User Setting data.
	This is load User Setting data from main area to work area sequence.<br>
	Main area is to load from NF.<br>
	Work area is to update setting value.<br>
	@image html fs_sequence_get_user_setting.png
	
	<hr>
	@section fs_sequence_set_user_setting Update User Setting data.(for Linux version)
	This is update User Setting data from work area to NF sequence.<br>
	@image html fs_sequence_set_user_setting.png
	
@}*//* --- end of fj_filesystem_sequence */

/*----------------------------------------------------------------------*/
/* Definition														    */
/*----------------------------------------------------------------------*/
/** @weakgroup fj_filesystem_definition
@{*/
// File type
//#define FJ_FILE_TYPE_JPEG		0x0001	/**< File Type JPEG */
//#define FJ_FILE_TYPE_TIFF		0x0002	/**< File Type TIFF(Not used) */
//#define FJ_FILE_TYPE_WAV		0x0004	/**< File Type WAV */
//#define FJ_FILE_TYPE_AVI		0x0008	/**< File Type AVI */
//#define FJ_FILE_TYPE_MPEG		0x0020	/**< File Type MPEG(Not used) */
//#define FJ_FILE_TYPE_MP4		0x0040	/**< File Type MP4 */
//#define FJ_FILE_TYPE_ASF		0x0080	/**< File Type ASF(Not used) */
//#define FJ_FILE_TYPE_WMV		0x0100	/**< File Type WMV(Not used) */
//#define FJ_FILE_TYPE_WMA		0x0200	/**< File Type WMA(Not used) */
//#define FJ_FILE_TYPE_MOV		0x0400	/**< File Type MOV */
//#define FJ_FILE_TYPE_TXT		0x0800	/**< File Type TXT(Not used) */

#define FJ_FILE_TYPE_JPEG		0x0001	/**< File Type JPEG */
//#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
#define FJ_FILE_TYPE_MP4		0x0002	/**< File Type MP4 */
//#else
#define FJ_FILE_TYPE_MOV		0x0004	/**< File Type MOV */
//#endif
#define FJ_FILE_TYPE_RAW		0x0008	/**< File Type RAW */
#define FJ_FILE_TYPE_WAV		0x0010	/**< File Type WAV */
//temporary
#define FJ_FILE_TYPE_THM		0x0020	/**< File Type THM */
#define FJ_FILE_TYPE_AVI		0x0040	/**< File Type AVI(Not used) */
#define FJ_FILE_TYPE_TIFF		0x0080	/**< File Type TIFF(Not used) */
#define FJ_FILE_TYPE_ASF		0x0100	/**< File Type ASF(Not used) */
#define FJ_FILE_TYPE_WMV		0x0200	/**< File Type WMV(Not used) */
#define FJ_FILE_TYPE_WMA		0x0400	/**< File Type WMA(Not used) */
#define FJ_FILE_TYPE_MPEG		0x0800	/**< File Type MPEG(Not used) */
#define FJ_FILE_TYPE_TXT		0x1000	/**< File Type TXT(Not used) */

// Media type
#define FJ_USED_MEDIA_UNKNOWN	0x00	/**< Unknown Media */
#define FJ_USED_MEDIA_NF		0x01	/**< NandFlash */
#define FJ_USED_MEDIA_SD		0x02	/**< SD Card */
#define FJ_USED_MEDIA_SDHC		0x03	/**< SDHC Card */
#define FJ_USED_MEDIA_SDXC		0x04	/**< SDXC Card */
#define FJ_USED_MEDIA_EMMC		0x05	/**< EMMC */

// File open mode
/**
File Open Mode for @ref FJ_Fs_Open<br>
	ReadOnly mode : Only enable the read access
*/
#define FJ_FS_OPEN_RDONLY		0x0000
/**
File Open Mode for @ref FJ_Fs_Open<br>
WriteOnly mode : Only enable the write access
*/
#define FJ_FS_OPEN_WRONLY		0x0001
/**
File Open Mode for @ref FJ_Fs_Open<br>
ReadWrite mode : Enable the read/write access
*/
#define FJ_FS_OPEN_RDWR			0x0002
/**
File Open Mode for @ref FJ_Fs_Open<br>
Append mode : After open the file, move the write operation file pointer to the end of file
*/
#define FJ_FS_OPEN_APPEND		0x0008
/**
File Open Mode for @ref FJ_Fs_Open<br>
Create mode : Create new file when no file is existed
*/
#define FJ_FS_OPEN_CREAT		0x0100
/**
File Open Mode for @ref FJ_Fs_Open<br>
Trunc mode : Set the file size to "0" when opening file (over write)
*/
#define FJ_FS_OPEN_TRUNC		0x0200

// File attribute
/**
File Attribute Normal<br>
normal file
*/
#define FJ_FS_ATTR_NORMAL		0x00
/**
File Attribute ReadOnly<br>
read only file
*/
#define FJ_FS_ATTR_READONLY		0x01
/**
File Attribute Hidden<br>
hidden file
*/
#define FJ_FS_ATTR_HIDDEN		0x02
/**
File Attribute System<br>
system file
*/
#define FJ_FS_ATTR_SYSTEM		0x04
/**
File Attribute Directory<br>
directory
*/
#define FJ_FS_ATTR_DIRECTORY	0x10
/**
File Attribute Archive<br>
archive file
*/
#define FJ_FS_ATTR_ARCHIVE		0x20
/**
File Attribute Mode to set
*/
#define FJ_FS_ATTR_MODE_SET		0
/**
File Attribute Mode to add
*/
#define FJ_FS_ATTR_MODE_ON		1
/**
File Attribute Mode to except
*/
#define FJ_FS_ATTR_MODE_OFF		2
// FAT type
/**
File System Type Unknown<br>
Unknown File System
*/
#define FJ_FS_TYPE_UNKNOWN		0x00
/**
File System Type FAT12<br>
FAT12 File System
*/
#define FJ_FS_TYPE_FAT12		0x01
/**
File System Type FAT16<br>
FAT16 File System (<= 32MByte)
*/
#define FJ_FS_TYPE_FAT16		0x04
/**
File System Type FAT16E<br>
FAT16 File System (> 32MByte)
*/
#define FJ_FS_TYPE_FAT16E		0x06
/**
File System Type exFAT<br>
exFAT File System
*/
#define FJ_FS_TYPE_EXFAT		0x07
/**
File System Type FAT32<br>
FAT32 File System
*/
#define FJ_FS_TYPE_FAT32		0x0B
/**
File System Type FAT32X<br>
FAT32C File System
*/
#define FJ_FS_TYPE_FAT32X		0x0C

// Dir Exist Status
/**
Directory Exist Flag<br>
not exist
*/
#define FJ_STATUS_DIR_NO_EXIST	0x00
/**
Directory Exist Flag<br>
exist
*/
#define FJ_STATUS_DIR_EXIST		0x01

/**
Kind of set TimeStamp for @ref FJ_Fs_UtimeEx<br>
Modify date and time are targeted.
*/
#define FJ_FS_TIMESTAMP_MODIFY		0x01

/**
Kind of set TimeStamp for @ref FJ_Fs_UtimeEx<br>
Create date and time are targeted.
*/
#define FJ_FS_TIMESTAMP_CREATE		0x02

/**
Kind of set TimeStamp for @ref FJ_Fs_UtimeEx<br>
Access date and time are targeted.
*/
#define FJ_FS_TIMESTAMP_ACCESS		0x04

/**
Kind of seek origin for @ref FJ_Fs_Lseek<br>
Seek from file top
*/
#define FJ_FS_SEEK_SET				0x00

/**
Kind of seek origin for @ref FJ_Fs_Lseek<br>
Seek from current
*/
#define FJ_FS_SEEK_CUR				0x01

/**
Kind of seek origin for @ref FJ_Fs_Lseek<br>
Seek from file end
*/
#define FJ_FS_SEEK_END				0x02

/**
Callback function for @ref FJ_Fs_RemoveMulti<br>
*/
typedef UINT32 (*FsDeleteMultiCB)(UINT32, UINT32);

/**
Callback function for @ref FJ_Fs_SetAttributeMulti<br>
*/
typedef UINT32 (*FsAttributesSetMultiCB)(UINT32, UINT32);

/**
Callback function for @ref FJ_Fs_WriteAsync<br>
*/
typedef VOID (*FsEntryCB)(INT32);

/**
Callback function for @ref FJ_Set_VFS_Write_Err_CB<br>
*/
typedef VOID (*FsWriteErrCB)(ULONG, INT32);

/*----------------------------------------------------------------------*/
/* Enumeration														    */
/*----------------------------------------------------------------------*/
/**
Media ID
*/
typedef enum {
	FJ_MEDIA_NF_STORAGE0,				/**< NandFlash Storage 0 */
	FJ_MEDIA_NF_STORAGE1,				/**< NandFlash Storage 1 */
	FJ_MEDIA_NF_STORAGE2,				/**< NandFlash Storage 2 */
	FJ_MEDIA_NF_STORAGE3,				/**< NandFlash Storage 3 */
	FJ_MEDIA_NF_STORAGE4,				/**< NandFlash Storage 4 */
	FJ_MEDIA_NF_STORAGE5,				/**< NandFlash Storage 5 */
	FJ_MEDIA_NF_STORAGE6,				/**< NandFlash Storage 6 */
	FJ_MEDIA_SD0,						/**< SD card 0 */
	FJ_MEDIA_SD1,						/**< SD card 1 */
	FJ_MEDIA_EMMC_STORAGE0,				/**< eMMC Storage 0	*/
	FJ_MEDIA_EMMC_STORAGE1,				/**< eMMC Storage 1	*/
	FJ_MEDIA_EMMC_STORAGE2,				/**< eMMC Storage 2	*/
	FJ_MEDIA_EMMC_STORAGE3,				/**< eMMC Storage 3	*/
	FJ_MEDIA_EMMC_STORAGE4,				/**< eMMC Storage 4	*/
	FJ_MEDIA_EMMC_STORAGE5,				/**< eMMC Storage 5	*/
	FJ_MEDIA_EMMC_STORAGE6				/**< eMMC Storage 6	*/
} FJ_MEDIA_ID;

/**
Entry mode
*/
typedef enum {
	FJ_FS_ENTRY_MODE_START,				/**< Start get Entry function. */
	FJ_FS_ENTRY_MODE_NEXT,				/**< Get next Entry. */
	FJ_FS_ENTRY_MODE_NEXT_NOT_SUB,		/**< Get next Entry (Not read Sub Dir). */
	FJ_FS_ENTRY_MODE_END				/**< Reset and End get Entry function. */
} FJ_FS_ENTRY_MODE;


/*----------------------------------------------------------------------*/
/* Structure														    */
/*----------------------------------------------------------------------*/
/** file information */
typedef struct {
	USHORT	create_date;		/**< create date of requested file or directory.
									<br>
									The meaning to have of bit of the "create_date"
									<table>
									<tr>
										<th>Bit</th>
										<th>Meaning</th></tr>
									<tr>
										<td>0-4</td>
										<td>Day (0 - 31)</td></tr>
									<tr>
										<td>5-8</td>
										<td>Month (0 - 12)</td></tr>
									<tr>
										<td>9-15</td>
										<td>Year - 1980 (0 - 127)</td></tr>
									</table>
									The calculation method of the real value.<br>
									Year = (date & 0xFC00) + 1980<br>
								*/
	USHORT	create_time;		/**< create time of requested file or directory.
									<br>
									The meaning to have of bit of the "create_time"
									<table>
									<tr>
										<th>Bit</th>
										<th>Meaning</th></tr>
									<tr>
										<td>0-4</td>
										<td>Second / 2 (0 - 29)</td></tr>
									<tr>
										<td>5-10</td>
										<td>Minute (0 - 59)</td></tr>
									<tr>
										<td>11-15</td>
										<td>Hour (0 - 23)</td></tr>
									</table>
									<br>
									The calculation method of the real value.<br>
									Second = (time & 0x001F) * 2<br>
								*/
	USHORT	modify_date;		/**< modify date of requested file or directory.<br>
									The meaning of the bit of "modify_date" is the same meaning as "create_date".<br>
									Please look at "create_date" about the meaning of the bit of "modify_date". */
	USHORT	modify_time;		/**< modify time of requested file or directory.<br>
									The meaning of the bit of "modify_time" is the same meaning as "create_time".<br>
									Please look at "create_time" about the meaning of the bit of "modify_time". */
	ULONG	size;				/**< size(byte) of requested file or directory. */
} FJ_FS_STAT;

/** file information(expand) */
typedef struct {
	USHORT	create_date;		/**< Create date of requested file or directory.
									<br>
									The meaning to have of bit of the "create_date"
									<table>
									<tr>
										<th>Bit</th>
										<th>Meaning</th></tr>
									<tr>
										<td>0-4</td>
										<td>Day (0 - 31)</td></tr>
									<tr>
										<td>5-8</td>
										<td>Month (0 - 12)</td></tr>
									<tr>
										<td>9-15</td>
										<td>Year - 1980 (0 - 127)</td></tr>
									</table>
									<br>
									The calculation method of the real value.<br>
									Year = (date & 0xFC00) + 1980<br>
								*/
	ULONG	create_time;		/**< Create time of requested file or directory.
									<br>
									The meaning to have of bit of the "create_time"
									<table>
									<tr>
										<th>Bit</th>
										<th>Meaning</th></tr>
									<tr>
										<td>0-5</td>
										<td>Second (0 - 59)</td></tr>
									<tr>
										<td>6-11</td>
										<td>Minute (0 - 59)</td></tr>
									<tr>
										<td>12-16</td>
										<td>Hour (0 - 23)</td></tr>
									</table>
								*/
	USHORT	modify_date;		/**< Modify date of requested file or directory.<br>
									The meaning of the bit of "modify_date" is the same meaning as "create_date".<br>
									Please look at "create_date" about the meaning of the bit of "modify_date". */
	ULONG	modify_time;		/**< Modify time of requested file or directory.<br>
									The meaning of the bit of "modify_time" is the same meaning as "create_time".<br>
									Please look at "create_time" about the meaning of the bit of "modify_time". */
	ULONG	size_ms;			/**< High word size(byte) of requested file or directory. */
	ULONG	size_ls;			/**< Low word size(byte) of requested file or directory. */
} FJ_FS_STAT_EX;

/** Set date and time information */
typedef struct {
	USHORT	date;				/**< This is date set to the demanded file or directory.<br>
									<br>
									The meaning to have of bit of the "date"
									<table>
									<tr>
										<th>Bit</th>
										<th>Meaning</th></tr>
									<tr>
										<td>0-4</td>
										<td>Day (0 - 31)</td></tr>
									<tr>
										<td>5-8</td>
										<td>Month (0 - 12)</td></tr>
									<tr>
										<td>9-15</td>
										<td>Year - 1980 (0 - 127)</td></tr>
									</table>
									<br>
									The calculation method of the real value.<br>
									Year = (date & 0xFC00) + 1980<br>
								*/
	ULONG	time;				/**< This is time set to the demanded file or directory.<br>
									<br>
									The meaning to have of bit of the "time"
									<table>
									<tr>
										<th>Bit</th>
										<th>Meaning</th></tr>
									<tr>
										<td>0-5</td>
										<td>Second (0 - 59)</td></tr>
									<tr>
										<td>6-11</td>
										<td>Minute (0 - 59)</td></tr>
									<tr>
										<td>12-16</td>
										<td>Hour (0 - 23)</td></tr>
									</table>
								*/
} FJ_FS_TIME;

/** The storage media information */
typedef struct {
	UCHAR	media;				/**< The Media Type of using.<br> 
								<ul><li>@ref FJ_USED_MEDIA_UNKNOWN(0x00)
									<li>@ref FJ_USED_MEDIA_NF(0x01)
									<li>@ref FJ_USED_MEDIA_SD(0x02)
									<li>@ref FJ_USED_MEDIA_SDHC(0x03)
									<li>@ref FJ_USED_MEDIA_SDXC(0x04)</ul> */
	USHORT	BytePerSector;		/**< byte per sector. */
	USHORT	SectorPerCluster;	/**< sector per cluster. */
	UINT32	TotalClusters;		/**< total cluster num. */
	UINT32	FreeClusters;		/**< free cluster num. */
	UCHAR	FatType;			/**< filesystem type. */
} FJ_FS_GEOMETRY;

/*----------------------------------------------------------------------*/
/* Global Data														    */
/*----------------------------------------------------------------------*/
// Nothing Special
/*@} --- end of fj_filesystem_definition */

/*----------------------------------------------------------------------*/
/* Macro															    */
/*----------------------------------------------------------------------*/
/** @weakgroup fj_filesystem_api
@{*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function															    */
/*----------------------------------------------------------------------*/
// File Operation
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function opens the requested file and returns the File ID.
@param[in] szName The name of file to be opened
@param[in] dwFlags Opening flags<br>
					= @ref FJ_FS_OPEN_RDONLY Open file with Read Only mode.<br>
					= @ref FJ_FS_OPEN_WRONLY Open file with Write Only mode.<br>
					= @ref FJ_FS_OPEN_RDWR Open file with nomal mode (reading and writing is possible).<br>
					= @ref FJ_FS_OPEN_APPEND After open the file, move the write operation file pointer to the end of file.<br>
					= @ref FJ_FS_OPEN_CREAT Create new file and open when no file is existed.<br>
					= @ref FJ_FS_OPEN_TRUNC Open file with Over Write mode.<br>
@return File ID
@remarks
File ID:0 is file open error.<br>
Please unite @ref FJ_FS_OPEN_APPEND or @ref FJ_FS_OPEN_CREAT or @ref FJ_FS_OPEN_TRUNC with @ref FJ_FS_OPEN_WRONLY or @ref FJ_FS_OPEN_RDWR by all means.<br>
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
ULONG		FJ_Fs_Open(CHAR* szName, INT32 dwFlags);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function closes the requested file.
@param[in] ulFileId The ID of the file to close
*/
VOID		FJ_Fs_Close(ULONG ulFileId);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function reads requested number of bytes from the specified file into a specified buffer.
@param[in] ulFileId The ID of the file to read
@param[out] pbyBuffer The buffer address for read
@param[in] ulRequestSize The requested number of bytes to read
@param[out] ulLength The byte count that was read
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_READ_EOF Reached to the end of the file.
*/
FJ_ERR_CODE	FJ_Fs_Read(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function writes requested number of bytes to the specified file from a specified buffer.
@param[in] ulFileId The ID of the file to write
@param[in] pbyBuffer The buffer address for write
@param[in] ulRequestSize The requested number of bytes to write
@param[out] ulLength The byte count that was write
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_Write(ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize, ULONG* ulLength);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function writes requested number of bytes to the specified file from a specified buffer.(Async)
@param[in] pCallback callback function pointer
@param[in] ulFileId The ID of the file to write
@param[in] pbyBuffer The buffer address for write
@param[in] ulRequestSize The requested number of bytes to write
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_WriteAsync(FsEntryCB pCallback, ULONG ulFileId, UINT32* pbyBuffer, ULONG ulRequestSize);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the actual written result.
@param[out] length Specify the pointer of area which is stored the actual written size.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
If necessary, use this function in a callback to set in FJ_Fs_WriteAsync().<br>

*/
FJ_ERR_CODE FJ_Fs_GetWriteLengthAs(ULONG* length);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function deletes the requested file/directory. When deleting a directory it must be empty of files.
@param[in] szName The name of file/directory to be deleted
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_Remove(CHAR* szName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function deletes the requested file/directory When deleting a directory it must be empty of files.
@param[in] szName The name of directory to be deleted.
@param[in] filename_list A list of entries that are eligible for deletion.(13 is the size of the number of characters plus the NULL to the 8.3 format.)
@param[in] iFnum The number of elements in the list of list of entries for removal.(max=10000)
@param[in] pcallback A callback function that gets called every time it processes a single entry.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.<br>
The maximum number of entries in the list is the same as the number of files that can be acceptable to a directory.<br>
*/
FJ_ERR_CODE FJ_Fs_RemoveMulti(CHAR* szName, CHAR filename_list[][13], INT32 iFnum, FsDeleteMultiCB pcallback);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
The function create the requested directory.
@param[in] szName The name of directory to created
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_Mkdir(CHAR* szName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function changes the path from the current directory to the requeste path.
@param[in] szInputPath The requested directory path to change to
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_Chdir(CHAR* szInputPath);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function renames the requested file/directory.
@param[in] szSrcName The requested source file/directory name
@param[in] szDstName The requested destination file/directory name
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_Rename(CHAR* szSrcName, CHAR* szDstName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function returns the atrtbiutes of requested file or directory.
@param[in] szName The name of the directory or file
@param[out] puiAttributes The attribute of the requested directory or file<br>
							= @ref FJ_FS_ATTR_NORMAL : normal file or directory<br>
							= @ref FJ_FS_ATTR_READONLY : readonly file or directory<br>
							= @ref FJ_FS_ATTR_HIDDEN : hidden file or directory<br>
							= @ref FJ_FS_ATTR_SYSTEM : system file or directory<br>
							= @ref FJ_FS_ATTR_ARCHIVE : archive file or directory<br>
							= @ref FJ_FS_ATTR_DIRECTORY : directory<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_GetAttribute(CHAR* szName, UINT32* puiAttributes);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the atrtbiutes to requested file or directory.
@param[in] szName The name of the directory or file
@param[in] puiAttributes The attribute to set the requested directory or file<br>
							= @ref FJ_FS_ATTR_NORMAL : normal file or directory<br>
							= @ref FJ_FS_ATTR_READONLY : readonly file or directory<br>
							= @ref FJ_FS_ATTR_HIDDEN : hidden file or directory<br>
							= @ref FJ_FS_ATTR_SYSTEM : system file or directory<br>
							= @ref FJ_FS_ATTR_ARCHIVE : archive file or directory<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE	FJ_Fs_SetAttribute(CHAR* szName, UINT32 puiAttributes);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set the atrtbiutes to requested file or directory.
@param[in] szName The name of the directory or file
@param[in] puiAttributes The attribute to set the requested directory or file<br>
							= @ref FJ_FS_ATTR_NORMAL : normal file or directory<br>
							= @ref FJ_FS_ATTR_READONLY : readonly file or directory<br>
							= @ref FJ_FS_ATTR_HIDDEN : hidden file or directory<br>
							= @ref FJ_FS_ATTR_SYSTEM : system file or directory<br>
							= @ref FJ_FS_ATTR_ARCHIVE : archive file or directory<br>
@param[in] ucAttrmode Mode attribute setting<br>
							= @ref FJ_FS_ATTR_MODE_SET : To change the attributes specified in the argument "ucAttrmode", the attributes of the target entry.<br>
							= @ref FJ_FS_ATTR_MODE_ON : The attributes of the target entry, add an attribute that is specified in the attributes argument.<br>
							= @ref FJ_FS_ATTR_MODE_OFF : From the attributes of the target entry, except the attributes specified in the attributes argument.<br>
@param[in] filename_list A list of entries that are eligible for deletion.(13 is the size of the number of characters plus the NULL to the 8.3 format.)<br>
@param[in] iFnum The number of elements in the list of list of entries for set attribute.(max=10000)
@param[in] pcallback A callback function that gets called every time it processes a single entry.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.<br>
The maximum number of entries in the list is the same as the number of files that can be acceptable to a directory.<br>
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.
// --- REMOVE_PRFILE2 END ---
*/
FJ_ERR_CODE FJ_Fs_SetAttributeMulti(CHAR* szName, UINT32 puiAttributes, UINT32 ucAttrmode, CHAR filename_list[][13], INT32 iFnum, FsAttributesSetMultiCB pcallback);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets information of requested file or directory.
@param[in] szName The name of the file or directory
@param[out] pStat Information of requested file or directory. See @ref FJ_FS_STAT
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
In all media format, the accuracy of "create_time" and "modify_time" is two seconds.<br>
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.
*/
FJ_ERR_CODE FJ_Fs_Stat(CHAR* szName, FJ_FS_STAT* pStat);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets information of requested file or directory.<br>
Create time and modify time and, size of the file or directory data size accuracy has been expanded.
@param[in] szName The name of the file or directory
@param[out] pStatEx Information of requested file or directory. See @ref FJ_FS_STAT_EX
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
When the media format is FAT12, FAT16, and FAT32, the accuracy of "create_time" and "modify_time" is two seconds.<br>
When the media format is FAT12, FAT16, and FAT32, the accuracy of "create_time" is one second or two seconds. Because it is file dependence.<br>
When the media format is exFAT *1, the accuracy of "create_time" and "modify_time" is one seconds.<br>
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.<br>
<br>
 * *1 To use the exFAT is a license required.
*/
FJ_ERR_CODE FJ_Fs_StatEx(CHAR* szName, FJ_FS_STAT_EX* pStatEx);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function changes the timestamp of demanded file or directory.<br>
@param[in] szName The name of the file or directory
@param[in] timestamp_type The kind of the TimeStamp that changes is set.<br>
						The parameter that can be set is three of the following.<br>
						@ref FJ_FS_TIMESTAMP_MODIFY, @ref FJ_FS_TIMESTAMP_CREATE, @ref FJ_FS_TIMESTAMP_ACCESS
@param[in] ptime Information of date and time. See @ref FJ_FS_TIME
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
Timestamp_type can set two or more date type by using OR operator ("|").<br>
For example, when timestamp_type is set as follows, "modify time" and "create time" are set.<br>
FJ_FS_TIMESTAMP_MODIFY | FJ_FS_TIMESTAMP_CREATE<br>
<br>
FAT12, 16, and 32 of the modify time become it every two seconds.<br>
exFAT *1 becomes it every second.<br>
FAT12, 16, and 32 cannot set the access time.<br>
exFAT *1 of the access time becomes it every two seconds.<br>
<br>
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.<br>
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.<br>
// --- REMOVE_PRFILE2 END ---
<br>
 * *1 To use the exFAT is a license required.
*/
FJ_ERR_CODE FJ_Fs_UtimeEx(CHAR* szName, USHORT timestamp_type, FJ_FS_TIME* ptime);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function moves the internal file pointer to the specified byte offset.
@param[in] ulFileId The ID of the file to seek
@param[in] ulOffset The offset into the file
@param[in] whence Indicate the standard position of offset<br>
				  = 0 : seek from file top<br>
				  = 1 : seek from current<br>
				  = 2 : seek from file end<br>
@param[in] pulLength Pointer for storing the file pointer after moving
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_Fs_Lseek(ULONG ulFileId, ULONG ulOffset, USHORT whence, ULONG* pulLength);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function moves the internal file pointer to the specified byte offset.
@param[in] ulFileId The ID of the file to seek
@param[in] iOffset The offset into the file (Max size : 0x7FFFFFFFFFFFFFFF)
@param[in] whence Indicate the standard position of offset<br>
				  = 0 : seek from file top<br>
				  = 1 : seek from current<br>
				  = 2 : seek from file end<br>
@param[in] piLength Pointer for storing the file pointer after moving
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_Fs_Lseek64(ULONG ulFileId, INT64 iOffset, USHORT whence, INT64* piLength);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function moves the internal file pointer to the specified byte offset.<br>
@param[in] ulFileId The ID of the file to seek
@param[in] ulOffset The offset into the file
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The standard position is current file pointer.<br>
If it exceeds the file size, file pointer becomes the end of the file.<br>
*/
FJ_ERR_CODE	FJ_Fs_LseekWithLimit(ULONG ulFileId, ULONG ulOffset);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function flushes the device at exists the specified file.
@param[in] ulFileId The ID of the file to flush
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_Fs_Fsync(ULONG ulFileId);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sync the sepecified device.
@param[in] szDrvName The name of the drive to flush
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE FJ_Fs_Sync(CHAR* szDrvName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function saves the raw data to file.
@param[in] szName The name of the file
@param[in] addr The address of data to save to the file
@param[in] size The size of data to save
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The maximum value of file/directory name is 518 bytes excluding Drive Name and colon(:).<br>
Moreover, please specify it with short filename.<br>
*/
FJ_ERR_CODE	FJ_Fs_FileSave(const char* szName, UINT32* addr, INT32 size);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function returns the result of the check exist 'DCIM' and 'MISC' folders.
@param[in] bMediaId Media ID.<br>
					= @ref FJ_MEDIA_SD0 : SD card 0<br>
					= @ref FJ_MEDIA_SD1 : SD card 1<br>
@param[out] pDcimExist Check result of the 'DCIM' folder is exist or not<br>
						= @ref FJ_STATUS_DIR_NO_EXIST	: 'DCIM' folder is not exist of the root entry.<br>
						= @ref FJ_STATUS_DIR_EXIST		: 'DCIM' folder is exist of the root entry.<br>
@param[out] pMiscExist Check result of the 'MISC' folder is exist or not<br>
						= @ref FJ_STATUS_DIR_NO_EXIST	: 'MISC' folder is not exist of the root entry.<br>
						= @ref FJ_STATUS_DIR_EXIST		: 'MISC' folder is exist of the root entry.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.
// --- REMOVE_PRFILE2 END ---
*/
FJ_ERR_CODE	FJ_Fs_ExistRootDirectoryCheck(FJ_MEDIA_ID bMediaId, UCHAR* pDcimExist, UCHAR* pMiscExist);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function copy the files to NF data drive.
@param[in] bMediaId Media ID.<br>
					= @ref FJ_MEDIA_SD0 : SD card 0<br>
					= @ref FJ_MEDIA_SD1 : SD card 1<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks 
The following preparations are necessary before carrying out API.<br>
1.Make directories in SD card. <br>
	The name of those directories makes the name same as a drive of the NAND. <br>
	It stores the data necessary for NF1 drive in SD:\\NF1.<br>
2.Set the SD card to a board.<br>
3.Execute this API.<br>
<br>
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.
// --- REMOVE_PRFILE2 END ---
*/
FJ_ERR_CODE	FJ_Fs_FileCopyToDrive(FJ_MEDIA_ID bMediaId);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the entry name of when searching for files or directory.<br>
The entry name is a wild card can be specified.
@param[in] szEntryName The search entry name.<br>
						It can be used the following wild card.<br>
						"*" : The asterisk (*) can be all single characters or any string. <br>
						"?" : The question mark (?) can be any single character. <br>
						e.g. ) G002*.JPG, SD:\\DCIM\\200SOCIO\\G0020*.JPG
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The entry name specified by this API can be get by FJ_Fs_DirectoryEntryGet().
*/
FJ_ERR_CODE	FJ_Fs_SetSearchEntry(CHAR* szEntryName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function returns entries(file or directory) information of the current directory one by one.
@param[in] bEntryMode Specifies whether the first entry should be retrieved or a subsequent entry or reset the function.<br>
					= @ref FJ_FS_ENTRY_MODE_START : Start get Entry function.<br>
					= @ref FJ_FS_ENTRY_MODE_NEXT : Get next Entry.<br>
					= @ref FJ_FS_ENTRY_MODE_NEXT_NOT_SUB : Get next Entry (Not read Sub Dir).<br>
					= @ref FJ_FS_ENTRY_MODE_END : Reset and End get Entry function.<br>
@param[out] szOutput The output buffer for returning the entry name
@param[in] uiOutputLen The length of the output string buffer
@param[out] puiAttributes The attributes of entry
@param[out] pulSize The size of entry
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_LAST_ENTRY Trying to get entry after reached to the last entry.
@retval FJ_ERR_NG Error occurred.
@remarks
If you want specify the entry name, please call the FJ_Fs_SetSearchEntry() before this API.<br>
If you want to change the set entry name, please call this API by specifying the @ref FJ_FS_ENTRY_MODE_END in entry mode. <br>
After that, call the FJ_Fs_SetSearchEntry() to set the entry name.
*/
FJ_ERR_CODE	FJ_Fs_DirectoryEntryGet(FJ_FS_ENTRY_MODE bEntryMode, CHAR* szOutput, UINT32 uiOutputLen, UINT32* puiAttributes, ULONG* pulSize);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get the root entries count.
@param[out] count Count of root entries
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE	FJ_Fs_RootEntriesCount(ULONG *count);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function mount a storage media.
@param[in] bMediaId Media ID. See @ref FJ_MEDIA_ID
@param[in] szDrvName The name of the drive.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NOT_FAT The target drive is formatted excluding the FAT filesystem.
@remarks
If returned "FJ_ERR_NOT_FAT", please full format the target drive by FJ_FullFormatStorageMedia() API.
*/
FJ_ERR_CODE	FJ_Mount(FJ_MEDIA_ID bMediaId, CHAR *szDrvName);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function unmount a storage media.
@param[in] szDrvName The name of the drive.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE	FJ_UnMount(CHAR *szDrvName);

// get Media geometry function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function reads the current storage media information.<br>
@param[in] bMediaId Media ID. See @ref FJ_MEDIA_ID
@param[out] pGeometry storage media information.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NOT_MOUNT Media is not mount.
@remarks
@ref FJ_Mount is called before this API.
*/
FJ_ERR_CODE	FJ_GetStorageMediaGeometryEx(FJ_MEDIA_ID bMediaId, FJ_FS_GEOMETRY* pGeometry);

// Media format (quick-format) function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function enables the HOST to format a storage media (QuickFormat).
@param[in] bMediaId Media ID. See @ref FJ_MEDIA_ID
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE	FJ_FormatStorageMedia(FJ_MEDIA_ID bMediaId);

// Media format (full-format) function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function enables the HOST to format a storage media (FullFormat).
@param[in] bMediaId Media ID. See @ref FJ_MEDIA_ID
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@post @ref FJ_GetFormatProgress is called after this API.
*/
FJ_ERR_CODE	FJ_FullFormatStorageMedia(FJ_MEDIA_ID bMediaId);

// get Media format (quick/full) progress function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get format progress.
@param[out] progress A pointer to now progress.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@pre @ref FJ_FullFormatStorageMedia is called before this API.
@remarks
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.
// --- REMOVE_PRFILE2 END ---
*/
FJ_ERR_CODE	FJ_GetFormatProgress(UINT32* progress);

// set Volume label function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the volume label character string used when media are formatted.
@param[in] szDrvName The name of the drive.
@param[in] szVolLabel volume label character string(Max 11 characters).
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
The volume label character string are able to set up to 11 characters.<br>
(If you set over 12 characters to string, this function will return error code(FJ_ERR_NG).)<br>
<br>
If you wants to delete volume label settings, please call this function by NULL character string (FJ_SetVolLabel("A", "\0")).<br>
*/
FJ_ERR_CODE	FJ_SetVolLabel(CHAR *szDrvName, CHAR* szVolLabel);

// get Volume label function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets the volume label character string used.
@param[in] szDrvName The name of the drive.
@param[out] szVolLabel volume label character string.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
*/
FJ_ERR_CODE	FJ_GetVolLabel(CHAR *szDrvName, CHAR* szVolLabel);

// internal memory drive to drive copy function
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function copies all data in the NF drive to other drive.
@param[in] szSrcDrive copy source drive name.<br>
@param[in] szDstDrive copy destination drive name.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
This function can use only NF drive.<br>
Please set character string of drive name to szSrcDrive and szDstDrive.<br>
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.
// --- REMOVE_PRFILE2 END ---
*/
FJ_ERR_CODE	FJ_DriveToDriveCopy(CHAR* szSrcDrive, CHAR* szDstDrive);

// set callback function for writing error
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets callback to notice writing error for non shared memory.
@param[in] ulFileId The ID of the file to set for writing request.<br>
@param[in] pCallback The callback function is used when an writing error is occured for non shared memory.<br>
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks 
This function can use for Linux Version project.<br>
// --- REMOVE_PRFILE2 BEGIN ---
This function is unsupported when the file system is PrFILE2.<br>
// --- REMOVE_PRFILE2 END ---
*/
FJ_ERR_CODE FJ_Set_VFS_Write_Err_CB( ULONG ulFileId, FsWriteErrCB pCallback );

// read user setting data function.
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets a user setting data from user setting area on SDRAM.
@param[in] pbyBuffer The buffer address for read
@param[in] ulRequestSize The requested number of bytes to read
@param[in] ulOffset The offset into user setting area.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks 
This function loads a user setting data from main area to work area.<br>
Main area is to load from NF.<br>
Work area is to update setting value.<br>
*/
FJ_ERR_CODE FJ_Fs_GetUserSettingData( UINT32 *pbyBuffer, ULONG ulRequestSize, ULONG ulOffset );


// set user setting data to nand flash.
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets a user setting data to user setting area on nand flash.
@param[in] pbyBuffer The buffer address for a user setting data to write.
@param[in] ulRequestSize The size for a user setting data to write.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@remarks
This function updates a user setting data from work area to NF.<br>
Main area is to load from NF.<br>
Work area is to update setting value.<br>
If updating to NF is success, main area is updated too.<br>
*/
FJ_ERR_CODE FJ_Fs_SetUserSettingData( UINT32 *pbyBuffer, ULONG ulRequestSize );

/*@} --- end of fj_filesystem_api */

FJ_ERR_CODE FJ_Set_NF_Storage_ID(UCHAR id);
FJ_ERR_CODE FJ_Set_EMMC_Storage_ID(UCHAR id);
UCHAR FJ_Get_NF_Storage_ID(VOID);
UCHAR FJ_Get_EMMC_Storage_ID(VOID);

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_filesystem_message
@{*/
/*@} --- end of fj_filesystem_message */

#endif	// _FJ_FILESYSTEM_H
