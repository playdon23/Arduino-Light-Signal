#include <avr/io.h>
#include <stdbool.h>
#include "defs.h"


// #define BUTTON1_PIN PB1  // 0b00000010
// #define BUTTON2_PIN PB2  // 0b00000100
// #define BUTTON3_PIN PB3  // 0b00001000

void initButton(void);

bool getButtonState(uint8_t buttonIndex);
