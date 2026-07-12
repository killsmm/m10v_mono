/*
 * (C) Copyright 2013 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include "asm/utils.h"
#include "mmc.h"
#include "fat.h"
#include "block.h"
#include "io.h"
#include "string.h"
#include "crypto/crypto.h"
#include "device.h"

#if defined(CONFIG_FILEPATH_DTB_CONFIG)
char dtb_config[4096]  __attribute__ (( section (".sdram_data") ));
#endif
#ifdef CONFIG_RSA_SIGNATURE
char sig_area[512]  __attribute__ (( section (".sdram_data") ));
#endif

void *load_ads_kernel __attribute__ (( section (".sdram_data") ));
void *load_ads_dtb __attribute__ (( section (".sdram_data") ));
#ifdef CONFIG_FILEPATH_INITRAMFS
void *load_ads_initramfs __attribute__ (( section (".sdram_data") ));
#endif
#define INITRAM_SIZE_OFFSET0	(0x68)
#define INITRAM_SIZE_OFFSET1	(0x69)
#define INITRAM_SIZE_OFFSET2	(0x6A)
#define INITRAM_SIZE_OFFSET3	(0x6B)
#define INITRAM_DATA_OFFSET		(0x80)
extern int unpack_rle256_bmp(void *in, u32 *fb32, int fb_width, int fb_height);

#define MAX_DTB_SIZE (50 * 1024)
#define MAX_INITRAMFS_SIZE (128 * 1024 * 1024)
#define MAX_BOOTLOADER_SIZE (1024 * 1024)
int _filesystem_read(enum filesystem_types rootfstype, const char *name, void *la, u32 len)
{
#ifdef ASSETS_XIP
	void *v;
	int len1;

	if (rootfstype != FS_ROMFS)
#endif
		goto copy_it;
#ifdef ASSETS_XIP
	v = romfs_filesystem_get_ads(name, &len1);
	if (!v)
		return -1;

	if (la == load_ads_kernel)
		load_ads_kernel = v;
	if (la == load_ads_dtb)
	//	load_ads_dtb = v;
		goto copy_it;
#ifdef CONFIG_FILEPATH_INITRAMFS
	if (la == load_ads_initramfs)
		load_ads_initramfs = v;
#endif

	return len1;
#endif

copy_it:
	return filesystem_read(name, la, len);
}

int load_from_filesystem(enum boot_source device, int part, enum filesystem_types rootfstype)
{
	const char *kernel_path = CONFIG_FILEPATH_KERNEL;
	const char *dtb_path;
	const char *bootloader_path = CONFIG_FILEPATH_BOOTLOADER;
	int run_bootloader = 0;
#ifdef CONFIG_FILEPATH_INITRAMFS
	int initramfslen = 0;
	u8 *p;
#endif
#if defined(CONFIG_FILEPATH_KERNEL_SIG) && defined(CONFIG_RSA_SIGNATURE)
	const char *sig_path = CONFIG_FILEPATH_KERNEL_SIG;
#endif
	int klen = 0, dtblen = 0, n;
#ifdef CONFIG_RSA_SIGNATURE
	hash_type ctx;
	unsigned char hash[MAX_BINARY_DIGEST_BYTES + 1];
#endif
#if defined(CONFIG_FILEPATH_DTB_CONFIG)
	int dtb_config_len;
#endif

	load_ads_kernel =	(void *)(CONFIG_SYS_SDRAM_BASE + OFFSET_KERNEL);
	load_ads_dtb =		(void *)(CONFIG_SYS_SDRAM_BASE + OFFSET_DTB);
	load_ads_initramfs =(void *)(CONFIG_SYS_SDRAM_BASE + CONFIG_INITRAMFS_OFFSET);
	unsigned char*  initramfs_size_add= (unsigned char*)(load_ads_initramfs+INITRAM_SIZE_OFFSET0);

	dtb_path = CONFIG_FILEPATH_DTB;

#ifdef CONFIG_SPLASH
	/* if the optional splash exists,, load it reusing kernel mem area */

	if (_filesystem_read(rootfstype, CONFIG_FILEPATH_SPLASH, load_ads_kernel, 1000000) > 0) {
		puts("  read splash\n");
		/* unpack it into framebuffer, kernel mem area unused again */
		if (!unpack_rle256_bmp(load_ads_kernel, (void *)CONFIG_SPLASH_FB_PA,
			       CONFIG_SPLASH_FB_WIDTH, CONFIG_SPLASH_FB_HEIGHT))
			/* if it unpacked correctly, start video */
			board_start_video();
	}
