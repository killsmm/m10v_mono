/************************************************************************
 *
 *    Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. File name
 *        FS_Endian.h
 * 2. Outline of source file
 *        Macro definition for access of Little endian area
 *
 ************************************************************************/

#ifndef __FS_ENDIAN_H__
#define __FS_ENDIAN_H__

#ifdef  FS_CPUENDIAN_LITTLE
/*
 * Data is acquired from two byte Little endian area where the alignment is done
 * Argument : adr2    The first address in two byte area
 * Return   : Two byte numerical value
 */
#define FS_GETAL2(adr2)         (*(FS_WORD*)(adr2))

/*
 * It writes it in the Little endian area where two byte numerical value is done in the alignment
 * Argument : adr2    The first address in two byte area
 * Return   : None
 */
#define FS_PUTAL2(adr2, num)    (*(FS_WORD*)(adr2) = (FS_WORD)(num))

/*
 * Data is acquired from four byte Little endian area where the alignment is done
 * Argument : adr4    The first address in four byte area
 * Return   : Four byte numerical value
 */
#define FS_GETAL4(adr4)         (*(FS_DWORD*)(adr4))

/*
 * It writes it in the Little endian area where four byte numerical value is done in the alignment
 * Argument : adr4    The first address in four byte area
 * Return   : None
 */
#define FS_PUTAL4(adr4, num)    (*(FS_DWORD*)(adr4) = (num))

/*
 * Data is acquired from eight byte Little endian area where the alignment is done
 * Argument : adr8    The first address in eight byte area
 * Return   : Eight byte numerical value
 */
#define FS_GETAL8(adr8)         (*(FS_QWORD*)(adr8))

/*
 * It writes it in the Little endian area where eight byte numerical value is done in the alignment
 * Argument : adr8    The first address in eight byte area
 * Return   : None
 */
#define FS_PUTAL8(adr8, num)    (*(FS_QWORD*)(adr8) = (num))



#define FS_SWAP2(bignum)        ((FS_WORD)(bignum))
#define FS_SWAP4(bignum)        ((FS_DWORD)(bignum))

#else

#define FS_GETAL2(adr2)         ((*(FS_WORD*)(adr2) >> 8)                               \
                                    | ((*(FS_WORD*)(adr2) & 0xff) << 8))

#define FS_PUTAL2(adr2, num)    (*(FS_WORD*)(adr2)                                      \
                                    = (FS_WORD)((FS_WORD)(num) >> 8)                    \
                                    | ((FS_WORD)(num) << 8))

#define FS_GETAL4(adr4)         ((*(FS_DWORD*)(adr4) >> 24)                             \
                                    | ((*(FS_DWORD*)(adr4) & 0xff0000) >> 8)            \
                                    | ((*(FS_DWORD*)(adr4) & 0xff00) << 8)              \
                                    | (*(FS_DWORD*)(adr4) << 24))

#define FS_PUTAL4(adr4, num)    (*(FS_DWORD*)(adr4)                                     \
                                    = ((FS_DWORD)(num) >> 24)                           \
                                    | (((num) & 0xff0000) >> 8)                         \
                                    | (((num) & 0xff00) << 8)                           \
                                    | ((num) << 24))
                                    
#define FS_GETAL8(adr8)         ((*(FS_QWORD*)(adr8) >> 56)                             \
                                    | ((*(FS_QWORD*)(adr8) & 0xff000000000000) >> 40)   \
                                    | ((*(FS_QWORD*)(adr8) & 0xff0000000000) >> 24)     \
                                    | ((*(FS_QWORD*)(adr8) & 0xff00000000) >> 8)        \
                                    | ((*(FS_QWORD*)(adr8) & 0xff000000) << 8)          \
                                    | ((*(FS_QWORD*)(adr8) & 0xff0000) << 24)           \
                                    | ((*(FS_QWORD*)(adr8) & 0xff00) << 40)             \
                                    | ((*(FS_QWORD*)(adr8) & 0xff) << 56))

#define FS_PUTAL8(adr8, num)    (*(FS_QWORD*)(adr8)                                     \
                                    = ((FS_QWORD)((num) & 0xff00000000000000) >> 56)    \
                                    | ((FS_QWORD)((num) & 0xff000000000000) >> 40)      \
                                    | ((FS_QWORD)((num) & 0xff0000000000) >> 24)        \
                                    | ((FS_QWORD)((num) & 0xff00000000) >> 8)           \
                                    | ((FS_QWORD)((num) & 0xff000000) << 8)             \
                                    | ((FS_QWORD)((num) & 0xff0000) << 24)              \
                                    | ((FS_QWORD)((num) & 0xff00) << 40)                \
                                    | ((FS_QWORD)((num) & 0xff) << 56))

