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
#include <Barometer.h>
#include <Wire.h>
Barometer barometer;

//=====================================================================================
// routine to get pressure from barometer
//=====================================================================================
void barometerGetPressure(void)
{
  float temp;
  long pressure;

  // get the temperature
  //====================
  temp = barometer.bmp085GetTemperature(barometer.bmp085ReadUT());
  dataRecord.temperatureP = temp * 10;

  // get the pressure
  //=================
  pressure = barometer.bmp085GetPressure(barometer.bmp085ReadUP());
  dataRecord.pressure = pressure / 100;
}

//=====================================================================================
// routine to initialise barometer sensor
//=====================================================================================
void barometerInit(void)
{
  barometer.init();
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
  
  DIAGPRINT(F("      TempP: "));
  DIAGPRINT(temp, 1);
  DIAGPRINTLN(F(" C"));

  DIAGPRINT(F("   Pressure: "));
  DIAGPRINT(hPa);
  DIAGPRINTLN(F(" hPa"));
}


