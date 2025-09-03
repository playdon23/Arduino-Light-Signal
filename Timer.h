#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

extern volatile unsigned long ms;

void timer1_init(void);
unsigned long get_systime();

#endif