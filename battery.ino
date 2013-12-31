//==============================================================
//  mikeWeatherStation
//
//  battery module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// battery voltage
//=================================
#define BATVOLTPIN A7
#define BATVOLT_R1      10              // voltage divider R1 = 10M
#define BATVOLT_R2      2               // voltage divider R2 = 2M
#define ADC_AREF        3.3
#define MIN_BATTERY_USE_GPRSBEE      3.5    // Below this level do not use GPRSbee
#define MIN_BATTERY_TRY_GPRSBEE      3.9    // Below this level try GPRSbee on and check

#ifdef GPRS
//=====================================================================================
// check battery voltage with xbee powered on
//=====================================================================================
bool checkBatteryOnGPRSbee(void)
{
  float batteryVoltage;

  // check battery before we start
  //==============================
  batteryGetVoltage();
  batteryVoltage = dataRecord.batteryVoltage / 100.0;
  DIAGPRINT(F("check battery (GPRSbee OFF): ")); DIAGPRINTLN(batteryVoltage);
  
  if (batteryVoltage < MIN_BATTERY_USE_GPRSBEE) 
  {
    // Don't even try. We're very low on battery
    //==========================================
    return false;
  }
  
  if (batteryVoltage < MIN_BATTERY_TRY_GPRSBEE) 
  {
    // First switch it on and see if the battery holds
    //================================================
    setGPRSstate(true);
    batteryGetVoltage();
    batteryVoltage = dataRecord.batteryVoltage / 100.0;
    DIAGPRINT(F("check battery (GPRSbee ON): ")); DIAGPRINTLN(batteryVoltage);
    if (batteryVoltage < MIN_BATTERY_USE_GPRSBEE) 
    {
      DIAGPRINT(F("Battery too low: ")); DIAGPRINTLN(batteryVoltage);
      setGPRSstate(false);
      return false;
    }
  }
  
  // Battery is OK
  //==============
  return true;
}
#endif

//=====================================================================================
// routine to get current Battery voltage
//=====================================================================================
void batteryGetVoltage(void)
{
  int adc;
  float batteryVoltage;
  
  //=============================================================
  // This pin is connected to the middle of a 10M and 2M resistor
  // that are between Vcc and GND.
  // So actual battery voltage is:
  //    (<adc value> * Aref / 1023) * (R1+R2) / R2
  //=============================================================

  // read the raw voltage
  // twice to get more accurate reading
  //===================================
  adc = analogRead(BATVOLTPIN);
  adc = analogRead(BATVOLTPIN);
  batteryVoltage = (adc * ADC_AREF / 1023) * (BATVOLT_R1+BATVOLT_R2) / BATVOLT_R2;
  
  // save voltage * 100
  //===================
  dataRecord.batteryVoltage = batteryVoltage * 100;
}

//=====================================================================================
// routine to show current Battery voltage
//=====================================================================================
void batteryShowVoltage(void)
{
  float batteryVoltage;

  batteryVoltage = dataRecord.batteryVoltage / 100.0;

  DIAGPRINT(F("    Battery: "));
  DIAGPRINT(batteryVoltage, 2);
  DIAGPRINTLN(F(" V"));
}


