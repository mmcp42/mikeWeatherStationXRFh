//==============================================================
//  mikeWeatherStation
//
//  rain module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// rain
//=================================
#define RAINPIN 2

//=================================
// ticks from rain bucket
//=================================
int rainTicks;

//=================================
// flag to show interrupt was from rain
//=================================
boolean rainFlag;

//=====================================================================================
// interrupt handler for rain ticks
//=====================================================================================
void handleRainTick()
{
  // simply increment the counter
  //=============================
  rainTicks++;
  rainFlag = true;
}

//=====================================================================================
// routine to initialise rain sensor
//=====================================================================================
void rainInit(void)
{
  pinMode(RAINPIN, INPUT_PULLUP);

  // Sodaq Moja INT0 => D2
  //======================
  attachInterrupt(0, handleRainTick, FALLING);          
  
  // ensure interrupts are enabled
  //==============================
  interrupts();

  rainTicks = 0;
  rainFlag = false;

  DIAGPRINT("r");
}

//=====================================================================================
// routine to show current Rain settings
//=====================================================================================
void rainShow(void)
{
  float rain;
  int myTicks;
      
  // convert rainTicks to rain rate
  //===============================
  cli();
  myTicks = rainTicks;
  rainTicks = 0;
  sei();
  
  // convert to mm
  //==============
  rain = myTicks * 0.279;
  
  dataRecord.rain_ticks = myTicks;
  
  XRFPRINT(F("  rain rate: "));
  XRFPRINT(rain, 1);
  XRFPRINT(F(" ("));
  XRFPRINT(myTicks);
  XRFPRINTLN(F(")"));
}


