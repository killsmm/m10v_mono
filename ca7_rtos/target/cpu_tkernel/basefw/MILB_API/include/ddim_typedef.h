/**
 * @file		ddim_typedef.h
 * @brief		Common typedef
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup common
@{
*/

#ifndef _DDIM_TYPEDEF_H_
#define _DDIM_TYPEDEF_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define VOID			void					/**< Type is defined to VOID */
#define CHAR			char					/**< Type is defined to CHAR */
#define UCHAR			unsigned char			/**< Type is defined to UCHAR */

#ifndef NULL
#define NULL			(0)						/**< Null value */
#endif	// NULL

#ifndef TRUE
#define TRUE			(1)						/**< True value */
#endif	// TRUE

#ifndef FALSE
#define FALSE			(0)						/**< False value */
#endif	// FALSE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Type Definition														*/
/*----------------------------------------------------------------------*/
typedef unsigned char		BYTE;				/**< Type is defined to BYTE */
typedef unsigned char*		PBYTE;				/**< Type is defined to PBYTE */
typedef signed short		SHORT;				/**< Type is defined to SHORT */
typedef unsigned short		USHORT;				/**< Type is defined to USHORT */
typedef signed char			INT8;				/**< Type is defined to INT8 */
typedef signed short		INT16;				/**< Type is defined to INT16 */
typedef signed int			INT32;				/**< Type is defined to INT32 */
typedef signed long long	INT64;				/**< Type is defined to INT64 */
typedef unsigned char		UINT8;				/**< Type is defined to UINT8 */
typedef unsigned short		UINT16;				/**< Type is defined to UINT16 */
typedef unsigned int		UINT32;				/**< Type is defined to UINT32 */
typedef unsigned long long	UINT64;				/**< Type is defined to UINT64 */
typedef signed long			LONG;				/**< Type is defined to LONG */
typedef unsigned long		ULONG;				/**< Type is defined to ULONG */
typedef long long			LLONG;				/**< Type is defined to LLONG */
typedef unsigned long long	ULLONG;				/**< Type is defined to ULLONG */
typedef double				DOUBLE;				/**< Type is defined to DOUBLE */
typedef float				FLOAT;				/**< Type is defined to FLOAT */
#ifndef __TYPEDEF_H__
typedef int					BOOL;				/**< Type is defined to BOOL */
#endif	// __TYPEDEF_H__
typedef unsigned int		SIZE_T;				/**< Type is defined to SIZE_T */
typedef void				(*VP_CALLBACK)();	/**< Type is defined to Callback function pointer */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define MSKPTN(pos,len)				(~(~0U<<(len))<<(pos))							/**< mask bit pattern */
#define GETPRM(reg,pos,len)			(((reg)>>(pos))&(~(~0U<<(len))))				/**< get bit field */
#define SETPRM(reg,pos,len,val)		(reg=((reg)&~MSKPTN(pos,len))|((val)<<(pos)))	/**< set bit field */
#define FREE(x)						if(x) { free(x); x = NULL; }					/**< check not to free heap memory in double */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

#endif	// _DDIM_TYPEDEF_H_

/*@}*/
