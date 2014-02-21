//==============================================================
//  mikeWeatherStation
//
//  sleep module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// sleep info
//=================================
#include <avr/sleep.h>
#include <avr/wdt.h>

//=====================================================================================
// prepare the watch dog timer and sleep
//=====================================================================================
void sleep(boolean start)
{
  // on entry 
  // true = start the clock
  // false = leave clock running
  //============================
  
  DIAGPRINTLN(F("s"));
  DIAGFLUSH();

  // set interrupt mode and interval
  //
  // WDP3 WDP2 WDP1 WDP0 timer   ~seconds
  //    0    0    0    0   16 mS
  //    0    0    0    1   32 mS
  //    0    0    1    0   64 mS
  //    0    0    1    1  128 mS 0.125
  //    0    1    0    0  256 mS 0.25
  //    0    1    0    1  512 mS 0.5
  //    0    1    1    0 1024 mS 1
  //    0    1    1    1 2048 mS 2
  //    1    0    0    0 4096 mS 4
  //    1    0    0    1 8192 mS 8
  //
  //============================================
  
  // clear various "reset" flags
  //============================
  MCUSR = 0;     
    
  if (start)
  {
    // starting the timer
    //===================
    
    // allow changes, disable reset
    //=============================
    WDTCSR = bit (WDCE) | bit (WDE);
    
    // wdt enabled and 2 second timer
    //===============================
    WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0);
  
    // call reset
    //===========
    wdt_reset();
  }

  // set sleep mode
  //===============
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 

  // disable ADC
  //============
  ADCSRA &= ~_BV(ADEN);
  
  // turn off brown-out enable in software
  //======================================
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 

  // sleep...
  //=========
  sleep_mode(); 





  // ... and wake-up
  //================
  
  
  // disable the timer
  //==================
  sleep_disable();

  // enable ADC
  //===========
  ADCSRA |= _BV(ADEN);

  DIAGPRINT('w');
  DIAGFLUSH();
}

//=====================================================================================
// watchDogTimer interrupt
//=====================================================================================
ISR(WDT_vect)
{
  // just set the watchdog flag
  //===========================
  wdtFlag = true;
}

