//==============================================================
//  mikeWeatherStation
//
//  clock (rtc) module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
//RTC
//=================================
#include <Sodaq_DS3231.h>
DateTime timeNow;

//=====================================================================================
// routine to initialise RTC (RealTime Clock)
//=====================================================================================
void clockInit()
{
  uint32_t newTs;
  
  // initialise the clock  
  //=====================
  rtc.begin();
  
#ifdef GPRS
  // sanity check time/date
  // oops clock seems to have stopped
  //=================================
  while (rtc.now().get() < 10000)
  {
    // attempt to get time from the server
    //====================================
    newTs = getEpochFromServer();
    if (newTs > 0)
    {
      // set the time
      //=============
      DIAGPRINTLN(newTs);
      rtc.setEpoch(newTs);      
      break;
    }
    // failed, could be flat battery
    // so wait 10 seconds
    // and try again
    //==============================
    groveOff();
    delay(10000);
    groveOn();
  }
#endif

  DIAGPRINT("c");
}

//=====================================================================================
// routine to show RTC temperature
//=====================================================================================
void rtcTempShow(void)
{
  float temp;

  temp = rtc.getTemperature();
  dataRecord.temperatureRTC = temp *10;
  temp = dataRecord.temperatureRTC / 10.0;
  DIAGPRINT(F("   RTC temp: "));
  DIAGPRINT(temp, 1);
  DIAGPRINTLN(F(" C"));
}

//=====================================================================================
// routine to get time
//=====================================================================================
uint32_t getTime(void)
{
  timeNow = rtc.now();
  return(timeNow.get());
}

//=====================================================================================
// set epoch
//=====================================================================================
void setEpoch(void)
{
  uint32_t time;
  char charCom;

  charCom = ' ';
  time = getNumber(&charCom);
  DIAGPRINT(F("      epoch: ")); DIAGPRINTLN(time);
  if (time > 0)
  {
    rtc.setEpoch(time);
    startTime = getTime();
    DIAGPRINT(F(" start time: "));
    timestampShow(false);
  }
}

//=====================================================================================
// routine to show time
//=====================================================================================
void timeShow(void)
{
  int hour;
  int minute;
  int second;

  // show date/time
  //===============
  hour = timeNow.hour();
  minute = timeNow.minute();
  second = timeNow.second();

  if (hour<10)
    DIAGPRINT("0");
  DIAGPRINT(hour, DEC);
  DIAGPRINT(':');
  if (minute<10)
    DIAGPRINT("0");
  DIAGPRINT(minute, DEC);
  DIAGPRINT(':');
  if (second<10)
    DIAGPRINT("0");
  DIAGPRINT(second, DEC);
  DIAGPRINT(' ');
}

//=====================================================================================
// routine to show timestamp and RTC temperature
//=====================================================================================
void timestampShow(bool fetchFirst)
{
  static char months[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  int day;
  int hour;
  int minute;
  int second;
    
  // show date/time
  //===============
  if (fetchFirst)
    getTime();
  day = timeNow.date();
  hour = timeNow.hour();
  minute = timeNow.minute();
  second = timeNow.second();
  
  if (day<10)
    DIAGPRINT("0");
  DIAGPRINT(day, DEC);
  DIAGPRINT('-');
  DIAGPRINT(months[timeNow.month()-1]);
  DIAGPRINT('-');
  DIAGPRINT(timeNow.year(), DEC);
  DIAGPRINT(' ');
  
  timeShow();
  DIAGPRINTLN();
}

void tsShow(void)
{
  DIAGPRINT(F("         ts: "));
  DIAGPRINTLN(timeNow.get());
}


