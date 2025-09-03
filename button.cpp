#include "button.h"
#include<avr/io.h>
#include<util/delay.h>
#include <stdbool.h>  

void initButton(void) {
     clrbit(DDRC,PC2);
     clrbit(DDRC,PC3);
     clrbit(DDRC,PC4);
} 

bool getButtonState(uint8_t buttonIndex) {
    // return (PINC & (1 << buttonIndex));
    return testbit(PINC,buttonIndex);
}