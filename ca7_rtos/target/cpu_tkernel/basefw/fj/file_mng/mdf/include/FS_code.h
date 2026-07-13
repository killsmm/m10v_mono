/*************************************************************************
 *
 *    Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. File name
 *        FS_code.h
 * 2. Outline of source file
 *        UNICODE, ASCII code
 *
 ************************************************************************/

#ifndef _FS_CODE_H_
#define _FS_CODE_H_

#define FS_ASCII_MIN        0x20
#define FS_ASCII_SPC        0x20    /* SP */
#define FS_ASCII_PLUS       0x2b    /* + */
#define FS_ASCII_COMMA      0x2c    /* , */
#define FS_ASCII_DOT        0x2e    /* . */
#define FS_ASCII_0          0x30    /* 0 */
#define FS_ASCII_9          0x39    /* 9 */
#define FS_ASCII_SEMICOLON  0x3b    /* ; */
#define FS_ASCII_EQUAL      0x3d    /* = */
#define FS_ASCII_A          0x41    /* A */
#define FS_ASCII_Z          0x5a    /* Z */
#define FS_ASCII_LBRACKET   0x5b    /* [ */
#define FS_ASCII_RBRACKET   0x5d    /* ] */
#define FS_ASCII_a          0x61    /* a */
#define FS_ASCII_z          0x7a    /* z */
#define FS_ASCII_TIL        0x7E    /* ~ */
#define FS_ASCII_MAX        0x7E

#define FS_ASCII_CV         (FS_ASCII_a - FS_ASCII_A) /* 'a' - 'A' */

#define FS_UNICODE_NULL     0x00    /* UNICODE NULL */
#define FS_UNICODE_SPC      0xFF    /* Surplus area */

#endif


