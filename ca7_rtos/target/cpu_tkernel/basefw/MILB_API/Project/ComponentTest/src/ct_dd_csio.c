/**
 * @file		ct_dd_csio.c
 * @brief		This is ct code for dd_csio
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "stdlib.h"
#include "string.h"
#include "dd_csio.h"
#include "dd_uart.h"
#include "dd_top.h"
#include "dd_hdmac1.h"
#include "dd_gic.h"
#include "dd_tmr32.h"
#include "uart_csio.h"
#include "ct_dd_csio.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CSIO_HDMAC_DATA_ADDR_SEND	(0x7A000000)				// DMA TRANSFER BUFFER START ADDRESS
#define D_CSIO_HDMAC_DATA_ADDR_RECV	(0x7A400000)				// DMA RECEIVE BUFFER START ADDRESS
#define D_CSIO_TIMER_CH			(7)

#ifdef CO_DEBUG_ON_PC
#define D_CSIO_TEST_CMD_LEN		20
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#if 0
static UCHAR gBuf8[255]		= {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};
#endif

#if 1
static UCHAR gBuf8_2[32768] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));
static UCHAR gRcv_buf8_2[32768] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));

static USHORT gBuf16_2[16384] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));
static USHORT gRcv_buf16_2[16384] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));

#else
static UCHAR gBuf8_2[512]			= {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
									   0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
									   0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
									   0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
									   0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,
									   0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
									   0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80,
									   0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90,
									   0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0,
									   0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0,
									   0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
									   0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0,
									   0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0,
									   0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0,
									   0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0x12,
									   0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
									   0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32,
									   0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42,
									   0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52,
									   0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62,
									   0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72,
									   0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82,
									   0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
									   0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2,
									   0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2,
									   0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2,
									   0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2,
									   0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2,
									   0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2,
									   0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0x13, 0x14, 0x15,
									   0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
									   0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35};

static USHORT gBuf16_2[512]			= {0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 0x010F,
									   0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011A, 0x011B, 0x011C, 0x011D, 0x011E, 0x011F,
									   0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0125, 0x0126, 0x0127, 0x0128, 0x0129, 0x012A, 0x012B, 0x012C, 0x012D, 0x012E, 0x012F,
									   0x0130, 0x0131, 0x0132, 0x0133, 0x0134, 0x0135, 0x0136, 0x0137, 0x0138, 0x0139, 0x013A, 0x013B, 0x013C, 0x013D, 0x013E, 0x013F,
									   0x0140, 0x0141, 0x0142, 0x0143, 0x0144, 0x0145, 0x0146, 0x0147, 0x0148, 0x0149, 0x014A, 0x014B, 0x014C, 0x014D, 0x014E, 0x014F,
									   0x0150, 0x0151, 0x0152, 0x0153, 0x0154, 0x0155, 0x0156, 0x0157, 0x0158, 0x0159, 0x015A, 0x015B, 0x015C, 0x015D, 0x015E, 0x015F,
									   0x0160, 0x0161, 0x0162, 0x0163, 0x0164, 0x0165, 0x0166, 0x0167, 0x0168, 0x0169, 0x016A, 0x016B, 0x016C, 0x016D, 0x016E, 0x016F,
									   0x0170, 0x0171, 0x0172, 0x0173, 0x0174, 0x0175, 0x0176, 0x0177, 0x0178, 0x0179, 0x017A, 0x017B, 0x017C, 0x017D, 0x017E, 0x017F,
									   0x0180, 0x0181, 0x0182, 0x0183, 0x0184, 0x0185, 0x0186, 0x0187, 0x0188, 0x0189, 0x018A, 0x018B, 0x018C, 0x018D, 0x018E, 0x018F,
									   0x0190, 0x0191, 0x0192, 0x0193, 0x0194, 0x0195, 0x0196, 0x0197, 0x0198, 0x0199, 0x019A, 0x019B, 0x019C, 0x019D, 0x019E, 0x019F,
									   0x01A0, 0x01A1, 0x01A2, 0x01A3, 0x01A4, 0x01A5, 0x01A6, 0x01A7, 0x01A8, 0x01A9, 0x01AA, 0x01AB, 0x01AC, 0x01AD, 0x01AE, 0x01AF,
									   0x01B0, 0x01B1, 0x01B2, 0x01B3, 0x01B4, 0x01B5, 0x01B6, 0x01B7, 0x01B8, 0x01B9, 0x01BA, 0x01BB, 0x01BC, 0x01BD, 0x01BE, 0x01BF,
									   0x01C0, 0x01C1, 0x01C2, 0x01C3, 0x01C4, 0x01C5, 0x01C6, 0x01C7, 0x01C8, 0x01C9, 0x01CA, 0x01CB, 0x01CC, 0x01CD, 0x01CE, 0x01CF,
									   0x01D0, 0x01D1, 0x01D2, 0x01D3, 0x01D4, 0x01D5, 0x01D6, 0x01D7, 0x01D8, 0x01D9, 0x01DA, 0x01DB, 0x01DC, 0x01DD, 0x01DE, 0x01DF,
									   0x01E0, 0x01E1, 0x01E2, 0x01E3, 0x01E4, 0x01E5, 0x01E6, 0x01E7, 0x01E8, 0x01E9, 0x01EA, 0x01EB, 0x01EC, 0x01ED, 0x01EE, 0x01EF,
									   0x01F0, 0x01F1, 0x01F2, 0x01F3, 0x01F4, 0x01F5, 0x01F6, 0x01F7, 0x01F8, 0x01F9, 0x01FA, 0x01FB, 0x01FC, 0x01FD, 0x01FE, 0x01FF,
									   0x01F0, 0x01F1, 0x01F2, 0x01F3, 0x01F4, 0x01F5, 0x01F6, 0x01F7, 0x01F8, 0x01F9, 0x01FA, 0x01FB, 0x01FC, 0x01FD, 0x01FE, 0x01FF,
									   0x01E0, 0x01E1, 0x01E2, 0x01E3, 0x01E4, 0x01E5, 0x01E6, 0x01E7, 0x01E8, 0x01E9, 0x01EA, 0x01EB, 0x01EC, 0x01ED, 0x01EE, 0x01EF,
									   0x01D0, 0x01D1, 0x01D2, 0x01D3, 0x01D4, 0x01D5, 0x01D6, 0x01D7, 0x01D8, 0x01D9, 0x01DA, 0x01DB, 0x01DC, 0x01DD, 0x01DE, 0x01DF,
									   0x01C0, 0x01C1, 0x01C2, 0x01C3, 0x01C4, 0x01C5, 0x01C6, 0x01C7, 0x01C8, 0x01C9, 0x01CA, 0x01CB, 0x01CC, 0x01CD, 0x01CE, 0x01CF,
									   0x01B0, 0x01B1, 0x01B2, 0x01B3, 0x01B4, 0x01B5, 0x01B6, 0x01B7, 0x01B8, 0x01B9, 0x01BA, 0x01BB, 0x01BC, 0x01BD, 0x01BE, 0x01BF,
									   0x01A0, 0x01A1, 0x01A2, 0x01A3, 0x01A4, 0x01A5, 0x01A6, 0x01A7, 0x01A8, 0x01A9, 0x01AA, 0x01AB, 0x01AC, 0x01AD, 0x01AE, 0x01AF,
									   0x0190, 0x0191, 0x0192, 0x0193, 0x0194, 0x0195, 0x0196, 0x0197, 0x0198, 0x0199, 0x019A, 0x019B, 0x019C, 0x019D, 0x019E, 0x019F,
									   0x0180, 0x0181, 0x0182, 0x0183, 0x0184, 0x0185, 0x0186, 0x0187, 0x0188, 0x0189, 0x018A, 0x018B, 0x018C, 0x018D, 0x018E, 0x018F,
									   0x0170, 0x0171, 0x0172, 0x0173, 0x0174, 0x0175, 0x0176, 0x0177, 0x0178, 0x0179, 0x017A, 0x017B, 0x017C, 0x017D, 0x017E, 0x017F,
									   0x0160, 0x0161, 0x0162, 0x0163, 0x0164, 0x0165, 0x0166, 0x0167, 0x0168, 0x0169, 0x016A, 0x016B, 0x016C, 0x016D, 0x016E, 0x016F,
									   0x0150, 0x0151, 0x0152, 0x0153, 0x0154, 0x0155, 0x0156, 0x0157, 0x0158, 0x0159, 0x015A, 0x015B, 0x015C, 0x015D, 0x015E, 0x015F,
									   0x0140, 0x0141, 0x0142, 0x0143, 0x0144, 0x0145, 0x0146, 0x0147, 0x0148, 0x0149, 0x014A, 0x014B, 0x014C, 0x014D, 0x014E, 0x014F,
									   0x0130, 0x0131, 0x0132, 0x0133, 0x0134, 0x0135, 0x0136, 0x0137, 0x0138, 0x0139, 0x013A, 0x013B, 0x013C, 0x013D, 0x013E, 0x013F,
									   0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0125, 0x0126, 0x0127, 0x0128, 0x0129, 0x012A, 0x012B, 0x012C, 0x012D, 0x012E, 0x012F,
									   0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011A, 0x011B, 0x011C, 0x011D, 0x011E, 0x011F,
									   0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 0x010F};

static UCHAR gRcv_buf8_2[512];
static USHORT gRcv_buf16_2[512];
#endif

static UCHAR gBuf8[8]		= {0x11, 0x22, 0x44, 0x08, 0x10, 0x20, 0x40, 0x80};
static UCHAR gBuf8_slave[8] = {0x33, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB};
static USHORT gBuf16[8]		= {0x0198, 0x0176, 0x0154, 0x0132, 0x0110, 0x0120, 0x0140, 0x0180};

static UCHAR gRcv_buf8[8] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));
static UCHAR gRcv_buf8_slave[8] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));
static USHORT gRcv_buf16[8] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));

static E_DD_CSIO_TYPE gType[D_DD_USIO_CH_NUM_MAX]= {E_DD_CSIO_TYPE_MASTER};
static UINT32 gSend_size;
static E_DD_CSIO_DATA_LENGTH gData_length = E_DD_CSIO_DATA_LENGTH_8;
static UCHAR gAsync;
static UCHAR gBuf2_Used;
static UCHAR gNon_Chache_Area_Used;
static UCHAR gSend_flg;
static UCHAR gSending_ch_number;

#ifdef CO_DEBUG_ON_PC
static char* gCmd_Argv[25];
#endif	// CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
static void ct_csio_set_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_CSIO_TEST_CMD_LEN ) ); 
		cmd_pos++;
	}
}

static void ct_csio_reg_dump(int ch)
{
	Ddim_Print(("---- CSIO Register dump ----\n"));
	Ddim_Print(("CSIO%d.SMR   = 0x%02x\n", ch, IO_USIO.CSIO[ch].SMR.byte));
	Ddim_Print(("CSIO%d.SCR   = 0x%02x\n", ch, IO_USIO.CSIO[ch].SCR.byte));
	Ddim_Print(("CSIO%d.ESCR  = 0x%02x\n", ch, IO_USIO.CSIO[ch].ESCR.byte));
	Ddim_Print(("CSIO%d.SSR   = 0x%02x\n", ch, IO_USIO.CSIO[ch].SSR.byte));
	Ddim_Print(("CSIO%d.DR    = 0x%04x\n", ch, IO_USIO.CSIO[ch].DR.hword));
	Ddim_Print(("CSIO%d.BGR   = 0x%04x\n", ch, IO_USIO.CSIO[ch].BGR.hword));
	Ddim_Print(("CSIO%d.FCR   = 0x%04x\n", ch, IO_USIO.CSIO[ch].FCR.hword));
	Ddim_Print(("CSIO%d.FBYTE = 0x%04x\n", ch, IO_USIO.CSIO[ch].FBYTE.hword));
	Ddim_Print(("----------------------------\n"));
	
	return;
}
#endif	// CO_DEBUG_ON_PC

static VOID ct_csio_callback( INT32 result )
{
	INT32 index;

	Ddim_Print(("CSIO Callback. result=%X\n", result));

	if (gSend_size <= 300) {
		if ( gBuf2_Used ) {
			for (index = 0; index < gSend_size; index++ ){
				Ddim_Print(("gRcv_buf8_2[%d]=0x%X    gRcv_buf16_2[%d]=0x%X\n", index, gRcv_buf8_2[index], index, gRcv_buf16_2[index]));
			}
		}
		else {
			for (index = 0; index < gSend_size; index++ ){
				Ddim_Print(("Rcv_buf8[%d]=0x%X    Rcv_buf16[%d]=0x%X\n", index, gRcv_buf8[index], index, gRcv_buf16[index]));
			}
		}
	}
}

static VOID ct_csio_send_callback( INT32 result )
{
	Ddim_Print(("CSIO Send Callback. result=%X\n", result));

}

static VOID ct_csio_receive_callback( INT32 result )
{
	INT32 index;

	Ddim_Print(("CSIO Receive Callback. result=%X\n", result));

	if (gSend_size <= 300) {
		if ( gBuf2_Used ) {
			for (index = 0; index < gSend_size; index++ ){
				Ddim_Print(("gRcv_buf8_2[%d]=0x%X    gRcv_buf16_2[%d]=0x%X\n", index, gRcv_buf8_2[index], index, gRcv_buf16_2[index]));
			}
		}
		else {
			for (index = 0; index < gSend_size; index++ ){
				Ddim_Print(("Rcv_buf8[%d]=0x%X    Rcv_buf16[%d]=0x%X\n", index, gRcv_buf8[index], index, gRcv_buf16[index]));
			}
		}
	}
}

static VOID ct_csio_callback_full_duplex( INT32 result )
{
	INT32 index;

	Ddim_Print(("CSIO Callback. (full_duplex) result=%X\n", result));

	for (index = 0; index < 8; index++ ){
		Ddim_Print(("Rcv_buf8[%d]=0x%X    Rcv_buf16[%d]=0x%X    Rcv_buf8_slave[%d]=0x%X\n", index, gRcv_buf8[index], index, gRcv_buf16[index], index, gRcv_buf8_slave[index]));
	}
}

static VOID ct_csio_callback_full_duplex_dma( INT32 result )
{
	Ddim_Print(("CSIO Callback. (full_duplex_dma) result=%X\n", result));
}

static VOID ct_csio_timer_handler( VOID )
{
	INT32 ret;

	if(gSend_flg == 1){
		Ddim_Print(("CSIO Send Timeout.\n"));
		ret = Dd_CSIO_Stop(gSending_ch_number);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Stop ERR. ret=0x%x\n", ret));
		}
		gSend_flg = 0;
	}
}

static INT32 ct_csio_process(UCHAR ch, UCHAR num)
{
	INT32				ret = D_DDIM_OK;
	T_DD_CSIO_CAL		cal_tbl;
	T_DD_CSIO_CTRL		csio_ctrl;
	T_DD_CSIO_FIFO_CTRL	fifo_ctrl;
	UCHAR*				send_p;
	UCHAR*				recv_p;
	INT32				tmout = D_DDIM_USER_SEM_WAIT_POL;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;
	gBuf2_Used = 0;
	gNon_Chache_Area_Used = 0;

	switch (num) {
		// Send
		case 1:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;

			gSend_size = 3;

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 2:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_L;	// Transmit mode              :Nomal II
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_LSB_FIRST;	// Bit direction              :LSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_5;		// Data length                :5 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_send_callback;
			gAsync = 1;
			
			gSend_size = 3;
			
			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 3:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_H;		// Transmit mode              :SPI I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_9;		// Data length                :9 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;
			
			gSend_size = 3;
			
			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf16, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 4:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_L;		// Transmit mode              :SPI II
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_LSB_FIRST;	// Bit direction              :LSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 1;							// FIFO selection             :Send=FIFO2,Receive=FIFO1
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_send_callback;
			gAsync = 1;
			
			gSend_size = 3;
			
			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 5:	// TEST1 + interrupt enable
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_send_callback;
			gAsync = 1;
			
			gSend_size = 3;

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;
			
		case 6:	// TEST1 + FIFO
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;
			
			gSend_size = 3;

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 7:	// TEST1 + FIFO + enable
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_send_callback;
			gAsync = 1;
			
			gSend_size = 3;

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 8:	// TEST1 + FIFO + DMA + 8bit
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 1;							// FIFO selection             :Send=FIFO2,Receive=FIFO1
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_send_callback;
			gAsync = 1;
			
			gSend_size = 3;

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 9:	// TEST3 + DMA
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_H;		// Transmit mode              :SPI I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_9;		// Data length                :9 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = 0;
			gAsync = 1;
			
			gSend_size = 3;
			
			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf16, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		// Receive
		case 10:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_receive_callback;
			gAsync = 1;
			
			gSend_size = 3;

			memset(gRcv_buf8, 0, sizeof(gRcv_buf8));
			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 11:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;
			
			gSend_size = 3;

			memset(gRcv_buf8, 0, sizeof(gRcv_buf8));
			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 12:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_L;	// Transmit mode              :Nomal II
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_LSB_FIRST;	// Bit direction              :LSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_5;		// Data length                :5 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_receive_callback;
			gAsync = 1;
			
			gSend_size = 3;
			
			memset(gRcv_buf8, 0, sizeof(gRcv_buf8));
			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 13:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_H;		// Transmit mode              :SPI I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_9;		// Data length                :9 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 1;							// FIFO receive data size     :1 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;
			
			gSend_size = 3;
			
			memset(gRcv_buf8, 0, sizeof(gRcv_buf8));
			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf16, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 14:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_L;		// Transmit mode              :SPI II
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_LSB_FIRST;	// Bit direction              :LSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 1;							// FIFO selection             :Send=FIFO2,Receive=FIFO1
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 1;							// FIFO receive data size     :1 byte

			csio_ctrl.pcallback = ct_csio_receive_callback;
			gAsync = 1;
			
			gSend_size = 3;
			
			memset(gRcv_buf8, 0, sizeof(gRcv_buf8));
			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 15: // // #4 < none callback >
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_L;		// Transmit mode              :SPI II
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_LSB_FIRST;	// Bit direction              :LSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 1;							// FIFO selection             :Send=FIFO2,Receive=FIFO1
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 0;							// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;
			
			gSend_size = 3;
			
			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 16: // #4 < fsel=0 FIFO1:send FIFO2:recieve >
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_L;		// Transmit mode              :SPI II
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_LSB_FIRST;	// Bit direction              :LSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 0;							// FIFO 2 enable              :Disable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// FIFO receive data size     :0 byte

			csio_ctrl.pcallback = ct_csio_send_callback;
			gAsync = 1;
			
			gSend_size = 3;
			
			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf8, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 20:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave
			if( gType[ch] == E_DD_CSIO_TYPE_MASTER ){
				send_p                      = gBuf8;
				recv_p                      = gRcv_buf8;
			}
			else{
				send_p                      = gBuf8_slave;
				recv_p                      = gRcv_buf8_slave;
			}
			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 1;							// FIFO receive data size     :1 byte

			csio_ctrl.pcallback = ct_csio_callback_full_duplex;
			gAsync = 1;

			gSend_size = 8;

			memset(gRcv_buf8, 0, sizeof(gRcv_buf8));
			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));
			memset(gRcv_buf8_slave, 0, sizeof(gRcv_buf8_slave));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, send_p, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, recv_p, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 21:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_NORMAL_SC_H;	// Transmit mode              :Nomal I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_9;		// Data length                :9 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 1;							// FIFO receive data size     :1 byte

			csio_ctrl.pcallback = ct_csio_callback_full_duplex;
			gAsync = 1;

			gSend_size = 3;

			memset(gRcv_buf16, 0, sizeof(gRcv_buf16));

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, gBuf16, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf16, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 23:
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_H;		// Transmit mode              :SPI I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_9;		// Data length                :9 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave

			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 1;							// FIFO receive data size     :1 byte

			csio_ctrl.pcallback = 0;
			gAsync = 0;
			
			gSend_size = 3;
			
			gNon_Chache_Area_Used = 1;

			memset((VOID *)D_CSIO_HDMAC_DATA_ADDR_RECV, 0, 6);

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, (VOID *)D_CSIO_HDMAC_DATA_ADDR_RECV, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 24:	// Full Duplex + DMA(8bit)
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_H;		// Transmit mode              :SPI I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_8;		// Data length                :8 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave
			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0x80;							// FIFO receive data size     :128 byte

			csio_ctrl.pcallback = ct_csio_callback_full_duplex_dma;
			gAsync = 1;

			gSend_size = 0x003FFFFF;

			memset((VOID *)D_CSIO_HDMAC_DATA_ADDR_RECV, 0, 0x00400000);

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, (VOID *)D_CSIO_HDMAC_DATA_ADDR_SEND, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, (VOID *)D_CSIO_HDMAC_DATA_ADDR_RECV, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		case 25:	// Full Duplex + DMA(9bit)
			cal_tbl.in_freq  = 100000;
			cal_tbl.cal_freq = 0;
			ret = Dd_CSIO_Calculate(&cal_tbl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			}

			csio_ctrl.mode                  = E_DD_CSIO_MODE_SPI_SC_H;		// Transmit mode              :SPI I
			csio_ctrl.baudrate              = cal_tbl.cal_freq;				// Baudrate
			csio_ctrl.sop                   = 0;							// Serial output              :Output
			csio_ctrl.bit_direction         = E_DD_CSIO_BIT_DIR_MSB_FIRST;	// Bit direction              :MSB First
			csio_ctrl.data_length           = E_DD_CSIO_DATA_LENGTH_9;		// Data length                :9 bit
			csio_ctrl.type                  = gType[ch];					// Type                       :Master/Slave
			csio_ctrl.fifo_ctrl->fsel       = 0;							// FIFO selection             :Send=FIFO1,Receive=FIFO2
			csio_ctrl.fifo_ctrl->flste      = 0;							// FIFO Data-lost check       :Disable
			csio_ctrl.fifo_ctrl->fset       = 0;							// FIFO pointer               :Not save
			csio_ctrl.fifo_ctrl->fe2        = 1;							// FIFO 2 enable              :Enable
			csio_ctrl.fifo_ctrl->fe1        = 1;							// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fbyte_recv = 0x80;							// FIFO receive data size     :128 byte

			csio_ctrl.pcallback = ct_csio_callback_full_duplex_dma;
			gAsync = 1;

			gSend_size = 0x00200000;
			gData_length = E_DD_CSIO_DATA_LENGTH_9;

			memset((VOID *)D_CSIO_HDMAC_DATA_ADDR_RECV, 0, 0x00400000);

			ret = Dd_CSIO_Open(ch, tmout);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Send_Data(ch, (VOID *)D_CSIO_HDMAC_DATA_ADDR_SEND, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
			}
			ret = Dd_CSIO_Set_Recv_Data(ch, (VOID *)D_CSIO_HDMAC_DATA_ADDR_RECV, gSend_size);
			if (ret !=D_DDIM_OK) {
				Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
			}
			break;

		default:
			break;
	}
	return ret;
}

static VOID ct_csio_set_port( VOID )
{
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_P86, 1 );
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_P87, 1 );
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PE5, 1 );
	Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PE3, 1 );
	Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_PE4, 0 );
	Dd_Top_Set_Gpio_Direction( E_DD_TOP_GPIO_PE2, 0 );

	Dd_GIC_Ctrl(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT, 1, D_DD_GIC_PRI14, 1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_UART_CH1_RECEPTION_INT, 1, D_DD_GIC_PRI14, 1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT, 1, D_DD_GIC_PRI14, 1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_UART_CH1_TRANSMISSION_INT, 1, D_DD_GIC_PRI14, 1);
}

/**
 * @brief  Command main function for CSIO test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| P1             | P2       |  P3         | P4          | P5        | P6     | P7  | P8     | Meaning                            |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| open           | ch       | tmout       |             |           |        |     |        | Dd_CSIO_Open()                     |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| close          | ch       |             |             |           |        |     |        | Dd_CSIO_Close()                    |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_type       | ch       | type        |             |           |        |     |        | Set Master or Slave                |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| cal            | cal_freq |             |             |           |        |     |        | Dd_CSIO_Calculate()                |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_test       | ch       | num         |             |           |        |     |        | Continuous execution               |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| ctrl           | Refer to the following tables.       |           |        |     |        | Dd_CSIO_Ctrl()                     |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_send       | ch       | data num    | sdram       |           |        |     |        | Dd_CSIO_Set_Send_Data()            | sdram:for send to SDRAM test
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_recv       | ch       | data num    | sdram       |           |        |     |        | Dd_CSIO_Set_Recv_Data()            | sdram:for recv to SDRAM test
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_send     | ch       |             |             |           |        |     |        | Dd_CSIO_Start_Send()               |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_send_dma | ch       | dma_ch      | dma_mode    |           |        |     |        | Dd_CSIO_Start_Send_DMA()           |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_recv_dma | ch       | dma_ch      | dma_mode    |           |        |     |        | Dd_CSIO_Start_Recv_DMA()           |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_recv     | ch       |             |             |           |        |     |        | Dd_CSIO_Start_Recv()               |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_full     | ch       |             |             |           |        |     |        | Dd_CSIO_Start_Full_Duplex()        |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_full_dma | ch       | dma_ch_send | dma_ch_recv |           |        |     |        | Dd_CSIO_Start_Full_Duplex_DMA()    |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| stop           | ch       |             |             |           |        |     |        | Dd_CSIO_Stop()                     |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| ut_send        | ch       | mode        | baudrate    | direction | length | dma | dma_ch | Dd_CSIO_Send()                     |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| get_ctrl       |          |             |             |           |        |     |        | Dd_CSIO_Get_Ctrl()                 |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_brate      | ch       | baudrate    |             |           |        |     |        | Dd_CSIO_Set_Baudrate()             |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| get_brate      | ch       |             |             |           |        |     |        | Dd_CSIO_Get_Baudrate()             |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_fset       | ch       | lost_detect |             |           |        |     |        | Dd_CSIO_Save_Send_FIFO_Pointer()   |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| set_fld        | ch       |             |             |           |        |     |        | Dd_CSIO_Reload_Send_FIFO_Pointer() |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| get_fld        | ch       |             |             |           |        |     |        | Dd_CSIO_Get_Reload_Status()        |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| reset_fifo     | ch       | fifo_num    |             |           |        |     |        | Dd_CSIO_Reset_FIFO()               |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| err            |          |             |             |           |        |     |        | Parameter error test               |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *	| start_stop     |          |             |             |           |        |     |        | Stop test during transfer(Sync)    |
 *	+----------------+----------+-------------+-------------+-----------+--------+-----+--------+------------------------------------+
 *
 *			ch			:	Chanel Number (0-5)
 *			in_freq		:	Input frequency
 *			cal_freq	:	Output frequency
 *			num			:	Test ID
 *			mode		:	Trasmit mode (0-3)
 *			baudrate	:	Baudrate
 *			direction	:	Bit direction (0=LSB first, 1=MSB first)
 *			length		:	Data bit length (5-9)
 *			data		:	Send data
 *			type		:	Type (0=Master, 1=Slave)
 *			data_num	:	Number of data
 *
 *
 *	The meaning of "ctrl" command parameters
 *	+----+---- +----+--------+---+--------+----+----------+-----+----+---------+-----+------+---+----------------+
 *	| P1 | P2  | P3 | P4     | P5| P6     | P7 | P8       | P9  | P10| P11     | P12 | P13  |P14| Meaning        |
 *	+----+-----+----+--------+---+--------+----+----------+-----+----+---------+-----+------+---+----------------+
 *	|ctrl| ch  |send|normal_h|lsb|[length]|sop0|[Baudrate]|fsel0|fe00|[fbyte_r]|fset0|flste0|cb0| Dd_CSIO_Ctrl() |
 *  |    |(0-5)|recv|normal_l|msb| (5-9)  |sop1|(bps)     |fsel1|fe12|(num)    |fset1|flste1|cb1|                |
 *  |    |     |    |spi_h   |   |        |    |          |     |fe10|         |     |      |   |                |
 *  |    |     |    |spi_l   |   |        |    |          |     |fe02|         |     |      |   |                |
 *	+----+-----+----+--------+---+--------+----+----------+-----+----+---------+-----+------+---+----------------+
 *
 * @return VOID
 */
