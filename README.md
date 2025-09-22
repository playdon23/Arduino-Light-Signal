# 🚗 AVR Automotive Signal System

![AVR](https://img.shields.io/badge/AVR-Microcontroller-003366)
![C++](https://img.shields.io/badge/C++-Embedded-00599C)
![Status](https://img.shields.io/badge/Status-Stable-success)

**Professional Automotive Signaling System with Intelligent Controls**

---

## 📋 Project Overview

This project implements a comprehensive automotive signal system using an AVR microcontroller.

The system controls turn signals, hazard lights, and door lock/unlock animations with an LCD display interface.

It demonstrates real-time embedded programming with:

- Button input handling
- ADC reading
- Timer management
- LED control

---

## 🏗️ System Architecture

### Project Structure
```text
automotive-signal-system/
├── ADC.[cpp/h]       - Analog-to-Digital Converter interface
├── button.[cpp/h]    - Button input handling
├── defs.h            - Common definitions and macros
├── LEDS.[cpp/h]      - LED control and mode management (main logic)
├── Timer.[cpp/h]     - System timer and timekeeping
└── main.cpp          - Main application loop

Hardware Requirements
 Component	Specification	Purpose
 Microcontroller	AVR ATmega series	Main processing unit
 Buttons	PC2, PC3, PC4, PC5	Mode selection & control
 LEDs	PD6, PD7	Left/Right turn indicators
 Potentiometer	ADC0	Blink rate adjustment
 LCD	16x2	Status display
 Clock	16MHz	System timing
 Features
 Signal Modes
 OFF: All signals off

Left/Right 3-Blink: Three quick blinks for lane change
Left/Right Hold: Continuous blinking until cancelled
Hazard: Both LEDs blinking simultaneously
Lock/Unlock Animations: Visual feedback for security system
Smart Input Handling
Software debouncing

Edge detection for button press/release
Long-press support
Priority system (hazard overrides other modes)
User Interface
Real-time LCD display
Adjustable blink rate via potentiometer (100–1000ms)
LED visual feedback for all operations

File	Description
ADC.[cpp/h]	Handles ADC reading, reads potentiometer values, functions: adc_init(), adc_read(), adc_lightread()
button.[cpp/h]	Handles button inputs with debouncing, functions: initButton(), getButtonState()
Timer.[cpp/h]	Implements Timer1 in CTC mode, functions: timer1_init(), get_systime()
LEDS.[cpp/h]	Main logic: LED control, mode management, LCD updates, functions: Calc_Mode(), Trigger_Lamp_Req(), LCD_Message(), L_Ctrl_Main()
defs.h	Common macros (setbit, clrbit, testbit) and configuration constants
main.cpp	Main control loop

Mode State Machine

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

Configuration

Timer Setup
Timer1: CTC mode, prescaler 256
Interrupt: 10ms intervals
Resolution: 10ms system time ticks

ADC Configuration
 Reference: AVcc with external capacitor
 Prescaler: 128 (125kHz ADC clock @ 16MHz)
 Channels: Channel 0 for potentiometer

Pin Assignments

 Component	Pin/Channel	Function 
 LED Right	PD6	Right turn signal
 LED Left	PD7	Left turn signal
 Button 2	PC2	Hazard toggle
 Button 3	PC3	Left signal
 Button 4	PC4	Right signal
 Button 5	PC5	Lock/Unlock
 Potentiometer	ADC0	Blink rate adjustment
 LCD	2,3,4,5,11,12	Display status
 Usage

Initialization
 
 adc_init();
 initButton();
 timer1_init();
 initLeds();
 LCDsetup();
 Main Loop

while(1) {
    L_Ctrl_Main();  // Main control function
    // Other tasks...
}

Button Functions:
-Button 2 (PC2) - Hazard mode toggle
-Button 3 (PC3) - Left turn signals
-Button 4 (PC4) - Right turn signals
-Button 5 (PC5) - Lock/unlock control

Customization
Adjusting Blink Rates

uint16_t minInterval = 100;  // Minimum blink interval (ms)
uint16_t maxInterval = 1000; // Maximum blink interval (ms)

Adding New Modes
Extend ModeState enum in LEDS.h

Update Calc_Mode()

Implement LED control in Trigger_Lamp_Req()

Add LCD message in LCD_Message()

Technical Details:
-System timer: 10ms resolution
-Blink intervals: 100–1000ms adjustable
-Button long-press detection: 500ms threshold
-Efficient ISR for timekeeping

Future Enhancements
Support additional LED channels

Configurable button mappings

EEPROM storage for user preferences

CAN bus integration for vehicle communication
