/******************************************************************************/
/**
 *  @file   sampl_code_userland.h
 *  @brief  Userland Definition
 */
/*
 *  Copyright 2015 Socionext Inc.
 ******************************************************************************/
#ifndef __SAMPLE_CODE_USERLAND_H
#define __SAMPLE_CODE_USERLAND_H


/******************************************************************** 
 *  Type define
 ********************************************************************/
/* standard type */
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed int          INT32;
typedef signed long long    INT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;

/* type definition conversion between cpu_linux-cpu_rtos */
typedef void                VOID;
typedef INT8                CHAR;
typedef INT16               SHORT;
typedef INT32               LONG;
typedef INT64               LLONG;
typedef UINT8               UCHAR;
typedef UINT16              USHORT;
typedef UINT32              ULONG;
typedef UINT64              ULLONG;

/******************************************************************** 
 *  Common define definition
 ********************************************************************/


/******************************************************************** 
 *  Debug print
 ********************************************************************/
/*  Trace log output macro      */
#define M_PRINT_TRACE( STRING, ... ) printf( "%s:%s()[%d]:-- TRACE  "STRING"\n", __FILE__,__func__, __LINE__, ## __VA_ARGS__ )
/*  Error log output macro      */
#define M_PRINT_ERROR( STRING, ... ) printf( "%s:%s()[%d]:** ERROR  "STRING"\n", __FILE__,__func__, __LINE__, ## __VA_ARGS__ )
/*  Debug log output macro      */
#define M_PRINT_DEBUG( STRING, ... ) printf( "%s:%s()[%d]:=> DEBUG  "STRING"\n", __FILE__,__func__, __LINE__, ## __VA_ARGS__ )

#endif	/* __SAMPLE_CODE_USERLAND_H */