VOID Ct_Dd_CSIO_Main( int argc, char** argv )
{
	INT32		ret;
	UCHAR		ch;
	ULONG		intid[2];
	UCHAR		dma_ch[2];
	ULONG		dma_intid[2];
	UCHAR		num;
	INT32		index;
	INT32		tmout = D_DDIM_USER_SEM_WAIT_POL;
	USHORT		baudrate;
	T_DD_CSIO_CAL		cal_tbl;
	T_DD_CSIO_CTRL		csio_ctrl;
	T_DD_CSIO_FIFO_CTRL	fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	if( strcmp(argv[1], "open") == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		tmout = atoi(argv[3]);

		ret = Dd_CSIO_Open( ch, tmout );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_CSIO_Open ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "close") == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Close( ch );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_CSIO_Close ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "set_type") == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		/* type */
		gType[ch] = (E_DD_CSIO_TYPE)atoi( argv[3] );
		Ddim_Print(("Master/Slave Type:\n"));
		for (index = 0; index < D_DD_USIO_CH_NUM_MAX; index++ ){
			Ddim_Print(("  CH[%d] =%d\n", index, gType[index]));
		}

		ct_csio_set_port();
	}
	else if( strcmp(argv[1], "cal") == 0 ){
		cal_tbl.in_freq = atoi( argv[2] );

		ret = Dd_CSIO_Calculate( &cal_tbl );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_CSIO_Calculate ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "ctrl") == 0 ){			// Set CTRL data
		/* ch number */
		ch = atoi( argv[2] );

		// Type
		csio_ctrl.type = gType[ch];

		// Trasmit mode
		if( strcmp(argv[4], "normal_h") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_NORMAL_SC_H;
		}
		else if( strcmp(argv[4], "normal_l") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_NORMAL_SC_L;
		}
		else if( strcmp(argv[4], "spi_h") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_SPI_SC_H;
		}
		else if( strcmp(argv[4], "spi_l") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_SPI_SC_L;
		}
		else{
			Ddim_Print(("please check 4th parameter!!\n"));
			return;
		}

		// LSB first or MSB first
		if( strcmp(argv[5], "lsb") == 0 ){
			csio_ctrl.bit_direction = E_DD_CSIO_BIT_DIR_LSB_FIRST;
		}
		else if( strcmp(argv[5], "msb") == 0 ){
			csio_ctrl.bit_direction = E_DD_CSIO_BIT_DIR_MSB_FIRST;
		}
		else{
			Ddim_Print( ("please check 5th parameter!!\n" ) );
			return;
		}

		// Data bit length
		if( atoi(argv[6]) == 8 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_8;
		}
		else if( atoi(argv[6]) == 5 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_5;
		}
		else if( atoi(argv[6]) == 6 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_6;
		}
		else if( atoi(argv[6]) == 7 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_7;
		}
		else if( atoi(argv[6]) == 9 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_9;
		}
		else{
			Ddim_Print( ("please check 6th parameter!!\n" ) );
			return;
		}

		gData_length = csio_ctrl.data_length;

		// Serial output port setting (0:uninfluential, 1:High )
		if( strcmp(argv[7], "sop0") == 0 ){
			csio_ctrl.sop = 0;
		}
		else if( strcmp(argv[7], "sop1") == 0 ){
			csio_ctrl.sop = 1;
		}
		else{
			Ddim_Print( ("please check 7th parameter!!\n" ) );
			return;
		}

		// Baudrate (bps) 
		cal_tbl.in_freq  = atol(argv[8]);
		cal_tbl.cal_freq = 0;
		ret = Dd_CSIO_Calculate(&cal_tbl);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			return;
		}
		csio_ctrl.baudrate = cal_tbl.cal_freq;

		csio_ctrl.sop = 0;

		// FIFO selection
		if( strcmp(argv[9], "fsel0") == 0 ){
			csio_ctrl.fifo_ctrl->fsel = 0;
		}
		else if( strcmp(argv[9], "fsel1") == 0 ){
			csio_ctrl.fifo_ctrl->fsel = 1;
		}
		else{
			Ddim_Print( ("please check 9th parameter!!\n" ) );
			return;
		}

		// FIFO 1/2 enable
		if( strcmp(argv[10], "fe00") == 0 ){
			csio_ctrl.fifo_ctrl->fe1 = 0;		// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fe2 = 0;		// FIFO 2 enable              :Disable
		}
		else if( strcmp(argv[10], "fe12") == 0 ){
			csio_ctrl.fifo_ctrl->fe1 = 1;		// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fe2 = 1;		// FIFO 2 enable              :Enable
		}
		else if( strcmp(argv[10], "fe10") == 0 ){
			csio_ctrl.fifo_ctrl->fe1 = 1;		// FIFO 1 enable              :Enable
			csio_ctrl.fifo_ctrl->fe2 = 0;		// FIFO 2 enable              :Disable
		}
		else if( strcmp(argv[10], "fe02") == 0 ){
			csio_ctrl.fifo_ctrl->fe1 = 0;		// FIFO 1 enable              :Disable
			csio_ctrl.fifo_ctrl->fe2 = 1;		// FIFO 2 enable              :Enable
		}
		else{
			Ddim_Print( ("please check 10th parameter!!\n" ) );
			return;
		}

		// FIFO receive data size
		csio_ctrl.fifo_ctrl->fbyte_recv = atoi(argv[11]);

		// FIFO reload pointer
		if( strcmp(argv[12], "fset0") == 0 ){
			csio_ctrl.fifo_ctrl->fset = 0;
		}
		else if( strcmp(argv[12], "fset1") == 0 ){
			csio_ctrl.fifo_ctrl->fset = 1;
		}
		else{
			Ddim_Print( ("please check 12th parameter!!\n" ) );
			return;
		}

		// FIFO Data-lost check
		if( strcmp(argv[13], "flste0") == 0 ){
			csio_ctrl.fifo_ctrl->flste = 0;
		}
		else if( strcmp(argv[13], "flste1") == 0 ){
			csio_ctrl.fifo_ctrl->flste = 1;
		}
		else{
			Ddim_Print( ("please check 13th parameter!!\n" ) );
			return;
		}

		// Callback function
		if( strcmp(argv[14], "cb0") == 0 ){
			csio_ctrl.pcallback = NULL;
		}
		else if( strcmp(argv[14], "cb1") == 0 ){
			csio_ctrl.pcallback = ct_csio_callback;
			gAsync = 1;
		}
		else{
			Ddim_Print( ("please check 14th parameter!!\n" ) );
			return;
		}

		// Call API
		ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "ctrl_test") == 0 ){			// Set CTRL data(for Test)
		/* ch number */
		ch = atoi( argv[2] );

		// Type
		csio_ctrl.type = gType[ch];

		// Trasmit mode
		if( strcmp(argv[3], "normal_h") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_NORMAL_SC_H;
		}
		else if( strcmp(argv[3], "normal_l") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_NORMAL_SC_L;
		}
		else if( strcmp(argv[3], "spi_h") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_SPI_SC_H;
		}
		else if( strcmp(argv[3], "spi_l") == 0 ){
			csio_ctrl.mode = E_DD_CSIO_MODE_SPI_SC_L;
		}
		else{
			Ddim_Print(("please check 3th parameter!!\n"));
			return;
		}

		// LSB first or MSB first
		if( strcmp(argv[4], "lsb") == 0 ){
			csio_ctrl.bit_direction = E_DD_CSIO_BIT_DIR_LSB_FIRST;
		}
		else if( strcmp(argv[4], "msb") == 0 ){
			csio_ctrl.bit_direction = E_DD_CSIO_BIT_DIR_MSB_FIRST;
		}
		else{
			Ddim_Print( ("please check 4th parameter!!\n" ) );
			return;
		}

		// Data bit length
		if( atoi(argv[5]) == 8 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_8;
		}
		else if( atoi(argv[5]) == 5 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_5;
		}
		else if( atoi(argv[5]) == 6 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_6;
		}
		else if( atoi(argv[5]) == 7 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_7;
		}
		else if( atoi(argv[5]) == 9 ){
			csio_ctrl.data_length = E_DD_CSIO_DATA_LENGTH_9;
		}
		else{
			Ddim_Print( ("please check 5th parameter!!\n" ) );
			return;
		}

		gData_length = csio_ctrl.data_length;

		// Serial output port setting (0:uninfluential, 1:High )
		csio_ctrl.sop = 0;

		// Baudrate (bps) 
		cal_tbl.in_freq  = atol(argv[6]);
		cal_tbl.cal_freq = 0;
		ret = Dd_CSIO_Calculate(&cal_tbl);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
			return;
		}
		csio_ctrl.baudrate = cal_tbl.cal_freq;

		csio_ctrl.sop = 0;

		// FIFO selection
		csio_ctrl.fifo_ctrl->fsel = 0;

		// FIFO 1/2 enable
		csio_ctrl.fifo_ctrl->fe1 = 1;		// FIFO 1 enable              :Enable
		csio_ctrl.fifo_ctrl->fe2 = 1;		// FIFO 2 enable              :Enable

		// FIFO receive data size
		csio_ctrl.fifo_ctrl->fbyte_recv = 1;

		// FIFO reload pointer
		csio_ctrl.fifo_ctrl->fset = 0;

		// FIFO Data-lost check
		csio_ctrl.fifo_ctrl->flste = 0;

		// Callback function
		if( strcmp(argv[7], "cb0") == 0 ){
			csio_ctrl.pcallback = NULL;
			gAsync = 0;
		}
		else if( strcmp(argv[7], "cb1") == 0 ){
			csio_ctrl.pcallback = ct_csio_callback;
			gAsync = 1;
		}
		else{
			Ddim_Print( ("please check 7th parameter!!\n" ) );
			return;
		}

		// Call API
		ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "set_send") == 0 ){			// Set send data
		UINT32 idx;
		UINT32 idx2 = 0;

		/* ch number */
		ch = atoi( argv[2] );

		// Data size
		gSend_size = atoi(argv[3]);

		if(strcmp(argv[4], "sdram") != 0){
			if (gData_length == E_DD_CSIO_DATA_LENGTH_9){
				for( idx = 0; idx < gSend_size; idx += idx2 ){
					gBuf16_2[idx] = 0x155;	// 101010101
					for( idx2 = 1; idx2 < 512; idx2++ ){		// 9bit max
						if( idx+idx2 >= gSend_size ){
							break;
						}
						else{
							gBuf16_2[idx+idx2] = idx2;
						}
					}
				}
				ret = Dd_CSIO_Set_Send_Data(ch, gBuf16_2, gSend_size);
			}
			else {
				for( idx = 0; idx < gSend_size; idx += idx2 ){
					gBuf8_2[idx] = 0xAA;	// 10101010
					for( idx2 = 1; idx2 < 256; idx2++ ){		// 8bit max
						if( idx+idx2 >= gSend_size ){
							break;
						}
						else{
							gBuf8_2[idx+idx2] = idx2;
						}
					}
				}
				ret = Dd_CSIO_Set_Send_Data(ch, gBuf8_2, gSend_size);
			}
		}
		else{
			ret = Dd_CSIO_Set_Send_Data(ch, (VOID*)D_CSIO_HDMAC_DATA_ADDR_SEND, gSend_size);
		}

		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Set Send Data ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "set_recv") == 0 ){			// Set revceive data
		/* ch number */
		ch = atoi( argv[2] );

		// Data size
		gSend_size = atoi(argv[3]);

		// Recv Buffer2 used
		gBuf2_Used = 1;

		if(strcmp(argv[4], "sdram") != 0){
			if (gData_length == E_DD_CSIO_DATA_LENGTH_9){
				memset(gRcv_buf16_2, 0, sizeof(gRcv_buf16_2));
				ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf16_2, gSend_size);
			}
			else {
				memset(gRcv_buf8_2, 0, sizeof(gRcv_buf8_2));
				ret = Dd_CSIO_Set_Recv_Data(ch, gRcv_buf8_2, gSend_size);
			}
		}
		else{
			memset((VOID*)D_CSIO_HDMAC_DATA_ADDR_RECV, 0, 4 * 1024 * 1024);	// Clear 4MB
			ret = Dd_CSIO_Set_Recv_Data(ch, (VOID*)D_CSIO_HDMAC_DATA_ADDR_RECV, gSend_size);
		}

		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Set Receive Data ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "set_test") == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		/* Test number */
		num = atoi( argv[3] );
		
		ret = ct_csio_process( ch, num );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("ct_csio_process ERR. ret=0x%x\n", ret) );
		}
	}
	else if ( strcmp(argv[1], "get_ctrl") == 0 ){
		T_DD_CSIO_CTRL csio_ctrl;
		T_DD_CSIO_FIFO_CTRL fifo_ctrl;

		csio_ctrl.fifo_ctrl = &fifo_ctrl;

		/* ch number */
		ch = atoi( argv[2] );

		Dd_CSIO_Get_Ctrl(ch, &csio_ctrl);
		Ddim_Print(("CSIO Ctrl_Get OK.\n"));
		Ddim_Print(("CSIO baudrate              =%lu\n", csio_ctrl.baudrate));
		Ddim_Print(("CSIO mode                  =%d\n", csio_ctrl.mode));
		Ddim_Print(("CSIO bit_direction         =%d\n", csio_ctrl.bit_direction));
		Ddim_Print(("CSIO data_length           =%d\n", csio_ctrl.data_length));
		Ddim_Print(("CSIO type                  =%d\n", csio_ctrl.type));
		Ddim_Print(("CSIO output                =%d\n", csio_ctrl.sop));
		Ddim_Print(("CSIO pcallback             =%p\n", csio_ctrl.pcallback));
		Ddim_Print(("CSIO fifo_ctrl->fsel       =%d\n", csio_ctrl.fifo_ctrl->fsel));
		Ddim_Print(("CSIO fifo_ctrl->fe2        =%d\n", csio_ctrl.fifo_ctrl->fe2));
		Ddim_Print(("CSIO fifo_ctrl->fe1        =%d\n", csio_ctrl.fifo_ctrl->fe1));
		Ddim_Print(("CSIO fifo_ctrl->fbyte_recv =%d\n", csio_ctrl.fifo_ctrl->fbyte_recv));
	}
	else if ( strcmp( argv[1], "start_send" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Start_Send(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Start Send ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "start_send_dma" ) == 0 ){
		UCHAR dma_mode;

		/* ch number */
		ch = atoi( argv[2] );
		dma_ch[0] = atoi( argv[3] );
		dma_mode = atoi( argv[4] );

		intid[0] = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch;
		dma_intid[0] = E_DD_GIC_INTID_HDMAC1_CH0_INT + dma_ch[0];

		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 0, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt disable */
		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 1, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt enable */

		if(gData_length == E_DD_CSIO_DATA_LENGTH_9){
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)gBuf16_2, sizeof(gBuf16_2));
		}
		else{
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)gBuf8_2, sizeof(gBuf8_2));
		}

		ret = Dd_CSIO_Start_Send_DMA(ch, dma_ch[0], dma_mode);

