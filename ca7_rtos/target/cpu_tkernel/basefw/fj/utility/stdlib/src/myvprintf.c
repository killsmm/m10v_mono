/**
 * @file		myvprintf.c
 * @brief		clone of printf and vprintf functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "os_user_custom.h"
#include "dd_arm.h"
#include "bf_arm.h"
#include "uart.h"
#include "debug.h"

#define _isnumc(x) ( (x) >= '0' && (x) <= '9' )
#define _ctoi(x)   ( (x) -  '0' )

#define    ZERO_PADDING         (1<<1)
#define    ALTERNATIVE          (1<<2)
#define    THOUSAND_GROUP       (1<<3)
#define    CAPITAL_LETTER       (1<<4)
#define    WITH_SIGN_CHAR       (1<<5)
#define    LEFT_JUSTIFIED       (1<<6)

typedef enum INTEGER_t {
    LL =  2,    //Long Ling
    L  =  1,    //Long
    I  =  0,    //Int
    S  = -1,    //Short
    C  = -2     //Char
} INTEGER;

static UINT32 gMySprintf_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));
static ULONG printf_critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
static char *gpStrBuff;

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
int _EXFUN(__real_vprintf, (const char *, __VALIST) _ATTRIBUTE ((__format__ (__printf__, 1, 0))));
int _EXFUN(__real_printf, (const char *__restrict, ...) _ATTRIBUTE ((__format__ (__printf__, 1, 2))));


/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

int __wrap_printf(const char* format, ...)
{
	va_list		ap;
	int			retcd;

#ifdef CO_DISABLE_PRINT_IN_INTERRUPT
	if( OS_User_Sns_Ctx() ) {
		return 0;
	}
#endif	// CO_DISABLE_PRINT_IN_INTERRUPT

	va_start(ap, format);	/* pgr0039 */
	Dd_ARM_Critical_Section_Start( printf_critseq );
	retcd = __real_vprintf(format, ap);
	Dd_ARM_Critical_Section_End( printf_critseq );
	va_end(ap);

	return retcd;
}

int __wrap_vprintf(const char* format, __VALIST ap)
{
	int		retcd;

#ifdef CO_DISABLE_PRINT_IN_INTERRUPT
	if( OS_User_Sns_Ctx() ) {
		return 0;
	}
#endif	// CO_DISABLE_PRINT_IN_INTERRUPT

	Dd_ARM_Critical_Section_Start( printf_critseq );
	retcd = __real_vprintf(format, ap);
	Dd_ARM_Critical_Section_End( printf_critseq );

	return retcd;
}


static void put_integer(void (*__putc)(int), unsigned long long n, int radix, int length, char sign, int flags)
{
    static char *symbols_s = "0123456789abcdef";
    static char *symbols_c = "0123456789ABCDEF";
    char buf[80];
    int i = 0;
    int pad = ' ';
    char *symbols = symbols_s;
    
    if(flags & CAPITAL_LETTER) symbols = symbols_c;
    do {
        buf[i++] = symbols[n % radix];
        if( (flags & THOUSAND_GROUP) && (i%4)==3) buf[i++] = ',';
    } while (n /= radix);

    length -= i;

    if (!(flags & LEFT_JUSTIFIED)) {
        if(flags & ZERO_PADDING) pad = '0';
        while (length > 0) { length--; buf[i++] = pad;}
    }

    if (sign && radix == 10) buf[i++] = sign;
    if (flags & ALTERNATIVE)
    {
        if (radix == 8) buf[i++] = '0';
        else if (radix == 16)
        {
           buf[i++] = 'x';
           buf[i++] = '0';
        }
    }

    while (      i > 0 ) { __putc(buf[--i]);      }
    while ( length > 0 ) { length--; __putc(pad); }

}

static char * mystrchr(const char *s, int c)
{
    char ch = (char) c;
    while (*s) {
        if (*s == ch)
            return (char *) s;
        s++;
    }
    return NULL;
}

static void align_ap( va_list *p_ap, INTEGER type )
{
    INTEGER t; t = type;
    if(t >=  LL) t = LL;
    if(t <=   C) t = C;

	if( LL == t ) {
		__asm__ __volatile__ (
			"ldr r7, [%0]\n\t"
			"add r7, r7, #7\n\t"
			"bic r7, r7, #7\n\t"
			"str r7, [%0]\n\t"
			: "+r" (p_ap) :  : "r7" );
	}
}

