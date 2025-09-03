#include "LEDS.h"

volatile uint8_t blinkLeftCount;
volatile uint8_t blinkRightCount;
volatile uint8_t blinkLockCount;
volatile bool cycleHazard = false;
volatile bool cycleLock = false;
volatile bool isLocked = false;

void initLeds()
{
    DDRD |= 0b11000000; // LED1 = PD7, LED2 = PD6
}

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
void LCDsetup()
{
    lcd.begin(16, 2);
    lcd.print("Semnalizare:");
    lcd.setCursor(0, 1);
    lcd.print("OFF         ");
}

float getinterval()
{
    uint16_t adcVal = adc_read(0);
    uint16_t minInterval = 100;
    uint16_t maxInterval = 1000;

    uint16_t interval = minInterval + ((uint32_t)(maxInterval - minInterval) * adcVal) / 1023;

    return interval; 
}



ModeState Calc_Mode()
{
    // hazard state
    static bool hazardActive = false, lockActive = false;
    // starea anterioara a butoanelor (edge detect)
    static bool btnLeftPrev = false, btnRightPrev = false, btnHazardPrev = false, btnLockPrev = false;
    // este butonul apasat ?
    static bool btnLeftHeld = false, btnRightHeld = false;
    // flag pentru prevenire retrigger la long press
    static bool leftLongPressHandled = false, longPressHandled2 = false;
    // timestamp buton pentru cand se aprinde prima oara
    static unsigned long pressStartLeft = 0, pressStartRight = 0, pressStartHazzard = 0, pressStartLock = 0;
    // stateul curent
    static ModeState currentMode = MODE_OFF;
    static ModeState lockMode = MODE_OFF;

    bool btnLeft = getButtonState(3);
    bool btnRight = getButtonState(4);
    bool btnHazard = getButtonState(2);
    bool btnLock = getButtonState(5);

    unsigned long now = get_systime();
    unsigned long duration;
    
    // LOCK MODE

    if (btnLock && !btnLockPrev) {
        pressStartLock = now;
    }

    if (!btnLock && btnLockPrev) {
        duration = now - pressStartLock;

        if (!isLocked && duration > 800) {
            // Go to LOCK mode
            isLocked = true;
            lockMode = MODE_LOCK;
            blinkLockCount = 1;
        } 
        else if (isLocked && duration > 100 && duration < 800) {
            // Go to DOUBLE LOCK mode
            lockMode = MODE_DOUBLELOCK;
            blinkLockCount = 3;
        } 
        else if (isLocked && duration > 800) {
            // Go to UNLOCK (animation)
            isLocked = false;
            lockMode = MODE_UNLOCK_ANIM;
            blinkLockCount = 2;
        }
    }

    btnLockPrev = btnLock;

    if (lockMode == MODE_LOCK && (blinkLockCount > 0) && !cycleHazard) return MODE_LOCK;
    if (lockMode == MODE_DOUBLELOCK && (blinkLockCount > 0) && !cycleHazard) return MODE_DOUBLELOCK;
    if (lockMode == MODE_UNLOCK_ANIM && (blinkLockCount > 0) && !cycleHazard) return MODE_UNLOCK_ANIM;
    

    // HAZARD BUTTON (Toggle)
    if (btnHazard && !btnHazardPrev) {
        pressStartHazzard = now;
    }

    if (!btnHazard && btnHazardPrev) {
        duration = now - pressStartHazzard;
        if (duration > 200) {
            hazardActive = !hazardActive;
        }
    }

    btnHazardPrev = btnHazard;

    // hazard activ rezulta highest priority
    if (hazardActive || cycleHazard) {
        blinkLeftCount = 0;
        blinkRightCount = 0;
        return MODE_HAZARD;
    } 

    // LEFT BUTTON
    if (btnLeft && !btnLeftPrev) {
        pressStartLeft = now;
        btnLeftHeld = true;
        leftLongPressHandled = false;
    }
    if (btnLeft && btnLeftPrev && !leftLongPressHandled && (now - pressStartLeft > 500)) {
        leftLongPressHandled = true;
        currentMode = MODE_LEFT_HOLD;
    }
    if (!btnLeft && btnLeftPrev && btnLeftHeld) {
        btnLeftHeld = false;
        if (!leftLongPressHandled) {
            blinkLeftCount = 3;
            blinkRightCount = 0;
            currentMode = MODE_LEFT_3BLINK;
        } else if (currentMode == MODE_LEFT_HOLD) {
            currentMode = MODE_OFF;
        }
    }

    // RIGHT BUTTON
    if (btnRight && !btnRightPrev) {
        pressStartRight = now;
        btnRightHeld = true;
        longPressHandled2 = false;
    }
    if (btnRight && btnRightPrev && !longPressHandled2 && (now - pressStartRight > 500)) {
        longPressHandled2 = true;
        currentMode = MODE_RIGHT_HOLD;
    }
    if (!btnRight && btnRightPrev && btnRightHeld) {
        btnRightHeld = false;
        if (!longPressHandled2) {
            blinkRightCount = 3;
            currentMode = MODE_RIGHT_3BLINK;
        } else if (currentMode == MODE_RIGHT_HOLD) {
            currentMode = MODE_OFF;
        }
    }

    if (currentMode == MODE_LEFT_3BLINK && blinkLeftCount == 0) {
        currentMode = MODE_OFF;
    }
    if (currentMode == MODE_RIGHT_3BLINK && blinkRightCount == 0) {
        currentMode = MODE_OFF;
    }

    btnLeftPrev = btnLeft;
    btnRightPrev = btnRight;

    return currentMode;
}

