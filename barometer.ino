//==============================================================
//  mikeWeatherStation
//
//  barometer module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================================
// barometer
//
// uses SCL/SDA pins
//
// define BMP085 or BMP180 to select barometer type
//=================================================

// #define BMP085 1
#define BMP180 1

#include <Wire.h>

#ifdef BMP085
#include <ADAFRUIT_BMP085.h>
Adafruit_BMP085 barometer;
#endif

#ifdef BMP180
#include <SFE_BMP180.h>
SFE_BMP180 pressure;
#endif

//=====================================================================================
// routine to get pressure from barometer
//=====================================================================================
void barometerGetPressure(void)
{
#ifdef BMP085
  float temp;
  long pressure;
#endif

#ifdef BMP180
  uint8_t status;
  double T, P;
#endif


#ifdef BMP085
  // get the temperature
  //====================
  temp = barometer.readTemperature();
  dataRecord.temperatureP = temp * 10;

  // get the pressure
  //=================
  pressure = barometer.readPressure();
  dataRecord.pressure = pressure / 100;
#endif

#ifdef BMP180
  // start the process
  //==================
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete
    //=====================================
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T
    // Function returns 1 if successful, 0 if failure
    //======================================================
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait)
      // If request is successful, the number of ms to wait is returned
      // If request is unsuccessful, 0 is returned
      //===================================================================================
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete
        //=====================================
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P
        // Note also that the function requires the previous temperature measurement (T)
        // Function returns 1 if successful, 0 if failure
        //==============================================================================
        status = pressure.getPressure(P, T);
        if (status != 0)
        {}
        else 
          DIAGPRINTLN(F("error retrieving pressure measurement"));
      }
      else 
        DIAGPRINTLN(F("error starting pressure measurement"));
    }
    else DIAGPRINTLN(F("error retrieving temperature measurement"));
  }
  else 
    DIAGPRINTLN(F("error starting temperature measurement"));

  dataRecord.temperatureP = T * 10;
  dataRecord.pressure = P;  
  
  DIAGPRINT(F("[BMP180] Pressure(mBar): "));
  DIAGPRINT(P, 1);
  DIAGPRINT(F("    Temperature(C): "));
  DIAGPRINTLN(T, 1);
#endif
}

//=====================================================================================
// routine to initialise barometer sensor
//=====================================================================================
void barometerInit(void)
{
#ifdef BMP085
  barometer.begin();
  DIAGPRINT("b");
#endif

#ifdef BMP180
  if (pressure.begin())
    DIAGPRINT("b");
  else
    DIAGPRINTLN(F("BMP180 init failed"));
#endif
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


