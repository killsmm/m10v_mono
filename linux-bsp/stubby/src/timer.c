/*
 * u-boot/arch/arm/cpu/armv7/mb8ac0300/timer.c
 *
 * Copyright (C) 2011-2012 FUJITSU SEMICONDUCTOR LIMITED
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "io.h"
#include "config.h"
#include "asm/utils.h"

/* Global Timer Register */
#define TIMER_REG_LOAD (CONFIG_SYS_TIMERBASE + 0x00)
#define TIMER_REG_VALUE (CONFIG_SYS_TIMERBASE + 0x04)
#define TIMER_REG_CONTROL (CONFIG_SYS_TIMERBASE + 0x08)


#define TIMER_LOAD_VAL 0xffffffff
#define TIMER_FREQ (CONFIG_TIMER_CLK / 16) /* Peripheral clock is divided by 16 */


//unsigned long timestamp;
//unsigned long lastdec;
#define LAST_DEC_ADDR (0xA0005000)
#define TIMESTAMP_ADDR (0xA0005100)

/* nothing really to do with interrupts, just starts up a counter. */
int timer_init(void)
{
	writel(TIMER_LOAD_VAL, TIMER_REG_LOAD);  /* decrement counter */

	writel(0x86, TIMER_REG_CONTROL);

	/* bit0: OnShot -> 0 = wrapping mode (default) */
	/* bit1: TimerSize -> 1 = 32-bit counter. */
	/* bit2-3: TimerPre -> 01 = 4 stages of prescale, clock is divided by 16 */
	/* bit4: Reserved */
	/* bit5: IntEnable -> 0 = Timer module Interrupt disabled */
	/* bit6: TimerMode -> 0 = Timer module is in free-running mode (default) */
	/* bit7: TimerEn -> 1 = Timer module enabled. */

	reset_timer_masked();

	return 0;
}

/*
 * timer without interrupts
 */
unsigned long get_ticks(void)
{
	unsigned int now = readl(TIMER_REG_VALUE);
	unsigned int lastdec = *((ulong *)LAST_DEC_ADDR);
	unsigned int timestamp = *((ulong *)TIMESTAMP_ADDR);

	if (now <= lastdec) { /* normal mode (non roll) */
	/* move stamp forward with absolut diff ticks */
		timestamp += (lastdec - now);
	}
	else {/* we have rollover of incrementer */
		timestamp += lastdec + TIMER_LOAD_VAL - now;
	}
	*((ulong *)TIMESTAMP_ADDR) = timestamp;

	lastdec = now;
	*((ulong *)LAST_DEC_ADDR) = lastdec;
	return timestamp;
}

void reset_timer_masked(void)
{
	/* reset time */
	//lastdec = readl(TIMER_REG_VALUE);
	//timestamp = 0;

	*((ulong *)LAST_DEC_ADDR) = readl(TIMER_REG_VALUE);
	*((ulong *)TIMESTAMP_ADDR) = 0;
}

void __udelay(unsigned long usec)
{
	unsigned int wait = 
	while (get_ticks() < tmp) /* loop till event */
		;
}

void reset_timer(void)
{
	reset_timer_masked();
}

unsigned int get_timer(unsigned long base)
{
	return get_timer_masked() - base;
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return TIMER_FREQ;
}
