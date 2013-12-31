//==============================================================
//  mikeWeatherStation
//
//  barometer module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=====================================================================================
// EEPROM
//=====================================================================================
#include <EEPROM.h>

// offsets into EEPROM
//====================

#ifdef GPRS
// data for GPRS
//==============
#define eeAPN        0
#define eeSERVER    32
#define eeUSERNAME  48
#define eePASSWORD  64
#define eeFTPPATH   80
#define eeFTPSQL   128
#define eeTIMEURL  192
#endif

// common data
//============
#define eeWXID      96
#define eeMAGIC    112

#ifdef GPRS
// data for GPRS
//==============
extern char      gAPN[32];
extern char   gServer[16];
extern char gUserName[16];
extern char gPassword[16];
extern char  gFTPpath[16];
extern char   gFtpSql[48];
extern char  gTimeURL[48];
#endif

extern char     gWxId[16];
extern char    gMagic[ 8];

void eepromDump(void)
{
  int row;
  int col;
  byte oneByte;

  // read entire eeprom
  //===================
  for (row=0; row<16; row++)
  {
    Serial.print("0");
    Serial.print(row, HEX);
    Serial.print(":");
    for (col=0; col<16; col++)
    {
      oneByte = EEPROM.read(row*16+col);
      Serial.print(" ");
      if (oneByte < 16)
        Serial.print("0");
      Serial.print(oneByte, HEX);
    }
    Serial.println();
  }
  Serial.println();
}

//=====================================================================================
// read from EEPROM
//=====================================================================================
void eepromRead(char *target, int offset, int size)
{
  int i;

  // read value from address
  //========================
  for (i=0; i<size-1; i++)
    target[i] = EEPROM.read(offset + i);
  
  target[size-1] = '/0';
}

//=====================================================================================
// read settings from EEPROM
//=====================================================================================
void eepromReadAll(void)
{
  // read saved settings from EEPROM
  //================================

#ifdef GPRS
// data for GPRS
//==============
  eepromRead(     &gAPN[0], eeAPN, sizeof(gAPN));
  eepromRead(  &gServer[0], eeSERVER, sizeof(gServer));
  eepromRead(&gUserName[0], eeUSERNAME, sizeof(gUserName));
  eepromRead(&gPassword[0], eePASSWORD, sizeof(gPassword));
  eepromRead( &gFTPpath[0], eeFTPPATH, sizeof(gFTPpath));
  eepromRead(  &gFtpSql[0], eeFTPSQL, sizeof(gFtpSql));
  eepromRead( &gTimeURL[0], eeTIMEURL, sizeof(gTimeURL));
#endif

  eepromRead(    &gWxId[0], eeWXID, sizeof(gWxId));
  eepromRead(   &gMagic[0], eeMAGIC, sizeof(gMagic));
}

void eepromSaveAll(void)
{
  // save all settings
  //==================

#ifdef GPRS
// data for GPRS
//==============
  eepromWrite(     gAPN, eeAPN, sizeof(gAPN));
  eepromWrite(  gServer, eeSERVER, sizeof(gServer));
  eepromWrite(gUserName, eeUSERNAME, sizeof(gUserName));
  eepromWrite(gPassword, eePASSWORD, sizeof(gPassword));
  eepromWrite( gFTPpath, eeFTPPATH, sizeof(gFTPpath));
  eepromWrite(  gFtpSql, eeFTPSQL, sizeof(gFtpSql));
  eepromWrite( gTimeURL, eeTIMEURL, sizeof(gTimeURL));
#endif

  eepromWrite(    gWxId, eeWXID, sizeof(gWxId));
  eepromWrite(   gMagic, eeMAGIC, sizeof(gMagic));
}
void eepromWrite(char *target, int offset, int size)
{
  int i;

  // write value to next location
  //=============================
  for (i=0; i<size; i++)
  {
    // write one byte
    //===============
    EEPROM.write(offset + i, target[i]);
  }
}

void eepromWriteByte(int address, uint8_t data)
{
  EEPROM.write(address, data);
}

