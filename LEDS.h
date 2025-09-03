#ifndef LEDS_H
#define LEDS_H

#include "button.h"
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "Timer.h"
#include "LEDS.h"
#include <LiquidCrystal.h>
#include "defs.h"

typedef enum {
    MODE_OFF,
    MODE_RIGHT_3BLINK,
    MODE_RIGHT_HOLD,
    MODE_LEFT_3BLINK,
    MODE_LEFT_HOLD,
    MODE_HAZARD,
    MODE_LOCK,
    MODE_DOUBLELOCK,
    MODE_UNLOCK_ANIM
} ModeState;

void initLeds();

ModeState Calc_Mode();
ModeState Trigger_Lamp_Req(ModeState mode);
void L_Ctrl_Main();

void LCDsetup();


float getinterval();

extern LiquidCrystal lcd;

#endif