static void update_ap( va_list *p_ap, INTEGER type )
{
	unsigned num;
    INTEGER t; t = type;
    if(t >=  LL) t = LL;
    if(t <=   C) t = C;

        switch(t)
        {
          case LL: num = 8; break;
          case  L: num = 4; break;
          case  I: num = 4; break;
          case  S: num = 4; break;
          case  C: num = 4; break;
          default: num = 4; break;
        }
	__asm__ __volatile__ (
		"ldr r7, [%0]\n\t"
		"add r7, %1\n\t"
		"str r7, [%0]\n\t"
		: "+r" (p_ap) : "r" (num) : "r7" );
}

static long long get_signed(va_list ap, INTEGER type)
{
    INTEGER t; t = type;
    if(t >=  LL) t = LL;
    if(t <=   C) t = C;

        switch(t)
        {
          case LL: return va_arg(ap, long long);         break;
          case  L: return va_arg(ap, long);              break;
          case  I: return va_arg(ap, int);               break;
          case  S: return (short) va_arg(ap, unsigned ); break;
          case  C: return (signed char) va_arg(ap, unsigned );  break;
        }

        return (signed char) va_arg(ap, unsigned );
}

static unsigned long long get_unsigned(va_list ap, INTEGER type)
{
    INTEGER t; t = type;
    if(t >=  LL) t = LL;
    if(t <=   C) t = C;

        switch(t)
        {
          case LL: return va_arg(ap, unsigned long long);         break;
          case  L: return va_arg(ap, unsigned long);              break;
          case  I: return va_arg(ap, unsigned int);               break;
          case  S: return (unsigned short) va_arg(ap, unsigned ); break;
          case  C: return (unsigned char) va_arg(ap, unsigned );  break;
        }

        return (unsigned char) va_arg(ap, unsigned );
}

void myvprintf_a(void (*__putc)(int), const char *fmt, va_list ap)
{

    for (;;) {

        unsigned long long ui;
        long long i;
        char *s = NULL;
//      double d = 0.0;
        int sign = 0;

        int flags = 0;
        int length = 0;
        int precision = 0;
        int tmp = 0;
        INTEGER int_type = 0;

        while (*fmt && *fmt != '%') __putc(*fmt++);
        if (*fmt == '\0') { va_end(ap); break; }
        fmt++;


        while (mystrchr("'-+ #0", *fmt)) {
            switch (*fmt++) {
                  case '\'': flags |= THOUSAND_GROUP;             break;
                  case  '-': flags |= LEFT_JUSTIFIED;             break;
                  case  '+': flags |= WITH_SIGN_CHAR; sign = '+'; break;
                  case  '#': flags |= ALTERNATIVE;                break;
                  case  '0': flags |= ZERO_PADDING;               break;
                  case  ' ': flags |= WITH_SIGN_CHAR; sign = ' '; break;
            }
        }

        if(*fmt == '*'){ length = va_arg(ap,int); fmt++; }
        else {  while( _isnumc(*fmt) ) length = (length*10)+_ctoi(*fmt++);  }


        if (*fmt == '.')
        {
            fmt++;
            if (*fmt == '*'){ fmt++; precision = va_arg(ap, int);}
            else { while (_isnumc(*fmt) ) precision = precision * 10 + _ctoi(*fmt++); }
        }

        while (mystrchr("hljzt", *fmt)) {
            switch (*fmt++) {
                  case 'h': int_type--;  break;
                  case 'l': int_type++;  break;
                  case 'j': /*intmax   : long      */
                  case 'z': /*size     : long      */
                  case 't': /*ptrdiff  : long      */
                            int_type=L;  break;
            }
        }


        switch (*fmt) {

            case 'd':
            case 'i':
        		align_ap( &ap, int_type );
                i = get_signed(ap, int_type);
        		update_ap( &ap, int_type );
                if (i < 0) { i = -i; sign = '-'; }
                put_integer(__putc, i, 10, length, sign, flags);
                break;

            case 'u':
        		align_ap( &ap, int_type );
                ui = get_unsigned(ap, int_type);
        		update_ap( &ap, int_type );
                put_integer(__putc,  ui, 10, length, sign, flags);
                break;

            case 'o':
        		align_ap( &ap, int_type );
                ui = get_unsigned(ap, int_type);
        		update_ap( &ap, int_type );
                put_integer(__putc,  ui, 8, length, sign, flags);
                break;

            case 'p':
                length = sizeof(long) * 2;
                int_type = L;
                sign = 0;
                flags = ZERO_PADDING | ALTERNATIVE;		/* pgr0007 */
            case 'X':
                flags |= CAPITAL_LETTER;				/* pgr0007 */
            case 'x':
        		align_ap( &ap, int_type );
                ui = get_unsigned(ap, int_type);
        		update_ap( &ap, int_type );
                put_integer(__putc, ui, 16, length, sign, flags);
                break;

            case 'c':
                i = get_signed(ap, C);
        		update_ap( &ap, C );
                __putc(i);
                break;

            case 's':
        		s = (char *)va_arg(ap, char *);
                if (s==NULL) s = "(null)";
                tmp = strlen(s);
                if (precision && precision < tmp)  tmp = precision;
                length = length - tmp;
                if (!(flags & LEFT_JUSTIFIED))
                {   while ( length > 0   ) { length--; __putc(' '); } }
                    while ( tmp--       )  { __putc(*s++);          }
                    while ( length-- > 0)  {__putc(' ');            }

                break;

        case '%':
            __putc('%');
            break;

        default:
            while (*fmt != '%') fmt--;
            break;
        }

        fmt++;
    }
}