//		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 1, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt enable */
//		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 0, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt disable */

		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Start Send ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "start_recv" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Start_Recv(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Start Receive ERR. ret=0x%x\n", ret));
		}

		if((gSend_size <= 300) && (!gAsync)){
			if ( gBuf2_Used ) {
				for (index = 0; index < gSend_size; index++ ){
					Ddim_Print(("Rcv_buf8_2[%d]=0x%X    Rcv_buf16_2[%d]=0x%X\n", index, gRcv_buf8_2[index], index, gRcv_buf16_2[index]));
				}
			}
			else {
				for (index = 0; index < gSend_size; index++ ){
					Ddim_Print(("Rcv_buf8[%d]=0x%X    Rcv_buf16[%d]=0x%X\n", index, gRcv_buf8[index], index, gRcv_buf16[index]));
				}
			}
		}
	}
	else if ( strcmp( argv[1], "start_recv_dma" ) == 0 ){
		UCHAR dma_mode;

		/* ch number */
		ch = atoi( argv[2] );
		dma_ch[0] = atoi( argv[3] );
		dma_mode = atoi( argv[4] );

		intid[0] = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch;
		dma_intid[0] = E_DD_GIC_INTID_HDMAC1_CH0_INT + dma_ch[0];

		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 0, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt disable */
		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 1, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt enable */

		if(gData_length == E_DD_CSIO_DATA_LENGTH_9){
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)gRcv_buf16_2, sizeof(gRcv_buf16_2));
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)D_CSIO_HDMAC_DATA_ADDR_RECV, gSend_size);
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)gRcv_buf16, sizeof(gRcv_buf16));
		}
		else{
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)gRcv_buf8_2, sizeof(gRcv_buf8_2));
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)D_CSIO_HDMAC_DATA_ADDR_RECV, gSend_size);
			DDIM_User_L1l2cache_Clean_Flush_Addr((ULONG)gRcv_buf8, sizeof(gRcv_buf8));
		}

		ret = Dd_CSIO_Start_Recv_DMA(ch, dma_ch[0], dma_mode);

