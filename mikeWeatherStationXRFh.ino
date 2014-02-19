//==============================================================
//  mikeWeatherStation
//  Mike McPherson
//  December 2013
//==============================================================

#define PROGRAMID F("Mike's XRF weather station ")
#define PROGRAMVERSION F("v1.01h")

//=================================
// define GPRS to use GPRS
// comment out if not
//=================================
//#define GPRS 1

//=================================
// define XRF to use XRF
// comment out if not
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

uint32_t ftp2sqlTime;     // time for next ftp2sql command

char     gWxId[16];
char    gMagic[ 8];

//===============================
// flags to show interrupt source
//===============================
boolean wdtFlag;
extern boolean windFlag;
extern boolean rainFlag;

//=================================
// data record for writing results to flash
// and uploading to server
//=================================
#include "DataRecord.h"
extern myRecord dataRecord;

//=================================
// diagnostics/debug support
//=================================

//===========================================
// define DEBUG to generate debug diagnostics
//===========================================
//#define DEBUG 1

#ifdef DEBUG
#define diagPort Serial
#define DIAGINIT(...)           diagPort.begin(57600)
#define DIAGPRINT(...)          diagPort.print(__VA_ARGS__)
#define DIAGPRINTLN(...)        diagPort.println(__VA_ARGS__)
#define DIAGFLUSH(...)          diagPort.flush(__VA_ARGS__)
#else
#define DIAGINIT(...)
#define DIAGPRINT(...)
#define DIAGPRINTLN(...)
#define DIAGFLUSH(...)
#endif

// flag to clean up debug output
//==============================
boolean reported = false;

//=================================
// software serial used for radio
//===============================
#include <SoftwareSerial.h>
#define RADIOPORT_TX     4
#define RADIOPORT_RX     5

#ifdef XRF
//=======================================================
// for XRF we simply print direct to the soft serial port
//=======================================================
#define XRFBEGIN(...)          radioPort.begin(__VA_ARGS__)
#define XRFAVAILABLE(...)      radioPort.available(__VA_ARGS__)
#define XRFREAD(...)           radioPort.read(__VA_ARGS__)
#define XRFPRINT(...)          radioPort.print(__VA_ARGS__)
#define XRFPRINTLN(...)        radioPort.println(__VA_ARGS__)
#define XRFFLUSH(...)          radioPort.flush(__VA_ARGS__)
#else
//=======================================================
// else we don't
//=======================================================
#define XRFBEGIN(...)
#define XRFAVAILABLE(...)
#define XRFREAD(...)
#define XRFPRINT(...)
#define XRFPRINTLN(...)
#define XRFFLUSH(...)
#endif

#ifdef GPRS
//=======================================================
// GPRS uses the soft serial port
//=======================================================
#define GPRSBEGIN(...)          radioPort.begin(__VA_ARGS__)
#define GPRSAVAILABLE(...)      radioPort.available(__VA_ARGS__)
#define GPRSREAD(...)           radioPort.read(__VA_ARGS__)
#define GPRSPRINT(...)          radioPort.print(__VA_ARGS__)
#define GPRSPRINTLN(...)        radioPort.println(__VA_ARGS__)
#define GPRSFLUSH(...)          radioPort.flush(__VA_ARGS__)
#else
//=======================================================
// else we don't
//=======================================================
#define GPRSBEGIN(...)
#define GPRSAVAILABLE(...)
#define GPRSREAD(...)
#define GPRSPRINT(...)
#define GPRSPRINTLN(...)
#define GPRSFLUSH(...)
#endif

SoftwareSerial radioPort(RADIOPORT_RX, RADIOPORT_TX);

extern void listSettings(void);

//=====================================================================================
// Arduino setup
//=====================================================================================
void setup() 
{
  // start the serial (debug) port
  //==============================
  DIAGINIT();

  // announce self to world
  //=======================
  DIAGPRINTLN();

    // get settings from EEPROM
    //=========================
    eepromReadAll();
    listSettings();
  
  // initialize the Grove power pin
  //===============================
  groveInit();

  // set Grove power on
  //===================
  groveOn();

#ifdef GPRS
  // initialise gprs interface
  //==========================
  GPRSinit();
  
  // initialise data flash
  //======================
  dataFlashInit();  
#endif

#ifdef XRF
  // initialise XRF interface
  //=========================
  xrfInit();
#endif

  DIAGPRINT(F(" initialise: "));
  
  // initialise RTC
  //===============
  clockInit();
  
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

  // get time from RTC
  //==================
  dataRecord.ts = getTime();
  startTime = dataRecord.ts;
  
  // initialise timers
  //==================
  timersInit();
  
  // show timestamp
  //===============
  DIAGPRINTLN();
  DIAGPRINT(F(" start time: "));
  XRFPRINT(F(" start time: "));
  timestampShow(false, true);
  
  // bad date; power loss has stopped RTC
  //=====================================
  while (startTime < 10000)
  {
    // check if we got a new epoch time command
    //=========================================
    uint32_t x = millis();
    while ( x + 1000 > millis())
    {
      // check commands from radio interface!
      //=====================================
      checkCommands();
    }

    if(startTime < 10000)
    {
      // show timestamp
      //===============
      DIAGPRINTLN();
      DIAGPRINT(F(" start time: "));
      XRFPRINT(F(" start time: "));
      timestampShow(true, true);
    }
  }

  // clear the interrupt flags
  //==========================
  wdtFlag = false;
  warmupTime = 0;

  // show amount of RAM free
  //========================
  freeRAMshow();

#ifdef XRF
  // XRF off
  //========
  xrfSleep();
#endif

  // set Grove power off
  //====================
  groveOff();

  // empty diag buffer
  //==================
  DIAGFLUSH();
  
  // ... and sleep
  //==============
  sleep(true);
}