void LCD_Message(ModeState mode) {
    static ModeState lastDisplayMode = MODE_OFF;

    if(mode == lastDisplayMode) {
        return;
    }
    lcd.setCursor(0,1);

    switch(mode) {
        case MODE_LOCK:
            lcd.print("LOCK        ");
            break;
        case MODE_DOUBLELOCK:
            lcd.print("DOUBLE LOCK ");
            break;
        case MODE_UNLOCK_ANIM:
            lcd.print("UNLOCK      ");
            break;
        case MODE_HAZARD:
            lcd.print("HAZZARD     ");
            break;
        case MODE_RIGHT_HOLD:
            lcd.print("NORMAL RIGHT");
            break;
        case MODE_LEFT_HOLD:
            lcd.print("NORMAL LEFT ");
            break;
        case MODE_RIGHT_3BLINK:
            lcd.print("3BLINK RIGHT");
            break;
        case MODE_LEFT_3BLINK:
            lcd.print("3BLINK LEFT ");
            break;
        case MODE_OFF:
        default:
            lcd.print("OFF         ");
            break;
    }

    lastDisplayMode = mode;
}

ModeState Trigger_Lamp_Req(ModeState mode) {
    static unsigned long ts_led_left = 0;
    static unsigned long ts_led_right = 0;
    static unsigned long ts_led_hazard = 0;
    static unsigned long ts_led_lock = 0;
    static bool leftState = false;
    static bool rightState = false;
    static bool hazardState = false;
    static bool lockState = false;
    static bool delayDone = false;

    unsigned long now = get_systime();
    uint16_t interval = getinterval();

    // aici se face resetarea variabilelor pt alte moduri (for safety)
    switch (mode) {
        case MODE_LOCK:
        case MODE_DOUBLELOCK:
        case MODE_UNLOCK_ANIM:
            leftState = false;
            rightState = false;
            hazardState = false;
            ts_led_left = now;
            ts_led_right = now;   
            break;
        case MODE_HAZARD:
            leftState = false;
            rightState = false;
            ts_led_left = now;
            ts_led_right = now;
            break;
        case MODE_LEFT_HOLD:
        case MODE_LEFT_3BLINK:
            clrbit(PORTD,PD6); 
            rightState = false;
            hazardState = false;
            break;
        case MODE_RIGHT_HOLD:
        case MODE_RIGHT_3BLINK:
            clrbit(PORTD,PD7);
            leftState = false; 
            hazardState = false;
            break;
        case MODE_OFF:
        default:
            clrbit(PORTD,PD6);
            clrbit(PORTD,PD7); 
            break;
    }

    // switch pt logica LEDurilor
    switch (mode) {
        case MODE_LOCK:
        case MODE_DOUBLELOCK:
        case MODE_UNLOCK_ANIM:
            if(!delayDone) {
                clrbit(PORTD,PD7);
                clrbit(PORTD,PD6);
                ts_led_lock = now + 800;
                delayDone = true;
            }
            if(blinkLockCount > 0 && now >= ts_led_lock) {
                ts_led_lock = now;
                lockState = !lockState;
                if(lockState) {
                    setbit(PORTD,PD7);
                    setbit(PORTD,PD6);
                    ts_led_lock = now + 200;
                } else {
                    clrbit(PORTD,PD7);
                    clrbit(PORTD,PD6);
                    ts_led_lock = now + 800;
                    blinkLockCount--;
                }
                if (blinkLockCount == 0) {
                    delayDone = false;
                }
            }
            break;
        case MODE_HAZARD:
            if (now - ts_led_hazard >= 500) {
                ts_led_hazard = now;
                hazardState = !hazardState;
                if (hazardState)
                {
                    clrbit(PORTD,PD6);
                    clrbit(PORTD,PD7);
                    cycleHazard = false;
                }
                else
                {
                    setbit(PORTD,PD6);
                    setbit(PORTD,PD7);
                    cycleHazard = true;
                }
            }
            break;

        case MODE_LEFT_HOLD:
            if (now - ts_led_left >= interval) {
                ts_led_left = now;
                leftState = !leftState;
                if (leftState)
                    setbit(PORTD,PD7);
                else
                    clrbit(PORTD,PD7);
            }
            break;

        case MODE_RIGHT_HOLD:
            if (now - ts_led_right >= interval) {
                ts_led_right = now;
                rightState = !rightState;
                if (rightState)
                    setbit(PORTD,PD6);
                else
                    clrbit(PORTD,PD6);
            }
            break;

        case MODE_LEFT_3BLINK:
            if (blinkLeftCount > 0 && now >= ts_led_left) {
                leftState = !leftState;
                if (leftState) {
                    setbit(PORTD,PD7);
                    ts_led_left = now + 300;
                } else {
                    clrbit(PORTD,PD7);
                    ts_led_left = now + 700;
                    blinkLeftCount--;
                }
            }
            break;

        case MODE_RIGHT_3BLINK:
            if (blinkRightCount > 0 && now >= ts_led_right) {
                rightState = !rightState;
                if (rightState) {
                    setbit(PORTD,PD6);
                    ts_led_right = now + 300;
                } else {
                    clrbit(PORTD,PD6);
                    ts_led_right = now + 700;
                    blinkRightCount--;
                }
            }
            break;

        case MODE_OFF:
        default:
            clrbit(PORTD,PD6);
            clrbit(PORTD,PD7);
            break;
    }

    return mode;
}


void L_Ctrl_Main() {
    ModeState currentMode = Calc_Mode();
    currentMode = Trigger_Lamp_Req(currentMode);
    LCD_Message(currentMode);
}