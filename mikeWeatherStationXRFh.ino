//==============================================================
//  mikeWeatherStation
//  Mike McPherson
//  December 2013
//==============================================================

#define PROGRAMID F("Mike's XRF weather station ")
#define PROGRAMVERSION F("v1.01h")

//=================================
// define GPRS to use GPRS
//=================================
//#define GPRS 1

//=================================
// define XRF to use XRF
//=================================
#define XRF 1

//=================================
//RTC
//=================================
#include <Sodaq_DS3231.h>
extern DateTime timeNow;

//==============================================
// loop control
//
// every .. seconds do wind direction calculation
// every .. seconds write data record to flash
// every .. seconds upload data to server
// wait .. seconds then run sql
//==============================================
#define WIND_INTERVAL      1  // every 1 second
#define RECORD_INTERVAL   15  // 4 times a minute
#define UPLOAD_INTERVAL  300  // every 5 minutes
#define FTP2SQL_DELAY     30  // run after upload finishes

//==============================================
// sensor warm up time in mS
//==============================================
#define WARMUPTIME 2100

//==============================================
// time variables
//==============================================
uint32_t windTime;        // time for next wind calculation
uint32_t recordTime;      // time for next record write
uint32_t uploadTime;      // time for next data upload
uint32_t warmupTime;      // sensor warm-up timer (non-blocking)
uint32_t epochRecord;     // time at start of record processing
uint32_t startTime;       // time when processor last restarted

#ifdef GPRS
                          // only used in GPRS mode
uint32_t ftp2sqlTime;     // time for next ftp2sql command
#endif

char     gWxId[16];
char    gMagic[ 8];

//=================================
// flag to show interrupts was from watchdog
//=================================
boolean wdtFlag;

//=================================
// data record for writing results to flash
// and uploading to server
//=================================
#include "DataRecord.h"
extern myRecord dataRecord;

//=================================
// define XRF print routines
//=================================
#ifdef XRF
#define XRFPRINT(...)          diagport.print(__VA_ARGS__)
#define XRFPRINTLN(...)        diagport.println(__VA_ARGS__)
#define XRFFLUSH(...)          diagport.flush(__VA_ARGS__)
#define xrfport Serial
#else
#define XRFPRINT(...)
#define XRFPRINTLN(...)
#define XRFFLUSH(...)
#define xrfport
#endif

//=================================
// diagnostics/debug support
// define to use software serial
// comment SOFT_SERIAL out to use "real" Serial
//=================================
#include "diag.h"
//#define SOFT_SERIAL 1

#ifdef SOFT_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial diagport(DIAGPORT_RX, DIAGPORT_TX);
#else
#define diagport Serial
#endif

//===========================================
// define DEBUG to generate debug diagnostics
//===========================================
//#define DEBUG 1

//=====================================================================================
// Arduino setup
//=====================================================================================
void setup() 
{
  // start the serial (debug) port
  //==============================
  diagInit();

  // get settings from EEPROM
  //=========================
  eepromReadAll();
  
  // initialize the Grove power pin
  //===============================
  groveInit();

  // set Grove power on
  //===================
  groveOn();

#ifdef GPRS
  // initialise xbee interface
  //==========================
  xbeeInit();
  
  // initialise data flash
  //======================
  dataFlashInit();  
#endif

#ifdef XRF
  // initialise XRF interface
  //=========================
  xrfInit();
#endif

  // annonunce self to world
  //========================
  DIAGPRINTLN();
  showVersion();
  DIAGPRINT(F(" initialise: "));
  
  // initialise RTC
  //===============
  clockInit();
  
  // get time from RTC
  //==================
  dataRecord.ts = getTime();
  startTime = dataRecord.ts;
  
  // initialise humidity
  //====================
  humidityInit();

  // initialise barometer
  //=====================
  barometerInit();

  // initialise wind
  //================
  windInit();

  // initialise rain
  //================
  rainInit();

  // initialise timers
  //==================
  timersInit();
  
  // show timestamp
  //===============
  DIAGPRINTLN();
  DIAGPRINT(F(" start time: "));
  timestampShow(true);
  
  // bad date; power loss has stopped RTC
  //=====================================
  while (startTime < 10000)
  {
    // check if we got a new epoch time command
    //=========================================
    uint32_t x = millis();
    while ( x + 1000 > millis())
    {
      checkCommands();
    }

    if(startTime < 10000)
    {
      // show timestamp
      //===============
      DIAGPRINTLN();
      DIAGPRINT(F(" start time: "));
      timestampShow(true);
    }
  }

  // clear the interrupt flags
  //==========================
  wdtFlag = false;
  warmupTime = 0;

  // show amount of RAM free
  //========================
  freeRAMshow();
  separatorPrint(40);

  // XRF off
  //========
  xrfSleep();

  // set Grove power off
  //====================
  groveOff();

  // ... and sleep
  //==============
  sleep();
}

