//==============================================================
//  mikeWeatherStation
//
//  gprs module
//
//  Mike McPherson
//  December 2013
//==============================================================

//======================================
// entire module depeneds on GPRS #define
//=======================================
#ifdef GPRS

//=================================
// GPRS settings
//=================================
#include <GPRSbee.h>
#define GPRSBEE_PWRPIN  7
#define XBEECTS_PIN     8

char      gAPN[32];
char   gServer[16];
char gUserName[16];
char gPassword[16];
char  gFTPpath[16];
char   gFTPsql[48];
char  gTimeURL[48];

myRecord wxRec;
myHeader wxHdr;

//=====================================================================================
// routine to upload data to server
//=====================================================================================
void dataUpload(void)
{
  char buffer[40];
  
  if (uploadPage == -1)
  {
    // nothing to send
    //================
    gprsbee.off();
    return;
  }
  
  if (uploadPage == curPage)
  {
    // nothing to send
    //================
    gprsbee.off();
    return;
  }
 
  if (!checkBatteryOnGPRSbee()) 
  {
    // battery too low
    //================
    gprsbee.off();
    return;
  } 
  
  // prepare the filename
  //=====================
  strcpy(buffer, gWxId);
  strcat(buffer, ".");
  ultoa(uploadTime, buffer + strlen(buffer), 10);
  strcat(buffer, ".wxdat");
  
  // start FTP session
  //==================
  DIAGPRINTLN(F("openFTP()"));
  if (!gprsbee.openFTP(gAPN, gServer, gUserName, gPassword)) 
  {
    // Failed to open connection
    //==========================
    DIAGPRINTLN(F("openFTP() failed"));
    delay(1000);
    gprsbee.off();
    return;
  }

  // open FTP file
  //==============
  DIAGPRINT(F("openFTPfile(")); DIAGPRINT(buffer); DIAGPRINTLN(")");
  if (!gprsbee.openFTPfile(buffer, gFTPpath)) 
  {
    // Failed to open file on FTP server
    //==================================
    DIAGPRINTLN(F("openFTPfile() failed"));
    gprsbee.off();
    return;
  }

  // start at uploadPage
  //====================
  if (isValidUploadPage(uploadPage))
  {
    // add page header
    //================
    if (addPageHeaderToFTP(uploadPage))
    {
      while (uploadPage != -1)
      {
        // add data records
        //=================
        if (addOnePageToFTP(uploadPage))
        {
        
          // mark the page as empty/sent
          //============================
          dataFlashErase(uploadPage);
          
          // get the next page
          //==================
          uploadPage = getNextPage(uploadPage);
          
          if (!isValidUploadPage(uploadPage))
          {
            // next page is invalid so stop
            //=============================
            uploadPage = -1;
          }
            
          if (uploadPage == curPage)
          {
            // we're still using this page!
            //=============================
            break;
          }
        }
        else
        {
          // something went wrong
          //=====================
          break;
        }
      }
    }
  }

  // close FTP file
  //===============
  DIAGPRINTLN(F("closeFTPfile()"));
  if (!gprsbee.closeFTPfile()) 
  {
    // Failed to close file
    // The file upload may still have succeeded
    //=========================================
    DIAGPRINTLN(F("closeFTPfile failed"));
    gprsbee.off();
    return;
  }

  // send http request to parse any outstanding ftp files
  // don't really care about reply
  //=====================================================
  gprsbee.doHTTPGET(gAPN, gFTPsql, &buffer[0], sizeof(buffer));

  // and power down
  //===============
  gprsbee.off();
}

//=====================================================================================
// add data records from passed page
//=====================================================================================
bool addOnePageToFTP(int page)
{
  // don't send invalid page number
  //===============================
  if (page < 0)
    return false;

  DIAGPRINT(F("uploadOnePage: ")); DIAGPRINTLN(page);
  
  // read records from page
  //=======================
  for (size_t i = 0; i < NR_RECORDS_PER_PAGE; ++i) 
  {
    if (!readPageNthRecord(page, i, &wxRec)) 
      break;

    // send to xbee
    //=============
    if (!gprsbee.sendFTPdata((uint8_t *)&wxRec, sizeof(wxRec))) 
    {
      // An error
      //=========
      DIAGPRINTLN(F("doPageUpload - failed"));
      return false;
    }
  }
  
  // All went well
  //==============
  DIAGPRINTLN(F("doPageUpload - end"));
  return true;
}

//=====================================================================================
// add page header as file header
//=====================================================================================
bool addPageHeaderToFTP(int page)
{
  // don't send invalid page number
  //===============================
  if (page < 0) 
    return false;

  // read page header
  //=================
  DIAGPRINT(F("addPageHeaderToFTP: ")); DIAGPRINTLN(page);
  if (!readPageHeader(page, &wxHdr)) 
  {
    // This is nasty 
    // It's an invalid page header
    //============================
    DIAGPRINTLN(F("addPageHeaderToFTP - readPageHeader failed"));
    return false;
  }
  
  // send to xbee
  //=============
  if (!gprsbee.sendFTPdata((uint8_t *)&wxHdr, sizeof(wxHdr))) 
  {
    // An error!
    //==========
    DIAGPRINTLN(F("addPageHeaderToFTP - failed"));
    return false;
  }

  // All went well
  //==============
  DIAGPRINTLN(F("addPageHeaderToFTP - end"));
  return true;
}

//=====================================================================================
// run ftp2sql php script
//=====================================================================================
void ftp2sql(void)
{
  char buffer[20];
  
  if (checkBatteryOnGPRSbee()) 
  {
    // send http request to parse any outstanding ftp files
    // don't really care about reply
    //=====================================================
    gprsbee.doHTTPGET(gAPN, gFTPsql, &buffer[0], sizeof(buffer));
  } 
  
  // and power down
  //===============
  gprsbee.off();
}

//=====================================================================================
// set GPRS state
//=====================================================================================
void setGPRSstate(bool state)
{
  // turn xbee on/off
  //=================
  if (state)
    gprsbee.on();
  else
    gprsbee.off();
}

//=====================================================================================
// get time from internet and set RTC
//=====================================================================================
uint32_t getEpochFromServer(void)
{
  char buffer[20];
  char *ptr;
  uint32_t newTs;
  
  if (!checkBatteryOnGPRSbee())
    return 0;

  if (gprsbee.doHTTPGET(gAPN, gTimeURL, buffer, sizeof(buffer))) 
  {
    DIAGPRINT(F("HTTP GET: ")); DIAGPRINTLN(buffer);
    
    // skip over "ts="
    //================
    newTs = strtoul(buffer+3, &ptr, 0);
    
    // Tweak the timestamp a little because doHTTPGET took a few second
    // to close the connection after getting the time from the server
    //=================================================================
    newTs += 3;
  }
    
  // finished with radio
  //====================
  gprsbee.off();
  
  // return new epoch value
  //=======================
  return newTs;
}

//=====================================================================================
// initialise xbee interface
//=====================================================================================
void GPRSinit(void)
{
  // xbee uses REAL serial port
  //===========================
  GPRSBEGIN(9600);
  gprsbee.init(radioPort, XBEECTS_PIN, GPRSBEE_PWRPIN);
  gprsbee.setDiag(diagPort);

  // start switched off
  //===================
  gprsbee.off();
}

#endif

