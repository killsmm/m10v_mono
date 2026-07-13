/*************************************************************************
 *
 *    Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. File name
 *        FS_errno.h
 * 2. Outline of source file
 *        Error code
 *
 ************************************************************************/

#ifndef _FSERRNO_H
#define _FSERRNO_H


#define FS_ENT_LAST             1               /* Even the last file or the directory was read(Only FS_readdir) */

#define FS_ERR_NOENT            (-2)            /* The file or the directory doesn't exist */
#define FS_ERR_SRCH             (-3)            /* The data of the object task doesn't exist */
#define FS_ERR_IO               (-5)            /* The IO error occurred */
#define FS_ERR_NXIO             (-6)            /* There is no drive(cutting condition) */
#define FS_ERR_BADF             (-9)            /* The file number is illegal */
#define FS_ERR_NOMEM            (-12)           /* It failed in the acquisition of the memory */
#define FS_ERR_BUSY             (-16)           /* The drive is busy */
#define FS_ERR_EXIST            (-17)           /* The file or the directory already exists */
#define FS_ERR_NODEV            (-19)           /* Specification is not driven */
#define FS_ERR_NOTDIR           (-20)           /* The directory doesn't exist. It is not a directory */
#define FS_ERR_ISDIR            (-21)           /* It is not possible to process it to the directory */
#define FS_ERR_INVAL            (-22)           /* The value of the argument is illegal */
#define FS_ERR_MOPEN            (-24)           /* The maximum, open number (number of maximum continuance processing) was exceeded */
#define FS_ERR_FBIG             (-27)           /* The size of the file that can be treated is exceeded */
#define FS_ERR_NOSPC            (-28)           /* An empty area of the filesystem is insufficient */
#define FS_ERR_SPIPE            (-29)           /* It is illegal seek */
#define FS_ERR_ROFS             (-30)           /* It is not possible to write it in the drive only for reading or the file */
#define FS_ERR_NOSYS            (-38)           /* It is a function not mounted */
#define FS_ERR_NOTEMPTY         (-39)           /* The directory has not emptied */

#define FS_ERR_NAMETOOLONG      (-78)           /* The file name or the directory name exceeds prescribed length */



#define FS_ERR_DELENT           (FS_ERR_NOENT)  /* The file or the directory is deleted */
#define FS_ERR_EOF              (-501)          /* When you reach the terminal of the file by reading */
#define FS_ERR_EXCLUSIVE        (-502)          /* The file or the directory has already been used */
#define FS_ERR_RDONLY           (FS_ERR_ROFS)   /* Open mode only for reading */
#define FS_ERR_WRONLY           (-511)          /* Open mode only for writing */
#define FS_ERR_NAMEERR          (-520)          /* The character that cannot be used for the file or the directory name is included */

#define FS_ERR_MDF              (-600)          /* MDF has already been begun */
#define FS_ERR_NOMDF            (-601)          /* MDF is not begun */

#define FS_ERR_MDRV             (-610)          /* The number of drive registration that was able to be used was exceeded */
#define FS_ERR_DEVENT           (-611)          /* The same drive letter is registered */
#define FS_ERR_NODRV            (FS_ERR_NODEV)  /* The drive has not been initialized */
#define FS_ERR_DRVUSE           (FS_ERR_BUSY)   /* The drive is being used */
#define FS_ERR_SYNC             (FS_ERR_BUSY)   /* The drive is being tuned */
#define FS_ERR_FORMAT           (FS_ERR_BUSY)   /* It is formatting it */
#define FS_ERR_NOCONDRV         (FS_ERR_NXIO)   /* The drive is cutting condition */

#define FS_ERR_MTASK            (-620)          /* An accessible number of tasks was exceeded MDF */
#define FS_ERR_NOTASK           (FS_ERR_SRCH)   /* The data of the object task doesn't exist */

#define FS_ERR_NOSEM            (-630)          /* There is no empty semaphore */
#define FS_ERR_CRESEM           (-631)          /* The semaphore was not generable. */
#define FS_ERR_DELSEM           (-632)          /* The semaphore was not able to be deleted */
#define FS_ERR_ENQSEM           (-633)          /* The semaphore was not able to be excluded */
#define FS_ERR_ENQSEMTMOUT      (-634)          /* The time-out was done by excluding the semaphore*/
#define FS_ERR_DEQSEM           (-635)          /* Exclusive release of the semaphore cannot have been done */


#define FS_ERR_UNCORRESPONDING  (-801)          /* It is a format that doesn't correspond */
#define FS_ERR_NOROOT           (-810)          /* There is no empty directory entry in the root directory */
#define FS_ERR_INVOKE           (-820)          /* It is processing that cannot be executed and a function */


/* An error code that is smaller than '-900' is an error that cannot continuously process the filesystem.
 * Please discontinue the use of the medium, and the medium check or format it.
 * There is a possibility that data may be broken when continuously processing it.
 */
#define FS_ERR_FAT_READ         (-901)          /* It failed in reading the FAT area */
#define FS_ERR_FAT_WRITE        (-902)          /* It failed in writing in the FAT area */
#define FS_ERR_ROOT_READ        (-903)          /* It failed in reading the route area of FAT12 and 16 */
#define FS_ERR_ROOT_WRITE       (-904)          /* It failed in writing the route area of FAT12 and 16 and initialization */
#define FS_ERR_BADREAD          (-905)          /* Because the lead of a defective area (cluster) had failed, alternative processing was not able to be done */
#define FS_ERR_BADWRITE         (-906)          /* The light in a defective area (cluster) was done and because the Ritorai frequency had failed, alternative
                                                   processing was not able to be done */
#define FS_ERR_FSNOADJ          (-910)          /* There are no adjustments in the use filesystem and the medium */
#define FS_ERR_DIRERR           (-911)          /* In the composition of the directory entry, abnormality is found */
#define FS_ERR_CHAIN            (-912)          /* In the composition of the cluster chain, abnormality is found */

#define FS_ERR_FSID             (-999)          /* The error occurred by the filesystem */

/* Please refer to the error code of the driver function for an error code that is smaller than '-1000' */
#define FS_DRV_ERR_INVALID_DATA (-1000)         /* Data became invalid by card disconnection */


#endif /* _FSERRNO_H */
