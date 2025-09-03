#include<avr/io.h>
#include"ADC.h"

void adc_init(void) {

      //ADMUX=0b01000000;
      ADCSRA=0b10000111;
      ADCSRB=0b00000000;
}

uint16_t adc_read(uint8_t channel) {
    ADMUX=0b01000000;
    ADCSRA |= 0b01000000; // start conversie
    while (ADCSRA & 0b01000000); // așteaptă să termine
    return ADC;
}

uint16_t adc_lightread(uint8_t channel){
    ADMUX   = 0b01000001;
    ADCSRA |= 0b01000000;
    while (ADCSRA & 0b01000000);
    return ADC;
}




