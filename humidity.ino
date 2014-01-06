//==============================================================
//  mikeWeatherStation
//
//  humidity module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// temp & humidity
//=================================
#include <DHT22.h>
#define DHTPIN     A3
DHT22 dht(DHTPIN);

//=====================================================================================
// routine to get humidity and temperature from humidity sensor
//=====================================================================================
void humidityGetHumidity(void)
{  
  DHT22_ERROR_t errorCode;
  float temp;

  errorCode = dht.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      // get raw readings
      //=================
      temp = dht.getHumidity();
      dataRecord.humidity = temp * 10;      

      temp = dht.getTemperatureC();
      dataRecord.temperatureH = temp * 10;
      break;
    case DHT_ERROR_CHECKSUM:
      DIAGPRINTLN(F("checksum error"));
      break;
    case DHT_BUS_HUNG:
      DIAGPRINTLN(F("BUS Hung"));
      break;
    case DHT_ERROR_NOT_PRESENT:
      DIAGPRINTLN(F("Not Present"));
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      DIAGPRINTLN(F("ACK time out"));
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      DIAGPRINTLN(F("Sync Timeout"));
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      DIAGPRINTLN(F("Data Timeout"));
      break;
    case DHT_ERROR_TOOQUICK:
      DIAGPRINTLN(F("Polled to quick"));
      break;
  }
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

  DIAGPRINT(F("      TempH: "));
  DIAGPRINT(temp, 1);
  DIAGPRINTLN(F(" C"));
  DIAGPRINT(F("   Humidity: "));
  DIAGPRINT(humidity, 1);
  DIAGPRINTLN(F(" %"));
  DIAGPRINT(F("  Dew point: "));
  DIAGPRINT(dewPoint, 1);
  DIAGPRINTLN(F(" C"));
  DIAGPRINT(F("  Cloudbase: "));
  DIAGPRINT(cloudBase);
  DIAGPRINTLN(F(" ft est"));
}