//=====================================================================================
// main code loop
//=====================================================================================
void loopTest(void)
{
  checkCommands();
}

void loop(void)
{
  while (true)
  {
    //============================================================
    // wind interrupt
    //============================================================
    if (windFlag)
    {
      // just clear the flag
      //====================
      DIAGPRINT('W');
      DIAGFLUSH();
      windFlag = false;
      if (warmupTime == 0)
        sleep(false);
    }
    
    //============================================================
    // rain interrupt
    //============================================================
    if (rainFlag)
    {
      // just clear the flag
      //====================
      DIAGPRINT('R');
      DIAGFLUSH();
      rainFlag = false;
      if (warmupTime == 0)
        sleep(false);
    }

    //============================================================
    // watchdog interrupt
    //============================================================
    if (wdtFlag)
    {
      // real work, so wake up
      //======================
      if (!reported)
        DIAGPRINT('T');
      reported = true;
      break;
    }

    // all done
    // back to sleep
    //==============
    sleep(false);
  }
  
  // we woke due to watchdog timer
  // so may have real work to do
  //==============================

#ifdef XRF
  // wake up XRF
  //============
  xrfWake();
#endif

  // need grove on so RTC works properly
  //====================================
  groveOn();
  
  // allow time for sensors to settle
  //=================================
  delay(5);
  
  // see if any commands to process
  //===============================
  checkCommands();

  // get time from RTC
  //==================
  dataRecord.ts = getTime();
  if (!reported)
    DIAGPRINT('t');

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
  
#ifdef GPRS    
  // note next upload time
  //======================
  timeNow = uploadTime;
  //DIAGPRINT(F("next upload: ")); timeShow(true); DIAGPRINTLN();

  // is it time to upload data?
  //===========================
  if (dataRecord.ts >= uploadTime)
  { 
    // time to upload data to the server
    //==================================
    timeShow(true);
    DIAGPRINTLN(F("upload"));
    dataUpload();
    
    // set time for next upload
    //=========================
    uploadTime = uploadTime + UPLOAD_INTERVAL;

    // schedule sql after DELAY
    //=========================
    ftp2sqlTime = getTime() + FTP2SQL_DELAY;
  }

  // get time from RTC
  //==================
  dataRecord.ts = getTime();

  // check ftp2sql timer
  //====================
  if (dataRecord.ts > ftp2sqlTime && ftp2sqlTime != 0)
  { 
    // time to run ftp2sql on the server
    //==================================
    timeShow(true);
    DIAGPRINTLN(F("ftp2sql"));

    ftp2sql();

    // stop the clock
    //===============
    ftp2sqlTime = 0;
  }
#endif

  // see if any commands to process
  //===============================
  checkCommands();
  
  // flush the diagnostic port
  //==========================
  DIAGFLUSH();
  
  if (warmupTime == 0)
  {
#ifdef XRF
    // xrf to sleep
    //=============
    xrfSleep();
#endif

    // we've finished with the sensor warm up
    //=======================================
    groveOff();

    // so now we can sleep again
    // watchDog timer will wake us up in a bit
    //========================================
    wdtFlag = false;
    reported = false;
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
  uint8_t i;
  
  if (warmupTime == 0)
  {
    warmupTime = millis();

    // save the epoch time
    //====================
    epochRecord = dataRecord.ts;

#ifdef XRF
    // wake up XRF
    //============
    xrfWake();
#endif

  }
  
  // pause while the sensors warm up
  //================================
  if ( (millis() - warmupTime) > WARMUPTIME)
  {
    // time to write out a record
    //===========================
    //showVersion();
    DIAGPRINT('c');
    
    // show wx id
    //===========
    XRFPRINT(F("       wxid: "));
    XRFPRINTLN((char*) gWxId);
    
    // show when we started
    //=====================
    timeNow = startTime;
    XRFPRINT(F(" start time: ")); 
    timestampShow(false, false);
    
    // show time now
    //==============
    XRFPRINT(F("  Timestamp: ")); 
    timestampShow(true, false);
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
    // and calculated cloud base
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
    XRFPRINT(F("  next time: ")); 
    XRFPRINTLN(recordTime);
    warmupTime = 0;

    // separator for readability
    //==========================
    for (i=0; i<33; i++)
      XRFPRINT("=");
    XRFPRINTLN();
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

  // clear "time to call ftp2sql command" time
  //==========================================
  ftp2sqlTime = 0;
}