void myputc(int c)
{
	CHAR str;
	str = (CHAR)c;
#ifdef D_COM_BUF_FOR_TERMINAL
	if( BF_Com_Buff_Is() ) {
		BF_Com_Buff_Put_Char( str );
	}
	else {
		BF_UART_Debug_Put_Char( str );
	}
#else	// D_COM_BUF_FOR_TERMINAL
	BF_UART_Debug_Put_Char( str );
#endif	// D_COM_BUF_FOR_TERMINAL
}

void myvprintf(const char *fmt, va_list ap)
{
#ifdef CO_DISABLE_PRINT_IN_INTERRUPT
	if( OS_User_Sns_Ctx() ) {
		return;
	}
#endif	// CO_DISABLE_PRINT_IN_INTERRUPT

	Dd_ARM_Critical_Section_Start( printf_critseq );
	myvprintf_a(myputc, fmt, ap);
	Dd_ARM_Critical_Section_End( printf_critseq );
}

void myprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);		/* pgr0039 */
	myvprintf(fmt, ap);
	va_end(ap);
}

void myputc_to_s(int c)
{
    *gpStrBuff = c;
	gpStrBuff++;
}

void myvsprintf(const char *fmt, va_list ap)
{
	myvprintf_a(myputc_to_s, fmt, ap);
}

void mysprintf(char *str, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);		/* pgr0039 */
	Dd_ARM_Critical_Section_Start( gMySprintf_Spin_Lock );
	gpStrBuff = str;
	myvsprintf(fmt, ap);
	Dd_ARM_Critical_Section_End( gMySprintf_Spin_Lock );
	va_end(ap);
}

//--------------------------

static void put_n_integer(void (*__putc)(int, int*), int *p_remain_size, unsigned long long n, int radix, int length, char sign, int flags)
{
    static char *symbols_s = "0123456789abcdef";
    static char *symbols_c = "0123456789ABCDEF";
    char buf[80];
    int i = 0;
    int pad = ' ';
    char *symbols = symbols_s;
    
    if(flags & CAPITAL_LETTER) symbols = symbols_c;
    do {
        buf[i++] = symbols[n % radix];
        if( (flags & THOUSAND_GROUP) && (i%4)==3) buf[i++] = ',';
    } while (n /= radix);

    length -= i;

    if (!(flags & LEFT_JUSTIFIED)) {
        if(flags & ZERO_PADDING) pad = '0';
        while (length > 0) { length--; buf[i++] = pad;}
    }

    if (sign && radix == 10) buf[i++] = sign;
    if (flags & ALTERNATIVE)
    {
        if (radix == 8) buf[i++] = '0';
        else if (radix == 16)
        {
           buf[i++] = 'x';
           buf[i++] = '0';
        }
    }

    while (      i > 0 ) {
    	__putc(buf[--i], p_remain_size);
		if( 1 >= *p_remain_size ) break;
    }
    while ( length > 0 ) {
    	length--;
    	__putc(pad, p_remain_size);
		if( 1 >= *p_remain_size ) break;
    }
}

void myputc_n_to_s(int c, int *p_size)
{
	if( 1 >= *p_size ) return;
	*p_size = (*p_size - 1);
    *gpStrBuff = c;
	gpStrBuff++;
}