//=====================================================================================
// main code loop
//=====================================================================================
void loop()
{
  if (wdtFlag)
  {
    // watchdog fired
    //===============
#ifdef DEBUG
    DIAGPRINT('d');
#endif
    wdtFlag = false;
  }
  
  // wake up XRF
  // for diagnostics
  //================
  xrfWake();
  
  // need grove on so RTC works properly
  //====================================
  groveOn();
  
  // allow time for sensors to settle
  //=================================
  delay(10);
  
  // get time from RTC
  //==================
  dataRecord.ts = getTime();
#ifdef DEBUG
  DIAGPRINT('g');
#endif

  if (dataRecord.ts >= windTime)
  { 
    // time to do wind calculation
    //============================
    //timeShow();
    //DIAGPRINTLN(F("wind"));
    windTime = windTime + WIND_INTERVAL;
  }
  
  // check if it's time to create a data record
  //===========================================
  checkDataRecord();
  
#ifdef SOFT_SERIAL
#ifdef GPRS    
  // note next upload time
  //======================
  timeNow = uploadTime;
  DIAGPRINT(F("next upload: ")); timeShow(); DIAGPRINTLN();

  // only upload if SOFT_SERIAL is defined
  // if not we're WiFi connected
  // so t'other end does the uploading
  //======================================
  if (dataRecord.ts >= uploadTime)
  { 
    // time to upload data to the server
    //==================================
    timeShow();
    DIAGPRINTLN(F("upload"));

    dataUpload();
    uploadTime = uploadTime + UPLOAD_INTERVAL;

    // schedule sql after DELAY
    //=========================
    ftp2sqlTime = getTime() + FTP2SQL_DELAY;
  }

  // check ftp2sql timer
  //====================
  if (dataRecord.ts >= ftp2sqlTime && ftp2sqlTime != 0)
  { 
    // time to run ftp2sql on the server
    //==================================
    timeShow();
    DIAGPRINTLN(F("ftp2sql"));

    ftp2sql();

    // stop the clock
    //===============
    ftp2sqlTime = 0;
  }
#endif
#endif

  // see if any commands to process
  //===============================
  checkCommands();

  if (warmupTime == 0)
  {
    // xrf to sleep
    //=============
    xrfSleep();

    // we've finished with the sensor warm up
    //=======================================
    groveOff();

    // so now we can sleep again
    // watchDog timer will wake us up in a bit
    //========================================
    sleep();
  }
}

//=====================================================================================
// is it time for another data record?
//=====================================================================================
void checkDataRecord(void)
{
  // get time from RTC
  //==================
  dataRecord.ts = getTime();
#ifdef DEBUG
  DIAGPRINT('c');
#endif

  if (dataRecord.ts >= recordTime)
  { 
    // time to create a data record
    //=============================
    createRecord();
  }
}

//=====================================================================================
// routine to create a data record in flash
//=====================================================================================
void createRecord(void)
{
  if (warmupTime == 0)
  {
    warmupTime = millis();

    // save the epoch time
    //====================
    epochRecord = dataRecord.ts;

    // wake up XRF
    //============
    xrfWake();
  }
  
  // pause while the sensors warm up
  //================================
  if ( (millis() - warmupTime) > WARMUPTIME)
  {
    // time to write out a record
    //===========================
    showVersion();
    
    // show wx id
    //===========
    DIAGPRINT(F("       wxid: "));
    DIAGPRINTLN(gWxId);
    
    // show when we started
    //=====================
    timeNow = startTime;
    DIAGPRINT(F(" start time: ")); 
    timestampShow(false);
    
    // show time now
    //==============
    DIAGPRINT(F("  Timestamp: ")); 
    timestampShow(true);
    tsShow();
    
    // RTC temperature
    //================
    rtcTempShow();

    // battery voltage
    //================
    batteryGetVoltage();
    batteryShowVoltage();

    // barometer temp and pressure
    //============================
    barometerGetPressure();
    barometerShow();

    // wind speed and direction
    //=========================
    windShow();
    
    // rainfall
    //=========
    rainShow();
  
    // humidity temperature and %age
    // and calcualted cloud base
    //==============================
    humidityGetHumidity();
    humidityShow();

#ifdef GPRS
    // write data record to flash memory
    // but only for GPRS
    //==================================
    dataRecordWriteToFlash(epochRecord);
#endif

    // reset the timers
    //=================
    recordTime = epochRecord + RECORD_INTERVAL;
    
    // show ts for next record
    //========================
    DIAGPRINT(F("  next time: ")); DIAGPRINTLN(recordTime);
    warmupTime = 0;

    separatorPrint(40);
  }
}

//=====================================================================================
// initialise internal timers
//=====================================================================================
void timersInit(void)
{
  uint32_t epoch;
  
  // initialise timers to current clock setting
  //===========================================
  epoch = rtc.now().get();
  windTime = epoch + WIND_INTERVAL;
  recordTime = epoch + RECORD_INTERVAL;
  uploadTime = epoch + UPLOAD_INTERVAL;

#ifdef GPRS
  // only used in GPRS mode
  //=======================
  ftp2sqlTime = 0;
#endif

}