//		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 1, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt enable */
//		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 0, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt disable */

		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Start Recv ERR. ret=0x%x\n", ret));
		}

		if((gSend_size <= 300) && (!gAsync)){
			if ( gBuf2_Used ) {
				for (index = 0; index < gSend_size; index++ ){
					Ddim_Print(("gRcv_buf8_2[%d]=0x%X    gRcv_buf16_2[%d]=0x%X\n", index, gRcv_buf8_2[index], index, gRcv_buf16_2[index]));
				}
			}
			else if ( gNon_Chache_Area_Used ) {
				for (index = 0; index < gSend_size; index++ ){
					Ddim_Print(("Rcv_Data8[%d]=0x%X    Rcv_Data16[%d]=0x%X\n", index, ((UCHAR*)D_CSIO_HDMAC_DATA_ADDR_RECV)[index], index, ((USHORT*)D_CSIO_HDMAC_DATA_ADDR_RECV)[index]));
				}
			}
			else {
				for (index = 0; index < gSend_size; index++ ){
					Ddim_Print(("Rcv_buf8[%d]=0x%X    Rcv_buf16[%d]=0x%X\n", index, gRcv_buf8[index], index, gRcv_buf16[index]));
				}
			}
		}
	}
	else if ( strcmp( argv[1], "start_full" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Start_Full_Duplex(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Full Duplex Start ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "start_full_dma" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		dma_ch[0] = atoi( argv[3] );
		dma_ch[1] = atoi( argv[4] );

		intid[0] = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch;
		intid[1] = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch;
		dma_intid[0] = E_DD_GIC_INTID_HDMAC1_CH0_INT + dma_ch[0];
		dma_intid[1] = E_DD_GIC_INTID_HDMAC1_CH0_INT + dma_ch[1];

		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 0, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt disable */
		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[1], 0, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt disable */
		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 1, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt enable */
		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[1], 1, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt enable */

		if(gData_length == E_DD_CSIO_DATA_LENGTH_9){
			DDIM_User_L1l2cache_Clean_Flush_Addr(D_CSIO_HDMAC_DATA_ADDR_SEND, 0x00400000);
			DDIM_User_L1l2cache_Clean_Flush_Addr(D_CSIO_HDMAC_DATA_ADDR_RECV, 0x00400000);
		}
		else{
			DDIM_User_L1l2cache_Clean_Flush_Addr(D_CSIO_HDMAC_DATA_ADDR_SEND, 0x00400000);
			DDIM_User_L1l2cache_Clean_Flush_Addr(D_CSIO_HDMAC_DATA_ADDR_RECV, 0x00400000);
		}

		ret = Dd_CSIO_Start_Full_Duplex_DMA(ch, dma_ch[0], dma_ch[1]);

//		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 1, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt enable */
//		Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[1], 1, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt enable */
//		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 0, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt disable */
//		Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[1], 0, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt disable */

		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Full Duplex DMA ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "stop" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Stop(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Stop ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "ut_send" ) == 0 ){
		T_DD_CSIO_SEND_CTRL csio_util_ctrl;

		/* ch number */
		ch = atoi( argv[2] );

		csio_util_ctrl.mode 		 = (E_DD_CSIO_MODE) (atoi( argv[3] ));
		csio_util_ctrl.baudrate 	 = atol( argv[4] );
		csio_util_ctrl.bit_direction = (E_DD_CSIO_BIT_DIR) (atoi( argv[5] ));
		csio_util_ctrl.data_length	 = (E_DD_CSIO_DATA_LENGTH) (atoi( argv[6] ));

		if( atoi( argv[7] ) == 1 ){
			dma_ch[0] = atoi( argv[8] );

			csio_util_ctrl.fifo_dma_enable	 = 1;
			csio_util_ctrl.dma_ch			 = dma_ch[0];

			intid[0] = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch;
			dma_intid[0] = E_DD_GIC_INTID_HDMAC1_CH0_INT + dma_ch[0];

			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 0, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt disable */
			Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 1, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt enable */

			ret = Dd_CSIO_Send( ch, &csio_util_ctrl, gBuf8, 3 );

			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid[0], 1, D_DD_GIC_PRI30, 0x1);			/** CSIO interrupt enable */
			Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid[0], 0, D_DD_GIC_PRI30, 0x1);		/** HDMAC1 interrupt disable */

			if (ret != D_DDIM_OK) {
				Ddim_Print( ("Dd_CSIO_Send ERR. ret=0x%x\n", ret) );
			}
		
		}
		else{
			csio_util_ctrl.fifo_dma_enable	 = 0;
			csio_util_ctrl.dma_ch			 = 0;

			ret = Dd_CSIO_Send( ch, &csio_util_ctrl, gBuf8, 3 );
			if (ret != D_DDIM_OK) {
				Ddim_Print( ("Dd_CSIO_Send ERR. ret=0x%x\n", ret) );
			}
		}
	}
	else if ( strcmp( argv[1], "set_brate" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* baudrate */
		baudrate = atoi( argv[3] );

		ret = Dd_CSIO_Set_Baudrate(ch, baudrate);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_CSIO_Set_Baudrate ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "get_brate" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Get_Baudrate(ch, &baudrate);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_CSIO_Get_Baudrate ERR. ret=0x%x\n", ret));
		}
		else {
			Ddim_Print(("Dd_CSIO_Get_Baudrate: Baudrate=%d\n", baudrate));
		}
	}
	else if ( strcmp( argv[1], "set_fset" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* lost_detect */
		num = atoi( argv[3] );

		ret = Dd_CSIO_Save_Send_FIFO_Pointer(ch, num);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_CSIO_Save_Send_FIFO_Pointer ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "set_fld" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Reload_Send_FIFO_Pointer(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_CSIO_Reload_Send_FIFO_Pointer ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "get_fld" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_CSIO_Get_Reload_Status(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_CSIO_Get_Reload_Status ERR. ret=0x%x\n", ret));
		}
		else {
			Ddim_Print(("Dd_CSIO_Get_Reload_Status: FLD = %d\n", ret));
		}
	}
	else if ( strcmp( argv[1], "reset_fifo" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* fifo_num */
		num = atoi( argv[3] );

		ret = Dd_CSIO_Reset_FIFO(ch, num);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_CSIO_Reset_FIFO ERR. ret=0x%x\n", ret));
		}
	}
	else if (strcmp(argv[1], "err") == 0){
		T_DD_CSIO_CTRL csio_ctrl;
		T_DD_CSIO_SEND_CTRL csio_send_ctrl;

		ret = Dd_CSIO_Open( D_DD_USIO_CH_NUM_MAX, tmout );
		Ddim_Print(("Dd_CSIO_Open(): CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Open( 0, -2 );
		Ddim_Print(("Dd_CSIO_Open(): tmout value error. ret=%X\n", ret ));

		ret = Dd_CSIO_Close( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Close(): CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Ctrl( D_DD_USIO_CH_NUM_MAX, &csio_ctrl );
		Ddim_Print(("Dd_CSIO_Ctrl(): CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Ctrl( 0, NULL );
		Ddim_Print(("Dd_CSIO_Ctrl(): csio_ctrl is NULL. ret=%X\n", ret ));


		ret = Dd_CSIO_Get_Ctrl( D_DD_USIO_CH_NUM_MAX, &csio_ctrl );
		Ddim_Print(("Dd_CSIO_Get_Ctrl(): CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Get_Ctrl( 0, NULL );
		Ddim_Print(("Dd_CSIO_Get_Ctrl(): csio_ctrl is NULL. ret=%X\n", ret ));


		ret = Dd_CSIO_Set_Send_Data( D_DD_USIO_CH_NUM_MAX, gBuf8, 1 );
		Ddim_Print(("Dd_CSIO_Set_Send_Data(): CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Set_Send_Data( 0, NULL, 1 );
		Ddim_Print(("Dd_CSIO_Set_Send_Data(): send_addr is NULL. ret=%X\n", ret ));

		ret = Dd_CSIO_Set_Send_Data( 0, gBuf8, 0 );
		Ddim_Print(("Dd_CSIO_Set_Send_Data(): num is 0. ret=%X\n", ret ));


		ret = Dd_CSIO_Start_Send( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Start_Send(): CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Set_Recv_Data( D_DD_USIO_CH_NUM_MAX, gRcv_buf8, 1 );
		Ddim_Print(("Dd_CSIO_Set_Recv_Data(): CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Set_Recv_Data( 0, NULL, 1 );
		Ddim_Print(("Dd_CSIO_Set_Recv_Data(): recv_addr is NULL. ret=%X\n", ret ));

		ret = Dd_CSIO_Set_Recv_Data( 0, gRcv_buf8, 0 );
		Ddim_Print(("Dd_CSIO_Set_Recv_Data(): num is 0. ret=%X\n", ret ));


		ret = Dd_CSIO_Start_Recv( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Start_Recv(): CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Start_Full_Duplex( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Start_Full_Duplex(): CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Stop( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Stop(): CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Calculate( NULL );
		Ddim_Print(("Dd_CSIO_Calculate(): cal_tbl is NULL. ret=%X\n", ret ));


		Dd_CSIO_Int_Handler_Tx( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Int_Handler_Tx(): CH number error.\n"));


		Dd_CSIO_Int_Handler_Rx( D_DD_USIO_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Int_Handler_Rx(): CH number error.\n"));


		ret = Dd_CSIO_Send( D_DD_USIO_CH_NUM_MAX, &csio_send_ctrl, gBuf8, 1 );
		Ddim_Print(("Dd_CSIO_Send(): CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Send( 0, NULL, gBuf8, 1 );
		Ddim_Print(("Dd_CSIO_Send(): csio_send_ctrl is NULL. ret=%X\n", ret ));

		ret = Dd_CSIO_Send( 0, &csio_send_ctrl, NULL, 1 );
		Ddim_Print(("Dd_CSIO_Send(): send_addr is NULL. ret=%X\n", ret ));

		ret = Dd_CSIO_Send( 0, &csio_send_ctrl, gBuf8, 0 );
		Ddim_Print(("Dd_CSIO_Send(): num is 0. ret=%X\n", ret ));


		ret = Dd_CSIO_Start_Send_DMA( D_DD_USIO_CH_NUM_MAX, 5, D_DD_CSIO_SEND_DMA_MODE_DEMAND );
		Ddim_Print(("Dd_CSIO_Start_Send_DMA(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Start_Send_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX, D_DD_CSIO_SEND_DMA_MODE_DEMAND );
		Ddim_Print(("Dd_CSIO_Start_Send_DMA(): DMA CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Start_Send_DMA( 0, 0, D_DD_CSIO_SEND_DMA_MODE_DEMAND + 1 );
		Ddim_Print(("Dd_CSIO_Start_Send_DMA(): DMA mode error. ret=%X\n", ret ));


		ret = Dd_CSIO_Start_Recv_DMA( D_DD_USIO_CH_NUM_MAX, 5, D_DD_CSIO_RECV_DMA_MODE_DEMAND );
		Ddim_Print(("Dd_CSIO_Start_Recv_DMA(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Start_Recv_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX, D_DD_CSIO_RECV_DMA_MODE_DEMAND  );
		Ddim_Print(("Dd_CSIO_Start_Recv_DMA(): DMA CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Start_Recv_DMA( 0, 0, D_DD_CSIO_RECV_DMA_MODE_DEMAND + 1 );
		Ddim_Print(("Dd_CSIO_Start_Recv_DMA(): DMA mode error. ret=%X\n", ret ));


		ret = Dd_CSIO_Start_Full_Duplex_DMA( D_DD_USIO_CH_NUM_MAX, 0, 1 );
		Ddim_Print(("Dd_CSIO_Start_Full_DMA(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Start_Full_Duplex_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX, 7 );
		Ddim_Print(("Dd_CSIO_Start_Full_DMA(): DMA CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Start_Full_Duplex_DMA( 0, 7, D_DD_HDMAC1_CH_NUM_MAX );
		Ddim_Print(("Dd_CSIO_Start_Full_DMA(): DMA CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Set_Baudrate(D_DD_USIO_CH_NUM_MAX, 10000);
		Ddim_Print(("Dd_CSIO_Set_Baudrate(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Get_Baudrate(D_DD_USIO_CH_NUM_MAX, &baudrate);
		Ddim_Print(("Dd_CSIO_Get_Baudrate(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Get_Baudrate(0, NULL);
		Ddim_Print(("Dd_CSIO_Get_Baudrate(): baud_rate is NULL. ret=%X\n", ret ));


		ret = Dd_CSIO_Save_Send_FIFO_Pointer(D_DD_USIO_CH_NUM_MAX, 0);
		Ddim_Print(("Dd_CSIO_Save_Send_FIFO_Pointer(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Save_Send_FIFO_Pointer(0, 2);
		Ddim_Print(("Dd_CSIO_Save_Send_FIFO_Pointer(): CSIO lost_detect error. ret=%X\n", ret ));


		ret = Dd_CSIO_Reload_Send_FIFO_Pointer(D_DD_USIO_CH_NUM_MAX);
		Ddim_Print(("Dd_CSIO_Reload_Send_FIFO_Pointer(): CSIO CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Get_Reload_Status(D_DD_USIO_CH_NUM_MAX);
		Ddim_Print(("Dd_CSIO_Get_Reload_Status(): CSIO CH number error. ret=%X\n", ret ));


		ret = Dd_CSIO_Reset_FIFO(D_DD_USIO_CH_NUM_MAX, 0);
		Ddim_Print(("Dd_CSIO_Reset_FIFO(): CSIO CH number error. ret=%X\n", ret ));

		ret = Dd_CSIO_Reset_FIFO(0, 0);
		Ddim_Print(("Dd_CSIO_Reset_FIFO(): fifo_num number error. ret=%X\n", ret ));
	}
	else if ( strcmp( argv[1], "start_stop" ) == 0 ){
		/* ch number */
		gSending_ch_number = atoi( argv[2] );

		ret = Dd_TMR32_Open( D_CSIO_TIMER_CH, D_DDIM_USER_SEM_WAIT_POL );
		if (ret != D_DDIM_OK) {
			Ddim_Print(("timer open error : ret=%d \n", ret));
			return;
		}

		ret = Dd_TMR32_SetTimer( D_CSIO_TIMER_CH, 150*1000, ct_csio_timer_handler );
		if(ret != D_DDIM_OK) {
			Dd_TMR32_Stop(D_CSIO_TIMER_CH);
			Dd_TMR32_Close(D_CSIO_TIMER_CH);
			Ddim_Print(("timer set error : ret=%d \n", ret));
			return;
		}

		gSend_flg = 1;
		ret = Dd_TMR32_Start(D_CSIO_TIMER_CH);
		if(ret != D_DDIM_OK) {
			gSend_flg = 0;
			Ddim_Print(("timer start error : ret=%d \n", ret));
			return;
		}

		ret = Dd_CSIO_Start_Send(gSending_ch_number);
		Dd_TMR32_Stop(D_CSIO_TIMER_CH);
		Dd_TMR32_Close(D_CSIO_TIMER_CH);
		gSend_flg = 0;
		gSending_ch_number = 0;
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("CSIO Start Send ERR. ret=0x%x\n", ret));
		}
	}
	else{
		Ddim_Print(("please check 1st parameter!!\n"));
	}
	return ;
}

/**
 * @brief	CSIO PCSim Test.
 * @param	VOID
 * @return	VOID
 */
VOID Ct_Dd_CSIO_PCSim_Test( VOID )
{
#ifdef CO_DEBUG_ON_PC
	int cmd_cnt = 0;
	
	// CSIO Test command data
	char csio_test_argv1[][D_CSIO_TEST_CMD_LEN] = { "ddcsio", "open", "0", "0" };
	char csio_test_argv2[][D_CSIO_TEST_CMD_LEN] = { "ddcsio", "ctrl", "0", "0", "normal_h",
													"lsb", "8", "sop0", "1200", "fsel0",
													"fe00", "2", "fset0", "flste0", "cb1" };
	char csio_test_argv3[][D_CSIO_TEST_CMD_LEN] = { "ddcsio", "get_ctrl", "0" };
	char csio_test_argv4[][D_CSIO_TEST_CMD_LEN] = { "ddcsio", "set_brate", "0", "3488" };
	
	// Test CSIO
	
	Ddim_Print(("\n======== CSIO ========\n"));
	Ddim_Print((">> Test 1-1-1 ~ 1-1-6\n"));
	// Dd_CSIO_Open
	cmd_cnt = 4;
	ct_csio_set_cmd(cmd_cnt, (char*)csio_test_argv1);
	Ct_Dd_CSIO_Main(cmd_cnt, gCmd_Argv);
	
	// Dd_CSIO_Ctrl
	cmd_cnt = 15;
	ct_csio_set_cmd(cmd_cnt, (char*)csio_test_argv2);
	Ct_Dd_CSIO_Main(cmd_cnt, gCmd_Argv);
	
	// check register
	ct_csio_reg_dump(0);
	
	Ddim_Print(("\n>> Test 1-2-1 ~ 1-2-6\n"));
	// Dd_CSIO_Get_Ctrl
	cmd_cnt = 3;
	ct_csio_set_cmd(cmd_cnt, (char*)csio_test_argv3);
	Ct_Dd_CSIO_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-3-1\n"));
	// Dd_UART_Open (same ch)
	cmd_cnt = 4;
	ct_csio_set_cmd(cmd_cnt, (char*)csio_test_argv4);
	Ct_Dd_CSIO_Main(cmd_cnt, gCmd_Argv);
	
	// check register
	ct_csio_reg_dump(0);

	Ddim_Print(("\n>> Test 2-1-1\n"));
	// Dd_UART_Open (same ch)
	cmd_cnt = 4;
	ct_csio_set_cmd(cmd_cnt, (char*)csio_test_argv1);
	Ct_Dd_CSIO_Main(cmd_cnt, gCmd_Argv);
#endif	// CO_DEBUG_ON_PC
}
