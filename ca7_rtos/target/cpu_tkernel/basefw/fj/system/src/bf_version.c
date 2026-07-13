/**
 * @file		bf_version.c
 * @brief		bf_version process api
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015,2016 Socionext Inc.</I></B>
 */

#include "boot.h"

/*---------------------------------------------------------------*/
/* Definition
 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration
 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure
 */
/*---------------------------------------------------------------*/
typedef struct {
  CHAR str[16]; //// key word to search at DATA.bin
  USHORT mp;    /// Release version for MP
  UCHAR sub;    /// Release version for sub
  UCHAR debug;  /// Release version for Debug
} T_OK_VERSION;

/*---------------------------------------------------------------*/
/* Global Data
 */
/*---------------------------------------------------------------*/
/**
         For example:
                 0001.0000 - 2015.01.01 v0.1.0
                 0002.0000 - 2015.02.01 v0.2.0
                 0003.0000 - 2015.03.01 v0.3.0

                 0100.0000 - 2015.02.06 (formal release V01L00)
 */

static const T_OK_VERSION g_OK_Version = {
    // 2021.01.20 V02.3.10
    .str = "firmware ver.",
    .mp = 0x0002,
    .sub = 0x06,
    .debug = 0x00,
};

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_SUPPORT_60FPS
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_60FPS BEGIN ---
static char *gFW_type_str = "ES1 60fps";
// --- REMOVE_60FPS END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#else
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ONLY_30FPS BEGIN ---
static char *gFW_type_str = "ES1 30fps";
// --- REMOVE_ONLY_30FPS END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_SUPPORT_60FPS
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
static char *gFW_type_str = "ES3";
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

#ifdef BOARD_TYPE_SNI_SMALL
static char *gBoard_type_str = "SmartKit";
#else
static char *gBoard_type_str = "Big Board";
#endif

#ifdef CO_MEMORY_1GB
static char *gMemory_size_type_str = "1GB";
#else
static char *gMemory_size_type_str = "2GB";
#endif

/*----------------------------------------------------------------*/
/* Macro
 */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function
 */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function
 */
/*----------------------------------------------------------------*/
unsigned long fj_get_ok_version(void) {
  return (g_OK_Version.mp << 16) | (g_OK_Version.sub << 8) |
         (g_OK_Version.debug);
}

const CHAR *BF_Get_OK_Version_str(void) { return g_OK_Version.str; }

unsigned short BF_Get_OK_MP_Version(void) { return g_OK_Version.mp; }

unsigned char BF_Get_OK_Sub_Version(void) { return g_OK_Version.sub; }

unsigned char BF_Get_OK_Debug_Version(void) { return g_OK_Version.debug; }

char *BF_Get_OK_FW_type(void) { return gFW_type_str; }

char *BF_Get_OK_Board_type(void) { return gBoard_type_str; }

char *BF_Get_OK_Memory_size_type(void) { return gMemory_size_type_str; }
