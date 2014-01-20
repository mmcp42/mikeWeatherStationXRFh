//=====================================================================
// commands.ino
//
//  Created on: 25 Jul 2011
//      Author: Mike McPherson
//=====================================================================

// define some characters
//=======================
#define null (char)0
#define CR '\r'
#define LF '\n'

#ifdef GPRS
// data for GPRS
//==============
extern char      gAPN[32];
extern char   gServer[16];
extern char gUserName[16];
extern char gPassword[16];
extern char  gFTPpath[16];
extern char   gFTPsql[48];
extern char  gTimeURL[48];
#endif

static int freeRAM(void);
void listSettings(void);

// command interface when connected to PC
//=======================================
void checkCommands(void)
{
  char charCom;
  char parameter;
  int i;

  charCom = getChar();
  while (charCom != null)
  {
    switch (charCom)
    {
    case '?':
      showVersion();
      showCommands();
      break;
      
    case 'e':
      // EEPROM functions
      // next character says which action
      //====================================
      parameter = skipChars(' ');
      if (parameter==CR || parameter==LF)
      {
        Serial.println(F("eeprom?"));
        break;
      }
      switch (parameter)
      {
        // clear eeprom
        //==================
        case 'c':
        Serial.println("EEEPROM clear");
        for (i=0; i<256; i=i+1)
          eepromWriteByte(i, 0xFF);
        Serial.println(F("EEPROM cleared"));
        break;

        case 'd':
        // dump EEPROM
        //============
        Serial.println("EEEPROM dump");
        eepromDump();
        break;
        
        case 'r':
        // read all from EEPROM
        //=====================
        Serial.println("EEEPROM read settings");
        eepromReadAll();
        Serial.println(F("settings read from EEPROM"));
        listSettings();
        break;

        case 's':
        // saveall to EEPROM
        //====================
        Serial.println("EEEPROM save settings");
        eepromSaveAll();
        Serial.println(F("settings saved to EEPROM"));
        break;
      }
      break;
      
    case 'h':
      // set epoch
      //==========
      setEpoch();
      break;
        
    case 's':
      // list settings
      //==============
      listSettings();
      break;

    // parameter setting
    //==================
    case 'p':
      // set parameter
      // next character says which parameter
      // followed by value to set
      //====================================
      parameter = skipChars(' ');
      if (parameter==CR || parameter==LF)
      {
        Serial.println(F("parameter?"));
        break;
      }

      switch (parameter)
      {
      case 'm':
      // set Magic
      //============
      setString(&gMagic[0], sizeof(gMagic));
      listSettings();
      break;

      case 'w':
      // set WxId
      //=========
      setString(&gWxId[0], sizeof(gWxId));
      listSettings();
      break;

      default:
        Serial.print(parameter);
        Serial.println(F("??"));
        break;
      }
      break;

    case '\r':
      break;

    case '\n':
      break;

    default:
      Serial.print(charCom);
      Serial.println("??");
      break;
    }
    charCom = getChar();
  }
}

void listSettings(void)
{
  // show version to serial monitor
  //===============================
  showVersion();
  
  // list settings
  //==============
  Serial.print(F("       wxID: ")); Serial.println((char*) gWxId);
  Serial.print(F("      magic: ")); Serial.println((char*) gMagic);
//  Serial.print(F("        APN: ")); Serial.println((char*) gAPN);
//  Serial.print(F("     server: ")); Serial.println((char*) gServer);
//  Serial.print(F("   username: ")); Serial.println((char*) gUserName);
//  Serial.print(F("   password: ")); Serial.println((char*) gPassword);
//  Serial.print(F("   FTP path: ")); Serial.println((char*) gFTPpath);
//  Serial.print(F("    FTP sql: ")); Serial.println((char*) gFTPsql);
//  Serial.print(F("   time URL: ")); Serial.println((char*) gTimeURL);
}

void resetFactoryDefaults(void)
{
  // set and save initial values
  //============================
}

void showVersion(void)
{
  Serial.println();
  Serial.print(F("   software: "));
  Serial.println(PROGRAMID);
  Serial.print(F("    version: "));
  Serial.println(PROGRAMVERSION);
  
  freeRAMshow(); 
}

void showCommands(void)
{
  Serial.println(F("ec    = eeprom clear"));
  Serial.println(F("ed    = eeprom hex dump"));
  Serial.println(F("er    = eeprom read all settings"));
  Serial.println(F("es    = eeprom save all settings"));
  Serial.println(F("fc    = flash clear entire chip"));
  Serial.println(F("h xxx = set epoch"));
  Serial.println(F("s     = list settings"));
  Serial.println(F("p m   = set magic header prefix"));
  Serial.println(F("p w   = set wx station id"));
}

