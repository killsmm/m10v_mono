/**
 * @file		emmc_custom.h
 * @brief		Definition of customization for user
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _EMMC_CUSTOM_H
#define _EMMC_CUSTOM_H

/*----------------------------------------------------------------------*/
/* Format related														*/
/*----------------------------------------------------------------------*/
// FAT type
#define D_EM_FAT_TYPE_NOT_USE			0x00					// unused
#define D_EM_FAT_TYPE_FAT12				0x01					// FAT12
#define D_EM_FAT_TYPE_FAT16				0x04					// FAT16
#define D_EM_FAT_TYPE_FAT32				0x0B					// FAT32

// Cluster size
#define D_EM_CLUSTER_NOT_USE			0						// unused
#define D_EM_CLUSTER_SIZE_1				512						// 1Sector/Cluster
#define D_EM_CLUSTER_SIZE_2				1024					// 2Sector/Cluster
#define D_EM_CLUSTER_SIZE_4				2048					// 4Sector/Cluster
#define D_EM_CLUSTER_SIZE_8				4096					// 8Sector/Cluster
#define D_EM_CLUSTER_SIZE_16			8192					// 16Sector/Cluster
#define D_EM_CLUSTER_SIZE_32			16384					// 32Sector/Cluster
#define D_EM_CLUSTER_SIZE_64			32768					// 64Sector/Cluster
#define D_EM_CLUSTER_SIZE_128			65536					// 128Sector/Cluster

/*----------------------------------------------------------------------*/
/* Storage area(sector number)											*/
/*----------------------------------------------------------------------*/
#define D_EM_STORAGE_NUM				4						// number of the storages

#define D_EM_STORAGE_MBR_SIZE	 		0x00000100				// MBR
#define D_EM_STORAGE_0_SIZE		 		0x00100000				// STORAGE0 Size =512MB (512   *1024*1024 / 512 sector)
#define D_EM_STORAGE_1_SIZE		 		0x00100000				// STORAGE1 Size =512MB (512   *1024*1024 / 512 sector)
#define D_EM_STORAGE_2_SIZE		 		0x00200000				// STORAGE2 Size =  1GB (1*1024*1024*1024 / 512 sector)
#define D_EM_STORAGE_3_SIZE		 		0x00200000				// STORAGE3 Size =  1GB (1*1024*1024*1024 / 512 sector)
#define D_EM_STORAGE_4_SIZE		 		0x00000000				// STORAGE4 Size =  0KB not use
#define D_EM_STORAGE_5_SIZE		 		0x00000000				// STORAGE5 Size =  0KB not use
#define D_EM_STORAGE_6_SIZE		 		0x00000000				// STORAGE6 Size =  0KB not use
#define D_EM_PARAMETER_SIZE		 		0x00100000				// Parameter block Size = 512MB (512*1024*1024 / 512 sector)

#define D_EM_STORAGE_0_FAT		 		D_EM_FAT_TYPE_FAT16		// STORAGE0 FAT type = FAT16
#define D_EM_STORAGE_1_FAT		 		D_EM_FAT_TYPE_FAT16		// STORAGE1 FAT type = FAT16
#define D_EM_STORAGE_2_FAT		 		D_EM_FAT_TYPE_FAT16		// STORAGE2 FAT type = FAT16
#define D_EM_STORAGE_3_FAT		 		D_EM_FAT_TYPE_FAT16		// STORAGE3 FAT type = FAT16
#define D_EM_STORAGE_4_FAT		 		D_EM_FAT_TYPE_FAT16		// STORAGE4 FAT type = FAT16
#define D_EM_STORAGE_5_FAT		 		D_EM_FAT_TYPE_NOT_USE	// STORAGE5 not use
#define D_EM_STORAGE_6_FAT		 		D_EM_FAT_TYPE_NOT_USE	// STORAGE6 not use

#define D_EM_STORAGE_0_CLUSTER	 		D_EM_CLUSTER_SIZE_32	// STORAGE0 Cluster size = 16kbyte(32Sector)
#define D_EM_STORAGE_1_CLUSTER	 		D_EM_CLUSTER_SIZE_32	// STORAGE1 Cluster size = 16kbyte(32Sector)
#define D_EM_STORAGE_2_CLUSTER	 		D_EM_CLUSTER_SIZE_32	// STORAGE2 Cluster size = 16kbyte(32Sector)
#define D_EM_STORAGE_3_CLUSTER	 		D_EM_CLUSTER_SIZE_32	// STORAGE3 Cluster size = 16kbyte(32Sector)
#define D_EM_STORAGE_4_CLUSTER	 		D_EM_CLUSTER_SIZE_32	// STORAGE4 Cluster size = 16kbyte(32Sector)
#define D_EM_STORAGE_5_CLUSTER	 		D_EM_CLUSTER_NOT_USE	// STORAGE5 not use
#define D_EM_STORAGE_6_CLUSTER	 		D_EM_CLUSTER_NOT_USE	// STORAGE6 not use

#define D_EM_STORAGE_0_ENTRY	 		512						// STORAGE0 Entry number = 512
#define D_EM_STORAGE_1_ENTRY	 		512						// STORAGE1 Entry number = 512
#define D_EM_STORAGE_2_ENTRY	 		512						// STORAGE2 Entry number = 512
#define D_EM_STORAGE_3_ENTRY	 		512						// STORAGE3 Entry number = 512
#define D_EM_STORAGE_4_ENTRY	 		512						// STORAGE4 not use
#define D_EM_STORAGE_5_ENTRY	 		0						// STORAGE5 not use
#define D_EM_STORAGE_6_ENTRY	 		0						// STORAGE6 not use

#endif	// _EMMC_CUSTOM_H
