#include <avr/io.h>
#include "timer.h"

volatile unsigned long ms = 0;

void timer1_init(void) {
  
  cli();
  TCCR1A = 0b10000000;
  TCCR1B = 0b00001100; //CTC MODE, p = 256
  OCR1A = 625;
  TIMSK1 |= (1 << OCIE1A);
  sei();

}

unsigned long get_systime()
{
  unsigned long temp_ms;
  cli();
  temp_ms = ms;
  sei();
  return temp_ms;
}  

ISR(TIMER1_COMPA_vect)
{
  ms += 10;
}
