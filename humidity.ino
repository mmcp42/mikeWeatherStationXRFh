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
//
// define DHT22 or SHT21 to select sensor type
//============================================

//#define DHT22 1
#define SHT21 1

#ifdef DHT22
#include <DHT22.h>
#define DHTPIN     A3
DHT22 dht(DHTPIN);
#endif

#ifdef SHT21
// uses SCL/SDA pins
//==================
#include <SHT2x.h>
#endif

//=====================================================================================
// routine to get humidity and temperature from humidity sensor
//=====================================================================================
void humidityGetHumidity(void)
{  
#ifdef DHT22
  DHT22_ERROR_t errorCode;
#endif

  float temp;

#ifdef DHT22
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
#endif

#ifdef SHT21
  temp = SHT2x.GetHumidity();
  dataRecord.humidity = temp * 10;
  
  temp = SHT2x.GetTemperature();
  dataRecord.temperatureH = temp * 10;

  DIAGPRINT(F("[ SHT21] Humidity(%): "));
  DIAGPRINT(dataRecord.humidity/ 10.0, 1);
  DIAGPRINT(F("    Temperature(C): "));
  DIAGPRINTLN(dataRecord.temperatureH / 10.0, 1);
#endif
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


