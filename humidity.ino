//==============================================================
//  mikeWeatherStation
//
//  humidity module
//
//  Mike McPherson
//  December 2013
//==============================================================

//============================================
// temp & humidity
//============================================

// uses SCL/SDA pins
//==================
#include <SHT2x.h>

//=====================================================================================
// routine to get humidity and temperature from humidity sensor
//=====================================================================================
void humidityGetHumidity(void)
{  
  float temp;

  temp = SHT2x.GetHumidity();
  dataRecord.humidity = temp * 10;
  
  temp = SHT2x.GetTemperature();
  dataRecord.temperatureH = temp * 10;

  DIAGPRINT(F("[ SHT21] Humidity(%): "));
  DIAGPRINT(dataRecord.humidity/ 10.0, 1);
  DIAGPRINT(F("    Temperature(C): "));
  DIAGPRINTLN(dataRecord.temperatureH / 10.0, 1);
}

//=====================================================================================
// routine to initialise humidity sensor
//=====================================================================================
void humidityInit(void)
{
  // initialise humidity
  //====================
  DIAGPRINT("h");
}

//=====================================================================================
// routine to show current Humidity from humidity sensor
//=====================================================================================
void humidityShow()
{
  float dewPoint;
  int cloudBase;
  float temp;
  float humidity;
        
  // calculate dew point
  //====================
  temp = dataRecord.temperatureH / 10.0;
  humidity = dataRecord.humidity / 10.0;
  dewPoint = temp - (100.0 - humidity) / 5.0;
  
  // calculate cloud base (to nearest 100')
  //==============================
  cloudBase = (temp - dewPoint) * 5;
  cloudBase = cloudBase * 100;

  XRFPRINT(F("      TempH: "));
  XRFPRINT(temp, 1);
  XRFPRINTLN(F(" C"));
  
  XRFPRINT(F("   Humidity: "));
  XRFPRINT(humidity, 1);
  XRFPRINTLN(F(" %"));
  
  XRFPRINT(F("  Dew point: "));
  XRFPRINT(dewPoint, 1);
  XRFPRINTLN(F(" C"));
  
  XRFPRINT(F("  Cloudbase: "));
  XRFPRINT(cloudBase);
  XRFPRINTLN(F(" ft est"));
}