#define FS_SWAP2(bignum)        ((FS_WORD)(((0xff00 & (bignum)) >> 8) | ((0x00ff & (bignum)) << 8)))
#define FS_SWAP4(bignum)        ((((FS_DWORD)(bignum)) >> 24) | ((((FS_DWORD)(bignum)) & 0xff0000) >> 8) | \
                                    ((((FS_DWORD)(bignum)) & 0xff00) << 8) | (((FS_DWORD)(bignum)) << 24))

#endif

/* Replacement for keeping interchangeability with MDF V02L02 */
#define FS_GETNotAL2(adr2)      FS_GETNOTAL2(adr2)
#define FS_GETNotAL4(adr4)      FS_GETNOTAL4(adr4)
#define FS_PUTNotAL2(adr2, num) FS_PUTNOTAL2(adr2, num)
#define FS_PUTNotAL4(adr4, num) FS_PUTNOTAL4(adr4, num)


/*
 * Data is acquired from two byte Little endian area where the alignment is not done
 * Argument : adr2    The first address in two byte area
 * Return   : Two byte numerical value
 */
#define FS_GETNOTAL2(adr2)      ((*(FS_BYTE*)(adr2))                                    \
                                    | ((FS_WORD)(*((FS_BYTE*)(adr2) + 1)) << 8))

/*
 * It writes it in the Little endian area where two byte numerical value is not done in the alignment
 * Argument : adr2    The first address in two byte area
 * Return   : None
 */
#define FS_PUTNOTAL2(adr2, num) { *(FS_BYTE*)(adr2) = (FS_BYTE)(num);                   \
                                    *((FS_BYTE*)(adr2) + 1) = (FS_BYTE)((num) >> 8); }

/*
 * Data is acquired from four byte Little endian area where the alignment is not done
 * Argument : adr4    The first address in four byte area
 * Return   : Four byte numerical value
 */
#define FS_GETNOTAL4(adr4)      ((*(FS_BYTE*)(adr4))                                    \
                                    | ((FS_DWORD)(*((FS_BYTE*)(adr4) + 1)) << 8)        \
                                    | ((FS_DWORD)(*((FS_BYTE*)(adr4) + 2)) << 16)       \
                                    | ((FS_DWORD)(*((FS_BYTE*)(adr4) + 3)) << 24))

/*
 * It writes it in the Little endian area where four byte numerical value is not done in the alignment
 * Argument : adr4    The first address in four byte area
 * Return   : None
 */
#define FS_PUTNOTAL4(adr4, num) { *(FS_BYTE*)(adr4) = (FS_BYTE)(num);                   \
                                    *((FS_BYTE*)(adr4) + 1) = (FS_BYTE)((FS_DWORD)(num) >> 8);    \
                                    *((FS_BYTE*)(adr4) + 2) = (FS_BYTE)((FS_DWORD)(num) >> 16);   \
                                    *((FS_BYTE*)(adr4) + 3) = (FS_BYTE)((FS_DWORD)(num) >> 24); }
                                    
/*
 * Data is acquired from eight byte Little endian area where the alignment is not done
 * Argument : adr8    The first address in eight byte area
 * Return   : Eight byte numerical value
 */
#define FS_GETNOTAL8(adr8)      ((*(FS_BYTE*)(adr8))                                    \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 1)) << 8)        \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 2)) << 16)       \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 3)) << 24)       \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 4)) << 32)       \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 5)) << 40)       \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 6)) << 48)       \
                                    | ((FS_QWORD)(*((FS_BYTE*)(adr8) + 7)) << 56))      \

/*
 * It writes it in the Little endian area where eight byte numerical value is not done in the alignment
 * Argument : adr8    The first address in eight byte area
 * Return   : None
 */
#define FS_PUTNOTAL8(adr8, num) { *(FS_BYTE*)(adr8) = (FS_BYTE)(num);                   \
                                    *((FS_BYTE*)(adr8) + 1) = (FS_BYTE)((num) >> 8);    \
                                    *((FS_BYTE*)(adr8) + 2) = (FS_BYTE)((num) >> 16);   \
                                    *((FS_BYTE*)(adr8) + 3) = (FS_BYTE)((num) >> 24);   \
                                    *((FS_BYTE*)(adr8) + 4) = (FS_BYTE)((num) >> 32);   \
                                    *((FS_BYTE*)(adr8) + 5) = (FS_BYTE)((num) >> 40);   \
                                    *((FS_BYTE*)(adr8) + 6) = (FS_BYTE)((num) >> 48);   \
                                    *((FS_BYTE*)(adr8) + 7) = (FS_BYTE)((num) >> 56); } \

#endif /* __FS_ENDIAN_H__ */
