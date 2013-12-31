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
  windTicks = 0;
  windFlag = false;
  pinMode(WINDSPEEDPIN, INPUT_PULLUP);

  // Sodaq Moja INT1 => D3
  //======================
  attachInterrupt(1, handleWindTick, FALLING);          
  
  // ensure interrupts are enabled
  //==============================
  interrupts();

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
  
  static int  readings[]     = {    0,    74,    88,   110,   156,   215,   267,   349,   436,   533,   617,   669,   747,   809,   859,   918, 1024};
  static char points[][4]    = {"ESE", "ENE", "E  ", "SSE", "SE ", "SSW", "S  ", "NNE", "NE ", "WSW", "SW ", "NNW", "N  ", "WNW", "NW ", "W  "};
  static uint16_t bearings[] = { 1125,   675,   900,  1575,  1350,  2025,  1800,   225,   450,  2475,  2250,  3375,     0,  2925,  3150,  2700};
  int i;
  
  // read raw data
  //==============
  windDir = analogRead(WINDDIRPIN);
  
  DIAGPRINT(F("   Wind dir: "));

  // convert to direction
  //=====================
  for (i=0; i<sizeof(readings); i++)
  {
    if (windDir > readings[i] && windDir < readings[i+1])
    {
        dataRecord.wind_dir = bearings[i];
        DIAGPRINT(bearings[i]/10.0, 1);
        break;
    }
  }
  DIAGPRINT(F(" ("));
  DIAGPRINT(windDir);
  DIAGPRINTLN(F(")"));
  
  // convert windTicks to wind speed
  //================================
  cli();
  myTicks = windTicks;
  windTicks = 0;
  sei();
  
  dataRecord.wind_ticks = myTicks;
  
  windSpeed = myTicks * 0.6;
  
  DIAGPRINT(F(" Wind speed: "));
  DIAGPRINT(windSpeed, 1);
  DIAGPRINT(F(" ("));
  DIAGPRINT(myTicks);
  DIAGPRINTLN(F(")"));
}


