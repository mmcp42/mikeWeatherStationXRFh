//==============================================================
//  mikeWeatherStation
//
//  barometer module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// barometer
//
// uses SCL/SDA pins
//=================================
//#include <Barometer.h>
#include <ADAFRUIT_BMP085.h>
#include <Wire.h>
Adafruit_BMP085 barometer;

//=====================================================================================
// routine to get pressure from barometer
//=====================================================================================
void barometerGetPressure(void)
{
  float temp;
  long pressure;

  // get the temperature
  //====================
  temp = barometer.readTemperature();
  dataRecord.temperatureP = temp * 10;

  // get the pressure
  //=================
  pressure = barometer.readPressure();
  dataRecord.pressure = pressure / 100;
}

//=====================================================================================
// routine to initialise barometer sensor
//=====================================================================================
void barometerInit(void)
{
  barometer.begin();
  DIAGPRINT("b");
}

//=====================================================================================
// routine to show current Temperature and Pressure
//=====================================================================================
void barometerShow(void)
{
  long hPa;
  float temp;

  hPa = dataRecord.pressure;
  temp = dataRecord.temperatureP / 10.0;
  
  XRFPRINT(F("      TempP: "));
  XRFPRINT(temp, 1);
  XRFPRINTLN(F(" C"));

  XRFPRINT(F("   Pressure: "));
  XRFPRINT(hPa);
  XRFPRINTLN(F(" hPa"));
}