#endif
	klen = _filesystem_read(rootfstype, bootloader_path, load_ads_kernel, MAX_BOOTLOADER_SIZE);
	if (klen > 0)
		run_bootloader = 1;
	else {
		klen = _filesystem_read(rootfstype, kernel_path, load_ads_kernel, OFFSET_DTB - OFFSET_KERNEL);
		if (klen < 1) {
			puts("  failed kernel\n");
			return 1;
		}

		dtblen = _filesystem_read(rootfstype, dtb_path, load_ads_dtb, MAX_DTB_SIZE);
		if (dtblen < 1) {
			puts("  failed dtb\n");
			return 1;
		}

#ifdef CONFIG_FILEPATH_INITRAMFS
	#ifndef COMBINE_KERNEL_INITRAMFS_2_1_FILE
		/* initramfs is optional */
		initramfslen = _filesystem_read(rootfstype, 
			CONFIG_FILEPATH_INITRAMFS, load_ads_initramfs,
							MAX_INITRAMFS_SIZE);
		if (initramfslen < 1) {
			initramfslen = 0;
			/* stop an attack by preparing an initramfs in DDR */
			memset(load_ads_initramfs, 0, 32 * 1024);
		} else
			puts("  Loaded initramfs\n");
	#else

		initramfslen = (*initramfs_size_add)<<24; 
		initramfs_size_add++;
		initramfslen |=((*initramfs_size_add)<<16);
		initramfs_size_add++;
		initramfslen |=((*initramfs_size_add)<<8);
		initramfs_size_add++;
		initramfslen |=(*initramfs_size_add);
		load_ads_initramfs = load_ads_initramfs+INITRAM_DATA_OFFSET;
//		memcpy(load_ads_initramfs,(load_ads_initramfs+INITRAM_DATA_OFFSET),initramfslen);
	#endif
#endif
#if defined(CONFIG_FILEPATH_DTB_CONFIG)
		dtb_config_len = _filesystem_read(rootfstype, CONFIG_FILEPATH_DTB_CONFIG,
					    dtb_config, sizeof(dtb_config) - 1);
#endif
	}

#ifdef CONFIG_RSA_SIGNATURE
	if (run_bootloader)
		sig_path = CONFIG_FILEPATH_BOOTLOADER_SIG;

	/* get the kernel binary signature */

	n = _filesystem_read(rootfstype, sig_path, sig_area, 512);
	if (n < 1 || n > 512)
		goto fail_sig;

	puts("  confirming auth\n");
	hash_init(&ctx);

	hash_loop(&ctx, load_ads_kernel, klen);

	if (!run_bootloader) {
		hash_loop(&ctx, load_ads_dtb, dtblen);
#ifdef CONFIG_FILEPATH_INITRAMFS
		   if (initramfslen)
			  hash_loop(&ctx, load_ads_initramfs, initramfslen);
#endif
#if defined(CONFIG_FILEPATH_DTB_CONFIG)
		if (dtb_config_len > 0)
			hash_loop(&ctx, dtb_config, dtb_config_len);
#endif
	}

	hash_result(&ctx, hash);

	/* check it against the signature we were given */

	if (verify_signature(hash, sig_area)) {

fail_sig:

	/* signature did not allow it */

#ifdef CONFIG_RSA_SIGNATURE_ENFORCING

		/* do not accept to leave any attacker-controlled data */

		memset(sig_area, 0, 512);
		if (klen >= 0)
			memset(load_ads_kernel, 0, klen);
		if (dtblen >= 0)
			memset(load_ads_dtb, 0, dtblen);
#ifdef CONFIG_FILEPATH_INITRAMFS
		if (initramfslen)
			memset(load_ads_initramfs, 0, initramfslen);
#endif
#if defined(CONFIG_FILEPATH_DTB_CONFIG)
		if (dtb_config_len > 0)
			memset(dtb_config, 0, dtb_config_len);
#endif
		return 1;
#else
		puts("   (not enforcing auth check)\n");
#endif
	}

	/* no problems */
#endif

	if (run_bootloader) {
		puts("  bootloader loaded\n");
		return 0;
	}

	puts("  kernel + dtb loaded\n");

	/* we don't dare walk the fdt until sig check OK if enforcing */

#ifdef CONFIG_FILEPATH_INITRAMFS
	if (!initramfslen)
#endif
		update_dtb(load_ads_dtb, device, part, rootfstype);
		
#if defined(CONFIG_FILEPATH_DTB_CONFIG)
	process_dtb_config(load_ads_dtb, dtb_config, dtb_config_len);
#endif

#ifdef CONFIG_FILEPATH_INITRAMFS

	p = fdt_find_path(load_ads_dtb, "/chosen/linux,initrd-start", &n);
	if (!p || n != 4) {
		puts("no initrd-start in dtb\n");
		return 1;
	} else
		uaw_be32(p, (u32)load_ads_initramfs);

	/* update the dtb with actual length, or 0 length if none */

	p = fdt_find_path(load_ads_dtb, "/chosen/linux,initrd-end", &n);
	if (!p || n != 4) {
		puts("no initrd-end in dtb\n");
		return 1;
	} else
		uaw_be32(p, (u32)load_ads_initramfs + initramfslen);
#endif

#ifdef CONFIG_ARCH_DTB_PREP
	CONFIG_ARCH_DTB_PREP
#endif

	fdt_set_memory_from_mhu(load_ads_dtb);

bail:

	return 0;
}
