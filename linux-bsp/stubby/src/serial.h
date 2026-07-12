/**
 * PL011 UART Driver
 *
 * Copyright (C) 2014 FUJITSU MICROELECTRONICS SOLUTIONS LIMITED. All rights reserved.
 *
 * Licensed under Stubby's GPL2.1 license
 */

#ifndef PL011_REG_H
#define PL011_REG_H

void uart_init(void);
void _putc(char c);
int _getc(void);

#endif /* ! PL011_REG_H */
