/**
 *
 * Cubieboard2 LED routines
 *
 * aLab inc, Watanabe-141121
 */
#include <stdint.h>
#ifndef Asm
#define Asm		__asm__ volatile
#endif

#define PIO_BASE_ADDR	(0x01C20800)
#define PIO_PH_DATAR	(PIO_BASE_ADDR + 0x010C)
#define LED_BLUE	(1 << 21)
#define LED_GREEN	(1 << 20)

void led_blue_on( void ) {
#if 0
	Asm("push	{r0-r1}\n\t"
	    "ldr	r1, =%0\n\t"
	    "ldr	r0, [r1]\n\t"
	    "orr	r0, r0, %1\n\t"
	    "str	r0, [r1]\n\t"
	    "pop	{r0-r1}\n\t"
	    "bx	lr\n\t"
	    ::"n"(PIO_PH_DATAR), "n"(LED_BLUE):"r0", "r1");
#endif
}

void led_blue_off( void ) {
#if 0
  Asm("push	{r0-r1}\n\t"
      "ldr	r1, =%0\n\t"
      "ldr	r0, [r1]\n\t"
      "bic	r0, r0, %1\n\t"
      "str	r0, [r1]\n\t"
      "pop	{r0-r1}\n\t"
      "bx	lr\n\t"
      ::"n"(PIO_PH_DATAR), "n"(LED_BLUE):"r0", "r1");
#endif
}

void led_blue_toggle( void ) {
#if 0
  Asm("push	{r0-r2}\n\t"
      "ldr	r1, =%0\n\t"
      "ldr	r0, [r1]\n\t"
      "ands	r2, r0, %1\n\t"
      "bicne	r0, r0, %1\n\t"
      "orreq	r0, r0, %1\n\t"
      "str	r0, [r1]\n\t"
      "pop	{r0-r2}\n\t"
      "bx	lr\n\t"
      ::"n"(PIO_PH_DATAR), "n"(LED_BLUE):"r0", "r1", "r2");
#endif
}

void led_green_on( void ) {
#if 0
  Asm("push	{r0-r1}\n\t"
      "ldr	r1, =%0\n\t"
      "ldr	r0, [r1]\n\t"
      "orr	r0, r0, %1\n\t"
      "str	r0, [r1]\n\t"
      "pop	{r0-r1}\n\t"
      "bx	lr\n\t"
      ::"n"(PIO_PH_DATAR), "n"(LED_GREEN):"r0", "r1");
#endif
}

void led_green_off( void ) {
#if 0
  Asm("push	{r0-r1}\n\t"
      "ldr	r1, =%0\n\t"
      "ldr	r0, [r1]\n\t"
      "bic	r0, r0, %1\n\t"
      "str	r0, [r1]\n\t"
      "pop	{r0-r1}\n\t"
      "bx	lr\n\t"
      ::"n"(PIO_PH_DATAR), "n"(LED_GREEN):"r0", "r1");
#endif
}

void led_green_toggle( void ) {
#if 0
  Asm("push	{r0-r2}\n\t"
      "ldr	r1, =%0\n\t"
      "ldr	r0, [r1]\n\t"
      "ands	r2, r0, %1\n\t"
      "bicne	r0, r0, %1\n\t"
      "orreq	r0, r0, %1\n\t"
      "str	r0, [r1]\n\t"
      "pop	{r0-r2}\n\t"
      "bx	lr\n\t"
      ::"n"(PIO_PH_DATAR), "n"(LED_GREEN):"r0", "r1", "r2");
#endif
}