int myvnprintf_a(void (*__putc)(int, int*), int size, const char *fmt, va_list ap)
{
	int remain_size = size;
	if( 0 == size ) {
		return 0;
	}

    for (;;) {

        unsigned long long ui;
        long long i;
        char *s = NULL;
        int sign = 0;

        int flags = 0;
        int length = 0;
        int precision = 0;
        int tmp = 0;
        INTEGER int_type = 0;

    	while (*fmt && *fmt != '%') {
    		__putc(*fmt++, &remain_size);
    		if( 1 >= remain_size ) break;
    	}
    	if( 1 >= remain_size ) break;
        if (*fmt == '\0') { va_end(ap); break; }
        fmt++;


        while (mystrchr("'-+ #0", *fmt)) {
            switch (*fmt++) {
                  case '\'': flags |= THOUSAND_GROUP;             break;
                  case  '-': flags |= LEFT_JUSTIFIED;             break;
                  case  '+': flags |= WITH_SIGN_CHAR; sign = '+'; break;
                  case  '#': flags |= ALTERNATIVE;                break;
                  case  '0': flags |= ZERO_PADDING;               break;
                  case  ' ': flags |= WITH_SIGN_CHAR; sign = ' '; break;
            }
        }

        if(*fmt == '*'){ length = va_arg(ap,int); fmt++; }
        else {  while( _isnumc(*fmt) ) length = (length*10)+_ctoi(*fmt++);  }


        if (*fmt == '.')
        {
            fmt++;
            if (*fmt == '*'){ fmt++; precision = va_arg(ap, int);}
            else { while (_isnumc(*fmt) ) precision = precision * 10 + _ctoi(*fmt++); }
        }

        while (mystrchr("hljzt", *fmt)) {
            switch (*fmt++) {
                  case 'h': int_type--;  break;
                  case 'l': int_type++;  break;
                  case 'j': /*intmax   : long      */
                  case 'z': /*size     : long      */
                  case 't': /*ptrdiff  : long      */
                            int_type=L;  break;
            }
        }


        switch (*fmt) {

            case 'd':
            case 'i':
        		align_ap( &ap, int_type );
                i = get_signed(ap, int_type);
        		update_ap( &ap, int_type );
                if (i < 0) { i = -i; sign = '-'; }
                put_n_integer(__putc, &remain_size, i, 10, length, sign, flags);
                break;

            case 'u':
        		align_ap( &ap, int_type );
                ui = get_unsigned(ap, int_type);
        		update_ap( &ap, int_type );
                put_n_integer(__putc, &remain_size, ui, 10, length, sign, flags);
                break;

            case 'o':
        		align_ap( &ap, int_type );
                ui = get_unsigned(ap, int_type);
        		update_ap( &ap, int_type );
                put_n_integer(__putc, &remain_size, ui, 8, length, sign, flags);
                break;

            case 'p':
                length = sizeof(long) * 2;
                int_type = L;
                sign = 0;
                flags = ZERO_PADDING | ALTERNATIVE;		/* pgr0007 */
            case 'X':
                flags |= CAPITAL_LETTER;				/* pgr0007 */
            case 'x':
        		align_ap( &ap, int_type );
                ui = get_unsigned(ap, int_type);
        		update_ap( &ap, int_type );
                put_n_integer(__putc, &remain_size, ui, 16, length, sign, flags);
                break;

            case 'c':
                i = get_signed(ap, C);
        		update_ap( &ap, C );
                __putc(i, &remain_size);
                break;

            case 's':
        		s = (char *)va_arg(ap, char *);
                if (s==NULL) s = "(null)";
                tmp = strlen(s);
                if (precision && precision < tmp)  tmp = precision;
                length = length - tmp;
                if (!(flags & LEFT_JUSTIFIED)) {
                	while ( length > 0   ) {
                		length--;
                		__putc(' ', &remain_size);
						if( 1 >= remain_size ) break;
                	}
					if( 1 >= remain_size ) break;
                }
                while ( tmp--       ) {
                	__putc(*s++, &remain_size);
					if( 1 >= remain_size ) break;
                }
				if( 1 >= remain_size ) break;
                while ( length-- > 0) {
                	__putc(' ', &remain_size);
					if( 1 >= remain_size ) break;
                }
                break;

        case '%':
            __putc('%', &remain_size);
            break;

        default:
            while (*fmt != '%') fmt--;
            break;
        }
		if( 1 >= remain_size ) break;

        fmt++;
    }
	{
		int size_tmp = 4;
		__putc('\0', &size_tmp);
	}
	if( 1 >= remain_size ) {
		return -1;
	}
	return( size - remain_size );
}

int myvnsprintf(int n, const char *fmt, va_list ap)
{
	return myvnprintf_a(myputc_n_to_s, n, fmt, ap);
}

int mysnprintf(char *str, int n, const char *fmt, ...)
{
	int ans;
	va_list ap;
	va_start(ap, fmt);		/* pgr0039 */
	Dd_ARM_Critical_Section_Start( gMySprintf_Spin_Lock );
	gpStrBuff = str;
	ans = myvnsprintf(n, fmt, ap);
	Dd_ARM_Critical_Section_End( gMySprintf_Spin_Lock );
	va_end(ap);
	return ans;
}

