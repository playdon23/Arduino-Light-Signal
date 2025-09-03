#include "button.h"
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "Timer.h"
#include <LiquidCrystal.h>
#include "LEDS.h"
#include "defs.h"

unsigned long ts_intr_ante = 0;
  
bool ok = false;

int main(void)
{
    initButton();
    initLeds();
    adc_init();
    timer1_init();
    LCDsetup();
    sei(); 

    while (1)
    {
        unsigned long ts_intr_now = get_systime();
        static unsigned long ts_intr_ante = 0;

        int16_t ADCVAL = adc_lightread(1);
              
        if (ts_intr_now - ts_intr_ante >= 10)
        {
            ts_intr_ante = ts_intr_now;
                
            if ( ADCVAL > 200)
            {
               ok = true;
            }

            if(ok)
            {
                L_Ctrl_Main();
            }
            else
            {
                lcd.setCursor(0, 1)
                lcd.print("Awaiting key")
            }
          //  L_Ctrl_Main();

        }
    }
}


