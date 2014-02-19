//==============================================================
//  mikeWeatherStation
//
//  wind module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// wind
//=================================
#define WINDDIRPIN A0
#define WINDSPEEDPIN 3

//=================================
// ticks from anemometer
//=================================
int windTicks;

// time of last wind calculation
//==============================
uint32_t lastWindTime;

//=================================
// flag to show interrupt was from wind
//=================================
boolean windFlag;

//=====================================================================================
// interrupt handler for wind ticks
//=====================================================================================
void handleWindTick()
{
  // simply increment the counter
  //=============================
  windTicks++;
  windFlag = true;
}

//=====================================================================================
// routine to initialise wind sensors
//=====================================================================================
void windInit(void)
{
  lastWindTime = getTime();
  pinMode(WINDSPEEDPIN, INPUT_PULLUP);

  // Sodaq Moja INT1 => D3
  //======================
  attachInterrupt(1, handleWindTick, FALLING);          
  
  // ensure interrupts are enabled
  //==============================
  interrupts();

  windTicks = 0;
  windFlag = false;

  DIAGPRINT("w");
}

//=====================================================================================
// routine to show current Wind settings
//=====================================================================================
void windShow(void)
{
  float windSpeed;
  int myTicks;
  int windDir;
  uint32_t windTime;
  
  static int  readings[]     = {    0,    74,    88,   110,   156,   215,   267,   349,   436,   533,   617,   669,   747,   809,   859,   918, 1024};
  //static char points[][4]    = {"ESE", "ENE", "E  ", "SSE", "SE ", "SSW", "S  ", "NNE", "NE ", "WSW", "SW ", "NNW", "N  ", "WNW", "NW ", "W  "};
  static uint16_t bearings[] = { 1125,   675,   900,  1575,  1350,  2025,  1800,   225,   450,  2475,  2250,  3375,     0,  2925,  3150,  2700};
  int i, j;
  
  // read raw data
  //==============
  windDir = analogRead(WINDDIRPIN);
  
  XRFPRINT(F("   Wind dir: "));

  // convert to direction
  //=====================
  j = sizeof(readings)/sizeof(readings[0]);
  for (i=0; i<j; i++)
  {
    if (windDir > readings[i] && windDir < readings[i+1])
    {
        dataRecord.wind_dir = bearings[i];
        XRFPRINT(bearings[i]/10.0, 1);
        break;
    }
  }
  XRFPRINT(F(" ("));
  XRFPRINT(windDir);
  XRFPRINTLN(F(")"));
  
  // convert windTicks to wind speed
  //================================
  cli();
  myTicks = windTicks;
  windTicks = 0;
  sei();
  
  // get time for that many clicks
  //==============================
  windTime = getTime() - lastWindTime;
  lastWindTime = getTime();
  
  dataRecord.wind_ticks = myTicks;
  
  // convert ticks to knots
  // conversion factor is 
  // 1 mph = 1600 ticks/hour
  // 1 mph = 26.6667 ticks/min
  // 1 mph = 0.4444 ticks/sec
  // we need to use actual time between calculations
  //================================================
  windSpeed = myTicks / 0.4444 / windTime;
  
  // 1 mph = 0.868976 kt
  //====================
  windSpeed = windSpeed * 0.868976;
  
  XRFPRINT(F(" Wind speed: "));
  XRFPRINT(windSpeed, 1);
  XRFPRINT(F(" ("));
  XRFPRINT(myTicks);
  XRFPRINTLN(F(")"));
}


