/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan
 
     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.
 
     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.
 
     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_w_clib_c.h] - C Standard Library Alternatives.
 ****************************************************************************/
#ifndef __PF_W_CLIB_C_H__
#define __PF_W_CLIB_C_H__

#include "prfile2/pf_types.h"
#include "prfile2/pf_w_clib.h"


/*---------------------------------------------------------------------------
 pf_w_strlen - calculate the length of a string

 [Synopsis]
    PF_SIZE_T   pf_w_strlen(const PF_WCHAR *s);

 [Description]
    The pf_w_strlen() function returns the number of characters in 's'.

 [Return Value]
    Returns the number of characters in 's'.

 ----------------------------------------------------------------------------*/
PF_SIZE_T pf_w_strlen(const PF_WCHAR* s)
{
    const PF_WCHAR*     t;

    for (t = s; *t != 0x0000; t++)
    {
    }
    return (PF_SIZE_T)(((PF_U_CHAR*)t - (PF_U_CHAR*)s) >> 1);
}


/*---------------------------------------------------------------------------
 pf_w_strcpy - copy a string

 [Synopsis]
    PF_WCHAR*   pf_w_strcpy(PF_WCHAR *dst, const PF_WCHAR *src);

 [Description]
    The pf_strcpy() function copies the string pointed to by 'src'
    (including the terminating `\0' character) to the array pointed to by
    'dst'.  The strings may not overlap, and the destination string 'dst'
    must be large enough to receive the copy.

 [Return Value]
    Returns the value of 'dst'.

 ----------------------------------------------------------------------------*/
PF_WCHAR* pf_w_strcpy(PF_WCHAR* dst, const PF_WCHAR* src)
{
    PF_WCHAR*       d = dst;

    for (; (*d = *src) != 0x0000; src++, d++)
    {
    }
    return dst;
}


/*---------------------------------------------------------------------------
 pf_w_strncpy - copy a string

 [Synopsis]
    PF_WCHAR*   pf_w_strncpy(PF_WCHAR *dst, const PF_WCHAR *src, PF_SIZE_T length);

 [Description]
    The pf_w_strcpy() function copies the first 'length' bytes of 'src' 
    (including the terminating `\0' character) to the array pointed to by
    'dst'.  The strings may not overlap, and the destination string 'dst'
    must be large enough to receive the copy.

    In the case where the length of 'src' is less than 'length', the
    remainder of 'dst' will be padded with nulls.

 [Return Value]
    Returns the value of 'dst'.

 ----------------------------------------------------------------------------*/
PF_WCHAR* pf_w_strncpy(PF_WCHAR *dst, const PF_WCHAR *src, PF_SIZE_T length)
{
    PF_WCHAR*       d = dst;

    for (; length > 0; length--, src++, d++)
    {
        *d = *src;
        if (*src == 0x0000)
        {
            break;
        }
    }
    return dst;
}


/*---------------------------------------------------------------------------
 pf_w_strcmp - compare strings

 [Synopsis]
    PF_S_LONG pf_w_strcmp(const PF_WCHAR *s1, const PF_WCHAR *s2);

 [Description]
    The pf_w_strcmp() function compares the string pointed to by 's1' to
    the string pointed to by 's2'.

    The sign of a non-zero return value is determined by the sign of the
    difference between the values of the first pair of bytes (both
    interpreted as type unsigned char) that differ in the strings being
    compared.

 [Return Value]
    An integer, with the value given below: 

      < 0       The string pointed to by 's1' is less than the string
                pointed to by 's2'. 
        0       The string pointed to by 's1' is equal to the string
                pointed to by 's2'. 
      > 0       The string pointed to by 's1' is greater than the string
                pointed to by 's2'. 

 ----------------------------------------------------------------------------*/
PF_S_LONG pf_w_strcmp(const PF_WCHAR* s1, const PF_WCHAR* s2)
{
    const PF_WCHAR*   p1 = (const PF_WCHAR*)s1;
    const PF_WCHAR*   p2 = (const PF_WCHAR*)s2;

    while (*p1 != 0x0000 && *p2 != 0x0000 && *p1 == *p2)
    {
        p1++;
        p2++;
    }
    return (*p1 - *p2);
}


/*---------------------------------------------------------------------------
 pf_w_strncmp - compare fixed-size strings

 [Synopsis]
    PF_S_LONG pf_w_strncmp(const PF_WCHAR *s1, const PF_WCHAR *s2, PF_SIZE_T length);

 [Description]
    The pf_w_strncmp() function compares not more than 'length' bytes (bytes
    that follow a null byte are not compared) from the array pointed to
    by 's1' to the array pointed to by 's2'. 

    The sign of a non-zero return value is determined by the sign of the
    difference between the values of the first pair of bytes (both
    interpreted as type unsigned char) that differ in the strings being
    compared.

 [Return Value]
    An integer, with the value given below: 

      < 0       The string pointed to by 's1' is less than the string
                pointed to by 's2'. 
        0       The string pointed to by 's1' is equal to the string
                pointed to by 's2'. 
      > 0       The string pointed to by 's1' is greater than the string
                pointed to by 's2'. 

 ----------------------------------------------------------------------------*/
PF_S_LONG pf_w_strncmp(const PF_WCHAR* s1, const PF_WCHAR* s2, PF_SIZE_T length)
{
    const PF_WCHAR*   p1 = (const PF_WCHAR*)s1;
    const PF_WCHAR*   p2 = (const PF_WCHAR*)s2;

    while (length-- > 0)
    {
        if (*p1 == 0x0000 || *p2 == 0x0000 || *p1 != *p2)
        {
            return (*p1 - *p2);
        }
        p1++;
        p2++;
    }
    return 0;
}

/*---------------------------------------------------------------------------
 pf_w_strchr - Find a character in a string

 [Synopsis]
    PF_WCHAR*   pf_w_strchr(PF_WCHAR *dst, const PF_WCHAR *src);

 [Description]
    The pf_w_strcpy() function copies the first 'length' bytes of 'src'
    (including the terminating `\0' character) to the array pointed to by
    'dst'.  The strings may not overlap, and the destination string 'dst'
    must be large enough to receive the copy.

    In the case where the length of 'src' is less than 'length', the
    remainder of 'dst' will be padded with nulls.

 [Return Value]
    Returns the value of 'dst'.

 ----------------------------------------------------------------------------*/
PF_WCHAR* pf_w_strchr(const PF_WCHAR *s, PF_WCHAR c)
{
    for (; *s != '\0'; s++)
    {
        if (*s == (PF_WCHAR)c)
        {
            return (PF_WCHAR*)s;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------
 pf_w_strcat - concatenate strings

 [Synopsis]
    PF_WCHAR*   pf_w_strcat(PF_WCHAR* dst, const PF_WCHAR* src);

 [Description]
    The pf_strcat() function appends the 'src' string to the 'dst' string
    overwriting the '\0' character at the end of 'dst', and then adds a
    terminating '\0' character. The strings may not overlap, and the 'dst'
    string must have enough space for the result.

 [Return Value]
    Returns the value of 'dst'.

 ----------------------------------------------------------------------------*/
PF_WCHAR* pf_w_strcat(PF_WCHAR* dst, const PF_WCHAR* src)
{
    PF_WCHAR*       d = dst;

    while (*d != '\0')
    {
        d++;
    }
    while (*src != '\0')
    {
        *d++ = *src++;
    }
    *d = '\0';
    return dst;
}

#endif  /* __PF_W_CLIB_C_H__ */

